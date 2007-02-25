/* serverpacket.c
 *
 * Constuct and send DHCP server packets
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <syslog.h>
#include "packet.h"
#include "debug.h"
#include "dhcpd.h"
#include "options.h"
#include "leases.h"
#include "get_time.h"

#include <shutils.h>
#ifdef HSIAB_SUPPORT
extern char *send_command(char *send_value, char *get_value);
#endif

/* da_f 2005.6.21 add */
#define cprintf(str, arg...)

//zhangbin-2004.08.05
#define DEBUG(...)

extern int mac_ip_count;
extern int hostname_ip_count;
extern int send_nak_flag;
extern int single_pc_mode_flag;

/* send a packet to giaddr using the kernel ip stack */
static int send_packet_to_relay(struct dhcpMessage *payload)
{
	DEBUG(LOG_INFO, "Forwarding packet to relay");

	return kernel_packet(payload, server_config.server, SERVER_PORT,
			payload->giaddr, SERVER_PORT);
}


/* send a packet to a specific arp address and ip address by creating our own ip packet */
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast)
{
	unsigned char *chaddr;
	u_int32_t ciaddr;
	
	if (force_broadcast) {
		DEBUG(LOG_INFO, "broadcasting packet to client (NAK)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else if (payload->ciaddr) {
		DEBUG(LOG_INFO, "unicasting packet to client ciaddr");
		ciaddr = payload->ciaddr;
		chaddr = payload->chaddr;
	} else if (ntohs(payload->flags) & BROADCAST_FLAG) {
		DEBUG(LOG_INFO, "broadcasting packet to client (requested)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else {
		DEBUG(LOG_INFO, "unicasting packet to client yiaddr");
		ciaddr = payload->yiaddr;
		chaddr = payload->chaddr;
	}
	return raw_packet(payload, server_config.server, SERVER_PORT, 
			ciaddr, CLIENT_PORT, chaddr, server_config.ifindex);
}


/* send a dhcp packet, if force broadcast is set, the packet will be broadcast to the client */
static int send_packet(struct dhcpMessage *payload, int force_broadcast)
{
	int ret;

	if (payload->giaddr)
		ret = send_packet_to_relay(payload);
	else ret = send_packet_to_client(payload, force_broadcast);
	return ret;
}


static void init_packet(struct dhcpMessage *packet, struct dhcpMessage *oldpacket, char type)
{
	init_header(packet, type);
	packet->xid = oldpacket->xid;
	memcpy(packet->chaddr, oldpacket->chaddr, 16);
	packet->flags = oldpacket->flags;
	packet->giaddr = oldpacket->giaddr;
	packet->ciaddr = oldpacket->ciaddr;
	add_simple_option(packet->options, DHCP_SERVER_ID, server_config.server);
}


/* add in the bootp options */
static void add_bootp_options(struct dhcpMessage *packet)
{
	packet->siaddr = server_config.siaddr;
	if (server_config.sname)
		strncpy(packet->sname, server_config.sname, sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		strncpy(packet->file, server_config.boot_file, sizeof(packet->file) - 1);
}
	

/* send a DHCP OFFER to a DHCP DISCOVER */
int sendOffer(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	struct dhcpOfferedAddr *lease = NULL;
	u_int32_t req_align, lease_time_align = server_config.lease;
	u_int32_t tmp_lease = 0;
	unsigned char *req, *lease_time;
	struct option_set *curr;
	struct in_addr addr;
	u_int32_t ipaddr;
	char hostname[64];	
	char *name;
	int bytes;

  int i,j;

	//wwzh 	
	memset(hostname,0 ,sizeof(hostname));
	name = get_option(oldpacket, DHCP_HOST_NAME);
	if (name)
	{
		bytes = name[-1];
		if (bytes >= 64)
			bytes = 63;
		memset((void *)hostname, 0, sizeof(hostname));
		memcpy(hostname, name, bytes);
		hostname[bytes] = 0;
	}
	else
		strcpy(hostname, " ");
	
	init_packet(&packet, oldpacket, DHCPOFFER);
	/*
        cprintf(" in offer hostname = %s\n ",hostname);
        for (i =0 ; i<6; i++)
        	{
                 cprintf ("chaddr[%d]=%02x ,",i ,oldpacket->chaddr[i]);
                 

        	}

        cprintf("\n");
        for(i=0; i<mac_ip_count; i++)
                  cprintf("mac%d: %02x:%02x:%02x:%02x:%02x:%02x  ipaddr = %x\n", i,
			mac_ip[i].macaddr[0], mac_ip[i].macaddr[1],mac_ip[i].macaddr[2],
			mac_ip[i].macaddr[3],mac_ip[i].macaddr[4],mac_ip[i].macaddr[5],
			mac_ip[i].ipaddr);
        for(i=0; i<hostname_ip_count; i++)
                  cprintf("hostname%d: %s  ipaddr = %x\n", i,
			hostname_ip[i].hostname,
			hostname_ip[i].ipaddr);
	*/		
//    cprintf("in send offer2\n");    
	// first check the mac is share wan ip.
	if (server_config.share_wanip_flag)
	{
		i = find_share_wan_ip(oldpacket->chaddr, &ipaddr);
	}else i = -1;

	if (i > 0) packet.yiaddr = ipaddr; //get wan ip by mac
	else if (i == 0) goto find_ip;//no wan ip, give it a lan ip
	//not the share wan ip mac
	else if((ipaddr = find_ip_by_mac(oldpacket->chaddr)) )
	{
		//wwzh add for mac & ip pair
		packet.yiaddr = ipaddr;
//		cprintf(" in dhcp sendoffer : find ip = %x by mac\n",ipaddr);
	}else if ((ipaddr = find_ip_by_hostname(hostname)) ){
		//wwzh add for host & ip 
		packet.yiaddr = ipaddr;
//		cprintf(" in dhcp sendoffer : find ip = %x by hosrname\n",ipaddr);
	/* Or the client has a requested ip */
	}
		/* ADDME: if static, short circuit */
	/* the client is in our lease/offered table */
	else if ((lease = find_lease_by_chaddr(oldpacket->chaddr))) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"(1)the client is in our lease/offered table");
	
#endif
//		printf(" in dhcp sendoffer :the client is in our lease/offered table ");
		if(!check_ip(lease->yiaddr)){	// Confirm the ip isn't used by someone, by honor
			if (!lease_expired(lease)) 
				lease_time_align = lease->expires - get_time(0);
			packet.yiaddr = lease->yiaddr;
		}
		else
			goto find_ip;
	} else if ((req = get_option(oldpacket, DHCP_REQUESTED_IP)) &&

		   /* Don't look here (ugly hackish thing to do) */
		   memcpy(&req_align, req, 4) &&

		   /* and the ip is in the lease range */
		   ntohl(req_align) >= ntohl(server_config.start) &&
		   ntohl(req_align) <= ntohl(server_config.end) &&
		   
		   /* and its not already taken/offered */ /* ADDME: check that its not a static lease */
		   ((!(lease = find_lease_by_yiaddr(req_align)) ||
		   
		   /* or its taken, but expired */ /* ADDME: or maybe in here */
		   lease_expired(lease)))) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"(2)the client has a requested ip");
#endif
		if(req_align == server_config.server)   // The request ip cann't be router ip, by honor
			goto find_ip;
		else if(!check_ip(req_align))		// Confirm the ip isn't used by someone, by honor
			packet.yiaddr = req_align; /* FIXME: oh my, is there a host using this IP? */
		else
			goto find_ip;

	/* otherwise, find a free IP */ /*ADDME: is it a static lease? */
	} else {
find_ip:	// by honor
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"(3)find a free IP");
#endif

		packet.yiaddr = find_address(0);
		
		/* try for an expired lease */
		if (!packet.yiaddr) packet.yiaddr = find_address(1);
	}
	
	if(!packet.yiaddr) {
		LOG(LOG_WARNING, "no IP addresses to give -- OFFER abandoned");
		return -1;
	}
	
	if (!add_lease(packet.chaddr, packet.yiaddr, server_config.offer_time)) {
		LOG(LOG_WARNING, "lease pool is full -- OFFER abandoned");
		return -1;
	}		

	if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
		if (lease_time_align > server_config.lease) 
			lease_time_align = server_config.lease;
	}

	/* Make sure we aren't just using the lease time from the previous offer */
	if (lease_time_align < server_config.min_lease) 
		lease_time_align = server_config.lease;
	/* ADDME: end of short circuit */		
