/*
 * ppp scripts
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ppp.c,v 1.4 2005/08/25 09:04:16 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>

#include <code_pattern.h>
#include <cy_conf.h>
#include <utils.h>

#include "pvc.h"
#include "ledapp.h"

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

extern pid_t ppp_pid[MAX_CHAN];
extern int which_conn;

#ifdef CWMP_SUPPORT
//by add peny 2004/10.26 --start_dhcpc end  via send dhcp inform  message get options inform
extern pid_t udhcpc_pid[MAX_CHAN];
#define INFORM_DHCP_PID_FILE "/var/run/inform_dhcp_pid"
int stop_inform_dhcpc(int whichconn)
{
	
	char pidstr[32];
	FILE *fp; 
	char *tmp = NULL;

	/* da_f@2005.3.4 start. add
	 * Do not use Macro INFORM_DHCP_PID_FILE any more */
	char pidfile[32];

	bzero(pidfile, sizeof(pidfile));
	sprintf(pidfile, "/var/run/udhcpc%d.pid", whichconn);
	/* da_f@2005.3.4 end */

	/* da_f@2005.3.4 modify
	 * Do not need "enable_inform" any more.
	nvram_set("enable_inform","0"); */

	fp = fopen(pidfile, "r"); /* da_f@2005.3.4 modify */
	if(fp == NULL)
	{
		printf("no udhcpc work\n");
		return 0;
	}
		
	memset(pidstr, 0, sizeof(pidstr));
	if(fread(pidstr, 1, sizeof(pidstr), fp) > 0) 
	{
		if((tmp = strstr(pidstr, "\n")) != NULL)
			*tmp = '\0';
	}	
	fclose(fp);
		
	eval("kill", "-9", pidstr);
	unlink(pidfile); /* da_f@2005.3.4 modify */
	return 0;
}

/* da_f@2005.3.4 modify
 * Add "int whichconn" parameter that represents the current PVC */
void inform_start_dhcpc(char *wifname, char *ipaddr, int whichconn)
{
        char devname[10];
        int i;
        int randomtime;
        int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	
		/* da_f@2005.3.4 start. add
		 * Do not use Macro INFORM_DHCP_PID_FILE any more */
		char pidfile[32];

		bzero(pidfile, sizeof(pidfile));
		sprintf(pidfile, "/var/run/udhcpc%d.pid", whichconn);
		/* da_f@2005.3.4 end */

		//junzhao 2005.2.17 for check cwmp enable
		if(nvram_invmatch("cwmp_enable", "1"))
			return;
	
        nvram_set("inform_ip",ipaddr);

		/* da_f@2005.3.4 modify
		 * Do not need "enable_inform" any more
        nvram_set("enable_inform","1"); */

        cprintf("the interface it's %s\n",wifname);

        srand((int)time(0));
        randomtime = 1 + (int) (5.0 * rand()/(RAND_MAX + 1.0));
        printf("random time %d\n", randomtime);
        sleep(randomtime);
        memset((char *)devname, 0, sizeof(devname));
        strcpy(devname,wifname);
	
        {
          char *wan_hostname = nvram_get("wan_hostname");
          char *dhcp_argv[] =
           {
                "udhcpc",
				"-e", /* da_f@2005.3.4 add. Enable send inform packet when udhcpc startup */
                "-i", devname,
                "-p", pidfile, /* da_f@2005.3.4 modify */
                "-s", "/tmp/udhcpc",
                wan_hostname && *wan_hostname ? "-H" : NULL,
                wan_hostname && *wan_hostname ? wan_hostname : NULL,
                NULL
	    };
          symlink("/sbin/rc", "/tmp/udhcpc");

          _eval(dhcp_argv, NULL, 0, &udhcpc_pid[which_conn]);

          for(i=0; i<100000000; i++)
	          ;

        }
 }
#endif
/*
 * Called when link comes up
 */
