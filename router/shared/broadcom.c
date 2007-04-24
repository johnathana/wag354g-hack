
/*
 *********************************************************
 *   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
 *********************************************************

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
 the contents of this file may not be disclosed to third parties,
 copied or duplicated in any form without the prior written
 permission of CyberTAN Inc.

 This software should be used as a reference only, and it not
 intended for production use!


 THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
 SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
*/

/*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: broadcom.c,v 1.22 2006/09/08 03:18:36 dom Exp $
 */

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <httpd.h>
#include <errno.h>
#endif /* WEBS */

#include <proto/ethernet.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>

#include "broadcom.h"
#include <cyutils.h>
#include <support.h>
#include <cy_conf.h>
#include "pvc.h"

char vcc_config_buf[256];
extern int vcc_inited;
extern char re_ip_des[20];
extern char re_ip_local[20];
extern unsigned int accessing_ip;
extern unsigned int remote_pvc_ip;
int gozila_action = 0;
int error_value = 0;
/*-----------songtao-----------*/
int cl_page=0;
int status=1;
int refresh=1;
int pagenum=0;
int selet_flags=0;
struct logmsg *head=NULL;
struct logmsg *cur_line=NULL;
struct logmsg *tail=NULL;
struct show_page *show_page_head=NULL;
struct show_page *cur_page=NULL;
struct show_page *show_page_tail=NULL;
       

#define MAX_SIZE                20480
void signal_wakeup(int sig);

#ifdef NOMAD_SUPPORT
/* add by xiaoqin for nomad, 2005.03.31*/
extern int do_nomad_file(char *path, webs_t stream, int flag);
#endif

#ifdef DEBUG_WEB
int debug_value = 1;
#else
int debug_value = 0;
#endif

#ifdef HSIAB_SUPPORT
extern int register_status;
extern int new_device;
#endif

//junzhao
extern char connection[5];

//static char * rfctime(const time_t *timep);
//static char * reltime(unsigned int seconds);

//#if defined(linux)

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#define service_restart() kill(1, SIGUSR1)
#define sys_restart() kill(1, SIGHUP)
#define sys_reboot() kill(1, SIGTERM)
//junzhao 2004.8.9
#define sys_stop() kill(1, SIGPROF)
#define sys_start() kill(1, SIGUSR2)

#define sys_stats(url) eval("stats", (url))

/* Example:
 * ISDIGIT("", 0); return true;
 * ISDIGIT("", 1); return false;
 * ISDIGIT("123", 1); return true;
 */
int
ISDIGIT(char *value, int flag)
{
	int i, tag = TRUE;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): len=(%d)",__FUNCTION__,value,strlen(value));
#endif

	if(!strcmp(value,"")){
		if (flag) return 0;   // null
		else	return 1;
	}

	for(i=0 ; *(value+i) ; i++){
		if(!isdigit(*(value+i))){
			tag = FALSE;
			break;
		}
	}
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): (%s)",__FUNCTION__,value,tag ? "TRUE" : "FALSE");
#endif
	return tag;
}

/* Example:
 * ISASCII("", 0); return true;
 * ISASCII("", 1); return false;
 * ISASCII("abc123", 1); return true;
 */
int
ISASCII(char *value, int flag)
{
	int i, tag = TRUE;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): len=(%d)",__FUNCTION__,value,strlen(value));
#endif

#if COUNTRY == JAPAN
	return tag;	// don't check for japan version
#endif

	if(!strcmp(value,"")){
		if (flag) return 0;   // null
		else	return 1;
	}

	for(i=0 ; *(value+i) ; i++){
		if(!isascii(*(value+i))){
			tag = FALSE;
			break;
		}
	}
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): (%s)",__FUNCTION__,value,tag ? "TRUE" : "FALSE");
#endif
	return tag;
}

/* Example:
 * legal_hwaddr("00:11:22:33:44:aB"); return true;
 * legal_hwaddr("00:11:22:33:44:5"); return false;
 * legal_hwaddr("00:11:22:33:44:HH"); return false;
 */
int
legal_hwaddr(char *value)
{
	unsigned int hwaddr[6];
	int tag = TRUE;
	int i,count;

	/* Check for bad, multicast, broadcast, or null address */
	for(i=0,count=0 ; *(value+i) ; i++){
		if(*(value+i) == ':'){
			if((i+1)%3 != 0){
				tag = FALSE;
				break;
			}	
			count++;
		}
		else if(isxdigit(*(value+i))) /* one of 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F */
			continue;
		else{
			tag = FALSE;
			break;
		}
	}
	
	if (!tag || i != 17 || count != 5)		/* must have 17's characters and 5's ':' */
		tag = FALSE;	
	else if (sscanf(value, "%x:%x:%x:%x:%x:%x",
		   &hwaddr[0], &hwaddr[1], &hwaddr[2],
		   &hwaddr[3], &hwaddr[4], &hwaddr[5]) != 6 ){
	    //(hwaddr[0] & 1) ||		// the bit 7 is 1 
	    //(hwaddr[0] & hwaddr[1] & hwaddr[2] & hwaddr[3] & hwaddr[4] & hwaddr[5]) == 0xff ){ // FF:FF:FF:FF:FF:FF
	    //(hwaddr[0] | hwaddr[1] | hwaddr[2] | hwaddr[3] | hwaddr[4] | hwaddr[5]) == 0x00){ // 00:00:00:00:00:00
		tag = FALSE;
	}
	else
		tag = TRUE;
	
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): (%s)",__FUNCTION__,value,tag?"TRUE":"FALSE");
#endif

	return tag;
}

/* Example:
 * 255.255.255.0  (111111111111111111111100000000)  is a legal netmask
 * 255.255.0.255  (111111111111110000000011111111)  is an illegal netmask
 */
int
legal_netmask(char *value)
{
	struct in_addr ipaddr;
	int ip[4]={0,0,0,0};
	int i,j;
	int match0 = -1;
	int match1 = -1;
	int ret, tag;

	ret=sscanf(value,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);

	if (ret == 4 && inet_aton(value, &ipaddr)) {
		for(i=3;i>=0;i--){
			for(j=1;j<=8;j++){
				if((ip[i] % 2) == 0)   match0 = (3-i)*8 + j;
				else if(((ip[i] % 2) == 1) && match1 == -1)   match1 = (3-i)*8 + j;
				ip[i] = ip[i] / 2;
			}
		}
	}

	if (match0 >= match1)
		tag = FALSE;
	else
		tag = TRUE;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): match0=[%d] match1=[%d] (%s)",__FUNCTION__,value,match0,match1,tag?"TRUE":"FALSE");
#endif

	return tag;
}


/* Example:
 * legal_ipaddr("192.168.1.1"); return true;
 * legal_ipaddr("192.168.1.1111"); return false;
 */
int
legal_ipaddr(char *value)
{
	struct in_addr ipaddr;
	int ip[4];
	int ret, tag;

	ret = sscanf(value,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);

	if (ret != 4 || !inet_aton(value, &ipaddr))
		tag = FALSE;
	else
		tag = TRUE;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): ret=[%d] ip[%d][%d][%d][%d] (%s)",__FUNCTION__,value,ret,ip[0],ip[1],ip[2],ip[3],tag?"TRUE":"FALSE");
#endif

	return tag;
}

/* Example:
 * legal_ip_netmask("192.168.1.1","255.255.255.0","192.168.1.100"); return true;
 * legal_ip_netmask("192.168.1.1","255.255.255.0","192.168.2.100"); return false;
 */
int
legal_ip_netmask(char *sip, char *smask, char *dip)
{
	struct in_addr ipaddr, netaddr, netmask;
	int tag;

	inet_aton(nvram_safe_get(sip), &netaddr);
	inet_aton(nvram_safe_get(smask), &netmask);
	inet_aton(dip, &ipaddr);

	netaddr.s_addr &= netmask.s_addr;

	if (netaddr.s_addr != (ipaddr.s_addr & netmask.s_addr))
		tag = FALSE;
	else
		tag = TRUE;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): sip=[%s] smask=[%s] dip=[%s] (%s)",__FUNCTION__,nvram_safe_get(sip),nvram_safe_get(smask),dip,tag?"TRUE":"FALSE");
#endif
	
	return tag;
}


/* Example:
 * wan_dns = 1.2.3.4 10.20.30.40 15.25.35.45
 * get_dns_ip("wan_dns", 1, 2); produces "20"
 */
int
get_dns_ip(char *name, int which, int count)
{
        static char word[256];
        char *next;
	int ip;

        foreach(word, nvram_safe_get(name), next) {
                if (which-- == 0){
			ip = get_single_ip(word,count);
#ifdef MY_DEBUG
	//LOG(LOG_DEBUG,"%s(): ip=[%d]",__FUNCTION__,ip);
#endif
                	return ip;
                }
        }
        return 0;
}


/* Example:
 * wan_mac = 00:11:22:33:44:55
 * get_single_mac("wan_mac", 1); produces "11"
 */
int
get_single_mac(char *macaddr, int which)
{
	int mac[6]={0,0,0,0,0,0};
	int ret;

	ret = sscanf(macaddr,"%2X:%2X:%2X:%2X:%2X:%2X",&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
	return mac[which];	
}


/* Example:
 * lan_ipaddr_0 = 192
 * lan_ipaddr_1 = 168
 * lan_ipaddr_2 = 1
 * lan_ipaddr_3 = 1
 * get_merge_ipaddr("lan_ipaddr", ipaddr); produces ipaddr="192.168.1.1"
 */
int
get_merge_ipaddr(char *name, char *ipaddr)
{
	char ipname[30];
	int i;
	
	strcpy(ipaddr,"");	

	for(i=0 ; i<4 ; i++){
		snprintf(ipname,sizeof(ipname),"%s_%d",name,i);
		strcat(ipaddr, websGetVar(wp, ipname , "0"));
		if(i<3)	strcat(ipaddr, ".");
	}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): ipaddr=[%s]",__FUNCTION__,ipaddr);
#endif
	return 1;

}


/* Example:
 * wan_mac_0 = 00
 * wan_mac_1 = 11
 * wan_mac_2 = 22
 * wan_mac_3 = 33
 * wan_mac_4 = 44
 * wan_mac_5 = 55
 * get_merge_mac("wan_mac",mac); produces mac="00:11:22:33:44:55"
 */
int 
get_merge_mac(char *name, char *macaddr)
{
	char macname[30];
	char *mac;
	int i;

	strcpy(macaddr,"");

	for(i=0 ; i<6 ; i++){
		snprintf(macname,sizeof(macname),"%s_%d",name,i);
		mac = websGetVar(wp, macname , "00");
		if(strlen(mac) == 1)	strcat(macaddr,"0");
		strcat(macaddr,mac);
		if(i<5)	strcat(macaddr,":");
	}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): macaddr=[%s]",__FUNCTION__,macaddr);
#endif
	return 1;

}

struct onload onloads[] = {
	//{ "Filters", filter_onload },
//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "WL_ActiveTable", wl_active_onload },
	//wwzh
	{ "Wireless_Connected", wl_active_onload },
#endif
	{ "MACClone", macclone_onload },
#ifdef FIREWALL_LEVEL_SUPPORT
	{ "Filter_Inb_Summary", filtersummary_inb_onload },
#endif
#ifdef DIAG_SUPPORT
	{ "Ping", ping_onload },
	{ "Traceroute", traceroute_onload },
#endif
};

int
ej_onload(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type, *arg;
	int ret = 0;
	struct onload *v;

        if (ejArgs(argc, argv, "%s %s", &type, &arg) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	for(v = onloads ; v < &onloads[STRUCT_LEN(onloads)] ; v++) {
#ifdef MY_DEBUG
   		LOG(LOG_DEBUG,"%s(%s): onload=[%s]\n",__FUNCTION__,type,v->name);
#endif
   		if(!strcmp(v->name,type)){
      			ret = v->go(wp, arg);
			return ret;
		}
   	}

        return ret;
}

/* Meta tag command that will no allow page cached by browsers.
 * The will force the page to be refreshed when visited.
 */
int
ej_no_cache(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;

	ret = websWrite(wp,"<meta http-equiv=\"expires\" content=\"0\">\n");
	ret += websWrite(wp,"<meta http-equiv=\"cache-control\" content=\"no-cache\">\n");
	ret += websWrite(wp,"<meta http-equiv=\"pragma\" content=\"no-cache\">");

	return ret;
}
	

/*
 * Example: 
 * lan_ipaddr=192.168.1.1
 * <% prefix_ip_get("lan_ipaddr",1); %> produces "192.168.1."
 */
int
ej_prefix_ip_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name;
	int type;
	int ret = 0;

        if (ejArgs(argc, argv, "%s %d", &name,&type) < 2) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): ",__FUNCTION__,name,type);	
#endif
	
	if(type == 1)
		ret = websWrite(wp,"%d.%d.%d.",get_single_ip(nvram_safe_get("lan_ipaddr"),0),
					       get_single_ip(nvram_safe_get("lan_ipaddr"),1),
					       get_single_ip(nvram_safe_get("lan_ipaddr"),2));

	return ret;
}

/* Deal with side effects before committing */
int
sys_commit(void)
{
	//if (nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "pptp") )
	//	nvram_set("wan_ifname", "ppp0");
	//else
	//	nvram_set("wan_ifname", nvram_get("pppoe_ifname"));
	return nvram_commit();
}


char *
rfctime(const time_t *timep)
{
	static char s[201];
	struct tm tm;

	setenv("TZ", nvram_safe_get("time_zone"), 1);
	memcpy(&tm, localtime(timep), sizeof(struct tm));
	//strftime(s, 200, "%a, %d %b %Y %H:%M:%S %z", &tm);
	strftime(s, 200, "%a, %d %b %Y %H:%M:%S", &tm);  // spec for linksys
	return s;
}

static char *
reltime(unsigned int seconds)
{
	static char s[] = "XXXXX days, XX hours, XX minutes, XX seconds";
	char *c = s;

	if (seconds > 60*60*24) {
		c += sprintf(c, "%d days, ", seconds / (60*60*24));
		seconds %= 60*60*24;
	}
	if (seconds > 60*60) {
		c += sprintf(c, "%d hours, ", seconds / (60*60));
		seconds %= 60*60;
	}
	if (seconds > 60) {
		c += sprintf(c, "%d minutes, ", seconds / 60);
		seconds %= 60;
	}
	c += sprintf(c, "%d seconds", seconds);

	return s;
}

//junzhao 2004.4.22 for dhcp setting
int
ej_lan_dhcp_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *lan_proto = nvram_safe_get("lan_proto");
	char *count;

	ejArgs(argc, argv, "%s", &count);
	if(!strcmp(lan_proto, "dhcp"))
	{
		if(!strcmp(count, "dom"))
			return websWrite(wp, "<script>Capture(share.enable)</script>");
		else
			return websWrite(wp, "%s", "Enable");
	}
	else if(!strcmp(lan_proto, "static"))
	{
		if(!strcmp(count, "dom"))
			return websWrite(wp, "<script>Capture(share.disable)</script>");
		else
			return websWrite(wp, "%s", "Disable");
	}
	else if(!strcmp(lan_proto, "dhcprelay"))
	{
		if(!strcmp(count, "dom"))
			return websWrite(wp, "%s (%s)", "<script>Capture(setnetsetup.dhcprelay)</script>", nvram_safe_get("dhcpserver_ipaddr"));
		else
			return websWrite(wp, "%s (DHCP Relay)", nvram_safe_get("dhcpserver_ipaddr"));
	}
}

/*
 * Example: 
 * lan_ipaddr = 192.168.1.1
 * <% nvram_get("lan_ipaddr"); %> produces "192.168.1.1"
 */
static int
ej_nvram_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c, *tok;
	int ret = 0;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (!strcmp(name, "dom_release_date"))
	{//dom 2005/05/27 check ang change month to other language
		char *mon[] = { "January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December" };
		char *mon2[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		int i;
		for ( i = 0; i < 12; i++ )
		{
			if ( strstr( nvram_safe_get("release_date"), mon2[i] ) != NULL )
			{
				c = nvram_safe_get("release_date") + 4;
				tok = strtok(c, " ");
				ret += websWrite(wp, "%s.", tok);
				ret =  websWrite(wp, " <script>Capture(share.%s)</script> ", mon[i]);
				c = strtok(NULL, mon2[i]);
				break;
			}

		}

	}
	else
		c = nvram_safe_get(name);
#if COUNTRY == JAPAN
	ret = websWrite(wp, "%s",nvram_safe_get(name));
#else
	for (c; *c; c++) {
		if (isprint((int) *c) &&
		    *c != '"' && *c != '&' && *c != '<' && *c != '>')
			ret += websWrite(wp, "%c", *c);
		else
			ret += websWrite(wp, "&#%d", *c);
	}
#endif
	return ret;
}

/*
 * Example: 
 * lan_ipaddr = 192.168.1.1, gozila_action = 0
 * <% nvram_selget("lan_ipaddr"); %> produces "192.168.1.1"
 * lan_ipaddr = 192.168.1.1, gozila_action = 1, websGetVar(wp, "lan_proto", NULL) = 192.168.1.2;
 * <% nvram_selget("lan_ipaddr"); %> produces "192.168.1.2"
 */
static int
ej_nvram_selget(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c;
	int ret = 0;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	if(gozila_action){
		char *buf = websGetVar(wp, name, NULL);
		if(buf)
			return websWrite(wp, "%s", buf);
	}

	for (c = nvram_safe_get(name); *c; c++) {
		if (isprint((int) *c) &&
		    *c != '"' && *c != '&' && *c != '<' && *c != '>')
			ret += websWrite(wp, "%c", *c);
		else
			ret += websWrite(wp, "&#%d", *c);
	}

	return ret;
}

/*
 * Example: 
 * wan_mac = 00:11:22:33:44:55
 * <% nvram_mac_get("wan_mac"); %> produces "00-11-22-33-44-55"
 */
static int
ej_nvram_mac_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c;
	int ret = 0;
	char *mac;
	int i;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	c = nvram_safe_get(name);

	if(c){
		mac = strdup(c);
		for(i=0 ; *(mac+i) ; i++){
			if(*(mac+i) == ':')	*(mac+i) = '-';
		}
		ret = websWrite(wp, "%s",mac );
	}

	return ret;

}
#ifdef MULTIUSER_SUPPORT
/*modified by zls 2004/11/19*/
int ej_show_http_username_table(int eid,webs_t wp,int argc, char_t **argv)
{
        int i,ret=0,page;
        char *type;
        char cur_userid[5];
        if (ejArgs(argc,argv,"%s",&type)<1){
                websError(wp,400,"Insufficient args\n");
                return -1;
        }
        if(gozila_action) //for form post
        {
                char *temp_userid=websGetVar(wp,"http_username_page",NULL);
                strncpy(cur_userid,temp_userid,sizeof(cur_userid));
                if(atoi(cur_userid)<AUTH_SUM)
                {
                        nvram_set("login_userid",cur_userid);
                        nvram_commit();
                }
        }
        else //first display
        {
#if 0
        strncpy(cur_userid,nvram_safe_get("cur_userid"),sizeof(cur_userid));
        nvram_set("cur_userid",cur_userid);
#else
        strcpy(cur_userid,nvram_safe_get("login_userid"));
#endif
 	}
        page=atoi(cur_userid);
        if(!strcmp(type,"select")){
                for(i=0;i<AUTH_SUM;i++){
                   ret=websWrite(wp,"\t\t<option value=\"%d\"%s>%d</option>\n",i,(i==page)?"selected":"",i+1);
        }
        }
        return ret;
}
/*modified by zls 2004/11/22*/
int ej_get_user(int eid,webs_t wp,int argc,char_t **argv)
{
        char *type;
        char  cur_userid[5];
        char temp_username[AUTH_MAX],temp_passwd[AUTH_MAX],var_username[AUTH_MAX],var_passwd[AUTH_MAX];
        char *temp_userid;
        if(ejArgs(argc,argv,"%s",&type)<1){
                websError(wp,400,"Insufficient args\n");
                return -1;
        }
#ifdef MULTIUSER_SUPPORT
        if(gozila_action){
                temp_userid=websGetVar(wp,"http_username_page",NULL);
                strcpy(cur_userid,temp_userid);
        }else
        {
                strcpy(cur_userid,nvram_safe_get("login_userid"));
        }
        if (atoi(cur_userid) !=0){
               if(!strcasecmp(type,"http_username"))
                {
                        sprintf(var_username,"http_username%s",cur_userid);
                        strcpy(temp_username,nvram_safe_get(var_username));
                        return websWrite(wp,"%s",temp_username);
                }
                else if(!strcasecmp(type,"http_passwd"))
                 {
                        sprintf(var_passwd,"http_passwd%s",cur_userid);
                        strcpy(temp_passwd,nvram_safe_get(var_passwd));
           return websWrite(wp,"%s",temp_passwd);
                }
        }else{
                if(!strcasecmp(type,"http_username"))
                {
                        strcpy(var_username,"http_username");
                        strcpy(temp_username,nvram_safe_get(var_username));
                        return websWrite(wp,"%s",temp_username);
                }
                else if(!strcasecmp(type,"http_passwd"))
                {
                        strcpy(var_passwd,"http_passwd");
                        strcpy(temp_passwd,nvram_safe_get(var_passwd));
                        return websWrite(wp,"%s",temp_passwd);
                }
                                                                                                               
        }
        return websWrite(wp,"");
#else
    /* 	char *name, *c;
	int ret = 0;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

#if COUNTRY == JAPAN
	ret = websWrite(wp, "%s",nvram_safe_get(name));
#else
	for (c = nvram_safe_get(name); *c; c++) {
		if (isprint((int) *c) &&
		    *c != '"' && *c != '&' && *c != '<' && *c != '>')
			ret += websWrite(wp, "%c", *c);
		else
			ret += websWrite(wp, "&#%d", *c);
	}
#endif
	return ret;
    
*/
#endif
}
#endif
/*
 * Example: 
 * wan_proto = dhcp; gozilla = 0;
 * <% nvram_gozila_get("wan_proto"); %> produces "dhcp"
 *
 * wan_proto = dhcp; gozilla = 1; websGetVar(wp, "wan_proto", NULL) = static;
 * <% nvram_gozila_get("wan_proto"); %> produces "static"
 */
