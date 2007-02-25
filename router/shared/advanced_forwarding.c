
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
#include <cy_conf.h>

#define ADVANCED_FORWARDING_NUM 30
#if 0
#define CSH_DEBUG //define for debug print
#endif

/* Example:
 * [on|off]:[tcp|udp|all]:10.10.10.10[-10.10.10.20]:80:90>33:myDomain:60:80
 */
void
validate_advanced_forwarding(webs_t wp, char *value, struct variable *v)
{
	int i, error=0;
	char buf[3000] = "", *cur = buf;
	char *adv_enabled;
	adv_enabled= websGetVar(wp, "enabled_disabled", "0");
	if(adv_enabled)
	{
		nvram_set("advanced_status", adv_enabled);
	}
	
	for (i = 0; i < ADVANCED_FORWARDING_NUM; i++) {
		int sip_flag = 0;
		char adv_addr_range[] = "addr_range_XXXXXX";
		char adv_sip0[] = "srcipaddr_XXXXXX";
		char adv_sip1[] = "srcipaddr_XXXXXX";
		char adv_sip2[] = "srcipaddr_XXXXXX";
		char adv_sip3[] = "srcipaddr_XXXXXX";
		char adv_sip4[] = "srcipaddr_XXXXXX";
		char adv_sip5[] = "srcipaddr_XXXXXX";
		char adv_sip6[] = "srcipaddr_XXXXXX";
		char adv_sip7[] = "srcipaddr_XXXXXX";
		char adv_sSPort[] = "srcstartport_XXXXXX";
		char adv_sEPort[] = "srcendport_XXXXXX";
		char adv_pro[] = "pro_XXXXXX";
		char adv_IPorD[] = "ip_domain_XXXXXX";
		char adv_dIP[] = "dstipaddr_XXXXXX";
		char adv_dDomain[] = "dstdomain_XXXXXX";
		char adv_dSPort[] = "dststartport_XXXXXX";
		char adv_dEPort[] = "dstendport_XXXXXX";
		char adv_enable[] = "enable_XXXXXX";

		char *addr_range="", sourceIPAddr[35], *sip0="", *sip1="", *sip2="", *sip3="", *sip4="", *sip5="", *sip6="", *sip7="", *sSPort="", *sEPort="", *pro="", *IPorD="", ipordomain[66], *dIP="", *dDomain="", *dSPort="", *dEPort="",*enable="";
		snprintf(adv_addr_range, sizeof(adv_addr_range), "addr_range_%d", i);
                snprintf(adv_sip0, sizeof(adv_sip0), "srcipaddr_%d_0", i);
                snprintf(adv_sip1, sizeof(adv_sip1), "srcipaddr_%d_1", i);
                snprintf(adv_sip2, sizeof(adv_sip2), "srcipaddr_%d_2", i);
                snprintf(adv_sip3, sizeof(adv_sip3), "srcipaddr_%d_3", i);
                snprintf(adv_sip4, sizeof(adv_sip4), "srcipaddr_%d_4", i);
                snprintf(adv_sip5, sizeof(adv_sip5), "srcipaddr_%d_5", i);
                snprintf(adv_sip6, sizeof(adv_sip6), "srcipaddr_%d_6", i);
                snprintf(adv_sip7, sizeof(adv_sip7), "srcipaddr_%d_7", i);
                snprintf(adv_sSPort, sizeof(adv_sSPort), "srcstartport_%d", i);
                snprintf(adv_sEPort, sizeof(adv_sEPort), "srcendport_%d", i);
                snprintf(adv_pro, sizeof(adv_pro), "pro_%d", i);
                snprintf(adv_IPorD, sizeof(adv_IPorD), "ip_domain_%d", i);
                snprintf(adv_dIP, sizeof(adv_dIP), "dstipaddr%d", i);
                snprintf(adv_dDomain, sizeof(adv_dDomain), "dstdomain_%d", i);
                snprintf(adv_dSPort, sizeof(adv_dSPort), "dststartport_%d", i);
                snprintf(adv_dEPort, sizeof(adv_dEPort), "dstendport_%d", i);
                snprintf(adv_enable, sizeof(adv_enable), "enable_%d", i);
	
		addr_range = websGetVar(wp, adv_addr_range, NULL);
		if(!addr_range) continue;
		else{
			if(!strcmp(addr_range, "all")) sip_flag = 0;
			else if(!strcmp(addr_range, "addr")) sip_flag = 1;
			else if(!strcmp(addr_range, "range")) sip_flag = 2;
			else continue;
		}
		if(sip_flag != 0){
			sip0 = websGetVar(wp, adv_sip0, "");
			sip1 = websGetVar(wp, adv_sip1, "");
			sip2 = websGetVar(wp, adv_sip2, "");
			sip3 = websGetVar(wp, adv_sip3, "");
		}
		if(sip_flag == 2){
			sip4 = websGetVar(wp, adv_sip4, "");
			sip5 = websGetVar(wp, adv_sip5, "");
			sip6 = websGetVar(wp, adv_sip6, "");
			sip7 = websGetVar(wp, adv_sip7, "");
		}
		sSPort = websGetVar(wp, adv_sSPort, "0");
		sEPort = websGetVar(wp, adv_sEPort, "0");
		pro = websGetVar(wp, adv_pro, NULL);
		IPorD = websGetVar(wp, adv_IPorD, NULL);	
		dIP = websGetVar(wp, adv_dIP, "0");
		dDomain = websGetVar(wp, adv_dDomain, "0");
		dSPort = websGetVar(wp, adv_dSPort, "0");
		dEPort = websGetVar(wp, adv_dEPort, "0");
		enable = websGetVar(wp, adv_enable, "off");


#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s] \n%s=[%s] %s=[%s] %s=[%s] %s=[%s] \n%s=[%s] %s=[%s] %s=[%s]\n %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,adv_sip0,sip0,adv_sip1,sip1,adv_sip2,sip2,adv_sip3,sip3,adv_sSPort,sSPort,adv_sEPort,sEPort,adv_pro,pro,adv_IPorD,IPorD,adv_dIP,dIP,adv_dDomain,dDomain,adv_dSPort,dSPort,adv_dEPort,dEPort,adv_enable,enable);
	printf("%s(%d): %s=[%s] %s=[%s] %s=[%s] \n%s=[%s] %s=[%s] %s=[%s] %s=[%s] \n%s=[%s] %s=[%s] %s=[%s]\n %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,adv_sip0,sip0,adv_sip1,sip1,adv_sip2,sip2,adv_sip3,sip3,adv_sSPort,sSPort,adv_sEPort,sEPort,adv_pro,pro,adv_IPorD,IPorD,adv_dIP,dIP,adv_dDomain,dDomain,adv_dSPort,dSPort,adv_dEPort,dEPort,adv_enable,enable);

#endif
		if(!*sSPort || !*sEPort || !*dSPort || !*dEPort)
			continue;
		if(sip_flag != 0 && (!*sip0 || !*sip1 || !*sip2 || !*sip3))
			continue;
		if(sip_flag == 2 &&(!sip4 || !sip5 || !sip6 || !sip7))
			continue;
		if(!*IPorD)
			continue;
		else{
			if(!strcmp(IPorD, "ip") && !*dIP)
				continue;
			if(!strcmp(IPorD, "ip") && *dIP){
				if(atoi(dIP)>0 && atoi(dIP)<254)
					{
						dDomain = "";
					}
				else continue;
			}
			else if(!strcmp(IPorD, "domain") && !*dDomain)
				continue;
			else if(!strcmp(IPorD, "domain") && *dDomain)
				if(!strcmp(dDomain, "")) continue;
				else{
					dIP = "255";
				}
		}
		if(!*pro || !*enable){
			continue;}
		if(sip_flag == 0)
			strcpy(sourceIPAddr, "0");
		else if(sip_flag == 1)
			sprintf(sourceIPAddr, "%s.%s.%s.%s", sip0, sip1, sip2, sip3);
		else if(sip_flag == 2)
		{
			int swap_flag = 0;
			if(atoi(sip0) > atoi(sip4)) swap_flag = 1;
			else if((atoi(sip0)==atoi(sip4)) && (atoi(sip1)>atoi(sip5)))
				swap_flag = 1;
			else if((atoi(sip0)==atoi(sip4)) && (atoi(sip1)==atoi(sip5)) && (atoi(sip2)>atoi(sip6)))
				swap_flag = 1;
			else if((atoi(sip0)==atoi(sip4)) && (atoi(sip1)==atoi(sip5)) && (atoi(sip2)==atoi(sip6)) && (atoi(sip3)>atoi(sip7)))
				swap_flag = 1;
			if(swap_flag == 0)
				sprintf(sourceIPAddr, "%s.%s.%s.%s-%s.%s.%s.%s", sip0, sip1, sip2, sip3, sip4, sip5, sip6, sip7);
			else
				sprintf(sourceIPAddr, "%s.%s.%s.%s-%s.%s.%s.%s", sip4, sip5, sip6, sip7, sip0, sip1, sip2, sip3);
		}		
		/*if((!strcmp(sSPort,"0") || !strcmp(sSPort,"")) && 
		   (!strcmp(sEPort,"0") || !strcmp(sEPort,""))){
			continue;
		}
		if((!strcmp(dSPort,"0") || !strcmp(dSPort,"")) && 
		   (!strcmp(dEPort,"0") || !strcmp(dEPort,""))){
			continue;
		}*/
		if(!strcmp(sSPort,"") && !strcmp(sEPort,""))
		{
			sSPort = "1";
			sEPort = "1";
		}
		if (!strcmp(sSPort,"")) sSPort = sEPort;
		if (!strcmp(sEPort,"")) sEPort = sSPort;

		if(atoi(sSPort) > atoi(sEPort)){
			SWAP(sSPort, sEPort);
		}

		if(!strcmp(dSPort,"") && !strcmp(dEPort,""))
		{
			dSPort = "1";
			dEPort = "1";
		}
		if (!strcmp(dSPort,"")) dSPort = dEPort;
		if (!strcmp(dEPort,"")) dEPort = dSPort;
		if(atoi(dSPort) > atoi(dEPort)){
			SWAP(dSPort, dEPort);
		}
		if(!strcmp(IPorD, "ip"))
			strcpy(ipordomain, dIP);
		else if(!strcmp(IPorD, "domain"))
			strcpy(ipordomain, dDomain);
		
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%s:%s:%s:%s>%s:%s:%s:%s",
				cur == buf ? "" : " ",enable, pro, sourceIPAddr, sSPort, sEPort, dIP, dDomain, dSPort, dEPort);

	}
	if(!error)
		nvram_set(v->name, buf);
}

