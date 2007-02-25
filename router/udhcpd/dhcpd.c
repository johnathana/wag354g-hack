/* dhcpd.c
 *
 * udhcp Server
 * Copyright (C) 1999 Matthew Ramsay <matthewr@moreton.com.au>
 *			Chris Trew <ctrew@moreton.com.au>
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
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

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <bcmnvram.h>
#include <cy_conf.h>

#include "debug.h"
#include "dhcpd.h"
#include "arpping.h"
#include "socket.h"
#include "options.h"
#include "files.h"
#include "leases.h"
#include "packet.h"
#include "serverpacket.h"
#include "pidfile.h"
#include "get_time.h"


#include <shutils.h>
#ifdef STB_SUPPORT
#include "stbbridge.h"
#endif

/* da_f 2005.6.21 add */
#define cprintf(str, arg...)

//zhangbin-2004.08.05
#define DEBUG(...)
extern int is_in_machost_table(u_int32_t ip,int *index);
/* globals */
struct dhcpOfferedAddr *leases;
struct server_config_t server_config;
static int signal_pipe[2];

//single pc mode -- leijun
u_int8_t single_pc_mac[16];
int send_nak_flag = 0;
int single_pc_mode_flag = 0;
#ifdef STB_SUPPORT
struct stb_info stbinfo[MAXSTBDEV];
char stbmatchtypes[MAXSTBTYPE][58];
char *vendor;
char wanipaddr[20];
#endif


/* Exit and cleanup */
static void exit_server(int retval)
{
	pidfile_delete(server_config.pidfile);
	CLOSE_LOG();
	exit(retval);
}


/* Signal handler */
static void signal_handler(int sig)
{
	if (send(signal_pipe[1], &sig, sizeof(sig), MSG_DONTWAIT) < 0) {
		LOG(LOG_ERR, "Could not send signal: %s", 
			strerror(errno));
	}
}


#ifdef COMBINED_BINARY	
int udhcpd_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{	
	fd_set rfds;
#ifdef STB_SUPPORT
        fd_set w;
        fd_set x;

#endif	
	struct timeval tv;
	int server_socket = -1;
	int bytes, retval;
	struct dhcpMessage packet;
	unsigned char *state;
	unsigned char *server_id, *requested, *name;
	u_int32_t server_id_align, requested_align, tmp_share_wanip,tmp_old_wanip = 0;

	unsigned long timeout_end;
	struct option_set *option;
	struct dhcpOfferedAddr *lease;
	
	int pid_fd;
	int max_sock;
	int sig;
  char mac[32];
  char hostname[64];
       
        
#ifdef HSIAB_SUPPORT
	char message[254];
        struct in_addr ipaddr;
#endif
#ifdef STB_SUPPORT
	 int relay_socket = -1;
         int which_tmp =256 ;
#endif
	 
  int i,j;
     
	OPEN_LOG("udhcpd");
	LOG(LOG_INFO, "udhcp server (v%s) started", VERSION);

	memset(&server_config, 0, sizeof(struct server_config_t));
	memset((void *)&single_pc_mac, 0, sizeof(single_pc_mac));
	
	if (argc < 2)
		read_config(DHCPD_CONF_FILE);
	else read_config(argv[1]);
	//add by leijun
#if 1
	if (server_config.single_pc_mode > 0)
	{
		clear_sharewanip_mac();
	}
#endif
	pid_fd = pidfile_acquire(server_config.pidfile);
	pidfile_write_release(pid_fd);

	if ((option = find_option(server_config.options, DHCP_LEASE_TIME))) {
		memcpy(&server_config.lease, option->data + 2, 4);
		server_config.lease = ntohl(server_config.lease);
	}
	else server_config.lease = LEASE_TIME;
	
	leases = malloc(sizeof(struct dhcpOfferedAddr) * server_config.max_leases);
	memset(leases, 0, sizeof(struct dhcpOfferedAddr) * server_config.max_leases);
	read_leases(server_config.lease_file);

	if (read_interface(server_config.interface, &server_config.ifindex,
			   &server_config.server, server_config.arp) < 0)
		exit_server(1);

#ifndef DEBUGGING
	pid_fd = pidfile_acquire(server_config.pidfile); /* hold lock during fork. */
	if (daemon(0, 0) == -1) {
		perror("fork");
		exit_server(1);
	}
	pidfile_write_release(pid_fd);
#endif

	socketpair(AF_UNIX, SOCK_STREAM, 0, signal_pipe);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, signal_handler);

	timeout_end = get_time(0) + server_config.auto_time;
	if (server_config.single_pc_mode > 0)
	{
		eval("iptables", "-F");
		eval("iptables", "-A", "FORWARD", "-i", "br0", "-j", "DROP");
	}
