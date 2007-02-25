/*
 * Miscellaneous services
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: services.c,v 1.5 2005/08/25 09:04:16 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wait.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <cyutils.h>
#include <code_pattern.h>
#include <cy_conf.h>
#include <rc.h>
#include "pvc.h"
#include "ipsec_rc.h"
#include "ledapp.h"
//add by lzh;
#include <sys/sysinfo.h>
#include <time.h>

char rc_entry_config_buf[100];
char rc_rtype_config_buf[100];
char rc_ltype_config_buf[100];
char rc_sg_config_buf[100];
char rc_keytype_config_buf[250];
char rc_ipsecadv_config_buf[200];
char ipaddress[20];
char rc_wan_ipaddress[20];
char rc_wan_netmask[20];
char rc_wan_broadcast[20];
char rc_wan_gateway[20];
char rc_wan_ifname[20];

char del_status[80];

//add by lzh;
unsigned int firstsetlanip = 1;

int ppp_ever_used[MAX_CHAN] = {-1,-1,-1,-1,-1,-1,-1,-1};
//modify by junzhao 2004.3.11
int sockfd_ever_used[MAX_CHAN] = {-1,-1,-1,-1,-1,-1,-1,-1};
extern int sockfd_used[MAX_CHAN];

#ifdef CLIP_SUPPORT
extern int atmarpd_index;
#endif

#ifdef IPCP_NETMASK_SUPPORT
//junzhao 2005.1.24 
//for unip dhcpd lease time change
int unip_started = 0;
#endif


#define AUTOKEY_FILE "/tmp/rc_auto"
#define MANUALKEY_FILE "/tmp/rc_manual"
#define IPSEC_START_FILE "/tmp/rc_ipsec"
#define PRESHARED_FILE "/tmp/ipsec.secrets"

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)
extern pid_t ppp_pid[MAX_CHAN];
extern pid_t udhcpc_pid[MAX_CHAN];
extern int which_conn;
extern int mac_clone(char *devname);

extern struct vcc_config *vcc_config_ptr;
extern struct pppoe_config *pppoe_config_ptr;
extern struct pppoa_config *pppoa_config_ptr;
extern struct bridged_config *bridged_config_ptr;
extern struct routed_config *routed_config_ptr;

extern int firstboot;
extern int secondenter;

//extern char origin_ledstatus[5];
void enable_disable_pppoe_pass();
void enable_disable_icmp_block(int);
void StrToHex( char *source, char *target, int len )
{
	char ch[28], tmp1[3];
	int i,offset,i0;
	
	if( len % 2)
	{
		offset = 1;
		ch[0]='0';
	}
	else
		offset = 0;
	
	strcpy( (char *)&ch[offset], source );

	// transfer string to hex
	for( i=0 ; i < (len + offset); i+=2 )
	{
		strncpy( tmp1, &ch[i], 2 );
		tmp1[2]=0;
		sscanf( tmp1, "%2x", &i0 );
		(*target) = i0;
		target++;
	} 
}

int
adjust_dhcp_range(void)
{
	struct in_addr ipaddr, netaddr, netmask;

	char *lan_ipaddr = nvram_safe_get("lan_ipaddr");
	char *lan_netmask = nvram_safe_get("lan_netmask");
	char *dhcp_num = nvram_safe_get("dhcp_num");
	char *dhcp_start = nvram_safe_get("dhcp_start");

	int legal_start_ip, legal_end_ip, legal_total_ip, dhcp_start_ip;
	int set_dhcp_start_ip=0, set_dhcp_num=0;
	int adjust_ip = 0, adjust_num = 0;

	inet_aton(lan_ipaddr, &netaddr);
        inet_aton(lan_netmask, &netmask);
        inet_aton(dhcp_start, &ipaddr);
	
	legal_total_ip = 254 - get_single_ip(lan_netmask,3);
	legal_start_ip = (get_single_ip(lan_ipaddr,3) & get_single_ip(lan_netmask,3)) + 1;
	legal_end_ip = legal_start_ip + legal_total_ip - 1;
	dhcp_start_ip = atoi(dhcp_start);

	dprintf("legal_total_ip=[%d] legal_start_ip=[%d] legal_end_ip=[%d] dhcp_start_ip=[%d]\n", 
		legal_total_ip, legal_start_ip, legal_end_ip, dhcp_start_ip);

        if ((dhcp_start_ip > legal_end_ip) || (dhcp_start_ip < legal_start_ip)){
		dprintf("Illegal DHCP Start IP: We need to adjust DHCP Start ip.\n");
		set_dhcp_start_ip = legal_start_ip;
		adjust_ip = 1;
		if(atoi(dhcp_num) > legal_total_ip){
			dprintf("DHCP num is exceed, we need to adjust.");
			set_dhcp_num = legal_total_ip;
			adjust_num = 1;
		}
	}
	else{
		dprintf("Legal DHCP Start IP: We need to check DHCP num.\n");
		if((atoi(dhcp_num) + dhcp_start_ip) > legal_end_ip){
			dprintf("DHCP num is exceed, we need to adjust.\n");
			set_dhcp_num = legal_end_ip - dhcp_start_ip + 1;
			adjust_num = 1;
		}
	}

	if(adjust_ip){
		char ip[20];
		dprintf("set_dhcp_start_ip=[%d]\n", set_dhcp_start_ip);	
		snprintf(ip, sizeof(ip), "%d", set_dhcp_start_ip);
		nvram_set("dhcp_start", ip);
	}
	if(adjust_num){
		char num[5];
		dprintf("set_dhcp_num=[%d]\n", set_dhcp_num);	
		snprintf(num, sizeof(num), "%d", set_dhcp_num);
		nvram_set("dhcp_num", num);
	}
	
	return 1;
}


#ifdef CLOUD_SUPPORT
void start_cloud_dhcprelay(int lantype) //lantype-- 0:static 1:dhcp 2:dhcprelay
{
	char dhcp_cloud_server[16];
	memset(dhcp_cloud_server, 0, sizeof(dhcp_cloud_server));
	strcpy(dhcp_cloud_server, nvram_safe_get("cloud_dhcp_server"));
	
	//printf("junzhao debug: %d  %s\n", lantype, dhcp_cloud_server);
	
	if(lantype == 2)

		/* zhangbin 2005.5.11 */
		//eval("dhcrelay", dhcp_cloud_server, nvram_safe_get("dhcpserver_ipaddr"));
		eval("dhcrelay", "-i",CLOUD_LANFACE, "-i", CLOUD_VPNFACE, nvram_safe_get("dhcpserver_ipaddr"));
	else
	{
		/* zhangbin 2005.5.11 */
		printf("will start dhcrelay!\n");
		//eval("dhcrelay", "-i", CLOUD_WLANFACE, "-i", CLOUD_GREFACE, dhcp_cloud_server);
		eval("dhcrelay", "-i", CLOUD_LANFACE, "-i", CLOUD_VPNFACE, dhcp_cloud_server);
	}
}	
#endif

int start_dhcprelay()
{
	char *dhcp_server_ip = NULL;
	dhcp_server_ip = nvram_safe_get("dhcpserver_ipaddr");
	eval("dhcrelay", dhcp_server_ip);
	return 0;
}
#if 0
/////////////////////////////////////////////////////////guohong
int get_string(char *name,char *buf,int  index,int iterm_num )
{
        char vcccbuf [1024];
        char tmpbuf[1024];

        char *p;
        char *p1;
        int i;

         if (index >iterm_num || index <= 0)  return 0;
          
         //memset (buf ,0,1024);
         memset (tmpbuf,0,1024);
         
         strcpy(vcccbuf,name);
        // strcpy(vcccbuf,nvram_get(name));
         
//printf ("vcccbuf=%s\n",vcccbuf);
         p = vcccbuf;
         for (i = 0 ;i<=index-1; i++)
         {
              
         	p1 = strstr(p," ");
         	
         	if (i ==( index -1))
         	{
         	   if (p1)         	         
                     *p1 =0;
                     strcpy(buf ,p);
                    break;        
         		}    
         	p = p1+1;
         }
//printf("get config:buf=%s\n",buf);

          return 1;

}
#endif

unsigned long str2hex(char *str)
{
	unsigned long hexval = 0;
	for(; *str; str ++){
		if(*str >= '0' && *str <= '9'){
			hexval = hexval << 4 | (*str - '0');
		}
		if(*str >= 'a' && *str <= 'f'){
			hexval = hexval << 4 | (*str - 'a' + 10);
		}
		if(*str >= 'A' && *str <= 'F'){
			hexval = hexval << 4 | (*str - 'A' + 10);
		}
	}
	return hexval;
}
//add by lzh;
static time_t get_sys_time()
{
	time_t t;
	struct sysinfo info;
	
	t = NULL;
	sysinfo(&info);
	return (info.uptime);
}
//-------------------------------------------------
static void WaitForLanDetectDhcp(void)
{
	time_t start,current;
	
	start = get_sys_time();
        printf("start wait for detect dhcp:start=%d\n",start);

	while (1)
	{
		current = get_sys_time();
		if ((current - start) >= 5) break;
		if (nvram_match("AutoDetect_dhcp","0")) break;
	}
	printf("wait detect dhcp over:time=%d\n",current);	
}
//---------------------------------------------------------
int
start_dhcpcc(void)
{
	char devname[10];
	char pidfile[32];
	char *tmp=NULL;
	int i;
	FILE *fp;

	//judge auto detect LAN dhcp server;
	if (nvram_match("lan_detect_dhcp","0")||(firstsetlanip == 0))
	{
		start_dhcpd();
		return 0;
	}
	if (nvram_match("router_disable","1"))
		return 0;

	memset((char *)pidfile,0,sizeof(pidfile));
	sprintf(pidfile,"/var/run/udhcpcc.pid");

	{
		/* da_f 2005.4.26 fix bug */
		pid_t pid;
		char *dhcp_argv[]=
		{
			"udhcpc",
			"-i","br0",
			"-p",pidfile,
			NULL
		};
		_eval(dhcp_argv,NULL,0,&pid);
	}
	dprintf("done\n");
	WaitForLanDetectDhcp();
	return 0;	
}	
int 
stop_dhcpcc(void)
{
	int ret;
	char pidfile[32],pidstr[6];
	char *tmp=NULL;
	FILE *fp;
#if 0
	if (!nvram_match("lan_proto","static"))
	{
		ret = stop_dhcpd();
		dprintf("done\n");
		return ret;
	}
#else
	stop_dhcpd();
#endif
	memset(pidfile,0,sizeof(pidfile));
	sprintf(pidfile,"/var/run/udhcpcc.pid");
	
	fp = fopen(pidfile,"r");
	if (fp == NULL)
	{
		printf("No udhcpcc.pid file\n");
		return 0;
	}	
	memset(pidstr,0,sizeof(pidstr));
	if (fread(pidstr,1,sizeof(pidstr),fp) > 0)
	{
		if ((tmp = strstr(pidstr,"\n")) != NULL)
			/* zhangbin 2005.3.8 */
			*tmp = '\0';
				
	}
	fclose(fp);
	eval("kill","-9",pidstr);	
	unlink(pidfile);

	dprintf("done\n");
	return ret;
}	
//end add by lzh;
int
start_dhcpd(void)
{
	struct dns_lists *dns_list = NULL;
        FILE *fp;

//        char buf[512];
        char flag[4];
        char tmpbuf[128];
        char *p,*p1;
//        char databuf[512];
//        char ip[32];
//        char mac[32];
//        char hostname[64];
        int i,j;
        unsigned long tmp_wan_num = 0;
        struct in_addr t_addr;

	//add by lzh;
	printf("dhcpd:auto_search_ip=%d,firstsetlanip=%d\n",atoi(nvram_safe_get("aut_search_ip")),firstsetlanip);
	
	if (nvram_match("auto_search_ip","1"))
	{
		if (firstsetlanip == 1)
		{
			if (SetLanIpbyDhcp() > 0)
			{
				//printf("success search a free lan ip\n");
				firstsetlanip = 0;
				kill(1,SIGHUP);
				return 0;
			}else 
				printf("fail to search or same to lan ip\n");			
		}else if (firstsetlanip == 0)
		{
			firstsetlanip = 1;
		}
	}
	//2004-0609 leijun
	if (!(fp = fopen("/proc/Cybertan/half_bridge_enable", "w"))) {
		cprintf("error to open /proc/Cybertan/half_bridge_enable");
	}else {
		fprintf(fp, "%d", 0);
		fclose(fp);
	}

	if (!(fp = fopen("/proc/Cybertan/wan_ip_addr", "r"))) {
		cprintf("error to open /proc/Cybertan/wan_ip_addr");
//		return;
	}else {
		i = fread(tmpbuf, 1, 32, fp);
		fclose(fp);
		tmpbuf[i] = '\0';
		tmp_wan_num = str2hex(tmpbuf);
//		cprintf("leijun----wan_ip_addr = %8d\n", tmp_wan_num);
	}

	if (nvram_match("router_disable", "1") || nvram_invmatch("lan_proto", "dhcp"))

#ifdef CLOUD_SUPPORT
	if (nvram_match("router_disable", "1"))
		return 0;
		
	/* zhangbin 2005.5.11 */
	//if(nvram_match("cloud_enable", "1") && nvram_match("cloud_dhcp_enable", "1"))
	if(nvram_match("cloud_enable", "1"))
	{
		
		if(nvram_match("lan_proto", "static"))
		{
			start_cloud_dhcprelay(0);
			return 0;
		}
		else if(nvram_match("lan_proto", "dhcp"))
		{
			start_cloud_dhcprelay(1);
		}
		else if(nvram_match("lan_proto", "dhcprelay"))
		{
			start_cloud_dhcprelay(2);
			return 0;
		}
	}
	else
	{	
		if (nvram_match("router_disable", "1") || nvram_match("lan_proto", "static"))
			return 0;

		//cjg dhcp_relay 2004.4.22
		if(nvram_match("lan_proto", "dhcprelay"))
		{
			start_dhcprelay();
			return 0;
		}
	}
#else	
	if (nvram_match("router_disable", "1") || nvram_match("lan_proto", "static"))
		return 0;

	//cjg dhcp_relay 2004.4.22
	if(nvram_match("lan_proto", "dhcprelay"))
	{
		start_dhcprelay();
		return 0;
	}
#endif
	
	/* Automatically adjust DHCP Start IP and num when LAN IP or netmask is changed */
	//adjust_dhcp_range();

	dprintf("%s %d.%d.%d.%s %s %s\n",
		nvram_safe_get("lan_ifname"),
		get_single_ip(nvram_safe_get("lan_ipaddr"),0),
		get_single_ip(nvram_safe_get("lan_ipaddr"),1),
		get_single_ip(nvram_safe_get("lan_ipaddr"),2),
		nvram_safe_get("dhcp_start"),
		nvram_safe_get("dhcp_end"),
		nvram_safe_get("lan_lease"));

	/* Touch leases file */
	if (!(fp = fopen("/tmp/udhcpd.leases", "a"))) {
		perror("/tmp/udhcpd.leases");
		return errno;
	}
	fclose(fp);

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/udhcpd.conf", "w"))) {
		perror("/tmp/udhcpd.conf");
		return errno;
	}
	fprintf(fp, "pidfile /var/run/udhcpd.pid\n");
	fprintf(fp, "start %d.%d.%d.%s\n", get_single_ip(nvram_safe_get("lan_ipaddr"),0),
					   get_single_ip(nvram_safe_get("lan_ipaddr"),1),
					   get_single_ip(nvram_safe_get("lan_ipaddr"),2),
					   nvram_safe_get("dhcp_start"));
	fprintf(fp, "end %d.%d.%d.%d\n", get_single_ip(nvram_safe_get("lan_ipaddr"),0),
			      		 get_single_ip(nvram_safe_get("lan_ipaddr"),1),
			 	         get_single_ip(nvram_safe_get("lan_ipaddr"),2),
					 atoi(nvram_safe_get("dhcp_start")) + atoi(nvram_safe_get("dhcp_num")) - 1);
	fprintf(fp, "max_leases 254\n");	// Need to adjust ?
	fprintf(fp, "interface %s\n", nvram_safe_get("lan_ifname"));
#ifdef STB_SUPPORT
        fprintf(fp, "stb_support %s\n", nvram_safe_get("stb_enable"));
        fprintf(fp, "stb_server %s\n", nvram_safe_get("telus_config"));