static int
ej_nvram_gozila_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *type;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	type = GOZILA_GET(name);

	return websWrite(wp, "%s", type);
}

/*
 * Example: 
 * lan_ipaddr = 192.168.1.1
 * <% get_single_ip("lan_ipaddr","1"); %> produces "168"
 */
static int
ej_get_single_ip(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c;
	int ret = 0;
	int which;

	if (ejArgs(argc, argv, "%s %d", &name, &which) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	c = nvram_safe_get(name);
	if(c){
		if(!strcmp(c, PPP_PSEUDO_IP) || !strcmp(c, PPP_PSEUDO_GW))
			c = "0.0.0.0";
		else if (!strcmp(c, PPP_PSEUDO_NM))
			c = "255.255.255.0";

		ret += websWrite(wp, "%d", get_single_ip(c,which));
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): name=[%s] which=[%d] ip=[%d]",__FUNCTION__,name,which,get_single_ip(c,which));
#endif
	}
	else
		ret += websWrite(wp, "0");

	return ret;
}

/*
 * Example: 
 * wan_mac = 00:11:22:33:44:55
 * <% get_single_mac("wan_mac","1"); %> produces "11"
 */
static int
ej_get_single_mac(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c;
	int ret = 0;
	int which;
	int mac;

	if (ejArgs(argc, argv, "%s %d", &name, &which) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	c = nvram_safe_get(name);
	if(c){
		mac = get_single_mac(c,which);
		ret += websWrite(wp, "%02X", mac);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): name=[%s] which=[%d] mac=[%02X]",__FUNCTION__,name,which,mac);
#endif
	}
	else
		ret += websWrite(wp, "00");

	return ret;
}

/*
 * Example: 
 * wan_proto = dhcp; gozilla = 0;
 * <% nvram_selmatch("wan_proto", "dhcp", "selected"); %> produces "selected"
 *
 * wan_proto = dhcp; gozilla = 1; websGetVar(wp, "wan_proto", NULL) = static;
 * <% nvram_selmatch("wan_proto", "static", "selected"); %> produces "selected"
 */
int
ej_nvram_selmatch(int eid, webs_t wp, int argc, char_t **argv)
{
        char *name, *match, *output;
        char *type;

        if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

        type = GOZILA_GET(name);
        
        if (!type){
           if (nvram_match(name, match)){
                return websWrite(wp, output);
           }
        }
        else{
           if(!strcmp(type, match)){
                return websWrite(wp, output);
           }
        }

        return 0;
}       

int
ej_nvram_else_selmatch(int eid, webs_t wp, int argc, char_t **argv)
{
        char *name, *match, *output1, *output2;
        char *type;

        if (ejArgs(argc, argv, "%s %s %s %s", &name, &match, &output1, &output2) < 4) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

        type = GOZILA_GET(name);
        
        if (!type){
           if (nvram_match(name, match)){
                return websWrite(wp, output1);
           }
	   else
                return websWrite(wp, output2);
        }
        else{
           if(!strcmp(type, match)){
                return websWrite(wp, output1);
           }
	   else
                return websWrite(wp, output2);
        }

        return 0;
}       

/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_else_match("wan_proto", "dhcp", "0","1"); %> produces "0"
 * <% nvram_else_match("wan_proto", "static", "0","1"); %> produces "1"
 */
static int
ej_nvram_else_match(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *match, *output1, *output2;

	if (ejArgs(argc, argv, "%s %s %s %s", &name, &match, &output1, &output2) < 4) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match(name, match))
		return websWrite(wp, output1);
	else
		return websWrite(wp, output2);

	return 0;
}	

/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_match("wan_proto", "dhcp", "selected"); %> produces "selected"
 * <% nvram_match("wan_proto", "static", "selected"); %> does not produce
 */
static int
ej_nvram_match(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *match, *output;

	if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match(name, match))
		return websWrite(wp, output);

	return 0;
}	

/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_invmatch("wan_proto", "dhcp", "disabled"); %> does not produce
 * <% nvram_invmatch("wan_proto", "static", "disabled"); %> produces "disabled"
 */
