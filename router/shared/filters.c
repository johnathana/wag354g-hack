
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <broadcom.h>

/* da_f 2005.3.23 add */
#ifdef NOMAD_SUPPORT
#include <time.h>
#include "httpd.h"
#endif

/* da_f@2005.1.31 */
#ifdef FILTER_WAN_SELECT_SUPPORT
struct ip_or_range_info
{
	int ip_num;
	int ip_node_num;
	char prefix_variable_name[64];
};
#endif

/* da_f 2005.3.18 add */
#ifdef NOMAD_SUPPORT

#define MAX_VPN_ACCOUNT 5
#define DEFAULT_VPN_USERNAME "3.1415"
#define DEFAULT_VPN_PASSWD "admin"

struct VPNAccount
{
	char username[32];
	char password[32];
	char change_enable[2];	/* "0": not allow to change passwd; "1": allow */
	char active_flag[2];	/* "0": not active; "1": active */
};

struct VPNCliStat
{
	char online[2];		/* "0": offine; "1": online */
	char s_time[32];
	char e_time[32];
};

extern int random_seed(int, int);
extern int gui_stopnomad_conn(int);
#endif

/* da_f@2005.1.30 */
#ifdef FILTER_WAN_SELECT_SUPPORT
void validate_filter_wan_ip(webs_t wp, char *value, struct variable *v);
#endif

/* 
   Format:
     filter_rule{1...10}=$STAT:1$NAME:test1$
	(1=>disable 2=>enable)

   Format:
     filter_tod{1...10} = hr:min hr:min wday
     filter_tod_buf{1...10} = sun mon tue wed thu fri sat	 //only for web page read
   Example:
     Everyday and 24-hour
     filter_todXX = 0:0 23:59 0-0
     filter_tod_bufXX = 7	(for web)

     From 9:55 to 22:00 every sun, wed and thu
     filter_todXX = 9:55 22:00 0,3-4
     filter_tod_bufXX = 1 0 1 1 0 0 0	(for web)

   Format:
     filter_ip_grp{1...10} = ip1 ip2 ip3 ip4 ip5 ip6 ip_r1-ipr2 ip_r3-ip_r4
     filter_ip_mac{1...10} = 00:11:22:33:44:55 00:12:34:56:78:90
 
   Format:
     filter_port=udp:111-222 both:22-33 disable:22-333 tcp:11-22222

   Converting Between AM/PM and 24 Hour Clock:
    Converting from AM/PM to 24 hour clock:
     12:59 AM -> 0059 	 (between 12:00 AM and 12:59 AM, subtract 12 hours)
     10:00 AM -> 1000    (between 1:00 AM and 12:59 PM, a straight conversion)
     10:59 PM -> 2259    (between 1:00 PM and 11:59 PM, add 12 hours)
    Converting from 24 hour clock to AM/PM
     0059 -> 12:59 AM    (between 0000 and 0059, add 12 hours)
     0100 -> 1:00  AM    (between 0100 and 1159, straight converion to AM)
     1259 -> 12:59 PM    (between 1200 and 1259, straight converion to PM)
     1559 -> 3:59  PM    (between 1300 and 2359, subtract 12 hours)
     
*/

int filter_id  = 1;
int day_all=0, week0=0, week1=0, week2=0, week3=0, week4=0, week5=0, week6=0;
int start_week=0, end_week=0;
int time_all=0, start_hour=0, start_min=0, start_time=0, end_hour=0, end_min=0, end_time=0;
int tod_data_null = 0;


void
validate_filter_ip_grp(webs_t wp, char *value, struct variable *v)
{
	int i=0, i_checkbox_value;
	char buf[256] = "";
	char *ip0, *ip1, *ip2, *ip3, *ip4, *ip5, *ip_range0_0, *ip_range0_1, *ip_range1_0, *ip_range1_1, *p_checkbox_value;
	unsigned char ip[10]={0,0,0,0,0,0,0};
	struct variable filter_ip_variables[] = {
		{ longname: "TCP/UDP IP address", argv: ARGV("0", "254") },
	}, *which;
	char _filter_lan_checkbox[] = "filter_lan_checkboxXXX";
	char _filter_ip[] = "filter_ip_grpXXX";
	//char _filter_rule[] = "filter_ruleXXX";
	//char _filter_tod[] = "filter_todXXX";

	which = &filter_ip_variables[0];

	/* Get the value of checkbox:"all_in_wan". da_f@2005.1.29  */
#ifdef FILTER_WAN_SELECT_SUPPORT
 	snprintf(_filter_lan_checkbox, sizeof(_filter_lan_checkbox), "filter_lan_checkbox%s", nvram_safe_get("filter_id"));
	p_checkbox_value = websGetVar(wp, "all_in_lan", "0");
	i_checkbox_value = atoi(p_checkbox_value);
	nvram_set(_filter_lan_checkbox, i_checkbox_value? "1" : "0");
	if(1 == i_checkbox_value)
		goto wan;
#endif

	ip0 = websGetVar(wp, "ip0", "0");
	ip1 = websGetVar(wp, "ip1", "0");
	ip2 = websGetVar(wp, "ip2", "0");
	ip3 = websGetVar(wp, "ip3", "0");
	ip4 = websGetVar(wp, "ip4", "0");
	ip5 = websGetVar(wp, "ip5", "0");
	ip_range0_0 = websGetVar(wp, "ip_range0_0", "0");
	ip_range0_1 = websGetVar(wp, "ip_range0_1", "0");
	ip_range1_0 = websGetVar(wp, "ip_range1_0", "0");
	ip_range1_1 = websGetVar(wp, "ip_range1_1", "0");

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): ip=[%s %s %s %s %s %s] [%s-%s %s-%s]",__FUNCTION__,ip0,ip1,ip2,ip3,ip4,ip5,ip_range0_0,ip_range0_1,ip_range1_0,ip_range1_1);
#endif

	if (!valid_range(wp, ip0, &which[0]) || 
	    !valid_range(wp, ip1, &which[0]) ||
	    !valid_range(wp, ip2, &which[0]) ||
	    !valid_range(wp, ip3, &which[0]) ||
	    !valid_range(wp, ip4, &which[0]) ||
	    !valid_range(wp, ip5, &which[0]) ||
	    !valid_range(wp, ip_range0_0, &which[0]) ||
	    !valid_range(wp, ip_range0_1, &which[0]) ||
	    !valid_range(wp, ip_range1_0, &which[0]) ||
	    !valid_range(wp, ip_range1_0, &which[0])){
		error_value = 1;
		return;
	}

	if(atoi(ip0))	ip[i++] = atoi(ip0);
	if(atoi(ip1))	ip[i++] = atoi(ip1);
	if(atoi(ip2))	ip[i++] = atoi(ip2);
	if(atoi(ip3))	ip[i++] = atoi(ip3);
	if(atoi(ip4))	ip[i++] = atoi(ip4);
	if(atoi(ip5))	ip[i++] = atoi(ip5);

	if(atoi(ip_range0_0) > atoi(ip_range0_1))
		SWAP(ip_range0_0,ip_range0_1);		

	if(atoi(ip_range1_0) > atoi(ip_range1_1))
		SWAP(ip_range1_0,ip_range1_1);		


	sprintf(buf,"%d %d %d %d %d %d %s-%s %s-%s",ip[0],ip[1],ip[2],ip[3],ip[4],ip[5],ip_range0_0,ip_range0_1,ip_range1_0,ip_range1_1);


 	snprintf(_filter_ip, sizeof(_filter_ip), "filter_ip_grp%s", nvram_safe_get("filter_id"));
	nvram_set(_filter_ip, buf);
	
 	//snprintf(_filter_rule, sizeof(_filter_rule), "filter_rule%s", nvram_safe_get("filter_id"));
 	//snprintf(_filter_tod, sizeof(_filter_tod), "filter_tod%s", nvram_safe_get("filter_id"));
	//if(nvram_match(_filter_rule, "")){
	//	nvram_set(_filter_rule, "$STAT:1$NAME:$$");
	//	nvram_set(_filter_tod, "0:0 23:59 0-6");
	//}

	/* Determine which WAN IP addresses can be accessed. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
wan:
	validate_filter_wan_ip(wp, value, v);
#endif
}

/* Save the IP address of the WAN Hosts or IP Range of the WAN Hosts. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
void
save_filter_wan_ip_or_range(webs_t wp, char *value, struct variable *v, const struct ip_or_range_info *info)
{
	int i;
	char buf[256], variable_name[128];

	int ip_num = info->ip_num;
	int ip_node_num = info->ip_node_num;
	char *p_prefix_name = info->prefix_variable_name;

	struct variable filter_ip_variables[] = { { longname: "TCP/UDP IP address", argv: ARGV("0", "254") }, }, 
					*which;

	bzero(buf, sizeof(buf));
	bzero(variable_name, sizeof(variable_name));

	which = &filter_ip_variables[0];

	for(i = 0; i < ip_num; i++)
	{
		int j;
		char ip[32];

		bzero(ip, sizeof(ip));

		for(j = 0; j < ip_node_num; j++)
		{
			char ip_node_name[32]; /* Example: wan_ip0_0, wan_ip_range0_0 */
			char *p_ip_node = NULL;
			char ip_node[8];

			bzero(ip_node_name, sizeof(ip_node_name));
			bzero(ip_node, sizeof(ip_node));

			if(4 == ip_node_num) /* 0.0.0.0 */
				snprintf(ip_node_name, sizeof(ip_node_name), "wan_ip%d_%d", i, j);
			else if(5 == ip_node_num) /* 0.0.0.0.0 */
				snprintf(ip_node_name, sizeof(ip_node_name), "wan_ip_range%d_%d", i, j);
			else
				return;

			p_ip_node = websGetVar(wp, ip_node_name, "0");
			if (!valid_range(wp, p_ip_node, &which[0]))
			{
				error_value = 1;
				return;
			}

			if((ip_node_num - 1) == j)
				snprintf(ip_node, sizeof(ip_node), "%s", p_ip_node);
			else
				snprintf(ip_node, sizeof(ip_node), "%s.", p_ip_node);

			strcat(ip, ip_node);
		}

		if(4 == ip_node_num && !strcmp(ip, "0.0.0.0"))
			continue;
		if(5 == ip_node_num && !strcmp(ip, "0.0.0.0.0"))
			continue;

		strcat(buf, ip);
		if((ip_num - 1) != i)
			strcat(buf, " ");
	}
 	snprintf(variable_name, sizeof(variable_name), "%s%s", p_prefix_name, nvram_safe_get("filter_id"));
	nvram_set(variable_name, buf);
}
#endif

