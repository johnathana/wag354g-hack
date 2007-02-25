
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
 * Router firewall 
 *
 * Copyright 2002, Cybertan Corporation
 * All Rights Reserved.
 *
 * Description:
 *   We use Netfilter to be our firewall. This program will generate a policy 
 *   file, and it is the same format as the output of "iptables-save". Thus, 
 *   we can use "iptables-restore" to insert the rules in one time.
 *   It's pretty fast ;-)
 *
 *   
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Cybertan Corporation;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Cybertan Corporation.
 *
 */


/* The macro 'DEVELOPE_ENV' is for developement only. If it is defined, you
 * can compile it in your (x86) host machine and run it locally. This speeds
 * up the developing.
 *
 * Tips :
 * shell> gcc firewall.c -o fw
 * shell> ./fw
 * shell> iptables-restore /tmp/.ipt
 */
//#define DEVELOPE_ENV

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <net/if.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <utils.h>

//junzhao 2004.4.9
#include "pvc.h"
//zhs for quickvpn. 2005-4-11
#ifdef NOMAD_SUPPORT
#include <glob.h> //used for glob()
#endif

#ifndef DEVELOPE_ENV
#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <code_pattern.h>
#include <cy_conf.h>
#endif	/* DEVELOPE_ENV */
#define y_printf printf
/* Same as the file "linux/netfilter_ipv4/ipt_webstr.h" */
#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08

/* possible files path */
#define IPTABLES_SAVE_FILE	"/tmp/.ipt"
#define CRONTAB			"/tmp/crontab"
#define IPTABLES_RULE_STAT	"/tmp/.rule"

/* Known port */
#define TFTP_PORT		69	/* UDP */
#define HTTP_PORT		80	/* TCP */
#define PPTP_PORT		1723	/* TCP */
#define L2TP_PORT		1701	/* UDP */
#define ISAKMP_PORT		500	/* UDP */
/* zhangbin 2005.3.4 */
#define IKE_NATTPORT		4500	/* UDP */

/* zhangbin 2005.3.2 */
#define ESP_PPORT		0x32	
#define AH_PPORT		0x33	
/* zhangbin 2005.3.4 */
#define GRE_PPORT		47	

#define RIP_PORT		520	/* UDP */
#define DNS_PORT		53	/* UDP */
#define HTTPS_PORT		443	/* TCP */

/* Limitation definition */
#define NR_RULES		10
#define NR_IPGROUPS		5
#define NR_MACGROUPS		5

/* MARK number in mangle table */
#define MARK_OFFSET		0x10
//#define MARK_MASK		0xf0
#define MARK_DROP		0x1e
//#define MARK_ACCEPT		0x1f
//#define MARK_HTTP 		0x30
#define MARK_LAN2WAN		0x100	/* For HotSpot */

/* If it needs to reverse the rule's sequential. It is used when 
 * the MARK match/target be using. */
//#define REVERSE_RULE_ORDER

/* Define if we support AOL */
#define AOL_SUPPORT
/* Define if we support Microsoft XBox game machine */
#define XBOX_SUPPORT 1

#define SIP_ALG_SUPPORT
#define FLOOD_PROTECT
//junzhao
//status file name
#define WAN_IFNAME  "/tmp/status/wan_ifname"
#define WAN_IPADDR  "/tmp/status/wan_ipaddr"

#ifdef FLOOD_PROTECT
	#define FLOOD_RATE 200
	#define TARG_PASS  "limaccept"
	#define TARG_RST   "logreject"
#else
	#define TARG_PASS  "ACCEPT"
	#define TARG_RST   "REJECT --reject-with tcp-reset"
#endif

#if 0
#define DEBUG printf
#else
#define DEBUG(format, args...) 
#endif
//cjg
//

/* zhangbin 2005.5.8 */
#undef CLOUD_SUPPORT

//zhs add for debug
#if 0
#define ZHS_DEBUG_OUT printf
#else
#define ZHS_DEBUG_OUT DEBUG
#endif
//

/*zhs add for ad/Forward to input table*/
#ifdef ADVANCED_FORWARD_SUPPORT
//#define LAN_FORWARD
#ifdef LAN_FORWARD

struct ad_forward_input
{
	int is_need;/*0-no need,1-need*/
	char proto[4];
	char sIP_start[16];
	char sIP_end[16];
	char sPort_start[6];
	char sPort_end[6];
	char dPort_start[6];
	char dPort_end[6];
	struct ad_forward_input *next;
} t_ad_forward;

t_ad_forward adfw_t;
t_ad_forward *new_ad_t(void)
{
	t_ad_forward *p;
	p = (t_ad_forward *)malloc(sizeof(t_ad_forward));
	if(NULL == p)
	{
		printf("\n!!! new_ad_t malloc Failed!!!\n");
		return NULL;
	};
	memset(p,0,sizeof(t_ad_forward));
	return p;
}

/*************************************************************
 *find_adfw_pre:find the pre t_ad_forward point at adfw_t chain
 *              if return NULL,that is to said, adfw_t is empty.
*/
t_ad_forward *find_adfw_pre(void)
{
	t_ad_forward *pt;
	if(0 == adfw_t->need)
		return NULL;
	
	for(pt = adfw_t; ;pt =pt->next)
	{
		if((1 ==pt->need)&&(NULL==pt->next))
			return pt;
	}
}
#endif
#endif
/*end by zhs*/

/*volatile int nat_pre_flag  = 0;*///xiaoqin delete 2005.02.04 涉及此变量处全部修改
volatile int nat_post_flag = 0;
volatile int filter_input_flag = 0;
volatile int filter_forward_flag = 0;
static char All_buf[NVRAM_SPACE];
static int conn_no;
static char wan_if_addr[MAX_CHAN][16]= {
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx",
				"xxx.xxx.xxx.xxx"
				};
static char wan_if_name[MAX_CHAN][IFNAMSIZ];

static char *suspense;
static unsigned int count = 0;
static char log_accept[15];
static char log_drop[15];
static char log_reject[64];
static char wanface[IFNAMSIZ];
static char lanface[IFNAMSIZ];
static char lan_cclass[]="xxx.xxx.xxx.";
static char wanaddr[]="xxx.xxx.xxx.xxx";
static int web_lanport = HTTP_PORT; 

static FILE *ifd;	/* /tmp/.rule */
static FILE *cfd;	/* /tmp/crontab */

static unsigned int now_wday, now_hrmin;
static int webfilter = 0;
static int dmzenable = 0;
static int remotemanage = 0;

//junzhao 2004.4.9
extern int which_conn;
extern struct vcc_config *vcc_config_ptr;
/* xiaoqin add 2005.03.08 */
extern int check_ppp_proc(int num);
extern int wanstatus_info_get(char *devname, char *type, char *linkinfo);
extern int check_ppp_link(int num);
extern struct dns_lists *get_dns_list_every(int index, int way);
/*             end               */


/******************************* DEVELOPE_ENV ***********************************************/
#ifdef DEVELOPE_ENV

#undef AOL_SUPPORT
#undef XBOX_SUPPORT 

#define diag_led(a, b) printf("........ diag_led()\n")
#define eval(args...) printf(".........eval()\n")
#define nvram_safe_get(name) (nvram_get(name) ? : "")
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

#define split(word, wordlist, next, delim) \
	for (next = wordlist, \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL ; \
	     strlen(word); \
	     next = next ? : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL)

	
char *
nvram_get(const char *name)
{
	char *value;
	int i;

	struct nvram_pair {
	    char *name;
	    char *value;
	} nvp[] = {
	    "wan_ipaddr",   "200.100.77.77",
	    "lan_ipaddr",   "192.168.7.1",
	    "dmz_ipaddr",   "192.168.7.100",
	    "dmz_enable", "0",
	    "http_wanport", "8080",
	    "http_lanport", "80",
	    //"forward_spec", "",
	    "forward_port", "",
	    "filter_port",  "",
	    //"filter_ip",    "",
	    //"filter_mac",   "",
	    "lan_ifname",   "eth0",
	    "log_level",    "0",
	    "remote_upgrade", "",
	    "wk_mode", "gateway",
	    "block_wan", "1",
	    "wan_proto", "dhcp",
	    "mtu_enable", "0",
	    //"pptp_pass", "",
	    //"l2tp_pass", "0",
	    //"ipsec_pass", "",
	    //"hsiab_mode", "",
	    "block_cookie", "1",
	    "block_java", "",
	    "block_activex", "",
	    "block_proxy", "1",
	    //"multicast_pass", "",
	    "remote_management", "1",
	    "dr_wan_rx", "0",
	    //"aol_block_traffic", "",
		"port_trigger", "test1:on:both:10000-20000>1000-2000 test2:off:both:30000-40000>3000-4000",
	    "filter_rule1", "$STAT:2$NAME:policy01$$",
	    "filter_rule2", "$STAT:1$NAME:policy02$$",
	    "filter_ip_grp1", "20 21 0 0 0 0 30-40 0-0",
	    "filter_ip_grp2", "200 0 0 0 0 0 100-120 0-0",
	    "filter_mac_grp1", "22:22:22:22:22:22 44:44:44:44:44:44",
	    "filter_mac_grp2", "66:66:66:66:66:66 88:88:88:88:88:88",
	    "filter_dport_grp1",  "tcp:50-60 both:600-600",
	    "filter_dport_grp2",  "udp:2000-2000 both:600-600",
	    "filter_tod1", "0:0 23:59 0-6",
	    "filter_tod2", "0:0 23:59 0-6",
	    "filter_web_host1", "hello<&nbsp;>world<&nbsp;>friend",
	    "filter_web_host2", "",
	    "filter_web_url1", "hello<&nbsp;>world<&nbsp;>friend",
	    "filter_web_url2", "",
	    "filter_port_grp1", "FTP<&nbsp;>Telnet G",
	    "filter_port_grp2", "FTP<&nbsp;>Telnet G",
	    "filter_services",  "$NAME:008:Telnet G$PROT:004:both$PORT:009:2323:2323<&nbsp;>$NAME:003:FTP$PROT:003:tcp$PORT:005:21:21",
	    "filter", "on",
	};

	for (i=0 ; i < (sizeof(nvp)/sizeof(nvp[0])) ; i++) {
	    if (strcmp(name, nvp[i].name) == 0)
		return nvp[i].value;
	}

	return NULL;
}

static inline int
nvram_match(char *name, char *match) {
	const char *value = nvram_get(name);
	return (value && !strcmp(value, match));
}

static inline int
nvram_invmatch(char *name, char *invmatch) {
	const char *value = nvram_get(name);
	return (value && strcmp(value, invmatch));
}

char * 
range(char *start, char *end) {
	return start;
}

char *
get_wan_face(void) {
	return "eth1";
}

int
get_single_ip(char *ipaddr, int which)
{
	int ip[4]={0,0,0,0};
	int ret;

	ret = sscanf(ipaddr,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);

	return ip[which];	
}
#endif  /* DEVELOPE_ENV */
/******************************* DEVELOPE_ENV ***********************************************/
void my_nvram_safe_getall()
{
//	nvram_getall(All_buf, sizeof(All_buf));
}
char *my_nvram_safe_get(char *name)
{
#if 1
	char *tmp, *tmp1;
	char *value;
	nvram_getall(All_buf, sizeof(All_buf));
	for(tmp = All_buf; *tmp; tmp += strlen(tmp) + 1)
	{
		tmp1 = strsep(&tmp, "=");
		if(!strncmp(name, tmp1, strlen(name)))
		{
			value = strchr(tmp, '=');
			if(value != NULL)
				value ++;	
			return value;
		}
	}
	return NULL;
#else
	return nvram_safe_get(name);
#endif
}

void save2file(const char *fmt,...)
{
	char buf[10240];
	va_list args;
	FILE *fp;

	if ((fp = fopen(IPTABLES_SAVE_FILE, "a")) == NULL)
	{
		printf("Can't open /tmp/.ipt\n");
		exit(1);
	}

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args); 
	va_start(args, fmt);
	fprintf(fp, "%s", buf);
	va_end(args);

	fclose(fp);
}

int ip2cclass(char *ipaddr, char *new, int count)
{
	int ip[4];

	if(sscanf(ipaddr,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]) != 4)
		return 0;
	
	return snprintf(new, count, "%d.%d.%d.",ip[0],ip[1],ip[2]);
}

void parse_port_forward(char *wordlist){
	char var[256], *next;
	char *name, *enable, *proto, *port, *ip;
	char buff[256];

	/* zhangbin for GRE packet 2005.3.3 */
	char port_range[15];
	char *port_start;
	char *port_end;

	/* name:enable:proto:port>ip name:enable:proto:port>ip */
	foreach(var, wordlist, next) {
	
		/* zhangbin 2005.3.4 */
		int start_port;
		int end_port;
		/* zhangbin 2005.3.4 */

		enable = var;
		name = strsep(&enable, ":");
		if (!name || !enable)
			continue;
		proto = enable;
		enable = strsep(&proto, ":");
		if (!enable || !proto)
			continue;
		port = proto;
		proto = strsep(&port, ":");
		if (!proto || !port)
			continue;
		ip = port;
		port = strsep(&ip, ">");
		if (!port || !ip)
			continue;

		/* skip if it's disabled */
		if( strcmp(enable, "off") == 0 )
			continue;

		/* -A PREROUTING -i eth1 -p tcp -m tcp --dport 8899:88 -j DNAT 
		                 --to-destination 192.168.1.88:0 
		   -A PREROUTING -i eth1 -p tcp -m tcp --dport 9955:99 -j DNAT 
		                 --to-destination 192.168.1.99:0 */

		/* zhangbin 2005.3.4 */
		y_printf("Now handle port range %s\n", port);
		strcpy(port_range, port);
		y_printf("Now handle port range %s\n", port_range);
		port_end = port_range;
		port_start = strsep(&port_end, ":");
		start_port = atoi(port_start);
		end_port = atoi(port_end);
		y_printf("port range forward %d:%d\n",start_port, end_port);
		/* zhangbin 2005.3.4 */

		if( !strcmp(proto,"tcp") || !strcmp(proto,"both") ){

			/* zhangbin for GRE and ESP/AH packet 2005.3.4 */
			int flag_range,flag_ipsec;
			flag_range = 0;
			flag_ipsec = 0;
			/* zhangbin for GRE and ESP/AH packet 2005.3.4 */

			/* zhangbin for GRE packet 2005.3.3 */
			if((start_port <= PPTP_PORT) && (end_port >= PPTP_PORT))
			{
				save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", GRE_PPORT,wan_if_addr[conn_no],lan_cclass, ip);
				flag_range = 1;
			}
			/* zhangbin for GRE packet 2005.3.3 */
	
			/* zhangbin for ESP/AH packet 2005.3.4 */
			if((((start_port <= ISAKMP_PORT) && (end_port >= ISAKMP_PORT)) || ((start_port <= IKE_NATTPORT) && (end_port >= IKE_NATTPORT))) && (!strcmp(proto,"both")))
			{
				save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", ESP_PPORT,wan_if_addr[conn_no],lan_cclass, ip);
				save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", AH_PPORT,wan_if_addr[conn_no],lan_cclass, ip);
				flag_ipsec = 1;
			}

			save2file("-A PREROUTING -p tcp -m tcp -d %s --dport %s "
				  "-j DNAT --to-destination %s%s\n"
					, wan_if_addr[conn_no], port, lan_cclass, ip);

			snprintf(buff, sizeof(buff), "-A FORWARD -i %s -p tcp "
				 "-m tcp -d %s%s --dport %s -j %s\n"
				 ,wan_if_name[conn_no], lan_cclass, ip, port, log_accept);
		
			/* zhangbin for GRE packet in port range 2005.3.3 */
			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff );

			if(flag_range)
			{
				snprintf(buff, sizeof(buff), "-A FORWARD -p %d -d %s%s -j ACCEPT\n",GRE_PPORT,lan_cclass, ip);
			
				count += strlen(buff) + 1;
				suspense = realloc(suspense, count );
				strcat(suspense, buff );
			}
			/* zhangbin for GRE packet in port range 2005.3.3 */

			/* zhangbin for ESP/AH packet in port range 2005.3.4 */
			if(flag_ipsec)
			{
				snprintf(buff, sizeof(buff), "-A FORWARD -p %d -d %s%s -j ACCEPT\n",ESP_PPORT,lan_cclass, ip);

				count += strlen(buff) + 1;
				suspense = realloc(suspense, count );
				strcat(suspense, buff );

				snprintf(buff, sizeof(buff), "-A FORWARD -p %d -d %s%s -j ACCEPT\n",AH_PPORT,lan_cclass, ip);

			}
			/* zhangbin for ESP/AH packet in port range 2005.3.4 */
			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff );

		}
		if( !strcmp(proto,"udp") || !strcmp(proto,"both") ){

			/* zhangbin for ESP/AH packet 2005.3.4 */
			int flag_ipsec_udp;
			flag_ipsec_udp = 0;
	
			if(((start_port <= ISAKMP_PORT) && (end_port >= ISAKMP_PORT)) || ((start_port <= IKE_NATTPORT) && (end_port >= IKE_NATTPORT))) 
			{
				save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", ESP_PPORT,wan_if_addr[conn_no],lan_cclass, ip);
				save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", AH_PPORT,wan_if_addr[conn_no],lan_cclass, ip);
				flag_ipsec_udp = 1;
			}

			save2file("-A PREROUTING -p udp -m udp -d %s --dport %s "
				  "-j DNAT --to-destination %s%s\n"
					, wan_if_addr[conn_no], port, lan_cclass, ip);
			/* zhangbin for ESP/AH packet 2005.3.4 */
			if(flag_ipsec_udp)
			{
				snprintf(buff, sizeof(buff), "-A FORWARD -p %d -d %s%s -j ACCEPT\n",ESP_PPORT,lan_cclass, ip);
				count += strlen(buff) + 1;
				suspense = realloc(suspense, count );
				strcat(suspense, buff );

				snprintf(buff, sizeof(buff), "-A FORWARD -p %d -d %s%s -j ACCEPT\n",AH_PPORT,lan_cclass, ip);
				count += strlen(buff) + 1;
				suspense = realloc(suspense, count );
				strcat(suspense, buff );
			}
			/* zhangbin for ESP/AH packet 2005.3.4 */

			snprintf(buff, sizeof(buff), "-A FORWARD -i %s -p udp "
				 "-m udp -d %s%s --dport %s -j %s\n"
				 ,wan_if_name[conn_no], lan_cclass, ip, port, log_accept);

			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff); 
		}
	}
}

/****add by zhs for ADVANCED_FORWARDING_SUPPORT****/
#ifdef ADVANCED_FORWARD_SUPPORT
#define PORTCHAINNAME        "NATPortChain"
#define INPUTPORTCHAINNAME   "InputPortChain"
#define DNS_ENTRY_FILE "/tmp/.dns_entry"
#define PRO_EMPTY 0x0
#define PRO_TCP 0x1
#define PRO_UDP 0x2
#define sIP_EMPTY 0x0
#define sIP_S_MASK 0x1
#define sIP_E_MASK 0x2
/****************************************
 *ad_forward_input use to store ad_forward that
 *must write to IPPUT table at iptables.It is a double
 *direction chain that the first is the global adfw_t.
 */
typedef struct ad_forward_input
{
       int is_need;/*0-no need,1-need*/
       int  proto;/* &PRO_TCP,&PRO_UDP*/
       int  sIp_attr;/*0-Empty,1-sip_start exist,2-sip_end exist*/
       char sIp_start[16];
       char sIp_end[16];
       int  port_range;
       char sPort_start[6];
       char sPort_end[6];
       char dIp[16];
       char dPort_start[6];
       char dPort_end[6];
       struct ad_forward_input *next;
       struct ad_forward_input *pre;
} t_ad_forward;