#endif
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "auto_time 30\n");		// N seconds to update lease table
	fprintf(fp, "lease_file /tmp/udhcpd.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));
	if (nvram_invmatch("wan_wins", "") && nvram_invmatch("wan_wins", "0.0.0.0"))
		fprintf(fp, "option wins %s\n", nvram_get("wan_wins"));

	//junzhao 2004.3.31
	//dns_list = get_dns_list(which_conn);
/*
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
   {
	int whichconn = atoi(nvram_safe_get("wan_connection"));
	char readbuf[60], *next, word[5];
	char encap[15];
	memset(encap, 0, sizeof(encap));
	check_cur_encap(whichconn, encap);
	printf("encap %s-------------\n", encap);
	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	        	goto comeon;
		}
	#else
		goto comeon;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
		{
			printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	        	goto comeon;
		}
	#else
		goto comeon;
	#endif
	}

	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
			break;
	}
	printf("word %s----------\n", word);
	if(strcmp(word, "Down"))
	{
		fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
                fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	}
	else
#endif
comeon:	{
		dns_list = get_dns_list(atoi(nvram_safe_get("wan_connection")));
		if(!dns_list || dns_list->num_servers == 0){
		//fprintf(fp, "option lease %s\n", "300");	// no dns, lease time default to 300 seconds
			fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
                	fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
		}
		else{
			fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
			fprintf(fp, "option dns %s %s %s\n", dns_list->dns_server[0], dns_list->dns_server[1], dns_list->dns_server[2]);
		}
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
   }
#endif
*/
#ifdef IPCP_NETMASK_SUPPORT
	//junzhao 2005.1.24 //for unip 
	if(nvram_match("ipcp_netmask_enable","1") && (!check_ppp_link(0) || !unip_started))
		fprintf(fp, "option lease %d\n", 60);
	else	
#endif
		fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
			
	dns_list = get_dns_list_total();
	if(!dns_list || dns_list->num_servers == 0){
		//fprintf(fp, "option lease %s\n", "300");	// no dns, lease time default to 300 seconds
		//fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
                fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	}
	else{
		int i;
		//fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
		fprintf(fp, "option dns");
		for(i=0; i<dns_list->num_servers; i++)
			fprintf(fp, " %s", dns_list->dns_server[i]);
                fprintf(fp, " %s\n", nvram_safe_get("lan_ipaddr"));
	}

	//junzhao 2004.10.11
	if(dns_list)
		free(dns_list);

/*	
	fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
        fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
*/
	if (nvram_invmatch("wan_domain", ""))
		fprintf(fp, "option domain %s\n", nvram_get("wan_domain"));
	else
		fprintf(fp, "option domain domain_not_set.invalid\n");

	{
		char word[256] = "";
		char *next;
		char *enable = NULL, *ip = NULL, *name = NULL;
//		int select_index = 0;
		int tmp_flag1 = 0, tmp_flag2 = 0, tmp_flag3 = 0;

//		if (atoi(nvram_safe_get("share_wanip")))
		tmp_flag1 = atoi(nvram_safe_get("share_wanip"));
		tmp_flag2 = atoi(nvram_safe_get("single_pc_mode"));
		tmp_flag3 = atoi(nvram_safe_get("share_wanip_comm"));
		if (tmp_flag1)
		{
			next = nvram_safe_get("share_wanip_content");
			if ((strcmp(next, "share_wanip_content")) && (next))
			{
				char *tmp_ch;
				tmp_ch = strchr(next, '$');
				if (tmp_ch)
				{
					strncpy(word, next, (int)(tmp_ch - next));
//					tmp_ch ++;
//					select_index = atoi(tmp_ch);
				}
			}
			if (tmp_wan_num)
			{
				char tmp_aa[32];
				FILE *tmp_fp;
				t_addr.s_addr = htonl(tmp_wan_num);
				strcpy(tmp_aa, inet_ntoa(t_addr));
				if (tmp_flag3)
				{
					eval("route", "add", tmp_aa, "dev", "br0");
				}
				//cprintf("leijun--------%s \n", tmp_aa);

				if (!(tmp_fp = fopen("/proc/Cybertan/half_bridge_enable", "w"))) 
				{
					cprintf("error to open /proc/Cybertan/half_bridge_enable");
				}else {
					fprintf(tmp_fp, "%d", 1);
					fclose(tmp_fp);
				}
			}
			
			if ((strlen(word) == 17) && (tmp_flag2 == 0))
			{
				fprintf(fp, "share_wanip %s\n", nvram_safe_get("share_wanip_content"));
				fprintf(fp, "share_wanip_comm %s\n", nvram_safe_get("share_wanip_comm"));
			}else fprintf(fp, "share_wanip 0$0$0\n");
		}
		fprintf(fp, "single_pc_mode %d\n", atoi(nvram_safe_get("single_pc_mode")));

		sprintf(tmpbuf,"%d.%d.%d.", get_single_ip(nvram_safe_get("lan_ipaddr"),0),
			                get_single_ip(nvram_safe_get("lan_ipaddr"),1),
			                get_single_ip(nvram_safe_get("lan_ipaddr"),2));

		i = 0;
		foreach(word, nvram_get("dhcp_reserved_ip"), next)
		{
			i ++;
			ip = word;
			enable = strsep(&ip, ":");

			if ((enable) && (ip))
			{
				name = ip;
				ip = strsep(&name, ":");
			}
			if (!enable || !ip || !name) continue;
//			printf("name = %s; ip = %s\n", name, ip);
			if (!atoi(enable)) continue;

	        if (i < 11)
    	   	{
	            fprintf(fp,"macip %s$%s%s\n", name, tmpbuf, ip);
    	   	}
        	else
       		{
            	fprintf(fp,"hostip %s$%s%s\n", name, tmpbuf, ip);
      		}
		}
	}
#if 0
	//guohong
	strcpy(databuf,nvram_get("dhcp_reserved_ip"));
 //strcpy(databuf,"0:: 1:111:00:0a:eb:24:0a:b5 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 1:112:guo 0:: 0:: 0:: 0::");		
        for (i = 1; i<=20; i++)
		{
                        get_string(databuf,  buf, i,20);
                           //printf("buf =%s\n",buf);
                        //get flag
                        p1 = buf;
                        p = strstr(buf, ":");
                        *p = 0;
                        strcpy(flag,p1);
                       

                        if (!strcmp(flag,"0")   )    continue;
                        
                         p ++;
                        p1 = p;
                        
                       //get ip
                         p = strstr(p,":");
                         *p = 0;
                           strcpy(ip,p1);
                           //ip[p-p1] = 0;
                           sprintf(tmpbuf,"%d.%d.%d.%s",get_single_ip(nvram_safe_get("lan_ipaddr"),0),
					                get_single_ip(nvram_safe_get("lan_ipaddr"),1),
					                get_single_ip(nvram_safe_get("lan_ipaddr"),2),
					                  ip);
                           p++;
                           if (i<11)
                           	{
                                    strcpy(mac,p);
                                   
                                    fprintf(fp,"macip %s$%s\n", mac,tmpbuf  );

                                    
                           	}
                           else
                           	{ 

                           	        strcpy(hostname,p);
                                   // fprintf(fp,"hostip %s$%s\n",hostname,ip);
                                    fprintf(fp,"hostip %s$%s\n", hostname, tmpbuf);
                                     
                           	}
                                               
                        
           
		}
#endif
	fclose(fp);
	eval("udhcpd", "/tmp/udhcpd.conf");

	dprintf("done\n");
	return 0;
}

int
stop_dhcpd(void)
{
	char sigusr1[] = "-XX";
	int ret;

/*
* Process udhcpd handles two signals - SIGTERM and SIGUSR1
*
*  - SIGUSR1 saves all leases in /tmp/udhcpd.leases
*  - SIGTERM causes the process to be killed
*
* The SIGUSR1+SIGTERM behavior is what we like so that all current client
* leases will be honorred when the dhcpd restarts and all clients can extend
* their leases and continue their current IP addresses. Otherwise clients
* would get NAK'd when they try to extend/rebind their leases and they 
* would have to release current IP and to request a new one which causes 
* a no-IP gap in between.
*/
	sprintf(sigusr1, "-%d", SIGUSR1);
	eval("killall", sigusr1, "udhcpd");
	ret = eval("killall", "-9", "udhcpd");//xiaoqin add "-9" 2005.05.09
	
	//junzhao 2004.4.22 dhcprelay
	ret = eval("killall", "dhcrelay");

	dprintf("done\n");
	return ret;
}

int
start_dns(void)
{
#ifdef DNSPROXY_BUTTON_SUPPORT
	/* zhangbin 2005.3.25*/
	dprintf("%s!!\n",__FUNCTION__)
	if(nvram_match("dns_proxy","1"))
	{
#endif

	int ret;
	
	ret = eval("dnsmasq",
		   "-h",
		   "-i", nvram_safe_get("lan_ifname"),
		   "-r", "/tmp/resolv.conf");

	dprintf("done\n");
	return ret;

#ifdef DNSPROXY_BUTTON_SUPPORT
	/* zhangbin 2005.3.25*/
	}
	else
		return 0;
#endif

}	

int
stop_dns(void)
{
	int ret = eval("killall", "dnsmasq");

	dprintf("done\n");
	return ret;
}	

#if defined(CONFIG_CWMP)
int
start_cwmp(void)
{
	int ret;
	
	if (nvram_match("cwmp_enable", "0"))
		return 0;

	ret = eval("cwmpCPE");
	dprintf("done\n");
	return ret;
}	

int
stop_cwmp(void)
{
	int ret = eval("killall", "cwmpCPE");

	dprintf("done\n");
	return ret;
}
#endif

int
start_httpd(void)
{
	int ret;

	chdir("/www");
#ifdef MULTILANG_SUPPORT
    {
	if(chdir("/tmp/new_www") == 0)
		cprintf("[HTTPD Starting on /tmp/new_www]\n");
	else
		cprintf("[HTTPD Starting on /www]\n");
    }
#endif
	ret = eval("httpd");
	chdir("/");
#ifdef HTTPS_SUPPORT	
	chdir("/www");
	ret = eval("httpd", "-S");
	chdir("/");
#endif
	dprintf("done\n");
	return ret;
}

int
stop_httpd(void)
{
	int ret = eval("killall", "httpd");

	dprintf("done\n");
	return ret;
}
/* start_nat() has serious problem! -- zhangbin 2005.3.8 */
//added by leijun 2004-0314_10:00
int start_nat(char *command)
{
	int i = 0;
	char name[60];
	char tmp_buff[256];
	
	char *str_name, *tmp_name, *pt;
		//junzhao
		if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
        		return 0;
		}
#if 0
		str_name = name;
		foreach(tmp_name, name, next)
		{
			if(!strcasecmp(tmp_name, "Down"))
				continue;
			eval("ifconfig", tmp_name, command);
		}
#endif
		printf("name = %s\n", name);
		for(i = 0; i < MAX_CHAN; i ++)
		{
			pt = strchr(str_name, ' ');
			tmp_name = str_name;
			if(pt)
			{
				str_name = pt + 1;
				*pt = '\0';
			}
			printf("tmp_name = %s\n", tmp_name);
			if(!strcasecmp(tmp_name, "Down"))
				continue;	
//			sprintf(tmp_buff, "/usr/sbin/iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", tmp_name);
//			printf("%s\n", tmp_buff);
			sprintf(tmp_buff, "/sbin/ifconfig %s %s", tmp_name, command);
			printf("%s\n", tmp_buff);
			system(tmp_buff);
		}
//		eval("/usr/sbin/iptables", "-t", "nat", "-F");
	return 1;
}