/* Save WAN Address for FilterIPMAC.asp. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
void
validate_filter_wan_ip(webs_t wp, char *value, struct variable *v)
{
	int i_checkbox_value;
	char *p_checkbox_value, _filter_wan_checkbox[] = "filter_wan_checkboxXXX";
	struct ip_or_range_info ip_group_info = { 6, 4, "filter_wan_ip_grp\0" };
	struct ip_or_range_info ip_range_info = { 2, 5, "filter_wan_ip_range\0" };

	/* Get the value of checkbox:"all_in_wan" */
 	snprintf(_filter_wan_checkbox, sizeof(_filter_wan_checkbox), "filter_wan_checkbox%s", nvram_safe_get("filter_id"));
	p_checkbox_value = websGetVar(wp, "all_in_wan", "0");
	i_checkbox_value = atoi(p_checkbox_value);
	nvram_set(_filter_wan_checkbox, i_checkbox_value? "1" : "0");
	if(1 == i_checkbox_value)
		return;

	/* Format: "210.5.192.3 110.52.10.28 61.144.56.100 202.96.128.68 218.30.128.176 218.30.55.65" */
	save_filter_wan_ip_or_range(wp, value, v, &ip_group_info);
	/* Format: "210.30.128.176.254 218.30.66.1.254 */
	save_filter_wan_ip_or_range(wp, value, v, &ip_range_info);
}
#endif

/* Example:
 * tcp:100-200 udp:210-220 both:250-260
 */
void
validate_filter_port(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	struct variable filter_port_variables[] = {
		{ longname: "TCP/UDP Port Filter Starting LAN Port", argv: ARGV("0", "65535") },
		{ longname: "TCP/UDP Port Filter Ending LAN Port", argv: ARGV("0", "65535") },
	}, *which;

	which = &filter_port_variables[0];

	for (i = 0; i < FILTER_PORT_NUM ; i++) {
		char filter_port[] = "protoXXX";
		char filter_port_start[] = "startXXX";
		char filter_port_end[] = "endXXX";
		char *port, *start, *end;
		char *temp;

 		snprintf(filter_port, sizeof(filter_port), "proto%d", i);
 		snprintf(filter_port_start, sizeof(filter_port_start), "start%d", i);
		snprintf(filter_port_end, sizeof(filter_port_end), "end%d", i);
		port = websGetVar(wp, filter_port, NULL);
		start = websGetVar(wp, filter_port_start, NULL);
		end = websGetVar(wp, filter_port_end, NULL);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,filter_port,port,filter_port_start,start,filter_port_end,end);
#endif

		if (!port || !start || !end)
			continue;

		if (!*start && !*end)
			continue;

		if ((!strcmp(start,"0") || !strcmp(start,"")) &&
		    (!strcmp(end,"0") || !strcmp(end,"")))
			continue;

		if (!*start || !*end) {
			//websWrite(wp, "Invalid <b>%s</b>: must specify a LAN Port Range<br>", v->longname);
			continue;
		}
		if (!valid_range(wp, start, &which[0]) || !valid_range(wp, end, &which[1])){
			error_value = 1;
			continue;
		}
		if (atoi(start) > atoi(end)) {
			temp = start;
			start = end;
			end = temp;			
		}
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%d-%d",
				cur == buf ? "" : " ", port, atoi(start), atoi(end));
	}
	

	nvram_set(v->name, buf);

}

void
validate_filter_dport_grp(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	struct variable filter_port_variables[] = {
		{ longname: "TCP/UDP Port Filter Starting LAN Port", argv: ARGV("0", "65535") },
		{ longname: "TCP/UDP Port Filter Ending LAN Port", argv: ARGV("0", "65535") },
	}, *which;
	char _filter_port[] = "filter_dport_grpXXX";
	//char _filter_rule[] = "filter_ruleXXX";
	//char _filter_tod[] = "filter_todXXX";

	which = &filter_port_variables[0];

	for (i = 0; i < FILTER_PORT_NUM ; i++) {
		char filter_port[] = "protoXXX";
		char filter_port_start[] = "startXXX";
		char filter_port_end[] = "endXXX";
		char *port, *start, *end;
		char *temp;

 		snprintf(filter_port, sizeof(filter_port), "proto%d", i);
 		snprintf(filter_port_start, sizeof(filter_port_start), "start%d", i);
		snprintf(filter_port_end, sizeof(filter_port_end), "end%d", i);
		port = websGetVar(wp, filter_port, NULL);
		start = websGetVar(wp, filter_port_start, NULL);
		end = websGetVar(wp, filter_port_end, NULL);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,filter_port,port,filter_port_start,start,filter_port_end,end);
#endif

		if (!port || !start || !end)
			continue;

		if (!*start && !*end)
			continue;

		if ((!strcmp(start,"0") || !strcmp(start,"")) &&
		    (!strcmp(end,"0") || !strcmp(end,"")))
			continue;

		if (!*start || !*end) {
			//websWrite(wp, "Invalid <b>%s</b>: must specify a LAN Port Range<br>", v->longname);
			continue;
		}
		if (!valid_range(wp, start, &which[0]) || !valid_range(wp, end, &which[1])){
			error_value = 1;
			continue;
		}
		if (atoi(start) > atoi(end)) {
			temp = start;
			start = end;
			end = temp;			
		}
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%d-%d",
				cur == buf ? "" : " ", port, atoi(start), atoi(end));
	}
	
 	snprintf(_filter_port, sizeof(_filter_port), "filter_dport_grp%s", nvram_safe_get("filter_id"));
	nvram_set(_filter_port, buf);
	
 	//snprintf(_filter_rule, sizeof(_filter_rule), "filter_rule%s", nvram_safe_get("filter_id"));
 	//snprintf(_filter_tod, sizeof(_filter_tod), "filter_tod%s", nvram_safe_get("filter_id"));
	//if(nvram_match(_filter_rule, "")){
	//	nvram_set(_filter_rule, "$STAT:1$NAME:$$");
	//	nvram_set(_filter_tod, "0:0 23:59 0-6");
	//}

}

/* Example:
 * 2 00:11:22:33:44:55 00:11:22:33:44:56
 */

void
validate_filter_mac_grp(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	char _filter_mac[] = "filter_mac_grpXXX";
	//char _filter_rule[] = "filter_ruleXXX";
	//har _filter_tod[] = "filter_todXXX";

	for (i=0; i<FILTER_MAC_NUM ; i++) {
		char filter_mac[] = "macXXX";
		char *mac, mac1[20]="";
		

 		snprintf(filter_mac, sizeof(filter_mac), "mac%d", i);

		mac = websGetVar(wp, filter_mac, NULL);
		if(!mac)	continue;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): [%s]=[%s]",__FUNCTION__,i,filter_mac,mac);
#endif
		if(strcmp(mac,"") && strcmp(mac,"00:00:00:00:00:00") && strcmp(mac,"000000000000")){
			if(strlen(mac) == 12){
				char hex[] = "XX";
				unsigned char h;
				while (*mac) {
		                        strncpy(hex, mac, 2);
		                        h = (unsigned char) strtoul(hex, NULL, 16);
					if(strlen(mac1))
						sprintf(mac1+strlen(mac1), ":");
					sprintf(mac1+strlen(mac1), "%02X", h);
		                        mac += 2;
		                }
				mac1[17] = '\0';
			}
			else if(strlen(mac) == 17){
				strcpy(mac1, mac);
			}
			if (!valid_hwaddr(wp, mac1, v)){
				error_value = 1;
				continue;
			}
		}	
		else{
			continue;
		}
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", mac1);
	}


 	snprintf(_filter_mac, sizeof(_filter_mac), "filter_mac_grp%s", nvram_safe_get("filter_id"));
	nvram_set(_filter_mac, buf);

 	//snprintf(_filter_rule, sizeof(_filter_rule), "filter_rule%s", nvram_safe_get("filter_id"));
 	//snprintf(_filter_tod, sizeof(_filter_tod), "filter_tod%s", nvram_safe_get("filter_id"));
	//if(nvram_match(_filter_rule, "")){
	//	nvram_set(_filter_rule, "$STAT:1$NAME:$$");
	//	nvram_set(_filter_tod, "0:0 23:59 0-6");
	//}
}


/* Example:
 * 100-200 250-260 (ie. 192.168.1.100-192.168.1.200 192.168.1.250-192.168.1.260)
 */

char
*filter_ip_get(char *type, int which)
{
	static char word[256];
	char *start, *end, *wordlist, *next;
	int temp;
	char filter_ip[] = "filter_ip_grpXXX";
        snprintf(filter_ip, sizeof(filter_ip), "filter_ip_grp%s", nvram_safe_get("filter_id"));
	
	wordlist = nvram_safe_get(filter_ip);
	if(!wordlist)	return "0";

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"[%s]=[%s]",filter_ip,wordlist);
#endif
	temp = which;

	foreach(word, wordlist, next) {
		if (which-- == 0) {
			if(temp == 6){
				end = word;
				start = strsep(&end, "-");
				if (!strcmp(type, "ip_range0_0"))
					return start;	
				else
					return end;
			}
			else if(temp == 7){
				end = word;
				start = strsep(&end, "-");
				if (!strcmp(type, "ip_range1_0"))
					return start;	
				else
					return end;
			}
			return word;
		}
	}

	return "0";
}

/* Example:
 * tcp:100-200 udp:210-220 both:250-260
 */

