
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


int gre_entry_config_inited;
char gre_config_setting[100];
char tunnel[20];
char *gre_tunnel_status;
char *gre_tunnel_name;
char *gre_localgw;
char *gre_remote_net_ip[4];
char *gre_remote_netmask[4];
char *gre_remote_gateway_ip[4];

char my_cloud_setting[100];
char *cloud_dhcp_status;
char *cloud_dhcp_ipaddr[4];
char *cloud_dhcp_server[4];
char *cloud_dhcp_netmask[4];



#define WAN_IFNAME  "/tmp/status/wan_ifname"
#define WAN_IPADDR  "/tmp/status/wan_ipaddr"
#define SUM_OF_GRE_ENTRY 5   //the number of GRE entries
#if 0
#define CSH_DEBUG //define for debug print
#endif

void
validate_gre_config(webs_t wp, char *value, struct variable *v)
{
	char buf[100] = "", my_variable[12] = "";

	char *my_tunnel_entry="", *my_tunnel_status="", *my_tunnel_name="", *my_localgw_conn="", *my_net_ipaddr0="", *my_net_ipaddr1="", *my_net_ipaddr2="", *my_net_ipaddr3="", *my_netmask0="", *my_netmask1="", *my_netmask2="", *my_netmask3="", *my_gateway_ipaddr0="", *my_gateway_ipaddr1="", *my_gateway_ipaddr2="", *my_gateway_ipaddr3="", *my_dhcp_status="", *my_dhcp_ipaddr0="", *my_dhcp_ipaddr1="", *my_dhcp_ipaddr2="", *my_dhcp_ipaddr3="", *my_dhcp_server0="", *my_dhcp_server1="", *my_dhcp_server2="", *my_dhcp_server3="", *my_dhcp_netmask0="", *my_dhcp_netmask1="", *my_dhcp_netmask2="", *my_dhcp_netmask3="";
	
#ifdef GRE_SUPPORT
	if(!strcmp("gre", nvram_safe_get("cloud_gre")))
		my_tunnel_entry = websGetVar(wp, "gre_tunnel_entry", NULL);
#endif
	my_tunnel_status = websGetVar(wp, "gre_tunnel_status", "0");
	my_tunnel_name = websGetVar(wp, "gre_tunnel_name", "");
	my_localgw_conn = websGetVar(wp, "localgw_conn", NULL);
	my_net_ipaddr0 = websGetVar(wp, "gre_net_ipaddr0", "");
	my_net_ipaddr1 = websGetVar(wp, "gre_net_ipaddr1", "");
	my_net_ipaddr2 = websGetVar(wp, "gre_net_ipaddr2", "");
	my_net_ipaddr3 = websGetVar(wp, "gre_net_ipaddr3", "");
	my_netmask0 = websGetVar(wp, "gre_netmask0", "");
	my_netmask1 = websGetVar(wp, "gre_netmask1", "");
	my_netmask2 = websGetVar(wp, "gre_netmask2", "");
	my_netmask3 = websGetVar(wp, "gre_netmask3", "");
	my_gateway_ipaddr0 = websGetVar(wp, "gre_gateway_ipaddr0", "0");
	my_gateway_ipaddr1 = websGetVar(wp, "gre_gateway_ipaddr1", "0");
	my_gateway_ipaddr2 = websGetVar(wp, "gre_gateway_ipaddr2", "0");
	my_gateway_ipaddr3  = websGetVar(wp, "gre_gateway_ipaddr3", "0");
#ifdef CLOUD_SUPPORT
	if(!strcmp("cloud", nvram_safe_get("cloud_gre")))
	{
		my_dhcp_status = websGetVar(wp, "cloud_dhcp_status", "0");
		my_dhcp_ipaddr0 = websGetVar(wp, "cloud_dhcp_ipaddr0", "");
		my_dhcp_ipaddr1 = websGetVar(wp, "cloud_dhcp_ipaddr1", "");
		my_dhcp_ipaddr2 = websGetVar(wp, "cloud_dhcp_ipaddr2", "");
		my_dhcp_ipaddr3 = websGetVar(wp, "cloud_dhcp_ipaddr3", "");
		my_dhcp_server0 = websGetVar(wp, "cloud_dhcp_server0", "");
		my_dhcp_server1 = websGetVar(wp, "cloud_dhcp_server1", "");
		my_dhcp_server2 = websGetVar(wp, "cloud_dhcp_server2", "");
		my_dhcp_server3 = websGetVar(wp, "cloud_dhcp_server3", "");
		my_dhcp_netmask0 = websGetVar(wp, "cloud_dhcp_netmask0", "");
		my_dhcp_netmask1 = websGetVar(wp, "cloud_dhcp_netmask1", "");
		my_dhcp_netmask2 = websGetVar(wp, "cloud_dhcp_netmask2", "");
		my_dhcp_netmask3 = websGetVar(wp, "cloud_dhcp_netmask3", "");
		if(my_dhcp_status && my_dhcp_ipaddr0 && my_dhcp_ipaddr1 && my_dhcp_ipaddr2 && my_dhcp_ipaddr3 && my_dhcp_server0 && my_dhcp_server1 && my_dhcp_server2 && my_dhcp_server3 && my_dhcp_netmask0 && my_dhcp_netmask1 && my_dhcp_netmask2 && my_dhcp_netmask3){
			sprintf(buf, "%s", my_dhcp_status);
			nvram_set("cloud_dhcp_enable", buf);
			sprintf(buf, "%s.%s.%s.%s", my_dhcp_ipaddr0, my_dhcp_ipaddr1, my_dhcp_ipaddr2, my_dhcp_ipaddr3);
			nvram_set("cloud_dhcp_agent", buf);
			sprintf(buf, "%s.%s.%s.%s", my_dhcp_server0, my_dhcp_server1, my_dhcp_server2, my_dhcp_server3);
			nvram_set("cloud_dhcp_server", buf);
			sprintf(buf, "%s.%s.%s.%s", my_dhcp_netmask0, my_dhcp_netmask1, my_dhcp_netmask2, my_dhcp_netmask3);
			nvram_set("cloud_dhcp_netmask", buf);
		}

	}
#endif

#ifdef GRE_SUPPORT
	if(!strcmp("gre", nvram_safe_get("cloud_gre")))
		if(!my_tunnel_entry)
			return;
#endif

	if(my_tunnel_status && my_tunnel_name && my_localgw_conn && my_net_ipaddr0 && my_net_ipaddr1 && my_net_ipaddr2 && my_net_ipaddr3 && my_netmask0 && my_netmask1 && my_netmask2 && my_netmask3 && my_gateway_ipaddr0 && my_gateway_ipaddr1 && my_gateway_ipaddr2 && my_gateway_ipaddr3)
	{
		sprintf(buf, "%s:%s:%s.%s.%s.%s:%s.%s.%s.%s:%s.%s.%s.%s:%s",
				my_tunnel_status, my_tunnel_name, my_net_ipaddr0, my_net_ipaddr1, my_net_ipaddr2, my_net_ipaddr3, my_netmask0, my_netmask1, my_netmask2, my_netmask3, my_gateway_ipaddr0, my_gateway_ipaddr1, my_gateway_ipaddr2, my_gateway_ipaddr3, my_localgw_conn);
#ifdef GRE_SUPPORT
		if(!strcmp("gre", nvram_safe_get("cloud_gre")))
		{
			sprintf(my_variable, "%s%s", v->name, my_tunnel_entry);
			nvram_set("gre_tunnel_entry", my_tunnel_entry);
		}
#endif

#ifdef CLOUD_SUPPORT
		if(!strcmp("cloud", nvram_safe_get("cloud_gre")))
			sprintf(my_variable, "%s1", v->name);
#endif
		nvram_set(my_variable, buf);
	}
}// end of validate_gre_config(webs_t wp, char *value, struct variable *v)

