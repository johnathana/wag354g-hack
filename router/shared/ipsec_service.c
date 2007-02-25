/*
 * Ipsec service
 *
 * Copyright 2001-2003, Cybertan
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ipsec_service.c,v 1.1.1.3 2005/08/25 06:27:58 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <time.h>

#include <broadcom.h>
#include <netconf.h>
#include <shutils.h>
#include <code_pattern.h>
#include <wlutils.h>
#include <cy_conf.h>
#include <cymac.h>
#include "ipsec_service.h"

//status file name
#define WAN_IFNAME  "/tmp/status/wan_ifname"
#define WAN_IPADDR  "/tmp/status/wan_ipaddr"
#define WAN_NETMASK  "/tmp/status/wan_netmask"
#define WAN_GATEWAY  "/tmp/status/wan_gateway"
#define WAN_BROADCAST  "/tmp/status/wan_broadcast"
#define WAN_GET_DNS "/tmp/status/wan_get_dns"
#define WAN_HWADDR  "/tmp/status/wan_hwaddr"
#define WAN_PVC_STATUS "/tmp/status/wan_pvc_status"
#define WAN_PVC_ERRSTR "/tmp/status/wan_pvc_errstr"

#define IPSEC_START_FILE "/tmp/ipsec"
#ifdef  NOMAD_SUPPORT
#define PRESHARED_FILE "/tmp/ipsec_secrets/"//zhs add for quickVPN,3-23-2005
#else
#define PRESHARED_FILE "/tmp/ipsec.secrets"
#endif
#define MANUALKEY_FILE "/tmp/maual"
#define AUTOKEY_FILE "/tmp/auto"
#define CONNECT_FILE "/tmp/connect"
#define DISCONNECT_FILE "/tmp/disconnect"
#define STATUS_FILE "/tmp/ipsec_status"

//for dns resolve
#define DNSsz 12
#ifdef ETIMEDOUT
#undef ETIMEDOUT
#endif
#define ETIMEDOUT -12
#define NC2(val) (((val && 0xff) << 8) | ((unsigned short)val >> 8))
typedef struct {
	unsigned short id;
	unsigned short flags;
	unsigned short no_q;
	unsigned short no_ans;
	unsigned short no_auth;
	unsigned short no_add;
	unsigned char data[512];
}DNSstr;

char ipaddress[20];
char lsub_disconn[80];
char remote_client_disconn[80];

//char tunnel[3];
char ser_entry_config_buf[100];
char ser_rtype_config_buf[100];
char ser_ltype_config_buf[100];
char ser_sg_config_buf[100];
char ser_keytype_config_buf[250];
char ser_ipsecadv_config_buf[200];
char ser_wan_ipaddress[20];
char ser_wan_netmask[20];
char ser_wan_broadcast[20];
char ser_wan_gateway[20];
char ser_wan_ifname[20];

/* zhangbin for local/remote id type in Cloud */
#ifdef CLOUD_SUPPORT
char ser_lidfqdn[60];
int ser_lidflg;
char ser_ridfqdn[60];
int ser_ridflg;
int lr_flg;
#endif

/*struct entry_config entry_config_ptr;
struct ltype_config ltype_config_ptr;
struct rtype_config rtype_config_ptr;
struct sg_config sg_config_ptr;
struct keytype_config keytype_config_ptr;*/

//#define ZB_DEBUG

