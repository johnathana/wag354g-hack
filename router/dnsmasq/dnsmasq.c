/* dnsmasq is Copyright (c) 2000 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

/* See RFC1035 for details of the protocol this code talks. */

/* Author's email: simon@thekelleys.org.uk */

#include "dnsmasq.h"
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
#include <bcmnvram.h>
#include <shutils.h>
#include <code_pattern.h>
#include <cy_conf.h>
#endif
static int sighup, sigusr1;

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
char pdns[16], sdns[16];
char domainname[64];
#endif

extern struct frec *ftab;
#define WAIT_TIMEOUT 5
//junzhao 2004.5.31
void check_servers_response()
{
	time_t now = time(NULL);
	int i;
	for(i=0; i<FTABSIZ; i++)
	{
		struct frec *f = &ftab[i];
		
		if((f->new_id != 0) && (now - (f->time) >= WAIT_TIMEOUT))
		{
			char packet[sizeof(HEADER) + 1];
			HEADER *header = (HEADER *)packet;
			memset(packet, 0, sizeof(packet));
 			/* could not send on, return empty answer */
      			header->id = htons(f->orig_id);
 			header->qr = 1; /* response */
			header->aa = 0; /* authoritive - never */
			header->ra = 1; /* recursion if available */
			header->tc = 0; /* not truncated */
			header->rcode = NOERROR; /* no error */
			header->ancount = htons(0); /* no answers */
			header->nscount = htons(0);
			header->arcount = htons(0);
	  		sendto(f->fd, packet, sizeof(packet), 0, 
		 		&f->source.sa, sa_len(&f->source));
	  		f->new_id = 0; /* cancel */
		}
	}
	return;
}

static void sig_handler(int sig)
{
  if (sig == SIGHUP)
    sighup = 1;
  else if (sig == SIGUSR1)
    sigusr1 = 1;
}
//wwzh add for DNS ENTRY 

#ifdef DNS_ENTRY_SUPPORT

#include <signal.h>

struct dns_entry{
	unsigned char domainname[254];
	//unsigned char mac[6];
	unsigned int ipaddr;
};

struct lease_t {
	unsigned char chaddr[16];
	unsigned int yiaddr;
	unsigned int expires;
	char hostname[64];
};

#define 		INTERVAL		120
#define 		MAX_ENTRY		254
#define 		MAC_LEN		6	
#define 		DHCP_LEASE_FILE	"/tmp/udhcpd.leases"
#define		DNS_ENTRY_FILE	"/tmp/.dns_entry"

struct dns_entry dns_entry_list[MAX_ENTRY];
static  int dns_entry_count;

int
ether_atoe(const char *a, unsigned char *e)
{
	char *c = (char *) a;
	int i = 0;

	memset(e, 0, MAC_LEN);
	for (;;) {
		e[i++] = (unsigned char) strtoul(c, &c, 16);
		if (!*c++ || i == MAC_LEN)
			break;
	}
	return (i == MAC_LEN);
}
char *
ether_etoa(const unsigned char *e, char *a)
{
	char *c = a;
	int i;

	for (i = 0; i < MAC_LEN; i++) {
		//if (i)
		//	*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}
int add_dns_entry(char *name, /*unsigned char *mac, */unsigned int ipaddr)
{
	int i;
	
	for (i = 0; i < MAX_ENTRY;  i++)
	{
		if ((dns_entry_list[i].ipaddr == 0))
		{
			break;
		}
		if (dns_entry_list[i].ipaddr == ipaddr)
		{
			break;
		}
	}
	if (i == MAX_ENTRY)
		return -1;
	
	dns_entry_list[i].ipaddr = ipaddr;
	//memcpy(dns_entry_list[i].mac, mac, MAC_LEN);
	strcpy(dns_entry_list[i].domainname, name);
	return 0;
}

/* set sock to nonblock */
void set_nonblocking(int fd)
{
	int opts;
	
	opts = fcntl(fd, F_GETFL);
	if (opts < 0)
	{
		perror("CPE fcntl Get");
		return;
	}
	opts |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, opts) < 0)
	{
		perror("CPE fcntl Set");
		return;
	}
	return;
}