int
ej_adv_forward(int eid, webs_t wp, int argc, char_t **argv)
{
		char *type;
		int  which;
	
		if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
			 websError(wp, 400, "Insufficient args\n");
			 return -1;
		}
		if(!strcmp(type, "enabled_disabled"))
		{
			char status[3];
			strcpy(status, nvram_safe_get("advanced_status"));
			if((!strcmp(status, "1") && which == 1) || (!strcmp(status, "0") && which == 0))
				return websWrite(wp,"checked");
			else
				return websWrite(wp,"");
		}
		return 0;
}
		
int ej_advanced_forwarding_init(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, j;
	char config[3000], *pre, *next;
	char *child_pre, *child_next;
#ifdef CSH_DEBUG
	//printf("\ninto advanced_forwarding_init()\n");
#endif
	for(i=0; i<ADVANCED_FORWARDING_NUM; i++)
	{
		websWrite(wp, "\tdocument.advancedforwardingtable.dstdomain_%d.disabled=true;\n", i);
	}
	for(i=0; i<ADVANCED_FORWARDING_NUM; i++)
		for(j=0; j<8; j++)
			websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_%d.disabled=true;\n", i, j);
			

	i = 0;
	strcpy(config, nvram_safe_get("advanced_forwarding_config"));
	sprintf(config, "%s%s", config, " ");
	pre = config;
	next = strstr(pre, " ");//???????????
	for(; next && i<ADVANCED_FORWARDING_NUM; i++,pre=next+1,next=strstr(pre," "))
	{
		if(next)
		{
			*next = '\0';
			child_pre = pre;
			child_next = strchr(pre, ':');
			if(!child_next){printf("error1;\n"); continue;}
			*child_next = '\0';
			if(!strcmp(child_pre, "on"))
				websWrite(wp, "\tdocument.advancedforwardingtable.enable_%d.checked=true;\n", i);
			else if(!strcmp(child_pre, "off"))
				websWrite(wp, "\tdocument.advancedforwardingtable.enable_%d.checked=false;\n", i);

			child_pre = child_next + 1;
			child_next = strchr(child_pre, ':');
			if(!child_next){printf("error2;\n"); continue;}
			*child_next = '\0';
			if(!strcmp(child_pre, "all"))
				websWrite(wp, "\tdocument.advancedforwardingtable.pro_%d.selectedIndex=0;\n", i);
			else if(!strcmp(child_pre, "tcp"))
				websWrite(wp, "\tdocument.advancedforwardingtable.pro_%d.selectedIndex=1;\n", i);
			else if(!strcmp(child_pre, "udp"))
				websWrite(wp, "\tdocument.advancedforwardingtable.pro_%d.selectedIndex=2;\n", i);
			//get source IP address
			child_pre = child_next + 1;
			child_next = strchr(child_pre, ':');
			if(!child_next){printf("error3;\n"); continue;}
			*child_next = '\0';
			if(!strcmp(child_pre, "0"))
			{
				websWrite(wp, "\tdocument.advancedforwardingtable.addr_range_%d.selectedIndex=0;\n", i);
				//ip address is 0.0.0.0
			}
			else if(!strchr(child_pre, '-'))
			{
				int j;
				websWrite(wp, "\tdocument.advancedforwardingtable.addr_range_%d.selectedIndex=1;\n", i);
				for(j=0; j<3; j++)
				{
					char *ptr;
					ptr = strchr(child_pre, '.');
					if(*ptr)
					{
						*ptr = '\0';
						websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_%d.value=%s;\n", i, j, child_pre);
						websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_%d.disabled=false;\n", i, j);
						child_pre = ptr + 1;
					}
				}
				if(*child_pre){
					websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_3.value=%s;\n", i, child_pre);
					websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_3.disabled=false;\n", i);
				}
				//ip address is leagal
			}
			else if(strchr(child_pre, '-'))
			{
				int j;
				char *ptr;
				ptr = strchr(child_pre, '-');
				if(!ptr){printf("Error\n");  continue;}
				*ptr = '.';
				websWrite(wp, "\tdocument.advancedforwardingtable.addr_range_%d.selectedIndex=2;\n", i);
				for(j=0; j<7; j++)
				{
					char *ptr;
					ptr = strchr(child_pre, '.');
					if(*ptr)
					{
						*ptr = '\0';
						websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_%d.value=%s;\n", i, j, child_pre);
						websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_%d.disabled=false;\n", i, j);
						child_pre = ptr + 1;
					}
				}
				if(*child_pre){
					websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_7.value=%s;\n", i, child_pre);
					websWrite(wp, "\tdocument.advancedforwardingtable.srcipaddr_%d_7.disabled=false;\n", i);
				}
				//ip address is leagal
			}

			//get source start/end port
			child_pre = child_next + 1;
			child_next = strchr(child_pre, ':');
			if(!child_next){printf("error4;\n"); continue;}
			*child_next = '\0';
			if(child_pre)
				websWrite(wp, "\tdocument.advancedforwardingtable.srcstartport_%d.value=%s;\n", i, child_pre);
				;//source_start_port
			
			child_pre = child_next + 1;
			child_next = strchr(child_pre, '>');
			if(!child_next){printf("error5;\n"); continue;}
			*child_next = '\0';
			if(child_pre)
				websWrite(wp, "\tdocument.advancedforwardingtable.srcendport_%d.value=%s;\n", i, child_pre);

			//get destination ipaddress/domin_name,and IPorDomain selected box
			child_pre = child_next + 1;
			child_next = strchr(child_pre, ':');
			if(!child_next){printf("error6;\n"); continue;}
			*child_next = '\0';
			if(!strcmp(child_pre, "255"))
			{
				child_pre = child_next + 1;
				child_next = strchr(child_pre, ':');
				if(!child_next){printf("error7;\n"); continue;}
				*child_next = '\0';
				websWrite(wp, "\tdocument.advancedforwardingtable.dstdomain_%d.value=\"%s\";\n", i, child_pre);
				websWrite(wp, "\tdocument.advancedforwardingtable.dstdomain_%d.disabled=false;\n", i);
				websWrite(wp, "\tdocument.advancedforwardingtable.dstipaddr%d.disabled=true;\n", i);
				websWrite(wp, "\tdocument.advancedforwardingtable.ip_domain_%d.selectedIndex=1;\n", i);
					;//dst ip, iporDomin, dst domain
			}
			else
			{
				//the destination ipaddresses of the 21 lines from begin is set according with the config in variable "lan_ipaddr"
				if(i<28)
				{
					char newIP[20], *newptr;
					strcpy(newIP, nvram_safe_get("lan_ipaddr"));
					newptr = strrchr(newIP, '.');
					if(!newptr){printf("error\n");continue;}
					newptr++;
					websWrite(wp, "\tdocument.advancedforwardingtable.dstipaddr%d.value=%s;\n", i, newptr);
				}
				//the destination ipaddresses of the last 9 lines are set according to the default value of "advanced_forwarding_config"
				else
					websWrite(wp, "\tdocument.advancedforwardingtable.dstipaddr%d.value=%s;\n", i, child_pre);
					
				websWrite(wp, "\tdocument.advancedforwardingtable.dstdomain_%d.disabled=true;\n", i);
				websWrite(wp, "\tdocument.advancedforwardingtable.dstipaddr%d.disabled=false;\n", i);
				websWrite(wp, "\tdocument.advancedforwardingtable.ip_domain_%d.selectedIndex=0;\n", i);
				child_pre = child_next + 1;
				child_next = strchr(child_pre, ':');
			}

			//get destination start port
			child_pre = child_next + 1;
			child_next = strchr(child_pre, ':');
			if(!child_next){printf("error8;\n"); continue;}
		//printf("csh:child_pre=%s\n", child_pre);
			*child_next = '\0';
			if(child_pre)
				websWrite(wp, "\tdocument.advancedforwardingtable.dststartport_%d.value=%s;\n", i, child_pre);
				;//dst_start_port
			
			//get destination end port
			child_pre = child_next + 1;
			if(!child_pre){printf("error9;\n"); continue;}
			if(child_pre)
				websWrite(wp, "\tdocument.advancedforwardingtable.dstendport_%d.value=%s;\n", i, child_pre);
				;//dst_end_port
		}
	}
	return 0;
}
