/*
 * autodetect.c - Wan PVC autodetction
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <linux/icmp.h>
#include <bcmnvram.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <shutils.h>
#include "autodetect.h"
#include "pvc.h"

extern int which_conn;
extern struct vcc_config *vcc_config_ptr;
extern struct pppoe_config *pppoe_config_ptr;
extern struct pppoa_config *pppoa_config_ptr;
extern struct bridged_config *bridged_config_ptr;
extern struct routed_config *routed_config_ptr;
#ifdef CLIP_SUPPORT
extern struct clip_config *clip_config_ptr;
#endif

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
struct unip_pppoe_config *unip_pppoe_config_ptr;
struct unip_pppoa_config *unip_pppoa_config_ptr;
#endif

//#define IfName(N) nasN
#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

void sendPacket(int sock, struct PPPoEPacket *pkt, int size);
void receivePacket(int sock, struct PPPoEPacket *pkt, int *size);
void sendPADI(void);
int waitForPADO(int timeout);
int openInterface(const char *ifname, unsigned short type, unsigned char *hwaddr);
int in_cksum(unsigned short *addr,int len);
void udpipgen(udpiphdr *udpip,  unsigned int saddr, unsigned int daddr, 
	      unsigned short sport, unsigned short dport,
	      unsigned short msglen);
void buildDhcpDiscover();
int waitForOFFER(int timval);
int ifconfig(char *name, int flags, char *addr, char *netmask);

unsigned char BroadcastAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char MyEthAddr[6];
unsigned short Eth_PPPOE_Discovery = 0x8863;
int DiscoverySocket = -1;

int dhcpSocket = -1;
unsigned int xid = 0xEF;
unsigned short dhcpMsgSize;
unsigned int magic_cookie;

dhcpOptions    DhcpOptions;
udpipMessage   UdpIpMsgSend,UdpIpMsgRecv;

struct iphdr *ipSend=(struct iphdr *)((struct udpiphdr *)UdpIpMsgSend.udpipmsg)->ip;
struct udphdr *udpSend=(struct udphdr *)((char *)(((struct udpiphdr *)UdpIpMsgSend.udpipmsg)->ip) + sizeof(struct iphdr));
struct iphdr *ipRecv=(struct iphdr *)((struct udpiphdr *)UdpIpMsgRecv.udpipmsg)->ip;
struct udphdr *udpRecv=(struct udphdr *)((char *)(((struct udpiphdr *)UdpIpMsgRecv.udpipmsg)->ip) + sizeof(struct iphdr));
dhcpMessage *DhcpMsgSend = (dhcpMessage *)&UdpIpMsgSend.udpipmsg[sizeof(udpiphdr)];
dhcpMessage *DhcpMsgRecv = (dhcpMessage *)&UdpIpMsgRecv.udpipmsg[sizeof(udpiphdr)];
      
void sendPacket(int sock, struct PPPoEPacket *pkt, int size)
{
	struct sockaddr sa;
	char itfname[8];
	memset(itfname, 0, sizeof(itfname));
	sprintf(itfname, "nas%d", which_conn);
    	strcpy(sa.sa_data, itfname);
    	if(sendto(sock, pkt, size, 0, &sa, sizeof(sa)) < 0) 
	{
		printf("sendto (sendPacket)\n");
		exit(1);
    	}
}

void receivePacket(int sock, struct PPPoEPacket *pkt, int *size)
{
        if((*size = recv(sock, pkt, sizeof(struct PPPoEPacket), 0)) < 0) 
	{
		printf("recv (receivePacket)\n");
		exit(1);
    	}
}

void sendPADI(void)
{
	struct PPPoEPacket packet;
	struct PPPoETag *svc = (struct PPPoETag *) (&packet.payload);
	unsigned short plen = TAG_HDE_SIZE;
	int i;
	
	//junzhao 2004.10.27
	memset(&packet, 0, sizeof(struct PPPoEPacket));
	
	memcpy(packet.ethHdr.h_dest, BroadcastAddr, ETH_ALEN);
	memcpy(packet.ethHdr.h_source, MyEthAddr, ETH_ALEN);
	packet.ethHdr.h_proto = htons(Eth_PPPOE_Discovery);
	packet.ver = 1;
	packet.type = 1;
	packet.code = CODE_PADI;
	packet.session = 0;
	//packet.length = htons(plen);
	svc->type = TAG_SERVICE_NAME;

//junzhao 2004.10.27 add service name for autodetect
	if(!strcmp(e_servicename, ""))
	{
		packet.length = htons(plen);
		svc->length = 0;
	}
	else
	{
		svc->length = htons(strlen(e_servicename));
		packet.length = htons(plen + strlen(e_servicename));
		memcpy((void *)(svc->payload), (void *)(e_servicename), strlen(e_servicename));
		//add HOST_UNIQ tag
		/*
		svc = (struct PPPoETag *)(&(svc->payload[svc->length]));
		svc->type = htons(TAG_HOST_UNIQ);
		svc->length = sizeof(struct PPPoETag *);
		memcpy((void *)(svc->payload), &svc, sizeof(struct PPPoETag *));
		*/
	}
	
