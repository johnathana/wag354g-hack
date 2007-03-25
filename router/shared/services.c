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
 * $Id: services.c,v 1.1.1.3 2005/08/25 06:27:59 sparq Exp $
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

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <rc.h>
#include "pvc.h"
#include "ipsec_rc.h"
#include "ledapp.h"

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

int ppp_ever_used[MAX_CHAN];

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

//extern char origin_ledstatus[5];

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


int
start_dhcpd(void)
{
	struct dns_lists *dns_list = NULL;
        FILE *fp;

	if (nvram_match("router_disable", "1") || nvram_invmatch("lan_proto", "dhcp"))
		return 0;

	/* Automatically adjust DHCP Start IP and num when LAN IP or netmask is changed */
	adjust_dhcp_range();

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
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "auto_time 30\n");		// N seconds to update lease table
	fprintf(fp, "lease_file /tmp/udhcpd.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));
	if (nvram_invmatch("wan_wins", "") && nvram_invmatch("wan_wins", "0.0.0.0"))
		fprintf(fp, "option wins %s\n", nvram_get("wan_wins"));

	//junzhao
	dns_list = get_dns_list(which_conn);
	if(!dns_list || dns_list->num_servers == 0){
		fprintf(fp, "option lease %s\n", "300");	// no dns, lease time default to 300 seconds
                fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	}
	else{
		fprintf(fp, "option lease %d\n", atoi(nvram_safe_get("dhcp_lease")) ? atoi(nvram_safe_get("dhcp_lease"))*60 : 86400);
		fprintf(fp, "option dns %s %s %s\n", dns_list->dns_server[0], dns_list->dns_server[1], dns_list->dns_server[2]);
	}
	if (nvram_invmatch("wan_domain", ""))
		fprintf(fp, "option domain %s\n", nvram_get("wan_domain"));
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
	ret = eval("killall", "udhcpd");

	dprintf("done\n");
	return ret;
}

int
start_dns(void)
{
	int ret;
	
	ret = eval("dnsmasq",
		   "-h",
		   "-i", nvram_safe_get("lan_ifname"),
		   "-r", "/tmp/resolv.conf");

	dprintf("done\n");
	return ret;
}	

int
stop_dns(void)
{
	int ret = eval("killall", "dnsmasq");

	dprintf("done\n");
	return ret;
}	

