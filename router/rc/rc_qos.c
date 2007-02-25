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

#define QOS_CONF "/tmp/qos.conf"
#define QOS_DECONF "/tmp/qos.deconf"

//#define ROOT_PRIO 0
#define EF_PRIO 0
#define AF1_PRIO 1 
#define AF2_PRIO 2
#define AF3_PRIO 3
#define AF4_PRIO 4
#define BE_PRIO 5

//#define ZB_DEBUG

int up_rate/*,ef_rate*/,af1_rate,af2_rate,af3_rate,af4_rate,be_rate;
char qos_appport[5][8];

int init_rate()
{
	//for upstream QoS
	up_rate = sar_getstats("US Connection Rate:");
	af1_rate = up_rate * 0.8 ;
	af2_rate = up_rate * 0.1;
	af3_rate = up_rate * 0.05;
	af4_rate = up_rate * 0.04;
	be_rate = up_rate - af1_rate - af2_rate - af3_rate - af4_rate;
	return 0;
}

int init_application()
{
	int i;
	
	for(i = 0;i < 8;i++)
	{
		memset(qos_appport[i],0,sizeof(qos_appport[i]));
			
	}
		
	sprintf(qos_appport[0],"%s","20");
	sprintf(qos_appport[1],"%s","80");
	sprintf(qos_appport[2],"%s","23");
	sprintf(qos_appport[3],"%s","25");
	sprintf(qos_appport[4],"%s","110");
	sprintf(qos_appport[5],"%s",nvram_safe_get("qos_spec_port0"));
	sprintf(qos_appport[6],"%s",nvram_safe_get("qos_spec_port1"));
	sprintf(qos_appport[7],"%s",nvram_safe_get("qos_spec_port2"));
	return 0;
}