static int
ej_nvram_invmatch(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *invmatch, *output;

	if (ejArgs(argc, argv, "%s %s %s", &name, &invmatch, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_invmatch(name, invmatch))
		return websWrite(wp, output);

	return 0;
}	

/*
 * Example: 
 * HEARTBEAT_SUPPORT = 1
 * <% support_match("HEARTBEAT_SUPPORT", "0", "selected"); %> does not produce
 * <% support_match("HEARTBEAT_SUPPORT", "1", "selected"); %> produces "selected"
 */
static int
ej_support_match(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *value, *output;
	struct support_list *v;

	if (ejArgs(argc, argv, "%s %s %s", &name, &value, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(!strcmp(name, "BACKUP_RESTORE_SUPPORT") ||
	   !strcmp(name, "SYSLOG_SUPPORT"))	return 1;
	
	for(v = supports ; v < &supports[SUPPORT_COUNT] ; v++) {
   		if(!strcmp(v->supp_name, name) && !strcmp(v->supp_value, value)){
			return websWrite(wp, output);
		}
   	}
	
	return 1;	
}	


/*
 * Example: 
 * HEARTBEAT_SUPPORT = 1
 * <% support_invmatch("HEARTBEAT_SUPPORT", "1", "<!--"); %> does not produce
 * HEARTBEAT_SUPPORT = 0
 * <% support_invmatch("HEARTBEAT_SUPPORT", "1", "-->"); %> produces "-->"
 */
static int
ej_support_invmatch(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *value, *output;
	struct support_list *v;

	if (ejArgs(argc, argv, "%s %s %s", &name, &value, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(!strcmp(name, "BACKUP_RESTORE_SUPPORT") ||
	   !strcmp(name, "SYSLOG_SUPPORT"))
		return websWrite(wp, output);
	for(v = supports ; v < &supports[SUPPORT_COUNT] ; v++) {
   		if(!strcmp(v->supp_name, name)){
			if (strcmp(v->supp_value, value)){
				return websWrite(wp, output);
			}
			else
				return 1;
		}
   	}
	return websWrite(wp, output);
}

/*
 * Example: 
 * HEARTBEAT_SUPPORT = 1
 * <% support_elsematch("HEARTBEAT_SUPPORT", "1", "black", "red"); %> procude "black"
 */
static int
ej_support_elsematch(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *value, *output1, *output2;
	struct support_list *v;

	if (ejArgs(argc, argv, "%s %s %s %s", &name, &value, &output1, &output2) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

#if 0
	if(!strcmp(name, "BACKUP_RESTORE_SUPPORT") ||
	   !strcmp(name, "SYSLOG_SUPPORT"))
		return websWrite(wp, output2);
#else
	if(!strcmp(name, "SYSLOG_SUPPORT"))
		return websWrite(wp, output2);

#endif
	
	for(v = supports ; v < &supports[SUPPORT_COUNT] ; v++) {
   		if(!strcmp(v->supp_name, name) && !strcmp(v->supp_value, value)){
			return websWrite(wp, output1);
		}
   	}
	
	return websWrite(wp, output2);	
}	

static int
ej_scroll(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int y;

	if (ejArgs(argc, argv, "%s %d", &type, &y) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	if(gozila_action)
		websWrite(wp, "%d", y);
	else
		websWrite(wp, "0");
		
	return 0;
}
/*
 * Example: 
 * filter_mac=00:12:34:56:78:00 00:87:65:43:21:00
 * <% nvram_list("filter_mac", 1); %> produces "00:87:65:43:21:00"
 * <% nvram_list("filter_mac", 100); %> produces ""
 */
static int
ej_nvram_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name;
	int which;
	char word[256], *next;
	int ret = 0;

	if (ejArgs(argc, argv, "%s %d", &name, &which) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	foreach(word, nvram_safe_get(name), next) {
		if (which-- == 0)
			ret += websWrite(wp, word);
	}

	return ret;
}

/* Example: 
 * wan_dns = 168.95.1.1 210.66.161.125 168.95.192.1
 * <% get_dns_ip("wan_dns", "1", "2"); %> produces "161"
 * <% get_dns_ip("wan_dns", "2", "3"); %> produces "1"
 */
int
ej_get_dns_ip(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name;
	int count, which;
	char word[256], *next;

	if (ejArgs(argc, argv, "%s %d %d", &name, &which, &count) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	foreach(word, nvram_safe_get(name), next) {
		if (which-- == 0)
			return websWrite(wp, "%d", get_single_ip(word,count));
	}

	return websWrite(wp, "0");	// not find
}


static unsigned long
inet_atoul(char *cp)
{
	struct in_addr in;

	(void) inet_aton(cp, &in);
	return in.s_addr;
}

int
valid_wep_key(webs_t wp, char *value, struct variable *v)
{
	int i;

	switch(strlen(value)){
		case 5:
		case 13: 
			for(i=0 ; *(value+i) ; i++){
                                if(isascii(*(value+i))){ 
                                        continue;
                                }
                                else{ 
                                        websDebugWrite(wp, "Invalid <b>%s</b> %s: must be ascii code<br>",
                                            v->longname, value);
                                        return FALSE;
                                }
                         }
			break;
		case 10:
		case 26:
			for(i=0 ; *(value+i) ; i++){
		                if(isxdigit(*(value+i))){ /* one of 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F */
                		        continue;
          		        }
         		        else{   
                    		        websDebugWrite(wp, "Invalid <b>%s</b> %s: must be hexadecimal digits<br>",
                              		    v->longname, value);
                     			return FALSE;
               			}
       			 }
			 break;

		default: websDebugWrite(wp, "Invalid <b>%s</b>: must be 5 or 13 ASCII characters or 10 or 26 hexadecimal digits<br>", v->longname); return FALSE;

	}

/*
	for(i=0 ; *(value+i) ; i++){
		if(isxdigit(*(value+i))){ 
			continue;
		}
		else{
			websDebugWrite(wp, "Invalid <b>%s</b> %s: must be hexadecimal digits<br>",
				  v->longname, value);
			return FALSE;
		}
	}

	if (i != length) {
		websDebugWrite(wp, "Invalid <b>%s</b> %s: must be %d characters<br>",
			  v->longname, value,length);
		return FALSE;
	}
*/
	return TRUE;
}

int
valid_netmask(webs_t wp, char *value, struct variable *v)
{

	if(!legal_netmask(value)){
		websDebugWrite(wp, "Invalid <b>%s</b> %s: not a legal netmask<br>",
			  v->longname, value);
		return FALSE;
	}

	return TRUE;

}

static void
validate_netmask(webs_t wp, char *value, struct variable *v)
{
	if (valid_netmask(wp, value, v))
		nvram_set(v->name, value);
}

static void
validate_merge_netmask(webs_t wp, char *value, struct variable *v)
{
	char *netmask, maskname[30];
	char *mask;
	int i;

	netmask = malloc(20);
	strcpy(netmask,"");
	for(i=0 ; i<4 ; i++){
		snprintf(maskname,sizeof(maskname),"%s_%d",v->name,i);
		mask = websGetVar(wp, maskname , NULL);
		if(mask){
			strcat(netmask,mask);
			if(i<3)	strcat(netmask,".");
		}
		else
			return ;
	}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): netmask=[%s]",__FUNCTION__,netmask);
#endif

	if(valid_netmask(wp, netmask, v))
		nvram_set(v->name, netmask);

	if(netmask)	free(netmask);
}

static void
validate_list(webs_t wp, char *value, struct variable *v,
	      int (*valid)(webs_t, char *, struct variable *))
{
	int n, i;
	char name[100];
	char buf[1000] = "", *cur = buf;

	n = atoi(value);

	for (i = 0; i < n; i++) {
		snprintf(name, sizeof(name), "%s%d", v->name, i);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): [%s]=[%s]", __FUNCTION__, i, name, websGetVar(wp, name, NULL));
#endif
		if (!(value = websGetVar(wp, name, NULL)))
			return;
		if (!*value && v->nullok)
			continue;
		if (!valid(wp, value, v))
			continue;
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
			cur == buf ? "" : " ", value);
	}
	nvram_set(v->name, buf);

}	

int
valid_ipaddr(webs_t wp, char *value, struct variable *v)
{
	struct in_addr netaddr, netmask;
	
	if (!legal_ipaddr(value)) {
		websDebugWrite(wp, "Invalid <b>%s</b> %s: not an IP address<br>",
			  v->longname, value);
		return FALSE;
	}

	if (v->argv) {
		if(!strcmp(v->argv[0],"lan")){
			if(*(value+strlen(value)-2)=='.' && *(value+strlen(value)-1)=='0'){
				websDebugWrite(wp, "Invalid <b>%s</b> %s: not an IP address<br>",
			 		 v->longname, value);
				return FALSE;
			}
		}
		
		else if(!legal_ip_netmask(v->argv[0],v->argv[1],value)){
			(void) inet_aton(nvram_safe_get(v->argv[0]), &netaddr);
			(void) inet_aton(nvram_safe_get(v->argv[1]), &netmask);
			websDebugWrite(wp, "Invalid <b>%s</b> %s: not in the %s/",
				  v->longname, value, inet_ntoa(netaddr));
			websDebugWrite(wp, "%s network<br>", inet_ntoa(netmask));
			return FALSE;
		}
	}

	return TRUE;
}

static void
validate_ipaddr(webs_t wp, char *value, struct variable *v)
{
	if (valid_ipaddr(wp, value, v))
		nvram_set(v->name, value);
}

static void
validate_ipaddrs(webs_t wp, char *value, struct variable *v)
{
	validate_list(wp, value, v, valid_ipaddr);
}

int
valid_merge_ip_4(webs_t wp, char *value, struct variable *v)
{
	char *ipaddr;

	if(atoi(value) == 255){
			websDebugWrite(wp, "Invalid <b>%s</b> %s: out of range 0 - 254 <br>",
				  v->longname, value);
			return FALSE;
	}

	ipaddr = malloc(20);
	sprintf(ipaddr,"%d.%d.%d.%s",get_single_ip(nvram_safe_get("lan_ipaddr"),0),
				     get_single_ip(nvram_safe_get("lan_ipaddr"),1),
				     get_single_ip(nvram_safe_get("lan_ipaddr"),2),
				     value);

	if (!valid_ipaddr(wp, ipaddr, v)){
		free(ipaddr);
		return FALSE;
	}
	
	if(ipaddr) free(ipaddr);
	
	return TRUE;
}

static void
validate_merge_ip_4(webs_t wp, char *value, struct variable *v)
{
	if(!strcmp(value,"")){
		nvram_set(v->name, "0");
		return ;
	}	

	if(valid_merge_ip_4(wp, value, v))
		nvram_set(v->name, value);
}

static void
validate_merge_ipaddrs(webs_t wp, char *value, struct variable *v)
{
	char ipaddr[20];

	get_merge_ipaddr(v->name, ipaddr);

	if(valid_ipaddr(wp, ipaddr, v))
		nvram_set(v->name, ipaddr);
}

static void
validate_merge_mac(webs_t wp, char *value, struct variable *v)
{
	char macaddr[20];

	get_merge_mac(v->name, macaddr);

	if(valid_hwaddr(wp, macaddr, v))
		nvram_set(v->name, macaddr);

}

static void
validate_dns(webs_t wp, char *value, struct variable *v)
{
	char buf[100] = "", *cur = buf;
	char ipaddr[20], ipname[30];
	char *ip;
	int i, j;

	for(j=0; j<3 ; j++){
		strcpy(ipaddr,"");
		for(i=0 ; i<4 ; i++){
			snprintf(ipname,sizeof(ipname),"%s%d_%d",v->name,j,i);
			ip = websGetVar(wp, ipname , NULL);
			if(ip){
				strcat(ipaddr,ip);
				if(i<3)	strcat(ipaddr,".");
			}
			else
				return ;
		}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): ipaddr=[%s]",__FUNCTION__,ipaddr);
#endif
		if(!strcmp(ipaddr,"0.0.0.0"))
			continue;
		if(!valid_ipaddr(wp, ipaddr, v))
			continue;
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", ipaddr);
	}
	nvram_set(v->name, buf);
//junzhao
	//dns_to_resolv(atoi(connection));
}

int
valid_choice(webs_t wp, char *value, struct variable *v)
{
	char **choice;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s):",__FUNCTION__,value);
#endif
	for (choice = v->argv; *choice; choice++) {
		if (!strcmp(value, *choice))
			return TRUE;
	}
	websDebugWrite(wp, "Invalid <b>%s</b> %s: not one of ", v->longname, value);
	for (choice = v->argv; *choice; choice++)
		websDebugWrite(wp, "%s%s", choice == v->argv ? "" : "/", *choice);
	websDebugWrite(wp, "<br>");
	return FALSE;
}

static void
validate_choice(webs_t wp, char *value, struct variable *v)
{
	if (valid_choice(wp, value, v))
		nvram_set(v->name, value);
}

int
valid_range(webs_t wp, char *value, struct variable *v)
{
	int n, start, end;

	n = atoi(value);
	start = atoi(v->argv[0]);
	end = atoi(v->argv[1]);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): start=[%s] end=[%s]",__FUNCTION__,value,v->argv[0],v->argv[1]);
#endif

	if (!ISDIGIT(value,1) || n < start || n > end) {
		websDebugWrite(wp, "Invalid <b>%s</b> %s: out of range %d-%d<br>",
			  v->longname, value, start, end);
		return FALSE;
	}

	return TRUE;
}

static void
validate_range(webs_t wp, char *value, struct variable *v)
{
	char buf[20];
	int range;
	if (valid_range(wp, value, v)){
		range = atoi(value);
		snprintf(buf, sizeof(buf),"%d", range);
		nvram_set(v->name, buf);
	}
}

int
valid_name(webs_t wp, char *value, struct variable *v)
{
	int n, max;

	n = atoi(value);
	max = atoi(v->argv[0]);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): max=[%d]",__FUNCTION__,value,max);
#endif

	if (!ISASCII(value,1)){
		websDebugWrite(wp, "Invalid <b>%s</b> %s: NULL or have illegal characters<br>",
			  v->longname, value);
		return FALSE;
	}
	if (strlen(value) > max) {
		websDebugWrite(wp, "Invalid <b>%s</b> %s: out of range 1-%d characters<br>",
			  v->longname, value,max);
		return FALSE;
	}

	return TRUE;
}
/*modified by zls 2004/11/29*/
#ifdef MULTIUSER_SUPPORT
static void
validate_user(webs_t wp,struct variable *v,char *value)
{
        if(!strcmp(v->name,"http_username")){
                        char cur_userid[5];
                        char var_username[AUTH_MAX];
                        char login_userid[5];
                        char temp_userid[5];
                        strncpy(login_userid,nvram_safe_get("login_userid"),sizeof(login_userid));
                         if(!strcmp(login_userid,"0"))
                           strncpy(temp_userid,websGetVar(wp,"http_username_page",NULL),sizeof(temp_userid));
                        else
                           sprintf(temp_userid,"%s",login_userid);
                        strcpy(cur_userid,temp_userid);
                        if(atoi(cur_userid)==0)
                                sprintf(var_username,"%s",v->name);
                        else
                                sprintf(var_username,"%s%s",v->name,cur_userid);
                        nvram_set(var_username, value);
                 }
        if(!strcmp(v->name,"http_passwd")){
			char cur_userid[5];
                        char var_passwd[AUTH_MAX];
                        char login_userid[5];
                        char temp_userid[5];//websGetVar(wp,"http_username_page",NULL);
                        strncpy(login_userid,nvram_safe_get("login_userid"),sizeof(login_userid));
                        if(!strcmp(login_userid,"0"))//Administrator
                            strncpy(temp_userid,websGetVar(wp,"http_username_page",NULL),sizeof(temp_userid));
                        else
                            sprintf(temp_userid,"%s",login_userid);
                        strcpy(cur_userid,temp_userid);
                        if(atoi(cur_userid)==0)
                                sprintf(var_passwd,"%s",v->name);
                        else
                                sprintf(var_passwd,"%s%s",v->name,cur_userid);
                        nvram_set(var_passwd, value);
                }
}
#endif
//modified by zls 2004/11/22
static void
validate_name(webs_t wp, char *value, struct variable *v)
{
        if (valid_name(wp, value, v)){
                #ifdef MULTIUSER_SUPPORT
                if(!strcmp(v->name,"http_username"))
                        validate_user(wp,v,value);
                 else
		#endif
                        nvram_set(v->name,value);
       }
                                                                                                               
        //junzhao 2004.4.13
        if(!strcmp(v->name, "devname"))
                nvram_set("router_name", value);
}

/* the html always show "d6nw5v1x2pc7st9m"
 * so we must filter it.
 */
/*modified by zls 2004/11/23*/
static void
validate_password(webs_t wp, char *value, struct variable *v)
{
        if (strcmp(value, TMP_PASSWD) && valid_name(wp, value, v))
        {
                #ifdef MULTIUSER_SUPPORT
		if(!strcmp(v->name,"http_passwd")){
                        validate_user(wp,v,value);
                }else
		#endif
                        nvram_set(v->name,value);
                #ifdef DDM_SUPPORT
                if (!strcmp(v->name,"http_passwd"))
                {
                        nvram_set("DDM_pass_flag","1");
                }
                #endif
        }
}

#ifdef SETUP_WIZARD_FT_SUPPORT
static void
validate_setupwizard_http_password(webs_t wp, char *value, struct variable *v)
{
	if (value && value[0] && valid_name(wp, value, v))
	{
		nvram_set(v->name, value);
	}
}
struct variable setupwizard_variables[] = {
	/* for index */
	{ "lan_ipaddr", "LAN IP Address", validate_lan_ipaddr, ARGV("lan"), FALSE },
	//junzhao 2004.4.22 dhcprelay
	{ "dhcpserver_ipaddr", "LAN DHCP Server Address", validate_dhcpserver_ipaddr, ARGV("lan"), FALSE },
	//{ "lan_netmask", "LAN Subnet Mask", validate_choice, ARGV("255.255.255.0","255.255.255.128","255.255.255.192","255.255.255.224","255.255.255.240","255.255.255.248","255.255.255.252"), FALSE },
	{ "wan_dns", "WAN DNS Server", validate_dns, NULL, FALSE },
	{ "lan_proto", "LAN Protocol", validate_choice, ARGV("dhcp", "static", "dhcprelay"), FALSE },
	{ "dhcp_check", "DHCP check", dhcp_check, NULL, FALSE },
	{ "dhcp_start", "DHCP Server LAN IP Address Range", validate_range, ARGV("0","255"), FALSE },
	{ "dhcp_num", "DHCP Users", validate_range, ARGV("1","253"), FALSE },
	{ "dhcp_lease", "DHCP Client Lease Time", validate_range, ARGV("0","99999"), FALSE },
	{ "wan_wins", "WAN WINS Server", validate_merge_ipaddrs, NULL, FALSE },
	/* for password */
	{ "http_username", "Router Username", validate_name, ARGV("63"), TRUE },
	{ "http_passwd", "Router Password", validate_setupwizard_http_password, ARGV("63"), FALSE },

};//End of Setup wizard Variables Array

static void
validate_setupwizard_cgi(webs_t wp)
{
	struct variable *v;
	char *value;

	for (v = setupwizard_variables; v < &setupwizard_variables[STRUCT_LEN(setupwizard_variables)]; v++) {
		value = websGetVar(wp, v->name, NULL);
#ifdef MY_DEBUG
	if(value)
		LOG(LOG_DEBUG,"%s(): [%s]=[%s] nullok=[%d] (%s)",__FUNCTION__,v->name,value ? value :"",v->nullok,!value ? "" : "have");
#endif
		if (!value)
			continue;
		if ((!*value && v->nullok) || !v->validate)
			nvram_set(v->name, value);
		else
			v->validate(wp, value, v);
	}
}
#endif




int
valid_hwaddr(webs_t wp, char *value, struct variable *v)
{
	/* Make exception for "NOT IMPLELEMENTED" string */
	if (!strcmp(value,"NOT_IMPLEMENTED")) 
		return(TRUE);

	/* Check for bad, multicast, broadcast, or null address */
	if (!legal_hwaddr(value)){
		websDebugWrite(wp, "Invalid <b>%s</b> %s: not a legal MAC address<br>",
			  v->longname, value);
		return FALSE;
	}

	return TRUE;
}

static void
validate_hwaddr(webs_t wp, char *value, struct variable *v)
{
	if (valid_hwaddr(wp, value, v))
		nvram_set(v->name, value);
}

static void
validate_hwaddrs(webs_t wp, char *value, struct variable *v)
{
	validate_list(wp, value, v, valid_hwaddr);
}

/* 
 * Variables are set in order (put dependent variables later). Set
 * nullok to TRUE to ignore zero-length values of the variable itself.
 * For more complicated validation that cannot be done in one pass or
 * depends on additional form components or can throw an error in a
 * unique painful way, write your own validation routine and assign it
 * to a hidden variable (e.g. filter_ip).
 */
struct variable variables[] = {
	/* for index */
	{ "lan_ipaddr", "LAN IP Address", validate_lan_ipaddr, ARGV("lan"), FALSE },
	//csh for gre
#ifdef GRE_SUPPORT
	{"gre_config", "GRE Tunnel", validate_gre_config, NULL, FALSE},
#else
#ifdef CLOUD_SUPPORT
	{"gre_config", "GRE Tunnel", validate_gre_config, NULL, FALSE},
	{ "cloud_enable", "Cloud Status", validate_choice, ARGV("0", "1"), FALSE },
#endif
#endif
	// peny 2005.1.6
	{"trap_ipaddr","TRAP IP Address", validate_trap_ipaddr, ARGV("lan"),FALSE},
	//junzhao 2004.4.22 dhcprelay
	{ "dhcpserver_ipaddr", "LAN DHCP Server Address", validate_dhcpserver_ipaddr, ARGV("lan"), FALSE },
	//{ "lan_netmask", "LAN Subnet Mask", validate_choice, ARGV("255.255.255.0","255.255.255.128","255.255.255.192","255.255.255.224","255.255.255.240","255.255.255.248","255.255.255.252"), FALSE },
	{ "router_name", "Routert Name", validate_name, ARGV("255"), TRUE },
	{ "wan_hostname","WAN Host Name", validate_name, ARGV("255"), TRUE },
	{ "wan_domain", "WAN Domain Name", validate_name, ARGV("255"), TRUE },
	{ "wan_ipaddr", "WAN IP Address", validate_wan_ipaddr, NULL, FALSE },
	//{ "wan_ipaddr", "WAN IP Address", validate_merge_ipaddrs, NULL, FALSE },
	//{ "wan_netmask", "WAN Subnet Mask", validate_merge_netmask, FALSE },
	//{ "wan_gateway", "WAN Gateway", validate_merge_ipaddrs, ARGV("wan_ipaddr","wan_netmask"), FALSE },
	{ "wan_proto", "WAN Protocol", validate_choice, ARGV("dhcp", "static", "pppoe", "pptp", "heartbeat"), FALSE },
	{ "ntp_server", "NTP Server", NULL, NULL, TRUE },  // not use 
	{ "ntp_mode", "NTP Mode", validate_choice, ARGV("manual","auto"), TRUE },
	{ "daylight_time", "Daylight", validate_choice, ARGV("0", "1"), TRUE },
	/* sntp kirby 2004/07/21 */
	{ "timer_interval", "Time Interval", validate_range, ARGV("60", "9999"), FALSE },
	/* da_f 2005.6.16 modify */
	{ "time_zone", "Time Zone", validate_choice, ARGV("-12 1 0","-11 1 0","-10 1 0","-09 1 1","-08 1 1","-07 1 0","-07 2 1","-06 1 0","-06 2 1","-05 1 0","-05 2 1","-04 1 0","-04 2 1","-03.5 1 1","-03 1 0","-03 2 1","-02 1 0","-01 1 2","+00 1 0","+00 2 2","+01 1 0","+01 2 2","+02 1 0","+02 2 2","+03 1 0","+04 1 0","+05 1 0","+05.5 1 0","+06 1 0","+07 1 0","+08 1 0","+08 2 0","+09 1 0","+09.5 0 0","+10 1 0","+10 2 4","+11 1 0","+12 1 0","+12 2 4"), FALSE },
	//{ "pptp_server_ip", "WAN Gateway", validate_merge_ipaddrs, ARGV("wan_ipaddr","wan_netmask"), FALSE },
	{ "ppp_username", "Username", validate_name, ARGV("63"), FALSE },
	{ "ppp_passwd", "Password", validate_password, ARGV("63"), TRUE },
	{ "ppp_keepalive", "Keep Alive", validate_choice, ARGV("0", "1"), FALSE },
	{ "ppp_demand", "Connect on Demand", validate_choice, ARGV("0", "1"), FALSE },
	{ "ppp_idletime", "Max Idle Time", validate_range, ARGV("1", "9999"), FALSE },
	{ "ppp_redialperiod", "Redial Period", validate_range, ARGV("1", "9999"), FALSE },
	{ "ppp_service", "Service Name", validate_name, ARGV("63"), TRUE },	// 2003-03-19 by honor
	{ "ppp_static", "Enable /Disable Static IP", validate_choice, ARGV("0", "1"), TRUE },
	{ "ppp_static_ip", "Static IP", validate_merge_ipaddrs, NULL, FALSE },
#ifdef MPPPOE_SUPPORT
	/*{ "mpppoe_enable", "Multi PPPoE", validate_choice, ARGV("0","1"), FALSE },
	{ "mpppoe_dname", "Match Doamin Name", validate_name, ARGV("63"), FALSE },
	{ "ppp_username_1", "Username", validate_name, ARGV("63"), FALSE },
	{ "ppp_passwd_1", "Password", validate_password, ARGV("63"), TRUE },
	{ "ppp_keepalive_1", "Keep Alive", validate_choice, ARGV("0", "1"), FALSE },
	{ "ppp_demand_1", "Connect on Demand", validate_choice, ARGV("0", "1"), FALSE },
	{ "ppp_idletime_1", "Max Idle Time", validate_range, ARGV("1", "9999"), FALSE },
	{ "ppp_redialperiod_1", "Redial Period", validate_range, ARGV("1", "9999"), FALSE },
	{ "ppp_service_1", "Service Name", validate_name, ARGV("63"), TRUE },	// 2003-03-19 by honor*/
#endif
	/* for index, dhcp */
	{ "wan_dns", "WAN DNS Server", validate_dns, NULL, FALSE },
	/* for dhcp */
	//{ "lan_proto", "LAN Protocol", validate_choice, ARGV("dhcp", "static"), FALSE },
	//junzhao 2004.4.22 dhcprelay
	{ "auto_search_ip", "AUTO SEARCH LOCAL IP", validate_choice, ARGV("0","1"), FALSE }, // add by lzh;			
	{ "lan_proto", "LAN Protocol", validate_choice, ARGV("dhcp", "static", "dhcprelay"), FALSE },
	{ "dhcp_check", "DHCP check", dhcp_check, NULL, FALSE },
	{ "lan_detect_dhcp", "LAN DETECT DHCP", validate_choice, ARGV("0","1"),FALSE }, //add by lzh;
	{ "dhcp_start", "DHCP Server LAN IP Address Range", validate_range, ARGV("0","255"), FALSE },
	//{ "dhcp_start", "DHCP Server LAN IP Address Range", validate_merge_ip_4, NULL, FALSE },
	{ "dhcp_num", "DHCP Users", validate_range, ARGV("1","253"), FALSE },
	{ "dhcp_lease", "DHCP Client Lease Time", validate_range, ARGV("0","99999"), FALSE },
	{ "wan_wins", "WAN WINS Server", validate_merge_ipaddrs, NULL, FALSE },
	/* for password */
	{ "http_username", "Router Username", validate_name, ARGV("63"), TRUE },
	{ "http_passwd", "Router Password", validate_password, ARGV("63"), TRUE },

	{ "upnp_enable", "UPnP", validate_choice, ARGV("0", "1"), FALSE },
//	{ "wt82_enable", "WT82", validate_choice, ARGV("0", "1"), FALSE },//guohong
	{ "tr064_enable", "tr064", validate_choice, ARGV("0", "1"), FALSE },//leijun
	{"snmp_enable","SNMP",validate_choice,ARGV("0","1"),FALSE},//by songtao
	{"get_auth","Get Auth",validate_name,ARGV("255"),TRUE},
        {"set_auth","Get Auth",validate_name,ARGV("255"),TRUE},
        {"devname","Device Name",validate_name,ARGV("255"),TRUE},
	/* for log */
	{ "log_level", "Connection Logging", validate_range, ARGV("0", "3"), FALSE },
	{ "log_enable", "Access log", validate_choice, ARGV("0", "1"), FALSE },
	{ "log_ipaddr", "Log server ip", validate_range, ARGV("0","255"), FALSE },
#ifdef SYSLOG_SUPPORT
	{ "log_ipaddr", "Log server ip", validate_range, ARGV("0","255"), FALSE },
	{ "log_settings", "Log settings", validate_log_settings, NULL, FALSE },
	{ "log_show_all", "Show all log messages", validate_choice, ARGV("0", "1"), FALSE },
#endif
	/* for filter */
#ifndef PVC_PER_SUPPORT
	/*cjg:remote http password*/
	{ "http_remote_username", "Router Username", validate_name, ARGV("63"), TRUE },
	{ "http_remote_passwd", "Router Password", validate_password, ARGV("63"), TRUE },
	{ "filter", "Firewall Protection", validate_choice, ARGV("on", "off"), FALSE },
	{ "block_wan", "Block WAN Request", validate_choice, ARGV("0", "1"), FALSE },
	{ "block_proxy", "Block Proxy", validate_choice, ARGV("0", "1"), FALSE },
	{ "block_java", "Block Java", validate_choice, ARGV("0", "1"), FALSE },
	{ "block_activex", "Block ActiveX", validate_choice, ARGV("0", "1"), FALSE },
	{ "block_cookie", "Block Cookie", validate_choice, ARGV("0", "1"), FALSE },
	{ "remote_management", "Remote Management", validate_choice, ARGV("0", "1"), FALSE },
#ifdef HTTPS_SUPPORT
	{ "remote_mgt_https", "Remote Management use https", validate_choice, ARGV("0", "1"), FALSE },
#endif
	{ "http_wanport", "Router WAN Port", validate_range, ARGV("0", "65535"), TRUE },

	{ "dmz_enable", "DMZ enable", validate_choice, ARGV("0", "1"), FALSE  },
	{ "dmz_ipaddr", "DMZ LAN IP Address", validate_range, ARGV("0","255"), FALSE  },
	{"nat_enable", "", validate_nat_func, ARGV("0", "1"), FALSE },
#else

	{ "http_remote_username", "Router Username", validate_name_manage, ARGV("63"), TRUE },
	{ "http_remote_passwd", "Router Password", validate_password_manage, ARGV("63"), TRUE },
	/* for filter */
	{ "filter", "Firewall Protection", validate_choice_firewall, ARGV("on", "off"), FALSE },
	{ "block_wan", "Block WAN Request", validate_choice_firewall, ARGV("0", "1"), FALSE },//revised by yl
	{ "block_proxy", "Block Proxy", validate_choice_firewall, ARGV("0", "1"), FALSE },
	{ "block_java", "Block Java", validate_choice_firewall, ARGV("0", "1"), FALSE },
	{ "block_activex", "Block ActiveX", validate_choice_firewall, ARGV("0", "1"), FALSE },
	{ "block_cookie", "Block Cookie", validate_choice_firewall, ARGV("0", "1"), FALSE },
	{ "remote_management", "Remote Management", validate_choice_manage, ARGV("0", "1"), FALSE },
#ifdef HTTPS_SUPPORT
	#ifdef PVC_PER_SUPPORT
		{ "remote_mgt_https", "Remote Management use https", validate_choice_manage, ARGV("0", "1"), FALSE }, //revised by yl
	#else
		{ "remote_mgt_https", "Remote Management use https", validate_choice, ARGV("0", "1"), FALSE }, 
	#endif
#endif
	{ "http_wanport", "Router WAN Port", validate_range_manage, ARGV("0", "65535"), TRUE },
	{ "dmz_enable", "DMZ enable", validate_choice_dmz, ARGV("0", "1"), FALSE  }, //modified by yl
	{ "dmz_ipaddr", "DMZ LAN IP Address", validate_range_dmz, ARGV("0","255"), FALSE  },

#endif

#ifdef PORT_TRIGGER_SUPPORT
	   /* for port trigger */
	{ "port_trigger", "TCP/UDP Port Trigger", validate_port_trigger, NULL, FALSE },
#endif

#ifdef FIREWALL_LEVEL_SUPPORT
	{ "filter_level", "Security Level", validate_choice, ARGV("0", "1", "2", "3"), FALSE },
	{ "filter_inb_policy", "Filter Inbound", validate_filter_inb_policy, NULL, FALSE },
	{ "filter_inb_mix", "Filter Mix rule", validate_filter_inb_mix, NULL, FALSE },
#endif
	{ "filter_policy", "Filter", validate_filter_policy, NULL, FALSE },
	{ "filter_ip_value", "TCP/UDP IP Filter", validate_filter_ip_grp, NULL, FALSE },
	{ "filter_port", "TCP/UDP Port Filter", validate_filter_port, NULL, FALSE },
	{ "filter_dport_value", "TCP/UDP Port Filter", validate_filter_dport_grp, NULL, FALSE },
	{ "blocked_service", "TCP/UDP Port Filter", validate_blocked_service, NULL, FALSE },
	{ "filter_mac_value", "TCP/UDP MAC Filter", validate_filter_mac_grp, NULL, FALSE },
	{ "filter_web", "Website Filter", validate_filter_web, NULL, FALSE },

	{ "multicast_pass", "Multicast Pass Through", validate_choice, ARGV("0", "1"), FALSE },
	{ "ipsec_pass", "IPSec Pass Through", validate_choice, ARGV("0", "1"), FALSE },
	{ "pppoe_pass", "PPPoE Pass Through", validate_choice, ARGV("0", "1"), FALSE },
	{ "pptp_pass", "PPTP Pass Through", validate_choice, ARGV("0", "1"), FALSE },
	{ "l2tp_pass", "L2TP Pass Through", validate_choice, ARGV("0", "1"), FALSE },

#ifdef CONFIG_DSL_MODULA_SEL
	{ "dsl_modulation", "DSL Modulation", validate_choice, ARGV("MMODE", "T1413", "GDMT", "GLITE", "AD2MOD", "A2PMOD"), FALSE },
#endif

	{ "remote_upgrade", "Remote Upgrade", validate_choice, ARGV("0", "1"), FALSE },
	{ "mtu_enable", "MTU enable", validate_choice, ARGV("0", "1"), FALSE },
	{ "wan_mtu", "WAN MTU", validate_range, ARGV("576","1500"), FALSE },

	{ "forward_port", "TCP/UDP Port Forward", validate_forward_proto, NULL, FALSE },
//#ifdef UPNP_FORWARD_SUPPORT modified by leijun 0313 22:00
	{ "forward_upnp", "TCP/UDP UPnP Forward", validate_forward_upnp, NULL, FALSE },
//#endif
#ifdef SPECIAL_FORWARD_SUPPORT
	{ "forward_spec", "TCP/UDP special Forward", validate_forward_spec, NULL, FALSE },
#endif
#ifdef ADVANCED_FORWARD_SUPPORT//add by zhs
        {"advanced_forwarding_config","Advanced Forwarding",validate_advanced_forwarding,NULL,FALSE},
#endif//end by zhs
	//cjg:2004-3-7:PM
	{"ping_config", "Diagnostic Ping Test", validate_ping_config, NULL, FALSE},
	{"email_config", "Email Alert", validate_email_config, NULL, FALSE},
	/* for static route */
	
	{ "static_route", "Static Route", validate_static_route, NULL, FALSE },
	/* for dynamic route */
//	{ "wk_mode", "Working Mode", validate_dynamic_route, ARGV("gateway", "router"), FALSE }, //modified by leijun
	//{ "dr_setting", "Dynamic Routing", validate_choice, ARGV("0", "1", "2", "3"), FALSE },
	//{ "dr_lan_tx", "Dynamic Routing LAN TX", validate_choice, ARGV("0","1 2"), FALSE },
	//{ "dr_lan_rx", "Dynamic Routing LAN RX", validate_choice, ARGV("0","1 2"), FALSE },
	//{ "dr_wan_tx", "Dynamic Routing WAN TX", validate_choice, ARGV("0","1 2"), FALSE },
	//{ "dr_wan_rx", "Dynamic Routing WAN RX", validate_choice, ARGV("0","1 2"), FALSE },


	/* for MAC Clone */
	{ "mac_clone_enable", "User define WAN MAC Address", validate_choice, ARGV("0","1"), TRUE },
	{ "wan_def_hwaddr", "User define WAN MAC Address", validate_merge_mac, NULL, TRUE },
	/* for upgrade */
	{ "upgrade_enable", "Tftp upgrade", validate_choice, ARGV("0", "1"), FALSE },
	/* for wireless */	
//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "wl_enable", "Enable Wireless", validate_choice, ARGV("0","1"), TRUE },
	{ "wl_ssid", "Network Name (SSID)", validate_name, ARGV("32"), TRUE },
        { "wl_closed", "Network Type", validate_choice, ARGV("0", "1"), FALSE },
	{ "wl_country", "Country", validate_choice, ARGV("Worldwide", "Thailand", "Israel", "Jordan", "China", "Japan", "USA", "Europe", "USA Low", "Japan High", "All"), FALSE },
        { "wl_mode", "AP Mode", validate_choice, ARGV("ap", "sta", "wds"), FALSE },
        { "wl_lazywds", "Bridge Restrict", validate_choice, ARGV("0", "1"), FALSE },
        { "wl_wds", "Remote Bridges", validate_hwaddrs, NULL, TRUE },
        /* Order and index matters for WEP keys so enumerate them separately */
        { "wl_WEP_key", "Network Key Index", validate_wl_wep_key, NULL, FALSE },
        //{ "wl_passphrase", "Network Passphrase", validate_name, ARGV("20"), FALSE },
        //{ "wl_key", "Network Key Index", validate_range, ARGV("1","4"), FALSE },
        //{ "wl_key1", "Network Key 1", validate_wl_key, NULL, TRUE },
        //{ "wl_key2", "Network Key 2", validate_wl_key, NULL, TRUE },
        //{ "wl_key3", "Network Key 3", validate_wl_key, NULL, TRUE },
        //{ "wl_key4", "Network Key 4", validate_wl_key, NULL, TRUE },
        //{ "wl_wep_bit", "WEP Mode", validate_choice, ARGV("64", "128"), FALSE },
        { "wl_wep", "WEP Mode", validate_wl_wep, ARGV("off", "on", "restricted","tkip","aes","tkip+aes"), FALSE },
        { "wl_auth", "Authentication Mode", validate_wl_auth, ARGV("0", "1"), FALSE },
        { "wl_macmode1", "MAC Restrict Mode", validate_macmode, NULL , FALSE },
        //{ "wl_mac", "Allowed MAC Address", validate_hwaddrs, NULL, TRUE },
	{ "wl_radio", "Radio Enable", validate_choice, ARGV("0", "1"), FALSE }, //from 11.9
        { "wl_mac_list", "Filter MAC Address", validate_wl_hwaddrs, NULL, FALSE },
        //{ "wl_active_mac", "Active MAC Address", validate_wl_active_mac, NULL, FALSE },
        { "wl_channel", "802.11g Channel", validate_range, ARGV("1","14"), FALSE },
        { "wl_rate", "802.11g Rate", validate_choice, ARGV("0", "1000000", "2000000", "5500000", "6000000", "9000000", "11000000", "12000000", "18000000", "24000000", "36000000", "48000000", "54000000"), FALSE },
        { "wl_rateset", "802.11g Supported Rates", validate_choice, ARGV("all", "default","12"), FALSE },
        { "wl_frag", "802.11g Fragmentation Threshold", validate_range, ARGV("256", "4096"), FALSE },
        { "wl_rts", "802.11g RTS Threshold", validate_range, ARGV("0", "4096"), FALSE },
        { "wl_dtim", "802.11g DTIM Period", validate_range, ARGV("1", "255"), FALSE },
        { "wl_bcn", "802.11g Beacon Interval", validate_range, ARGV("1", "65535"), FALSE },
        { "wl_gmode", "802.11g mode", validate_choice, ARGV("-1", "0", "1", "2", "4", "5"), FALSE },
	{ "wl_gmode_protection", "54g Protection", validate_choice, ARGV("off", "auto"), FALSE },
	{ "wl_frameburst", "Frame Bursting", validate_choice, ARGV("off", "on"), FALSE },
	{ "wl_plcphdr", "Preamble Type", validate_choice, ARGV("long", "short"), FALSE },
	{ "wl_phytype", "Radio Band", validate_choice, ARGV("a", "b", "g"), TRUE },
	/* WPA parameters */
	{ "wl_wpa_psk", "WPA Pre-Shared Key", validate_wpa_psk, ARGV("64"), TRUE },
	{ "wl_wpa_gtk_rekey", "WPA GTK Rekey Timer", validate_range, ARGV("0","99999"), TRUE },
	{ "wl_radius_ipaddr", "RADIUS Server", validate_merge_ipaddrs, NULL, TRUE },
	{ "wl_radius_port", "RADIUS Port", validate_range, ARGV("0", "65535"), FALSE },
	{ "wl_radius_key", "RADIUS Shared Secret", validate_name, ARGV("255"), TRUE },
	{ "wl_auth_mode", "Auth Mode", validate_auth_mode, ARGV("disabled", "radius", "wpa", "psk"), FALSE },
	{ "security_mode", "Security Mode", validate_security_mode, ARGV("disabled", "radius", "wpa", "psk","wep"), FALSE },
	/* MUST leave this entry here after all wl_XXXX variables */
	{ "wl_unit", "802.11 Instance", wl_unit, NULL, TRUE },
	{"wl_management_enable", "Management via wlan", validate_choice, ARGV("0", "1"), FALSE },
#endif	
#ifdef WL_STA_SUPPORT
	{ "wl_ap_ssid", "SSID of associating AP", validate_name, ARGV("32"), TRUE },
	{ "wl_ap_ip", "Default IP of associating AP", validate_merge_ipaddrs, NULL, TRUE },
#endif

	/* for DDNS */
	//{ "ddns_enable", "DDNS", validate_choice, ARGV("0", "1"), FALSE },
	//{ "ddns_username", "DDNS username", validate_name, ARGV("63"), FALSE },
	//{ "ddns_passwd", "DDNS password", validate_password, ARGV("63"), FALSE },
	//{ "ddns_hostname", "DDNS hostname", validate_name, ARGV("255"), TRUE },
        //{ "ddns_server", "DDNS server", validate_choice,ARGV("ath.cx","dnsalias.com","dnsalias.net","dnsalias.org","dyndns.biz","dyndns.info","dyndns.org","dyndns.tv","gotdns.com","gotdns.org","homedns.org","homeftp.net","homeftp.org","homeip.net","homelinux.com","homelinux.net","homelinux.org","homeunix.com","homeunix.net","homeunix.org","kicks-ass.net","kicks-ass.org","merseine.nu","mine.nu","serveftp.net"), FALSE },
#ifdef AOL_SUPPORT
	/* for AOL */
	{ "aol_block_traffic2", "AOL Parental Control", validate_choice, ARGV("0","1"), FALSE },
#endif
#ifdef HSIAB_SUPPORT
	//{ "hsiab_mode", "HSIAB mode", validate_choice, ARGV("1","0"), FALSE },
	//{ "hsiab_provider", "HSIAB provider", validate_choice, ARGV("0","1","2"), FALSE },
	//{ "hsiab_sync", "Sync Config", validate_choice, ARGV("0","1"), FALSE },
#endif
#ifdef HEARTBEAT_SUPPORT
	{ "hb_server_ip", "Heart Beat Server", validate_merge_ipaddrs, NULL, FALSE },
#endif
#ifdef PARENTAL_CONTROL_SUPPORT
	{ "artemis_enable", "Parental Control", validate_choice, ARGV("0","1"), FALSE },
#endif
	/* Internal variables */
	{ "os_server", "OS Server", NULL, NULL, TRUE },
	{ "stats_server", "Stats Server", NULL, NULL, TRUE },
	/* for radius */
	
	/* zhangbin for ipsec */
	{"tunnel_entry","Tunnel Entry",NULL,NULL,FALSE},
	{"entry_config","Entry Config",validate_entry_config,NULL,FALSE},
	{"ltype_config","LType Config",validate_ltype_config,NULL,FALSE},
	{"rtype_config","RType Config",validate_rtype_config,NULL,FALSE},
	{"sg_config","Sg Config",validate_sg_config,NULL,FALSE},

#ifdef CLOUD_SUPPORT
	{"lid_config","Local ID Type Config",validate_lid_config,NULL,FALSE},
	{"rid_config","Remote ID Type Config",validate_rid_config,NULL,FALSE},
#endif

	{"keytype_config","KeyType Config",validate_keytype_config,NULL,FALSE},
	{"ipsecadv_config","IPSec Advanced Config",validate_ipsecadv_config,NULL,FALSE},

	/* zhangbin for qos*/
	{"qosSW", "QoS Enable", NULL, NULL, FALSE},
#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
	{"qos_config","QoS Config",validate_qos_config,NULL,FALSE},
#endif
	//add by lijunzhao
	/* for multi pvc */
	{"wan_connection", "PVC Connection", NULL, NULL, FALSE},
	{"mtu_config", "WAN MTU VALUE", validate_mtu_config, NULL, FALSE},
	{"macclone_config", "WAN MAC CLONE", validate_macclone_config, NULL, FALSE},
	{"vcc_config", "PVC Configuration", validate_vcc_config, NULL, FALSE},
	{"pppoe_config", "PPPOE Configuration", validate_pppoe_config, NULL, FALSE},
	{"pppoa_config", "PPPOA Configuration", validate_pppoa_config, NULL, FALSE},
	{"bridged_config", "BRIDGED Configuration", validate_bridged_config, NULL, FALSE},
	{"routed_config", "ROUTED Configuration", validate_routed_config, NULL, FALSE},
	//junzhao 2004.6.1
	{"pvc_selection_config", "PVC Selection", validate_pvc_selection_config, NULL, FALSE},
	{"active_connection_selection", "PVC Selection", NULL, NULL, FALSE},

#ifdef CLIP_SUPPORT
	{"clip_config", "CLIP Configuration", validate_clip_config, NULL, FALSE},
#endif	
#ifdef MPPPOE_SUPPORT
	{"mpppoe_config", "MPPPoE Configuration", validate_mpppoe_config, NULL, FALSE},
#endif
#ifdef IPPPOE_SUPPORT
	{"ipppoe_config", "IP_PPPoE Configuration", validate_ipppoe_config, NULL, FALSE},
#endif
#ifdef CWMP_SUPPORT
        {"cwmp_enable","TR-069",validate_choice,ARGV("0","1"),FALSE},
        {"cpe_serialnumber", "TR-069 CPE SERIAL NUMBER", validate_name, ARGV("63"), TRUE },
        {"cpe_password", "TR-069 CPE PASSWORD", validate_password, ARGV("63"), TRUE },
        {"acs_url", "TR-069 ACS URL", validate_name, ARGV("63"), TRUE },
        {"cpe_username", "TR-069 CPE USERNAME", validate_name, ARGV("63"), TRUE },
        {"acs_username", "TR-069 ACS USERNAME", validate_name, ARGV("63"), TRUE },
        {"acs_password", "TR-069 ACS PASSWORD", validate_password, ARGV("63"), TRUE },
	//add by johnny for cisco test 2005-08-26
        {"cpe_url", "Connection Request URL", validate_name, ARGV("63"), TRUE },
        {"cpe_periodicinform_interval", "TR-069 inform interval", validate_range, ARGV("0", "86400"), TRUE },
        {"cpe_periodicinform_enable", "TR-069 inform enable", validate_choice, ARGV("0", "1"), TRUE },
#endif
	//add by leijun for dhcp reserved ip 2004-05-20
	{"dhcp_reserved_ip_list", "dhcp reserved ip", validate_dhcp_reservedIp, NULL, FALSE},

	//add by leijun for advance routing
	{"nat_enable", "", validate_nat_func, ARGV("0", "1"), FALSE },
	{"rip_enable", "", validate_dynamic_route, ARGV("0", "1"), FALSE },
	{"share_wanip", "share wan ip", validate_choice, ARGV("0", "1"), FALSE },
	{"single_pc_mode", "single pc mode", validate_choice,ARGV("0", "1"), FALSE },
	{"share_wanip_comm", "share wan ip comm", validate_choice,ARGV("0", "1"), FALSE },
#ifdef STB_SUPPORT
        {"stbentry_config", "STB Entry Configuration", validate_stbentry_config, NULL, FALSE},
        {"telus_config", "TELUS Server Configuration", validate_telus_config, NULL, FALSE},

#endif

	// add for IGMP Proxy 2004-10-10
#ifdef IGMP_PROXY_SUPPORT
	{"igmp_proxy_enable", "igmp proxy enable", NULL, NULL, FALSE},
	{"igmp_proxy_channel", "igmp proxy channel", NULL, NULL, FALSE},
	{"igmp_proxy_config", "igmp proxy config", validate_igmp_proxy_config, NULL, FALSE},
#endif

	//  FOR IP Range Restricted 2004-11-17
	{"ip_range_config", "TP Range Config", validate_ip_range_setting, NULL, FALSE},
	
	//CSH for SNMP
	{"snmp_ip_limit", "Snmp Management", validate_snmp_config, NULL, FALSE},

        // peny 2005.1.6
	//Lai modify 2005-07-25
	{"snmpv3_support", "Snmpv3 version", validate_snmpv3_support, NULL, FALSE},
	
#ifdef HTTPS_SUPPORT
	//Lai modify 2005-07-04
	{"authprotocol", "Auth protocol", validate_authproto, NULL, FALSE},
	//Lai add 2005-07-04
	{"privprotocol", "Priv protocol", validate_privproto, NULL, FALSE},
#endif
        {"v3authpasw","Snmpv3 auth password",validate_password,ARGV("63"),TRUE},        {"v3privpasw","Snmpv3 priv password",validate_password,ARGV("63"),TRUE},
        {"v3rwuser","Snmpv3 user",validate_name,ARGV("255"),TRUE},

	/*  for Restricted IP Range by jackel  2004-11-8*/
	{"range_start_ip", "Range Start IP", validate_ip_range_setting, NULL, FALSE},
	{"range_end_ip", "Range End IP", validate_ip_range_setting, NULL, FALSE},
	{"range_ip", "Range IP", validate_ip_range_setting, NULL, FALSE},

#ifdef CLI_SUPPORT	
	//zhangbin 2005.1.13
	{"telnet_enable", "Telnet for CLI", validate_choice, ARGV("0", "1"), FALSE },
#endif
	//zhangbin 2005.3.25
	{"dns_proxy", "DNS Proxy", validate_choice, ARGV("0", "1"), FALSE },

	/* kirby add for FFT */
#ifdef SECLAN_SUPPORT	
        { "seclan_enable", "Secondary Lan Interface Enable", validate_choice, ARGV("0", "1"), TRUE },
        { "seclan_ipaddr", "Secondary LAN IP Address", validate_seclan_ipaddr, ARGV("lan"), FALSE },
#endif
#ifdef IPCP_NETMASK_SUPPORT	
        { "ipcp_netmask_enable", "IPCP/IPCPUnumbered Enable", validate_choice, ARGV("0", "1"), TRUE },
        /* kirby add */
#endif

	/* da_f 2005.3.18 add */
#ifdef NOMAD_SUPPORT
	{ "save_vpn_account", "Save VPN Client Account", validate_vpn_account, NULL, FALSE },
	{ "vpn_cli_active_stat", "Save VPN Client Active status", validate_vpn_cli_active, NULL, FALSE },
	{ "vpn_cli_disconn", "Disconnect the VPN Client connection", validate_vpn_cli_disconn, NULL, FALSE },
#endif
};//End of Variables Array

static void
validate_cgi(webs_t wp)
{
	struct variable *v;
	char *value;

	/* Validate and set variables in table order */
	for (v = variables; v < &variables[STRUCT_LEN(variables)]; v++) {
		value = websGetVar(wp, v->name, NULL);
#ifdef MY_DEBUG
	if(value)
		LOG(LOG_DEBUG,"%s(): [%s]=[%s] nullok=[%d] (%s)",__FUNCTION__,v->name,value ? value :"",v->nullok,!value ? "" : "have");
#endif
		if (!value)
			continue;
		if ((!*value && v->nullok) || !v->validate)
			nvram_set(v->name, value);
		else
			v->validate(wp, value, v);
	}
}

enum {
	NOTHING,
	REBOOT,
	RESTART,
	SERVICE_RESTART,
	REFRESH,
};

struct gozila_action gozila_actions[] = {
	/* bellow for setup page */
	{ "index",		"clone_mac",	"",		1,	REFRESH,		clone_mac},
	//csh for gre
#ifdef GRE_SUPPORT
	{"GRE_tunnel",		"delete",	"",		1,	REFRESH,		gre_delete_tunnel},
#endif
	//junzhao add for apply button in index.asp
	{ "index",		"apply",	"",		20,	RESTART,		NULL},
	//add by junzhao 2004.3.10
	{ "index",		"savebuf",	"",		0,	REFRESH,	save_vcc_tmpbuf},
#ifdef MPPPOE_SUPPORT	
	//{ "index",		"savepppoebuf",	"",		0,	REFRESH,	save_vcc_pppoe_tmpbuf},
#endif	
	{ "WanMAC",		"clone_mac",	"",		1,	REFRESH,		clone_mac},	// for cisco style
	{ "WanMAC",		"clone_mac",	"",		1,	REFRESH,		clone_mac},	// for cisco style
	{ "DHCPTable",		"delete",	"",		2,	REFRESH,		delete_leases},
	{ "Status",		"release",	"",		2,	REFRESH,		dhcp_release},
	{ "Status",             "renew",        "",             3,      REFRESH,                dhcp_renew},
	{ "Status",		"Connect",	"start_pppoe",	1,	RESTART,		NULL},
	//junzhao
	{ "Status_Router",	"connect",	"connect_ppp",   30,	SERVICE_RESTART,  NULL},	// for cisco style
	{ "Status_Router",	"disconnect",  "disconnect_ppp", 6,	SERVICE_RESTART,  NULL},	// for cisco style
	{ "Status_Router",	"release",	"dhcp_release",	 6,	SERVICE_RESTART,  NULL},	// for cisco style
	{ "Status_Router",      "renew",        "dhcp_renew",    20,     SERVICE_RESTART,  NULL},	// for cisco style
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	{ "Status_Router",	"connect_multippp",	"connect_multi_ppp",   30,	SERVICE_RESTART,  NULL},	// for cisco style
	{ "Status_Router",	"disconnect_multippp",  "disconnect_multi_ppp", 4,	SERVICE_RESTART,  NULL},	// for cisco style
#endif
//cjg:2004-3-7:PM
	{ "Diagnostics", "startping" , "start_ping",   5,        NOTHING, /*NULL*/diag_ping_start},
	// zhangbin for ipsec
	{ "ipsec",		"connect",	"",		5,	REFRESH,		connect_ipsec},
	{ "ipsec",		"disconnect",	"",		5,	REFRESH,		disconnect_ipsec},
	{ "ipsec",		"delete",	"ipsec",	1,	REFRESH,	ipsec_delete_tunnel},
	
#ifdef CLOUD_SUPPORT

	{ "Cloud",		"connect",	"",		5,	REFRESH,		connect_ipsec},
	{ "Cloud",		"disconnect",	"",		5,	REFRESH,		disconnect_ipsec},

#endif
	/*--------songtao-----------*/
  	{"Log_incoming",        "pagerefresh",  "",     1,      REFRESH,
	  syslog_refresh},
	{"Log_incoming",        "pagepre",      "",     1,      REFRESH,
		syslog_pagepre},
	{"Log_incoming",        "pagenext",     "",     1,      REFRESH,
		syslog_pagenext},
	{"Log_incoming",        "pageclear",    "",     1,      REFRESH,
		syslog_clear},
		
	/*----------lzh-------------*/
	{"Management",  "upnp_select_connect",  "forward_upnp",  2, SERVICE_RESTART,  NULL},
		
	/*-----------songtao--------*/						  

	{ "Status",		"Disconnect",	"stop_pppoe",	2,	SERVICE_RESTART,	stop_ppp},
	{ "Status",		"Connect_pppoe",	"start_pppoe",	1,	RESTART,		NULL},
	{ "Status",		"Disconnect_pppoe",	"stop_pppoe",	2,	SERVICE_RESTART,	stop_ppp},
	{ "Status",		"Connect_pptp",	"start_pptp",	1,	RESTART,		NULL},
	{ "Status",		"Disconnect_pptp",	"stop_pptp",	2,	SERVICE_RESTART,	stop_ppp},
	{ "Status",		"Connect_heartbeat",	"start_heartbeat",	1,	RESTART,		NULL},
	{ "Status",		"Disconnect_heartbeat",	"stop_heartbeat",	2,	SERVICE_RESTART,	stop_ppp},
	{ "Status_Router",	"Disconnect",	"stop_pppoe",	2,	SERVICE_RESTART,	stop_ppp},	// for cisco style
	{ "Status_Router",	"Connect_pppoe",	"start_pppoe",	1,	RESTART,		NULL},	// for cisco style
	{ "Status_Router",	"Disconnect_pppoe",	"stop_pppoe",	2,	SERVICE_RESTART,	stop_ppp},	// for cisco style
	{ "Status_Router",	"Connect_pptp",	"start_pptp",	1,	RESTART,		NULL},	// for cisco style
	{ "Status_Router",	"Disconnect_pptp",	"stop_pptp",	2,	SERVICE_RESTART,	stop_ppp},	// for cisco style
	{ "Status_Router",	"Connect_heartbeat",	"start_heartbeat",	1,	RESTART,		NULL},	// for cisco style
	{ "Status_Router",	"Disconnect_heartbeat",	"stop_heartbeat",	2,	SERVICE_RESTART,	stop_ppp},	// for cisco style
	{ "Filters",		"save",		"filters",	1,	SERVICE_RESTART,	save_policy},
	{ "Filters",		"delete",	"filters",	1,	SERVICE_RESTART,	single_delete_policy},
	{ "FilterSummary",	"delete",	"filters",	1,	SERVICE_RESTART,	summary_delete_policy},
#ifdef FIREWALL_LEVEL_SUPPORT
	{ "Filters_inb",	"save",		"filters_inb",	1,	SERVICE_RESTART,save_inb_policy},
	{ "Filters_inb",	"delete",	"filters_inb",	1,	SERVICE_RESTART,single_delete_inb_policy},
	{ "Filter_Inb_Summary",	"delete",	"filters_inb",	1,	SERVICE_RESTART,summary_delete_inb_policy},
#endif
//	{ "Routing",		"del",		"static_route_del",	1,	SERVICE_RESTART,	delete_static_route},
//	{ "RouteStatic",	"del",		"static_route_del",	1,	SERVICE_RESTART,	delete_static_route},

	{ "Routing",		"del",		"static_route_del",	1,	REFRESH,	delete_static_route},
	{ "RouteStatic",	"del",		"static_route_del",	1,	REFRESH,	delete_static_route},

//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "WL_WEPTable",	"key_64",	"",		1,	REFRESH,		generate_key_64},
	{ "WL_WEPTable",	"key_128",	"",		1,	REFRESH,		generate_key_128},
	{ "WL_WPATable",	"key_64",	"",		1,	REFRESH,		generate_key_64},
	{ "WL_WPATable",	"key_128",	"",		1,	REFRESH,		generate_key_128},
	{ "WL_ActiveTable",	"add_mac",	"",		1,	REFRESH,		add_active_mac},
	//wwzh 2004-03-07
	{ "Wireless_Connected",	"add_mac",	"",		1,	REFRESH,		add_active_mac},
#endif
	{ "Port_Services",	"save_services",	"filters",		2,	SERVICE_RESTART,		save_services_port},
#ifdef EMI_TEST
	{ "emi_test",		"exec",		"",		1,	REFRESH,		emi_test_exec},
	{ "emi_test",		"del",		"",		1,	REFRESH,		emi_test_del},
#endif
#ifdef ARP_TABLE_SUPPORT
	{ "ARPTable",		"add",		"",		1,	REFRESH,		add_arp_table},
	{ "ARPTable",		"del",		"",		1,	REFRESH,		del_arp_table},
#endif
#ifdef DIAG_SUPPORT
	{ "Ping",		"start",	"start_ping",	1,	SERVICE_RESTART,	diag_ping_start},
	{ "Ping",		"stop",		"",		0,	REFRESH,		diag_ping_stop},
	{ "Ping",		"clear",	"",		0,	REFRESH,		diag_ping_clear},
	{ "Traceroute",		"start",	"start_traceroute",		1,	SERVICE_RESTART,		diag_traceroute_start},
	{ "Traceroute",		"stop",		"",		0,	REFRESH,		diag_traceroute_stop},
	{ "Traceroute",		"clear",	"",		0,	REFRESH,		diag_traceroute_clear},
#endif
#ifdef SYSLOG_SUPPORT
	{ "Log_all",		"",	"",		0,	REFRESH,		set_log_type},
#endif
//wwzh add for DNS ENTRY
#ifdef DNS_ENTRY_SUPPORT
	{"Dns_Entry",  "add_dns_entry",  "", 3, REFRESH,    add_dns_entry},
	{"Dns_Entry",  "delete_dns_entry", "", 3, REFRESH,  delete_dns_entry},
#endif
        /* add diag test entry by kirby 2004/6/29 */
        /* name           type           service            sleep time    action   go*/
       {"Diagnostics",  "startdiag",   "start_diagtest",       0 ,            NOTHING , diagtool_start  },
	   /* da_f 2005.3.21 add */
#ifdef NOMAD_SUPPORT
       {"VPNCli_Account",  "remove",  "",  0 ,  NOTHING, rm_vpn_cli_account  },
#endif
};

struct gozila_action *
handle_gozila_action(char *name, char *type)
{
	struct gozila_action *v;

	if(!name || !type)
		return NULL;

	for(v = gozila_actions ; v < &gozila_actions[STRUCT_LEN(gozila_actions)]; v++) {
   		if(!strcmp(v->name, name) && !strcmp(v->type, type)){
			return v;
		}
   	}
        return NULL;
}

int 
gozila_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query, int remote_flag)
{
	char *submit_button, *submit_type;
	int action = REFRESH;
	int sleep_time;
	struct gozila_action *act;

	gozila_action = 1;

	submit_button = websGetVar(wp, "submit_button", NULL);	/* every html must have the name */
	submit_type = websGetVar(wp, "submit_type", NULL);	/* add, del, renew, release ..... */

	nvram_set("action_service","");
	nvram_set("action_service_arg1","");

	cprintf("submit_button=[%s] submit_type=[%s]\n",submit_button, submit_type);
	act = handle_gozila_action(submit_button, submit_type);
//cjg
	if(remote_flag == 1)
		goto readonly_action;
	if(act){
		cprintf("name=[%s] type=[%s] service=[%s] sleep=[%d] action=[%d]\n",act->name, act->type, act->service, act->sleep_time, act->action);
		nvram_set("action_service",act->service);
		//junzhao
		if((!strcmp(submit_type, "connect")) || (!strcmp(submit_type, "disconnect")) || (!strcmp(submit_type, "release")) || (!strcmp(submit_type, "renew")) || (!strcmp(submit_type, "clone_mac")) || (!strcmp(submit_type, "connect_multippp")) || (!strcmp(submit_type, "disconnect_multippp")))
			nvram_set("wan_post_connection", connection);
		//add by lzh;
		if ((!strcmp(submit_button,"Management"))&&(!strcmp(submit_type,"upnp_select_connect")))
		{
	
			char *connection_index = websGetVar(wp,"upnp_pvc_connection",NULL);
			cprintf("save upnp connection:%s\n",connection_index);
			nvram_set("upnp_active_connection",connection_index);
		}			
		//zhangbin for delete conn
		if(!strcmp(submit_button, "ipsec") && !strcmp(submit_type, "delete"))
		{

		#if 0
			char *id = websGetVar(wp,"tunnel_entry",NULL);
			char *name = websGetVar(wp,"tunnel_name",NULL);
			if(name != NULL)
			{
				FILE *fp;
				if((fp = fopen("/tmp/del-vpn","w")) != NULL)
				{
					fprintf(fp,"#!/bin/sh\n");
					fprintf(fp,"/bin/ipsec/whack --terminate --name %s \n",name);
					fprintf(fp,"echo\n");
					fprintf(fp,"echo\n");
					fprintf(fp,"/bin/ipsec/whack --unroute --name %s \n",name);
					fclose(fp);
					eval("/bin/sh","/tmp/del-vpn");
				}
			}
		#endif

			do_ej("DeleteSuccess.asp", wp); 
			websDone(wp, 200);
			act->go(wp);
			goto last;
		}
		//csh for delete gre tunnel
		if(!strcmp(submit_button, "GRE_tunnel") && !strcmp(submit_type, "delete"))
		{
		#ifdef GRE_SUPPORT
			do_ej("DeleteSuccess.asp", wp); 
			websDone(wp, 200);
			act->go(wp);
			goto last;
		#endif
		}


		/* da_f 2005.3.21 add */
#ifdef NOMAD_SUPPORT
		if(!strcmp(submit_button, "VPNCli_Account") && 
			!strcmp(submit_type, "remove"))
		{
			websDone(wp, 200);
			act->go(wp);
			do_ej("vpn_cli_access.asp", wp); 
			goto last;
		}
#endif

		sleep_time = act->sleep_time;
		action = act->action;
		if(act->go)
		{
			act->go(wp);
		}
	}
	else{
		sleep_time = 0;
		action = REFRESH;
	}

	if(action == REFRESH)
	{//zhangbin
		if(!strcmp(submit_button, "ipsec") && !strcmp(submit_type, "connect"))
		{
			do_ej("ConnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(10);
			goto last;
		}
		else if(!strcmp(submit_button, "ipsec") && !strcmp(submit_type, "disconnect"))
		{
			do_ej("DisconnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(8);
			goto last;
		}
#if 0		
		else if(!strcmp(submit_button, "ipsec") && !strcmp(submit_type, "delete"))
		{
			do_ej("DeleteSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(5);
			goto last;
		}
#endif
#ifdef CLOUD_SUPPORT
		if(!strcmp(submit_button, "Cloud") && !strcmp(submit_type, "connect"))
		{
			do_ej("ConnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(10);
			goto last;
		}
		else if(!strcmp(submit_button, "Cloud") && !strcmp(submit_type, "disconnect"))
		{
			do_ej("DisconnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(8);
			goto last;
		}
#endif
		sleep(sleep_time);
	}
	else if(action == SERVICE_RESTART){
		sys_commit();
		//junzhao
		service_restart();
		if(!strcmp(submit_button, "Status_Router") && (!strcmp(submit_type, "connect") || !strcmp(submit_type, "connect_multippp")))
		{
			do_ej("ConnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(sleep_time);
			goto last;
		}
		else if(!strcmp(submit_button, "Status_Router") && (!strcmp(submit_type, "disconnect") || !strcmp(submit_type, "disconnect_multippp")))
		{
			do_ej("DisconnectSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(sleep_time);
			goto last;
		}
		//junzhao 2004.3.29
		else if(!strcmp(submit_button, "Status_Router") && !strcmp(submit_type, "release"))
		{
			do_ej("ReleaseSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(sleep_time);
			goto last;
		}
		else if(!strcmp(submit_button, "Status_Router") && !strcmp(submit_type, "renew"))
		{
			do_ej("RenewSuccess.asp", wp); 
			websDone(wp, 200);
			sleep(sleep_time);
			goto last;
		}
		//zhangbin
		else if(!strcmp(submit_button, "FilterSummary"))
		{
			do_ej("Success_adv.asp",wp);
			websDone(wp, 200);
			sleep(2);
			goto last;
		}


		sleep(sleep_time);
	}
	else if (action == RESTART){
		sys_commit();
		sys_restart();	
		//junzhao
		if(!strcmp(submit_button, "index") && !strcmp(submit_type, "apply"))
		{
			do_ej("ApplySuccess.asp", wp); 
			websDone(wp, 200);
			sleep(sleep_time);
			goto last;
		}
	}
	//cjg
readonly_action:	
	sprintf(path,"%s.asp",submit_button);
	do_ej(path,wp);       //refresh
	websDone(wp, 200);

last:
	gozila_action = 0;    //reset gozila_action

//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	generate_key = 0;
#endif
	clone_wan_mac = 0;

	return 1;
}

struct apply_action apply_actions[] = {
	/* bellow for setup page */
	{ "index",		"",		15,	RESTART		,	NULL},
	//csh for gre
#ifdef GRE_SUPPORT
	{"GRE_tunnel",	"GRETunnel",	0,	SERVICE_RESTART,	NULL},
#endif
	//csh for Cloud
#ifdef CLOUD_SUPPORT
	{"Cloud",	"Cloud",	0,	SERVICE_RESTART,	NULL},
#endif
	//junzhao 2004.6.2
#ifdef STB_SUPPORT
        { "Stb_Entry",    "stbentry",   0,      REFRESH ,       NULL},
#endif

	{ "PVC_Selection",    "pvc_select",	0,	SERVICE_RESTART	,	NULL},
	//{ "index",		"",		2,	REFRESH		,	NULL},
//junzhao
	{ "WanMAC",		"clone_mac",	2,	SERVICE_RESTART ,	NULL},	// for cisco style
	{ "OnePage",		"",		0,	RESTART		,	NULL},	// same as index, for NonBrand
	{ "Security",		"",		1,	RESTART		,	NULL},
//cjg:2004-3-12
	{"Log", "email_alert", 0, SERVICE_RESTART, NULL},
	/*zhangbin for ipsec*/
	{ "ipsec",	"ipsec",	10,	SERVICE_RESTART	,	NULL},
	{ "ipsecadv",	"ipsecadv",	0,	REFRESH	,	NULL},

	/* zhangbin for qos */
	{ "qos",	"qos",		5,	SERVICE_RESTART	,	NULL},
	{ "basic_qos",	"basic_qos",		5,	SERVICE_RESTART	,	NULL},
	
	{ "System",		"",		0,	RESTART		,	NULL},
	{ "DHCP",		"dhcp",		0,	SERVICE_RESTART	,	NULL},
//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "WL_WEPTable",	"",		0,	RESTART		,	NULL},
	{ "WL_WPATable",	"wireless",	12,	SERVICE_RESTART	,	NULL},
#endif
	/* bellow for advanced page */
	{ "DMZ",		"filters",	0,	SERVICE_RESTART	,	NULL},	// for cisco style
	{ "Filters",		"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "FilterIPMAC",	"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "FilterIP",		"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "FilterMAC",		"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "FilterPort",		"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "VPN",		"filters",	0,	SERVICE_RESTART	,	NULL},	// for cisco style
	{ "Firewall",		"filters",	0,	SERVICE_RESTART	,	NULL},	// for cisco style
	{ "Expose",		"filters",	0,	SERVICE_RESTART	,	NULL},
	{ "Forward",		"forward",	0,	SERVICE_RESTART	,	NULL},
	{ "VServer",		"forward",	0,	SERVICE_RESTART	,	NULL},	// same as Forward, for NonBrand
#ifdef PORT_TRIGGER_SUPPORT
	{ "Triggering",		"filters",	0,	SERVICE_RESTART	,	NULL},
#endif
//#ifdef UPNP_FORWARD_SUPPORT
	{ "Forward_UPnP",	"forward_upnp",	0,	SERVICE_RESTART	,	NULL},
//#endif
#ifdef ADVANCED_FORWARD_SUPPORT//add by zhs
        {"AdvancedForwarding","advanced_forwarding",0,SERVICE_RESTART,NULL},
#endif//end by zhs
	{ "Routing",		"static_route_add",	0,	SERVICE_RESTART		,	NULL},//modified by leijun
	//cjg:2004-3-7
	{ "Diagnostics",                "diagnostic",           0,      REFRESH
		, NULL},
	{ "DDNS",		"ddns",		4,	SERVICE_RESTART	,	ddns_save_value},
//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "Wireless",		"wireless",	0,	SERVICE_RESTART	,	NULL},
	//{ "Wireless_Basic",	"wireless",	0,	RESTART	,		NULL},
	{ "Wireless_Basic",	"wireless",	12,	SERVICE_RESTART	,		NULL},
	{ "Wireless_Advanced",	"wireless",	12,	SERVICE_RESTART	,	NULL},
	{ "Wireless_MAC",	"wireless",	12,	SERVICE_RESTART	,	NULL},
	{ "WL_FilterTable",	"wireless",	12,	SERVICE_RESTART	,	NULL},
#endif
#ifdef HSIAB_SUPPORT
	{ "HotSpot_Admin",      "hsiab_register",     2,      SERVICE_RESTART ,	hsiab_register},
	{ "HotSpot_Register_ok","hsiab_register_ok",  2,      RESTART ,		NULL},
	{ "finish_registration","hsiab_register",     2,      REFRESH ,		hsiab_finish_registration},
#endif
#ifdef SYSLOG_SUPPORT
	{ "Log_settings",    	"",		0,      RESTART ,		NULL},

#endif
    //added by leijun 2004-05-20
   	{"dhcp_reserved", "dhcp_reserved_ip", 0, NULL, NULL},
	
        //{"Management"     ,     "snmpd",         0,      SERVICE_RESTART ,
        {"Management"     ,     "snmpd",         0,      RESTART ,
		    NULL},                 //songtao
#ifdef PARENTAL_CONTROL_SUPPORT
        {"Parental_Control"     ,     "artemis",         0,      SERVICE_RESTART ,  NULL}, 	
#endif	    	    

	/* da_f 2005.3.18 add */
#ifdef NOMAD_SUPPORT
   	{"Save_Account", "", 0, NULL, reboot_by_vpn},
   	{"VPNCli_Active", "", 0, NULL, NULL},
   	{"VPNCli_Disconn", "", 0, NULL, NULL},
#endif
};

struct apply_action *
handle_apply_action(char *name)
{
	struct apply_action *v;

	if(!name)
		return NULL;

	for(v = apply_actions ; v < &apply_actions[STRUCT_LEN(apply_actions)] ; v++) {
   		if(!strcmp(v->name, name)){
			return v;
		}
   	}
        return NULL;
}
int static_route_added = 1; //added by leijun
static int
apply_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	  char_t *url, char_t *path, char_t *query, int remote_flag)
{
	int action = NOTHING;
	char *value;
	char *submit_button;
	int sleep_time = 0;
	int need_commit = 1;
	int ret_code;
		
	//zhangbin 2005.4.20
	char pre_lanip[18],pre_lanmask[18];

	error_value = 0;
	ret_code = -1;
	static_route_added = 1;//added by leijun
        /********************/
        value = websGetVar(wp, "change_action", "");

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): change_action=[%s]",__FUNCTION__,value);
#endif
/*
	//junzhao 2004.4.15 to check if busy now
	{
	char tmpbuf[5];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))
	{
		if(!strcmp(tmpbuf, "1"))
		{	
	        	do_ej("Busy.asp",wp);
			websDone(wp, 200);
			sleep(2);
			return 1;
		}
	}
	}
*/
        if(value && !strcmp(value,"gozila_cgi")){     
             gozila_cgi(wp, urlPrefix, webDir, arg, url, path, query, remote_flag/*cjg*/);
             return 1;
        }
        /********************/
	//junzhao 2004.4.15 to check if busy now
	/*
	{
	char tmpbuf[5];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))
	{
		if(!strcmp(tmpbuf, "1"))
		{	
	        	do_ej("Busy.asp",wp);
			websDone(wp, 200);
			sleep(2);
			return 1;
		}
	}
	}
	*/
	
	submit_button = websGetVar(wp, "submit_button", "");
	//cjg
	if(remote_flag == 1){
		sprintf(path, "%s.asp", submit_button);
		do_ej(path, wp);
		websDone(wp, 200);
		return 1;
	}
	if (!query)
		goto footer;

#if 0
        if(check_hw_type() == BCM4702_CHIP) /* barry add for 4712 or 4702 RF test */
	        {
	          printf("\nBoard is 4702 \n");
	          value = websGetVar(wp, "StartContinueTx", NULL);
	          if(value){
	                    StartContinueTx_4702(value);
	                    goto footer;
	                    }

                  value = websGetVar(wp, "StopContinueTx", NULL);
                  if(value){
                            StopContinueTx_4702(value);
			    goto footer;
			    }
	       }
        else
        {
		value = websGetVar(wp, "StartContinueTx", NULL);
		printf("\nBarry StartContinueTx,value=%s\n",value);
		if(value){
			StartContinueTx(value);
			goto footer;
		}
	
		value = websGetVar(wp, "StopContinueTx", NULL);
		printf("\nBarry StopContinueTx,value=%s\n",value);
		if(value){
			StopContinueTx(value);
			goto footer;
		}
		/* 1030 */
		value = websGetVar(wp, "WL_atten_bb", NULL);
		printf("\nBarry WL_atten_bb,value=%s\n",value);
		if(value){
			Check_TSSI(value);
			goto footer;
		}
		/* 1030 */
		value = websGetVar(wp, "WL_tssi_enable", NULL);
		printf("\nBarry WL_tssi_enable,value=%s\n",value);
		if(value){
			Enable_TSSI(value);
			goto footer;
		}
		/* 1216 */
		value = websGetVar(wp, "ChangeANT", NULL);
		printf("\nBarry ChangeANT,value=%s\n",value);
		if(value){
			Change_Ant(value);
			goto footer;
		}
	}
	value = websGetVar(wp, "skip_amd_check", NULL);
	if(value){
		if(atoi(value) == 0 || atoi(value) == 1){
			nvram_set("skip_amd_check", value);
			sys_commit();
		}
		goto footer;
	}
#endif

	need_commit = atoi(websGetVar(wp, "commit", "1"));
	
	value = websGetVar(wp, "action", "");

	/* Apply values */
	if (!strcmp(value, "Apply")) {
		struct apply_action *act;

		memset(pre_lanip,0,sizeof(pre_lanip));
		memset(pre_lanmask,0,sizeof(pre_lanmask));
	
		strcpy(pre_lanip,nvram_safe_get("lan_ipaddr"));	
		strcpy(pre_lanmask,nvram_safe_get("lan_netmask"));	

		printf("pre_lanip=%s,pre_lanmask=%s\n",pre_lanip,pre_lanmask);
		
		validate_cgi(wp);

		act = handle_apply_action(submit_button);

		if(act){
			cprintf("submit_button=[%s] service=[%s] sleep_time=[%d] action=[%d]\n", 
				 act->name, act->service, act->sleep_time, act->action);
			if(act->action == SERVICE_RESTART)
				nvram_set("action_service",act->service);
			else
				nvram_set("action_service","");
			sleep_time = act->sleep_time;
			action = act->action;

			if(act->go)
				ret_code = act->go(wp);

			/* da_f 2005.4.1 add */
#ifdef NOMAD_SUPPORT
			if(!strcmp(submit_button, "Save_Account") && 1 == ret_code)
				action = REBOOT;
#endif

#ifdef HSIAB_SUPPORT
			if(ret_code == DISABLE_HSIAB)
				action = RESTART;
			else if(ret_code == RESTART_HSIAB)
				action = RESTART;
#endif
		}
		else{
			nvram_set("action_service","");
			sleep_time = 1;
			action = RESTART;
		}
#if 1
		if (!static_route_added) error_value = 1;		
		if((need_commit) && (static_route_added))
			sys_commit();
#else				
		if(need_commit)
			sys_commit();
#endif
	}
	/* Restore defaults */
	else if (!strncmp(value, "Restore", 7)) {
		ACTION("ACT_SW_RESTORE"); 
		//eval("erase","nvram");
		nvram_set("restore_defaults", "1");
		eval("killall", "-9", "udhcpc");
		sys_commit();
		action = REBOOT;
	}

	//junzhao 2004.6.14
	else if(!strncmp(value, "Rebootvalidate", 14))
	{
		char *type = websGetVar(wp, "submit_type", "");
		char *reboot_user = websGetVar(wp, "reboot_username", NULL);
		char *reboot_password = websGetVar(wp, "reboot_password", NULL);
		if(reboot_user && reboot_password)
		{
			if(!strcmp(reboot_user, nvram_safe_get("http_username")) &&
			  !strcmp(reboot_password, nvram_safe_get("http_passwd")))
			{
				if(!strcmp(type, "HRebootvalidate"))
				{
					do_ej("Hard_reboot_validate_ok.asp", wp);
					websDone(wp, 200);
					sys_reboot();
				}
				else if(!strcmp(type, "SRebootvalidate"))
				{
					do_ej("Soft_reboot_validate_ok.asp", wp);
					websDone(wp, 200);

					//junzhao 2004.8.9 for soft restart..
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
					buf_to_file("/tmp/adsl_action", "down");
					sys_stop();
					sleep(10);
					sys_restart();
					
					/*
					sys_restart();
					*/
					if(check_adsl_status())
					{
						vcc_config_init();
						if(!strcmp(autodetect, "1"))
							sleep(60);
						else
							sleep(40);
						vcc_inited = 0;
					}
					else
						sleep(10);
				}
			}
			else
			{
				do_ej("Reboot_validate_fail.asp", wp);
				websDone(wp, 200);
				sleep(2);
			}
		}
		return 1;
	}		

	/* Reboot */
	else if (!strncmp(value, "Reboot", 7)) {
		action = REBOOT;
	}
#ifdef BRCM
	/* Release lease */
	else if (!strcmp(value, "Release")) {
		websWrite(wp, "Releasing lease...");
		//sys_release();
		websWrite(wp, "done<br>");
		action = NOTHING;
	}

	/* Renew lease */
	else if (!strcmp(value, "Renew")) {
		websWrite(wp, "Renewing lease...");
		//sys_renew();
		websWrite(wp, "done<br>");
		action = NOTHING;
	}
#endif	

#ifdef SETUP_WIZARD_FT_SUPPORT
	else if (!strcmp(value, "ApplyWizard"))
	{
		char *pppoa_user;
		char *pppoa_passwd;
		char pppoa_config[511];
		char p_tmp[511];
		char *next;
		char p_config[126];
	#ifdef CLI_SUPPORT
		char telnet_enable[4], *pt_telnet;
	#endif
		int p_ii = 0;
		char *ipcp_netmask;
		y_printf("hHHH WIZARD TEST\n");
		
		pppoa_user = websGetVar(wp, "pppoa_username", NULL);
		pppoa_passwd = websGetVar(wp, "pppoa_password", NULL);
	#ifdef CLI_SUPPORT
		pt_telnet = websGetVar(wp, "telnet_enable", NULL);
		strcpy(telnet_enable, pt_telnet);
	#endif
	#ifdef IPCP_NETMASK_SUPPORT	
		ipcp_netmask = websGetVar(wp, "ipcp_netmask_enable", "0");
	#endif
		
		strcpy(pppoa_config, nvram_safe_get("pppoa_config"));
		y_printf("pp %s %s \n", pppoa_passwd, pppoa_user);
		foreach(p_config, pppoa_config, next)
		{
			if (p_ii == 0)
			{
				string_rep(p_config, 1, pppoa_user);
				if(pppoa_passwd && pppoa_passwd[0])
				{
					string_rep(p_config, 2, pppoa_passwd);
				}
				strcpy(p_tmp, p_config);
				strcat(p_tmp, " ");
			}
			else
			{
				strcat(p_tmp, p_config);
				if (p_ii <= 6)
					strcat(p_tmp, " ");
			}
			p_ii++;
		}
		
		nvram_set("pppoa_config", p_tmp);
	#ifdef CLI_SUPPORT
		nvram_set("telnet_enable", telnet_enable);
	#endif
	#ifdef IPCP_NETMASK_SUPPORT	
		nvram_set("ipcp_netmask_enable", ipcp_netmask);
	#endif	
		validate_setupwizard_cgi(wp);
		//validate_cgi(wp);
		nvram_commit();
		action = RESTART;
	}
#endif

	/* Invalid action */
	else
		websDebugWrite(wp, "Invalid action %s<br>", value);


footer:

	{
	char tmpbuf[5];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))
	{
		if(!strcmp(tmpbuf, "1"))
		{
			FILE *fp = fopen("/var/run/httpd_nosignal", "w");
			fclose(fp);
			if (action == RESTART)
				sys_restart();
			else if (action == REBOOT)
				sys_reboot();
			else if (action == SERVICE_RESTART)
				service_restart();
	        	do_ej("Busy.asp",wp);
			websDone(wp, 200);
			sleep(40);
			unlink("/var/run/httpd_nosignal");
			return 1;
		}
		else
			unlink("/var/run/httpd_nosignal");
	}
	}

	//2004.11.30 for goto Success_r_s.asp when restore
	if(!strcmp(submit_button, "Factory_Defaults"))
	{
		if(!strcmp(value, "Restore"))
		{
			do_ej("Success_r_s.asp",wp);
			websDone(wp, 200);
			sleep(2);
			goto setaction;
		}
		else
			action = REFRESH;
	}
	
#if OEM == LINKSYS
#ifdef HSIAB_SUPPORT
	if(!strcmp(submit_button, "HotSpot_Admin") && ret_code != DISABLE_HSIAB && ret_code != RESTART_HSIAB){
		if(register_status == -1 )
	        	do_ej("HotSpot_Register_fail.asp",wp);
		else{
			if(new_device == 1)
	        		do_ej("HotSpot_New_device.asp",wp);
			else{
	        		do_ej("HotSpot_Old_device.asp",wp);
				nvram_set("action_service", "");
				action = REFRESH;
			}
		}
	}
	else if(!strcmp(submit_button,"finish_registration")){
		do_ej("HotSpot_Register_ok.asp",wp);
	}
	else
#endif
		//zhangbin 2005.4.20
		if(!strcmp(submit_button, "index"))
		{
			char new_lanip[18],new_lanmask[18];
			
			memset(new_lanip,0,sizeof(new_lanip));
			memset(new_lanmask,0,sizeof(new_lanmask));

			strcpy(new_lanip,nvram_safe_get("lan_ipaddr"));
			strcpy(new_lanmask,nvram_safe_get("lan_netmask"));

			printf("new_lanip=%s,new_lanmask=%s\n",new_lanip,new_lanmask);

			if((strcmp(pre_lanip,new_lanip) != 0) || (strcmp(pre_lanmask,new_lanmask) != 0))
			{
				printf("LAN IP/Netmask changed!\n");
				do_ej("Success_adv.asp", wp);
				websDone(wp, 200);
				goto setaction;
			}
		}


	{
		if(!strcmp(submit_button,"ipsecadv"))
		{
			do_ej("Success_adv.asp",wp);
			return 1;
		}			
		//csh for Cloud.asp	
		if(!strcmp(submit_button, "Cloud"))
		{
			do_ej("Success_adv.asp", wp);
			return 1;
		}
		
#ifdef NOMAD_SUPPORT
		/* da_f 2005.3.21 add. For vpn_account.asp */
		/*
		if(!strcmp(submit_button, "Save_Account")) 
		{
			do_ej("Success_adv.asp", wp);
			return 1;
		}*/

		/* da_f 2005.3.21 add. For vpn_cli_access.asp */
		if(!strcmp(submit_button, "VPNCli_Active") || 
			!strcmp(submit_button, "Save_Account")) 
		{
			if(1 == ret_code) /* need reboot */
			{
				do_ej("Success_adv.asp", wp);
				websDone(wp, 200);
				goto setaction;
			}
			else
			{
				do_ej("vpn_cli_access.asp", wp);
				return 1;
			}
		}
		/* da_f 2005.3.21 add. For Status_VPNCli.asp */
		if(!strcmp(submit_button, "VPNCli_Disconn"))
		{
			do_ej("Status_VPNCli.asp", wp);
			return 1;
		}
#endif
		
		if(!error_value){
			if(websGetVar(wp, "small_screen", NULL))
        			do_ej("Success_s.asp",wp);
			else
        			do_ej("Success.asp",wp);
		}
		else{
			if(websGetVar(wp, "small_screen", NULL))
        			do_ej("Fail_s.asp",wp);
			else
        			do_ej("Fail.asp",wp);
		}
	}
#else
	do_ej("Success.asp",wp); 
#endif
	websDone(wp, 200);

setaction:	
	if (action == RESTART)
		sys_restart();
	else if (action == REBOOT)
		sys_reboot();
	else if (action == SERVICE_RESTART){
		service_restart();
	}
//junzhao
/*
	else if (action == REFRESH){
		sleep(sleep_time);
		sprintf(path, "%s.asp", submit_button);
		do_ej(path, wp);
		websDone(wp, 200);
		return 1;
	}
*/	
//junzhao
	//sleep(sleep_time);
//junzhao 2004.4.23	
/*
	{
	char tmpbuf[5];
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))
	{
		if(!strcmp(tmpbuf, "1"))
		{
			file *fp = fopen("/var/run/httpd_nosignal", "w");
			fclose(fp);
		}
		else
			unlink("/var/run/httpd_nosignal");
	}
	}
*/
	if(strcmp(submit_button, "index"))	
		sleep(sleep_time);
	else
	{
		//junzhao 2004.4.22
		if(check_adsl_status())
		{
			vcc_config_init();
			if(!strcmp(autodetect, "1"))
				sleep(60);
			else
				sleep(40);
			vcc_inited = 0;
		}
		else
			sleep(10);
	}
	
	return 1;
}

#ifdef BRCM
static int
internal_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	     char_t *url, char_t *path, char_t *query)
{
	int action = NOTHING;
	char *value;

	websHeader(wp);
	websWrite(wp, (char_t *) apply_header, error_value ? "Settings are failed" : "Settings are successful");

	if (!query)
		goto footer;

	value = websGetVar(wp, "action", "");

	/* Apply values */
	if (!strcmp(value, "Apply")) {
		validate_cgi(wp);
		action = RESTART;
	}

	/* Upgrade image */
	else if (!strcmp(value, "Upgrade")) {
		char *os_name = nvram_safe_get("os_name");
		char *os_server = websGetVar(wp, "os_server", nvram_safe_get("os_server"));
		char *os_version = websGetVar(wp, "os_version", "current");
		char url[PATH_MAX];
		if (!*os_version)
			os_version = "current";
		snprintf(url, sizeof(url), "%s/%s/%s/%s.trx",
			 os_server, os_name, os_version, os_name);
		websWrite(wp, "Retrieving %s...", url);
		if (sys_upgrade(url, NULL, NULL)) {
			websWrite(wp, "error<br>");
			goto footer;
		} else {
			websWrite(wp, "done<br>");
			action = REBOOT;
		}
	}

	/* Report stats */
	else if (!strcmp(value, "Stats")) {
		char *server = websGetVar(wp, "stats_server", nvram_safe_get("stats_server"));
		websWrite(wp, "Contacting %s...", server);
		if (sys_stats(server)) {
			websWrite(wp, "error<br>");
			goto footer;
		} else {
			websWrite(wp, "done<br>");
			nvram_set("stats_server", server);
		}
	}

	/* Invalid action */
	else {
		websWrite(wp, "Invalid action<br>");
		goto footer;
	}

	websWrite(wp, "Committing values...");
	sys_commit();
	websWrite(wp, "done");
 footer:
	//apply_footer(wp);
	websWrite(wp, (char_t *) apply_footer, "history.go(-1)");
	websFooter(wp);
	websDone(wp, 200);

	if (action == RESTART)
		sys_restart();
	else if (action == REBOOT)
	{
		//junzhao 2004.6.17
		nvram_set("autosearchflag", "0");
		nvram_commit();
		sys_reboot();
	}
	return 1;
}
#endif
#ifdef WEBS

void
initHandlers(void)
{
	websAspDefine("nvram_get", ej_nvram_get);
	websAspDefine("nvram_match", ej_nvram_match);
	websAspDefine("nvram_invmatch", ej_nvram_invmatch);
	websAspDefine("nvram_list", ej_nvram_list);
	websAspDefine("filter_ip", ej_filter_ip);
	websAspDefine("filter_port", ej_filter_port);
	websAspDefine("forward_port", ej_forward_port);
	websAspDefine("static_route", ej_static_route);
	websAspDefine("localtime", ej_localtime);
	websAspDefine("show_uptime", ej_show_uptime);
//	websAspDefine("dumplog", ej_dumplog);
	websAspDefine("dump_syslog",ej_dump_syslog);
	websAspDefine("dump_vpnlog",ej_dump_vpnlog); //zhangbin  2004.4.20
	/* zhangbin for vpn-log*/
	websAspDefine("dumpvpnlog", ej_dumpvpnlog);
	websAspDefine("dumpleases", ej_dumpleases);
	websAspDefine("ppplink", ej_ppplink);
        
        websAspDefine("dumparp", ej_dumparp);//guohong 2004.07.07        
	websUrlHandlerDefine("/apply.cgi", NULL, 0, apply_cgi, 0);
	websUrlHandlerDefine("/internal.cgi", NULL, 0, internal_cgi, 0);

	websSetPassword(nvram_safe_get("http_passwd"));

	websSetRealm("Broadcom Home Gateway Reference Design");
}

#else /* !WEBS */

static void
do_auth(char *userid, char *passwd, char *realm)
{
  //modified by zls 2005-0112
        char userid_temp[AUTH_MAX],passwd_temp[AUTH_MAX];
        int type=0;
        type = atoi(nvram_safe_get("validate_userid"));
//fixed remote_management issue from dom 2005/10/12
#ifdef HTTPS_SUPPORT
	sprintf(userid_temp,"http_remote_username%d",type);
	sprintf(passwd_temp,"http_remote_passwd%d",type);
	strncpy(userid, nvram_safe_get("http_remote_username"), AUTH_MAX);
	strncpy(passwd, nvram_safe_get("http_remote_passwd"), AUTH_MAX);
#else
	sprintf(userid_temp,"http_username%d",type);
	sprintf(passwd_temp,"http_passwd%d",type);
	strncpy(userid, nvram_safe_get("http_username"), AUTH_MAX);
	strncpy(passwd, nvram_safe_get("http_passwd"), AUTH_MAX);
#endif

	if(!do_ssl){
		#ifdef MULTIUSER_SUPPORT
		if ( type !=0 ){
			strncpy(userid, nvram_safe_get(userid_temp), AUTH_MAX);
			strncpy(passwd, nvram_safe_get(passwd_temp), AUTH_MAX);
		}
		#endif
	}//over zls
	else{
		strncpy(userid, nvram_safe_get("http_remote_username"), AUTH_MAX);
		strncpy(passwd, nvram_safe_get("http_remote_passwd"), AUTH_MAX);
	}
	//strncpy(realm, MODEL_NAME, AUTH_MAX);
	//zhangbin 2004.12.13
	//strncpy(realm, nvram_safe_get("router_name"), AUTH_MAX);

	
/*	if(!do_ssl){
		strncpy(userid, nvram_safe_get("http_username"), AUTH_MAX);
		strncpy(passwd, nvram_safe_get("http_passwd"), AUTH_MAX);
	}else
	{
		strncpy(userid, nvram_safe_get("http_remote_username"), AUTH_MAX);
		strncpy(passwd, nvram_safe_get("http_remote_passwd"), AUTH_MAX);
	}
	//strncpy(realm, MODEL_NAME, AUTH_MAX);
	//zhangbin 2004.12.13
	//strncpy(realm, nvram_safe_get("router_name"), AUTH_MAX);
	*/
#if 0
struct in_addr remote_pvc;
	
	char ipaddr[200];
	int i,j;
	char *wan_ip,*str_ipaddr,*tmp_ipaddr;
	char wan_ip_addr[8][16]={
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx",
		"xxx.xxx.xxx.xxx"
	};

	char http_name[22],http_password[20];
	char *name_index=http_name;
	char *password_index=http_password;
	char var1[256],var2_buf[20], *next1;
	char  *next2;
	char *var2=var2_buf;
	char *name1,name3, *enable1,enable3, *proto1, *proto3,*port3,*port1, *ip1;
	char *name2, *enable2, *proto2, *port2, *ip3,*ip2,*t1,*t2,*t3;
	char name1_buf[20],name2_buf[20];
	char *name1_index = name1_buf;
	char *name2_index = name2_buf;
	char wordlist_buf1[20],wordlist_buf2[20],wordlist_buf3[20];
	char *wordlist1=wordlist_buf1;
	char *wordlist2=wordlist_buf2;
	char *wordlist3=wordlist_buf3;
	struct in_addr tt;
	int sig=0;
	int save;
	int samecount=0;
	int addr_select=3;
	int temp[5];
	int id1,id2,id3;
	int retcount=0;
	
	unsigned char startipc[4],endipc[4];
	unsigned int startip[4], endip[4],midip[4];

	 //modified by zls 2005-0112
        char userid_temp[AUTH_MAX],passwd_temp[AUTH_MAX];
        int type=0;

	remote_pvc.s_addr =accessing_ip;
	
	y_printf("ip_local=%x,des=%s,pvc=%s\n",(accessing_ip),re_ip_des,inet_ntoa(remote_pvc));
	//printf("ip_local()=%s,des=%s,pvc=%s\n",re_ip_local,re_ip_des,inet_ntoa(remote_pvc));
	if(!file_to_buf("/tmp/status/wan_ipaddr",ipaddr,sizeof(ipaddr)))
	{
		return 0;
	}
	str_ipaddr = ipaddr;
	for(i=0;i<8;i++)
	{
		tmp_ipaddr = strsep(&str_ipaddr," ");
		strncpy(wan_ip_addr[i],tmp_ipaddr,sizeof(wan_ip_addr[i]));
		//printf("wan_ip_addr=%s\n",wan_ip_addr[i]);
	}

	for(i=0;i<8;i++)
	{
		y_printf("wan=%s,des=%s,cmp=%d\n",wan_ip_addr[i],re_ip_des,strcmp(wan_ip_addr[i],re_ip_des));
		if(!strcmp(wan_ip_addr[i],re_ip_des))
		{
			y_printf("!!!\n");
		
			snprintf(name1_index,15,"range_save_ip%d",i);
	
			y_printf("re=%s\n",re_ip_local);

			strcpy(var2 ,re_ip_local);
			strcpy(wordlist3,nvram_safe_get("ip_range_setting"));
		///	strcpy(wordlist3,"0:2:10.0.0.2-10.0.0.9");
			wordlist1 = strsep(&wordlist3,"-");
			printf("word3=%s,word1=%s\n",wordlist3,wordlist1);
			// name:enable:proto:port>ip name:enable:proto:port>ip 
			foreach(var1, wordlist1, next1) {
		
			y_printf("var =%s,wordlist1=%s,next=%s\n",var1,wordlist1,next1);
			y_printf("var =%s,wordlist1=,next=\n",var2);
		t1 = var1;
		t2 = strsep(&t1,":");
		printf("t1=%s,t2=%s\n",t1,t2);

		t3 = strsep(&t1,":");
		printf("t1=%s,t3=%s\n",t1,t3);
		strcpy(temp,t3);
		printf("temp=%s\n",temp);
		if(atoi(temp)==0)
		{
		printf("temp 0\n");
			save = i;
			sig = 1;
		}
		if(atoi(temp)==1)
		{
		printf("temp 1\n");
		if(!strcmp(t1,var2))		
			{
				save = i;
				sig = 1;
			}
		}//select single ip t3==1;
//		enable1 = var1;
		if(atoi(temp)==2)
		{
		enable1 = t1;
		printf("1ok\n");
		enable2 = var2;
		
		printf("errO?enable = %s\n",wordlist3);

	retcount = sscanf(enable1,"%u.%u.%u.%u", &startip[0],&startip[1],&startip[2],&startip[3]);
	retcount += sscanf(wordlist3,"%u.%u.%u.%u", &endip[0],&endip[1],&endip[2],&endip[3]);
	retcount += sscanf(enable2,"%u.%u.%u.%u", &midip[0],&midip[1],&midip[2],&midip[3]);
		for(id1=0;id1<4;id1++)
		{
	printf("startip[%d]=%d,endip[%d],midip[%d]=%d\n",id1,startip[id1],endip[id1],id1,midip[id1]);
		}
		for(j=0;j<4;j++)
		{
			printf("j=%d\n",j);
		if(startip[j]<midip[j] && midip[j]<endip[j])
		{
		y_printf(".............+\n");
			sig=1;
			save = i;
			break;
			
		}
		if(startip[j]==midip[j] || endip[j]==midip[j])
		{
			samecount++;
			y_printf("same=%d...++++++++++\n",samecount);
		}
		else
		{
			printf("sig=====000\n");
			sig=0;
			break;
		}
		}

		}//if t3==2,select range ip

	}


	}

	}
	if(samecount==4)
	{
		sig=1;
		samecount=0;
	}
	//printf("sig=%d\n",sig);
	if(sig==1)
	{
		snprintf(name_index,22,"http_remote_username%d",save);
		snprintf(password_index,20,"http_remote_passwd%d",save);
			
		printf("i=%d is ok!\n",save);
	}
         //modified by zls 2005-0112
	/*char userid_temp[AUTH_MAX],passwd_temp[AUTH_MAX];
        int type=0;*/
                                                                                                               
        type = atoi(nvram_safe_get("validate_userid"));
        sprintf(userid_temp,"http_username%d",type);
        sprintf(passwd_temp,"http_passwd%d",type);

	if(!do_ssl){
		 if (type==0){
                        strncpy(userid, nvram_safe_get("http_username"), AUTH_MAX);
                        strncpy(passwd, nvram_safe_get("http_passwd"), AUTH_MAX);
                }else {
                         strncpy(userid, nvram_safe_get(userid_temp), AUTH_MAX);
                         strncpy(passwd, nvram_safe_get(passwd_temp), AUTH_MAX);
                }
	}//over zls
	else
	
	{
		printf("!dossl\n");
		if(sig==1)
		{
		printf("enter?\n");
//		strncpy(userid, nvram_safe_get(name_index), AUTH_MAX);
//		strncpy(passwd, nvram_safe_get(password_index), AUTH_MAX);
		strncpy(userid, nvram_safe_get("http_remote_username"), AUTH_MAX);
		strncpy(passwd, nvram_safe_get("http_remote_passwd"), AUTH_MAX);
		}
		else
		{
		printf("hoho\n");
		strncpy(userid, "XXXXXXXX", AUTH_MAX);
		strncpy(passwd, "gggggg", AUTH_MAX);

		}
	}
	//zhangbin 2004.12.13
	//strncpy(realm, nvram_safe_get("router_name"), AUTH_MAX);	
#endif
}

#ifdef GET_POST_SUPPORT

static char post_buf[10000] = { 0 };
extern int post;

static void	// support GET and POST 2003-08-22
do_apply_post(char *url, webs_t stream, int len, char *boundary)
{
   char buf[1024];
   int count;

   if (post==1) {
        /* Get query */
        if (!(count = wfread(post_buf, 1, len, stream)))
                return;
	post_buf[count] = '\0';;
        len -= strlen(post_buf);

        /* Slurp anything remaining in the request */
        while (len--)

#if defined(HTTPS_SUPPORT)
	     if(do_ssl)	
		     BIO_gets((BIO *)stream,buf,1);
	     else
#endif

	    	     (void) fgetc(stream);
   }
//   printf("\nEMIly TEST - buf=%s\n",buf);
}

static void
do_apply_cgi(char *url, webs_t stream, int flag)
{
	char *path, *query;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): url=[%s]",__FUNCTION__,url);
#endif
        if (post==1) {
                query=post_buf;
                path=url;
        }
        else {
                query=url;
                path = strsep(&query, "?") ? : url;
        }

	if(!query)
		return;	

	init_cgi(query);
	apply_cgi(stream, NULL, NULL, 0, url, path, query, flag);
}

#else	// only support GET
static void
//do_apply_cgi(char *url, FILE *stream)
do_apply_cgi(char *url, webs_t stream) // jimmy, https, 8/4/2003
{
	char *path, *query;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): url=[%s]",__FUNCTION__,url);
#endif
	query = url;
	path = strsep(&query, "?") ? : url;

	init_cgi(query);
	apply_cgi(stream, NULL, NULL, 0, url, path, query);
}
#endif

#ifdef BRCM
static void
do_internal_cgi(char *url, FILE *stream)
{
	char *path, *query;

	query = url;
	path = strsep(&query, "?") ? : url;
	init_cgi(query);
	internal_cgi(stream, NULL, NULL, 0, url, path, query);
}
#endif

int
ej_get_http_method(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GET_POST_SUPPORT
	return websWrite(wp, "%s", "post");
#else
	return websWrite(wp, "%s", "get");
#endif
}

int
ej_ipppoe_support(int eid, webs_t wp, int argc, char_t **argv)
{	
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

#ifndef IPPPOE_SUPPORT
	if(count == 1)
		return websWrite(wp, "%s", "<!--");
	else if(count == 2)
		return websWrite(wp, "%s", "-->");
	else if(count == 3)
		return websWrite(wp, "%s", "/*");
	else if(count == 4)
		return websWrite(wp, "%s", "*/");
#endif
	return 0;
}

int
ej_mpppoe_support(int eid, webs_t wp, int argc, char_t **argv)
{	
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

#ifndef MPPPOE_SUPPORT
	if(count == 1)
		return websWrite(wp, "%s", "<!--");
	else if(count == 2)
		return websWrite(wp, "%s", "-->");
	else if(count == 3)
		return websWrite(wp, "%s", "/*");
	else if(count == 4)
		return websWrite(wp, "%s", "*/");
#endif
	return 0;
}

//junzhao 2004.6.25
int
ej_clip_support(int eid, webs_t wp, int argc, char_t **argv)
{	
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

#ifndef CLIP_SUPPORT
	if(count == 1)
		return websWrite(wp, "%s", "<!--");
	else if(count == 2)
		return websWrite(wp, "%s", "-->");
	else if(count == 3)
		return websWrite(wp, "%s", "/*");
	else if(count == 4)
		return websWrite(wp, "%s", "*/");
#endif
	return 0;
}


//junzhao 2004.4.8 for wireless support
int
ej_wireless_support(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
#ifdef WIRELESS_SUPPORT
	if(count == 3)
		return websWrite(wp, "%s", "image/UI_06.gif");
	else if(count == 4)
		return websWrite(wp, "%s", "image/UI_07.gif");
#else
	if(count == 1)
		return websWrite(wp, "%s", "<!--");
	else if(count == 2)
		return websWrite(wp, "%s", "-->");
	else if(count == 3)
		return websWrite(wp, "%s", "image/UI_07.gif");
	else if(count == 4)
		return websWrite(wp, "%s", "image/UI_06.gif");
	else if(count == 5)
		return websWrite(wp, "%s", "/*");
	else if(count == 6)
		return websWrite(wp, "%s", "*/");
#endif
}
	
//wwzh add for WAN UPGRADE
#ifdef WAN_UPGRADE
int
ej_show_upgrade_info(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char version[63];

	memset((void *)version, 0, sizeof(version));
	strcpy(version, nvram_get("latest_version"));
        ret = websWrite(wp, "You can upgrade the latest version : %s from the web site: %s, are you sure?", 
			version, nvram_get("upgrading_wan_url"));

	return ret;
}
#endif

int ej_wan_upgrade_support(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
#ifdef WAN_UPGRADE 
	if (count == 1)
		return websWrite(wp, "%s", "");
	if (count == 2)
		return websWrite(wp, "%s", "");
#else
	if (count == 1)
		return websWrite(wp, "%s", "<!--");
	if (count == 2)
		return websWrite(wp, "%s", "-->");
#endif
}

int ej_wlan_management_support(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
#ifdef WIRELESS_SUPPORT 
	if (count == 1)
		return websWrite(wp, "%s", "");
	if (count == 2)
		return websWrite(wp, "%s", "");
#else
	if (count == 1)
		return websWrite(wp, "%s", "<!--");
	if (count == 2)
		return websWrite(wp, "%s", "-->");
#endif
}


/* Dump arp in <tr><td>IP</td><td>MAC</td></tr> format        guohong */
int
ej_dumparp(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	
	int i;
	struct in_addr addr;
	unsigned long expires;
	
	int ret = 0;
	int count = 0;
	char ipaddr[16], mac[20];
        char buf[256];
        char *p,*p1;
      
	/* Parse leases file */
	if ((fp = fopen("/proc/net/arp", "r"))) {
	   fgets(buf,sizeof(buf),fp);  // skip the first line	   
	   while (fgets(buf,sizeof(buf),fp)) {	   	
                p = buf;
		p1 = strstr(p," ");
		strncpy(ipaddr,p,p1-p);
		ipaddr[p1-p] = 0;
             /*skip hardware type iterm*/
		while(isblank(*p1))   
			p1++;
		while(!isblank(*p1))
			p1++;
		
            /*skip flag iterm*/
             while(isblank(*p1))
			p1++;
		while(!isblank(*p1))
			p1++;
		
              /*skip the blank char preceding the MAC address*/
		 while(isblank(*p1))
			p1++;
		 
		 p = p1;
		 p1 = strstr(p," ");
               strncpy(mac,p,p1-p);
		 mac[p1-p] = 0;
		 	
 
ret += websWrite(wp, "%c'%s','%s'\n",count ? ',' : ' ', 
			
			ipaddr, 
			mac
			
			);
                count++;
	   }
	fclose(fp);
	}

	return ret;
}
int
ej_passwd_match(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	char http_remote_passwd[50],http_def_passwd[50];

	memset(http_remote_passwd, 0, sizeof(http_remote_passwd));
	memset(http_def_passwd, 0, sizeof(http_def_passwd));
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	strcpy(http_remote_passwd,nvram_safe_get("http_remote_passwd"));	
	strcpy(http_def_passwd,nvram_safe_get("http_def_passwd"));

	if(!strcmp(http_remote_passwd,http_def_passwd))
	//if(http_remote_passwd == nvram_get("http_def_passwd"))
		return websWrite(wp, "1");
	else 
		return websWrite(wp, "0");
}

int
ej_l_passwd_match(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	char http_passwd[50],http_def_passwd[50];

	memset(http_passwd, 0, sizeof(http_passwd));
	memset(http_def_passwd, 0, sizeof(http_def_passwd));
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	strcpy(http_passwd,nvram_safe_get("http_passwd"));	
	strcpy(http_def_passwd,nvram_safe_get("http_def_passwd"));	
	
	if(!strcmp(http_passwd,http_def_passwd))
		return websWrite(wp, "%s", "1");
	else 
		return websWrite(wp, "%s", "0");
}

static char no_cache[] =
"Cache-Control: no-cache\r\n"
"Pragma: no-cache\r\n"
"Expires: 0"
;


extern int http_get(const char *server, char *buf, size_t count, off_t offset);
unsigned long total_len = 0, kernel_len = 0, filesystem_len = 0;
    struct web_header{
        char magic[4];
        char res1[4];
        char fwdate[3];
        char fwvern[3];
        char id[4];
        unsigned char res2[14];
        unsigned char length[16];
    };


int create_download_connect(char *ipaddr, int port)
{
        int ret;
        struct sockaddr_in serveraddr;
        int fd;
        int max;
        webs_t stream;

        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
                //printf("Error: create download socket failed\n");
                websWrite(stream,"%s","Error : Create download socket failed\n");
                return 1;
        }
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
        serveraddr.sin_port = htons(port);
        max = MAX_SIZE;
        ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&max, sizeof(int));
        if (ret < 0)
        {
                //printf("SETSOCKOPT SNDBUF ERROR\n");
                return 2 ; 
        }
        ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        if (ret == -1)
        {
                //printf("Error: download connect failed\n");
                close(fd);
                return 3;
        }
        return fd;
}