#if 0
int start_upnp(void)
{
	char wan_ifname[10];
	int i = 0, upnp_content_number = 0;
	char buf[256];
	char *pt1, *pt2;
	char pt[60];
	FILE *fp;

//	printf("leijun -- upnpd-igd ver 0.81 2004-0313_1400\n");
	if (nvram_match("nat_enable", "0"))
	{
		return 0;
	}

	if (nvram_match("upnp_enable", "0"))
	{
		return 0;
	}

	if(!file_to_buf(WAN_IFNAME, pt, sizeof(pt)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return 0;
	}

	i = 0;
	while(*(pt + i) != ' ')
	{
		wan_ifname[i] = *(pt + i);
		i ++;
	}
	wan_ifname[i] = '\0';
	if (!strcmp(wan_ifname, "Down")) strcpy(wan_ifname, "nas0");
	
	if (!(fp = fopen("/tmp/portmap.conf", "w"))) {
		printf("can not create /tmp/portmap.conf\n");
		return errno;
	}
	fprintf(fp, "%s\n", nvram_safe_get("upnp_content_num"));
	upnp_content_number = atoi(nvram_safe_get("upnp_content_num"));
	if (upnp_content_number > 0)
	{
		for (i = 0; i < upnp_content_number; i ++)
		{
			sprintf(buf, "upnp_content%d", i);
			fprintf(fp, "%s\n", nvram_safe_get(buf));
		}
	}
	fclose(fp);
	
	sprintf(buf,"/usr/sbin/upnpd-igd %s %s /tmp/portmap.conf &", wan_ifname, nvram_safe_get("lan_ifname"));
	system(buf);
	eval("killall", "leijun--upnpd-igd ver 0.82 2004-0313");
	return 1;
}
//#endif
//modifed by leijun
int
stop_upnp(void)
{
	int ret = 0;

	//if (nvram_match("upnp_enable", "0"))
	ret = eval("killall", "upnpd-igd");
	sleep(2);
	dprintf("done\n");
	return ret;
}
//////////end
#endif
int start_upnp(void)
{
	char *pt, *pt1;
	int index = 0, i;
//	printf("leijun -- upnpd-igd ver 0.81 2004-0317_1000\n");
	if (nvram_match("nat_enable", "0"))
		return 0;
	if (nvram_match("upnp_enable", "0"))
		return 0;
	index = atoi(nvram_safe_get("wan_active_connection"));
	if ((index < 0) || (index > MAX_CHAN)) index = MAX_CHAN;
	if (index < MAX_CHAN)
	{
		pt = nvram_safe_get("vcc_config");
		if (pt)
		{
			for (i = 0; i < index; i ++)
			{
				pt1 = strchr(pt, ' ');
				if (pt1)
				{
					*pt1 = '\0';
					pt = pt1 + 1;
				}else break;
			}
			pt1 = strstr(pt, "bridgedonly");
			if (pt1) return 0;
			
		}
	}
//	sprintf(buf,"/usr/sbin/upnpd-igd %s &", nvram_safe_get("lan_ifname"));
	//system(buf);
   {
	char *argv[] = {"upnpd-igd", nvram_safe_get("lan_ifname"), NULL};
	pid_t pid;
	_eval(argv, NULL, 0, &pid);
   }
	eval("killall", "leijun--upnpd-igd ver 0.83 2004-0329");
	return 1;
}
void  stop_upnp(int flag)
{
	int pid = 0;
//	pid = atoi(nvram_safe_get("upnpd_pid_num"));
	
	switch(flag)
	{
	case 0:  eval("killall", "upnpd-igd"); sleep(2); break;//killed
	case 1:	 nvram_set("kill_signal_flag", "1"); break;
	case 2:	 nvram_set("kill_signal_flag", "2"); break;
//	case 1:	 eval("killall", "-SIGUSR1", "upnpd-igd");break;//wan down
//	case 2:	 eval("killall", "-SIGUSR2", "upnpd-igd");break;//wan up
	defaule: break;
	}
	dprintf("done\n");
}
//guohong  leijun
int start_tr064(void)
{
   
	if (nvram_match("tr064_enable", "0"))
		return 0;
//	eval("cp", "-a", "/etc/cpe", "/tmp/");
//	sleep(1);
//	eval("cped", "&");
#if 0
	if (find_pid_by_name("cped") != NULL)
	{
		printf("have cped exist.\n");
		return 0;
	}
#endif
	{
		pid_t pid;
		char *argv[] = {"cped", NULL};
		_eval(argv, NULL, 0, &pid);
	}
	//zhangbin 2005.3.8
	return 0;
}
#ifdef WIRELESS_SUPPORT
int start_management_via_wlan(void)
{
#ifdef CLOUD_SUPPORT
	if(nvram_match("cloud_enable", "1") && nvram_match("cloud_dhcp_enable", "1"))
	{
		stop_firewall();
		start_firewall();
	}
	else
	{
		FILE *file;
		char buff[25];
		char enable[12];
		char lanip[20];

		file = fopen("/proc/Cybertan/wl_management", "w");
		if (file == NULL)
			return 0;
		strcpy(lanip, nvram_get("lan_ipaddr"));
		strcpy(enable, nvram_get("wl_management_enable"));
		sprintf(buff, "%s:%s", lanip, enable);
		fputs(buff, file);
		fclose(file);
		return 1;
	}
#else
	FILE *file;
	char buff[25];
	char enable[12];
	char lanip[20];

	file = fopen("/proc/Cybertan/wl_management", "w");
	if (file == NULL)
		return 0;
	strcpy(lanip, nvram_get("lan_ipaddr"));
	strcpy(enable, nvram_get("wl_management_enable"));
	sprintf(buff, "%s:%s", lanip, enable);
	fputs(buff, file);
	fclose(file);
	return 1;
#endif	
}
#endif
int stop_tr064(void)
{
	if (nvram_match("tr064_enable", "1"))
		return 0;

   eval("killall","cped");
   return 0;
}

int stop_tr64_1(void)
{
   eval("killall","cped");
   return 0;
}

// add for IGMP Proxy 2004-10-10
#ifdef IGMP_PROXY_SUPPORT
int start_igmp_proxy(void)
{
	char enable[3], channel[60];
	pid_t pid;
	int ret = 0;
	char readbuf[256], word[256], *next;

	memset(enable, 0, sizeof(enable));
	memset(channel, 0, sizeof(channel));

	sprintf(enable, "%s", nvram_safe_get("igmp_proxy_enable"));
	if(!strcmp(enable, "0"))
	{
		printf("IGMP Proxy Disabled Now !!!\n");
		return 0;
	}

	memset(readbuf, 0, sizeof(readbuf));
	memset(word, 0, sizeof(word));
	if(!file_to_buf("/tmp/status/wan_ifname", readbuf, sizeof(readbuf)))
	{
		printf("No Buf in /tmp/status/wan_ifname !!!\n");
		return 0;
	}
	foreach(word, readbuf, next)
	{
		if(strstr(word, nvram_safe_get("igmp_proxy_channel")))
		{
			sprintf(channel, "%s", word);
			break;
		}
	}

	if(channel)
	{
		char *argv[] = {"IGMPProxy", "-n", channel, NULL};
		printf("Now Start IGMP Proxy !!!\n");
		_eval(argv, NULL, 0, &pid);
		stop_firewall();
		start_firewall();
	}
	
	return ret;
}
int stop_igmp_proxy(void)
{
	printf("Now Stop IGMP Proxy !!!\n");
	eval("killall", "IGMPProxy");
	return 0;
}
#endif

int
start_nas(char *type)
{
	char cfgfile[64];
	char pidfile[64];
	if (!type || !*type)
		type = "lan";
	snprintf(cfgfile, sizeof(cfgfile), "/tmp/nas.%s.conf", type);
	snprintf(pidfile, sizeof(pidfile), "/tmp/nas.%s.pid", type);
	{
		char *argv[] = {"nas", cfgfile, pidfile, type, NULL};
		pid_t pid;

		_eval(argv, NULL, 0, &pid);
		dprintf("done\n");
	}
	return 0;
}

int
stop_nas(void)
{
	int ret = eval("killall", "nas");

	dprintf("done\n");
	return ret;
}

int
start_ntpc(void)
{
#ifdef NTP_SUPPORT
	char *servers = nvram_safe_get("ntp_server");
	
	if (strlen(servers)) {
		char *nas_argv[] = {"ntpclient", "-h", servers, "-i", "3", "-l", "-s", NULL};
		pid_t pid;

		_eval(nas_argv, NULL, 0, &pid);
	}
	
	dprintf("done\n");
#endif
	return 0;
}

int
stop_ntpc(void)
{
#ifdef NTP_SUPPORT
	int ret = eval("killall", "ntpclient");

	dprintf("done\n");
	return ret;
#else
	return 0;
#endif
}

int
start_services(void)
{
#ifdef CONFIG_ANTSEL
	start_antsel();
#endif
	//start_SysLog();
	start_cron();

#ifdef SECLAN_SUPPORT	
	/* kirby 2004/12.22 */
	start_second_lan();
#endif	
	
	start_tftpd();
	start_httpd();
	start_dns();
	//modify by lzh;
	//start_dhcpd();
	start_dhcpcc();

#ifdef CLI_SUPPORT		
	/* kirby 2004/12.22 */
	start_telnetd();
#endif	

	start_snmp();//by songtao
	//start_wt82();//guohong
#ifdef CPED_TR064_SUPPORT
	start_tr064();//guohong
#endif
	start_SysLog();
	start_upnp();
	start_zebra();
	enable_disable_pppoe_pass();

	/* zhangbin 2005.5.12 */
#ifdef CLOUD_SUPPORT
	if(nvram_match("cloud_enable", "1"))
	{
		start_cloud();
	}
#endif

	dprintf("done\n");
	return 0;
}

int
stop_services(void)
{

#ifdef CLI_SUPPORT		
	/* kirby 2004/12.22 */ 
	stop_telnetd(); 
#endif	
#ifdef CONFIG_ANTSEL
	stop_antsel();
#endif

	stop_zebra();
        //stop_upnp();
	//junzhao 2004.3.18
        stop_upnp(0);
#ifdef CPED_TR064_SUPPORT
	stop_tr064();
#endif
        stop_snmp();//by songtao
	//modify by lzh;
	//stop_dhcpd();
	stop_dhcpcc();
	
#ifdef SECLAN_SUPPORT		
	/* kirby 2004/12.22 */
	stop_second_lan();
#endif	
	
	stop_dns();
	//stop_httpd();	// Don't kill httpd when user press Apply.
	stop_cron();
	stop_SysLog();

	/* zhangbin 2005.5.12 */
#ifdef CLOUD_SUPPORT
	stop_cloud();
#endif
	
	dprintf("done\n");
	return 0;
}

/////////////////////////////////////////////////////
int
start_resetbutton(void)
{
        int ret = 0;

        ret = eval("resetbutton");

        dprintf("done\n");
        return ret;
}

int 
stop_resetbutton(void)
{
        int ret = 0;

        ret = eval("killall","-9","resetbutton");

        dprintf("done\n");
        return ret ;
}

#ifdef CONFIG_ANTSEL
int
start_antsel(void)
{
        int ret = 0;

        ret = eval("antsel");

        dprintf("done\n");
        return ret;
}

int 
stop_antsel(void)
{
        int ret = 0;

        ret = eval("killall","-9","antsel");

        dprintf("done\n");
        return ret ;
}
#endif

int
start_iptqueue(void)
{
        int ret = 0;

        ret = eval("iptqueue");

        dprintf("done\n");
        return ret;
}

int 
stop_iptqueue(void)
{
        int ret = 0;

        ret = eval("killall","-9","iptqueue");

        dprintf("done\n");
        return ret ;
}

int
start_tftpd(void)
{
	int ret = 0;
	pid_t pid;
	char *tftpd_argv[] = { "tftpd",
		//zhangbin 2004.12.22 for remote upgrading
				"-a",nvram_safe_get("lan_ipaddr"),
				"-s","/tmp",	// chroot to /tmp
				"-c",		// allow new files to be created 
				"-l",		// standalone
			      NULL
	};

	ret = _eval(tftpd_argv, NULL, 0, &pid); 

	dprintf("done\n");
	return ret;
}

int 
stop_tftpd(void)
{
	int ret;

        ret = eval("killall","-9","tftpd");

	dprintf("done\n");
	return ret ;
}

#ifdef CLI_SUPPORT	
/* added by wavestorm  kirby 2004/12.22*/
int start_telnetd(void){
	int ret = 0;
	if(nvram_match("telnet_enable","1"))
	{
		pid_t pid;
        	char *telnetd_argv[] = { "telnetd",
                              NULL
	        };

		mk_etc_passwd();
	        ret = _eval(telnetd_argv, NULL, 0, &pid);

        	dprintf("done\n");
	}
        return ret;
}
/* added by wavestorm kirby 2004/12.22*/
int stop_telnetd(void){
	int ret;
	
	ret = eval("killall", "-9", "telnetd");

	dprintf("done\n");

	return ret;
}
#endif


int
start_cron(void)
{
	int ret = 0;
	struct stat buf;

	/* Create cron's database directory */
	if( stat("/var/spool", &buf) != 0 ){
		mkdir("/var/spool", 0700);
		mkdir("/var/spool/cron", 0700);
	}
	mkdir("/tmp/cron.d", 0700);
	
	buf_to_file("/tmp/cron.d/monitor_ps", "*/2 * * * * root /sbin/monitor_ps\n");
	ret = eval("cron"); 
	
		
	dprintf("done\n");
	return ret;
}

int 
stop_cron(void)
{
	int ret = 0;

        ret = eval("killall","-9","cron");

	dprintf("done\n");
	return ret ;
}
int get_cur_active_if(char *dst_buf, char *if_name_buf)
{
	char tmp_buf[80], tmp_name[8][8];
	char *t_pt, *t_pt_head;
	int t_index = 0, i;
	
	if(!file_to_buf(if_name_buf, tmp_buf, sizeof(tmp_buf)))
	{
		printf("no buf in %s!\n", if_name_buf);
		return 0;
	}
	sscanf(tmp_buf, "%s %s %s %s %s %s %s %s", tmp_name[0], tmp_name[1], tmp_name[2], tmp_name[3], tmp_name[4], tmp_name[5], tmp_name[6], tmp_name[7]);
	for (i = 0; i < MAX_CHAN; i ++)
	{
		if (strcasecmp(tmp_name[i], "Down"))
		{
			strcpy(dst_buf + t_index, tmp_name[i]);
			t_index += strlen(tmp_name[i]);
			dst_buf[t_index ++] = ' ';
		}
	}
	return t_index;
}

int get_all_active_if(char *dst_buf)
{
	char *cur_pt = dst_buf;
	int result = 0;
	result = get_cur_active_if(cur_pt, WAN_IFNAME);
#ifdef MPPPOE_SUPPORT
	cur_pt += result;
	result = get_cur_active_if(cur_pt, WAN_MPPPOE_IFNAME);
	#if defined(IPPPOE_SUPPORT)
	cur_pt += result;
	result = get_cur_active_if(cur_pt, WAN_IPPPOE_IFNAME);
	#endif
#endif
	cur_pt += result;
	cur_pt = '\0';
	return 1;
}
//written by zls 2005-0112
int get_active_ifname(char *dst_buf,int dst_len,char *index,int index_len)
{
        int i, selectable, ret = 0, which_to_select;
        char word[PVC_CONNECTIONS][6], readbuf[60];
        int t_index = 0,i_index=0,dst_count=0,index_count=0;
        char tmp_index[6];
        memset(word, 0, sizeof(word));
        memset(dst_buf,0,dst_len);
        memset(index,0,index_len);
        if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
        {
                printf("no buf in %s!\n", WAN_IFNAME);
                return 0;
        }
       sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
        for(i=0 ; i<PVC_CONNECTIONS; i++)
        {
                selectable = 0;
                which_to_select = 0;
                if(strcmp(word[i], "Down"))
                {
                        if(strstr(word[i], "ppp"))
                        {
                                //if(check_ppp_link(i))
 if(check_ppp_proc(i))
                                {
                                        selectable = 1;
                                        which_to_select = (1<<PPP_MASK) | i;   //ppp flag
                                }
                        }
                        else if(strstr(word[i], "nas"))
                        {
                                selectable = 1;
                                which_to_select = i;
                        }
                #ifdef CLIP_SUPPORT
                        else if(strstr(word[i], "atm"))
                        {
                                selectable = 1;
                                which_to_select = i;
                        }
                #endif
                }
                if(selectable)
                {
                        strncpy(dst_buf + t_index,word[i],strlen(word[i]));
                        sprintf(tmp_index,"%d",i);
                        strncpy(index + i_index,tmp_index,strlen(tmp_index));
                        t_index +=strlen(word[i]);
                        i_index +=strlen(tmp_index);
                        dst_buf[t_index ++] = ' ';
                        index[i_index ++] = ' ';
                        dst_count += strlen(word[i]) + 1;
 index_count += strlen(tmp_index) + 1;
                 }
        }//end for(i=0;i<PVC_CONNECTIONS)
#if defined(MPPPOE_SUPPORT)
   {
        memset(word, 0, sizeof(word));
        if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
        {
                printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
                return 0;
        }
       sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
        for(i=0 ; i<PVC_CONNECTIONS; i++)
        {
                selectable = 0;
                which_to_select = 0;
                if(strcmp(word[i], "Down"))
                {
                        if(strstr(word[i], "ppp"))
                        {
                                if(check_ppp_proc(i + 8))
                                {
                                        selectable = 1;
  which_to_select = (1<<PPP_MASK) | (1<<MUL_MASK) | i;   //ppp flag
                                }
                        }
                }
                #if defined(IPPPOE_SUPPORT)
   else
                {
                        char *next;
                        int which = i;
                        if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
                        {
                                printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
                                return 0;
                        }
                        memset(word[i], 0, sizeof(word[i]));
                        foreach(word[i], readbuf, next)
                        {
                                if(which -- == 0)
                                        break;
                                memset(word[i], 0, sizeof(word[i]));
                        }
                        if(strstr(word[i], "ppp"))
                        {
                                if(check_ppp_proc(i + 8))
                                {
                                        selectable = 1;
                                        which_to_select = (1<<MUL_MASK) | i;   //ppp flag
           which_to_select = (1<<MUL_MASK) | i;   //ppp flag
                                }
                        }
                }
                #endif
                if(selectable)
  {
                        strncpy(dst_buf + t_index,word[i],strlen(word[i]));
                        sprintf(tmp_index,"%d",i);
                        strncpy(index+ i_index,tmp_index,strlen(tmp_index));
                        t_index +=strlen(word[i]);
                        i_index +=strlen(tmp_index);
                        dst_buf[t_index ++] = ' ';
                        index[i_index ++] = ' ';
                        dst_count += strlen(word[i]) + 1;
                        index_count += strlen(tmp_index) + 1;
                }
         }//end  for(i=0;)
   }
#endif
        *(dst_buf + dst_count) = '\0';
        *(index + index_count) = '\0';
        return ret;
}
//modified by zls 2005-0110
int start_zebra(void){
        FILE *fp;
        char interface[16];
        int i=0,ret1,ret2;
        char rip_interface_content[32],rip_interface_var[32];
        int  index=9,i_send=0,i_receive=0,i_MulticastBroadcast=0;
        int ifname_count=0,index_count=0;
        char send_droute[4],lan_ifname[10],wan_ifnames[128],wan_indexs[64],wan_ifname[8][16],wan_index[8][2];
        char *tmp_name_pt,*tmp_name_head,*tmp_index_head,*tmp_index_pt;
        //added by zls 2005-0110
                                                                                                               
        if (!nvram_invmatch("rip_enable","0"))
        {
                printf("zebra disabled\n");
                return 0;
        }
        if (!(fp = fopen("/tmp/zebra.conf","w"))){
                perror("/tmp/zebra.conf");
                return errno;
        }
        fclose(fp);
        if (!(fp = fopen("/tmp/ripd.conf","w"))){
                perror("/tmp/ripd.conf");
                return errno;
        }
         strcpy(lan_ifname,nvram_safe_get("lan_ifname"));
        #ifdef RIP_EXTEND_SUPPORT
         strcpy(send_droute,nvram_safe_get("send_droute"));
	#endif
        //write configure file
        fprintf(fp, "router rip\n");
        fprintf(fp, "  network %s\n", lan_ifname);

        #ifdef RIP_EXTEND_SUPPORT
        fprintf(fp, "  send_droute %s\n", send_droute);
        #endif                                                                                                      
        fprintf(fp, "redistribute connected\n");
        get_active_ifname(wan_ifnames,sizeof(wan_ifnames),wan_indexs,sizeof(wan_indexs));
                                                                                                               
        //cprintf("wanifnames=%s,wanindexs=%s\n",wan_ifnames,wan_indexs);
        //strcpy(wan_ifnames,"ppp ppp1 ppp2 ppp4 ");
        //strcpy(wan_indexs,"0 1 2 4 ");
        tmp_name_head = &wan_ifnames[0];
         while((tmp_name_pt = strchr(tmp_name_head,' ')) != NULL)
         {      *tmp_name_pt = '\0';
                if(!(strcmp(tmp_name_head," "))) break;
                fprintf(fp,"  interface %s\n",tmp_name_head);
                strcpy(wan_ifname[i],tmp_name_head);
                *tmp_name_pt =' ';
                 tmp_name_head = tmp_name_pt +1;
                i++;
                ifname_count++;
         }
        tmp_index_head = &wan_indexs[0];
                                                                                                               
        i = 0;
 while((tmp_index_pt = strchr(tmp_index_head,' ')) != NULL){
                        *tmp_index_pt = '\0';
                        if(!(strcmp(tmp_index_head," "))) break;
                        strcpy(wan_index[i], tmp_index_head);
                        *tmp_index_pt = ' ';
                        tmp_index_head = tmp_index_pt + 1;
                        i++;
                        index_count++;
        }
                                                                                                               
        /*for(i=0;i<ifname_count;i++)
        {
                printf("ifname is %s\n",wan_ifname[i]);
                printf("index is %s\n",wan_index[i]);
        }*/
                                                                                                               
                                                                                                               
        for(i=0;i< RIP_INTERFACE_SUM;i++)
        {
                if ((i>= index_count) && (i < RIP_INTERFACE_SUM  - 1)) continue;
                else if ((i == (RIP_INTERFACE_SUM - 1)))
                        sprintf(rip_interface_var,"rip_interface_content_%d",RIP_INTERFACE_SUM  - 1);
                else{
                        if((*(wan_index[i]) == NULL) || (*(wan_index[i]) == ' ')) continue;
                        sprintf(rip_interface_var,"rip_interface_content_%d",atoi(wan_index[i]));
                }
                strcpy(rip_interface_content,nvram_safe_get(rip_interface_var));
                if (!(strcmp(rip_interface_content,rip_interface_var)))
                        continue;
 if (strlen(rip_interface_content)>4){
                        sscanf(rip_interface_content,"%d:%d:%d",&i_send,&i_receive,&i_MulticastBroadcast);
                          //set interface
                        if (i == RIP_INTERFACE_SUM - 1)
                                sprintf(interface,"interface %s\n",lan_ifname);
                        else{
                                printf("wan_ifname[%d]=%s\n",i,wan_ifname[i]);
                                sprintf(interface,"interface %s\n",wan_ifname[i]);
                        }
                        fprintf(fp,interface);
                        switch(i_send){
                                case 0:fprintf(fp,"\n");break;
                                case 1:fprintf(fp,"  ip rip send version 1\n");break;
                                case 2:fprintf(fp,"  ip rip send version 1 2\n");break;
                                case 3:fprintf(fp,"  ip rip send version 2\n");break;
                        }
                        switch(i_receive){
                                case 0:fprintf(fp,"\n");break;
                                case 1:fprintf(fp,"  ip rip receive version 1\n");break;
                                case 2:fprintf(fp,"  ip rip receive version 2\n");break;
                        }
                                                                                                               
                       #ifdef RIP_EXTEND_SUPPORT
                        switch(i_MulticastBroadcast){
                                case 0:fprintf(fp,"  broadcast 0\n");break;
                                case 1:fprintf(fp,"  broadcast 1\n");break;
                        }
                       #endif
                }//end if(strlen()
 }//end for(i=0
        fprintf(fp,"access-list private deny any\n");
                                                                                                               
        fflush(fp);
        fclose(fp);
                                                                                                               
        ret1 = eval("zebra", "-d", "-f", "/tmp/zebra.conf");
        ret2 = eval("ripd",  "-d", "-f", "/tmp/ripd.conf");
        dprintf("zebra done\n");
        return ret1 | ret2 ;
}
//add by lzh
#define LOG_PATH "/var/log/messages"
static int RenewLogMessFromFlash(void)
{
        char *tmpbuf,*tmp;
        int len;
                                                                                
        printf("Renew Log Message from flash by lzh\n");
                                                                                
        tmp = nvram_safe_get("log_save");
        len = strlen(tmp);
        printf("1:log len=%d,save=%s\n",len,tmp);
        if (!len) return -1;
                                                                                
        tmpbuf = malloc(len);
        memset(tmpbuf,0,len);
        strcpy(tmpbuf,tmp);
        printf("2:log save=%s\n",tmpbuf);
                                                                                
        buf_to_file(LOG_PATH,tmpbuf);
                                                                                
        free(tmpbuf);
        printf("renew log message over!\n");
        return 0;
}

//end of lzh add;

//written by leijun
/*int start_zebra(void)
{
	int rip_tx, rip_rx, i=0;
	FILE *fp;
	int ret1, ret2;
	char lan_ifname[10];
	char pt[60], tx_ver[5], rx_ver[5];
	char wan_if_names[128];
	char *tmp_pt, *tmp_pt_head;
	
	//modified by leijun 2004-0313:14:30 
	if (!nvram_invmatch("rip_enable", "0"))
	{
		printf("zebra disabled\n");
		return 0;
	}
	rip_tx = atoi(nvram_safe_get("sel_ripTX"));
	rip_rx = atoi(nvram_safe_get("sel_ripRX"));
	if ((rip_tx == 0) && (rip_rx == 0)) 
	{
		printf("zebra disabled1\n");
		return 0;
	}

	switch(rip_tx)
	{
	case 1:	strcpy(tx_ver, "1");break;
	case 2:	strcpy(tx_ver, "1 2");break;
	case 3:	strcpy(tx_ver, "2");break;
	}
	switch(rip_rx)
	{
	case 1:	strcpy(rx_ver, "1");break;
	case 2:	strcpy(rx_ver, "2");break;
	}

	// Write configuration file based on current information 
	if (!(fp = fopen("/tmp/zebra.conf", "w"))) {
		perror("/tmp/zebra.conf");
		return errno;
	}
	fclose(fp);
	
	if (!(fp = fopen("/tmp/ripd.conf", "w"))) {
		perror("/tmp/ripd.conf");
		return errno;
	}
	strcpy(lan_ifname, nvram_safe_get("lan_ifname"));
	fprintf(fp, "router rip\n");
	fprintf(fp, "  network %s\n", lan_ifname);
	
	if (rip_rx == 2)
	{
		get_all_active_if(wan_if_names);
		tmp_pt_head = &wan_if_names[0];
		while((tmp_pt = strchr(tmp_pt_head, ' ')) != NULL)
		{
			*tmp_pt = '\0';
			fprintf(fp, "  network %s\n", tmp_pt_head);
			*tmp_pt = ' ';
			tmp_pt_head = tmp_pt + 1;
		}
	}
	
	fprintf(fp, "redistribute connected\n");

	fprintf(fp, "interface %s\n", lan_ifname);
	if (rip_tx != 0)
	{
		fprintf(fp, "  ip rip send version %s\n", tx_ver);
	}
	if (rip_rx != 0)
		fprintf(fp, "  ip rip receive version %s\n", rx_ver);
		
	if (rip_rx == 2)
	{
		//ripd 2 receive wan ripd information.
		tmp_pt_head = &wan_if_names[0];
		while((tmp_pt = strchr(tmp_pt_head, ' ')) != NULL)
		{
			*tmp_pt = '\0';
			fprintf(fp, "interface %s\n", tmp_pt_head);
			fprintf(fp, "  no ip rip send version\n");
			fprintf(fp, "  ip rip receive version 2\n");
			tmp_pt_head = tmp_pt + 1;
		}
	}

	if (rip_tx == 0)
	{
		fprintf(fp, "  distribute-list private out %s\n",lan_ifname);
//		fprintf(fp, "  distribute-list private out %s\n",wan_ifname);
	}
	if (rip_rx == 0)
	{
		fprintf(fp, "  distribute-list private in  %s\n",lan_ifname);
//		fprintf(fp, "  distribute-list private in  %s\n",wan_ifname);
	}
	fprintf(fp, "access-list private deny any\n");

	fflush(fp);
	fclose(fp);
	
	ret1 = eval("zebra", "-d", "-f", "/tmp/zebra.conf");
	ret2 = eval("ripd",  "-d", "-f", "/tmp/ripd.conf");
	dprintf("zebra done\n");

	return ret1 | ret2 ;
}*/
//#define LOG_PATH "/var/log/messages"
int
start_SysLog(void)
{
	volatile static int i = 0;
	//zhangbin 2005.3.8
	int fd;
	printf ("Now Start syslog.........................!!");
	if(nvram_match("log_enable","1"))
	{
                //add by lzh;
		//modify by Lai at 2005-06-14
	#ifdef SAVE_LOG_SUPPORT
                if (!i) 
			RenewLogMessFromFlash();
	#endif	
		chdir("/sbin");
		eval("syslogd");
		eval("klogd");
		chdir("/");
		for(;;)
		{
			if((fd = open(LOG_PATH, O_RDONLY)) < 0)
				sleep(2);/*wait for the syslogd*/
			else
			{
				/* zhangbin 2005.3.8 */
				//close(LOG_PATH);
				close(fd);
				break;
			}
		}
		if(i == 0)
		{
			i ++;
			openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
			syslog(LOG_CRIT,"1nSystem cold started!\n");
			closelog();
			openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
			syslog(LOG_CRIT,"1nFirmware Version:%s%s\n", CYBERTAN_VERSION, MINOR_VERSION); 
			closelog();
		}
		else
		{
			openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
			syslog(LOG_CRIT,"1nSystem warm started!\n");
			closelog();

		}
	}

	return 0;

}



int
stop_SysLog(void)
{	
	//if(nvram_match("log_enable","1")){
	eval("killall","klogd");
	eval("killall","syslogd");
	//}
	//add by lzh for log_save_to flash
	//add by lai at 2005-06-14
#ifdef SAVE_LOG_SUPPORT
        eval("killall", "syslogs");
#endif
	dprintf("done\n");
	return 0 ;
}

/* Written by Sparq in 2002/07/16 */
int stop_zebra(void)
{
	int  ret1, ret2;

//	printf("Stop zebra !\n");

	ret1 = eval("killall", "zebra");
	ret2 = eval("killall", "ripd");

//	printf("Stop RET=%d, %d\n",ret1,ret2);

//	dprintf("done\n");
	return ret1 | ret2 ;
}

int
start_syslog(void)
{
        int ret = 0;
	return ret;
}

int
stop_syslog(void)
{
	int ret;

        ret = eval("killall","klogd");
        ret += eval("killall","syslogd");
        ret += eval("killall","-9","rotatelog.sh");
	ret += eval("killall","-9","sleep");

	dprintf("done\n");
	return ret ;
}

int
start_wanledctrl(int which, char *mode, char *submode)
{
	int ret;
	pid_t pid;
	char which_pvc[3];
	memset(which_pvc, 0, sizeof(which_pvc));
	sprintf(which_pvc, "%d", which);

    {
	char *wanledctrl_argv[] = { "/tmp/wanledctrl",
				which_pvc, mode, submode,
			      NULL
	};
	
	symlink("/sbin/rc", "/tmp/wanledctrl");

	ret = _eval(wanledctrl_argv, NULL, 0, &pid); 
    }
	dprintf("done\n");
	return ret;
}

int
stop_checkrate()
{
	int ret = 0;	
	FILE *fp = fopen("var/run/checkrate.pid", "r");
	if(fp != NULL)
	{
		fclose(fp);
		ret = eval("killall", "checkrate");
		unlink("/var/run/checkrate.pid");
	}
	return ret;
}	
	
int
start_checkrate(char *second)
{
	int ret = 0;
	pid_t pid;
	FILE *fp;

	char *checkrate_argv[] = { "/tmp/checkrate",
				second,
			      NULL
	};
	

	fp = fopen("/var/run/checkrate.pid", "r");
	if(fp == NULL)
	{
		symlink("/sbin/rc", "/tmp/checkrate");
		ret = _eval(checkrate_argv, NULL, 0, &pid); 
		//fp = fopen("/var/run/checkrate.pid", "w");
		//fclose(fp);
		creat("/var/run/checkrate.pid", O_CREAT|O_RDWR);
	}
	
	dprintf("done\n");
	return ret;
}

int
start_checkadsl(char *second)
{
	int ret;
	pid_t pid;
	char *checkadsl_argv[] = { "/tmp/checkadsl",
				second,
			      NULL
	};
	
	symlink("/sbin/rc", "/tmp/checkadsl");

	ret = _eval(checkadsl_argv, NULL, 0, &pid); 

	dprintf("done\n");
	return ret;
}

int
start_redial(char *second, char *devname)
{
	int ret;
	pid_t pid;
	//junzhao 2004.3.19
	char *redialfile = NULL;
	FILE *fp;
	char whichstr[8];
	char *redial_argv[] = { "/tmp/ppp/redial",
				second,
				devname,
			      NULL
	};
	int which_real = atoi(&devname[3]); 
	
	printf("enter start_redial %s  %s\n", second, devname);
	symlink("/sbin/rc", "/tmp/ppp/redial");

	ret = _eval(redial_argv, NULL, 0, &pid); 

	memset(whichstr, 0, sizeof(whichstr));
	sprintf(whichstr, "%ld", pid);
	redialfile = malloc(strlen("/var/run/redial.pid") + 3);
	if(redialfile == NULL)
		return -1;
	sprintf(redialfile, "/var/run/redial%d.pid", which_real);
	if(!buf_to_file(redialfile, whichstr))
	{
		printf("write %s file fail\n", redialfile);
		return -1;
	}
	free(redialfile);
	redialfile = NULL;
	
	dprintf("done\n");
	return ret;
}

int 
stop_wanledctrl(void)
{
	int ret;

	diag_led_ctl(PPP_OFF);
        ret = eval("killall","-9","wanledctrl");
	unlink("/tmp/wanledctrl");

	dprintf("done\n");
	return ret ;
}


int 
stop_redial(int which)
{
	int ret;
	char *redialfile = NULL;
	char redialpid[8];
	
	redialfile = malloc(strlen("/var/run/redial.pid") + 3);
	if(redialfile == NULL)
		return -1;
	sprintf(redialfile, "/var/run/redial%d.pid", which);
	if(!file_to_buf(redialfile, redialpid, sizeof(redialpid)))
		return -1;
        
	ret = eval("kill", "-9", redialpid);
	//unlink("/tmp/ppp/redial");
	unlink(redialfile);
	
	free(redialfile);
	redialfile = NULL;

	dprintf("done\n");
	return ret ;
}

int
stop_pppoe(void)
{
	int ret;

	unlink("/tmp/ppp/link");
	ret = eval("killall","-9", "pppd");
	ret += eval("killall", "ip-up");
	ret += eval("killall", "ip-down");
	dprintf("done\n");
	return ret ;
}

//junzhao 2004.11.2 for demand disconnect button!
int
stop_pppox_demand(int whichconn)
{
	char pppfile[28];
	char pidchar[32];
	int ret = -1;
	memset(pppfile, 0, sizeof(pppfile));
	sprintf(pppfile, "/var/run/ppp%d.pid", whichconn);
	if(file_to_buf(pppfile, pidchar, sizeof(pidchar)))
       		ret = eval("kill", "-SIGUSR1", pidchar);
	return ret;
}

//junzhao 2004.11.2 add for send padt!!
int
stop_pppox(int whichconn)
{
	char pppfile[28];
	char pidchar[32];
	int ret = -1;
	memset(pppfile, 0, sizeof(pppfile));
	sprintf(pppfile, "/var/run/ppp%d.pid", whichconn);
	if(file_to_buf(pppfile, pidchar, sizeof(pidchar)))
	{
		//junzhao 2004.11.2 send PADT
		char buf[32];
		memset(buf, 0, sizeof(buf));
		printf("ppp_pid[%d], %s\n", whichconn, pidchar);
		sprintf(buf, "kill -15 %s", pidchar);
       		system(buf);
       		//ret = eval("kill", "-15", pidchar);
		//junzhao 2004.6.15
       		ret = eval("kill", "-9", pidchar);
	}
	if(whichconn < MAX_CHAN)
		ppp_pid[whichconn] = -1;
	unlink(pppfile);
	return ret;
}

#if 0
int
stop_pppox(int whichconn)
{
	char pppfile[28];
	char pidchar[32];
	int ret = -1;
	memset(pppfile, 0, sizeof(pppfile));
	sprintf(pppfile, "/var/run/ppp%d.pid", whichconn);
	if(file_to_buf(pppfile, pidchar, sizeof(pidchar)))
	{
		printf("ppp_pid[%d], %s\n", whichconn, pidchar);
       		ret = eval("kill", "-15", pidchar);
		//junzhao 2004.6.15
       		ret = eval("kill", "-9", pidchar);
	}
	if(whichconn < MAX_CHAN)
		ppp_pid[whichconn] = -1;
	unlink(pppfile);
	return ret;
}
#endif

int
stop_dhcpc(int whichconn)
{
  #if 0
	//junzhao 2004.3.19
	char udhcpcfile[28];
	char pidchar[32];
	int ret = -1;
	memset(udhcpcfile, 0, sizeof(udhcpcfile));
	sprintf(udhcpcfile, "/var/run/udhcpc%d.pid", which_conn);
	if(file_to_buf(udhcpcfile, pidchar, sizeof(pidchar)))
	{
		printf("udhcpc_pid[%d], %s\n", which_conn, pidchar);
       		ret = eval("kill", "-9", pidchar);
	}
	udhcpc_pid[whichconn] = -1;
	unlink(udhcpcfile);
	return ret;

   #else
	if(udhcpc_pid[whichconn] < 0)
		return 0;
	else
	{
		char pidstr[32];
		char pidfile[32];
		FILE *fp; 
		char *tmp = NULL;
			
		memset(pidfile, 0, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", whichconn);
		
		fp = fopen(pidfile, "r");
		if(fp == NULL)
		{
			printf("no udhcpc work\n");
			return 0;
		}
		
		memset(pidstr, 0, sizeof(pidstr));
		if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
		{
			if((tmp = strstr(pidstr, "\n")) != NULL)
				*tmp = '\0';
		}	
		fclose(fp);
		
		//eval("kill", "-SIGUSR2", pidstr);
		//sleep(1);
		eval("kill", "-9", pidstr);
		
		unlink(pidfile);
		
		dprintf("done\n");
		udhcpc_pid[whichconn] = -1;
		return 0;
	}
    #endif
}

int start_polling_check(void)
{
	char adsl_action[8];	

/* for check adsl */
	if(!file_to_buf("/tmp/adsl_action", adsl_action, sizeof(adsl_action)))
	{
		printf("read file %s fail\n", "/tmp/adsl_action");
		//goto real_redial;
		return -1;
	}
	
	//junzhao 2004.4.15 set signal handler flag!!
	{
	char tmpbuf[] = "1";
	buf_to_file("/var/run/being_busy", tmpbuf);
	}
	
	printf("ok------adsl_action start!\n");
	if(!strcmp(adsl_action, "down"))
	{
		//modify by junzhao 2004.3.11
		int i,j;
		//junzhao 2004.3.18
		stop_upnp(0);
		//stop_upnp(1);
	#ifdef MULTIPVC_SUPPORT
		for(i=0; i<MAX_CHAN; i++)
	#else
		for(i=0; i<1; i++)
	#endif
		//for(i=0; i<MAX_CHAN; i++)
		{
			for(j=0; j<60000000; j++);
			if(sockfd_used[i] >= 0)
				sockfd_ever_used[i] = 1;
			which_conn = i;
			stop_pvc_wan();
			//add by junzhao 2004.3.12
			//if(i == atoi(nvram_safe_get("wan_active_connection")))
			#if 0
			{
				/* save dns to resolv.conf */
        			dns_to_resolv(which_conn);
	
				/* Restart DHCP server */
				stop_dhcpd();
				start_dhcpd();

				/* Restart DNS proxy */
				stop_dns();
				start_dns();

				//junzhao 2004.7.23 
				/* Restart RIP */
				stop_zebra();
				start_zebra();

				//junzhao 2004.7.29 restart qos
				stop_qos();

				//junzhao 2004.7.29 restart ipsec
				stop_ipsec();
				
				stop_firewall();
			}
			#endif	
		}
		
		//junzhao 2004.8.16
		start_upnp();
		
		/* save dns to resolv.conf */
        	dns_to_resolv(which_conn);
	
		/* Restart DHCP server */
		stop_dhcpd();
		start_dhcpd();

		/* Restart DNS proxy */
		stop_dns();
		start_dns();

		//junzhao 2004.7.23 
		/* Restart RIP */
		stop_zebra();
		start_zebra();

	#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
		stop_gre();
	#endif
		
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		stop_qos();
	#endif
		stop_ipsec();	
		
		stop_firewall();
		start_firewall();

		//2004.3.18 junzhao
		//start_upnp();
	}
	else if(!strcmp(adsl_action, "up"))
	{
		int i,j;
		
	#ifdef MULTIPVC_SUPPORT
		for(i=0; i<MAX_CHAN; i++)
	#else
		for(i=0; i<1; i++)
	#endif
		//for(i=0; i<MAX_CHAN; i++)
		{
			if(sockfd_ever_used[i] > 0)
			{
				for(j=0; j<60000000; j++);
				sockfd_ever_used[i] = -1;
				which_conn = i;
				start_wan(BOOT);
			}
		}
		if(firstboot)
			firstboot = 0;   //have pass the first boot
		//junzhao 2004.4.2
		if(!secondenter)
			secondenter = 1;
		
		//junzhao 2004.7.10
		//sleep(2);
		for(i=0; i<150000000; i++);
		defaultgw_table_sort();

		//junzhao 2004.4.19 finally commit to flash!!!
		nvram_commit();

		//junzhao 2004.4.23 to check httpd!!
		{
			FILE *fp = fopen("/var/run/httpd_nosignal", "r");
			if(fp == NULL)
				eval("killall", "-SIGUSR1", "httpd");
			else
				fclose(fp);
			unlink("/var/run/httpd_nosignal");
		}
	}
	//junzhao 2004.7.9
	else if(!strcmp(adsl_action, "pppd"))
	{
		int i;
	#ifdef MULTIPVC_SUPPORT
		for(i=0; i<MAX_CHAN; i++)
	#else
		for(i=0; i<1; i++)
	#endif
		//for(i=0; i<MAX_CHAN; i++)
		{
			if(sockfd_used[i] > 0)
			{
				char encap[15];
				memset(encap, 0, sizeof(encap));
				check_cur_encap(i, encap);
				if(!strcmp(encap, "pppoe"))
				{
					int k, l;
					printf("waiting before check pppd proc.....\n");
					for(l=0; l<20; l++)
					{
						for(k=0; k<80000000; k++)
						;
					}
					printf("waiting done.....\n");
					//sleep(30);
					if(!check_ppp_link(i))
					{
						stop_redial(i);
						stop_pppox(i);
						
						which_conn = i;
						pppoe_global_init();
						start_pppoe(BOOT, 1);
						//start_wan(BOOT);
					}
				#ifdef MPPPOE_SUPPORT
					if(!check_ppp_link(MAX_CHAN + i))
					{
						int count = i;
						char menable[3], *next;
						memset(menable, 0, sizeof(menable));
						foreach(menable, nvram_safe_get("mpppoe_enable"), next)
						{
							if(count -- == 0)
								break;
						}
						if(!strcmp(menable, "1"))
						{
							stop_redial(MAX_CHAN+i);
							stop_pppox(MAX_CHAN+i);
							
							which_conn = i;
							start_pppoe2(BOOT, 1);
						}	
					}
				#endif	
				}
				else if(!strcmp(encap, "1483bridged"))
				{
				#ifdef IPPPOE_SUPPORT
					int count = i;
					char menable[3], *next;
					memset(menable, 0, sizeof(menable));
					foreach(menable, nvram_safe_get("ipppoe_enable"), next)
					{
						if(count -- == 0)
							break;
					}
					if(!strcmp(menable, "1"))
					{
						int k, l;
						printf("waiting before check pppd proc.....\n");
						for(l=0; l<20; l++)
						{
							for(k=0; k<80000000; k++)
							;
						}
						printf("waiting done.....\n");

						if(!check_ppp_link(MAX_CHAN + i))
						{
							stop_redial(MAX_CHAN+i);
							stop_pppox(MAX_CHAN+i);	
							which_conn = i;
							start_pppoe2(BOOT, 1);
						}	
					}
				#endif	
				}	
			}
		}
	}


	unlink("/tmp/adsl_action");	
	
	//junzhao 2004.4.15 clear signal handler flag!!
	{
	char tmpbuf[] = "0";
	buf_to_file("/var/run/being_busy", tmpbuf);
	}
	
	return 0;
/* end for check a`dsl */

#if 0 //junzhao 2004.3.19
/* for redial */
real_redial:
	if(!file_to_buf("/tmp/ppp/redialing", redialing, sizeof(redialing)))
	{
		printf("read file %s fail\n", "/tmp/ppp/redialing");
		return -1;
	}
	printf("ok------redial_action start!\n");
	if(!strcmp(redialing, "1"))
	{
		char pidchar[18];
		if(!file_to_buf("/tmp/ppp/redial_num", redialing, sizeof(redialing)))
		{
			printf("read file %s fail\n", "/tmp/ppp/redial_num");
			return 0;
		}
		
		which_conn = atoi(redial_num);
		
       		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "%d", ppp_pid[which_conn]);
		printf("ppp_pid[%d], %s\n", which_conn, pidchar);
       		eval("kill", "-9", pidchar);
		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "/tmp/ppp/link%d", which_conn);
		unlink(pidchar);
		ppp_pid[which_conn] = -1;
		
		//sleep(2);
		
		vcc_global_init();
		if(!strcmp(encapmode, "pppoe"))
		{
			pppoe_global_init();
			start_pppoe(REDIAL);
		}
		else if(!strcmp(encapmode, "pppoa"))
		{
			pppoa_global_init();
			start_pppoa(AUTODISABLE, REDIAL);
		}
		unlink("/tmp/ppp/redialing");
		unlink("/tmp/ppp/redial_num");
	}
	return 0;
/* end for redial */
#endif 
}
void enable_disable_pppoe_pass()
{
	FILE *fp;
	if (!(fp = fopen("/proc/Cybertan/pppoe_pass", "w"))) {
		cprintf("error to open /proc/Cybertan/half_bridge_enable");
	}else {
		if(nvram_match("pppoe_pass", "1"))
			fprintf(fp, "%d", 1); /*enable pppoe_pass*/
		else
			fprintf(fp, "%d", 0);/*disable pppoe_pass*/
		fclose(fp);
	}

}