t_ad_forward  adfw_t={ 0 };
t_ad_forward *new_ad_t(void)
{
	t_ad_forward *p;
	p = (t_ad_forward *)malloc(sizeof(t_ad_forward));
	if(NULL == p)
	{
		printf("\n!!! new_ad_t malloc Failed!!!\n");
			return NULL;
	};
	memset(p,0,sizeof(t_ad_forward));
	return p;
}
void show_adfw(t_ad_forward *current,char *at)
{
     ZHS_DEBUG_OUT("--------------\nshow adfw at %s:
          is_need=%d,proto=%d\n
          sIp_start=%s;sIp_end[16]=%s;\n
          port_range=%d;
          sPort_start=%s;sPort_end=%s;
          dIp=%s;dPort_start=%s;dPort_end=%s;
          next=%x;pre=%x.\n
          ----------------------\n",at,
         current->is_need,current->proto,current->sIp_start,
         current->sIp_end,current->port_range,current->sPort_start,
         current->sPort_end,current->dIp,current->dPort_start,
         current->dPort_end,current->next,current->pre);
         
}
/*************************************************************
 *find_adfw_pre:find the last object at adfw_t chain.
 *              if return NULL,that is to said, adfw_t is empty.
 */
t_ad_forward *find_adfw_last(void)
{
        t_ad_forward *pt;
        if(0 ==adfw_t.is_need)
                return NULL;
        for(pt = &adfw_t; pt;pt =pt->next )
        {
                if((1 ==pt->is_need)&&(NULL==pt->next))
                        return pt;
        }
}
void delete_adfw_t(void)
{
        t_ad_forward *last=find_adfw_last();
        if(last)
        {
                t_ad_forward *dlt;
                for(;last!=&adfw_t;)
                {
                        dlt=last;
                        last=last->pre;
                        memset(dlt,0,sizeof(t_ad_forward));
                        free(dlt);
                }
        }
        memset(&adfw_t,0,sizeof(t_ad_forward));
}
void wr_to_suspense(char *pro,char *dIp,char *dPort,char *dPortMax)
{
        char buff[256];
	
	 snprintf(buff,sizeof(buff), "-A FORWARD -i %s -p %s "
                        "-m %s -d %s --dport %s:%s -j %s\n",
                        wan_if_name[conn_no], pro,pro,
                         dIp, dPort, dPortMax, log_accept);

        count += strlen(buff) +1;
        suspense =realloc(suspense, count);
        strcat(suspense,buff);
}

void add_chain_to_accept( 
              char *pro_buf,
              char *sip_range,
              char *dIp,
              char *dPort,
              char *dPortMax,
              char *a_chain
            )
{
        char tmp_sIp[20];
        char ip_entry[20],*rangeNext,*sIp;
        int  i;
	char unip_lanip[20];
	memset(unip_lanip,0,sizeof(unip_lanip));
	strcpy(unip_lanip,nvram_safe_get("lan_ipaddr"));
      /*  int  sPortNum,dPortNum;
*/
	foreach(ip_entry,sip_range,rangeNext)
        {
              ZHS_DEBUG_OUT("&&&INPUT,ip entry=%s\n",ip_entry);
              if(strcmp(ip_entry,"E"))
              {
                    strcpy(tmp_sIp,"-s ");
                    sIp = strcat(tmp_sIp,ip_entry);
              }
              else
              {
                    sIp = " ";
              }
              ZHS_DEBUG_OUT("sIp =%s\n",sIp);
	      if(nvram_match("ipcp_netmask_enable","1"))
	      {
		      save2file("-A %s -p %s %s -m %s -d %s --dport %s:%s "
                        "-j ACCEPT\n",
                        a_chain,pro_buf,sIp,pro_buf, 
                        unip_lanip,dPort, dPortMax);
	      }
	      else
	      {
		      save2file("-A %s -p %s %s -m %s -d %s --dport %s:%s "
                        "-j ACCEPT\n",
                        a_chain,pro_buf,sIp,pro_buf, 
                        dIp,dPort, dPortMax);
	      }

         }
}
void add_chain_to_dnat( 
              char *pro_buf,
              char *sip_range,
              char *sPort,
              char *sPortMax,
              char *dIp,
              char *dPort,
              char *dPortMax,
              char *a_chain,
              int  portRange
            )
{
        char tmp_sIp[20];
        char ip_entry[20],*rangeNext,*sIp;
        int  i;
        int  sPortNum,dPortNum;
	//zhangbin
	char unip_lanaddr[20];
	memset(unip_lanaddr,0,sizeof(unip_lanaddr));
	strcpy(unip_lanaddr,nvram_safe_get("lan_ipaddr"));

    /*    printf("^^^^^^^^^^^^^^^^^^^^^^^
           add_chain_to_dnat:%s sIp-%s:%s-%s
            >dIp-%s:%s-%s,add to %s chain,
            portrange=%d\n",
            pro_buf,sip_range,sPort,sPortMax,dIp,
            dPort,dPortMax,a_chain,portRange);*/
        foreach(ip_entry,sip_range,rangeNext)
        {
              ZHS_DEBUG_OUT("ip range,ip entry=%s\n",ip_entry);
              if(strcmp(ip_entry,"E"))
              {
                    strcpy(tmp_sIp,"-s ");
                    sIp = strcat(tmp_sIp,ip_entry);
              }
              else
              {
                    sIp = " ";
              }
              if(portRange)
              {
                     sPortNum = atoi(sPort);
                     dPortNum = atoi(dPort);
                     for(i=0;i<portRange;i++)
                     {
			     if(nvram_match("ipcp_netmask_enable","1"))
			     {
	  			     save2file( "-A %s -p %s %s -m %s -d %s --dport %d "
                                      "-j DNAT --to-destination %s:%d\n",
                                      a_chain,pro_buf,sIp,pro_buf,unip_lanaddr,
                                      i+sPortNum,unip_lanaddr,i+dPortNum);
			     }
			     else
			     {
	       			     save2file( "-A %s -p %s %s -m %s -d %s --dport %d "
                                      "-j DNAT --to-destination %s:%d\n",
                                      a_chain,pro_buf,sIp,pro_buf,wan_if_addr[conn_no],
                                      i+sPortNum,dIp,i+dPortNum);
			     }
                     }
    
              }
	      else
	      {
			     
		      if(nvram_match("ipcp_netmask_enable","1"))
		     {
	   
			     save2file("-A %s -p %s %s -m %s -d %s --dport %s:%s "
                            "-j DNAT --to-destination %s:%s-%s\n",
                            a_chain,pro_buf,sIp,pro_buf,unip_lanaddr,
                            sPort, sPortMax,unip_lanaddr,dPort, dPortMax);
		     }
		      else
		      {
		                 save2file("-A %s -p %s %s -m %s -d %s --dport %s:%s "
                            "-j DNAT --to-destination %s:%s-%s\n",
                            a_chain,pro_buf,sIp,pro_buf,wan_if_addr[conn_no],
                            sPort, sPortMax,dIp,dPort, dPortMax);
		      }

#if 0 
		  if(nvram_match("ipcp_netmask_enable","1"))
		  {
			  save2file("-A PREROUTING -p %s %s -m %s -d %s --dport %s:%s "
        	                    "-j DROP\n",
                	            pro_buf,sIp,pro_buf,unip_lanaddr,
                        	    dPort, dPortMax);
		  }
#endif

	      }
         }
}

/*getIPbyDomainName(char *pDname,char *pIp):get the IP address
 *         depending the given Dnmae,and store the IP address to
 *         the pointer pIp and return 0.if the given domain name
 *         does not exit,return 1.
 */
int getIPbyDomainName(const char *pDname,char *pIp)
{
     char *ptr;
     char var[256],*next;
     char *cfg_dname,*cfg_ip;
     FILE *file = fopen(DNS_ENTRY_FILE,"r");
    
    if(NULL==file)
    {
	printf("open DNS_ENTRY_FILE failed!!\n");
        return 1;
    }
    while(fgets(var,sizeof(var),file))
    {
	ptr =strchr(var,'\n');
        if(NULL!=ptr)
               *ptr ='\0';
        cfg_dname=cfg_ip=var;
        strsep(&cfg_ip,"=");
        if(0==strcmp(cfg_dname,pDname))
        {
              strcpy(pIp, cfg_ip);
              return 0;
        }
    }
    return 1;
 }
/************************************************************
 *parse_advanced_port_forward:config the port forward function
 *     depending the configuration of advanced_forwarding_config
 *Input: cfg--the pointer that point the advanced_forwarding_config
 *       words read from FLASH.
 */

void parse_advanced_port_forward(void)
{
   char var[256], *next,*wordlist;
   char *enable,*sIp,*sPort,*sPortMax,*proto, *dIp,*dDname, *dPort,*dPortMax;
   char *sip_buf ;
   char buff[256];
   char tmp_sIp[20];
   char tmp_dIp[20];
   int  sPortNum,sPortMaxNum,dPortNum,dPortMaxNum,portRange;
   int portChainCnt =0;/*Count for NEW Port chain*/
   int i,is_forward;
   char *cfg;//ad forward config form flash.
   char ip_entry[20],*rangeNext;
   char lan_addr[20];
   int pro,entry_cnt=0;
   char *sIp_end,a_chain[25] ;
       
   delete_adfw_t();//clear adfw_t chain
   ZHS_DEBUG_OUT("adfw_t Addr=%x\n",&adfw_t);
   memset(lan_addr,0,sizeof(lan_addr));
   strcpy(lan_addr, nvram_safe_get("lan_ipaddr"));
   ZHS_DEBUG_OUT("\n***get lan ip=%s,lan_addr_len=%d****\n",lan_addr,lan_addr_len);
   ZHS_DEBUG_OUT("\n****Start advanced forwarding!***\n");
  /*on:proto:sIp:s_start_port:s_end_port>d_ip:domainName:d_start_port:d_end_port */
   cfg = nvram_safe_get("advanced_forwarding_config");
   wordlist = malloc( 1 + strlen(cfg));
   if (NULL == wordlist)
   {
       printf("\n ***Malloc Failed at function of pare_advance_port_forward!\n");
       return;
   }
   strcpy(wordlist,cfg);

   foreach(var, wordlist, next) {
      proto = var;
      ZHS_DEBUG_OUT("\n ***cfg entry:%s\n",var);
      enable = strsep(&proto, ":");
      if (!enable || !proto)
          continue;
      if(strcmp(enable,"on"))/*enable field =="off",disable,then skip*/
          continue;
      sIp = proto;
      proto = strsep(&sIp,":");
      if (!proto || !sIp)
          continue;
      sPort = sIp;
      sIp = strsep(&sPort, ":");
      if (!sIp|| !sPort)
          continue;
      sPortMax = sPort;
      sPort = strsep(&sPortMax,":");
      if (!sPort || !sPortMax)
          continue;
      dIp=sPortMax;
      sPortMax = strsep(&dIp, ">");
      if (!sPortMax || !dIp)
          continue;
      dDname = dIp;
      dIp = strsep(&dDname,":");
      if (!dIp || !dDname)
          continue;
      dPort = dDname;
      dDname = strsep(&dPort,":");
      if (!dDname || !dPort)
          continue;
      dPortMax = dPort;
      dPort = strsep(&dPortMax,":");
      if (!dPort || !dPortMax)
         continue;
      if (0 == strcmp(dIp,"255"))
      {/*dIp invalid,use DomainName to get  dIp*/
         if (getIPbyDomainName(dDname,tmp_dIp))
         {
            printf("\nConfig Error:not found DomainName %s\n",dDname);
            continue;
         }
      }
      else
      {
         strcpy (tmp_dIp,lan_cclass);
         strcat(tmp_dIp,dIp);
      }
      dIp = tmp_dIp;
      ZHS_DEBUG_OUT("\n ***dIp:%s\n",dIp);
      ZHS_DEBUG_OUT("\n Ad_port_forward cfg:\n%s:%s:%s-%s:%s:%s:%s:%s-%s\n",
              enable,sIp,sPort,sPortMax,proto, dIp,dDname, dPort, dPortMax);
                  
      /*transform the source IP to iptables command*/
      if (strcmp(sIp,"0"))
      {//sIP is not empty
          sIp_end = sIp;
	 sIp = strsep(&sIp_end,"-");
	 if (!sIp)
	      continue;
	 if(sIp_end)
	 {//sIP is a range
	    sip_buf = range(sIp,sIp_end);
	 }
	 else
	 {// single  source IP
	    sip_buf =  sIp;
	 }
      }
      else
      {//source ip can be empty
   	 sip_buf = "E";//EMPTY
         sIp = NULL;
         sIp_end = NULL;
      }
     ZHS_DEBUG_OUT("sip range=%s\n",sip_buf); 
     sPortNum = atoi(sPort);
      sPortMaxNum = atoi(sPortMax);
      dPortNum = atoi(dPort);
      dPortMaxNum = atoi(dPortMax);
      if((sPortNum != sPortMaxNum)&&(dPortNum != dPortMaxNum)
         &&(sPortNum != dPortNum)&&
         (sPortMaxNum - sPortNum) == (dPortMaxNum - dPortNum))
      {
            /*
             *port range judgement,if translating port range is regular,
             *but no the same ,and have a fixed offset, we create a New
             * chain to deal with it
             */
           portRange=sPortMaxNum -sPortNum +1;
      }
      else portRange = 0;
      pro = PRO_EMPTY;
      is_forward =1;

      if( !strcmp(proto,"tcp") || !strcmp(proto,"all") )
           pro |=PRO_TCP;
      if( !strcmp(proto,"udp") || !strcmp(proto,"all") )
           pro |=PRO_UDP;
      ZHS_DEBUG_OUT("pro=%d,portRange=%d\n",pro,portRange); 
      if(PRO_EMPTY == pro)
           continue;
      //????????,add for default configuration for FT,2005.1.24
      if(entry_cnt<28)
      {
          dIp=lan_addr;
      }
      entry_cnt++;
      //end add
      //if(!strcmp(dIp,lan_addr))
      if(nvram_match("ipcp_netmask_enable","1")||(!strcmp(dIp,lan_addr)))
     {/*if the dIP is the LAN ip of gateway,put the iptables item to
        *input table
        */
         t_ad_forward *pre =find_adfw_last();
         t_ad_forward *current;
         if(NULL ==pre)
         {// adfw_t is empty,use it!
               current = &adfw_t;
         }
         else
         {
             current = new_ad_t();
             pre->next = current;
             current->pre =pre;
         }
         current->is_need = 1;
         current->proto = pro;
         current->port_range = portRange;
         if(sIp)
         {
            current->sIp_attr |=sIP_S_MASK;
	    strcpy(current->sIp_start, sIp);
         }
         if(sIp_end)
         {
            current->sIp_attr |=sIP_E_MASK;
            strcpy(current->sIp_end, sIp_end);
         }
         strcpy(current->sPort_start,  sPort);
         strcpy(current->sPort_end, sPortMax);
         strcpy(current->dIp, dIp);
         strcpy(current->dPort_start, dPort);
         strcpy(current->dPort_end, dPortMax);
         current->next = NULL;
	 is_forward = 0;
         show_adfw(current,"set adfw");
      }
      //2004.1.24
      if(nvram_match("ipcp_netmask_enable","1"))
       	   is_forward = 0;
      /*src:139.0.0.5 dPort  255:355>5:100:200
        iptables -A PREROUTING -p tcp  -m tcp -s 139.0.0.5 -d 138.0.0.2 --dport 255:355
         -j DNAT --to-destination 192.168.6.5:100-:200
       */
      if(portRange)
      {
               /*create a NEW chain for NAT port forwarding.
                *In nat main chain,if match then jump to this sub chain
                */
               if(pro & PRO_TCP)
               {
                   memset(a_chain,0,sizeof(a_chain));
                   sprintf(a_chain,"%s%d",PORTCHAINNAME,portChainCnt);
                   save2file( "-N %s\n",a_chain);
                   save2file("-A PREROUTING -i %s -p tcp -m tcp --dport %s:%s "
                             "-j %s\n",wan_if_name[conn_no], sPort,sPortMax,
                             a_chain);
	
                   add_chain_to_dnat("tcp",sip_buf,sPort,sPortMax,
                             dIp,dPort,dPortMax,a_chain,portRange);
                   portChainCnt++;
                   if(is_forward)
                   {
                        wr_to_suspense("tcp",dIp,dPort,dPortMax);
                   }
                }
	       if(pro & PRO_UDP)
                {
                   memset(a_chain,0,sizeof(a_chain));
                   sprintf(a_chain,"%s%d",PORTCHAINNAME,portChainCnt);
                   save2file( "-N %s\n",a_chain);
                   save2file("-A PREROUTING -i %s -p udp -m udp --dport %s:%s "
                             "-j %s\n",wan_if_name[conn_no], sPort,sPortMax,
                             a_chain);
                   add_chain_to_dnat("udp",sip_buf,sPort,sPortMax,
                             dIp,dPort,dPortMax,a_chain,portRange);
                   portChainCnt++;
                   if(is_forward)
                   {
                        wr_to_suspense("udp",dIp,dPort,dPortMax);
                   }
                }
                continue;
      }
      //not in port range case
            //TCP 
     if(pro & PRO_TCP)
     {    
            add_chain_to_dnat("tcp",sip_buf,sPort,sPortMax,
                     dIp,dPort,dPortMax,"PREROUTING",0);
           if(is_forward)
           {
                 wr_to_suspense("tcp",dIp,dPort,dPortMax);
           }
         }//end of TCP
       //udp
         if(pro & PRO_UDP)
         {
	      add_chain_to_dnat("udp",sip_buf,sPort,sPortMax,
                         dIp,dPort,dPortMax,"PREROUTING",0);
	/* add to forward filter*/
           if(is_forward)
           {
               wr_to_suspense("tcp",dIp,dPort,dPortMax);
           }
      }
    
  }
  free(wordlist);
}

/************************************************************
 *parse_adv_forward_input:config the port forward function
 *     depending the configuration of advanced_forwarding_config
 *Input: NULL
 */

void parse_adv_forward_input(void)
{
      t_ad_forward *current;
      char *sip_buf;

      ZHS_DEBUG_OUT("-----parse ad fw INPUT--------\n");
      if(0 == adfw_t.is_need)
          return;
      for(current=&adfw_t;NULL!=current;current=current->next)
      {

        show_adfw(current,"ad_fw input");
        ZHS_DEBUG_OUT("$$$current->sIp_end=%x\n",current->sIp_end);
        if((current->sIp_attr & sIP_S_MASK)
         &&(current->sIp_attr & sIP_E_MASK))
        {//sIP is a range
              sip_buf = range(current->sIp_start,current->sIp_end);
        }
        else if(current->sIp_attr & sIP_S_MASK)
        {// single  source IP
              sip_buf =  current->sIp_start;
        }
        else
        {//source ip can be empty
            
             sip_buf = "E";//EMPTY
        }          
        ZHS_DEBUG_OUT("sip range=%s\n",sip_buf); 

	if(current->proto& PRO_TCP)
        {
           add_chain_to_accept("tcp",sip_buf, current->dIp,
               current->dPort_start,current->dPort_end,"INPUT");
        }
        if(current->proto & PRO_UDP)
        {
           add_chain_to_accept("udp",sip_buf, current->dIp,
               current->dPort_start,current->dPort_end,"INPUT");
        }
    
      }
      delete_adfw_t();
}
 #endif
/****end  by zhs for ADVANCED_FORWARDING_SUPPORT****/