#ifdef DHCP_DOMAIN_IP_MAPPING_SUPPORT
int Isblankfile(void)
{
	struct stat buf;
	int fd;

	fd = open("/tmp/http_voip", O_RDONLY);
	fstat(fd, &buf);
	if (buf.st_size == 0) return 1 ;
	return 0 ;
}

/*
   ===int removeSIPURA(int NOTADMIN)===
   1.remove all of 'SIPURA' words and images.
   2.change href from /admin/voice/ to /admin , from /voice/advanced to /advanced and
                 from /voice/basic to /basic ...etc.
   ====EMILY 2006.8.23===
*/
int removeSIPURA(int ISADMIN)
{
	FILE *in, *out;
	char *line[512], *p, *p1, *tmp[512], *tmp1[512], *tmp2[512];
	char *keyword[] = { "<a href=\"http://www.sipura.com\" target=\"sipura\">",
			    "<img",
			    "<div class=\"navbar\">",
			    "                    <a href=\"/voice/\">",
			    "Copyright",
			    "<td width=130",
			    "<TD width=1 bgColor=#808080>",
			    "<td bgColor=#cfcfcf width=436>",
			    "<tr bgcolor=#eaf2ff><td>"
			   };

	if ( ISADMIN )
		 keyword[3] = "                    <a href=\"/admin/voice/";
	if ((in = fopen("/tmp/http_voip", "rb")) == NULL )
	{
		printf("%s", "Cannot open file \r\n");
		return 0;
	}
	if ((out = fopen("/tmp/http_voip1", "wb")) == NULL )
	{
		printf("%s", "Cannot open http_voip1.\r\n");
		return 0;
	}

	while(!feof(in))
	{
		fgets(line, sizeof(line), in);
		if ( strncmp(line, keyword[0], strlen(keyword[0])) && strncmp(line, keyword[1], strlen(keyword[1])) &&
		     strncmp(line, keyword[4], strlen(keyword[4])) && strncmp(line, keyword[5], strlen(keyword[5])) &&
		     strncmp(line, keyword[6], strlen(keyword[6])) && strncmp(line, keyword[7], strlen(keyword[7])) &&
		     strncmp(line, keyword[8], strlen(keyword[8])))
		{
			if (!strncmp(line, keyword[3], strlen(keyword[3])) || !strncmp(line, keyword[2], strlen(keyword[2])))
			{
				memset(tmp, 0, sizeof(tmp));
				memset(tmp1, 0, sizeof(tmp1));
				p = line;
				if ( !ISADMIN )
				{
					p1 = strstr(p, "/voice/");
					if ( p1!=NULL ) strncpy(tmp, p, p1 - p + 6);
				}
				else
				{
					p1 = strstr(p, "/admin/voice");
					if ( p1 != NULL ) strncpy(tmp, p, p1 - p + 6);
				}
				if ( p1 != NULL )
				{
					p = p1;
					p1 = strstr(p, "\n");
					if ( !ISADMIN )
						strncpy(tmp1, p + 7, p1 - p);
					else
						strncpy(tmp1, p + 13, p1 - p);
					strcat(tmp, tmp1);
					if ( !ISADMIN )
					{
						p = tmp;
						p1 = strstr(p, "<a href=\"advanced");
						if ( p1 != NULL )
						{
							memset(tmp2, 0, sizeof(tmp2));
							memset(tmp1, 0, sizeof(tmp1));
							strncpy(tmp2, p, p1 - p + 17);
							strcat(tmp2, "_admin");
							p = p1;
							p1 = strstr(p, "\n");
							strncpy(tmp1, p + 17, p1 - p);
							strcat(tmp2, tmp1);
							fputs(tmp2, out);
						}
						else
							fputs(line, out);
					}
					else
						fputs(tmp, out);
				}
				else
					fputs(line, out);
			}
			else
				fputs(line, out);
		}
	}
	fclose(in);
	fclose(out);
	return 1 ;
}