#if 0
int DNSresolve(char *fullname, char *DNSname)  //fullname="www.abc.com" DNSname=dns server's ip
{
	int i1, i2, stat, len, reqtype;
	unsigned char cc, *cp, *cp2, *cp3;
	unsigned short xid;
	DNSstr DNSstr;
	struct sockaddr_in sock;
	unsigned char temp[20];
	struct sockaddr_in smtp_server_ip;
	int fd;

	reqtype = 1;
	
	srandom(time(NULL));
	xid = random();
#ifdef ZB_DEBUG
	printf("xid = %d\n", xid);
#endif
	if(!(fd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		printf("socket error\n");
		return -1;
	}	
	
	sock.sin_family = AF_INET;
	sock.sin_port = htons(53);
	inet_pton(AF_INET, DNSname, (struct in_addr *)&(sock.sin_addr));
#ifdef ZB_DEBUG
	printf("DNSname = %s\n", inet_ntoa(sock.sin_addr));
#endif
	if(connect(fd, (struct sockaddr *)(&sock), (socklen_t)sizeof(sock)) < 0)
	{
		printf("connect error\n");
		return -1;
	}	

	memset(&DNSstr, 0, sizeof(DNSstr));
	DNSstr.id = htons((unsigned short)xid);
	DNSstr.flags = htons(0x0100);
	DNSstr.no_q = NC2(1);

	cp = fullname;
	cp3 = DNSstr.data;
	cp2 = cp3 + 1;
	for(;;)
	{
		cc = *cp ++;
		if(cc == '.' || cc == 0)
		{
			*cp3 = cp2 - cp3 - 1;
			cp3 = cp2;
		}
		*cp2 ++ = cc;
		if(cc == 0)
			break;
	} 

	*cp2 ++ = 0; *cp2 ++ = reqtype;
	*cp2 ++ = 0; *cp2 ++ = 1;
	*cp2 = '\0';
	len = cp2 - (unsigned char *)&DNSstr;
	write(fd, &DNSstr, len);
read3:

	memset(&DNSstr, 0, sizeof(DNSstr));
	stat = read(fd, &DNSstr, sizeof(DNSstr));
	if(stat > DNSsz)
	{
		printf("DNSstr.id = %d\n", ntohs(DNSstr.id));
		if(DNSstr.id != htons(xid))
			goto read3;
		printf("DNSstr.flags = %d\n", DNSstr.flags);
		if((DNSstr.flags & ntohs(0x8000)) == 0) /*response datagram*/
			goto read3;
	}
	i1 = ntohs(DNSstr.flags);
	i1 &= 0x0f;
	if(i1 !=0) /* name error*/
		goto nextserv; /*query next dns server*/
	
	cp = (unsigned char *)&DNSstr + len;
#ifdef ZB_DEBUG
	printf("stat = %d -- len = %d\n", stat, len);
	{
		int i = 0;
		for(; i < (stat - len); i ++)
		{
			printf("%02x ", cp[i]);
		}
	}
#endif
aga2:
	for(;;)
	{
		if(stat < 2)
			break;

		cc = *cp ++;
		stat --;
		if(cc >= 0xc0)
		{
			cp ++;
			stat --;
			break;
		}
		if(cc == 0)
			break;
		cp += cc;
		stat -= cc;
		
	}

	if(stat < 10)
		goto nextserv;

	i1 = (cp[0] << 8) | cp[1];
	cp += 8;
	i2 = (cp[0] << 8) | cp[1];
	cp += 2;
	stat -= i2 + 10;
	if(stat < 0)
		goto nextserv;

	if(i1 != 1)
	{
		if(i1 == 12 && reqtype == 12)
		{
			;/*for(cp3 = cp; cp2 = fullname;;)*/
		}
		cp += i2;
		goto aga2;
	}
#define ZB_DEBUG
#ifdef ZB_DEBUG
	printf("%d:%d:%d:%d\n", cp[0], cp[1], cp[2], cp[3]);
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%d.%d.%d.%d", cp[0], cp[1], cp[2], cp[3]);
	inet_pton(AF_INET, temp, (struct in_addr *)&smtp_server_ip.sin_addr);
	strncpy(ipaddress,inet_ntoa(smtp_server_ip.sin_addr),sizeof(ipaddress));
	printf("sin_addr = %s\n", inet_ntoa(smtp_server_ip.sin_addr));
	printf("ipaddress = %s\n",ipaddress);
	
#endif
nextserv:
	return 2; /*for next dns server*/
	
	close(fd);

	return 0;		
}
#endif

int send_log(const char *message,...)
{
#if 0
    va_list args;
    char m[1024];	/* longer messages will be truncated */

    va_start(args, message);
    fmt_log(m, sizeof(m), message, args);
    va_end(args);

    syslog(LOG_CRIT, "%s", m);
#endif
	//log("message=%s",message);

	openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
	syslog(LOG_CRIT, "%s", message);
	closelog();
    	return 0;	
}

int ipsec_global_init(webs_t wp)
{
	char word[250],readbuf[256];
	char *next,*tunnel_l,*wan_ipaddr;
	int which,local_connection;

	#ifdef ZB_DEBUG
		printf("Enter ipsec_global_init!\n");
	#endif

	tunnel_l = websGetVar(wp,"tunnel_entry",NULL);

	if(tunnel_l == NULL)
		return -1;
	/*	
	wan_ipaddr = nvram_safe_get("wan_ipaddr");
	if(wan_ipaddr == NULL)
	{
		printf("wan interface is disabled!\n");
		return -1;
	}
	*/		
	which = atoi(tunnel_l);
	#ifdef ZB_DEBUG
		printf("which = %d\n",which);
	#endif

	foreach(word, nvram_safe_get("entry_config"), next) 
	{
		if (which-- == 0) 
		{
		sscanf(word, "%3[^:]:%25[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", ser_tunnel_status,ser_tunnel_name,ser_local_type, ser_localgw_conn,ser_remote_type, ser_sg_type, ser_enc_type, ser_auth_type, ser_key_type);
			
		#ifdef ZB_DEBUG
			printf("ser_tunnel_status=%s, ser_tunnel_name=%s,ser_local_type=%s,ser_localgw_conn=%s,ser_remote_type=%s,ser_sg_type=%s,ser_enc_type=%s,ser_auth_type=%s,ser_key_type=%s\n", ser_tunnel_status,ser_tunnel_name,ser_local_type,ser_localgw_conn,ser_remote_type,ser_sg_type,ser_enc_type,ser_auth_type,ser_key_type);
		#endif 
			break;
		}
	}
	
	memset(ser_ltype_config_buf, 0, sizeof(ser_ltype_config_buf));
	which = atoi(tunnel_l);

	foreach(ser_ltype_config_buf, nvram_safe_get("ltype_config"), next) 
	{
		if (which-- == 0) 
		{
	//		sscanf(ltype_config_buf,"%s:%s",l_ipaddr,l_netmask);
			sscanf(ser_ltype_config_buf, "%20[^:]:%20[^\n]", ser_l_ipaddr,ser_l_netmask);

		#ifdef ZB_DEBUG
			printf("ser_l_ipaddr=%s, ser_l_netmask=%s\n", ser_l_ipaddr, ser_l_netmask);
		#endif 
			break;
		}
	}
	
	memset(ser_rtype_config_buf, 0, sizeof(ser_rtype_config_buf));
	which = atoi(tunnel_l);
	
	foreach(ser_rtype_config_buf, nvram_safe_get("rtype_config"), next)
		{
			if (which-- == 0) 
			{
				sscanf(ser_rtype_config_buf, "%20[^:]:%20[^\n]", ser_r_ipaddr,ser_r_netmask);

			#ifdef ZB_DEBUG
				printf("ser_r_ipaddr=%s, ser_r_netmask=%s\n", ser_r_ipaddr, ser_r_netmask);
			#endif 
			
			break;
			}
		}
		
	memset(ser_sg_config_buf, 0, sizeof(ser_sg_config_buf));
	which = atoi(tunnel_l);
	
	foreach(ser_sg_config_buf, nvram_safe_get("sg_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(ser_sg_config_buf, "%20[^:]:%50[^\n]", ser_sg_ipaddr,ser_sg_domain);
			
		#ifdef ZB_DEBUG
			printf("ser_sg_ipaddr=%s,ser_sg_domain=%s\n",ser_sg_ipaddr,ser_sg_domain);
		#endif 
			break;
		}
	}
	
#ifdef CLOUD_SUPPORT

	{
		char tmp_lidcfg[5][3],ipsec_lid_cfg[12];
		memset(tmp_lidcfg,0,sizeof(tmp_lidcfg));
		memset(ipsec_lid_cfg,0,sizeof(ipsec_lid_cfg));
		memset(ser_lidfqdn,0,sizeof(ser_lidfqdn));
		ser_lidflg = 0;
		lr_flg = 0;
		
		strcpy(ipsec_lid_cfg,nvram_safe_get("ipsec_lid_cfg"));
		
		sscanf(ipsec_lid_cfg,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_lidcfg[0],tmp_lidcfg[1],tmp_lidcfg[2],tmp_lidcfg[3],tmp_lidcfg[4]);

		if(!strcmp(tmp_lidcfg[atoi(tunnel_l)],"1"))
		{

			char lid_buf[300],*lid_ptr = lid_buf;
			char *tmp_ptr;
			char lid_ip[18];
			int which_t,icnt;

			which_t = atoi(tunnel_l);
			memset(lid_buf,0,sizeof(lid_buf));
			memset(lid_ip,0,sizeof(lid_ip));
				
			strcpy(lid_buf,nvram_safe_get("lid_config"));
			for(icnt = 0;icnt < IPSEC_TUNNELS;icnt++)
			{
				tmp_ptr = strsep(&lid_ptr," ");
				if(icnt == which_t)
					sscanf(tmp_ptr, "%18[^:]:%60[^\n]", lid_ip, ser_lidfqdn);
			}

			ser_lidflg = 1;
		}

	}	

	{
		char tmp_ridcfg[5][3],ipsec_rid_cfg[12];
		memset(tmp_ridcfg,0,sizeof(tmp_ridcfg));
		memset(ipsec_rid_cfg,0,sizeof(ipsec_rid_cfg));
		memset(ser_ridfqdn,0,sizeof(ser_ridfqdn));
		ser_ridflg = 0;
		
		strcpy(ipsec_rid_cfg,nvram_safe_get("ipsec_rid_cfg"));
		
		sscanf(ipsec_rid_cfg,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_ridcfg[0],tmp_ridcfg[1],tmp_ridcfg[2],tmp_ridcfg[3],tmp_ridcfg[4]);
		
		if(!strcmp(tmp_ridcfg[atoi(tunnel_l)],"1"))
		{

			char rid_buf[300],*rid_ptr = rid_buf;
			char *tmp_ptr;
			char rid_ip[18];
			int which_t,icnt;

			which_t = atoi(tunnel_l);
			memset(rid_buf,0,sizeof(rid_buf));
			memset(rid_ip,0,sizeof(rid_ip));
				
			strcpy(rid_buf,nvram_safe_get("rid_config"));
			for(icnt = 0;icnt < IPSEC_TUNNELS;icnt++)
			{
				tmp_ptr = strsep(&rid_ptr," ");
				if(icnt == which_t)
					sscanf(tmp_ptr, "%18[^:]:%60[^\n]", rid_ip, ser_ridfqdn);
			}

			ser_ridflg = 1;
		}

		lr_flg = (ser_lidflg << 1) | ser_ridflg;

	}	
#endif	

	memset(ser_keytype_config_buf, 0, sizeof(ser_keytype_config_buf));	
	which = atoi(tunnel_l);
	
	foreach(ser_keytype_config_buf, nvram_safe_get("keytype_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(ser_keytype_config_buf, "%3[^:]:%100[^:]:%20[^:]:%100[^:]:%100[^:]:%100[^:]:%100[^\n]", ser_auto_pfs,ser_auto_presh,ser_auto_klife,ser_manual_enckey,ser_manual_authkey,ser_manual_ispi,ser_manual_ospi);
			
		#ifdef ZB_DEBUG
			printf(" ser_auto_pfs=%s,ser_auto_presh=%s,ser_auto_klife=%s,ser_manual_enckey=%s,ser_manual_authkey=%s,ser_manual_ispi=%s,ser_manual_ospi=%s\n", ser_auto_pfs,ser_auto_presh,ser_auto_klife,ser_manual_enckey,ser_manual_authkey,ser_manual_ispi,ser_manual_ospi);
		#endif 
			break;
		}
	}
	
	memset(ser_ipsecadv_config_buf, 0, sizeof(ser_ipsecadv_config_buf));
	which = atoi(tunnel_l);
	
	foreach(ser_ipsecadv_config_buf, nvram_safe_get("ipsecadv_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(ser_ipsecadv_config_buf, "%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^:]:%3[^:]:%10[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^\n]", ser_ipsec_opmode,ser_ipsecp1_enc,ser_ipsecp1_auth,ser_ipsecp1_group,ser_ipsecp1_klife,ser_ipsecp2_group,ser_ipsecp2_klife,ser_ipsec_netbios,ser_ipsec_antireplay,ser_ipsec_keepalive,ser_ipsec_blockip,ser_ipsec_retrytimes,ser_ipsec_blocksecs);
		#ifdef ZB_DEBUG
			printf("ipsec_opmode=%s,ipsecp1_enc=%s,ipsecp1_auth=%s,ipsecp1_group=%s,ipsecp1_klife=%s,ipsecp2_group=%s,ipsecp2_klife=%s,ipsec_netbios=%s,ipsec_antireplay=%s,ipsec_keepalive=%s,ipsec_blockip=%s,ipsec_retrytimes=%s,ipsec_blocksecs=%s\n", ser_ipsec_opmode,ser_ipsecp1_enc,ser_ipsecp1_auth,ser_ipsecp1_group,ser_ipsecp1_klife,ser_ipsecp2_group,ser_ipsecp2_klife,ser_ipsec_netbios,ser_ipsec_antireplay,ser_ipsec_keepalive,ser_ipsec_blockip,ser_ipsec_retrytimes,ser_ipsec_blocksecs);
		#endif 
			break;
		}
        
	}

	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_ifname"),next)
	foreach(word, readbuf, next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_ifname,word);
			printf("ser_wan_ifname=%s\n",ser_wan_ifname);
			break;
		}
	}
	
	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_ipaddr"),next)
	foreach(word, readbuf, next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_ipaddress,word);
			break;
		}
	}
	
	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_NETMASK, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_NETMASK);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_netmask"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_netmask,word);
			break;
		}
	}		
	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_BROADCAST, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_BROADCAST);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_broadcast"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_broadcast,word);
			break;
		}
	}

	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_GATEWAY);
		return 0;
	}
	//foreach(word,nvram_safe_get("wan_gateway"),next)
	foreach(word, readbuf,next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_gateway,word);
			break;
		}
	}

	
