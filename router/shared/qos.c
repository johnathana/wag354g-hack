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

#include "broadcom.h"

#define ZB_DEBUG
	
int ej_qos_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	int ret = 0;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
		
	if(!strcmp(arg,"qosSW"))
	{
		char *qosSW = nvram_safe_get("qosSW");	
		if(((count == 1) && (!strcmp(qosSW, "1"))) || ((count == 0) && (!strcmp(qosSW, "0"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_ftp"))
	{
		char *qos_ftp = nvram_safe_get("qos_app0");	
		if(((count == 1) && (!strcmp(qos_ftp, "1"))) || ((count == 0) && (!strcmp(qos_ftp, "0"))) || ((count == 2) && (!strcmp(qos_ftp, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_http"))
	{
		char *qos_http = nvram_safe_get("qos_app1");	
		if(((count == 1) && (!strcmp(qos_http, "1"))) || ((count == 0) && (!strcmp(qos_http, "0"))) || ((count == 2) && (!strcmp(qos_http, "2"))))
			return websWrite(wp, "checked");
	}
	
	if(!strcmp(arg,"qos_telnet"))
	{
		char *qos_telnet = nvram_safe_get("qos_app2");	
		if(((count == 1) && (!strcmp(qos_telnet, "1"))) || ((count == 0) && (!strcmp(qos_telnet, "0"))) || ((count == 2) && (!strcmp(qos_telnet, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_smtp"))
	{
		char *qos_smtp = nvram_safe_get("qos_app3");	
		if(((count == 1) && (!strcmp(qos_smtp, "1"))) || ((count == 0) && (!strcmp(qos_smtp, "0"))) || ((count == 2) && (!strcmp(qos_smtp, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_pop3"))
	{
		char *qos_pop3 = nvram_safe_get("qos_app4");	
		if(((count == 1) && (!strcmp(qos_pop3, "1"))) || ((count == 0) && (!strcmp(qos_pop3, "0"))) || ((count == 2) && (!strcmp(qos_pop3, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_spec_port0"))
	{
		char *qos_spec_port0 = nvram_safe_get("qos_spec_port0");	
		return websWrite(wp,"%s",qos_spec_port0);
	}
	
	if(!strcmp(arg,"qos_spec_port1"))
	{
		char *qos_spec_port1 = nvram_safe_get("qos_spec_port1");	
		return websWrite(wp,"%s",qos_spec_port1);
	}
	
	if(!strcmp(arg,"qos_spec_port2"))
	{
		char *qos_spec_port2 = nvram_safe_get("qos_spec_port2");	
		return websWrite(wp,"%s",qos_spec_port2);
	}
	
	if(!strcmp(arg,"qos_spec0"))
	{
		//char *qos_spec0 = nvram_safe_get("qos_spec0");	
		char *qos_spec0 = nvram_safe_get("qos_app5");	
		if(((count == 1) && (!strcmp(qos_spec0, "1"))) || ((count == 0) && (!strcmp(qos_spec0, "0"))) || ((count == 2) && (!strcmp(qos_spec0, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_spec1"))
	{
		//char *qos_spec1 = nvram_safe_get("qos_spec1");	
		char *qos_spec1 = nvram_safe_get("qos_app6");	
		if(((count == 1) && (!strcmp(qos_spec1, "1"))) || ((count == 0) && (!strcmp(qos_spec1, "0"))) || ((count == 2) && (!strcmp(qos_spec1, "2"))))
			return websWrite(wp, "checked");
		
	}
	
	if(!strcmp(arg,"qos_spec2"))
	{
		//char *qos_spec2 = nvram_safe_get("qos_spec2");	
		char *qos_spec2 = nvram_safe_get("qos_app7");	
		if(((count == 1) && (!strcmp(qos_spec2, "1"))) || ((count == 0) && (!strcmp(qos_spec2, "0"))) || ((count == 2) && (!strcmp(qos_spec2, "2"))))
			return websWrite(wp, "checked");
		
	}

	if(!strcmp(arg,"qos_adv"))
	{
		char *qos_adv = nvram_safe_get("qos_adv");	
		if((!strcmp(qos_adv, "1")))
			return websWrite(wp, "checked");
		else
			return websWrite(wp,"");
	}

	if(!strcmp(arg,"qos_size"))
	{
		char *qos_size = nvram_safe_get("qos_size");	
		return websWrite(wp,"%s",qos_size);
	}

	if(!strcmp(arg,"qos_vlan"))
	{
		char *qos_vlan = nvram_safe_get("qos_vlan");	
		if((!strcmp(qos_vlan, "1")))
			return websWrite(wp, "checked");
		else
			return websWrite(wp,"");
		
	}

	if(!strcmp(arg,"qos_vid"))
	{
		return websWrite(wp,"%s",nvram_safe_get("qos_vid"));
	}

	/*if(!strcmp(arg,"ipaddr"))
	{
		char *vidip = nvram_safe_get("qos_vidip");
		return websWrite(wp, "%d", get_single_ip(vidip,count));
	}*/
	
	return ret;
}

void validate_qos_config(webs_t wp, char *value, struct variable *v)
{
	char *qosSW_l,*qos_ftp_l,*qos_http_l,*qos_telnet_l,*qos_smtp_l,*qos_pop3_l,*qos_spec_port0_l,*qos_spec_port1_l,*qos_spec_port2_l,*qos_spec0_l,*qos_spec1_l,*qos_spec2_l,*qos_adv_l,*qos_size_l,*qos_vlan_l,*qos_vid_l/*,*vid_ip_l*/;

#ifdef ZB_DEBUG
	printf("enter validate_qos_config\n");
#endif 

	qosSW_l = websGetVar(wp, "qosSW", "0");
	qos_ftp_l = websGetVar(wp, "qos_ftp", "0");
	qos_http_l = websGetVar(wp, "qos_http", "0");
	qos_telnet_l = websGetVar(wp, "qos_telnet", "0");
	qos_smtp_l = websGetVar(wp, "qos_smtp", "0");
	qos_pop3_l = websGetVar(wp, "qos_pop3", "0");
	qos_spec_port0_l = websGetVar(wp, "qos_spec_port0", "0");
	qos_spec_port1_l = websGetVar(wp, "qos_spec_port1", "0");
	qos_spec_port2_l = websGetVar(wp, "qos_spec_port2", "0");
	qos_spec0_l = websGetVar(wp, "qos_spec0", NULL);
	qos_spec1_l = websGetVar(wp, "qos_spec1", NULL);
	qos_spec2_l = websGetVar(wp, "qos_spec2", NULL);

	qos_adv_l = websGetVar(wp, "hid_qos_adv", "0");
	qos_vlan_l = websGetVar(wp, "hid_qos_vlan", "0");

	printf("qos_adv=%s,qos_vlan=%s\n",qos_adv_l,qos_vlan_l);

	//if(!strcmp(qos_adv_l,"1"))
		qos_size_l = websGetVar(wp, "qos_size", "0");
	//else
	//	qos_size_l = "0";

	qos_vid_l = websGetVar(wp,"qos_vid","0");

	#if 0
	//if(!strcmp(qos_vlan_l,"1"))
	{
		char temp[30], *val=NULL;
		int i;
		
		
		vid_ip_l = malloc(20);
		memset(vid_ip_l, 0, sizeof(vid_ip_l));
		memset(temp, 0, sizeof(temp));
		
	        for(i = 0 ; i < 4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s%d", "vid_ip", i);
	                val = websGetVar(wp, temp , NULL);
        	        if(val)
			{
                        	strcat(vid_ip_l,val);
	                        if(i < 3) 
					strcat(vid_ip_l,".");
                	}
	                else
			{	
				free(vid_ip_l);
	                        return ;
			}
    		}
	}
	/*else
	{
		qos_vid_l = "0";
		vid_ip_l = "0.0.0.0";
	}*/
#endif

	#ifdef ZB_DEBUG
		printf("enter validate_qos_config2\n");
	#endif 
	
	nvram_set("qosSW", qosSW_l);
	//printf("qosSW %s", qosSW_l);
	
	if(!strcmp(qosSW_l,"1"))
	{
		#ifdef ZB_DEBUG
			printf("qosSW enabled--nvram set other options.\n");
		#endif
	
		nvram_set("qos_app0", qos_ftp_l);
		nvram_set("qos_app1", qos_http_l);
		nvram_set("qos_app2", qos_telnet_l);
		nvram_set("qos_app3", qos_smtp_l);
		nvram_set("qos_app4", qos_pop3_l);
		nvram_set("qos_spec_port0", qos_spec_port0_l);
		nvram_set("qos_spec_port1", qos_spec_port1_l);
		nvram_set("qos_spec_port2", qos_spec_port2_l);
		nvram_set("qos_app5", qos_spec0_l);
		nvram_set("qos_app6", qos_spec1_l);
		nvram_set("qos_app7", qos_spec2_l);
		nvram_set("qos_adv",qos_adv_l);
		nvram_set("qos_size",qos_size_l);
		nvram_set("qos_vlan",qos_vlan_l);
		nvram_set("qos_vid",qos_vid_l);
		//nvram_set("qos_vidip",vid_ip_l);
#if 0
		if(!strcmp(qos_vlan_l,"1") && vid_ip_l)
		{
			free(vid_ip_l);
		}
#endif		
	}

	nvram_commit();	
	return;
}

