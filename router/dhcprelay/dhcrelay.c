/* dhcrelay.c

   DHCP/BOOTP Relay Agent. */

/*
 * Copyright (c) 1997, 1998, 1999 The Internet Software Consortium.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of The Internet Software Consortium nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INTERNET SOFTWARE CONSORTIUM AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNET SOFTWARE CONSORTIUM OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This software has been written for the Internet Software Consortium
 * by Ted Lemon <mellon@fugue.com> in cooperation with Vixie
 * Enterprises.  To learn more about the Internet Software Consortium,
 * see ``http://www.vix.com/isc''.  To learn more about Vixie
 * Enterprises, see ``http://www.vix.com''.
 */

#include <netinet/in.h>

#ifndef lint
static char ocopyright [] =
"$Id: dhcrelay.c,v 1.1.1.3 2005/08/25 06:27:19 sparq Exp $ Copyright (c) 1997, 1998, 1999 The Internet Software Consortium.  All rights reserved.\n";
#endif /* not lint */

#include "dhcpd.h"
#include "version.h"


#ifdef CLOUD_SUPPORT
#ifdef ZB_DEBUG
#include "../shared/shutils.h"
#define zprintf cprintf
#else
#define zprintf printf
#endif
#endif
#define warn printf
#define note printf
static void usage PROTO ((char *));

TIME cur_time;
TIME default_lease_time = 43200; /* 12 hours... */
TIME max_lease_time = 86400; /* 24 hours... */
struct tree_cache *global_options [256];

int log_perror = 1;

/* Needed to prevent linking against conflex.c. */
int lexline;
int lexchar;
char *token_line;
char *tlname;

char *path_dhcrelay_pid = _PATH_DHCRELAY_PID;

u_int16_t local_port;
u_int16_t remote_port;
int log_priority;

struct server_list {
	struct server_list *next;
	struct sockaddr_in to;
} *servers;


static char copyright [] =
"Copyright 1997, 1998, 1999 The Internet Software Consortium.";
static char arr [] = "All rights reserved.";
static char message [] = "Internet Software Consortium DHCP Relay Agent";
static char contrib [] = "Please contribute if you find this software useful.";
static char url [] = "For info, please visit http://www.isc.org/dhcp-contrib.html";
#define debug note


#ifdef CLOUD_SUPPORT

struct cloud_leases_list cloud_leases[255];

/* get an option with bounds checking (warning, not aligned). */
unsigned char *relay_get_option(struct dhcp_packet *packet, int code)
{
#if 1
	int i, length;
	unsigned char *optionptr;
	int over = 0, done = 0, curr = OPTION_FIELD;
	zprintf("Here is get_option!\n");	
	optionptr = packet->options; 
	optionptr += 4;//ignore cookies;
	i = 0;
	length = DHCP_MTU_MAX - DHCP_FIXED_LEN;

	while (!done) 
	//for(;;)
	{
		if (i >= length) 
		{
			zprintf("bogus packet, option fields too long.");
			return NULL;
		}
		if (optionptr[i + OPT_CODE] == code) 
		{
			if (i + 1 + optionptr[i + OPT_LEN] >= length) 
			{
				zprintf("bogus packet, option fields too long.");
				return NULL;
			}
			return optionptr + i + 2;
		}			
		switch (optionptr[i + OPT_CODE]) 
		{
			case DHCP_PADDING:
				i++;
				break;
			case DHCP_OPTION_OVER:
				if (i + 1 + optionptr[i + OPT_LEN] >= length) 
				{
					zprintf("bogus packet, option fields too long.");
					return NULL;
				}
				over = optionptr[i + 3];
				i += optionptr[OPT_LEN] + 2;
				break;
			case DHCP_END:
				if (curr == OPTION_FIELD && over & FILE_FIELD) 
				{
					optionptr = packet->file;
					i = 0;
					length = 128;
					curr = FILE_FIELD;
				}
			       	else if (curr == FILE_FIELD && over & SNAME_FIELD) 
				{
					optionptr = packet->sname;
					i = 0;
					length = 64;
					curr = SNAME_FIELD;
				}
			       	else
				       	done = 1;
				break;
			default:
				i += optionptr[OPT_LEN + i] + 2;
		}
				//i += optionptr[OPT_LEN + i] + 2;
	}
#endif
	return NULL;
}

