/*
 * Network services
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: network.c,v 1.5 2006/06/14 09:12:00 johnny Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <fcntl.h>
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <code_pattern.h>
#include <wlutils.h>
#include <utils.h>
#include <rc.h>
#include <cy_conf.h>
#include <cymac.h>
#include <time.h>
#include "pvc.h"
#include "ledapp.h"
//#include <parm.h>

//junzhao 2005.5.16 for ':' setting
enum {SET, GET};

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)
#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)
//#define LJZ_DEBUG

int which_conn = -1;
struct vcc_config *vcc_config_ptr;
struct pppoe_config *pppoe_config_ptr;
struct pppoa_config *pppoa_config_ptr;
struct bridged_config *bridged_config_ptr;
struct routed_config *routed_config_ptr;
#ifdef CLIP_SUPPORT
struct clip_config *clip_config_ptr;
#endif

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
struct unip_pppoe_config *unip_pppoe_config_ptr;
struct unip_pppoa_config *unip_pppoa_config_ptr;
#endif

extern unsigned short vpivci_value[2*MAX_CHAN];
extern int vpivci_final_value[MAX_CHAN][2];
extern int sockfd_used[MAX_CHAN];
extern pid_t ppp_pid[MAX_CHAN];
extern pid_t udhcpc_pid[MAX_CHAN];
extern int firstboot;
//add by zhs for quickVPN
#ifdef NOMAD_SUPPORT
extern int att_ipsec0_t0_def_wan(void);
extern int set_ipsec_def_ipt(void);
#endif
//end by zhs
#ifdef CLIP_SUPPORT
//int atmarpd_num = 0;
int atmarpd_index = 0;
extern int clip_load();
#endif

/* by michael */
#ifdef CWMP_SUPPORT
extern int factory_default;
#endif

extern int secondenter;

extern int pppoa_load();
extern int rfc2684_load(int autoway);
extern int create_br(int nrum);
extern void nvram_modify_status(char *type, char *str);
extern int ram_modify_status(const char *filename, const char *str);
extern void nvram_modify_vpivci(int vpi_value, int vci_value);
int mac_clone(char *devname);

//unsigned char lan_macaddr[6];

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
int mpppoe_dns_to_resolv(int index)
{
	FILE *fp_w;
	char readbuf[256], dnsbuf[32], pdns[16], sdns[16], *tmpstr = readbuf;
	char *dnsptr;
	int which = index;
	char encap[15];
	
	/* Save DNS to resolv.conf */
	if (!(fp_w = fopen(RESOLV_FILE, "a+"))) {
                perror(RESOLV_FILE);
                return errno;
        }
	
	memset(dnsbuf, 0, sizeof(dnsbuf));
	memset(pdns, 0, sizeof(pdns));
	memset(sdns, 0, sizeof(sdns));
	memset(encap, 0, sizeof(encap));

	check_cur_encap(index, encap);

	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		if(!file_to_buf(WAN_MPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
		{
			printf("read %s file fail\n", WAN_MPPPOE_GET_DNS);
			sprintf(pdns, "0.0.0.0");
			sprintf(sdns, "0.0.0.0");
			//return NULL;
		}
		else
		{
			//junzhao 2004.8.4
			int i;
			for(i=0; i<MAX_CHAN; i++)
			{
				dnsptr = strsep(&tmpstr, " ");
				if(i == which)
				{
					sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
					break;
				}
			}
		/*	
        		foreach(dnsbuf, readbuf, next) 
			{
				if(which-- == 0)
					break;
			}		
			sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
		*/
		}
	#else
		return 0;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		if(!file_to_buf(WAN_IPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
		{
			printf("read %s file fail\n", WAN_IPPPOE_GET_DNS);
			sprintf(pdns, "0.0.0.0");
			sprintf(sdns, "0.0.0.0");
			//return NULL;
		}
		else
		{	
			//junzhao 2004.8.4
			int i;
			for(i=0; i<MAX_CHAN; i++)
			{
				dnsptr = strsep(&tmpstr, " ");
				if(i == which)
				{
					sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
					break;
				}
			}

			/*
        		foreach(dnsbuf, readbuf, next) 
			{
				if(which-- == 0)
					break;
			}		
			sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
			*/
		}
	#else
		return 0;
	#endif
	}
	else
		return 0;
	
	if(strcmp(pdns, "0.0.0.0"))
        	fprintf(fp_w, "nameserver %s\n", pdns);
	if(strcmp(sdns, "0.0.0.0"))
        	fprintf(fp_w, "nameserver %s\n", pdns);
	fclose(fp_w);
	return 0;
}
#endif

int
dns_to_resolv(int index)
{
	FILE *fp_w;
	struct dns_lists *dns_list = NULL;
	int i = 0;

	/* Save DNS to resolv.conf */
	if (!(fp_w = fopen(RESOLV_FILE, "w"))) {
                perror(RESOLV_FILE);
                return errno;
        }

	//dns_list = get_dns_list(index);
	dns_list = get_dns_list_total();

        for(i=0 ; i<dns_list->num_servers; i++)
                fprintf(fp_w, "nameserver %s\n", dns_list->dns_server[i]);
	
#ifdef DNSMASQ_SUPPORT
	/* Put a pseudo DNS IP to trigger Connect On Demand */
	/*
	if(dns_list->num_servers == 0 && 
	   (nvram_match("wan_proto","pppoe") || nvram_match("wan_proto","pptp")) && 
	   nvram_match("ppp_demand","1"))
              fprintf(fp_w, "nameserver 1.1.1.1\n");
	 */

	//junzhao 2004.10.11
	if(dns_list->num_servers == 0)
	{
		which_conn = index;
		if(vcc_global_init() < 0)
		{
        		fclose(fp_w);
        		if(dns_list)    
				free(dns_list);
			return -1; 
		}
		if(!strcmp(encapmode, "pppoe"))
		{
			if(pppoe_global_init() < 0)
			{
        			fclose(fp_w);
        			if(dns_list)    
					free(dns_list);
				return -1; 
			}
			if(!strcmp(e_demand, "1"))
               			 fprintf(fp_w, "nameserver 1.1.1.1\n");
		}
		else if(!strcmp(encapmode, "pppoa"))
		{
			if(pppoa_global_init() < 0)
			{
        			fclose(fp_w);
        			if(dns_list)    
					free(dns_list);
				return -1; 
			}
			if(!strcmp(a_demand, "1"))
               			 fprintf(fp_w, "nameserver 1.1.1.1\n");
		}
	}
#endif

        fclose(fp_w);
        if(dns_list)    free(dns_list);

	eval("touch", "/tmp/hosts");
				
	return 1;
}

void
start_lan(void)
{
	char *lan_ifname = strdup (nvram_safe_get("lan_ifname"));
	char *lan_ifnames = strdup (nvram_safe_get("lan_ifnames")); 
	char name[80], *next;
	int s;
	struct ifreq ifr;
	char wl_face[10];
	char ipaddr[20],netmask[20];
	FILE *fp;
	dprintf("%s\n", lan_ifname);

//wwzh
#ifdef CONFIG_WIRELESS
	/* Bring up wrieless */
	#if 0
        if(nvram_invmatch("wl_gmode","-1")) eval("insmod", "wl");
        else    eval("rmmod", "wl");
	#endif
#endif
	
	//junzhao 2004.3.19
	//if(macaddr_modify("eth0", nvram_safe_get("lan_def_hwaddr")))
	//	return;	
	
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return;

 	/* Bring up bridged interfaces */
	if (strncmp(lan_ifname, "br", 2) == 0) {
		eval("brctl", "addbr", lan_ifname);
		//wwzh
	#ifdef CONFIG_BROADCOM
		eval("brctl", "setfd", lan_ifname, "0");
		if (nvram_match("router_disable", "1") || nvram_match("lan_stp", "0"))
			eval("brctl", "stp", lan_ifname, "dis");
	#endif
		foreach(name, lan_ifnames, next) {
			printf("name=[%s] lan_ifname=[%s]\n", name, lan_ifname);
		//wwzh
		#ifdef CONFIG_WIRELESS
		#if 0
			if(check_hw_type() == BCM4702_CHIP)
				strcpy(wl_face, "eth2");
			else
				strcpy(wl_face, "eth1");
				
			/* Write wireless mac , add by honor 2003-10-27 */
			if(!strcmp(name, wl_face)){
				unsigned char mac[20]; ;
				fprintf(stderr, "Write wireless mac\n");
				strcpy(mac, nvram_safe_get("et0macaddr"));
				MAC_ADD(mac);
				MAC_ADD(mac);	// The wireless mac equal lan mac add 2
				ether_atoe(mac, ifr.ifr_hwaddr.sa_data);
				ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
				strncpy(ifr.ifr_name, wl_face, IFNAMSIZ);
				if (ioctl(s, SIOCSIFHWADDR, &ifr) == -1)
					perror("Write wireless mac fail : ");
				else
					fprintf(stderr, "Write wireless mac successfully\n");
			}
		#endif
		#endif //end by wwzh

			/* Bring up interface */
			ifconfig(name, IFUP, "0.0.0.0", NULL);
			/* Set the logical bridge address to that of the first interface */
/*			strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
			
			if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0 &&
			    memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0", ETHER_ADDR_LEN) == 0) {
				strncpy(ifr.ifr_name, name, IFNAMSIZ);
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
					strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
					ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
					ioctl(s, SIOCSIFHWADDR, &ifr);
					fprintf(stderr,"=====> set %s hwaddr to %s\n",lan_ifname,name);
				}
				else 
					perror(lan_ifname);
			}
			else
*/			strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
			
			if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0 &&
			    memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0", ETHER_ADDR_LEN) == 0) {
				strncpy(ifr.ifr_name, name, IFNAMSIZ);
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
									
					strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
					ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
					ioctl(s, SIOCSIFHWADDR, &ifr);
					fprintf(stderr,"=====> set %s hwaddr to %s\n",lan_ifname,name);
				}
				else 
					perror(lan_ifname);
			}
			else
				perror(lan_ifname);
			/* If not a wl i/f then simply add it to the bridge */
			if (eval("wlconf", name, "up"))
				eval("brctl", "addif", lan_ifname, name);
		//wwzh
		#ifdef CONFIG_WIRELESS
		#if 0
			else {
				/* get the instance number of the wl i/f */
				char wl_name[] = "wlXXXXXXXXXX_mode";
				int unit;
				wl_ioctl(name, WLC_GET_INSTANCE, &unit, sizeof(unit));
				snprintf(wl_name, sizeof(wl_name), "wl%d_mode", unit);
				/* Receive all multicast frames in WET mode */
				if (nvram_match(wl_name, "wet"))
					ifconfig(name, IFUP | IFF_ALLMULTI, NULL, NULL);
				/* Do not attach the main wl i/f if in wds mode */
				if (nvram_invmatch(wl_name, "wds"))
					eval("brctl", "addif", lan_ifname, name);
			}
		#endif
		#endif //end by wwzh
		}
	}
	/* specific non-bridged lan i/f */
	else if (strcmp(lan_ifname, "")) {	// FIXME
		/* Bring up interface */
		ifconfig(lan_ifname, IFUP, NULL, NULL);
	//wwzh
	#ifdef CONFIG_WIRELESS
		#if 0
		/* config wireless i/f */
		if (!eval("wlconf", lan_ifname, "up")) {
			char tmp[100], prefix[] = "wanXXXXXXXXXX_";
			int unit;
			/* get the instance number of the wl i/f */
			wl_ioctl(lan_ifname, WLC_GET_INSTANCE, &unit, sizeof(unit));
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			/* Receive all multicast frames in WET mode */
			if (nvram_match(strcat_r(prefix, "mode", tmp), "wet"))
				ifconfig(lan_ifname, IFUP | IFF_ALLMULTI, NULL, NULL);
		}
		#endif
	#endif //end by wwzh

	}

	/* Bring up and configure LAN interface */
	strcpy(ipaddr, nvram_safe_get("lan_ipaddr"));
	strcpy(netmask, nvram_safe_get("lan_netmask"));
	ifconfig(lan_ifname, IFUP, ipaddr, netmask);
		 //nvram_safe_get("lan_ipaddr"), nvram_safe_get("lan_netmask"));
//wwzh
	printf("Lan Ipaddr: %s Netmask: %s................\n", 
		nvram_get("lan_ipaddr"), nvram_get("lan_netmask"));

	/* Get current LAN hardware address */
	strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
	if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
		char eabuf[32];
		//junzhao 2004.3.25
		/*
		memset(lan_macaddr, 0, 6);
		memcpy(lan_macaddr, ifr.ifr_hwaddr.sa_data, 6);
		printf("??????%02x:%02x:%02x:%02x:%02x:%02x\n", lan_macaddr[0], lan_macaddr[1], lan_macaddr[2], lan_macaddr[3], lan_macaddr[4], lan_macaddr[5]);
		*/
		nvram_set("lan_hwaddr", ether_etoa(ifr.ifr_hwaddr.sa_data, eabuf));
#ifdef CWMP_SUPPORT
		if (factory_default == 1) {
			set_acsusername();
			factory_default = 0;
		}
#endif

	}
	close(s);
	printf("%s %s\n",
		nvram_safe_get("lan_ipaddr"),
		nvram_safe_get("lan_netmask"));

	/* Bring up local host interface */
	config_loopback();

	/* Set additional lan static routes if need */
	set_routes();
	
	//enable_disable_pppoe_pass();
	free(lan_ifnames);
	free(lan_ifname);
}

void
stop_lan(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char name[80], *next;

	dprintf("%s\n", lan_ifname);
	/* Bring down LAN interface */
	ifconfig(lan_ifname, 0, NULL, NULL);

	/* Bring down bridged interfaces */
	if (strncmp(lan_ifname, "br", 2) == 0) {
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
		//wwzh
		#ifdef CONFIG_WIRELESS
			eval("wlconf", name, "down");
		#endif
			ifconfig(name, 0, NULL, NULL);
			eval("brctl", "delif", lan_ifname, name);
		}
		eval("brctl", "delbr", lan_ifname);
	}
	//wwzh
#ifdef CONFIG_WIRELESS
	/* Bring down specific interface */
	else if (strcmp(lan_ifname, ""))
		eval("wlconf", lan_ifname, "down");
#endif //end by wwzh

	dprintf("done\n");
}

int asciitoint(char str)
{
	int i = 0;
	//printf("str = %c\n", str);
	if((str >= '0') && (str <= '9'))
		i = str - '0';
	if((str >= 'a') && (str <= 'f'))
		i = str - 'a' + 10;
	if((str >= 'A') && (str <= 'F'))                                                        i = str - 'A' + 10;
	
	return i;
}

u_int32_t asciitohex(char *str)
{
	int  buf[8];
	int i = 0;
	u_int32_t hexvalue = 0;

	while(*str != '\0')
	{
        	buf[i] = asciitoint(*str ++);                                                   i++;
	}
	hexvalue = (buf[0] << 4 | buf[1]) << 24 |
		   (buf[2] << 4 | buf[3]) << 16 |
		   (buf[4] << 4 | buf[5]) << 8  |
		   (buf[6] << 4 | buf[7]) ;
	
	return hexvalue;
}
		
