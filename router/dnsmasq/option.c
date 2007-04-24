/* dnsmasq is Copyright (c) 2000 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

/* Author's email: simon@thekelleys.org.uk */

#include "dnsmasq.h"

struct myoption {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};

static struct myoption opts[] = { 
  {"version", 0, 0, 'v'},
  {"no-hosts", 0, 0, 'h'},
  {"no-poll", 0, 0, 'n'},
  {"help", 0, 0, 'w'},
  {"no-daemon", 0, 0, 'd'},
  {"log-queries", 0, 0, 'q'},
  {"user", 1, 0, 'u'},
  {"resolv-file", 1, 0, 'r'},
  {"mx-host", 1, 0, 'm'},
  {"mx-target", 1, 0, 't'},
  {"cache-size", 1, 0, 'c'},
  {"port", 1, 0, 'p'},
  {"dhcp-lease", 1, 0, 'l'},
  {"domain-suffix", 1, 0, 's'},
  {"interface", 1, 0, 'i'},
  {"listen-address", 1, 0, 'a'},
  {"bogus-priv", 0, 0, 'b'},
  {"selfmx", 0, 0, 'e'},
  {"filterwin2k", 0, 0, 'f'},
  {"pid-file", 1, 0, 'x'},
  {"strict-order", 0, 0, 'o'},
  {"server", 1, 0, 'S'},
  {"local", 1, 0, 'S' },
  {"address", 1, 0, 'A' },
  {"conf-file", 1, 0, 'C'},
  {"no-resolv", 0, 0, 'R'},
  {"expand-hosts", 0, 0, 'E'},
  {0, 0, 0, 0}
};

struct optflags {
  char c;
  unsigned int flag; 
};

static struct optflags optmap[] = {
  { 'b', OPT_BOGUSPRIV },
  { 'f', OPT_FILTER },
  { 'q', OPT_LOG },
  { 'e', OPT_SELFMX },
  { 'h', OPT_NO_HOSTS },
  { 'n', OPT_NO_POLL },
  { 'd', OPT_DEBUG },
  { 'o', OPT_ORDER },
  { 'R', OPT_NO_RESOLV },
  { 'E', OPT_EXPAND },
  { 'v', 0},
  { 'w', 0},
  { 0, 0 }
};

static char *usage =
"Usage: dnsmasq [options]\n"
"\nValid options are :\n"
"-A  --addr=/domain/ipaddr   Return ipaddr for all hosts in specified domains.\n"
"-a, --listen-address=ipaddr Specify local address(es) to listen on.\n"
"-b, --bogus-priv            Fake reverse lookups for RFC1918 private address ranges.\n"
"-c, --cache-size=cachesize  Specify the size of the cache in entries (defaults to %d).\n"
"-C, --conf-file=path        Specify configuration file (defaults to " CONFFILE ").\n"
"-d, --no-daemon             Do NOT fork into the background: run in debug mode.\n"
"-e, --selfmx                Return self-pointing MX records for local hosts.\n"
"-E, --expand-hosts          Expand simple names in /etc/hosts with domain-suffix.\n"
"-f, --filterwin2k           Don't forward spurious DNS requests from Windows hosts.\n"
"-h, --no-hosts              Do NOT load " HOSTSFILE " file.\n"
"-i, --interface=interface   Specify interface(s) to listen on.\n"
"-l, --dhcp-lease=path       Specify the path to the DHCP lease file.\n"
"-m, --mx-host=host_name     Specify the MX name to reply to.\n"
"-n, --no-poll               Do NOT poll " RESOLVFILE " file, reload only on SIGHUP.\n"
"-o, --strict-order          Use nameservers strictly in the order given in " RESOLVFILE ".\n"
"-p, --port=number           Specify port to listen for DNS requests on (defaults to 53).\n"
"-q, --log-queries           Log queries.\n"
"-R, --no-resolv             Do NOT read resolv.conf.\n"
"-r, --resolv-file=path      Specify path to resolv.conf (defaults to " RESOLVFILE ").\n"
"-S, --server=/domain/ipaddr Specify address(es) of upstream servers with optional domains.\n"
"    --local=/domain/        Never forward queries to specified domains.\n"
"-s, --domain-suffix=domain  Specify the domain suffix which DHCP entries will use.\n"
"-t, --mx-target=host_name   Specify the host in an MX reply.\n"
"-u, --user=username         Change to this user after startup. (defaults to " CHUSER ").\n" 
"-v, --version               Display dnsmasq version.\n"
"-w, --help                  Display this message.\n"
"-x, --pid-file=path         Specify path of PID file. (defaults to " RUNFILE ").\n"
"\n";


unsigned int read_opts (int argc, char **argv, char *buff, struct resolvc **resolv_files, 
			char **mxname, char **mxtarget, char **lease_file, 
			char **username, char **domain_suffix, char **runfile, 
			struct iname **if_names, struct iname **if_addrs,
			struct server **serv_addrs, int *cachesize, int *port)
{
  int option = 0, i;
  unsigned int flags = 0;
  FILE *f = NULL;
  char *conffile = CONFFILE;
  int conffile_set = 0;

