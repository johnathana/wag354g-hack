
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
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>

#include <broadcom.h>

//modifed by leijun 2004-03-07--22:40
/*
 * Example: 
 * static_route=192.168.2.0:255.255.255.0:192.168.1.2:1:br0
 * <% static_route("ipaddr", 0); %> produces "192.168.2.0"
 * <% static_route("lan", 0); %> produces "selected" if nvram_match("lan_ifname", "br0")
 */
int
ej_static_route_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int which, count;
	char word[256], *next, *page;
//	char name[50]="";
	char *ipaddr, *netmask, *gateway, *metric, *ifname;
	int ret = 0, temp;
//	char new_name[200];

	if (ejArgs(argc, argv, "%s %d", &arg, &count) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	page = websGetVar(wp, "route_page", NULL);
	if(!page)	page ="0";
	
	which = atoi(page);
	
	temp = which;

	foreach(word, nvram_safe_get("static_route"), next) {
		if (which-- == 0) {
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
/*			metric = gateway;
			gateway = strsep(&metric, ":");
			if (!gateway || !metric)
				continue;
			ifname = gateway;
			gateway = strsep(&ifname, ":");
			if (!ifname)
				continue;
*/
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): ip=[%s] mask=[%s] gw=[%s] metric=[%s] ifname=[%s]",__FUNCTION__,page,ipaddr,netmask,gateway,metric,ifname);
#endif
			if (!strcmp(arg, "ipaddr")){
				return websWrite(wp, "%d", get_single_ip(ipaddr,count));
			}
			else if (!strcmp(arg, "netmask")){
				return websWrite(wp, "%d", get_single_ip(netmask,count));
			}
			else if (!strcmp(arg, "gateway")){
				return websWrite(wp, "%d", get_single_ip(gateway,count) );
			}

			else if (!strcmp(arg, "metric"))
				return websWrite(wp, metric);
/*
			else if (!strcmp(arg, "lan") && !strcmp(ifname, "lan"))
				return websWrite(wp, "selected");
			else if (!strcmp(arg, "wan") && !strcmp(ifname, "wan"))
				return websWrite(wp, "selected");
*/
		}
	}

	if(!strcmp(arg, "ipaddr") || !strcmp(arg, "netmask") || !strcmp(arg, "gateway"))
		ret += websWrite(wp, "0");
	else if (!strcmp(arg, "metric"))
		ret +=  websWrite(wp, "1");

	return ret;
}
int entry_in_route_table(char *d_ipaddr, char *d_netmask, char *d_gateway)
{
	FILE *fp;
	int count = 0;
	int flgs, ref, use, metric;
    	unsigned long dest, gw, netmask;
    	struct in_addr dest_ip, gw_ip, netmask_ip;
	char sdest[16], sgw[16], snetmask[16], word[256];

	if ((fp = fopen("/proc/net/route", "r")) == NULL) 
	{
		printf("No route table\n");
    		return -1;
    	}
	
	while( fgets(word, sizeof(word), fp) != NULL ) 
	{
		if(count) 
		{	
			int ifl = 0;
			while(word[ifl]!=' ' && word[ifl]!='\t' && word[ifl]!='\0')	ifl++;
			word[ifl]=0;    /* interface */
			if(sscanf(word+ifl+1, "%lx%lx%X%d%d%d%lx", &dest, &gw, &flgs, &ref, &use, &metric, &netmask)!=7) 
			{
				return -1;
		    }  
			dest_ip.s_addr = dest;
			gw_ip.s_addr   = gw;
			netmask_ip.s_addr = netmask;

			strcpy(sdest,  		(dest_ip.s_addr==0 ? "0.0.0.0" : inet_ntoa(dest_ip)));    //default
			strcpy(sgw,    		(gw_ip.s_addr==0   ? "0.0.0.0" : inet_ntoa(gw_ip)));      //*
			strcpy(snetmask,    (netmask_ip.s_addr==0   ? "0.0.0.0" : inet_ntoa(netmask_ip)));
			
			if(!strcmp(sdest, d_ipaddr) && !strcmp(snetmask, d_netmask) && !strcmp(sgw, d_gateway)) 
			{
//				printf("find now \n");
				return 1;
			}
		}
		count++;
	}
	fclose(fp);
	return 0;
}
extern int static_route_added; //added by leijun
void
validate_static_route(webs_t wp, char *value, struct variable *v)
{
	//modified by leijun 2004-0331
	int i, modify_flag = 0;
	char word[256], *next;
	char buf[1000] = "";
	char old[STATIC_ROUTE_PAGE][60], tmp_entry[60];
	struct variable static_route_variables[] = {
		{ longname: "Route IP Address", argv: NULL },
		{ longname: "Route Subnet Mask", argv: NULL },
		{ longname: "Route Gateway", argv: NULL },
		{ longname: "Route Interface", argv: ARGV("lan", "wan") },
	};

	char /**name,*/ *ipaddr = NULL, *netmask = NULL, *gateway = NULL, *metric="0", *ifname, *page;
	char temp[30], *val=NULL;

//	printf("leijun in validate_static_route\n");
//	name = websGetVar(wp, "route_name", "");	// default empty if no find route_name
	static_route_added = 0;
		
	/* validate ip address */
	ipaddr = malloc(20);
    strcpy(ipaddr,"");
    for(i=0 ; i<4 ; i++)
    {
	    snprintf(temp,sizeof(temp),"%s_%d","route_ipaddr",i);
        val = websGetVar(wp, temp , NULL);
        if(val)
        {
        	strcat(ipaddr,val);
            if(i<3) strcat(ipaddr,".");
        }
        else goto _exit_validate_static_route;
    }

	/* validate netmask */
	netmask = malloc(20);
    strcpy(netmask,"");
    for(i=0 ; i<4 ; i++)
    {
    	snprintf(temp,sizeof(temp),"%s_%d","route_netmask",i);
    	val = websGetVar(wp, temp , NULL);
    	if(val)
    	{
        	strcat(netmask,val);
        	if(i<3) strcat(netmask,".");
        }else goto _exit_validate_static_route;
    }

	/* validate gateway */
	gateway = malloc(20);
    strcpy(gateway,"");
    for(i=0 ; i<4 ; i++)
    {
    	snprintf(temp,sizeof(temp),"%s_%d","route_gateway",i);
    	val = websGetVar(wp, temp , NULL);
    	if(val)
    	{
    		strcat(gateway,val);
            if(i<3) strcat(gateway,".");
        }else goto _exit_validate_static_route;
    }

	page = websGetVar(wp, "route_page", NULL);
	metric = websGetVar(wp, "route_metric", NULL);
//	ifname = websGetVar(wp, "route_ifname", NULL);

#ifdef MY_DEBUG
//	LOG(LOG_DEBUG,"%s(%s): ip=[%s] mask=[%s] gw=[%s] metric=[%s] ifname=[%s]",__FUNCTION__,page,ipaddr,netmask,gateway,metric,ifname);
#endif
		
	if (!page || !ipaddr || !netmask || !gateway || !metric)
		goto _exit_validate_static_route;

	if ((atoi(metric) < 1) || (atoi(metric) > 16) || !strcmp(ipaddr,"")) goto _exit_validate_static_route;
	
	if ((!strcmp(ipaddr,"0.0.0.0") || !strcmp(ipaddr,"")) &&
	    (!strcmp(netmask,"0.0.0.0") || !strcmp(netmask,"")) &&
	    (!strcmp(gateway,"0.0.0.0") || !strcmp(gateway,""))){
		goto _setting_value;
	}
/*	
	if (!valid_choice(wp, ifname, &static_route_variables[3]))
		goto _exit_validate_static_route;
*/	
	if (!*ipaddr) {
		websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
		goto _exit_validate_static_route;
	}
	if (!*netmask) {
		websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Subnet Mask<br>", v->longname);
		goto _exit_validate_static_route;
	}
/*
	if (!strcmp(ifname, "lan")) {
		ifname = nvram_safe_get("lan_ifname");
		//if(!strcmp(gateway,"0.0.0.0") || !strcmp(gateway,""))
			static_route_variables[2].argv = NULL;
		//else
		//	static_route_variables[2].argv = ARGV("lan_ipaddr", "lan_netmask");
	} else {
		ifname = nvram_safe_get("wan_ifname");
		static_route_variables[2].argv = NULL;
	}
*/
	if (!valid_ipaddr(wp, ipaddr, &static_route_variables[0]) ||
	    !valid_netmask(wp, netmask, &static_route_variables[1]) ||
	    !valid_ipaddr(wp, gateway, &static_route_variables[2]))
		goto _exit_validate_static_route;

	for(i=0 ; i<STATIC_ROUTE_PAGE ; i++)
		strcpy(old[i],"");
	strcpy(tmp_entry, "");	
	i=0;
	foreach(word, nvram_safe_get("static_route"), next) {
		strcpy(old[i ++],word);
	}	
	sprintf(tmp_entry, "%s:%s:%s", ipaddr, netmask, gateway);
	//judge if the entry is saved before
	for (i = 0; i < STATIC_ROUTE_PAGE; i ++)	
	{
		if (!strcmp(old[i], "$")) continue;
		if (i == atoi(page))
		{
			sprintf(tmp_entry, "%s:%s:%s:%s", ipaddr, netmask, gateway, metric);
			if(!strcmp(old[i], tmp_entry)) goto _setting_value;
			modify_flag = 1;
			sprintf(tmp_entry, "%s:%s:%s", ipaddr, netmask, gateway);
		}else if(!strncmp(old[i], tmp_entry, strlen(tmp_entry))) goto _exit_validate_static_route;
	}
	
	if (i >= STATIC_ROUTE_PAGE)
	{
		if (!strcmp(gateway,"0.0.0.0") || !strcmp(gateway,"")) goto _exit_validate_static_route; 
		else {
//			printf("leijun haha\n");
			eval("/usr/bin/route", "add", "-net", ipaddr, "netmask", netmask, "gw", gateway, "metric", metric);
			//judge the entry is adde to router tables successful!
			if (entry_in_route_table(ipaddr, netmask, gateway) < 1) 
				goto _exit_validate_static_route; 
			else if (modify_flag)
			{
				del_route(old[atoi(page)]);
			}
			sprintf(old[atoi(page)], "%s:%s:%s:%s", ipaddr, netmask, gateway, metric);
		}
	}

	for(i=0 ; i<STATIC_ROUTE_PAGE ; i++)
	{
		if (i == 0) sprintf(buf, "%s", strcmp(old[i], "")? old[i] : "$");
		else sprintf(buf, "%s %s", buf, strcmp(old[i], "")? old[i] : "$");
	}

	nvram_set("static_route", buf);
	
_setting_value:
	static_route_added = 1;	
		
_exit_validate_static_route:

	if(ipaddr)	free(ipaddr);
	if(netmask)	free(netmask);
	if(gateway)	free(gateway);
}

