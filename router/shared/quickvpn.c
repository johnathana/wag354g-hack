/*
 *quickvpn.c:
 *	support quickVPN function for linksys.This file supply the 
 * 	function of creating quickVPN tunnel,deleting quickVPN 
 *	tunnel.    
 *
 *Copyright, CyberTAN Corporation
 *All Rights Reserved.
 *
 *
 *Modify History:
 ----------------------------------------------------------
 *01a Mar 23,2005 Created by zhs
 *
 */
//#define NOMAD_SUPPORT //xiaoqin remove

#include <cy_conf.h>

#ifdef NOMAD_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <utils.h>
#include <unistd.h>
#include <rc.h>
#include "nomad.h" //xiaoqin add

#define QUICKVPN_ENTRY_MAX    5//max quickVPN entry allowed
//#define MAX_NAME              36//MAX of Name byte number //xiaoqin remove
#define DESC_INVALID          -1
#define QUICKVPN_DIR         "/tmp/quickvpn/" //use for store quickvpn file,connection and deletion files
#define PRESHARED_FILE       "/tmp/ipsec_secrets/"//use for store PSK
#define PLUTOPID_FILE           "/var/run/pluto.pid" //the pid file of pluto
#define QV_SECR_FN_POST         "_qv.secret"//the postfix of the secret file name
#define CONN_FN_PRE          "conn_" //the prefix of the vpn connection file name
#define DELT_FN_PRE          "del_"     //the prefix of the vpn deletion file name
#define IPSECIF_FN_PRE       "/tmp/rc_"//the prefix of the ipsec interface file using for attaching 
					// the WAN interface,the file name is similar as rc_ipsec0,
//use for store iptables file that allow HTTPS packets sending to quickVPN client,
#define IPSEC_HTTPS_IPT_DIR  "/tmp/ipsec_https_ipt/" 
#define IPSEC_IF_CNF        "ipsec_if_cnf" //ipsec interface id for nvram
#define DEFAULT_IPSECIF_CNF "def_ipsecif_name"//the ipsec interface name that attaching to default WANif

#if 0 //xiaoqin remove
typedef enum QV_OPT_TYPE
{
	QV_START,
	QV_STOP,
}qv_opt_t;

typedef struct vpnmsg
{
	char post_nat_ip[20];
	char pre_nat_ip[20];
	char random_psk[MAX_NAME];
	char username[MAX_NAME];
	qv_opt_t	type;
}quick_vpn_t;
#endif