//	printf("sendPacket before!\n");
	//junzhao 2004.6.17


#ifdef VZ7
	for(i=0; i<5; i++)
	{
		usleep(20);
		if(!strcmp(e_servicename, ""))
			sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE));
		else
			sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(e_servicename) + plen + sizeof(struct PPPoETag *)));
			//sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(e_servicename)));
	}
#else
	if(!strcmp(e_servicename, ""))
		sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE));
	else
		//sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(e_servicename)));
		sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(e_servicename) + plen + sizeof(struct PPPoETag *)));
#endif
}

/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
void unip_sendPADI(void)
{
	struct PPPoEPacket packet;
	struct PPPoETag *svc = (struct PPPoETag *) (&packet.payload);
	unsigned short plen = TAG_HDE_SIZE;
	int i;
	
	//junzhao 2004.10.27
	memset(&packet, 0, sizeof(struct PPPoEPacket));
	
	memcpy(packet.ethHdr.h_dest, BroadcastAddr, ETH_ALEN);
	memcpy(packet.ethHdr.h_source, MyEthAddr, ETH_ALEN);
	packet.ethHdr.h_proto = htons(Eth_PPPOE_Discovery);
	packet.ver = 1;
	packet.type = 1;
	packet.code = CODE_PADI;
	packet.session = 0;
	//packet.length = htons(plen);
	svc->type = TAG_SERVICE_NAME;

//junzhao 2004.10.27 add service name for autodetect
       printf("unip_sendPADI: ue_servicename = %s\n",ue_servicename);
	if(!strcmp(ue_servicename, ""))
	{
		packet.length = htons(plen);
		svc->length = 0;
	}
	else
	{
		svc->length = htons(strlen(ue_servicename));
		packet.length = htons(plen + strlen(ue_servicename));
		memcpy((void *)(svc->payload), (void *)(ue_servicename), strlen(ue_servicename));
	}

#ifdef VZ7
	for(i=0; i<5; i++)
	{
		usleep(20);
		if(!strcmp(ue_servicename, ""))
			sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE));
		else
			sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(ue_servicename) + plen + sizeof(struct PPPoETag *)));
	}
#else
	if(!strcmp(ue_servicename, ""))
		sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE));
	else
		sendPacket(DiscoverySocket, &packet, (int)(plen + HDR_SIZE + strlen(ue_servicename) + plen + sizeof(struct PPPoETag *)));
#endif
}

#endif

int waitForPADO(int timeout)
{
	fd_set readable;
	int r;
	struct timeval tv;
	struct PPPoEPacket packet;
	int len;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	FD_ZERO(&readable);
	FD_SET(DiscoverySocket, &readable);
	
	//junzhao 2004.10.27
	memset(&packet, 0, sizeof(struct PPPoEPacket));
	
	while(1) 
	{
		r = select(DiscoverySocket+1, &readable, NULL, NULL, &tv);
		if(r < 0) 
		{
			printf("select (waitForPADO) fail\n");
			//return -1;
			continue;
		}
		if(r == 0) 
		{	
			printf("have time out---\n");			
			return -1;        /* Timed out */
		}
		if(!FD_ISSET(DiscoverySocket,&readable)) 
		{		
			printf("why other fd\n");		
			continue;		
		}
   
		/* Get the packet */
		receivePacket(DiscoverySocket, &packet, &len);
		/* Check length */
		if(ntohs(packet.length) + HDR_SIZE > len) 
		{
			printf("Bogus PPPoE length field");
			continue;
		}
		if(packet.code == CODE_PADO) 
		{		
			printf("catch PADO!\n");		
	    		break;
		}
	}
	return 0;
}