/* fail -1 is returned, success zero is returned */

int connect_dns_server(char *ipaddr, int port)
{
	int ret;
	struct sockaddr_in serveraddr;
	int fd;
	int max;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("Error: create download socket failed\n");
		return -1;
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	
	set_nonblocking(fd);
	ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret == -1)
	{
		printf("Error: download connect failed\n");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


static void init_dns_entry(int signo)
{
	char var[510];
	char *next;
	char dns_list[65535];
	FILE *file;
	char buff[511];
	char *ptr;
	char mac[20], name[126], ipaddr[20];
	unsigned char mac_addr[MAC_LEN];
	unsigned int ip;
	struct lease_t lease;
	struct  in_addr in;
	unsigned int delete_entry[MAX_ENTRY];
	int count = 0;
	int del_flag = 0;
	int i;
	
	file = fopen("/tmp/.delete_dns_entry", "r");
	if (file != NULL)
	{
		i = 0;
		while(fgets(ipaddr, sizeof(ipaddr), file))
		{
			delete_entry[i] = inet_addr(ipaddr);
			printf("delete = %s\n", ipaddr);
			i++;
			memset((void *)ipaddr, 0, sizeof(ipaddr));
		}
		count = i;
		fclose(file);
	}

	memset((void *)&dns_entry_list, 0, sizeof(dns_entry_list));
	ip = inet_addr(nvram_get("lan_ipaddr"));
	add_dns_entry("dsldevice", ip);
	
	file = fopen(DHCP_LEASE_FILE, "r");
	if (file != NULL)
	{
		while(fread(&lease, sizeof(lease), 1, file))
		{
			if (!strcmp(lease.hostname, ""))
			{
				ether_etoa(lease.chaddr, mac);
				strcpy(lease.hostname, "unknow");
				strcat(lease.hostname, mac);
			}
			del_flag = 0;
			for (i = 0; i < count; i++)
			{
				if (lease.yiaddr == delete_entry[i])
				{
					del_flag = 1;
					break;
				}
			}
			if (del_flag == 0)
				add_dns_entry(lease.hostname, /*lease.chaddr, */lease.yiaddr);
		}
		fclose(file);
	}
	memset((void *)dns_list, 0, sizeof(dns_list));
	strcpy(dns_list, nvram_get("dns_entry_list"));

	foreach(var, dns_list, next)
	{
		memset((void *)name, 0, sizeof(name));
		//memset((void *)mac, 0, sizeof(mac));
		memset((void *)ipaddr, 0, sizeof(ipaddr));
		ptr = strtok(var, "#");
		if (ptr == NULL)
			continue;
		strcpy(name, ptr);
		
		ptr = strtok(NULL, "#");
		if (ptr == NULL)
			continue;
		strcpy(ipaddr, ptr);
		ip = inet_addr(ipaddr);
		//ether_atoe(mac, mac_addr);
		add_dns_entry(name, /*mac_addr,*/ ip);
	}

	file = fopen(DNS_ENTRY_FILE, "w");
	if (file != NULL)
	{
		for (i = 0; i < MAX_ENTRY; i ++)
		{
			if (dns_entry_list[i].ipaddr == 0)
				break;
		printf("name = %s     ipaddr = %x\n", 
				dns_entry_list[i].domainname,
			 	dns_entry_list[i].ipaddr);
		
			memset((void *)buff, 0, sizeof(buff));
			memset((void *)ipaddr, 0, sizeof(ipaddr));
			
		 	in.s_addr = dns_entry_list[i].ipaddr;
			strcpy(ipaddr, inet_ntoa(in));
			sprintf(buff, "%s=%s\n",dns_entry_list[i].domainname, 
				ipaddr);
			fputs(buff, file);
		}
		fclose(file);
	}
	
	alarm(INTERVAL);
	
		//wwzh add for los communication
{
	int ii;
	char dnsipaddr[20];
	file = fopen("/etc/resolv.conf", "r");
	if (file == NULL)
		return;
	memset((void *)buff, 0, sizeof(buff));
	while(fgets(buff, sizeof(buff), file))
	{
		ptr = strchr(buff, '\n');
		if (ptr != NULL)
			*ptr = '\0';
		ptr = strtok(buff, " ");
		if (strcmp(ptr, "nameserver"))
			continue;
		ptr = strtok(NULL, " ");
		if (ptr == NULL)
			continue;
		if (!strcmp(ptr, "1.1.1.1"))
			continue;
		strcpy(dnsipaddr, ptr);
		ii = connect_dns_server(dnsipaddr, 53);
	printf("CONNECT DNS SERVER = %s\n", dnsipaddr);
	printf("iiiiiii = %d\n", ii);
	
		if (ii == -1)
		{
			syslog(LOG_INFO, "using nameserver %s loss communication", dnsipaddr);
		}
		else
		{
			syslog(LOG_INFO, "using nameserver %s  communicate successfully", dnsipaddr);
		}
	}
	fclose(file);
}
	
}
//wwzh add for DNS entries return great zero show find successfully or failed
int answer_request_from_entry(HEADER *header, int qlen)
{
	
	unsigned short question_count;
	unsigned short name_len;
	char name_buff[54];
	int total_name_len;
	int answer_len;
	char *ptr;
	unsigned int ttl;
	unsigned int  dns_ip = 0;
	int i;
	
	question_count = ntohs(header->qdcount);

	ptr = header;
	ptr += sizeof(HEADER);
	while(question_count)
	{	
		total_name_len = 0;
		name_len = *ptr;
		while(name_len)
		{
			memcpy(&name_buff[total_name_len], (ptr + 1), name_len);
			
			total_name_len += name_len;
			ptr = ptr + 1 + name_len;
			name_len = *ptr;
			if (name_len != 0)
			{
				memcpy(&name_buff[total_name_len], ".", 1);
				total_name_len += 1;
			}
				
		}
		name_buff[total_name_len] = '\0';
			
		ptr += 1;
		question_count--;
			
	}

	for (i = 0; i < MAX_ENTRY; i++)
	{
		if (!strcasecmp(dns_entry_list[i].domainname, name_buff))
		{
			dns_ip = dns_entry_list[i].ipaddr;
			break;
		}
			
	}
	if (dns_ip == 0)
		return -1;
	
	header->qr = 1;
	header->ancount = htons(1);
	ptr += 4; //query type and type

	*ptr = 0xc0;
	ptr += 1;
	*ptr = 0x0c;
	ptr += 1;

	   //  PUTSHORT(nameoffset | 0xc000, ansp); 
	PUTSHORT(T_A, ptr);
	PUTSHORT(C_IN, ptr);
	ttl = 3600;
	PUTLONG(ttl, ptr); /* TTL */
		      
	PUTSHORT(INADDRSZ, ptr);
	
	memcpy(ptr, &dns_ip, INADDRSZ);
	ptr += INADDRSZ;
		
	answer_len = ptr - (char *)header;

	return answer_len;
}

#endif

int main (int argc, char **argv)
{
  int i;
  int cachesize = CACHESIZ;
  int port = NAMESERVER_PORT;
  unsigned int options;
  int first_loop = 1;
#ifdef HAVE_FILE_SYSTEM
  int logged_resolv = 0, logged_lease = 0; 
  char *resolv = RESOLVFILE;
  char *runfile = RUNFILE;
  time_t resolv_changed = 0;
  char *lease_file = NULL;
  off_t lease_file_size = (off_t)0;
  ino_t lease_file_inode = (ino_t)0;
#endif
  struct irec *iface;
  int peerfd, peerfd6;
  struct irec *interfaces = NULL;
  char *mxname = NULL;
  char *mxtarget = NULL;
  char *domain_suffix = NULL;
  char *username = CHUSER;
  struct iname *if_names = NULL;
  struct iname *if_addrs = NULL;
  struct server *serv_addrs = NULL;
  char *dnamebuff, *packet;
  struct server *servers, *last_server;
 
  sighup = 1; /* init cache the first time through */
  sigusr1 = 0; /* but don't dump */
  signal(SIGUSR1, sig_handler);
  signal(SIGHUP, sig_handler);

  //wwzh add for DNS ENTRY
#ifdef DNS_ENTRY_SUPPORT
	signal(SIGALRM, init_dns_entry);
	
	kill(getpid(), SIGALRM);
#endif

  /* These get allocated here to avoid overflowing the small stack
     on embedded systems. dnamebuff is big enough to hold one
     maximal sixed domain name and gets passed into all the processing
     code. We manage to get away with one buffer. */
  dnamebuff = safe_malloc(MAXDNAME);
  /* Size: we check after adding each record, so there must be 
     memory for the largest packet, and the largest record */
  packet = safe_malloc(PACKETSZ+MAXDNAME+RRFIXEDSZ);

#ifdef HAVE_FILE_SYSTEM
  options = read_opts(argc, argv, dnamebuff, &resolv, &mxname, &mxtarget, &lease_file,
		      &username, &domain_suffix, &runfile, &if_names, &if_addrs, 
		      &serv_addrs, &cachesize, &port) ;
#else
  options = read_opts(argc, argv, dnamebuff, NULL, &mxname, &mxtarget, NULL,
		      &username, &domain_suffix, NULL, &if_names, &if_addrs, 
		      &serv_addrs, &cachesize, &port) ;
#endif
  
  /* peerfd is not bound to a low port
     so that we can send queries out on it without them getting
     blocked at firewalls */
  
  if ((peerfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 && 
      errno != EAFNOSUPPORT &&
      errno != EINVAL)
    die("dnsmasq: cannot create socket: %s", NULL);
  
#ifdef HAVE_IPV6
  if ((peerfd6 = socket(AF_INET6, SOCK_DGRAM, 0)) == -1 && 
      errno != EAFNOSUPPORT &&
      errno != EINVAL)
    die("dnsmasq: cannot create IPv6 socket: %s", NULL);
#else
  peerfd6 = -1;
#endif
  
  if (peerfd == -1 && peerfd6 == -1)
    die("dnsmasq: no kernel support for IPv4 _or_ IPv6.", NULL);

  interfaces = find_all_interfaces(if_names, if_addrs, port);
  
  /* open a socket bound to NS port on each local interface.
     this is necessary to ensure that our replies originate from
     the address they were sent to. See Stevens page 531 */
  for (iface = interfaces; iface; iface = iface->next)
    {
      if ((iface->fd = socket(iface->addr.sa.sa_family, SOCK_DGRAM, 0)) == -1)
	die("cannot create socket: %s", NULL);
	      
      if (bind(iface->fd, &iface->addr.sa, sa_len(&iface->addr)))
	die("bind failed: %s", NULL);
    }

  forward_init(1);

  cache_init(cachesize, options & OPT_LOG);
  
  setbuf(stdout, NULL);

#ifdef HAVE_FILE_SYSTEM
  if (!(options & OPT_DEBUG))
    {
      FILE *pidfile;
      struct passwd *ent_pw;
        
      /* The following code "daemonizes" the process. 
	 See Stevens section 12.4 */

#ifdef HAVE_FORK
      if (fork() != 0 )
	exit(0);
      
      setsid();
      
      if (fork() != 0)
	exit(0);
#endif
      
      chdir("/");
      umask(022); /* make pidfile 0644 */
      
      /* write pidfile _after_ forking ! */
      if (runfile && (pidfile = fopen(runfile, "w")))
      	{
	  fprintf(pidfile, "%d\n", (int) getpid());
	  fclose(pidfile);
	}
      
      umask(0);
#if 0
      for (i=0; i<64; i++)
	{
	  if (i == peerfd || i == peerfd6)
	    continue;
	  for (iface = interfaces; iface; iface = iface->next)
	    if (iface->fd == i)
	      break;
	  if (!iface)
	    close(i);
	}
#endif
      /* Change uid and gid for security */
      if (username && (ent_pw = getpwnam(username)))
	{
	  gid_t dummy;
	  struct group *gp;
	  /* remove all supplimentary groups */
	  setgroups(0, &dummy);
	  /* change group to "dip" if it exists, for /etc/ppp/resolv.conf 
	     otherwise get the group for "nobody" */
	  if ((gp = getgrnam("dip")) || (gp = getgrgid(ent_pw->pw_gid)))
	    setgid(gp->gr_gid); 
	  /* finally drop root */
	  setuid(ent_pw->pw_uid);
	}
    }
#else
#endif

  /* In debug mode, log to stderr too and cut the prefix crap. */
  openlog("dnsmasq", options & OPT_DEBUG ? LOG_PERROR : LOG_PID, LOG_DAEMON);
  
  if (cachesize)
    syslog(LOG_INFO, "started, version %s cachesize %d", VERSION, cachesize);
  else
    syslog(LOG_INFO, "started, version %s cache disabled", VERSION);
  
  if (mxname)
    syslog(LOG_INFO, "serving MX record for mailhost %s target %s", 
	   mxname, mxtarget);
  
  //if (getuid() == 0 || geteuid() == 0)
  //  syslog(LOG_WARNING, "failed to drop root privs");
  
  serv_addrs = servers = last_server = check_servers(serv_addrs, interfaces, peerfd, peerfd6);
  
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
  {
	//junzhao 2004.5.25
	int whichconn = atoi(nvram_safe_get("wan_active_connection"));
	int which = whichconn;
	char word[280], *next, dnstmp[32];
	char *dname;
	char encap[15];

 	memset(domainname, 0, sizeof(domainname));
	if(which == 8)
		goto origin;

	memset(encap, 0, sizeof(encap));
	check_cur_encap(which, encap);
	if(strcmp(encap, "pppoe") && strcmp(encap, "1483bridged"))
		goto origin;
	
	memset(word, 0, sizeof(word));
	if(!strcmp(encap, "pppoe"))
	{
	#ifdef MPPPOE_SUPPORT
		foreach(word, nvram_safe_get("mpppoe_enable"), next)
		{
			if(which -- == 0)
				break;
		}
	#else 
		goto origin;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#ifdef IPPPOE_SUPPORT
		foreach(word, nvram_safe_get("ipppoe_enable"), next)
		{
			if(which -- == 0)
				break;
		}
	#else 
		goto origin;
	#endif
	}
	if(!strcmp(word, "0"))
		goto origin;
		
	which = whichconn;
	memset(word, 0, sizeof(word));
	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		#define WAN_MPPPOE_GET_DNS "/tmp/status/wan_mpppoe_get_dns"
		if(!file_to_buf(WAN_MPPPOE_GET_DNS, word, sizeof(word)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_GET_DNS);
			goto origin;
		}
	#else
		goto origin;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		#define WAN_IPPPOE_GET_DNS "/tmp/status/wan_ipppoe_get_dns"
		if(!file_to_buf(WAN_IPPPOE_GET_DNS, word, sizeof(word)))
		{
			printf("no buf in %s!\n", WAN_IPPPOE_GET_DNS);
			goto origin;
		}
	#else
		goto origin;
	#endif
	}
	
	foreach(dnstmp, word, next)
	{
		if(which -- == 0)
		{
			memset(pdns, 0, sizeof(pdns));
			memset(sdns, 0, sizeof(sdns));
			sscanf(dnstmp, "%16[^:]:%16[^\n]", pdns, sdns);
			break;
		}
	}
	
	which = whichconn;
	memset(word, 0, sizeof(word));
	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("mpppoe_config"), next)
		{
			if(which -- == 0)
				break;
		}
	#else
		goto origin;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("ipppoe_config"), next)
		{
			if(which -- == 0)
				break;
		}
	#else
		goto origin;
	#endif
	}

	dname = rindex(word, ':') + 1;
	if((dname[0]) && (dname[1]))
		strcpy(domainname, dname);