int parse_default_gw(char *ifname, struct in_addr *gateway)
{
	FILE *fp;
	char buf[512];
	struct in_addr defaultgw;
	char gate_addr[16], net_addr[16], mask_addr[16], iface[8];
       	int iflags, metric, refcnt, use, mss, window, irtt;
      	u_int32_t dest = 0, netmask = 0;
	
	memset(&defaultgw, 0, sizeof(defaultgw));
	fp = fopen("/proc/net/route", "r");
	if(NULL == fp) 
	{
                printf("can not open the /proc/net/route file\n");
                return -1;
	}
	
	memset(buf, 0, sizeof(buf));
	while (fgets(buf, 512, fp))
	{
		sscanf(buf, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d", iface, net_addr, gate_addr,  &iflags, &refcnt, &use, &metric, mask_addr, &mss, &window, &irtt);
		
		if(!strcmp(iface, ifname))
		{
			if(!(iflags & 0x1))
				continue;
			dest = asciitohex(net_addr);
			defaultgw.s_addr = asciitohex(gate_addr);
			netmask = asciitohex(mask_addr);
			if((dest == 0x0) && (netmask == 0x0))
				break;
		}
	}

	memcpy(gateway, &defaultgw, sizeof(gateway));
	return 0;
}			
			 
int wanstatus_info_get(char *devname, char *type, char *linkinfo)
{
	int sockfd, upflag = 0;
	struct ifreq ifr;
	if(linkinfo == NULL || devname == NULL || type == NULL)
	{
		printf("param fail\n");
		return -1;
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
		return -1;
	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(sockfd);
		return -1;
	}
	if(ifr.ifr_flags & IFF_UP)
		upflag = 1;
	if(!strcmp(type, "ipaddr"))
	{
		if(upflag == 1)
		{
			strncpy(ifr.ifr_name, devname, IFNAMSIZ);
			if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
				upflag = 0;
		}
		if(upflag == 1)
			strcpy(linkinfo, inet_ntoa(sin_addr(&ifr.ifr_addr)));
		else
			strcpy(linkinfo, "0.0.0.0");
	}		
	else if(!strcmp(type, "netmask"))
	{
		if(upflag == 1)
		{
			strncpy(ifr.ifr_name, devname, IFNAMSIZ);
			if(ioctl(sockfd, SIOCGIFNETMASK, &ifr) < 0)
				upflag = 0;
		}
		if(upflag == 1)
			strcpy(linkinfo, inet_ntoa(sin_addr(&ifr.ifr_addr)));
		else
			strcpy(linkinfo, "0.0.0.0");
	}
	else if(!strcmp(type, "broadcast"))	
	{
		if(upflag == 1)
		{
			strncpy(ifr.ifr_name, devname, IFNAMSIZ);
			if(ioctl(sockfd, SIOCGIFBRDADDR, &ifr) < 0)
				upflag = 0;
		}
		if(upflag == 1)
			strcpy(linkinfo, inet_ntoa(sin_addr(&ifr.ifr_netmask)));
		else
			strcpy(linkinfo, "0.0.0.0");
	}
	else if(!strcmp(type, "gateway"))
	{
		if(strstr(devname, "ppp"))
		{
			if(upflag == 1)
			{
				strncpy(ifr.ifr_name, devname, IFNAMSIZ);
				if(ioctl(sockfd, SIOCGIFDSTADDR, &ifr) < 0)
					upflag = 0;
			}
			if(upflag == 1)
				strcpy(linkinfo, inet_ntoa(sin_addr(&ifr.ifr_dstaddr)));
			else
				strcpy(linkinfo, "0.0.0.0");
		}
		else
		{
			struct in_addr gw;
			if(upflag == 1)
			{
				if(parse_default_gw(devname, &gw) < 0)
					upflag = 0;
			}
			if(upflag == 1)
				strcpy(linkinfo, inet_ntoa(gw));
			else
				strcpy(linkinfo, "0.0.0.0");
		}	
	}
	close(sockfd);		
	return 0;
}

void start_pppoe(int status, int way)
{
	char *pppoe_argv[32];
	int arg_cnt = 0;
	char devname[10];
	int timeout = 5;
	//pid_t pid;
	char pppoe_mtu[8];
	char pppdev[8];
	char *filename;
	FILE *fp;
	int randomtime;
	
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));

	if(!strcmp(e_username, "") || !strcmp(e_password, ""))
		return;	
	
	if(way == 0)
	{		
		printf("random time %d\n", randomtime);
		sleep(randomtime);	
	}
	
	memset((char *)devname, 0, sizeof(devname));
	sprintf(devname, "nas%d", which_conn);
	
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
		
	init_mtu("pppoe", pppoe_mtu);
	
	if(way == 0)
		mac_clone(devname);
	
	printf("in start_pppoe %s %s %s %s %s %s\n", devname, e_username, e_password, e_demand, e_idletime, pppoe_mtu);
	
	//now launch pppoe
	pppoe_argv[arg_cnt++] = "pppd";
	pppoe_argv[arg_cnt++] = "plugin";
	pppoe_argv[arg_cnt++] = "pppoe";
	//pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = "user";
	pppoe_argv[arg_cnt++] = e_username;
	pppoe_argv[arg_cnt++] = "password";
	pppoe_argv[arg_cnt++] = e_password;
	pppoe_argv[arg_cnt++] = "nodetach";
	pppoe_argv[arg_cnt++] = "usepeerdns";
	pppoe_argv[arg_cnt++] = "unit";
	pppoe_argv[arg_cnt++] = (char *)(&devname[3]);   //which conn
		
	//if(nvram_match("wan_ppp_defaultroute", "1"))
	if(which_conn == which_tmp)
		pppoe_argv[arg_cnt++] = "defaultroute";
			
	pppoe_argv[arg_cnt++] = "mru";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "mtu";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	/*
	pppoe_argv[arg_cnt++] = "maxfail";
	pppoe_argv[arg_cnt++] = "10";
	*/
	
#ifdef VZ16
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "10";
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "6";
#else
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "30";
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "3";
#endif	

	if(strcmp(e_servicename, ""))
	{
		pppoe_argv[arg_cnt++] = "pppoe_srv_name";
		pppoe_argv[arg_cnt++] = e_servicename;
	}
	
	pppoe_argv[arg_cnt++] = "ipparam";
	pppoe_argv[arg_cnt++] = "0";
		
	if(!strcmp(e_demand, "1"))
	{
		char idletime[8];
		memset(idletime, 0, sizeof(idletime));
		sprintf(idletime, "%d", (atoi(e_idletime))*60);
		pppoe_argv[arg_cnt++] = "demand";
		pppoe_argv[arg_cnt++] = "holdoff";
		pppoe_argv[arg_cnt++] = "3";
		pppoe_argv[arg_cnt++] = "idle";
		pppoe_argv[arg_cnt++] = idletime;
	}

/* kirby add for FFT */ 
#ifdef IPCP_NETMASK_SUPPORT
	if (nvram_match("ipcp_netmask_enable","1")) {
		pppoe_argv[arg_cnt++] = "usepeernetmask";
	}
#endif
	
	pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = NULL;
	
	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
					
	_eval(pppoe_argv, NULL, 0, &ppp_pid[which_conn]);
	printf("&&&&&&&& ppp_pid[%d] %d $$$$$$$$\n", which_conn, ppp_pid[which_conn]);

	filename = malloc(strlen("/var/run/ppp.pid") + 3);
	memset(filename, 0, strlen("/var/run/ppp.pid"+3));
	if(filename == NULL)
		return;
	sprintf(filename, "/var/run/ppp%d.pid", which_conn);
	if (!(fp = fopen(filename, "w"))) 
	{
		perror(filename);
		return;
	}
	
	fprintf(fp, "%d", ppp_pid[which_conn]);
	
	fclose(fp);
	free(filename);
	filename = NULL;
	
	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%d", which_conn);
	
	ram_modify_status(WAN_IFNAME, pppdev);
	
	if (!strcmp(e_demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial
		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(1);
			nvram_set("action_service","");
		}
		
		if(which_conn == which_tmp)
		{
			FILE *fp;
			char filename[32];
			stop_wanledctrl();
			start_wanledctrl(which_conn, "ppp", "demand");
			
			//junzhao 2004.11.2 for wanledctrl
			nvram_set("wanled_index", &pppdev[3]);
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/tmp/ppp/link%d.begin", which_conn);
			fp = fopen(filename, "w");
			fclose(fp);

		}
		close(s);
	}
	else
	{
		if(status != REDIAL)
		{
			printf("enter redial\n");
			start_redial(e_redialperiod, pppdev);
			if(which_conn == which_tmp)
			{
				stop_wanledctrl();
				start_wanledctrl(which_conn, "ppp", "keepalive");
				nvram_set("wanled_index", &pppdev[3]);
			}	
		}
	}
}

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT 
void start_unpppoe(int status, int way)
{
	char *pppoe_argv[128];
	int arg_cnt = 0;
	char devname[10];
	int timeout = 5;
	//pid_t pid;
	char pppoe_mtu[8];
	char pppdev[8];
	char *filename;
	FILE *fp;
	int randomtime;
	
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));

	if(!strcmp(ue_username, "") || !strcmp(ue_password, ""))
		return;	
	
	if(way == 0)
	{		
		printf("random time %d\n", randomtime);
		sleep(randomtime);	
	}
	
	memset((char *)devname, 0, sizeof(devname));
	sprintf(devname, "nas%d", which_conn);
	
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
	memset(pppoe_argv,0,sizeof(pppoe_argv));
		
	init_mtu("pppoe", pppoe_mtu);
	
	if(way == 0)
		mac_clone(devname);
	
	printf("in start_pppoe %s %s %s %s %s %s\n", devname, ue_username, ue_password, ue_demand, ue_idletime, pppoe_mtu);
	
	//now launch pppoe
	pppoe_argv[arg_cnt++] = "pppd";
	pppoe_argv[arg_cnt++] = "plugin";
	pppoe_argv[arg_cnt++] = "pppoe";
	//pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = "user";
	pppoe_argv[arg_cnt++] = ue_username;
	pppoe_argv[arg_cnt++] = "password";
	pppoe_argv[arg_cnt++] = ue_password;
	pppoe_argv[arg_cnt++] = "nodetach";
	pppoe_argv[arg_cnt++] = "usepeerdns";
	pppoe_argv[arg_cnt++] = "unit";
	pppoe_argv[arg_cnt++] = (char *)(&devname[3]);   //which conn
		
	//if(nvram_match("wan_ppp_defaultroute", "1"))
	if(which_conn == which_tmp)
		pppoe_argv[arg_cnt++] = "defaultroute";
			
	pppoe_argv[arg_cnt++] = "mru";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "mtu";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	/*
	pppoe_argv[arg_cnt++] = "maxfail";
	pppoe_argv[arg_cnt++] = "10";
	*/
	
#ifdef VZ16
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "10";
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "6";
#else
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "30";
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "3";
#endif	

	if(strcmp(ue_servicename, ""))
	{
		pppoe_argv[arg_cnt++] = "pppoe_srv_name";
		pppoe_argv[arg_cnt++] = ue_servicename;
	}
	
	pppoe_argv[arg_cnt++] = "ipparam";
	pppoe_argv[arg_cnt++] = "0";
		
	if(!strcmp(ue_demand, "1"))
	{
		char idletime[8];
		memset(idletime, 0, sizeof(idletime));
		sprintf(idletime, "%d", (atoi(ue_idletime))*60);
		pppoe_argv[arg_cnt++] = "demand";
		pppoe_argv[arg_cnt++] = "holdoff";
		pppoe_argv[arg_cnt++] = "3";
		pppoe_argv[arg_cnt++] = "idle";
		pppoe_argv[arg_cnt++] = idletime;
	}
        pppoe_argv[arg_cnt++] = "usepeernetmask";  /* kirby for new 90 */
	pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = NULL;
	
	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");

	printf("pppoe_argv = %s\n",pppoe_argv);
					
	_eval(pppoe_argv, NULL, 0, &ppp_pid[which_conn]);
	printf("&&&&&&&& ppp_pid[%d] %d $$$$$$$$\n", which_conn, ppp_pid[which_conn]);

	filename = malloc(strlen("/var/run/ppp.pid") + 3);
	memset(filename, 0, strlen("/var/run/ppp.pid"+3));
	if(filename == NULL)
		return;
	sprintf(filename, "/var/run/ppp%d.pid", which_conn);
	if (!(fp = fopen(filename, "w"))) 
	{
		perror(filename);
		return;
	}
	
	fprintf(fp, "%d", ppp_pid[which_conn]);
	
	fclose(fp);
	free(filename);
	filename = NULL;
	
	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%d", which_conn);
	
	ram_modify_status(WAN_IFNAME, pppdev);
	
	if (!strcmp(ue_demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial

		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(1);
			nvram_set("action_service","");
		}
		
		if(which_conn == which_tmp)
		{
			FILE *fp;
			char filename[32];
			stop_wanledctrl();
			start_wanledctrl(which_conn, "ppp", "demand");
			
			//junzhao 2004.11.2 for wanledctrl
			nvram_set("wanled_index", &pppdev[3]);
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/tmp/ppp/link%d.begin", which_conn);
			fp = fopen(filename, "w");
			fclose(fp);

		}
		close(s);
	}
	else
	{
		if(status != REDIAL)
		{
			printf("enter redial\n");
			start_redial(ue_redialperiod, pppdev);
			if(which_conn == which_tmp)
			{
				stop_wanledctrl();
				start_wanledctrl(which_conn, "ppp", "keepalive");
				nvram_set("wanled_index", &pppdev[3]);
			}	
		}
	}
}

