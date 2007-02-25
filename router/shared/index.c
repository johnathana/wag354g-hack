
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
#include <unistd.h>

#include <broadcom.h>
#include <cymac.h>

#include <cy_conf.h>
#include "pvc.h"


/* da_f 2005.4.6 add. start... */
//#ifdef NETMASK_CLASS_SUPPORT
enum lan_type
{
	FIR_LAN,
	SEC_LAN
};
//#endif
/* da_f 2005.4.6 end */

int clone_wan_mac;

extern int connchanged;

void validate_trap_ipaddr(webs_t wp, char *value, struct variable *v)
{
	  char trap_ipaddr[20];
          printf("the v->name=%s!!!!!!\n",v->name); 
      	  get_merge_ipaddr(v->name, trap_ipaddr);
          //if(!valid_ipaddr(wp,trap_ipaddr,v))
            //    return;
	  printf("write the trap_ipaddr value!!!!!\n");
          nvram_set(v->name,trap_ipaddr);
}

//junzhao
int
ej_show_index_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *wan_encapmode;
	if(!gozila_action || connchanged)
	{
		//connchanged = 0;
		wan_encapmode = encapmode;
	}
	else
		wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
	
	if(!strcmp(wan_encapmode, "1483bridged")) 
		do_ej("index_1483bridged.asp",wp);
	else if(!strcmp(wan_encapmode, "routed")) 
		do_ej("index_routed.asp",wp);
#ifdef CLIP_SUPPORT
	else if(!strcmp(wan_encapmode, "clip")) 
		do_ej("index_clip.asp",wp);
#endif
	else if(!strcmp(wan_encapmode, "pppoa")) 
		do_ej("index_pppoa.asp",wp);
	else if(!strcmp(wan_encapmode,"pppoe"))
/*#ifdef MPPPOE_SUPPORT
		do_ej("index_mpppoe.asp",wp);
#else*/
		do_ej("index_pppoe.asp",wp);
//#endif
	else if(!strcmp(wan_encapmode,"pptp"))
		do_ej("index_pptp.asp",wp);
	/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
       else if(!strcmp(wan_encapmode,"unpppoe")) 	   
	   	do_ej("index_unip_pppoe.asp",wp);
	else if(!strcmp(wan_encapmode,"unpppoa")) 	   
	   	do_ej("index_unip_pppoa.asp",wp);    
#endif

	return ret;
}

//junzhao 2004.4.22 dhcprelay
void
validate_dhcpserver_ipaddr(webs_t wp, char *value, struct variable *v)
{
	char dhcpserver_ipaddr[20];
	get_merge_ipaddr(v->name, dhcpserver_ipaddr);
	if(!valid_ipaddr(wp, dhcpserver_ipaddr, v))	
		return;
	nvram_set(v->name, dhcpserver_ipaddr);
	return;
}

//#ifdef NETMASK_CLASS_SUPPORT
/* da_f 2005.3.16 add.
 * Save the lan netmask.
 * @type: 0, first lan; 1, second lan */
