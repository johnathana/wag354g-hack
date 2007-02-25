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
#include <errno.h>
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <cy_conf.h>
#include <rc.h>
#include "pvc.h"


#define AUTOKEY_FILE "/tmp/rc_auto"
#define CLEAR_FILE "/tmp/vpn_clear"
#define IPT_FILE "/tmp/vpn-ipt"
#define MANUALKEY_FILE "/tmp/rc_manual"
#define IPSEC_START_FILE "/tmp/rc_ipsec"
#define PLUTO_FILE "/tmp/rc_pluto"
#define LISTEN_START_FILE "/tmp/rc_listen"
#ifdef  NOMAD_SUPPORT
#define PRESHARED_FILE "/tmp/ipsec_secrets/"//zhs add for quickVPN,3-23-2005
#else
#define PRESHARED_FILE "/tmp/ipsec.secrets"
#endif
#define STRUCT_LEN(name) sizeof(name)/sizeof(name[0])
#define IPSEC_TUNNELS 5

//#define ZB_DEBUG
int g_reboot = 0;

enum {SET,GET};

extern int check_ppp_proc(int num);
extern int check_ppp_link(int num);

//#define ZHSDEBUG
#ifdef ZHSDEBUG
#define zhs_printf printf
#else
#define zhs_printf(fmt,arg...)
#endif

#define clear_array(mem)   memset(mem,0,sizeof(mem))
#ifdef  NOMAD_SUPPORT
//add by zhs for quickVPN
#define Err_log(format,args...) \
{\
         fprintf(stderr,"%s %s line%d:",__FILE__,__FUNCTION__,__LINE__);\
         fprintf(stderr,format,##args);\
}

#define ZHS_DEBUG(format,args...) /*\
{\
         fprintf(stderr,"%s %s line%d:",__FILE__,__FUNCTION__,__LINE__);\
         fprintf(stderr,format,##args);\
}*/
const char del_file_cmd[]="rm -f";
//const char del_file_cmd1[]="rm";
//const char del_file_cmd2[]=" -f";
#define del_f(fn) eval(del_file_cmd,fn)
//#define del_f(fn) eval(del_file_cmd1fn)
const char shell_cmd[]="/bin/sh";
#define implement_f(fn) eval(shell_cmd,fn)

#define WAN_IPADDR    "/tmp/status/wan_ipaddr"
#define WAN_IFNAME    "/tmp/status/wan_ifname"
#define WAN_GWFNAME   "/tmp/status/wan_gateway"
#define WAN_MASKFN    "/tmp/status/wan_netmask"
#define WAN_BROADCFN     "/tmp/status/wan_broadcast"
#define MAX_CHAN   8
/***********************************************************
 *get_def_wan:get the default  wan  configuration,include:
 *      ip,ifname,netmask,broadcast,gateway
 */
int get_def_wan( char *ip
                ,char *gw
                ,char *ifname
                ,char *netmask
                ,char *broadcast)
{
        int i = 0;
        char name[50];
        char ipaddr[200];
        char wan_gw[200];
        char mask[200],bc[200];
        char *str_name, *str_ipaddr,*str_gw,*str_mask,*str_bc;
        char *tmp_name, *tmp_ipaddr,*tmp_gw,*tmp_mask,*tmp_bc;
        int def;
                                                                                                                             
                                                                                                                             
        if(!file_to_buf(WAN_IFNAME, name, sizeof(name)))
        {
                printf("no buf in %s!\n", WAN_IFNAME);
                return -1;
        }
        if(!file_to_buf(WAN_IPADDR, ipaddr, sizeof(ipaddr)))
        {
                printf("no buf in %s!\n", WAN_IPADDR);
                return -1;
        }
        if(!file_to_buf(WAN_GWFNAME, wan_gw, sizeof(wan_gw)))
        {
                printf("no buf in %s!\n", WAN_GWFNAME);
                return -1;
        }
        if(!file_to_buf(WAN_MASKFN, mask, sizeof(mask)))
        {
                printf("no buf in %s!\n", WAN_MASKFN);
                return -1;
        }
        if(!file_to_buf(WAN_BROADCFN, bc, sizeof(bc)))
        {
                printf("no buf in %s!\n", WAN_BROADCFN);
                return -1;
        }
                                                                                                                             
        def = atoi(nvram_safe_get("wan_active_connection"));
        ZHS_DEBUG("default wan is %s(%d)",nvram_safe_get("wan_active_connection"),def);
        if(def>=MAX_CHAN)
        {
                printf("get default wan is %d,OVERFLOW!!!\n",def);
                return -1;
        }
        str_name = name;
        str_ipaddr = ipaddr;
        str_gw = wan_gw;
        str_mask = mask;
        str_bc = bc;
        for(i = 0; i < MAX_CHAN; i ++)
        {
                ZHS_DEBUG("str_name = %s \n", str_name);
                ZHS_DEBUG("str_wanip =%s \n", str_ipaddr);
                ZHS_DEBUG("str_gw =%s \n", str_gw);
                ZHS_DEBUG("str_mask =%s \n",str_mask);
                ZHS_DEBUG("str_broadcast=%s\n",str_bc);
                tmp_name = strsep(&str_name, " ");
                tmp_ipaddr = strsep(&str_ipaddr, " ");
		tmp_gw = strsep(&str_gw, " ");
                tmp_mask = strsep(&str_mask," ");
                tmp_bc = strsep(&str_bc," ");
                ZHS_DEBUG("tmp_name = %s\n", tmp_name);
                ZHS_DEBUG("tmp_wanip = %s\n", tmp_ipaddr);
                ZHS_DEBUG("tmp_gw = %s\n", tmp_gw);
                if(i==def)
                {
                        if(!strncmp(tmp_name, "Down", 4))
                        {
                                Err_log("find the default WAN but it's stutus is Down,failed!!!!\n");
                                return -1;
                        }
                        if(NULL!=ip)
                                strcpy(ip, tmp_ipaddr);
                        if(NULL!=gw)
                                strcpy(gw,tmp_gw);
                        if(NULL!=ifname)
                                strcpy(ifname,tmp_name);
                        if(NULL !=netmask)
                                strcpy(netmask,tmp_mask);
                        if(NULL !=broadcast)
                                strcpy(broadcast,tmp_bc);
                        ZHS_DEBUG("get wanip=%s,gw=%s,ifname=%s,mask=%s,broadcast=%s and return OK\n"
                                        ,ip,gw,ifname,netmask,broadcast);
                        return 0;
                }
        }
        return -1;
                                                                                                                             
}
#endif
//end by zhs
int
filter_name(char *old_name, char *new_name, size_t size, int type)
{
        int i, j, match;

        struct pattern {
                char ch;
                char *string;
        };

        struct pattern patterns[] = {
                { ' ', "&nbsp;" },
                { ':', "&semi;" },
        };

        struct pattern *v;

	strcpy(new_name,"");	

        switch(type){
        case SET:
                for(i=0 ; *(old_name+i) ; i++){	
                        match = 0;
                        for(v = patterns ; v < &patterns[STRUCT_LEN(patterns)] ; v++) {
                                if(*(old_name+i) == v->ch){
					if(strlen(new_name) + strlen(v->string) > size){	// avoid overflow
                                                cprintf("%s(): overflow\n",__FUNCTION__);
                                                new_name[strlen(new_name)] = '\0';
                                                return 1;
                                        }
                                        sprintf(new_name+strlen(new_name), "%s", v->string);
                                        match = 1;
                                        break;
                                }
                        }
                        if(!match){
				if(strlen(new_name) + 1 > size){
                                        cprintf("%s(): overflow\n",__FUNCTION__);	// avoid overflow
                                        new_name[strlen(new_name)] = '\0';
                                        return 1;
                                }
                                sprintf(new_name+strlen(new_name),"%c",*(old_name+i));
			}
                }

                break;
        case GET:
                for(i=0,j=0 ; *(old_name+j) ; j++){
                        match = 0;
                        for(v = patterns ; v < &patterns[STRUCT_LEN(patterns)] ; v++) {
				if(!memcmp(old_name+j, v->string, strlen(v->string))){
                                        *(new_name+i) = v->ch ;
                                        j = j + strlen(v->string) - 1;
                                        match = 1;
                                        break;
                                }
                        }
                        if(!match)
                                *(new_name+i) = *(old_name+j);

                        i++;
                }
                *(new_name+i)='\0';
                break;
        default:
                cprintf("%s():Invalid type!\n",__FUNCTION__);
                break;
        }
        //cprintf("%s():new_name=[%s]\n", __FUNCTION__, new_name);

	return 1;
}

int send_log(const char *message,...)
{
	openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
	syslog(LOG_CRIT, "%s", message);
	closelog();
    	return 0;	
}

//add by zhs for quickVPN,3-24-2005
#ifdef  NOMAD_SUPPORT
#if 0
#define IPSEC_IF_MAX    6 //IPSEC interface max number
#define IPSEC_IF_CNF    "ipsec_if_cnf"
typedef struct vif_ipsec
{
	char wan_if[8];//WAN interface name,nas0-7
	char vipsec[8];//IPSEC vitual interface,ipsec0-4	
}vif_ipsec_t;
#endif
/*name of the file that attach the ipsec interface to the default WAN interface 
 */
#define IPSECIF_DEF_FN       "/tmp/ipsec_def_attach"
/************************************************************
 *attach_ipsec_if:attach virtual ipsec interface to WAN interface
 *                and set the iptables rule.
 */
int attach_ipsec_if( const char *ifname //wan interface name
		    ,const char *gw
                    ,const char *wan_ip // ipaddress
                    ,const char *broadcast // broadcast
                    ,const char *netmask // netmask
		    ,const char *ipsec_if //virtual ipsec interface name
		    ,const char *script_fn)//script file name that implement attaching function
{
      //  char ipsec_if[10];
       // int fin_r;
        FILE *att_f;
#if 0                                                                                                                       
        char ipsecif_fn[20];
                                                                                                                             
        //memset(ipsec_if,0,sizeof(ipsec_if));
        memset(ipsecif_fn,0,sizeof(ipsecif_fn));
      
        fin_r =find_ipsecif(ifname,ipsec_if,IPSEC_IF_CNF);
        if(1==fin_r)
        {/*find the ipsec interface for the wan interface,that means it has
          *been attached.we must not attach it.
          */
                return 0;
        }
        if(-1==fin_r)
        {
                Err_log("can't find attach ipsec interface, %d\n",fin_r);
                return -1;
        }
        sprintf(ipsecif_fn,"%s", IPSECIF_DEF_FN);
        ZHS_DEBUG("get ipsecif file name is %s",ipsecif_fn);
        
#endif
	//remove the file first 
	//del_f(script_fn);
	eval("rm -f", IPSECIF_DEF_FN);

        att_f = fopen(script_fn,"w");
        if(NULL==att_f)
        {
                Err_log("fopen file(%s) failed!\n ",script_fn);
                return -1;
        }
                                                                                                                             
        {
                fprintf(att_f,"#!/bin/sh\n");
                fprintf(att_f,"echo 0> /proc/sys/net/ipv4/%s/rp_filter\n",ifname);
                fprintf(att_f,"echo\n");
                fprintf(att_f,"/bin/ipsec/tncfg --attach --virtual %s"
                                    " --physical %s\n",ipsec_if,ifname);
                fprintf(att_f,"echo\n");
		if(!strncmp(ifname,"ppp",3))
			fprintf(att_f,"ifconfig %s inet %s netmask %s dstaddr %s\n"
					,ipsec_if,wan_ip,netmask,gw);
		else
                	fprintf(att_f,"ifconfig %s inet %s broadcast %s netmask %s\n"
                                	,ipsec_if,wan_ip,broadcast,netmask);
                fprintf(att_f,"echo\n");
                fprintf(att_f,"ifconfig %s mtu 1430\n",ipsec_if);
                fprintf(att_f,"echo\n");
                fprintf(att_f,"echo 0> /proc/sys/net/ipv4/%s/rp_filter\n",ipsec_if);
        }
        fclose(att_f);
        //implement_f(script_fn);
	eval("/bin/sh", IPSECIF_DEF_FN);
#if 0                                                                                                                       
        //nvram set default wan ipsec ifname
        {
                nvram_set(DEFAULT_IPSECIF_CNF,ipsec_if);
        }
#endif
        return 0;
}
/*******************************************************************
 *att_ipsec0_t0_def_wan:attach the virtual ipsec interface "ipsec0" to the 
 *			  default WAN interface 
 */
int att_ipsec0_t0_def_wan(void)
{
      //get default wan configuration and attach ipsec if to default wan
       char ifname[5],broadcast[20],netmask[20],wan_ip[20],gw[20];
                                                                                                                             
       clear_array(ifname);
       clear_array(broadcast);
       clear_array(netmask);
       clear_array(wan_ip);
       clear_array(gw);
                                                                                                                           
       if(get_def_wan(wan_ip,gw,ifname,netmask,broadcast))/*zhs md the netmask error bug,2005-07-14*/
       {
               Err_log("no wan connected and can not start QUICKVPN!!\n");
               return -1;
       }
       if(attach_ipsec_if(ifname,gw,wan_ip,broadcast,netmask,"ipsec0",IPSECIF_DEF_FN))
       {
              Err_log("attach ipsec interface to WAN interface failed\n");
              return -1;
       }
	   //create /var/def_wan file for freeswan/pluto/_updown to set mangle iptables
	 {
	 #define DEF_WAN_F    "/var/def_wan"

	 	FILE *def_wan=fopen(DEF_WAN_F,"w");
		if(NULL==def_wan)
		{
			Err_log("fopen file %s failed!!!\n",DEF_WAN_F);
			return -1;
		}

		fprintf(def_wan,"DEF_WAN_IFNAME=%s\n",ifname);
		fclose(def_wan);
	   }
        
	return 0; 
}
/***********************************************************
 *set_ipsec_def_ipt: set iptables default rules for ipsec,not for specified
 *               ipsec interface.
 *
 */
#define IPSEC_DEF_IPT  "/tmp/ipsec_def_ipt"
int set_ipsec_def_ipt(void)
{
	FILE *ipt_fp;

	//del_f(IPSEC_DEF_IPT);//remove the  file first	
	eval("rm -f", IPSEC_DEF_IPT);
	ipt_fp=fopen(IPSEC_DEF_IPT,"w");	
	if(NULL==ipt_fp)
	{
		Err_log("fopen file(%s) failed",IPSEC_DEF_IPT);
		return -1;
	}
	fprintf(ipt_fp,"#!/bin/sh\n");
	fprintf(ipt_fp,"iptables -I INPUT -p 50 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I OUTPUT -p 50 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I INPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I INPUT -p udp --sport 4500 --dport 4500 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I OUTPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I OUTPUT -p udp --sport 4500 --dport 4500 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I FORWARD -i ipsec0 -j ACCEPT\n");
	fprintf(ipt_fp,"iptables -I INPUT -i ipsec0 -j ACCEPT\n");
	#if 0
	fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s\n"
		,"157.0.0.100","157.0.0.10","nas0"/*rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname*/);
	fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 4500 --dport 4500 -j ROUTE --oif %s\n"
		,"157.0.0.100","157.0.0.10","nas0"/*rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname*/);
	#endif 
						
	fclose(ipt_fp);

	ZHS_DEBUG("will eval %s!\n",IPSEC_DEF_IPT);
	//implement_f(IPSEC_DEF_IPT);
	eval("/bin/sh", IPSEC_DEF_IPT);

	return 0;
}
#endif
//end by zhs
//add by zhs to support netbios broadcast,2005-07-19
#define NBS_DIR            "/tmp/nbsbrdc/"
#define SUBMASK_32             "255.255.255.255"
/********************************************************
 *set_nbs_pair_to_file:set the local secure group and remote
 * 		       secure group to file:/tmp/nbsbrdc/ *.nbs.
 *		       the "*" is replaced by the ipsec name.
 *		       if the mask is 0,that means 0xffffffff
 */
