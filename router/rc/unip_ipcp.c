/* This file is unnumbered IP support */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <epivers.h>
#include <bcmnvram.h>
#include <mtd.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>
#include <bcmdevs.h>

#include <utils.h>
#include <cyutils.h>
#include <code_pattern.h>
#include <cy_conf.h>
#include <support.h>
#include "pvc.h"
#include "ledapp.h"

#define    UNIP_IMMFILE                "/tmp/ppp/realaddr"
#define    UNIP_MUTIFILE              "/tmp/ppp/unip"
//#define    UNIP_IMMFILE                "/tmp/realaddr"
//#define    UNIP_MUTIFILE              "/tmp/unip"

extern struct vcc_config *vcc_config_ptr;

//junzhao 2005.1.24 
//for unip dhcpd lease time change
extern int unip_started;

/* save unip info */
int  unip_save(int  pvc)
{
        FILE* fp;
	 char ifgbuf[480];
	 int unip[8][4];
	 int unmask[8][4];
	 int r;
	 char word[48];
	 char* next;
	 int i = 0;
	 int curip[4];
	 int curmask[4];

        /* check /tmp/ppp/unip exist ? */
	 //junzhao 2005.1.22 
	 //mkdir("/tmp/ppp", 0777);

	 fp = fopen(UNIP_MUTIFILE,"r");
	 if(fp == NULL){
	 	printf("First Create unip multi-file\n");
		fp = fopen(UNIP_MUTIFILE,"w+");
	 }

	 memset(ifgbuf,0,sizeof(ifgbuf));
        memset(unip,0,sizeof(unip));
        memset(unmask,0,sizeof(unmask));

	 r = fread(ifgbuf,1,sizeof(ifgbuf),fp);
	 
	 if(r < 32){
	 	printf("This file is empty!!!!\n");
	 }
	 else{
	 	printf("ifgbuf = %s\n",ifgbuf);
	 	memset(word,0,sizeof(word));
	 	foreach(word, ifgbuf, next){
			if(word)
			    sscanf(word,"%d.%d.%d.%d@%d.%d.%d.%d",&unip[i][0],&unip[i][1],&unip[i][2],&unip[i][3],&unmask[i][0],&unmask[i][1],&unmask[i][2],&unmask[i][3]);
			i++;
	 	}
	 }

	 memset(word,0,sizeof(word));
	 if(!file_to_buf(UNIP_IMMFILE, word, sizeof(word))){
	 	printf("UNIP IMM File is not exist, UNIP Function Failed\n");
		fclose(fp);
		return -1;
	 }

	 memset(curip,0,sizeof(curip));
	 memset(curmask,0,sizeof(curmask));
        /* get current unip ip and mask */
	 sscanf(word,"%d.%d.%d.%d@%d.%d.%d.%d",&curip[0],&curip[1],&curip[2],&curip[3],&curmask[0],&curmask[1],&curmask[2],&curmask[3]);	

        if(pvc >=0 && pvc <8){
		unip[pvc][0] = curip[0];
		unip[pvc][1] = curip[1];
		unip[pvc][2] = curip[2];
		unip[pvc][3] = curip[3];

		unmask[pvc][0] = curmask[0];
		unmask[pvc][1] = curmask[1];
		unmask[pvc][2] = curmask[2];
		unmask[pvc][3] = curmask[3];
        }
	 else{
	 	printf("Invalid PVC number\n");
		fclose(fp);
		return -1;
	 }

	 /* save new */
	 memset(ifgbuf,0,sizeof(ifgbuf));
	 for(i=0;i<8;i++){
	 	if(!i){
			snprintf(ifgbuf,sizeof(ifgbuf),"%s%d.%d.%d.%d@%d.%d.%d.%d",ifgbuf,unip[i][0],unip[i][1],unip[i][2],unip[i][3],unmask[i][0],unmask[i][1],unmask[i][2],unmask[i][3]);
	 	}
		else{
			snprintf(ifgbuf,sizeof(ifgbuf),"%s %d.%d.%d.%d@%d.%d.%d.%d",ifgbuf,unip[i][0],unip[i][1],unip[i][2],unip[i][3],unmask[i][0],unmask[i][1],unmask[i][2],unmask[i][3]);
		}
	 }

	 	
	 fclose(fp);

	 /* save new */
	 printf("new ifgbuf = %s\n",ifgbuf);
	 fp = fopen(UNIP_MUTIFILE,"w");
	 if(fp == NULL){
	 	printf("Program must not reach here\n");
	 	return -1;
	 }
	 fwrite(ifgbuf,1,sizeof(ifgbuf),fp);
	 fclose(fp);
	 

	 return 0;
}