void print_leases(struct cloud_leases_list cloud_lease)
{
	printf("mac=%0x:%0x:%0x:%0x:%0x:%0x\n",cloud_lease.mac[0],cloud_lease.mac[1],cloud_lease.mac[2],cloud_lease.mac[3],cloud_lease.mac[4],cloud_lease.mac[5]);
	printf("ip=%0x\n",cloud_lease.ipaddr);
	printf("xid=%0x\n",cloud_lease.xid);
	printf("hostname=%s\n",cloud_lease.hostname);
	printf("lease_time=%d\n",cloud_lease.lease_time);
	printf("timestamp=%d\n",cloud_lease.timestamp);
}

int cloud_set_time(struct dhcp_packet *packet,int i)
{
	unsigned char *tmp_time;
	u_int32_t l_time;
	
	tmp_time = relay_get_option(packet,DHCP_LEASE_TIME);
	zprintf("%s\n",__FUNCTION__);

	if(tmp_time)
	{
		memcpy(&(l_time),tmp_time,4);
		cloud_leases[i].lease_time = ntohl(l_time);
		zprintf("lease_time= %0x , %ld\n",cloud_leases[i].lease_time,cloud_leases[i].lease_time);
	}
	else
	{
		cloud_leases[i].lease_time = 60*60;
	}

	cloud_leases[i].timestamp = cloud_get_time(0);

	return 0;
}

int update_cloudleases(struct dhcp_packet *packet)
{
	int i,find_flg = 0;
	for(i = 0 ;i < 254,cloud_leases[i].xid; i++)
	{
		if(cloud_leases[i].xid == packet->xid)
		{
			cloud_set_time(packet,i);
			find_flg = 1;
			cloud_leases[i].mac[0] = packet->chaddr[0];
			cloud_leases[i].mac[1] = packet->chaddr[1];
			cloud_leases[i].mac[2] = packet->chaddr[2];
			cloud_leases[i].mac[3] = packet->chaddr[3];
			cloud_leases[i].mac[4] = packet->chaddr[4];
			cloud_leases[i].mac[5] = packet->chaddr[5];
			memcpy(&(cloud_leases[i].ipaddr),&(packet->yiaddr),4);
			printf("%s:cloud_leases[%d]:\n",__FUNCTION__,i);
			print_leases(cloud_leases[i]);
			break;
		}
		else if(!strncmp(cloud_leases[i].mac,packet->chaddr,6))
		{
			cloud_set_time(packet,i);
			find_flg = 1;
			memcpy(&(cloud_leases[i].ipaddr),&(packet->yiaddr),4);
			break;
		}
	}
	if(!find_flg)
	{
		printf("not found!\n");
	}
	return 0;
	
}

int del_leases(int num)
{
	int j = num;
	for(;j < 255;j++)
	{
		if(cloud_leases[j].xid)
		{
			zprintf("will change!\n");
			cloud_leases[j] = cloud_leases[j + 1];
		}
	}
	zprintf("will return in del_leases!\n");
	return 0;
		
}

int release_leases(struct dhcp_packet *packet)
{
	int i;
	zprintf("in release_leases!\n");
	for(i = 0 ;i < 254,cloud_leases[i].xid; i++)
	{
		if(!strncmp(cloud_leases[i].mac,packet->chaddr,6))
		{
			zprintf("will del_leases!\n");
			del_leases(i);
		}
	}
	zprintf("will return in release_leases!\n");
	return 0;
}


int add_hostname(unsigned char *hostname,struct dhcp_packet *packet)
{
	int i,find_flg = 0;
	for(i = 0 ;i < 254,cloud_leases[i].xid; i++)
	{
		if(!strncmp(cloud_leases[i].mac,packet->chaddr,6))
		{
			if(!strcmp(cloud_leases[i].hostname,""))
				memcpy(cloud_leases[i].hostname,hostname,64);
			find_flg = 1;
			break;
		}
	}
	if(!find_flg)
	{
		printf("not found,will add!\n");
		memcpy(cloud_leases[i].hostname,hostname,64);
		memcpy(cloud_leases[i].mac,packet->chaddr,6);
		cloud_leases[i].xid = packet->xid;
		printf("%s:cloud_leases[%d]:xid=%0x,hostname=%s\n",__FUNCTION__,i,cloud_leases[i].xid,cloud_leases[i].hostname);
	}
	return 0;

}
#endif