char 
*filter_dport_get(char *type,int which)
{
	static char word[256];
	char *wordlist, *next;
	char *start, *end, *proto;
	int temp;
	char name[] = "filter_dport_grpXXX";
	
	sprintf(name, "filter_dport_grp%s", nvram_safe_get("filter_id"));
	wordlist = nvram_safe_get(name);
	temp = which;

	foreach(word, wordlist, next) {
		if (which-- == 0) {
			start = word;
			proto = strsep(&start, ":"); 
			end = start;
			start = strsep(&end, "-");
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): proto=[%s] start=[%s] end=[%s]",__FUNCTION__,type,temp,proto,start,end);
#endif
			if (!strcmp(type, "disable")){
				if (!strcmp(proto,"disable"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "both")){
				if (!strcmp(proto,"both"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "tcp")){
				if (!strcmp(proto,"tcp"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "udp")){
				if (!strcmp(proto,"udp"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "start"))
				return start;
			else if (!strcmp(type, "end"))
				return end;
		}
	}

	if (!strcmp(type, "start") || !strcmp(type, "end"))
		return "0";
	else
		return "";
}

int
ej_filter_dport_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, which;
	char *type;

        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): type=[%s] which=[%d]",__FUNCTION__, type, which);
#endif

	ret = websWrite(wp,"%s",filter_dport_get(type,which));


	return ret;

}

/* Example:
 * tcp:100-200 udp:210-220 both:250-260
 */

char 
*filter_port_get(char *type,int which)
{
	static char word[256];
	char *wordlist, *next;
	char *start, *end, *proto;
	int temp;

	wordlist = nvram_safe_get("filter_port");
	temp = which;

	foreach(word, wordlist, next) {
		if (which-- == 0) {
			start = word;
			proto = strsep(&start, ":"); 
			end = start;
			start = strsep(&end, "-");
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): proto=[%s] start=[%s] end=[%s]",__FUNCTION__,type,temp,proto,start,end);
#endif
			if (!strcmp(type, "disable")){
				if (!strcmp(proto,"disable"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "both")){
				if (!strcmp(proto,"both"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "tcp")){
				if (!strcmp(proto,"tcp"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "udp")){
				if (!strcmp(proto,"udp"))
					return "selected";
				else
					return " ";
			}
			else if (!strcmp(type, "start"))
				return start;
			else if (!strcmp(type, "end"))
				return end;
		}
	}

	if (!strcmp(type, "start") || !strcmp(type, "end"))
		return "0";
	else
		return "";
}

int
ej_filter_port_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, which;
	char *type;

        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): type=[%s] which=[%d]",__FUNCTION__, type, which);
#endif

	ret = websWrite(wp,"%s",filter_port_get(type,which));


	return ret;

}

/* Example:
 * 00:11:22:33:44:55 00:11:22:33:44:56
 */

char 
*filter_mac_get(int which)
{
	static char word[256];
	char *wordlist, *next;
	char *mac;
	int temp;
	char filter_mac[] = "filter_mac_grpXXX";

        snprintf(filter_mac, sizeof(filter_mac), "filter_mac_grp%s", nvram_safe_get("filter_id"));
	
	wordlist = nvram_safe_get(filter_mac);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"[%s]=[%s]",filter_mac,wordlist);
#endif
	if(!wordlist)	return "";
		
	temp = which;

	foreach(word, wordlist, next) {
		if (which-- == 0) {
			mac = word;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): mac=[%s]",__FUNCTION__,temp,mac);
#endif
			return mac;
		}
	}
	return "00:00:00:00:00:00";
}


int
ej_filter_ip_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret=0, which;
	char *type;

        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): which=[%d] type=[%s]",__FUNCTION__,which,type);
#endif

	ret = websWrite(wp,"%s",filter_ip_get(type,which));


	return ret;
}


int
ej_filter_mac_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, which;

        if (ejArgs(argc, argv, "%d", &which) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): which=[%d]",__FUNCTION__,which);
#endif

	ret = websWrite(wp,"%s",filter_mac_get(which));

	return ret;
}
#if 0
void
validate_filter_ip_grp(webs_t wp, char *value, struct variable *v)
{
	char *filter_ip_grp_1, *filter_ip_grp_2;	

	filter_ip_grp_1= websGetVar(wp, "filter_ip_grp_1", NULL);
	filter_ip_grp_2= websGetVar(wp, "filter_ip_grp_2", NULL);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): filter_ip_grp_1=[%s] filter_ip_grp_2=[%s]",__FUNCTION__,filter_ip_grp_1,filter_ip_grp_2);
#endif
	if(!filter_ip_grp_1)	return;

	if(!strcmp(filter_ip_grp_1,"0"))
		nvram_set(v->name,"0");
	else if(!strcmp(filter_ip_grp_1,"other")){
		if(!filter_ip_grp_2)	return;
                nvram_set(v->name,filter_ip_grp_2);
	}
}

void
validate_filter_mac_grp(webs_t wp, char *value, struct variable *v)
{
	char *filter_mac_grp_1, *filter_mac_grp_2;	

	filter_mac_grp_1= websGetVar(wp, "filter_mac_grp_1", NULL);
	filter_mac_grp_2= websGetVar(wp, "filter_mac_grp_2", NULL);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): filter_mac_grp_1=[%s] filter_mac_grp_2=[%s]",__FUNCTION__,filter_mac_grp_1,filter_mac_grp_2);
#endif

	if(!filter_mac_grp_1)	return;

	if(!strcmp(filter_mac_grp_1,"0"))
		nvram_set(v->name,"0");
	else if(!strcmp(filter_mac_grp_1,"other")){
		if(!filter_mac_grp_2)	return;
                nvram_set(v->name,filter_mac_grp_2);
	}
}
#endif

/*  Format: url0=www.kimo.com.tw, ...
 *	    keywd0=sex, ...
 */
void
validate_filter_web(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	char buf1[1000] = "", *cur1 = buf1;
	char filter_host[] = "filter_web_hostXXX";
	char filter_url[] = "filter_web_urlXXX";

	/* Handle Website Blocking by URL Address */	
	for (i=0; i<4 ; i++) {
		char filter_host[] = "urlXXX";
		char *host;

 		snprintf(filter_host, sizeof(filter_host), "host%d", i);
		host = websGetVar(wp, filter_host, "");
		
		if(!strcmp(host, ""))	continue;

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : "<&nbsp;>", host);
	}

	if(strcmp(buf, ""))
		strcat(buf,"<&nbsp;>");

 	snprintf(filter_host, sizeof(filter_host), "filter_web_host%s", nvram_safe_get("filter_id"));
	nvram_set(filter_host, buf);
	
	/* Handle Website Blocking by Keyword */
	for (i=0; i<6 ; i++) {
		char filter_url[] = "urlXXX";
		char *url;

 		snprintf(filter_url, sizeof(filter_url), "url%d", i);
		url = websGetVar(wp, filter_url, "");
		
		if(!strcmp(url, ""))	continue;

		cur1 += snprintf(cur1, buf1 + sizeof(buf1) - cur1, "%s%s",
				cur1 == buf1 ? "" : "<&nbsp;>", url);
	}
	if(strcmp(buf1,""))
		strcat(buf1, "<&nbsp;>");

 	snprintf(filter_url, sizeof(filter_url), "filter_web_url%s", nvram_safe_get("filter_id"));
	nvram_set(filter_url, buf1);

}

int
validate_filter_tod(webs_t wp)
{
	char buf[256] = "";
	char tod_buf[20];
	struct variable filter_tod_variables[] = {
		{ longname: "Tod name", argv: ARGV("20") },
		{ longname: "Tod Status", argv: ARGV("0", "1","2") },

	}, *which;


	char *day_all, *week0, *week1, *week2, *week3, *week4, *week5, *week6;
	char *time_all, *start_hour, *start_min, *start_time, *end_hour, *end_min, *end_time;
	int _start_hour, _start_min, _end_hour, _end_min;
	char time[20];
	int week[7];
	int i, flag=-1, dash=0;
	char filter_tod[] = "filter_todXXX";
	char filter_tod_buf[] = "filter_tod_bufXXX";

	which = &filter_tod_variables[0];

	day_all = websGetVar(wp, "day_all", "0");
	week0 = websGetVar(wp, "week0", "0");
	week1 = websGetVar(wp, "week1", "0");
	week2 = websGetVar(wp, "week2", "0");
	week3 = websGetVar(wp, "week3", "0");
	week4 = websGetVar(wp, "week4", "0");
	week5 = websGetVar(wp, "week5", "0");
	week6 = websGetVar(wp, "week6", "0");
	time_all = websGetVar(wp, "time_all", "0");
	start_hour = websGetVar(wp, "start_hour", "0");
	start_min = websGetVar(wp, "start_min", "0");
	start_time = websGetVar(wp, "start_time", "0");
	end_hour = websGetVar(wp, "end_hour", "0");
	end_min = websGetVar(wp, "end_min", "0");
	end_time = websGetVar(wp, "end_time", "0");

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): day_all=[%s] week=[%s %s %s %s %s %s %s] time_all=[%s] start=[%s:%s:%s] end=[%s:%s:%s]",__FUNCTION__,filter_id,day_all,week0,week1,week2,week3,week4,week5,week6,time_all,start_hour,start_min,start_time,end_hour,end_min,end_time);
#endif
	//if(atoi(time_all) == 0)
	//	if(!start_hour || !start_min || !start_time || !end_hour || !end_min || !end_time)
	//		return 1;

	if(atoi(day_all) == 1){
		strcpy(time,"0-6");
		strcpy(tod_buf,"7");
	}
	else{
		week[0] = atoi(week0);
		week[1] = atoi(week1);
		week[2] = atoi(week2);
		week[3] = atoi(week3);
		week[4] = atoi(week4);
		week[5] = atoi(week5);
		week[6] = atoi(week6);
		strcpy(time,"");

		for(i = 0 ; i < 7 ; i ++){
			if(week[i] == 1){
				if(i == 6){
					 if(dash == 0 && flag == 1)
                                                sprintf(time+strlen(time),"%c",'-');
					 sprintf(time+strlen(time),"%d",i);
				}
				else if(flag == 1 && dash == 0){
					sprintf(time+strlen(time),"%c",'-');
					dash = 1;
				}
				else if(dash ==0){
					sprintf(time+strlen(time),"%d",i);
					flag = 1;
					dash = 0;
				}
			}
			else{
				if(!strcmp(time,""))	continue;
				if(dash == 1)
					sprintf(time+strlen(time),"%d",i-1);
				if(flag != 0)
					sprintf(time+strlen(time),"%c",',');
				flag = 0;
				dash = 0;
			}
		}
		if(time[strlen(time)-1] == ',')
			time[strlen(time)-1] = '\0';

		snprintf(tod_buf,sizeof(tod_buf),"%s %s %s %s %s %s %s",week0, week1, week2, week3, week4, week5, week6);
	}
	if(atoi(time_all) == 1){
		_start_hour = 0;
		_start_min = 0;
		_end_hour = 23;
		_end_min = 59;
	}
	else{
		_start_hour = atoi(start_time) ? 12 + atoi(start_hour) : atoi(start_hour);
		_start_min = atoi(start_min);
		_end_hour = atoi(end_time) ? 12 + atoi(end_hour) : atoi(end_hour);
		_end_min = atoi(end_min);
	}

	sprintf(buf,"%d:%d %d:%d %s",_start_hour,_start_min,_end_hour,_end_min,time);
 	snprintf(filter_tod, sizeof(filter_tod), "filter_tod%s", nvram_safe_get("filter_id"));
 	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_tod_buf%s", nvram_safe_get("filter_id"));
	
	nvram_set(filter_tod, buf);
	nvram_set(filter_tod_buf, tod_buf);

	return 0;
	
}

