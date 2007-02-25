/*
 * udhcpc scripts
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: udhcpc.c,v 1.1.1.3 2005/08/25 06:27:57 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <code_pattern.h>
#include <cy_conf.h>
#include <utils.h>
#include "pvc.h"
#include "ledapp.h"

extern int which_conn;
#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

static int
expires(unsigned int in)
{
	struct sysinfo info;
	FILE *fp;

        sysinfo(&info);

	/* Save uptime ranther than system time, because the system time may change */
	if (!(fp = fopen("/tmp/udhcpc.expires", "w"))) {
		perror("/tmp/udhcpd.expires");
		return errno;
	}
	fprintf(fp, "%d", (unsigned int) info.uptime + in);
	fclose(fp);
	return 0;
}	
/* 
 * deconfig: This argument is used when udhcpc starts, and when a
 * leases is lost. The script should put the interface in an up, but
 * deconfigured state.
*/
static int
deconfig(void)
{
	char *wan_ifname = safe_getenv("interface");
//junzhao	
	if(wan_ifname != NULL && strcmp(wan_ifname, ""))
	{
		//junzhao 2004.7.15
		int select_tmp = atoi(nvram_safe_get("active_connection_selection"));
		which_conn = atoi(&wan_ifname[3]);  //set the global flag!!!!important
		//junzhao 2004.7.28 for wan uptime record
		if(which_conn < MAX_CHAN)
		{
			char buf[16];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "wanuptime%d", which_conn);
			nvram_set(buf, "0");
		}

		if((select_tmp) != 32)
		{
			int data = 0;
			data |= which_conn;
			if(data == select_tmp)
				nvram_set("active_connection_selection", "32");
		}

		ifconfig(wan_ifname, IFUP,
			 "0.0.0.0", NULL);
		/*
		nvram_modify_status("wan_ipaddr","0.0.0.0");
		nvram_modify_status("wan_netmask","0.0.0.0");
		nvram_modify_status("wan_gateway","0.0.0.0");
		nvram_modify_status("wan_get_dns","0.0.0.0:0.0.0.0");
		*/
		//if(!check_udhcpc_proc(which_conn))
		//	ram_modify_status(WAN_IFNAME,"Down");
		ram_modify_status(WAN_IPADDR,"0.0.0.0");
		ram_modify_status(WAN_NETMASK,"0.0.0.0");
		ram_modify_status(WAN_GATEWAY,"0.0.0.0");
		ram_modify_status(WAN_GET_DNS,"0.0.0.0:0.0.0.0");
	}
    
    {
	char *filename;
	FILE *fp;
	filename = malloc(strlen("/var/run/udhcpc.up") + 3);
	if(filename == NULL)
		return -1;
	memset(filename, 0, strlen("/var/run/udhcpc.up"+3));
	sprintf(filename, "/var/run/udhcpc%d.up", which_conn);
	unlink(filename);
	free(filename);
	
	//junzhao 2004.10.28
	filename = malloc(strlen("/var/run/udhcpc.down") + 3);
	if(filename == NULL)
		return -1;
	memset(filename, 0, strlen("/var/run/udhcpc.down"+3));
	sprintf(filename, "/var/run/udhcpc%d.down", which_conn);
	if ((fp = fopen(filename, "w"))) 
		fclose(fp);
	free(filename);
	filename = NULL;
   }

	//junzhao 2004.9.14
    	stop_upnp(1);

        if(which_conn == atoi(nvram_safe_get("wan_enable_last")))
	{
		stop_ddns();    // Aviod to trigger DOD
   	     	stop_ntp();
		stop_email_alert();
		//junzhao 2004.3.18;
		//stop_upnp(1);
	#ifdef PARENTAL_CONTROL_SUPPORT
 		stop_parental_control();	
	#endif
		
		//junzhao 2004.7.27
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
	
	//junzhao 2004.7.8	
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
#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
#if 0
	stop_gre();
	enable_gre();
#else
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
	//down_ipsec(wan_ifname, which_conn);
	
	expires(0);
	
//	nvram_set("wan_ipaddr","0.0.0.0");
//	nvram_set("wan_netmask","0.0.0.0");
//	nvram_set("wan_gateway","0.0.0.0");
//	nvram_set("wan_get_dns","");
	//nvram_set("wan_wins","0.0.0.0");      // Don't care for linksys spec
	nvram_set("wan_lease","0");
	
	unlink("/tmp/get_lease_time");
	unlink("/tmp/lease_time");
	
	//junzhao 2004.3.18;
	//start_upnp();

	//junzhao 2004.6.17
	if(which_conn == 0)
		nvram_set("wan_dhcp_ipaddr", "0.0.0.0");
	
	dprintf("done\n");
	return 0;
}