int main (argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	int i;
	struct servent *ent;
	struct server_list *sp = (struct server_list *)0;
	int no_daemon = 0;
	int quiet = 0;
	char *s;

	s = strrchr (argv [0], '/');
	if (!s)
		s = argv [0];
	else
		s++;
	

	/* Initially, log errors to stderr as well as to syslogd. */
#ifdef SYSLOG_4_2
	openlog (s, LOG_NDELAY);
	log_priority = DHCPD_LOG_FACILITY;
#else
	openlog (s, LOG_NDELAY, DHCPD_LOG_FACILITY);
#endif

#if !(defined (DEBUG) || defined (SYSLOG_4_2))
	setlogmask (LOG_UPTO (LOG_INFO));
#endif	

	for (i = 1; i < argc; i++) {
		if (!strcmp (argv [i], "-p")) {
			if (++i == argc)
				usage (s);
			local_port = htons (atoi (argv [i]));
			debug ("binding to user-specified port %d",
			       ntohs (local_port));
		} else if (!strcmp (argv [i], "-pf")) {
			if (++i == argc)
				usage (s);
			path_dhcrelay_pid = argv [i];
		} else if (!strcmp (argv [i], "-d")) {
			no_daemon = 1;
 		} else if (!strcmp (argv [i], "-i")) {
			struct interface_info *tmp =
				((struct interface_info *)
				 dmalloc (sizeof *tmp, "specified_interface"));
			if (!tmp)
				error ("Insufficient memory to %s %s",
				       "record interface", argv [i]);
			if (++i == argc) {
				usage (s);
			}
			memset (tmp, 0, sizeof *tmp);
			strcpy (tmp -> name, argv [i]);
			tmp -> next = interfaces;
			tmp -> flags = INTERFACE_REQUESTED;
			interfaces = tmp;
		} else if (!strcmp (argv [i], "-q")) {
			quiet = 1;
			quiet_interface_discovery = 1;
 		} else if (argv [i][0] == '-') {
 		    usage (s);
 		} else {
			struct hostent *he;
			struct in_addr ia, *iap = (struct in_addr *)0;
			if (inet_aton (argv [i], &ia)) {
				iap = &ia;
			} else {
				he = gethostbyname (argv [i]);
				if (!he) {
					warn ("%s: host unknown", argv [i]);
				} else {
					iap = ((struct in_addr *)
					       he -> h_addr_list [0]);
				}
			}
			if (iap) {
				sp = (struct server_list *)malloc (sizeof *sp);
				if (!sp)
					error ("no memory for server.\n");
				sp -> next = servers;
				servers = sp;
				memcpy (&sp -> to.sin_addr,
					iap, sizeof *iap);
			}
 		}
	}

	if (!quiet) {
		note ("%s %s", message, DHCP_VERSION);
		note (copyright);
		note (arr);
		note ("");
		note (contrib);
		note (url);
		note ("");
	} else
		log_perror = 0;

	/* Default to the DHCP/BOOTP port. */
	if (!local_port) {
		ent = getservbyname ("dhcps", "udp");
		if (!ent)
			local_port = htons (67);
		else
			local_port = ent -> s_port;
		endservent ();
	}
	remote_port = htons (ntohs (local_port) + 1);
  
	/* We need at least one server. */
	if (!sp) {
		usage (s);
	}

	/* Set up the server sockaddrs. */
	for (sp = servers; sp; sp = sp -> next) {
		sp -> to.sin_port = local_port;
		sp -> to.sin_family = AF_INET;
#ifdef HAVE_SA_LEN
		sp -> to.sin_len = sizeof sp -> to;
#endif
	}

	/* Get the current time... */
	GET_TIME (&cur_time);

	/* Discover all the network interfaces. */
	discover_interfaces (DISCOVER_RELAY);

	/* Set up the bootp packet handler... */
	bootp_packet_handler = relay;

	/* Become a daemon... */
	if (!no_daemon) {
		int pid;
		FILE *pf;
		int pfdesc;

		log_perror = 0;
#if 1

		if ((pid = fork()) < 0)
			error ("can't fork daemon: %m");
		else if (pid)
			exit (0);

#endif
		pfdesc = open (path_dhcrelay_pid,
			       O_CREAT | O_TRUNC | O_WRONLY, 0644);

		if (pfdesc < 0) {
			warn ("Can't create %s: %m", path_dhcrelay_pid);
		} else {
			pf = fdopen (pfdesc, "w");
			if (!pf)
				warn ("Can't fdopen %s: %m",
				      path_dhcrelay_pid);
			else {
				fprintf (pf, "%ld\n", (long)getpid ());
				fclose (pf);
			}	
		}

		close (0);
		close (1);
		close (2);
		pid = setsid ();
	}

#ifdef CLOUD_SUPPORT
	memset(cloud_leases,0,sizeof(cloud_leases));
#endif
	/* Start dispatching packets and timeouts... */
	dispatch ();

	/*NOTREACHED*/
	return 0;
}