//modify by lzh 2004/9/15
#if 0
void parse_upnp_forward(int flag)
{
#if 0
	char name[] = "forward_portXXXXXXXXXX", value[1000];
        char *wan_port0, *wan_port1, *lan_ipaddr, *lan_port0, *lan_port1, *proto;
        char *enable, *desc;
	char buff[256];
	int i;
	/* Set wan_port0-wan_port1>lan_ipaddr:lan_port0-lan_port1,proto,enable,desc */
	for(i=0 ; i<15 ; i++){
		snprintf(name, sizeof(name), "forward_port%d", i);

		strncpy(value, nvram_safe_get(name), sizeof(value));

		/* Check for LAN IP address specification */
		lan_ipaddr = value;
		wan_port0 = strsep(&lan_ipaddr, ">");
		if (!lan_ipaddr)
			continue;

		/* Check for LAN destination port specification */
		lan_port0 = lan_ipaddr;
		lan_ipaddr = strsep(&lan_port0, ":");
		if (!lan_port0)
			continue;

		/* Check for protocol specification */
		proto = lan_port0;
		lan_port0 = strsep(&proto, ":,");
		if (!proto)
			continue;

		/* Check for enable specification */
		enable = proto;
		proto = strsep(&enable, ":,");
		if (!enable)
			continue;

		/* Check for description specification (optional) */
		desc = enable;
		enable = strsep(&desc, ":,");

		/* Check for WAN destination port range (optional) */
		wan_port1 = wan_port0;
		wan_port0 = strsep(&wan_port1, "-");
		if (!wan_port1)
			wan_port1 = wan_port0;

		/* Check for LAN destination port range (optional) */
		lan_port1 = lan_port0;
		lan_port0 = strsep(&lan_port1, "-");
	        if (!lan_port1)
			lan_port1 = lan_port0;

		/* skip if it's disabled */
		if( strcmp(enable, "off") == 0 )
			continue;

		/* skip if it's illegal ip */
		if(get_single_ip(lan_ipaddr,3) == 0 || 
		   get_single_ip(lan_ipaddr,3) == 255)
			continue;

		/* -A PREROUTING -p tcp -m tcp --dport 823 -j DNAT 
		                 --to-destination 192.168.1.88:23  */
		if( !strcmp(proto,"tcp") || !strcmp(proto,"both") ){
	   		save2file("-A PREROUTING -p tcp -m tcp -d %s --dport %s "
				  "-j DNAT --to-destination %s%d:%s\n"
					, nvram_safe_get("wan_ipaddr"), wan_port0, lan_cclass, get_single_ip(lan_ipaddr,3), lan_port0);

			snprintf(buff, sizeof(buff), "-A FORWARD -p tcp "
				 "-m tcp -d %s%d --dport %s -j %s\n"
				 , lan_cclass, get_single_ip(lan_ipaddr,3), lan_port0, log_accept);

			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff );
		}
		if( !strcmp(proto,"udp") || !strcmp(proto,"both") ){
	   		save2file("-A PREROUTING -p udp -m udp -d %s --dport %s "
				  "-j DNAT --to-destination %s%d:%s\n"
				  	, nvram_safe_get("wan_ipaddr"), wan_port0, lan_cclass, get_single_ip(lan_ipaddr,3), lan_port0);

			snprintf(buff, sizeof(buff), "-A FORWARD -p udp "
				 "-m udp -d %s%d --dport %s -j %s\n"
				 , lan_cclass, get_single_ip(lan_ipaddr,3), lan_port0, log_accept);

			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff); 
		}
	}
#else
	char tmp_name[] = "upnp_contentXXX", content_buf[256], new_name[128];
	char wan_ipaddr[32], lan_ipaddr[32], proto[5], *pt, *tmp_content;
	int i, content_num = 0;
	int i_enable = 0, i_proto = 0, i_ext_port = 0, i_int_port = 0, i_int_ip = 0, tmp = 0;
	int index = 0;
	
	if (nvram_match("nat_enable", "0")) return ;
//	if (nvram_match("upnp_enable", "0")) return ;
#if 0
	tmp_content = nvram_safe_get("vcc_config");
	if (tmp_content)
	{
		pt = strchr(tmp_content, ' ');
		if (pt)
		{
			*pt = '\0';
			pt = strstr(tmp_content, "bridgedonly");
			if (pt) return ;
		}
	}
#endif
	index = atoi(nvram_safe_get("wan_active_connection"));
	if ((index < 0) || (index > MAX_CHAN)) index = MAX_CHAN;
	if (index == MAX_CHAN) return;
	else {
		if(!file_to_buf(WAN_IPADDR, content_buf, sizeof(content_buf)))
		{
			printf("no buf in %s!\n", WAN_IPADDR);
			return;
		}
		tmp_content = content_buf;
		for (i = 0; i <= index; i++)
		{
			pt = strchr(tmp_content, ' ');
			if (pt) 
			{
				if (i < index) tmp_content = pt + 1;
				*pt = '\0';
			}else break;
		}
		if (!strcmp(tmp_content, "0.0.0.0")) return;
	}
	
	strcpy(wan_ipaddr, tmp_content);
	strcpy(lan_ipaddr, nvram_safe_get("lan_ipaddr"));
	
	content_num = atoi(nvram_safe_get("upnp_content_num"));
	for (i = 0; i < content_num; i ++)
	{
		sprintf(tmp_name, "upnp_content%d", i);
		if (!nvram_invmatch(tmp_name, "")) continue;
		strcpy(content_buf, nvram_safe_get(tmp_name));
		if (!strcasecmp(content_buf, tmp_name)) continue;
		if (strlen(content_buf) < 10) continue;
		
		sscanf(content_buf, "%d:%d:%d:%d:%d:%d:%s", &i_enable, &i_proto, &i_ext_port, &i_int_port, &i_int_ip, &tmp, new_name);
		if (i_enable)
		{
			if (i_proto == 1) strcpy(proto, "tcp");
			else strcpy(proto, "udp");
			pt = strrchr(lan_ipaddr, '.');
			if (pt) *pt = '\0';
			sprintf(lan_ipaddr, "%s.%d", lan_ipaddr, i_int_ip);
			if (flag == 1)
			{
				save2file("-I PREROUTING -p %s -d %s --dport %d -j DNAT --to %s:%d\n", proto, wan_ipaddr, i_ext_port, lan_ipaddr, i_int_port);			//zhangbin 2004.12.04 for PPTP Port fording,we must fording GRE
					if(i_ext_port == PPTP_PORT)
					{
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", GRE_PPORT,wan_ipaddr, lan_ipaddr);
					}
	
				/* zhangbin for ESP/AH packet 2005.3.4 */
					if(((i_ext_port == ISAKMP_PORT) || (i_ext_port == IKE_NATTPORT)) && (!strcmp(proto,"udp")))
					{
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", ESP_PPORT,wan_ipaddr, lan_ipaddr);
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", AH_PPORT,wan_ipaddr, lan_ipaddr);
					}
				/* zhangbin for ESP/AH packet 2005.3.4 */

			}
			else if (flag == 2)
			{
				save2file("-I FORWARD -d %s -p %s --dport %d -j ACCEPT\n", lan_ipaddr, proto, i_int_port);
			//zhangbin 2004.12.04 for PPTP Port fording,we must fording GRE
					if(i_ext_port == PPTP_PORT)
					{
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", GRE_PPORT,lan_ipaddr);
					}
	
				/* zhangbin for ESP/AH packet 2005.3.4 */
					if(((i_ext_port == ISAKMP_PORT) || (i_ext_port == IKE_NATTPORT)) && (!strcmp(proto,"udp")))
					{
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", ESP_PPORT,lan_ipaddr);
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", AH_PPORT,lan_ipaddr);
					}
				/* zhangbin for ESP/AH packet 2005.3.4 */

			}

		}
	}
#endif
}
#endif

void parse_upnp_forward(int flag)
{
	char tmp_name[32], content_buf[256], new_name[128];
	char wan_ipaddr[32], lan_ipaddr[32], proto[5], *pt, *tmp_content;
	int i,j, content_num = 0;
	int i_enable = 0, i_proto = 0, i_ext_port = 0, i_int_port = 0, i_int_ip = 0, tmp = 0;
	
	if (nvram_match("nat_enable", "0")) return ;
	
	strcpy(lan_ipaddr,nvram_safe_get("lan_ipaddr"));	

	for (j=0;j<MAX_CHAN;j++)
	{
		if(!file_to_buf(WAN_IPADDR, content_buf, sizeof(content_buf)))
		{
			printf("no buf in %s!\n", WAN_IPADDR);
			return;
		}
		tmp_content = content_buf;
		for (i = 0; i <= j; i++)
		{
			pt = strchr(tmp_content, ' ');
			if (pt) 
			{
				if (i < j) tmp_content = pt + 1;
				*pt = '\0';
			}else break;
		}
		
		if (!strcmp(tmp_content, "0.0.0.0")) continue;
		
		strcpy(wan_ipaddr, tmp_content);
		//strcpy(lan_ipaddr, nvram_safe_get("lan_ipaddr"));

		sprintf(tmp_name,"upnp_content_num_%d",j);
		content_num = atoi(nvram_safe_get(tmp_name));
		for (i = 0; i < content_num; i ++)
		{
			sprintf(tmp_name, "upnp_content_%d_%d", j , i);
			if (!nvram_invmatch(tmp_name, "")) continue;
			strcpy(content_buf, nvram_safe_get(tmp_name));
			if (!strcasecmp(content_buf, tmp_name)) continue;
			if (strlen(content_buf) < 10) continue;
	
			sscanf(content_buf, "%d:%d:%d:%d:%d:%d:%s", &i_enable, &i_proto, &i_ext_port, &i_int_port, &i_int_ip, &tmp, new_name);
			
			if (i_enable)
			{
				if (i_proto == 1) strcpy(proto, "tcp");
				else strcpy(proto, "udp");
				pt = strrchr(lan_ipaddr, '.');
				if (pt) *pt = '\0';
				sprintf(lan_ipaddr, "%s.%d", lan_ipaddr, i_int_ip);
				if (flag == 1)
				{

					save2file("-I PREROUTING -p %s -d %s --dport %d -j DNAT --to %s:%d\n", proto, wan_ipaddr, i_ext_port, lan_ipaddr, i_int_port);
						//zhangbin 2004.12.04 for PPTP Port fording,we must fording GRE
					if(i_ext_port == PPTP_PORT)
					{
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", GRE_PPORT,wan_ipaddr, lan_ipaddr);
					}

				/* zhangbin for ESP/AH packet 2005.3.4 */
					if(((i_ext_port == ISAKMP_PORT) || (i_ext_port == IKE_NATTPORT)) && (!strcmp(proto,"udp")))
					{
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", ESP_PPORT,wan_ipaddr, lan_ipaddr);
						save2file("-I PREROUTING -p %d -d %s -j DNAT --to-destination %s\n", AH_PPORT,wan_ipaddr, lan_ipaddr);
					}
				/* zhangbin for ESP/AH packet 2005.3.4 */

				}

				else if (flag == 2)
				{
					save2file("-I FORWARD -d %s -p %s --dport %d -j ACCEPT\n", lan_ipaddr, proto, i_int_port);
					//zhangbin 2004.12.04 for PPTP Port fording,we must fording GRE
					if(i_ext_port == PPTP_PORT)
					{
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", GRE_PPORT,lan_ipaddr);
					}

				/* zhangbin for ESP/AH packet 2005.3.4 */
					if(((i_ext_port == ISAKMP_PORT) || (i_ext_port == IKE_NATTPORT)) && (!strcmp(proto,"udp")))
					{
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", ESP_PPORT,lan_ipaddr);
						save2file("-I FORWARD -p %d -d %s -j ACCEPT\n", AH_PPORT,lan_ipaddr);
					}
				/* zhangbin for ESP/AH packet 2005.3.4 */

				}

			}
		}
	}
}
//end of lzh modify 

void parse_spec_forward(char *wordlist){
	char var[256], *next;
	char *name, *enable, *proto, *from, *to, *ip;
	char buff[256];


	/* name:enable:proto:ext_port>ip:int_port name:enable:proto:ext_port>ip:int_port */
	foreach(var, wordlist, next) {
		enable = var;
		name = strsep(&enable, ":");
		if (!name || !enable)
			continue;
		proto = enable;
		enable = strsep(&proto, ":");
		if (!enable || !proto)
			continue;
		from = proto;
		proto = strsep(&from, ":");
		if (!proto || !from)
			continue;
		to = from;
		from = strsep(&to, ">");
		if (!to || !from)
			continue;
		ip = to;
		to = strsep(&ip, ":");
		if (!ip || !to)
			continue;

		/* skip if it's disabled */
		if( strcmp(enable, "off") == 0 )
			continue;

		/* -A PREROUTING -i eth1 -p tcp -m tcp -d 192.168.88.11 --dport 823 -j DNAT 
		                 --to-destination 192.168.1.88:23  */
		if( !strcmp(proto,"tcp") || !strcmp(proto,"both") ){
	   		save2file("-A PREROUTING -p tcp -m tcp -d %s --dport %s "
				  "-j DNAT --to-destination %s%s:%s\n"
					, wan_if_addr[conn_no], from, lan_cclass, ip, to);

			snprintf(buff, sizeof(buff), "-A FORWARD -i %s -p tcp "
				 "-m tcp -d %s%s --dport %s -j %s\n"
				 ,wan_if_name[conn_no], lan_cclass, ip, to, log_accept);

			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff );
		}
		if( !strcmp(proto,"udp") || !strcmp(proto,"both") ){
	   		save2file("-A PREROUTING -p udp -m udp -d %s --dport %s "
				  "-j DNAT --to-destination %s%s:%s\n"
					, wan_if_addr[conn_no], from, lan_cclass, ip, to);

			snprintf(buff, sizeof(buff), "-A FORWARD -p udp "
				 "-m udp -d %s%s --dport %s -j %s\n"
				 , lan_cclass, ip, to, log_accept);

			count += strlen(buff) + 1;
			suspense = realloc(suspense, count );
			strcat(suspense, buff); 
		}
	}
}