  opterr = 0;

  while (1)
    {
      if (!f)
#ifdef HAVE_GETOPT_LONG
	option = getopt_long(argc, argv, "ERowefnbvhdqr:m:p:c:l:s:i:t:u:a:x:S:C:A:", 
			     (struct option *)opts, NULL);
#else
        option = getopt(argc, argv, "ERowefnbvhdqr:m:p:c:l:s:i:t:u:a:x:S:C:A:");
#endif
      else
	{ /* f non-NULL, reading from conffile. */
	  if (!fgets(buff, MAXDNAME, f))
	    {
	      /* At end of file, all done */
	      fclose(f);
	      break;
	    }
	  else
	    {
	      char *p;
	      /* fgets gets end of line char too. */
	      while (strlen(buff) > 0 && 
		     (buff[strlen(buff)-1] == '\n' || 
		      buff[strlen(buff)-1] == ' ' || 
		      buff[strlen(buff)-1] == '\t'))
		buff[strlen(buff)-1] = 0;
	      if (*buff == '#' || *buff == 0)
		continue; /* comment */
	      if ((p=strchr(buff, '=')))
		{
		  optarg = p+1;
		  *p = 0;
		}
	      else
		optarg = NULL;
	      
	      option = 0;
	      for (i=0; opts[i].name; i++) 
		if (strcmp(opts[i].name, buff) == 0)
		  option = opts[i].val;
	      if (!option)
		die("bad option %s", buff);
	    }
	}
      
      if (option == -1)
	{ /* end of command line args, start reading conffile. */
	  if (!conffile)
	    break; /* "confile=" option disables */
	  option = 0;
	  if (!(f = fopen(conffile, "r")))
	    {   
	      if (errno == ENOENT && !conffile_set)
		break; /* No conffile, all done. */
	      else
		die("cannot read %s: %s", conffile);
	    }
	}
     
      if (!f && option == 'w')
	{
	  fprintf (stderr, usage,  CACHESIZ);
	  exit(0);
	}

      if (!f && option == 'v')
        {
          fprintf(stderr, "dnsmasq version %s\n", VERSION);
          exit(0);
        }
      
      for (i=0; optmap[i].c; i++)
	if (option == optmap[i].c)
	  {
	    flags |= optmap[i].flag;
	    option = 0;
	    if (f && optarg)
	      die("extraneous parameter for %s in config file.", buff);
	    break;
	  }
      
      if (option && option != '?')
	{
	  if (f && !optarg)
	    die("missing parameter for %s in config file.", buff);
	  
	  switch (option)
	    { 
	     case 'C': 
	       conffile = safe_string_alloc(optarg);
	       conffile_set = 1;
	       break;
	      
	    case 'x': 
	      *runfile = safe_string_alloc(optarg);
	      break;
	      
	    case 'r':
	      {
		char *name = safe_string_alloc(optarg);
		struct resolvc *new, *list = *resolv_files;
		if (list && list->is_default)
		  {
		    /* replace default resolv file - possibly with nothing */
		    if (name)
		      {
			list->is_default = 0;
			list->name = name;
		      }
		    else
		      list = NULL;
		  }
		else if (name)
		  {
		    new = safe_malloc(sizeof(struct resolvc));
		    new->next = list;
		    new->name = name;
		    new->is_default = 0;
		    new->logged = 0;
		    list = new;
		  }
		*resolv_files = list;
		break;
	      }

	    case 'm':
	      canonicalise(optarg);
	      *mxname = safe_string_alloc(optarg);
	      break;
	      
	    case 't':
	      canonicalise(optarg);
	      *mxtarget = safe_string_alloc(optarg);
	      break;
	      
	    case 'l':
	      *lease_file = safe_string_alloc(optarg);
	      break;
	      
	    case 's':
	      canonicalise(optarg);
	      *domain_suffix = safe_string_alloc(optarg);
	      break;
	      
	    case 'u':
	      *username = safe_string_alloc(optarg);
	      break;
	      
	    case 'i':
	      {
		struct iname *new = safe_malloc(sizeof(struct iname));
		new->next = *if_names;
		*if_names = new;
		new->name = safe_string_alloc(optarg);
		new->found = 0;
		break;
	      }
	      
	    case 'a':
	      {
		struct iname *new = safe_malloc(sizeof(struct iname));
		new->next = *if_addrs;
		*if_addrs = new;
		new->found = 0;
#ifdef HAVE_IPV6
		if (inet_pton(AF_INET, optarg, &new->addr.in.sin_addr))
		  new->addr.sa.sa_family = AF_INET;
		else if (inet_pton(AF_INET6, optarg, &new->addr.in6.sin6_addr))
		  new->addr.sa.sa_family = AF_INET6;
#else
		if ((new->addr.in.sin_addr.s_addr = inet_addr(optarg)) != (in_addr_t)-1)
                    new->addr.sa.sa_family = AF_INET;
#endif
		else
		  option = '?'; /* error */
		break;
	      }
	      
	    case 'S':
	    case 'A':
	      {
		struct server *serv, *newlist = NULL;
		
		if (*optarg == '/')
		  {
		    char *end;
		    optarg++;
		    while ((end = strchr(optarg, '/')))
		      {
			char *domain;
			*end = 0;
			canonicalise(optarg);
			if ((domain = safe_string_alloc(optarg)))
			  {
			    serv = safe_malloc(sizeof(struct server));
			    serv->next = newlist;
			    newlist = serv;
			    serv->from_resolv = 0;
			    serv->domain = domain;
			  }
			optarg = end+1;
		      }
		    if (!newlist)
		      {
			option = '?';
			break;
		      }
		
		  }
		else
		  {
		    newlist = safe_malloc(sizeof(struct server));
		    newlist->next = NULL;
		    newlist->from_resolv = 0;
		    newlist->domain = NULL;
		  }
		
		if (option == 'A')
		  {
		    newlist->literal_address = 1;
		    if (!newlist->domain)
		      {
			option = '?';
		        break;
		      }
		  }
		else
		  newlist->literal_address = 0;
		
		if (!*optarg)
		  {
		    newlist->no_addr = 1; /* no server */
		    if (newlist->literal_address)
		      {
			option = '?';
		        break;
		      }
		  }
		else
		  {
		    int serv_port = NAMESERVER_PORT;
		    char *portno;
		    
		    newlist->no_addr = 0;
		    
		    if ((portno = strchr(optarg, '#'))) /* is there a port no. */
		      {
			*portno = 0;
			serv_port = atoi(portno+1);
		      }
#ifdef HAVE_IPV6
		    if (inet_pton(AF_INET, optarg, &newlist->addr.in.sin_addr))
#else
		    if ((newlist->addr.in.sin_addr.s_addr = inet_addr(optarg)) != (in_addr_t) -1)
#endif
		      {
#ifdef HAVE_SOCKADDR_SA_LEN
			newlist->addr.in.sin_len = sizeof(struct sockaddr_in);
#endif
			newlist->addr.sa.sa_family = AF_INET;
			newlist->addr.in.sin_port = htons(serv_port);
		      }
#ifdef HAVE_IPV6
		    else if (inet_pton(AF_INET6, optarg, &newlist->addr.in6.sin6_addr))
		      {
#ifdef HAVE_SOCKADDR_SA_LEN
			newlist->addr.in6.sin6_len = sizeof(struct sockaddr_in6);
#endif
			newlist->addr.sa.sa_family = AF_INET6;
			newlist->addr.in6.sin6_port = htons(serv_port);
			newlist->addr.in6.sin6_flowinfo = htonl(0);
		      }
#endif
		    else
		      option = '?'; /* error */
		  }
		
		serv = newlist;
		while (serv->next)
		  {
		    serv->next->literal_address = serv->literal_address;
		    serv->next->no_addr = serv->no_addr;
		    serv->next->addr = serv->addr;
		    serv = serv->next;
		  }
		serv->next = *serv_addrs;
		*serv_addrs = newlist;
		break;
		
	      }
	      
	    case 'c':
	      {
		int size = atoi(optarg);
		/* zero is OK, and means no caching. */
		
		if (size < 0)
		  size = 0;
		else if (size > 1000)
		  size = 1000;
		
		*cachesize = size;
		break;
	      }
	      
	    case 'p':
	      *port = atoi(optarg);
	      break;
	      
	    }
	}
      
      if (option == '?')
	{
	  if (f)
	    die("bad argument for option %s", buff);
	  else
	    die("bad command line options: try --help.", NULL);
	}
    }
      