void relay (ip, packet, length, from_port, from, hfrom)
	struct interface_info *ip;
	struct dhcp_packet *packet;
	int length;
	unsigned int from_port;
	struct iaddr from;
	struct hardware *hfrom;
{
	struct server_list *sp;
	struct sockaddr_in to;
	struct interface_info *out;
	struct hardware hto;

#ifdef CLOUD_SUPPORT
	zprintf("\n $$$$ in relay function!!!\n");
#endif
	if (packet -> hlen > sizeof packet -> chaddr) {
		printf ("Discarding packet with invalid hlen.");
		return;
	}

#ifdef CLOUD_SUPPORT
	if(packet -> op == BOOTREQUEST)
       	{
		unsigned char *relay_msg_type;
	       	relay_msg_type =  relay_get_option(packet,DHCP_MESSAGE_TYPE);
		zprintf("BOOTREQUEST!\n");
		if(NULL == relay_msg_type)
		{
			zprintf("can't get option!\n");
		}

		zprintf("relay_msg_type[0]=%d\n",relay_msg_type[0]);
		
		if(relay_msg_type[0] == DHCPREQUEST)
		{
			unsigned char hostname[64];
			unsigned char *dhcp_hostname;
			int len_name;
			memset(hostname,0,sizeof(hostname));
			debug("%s:will get hostname!\n",__FUNCTION__);
			dhcp_hostname = relay_get_option(packet,DHCP_HOST_NAME);
			if(NULL != dhcp_hostname)
			{
				len_name = dhcp_hostname[-1];
				if(len_name > 63)
				{
					len_name = 63;
				}
				memcpy(hostname,dhcp_hostname,len_name);
				add_hostname(hostname,packet);
			}
		}

		if(relay_msg_type[0] == DHCPRELEASE)
		{
			zprintf("DHCP Release Packet!\n");
			release_leases(packet);
		}

	}
#endif