int
ej_static_route_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, page;
//	int which;
	char *type;
//	char word[256], *next;

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	page = atoi(websGetVar(wp, "route_page", "0"));	// default to 0

	if(!strcmp(type,"select")){
		for(i=0 ; i < STATIC_ROUTE_PAGE ; i++){
#if 0
			char name[50]=" ";
			char new_name[80]=" ";
			char buf[80]="";
			which = i;
			foreach(word, nvram_safe_get("static_route_name"), next) {
				if (which-- == 0) {
					find_match_pattern(name, sizeof(name), word, "$NAME:"," ");
					filter_name(name, new_name, sizeof(new_name), GET);
				}
			}
			snprintf(buf, sizeof(buf), "(%s)", new_name);
#endif
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d</option>\n",i,(i == page) ? "selected" : "",i+1);
		}
	}

	return ret;
}
//added by leijun
int
del_route(char *route)
{
	char word[80], *tmp;
	char *ipaddr, *netmask, *gateway, *metric, *ifname;

	foreach(word, route, tmp) {
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
#if 0
		ifname = metric;
		metric = strsep(&ifname, ":");
		if (!ifname)
			continue;		
#endif
		eval("/usr/bin/route", "del", "-net", ipaddr, "netmask", netmask, "gw", gateway, "metric", metric);
	}

	return 0;
}


int
delete_static_route(webs_t wp)
{
	char buf[1000]="", *cur = buf;
//	char buf_name[1000]="", *cur_name = buf_name;
	static char word[256], *next;
//	static char word_name[256], *next_name;
	char *page = websGetVar(wp, "route_page", NULL); 
	int i = 0;

	foreach(word, nvram_safe_get("static_route"), next) {
		if(i == atoi(page))	{
		//modifed by leijun
			del_route(word);
			i++; 
			strcpy(word, "$");
		}
				
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", word);
		i++;
	}

	nvram_set("static_route",buf);

	return 1;
}
