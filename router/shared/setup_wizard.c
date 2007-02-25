
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
#include <time.h>
#include <sys/klog.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <broadcom.h>
#ifdef SETUP_WIZARD_SUPPORT

extern int sys_commit(void);
extern void show_error(webs_t wp, char *message);

static int
valid_value(webs_t wp)
{

	char *setupwizard, *hostname="", *domainname="", *routername;
	char *mac=NULL, *mac0, *mac1, *mac2, *mac3, *mac4, *mac5;
	char dns[100]="", *dns0=NULL, *dns1=NULL, *dns2=NULL, *dns0_1, *dns0_2, *dns0_3, *dns0_4, *dns1_1, *dns1_2, *dns1_3, *dns1_4, *dns2_1, *dns2_2, *dns2_3, *dns2_4;
	char *wan_ip=NULL, *wan_ip0, *wan_ip1, *wan_ip2, *wan_ip3;
	char *wan_mask=NULL, *wan_mask0, *wan_mask1, *wan_mask2, *wan_mask3;
	char *wan_gw=NULL, *wan_gw0, *wan_gw1, *wan_gw2, *wan_gw3;
#ifdef AOL_SUPPORT
	char *aol_block_traffic;
#endif
	
	char wan_proto[20], *wan_status, *ppp_username=NULL, *ppp_passwd=NULL, *http_passwd, *http_passwdcom;
	char wl_wep_buf[100], *wl_status, *wl_key, *wl_wep_bit, *wl_passphrase, *wl_key1, *wl_ssid, *wl_channel, *wl_bcast, *wl_encryption, *wl_wpa_psk, *wl_mode;
	char nv_config[511], p_tmp[511], *next, p_config[126], vcc_config[511], *wan_encapmode, *wan_multiplex, *wan_vpi, *wan_vci;//Dom 2005.03.30 add for DSL
	char *ppp_servicename, *KaliveStatus, *KaliveTime, *fixedip, buf[20], mtu_config[511], *mtu_enable, *mtu_size;//Dom 2006.02.10 ass dor MTU
	int p_ii = 0;

	struct variable setup_wizard_variables[] = {
		{ longname: "MAC address", NULL },				// 0
		{ longname: "DNS 0 IP address", NULL },				// 1
		{ longname: "DNS 1 IP address", NULL },				// 2
		{ longname: "DNS 2 IP address", NULL },				// 3
		{ longname: "WAN IP address", NULL },				// 4
		{ longname: "netmask IP address", NULL },			// 5
		{ longname: "gateway IP address", NULL },			// 6
		{ longname: "802.11g Channel", argv: ARGV("1", "14") },		// 7
		{ longname: "Network Type", argv: ARGV("0", "1") },		// 8 (1:broadcast)
		{ longname: "wan status", argv: ARGV("0","4") },		// 9 (0:1483 Bridged 1:1483 Routed 2:PPPoE 3:PPPoA 4:Bridge only)
		{ longname: "Auth Mode", argv: ARGV("0","1","2") },		// 10 (0:off 1:restricted 2:psk)	// Add psk, by honor 2003-07-24
		{ longname: "Network Key Index", argv: ARGV("1","4") },		// 11
		{ longname: "Encryption bit", argv: ARGV("64","128") },		// 12
		{ longname: "aol status", argv: ARGV("0","1","2") },		// 13
		{ longname: "Wireless Encryption", argv: ARGV("off","wep","tkip","aes") },	// 14	// Add for New Setup Wizard, by honor 2003-07-24
		{ longname: "Wireless Mode", argv: ARGV("-1","0","1","2") },	// 15	// Add for Parental, by honor 2003-10-13
	}, *which;

	which = &setup_wizard_variables[0];

	setupwizard = websGetVar(wp, "SetupWizard", NULL);
	if(!setupwizard || strcmp(setupwizard,"1")){
		if(!websGetVar(wp, "sysAction_FactoryDefault", NULL) && !websGetVar(wp, "sysAction_Reboot", NULL))
			show_error(wp, "Invalid setup wizard format!");
		goto fail;
	}

	wan_status = websGetVar(wp, "SWwanStatus", NULL);
	hostname = websGetVar(wp, "SWhostName", NULL);
	domainname = websGetVar(wp, "SWDomainName", NULL);
	routername = websGetVar(wp, "SWRouterName", NULL);
	fixedip = websGetVar(wp, "SWFixedIP", NULL);

	mac0 = websGetVar(wp, "SWwanMac0", NULL);
	mac1 = websGetVar(wp, "SWwanMac1", NULL);
	mac2 = websGetVar(wp, "SWwanMac2", NULL);
	mac3 = websGetVar(wp, "SWwanMac3", NULL);
	mac4 = websGetVar(wp, "SWwanMac4", NULL);
	mac5 = websGetVar(wp, "SWwanMac5", NULL);
	dns0_1 = websGetVar(wp, "SWdnsA1", NULL);
	dns0_2 = websGetVar(wp, "SWdnsA2", NULL);
	dns0_3 = websGetVar(wp, "SWdnsA3", NULL);
	dns0_4 = websGetVar(wp, "SWdnsA4", NULL);
	dns1_1 = websGetVar(wp, "SWdnsB1", NULL);
	dns1_2 = websGetVar(wp, "SWdnsB2", NULL);
	dns1_3 = websGetVar(wp, "SWdnsB3", NULL);
	dns1_4 = websGetVar(wp, "SWdnsB4", NULL);
	dns2_1 = websGetVar(wp, "SWdnsC1", NULL);
	dns2_2 = websGetVar(wp, "SWdnsC2", NULL);
	dns2_3 = websGetVar(wp, "SWdnsC3", NULL);
	dns2_4 = websGetVar(wp, "SWdnsC4", NULL);

#ifdef AOL_SUPPORT
	aol_block_traffic = websGetVar(wp, "SWblock_traffic", NULL);

	if(!aol_block_traffic || !valid_range(wp, aol_block_traffic, &which[13])){
		show_error(wp, "Cannot find aol_block_traffic or invalid value!");
		goto fail;
	}
#endif

	cprintf("setupwizard=[%s] wan_status=[%s] host=[%s] domain=[%s] routername=[%s]\n",
		setupwizard, wan_status, hostname, domainname, routername);
	cprintf("mac=[%s][%s][%s][%s][%s][%s] dns=[%s][%s][%s][%s] [%s][%s][%s][%s] [%s][%s][%s][%s]\n",
		mac0,mac1,mac2,mac3,mac4,mac5,dns0_1,dns0_2,dns0_3,dns0_4,dns1_1,dns1_2,dns1_3,dns1_4,dns2_1,dns2_2,dns2_3,dns2_4);

	http_passwd = websGetVar(wp, "SWsysPasswd", NULL);
	http_passwdcom = websGetVar(wp, "SWsysPasswdconfirm", NULL);

	if(!wan_status || !valid_range(wp, wan_status, &which[9])){
		show_error(wp, "Cannot find wan_status or invalid value!");
		goto fail;
	}

	if(((atoi(wan_status) == 0 ) && (atoi(fixedip) == 0 )) || (atoi(wan_status) == 4 )) {	// DHCP mode	
		cprintf("Wan Status: DHCP mode\n");
		strcpy(wan_proto,"dhcp");
	}
	else if(((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 )) || (atoi(wan_status) == 1 )){	// static mode
		cprintf("Wan Status: static mode\n");
		strcpy(wan_proto,"static");
		wan_ip0 = websGetVar(wp, "SWaliasIP1", NULL);
		wan_ip1 = websGetVar(wp, "SWaliasIP2", NULL);
		wan_ip2 = websGetVar(wp, "SWaliasIP3", NULL);
		wan_ip3 = websGetVar(wp, "SWaliasIP4", NULL);
		wan_mask0 = websGetVar(wp, "SWaliasMaskIP0", NULL);
		wan_mask1 = websGetVar(wp, "SWaliasMaskIP1", NULL);
		wan_mask2 = websGetVar(wp, "SWaliasMaskIP2", NULL);
		wan_mask3 = websGetVar(wp, "SWaliasMaskIP3", NULL);
		wan_gw0 = websGetVar(wp, "SWrouterIP1", NULL);
		wan_gw1 = websGetVar(wp, "SWrouterIP2", NULL);
		wan_gw2 = websGetVar(wp, "SWrouterIP3", NULL);
		wan_gw3 = websGetVar(wp, "SWrouterIP4", NULL);
		if(wan_ip0 && wan_ip1 && wan_ip2 && wan_ip3){
			wan_ip = malloc(20);
			snprintf(buf,sizeof(buf),"%s.%s.%s.%s",wan_ip0, wan_ip1, wan_ip2, wan_ip3);
			strcpy(wan_ip,buf);
			cprintf("wan_ip=[%s]\n",wan_ip);
			if(!valid_ipaddr(wp, wan_ip, &which[4])){
				goto fail;
			}
		}
		if(wan_mask0 && wan_mask1 && wan_mask2 && wan_mask3){
			wan_mask = malloc(20);
			snprintf(buf,sizeof(buf),"%s.%s.%s.%s",wan_mask0, wan_mask1, wan_mask2, wan_mask3);
			strcpy(wan_mask,buf);
			cprintf("wan_mask=[%s]\n",wan_mask);
			if(!valid_ipaddr(wp, wan_mask, &which[5])){
				goto fail;
			}
		}
		if(wan_gw0 && wan_gw1 && wan_gw2 && wan_gw3){
			wan_gw = malloc(20);
			snprintf(buf,sizeof(buf),"%s.%s.%s.%s",wan_gw0, wan_gw1, wan_gw2, wan_gw3);
			strcpy(wan_gw,buf);
			cprintf("wan_gw=[%s]\n",wan_gw);
			if(!valid_ipaddr(wp, wan_gw, &which[6])){
				goto fail;
			}
		}
	}
	else if((atoi(wan_status) == 2 ) || (atoi(wan_status) == 3 )){	// pppoe && pppoa mode
		if(atoi(wan_status) == 2 ){
			cprintf("Wan Status: PPPoE mode\n");
			strcpy(nv_config, nvram_safe_get("pppoe_config"));
			ppp_username = websGetVar(wp, "SWpppoeUName", NULL);
			ppp_passwd = websGetVar(wp, "SWpppoePWD", NULL);
			ppp_servicename = websGetVar(wp, "SWppp_servicename", NULL);
	                KaliveStatus = websGetVar(wp, "SWKaliveStatus", NULL);
        	        KaliveTime = websGetVar(wp, "SWKaliveTime", NULL);
                	y_printf("pp %s %s %s %s %s\n", ppp_passwd, ppp_username, KaliveStatus, KaliveTime, ppp_servicename );
			cprintf("ppp_username[%s] ppp_passwd[%s]  KaliveStatus[%s] KaliveTime[%s] ppp_servicename[%s]\n",
				 ppp_username,    ppp_passwd,     KaliveStatus,    KaliveTime,    ppp_servicename );
		}
                else if(atoi(wan_status) == 3 ){ // Dom 2005.03.28
                        cprintf("Wan Status: PPPoA mode\n");
                        strcpy(nv_config, nvram_safe_get("pppoa_config"));
                        ppp_username = websGetVar(wp, "SWpppoaUName", NULL);
                        ppp_passwd = websGetVar(wp, "SWpppoaPWD", NULL);
			KaliveStatus = websGetVar(wp, "SWKaliveStatus", NULL);
                        KaliveTime = websGetVar(wp, "SWKaliveTime", NULL);
                        y_printf("pp %s %s %s %s\n", ppp_passwd, ppp_username, KaliveStatus, KaliveTime );
                        cprintf("ppp_username[%s] ppp_passwd[%s]  KaliveStatus[%s] KaliveTime[%s]\n",
				 ppp_username,    ppp_passwd,     KaliveStatus,    KaliveTime );

                }
	}
	cprintf("Vcc Statue Get\n");
	strcpy(vcc_config, nvram_safe_get("vcc_config"));
	strcpy(mtu_config, nvram_safe_get("mtu_config"));
        if(atoi(wan_status) == 0 )
	   wan_encapmode="1483bridged";
        else if(atoi(wan_status) == 1 )
           wan_encapmode="routed";
        else if(atoi(wan_status) == 2 )
           wan_encapmode="pppoe";
        else if(atoi(wan_status) == 3 )
           wan_encapmode="pppoa";
        else if(atoi(wan_status) == 4 )
           wan_encapmode="bridgedonly";
//	wan_encapmode = websGetVar(wp, "SWwan_encapmode", NULL);
	wan_multiplex = websGetVar(wp, "SWwan_multiplex", NULL);
	wan_vpi = websGetVar(wp, "SWwan_vpi", NULL);
	wan_vci = websGetVar(wp, "SWwan_vci", NULL);
	mtu_enable = websGetVar(wp, "SWmtu_enable", NULL);
	mtu_size = websGetVar(wp, "SWmtu_size", NULL);
	y_printf("ppp %s %s %s %s %s %s\n", wan_vpi, wan_vci, wan_encapmode, wan_multiplex, mtu_enable, mtu_size );
	cprintf("wan_vpi[%s] wan_vci[%s] wan_encapmode[%s] wan_multiplex[%s] mtu_enable[%s] mtu_size[%s]\n",
		 wan_vpi,    wan_vci,    wan_encapmode,    wan_multiplex,    mtu_enable,    mtu_size );

	/*Set wireless variable*/
	wl_ssid = websGetVar(wp, "SWwirelessESSID", NULL);
	wl_mode = websGetVar(wp, "SWwirelessMode", NULL);
	wl_channel = websGetVar(wp, "SWwirelessChannel", NULL);
	wl_bcast = websGetVar(wp, "SWbroadcastSSID", "0");	// Setup Wizard 2.0 don't need

	wl_status = websGetVar(wp, "SWwirelessStatus", NULL);
	wl_key = websGetVar(wp, "SWwepKey", NULL);
	wl_wep_bit = websGetVar(wp, "SWwepEncryption", NULL);
	wl_passphrase = websGetVar(wp, "SWpassphrase", NULL);
	wl_key1 = websGetVar(wp, "SWwepKey1", NULL);
	wl_wpa_psk = websGetVar(wp, "SWwpaPSK", NULL);
	wl_encryption = websGetVar(wp, "SWwlEncryption", NULL);

	cprintf("http_passwd=[%s] http_passwdcom=[%s]\n",
		http_passwd, http_passwdcom);
	cprintf("wl_ssid=[%s] wl_channel=[%s] wl_bcast=[%s] wl_mode=[%s]\n",
		wl_ssid, wl_channel, wl_bcast, wl_mode);
	cprintf("wl_status=[%s] wl_encryption=[%s] wl_key=[%s] wl_wep_bit=[%s] wl_passphrase=[%s] wl_key1=[%s], wl_wpa_psk=[%s]\n",
		wl_status, wl_encryption, wl_key, wl_wep_bit, wl_passphrase, wl_key1, wl_wpa_psk);
	
	/* valid wan MAC */
	if(mac0 && mac1 && mac2 && mac3 && mac4 && mac5 && strcmp(mac0,"")){
		mac = malloc(20);
		snprintf(buf,sizeof(buf),"%s:%s:%s:%s:%s:%s",mac0,mac1,mac2,mac3,mac4,mac5);
		strcpy(mac,buf);
		cprintf("mac=[%s]\n",mac);
		if(!valid_hwaddr(wp, mac, &which[0])){
			cprintf("MAC format is error!\n");
			goto fail;
		}
	}
	
	/* valid DNS 1 */
	if(dns0_1 && dns0_2 && dns0_3 && dns0_4){
		dns0 = malloc(20);
		snprintf(buf,sizeof(buf),"%s.%s.%s.%s",dns0_1, dns0_2, dns0_3, dns0_4);
		strcpy(dns0,buf);
		cprintf("dns0=[%s]\n",dns0);
		if(!valid_ipaddr(wp, dns0, &which[1])){
			goto fail;
		}
	}
	/* valid DNS 2 */
	if(dns1_1 && dns1_2 && dns1_3 && dns1_4){
		dns1 = malloc(20);
		snprintf(buf,sizeof(buf),"%s.%s.%s.%s",dns1_1, dns1_2, dns1_3, dns1_4);
		strcpy(dns1,buf);
		cprintf("dns1=[%s]\n",dns1);
		if(!valid_ipaddr(wp, dns1, &which[2])){
			goto fail;
		}
	}
	/* valid DNS 3 */
	if(dns2_1 && dns2_2 && dns2_3 && dns2_4){
		dns2 = malloc(20);
		snprintf(buf,sizeof(buf),"%s.%s.%s.%s",dns2_1, dns2_2, dns2_3, dns2_4);
		strcpy(dns2,buf);
		cprintf("dns2=[%s]\n",dns2);
		if(!valid_ipaddr(wp, dns2, &which[3])){
			goto fail;
		}
	}

	if(dns0 && strcmp(dns0,"0.0.0.0"))
		snprintf(dns+strlen(dns), sizeof(dns),"%s ",dns0);
	if(dns1 && strcmp(dns1,"0.0.0.0"))
		snprintf(dns+strlen(dns), sizeof(dns),"%s ",dns1);
	if(dns2 && strcmp(dns2,"0.0.0.0"))
		snprintf(dns+strlen(dns), sizeof(dns),"%s",dns2);
	if(((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 )) || (atoi(wan_status) == 1 )){
		if ((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 ))
			strcpy(nv_config, nvram_safe_get("bridged_config"));
		else if (atoi(wan_status) == 1 )
			strcpy(nv_config, nvram_safe_get("routed_config"));
 		y_printf("pp %s %s %s %s %s\n", wan_ip, wan_mask, wan_gw, dns0, dns1 );
	}
	cprintf("Validing http password\n");
	if(http_passwd && http_passwdcom){
		if(strcmp(http_passwd, http_passwdcom)){
			show_error(wp, "Invalid passwd!");
			goto fail;
		}
	}

	/* valid wireless */
	cprintf("Validing Wireless Channel\n");
	if(wl_channel && !valid_range(wp, wl_channel, &which[7])){
		show_error(wp, "Cannot find wireless channel or invalid value!");
		goto fail;
	}
	cprintf("Validing Wireless SSID Broadcast\n");
	if(wl_bcast && !valid_range(wp, wl_bcast, &which[8])){
		show_error(wp, "Cannot find wireless broadcast or invalid value!");
		goto fail;
	}
	cprintf("Validing Wireless Encryption\n");
	if(wl_encryption && !valid_choice(wp, wl_encryption, &which[14])){
		show_error(wp, "Cannot find wireless encryption or invalid value!");
		goto fail;
	}
	cprintf("Validing Wireless Mode\n");
	if(wl_mode && !valid_choice(wp, wl_mode, &which[15])){ // Only for Parental Control
		show_error(wp, "Wireless Mode have invalid value!");
		goto fail;
	}
	
	if(wl_status && atoi(wl_status) == 1){	// wep mode
		cprintf("Wireless Auth Mode: WEP mode\n");
		if(!wl_status || !valid_range(wp, wl_status, &which[10])){
			show_error(wp, "Cannot find wireless status or invalid value!");
			goto fail;
		}
		if(!wl_key || !valid_range(wp, wl_key, &which[11])){
			show_error(wp, "Cannot find wireless key or invalid value!");
			goto fail;
		}
		if(!wl_wep_bit || !valid_range(wp, wl_wep_bit, &which[12])){
			show_error(wp, "Cannot find wireless wep bit or invalid value!");
			goto fail;
		}
	}
	if(wl_status && atoi(wl_status) == 2){	// psk mode
		cprintf("Wireless Auth Mode: PSK mode\n");
		if(!wl_wpa_psk)	
			cprintf("Warning: Cannot find wireless WPA PSK!");
		
	}
	else
		cprintf("Wireless Auth Mode: Disabled mode\n");

	cprintf("Valid OK, writing settings to nvram\n");
	
	/* basic setting */
	nvram_set("wan_proto",wan_proto);
	if(hostname)
		nvram_set("wan_hostname",hostname);
	if(domainname)
		nvram_set("wan_domain",domainname);
	if(routername)
		nvram_set("router_name",routername);
	if(dns0 && dns1 && dns2)
		nvram_set("wan_dns",dns);
	if(mac){
		nvram_set("def_hwaddr",mac);
		nvram_set("mac_clone_enable","1");
	}

//    	cprintf ("PPP_debug1:%s \n",p_config);
//	cprintf ("PPP_debug2:%s \n",nv_config);
	foreach(p_config, nv_config, next)
	{
		if(p_ii == 0)
		{
			if(((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 )) || (atoi(wan_status) == 1)){
				if((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 ))
					string_rep(p_config, 1, "0");
				string_rep(p_config, 2 - atoi(wan_status), wan_ip);
				string_rep(p_config, 3 - atoi(wan_status), wan_mask);
				string_rep(p_config, 4 - atoi(wan_status), wan_gw);
				string_rep(p_config, 5 - atoi(wan_status), dns0);
				string_rep(p_config, 6 - atoi(wan_status), dns1);
			}
			else if((atoi(wan_status) == 2) || (atoi(wan_status) == 3)){
				string_rep(p_config, 1, ppp_username);
				if(ppp_passwd && ppp_passwd[0])
					string_rep(p_config, 2, ppp_passwd);

				if (atoi(KaliveStatus) == 1)
                                        string_rep(p_config, 3, "0");
				else if (atoi(KaliveStatus) == 2)
					string_rep(p_config, 3, "1");

				strncpy(p_config, p_config, (strrchr(p_config, ':') - p_config + 1));
				if (atoi(wan_status) == 2){
					string_rep(p_config, 6 - atoi(KaliveStatus) , KaliveTime);
					strcpy(p_config + (strrchr(p_config, ':') - p_config + 1), ppp_servicename);
				}
				else if (atoi(wan_status) == 3 )
					strcpy(p_config + (strrchr(p_config, ':') - p_config + 1), KaliveTime);
			}
			strcpy(p_tmp, p_config);
		}
		else
			strcat(p_tmp, p_config);
		if(p_ii <= 8)
			strcat(p_tmp, " ");
		p_ii++;
	}
        /* for static mode */
        if(((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 )) || (atoi(wan_status) == 1)){
                cprintf("Write static settings\n");
		if((atoi(wan_status) == 0 ) && (atoi(fixedip) == 1 ))
			nvram_set("bridged_config",p_tmp);
                if(atoi(wan_status) == 1 )   
                        nvram_set("routed_config",p_tmp);
        }
        else if((atoi(wan_status) == 2) || (atoi(wan_status) == 3)){
                if (atoi(wan_status) == 2){
                        cprintf("Write pppoe settings\n");
			nvram_set("pppoe_config", p_tmp);
		}
                else if(atoi(wan_status) == 3){ // Dom 2005.03.28
                        cprintf("Write pppoa settings\n");
			nvram_set("pppoa_config", p_tmp);
		} 
	}
	p_ii = 0;
	cprintf("Write vcc config setting\n");//Dom 2005.03.30 add for DSL
//	cprintf ("VCC_debug1:%s \n",p_config);
//	cprintf ("VCC_debug2:%s \n",vcc_config);
	foreach(p_config, vcc_config, next)
	{
		if(p_ii == 0)
		{
			string_rep(p_config, 1, wan_vpi);
			string_rep(p_config, 2, wan_vci);
			string_rep(p_config, 3, wan_encapmode);
			string_rep(p_config, 4, wan_multiplex);
			strcpy(p_tmp, p_config);
		}
                else
			strcat(p_tmp, p_config);
		if(p_ii <= 8)
			strcat(p_tmp, " ");
                p_ii++;
	}
	nvram_set("vcc_config", p_tmp);

	p_ii = 0;
	cprintf("Write mtu config setting\n");//Dom 2006.02.10 add for MTU
	foreach(p_config, mtu_config, next)
	{
		if(p_ii == 0)
		{
			if ( mtu_enable != NULL ) {
				string_rep(p_config, 1, mtu_enable);
				if ( mtu_size != NULL ) {
					strncpy(p_config, p_config, (strrchr(p_config, ':') - p_config + 1));
					strcpy(p_config + (strrchr(p_config, ':') - p_config + 1), mtu_size);
				}
			}
			strcpy(p_tmp, p_config);
		}
		else
			strcat(p_tmp, p_config);
		if(p_ii <= 8)
			strcat(p_tmp, " ");
		p_ii++;
	}
        nvram_set("mtu_config", p_tmp);

	/* for wireless */
	if(wl_ssid){
                nvram_set("wl_ssid",wl_ssid);
		nvram_set("wl0_ssid",wl_ssid);
	}
	if(wl_mode){
		nvram_set("wl_gmode",wl_mode);
		nvram_set("wl0_gmode",wl_mode);
	}
        nvram_set("wl_channel",wl_channel);
	nvram_set("wl0_channel",wl_channel);
	nvram_set("wl_closed",wl_bcast);
	nvram_set("wl0_closed",wl_bcast);
	if(wl_status && atoi(wl_status) == 2){
		cprintf("Write PSK settings\n");
		if(wl_wpa_psk){
			nvram_set("wl_wpa_psk", wl_wpa_psk);
			nvram_set("wl0_wpa_psk",wl_wpa_psk);
		}
		nvram_set("wl_wep", wl_encryption);	// tkip or aes
                nvram_set("wl0_wep", wl_encryption);
		nvram_set("wl_auth_mode","psk");
		nvram_set("wl0_auth_mode","psk");
		nvram_set("security_mode","psk");
	}
	else if(wl_status && atoi(wl_status) == 1){
		cprintf("Write WEP settings\n");
		nvram_set("wl_wep","restricted");
		nvram_set("wl0_wep","restricted");
		//nvram_set("wl_auth_mode","wep");
		nvram_set("security_mode","wep");
	        nvram_set("wl_key",wl_key);
	        nvram_set("wl_wep_bit",wl_wep_bit);
	        nvram_set("wl_passphrase",wl_passphrase);
	        nvram_set("wl_key1",wl_key1);
		snprintf(wl_wep_buf, sizeof(wl_wep_buf),"%s:%s::::%s",wl_passphrase,wl_key1,wl_key);
       		nvram_set("wl_wep_buf",wl_wep_buf);
	}
	else if (wl_status){
		cprintf("Write no WEP settings\n");
		nvram_set("wl_macmode","disable");
		nvram_set("wl_macmode1","disable");
		nvram_set("wl_auth_mode","disabled");
		nvram_set("wl0_auth_mode","disabled");
		nvram_set("security_mode","disabled");
	}
	nvram_set("http_passwd",http_passwd);
	
	/* for aol */
#ifdef AOL_SUPPORT
	cprintf("Write AOL settings\n");
	if(atoi(aol_block_traffic) != 0)
		nvram_set("aol_block_traffic2","1");
	else
		nvram_set("aol_block_traffic2","0");
#endif

	goto ok;
	
fail:
	error_value = 1;
ok:
	if(mac)		free(mac);
	if(dns0)	free(dns0);
	if(dns1)	free(dns1);
	if(dns2)	free(dns2);
	if(wan_ip)	free(wan_ip);
	if(wan_mask)	free(wan_mask);
	if(wan_gw)	free(wan_gw);

	return 0;

}