void nat_prerouting(){

	if (nvram_match("wk_mode", "gateway"))
		save2file("-A PREROUTING -i %s -d %s%s -j DROP\n", wan_if_name[conn_no], lan_cclass, "0/24");
	
	if (remotemanage)
	{
		/* zhangbin 2005.3.21 */
		char http_wanport_buf[10];
		//char *http_wanport = nvram_safe_get("http_wanport");
		//char *http_wanport_buf = malloc(strlen(http_wanport) + 1);
		memset(http_wanport_buf,0,sizeof(http_wanport_buf));
	
		//strncpy(http_wanport_buf, http_wanport, strlen(http_wanport));
		strcpy(http_wanport_buf, nvram_safe_get("http_wanport"));
		//http_wanport_buf[strlen(http_wanport)] = '\0';

		save2file("-A PREROUTING -p tcp -m tcp -d %s --dport %s "
			  "-j DNAT --to-destination %s:%d\n"
			  , wan_if_addr[conn_no]
			  , http_wanport_buf/*nvram_safe_get("http_wanport")*/
			  , nvram_safe_get("lan_ipaddr")
			  , web_lanport);
		//free(http_wanport_buf);
	}
#if RELCOUN == ICELAND
        {
		char lan_ip[20];
		char *wordlist,*next,var[100];

		strcpy(lan_ip,nvram_safe_get("lan_ipaddr"));
		wordlist = nvram_safe_get("deflt_rmt_access");
		//printf("deflt_rmt_access cfg is:%s\n",wordlist);
		foreach(var, wordlist, next)
			save2file("-A PREROUTING -p tcp -m tcp -s %s -d %s --dport 80 "
				  "-j DNAT --to-destination %s:%d\n"
				   , var
				   , wan_if_addr[conn_no]
				   , lan_ip
				   , web_lanport);
	}
#endif
	/* xiaoqin add start 2005.04.02 */
	#ifdef NOMAD_SUPPORT
	save2file("-A PREROUTING -p tcp -m tcp -d %s --dport %d "
			  "-j DNAT --to-destination %s:%d\n"
			  , wan_if_addr[conn_no]
			  , HTTPS_PORT
			  , nvram_safe_get("lan_ipaddr")
			  , HTTPS_PORT);
	#endif
	/* ******* xiaoqin end *******/
	/* ICMP packets are always redirected to INPUT chains */
#if 0
	save2file("-A PREROUTING -p icmp -d %s -j DNAT --to-destination %s\n"
		          , wan_if_addr[conn_no]
			  , nvram_safe_get("lan_ipaddr"));
#endif
	/* Enable remote upgrade */
	if (nvram_match("remote_upgrade", "1"))
		save2file("-A PREROUTING -p udp -m udp -d %s --dport %d "
			  "-j DNAT --to-destination %s\n"
			  , wan_if_addr[conn_no], TFTP_PORT
			  , nvram_safe_get("lan_ipaddr"));

	/* Initiate suspense string for  parse_port_forward() */

	/******* xiaoqin delete 2005.02.04 ******/
	/*suspense = malloc(1);
	*suspense = 0;*/
	/************** delete end **************/


	/* Port forwarding */
	if (nvram_match("wk_mode", "gateway")){
		/*if(nat_pre_flag == 0){*///xiaoqin
			parse_upnp_forward(1);
			parse_spec_forward(nvram_safe_get("forward_spec"));
			parse_port_forward(nvram_safe_get("forward_port"));

#ifdef ADVANCED_FORWARD_SUPPORT
                          /*add by zhs for advance port forwarding,Oct 22,2004*/
                         parse_advanced_port_forward();
			 if(nvram_match("ipcp_netmask_enable","1"))
			 {
				 save2file("-A PREROUTING -i %s -p tcp -m tcp -d %s --dport 23 -j DROP\n",wan_if_name[conn_no],nvram_safe_get("lan_ipaddr"));
				 save2file("-A PREROUTING -i %s -p tcp -m tcp -d %s --dport 80 -j DROP\n",wan_if_name[conn_no],nvram_safe_get("lan_ipaddr"));
			 }
#endif

		
#ifdef PORT_TRIGGER_SUPPORT
	/* Port triggering */
#ifdef IPCP_NETMASK_SUPPORT
	//zhangbin
	if(!nvram_match("ipcp_netmask_enable","1"))
#endif
		save2file("-A PREROUTING -d %s -j TRIGGER --trigger-type dnat\n",
			 wan_if_addr[conn_no]/*, lan_cclass*/);
#endif
		/*}*///xiaoqin
	}

	/* DMZ forwarding */
	if (dmzenable)//if (dmzenable && (nat_pre_flag == 0)) xiaoqin modify
	{
#if 0
   		save2file("-A PREROUTING -d %s -j DNAT --to-destination %s%s\n"
			  , wan_if_addr[conn_no], lan_cclass
			  , nvram_safe_get("dmz_ipaddr"));
#else
		//we need block icmp packet?? cjg:2004-4-16
	save2file("-A PREROUTING -p tcp -d %s -j DNAT --to-destination %s%s\n"
			  , wan_if_addr[conn_no], lan_cclass
			  , nvram_safe_get("dmz_ipaddr"));
	save2file("-A PREROUTING -p udp -d %s -j DNAT --to-destination %s%s\n"
			  , wan_if_addr[conn_no], lan_cclass
			  , nvram_safe_get("dmz_ipaddr"));

	/* zhangbin for GRE packet 2005.3.3 */
	save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", GRE_PPORT,wan_if_addr[conn_no],lan_cclass, nvram_safe_get("dmz_ipaddr"));

	/* zhangbin for ESP/AH packet 2005.3.4 */
	save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", ESP_PPORT,wan_if_addr[conn_no],lan_cclass, nvram_safe_get("dmz_ipaddr"));
	save2file("-A PREROUTING -p %d -d %s -j DNAT --to %s%s\n", AH_PPORT,wan_if_addr[conn_no],lan_cclass, nvram_safe_get("dmz_ipaddr"));
	/* zhangbin for ESP/AH packet 2005.3.4 */

#endif
	}
	/*if(nat_pre_flag == 0)
		nat_pre_flag = 1;*///xiaoqin delete
}

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
#define WAN_MPPPOE_IFNAME "/tmp/status/wan_mpppoe_ifname"
#define WAN_IPPPOE_IFNAME "/tmp/status/wan_ipppoe_ifname"
void mp_nat_filter_postrouting(int flag){
	int i = 0;
	char name[50];
	char *str_name;
	char *tmp_name;
	if ( nvram_match("wk_mode", "gateway")){
	char ipaddr[140];	
	char *str_ipaddr;
	char *tmp_ipaddr;
		//junzhao
	#if defined(MPPPOE_SUPPORT)
		if(!file_to_buf(WAN_MPPPOE_IFNAME, name, sizeof(name)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	        	return;
		}
	
		if(!file_to_buf(WAN_MPPPOE_IPADDR, ipaddr, sizeof(ipaddr)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_IPADDR);
	        	return;
		}

		str_name = name;
		str_ipaddr = ipaddr;
		
		for(i = 0; i < MAX_CHAN; i ++)
		{
			tmp_name = strsep(&str_name, " ");
			if(!strncmp(tmp_name, "Down", 4))
				continue;

			tmp_ipaddr = strsep(&str_ipaddr, " ");
			if(flag == 0)	
				save2file("-A POSTROUTING -o %s -j MASQUERADE\n", tmp_name);
				//save2file("-A POSTROUTING -o %s -j SNAT --to %s\n", tmp_name, tmp_ipaddr);
			else if (nvram_invmatch("dr_wan_rx", "0"))
				save2file("-A INPUT -p udp -m udp --dport %d -j %s\n"
					, RIP_PORT, "ACCEPT");
	
		}		
	#endif	
		
	#if defined(IPPPOE_SUPPORT)
		if(!file_to_buf(WAN_IPPPOE_IFNAME, name, sizeof(name)))
		{
			printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	        	return;
		}
		if(!file_to_buf(WAN_IPPPOE_IPADDR, ipaddr, sizeof(ipaddr)))
		{
			printf("no buf in %s!\n", WAN_IPPPOE_IPADDR);
	        	return;
		}

		str_name = name;
		str_ipaddr = ipaddr;
		
		for(i=0; i < MAX_CHAN; i ++)
		{
			tmp_name = strsep(&str_name, " ");
			if(!strncmp(tmp_name, "Down", 4))
				continue;	
			
			tmp_ipaddr = strsep(&str_ipaddr, " ");
			if(flag == 0)
				save2file("-A POSTROUTING -o %s -j MASQUERADE\n", tmp_name);
				//save2file("-A POSTROUTING -o %s -j SNAT --to %s\n", tmp_name, tmp_ipaddr);
			else if (nvram_invmatch("dr_wan_rx", "0"))
				save2file("-A INPUT -p udp -m udp --dport %d -j %s\n"
				, RIP_PORT, "ACCEPT");
	
		}	
	#endif
	}
}
#endif

void nat_postrouting(){
	
	 
	if ( nvram_match("wk_mode", "gateway")){

#ifdef SIP_ALG_SUPPORT
	save2file("-A POSTROUTING -p udp -m udp -o %s --sport 5060:5070 -j MASQUERADE "
			"--to-ports 5056-5071\n", wan_if_name[conn_no]);
#endif
	
		save2file("-A POSTROUTING -o %s -j MASQUERADE\n", wan_if_name[conn_no]);
		//2004.7.30
		//save2file("-A POSTROUTING -o %s -j SNAT --to %s\n", wan_if_name[conn_no], wan_if_addr[conn_no]);

		/* Allow "LAN to LAN" loopback connection.
		 * DMZ and port-forwarding could match it. */
		if(nat_post_flag == 0)	
			save2file("-A POSTROUTING -o %s -s %s%s -d %s%s -j MASQUERADE\n"
		 	  , lanface, lan_cclass, "0/24", lan_cclass, "0/24" );
		//junzhao 2004.7.14
		//save2file("-A POSTROUTING -s %s%s -p icmp -j DROP\n", 
		// 	  lan_cclass, "0/24");
	}
	if(nat_post_flag == 0)
		nat_post_flag = 1;

}

void parse_port_filter(char *wordlist){
	char var[256], *next;
	char *protocol, *lan_port0, *lan_port1;


	/* Parse protocol:lan_port0-lan_port1 ... */
	foreach(var, wordlist, next) {
		lan_port0 = var;
		protocol = strsep(&lan_port0, ":");
		if (!protocol || !lan_port0)
			continue;
		lan_port1 = lan_port0;
		lan_port0 = strsep(&lan_port1, "-");
		if (!lan_port0 || !lan_port1)
			continue;

		if( !strcmp(protocol,"disable") )
			continue;

		/* -A FORWARD -i br0 -p tcp -m tcp --dport 0:655 -j logdrop 
		   -A FORWARD -i br0 -p udp -m udp --dport 0:655 -j logdrop */
		if( !strcmp(protocol,"tcp") || !strcmp(protocol,"both") ){
			save2file("-A FORWARD -i %s -p tcp -m tcp --dport %s:%s -j %s\n",
				  lanface, lan_port0, lan_port1, log_drop);
		}
		if( !strcmp(protocol,"udp") || !strcmp(protocol,"both") ){
			save2file("-A FORWARD -i %s -p udp -m udp --dport %s:%s -j %s\n", 
				  lanface, lan_port0, lan_port1, log_drop);
		}
	}
}

/* Return 1 for match, 0 for accept, -1 for partial. */
static int find_pattern(const char *data, size_t dlen,
			const char *pattern, size_t plen,
			char term, 
			unsigned int *numoff,
			unsigned int *numlen)
{
	size_t i, j, k;

	//DEBUGP("find_pattern `%s': dlen = %u\n", pattern, dlen);
	if (dlen == 0)
		return 0;

	if (dlen <= plen) {
		/* Short packet: try for partial? */
		if (strncmp(data, pattern, dlen) == 0)
			return -1;
		else return 0;
	}

	for(i=0; i<= (dlen - plen); i++){
		if( memcmp(data + i, pattern, plen ) != 0 ) continue;	

		/* patten match !! */	
		*numoff=i + plen;
		for (j=*numoff, k=0; data[j] != term; j++, k++)
			if( j > dlen ) return -1 ;	/* no terminal char */

		*numlen = k;
		return 1;
	}

	return 0;
}

int match_wday(char *wday)
{
	int wd[7]={0, 0, 0, 0, 0, 0, 0};
	char sep[]=",";
	char *token;
	int st, end;	
	int i;

	token = strtok(wday, sep);
	while( token != NULL ){
		if( sscanf(token, "%d-%d", &st, &end) == 2)
			for(i=st ; i<= end ; i++)
				wd[i] = 1;
		else
			wd[atoi(token)] = 1;

		token = strtok(NULL, sep);
	}

	DEBUG("week map=%d%d%d%d%d%d%d\n", wd[0], wd[1], wd[2], wd[3], wd[4], wd[5], wd[6]);
	DEBUG("now_wday=%d, match_wday()=%d\n", now_wday, wd[now_wday]);
	return wd[now_wday];
}	

int match_hrmin(int hr_st, int mi_st, int hr_end, int mi_end) 
{
	unsigned int hm_st, hm_end;

	/* convert into %d%2d format */
	hm_st  = hr_st  * 100 + mi_st;
	hm_end = hr_end * 100 + mi_end;

	if( hm_st < hm_end ){
		if( now_hrmin < hm_st || now_hrmin > hm_end ) return 0;
	}
	else{		// time rotate
		if( now_hrmin < hm_st && now_hrmin > hm_end ) return 0;
	}

	return 1;
}

/*
 * PARAM - seq : Seqence number.
 *
 * RETURN - 0 : Data error or be disabled until in scheduled time.
 *          1 : Enabled.
 */
int schedule_by_tod(int seq)
{
	char todname[]="filter_todxxx";
	char *todvalue;
	int sched = 0, allday = 0;
	int hr_st, hr_end;	/* hour */
	int mi_st, mi_end;	/* minute */
	char wday[128];
	int intime=0;

	/* Get the NVRAM data */
	sprintf(todname, "filter_tod%d", seq);
	todvalue = nvram_safe_get(todname);

	DEBUG("%s: %s\n", todname, todvalue);
	if ( strcmp(todvalue, "") == 0 )
		return 0;

	/* Is it anytime or scheduled ? */
	if(strcmp(todvalue, "0:0 23:59 0-0") == 0 ||
	   strcmp(todvalue, "0:0 23:59 0-6") == 0 ){
		sched = 0;
	}
	else{
		sched = 1;
	       	if( strcmp(todvalue, "0:0 23:59") == 0 )
			allday = 1;
		if( sscanf(todvalue, "%d:%d %d:%d %s", &hr_st, &mi_st,
				&hr_end, &mi_end, wday) != 5 )
			return 0; /* error format */
	}	

	DEBUG("sched=%d, allday=%d\n", sched, allday);
	/* Anytime */
	if( !sched ){
		save2file("-A lan2wan -j grp_%d\n", seq);
		return 1;
	}

	/* Scheduled */
	if( allday ){	/* 24-hour, but not everyday */
		char wday_st[64], wday_end[64];	/* for crontab */
		int rotate = 0;		/* wday continugoue */
		char sep[]=",";		/* wday seperate character */
		char *token;		
		int st, end;	

		/* If its format looks like as "0-1,3,5-6" */
		if( *wday == '0')
			if( *(wday + strlen(wday) - 1) == '6' )
				rotate = 1;

		/* Parse the 'wday' format for crontab */
		token = strtok(wday, sep);
		while( token != NULL ){
			/* which type of 'wday' ? */
			if( sscanf(token, "%d-%d", &st, &end) != 2)
				st = end = atoi(token);

			if( rotate == 1 && st == 0)
				sprintf(wday_end + strlen(wday_end), ",%d", end);
			else if( rotate == 1 && end == 6)
				sprintf(wday_st  + strlen(wday_st),  ",%d", st );
			else{
				sprintf(wday_st  + strlen(wday_st),  ",%d", st );
				sprintf(wday_end + strlen(wday_end), ",%d", end);
			}

			token = strtok(NULL, sep);
		}

		/* Write to crontab for triggering the event */
		/* "wday_xx + 1" can ignor the first character ',' */
		fprintf(cfd, "%02d %2d * * %s root filter add %d\n", 
					mi_st, hr_st, wday_st + 1, seq); 
		fprintf(cfd, "%02d %2d * * %s root filter del %d\n", 
					mi_end, hr_end, wday_end + 1, seq); 
		if( match_wday(wday) )
			intime=1;
	}
	else{		/* Nither 24-hour, nor everyday */
		/* Write to crontab for triggering the event */
		fprintf(cfd, "%02d %2d * * %s root filter add %d\n", 
					mi_st, hr_st, wday, seq); 
		fprintf(cfd, "%02d %2d * * %s root filter del %d\n", 
					mi_end, hr_end, wday, seq); 
		if( match_wday(wday) && 
		    match_hrmin(hr_st, mi_st, hr_end, mi_end) )
			intime=1;
	}
	
	/* Would it be enabled now ? */
	DEBUG("intime=%d\n", intime);
	if( intime ){
		save2file("-A lan2wan -j grp_%d\n", seq);
		return 1;
	}

	return 0;
}

/* *modify history **************
  * * Jan 29,2005 modify by xiaoqin */	
void macgrp_chain(int seq, unsigned int mark, int urlenable)
{
	char var[256], *next;
	char buf[100];
	char *wordlist;

	sprintf(buf, "filter_mac_grp%d", seq);
	wordlist = nvram_safe_get(buf);
	if (strcmp(wordlist, "") == 0)
		return;

/* xiaoqin modify 2005.01.29 */
#ifndef FILTER_WAN_SELECT_SUPPORT
	if (mark == MARK_DROP) {
		foreach(var, wordlist, next) {
			save2file("-A grp_%d -m mac --mac-source %s -j %s\n"
				  ,seq, var, log_drop);
		}
	}
	else {
		foreach(var, wordlist, next) {
			save2file("-A grp_%d -m mac --mac-source %s -j advgrp_%d\n"
				  ,seq, var, seq);

			/*
			mark = urlenable  ? mark : webfilter  ? MARK_HTTP : 0;
			if (mark) {
				save2file("-A macgrp_%d -p tcp -m tcp --dport %d -m mac "
					  "--mac-source %s -j MARK --set-mark %d\n"
					  , seq, HTTP_PORT, var, mark);
			}
			*/
		}
	}
#else
	foreach(var, wordlist, next) {
		save2file("-A grp_%d -m mac --mac-source %s -j wangrp_%d\n"
				  ,seq, var, seq);
		}
#endif /* xiaoqin modify end */
}

/* *modify history **************
  * * Jan 29,2005 modify by xiaoqin */	
void ipgrp_chain(int seq, unsigned int mark, int urlenable)
{
	char buf[256];
	char var1[256], *wordlist1, *next1;
	char var2[256], *wordlist2, *next2;
	char from[100], to[100];
	int  a1 = 0, a2 = 0;


	sprintf(buf, "filter_ip_grp%d", seq);
	wordlist1 = nvram_safe_get(buf);
	if( strcmp(wordlist1, "") == 0)
		return ;

	foreach(var1, wordlist1, next1) {
		if( sscanf(var1, "%d-%d", &a1, &a2) == 2 ){
			if(a1 == 0 && a2 == 0)	/* unset */
				continue;
			if(a1 == 0)		/* from 1 */
				a1 = 1;

			snprintf(from, sizeof(from), "%s%d", lan_cclass, a1);
			snprintf(to,   sizeof(to),   "%s%d", lan_cclass, a2);
			/* The return value of range() is global string array */
			wordlist2 = range(from, to);
		}
		else if( sscanf(var1, "%d", &a1) == 1 ){
			if(a1 == 0)		/* unset */
				continue;

			snprintf(buf, sizeof(buf), "%s%d", lan_cclass, a1);
			wordlist2 = buf;
		}
		else
			continue;

		DEBUG("range=%s\n", wordlist2);

/* xiaoqin modify 2005.01.29 */
#ifndef FILTER_WAN_SELECT_SUPPORT
		if (mark == MARK_DROP) {
			foreach(var2, wordlist2, next2) {
				save2file("-A grp_%d -s %s -j %s\n",
						seq, var2, log_drop);
			}
		}
		else {
			foreach(var2, wordlist2, next2) {
				save2file("-A grp_%d -s %s -j advgrp_%d\n",
						seq, var2, seq);

				/*
				mark = urlenable  ? mark : webfilter  ? MARK_HTTP : 0;
				if (mark){ 
					save2file("-A ipgrp_%d -p tcp -m tcp --dport %d -s %s "
						  "-j MARK --set-mark %d\n", seq, HTTP_PORT,
						  var2, mark);
				}
				*/
			}
		}
#else
		foreach(var2, wordlist2, next2) {
			save2file("-A grp_%d -s %s -j wangrp_%d\n",
						seq, var2, seq);
		}
#endif /* xiaoqin modify end */
	}
}

#if defined(FILTER_WAN_SELECT_SUPPORT)
/****************************************************************
* wangrp_chain -- wangrp chain
* DESCRIPTION
*    This routine deal with the wangrp chain
* Input: N/A
* Output: N/A
* modification history
*----------------------------
* 1.00, Jan 29, 2005, xiaoqin write.
****************************************************************/
void wangrp_chain(int seq, unsigned int mark, int urlenable)
{
	char buf1[256], buf2[256];
	char var1[256], *wordlist1, *next1;
	char var2[256], *wordlist2, *next2;
	char var3[256], *wordlist3, *next3;
	char from[100], to[100];
	char target[20];
	int  a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0;
	int nWgrp = 0, nWrange = 0;
	

	if (mark == MARK_DROP) 
		strncpy(target, log_drop, sizeof(log_drop));
	else 
		sprintf(target, "advgrp_%d", seq);

	sprintf(buf1, "filter_wan_ip_grp%d", seq);
	wordlist1 = nvram_safe_get(buf1);
	if (0 != strcmp(wordlist1, ""))
	{
		foreach(var1, wordlist1, next1) {
			save2file("-A wangrp_%d -d %s -j %s\n", seq, var1, target);
		}
	}
	else
		nWgrp = 1;

	sprintf(buf2, "filter_wan_ip_range%d", seq);
	wordlist2 = nvram_safe_get(buf2);
	if (0 != strcmp(wordlist2, ""))
	{
		foreach(var2, wordlist2, next2) {
			if( sscanf(var2, "%d.%d.%d.%d.%d", &a1, &a2, &a3, &a4, &a5) == 5 ){
				if(a4 > a5)	/* unset */
					continue;
				snprintf(from, sizeof(from), "%d.%d.%d.%d", a1, a2, a3, a4);
				snprintf(to,   sizeof(to),   "%d.%d.%d.%d", a1, a2, a3, a5);
				/* The return value of range() is global string array */
				wordlist3 = range(from, to);
			}
			else
				continue;

			foreach(var3, wordlist3, next3) 
				save2file("-A wangrp_%d -d %s -j %s\n", seq, var3, target);
		}
	}
	else
		nWrange = 1;

	/* if LAN_IP == NULL, all match */
	if( 1 == nWgrp && 1 == nWrange)
	{
		printf("@@@@@This is maybe anything wrong!@@@@@\n");
		return;
		//save2file("-A wangrp_%d -j %s\n",	seq, target);
	}
}
#endif/* FILTER_WAN_SELECT_SUPPORT */
/* function end write by xiaoqin */


/* *modify history **************
  * * Jan 29,2005 modify by xiaoqin */	
void portgrp_chain(int seq, unsigned int mark, int urlenable)
{
	char var[256], *next;
	char *wordlist;
	char buf[100];
	char target[12];//xiaoqin modify
	char *protocol, *lan_port0, *lan_port1;


	sprintf(buf, "filter_dport_grp%d", seq);
	wordlist = nvram_safe_get(buf);
	if (strcmp(wordlist, "") == 0)
		return;

/* Determine the filter target */
/* xiaoqin modify 2005.01.29 */
#if defined(FILTER_WAN_SELECT_SUPPORT)
	sprintf(target, "wangrp_%d", seq);
#else
	if (mark == MARK_DROP) 
	    strncpy(target, log_drop, sizeof(log_drop));
	else 
	    sprintf(target, "advgrp_%d", seq);
#endif
/* xiaoqin modify end */

	/* Parse protocol:lan_port0-lan_port1 ... */
	foreach(var, wordlist, next) {
		lan_port0 = var;
		protocol = strsep(&lan_port0, ":");
		if (!protocol || !lan_port0)
			continue;
		lan_port1 = lan_port0;
		lan_port0 = strsep(&lan_port1, "-");
		if (!lan_port0 || !lan_port1)
			continue;

		if (!strcmp(protocol,"disable"))
			continue;

		/* -A grp_* -p tcp -m tcp --dport 0:655 -j logdrop 
		   -A grp_* -p udp -m udp --dport 0:655 -j logdrop */
		if( !strcmp(protocol,"tcp") || !strcmp(protocol,"both") ){
			save2file("-A grp_%d -p tcp -m tcp --dport %s:%s -j %s\n",
				  seq, lan_port0, lan_port1, target);
		}
		if( !strcmp(protocol,"udp") || !strcmp(protocol,"both") ){
			save2file("-A grp_%d -p udp -m udp --dport %s:%s -j %s\n", 
				  seq, lan_port0, lan_port1, target);
		}
	}
}

void advgrp_chain(int seq, unsigned int mark, int urlenable)
{
	char nvname[100];
	char *wordlist, word[1024], *next;
	char *services, srv[1024], *next2;
	char delim[] = "<&nbsp;>";


	/* filter_services=$NAME:006:My ICQ$PROT:002:17$PORT:009:5000:5010<&nbsp;>.. */
	services= malloc(strlen(nvram_safe_get("filter_services")) + 1);
	if(services == NULL)
		return;
	
	sprintf(services, "%s", nvram_safe_get("filter_services"));
	/* filter_port_grp5=My ICQ<&nbsp;>Game boy */
	sprintf(nvname, "filter_port_grp%d", seq);
	wordlist = nvram_safe_get(nvname);
	split(word, wordlist, next, delim) {
	    DEBUG("word=%s\n", word);

	    split(srv, services, next2, delim) {
		int len = 0;
		char *name, *prot, *port;
		char protocol[100], ports[100];

		if ((name=strstr(srv, "$NAME:")) == NULL ||
		    (prot=strstr(srv, "$PROT:")) == NULL ||
		    (port=strstr(srv, "$PORT:")) == NULL) 
		    continue;

		/* $NAME */
		if (sscanf(name, "$NAME:%3d:", &len) != 1 || strlen(word) != len) 
		    continue;
		if (memcmp(name + sizeof("$NAME:nnn:") - 1, word, len) != 0) 
		    continue;

		/* $PROT */
		if (sscanf(prot, "$PROT:%3d:", &len) != 1) 
		    continue;
		strncpy(protocol, prot + sizeof("$PROT:nnn:") - 1, len);
		protocol[len] = '\0';

		/* $PORT */
		if (sscanf(port, "$PORT:%3d:", &len) != 1) 
		    continue;
		strncpy(ports, port + sizeof("$PORT:nnn:") - 1, len);
		ports[len] = '\0';

		DEBUG("match:: name=%s, protocol=%s, ports=%s\n", 
			word, protocol, ports);
		if (!strcmp(protocol,"tcp") || !strcmp(protocol,"both"))
			save2file("-A advgrp_%d -p tcp -m tcp --dport %s -j %s\n",
				  seq, ports, log_drop);
		if (!strcmp(protocol,"udp") || !strcmp(protocol,"both"))
			save2file("-A advgrp_%d -p udp -m udp --dport %s -j %s\n", 
				  seq, ports, log_drop);
		if (!strcmp(protocol,"icmp"))
			save2file("-A advgrp_%d -p icmp -j %s\n", 
				  seq, log_drop);
	    }
	}
	
	/* filter_web_host2=hello<&nbsp;>world<&nbsp;>friend */
	sprintf(nvname, "filter_web_host%d", seq);
	wordlist = nvram_safe_get(nvname);
	DEBUG("advgrp_chain:%s\n", wordlist);
	if (strcmp(wordlist, "")) {
	    save2file("-A advgrp_%d -p tcp -m tcp -m webstr --url \"%s\" -j %s\n"
		    ,seq, wordlist, log_reject);//xiaoqin modify 2005.07.15
	}
	/* filter_web_url3=hello<&nbsp;>world<&nbsp;>friend */
	sprintf(nvname, "filter_web_url%d", seq);
	wordlist = nvram_safe_get(nvname);
	DEBUG("1:advgrp_chain:%s\n", wordlist);
	if (strcmp(wordlist, "")) {
	    save2file("-A advgrp_%d -p tcp -m tcp -m webstr --url \"%s\" -j %s\n"
		    ,seq, wordlist, log_reject);
	}
	/* Others will be accepted */
	save2file("-A advgrp_%d -j %s\n", seq, log_accept);
	free(services);
	DEBUG("2:advgrp_chain:%s\n", wordlist);
}

/* *modify history **************
  * * Jan 29,2005 modify by xiaoqin */	
void lan2wan_chains(void)
{
	time_t ct;      /* Calendar time */
	struct tm *bt;  /* Broken time */
	int seq;
	char buf[]="filter_rulexxx";
	char *data;
#if defined(FILTER_WAN_SELECT_SUPPORT)
	char checkbuf1[]="filter_lan_checkboxXXX";//xiaoqin
	char checkbuf2[]="filter_wan_checkboxXXX";//xiaoqin
	char *cswitch_data;//xiaoqin
#endif
	int offset, len;
	unsigned int mark = 0;
	int up = 0;
	int  urlfilter = 1;
	//char urlhost[] ="filter_url_hostxxx";
	//char urlkeywd[]="filter_url_keywdxxx";

	/* Get local calendar time */
	time(&ct);
	bt=localtime(&ct);

	/* Convert to 3-digital format */
	now_hrmin = bt->tm_hour * 100 + bt->tm_min;
	now_wday = bt->tm_wday;
	
	/* keep the status using bitmap */
	if( (ifd=fopen(IPTABLES_RULE_STAT, "w")) == NULL ){
		printf("Can't open %s\n", IPTABLES_RULE_STAT);
		exit(1);
	}

	/* Open the crontab file for modification */
	if( (cfd=fopen(CRONTAB, "w")) == NULL ){
		printf("Can't open %s\n", CRONTAB);
		exit(1);
	}	
	fprintf(cfd, "PATH=/sbin:/bin:/usr/sbin:/usr/bin\n\n");

#if defined(REVERSE_RULE_ORDER)
	for( seq=NR_RULES; seq >= 1; seq-- ){
#else
	for( seq=1; seq <= NR_RULES; seq++ ){
#endif
		sprintf(buf, "filter_rule%d", seq);
		data = nvram_safe_get(buf);

		if( strcmp( data, "") == 0)
			up = 0;
		else
			up = schedule_by_tod(seq);

		fprintf(ifd, "%d,", up);
	}

	fclose(ifd);
	fclose(cfd);

	for( seq=1; seq <= NR_RULES; seq++ ){
		sprintf(buf, "filter_rule%d", seq);
		data = nvram_safe_get(buf);
		if( strcmp(data , "") == 0)
			continue;

		/* Check if it is enabled */
		find_pattern(data, strlen(data), "$STAT:", 
			sizeof("$STAT:")-1, '$', &offset, &len);

		if( len < 1 ) 
			continue;	/* error format */
	
		strncpy(buf, data + offset, len);
		*(buf+len) = 0;
		DEBUG("STAT: %s\n", buf);

		switch(atoi(buf)){
		case 1:		/* Drop it */
			mark = MARK_DROP;
			break;
		case 2:		/* URL checking */
			mark = MARK_OFFSET + seq;
			break;
		default:	/* jump to next iteration */
			continue;
		}

		/*
		sprintf(urlhost,  "filter_url_host%d",  seq);
		sprintf(urlkeywd, "filter_url_keywd%d", seq);
		if (nvram_match(urlhost, "") && nvram_match(urlkeywd, ""))
			urlfilter = 0;

		DEBUG("host=%s, keywd=%s\n", urlhost, urlkeywd);
		*/

		/* ********modify by xiaoqin 2005.01.29 **************/
	#if defined(FILTER_WAN_SELECT_SUPPORT)
		sprintf(checkbuf1, "filter_lan_checkbox%d", seq);
		cswitch_data = nvram_safe_get(checkbuf1);
		if( (0 == strcmp(cswitch_data , "")) || (0 == strcmp(cswitch_data , "0")))
		{
			macgrp_chain(seq, mark, urlfilter);
			ipgrp_chain(seq, mark, urlfilter);
			portgrp_chain(seq, mark, urlfilter);
		}
		else
		{
			save2file("-A grp_%d -j wangrp_%d\n", seq, seq);
		}
		
		sprintf(checkbuf2, "filter_wan_checkbox%d", seq);
		cswitch_data = nvram_safe_get(checkbuf2);
		if( (0 == strcmp(cswitch_data , "")) || (0 == strcmp(cswitch_data , "0")))
		{
			wangrp_chain(seq, mark, urlfilter);
		}
		else
		{
			if (mark == MARK_DROP) 
				save2file("-A wangrp_%d -j %s\n",	seq, log_drop);
			else
				save2file("-A wangrp_%d -j advgrp_%d\n", seq, seq);
		}
	#else
		macgrp_chain(seq, mark, urlfilter);
		ipgrp_chain(seq, mark, urlfilter);
		portgrp_chain(seq, mark, urlfilter);
	#endif
		/* **************xiaoqin modify end ****************/
		advgrp_chain(seq, mark, urlfilter);
	}
}
#ifdef PORT_TRIGGER_SUPPORT
static void parse_trigger_out(char *wordlist)
{
	char var[256], *next;
	char *name, *enable, *proto;
	char *wport0, *wport1, *lport0, *lport1;

	/* port_trigger=name:[on|off]:[tcp|udp|both]:wport0-wport1>lport0-lport1 */
	foreach(var, wordlist, next) {
	enable = var;
	name = strsep(&enable, ":");
	if (!name || !enable)
		continue;
	proto = enable;
	enable = strsep(&proto, ":");
	if (!enable || !proto)
		continue;
	wport0 = proto;
	proto = strsep(&wport0, ":");
	if (!proto || !wport0)
		continue;
	wport1 = wport0;
	wport0 = strsep(&wport1, "-");
	if (!wport0 || !wport1)
		continue;
	lport0 = wport1;
	wport1 = strsep(&lport0, ">");
	if (!wport1 || !lport0)
		continue;
	lport1 = lport0;
	lport0 = strsep(&lport1, "-");
	if (!lport0 || !lport1)
		continue;

	/* skip if it's disabled */
	if( strcmp(enable, "off") == 0 )
		continue;

	if( !strcmp(proto,"tcp") || !strcmp(proto,"udp") ){
		save2file("-A trigger_out -p %s -m %s --dport %s:%s "
			"-j TRIGGER --trigger-type out --trigger-proto %s "
			"--trigger-match %s-%s --trigger-relate %s-%s\n",
			proto, proto, wport0, wport1, proto,
			wport0, wport1, lport0, lport1);
	}
	else if(!strcmp(proto,"both") ){
		save2file("-A trigger_out -p tcp -m tcp --dport %s:%s "
			"-j TRIGGER --trigger-type out --trigger-proto all "
			"--trigger-match %s-%s --trigger-relate %s-%s\n",
			wport0, wport1, wport0, wport1, lport0, lport1);
		save2file("-A trigger_out -p udp -m udp --dport %s:%s "
			"-j TRIGGER --trigger-type out --trigger-proto all "
			"--trigger-match %s-%s --trigger-relate %s-%s\n",
			wport0, wport1, wport0, wport1, lport0, lport1);
	}
	}
}
#endif /* PORT_TRIGGER_SUPPORT */

//add by zhs for snmp limit management,Nov 19,2004
//    #ifdef SNMP_LIMIT_IP_SUPPORT
 #define SNMP_CHAIN "SNMP_LIMIT_CHAIN"
/***************************************************************
 *parse_snmp_ip_limit:for snmp management,only the specified snmp
 *                    station can manage the router.
 */
void parse_snmp_ip_limit(char *cfg)
{
        char *lmt_type, *ip,*ip_end;
        int lmt_number;
        char *ip_range,*next;
        char var[500];

       if( NULL==cfg)
       {
              printf("\n***ERROR:parse_snmp_ip_limit input parameter is NULL\n");
              return;
       }
       ip = cfg;
       lmt_type =strsep(&ip,":") ;
       ZHS_DEBUG_OUT("limit type =%s\n",lmt_type);
      if(!lmt_type)
             return ;
      lmt_number=((int)*lmt_type) - 0x30;
      ZHS_DEBUG_OUT("limit type =%d(%s)\n",lmt_number,lmt_type);
     /*
      *SNMP port:161/tcp,161/udp,
      *SNMP TRAP port :162/tcp,162/udp
      *but now we only use 161/udp
      */
      if(lmt_number>2)
      {
          // save2file("-A INPUT -i %s -p ! icmp -m ! icmp --dport 161 -j ACCEPT\n",wan_if_name[conn_no]);
           return ;
       }
       if(0 == lmt_number)
       {

           save2file("-A INPUT -i %s -p udp -m udp --dport 161 -j ACCEPT\n",
                            wan_if_name[conn_no]);
	   return;
       }
       //else
        ip_end = ip;
        ip = strsep(&ip_end,"-");
        ZHS_DEBUG_OUT("ip =%s end_ip=%s\n",ip,ip_end);

        if(!ip)
             return;
       if(1 == lmt_number)
       {
        // save2file("-A INPUT -i %s -s % -p ! icmp -m ! icmp --dport 161 -j ACCEPT\n",
              //              wan_if_name[conn_no], ip);
           save2file("-A INPUT -i %s -s %s -p udp -m udp --dport 161 -j ACCEPT\n",
                            wan_if_name[conn_no], ip);
	   save2file("-A INPUT -i %s -p udp -m udp --dport 161 -j DROP\n",wan_if_name[conn_no]);
           return ;
      }
      if((2 == lmt_number)&&(NULL != ip_end))
      {
            if(0 > strcmp(ip_end,ip))
            {
                  char *temp = ip_end;
                  ip_end = ip;
                  ip = temp;
            }
            ip_range = range(ip,ip_end);
            ZHS_DEBUG_OUT("ip_range:\n%s\n",ip_range);
            save2file("-N %s \n",SNMP_CHAIN);
            save2file("-A INPUT -i %s -p udp -m udp --dport 161 -j %s\n",
                        wan_if_name[conn_no],SNMP_CHAIN);
            foreach(var,ip_range,next)
            {
              // save2file("-A %s -i %s -p ! icmp -m ! icmp -s %s --dport 161 -j ACCEPT\n",
             //       SNMP_CHAIN, wan_if_name[conn_no], var);
             ZHS_DEBUG_OUT("ipnet=%s\n",var);
             save2file("-A %s -i %s -p udp -m udp -s %s --dport 161 -j ACCEPT\n",
                    SNMP_CHAIN, wan_if_name[conn_no], var);
             }
       }
       /*drop the other snmp packets*/
       save2file("-A INPUT -i %s -p udp -m udp --dport 161 -j DROP\n",wan_if_name[conn_no]);
}
//#endif											     

		  
void filter_input(){
	char *targ;
	char var[256], *wordlist, *next;	
	char from_ip[16], to_ip[14];
	char *start_ip=from_ip;
	char *end_ip = to_ip;
	char from_buf[20],to_buf[20];
	char *from=from_buf;
	char *to = to_buf;
	int ip_r;
	char range_buf[20];
	char *range_all = range_buf;//xiaoqin modify 2005.03.08

	
	/* Filter known SPI state */
	if(filter_input_flag == 0)
	{
		save2file("-A INPUT -m state --state INVALID -j DROP\n"
	          "-A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT\n"
	          "-A INPUT -i lo -m state --state NEW -j ACCEPT\n"
	          "-A INPUT -i %s -m state --state NEW -j %s\n"
		  , lanface, "ACCEPT");
		  
	//junzhao 2005.1.26 for cloud
	#ifdef CLOUD_SUPPORT
		if(nvram_match("cloud_enable", "1") && nvram_match("cloud_dhcp_enable", "1"))
		{
			char wlan_agent_ip[16];
			memset(wlan_agent_ip, 0, sizeof(wlan_agent_ip));
			strcpy(wlan_agent_ip, nvram_safe_get("cloud_dhcp_agent"));
			if(nvram_match("lan_proto", "dhcprelay"))
			{
				char wlan_dhcpser_ip[16];
				memset(wlan_dhcpser_ip, 0, sizeof(wlan_dhcpser_ip));
				strcpy(wlan_dhcpser_ip, nvram_safe_get("cloud_dhcp_server"));
				save2file("-A INPUT -i %s -s %s -d %s -j %s\n", CLOUD_WLANFACE, nvram_safe_get("dhcpserver_ipaddr"), wlan_agent_ip, "DROP");
				save2file("-A INPUT -i %s -s %s -d %s -j %s\n", lanface, wlan_dhcpser_ip, nvram_safe_get("lan_ipaddr"), "DROP");
			}
			
			if(nvram_match("wl_management_enable", "0"))
			{
				//for http
				save2file("-A INPUT -i %s -d %s -p tcp --dport 80 -j %s\n", CLOUD_WLANFACE, wlan_agent_ip, "DROP");
				//for snmp 
				save2file("-A INPUT -i %s -d %s -p tcp --dport 161 -j %s\n", CLOUD_WLANFACE, wlan_agent_ip, "DROP");
			}	
					
			save2file("-A INPUT -i %s -m state --state NEW -j %s\n", CLOUD_WLANFACE, "ACCEPT");
		}	
	#endif
		
	/* Routing protocol, RIP, accept */
	
		if (nvram_invmatch("dr_wan_rx", "0"))
			save2file("-A INPUT -p udp -m udp --dport %d -j %s\n"
				, RIP_PORT, TARG_PASS/*"ACCEPT"*/);
		
	/* CWMP (TR-069) accept */
		if (nvram_invmatch("cwmp_enable", "0"))
			save2file("-A INPUT -p tcp -m tcp --dport %d -j %s\n"
				, 12345, TARG_PASS/*"ACCEPT"*/);

	/* Remote Management 
	 * Use interface name, destination address, and port to make sure
	 * that it's redirected from WAN */
		if (remotemanage)
		{
			strcpy(range_all,nvram_safe_get("range_all"));
			ip_r = atoi(range_all);
			//printf("remote..i=%d\n",ip_r);
			if(ip_r == 0)
			{
				save2file("-A INPUT -p tcp -m tcp -d %s --dport %d -j limaccept\n"
				  , nvram_safe_get("lan_ipaddr")
				  , web_lanport);
			}
			else
			{			
				strcpy(start_ip,"range_start_ip");
				strcpy(end_ip,"range_end_ip");
				strcpy(from,nvram_safe_get(start_ip));
				strcpy(to,nvram_safe_get(end_ip));
				wordlist = range(from, to);
				foreach(var, wordlist, next) 
					save2file("-I INPUT  -s %s -d %s -p tcp --dport %d -j limaccept\n", var, nvram_safe_get("lan_ipaddr"),web_lanport);
			}
		}	
#if RELCOUN == ICELAND
		{
			char lan_ip[20];

			strcpy(lan_ip,nvram_safe_get("lan_ipaddr"));

			wordlist = nvram_safe_get("deflt_rmt_access");
			//printf("deflt_rmt_access cfg is:%s\n",wordlist);
			foreach(var, wordlist, next)
			{
			//printf("var is:%s,next is:%s\n",var,next);
			save2file("-I INPUT  -s %s -d %s -p tcp --dport %d -j limaccept\n"
					, var,lan_ip ,web_lanport);
			}
		}
#endif
		/* xiaoqin add start  2005.04.02 */
		#ifdef NOMAD_SUPPORT
		save2file("-A INPUT -p tcp -m tcp -d %s --dport %d -j limaccept\n"
				  , nvram_safe_get("lan_ipaddr")
				  , HTTPS_PORT);
		#endif
		/* ***** xiaoqin end *********/
	}
	//add by zhs for snmp limit management,Nov 19,2004
	// #ifdef SNMP_LIMIT_IP_SUPPORT
	if( nvram_match("snmp_enable","1"))
	{
	       parse_snmp_ip_limit(nvram_safe_get("snmp_ip_limit"));
	 }
	// #endif
	#ifdef ADVANCED_FORWARD_SUPPORT 
        parse_adv_forward_input();
	#endif
                  
	if (nvram_match("remote_mgt_https", "1"))
		save2file("-A INPUT -i %s -p tcp -m tcp --dport 443 -j DROP\n", wan_if_name[conn_no]);
		
	//zhangbin 2005.4.21
#ifdef IPCP_NETMASK_SUPPORT
	if(!nvram_match("ipcp_netmask_enable","1"))
#endif
	save2file("-A INPUT -i %s -p tcp -m tcp --dport 80 -j DROP\n", wan_if_name[conn_no]);

	//zhangbin 2005.4.21
#ifdef IPCP_NETMASK_SUPPORT
	if(!nvram_match("ipcp_netmask_enable","1"))
#endif
		save2file("-A INPUT -i %s -p tcp -m tcp --dport 23 -j DROP\n", wan_if_name[conn_no]);

	/* ICMP request from WAN interface */
	//targ = nvram_match("block_wan", "1") ? log_drop : log_accept ;
	targ = nvram_match("block_wan", "1") ? log_drop : TARG_PASS/*"ACCEPT"*/;
	if(filter_input_flag == 0)
		save2file("-A INPUT -p icmp -j %s\n", targ);
//2004-4-3
	/* zhangbin remove this line -- 2005.5.16 */
	//save2file("-A INPUT -i %s -j wan2lan\n", wan_if_name[conn_no]);
	/* Remote Upgrade */
	if(filter_input_flag == 0){
		/*
		if(!remotemanage)*/
		{
			if( nvram_match("remote_upgrade", "1") ){
			
				save2file("-I INPUT -p udp -m udp --dport %d -j %s\n"
				  , TFTP_PORT, /*"ACCEPT"*/TARG_PASS);
			}
		}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	mp_nat_filter_postrouting(1);
#endif
	
	/* zhangbin 2005.5.16 */
#ifdef FLOOD_PROTECT
	save2file("-A INPUT -i %s -j wan2lan\n", wan_if_name[conn_no]);
#endif

	/* Drop those packets we are NOT recognizable */
		save2file( "-A INPUT -j %s\n", log_drop);
	}
	if(filter_input_flag == 0)
		filter_input_flag = 1;
}


void filter_output(){

}


void filter_forward()
{
	/* zhangbin 2005.5.8 */
	//char *mtuptr = nvram_safe_get("mtu_config");
	char mtu_cfg[80],*mtuptr = mtu_cfg;
	char *tmp;

	/* zhangbin 2005.5.8 */
	memset(mtu_cfg,0,sizeof(mtu_cfg));
	strcpy(mtu_cfg,nvram_safe_get("mtu_config"));

/********************* xiaoqin modify start 2005.05.25 ***********************/
	if(filter_forward_flag == 0)
	{
		if (nvram_match("wk_mode", "gateway"))
			parse_upnp_forward(2);

	//junzhao 2005.1.26 for cloud lans isolation
	#ifdef CLOUD_SUPPORT
		if(nvram_match("cloud_enable", "1") && nvram_match("cloud_dhcp_enable", "1"))
		{
			save2file("-A FORWARD -i %s -o %s -j DROP\n", nvram_safe_get("lan_ifname"), CLOUD_WLANFACE);
			save2file("-A FORWARD -i %s -o %s -j DROP\n", CLOUD_WLANFACE, nvram_safe_get("lan_ifname"));
			
			//junzhao 2005.2.25
			if(nvram_match("filter", "on"))
				save2file("-A FORWARD -i %s -m state --state NEW -j logaccept\n", CLOUD_WLANFACE);
		}
	#endif
	}
	
	tmp = strsep(&mtuptr, " ");
	if(strstr(wan_if_name[conn_no], "ppp") && check_ppp_proc(conn_no))
	{
		save2file("-I FORWARD -i %s -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n", lanface, wan_if_name[conn_no]);
		save2file("-I FORWARD -i %s -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n", wan_if_name[conn_no], lanface);
	}
	else
	{
		if(tmp[0] == '0')
		{
			save2file("-I FORWARD -i %s -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n", lanface, wan_if_name[conn_no]);
			save2file("-I FORWARD -i %s -o %s -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n", wan_if_name[conn_no], lanface);
		}
	}
/*********************** xiaoqin modify end 2005.05.25 ***********************/

	/* Accept the redirect, might be seen as INVALID, packets */
	
	//junzhao 2004.12.20 add for excute filter forward when firewall off
	if(nvram_match("filter", "on"))
	{
		if(filter_forward_flag == 0)
		{
#ifdef CWMP_SUPPORT
			if( nvram_match("WAN_Internet_Access", "0") ){
				save2file("-A FORWARD -j %s\n", log_drop);
			}
#endif	
			save2file("-A FORWARD -i %s -o %s -j ACCEPT\n", lanface, lanface);

			/* Drop the wrong state, INVALID, packets */
			save2file("-A FORWARD -m state --state INVALID -j DROP\n");
		}
	}
	
	/* DROP packets for PPTP pass through. */
	if (nvram_match("pptp_pass", "0"))
		save2file("-A FORWARD -o %s -p tcp -m tcp --dport %d -j %s\n"
					, wan_if_name[conn_no], PPTP_PORT, log_drop);

	/* DROP packets for PPTP pass through. */
	if (nvram_match("l2tp_pass", "0"))
		save2file("-A FORWARD -o %s -p udp -m udp --dport %d -j %s\n"
					, wan_if_name[conn_no], L2TP_PORT, log_drop);

	/* DROP packets for IPsec pass through */
	if (nvram_match("ipsec_pass", "0"))
	{
		save2file("-A FORWARD -o %s -p udp -m udp --dport %d -j %s\n"
					, wan_if_name[conn_no], ISAKMP_PORT, log_drop);
		//zhangbin 2005.3.3
		save2file("-A FORWARD -o %s -p %d -j %s\n"
					, wan_if_name[conn_no], ESP_PPORT, log_drop);
		save2file("-A FORWARD -o %s -p %d -j %s\n"
					, wan_if_name[conn_no], AH_PPORT, log_drop);
	}


	//junzhao 2004.12.20 add for excute filter forward when firewall off
	if(nvram_match("filter", "on"))
	{
		/* Filter Web application */
		if (webfilter)
		{
			save2file("-A FORWARD -i %s -o %s -p tcp -m tcp --dport %d "
				"-m webstr --content %d -j %s\n", 
				lanface, wan_if_name[conn_no], HTTP_PORT, webfilter,log_reject);
			save2file("-A FORWARD -i %s -o %s -p tcp -m tcp --dport 8080 "
				"-m webstr --content %d -j %s\n", 
				lanface, wan_if_name[conn_no], webfilter,log_reject);
		}
	}
	
#ifdef PORT_TRIGGER_SUPPORT
	/* Port trigger by user definition */
	/* Incoming connection will be accepted, if it match the port-ranges. */
	save2file("-A FORWARD -i %s -o %s -j TRIGGER --trigger-type in\n", wan_if_name[conn_no], lanface);
	if(filter_forward_flag == 0)
		save2file("-A FORWARD -i %s -j trigger_out\n", lanface);
#endif

	//junzhao 2004.12.20 add for excute filter forward when firewall off
	if(nvram_match("filter", "on"))
	{
		/* Filter setting by user definition */
		if(filter_forward_flag == 0){
			save2file("-A FORWARD -i %s -j lan2wan\n", lanface);

			/* Filter by destination ports "filter_port" */
			parse_port_filter(nvram_safe_get("filter_port"));
		}
	}
	
	if(filter_forward_flag == 0){
		/* port-forwarding accepting rules */
		if( *suspense != 0 )
			save2file( "%s", suspense);
		//free(suspense);//xiaoqin 2005.03.04

		/* DMZ forwarding */
		if (dmzenable)
		{
			save2file("-A FORWARD -o %s -d %s%s -j %s\n", lanface, lan_cclass
				, nvram_safe_get("dmz_ipaddr"), log_accept);

			/* zhangbin for GRE packet 2005.3.3 */
			save2file("-A FORWARD -p %d -d %s%s -j ACCEPT\n",GRE_PPORT,lan_cclass, nvram_safe_get("dmz_ipaddr"));
		
			/* zhangbin for ESP/AH packet 2005.3.4 */	
			save2file("-A FORWARD -p %d -d %s%s -j ACCEPT\n",ESP_PPORT,lan_cclass, nvram_safe_get("dmz_ipaddr"));
			save2file("-A FORWARD -p %d -d %s%s -j ACCEPT\n",AH_PPORT,lan_cclass, nvram_safe_get("dmz_ipaddr"));
			/* zhangbin for ESP/AH packet 2005.3.4 */	
		}

		/* xiaoqin delete anything of codes here 2005.03.14 */
	
	#ifdef PORT_TRIGGER_SUPPORT
		parse_trigger_out(nvram_safe_get("port_trigger"));
	#endif
	}
	if(filter_forward_flag == 0)
		filter_forward_flag = 1;
}


void get_gateway(char *wan_ifname, char *gateway)
{
	int which = atoi(&wan_ifname[3]), i;
	char readbuf[140], *tmpstr = readbuf, *ptr;
	if(strstr(wan_ifname, "ppp") || !strcmp(wan_ifname, "Down"))
		return;
	if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
		return;
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		ptr = strsep(&tmpstr, " ");
		if(i == which)
		{
			strcpy(gateway, ptr);
			return;
		}
	}

	/*
	memset(word, 0, sizeof(word));
	foreach(word, readbuf, next)
	{
		if(which -- == 0)
		{
			strcpy(gateway, word);
			return;
		}
	}
	*/
	return;
}

//junzhao 2004.6.2
//create the condition of pvc selection
void create_match_flag(char *wan_ifname, char *conf)
{
	//char dstipaddr[16], dstnetmask[16], srcipaddr[16], srcnetmask[16], srcmac[18], pro[2], srcport[6], dstport[6], tos[3], buf[200], *cur = buf;
	//2004.9.16
	char dstipaddr[16], dstnetmask[16], srcipaddr[16], srcnetmask[16], srcmac[18], pro[2], srcport[6], dstport[6], cos[3], type[5], vid[5], lengthmin[6], lengthmax[6], tos[3], buf[200], *cur = buf;
	
	char gateway[16];

	memset(dstipaddr, 0, sizeof(dstipaddr));
	memset(dstnetmask, 0, sizeof(dstnetmask));
	memset(srcipaddr, 0, sizeof(srcipaddr));
	memset(srcnetmask, 0, sizeof(srcnetmask));
	memset(srcmac, 0, sizeof(srcmac));
	memset(pro, 0, sizeof(pro));
	memset(srcport, 0, sizeof(srcport));
	memset(dstport, 0, sizeof(dstport));
	memset(tos, 0, sizeof(tos));
	
	//2004.9.16
	memset(cos, 0, sizeof(cos));
	memset(type, 0, sizeof(type));
	memset(vid, 0, sizeof(vid));
	memset(lengthmin, 0, sizeof(lengthmin));
	memset(lengthmax, 0, sizeof(lengthmax));
	
	memset(gateway, 0, sizeof(gateway));

	//junzhao 2004.9.16
	//sscanf(conf, "%16[^:]:%16[^:]:%16[^:]:%16[^:]:%18[^:]:%2[^:]:%6[^:]:%6[^:]:%3[^\n]", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos);
	sscanf(conf, "%16[^:]:%16[^:]:%16[^:]:%16[^:]:%18[^:]:%2[^:]:%6[^:]:%6[^:]:%3[^:]:%3[^:]:%5[^:]:%5[^:]:%6[^:]:%6[^\n]", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos, cos, type, vid, lengthmin, lengthmax);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "-d %s/%s -s %s/%s ", dstipaddr, dstnetmask, srcipaddr, srcnetmask);
	cur += strlen(buf);
	if(strcmp(srcmac, "00-00-00-00-00-00"))
	{
		sprintf(cur, "-m mac --mac-source %s ", srcmac);
		cur = buf+strlen(buf);
	}
	if(strcmp(pro, "0"))
	{	
		if(!strcmp(pro, "1"))
		{
			sprintf(cur, "-p tcp -m tcp ");
			cur = buf+strlen(buf);
		}
		else if(!strcmp(pro, "2"))
		{
			sprintf(cur, "-p udp -m udp ");
			cur = buf+strlen(buf);
		}
		if(strcmp(srcport, "0"))
		{
			sprintf(cur, "--sport %s ", srcport);
			cur = buf+strlen(buf);
		}
		if(strcmp(dstport, "0"))
		{
			sprintf(cur, "--dport %s ", dstport);
			cur = buf+strlen(buf);
		}
	}
	
	//2004.9.16
	if(strcmp(cos, "0") || strcmp(vid, "0") || strcmp(type, "0"))
	{
		sprintf(cur, "-m packet ");
		cur = buf+strlen(buf);
		
		if(strcmp(cos, "0"))
		{
			sprintf(cur, "--cos 0x%s ", cos);
			cur = buf+strlen(buf);
		}
		if(strcmp(type, "0"))
		{
			sprintf(cur, "--proto 0x%s ", type);
			cur = buf+strlen(buf);
		}
		if(strcmp(vid, "0"))
		{
			sprintf(cur, "--vid %s ", vid);
			cur = buf+strlen(buf);
		}
	}
	if(strcmp(lengthmin, "0") || strcmp(lengthmax, "0"))
	{
		sprintf(cur, "-m length --length %s:%s ", lengthmin, lengthmax);
		cur = buf+strlen(buf);
	}

	if(strcmp(tos, "0"))
	{
		sprintf(cur, "-m dscp --dscp 0x%s ", tos);
		cur = buf+strlen(buf);
	}
	
	if(strstr(wan_ifname, "ppp"))
	{
		//junzhao 2005.2.3 for pppd fail
		int timeout = 5;
		while(wanstatus_info_get(wan_ifname, "gateway", &gateway[0]) < 0 && timeout--);
	}
		//while(wanstatus_info_get(wan_ifname, "gateway", &gateway[0]) < 0);
	else
		get_gateway(wan_ifname, &gateway[0]);
	
	if(strcmp(gateway, "0.0.0.0"))
		save2file("-I FORWARD -i %s %s -j ROUTE --gw %s\n", lanface, buf, gateway);
	
	//save2file("-I PREROUTING -i %s %s -j MARK --set-mark %d\n", lanface, buf, flag);
	/*
	//junzhao 2004.7.19 //add default gw for localout packet
	if(which_tmp == atoi(&wan_ifname[3]))
	{
		int add_defaultgw = 1;
		if((strstr(wan_ifname, "ppp") && !check_ppp_link(which_conn)))
			add_defaultgw = 0;
		if(add_defaultgw)
			save2file("-I OUTPUT -j ROUTE --gw %s\n", gateway);
	}
	*/
	return;
}