int openInterface(const char *ifname, unsigned short type, unsigned char *hwaddr)
{
	int optval = 1;
	int fd;
	struct ifreq ifr;
	struct sockaddr sa;
	memset(&sa, 0, sizeof(sa));
	if((fd = socket(AF_INET, SOCK_PACKET, htons(type))) < 0) 
	{
		printf("socket fail!\n");
		return -1;
    	}
	
	if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0) 
	{
		printf("setsockopt fail\n");
		return -1;
    	}
/*	
	ifr.ifr_flags = IFUP;	
	if (ioctl(fd,SIOCSIFFLAGS,&ifr))
	{		
		printf("ioctl SIOCSIFFLAGS fail\n");		
		return -1;
	}	  	      
*/
    /* Fill in hardware address */
	if(hwaddr) 
	{
		strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
		if(ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) 
		{
	    		printf("ioctl(SIOCGIFHWADDR) fail\n");
			return -1;
		}
		memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, sizeof(ifr.ifr_hwaddr.sa_data));
    	}

    /* Sanity check on MTU */
    	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    		if(ioctl(fd, SIOCGIFMTU, &ifr) < 0) 
		{
			printf("ioctl(SIOCGIFMTU) fail\n");
			return -1;
    		}

    	if(ifr.ifr_mtu < ETH_DATA_LEN) 		
		return -1;    	
	
	strcpy(sa.sa_data, ifname);

    /* We're only interested in packets on specified interface */
    	if(bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) 
	{
		printf("bind error\n");
		return -1;
    	}
    	return fd;
}

int in_cksum(unsigned short *addr,int len)
{
	register int sum = 0;	    
	register unsigned short  *w = addr;	      
	register int nleft = len;	       
       	while ( nleft > 1 )
	{				        
		sum += *w++;					        
		nleft -= 2;
	}		  
	if ( nleft == 1 )
	{				            
		u_char a = 0;					          
		memcpy(&a,w,1);						        
		sum += a;
	 }		    
	sum = (sum >> 16) + (sum & 0xffff);		      
	sum += (sum >> 16);		        
	return ~sum;
}

void udpipgen(udpiphdr *udpip,  unsigned int saddr, unsigned int daddr, 
	      unsigned short sport, unsigned short dport,
	      unsigned short msglen)
{
	  /* Use local copy because udpip->ip is not aligned.  */	  
	struct iphdr ip_local;	    
	struct iphdr *ip=&ip_local;	     
       	struct ipovly *io=(struct ipovly *)udpip->ip;       
	struct udphdr *udp=(struct udphdr *)udpip->udp;
	
	io->ih_next = io->ih_prev = 0;
	io->ih_x1 = 0;
	io->ih_pr = IPPROTO_UDP;
	io->ih_len = htons(msglen + sizeof(struct udphdr));

	io->ih_src.s_addr = saddr;
	io->ih_dst.s_addr = daddr;					
	
	udp->uh_sport = sport;			        
	udp->uh_dport = dport;				  
	udp->uh_ulen = htons(msglen + sizeof(struct udphdr));				udp->uh_sum = 0;				      
	udp->uh_sum = in_cksum((unsigned short *)udpip, msglen+sizeof(udpiphdr));				        
	if ( udp->uh_sum == 0 ) 
		udp->uh_sum = 0xffff;		
	
	ip->ihl = 5;					    
	ip->version = IPVERSION;					      
	ip->tos = 0;       /* normal service */			        
	ip->tot_len = htons(msglen + sizeof(udpiphdr));			
	ip->id = htons(1);						    		ip->frag_off = htons(0x4000);						
	ip->ttl = IPDEFTTL; /* time to live, 64 by default */	      
	ip->protocol = IPPROTO_UDP;					
	ip->saddr = saddr;						
	ip->daddr = daddr;
	ip->check = 0;						
	ip->check = in_cksum((unsigned short *)&ip_local,sizeof(struct iphdr));
	memcpy(udpip->ip, ip, sizeof(struct iphdr));
}