static int
setup_wizard(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	     char_t *url, char_t *path, char_t *query)
{
	valid_value(wp);			// valid value
	if(websGetVar(wp, "sysAction_FactoryDefault", NULL)){//Dom 2005.03.30 Fixed can't run with setup_wizard issue
		cprintf("FactoryDefault\n");
		nvram_set("restore_defaults", "1");
		sys_commit();
		websDone(wp, 200);
		sleep(2);
		reboot(RB_AUTOBOOT);
		return 1;
	}
        else if(websGetVar(wp, "sysAction_Reboot", NULL)){
		cprintf("Action_Reboot\n");
		websDone(wp, 200);
		reboot(RB_AUTOBOOT);
		return 1;
	}

	if(error_value != 1){		// save to nvram if no any error
		sys_commit();	
		eval("wlcfg", "stop");
		eval("wlcfg", "start");
		kill(1, SIGHUP);	// restart system
	}

	if(!error_value)
        	do_ej("Success.asp",wp);
	else
        	do_ej("Fail.asp",wp);

	websDone(wp, 200);

	error_value = 0;

	return 1;
}

extern int do_ssl;
int
//do_setup_wizard(char *url, FILE *stream)
do_setup_wizard(char *url, webs_t stream) //jimmy, https, 8/4/2003
{
	char *path, *query;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): url=[%s]",__FUNCTION__,url);
