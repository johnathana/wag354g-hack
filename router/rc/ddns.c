
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>
#include <sys/wait.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <utils.h>
#include <rc.h>

#define WAN_IPADDR "/tmp/status/wan_ipaddr"

char service[10];
char disable_ip[20];
char _username[] = "ddns_username_X";
char _passwd[] = "ddns_passwd_X";
char _hostname[] = "ddns_hostname_X";

int
init_ddns(void)
{
	int flag = 0;

	if(nvram_match("ddns_enable","0")) {			// disable from ui or default
		if(nvram_match("ddns_enable_buf","1")){		// before disable is dyndns, so we want to disable dyndns
			strcpy(service,"dyndns");
			strcpy(disable_ip,"192.168.1.1");	// send this address to disable dyndns
			flag = 1;
		}
		else if(nvram_match("ddns_enable_buf","2")){	// before disable is tzo, so we want to disable tz
			strcpy(service,"tzo");
			strcpy(disable_ip,"0.0.0.0");
			flag = 2;
		}
		else return -1;					// default 
	}
	else if(nvram_match("ddns_enable","1")){
		strcpy(service,"dyndns");
		flag = 1;
	}
	else if(nvram_match("ddns_enable","2")){
		strcpy(service,"tzo");
		flag = 2;
	}

	if(flag == 1){
		snprintf(_username, sizeof(_username),"%s","ddns_username");
		snprintf(_passwd, sizeof(_passwd),"%s","ddns_passwd");
		snprintf(_hostname, sizeof(_hostname),"%s","ddns_hostname");
	}
	else if(flag == 2){
		snprintf(_username, sizeof(_username),"%s","ddns_username_2");
		snprintf(_passwd, sizeof(_passwd),"%s","ddns_passwd_2");
		snprintf(_hostname, sizeof(_hostname),"%s","ddns_hostname_2");
	}

	return 0;
}

int
start_ddns(void)
{
	int ret;
	FILE *fp;
	pid_t pid;
	char string[80]="";
//junzhao
	char ddnsusername[32];
	char ddnspasswd[32];
	char ddnshostname[32];
	char ddnsenable[3];
	
	/* Get correct username, password and hostname */
	if(init_ddns() < 0)
		return -1;

	memset(ddnsusername, 0, sizeof(ddnsusername));
	memset(ddnspasswd, 0, sizeof(ddnspasswd));
	memset(ddnshostname, 0, sizeof(ddnshostname));
	memset(ddnsenable, 0, sizeof(ddnsenable));
	
	strcpy(ddnsusername, nvram_safe_get(_username));
	strcpy(ddnspasswd, nvram_safe_get(_passwd));
	strcpy(ddnshostname, nvram_safe_get(_hostname));
	strcpy(ddnsenable, nvram_safe_get("ddns_enable"));
	
	/* We don't want to update, if user don't input below field */
	if(nvram_match(_username,"")||
	   nvram_match(_passwd,"")||
	   nvram_match(_hostname,""))
		return -1;
	
	/* We want to re-update if user change some value from UI */
	if(strcmp(nvram_safe_get("ddns_enable_buf"), ddnsenable) ||	// ddns mode change
            strcmp(nvram_safe_get("ddns_username_buf"), ddnsusername) ||	// ddns username chane
            strcmp(nvram_safe_get("ddns_passwd_buf"), ddnspasswd) ||	// ddns password change
            strcmp(nvram_safe_get("ddns_hostname_buf"), ddnshostname))
	{  // ddns hostname change
	    cprintf("Some value had been changed , need to update\n");
	    
	    if(nvram_match("action_service", "ddns") || !file_to_buf("/tmp/ddns_msg", string, sizeof(string))){
		    cprintf("Upgrade from UI or first time\n");
		    //nvram_unset("ddns_cache");	// The will let program to re-update
		    //modify by junzhao
		    nvram_set("ddns_cache","");	// The will let program to re-update
		    unlink("/tmp/ddns_msg");	// We want to get new message
	    }
	}

	/* Some message we want to stop to update */
	if(file_to_buf("/tmp/ddns_msg", string, sizeof(string))){
		cprintf("string=[%s]\n", string);
		if(strcmp(string, "") && 
		   !strstr(string, "dyn_noupdate") && 
		   !strstr(string, "_good") && 
		   !strstr(string, "all_")){
			cprintf("Last update have error message : %s, don't re-update\n", string);
			return -1;
		}
	}
	
	/* Generate ddns configuration file */
       	if ((fp = fopen("/tmp/ddns.conf", "w"))) {
     		fprintf(fp, "service-type=%s\n",service); 
     	  	fprintf(fp, "user=%s:%s\n",ddnsusername, ddnspasswd); 
       		fprintf(fp, "host=%s\n",nvram_safe_get(_hostname)); 

		if(nvram_match("ddns_enable","0")){
			fprintf(fp, "address=%s\n",disable_ip);	// send error ip address
		}
		else 
		{
			//int which = atoi(nvram_safe_get("wan_connection"));
			//int which = 0;
			//junzhao 2004.4.8
			int which = atoi(nvram_safe_get("wan_active_connection"));
			char word[20], *next, *ipaddr;
			char readbuf[160];
			
			if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
			{
				printf("read %s file fail\n", WAN_IPADDR);
				return -1;
			}
			
			//foreach(word, nvram_safe_get("wan_ipaddr"), next)
			foreach(word, readbuf, next)
			{									                if(which -- == 0)								{
					ipaddr = word;									break;
				}									}
	       		fprintf(fp, "address=%s\n", ipaddr); 					}
	       		//fprintf(fp, "address=%s\n",nvram_safe_get("wan_ipaddr")); 		
		fclose(fp);
	}
	else{
        	perror("/tmp/ddns.conf");
        	return -1;
	}

	/* Restore cache data to file */
	if(nvram_invmatch("ddns_enable", ""))	
		nvram2file("ddns_cache", "/tmp/ddns.cache");
	{//junzhao
	char *argv[] = {"ez-ipupdate",
	      //"-i", nvram_safe_get("wan_ifname"),
	      "-D",
	      //"-P", "3600",
	      "-e", "ddns_success",
	      "-c", "/tmp/ddns.conf",
	      "-b", "/tmp/ddns.cache", 
	      NULL };

	ret = _eval(argv, ">/dev/console", 0, &pid);
	}//endjunzhao
	dprintf("done\n");
	
	return ret;
}

int
stop_ddns(void)
{
        int ret;
	
        ret = eval("killall","-9","ez-ipupdate");

        dprintf("done\n");

        return ret;
}

int
ddns_success_main(int argc, char *argv[])
{
	char buf[80];
	
	init_ddns();

	snprintf(buf, sizeof(buf), "%ld,%s", time(NULL), argv[1]);
	
	nvram_set("ddns_cache", buf);
	nvram_set("ddns_status", "1");
	nvram_set("ddns_enable_buf", nvram_safe_get("ddns_enable"));
	nvram_set("ddns_username_buf", nvram_safe_get(_username));
	nvram_set("ddns_passwd_buf", nvram_safe_get(_passwd));
	nvram_set("ddns_hostname_buf", nvram_safe_get(_hostname));
	nvram_set("ddns_change", "");

	nvram_commit();

	dprintf("done\n");

	return 0;
}