/*
 * bound: This argument is used when udhcpc moves from an unbound, to
 * a bound state. All of the paramaters are set in enviromental
 * variables, The script should configure the interface, and set any
 * other relavent parameters (default gateway, dns server, etc).
*/
static int
bound(void)
{
//	char *wan_ifname = safe_getenv("interface");
	char wan_ifname[8];
	char *value = NULL, *valuebuf = NULL;
	int which_tmp = atoi(nvram_safe_get("wan_active_connection"));
	
	memset(wan_ifname, 0, sizeof(wan_ifname));
	strcpy(wan_ifname, safe_getenv("interface"));
	printf("dhcp wan_ifname is %s\n", wan_ifname);
	
	which_conn = atoi(&wan_ifname[3]);
	
	//junzhao 2004.7.27	
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

	if ((value = getenv("ip")))
	{
		chomp(value);
		//nvram_set("wan_ipaddr", value);
		//nvram_modify_status("wan_ipaddr", value);
	}
	if ((valuebuf = getenv("subnet")))
	{
		chomp(valuebuf);
		//nvram_modify_status("wan_netmask", valuebuf);
	}
	ifconfig(wan_ifname, IFUP, value, valuebuf);

        if ((value = getenv("router")))
	{
		chomp(value);
		if(which_conn == which_tmp)
		{
			//nvram_modify_status("wan_gateway", value);
			/* Set default route to gateway if specified */
	       	 	route_add(wan_ifname, 0, "0.0.0.0", value, "0.0.0.0");
		}
		//junzhao 2004.8.5
		ram_modify_status(WAN_GATEWAY, value);	
	}
	if ((value = getenv("dns")))
	{	
		char buf[32];
		char *tmp = NULL;
		memset(buf, 0, sizeof(buf));
		chomp(value);
		
		tmp = strstr(value, " ");
		if(!tmp)
			sprintf(buf, "%s%s", value, ":0.0.0.0");
		else
		{
			*tmp = ':';	
			sprintf(buf, "%s", value);
		}
		
		if((tmp = strstr(buf, " ")) != NULL)
			*tmp = '\0';	
		
		//nvram_modify_status("wan_get_dns", buf);
		ram_modify_status(WAN_GET_DNS, buf);
	}
	/*if(NULL != (value = getenv("wins")))
	{
	}
	 if ((value = getenv("hostname")))
		sethostname(value, strlen(value) + 1);
	*/

	/*
	if ((value = getenv("domain"))){
		chomp(value);
		nvram_modify_status("wan_get_domain", value);	// HeartBeat need to use
	}
	if ((value = getenv("lease"))) {
		chomp(value);
		//nvram_modify_status("wan_lease", value);	// HeartBeat need to use
		expires(atoi(value));
	}
	*/

	//junzhao for led ctrl
	/*
	if(check_adsl_status())
	{
		diag_led_ctl(PPP_CON);
		start_checkrate("1");
	}
	else
	{
		diag_led_ctl(PPP_OFF);
		stop_checkrate();
	}
	*/

   {
	char *filename;
	FILE *fp;

	//junzhao 2004.10.28
	filename = malloc(strlen("/var/run/udhcpc.down") + 3);
	if(filename == NULL)
		return -1;
	memset(filename, 0, strlen("/var/run/udhcpc.down"+3));
	sprintf(filename, "/var/run/udhcpc%d.down", which_conn);
	unlink(filename);
	free(filename);

	filename = malloc(strlen("/var/run/udhcpc.up") + 3);
	if(filename == NULL)
		return -1;
	memset(filename, 0, strlen("/var/run/udhcpc.up"+3));
	sprintf(filename, "/var/run/udhcpc%d.up", which_conn);
	if (!(fp = fopen(filename, "a"))) 
	{
		perror(filename);
		return errno;
	}
	fclose(fp);
	free(filename);
	filename = NULL;
   }	
		
	start_wan_done(wan_ifname);
	dprintf("done\n");
	return 0;
}

/*
 * renew: This argument is used when a DHCP lease is renewed. All of
 * the paramaters are set in enviromental variables. This argument is
 * used when the interface is already configured, so the IP address,
 * will not change, however, the other DHCP paramaters, such as the
 * default gateway, subnet mask, and dns server may change.
 */
static int
renew(void)
{
	if(which_conn == atoi(nvram_safe_get("wan_enable_last")))
		stop_firewall();
	bound();

	dprintf("done\n");
	return 0;
}

int
udhcpc_main(int argc, char **argv)
{
	if(check_action() != ACT_IDLE)
                return -1;
	
	if (!argv[1])
		return EINVAL;
	else if (strstr(argv[1], "deconfig"))
		return deconfig();
	else if (strstr(argv[1], "bound"))
		return bound();
	else if (strstr(argv[1], "renew"))
		return renew();
	else
		return EINVAL;
}