void
save_lan_netmask(webs_t wp, char *value, struct variable *v, int type)
{
	int j =0, ip_node_num = 4;
	char ip[32], ip_node_name[32], ip_node[8], var_name[32];
	struct variable *which;
   	struct variable	ip_variables[] = { { longname: "TCP/UDP IP address", argv: ARGV("0", "255") }, };

	bzero(ip, sizeof(ip));
	which = &ip_variables[0];

	for(; j < ip_node_num; j++)
	{
		char *p_ip_node = NULL;

		bzero(ip_node_name, sizeof(ip_node_name));
		bzero(ip_node, sizeof(ip_node));

		if(FIR_LAN == type)
			snprintf(ip_node_name, sizeof(ip_node_name), "lan_netmask_%d", j);
		else if(SEC_LAN == type)
			snprintf(ip_node_name, sizeof(ip_node_name), "seclan_netmask_%d",j);
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

	bzero(var_name, sizeof(var_name));
	sprintf(var_name, "%s", type?"seclan_netmask":"lan_netmask");

	/* da_f 2005.3.16 modify */
	if(FIR_LAN == type && strcmp(nvram_safe_get(var_name), ip))
		unlink("/tmp/udhcpd.leases"); 

	nvram_set(var_name, ip);
}
//#endif

void
validate_lan_ipaddr(webs_t wp, char *value, struct variable *v)
{
	char lan_ipaddr[20];

#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
    char *lan_netmask;
	struct variable lan_variables[] = {
		{ longname: "LAN Subnet Mask", argv: ARGV("255.255.255.0","255.255.255.128","255.255.255.192","255.255.255.224","255.255.255.240","255.255.255.248","255.255.255.252") },
	}, *which;

	which = &lan_variables[0];

	lan_netmask = websGetVar(wp, "lan_netmask", NULL);
	if(!lan_netmask)	return;
#endif

	get_merge_ipaddr(v->name, lan_ipaddr);

	if(!valid_ipaddr(wp, lan_ipaddr, v))	
		return;

#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	if(!valid_choice(wp, lan_netmask, &which[0]))
		return;
#endif

	if(strcmp(nvram_safe_get("lan_ipaddr"),lan_ipaddr))	
		unlink("/tmp/udhcpd.leases");

#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	if(strcmp(nvram_safe_get("lan_netmask"),lan_netmask))	
		unlink("/tmp/udhcpd.leases");
#endif

	nvram_set(v->name, lan_ipaddr);

#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	nvram_set("lan_netmask", lan_netmask);
#else
	/* da_f 2005.3.16 add */
	save_lan_netmask(wp, value, v, FIR_LAN);
#endif
}

#ifdef SECLAN_SUPPORT
/* kirby add for FFT */
void validate_seclan_ipaddr(webs_t wp, char *value, struct variable *v)
{
	char lan_ipaddr[20];

#if 0
#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	char *lan_netmask;
	struct variable lan_variables[] = {
		{ longname: "Secondary LAN Subnet Mask", argv: ARGV("255.255.255.0","255.255.255.128","255.255.255.192","255.255.255.224","255.255.255.240","255.255.255.248","255.255.255.252") },
	}, *which;
#endif
#endif

	if(nvram_match("seclan_enable", "0")) return;
	   
#if 0
#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	which = &lan_variables[0];

	lan_netmask = websGetVar(wp, "seclan_netmask", NULL);
	
	printf("lan_netmask=%s\n",lan_netmask);

	if(!lan_netmask)	return;
#endif
#endif

	get_merge_ipaddr(v->name, lan_ipaddr);

	if(!valid_ipaddr(wp, lan_ipaddr, v))	
		return;
	
#if 0
#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	if(!valid_choice(wp, lan_netmask, &which[0]))
		return;
#endif
#endif


	nvram_set(v->name, lan_ipaddr);
#if 0
#ifndef NETMASK_CLASS_SUPPORT /* da_f 2005.4.18 modify */
	nvram_set("seclan_netmask", lan_netmask);
#else
	/* da_f 2005.4.6 add */
	save_lan_netmask(wp, value, v, SEC_LAN);
#endif
#else
	save_lan_netmask(wp, value, v, SEC_LAN);
#endif
}
/* kirby add */
#endif

void
validate_wan_ipaddr(webs_t wp, char *value, struct variable *v)
{
	char wan_ipaddr[20], wan_netmask[20], wan_gateway[20];
	char *wan_proto = websGetVar(wp, "wan_proto", NULL);

	struct variable wan_variables[] = {
		{ longname: "WAN IP Address", NULL },
		{ longname: "WAN Subnet Mask", NULL },
		{ longname: "WAN Gateway", argv: ARGV("wan_ipaddr","wan_netmask")},
	}, *which;

	which = &wan_variables[0];

	get_merge_ipaddr("wan_ipaddr", wan_ipaddr);
	get_merge_ipaddr("wan_netmask", wan_netmask);
	if(!strcmp(wan_proto,"pptp"))
		get_merge_ipaddr("pptp_server_ip", wan_gateway);
	else
		get_merge_ipaddr("wan_gateway", wan_gateway);

	if(!valid_ipaddr(wp, wan_ipaddr, &which[0]))
		return;	
	nvram_set("wan_ipaddr_buf",nvram_safe_get("wan_ipaddr"));
	nvram_set("wan_ipaddr", wan_ipaddr);

	if(!valid_netmask(wp, wan_netmask, &which[1]))	
		return;	
	nvram_set("wan_netmask", wan_netmask);

	if(!valid_ipaddr(wp, wan_gateway, &which[2]))	
		return;	

	if(!strcmp(wan_proto,"pptp"))
		nvram_set("pptp_server_ip", wan_gateway);
	else
		nvram_set("wan_gateway", wan_gateway);

}

int
ej_get_wl_max_channel(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

        ret = websWrite(wp, "%s", WL_MAX_CHANNEL);

	return ret;
}

int
ej_get_wl_domain(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

#if COUNTRY == EUROPE
	ret += websWrite(wp,"ETSI");
#elif COUNTRY == JAPAN 
	ret += websWrite(wp,"JP");
#elif COUNTRY == KOREA	// <remove the line>
	ret += websWrite(wp,"Korea");	// <remove the line>
#else
	ret += websWrite(wp,"US");
#endif

	return ret;
}

int 
clone_mac(webs_t wp)
{
	int ret = 0;

	clone_wan_mac = 1;

	return ret;
}

int
ej_get_clone_mac(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *c;
	int mac, which;

	if (ejArgs(argc, argv, "%d", &which) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(clone_wan_mac)
		c = nvram_safe_get("http_client_mac");
	else{
		if(nvram_match("def_hwaddr","00:00:00:00:00:00")){
			c = strdup(nvram_safe_get("et0macaddr"));
			MAC_ADD(c);
		}
		else
			c = nvram_safe_get("def_hwaddr");
	}

	if(c){
		mac = get_single_mac(c, which);
		ret += websWrite(wp, "%02X", mac);
	}
	else
		ret += websWrite(wp, "00");
		
	return ret;
}

int
macclone_onload(webs_t wp, char *arg)
{
	int ret = 0;

	if(clone_wan_mac)
		ret += websWrite(wp, arg);

	return ret;
}
