
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
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <shutils.h>
#include <bcmnvram.h>
#include <rc.h>
#include <code_pattern.h>
#include <cy_conf.h>



#ifdef DEBUG_HEARTBEAT
#define MY_LOG syslog
#else
#define MY_LOG(level, fmt, args...)
#endif

int
start_heartbeat(int status)
{
	FILE *fp;
	int ret;
	char authserver[20];
	char authdomain[80];
	char buf[254];

	if(nvram_invmatch("wan_proto", "heartbeat"))	return 0;
	
	openlog("heartbeat", LOG_PID | LOG_NDELAY, LOG_DAEMON);
	
	MY_LOG(LOG_DEBUG, "heartbeat: Start heartbeat daemon\n");
	MY_LOG(LOG_DEBUG, "heartbeat: hb_server_ip[%s] wan_get_domain[%s]\n", nvram_safe_get("hb_server_ip"), nvram_safe_get("wan_get_domain"));

	/* We must find out HB auth server from domain that get by dhcp if user don't input HB sever. */
	if(nvram_invmatch("hb_server_ip", "") && nvram_invmatch("hb_server_ip", "0.0.0.0")){
		snprintf(authserver, sizeof(authserver), "%s", nvram_safe_get("hb_server_ip"));
		snprintf(authdomain, sizeof(authdomain), "%s", "");
	}
	else if(nvram_match("wan_get_domain", "nsw.bigpond.net.au")){            // NSW
		snprintf(authserver, sizeof(authserver), "%s", "spr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "nsw-remote.bigpond.net.au");
	}
	else if(nvram_match("wan_get_domain", "vic.bigpond.net.au")){           // Victoria
		snprintf(authserver, sizeof(authserver), "%s", "mer3");
		snprintf(authdomain, sizeof(authdomain), "%s", "vic-remote.bigpond.net.au");
	}
	else if(nvram_match("wan_get_domain","qld.bigpond.net.au")){            // Queensland
		snprintf(authserver, sizeof(authserver), "%s", "bcr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "qld-remote.bigpond.net.au");
	}
	else if(nvram_match("wan_get_domain", "sa.bigpond.net.au")){            // South Austrialia
		snprintf(authserver, sizeof(authserver), "%s", "afr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "sa-remote.bigpond.net.au");
	}
	else if(nvram_match("wan_get_domain", "wa.bigpond.net.au")){            // Western Austrialia
		snprintf(authserver, sizeof(authserver), "%s", "pwr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "wa-remote.bigpond.net.au");
	}
	else{
		MY_LOG(LOG_ERR, "heartbeat: Cann't find HB server from domain! Use gateway.\n");
		snprintf(authserver, sizeof(authserver), "%s", nvram_safe_get("wan_gateway"));
		snprintf(authdomain, sizeof(authdomain), "%s", "");
		//return 1;
	}

	MY_LOG(LOG_DEBUG, "heartbeat: authserver[%s] authdomain[%s]\n", authserver, authdomain);
	
	snprintf(buf, sizeof(buf), "%s%c%s", authserver, !strcmp(authdomain,"") ? '\0' : '.', authdomain);
	nvram_set("hb_server_name", buf);
	
	MY_LOG(LOG_DEBUG, "heartbeat: Connect to HB server [%s]\n", buf);
	
	//syslog(LOG_DEBUG, "heartbeat: authserver[%s] authdomain[%s]\n", authserver, authdomain);
	if (!(fp = fopen("/tmp/bpalogin.conf", "w"))) {
		MY_LOG(LOG_ERR, "heartbeat: Cann't write %s\n", "/tmp/bpalogin.conf");
		perror("/tmp/bpalogin.conf");
		return errno;
	}
	fprintf(fp, "username %s\n", nvram_safe_get("ppp_username"));
	fprintf(fp, "password %s\n", nvram_safe_get("ppp_passwd"));
	fprintf(fp, "authserver %s\n", authserver);
	fprintf(fp, "%cauthdomain %s\n", strcmp(authdomain,"") ? '\0' : '#', authdomain);
	fprintf(fp, "localport 5050\n");
	fprintf(fp, "logging stdout\n");
	fprintf(fp, "debuglevel 2\n");
	fprintf(fp, "minheartbeatinterval 60\n");
	fprintf(fp, "maxheartbeatinterval 420\n");
	fprintf(fp, "connectedprog hb_connect\n"); 
	fprintf(fp, "disconnectedprog hb_disconnect\n");
	
	fclose(fp);
	
	mkdir("/tmp/ppp", 0777);
	if((fp = fopen("/tmp/hb_connect_success", "r"))){
		ret = eval("bpalogin", "-c", "/tmp/bpalogin.conf", "-t");
		fclose(fp);	
	}
	else
		ret = eval("bpalogin", "-c", "/tmp/bpalogin.conf");
	
	if (nvram_invmatch("ppp_demand", "1")) {
		if(status != REDIAL)
			start_redial("2", "ppp0");	
	}

	return ret;
}

int
stop_heartbeat(void)
{
	int ret;
	
	unlink("/tmp/ppp/link");
	ret = eval("killall", "bpalogin");
	ret += eval("killall", "-9", "bpalogin");
	
	openlog("heartbeat", LOG_PID | LOG_NDELAY, LOG_DAEMON);
	MY_LOG(LOG_DEBUG, "heartbeat: Stop heartbeat daemon\n");
	closelog();

	dprintf("done\n");
	
	return ret;
}

/*
 *  Called when link comes up
 *  argv[1] : listenport
 *  argv[2] : pid
 */
int
hb_connect_main(int argc, char **argv)
{
	FILE *fp;
	char buf[254];
	
	openlog("heartbeat" ,LOG_PID, LOG_DAEMON);
	
	MY_LOG(LOG_INFO, "The user have logged in successfully : %s\n", argv[1]);

	mkdir("/tmp/ppp", 0777);

	if (!(fp = fopen("/tmp/ppp/link", "a"))) {
                perror("/tmp/ppp/link");
                return errno;
        }
	fprintf(fp, "%s", argv[2]);
        fclose(fp);
	
	start_wan_done(nvram_safe_get("wan_ifname"));
	
	//snprintf(buf, sizeof(buf), "iptables -I INPUT -s %s -d %s -i %s -p udp --dport %d -j %s"
	//	   , nvram_invmatch("hb_server_ip", "") && nvram_invmatch("hb_server_ip", "0.0.0.0") ? nvram_safe_get("hb_server_ip") : nvram_safe_get("hb_server_name"), 
	//	   nvram_safe_get("wan_ipaddr"), nvram_safe_get("wan_ifname"), 5050, "ACCEPT");
	snprintf(buf, sizeof(buf), "iptables -I INPUT -d %s -i %s -p udp --dport %d -j %s", 
		   nvram_safe_get("wan_ipaddr"), 
		   nvram_safe_get("wan_ifname"),
		   5050, 
		   "ACCEPT");

	MY_LOG(LOG_DEBUG, "heartbeat: Add firewall [%s]\n", buf);

	system(buf);

	return TRUE;	
}

/*
 *  * Called when link goes down
 *   */
int
hb_disconnect_main(int argc, char **argv)
{
	openlog("heartbeat" ,LOG_PID, LOG_DAEMON);
	
	MY_LOG(LOG_INFO, "The user have logged out: %s\n", argv[1]);

	stop_wan();
		
	return unlink("/tmp/ppp/link");
}