  /* port might no be known when the address is parsed - fill in here */
  if (*if_addrs)
    {  
      struct iname *tmp;
      for(tmp = *if_addrs; tmp; tmp = tmp->next)
	if (tmp->addr.sa.sa_family == AF_INET)
	  {
#ifdef HAVE_SOCKADDR_SA_LEN
	    tmp->addr.in.sin_len = sizeof(struct sockaddr_in);
#endif
	    tmp->addr.in.sin_port = htons(*port);
	  }
#ifdef HAVE_IPV6
	else
	  { 
#ifdef HAVE_SOCKADDR_SA_LEN
	    tmp->addr.in6.sin6_len = sizeof(struct sockaddr_in6);
#endif
	    tmp->addr.in6.sin6_port = htons(*port);
	    tmp->addr.in6.sin6_flowinfo = htonl(0);
	  }
#endif /* IPv6 */
    }
		      
  /* only one of these need be specified: the other defaults to the
     host-name */
  if (*mxname || *mxtarget)
    {
      if (gethostname(buff, MAXDNAME) == -1)
	die("cannot get host-name: %s", NULL);
	      
      if (!*mxname)
	*mxname = safe_string_alloc(buff);
      
      if (!*mxtarget)
	*mxtarget = safe_string_alloc(buff);
    }
  
  if (flags & OPT_NO_RESOLV)
    *resolv_files = 0;
  else if (*resolv_files && (*resolv_files)->next && (flags & OPT_NO_POLL))
    die("only one resolv.conf file allowed in no-poll mode.", NULL);
  
  return flags;
}
      
      