int do_qos(FILE *qos_fd,char *wanif)
{
	int k;
	char qos_appname[10];
	
	fprintf(qos_fd,"/usr/sbin/tc qdisc del dev %s root 2> /dev/null > /dev/null\n",wanif);
	fprintf(qos_fd,"echo \"root PQ\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s root handle 1: prio\n",wanif);
	fprintf(qos_fd,"echo \"PQ 1:1\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 1:1 handle 110:0 dsmark indices 8\n", wanif);
	fprintf(qos_fd,"/usr/sbin/tc class change dev %s classid 110:1 dsmark mask 0x3 value 0x80\n",wanif);
	fprintf(qos_fd,"echo \"change tos of EF\"\n");

	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 1:2 handle 2:0 htb default 50\n",wanif);
	fprintf(qos_fd,"echo \"HTB for non-EF\"\n");
	fprintf(qos_fd,"/usr/sbin/tc class  add dev %s parent 2:0 classid 2:1 htb ceil %dkbit rate %dkbit\n",wanif,up_rate,up_rate);
	fprintf(qos_fd,"echo \"HTB parent class\"\n");
	fprintf(qos_fd,"/usr/sbin/tc class add dev %s parent 2:1 classid 2:10 htb rate %dkbit ceil %dkbit prio %d quantum %d burst 6k\n",wanif,up_rate,af1_rate,AF1_PRIO, AF1_PRIO);
	fprintf(qos_fd,"echo \"HTB AF1\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:10 handle 210:0 dsmark indices 8\n", wanif);
	fprintf(qos_fd,"/usr/sbin/tc class change dev %s classid 210:1 dsmark mask 0x3 value 0x60\n",wanif);
	fprintf(qos_fd,"echo \"change tos of AF1\"\n");

	fprintf(qos_fd,"/usr/sbin/tc class add dev %s parent 2:1 classid 2:20 htb rate %dkbit ceil %dkbit prio %d quantum %d burst 6k\n",wanif,up_rate,af2_rate,AF2_PRIO, AF2_PRIO);
	fprintf(qos_fd,"echo \"HTB AF2\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:20 handle 220:0 dsmark indices 8\n", wanif);
	fprintf(qos_fd,"/usr/sbin/tc class change dev %s classid 220:1 dsmark mask 0x3 value 0x40\n",wanif);
	fprintf(qos_fd,"echo \"change tos of AF2\"\n");

	fprintf(qos_fd,"/usr/sbin/tc class add dev %s parent 2:1 classid 2:30 htb rate %dkbit ceil %dkbit prio %d quantum %d burst 6k\n",wanif,up_rate,af3_rate,AF3_PRIO, AF3_PRIO);
	fprintf(qos_fd,"echo \"HTB AF3\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:30 handle 230:0 dsmark indices 8\n", wanif);
	fprintf(qos_fd,"/usr/sbin/tc class change dev %s classid 230:1 dsmark mask 0x3 value 0x20\n",wanif);
	fprintf(qos_fd,"echo \"change tos of AF3\"\n");

	fprintf(qos_fd,"/usr/sbin/tc class add dev %s parent 2:1 classid 2:40 htb rate %dkbit ceil %dkbit prio %d quantum %d burst 6k\n",wanif,up_rate,af4_rate,AF4_PRIO, AF4_PRIO);
	fprintf(qos_fd,"echo \"HTB AF4\"\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:40 handle 240:0 dsmark indices 8\n", wanif);
	fprintf(qos_fd,"/usr/sbin/tc class change dev %s classid 240:1 dsmark mask 0x3 value 0xa0\n",wanif);
	fprintf(qos_fd,"echo \"change tos of AF4\"\n");

	fprintf(qos_fd,"/usr/sbin/tc class add dev %s parent 2:1 classid 2:50 htb rate %dkbit ceil %dkbit prio %d quantum %d burst 6k\n",wanif,up_rate,be_rate,BE_PRIO, BE_PRIO);
	fprintf(qos_fd,"echo \"HTB BE!\"\n");
	
/*	fprintf(qos_fd,"\n#### Attach SFQ to each class #####\n");
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:10 handle 10: sfq perturb 10\n",wanif);
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:20 handle 20: sfq perturb 10\n",wanif);
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:30 handle 30: sfq perturb 10\n",wanif);
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:40 handle 40: sfq perturb 10\n",wanif);
	fprintf(qos_fd,"/usr/sbin/tc qdisc add dev %s parent 2:50 handle 50: sfq perturb 10\n",wanif);*/

	fprintf(qos_fd,"\n######Filters#########\n");

	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 110:0 protocol ip u32 match ip src 0.0.0.0/0 flowid 110:1\n",wanif);	
	
	memset(qos_appname,0,sizeof(qos_appname));
	for(k = 0;k < 8;k++)
	{
		sprintf(qos_appname,"qos_app%d",k);
		if(!strcmp(qos_appport[k],"0"))
			continue;
			
		switch(atoi(nvram_safe_get(qos_appname)))
		{
			case 2:
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 1:2\n",wanif,AF1_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 2:10\n",wanif,AF1_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 210:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 210:1\n",wanif,AF1_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 1:2\n",wanif,AF1_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 2:10\n",wanif,AF1_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 210:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 210:1\n",wanif,AF1_PRIO,qos_appport[k]);	
				break;
			case 1:
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 1:2\n",wanif,AF3_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 2:30\n",wanif,AF3_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 230:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 230:1\n",wanif,AF3_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 1:2\n",wanif,AF3_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 2:30\n",wanif,AF3_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 230:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 230:1\n",wanif,AF3_PRIO,qos_appport[k]);	
				break;
			case 0:
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 1:2\n",wanif,BE_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip sport %s 0xffff flowid 2:50\n",wanif,BE_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 1:2\n",wanif,BE_PRIO,qos_appport[k]);	
				fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip dport %s 0xffff flowid 2:50\n",wanif,BE_PRIO,qos_appport[k]);	
				break;
			default:
				break;
		}
	}
	
	/* put the traffice with DSCP into appropriate queues*/

	/* EF  */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0x80 0xe0 flowid 1:1\n",wanif,EF_PRIO);
	/* AF Class 1 */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0x60 0xe0 flowid 1:2\n",wanif,AF1_PRIO);
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip tos 0x60 0xe0 flowid 2:10\n",wanif,AF1_PRIO);
	/* AF Class 2 */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0x40 0xe0 flowid 1:2\n",wanif,AF2_PRIO);
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip tos 0x40 0xe0 flowid 2:20\n",wanif,AF2_PRIO);
	/* AF Class 3 */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0x20 0xe0 flowid 1:2\n",wanif,AF3_PRIO);
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip tos 0x20 0xe0 flowid 2:30\n",wanif,AF3_PRIO);
	/* AF Class 4 */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0xa0 0xe0 flowid 1:2\n",wanif,AF4_PRIO);
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip tos 0xa0 0xe0 flowid 2:40\n",wanif,AF4_PRIO);
	/* BE Class  */
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 1:0 protocol ip prio %d u32 match ip tos 0x0 0xe0 flowid 1:2\n",wanif,BE_PRIO);
	fprintf(qos_fd,"/usr/sbin/tc filter add dev %s parent 2:0 protocol ip prio %d u32 match ip tos 0x0 0xe0 flowid 2:50\n",wanif,BE_PRIO);

	if(strcmp(nvram_safe_get("qos_adv"),"0"))
		fprintf(qos_fd,"\nifconfig %s mtu %s\n",wanif,nvram_safe_get("qos_size"));
	return 0;
}