int
delete_policy(webs_t wp, int which)
{
	char filter_rule[] = "filter_ruleXXX";
	char filter_tod[] = "filter_todXXX";
	char filter_tod_buf[] = "filter_tod_bufXXX";
	char filter_host[] = "filter_web_hostXXX";
	char filter_url[] = "filter_web_urlXXX";
	char filter_ip_grp[] = "filter_ip_grpXXX";
	char filter_mac_grp[] = "filter_mac_grpXXX";
	char filter_port_grp[] = "filter_port_grpXXX";
	//zhangbin 2004.11.18
	char filter_dport_grp[] = "filter_dport_grpXXX";

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): which=[%d]",__FUNCTION__,which);
#endif

 	snprintf(filter_rule, sizeof(filter_rule), "filter_rule%d", which);
 	snprintf(filter_tod, sizeof(filter_tod), "filter_tod%d", which);
 	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_tod_buf%d", which);
 	snprintf(filter_host, sizeof(filter_host), "filter_web_host%d", which);
 	snprintf(filter_url, sizeof(filter_url), "filter_web_url%d", which);
 	snprintf(filter_ip_grp, sizeof(filter_ip_grp), "filter_ip_grp%d", which);
 	snprintf(filter_mac_grp, sizeof(filter_mac_grp), "filter_mac_grp%d", which);
 	snprintf(filter_port_grp, sizeof(filter_port_grp), "filter_port_grp%d", which);
 	snprintf(filter_dport_grp, sizeof(filter_dport_grp), "filter_dport_grp%d", which);

	nvram_set(filter_rule,"");
	nvram_set(filter_tod,"");
	nvram_set(filter_tod_buf,"");
	nvram_set(filter_host,"");
	nvram_set(filter_url,"");
	nvram_set(filter_ip_grp,"");
	nvram_set(filter_mac_grp,"");
	nvram_set(filter_port_grp,"");
	nvram_set(filter_dport_grp,"");
	
	return 1;
}

int
single_delete_policy(webs_t wp)
{
	int ret = 0;
	char *id = nvram_safe_get("filter_id");

	ret = delete_policy(wp, atoi(id));

	return ret;
}

int
summary_delete_policy(webs_t wp)
{
	int i, ret = 0;

	for(i=1 ; i<=10 ; i++){
		char filter_sum[] = "sumXXX";
		char *sum;
	 	snprintf(filter_sum, sizeof(filter_sum), "sum%d", i);
		sum = websGetVar(wp, filter_sum, NULL);
		if(sum)
			ret += delete_policy(wp, i);
	}

	return ret;
}

int
save_policy(webs_t wp)
{
	char *f_id, *f_name, *f_status, *f_status2;
	char buf[256] = "";
	struct variable filter_variables[] = {
		{ longname: "Filter ID", argv: ARGV("1","10") },
		{ longname: "Filter Status", argv: ARGV("0","1","2") },
		{ longname: "Filter Status", argv: ARGV("deny","allow") },

	}, *which;
	char filter_buf[] = "filter_ruleXXX";
	which = &filter_variables[0];

	f_id = websGetVar(wp, "f_id", NULL);
	f_name = websGetVar(wp, "f_name", NULL);
	f_status = websGetVar(wp, "f_status", NULL);	// 0=>Disable / 1,2=>Enable
	f_status2 = websGetVar(wp, "f_status2", NULL);	// deny=>Deny / allow=>Allow
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): f_id=[%s] f_name=[%s] f_status=[%s]",__FUNCTION__,f_id,f_name,f_status);
#endif
	if(!f_id || !f_name || !f_status || !f_status2){
		error_value = 1;
		return -1;
	}
	if(!valid_range(wp, f_id, &which[0])){
		error_value = 1;
		return -1;
        }
	if(!valid_choice(wp, f_status, &which[1])){
		error_value = 1;
		return -1;
        }
	if(!valid_choice(wp, f_status2, &which[2])){
		error_value = 1;
		return -1;
        }

	validate_filter_tod(wp);

 	snprintf(filter_buf, sizeof(filter_buf), "filter_rule%s", nvram_safe_get("filter_id"));

	// Add $DENY to decide that users select Allow or Deny, if status is Disable	// 2003/10/21
	snprintf(buf,sizeof(buf),"$STAT:%s$NAME:%s$DENY:%d$$", f_status, f_name, !strcmp(f_status2,"deny") ? 1 : 0);
	
	nvram_set(filter_buf,buf);

	return 0;
}

void
validate_filter_policy(webs_t wp, char *value, struct variable *v)
{
	char *f_id = websGetVar(wp, "f_id", NULL);

	if(f_id)
		nvram_set("filter_id", f_id);
	else
		nvram_set("filter_id", "1");

	save_policy(wp);

}

/*   Format: 21:21:tcp:FTP(&nbsp;)500:1000:both:TEST1
 *
 */

int
validate_services_port(webs_t wp)
{
	char buf[1000] = "", *cur = buf;
	char *services_array = websGetVar(wp, "services_array", NULL);
	char *services_length = websGetVar(wp, "services_length", NULL);
	char word[1024], *next;
	char delim[] = "(&nbsp;)";
	
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): services_array=[%s] service_length=[%s]", __FUNCTION__, services_array, services_length);
#endif	

	if(!services_array || !services_length)		return 0;
	
	split(word, services_array, next, delim){
		int from, to, proto;
		char name[80];
		if(sscanf(word, "%d:%d:%d:%s", &from, &to, &proto, name) != 4 )
			continue;
#ifdef MY_DEBUG
		LOG(LOG_DEBUG,"%s(): from=[%d] to=[%d] proto=[%d] name=[%s]", __FUNCTION__, from, to, proto, name);
#endif

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s$NAME:%03d:%s$PROT:%03d:%s$PORT:%03d:%d:%d",
				cur == buf ? "" : "<&nbsp;>", 
				strlen(name), name, 
				strlen(num_to_protocol(proto)), num_to_protocol(proto),
				(get_int_len(from)+get_int_len(to)+strlen(":")), 
				from, to);
	}
	nvram_set("filter_services", buf);
	return 1;
}

int
save_services_port(webs_t wp)
{
	return validate_services_port(wp);
}

void
validate_blocked_service(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	char port_grp[] = "filter_port_grpXXX";

	for(i=0 ; i<BLOCKED_SERVICE_NUM ; i++){
		char blocked_service[] = "blocked_serviceXXX";
		char *service;
		snprintf(blocked_service, sizeof(blocked_service), "blocked_service%d", i);
		service = websGetVar(wp, blocked_service, NULL);
		if(!service || !strcmp(service, "None"))	continue;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): %s=[%s]", __FUNCTION__, blocked_service, service);
#endif	

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s", service, "<&nbsp;>");
                       //cur == buf ? "" : "<&nbsp;>", service);
	}

 	snprintf(port_grp, sizeof(port_grp), "filter_port_grp%s", nvram_safe_get("filter_id"));
	nvram_set(port_grp, buf);

}
	

int
ej_filter_policy_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0;

	for(i = 1 ; i <= 10 ; i ++ ){
		char filter[] = "filter_ruleXXX";
		char *data="";
        	char name[50]="";
 		snprintf(filter, sizeof(filter), "filter_rule%d", i);
		data = nvram_safe_get(filter);

		if(data && strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","");	// get name value
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): data=[%s] name=[%s]",__FUNCTION__,i,data,name);
#endif	
		}
		ret += websWrite(wp,"<option value=%d %s>%d ( %s ) </option>\n",
			i,
			(atoi(nvram_safe_get("filter_id")) == i ? "selected" : ""),
			i,
			name);
	}
	return ret;
}