#if 0
	local_connection = atoi(ser_localgw_conn);
	if(!file_to_buf(WAN_BROADCAST, readbuf, sizeof(readbuf)))
	foreach(word,nvram_safe_get("wan_ifname"),next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_ifname,word);
			printf("ser_wan_ifname=%s\n",ser_wan_ifname);
			break;
		}
	}


	local_connection = atoi(ser_localgw_conn);
	foreach(word,nvram_safe_get("wan_ipaddr"),next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_ipaddress,word);
			break;
		}
	}
	
	local_connection = atoi(ser_localgw_conn);
	foreach(word,nvram_safe_get("wan_netmask"),next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_netmask,word);
			break;
		}
	}

	local_connection = atoi(ser_localgw_conn);
	foreach(word,nvram_safe_get("wan_broadcast"),next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_broadcast,word);
			break;
		}
	}

	local_connection = atoi(ser_localgw_conn);
	foreach(word,nvram_safe_get("wan_gateway"),next)
	{
		if(local_connection-- == 0 )
		{
			strcpy(ser_wan_gateway,word);
			break;
		}
	}
#endif
	return 0;
}

int start_ipsec()
{
	FILE *file;
	
	#ifdef ZB_DEBUG
		printf("start ipsec()!\n");
	#endif

	if((file = fopen(IPSEC_START_FILE,"w")) == NULL)
	{
		printf("Cannot open file /tmp/ipsec!\n");
		return -1;
	}
	
	fprintf(file,"#!/bin/sh\n");
	fprintf(file,"/bin/ipsec/klipsdebug --none\n");
	fprintf(file,"/bin/ipsec/eroute --clear\n");
	fprintf(file,"/bin/ipsec/spi --clear\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",ser_wan_ifname);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"/bin/ipsec/tncfg --attach --virtual ipsec0 --physical %s\n",ser_wan_ifname);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"ifconfig ipsec0 inet %s broadcast %s netmask %s\n",ser_wan_ipaddress,ser_wan_broadcast,ser_wan_netmask);
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"ifconfig ipsec0 mtu 1430\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/ipsec0/rp_filter\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I FORWARD -i ipsec0 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I INPUT -p 50 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I OUTPUT -p 50 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I INPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(file,"echo\n");
	fprintf(file,"echo\n");
	fprintf(file,"iptables -I OUTPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");

	fprintf(file,"echo\n");
	fprintf(file,"/bin/ipsec/pluto & \n");	
	fclose(file);
	
	printf("Starting PLUTO...........\n");
	eval("/bin/sh",IPSEC_START_FILE);
	
	return 0;
	
}