void buildDhcpDiscover()
{
	unsigned char *p;
	magic_cookie = htonl(MAGIC_COOKIE);
	dhcpMsgSize = htons(sizeof(struct dhcpMessage));
	/* build Ethernet header */
	memset(&UdpIpMsgSend,0, sizeof(udpipMessage));	  
	memcpy(UdpIpMsgSend.ethhdr.ether_dhost, BroadcastAddr, ETH_ALEN); 
	memcpy(UdpIpMsgSend.ethhdr.ether_shost, MyEthAddr, ETH_ALEN);	
	UdpIpMsgSend.ethhdr.ether_type = htons(ETHERTYPE_IP);
	DhcpMsgSend->op       =       DHCP_BOOTREQUEST;	
	DhcpMsgSend->htype    =       ARPHRD_ETHER;	
	DhcpMsgSend->hlen     =       ETH_ALEN;	
	DhcpMsgSend->xid      =       htonl(xid);		  
	DhcpMsgSend->secs     =       htons(3);		    
	DhcpMsgSend->flags  =       htons(BROADCAST_FLAG);		      
	memcpy(DhcpMsgSend->chaddr, MyEthAddr, ETH_ALEN);		        
	p = DhcpMsgSend->options;
	memcpy(p, &magic_cookie, 4);
	p += 4;	
	*p++ = dhcpMessageType;			    
	*p++ = 1;			      
	*p++ = DHCP_DISCOVER;			        
	*p++ = dhcpMaxMsgSize;				
	*p++ = 2;				    
	memcpy(p,&dhcpMsgSize,2);				      
	p += 2;	       
       	*p = endOption;
	
	/* build UDP/IP header */		  
	udpipgen((udpiphdr *)UdpIpMsgSend.udpipmsg, 0, INADDR_BROADCAST, 
		 htons(DHCP_CLIENT_PORT), htons(DHCP_SERVER_PORT),
		 sizeof(dhcpMessage));
}

/***********************************************************************
 *
 * dhcp AutoDetect program core!
 *  		
***********************************************************************/		
//junzhao 2005.2.5
int _DHCPAutoDetect()
{
	struct sockaddr sa;
	/*
	if(strcmp(nvram_safe_get("wan_proto"), "dhcp") || 
		strcmp(nvram_safe_get("wan_autodetect"), "1"))
	{
		printf("not dhcp or autodetect\n");
		return result;
	}
	*/
	char itfname[8];
	memset(itfname, 0, sizeof(itfname));
	sprintf(itfname, "nas%d", which_conn);
	memset(MyEthAddr, 0, sizeof(MyEthAddr));
	dhcpSocket = openInterface(itfname, ETH_P_ALL, MyEthAddr);
	printf("dhcpSocket = %d  mac = %2x:%2x:%2x:%2x:%2x:%2x\n", 
		dhcpSocket, MyEthAddr[0], MyEthAddr[1], MyEthAddr[2], 
		MyEthAddr[3], MyEthAddr[4], MyEthAddr[5]);
	
	buildDhcpDiscover();
	
	memset(&sa, 0, sizeof(struct sockaddr));
    	memcpy(sa.sa_data, itfname, sizeof(itfname));
	if(sendto(dhcpSocket, &UdpIpMsgSend,
		    sizeof(struct packed_ether_header)+sizeof(udpiphdr)+sizeof(dhcpMessage), 0, &sa, sizeof(sa)) <= 0 )
        {
              	printf("send dhcp: fail\n");
		close(dhcpSocket);
		return -1;
        }
	printf("begin waiting\n");
	if(waitForOFFER(3) < 0)
	{
		printf("no message!\n");
		return -1;
	}
	return 0;
}