	/* If it's a bootreply, forward it to the client. */
	if (packet -> op == BOOTREPLY) 
	{
		if (!(packet -> flags & htons (BOOTP_BROADCAST)) &&
		    can_unicast_without_arp ()) 
		{
			to.sin_addr = packet -> yiaddr;
			to.sin_port = remote_port;
#ifdef CLOUD_SUPPORT
			zprintf("enter cloud operation!\n");
			{
				unsigned char *rl_msg_type;
			       	rl_msg_type = relay_get_option(packet,DHCP_MESSAGE_TYPE);
				zprintf("after rl_msg_type!\n");
				if(NULL == rl_msg_type)
				{
					zprintf("Couldn't get option from packet!\n");
					goto ignore_cloud;
				}
				
				zprintf("before DHCPACK!\n");

				if(rl_msg_type[0] == DHCPACK)
				{
					int cnt;
					zprintf("before update leases!\n");
					cnt = update_cloudleases(packet);
					zprintf("after update leases!\n");
					if(cnt) // found
					{
						zprintf("cnt!=0!\n");
					}
					else
						zprintf("Not found!\n");
				}
			}
ignore_cloud:
#endif
		}
	       	else
	       	{
#ifdef CLOUD_SUPPORT
			zprintf("broadcast\n");
			zprintf("cloud operation inc broadcast!\n");
			{
				unsigned char *rl_msg_type;
			       	rl_msg_type = relay_get_option(packet,DHCP_MESSAGE_TYPE);
				zprintf("after rl_msg_type!\n");
				if(NULL == rl_msg_type)
				{
					zprintf("Couldn't get option from packet!\n");
					goto no_cloud;
				}
				
				zprintf("before DHCPACK in broadcast!\n");

				if(rl_msg_type[0] == DHCPACK)
				{
					int cnt;
					zprintf("before update leases in broadcast!\n");
					cnt = update_cloudleases(packet);
					zprintf("after update leases in broadcast!\n");
				}
			}
no_cloud:
#endif
			to.sin_addr.s_addr = htonl (INADDR_BROADCAST);
			to.sin_port = remote_port;
		}
		to.sin_family = AF_INET;
#ifdef HAVE_SA_LEN
		to.sin_len = sizeof to;
#endif

		/* Set up the hardware destination address. */
		hto.hlen = packet -> hlen;
		if (hto.hlen > sizeof hto.haddr)
			hto.hlen = sizeof hto.haddr;
		memcpy (hto.haddr, packet -> chaddr, hto.hlen);
		hto.htype = packet -> htype;

		/* Find the interface that corresponds to the giaddr
		   in the packet. */
		for (out = interfaces; out; out = out -> next) {
			if (!memcmp (&out -> primary_address,
				     &packet -> giaddr,
				     sizeof packet -> giaddr))
				break;
		}
		if (!out) {
			printf("packet to bogus giaddr %s.\n",
			      inet_ntoa (packet -> giaddr));
			return;
		}

		if (!send_packet (out,
				  (struct packet *)0,
				  packet, length, out -> primary_address,
				  &to, &hto) < 0)
			debug ("forwarded BOOTREPLY for %s to %s",
			       print_hw_addr (packet -> htype, packet -> hlen,
					      packet -> chaddr),
			       inet_ntoa (to.sin_addr));
 
	       printf("after sending BOOTREPLY packet!\n");	
		return;
	}


	/* If giaddr is set on a BOOTREQUEST, ignore it - it's already
	   been gatewayed. */
	if (packet -> giaddr.s_addr) {
		printf ("ignoring BOOTREQUEST with giaddr of %s\n",
		      inet_ntoa (packet -> giaddr));
		return;
	}

	/* Set the giaddr so the server can figure out what net it's
	   from and so that we can later forward the response to the
	   correct net. */
	packet -> giaddr = ip -> primary_address;

	/* Otherwise, it's a BOOTREQUEST, so forward it to all the
	   servers. */
	for (sp = servers; sp; sp = sp -> next) {
		if (!send_packet ((fallback_interface
				   ? fallback_interface : interfaces),
				  (struct packet *)0,
				  packet, length, ip -> primary_address,
				  &sp -> to, (struct hardware *)0) < 0) {
			debug ("forwarded BOOTREQUEST for %s to %s",
			       print_hw_addr (packet -> htype, packet -> hlen,
					      packet -> chaddr),
			       inet_ntoa (sp -> to.sin_addr));
		}
	}
	      printf("after forward BOOTREQUEST packet!\n");	

				 
}

static void usage (appname)
	char *appname;
{
	note (message);
	note (copyright);
	note (arr);
	note ("");
	note (contrib);
	note (url);
	note ("");

	warn ("Usage: %s [-i] [-d] [-i if0] [...-i ifN] [-p <port>]", appname);
	error ("      [-pf pidfilename] [server1 [... serverN]]");
}

void cleanup ()
{
}

int write_lease (lease)
	struct lease *lease;
{
	return 1;
}

int commit_leases ()
{
	return 1;
}

void bootp (packet)
	struct packet *packet;
{
}

void dhcp (packet)
	struct packet *packet;
{
}