int ej_ipsec_gre_config(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg,*arg2;
	int count;
	int ret=0;

	if(ejArgs(argc, argv, "%s %s", &arg, &arg2) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(arg,"ipsec_gre_link"))
	{
#ifdef GRE_SUPPORT
		if(nvram_match("cloud_enable", "0"))
		{
			return websWrite(wp, "<a href=\"GRE_tunnel.asp\">GRE</a>");
		}
#endif
#ifdef CLOUD_SUPPORT
		if(nvram_match("cloud_enable", "1"))
		{
			return websWrite(wp, "GRE");
		}
#endif

	}
	return -1;
}

int ej_cloud_init(int eid, webs_t wp, int argc, char_t **argv)
{
	char status[2], *status_l;
	int i;
	//if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
	if(gozila_action)
	{
		status_l = websGetVar(wp, "cloud_dhcp_status", "0");
		if(!strcmp(status_l, "1"))
			for(i=0; i<4; i++)
				websWrite(wp, "\tdocument.Cloud.cloud_dhcp_server%d.disabled=false\n", i);
		else if(!strcmp(status_l, "0"))
			for(i=0; i<4; i++)
				websWrite(wp, "\tdocument.Cloud.cloud_dhcp_server%d.disabled=true\n", i);

	}
	else
	{
		strcpy(status, nvram_safe_get("cloud_dhcp_enable"));
		if(!strcmp(status, "1"))
			for(i=0; i<4; i++)
				websWrite(wp, "\tdocument.Cloud.cloud_dhcp_server%d.disabled=false\n", i);
		else if(!strcmp(status, "0"))
			for(i=0; i<4; i++)
				websWrite(wp, "\tdocument.Cloud.cloud_dhcp_server%d.disabled=true\n", i);
	}
	return 0;

}
int ej_index_init(int eid, webs_t wp, int argc, char_t **argv)
{
	char status[2];
	strcpy(status, nvram_safe_get("cloud_enable"));
	if(!strcmp(status, "1"))
	{
			websWrite(wp, "\tdocument.setup.lan_proto[2].disabled=true;\n");
			websWrite(wp, "\tdocument.setup.lan_proto[0].checked=true;\n");
	}
	else if(!strcmp(status, "0"))
			websWrite(wp, "\tdocument.setup.lan_proto[2].disabled=false;\n");
	return 0;

}