int bind_qos(char *bindif)
{
	FILE *bind_fp;

	if((bind_fp = fopen("/tmp/qos_bind","w")) == NULL)
	{
		printf("Create /tmp/qos_bind failed!\n");
		return -1;
	}

	fprintf(bind_fp,"#!/bin/sh\n");
	fprintf(bind_fp,"/usr/sbin/tc qdisc del dev %s root 2> /dev/null > /dev/null\n",bindif);
	fprintf(bind_fp,"/usr/sbin/tc qdisc add dev %s root handle 1:0 prio 2> /dev/null > /dev/null\n",bindif);
	fclose(bind_fp);
	eval("/bin/sh","/tmp/qos_bind");
	return 0;
}

int start_qos()
{
#if 1
	if(!strcmp(nvram_safe_get("qosSW"),"1"))
	{
		FILE *qos_fd;
		char read_buf[100];
		char wanif[8][6];
		int selectable,i,j,wanconn_exist = 0,b_ppp[8];

		memset(wanif,0,sizeof(wanif));
		//for getting active wan_ifname
		if(!file_to_buf(WAN_IFNAME, read_buf, sizeof(read_buf)))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
			return 0;
		}
		sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[0], wanif[1], wanif[2], wanif[3], wanif[4], wanif[5], wanif[6], wanif[7]);
	
		/*if(atoi(nvram_safe_get("wan_active_connection")) == 8)
		{
			printf("NO WAN Connection!\n");
			return -1;
		}*/

		for(i = 0;i < 8;i++)	
		{
			b_ppp[i] = 0;

			if(strcmp(wanif[i],"Down"))
			{
				//char temp[80];
				//memset(temp,0,sizeof(temp));
				wanconn_exist = 1;
				//sprintf(temp,"%s",wanif[i]);
				//printf("***execute del before add!\n");
				//eval("/usr/sbin/tc","qdisc","del","dev",temp,"root");
			}
		}
		
		if(!wanconn_exist)
		{
			printf("NO WAN Connection!\n");
			return -1;
		}
		
		init_application();

		init_rate();

		if((qos_fd = fopen(QOS_CONF,"w")) == NULL)
		{
			printf("Can't create %s!\n",QOS_CONF);	
			return -1;
		}

		fprintf(qos_fd,"#!/bin/sh\n\n");

		fprintf(qos_fd,"###### Upstream side #########\n");
	
		for(j = 0;j < 8;j++)
		{
			selectable = 0;
			
			if(strcmp(wanif[j], "Down"))
			{
				if(strstr(wanif[j], "nas"))
				{
					selectable = 1;
				}

				else if(strstr(wanif[j], "ppp"))
				{
					if(check_ppp_link(j) && check_ppp_proc(j))
					{
						selectable = 1;
						b_ppp[j] = 1;

					}
				}
			}
			
			if(selectable)
			{
				printf("for MPVC!\n");
				do_qos(qos_fd,wanif[j]);
				if(b_ppp[j] == 1)	
				{
					char bindif[8];
					memset(bindif,0,sizeof(bindif));
					sprintf(bindif,"nas%d",j);
					if(bind_qos(bindif))
						printf("Bind_qos error!\n");
				}
			}
		}

#ifdef MPPPOE_SUPPORT
		memset(wanif, 0, sizeof(wanif));
		if(!file_to_buf(WAN_MPPPOE_IFNAME, read_buf, sizeof(read_buf)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
		        return 0;
		}
		sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[0], wanif[1], wanif[2], wanif[3], wanif[4], wanif[5], wanif[6], wanif[7]);  
	
		for(i = 0 ; i < 8; i++)
		{
			selectable = 0;
			//which_to_select = 0;
			if(strcmp(wanif[i], "Down"))
			{
				if(strstr(wanif[i], "ppp"))
				{
					if(check_ppp_link(i + 8) && check_ppp_proc(i + 8))
					{
						selectable = 1;
						//which_to_select = (1 << 3) | i | PPP_FLAGS;   //ppp flag
					}
				}
			}
			#ifdef IPPPOE_SUPPORT
			else
			{
				char *next;
				int which = i;
				memset(wanif[i], 0, sizeof(wanif[i]));
				if(!file_to_buf(WAN_IPPPOE_IFNAME, read_buf, sizeof(read_buf)))
				{
					printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
		       		 	return 0;
				}
				memset(wanif[i], 0, sizeof(wanif[i]));
				foreach(wanif[i], read_buf, next)
				{
					if(which -- == 0)
						break;
					memset(wanif[i], 0, sizeof(wanif[i]));
				}
				if(strstr(wanif[i], "ppp"))
				{
					if(check_ppp_link(i + 8) && check_ppp_proc(i + 8))
					{
						selectable = 1;
						//which_to_select = (1 << 4) | i | PPP_FLAGS;   //ppp flag
					}
				}
			}
		#endif		
		if(selectable)
		{
			printf("for MPPPOE!\n");
			do_qos(qos_fd,wanif[i]);	
		}
	}
