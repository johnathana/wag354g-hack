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
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wait.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <rc.h>
#include "pvc.h"

char *gre_tunnel_name;
char *gre_remote_net_ip;
char *gre_remote_netmask;
char *gre_remote_gateway_ip;
char *gre_localgw;
char wan_ipaddr[8][20];

#define CSH_DEBUG 1
#define GRE_CONF "/tmp/gretunnel.conf"
#define GRE_DECONF "/tmp/gretunnel.deconf"
#define SUM_OF_GRE_ENTRY 5

/*if somesome wrong, return -1, else return 0.*/
int gre_config_sep(char *gre_config_setting)
{
	char setting[100], *pre;
	if(!gre_config_setting) return -1;
	strcpy(setting, gre_config_setting);
	pre = setting;
	gre_tunnel_name = strsep(&pre, ":");
	gre_tunnel_name = strsep(&pre, ":");
	gre_remote_net_ip = strsep(&pre, ":");
	gre_remote_netmask = strsep(&pre, ":");
	gre_remote_gateway_ip = strsep(&pre, ":");
	gre_localgw = strsep(&pre, ":");
	if(!gre_tunnel_name || !gre_remote_net_ip || !gre_remote_netmask ||
!gre_remote_gateway_ip || !gre_localgw)
		return -1;
	if(atoi(gre_localgw) == 8)
		return -1;

	{
		int i, selectable, wanconn_exist = 0;
		char readbuf[256];
		char word[8][6], ipbuf[256];

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
				if(atoi(gre_localgw) == i)
				{
					wanconn_exist = 1;
					gre_localgw = wan_ipaddr[i];
					break;
				}
			}
		}

		if(!wanconn_exist)
			return -1;

	}

	if(!strcmp(gre_remote_netmask, "128.0.0.0"))
		gre_remote_netmask = "1";
	else if(!strcmp(gre_remote_netmask, "192.0.0.0"))
		gre_remote_netmask = "2";
	else if(!strcmp(gre_remote_netmask, "224.0.0.0"))
		gre_remote_netmask = "3";
	else if(!strcmp(gre_remote_netmask, "240.0.0.0"))
		gre_remote_netmask = "4";
	else if(!strcmp(gre_remote_netmask, "248.0.0.0"))
		gre_remote_netmask = "5";
	else if(!strcmp(gre_remote_netmask, "252.0.0.0"))
		gre_remote_netmask = "6";
	else if(!strcmp(gre_remote_netmask, "254.0.0.0"))
		gre_remote_netmask = "7";
	else if(!strcmp(gre_remote_netmask, "255.0.0.0"))
		gre_remote_netmask = "8";
	else if(!strcmp(gre_remote_netmask, "255.128.0.0"))
		gre_remote_netmask = "9";
	else if(!strcmp(gre_remote_netmask, "255.192.0.0"))
		gre_remote_netmask = "10";
	else if(!strcmp(gre_remote_netmask, "255.224.0.0"))
		gre_remote_netmask = "11";
	else if(!strcmp(gre_remote_netmask, "255.240.0.0"))
		gre_remote_netmask = "12";
	else if(!strcmp(gre_remote_netmask, "255.248.0.0"))
		gre_remote_netmask = "13";
	else if(!strcmp(gre_remote_netmask, "255.252.0.0"))
		gre_remote_netmask = "14";
	else if(!strcmp(gre_remote_netmask, "255.254.0.0"))
		gre_remote_netmask = "15";
	else if(!strcmp(gre_remote_netmask, "255.255.0.0"))
		gre_remote_netmask = "16";
	else if(!strcmp(gre_remote_netmask, "255.255.128.0"))
		gre_remote_netmask = "17";
	else if(!strcmp(gre_remote_netmask, "255.255.192.0"))
		gre_remote_netmask = "18";
	else if(!strcmp(gre_remote_netmask, "255.255.224.0"))
		gre_remote_netmask = "19";
	else if(!strcmp(gre_remote_netmask, "255.255.240.0"))
		gre_remote_netmask = "20";
	else if(!strcmp(gre_remote_netmask, "255.255.248.0"))
		gre_remote_netmask = "21";
	else if(!strcmp(gre_remote_netmask, "255.255.252.0"))
		gre_remote_netmask = "22";
	else if(!strcmp(gre_remote_netmask, "255.255.254.0"))
		gre_remote_netmask = "23";
	else if(!strcmp(gre_remote_netmask, "255.255.255.0"))
		gre_remote_netmask = "24";
	else if(!strcmp(gre_remote_netmask, "255.255.255.128"))
		gre_remote_netmask = "25";
	else if(!strcmp(gre_remote_netmask, "255.255.255.192"))
		gre_remote_netmask = "26";
	else if(!strcmp(gre_remote_netmask, "255.255.255.224"))
		gre_remote_netmask = "27";
	else if(!strcmp(gre_remote_netmask, "255.255.255.240"))
		gre_remote_netmask = "28";
	else if(!strcmp(gre_remote_netmask, "255.255.255.248"))
		gre_remote_netmask = "29";
	else if(!strcmp(gre_remote_netmask, "255.255.255.252"))
		gre_remote_netmask = "30";
	return 0;
}