int
ej_filter_policy_get(int eid, webs_t wp, int argc, char_t **argv)
{

	char *type, *part;
	int ret = 0;


        if (ejArgs(argc, argv, "%s %s", &type, &part) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	if(!strcmp(type,"f_id")){
		ret = websWrite(wp,"%s",nvram_safe_get("filter_id"));	
	}
	else if(!strcmp(type,"f_name")){
     		char name[50]="";
		char filter[] = "filter_ruleXXX";
		char *data="";
 		snprintf(filter, sizeof(filter), "filter_rule%s", nvram_safe_get("filter_id"));
		data = nvram_safe_get(filter);
		if(strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","");	// get name value
			ret = websWrite(wp,"%s",name);	
		}
	}
	else if(!strcmp(type,"f_status")){
     		char status[50]="", deny[50]="";
		char filter[] = "filter_ruleXXX";
		char *data="";
 		snprintf(filter, sizeof(filter), "filter_rule%s", nvram_safe_get("filter_id"));
		data = nvram_safe_get(filter);
		if(strcmp(data,"")){	// have data
			find_match_pattern(status, sizeof(status), data, "$STAT:","1");	// get status value
			find_match_pattern(deny, sizeof(deny), data, "$DENY:", "");	// get deny value
			if(!strcmp(deny,"")){	// old format
				if(!strcmp(status,"0") || !strcmp(status,"1"))
					strcpy(deny,"1");	// Deny
				else
					strcpy(deny,"0");	// Allow
			}
#if 0
			if(!strcmp(part,"disable")){
				if(!strcmp(status,"1"))
					ret = websWrite(wp,"checked");	
			}
			else if(!strcmp(part,"enable")){
				if(!strcmp(status,"2"))
					ret = websWrite(wp,"checked");	
			}
#endif
			if(!strcmp(part,"disable")){
				if(!strcmp(status,"0"))
					ret = websWrite(wp,"checked");	
			}
			else if(!strcmp(part,"enable")){
				if(strcmp(status,"0"))
					ret = websWrite(wp,"checked");	
			}
			else if(!strcmp(part,"deny")){
				if(!strcmp(deny,"1"))
					ret = websWrite(wp,"checked");	
			}
			else if(!strcmp(part,"allow")){
				if(!strcmp(deny,"0"))
					ret = websWrite(wp,"checked");	
			}
			else if(!strcmp(part,"onload_status")){
				if(!strcmp(deny,"1"))
					ret = websWrite(wp,"deny");	
				else
					ret = websWrite(wp,"allow");	
				
			}
			else if(!strcmp(part,"init")){
				if(!strcmp(status,"1"))
					ret = websWrite(wp,"disable");	
				else if(!strcmp(status,"2"))
					ret = websWrite(wp,"enable");
				else
					ret = websWrite(wp,"disable");
			}
		}
		else{	// no data
			if(!strcmp(part,"disable"))
				ret = websWrite(wp,"checked");
			else if(!strcmp(part,"allow"))		// default policy is allow, 2003-10-21
				ret = websWrite(wp,"checked");
			else if(!strcmp(part,"onload_status"))	// default policy is allow, 2003-10-21
				ret = websWrite(wp,"allow");	
			else if(!strcmp(part,"init"))
				ret = websWrite(wp,"disable");	
		}
	}

	return ret;
}

int
filter_tod_init(int which)
{
	char  *tod_data, *tod_buf_data;
	char filter_tod[] = "filter_todXXX";
	char filter_tod_buf[] = "filter_tod_bufXXX";
	char temp[3][20];
	int ret;

	tod_data_null = 0;
	day_all = week0 = week1 = week2 = week3 = week4 = week5 = week6 = 0;
	time_all = start_hour = start_min = start_time = end_hour = end_min = end_time = 0;
	start_week = end_week = 0;

 	snprintf(filter_tod, sizeof(filter_tod), "filter_tod%d", which);
 	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_tod_buf%d", which);
	
	/* Parse filter_tod{1...10} */
	tod_data = nvram_safe_get(filter_tod);
	if(!tod_data)	return -1;	// no data
	if(strcmp(tod_data,"")){
		sscanf(tod_data,"%s %s %s",temp[0],temp[1],temp[2]);
		sscanf(temp[0],"%d:%d",&start_hour,&start_min);
		sscanf(temp[1],"%d:%d",&end_hour,&end_min);
		ret = sscanf(temp[2],"%d-%d",&start_week,&end_week);	
		if(ret == 1)	end_week = start_week;

		if(start_hour == 0 && start_min ==0 && end_hour == 23 && end_min== 59){	// 24 Hours
			time_all = 1;
			start_hour = end_hour = 0;
			start_min = start_time = end_min = end_time = 0;	
		}
		else{	// check AM or PM
			time_all = 0;
			if(start_hour > 11 ){
				start_hour = start_hour - 12;
				start_time = 1;
			}
			if(end_hour > 11 ){
				end_hour = end_hour - 12;
				end_time = 1;
			}
		}
	}
	else{	// default Everyday and 24 Hours
		tod_data_null = 1;
		day_all = 1;
		time_all = 1;
	}

	if(tod_data_null == 0){
		/* Parse filter_tod_buf{1...10} */
		tod_buf_data = nvram_safe_get(filter_tod_buf);
		if(!strcmp(tod_buf_data,"7")){
				day_all = 1;
		}
		else if(strcmp(tod_buf_data,"")){
			sscanf(tod_buf_data,"%d %d %d %d %d %d %d",&week0, &week1, &week2, &week3, &week4, &week5, &week6);	
			day_all = 0;
		}
	}

#ifdef MY_DEBUG
	if(tod_data_null == 1)
		LOG(LOG_DEBUG,"%s(%d): tod data null",__FUNCTION__,which);
	else
		LOG(LOG_DEBUG,"%s(%d):day_all=[%d] week=[%d %d %d %d %d %d %d] time_all=[%d] [%d %d %d %d %d %d]",__FUNCTION__,which,day_all,week0,week1,week2,week3,week4,week5,week6,time_all,start_hour,start_min,start_time,end_hour,end_min,end_time);
#endif	
	return 0;
}

int
ej_filter_tod_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int ret=0 ,i;

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	filter_tod_init(atoi(nvram_safe_get("filter_id")));

	if(!strcmp(type,"day_all_init")){
		if(day_all == 0)
			ret = websWrite(wp,"1");
		else
			ret = websWrite(wp,"0");	
	}
	else if(!strcmp(type,"time_all_init")){
		if(time_all == 0)
			ret = websWrite(wp,"1");	
		else
			ret = websWrite(wp,"0");	
	}
	else if(!strcmp(type,"day_all")){
		ret = websWrite(wp,"%s",day_all == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"start_week")){
		ret = websWrite(wp,"%d",start_week);	
	}
	else if(!strcmp(type,"end_week")){
		ret = websWrite(wp,"%d",end_week);	
	}
	else if(!strcmp(type,"week0")){	// Sun
		ret = websWrite(wp,"%s",week0 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week1")){	// Mon
		ret = websWrite(wp,"%s",week1 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week2")){	// Tue
		ret = websWrite(wp,"%s",week2 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week3")){	// Wed
		ret = websWrite(wp,"%s",week3 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week4")){	// Thu
		ret = websWrite(wp,"%s",week4 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week5")){	// Fri
		ret = websWrite(wp,"%s",week5 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"week6")){	// Sat
		ret = websWrite(wp,"%s",week6 == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"time_all_en")){	// for linksys
		ret = websWrite(wp,"%s",time_all == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"time_all_dis")){	// for linksys
		ret = websWrite(wp,"%s",time_all == 0 ? "checked" : "");	
	}
	else if(!strcmp(type,"time_all")){
		ret = websWrite(wp,"%s",time_all == 1 ? "checked" : "");	
	}
	else if(!strcmp(type,"start_hour_24")){	// 00 -> 23
		for(i=0 ; i<24 ; i++){
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",i, i == start_hour+start_time*12 ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"start_min_15")){	// 0 15 30 45
		for(i=0 ; i<4 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*15, i*15 == start_min ? "selected" : "", i*15);	
		}
	}
	else if(!strcmp(type,"end_hour_24")){	// 00 ->23
		for(i=0 ; i<24 ; i++){
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",i, i == end_hour+end_time*12 ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"end_min_15")){	// 0 15 30 45
		for(i=0 ; i<4 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*15, i*15 == end_min ? "selected" : "", i*15);	
		}
	}
	else if(!strcmp(type,"start_hour_12")){	// 1 -> 12
		for(i=1 ; i<=12 ; i++){		
			int j;
			if(i == 12)	j = 0;
			else		j = i;
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",j, j == start_hour ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"start_min_5")){	// 0 5 10 15 20 25 30 35 40 45 50 55
		for(i=0 ; i<12 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*5, i*5 == start_min ? "selected" : "", i*5);	
		}
	}
	else if(!strcmp(type,"start_time_am")){
		ret = websWrite(wp,"%s",start_time == 1 ? "" : "selected");	
	}
	else if(!strcmp(type,"start_time_pm")){
		ret = websWrite(wp,"%s",start_time == 1 ? "selected" : "");	
	}
	else if(!strcmp(type,"end_hour_12")){	// 1 -> 12
		for(i=1 ; i<=12 ; i++){	
			int j;
			if(i == 12)     j = 0;
			else		j = i;
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",j, j == end_hour ? "selected" : "", i);	
		}
	}
	else if(!strcmp(type,"end_min_5")){	// 0 5 10 15 20 25 30 35 40 45 50 55
		for(i=0 ; i<12 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*5, i*5 == end_min ? "selected" : "", i*5);	
		}
	}
	else if(!strcmp(type,"end_time_am")){
		ret = websWrite(wp,"%s",end_time == 1 ? "" : "selected");	
	}
	else if(!strcmp(type,"end_time_pm")){
		ret = websWrite(wp,"%s",end_time == 1 ? "selected" : "");	
	}

	return ret;
}	

/*  Format: url0, url1, url2, url3, ....
 *	    keywd0, keywd1, keywd2, keywd3, keywd4, keywd5, ....
 */
int
ej_filter_web_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int ret = 0;
	int which;
	char *token = "<&nbsp;>";

        if (ejArgs(argc, argv, "%s %d", &type, &which) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d)\n",__FUNCTION__, type, which);
#endif
	
	if(!strcmp(type, "host")){
		char *host_data, filter_host[] = "filter_web_hostXXX";
		char host[80];
		
 		snprintf(filter_host, sizeof(filter_host), "filter_web_host%s", nvram_safe_get("filter_id"));
		host_data = nvram_safe_get(filter_host);
		if(!strcmp(host_data, ""))	return -1;	// no data
		find_each(host, sizeof(host), host_data, token, which, "");
		ret = websWrite(wp, "%s", host);
	}
	else if(!strcmp(type, "url")){
		char *url_data, filter_url[] = "filter_web_urlXXX";
		char url[80];
		
 		snprintf(filter_url, sizeof(filter_url), "filter_web_url%s", nvram_safe_get("filter_id"));
		url_data = nvram_safe_get(filter_url);
		if(!strcmp(url_data,""))	return -1;	// no data
		find_each(url, sizeof(url), url_data, token, which, "");
		ret = websWrite(wp, "%s", url);
	}

	return ret;
}

int
ej_filter_summary_show(int eid, webs_t wp, int argc, char_t **argv)
{
	int i,ret;
#if LANGUAGE == JAPANESE
	char w[7][10] = { "��","��","��","��","��","��","�y"}
	char week_d[7][10] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	char week_s[7][10] = { "���j","���j","�Ηj","���j","�ؗj","���j","�y�j"}
	char am[] ="�ߑO";
	char pm[] ="�ߌ�";
	char _24h[] ="24 ����";
	char everyday[] ="����";
#elif LANGUAGE == KOREAN && OEM == REENET	// <remove the line>
	char w[7][10] = { "��","��","ȭ","��","��","��","��"};	// <remove the line>
	char week_d[7][10] = { "��","��","ȭ","��","��","��","��"};	// <remove the line>
	char am[] ="����";	// <remove the line>
	char pm[] ="����";	// <remove the line>
	char _24h[] ="����";	// <remove the line>
	char everyday[] ="24�ð�";	// <remove the line>
#else
	#ifdef MULTILANG_SUPPORT
		//Dom 2005/11/14 add to js but websWrite too long will fail.
		char *w[] = { "0","1","2","3","4","5","6" };
		char *week_s[] = { "suns","mon","tue","wed","thu","fri","sat" };
		char _24h[] ="<script>Capture(accinetacc.hour24)</script>.";
		char everyday[] ="<script>Capture(accinetacc.everyday)</script>";
	#else
		char w[7][2] = { "S","M","T","W","T","F","S"};
		char week_s[7][10] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
		char _24h[] ="24 Hours.";
		char everyday[] ="Everyday";
	#endif
		char week_d[7][10] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
		char am[] ="AM";
		char pm[] ="PM";
#endif

	for(i=0 ; i<10 ; i++){
     		char name[50]="---";
     		char status[5]="---";
		char filter[] = "filter_ruleXXX";
		char *data = "";
		char status_buf[50]="---";
		char day_buf[100]="---";
		char time_buf[50]="---";
 		snprintf(filter, sizeof(filter), "filter_rule%d", i+1);
		data = nvram_safe_get(filter);
		if(data && strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","&nbsp;");	// get name value
			find_match_pattern(status, sizeof(status), data, "$STAT:","---");	// get name value
		}

		filter_tod_init(i+1);

ret = websWrite(wp," \
	<tr bgcolor=cccccc align=middle>\n\
        <td width=50><font face=Arial size=2>%d.</font></td>\n\
        <td width=200><font face=Arial size=2>%s</font></td>\n",i+1,name);
#if OEM == LINKSYS
	ret = websWrite(wp,
		"\n<td height=30 width=150 bordercolor=#000000 bgcolor=#CCCCCC> \n\
		<table border=1 cellspacing=1 bordercolor=#000000 style=\"border-collapse: collapse\" width=124 bgcolor=#FFFFFF>\n<tr>\n" );
	ret = websWrite(wp,"\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		<td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\"><script>Capture(share.day%s)</script></td>\n\
		</tr>\n</table></td>",
		tod_data_null == 0 && (day_all == 1 || week0 == 1) ? "#C0C0C0" : "#FFFFFF",w[0],
		tod_data_null == 0 && (day_all == 1 || week1 == 1) ? "#C0C0C0" : "#FFFFFF",w[1],
		tod_data_null == 0 && (day_all == 1 || week2 == 1) ? "#C0C0C0" : "#FFFFFF",w[2],
		tod_data_null == 0 && (day_all == 1 || week3 == 1) ? "#C0C0C0" : "#FFFFFF",w[3],
		tod_data_null == 0 && (day_all == 1 || week4 == 1) ? "#C0C0C0" : "#FFFFFF",w[4],
		tod_data_null == 0 && (day_all == 1 || week5 == 1) ? "#C0C0C0" : "#FFFFFF",w[5],
		tod_data_null == 0 && (day_all == 1 || week6 == 1) ? "#C0C0C0" : "#FFFFFF",w[6]
	);
	if(tod_data_null == 0){
		if(time_all == 1)
			strcpy(time_buf, _24h);
		else{
			snprintf(time_buf,sizeof(time_buf),"%02d:%02d %s - %02d:%02d %s",
				start_hour == 0 ? 12 : start_hour,
				start_min,
				start_time == 0 ? am : pm,
				end_hour == 0 ? 12 : end_hour,
				end_min,
				end_time == 0 ? am : pm);
		}
	}
#elif OEM == PCI || OEM == ELSA
	//if(!day_all){
	if(tod_data_null == 0){
		if(day_all == 1)
			strcpy(day_buf, everyday);
		else{
		#ifdef MULTILANG_SUPPORT
			snprintf(day_buf,sizeof(day_buf),
				 "<script>Capture(accinetacc.%s)</script> - <script>Capture(accinetacc.%s)</script>",week_s[start_week], week_s[end_week]);
		#else
			snprintf(day_buf,sizeof(day_buf),"%s - %s",week_s[start_week], week_s[end_week]);
		#endif
		}
#if LANGUAGE == ENGLISH	// <remove the line>
		snprintf(status_buf,sizeof(status_buf),"%s",atoi(status)==1 ? "Deny" : "Allow");
#else	// <remove the line>
		snprintf(status_buf,sizeof(status_buf),"%s",atoi(status)==1 ? "����" : "����");
#endif	// <remove the line>
	}
ret = websWrite(wp,"<td width=80><font face=Arial size=2>%s</font></td>\n",status_buf);
ret = websWrite(wp,"<td width=124><font face=Arial size=2>%s</font></td>\n",day_buf);
	if(tod_data_null == 0){
		if(time_all == 1)
			strcpy(time_buf, _24h);
		else
			snprintf(time_buf,sizeof(time_buf),"%02d:%02d - %02d:%02d",start_hour+start_time*12, start_min,
									   end_hour+end_time*12, end_min);
	}
#endif
ret = websWrite(wp," \
        <td width=150><font face=Arial size=2> %s </font> </td>\n\
        <td width=70><input type=checkbox name=sum%d value=1 ></td>\n\
      </tr>\n",time_buf, i+1);
	}
	return ret;
	
}

int
ej_filter_init(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *f_id = websGetVar(wp, "f_id", NULL);	

	if(f_id)	// for first time enter this page and don't press apply.
		nvram_set("filter_id", f_id);
	else
		nvram_set("filter_id", "1");

	return ret;
}

int
ej_filter_port_services_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *type;
	int which;
	char word[1024], *next, *services;
	char delim[] = "<&nbsp;>";
	
        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(!strcmp(type, "all_list")){
		int count = 0;
		services = nvram_safe_get("filter_services");
		split(word, services, next, delim) {
			int len = 0;
			char *name, *prot, *port;
			char protocol[100], ports[100];
			int from = 0, to = 0;
			//int proto;
			
			if ((name=strstr(word, "$NAME:")) == NULL ||
			    (prot=strstr(word, "$PROT:")) == NULL ||
			    (port=strstr(word, "$PORT:")) == NULL) 
			    continue;

			/* $NAME */
			if (sscanf(name, "$NAME:%3d:", &len) != 1) 
			    continue;
			strncpy(name, name + sizeof("$NAME:nnn:") - 1, len);
                        name[len] = '\0';
	
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
			if (sscanf(ports, "%d:%d", &from, &to) != 2)
				continue;

			//cprintf("match:: name=%s, protocol=%s, ports=%s\n", 
			//	word, protocol, ports);
			
			ret = websWrite(wp, "services[%d]=new service(%d, \"%s\", %d, %d, %d);\n",
					    count, count, name, from, to, protocol_to_num(protocol));
			count ++;
			    
		}
				    
		ret += websWrite(wp, "services_length = %d;\n", count);
	}
	else if (!strcmp(type, "service")){
		char *port_data, filter_port[] = "filter_port_grpXXX";
		char name[80];
		
 		snprintf(filter_port, sizeof(filter_port), "filter_port_grp%s", nvram_safe_get("filter_id"));
		port_data = nvram_safe_get(filter_port);
		if(!strcmp(port_data, ""))	return -1;	// no data
		find_each(name, sizeof(name), port_data, "<&nbsp;>", which, "");
		ret += websWrite(wp, "%s", name);
		
	}
	
	return ret;
}

/* Get the value of the checkbox for FilterIPMAC.asp. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
int
ej_filter_checkbox_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int filter_id, ret=0;
	char filter_lan_checkbox[] = "filter_lan_checkboxXXX";
	char filter_wan_checkbox[] = "filter_wan_checkboxXXX";

	if (ejArgs(argc, argv, "%s", &type) < 1) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	}

	filter_id = atoi(nvram_safe_get("filter_id"));

	if(!strcmp(type,"lan"))
	{
		snprintf(filter_lan_checkbox, sizeof(filter_lan_checkbox), "filter_lan_checkbox%d", filter_id);
		ret = websWrite(wp,"%s",atoi(nvram_safe_get(filter_lan_checkbox))? "checked" : "");	
	}

	if(!strcmp(type,"wan"))
	{
		snprintf(filter_wan_checkbox, sizeof(filter_wan_checkbox), "filter_wan_checkbox%d", filter_id);
		ret = websWrite(wp,"%s",atoi(nvram_safe_get(filter_wan_checkbox))? "checked" : "");	
	}
	return ret;
}	
#endif

/* Get the IP Address of the WAN Hosts for FilterIPMAC.asp. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
char
*filter_wan_group_ip_get(int which)
{
	static char word[256];
	char *wordlist, *next, *ip;
	char filter_wan_ip_grp[] = "filter_wan_ip_grpXXX";

	snprintf(filter_wan_ip_grp, sizeof(filter_wan_ip_grp), "filter_wan_ip_grp%s", nvram_safe_get("filter_id"));

	wordlist = nvram_safe_get(filter_wan_ip_grp);

	if(!wordlist) 
		return "0.0.0.0";

	foreach(word, wordlist, next)
	{
		if(which-- == 0)
		{
			ip = word;
			return ip;
		}
	}
	return "0.0.0.0";
}
#endif

/* Call filter_wan_group_ip_get function. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
int
ej_filter_wan_group_ip_node_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *ip;
	int ret=0, group, node, ip_node;

	if (ejArgs(argc, argv, "%d %d", &group, &node) < 1) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	}

	ip = filter_wan_group_ip_get(group);

	ip_node = get_single_ip(ip, node);

	ret = websWrite(wp,"%d",ip_node);	

	return ret;
}
#endif

/* Get the IP Range of the WAN Hosts for FilterIPMAC.asp. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
char
*filter_wan_range_ip_get(int which)
{
	static char word[256];
	char *wordlist, *next, *range;
	char filter_wan_ip_range[] = "filter_wan_ip_rangeXXX";

	snprintf(filter_wan_ip_range, sizeof(filter_wan_ip_range), "filter_wan_ip_range%s", nvram_safe_get("filter_id"));

	wordlist = nvram_safe_get(filter_wan_ip_range);

	if(!wordlist) 
		return "0.0.0.0.0";

	foreach(word, wordlist, next)
	{
		if(which-- == 0)
		{
			range = word;
			return range;
		}
	}
	return "0.0.0.0.0";
}
#endif

/* Call filter_wan_range_ip_get function. da_f@2005.1.29 */
#ifdef FILTER_WAN_SELECT_SUPPORT
int
ej_filter_wan_range_ip_node_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *range;
	int i_range[5] = {0, 0, 0, 0, 0};
	int ret=0, range_seq, node_seq, range_node;

	if (ejArgs(argc, argv, "%d %d", &range_seq, &node_seq) < 1) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	}

	range = filter_wan_range_ip_get(range_seq);

	sscanf(range, "%d.%d.%d.%d.%d", &i_range[0],&i_range[1],&i_range[2], &i_range[3],&i_range[4]);
	range_node = i_range[node_seq];

	ret = websWrite(wp,"%d",range_node);	

	return ret;
}
#endif