/* get unip info */
int unip_get(int pvc,char* unip,char* unmask,char* unnet)
{
        FILE* fp;
	 char ifgbuf[480];	
	 int r;	
	 char word[48];
	 char* next;
	 int i = 0;
	 int unip_tmp[4];
	 int unmask_tmp[4];
	 int unnet_tmp[4];

	 //zhangbin
	 int unip_last;


        if(pvc <0 || pvc >=8){
		printf("Invalid PVC number\n");	
		return -1;
        }

	 if(!unip || !unmask || !unnet){
	 	printf("Invalid pointer\n");
		return -1;
	 }

	 fp = fopen(UNIP_MUTIFILE,"r");
	 if(fp == NULL){
	 	printf("/tmp/ppp/unip is not exist!unip funtion failed\n");
		return -1;
	 }
        

        r = fread(ifgbuf,1,sizeof(ifgbuf),fp);
	 
	 if(r < 32){
	 	printf("This file is bad!!!!\n");
		return -1;
	 }
	 else{
	 	printf("ifgbuf = %s\n",ifgbuf);
	 	memset(word,0,sizeof(word));
	 	foreach(word, ifgbuf, next){
			if(i == pvc)
				break;
			i++;
	 	}
	 }

	 memset(unip_tmp,0,sizeof(unip_tmp));
	 memset(unmask_tmp,0,sizeof(unmask_tmp));
	 memset(unnet_tmp,0,sizeof(unnet_tmp));
	 
	 printf("current unip info : word = %s\n",word);
	 sscanf(word,"%d.%d.%d.%d@%d.%d.%d.%d",&unip_tmp[0],&unip_tmp[1],&unip_tmp[2],&unip_tmp[3],&unmask_tmp[0],&unmask_tmp[1],&unmask_tmp[2],&unmask_tmp[3]);

	 //zhangbin
	unip_last = (unip_tmp[3] & unmask_tmp[3]) + 1;
	 
	 memset(word,0,sizeof(word)); 
	 //zhangbin
        //snprintf(word,sizeof(word),"%d.%d.%d.%d",unip_tmp[0],unip_tmp[1],unip_tmp[2],unip_tmp[3]);
        snprintf(word,sizeof(word),"%d.%d.%d.%d",unip_tmp[0],unip_tmp[1],unip_tmp[2],unip_last);
	 strcpy(unip,word); 	
	 memset(word,0,sizeof(word));
        snprintf(word,sizeof(word),"%d.%d.%d.%d",unmask_tmp[0],unmask_tmp[1],unmask_tmp[2],unmask_tmp[3]);
	 strcpy(unmask,word); 		
	 memset(word,0,sizeof(word));
	 snprintf(word,sizeof(word),"%d.%d.%d.%d",unip_tmp[0],unip_tmp[1],unip_tmp[2],0);	
	 strcpy(unnet,word);  
        printf("unip = %s\n",unip);
	 printf("unmask = %s\n",unmask);	
	 printf("unnet = %s\n",unnet);

		  
        fclose(fp);
	 return 0;
}

/* modify iptables called by firewall.c */
int unip_iptables(void)
{
      //   char unip[32],unmask[32],unnet[32];
	  //int ret;	 
	  //char iface[10];
	  //char ipt_source[48];
	  //int pvc;
         
       //  if(nvram_match("unip_enable","0"))
		 //return 0;	
      //   pvc = atoi(nvram_safe_get("wan_connection"));
	  //memset(unip,0,sizeof(unip)); 	
	  //memset(unmask,0,sizeof(unmask));
	 // memset(unnet,0,sizeof(unnet));

      //   ret = unip_get(pvc,unip,unmask,unnet);
	  //if(ret < 0)
	 	//return ret;  

	  //memset(iface,0,sizeof(iface));
	  //snprintf(iface,sizeof(iface),"ppp%d",pvc);
	  //memset(ipt_source,0,sizeof(ipt_source));
	  //snprintf(ipt_source,sizeof(ipt_source),"%s/%s",unip,unmask);

	  /* iptables */
	  //printf("kirby debug: iptables -t nat -A POSTROUTING\n");
	  //eval("iptables","-t", "nat", "-A", "POSTROUTING", "-s", ipt_source, "-o", iface, "-j", "SNAT", "--to",unip);
	  //eval("iptables", "-A", "FORWARD", "-i", "br0", "-o", iface, "-p", "tcp", "-m", "tcp", "--tcp-flags", "SYN,RST", "SYN", "-j", "TCPMSS", "--set-mss=1440");
	  //unip_filter_forward();
	  return 0;

}

