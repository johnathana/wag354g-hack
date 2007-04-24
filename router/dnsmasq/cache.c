/* dnsmasq is Copyright (c) 2000 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

#include "dnsmasq.h"

static struct crec *cache_head, *cache_tail;
static int cache_inserted, cache_live_freed;
static union bigname *big_free;
static int bignames_left, log_queries, cache_size;

static void cache_free(struct crec *crecp);
static void cache_unlink (struct crec *crecp);

void cache_init(int size, int logq)
{
  struct crec *crecp;
  int i;

  log_queries = logq;
  cache_head = cache_tail = NULL;
  cache_size = size;
  big_free = NULL;
  bignames_left = size/10;

  cache_inserted = cache_live_freed = 0;

  if (cache_size > 0)
    {
      crecp = safe_malloc(size*sizeof(struct crec));
      
      for (i=0; i<size; i++, crecp++)
	{
	  cache_link(crecp);
	  crecp->flags = 0;
	}
    }
}

/* Note that it's OK to free slots with F_DHCP set */
/* They just float around unused until the new dhcp.leases load */
static void cache_free(struct crec *crecp)
{
  cache_unlink(crecp);
  crecp->flags &= ~F_FORWARD;
  crecp->flags &= ~F_REVERSE;
  cache_tail->next = crecp;
  crecp->prev = cache_tail;
  crecp->next = NULL;
  cache_tail = crecp;
  /* retrieve big name for further use. */
  if (crecp->flags & F_BIGNAME)
    {
      crecp->name.bname->next = big_free;
      big_free = crecp->name.bname;
      crecp->flags &= ~F_BIGNAME;
    }
}

/* insert a new cache entry at the head of the list (youngest entry) */
void cache_link(struct crec *crecp)
{
  if (cache_head) /* check needed for init code */
    cache_head->prev = crecp;
  crecp->next = cache_head;
  crecp->prev = NULL;
  cache_head = crecp;
  if (!cache_tail)
    cache_tail = crecp;
}

/* remove an arbitrary cache entry for promotion */ 
static void cache_unlink (struct crec *crecp)
{
  if (crecp->prev)
    crecp->prev->next = crecp->next;
  else
    cache_head = crecp->next;

  if (crecp->next)
    crecp->next->prev = crecp->prev;
  else
    cache_tail = crecp->prev;
}

/* before starting an insertion, mark all existing entries as old, 
   and candidates for replacement */
void cache_start_insert(void)
{
  struct crec *crecp;
  
  for (crecp = cache_head; crecp; crecp = crecp->next)
    crecp->flags &= ~F_NEW;
}

/* after end of insertion, if any insertions failed (due to no memory)
   back out the whole lot */
void cache_end_insert(int failed)
{
  struct crec *crecp = cache_head;

  if (failed)
    while (crecp)
      { 
	struct crec *tmp = crecp->next;
	if (crecp->flags & F_NEW)
	  {
	    cache_free(crecp);
	    crecp->flags &= ~F_NEW;
	  }
	crecp = tmp;
      }
}

char *cache_get_name(struct crec *crecp)
{
  return (crecp->flags & F_BIGNAME) ? crecp->name.bname->name : crecp->name.sname;
}