void enable_disable_icmp_block(int i)
{
	FILE *fp;
	if (!(fp = fopen("/proc/Cybertan/icmp_block", "w"))) {
		cprintf("error to open /proc/Cybertan/icmp_block");
	}else {
		fprintf(fp, "%d", i); /*enable/disable icmp_block*/
		fclose(fp);
	}

}

extern int vpivci_final_value[MAX_CHAN][2];

/* xiaoqin modify function start_single_service in 2005.04.27 *
 * when configure pppox's username/password using Motive tool,*
 * connect() should be able to pick up new username/password  *
 * and reboot shouldn't be required                           */
int
start_single_service(void)
{
	char *service;
	
real_start:
	service = nvram_get("action_service");

//wwzh
	if(!service)
		kill(1, SIGHUP);

	cprintf("Restart service=[%s]\n",service);
	
//add by lijunzhao
	//if(!strncmp(service, "connect_ppp", strlen("connect_ppp")))
	if(!strcmp(service, "connect_static"))
	{
		//char pidchar[18];
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		
		sleep(2);
		
		if(vcc_global_init() < 0)
			goto out;
		
		if(!strcmp(encapmode, "1483bridged"))
		{
			if(bridged_global_init() < 0)
				goto out;
			if(!strcmp(dhcpenable, "0"))
			{
				if(rfc2684_load(AUTODISABLE) < 0)
				{
					printf("rfc2684 load fail(1483bridged,nonauto)\n");
					ram_modify_status(WAN_PVC_STATUS, "Down");
					goto out;
				}
				ram_modify_status(WAN_PVC_STATUS, "Applied");				
				start_static(BRIDGED);
			}
			else
				goto out;
		}
		else if(!strcmp(encapmode, "routed"))
		{
			if(routed_global_init() < 0)
				goto out;
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(1483bridged,nonauto)\n");
				ram_modify_status(WAN_PVC_STATUS, "Down");
				goto out;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");			
			start_static(ROUTED);
		}
		else if(!strcmp(encapmode, "bridgedonly"))
		{
			if(rfc2684_load(AUTODISABLE) < 0)
			{
				printf("rfc2684 load fail(1483bridged,nonauto)\n");
				ram_modify_status(WAN_PVC_STATUS, "Down");
				goto out;
			}
			ram_modify_status(WAN_PVC_STATUS, "Applied");			
			process_bridgedonly();
		}
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
		{
			char *atmarpd_argv[] = {"atmarpd", NULL};
			char ifname[5];
			char vpivci_str[10];
			pid_t pid;
			
			if(clip_global_init() < 0)
				goto out;
				
			if(!atmarpd_index)
				_eval(atmarpd_argv, NULL, 0, &pid);
			//atmarpd_num++;
			atmarpd_index |= (1<<which_conn);
			
			sleep(1);
			memset(ifname, 0, sizeof(ifname));
			sprintf(ifname, "atm%d", which_conn);
			eval("atmarp", "-c", ifname);
			sleep(1);
			
	       	vpivci_final_value[which_conn][0] = atoi(vvpi);
			vpivci_final_value[which_conn][1] = atoi(vvci);
			memset(vpivci_str, 0, sizeof(vpivci_str));
			sprintf(vpivci_str, "%d.%d", atoi(vvpi), atoi(vvci));
			
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			sockfd_used[which_conn] = 0;
			//junzhao 2004.8.10
			ram_modify_status(WAN_ENCAP, "clip");
			ram_modify_status(WAN_PVC_STATUS, "Applied");
			
			start_static(CLIP);
		}
	#endif
			
		printf("haha................\n");
		//junzhao 2004.4.14 send a signal to httpd to wake it up
		eval("killall", "-SIGUSR1", "httpd");
		/*
		nvram_set("action_service","");
		nvram_set("action_service_arg1","");
		return 0;
		*/
		
		//junzhao 2004.7.10
		//sleep(3);
		//defaultgw_table_sort();
	}
	else if(!strcmp(service, "disconnect_static"))
	{
		//char pidstr[5];
		int i;
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		ram_modify_status(WAN_IFNAME, "Down");
		ram_modify_status(WAN_PVC_ERRSTR, "Down");
		ram_modify_status(WAN_PVC_STATUS, "Down");
		ram_modify_status(WAN_IPADDR, "0.0.0.0");
		ram_modify_status(WAN_NETMASK, "0.0.0.0");
		ram_modify_status(WAN_GATEWAY, "0.0.0.0");
		ram_modify_status(WAN_BROADCAST, "0.0.0.0");
		ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
		ram_modify_status(WAN_HWADDR, "00:00:00:00:00:00");
		
	#ifdef VZ7
		ram_modify_status(WAN_DHCPD_IPADDR, "0.0.0.0");
	#endif

		stop_wan();
		for(i=0; i<200000000; i++);
		if(which_conn == atoi(nvram_safe_get("wan_enable_last")))
				stop_wan_service();

		if(add_next_gw())
		{
			start_ddns();
			start_ntp();
			start_email_alert();
			stop_upnp(2);
		#ifdef PARENTAL_CONTROL_SUPPORT
			start_parental_control();
		#endif
		}
				
		if(sockfd_used[which_conn] > 0)
			close(sockfd_used[which_conn]);
		
			
		sockfd_used[which_conn] = -1;                  
	      	 vpivci_final_value[which_conn][0] = -1;					        
		vpivci_final_value[which_conn][1] = -1;		 

cprintf("disconnect static done!\n");	 
	}
///////////////////kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	
	
	else if(!strcmp(service, "connect_ppp"))
	{
		int which_real;
		int i;
		char *tmpptr, *ptr, encap[15], connecttype[3];
		char tempbuf[28], chflag[4];//xiaoqin add 2005.04.27
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		which_real = which_conn;
		
		//junzhao 2004.11.2 for demand disconnect button!
		memset(encap, 0, sizeof(encap));
		check_cur_encap(which_conn, encap);
		memset(connecttype, 0, sizeof(connecttype));
		
		if(!strcmp(encap, "pppoe"))
			ptr = nvram_safe_get("pppoe_config");
		else if(!strcmp(encap, "pppoa"))
			ptr = nvram_safe_get("pppoa_config");

		/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encap, "unpppoe"))
			ptr = nvram_safe_get("unip_pppoe_config");
		else if(!strcmp(encap, "unpppoa"))
			ptr = nvram_safe_get("unip_pppoa_config");	  