int enable_gre()
{
	int i;
	char gre_config[15];
	char gre_config_setting[100];
	FILE *gre_fd;
	int cloud_gre_start = 0;
	if((gre_fd = fopen(GRE_CONF, "w")) == NULL)
	{
		printf("can't create %s\n", GRE_CONF);
		return -1;
	}
	fprintf(gre_fd, "#!/bin/sh\n\n");
#ifdef GRE_SUPPORT
	if(nvram_match("cloud_enable", "0"))
	{
		for(i=1; i<=SUM_OF_GRE_ENTRY; i++)
		{
			sprintf(gre_config, "%s%d", "gre_config", i);
			strcpy(gre_config_setting, nvram_safe_get(gre_config));
			if(!strcmp(gre_config_setting, gre_config))
				continue;
			if(gre_config_setting[0] == '0')
				continue;
			if(gre_config_sep(gre_config_setting))
				continue;
		
			fprintf(gre_fd, "ip tunnel add gre%d mode gre remote %s local %s ttl 255\n", i, gre_remote_gateway_ip, gre_localgw);
			fprintf(gre_fd, "ip link set gre%d up\n", i);
			fprintf(gre_fd, "ip addr add %s dev gre%d\n", gre_localgw, i);
			fprintf(gre_fd, "ip route add %s/%s dev gre%d\n", gre_remote_net_ip, gre_remote_netmask, i);
			fprintf(gre_fd, "iptables -I INPUT -i gre%d -j ACCEPT\n", i);
		}
	}
#endif
#ifdef CLOUD_SUPPORT
	if(nvram_match("cloud_enable", "1"))
	{
		sprintf(gre_config, "%s", "gre_config1");
		strcpy(gre_config_setting, nvram_safe_get(gre_config));
		if(!strcmp(gre_config_setting, gre_config))
			return -1;
		if(gre_config_setting[0] == '0')
			return -1;
		if(gre_config_sep(gre_config_setting))
			return -1;
		
		fprintf(gre_fd, "ip tunnel add GRE mode gre remote %s local %s ttl 255\n", gre_remote_gateway_ip, gre_localgw);
		fprintf(gre_fd, "ip link set GRE up\n");
		fprintf(gre_fd, "ip addr add %s dev GRE\n", gre_localgw);
		fprintf(gre_fd, "ip route add %s/%s dev GRE\n", gre_remote_net_ip, gre_remote_netmask);
		fprintf(gre_fd, "iptables -I INPUT -i GRE -j ACCEPT\n");
		cloud_gre_start = 1;
	}
#endif

	fclose(gre_fd);
	eval("/bin/sh", GRE_CONF);
	if(cloud_gre_start == 1)
	{
		stop_dhcpd();
		start_dhcpd();
	}
	return 0;
}

int stop_gre()
{
	int i;
	char gre_config[15];
	char gre_config_setting[100], *pre;
	FILE *gre_fd;
	if((gre_fd = fopen(GRE_DECONF, "w")) == NULL)
	{
		printf("can't create %s\n", GRE_DECONF);
		return -1;
	}
	fprintf(gre_fd, "#!/bin/sh\n\n");
#ifdef GRE_SUPPORT
	if(nvram_match("cloud_enable", "0"))
	{
		for(i=1; i<=SUM_OF_GRE_ENTRY; i++)
		{
			fprintf(gre_fd, "ip link set gre%d down\n", i);
			fprintf(gre_fd, "ip tunnel del gre%d\n", i);
		}
	}
#endif
#ifdef CLOUD_SUPPORT
	if(nvram_match("cloud_enable", "1"))
	{
		fprintf(gre_fd, "ip link set GRE down\n");
		fprintf(gre_fd, "ip tunnel del GRE\n");
	}
#endif
	fclose(gre_fd);

	eval("/bin/sh", GRE_DECONF);
	return 0;
}
int start_gre_firewall()
{
	int i;
	char gre_config[15];
	char gre_config_setting[100];
	FILE *gre_fd;
	if((gre_fd = fopen("/tmp/gre_firewall", "w")) == NULL)
	{
		printf("can't create %s\n", "/tmp/gre_firewall");
		return -1;
	}
	fprintf(gre_fd, "#!/bin/sh\n\n");
#ifdef GRE_SUPPORT
	if(nvram_match("cloud_enable", "0"))
	{
		for(i=1; i<=SUM_OF_GRE_ENTRY; i++)
		{
			sprintf(gre_config, "%s%d", "gre_config", i);
			strcpy(gre_config_setting, nvram_safe_get(gre_config));
			if(!strcmp(gre_config_setting, gre_config))
				continue;
			if(gre_config_setting[0] == '0')
				continue;
			if(gre_config_sep(gre_config_setting))
				continue;
			fprintf(gre_fd, "iptables -I INPUT -i gre%d -j ACCEPT\n", i);
			fprintf(gre_fd, "iptables -I FORWARD -o gre%d -j ACCEPT\n", i);
			fprintf(gre_fd, "iptables -I FORWARD -s %s/%s -i gre%d -j ACCEPT\n", gre_remote_net_ip, gre_remote_netmask, i);
		}
	}
#endif
#ifdef CLOUD_SUPPORT
	if(nvram_match("cloud_enable", "1"))
	{
		sprintf(gre_config, "%s", "gre_config1");
		strcpy(gre_config_setting, nvram_safe_get(gre_config));
		if(!strcmp(gre_config_setting, gre_config))
			return -1;
		if(gre_config_setting[0] == '0')
			return -1;
		if(gre_config_sep(gre_config_setting))
			return -1;
		fprintf(gre_fd, "iptables -I INPUT -i GRE -j ACCEPT\n");
		fprintf(gre_fd, "iptables -I FORWARD -o GRE -j ACCEPT\n");
		fprintf(gre_fd, "iptables -I FORWARD -s %s/%s -i GRE -j ACCEPT\n", gre_remote_net_ip, gre_remote_netmask);
	}
#endif

	fclose(gre_fd);
#if CSH_DEBUG
	printf("before gre_firewall\n");
#endif
	eval("/bin/sh", "/tmp/gre_firewall");
#if CSH_DEBUG
	printf("after gre_firewall\n");
#endif

	return 0;
}