//junzhao 2004.7.19 //add default gw for localout packet
void add_local_defaultgw()
{
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	char ifname[6], readbuf[60], *next;
	int which = which_tmp, add_flag = 1;
	
	if(which == MAX_CHAN)
		return;
	
	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
		return;
	
	memset(ifname, 0, sizeof(ifname));	
	foreach(ifname, readbuf, next)
	{
		if(which-- == 0)
			break;
	}

	if((strstr(ifname, "ppp") && !check_ppp_link(which_tmp)) || (!strcmp(ifname, "Down")))
		add_flag = 0;
	
	if(add_flag)
	{
		char gateway[16];
		memset(gateway, 0, sizeof(gateway));
		if(strstr(ifname, "ppp"))
		{
			//junzhao 2005.2.3 for pppd fail
			int timeout = 5;
			while(wanstatus_info_get(ifname, "gateway", &gateway[0]) < 0 && timeout--);
		}
			//while(wanstatus_info_get(ifname, "gateway", &gateway[0]) < 0);
		else
			get_gateway(ifname, &gateway[0]);
	
		//printf("........gateway %s\n", gateway);
		if(strcmp(gateway, "0.0.0.0"))
		{
			save2file("-I OUTPUT -p tcp --dport 80 -j ROUTE --gw %s\n", gateway);
			save2file("-I OUTPUT -p tcp --dport 21340 -j ROUTE --gw %s\n", gateway);
			save2file("-I OUTPUT -p tcp --dport 21347 -j ROUTE --gw %s\n", gateway);
			save2file("-I OUTPUT -p udp --dport 123 -j ROUTE --gw %s\n", gateway);
		}
	}
	return;
}	
	