void start_unpppoa(int autoway, int status)
{
	char *pppoa_argv[32];
	int arg_cnt = 0;
	char vpi_vci[32];
	int timeout = 5;
//	pid_t pid;
	int vpi_value = 0;
	int vci_value = 0;
	char wan_conn[4];
	char pppoa_mtu[8];
	char pppdev[8];
	char qosstr[5];
	char *filename;
	FILE *fp;
	int randomtime;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));

	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));

	if(!strcmp(ua_username, "") || !strcmp(ua_password, ""))
		return;	
	
	if(status == 0)
	{
		printf("random time %d\n", randomtime);
		sleep(randomtime);	
	}
	
	if(autoway == AUTOENABLE)
	{
		vpi_value = vpivci_final_value[which_conn][0];
		vci_value = vpivci_final_value[which_conn][1];
	}
	else
	{
		vpi_value = atoi(vvpi);
		vci_value = atoi(vvci);
	}
	
	memset(pppoa_mtu, 0, sizeof(pppoa_mtu));
	init_mtu("pppoa", pppoa_mtu);
	
	memset((char *)vpi_vci, 0, sizeof(vpi_vci));
	sprintf(vpi_vci, "%d.%d", vpi_value, vci_value);						
	memset((char *)wan_conn, 0, sizeof(wan_conn));
	sprintf(wan_conn, "%d", which_conn);						
	//now launch pppoa
	pppoa_argv[arg_cnt++] = "pppd";
	pppoa_argv[arg_cnt++] = "plugin";
	pppoa_argv[arg_cnt++] = "pppoa";
	pppoa_argv[arg_cnt++] = vpi_vci;
	pppoa_argv[arg_cnt++] = "user";
	pppoa_argv[arg_cnt++] = ua_username;
	pppoa_argv[arg_cnt++] = "password";
	pppoa_argv[arg_cnt++] = ua_password;
	pppoa_argv[arg_cnt++] = "nodetach";
	pppoa_argv[arg_cnt++] = "usepeerdns";
	pppoa_argv[arg_cnt++] = "unit";
	pppoa_argv[arg_cnt++] = wan_conn;   //see which conn

	if(!strcmp(multiplex, "llc"))
		pppoa_argv[arg_cnt++] = "llc-encaps";
	else if(!strcmp(multiplex, "vc"))
		pppoa_argv[arg_cnt++] = "vc-encaps";
			
	pppoa_argv[arg_cnt++] = "qos";
	memset(qosstr, 0, sizeof(qosstr));
	if(!strcmp(qostype, "ubr"))
		strncpy(qosstr, "UBR", sizeof(qosstr));
	else if(!strcmp(qostype, "cbr"))
		strncpy(qosstr, "CBR", sizeof(qosstr));
	else if(!strcmp(qostype, "vbr"))
		strncpy(qosstr, "VBR", sizeof(qosstr));
	pppoa_argv[arg_cnt++] = qosstr;
	
	if(strcmp(qostype, "ubr"))
	{
		pppoa_argv[arg_cnt++] = "qos_pcr";
		pppoa_argv[arg_cnt++] = vpcr;
	}
	if(!strcmp(qostype, "vbr"))
	{
		pppoa_argv[arg_cnt++] = "qos_scr";
		pppoa_argv[arg_cnt++] = vscr;
	}		
	if(which_conn == which_tmp)
		pppoa_argv[arg_cnt++] = "defaultroute";
		
	pppoa_argv[arg_cnt++] = "mru";
	pppoa_argv[arg_cnt++] = pppoa_mtu;
	
	pppoa_argv[arg_cnt++] = "mtu";
	pppoa_argv[arg_cnt++] = pppoa_mtu;

	pppoa_argv[arg_cnt++] = "lcp-echo-interval";
	pppoa_argv[arg_cnt++] = "30";
	
	pppoa_argv[arg_cnt++] = "lcp-echo-failure";
	pppoa_argv[arg_cnt++] = "3";
	
	pppoa_argv[arg_cnt++] = "ipparam";
	pppoa_argv[arg_cnt++] = "0";
	
	if(!strcmp(ua_demand, "1"))
	{
		char idletime[8];
		memset(idletime, 0, sizeof(idletime));
		sprintf(idletime, "%d", (atoi(ua_idletime))*60);
		pppoa_argv[arg_cnt++] = "demand";
		pppoa_argv[arg_cnt++] = "holdoff";
		pppoa_argv[arg_cnt++] = "3";
		pppoa_argv[arg_cnt++] = "idle";
		pppoa_argv[arg_cnt++] = idletime;
	}

        pppoa_argv[arg_cnt++] = "usepeernetmask";  /* kirby for new 90 */

	pppoa_argv[arg_cnt++] = NULL;

	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
	
	_eval(pppoa_argv, NULL, 0, &ppp_pid[which_conn]);
	
	
	filename = malloc(strlen("/var/run/ppp.pid") + 3);
	memset(filename, 0, strlen("/var/run/ppp.pid"+3));
	if(filename == NULL)
		return;
	sprintf(filename, "/var/run/ppp%d.pid", which_conn);
	if (!(fp = fopen(filename, "a"))) 
	{
		perror(filename);
		return;
	}
	
	fprintf(fp, "%d\n", ppp_pid[which_conn]);
	fclose(fp);
	free(filename);
	filename = NULL;
	
	
	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%d", which_conn);
	

	ram_modify_status(WAN_IFNAME, pppdev);
	
	if(!strcmp(ua_demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial
		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(1);
			nvram_set("action_service","");
		}
		if(which_conn == which_tmp)
		{
			FILE *fp;
			char filename[32];
			stop_wanledctrl();
			start_wanledctrl(which_conn, "ppp", "demand");
			
			//junzhao 2004.11.2 for wanledctrl
			nvram_set("wanled_index", &pppdev[3]);
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/tmp/ppp/link%d.begin", which_conn);
			fp = fopen(filename, "w");
			fclose(fp);

		}
		close(s);
	}
	else
	{
		printf("here is nodemand mode\n");
		if(status == BOOT)
		{
			printf("enter redial\n");
			start_redial(ua_redialperiod, pppdev);
			if(which_conn == which_tmp)
			{
				stop_wanledctrl();
				start_wanledctrl(which_conn, "ppp", "keepalive");
				nvram_set("wanled_index", &pppdev[3]);
			}
		}
	}
}

#endif

void start_dhcp(void)
{
	char devname[10];
	char pidfile[32];
	int i;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));

//junzhao 2005.2.5
/*	
	int randomtime;
	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));
	printf("random time %d\n", randomtime);
	sleep(randomtime);	
*/
//	pid_t pid;
			
	memset((char *)devname, 0, sizeof(devname));
	sprintf(devname, "nas%d", which_conn);
	
	memset((char *)pidfile, 0, sizeof(pidfile));
	sprintf(pidfile, "/var/run/udhcpc%d.pid", which_conn);
    
	init_mtu(devname, NULL);
	
	mac_clone(devname);

     {//now launch dhcpc	
	char *wan_hostname = nvram_get("wan_hostname");
	char *dhcp_argv[] = 
	{  	
		"udhcpc",
	    	"-i", devname,
		//"-p", "/var/run/udhcpc.pid",
		"-p", pidfile,
		"-s", "/tmp/udhcpc",
		wan_hostname && *wan_hostname ? "-H" : NULL,
		wan_hostname && *wan_hostname ? wan_hostname : NULL,
		NULL
	};
			
	symlink("/sbin/rc", "/tmp/udhcpc");
//	nvram_set("wan_get_dns","");
	
	_eval(dhcp_argv, NULL, 0, &udhcpc_pid[which_conn]);
	printf("&&&&&&&& dhcpc_pid[%d] %d $$$$$$$$\n", which_conn, udhcpc_pid[which_conn]);
//	eval("udhcpc");
//	sleep(3);	
	
	for(i=0; i<100000000; i++)
	;

	//start_wan_done(devname);
	
	if(which_conn == which_tmp)
	{
		stop_wanledctrl();
		start_wanledctrl(which_conn, "dhcp", NULL);
		nvram_set("wanled_index", &devname[3]);
	}
    }
}

void start_pppoa(int autoway, int status)
{
	char *pppoa_argv[32];
	int arg_cnt = 0;
	char vpi_vci[32];
	int timeout = 5;
//	pid_t pid;
	int vpi_value = 0;
	int vci_value = 0;
	char wan_conn[4];
	char pppoa_mtu[8];
	char pppdev[8];
	char qosstr[5];
	char *filename;
	FILE *fp;
	int randomtime;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));

	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));

	if(!strcmp(a_username, "") || !strcmp(a_password, ""))
		return;	
	
	if(status == 0)
	{
		printf("random time %d\n", randomtime);
		sleep(randomtime);	
	}
	
	if(autoway == AUTOENABLE)
	{
		vpi_value = vpivci_final_value[which_conn][0];
		vci_value = vpivci_final_value[which_conn][1];
	}
	else
	{
		vpi_value = atoi(vvpi);
		vci_value = atoi(vvci);
	}
	
	memset(pppoa_mtu, 0, sizeof(pppoa_mtu));
	init_mtu("pppoa", pppoa_mtu);
	
	memset((char *)vpi_vci, 0, sizeof(vpi_vci));
	sprintf(vpi_vci, "%d.%d", vpi_value, vci_value);						
	memset((char *)wan_conn, 0, sizeof(wan_conn));
	sprintf(wan_conn, "%d", which_conn);						
	//now launch pppoa
	pppoa_argv[arg_cnt++] = "pppd";
	pppoa_argv[arg_cnt++] = "plugin";
	pppoa_argv[arg_cnt++] = "pppoa";
	pppoa_argv[arg_cnt++] = vpi_vci;
	pppoa_argv[arg_cnt++] = "user";
	pppoa_argv[arg_cnt++] = a_username;
	pppoa_argv[arg_cnt++] = "password";
	pppoa_argv[arg_cnt++] = a_password;
	pppoa_argv[arg_cnt++] = "nodetach";
	pppoa_argv[arg_cnt++] = "usepeerdns";

/* kirby add for FFT */ 
#ifdef IPCP_NETMASK_SUPPORT
	if (nvram_match("ipcp_netmask_enable","1")) {
		pppoa_argv[arg_cnt++] = "usepeernetmask";
	}
#endif
	
	pppoa_argv[arg_cnt++] = "unit";
	pppoa_argv[arg_cnt++] = wan_conn;   //see which conn

	if(!strcmp(multiplex, "llc"))
		pppoa_argv[arg_cnt++] = "llc-encaps";
	else if(!strcmp(multiplex, "vc"))
		pppoa_argv[arg_cnt++] = "vc-encaps";
			
	pppoa_argv[arg_cnt++] = "qos";
	memset(qosstr, 0, sizeof(qosstr));
	if(!strcmp(qostype, "ubr"))
		strncpy(qosstr, "UBR", sizeof(qosstr));
	else if(!strcmp(qostype, "cbr"))
		strncpy(qosstr, "CBR", sizeof(qosstr));
	else if(!strcmp(qostype, "vbr"))
		strncpy(qosstr, "VBR", sizeof(qosstr));
	pppoa_argv[arg_cnt++] = qosstr;
	
	if(strcmp(qostype, "ubr"))
	{
		pppoa_argv[arg_cnt++] = "qos_pcr";
		pppoa_argv[arg_cnt++] = vpcr;
	}
	if(!strcmp(qostype, "vbr"))
	{
		pppoa_argv[arg_cnt++] = "qos_scr";
		pppoa_argv[arg_cnt++] = vscr;
	}		
	if(which_conn == which_tmp)
		pppoa_argv[arg_cnt++] = "defaultroute";
		
	pppoa_argv[arg_cnt++] = "mru";
	pppoa_argv[arg_cnt++] = pppoa_mtu;
	
	pppoa_argv[arg_cnt++] = "mtu";
	pppoa_argv[arg_cnt++] = pppoa_mtu;
	
/*
	pppoa_argv[arg_cnt++] = "maxfail";
	pppoa_argv[arg_cnt++] = "10";
*/		
	pppoa_argv[arg_cnt++] = "lcp-echo-interval";
	pppoa_argv[arg_cnt++] = "30";
	
	pppoa_argv[arg_cnt++] = "lcp-echo-failure";
	pppoa_argv[arg_cnt++] = "3";
	
	pppoa_argv[arg_cnt++] = "ipparam";
	pppoa_argv[arg_cnt++] = "0";
	
	if(!strcmp(a_demand, "1"))
	{
		char idletime[8];
		memset(idletime, 0, sizeof(idletime));
		sprintf(idletime, "%d", (atoi(a_idletime))*60);
		pppoa_argv[arg_cnt++] = "demand";
		pppoa_argv[arg_cnt++] = "holdoff";
		pppoa_argv[arg_cnt++] = "3";
		pppoa_argv[arg_cnt++] = "idle";
		pppoa_argv[arg_cnt++] = idletime;
	}
	/*
	else
	{
		pppoa_argv[arg_cnt++] = "lcp-echo-interval";
		pppoa_argv[arg_cnt++] = a_redialperiod;
	}
	*/
	pppoa_argv[arg_cnt++] = NULL;

	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
	
	_eval(pppoa_argv, NULL, 0, &ppp_pid[which_conn]);
	
	
	filename = malloc(strlen("/var/run/ppp.pid") + 3);
	memset(filename, 0, strlen("/var/run/ppp.pid"+3));
	if(filename == NULL)
		return;
	sprintf(filename, "/var/run/ppp%d.pid", which_conn);
	if (!(fp = fopen(filename, "a"))) 
	{
		perror(filename);
		return;
	}
	
	fprintf(fp, "%d\n", ppp_pid[which_conn]);
	fclose(fp);
	free(filename);
	filename = NULL;
	
	
	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%d", which_conn);
	
	ram_modify_status(WAN_IFNAME, pppdev);
	
	if(!strcmp(a_demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial
		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(1);
			nvram_set("action_service","");
		}
		if(which_conn == which_tmp)
		{
			FILE *fp;
			char filename[32];
			stop_wanledctrl();
			start_wanledctrl(which_conn, "ppp", "demand");
			
			//junzhao 2004.11.2 for wanledctrl
			nvram_set("wanled_index", &pppdev[3]);
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/tmp/ppp/link%d.begin", which_conn);
			fp = fopen(filename, "w");
			fclose(fp);

		}
		close(s);
	}
	else
	{
		printf("here is nodemand mode\n");
		if(status == BOOT)
		{
			printf("enter redial\n");
			start_redial(a_redialperiod, pppdev);
			if(which_conn == which_tmp)
			{
				stop_wanledctrl();
				start_wanledctrl(which_conn, "ppp", "keepalive");
				nvram_set("wanled_index", &pppdev[3]);
			}
		}
	}
}

void start_static(int mode)
{
	char devname[10];
	char *gateway = NULL;
	int timeout = 3;
	char dnsbuf[64];
	int randomtime;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	srand((int)time(0));
	randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));
	printf("random time %d\n", randomtime);
	sleep(randomtime);	
	
	memset(devname, 0, sizeof(devname));	
	memset(dnsbuf, 0, sizeof(dnsbuf));	

#ifndef CLIP_SUPPORT	
	sprintf(devname, "nas%d", which_conn);
#else
	if((mode == BRIDGED) || (mode == ROUTED))
		sprintf(devname, "nas%d", which_conn);
	else if(mode == CLIP)
		sprintf(devname, "atm%d", which_conn);
#endif
	
	if(mode == BRIDGED)
	{
		ifconfig(devname, IFUP,  b_ipaddr, b_netmask);
		gateway = b_gateway;
		sprintf(dnsbuf, "%s:%s", b_pdns, b_sdns);
                printf("$$$$$$$$$$$$the b_ipaddr=%s$$$$$$$$$$$$\n",b_ipaddr);
                syslog(LOG_INFO,"1nSTATICIP:%s",b_ipaddr);

	}
	else if(mode == ROUTED)
	{
		ifconfig(devname, IFUP,  r_ipaddr, r_netmask);
		gateway = r_gateway;
		sprintf(dnsbuf, "%s:%s", r_pdns, r_sdns);
                printf("$$$$$$$$$$$$the r_ipaddr=%s$$$$$$$$$$$$\n",r_ipaddr);
                syslog(LOG_INFO,"1nSTATICIP:%s",r_ipaddr);

	}
#ifdef CLIP_SUPPORT	
	else if(mode == CLIP)
	{
		char vpivci_str[10];
		memset(vpivci_str, 0, sizeof(vpivci_str));
		sprintf(vpivci_str, "%d.%d", vpivci_final_value[which_conn][0], vpivci_final_value[which_conn][1]);
		ifconfig(devname, IFUP,  c_ipaddr, c_netmask);
		usleep(10);
		//eval("ifconfig", devname, c_ipaddr, "netmask", c_netmask, "up");
		gateway = c_gateway;
		sprintf(dnsbuf, "%s:%s", c_pdns, c_sdns);
		eval("atmarp", "-s", c_gateway, vpivci_str);
		sleep(1);
	}
#endif

	init_mtu(devname, NULL);
	
#ifndef CLIP_SUPPORT
	mac_clone(devname);
#else
	if(mode != CLIP)
		mac_clone(devname);		
#endif

#ifdef GATEWAY_BEYOND_SUBNET_SUPPORT
	if(mode == BRIDGED)
	{
		if (!legal_ip_netmask(b_ipaddr, b_netmask, gateway)){
			route_add(devname, 0, gateway, "0.0.0.0", "255.255.255.255");
		}
	}
#endif
	//if(which_conn == which_tmp)
	{
		while (route_del(devname, 0, NULL, NULL, NULL) == 0);
		while (route_add(devname, 0, "0.0.0.0", gateway, "0.0.0.0") && timeout-- );
    	}
	//nvram_modify_status("wan_get_dns", dnsbuf);
	ram_modify_status(WAN_GET_DNS, dnsbuf);
	
	//add by lijunzhao
	if(which_conn == which_tmp)
	{
		stop_wanledctrl();
		start_wanledctrl(which_conn, "static", NULL);
		nvram_set("wanled_index", &devname[3]);
	}
	start_wan_done(devname);