int
start_httpd(void)
{
	int ret;

	chdir("/www");
	ret = eval("httpd");
	chdir("/");
/*	
	chdir("/www");
	ret = eval("httpd", "-S");
	chdir("/");
*/
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
#if 0
int
start_upnp(void)
{
	char *wan_ifname = nvram_match("wan_proto", "pppoe") ? "ppp0" : nvram_safe_get("wan_ifname");
	int ret;

	if (!nvram_invmatch("upnp_enable", "0"))
		return 0;

	ret = eval("killall", "-SIGUSR1", "upnp");
	if (ret != 0) {
	    ret = eval("upnp", "-D",
		       "-L", nvram_safe_get("lan_ifname"),
		       "-W", wan_ifname);

	}
	dprintf("done\n");
	return ret;
}

int
stop_upnp(void)
{
	int ret = 0;

	if (nvram_match("upnp_enable", "0"))
	    ret = eval("killall", "upnp");

	dprintf("done\n");
	return ret;
}
#else
#if 1 
int start_upnp(void)
{
	char wan_ifname[10];
	int i = 0;
	char buf[128];
//	char *pt;
	char pt[60];
	FILE *fp;
	if (nvram_match("nat_enable", "0"))
	{
		eval("killall", "upnpd-igd");
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

//	pt = nvram_safe_get("wan_ifname");
	i = 0;
	while(*(pt + i) != ' ')
	{
		wan_ifname[i] = *(pt + i);
		i ++;
	}
	wan_ifname[i] = '\0';
	if (!strcmp(wan_ifname, "Down")) strcpy(wan_ifname, "nas0");
	
	i = eval("killall", "upnpd-igd");
	if (i != 0) {
		if (!(fp = fopen("/tmp/portmap.conf", "w"))) {
			printf("can not create /tmp/portmap.conf\n");
			return errno;
		}
		if (strlen(nvram_safe_get("upnp_content")) < 20)
			fprintf(fp, "80:TCP:80:10.38.0.5:0:Web:0~21:TCP:21:10.128.32.12:0:Ftp:0~22:TCP:22:10.128.32.12:0:Ftp:0~23:TCP:23:10.128.32.112:0:Telnet:0");
		else 
		{
			fprintf(fp, "%s", nvram_safe_get("upnp_content"));
			//printf("upnp_content = %s\n", nvram_safe_get("upnp_content"));
		}
		fclose(fp);
		sprintf(buf,"/usr/sbin/upnpd-igd %s %s /tmp/portmap.conf &", wan_ifname, nvram_safe_get("lan_ifname"));
		system(buf);
	}
	cprintf("done\n");
	return i;
}
#endif
//modifed by leijun
#if 0
int
start_upnp(void)
{
	char wan_ifname[128];
	char buf[128];
	char *pt;
	int ret = 0;
	strcpy(wan_ifname, nvram_safe_get("wan_ifname"));
	strsep(wan_ifname, " ");
	if(!strcmp(wan_ifname, "noup")) strcpy(wan_ifname, "nas0");
	if (nvram_match("upnp_enable", "0"))
	{
		return 0;
	}
	ret = eval("killall", "-SIGUSR1", "upnpd-igd");
	if (ret != 0) {
		sprintf(buf,"/usr/sbin/upnpd-igd %s %s &", wan_ifname, "br0");
		//eval(buf);
		system(buf);
	}
	cprintf("done\n");
	return ret;
}
#endif
int
stop_upnp(void)
{
	int ret = 0;

	//if (nvram_match("upnp_enable", "0"))
	ret = eval("killall", "upnpd-igd");

	dprintf("done\n");
	return ret;
}
//////////end

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
	char *servers = nvram_safe_get("ntp_server");
	
	if (strlen(servers)) {
		char *nas_argv[] = {"ntpclient", "-h", servers, "-i", "3", "-l", "-s", NULL};
		pid_t pid;

		_eval(nas_argv, NULL, 0, &pid);
	}
	
	dprintf("done\n");
	return 0;
}

int
stop_ntpc(void)
{
	int ret = eval("killall", "ntpclient");

	dprintf("done\n");
	return ret;
}

int
start_services(void)
{
	start_syslog();
	start_tftpd();
	start_cron();
	start_httpd();
	start_dns();
	start_dhcpd();
//	start_upnp(); //modified by leijun
	start_snmp();//by songtao
//wwzh
//	start_nas("lan");

	dprintf("done\n");
	return 0;
}

int
stop_services(void)
{
//wwzh
//	stop_nas();
	stop_dhcpd();
	stop_dns();
	//stop_httpd();	// Don't kill httpd when user press Apply.
	stop_cron();
	stop_tftpd();
	stop_syslog();
	stop_upnp();
        stop_snmp();//by songtao
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
//written by leijun
int start_zebra(void)
{
	int rip_tx, rip_rx, i=0;
	FILE *fp;
	int ret1, ret2;
	char lan_ifname[10];
	char pt[60], tx_ver[5], rx_ver[5];
	char wan_ifname[10];
	
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

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/zebra.conf", "w"))) {
		perror("/tmp/zebra.conf");
		return errno;
	}
	fclose(fp);
	
	if (!(fp = fopen("/tmp/ripd.conf", "w"))) {
		perror("/tmp/ripd.conf");
		return errno;
	}

	if(!file_to_buf(WAN_IFNAME, pt, sizeof(pt)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return 0;
	}
	//pt = nvram_safe_get("wan_ifname");
	while(*(pt + i) != ' ')
	{
		wan_ifname[i] = *(pt + i);
		i ++;
	}
	wan_ifname[i] = '\0';
	if (!strcmp(wan_ifname, "Down")) strcpy(wan_ifname, "nas0");

	fprintf(fp, "router rip\n");
	fprintf(fp, "  network %s\n", lan_ifname);
	fprintf(fp, "  network %s\n", wan_ifname);
	fprintf(fp, "redistribute connected\n");

	fprintf(fp, "interface %s\n", lan_ifname);
	if (rip_tx != 0)
	{
		fprintf(fp, "  ip rip send version %s\n", tx_ver);
	}
	if (rip_rx != 0)
		fprintf(fp, "  ip rip receive version %s\n", rx_ver);
	
	fprintf(fp, "interface %s\n", wan_ifname);
	if (rip_tx != 0)
		fprintf(fp, "  ip rip send version %s\n", tx_ver);
	if (rip_rx != 0)
		fprintf(fp, "  ip rip receive version %s\n", rx_ver);
		
	fprintf(fp, "router rip\n");
	if (rip_tx == 0)
	{
		fprintf(fp, "  distribute-list private out %s\n",lan_ifname);
		fprintf(fp, "  distribute-list private out %s\n",wan_ifname);
	}
	if (rip_rx == 0)
	{
		fprintf(fp, "  distribute-list private in  %s\n",lan_ifname);
		fprintf(fp, "  distribute-list private in  %s\n",wan_ifname);
	}
	fprintf(fp, "access-list private deny any\n");

	fflush(fp);
	fclose(fp);
	
	ret1 = eval("zebra", "-d", "-f", "/tmp/zebra.conf");
	ret2 = eval("ripd",  "-d", "-f", "/tmp/ripd.conf");
	dprintf("zebra done\n");

	return ret1 | ret2 ;
}

/* Written by Sparq in 2002/07/16 */
int stop_zebra(void)
{
	int  ret1, ret2;

//	printf("Stop zebra !\n");

	ret1 = eval("killall", "zebra");
	ret2 = eval("killall", "ripd");

//	printf("Stop RET=%d, %d\n",ret1,ret2);
//	printf("Stop zebra done!\n");

	dprintf("done\n");
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
	char *redial_argv[] = { "/tmp/ppp/checkadsl",
				second,
			      NULL
	};
	
	symlink("/sbin/rc", "/tmp/ppp/checkadsl");

	ret = _eval(redial_argv, NULL, 0, &pid); 

	dprintf("done\n");
	return ret;
}

int
start_redial(char *second, char *devname)
{
	int ret;
	pid_t pid;
	char *redial_argv[] = { "/tmp/ppp/redial",
				second,
				devname,
			      NULL
	};
	
	printf("enter start_redial %s  %s\n", second, devname);
	symlink("/sbin/rc", "/tmp/ppp/redial");

	ret = _eval(redial_argv, NULL, 0, &pid); 

	dprintf("done\n");
	return ret;
}

int 
stop_redial(void)
{
	int ret;

        ret = eval("killall","-9","redial");

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
				              
//junzhao
int
stop_pppox(int whichconn)
{
#if 1
        int ret;
	if(ppp_pid[whichconn] < 0)
		return 0;
	else
	{
		char pidchar[18];
       		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "%d", ppp_pid[whichconn]);
		printf("ppp_pid[%d], %s\n", whichconn, pidchar);
       		ret = eval("kill", "-15", pidchar);
        	//ret += eval("killall", "ip-up");
        	//ret += eval("killall", "ip-down");
		
		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "/tmp/ppp/link%d", whichconn);
		unlink(pidchar);

		ppp_pid[whichconn] = -1;
        	dprintf("done\n");
        	return ret ;
	}
#endif
}

int
stop_dhcpc(int whichconn)
{
	
	if(udhcpc_pid[whichconn] < 0)
		return 0;
	else
	{
		char pidstr[6];
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
		
		eval("kill", "-9", pidstr);
		
		unlink(pidfile);
		
		/*
		char pidchar[16];
        	memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "%d", udhcpc_pid[whichconn]);
		printf("dhcpc_pid[%d], %s", whichconn, pidchar);
       		ret = eval("kill", "-9", pidchar);
		*/
		//ret += eval("killall","udhcpc");

		dprintf("done\n");
		udhcpc_pid[whichconn] = -1;
		return 0;
	}
}

int
start_single_service(void)
{
	char *service;
	char redialing[3];
	char redial_num[3];
	char ppp_action[8];	

/* for check adsl */
	if(!file_to_buf("/tmp/ppp/ppp_action", ppp_action, sizeof(ppp_action)))
	{
		printf("read file %s fail\n", "/tmp/ppp/ppp_action");
		goto real_redial;
	}
	printf("ok------ppp_action start!\n");
	if(!strcmp(ppp_action, "down"))
	{
		int i;
		char pidchar[18];
		stop_ntp();
		stop_ddns();
		stop_zebra();
		stop_upnp();
		eval("killall", "pppd");	
		eval("killall", "redial");	
		//eval("killall", "ip-up");	
		//eval("killall", "ip-down");
		for(i=0; i<MAX_CHAN; i++)
		{
			if(ppp_pid[i] > 0)
			{
       				memset(pidchar, 0, sizeof(pidchar));
				sprintf(pidchar, "/tmp/ppp/link%d", which_conn);
				unlink(pidchar);
				ppp_ever_used[i] = 1;
				ppp_pid[i] = -1;
			}
		}
		
	}
	else if(!strcmp(ppp_action, "up"))
	{
		int i;
		
		eval("killall", "checkadsl");
		for(i=0; i<MAX_CHAN; i++)
		{
			if(ppp_ever_used[i] > 0)
			{
				ppp_ever_used[i] = 0;
				which_conn = i;
				
				if(vcc_global_init() < 0)
					return -1;
		
				if(!strcmp(encapmode, "pppoe"))
				{
					pppoe_global_init();
					start_pppoe(BOOT);
				}
				else if(!strcmp(encapmode, "pppoa"))
				{
					pppoa_global_init();
					start_pppoa(AUTODISABLE, BOOT);
				}
			}
		}	
	}	
	unlink("/tmp/ppp/ppp_action");	
	return 0;
/* end for check adsl */

/* for redial */
real_redial:
	if(!file_to_buf("/tmp/ppp/redialing", redialing, sizeof(redialing)))
	{
		printf("read file %s fail\n", "/tmp/ppp/redialing");
		goto real_start;
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

real_start:
	service = nvram_get("action_service");

//wwzh

	if(!service)
		kill(1, SIGHUP);

	cprintf("Restart service=[%s]\n",service);

//add by lijunzhao  guohong 
	if(!strcmp(service, "connect_static"))
	{
		//char pidchar[18];
		int which_real;
		which_conn = atoi(nvram_safe_get("wan_post_connection"));
		which_real = which_conn;
		
		sleep(2);
		
		if(vcc_global_init() < 0)
			goto out;

		if(!strcmp(encapmode, "1483bridged"))
		{
			if(bridged_global_init() < 0)
				goto out;
			if(!strcmp(dhcpenable, "0"))
				start_static(BRIDGED);
			else
				goto out;
		}
		else if(!strcmp(encapmode, "routed"))
		{
			if(routed_global_init() < 0)
				goto out;
			start_static(ROUTED);
		}
		else if(!strcmp(encapmode, "bridgedonly"))
		{
			process_bridgedonly();
		}
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
		{
			if(clip_global_init() < 0)
				goto out;
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
	else if(!strcmp(service, "connect_ppp"))
	{
		char pidchar[18];
		which_conn = atoi(nvram_safe_get("wan_connection"));
		
		//stop_pppox(which_conn);
		stop_ntp();
		stop_ddns();
		stop_zebra();
		stop_upnp();
		stop_redial();
		
       		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "%d", ppp_pid[which_conn]);
		printf("ppp_pid[%d], %s\n", which_conn, pidchar);
       		eval("kill", "-9", pidchar);
		memset(pidchar, 0, sizeof(pidchar));
		sprintf(pidchar, "/tmp/ppp/link%d", which_conn);
		unlink(pidchar);
		ppp_pid[which_conn] = -1;
		
		sleep(2);
		
		if(vcc_global_init() < 0)
			return -1;
		
		nvram_set("action_service", "start_pppoe");
		if(!strcmp(encapmode, "pppoe"))
		{
			pppoe_global_init();
			start_pppoe(BOOT);
		}
		else if(!strcmp(encapmode, "pppoa"))
		{
			pppoa_global_init();
			start_pppoa(AUTODISABLE, BOOT);
		}
		
#if 0	
		int s;
		struct ifreq ifr;
		char word[32], *next, *gateway = NULL;
		char devname[8];
		int which = atoi(nvram_safe_get("wan_connection"));
		
		memset(devname, 0, sizeof(devname));
		sprintf(devname, "ppp%d", which);
		
		s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
		if(s < 0)
			return -1;
		
		strncpy(ifr.ifr_name, devname, IFNAMSIZ);
		if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
		{
			close(s);
			return -1;
		}
		ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
		if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
		{
			close(s);
			return -1;
		}
		close(s);
		
		foreach(word, nvram_safe_get("wan_gateway"), next)
		{
			if(which -- == 0)
			{
				gateway = word;
				break;
			}
		}	
		printf("connect ppp: %d--%s\n", which, gateway);
				
		eval("ping", "-c", "1", gateway);
#endif
		//return 0;
	}
	else if(!strcmp(service, "disconnect_ppp"))
	{
		char pidstr[5];
		int which = atoi(nvram_safe_get("wan_connection"));
		
		/*
		stop_ntp();
		stop_ddns();
		stop_zebra();
		*/
		memset(pidstr, 0, sizeof(pidstr));
		sprintf(pidstr, "%d", ppp_pid[which]); 
		printf("disconnect ppp: %d--%s\n", which, pidstr);
		
		//eval("kill", "-SIGUSR1", pidstr);
		eval("kill", "-15", pidstr);
		stop_redial();
		
		//return 0;
	}
	else if(!strcmp(service, "dhcp_release"))
	{
		char pidstr[6];
		char pidfile[32];
		FILE *fp; 
		char *tmp = NULL;
		int which = atoi(nvram_safe_get("wan_connection"));
			
		memset(pidfile, 0, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", which);
		
		fp = fopen(pidfile, "r");
		if(fp == NULL)
		{
			printf("no udhcpc work\n");
			return 0;
		}
		
		memset(pidstr, 0, sizeof(pidstr));
		if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
			udhcpc_pid[which] = atoi(pidstr);
		
		if((tmp = strstr(pidstr, "\n")) != NULL)
			*tmp = '\0';
		
		fclose(fp);
		
		printf("release pidstr %s\n", pidstr);
		
		eval("kill", "-SIGUSR2", pidstr);
		//return 0;
	}
	else if(!strcmp(service, "dhcp_renew"))
	{
		char pidstr[6];
		char pidfile[32];
		FILE *fp; 
		char *tmp = NULL;
		int which = atoi(nvram_safe_get("wan_connection"));
			
		memset(pidfile, 0, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", which);
		fp = fopen(pidfile, "r");
		if(fp == NULL)
		{
			printf("no udhcpc work\n");
			return 0;
		}
		
		memset(pidstr, 0, sizeof(pidstr));
		if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
			udhcpc_pid[which] = atoi(pidstr);
		
		if((tmp = strstr(pidstr, "\n")) != NULL)
			*tmp = '\0';
		
		fclose(fp);
		
		printf("renew pidstr %s\n", pidstr);
		
		eval("kill", "-SIGUSR1", pidstr);
		//return 0;
	}
	else if(!strcmp(service, "clone_mac"))
	{
		char devname[8];
		which_conn = atoi(nvram_safe_get("wan_connection"));
		memset(devname, 0, sizeof(devname));
		sprintf(devname, "nas%d", which_conn);
		mac_clone(devname);
		//return 0;
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
	//by songtao
	else if(!strcmp(service,"snmpd"))
	{       stop_snmp();
		start_snmp();
		stop_upnp();
		start_upnp();
		//return 0;
	}
	else if(!strcmp(service,"start_pppoe") || !strcmp(service,"start_pptp") || !strcmp(service,"start_heartbeat")){
		unlink("/tmp/ppp/log");
		stop_lan();
		stop_wan();
		//stop_pppoe();
		start_lan();
		start_wan(BOOT);
		//start_pppoe(BOOT);
	}
	else if(!strcmp(service,"stop_pppoe") || !strcmp(service,"stop_pptp") || !strcmp(service,"stop_heartbeat")){
		stop_wan();
	}
	else if(!strcmp(service,"filters")){
		stop_cron();
		start_cron();
		//stop_iptqueue();
		//start_iptqueue();
		stop_firewall();
		start_firewall();
	}
	else if(!strcmp(service,"forward")){
		stop_firewall();
		start_firewall();
	}
	else if(!strcmp(service,"forward_upnp")){
		stop_upnp();
		stop_firewall();
		start_upnp();
		start_firewall();
	}
	else if(!strcmp(service,"static_route_add")){
		stop_zebra();
		start_zebra();
		start_upnp();//leijun
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

	/* zhangbin for Ipsec starting*/
	else if(!strcmp(service,"ipsec"))
	{
		if(nvram_match("filter", "on"))
		{
			if(nvram_match("ipsec_pass", "0") ||
			   nvram_match("pptp_pass", "0") ||
			   nvram_match("l2tp_pass", "0"))
			{
				stop_firewall();
				sleep(2);
				start_firewall();
			}
			
		}
		stop_ipsec();
		sleep(2);
		start_ipsec();
		
	}	

	else if(!strcmp(service,"tftp_upgrade")){
		stop_wan();
		stop_httpd();
		stop_zebra();
		stop_upnp();
		stop_cron();
	}
	else if(!strcmp(service,"http_upgrade")){
		stop_wan();
		stop_zebra();
		stop_upnp();
		stop_cron();
	}
	else if(!strcmp(service,"wireless")){
		eval("/sbin/restart_wlan.sh");
	}
	else{
		nvram_unset("action_service");
		nvram_unset("action_service_arg1");
		kill(1, SIGHUP);
	}

	nvram_set("action_service","");
	nvram_set("action_service_arg1","");

	return 0;
}

int
start_pptp(int status)
{
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

/*
 * Call when keepalive mode
 */

int check_ppp_link(int num)
{
	int wan_link = 0;
	FILE *fp;
	char filename[18];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_link = 1;
	}
	return wan_link;
}


int check_ppp_proc(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[20];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/ppp%d.pid", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

int check_udhcpc_proc(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/udhcpc%d.pid", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

int check_udhcpc_link(int num)
{
	int wan_link = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/udhcpc%d.up", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_link = 1;
	}
	return wan_link;
}

int wanledctrl_main(int argc, char **argv)
{
	char *which_pvc = argv[1];
	char *mode = argv[2];
	char *submode = argv[3];
	unsigned int last_tx, last_rx;
	unsigned int curr_tx, curr_rx;
	last_tx = sar_getstats("Tx Total Bytes:");
	last_rx = sar_getstats("Rx Total Bytes:");
	
	while(1)
	{
		sleep(1);
		if(!check_adsl_status())
			diag_led_ctl(PPP_OFF);
		else
		{
			curr_tx = sar_getstats("Tx Total Bytes:");
			curr_rx = sar_getstats("Rx Total Bytes:");
			if((curr_tx != last_tx) || (curr_rx != last_rx))
			{
				last_tx = curr_tx;
				last_rx = curr_rx;
				diag_led_ctl(PPP_ACT);
			}
			else
			{
				if(!strcmp(mode, "ppp"))
				{
					if(!strcmp(submode, "demand"))
					{
						if(check_ppp_proc(atoi(which_pvc)))
							diag_led_ctl(PPP_CON);	
						else 
							diag_led_ctl(PPP_OFF);
					}
					else if(!strcmp(submode, "keepalive"))
					{
						if(check_ppp_proc(atoi(which_pvc)))	
						{
							if(check_ppp_link(atoi(which_pvc)))
								diag_led_ctl(PPP_CON);
							else
								diag_led_ctl(PPP_FAIL);
						}
						else
							diag_led_ctl(PPP_FAIL);
					}
				}
				else if(!strcmp(mode, "dhcp"))
				{	
					if(check_udhcpc_proc(atoi(which_pvc)))
					{
						if(check_udhcpc_link(atoi(which_pvc)))
							diag_led_ctl(PPP_CON);
						else
							diag_led_ctl(PPP_FAIL);
					}
					else
						diag_led_ctl(PPP_OFF);
				}
				else if(!strcmp(mode, "static"))
					diag_led_ctl(PPP_CON);
			}		
		}	
	}
	return 0;
}
			
#if 0
	if(!strcmp(mode, "ppp"))
	{
		while(1)
		{
			sleep(2);
			if(!strcmp(submode, "demand"))
			{
				if(check_adsl_status())
				{
					if(check_ppp_proc(atoi(which_pvc)))
					{
						diag_led_ctl(PPP_CON);	
						start_checkrate("2");
					}
					else 
					{	
						diag_led_ctl(PPP_OFF);
						stop_checkrate();
					}
				}
				else
				{
					diag_led_ctl(PPP_OFF);
					stop_checkrate();
				}
			}
			else if(!strcmp(submode, "keepalive"))
			{
				if(check_adsl_status())
				{
					if(check_ppp_proc(atoi(which_pvc)))	
					{
						if(check_ppp_link(atoi(which_pvc)))
						{
							diag_led_ctl(PPP_CON);
							start_checkrate("2");
						}
						else
						{
							diag_led_ctl(PPP_FAIL);
							stop_checkrate();
						}
					}
					else
					{
						diag_led_ctl(PPP_FAIL);
						stop_checkrate();
					}
				}
				else
				{
					diag_led_ctl(PPP_OFF);
					stop_checkrate();
				}
			}
		}		
	}
	else if(!strcmp(mode, "dhcp"))
	{	
		while(1)
		{
			sleep(2);
			if(check_adsl_status())
			{
				if(check_udhcpc_proc(atoi(which_pvc)))
				{
					if(check_udhcpc_link(atoi(which_pvc)))
					{
						diag_led_ctl(PPP_CON);
						start_checkrate("2");
					}
					else
					{	
						diag_led_ctl(PPP_FAIL);
						stop_checkrate();
					}
				}
				else
				{
					diag_led_ctl(PPP_OFF);
					stop_checkrate();
				}
			}
			else
			{
				diag_led_ctl(PPP_OFF);
				stop_checkrate();
			}
		}		
	}	
	return 0;
}	
#endif
			
	
//junzhao
int
redial_main(int argc, char **argv)
{
	int need_redial = 0;
	int status;
	pid_t pid;
	
	which_conn = atoi(&argv[2][3]);
	printf("--------which conn %d\n", which_conn);
	
	while(1)
	{
		if(need_redial)
			need_redial = 0;
		sleep(atoi(argv[1]));	

		//fprintf(stderr, "check PPPoE %d\n", num);
		if(!check_ppp_link(which_conn))
		{
			//fprintf(stderr, "PPPoE %d need to redial\n", num);
			need_redial = 1;
		}
		else{
			//fprintf(stderr, "PPPoE %d not need to redial\n", num);
			continue;
		}

			
#if 0
		dprintf("Check pppx if exist: ");
		if ((fp = fopen("/proc/net/dev", "r")) == NULL) {
  	              return -1;
	        }

		while( fgets(line, sizeof(line), fp) != NULL ) {
			if(strstr(line,"ppp")) {
				match=1;
				break;	
			}
		}
		fclose(fp);
		dprintf("%s",match == 1 ? "have exist\n" : "ready to dial\n");
#endif
	
		if(need_redial)
		{
			char whichconn[3];
			sprintf(whichconn, "%d", which_conn);
			//nvram_set("redial_num", whichconn);
			//nvram_set("redialing", "1");
			if(!buf_to_file("/tmp/ppp/redialing", "1"))
			{
				printf("in checkppp write %s file fail\n", "/tmp/ppp/redialing");
				continue;
			}
			if(!buf_to_file("/tmp/ppp/redial_num", whichconn))
			{
				printf("in checkppp write %s file fail\n", "/tmp/ppp/redial_num");
				continue;
			}
			pid = fork();
			switch(pid)
			{
				case -1:
					perror("fork failed");
					exit(1);
				case 0:
					/*
					//stop_wan();
					//stop_wan();
					stop_upnp();
					//sleep(1);
					start_wan(REDIAL);
					*/
					//kill(1, SIGHUP);
					kill(1, SIGUSR1);
					exit(0);
					break;
				default:
					waitpid(pid, &status, 0);
					//dprintf("parent\n");
					break;
			} // end switch
		} // end if
	} // end while
} // end main

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
checkadsl_main(int argc, char **argv)
{
	int need_redial=0;
	int status;
	pid_t pid;
	int lastadslstatus = 1, curadslstatus;
	
	while(1)
	{
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
			if(!buf_to_file("/tmp/ppp/ppp_action", "up"))
			{
				printf("in checkadsl write %s file fail\n", "/tmp/ppp/ppp_action");
				continue;
			}
		}
		else if(need_redial < 0)
		{
			if(!buf_to_file("/tmp/ppp/ppp_action", "down"))
			{
				printf("in checkadsl write %s file fail\n", "/tmp/ppp/ppp_action");
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
				kill(1, SIGUSR1);
				exit(0);
				break;
			default:
				waitpid(pid, &status, 0);
				//dprintf("parent\n");
				break;
		} // end switch
	} // end while
} // end main

//by songtao
int start_snmp(void)
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
}

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
	char rc_lsub[80],rc_lsub_client[50],rc_sec_gw[40],rc_any_gw[10],remote[100],remote_client[50],enc_config[8],auth_config[8],p1enc_config[10],p1auth_config[10],p1group_config[10],ike_config[30],rc_nexthop_config[20],rc_ikemode_config[15],rc_esp_config[20],manual_file[200];
	
	memset(rc_sec_gw,0,sizeof(rc_sec_gw));
	memset(rc_any_gw,0,sizeof(rc_any_gw));
	memset(rc_nexthop_config,0,sizeof(rc_nexthop_config));
	memset(rc_lsub,0,sizeof(rc_lsub));
	memset(rc_lsub_client,0,sizeof(rc_lsub_client));
	memset(rc_ikemode_config,0,sizeof(rc_ikemode_config));
	memset(rc_esp_config,0,sizeof(rc_esp_config));
	memset(manual_file,0,sizeof(manual_file));

		
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
	/* first,we should del the rule which would let the packets go around nat'ed */	
	fprintf(nat_fd,"#!/bin/sh\n");
	fprintf(nat_fd,"iptables -t nat -D POSTROUTING -s %s -d %s -j ACCEPT\n",rc_lsub_client,remote_client);
	fprintf(nat_fd,"iptables -D FORWARD -i ipsec0 -j ACCEPT\n");
	fprintf(nat_fd,"iptables -D INPUT -p 50 -j ACCEPT\n");
	fprintf(nat_fd,"echo 1 > /proc/sys/net/ipv4/conf/nas0/rp_filter\n");
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
	//printf("rc_sec_gw=%s,rc_any_gw=%s\n,rc_nexthop_config=%s,rc_lsub=%s,rc_lsub_client=%s\n",rc_sec_gw,rc_any_gw,rc_nexthop_config,rc_lsub,rc_lsub_client);
	printf("rc_key_type=%s\n",rc_key_type);
	if(!strcmp(rc_key_type,"0"))
	{
		FILE *afd,*prefd,*psk_fd; //auto key fd and preshared key fd
		char pfs_config[10];
		char state_file[30],pskbuf[1024],psk[100],ip1[20],ip2[20];
		int find_it = 0;
			
		sprintf(state_file,"/tmp/%s",rc_tunnel_name);
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
		while(fgets(pskbuf,1024,psk_fd))
		{
			printf("parsing!\n");
			sscanf(pskbuf,"%s %s : PSK %s",ip1,ip2,psk);
			printf("ip1=%s,ip2=%s\n",ip1,ip2);
			if(!strcmp(ip2,rc_sec_gw) && !strcmp(ip1,rc_wan_ipaddress))
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
			printf("NOT found!\n");

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
			
		if((afd = fopen(AUTOKEY_FILE,"w")) == NULL)
		{
			printf("cannot open auto key file!\n");
			return -1;
		}
				
		fprintf(afd,"#!/bin/sh\n");
		fprintf(afd,"/bin/ipsec/whack --name %s --encrypt --tunnel --esp %s --ike %s %s --psk --host %s %s --nexthop %s --to --host %s %s --nexthop %s %s --ikelifetime %s --ipseclifetime %s --rekeywindow 540 --keyingtries 0\n",rc_tunnel_name,rc_esp_config,ike_config,pfs_config,rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rc_sec_gw,remote,rc_nexthop_config,rc_ikemode_config,rc_ipsecp1_klife,rc_auto_klife);
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
			
		eval("/bin/sh",AUTOKEY_FILE);

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
			
	ret = eval("killall","-9","pluto");
	ret += eval("rm","/var/run/pluto.pid");
	ret += eval("ifconfig","ipsec0","down");
	
	return ret;
}
