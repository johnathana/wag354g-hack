#include <net/if.h>
#include <errno.h>
#include <features.h>
#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <bcmnvram.h>
#include "ArpLanIp.h"


int ArpLanIpbyDhcp(void);

/*-----------------------------------------------------------------
 args:	yiaddr - what IP to ping
 *		ip - our ip
 *		mac - our arp address
 *		interface - interface to use
 * retn: 	1 addr free
 *		0 addr used
 *		-1 error 
 ---------------------------------------------------------------*/  
static int SendARP(u_int32_t yiaddr, u_int32_t ip, unsigned char *mac, char *interface)
{

	int	timeout = 1;		// we need to reduce time
	int 	optval = 1;
	int	s,datalen=0;			/* socket */
	int	rv = 1;			/* return value */
	struct sockaddr addr;		/* for interface name */
	struct arpMsg	arp;
	fd_set		fdset;
	struct timeval	tm;
	time_t		prevTime;

	//if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) {
	if ((s = socket(PF_INET,SOCK_PACKET,htons(ETH_P_ARP))) == -1){	
		printf("Could not open raw socket\n");
		return -1;
	}
	
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		printf("Could not setsocketopt on raw socket\n");
		close(s);
		return -1;
	}

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	//memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);	/* MAC DA */
	memset(arp.ethhdr.h_dest,0xff,6);
	memcpy(arp.ethhdr.h_source, mac, 6);		/* MAC SA */
	arp.ethhdr.h_proto = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);		/* ARP op code */
	*((u_int *) arp.sInaddr) = ip;			/* source IP address */
	memcpy(arp.sHaddr, mac, 6);			/* source hardware address */
	*((u_int *) arp.tInaddr) = yiaddr;		/* target IP address */
	
	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, interface);

	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
	{
		rv = 0;
		printf("sock send to target error\n");
	}

	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	time(&prevTime);

	while (timeout > 0) {

		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec = timeout;

		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0) {
			printf("Error %d on ARPING request: %s\n", errno,strerror(errno));
			if (errno != EINTR) rv = 0;
		} else if (FD_ISSET(s, &fdset)) {
			printf("start receive arp reply\n");
			if (recv(s, &arp, sizeof(arp), 0) < 0 ) 
			{
				rv = 0;
				printf("not received arp reply\n");	
			}
			printf("operation=%d,hardware addr=%s,ip=%ld\n",arp.operation,arp.tHaddr,arp.sInaddr);
			if (arp.operation == htons(ARPOP_REPLY) && 
			    bcmp(arp.tHaddr, mac, 6) == 0 && 
			    *((u_int *) arp.sInaddr) == yiaddr) {
				printf("Valid arp reply receved for this address\n");
				rv = 0;
				break;
			}else
				printf("no match source ip with destination ip\n");
		}else
			printf("not arp reply\n");

		timeout -= time(NULL) - prevTime;
		time(&prevTime);
	}
	close(s);
	printf("send arp request for ip over\n");
	return rv;
}

/* check is an IP is taken, if it is, add it to the lease table */
static int check_ip(u_int32_t addr, u_int32_t ip, unsigned char *mac, char *interface)
{
	struct in_addr temp;
	int receiveflag=0;
	
	receiveflag = SendARP(addr, ip, mac, interface);
	if (receiveflag == 0) 
	{
		temp.s_addr = addr;
	 	printf("%s belongs to someone, then search another...\n",inet_ntoa(temp));
		return 1;
	} else if (receiveflag == -1)
	{
		printf("Arp socket error!\n");	
		return 1;
	}else{ 
		temp.s_addr = addr;
		printf("success to get free lan ip addr=%s\n",inet_ntoa(temp));
		return 0;	
	}	

}
//--------------------------------------------------------------------
//int CheckIpSame()
//--------------------------------------------------------------------------
int SetLanIpbyDhcp(void)
{
	u_int32_t start_addr,server_addr,addr;
	int i,ip_num;
	char lan_ifname[6];
	unsigned char ipaddr[20],ArpMacAddr[6];
	int fd;
	struct ifreq ifr;
	struct sockaddr_in *our_ip;

	memset((void *)lan_ifname, 0, sizeof(lan_ifname));
	strcpy(lan_ifname, nvram_safe_get("lan_ifname"));
	strcpy(ipaddr,nvram_safe_get("lan_ipaddr"));

	printf("set lan ip by dhcp setting:%s,interface=%s\n",nvram_safe_get("lan_proto"),lan_ifname);

	if (nvram_match("lan_proto","dhcp"))
	{
		//if dhcp enable,then set lan ip addr 192.168.1.254;
		printf("dhcp enable;then set lan ip 192.168.1.254\n");
		//strcpy(ipaddr,nvram_safe_get("lan_ipaddr"));
		if (strcmp(ipaddr,DEFAULT_LAN_IPADDR))
		{
			nvram_set("lan_ipaddr",DEFAULT_LAN_IPADDR);
			return 1;
		}else
			return 0;
	}else if (nvram_match("lan_proto","static"))
	{	
		//if dhcp disable,then ARP ip address from 192.168.1.253 to 192.168.1.64
		printf("dhcp disable,then arp ip addr from 253 to 64\n");
		memset(&ifr, 0, sizeof(struct ifreq));
		if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) 
		{
			ifr.ifr_addr.sa_family = AF_INET;
			strcpy(ifr.ifr_name, lan_ifname);	
			//get current our ip address;
			if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) 
			{
				our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
				server_addr = our_ip->sin_addr.s_addr;
				printf("our ip addr=%s\n",inet_ntoa(our_ip->sin_addr));
			}
			//get mac address;
			if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) 
			{
				memcpy(ArpMacAddr, ifr.ifr_hwaddr.sa_data, 6);
				printf("adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x\n",ArpMacAddr[0],
			 		ArpMacAddr[1], ArpMacAddr[2], ArpMacAddr[3], ArpMacAddr[4], ArpMacAddr[5]);
			}
		}	
		close(fd);

		//get ip address;	
		start_addr = inet_addr(DEFAULT_LAN_IPADDR);
		start_addr = ntohl(start_addr);
		
		for (i=0;i<190;i++)	
		{
			start_addr--;
			/* ie, 192.168.1.0 */
			if (!(start_addr & 0xFF)) continue;

			/* ie, 192.168.1.255 */
			if ((start_addr & 0xFF) == 0xFF) continue;	
		
			addr = htonl(start_addr);
			if (check_ip(addr,server_addr,ArpMacAddr, lan_ifname) == 0)
			{
				our_ip->sin_addr.s_addr = addr;
				if (strcmp(ipaddr,inet_ntoa(our_ip->sin_addr)))
				{
					nvram_set("lan_ipaddr",inet_ntoa(our_ip->sin_addr));
					return 1;	
				}else
					return 0;
			}
		}
		return -1;
	}	
}
/*
int main(void)
{
	SetLanIpbyDhcp();
	return 1;
}
*/