int set_nbs_pair_to_file(const char *name,
			 const char *lsub,
			 const char *lmask,
			 const char *rsub,
			 const char *rmask)
{
	char fl_n[50];
	FILE *fd;
	char mk_32[]=SUBMASK_32;
	char *lmsk_use,*rmsk_use;

	if(-1 == access(NBS_DIR,F_OK))
	{
		eval("mkdir",NBS_DIR);
	}	
	zhs_printf("%s with %s:%s:%s:%s...\n",__FUNCTION__,lsub,lmask,rsub,rmask);
	clear_array(fl_n);
	sprintf(fl_n,"%s%s.nbs",NBS_DIR,name);
	if(NULL==(fd=fopen(fl_n,"w")))
	{
		printf("fopen %s failed! errno=%d(%s)",fl_n,errno,strerror(errno));
		return -1;
	}		

	if(strlen(lmask))	
	{
		lmsk_use = lmask;
	}
	else
	{
		lmsk_use = mk_32;
	}
	if(strlen(rmask))	
	{
		rmsk_use = rmask;
	}
	else
	{
		rmsk_use = mk_32;
	}
	fprintf(fd,"%s:%s:%s:%s",lsub,lmsk_use,rsub,rmsk_use);
	fclose(fd);
	return 0;
}
//end by zhs