#endif	

		if(strcmp(nvram_safe_get("qos_vlan"),"0"))
		{
			//char qos_vid[8],qos_vlanip[20],tempbuf[20];
			char qos_vid[8],tempbuf[20],lanip[20],lanmask[20];
			memset(qos_vid,0,sizeof(qos_vid));
			memset(tempbuf,0,sizeof(tempbuf));
			memset(lanip,0,sizeof(lanip));
			memset(lanmask,0,sizeof(lanmask));
			//memset(qos_vlanip,0,sizeof(qos_vlanip));
			strcpy(qos_vid,nvram_safe_get("qos_vid"));
			strcpy(lanip,nvram_safe_get("lan_ipaddr"));
			strcpy(lanmask,nvram_safe_get("lan_netmask"));
			//strcpy(qos_vlanip,nvram_safe_get("qos_vidip"));
			fprintf(qos_fd,"\nvconfig add br0 %s\n",qos_vid);
			//fprintf(qos_fd,"ifconfig br0.%s %s up\n",qos_vid,qos_vlanip);
			fprintf(qos_fd,"ifconfig br0.%s %s netmask %s up\n",qos_vid,lanip, lanmask);
			sprintf(tempbuf,"br0.%s",qos_vid);
			eval("iptables","-I","FORWARD","-i",tempbuf,"-j","ACCEPT");
		}
		fclose(qos_fd);
		//sleep(2);
		eval("/bin/sh",QOS_CONF);
		//sleep(3);
		
	}	
	return 0;
#endif
}

int restore_mtu(int i,char *wanif)
{
	char word[16], *next, mtu_auto[3], mtu_value[8];
	char tmpbuf[6];
	int which = i;

	memset(tmpbuf,0,sizeof(tmpbuf));
	
	//printf("enter restore_mtu!\n");

	foreach(word, nvram_safe_get("mtu_config"), next)
	{
		if(which -- == 0)
		{
			sscanf(word, "%3[^:]:%10[^\n]", mtu_auto, mtu_value);
			break;
		}
	}
	
	if(strstr(wanif, "ppp")) 
	{
		if(!strcmp(mtu_auto, "1"))     //Auto config
		{
#if COUNTRY == JAPAN
                       	strcpy(tmpbuf,"1454"); 
			eval("ifconfig",wanif,"mtu",tmpbuf);
#else
                       	strcpy(tmpbuf,"1492"); 
			eval("ifconfig",wanif,"mtu",tmpbuf);
#endif
		}
                else  //mtu_auto == 0 ,Manual
		{ 
			eval("ifconfig",wanif,"mtu",mtu_value);
		}
	}
	else // for "nas" interface 
	{
		if(!strcmp(mtu_auto, "1")) //Auto config
		{
                       	strcpy(tmpbuf,"1500"); 
			eval("ifconfig",wanif,"mtu",tmpbuf);
		}
		else // Manual
		{
			printf("restore!\n");
			eval("ifconfig",wanif,"mtu",mtu_value);
                }
	}
	return 0;
}