#endif
	query = url;
	cprintf("url:%s\n", url);
	fprintf(stderr, "url:%s\n", url);
	path = strsep(&query, "?") ? : url;

	init_cgi(query);
	setup_wizard(stream, NULL, NULL, 0, url, path, query);
	init_cgi(NULL);
	return 0;
}
//#else
#if 0
extern int do_ssl;
int 
do_setup_wizard(char *url, webs_t stream){
//	cprintf("url:%s\n", url);
	websDone(stream, 200);
	if(!do_ssl){
		if(strcmp(url, "Gozila.cgi?SysInfo.htm=255&hid_returnPoint=&sysAction_FactoryDefault=1") == 0){
	//		cprintf("FactoryDefault\n");
			nvram_set("restore_defaults", "1");
			sys_commit();
			sleep(2);
			reboot(RB_AUTOBOOT);
		}else if(strcmp(url, "Gozila.cgi?SysInfo.htm=255&hid_returnPoint=&sysAction_Reboot=1") == 0){
	//		cprintf("Reboot\n");
			reboot(RB_AUTOBOOT);
		}
	}
        return 0;
}
#endif
void
show_error(webs_t wp, char *message)
{
	websWrite(wp, message);
	cprintf("%s\n", message);
}
#endif
/* Example:
  hint:
  -- the first character of Gozila.cgi must be uppercase letter
  -- the SetupWizard value must be 1

  -- DHCP mode without wep key encryption
     http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-NB&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_wrt54g&SWwirelessChannel=11&SWbroadcastSSID=1&SWwirelessStatus=0

  -- DHCP mode with 128 bit wep key encryption
     http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-NB&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_wrt54g&SWwirelessChannel=11&SWbroadcastSSID=1&SWwirelessStatus=1&SWwepKey=1&SWwepEncryption=128&SWpassphrase=abcd&SWwepKey1=386f81fd57366030ae7ea0392a

==================================================================
Bellow for Setup Wizard 2.0

DHCP & PSK mode (tkip)
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-nb&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_123&SWwirelessChannel=5&SWwirelessStatus=2&SWwlEncryption=tkip&SWwpaPSK=1234567890&SWRouterName=WRT54G_123 

DHCP & PSK mode (aes)
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-nb&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_111&SWwirelessChannel=1&SWwirelessStatus=2&SWwlEncryption=aes&SWwpaPSK=1111111111122222222223333333333&SWRouterName=WRT54G_111

DHCP & Disabled mode
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-nb&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_222&SWwirelessChannel=6&SWwirelessStatus=0&SWwlEncryption=off&SWRouterName=WRT54G_222

DHCP & WEP 64bit
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-nb&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_333&SWwirelessChannel=10&SWwirelessStatus=1&SWwlEncryption=wep&SWwepKey=1&SWwepEncryption=64&SWpassphrase=abcd&SWwepKey1=41192b542a&SWRouterName=WRT54G_333

DHCP & WEP 128bit
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWhostName=honor-nb&SWDomainName=cybertan.com.tw&SWwanStatus=0&SWwanStatus=0&SWaliasIP1=0&SWaliasIP2=0&SWaliasIP3=0&SWaliasIP4=0&pppoeStatus=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=00&SWwanMac1=00&SWwanMac2=39&SWwanMac3=A6&SWwanMac4=97&SWwanMac5=9B&SWwirelessESSID=linksys_555&SWwirelessChannel=7&SWwirelessStatus=1&SWwlEncryption=wep&SWwepKey=1&SWwepEncryption=128&SWpassphrase=123&SWwepKey1=ae07938c6f3da12a9704d37241&SWRouterName=WRT54G_555

PPPoE & PSK
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWwanStatus=2&pppoeStatus=1&SWpppoeUName=84204099@hinet.net&SWpppoePWD=2xgigldl&MACClone_Status=0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwanMac0=&SWwanMac1=&SWwanMac2=&SWwanMac3=&SWwanMac4=&SWwanMac5=&SWwirelessESSID=linksys_666&SWwirelessChannel=5&SWwirelessStatus=2&SWwlEncryption=tkip&SWwpaPSK=1234567890&SWRouterName=WRT54G_666

PPPoE & VCC
http://192.168.1.1/Gozila.cgi?SetupWizard=1&SWwanStatus=2&SWwan_encapmode=pppoe&SWpppoeUName=84204099@hinet.net&SWpppoePWD=2xgigldl&SWwan_multiplex=llc0&SWblock_traffic=0&SWsysPasswd=admin&SWsysPasswdconfirm=admin&SWwan_vpi=0&SWwan_vci=35&SWwirelessESSID=linksys_666&SWwirelessChannel=5&SWwirelessStatus=2&SWwlEncryption=tkip&SWwpaPSK=1234567890&SWRouterName=WRT54G_666&SWKaliveStatus=2&SWKaliveTime=20&SWppp_servicename=
*/

//Setup Wizard 2.0:
//-- Remove wireless broadcast
//-- Add Router Name
//-- Add WPA support
//-- Add SWwlEncryption, SWRouterName, SWwpaWPA
//-- Add SWwirelessMode for Parental Control
//-- Add PPPoA mode Setting & Vcc_config Setting
//-- Add SWKaliveStatus, SWKaliveTime, SWppp_servicename