#endif

		
		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == which_conn)
			{
				string_sep(tmpptr, 3, connecttype);	
				break;
			}
		}
		/*************** xiaoqin modify start 2005.04.27 ******************/
		sprintf(tempbuf, "pppox_change_userpasswd_%d", which_conn);
		strcpy(chflag, nvram_get(tempbuf));
		/* (keep alive mode)
		|| (demand mode && cpe tools has changed the pppox's user\passwd) */
		if((connecttype[0] == '0') || (0 == strcmp(chflag, "yes")))
		{
			if((0 == strcmp(chflag, "yes")) && (connecttype[0] == '1'))
			{
				char pppname[6];
				memset(pppname, 0, sizeof(pppname));
				sprintf(pppname, "ppp%d", which_conn);
				down_ipsec(pppname, which_conn);
			}
			stop_redial(which_real);
			stop_pppox(which_real);
		
			sleep(2);
		
			if(vcc_global_init() < 0)
				goto out;
					
			if(!strcmp(encapmode, "pppoe"))
			{
				pppoe_global_init();
				start_pppoe(BOOT, 1);
			}
			else if(!strcmp(encapmode, "pppoa"))
			{
				pppoa_global_init();
				start_pppoa(AUTODISABLE, BOOT);
			}
			/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT 
                    else if(!strcmp(encapmode, "unpppoe"))
                    {
                            unip_pppoe_global_init();
				start_unpppoe(BOOT, 1);
                    }
		      else if(!strcmp(encapmode, "unpppoa"))
                    {
                            unip_pppoa_global_init();
				start_unpppoa(AUTODISABLE, BOOT);
                    }			
#endif

		}
		//demand mode
		if((connecttype[0] == '1'))
		{
			char wan_ifname[6], wangateway[16];
			int timeout = 5;
			memset(wan_ifname, 0, sizeof(wan_ifname));
			memset(wangateway, 0, sizeof(wangateway));
			sprintf(wan_ifname, "ppp%d", which_conn);
			while(wanstatus_info_get(wan_ifname, "gateway", &wangateway[0]) < 0 && timeout--);
			if(strcmp(wangateway, "0.0.0.0"))
			{	
				printf("connect ppp:  %s\n", wangateway);
				eval("ping", "-c", "1", wangateway);
				sleep(1);
			}
		}
		nvram_set(tempbuf, "no");
		/*********************** xiaoqin modify end ************************/
	
	//junzhao 2005.3.1
	#if 0
		//junzhao 2004.11.2 for wanledctrl
		{
		FILE *fp;
		fp = fopen("/var/run/resetledctrl", "w");
		fclose(fp);
		}
	#endif
		
		printf("haha................\n");
		//junzhao 2004.4.14 send a signal to httpd to wake it up
		
		/* zhangbin remove this line 2005.5.31 */
		//eval("killall", "-SIGUSR1", "httpd");

		/* kirby for unip 2004/11.23 */
#ifdef UNNUMBERED_SUPPORT
	       sleep(4);
	       printf("Start Unnumbered IP Mode\n");
              if(unip_start() < 0)
	   	    printf("Unnumbered IP mode failed\n");
#endif

	}
	//else if(!strncmp(service, "disconnect_ppp", strlen("disconnect_ppp")))
	else if(!strcmp(service, "disconnect_ppp"))
	{
		int which = atoi(nvram_safe_get("wan_post_connection"));
		int i;
		char *tmpptr, *ptr, encap[15], connecttype[3];
		
		//junzhao 2004.11.2 for demand disconnect button!
		memset(encap, 0, sizeof(encap));
		check_cur_encap(which, encap);
		memset(connecttype, 0, sizeof(connecttype));
		
		if(!strcmp(encap, "pppoe"))
			ptr = nvram_safe_get("pppoe_config");
		else if(!strcmp(encap, "pppoa"))
			ptr = nvram_safe_get("pppoa_config");

		/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encap, "unpppoe"))
			ptr = nvram_safe_get("unip_pppoe_config");
		else if(!strcmp(encap, "unpppoa"))
			ptr = nvram_safe_get("unip_pppoa_config");	  
#endif

		
		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == which)
			{
				string_sep(tmpptr, 3, connecttype);	
				break;
			}
		}
		//demand mode
		if(connecttype[0] == '1')
			stop_pppox_demand(which);
		
		//keep alive mode	
		else
		{
			char pppname[6];
			memset(pppname,0,sizeof(pppname));
			sprintf(pppname,"ppp%d",which);
			down_ipsec(pppname,which);

			stop_redial(which);
			stop_pppox(which);
		}
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	//else if(!strncmp(service, "connect_multi_ppp", strlen("connect_multi_ppp")))
	else if(!strcmp(service, "connect_multi_ppp"))
	{
		int which_real; 
		//junzhao 2004.11.2 for demand disconnect button
		char *tmpptr, *ptr, encap[15], connecttype[3];
		int i;
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		which_real = MAX_CHAN + which_conn;
		
		memset(connecttype, 0, sizeof(connecttype));
		ptr = nvram_safe_get("mpppoe_config");

		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == which_conn)
			{
				string_sep(tmpptr, 3, connecttype);	
				break;
			}
		}
		//demand mode
		if(connecttype[0] == '1')
		{
			char wan_ifname[6], wangateway[16];
			int timeout = 5;
			memset(wan_ifname, 0, sizeof(wan_ifname));
			memset(wangateway, 0, sizeof(wangateway));
			sprintf(wan_ifname, "ppp%d", which_real);
			while(wanstatus_info_get(wan_ifname, "gateway", &wangateway[0]) < 0 && timeout--);
			if(strcmp(wangateway, "0.0.0.0"))
			{		
				printf("connect ppp:  %s\n", wangateway);
				eval("ping", "-c", "1", wangateway);
				sleep(1);
			}
		}
		//keepalive mode
		else
		{
			stop_redial(which_real);
			stop_pppox(which_real);
		
			sleep(2);
		
			start_pppoe2(BOOT, 1);
		}
		
		//junzhao 2004.4.14 send a signal to httpd to wake it up
		eval("killall", "-SIGUSR1", "httpd");
	}
	//else if(!strncmp(service, "disconnect_multi_ppp", strlen("disconnect_multi_ppp")))
	else if(!strcmp(service, "disconnect_multi_ppp"))
	{
		int which_real = MAX_CHAN + atoi(nvram_safe_get("wan_post_connection"));
		//junzhao 2004.11.2 for demand disconnect button!
		int i;
		char *tmpptr, *ptr, connecttype[3];
		
		memset(connecttype, 0, sizeof(connecttype));
		ptr = nvram_safe_get("mpppoe_config");
		for(i=0; i<MAX_CHAN; i++)
		{
			tmpptr = strsep(&ptr, " ");
			if(i == which_real-MAX_CHAN)
			{
				string_sep(tmpptr, 3, connecttype);	
				break;
			}
		}
		
		//demand mode
		if(connecttype[0] == '1')
			stop_pppox_demand(which_real);
		
		//keep alive mode	
		else
		{
			stop_redial(which_real);
			stop_pppox(which_real);
		}	
	}
	
