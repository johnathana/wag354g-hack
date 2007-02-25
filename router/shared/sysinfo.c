
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

#include <broadcom.h>
#include <cyutils.h>

// for test
int
ej_show_sysinfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;
	
	websDone(wp, 200);	// Let header in first packet, and bellow information in second packet.
	
	ret = websWrite(wp, "Vendor:%s\n", VENDOR);
	ret = websWrite(wp, "ModelName:%s\n", MODEL_NAME);
	ret = websWrite(wp, "Firmware Version:%s%s , %s\n", CYBERTAN_VERSION,MINOR_VERSION, nvram_safe_get("release_date"));
	ret = websWrite(wp, "#:%s\n", SERIAL_NUMBER);
	//ret = websWrite(wp, "Boot Version:%d.%d.%d\n", MonitorMajorRev, MonitorMinorRev, TelogyMonitorRev);
	ret = websWrite(wp, "Boot Version:%s\n", nvram_safe_get("boot_ver"));
	//ret = websWrite(wp, "CodePattern:%s\n", CODE_PATTERN);
	//modify by  junzhao 2004.3.15
	ret = websWrite(wp, "CodePattern:%s\n", nvram_safe_get("code_pattern"));
#if LOCALE == EUROPE
#if RELCOUN == FRANCE
	ret = websWrite(wp, "Country:France\n");
#elif RELCOUN == GERMANY
	ret = websWrite(wp, "Country:Germany\n");
#elif RELCOUN == AUSTRALIA
	ret = websWrite(wp, "Country:Australia\n");
#elif RELCOUN == UK
	ret = websWrite(wp, "Country:United Kingdom\n");
#elif RELCOUN == SWEDEN
	ret = websWrite(wp, "Country:Sweden\n");
#elif RELCOUN == ICELAND
	ret = websWrite(wp, "Country:Iceland\n");
#else
	ret = websWrite(wp, "Country:Europe\n");
#endif
	//For France
	//ret = websWrite(wp, "Country:France\n");
#elif LOCALE == JAPAN
	ret = websWrite(wp, "Country:Japan\n");
#elif LOCALE == KOREA	// <remove the line>
	ret = websWrite(wp, "Country:Korea\n");	// <remove the line>
#else
	ret = websWrite(wp, "Country:US\n");
#endif
	ret = websWrite(wp, "\n");

	{
		char buf[100];
		FILE *fp;
		memset(buf,0,sizeof(buf));

		if((fp = fopen("/proc/avalanche/avsar_ver","r")) == NULL)
		{
			printf("open failed!\n");
			return -1;
		}
		while(fgets(buf,sizeof(buf),fp))
		{
			if(strstr(buf,"Datapump"))
				break;
		}
		ret = websWrite(wp, "DSL %s",buf);
	}

	{
		char *vpi,*vci,*t;
		vci = nvram_safe_get("vcc_config");
		vpi = strsep(&vci,":");
		t = vci;
		vci = strsep(&t,":");

		ret = websWrite(wp, "DSL VPI/VCI:%s/%s\n",vpi,vci);
	}

	ret = websWrite(wp, "\n");

#ifdef WIRELESS_SUPPORT	
	ret = websWrite(wp, "RF Status:%s\n", (nvram_match("wl0_hwaddr","") || nvram_match("wl_gmode", "-1")) ? "disabled" :"enabled");
	//ret = websWrite(wp, "RF Firmware Version:%s%s\n", CYBERTAN_VERSION,MINOR_VERSION);
	//modify by  junzhao 2004.3.15
	ret = websWrite(wp, "RF Firmware Version:%s\n", nvram_safe_get("wl_firmware_version"));
#if LOCALE == EUROPE
	ret = websWrite(wp, "RF Domain:ETSI (channel 1~%s)\n", WL_MAX_CHANNEL);
#elif LOCALE == JAPAN
	ret = websWrite(wp, "RF Domain:JPN (channel 1~%s)\n", WL_MAX_CHANNEL);
#elif LOCALE == KOREA	// <remove the line>
	ret = websWrite(wp, "RF Domain:Korean (channel 1~%s)\n", WL_MAX_CHANNEL);	// <remove the line>
#else
	ret = websWrite(wp, "RF Domain:US (channel 1~%s)\n", WL_MAX_CHANNEL);
#endif
	ret = websWrite(wp, "RF Channel:%s\n", nvram_safe_get("wl_channel"));
	ret = websWrite(wp, "RF SSID:%s\n", nvram_safe_get("wl_ssid"));
#endif
	ret = websWrite(wp, "\n-----Dynamic Information\n");

	//ret = websWrite(wp, "RF Mac Address:%s\n", nvram_safe_get("wl0_hwaddr"));
	{
	char hwaddr[20];
#ifdef WIRELESS_SUPPORT	

#ifdef CONFIG_ANTSEL
	websWrite(wp, "RF Antenna Selection:%s\n", nvram_safe_get("wl_antselstat"));
#endif
	memset(hwaddr, 0, sizeof(hwaddr));
	strncpy(hwaddr, nvram_safe_get("wl0_hwaddr"), sizeof(hwaddr));
	hwaddr_convert(hwaddr);
	websWrite(wp, "RF Mac Address:%s\n", hwaddr);
#endif	
	memset(hwaddr, 0, sizeof(hwaddr));
	strncpy(hwaddr, nvram_safe_get("lan_hwaddr"), sizeof(hwaddr));
	hwaddr_convert(hwaddr);
	ret = websWrite(wp, "LAN Mac Address:%s\n", hwaddr);
	memset(hwaddr, 0, sizeof(hwaddr));
	strncpy(hwaddr, nvram_safe_get("wan_def_hwaddr"), sizeof(hwaddr));
	hwaddr_convert(hwaddr);
	ret = websWrite(wp, "WAN Mac Address:%s\n", hwaddr);
	}