#ifdef CWMP_SUPPORT
       /*add by peny*/
       	if(mode == BRIDGED)
	          inform_start_dhcpc(devname, b_ipaddr, which_conn); /* da_f@2005.3.4 modify */
        else if(mode == ROUTED)
	          inform_start_dhcpc(devname, r_ipaddr, which_conn); /* da_f@2005.3.4 modify */
#ifdef CLIP_SUPPORT
        else if(mode == CLIP)
	          inform_start_dhcpc(devname, c_ipaddr, which_conn); /* da_f@2005.3.4 modify */
#endif
#endif
	
}

int vcc_global_init()
{
	char *ptr = nvram_safe_get("vcc_config"), *tmpptr;
	int now_conn, i;

/*	
	//for the first time after boot 
	if(firstboot)
	{
		char wan_connection[4];
		memset(wan_connection, 0, sizeof(wan_connection));
		strncpy(wan_connection, nvram_safe_get("wan_connection"), sizeof(wan_connection));
		which_conn = atoi(wan_connection); 
	}
*/	
	now_conn = which_conn;
	
#ifdef LJZ_DEBUG
	printf("which_conn %d\n", which_conn);
#endif

	if(vcc_config_ptr)
	{
		free(vcc_config_ptr);
		vcc_config_ptr = NULL;
	}

	if((vcc_config_ptr = (struct vcc_config *)malloc(sizeof(struct vcc_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	
	memset(vcc_config_ptr, 0, sizeof(struct vcc_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			sscanf(tmpptr, "%5[^:]:%8[^:]:%20[^:]:%5[^:]:%8[^:]:%8[^:]:%8[^:]:%3[^:]:%3[^\n]", vvpi, vvci, encapmode, multiplex, qostype, vpcr, vscr, autodetect, applyonboot);
			break;
		}
	}
			
	/*
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(now_conn-- == 0)
		{
		#ifdef LJZ_DEBUG
			printf("word %s\n", word);
		#endif
			sscanf(word, "%5[^:]:%8[^:]:%20[^:]:%5[^:]:%8[^:]:%8[^:]:%8[^:]:%3[^:]:%3[^\n]", vvpi, vvci, encapmode, multiplex, qostype, vpcr, vscr, autodetect, applyonboot);
		#ifdef LJZ_DEBUG
                        printf("vpi_value=%s, vci_value=%s, encapmode=%s, multiplex=%s, qostype=%s, pcr=%s, scr=%s, autodetect=%s, applyonboot=%s\n", vvpi, vvci, encapmode, multiplex, qostype, vpcr, vscr, autodetect, applyonboot);
                #endif
			break;
		}
	}
	*/	
	
	return 0; 
}

int pppoe_global_init()
{
	char *ptr = nvram_safe_get("pppoe_config"), *tmpptr;
	int now_conn = which_conn, i;

	//junzhao 2005.5.16 for ':' setting
	char new_username[320];
	char new_password[320];
	char new_servicename[320];
	
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));

	if(pppoe_config_ptr)
	{
		free(pppoe_config_ptr);
		pppoe_config_ptr = NULL;
	}
	
	if((pppoe_config_ptr = (struct pppoe_config *)malloc(sizeof(struct pppoe_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(pppoe_config_ptr, 0, sizeof(struct pppoe_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			//sscanf(tmpptr, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^\n]", e_username, e_password, e_demand, e_idletime, e_redialperiod, e_servicename);
			
			//junzhao 2005.5.16 for ':' setting
			sscanf(tmpptr, "%320[^:]:%320[^:]:%3[^:]:%8[^:]:%8[^:]:%320[^\n]", new_username, new_password, e_demand, e_idletime, e_redialperiod, new_servicename);
			filter_name(new_username, e_username, sizeof(e_username), GET);
			filter_name(new_password, e_password, sizeof(e_password), GET);
			filter_name(new_servicename, e_servicename, sizeof(e_servicename), GET);

			break;
		}
	}
	
	/*
	foreach(word, nvram_safe_get("pppoe_config"), next)
	{
		if(now_conn-- == 0)
		{
			sscanf(word, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^\n]", e_username, e_password, e_demand, e_idletime, e_redialperiod, e_servicename);
		
		//#ifdef LJZ_DEBUG
                	printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s, servicename=%s\n", e_username, e_password, e_demand, e_idletime, e_redialperiod, e_servicename);
        	//#endif
			break;
		}
	}
	*/
	return 0;
}

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
int unip_pppoe_global_init()
{
	char *ptr = nvram_safe_get("unip_pppoe_config"), *tmpptr;
	int now_conn = which_conn, i;

	//junzhao 2005.5.16 for ':' setting
	char new_username[320];
	char new_password[320];
	char new_servicename[320];
	
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));

	
       printf("kirby debug:enter into unip_pppoe_global_init\n");
	if(unip_pppoe_config_ptr)
	{
		free(unip_pppoe_config_ptr);
		unip_pppoe_config_ptr = NULL;
	}
	
	if((unip_pppoe_config_ptr = (struct unip_pppoe_config *)malloc(sizeof(struct unip_pppoe_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(unip_pppoe_config_ptr, 0, sizeof(struct unip_pppoe_config));
	
	//junzhao 2004.8.4

	printf("kirby debug: unpppoe config load\n");
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		printf("ptr = %s\n",ptr);
		printf("tmpptr = %s\n",tmpptr);
		printf("now_conn = %d\n",now_conn);
		if(i == now_conn)
		{
			//sscanf(tmpptr, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^\n]", ue_username, ue_password, ue_demand, ue_idletime, ue_redialperiod, ue_servicename);
			//junzhao 2005.5.16 for ':' setting
			sscanf(tmpptr, "%320[^:]:%320[^:]:%3[^:]:%8[^:]:%8[^:]:%320[^\n]", new_username, new_password, ue_demand, ue_idletime, ue_redialperiod, new_servicename);
			
			filter_name(new_username, ue_username, sizeof(ue_username), GET);
			filter_name(new_password, ue_password, sizeof(ue_password), GET);
			filter_name(new_servicename, ue_servicename, sizeof(ue_servicename), GET);
				
			printf("kirby debug : unpppoe username = %s\n",ue_username);
			printf("kirby debug : unpppoe password = %s\n",ue_password);
			break;
		}
	}

	return 0;
}
int unip_pppoa_global_init()
{
	char *ptr = nvram_safe_get("unip_pppoa_config"), *tmpptr;
	int now_conn = which_conn, i;
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[320];
	char new_password[320];
	
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));

	if(unip_pppoa_config_ptr)
	{
		free(unip_pppoa_config_ptr);
		unip_pppoa_config_ptr = NULL;
	}
	
	if((unip_pppoa_config_ptr = (struct unip_pppoa_config *)malloc(sizeof(struct unip_pppoa_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(unip_pppoa_config_ptr, 0, sizeof(struct unip_pppoa_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			//sscanf(tmpptr, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^\n]", ua_username, ua_password, ua_demand, ua_idletime, ua_redialperiod);
			
			//junzhao 2005.5.16 for ':' setting
			sscanf(tmpptr, "%320[^:]:%320[^:]:%3[^:]:%8[^:]:%8[^\n]", new_username, new_password, ua_demand, ua_idletime, ua_redialperiod);
			filter_name(new_username, ua_username, sizeof(ua_username), GET);
			filter_name(new_password, ua_password, sizeof(ua_password), GET);

			break;
		}
	}
	

	return 0;
}

#endif


int pppoa_global_init()
{
	char *ptr = nvram_safe_get("pppoa_config"), *tmpptr;
	int now_conn = which_conn, i;
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[320];
	char new_password[320];
	
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));

	if(pppoa_config_ptr)
	{
		free(pppoa_config_ptr);
		pppoa_config_ptr = NULL;
	}
	
	if((pppoa_config_ptr = (struct pppoa_config *)malloc(sizeof(struct pppoa_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(pppoa_config_ptr, 0, sizeof(struct pppoa_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			//sscanf(tmpptr, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^\n]", a_username, a_password, a_demand, a_idletime, a_redialperiod);
			
			//junzhao 2005.5.16 for ':' setting
			sscanf(tmpptr, "%320[^:]:%320[^:]:%3[^:]:%8[^:]:%8[^\n]", new_username, new_password, a_demand, a_idletime, a_redialperiod);
			filter_name(new_username, a_username, sizeof(a_username), GET);
			filter_name(new_password, a_password, sizeof(a_password), GET);

			break;
		}
	}
	
	/*
	foreach(word, nvram_safe_get("pppoa_config"), next)
	{
		if(now_conn-- == 0)
		{
			sscanf(word, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^\n]", a_username, a_password, a_demand, a_idletime, a_redialperiod);
		
		#ifdef LJZ_DEBUG
                	printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s\n", a_username, a_password, a_demand, a_idletime, a_redialperiod);
        	#endif
			break;
		}
	}
	*/
	return 0;
}

int bridged_global_init()
{
	char *ptr = nvram_safe_get("bridged_config"), *tmpptr;
	int now_conn = which_conn, i;
	
	if(bridged_config_ptr)
	{
		free(bridged_config_ptr);
		bridged_config_ptr = NULL;
	}
	
	if((bridged_config_ptr = (struct bridged_config *)malloc(sizeof(struct bridged_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	
	memset(bridged_config_ptr, 0, sizeof(struct bridged_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			sscanf(tmpptr, "%3[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", dhcpenable, b_ipaddr, b_netmask, b_gateway, b_pdns, b_sdns);
			break;
		}
	}
	
	/*
	foreach(word, nvram_safe_get("bridged_config"), next)
	{
		if(now_conn-- == 0)
		{
			sscanf(word, "%3[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", dhcpenable, b_ipaddr, b_netmask, b_gateway, b_pdns, b_sdns);
		
		#ifdef LJZ_DEBUG
                	printf("dhcpenable=%s, ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", dhcpenable, b_ipaddr, b_netmask, b_gateway, b_pdns, b_sdns);
        	#endif
			break;
		}
	}
	*/
	return 0;
}

#ifdef CLIP_SUPPORT
int clip_global_init()
{
	char *ptr = nvram_safe_get("clip_config"), *tmpptr;
	int now_conn = which_conn, i;
	
	if(clip_config_ptr)
	{
		free(clip_config_ptr);
		clip_config_ptr = NULL;
	}
	
	if((clip_config_ptr = (struct clip_config *)malloc(sizeof(struct clip_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(clip_config_ptr, 0, sizeof(struct clip_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			sscanf(tmpptr, "%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", c_ipaddr, c_netmask, c_gateway, c_pdns, c_sdns);
			break;
		}
	}
	
	/*
	foreach(word, nvram_safe_get("clip_config"), next)
	{
		if(now_conn-- == 0)
		{
			sscanf(word, "%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", c_ipaddr, c_netmask, c_gateway, c_pdns, c_sdns);
		
		#ifdef LJZ_DEBUG
                	printf("ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", c_ipaddr, c_netmask, c_gateway, c_pdns, c_sdns);
        	#endif
			break;
		}
	}
	*/
	return 0;
}
#endif

int routed_global_init()
{
	char *ptr = nvram_safe_get("routed_config"), *tmpptr;
	int now_conn = which_conn, i;
	
	if(routed_config_ptr)
	{
		free(routed_config_ptr);
		routed_config_ptr = NULL;
	}
	
	if((routed_config_ptr = (struct routed_config *)malloc(sizeof(struct routed_config))) == NULL)
	{
		printf("out of memory\n");
		return -1;
	}
	memset(routed_config_ptr, 0, sizeof(struct routed_config));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == now_conn)
		{
			sscanf(tmpptr, "%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", r_ipaddr, r_netmask, r_gateway, r_pdns, r_sdns);
			break;
		}
	}
	
	//printf("enter routed global_init\n");
	/*
	foreach(word, nvram_safe_get("routed_config"), next)
	{
		if(now_conn-- == 0)
		{
			sscanf(word, "%32[^:]:%32[^:]:%32[^:]:%32[^:]:%32[^\n]", r_ipaddr, r_netmask, r_gateway, r_pdns, r_sdns);
		
		#ifdef LJZ_DEBUG
                	printf("ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", r_ipaddr, r_netmask, r_gateway, r_pdns, r_sdns);
        	#endif
			break;
		}
	}
	*/
	return 0;
}

/*
int check_adsl_status()
{
	FILE *fp = NULL;
	char buf[16];
	int ret = 0;
	
	fp = fopen("/proc/avalanche/avsar_modem_training", "r");
	if(fp == NULL)
	{
		printf("no avsar_status file!!\n");
		return ret;
	}
	memset(buf, 0, sizeof(buf));
	if(fread(buf, sizeof(char), sizeof(buf)*sizeof(char), fp) < 0)
	{
		printf("the file has no status\n");
		fclose(fp);
		return ret;
	} 
	//printf("read buf ---- %s\n", buf);
	if(!strncmp(buf, "SHOWTIME", strlen("SHOWTIME")))
		ret = 1;
	
	fclose(fp);
	return ret;
}
*/

void process_bridgedonly()
{
	char devname[10];
	char conf_name[16];
	memset(devname, 0, sizeof(devname));	
	sprintf(devname, "nas%d", which_conn);
	
	stop_upnp(0); //we needn't upnp now
	
	eval("iptables", "-t", "nat", "-F");
	eval("iptables", "-F");
	eval("iptables", "-t", "mangle", "-F");
	
	ram_modify_status(WAN_IFNAME, "Down");
	ram_modify_status(WAN_IPADDR, "0.0.0.0");
	ram_modify_status(WAN_NETMASK, "0.0.0.0");
	ram_modify_status(WAN_GATEWAY, "0.0.0.0");
	ram_modify_status(WAN_BROADCAST, "0.0.0.0");
	ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
	ram_modify_status(WAN_HWADDR, "00:00:00:00:00:00");
	
	mac_clone(devname);	
	
	{
	char *brctl_argv[] = 
		{ 
		   "brctl",
		   "addif", "br0",
		    devname,	     
		    NULL
		};
	_eval(brctl_argv, NULL, 0, NULL);
	}
	
	{
		char buf[]="1";
		int fd=open("/proc/Cybertan/bridge_only_enable", O_RDWR);
		if(fd > 0)
		{
			write(fd, buf, sizeof(buf));
			close(fd);
		}
   	}
	
	memset(conf_name, 0, sizeof(conf_name));
	sprintf(conf_name, "wanuptime%d", which_conn);
	if(nvram_match(conf_name, "0")) 
	{	
#if 0
		time_t now = time(NULL);
		char buf[32];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%ld", (long int)now);
		nvram_set(conf_name, buf);
#endif
		struct sysinfo info;
		char buf[32];

		if (sysinfo(&info) == -1)
			strcpy(buf, "0");
		else
			sprintf(buf, "%ld", info.uptime);
		
		nvram_set(conf_name, buf);
	}

	//start_wanledctrl(which_conn, "bridgedonly", NULL);
	return;
}

int
start_adslpolling()
{
	int ret;
	pid_t pid;
	char *adslpolling_argv[] = { "/tmp/adslpolling",
			      NULL
	};
	firstboot = -1;         //begin polling
	eval("killall", "adslpolling");
	unlink("/tmp/adslpolling");
	symlink("/sbin/rc", "/tmp/adslpolling");

	ret = _eval(adslpolling_argv, NULL, 0, &pid); 

	dprintf("done\n");
	return ret;
}

int
adslpolling_main(int argc, char **argv)
{
	char wan_hwaddr_get[18];
	memset(wan_hwaddr_get, 0, sizeof(wan_hwaddr_get));
	/*
	if(!macaddr_get(wan_hwaddr_get))
	{
		unsigned char buf[6];
		ether_atoe(wan_hwaddr_get, buf);
		memset(wan_hwaddr_get, 0, sizeof(wan_hwaddr_get));
		buf[5] = buf[5] + 1;
		ether_etoa(buf, wan_hwaddr_get);
		nvram_set("wan_def_hwaddr", wan_hwaddr_get);
		nvram_commit();
		printf("wan def hwaddr %s\n", wan_hwaddr_get);
	}
	*/
	{
	unsigned char buf[6];
	ether_atoe(nvram_safe_get("lan_hwaddr"), buf);
	if(++buf[5] == 0)
	{
		if(++buf[4] == 0)
			++buf[3];	
	}
	ether_etoa(buf, wan_hwaddr_get);
	usleep(100);
	nvram_set("wan_def_hwaddr", wan_hwaddr_get);
	//nvram_commit();
	printf("wan def hwaddr %s\n", wan_hwaddr_get);
	}
	
	printf("polling now .......\n");
	
	//junzhao 2004.7.28 for time record
	if(!check_adsl_status())
		nvram_set("adsluptime", "0");

	if(!secondenter)
	{
		nvram_set("wanuptime0", "0");
		nvram_set("wanuptime1", "0");
		nvram_set("wanuptime2", "0");
		nvram_set("wanuptime3", "0");
		nvram_set("wanuptime4", "0");
		nvram_set("wanuptime5", "0");
		nvram_set("wanuptime6", "0");
		nvram_set("wanuptime7", "0");
	}
	
	while(1)
	{
		sleep(1);
		if(check_adsl_status())
		{
			//junzhao 2004.7.28 for time record
			if(nvram_match("adsluptime", "0"))
			{
				time_t now = time(NULL);
				char buf[32];
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%ld", (long int)now);
				nvram_set("adsluptime", buf);
			}
			//junzhao 2004.10.11
                	syslog(LOG_INFO,"1rADSL:%s.", "SHOWTIME");
			break;
		}
	}
	printf("polling end .......\n");
	kill(1, SIGALRM);
	return -1;
}

void statusfile_init()
{
	//if(firstboot)
	{
		char buf[160], *cur=buf;
		FILE *fp = NULL;
		int i;
		
		mkdir("/tmp/status", 0777);
		
		memset(buf, 0 , sizeof(buf));
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "Down");
		if((fp = fopen(WAN_PVC_STATUS, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}

		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "Down");
		/*
		if((fp = fopen(WAN_PVC_ERRSTR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		*/
		if((fp = fopen(WAN_IFNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#ifdef MPPPOE_SUPPORT
		if((fp = fopen(WAN_MPPPOE_IFNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif	
	
	#ifdef IPPPOE_SUPPORT
		if((fp = fopen(WAN_IPPPOE_IFNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif	
	
		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "Down");
		if((fp = fopen(WAN_PVC_ERRSTR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		
		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "0.0.0.0");
		if((fp = fopen(WAN_IPADDR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_NETMASK, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_GATEWAY, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_BROADCAST, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#ifdef MPPPOE_SUPPORT
		if((fp = fopen(WAN_MPPPOE_IPADDR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_MPPPOE_NETMASK, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_MPPPOE_GATEWAY, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_MPPPOE_BROADCAST, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
	#ifdef IPPPOE_SUPPORT
		if((fp = fopen(WAN_IPPPOE_IPADDR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_IPPPOE_NETMASK, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_IPPPOE_GATEWAY, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		if((fp = fopen(WAN_IPPPOE_BROADCAST, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
	
	//junzhao 2004.8.17
	#ifdef VZ7
		if((fp = fopen(WAN_DHCPD_IPADDR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
	
		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "0.0.0.0:0.0.0.0");
		if((fp = fopen(WAN_GET_DNS, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#ifdef MPPPOE_SUPPORT
		if((fp = fopen(WAN_MPPPOE_GET_DNS, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif		
	#ifdef IPPPOE_SUPPORT
		if((fp = fopen(WAN_IPPPOE_GET_DNS, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif		

		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "0");
		if((fp = fopen(WAN_ENABLE_TABLE, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}

		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "None");
		if((fp = fopen(WAN_PPPOE_ACNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#ifdef MPPPOE_SUPPORT
		if((fp = fopen(WAN_MPPPOE_ACNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
	#ifdef IPPPOE_SUPPORT
		if((fp = fopen(WAN_IPPPOE_ACNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
		if((fp = fopen(WAN_PPPOE_SRVNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}

	//junzhao 2004.8.10
		if((fp = fopen(WAN_ENCAP, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		
	#ifdef MPPPOE_SUPPORT
		if((fp = fopen(WAN_MPPPOE_SRVNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif
	#ifdef IPPPOE_SUPPORT
		if((fp = fopen(WAN_IPPPOE_SRVNAME, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
	#endif

		memset(buf, 0 , sizeof(buf));
		cur = buf;
		for(i=0; i<MAX_CHAN; i++)
			cur += snprintf(cur, buf+sizeof(buf)-cur, "%s%s", cur==buf?"":" ", "00:00:00:00:00:00");
		if((fp = fopen(WAN_HWADDR, "w"))!=NULL)
		{
			fprintf(fp, buf);
			fclose(fp);
		}
		
		for(i=0; i<MAX_CHAN; i++)
		{
			ppp_pid[i] = -1;
			udhcpc_pid[i] = -1;
			sockfd_used[i] = -1;
		}
	}
}	

void stop_pvc_wan(void)
{
	int i;
	//which_conn = atoi(nvram_safe_get("wan_connection"));
	printf("&&&&&&&&&&&&&&&& now will stop wan connection %d &&&&&&&&&&&&&&\n", which_conn);
	if(firstboot != 1)
	{
		ram_modify_status(WAN_IFNAME, "Down");
		ram_modify_status(WAN_PVC_ERRSTR, "Down");
		ram_modify_status(WAN_PVC_STATUS, "Down");
		ram_modify_status(WAN_IPADDR, "0.0.0.0");
		ram_modify_status(WAN_NETMASK, "0.0.0.0");
		ram_modify_status(WAN_GATEWAY, "0.0.0.0");
		ram_modify_status(WAN_BROADCAST, "0.0.0.0");
		ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
		ram_modify_status(WAN_HWADDR, "00:00:00:00:00:00");	
		ram_modify_status(WAN_PPPOE_ACNAME, "None");
	#ifdef MPPPOE_SUPPORT	
		ram_modify_status(WAN_MPPPOE_IFNAME, "Down");
		ram_modify_status(WAN_MPPPOE_IPADDR, "0.0.0.0");
		ram_modify_status(WAN_MPPPOE_NETMASK, "0.0.0.0");
		ram_modify_status(WAN_MPPPOE_GATEWAY, "0.0.0.0");
		ram_modify_status(WAN_MPPPOE_BROADCAST, "0.0.0.0");
		ram_modify_status(WAN_MPPPOE_GET_DNS, "0.0.0.0:0.0.0.0");
		ram_modify_status(WAN_MPPPOE_ACNAME, "None");	
	#endif	
	#ifdef IPPPOE_SUPPORT	
		ram_modify_status(WAN_IPPPOE_IFNAME, "Down");
		ram_modify_status(WAN_IPPPOE_IPADDR, "0.0.0.0");
		ram_modify_status(WAN_IPPPOE_NETMASK, "0.0.0.0");
		ram_modify_status(WAN_IPPPOE_GATEWAY, "0.0.0.0");
		ram_modify_status(WAN_IPPPOE_BROADCAST, "0.0.0.0");
		ram_modify_status(WAN_IPPPOE_GET_DNS, "0.0.0.0:0.0.0.0");
		ram_modify_status(WAN_IPPPOE_ACNAME, "None");	
	#endif	
		//junzhao 2004.8.17
	#ifdef VZ7
		ram_modify_status(WAN_DHCPD_IPADDR, "0.0.0.0");
	#endif
	}
	if(sockfd_used[which_conn] >= 0)
	{
		if(firstboot != 1)
		{
			stop_wan();
			for(i=0; i<200000000; i++);
		#ifdef LJZ_DEBUG
			printf("stop_wan done!!!\n");
		#endif
		
			if(which_conn == atoi(nvram_safe_get("wan_enable_last")))
			{
				stop_wan_service();
			#ifdef LJZ_DEBUG
				printf("stop_wan_service done!!!\n");
			#endif
			}
		}	
		if(sockfd_used[which_conn] > 0)
		{
			close(sockfd_used[which_conn]);
		}
		sockfd_used[which_conn] = -1;                  
	       	vpivci_final_value[which_conn][0] = -1;					        vpivci_final_value[which_conn][1] = -1;		 			}
}
		
void
start_wan(int status)
{
	char wan_connection[3];
	char autosearchflag[16], autosearchlist[16];
	int j;
	//kirby
	char tr64_defconn[8];
	char tmp_value[4];
	char tempbuf[28];//xiaoqin add 2005.04.27
	int idef;

	//char wan_hwaddr_get[18];
	
	//which_conn = atoi(nvram_safe_get("wan_connection"));
	
	if(which_conn == 0)
		nvram_set("wan_dhcp_ipaddr", "0.0.0.0");
	
begin:	
	printf("-------now will start wan connection %d-------\n", which_conn);

	/***************** xiaoqin add 2005.04.27 ****************/
	sprintf(tempbuf, "pppox_change_userpasswd_%d", which_conn);
	nvram_set(tempbuf, "no");
	/************************ end ****************************/
	
	if(vcc_global_init() < 0)
		return;
	
	//disable this connection
	if(!strcmp(applyonboot, "0"))
	{
		ram_modify_status(WAN_ENABLE_TABLE, "0");
		return;
	}
	
	//waiting.....
	printf("waiting last conn to finish.............\n");
	if(which_conn && !secondenter)
	{
		for(j=0; j<150000000; j++);
	}
	printf("waiting done.............\n");

	ram_modify_status(WAN_ENABLE_TABLE, "1");
	
#ifdef    CPED_TR064_SUPPORT  
	strcpy(tmp_value, nvram_safe_get("default_connection_which"));
	
	if((strcmp(tmp_value,"1") == 0) &&(strcmp(tmp_value,"8")))
	{   /* kirby TR064 */
		   	memset(tr64_defconn,0,sizeof(tr64_defconn));
			strcpy(tr64_defconn,nvram_safe_get("default_conncetion_service"));
			idef = atoi(tr64_defconn);
			memset(wan_connection, 0, sizeof(wan_connection));
	              sprintf(wan_connection, "%d", idef);
	              nvram_set("wan_active_connection", wan_connection);
	              //junzhao 2004.4.3
	              nvram_set("wan_post_connection", wan_connection);
	              nvram_set("wan_enable_last", wan_connection);
	}
	else{   
#endif		
	      if(which_conn < atoi(nvram_safe_get("wan_active_connection")))
	      {
		      memset(wan_connection, 0, sizeof(wan_connection));
	             sprintf(wan_connection, "%d", which_conn);
	             nvram_set("wan_active_connection", wan_connection);
	             //junzhao 2004.4.3
	             nvram_set("wan_post_connection", wan_connection);
	             nvram_set("wan_enable_last", wan_connection);
		      /* kirby 2004/09 */
                    nvram_set("default_conncetion_service", wan_connection);		 
	       }
#ifdef    CPED_TR064_SUPPORT 		  // kirby 2004/09 
	}
#endif	
	
	/*
	if (nvram_match("router_disable", "1") || strcmp(wan_proto, "disabled") == 0) 
	{
		start_wan_done(wan_ifname);
		which_conn = atoi(wan_connection);
		goto final;
	}
	*/
	
//global buf setting
	if(!strcmp(encapmode, "1483bridged"))
	{
		if(bridged_global_init() < 0)
			return;
	}
	else if(!strcmp(encapmode, "routed"))
	{
		if(routed_global_init() < 0)
			return;
	}
	else if(!strcmp(encapmode, "pppoe"))
	{
		if(pppoe_global_init() < 0)
			return;
	}
	else if(!strcmp(encapmode, "pppoa"))
	{
		if(pppoa_global_init() < 0)
			return;
	}

#ifdef CLIP_SUPPORT
	else if(!strcmp(encapmode, "clip"))
	{
		if(clip_global_init() < 0)
			return;
	}
#endif
 	/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
       else if(!strcmp(encapmode, "unpppoe"))
	{
		if(unip_pppoe_global_init() < 0){
			printf("kirby debug : unpppoe global init failed\n");
			return;
		}
	}
	else if(!strcmp(encapmode, "unpppoa"))
	{
	       printf("unip pppoa global init\n");
		if(unip_pppoa_global_init() < 0){
			printf("kirby debug : unpppoa global init failed\n");
			return;
		}
	}   
#endif

//junzhao 2004.6.16 for autosearch on xml
	//if((which_conn == 0) && nvram_match("autosearchflag", "1")) // xml auto search
	
	memset(autosearchflag, 0, sizeof(autosearchflag));
	sprintf(autosearchflag, "autosearchflag%d", which_conn);
	
	if(nvram_match(autosearchflag, "1")) // xml auto search
	{
	#ifndef CLIP_SUPPORT
		if(strcmp(encapmode, "pppoa")) 
	#else
			
#ifdef UNNUMBERED_SUPPORT
			/* kirby for unip 2004/12.3 */
		if(strcmp(encapmode, "pppoa") && strcmp(encapmode, "clip")&& strcmp(encapmode, "unpppoa")) 	
#else

		if(strcmp(encapmode, "pppoa") && strcmp(encapmode, "clip")) 
#endif
	#endif
		{
			if(rfc2684_load(AUTOENABLE) < 0)
			{
				char buf[5];
				printf("autodetect rfc2684 load fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			printf("autodetect rfc2684_load end\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			if(!strcmp(encapmode, "pppoe"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "pppoe");
				start_pppoe(status, 0);
			#ifdef MPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}

			/* kirby for unip 2004/12.3 */
#ifdef UNNUMBERED_SUPPORT 
                     if(!strcmp(encapmode, "unpppoe"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "unpppoe");
				start_unpppoe(status, 0);
			}
#endif

			else if(!strcmp(encapmode, "routed"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "routed");
				start_static(ROUTED);
			}
			else if((!strcmp(encapmode, "1483bridged")) && (!strcmp(dhcpenable, "0")))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "static");
				start_static(BRIDGED);
			#ifdef IPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}
			else if((!strcmp(encapmode, "1483bridged")) && (!strcmp(dhcpenable, "1")))
			{	
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "dhcp");
				//junzhao 2005.2.5
				//start_dhcp();				
			#ifdef IPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}
		}
		else if(!strcmp(encapmode, "pppoa")) //pppoa
		{
			if(pppoa_load() < 0)
			{
				char buf[5];
				printf("autodetect pppoa load fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "pppoa");
			start_pppoa(AUTOENABLE, status);
		}
		/* kirby for unip 2004/12.3 */
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encapmode, "unpppoa")) //pppoa
		{
			if(pppoa_load() < 0)
			{
				char buf[5];
				printf("autodetect unip pppoa load fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "unpppoa");
			start_unpppoa(AUTOENABLE, status);
		}
#endif

	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip")) //clip
		{
			char *atmarpd_argv[] = {"atmarpd", NULL};
			char ifname[5];
			pid_t pid;
			
			if(!atmarpd_index)
				_eval(atmarpd_argv, NULL, 0, &pid);
			//atmarpd_num++;
			atmarpd_index |= (1<<which_conn);
			
			sleep(1);
			memset(ifname, 0, sizeof(ifname));
			sprintf(ifname, "atm%d", which_conn);
			eval("atmarp", "-c", ifname);
			sleep(1);
			
			if(clip_load() < 0)
			{
				char buf[5];
				printf("autodetect clip fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "clip");
			start_static(CLIP);
		}
	#endif
		goto increase;
	}

	// httpd process
	if(!strcmp(autodetect, "0"))  //no autodetect, see which mode!
	{
		printf("nonauto!!!\n");
		if(strcmp(encapmode, "pppoe") == 0) //pppoe proto
		{
			int ret;
			printf("rfc2684_load begin(pppoe,nonauto)\n");
		/*	
		#ifdef VZ7
			if((ret = rfc2684_load(AUTODISABLE)) == -1)
			{
				printf("rfc2684 load fail(pppoe,nonauto)\n");
				ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
				goto increase;			
			}
			else if(ret == -2)
			{
				//junzhao 2004.8.17
				if(DHCPAutoDetect() < 0)
				{
					printf("dhcp server autodetect fail\n");
				}
				else
					printf("dhcp server autodetect success\n");
				goto increase;
				//goto begin;
			}
		#else	
		*/
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(pppoe,nonauto)\n");
				ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
				goto increase;
			}
		//#endif
			printf("rfc2684_load end(pppoe,nonauto)\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "pppoe");
			start_pppoe(status, 0);
		#ifdef MPPPOE_SUPPORT
			start_pppoe2(status, 0);
		#endif
		}
		else if (strcmp(encapmode, "1483bridged") == 0) //1483bridged 
		{
			printf("rfc2684_load begin(1483bridged,nonauto)\n");
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(1483bridged,nonauto)\n");
				ram_modify_status(WAN_PVC_STATUS, "Down");
				goto increase;
			}
			printf("rfc2684_load end(1483bridged,nonauto)\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			if(!strcmp(dhcpenable, "1"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "dhcp");
				
				//junzhao 2005.2.5
				{
				int randomtime;
				srand((int)time(0));
				randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));
				printf("random time %d\n", randomtime);
				sleep(randomtime);	
				}
				
				start_dhcp();
			}
			else
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "static");
				start_static(BRIDGED);
			}
		#ifdef IPPPOE_SUPPORT
			start_pppoe2(status, 0);
		#endif
		}
		else if(strcmp(encapmode, "pppoa") == 0) //pppoa proto
		{
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "pppoa");
			start_pppoa(AUTODISABLE, status);
		}
	#ifdef CLIP_SUPPORT
		else if(strcmp(encapmode, "clip") == 0) //clip proto
		{
			char *atmarpd_argv[] = {"atmarpd", NULL};
			char ifname[5];
			char vpivci_str[10];
			pid_t pid;
			
			if(!atmarpd_index)
				_eval(atmarpd_argv, NULL, 0, &pid);
			//atmarpd_num++;
			atmarpd_index |= (1<<which_conn);
			
			sleep(1);
			memset(ifname, 0, sizeof(ifname));
			sprintf(ifname, "atm%d", which_conn);
			eval("atmarp", "-c", ifname);
			sleep(1);
			
	       		vpivci_final_value[which_conn][0] = atoi(vvpi);					vpivci_final_value[which_conn][1] = atoi(vvci);		 		
			memset(vpivci_str, 0, sizeof(vpivci_str));
			sprintf(vpivci_str, "%d.%d", atoi(vvpi), atoi(vvci));
			
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "clip");
			start_static(CLIP);
		}
	#endif
		else if(strcmp(encapmode, "routed") == 0)//1483 routed
		{
			printf("rfc2684_load begin(routed,nonauto)\n");
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(static,nonauto)\n");
				ram_modify_status(WAN_PVC_STATUS, "Down");
				goto increase;
			}
			printf("rfc2684_load end(routed,nonauto)\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "routed");
			start_static(ROUTED);
		}
		else if(strcmp(encapmode, "bridgedonly") == 0)//bridged only
		{
			printf("rfc2684_load begin(bridgedonly,nonauto)\n");
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(bridgedonly,nonauto)\n");
				ram_modify_status(WAN_PVC_STATUS, "Down");
				goto increase;
			}
			printf("rfc2684_load end(bridgedonly,nonauto)\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "bridged");
			process_bridgedonly();
		}
	}
	
              /* kirby for unip 2004/12.3 */
#ifdef UNNUMBERED_SUPPORT
              else if(strcmp(encapmode, "unpppoe") == 0) //pppoe proto
		{
			int ret;
			printf("rfc2684_load begin(unpppoe,nonauto)\n");
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(unpppoe,nonauto)\n");
				ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
				goto increase;
			}
		//#endif
			printf("rfc2684_load end(unpppoe,nonauto)\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "unpppoe");
			start_unpppoe(status, 0);
		}
		else if(strcmp(encapmode, "unpppoa") == 0) //pppoa proto
		{
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "unpppoa");
			start_unpppoa(AUTODISABLE, status);
		}	  
#endif

	else //need autodetect
	{
	#ifndef CLIP_SUPPORT
		if(strcmp(encapmode, "pppoa"))
	#else
		//if(strcmp(encapmode, "pppoa") && strcmp(encapmode, "clip"))
			/* kirby for unip 2004/12.3 */	
#ifdef UNNUMBERED_SUPPORT
              if(strcmp(encapmode, "pppoa") && strcmp(encapmode, "clip") && strcmp(encapmode, "unpppoa"))
#else
		if(strcmp(encapmode, "pppoa") && strcmp(encapmode, "clip"))
#endif
	#endif
		{
			printf("autodetect rfc2684_load begin\n");
			if(rfc2684_load(AUTOENABLE) < 0)
			{
				char buf[5];
				printf("autodetect rfc2684 load fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			printf("autodetect rfc2684_load end\n");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			if(!strcmp(encapmode, "pppoe"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "pppoe");
				start_pppoe(status, 0);
			#ifdef MPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}
			else if(!strcmp(encapmode, "routed"))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "routed");
				start_static(ROUTED);
			}
			else if((!strcmp(encapmode, "1483bridged")) && (!strcmp(dhcpenable, "0")))
			{
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "static");
				start_static(BRIDGED);
			#ifdef IPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}
			else if((!strcmp(encapmode, "1483bridged")) && (!strcmp(dhcpenable, "1")))
			{	
				//junzhao 2004.8.10
				ram_modify_status(WAN_ENCAP, "dhcp");
				//junzhao 2005.2.5
				//start_dhcp();				
			#ifdef IPPPOE_SUPPORT
				start_pppoe2(status, 0);
			#endif
			}
		}
		else if(!strcmp(encapmode, "pppoa"))//pppoa
		{
			if(pppoa_load() < 0)
			{
				char buf[5];
				printf("autodetect pppoa load fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "pppoa");
			start_pppoa(AUTOENABLE, status);
		}
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))//clip
		{
			char *atmarpd_argv[] = {"atmarpd", NULL};
			char ifname[5];
			pid_t pid;
			
			if(!atmarpd_index)
				_eval(atmarpd_argv, NULL, 0, &pid);
			//atmarpd_num++;
			atmarpd_index |= (1<<which_conn);
			
			sleep(1);
			memset(ifname, 0, sizeof(ifname));
			sprintf(ifname, "atm%d", which_conn);
			eval("atmarp", "-c", ifname);
			sleep(1);
			
			if(clip_load() < 0)
			{
				char buf[5];
				printf("autodetect clip fail\n");
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s:%s", "0", "-1");
				nvram_modify_status("wan_autoresult", buf);
				ram_modify_status(WAN_PVC_STATUS, "Down");
				nvram_modify_vpivci(0, 0);
				goto increase;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "clip");
			start_static(CLIP);
		}
	#endif
	}

increase:
	if(!strcmp(autodetect, "0"))
	{
		char buf[5];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s:%s", "0", "-1");
		nvram_modify_status("wan_autoresult", buf);
	}
	
//final:
//	if(firstboot)
//		firstboot = 0;   //have pass the first boot
	return;
}

void
start_wan_service(void)
{
	stop_ntp();
	stop_ddns();
	stop_email_alert();
	//stop_upnp(2);
	start_ntp();
	start_ddns();
	start_email_alert();
#ifdef PARENTAL_CONTROL_SUPPORT
 	stop_parental_control();	
 	start_parental_control();	
#endif	

}

#define LJZ_DEBUG
void
start_wan_done(char *wan_ifname)
{
	char wanipaddr[20], wannetmask[20], wangateway[20], wanbroadcast[20];
	int which = atoi(&wan_ifname[3]);
	char encap[15];
    	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	int timeout = 5;
	
	//junzhao 2004.7.28 for time record
	if(which < MAX_CHAN)
	{
		char conf_name[16];
		memset(conf_name, 0, sizeof(conf_name));
		sprintf(conf_name, "wanuptime%d", which);
		if(nvram_match(conf_name, "0") && ((!strstr(wan_ifname, "ppp") || check_ppp_link(which)))) 
		{	
#if 0
			time_t now = time(NULL);
			char buf[32];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "%ld", (long int)now);
			nvram_set(conf_name, buf);
#endif
			struct sysinfo info;
			char buf[32];

			if (sysinfo(&info) == -1)
				strcpy(buf, "0");
			else
				sprintf(buf, "%ld", info.uptime);
			
			nvram_set(conf_name, buf);
		}
	}

	//junzhao 2004.6.3
	/*
	if((tmpdata = select_tmp) != 32)
	{
		int data = 0;
		data |= which;
		if(strstr(wan_ifname, "ppp"))
			data |= (1<<PPP_MASK);
		if(data != tmpdata)
			nvram_set("active_connection_selection", "32");
	}
	*/
	/* save the interface name */
	//nvram_modify_status("wan_ifname", wan_ifname);

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)	
	memset(encap, 0, sizeof(encap));
	check_cur_encap(which_conn, encap);
	if(which > MAX_CHAN-1)
	{
		if(!strcmp(encap, "pppoe"))
		{
		/* save the interface name */
		#if defined(MPPPOE_SUPPORT)
			ram_modify_status(WAN_MPPPOE_IFNAME, wan_ifname);
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			ram_modify_status(WAN_IPPPOE_IFNAME, wan_ifname);
		#endif
		}
	}
	else
#endif
		ram_modify_status(WAN_IFNAME, wan_ifname);
		
	/* get the info from real interface, and save to nvram */
	memset(wanipaddr, 0, sizeof(wanipaddr));
	memset(wannetmask, 0, sizeof(wannetmask));
	memset(wangateway, 0, sizeof(wangateway));
	memset(wanbroadcast, 0, sizeof(wanbroadcast));

	while(wanstatus_info_get(wan_ifname, "ipaddr", &wanipaddr[0]) < 0 && timeout--);
#ifdef LJZ_DEBUG
	printf("which_conn %d wanipaddr %s\n", which_conn, wanipaddr);
#endif
	//nvram_modify_status("wan_ipaddr", wanipaddr);
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which > MAX_CHAN-1)
	{
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
		/* save the interface name */
			ram_modify_status(WAN_MPPPOE_IPADDR, wanipaddr);
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			ram_modify_status(WAN_IPPPOE_IPADDR, wanipaddr);
		#endif
		}
	}
	else
#endif
		ram_modify_status(WAN_IPADDR, wanipaddr);
	
	timeout = 5;
	while(wanstatus_info_get(wan_ifname, "netmask", &wannetmask[0]) < 0 && timeout--);
#ifdef LJZ_DEBUG
	printf("which_conn %d wannetmask %s\n", which_conn, wannetmask);
#endif
	//nvram_modify_status("wan_netmask", wannetmask);
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which > MAX_CHAN-1)
	{
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			/* save the interface name */
			ram_modify_status(WAN_MPPPOE_NETMASK, wannetmask);
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			ram_modify_status(WAN_IPPPOE_NETMASK, wannetmask);
		#endif
		}
	}
	else
#endif
		ram_modify_status(WAN_NETMASK, wannetmask);
	
	timeout = 5;
	while(wanstatus_info_get(wan_ifname, "gateway", &wangateway[0]) < 0 && timeout--);
	//junzhao 2004.8.5 //for dhcp mode....
	if(strcmp(wangateway, "0.0.0.0"))
	{
		
#ifdef LJZ_DEBUG
	printf("which_conn %d wangateway %s\n", which_conn, wangateway);
#endif
	//nvram_modify_status("wan_gateway", wangateway);
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which > MAX_CHAN-1)
	{
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			/* save the interface name */
			ram_modify_status(WAN_MPPPOE_GATEWAY, wangateway);
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			ram_modify_status(WAN_IPPPOE_GATEWAY, wangateway);
		#endif
		}
	}
	else
#endif
		ram_modify_status(WAN_GATEWAY, wangateway);
	
	}
	
	timeout = 5;
	while(wanstatus_info_get(wan_ifname, "broadcast", &wanbroadcast[0]) < 0 && timeout--);
#ifdef LJZ_DEBUG
	printf("which_conn %d wanbroadcast %s\n", which_conn, wanbroadcast);
#endif
	//nvram_modify_status("wan_broadcast", wanbroadcast);
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which > MAX_CHAN-1)
	{
		/* save the interface name */
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			ram_modify_status(WAN_MPPPOE_BROADCAST, wanbroadcast);
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			ram_modify_status(WAN_IPPPOE_BROADCAST, wanbroadcast);
		#endif
		}
	}
	else
#endif
		ram_modify_status(WAN_BROADCAST, wanbroadcast);
    	
	/* Start firewall */
	
	stop_firewall();
	start_firewall();

#if defined(CONFIG_CWMP)
	stop_cwmp();
	start_cwmp();	/* CPE WAN Management protocol */
#endif

	if(which < MAX_CHAN)
	{
		/* Set additional wan static routes if need */
		set_routes();
	}
	
	dns_to_resolv(which_conn);
	stop_dhcpd();
	start_dhcpd();

	/* Restart DNS proxy */
	stop_dns();
	start_dns();

	/* add for IGMP Proxy 2004-10-10 */
#ifdef IGMP_PROXY_SUPPORT
	stop_igmp_proxy();
	sleep(2);
	start_igmp_proxy();
#endif

	//junzhao 2004.7.29 restart ipsec
	if(!strstr(wan_ifname, "ppp") || check_ppp_link(which))
	{

		//junzhao 2004.7.23 
		/* Restart RIP */
		stop_zebra();
		start_zebra();
	#ifdef NOMAD_SUPPORT
	//zhs add for quickvpn,3-28-2005
                set_ipsec_def_ipt();
                att_ipsec0_t0_def_wan();
        //end by zhs
	#endif
		stop_ipsec();
		start_ipsec();

	#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
		stop_gre();
		enable_gre();
	#endif

		//junzhao 2004.7.29 restart qos
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		stop_qos();
		start_qos();
	#endif
		
		//junzhao 2004.9.14
		stop_upnp(2);
	}
	
	if((which == which_conn) && (which == which_tmp))
     	{	
		if(strstr(wan_ifname, "ppp"))
		{
			FILE *fp;
			char linkfile[18];
			memset(linkfile, 0, sizeof(linkfile));
			sprintf(linkfile, "/tmp/ppp/link%d", which);
			fp = fopen(linkfile, "r");
			if(fp != NULL)
			{
			#ifdef IPCP_NETMASK_SUPPORT
				int unip_enable = atoi(nvram_safe_get("ipcp_netmask_enable"));
			#endif	
				start_wan_service();
				fclose(fp);

			#ifdef IPCP_NETMASK_SUPPORT
				//junzhao 2005.1.24
				if(which_conn == 0 && unip_enable == 1)
				{	
					
	      				printf("Start Unnumbered IP Mode\n");
		
					/* just for debug */	
					eval("killall", "junzhaotest");
					
	      				/* only active pvc set UNIP mode, this will be applied */
             				if(unip_start(which) < 0)
	  	   				printf("Unnumbered IP mode failed\n");
				}
			#endif	
			}
		}
		else
			start_wan_service();
	}
	//wwzh add for WAN_UPGRADE
#ifdef WAN_UPGRADE
{
	char pid_buff[12];
	int pid;
	FILE *file;
	
	file = fopen("/var/run/httpd.pid", "r");
	if (file != NULL)
	{
		fgets(pid_buff, sizeof(pid_buff), file);
		pid = atoi(pid_buff);
		kill(pid, SIGUSR2);
		fclose(file);
	}
}
#endif
         dprintf("done\n");
}

void
stop_wan(void)
{
	//junzhao 2004.8.10
	char encap[15];
	memset(encap, 0, sizeof(encap));
	check_encap(which_conn, encap);

	if(!strcmp(encap, "None"))
		return;
	
	//junzhao 2004.11.2 for wanledctrl
	if(which_conn == atoi(nvram_safe_get("wanled_index")))
		stop_wanledctrl();
	
	if(!strcmp(encap, "pppoe") || !strcmp(encap, "pppoa"))
	{
		char ifname[5];
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "ppp%d", which_conn);
		down_ipsec(ifname, which_conn);
		stop_redial(which_conn);
		stop_pppox(which_conn);
	}
	
	/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
       if(!strcmp(encap, "unpppoe") || !strcmp(encap, "unpppoa")){
	   	char ifname[5];
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "ppp%d", which_conn);
		down_ipsec(ifname, which_conn);
		stop_redial(which_conn);
		stop_pppox(which_conn);
       }
#endif

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(!strcmp(encap, "pppoe") || !strcmp(encap, "static") || !strcmp(encap, "dhcp"))
	{
		stop_redial(MAX_CHAN + which_conn);
		stop_pppox(MAX_CHAN + which_conn);
	}
#endif 
	
	unlink("/tmp/ppp/redial");
	
	if(!strcmp(encap, "dhcp"))
	{
		char ifname[6]; 
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "nas%d", which_conn);
		down_ipsec(ifname, which_conn);
		stop_dhcpc(which_conn);
	}
	
	//junzhao 2004.11.2 for del ppp status file
	{
	char filename[32];
	
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d", which_conn);
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.down", which_conn);
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.down", MAX_CHAN + which_conn);
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d", MAX_CHAN + which_conn);
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.idle", which_conn);
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.idle", MAX_CHAN + which_conn);
	unlink(filename);
	
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/authfail%d", which_conn); 
	unlink(filename);
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/authfail%d", MAX_CHAN + which_conn); 
	unlink(filename);

	memset(filename, 0, strlen("/var/run/udhcpc.down"+3));
	sprintf(filename, "/var/run/udhcpc%d.down", which_conn);
	unlink(filename);
	memset(filename, 0, strlen("/var/run/udhcpc.up"+3));
	sprintf(filename, "/var/run/udhcpc%d.up", which_conn);
	unlink(filename);
	}

#ifdef CLIP_SUPPORT
	if(!strcmp(encap, "clip"))
	{
		char ifname[5];
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "atm%d", which_conn);
		eval("ifconfig", ifname, "down");
		down_ipsec(ifname, which_conn);
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		down_qos(ifname, which_conn);
	#endif

	#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
		stop_gre();
	#endif
	
		//junzhao 2004.9.8
		stop_firewall();
		start_firewall();
		
		//junzhao 2004.9.14
		stop_upnp(1);
		
		/*add by peny*/
	#ifdef CWMP_SUPPORT
	    	stop_inform_dhcpc(which_conn);
	#endif
		/*end by peny*/
		
		atmarpd_index &= ~(1<<which_conn);
		if(!atmarpd_index)
			eval("killall", "atmarpd");
	}
#endif
	if(!strcmp(encap, "static") || !strcmp(encap, "routed"))
	{
		char ifname[6]; 
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "nas%d", which_conn);
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		down_qos(ifname, which_conn);
	#endif
		down_ipsec(ifname, which_conn);
		
	#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
		stop_gre();
	#endif
		//junzhao 2004.9.8
		stop_firewall();
		start_firewall();
		
		/*add by peny*/
	#ifdef CWMP_SUPPORT
	    	stop_inform_dhcpc(which_conn);
	#endif
		/*end by peny*/

		//junzhao 2004.9.14
		stop_upnp(1);
	}

	{
	//junzhao 2004.7.15
	int select_tmp = atoi(nvram_safe_get("active_connection_selection"));
	
	if(select_tmp != 32)
	{
		int data = 0;
		if((data | which_conn) == select_tmp)
			nvram_set("active_connection_selection", "32");
	}
	}

	//junzhao 2004.7.28 for wan uptime record
	{
	char buf[16];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "wanuptime%d", which_conn);
	nvram_set(buf, "0");
	}
	
	dprintf("done\n");
}

void stop_wan_service(void)
{
	//junzhao 2004.11.2
	//stop_wanledctrl();
	
	stop_ntp();
	stop_ddns();
	stop_email_alert();

#if defined(CONFIG_CWMP)
	stop_cwmp();	/* CPE WAN Management protocol */
#endif

#ifdef PARENTAL_CONTROL_SUPPORT
 	stop_parental_control();	
#endif
}	

int
set_routes(void)
{
	char word[80], *tmp;
	char *ipaddr, *netmask, *gateway, *metric, *ifname;

	foreach(word, nvram_safe_get("static_route"), tmp) {
		if (!strcmp(word, "$")) continue;
		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;
/*
		ifname = gateway;
		gateway = strsep(&ifname, ":");
		if (!ifname)
			continue;	
*/			
//		route_add(ifname, atoi(metric), ipaddr, gateway, netmask);
		eval("/usr/bin/route", "add", "-net", ipaddr, "netmask", netmask, "gw", gateway, "metric", metric); //modified by leijun
	}

	return 0;
}

//added by leijun
int
del_routes(char *route)
{
	char word[80], *tmp;
	char buf[256]; //added by leijun
	char *ipaddr, *netmask, *gateway, *metric, *ifname;

	foreach(word, nvram_safe_get("static_route"), tmp) {
		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;
/*
		ifname = metric;
		metric = strsep(&ifname, ":");
		if (!metric || !ifname)
			continue;
*/
//		route_del(ifname, atoi(metric), ipaddr, gateway, netmask);
		eval("/usr/bin/route", "del", "-net", ipaddr, "netmask", netmask, "gw", gateway, "metric", metric);
	}

	return 0;
}

int get_macaddr(char *devname, char *macaddr)
{
	int s;
	struct ifreq ifr;
	
	if(!devname || !macaddr)
		return -1;

	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;
	
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;	
	
	if(ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
	{
		close(s);
		printf("ioctl siocgifhwaddr fail %d %s\n", errno, strerror(errno));
		return -1;
	}
	ether_etoa(ifr.ifr_hwaddr.sa_data, macaddr);
	
	close(s);
	return 0;
}

int macaddr_modify(char *devname, char *hwaddr)
{
	int s;
	struct ifreq ifr;

	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;
	
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(s);
		return -1;
	}
        ifr.ifr_flags &= ~(IFF_UP|IFF_RUNNING);
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
	{							                        	close(s);
		return -1;
	}       
				
	ether_atoe(hwaddr, ifr.ifr_hwaddr.sa_data);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;	
	
	if(ioctl(s, SIOCSIFHWADDR, &ifr) < 0)
	{
		close(s);
		return -1;
	}
	
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(s);
		return -1;
	}
        ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
	{							                        	close(s);
		return -1;
        }
	close(s);
	return 0;
}

int mac_clone(char *devname)
{
	char word[32], *next;
	char *macenable = NULL;
	char *macaddr = NULL;
	//char wan_hwaddr_get[18];
	int s;
	struct ifreq ifr;
	char macaddrbuf[20];
	int which = which_conn;
	
	if(!devname)
		return -1;
	
	memset(word, 0, sizeof(word));
	
	foreach(word, nvram_safe_get("macclone_enable"), next)
	{
		if(which -- == 0)
		{
			macenable = word;
			break;
		}
	}
	
	if(!strcmp(macenable, "0"))
	{
		/*
		memset(wan_hwaddr_get, 0, sizeof(wan_hwaddr_get));
		if(!macaddr_get(wan_hwaddr_get))
		{
			unsigned char buf[6];
			ether_atoe(wan_hwaddr_get, buf);
			memset(wan_hwaddr_get, 0, sizeof(wan_hwaddr_get));
			buf[5] = buf[5] + 1;
			ether_etoa(buf, wan_hwaddr_get);
			macaddr = wan_hwaddr_get;
			nvram_set("wan_def_hwaddr", wan_hwaddr_get);
			nvram_commit();
			printf("wan def hwaddr %s\n", macaddr);
		}
		*/
		macaddr = nvram_safe_get("wan_def_hwaddr");
		//printf("restore def hwaddr %s\n", macaddr);
	}
	else
	{
		which = which_conn;
		memset(word, 0, sizeof(word));
		foreach(word, nvram_safe_get("macclone_addr"), next)
		{
			if(which -- == 0)
			{
				macaddr = word;
				break;
			}
		}
	}
	
	if(!strcmp(macaddr, "00:00:00:00:00:00"))
		goto macclone_foot;
	
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		goto macclone_foot;
	
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(s);
		goto macclone_foot;
	}
        ifr.ifr_flags &= ~(IFF_UP|IFF_RUNNING);
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
	{							                        	close(s);
		goto macclone_foot;
	}       
				
	ether_atoe(macaddr, ifr.ifr_hwaddr.sa_data);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;	
	
	if(ioctl(s, SIOCSIFHWADDR, &ifr) < 0)
	{
		close(s);
		printf("ioctl siocsifmtu fail %d %s\n", errno, strerror(errno));
		goto macclone_foot;
	}
	
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(s);
		goto macclone_foot;
	}
        ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
	{							                        	close(s);
		goto macclone_foot;
        }
	close(s);

macclone_foot:	
	memset(macaddrbuf, 0, sizeof(macaddrbuf));
	if(get_macaddr(devname, macaddrbuf) < 0)
		return -1;
	//nvram_modify_status("wan_hwaddr", macaddrbuf);
	ram_modify_status(WAN_HWADDR, macaddrbuf);
	
	return 0;
}

int init_mtu(char *devname, char *buf)
{
	char word[16], *next, mtu_auto[3], mtu_value[8];
	int which = which_conn;

	foreach(word, nvram_safe_get("mtu_config"), next)
	{
		if(which -- == 0)
		{
			sscanf(word, "%3[^:]:%10[^\n]", mtu_auto, mtu_value);
			break;
		}
	}
	
	if(strstr(devname, "pppoe"))  //576<mtu<1454(japan) || 1492(other)
	{
		if(buf == NULL)
			return -1;
		
		if(!strcmp(mtu_auto, "1"))     //Auto config
		{
#if COUNTRY == JAPAN
                        nvram_modify_status("mtu_config", "1:1454");	
			strcpy(buf, "1454");
#else
                        nvram_modify_status("mtu_config", "1:1492");	
			strcpy(buf, "1492");
#endif
		}
                else  //Manual
		{ 
#if COUNTRY == JAPAN
                        if(atoi(mtu_value) > 1454)
			{
                        	nvram_modify_status("mtu_config", "0:1454");
				strcpy(buf, "1454");
			}
#else
                        if(atoi(mtu_value) > 1492)
			{
                        	nvram_modify_status("mtu_config", "0:1492");
				strcpy(buf, "1492");
			}
#endif
			else if(atoi(mtu_value) < 576)
			{
                        	nvram_modify_status("mtu_config", "0:576");
				strcpy(buf, "576");
			}
			else
				strncpy(buf, mtu_value, strlen(mtu_value));

		}
	}
	else if(strstr(devname, "pppoa"))  //576<mtu<1454(japan) || 1492(other)
	{
		if(buf == NULL)
			return -1;
		
		if(!strcmp(mtu_auto, "1"))     //Auto config
		{
#if COUNTRY == JAPAN
                        nvram_modify_status("mtu_config", "1:1454");	
			strcpy(buf, "1454");
#else
                        nvram_modify_status("mtu_config", "1:1500");	
			strcpy(buf, "1500");
#endif
		}
                else  //Manual
		{ 
#if COUNTRY == JAPAN
                        if(atoi(mtu_value) > 1454)
			{
                        	nvram_modify_status("mtu_config", "0:1454");
				strcpy(buf, "1454");
			}
#else
                        if(atoi(mtu_value) > 1500)
			{
                        	nvram_modify_status("mtu_config", "0:1500");
				strcpy(buf, "1500");
			}
#endif
			else if(atoi(mtu_value) < 576)
			{
                        	nvram_modify_status("mtu_config", "0:576");
				strcpy(buf, "576");
			}
			else
				strncpy(buf, mtu_value, strlen(mtu_value));

		}
	}

	else // 576 < mtu < 1500
	{
		int s;
		int mtutmp = atoi(mtu_value);
		struct ifreq ifr;
		
		if(!strcmp(mtu_auto, "1")) //Auto config
		{
                        nvram_modify_status("mtu_config", "1:1500");	// set max value
			mtutmp = 1500;
		}
		else // Manual
		{
                        if(mtutmp > 1500)
			{
                        	nvram_modify_status("mtu_config", "0:1500");	// set max value
				mtutmp = 1500;
			}
			else if(mtutmp < 576)
			{
                        	nvram_modify_status("mtu_config", "0:576");	// set max value
				mtutmp = 576;
			}
		}

		if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			return -1;
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_name, devname, IFNAMSIZ);
		ifr.ifr_mtu = mtutmp;
		
		if(ioctl(s, SIOCSIFMTU, &ifr) < 0)
		{
			close(s);
			printf("ioctl siocsifmtu fail %d %s\n", errno, strerror(errno));
			return -1;
		}
		close(s);
	}
	return 0;
}


/* Trigger Connect On Demand */
int
//force_to_dial(void)
force_to_dial(int num)
{
	int which = which_conn;
	char *gateway=NULL;
	char readbuf[160], *tmpstr = readbuf;
	
	if(num == 1)
	{
		int i;
		if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
		{
			printf("no buf in %s!\n", WAN_GATEWAY);
			return 0;
		}
		//junzhao 2004.8.4
		for(i=0; i<MAX_CHAN; i++)
		{
			gateway = strsep(&tmpstr, " ");
			if(i == which)
				break;
		}

		/*
		foreach(word, readbuf, next)
		{
			if(which -- == 0)
			{
				gateway = word;
				break;
			}
		}
		*/
	}
	
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else if(num == 2)	
	{
		int i;
		char encap[15];
		memset(encap, 0, sizeof(encap));
		check_cur_encap(which_conn, encap);
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			if(!file_to_buf(WAN_MPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_MPPPOE_GATEWAY);
				return 0;
			}
			//junzhao 2004.8.4
			for(i=0; i<MAX_CHAN; i++)
			{
				gateway = strsep(&tmpstr, " ");
				if(i == which)
					break;
			}

			/*
			foreach(word, readbuf, next)
			{
				if(which -- == 0)
				{
					gateway = word;
					break;
				}
			}
			*/
		#else
			return 0;
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			if(!file_to_buf(WAN_IPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_GATEWAY);
				return 0;
			}	
			
			//junzhao 2004.8.4
			for(i=0; i<MAX_CHAN; i++)
			{
				gateway = strsep(&tmpstr, " ");
				if(i == which)
					break;
			}

			/*
			foreach(word, readbuf, next)
			{
				if(which -- == 0)
				{
					gateway = word;
					break;
				}
			}
			*/
		#else
			return 0;
		#endif
		}

	}
#endif	

	printf("connect ppp:  %s\n", gateway);
	eval("ping", "-c", "1", gateway);
				
	sleep(1);
	//printf("haha..........why??\n");

	return 0;
}
#if 0
void start_pppoe3(int status)
{
	char *pppoe_argv[32];
	int arg_cnt = 0;
	char devname[10];
	int timeout = 5;
	pid_t pid;
	char pppdev[8];
	char word[280], *next; 
	char index_str[4];
	char username[64], password[64], servicename[64], domainname[64], demand[3], idletime[8], redialperiod[8], pppoe_mtu[8];
	int index = which_conn;
	
	foreach(word, nvram_safe_get("mpppoe_enable"), next)
	{
		if(index -- == 0)
			break;
	}
	if(!strcmp(word, "0"))
		return;
	
	index = which_conn;
	foreach(word, nvram_safe_get("mpppoe_config"), next)
	{
		if(index -- == 0)
			break;
	}
	
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(demand, 0, sizeof(demand));
	memset(idletime, 0, sizeof(idletime));
	memset(redialperiod, 0, sizeof(redialperiod));
	memset(servicename, 0, sizeof(servicename));
	memset(domainname, 0, sizeof(domainname));
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
	
	memset(index_str, 0, sizeof(index_str));
	sprintf(index_str, "%d", which_conn);
	
	sscanf(word, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^:]:%64[^\n]:", username, password, demand, idletime, redialperiod, servicename, domainname);
		
	//junzhao 2004.4.28
	if(!strcmp(username, "") || !strcmp(password, ""))
		return;	
	
	//int status_tmp = status;
	memset((char *)devname, 0, sizeof(devname));
	sprintf(devname, "nas%d", which_conn);
	
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
	init_mtu("ppp", pppoe_mtu);

	printf("in start_pppoe3 %s %s %s %s %s %s %s %s\n", devname, username, password, demand, idletime, redialperiod, servicename, domainname);
	
	{
	int i;
	printf("before start_mpppoe waiting.........!\n");
	for(i=0; i<600000000; i++);
	}
	//now launch pppoe
	pppoe_argv[arg_cnt++] = "pppd";
	pppoe_argv[arg_cnt++] = "plugin";
	pppoe_argv[arg_cnt++] = "pppoe";
	pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = "user";
	pppoe_argv[arg_cnt++] = username;
	pppoe_argv[arg_cnt++] = "password";
	pppoe_argv[arg_cnt++] = password;
	pppoe_argv[arg_cnt++] = "nodetach";
	pppoe_argv[arg_cnt++] = "usepeerdns";
	pppoe_argv[arg_cnt++] = "unit";
	pppoe_argv[arg_cnt++] = index_str;   //which conn
		
	//if(nvram_match("wan_ppp_defaultroute", "1"))
	//	pppoe_argv[arg_cnt++] = "defaultroute";
		
	pppoe_argv[arg_cnt++] = "mru";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "mtu";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "30";
	
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "3";

	pppoe_argv[arg_cnt++] = "ipparam";
	pppoe_argv[arg_cnt++] = "0";

	if(strcmp(servicename, ""))
	{
		pppoe_argv[arg_cnt++] = "pppoe_srv_name";
		pppoe_argv[arg_cnt++] = servicename;
	}		
		
	if(!strcmp(demand, "1"))
	{
		char idletime_tmp[8];
		memset(idletime_tmp, 0, sizeof(idletime_tmp));
		sprintf(idletime_tmp, "%d", (atoi(idletime))*60);
		pppoe_argv[arg_cnt++] = "demand";
		pppoe_argv[arg_cnt++] = "holdoff";
		pppoe_argv[arg_cnt++] = "3";
		pppoe_argv[arg_cnt++] = "idle";
		pppoe_argv[arg_cnt++] = idletime_tmp;
	}
	pppoe_argv[arg_cnt++] = NULL;
	
	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
	
	_eval(pppoe_argv, NULL, 0, &pid);
	
	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%s", index_str);
	
#if defined(MPPPOE_SUPPORT)
	ram_modify_status(WAN_MPPPOE_IFNAME, pppdev);
#endif	
	if (!strcmp(demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial
		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(1);
			nvram_set("action_service","");
		}
		
		close(s);
	}
	else
	{
		//printf("here is nodemand mode\n");
		//printf("status------- %d %d   %d   %d\n", status_tmp, status, BOOT, REDIAL);
		if(status != REDIAL)
		{
			printf("enter redial\n");
			start_redial(redialperiod, pppdev);
		}
	}
}
#endif

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
void start_pppoe2(int status, int way)
{
	char *pppoe_argv[32];
	int arg_cnt = 0;
	char devname[10];
	int timeout = 5;
	pid_t pid;
	char pppdev[8];
	char word[280], *next; 
	char index_str[4];
	char username[64], password[64], servicename[64], domainname[64], demand[3], idletime[8], redialperiod[8], pppoe_mtu[8];
	int index = which_conn;
	char encap[15];
	char *filename;
	FILE *fp;
	char *ptr, *tmpptr;
	int i;
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[320];
	char new_password[320];
	char new_servicename[320];
	
	//junzhao 2004.6.14
	//int randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));
	
	memset(encap, 0, sizeof(encap));
	check_cur_encap(which_conn, encap);

	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("mpppoe_enable"), next)
		{
			if(index -- == 0)
				break;
		}
		if(!strcmp(word, "0"))
			return;
	
		index = which_conn;
		
		ptr = nvram_safe_get("mpppoe_config");
		//junzhao 2004.8.4
		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == index)
				break;
		}
		
		/*
		foreach(word, nvram_safe_get("mpppoe_config"), next)
		{
			if(index -- == 0)
				break;
		}
		*/
	#else 
		return; 
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("ipppoe_enable"), next)
		{
			if(index -- == 0)
				break;
		}
		if(!strcmp(word, "0"))
			return;
	
		index = which_conn;
		
		ptr = nvram_safe_get("ipppoe_config");
		//junzhao 2004.8.4
		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == index)
				break;
		}
		
		/*
		foreach(word, nvram_safe_get("ipppoe_config"), next)
		{
			if(index -- == 0)
				break;
		}
		*/
	#else 
		return; 
	#endif
	}
	
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
	memset(demand, 0, sizeof(demand));
	memset(idletime, 0, sizeof(idletime));
	memset(redialperiod, 0, sizeof(redialperiod));
	memset(servicename, 0, sizeof(servicename));
	memset(domainname, 0, sizeof(domainname));
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
	
	memset(index_str, 0, sizeof(index_str));
	sprintf(index_str, "%d", 8 + which_conn);
	
	//junzhao 2005.5.16 for ':' setting
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	
	//sscanf(word, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^:]:%64[^\n]:", username, password, demand, idletime, redialperiod, servicename, domainname);
	//sscanf(tmpptr, "%64[^:]:%64[^:]:%3[^:]:%8[^:]:%8[^:]:%64[^:]:%64[^\n]:", username, password, demand, idletime, redialperiod, servicename, domainname);
	
	//junzhao 2005.5.16 for ':' setting
	sscanf(tmpptr, "%320[^:]:%320[^:]:%3[^:]:%8[^:]:%8[^:]:%320[^:]:%64[^\n]:", new_username, new_password, demand, idletime, redialperiod, new_servicename, domainname);
	filter_name(new_username, username, sizeof(username), GET);
	filter_name(new_password, password, sizeof(password), GET);
	filter_name(new_servicename, servicename, sizeof(servicename), GET);
	
	if(!strcmp(username, "") || !strcmp(password, ""))
		return;	
	
	memset((char *)devname, 0, sizeof(devname));
	sprintf(devname, "nas%d", which_conn);
	
	memset(pppoe_mtu, 0, sizeof(pppoe_mtu));
	/* da_f 2005.7.14 modify. ppp -> pppoe */
	init_mtu("pppoe", pppoe_mtu);

	printf("in start_pppoe2 %s %s %s %s %s %s %s %s\n", devname, username, password, demand, idletime, redialperiod, servicename, domainname);
	
	{
	int i;
	if(way == 0)
	{
		printf("before start_mpppoe waiting.........!\n");
		for(i=0; i<100000000; i++);
		printf("waiting done.........!\n");
	}
	}
	//now launch pppoe
	pppoe_argv[arg_cnt++] = "pppd";
	pppoe_argv[arg_cnt++] = "plugin";
	pppoe_argv[arg_cnt++] = "pppoe";
	//pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = "user";
	pppoe_argv[arg_cnt++] = username;
	pppoe_argv[arg_cnt++] = "password";
	pppoe_argv[arg_cnt++] = password;
	pppoe_argv[arg_cnt++] = "nodetach";
	pppoe_argv[arg_cnt++] = "usepeerdns";
	pppoe_argv[arg_cnt++] = "unit";
	pppoe_argv[arg_cnt++] = index_str;   //which conn
		
	//if(nvram_match("wan_ppp_defaultroute", "1"))
	//	pppoe_argv[arg_cnt++] = "defaultroute";
		
	pppoe_argv[arg_cnt++] = "mru";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "mtu";
	pppoe_argv[arg_cnt++] = pppoe_mtu;
	
	pppoe_argv[arg_cnt++] = "lcp-echo-interval";
	pppoe_argv[arg_cnt++] = "30";
	
	pppoe_argv[arg_cnt++] = "lcp-echo-failure";
	pppoe_argv[arg_cnt++] = "3";

	pppoe_argv[arg_cnt++] = "ipparam";
	pppoe_argv[arg_cnt++] = "1";

	if(strcmp(servicename, ""))
	{
		printf("servicename not null!!!!!!!!\n");
		pppoe_argv[arg_cnt++] = "pppoe_srv_name";
		pppoe_argv[arg_cnt++] = servicename;
	}		
		
	if(!strcmp(demand, "1"))
	{
		char idletime_tmp[8];
		memset(idletime_tmp, 0, sizeof(idletime_tmp));
		sprintf(idletime_tmp, "%d", (atoi(idletime))*60);
		pppoe_argv[arg_cnt++] = "demand";
		pppoe_argv[arg_cnt++] = "holdoff";
		pppoe_argv[arg_cnt++] = "3";
		pppoe_argv[arg_cnt++] = "idle";
		pppoe_argv[arg_cnt++] = idletime_tmp;
	}
	pppoe_argv[arg_cnt++] = devname;
	pppoe_argv[arg_cnt++] = NULL;
	
	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
	
	_eval(pppoe_argv, NULL, 0, &pid);
	
	filename = malloc(strlen("/var/run/ppp.pid") + 3);
	memset(filename, 0, strlen("/var/run/ppp.pid"+3));
	if(filename == NULL)
		return;
	sprintf(filename, "/var/run/ppp%s.pid", index_str);
	if (!(fp = fopen(filename, "w"))) 
	{
		perror(filename);
		return;
	}
	
	fprintf(fp, "%ld", pid);
	
	fclose(fp);
	free(filename);
	filename = NULL;

	memset(pppdev, 0, sizeof(pppdev));
	sprintf(pppdev, "ppp%s", index_str);
	
	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		ram_modify_status(WAN_MPPPOE_IFNAME, pppdev);
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		ram_modify_status(WAN_IPPPOE_IFNAME, pppdev);
	#endif
	}
	if (!strcmp(demand, "1")) 
	{
		int s;
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			return;
		
		/* Wait for pppx to be created */
		while (ifconfig(pppdev, IFUP, NULL, NULL) && timeout--)
			sleep(1);
		
		start_wan_done(pppdev);

		// if user press Connect" button from web, we must force to dial
		if(nvram_match("action_service","start_pppoe"))
		{
			sleep(3);
			force_to_dial(2);
			nvram_set("action_service","");
		}
		
		close(s);
	}
	else
	{
		if(status != REDIAL)
		{
			printf("enter redial\n");
			start_redial(redialperiod, pppdev);
		}
	}
}
#endif

int add_next_gw()
{
	char if_word[MAX_CHAN][6], if_readbuf[60], index[3];
	int i, which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	int timeout = 5;
	
	if(!file_to_buf(WAN_IFNAME, if_readbuf, sizeof(if_readbuf)))
		return -1;

	memset(if_word, 0, sizeof(if_word));
	sscanf(if_readbuf, "%s %s %s %s %s %s %s %s", if_word[0], if_word[1], if_word[2], if_word[3], if_word[4], if_word[5], if_word[6], if_word[7]);
	
#ifdef    CPED_TR064_SUPPORT  
	for(i=0; i<MAX_CHAN; i++)
#else	
       for(i=which_tmp+1; i<MAX_CHAN; i++)
#endif	   	

	{
		if(!strcmp(if_word[i], "Down") || ((strstr(if_word[i], "ppp") && !check_ppp_proc(i))))
			continue;
		else
		{
			char gw_readbuf[160], *gw_ptr, *tmpstr = gw_readbuf;
			int j;
			if(!file_to_buf(WAN_GATEWAY, gw_readbuf, sizeof(gw_readbuf)))
				return -1;
			
			//junzhao 2004.8.4
			for(j=0; j<MAX_CHAN; j++)
			{
				gw_ptr = strsep(&tmpstr, " ");
				if(i == j)
				{
					if(strcmp(gw_ptr, "0.0.0.0"))
						while(route_add(if_word[i], 0, "0.0.0.0", gw_ptr, "0.0.0.0") && timeout--);
					break;
				}
			}

			/*
			foreach(gw_word, gw_readbuf, next)
			{
				if(j -- == 0)
				{
					if(strcmp(gw_word, "0.0.0.0"))
						while(route_add(if_word[i], 0, "0.0.0.0", gw_word, "0.0.0.0") && timeout--);
					break;
				}
			}
			*/
		}
		break;
	}	

	memset(index, 0, sizeof(index));
	sprintf(index, "%d", i);
	nvram_set("wan_active_connection", index);
#ifdef    CPED_TR064_SUPPORT  	
	/* kirby 2004/09 */
       nvram_set("default_conncetion_service", index);
#endif

	nvram_set("wan_enable_last", index);
	return (i>=MAX_CHAN?0:1);
}

	
int defaultgw_table_sort()
{
	int i, which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	char gw_readbuf[140], if_readbuf[60], gw_word[MAX_CHAN][16], if_word[MAX_CHAN][6];
	int timeout = 5;
	
	if(which_tmp == MAX_CHAN)
		return -1;
	
	//printf("enter default gw sort\n");
	memset(gw_word, 0, sizeof(gw_word));
	memset(if_word, 0, sizeof(if_word));
	if(!file_to_buf(WAN_GATEWAY, gw_readbuf, sizeof(gw_readbuf)))
	{
		printf("read wan_gateway fail\n");
		return -1;
	}
	if(!file_to_buf(WAN_IFNAME, if_readbuf, sizeof(if_readbuf)))
	{
		printf("read wan_ifname fail\n");
		return -1;
	}
	//printf("if_readbuf %s, gw_readbuf %s\n", if_readbuf, gw_readbuf);
	
	sscanf(gw_readbuf, "%s %s %s %s %s %s %s %s", gw_word[0], gw_word[1], gw_word[2], gw_word[3], gw_word[4], gw_word[5], gw_word[6], gw_word[7]);
	sscanf(if_readbuf, "%s %s %s %s %s %s %s %s", if_word[0], if_word[1], if_word[2], if_word[3], if_word[4], if_word[5], if_word[6], if_word[7]);
	
	for(i=MAX_CHAN-1; i>=0; i--)
	{
		//printf("%d: gw_word %s\n", i, gw_word[i]);
		if(!strcmp(gw_word[i], "0.0.0.0"))
			continue;
				
		//printf("%d: if_word %s\n", i, if_word[i]);
		if(!strcmp(if_word[i], "Down"))
			continue;
		
		//printf("%d: if_word %s, gw_word %s\n", i, if_word[i], gw_word[i]);
		while(route_del(if_word[i], 0, NULL, NULL, NULL) == 0);
		if(i == which_tmp)
			while(route_add(if_word[i], 0, "0.0.0.0", gw_word[i], "0.0.0.0") && timeout--);
		usleep(5);
	}		
	return 0;
}	