origin:		       	
   }
#endif

  while (1)
    {
      int ready, maxfd = peerfd > peerfd6 ? peerfd : peerfd6;
      fd_set rset;
      HEADER *header;
#ifdef HAVE_FILE_SYSTEM
      struct stat statbuf;
#endif
   
      if (first_loop)
	/* do init stuff only first time round. */
	{
	  first_loop = 0;
	  ready = 0;
	}
      else
	{
	  FD_ZERO(&rset);

	  if (peerfd != -1)
	    FD_SET(peerfd, &rset);
	  if (peerfd6 != -1)
	    FD_SET(peerfd6, &rset);
	    	  
	  for (iface = interfaces; iface; iface = iface->next)
	    {
	      FD_SET(iface->fd, &rset);
	      if (iface->fd > maxfd)
		maxfd = iface->fd;
	    }
	  
	  ready = select(maxfd+1, &rset, NULL, NULL, NULL);
	  
	  if (ready == -1)
	    {
	      if (errno == EINTR)
		ready = 0; /* do signal handlers */
	      else
		continue;
	    }
	}

#ifdef HAVE_FILE_SYSTEM
      if (sighup)
	{
	  signal(SIGHUP, SIG_IGN);
	  cache_reload(options & OPT_NO_HOSTS, dnamebuff);
	  if (resolv && (options & OPT_NO_POLL))
	    servers = last_server = 
	      check_servers(reload_servers(resolv, dnamebuff, servers), 
			    interfaces, peerfd, peerfd6);
	  sighup = 0;
	  signal(SIGHUP, sig_handler);
	}

      if (sigusr1)
	{
	  signal(SIGUSR1, SIG_IGN);
	  dump_cache(options & (OPT_DEBUG | OPT_LOG), cachesize);
	  sigusr1 = 0;
	  signal(SIGUSR1, sig_handler);
	}

      if (resolv && !(options & OPT_NO_POLL))
	{
	  if (stat(resolv, &statbuf) == -1)
	    {
	      if (!logged_resolv)
		syslog(LOG_WARNING, "failed to access %s: %m", resolv);
	      logged_resolv = 1;
	    }
	  else
	    {
	      logged_resolv = 0;
	      if ((statbuf.st_mtime > resolv_changed) &&
		  (statbuf.st_mtime < time(NULL) || resolv_changed == 0))
		{
		  resolv_changed = statbuf.st_mtime;
		  servers = last_server = 
		    check_servers(reload_servers(resolv, dnamebuff, servers),
				  interfaces, peerfd, peerfd6);
		}
	    }
	}
#else
#endif

#ifdef HAVE_FILE_SYSTEM
      if (lease_file)
	{
	  if (stat(lease_file, &statbuf) == -1)
	    {
	      if (!logged_lease)
		syslog(LOG_WARNING, "failed to access %s: %m", lease_file);
	      logged_lease = 1;
	    }
	  else
	    { 
	      logged_lease = 0;
	      if ((lease_file_size == (off_t)0) ||
		  (statbuf.st_size > lease_file_size) ||
		  (statbuf.st_ino != lease_file_inode))
		{
		  lease_file_size = statbuf.st_size;
		  lease_file_inode = statbuf.st_ino;
		  load_dhcp(lease_file, domain_suffix, time(NULL), dnamebuff);
		}
	    }
	}
#else
#endif

      if (ready == 0)
	continue; /* no sockets ready */
      
      if (peerfd != -1 && FD_ISSET(peerfd, &rset))
	last_server = reply_query(peerfd, packet, dnamebuff, last_server);
      if (peerfd6 != -1 && FD_ISSET(peerfd6, &rset))
	last_server = reply_query(peerfd6, packet, dnamebuff, last_server);

  	//junzhao 2004.5.31
 	 check_servers_response();

      for (iface = interfaces; iface; iface = iface->next)
	{
	  if (FD_ISSET(iface->fd, &rset))
	    {
	      /* request packet, deal with query */
	      union mysockaddr udpaddr;
	      socklen_t udplen = sizeof(udpaddr);
	      int m, n = recvfrom(iface->fd, packet, PACKETSZ, 0, &udpaddr.sa, &udplen); 
	      udpaddr.sa.sa_family = iface->addr.sa.sa_family;
#ifdef HAVE_IPV6
	      if (udpaddr.sa.sa_family == AF_INET6)
		udpaddr.in6.sin6_flowinfo = htonl(0);
#endif	      
	      header = (HEADER *)packet;
	      if (n >= (int)sizeof(HEADER) && !header->qr)
		{
		  m = answer_request (header, ((char *) header) + PACKETSZ, (unsigned int)n, 
				      mxname, mxtarget, options, dnamebuff);
		  if (m >= 1)
		    {
		      /* answered from cache, send reply */
		      sendto(iface->fd, (char *)header, m, 0, 
			     &udpaddr.sa, sa_len(&udpaddr));
		    }
	#ifdef DNS_ENTRY_SUPPORT   //wwzh add for DNS ENTRY
		  else if ((m = answer_request_from_entry(header, n)) >(int)sizeof(HEADER) )
		  {
		  	sendto(iface->fd, (char *)header, m, 0,
					&udpaddr.sa, sa_len(&udpaddr));
		  }
	#endif
		  else 
		    {
		      /* cannot answer from cache, send on to real nameserver */
		      
		#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)
			int whichconn = atoi(nvram_safe_get("wan_active_connection"));
			int which = whichconn;
			
			if(which == 8)
				goto last;
			/*
			char word[280], *next;
			char *dname;
			char encap[15];
			int whichconn = atoi(nvram_safe_get("wan_active_connection"));
			int which = whichconn;
			
			if(which == 8)
				goto last;

			memset(encap, 0, sizeof(encap));
			check_cur_encap(which, encap);
			if(strcmp(encap, "pppoe") && strcmp(encap, "1483bridged"))
				goto last;
			which = whichconn;
			memset(word, 0, sizeof(word));
			if(!strcmp(encap, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				foreach(word, nvram_safe_get("mpppoe_enable"), next)
				{
					if(which -- == 0)
						break;
				}
			#else 
				goto last;
			#endif
			}
			else if(!strcmp(encap, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				foreach(word, nvram_safe_get("ipppoe_enable"), next)
				{
					if(which -- == 0)
						break;
				}
			#else 
				goto last;
			#endif
			}

			if(!strcmp(word, "0"))
				goto last;
			
			which = whichconn;
			*/
			if(check_ppp_proc(8+which))							{
				if(check_ppp_link(8+which))
					goto last;
				else
				{
					/*
					memset(word, 0, sizeof(word));
					if(!strcmp(encap, "pppoe"))
					{
						foreach(word, nvram_safe_get("mpppoe_config"), next)
						{
							if(which -- == 0)
								break;
						}
					}
					else if(!strcmp(encap, "1483bridged"))
					{
						foreach(word, nvram_safe_get("ipppoe_config"), next)
						{
							if(which -- == 0)
								break;
						}
					}
					
					dname = rindex(word, ':') + 1;
					if(dname[0] && strstr(dnamebuff,dname))*/
					if(domainname[0] && strstr(dnamebuff,domainname))
					{
						char devname[6];
						char gateway[16];
						int sockfd;
						struct ifreq ifr;
						which = whichconn;
						memset(devname, 0 , sizeof(devname));
						sprintf(devname, "ppp%d", 8+which); 
						sockfd = socket(AF_INET, SOCK_DGRAM, 0);
						if(sockfd < 0)
							goto last;
						strncpy(ifr.ifr_name, devname, IFNAMSIZ);
						if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
						{
							close(sockfd);
							goto last;
						}
						if(ifr.ifr_flags & IFF_UP)
						{
							strncpy(ifr.ifr_name, devname, IFNAMSIZ);
							if(ioctl(sockfd, SIOCGIFDSTADDR, &ifr) < 0)
							{
								close(sockfd);	
								goto last;
							}
							memset(gateway, 0, sizeof(gateway));
							strcpy(gateway, inet_ntoa(sin_addr(&ifr.ifr_dstaddr)));
							eval("ping", "-c", "1", gateway);	
							close(sockfd);
							sleep(1);
							continue;
						}
						close(sockfd);
					}
				}
			}	

		#endif
last:	
		      last_server = forward_query(iface->fd, peerfd, peerfd6, &udpaddr, header, n, 
						  options && OPT_ORDER, dnamebuff,
						  servers, last_server);
		    }
		}
	      
	    }
	}
    }
  
  return 0;
}