#endif
	else if(!strcmp(service, "dhcp_release"))
	{
		char pidstr[6];
		char pidfile[32];
		FILE *fp; 
		char *tmp = NULL;
		int which = atoi(nvram_safe_get("wan_post_connection"));
		
		//zhangbin --2004.08.12
		{
			char ifname[6];
			memset(ifname,0,sizeof(ifname));
			sprintf(ifname,"nas%d",which);
			down_ipsec(ifname,which);
		}
	
		memset(pidfile, 0, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", which);
		
		fp = fopen(pidfile, "r");
		if(fp == NULL)
		{
			printf("no udhcpc work\n");
			goto out;
		}
		
		memset(pidstr, 0, sizeof(pidstr));
		if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
			udhcpc_pid[which] = atoi(pidstr);
		
		if((tmp = strstr(pidstr, "\n")) != NULL)
			*tmp = '\0';
		
		fclose(fp);
		
		printf("release pidstr %s\n", pidstr);
		
		eval("kill", "-SIGUSR2", pidstr);
		
		//junzhao 2004.4.14 send a signal to httpd to wake it up
		eval("killall", "-SIGUSR1", "httpd");
	}
	else if(!strcmp(service, "dhcp_renew"))
	{
		char pidstr[6];
		char pidfile[32];
		FILE *fp; 
		char *tmp = NULL;
		int which = atoi(nvram_safe_get("wan_post_connection"));
			
		memset(pidfile, 0, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", which);
		fp = fopen(pidfile, "r");
		if(fp == NULL)
		{
			printf("no udhcpc work\n");
			goto out;
		}
		
		memset(pidstr, 0, sizeof(pidstr));
		if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
			udhcpc_pid[which] = atoi(pidstr);
		
		if((tmp = strstr(pidstr, "\n")) != NULL)
			*tmp = '\0';
		
		fclose(fp);
		
		printf("renew pidstr %s\n", pidstr);
	
		//junzhao 2004.11.2 for wanledctrl
		{
		char filename[32];
	
	//junzhao 2005.3.1
	#if 0
		FILE *fp;
		fp = fopen("/var/run/resetledctrl", "w");
		fclose(fp);
	#endif

		memset(filename, 0, strlen("/var/run/udhcpc.down"+3));
		sprintf(filename, "/var/run/udhcpc%d.down", which);
		unlink(filename);
		}

		eval("kill", "-SIGUSR1", pidstr);
		
		//junzhao 2004.4.14 send a signal to httpd to wake it up
		eval("killall", "-SIGUSR1", "httpd");
		
	}
	else if(!strcmp(service, "clone_mac"))
	{
		char devname[8];
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		memset(devname, 0, sizeof(devname));
		sprintf(devname, "nas%d", which_conn);
		mac_clone(devname);
	}
	//junzhao 2004.6.2
	else if(!strcmp(service, "pvc_select"))
	{
		int which_to_select = atoi(nvram_safe_get("active_connection_selection"));
		if(which_to_select < 32)
		{
	        	//enable_disable_icmp_block(1);	
			stop_firewall();
			start_firewall();
	        	//enable_disable_icmp_block(0);	
		}
		printf("ok!!!\n");
	}
	/*
	else if(!strcmp(service, "redial"))
	{
		char pidchar[18];
		which_conn = atoi(nvram_safe_get("redial_num"));
		//stop_redial();
		
		stop_ntp();
		stop_ddns();
		stop_zebra();
		stop_upnp();
		
		//stop_pppox(which_conn);
		
       		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "%d", ppp_pid[which_conn]);
		printf("ppp_pid[%d], %s\n", which_conn, pidchar);
       		eval("kill", "-9", pidchar);
		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "/tmp/ppp/link%d", which_conn);
		unlink(pidchar);
		ppp_pid[which_conn] = -1;
		
		sleep(2);
		
		vcc_global_init();
		if(!strcmp(encapmode, "pppoe"))
		{
			pppoe_global_init();
			start_pppoe(REDIAL);
		}
		else if(!strcmp(encapmode, "pppoa"))
		{
			pppoa_global_init();
			start_pppoa(REDIAL);
		}
		nvram_unset("redial_num");
		//return 0;
	}
	*/
//end add by lijunzhao
	
	else if(!strcmp(service,"dhcp")){
		stop_dhcpd();
		start_dhcpd();
	}
	//add by lzh;
	else if(!strcmp(service,"dhcpc"))
	{
		stop_dhcpcc();
		start_dhcpd();
	}	
	//by songtao
	else if(!strcmp(service,"snmpd"))
	{     
	        //enable_disable_icmp_block(1);	
		stop_upnp(0);
		if (nvram_match("upnp_enable", "1"))
			start_upnp();

//		if(nvram_match("filter", "on"))
		{
			stop_firewall();
			start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	

		}
		/*
		//if(nvram_match("filter", "on"))
		{
			stop_firewall();
			start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	

		}
		*/
	       	sleep(1);
		stop_snmp();
		sleep(2);
		start_snmp();
		
		// add for IGMP Proxy 2004-10-10
	#ifdef IGMP_PROXY_SUPPORT
		stop_igmp_proxy();
		sleep(2);
		start_igmp_proxy();
	#endif

	#ifdef CPED_TR064_SUPPORT
		stop_tr064();//guohong
	#endif
		
	#ifdef CPED_TR064_SUPPORT
		start_tr064();//guohong
	#endif
	
	#ifdef WIRELESS_SUPPORT
		start_management_via_wlan();
	#endif
		//return 0;
	        //enable_disable_icmp_block(0);	
		//
		//
	#ifdef CLI_SUPPORT		
	/* zhangbin 2005/01/17 */
		stop_telnetd();
		start_telnetd();
	#endif	

	}
#ifdef PARENTAL_CONTROL_SUPPORT
        else if(!strcmp(service,"artemis"))
	{
        	stop_parental_control();
                start_parental_control();
	}	
#endif
	else if(!strcmp(service,"start_pppoe") || !strcmp(service,"start_pptp") || !strcmp(service,"start_heartbeat")){
		unlink("/tmp/ppp/log");
		
		which_conn = atoi(nvram_safe_get("wan_ddm_connection"));
		
		stop_redial(which_conn);
		stop_pppox(which_conn);
		sleep(2);
		
		while(1)
		{
			sleep(1);
			if(check_adsl_status())
				break;
		}

		if(vcc_global_init() < 0)
			return -1;
		
		if(!strcmp(encapmode, "pppoe"))
		{
			pppoe_global_init();
			start_pppoe(BOOT, 1);
		}

		/*
		stop_lan();
		stop_wan();
		start_lan();
		start_wan(BOOT);
		*/
	}
	else if(!strcmp(service,"stop_pppoe") || !strcmp(service,"stop_pptp") || !strcmp(service,"stop_heartbeat")){
		//stop_wan();
		int which = atoi(nvram_safe_get("wan_ddm_connection"));
		stop_redial(which);
		stop_pppox(which);

	}
	else if(!strcmp(service,"filters")
#ifdef FIREWALL_LEVEL_SUPPORT
		|| !strcmp(service,"filters_inb")
#endif
	){
		stop_cron();
		start_cron();
		//stop_iptqueue();
		//start_iptqueue();
		stop_firewall();
		start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
	}
	else if(!strcmp(service,"forward")){
		stop_firewall();
		start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
	}
#ifdef ADVANCED_FORWARD_SUPPORT//add by zhs for adv forwarding
        else if(!strcmp(service,"advanced_forwarding")){
                stop_firewall();
                start_firewall();
                //  songtao for add/del artemis rules  6.2
                #ifdef PARENTAL_CONTROL_SUPPORT
                        stop_parental_control();
                        start_parental_control();
                #endif
        }
  #endif

	else if(!strcmp(service,"forward_upnp")){
		//junzhao 2004.3.18
		//added by leijun 2004-03-20
		if (nvram_match("nat_enable", "1"))
		{
			stop_firewall();
			stop_upnp(0);
			stop_tr64_1();

			start_firewall();
			start_upnp();
			if (nvram_match("upnp_enable", "0"))
			{
				//stop_firewall();
				//start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
			}
			//add by lzh;
			//stop_tr64_1();
			start_tr064();
		}
	}
	else if(!strcmp(service,"static_route_add"))
	{
#if 1
		stop_zebra();
		//junzhao 2004.3.18
		stop_upnp(0);
		stop_firewall();
#if 0
		start_nat("down");
		if (nvram_match("nat_enable", "1"))
		{
			start_firewall();
			start_upnp();
		}else {
			eval("/usr/sbin/iptables", "-t", "nat", "-F");
		}
		start_nat("up");
#endif
		start_zebra();
		start_firewall();
		start_upnp();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
#endif 
	}
	else if(!strcmp(service,"static_route_del")){
		if(nvram_safe_get("action_service_arg1")){
			del_routes(nvram_safe_get("action_service_arg1"));
		}
	}
	else if(!strcmp(service,"ddns")){
		stop_ddns();
		start_ddns();
		nvram_set("ddns_change","update");
	}
#if 0
	//cjg:2004-3-7:PM
	else if(!strcmp(service,"start_ping")){
		char *ip;
		char *tmp = nvram_safe_get("ping_ip");
		ip = malloc(strlen(tmp) + 1);
		strncpy(ip, tmp, strlen(tmp));
		ip[strlen(tmp)] = '\0';
		if(/*!check_wan_link(0)*/0)
			buf_to_file(PING_TMP, "Network is unreachable");
		
		else if(strchr(ip, ' ') || strchr(ip, '`') || strstr(ip, PING_TMP))		// Fix Ping.asp bug, user can execute command ping in Ping.asp
			buf_to_file(PING_TMP, "Invalid IP Address or Domain Name");
			
		else if(nvram_invmatch("ping_times","") && nvram_invmatch("ping_ip","")){
			char cmd[80];
			snprintf(cmd, sizeof(cmd), "ping -c %s -f %s %s &", nvram_safe_get("ping_times"), PING_TMP, ip);
	   	     	printf("cmd=[%s]\n",cmd);
	        	system(cmd);    
		}
		free(ip);
	}
	else if(!strcmp(service,"start_traceroute")){
		char *tmp = nvram_safe_get("traceroute_ip");
		char *ip = malloc(strlen(tmp) + 1);
		strncpy(ip, tmp, strlen(tmp));
		ip[strlen(tmp)] = '\0';
		
		if(/*!check_wan_link(0)*/0)
			buf_to_file(TRACEROUTE_TMP, "Network is unreachable");
		
		else if(strchr(ip, ' ') || strchr(ip, '`') || strstr(ip, TRACEROUTE_TMP))	// Fix Traceroute.asp bug, users can execute command in Traceroute.asp 
			buf_to_file(TRACEROUTE_TMP, "Invalid IP Address or Domain Name");
		
		else if(nvram_invmatch("traceroute_ip","")){
			char cmd[80];
			snprintf(cmd, sizeof(cmd), "traceroute -f %s %s &", TRACEROUTE_TMP, ip);
	        	printf("cmd=[%s]\n",cmd);
	        	system(cmd);    
		}
		free(ip);
	}
#endif
	/* zhangbin for Ipsec starting*/
	else if(!strcmp(service,"ipsec"))
	{
	//	if(nvram_match("filter", "on"))
		
		{
	//		if(nvram_match("ipsec_pass", "0") ||
	//		   nvram_match("pptp_pass", "0") ||
	//		   nvram_match("l2tp_pass", "0"))
			{
				stop_firewall();
				sleep(2);
				start_firewall();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
			}
			
		}
		enable_disable_pppoe_pass();
		stop_ipsec();
		sleep(2);
		start_ipsec();

		/* zhangbin 2005.5.11 */
		#ifdef CLOUD_SUPPORT
		if(nvram_match("cloud_enable","1"))
		{
			stop_dhcpd();
			start_dhcpd();
		}
		#endif
		
	}	
	//zhangbin for qos 2004.7.19
	else if(!strcmp(service,"qos"))
	{
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		stop_qos();
		start_qos();
	#endif
	}
	//csh 2005.3.11
	else if(!strcmp(service, "basic_qos"))
	{
	#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
		stop_qos();
		start_qos();
	#endif
	}
	//cjg:2003-3-12
	else if(!strcmp(service, "email_alert")){
		stop_email_alert();
		stop_firewall();
		stop_SysLog();		
		sleep(2);
		printf("email_alert restart\n");
		start_email_alert();	
		start_firewall();		
		start_SysLog();
		//  songtao for add/del artemis rules  6.2
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
 			start_parental_control();	
		#endif	
	}										else if(!strcmp(service,"tftp_upgrade")){
	//junzhao 2004.4.7
	#ifdef WIRELESS_SUPPORT
		stop_wireless();
	#endif
		//stop_wan();
		//stop_httpd();
		stop_zebra();
		//junzhao 2004.3.18
		stop_upnp(0);
		stop_cron();
	}
	else if(!strcmp(service,"http_upgrade")){
		stop_wan();
		stop_zebra();
		//junzhao 2004.3.18
		stop_upnp(0);
		stop_cron();
	}
	else if(!strcmp(service,"wireless")){
	//junzhao 2004.4.7
	#ifdef WIRELESS_SUPPORT
		restart_wireless();
	#endif
	}
	else if(!strcmp(service, "GRETunnel")){
	//csh for gre
	#ifdef GRE_SUPPORT
		stop_gre();
		enable_gre();
	#endif
	}
	else if(!strcmp(service, "Cloud")){
	//csh for Cloud
	#ifdef CLOUD_SUPPORT
#if 0
		stop_gre();
		enable_gre();
#else
		disable_cloud();
		enable_cloud();

#endif
	#endif
	}
	else{
		nvram_unset("action_service");
		nvram_unset("action_service_arg1");
		kill(1, SIGHUP);
	}
out:
	nvram_set("action_service","");
	nvram_set("action_service_arg1","");

	return 0;
}

int
start_pptp(int status)
{
#if 0
	int ret;
	FILE *fp;
	char *pptp_argv[] = { "pppd",
			      NULL
	};
	char username[80],passwd[80];

	//stop_dhcpc();
	//stop_pppoe();

	if(nvram_match("aol_block_traffic","0")){
		snprintf(username, sizeof(username), "%s", nvram_safe_get("ppp_username"));
		snprintf(passwd, sizeof(passwd), "%s", nvram_safe_get("ppp_passwd"));
	}
	else{
		if(!strcmp(nvram_safe_get("aol_username"),"")){
			snprintf(username, sizeof(username), "%s", nvram_safe_get("ppp_username"));
			snprintf(passwd, sizeof(passwd), "%s", nvram_safe_get("ppp_passwd"));
		}
		else{
			snprintf(username, sizeof(username), "%s", nvram_safe_get("aol_username"));
			snprintf(passwd, sizeof(passwd), "%s", nvram_safe_get("aol_passwd"));
		}		
	}

	if(status != REDIAL){
		mkdir("/tmp/ppp", 0777);
		symlink("/sbin/rc", "/tmp/ppp/ip-up");
		symlink("/sbin/rc", "/tmp/ppp/ip-down");
		symlink("/dev/null", "/tmp/ppp/connect-errors");

		/* Generate options file */
	       	if (!(fp = fopen("/tmp/ppp/options", "w"))) {
       	        	perror("/tmp/ppp/options");
       	        	return -1;
	       	}
	       	fprintf(fp, "defaultroute\n");  //Add a default route to the system routing tables, using the peer as the gateway
      	 	fprintf(fp, "usepeerdns\n");    //Ask the peer for up to 2 DNS server addresses
       		fprintf(fp, "pty 'pptp %s --nolaunchpppd'\n",nvram_safe_get("pptp_server_ip")); 
       		fprintf(fp, "user '%s'\n",username);
       		//fprintf(fp, "persist\n");        // Do not exit after a connection is terminated.

		if(nvram_match("mtu_enable", "1")){
       			fprintf(fp, "mtu %s\n",nvram_safe_get("wan_mtu"));
		}
	
		if(nvram_match("ppp_demand", "1")){ //demand mode
       			fprintf(fp, "idle %d\n",nvram_match("ppp_demand", "1") ? atoi(nvram_safe_get("ppp_idletime"))*60 : 0);
       			fprintf(fp, "demand\n");         // Dial on demand
       			fprintf(fp, "persist\n");        // Do not exit after a connection is terminated.
	       		fprintf(fp, "%s:%s\n",PPP_PSEUDO_IP,PPP_PSEUDO_GW);   // <local IP>:<remote IP>
       			fprintf(fp, "ipcp-accept-remote\n");        
       			fprintf(fp, "ipcp-accept-local\n");        
       			fprintf(fp, "connect true\n"); 
       			fprintf(fp, "noipdefault\n");          // Disables  the  default  behaviour when no local IP address is specified
       			fprintf(fp, "ktune\n");         // Set /proc/sys/net/ipv4/ip_dynaddr to 1 in demand mode if the local address changes
		}
		else{	// keepalive mode
			char *second = "2";
			start_redial(second, "ppp0");
		}

    	  	fprintf(fp, "default-asyncmap\n"); // Disable  asyncmap  negotiation
		fprintf(fp, "nopcomp\n");	// Disable protocol field compression
		fprintf(fp, "noaccomp\n");	// Disable Address/Control compression 
       		fprintf(fp, "noccp\n");         // Disable CCP (Compression Control Protocol)
       		fprintf(fp, "novj\n");          // Disable Van Jacobson style TCP/IP header compression
       		fprintf(fp, "nobsdcomp\n");     // Disables BSD-Compress  compression
       		fprintf(fp, "nodeflate\n");     // Disables Deflate compression
       		fprintf(fp, "lcp-echo-interval 0\n");     // Don't send an LCP echo-request frame to the peer
       		fprintf(fp, "lock\n");
       		fprintf(fp, "noauth");
	
       		fclose(fp);

       		/* Generate pap-secrets file */
       		if (!(fp = fopen("/tmp/ppp/pap-secrets", "w"))) {
       	        	perror("/tmp/ppp/pap-secrets");
        	       	return -1;
       		}
       		fprintf(fp, "\"%s\" * \"%s\" *\n",
			username,
			passwd);
       		fclose(fp);
		chmod("/tmp/ppp/pap-secrets", 0600);

       		/* Generate chap-secrets file */
       		if (!(fp = fopen("/tmp/ppp/chap-secrets", "w"))) {
        	       	perror("/tmp/ppp/chap-secrets");
               		return -1;
       		}
       		fprintf(fp, "\"%s\" * \"%s\" *\n",
			username,
			passwd);
       		fclose(fp);
		chmod("/tmp/ppp/chap-secrets", 0600);

		/* Enable Forwarding */
		if ((fp = fopen("/proc/sys/net/ipv4/ip_forward", "r+"))) {
			fputc('1', fp);
			fclose(fp);
		} else
			perror("/proc/sys/net/ipv4/ip_forward");
	}
	
	/* Bring up  WAN interface */
	ifconfig(nvram_safe_get("wan_ifname"), IFUP,
		 nvram_safe_get("wan_ipaddr"), nvram_safe_get("wan_netmask"));

	ret = _eval(pptp_argv, NULL, 0, NULL); 
	
	if (nvram_match("ppp_demand", "1")){
		/* Trigger Connect On Demand if user press Connect button in Status page */
		if(nvram_match("action_service","start_pptp")){
			force_to_dial();
			nvram_set("action_service","");
		}
		/* Trigger Connect On Demand if user ping pptp server */
		else
			eval("listen", nvram_safe_get("lan_ifname"));
	}

	dprintf("done\n");
	return ret;
#endif
	return 0;
}

int
stop_pptp(void)
{
	int ret;

	unlink("/tmp/ppp/link");
        ret = eval("killall","-9","pppd");
        ret += eval("killall","-9","pptp");
        ret += eval("killall","-9","listen");

	dprintf("done\n");
	return ret ;
}

int check_ppp_links()
{
	int i;
	for(i=0; i<MAX_CHAN; i++)
	{
		if(check_ppp_link(i))
			return 1;
	}
	return 0;
}

//junzhao 2004.11.2 modify wanledctrl...
int wanledctrl_main(int argc, char **argv)
{
//junzhao 2005.3.1
#if 0
	char *which_pvc = argv[1];
#endif
	int which_pvc = atoi(argv[1]);
	
	char *mode = argv[2];
	char *submode = argv[3];
	int last_tx, last_rx;
	int curr_tx, curr_rx;
	FILE *fp;

//junzhao 2005.3.1
#if 0
	int time_count = 0;
#endif
	
	last_tx = sar_getstats("Tx Total Bytes:");
	last_rx = sar_getstats("Rx Total Bytes:");
	
	while(1)
	{
		//printf("time_count %d\n", time_count);
		sleep(1);

	//junzhao 2005.3.1
	#if 0		
		if(fp = fopen("/var/run/resetledctrl", "r"))
		{
			fclose(fp);
			unlink("/var/run/resetledctrl");
			time_count = 0;
		}
		time_count++;
	#endif		
		
		if(!check_adsl_status())
			diag_led_ctl(PPP_OFF);
		else
		{
			curr_tx = sar_getstats("Tx Total Bytes:");
			curr_rx = sar_getstats("Rx Total Bytes:");
			
			if(!strcmp(mode, "ppp"))
			{
				if(check_ppp_authfail(which_pvc))
				{
					diag_led_ctl(PPP_FAIL);		
					continue;
				}
				if(check_ppp_proc(which_pvc) && check_ppp_link(which_pvc))
				{
					if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
					{
						last_tx = curr_tx;
						last_rx = curr_rx;
						diag_led_ctl(PPP_ACT);
					}
					else
						diag_led_ctl(PPP_CON);						}
				else
					diag_led_ctl(PPP_OFF);

					
			//junzhao 2005.3.1
			#if 0
				if(!strcmp(submode, "demand"))
				{
					if(check_ppp_proc(atoi(which_pvc)))						{
						if(check_ppp_link(atoi(which_pvc)))
						{
							if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
							{
								last_tx = curr_tx;
								last_rx = curr_rx;
								diag_led_ctl(PPP_ACT);
							}
							else
								diag_led_ctl(PPP_CON);						
						}	
						else
						{
							//for idle state
							if(check_ppp_idle(atoi(which_pvc)))
							{
								time_count = 0;
								diag_led_ctl(PPP_CON);
								continue;
							}
							//for begin status
							if(check_ppp_begin(atoi(which_pvc)))
							{
								time_count = 0;
								diag_led_ctl(PPP_OFF);
								continue;
							}
							//wait 8 sec
							if(time_count > 7)
								diag_led_ctl(PPP_FAIL);		
						}
					}
					else 
						diag_led_ctl(PPP_OFF);
				}
				else if(!strcmp(submode, "keepalive"))
				{
					if(check_ppp_proc(atoi(which_pvc)))	
					{
						if(check_ppp_link(atoi(which_pvc)))
						{
							if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
							{
								last_tx = curr_tx;
								last_rx = curr_rx;
								diag_led_ctl(PPP_ACT);
							}
							else
								diag_led_ctl(PPP_CON);
						}
						else
						{
							//wait 8 sec
							if(time_count > 7)
								diag_led_ctl(PPP_FAIL);
						}
					}
					else
					{
						if(check_redial_proc(atoi(which_pvc)))
							diag_led_ctl(PPP_FAIL);
						else
							diag_led_ctl(PPP_OFF);
					}
				}
			#endif
			}
			else if(!strcmp(mode, "dhcp"))
			{	
				if(check_udhcpc_proc(which_pvc))
				{
					if(check_udhcpc_link(which_pvc))
					{
						if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
						{
							last_tx = curr_tx;
							last_rx = curr_rx;
							diag_led_ctl(PPP_ACT);
						}
						else
							diag_led_ctl(PPP_CON);
					}
				
				//junzhao 2005.3.1
				#if 0
					else if(check_udhcpc_down(which_pvc))
						diag_led_ctl(PPP_OFF);
					//wait 8 sec
					else if(time_count > 7)
						diag_led_ctl(PPP_FAIL);
				#endif
					else
						diag_led_ctl(PPP_OFF);
				}
				else
					diag_led_ctl(PPP_OFF);
			}
			else if(!strcmp(mode, "static"))
			{
				if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
				{
					last_tx = curr_tx;
					last_rx = curr_rx;
					diag_led_ctl(PPP_ACT);
				}
				else
					diag_led_ctl(PPP_CON);		
			}
			/*	
			else if(!strcmp(mode, "bridgedonly"))
			{
				if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
				{
					last_tx = curr_tx;
					last_rx = curr_rx;
					diag_led_ctl(PPP_ACT);
				}
				else
					diag_led_ctl(PPP_OFF);		
			}
			*/
		}	
	}
	return 0;
}

//junzhao 2004.6.15
int
redial_main(int argc, char **argv)
{
	int need_redial = 0;
	int status;
	pid_t pid;
	int which_real;
	static int firstdelay = 0;
	static int state = 3;
	static int first_has_checked = 0;

	which_real = atoi(&argv[2][3]);

	if(which_real < MAX_CHAN)
		which_conn = which_real;
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else
		which_conn = which_real % MAX_CHAN;
#endif
	printf("--------which conn %d\n", which_conn);

	//sleep(atoi(argv[1]) - 3);
	sleep(3);

	while(1)
	{
		sleep(atoi(argv[1]) - 3);
		//sleep(3);
		
		if(need_redial)
			need_redial = 0;
		
	//junzhao 2004.5.27
		if(check_ppp_link(which_real))
		{
			if(first_has_checked)
				first_has_checked = 0;
			sleep(3);
			state = 3;
			firstdelay = 0;
			continue;
		}
		
		need_redial = 1;
		if(check_ppp_authfail(which_real))
		{
			printf("state %d, firstdelay %d\n", state, firstdelay);
			if(first_has_checked)
				first_has_checked = 0;
			switch(state)
			{
				case 0:
					state++;
					break;
					/*
					if(first_has_checked)
						sleep(atoi(argv[1]) - 3);
					if(!first_has_checked)
						first_has_checked = 1;
					state++;
					break;
					*/
				case 1:
				case 2:
					sleep(3);
					state++;
					if(state > 2 && firstdelay)
						state++;
					break;
				case 3:
					sleep(300 - atoi(argv[1]) + 3);
					state = 1;
					firstdelay = 1;
					break;
				case 4:
					sleep(1800 - atoi(argv[1]) + 3);
					state = 1;
					break;
				default:
					sleep(3);
					state = 0;
					firstdelay = 0;
					break;
			}
		}
		else
		{
			printf("Why comes here???\n");
			/*
			if(!first_has_checked)
				first_has_checked = 1;
			*/
			if(first_has_checked)
				sleep(3);
			if(!first_has_checked)
				first_has_checked = 1;
			state = 3;
			firstdelay = 0;
		}
	/*
		sleep(atoi(argv[1]));

		if(!check_ppp_link(which_real))
			need_redial = 1;
		else
			continue;
	*/
		if(need_redial)
		{
			pid = fork();
			switch(pid)
			{
				case -1:
					perror("fork failed");
					exit(1);
				case 0:
					//modify by junzhao 2004.3.19
					printf("ok------redial_action start!\n");
					stop_pppox(which_real);

					//junzhao 2004.6.15
					{
						char filename[30];
						memset(filename, 0, sizeof(filename));
						sprintf(filename, "/tmp/ppp/authfail%d", which_real);
						unlink(filename);
					}

					if(which_conn == which_real)
					{
						vcc_global_init();
						if(!strcmp(encapmode, "pppoe"))
						{
							pppoe_global_init();
							start_pppoe(REDIAL, 1);
						}
						else if(!strcmp(encapmode, "pppoa"))
						{
							pppoa_global_init();
							start_pppoa(AUTODISABLE, REDIAL);
						}
						/* kirby for unip 2004/12.3 */
						#ifdef UNNUMBERED_SUPPORT
						else if(!strcmp(encapmode, "unpppoe"))
						{
							unip_pppoe_global_init();
							start_unpppoe(REDIAL, 1);
						}
						else if(!strcmp(encapmode, "unpppoa"))
						{
							unip_pppoa_global_init();
							start_unpppoa(AUTODISABLE, REDIAL);
						}
						#endif

					}
				#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
					else
					{
						start_pppoe2(REDIAL, 1);
					}
				#endif

					exit(0);
					break;
				default:
					waitpid(pid, &status, 0);
					break;
			} // end switch
		} // end if
	} // end while
} // end main

//xtt add for diagtool PPPoE&PPPoA 07-08-2004
int
diag_redial_main(int argc, char **argv)
{
	int need_redial = 1;
	int status;
	pid_t pid;
	
	which_conn = atoi(&argv[2][3]);
	printf("--------which conn %d\n", which_conn);

		printf("In diag_redial_main, need_redial=%d\n", need_redial);
		if(need_redial)
		{
			//char pidchar[10];
			//char pppfile[28];
			
			pid = fork();
			switch(pid)
			{
				case -1:
					perror("fork failed");
					exit(1);
				case 0:
					//modify by junzhao 2004.3.19
					printf("ok------redial_action start!\n");					
					stop_pppox(which_conn);
       					/*
					memset(pppfile, 0, sizeof(pppfile));
					sprintf(pppfile, "/var/run/ppp%d.pid", which_conn);
					if(file_to_buf(pppfile, pidchar, sizeof(pidchar)))
					{
						printf("ppp_pid[%d], %s\n", which_conn, pidchar);
       						eval("kill", "-9", pidchar);
					}
					//ppp_pid[which_conn] = -1;
					*/
					vcc_global_init();
					if(!strcmp(encapmode, "pppoe"))
					{
						pppoe_global_init();
						start_pppoe(REDIAL,1);
					}
					else if(!strcmp(encapmode, "pppoa"))
					{
						pppoa_global_init();
						start_pppoa(AUTODISABLE, REDIAL);
					}
					/* end for redial */

					//kill(1, SIGPROF);
					exit(0);
					break;
				default:
					printf("in diag_redial_main,parent process\n");
					waitpid(pid, &status, 0);
					//dprintf("parent\n");
					break;
			} // end switch
		} // end if
//	} // end while
} // end main
//xtt end

int
checkrate_main(int argc, char **argv)
{
	int last_tx, last_rx;
	int curr_tx, curr_rx;
	last_tx = sar_getstats("Tx Total Bytes:");
	last_rx = sar_getstats("Rx Total Bytes:");

	while(1)
	{
		sleep(atoi(argv[1]));
		
		curr_tx = sar_getstats("Tx Total Bytes:");
		curr_rx = sar_getstats("Rx Total Bytes:");
		//printf("curr_tx %d, last_tx %d\n", curr_tx, last_tx);
		//printf("curr_rx %d, last_rx %d\n", curr_rx, last_rx);
		if((curr_tx - last_tx > 0) || (curr_rx - last_rx > 0))
		{
			last_tx = curr_tx;
			last_rx = curr_rx;
			diag_led_ctl(PPP_ACT);
		}
		else
		{
			//printf("hey origin_ledstatus is %s\n", origin_ledstatus);
			diag_led_ctl(PPP_CON);
			//diag_led_ctl(&origin_ledstatus[0]);
		}
	}
	return 0;
}

int 
stop_checkadsl(void)
{
	int ret;

        ret = eval("killall","-9","checkadsl");
	unlink("/tmp/checkadsl");

	dprintf("done\n");
	return ret ;
}

//junzhao 2004.7.9
int
checkadsl_main(int argc, char **argv)
{
	int need_redial=0;
	int status;
	pid_t pid;
	int lastadslstatus = 1, curadslstatus;
	int counter = 0, downwaiting = 0, downflag = 0;
	
	while(1)
	{
		//junzhao 2004.4.16 to check busy flag
		while(1)
		{
			char tmpbuf[3];
			memset(tmpbuf, 0, sizeof(tmpbuf));
			if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
				break;
			if(!strcmp(tmpbuf, "0"))
				break;
			sleep(1);
		}
		
		if(need_redial)
			need_redial = 0;
		
		sleep(atoi(argv[1]));	
		
		//cprintf("counter %d\n", counter);
		if(downflag)
			downwaiting = (++counter<60)?1:2; //1:waiting 2:timeout
		else
			downwaiting = 0;	
		//cprintf("downwaiting %d, downflag %d\n", downwaiting, downflag);
		
		curadslstatus = check_adsl_status();
		need_redial = curadslstatus - lastadslstatus;
		lastadslstatus = curadslstatus;
		
		if(need_redial == 0 && !downwaiting)
			continue;
		
		if(need_redial > 0)
		{
			//junzhao 2004.7.28
			time_t now = time(NULL);
			char buf[32];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "%ld", (long int)now);
			nvram_set("adsluptime", buf);

			//junzhao 2004.10.11
                	syslog(LOG_INFO,"1rADSL:%s.", "SHOWTIME");
			
			downflag = 0;
			if(downwaiting)
			{
				if(!buf_to_file("/tmp/adsl_action", "pppd"))
				{
					printf("in checkadsl write %s file fail\n", "/tmp/adsl_action");
					continue;
				}
			}
			else
			{
				if(!buf_to_file("/tmp/adsl_action", "up"))
				{
					printf("in checkadsl write %s file fail\n", "/tmp/adsl_action");
					continue;
				}
			}
		}
		else if(need_redial < 0)
		{
			nvram_set("adsluptime", "0");
			counter = 0;
			downflag = 1;
			//junzhao 2004.10.11
                	syslog(LOG_INFO,"1rADSL:%s.", "IDLE");
			continue;
		}
		else //need_redial == 0
		{
			if(downwaiting == 1) //now waiting
				continue;
			
			downflag = 0;
			if(!buf_to_file("/tmp/adsl_action", "down"))
			{
				printf("in checkadsl write %s file fail\n", "/tmp/adsl_action");
				continue;
			}
		}
		
		pid = fork();
		switch(pid)
		{
			case -1:
				perror("fork failed");
				exit(1);
			case 0:
			{
				/*
				while(1)
				{
					char tmpbuf[3];
					memset(tmpbuf, 0, sizeof(tmpbuf));
					if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
						break;
					if(!strcmp(tmpbuf, "0"))
						break;
					sleep(1);
				}
				*/
				kill(1, SIGPROF);
				exit(0);
				break;
			}
			default:
				waitpid(pid, &status, 0);
				//dprintf("parent\n");
				break;
		} // end switch
		
	} // end while
} // end main
	