int connect_ipsec(webs_t wp) 
{
	FILE *disconn_fd,*ipt_post;
	char lsub[80],lsub_client[50],ser_sec_gw[40],ser_any_gw[10],remote[100],remote_client[50],enc_config[8],auth_config[8],p1enc_config[10],p1auth_config[10],p1group_config[10],ike_config[30],nexthop_config[20],ikemode_config[15],esp_config[20],retries[5];

	memset(ikemode_config,0,sizeof(ikemode_config));	
	memset(esp_config,0,sizeof(esp_config));	
	memset(retries,0,sizeof(retries));	
	if(ipsec_global_init(wp) < 0)
		return -1;
//zhangbin 20040409
	printf("step1\n");
	//terminate this conn first
	if((disconn_fd = fopen("/tmp/disconn","w")) == NULL)
	{
		printf("Cannot open /tmp/disconn!\n");
		return -1;
	}
	fprintf(disconn_fd,"#!/bin/sh\n");
	fprintf(disconn_fd,"/bin/ipsec/whack --name %s --terminate\n",ser_tunnel_name);
	fprintf(disconn_fd,"echo\n");
	//fprintf(disconn_fd,"echo\n");
	/* terminate twice may be cleaner */
	//fprintf(disconn_fd,"/bin/ipsec/whack --name %s --terminate\n",ser_tunnel_name); //zhangbin 2004.08.11
	//fprintf(disconn_fd,"echo\n");
	//fprintf(disconn_fd,"echo\n");
	fclose(disconn_fd);
	eval("/bin/sh","/tmp/disconn");

	//zhangbin 20040409
#ifdef ZB_DEBUG
	printf("step2\n");
#endif

	if(!strcmp(ser_tunnel_status,"0"))
	{
		printf("Tunnel Status is Disabled,no need to start ipsec!\n");
		{
			char errmsg[100];
			char *id = websGetVar(wp,"tunnel_entry",NULL);
			memset(errmsg,0,sizeof(errmsg));
			sprintf(errmsg,"2rIKE[\"%s\"] ERROR: Tunnel %d is not enabled!",ser_tunnel_name,(atoi(id)+1));
			send_log(errmsg);
		}
		return -1;
	}
	
	if(!strcmp(ser_local_type,"0"))
	{
		sprintf(lsub,"--client %s/32",ser_l_ipaddr); //for auto key
		sprintf(lsub_client," %s/32",ser_l_ipaddr); //for manual key
	}
	else if(!strcmp(ser_local_type,"1"))
	{
		sprintf(lsub,"--client %s/%s",ser_l_ipaddr,ser_l_netmask);
		sprintf(lsub_client,"%s/%s",ser_l_ipaddr,ser_l_netmask);
	}

	if(!strcmp(ser_ipsec_opmode,"0"))
	{
		sprintf(ikemode_config,"%s","");
	}
	else if(!strcmp(ser_ipsec_opmode,"1"))
		sprintf(ikemode_config,"%s","--aggrmode");	

	strcpy(nexthop_config,"\"%direct\""); //for non-%any
	memset(ser_any_gw,0,sizeof(ser_any_gw));
	if(!strcmp(ser_sg_type,"0"))
	{
		strncpy(ser_sec_gw,ser_sg_ipaddr,sizeof(ser_sec_gw));
	}
	else if(!strcmp(ser_sg_type,"1")) //security gateway is "any"
	{
		strncpy(ser_sec_gw,"\"%any\"",sizeof(ser_sec_gw));
		strncpy(ser_any_gw,"%any",sizeof(ser_any_gw));
		//strcpy(nexthop_config,"--nexthop \"%direct\"");
	}
	else if(!strcmp(ser_sg_type,"2")) //security gateway is "domain"
	{
		char temp[50];
		struct hostent *gw_host;
		
		gw_host = gethostbyname(ser_sg_domain);

		if(NULL == gw_host)
		{
			printf("Cannot resolve domain: %s\n",ser_sg_domain);
			{
				char gwerr[100];
				memset(gwerr,0,sizeof(gwerr));
				sprintf(gwerr,"2rIKE[\"%s\"] ERROR: Remote Security Gateway domain name problem!\n",ser_tunnel_name);
				send_log(gwerr);
			}
			return -1;
		}
	
		sprintf(temp, "%d.%d.%d.%d", 
			(unsigned char)gw_host->h_addr_list[0][0],
			(unsigned char)gw_host->h_addr_list[0][1],
			(unsigned char)gw_host->h_addr_list[0][2],
			(unsigned char)gw_host->h_addr_list[0][3]
			);
		strncpy(ser_sec_gw,temp,sizeof(ser_sec_gw));
	}	

	//zhangbin 20040409
#ifdef ZB_DEBUG	
	printf("step3\n");
#endif
	
	if(!strcmp(ser_remote_type,"0"))  //remote secure group is Ip addr.
	{
		sprintf(remote_client,"%s/32",ser_r_ipaddr);
		sprintf(remote,"--client %s/32",ser_r_ipaddr);
	}
			
	if(!strcmp(ser_remote_type,"1")) //remote secure group is subnet.
	{
		sprintf(remote_client,"%s/%s",ser_r_ipaddr,ser_r_netmask);
		sprintf(remote,"--client %s/%s",ser_r_ipaddr,ser_r_netmask);
	}	
	
	if(!strcmp(ser_remote_type,"4")) //remote secure group is host.
	{
		sprintf(remote_client,"--host %s ",ser_sec_gw);
		strcpy(remote,"");
	}	
	
	if(!strcmp(ser_remote_type,"3")) //remote secure group is any.
	{
		sprintf(remote_client,"0.0.0.0/0.0.0.0");
		sprintf(remote,"--client 0.0.0.0/0.0.0.0");
		
	}
	
	if(!strcmp(ser_enc_type,"1"))
	{
		strcpy(enc_config,"des"); 
	}
	else if(!strcmp(ser_enc_type,"2"))
	{
		strcpy(enc_config,"3des"); 
	}
	else if(!strcmp(ser_enc_type,"3"))
	{
		strcpy(enc_config,"aes"); 
	}
	else if(!strcmp(ser_enc_type,"0"))
	{
		strcpy(enc_config,"null"); 
	}
	
	if(!strcmp(ser_auth_type,"1"))
	{
		strcpy(auth_config,"md5"); 
	}
	else if(!strcmp(ser_auth_type,"2"))
	{
		strcpy(auth_config,"sha1"); 
	}

	sprintf(esp_config,"%s-%s-96",enc_config,auth_config);	
	if(!strcmp(ser_ipsecp1_enc,"1"))
	{
		strcpy(p1enc_config,"des"); 
	}
	else 
	{
		strcpy(p1enc_config,"3des"); 
	}
	
	if(!strcmp(ser_ipsecp1_auth,"1"))
	{
		strcpy(p1auth_config,"md5"); 
	}
	else 
	{
		strcpy(p1auth_config,"sha"); 
	}
	
	if(!strcmp(ser_ipsecp1_group,"1"))
	{
		strcpy(p1group_config,"modp768"); 
	}
	else 
	{
		strcpy(p1group_config,"modp1024"); 
	}
	
	sprintf(ike_config,"%s-%s-%s",p1enc_config,p1auth_config,p1group_config);
	sprintf(lsub_disconn,"%s",lsub);
	sprintf(remote_client_disconn,"%s",remote_client_disconn);
	if(!strcmp(ser_ipsec_blockip,"1"))
	{
		sprintf(retries,"%s",ser_ipsec_retrytimes);
	}
	else
		sprintf(retries,"%s","8");
	//re-start pluto is no need now
#if 0
	/* If pluto exit,we should try to start it again before connect */	
	//if((stat("/var/run/pluto.pid",errbuf)) != 0)
	if((fopen("/var/run/pluto.pid","r")) == NULL)
	{
		#ifdef ZB_DEBUG
			printf("process pluto stopped!\n");
		#endif
		if(start_ipsec() < 0)
		{
			printf("Start Pluto Failed!\n");
			return -1;
		}
		if((ipt_post = fopen("/tmp/ipt_pos","w")) == NULL)
		{
			printf("Cannot add NAT-excluded rule!\n");
			//	return -1;
		}
		fprintf(ipt_post,"iptables -t nat -I POSTROUTING -s %s -d %s -j ACCEPT\n",lsub,remote_client);
		fclose(ipt_post);
		eval("/bin/sh","/tmp/ipt_pos");
	}
				
#endif	
	if(!strcmp(ser_key_type,"0"))
	{
		FILE *afd,*prefd,*status_fd,*fd_psk; //auto key fd and preshared key fd
		char pfs_config[10];
		//char st_file[50],read_buf[3],ser_pskbuf[512],ser_ip1[20],ser_ip2[20],ser_psk[100];
		char st_file[50],read_buf[3],ser_pskbuf[512],ser_ip1[60],ser_ip2[60],ser_psk[150];
		int ser_find = 0;
	#ifdef NOMAD_SUPPORT
		char psk_fn[50];
		memset(psk_fn,0,sizeof(psk_fn));
	#endif						
		#ifdef ZB_DEBUG
			printf("enter auto key!\n");
		#endif
		
		sprintf(st_file,"/tmp/ipsecst/%s",ser_tunnel_name);
		//status file would be created after show_status,so before re-connecting we should check the file.	
		if((status_fd = fopen(st_file,"r")) != NULL)
		{
			fread(read_buf,1,1,status_fd);
			if(!strcmp(read_buf,"1"))
				return 0;
		}
				
		if(!strcmp(ser_auto_pfs,"1"))
			strcpy(pfs_config,"--pfs");
		else
			strcpy(pfs_config,"");
	#ifdef NOMAD_SUPPORT
	{
		sprintf(psk_fn,"%s%s_fix.secret",PRESHARED_FILE,ser_tunnel_name);
		/*if((fd_psk = fopen(psk_fn,"r")) == NULL)
		{
			printf("ipsec.secrets has not been created!\n");
		} ZHS Remove for open no used,2005-6-29*/
	}
	#else
		if((fd_psk = fopen(PRESHARED_FILE,"r")) == NULL)		
		{
			printf("ipsec.secrets has not been created!\n");
			goto write_psk;
		}
		memset(ser_pskbuf,0,sizeof(ser_pskbuf));
		while(fgets(ser_pskbuf,512,fd_psk))
		{

			#ifdef CLOUD_SUPPORT
			if(lr_flg == 0x1) //local id is ip,remote id is fqdn
				sscanf(ser_pskbuf,"%s @%s : PSK %s",ser_ip1,ser_ip2,ser_psk);
			else if(lr_flg == 0x2) //local id is fqdn,remote id is ip
				sscanf(ser_pskbuf,"@%s %s : PSK %s",ser_ip1,ser_ip2,ser_psk);
			else if(lr_flg == 0x3) //local id is fqdn,remote id is ip
				sscanf(ser_pskbuf,"@%s @%s : PSK %s",ser_ip1,ser_ip2,ser_psk);
			else
			#endif

			sscanf(ser_pskbuf,"%s %s : PSK %s",ser_ip1,ser_ip2,ser_psk);
			#ifdef CLOUD_SUPPORT
			//if(ser_ridflg == 1)
			if(lr_flg == 0x1) //local id is ip,remote id is fqdn
			{
				if((!strcmp(ser_ip2,ser_ridfqdn)) && !strcmp(ser_ip1,ser_wan_ipaddress))
				{
					ser_find = 1;
					break;
				}
			}

			else if(lr_flg == 0x2) //local id is fqdn,remote id is ip
			{
				if((!strcmp(ser_ip2,ser_sec_gw)) && !strcmp(ser_ip1,ser_lidfqdn))
				{
					ser_find = 1;
					break;
				}
			}
			else if(lr_flg == 0x3) //local id is fqdn,remote id is fqdn
			{
				if((!strcmp(ser_ip2,ser_ridfqdn)) && !strcmp(ser_ip1,ser_lidfqdn))
				{
					ser_find = 1;
					break;
				}
			}

			else
			#endif

			if((!strcmp(ser_ip2,ser_sec_gw) || !strcmp(ser_ip2,ser_any_gw)) && !strcmp(ser_ip1,ser_wan_ipaddress))
			{
				ser_find = 1;
				break;
			}
		}
		fclose(fd_psk);
		if(ser_find == 1)
		{
			printf("Item is found in ipsec.secrets!\n");
		}
		else
        #endif
		{

write_psk:
			printf("Item will be added in ipsec.secrets!\n");	
		#ifdef NOMAD_SUPPORT
			if((prefd = fopen(psk_fn,"w")) == NULL)
		#else
			if((prefd = fopen(PRESHARED_FILE,"a+")) == NULL)
		#endif
			{
				printf("cannot open ipsec.secrets!\n");
				return -1;
			}
		#ifdef CLOUD_SUPPORT
			//if(ser_ridflg == 1)
			if(lr_flg == 0x1) //local id is ip,remote id is fqdn
			{
				fprintf(prefd,"%s @%s : PSK \"%s\"\n",ser_wan_ipaddress,ser_ridfqdn,ser_auto_presh);
			}
			else if(lr_flg == 0x2) //local id is fqdn,remote id is ip
			{
				fprintf(prefd,"@%s %s : PSK \"%s\"\n",ser_lidfqdn,ser_sec_gw,ser_auto_presh);
			}
			else if(lr_flg == 0x3) //local id is fqdn,remote id is fqdn
			{
				fprintf(prefd,"@%s @%s : PSK \"%s\"\n",ser_lidfqdn,ser_ridfqdn,ser_auto_presh);
			}
			else

		#endif

			if(!strcmp(ser_sg_type,"1"))
			{	
				fprintf(prefd,"%s %s : PSK \"%s\"\n",ser_wan_ipaddress,ser_any_gw,ser_auto_presh);
			}
			else
				fprintf(prefd,"%s %s : PSK \"%s\"\n",ser_wan_ipaddress,ser_sec_gw,ser_auto_presh);

			fclose(prefd);
		}
			
		if((afd = fopen(AUTOKEY_FILE,"w")) == NULL)
		{
			printf("cannot open auto key file!\n");
			return -1;
		}
				
		fprintf(afd,"#!/bin/sh\n");
		//zhangbin 2004.08.11
		//fprintf(afd,"echo\n");
		//fprintf(afd,"/bin/ipsec/whack --name %s --encrypt --tunnel --esp %s --ike %s %s --psk --host %s %s --nexthop %s --to --host %s %s --nexthop %s %s --ikelifetime %s --ipseclifetime %s --rekeywindow 540 --keyingtries %s\n",ser_tunnel_name,esp_config,ike_config,pfs_config,ser_wan_ipaddress,lsub,ser_wan_gateway,ser_sec_gw,remote,nexthop_config,ikemode_config,ser_ipsecp1_klife,ser_auto_klife,retries);
		//if no re-start pluto,we put ipt rules here.
		//fprintf(afd,"iptables -t nat -I POSTROUTING -s %s -d %s -j ACCEPT\n",lsub,remote_client);
		//fprintf(afd,"echo\n");
	//	fprintf(afd,"/bin/ipsec/whack --listen & \n");
		//if(!strcmp(ser_remote_type,"4")  || !strcmp(ser_remote_type,"3") || !strcmp(ser_sg_type,"1"))
		//fprintf(afd,"echo\n");
		//zhangbin 2005.4.18
		//if(!strcmp(ser_remote_type,"3") || !strcmp(ser_sg_type,"1"))
		if(!strcmp(ser_sg_type,"1"))
		{
			char errmsg[100];
			memset(errmsg,0,sizeof(errmsg));
			sprintf(errmsg,"2rIKE[\"%s\"] ERROR: This tunnel should not be initiator!",ser_tunnel_name);
			send_log(errmsg);
		}

		else
		{
			printf("remote_type=%s,sg_type=%s\n",ser_remote_type,ser_sg_type);
			fprintf(afd,"/bin/ipsec/whack --name %s --initiate &\n",ser_tunnel_name);
		}
		fclose(afd);
			
		#ifdef ZB_DEBUG
			printf("Connecting auto key!\n");
		#endif
			
		eval("/bin/sh",AUTOKEY_FILE);
//		eval("/bin/sh",AUTOKEY_FILE);//twice ??
	//	if(!strcmp(ser_local_type,"1") && !strcmp(ser_remote_type,"1"))
	//		eval("/bin/sh",AUTOKEY_FILE);
			

		#ifdef ZB_DEBUG
			printf("ike_config=%s\n",ike_config);
		#endif
	//}			
					
	}
	else //manual key
	{
	#if 0 
		FILE *init_auto;
		
		if((init_auto = fopen(AUTOKEY_FILE,"w")) == NULL)
		{
			printf("cannot open!\n");
			return -1;
		}
		
		fprintf(init_auto,"#!/bin/sh\n\n");
		fprintf(init_auto,"/bin/ipsec/whack --name %s --initiate & \n",ser_tunnel_name);
		fclose(init_auto);
		
		eval("/bin/sh",AUTOKEY_FILE);
	#endif
		printf("Connection is not auto-keying!\n");
		return -1;
	}
#if 0
	else if(!strcmp(key_type,"1"))
	{
		FILE *mfd;   //manual key fd
		char tun_in[30],tun_out[30];
		char esp_in[30],esp_out[30];
	
		#ifdef ZB_DEBUG
			printf("enter manual keying!\n");
		#endif	
		//inbound tun,esp and outbound tun,esp
		sprintf(tun_in,"tun0x%s@%s",manual_ispi,wan_ipaddr);
		sprintf(tun_out,"tun0x%s@%s",manual_ospi,ser_sec_gw);
		
		sprintf(esp_in,"esp0x%s@%s",manual_ispi,wan_ipaddr);
		sprintf(esp_out,"tun0x%s@%s",manual_ospi,ser_sec_gw);
		
		if((mfd = fopen(MANUALKEY_FILE,"w")) == NULL)
		{
			printf("cannot open manual key file!\n");
			return -1;
		}
		
			fprintf(mfd,"#!/bin/sh\n\n");
			fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",tunnel_name,tun_out,wan_ipaddr,ser_sec_gw);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",tunnel_name,esp_out,enc_type,wan_ipaddr,manual_authkey,manual_enckey);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",tunnel_name,tun_out,esp_out);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",tunnel_name,tun_in,ser_sec_gw,wan_ipaddr);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",tunnel_name,esp_in,enc_config,ser_sec_gw,manual_authkey,manual_enckey);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",tunnel_name,tun_in,esp_in);
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"echo\n");
			fprintf(mfd,"/bin/ipsec/eroute --label \"%s\" --eraf inet --replace --src % --dst %s --said %s\n",tunnel_name,lsub_client,remote_client,tun_out); 
			fclose(mfd);
			
			#ifdef ZB_DEBUG
				printf("Connecting manual key!\n");
			#endif
			
			eval("/bin/sh",MANUALKEY_FILE);
	
	}
#endif
	return 0;
}

int disconnect_ipsec(webs_t wp)
{
	FILE *dfd;
	char st_file[50];
		
	if(ipsec_global_init(wp) < 0)
		return -1;
		
	if((dfd = fopen(DISCONNECT_FILE,"w")) == NULL )
	{
		printf("cannot open file %s!\n",DISCONNECT_FILE);
		return -1;
	}
	
	sprintf(st_file,"/tmp/ipsecst/%s",ser_tunnel_name);
	eval("rm",st_file);
	
	fprintf(dfd,"/bin/ipsec/whack --name %s --terminate\n",ser_tunnel_name);
	//fprintf(dfd,"echo\n");
//	fprintf(dfd,"/bin/ipsec/whack --name %s --terminate\n",ser_tunnel_name);
	//fprintf(dfd,"iptables -t nat -D POSTROUTING -s %s -d %s -j ACCEPT 2> /dev/null > /dev/null\n",lsub_disconn,remote_client_disconn);

	fclose(dfd);
	
	eval("/bin/sh",DISCONNECT_FILE);
	return 0;
}
	