void cache_insert(char *name, struct all_addr *addr, time_t now, 
		  unsigned long ttl, int flags, int *fail)
{
#ifdef HAVE_IPV6
  int addrlen = (flags & F_IPV6) ? IN6ADDRSZ : INADDRSZ;
#else
  int addrlen = INADDRSZ;
#endif
  struct crec *new, *crecp = cache_head;
  union bigname *big_name = NULL;
  int auxflags = flags & (F_NEG | F_NXDOMAIN);

  log_query(flags | F_UPSTREAM, name, addr);
  
  /* strip extra info out of flags arg (already saved in auxflags) */ 
  flags &= F_REVERSE | F_FORWARD | F_IPV6 | F_IPV4;
  
    /* if previous insertion failed give up now. */
  if (*fail || cache_size == 0)
    return;

  /* first remove old entries for the same name or address and 
     protocol and any expired entries */
  /* flags arg is F_FORWARD or F_REVERSE and F_IPV4 or F_IPV6 */
  while (crecp)
    {
      struct crec *tmp = crecp->next;
      /* Note that cache entries from /etc/hosts can have both F_FORWARD and
	 F_REVERSE set. Since we don't reap those here, it's no problem. */
      if (!(crecp->flags & (F_HOSTS | F_DHCP)))
	{
	  /* remove expired entries. */
	  if ((crecp->flags & (F_FORWARD | F_REVERSE)) &&
	      (crecp->ttd < now) && !(crecp->flags & F_IMMORTAL))
	    cache_free(crecp);
	  
	  else if (!(crecp->flags & F_NEW) &&
		   (crecp->flags & (F_REVERSE | F_FORWARD | F_IPV6 | F_IPV4)) == flags)
	    {
	      if ((flags & F_REVERSE) && 
		  memcmp(&crecp->addr, addr, addrlen) == 0)
		cache_free(crecp);
	      
	      else if ((flags & F_FORWARD) &&
		       strcmp(cache_get_name(crecp), name) == 0)
		cache_free(crecp);
	    }
       	}
      crecp = tmp;
    }
  
  /* Now get a cache entry from the end of the LRU list */
  do {
    new = cache_tail;
     /* if we find a new entry there are not enough freeable entries
	in the cache so bail out. !new catches completely empty list,
	new == tmp catches no ordinary entries, just HOSTS and DHCP. */
    if (new->flags & F_NEW)
      {
	*fail = 1;
	return;
      }
    
    /* just push non-vanila entries back to the top and try again. */
    if (new->flags & (F_HOSTS | F_DHCP))
      {
	cache_tail = cache_tail->prev;
	cache_tail->next = NULL;
	cache_link(new);
	new = NULL;
      }
    else
      {
	/* Check if we need to and can allocate extra memory for a long name.
	   If that fails, give up now. */
	if (strlen(name) > SMALLDNAME-1)
	  {
	    if (big_free)
	      { 
		big_name = big_free;
		big_free = big_free->next;
	      }
	    else if (!bignames_left ||
		     !(big_name = (union bigname *)malloc(sizeof(union bigname))))
	      {
		*fail = 1;
		return;
	      }
	    else
	      bignames_left--;
	    
	  }
	/* Got the rest: finally grab entry. */
	cache_tail = cache_tail->prev;
	cache_tail->next = NULL;
      }    
  } while (!new);

  /* The next bit ensures that if there is more than one entry
     for a name or address, they all get removed at once */
  if (new->flags & (F_FORWARD | F_REVERSE))
    { 
      int newflags = new->flags & (F_REVERSE | F_FORWARD | F_IPV6 | F_IPV4);
#ifdef HAVE_IPV6
      int newaddrlen = (newflags & F_IPV6) ? IN6ADDRSZ : INADDRSZ;
#else
      int newaddrlen = INADDRSZ;
#endif
      /* record still-live cache entries we have to blow away */
      cache_live_freed++;
    
      crecp = cache_head;
      while (crecp)
	{
	  struct crec *tmp = crecp->next;
	  if (!(crecp->flags & (F_HOSTS | F_DHCP)))
	    {
	      
	      if (!(crecp->flags & F_NEW) &&
		  (crecp->flags & (F_REVERSE | F_FORWARD | F_IPV6 | F_IPV4)) == newflags)
		{
		  if ((newflags & F_REVERSE) && 
		      memcmp(&crecp->addr, &new->addr, newaddrlen) == 0)
		    cache_free(crecp);
		  
		  if ((newflags & F_FORWARD) &&
		      strcmp(cache_get_name(crecp), cache_get_name(new)) == 0)
		    cache_free(crecp);
		}
	    }
	  crecp = tmp;
	}
    }
  
    
  new->flags = F_NEW | flags;
  if (big_name)
    {
      new->name.bname = big_name;
      new->flags |= F_BIGNAME;
    }
  strcpy(cache_get_name(new), name);
  if (addr)
    memcpy(&new->addr, addr, addrlen);

  new->flags |= auxflags;
  new->ttd = ttl + now;
  cache_link(new);
  cache_inserted++;
}

struct crec *cache_find_by_name(struct crec *crecp, char *name, time_t now, int prot)
{
  if (crecp) /* iterating */
    {
      if (crecp->next && 
	  (crecp->next->flags & F_FORWARD) && 
	  (crecp->next->flags & prot) &&
	  strcmp(cache_get_name(crecp->next), name) == 0)
	return crecp->next;
      else
	return NULL;
    }
  
  /* first search, look for relevant entries and push to top of list
     also free anything which has expired */
  
  crecp = cache_head;
  while (crecp)
    {
      struct crec *tmp = crecp->next;
      if ((crecp->flags & F_FORWARD) && 
	  (crecp->flags & prot) &&
	  (strcmp(cache_get_name(crecp), name) == 0))
	{
	  if ((crecp->flags & F_IMMORTAL) || crecp->ttd > now)
	    {
	      cache_unlink(crecp);
	      cache_link(crecp);
	    }
	  else
	    cache_free(crecp);
	}
      crecp = tmp;
    }