#if 0
int
checkadsl_main(int argc, char **argv)
{
	int need_redial=0;
	int status;
	pid_t pid;
	int lastadslstatus = 1, curadslstatus;
	
	while(1)
	{
		//junzhao 2004.4.16 to check busy flag
		while(1)
		{
			char tmpbuf[3];
			memset(tmpbuf, 0, sizeof(tmpbuf));
			if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
				break;
			if(!strcmp(tmpbuf, "0"))
				break;
			sleep(1);
		}
		
		if(need_redial)
			need_redial = 0;
		
		sleep(atoi(argv[1]));	
		
		curadslstatus = check_adsl_status();
		need_redial = curadslstatus - lastadslstatus;
		lastadslstatus = curadslstatus;
		
		if(need_redial == 0)
			continue;

		if(need_redial > 0)
		{
			if(!buf_to_file("/tmp/adsl_action", "up"))
			{
				printf("in checkadsl write %s file fail\n", "/tmp/adsl_action");
				continue;
			}
		}
		else if(need_redial < 0)
		{
			if(!buf_to_file("/tmp/adsl_action", "down"))
			{
				printf("in checkadsl write %s file fail\n", "/tmp/adsl_action");
				continue;
			}
		}
		
		pid = fork();
		switch(pid)
		{
			case -1:
				perror("fork failed");
				exit(1);
			case 0:
			{
				/*
				while(1)
				{
					char tmpbuf[3];
					memset(tmpbuf, 0, sizeof(tmpbuf));
					if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
						break;
					if(!strcmp(tmpbuf, "0"))
						break;
					sleep(1);
				}
				*/
				kill(1, SIGPROF);
				exit(0);
				break;
			}
			default:
				waitpid(pid, &status, 0);
				//dprintf("parent\n");
				break;
		} // end switch
		
	} // end while
} // end main
#endif

//by songtao
/*int start_snmp(void)
{  if(nvram_match("snmp_enable","1"))
	eval("snmpd");
   printf("snmpd starting\n");
   return;
	
}
int stop_snmp(void)
{
   if(nvram_match("snmp_enable","0"))
        eval("killall","-9","snmpd");	
    printf("stop snmpd.\n"); 
    return;  
}*/
//cjg;2004-3-12
int start_email_alert(void)
{
	eval("email_alert");
	return 0;
}

int stop_email_alert(void)
{
	eval("killall", "-9", "email_alert");
	return 0;
}


#if 0
/*zhangbin for ipsec*/
int vpn_global_init()
{
	char word[250];
	char *next;
	char tunnel_l[5];
	int which,local_connection;
	char readbuf[256];
	
	#ifdef ZB_DEBUG
		printf("Enter vpn_global_init!\n");
	#endif

	strncpy(tunnel_l,nvram_safe_get("tunnel_entry"),sizeof(tunnel_l));

	/*if(tunnel_l == NULL)
		return -1;
	*/	
				
	which = atoi(tunnel_l);
	#ifdef ZB_DEBUG
		printf("which = %d\n",which);
	#endif

	foreach(word, nvram_safe_get("entry_config"), next) 
	{
		if (which-- == 0) 
		{
		sscanf(word, "%3[^:]:%25[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", rc_tunnel_status,rc_tunnel_name,rc_local_type, rc_localgw_conn,rc_remote_type, rc_sg_type, rc_enc_type, rc_auth_type, rc_key_type);
			
		#ifdef ZB_DEBUG
			printf("rc_tunnel_status=%s, rc_tunnel_name=%s,rc_local_type=%s,rc_localgw_conn=%s,rc_remote_type=%s,rc_sg_type=%s,rc_enc_type=%s,rc_auth_type=%s,rc_key_type=%s\n", rc_tunnel_status,rc_tunnel_name,rc_local_type,rc_localgw_conn,rc_remote_type,rc_sg_type,rc_enc_type,rc_auth_type,rc_key_type);
		#endif 
			break;
		}
	}
	
	memset(rc_ltype_config_buf, 0, sizeof(rc_ltype_config_buf));
	which = atoi(tunnel_l);

	foreach(rc_ltype_config_buf, nvram_safe_get("ltype_config"), next) 
	{
		if (which-- == 0) 
		{
	//		sscanf(ltype_config_buf,"%s:%s",l_ipaddr,l_netmask);
			sscanf(rc_ltype_config_buf, "%20[^:]:%20[^\n]", rc_l_ipaddr,rc_l_netmask);
		#ifdef ZB_DEBUG
			printf("rc_l_ipaddr=%s, rc_l_netmask=%s\n", rc_l_ipaddr, rc_l_netmask);
		#endif 
			break;
		}
	}
	
	memset(rc_rtype_config_buf, 0, sizeof(rc_rtype_config_buf));
	which = atoi(tunnel_l);
	
	foreach(rc_rtype_config_buf, nvram_safe_get("rtype_config"), next)
		{
			if (which-- == 0) 
			{
				sscanf(rc_rtype_config_buf, "%20[^:]:%20[^\n]", rc_r_ipaddr,rc_r_netmask);

			#ifdef ZB_DEBUG
				printf("rc_r_ipaddr=%s, rc_r_netmask=%s\n", rc_r_ipaddr, rc_r_netmask);
			#endif 
			
			break;
			}
		}
		
	memset(rc_sg_config_buf, 0, sizeof(rc_sg_config_buf));
	which = atoi(tunnel_l);
	
	foreach(rc_sg_config_buf, nvram_safe_get("sg_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(rc_sg_config_buf, "%20[^:]:%50[^\n]", rc_sg_ipaddr,rc_sg_domain);
			
		#ifdef ZB_DEBUG
			printf("rc_sg_ipaddr=%s,sg_domain=%s\n",rc_sg_ipaddr,rc_sg_domain);
		#endif 
			break;
		}
	}
	
	memset(rc_keytype_config_buf, 0, sizeof(rc_keytype_config_buf));	
	which = atoi(tunnel_l);
	
	foreach(rc_keytype_config_buf, nvram_safe_get("keytype_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(rc_keytype_config_buf, "%3[^:]:%100[^:]:%20[^:]:%100[^:]:%100[^:]:%100[^:]:%100[^\n]", rc_auto_pfs,rc_auto_presh,rc_auto_klife,rc_manual_enckey,rc_manual_authkey,rc_manual_ispi,rc_manual_ospi);
			
		#ifdef ZB_DEBUG
			printf(" rc_auto_pfs=%s,rc_auto_presh=%s,rc_auto_klife=%s,rc_manual_enckey=%s,rc_manual_authkey=%s,rc_manual_ispi=%s,rc_manual_ospi=%s\n", rc_auto_pfs,rc_auto_presh,rc_auto_klife,rc_manual_enckey,rc_manual_authkey,rc_manual_ispi,rc_manual_ospi);
		#endif 
			break;
		}
	}
	
	memset(rc_ipsecadv_config_buf, 0, sizeof(rc_ipsecadv_config_buf));
	which = atoi(tunnel_l);
	
	foreach(rc_ipsecadv_config_buf, nvram_safe_get("ipsecadv_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(rc_ipsecadv_config_buf, "%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^:]:%3[^:]:%10[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^\n]", rc_ipsec_opmode,rc_ipsecp1_enc,rc_ipsecp1_auth,rc_ipsecp1_group,rc_ipsecp1_klife,rc_ipsecp2_group,rc_ipsecp2_klife,rc_ipsec_netbios,rc_ipsec_antireplay,rc_ipsec_keepalive,rc_ipsec_blockip,rc_ipsec_retrytimes,rc_ipsec_blocksecs);
		#ifdef ZB_DEBUG
			printf("rc_ipsec_opmode=%s,rc_ipsecp1_enc=%s,rc_ipsecp1_auth=%s,rc_ipsecp1_group=%s,rc_ipsecp1_klife=%s,rc_ipsecp2_group=%s,rc_ipsecp2_klife=%s,rc_ipsec_netbios=%s,rc_ipsec_antireplay=%s,rc_ipsec_keepalive=%s,rc_ipsec_blockip=%s,rc_ipsec_retrytimes=%s,rc_ipsec_blocksecs=%s\n", rc_ipsec_opmode,rc_ipsecp1_enc,rc_ipsecp1_auth,rc_ipsecp1_group,rc_ipsecp1_klife,rc_ipsecp2_group,rc_ipsecp2_klife,rc_ipsec_netbios,rc_ipsec_antireplay,rc_ipsec_keepalive,rc_ipsec_blockip,rc_ipsec_retrytimes,rc_ipsec_blocksecs);
		#endif 
			break;
		}
        
	}

	local_connection = atoi(rc_localgw_conn);

	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_ifname"),next)
	foreach(word, readbuf, next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(rc_wan_ifname,word);
			printf("rc_wan_ifname=%s\n",rc_wan_ifname);
			break;
		}
	}


	local_connection = atoi(rc_localgw_conn);
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_ipaddr"),next)
	foreach(word, readbuf, next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(rc_wan_ipaddress,word);
			printf("rc_wan_ipaddress=%s\n",rc_wan_ipaddress);
			break;
		}
	}
	
	local_connection = atoi(rc_localgw_conn);
	if(!file_to_buf(WAN_NETMASK, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_NETMASK);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_netmask"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(rc_wan_netmask,word);
			printf("rc_wan_netmask=%s\n",rc_wan_netmask);
			break;
		}
	}

	local_connection = atoi(rc_localgw_conn);
	if(!file_to_buf(WAN_BROADCAST, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_BROADCAST);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_broadcast"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(rc_wan_broadcast,word);
			printf("rc_wan_broadcast=%s\n",rc_wan_broadcast);
			break;
		}
	}

	local_connection = atoi(rc_localgw_conn);
	if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_GATEWAY);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_gateway"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(rc_wan_gateway,word);
			printf("rc_wan_gateway=%s\n",rc_wan_gateway);
			break;
		}
	}

	return 0;
}