int
ipup_main(int argc, char **argv)
{
	FILE *fp;
//	char *wan_ifname = safe_getenv("IFNAME");
	char *wan_ifname = argv[1];
	char *value;
	char *filename;
	char buf[64];
	char dnstmp[128];
	char encap[15];
        char *local_ip;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));

	//printf("%s\n", argv[0]);
	
	//eval("killall", "-9", "listen");

	if(check_action() != ACT_IDLE)
                return -1;

	/* ipup will receive bellow six arguments */
	/* interface-name  tty-device  speed  local-IP-address   remote-IP-address ipparam */
	/* Touch connection file  for current connection */

	which_conn = atoi(&wan_ifname[3]);  //set the global flag!!!!important

	if(which_conn < which_tmp)
	{
		char index[3];
		eval("/usr/bin/route", "del", "default");
		memset(index, 0, sizeof(index));
		sprintf(index, "%d", which_conn);
		nvram_set("wan_active_connection", index);
		nvram_set("wan_enable_last", index);
		which_tmp = which_conn;
	}
	
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which_conn > MAX_CHAN-1)
		which_conn -= MAX_CHAN;
	memset(encap, 0, sizeof(encap));
	check_cur_encap(which_conn, encap);
#endif

	if(!strcmp(argv[6], "0"))
	{
		filename = malloc(strlen("/tmp/ppp/link.down") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link.down"+3));
		sprintf(filename, "/tmp/ppp/link%d.down", which_conn);
		unlink(filename);

		//junzhao 2004.10.28
		memset(filename, 0, strlen("/tmp/ppp/link.idle"+3));
		sprintf(filename, "/tmp/ppp/link%d.idle", which_conn);
		unlink(filename);
		
		free(filename);

		filename = malloc(strlen("/tmp/ppp/link") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link"+3));
		sprintf(filename, "%s%d", "/tmp/ppp/link", which_conn);
		if (!(fp = fopen(filename, "a"))) 
		{
			perror(filename);
			return errno;
		}
		fprintf(fp, "%s\n", argv[1]);
	//fprintf(fp, "%s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
		fclose(fp);
		free(filename);
		filename = NULL;
	}

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else if(!strcmp(argv[6], "1"))
	{
		filename = malloc(strlen("/tmp/ppp/link.down") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link.down"+3));
		sprintf(filename, "/tmp/ppp/link%d.down", MAX_CHAN + which_conn);
		unlink(filename);

		//junzhao 2004.10.28
		memset(filename, 0, strlen("/tmp/ppp/link.idle"+3));
		sprintf(filename, "/tmp/ppp/link%d.idle", MAX_CHAN + which_conn);
		unlink(filename);
		
		free(filename);

		filename = malloc(strlen("/tmp/ppp/link") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link"+3));
		sprintf(filename, "%s%d", "/tmp/ppp/link", MAX_CHAN + which_conn);
		if (!(fp = fopen(filename, "a"))) 
		{
			perror(filename);
			return errno;
		}
		fprintf(fp, "%s\n", argv[1]);
		//fprintf(fp, "%s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
		fclose(fp);
		free(filename);
		filename = NULL;
	}
#endif

	if ((value = getenv("IPLOCAL"))) 
	{
		//nvram_modify_status("wan_ipaddr", argv[4]);
		ifconfig(wan_ifname, IFUP, value, "255.255.255.255");
	}
	
	if(!strcmp(argv[6], "0"))
	{
		if(which_conn == which_tmp)
		{
 			if ((value = getenv("IPREMOTE")))
			{
				//nvram_modify_status("wan_gateway", value);
        			route_add(wan_ifname, 0, "0.0.0.0", value, "0.0.0.0");
			}
		}
	}
	
	fp = fopen("/tmp/ppp/resolv.conf", "r");
	if(fp != NULL)
	{
		char *begin, *end;
		int count = 0;
		memset(buf, 0, sizeof(buf));
		memset(dnstmp, 0, sizeof(dnstmp));
		
		if(fread(dnstmp, 1, sizeof(dnstmp), fp) > 0)
			printf("dnstmp %s\n", dnstmp);
		end = dnstmp;
		while((begin = strstr(end, "nameserver"))!=NULL)
		{
			if(count++ == 2)
				break;	
			begin += strlen("nameserver") + 1;
			end = strstr(begin, "\n");
			*end='\0';							                sprintf(&buf[strlen(buf)],"%s%s", strlen(buf)>0?":":"",begin);
	               	end ++;									}
		if(count == 1)
			sprintf(&buf[strlen(buf)], ":0.0.0.0");
		else if(count == 0)
			sprintf(&buf[0], "0.0.0.0:0.0.0.0");
								                		buf[strlen(buf)] = '\0';
		
		if(!strcmp(argv[6], "0"))
			ram_modify_status(WAN_GET_DNS, buf);
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		else
		{	
			char pdns[16], sdns[16], *wangateway;	
			int flag = 1;
			if(!strcmp(encap, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				ram_modify_status(WAN_MPPPOE_GET_DNS, buf);
			#else
				flag = 0;
			#endif
			}
			else if(!strcmp(encap, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				ram_modify_status(WAN_IPPPOE_GET_DNS, buf);
			#else
				flag = 0;
			#endif
			}
			if(flag)
			{
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(buf, "%16[^:]:%16[^\0]", pdns, sdns);
 				wangateway = getenv("IPREMOTE");
				
				if(strcmp(pdns, "0.0.0.0"))
					eval("/usr/bin/route", "add", "-host", pdns, "gw", wangateway, "dev", wan_ifname);
				//route_add(wan_ifname, 0, pdns, NULL, "255.255.255.255");
				if(strcmp(sdns, "0,0.0.0"))
					eval("/usr/bin/route", "add", "-host", sdns, "gw", wangateway, "dev", wan_ifname);
				//route_add(wan_ifname, 0, sdns, NULL, "255.255.255.255");
			}
		}
	#endif		
		fclose(fp);
	}
	
	//junzhao 2004.4.19 get ac_name
	if(!strcmp(argv[6], "0"))
	{
		{
		char acname[1024];
		if(file_to_buf("/tmp/ppp/acname", acname, sizeof(acname)))
			ram_modify_status(WAN_PPPOE_ACNAME, acname);
		}
		
		{
		char srvname[1024];
		if(file_to_buf("/tmp/ppp/srvname", srvname, sizeof(srvname)))
			ram_modify_status(WAN_PPPOE_SRVNAME, srvname);
		}

		//if ((value = getenv("AC_NAME")))
			//ram_modify_status(WAN_PPPOE_ACNAME, value);
        	//if ((value = getenv("SRV_NAME")))
			//ram_modify_status(WAN_PPPOE_SRVNAME, value);
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else
	{
		int flag = 1;
		{
		char acname[1024];
		if(file_to_buf("/tmp/ppp/acname", acname, sizeof(acname)))
		{
			if(!strcmp(encap, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				ram_modify_status(WAN_MPPPOE_ACNAME, acname);
			#else
				flag = 0;
			#endif
			}
			else if(!strcmp(encap, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				ram_modify_status(WAN_IPPPOE_ACNAME, acname);
			#else
				flag = 0;
			#endif
			}
		}
		}
		
		{
		char srvname[1024];
		if(file_to_buf("/tmp/ppp/srvname", srvname, sizeof(srvname)))
		{
			if(!strcmp(encap, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				ram_modify_status(WAN_MPPPOE_SRVNAME, srvname);
			#else
				flag = 0;
			#endif
			}
			else if(!strcmp(encap, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				ram_modify_status(WAN_IPPPOE_SRVNAME, srvname);
			#else
				flag = 0;
			#endif
			}
		}
		}

		//if ((value = getenv("AC_NAME")))
			//ram_modify_status(WAN_MPPPOE_ACNAME, value);
        	//if ((value = getenv("SRV_NAME")))
			//ram_modify_status(WAN_MPPPOE_SRVNAME, value);
	}
#endif
	{
	char filename[30];
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/authfail%d", atoi(&wan_ifname[3])); 
	unlink(filename);
	}
	
	start_wan_done(wan_ifname);
        /*add by peny*/
#ifdef CWMP_SUPPORT
	if(!strcmp(argv[6], "0"))
	  if(local_ip= getenv("IPLOCAL"))
	    inform_start_dhcpc(wan_ifname, local_ip, which_conn); /* da_f@2005.3.4 modify */
#endif
	/*end by peny*/
	dprintf("done\n");
	return 0;

}

/*
 * Called when link goes down
 */
int
ipdown_main(int argc, char **argv)
{
	char *filename, *filename_new;
	char *wan_ifname = argv[1];
	char encap[15];
	
	int select_tmp = atoi(nvram_safe_get("active_connection_selection"));
	int which;
	
	which_conn = atoi(&wan_ifname[3]);  //set the global flag!!!!important
	which = which_conn;
	
	if(select_tmp != 32)
	{
		int data = 0;
		data |= which;
		data |= (1<<PPP_MASK);
		if(data == select_tmp)
			nvram_set("active_connection_selection", "32");
	}
	
	//junzhao 2004.7.28 for wan uptime record
	if(which_conn < MAX_CHAN)
	{
		char buf[16];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "wanuptime%d", which_conn);
		nvram_set(buf, "0");
	}
	
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	if(which_conn > MAX_CHAN-1)
		which_conn -= MAX_CHAN;
	memset(encap, 0, sizeof(encap));
	check_cur_encap(which_conn, encap);
#endif
	
	if(check_action() != ACT_IDLE)
                return -1;
	
	if(!strcmp(argv[6], "0"))
	{
		filename = malloc(strlen("/tmp/ppp/link") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link"+3));
		sprintf(filename, "/tmp/ppp/link%d", which_conn);
	
		filename_new = malloc(strlen("/tmp/ppp/link.down") + 3);
		if(filename_new == NULL)
			return -1;
		memset(filename_new, 0, strlen("/tmp/ppp/link.down"+3));
		sprintf(filename_new, "/tmp/ppp/link%d.down", which_conn);

		rename(filename, filename_new);
	
		free(filename);
		filename = NULL;
		free(filename_new);
		filename_new = NULL;
	
		unlink("/tmp/ppp/resolv.conf");
		unlink("/tmp/ppp/acname");
		unlink("/tmp/ppp/srvname");
			
		ram_modify_status(WAN_IPADDR, "0.0.0.0");
		ram_modify_status(WAN_NETMASK, "0.0.0.0");
		ram_modify_status(WAN_GATEWAY, "0.0.0.0");
		ram_modify_status(WAN_GET_DNS, "0.0.0.0:0.0.0.0");
		
		ram_modify_status(WAN_PPPOE_ACNAME, "None" );
		ram_modify_status(WAN_PPPOE_SRVNAME, "None" );
	    	
		//junzhao 2004.9.14
    		stop_upnp(1);
		      
		/*add by peny*/
	#ifdef CWMP_SUPPORT
	    	stop_inform_dhcpc(which_conn);
	#endif
		/*end by peny*/

		if(which_conn == atoi(nvram_safe_get("wan_enable_last")))
		{
			stop_ddns();    // Aviod to trigger DOD
       	 		stop_ntp();
			stop_email_alert();
			//stop_upnp(1);
		#ifdef PARENTAL_CONTROL_SUPPORT
 			stop_parental_control();	
		#endif
			
			//junzhao 2004.7.27
			if(!check_ppp_idle(which_conn))
			{
				if(add_next_gw())
				{
					start_ddns();
					start_ntp();
					start_email_alert();
					stop_upnp(2);
				#ifdef PARENTAL_CONTROL_SUPPORT
					start_parental_control();
				#endif
				}
			}
			/*
			else
			{
				char filename[strlen("/tmp/ppp/link.idle") + 3];
				memset(filename, 0, sizeof(filename));
				sprintf(filename, "/tmp/ppp/link%d.idle", which_conn);
				unlink(filename);
			}
			*/
		}	
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else
	{
		filename = malloc(strlen("/tmp/ppp/link") + 3);
		if(filename == NULL)
			return -1;
		memset(filename, 0, strlen("/tmp/ppp/link"+3));
		sprintf(filename, "/tmp/ppp/link%d", MAX_CHAN + which_conn);
	
		filename_new = malloc(strlen("/tmp/ppp/link.down") + 3);
		if(filename_new == NULL)
			return -1;
		memset(filename_new, 0, strlen("/tmp/ppp/link.down"+3));
		sprintf(filename_new, "/tmp/ppp/link%d.down", MAX_CHAN + which_conn);

		rename(filename, filename_new);
	
		free(filename);
		filename = NULL;
		free(filename_new);
		filename_new = NULL;
		
		unlink("/tmp/ppp/resolv.conf");
		unlink("/tmp/ppp/acname");
		unlink("/tmp/ppp/srvname");
		
		if(!strcmp(encap, "pppoe"))
	  	{
		#if defined(MPPPOE_SUPPORT)	
	  		char readbuf[256], *tmpstr = readbuf;	  
			if(file_to_buf(WAN_MPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
			{
				char dnsbuf[32], *dnsptr = NULL;
				char pdns[16], sdns[16];
				int which = which_conn, i;
				memset(dnsbuf, 0, sizeof(dnsbuf));	
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				//junzhao 2004.8.4
				for(i=0; i<MAX_CHAN; i++)
				{
					dnsptr = strsep(&tmpstr, " ");
					if(i == which)
						break;
				}
				sscanf(dnsptr, "%16[^:]:%16[^\0]", pdns, sdns);
				
				/*
				foreach(dnsbuf, readbuf, next)
				{
					if(which-- == 0)
						break;
				}	
				sscanf(dnsbuf, "%16[^:]:%16[^\0]", pdns, sdns);
				*/
				if(strcmp(pdns, "0.0.0.0"))
					eval("/usr/bin/route", "del", pdns);
				if(strcmp(sdns, "0.0.0.0"))
					eval("/usr/bin/route", "del", sdns);
			}
			ram_modify_status(WAN_MPPPOE_IPADDR, "0.0.0.0");
			ram_modify_status(WAN_MPPPOE_NETMASK, "0.0.0.0");
			ram_modify_status(WAN_MPPPOE_GATEWAY, "0.0.0.0");
			ram_modify_status(WAN_MPPPOE_GET_DNS, "0.0.0.0:0.0.0.0");
		
			ram_modify_status(WAN_MPPPOE_ACNAME, "None" );
			ram_modify_status(WAN_MPPPOE_SRVNAME, "None" );
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
	  	{
		#if defined(IPPPOE_SUPPORT)
	  		char readbuf[256], *tmpstr = readbuf;	  
			if(file_to_buf(WAN_IPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
			{
				char dnsbuf[32], *dnsptr = NULL;
				char pdns[16], sdns[16];
				int which = which_conn, i;
				memset(dnsbuf, 0, sizeof(dnsbuf));	
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				
				//junzhao 2004.8.4
				for(i=0; i<MAX_CHAN; i++)
				{
					dnsptr = strsep(&tmpstr, " ");
					if(i == which)
						break;
				}
				sscanf(dnsptr, "%16[^:]:%16[^\0]", pdns, sdns);
				
				/*
				foreach(dnsbuf, readbuf, next)
				{
					if(which-- == 0)
						break;
				}	
				sscanf(dnsbuf, "%16[^:]:%16[^\0]", pdns, sdns);
				*/
				if(strcmp(pdns, "0.0.0.0"))
					eval("/usr/bin/route", "del", pdns);
				if(strcmp(sdns, "0.0.0.0"))
					eval("/usr/bin/route", "del", sdns);
			}
			ram_modify_status(WAN_IPPPOE_IPADDR, "0.0.0.0");
			ram_modify_status(WAN_IPPPOE_NETMASK, "0.0.0.0");
			ram_modify_status(WAN_IPPPOE_GATEWAY, "0.0.0.0");
			ram_modify_status(WAN_IPPPOE_GET_DNS, "0.0.0.0:0.0.0.0");
		
			ram_modify_status(WAN_IPPPOE_ACNAME, "None" );
			ram_modify_status(WAN_IPPPOE_SRVNAME, "None" );
		#endif
		}
	}
#endif	

	stop_firewall();
	start_firewall();
	
	dns_to_resolv(which_conn);
	/* Restart DHCP server */
	stop_dhcpd();
	start_dhcpd();

	/* Restart DNS proxy */
	stop_dns();
	start_dns();
	
	//junzhao 2004.7.23 
	/* Restart RIP */
	stop_zebra();
	start_zebra();

	/* zhangbin 2005.5.11 */
#if 0
#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
	stop_gre();
	enable_gre();
#endif
#else
#if defined(CLOUD_SUPPORT)
	disable_cloud();
	enable_cloud();
#endif


#endif

	//junzhao 2004.7.29 restart qos
	//stop_qos();
	//start_qos();
#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
	down_qos(wan_ifname, atoi(&wan_ifname[3]));
#endif

	//junzhao 2004.7.29 restart ipsec
	//stop_ipsec();
	//start_ipsec();
	//down_ipsec(wan_ifname, which_conn); //zhangbin 2004.08.12
	
	{
	char filename[30];
	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/authfail%d", atoi(&wan_ifname[3])); 
	unlink(filename);
	}

	return 0;
}