#ifdef WRITE_SN_SUPPORT
	ret = websWrite(wp, "Device Serial No.:%s\n", nvram_safe_get("get_sn"));
#endif
	//ret = websWrite(wp, "LAN Mac Address:%s\n", nvram_safe_get("lan_hwaddr"));
	//ret = websWrite(wp, "WAN Mac Address:%s\n", nvram_safe_get("wan_def_hwaddr"));
/*	
	if(check_hw_type() == BCM4702_CHIP)
		ret = websWrite(wp, "Hardware Version:1.x\n");
	else
		ret = websWrite(wp, "Hardware Version:2.0\n");
*/
    {
	char readbuf[8];
	FILE *fp;
	memset(readbuf, 0, sizeof(readbuf));
	if(!(fp=fopen("/proc/led_mod/hardware_version", "r")))
		strcpy(readbuf, " ");
	else
	{
		if(fread(readbuf, 1, sizeof(readbuf), fp))
		{	
			char *p = strstr(readbuf, "\n");
			if(p != NULL)
				*p = '\0';
		}
		else
			strcpy(readbuf, " ");
		fclose(fp);
	}
	ret = websWrite(wp, "Hardware Version:%s\n", readbuf);
   }
		
	ret = websWrite(wp, "Fast Mode Downstream Rate:%d kbits/s\n", sar_getstats("DS Connection Rate:"));
	ret = websWrite(wp, "Fast Mode Upstream Rate:%d kbits/s\n", sar_getstats("US Connection Rate:"));
	ret = websWrite(wp, "CRC Up:%d\n", sar_getstats("[Upstream (TX) Fast path]\n\tCRC:"));
	ret = websWrite(wp, "CRC Dn:%d\n", sar_getstats("[Downstream (RX) Fast path]\n\tCRC:"));
	//ret = websWrite(wp, "CRC Dn:%d\n", sar_getstats(":"));
    
	//ret = websWrite(wp, "\n");	// The last char must be '\n'
	
	return ret;
}

// for Setup Wizard and others test
int
ej_show_miscinfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	
	websDone(wp, 200);	// Let header in first packet, and bellow information in second packet.
	
	ret += websWrite(wp, "Module Name=%s;\n", MODEL_NAME);
	ret += websWrite(wp, "Firmware Version=%s%s,%s;\n", CYBERTAN_VERSION, MINOR_VERSION, __DATE__);
	ret += websWrite(wp, "Firmware Time=%s;\n", __TIME__);
	ret += websWrite(wp, "Flash Type=%s;\n", nvram_safe_get("flash_type"));
	ret += websWrite(wp, "Write Mac Address=%s;\n", nvram_safe_get("et0macaddr"));
	ret += websWrite(wp, "SWAOLstatus=%s;\n", nvram_safe_get("aol_block_traffic"));
	ret += websWrite(wp, "SWAT&Tstatus=0;\n");

	if(nvram_match("wan_proto","dhcp"))
		ret += websWrite(wp, "SWWanStatus=0;\n");
	if(nvram_match("wan_proto","static"))
		ret += websWrite(wp, "SWWanStatus=1;\n");
	if(nvram_match("wan_proto","pppoe"))
		ret += websWrite(wp, "SWWanStatus=2;\n");
	if(nvram_match("wan_proto","pptp"))
		ret += websWrite(wp, "SWWanStatus=3;\n");

	ret += websWrite(wp, "\n");
	
	ret += websWrite(wp, "SWGetRouterIP=%s;\n", nvram_safe_get("lan_ipaddr"));
	ret += websWrite(wp, "SWGetRouterDomain=%s;\n", nvram_safe_get("wan_domain"));
	ret += websWrite(wp, "SWpppoeUName=%s;\n", nvram_safe_get("ppp_username"));
		
	ret += websWrite(wp, "\n");

#ifdef WIRELESS_SUPPORT	
	ret += websWrite(wp, "SWGetRouterSSID=%s;\n", nvram_safe_get("wl_ssid"));
	ret += websWrite(wp, "SWGetRouterChannel=%s;\n", nvram_safe_get("wl_channel"));
	ret += websWrite(wp, "SWssidBroadcast=%s;\n", nvram_safe_get("wl_closed"));
	
	ret += websWrite(wp, "\n");
	
	if(nvram_match("security_mode", "disabled"))
		ret += websWrite(wp, "SWwirelessStatus=0;\n");
	if(nvram_match("security_mode", "wep"))
		ret += websWrite(wp, "SWwirelessStatus=1;\n");
	if(nvram_match("security_mode", "psk"))
		ret += websWrite(wp, "SWwirelessStatus=2;\n");
	if(nvram_match("security_mode", "radius"))
		ret += websWrite(wp, "SWwirelessStatus=3;\n");

	if(nvram_match("wl_wep", "off"))
		ret += websWrite(wp, "SWwlEncryption=off;\n");
	if(nvram_match("wl_wep", "on") || nvram_match("wl_wep", "restricted")){
		ret += websWrite(wp, "SWwlEncryption=wep;\n");
		ret += websWrite(wp, "SWwepEncryption=%s;\n", nvram_safe_get("wl_wep_bit"));
	}
	if(nvram_match("wl_wep", "tkip"))
		ret += websWrite(wp, "SWwlEncryption=tkip;\n");
	if(nvram_match("wl_wep", "aes"))
		ret += websWrite(wp, "SWwlEncryption=aes;\n");

	/* Below for RF test 2003-10-29 */
	ret += websWrite(wp, "WL_tssi_result=%s;\n", nvram_safe_get("wl_tssi_result"));
#endif	
	return ret;
}
