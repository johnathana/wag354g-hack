/* dhcpc.c
 *
 * udhcp DHCP client
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <stdio.h>
#include <cy_conf.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
//add by lzh;
#include <shutils.h>
#include <bcmnvram.h>
#include "dhcpd.h"
#include "dhcpc.h"
#include "options.h"
#include "clientpacket.h"
#include "packet.h"
#include "script.h"
#include "socket.h"
#include "debug.h"
#include "pidfile.h"
#include "get_time.h"

static int state;
static unsigned long requested_ip; /* = 0 */
static unsigned long server_addr;
//add by peny; 04/10.22
static unsigned long manual_ciaddr;
static unsigned long manual_server;

//add by lzh;
static unsigned long dns_addr;
static unsigned long subnet_mask;
static unsigned long client_ip_lease;
static unsigned long timeout;
static int packet_num; /* = 0 */
static int fd;
static int signal_pipe[2];

#define LISTEN_NONE 0
#define LISTEN_KERNEL 1
#define LISTEN_RAW 2
static int listen_mode;

#define DEFAULT_SCRIPT	"/usr/share/udhcpc/default.script"
static const unsigned long IP_Subnet_Mask[7]=
{
	0x00ffffff,0x80ffffff,0xc0ffffff,0xe0ffffff,
	0xf0ffffff,0xf8ffffff,0xfcffffff
};

struct client_config_t client_config = {
	/* Default options. */
	abort_if_no_lease: 0,
	foreground: 0,
	quit_after_lease: 0,
	background_if_no_lease: 0,
	interface: "eth0",
	pidfile: NULL,
	script: DEFAULT_SCRIPT,
	clientid: NULL,
	hostname: NULL,
	ifindex: 0,
	arp: "\0\0\0\0\0\0",		/* appease gcc-3.0 */
};

#ifndef BB_VER
static void show_usage(void)
{
	printf(
"Usage: udhcpc [OPTIONS]\n\n"
"  -c, --clientid=CLIENTID         Client identifier\n"
"  -H, --hostname=HOSTNAME         Client hostname\n"
"  -h                              Alias for -H\n"
"  -f, --foreground                Do not fork after getting lease\n"
"  -b, --background                Fork to background if lease cannot be\n"
"                                  immediately negotiated.\n"
"  -i, --interface=INTERFACE       Interface to use (default: eth0)\n"
"  -n, --now                       Exit with failure if lease cannot be\n"
"                                  immediately negotiated.\n"
"  -p, --pidfile=file              Store process ID of daemon in file\n"
"  -q, --quit                      Quit after obtaining lease\n"
"  -r, --request=IP                IP address to request (default: none)\n"
"  -s, --script=file               Run file at dhcp events (default:\n"
"                                  " DEFAULT_SCRIPT ")\n"
/* da_f@2005.3.4 add */
"  -e,                             Enable to send inform packet when dhcpc startup\n"
"  -v, --version                   Display version\n"
	);
	exit(0);
}
#endif
//written by lzh;
//convert long to str
static unsigned long StrToLong(char *src)
{
	unsigned long value[4],addr;
	int i;
	
	addr = 0;
       	for (i=0;i<4;i++) value[i] = 0;
	
	sscanf(src,"%d.%d.%d.%d",&value[0],&value[1],&value[2],&value[3]);
	addr = (value[0] & 0xff) |
	       ((value[1] & 0xff) << 8) |
	       ((value[2] & 0xff) << 16) |
 	       ((value[3] & 0xff) << 24);	
	return addr;	
}	
//--------------------------------------------------
static void LongToStr(unsigned long src,char *dst)
{
	unsigned long new[4];

	new[0] = src & 0xff;
	new[1] = (src & 0xff00) >> 8;
        new[2] = (src & 0xff0000) >> 16;
	new[3] = (src & 0xff000000) >> 24;
	sprintf(dst,"%d.%d.%d.%d",new[0],new[1],new[2],new[3]);	
}	
//--------------------------------------------------------
static int ValidDhcpSubnetMask(unsigned long subnetmask)
{
	int i;
	for (i=0;i<7;i++)
	{
		if (IP_Subnet_Mask[i] == subnetmask)
			return 1;
	}
	return 0;	
}
//------------------------------------------------------------
static void SaveInfoFromDHCPServer()
{
	char new_ip[20],tmp_lease[10];

	//save new lan ip addr;
	LongToStr(requested_ip,new_ip);
	cprintf("new lan ip addr=%s\n",new_ip);
	nvram_set("lan_ipaddr",new_ip);

	//save new subnet mask;
	if (ValidDhcpSubnetMask(subnet_mask))
	{
		LongToStr(subnet_mask,new_ip);
		cprintf("new subnet mask=%s\n",new_ip);
		nvram_set("lan_netmask",new_ip);	
	}else
		cprintf("new subnet mask is invalid!\n");

	//save dns;
	LongToStr(dns_addr,new_ip);
	cprintf("new lan dns addr=%s\n",new_ip);
	nvram_set("lan_dhcp_dns",new_ip);

	//save ip lease time;
	sprintf(tmp_lease,"%d",client_ip_lease);
	nvram_set("client_ip_lease",tmp_lease);

	//set dhcp disavle(lan_proto=static) if get lan dhcp server;
	nvram_set("lan_proto","static");	
}
//end of written by lzh;