int stop_qos()
{
#if 1
	char read_buf[100];
	char wanif[8][6];
	char mpif[8][6];
	char ipif[8][6];
	int i;
	FILE *q_fd;
	
	memset(wanif,0,sizeof(wanif));

	if(!file_to_buf(WAN_IFNAME, read_buf, sizeof(read_buf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return 0;
	}
	sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[0], wanif[1], wanif[2], wanif[3], wanif[4], wanif[5], wanif[6], wanif[7]);
		
		if((q_fd = fopen(QOS_DECONF,"w")) == NULL)
		{
			printf("Can't create %s\n",QOS_DECONF);
			return -1;
		}

#ifdef MPPPOE_SUPPORT
		{
			memset(mpif, 0, sizeof(mpif));
			if(!file_to_buf(WAN_MPPPOE_IFNAME, read_buf, sizeof(read_buf)))
			{
				printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
			        return 0;
			}
			sscanf(read_buf, "%s %s %s %s %s %s %s %s", mpif[0], mpif[1], mpif[2], mpif[3], mpif[4], mpif[5], mpif[6], mpif[7]);  
		}
#endif		

#ifdef IPPPOE_SUPPORT
		{ 
			memset(ipif, 0, sizeof(ipif));
			if(!file_to_buf(WAN_IPPPOE_IFNAME, read_buf, sizeof(read_buf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
			        return 0;
			}
			sscanf(read_buf, "%s %s %s %s %s %s %s %s", ipif[0], ipif[1], ipif[2], ipif[3], ipif[4], ipif[5], ipif[6], ipif[7]);  
		}
#endif		


		fprintf(q_fd,"#!/bin/sh\n\n");
		
		for(i = 0; i < 8;i++)
		{
			if(strcmp(wanif[i],"Down"))
			{
				fprintf(q_fd,"/usr/sbin/tc qdisc del dev %s root 2> /dev/null > /dev/null\n",wanif[i]);
				restore_mtu(i,wanif[i]);
			}

#ifdef MPPPOE_SUPPORT
			if(strcmp(mpif[i],"Down"))
			{
				fprintf(q_fd,"/usr/sbin/tc qdisc del dev %s root 2> /dev/null > /dev/null\n",mpif[i]);
				restore_mtu(i,wanif[i]);
			}
#endif			

#ifdef IPPPOE_SUPPORT
			if(strcmp(mpif[i],"Down"))
			{
				fprintf(q_fd,"/usr/sbin/tc qdisc del dev %s root 2> /dev/null > /dev/null\n",ipif[i]);
				restore_mtu(i,wanif[i]);
			}
#endif
		}

		if(strcmp(nvram_safe_get("qos_vlan"),"0"))
		{
			char *qos_vlanid,tempbuf[20];
		       	qos_vlanid = nvram_safe_get("qos_vid");
			fprintf(q_fd,"/usr/sbin/vconfig rem br0.%s 2> /dev/null > /dev/null\n",qos_vlanid);
			sprintf(tempbuf,"br0.%s",qos_vlanid);
			eval("iptables","-D","FORWARD","-i",tempbuf,"-j","ACCEPT");
		}
		
		fclose(q_fd);
		eval("/bin/sh",QOS_DECONF);
		stop_firewall();
		start_firewall();
		return 0;
#endif
}

int down_qos(char *wan_if,int which)
{
	int conn = which;
#if 0	
	char read_buf[120],wanif[16][6],*next;

	memset(wanif, 0, sizeof(wanif));
	if(which < MAX_CHAN)
	{
		if(!file_to_buf(WAN_IFNAME, read_buf, sizeof(read_buf)))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
			return 0;
		}
		sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[0], wanif[1], wanif[2], wanif[3], wanif[4], wanif[5], wanif[6], wanif[7]);

	}
#ifdef MPPPOE_SUPPORT
	else
	{
		if(!file_to_buf(WAN_MPPPOE_IFNAME, read_buf, sizeof(read_buf)))
		{
			printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
		        return 0;
		}
		sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[8], wanif[9], wanif[10], wanif[11], wanif[12], wanif[13], wanif[14], wanif[15]);  
		
	#ifdef IPPPOE_SUPPORT
		if(!strcmp(wanif[which],"Down"))
		{
			int which1 = which;
			if(!file_to_buf(WAN_IPPPOE_IFNAME, read_buf, sizeof(read_buf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
			        return 0;
			}
			
			foreach(wanif[which], read_buf, next)
			{
				if(which1 -- == 0)
					break;
				//memset(wanif[which], 0, sizeof(wanif[which]));
			}

			//sscanf(read_buf, "%s %s %s %s %s %s %s %s", wanif[8], wanif[9], wanif[10], wanif[11], wanif[12], wanif[13], wanif[14], wanif[15]);  

		}
	#endif

	}
#endif	
#endif		
	//if(strcmp(wanif[which],"Down"))
	if(strcmp(wan_if,"Down"))
	{
		eval("/usr/sbin/tc","qdisc","del","dev",wan_if,"root");		
		if(strcmp(nvram_safe_get("qos_adv"),"0"))
			restore_mtu(which,wan_if);
	}

	return 0;

}