int
do_postfile_ej(char *path , webs_t stream)
{
	static char buf[10000]={ 0 };
	char server[strlen(path) + 30];
	int returnvalue = 0;

	strcpy(buf, post_buf);
	sprintf(server, "http://%s/%s", nvram_get("DHCP_MAPPING_IP"), path);
	returnvalue = http_post(server, buf, sizeof(buf));
	if ( returnvalue == 200 )
		do_ej("Success.asp", stream);
	else
		do_ej("Fail.asp" , stream);
}

int
do_getfile_ej(char *path, webs_t stream)        // emily , 8/10/2005
{
	static char buf[10000]={ 0 };
	char server[strlen(path) + 30];
	long count = 0, off = 0, offset = 0, wr = 0;
	int tmp_fd = 0;
	char *http_tmp_file[15];

	if ( !strcmp(path, "VOIP.asp") || !strcmp(path, "basic") || !strcmp(path, "voice") )
	{
		sprintf(server, "http://%s/voice/", nvram_get("DHCP_MAPPING_IP"));
		nvram_set("DHCP_MAPPING_PATH", "voice/");
	}
	else if ( !strcmp(path,"admin"))
	{
		sprintf(server, "http://%s/admin/voice/", nvram_get("DHCP_MAPPING_IP"));
		nvram_set("DHCP_MAPPING_PATH", "admin/voice/");
	}
	else if ( !strcmp(path,"advanced_admin"))
	{
		sprintf(server, "http://%s/admin/voice/advanced", nvram_get("DHCP_MAPPING_IP"));
		nvram_set("DHCP_MAPPING_PATH", "admin/voice/");
	}
	else
	{
		sprintf(server, "http://%s/voice/advanced", nvram_get("DHCP_MAPPING_IP"));
		nvram_set("DHCP_MAPPING_PATH", "voice/");
	}

	http_get(server, buf, sizeof(buf), 0);

	if ( Isblankfile() == 0 )
	{
		tmp_fd = strncmp(path, "admin", 5);
		if ( tmp_fd != 0 ) tmp_fd = 1;
		do_ej("VOIP.asp", stream);
		if (  removeSIPURA(tmp_fd) )
			do_ej("../tmp/http_voip1", stream);
		else
			do_ej("../tmp/cannot_find_file", stream); //this file is not exist. 
	}
	else
		do_ej("../tmp/cannot_find_file", stream); //this file is not exist. 
}
#endif

