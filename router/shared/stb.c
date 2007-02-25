
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

#define STB_ENTRY_NUM 4
static unsigned char t_ip_addr[20];

int ej_dump_stb_entry(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, i, enable = atoi(nvram_safe_get("stb_matchenable"));
	char *stb_entry_conf = nvram_safe_get("stb_matchtype"), *conftmp = stb_entry_conf;
	
	for(i=0; i<STB_ENTRY_NUM; i++)
	{
		conftmp = strsep(&stb_entry_conf, "?");
		ret += websWrite(wp, "\tdocument.forms[0].stbvalue_%d.value=\"%s\";\n", i, conftmp[0]?conftmp:"");
		ret += websWrite(wp, "\tdocument.forms[0].enable_%d.checked=%s;\n", i, (enable & 1<<i)?"true":"false");
	}
	
	if(nvram_match("stb_enable", "0"))
	{
		for(i=0; i<STB_ENTRY_NUM; i++)
		{
			ret += websWrite(wp, "\tdocument.forms[0].stbvalue_%d.disabled=%s;\n", i, "true");
			ret += websWrite(wp, "\tdocument.forms[0].enable_%d.disabled=%s;\n", i, "true");
		}
		for(i=0; i<4; i++)
		{
			ret += websWrite(wp, "\tdocument.forms[0].telus_ipaddr_%d.disabled=%s;\n", i, "true");
		}
	}
	
	return ret;
}


int ej_telus_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{

	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	 memset(t_ip_addr, 0, sizeof(t_ip_addr));
		
	 strcpy(t_ip_addr, nvram_safe_get("telus_config"));
			
	if(!strcmp(arg, "ipaddr"))
		return websWrite(wp, "%d", get_single_ip(t_ip_addr, count));
	
	return -1;

}
void validate_telus_config(webs_t wp, char *value, struct variable *v)
{


	int i;
	char  *l_stbenable, *ipaddr;
	
	char temp[30], *val=NULL;
	
	

	l_stbenable = websGetVar(wp, "stb_enable", "0");
	
	
	
	if( !strcmp(l_stbenable, "0"))
		return;
	else if( !strcmp(l_stbenable, "1"))
	{
		ipaddr = malloc(20);
		memset(ipaddr, 0, sizeof(ipaddr));
		memset(temp, 0, sizeof(temp));
             for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "telus_ipaddr", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(ipaddr,val);
                        	if(i<3) 
					strcat(ipaddr,".");
                	}
                	else
			{	
				free(ipaddr);
                        	return ;
			}
             }
		
        	
        	
		
		if (!*ipaddr) 
		{
                	websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
                	return;
        	}
        
        	
	
		
	}
	
	nvram_set("telus_config", ipaddr);
	if(ipaddr)	
		free(ipaddr);
	return;


}
void validate_stbentry_config(webs_t wp, char *value, struct variable *v)
{
	int i, enable = 0;
	char conf_name[strlen("stb_matchtype") + 3];
	char enable_name[strlen("stb_matchenable") + 1];
	char buf[50*STB_ENTRY_NUM], *cur=buf;
	char enable_str[5];
	char *stb_enabled = websGetVar(wp, "stb_enable", "0");

	nvram_set("stb_enable", stb_enabled);
	if(!strcmp(stb_enabled, "0"))
		return;
     
	memset(conf_name, 0, sizeof(conf_name));
	sprintf(conf_name, "stb_matchtype");
	memset(enable_name, 0, sizeof(enable_name));
	sprintf(enable_name, "stb_matchenable");
	memset(buf, 0, sizeof(buf));
	
	for(i=0; i<STB_ENTRY_NUM; i++)
	{
		char tmp_name[15], *matchtype, *enableptr;
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "stbvalue_%d", i);
		matchtype = websGetVar(wp, tmp_name, "");

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "enable_%d", i);
		enableptr = websGetVar(wp, tmp_name, "0");
		
		if(matchtype[0])
			cur += sprintf(cur, matchtype);
		if(i != STB_ENTRY_NUM - 1)
			cur += sprintf(cur, "?");
		printf("cur %d %s\n", i, cur);
		//cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s", (cur == buf) ? "" : "?", matchtype);
		
		enable |= (!strcmp(enableptr, "1"))?(1<<i):0;
	}

	printf("haha...my post buf %s, enable %x\n", buf, enable);
	nvram_set(conf_name, buf);
	memset(enable_str, 0, sizeof(enable_str));
	sprintf(enable_str, "%d", enable);
	nvram_set(enable_name, enable_str);

	return;
}