#define Err_log(format,args...) \
{\
	 printf("%s %s line%d:",__FILE__,__FUNCTION__,__LINE__);\
	 printf(format,##args);\
}
#define clear_array(mem)   memset(mem,0,sizeof(mem))
#define ZHS_DEBUG(format,arg...) /* \ 
{ \ 
	printf("%s %s line%d:",__FILE__,__FUNCTION__,__LINE__);\
	printf(format,##arg);\
}*/

void init_quickvpn_mkdir(void)
{
	if(-1 == access(QUICKVPN_DIR, F_OK))
		eval("mkdir",QUICKVPN_DIR);
	if(-1 == access(PRESHARED_FILE, F_OK))
		eval("mkdir", PRESHARED_FILE);
	if(-1 == access(IPSEC_HTTPS_IPT_DIR, F_OK))
		eval("mkdir", IPSEC_HTTPS_IPT_DIR);
}

#if 0
typedef struct vpn_desc
{
	int index;//tunnel index,id the tunnel,between 1 and QUICKVPN_ENTRY_MAX,-1(negative) means invalid
	char name[MAX_NAME];//tunnel name,used for whack command 
}vpn_desc_t;

vpn_desc_t quickvpn_list[QUICKVPN_ENTRY_MAX];//the start of QuickVPN tunnel list 

//#define alloc_thing(thing,name) alloc_bytes(sizeof(thing),name)
//#define new_vpn_desc() alloc_thing(vpn_desc_t,"QuickVPN desc")

#if 0
void *alloc_bytes(size_t size,const char *name)
{
	void *p = malloc(size);
	if(NULL==p)
	{	
		fprintf(stderr,"unable to malloc %d bytes for %s\n",size,name);
		return NULL
	}
	memset(p,0,size);
	return p;
}
#endif
 
void init_quickvpn_list(void)
{
	int index;
	
	for(index=0;index<QUICKVPN_ENTRY_MAX;index++)
	{
		memset(&quickvpn_list[index],0,sizeof(quickvpn_list));
		quickvpn_list[index].index = DESC_INVALID;
	}
	if(-1 == access(QUICKVPN_DIR, F_OK))
		eval("mkdir",QUICKVPN_DIR);
	if(-1 == access(PRESHARED_FILE, F_OK))
		eval("mkdir", PRESHARED_FILE);
	if(-1 == access(IPSEC_HTTPS_IPT_DIR, F_OK))
		eval("mkdir", IPSEC_HTTPS_IPT_DIR);
	
}
/**************************************************************
 *
 */
int find_index_by_name(const char *name)
{
	int i;
	
/*	for(i=0;i<QUICKVPN_ENTRY_MAX;i++)
	{
		if(!strcmp(name,quickvpn_list[i].name))
			return quickvpn_list[i].index;
	}*/
	return -1;
}

/***************************************************************
 *add_vpn_desc:add one entry to quickvpn list.If success,return the 
 *             tunnel index;if list is full,return -1.
 */
int add_vpn_desc(const char *name)
{
	
	unsigned int i;
	
	for(i=0;i<QUICKVPN_ENTRY_MAX;i++)
	{	
		if(quickvpn_list[i].index<0)
		{
			quickvpn_list[i].index=i;
			strcpy(quickvpn_list[i].name,name);
			return i;
		}	

	}
	fprintf(stderr,"quickVPN:list has full and store %d entry and no space\n",i);
	return -1;
}
/****************************************************************
 *delete_vpn_desc:delete one entry from quickVPN description list	
 *
 */
int delete_vpn_desc(int index)
{
	
//	int index=find_index_by_name(name);

	if((index>=0)&&(index<QUICKVPN_ENTRY_MAX))
	{
		memset(&quickvpn_list[index],0,sizeof(quick_vpn_t));
		quickvpn_list[index].index=DESC_INVALID;
		return 0;
	}	
	Err_log("can not find the index(%d) in the list!\n",index);
	return 1;
}
#endif


#define WAN_IPADDR    "/tmp/status/wan_ipaddr"
#define WAN_IFNAME    "/tmp/status/wan_ifname"
#define WAN_GWFNAME   "/tmp/status/wan_gateway"
#define WAN_MASKFN    "/tmp/status/wan_netmask"
#define WAN_BROADCFN     "/tmp/status/wan_broadcast"
#define MAX_CHAN   8

/***********************************************************
 *get_def_wan:get the default  wan  configuration,include:
 *	ip,ifname,netmask,broadcast,gateway
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
/***********************************************************
 *run_pluto:create the process of pluto,if the pluto has been
 *           run,then return 0;else create the task and success
 *	     then return 1,otherwise create failed,return -1.
 *
 */
int run_pluto(void)
{
	FILE *pluto_f;
	/* zhangbin */
	FILE *pl_start;		

	pluto_f=fopen(PLUTOPID_FILE,"r");
	if(NULL!=pluto_f)
	{//pluto has been run	
		fclose(pluto_f);
		return 0;	
	}

	pl_start = fopen("/tmp/start_pl","w");
	if(NULL == pl_start)
	{
		printf("Can't create start_pl!\n");
		return -1;
	}

	fprintf(pl_start,"#!/bin/sh\n");
	//fprintf(pl_start,"/bin/ipsec/pluto\n");
	if(!strcmp(nvram_safe_get("ipsec_natt"),"1"))
			fprintf(pl_start,"/bin/ipsec/pluto --nat_traversal --force_keepalive\n");
	else
			fprintf(pl_start,"/bin/ipsec/pluto\n");

	fprintf(pl_start,"echo\n");
	fprintf(pl_start,"echo\n");
	fprintf(pl_start,"echo\n");
	//fprintf(pl_start,"/bin/ipsec/klibsdebug --all\n");
	fclose(pl_start);
	
	eval("/bin/sh","/tmp/start_pl");
	
	pluto_f=fopen(PLUTOPID_FILE,"r");
	if(NULL!=pluto_f)
	{//create the task successfully	
		fclose(pluto_f);
		return 1;	
	}
	Err_log("create pluto task failed!!!!!\n");
	return -1;
	
}

#define IPSEC_IF_MAX    6 //IPSEC interface max number
typedef struct vif_ipsec
{
        char wan_if[8];//WAN interface name,nas0-7
        char vipsec[8];//IPSEC vitual interface,ipsec0-4
}vif_ipsec_t;

#if 0                                                                                                                             
/*************************************************************
 *find_ipsecif:find the ipsec interface by spicified wan_if name
 *	       in nvram,if success,store in parameter - ipsecif.
 *             If no find,the store ipsecif with a unused ipsec
 *             interface name(ipsec0-4)
 *RETURN       1 --The wanif has been attach,ipsecif store the 
 *		   corresponding ipsec if.
 *             0 -- The wanif not been attach,ipsecif store the 
 *		   ipsec if that can used.
 *	       -1 --The wanif not been attach,not find the ipsec
 *	            if that can used. 
 *
 */
const char const_ipsec_if[5][8]=
	{"ipsec0","ipsec1","ipsec2","ipsec3","ipsec4"};
int find_ipsecif(const char *wan_if,char *ipsecif,const char *cfg)
{
	char ipsec_if[200];
	char *next,entry[24];
	char *nvram_wanif,*nvram_vpnif;
	int i;
	int is_noused;//the ipsec if name is used(0) or not(1)
	int no_find_noused=1;//0--find the noused name,1--no find
//	vif_ipsec_t ipsec_vif_list[IPSEC_IF_MAX];
	ZHS_DEBUG("start with paramter wan_if=%s,cfg=%s .....................\n");	
	memset(ipsec_if,0,sizeof(ipsec_if));
	strcpy(ipsec_if,nvram_safe_get(cfg));

	ZHS_DEBUG("get ipsec_if_list is %s\n",ipsec_if);	
	for(i=0;i<5;i++)
	{
		is_noused = 1;
		foreach(entry,ipsec_if,next)
		{
			nvram_vpnif = entry;
			nvram_wanif = strsep(&nvram_vpnif,":");
			if(!nvram_wanif || !nvram_vpnif)
				continue;
			if(!strcmp(wan_if,nvram_wanif))
			{
				strcpy(ipsecif,nvram_vpnif);		
				ZHS_DEBUG("find the wanif(%s) and ipsecif(%s) and return OK\n",nvram_wanif,ipsecif);
				return 1;
			}
			if(!strcmp(const_ipsec_if[i],nvram_vpnif))
			{
				is_noused =0;			
			}
		}
		if((is_noused)&&(no_find_noused))
		{//no used
			strcpy(ipsecif,const_ipsec_if[i]);
			no_find_noused =0;
			ZHS_DEBUG("find the unused ipsecif (%s),index is %d  \n",ipsecif,i);
		}
	}
	if(no_find_noused)
	{
		ZHS_DEBUG("not find the wanif and not find the unused ipsecif,Return ERROR!!!!!!!!!!!!!\n");
		return -1;//no find
	}
	
	ZHS_DEBUG("find the unused ipsecif (%s),index is %d  and return 0\n",ipsecif,i);
	return 0;

}
/************************************************************
 *attach_ipsec_if:attach virtual ipsec interface to WAN interface 
 *                and set the iptables rule. 
 */
int attach_ipsec_if( const char *ifname
		    ,const char *wan_ip
		    ,const char *broadcast
		    ,const char *netmask)
{
	char ipsec_if[10];
	int fin_r;
	FILE *att_f;
	char ipsecif_fn[20];

	memset(ipsec_if,0,sizeof(ipsec_if));
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

	sprintf(ipsecif_fn,"%s%s",IPSECIF_FN_PRE,ipsec_if);	
	ZHS_DEBUG("get ipsecif file name is %s",ipsecif_fn);
	
	att_f = fopen(ipsecif_fn,"w");
	if(NULL==att_f)
	{
		Err_log("fopen file(%s) failed!\n ",ipsecif_fn);
		return -1;
	}

	{
		fprintf(att_f,"#!/bin/sh\n");
		fprintf(att_f,"echo 0> /proc/sys/net/ipv4/%s/rp_filter\n",ifname);
		fprintf(att_f,"echo\n");
		fprintf(att_f,"/bin/ipsec/tncfg --attach --virtual %s"
	 	                    " --physical %s\n",ipsec_if,ifname);
		fprintf(att_f,"echo\n");
		fprintf(att_f,"ifconfig %s inet %s broadcast %s netmask %s\n"
				,ipsec_if,wan_ip,broadcast,netmask);
		fprintf(att_f,"echo\n");
		fprintf(att_f,"ifconfig %s mtu 1430\n",ipsec_if);
		fprintf(att_f,"echo\n");
		fprintf(att_f,"echo 0> /proc/sys/net/ipv4/%s/rp_filter\n",ipsec_if);
	}
	fclose(att_f);
	eval("/bin/sh",att_f);

	//nvram set default wan ipsec ifname
	{
		nvram_set(DEFAULT_IPSECIF_CNF,ipsec_if);
	}
	return 0;
} 
/***********************************************************
 *set_iptables:	set iptables rules
 *because the quickvpn use the  default WAN,
 *so we no need set iptables rules for ipsec interface
 */
int set_iptables(const char *ipsecif)
{
}
#endif	

/**********************************************************
  *is_fix_confuse
  */
  int is_fix_confuse(quick_vpn_t *p_vpn)
{
	char cfg_buf[100];
	char *next;
	char client_ip[20];
	char client_mask[20];
	char *sg_ip;
	char status[3];
	char *domain,*other, *subnet;
	int which,i;
	
	for(i = 0; i < MAX_NOMAD; i++)
	{
		which = i;
		clear_array(cfg_buf);
		clear_array(status);
		foreach(cfg_buf, nvram_safe_get("entry_config"), next)
		{
			other = cfg_buf;
			if (which-- == 0)
			{
				strcpy(status, strsep(&other, ":"));
				break;
			}
		}
		if(strcmp(status,"1"))
			continue;
		
		//clear_array(sg_ip);
		which = i;
		foreach(cfg_buf, nvram_safe_get("sg_config"), next)
	    {
	    	domain = cfg_buf;
			if (which-- == 0) 
			{
			 	sg_ip = strsep(&domain, ":");
				//sscanf(cfg_buf, "%20[^:]:%50[^\n]", sg_ip,ser_sg_domain);
				if(!strcmp(sg_ip,p_vpn->post_nat_ip))
					return 1;
	
				break;
			}
		}
		
		clear_array(cfg_buf);	
		clear_array(client_ip);
		clear_array(client_mask);
		which = i;
		foreach(cfg_buf, nvram_safe_get("rtype_config"), next)
		{
			subnet = cfg_buf;
			if (which-- == 0) 
			{
				int cip[4], mask[4], sw[4];

				char client_sub[20];
				char sw_sub[20];
				char *sptemp;
				clear_array(client_sub);
				clear_array(sw_sub);
				//sscanf(cfg_buf, "%20[^:]:%20[^\n]", client_ip,client_mask);
				sptemp = strsep(&subnet, ":");
				strcpy(client_ip, sptemp);
				if(strncmp(subnet, "255", 3))
					strcpy(client_mask, "255.255.255.255");
				else
					strcpy(client_mask, subnet);
				sscanf(client_ip, "%d.%d.%d.%d", &cip[0], &cip[1], &cip[2], &cip[3]);
				sscanf(client_mask, "%d.%d.%d.%d"
					 , &mask[0], &mask[1], &mask[2], &mask[3]);
				sscanf(p_vpn->post_nat_ip, "%d.%d.%d.%d"
					 , &sw[0], &sw[1], &sw[2], &sw[3]);
				sprintf(client_sub, "%d.%d.%d.%d"
					 , cip[0]&mask[0], cip[1]&mask[1], cip[2]&mask[2], cip[3]&mask[3]);
				sprintf(sw_sub, "%d.%d.%d.%d"
					 , sw[0]&mask[0], sw[1]&mask[1], sw[2]&mask[2], sw[3]&mask[3]);
				if(!strcmp(client_sub,sw_sub))
					return 1;
			 			
			break;
			}
			
		}		
	
	}
	return 0;
}
/************************************************************
 *start_quickvpn:configure one chennel VPN with QuickVPN 
 *and listen
 */
int start_quickvpn(quick_vpn_t *p_vpn, int index)
{
	char connt_fname[24],del_fname[24],psk_fn[30];
	char wan_ip[20],next_hop[20],lan_ip[20],lan_mask[20],lan_sub[20];
	//int index;
 	FILE *f_con,*f_del,*f_psk;
	//int is_behind_nat=0;
	char remote_host[]="\"%direct\"";

	if(NULL==p_vpn)
	{
		Err_log("%s %s input parameter p_vpn=NULL !!!\n",__FILE__,__FUNCTION__);
	}

	if(is_fix_confuse(p_vpn))
		return NOMAD_IP_USED;
	//create pluto if it does not run
	{
		int f_pluto =run_pluto();//flag of  pluto process
		if(-1 ==f_pluto)
		{
			Err_log("create PLUTO task failde !!!\n");
			return NOMAD_FAILED_PROCESS;
		}	
	}	
	memset(connt_fname,0,sizeof(connt_fname));
	memset(del_fname,0,sizeof(del_fname));
	memset(psk_fn,0,sizeof(psk_fn));
	memset(wan_ip,0,sizeof(wan_ip));
	memset(lan_ip,0,sizeof(lan_ip));
	memset(lan_mask,0,sizeof(lan_mask));
	memset(next_hop,0,sizeof(next_hop));
	memset(lan_sub,0,sizeof(lan_sub));

   	//index=add_vpn_desc(p_vpn->username);
   	/*if(index < 1 || index > 5)
	{
		return NOMAD_SERVER_ERR;
	}*/
	sprintf(connt_fname,"%s" "%s%d",QUICKVPN_DIR,CONN_FN_PRE,index);
	ZHS_DEBUG("connt_fname is %s",connt_fname);
	sprintf(del_fname,"%s" "%s%d",QUICKVPN_DIR,DELT_FN_PRE,index);
	ZHS_DEBUG("del_fname is %s\n",del_fname);
	//nvram get lansub,wanip,next hop
	strcpy(lan_ip,nvram_safe_get("lan_ipaddr"));
	strcpy(lan_mask,nvram_safe_get("lan_netmask"));
	{
		int a[4], b[4];

		sscanf(lan_ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
		sscanf(lan_mask, "%d.%d.%d.%d", &b[0], &b[1], &b[2], &b[3]);
		sprintf(lan_sub, "%d.%d.%d.%d", a[0]&b[0], a[1]&b[1], a[2]&b[2], a[3]&b[3]);
	}
	if(get_def_wan(wan_ip,next_hop,NULL,NULL,NULL))
	{	
		Err_log("no wan connected and can not start QUICKVPN!!\n");
		return NOMAD_SERVER_ERR;
	}

	/* zhangbin for configuring ipsecN only for DEBUGGING
	{
		FILE *cfg_ipsec;
		char cfg_ipsec_name[50];
		memset(cfg_ipsec_name,0,sizeof(cfg_ipsec_name));
		sprintf(cfg_ipsec_name,"%s%s",QUICKVPN_DIR,"ipsec_0");
		cfg_ipsec = fopen(cfg_ipsec_name,"w");
		if(NULL == cfg_ipsec)
		{
			printf("Create %s failed!\n",cfg_ipsec_name);
			return -1;
		}
		
		fprintf(cfg_ipsec,"#!/bin/sh\n");
		fprintf(cfg_ipsec,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n","nas0");
		fprintf(cfg_ipsec,"echo\n");

		fprintf(cfg_ipsec,"/bin/ipsec/tncfg --attach --virtual %s --physical %s\n","ipsec0","nas0");
		fprintf(cfg_ipsec,"echo\n");
		#if 0
		if(!strncmp(rc_wan_ifname,"ppp",3))
			fprintf(cfg_ipsec,"ifconfig %s inet %s netmask %s dstaddr %s\n",ipsec_if,rc_wan_ipaddress,rc_wan_netmask,rc_wan_gateway);
		else
		#endif

		fprintf(cfg_ipsec,"ifconfig %s inet %s broadcast %s netmask %s\n","ipsec0",wan_ip,"157.0.0.255","255.255.255.0");
		
		fprintf(cfg_ipsec,"echo\n");
		fprintf(cfg_ipsec,"ifconfig %s mtu 1430\n","ipsec0");
		//fprintf(file,"echo\n");
		fprintf(cfg_ipsec,"echo\n");
		fprintf(cfg_ipsec,"echo 0 > /proc/sys/net/ipv4/conf/%s/rp_filter\n","ipsec0");
		fprintf(cfg_ipsec,"ifconfig ispec0 %s\n");
		fclose(cfg_ipsec);
		eval("/bin/sh",cfg_ipsec_name);
	}*/
#if 0 
	{//get default wan configuration and attach ipsec if to default wan
		char ifname[5],broadcast[20],netmask[20];
		
		clear_array(ifname);
		clear_array(broadcast);
		clear_array(netmask);
		
		if(get_def_wan(wan_ip,next_hop,ifname,broadcast,netmask))
		{	
			Err_log("no wan connected and can not start QUICKVPN!!\n");
			return -1;
		}
		if(attach_ipsec_if(wan_ip,ifname,broadcast,netmask))
		{
			Err_log("attach ipsec interface to WAN interface failed\n");
			return -1;
		}
	}
#endif
	//create Preshared Key file
	sprintf(psk_fn,"%s" "%d%s",PRESHARED_FILE,index,QV_SECR_FN_POST);
	ZHS_DEBUG("psk_fname is %s\n",psk_fn);
	f_psk=fopen(psk_fn,"w");
	if(NULL==f_psk)
	{	
		Err_log("fopen file(%s) ERROR!!\n",psk_fn);
		return NOMAD_EROPEN_FILE;
	}	
	fprintf(f_psk,"%s %s : PSK \"%s\"\n"
		,wan_ip,p_vpn->pre_nat_ip,p_vpn->random_psk);
	fclose(f_psk);

//	is_behind_nat =strcpm(p_vpn->pre_nat_ip,p_vpn->post_nat_ip);	
	//configure the tunnel
	f_con=fopen(connt_fname,"w");
	if(NULL==f_con)
	{
		Err_log("fopen file (%s) ERROR!!\n",connt_fname);
		return NOMAD_EROPEN_FILE;
	}
	fprintf(f_con,"#!/bin/sh\n");
//	fprintf(f_con,"echo start to connect tunnel %s......\n",p_vpn->username);
	fprintf(f_con,"/bin/ipsec/whack");
	fprintf(f_con," --name %s --encrypt --tunnel --esp 3des-md5-96",p_vpn->username);
	fprintf(f_con," --ike 3des-md5-modp1024 --pfs --psk");
	fprintf(f_con," --host %s --client %s/%s --nexthop %s ",wan_ip,lan_sub,lan_mask,next_hop);
	fprintf(f_con," --to --host %s --client %s/32 --id %s --nexthop %s"
		,p_vpn->post_nat_ip,p_vpn->pre_nat_ip,p_vpn->pre_nat_ip,remote_host);
	//fprintf(f_con," --to --host %s --nexthop %s"
		//,p_vpn->post_nat_ip,/*p_vpn->pre_nat_ip,p_vpn->pre_nat_ip,*/remote_host);
	fprintf(f_con," --rekeymargin \"10\" --keyingtries 2\n");	
	fprintf(f_con,"echo\n");
	//listen
	fprintf(f_con,"/bin/ipsec/whack --listen\n");
	//rouete
/*	fprintf(f_con,"echo\n");
	fprintf(f_con,"echo\n");
	fprintf(f_con,"/bin/ipsec/whack --route --name %s\n",p_vpn->username);*/
	//connect
	fclose(f_con);
	eval("/bin/sh",connt_fname);
	//create delete tunnel file
	f_del=fopen(del_fname,"w");
	if(NULL==f_del)
	{
		Err_log("fopen file (%s) ERROR!!\n",del_fname);
		return NOMAD_EROPEN_FILE;
	}
	fprintf(f_del,"#!/bin/sh\n");
	//configure the tunnel
//	fprintf(f_del,"echo delete tunnel %s\n",p_vpn->username);
	fprintf(f_del,"/bin/ipsec/whack --terminate --name %s\n",p_vpn->username);
	fprintf(f_del,"/bin/ipsec/whack --unroute --name %s\n",p_vpn->username);
	fprintf(f_del,"/bin/ipsec/whack --delete --name %s\n",p_vpn->username);
       fprintf(f_del,"echo\n");
	//fprintf(f_del,"/bin/ipsec/whack --unroute --name %s\n",p_vpn->username);
	fprintf(f_del,"rm %s\n",connt_fname);
	fprintf(f_del,"rm %s\n",psk_fn);
	fclose(f_del);
	return 0;	
}

/**********************************************************************************
 *
 */
int clear_quickvpn(quick_vpn_t *del, int index)
{
 	//int index;
	char del_fname[24];
//	char conn_fname[24];
//	char psk_fname[36];
	FILE *f_del;

	//find index and delete the vpn desc from list	
	//index=find_index_by_name(del->username);
	if(index < 1 || index > 5)
	{
		Err_log("no the tunnel name:%s\n",del->username);
		return NOMAD_UNKNOWN_USER;
	}
	//delete_vpn_desc(index);
        //delete VPN tunnel 
//	memset(connt_fname,0,sizeof(connt_fname));
	memset(del_fname,0,sizeof(del_fname));
//	memset(psk_fname,0,sizeof(psk_fn));
	
	sprintf(del_fname,"%s" "%s%d",QUICKVPN_DIR,DELT_FN_PRE,index);
//	sprintf(connt_fname,"%s" "%s%d",QUICKVPN_DIR,CONN_FN_PRE,index);
//	sprintf(psk_fname,"%s" "%d%s",QUICKVPN_DIR,index,SECR_FN_POST);
	f_del=fopen(del_fname,"r");
	if(NULL==f_del)	
	{	
		Err_log("ERROR,no such file:%s\n",del_fname);
		return NOMAD_EROPEN_FILE;
	}
	fclose(f_del);
	eval("/bin/sh",del_fname);
        //rm the delete file
	eval("rm",del_fname);
	return 0;
}

#endif