struct mime_handler mime_handlers[] = {
#ifdef DHCP_DOMAIN_IP_MAPPING_SUPPORT
	{ "advanced*",	"text/html", no_cache, NULL, do_getfile_ej, do_auth},        
	{ "admin*",	"text/html", no_cache, NULL, do_getfile_ej, do_auth},
	{ "voice*",	"text/html", no_cache, NULL, do_getfile_ej, do_auth},
	{ "basic*",	"text/html", no_cache, NULL, do_getfile_ej, do_auth},
	{ "VOIP.asp",	"text/html", no_cache, NULL, do_getfile_ej, do_auth},
	{ "bsipura.spa","text/html", no_cache, do_apply_post, do_postfile_ej, do_auth},
	{ "asipura.spa","text/html", no_cache, do_apply_post, do_postfile_ej, do_auth},
#endif
        { "**.asp","text/html",no_cache,NULL,do_ej, do_auth},
#ifdef NOMAD_SUPPORT
	/* add by xiaoqin for nomad, 2005.03.31*/
	{ "nomad.lxb*", "text/html", NULL, NULL, do_nomad_file, do_auth},
#endif
	//junzhao 2004.7.28 adsl status
	{ "ADSLCStatus.htm*", "text/html", no_cache, NULL, do_ej, do_auth },
	{ "SysInfo.htm*", "text/plain", no_cache, NULL, do_ej, do_auth },
	{ "**.css", "text/css", NULL, NULL, do_file, do_auth },
	{ "**.gif", "image/gif", NULL, NULL, do_file, do_auth },
	{ "**.jpg", "image/jpeg", NULL, NULL, do_file, do_auth },
	{ "**.js", "text/javascript", NULL, NULL, do_file, do_auth },
#ifdef GET_POST_SUPPORT
	{ "apply.cgi*", "text/html", no_cache, do_apply_post, do_apply_cgi, do_auth },
#else
	{ "apply.cgi*", "text/html", no_cache, NULL, do_apply_cgi, do_auth },
#endif
#ifdef BRCM
	{ "internal.cgi*", "text/html", no_cache, NULL, do_internal_cgi, do_auth },
#endif
	{ "upgrade.cgi*", "text/html", no_cache, do_upgrade_post, do_upgrade_cgi, do_auth },

#ifdef SETUP_WIZARD_SUPPORT
	{ "Gozila.cgi*", "text/html", no_cache, NULL, do_setup_wizard, do_auth },	// for setup wizard
#endif
#ifdef WL_CALIBRATION_ONBOARD_SUPPORT
	{ "calibration.cgi*", "text/html", no_cache, NULL, do_calibration, do_auth },
#endif
#ifdef BACKUP_RESTORE_SUPPORT
	{ "**.cfg", "application/octet-stream", no_cache, NULL, do_backup, do_auth },
	{ "restore.cgi**", "text/html", no_cache, do_upgrade_post, do_upgrade_cgi, do_auth },
#endif
	{ "test.bin**", "application/octet-stream", no_cache, NULL, do_file, do_auth },
//for ddm 
#ifdef DDM_SUPPORT
	{ "verizon/page_info.xml*", "text/xml", no_cache, NULL, do_ej, NULL },
	{ "verizon/get_admin_info.xml*", "text/xml", no_cache, NULL, do_ej, NULL },
	{ "verizon/get_wan_ip_address_assigment_info.xml*", "text/xml", no_cache, NULL, do_ej, NULL },
	{ "verizon/get*.xml*", "text/xml", no_cache, NULL, do_ej, do_auth },
	{ "verizon/set**.xml", "text/xml", no_cache, do_ddm_post, do_ej, do_auth },
#endif	
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

//wwzh add for DNS ENTRY
extern int ej_dns_entry_support(int eid, webs_t wp, int argc, char_t **argv);

struct ej_handler ej_handlers[] = {
	/* for all */
	{ "nvram_get", ej_nvram_get },
	{ "nvram_selget", ej_nvram_selget },
	{ "nvram_match", ej_nvram_match },
	{ "nvram_invmatch", ej_nvram_invmatch },
	{ "nvram_selmatch", ej_nvram_selmatch },
	//csh for gre
#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
	{"gre_config", ej_gre_config},
#endif
#ifdef GRE_SUPPORT
	{"ipsec_gre_config", ej_ipsec_gre_config},
#endif
#ifdef CLOUD_SUPPORT
	{"cloud_init", ej_cloud_init},
	{"index_init", ej_index_init},
	{"cloud_ipsec_init", ej_cloud_ipsec_init},
#endif
/* for eight pvc by yl 2004-11-26*/
#ifdef PVC_PER_SUPPORT
	{"nvram_match_firewall",ej_nvram_match_firewall },
	{"nvram_match_manage",ej_nvram_match_manage },
	{"nvram_match_manage1",ej_nvram_match_manage1 },
	{"nvram_match_dmz",ej_nvram_match_dmz},
	{"nvram_get_manage",ej_nvram_get_manage},
	{"nvram_flesh_manage",ej_nvram_flesh_manage},
	{"nvram_flesh_dmz",ej_nvram_flesh_dmz},
	{"nvram_get_dmz",ej_nvram_get_dmz},
	{"nvram_match_nat1",ej_nvram_match_nat1},
	{"nvram_match_nat",ej_nvram_match_nat},
	{ "get_single_ip_manage", ej_get_single_ip_manage },