//junzhao 2005.2.5
int DHCPAutoDetect()
{
	//junzhao 2004.11.18
	char buf[60], *tmp=buf, *wanifname;
	int i;
	start_dhcp();
	sleep(3);
	if(!file_to_buf(WAN_IFNAME, buf, sizeof(buf)))
		return -1;
	for(i=0; i<MAX_CHAN; i++)
	{
		wanifname = strsep(&tmp, " ");
		if(i == which_conn)
			break;
	}
	if(strcmp(wanifname, "Down"))
		return 0;
	else
		return -1;
}

int waitForOFFER(int timval)
{	
	struct timeval timeout;
	struct sockaddr addr;	
    	fd_set fs;
        int r,i,len;	
	struct iphdr *ip_hdr;
	FD_ZERO(&fs);		        
	FD_SET(dhcpSocket,&fs);
	timeout.tv_sec = timval;
	timeout.tv_usec = 0;
	
	do	
	{
		r = select(dhcpSocket+1, &fs, NULL, NULL, &timeout);
		if (r < 0) 		
		{
			printf("select (waitForOffer fail)\n");
			close(dhcpSocket);
			return -1;
		}
		else if (r == 0) 
		{
			printf("time out!\n");
			close(dhcpSocket);
			return -1;
		}		
		
		if (!FD_ISSET(dhcpSocket,&fs)) 
		{
			printf("why has other fd\n");
			continue;
		}
		
		printf("has new message!!\n");
		memset(&UdpIpMsgRecv, 0, sizeof(udpipMessage));			
		i = sizeof(struct sockaddr);				
		len = recvfrom(dhcpSocket, &UdpIpMsgRecv,
		       sizeof(udpipMessage), 0, (struct sockaddr *)&addr, &i);	
		if ( len == -1 )								{
			printf("recvfrom: fail\n");
			close(dhcpSocket);
			return -1;		
		}
		if ( UdpIpMsgRecv.ethhdr.ether_type != htons(ETHERTYPE_IP) )
		{
			printf("wrong ether_type!\n");
			continue;
		}
		
		ip_hdr = (struct iphdr *)(((struct udpiphdr *)(UdpIpMsgRecv.udpipmsg))->ip);
		if ( ip_hdr->protocol != IPPROTO_UDP ) 
		{
			printf("wrong protocol(not udp)!\n");
			continue;
		}
#if 0
		len -= sizeof(struct packed_ether_header);
		i = (int)ntohs(ipRecv_local.ip_len);
		if ( len < i )
		{
//			fatalSys("corrupted IP packet of size=%d and ip_len=%d discarded\n",len,i);
			continue;
		}
		len = i-(ipRecv_local.ip_hl<<2);
		i = (int )ntohs(udpRecv->uh_ulen);
		if ( len < i )
		{
//			fatalSys("corrupted UDP msg of size=%d and uh_ulen=%d discarded\n",len,i);
			continue;
		 }
#endif
	      	DhcpMsgRecv = (dhcpMessage *)&UdpIpMsgRecv.udpipmsg[(ip_hdr->ihl<<2) + sizeof(struct udphdr)];
		
		if ( DhcpMsgRecv->htype != ARPHRD_ETHER ) 
		{
			printf("wrong dhcp addr(not arp)\n");
			continue;
		}
		if ( DhcpMsgRecv->xid != htonl(xid))  
		{
			printf("wrong xid!!\n");
			continue;
		}		              
		if(DhcpMsgRecv->op == DHCP_BOOTREPLY) 
		{
			register u_char *p = DhcpMsgRecv->options;
			if(memcmp(p, &magic_cookie, 4))
			{
				printf("wrong magic!!\n");
				continue;
			}
			p += 4;		
			while(p[0]!= 0xff)
			{
				if( p[0] == 0)
				{
					p++;
					continue;
			     	 }
				if(p[0] == 0xff)
					continue;
				
				if(p[0] == dhcpMessageType )							{
					p += 2;
					if(p[0] == DHCP_OFFER)
					{
						printf("have receive dhcpoffer!\n");						
					//junzhao 2004.8.17	
					#ifdef VZ7
						if(!strcmp(encapmode, "pppoe"))
						{
							char *ipaddr;
							struct in_addr in;
							in.s_addr = DhcpMsgRecv->siaddr;
							ipaddr = inet_ntoa(in);
							ram_modify_status(WAN_DHCPD_IPADDR, ipaddr);
						}
					#endif

						close(dhcpSocket);
						return 0;
					}
				}					
				p++;
				p+=p[0];
				p++;
			}
			continue;		 
		}
	} while (1);

	return 0;					
}