/* da_f 2005.3.18 add */
#ifdef NOMAD_SUPPORT

/* zb 2005.3.29 add. start ... */
#define IP0 10
#define IP3 1
int create_random_ip(char * ipstr)
{
	int i;
	int ip[2];
	for (i = 0; i < 2; i++)
	{
		ip[i] = random_seed(256, (i+3));
	}
	sprintf(ipstr,"%d.%d.%d.%d",IP0,ip[0],ip[1],IP3);
	printf("ipstr=%s\n",ipstr);
	return 1;
}

int chang_lanip()
{
	char lanip[18];
	/*eval("ifconfig","br0","down");*/
	memset(lanip,0,sizeof(lanip));
	create_random_ip(lanip);
	printf("lanip=%s\n",lanip);	
	nvram_set("lan_ipaddr",lanip);
	nvram_set("lan_netmask","255.255.255.0");
	nvram_commit();
	return 0;
}
/* zb 2005.3.29 end */

int reboot_by_vpn(webs_t wp)
{
	if(atoi(websGetVar(wp, "change_lan_ip", "0")) == 1) 
			return 1;  /* need reboot */

	return 0; /* do nothing */
}

/* Ckeck that if the user name has existed. */
int check_user_name(const char *u_name, int cur_id)
{
	int i;
	char username[32], var_name[32] = "vpn_account_x";
	char *p_account = NULL;

	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		if(i == cur_id) /* do not compare with itself */
			continue;

		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_account_%d", i);
		p_account = nvram_safe_get(var_name);

		bzero(username, sizeof(username));
		sscanf(p_account, "%32[^ ] ", username);
		if(!strncmp(u_name, username, sizeof(username)))
			return 1;
	}	
	return 0;
}