	{"connection_table_forward",ej_connection_table_forward},
	{"connection_table_trigger",ej_connection_table_trigger},
	{"connection_table_firewall",ej_connection_table_firewall},
	{"connection_table_dmz",ej_connection_table_dmz},
	{"wan_connection_routing",ej_connection_table_routing},
	{"connection_table_manage",ej_connection_table_manage},
	{"alg_config",ej_alg_config_setting},
#endif
/* for eight pvc end*/



	{ "show_uptime", ej_show_uptime },
	{ "nvram_else_selmatch", ej_nvram_else_selmatch },
	{ "nvram_else_match", ej_nvram_else_match },
	{ "nvram_list", ej_nvram_list },
	{ "nvram_mac_get", ej_nvram_mac_get },
	{ "nvram_gozila_get", ej_nvram_gozila_get },
	{ "nvram_status_get", ej_nvram_status_get },
	{ "support_match", ej_support_match },
	{ "support_invmatch", ej_support_invmatch },
	{ "support_elsematch", ej_support_elsematch },
	{ "get_firmware_version", ej_get_firmware_version },
	{ "get_model_name", ej_get_model_name },
	{ "get_single_ip", ej_get_single_ip },
	{ "get_single_mac", ej_get_single_mac },
	{ "prefix_ip_get", ej_prefix_ip_get },
	{ "no_cache", ej_no_cache },
	{ "scroll", ej_scroll },
	{ "get_dns_ip", ej_get_dns_ip },
	{ "onload", ej_onload },
	{ "get_web_page_name", ej_get_web_page_name },
	{ "show_logo", ej_show_logo },
	{ "get_clone_mac", ej_get_clone_mac },
	 /*modified by zls for Management*/
	#ifdef MULTIUSER_SUPPORT
	{ "show_http_username_table",ej_show_http_username_table},
        { "get_user",ej_get_user},
        #endif
	/* for index */
	{ "show_index_setting", ej_show_index_setting },
	{ "compile_date", ej_compile_date },
	{ "compile_time", ej_compile_time },
	{ "get_wl_max_channel", ej_get_wl_max_channel },
	{ "get_wl_domain", ej_get_wl_domain },
	/* for status */
	{ "show_status", ej_show_status },
	{ "show_status_setting", ej_show_status_setting },
	//cjg:2004-3-7:PM
	{"show_ping_status", ej_show_ping_status},
	{ "localtime", ej_localtime },
	{ "dhcp_remaining_time", ej_dhcp_remaining_time },
#ifdef ARP_TABLE_SUPPORT
	{ "dump_arp_table", ej_dump_arp_table },
#endif
        /*for arp */        
        { "dumparp", ej_dumparp },//guohong 2004.07.07
	/* for dhcp */
	{ "dumpleases", ej_dumpleases },
	/* for ddm */
#ifdef DDM_SUPPORT	
	{ "ddmdumpleases", ej_ddm_dumpleases },
	{ "ddm_check_passwd", ej_ddm_check_passwd },
	{ "ddm_error_no", ej_ddm_error_no },
	{ "ddm_error_desc", ej_ddm_error_desc },
	{ "ddm_show_ipend", ej_ddm_show_ipend },
	{ "ddm_show_wanproto", ej_ddm_show_wanproto },
	{ "ddm_show_lanproto", ej_ddm_show_lanproto },
	{ "ddm_show_idletime", ej_ddm_show_idletime },
	{ "ddm_show_hwversion", ej_ddm_show_hwversion },
	