#ifdef STB_SUPPORT
        if(server_config.stb_support == 1)
        {
          stbinfo_init();
          which_tmp = atoi(nvram_safe_get("wan_active_connection"));
          stbmatchtable_init();
          stb_enable_inkernel(1);
        }
	else 
          stb_enable_inkernel(0);
#endif
	
	while(1) { /* loop until universe collapses */

		if (server_socket < 0)
			if ((server_socket = listen_socket(INADDR_ANY, SERVER_PORT, server_config.interface)) < 0) {
				LOG(LOG_ERR, "FATAL: couldn't create server socket, %s", strerror(errno));
				exit_server(0);
			}			
            #ifdef STB_SUPPORT
                 #define MAX_CHAN 8
	         if(server_config.stb_support == 1)
	           {
	            if (relay_socket < 0)
	              {
	                    do
	                     {
                               char toifname[6] = "nas0";
                               if(which_tmp == MAX_CHAN)
			               which_tmp = 0;
			       memset(toifname, 0, sizeof(toifname));
			       if(getifname(which_tmp, toifname) < 0)
			               break;
                 	       if ((relay_socket = listen_socket(INADDR_ANY, SERVER_PORT, toifname)) < 0) {
				 LOG(LOG_ERR, "FATAL: couldn't create relay socket, %s", strerror(errno));
			         exit_server(0);
                             }
 	            }while(0);
                  }
             }
             #endif
#ifdef STB_SUPPORT 
	        FD_ZERO(&w);
                FD_ZERO(&x);
#endif
		FD_ZERO(&rfds);
		FD_SET(server_socket, &rfds);
       #ifdef STB_SUPPORT
                if((server_config.stb_support == 1)&&(relay_socket>0))
                     FD_SET(relay_socket, &rfds);
        #endif
		
		FD_SET(signal_pipe[0], &rfds);
		
		if (server_config.auto_time) {
			tv.tv_sec = timeout_end - get_time(0);
			tv.tv_usec = 0;
		}
		if (!server_config.auto_time || tv.tv_sec > 0) {
 		#ifdef STB_SUPPORT
		      max_sock = server_socket > signal_pipe[0] ? server_socket : signal_pipe[0];
                      if((server_config.stb_support == 1)&&(relay_socket>0))
                          max_sock = max_sock > relay_socket ? max_sock : relay_socket;
                #else	
			max_sock = server_socket > signal_pipe[0] ? server_socket : signal_pipe[0];
               #endif
#ifdef STB_SUPPORT			
			retval = select(max_sock + 1, &rfds, &w, NULL, 
					server_config.auto_time ? &tv : NULL);
#else	
			retval = select(max_sock + 1, &rfds, NULL, NULL, 
					server_config.auto_time ? &tv : NULL);
#endif
		} else retval = 0; /* If we already timed out, fall through */

		if (retval == 0) {
			write_leases();
			timeout_end = get_time(0) + server_config.auto_time;
			continue;
		} else if (retval < 0 && errno != EINTR) {
			DEBUG(LOG_INFO, "error on select");
			continue;
		}
		
		if (FD_ISSET(signal_pipe[0], &rfds)) {
			if (read(signal_pipe[0], &sig, sizeof(sig)) < 0)
				continue; /* probably just EINTR */
			switch (sig) {
			case SIGUSR1:
				LOG(LOG_INFO, "Received a SIGUSR1");
				write_leases();
				/* why not just reset the timeout, eh */
				timeout_end = get_time(0) + server_config.auto_time;
				continue;
			case SIGUSR2:
				 LOG(LOG_INFO, "Received a SIGUSR2");
				 delete_leases();
				 continue;
			case SIGTERM:
				LOG(LOG_INFO, "Received a SIGTERM");
//				delete_route_by(get_cur_share_wanip(NULL));
				exit_server(0);
			}
		}
#ifdef STB_SUPPORT
               if((relay_socket>0)&&(server_config.stb_support == 1))
                 {
                  if (FD_ISSET(relay_socket, &rfds))
                    {
                        char *vendor;
                        if ((bytes = get_packet(&packet, relay_socket)) < 0) {
                           if (bytes == -1 && errno != EINTR) {
                                 DEBUG(LOG_INFO, "error on read, %s, reopening socket", strerror(errno));
                                 close(relay_socket);
		                 relay_socket = -1;
                               }
                                continue;
                          }
                        if(packet.op == BOOTREPLY)
	                  {
                                char *message_type = get_option(&packet, DHCP_MESSAGE_TYPE);
                                u_int32_t to_addr;
                                char *stb_subnet_mask = get_option(&packet,DHCP_SUBNET);
                                struct stb_info *tmp;
                                if(message_type[0] == DHCPACK)
	                           {
                                 struct stb_info *tmpstb = lookup_stbinfo(packet.xid);
                                 int i;
                                 if(tmpstb){


                                   tmp=tmpstb;
				   tmpstb->mac[0]=packet.chaddr[0];
				   tmpstb->mac[1]=packet.chaddr[1];
				   tmpstb->mac[2]=packet.chaddr[2];
				   tmpstb->mac[3]=packet.chaddr[3];
				   tmpstb->mac[4]=packet.chaddr[4];
				   tmpstb->mac[5]=packet.chaddr[5];
                                  // cprintf("1\n");
                                   memcpy(&(tmpstb->netmsk),stb_subnet_mask,4);
                                  // cprintf("2\n");
                                   memcpy(&(tmpstb->ipaddr),&(packet.yiaddr),4);
                                  // cprintf("3\n");
                                   stbnet2kernel(tmp);
                                   cprintf("4\n");
                                   stbinfo2kernel(STB2KER_ADD,tmp ,packet.chaddr);
                                   cprintf("5\n");
				   //by songtao 1011
				   //strcpy(tmpstb->mac,packet.chaddr);
                                  }
                                  else if(tmpstb==NULL)
                                    cprintf("tmpstb it's NULL!\n");
                                }
                                packet.hops--;
                                inet_aton("255.255.255.255",&(to_addr));
			        cprintf("relay packet!!!!\n");
			        if(relay_send(server_socket, &packet, to_addr, CLIENT_PORT) < 0)
                                       printf("relay_send fail\n");
                             }
                           continue;
                      }
                  }
 #endif
	       
				
		if ((bytes = get_packet(&packet, server_socket)) < 0) { /* this waits for a packet - idle */
			if (bytes == -1 && errno != EINTR) {
				DEBUG(LOG_INFO, "error on read, %s, reopening socket", strerror(errno));
				close(server_socket);
				server_socket = -1;
			}
			continue;
		}
#ifdef STB_SUPPORT
               if ((packet.op == BOOTREQUEST) && (vendor = get_option(&packet, DHCP_VENDOR))) {
                       if(server_config.stb_support == 1)
	                   {
	                       int found = 0, i;
                               for (i=0; i<MAXSTBTYPE; i++)
                                {
                                        cprintf("relay stbmatchtypes=%s\n",stbmatchtypes[i]);
                           //             cprintf("relay vendor=%s\n",vendor);
                                  //if (strstr(vendor,stbmatchtypes[i]))
                                 if (vendor[OPT_LEN - 2] == (unsigned char) strlen(stbmatchtypes[i]) &&  !strncmp(vendor, stbmatchtypes[i], vendor[OPT_LEN - 2]))
	    					
				    {
                                        DEBUG(LOG_INFO, "found stb device %s, forcing relay",stbmatchtypes[i]);
                                        cprintf("relay stbmatchtypes=%s\n",stbmatchtypes[i]);
                                        found = 1;
                                        break;
                                     }
                                }

                                if(found)
	                            {
                                        char *message_type = get_option(&packet, DHCP_MESSAGE_TYPE);
                                        char *router=get_option(&packet,DHCP_ROUTER);
                                        char *serverid=get_option(&packet,DHCP_SERVER_ID);
                                        if(message_type[0] == DHCPREQUEST)//DHCPDISCOVER
	                                 {
                                               // struct stb_info *tmpstb = get_new_stbinfo();
                                               //by songtao 1012
						 int exist=0;
						 struct stb_info *tmpstb = get_new_stbinfo(packet.chaddr);
						exist = tmpstb->existed;
                                                tmpstb->xid = packet.xid;
						if(exist == 1)
                                                    stbinfo2kernel(STB2KER_DEL,tmpstb,packet.chaddr);
						   
                                                cprintf("tmpstb->xid=%d,packet.xid=%d \n",tmpstb->xid,packet.xid);
                                         }
           /*                             else if(message_type[0] == DHCPRELEASE)
                                         {
                                                struct stb_info *tmpstb = lookup_stbinfo(packet.xid);
						 // by songtao 1011
                                              //  struct stb_info *tmpstb = lookup_stbinfo(packet.xid,packet.ciaddr,packet.chaddr);
                                                if(tmpstb){
                                                    stbinfo2kernel(STB2KER_DEL,tmpstb,packet.chaddr);
                                                    //stbinfo_del(packet.xid);
                                                    //by songtao 1011
						    stbinfo_del(tmpstb->xid);
                                                  }
						else
					            cprintf("dhcprelease fail!\n");	
                                         }  */
					
                                        if(!_getwanip(which_tmp))
                                               cprintf("gateway no active!!");
                                        cprintf("gateway =%s!!",wanipaddr);
                                        inet_aton(wanipaddr,&(packet.giaddr));
                                        packet.hops++;
                                        if(relay_send(relay_socket, &packet, server_config.stb_server, SERVER_PORT) < 0)
                                                cprintf("relay_send fail\n");

                                        continue;
                                   }

                            }
                   }
        #endif
	       
////////////////////////////////////////////////////////////////////////////////////////////////////////////							
		if ((state = get_option(&packet, DHCP_MESSAGE_TYPE)) == NULL) {
			DEBUG(LOG_ERR, "couldn't get option from packet, ignoring");
			continue;
		}
		
		/* ADDME: look for a static lease */
		lease = find_lease_by_chaddr(packet.chaddr);
		switch (state[0]) {
		case DHCPDISCOVER:
		        // start_dhcpd1();
			LOG(LOG_INFO,"1nUDHCPD:received DISCOVER from %s", ether_etoa(packet.chaddr, mac));	// modify by honor
			cprintf("received DISCOVER from %s\n", ether_etoa(packet.chaddr, mac));	// modify by honor
#if 1
			if (server_config.single_pc_mode)
			{
				cprintf("single pc mode \n");
				if (single_pc_mode_flag)
				{
					cprintf("leijun 1 \n");
					lease = find_lease_by_chaddr(single_pc_mac);
					if (lease)
					{
						cprintf("leijun 2\n");
						if (lease_expired(lease))
						{
							cprintf("leijun 3 \n");
							memset(single_pc_mac, 0, sizeof(single_pc_mac));
							single_pc_mode_flag = 0;
							cprintf("release the ip\n");
						}else if (memcmp(single_pc_mac, packet.chaddr, MAC_LEN)) break;
					}else if (memcmp(single_pc_mac, packet.chaddr, MAC_LEN)) 
{
	cprintf("hahha1\n");
	break;
}
				}
			}
#endif
			if (sendOffer(&packet) < 0) {
				LOG(LOG_ERR, "1nUDHCPD:send OFFER failed");
				break;
			}
#if 1
			//leijun single pc mode;
			if (server_config.single_pc_mode)
			{
				memcpy(&single_pc_mac[0], packet.chaddr, MAC_LEN);
				single_pc_mode_flag = 1;
			}
#endif
			break;			
 		case DHCPREQUEST:
			LOG(LOG_INFO, "1nUDHCPD:received REQUEST from %s", ether_etoa(packet.chaddr, mac));	// modify by honor
			cprintf("received REQUEST from %s\n", ether_etoa(packet.chaddr, mac));	// modify by honor

			send_nak_flag = 0;
			requested = get_option(&packet, DHCP_REQUESTED_IP);
			server_id = get_option(&packet, DHCP_SERVER_ID);
			name = get_option(&packet, DHCP_HOST_NAME);
			memset(hostname,0 ,sizeof(hostname));
			
			/////////////////////////////////
			if (name)
			{
				bytes = name[-1];
				if (bytes >= 64)
					bytes = 63;
				memset((void *)hostname, 0, sizeof(hostname));
				memcpy(hostname, name, bytes);
				hostname[bytes] = 0;
			}
			else strcpy(hostname, "");
//////////////////////////////////
			if (requested) memcpy(&requested_align, requested, 4);
			if (server_id) memcpy(&server_id_align, server_id, 4);
			
			//share wan ip function
#if 1 //leijun -- 20040608
#if 1
			if (server_config.single_pc_mode)
			{
				if (single_pc_mode_flag)
				{
					if (memcmp(single_pc_mac, packet.chaddr, MAC_LEN))
					{
					cprintf("hahah2\n");
//   		    			memset(lease,0, sizeof(struct dhcpOfferedAddr));
   		    			sendNAK(&packet);
						break;
					}
				}
			}
#endif
			//fnd the share wan ip by mac, if <= 0 ,send the ip, or ,send nak,
			if (server_config.share_wanip_flag)
			{
#if 1
				if (!requested) tmp_old_wanip = packet.ciaddr;
				else tmp_old_wanip = requested_align;
				if (tmp_old_wanip)
				{
					if (!((tmp_old_wanip >= server_config.start) && (tmp_old_wanip <= server_config.end)))
					{
						delete_route_by(tmp_old_wanip);
					}
				}
#endif
				if (find_share_wan_ip(packet.chaddr, &tmp_share_wanip) > 0)
				{
				//have wan ip
//				cprintf("share wan ip = %ld, request ip = %ld\n", tmp_share_wanip, requested_align);
					if (tmp_share_wanip != tmp_old_wanip)
					{
//						cprintf("&&&&&&&&&&send nak now &&&&&&&&&&\n");
						sendNAK(&packet);
						clear_share_wanip();
						single_pc_mode_flag = 0;
						memset(single_pc_mac, 0, sizeof(single_pc_mac));
//   		    			memset(lease,0, sizeof(struct dhcpOfferedAddr));
					
						break;
					}
				}
			}
#endif
//			cprintf("hello\n");
			if (lease) 
			{ /*ADDME: or static lease */
                                
				if (server_id) {
					/* reply Offer */
					/* SELECTING State */
					DEBUG(LOG_INFO, "server_id = %08x", ntohl(server_id_align));
					if (server_id_align == server_config.server && requested && 
					    requested_align == lease->yiaddr) {
						sendACK(&packet, lease->yiaddr);
					}
				} else {
					if (requested) {
						/* INIT-REBOOT State */
						if (lease->yiaddr == requested_align)
						{
							 //guohong
						    if ((j=is_in_machost_table(requested_align,&i)))
						    {
						    	    if (j== 2) // find in mac-ip table
						    	    {
						    	        if (!memcmp(packet.chaddr,mac_ip[i].macaddr,MAC_LEN))
						    	                sendACK(&packet, lease->yiaddr);
						    	    	else
						    	    		{
						    	    		     memset(lease,0 ,sizeof(struct dhcpOfferedAddr));
						    	    		     sendNAK(&packet);
						    	    		}
						    	     }
						            if (j== 1) // find in hostname-ip table
						            {
						    	    	if (!memcmp(hostname,hostname_ip[i].hostname,strlen(hostname)))
						    	    		sendACK(&packet, lease->yiaddr);
						    	    	else
						    	    		{
						    	    		   memset(lease,0 ,sizeof(struct dhcpOfferedAddr));
						    	    		   sendNAK(&packet);
						    	    		}
						    	     }
						    }	     
						    else
							sendACK(&packet, lease->yiaddr);
						}
						else sendNAK(&packet);
					} else {
						/* RENEWING or REBINDING State */
						
						/* RENEWING or REBINDING State */
						if (lease->yiaddr == packet.ciaddr)
						{
                   if ((j=is_in_machost_table(packet.ciaddr,&i)))
						       {
						    	    if (j== 2) // find in mac-ip table
						    	    {
						    	    	
						    	    	//cprintf("renew find in mac i=%d\n",i);
						    	    	if (!memcmp(packet.chaddr,mac_ip[i].macaddr,MAC_LEN))
						    	    		{
						    	    		    
						    	    		    sendACK(&packet, lease->yiaddr);
						    	    		}
						    	    	else
						    	    		{
						    	    		     
						    	    		     memset(lease,0, sizeof(struct dhcpOfferedAddr));
						    	    		     sendNAK(&packet);
						    	    		}
						    	     }
						    	    if (j== 1) // find in hostname-ip table
						    	    {
//cprintf("renew find in hostname=%s len=%d i=%d\n",hostname,i,strlen(hostname));
						    	       if (hostname && 
						    	       	           !memcmp(hostname,hostname_ip[i].hostname,strlen(hostname)))
						    	        	{
						    	    	     
						    	    		        sendACK(&packet, lease->yiaddr);
						    	       	}
						    	    	     else
						    	    		{
						    	    		     
						    	    		     memset(lease,0 ,sizeof(struct dhcpOfferedAddr));
						    	    		     sendNAK(&packet);
						    	    		}
						    	     }
						    }

						    else
						          sendACK(&packet, lease->yiaddr);
					      }
							
						else {
							/* don't know what to do!!!! */
							sendNAK(&packet);
						}
					}						
				}
				
				if (name) {
					bytes = name[-1];
					if (bytes >= (int) sizeof(lease->hostname))
						bytes = sizeof(lease->hostname) - 1;
					strncpy(lease->hostname, name, bytes);
					lease->hostname[bytes] = '\0';
				} else
					lease->hostname[0] = '\0';
			
			/* what to do if we have no record of the client */
			} else if (server_id) {
				/* SELECTING State */
				sendNAK(&packet);       // by honor

			} else if (requested) {
				/* INIT-REBOOT State */
				if ((lease = find_lease_by_yiaddr(requested_align))) {
					if (lease_expired(lease)) {
						/* probably best if we drop this lease */
						memset(lease->chaddr, 0, 16);
					/* make some contention for this address */
					} else sendNAK(&packet);
				} else if (requested_align < server_config.start || 
					   requested_align > server_config.end) {
					sendNAK(&packet);
				} else {
					sendNAK(&packet);
				}

			} else if (packet.ciaddr) {
				/* RENEWING or REBINDING State */
				sendNAK(&packet);
			}
			if (server_config.single_pc_mode)
			{
				if (send_nak_flag)
				{
					single_pc_mode_flag = 0;
					memset(single_pc_mac, 0, sizeof(single_pc_mac));
				}
			}
			break;
		case DHCPDECLINE:
			LOG(LOG_INFO,"received DECLINE from %s", ether_etoa(packet.chaddr, mac));	// modify by honor
			cprintf("received DECLINE from %s\n", ether_etoa(packet.chaddr, mac));	// modify by honor
			if (lease) {
				memset(lease->chaddr, 0, 16);
				lease->expires = get_time(0) + server_config.decline_time;
			}
			if (server_config.single_pc_mode)
			{
				memset(single_pc_mac, 0, sizeof(single_pc_mac));
				single_pc_mode_flag = 0;
			}
			if (server_config.share_wanip_flag)
			{
				if (check_is_share_wanip_mac(packet.chaddr))
					clear_share_wanip();
			}
			break;
			
		case DHCPRELEASE:
			LOG(LOG_INFO,"1nUDHCPD:received RELEASE from %s", ether_etoa(packet.chaddr, mac));	// modify by honor
			cprintf("received RELEASE from %s\n", ether_etoa(packet.chaddr, mac));	// modify by honor
#ifdef HSIAB_SUPPORT // 2003-08-15 by honor
			ipaddr.s_addr = lease->yiaddr;
			snprintf(message, sizeof(message), "LOG send_status_dhcp release from %s assigned %s", inet_ntoa(ipaddr), ether_etoa(lease->chaddr, mac));
		        send_command(message, NULL);
#endif
			if (lease) lease->expires = get_time(0);
			
			if (server_config.single_pc_mode)
			{
				memset(single_pc_mac, 0, sizeof(single_pc_mac));
				single_pc_mode_flag = 0;
			}
			if (server_config.share_wanip_flag)
			{
				if (check_is_share_wanip_mac(packet.chaddr))
					clear_share_wanip();
			}
			break;
		case DHCPINFORM:
			LOG(LOG_INFO,"1nUDHCPD:received INFORM from %s", ether_etoa(packet.chaddr, mac));	// modify by honor
//			cprintf("received INFORM from %s\n", ether_etoa(packet.chaddr, mac));	// modify by honor
			send_inform(&packet);
			break;	
		default:
			LOG(LOG_WARNING, "1nUDHCPD:unsupported DHCP message (%02x) -- ignoring", state[0]);
		}
	}

	return 0;
}