/* just a little helper */
static void change_mode(int new_mode)
{
	DEBUG(LOG_INFO, "entering %s listen mode",
		new_mode ? (new_mode == 1 ? "kernel" : "raw") : "none");
	close(fd);
	fd = -1;
	listen_mode = new_mode;
}


/* perform a renew */
static void perform_renew(void)
{
	LOG(LOG_INFO, "Performing a DHCP renew");
	switch (state) {
	case BOUND:
		change_mode(LISTEN_KERNEL);
	case RENEWING:
	case REBINDING:
		state = RENEW_REQUESTED;
		break;
	case RENEW_REQUESTED: /* impatient are we? fine, square 1 */
		run_script(NULL, "deconfig");
	case REQUESTING:
	case RELEASED:
		change_mode(LISTEN_RAW);
		state = INIT_SELECTING;
		break;
	case INIT_SELECTING:
		break;
	}

	/* start things over */
	packet_num = 0;

	/* Kill any timeouts because the user wants this to hurry along */
	timeout = 0;
}


/* perform a release */
static void perform_release(void)
{
	char buffer[16];
	struct in_addr temp_addr;

	/* send release packet */
	if (state == BOUND || state == RENEWING || state == REBINDING) {
		temp_addr.s_addr = server_addr;
		sprintf(buffer, "%s", inet_ntoa(temp_addr));
		temp_addr.s_addr = requested_ip;
		LOG(LOG_INFO, "Unicasting a release of %s to %s", 
				inet_ntoa(temp_addr), buffer);
		send_release(server_addr, requested_ip); /* unicast */
		run_script(NULL, "deconfig");
	}
	LOG(LOG_INFO, "Entering released state");

	change_mode(LISTEN_NONE);
	state = RELEASED;
	timeout = 0x7fffffff;
}


/* Exit and cleanup */
static void exit_client(int retval)
{
	pidfile_delete(client_config.pidfile);
	CLOSE_LOG();
	exit(retval);
}


/* Signal handler */
static void signal_handler(int sig)
{
	if (write(signal_pipe[1], &sig, sizeof(sig)) < 0) {
		LOG(LOG_ERR, "Could not send signal: %s",
			strerror(errno));
	}
}


static void background(void)
{
	int pid_fd;

	pid_fd = pidfile_acquire(client_config.pidfile); /* hold lock during fork. */
	while (pid_fd >= 0 && pid_fd < 3) pid_fd = dup(pid_fd); /* don't let daemon close it */
	if (daemon(0, 0) == -1) {
		perror("fork");
		exit_client(1);
	}
	client_config.foreground = 1; /* Do not fork again. */
	pidfile_write_release(pid_fd);
}