  /* if there's anything relevant, it will be at the head of the cache now. */

  if (cache_head && 
      (cache_head->flags & F_FORWARD) &&
      (cache_head->flags & prot) &&
      (strcmp(cache_get_name(cache_head), name) == 0))
    return cache_head;
  
  return NULL;
}

struct crec *cache_find_by_addr(struct crec *crecp, struct all_addr *addr, 
				time_t now, int prot)
{
#ifdef HAVE_IPV6
  int addrlen = (prot == F_IPV6) ? IN6ADDRSZ : INADDRSZ;
#else
  int addrlen = INADDRSZ;
#endif
  
  if (crecp) /* iterating */
    {
      if (crecp->next && 
	  (crecp->next->flags & F_REVERSE) && 
	  (crecp->next->flags & prot) &&
	  memcmp(&crecp->next->addr, addr, addrlen) == 0)
	return crecp->next;
      else
	return NULL;
    }
  
  /* first search, look for relevant entries and push to top of list
     also free anything which has expired */
  
  crecp = cache_head;
  while (crecp)
    {
      struct crec *tmp = crecp->next;
      if ((crecp->flags & F_REVERSE) && 
	  (crecp->flags & prot) &&
	  memcmp(&crecp->addr, addr, addrlen) == 0)
	{	    
	  if ((crecp->flags & F_IMMORTAL) || crecp->ttd > now)
	    {
	      cache_unlink(crecp);
	      cache_link(crecp);
	    }
	  else
	    cache_free(crecp);
	}
      crecp = tmp;
    }

  /* if there's anything relevant, it will be at the head of the cache now. */

  if (cache_head && 
      (cache_head->flags & F_REVERSE) &&
      (cache_head->flags & prot) &&
      memcmp(&cache_head->addr, addr, addrlen) == 0)
    return cache_head;
  
  return NULL;
}

void cache_reload(int opts, char *buff, char *domain_suffix)
{
  struct crec *cache, *tmp;
  FILE *f;
  char *line;
  
  for (cache=cache_head; cache; cache=tmp)
    {
      tmp = cache->next;
      if (cache->flags & F_HOSTS)
	{
	  cache_unlink(cache);
	  free(cache);
	}
      else if (!(cache->flags & F_DHCP))
	{
	  if (cache->flags & F_BIGNAME)
	    {
	      cache->name.bname->next = big_free;
	      big_free = cache->name.bname;
	     }
	  cache->flags = 0;
	}
    }

  if (opts & OPT_NO_HOSTS)
    {
      if (cache_size > 0)
	syslog(LOG_INFO, "cleared cache");
      return;
    }
  
  f = fopen(HOSTSFILE, "r");
  
  if (!f)
    {
      syslog(LOG_ERR, "failed to load names from %s: %m", HOSTSFILE);
      return;
    }
  
  syslog(LOG_INFO, "reading %s", HOSTSFILE);
  
  while ((line = fgets(buff, MAXDNAME, f)))
    {
      struct all_addr addr;
      char *token = strtok(line, " \t\n\r");
      int addrlen, flags;
          
      if (!token || (*token == '#')) 
	continue;

#ifdef HAVE_IPV6      
      if (inet_pton(AF_INET, token, &addr) == 1)
	{
	  flags = F_HOSTS | F_IMMORTAL | F_FORWARD | F_REVERSE | F_IPV4;
	  addrlen = INADDRSZ;
	}
      else if(inet_pton(AF_INET6, token, &addr) == 1)
	{
	  flags = F_HOSTS | F_IMMORTAL | F_FORWARD | F_REVERSE | F_IPV6;
	  addrlen = IN6ADDRSZ;
	}
#else 
     if ((addr.addr.addr4.s_addr = inet_addr(token)) != (in_addr_t) -1)
        {
          flags = F_HOSTS | F_IMMORTAL | F_FORWARD | F_REVERSE | F_IPV4;
          addrlen = INADDRSZ;
	}
#endif
      else
	continue;

      while ((token = strtok(NULL, " \t\n")) && (*token != '#'))
	{
	  canonicalise(token);
	  /* If set, add a version of the name with a default domain appended */
	  if ((opts & OPT_EXPAND) && domain_suffix && !strchr(token, '.') && 
	      (cache = malloc(sizeof(struct crec) + 
			      strlen(token)+2+strlen(domain_suffix)-SMALLDNAME)))
	    {
	      strcpy(cache->name.sname, token);
	      strcat(cache->name.sname, ".");
	      strcat(cache->name.sname, domain_suffix);
	      cache->flags = flags;
	      memcpy(&cache->addr, &addr, addrlen);
	      cache_link(cache);
	      /* Only the first name is canonical, and should be 
		 returned to reverse queries */
	      flags &=  ~F_REVERSE;
	    }
	  if ((cache = malloc(sizeof(struct crec) + strlen(token)+1-SMALLDNAME)))
	    {
	      strcpy(cache->name.sname, token);
	      cache->flags = flags;
	      memcpy(&cache->addr, &addr, addrlen);
	      cache_link(cache);
	      /* Clear this here in case not done above. */
	      flags &=  ~F_REVERSE;
	    }
	}
    }

  fclose(f);
}
	    