int start_ipsec(void)
{
	FILE *file,*nat_fd;
	char rc_lsub[80],rc_lsub_client[50],rc_sec_gw[40],rc_any_gw[10],remote[100],remote_client[50],enc_config[8],auth_config[8],p1enc_config[10],p1auth_config[10],p1group_config[10],ike_config[30],rc_nexthop_config[20],rc_ikemode_config[15],rc_esp_config[20],manual_file[200],rc_retries[5],status_file[80];
	
	memset(rc_sec_gw,0,sizeof(rc_sec_gw));
	memset(rc_any_gw,0,sizeof(rc_any_gw));
	memset(rc_nexthop_config,0,sizeof(rc_nexthop_config));
	memset(rc_lsub,0,sizeof(rc_lsub));
	memset(rc_lsub_client,0,sizeof(rc_lsub_client));
	memset(rc_ikemode_config,0,sizeof(rc_ikemode_config));
	memset(rc_esp_config,0,sizeof(rc_esp_config));
	memset(manual_file,0,sizeof(manual_file));
	memset(rc_retries,0,sizeof(rc_retries));
	memset(status_file,0,sizeof(status_file));

		
	if(vpn_global_init() < 0)
		return -1;
#ifdef ZB_DEBUG		
	printf("Enter connect_ipsec!\n");

#endif
	if(!strcmp(rc_local_type,"0"))
	{
		sprintf(rc_lsub,"--client %s/32",rc_l_ipaddr); //for auto key
		sprintf(rc_lsub_client," %s/32",rc_l_ipaddr); //for manual key
	}
	else if(!strcmp(rc_local_type,"1"))
	{
		sprintf(rc_lsub,"--client %s/%s",rc_l_ipaddr,rc_l_netmask);
		sprintf(rc_lsub_client,"%s/%s",rc_l_ipaddr,rc_l_netmask);
	}
	
	strcpy(rc_nexthop_config,"\"%direct\"");
	if(!strcmp(rc_sg_type,"0"))
	{
		strncpy(rc_sec_gw,rc_sg_ipaddr,sizeof(rc_sec_gw));
	}
	else if(!strcmp(rc_sg_type,"1")) //security gateway is "any"
	{
		strncpy(rc_sec_gw,"\"%any\"",sizeof(rc_sec_gw));
		strcpy(rc_any_gw,"%any");
		//strcpy(rc_nexthop_config,"--nexthop \"%direct\"");
	}
	else if(!strcmp(rc_sg_type,"2"))
	{
#if 0
		if(DNSresolve(rc_sg_domain,nvram_safe_get("pdns")) == 0)
		{
			if(!strcmp(ipaddress,"0.0.0.0"))
			{
				printf("domain %s cannot be transformed to ipaddress!\n",rc_sg_domain);	
				return -1;
			}				
			strncpy(rc_sec_gw,ipaddress,sizeof(rc_sec_gw));
		}
#endif
		char temp[50];
		struct hostent *gw_host;
		
		gw_host = gethostbyname(rc_sg_domain);

		if(NULL == gw_host)
		{
			printf("Cannot resolve domain: %s\n",rc_sg_domain);
			return -1;
		}
	
		sprintf(temp, "%d.%d.%d.%d", 
			(unsigned char)gw_host->h_addr_list[0][0],
			(unsigned char)gw_host->h_addr_list[0][1],
			(unsigned char)gw_host->h_addr_list[0][2],
			(unsigned char)gw_host->h_addr_list[0][3]
			);
		strncpy(rc_sec_gw,temp,sizeof(rc_sec_gw));
	}	
		
	if(!strcmp(rc_remote_type,"0"))  //remote secure group is Ip addr.
	{
		sprintf(remote_client,"%s/32",rc_r_ipaddr);
		sprintf(remote,"--client %s/32",rc_r_ipaddr);
	}
			
	if(!strcmp(rc_remote_type,"1")) //remote secure group is subnet.
	{
		sprintf(remote_client,"%s/%s",rc_r_ipaddr,rc_r_netmask);
		sprintf(remote,"--client %s/%s",rc_r_ipaddr,rc_r_netmask);
	}	
	
	if(!strcmp(rc_remote_type,"4")) //remote secure group is host.
	{
		sprintf(remote_client,"%s ",rc_sec_gw);
		strcpy(remote,"");
	}	
	
	if(!strcmp(rc_ipsec_opmode,"0")) //remote secure group is host.
	{
		sprintf(rc_ikemode_config,"%s","");
	}	
	else if(!strcmp(rc_ipsec_opmode,"1"))
		sprintf(rc_ikemode_config,"%s","--aggrmode");
	
	if(!strcmp(rc_remote_type,"3")) //remote secure group is any.
	{
		sprintf(remote_client,"0.0.0.0/0.0.0.0");
		sprintf(remote,"--client 0.0.0.0/0.0.0.0");
		
	}

	if((nat_fd = fopen("/tmp/deconf_file","w")) == NULL)
	{
		printf("cannot open /tmp/deconf_file!\n");
		return -1;
	}
	
	sprintf(status_file,"/tmp/ipsecst/%s",rc_tunnel_name);
	strcpy(del_status,status_file);
	eval("rm",status_file);//delete statues file

	/* first,we should del the rule which would let the packets go around nat'ed */	
	fprintf(nat_fd,"#!/bin/sh\n");
	fprintf(nat_fd,"iptables -t nat -D POSTROUTING -s %s -d %s -j ACCEPT\n",rc_lsub_client,remote_client);
	fprintf(nat_fd,"iptables -D FORWARD -i ipsec0 -j ACCEPT\n");
	fprintf(nat_fd,"iptables -D INPUT -p 50 -j ACCEPT\n");
	fprintf(nat_fd,"iptables -D OUTPUT -p 50 -j ACCEPT\n");
	fprintf(nat_fd,"iptables -D INPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(nat_fd,"iptables -D OUTPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");

	fprintf(nat_fd,"echo 1 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",rc_wan_ifname);
	fclose(nat_fd);
	eval("/bin/sh","/tmp/deconf_file");


	if(!strcmp(rc_tunnel_status,"0"))
	{
		printf("Tunnel Status is Disabled,no need to start ipsec!\n");
		return -1;
	}

#ifdef ZB_DEBUG
		printf("start ipsec()!\n");
	#endif

	if((file = fopen(IPSEC_START_FILE,"w")) == NULL)
	{
		printf("Cannot open file /tmp/ipsec!\n");
		return -1;
	}
	
	fprintf(file,"#!/bin/sh\n");
	fprintf(file,"/bin/ipsec/klipsdebug --none\n");
	fprintf(file,"/bin/ipsec/eroute --clear\n");
	fprintf(file,"/bin/ipsec/spi --clear\n");
	fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",rc_wan_ifname);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"/bin/ipsec/tncfg --attach --virtual ipsec0 --physical %s\n",rc_wan_ifname);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"ifconfig ipsec0 inet %s broadcast %s netmask %s\n",rc_wan_ipaddress,rc_wan_broadcast,rc_wan_netmask);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"ifconfig ipsec0 mtu 1430\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/ipsec0/rp_filter\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I FORWARD -i ipsec0 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I INPUT -p 50 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I OUTPUT -p 50 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I INPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I OUTPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(file,"echo\n");

	fprintf(file,"iptables -t nat -I POSTROUTING -s %s -d %s -j ACCEPT\n",rc_lsub_client,remote_client);
	fprintf(file,"echo\n");
	fprintf(file,"/bin/ipsec/pluto & \n");	
	fclose(file);
	
	printf("Starting IPSEC...........\n");
	eval("/bin/sh",IPSEC_START_FILE);

	if(!strcmp(rc_enc_type,"1"))
	{
		strcpy(enc_config,"des"); 
	}
	else if(!strcmp(rc_enc_type,"2"))
	{
		strcpy(enc_config,"3des"); 
	}
	else if(!strcmp(rc_enc_type,"3"))
	{
		strcpy(enc_config,"aes"); 
	}
	else if(!strcmp(rc_enc_type,"0"))
	{
		strcpy(enc_config,"null"); 
	}
	
	if(!strcmp(rc_auth_type,"1"))
	{
		strcpy(auth_config,"md5"); 
	}
	else if(!strcmp(rc_auth_type,"2"))
	{
		strcpy(auth_config,"sha1"); 
	}

	sprintf(rc_esp_config,"%s-%s-96",enc_config,auth_config);
	
	if(!strcmp(rc_ipsecp1_enc,"1"))
	{
		strcpy(p1enc_config,"des"); 
	}
	else 
	{
		strcpy(p1enc_config,"3des"); 
	}
	
	if(!strcmp(rc_ipsecp1_auth,"1"))
	{
		strcpy(p1auth_config,"md5"); 
	}
	else 
	{
		strcpy(p1auth_config,"sha"); 
	}
	
	if(!strcmp(rc_ipsecp1_group,"1"))
	{
		strcpy(p1group_config,"modp768"); 
	}
	else 
	{
		strcpy(p1group_config,"modp1024"); 
	}
	
	sprintf(ike_config,"%s-%s-%s",p1enc_config,p1auth_config,p1group_config);
	if(!strcmp(rc_ipsec_blockip,"1"))
		sprintf(rc_retries,"%s",rc_ipsec_retrytimes);
	else
		sprintf(rc_retries,"%s","8");
	
	//printf("rc_sec_gw=%s,rc_any_gw=%s\n,rc_nexthop_config=%s,rc_lsub=%s,rc_lsub_client=%s\n",rc_sec_gw,rc_any_gw,rc_nexthop_config,rc_lsub,rc_lsub_client);
	printf("rc_key_type=%s\n",rc_key_type);
	if(!strcmp(rc_key_type,"0"))
	{
		FILE *afd,*prefd,*psk_fd; //auto key fd and preshared key fd
		char pfs_config[10];
		char state_file[30],pskbuf[512],ip1[20],ip2[20],psk[100];
		int find_it = 0;
			
		sprintf(state_file,"/tmp/ipsecst/%s",rc_tunnel_name);
		eval("rm",state_file);//delete statues file
			
		if(!strcmp(rc_auto_pfs,"1"))
			strcpy(pfs_config,"--pfs");
		else
			strcpy(pfs_config,"");
		#ifdef ZB_DEBUG
			printf("open preshared key file!\n");
		#endif

		if((psk_fd = fopen(PRESHARED_FILE,"r")) == NULL)
		{
			printf("file ipsec.secrets has not been created!\n");
			goto write_psk;
		}
		memset(pskbuf,0,sizeof(pskbuf));
		printf("fgets!\n");
		while(fgets(pskbuf,512,psk_fd))
		{
			sscanf(pskbuf,"%s %s : PSK %s",ip1,ip2,psk);
			printf("ip1=%s,ip2=%s\n",ip1,ip2);
			printf("rc_sec_gw=%s,rc_any_gw=%s\n",rc_sec_gw,rc_any_gw);
			if((!strcmp(ip2,rc_sec_gw) || !strcmp(ip2,rc_any_gw)) && !strcmp(ip1,rc_wan_ipaddress))
			{
				find_it = 1;
				break;
			}
		}
		fclose(psk_fd);
			
		if(find_it == 1)
		{
			printf("item is found in file!\n");
		}
		else
		{
write_psk:
			printf("Will add item!\n");

			if((prefd = fopen(PRESHARED_FILE,"a+")) == NULL)
			{
				printf("cannot open ipsec.secrets!\n");
				return -1;
			}
			if(!strcmp(rc_sg_type,"1"))
			{	
				fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_any_gw,rc_auto_presh);
			}
			else
				fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_sec_gw,rc_auto_presh);

			fclose(prefd);
		}
#if 0
		if((prefd = fopen(PRESHARED_FILE,"a+")) == NULL)
		{
			printf("cannot open ipsec.secrets!\n");
			return -1;
		}
		if(!strcmp(rc_sg_type,"1"))
		{	
			fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_any_gw,rc_auto_presh);
		}
		else
			fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_sec_gw,rc_auto_presh);

		fclose(prefd);
#endif			
		if((afd = fopen(AUTOKEY_FILE,"w")) == NULL)
		{
			printf("cannot open auto key file!\n");
			return -1;
		}
				
		fprintf(afd,"#!/bin/sh\n");
		fprintf(afd,"/bin/ipsec/whack --name %s --encrypt --tunnel --esp %s --ike %s %s --psk --host %s %s --nexthop %s --to --host %s %s --nexthop %s %s --ikelifetime %s --ipseclifetime %s --rekeywindow 540 --keyingtries %s\n",rc_tunnel_name,rc_esp_config,ike_config,pfs_config,rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rc_sec_gw,remote,rc_nexthop_config,rc_ikemode_config,rc_ipsecp1_klife,rc_auto_klife,rc_retries);
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"/bin/ipsec/whack --listen & \n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		fprintf(afd,"echo\n");
		
		if(!strcmp(rc_remote_type,"4")  || !strcmp(rc_remote_type,"3") || !strcmp(rc_sg_type,"1"))
		{
			fprintf(afd,"echo\n");
		}
		else
		{
			printf("in rc:remote_type=%s,sg_type=%s\n",rc_remote_type,rc_sg_type);
			fprintf(afd,"/bin/ipsec/whack --name %s --initiate &\n",rc_tunnel_name);
		}
#if 0
			if(strcmp(rc_remote_type,"4")  || strcmp(rc_remote_type,"3") || strcmp(rc_sg_type,"1"))
			{
				printf("in rc:remote_type=%s,sg_type=%s\n",rc_remote_type,rc_sg_type);
				fprintf(afd,"/bin/ipsec/whack --name %s --initiate &\n",rc_tunnel_name);
			}
#endif
		fclose(afd);
			
		#ifdef ZB_DEBUG
			printf("Start Auto-keying!\n");
		#endif
		if(!strcmp(rc_sg_type,"1"))
		{
			eval("/bin/sh",AUTOKEY_FILE);
			eval("/bin/sh",AUTOKEY_FILE);
		}

		#ifdef ZB_DEBUG
			printf("ike_config=%s\n,ipsecp1_enc=%s\n,ipsecp2_auth=%s\n",ike_config,rc_ipsecp1_enc,rc_ipsecp1_auth);
		#endif
					
	}
	else if(!strcmp(rc_key_type,"1"))
	{
		FILE *mfd,*it_fd;   //manual key fd
		char tun_in[30],tun_out[30],esp_in[30],esp_out[30],manualenc_hex[100],manualauth_hex[100],iptables_config[200];
	
		#ifdef ZB_DEBUG
			printf("enter manual keying!\n");
		#endif	

		if((it_fd = fopen("/tmp/manual_it","w")) == NULL)
		{
			printf("Open /tmp/manual_it failed!\n");
			return -1;
		}
		
		fprintf(it_fd,"#!/bin/sh\n");	
		//fprintf(it_fd,"iptables -t nat -I POSTROUTING -s %s -d %s -j ACCEPT\n",rc_lsub_client,remote_client);
		fclose(it_fd);
		eval("/bin/sh","/tmp/manual_it");
		
		//inbound tun,esp and outbound tun,esp
		sprintf(tun_in,"tun0x%s@%s",rc_manual_ispi,rc_wan_ipaddress);
		sprintf(tun_out,"tun0x%s@%s",rc_manual_ospi,rc_sec_gw);
		
		sprintf(esp_in,"esp0x%s@%s",rc_manual_ispi,rc_wan_ipaddress);
		sprintf(esp_out,"esp0x%s@%s",rc_manual_ospi,rc_sec_gw);
		printf("ENTER handling rc_manual_enckey!\n");
#if 1	
		if((strncmp(rc_manual_enckey, "0x", 2) == 0) || (strncmp(rc_manual_enckey, "0X", 2) == 0))
		{
			strcpy((char*)manualenc_hex, rc_manual_enckey);
			printf("has 0x!\n");

		}
		else
		{
			int j = 0;
			char temp1[3];
			memset( manualenc_hex, 0, sizeof(manualenc_hex) );
			sprintf(manualenc_hex,"%s","0x");
			printf("manualenc_hex=%s\n",manualenc_hex);
			printf("strlen=%d,j=%d\n",strlen(rc_manual_enckey),j);
			//for(j = 0;j < strlen(rc_manual_enckey);j++)
			for(;rc_manual_enckey[j];j++)
			{
				//printf("rc_manual_enckey[%d]=%s\n",j,rc_manual_enckey[j]);
				sprintf(temp1,"%02x",rc_manual_enckey[j]);
			//	printf("temp1=%s\n",temp1);
				strcat(manualenc_hex,temp1);
			//	printf("manualenc_hex=%s\n",manualenc_hex);
			}
			//	printf("out:manualenc_hex=%s\n",manualenc_hex);

		}
		if((strncmp(rc_manual_authkey, "0x", 2) == 0) || (strncmp(rc_manual_authkey, "0X", 2) == 0))
		{
			strcpy((char*)manualauth_hex, rc_manual_authkey);
		}
		else
		{
			int i;
			char temp1[3];
			memset( manualauth_hex, 0, 100 );
			sprintf(manualauth_hex,"%s","0x");
			for(i = 0;i < strlen(rc_manual_authkey) ;i++)
			{
				sprintf(temp1,"%02x",rc_manual_authkey[i]);
				strcat(manualauth_hex,temp1);
			}
		}
#endif		
	
		sprintf(manual_file,"/tmp/rc_manual_%s",rc_tunnel_name); 	
		if((mfd = fopen(MANUALKEY_FILE,"w")) == NULL)
//		if((mfd = fopen(manual_file,"w")) == NULL)
		{
			printf("cannot open manual key file!\n");
			return -1;
		}
		
		fprintf(mfd,"#!/bin/sh\n\n");
		fprintf(mfd,"echo \"first out spi\"\n");
		fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",rc_tunnel_name,tun_out,rc_wan_ipaddress,rc_sec_gw);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo \" second out spi\"\n");
		fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",rc_tunnel_name,esp_out,rc_esp_config,rc_wan_ipaddress,manualauth_hex,manualenc_hex);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo \"first out spigrp\"\n");
		fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",rc_tunnel_name,tun_out,esp_out);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo \"first in spi\"\n");
		fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",rc_tunnel_name,tun_in,rc_sec_gw,rc_wan_ipaddress);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo \"second in spi\"\n");
		fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",rc_tunnel_name,esp_in,rc_esp_config,rc_sec_gw,manualauth_hex,manualenc_hex);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo \"first in spigrp\"\n");
		fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",rc_tunnel_name,tun_in,esp_in);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"/bin/ipsec/eroute --label \"%s\" --eraf inet --replace --src %s --dst %s --said %s\n",rc_tunnel_name,rc_lsub_client,remote_client,tun_out); 
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		/* the same as "prepare-host ipsec _updown" */
		fprintf(mfd,"route del -net %s netmask %s 2>&1 \n",rc_r_ipaddr,rc_r_netmask);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		/* the same as "route-host ipsec _updown" */
		fprintf(mfd,"route add -net %s netmask %s dev ipsec0 gw %s \n",rc_r_ipaddr,rc_r_netmask,rc_wan_gateway);
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fprintf(mfd,"echo\n");
		fclose(mfd);
			
		#ifdef ZB_DEBUG
			printf("Connecting manual key!\n");
		#endif
			
		eval("/bin/sh",MANUALKEY_FILE);
		//eval("/bin/sh",manual_file);
	
	}
	return 0;
}

int stop_ipsec(void)
{
	int ret;
	FILE *fp;
			
	ret = eval("killall","-9","pluto");
	ret += eval("rm","/var/run/pluto.pid");
	ret += eval("ifconfig","ipsec0","down");
	if((fp = fopen("/tmp/detach","w")) == NULL)
	{
		printf("Can't open /tmp/detach!\n");
		return -1;
	}
	fprintf(fp,"#!/bin/sh\n");
	fprintf(fp,"/bin/ipsec/tncfg --detach --virtual ipsec0\n");
	fprintf(fp,"echo\n");
	fprintf(fp,"echo\n");
	fprintf(fp,"rm /tmp/ipsecst/* \n");
	fprintf(fp,"echo\n");
	fprintf(fp,"echo\n");
	fclose(fp);
	ret += eval("/bin/sh","/tmp/detach");
	
	return ret;
}
#endif