int ej_gre_config(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg,*arg2;
	int count;
	int ret=0;

	if(ejArgs(argc, argv, "%s %s", &arg, &arg2) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(!strcmp(arg, "cloud"))
	{
		nvram_set("cloud_gre", "cloud");
		return websWrite(wp, "");
	}

	if(!strcmp(arg, "gre"))
	{
		nvram_set("cloud_gre", "gre");
		return websWrite(wp, "");
	}

	if(!strcmp(arg,"gre_tunnel_status"))
	{
		//only in Cloud.asp, gozila is of use!
		if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
		{
			char *status_l = websGetVar(wp, "gre_tunnel_status", "0");
			if((!strcmp(arg2, "1") && !strcmp(status_l, "1")) || (!strcmp(arg2, "0") && (!strcmp(status_l, "0"))))
				return websWrite(wp,"checked");
		}
		else
		{
#ifdef CLOUD_SUPPORT
			if(!strcmp("cloud", nvram_safe_get("cloud_gre")))
				gre_entry_config_init();
#endif
			if(!gre_tunnel_status)
			{
				if(!strcmp(arg2, "0"))
					return websWrite(wp, "checked");
			}
			else
			{
				if((!strcmp(arg2, "1") && !strcmp(gre_tunnel_status, "1")) || (!strcmp(arg2, "0") && (!strcmp(gre_tunnel_status, "0"))))
					return websWrite(wp,"checked");
			}
		}
	}
	
	if(!strcmp(arg,"gre_tunnel_name"))
	{
		if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
		{
			char *name_l = websGetVar(wp, "gre_tunnel_name", "0");
			return websWrite(wp, name_l);
		}
		else
		{
			if(!gre_tunnel_name)
				return websWrite(wp, "");
			else
				return websWrite(wp, gre_tunnel_name);
		}
	}
	
	if(!strcmp(arg,"gre_net_ipaddr"))
	{
		if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
		{
			char my_ip[20], *ip_l;
			sprintf(my_ip, "%s%s", "gre_net_ipaddr", arg2);
			ip_l = websGetVar(wp, my_ip, "0");
			return websWrite(wp, ip_l);
		}
		else
		{
			count = atoi(arg2);
			if(!gre_remote_net_ip[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, gre_remote_net_ip[count]);
		}
	}
	
	if(!strcmp(arg,"gre_netmask"))
	{
		if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
		{
			char my_mask[20], *mask_l;
			sprintf(my_mask, "%s%s", "gre_netmask", arg2);
			mask_l = websGetVar(wp, my_mask, "0");
			return websWrite(wp, mask_l);
		}
		else
		{
			count = atoi(arg2);
			if(!gre_remote_netmask[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, gre_remote_netmask[count]);
		}
	}
	
	if(!strcmp(arg,"gre_gateway_ipaddr"))
	{
		if(gozila_action && !strcmp("cloud", nvram_safe_get("cloud_gre")))
		{
			char my_gateway[20], *gateway_l;
			sprintf(my_gateway, "%s%s", "gre_gateway_ipaddr", arg2);
			gateway_l = websGetVar(wp, my_gateway, "0");
			return websWrite(wp, gateway_l);
		}
		else
		{
			count = atoi(arg2);
			if(!gre_remote_gateway_ip[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, gre_remote_gateway_ip[count]);
		}
	}
	
	if(!strcmp(arg,"select"))
	{
		int i, which;
		char *tmptunnel;
		if(gozila_action){
			tmptunnel =websGetVar(wp, "gre_tunnel_entry", NULL);
			if(tmptunnel != NULL)
			{
				strcpy(tunnel, tmptunnel);
				gre_entry_config_inited = 0;
			}
		}
		else{
			strcpy(tunnel, nvram_safe_get("gre_tunnel_entry"));
			if(!strcmp(tunnel, "gre_tunnel_entry")) strcpy(tunnel, "1");
			gre_entry_config_inited = 0;
		}
		if(gre_entry_config_inited == 0)
			gre_entry_config_init();
		which = atoi(tunnel);
		for(i=1; i<=SUM_OF_GRE_ENTRY; i++)
		{
			ret = websWrite(wp, "<option value=\"%d\" %s> %d </option>\n", i, (i==which? "selected" : ""), i);
		}
		return ret;
	}

	if(!strcmp(arg, "localgwselect"))
	{
		int i, ret = 0, which;
		int selectable, wanconn_exist = 0;
		char readbuf[256];
		char wan_ifname[20],word[8][6],wan_ipaddr[8][20],ipbuf[256];

		if(!gre_localgw)
			gre_localgw = "1";	

		which = atoi(nvram_safe_get("wan_active_connection"));
	
		if(which == 8)	
		{
			return websWrite(wp,"\t\t<option value=\"8\">No Wan Connection</option>\n");
		}
	
		memset(wan_ifname,0,sizeof(wan_ifname));
		memset(wan_ipaddr,0,sizeof(wan_ipaddr));
		memset(ipbuf,0,sizeof(ipbuf));
		memset(word,0,sizeof(word));
		
		if(!file_to_buf(WAN_IFNAME, readbuf, 256))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
			return 0;
		}
		sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
		
		if(!file_to_buf(WAN_IPADDR, ipbuf, 256))
		{
			printf("no buf in %s!\n", WAN_IPADDR);
			return 0;
		}
		sscanf(ipbuf, "%s %s %s %s %s %s %s %s", wan_ipaddr[0], wan_ipaddr[1], wan_ipaddr[2], wan_ipaddr[3], wan_ipaddr[4], wan_ipaddr[5], wan_ipaddr[6], wan_ipaddr[7]);
		
		for(i=0 ; i < 8 ; i++)
		{
			selectable = 0;
			
			if(strcmp(word[i], "Down"))
			{
				if(strstr(word[i], "ppp"))
				{
					//if(check_ppp_link(i) && check_ppp_proc(i))
					if( check_ppp_proc(i))
					{
						selectable = 1;
					}
				}
				else if(strstr(word[i], "nas"))
				{
					selectable = 1;
				}
			}
			
			if(selectable)
			{
				wanconn_exist = 1;
				ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", i, (atoi(gre_localgw) == i)?"selected" : "", i+1, word[i]);	
			}
		}

		if(!wanconn_exist)
			ret += websWrite(wp,"\t\t<option value=\"8\">No WAN Connection</option>\n");

		return ret;

	}
	if(!strcmp(arg,"cloud_dhcp_status"))
	{
		if(gozila_action)
		{
			char *status_l = websGetVar(wp, "cloud_dhcp_status", "0");
			if((!strcmp(arg2, "1") && !strcmp(status_l, "1")) || (!strcmp(arg2, "0") && (!strcmp(status_l, "0"))))
				return websWrite(wp,"checked");
		}
		else
		{
			if(!cloud_dhcp_status)
			{
				if(!strcmp(arg2, "0"))
					return websWrite(wp, "checked");
			}
			else
			{
				if((!strcmp(arg2, "1") && !strcmp(cloud_dhcp_status, "1")) || (!strcmp(arg2, "0") && (!strcmp(cloud_dhcp_status, "0"))))
					return websWrite(wp,"checked");
			}
		}
	}
	if(!strcmp(arg,"cloud_dhcp_ipaddr"))
	{
		if(gozila_action)
		{
			char my_ip[20], *ipaddr_l;
			sprintf(my_ip, "%s%s", "cloud_dhcp_ipaddr", arg2);
			ipaddr_l = websGetVar(wp, my_ip, "0");
			return websWrite(wp, ipaddr_l);
		}
		else
		{
			dhcp_config_init();
			count = atoi(arg2);
			if(!cloud_dhcp_ipaddr[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, cloud_dhcp_ipaddr[count]);
		}
	}
	if(!strcmp(arg,"cloud_dhcp_server"))
	{
		if(gozila_action)
		{
			char my_server[20], *server_l;
			sprintf(my_server, "%s%s", "cloud_dhcp_server", arg2);
			server_l = websGetVar(wp, my_server, "");
			return websWrite(wp, server_l);
		}
		else
		{
			count = atoi(arg2);
			if(!cloud_dhcp_server[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, cloud_dhcp_server[count]);
		}
	}
	if(!strcmp(arg,"cloud_dhcp_netmask"))
	{
		if(gozila_action)
		{
			char my_netmask[20], *netmask_l;
			sprintf(my_netmask, "%s%s", "cloud_dhcp_netmask", arg2);
			netmask_l = websGetVar(wp, my_netmask, "0");
			return websWrite(wp, netmask_l);
		}
		else
		{
			count = atoi(arg2);
			if(!cloud_dhcp_netmask[count])
				return websWrite(wp, "");
			else
				return websWrite(wp, cloud_dhcp_netmask[count]);
		}
	}

	
	return -1;
}// end of ej_gre_config(int eid, webs_t wp, int argc, char_t **argv)

void gre_entry_config_init()
{
	char buf[15], *pre;
#ifdef GRE_SUPPORT
	if(!strcmp("gre", nvram_safe_get("cloud_gre")))
		sprintf(buf, "%s%s", "gre_config", tunnel);
#endif

#ifdef CLOUD_SUPPORT
	if(!strcmp("cloud", nvram_safe_get("cloud_gre")))
		sprintf(buf, "gre_config1");
#endif
	strcpy(gre_config_setting, nvram_safe_get(buf));
	if(!strcmp(gre_config_setting, buf) || !strcmp(gre_config_setting, ""))
		pre = NULL;
	else
		pre = gre_config_setting;

	gre_tunnel_status = strsep(&pre, ":");
	gre_tunnel_name = strsep(&pre, ":");
	gre_remote_net_ip[0] = strsep(&pre, ".");
	gre_remote_net_ip[1] = strsep(&pre, ".");
	gre_remote_net_ip[2] = strsep(&pre, ".");
	gre_remote_net_ip[3] = strsep(&pre, ":");
	gre_remote_netmask[0] = strsep(&pre, ".");
	gre_remote_netmask[1] = strsep(&pre, ".");
	gre_remote_netmask[2] = strsep(&pre, ".");
	gre_remote_netmask[3] = strsep(&pre, ":");
	gre_remote_gateway_ip[0] = strsep(&pre, ".");
	gre_remote_gateway_ip[1] = strsep(&pre, ".");
	gre_remote_gateway_ip[2] = strsep(&pre, ".");
	gre_remote_gateway_ip[3] = strsep(&pre, ":");
	gre_localgw = strsep(&pre, ":");

#ifdef GRE_SUPPORT
	if(!strcmp("gre", nvram_safe_get("cloud_gre")))
		gre_entry_config_inited = 1;
#endif
}// end of gre_entry_config_init(){}

#ifdef GRE_SUPPORT
int gre_delete_tunnel(webs_t wp)
{
	int ret = 0;
	char buf[15];
	char *id = websGetVar(wp,"gre_tunnel_entry",NULL);
	if(!id)
		return -1;
	
	sprintf(buf, "%s%s", "gre_config", id);
	strcpy(gre_config_setting, nvram_safe_get(buf));
	if(!strcmp(gre_config_setting, buf) || !strcmp(gre_config_setting, ""))
		return 0;
	else{
	//csh:Before deleting,we should down this tunnel. 
		gre_down_tunnel(atoi(id), gre_config_setting);
		nvram_set(buf, "");
	}
	strcpy(tunnel, id);
	gre_entry_config_inited = 0;
		
	return 0;
}

int gre_down_tunnel(int entry, char *gre_config_setting)
{
	char *pre, *ptr;
	FILE *gre_fd;
	
	if(!gre_config_setting)
		return -1;
	pre = gre_config_setting;
	ptr = strsep(&pre, ":");
	ptr = strsep(&pre, ":");
	if(!ptr) return -1;

	if((gre_fd = fopen("/tmp/gre_delete", "w")) == NULL)
	{
		printf("can't create /tmp/gre_delete.conf\n");
		return -1;
	}
	fprintf(gre_fd, "#!/bin/sh\n\n");
	fprintf(gre_fd, "/usr/sbin/ip link set gre%d down\n", entry);
	fprintf(gre_fd, "/usr/sbin/ip tunnel del gre%d\n", entry);
	fclose(gre_fd);

	eval("/bin/sh", "/tmp/gre_delete");
	return 0;
}
#endif

#ifdef CLOUD_SUPPORT 
// end of gre_entry_config_init(){}
void dhcp_config_init()
{
	int flag = 1;
	char *pre;
	char tmp[20];
	strcpy(tmp, nvram_safe_get("cloud_dhcp_enable"));
	if(!strcmp(tmp, "cloud_dhcp_enable") || !strcmp(tmp, ""))
		flag = 0;
	strcpy(my_cloud_setting, tmp);
	strcpy(tmp, nvram_safe_get("cloud_dhcp_agent"));
	if(!strcmp(tmp, "cloud_dhcp_agent") || !strcmp(tmp, ""))
		flag = 0;
	sprintf(my_cloud_setting, "%s:%s", my_cloud_setting, tmp);
	strcpy(tmp, nvram_safe_get("cloud_dhcp_server"));
	if(!strcmp(tmp, "cloud_dhcp_server") || !strcmp(tmp, ""))
		flag = 0;
	sprintf(my_cloud_setting, "%s:%s", my_cloud_setting, tmp);
	strcpy(tmp, nvram_safe_get("cloud_dhcp_netmask"));
	if(!strcmp(tmp, "cloud_dhcp_netmask") || !strcmp(tmp, ""))
		flag = 0;
	sprintf(my_cloud_setting, "%s:%s", my_cloud_setting, tmp);
	if(flag == 0)
		pre = NULL;
	else
		pre = my_cloud_setting;
	cloud_dhcp_status = strsep(&pre, ":");
	cloud_dhcp_ipaddr[0] = strsep(&pre, ".");
	cloud_dhcp_ipaddr[1] = strsep(&pre, ".");
	cloud_dhcp_ipaddr[2] = strsep(&pre, ".");
	cloud_dhcp_ipaddr[3] = strsep(&pre, ":");
	cloud_dhcp_server[0] = strsep(&pre, ".");
	cloud_dhcp_server[1] = strsep(&pre, ".");
	cloud_dhcp_server[2] = strsep(&pre, ".");
	cloud_dhcp_server[3] = strsep(&pre, ":");
	cloud_dhcp_netmask[0] = strsep(&pre, ".");
	cloud_dhcp_netmask[1] = strsep(&pre, ".");
	cloud_dhcp_netmask[2] = strsep(&pre, ".");
	cloud_dhcp_netmask[3] = strsep(&pre, ":");

}// end of dhcp_config_init(){}
#endif