/***********************************************************************
 *
 * pppoe AutoDetect program core!
 *  		
***********************************************************************/		
int PPPOEAutoDetect()
{
	/*if(strcmp(nvram_safe_get("wan_proto"), "pppoe") || 
		strcmp(nvram_safe_get("wan_autodetect"), "1"))
	{
		printf("not pppoe or autodetect\n");
		return -1;
	}*/
	char itfname[8];
	memset(itfname, 0, sizeof(itfname));
	sprintf(itfname, "nas%d", which_conn);
	memset(MyEthAddr, 0, sizeof(MyEthAddr));
	DiscoverySocket = openInterface(itfname, Eth_PPPOE_Discovery, MyEthAddr);
	printf("DiscoverSocket = %d  mac = %2x:%2x:%2x:%2x:%2x:%2x\n", 
		DiscoverySocket, MyEthAddr[0], MyEthAddr[1], MyEthAddr[2], 
		MyEthAddr[3], MyEthAddr[4], MyEthAddr[5]);
	sendPADI();
	if(waitForPADO(3) < 0)
	{
		close(DiscoverySocket);
		return -1;
	}
	close(DiscoverySocket);
	return 0;
}

/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
int unip_PPPOEAutoDetect()
{
	char itfname[8];
	memset(itfname, 0, sizeof(itfname));
	sprintf(itfname, "nas%d", which_conn);
	printf("itfname = %s\n",itfname);
	memset(MyEthAddr, 0, sizeof(MyEthAddr));
	DiscoverySocket = openInterface(itfname, Eth_PPPOE_Discovery, MyEthAddr);
	printf("DiscoverSocket = %d  mac = %2x:%2x:%2x:%2x:%2x:%2x\n", 
		DiscoverySocket, MyEthAddr[0], MyEthAddr[1], MyEthAddr[2], 
		MyEthAddr[3], MyEthAddr[4], MyEthAddr[5]);
	unip_sendPADI();
	if(waitForPADO(3) < 0)
	{
		close(DiscoverySocket);
		return -1;
	}
	close(DiscoverySocket);
	return 0;
}

#endif