/* Check that if there is white space, ... */ 
int check_input_str(const char *input)
{
	if(index(input, 0x20)) /* white space */
		return 1;
	
	return 0;
}

/* Add or modify a VPN Client Account. */
void 
validate_vpn_account(webs_t wp, char *value, struct variable *v)
{
	int id = 0;
	char account[128], var_name[32] = "vpn_account_x";
	struct VPNAccount vpn_account;

	id = atoi(websGetVar(wp, "vpn_account_id", ""));
	if(0 == id) /* want to add VPN User Account */
	{
		char username[32];
		char *p_account = NULL;
		int i;

		for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
		{
			bzero(var_name, sizeof(var_name));
			sprintf(var_name, "vpn_account_%d", i);
			p_account = nvram_safe_get(var_name);

			bzero(username, sizeof(username));
			sscanf(p_account, "%32[^ ] ", username);
			if(!strcmp(username, DEFAULT_VPN_USERNAME))
			{
				id = i;
				break;
			}
		}	

		if(0 == id)
			return;
	}
	else /* want to modify VPN User Account */
	{
		if(id <= 0 || id > MAX_VPN_ACCOUNT)
			return;

		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_account_%d", id);
	}

	bzero(&vpn_account, sizeof(vpn_account));
	memcpy(vpn_account.username, 
						websGetVar(wp, "vpn_username", DEFAULT_VPN_USERNAME), 
						sizeof(vpn_account.username));
	if(check_input_str(vpn_account.username))
		return;
	if(check_user_name(vpn_account.username, id))
		return;

	memcpy(vpn_account.password, 
						websGetVar(wp, "vpn_password", DEFAULT_VPN_PASSWD), 
											sizeof(vpn_account.password));
	if(check_input_str(vpn_account.password))
		return;
	memcpy(vpn_account.change_enable, 
								websGetVar(wp, "passwd_change_enable", "0"), 
								sizeof(vpn_account.change_enable));
	if(check_input_str(vpn_account.change_enable))
		return;
	vpn_account.active_flag[0] = '1';

	bzero(account, sizeof(account));
	snprintf(account, sizeof(account), "%s %s %s %s", vpn_account.username,
												vpn_account.password,
												vpn_account.change_enable, 
												vpn_account.active_flag);

	nvram_set(var_name, account);

	/* change LAN IP */
	if(atoi(websGetVar(wp, "change_lan_ip", "0")) == 1) 
	{
		/*
		char *lan_ip = nvram_safe_get("lan_ipaddr");

		if(!strncmp(lan_ip, "192.168.1", strlen("192.168.1")))*/
			chang_lanip();
	}
	
	return;
}

/* Save active status of the VPN Client Account. */
void 
validate_vpn_cli_active(webs_t wp, char *value, struct variable *v)
{
	int i, ckbox_value;
	char ckbox_name[32], account[128];
	char var_name[32] = "vpn_account_x";
	char *p_account = NULL;
	struct VPNAccount vpn_account;


	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		bzero(ckbox_name, sizeof(ckbox_name));
		sprintf(ckbox_name, "vpn_cli_active_%d", i);
		ckbox_value = atoi(websGetVar(wp, ckbox_name, "0"));

		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_account_%d", i);
		p_account = nvram_safe_get(var_name);

		bzero(&vpn_account, sizeof(vpn_account));
		sscanf(p_account, "%32[^ ] %32[^ ] %2[^ ] %2s", vpn_account.username, 
													vpn_account.password, 
													vpn_account.change_enable, 
													vpn_account.active_flag);

		/* If user does not want to change the active status, do nothing */
		if(ckbox_value == atoi(vpn_account.active_flag))
			continue;

		bzero(account, sizeof(account));
		snprintf(account, sizeof(account), "%s %s %s %d", vpn_account.username,
													vpn_account.password,
													vpn_account.change_enable, 
													ckbox_value);
		nvram_set(var_name, account);
	}	
	return;
}

/* Disconnect */
void 
validate_vpn_cli_disconn(webs_t wp, char *value, struct variable *v)
{
	int i, ckbox_value;
	char ckbox_name[32], status[128];
	char var_name[32] = "vpn_client_stat_x";
	char *p_status = NULL;
	struct VPNCliStat vpn_cli_stat;


	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		bzero(ckbox_name, sizeof(ckbox_name));
		sprintf(ckbox_name, "vpn_cli_disconn_%d", i);
		ckbox_value = atoi(websGetVar(wp, ckbox_name, "0"));

		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_client_stat_%d", i);
		p_status = nvram_safe_get(var_name);

		bzero(&vpn_cli_stat, sizeof(vpn_cli_stat));
		sscanf(p_status, "%2[^ ] %32[^ ] %s", vpn_cli_stat.online, 
												vpn_cli_stat.s_time, 
												vpn_cli_stat.e_time);

		/* If user does not want to disconnect, do nothing */
		if(0 == ckbox_value)
			continue;

		bzero(status, sizeof(status));
		snprintf(status, sizeof(status), "0 %s %s", vpn_cli_stat.s_time, 
													vpn_cli_stat.e_time);
		nvram_set(var_name, status);

		/* disconnect */
		gui_stopnomad_conn(i);
	}	
	return;
}

/* Remove a VPN Client Account. */
void 
rm_vpn_cli_account(webs_t wp, char *value, struct variable *v)
{
	int id = 0;
	char account[128], acc_status[128];
	char var_name[32] = "vpn_account_x", 
		var_stat_name[32] = "vpn_client_stat_x";

	id = atoi(websGetVar(wp, "vpn_account_id", ""));
	if(id <= 0 || id > MAX_VPN_ACCOUNT)
		return;

	sprintf(var_name, "vpn_account_%d", id);
	bzero(account, sizeof(account));
	snprintf(account, sizeof(account), "%s %s %d %d", DEFAULT_VPN_USERNAME,
													DEFAULT_VPN_PASSWD,
													0, 0);

	/* Should set the status of the VPN Client to default value */
	sprintf(var_stat_name, "vpn_client_stat_%d", id);
	bzero(acc_status, sizeof(acc_status));
	snprintf(acc_status, sizeof(acc_status), "%d %s %s",  0, "--", "--");

	nvram_set(var_name, account);
	nvram_set(var_stat_name, acc_status); /* status of the VPN Client */
	nvram_commit();

	return;
}