/* get encap */
void unip_get_encap(int pvc,char* encap)
{
	char *next;
	char vccbuf[240];
	char* running;
	char* token;
	char word[100];

	memset(vccbuf,0,sizeof(vccbuf));
	memset(word,0,sizeof(word));
	strcpy(vccbuf,nvram_safe_get("vcc_config"));
	
	
	foreach(word, vccbuf, next) 
	{
		if (pvc-- == 0) 
		{
			running = word;
			token = strsep(&running, ":");
			if (!token )
				break;
			token = strsep(&running, ":");
			if (!token )
				break;
			token = strsep(&running, ":");
			if (!token )
				break;
			strcpy(encap,token);

			break;
		}
	}
}

/* unip funtion */
//junzhao 2005.1.24
int unip_start(int pvc)
{
        //int pvc;
	 char unip[32],unmask[32],unnet[32];
	 char iface[10];
	 char ipt_source[48];
	 int i_unip[4];
	 int i_unmask[4];
	 int dhcp_start;
	 int dhcp_num;
	 int dhcp_and;
	 char dhcps[4];
	 char dhcpn[4];
	 int ret;
	 char encap[30];

        /* active wan pvc == 0??? */
	 //pvc = atoi(nvram_safe_get("wan_active_connection"));	
	 if(pvc != 0){
	 	printf("Unnumbered IP must be bind in pvc0");
		return -1;
	 }	
	 /* enable UNIP ???*/
	 //memset(encap,0,sizeof(encap));
	 //unip_get_encap(pvc,encap);
        //printf("encapmode = %s\n",encap);
	 //if(strcmp(encap,"pppoe") && strcmp(encap,"pppoa"))
	 //	return -1;
	 
	 //pvc = atoi(nvram_safe_get("wan_active_connection"));
	 ret = unip_save(pvc);
	 if(ret < 0)
	 	return ret;
	 sleep(2);
	 memset(unip,0,sizeof(unip));
	 memset(unmask,0,sizeof(unmask));
	 memset(unnet,0,sizeof(unnet));
	 ret = unip_get(pvc,unip,unmask,unnet);
        if(ret < 0)
	 	return ret;
	 /* set lan */
	 if(strcmp(nvram_safe_get("lan_ipaddr"),unip))	
              unlink("/tmp/udhcpd.leases");
        if(strcmp(nvram_safe_get("lan_netmask"),unmask))	
		unlink("/tmp/udhcpd.leases");

	 /* make param */
	 memset(iface,0,sizeof(iface));
	 snprintf(iface,sizeof(iface),"ppp%d",pvc);
	 memset(ipt_source,0,sizeof(ipt_source));
	 snprintf(ipt_source,sizeof(ipt_source),"%s/%s",unnet,unmask);
	 /**/

         /* kirby for unip: set lan and restart dhcp service */
	 printf("Change Wan IP to 1.1.1.1\n");
	 //eval("ifconfig",iface,"1.1.1.1","netmask","255.255.255.0");   
	 eval("ifconfig",iface,"10.234.1.9","netmask","255.255.255.0");   
	 eval("route","del","default");
	 eval("route","add","default","dev",iface);

        /* set unip mode */
	 nvram_set("unip_enable","1");
	 /* disable NAT */	
	 nvram_set("nat_enable","0");
	 //nvram_set("wk_mode","router");
	
        nvram_set("lan_ipaddr",unip);

	//zhangbin 2005.1.31
	if(unmask[0])
		 nvram_set("lan_netmask",unmask);
	else
		 nvram_set("lan_netmask","255.255.255.0");

	 /* hack limited  dhcp start = 100 dhcp user = 20 */
	 sscanf(unip,"%d.%d.%d.%d",&i_unip[0],&i_unip[1],&i_unip[2],&i_unip[3]);
	 sscanf(unmask,"%d.%d.%d.%d",&i_unmask[0],&i_unmask[1],&i_unmask[2],&i_unmask[3]);

	 /* now we only support C addr */
        dhcp_start = i_unip[3] + 1;
        dhcp_num = 255 - dhcp_start;
        memset(dhcps,0,sizeof(dhcps));
	memset(dhcpn,0,sizeof(dhcpn));
	snprintf(dhcps,sizeof(dhcps),"%d",dhcp_start);
	snprintf(dhcpn,sizeof(dhcpn),"%d",dhcp_num);
        printf("dhcp start = %s\n",dhcps);
	 printf("dhcp max user = %s\n",dhcpn);	
	
	 nvram_set("dhcp_start",dhcps);
	 //zhangbin
	 nvram_set("dhcp_num","5");		
	 //nvram_set("dhcp_num",dhcpn);		

	 stop_lan();
	 start_lan();

	 //junzhao 2005.1.24 
	 /* unip started */
	 unip_started = 1;
	 
	 stop_dhcpd();
	 start_dhcpd();

	 //junzhao 2005.1.24 
	 /* unip has started */
	 unip_started = 0;
	 
	 stop_firewall();
	 start_firewall();

	 return 0;
	 
}