/***********************************************************************
 *
 * static gateway AutoDetect program core!
 *  		
***********************************************************************/		
int STATICAutoDetect(char *vpivci)
{
	int result = -1;
	int i;
	char wan_ipaddr[32];
	char wan_gateway[32];
	char wan_netmask[32];
	struct sockaddr_in pingaddr;
	int pingsocket;
	struct icmphdr *pkt;
	char packet[128];
	struct timeval timeout;
	fd_set readable;
	struct sockaddr_in from;
	int cc;
	int fromlen;
	char itfname[8];
	memset(itfname, 0, sizeof(itfname));
#ifndef CLIP_SUPPORT
	sprintf(itfname, "nas%d", which_conn);
#else
	if(!strcmp(encapmode, "clip"))
		sprintf(itfname, "atm%d", which_conn);
	else
		sprintf(itfname, "nas%d", which_conn);
#endif

	/*
	if(strcmp(nvram_safe_get("wan_proto"), "static") || 
		strcmp(nvram_safe_get("wan_autodetect"), "1"))
	{
		printf("not static or autodetect\n");
		return result;
	}
	*/
	memset(wan_ipaddr, 0, sizeof(wan_ipaddr));
	memset(wan_gateway, 0, sizeof(wan_gateway));
	memset(wan_netmask, 0, sizeof(wan_netmask));
	
	if(!strcmp(encapmode, "routed"))
	{	
		strcpy(wan_ipaddr, r_ipaddr);
		strcpy(wan_gateway, r_gateway);
		strcpy(wan_netmask, r_netmask);
	}
#ifdef CLIP_SUPPORT
	else if(!strcmp(encapmode, "clip"))
	{
		strcpy(wan_ipaddr, c_ipaddr);
		strcpy(wan_gateway, c_gateway);
		strcpy(wan_netmask, c_netmask);
	}
#endif
	else if(!strcmp(encapmode, "1483bridged"))
	{
		if(!strcmp(dhcpenable, "1"))
			return result;
		strcpy(wan_ipaddr, b_ipaddr);
		strcpy(wan_gateway, b_gateway);
		strcpy(wan_netmask, b_netmask);
	}		
	printf("wan_ipaddr %s, wan_netmask %s, wan_gateway %s\n", wan_ipaddr, wan_netmask, wan_gateway);
	
	ifconfig(itfname, IFUP, wan_ipaddr, wan_netmask);
	usleep(10);

#ifdef CLIP_SUPPORT
	if(!strcmp(encapmode, "clip"))
	{
		eval("atmarp", "-s", c_gateway, vpivci);
		printf("atmarp add %s %s waiting....\n", c_gateway, vpivci);
		sleep(1);
	}
#endif
	pingsocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(pingsocket < 0)
	{
		printf("socket create fail\n");
		return result;
	}
	
	memset(&pingaddr, 0, sizeof(struct sockaddr_in));

	pingaddr.sin_family = AF_INET;
	pingaddr.sin_addr.s_addr = inet_addr(wan_gateway);
	/*
	if(inet_aton(wan_gateway, &pingaddr.sin_addr))
    	{
		printf("gateway addr wrong\n");
		close(pingsocket);
		return result;
	}
	*/
	pkt = (struct icmphdr *)packet;
	memset(pkt, 0, sizeof(packet));
	pkt->type = ICMP_ECHO;
	pkt->code = 0;
	pkt->un.echo.id = htons(0x0E01);
	pkt->un.echo.sequence = 0;
	pkt->checksum = in_cksum((unsigned short *)pkt, sizeof(packet));

	for(i=0; i<2; i++)
	{
		if(sendto(pingsocket, packet, sizeof(packet), 0, (struct sockaddr *)&pingaddr, sizeof(struct sockaddr_in)) < 0)
		{
			printf("send to fail\n");
			close(pingsocket);
			return -1;
		}
	}
	
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	FD_ZERO(&readable);
	FD_SET(pingsocket, &readable);
	
	while(1)
	{
		cc = select(pingsocket+1, &readable, NULL, NULL, &timeout);
		if(cc < 0)
		{
			printf("select error\n");
			close(pingsocket);
			return -1;
		}
		else if(cc == 0)
		{
			printf("time out\n");
			close(pingsocket);
			return -1;
		}
		else
		{
			if(FD_ISSET(pingsocket, &readable))
				break;
			else 
				continue;
		}
	}
	fromlen = sizeof(from);						
	cc = recvfrom(pingsocket, (char *)packet, sizeof(packet), 0, 
		(struct sockaddr *)&from, &fromlen);	
	if ( cc == -1 )									{
		printf("recvfrom: fail\n");
		close(pingsocket);
		return -1;								}	
	if(cc >= 76)
	{
		struct iphdr *iphdr = (struct iphdr *)packet;
		pkt = (struct icmphdr *)(packet + (iphdr->ihl << 2));
		if((pkt->type == ICMP_ECHOREPLY)
		    && (pkt->un.echo.id == htons(0x0E01)))
		{
			printf("ok, get the echo reply\n");
			close(pingsocket);
			return 0;
		}
	}
	printf("wrong packet\n");
	close(pingsocket);
	return -1;
}