#if 1
	//add by leijun
	if (tmp_lease = check_is_share_wanip_mac(oldpacket->chaddr))
	{
		lease_time_align = tmp_lease;
	}
	////////
#endif
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
		{
#if 1
			if (server_config.share_wanip_flag)
			{
				if (curr->data[OPT_CODE] == DHCP_ROUTER)
				{
					u_int32_t tmp_gwaddr = 0;
					tmp_gwaddr = get_share_wanip_gw(oldpacket->chaddr);
					if (tmp_gwaddr > 0)
					{
						struct option_set *tmp_new;
						tmp_new = malloc(sizeof(struct option_set));
						tmp_new->data = malloc(4 + 2);
						tmp_new->data[OPT_CODE] = DHCP_ROUTER;
						tmp_new->data[OPT_LEN] = 4;
						memcpy(tmp_new->data + 2, &tmp_gwaddr, 4);
						add_option_string(packet.options, tmp_new->data);
						free(tmp_new);
						curr = curr->next;
						continue;
					}
				}
			}
#endif
			add_option_string(packet.options, curr->data);
		}
		curr = curr->next;
	}

	add_bootp_options(&packet);
	
	addr.s_addr = packet.yiaddr;
	LOG(LOG_INFO, "sending OFFER to %s", inet_ntoa(addr));
	cprintf("sending OFFER to %s\n", inet_ntoa(addr));
	//single pc mode leijun
	return send_packet(&packet, 1);		// Send broadcast rather than unicast, by honor
}