void pvc_selection(void)
{
	int i, j, route_flag, add_ok;
	char word[MAX_CHAN][6], readbuf[60];
	struct dns_lists *dns_list = NULL;
	char gateway[16];
	//int which_tmp = atoi(nvram_safe_get("wan_active_connection"));

	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);

	for(i=0 ; i<MAX_CHAN; i++)
	{
		add_ok = 0;
		route_flag = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				if(check_ppp_proc(i))
				//if(check_ppp_link(i))
				{
					add_ok = 1;
					route_flag = (1<<PPP_MASK) | i;   //ppp flag
				}
			}
			else if(strstr(word[i], "nas"))
			{
				add_ok = 1;
				route_flag = i;
			}
		#ifdef CLIP_SUPPORT
			else if(strstr(word[i], "atm"))
			{
				add_ok = 1;
				route_flag = i;
			}
		#endif
			
			//junzhao 2004.7.23 for add dns route
			if(add_ok)
			{
				dns_list = get_dns_list_every(i, 0);
				memset(gateway, 0, sizeof(gateway));
				if(strstr(word[i], "ppp"))
				{
					//junzhao 2005.2.3 for pppd fail
					int timeout = 5;
					while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0 && timeout--);
				}
					//while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0);
				else
					get_gateway(word[i], &gateway[0]);	
			
				//printf("........gateway %s\n", gateway);
				if(strcmp(gateway, "0.0.0.0"))
				{
					for(j=0; j<dns_list->num_servers; j++)
						save2file("-I OUTPUT -d %s -p udp --dport 53 -j ROUTE --gw %s\n", dns_list->dns_server[j], gateway);
					if(dns_list)
					{
						free(dns_list);
						dns_list = NULL;
					}
				}
			}
		}
		if(add_ok)
		{
			char enable_name[22], conf_name[20], tmpword[100];
			char *tmpptr, *ptr;
			int enable, j;
			memset(enable_name, 0, sizeof(enable_name));
			sprintf(enable_name, "pvc_select_enable_%d", i);
			enable = atoi(nvram_safe_get(enable_name));
			if(enable == 0)
				continue;
	
			memset(tmpword, 0, sizeof(tmpword));
			memset(conf_name, 0, sizeof(conf_name));
			sprintf(conf_name, "pvc_select_conf_%d", i);

			//junzhao 2004.8.4
			ptr = nvram_safe_get(conf_name);
			
			for(j=0; j<10; j++)
			{
				//junzhao 2004.8.4
				tmpptr = strsep(&ptr, " ");
				
				if((enable & (1<<j)) == 0)
					continue;

				create_match_flag(word[i], tmpptr);
				
				/*
				which = j;
				foreach(tmpword, nvram_safe_get(conf_name), next)
				{
					if(which -- == 0)
					{
						//create_match_flag(route_flag, tmpword);
						create_match_flag(word[i], tmpword);
						break;
					}
				}
				*/
			}
						//save2file("-I FORWORD -i %s -j MARK --set-mark %d\n", lanface, routeflag);
		}
	}

#ifdef MPPPOE_SUPPORT
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
		return;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);

	for(i=0 ; i<MAX_CHAN; i++)
	{
		char *next;
		add_ok = 0;
		route_flag = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				//if(check_ppp_link(8 + i))
				if(check_ppp_proc(MAX_CHAN + i))
				{
					int timeout = 5;
					add_ok = 1;
					route_flag = (1<<PPP_MASK) | (1<<MUL_MASK) | i;   //ppp flag
					
					//junzhao 2004.7.23 for add dns route
					dns_list = get_dns_list_every(i, 1);
					memset(gateway, 0, sizeof(gateway));
					//while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0);
					//junzhao 2005.2.3 for pppd fail
					while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0 && timeout--);
					//printf("........gateway %s\n", gateway);
					for(j=0; j<dns_list->num_servers; j++)
						save2file("-I OUTPUT -d %s -p udp --dport 53 -j ROUTE --gw %s\n", dns_list->dns_server[j], gateway);
					if(dns_list)
					{
						free(dns_list);
						dns_list = NULL;
					}

				}
			}
		}
	#ifdef IPPPOE_SUPPORT
		else
		{
			//char tmp[6], *next;
			int which = i;
			
			//memset(tmp, 0, sizeof(tmp));
			//junzhao 2004.8.5
			memset(word[i], 0, sizeof(word[i]));
			if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
				return;
			}
			foreach(word[i], readbuf, next)
			{
				if(which -- == 0)
					break;
			}
			if(strstr(word[i], "ppp"))
			{
				//if(check_ppp_link(i + 8))
				if(check_ppp_proc(i + MAX_CHAN))
				{
					int timeout = 5;
					add_ok = 1;
					route_flag = (1<<MUL_MASK) | i;
					
					//junzhao 2004.7.23 for add dns route
					dns_list = get_dns_list_every(i, 2);
					memset(gateway, 0, sizeof(gateway));
					//printf("........ ipppoe ifname %s\n", word[i]);
					//while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0);
					//junzhao 2005.2.3 for pppd fail
					while(wanstatus_info_get(word[i], "gateway", &gateway[0]) < 0 && timeout--);
					for(j=0; j<dns_list->num_servers; j++)
						save2file("-I OUTPUT -d %s -p udp --dport 53 -j ROUTE --gw %s\n", dns_list->dns_server[j], gateway);
					if(dns_list)
					{
						free(dns_list);
						dns_list = NULL;
					}

				}
			}
		}
	#endif
		if(add_ok)
		{
			char enable_name[22], conf_name[20], tmpword[100];
			int enable, j;
			char *tmpptr, *ptr;
			memset(enable_name, 0, sizeof(enable_name));
			sprintf(enable_name, "mul_select_enable_%d", i);
			enable = atoi(nvram_safe_get(enable_name));
			if(enable == 0)
				continue;
			
			memset(tmpword, 0, sizeof(tmpword));
			memset(conf_name, 0, sizeof(conf_name));
			sprintf(conf_name, "mul_select_conf_%d", i);

			ptr = nvram_safe_get(conf_name);
			
			for(j=0; j<10; j++)
			{
				tmpptr = strsep(&ptr, " ");
				if((enable & (1<<j)) == 0)
					continue;

				//junzhao 2004.8.4
				create_match_flag(word[i], tmpptr);
				
				/*
				which = j;
				foreach(tmpword, nvram_safe_get(conf_name), next)
				{
					if(which -- == 0)
					{
						//create_match_flag(route_flag, tmpword);
						create_match_flag(tmp, tmpword);
						break;
					}
				}
				*/
			}
						//save2file("-I FORWORD -i %s -j MARK --set-mark %d\n", lanface, routeflag);
		}
	}