#ifdef COMBINED_BINARY
int udhcpc_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	unsigned char *temp, *message;
	unsigned long t1 = 0, t2 = 0, xid = 0;
	unsigned long start = 0, lease;
	fd_set rfds;
	int retval;
	struct timeval tv;
	int c, len;
	struct dhcpMessage packet;
	struct in_addr temp_addr;
	int pid_fd;
	time_t now;	
	int max_fd;
	int sig;
	//add by lzh;
	time_t AutoDetect_start_time;
	int ReceivedOfferFlag,AutoDetectFlag;
	char lanipaddr[32];
	int inform_enable = 0; /* da_f@2005.3.4 modify */

	static struct option arg_options[] = {
		{"clientid",	required_argument,	0, 'c'},
		{"foreground",	no_argument,		0, 'f'},
		{"background",	no_argument,		0, 'b'},
		{"hostname",	required_argument,	0, 'H'},
		{"hostname",    required_argument,      0, 'h'},
		{"interface",	required_argument,	0, 'i'},
		{"now", 	no_argument,		0, 'n'},
		{"pidfile",	required_argument,	0, 'p'},
		{"quit",	no_argument,		0, 'q'},
		{"request",	required_argument,	0, 'r'},
		{"script",	required_argument,	0, 's'},
		{"send_inform",	no_argument,		0, 'e'}, /* da_f@2005.3.4 add */
		{"version",	no_argument,		0, 'v'},
		{"help",	no_argument,		0, '?'},
		{0, 0, 0, 0}
	};

	/* get options */
	while (1) {
		int option_index = 0;
		/* da_f@2005.3.4 modify */
		c = getopt_long(argc, argv, "c:fbH:h:i:np:qr:s:ev", arg_options, &option_index);
		if (c == -1) break;
		
		switch (c) {
		case 'c':
			len = strlen(optarg) > 255 ? 255 : strlen(optarg);
			if (client_config.clientid) free(client_config.clientid);
			client_config.clientid = xmalloc(len + 2);
			client_config.clientid[OPT_CODE] = DHCP_CLIENT_ID;
			client_config.clientid[OPT_LEN] = len;
			client_config.clientid[OPT_DATA] = '\0';
			strncpy(client_config.clientid + OPT_DATA, optarg, len);
			break;
		case 'f':
			client_config.foreground = 1;
			break;
		case 'b':
			client_config.background_if_no_lease = 1;
			break;
		case 'h':
		case 'H':
			len = strlen(optarg) > 255 ? 255 : strlen(optarg);
			if (client_config.hostname) free(client_config.hostname);
			client_config.hostname = xmalloc(len + 2);
			client_config.hostname[OPT_CODE] = DHCP_HOST_NAME;
			client_config.hostname[OPT_LEN] = len;
			strncpy(client_config.hostname + 2, optarg, len);
			break;
		case 'i':
			client_config.interface =  optarg;
			break;
		case 'n':
			client_config.abort_if_no_lease = 1;
			break;
		case 'p':
			client_config.pidfile = optarg;
			break;
		case 'q':
			client_config.quit_after_lease = 1;
			break;
		case 'r':
			requested_ip = inet_addr(optarg);
			break;
		case 's':
			client_config.script = optarg;
			break;
		/* da_f@2005.3.4 start. add */
		case 'e':
			inform_enable = 1;
			break;
		/* da_f@2005.3.4 end */
		case 'v':
			printf("udhcpcd, version %s\n\n", VERSION);
			exit_client(0);
			break;
		default:
			show_usage();
		}
	}
        /*add by peny   daemonsize  */  
        if (fork() != 0 )
              exit(0);
        setsid();
        chdir("/");
        umask(022);  
    	/* end  daemonsize */
	OPEN_LOG("udhcpc");
	LOG(LOG_INFO, "udhcp client (v%s) started", VERSION);

	pid_fd = pidfile_acquire(client_config.pidfile);
	pidfile_write_release(pid_fd);

	if (read_interface(client_config.interface, &client_config.ifindex, 
			   NULL, client_config.arp) < 0)
		exit_client(1);
		
	if (!client_config.clientid) {
		client_config.clientid = xmalloc(6 + 3);
		client_config.clientid[OPT_CODE] = DHCP_CLIENT_ID;
		client_config.clientid[OPT_LEN] = 7;
		client_config.clientid[OPT_DATA] = 1;
		memcpy(client_config.clientid + 3, client_config.arp, 6);
	}

	/* ensure that stdin/stdout/stderr are never returned by pipe() */
	if (fcntl(STDIN_FILENO, F_GETFL) == -1)
		(void) open("/dev/null", O_RDONLY);
	if (fcntl(STDOUT_FILENO, F_GETFL) == -1)
		(void) open("/dev/null", O_WRONLY);
	if (fcntl(STDERR_FILENO, F_GETFL) == -1)
		(void) open("/dev/null", O_WRONLY);	

	/* setup signal handlers */
	pipe(signal_pipe);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, signal_handler);
	
	state = INIT_SELECTING;
	//run_script(NULL, "deconfig");
	change_mode(LISTEN_RAW);

	//add by lzh;
	AutoDetect_start_time = get_time(0);
	ReceivedOfferFlag = 0;
	AutoDetectFlag = 1;
	
	for (;;) {

		tv.tv_sec = timeout - get_time(0);
		tv.tv_usec = 0;
		FD_ZERO(&rfds);

		if (!strcmp(client_config.interface,"br0") && (AutoDetectFlag == 1))
		{
			cprintf("Autodetect_dhcp=%d\n",nvram_match("Autodetect_dhcp","1"));
			if (nvram_match("AutoDetect_dhcp","1"))
			{
				cprintf("Start Auto detect lan dhcp server in 6s\n");	
				if ((get_time(0) - AutoDetect_start_time) <= 6)
				{
					if (ReceivedOfferFlag == 1)
					{
						cprintf("Find dhcp server in LAN,then restart rc and continue run dhcp client\n");
						ReceivedOfferFlag = 0;
						SaveInfoFromDHCPServer();
						nvram_set("AutoDetect_dhcp","0");
						nvram_commit();
						//restart rc;
						kill(1,SIGHUP);
						exit_client(1);		
					}	
				}else{
					if (ReceivedOfferFlag == 0)
					{
						cprintf("Not find dhcp server in LAN,then exit dhcp client\n");
						nvram_set("action_service","dhcpc");
						kill(1,SIGUSR1);
						exit_client(1);	
					}	
				}
			}else{
				AutoDetectFlag = 0;
				nvram_set("AutoDetect_dhcp","1");

				lease = atoi(nvram_safe_get("client_ip_lease"));
				t1 = lease /2;
				t2 = (lease * 0x07) >> 3;
				start = get_time(0);
				timeout = t1 + start;
				
				memset(lanipaddr,0,sizeof(lanipaddr));
				strcpy(lanipaddr,nvram_get("lan_ipaddr"));
				requested_ip = StrToLong(lanipaddr);
				cprintf("dhcp client restart:request ip=%s,lease=%d\n",lanipaddr,lease);
				state = BOUND;	
				if (client_config.quit_after_lease)
					exit_client(0);
				else if (client_config.foreground)
					background();	
			}	
		}		
		//end add by lzh;
		
		if (listen_mode != LISTEN_NONE && fd < 0) {
			if (listen_mode == LISTEN_KERNEL)
				fd = listen_socket(INADDR_ANY, CLIENT_PORT, client_config.interface);
			else
				fd = raw_socket(client_config.ifindex);
			if (fd < 0) {
				LOG(LOG_ERR, "FATAL: couldn't listen on socket, %s", strerror(errno));
				exit_client(0);
			}
		}
		if (fd >= 0) FD_SET(fd, &rfds);
		FD_SET(signal_pipe[0], &rfds);		

		if (tv.tv_sec > 0) {
			DEBUG(LOG_INFO, "Waiting on select...\n");
			max_fd = signal_pipe[0] > fd ? signal_pipe[0] : fd;
			retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		} else retval = 0; /* If we already timed out, fall through */

		now = get_time(0);
		if (retval == 0) {
			/* timeout dropped to zero */
			switch (state) {
			case INIT_SELECTING:
				if (packet_num < 3) {
					if (packet_num == 0)
						xid = random_xid();

			    /* add by peny .send discover or inform packet */
					/* da_f@2005.3.4 modify
					 * Do not need "enable_inform" any more
                                        inform_enable = nvram_safe_get("enable_inform"); */
                                        if(0 == inform_enable) /* da_f@2005.3.4 modify */
                                          send_discover(xid, requested_ip);
                                        else if(1 == inform_enable) /* da_f@2005.3.4 modify */
					{
	                                  if(inet_aton(nvram_safe_get("inform_ip"),&(manual_ciaddr)))
					  {
		                             inet_aton(nvram_safe_get("dserver_ip"),&(manual_server));
		                             send_information(xid,manual_server, manual_ciaddr);
					   }
		                         }
				         /* end discaver or inform packet*/ 	
 
					timeout = now + ((packet_num == 2) ? 4 : 2);
					packet_num++;
				} else {
					if (client_config.background_if_no_lease) {
						LOG(LOG_INFO, "No lease, forking to background.");
						background();
					} else if (client_config.abort_if_no_lease) {
						LOG(LOG_INFO, "No lease, failing.");
						exit_client(1);
				  	}
					/* wait to try again */
					packet_num = 0;
					timeout = now + 5;	// change wait time from 60 to 5 seconds , by honor
				}
				break;
			case RENEW_REQUESTED:
			case REQUESTING:
				if (packet_num < 3) {
					/* send request packet */
					if (state == RENEW_REQUESTED)
						send_renew(xid, server_addr, requested_ip); /* unicast */
					else send_selecting(xid, server_addr, requested_ip); /* broadcast */
					
					timeout = now + ((packet_num == 2) ? 10 : 2);
					packet_num++;
				} else {
					/* timed out, go back to init state */
					if (state == RENEW_REQUESTED) run_script(NULL, "deconfig");
					state = INIT_SELECTING;
					timeout = now;
					packet_num = 0;
					change_mode(LISTEN_RAW);
				}
				break;
			case BOUND:
				/* Lease is starting to run out, time to enter renewing state */
				state = RENEWING;
				//change_mode(LISTEN_KERNEL);
				change_mode(LISTEN_RAW);	// Some DHCP server send broadcast ACK after they receive a REQUEST during RENEWING state, by honor
				DEBUG(LOG_INFO, "Entering renew state");
				/* fall right through */
			case RENEWING:
				/* Either set a new T1, or enter REBINDING state */
				if ((t2 - t1) <= (lease / 14400 + 1)) {
					/* timed out, enter rebinding state */
					state = REBINDING;
					timeout = now + (t2 - t1);
					//DEBUG(LOG_INFO, "Entering rebinding state");
					cprintf("dhcp client renewing over and start rebinding\n");
				} else {
					/* send a request packet */
					send_renew(xid, server_addr, requested_ip); /* unicast */
					cprintf("Send unicast renewing request\n");
					t1 = (t2 - t1) / 2 + t1;
					timeout = t1 + start;
				}
				break;
			case REBINDING:
				/* Either set a new T2, or enter INIT state */
				if ((lease - t2) <= (lease / 14400 + 1)) {
					/* timed out, enter init state */
					state = INIT_SELECTING;
					//LOG(LOG_INFO, "Lease lost, entering init state");
					cprintf("rebinding over and restart send discover\n");
					run_script(NULL, "deconfig");
					timeout = now;
					packet_num = 0;
					change_mode(LISTEN_RAW);

					//add by lzh;
					if (!strcmp(client_config.interface,"br0"))
					{
						AutoDetectFlag = 1;
						AutoDetect_start_time = now;
						ReceivedOfferFlag = 0;
					}
				} else {
					/* send a request packet */
					send_renew(xid, 0, requested_ip); /* broadcast */
					cprintf("Send broadcast rebinding request\n");
					t2 = (lease - t2) / 2 + t2;
					timeout = t2 + start;
				}
				break;
			case RELEASED:
				/* yah, I know, *you* say it would never happen */
				timeout = 0x7fffffff;
				break;
                        case ENDINFORM:  //by peny
				/* da_f@2005.3.4 modify
				 * Do not need "enable_inform" any more
                                nvram_set("enable_inform","0"); */
				inform_enable = 0; /* da_f@2005.3.4 modify */
                                timeout = 0x7fffffff;
                                break;
				
			}
		} else if (retval > 0 && listen_mode != LISTEN_NONE && FD_ISSET(fd, &rfds)) {
			/* a packet is ready, read it */
			
			if (listen_mode == LISTEN_KERNEL)
				len = get_packet(&packet, fd);
			else len = get_raw_packet(&packet, fd);
			
			if (len == -1 && errno != EINTR) {
				DEBUG(LOG_INFO, "error on read, %s, reopening socket", strerror(errno));
				change_mode(listen_mode); /* just close and reopen */
			}
			if (len < 0) continue;
			
			if (packet.xid != xid) {
				DEBUG(LOG_INFO, "Ignoring XID %lx (our xid is %lx)",
					(unsigned long) packet.xid, xid);
				continue;
			}
			
			if ((message = get_option(&packet, DHCP_MESSAGE_TYPE)) == NULL) {
				DEBUG(LOG_ERR, "couldnt get option from packet -- ignoring");
				continue;
			}
			
			switch (state) {
			case INIT_SELECTING:
				/* Must be a DHCPOFFER to one of our xid's */
				if (*message == DHCPOFFER) {
					if ((temp = get_option(&packet, DHCP_SERVER_ID))) {
					//	char *vendor_spc = get_option(%packet,DHCP_VENDOR_SPEC);
					
						memcpy(&server_addr, temp, 4);
						xid = packet.xid;
						requested_ip = packet.yiaddr;
						
                                                 /* enter requesting state */
						//xtt add for D_DHCP 07-06-2004
						{
							FILE *fp = NULL;
							if((fp = fopen("/tmp/D_DHCP.log","r")) == NULL)
							{
							if((fp = fopen("/tmp/D_DHCP.log","w")))
							{
								fwrite("1",1,1,fp);
							}
							}
							fclose (fp);
						}
						/* add by peny for tr069 
						 *
						 *  Code   Len   Data item        Code   Len   Data item   
					 	 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
						 * |  T1 |  n  |  d1 |  d2 | ... |  T2 |  n  |  D1 |  D2 | 
						 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+
						 */
						temp = get_option(&packet,DHCP_VENDOR_SPEC);
						if(temp) {
							char item[1024];
							memset(item,0,sizeof(item));
							cprintf("the temp[0]=%d\n",temp[0]); 
					
							/* Option number 1: URL of the ACS */
							if(temp[0]==1) {
								memcpy(item,&temp[2],temp[1]);
								cprintf("the item=%s\n",item); 
								item[temp[1]]='\0';
								nvram_set("acs_url",item);
								memset(item,0,sizeof(item));

								/* Option number 2: Provisioning code */
								if(temp[temp[1]+2]==2) {
									memcpy(item,&temp[temp[1]+2+2],temp[temp[1]+2+1]);
									item[temp[temp[1]+2+1]]='\0';

									nvram_set("acs_provisioncode",item);
								}
							}	
						}
					        //add by peny .  end	
						
						//add by lzh;
						if (!strcmp(client_config.interface,"br0"))
						{
							//get subnet mask;
							temp = get_option(&packet,DHCP_SUBNET);
							if (temp)
								memcpy(&subnet_mask,temp,4);
							//get domain name;
							temp = get_option(&packet,DHCP_DNS_SERVER);
							if(temp)
								memcpy(&dns_addr,temp,4);
							//get ip lease;
							temp = get_option(&packet,DHCP_LEASE_TIME);
							if (temp)
							{
								memcpy(&client_ip_lease,temp,4);
								client_ip_lease = ntohl(client_ip_lease);							
							}else{
								client_ip_lease = 60*60;
							}
							
							ReceivedOfferFlag  = 1;
							cprintf("dhcp client receive offer message\n");
						}	
						//end add by lzh;
						
						/* enter requesting state */
						state = REQUESTING;
						timeout = now;
						packet_num = 0;
					} else {
						DEBUG(LOG_ERR, "No server ID in message");
					}
			        /*add by peny about infrom pack*/  		
				}else if (*message == DHCPACK)
				  {
                                    temp = get_option(&packet,DHCP_VENDOR_SPEC);                                    if(temp)
                                     {
                                       char item[1024];
                                       memset(item,0,sizeof(item));
                                       cprintf("the temp[0]=%d\n",temp[0]);
                                       if(temp[0]==1)
	                                {
		                          memcpy(item,&temp[2],temp[1]);
		                          cprintf("the item=%s\n",item);
		                          item[temp[1]]='\0';
	                                  nvram_set("acs_url",item);
                                          memset(item,0,sizeof(item));
                                          if(temp[temp[1]+2]==2)
                                            {
                                              memcpy(item,&temp[temp[1]+2+2],temp[temp[1]+2+1]);
                                              item[temp[temp[1]+2+1]]='\0';
                                              nvram_set("acs_provisioncode",item);
                                             }
                                          }
                                        }/*end if(temp)*/

                                     change_mode(LISTEN_RAW);
                                     sleep(3);
                                     state =ENDINFORM ;
                                     cprintf("receive DHCPACK !!!\n");
                                     packet_num = 0;

                                } /*end else if(*message ==DHCPACK) */
				break;
			case RENEW_REQUESTED:
			case REQUESTING:
			case RENEWING:
			case REBINDING:
				if (*message == DHCPACK) 
				{
					if (!(temp = get_option(&packet, DHCP_LEASE_TIME))) {
						LOG(LOG_ERR, "No lease time with ACK, using 1 hour lease");
						lease = 60 * 60;
					} else {
						memcpy(&lease, temp, 4);
						lease = ntohl(lease);
					}
					
					if (!strcmp(client_config.interface,"br0"))
					{
						sprintf(temp,"%ld",lease);
						nvram_set("client_ip_lease",temp);
						cprintf("dhcp client Received ACK message\n");
					}	
					/* enter bound state */
					t1 = lease / 2;
					
					/* little fixed point for n * .875 */
					t2 = (lease * 0x7) >> 3;
					temp_addr.s_addr = packet.yiaddr;
					LOG(LOG_INFO, "(%d) Lease of %s obtained, lease time %ld", 
						state, inet_ntoa(temp_addr), lease);
					start = now;
					timeout = t1 + start;
					requested_ip = packet.yiaddr;
					if (state != RENEWING && state !=REBINDING)
						run_script(&packet,
						   ((state == RENEWING || state == REBINDING) ? "renew" : "bound"));

					state = BOUND;
					change_mode(LISTEN_NONE);
#if 1
					//add by leijun 2004-0617
					//if (atoi(nvram_get("wan_active_connection") == 0))
				
				{
					int num = atoi(nvram_get("wan_active_connection"));
					//printf("-------%d\n", num);
					if(!num)	
				
					{
		
#if 1
						struct in_addr tmp_addr;
						char tmp_buff[32];
					
						tmp_addr.s_addr = server_addr;
						sprintf(tmp_buff, "%s", inet_ntoa(tmp_addr));
						printf("tmp_buff %s\n", tmp_buff);

						if (strcmp(tmp_buff, nvram_get("wan_dhcp_ipaddr")))
						{
							nvram_set("wan_dhcp_ipaddr", tmp_buff);
						} 
#endif
					}
					//////////////////////////////////////////////
				}
#endif
					if (client_config.quit_after_lease) 
						exit_client(0);
					if (!client_config.foreground)
						background();

				} else if (*message == DHCPNAK) {
					/* return to init state */
					LOG(LOG_INFO, "Received DHCP NAK");
					run_script(&packet, "nak");
					if (state != REQUESTING)
						run_script(NULL, "deconfig");
					state = INIT_SELECTING;
					timeout = now;
					requested_ip = 0;
					packet_num = 0;
#if 1
					//add by leijun 2004-0617
					//if (atoi(nvram_get("wan_active_connection") == 0))
				{	
					int num = atoi(nvram_get("wan_active_connection"));
					if(!num)
					{
						nvram_set("wan_dhcp_ipaddr", "0.0.0.0");
					}
				}
					////////////////////////////////////////////
#endif
					change_mode(LISTEN_RAW);
					sleep(3); /* avoid excessive network traffic */
				}
				break;
			/* case BOUND, RELEASED: - ignore all packets */
			}	
		} else if (retval > 0 && FD_ISSET(signal_pipe[0], &rfds)) {
			if (read(signal_pipe[0], &sig, sizeof(sig)) < 0) {
				DEBUG(LOG_ERR, "Could not read signal: %s", 
					strerror(errno));
				continue; /* probably just EINTR */
			}
			switch (sig) {
			case SIGUSR1: 
				perform_renew();
				break;
			case SIGUSR2:
				perform_release();
				break;
			case SIGTERM:
				LOG(LOG_INFO, "Received SIGTERM");
				exit_client(0);
			}
		} else if (retval == -1 && errno == EINTR) {
			/* a signal was caught */		
		} else {
			/* An error occured */
			DEBUG(LOG_ERR, "Error on select");
		}
		
	}
	return 0;
}