int sendNAK(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	char mac[32];

	init_packet(&packet, oldpacket, DHCPNAK);
	
	LOG(LOG_INFO, "sending NAK to %s", ether_etoa(packet.chaddr, mac));
	cprintf("sending NAK to %s\n", ether_etoa(packet.chaddr, mac));
	send_nak_flag = 1;
	return (send_packet(&packet, 1));
}

int sendACK(struct dhcpMessage *oldpacket, u_int32_t yiaddr)
{
	struct dhcpMessage packet;
	struct option_set *curr;
	unsigned char *lease_time;
	u_int32_t lease_time_align = server_config.lease;
	u_int32_t tmp_lease = 0;
	struct in_addr addr;
#ifdef HSIAB_SUPPORT
	char message[254];
	char mac[32];
	struct in_addr ipaddr;
#endif

	u_int32_t tmp_share_wanip; //add by leijun
	init_packet(&packet, oldpacket, DHCPACK);
	packet.yiaddr = yiaddr;
	
	if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
		if (lease_time_align > server_config.lease) 
			lease_time_align = server_config.lease;
		else if (lease_time_align < server_config.min_lease) 
			lease_time_align = server_config.lease;
	}
#if 1
	//add by leijun
	if (tmp_lease = check_is_share_wanip_mac(oldpacket->chaddr))
	{
		lease_time_align = tmp_lease;
	}
	////////	
#endif
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));
	
	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
		{
#if 1
			if (server_config.share_wanip_flag)
			{
				if (curr->data[OPT_CODE] == DHCP_ROUTER)
				{
					u_int32_t tmp_gwaddr = 0;
					tmp_gwaddr = get_share_wanip_gw(oldpacket->chaddr);
					if (tmp_gwaddr > 0)
					{
						struct option_set *tmp_new;
						tmp_new = malloc(sizeof(struct option_set));
						tmp_new->data = malloc(4 + 2);
						tmp_new->data[OPT_CODE] = DHCP_ROUTER;
						tmp_new->data[OPT_LEN] = 4;
						memcpy(tmp_new->data + 2, &tmp_gwaddr, 4);
						add_option_string(packet.options, tmp_new->data);
						free(tmp_new);
						curr = curr->next;
						continue;
					}
				}
			}
	#endif
			add_option_string(packet.options, curr->data);
		}
		curr = curr->next;
	}

	add_bootp_options(&packet);

	addr.s_addr = packet.yiaddr;
	
#ifdef HSIAB_SUPPORT   // 2003-05-13   by honor
	ipaddr.s_addr = packet.yiaddr;
	snprintf(message, sizeof(message), "LOG send_status_dhcp request from %s assigned %s", inet_ntoa(ipaddr), ether_etoa(packet.chaddr, mac));
	send_command(message, NULL);
        syslog(LOG_INFO,"1nDHCP:%s assigned",inet_ntoa(ipaddr));

#endif

	
	if(get_option(oldpacket, DHCP_SERVER_ID)){	// by honor
		LOG(LOG_INFO, "sending ACK to %s", "255.255.255.255");
		cprintf("sending ACK to %s\n", "255.255.255.255");
		if (send_packet(&packet, 1) < 0) return -1;
	}
	else{
		LOG(LOG_INFO, "sending ACK to %s", inet_ntoa(addr));
		cprintf("sending ACK to %s\n", inet_ntoa(addr));
                syslog(LOG_INFO,"1nDHCP:%s assigned",inet_ntoa(addr));
		
		if (send_packet(&packet, 0) < 0) return -1;
	}

	add_lease(packet.chaddr, packet.yiaddr, lease_time_align);
	//if a share wanip, then write the ipaddr to the specical address. and add a route.
	//have problems
	if (server_config.share_wanip_flag)
	{
		if (find_share_wan_ip(packet.chaddr, &tmp_share_wanip) > 0)
		{
			if (tmp_share_wanip == packet.yiaddr)
			{
				SendInfoToKernal(packet.yiaddr, packet.chaddr);
			}
		}
	}
	if (server_config.single_pc_mode)
	{
		struct in_addr tmp_addr;		
		char tmp_buff[32];		
		tmp_addr.s_addr = packet.yiaddr;		
		sprintf(tmp_buff, "%s", inet_ntoa(tmp_addr));		
		eval("iptables", "-F");		
		eval("iptables", "-A", "FORWARD", "-i", "br0", "-s", tmp_buff, "-j", "ACCEPT");		
		eval("iptables", "-A", "FORWARD", "-i", "br0", "-j", "DROP");	
		single_pc_mode_flag = 2;
		cprintf("add iptables addr = %s\n", tmp_buff);
		nvram_set("single_pc_ipaddr", tmp_buff); 
	}
	return 0;
}


int send_inform(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	struct option_set *curr;

	init_packet(&packet, oldpacket, DHCPACK);
	
	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

	add_bootp_options(&packet);

	return send_packet(&packet, 0);
}