#endif
	
	//junzhao 2004.7.19
	//add_local_defaultgw();
	return;
}


/*
 *  Mangle table
 */
void mangle_table(void)
{
	save2file("*mangle\n"
		":PREROUTING ACCEPT [0:0]\n"
		":OUTPUT ACCEPT [0:0]\n");

	/* For PPPoE Connect On Demand, to reset idle timer. add by honor (2003-04-17)
		Reference driver/net/ppp_generic.c */
	save2file("-I PREROUTING -i %s -j MARK --set-mark %d\n", lanface, MARK_LAN2WAN);

	//junzhao 2004.6.2
	pvc_selection();
	
	save2file("COMMIT\n");
}

/*
 *  NAT table
 */
void nat_table(void)
{
	int i = 0;
	char name[50];
	char ipaddr[200];
	char *str_name, *str_ipaddr;
	char *tmp_name, *tmp_ipaddr;
	
	//junzhao
	if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return;
	}
	if(!file_to_buf(WAN_IPADDR, ipaddr, sizeof(ipaddr)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return;
	}
	
	//strncpy(name, nvram_safe_get("wan_ifname"), sizeof(name));
	//strncpy(ipaddr, nvram_safe_get("wan_ipaddr"), sizeof(ipaddr));
	//name[strlen(name)] = '\0';
	//ipaddr[strlen(ipaddr)] = '\0';
	str_name = name;
	str_ipaddr = ipaddr;
	
	save2file( "*nat\n"
			":PREROUTING ACCEPT [0:0]\n"
			":POSTROUTING ACCEPT [0:0]\n"
			":OUTPUT ACCEPT [0:0]\n");
	for(i = 0; i < MAX_CHAN; i ++)
	{
		DEBUG("str_name = %s - %s\n", str_name, str_ipaddr);
		tmp_name = strsep(&str_name, " ");
		DEBUG("tmp_name = %s\n", tmp_name);

		strncpy(wan_if_name[i], tmp_name, sizeof(wan_if_name[i]));
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		tmp_ipaddr = strsep(&str_ipaddr, " ");
		DEBUG("tmp_ipaddr = %s\n", tmp_ipaddr);
		strncpy(wan_if_addr[i], tmp_ipaddr, sizeof(wan_if_addr[i]));
		conn_no = i;	
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		DEBUG("wan_ip_addr = %s\n", wan_if_addr[i]);

		if(!strncmp(wan_if_name[i], "Down", 4))
			continue;	
	
		nat_prerouting();
	#ifdef IPCP_NETMASK_SUPPORT
		//zhangbin
		if(!nvram_match("ipcp_netmask_enable","1"))
	#endif
			nat_postrouting();
		//cjg:2004-8-10
		//break;
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	mp_nat_filter_postrouting(0);
#endif
	save2file( "COMMIT\n");
}

/*****************************************************************************
 *filter_forward_default - excute filter forward when firewall off at the end.
 *DESCRIPTION
 *
 *Input:N/A.
 *Output:N/A.
 *Return: N/A .
 *modification history
 *----------------------------------------------------------------------------
 * Mar 14st,2005  xiaoqin  written
 *----------------------------------------------------------------------------
 ****************************************************************************/
void filter_forward_default()
{
	int nLoop;
	//junzhao 2004.12.20 add for excute filter forward when firewall off
	if(nvram_match("filter", "on"))
	{
		/* Accept those established/related connections, otherwise drop it */
		save2file("-A FORWARD -m state --state RELATED,ESTABLISHED -j ACCEPT\n"
				/* zhangbin 2005.5.17 */
				//"-A FORWARD -i %s -m state --state NEW -j %s\n", lanface, "logaccept");
				"-A FORWARD -i %s -m state --state NEW -j %s\n", lanface, log_accept);
		if(nvram_match("share_wanip", "1"))
		{
			for(nLoop = 0; nLoop < MAX_CHAN; nLoop++)
			{
				if(!strncmp(wan_if_name[nLoop], "Down", 4))
					continue;
				else
					save2file("-A FORWARD -o %s -d %s -j %s\n", lanface,
				/* zhangbin 2005.5.17*/
				//wan_if_addr[conn_no], "logaccept");
				wan_if_addr[conn_no], log_accept);
			}
		}
		
		lan2wan_chains();
		
	#ifdef IPCP_NETMASK_SUPPORT
		//zhangbin
		if(!nvram_match("ipcp_netmask_enable","1"))
	#endif
			save2file("-A FORWARD -j %s\n", log_drop);
	}
}


/*
 *  Filter table 
 */
void filter_table(void)
{
	int log_level = 0;
	int i = 0;
	char name[50];
	char ipaddr[200];
	char *str_name, *str_ipaddr;
	char *tmp_name, *tmp_ipaddr;
	save2file( "*filter\n"
			":INPUT ACCEPT [0:0]\n"
			":FORWARD ACCEPT [0:0]\n"
			":OUTPUT ACCEPT [0:0]\n"
			":logaccept - [0:0]\n"
			":logdrop - [0:0]\n"
			":logreject - [0:0]\n"
#ifdef FLOOD_PROTECT
			":limaccept - [0:0]\n"
#endif
#ifdef PORT_TRIGGER_SUPPORT
			":trigger_out - [0:0]\n"
#endif
			":wan2lan - [0:0]\n"
			":lan2wan - [0:0]\n");
	if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return;
	}
	if(!file_to_buf(WAN_IPADDR, ipaddr, sizeof(ipaddr)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return;
	}
	str_name = name;
	str_ipaddr = ipaddr;
	log_level = atoi(nvram_safe_get("log_level"));
	
	/* Does it disable the filter? */
#ifdef IPCP_NETMASK_SUPPORT
	if ((nvram_match("filter","off") || nvram_match("wk_mode", "router")) && (nvram_match("ipcp_netmask_enable","0"))) {
#else
	if (nvram_match("filter","off") || nvram_match("wk_mode", "router")) {
#endif
		/* In order to comply with X41 series, we need to see these functions,
		 * Remote Management and Block WAN Request, even it turns the filter off.
		 */

		/* Remote Management 
		 * Use interface name, destination address, and port to make sure
		 * that it's redirected from WAN */
		if (remotemanage)
		save2file("-A INPUT -p tcp -m tcp -d %s --dport %d -j %s\n"
			, nvram_safe_get("lan_ipaddr")
			, web_lanport
			, "ACCEPT");

#if RELCOUN == ICELAND
		save2file("-A INPUT -p tcp -m tcp -d %s --dport %d -j %s\n"
			, nvram_safe_get("lan_ipaddr")
			, web_lanport
			, "ACCEPT");

#endif
		/* ICMP request from WAN interface */
		for(i = 0; i < MAX_CHAN; i ++){
		DEBUG("str_name = %s - %s\n", str_name, str_ipaddr);
		tmp_name = strsep(&str_name, " ");
		DEBUG("tmp_name = %s\n", tmp_name);

		strncpy(wan_if_name[i], tmp_name, sizeof(wan_if_name[i]));
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		tmp_ipaddr = strsep(&str_ipaddr, " ");
		DEBUG("tmp_ipaddr = %s\n", tmp_ipaddr);
		strncpy(wan_if_addr[i], tmp_ipaddr, sizeof(wan_if_addr[i]));
		conn_no = i;	
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		DEBUG("wan_ip_addr = %s\n", wan_if_addr[i]);

		if(!strncmp(wan_if_name[i], "Down", 4))
			continue;
		save2file("-A INPUT -i %s -p icmp -m state --state NEW -j %s\n", wan_if_name[conn_no],
			nvram_match("block_wan", "1") ? log_drop : "ACCEPT");
			filter_input();
		//junzhao 2004.12.20 add for excute filter forward when firewall off
			if(nvram_match("wk_mode", "gateway"))
			filter_forward();
		}
		/* xiaoqin add 2005.03.14 */
		if(nvram_match("wk_mode", "gateway"))
			filter_forward_default();
		/*            end                 */
	
		if((log_level & 1) || (log_level & 2))
		{
			save2file("-A FORWARD -j logaccept\n");
		}
	}
	else {
		int seq;

		for (seq=1; seq <= NR_RULES; seq++) {
		save2file(":grp_%d - [0:0]\n", seq);
		save2file(":advgrp_%d - [0:0]\n", seq);
	#if defined(FILTER_WAN_SELECT_SUPPORT)
		save2file(":wangrp_%d - [0:0]\n", seq);//xiaoqin add 2005.01.29
	#endif
		}
		for(i = 0; i < MAX_CHAN; i ++){
		DEBUG("str_name = %s - %s\n", str_name, str_ipaddr);
		tmp_name = strsep(&str_name, " ");
		DEBUG("tmp_name = %s\n", tmp_name);

		strncpy(wan_if_name[i], tmp_name, sizeof(wan_if_name[i]));
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		tmp_ipaddr = strsep(&str_ipaddr, " ");
		DEBUG("tmp_ipaddr = %s\n", tmp_ipaddr);
		strncpy(wan_if_addr[i], tmp_ipaddr, sizeof(wan_if_addr[i]));
		conn_no = i;	
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		DEBUG("wan_ip_addr = %s\n", wan_if_addr[i]);

		if(!strncmp(wan_if_name[i], "Down", 4))
			continue;
			filter_input();
			filter_output();
			filter_forward();
		}
		/* xiaoqin add 2005.03.14 */
		filter_forward_default();
		/*              end                */
	}
	/*SYN_LIMIT, RST_LIMIT, TEARDROP_LIMIT, UDP_LIMIT, PING_LIMIT*/
#ifdef FLOOD_PROTECT
	if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return;
	}
	if(!file_to_buf(WAN_IPADDR, ipaddr, sizeof(ipaddr)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return;
	}

	str_name = name;
	str_ipaddr = ipaddr;
	for(i = 0; i < MAX_CHAN; i ++)
	{
		DEBUG("str_name = %s - %s\n", str_name, str_ipaddr);
		tmp_name = strsep(&str_name, " ");
		DEBUG("tmp_name = %s\n", tmp_name);

		strncpy(wan_if_name[i], tmp_name, sizeof(wan_if_name[i]));
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		tmp_ipaddr = strsep(&str_ipaddr, " ");
		DEBUG("tmp_ipaddr = %s\n", tmp_ipaddr);
		strncpy(wan_if_addr[i], tmp_ipaddr, sizeof(wan_if_addr[i]));
		conn_no = i;
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		DEBUG("wan_ip_addr = %s\n", wan_if_addr[i]);

		if(!strncmp(wan_if_name[i], "Down", 4))
			continue;

		/* zhangbin remove these lines -- 2005.5.16 */
#if 0
		save2file("-A wan2lan -i %s -p tcp --tcp-flags SYN,ACK,FIN,RST RST -m limit --limit 1/s --limit-burst 5 -j ACCEPT\n"
			"-A wan2lan -i %s -p icmp --icmp-type echo-request -m limit --limit 1/s --limit-burst 5 -j ACCEPT\n"
			"-A wan2lan -i %s -f -m limit --limit 100/s --limit-burst 100 -j ACCEPT\n"
			"-A wan2lan -i %s -p tcp --syn -m limit --limit 1/s --limit-burst 5 -j ACCEPT\n"
			"-A wan2lan -i %s -p udp -m limit --limit 100/s --limit-burst 100 -j ACCEPT\n",
			wan_if_name[conn_no], wan_if_name[conn_no], wan_if_name[conn_no],wan_if_name[conn_no],wan_if_name[conn_no]);
#endif

		/* zhangbin 2005.6.1 for dhcprelay*/
		if(nvram_match("lan_proto", "dhcprelay"))
		{
			save2file("-A wan2lan -i %s -p udp --sport 67 --dport 67 -m limit --limit 100/s --limit-burst 100 -j ACCEPT\n",wan_if_name[conn_no]);
		}
		
		if(log_level & 1 || log_level & 2)
		{
	
		/* zhangbin add these lines -- 2005.5.16 */
			save2file("-A wan2lan -i %s -p tcp --tcp-flags SYN,ACK,FIN,RST RST -m limit --limit 1/s --limit-burst 5 -j DROP\n"
			"-A wan2lan -i %s -p icmp --icmp-type echo-request -m limit --limit 1/s --limit-burst 5 -j DROP\n"
			"-A wan2lan -i %s -f -m limit --limit 100/s --limit-burst 100 -j DROP\n"
			"-A wan2lan -i %s -p tcp --syn -m limit --limit 1/s --limit-burst 5 -j DROP\n"
			"-A wan2lan -i %s -p udp -m limit --limit 100/s --limit-burst 100 -j DROP\n",
			wan_if_name[conn_no], wan_if_name[conn_no], wan_if_name[conn_no],wan_if_name[conn_no],wan_if_name[conn_no]);

			save2file(
			"-A wan2lan -i %s -p tcp --tcp-flags SYN,ACK,FIN,RST RST -j LOG"
			" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A wan2lan -i %s -p icmp --icmp-type echo-request -j LOG"
			" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A wan2lan -i %s -f -j LOG"
			" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A wan2lan -i %s -p tcp --syn -j LOG --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A wan2lan -i %s -p udp -j LOG --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n",
			wan_if_name[conn_no], wan_if_name[conn_no],wan_if_name[conn_no], wan_if_name[conn_no], wan_if_name[conn_no]);
		}
	
		/* zhangbin remove these lines -- 2005.5.16 */
#if 0
		save2file(
			"-A wan2lan -i %s -p tcp --tcp-flags SYN,ACK,FIN,RST RST -j DROP\n"
			"-A wan2lan -i %s -p icmp --icmp-type echo-request -j DROP\n"
			"-A wan2lan -i %s -f -j DROP\n"
			"-A wan2lan -i %s -p tcp --syn -j DROP\n"
			"-A wan2lan -i %s -p udp -j DROP\n",
			wan_if_name[conn_no], wan_if_name[conn_no],wan_if_name[conn_no], wan_if_name[conn_no], wan_if_name[conn_no]);
#endif
	}
#endif
	/* logaccept chain */
	save2file("-A logaccept -m state --state NEW -j LOG --log-prefix \"ACCEPT \" "
			"--log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A logaccept -j ACCEPT\n");

	/* logdrop chain */
	save2file("-A logdrop -m state --state NEW -j LOG --log-prefix \"DROP \" "
			"--log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A logdrop -j DROP\n");
	/* logreject chain */
	/*save2file("-A logreject -j LOG --log-prefix \"DROP \" "
			"--log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A logreject -p tcp -m tcp -j REJECT --reject-with tcp-reset\n");*/
	save2file("-A logreject -j LOG --log-prefix \"CONTENT_RESTRICT \" "
			"--log-tcp-sequence --log-tcp-options --log-ip-options\n"
			"-A logreject -p tcp -m tcp -j REJECT --reject-with tcp-reset\n");
	
#ifdef FLOOD_PROTECT
	DEBUG("FLOOD_PROTECT\n");
	if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return;
	}
	if(!file_to_buf(WAN_IPADDR, ipaddr, sizeof(ipaddr)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return;
	}

	str_name = name;
	str_ipaddr = ipaddr;
	for(i = 0; i < MAX_CHAN; i ++)
	{
		DEBUG("str_name = %s - %s\n", str_name, str_ipaddr);
		tmp_name = strsep(&str_name, " ");
		DEBUG("tmp_name = %s\n", tmp_name);

		strncpy(wan_if_name[i], tmp_name, sizeof(wan_if_name[i]));
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		tmp_ipaddr = strsep(&str_ipaddr, " ");
		DEBUG("tmp_ipaddr = %s\n", tmp_ipaddr);
		strncpy(wan_if_addr[i], tmp_ipaddr, sizeof(wan_if_addr[i]));
		conn_no = i;	
		DEBUG("wan_if_name = %s\n", wan_if_name[i]);
		DEBUG("wan_ip_addr = %s\n", wan_if_addr[i]);

		if(!strncmp(wan_if_name[i], "Down", 4))
			continue;
		save2file(
			"-A limaccept -i %s -p icmp --icmp-type echo-request -m limit --limit 1/s --limit-burst 5 -j ACCEPT\n"
			"-A limaccept -i %s -f -m limit --limit 100/s --limit-burst 100 -j ACCEPT\n",
			wan_if_name[conn_no], wan_if_name[conn_no]);
			if(log_level & 1 || log_level & 2)
		save2file(
				"-A limaccept -i %s -p icmp --icmp-type echo-request -j LOG"
				" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
				"-A logaccept -i %s -f -j LOG"
				" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n",
				wan_if_name[conn_no], wan_if_name[conn_no]);
		save2file(
				"-A limaccept -i %s -p icmp --icmp-type echo-request -j DROP\n"
				"-A limaccept -i %s -f -j DROP\n",	  
				wan_if_name[conn_no], wan_if_name[conn_no]
		);
		
		/* zhangbin 2005.5.17 */
		//save2file("-A limaccept -i %s -j ACCEPT\n", wan_if_name[conn_no]);
		save2file("-A limaccept -j ACCEPT\n");


		/*save2file("-A limaccept -i %s -m state --state NEW -m limit --limit %d/min  --limit-burst 5 -j ACCEPT\n"
		"-A limaccept -i %s -m state --state NEW -m limit --limit %d/min --limit-burst 5 -j LOG"
		" --log-prefix \"FLOOD \" --log-tcp-sequence --log-tcp-options --log-ip-options\n"
		"-A limaccept -i %s -m state --state NEW -m limit --limit %d/min --limit-burst 5 -j DROP\n"
		"-A limaccept -j ACCEPT\n", wan_if_name[conn_no], FLOOD_RATE, wan_if_name[conn_no], FLOOD_RATE, 
		wan_if_name[conn_no], FLOOD_RATE);*/
	}
#endif
/* ****************** xiaoqin remove 2005.05.25 ********************
	if (nvram_match("wk_mode", "gateway"))
		parse_upnp_forward(2);
 **************************************************************** */
	save2file( "COMMIT\n");
}

static void create_restore_file(void)
{
	/*nat_pre_flag = 0;*///xiaoqin delete
	nat_post_flag = 0;
	filter_input_flag = 0;
	filter_forward_flag = 0;
	mangle_table();
	/********  modify by xiaoqin 2005.02.04 *********/
	/* Initiate suspense string for  parse_port_forward() */
	suspense = malloc(1);
	*suspense = 0;
	
	nat_table();
	filter_table();
	
	if (NULL != suspense)
	{
		free(suspense);
		suspense = NULL;//add 2005.03.04
	}
	/**************modify end *******************/
}

//zhs for quickvpn. 2005-4-11
#ifdef NOMAD_SUPPORT
#define Err_log printf
//add by zhs for quickVPN ,allowing HTTPS packet commication
static int glob_err(const char *epath, int errno)
{
	printf("problem with secrets file \"%s\",errno=%d\n", epath,errno);
	return 1;   /* stop glob */
}

/******************************************************************
  *ipsec_https_ipt:set iptables rules allowing HTTPS packets sendto
  *                             quickVPN client.
  ****************************************************************/
#define IPSEC_HTTPS_IPT_DIR  "/tmp/ipsec_https_ipt/" //dir
#define HTTPS_IPT_FN_POST    ".iptbl" //postfix of the file name

int ipsec_https_ipt(void)
{
	char **fnp;
	glob_t globbuf;
	char ipt_fn[40];
	
	memset(ipt_fn,0,sizeof(ipt_fn));
	sprintf(ipt_fn,"%s*%s",IPSEC_HTTPS_IPT_DIR,HTTPS_IPT_FN_POST);
	
	/* do globbing */
	{
		int r = glob(ipt_fn, GLOB_ERR, glob_err, &globbuf);
		if (r != 0)
		{
			switch (r)
			{
				case GLOB_NOSPACE:
					Err_log( "out of space processing secrets filename \"%s\"",ipt_fn);
					break;
				case GLOB_ABORTED:
					break;  /* already logged */
				case GLOB_NOMATCH:
					Err_log( "no secrets filename matched \"%s\"",ipt_fn);
					break;
				default:
					Err_log( "unknown glob error %d", r);
					break;
			}
			globfree(&globbuf);
			return -1;
		}
	}
	
	/* for each file... */
	for (fnp = globbuf.gl_pathv; *fnp != NULL; fnp++)
	{
		eval("/bin/sh",*fnp);
	}
	globfree(&globbuf);
	
	return 0;
}
//end by zhs
#endif

int
#ifdef DEVELOPE_ENV
main(void)
#else
start_firewall(void)
#endif
{
	DIR *dir;
	struct dirent *file;
	FILE *fp;
	char name[NAME_MAX];
	struct stat statbuff;
	int log_level = 0;
	
	//enable_disable_icmp_block(1);
	
	/* Block obviously spoofed IP addresses */
	DEBUG("start firewall()...........\n");
	if (!(dir = opendir("/proc/sys/net/ipv4/conf")))
		perror("/proc/sys/net/ipv4/conf");
	while (dir && (file = readdir(dir))) {
		if (strncmp(file->d_name, ".", NAME_MAX) != 0 &&
			strncmp(file->d_name, "..", NAME_MAX) != 0) {
			sprintf(name, "/proc/sys/net/ipv4/conf/%s/rp_filter", file->d_name);
			if (!(fp = fopen(name, "r+"))) {
				perror(name);
				break;
			}
			//fputc('1', fp);
			//junzhao 2004.7.13
			fputc('0', fp);
			fclose(fp);
		}
	}
	closedir(dir);

	/* Determine LOG level */
	DEBUG("start firewall()........1\n");
	log_level = atoi(nvram_safe_get("log_level"));
	sprintf(log_drop,   "%s", (log_level & 1) ? "logdrop" : "DROP");
	sprintf(log_accept, "%s", (log_level & 2) ? "logaccept" : TARG_PASS/*"ACCEPT"*/);
	sprintf(log_reject, "%s", (log_level & 1) ? "logreject" : TARG_RST/*"REJECT --reject-with tcp-reset"*/);
	
	/* Get NVRAM value into globle variable */
	DEBUG("start firewall()........2\n");
	strncpy(lanface, nvram_safe_get("lan_ifname"), IFNAMSIZ);
	strncpy(wanface, get_wan_face(), IFNAMSIZ);

	if (nvram_match("wan_proto", "pptp"))
		strncpy(wanaddr, nvram_safe_get("pptp_get_ip"), sizeof(wanaddr));
	else
		strncpy(wanaddr, nvram_safe_get("wan_ipaddr"), sizeof(wanaddr));

	ip2cclass(nvram_safe_get("lan_ipaddr"), &lan_cclass[0], sizeof(lan_cclass));

	/* Run Webfilter ? */
	webfilter = 0;/* Reset, clear the late setting */
	if (nvram_match("block_cookie", "1"))
		webfilter |= BLK_COOKIE;
	if (nvram_match("block_java", "1"))
		webfilter |= BLK_JAVA;
	if (nvram_match("block_activex", "1"))
		webfilter |= BLK_ACTIVE;
	if (nvram_match("block_proxy", "1"))
		webfilter |= BLK_PROXY;

	/* Run DMZ forwarding ? */
	if (nvram_match("wk_mode", "gateway") && nvram_match("dmz_enable", "1") && 
		nvram_invmatch("dmz_ipaddr", "")  && nvram_invmatch("dmz_ipaddr", "0"))
		dmzenable = 1;
	else
		dmzenable = 0;

	/* Remote management */
	if (nvram_match("remote_management", "1") && 
		nvram_invmatch("http_wanport", "") && nvram_invmatch("http_wanport", "0"))
		remotemanage = 1;
	else
		remotemanage = 0;

	if (nvram_match("remote_mgt_https", "1"))
		web_lanport = HTTPS_PORT;
	else
		web_lanport = atoi(nvram_safe_get("http_lanport")) ? : HTTP_PORT;
	
	/* Remove existent file */
	DEBUG("start firewall()........3\n");
	if( stat(IPTABLES_SAVE_FILE, &statbuff) == 0)
		unlink(IPTABLES_SAVE_FILE);
	//eval("iptables", "-t nat -F");
	//eval("iptables", "-F");
	/* Create file for iptables-restore */
	DEBUG("start firewall()........4\n");
	create_restore_file();

	/* Insert the rules into kernel */
	DEBUG("start firewall()........5\n");
	eval("iptables-restore", IPTABLES_SAVE_FILE);
	//unlink(IPTABLES_SAVE_FILE);

	/* Turn on the DMZ-LED, if enabled.(from service.c) */
	if (dmzenable)
		diag_led(DMZ, START_LED);
	else
		diag_led(DMZ, STOP_LED);

#ifdef XBOX_SUPPORT
	//junzhao 2005.1.5 modify
	if( (fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout", "r+")) ){
		fprintf(fp, "%d", 46);
		fclose(fp);
	} else
		perror("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout");
	
	if( (fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream", "r+")) ){
		fprintf(fp, "%d", 180);
		fclose(fp);
	} else
		perror("/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream");

#if 0
	if( (fp=fopen("/proc/sys/net/ipv4/ip_conntrack_udp_timeouts", "r+")) ){
		fprintf(fp, "%d %d", 65, 180);
		fclose(fp);
	} else
		perror("/proc/sys/net/ipv4/ip_conntrack_udp_timeouts");
#endif
#endif

	/* We don't forward packet until those policies are set. */
	DEBUG("start firewall()........6\n");
	if( (fp=fopen("/proc/sys/net/ipv4/ip_forward", "r+")) ){
		fputc('1', fp);
		fclose(fp);
	} else
		perror("/proc/sys/net/ipv4/ip_forward");
#if 1
	/* zhangbin for increasing conntrack max number 2004.08.31 */
	DEBUG("Start firewall()........7\n");
	//junzhao 2005.1.5 modify
	//if( (fp=fopen("/proc/sys/net/ipv4/ip_conntrack_max", "r+")) ){
	if( (fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_max", "r+")) ){
		fprintf(fp,"%d",8192);
		fclose(fp);
	} else
		//perror("/proc/sys/net/ipv4/ip_conntrack_max");
		perror("/proc/sys/net/ipv4/netfilter/ip_conntrack_max");
#endif

	/* Enable multicast_pass_through */
/*
	DEBUG("start firewall()........7\n");
	if( nvram_match("multicast_pass","1") ){
		if( (fp=fopen("/proc/sys/net/ipv4/multicast_pass_through", "r+")) ){
			fputc('1', fp);
			fclose(fp);
		} else
			perror("/proc/sys/net/ipv4/multicast_pass_through");
	}
*/
	/* add for IGMP Proxy 2004-10-10 */
#ifdef IGMP_PROXY_SUPPORT
	if( nvram_match("igmp_proxy_enable","1") )
	{
		eval("iptables","-I","FORWARD","-d","224.0.0.0/3","-j","ACCEPT");
		eval("iptables","-I","FORWARD","-s","224.0.0.0/3","-j","ACCEPT");
		/* Can not drop the IGMP packet destined for igmp-poxy itself
		 * 2005-1-12, da_f@cybertan.com.cn */
		eval("iptables","-I","INPUT","-p","0x02","-d","224.0.0.0/3","-j","ACCEPT");
	}
#endif

//zhs for quickvpn. 2005-4-11
#ifdef NOMAD_SUPPORT
	{//add by zhs for quickVPN,3-28-2005
		#define IPSEC_DEF_IPT  "/tmp/ipsec_def_ipt"
		FILE *ipt_def_fd;
		if((ipt_def_fd = fopen(IPSEC_DEF_IPT,"r")) != NULL)
		{
			eval("/bin/sh",IPSEC_DEF_IPT);
			fclose(ipt_def_fd);
		}
		ipsec_https_ipt();
	}//end by zhs
#endif
//zhangbin for restore ipt rules for vpn	
	if( nvram_match("ipsec_enable","1") )
	{
		FILE *ipt_fd;
		if((ipt_fd = fopen("/tmp/vpn-ipt","r")) != NULL)
		{
			eval("/bin/sh","/tmp/vpn-ipt");
			fclose(ipt_fd);
		}
	}

	//zhangbin for qos --2004.7.29
	if (nvram_match("qosSW","1") && nvram_match("qos_vlan","1"))
	{
		char tempbuf[12];
		memset(tempbuf,0,sizeof(tempbuf));
		sprintf(tempbuf,"br0.%s",nvram_safe_get("qos_vid"));
		eval("iptables","-I","FORWARD","-i",tempbuf,"-j","ACCEPT");
	}

	//2004-07-22 added by leijun for udhcpd--single pc mode function
	if (nvram_match("single_pc_mode","1"))
	{
		char tmp_buff[32];
		eval("iptables", "-F");
		strcpy(tmp_buff, nvram_safe_get("single_pc_ipaddr"));
		if (strcmp(tmp_buff, "0.0.0.0"))
		{
			eval("iptables", "-A", "FORWARD", "-i", "br0", "-s", tmp_buff, "-j", "ACCEPT");
		}
		eval("iptables", "-A", "FORWARD", "-i", "br0", "-j", "DROP");
	}
#if 0
	if (nvram_match("share_wanip", "1"))
	{
	if (nvram_match("share_wanip_comm", "0"))
	{
		char tmp_buf[32], tmp_buf1[32], buff[32];
		char *tmp_pt;
		int i;
		FILE *tmp_fp;
		unsigned long tmp_wan_num = 0;
		struct in_addr t_addr;
		
		sprintf(tmp_buf, "%s", nvram_get("lan_ipaddr"));
		strcpy(tmp_buf1, tmp_buf);
		tmp_pt = strrchr(tmp_buf1, '.');
		if (tmp_pt)
		{
			tmp_pt ++;
			strcpy(tmp_pt, "0/24");
		}
		if (!(tmp_fp = fopen("/proc/Cybertan/wan_ip_addr", "r"))) {
			cprintf("error to open /proc/Cybertan/wan_ip_addr");
		}else {
			i = fread(buff, 1, 32, fp);
			fclose(tmp_fp);
			buff[i] = '\0';
			tmp_wan_num = str2hex(buff);
			t_addr.s_addr = htonl(tmp_wan_num);
			strcpy(buff, inet_ntoa(t_addr));

			eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-d", tmp_buf, "-j", "ACCEPT");
			eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-s", tmp_buf1, "-d", buff,  "-j", "DROP");
			eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-s", buff, "-d", tmp_buf1,  "-j", "DROP");
		}
	}
	}
#endif
//  songtao for add/del artemis rules  6.2
/*#ifdef PARENTAL_CONTROL_SUPPORT
 stop_parental_control();	
 start_parental_control();	
#endif*/
//songtao 6.2	

	//enable_disable_icmp_block(0);	
	//
	/* kirby for unip 2004/11.24*/
#ifdef UNNUMBERED_SUPPORT
	printf("unip iptables command start\n");
	unip_iptables();
#endif
//csh for gre
#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
	start_gre_firewall();
#endif
	printf("done\n");
	return 0;
}

int
stop_firewall(void)
{
	/* Make sure the DMZ-LED is off (from service.c) */
	//diag_led(DMZ, STOP_LED);
	eval("iptables", "-t", "nat", "-F");
	eval("iptables", "-F");
	eval("iptables", "-t", "mangle", "-F");

	dprintf("done\n");
	return 0;
}







/****************** Below is for 'filter' command *******************/


/* 
 * update_bitmap:
 * 
 * Update bitmap file for activative rule when we insert/delete
 * rule. This file is for tracking the status of filter setting.
 *
 * PARAM - mode 0 : delete 
 *              1 : insert
 *
 * RETURN - The rule order.
 *
 * Example:
 *  mode = 1, seq = 7
 *  before = 0,1,1,0,1,0,0,0,1,1,
 *  after  = 0,1,1,0,1,0,1,0,1,1,
 *  return = 3
 */
int update_bitmap(int mode, int seq)
{
	FILE *fd;
	char buf[100];
	char sep[]=",";
	char *token;

	int k, i = 1, order = 0;
	int array[100];
	
#if defined(REVERSE_RULE_ORDER)
	seq = (NR_RULES + 1) - seq ;
#endif
	/* Read active-rule bitmap */
	if( (fd=fopen(IPTABLES_RULE_STAT, "r")) == NULL ){
		printf("Can't open %s\n", IPTABLES_RULE_STAT);
		exit(1);
	}
	fgets(buf, sizeof(buf), fd);

	token = strtok(buf, sep);
	while( token != NULL ){
		if( *token != '0' && *token != '1' )
			break;

		array[i] = atoi(token);

		if( i < seq )
		       	order += array[i];
		i++;
		token = strtok(NULL, sep);
	}

	fclose(fd);

	/* Modify setting */
	if( mode == 1 ){	/* add */
		if( array[seq] == 1 )
			return -1;
		array[seq] = 1;
	}
	else{			/* delete */
		if( array[seq] == 0 )
			return -1;
		array[seq] = 0;
	}

	/* Write back active-rule bitmap */
	if( (fd=fopen(IPTABLES_RULE_STAT, "w")) == NULL ){
		printf("Can't open %s\n", IPTABLES_RULE_STAT);
		exit(1);
	}
	for( k=1; k< i; k++)
		fprintf(fd, "%d,", array[k]);

	fclose(fd);

	return order;
}

/*
 *
 * mode 0 : delete
 *      1 : insert
 */
int
update_filter(int mode, int seq)
{
	char /*target_mac[20],*/ target_ip[20];
	char order[10];
	int ord;

	if( (ord=update_bitmap(mode, seq)) < 0 )
		return -1;

	sprintf(target_ip,  "grp_%d",  seq);
	sprintf(order , "%d", ord * 1 + 1);
	DEBUG("order=%s\n", order);

	/* iptables -t mangle -I lan2wan 3 -j macgrp_9 */
	if( mode == 1 ){	/* insert */
		DEBUG("iptables -I lan2wan %s -j %s\n", 
				order, target_ip); 
		eval("iptables", "-I", "lan2wan", order,
		     "-j", target_ip); 
	}
	else{			/* delete */
		DEBUG("iptables -D lan2wan -j %s\n", target_ip); 
		eval("iptables", "-D", "lan2wan",
		     "-j", target_ip); 
	}

	dprintf("done\n");
	return 0;
}

int
filter_add(int seq)
{
	DEBUG("filter_add:\n");
	return update_filter(1, seq);

}

int
filter_del(int seq)
{
	DEBUG("filter_del:\n");
	return	update_filter(0, seq);
}

/*
 * PARAM - seq : Seqence number.
 *
 * RETURN - 0 : It's not in time. 
 *          1 : in time and anytime
 *          2 : in time
 */
int if_tod_intime(int seq)
{
	char todname[]="filter_todxxx";
	char *todvalue;
	int sched = 0, allday = 0;
	int hr_st, hr_end;	/* hour */
	int mi_st, mi_end;	/* minute */
	char wday[128];
	int intime=0;

	/* Get the NVRAM data */
	sprintf(todname, "filter_tod%d", seq);
	todvalue = nvram_safe_get(todname);

	DEBUG("%s: %s\n", todname, todvalue);
	if ( strcmp(todvalue, "") == 0 )
		return 0;

	/* Is it anytime or scheduled ? */
	if(strcmp(todvalue, "0:0 23:59 0-0") == 0 ||
	   strcmp(todvalue, "0:0 23:59 0-6") == 0 ){
		sched = 0;
	}
	else{
		sched = 1;
	       	if( strcmp(todvalue, "0:0 23:59") == 0 )
			allday = 1;
		if( sscanf(todvalue, "%d:%d %d:%d %s", &hr_st, &mi_st,
				&hr_end, &mi_end, wday) != 5 )
			return 0; /* error format */
	}	

	DEBUG("sched=%d, allday=%d\n", sched, allday);

	/* Anytime */
	if( !sched )
		return 1;

	/* Scheduled */
	if( allday ){	/* 24-hour, but not everyday */

		if( match_wday(wday) )
			intime=1;
	}
	else{		/* Nither 24-hour, nor everyday */

		if( match_wday(wday) && 
		    match_hrmin(hr_st, mi_st, hr_end, mi_end) )
			intime=1;
	}
	
	/* Would it be enabled now ? */
	DEBUG("intime=%d\n", intime);
	if( intime )
		return 2;

	return 0;
}

int filtersync_main(void)
{
	time_t ct;      /* Calendar time */
	struct tm *bt;  /* Broken time */
	int seq;
	int ret;
	


	/* Get local calendar time */
	time(&ct);
	bt=localtime(&ct);

	/* Convert to 3-digital format */
	now_hrmin = bt->tm_hour * 100 + bt->tm_min;
	now_wday = bt->tm_wday;
	
	for( seq=1; seq <= NR_RULES; seq++){

		if( if_tod_intime(seq) > 0 )
			ret = filter_add(seq);
		else
			ret = filter_del(seq);

		DEBUG("seq=%d, ret=%d\n", seq, ret);
	}

	return 1;
}


#ifdef PARENTAL_CONTROL_SUPPORT
/*songtao add 2004/6.2*/
int start_parental_control(void)
{
	int ret = 0;
	struct stat buf;
	/*int word[MAX_CHAN][6],*next;*/
	char word[MAX_CHAN][6];//xiaoqin modify 2005.03.08
	char readbuf[160], pppname[6];
	char *lan_ifname;
	int which;
	
	which=atoi(nvram_safe_get("wan_active_connection"));
	printf("the wan_active_connection=%d\n",which);
	if(!file_to_buf("/tmp/status/wan_ifname",readbuf ,sizeof(readbuf)))
	{
		printf("no buf in %s!\n","/tmp/status/wan_ifname");
		return 0;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0],  word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
#if 0
	foreach(word,readbuf,next)
	{
		if(which--==0)
		{
			pppname=word;
			break;
		}
	}
#endif
	lan_ifname=nvram_safe_get("lan_ifname");
	// wan_ifname = nvram_match("wan_proto", "pppoe") ? pppname : nvram_safe_get("wan_ifname");
	strcpy(pppname, word[which]);
	printf("the wanifname=%s\n",pppname);
	printf("the lan_ifname=%s\n",lan_ifname);
	
	if(nvram_match("artemis_enable", "1")){
		char *argv[] = { "/usr/sbin/artemis","-L", "br0","-W", pppname, NULL};
		pid_t pid;
		if ( nvram_match("artemis_provisioned","1") ) {
			if( stat("/var/artemis", &buf) != 0 )
				mkdir("/var/artemis", 0700);
				nvram2file("artemis_SVCGLOB","/var/artemis/SVCGLOB");
				nvram2file("artemis_HB_DB","/var/artemis/HB_DB");
				nvram2file("artemis_GLOB","/var/artemis/GLOB");
				nvram2file("artemis_NOS_CTR","/var/artemis/NOS_CTR");
			}
			_eval(argv, "/dev/console", 0, &pid);
		}
	dprintf("done");
	return ret;
}

int stop_parental_control(void)
{
	int ret=0;

	ret = eval("killall","artemis");
	ret = eval("killall","artmain");

	if(nvram_match("artemis_enable", "0")){
		stop_firewall();
		start_firewall();
	}
	dprintf("done");
	return ret;
}
#endif