/***********************************************************************
 *
 * pppoa AutoDetect program core!
 *  		
***********************************************************************/		
int PPPOAAutoDetect(int fd)
{
	unsigned char packet[64];
	char *buf;
	struct PPPoALhdr *lhdr;
	struct PPPoAVhdr *vhdr;
	fd_set readable;
	int cc, i;
	struct timeval timeout;
	
	if(fd < 0)
		return -1;
	
	memset(packet, 0 ,sizeof(packet));
	
	if(!strcmp(multiplex, "llc")) //LLC
	{
		lhdr = (struct PPPoALhdr *)packet;
		lhdr->llc = htons(0xFEFE);
		lhdr->ctrl = 0x03;
		lhdr->nlpid = 0xCF;
		lhdr->ppp_proto = htons(0xC021);
		buf = packet + sizeof(struct PPPoALhdr);
	}

	else if(!strcmp(multiplex, "vc"))//VC
	{
		vhdr = (struct PPPoAVhdr *)packet;
		vhdr->ppp_proto = htons(0xC021);
		buf = packet + sizeof(struct PPPoAVhdr);
	}
/*	
	buf = packet;
	*(buf)++ = 0xC0;
	*(buf)++ = 0x21;
*/
	*(buf)++ = CONFREQ;
	*(buf)++ = MYID;
	*(buf)++ = 0;
	*(buf)++ = 0x04;
/*
	*(buf)++ = 0x03;
	*(buf)++ = 0x05;
	*(buf)++ = 0xc2;
	*(buf)++ = 0x23;
	*(buf)++ = 0x05;
*/
	
	printf("before write\n");
	for(i=0; i<sizeof(packet); i++)
		printf("%02x...", packet[i]);
	
	if(send(fd, (unsigned char *)packet, sizeof(packet), 0) < 0)
	{
		printf("write fail\n");
		close(fd);
		return -1;
	}

	FD_ZERO(&readable);
	FD_SET(fd, &readable);
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	
	printf("begin waiting\n");
	while(1)
	{
		cc = select(fd+1, &readable, NULL, NULL, &timeout);
		if(cc < 0)
		{
			printf("select error\n");
			close(fd);
			return -1;
		}
		else if(cc == 0)
		{
			printf("time out\n");
			close(fd);
			return -1;
		}
		else
		{
			printf("have new message\n");
			if(!FD_ISSET(fd, &readable))
			{
				printf("which fd readable??\n");
				continue;
			}
			else 
			{
				printf("before read\n");
				memset(packet, 0 ,sizeof(packet));
				if((cc = recv(fd, packet, sizeof(packet), 0))< 0)
				{
					printf("read error\n");
					close(fd);
					return -1;
				}	
				for(i=0; i<sizeof(packet); i++)
					printf("%02x...", packet[i]);
				/*
				memset(packet, 0 ,sizeof(packet));
				buf = packet;
				*(buf)++ = 0xC0;
				*(buf)++ = 0x21;
				*(buf)++ = CONFREQ;
				*(buf)++ = MYID;
				*(buf)++ = 0;
				*(buf)++ = 0x04;
				
				if(send(fd, (unsigned char *)packet, sizeof(packet), 0) < 0)
				{	
					printf("write fail\n");
					close(fd);
					return -1;
				}
				*/
				if(!strcmp(multiplex, "llc")) //LLC
				{
					printf("here is LLC process\n");
					lhdr = (struct PPPoALhdr *)packet;
					if(lhdr->llc != htons(0xFEFE) || 
					    (lhdr->ctrl != 0x03) || 
					    (lhdr->nlpid != 0xCF))
					{
						printf("wrong llc-aal5 header\n");
						continue;
					}
					if(lhdr->ppp_proto != htons(0xC021))
					{
						printf("wrong ppp proto(not lcp)\n");
						continue;
					}
					//buf = packet + sizeof(struct PPPoALhdr);				
				}
				else if(!strcmp(multiplex, "vc"))//VC
				{
					printf("here is VC process\n");
					vhdr = (struct PPPoAVhdr *)packet;
					if(vhdr->ppp_proto != htons(0xC021))
					{
						printf("wrong ppp proto(not lcp)\n");
						continue;
					}
				}
				/*
				if((*buf++)!= CONFACK)
				{
					printf("wrong code\n");
					continue;
				}
				if((*buf++)!= MYID)
				{
					printf("wrong id\n");
					continue;
				}
				*/
				printf("ok get lcp packet!!!\n");
				break;
				
			}	
		
		}	
	}
	close(fd);	
	return 0;
}	