	//junzhao 2004.3.23
	{ "ddm_show_wanipaddr", ej_ddm_show_wanipaddr },
	{ "ddm_show_wannetmask", ej_ddm_show_wannetmask },
	{ "ddm_show_wangateway", ej_ddm_show_wangateway },
	{ "ddm_show_wandns", ej_ddm_show_wandns },
	{ "ddm_get_pppuser", ej_ddm_get_pppuser },
	{ "ddm_get_pppstatus", ej_ddm_get_pppstatus },
	//junzhao 2004.4.16
	{ "ddm_get_ppppassword", ej_ddm_get_ppppassword },
	//junzhao 2004.4.19
	{ "ddm_show_pppoe_acname", ej_ddm_show_pppoe_acname },
	//junzhao 2004.6.14
	{ "ddm_show_pppoe_srvname", ej_ddm_show_pppoe_srvname },
#endif	
	/* for log */
//	{ "dumplog", ej_dumplog },
	/**-------songtao-------*/
	{ "dump_syslog", ej_dump_syslog },
	{ "dump_vpnlog", ej_dump_vpnlog }, //zhangbin 2004.4.20
        {"entry_logmsg",ej_entry_logmsg},
        {"vpn_logmsg",ej_vpn_logmsg}, //zhangbin 2004.4.20


	/* zhangbin for vpn-log*/
	{ "dumpvpnlog", ej_dumpvpnlog },
#ifdef SYSLOG_SUPPORT
	{ "dump_syslog", ej_dump_syslog },
	{ "dump_log_settings", ej_dump_log_settings },
#endif
	/* zhangbin for qos */
#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
  	{"qos_config",ej_qos_config_setting},
#endif
#ifdef QOS_ADVANCED_SUPPORT
	{"dump_init_setting", ej_dump_init_setting},
	{"dump_verify_setting", ej_dump_verify_setting},
#endif

	//CSH for SNMP
	{"show_snmp_setting", ej_show_snmp_setting},
	{"snmp_config", ej_snmp_config_setting},
        //peny 2005.1.6
#ifdef HTTPS_SUPPORT
	{"set_authproto",ej_set_authproto},
	//Lai add 2005-07-04
	{"set_privproto", ej_set_authproto},	
#endif
	/* add for IGMP Proxy 2004-10-10 */
#ifdef IGMP_PROXY_SUPPORT
	{"igmp_proxy_config_setting", ej_igmp_proxy_config_setting},
	{"igmp_proxy_channel", ej_igmp_proxy_channel},
#endif

	
	/* for filter */
	{ "filter_init", ej_filter_init },
	{ "filter_summary_show", ej_filter_summary_show },
	{ "filter_ip_get", ej_filter_ip_get },
	{ "filter_port_get", ej_filter_port_get },
	{ "filter_dport_get", ej_filter_dport_get },
	{ "filter_mac_get", ej_filter_mac_get },
	{ "filter_policy_select", ej_filter_policy_select },
	{ "filter_policy_get", ej_filter_policy_get },
	{ "filter_tod_get", ej_filter_tod_get },
	{ "filter_web_get", ej_filter_web_get },
	{ "filter_port_services_get", ej_filter_port_services_get },
	/* da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
	{ "filter_checkbox_get", ej_filter_checkbox_get },
	{ "filter_wan_group_ip_node_get", ej_filter_wan_group_ip_node_get },
	{ "filter_wan_range_ip_node_get", ej_filter_wan_range_ip_node_get },
#endif
#ifdef FIREWALL_LEVEL_SUPPORT
	{ "filter_inb_init", ej_filter_inb_init },
	{ "filter_inb_summary_show", ej_filter_inb_summary_show },
	{ "filter_inb_policy_select", ej_filter_inb_policy_select },
	{ "filter_inb_policy_get", ej_filter_inb_policy_get },
	{ "filter_inb_tod_get", ej_filter_inb_tod_get },
	{ "filter_inb_mix_get", ej_filter_inb_mix_get },	
#endif
	/* for forward */
	{ "port_forward_table", ej_port_forward_table },
#ifdef SPECIAL_FORWARD_SUPPORT
	{ "spec_forward_table", ej_spec_forward_table },
#endif
#ifdef ADVANCED_FORWARD_SUPPORT//add by zhs
        {"advanced_forwarding_init",ej_advanced_forwarding_init},
#endif//end by zhs
#ifdef PORT_TRIGGER_SUPPORT
	{ "port_trigger_table", ej_port_trigger_table },
#endif
//#ifdef UPNP_FORWARD_SUPPORT
	{ "forward_upnp", ej_forward_upnp },
//#endif
	/* for route */
	{ "static_route_table", ej_static_route_table },
	{ "static_route_setting", ej_static_route_setting },
	{ "dump_route_table", ej_dump_route_table },
 //modifed by zls 2005-0110
        { "nvram_match_routing",ej_nvram_match_routing },
#ifdef RIP_EXTEND_SUPPORT
        { "rip_interface_table",ej_rip_interface_table },
#endif
	/* for ddns */
	{ "show_ddns_status", ej_show_ddns_status },
	{ "show_ddns_ip", ej_show_ddns_ip },
	/* for wireless */
//junzhao 2004.4.7
#ifdef WIRELESS_SUPPORT
	{ "wireless_active_table", ej_wireless_active_table },
	{ "wireless_filter_table", ej_wireless_filter_table },
	//wwzh
	{ "wireless_connected_table", ej_wireless_connected_table },
	{ "show_wl_wep_setting", ej_show_wl_wep_setting },
	{ "get_wep_value", ej_get_wep_value },
	{ "get_wl_active_mac", ej_get_wl_active_mac },
	{ "get_wl_value", ej_get_wl_value },
	{ "show_wpa_setting", ej_show_wpa_setting },
	/* for test */
	{ "wl_packet_get", ej_wl_packet_get },
#endif
#ifdef AOL_SUPPORT
	/* for aol */
	{ "aol_value_get", ej_aol_value_get },
	{ "aol_settings_show", ej_aol_settings_show },
#endif
#ifdef EMI_TEST
	{ "dump_emi_test_log", ej_dump_emi_test_log },
#endif
#ifdef DIAG_SUPPORT
	{ "dump_ping_log", ej_dump_ping_log },
	{ "dump_traceroute_log", ej_dump_traceroute_log },
#endif
#ifdef HSIAB_SUPPORT
        { "get_hsiab_value", ej_get_hsiab_value },
        { "show_hsiab_setting", ej_show_hsiab_setting },
        { "show_hsiab_config", ej_show_hsiab_config },
        { "dump_hsiab_db", ej_dump_hsiab_db },
        { "dump_hsiab_msg", ej_dump_hsiab_msg },
#endif  
        { "show_sysinfo", ej_show_sysinfo },
        { "show_miscinfo", ej_show_miscinfo },
        { "get_http_method", ej_get_http_method },
#ifdef BACKUP_RESTORE_SUPPORT
        { "get_backup_name", ej_get_backup_name },
#endif

	/* zhangbin for ipsec */
	{"tunnel_select",ej_tunnel_select},
	{"localgw_select",ej_localgw_select},
	{"ipsec_show_summary",ej_ipsec_show_summary},
	{"name_array",ej_name_array},
	{"spi_array",ej_spi_array},
  	{"entry_config",ej_entry_config_setting},
	{"ltype_config",ej_ltype_config_setting},
	{"rtype_config",ej_rtype_config_setting},
	{"sg_config",ej_sg_config_setting},

#ifdef CLOUD_SUPPORT
	{"lid_config",ej_lid_config_setting},
	{"rid_config",ej_rid_config_setting},
#endif

	{"ipsecadv_config",ej_ipsecadv_config_setting},
	{"keytype_config",ej_keytype_config_setting},
	{"show_ltype_setting",ej_show_ltype_setting},
	{"show_rtype_setting",ej_show_rtype_setting},
	{"show_keytype_setting",ej_show_keytype_setting},
	{"show_sg_setting",ej_show_sg_setting},
	{"show_connect_setting",ej_show_connect_setting},

	/* zhangbin 2005.3.31 for remote id type in CLoud*/
#ifdef CLOUD_SUPPORT
	{"show_localid_setting",ej_show_localid_setting},
	{"show_remoteid_setting",ej_show_remoteid_setting},
#endif


	/* add by lijunzhao 2004.4.22 for dhcprelay */
	{ "lan_dhcp_setting", ej_lan_dhcp_setting },
	
	/* for multi PVC */

	{ "connection_table", ej_connection_table },
	{ "vcc_config", ej_vcc_config_setting },
	{ "pppoe_config", ej_pppoe_config_setting },
	{ "pppoa_config", ej_pppoa_config_setting },
	{ "bridged_config", ej_bridged_config_setting },
	{ "routed_config", ej_routed_config_setting },
#ifdef STB_SUPPORT
        { "telus_config", ej_telus_config_setting },

#endif
#ifdef STB_SUPPORT
	        { "dump_stb_entry", ej_dump_stb_entry },
#endif

	/*add by lzh;*/
	{ "pvc_connection_table", ej_pvc_connection_table },
	{ "upnp_connection_table", ej_upnp_connection_table },	
#ifdef CLIP_SUPPORT
	{ "clip_config", ej_clip_config_setting },	
#endif
	{ "nvram_dslstatus_get", ej_nvram_dslstatus_get },
	{ "nvram_wanstatus_get", ej_nvram_wanstatus_get },
	{ "status_connection_table", ej_status_connection_table },
	{ "mtu_config", ej_mtu_config_setting },
	{ "macclone_config", ej_macclone_config_setting },
	{ "status_htm", ej_status_htm },
	{ "vpivci_getvalue", ej_vpivci_getvalue },
	//junzhao 2004.4.16
	{ "hwaddr_get", ej_hwaddr_get },
	//junzhao 2004.6.14
	{ "pppoe_show_htm", ej_pppoe_show_htm },

	//junzhao 2004.6.2
	{ "dump_match_condition", ej_dump_match_condition },
	{ "active_connection_table", ej_active_connection_table },
	//junzhao 2004.7.7
	{ "dump_valid_parts", ej_dump_valid_parts },
	
	/* for wan dns check proto */
	//junzhao 2004.4.13
	{ "check_proto_fordns", ej_check_proto_fordns },
	
#ifdef MPPPOE_SUPPORT
	{ "mpppoe_config", ej_mpppoe_config_setting },
	{ "mpppoe_enable_setting", ej_mpppoe_enable_setting },
#endif
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	{ "show_status2_setting", ej_show_status2_setting },
#endif	
#ifdef IPPPOE_SUPPORT
	{ "ipppoe_config", ej_ipppoe_config_setting },
	{ "ipppoe_enable_setting", ej_ipppoe_enable_setting },
#endif
	/* for wireless support */
	{ "wireless_support", ej_wireless_support },
	/* for mpppoe support */
	{ "mpppoe_support", ej_mpppoe_support },
	/* for ipppoe support */
	{ "ipppoe_support", ej_ipppoe_support },
	//junzhao 2004.6.25
	{ "clip_support", ej_clip_support },
	
	//cjg:2004-3-7:PM
	{"ping_config", ej_ping_config_setting},
	
	{"email_config", ej_email_config_setting},
	{"email_enable_config", ej_email_enable_setting},
	
	//leijun 2004-05-20
	{"dhcp_reserved_ip", ej_dhcp_reserved_ip},
	//wwzh add for WAN UPGRADE at 2004-06-16
#ifdef WAN_UPGRADE
	{"show_upgrade_info", ej_show_upgrade_info},
#endif
	{"wan_upgrade_support", ej_wan_upgrade_support},
	{"wlan_management_support", ej_wlan_management_support},
//wwzh add for DNS ENTRY
#ifdef DNS_ENTRY_SUPPORT
	{"dns_entry_table", ej_dns_entry_table},
#endif
	{"dns_entry_support", ej_dns_entry_support},
	
	
        /* for diagnostics tool add by kirby 04/07/02 */
       {"dump_diag_log",ej_dump_diagtool_log},

	// for IP Range Restricted 2004-11-17
	{"show_ip_range_setting", ej_show_ip_range_setting},
	{"ip_restrict_config", ej_ip_restrict_config},
	{"remote_management_config", ej_remote_management_config},
	/* da_f 2005.3.18 add. start... */
#ifdef NOMAD_SUPPORT
	{"vpn_account_list_get", ej_vpn_account_list_get},
	{"vpn_usernames_get", ej_vpn_usernames_get},
	{"vpn_clients_status", ej_vpn_clients_status},
#endif
	/* da_f 2005.3.18 end */

	{ "passwd_match", ej_passwd_match },
	{ "l_passwd_match", ej_l_passwd_match },


	{ NULL, NULL }
};
#endif /* !WEBS */