/* Display VPN Client Account table list. */
int 
ej_vpn_account_list_get(int eid, webs_t wp, int argc, char_t **argv)
{	
#define INPUT_STYLE " style=\"BORDER-RIGHT: 0px; BORDER-TOP: 0px; FONT-SIZE: 8pt; BORDER-LEFT: 0px; BORDER-BOTTOM: 0px; TEXT-ALIGN: center\" "

	int flag = 0, i, ret;
	char var_name[32] = "vpn_account_x";
	char *p_account = NULL;
	struct VPNAccount vpn_account;

	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_account_%d", i);
		p_account = nvram_safe_get(var_name);

		bzero(&vpn_account, sizeof(vpn_account));
		sscanf(p_account, "%32[^ ] %32[^ ] %2[^ ] %2s", vpn_account.username, 
												vpn_account.password, 
												vpn_account.change_enable, 
												vpn_account.active_flag);

		flag = strcmp(vpn_account.username, DEFAULT_VPN_USERNAME);

		ret = websWrite(wp, 
"<TR align=center  height=30>
	<TD class=num>%d</TD>
	<TD><input type=checkbox name=vpn_cli_active_%d value=1 %s %s></TD>
	<TD><input %s readOnly size=12 name=vpn_username_%d value=\"%s\" disabled></TD>
	<input type=hidden readOnly size=12 name=vpn_password_%d value=\"%s\" disabled>
	<TD>%s</TD>
	<input type=hidden readOnly size=12 name=vpn_passwd_change_%d value=\"%s\" disabled>
   	<TD>
   		<input style=\"FONT-SIZE: 8pt\" type=button %s name=account_edit_%d value='Edit' onClick='EditUser(%d)'>&nbsp;
	   	<input style=\"FONT-SIZE: 8pt\" type=button %s name=account_remove_%d value='Remove' onClick='RemoveUser(%d)'> 
	</TD>
</TR>", i, 
		i, atoi(vpn_account.active_flag)?"checked":"", flag?"":"disabled", 
		INPUT_STYLE, i, flag?(vpn_account.username):"",   
		i, flag?(vpn_account.password):"", 
		flag?"******":"", 
		i, flag?(vpn_account.change_enable):"",  
		flag?"":"disabled", i, i, 
		flag?"":"disabled", i, i);
	}
	return ret;

#undef INPUT_STYLE
}

/* Display detail information of a VPN Client Account. */
#if 0
int 
ej_vpn_account_get(int eid, webs_t wp, int argc, char_t **argv)
{	
	int userid, ret = 0;
	char var_name[32] = "vpn_account_x";
	char *p_account = NULL;
	struct VPNAccount vpn_account;

	userid = atoi(nvram_safe_get("cur_vpn_account"));

	if(userid <= 0 || userid > MAX_VPN_ACCOUNT)
	{
		/*
		ret = websWrite(wp, 
		   "F.vpn_username.value = '';
			F.vpn_password.value = '';
			F.vpn_password_confirm.value = '';
			F.passwd_change_enable[0].checked = '';
			F.passwd_change_enable[1].checked = '';");*/
		return ret;
	}
	
	bzero(var_name, sizeof(var_name));
	sprintf(var_name, "vpn_account_%d", userid);
	p_account = nvram_safe_get(var_name);

	bzero(&vpn_account, sizeof(vpn_account));
	sscanf(p_account, "%32[^ ] %32[^ ] %2[^ ] %2s", vpn_account.username, 
												vpn_account.password, 
												vpn_account.change_enable, 
												vpn_account.active_flag);

	ret = websWrite(wp, 
	   "F.vpn_username.value = '%s';
		F.vpn_password.value = '%s';
		F.vpn_password_confirm.value = '%s';
		F.passwd_change_enable[0].checked = '%s';
		F.passwd_change_enable[1].checked = '%s';", 
		vpn_account.username, 
		vpn_account.password, 
		vpn_account.password, 
		strcmp(vpn_account.change_enable, "1")?"":"true", 
		strcmp(vpn_account.change_enable, "0")?"":"true");

	return ret;
}
#endif

/* Display User Name of all VPN Client Accounts. */
int
ej_vpn_usernames_get(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret;
	char username[32], all_names[256], var_name[32] = "vpn_account_x";
	char *p_account = NULL;

	bzero(all_names, sizeof(all_names));
	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		bzero(var_name, sizeof(var_name));
		sprintf(var_name, "vpn_account_%d", i);
		p_account = nvram_safe_get(var_name);

		bzero(username, sizeof(username));
		sscanf(p_account, "%32[^ ] ", username);

		if(i != 1)
		{
			strcat(all_names, " ");
		}
		strcat(all_names, username);
	}	

	ret = websWrite(wp, "%s", all_names);
	return ret;
}

/* eg. s_time: 16:32, e_time: 17:33 */
int cal_duration(const char *s_time, const char *e_time, int stat, char *res)
{
	int s_hr, s_min, s_mins, e_hr, e_min, e_mins, res_mins;

	if(NULL == s_time || NULL == e_time || NULL == res)
		return 0;

	sscanf(s_time, "%d:%d", &s_hr, &s_min);
	if(s_hr > 23 || s_hr < 0 || s_min > 59 || s_min < 0)
		return 0;

	if(stat) /* VPN Client is online */
	{
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		
		e_hr = tm->tm_hour;
		e_min = tm->tm_min;
	}
	else /* offline */
	{
		sscanf(e_time, "%d:%d", &e_hr, &e_min);
		if(e_hr > 23 || e_hr < 0 || e_min > 59 || e_min < 0)
			return 0;
	}

	s_mins = s_hr*60 + s_min;
	e_mins = e_hr*60 + e_min;
	if(s_mins > e_mins)
		res_mins = (e_mins + 24*60) - s_mins;
	else
		res_mins = e_mins - s_mins;

	sprintf(res, "%dh%dm", res_mins/60, res_mins%60);

	return 1;
}

/* Display status of all VPN Client Accounts. */
int 
ej_vpn_clients_status(int eid, webs_t wp, int argc, char_t **argv)
{	
	int flag = 0, i, ret;
	char ac_name[32] = "vpn_account_x", stat_name[32] = "vpn_client_stat_x";
	char *p_account = NULL;
	struct VPNAccount vpn_account;
	struct VPNCliStat vpn_cli_stat;

	for(i = 1; i <= MAX_VPN_ACCOUNT; i++)
	{
		bzero(ac_name, sizeof(ac_name));
		sprintf(ac_name, "vpn_account_%d", i);
		p_account = nvram_safe_get(ac_name);

		bzero(&vpn_account, sizeof(vpn_account));
		sscanf(p_account, "%32[^ ] %32[^ ] %2[^ ] %2s", vpn_account.username, 
												vpn_account.password, 
												vpn_account.change_enable, 
												vpn_account.active_flag);

		flag = strcmp(vpn_account.username, DEFAULT_VPN_USERNAME);

		bzero(stat_name, sizeof(stat_name));
		sprintf(stat_name, "vpn_client_stat_%d", i);
		p_account = nvram_safe_get(stat_name);

		bzero(&vpn_cli_stat, sizeof(vpn_cli_stat));
		sscanf(p_account, "%2[^ ] %32[^ ] %s", vpn_cli_stat.online, 
												vpn_cli_stat.s_time, 
												vpn_cli_stat.e_time);
		if(flag == 0) /* it is default username, so do not display */
			ret = websWrite(wp, 
				"<TR align=center  height=30>
					<TD class=num>%d</TD>
					<TD></TD>
					<TD></TD>
					<TD></TD>
					<TD></TD>
					<TD></TD>
		<TD><input type=checkbox name=vpn_cli_disconn_%d value=1 disabled></TD>
				</TR>", i, i);  
		else
		{
			int cal_ret, flag_online = atoi(vpn_cli_stat.online);
			char duration[32];

			bzero(duration, sizeof(duration));
			cal_ret  = cal_duration(vpn_cli_stat.s_time,
									vpn_cli_stat.e_time,
									flag_online, 
									duration);

			ret = websWrite(wp, 
				"<TR align=center  height=30>
					<TD class=num>%d</TD>
					<TD>%s</TD>
					<TD><FONT color=%s>%s</FONT></TD>
					<TD>%s</TD>
					<TD>%s</TD>
					<TD>%s</TD>
		<TD><input type=checkbox name=vpn_cli_disconn_%d value=1 %s %s></TD>
				</TR>", 
				i,
				vpn_account.username, 
				flag_online?"green":"red", flag_online?"online":"offline", 
				vpn_cli_stat.s_time, 
				flag_online?"_:_":vpn_cli_stat.e_time, 
				cal_ret?duration:"", 
				i, flag_online?"":"disabled");  
		}
	}
	return ret;
}

/* For vpn_accout.asp */
#if 0
void
do_vpn_ej(char *url, webs_t stream)
{
	char *path, *query, *ptr_id, str_id[2] = "\0\0";
	int id;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): url=[%s]",__FUNCTION__,url);
#endif

	query = url;
	/* eg. vpn_account.asp?id=1 */ 
	ptr_id = index(url, '=') + 1;
	while(ptr_id) /* skip the white space */
	{
		if(0x20 == *ptr_id)
			ptr_id++;  
		else
			break;
	}

	if(NULL == ptr_id)
		return;

	id  = atoi(ptr_id);
	if(id <= 0 || id > MAX_VPN_ACCOUNT)
		nvram_set("cur_vpn_account", "0");
	else
	{
		sprintf(str_id, "%d", id);
		nvram_set("cur_vpn_account", str_id);
	}

	path = strsep(&query, "?") ? : url;

	if(!query)
		return;	

	do_ej(path, stream);
}
#endif
#endif
