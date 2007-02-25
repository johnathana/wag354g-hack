/*
 * Frontend command-line utility for Linux NVRAM layer
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: main.c,v 1.1.1.2 2005/03/28 06:58:10 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <typedefs.h>
#include <bcmnvram.h>

static void
usage(void)
{
	fprintf(stderr, "usage: nvram [get name] [set name=value] [unset name] [show] [vcc_config] [pppoe_config] [mpppoe_config] [mpppoe_enable] [pppoa_config] [routed_config] [bridged_config] [wan_autoresult] [mtu_config] [macclone_enable] [macclone_addr] [entry_config] [ltype_config] [rtype_config] [sg_config] [keytype_config] [ipsecadv_config]\n");
	exit(0);
}

/* NVRAM utility */
int
main(int argc, char **argv)
{
	char *name, *value, buf[NVRAM_SPACE];
	int size;
	char *id; 	//by michael,  support CWMP's lock attribute 

	/* Skip program name */
	--argc;
	++argv;
	
	if (!*argv) 
		usage();

	/* Process the remaining arguments. */
	for (; *argv; argv++) {
		if (!strncmp(*argv, "get", 3)) {
			if (*++argv) {
				if ((value = nvram_get(*argv)))
					puts(value);
			}
		}
		else if (!strncmp(*argv, "set", 3)) {
			if (*++argv) {
				strncpy(value = buf, *argv, sizeof(buf));
				name = strsep(&value, "=");
				nvram_set(name, value);
			}
		}
		/* by michael, support CWMP's lock attribute */
		else if (!strncmp(*argv, "id_set", 6)) {
			if (*++argv) {
				strncpy(value = buf, *argv, sizeof(buf));
				name = strsep(&value, "=");
				if (*++argv) {					
					id = *argv;
					nvram_id_set(name, value, atoi(id));
				}
			}
		}
		else if (!strncmp(*argv, "unset", 5)) {
			if (*++argv)
				nvram_unset(*argv);
		}
		else if (!strncmp(*argv, "commit", 5)) {
			nvram_commit();
		}
		else if (!strncmp(*argv, "show", 4) ||
			   !strncmp(*argv, "getall", 6)) {
			nvram_getall(buf, sizeof(buf));
			for (name = buf; *name; name += strlen(name) + 1)
				puts(name);
			size = sizeof(struct nvram_header) + (int) name - (int) buf;
			fprintf(stderr, "size: %d bytes (%d left)\n", size, NVRAM_SPACE - size);
		}
		//junzhao
		else if(!strncmp(*argv, "vcc_config", 10))
		{
			char buff[] = "0:35:1483Bridged:LLC:UBR:0:0:0:0 8:35:1483Bridged:LLC:UBR:0:0:0:0 0:43:1483Bridged:LLC:UBR:0:0:0:0 8:43:1483Bridged:LLC:UBR:0:0:0:0 0:51:1483Bridged:LLC:UBR:0:0:0:0 8:51:1483Bridged:LLC:UBR:0:0:0:0 0:59:1483Bridged:LLC:UBR:0:0:0:0 8:59:1483Bridged:LLC:UBR:0:0:0:0";
			nvram_set("vcc_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "pppoe_config", 12))
		{
			char buff[] = "adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20: adsl:adsl:1:3:20:";
			nvram_set("pppoe_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "mpppoe_config", 13))
		{
			char buff[] = "::1:3:20:: ::1:3:20:: adsl:adsl:1:3:20:sv3:dm3 adsl:adsl:1:3:20:sv4:dm4 adsl:adsl:1:3:20:sv5:dm5 adsl:adsl:1:3:20:sv6:dm6 adsl:adsl:1:3:20:sv7:dm7 adsl:adsl:1:3:20:sv8:dm8";
			nvram_set("mpppoe_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "mpppoe_enable", 13))
		{
			char buff[] = "0 1 1 0 0 1 0 1";
			nvram_set("mpppoe_enable", buff);
			nvram_commit();
		}





		else if(!strncmp(*argv, "pppoa_config", 12))
		{
			char buff[] = "adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20 adsl:adsl:1:3:20";
			nvram_set("pppoa_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "routed_config", 13))
		{
			char buff[] = "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0";
			nvram_set("routed_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "bridged_config", 14))
		{
			char *buff = "0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0";
			nvram_set("bridged_config", buff);
			nvram_commit();
		}
/*
		else if(!strncmp(*argv, "wan_ifname", 10))
		{
			char buff[] = "no no no no no no no no";
			nvram_set("wan_ifname", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_ipaddr", 10))
		{
			char buff[] = "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0";
			nvram_set("wan_ipaddr", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_netmask", 11))
		{
			char buff[] = "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0";
			nvram_set("wan_netmask", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_gateway", 11))
		{
			char buff[] = "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0";
			nvram_set("wan_gateway", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_broadcast", 13))
		{
			char buff[] = "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0";
			nvram_set("wan_broadcast", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_get_dns", 11))
		{
			char buff[] = "0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0";
			nvram_set("wan_get_dns", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_pvc_status", 14))
		{
			char buff[] = "down down down down down down down down";
			nvram_set("wan_pvc_status", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "wan_pvc_errstr", 14))
		{
			char buff[] = "fail fail fail fail fail fail fail fail";
			nvram_set("wan_pvc_errstr", buff);
			nvram_commit();
		}
*/
		else if(!strncmp(*argv, "wan_autoresult", 14))
		{
			char buff[] = "0:-1 0:-1 0:-1 0:-1 0:-1 0:-1 0:-1 0:-1";
			nvram_set("wan_autoresult", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "mtu_config", 10))
		{
			char buff[] = "0:1500 0:1500 0:1500 0:1500 0:1500 0:1500 0:1500 0:1500";
			nvram_set("mtu_config", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "macclone_enable", 15))
		{
			char buff[] = "0 0 0 0 0 0 0 0";
			nvram_set("macclone_enable", buff);
			nvram_commit();
		}
		else if(!strncmp(*argv, "macclone_addr", 13))
		{
			char buff[] = "00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00";
			nvram_set("macclone_addr", buff);
			nvram_commit();
		}
/*		else if(!strncmp(*argv, "wan_hwaddr", 10))
		{
			char buff[] = "00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00";
			nvram_set("wan_hwaddr", buff);
			nvram_commit();
		}
*/
		else if (!strncmp(*argv, "entry_config", 12)) {
			char buf[]="1:test1:1:2:1:1:1:1:0  1:test2:1:3:1:1:1:1:0 1:test3:1:4:1:1:1:1:0 1:test4:1:6:1:1:1:1:0 1:test5:1:7:1:1:1:1:0";
			nvram_set("entry_config", buf);
			nvram_commit();
		}
		else if (!strncmp(*argv, "ltype_config", 12)) {
			char buf[]="192.168.1.0:255.255.255.0 192.168.1.0:255.255.255.0  192.168.1.0:255.255.255.0 192.168.1.0:255.255.255.0 192.168.1.0:255.255.255.0";
			nvram_set("ltype_config", buf);
			nvram_commit();
		}
		else if (!strncmp(*argv, "rtype_config", 12)) {
			char buf[]="200.0.0.0:255.255.255.0 200.0.0.0:255.255.255.0 200.0.0.0:255.255.255.0 200.0.0.0:255.255.255.0 200.0.0.0:255.255.255.0";
			nvram_set("rtype_config", buf);
			nvram_commit();
		}
	else if (!strncmp(*argv, "sg_config", 9)) {
			char buf[]="100.0.0.1:cytertan 100.0.0.1:www.sina.com.cn 100.0.0.1:www.sohu.com 100.0.0.1:www.aol.com 100.0.0.1:www.edu.cn";
			nvram_set("sg_config", buf);
			nvram_commit();
		}
	else if (!strncmp(*argv, "keytype_config", 14)) {
			char buf[]="1:hello:3600:null:null:null:null 1:hello:3600:null:null:null:null 1:hello:3600:null:null:null:null 1:hello:3600:null:null:null:null 1:hello:3600:null:null:null:null ";
			nvram_set("keytype_config", buf);
			nvram_commit();
		}
	else if (!strncmp(*argv, "ipsecadv_config", 15)) {
			char buf[]="0:2:1:1:3600:2:0:1:0:0:5:60 1:1:1:1:1800:2:0:1:0:0:5:60 1:2:2:2:3600:2:0:1:0:0:6:300 0:1:2:1:1000:2:0:1:0:0:10:300 0:2:1:1:3600:2:0:1:0:0:8:120";
			nvram_set("ipsecadv_config", buf);
			nvram_commit();
		}
		if (!*argv)		
			break;
	
	else if (!strncmp(*argv, "test", 4)) {
			char buf[]="&(5555555555;^!@#$%^&*()./?{}~`";
			nvram_set("test", buf);
			nvram_commit();
		}
	}


	return 0;
}	
