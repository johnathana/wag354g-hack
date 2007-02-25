
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
#include "pvc.h"
#define WAN_IFNAME "/tmp/status/wan_ifname"
extern char connection_routing[5];


/* Dump route in <tr><td>IP</td><td>MASK</td><td>GW</td><td>Hop Count</td><td>interface</td></tr> format */
int
ej_dump_route_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0, i = 0;
	int count = 0;
	char *format;
	FILE *fp;
	int flgs, ref, use, metric;
        unsigned long dest, gw, netmask;
	char line[256];
	struct in_addr dest_ip;
        struct in_addr gw_ip;
        struct in_addr netmask_ip;
	char sdest[16], sgw[16], pt[60], wan_ifname[10];
	int debug = 0, blank = 1;

	if (ejArgs(argc, argv, "%s", &format) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	/* open route table*/
	if ((fp = fopen("/proc/net/route", "r")) == NULL) {
       		websError(wp, 400, "No route table\n");
        	return -1;
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
	
        /* Read the route cache entries. */
	//Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT                                                       
	//vmnet1  004410AC        00000000        0001    0       0       0       00FFFFFF        40      0       0                   

	while( fgets(line, sizeof(line), fp) != NULL ) {
		if(count) {
			int ifl = 0;
			while(line[ifl]!=' ' && line[ifl]!='\t' && line[ifl]!='\0')
			ifl++;
			line[ifl]=0;    /* interface */
			if(sscanf(line+ifl+1, "%lx%lx%X%d%d%d%lx", &dest, &gw, &flgs, &ref, &use, &metric, &netmask)!=7) {
				break;
		        }  
			debug = 0;
			dest_ip.s_addr = dest;
			gw_ip.s_addr   = gw;
			netmask_ip.s_addr = netmask;

			strcpy(sdest,  (dest_ip.s_addr==0 ? "0.0.0.0" : inet_ntoa(dest_ip)));    //default
			strcpy(sgw,    (gw_ip.s_addr==0   ? "0.0.0.0" : inet_ntoa(gw_ip)));      //*

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): route table[%d] dest[%lx][%s] gw[%lx][%s] flags[%X] ref[%d] use[%d] metric[%d] netmask[%lx][%s] [%s]",__FUNCTION__,count,dest,sdest,gw,sgw,flgs,ref,use,metric,netmask,inet_ntoa(netmask_ip),(flgs&RTF_UP ? "" : "deleted"));
#endif
		
			/* not 0x0001 route usable */
			if(!(flgs&RTF_UP))	continue; 

			/* filter ppp pseudo interface for DOD */
			if(!strcmp(sdest,PPP_PSEUDO_IP) && !strcmp(sgw,PPP_PSEUDO_GW))	debug = 1;
			//added by leijun 2004-03-07:21:30	
			if(!strcmp(sdest,"224.0.0.0") && !strcmp(inet_ntoa(netmask_ip), "240.0.0.0") && !strcmp(sgw,"0.0.0.0") ) debug = 1;
			
			//added by leijun 2004-09-08:21:30	
			//if(strstr(sdest,"10.112.112"))
			//	debug = 1;
			
			/* Don't show loopback device */
			if(!strcmp(line,"lo"))	debug = 1;
			if (!strncasecmp(line, "ipsec", 5)) debug = 1;

			/* zhangbin 2005.4.28 */
			if(metric == 0)
			{
				metric++;
			}


			/* Don't show eth1 information for pppoe mode */
			//if(!strcmp(line, wan_ifname) && nvram_match("wan_proto","pppoe"))	debug = 1; //modified by leijun

			/* Don't show pseudo interface */
			/*
			if(!strncmp(line, "ppp", 3)){
				fp1 = fopen("/tmp/ppp/link", "r");
				if(!fp1)	debug = 1;
				else		fclose(fp1);
			}
			*/
			ret += websWrite(wp,"%s%c'%s','%s','%s','%d','%s'\n",
					debug ? "//" : "",
					blank ? ' ' : ',', 
					sdest,
					inet_ntoa(netmask_ip),
					sgw,
					metric,
					(strcmp(line,"br0") ? "WAN" : "LAN"));    

			if(debug && blank)	blank = 1;
			else		blank = 0;


		}

		count++;
	}

	return ret;
}
//modified by zls 2005-0106
void
validate_dynamic_route(webs_t wp,char *value,struct variable *v)
{
        int pvc_connection_index;
        char rip_enable[2],rip_interface_var[32], buf[16]/*,rip_interface_content[16]*/;
        char  *sel_ripTX,*sel_ripRX,*sel_MulticastBroadcast,*enable_SendDefaultRoute;
        struct variable rip_variables[] = {
                {longname: "Danamic Route", argv: ARGV("1","2","3","0")},
                {longname: "Danamic Route", argv: ARGV("1","2","0")},
                {longname: "Danamic Route",argv: ARGV("0","1")},
                {longname: "Danamic Route",argv: ARGV("0","1")},
        },*which;
        if (valid_choice(wp,value,v))
                nvram_set(v->name,value);
	#ifdef RIP_EXTEND_SUPPORT
        	pvc_connection_index = atoi(websGetVar(wp,"rip_interface","1"));
	#else
        	pvc_connection_index = RIP_INTERFACE_SUM -1;
	#endif
        strcpy(rip_enable,websGetVar(wp,"rip_enable",""));
	if(strcmp(rip_enable,"0")){
       		sel_ripTX=websGetVar(wp,"sel_ripTX","");
       		which = &rip_variables[0];
       	        sel_ripRX=websGetVar(wp,"sel_ripRX","");
		#ifdef RIP_EXTEND_SUPPORT
                sel_MulticastBroadcast=websGetVar(wp,"sel_MulticastBroadcast","");             
                enable_SendDefaultRoute=websGetVar(wp,"SDroute_enable","");
		#else
                sel_MulticastBroadcast = "0";
                enable_SendDefaultRoute="1";
                #endif 
	        #ifdef RIP_EXTEND_SUPPORT	
               if (!(valid_choice(wp,sel_ripTX,&which[0])) || !(valid_choice(wp,                   sel_ripRX,&which[1])) || !(valid_choice(wp,
                   sel_MulticastBroadcast,&which[2])) ||
                   !(valid_choice(wp,enable_SendDefaultRoute,&which[3]))){
                   return;
              }
              #else
              if (!(valid_choice(wp,sel_ripTX,&which[0])) || !(valid_choice(wp,                   sel_ripRX,&which[1]))){
                   return;
              }

              #endif
	     //make rip configure var
              sprintf(rip_interface_var,"rip_interface_content_%d",pvc_connection_index);
             
              sprintf(buf,"%d:%d:%d",atoi(sel_ripTX),atoi(sel_ripRX),atoi(sel_MulticastBroadcast));
            nvram_set(rip_interface_var,buf);
            //set send_droute
	   #ifdef RIP_EXTEND_SUPPORT
            nvram_set("send_droute",enable_SendDefaultRoute);
	   #endif
        }                
}
//modified by leijun
/*void
validate_dynamic_route(webs_t wp, char *value, struct variable *v)
{
	struct variable tx_variables[] = {
		{ longname: "Danamic Route", argv: ARGV("1", "2", "3") },
	}, *which;
	struct variable rx_variables[] = {
		{ longname: "Danamic Route", argv: ARGV("1", "2") },
	}, *which1;
	
	char *dr_setting;
	
	if (valid_choice(wp, value, v)) 
		nvram_set(v->name, value);

	which = &tx_variables[0];
	dr_setting = websGetVar(wp, "sel_ripTX", NULL);
	if(!dr_setting)	return;

	if (!valid_choice(wp, dr_setting, &which[0])) return;
	nvram_set("sel_ripTX",dr_setting);
	
	which1 = &rx_variables[0];
	
	dr_setting = websGetVar(wp, "sel_ripRX", NULL);
	if(!dr_setting)	return;

	if (!valid_choice(wp, dr_setting, &which1[0])) return;
	nvram_set("sel_ripRX",dr_setting);
}
*/
//modifid by zls 2005-0110
int
ej_nvram_match_routing(int eid,webs_t wp,int argc,char_t **argv){
        char rip_interface_var[32],rip_interface_content[32];
        int /*pvc_connection=0,*/i_send=0,i_receive=0,i_MulticastBroadcast=0;
        char *type,*match,*output;
     
        if (ejArgs(argc,argv,"%s %s %s",&type,&match,&output)<3){
                websError(wp,400,"Insufficent args\n");
                return -1;
        }
        #ifdef RIP_EXTEND_SUPPORT
        	pvc_connection =atoi( websGetVar(wp,"rip_interface","1"));
        	sprintf(rip_interface_var,"rip_interface_content_%d",pvc_connection);
		//pvc_connection=atoi(nvram_safe_get("singleportforward_connection"));
        #else
        	sprintf(rip_interface_var,"rip_interface_content_%d",RIP_INTERFACE_SUM -1);
	#endif
        //fetch the rip configure content
        strcpy(rip_interface_content,nvram_safe_get(rip_interface_var));
        if(!strcmp(rip_interface_content,rip_interface_var)) return 0;
         
        if (strlen(rip_interface_content)>4)
        {
                sscanf(rip_interface_content,"%d:%d:%d",&i_send,&i_receive,&i_MulticastBroadcast);
	 if (!(strcasecmp(type,"send")) && (atoi(match) == i_send)){
                        return  websWrite(wp,output);
                }
                if (!(strcasecmp(type,"receive")) && (atoi(match) == i_receive))
                        return  websWrite(wp,output);
                #ifdef RIP_EXTEND_SUPPORT   
                if (!(strcasecmp(type,"MulticastBroadcast")) && (atoi(match) == i_MulticastBroadcast))
                        return  websWrite(wp,output);
                 #endif
        }
        return websWrite(wp,"");
}
#ifdef RIP_EXTEND_SUPPORT
//written by zls 2005-0110
int
ej_rip_interface_table(int eid, webs_t wp, int argc, char_t **argv)
{
        int i, ret = 0, which;
        char *type;
        char cur_connect[5];
                                                                                                               
        if (ejArgs(argc, argv, "%s", &type) < 1)
        {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
                                                                                                               
        if(gozila_action) //for form post
        {
                char *tmpconn = websGetVar(wp, "rip_interface", NULL);
                strncpy(cur_connect, tmpconn, sizeof(cur_connect));
                if (atoi(cur_connect) < RIP_INTERFACE_SUM)
                {
                        nvram_set("rip_interface",cur_connect);
                        nvram_commit();
                }
        }
 else //first appear
  {
#if 0
                strncpy(cur_connect, nvram_safe_get("upnp_active_connection"), sizeof(cur_connect));
                nvram_set("singleportforward_connection",cur_connect);
#endif
                                                                                                               
                strncpy(cur_connect, nvram_safe_get("rip_interface"),sizeof(cur_connect));
       
 }
                                                                                                               
        which = atoi(cur_connect);
        if(!strcmp(type,"select"))
        {
                for(i=0 ; i < RIP_INTERFACE_SUM ; i++)
                {
                        if (i < PVC_CONNECTIONS)
                                ret = websWrite(wp,"\t\t<option value=\"%d\" %s>PVC%d </option>\n",i,(i == which) ? "selected" : "",i+1);
                        else
                                ret = websWrite(wp,"\t\t<option value=\"%d\" %s>BR%d </option>\n",i,(i == which) ? "selected" : "",i - PVC_CONNECTIONS);
                 }
        }
        return ret;
}
#endif
//add by leijun
void validate_nat_func(webs_t wp, char *value, struct variable *v)
{
#ifdef PER_PVC_SUPPORT
	char buf[24];
	char *tmp;
#endif
	if (valid_choice(wp, value, v)) 
		nvram_set(v->name, value);
#ifdef PER_PVC_SUPPORT
	tmp = buf;
	snprintf(tmp,24,"wan_connection_routing%d",atoi(connection_routing));
	nvram_set("wan_connection_routing",connection_routing);
#endif
	if (atoi(value) == 0)
	{
#ifdef PER_PVC_SUPPORT
		nvram_set(tmp,"0");
#endif
		nvram_set("wk_mode", "router");

	}else {
#ifdef PER_PVC_SUPPORT
		nvram_set(tmp,"1");
		printf("tmp=%s\n",tmp);
#endif
		nvram_set("wk_mode", "gateway");
	}	
}


int 
ej_nvram_match_nat(int eid,webs_t wp,int argc,char_t **argv)
{
	char buf[24];
	char *buf_index=buf;
#ifdef PER_PVC_SUPPORT
	char tmp[5];
	char *tmp_index=tmp;
#endif
	char num[5];
	char *num_tmp=num;
	char *name, *match, *output;

	snprintf(buf_index,24,"wan_connection_routing%d",atoi(connection_routing));
	num_tmp = nvram_safe_get(buf_index);
	printf("this is nat_match\n");


	if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	printf("num_tmp=%d\n",atoi(num_tmp));
      	if(atoi(num_tmp)==1)
	{
		printf("enter if \n");
                return websWrite(wp, output);
}
        return 0;



}
int 
ej_nvram_match_nat1(int eid,webs_t wp,int argc,char_t **argv)
{
	char buf[24];
	char *buf_index=buf;
#ifdef PER_PVC_SUPPORT
	char tmp[5];
	char *tmp_index=tmp;
#endif
	char num[5];
	char *num_tmp=num;
	char *name, *match, *output;
//	tmp_index = nvram_safe_get("wan_connection_routing");
	snprintf(buf_index,24,"wan_connection_routing%d",atoi(connection_routing));
	num_tmp = nvram_safe_get(buf_index);
	printf("this is nat_match1\n");


	if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	printf("num_tmp=%d\n",atoi(num_tmp));
      	if(atoi(num_tmp)==0)
	{
	printf("it is in if \n");
                return websWrite(wp, output);
}
	return 0;



}