struct crec *cache_clear_dhcp(void)
{
  struct crec *cache = cache_head, *ret = NULL;
  
  while (cache)
    {
      struct crec *tmp = cache->next;
      if (cache->flags & F_DHCP)
	{
	  cache_unlink(cache);
	  cache->next = ret;
	  ret = cache;
	}
      cache = tmp;
    }
  return ret;
}

void dump_cache(int debug, int cache_size)
{
  syslog(LOG_INFO, "Cache size %d, %d/%d cache insertions re-used unexpired cache entries.", 
	 cache_size, cache_live_freed, cache_inserted); 
  if (debug)
    {
      struct crec *cache ;
      char addrbuff[ADDRSTRLEN];
      syslog(LOG_DEBUG, "Host                                     Address                        Flags    Expires\n");
      
      for(cache = cache_head ; cache ; cache = cache->next)
	if (cache->flags & (F_FORWARD | F_REVERSE))
	  {
	    if (cache->flags & F_NEG)
	      addrbuff[0] = 0;
#ifdef HAVE_IPV6
	    else if (cache->flags & F_IPV4)
	      inet_ntop(AF_INET, &cache->addr, addrbuff, ADDRSTRLEN);
	    else if (cache->flags & F_IPV6)
	      inet_ntop(AF_INET6, &cache->addr, addrbuff, ADDRSTRLEN);
#else
            else 
                strcpy(addrbuff, inet_ntoa(cache->addr.addr.addr4));
#endif
	    syslog(LOG_DEBUG, "%-40.40s %-30.30s %s%s%s%s%s%s%s%s%s  %s",
		   cache_get_name(cache), addrbuff,
		   cache->flags & F_IPV4 ? "4" : "",
		   cache->flags & F_IPV6 ? "6" : "",
		   cache->flags & F_FORWARD ? "F" : " ",
		   cache->flags & F_REVERSE ? "R" : " ",
		   cache->flags & F_IMMORTAL ? "I" : " ",
		   cache->flags & F_DHCP ? "D" : " ",
		   cache->flags & F_NEG ? "N" : " ",
		   cache->flags & F_NXDOMAIN ? "X" : " ",
		   cache->flags & F_HOSTS ? "H" : " ",
		   cache->flags & F_IMMORTAL ? "\n" : ctime(&(cache->ttd))) ;
	  }
    }
}


void log_query(int flags, char *name, struct all_addr *addr)
{
  char *source;
  char *verb = "is";
  char addrbuff[ADDRSTRLEN];
  
  if (!log_queries)
    return;
  
  if (flags & F_NEG)
    {
      if (flags & F_NXDOMAIN)
	strcpy(addrbuff, "<NXDOMAIN>-");
      else
	strcpy(addrbuff, "<NODATA>-");
      
      if (flags & F_IPV4)
	strcat(addrbuff, "IPv4");
      else
	strcat(addrbuff, "IPv6");
    }
  else
#ifdef HAVE_IPV6
    inet_ntop(flags & F_IPV4 ? AF_INET : AF_INET6,
	      addr, addrbuff, ADDRSTRLEN);
#else
    strcpy(addrbuff, inet_ntoa(addr->addr.addr4));  
#endif
  
  if (flags & F_DHCP)
    source = "DHCP";
  else if (flags & F_HOSTS)
    source = HOSTSFILE;
  else if (flags & F_UPSTREAM)
    source = "reply";
  else if (flags & F_CONFIG)
    source = "config";
  else if (flags & F_SERVER)
    {
      source = "forwarded";
      verb = "to";
    }
  else if (flags & F_QUERY)
    {
      source = "query";
      verb = "from";
    }
  else
    source = "cached";
  
  if (flags & F_FORWARD)
    syslog(LOG_INFO, "%s %s %s %s", source, name, verb, addrbuff);
  else if (flags & F_REVERSE)
    syslog(LOG_INFO, "%s %s is %s", source, addrbuff, name);
}