int start_ipsec()
{
	FILE *cl_fd,*prefd;
	int g_autokey_enabled = 0;
	int g_manualkey_enabled = 0; //zhangbin -2004.08.04
	int g_tunnel_enabled = 0; //zhangbin --2004.08.11
	int i;
	
	//zhs add for quickVPN,3-24-2005
	/*int index=0;
	vif_ipsec_t ipsec_if_list[IPSEC_IF_MAX];//IPSEC interface list
	memset(ipsec_if_list,0,IPSEC_IF_MAX*sizeof(vif_ipsec_t));*/
 	//end by zhs
#if 1	
	//if((g_reboot == 0) && nvram_match("ipsec_enable","0"))
	if((g_reboot == 0) && !strstr(nvram_safe_get("ipsec_status"),"1")) //new
	{
			printf("First boot,ipsec is disabled!\n");
			g_reboot = 1;
			return 0;
	}
	else
	{
#endif
		char rc_wan_ifname[20],rc_wan_ipaddress[20],rc_wan_netmask[20],rc_wan_broadcast[20],rc_wan_gateway[20];
		FILE *bfd,*ipt_fp;		
		int pflag = 0,eroute_flag[5],whack_flag[5];
#ifdef CLOUD_SUPPORT
		int init_flag[5];
#endif
		int manual_flag[5];
		int cnt;
		char ipsec_sts[8];
		#ifdef  NOMAD_SUPPORT
		int ipsec_if_index=1;/*zhs add ,ipsec interface name index,ipsec0 use for default wan*/
		#endif
		//add by zhs to support netbios broadcast
		int nbs_en = 0;
		char nbs_rsub[20],nbs_rmask[20],nbs_lmask[20];
		int is_r_single ;/*0 -- the remote sg is any,do not netbios broadcast
				1 -- the remote sg is single*/
		//end by zhs
		g_reboot = 1;

		#ifndef NOMAD_SUPPORT
		//remove by zhs for quickVPN,3-28-2005
		//for file rc_ipsec
		if((cl_fd = fopen(CLEAR_FILE,"w")) == NULL)
		{
			printf("Cannot open file %s!\n",CLEAR_FILE);
			return -1;
		}
		fprintf(cl_fd,"#!/bin/sh\n");
		fprintf(cl_fd,"/bin/ipsec/klipsdebug --none\n");
		fprintf(cl_fd,"/bin/ipsec/eroute --clear\n");
		fprintf(cl_fd,"/bin/ipsec/spi --clear\n");
		fclose(cl_fd);
		printf("VPN Clear first!\n");
		eval("/bin/sh",CLEAR_FILE);
		#endif
		//add by zhs to support netbios broadcast,2005-07-19
		/*killall the nbsrelay process  and rm the files*/
		{
			char sys_buf[40];
			clear_array(sys_buf);
			sprintf(sys_buf,"rm %s/* 2> /dev/null",NBS_DIR);
			system(sys_buf);
			clear_array(sys_buf);
			sprintf(sys_buf,"killall nbsrelay 2> /dev/null");
			system(sys_buf);
		}//end by zhs		
		if((ipt_fp = fopen("/tmp/vpn-ipt","w")) == NULL)
		{
			printf("Can't open /tmp/vpn-ipt!\n");
			return -1;
		}
		//fprintf(ipt_fp,"#!/bin/sh\n\n");

		//for loop-adding PSK
		#ifndef NOMAD_SUPPORT
		/*zhs delete for quickVPN,3-23-2005*/
		if((prefd = fopen(PRESHARED_FILE,"w")) == NULL)
		{
			printf("cannot open ipsec.secrets!\n");
			return -1;
		}
		#endif
		if((bfd = fopen("/tmp/blockip","w")) == NULL)
		{
			printf("cannot open /tmp/blockip!\n");
			return -1;
		}

		for(cnt = 0;cnt < 5; cnt++)
		{
			eroute_flag[cnt] = 0;
			whack_flag[cnt] = 0;
#ifdef CLOUD_SUPPORT
			init_flag[cnt] = 0;
#endif
		}
		
		memset(ipsec_sts,0,sizeof(ipsec_sts));
		strcpy(ipsec_sts,nvram_safe_get("ipsec_status"));
		printf("ipsec_sts=%s\n",ipsec_sts);

		for(i = 0;i < 5;i++)
		{
#if 1
			//new
			printf("*****ipsec_sts[%d]=%c*****\n",i,ipsec_sts[i]); 	
			if(ipsec_sts[i] == '0')
			{
				printf("ipsec_status disabled!\n");
				continue;
			}
			else
#endif
			{
			char rc_lsub[80],remote[100],remote_client[50],rc_tunnel_status[3],rc_tunnel_name[80],rc_local_type[3], rc_localgw_conn[3],rc_remote_type[3], rc_sg_type[3], rc_enc_type[3], rc_auth_type[3], rc_key_type[3],rc_l_ipaddr[20], rc_l_netmask[20],rc_r_ipaddr[20], rc_r_netmask[20],rc_sg_ipaddr[20],rc_sg_domain[80],rc_sec_gw[40],rc_any_gw[10],rc_lsub_client[50];
			int which = 0,wan_exist = 0;
			char word[256],*next;
			char rc_rtype_config_buf[100];
			char rc_ltype_config_buf[100];
			char rc_sg_config_buf[100];
			//FILE *nat_fd; zhangbin 2004.08.11
			int local_conn;
			
			memset(rc_sec_gw,0,sizeof(rc_sec_gw));
			memset(rc_any_gw,0,sizeof(rc_any_gw));
			memset(rc_lsub_client,0,sizeof(rc_lsub_client));
			memset(rc_lsub,0,sizeof(rc_lsub));
			memset(remote_client,0,sizeof(remote_client));
			memset(rc_tunnel_status,0,sizeof(rc_tunnel_status));
			memset(rc_tunnel_name,0,sizeof(rc_tunnel_name));
			memset(rc_local_type,0,sizeof(rc_local_type));
			memset(rc_localgw_conn,0,sizeof(rc_localgw_conn));
			memset(rc_remote_type,0,sizeof(rc_remote_type));
			memset(rc_sg_type,0,sizeof(rc_sg_type));
			memset(rc_enc_type,0,sizeof(rc_enc_type));
			memset(rc_auth_type,0,sizeof(rc_auth_type));
			memset(rc_key_type,0,sizeof(rc_key_type));
			memset(word,0,sizeof(word));
			memset(rc_l_ipaddr,0,sizeof(rc_l_ipaddr));
			memset(rc_l_netmask,0,sizeof(rc_l_ipaddr));
			memset(rc_r_ipaddr,0,sizeof(rc_r_ipaddr));
			memset(rc_r_netmask,0,sizeof(rc_r_netmask));
			memset(rc_sg_ipaddr,0,sizeof(rc_sg_ipaddr));
			memset(rc_sg_domain,0,sizeof(rc_sg_domain));

			//add by zhs for broadcast
			clear_array(nbs_rsub);	
			clear_array(nbs_rmask);
			clear_array(nbs_lmask);
			is_r_single =1;//end by zhs
			which = i;

			#ifdef ZB_DEBUG
			printf("g_autokey_enabled=%d in for!\n",g_autokey_enabled);
			#endif

			foreach(word, nvram_safe_get("entry_config"), next) 
			{
				if (which-- == 0) 
				{
					sscanf(word, "%3[^:]:%25[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", rc_tunnel_status,rc_tunnel_name,rc_local_type, rc_localgw_conn,rc_remote_type, rc_sg_type, rc_enc_type, rc_auth_type, rc_key_type);
				#ifdef ZB_DEBUG
					printf("rc_tunnel_status=%s, rc_tunnel_name=%s,rc_local_type=%s,rc_localgw_conn=%s,rc_remote_type=%s,rc_sg_type=%s,rc_enc_type=%s,rc_auth_type=%s,rc_key_type=%s\n", rc_tunnel_status,rc_tunnel_name,rc_local_type,rc_localgw_conn,rc_remote_type,rc_sg_type,rc_enc_type,rc_auth_type,rc_key_type);
				#endif 
				break;
				}	
			}
		
			memset(rc_ltype_config_buf, 0, sizeof(rc_ltype_config_buf));
			
			which = i;
			//printf("which=%d\n",which);

			foreach(rc_ltype_config_buf, nvram_safe_get("ltype_config"), next) 
			{
				if (which-- == 0) 
				{
					sscanf(rc_ltype_config_buf, "%20[^:]:%20[^\n]", rc_l_ipaddr,rc_l_netmask);
				#ifdef ZB_DEBUG
					printf("rc_l_ipaddr=%s, rc_l_netmask=%s\n", rc_l_ipaddr, rc_l_netmask);
				#endif 
					break;
				}
			}
	
			memset(rc_rtype_config_buf, 0, sizeof(rc_rtype_config_buf));
			which = i;
	
			foreach(rc_rtype_config_buf, nvram_safe_get("rtype_config"), next)
			{
				if (which-- == 0) 
				{
					sscanf(rc_rtype_config_buf, "%20[^:]:%20[^\n]", rc_r_ipaddr,rc_r_netmask);

				#ifdef ZB_DEBUG
					printf("rc_r_ipaddr=%s, rc_r_netmask=%s\n", rc_r_ipaddr, rc_r_netmask);
				#endif 
			
				break;
				}
			}
			
			if(!strcmp(rc_tunnel_status,"1"))
			{
				g_tunnel_enabled = 1; //zhangbin --2004.08.11
			}

			{
				char read_buf[256];
				char wanif_file[60];
				memset(wanif_file,0,sizeof(wanif_file));
				local_conn = atoi(rc_localgw_conn);
				#ifdef ZB_DEBUG
				printf("**in wan_ifname,local_conn=%d,localgw_conn=%s\n",local_conn,rc_localgw_conn);
				#endif
				if(local_conn < 8)
				{
					strcpy(wanif_file,WAN_IFNAME);
				}
#ifdef MPPPOE_SUPPORT
				else
					strcpy(wanif_file,WAN_MPPPOE_IFNAME);
#endif

				if(!file_to_buf(wanif_file, read_buf, sizeof(read_buf)))
				{
					printf("no buf in %s!\n", WAN_IFNAME);
					return 0;
				}
				foreach(word, read_buf, next)
				{
					if(local_conn-- == 0 )
					{
						strcpy(rc_wan_ifname,word);
						#ifdef ZB_DEBUG
						printf("rc_wan_ifname=%s\n",rc_wan_ifname);
						#endif
						break;
					}
				}
			}
		
#if 0
			if(!strcmp(rc_wan_ifname,"Down"))
			{
				if(!strcmp(rc_tunnel_status,"1"))
				{
					char errmsg[100];
					memset(errmsg,0,sizeof(errmsg));
					sprintf(errmsg,"2rIKE[\"%s\"] ERROR : No WAN Connection !",rc_tunnel_name);
					//send_log(errmsg); 2004.08.06
				}
				continue;
			}
#endif				
			
			if(strcmp(rc_wan_ifname,"Down"))
			{
				if(strstr(rc_wan_ifname, "ppp"))
				{
					if(check_ppp_link(atoi(rc_localgw_conn)) && check_ppp_proc(atoi(rc_localgw_conn)))
					{
						wan_exist = 1;
					}
				}
				else if(strstr(rc_wan_ifname, "nas"))
				{
					wan_exist = 1;
				}
#ifdef CLIP_SUPPORT
				else if(strstr(rc_wan_ifname, "atm"))
				{
					wan_exist = 1;
				}
#endif

			}
			if(!wan_exist)
			{
				printf("No WAN Connection !");
				continue;
			}


			if(!strcmp(rc_local_type,"0"))
			{
				sprintf(rc_lsub,"--client %s/32",rc_l_ipaddr); //for auto key
				sprintf(rc_lsub_client," %s/32",rc_l_ipaddr); //for manual key
			}
			else if(!strcmp(rc_local_type,"1"))
			{
				sprintf(rc_lsub,"--client %s/%s",rc_l_ipaddr,rc_l_netmask);
				sprintf(rc_lsub_client,"%s/%s",rc_l_ipaddr,rc_l_netmask);
				strcpy(nbs_lmask,rc_l_netmask);//add by zhs to support netbios broadcast
			}
		
			memset(rc_sg_config_buf, 0, sizeof(rc_sg_config_buf));
			which = i;
	
			foreach(rc_sg_config_buf, nvram_safe_get("sg_config"), next) 
			{
				if (which-- == 0) 
				{
					sscanf(rc_sg_config_buf, "%20[^:]:%50[^\n]", rc_sg_ipaddr,rc_sg_domain);
			
				#ifdef ZB_DEBUG
					printf("rc_sg_ipaddr=%s,sg_domain=%s\n",rc_sg_ipaddr,rc_sg_domain);
				#endif 
					break;
				}
			}

			printf("debug:rc_remote_type=%s,rc_sg_type=%s\n",rc_remote_type,rc_sg_type);	
		
			if(!strncmp(rc_sg_type,"0",1))
			{
				strncpy(rc_sec_gw,rc_sg_ipaddr,sizeof(rc_sec_gw));
				#ifdef ZB_DEBUG
				printf("rc_sec_gw=%s,rc_sg_ipaddr=%s in rc_sg_type!\n",rc_sec_gw,rc_sg_ipaddr);
				#endif
			}
			else if(!strncmp(rc_sg_type,"1",1)) //security gateway is "any"
			{
				strncpy(rc_sec_gw,"\"%any\"",sizeof(rc_sec_gw));
				strcpy(rc_any_gw,"%any");
				is_r_single =0;
				//strcpy(rc_nexthop_config,"--nexthop \"%direct\"");
			}
			else if(!strncmp(rc_sg_type,"2",1))
			{
				char temp[50];
				struct hostent *gw_host;
		
				gw_host = gethostbyname(rc_sg_domain);
	
				if(NULL == gw_host)
				{
					printf("Cannot resolve domain: %s\n",rc_sg_domain);
					{
						char gwerr[100];
						memset(gwerr,0,sizeof(gwerr));
						sprintf(gwerr,"2rIKE[\"%s\"] ERROR: Remote Security Gateway domain name problem!\n",rc_tunnel_name);
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
				strncpy(rc_sec_gw,temp,sizeof(rc_sec_gw));
		
		/**** zhangbin 2005.6.5 for re-getting IP by FQDN start *****/
				{
					char fqdn_cfg[] = "00000";

					strcpy(fqdn_cfg,nvram_safe_get("fqdn_cfg"));
					fqdn_cfg[i] = '1';

					nvram_set("fqdn_cfg",fqdn_cfg);
				}
				{
					char fqdn_ip_cfg[100],nv_ip[5][20];

					memset(fqdn_ip_cfg,0,sizeof(fqdn_ip_cfg));
					memset(nv_ip,0,sizeof(nv_ip));

					strcpy(fqdn_ip_cfg,nvram_safe_get("fqdn_ip_cfg"));
					sscanf(fqdn_ip_cfg,"%s %s %s %s %s",nv_ip[0],nv_ip[1],nv_ip[2],nv_ip[3],nv_ip[4]);
					strcpy(nv_ip[i],temp);
					
					sprintf(fqdn_ip_cfg,"%s %s %s %s %s",nv_ip[0],nv_ip[1],nv_ip[2],nv_ip[3],nv_ip[4]);
					
					nvram_set("fqdn_ip_cfg",fqdn_ip_cfg);

				}
        	/**** zhangbin 2005.6.5 for re-getting IP by FQDN end *****/

			}	
			
			printf("rc_sec_gw=%s,rc_any_gw=%s!\n",rc_sec_gw,rc_any_gw);
			printf("remote_client=%s,remote=%s!\n",remote_client,remote);
	
			if(!strncmp(rc_remote_type,"0",1))  //remote secure group is Ip addr.
			{
				sprintf(remote_client,"%s/32",rc_r_ipaddr);
				sprintf(remote,"--client %s/32",rc_r_ipaddr);
				//add by zhs for netbios broadcast
				strcpy(nbs_rsub,rc_r_ipaddr);
			}
			else if(!strncmp(rc_remote_type,"1",1)) //remote secure group is subnet.
			{
				sprintf(remote_client,"%s/%s",rc_r_ipaddr,rc_r_netmask);
				sprintf(remote,"--client %s/%s",rc_r_ipaddr,rc_r_netmask);
				//add by zhs for netbios broadcast
				strcpy(nbs_rsub,rc_r_ipaddr);
				strcpy(nbs_rmask,rc_r_netmask);
			}	
			else if(!strncmp(rc_remote_type,"3",1)) //remote secure group is any.
			{
				sprintf(remote_client,"0.0.0.0/0.0.0.0");
				sprintf(remote,"--client 0.0.0.0/0.0.0.0");
				//add by zhs for netbios broadcast
				strcpy(nbs_rsub,"255.255.255.255");
				strcpy(nbs_rmask,"0.0.0.0");
		
			}
			else if(!strncmp(rc_remote_type,"4",1)) //remote secure group is host.
			{
				sprintf(remote_client,"%s/32 ",rc_sec_gw);
				printf("Host!rc_sec_gw=%s,remote_client=%s\n",rc_sec_gw,remote_client);
				strcpy(remote,"");
				//add by zhs for netbios broadcast
				strcpy(nbs_rsub,rc_sec_gw);
			}	
		
#if 0
			/* finish del iptables rules when tunnel is enabled */	
			if((nat_fd = fopen("/tmp/deconf_file","w")) == NULL)
			{
				printf("cannot open /tmp/deconf_file!\n");
				return -1;
			}
			//printf("will del -s %s -d %s\n",rc_lsub_client,remote_client);
			if(strcmp(rc_lsub_client,""))
			{
				fprintf(nat_fd,"iptables -t nat -D POSTROUTING -s %s -d %s -j  ACCEPT 2> /dev/null > /dev/null \n",rc_lsub_client,remote_client);
				fprintf(nat_fd,"iptables -t mangle -D OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s 2> /dev/null > /dev/null\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname);
			}
			//fprintf(nat_fd,"echo 1 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",rc_wan_ifname);
			fclose(nat_fd);
			printf("will eval deconf_file\n");
			eval("/bin/sh","/tmp/deconf_file");
#endif			
			
			//printf("rc_tunnel_status=%s\n",rc_tunnel_status);	
			if(!strcmp(rc_tunnel_status,"1"))
			{
				char rc_keytype_config_buf[250];
				char rc_ipsecadv_config_buf[200];
				char rc_auto_pfs[3],rc_auto_presh[220],rc_auto_klife[20],rc_manual_enckey[100],rc_manual_authkey[100],rc_manual_ispi[8],rc_manual_ospi[8],rc_ipsec_opmode[3],rc_ipsecp1_enc[3],rc_ipsecp1_auth[3],rc_ipsecp1_group[3],rc_ipsecp1_klife[20],rc_ipsecp2_group[3],rc_ipsec_netbios[3],rc_ipsec_antireplay[3],rc_ipsec_keepalive[3],rc_ipsec_blockip[3],rc_ipsec_retrytimes[10],rc_ipsec_blocksecs[10],rc_esp_config[20],rc_esp_config_m[20];
				char enc_config[8],auth_config[8],p1enc_config[10],p1auth_config[10],p1group_config[10],ike_config[30],rc_nexthop_config[20],rc_ikemode_config[15],manual_file[30],rc_retries[5];
				char new_auto_presh[100];
				char ipsec_dev[8][8];
				char keepalive[20];//add by zhs for keep-alive
				memset(keepalive,0,sizeof(keepalive));//add by zhs for keep-alive
				memset(ipsec_dev[8],0,sizeof(ipsec_dev[8]));

				memset(rc_nexthop_config,0,sizeof(rc_nexthop_config));
				memset(rc_ikemode_config,0,sizeof(rc_ikemode_config));
				memset(manual_file,0,sizeof(manual_file));
				memset(rc_esp_config,0,sizeof(rc_esp_config));
				memset(rc_esp_config_m,0,sizeof(rc_esp_config_m));
				memset(rc_retries,0,sizeof(rc_retries));
				memset(rc_auto_pfs,0,sizeof(rc_auto_pfs));
				memset(rc_ipsec_blockip,0,sizeof(rc_ipsec_blockip));
				memset(rc_ipsec_retrytimes,0,sizeof(rc_ipsec_retrytimes));
				memset(rc_ipsec_blocksecs,0,sizeof(rc_ipsec_blocksecs));
				//printf("test-status=%s\n",rc_tunnel_status);
			
				memset(rc_keytype_config_buf, 0, sizeof(rc_keytype_config_buf));	
				which = i;
	
				foreach(rc_keytype_config_buf, nvram_safe_get("keytype_config"), next) 
				{
					if (which-- == 0) 
					{
						sscanf(rc_keytype_config_buf, "%3[^:]:%100[^:]:%20[^:]:%100[^:]:%100[^:]:%100[^:]:%100[^\n]", rc_auto_pfs,rc_auto_presh,rc_auto_klife,rc_manual_enckey,rc_manual_authkey,rc_manual_ispi,rc_manual_ospi);
			
				#ifdef ZB_DEBUG
						printf(" rc_auto_pfs=%s,rc_auto_presh=%s,rc_auto_klife=%s,rc_manual_enckey=%s,rc_manual_authkey=%s,rc_manual_ispi=%s,rc_manual_ospi=%s\n", rc_auto_pfs,rc_auto_presh,rc_auto_klife,rc_manual_enckey,rc_manual_authkey,rc_manual_ispi,rc_manual_ospi);	
				#endif 
					break;
					}
				}
				filter_name(rc_auto_presh,new_auto_presh,sizeof(new_auto_presh),GET);	
				#ifdef ZB_DEBUG
				printf("new_auto_presh=%s\n",new_auto_presh);
				#endif


				memset(rc_ipsecadv_config_buf, 0, sizeof(rc_ipsecadv_config_buf));
				which = i;
	
				foreach(rc_ipsecadv_config_buf, nvram_safe_get("ipsecadv_config"), next) 
				{
					if (which-- == 0) 
					{
						sscanf(rc_ipsecadv_config_buf, "%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%5[^:]:%10[^\n]", rc_ipsec_opmode,rc_ipsecp1_enc,rc_ipsecp1_auth,rc_ipsecp1_group,rc_ipsecp1_klife,rc_ipsecp2_group,rc_ipsec_netbios,rc_ipsec_antireplay,rc_ipsec_keepalive,rc_ipsec_blockip,rc_ipsec_retrytimes,rc_ipsec_blocksecs);

				#ifdef ZB_DEBUG
						printf("rc_ipsec_opmode=%s,rc_ipsecp1_enc=%s,rc_ipsecp1_auth=%s,rc_ipsecp1_group=%s,rc_ipsecp1_klife=%s,rc_ipsecp2_group=%s,rc_ipsec_netbios=%s,rc_ipsec_antireplay=%s,rc_ipsec_keepalive=%s,rc_ipsec_blockip=%s,rc_ipsec_retrytimes=%s,rc_ipsec_blocksecs=%s\n", rc_ipsec_opmode,rc_ipsecp1_enc,rc_ipsecp1_auth,rc_ipsecp1_group,rc_ipsecp1_klife,rc_ipsecp2_group,rc_ipsec_netbios,rc_ipsec_antireplay,rc_ipsec_keepalive,rc_ipsec_blockip,rc_ipsec_retrytimes,rc_ipsec_blocksecs);
				#endif 
						printf("***rc_ipsec_blockip=%s\n",rc_ipsec_blockip);					

					break;
					}
				}
				//add by zhs to support netbios broadcast
				zhs_printf("ipsec_netbios =%s,is_r_singl=%d.......\n",rc_ipsec_netbios,is_r_single);
				if(is_r_single && (!strcmp(rc_ipsec_netbios,"1")))
				{
					nbs_en =1;
					set_nbs_pair_to_file(rc_tunnel_name,rc_l_ipaddr,nbs_lmask,nbs_rsub,nbs_rmask);
				}
				//end by zhs
				//zhangbin 2004.11.01
#if 0
				if(atoi(rc_auto_klife) > atoi(rc_ipsecp1_klife))
					strcpy(rc_auto_klife,rc_ipsecp1_klife);
#endif

				printf("rc_auto_pfs=%s\n",rc_auto_pfs);
				{//for getting wan config 
					char word1[256],*next1,readbuf[256];
					int local_connection;
					char wanip_file[60],wanmask_file[60],wancast_file[80],wangw_file[80];
					memset(wanip_file,0,sizeof(wanip_file));
					memset(wanmask_file,0,sizeof(wanmask_file));
					memset(wancast_file,0,sizeof(wancast_file));
					memset(wangw_file,0,sizeof(wangw_file));

					local_connection = atoi(rc_localgw_conn);
					#ifdef ZB_DEBUG
					printf("in wan_ip:local_connection=%d,localgw_conn=%s\n",local_connection,rc_localgw_conn);
					#endif
					if(local_connection < 8)
					{
						strcpy(wanip_file,"/tmp/status/wan_ipaddr");
						strcpy(wanmask_file,"/tmp/status/wan_netmask");
						strcpy(wancast_file,"/tmp/status/wan_broadcast");
						strcpy(wangw_file,"/tmp/status/wan_gateway");
					}
					else
					{
						strcpy(wanip_file,"/tmp/status/wan_mpppoe_ipaddr");
						strcpy(wanmask_file,"/tmp/status/wan_mpppoe_netmask");
						strcpy(wancast_file,"/tmp/status/wan_mpppoe_broadcast");
						strcpy(wangw_file,"/tmp/status/wan_mpppoe_gateway");
					}

						
					if(!file_to_buf(wanip_file, readbuf, sizeof(readbuf)))
					{
						printf("no buf in %s!\n", WAN_IPADDR);
						return 0;
					}
					foreach(word1, readbuf, next1)
					{
						if(local_connection-- == 0 )
						{
							strcpy(rc_wan_ipaddress,word1);
							#ifdef ZB_DEBUG
							printf("rc_wan_ipaddress=%s\n",rc_wan_ipaddress);
							#endif
							break;
						}
					}
	
					local_connection = atoi(rc_localgw_conn);
					//printf("3:local_connection=%d\n",local_connection);
					if(!file_to_buf(wanmask_file, readbuf, sizeof(readbuf)))
					{
						printf("no buf in %s!\n", WAN_NETMASK);
						return 0;
					}
					foreach(word1, readbuf,next1)
					{
						if(local_connection-- == 0 )
						{
							strcpy(rc_wan_netmask,word1);
							#ifdef ZB_DEBUG
							printf("rc_wan_netmask=%s\n",rc_wan_netmask);
							#endif
							break;
						}
					}

					local_connection = atoi(rc_localgw_conn);
					//printf("4:local_connection=%d\n",local_connection);
					if(!file_to_buf(wancast_file, readbuf, sizeof(readbuf)))
					{
						printf("no buf in %s!\n", WAN_BROADCAST);
						return 0;
					}
					foreach(word1, readbuf,next1)
					{
						if(local_connection-- == 0 )
						{
							strcpy(rc_wan_broadcast,word1);
							#ifdef ZB_DEBUG
							printf("rc_wan_broadcast=%s\n",rc_wan_broadcast);
							#endif
							break;
						}
					}

					local_connection = atoi(rc_localgw_conn);
					//printf("5:local_connection=%d\n",local_connection);
					if(!file_to_buf(wangw_file, readbuf, sizeof(readbuf)))
					{
						printf("no buf in %s!\n", WAN_GATEWAY);
						return 0;
					}
					foreach(word1, readbuf,next1)
					{
						if(local_connection-- == 0 )
						{
							strcpy(rc_wan_gateway,word1);
							#ifdef ZB_DEBUG
							printf("rc_wan_gateway=%s\n",rc_wan_gateway);
							#endif
							break;
						}
					}
				}//end for getting wan-config	
			
				//printf("after getting wan-config:rc_auto_pfs=%s\n",rc_auto_pfs);
				strcpy(rc_nexthop_config,"\"%direct\"");
						
				if(!strcmp(rc_ipsec_opmode,"0")) //remote secure group is host.
				{
					sprintf(rc_ikemode_config,"%s","");
				}	
				else if(!strcmp(rc_ipsec_opmode,"1"))
					sprintf(rc_ikemode_config,"%s","--aggrmode");
				if(!strcmp(rc_enc_type,"1"))
				{
					strcpy(enc_config,"des"); 
				}
				else if(!strcmp(rc_enc_type,"2"))
				{
					strcpy(enc_config,"3des"); 
				}
				else if(!strcmp(rc_enc_type,"3"))
				{
					strcpy(enc_config,"aes"); 
				}
				else if(!strcmp(rc_enc_type,"0"))
				{
					strcpy(enc_config,"null"); 
				}
	
				if(!strcmp(rc_auth_type,"1"))
				{
					strcpy(auth_config,"md5"); 
				}
				else if(!strcmp(rc_auth_type,"2"))
				{
					strcpy(auth_config,"sha1"); 
				}

				sprintf(rc_esp_config_m,"%s-%s-96",enc_config,auth_config);
				sprintf(rc_esp_config,"%s",enc_config);
	
				if(!strcmp(rc_ipsecp1_enc,"1"))
				{
					strcpy(p1enc_config,"des"); 
				}
				else 
				{
					strcpy(p1enc_config,"3des"); 
				}
	
				if(!strcmp(rc_ipsecp1_auth,"1"))
				{
					strcpy(p1auth_config,"md5"); 
				}
				else 
				{
					strcpy(p1auth_config,"sha"); 
				}
	
				if(!strcmp(rc_ipsecp1_group,"1"))
				{
					strcpy(p1group_config,"modp768"); 
				}
				else 
				{	
					strcpy(p1group_config,"modp1024"); 
				}
	
				sprintf(ike_config,"%s-%s-%s",p1enc_config,p1auth_config,p1group_config);
			#ifdef ZB_DEBUG
				printf("rc_ipsec_blockip=%s\n",rc_ipsec_blockip);
			#endif
				if(!strcmp(rc_ipsec_blockip,"1"))
				{
					if(strcmp(rc_tunnel_name,""))
					{
						char block_name[50];
						FILE *block_fp;
						memset(block_name,0,sizeof(block_name));
						mkdir("/tmp/block",0777);
						sprintf(block_name,"/tmp/block/%s",rc_tunnel_name);
						fprintf(bfd,"%s:%s:%s\n",rc_tunnel_name,rc_ipsec_retrytimes,rc_ipsec_blocksecs);
	
						if((block_fp = fopen(block_name,"w")) !=NULL)
						{
							fprintf(block_fp,"%s\n","1");
							fclose(block_fp);
						}
					}
					
				}

				sprintf(rc_retries,"%s","2"); //2004.08.31
				
				//sprintf(ipsec_if,"ipsec%d",i);
				//printf("loop:ipsec_if=%s\n",ipsec_if);
				
				{ //for starting ipsecN
				#ifdef NOMAD_SUPPORT
					int need_attach=1;//add by zhs for ipsec if name,3-25-2005
				#endif
				#ifdef ZB_DEBUG
					printf("HERE:rc_localgw_conn=%s,pflag=%d\n",rc_localgw_conn,pflag);	
					printf("&&result=%d\n",pflag & (1 << atoi(rc_localgw_conn)));	
				#endif
				//add by zhs for quickVPN,3-25-2005
				#ifdef NOMAD_SUPPORT
					{
#define DEFAULT_IPSECIF_CNF "def_ipsecif_name"//the ipsec interface name that attaching to default WANif
						char wan_def_ifname[5];
						if(get_def_wan(NULL,NULL,wan_def_ifname,NULL,NULL))
							return -1;
						if(!strcmp(wan_def_ifname,rc_wan_ifname))
						{
						//	if(strncmp(nvram_safe_get(DEFAULT_IPSEC_CNF),"ipsec",5))
								need_attach=0;		
						}

					}//end by zhs
				
					if(need_attach &&(!(pflag & (1 << atoi(rc_localgw_conn)))))/*modified by zhs
								for quickVPN,3-25-2005*/
				#else
					if(!(pflag & (1 << atoi(rc_localgw_conn))))
				#endif
					{//loop for adding different PVCs
					
						FILE *file,*de_fd;

						char ipsec_if[8],de_fs[20];
						memset(ipsec_if,0,sizeof(ipsec_if));
						memset(de_fs,0,sizeof(de_fs));
					#ifdef NOMAD_SUPPORT
						sprintf(ipsec_if,"ipsec%d",/*i*/ipsec_if_index++);//zhs modified,3-25-2005
					#else
						sprintf(ipsec_if,"ipsec%d",i);
					#endif
						printf("loop:ipsec_if=%s\n",ipsec_if);
						if((file = fopen(IPSEC_START_FILE,"w")) == NULL)	
						{
							printf("Create %s failed!\n",IPSEC_START_FILE);
							return -1;
						}

						fprintf(file,"#!/bin/sh\n");
						fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",rc_wan_ifname);
						fprintf(file,"echo\n");

						fprintf(file,"/bin/ipsec/tncfg --attach --virtual %s --physical %s\n",ipsec_if,rc_wan_ifname);
						fprintf(file,"echo\n");
						if(!strncmp(rc_wan_ifname,"ppp",3))
							fprintf(file,"ifconfig %s inet %s netmask %s dstaddr %s\n",ipsec_if,rc_wan_ipaddress,rc_wan_netmask,rc_wan_gateway);
						else
							fprintf(file,"ifconfig %s inet %s broadcast %s netmask %s\n",ipsec_if,rc_wan_ipaddress,rc_wan_broadcast,rc_wan_netmask);
						fprintf(file,"echo\n");
						fprintf(file,"ifconfig %s mtu 1430\n",ipsec_if);
						//fprintf(file,"echo\n");
						fprintf(file,"echo\n");
						fprintf(file,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",ipsec_if);
						//add by zhs for quickVPN
						/*{
							strcpy(ipsec_if_list[index].wan_if,rc_wan_ifname);
							strcpy(ipsec_if_list[index].vipsec,ipsec_if);
							ZHS_DEBUG("add to ipsec_if_list,index=%d,wanif is %s"
								",ipsec_if is %s",index,rc_wan_ifname,ipsec_if);
							index++;
						}*/
						//end by zhs
						 // adding iptables for ipsec
						fprintf(ipt_fp,"iptables -I FORWARD -i %s -j ACCEPT\n",ipsec_if);
						fprintf(ipt_fp,"iptables -I INPUT -i %s -j ACCEPT\n",ipsec_if);
						//fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --gw %s\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_gateway);
						//zhangbin 2004.12.10
						if(!strcmp(rc_sec_gw,"\"%any\""))
						{
							fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s\n",rc_wan_ipaddress,"0/0",rc_wan_ifname);
						fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 4500 --dport 4500 -j ROUTE --oif %s\n",rc_wan_ipaddress,"0/0",rc_wan_ifname);
						}
						else
						{
							fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname);
							fprintf(ipt_fp,"iptables -t mangle -I OUTPUT -s %s -d %s -p udp --sport 4500 --dport 4500 -j ROUTE --oif %s\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname);
						}
						
						fclose(file);
						printf("will eval IPSEC_START_FILE!\n");
						eval("/bin/sh",IPSEC_START_FILE);
						sprintf(de_fs,"/tmp/autode_%d",atoi(rc_localgw_conn));
						if((de_fd = fopen(de_fs,"w")) == NULL)
						{
							printf("create %s failed!\n",de_fs);
							return -1;
						}
					
						fprintf(de_fd,"echo FORWARD\n");	
						fprintf(de_fd,"iptables -D FORWARD -i %s -j ACCEPT\n",ipsec_if);
						fprintf(de_fd,"echo INPUT\n");	
						fprintf(de_fd,"iptables -D INPUT -i %s -j ACCEPT\n",ipsec_if);
						fprintf(de_fd,"echo mangle\n");	
						
						//zhangbin 2004.12.10
						if(!strcmp(rc_sec_gw,"\"%any\""))
						{
							fprintf(de_fd,"iptables -t mangle -D OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s\n",rc_wan_ipaddress,"0/0",rc_wan_ifname);
						fprintf(de_fd,"iptables -t mangle -D OUTPUT -s %s -d %s -p udp --sport 4500 --dport 4500 -j ROUTE --oif %s\n",rc_wan_ipaddress,"0/0",rc_wan_ifname);
						}
						else
						{
							fprintf(de_fd,"iptables -t mangle -D OUTPUT -s %s -d %s -p udp --sport 500 --dport 500 -j ROUTE --oif %s\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname);
						fprintf(de_fd,"iptables -t mangle -D OUTPUT -s %s -d %s -p udp --sport 4500 --dport 4500 -j ROUTE --oif %s\n",rc_wan_ipaddress,rc_sec_gw,rc_wan_ifname);
						}
						
						fprintf(de_fd,"/bin/ipsec/tncfg --detach --virtual %s 2> /dev/null > /dev/null\n",ipsec_if);
						fprintf(de_fd,"ifconfig %s down 2> /dev/null > /dev/null\n",ipsec_if);
						fprintf(de_fd,"ifconfig %s 0.0.0.0 2> /dev/null > /dev/null\n",ipsec_if);
						fclose(de_fd);

						//zhangbin 2004.08.06 for manualkey
						strcpy(ipsec_dev[atoi(rc_localgw_conn)],ipsec_if);
						printf("ipsec_dev[%d]=%s\n",atoi(rc_localgw_conn),ipsec_dev[atoi(rc_localgw_conn)]);
						
					}//loop for adding different PVCs
					pflag |= 1 << atoi(rc_localgw_conn);
				
				} //for starting ipsecN
				
			if(!strcmp(rc_key_type,"0")) //auto-keying only needs listen
			{
				char pfs_config[10],whack_fname[20],eroute_fname[20];
				/* zhangbin 2005.6.13 for initiating at first*/
#ifdef CLOUD_SUPPORT
				char init_fname[20];
				FILE *init_fd;
#endif
				FILE *whack_fd,*eroute_fd;
				#ifdef NOMAD_SUPPORT
				char psk_fname[50];//add by zhs for quickVPN,3-23-2005
				#endif
				
			#ifdef CLOUD_SUPPORT
			
                                char remote_id[5][3],rid_fqdn[60];
                                char ipsec_remote_id[12];
                                int id_flag = 0;
                                char local_id[5][3],lid_fqdn[60];
                                char ipsec_local_id[12];
                                int lid_flag = 0;
                                int lr_flag = 0;
                                
				memset(local_id,0,sizeof(local_id));
                                memset(lid_fqdn,0,sizeof(lid_fqdn));
                                memset(ipsec_local_id,0,sizeof(ipsec_local_id));
                                strcpy(ipsec_local_id,nvram_safe_get("ipsec_lid_cfg"));
                                sscanf(ipsec_local_id,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",local_id[0],local_id[1],local_id[2],local_id[3],local_id[4]);
                                if(!strcmp(local_id[i],"1"))
                                {
                                        char lid_buf[300],*lid_ptr = lid_buf;
                                        char *tmp_ptr;
                                        char lid_ip[18];
                                        int which_t,icnt;
                                        
					which_t = i;
                                        memset(lid_buf,0,sizeof(lid_buf));
                                        memset(lid_ip,0,sizeof(lid_ip));
                                                                                                                             
                                        strcpy(lid_buf,nvram_safe_get("lid_config"));
                                        for(icnt = 0;icnt < IPSEC_TUNNELS;icnt++)
                                        {
                                                tmp_ptr = strsep(&lid_ptr," ");
                                                if(icnt == which_t)
                                                        sscanf(tmp_ptr, "%18[^:]:%60[^\n]", lid_ip, lid_fqdn);
                                        }
                                        lid_flag = 1;
                                }
                                memset(remote_id,0,sizeof(remote_id));
                                memset(rid_fqdn,0,sizeof(rid_fqdn));
                                memset(ipsec_remote_id,0,sizeof(ipsec_remote_id));
                                strcpy(ipsec_remote_id,nvram_safe_get("ipsec_rid_cfg"));
                                sscanf(ipsec_remote_id,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",remote_id[0],remote_id[1],remote_id[2],remote_id[3],remote_id[4]);
                                                                                                                            
                                if(!strcmp(remote_id[i],"1"))
                                {
                                        char rid_buf[300],*rid_ptr = rid_buf;
                                        char *tmp_ptr;
                                        char rid_ip[18];
                                        int which_t,icnt;
                                        
					which_t = i;
                                        memset(rid_buf,0,sizeof(rid_buf));
                                        memset(rid_ip,0,sizeof(rid_ip));
                                                                                                                             
                                        strcpy(rid_buf,nvram_safe_get("rid_config"));
                                        for(icnt = 0;icnt < IPSEC_TUNNELS;icnt++)
                                        {
                                                tmp_ptr = strsep(&rid_ptr," ");
                                                if(icnt == which_t)
                                                        sscanf(tmp_ptr, "%18[^:]:%60[^\n]", rid_ip, rid_fqdn);
                                        }
                                        id_flag = 1;
                                }
                                lr_flag = (lid_flag << 1) | id_flag;
			
			#endif
				memset(pfs_config,0,sizeof(pfs_config));
				memset(whack_fname,0,sizeof(whack_fname));
				memset(eroute_fname,0,sizeof(eroute_fname));
#ifdef CLOUD_SUPPORT
				memset(init_fname,0,sizeof(init_fname));

#endif
				#ifdef NOMAD_SUPPORT
				memset(psk_fname,0,sizeof(psk_fname));//zhs add for quickVPN,3-23-2005
				#endif
				printf("in tunnel-%d,rc_auto_pfs=%s\n",i,rc_auto_pfs);
				if(!strcmp(rc_auto_pfs,"1"))
					strcpy(pfs_config,"--pfs");
				else
					strcpy(pfs_config,"");

				//printf("in tunnel-%d,pfs_config=%s\n",i,pfs_config);
				if(g_autokey_enabled == 0)
					g_autokey_enabled = 1;		

				//printf("2:g_autokey_enabled=%d in autokey!\n",g_autokey_enabled);
				
				//add Pre-shared in ipsec.secrets
				//zhs add for quickVPN,3-23-2005
			#ifdef NOMAD_SUPPORT
				sprintf(psk_fname,"%s%s_fix.secret",PRESHARED_FILE,rc_tunnel_name);
				if((prefd = fopen(psk_fname,"w")) == NULL)
				{
					printf("cannot open ipsec.secrets!\n");
					return -1;
				}//end by zhs
			#endif
				if(!strcmp(rc_sg_type,"1"))
				{	
					fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_any_gw,new_auto_presh);
				}
				else
				 /* zhangbin 2005.3.31 */
                           {
                                        #ifdef CLOUD_SUPPORT
                                        //if(id_flag == 1)
                                        if(lr_flag == 0x1)//local is ip,rid is fqdn
                                                fprintf(prefd,"%s @%s : PSK \"%s\" \n",rc_wan_ipaddress,rid_fqdn,new_auto_presh);
                                        else if(lr_flag == 0x2) //local is fqdn,rid is ip
                                                fprintf(prefd,"@%s %s : PSK \"%s\" \n",lid_fqdn,rc_sec_gw,new_auto_presh);
                                        else if(lr_flag == 0x3) //local is fqdn,rid is fqdn
                                                fprintf(prefd,"@%s @%s : PSK \"%s\" \n",lid_fqdn,rid_fqdn,new_auto_presh);
                                        else
                                        #endif
                                        fprintf(prefd,"%s %s : PSK \"%s\" \n",rc_wan_ipaddress,rc_sec_gw,new_auto_presh);
                           }/* zhangbin 2005.3.31 */
			#ifdef NOMAD_SUPPORT  //preshared key file is one to one.
				fclose(prefd);
		        #endif
				sprintf(whack_fname,"/tmp/whack_%d",i);
				sprintf(eroute_fname,"/tmp/eroute_%d",i);
#ifdef CLOUD_SUPPORT
				sprintf(init_fname,"/tmp/initiate_%d",i);
#endif
				
				if((whack_fd = fopen(whack_fname,"w")) == NULL)
				{
					printf("cannot create %s\n",whack_fname);
					return -1;
				}
					
				fprintf(whack_fd,"#!/bin/sh\n\n");
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo\n");	
				fprintf(whack_fd,"echo\n");
				//add by zhs for keepalive
				if(strcmp(rc_ipsec_keepalive,"1"))
				{
					strcpy(keepalive,"--notkeepalive");		
				}
			        //end by zhs		
				fprintf(whack_fd,"/bin/ipsec/whack --name %s %s --encrypt" \
						 " %s --tunnel --esp %s --ike %s %s --psk "
					,rc_tunnel_name,rc_ikemode_config,keepalive,rc_esp_config_m,ike_config,pfs_config);
				
                                #ifdef CLOUD_SUPPORT
                                //if(id_flag == 1)
                                if(lr_flag == 0x1) //rid is fqdn,lid is ip
                                        fprintf(whack_fd," --id %s --host %s %s --nexthop %s --to --id @%s --host %s %s --nexthop %s ",rc_wan_ipaddress,rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rid_fqdn,rc_sec_gw,remote,rc_nexthop_config);
                                else if(lr_flag == 0x2) //rid is ip,lid is fqdn
                                        fprintf(whack_fd," --id @%s --host %s %s --nexthop %s --to --id %s --host %s %s --nexthop %s ",lid_fqdn,rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rc_sec_gw,rc_sec_gw,remote,rc_nexthop_config);
                                else if(lr_flag == 0x3) //rid is fqdn,lid is fqdn
                                        fprintf(whack_fd," --id @%s --host %s %s --nexthop %s --to  --id @%s --host %s %s --nexthop %s ",lid_fqdn,rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rid_fqdn,rc_sec_gw,remote,rc_nexthop_config);
                                else
                                #endif
				fprintf(whack_fd," --host %s %s --nexthop %s --to --host %s %s --nexthop %s ",rc_wan_ipaddress,rc_lsub,rc_wan_gateway,rc_sec_gw,remote,rc_nexthop_config);
				fprintf(whack_fd," --ikelifetime %s --ipseclifetime %s --rekeymargin \"10\" --keyingtries %s\n",rc_ipsecp1_klife,rc_auto_klife,rc_retries);
				//zhangbin 2004.11.01
				//fprintf(whack_fd," --ikelifetime %s --ipseclifetime %s --rekeymargin \"59\" --keyingtries %s\n",rc_ipsecp1_klife,rc_ipsecp1_klife,rc_retries);
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo\n");
				fprintf(whack_fd,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n",rc_wan_ifname);
				fclose(whack_fd);
				whack_flag[i] = 1;
				#ifdef ZB_DEBUG	
				printf("after whack name!\n");
				printf("rc_local_type=%s,rc_remote_type=%s\n",rc_local_type,rc_remote_type);
				#endif
#if 1
				//zhangbin 2005.4.18
				//if(!strcmp(rc_remote_type,"3") || !strcmp(rc_sg_type,"1"))// any should not be routed!
				if(!strcmp(rc_sg_type,"1"))// any should not be routed!
				{
					printf("can't initiate by ping from inside!\n");	
					//printf("eroute_fname=%s\n",eroute_fname);
					//eval("rm",eroute_fname);
				}
				else
				{
#endif					
					if((eroute_fd = fopen(eroute_fname,"w")) == NULL)
					{
						printf("Error:cannot create %s\n",eroute_fname);
						//printf("eroute_fname=%s\n",eroute_fname);
						return -1;
					}
						
					fprintf(eroute_fd,"#!/bin/sh\n\n");
					fprintf(eroute_fd,"echo\n");
					fprintf(eroute_fd,"echo\n");
					fprintf(eroute_fd,"echo\n");
					fprintf(eroute_fd,"echo\n");
					fprintf(eroute_fd,"echo\n");
					fprintf(eroute_fd,"echo\n");	
					fprintf(eroute_fd,"/bin/ipsec/whack --route --name %s\n",rc_tunnel_name);
				//because "whack --route" will call "up-" and will set status files,so we must del status first.	
					fprintf(eroute_fd,"rm /tmp/ipsecst/* 2> /dev/null > /dev/null\n");
				//zhangbin 2004.07.06	
					fprintf(eroute_fd,"echo\n");
					//fprintf(eroute_fd,"echo\n");
					fclose(eroute_fd);
					eroute_flag[i] = 1;
					printf("eroute_flag[%d]=%d\n",i,eroute_flag[i]);

#ifdef CLOUD_SUPPORT
					if((init_fd = fopen(init_fname,"w")) == NULL)
					{
						printf("Cant't create %s!\n",init_fname);
						return -1;
					}

					fprintf(init_fd,"#!/bin/sh\n\n");
					fprintf(init_fd,"echo\n");
					fprintf(init_fd,"echo\n");
					fprintf(init_fd,"echo\n");
					fprintf(init_fd,"echo\n");	
					fprintf(init_fd,"/bin/ipsec/whack --initiate --name %s\n",rc_tunnel_name);
					init_flag[i] = 1;
					fclose(init_fd);

#endif
					
				}

			}//end if auto-key
			else if(!strcmp(rc_key_type,"1"))
			{
				FILE *mfd/*, *it_fd,m_file*/;   //manual key fd
				char tun_in[30],tun_out[30],esp_in[30],esp_out[30],manualenc_hex[100],manualauth_hex[100]/*,iptables_config[200]*/;
	
				#ifdef ZB_DEBUG
					printf("enter manual keying!\n");
				#endif	

				#if 0	
				if((it_fd = fopen("/tmp/manual_it","w")) == NULL)
				{
					printf("Open /tmp/manual_it failed!\n");
					return -1;
				}
		
				fprintf(it_fd,"#!/bin/sh\n");	
				fclose(it_fd);
				eval("/bin/sh","/tmp/manual_it");
				#endif
				
				//inbound tun,esp and outbound tun,esp
				sprintf(tun_in,"tun0x%s@%s",rc_manual_ispi,rc_wan_ipaddress);
				sprintf(tun_out,"tun0x%s@%s",rc_manual_ospi,rc_sec_gw);
		
				sprintf(esp_in,"esp0x%s@%s",rc_manual_ispi,rc_wan_ipaddress);
				sprintf(esp_out,"esp0x%s@%s",rc_manual_ospi,rc_sec_gw);
				printf("ENTER handling rc_manual_enckey!\n");
			
				printf("2:remote_client=%s,remote=%s!\n",remote_client,remote);
				
				if((strncmp(rc_manual_enckey, "0x", 2) == 0) || (strncmp(rc_manual_enckey, "0X", 2) == 0))
				{
					strcpy((char*)manualenc_hex, rc_manual_enckey);
					printf("has 0x!\n");

				}
				else
				{
					int num_j = 0;
					char temp1[3];
					memset( manualenc_hex, 0, sizeof(manualenc_hex) );
					sprintf(manualenc_hex,"%s","0x");
				#ifdef ZB_DEBUG
				//	printf("manualenc_hex=%s\n",manualenc_hex);
				//	printf("strlen=%d,j=%d\n",strlen(rc_manual_enckey),j);
				#endif
					for(;rc_manual_enckey[num_j];num_j++)
					{
						sprintf(temp1,"%02x",rc_manual_enckey[num_j]);
						strcat(manualenc_hex,temp1);
					}

				}
				if((strncmp(rc_manual_authkey, "0x", 2) == 0) || (strncmp(rc_manual_authkey, "0X", 2) == 0))
				{
					strcpy((char*)manualauth_hex, rc_manual_authkey);
				}
				else
				{
					int num_i;
					char temp1[3];
					memset( manualauth_hex, 0, 100 );
					sprintf(manualauth_hex,"%s","0x");
					for(num_i = 0;num_i < strlen(rc_manual_authkey) ;num_i++)
					{
						sprintf(temp1,"%02x",rc_manual_authkey[num_i]);
						strcat(manualauth_hex,temp1);
					}
				}
				//fclose(file);	
				//printf("in manual:execute IPSEC_START_FILE!\n");
				//eval("/bin/sh",IPSEC_START_FILE);

				sprintf(manual_file,"/tmp/rc_manual_%d",i); 	
				//if((mfd = fopen(MANUALKEY_FILE,"w")) == NULL)
				if((mfd = fopen(manual_file,"w")) == NULL)
				{
					printf("cannot open %s!\n",manual_file);
					return -1;
				}
				//zhangbin 	
				printf("3:remote_client=%s,remote=%s!\n",remote_client,remote);
				fprintf(mfd,"#!/bin/sh\n\n");
				fprintf(mfd,"echo \"first out spi\"\n");
				fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",rc_tunnel_name,tun_out,rc_wan_ipaddress,rc_sec_gw);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"echo \" second out spi\"\n");
				fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",rc_tunnel_name,esp_out,rc_esp_config_m,rc_wan_ipaddress,manualauth_hex,manualenc_hex);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"echo \"first out spigrp\"\n");
				fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",rc_tunnel_name,tun_out,esp_out);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"echo \"first in spi\"\n");
				fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --ip4 --src %s --dst %s\n",rc_tunnel_name,tun_in,rc_sec_gw,rc_wan_ipaddress);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"echo \"second in spi\"\n");
				fprintf(mfd,"/bin/ipsec/spi --label \"%s\" --af inet --said %s --esp %s --src %s --authkey %s --enckey %s\n",rc_tunnel_name,esp_in,rc_esp_config_m,rc_sec_gw,manualauth_hex,manualenc_hex);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"echo \"first in spigrp\"\n");
				fprintf(mfd,"/bin/ipsec/spigrp --label \"%s\" --said %s %s\n",rc_tunnel_name,tun_in,esp_in);
				fprintf(mfd,"echo\n");
				fprintf(mfd,"/bin/ipsec/eroute --label \"%s\" --eraf inet --replace --src %s --dst %s --said %s\n",rc_tunnel_name,rc_lsub_client,remote_client,tun_out); 
				fprintf(mfd,"echo\n");
				/* the same as "prepare-host ipsec _updown" */
				if(!strcmp(rc_remote_type,"0") || !strcmp(rc_remote_type,"4"))
					fprintf(mfd,"route del -net %s netmask 255.255.255.255 2> /dev/null > /dev/null  \n",rc_r_ipaddr);
				else
					fprintf(mfd,"route del -net %s netmask %s 2> /dev/null > /dev/null \n",rc_r_ipaddr,rc_r_netmask);

				fprintf(mfd,"echo\n");
				/* the same as "route-host ipsec _updown" */
				if(!strcmp(rc_remote_type,"0") || !strcmp(rc_remote_type,"4"))
					fprintf(mfd,"route add -net %s netmask 255.255.255.255 dev %s gw %s \n",rc_r_ipaddr,ipsec_dev[atoi(rc_localgw_conn)],rc_wan_gateway);
				else
					fprintf(mfd,"route add -net %s netmask %s dev %s gw %s \n",rc_r_ipaddr,rc_r_netmask,ipsec_dev[atoi(rc_localgw_conn)],rc_wan_gateway);
				fprintf(mfd,"echo\n");
				printf("*****rc_r_ipaddr=%s,rc_r_netmask=%s,rc_wan_gateway=%s,ipsec_if=%s\n",rc_r_ipaddr,rc_r_netmask,rc_wan_gateway,ipsec_dev[atoi(rc_localgw_conn)]);
				fclose(mfd);
				manual_flag[i] = 1;	
			#ifdef ZB_DEBUG
				printf("Connecting manual key!\n");
			#endif
				g_manualkey_enabled = 1;	
				eval("/bin/sh",manual_file);
				//for deleting one tunnel
				{
					FILE *del_fp;
					char del_file[20];
					memset(del_file,0,sizeof(del_file));
					sprintf(del_file,"/tmp/deltun_%d",i);
					if((del_fp = fopen(del_file,"w")) != NULL)
					{
						fprintf(del_fp,"#!/bin/sh\n");
						fprintf(del_fp,"/bin/ipsec/eroute --label \"%s\" --eraf inet --del --src %s --dst %s 2> /dev/null > /dev/null\n",rc_tunnel_name,rc_lsub_client,remote_client);
						fprintf(del_fp,"/bin/ipsec/spi --label \"%s\" --af inet --del --said %s 2> /dev/null > /dev/null\n",rc_tunnel_name,tun_out);
						fprintf(del_fp,"/bin/ipsec/spi --label \"%s\" --af inet --del --said %s 2> /dev/null > /dev/null\n",rc_tunnel_name,tun_in);					if(!strcmp(rc_remote_type,"0"))
							fprintf(del_fp,"route del -net %s netmask 255.255.255.255 dev %s \n",rc_r_ipaddr,ipsec_dev[atoi(rc_localgw_conn)]);	
						else
							fprintf(del_fp,"route del -net %s netmask %s dev %s \n",rc_r_ipaddr,rc_r_netmask,ipsec_dev[atoi(rc_localgw_conn)]);

						fprintf(del_fp,"\ncase \"$1\" in\n");
						fprintf(del_fp,"\n1)\n");
						fprintf(del_fp,"\t echo \"will down!\"\n");
						fprintf(del_fp,"\tifconfig %s down\n",ipsec_dev[atoi(rc_localgw_conn)]);
						fprintf(del_fp,"\t;;\n");
						fprintf(del_fp,"\n0)\n");
						fprintf(del_fp,"\t echo \"no down!\"\n");
						fprintf(del_fp,"\t;;\n");
						fprintf(del_fp,"esac\n");
					}
					fclose(del_fp);
					printf("del_tun created!\n");
				}
			} //manaulkey
		} //end if tunnel_status = 1
		printf("exit tunnel_status:g_autokey_enabled=%d!\n",g_autokey_enabled);
	} //ipsec_sts == '1'
	} //end if for i~4

#ifndef NOMAD_SUPPORT
        fclose(prefd);
#endif
	fclose(bfd);
	/*{//add by zhs for quickVPN,3-24-2005
		//format: nas0:ipsec0 nas1:ipsec1
		char	ipsecif[200];
		int  offset=0;
		memset(ipsecif,0,sizeof(ipsecif));
		for(i=0;i<index;i++)
		{	
			sprintf(ipsecif+offset,"%s:%s ",ipsec_if_list[i].wan_if,ipsec_if_list[i].vipsec);
			offset +=strlen(ipsec_if_list[i].wan_if)+strlen(ipsec_if_list[i].vipsec);	
			ZHS_DEBUG("i =%d,offset =%d,wanif is %s,ipsecif is %s \n"
			 ,i,offset,ipsec_if_list[i].wan_if,ipsec_if_list[i].vipsec);
		}
		ZHS_DEBUG("ipsec interface list is:%s",ipsecif);	
		nvram_set(IPSEC_IF_CNF,ipsecif);
	}//end by zhs */
	//nvram_set("ipsec_enable",(g_tunnel_enabled == 1)?"1":"0"); //2004.08.11
	
	if(g_autokey_enabled == 1)
	{
		FILE *fp,*listen_fd;
		int j;
		char eroute_file[20],whack_file[20];
#ifdef CLOUD_SUPPORT
		char init_file[20];
#endif
//	#ifndef NOMAD_SUPPORT //md by zhs for quickVPN
#define PLUTOPID_FILE           "/var/run/pluto.pid" //the pid file of pluto
		if(NULL==fopen(PLUTOPID_FILE,"r"))
       		 {//pluto does not run
                	
			if((fp = fopen(PLUTO_FILE,"w")) == NULL)
			{
				printf("Create %s failed!\n",PLUTO_FILE);
				return -1;
			}
			fprintf(fp,"echo\n");
			fprintf(fp,"echo\n");
			if(!strcmp(nvram_safe_get("ipsec_natt"),"1"))
				fprintf(fp,"/bin/ipsec/pluto --nat_traversal --force_keepalive\n");
			else
				fprintf(fp,"/bin/ipsec/pluto\n");

			fclose(fp);
			printf("Starting PLUTO....!\n");
			eval("/bin/sh",PLUTO_FILE);
		}
	//#endif
		memset(eroute_file,0,sizeof(eroute_file));
		memset(whack_file,0,sizeof(whack_file));
#ifdef CLOUD_SUPPORT
		memset(init_file,0,sizeof(init_file));
#endif
	
		for(j = 0;j < 5;j++)
		{
			if(whack_flag[j] == 1)
			{
				sprintf(whack_file,"/tmp/whack_%d",j);
				eval("/bin/sh",whack_file);
			}
		}

		if((listen_fd = fopen(LISTEN_START_FILE,"w")) == NULL)
		{
			printf("Cannot open %s!\n",LISTEN_START_FILE);
			return -1;
		}
		fprintf(listen_fd,"#!/bin/sh\n\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"echo\n");
		fprintf(listen_fd,"/bin/ipsec/whack --listen & \n");
		fprintf(listen_fd,"echo \n");
		fclose(listen_fd);

		printf("Starting IPSEC Listen...........\n");
		eval("/bin/sh",LISTEN_START_FILE);
		for(j = 0;j < 5;j++)
		{
			#ifdef ZB_DEBUG
			printf("eroute_flag[%d]=%d\n",j,eroute_flag[j]);
			#endif 

			if(eroute_flag[j] == 1)
			{
				sprintf(eroute_file,"/tmp/eroute_%d",j);
				eval("/bin/sh",eroute_file);
			}
		}

#ifdef CLOUD_SUPPORT
		for(j = 0;j < 5;j++)
		{
			#ifdef ZB_DEBUG
			printf("init_flag[%d]=%d\n",j,init_flag[j]);
			#endif 

			if(init_flag[j] == 1)
			{
				sprintf(init_file,"/tmp/initiate_%d",j);
				eval("/bin/sh",init_file);
			}
		}
#endif

	}//autokey=1;

	if(strstr(nvram_safe_get("ipsec_status"),"1"))
	{
	#ifndef NOMAD_SUPPORT
		/*remove by zhs for quickVPN,3-28-2005*/
		fprintf(ipt_fp,"iptables -I INPUT -p 50 -j ACCEPT\n");
		fprintf(ipt_fp,"iptables -I OUTPUT -p 50 -j ACCEPT\n");
		fprintf(ipt_fp,"iptables -I INPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
		fprintf(ipt_fp,"iptables -I INPUT -p udp --sport 4500 --dport 4500 -j ACCEPT\n");
		fprintf(ipt_fp,"iptables -I OUTPUT -p udp --sport 500 --dport 500 -j ACCEPT\n");
		fprintf(ipt_fp,"iptables -I OUTPUT -p udp --sport 4500 --dport 4500 -j ACCEPT\n");
	#endif
		fclose(ipt_fp);
		printf("will eval /tmp/vpn-ipt!\n");
		eval("/bin/sh","/tmp/vpn-ipt");	
		stop_firewall();
		start_firewall();
	}
	else
		fclose(ipt_fp);

	//add by zhs to support netbios broadcast,2005-07-19
	if(nbs_en)
	{
		char sys_buf[32];
		clear_array(sys_buf);
		sprintf(sys_buf,"/bin/ipsec/nbsrelay");
		system(sys_buf);
		zhs_printf("start nbsrelay:%s\n",sys_buf);
	}//end by zhs	
	//nvram_commit(); //zhangbin-2004.08.11 for commit "ipsec_enable"
	return 0;
 }//end if nvram_match("ipsec_enable","1");
}

int stop_ipsec(void)
{
	int ret = 0;
	FILE *fp,*ter_fd;
	char word[120],*tptr = word,*lptr,ipsec_ifname[8];
	int i = 0,j = 0,bpluto = 0;
	
//	printf("begin stopping ipsec............!\n");
#if 0
	if((pluto_fd = fopen("/var/run/pluto.pid","r")) != NULL)
	{
		bpluto = 1;
		fclose(pluto_fd);
	}
#endif
	if((fopen("/var/run/pluto.pid","r")) != NULL)
	{
		bpluto = 1;
		//fclose(pluto_fd);
	}

	printf("bpluto=%d\n",bpluto);

	if(bpluto)
	{
		char tunnel_status[3],tunnel_name[40],local_type[3],localgw_conn[3],remote_type[3], sg_type[3], enc_type[3], auth_type[3], key_type[3],sysbuf1[100],sysbuf2[100],sysbuf_clear[100];
		if((ter_fd = fopen("/tmp/ter_tun","w")) == NULL)
		{
			printf("Open /tmp/ter_tun failed!\n");
			return -1;
		}
		fprintf(ter_fd,"#!/bin/sh\n");
		
		memset(word,0,sizeof(word));
		strcpy(word, nvram_safe_get("entry_config"));
		
		for(;i< 5;i++)
		{
			lptr = strsep(&tptr," ");
			//printf("Num %d:lptr=%s\n",i,lptr);
			memset(tunnel_status,0,sizeof(tunnel_status));
			memset(tunnel_name,0,sizeof(tunnel_name));
			memset(sysbuf1,0,sizeof(sysbuf1));
			memset(sysbuf2,0,sizeof(sysbuf2));
			memset(sysbuf_clear,0,sizeof(sysbuf_clear));
			//foreach(word, nvram_safe_get("entry_config"), next) 
			//{
			//	if (which-- == 0) 
			//	{
					sscanf(lptr, "%3[^:]:%40[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tunnel_status,tunnel_name,local_type, localgw_conn,remote_type, sg_type, enc_type, auth_type, key_type);
					#ifdef ZB_DEBUG
					printf("tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s,key_type=%s\n", tunnel_status,tunnel_name,local_type,localgw_conn,remote_type,sg_type,enc_type,auth_type,key_type);
					#endif 
					//break;
				//}	
			//}
			if(strcmp(tunnel_name,""))
			{
				//fprintf(ter_fd,"echo\n");
				fprintf(ter_fd,"/bin/ipsec/whack --terminate --name %s 2> /dev/null > /dev/null\n",tunnel_name);
				sprintf(sysbuf1,"/bin/ipsec/whack --terminate --name %s",tunnel_name);
				sprintf(sysbuf_clear,"/bin/ipsec/whack --delete --name %s",tunnel_name);
				system(sysbuf1);
				fprintf(ter_fd,"echo\n");
				fprintf(ter_fd,"/bin/ipsec/whack --unroute --name %s 2> /dev/null > /dev/null\n",tunnel_name);
				sprintf(sysbuf2,"/bin/ipsec/whack --unroute --name %s",tunnel_name);
				system(sysbuf2);
				system(sysbuf_clear);
			//	printf("run ((%s\n%s\n%s)) \n ",sysbuf1,sysbuf2,sysbuf_clear);
			}
		}	
	
		fclose(ter_fd);	
		//ret = eval("/bin/sh","/tmp/ter_tun");	
	}

	if((fp = fopen("/tmp/detach","w")) == NULL)
	{
		printf("Can't open /tmp/detach!\n");
		return -1;
	}
	memset(ipsec_ifname,0,sizeof(ipsec_ifname));
	fprintf(fp,"#!/bin/sh\n");

#ifdef  NOMAD_SUPPORT
	for(j = /*0*/1; j < 5; j++)//zhs modified from 0 to 1,not detach ipsec0.it is used for QuickVPN,3-25-2005
#else
	fprintf(fp,"/bin/ipsec/eroute --clear 2> /dev/null > /dev/null\n");
	fprintf(fp,"/bin/ipsec/spi --clear 2> /dev/null > /dev/null\n");
	for(j = 0; j < 5; j++)
#endif
	{
		sprintf(ipsec_ifname,"ipsec%d",j);
		fprintf(fp,"/bin/ipsec/tncfg --detach --virtual %s 2> /dev/null > /dev/null\n",ipsec_ifname);
		fprintf(fp,"echo\n");
		fprintf(fp,"ifconfig %s down 2> /dev/null > /dev/null\n",ipsec_ifname);
		//fprintf(fp,"echo\n");
		fprintf(fp,"ifconfig %s 0.0.0.0 2> /dev/null > /dev/null\n",ipsec_ifname);
	}
	
	fprintf(fp,"rm /tmp/ipsecst/* 2> /dev/null > /dev/null\n");
	#ifndef  NOMAD_SUPPORT
	if(bpluto)
	{	
		fprintf(fp,"killall -9 pluto 2> /dev/null > /dev/null\n");
		fprintf(fp,"echo\n");	
	}
	fprintf(fp,"rm /var/run/pluto.pid 2> /dev/null > /dev/null\n");
	#endif
	fprintf(fp,"rm /tmp/eroute_* 2> /dev/null > /dev/null\n");
	fprintf(fp,"rm /tmp/whack_* 2> /dev/null > /dev/null\n");
#ifdef  NOMAD_SUPPORT
	{//add by zhs to remove the psk file
	//	char psk_fname[50];
	//	memset(psk_fname,0,sizeof(psk_fname));
	//	sprintf(psk_fname,"%s*_fix.secret",PRESHARED_FILE);
		fprintf(fp,"rm %s*_fix.secret 2> /dev/null > /dev/null\n",PRESHARED_FILE);
	}
#else
	
	fprintf(fp,"rm %s 2> /dev/null > /dev/null\n",PRESHARED_FILE);
#endif
	fclose(fp);
	ret += eval("/bin/sh","/tmp/detach");
//add by zhs to support netbios broadcast,2005-07-19
	/*killall the nbsrelay process  and rm the files*/
	{
		char sys_buf[40];
		clear_array(sys_buf);
		sprintf(sys_buf,"rm %s/* 2> /dev/null",NBS_DIR);
		system(sys_buf);
		clear_array(sys_buf);
		sprintf(sys_buf,"killall nbsrelay 2> /dev/null");
		system(sys_buf);
	}//end by zhs	{
	return ret;
}

/* zhangbin for re-getting IP by FQDN 2005.6.6 */
int ipsec_restart_tunnel(int num,char *name,char *new_ip,char *old_ip,int st)
{
	char whack_file[50],eroute_file[50];
	FILE *whack_fd,*new_whack_fd,*pre_fd,*new_pre_fd,*listen_fp,*eroute_fp;
	char *cfg1 = NULL,*cfg2 = NULL,buf[1024],pre_buf[200],ter_buf[100];

	memset(whack_file,0,sizeof(whack_file));
	memset(eroute_file,0,sizeof(eroute_file));
	memset(buf,0,sizeof(buf));
	memset(pre_buf,0,sizeof(pre_buf));
	memset(ter_buf,0,sizeof(ter_buf));

	printf("in %s!\n",__FUNCTION__);

	sprintf(ter_buf,"/bin/ipsec/whack --terminate --name %s",name);
	system(ter_buf);
	sprintf(ter_buf,"/bin/ipsec/whack --delete --name %s",name);
	system(ter_buf);
	sprintf(ter_buf,"/bin/ipsec/whack --unroute --name %s",name);
	system(ter_buf);

	sprintf(whack_file,"/tmp/whack_%d",num);
	whack_fd = fopen(whack_file,"r");
	if(NULL == whack_fd)
	{
		printf("Can't open %s!\n",whack_file);
		return -1;
	}

	new_whack_fd = fopen("/tmp/tmp_whack","w");
	if(NULL == new_whack_fd)
	{
		printf("Can't create tmp_whack!\n");
		return -1;
	}

	while(fgets(buf,sizeof(buf),whack_fd))
	{
		cfg1 = strstr(buf,old_ip);
		if(cfg1)
		{
			*cfg1 = 0;
			cfg1 += strlen(old_ip);
			cfg2 = strstr(cfg1,old_ip);
			if(cfg2)
			{
				*cfg2 = 0;
				cfg2 += strlen(old_ip);
				fprintf(new_whack_fd,"%s %s %s %s %s\n",buf,new_ip,cfg1,new_ip,cfg2);
			}
			else
			{
				fprintf(new_whack_fd,"%s %s %s\n",buf,new_ip,cfg1);
			}
		}
		else
		{
			fprintf(new_whack_fd,"%s\n",buf);
		}
	}

	fclose(whack_fd);
	fclose(new_whack_fd);

	pre_fd = fopen(PRESHARED_FILE,"r");
	if(NULL == pre_fd)
	{
		printf("Can't find %s!\n",PRESHARED_FILE);
		return -1;
	}

	new_pre_fd = fopen("/tmp/new_ipsec.secrets","w");
	if(NULL == new_pre_fd)
	{
		printf("Can't find new secrets!\n");
		return -1;
	}

	while(fgets(pre_buf,sizeof(pre_buf),pre_fd))
	{
		char *p;
		p = strstr(pre_buf,old_ip);
		if(p)
		{
			*p = 0;
			p += strlen(old_ip);
			fprintf(new_pre_fd,"%s %s %s",pre_buf,new_ip,p);
		}
		else
			fprintf(new_pre_fd,"%s",pre_buf);
	}

	fclose(pre_fd);
	fclose(new_pre_fd);
	
	remove(PRESHARED_FILE);
	rename("/tmp/new_ipsec.secrets",PRESHARED_FILE);
		
	remove(whack_file);
	rename("/tmp/tmp_whack",whack_file);
	
	eval("/bin/sh",whack_file);

	listen_fp = fopen(LISTEN_START_FILE,"r");
	if(listen_fp)
	{
		fclose(listen_fp);
		eval("/bin/sh",LISTEN_START_FILE);
	}

	sprintf(eroute_file,"/tmp/eroute_%d",num);	
	eroute_fp = fopen(eroute_file,"r");
	if(eroute_fp)
	{
		fclose(eroute_fp);
		eval("/bin/sh",eroute_file);
	}

	/* If this IPSec tunnel is connected before,we must re-initiate it 
	 * ----zhangbin 2005.07.08 */
	if(st == 1)
	{
		char init_buf[80];
		memset(init_buf,0,sizeof(init_buf));

		sprintf(init_buf,"%s","echo");
		system(init_buf);
		sprintf(init_buf,"%s","echo");
		system(init_buf);
		sprintf(init_buf,"/bin/ipsec/whack --initiate --name %s",name);
		system(init_buf);

	}
	return 0;
}

int check_ipsec_fqdn()
{
	char fqdn_arr[8];
	int i;

	memset(fqdn_arr,0,sizeof(fqdn_arr));

	strcpy(fqdn_arr,nvram_safe_get("fqdn_cfg"));

	printf("fqdn_cfg=%s\n",fqdn_arr);
	
	for(i = 0;i < 5;i++)
	{
		if(fqdn_arr[i] == '1')
		{
			char entry_cfg[200],*next;
			char t_status[3],t_name[50],l_type[3],l_conn[3],r_type[3], sg_type[3], enc_type[3], auth_type[3], key_type[3];
			int which_conn = i;
	
			foreach(entry_cfg, nvram_safe_get("entry_config"), next)
			{
				if (which_conn-- == 0) 
				{
					sscanf(entry_cfg, "%3[^:]:%25[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", t_status,t_name,l_type, l_conn,r_type, sg_type, enc_type, auth_type, key_type);
				#ifdef ZB_DEBUG
					printf("rc_tunnel_status=%s, rc_tunnel_name=%s,rc_local_type=%s,rc_localgw_conn=%s,rc_remote_type=%s,rc_sg_type=%s,rc_enc_type=%s,rc_auth_type=%s,rc_key_type=%s\n", t_status,t_name,l_type,l_conn,r_type,sg_type,enc_type,auth_type,key_type);
				#endif 
				break;
				}	
			}
			
			if(!strcmp(t_status,"1"))
			{
				char sg_arr[200],fqdn_ip[20],*sg_ptr = sg_arr,*tmp_ptr = NULL,*fqdn_ptr; 
				char fqdn_ip_cfg[100],nv_ip[5][20];
				struct hostent *fqdn_host;
				int which = i,j;

				memset(sg_arr,0,sizeof(sg_arr));
				memset(fqdn_ip,0,sizeof(fqdn_ip));
				memset(fqdn_ip_cfg,0,sizeof(fqdn_ip_cfg));
				memset(nv_ip,0,sizeof(nv_ip));

				strcpy(sg_arr,nvram_safe_get("sg_config"));

				for(j = 0;j < 5;j++)
				{
					tmp_ptr = strsep(&sg_ptr," ");
					
					if(j == which)
						break;
				}

				fqdn_ptr = strchr(tmp_ptr,':');
				fqdn_ptr ++;
				
				fqdn_host = gethostbyname(fqdn_ptr);

				if(NULL == fqdn_host)
				{
					printf("Cannot resolve domain: %s\n",fqdn_ptr);
					return -1;
				}
	
				sprintf(fqdn_ip, "%d.%d.%d.%d", 
					(unsigned char)fqdn_host->h_addr_list[0][0],
					(unsigned char)fqdn_host->h_addr_list[0][1],
					(unsigned char)fqdn_host->h_addr_list[0][2],
					(unsigned char)fqdn_host->h_addr_list[0][3]
				);

				strcpy(fqdn_ip_cfg,nvram_safe_get("fqdn_ip_cfg"));
				sscanf(fqdn_ip_cfg,"%s %s %s %s %s",nv_ip[0],nv_ip[1],nv_ip[2],nv_ip[3],nv_ip[4]);	

				printf("fqdn_ip=%s,nv_ip[%d]=%s\n",fqdn_ip,i,nv_ip[i]);			
				if(strcmp(fqdn_ip,nv_ip[i]))
				{
					char state_fname[50],st_buf[3];
					int ipsec_st = 0;
					FILE *st_fd;
					
					memset(state_fname,0,sizeof(state_fname));
					memset(st_buf,0,sizeof(st_buf));

					sprintf(state_fname,"/tmp/ipsecst/%s",t_name);
					if((st_fd = fopen(state_fname,"r")) == NULL)
					{
						printf("cannot open %s!\n",state_fname);
						ipsec_st = 0; //Disconnected
					}
					
					fread(st_buf,1,1,st_fd);
					st_buf[1] = '\0';
					fclose(st_fd);

					if(!strcmp(st_buf,"1"))
						ipsec_st = 1; //Connected
					else
						ipsec_st = 0; //Disconnected

					printf("Dynamic IP changed!\n");
				
					ipsec_restart_tunnel(i,t_name,fqdn_ip,nv_ip[i],ipsec_st);
				
					strcpy(nv_ip[i],fqdn_ip);

					sprintf(fqdn_ip_cfg,"%s %s %s %s %s",nv_ip[0],nv_ip[1],nv_ip[2],nv_ip[3],nv_ip[4]);

					nvram_set("fqdn_ip_cfg",fqdn_ip_cfg);	
	
				}
			}

		}
		
	}
	return 0;
}

