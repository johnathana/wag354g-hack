
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

/*
 * Router default NVRAM values
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: defaults.c,v 1.24 2006/09/08 03:35:51 dom Exp $
 */

#include <epivers.h>
#include <string.h>
#include <bcmnvram.h>
#include <typedefs.h>
#include <wlioctl.h>

#include <code_pattern.h>
#include <cy_conf.h>

#define XSTR(s) STR(s)
#define STR(s) #s

struct nvram_tuple router_defaults[] = {
        /*for tr069 parameters . add by peny*/
#ifdef HTTPS_SUPPORT
        {"v3authpasw","1111111111",0},
        {"v3rivpasw","1111111111",0},
	{"v3rwuser" ,"v3rwuser",0},
	/*Lai add 2005-07-11*/
	{"privprotocol", "DES", 0},
#else	
        {"v3authpasw","1111111111",0},
	{"v3privpasw","",0},
	{"v3rwuser" ,"v3rwuser",0},
#endif	
        {"authprotocol","MD5",0},
	/*Lai add 2005-07-11*/
	{"snmpv3_support", "Disable", 0},
	/* da_f@2005.3.4 modify
	 * Do not need "enable_inform" any more.
	{"enable_inform","0",0}, */
	{"trap_ipaddr","192.168.1.100",0},
	{"inform_ip","0.0.0.0",0},
	/* OS parameters */
	{ "os_name", "", 0 },			/* OS name string */
	{ "os_version", EPI_VERSION_STR, 0 },	/* OS revision */
	{ "os_date", __DATE__, 0 },		/* OS date */
	{ "ct_modules", "", 0 },		/* CyberTAN kernel modules */

	/* Miscellaneous parameters */
	{ "timer_interval", "3600", 0 },	/* Timer interval in seconds */
	{ "ntp_server", "", 0 },		/* NTP server */	/* Modify */
	//{ "time_zone", "PST8PDT", 0 },		/* Time zone (GNU TZ format) */
#if OEM == LINKSYS
#if COUNTRY == JAPAN
	{ "time_zone", "+09 1 0", 0 },		/* Time zone (GNU TZ format) Japan */
	{ "daylight_time", "0", 0 },		/* Automatically adjust clock for daylight */
#else
	{ "time_zone", "-08 1 1", 0 },		/* Time zone (GNU TZ format) USA */
	{ "daylight_time", "1", 0 },		/* Automatically adjust clock for daylight */
#endif
#elif OEM == PCI && LANGUAGE == ENGLISH
	{ "time_zone", "+08 2 0", 0 },		/* Time zone (GNU TZ format) (2003-03-19 by honor) */
	{ "daylight_time", "0", 0 },		/* Automatically adjust clock for daylight */
#elif OEM == ELSA
	{ "time_zone", "+01 2 2", 0 },		/* Time zone (GNU TZ format) Germany */
	{ "daylight_time", "1", 0 },		/* Automatically adjust clock for daylight */
#elif COUNTRY == KOREA
	{ "time_zone", "+09 1 0", 0 },		/* Time zone (GNU TZ format) Japan */
	{ "daylight_time", "0", 0 },		/* Automatically adjust clock for daylight */
#else
	{ "time_zone", "-08 1 1", 0 },		/* Time zone (GNU TZ format) USA */
	{ "daylight_time", "1", 0 },		/* Automatically adjust clock for daylight */
#endif

	//{ "log_level", "0", 0 },		/* Bitmask 0:off 1:denied 2:accepted */

#if OEM == LINKSYS
	{ "upnp_enable", "1", 0 },		/* 0:Disable 1:Enable */
#else
	{ "upnp_enable", "0", 0 },		/* 0:Disable 1:Enable */
#endif
	{ "os_server", "", 0 },			/* URL for getting upgrades */
	{ "stats_server", "", 0 },		/* URL for posting stats */
	{ "console_loglevel", "1", 0 },		/* Kernel panics only */

	/* Big switches */

	{ "router_disable", "0", 0 },		/* lan_proto=static lan_stp=0 wan_proto=disabled */
	{ "fw_disable", "0", 0 },		/* Disable firewall (allow new connections from the WAN) */

	/* TCP/IP parameters */
#if (RELCOUN == AUSTRALIA || RELCOUN == EUROPE  || RELCOUN == GERMANY )
	{ "log_enable", "1", 0 },		/* 0:Disable 1:Eanble */	/* Add */
	{ "log_level", "2", 0 },		/* Bitmask 0:off 1:denied 2:accepted */
#else
	{ "log_enable", "0", 0 },		/* 0:Disable 1:Eanble */	/* Add */
	{ "log_level", "0", 0 },		/* Bitmask 0:off 1:denied 2:accepted */
#endif
	{ "log_ipaddr", "255", 0 },		/* syslog recipient */
#ifdef SYSLOG_SUPPORT
	{ "log_show_all", "0", 0 },		/* show all message */
	{ "log_show_type", "ALL", 0 },		/* show log type */
#endif

	/* LAN H/W parameters */
	{ "lan_ifname", "", 0 },		/* LAN interface name */
	{ "lan_ifnames", "", 0 },		/* Enslaved LAN interfaces */
	{ "lan_hwnames", "", 0 },		/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "", 0 },		/* LAN interface MAC address */

	/* LAN TCP/IP parameters */
#if OEM == LINKSYS
	{ "lan_proto", "dhcp", 0 },		/* [static|dhcp] */
#elif OEM == ALLNET
	{ "lan_proto", "pppoe", 0 },		/* [static|dhcp] */
#else
	{ "lan_proto", "dhcp", 0 },		/* [static|dhcp] */
#endif
	{ "auto_search_ip", "0", 0 },           /* auto search local ip by lzh*/
	{ "lan_ipaddr", "192.168.1.1", 0 },	/* LAN IP address */
	{ "lan_netmask", "255.255.255.0", 0 },	/* LAN netmask */
	{ "lan_stp", "0", 0 },			/* LAN spanning tree protocol */	/* Please set to 0 */
	{ "lan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "lan_domain", "", 0 },		/* LAN domain name */
//junzhao
//	{ "lan_lease", "86400", 0 },		/* LAN lease time in seconds */
	{ "lan_lease", "1440", 0 },		/* LAN lease time in seconds */
	//junzhao 2004.4.22 dhcprelay
	{ "dhcpserver_ipaddr", "0.0.0.0", 0 },	/* LAN IP address */
	
	/* WAN H/W parameters */
	{ "wan_ifname", "", 0 },		/* WAN interface name */
	{ "wan_ifnames", "", 0 },		/* WAN interface names */
	{ "wan_hwname", "", 0 },		/* WAN driver name (e.g. et1) */
	{ "wan_hwaddr", "", 0 },		/* WAN interface MAC address */

	/* WAN TCP/IP parameters */
	{ "wan_proto", "dhcp", 0 },		/* [static|dhcp|pppoe|disabled] */
	{ "wan_ipaddr", "0.0.0.0", 0 },		/* WAN IP address */
	{ "wan_netmask", "0.0.0.0", 0 },	/* WAN netmask */
	{ "wan_gateway", "0.0.0.0", 0 },	/* WAN gateway */
	{ "wan_dns", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_hostname", "", 0 },		/* WAN hostname */
	{ "wan_domain", "", 0 },		/* WAN domain name */
	{ "wan_lease", "86400", 0 },		/* WAN lease time in seconds */
	{ "static_route", "", 0 },		/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */
	{ "static_route_name", "", 0 },		/* Static routes name ($NAME:name) */

#ifdef BRCM_3_51_8
	/* PPPoE parameters */
	{ "wan_pppoe_ifname", "", 0 },		/* PPPoE enslaved interface */
	{ "wan_pppoe_username", "", 0 },	/* PPP username */
	{ "wan_pppoe_passwd", "", 0 },		/* PPP password */
	{ "wan_pppoe_idletime", "60", 0 },	/* Dial on demand max idle time (seconds) */
	{ "wan_pppoe_keepalive", "0", 0 },	/* Restore link automatically */
	{ "wan_pppoe_demand", "0", 0 },		/* Dial on demand */
	{ "wan_pppoe_mru", "1492", 0 },		/* Negotiate MRU to this value */
	{ "wan_pppoe_mtu", "1492", 0 },		/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ "wan_pppoe_service", "", 0 },		/* PPPoE service name */
	{ "wan_pppoe_ac", "", 0 },		/* PPPoE access concentrator name */

	/* Misc WAN parameters */
	{ "wan_desc", "", 0 },			/* WAN connection description */
	{ "wan_route", "", 0 },			/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */
#endif
	{ "wan_primary", "1", 0 },		/* Primary wan connection */
	{ "wan_unit", "0", 0 },			/* Last configured connection */

	/* Filters */
	{ "filter_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "filter_macmode", "deny", 0 },	/* "allow" only, "deny" only, or "disabled" (allow all) */
	{ "filter_client0", "", 0 },		/* [lan_ipaddr0-lan_ipaddr1|*]:lan_port0-lan_port1,proto,enable,day_start-day_end,sec_start-sec_end,desc */

	{ "filter", "on", 0 },			/* [on | off] Firewall Protection */
	{ "filter_port", "", 0 },		/* [lan_ipaddr|*]:lan_port0-lan_port1 */
	{ "filter_rule1", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule2", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule3", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule4", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule5", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule6", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule7", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule8", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule9", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_rule10", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_tod1", "", 0 },		/* Filter Time of the day */
	{ "filter_tod2", "", 0 },		/* Filter Time of the day */
	{ "filter_tod3", "", 0 },		/* Filter Time of the day */
	{ "filter_tod4", "", 0 },		/* Filter Time of the day */
	{ "filter_tod5", "", 0 },		/* Filter Time of the day */
	{ "filter_tod6", "", 0 },		/* Filter Time of the day */
	{ "filter_tod7", "", 0 },		/* Filter Time of the day */
	{ "filter_tod8", "", 0 },		/* Filter Time of the day */
	{ "filter_tod9", "", 0 },		/* Filter Time of the day */
	{ "filter_tod10", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf1", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf2", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf3", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf4", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf5", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf6", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf7", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf8", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf9", "", 0 },		/* Filter Time of the day */
	{ "filter_tod_buf10", "", 0 },		/* Filter Time of the day */
	{ "filter_ip_grp1", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp2", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp3", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp4", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp5", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp6", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp7", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp8", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp9", "", 0 },		/* Filter IP group 1 */
	{ "filter_ip_grp10", "", 0 },		/* Filter IP group 1 */
	{ "filter_mac_grp1", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp2", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp3", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp4", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp5", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp6", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp7", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp8", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp9", "", 0 },		/* Filter MAC group 1 */
	{ "filter_mac_grp10", "", 0 },		/* Filter MAC group 1 */
/* da_f@2005.1.31 */
	{ "filter_lan_checkbox1", "0", 0 },	/* Filter Lan Checkbox 1 */
	{ "filter_lan_checkbox2", "0", 0 },	/* Filter Lan Checkbox 2 */
	{ "filter_lan_checkbox3", "0", 0 },	/* Filter Lan Checkbox 3 */
	{ "filter_lan_checkbox4", "0", 0 },	/* Filter Lan Checkbox 4 */
	{ "filter_lan_checkbox5", "0", 0 },	/* Filter Lan Checkbox 5 */
	{ "filter_lan_checkbox6", "0", 0 },	/* Filter Lan Checkbox 6 */
	{ "filter_lan_checkbox7", "0", 0 },	/* Filter Lan Checkbox 7 */
	{ "filter_lan_checkbox8", "0", 0 },	/* Filter Lan Checkbox 8 */
	{ "filter_lan_checkbox9", "0", 0 },	/* Filter Lan Checkbox 9 */
	{ "filter_lan_checkbox10", "0", 0 },/* Filter Lan Checkbox 10 */
	{ "filter_wan_checkbox1", "1", 0 }, /* Filter Wan Checkbox 1 */
	{ "filter_wan_checkbox2", "1", 0 }, /* Filter Wan Checkbox 2 */
	{ "filter_wan_checkbox3", "1", 0 }, /* Filter Wan Checkbox 3 */
	{ "filter_wan_checkbox4", "1", 0 }, /* Filter Wan Checkbox 4 */
	{ "filter_wan_checkbox5", "1", 0 }, /* Filter Wan Checkbox 5 */
	{ "filter_wan_checkbox6", "1", 0 }, /* Filter Wan Checkbox 6 */
	{ "filter_wan_checkbox7", "1", 0 }, /* Filter Wan Checkbox 7 */
	{ "filter_wan_checkbox8", "1", 0 }, /* Filter Wan Checkbox 8 */
	{ "filter_wan_checkbox9", "1", 0 }, /* Filter Wan Checkbox 9 */
	{ "filter_wan_checkbox10", "1", 0 },/* Filter Wan Checkbox 10 */
	{ "filter_wan_ip_grp1", "", 0 },    /* Filter Wan IP group 1 */
	{ "filter_wan_ip_grp2", "", 0 },    /* Filter Wan IP group 2 */
	{ "filter_wan_ip_grp3", "", 0 },    /* Filter Wan IP group 3 */
	{ "filter_wan_ip_grp4", "", 0 },    /* Filter Wan IP group 4 */
	{ "filter_wan_ip_grp5", "", 0 },    /* Filter Wan IP group 5 */
	{ "filter_wan_ip_grp6", "", 0 },    /* Filter Wan IP group 6 */
	{ "filter_wan_ip_grp7", "", 0 },    /* Filter Wan IP group 7 */
	{ "filter_wan_ip_grp8", "", 0 },    /* Filter Wan IP group 8 */
	{ "filter_wan_ip_grp9", "", 0 },    /* Filter Wan IP group 9 */
	{ "filter_wan_ip_grp10", "", 0 },   /* Filter Wan IP group 10 */
	{ "filter_wan_ip_range1", "", 0 },  /* Filter Wan IP range 1 */
	{ "filter_wan_ip_range2", "", 0 },  /* Filter Wan IP range 2 */
	{ "filter_wan_ip_range3", "", 0 },  /* Filter Wan IP range 3 */
	{ "filter_wan_ip_range4", "", 0 },  /* Filter Wan IP range 4 */
	{ "filter_wan_ip_range5", "", 0 },  /* Filter Wan IP range 5 */
	{ "filter_wan_ip_range6", "", 0 },  /* Filter Wan IP range 6 */
	{ "filter_wan_ip_range7", "", 0 },  /* Filter Wan IP range 7 */
	{ "filter_wan_ip_range8", "", 0 },  /* Filter Wan IP range 8 */
	{ "filter_wan_ip_range9", "", 0 },  /* Filter Wan IP range 9 */
	{ "filter_wan_ip_range10", "", 0 }, /* Filter Wan IP range 10 */
	{ "filter_web_host1", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host2", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host3", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host4", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host5", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host6", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host7", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host8", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host9", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_host10", "", 0 },		/* Website Blocking by URL Address */
	{ "filter_web_url1", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url2", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url3", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url4", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url5", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url6", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url7", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url8", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url9", "", 0 },		/* Website Blocking by keyword */
	{ "filter_web_url10", "", 0 },		/* Website Blocking by keyword */
	{ "filter_port_grp1", "", 0 },		/* Blocked Services */
	{ "filter_port_grp2", "", 0 },		/* Blocked Services */
	{ "filter_port_grp3", "", 0 },		/* Blocked Services */
	{ "filter_port_grp4", "", 0 },		/* Blocked Services */
	{ "filter_port_grp5", "", 0 },		/* Blocked Services */
	{ "filter_port_grp6", "", 0 },		/* Blocked Services */
	{ "filter_port_grp7", "", 0 },		/* Blocked Services */
	{ "filter_port_grp8", "", 0 },		/* Blocked Services */
	{ "filter_port_grp9", "", 0 },		/* Blocked Services */
	{ "filter_port_grp10", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp1", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp2", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp3", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp4", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp5", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp6", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp7", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp8", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp9", "", 0 },		/* Blocked Services */
	{ "filter_dport_grp10", "", 0 },	/* Blocked Services */
	{ "filter_services", "$NAME:003:DNS$PROT:003:udp$PORT:005:53:53<&nbsp;>$NAME:004:Ping$PROT:004:icmp$PORT:003:0:0<&nbsp;>$NAME:004:HTTP$PROT:003:tcp$PORT:005:80:80<&nbsp;>$NAME:005:HTTPS$PROT:003:tcp$PORT:007:443:443<&nbsp;>$NAME:003:FTP$PROT:003:tcp$PORT:005:21:21<&nbsp;>$NAME:004:POP3$PROT:003:tcp$PORT:007:110:110<&nbsp;>$NAME:004:IMAP$PROT:003:tcp$PORT:007:143:143<&nbsp;>$NAME:004:SMTP$PROT:003:tcp$PORT:005:25:25<&nbsp;>$NAME:004:NNTP$PROT:003:tcp$PORT:007:119:119<&nbsp;>$NAME:006:Telnet$PROT:003:tcp$PORT:005:23:23<&nbsp;>$NAME:004:SNMP$PROT:003:udp$PORT:007:161:161<&nbsp;>$NAME:004:TFTP$PROT:003:udp$PORT:005:69:69<&nbsp;>$NAME:003:IKE$PROT:003:udp$PORT:007:500:500<&nbsp;>", 0 },		/* Services List */
#ifdef FIREWALL_LEVEL_SUPPORT
	{ "filter_level", "0", 0 },		/* Security Level */
	{ "filter_inb_rule1", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule2", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule3", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule4", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule5", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule6", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule7", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule8", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule9", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_rule10", "", 0 },		/* $STAT: $NAME:$$ */
	{ "filter_inb_tod1", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod2", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod3", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod4", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod5", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod6", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod7", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod8", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod9", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod10", "", 0 },		/* Filter Time of the day */
	{ "filter_inb_tod_buf1", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf2", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf3", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf4", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf5", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf6", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf7", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf8", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf9", "", 0 },	/* Filter Time of the day */
	{ "filter_inb_tod_buf10", "", 0 },	/* Filter Time of the day */
	{ "filter_mix_grp1", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp2", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp3", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp4", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp5", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp6", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp7", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp8", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp9", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
	{ "filter_mix_grp10", "$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>", 0 },		/* Inbound Traffic Mix Rules */
#endif

	/* Port forwards */
	{ "dmz_enable", "0", 0 },		/* Enable (1) or Disable (0) */
	{ "dmz_ipaddr", "0", 0 },		/* x.x.x.x (equivalent to 0-60999>dmz_ipaddr:0-60999) */
	{"snmp_enable","0",0},  //by songtao
	{"get_auth","public",1},
	{"set_auth","private",0},
	{ "autofw_port0", "", 0 },		/* out_proto:out_port,in_proto:in_port0-in_port1>to_port0-to_port1,enable,desc */

#ifdef WIRELESS_SUPPORT
#if   (PROJECT == WAG354G)
	{"devname","WAG354G",0},
#elif (PROJECT == WAG354GV2)
	{"devname","WAG354G V.2",0},
#elif (PROJECT == HG200)
	{"devname","HG200",0},
#elif (PROJECT == DWG811A1)
	{"devname","DWG811A1",0},
#else
	{"devname","WAG54G V.2",0},
#endif
#elif (PROJECT == DWG814H)
	{"devname","DWG814H",0},
#elif (PROJECT == AG241V2)
	{"devname","AG241V2",0},
#else
	{"devname","AG241",0},
#endif	
	/* DHCP server parameters */
	//{ "dhcp_start", "100", 0 },		/* First assignable DHCP address */
	{ "dhcp_start", "64", 0 },		/* First assignable DHCP address */
	//{ "dhcp_end", "150", 0 },		/* Last assignable DHCP address */	/* Remove */
	{ "dhcp_num", "191", 0 },		/* Number of DHCP Users */	/* Add */
#if OEM == LINKSYS
	{ "dhcp_lease", "0", 0 },		/* LAN lease time in minutes */
#else
	{ "dhcp_lease", "60", 0 },		/* LAN lease time in minutes */ /* Add */
#endif
	{ "dhcp_domain", "wan", 0 },		/* Use WAN domain name first if available (wan|lan) */
	{ "dhcp_wins", "wan", 0 },		/* Use WAN WINS first if available (wan|lan) */
//	{ "wan_get_dns", "", 0 },		/* DNS IP address which get by dhcpc */ /* Add */
	//add by lzh to support detecting LAN dhcp server; 
	{ "AutoDetect_dhcp", "1", 0},
	{ "lan_detect_dhcp", "0", 0},
	{ "client_ip_lease", "172800", 0},
	{ "lan_dhcp_dns", "192.168.1.1", 0},

	/* Web server parameters */
#ifdef DDM_SUPPORT
	{ "http_username", "admin", 0 },		/* Username */
//junzhao
#else
	{ "http_username", "admin", 0 },		/* Username */
#endif
//cjg
	{"http_remote_username", "tech", 0},
#if OEM == LINKSYS

#ifdef DDM_SUPPORT
	{ "http_passwd", "password", 0 },		/* DDM_SUPPORT Password */
//cjg
	{"http_remote_passwd", "password", 0},
#else
	{ "http_passwd", "admin", 0 },		/* Password */
	{"http_remote_passwd", "admin", 0},
#endif

#ifdef DDM_SUPPORT
	{ "http_def_passwd", "password", 0 },		/* DDM_SUPPORT Password */
#else
	{ "http_def_passwd", "admin", 0 },		/* Password */
#endif

#elif OEM == PCI && LANGUAGE == ENGLISH
	{ "http_passwd", "0000", 0 },		/* Password */
#elif OEM == PCI && LANGUAGE == JAPANESE
	{ "http_passwd", "password", 0 },		/* Password */
#else
	{ "http_passwd", "admin", 0 },		/* Password */
#endif
	{ "http_wanport", "51003", 0 },		/* WAN port to listen on */
	{ "http_lanport", "80", 0 },		/* LAN port to listen on */

	/* PPPoE parameters */
	{ "pppoe_ifname", "", 0 },		/* PPPoE enslaved interface */
	{ "ppp_username", "", 0 },		/* PPP username */
	{ "ppp_passwd", "", 0 },		/* PPP password */
	{ "ppp_idletime", "5", 0 },		/* Dial on demand max idle time (mins) */
	{ "ppp_keepalive", "0", 0 },		/* Restore link automatically */
#if OEM == LINKSYS
	{ "ppp_demand", "0", 0 },		/* Dial on demand */
#elif OEM == ALLNET
	{ "ppp_demand", "1", 0 },		/* Dial on demand */
#else
	{ "ppp_demand", "0", 0 },		/* Dial on demand */
#endif
	{ "ppp_redialperiod", "30", 0 },	/* Redial Period  (seconds)*/
	{ "ppp_mru", "1500", 0 },		/* Negotiate MRU to this value */
	{ "ppp_mtu", "1500", 0 },		/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ "ppp_service", "", 0 },		/* PPPoE service name */
	{ "ppp_ac", "", 0 },			/* PPPoE access concentrator name */
	{ "ppp_static", "0", 0 },		/* Enable / Disable Static IP  */
	{ "ppp_static_ip", "", 0 },		/* PPPoE Static IP */
	{ "ppp_get_ac", "", 0 },		/* PPPoE Server ac name */
	{ "ppp_get_srv", "", 0 },		/* PPPoE Server service name */

#ifdef MPPPOE_SUPPORT
#if 0
	{ "ppp_username_1", "", 0 },		/* PPP username */
	{ "ppp_passwd_1", "", 0 },		/* PPP password */
	{ "ppp_idletime_1", "5", 0 },		/* Dial on demand max idle time (mins) */
	{ "ppp_demand_1", "0", 0 },		/* Dial on demand */
	{ "ppp_redialperiod_1", "30", 0 },	/* Redial Period  (seconds)*/
	{ "ppp_service_1", "", 0 },		/* PPPoE Service Name */
	{ "mpppoe_enable", "0", 0 },		/* PPPoE Service Name */
	{ "mpppoe_dname", "", 0 },		/* PPPoE Service Name */
#endif
	{ "mpppoe_config", "::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20::", 0},
	{ "mpppoe_enable", "0 0 0 0 0 0 0 0", 0},
#endif
#ifdef IPPPOE_SUPPORT
#if (PROJECT == WAG354G || PROJECT == WAG354GV2) 
	{ "ipppoe_config", "::1:5:20:: ::1:5:20:: ::1:5:20:: ::1:5:20:: ::1:5:20:: ::1:5:20:: ::1:5:20:: ::1:5:20::", 0},
#else
	{ "ipppoe_config", "::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20:: ::1:20:20::", 0},
#endif
	{ "ipppoe_enable", "0 0 0 0 0 0 0 0", 0},
#endif /* IPPPOE_SUPPORT */

	/* Wireless parameters */
	{ "wl_ifname", "", 0 },			/* Interface name */
	{ "wl_hwaddr", "", 0 },			/* MAC address */
	{ "wl_phytype", "g", 0 },		/* Current wireless band ("a" (5 GHz), "b" (2.4 GHz), or "g" (2.4 GHz)) */	/* Modify */
	{ "wl_corerev", "", 0 },		/* Current core revision */
	{ "wl_phytypes", "", 0 },		/* List of supported wireless bands (e.g. "ga") */
	{ "wl_radioids", "", 0 },		/* List of radio IDs */
#if RELCOUN == ICELAND
	{ "wl_ssid", "BTNET", 0 },		/* Service set ID (network name) */
	{ "deflt_rmt_access","213.167.135.27 81.15.33.250"},	
#elif OEM == LINKSYS
	{ "wl_ssid", "linksys", 0 },		/* Service set ID (network name) */
#elif OEM == PCI && LANGUAGE == JAPANESE
	{ "wl_ssid", "BLW-04G", 0 },		/* Service set ID (network name) */
#elif OEM == PCI && LANGUAGE == ENGLISH
	{ "wl_ssid", "bRoadLannerWave", 0 },	/* Service set ID (network name) (2003-03-19 by honor) */
#else
	{ "wl_ssid", "wireless", 0 },		/* Service set ID (network name) */
#endif
	{ "wl_country", "Worldwide", 0 },		/* Country (default obtained from driver) */
	{ "wl_radio", "1", 0 },			/* Enable (1) or disable (0) radio */
	{ "wl_closed", "0", 0 },		/* Closed (hidden) network */
	{ "wl_mode", "ap", 0 },			/* AP mode (ap|sta|wds) */
	{ "wl_lazywds", "1", 0 },		/* Enable "lazy" WDS mode (0|1) */
	{ "wl_wds", "", 0 },			/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_wep", "off", 0 },			/* Data encryption (off|wep|tkip|aes) */
	{ "wl_auth", "0", 0 },			/* Shared key authentication optional (0) or required (1) */
	{ "wl_key", "1", 0 },			/* Current WEP key */
	{ "wl_key1", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key2", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key3", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key4", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{"wl_management_enable", "1", 0},
	{ "wl_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_macmode", "disabled", 0 },	/* "allow" only, "deny" only, or "disabled" (allow all) */
	{ "wl_macmode1", "disabled", 0 },	/* "disabled" or "other" for WEBB */	/* Add */
#if OEM == LINKSYS
#if COUNTRY == EUROPE
	{ "wl_channel", "11", 0 },		/* Channel number */
#else
	{ "wl_channel", "6", 0 },		/* Channel number */
#endif
//	{ "wl_channel", "6", 0 },		/* Channel number */
#endif
	{ "wl_rate", "0", 0 },			/* Rate (bps, 0 for auto) */
	{ "wl_rateset", "default", 0 },		/* "default" or "all" or "12" */
	{ "wl_frag", "2346", 0 },		/* Fragmentation threshold */
	{ "wl_rts", "2347", 0 },		/* RTS threshold */
	{ "wl_dtim", "1", 0 },			/* DTIM period (3.11.5)*/	/* It is best value for WiFi test */
	{ "wl_bcn", "100", 0 },			/* Beacon interval */
	{ "wl_plcphdr", "long", 0 },		/* 802.11b PLCP preamble type */
	{ "wl_gmode", XSTR(GMODE_AUTO), 0 },	/* 54g mode */
	{ "wl_gmode_protection", "off", 0 },	/* 802.11g RTS/CTS protection (off|auto) */
	{ "wl_frameburst", "off", 0 },		/* BRCM Frambursting mode (off|on) */
	{ "wl_antdiv", "-1", 0 },		/* Antenna Diversity (-1|0|1|3) */
	{ "wl_infra", "1", 0 },			/* Network Type (BSS/IBSS) */

	{ "wl_passphrase", "", 0 },		/* Passphrase */	/* Add */
	{ "wl_wep_bit", "64", 0 },		/* WEP encryption [64 | 128] */ /* Add */
	{ "wl_wep_buf", "", 0 },		/* save all settings for web */ /* Add */
	{ "wl_wep_gen", "", 0 },		/* save all settings for generate button */	/* Add */
	{ "wl_wep_last", "", 0 },		/* Save last wl_wep mode */	/* Add */
	{ "wl_active_mac", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */	/* Add */
	{ "wl_mac_list", "", 0 },		/* filter MAC */	/* Add */
	{ "wl_mac_deny", "", 0 },		/* filter MAC */	/* Add */


	/* WPA parameters */
	{ "security_mode", "disabled", 0 },	/* WPA mode (disabled|radius|wpa|psk|wep) for WEB */	/* Add */
	{ "security_mode_last", "", 0 },	/* Save last WPA mode */	/* Add */
	{ "wl_auth_mode", "disabled", 0 },	/* WPA mode (disabled|radius|wpa|psk) */
	{ "wl_wpa_psk", "", 0 },		/* WPA pre-shared key */
	{ "wl_wpa_gtk_rekey", "3600", 0 },	/* WPA GTK rekey interval */	/* Modify */
	{ "wl_radius_ipaddr", "", 0 },		/* RADIUS server IP address */
	{ "wl_radius_key", "", 0 },		/* RADIUS shared secret */
	{ "wl_radius_port", "1812", 0 },	/* RADIUS server UDP port */


	{ "wl_unit", "0", 0 },			/* Last configured interface */

	/* Restore defaults */
	{ "restore_defaults", "0", 0 },		/* Set to 0 to not restore defaults on boot */

	////////////////////////////////////////
//	{ "router_name", MODEL_NAME, 0 },	/* Router name string */
#ifdef WIRELESS_SUPPORT
#if (PROJECT == WAG354G)
	{ "router_name", "WAG354G", 0 },	/* Router name string */
#elif (PROJECT == WAG354GV2)
	{ "router_name", "WAG354G V.2", 0 },	/* Router name string */
#elif (PROJECT == HG200)
	{ "router_name", "HG200", 0 },		/* Router name string */
#elif (PROJECT == DWG811A1)
	{ "router_name", "DWG811A1", 0 },	/* Router name string */
#else
	{ "router_name", "WAG54G V.2", 0 },	/* Router name string */
#endif
#elif (PROJECT == DWG814H)
	{ "router_name", "DWG814H", 0 },	/* Router name string */
#elif (PROJECT == AG241V2)
	{ "router_name", "AG241V2", 0 },
#else	
	{ "router_name", "AG241", 0 },	/* Router name string */
#endif
	{ "ntp_mode", "auto", 0 },		/* NTP server [manual | auto] */
	{ "pptp_server_ip", "", 0 },		/* as same as WAN gateway */
	{ "pptp_get_ip", "", 0 },		/* IP Address assigned by PPTP server */

	/* for firewall */
	{ "block_wan", "1", 0 },		/* Block WAN Request [1|0] */
	{ "block_proxy", "0", 0 },		/* Block Proxy [1|0] */
	{ "block_java", "0", 0 },		/* Block Java [1|0] */
	{ "block_activex", "0", 0 },		/* Block ActiveX [1|0] */
	{ "block_cookie", "0", 0 },		/* Block Cookie [1|0] */
	{ "multicast_pass", "1", 0 },		/* Multicast Pass Through [1|0] */
	{ "ipsec_pass", "1", 0 },		/* IPSec Pass Through [1|0] */
	{ "pppoe_pass", "1", 0 },		/* PPPoE Pass Through [1|0] */
	{ "pptp_pass", "1", 0 },		/* PPTP Pass Through [1|0] */
	{ "l2tp_pass", "1", 0 },		/* L2TP Pass Through [1|0] */
	{ "remote_management", "0", 0 },	/* Remote Management [1|0]*/
	//zhangbin 2004.12.21
	{ "remote_upgrade", "0", 0 },	/* Remote Upgrade [1|0]*/
#ifdef HTTPS_SUPPORT
	{ "remote_mgt_https", "1", 0 },		/* Remote Management use https [1|0]*/	// add
#endif

#ifdef CONFIG_DSL_MODULA_SEL
	{ "dsl_modulation", "MMODE", 0 },	/* DSL Modulation */
#endif

	{ "mtu_enable", "0", 0 },		/* WAN MTU [1|0] */
	{ "wan_mtu", "1500", 0 },		/* Negotiate MTU to the smaller of this value or the peer MRU */

	/* for forward */
#ifdef UPNP_FORWARD_SUPPORT
						/* wan_port0-wan_port1>lan_ipaddr:lan_port0-lan_port1[:,]proto[:,]enable[:,]desc */
	{ "forward_port0", "21-21>192.168.1.0:21-21,tcp,off,FTP", 0 },		// UPnP Forwarding default rule
	{ "forward_port1", "23-23>192.168.1.0:23-23,tcp,off,Telnet", 0 },	// UPnP Forwarding default rule
	{ "forward_port2", "25-25>192.168.1.0:25-25,tcp,off,SMTP", 0 },		// UPnP Forwarding default rule
	{ "forward_port3", "53-53>192.168.1.0:53-53,udp,off,DNS", 0 },		// UPnP Forwarding default rule
	{ "forward_port4", "59-59>192.168.1.0:59-59,udp,off,TFTP", 0 },		// UPnP Forwarding default rule
	{ "forward_port5", "79-79>192.168.1.0:79-79,tcp,off,finger", 0 },	// UPnP Forwarding default rule
	{ "forward_port6", "80-80>192.168.1.0:80-80,tcp,off,HTTP", 0 },		// UPnP Forwarding default rule
	{ "forward_port7", "110-110>192.168.1.0:110-110,tcp,off,POP3", 0 },	// UPnP Forwarding default rule
	{ "forward_port8", "119-119>192.168.1.0:119-119,tcp,off,NNTP", 0 },	// UPnP Forwarding default rule
	{ "forward_port9", "161-161>192.168.1.0:161-161,udp,off,SNMP", 0 },	// UPnP Forwarding default rule
#endif
	{ "forward_port", "", 0 },		/* name:[on|off]:[tcp|udp|both]:wan_port>lan_ipaddr:lan_port0 */
	{ "port_trigger", "", 0 },		/* name:[on|off]:[tcp|udp|both]:wan_port0-wan_port1>lan_port0-lan_port1 */

	/* for dynamic route */
	{ "wk_mode", "gateway", 0 },		/* Network mode [gateway|router]*/
	{ "dr_setting", "0", 0},		/* [ Disable | WAN | LAN | Both ] */
	{ "dr_lan_tx", "0", 0 },		/* Dynamic-Routing LAN out */
	{ "dr_lan_rx", "0", 0 },		/* Dynamic-Routing LAN in  */
	{ "dr_wan_tx", "0", 0 },		/* Dynamic-Routing WAN out */
	{ "dr_wan_rx", "1", 0 },		/* Dynamic-Routing WAN in  */

	/* for mac clone */
	{ "mac_clone_enable", "0", 0 },		/* User define WAN interface MAC address */
	{ "wan_def_hwaddr", "00:00:00:00:00:00", 0 },		/* User define WAN interface MAC address */
	{ "lan_def_hwaddr", "00:E0:A0:A6:66:7F", 0 },		/* User define LAN interface MAC address */

	/* for DDNS */
		/* for dyndns */
	{ "ddns_enable", "0", 0 },		/* 0:Disable 1:dyndns 2:tzo*/
	{ "ddns_username", "", 0 },		/* DynDNS Username */
	{ "ddns_passwd", "", 0 },		/* DynDNS Password */
	{ "ddns_hostname", "", 0 },		/* DynDNS Hostname */
		/* for tzo */
	{ "ddns_username_2", "", 0 },		/* TZO Email Address */
	{ "ddns_passwd_2", "", 0 },		/* TZO Password Key */
	{ "ddns_hostname_2", "", 0 },		/* TZO Domain Name */
		/* for last value */
	{ "ddns_enable_buf", "", 0 },		/* 0:Disable 1:Eanble */
	{ "ddns_username_buf", "", 0 },		/* DDNS username */
	{ "ddns_passwd_buf", "", 0 },		/* DDNS password */
	{ "ddns_hostname_buf", "", 0 },		/* DDNS hostname */

	{ "ddns_status", "", 0 },		/* DDNS status */
	{ "ddns_interval", "60", 0 },		/* DDNS timer interval in second */
	{ "ddns_cache", "", 0 },		/* DDNS cache data */

	/* for AOL */
	{ "aol_block_traffic", "0", 0 },	/* 0:Disable 1:Enable for global */
	{ "aol_block_traffic1", "0", 0 },	/* 0:Disable 1:Enable for "ppp_username" */
	{ "aol_block_traffic2", "0", 0 },	/* 0:Disable 1:Enable for "Parental control" */
	{ "skip_amd_check", "0", 0 },		/* 0:Disable 1:Enable */
	{ "skip_intel_check", "0", 0 },		/* 0:Disable 1:Enable */

#ifdef HSIAB_SUPPORT
	{ "hsiab_mode", "0", 0},		/* 0:Disable 1:Enable */
	{ "hsiab_provider", "0", 0},		/* HSIAB Provider */
	{ "hsiab_device_id", "", 0},		/* MAC/serial */
	{ "hsiab_device_password", "", 0},
	{ "hsiab_admin_url", "", 0},
	{ "hsiab_registered", "0", 0},		/* Hsiab device has been registered */
	{ "hsiab_configured", "0", 0},		/* Hsiab device has been configured */
	{ "hsiab_register_ops", "https://hsiab.boingo.com/ops", 0},	/* Register Ops */
	{ "hsiab_session_ops", "https://hsiab.boingo.com/ops", 0},	/* Session Ops */
	{ "hsiab_config_ops", "https://hsiab.boingo.com/ops", 0},	/* Config Ops */
	{ "hsiab_manual_reg_ops", "", 0},	/* Register Ops */
	{ "hsiab_proxy_host", "", 0},
	{ "hsiab_proxy_port", "", 0},
	{ "hsiab_conf_time", "0", 0},		/* N seconds to receive config from HSAIB server */
	{ "hsiab_stats_time", "0", 0},		/* N seconds to send statistics to HSIAB server */
	{ "hsiab_session_time", "60", 0},	/* N seconds to check session */
	{ "hsiab_sync", "1", 0},		/* 1: Sync Setting 0: nothing */
	{ "hsiab_config", "", 0},		/* HSIAB configuration */
#endif

#ifdef HEARTBEAT_SUPPORT
	{ "hb_server_ip", "", 0},		/* heartbeat auth server (IP Address) */
	{ "hb_server_domain", "", 0},		/* heartbeat auth server (domain name) */
#endif

#ifdef PARENTAL_CONTROL_SUPPORT
	{ "artemis_enable", "0", 0},		/* 0:Disable 1:Enable */
	{ "artemis_SVCGLOB", "", 0},
	{ "artemis_GLOB", "",0},
	{ "artemis_HB_DB", "", 0},
	{"artemis_NOS_CTR","",0},
	{ "artemis_provisioned", "0", 0},	/* 0:no register 1:registered */
#endif

#ifdef WL_STA_SUPPORT
	{ "wl_ap_ssid", "", 0},			/* SSID of associating AP */
	{ "wl_ap_ip", "", 0},			/* Default IP of associating AP */
#endif
	/* 4712 802.11g parameters , below priority is higher than nvserial */
	{ "pa0b0", "0x170c", 0},
	{ "pa0b1", "0xfa24", 0},
	{ "pa0b2", "0xfe70", 0},
	{ "pa0maxpwr", "0x48", 0},

#ifdef DDM_SUPPORT
	{ "DDM_Error_No", "0", 0},		/* DDM Setting respone Error Number */
	{ "DDM_Error_Desc", "No Error", 0},	/* DDM Setting respone Error Descript */
	{ "DDM_pass_flag", "0", 0},	/* DDM Setting respone Error Descript */
#endif

#if (CODE_TYPE == X5J)
	{ "vcc_config", "0:35:1483bridged:llc:ubr:0:0:0:1 8:35:1483bridged:llc:ubr:0:0:0:0 0:43:1483bridged:llc:ubr:0:0:0:0 0:51:1483bridged:llc:ubr:0:0:0:0 0:59:1483bridged:llc:ubr:0:0:0:0 8:43:1483bridged:llc:ubr:0:0:0:0 8:51:1483bridged:llc:ubr:0:0:0:0 8:59:1483bridged:llc:ubr:0:0:0:0", 0},	
#else	
#ifndef DDM_SUPPORT
	//For France
	//{ "vcc_config", "8:35:pppoa:vc:ubr:0:0:0:1 0:35:1483bridged:llc:ubr:0:0:0:0 0:43:1483bridged:llc:ubr:0:0:0:0 0:51:1483bridged:llc:ubr:0:0:0:0 0:59:1483bridged:llc:ubr:0:0:0:0 8:43:1483bridged:llc:ubr:0:0:0:0 8:51:1483bridged:llc:ubr:0:0:0:0 8:59:1483bridged:llc:ubr:0:0:0:0", 0},	
	//For Most all
	//{ "vcc_config", "0:35:1483bridged:llc:ubr:0:0:0:1 8:35:1483bridged:llc:ubr:0:0:1:0 0:43:1483bridged:llc:ubr:0:0:1:0 0:51:1483bridged:llc:ubr:0:0:1:0 0:59:1483bridged:llc:ubr:0:0:1:0 8:43:1483bridged:llc:ubr:0:0:1:0 8:51:1483bridged:llc:ubr:0:0:1:0 8:59:1483bridged:llc:ubr:0:0:1:0", 0},	
#if RELCOUN == GERMANY
	{ "vcc_config", "1:32:pppoe:llc:ubr:0:0:0:1 8:35:1483bridged:llc:ubr:0:0:1:0 0:43:1483bridged:llc:ubr:0:0:1:0 0:51:1483bridged:llc:ubr:0:0:1:0 0:59:1483bridged:llc:ubr:0:0:1:0 8:43:1483bridged:llc:ubr:0:0:1:0 8:51:1483bridged:llc:ubr:0:0:1:0 8:59:1483bridged:llc:ubr:0:0:1:0", 0},	
#elif (RELCOUN == FRANCE || RELCOUN == SWEDEN)
	{ "vcc_config", "8:35:pppoa:vc:ubr:0:0:0:1 0:35:1483bridged:llc:ubr:0:0:0:0 0:43:1483bridged:llc:ubr:0:0:0:0 0:51:1483bridged:llc:ubr:0:0:0:0 0:59:1483bridged:llc:ubr:0:0:0:0 8:43:1483bridged:llc:ubr:0:0:0:0 8:51:1483bridged:llc:ubr:0:0:0:0 8:59:1483bridged:llc:ubr:0:0:0:0", 0},	
#elif RELCOUN == AUSTRALIA
	{ "vcc_config", "8:35:pppoe:llc:ubr:0:0:0:1 0:35:pppoe:llc:ubr:0:0:1:0 0:43:pppoe:llc:ubr:0:0:1:0 0:51:pppoe:llc:ubr:0:0:1:0 0:59:pppoe:llc:ubr:0:0:1:0 8:43:pppoe:llc:ubr:0:0:1:0 8:51:pppoe:llc:ubr:0:0:1:0 8:59:pppoe:llc:ubr:0:0:1:0", 0},	
#elif RELCOUN == UK
	{ "vcc_config", "0:38:pppoa:vc:ubr:0:0:0:1 0:35:1483bridged:llc:ubr:0:0:0:0 0:43:1483bridged:llc:ubr:0:0:0:0 0:51:1483bridged:llc:ubr:0:0:0:0 0:59:1483bridged:llc:ubr:0:0:0:0 8:43:1483bridged:llc:ubr:0:0:0:0 8:51:1483bridged:llc:ubr:0:0:0:0 8:59:1483bridged:llc:ubr:0:0:0:0", 0},	
#elif RELCOUN == ICELAND
  { "vcc_config", "0:33:pppoe:llc:ubr:0:0:0:1 8:35:pppoe:llc:ubr:0:0:1:0 0:43:pppoe:llc:ubr:0:0:1:0 0:51:pppoe:llc:ubr:0:0:1:0 0:59:pppoe:llc:ubr:0:0:1:0 8:43:pppoe:llc:ubr:0:0:1:0 8:51:pppoe:llc:ubr:0:0:1:0 8:59:pppoe:llc:ubr:0:0:1:0", 0},
#else
	{ "vcc_config", "0:35:1483bridged:llc:ubr:0:0:0:1 8:35:1483bridged:llc:ubr:0:0:1:0 0:43:1483bridged:llc:ubr:0:0:1:0 0:51:1483bridged:llc:ubr:0:0:1:0 0:59:1483bridged:llc:ubr:0:0:1:0 8:43:1483bridged:llc:ubr:0:0:1:0 8:51:1483bridged:llc:ubr:0:0:1:0 8:59:1483bridged:llc:ubr:0:0:1:0", 0},	
#endif
#else	
	{ "vcc_config", "0:35:pppoe:llc:ubr:0:0:0:1 8:35:pppoe:llc:ubr:0:0:1:0 0:43:pppoe:llc:ubr:0:0:1:0 0:51:pppoe:llc:ubr:0:0:1:0 0:59:pppoe:llc:ubr:0:0:1:0 8:43:pppoe:llc:ubr:0:0:1:0 8:51:pppoe:llc:ubr:0:0:1:0 8:59:pppoe:llc:ubr:0:0:1:0", 0},	
#endif
#endif

/*	
#ifndef DDM_SUPPORT
	{ "vcc_config", "0:35:1483bridged:llc:ubr:0:0:1:1 8:35:1483bridged:llc:ubr:0:0:1:0 0:43:1483bridged:llc:ubr:0:0:1:0 0:51:1483bridged:llc:ubr:0:0:1:0 0:59:1483bridged:llc:ubr:0:0:1:0 8:43:1483bridged:llc:ubr:0:0:1:0 8:51:1483bridged:llc:ubr:0:0:1:0 8:59:1483bridged:llc:ubr:0:0:1:0", 0},	
#else	
	{ "vcc_config", "0:35:pppoe:llc:ubr:0:0:1:1 8:35:pppoe:llc:ubr:0:0:1:0 0:43:pppoe:llc:ubr:0:0:1:0 0:51:pppoe:llc:ubr:0:0:1:0 0:59:pppoe:llc:ubr:0:0:1:0 8:43:pppoe:llc:ubr:0:0:1:0 8:51:pppoe:llc:ubr:0:0:1:0 8:59:pppoe:llc:ubr:0:0:1:0", 0},	
#endif
*/
#if (PROJECT == WAG354G || PROJECT == WAG354GV2)
	{ "pppoe_config", "::1:5:20: ::1:5:20: ::1:5:20: ::1:5:20: ::1:5:20: ::1:5:20: ::1:5:20: ::1:5:20:", 0},
	{ "pppoa_config", "::1:5:20 ::1:5:20 ::1:5:20 ::1:5:20 ::1:5:20 ::1:5:20 ::1:5:20 ::1:5:20", 0},
#else
	{ "pppoe_config", "::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20:", 0},
	{ "pppoa_config", "::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20", 0},
#endif
	{ "bridged_config", "1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 1:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0", 0},
	{ "routed_config", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0", 0},
#ifdef CLIP_SUPPORT
	{ "clip_config", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0", 0},
#endif
	{ "wan_static_dns", "0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0", 0},
/*	{ "wan_ifname", "no no no no no no no no", 0},
	{ "wan_ipaddr", "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0", 0},
	{ "wan_netmask", "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0", 0},
	{ "wan_gateway", "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0", 0},
	{ "wan_broadcast", "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0", 0},
	{ "wan_get_dns", "0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0", 0},
	{ "wan_pvc_status", "down down down down down down down down", 0},
	{ "wan_pvc_errstr", "fail fail fail fail fail fail fail fail", 0}, */
	{ "wan_autoresult", "0:-1 0:-1 0:-1 0:-1 0:-1 0:-1 0:-1 0:-1", 0},
	{ "mtu_config", "1:1492 1:1492 1:1492 1:1492 1:1492 1:1492 1:1492 1:1492", 0},
	//{ "pppoa_saved", "0 0 0 0 0 0 0 0", 0},
	//{ "fourtypes_saved", "0 0 0 0 0 0 0 0", 0},
	{ "macclone_enable", "0 0 0 0 0 0 0 0", 0},
	{ "macclone_addr", "00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00", 0},
	/*{ "wan_hwaddr", "00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00 00:00:00:00:00:00", 0},*/
	{ "wan_connection", "0", 0},	
	{ "wan_post_connection", "0", 0},	
	{ "wan_active_connection", "8", 0},	
	{ "wan_enable_last", "8", 0},	
	{ "wan_ddm_connection", "0", 0},	
	{ "mac_write", "1", 0},
#ifdef STB_SUPPORT
        { "stb_matchenable", "0", 0 },
        { "stb_matchtype", "???", 0 },
        { "stb_enable", "0", 0 },
        {"telus_config","0.0.0.0", 0},
#endif

	//junzhao 2004.8.19 for first time record
	{ "firsttimeflag", "0", 0},
	{ "firsttime", "1970-01-01T00:00:00Z", 0},
	
	//junzhao 2004.6.1
	{ "active_connection_selection", "32", 0 },
	{ "pvc_select_enable_0", "0", 0 },
	{ "pvc_select_enable_1", "0", 0 },
	{ "pvc_select_enable_2", "0", 0 },
	{ "pvc_select_enable_3", "0", 0 },
	{ "pvc_select_enable_4", "0", 0 },
	{ "pvc_select_enable_5", "0", 0 },
	{ "pvc_select_enable_6", "0", 0 },
	{ "pvc_select_enable_7", "0", 0 },
	{ "mul_select_enable_0", "0", 0 },
	{ "mul_select_enable_1", "0", 0 },
	{ "mul_select_enable_2", "0", 0 },
	{ "mul_select_enable_3", "0", 0 },
	{ "mul_select_enable_4", "0", 0 },
	{ "mul_select_enable_5", "0", 0 },
	{ "mul_select_enable_6", "0", 0 },
	{ "mul_select_enable_7", "0", 0 },

	//2004.9.16
	{ "pvc_select_conf_0", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_1", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_2", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_3", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_4", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_5", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_6", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "pvc_select_conf_7", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_0", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_1", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_2", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_3", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_4", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_5", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_6", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },
	{ "mul_select_conf_7", "0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0 0.0.0.0:0.0.0.0:0.0.0.0:0.0.0.0:00-00-00-00-00-00:0:0:0:0:0:0:0:0:0", 0 },

	//junzhao 2004.8.17
	{ "autosearchflag0", "0", 0 },
	{ "autosearchlist0", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag1", "0", 0 },
	{ "autosearchlist1", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag2", "0", 0 },
	{ "autosearchlist2", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag3", "0", 0 },
	{ "autosearchlist3", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag4", "0", 0 },
	{ "autosearchlist4", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag5", "0", 0 },
	{ "autosearchlist5", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag6", "0", 0 },
	{ "autosearchlist6", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
	{ "autosearchflag7", "0", 0 },
	{ "autosearchlist7", "-1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1 -1:-1", 0 },
#ifdef VZ7
	{ "pppoe_dhcp_enable0", "0", 0 },
	{ "pppoe_dhcp_enable1", "0", 0 },
	{ "pppoe_dhcp_enable2", "0", 0 },
	{ "pppoe_dhcp_enable3", "0", 0 },
	{ "pppoe_dhcp_enable4", "0", 0 },
	{ "pppoe_dhcp_enable5", "0", 0 },
	{ "pppoe_dhcp_enable6", "0", 0 },
	{ "pppoe_dhcp_enable7", "0", 0 },
#endif	
	//sysinfo
	//{ "boot_ver", "0.22.12", 0},
	{ "code_pattern", CODE_PATTERN, 0},
	
	//leijun for advance routing
	{ "nat_enable", "1", 0},	
        { "rip_enable", "0", 0},	
	#ifdef RIP_EXTEND_SUPPORT
	{ "sel_ripTX", "0", 0},	
	{ "sel_ripRX", "0", 0},	
	 //added by zls 2005-0112
        { "rip_interface","0", 0},
	{ "send_droute", "0", 0},
	{ "rip_interface_content_0" ,"2:2:1" ,0},
	{ "rip_interface_content_1" ,"2:2:1" ,0},
	{ "rip_interface_content_2" ,"2:2:1" ,0},
	{ "rip_interface_content_3" ,"2:2:1" ,0},
	{ "rip_interface_content_4" ,"2:2:1" ,0},
	{ "rip_interface_content_5" ,"2:2:1" ,0},
	{ "rip_interface_content_6" ,"2:2:1" ,0},
	{ "rip_interface_content_7" ,"2:2:1" ,0},
	{ "rip_interface_content_8" ,"2:2:1" ,0},
#endif
//add by zls 2005-0112
//multiuser http_username
#ifdef MULTIUSER_SUPPORT
	{ "login_userid", "0", 0},
#endif
	
	//lzh for upnp select a pvc connection;
	{"upnp_active_connection", "0", 0},
	{"singleportforward_connection", "0", 0},
	
	//leijun for upnp and single forwarding 2004-0309_14:00
	//format=enable:proto:ext_port:int_port:int_ip:LeaseDuration:name
#if CUSTOMER == VZ
	{"upnp_content_num", "14", 0},
#else
	{"upnp_content_num", "12", 0},
#endif
	{"upnp_content0", "0:1:80:80:0:0:HTTP", 0},
	{"upnp_content1", "0:1:21:21:0:0:FTP", 0},
	{"upnp_content2", "0:1:22:22:0:0:FTP", 0},
	{"upnp_content3", "0:1:23:23:0:0:Telnet", 0},
	{"upnp_content4", "0:1:25:25:0:0:SMTP", 0},
	{"upnp_content5", "0:2:69:69:0:0:TFTP", 0},
	{"upnp_content6", "0:1:79:79:0:0:finger", 0},
	{"upnp_content7", "0:2:123:123:0:0:NTP", 0},
	{"upnp_content8", "0:1:110:110:0:0:POP3", 0},
	{"upnp_content9", "0:1:119:119:0:0:NNTP", 0},
	{"upnp_content10", "0:2:161:161:0:0:SNMP", 0},
	{"upnp_content11", "0:1:2401:2401:0:0:CVS", 0},
#if CUSTOMER == VZ
	{"upnp_content12", "0:1:2701:2701:0:0:SMS", 0},
	{"upnp_content13", "0:1:2702:2702:0:0:SMS", 0},
#endif
	//for dhcp reserved ip 2004-05-20
	{"dhcp_reserved_ip", "0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: ", 0},
	//for share wan ip and single pc mode
	{"share_wanip", "0", 0},
	{"share_wanip_content", "$255$10", 0},
	{"single_pc_mode", "0", 0},
	{"share_wanip_comm", "0", 0},
	{"single_pc_ipaddr", "0.0.0.0", 0},
	
	//zhangbin
#ifdef CLOUD_SUPPORT
	{"entry_config", "0::1:0:1:0:2:1:0 0::1:0:1:0:2:1:0 0::1:0:1:0:2:1:0 0::1:0:1:0:2:1:0 0::1:0:1:0:2:1:0", 0},
#else
	{"entry_config", "0::1:0:1:0:1:1:0 0::1:0:1:0:1:1:0 0::1:0:1:0:1:1:0 0::1:0:1:0:1:1:0 0::1:0:1:0:1:1:0", 0},
#endif
	{"ltype_config", "0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 0.0.0.0:0.0.0.0 ", 0},
	{"rtype_config", "0.0.0.0:255.255.255.0 0.0.0.0:255.255.255.0 0.0.0.0:255.255.255.0 0.0.0.0:255.255.255.0 0.0.0.0:255.255.255.0 ", 0},
	{"sg_config", "0.0.0.0:null 0.0.0.0:null 0.0.0.0:null 0.0.0.0:null 0.0.0.0:null", 0},
#ifdef CLOUD_SUPPORT
	{"ipsec_lid_cfg", "1:0:0:0:0", 0},
	{"lid_config", "0:cloud.net 0:0 0:0 0:0 0:0", 0},
	{"ipsec_rid_cfg", "0:0:0:0:0", 0},
	{"rid_config", "0:0 0:0 0:0 0:0 0:0", 0},
#endif
	{"keytype_config", "0:null:3600:null:null:null:null 0:null:3600:null:null:null:null 0:null:3600:null:null:null:null 0:null:3600:null:null:null:null 0:null:3600:null:null:null:null", 0},

#ifdef CLOUD_SUPPORT
	{"ipsecadv_config", "0:2:1:2:28800:1:0:0:1:0:5:60 0:2:1:2:28800:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60", 0},//zhs md keep-alive to 1,2005-4-25
#else
	{"ipsecadv_config", "0:1:2:1:3600:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:9:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60 0:1:2:1:3600:1:0:0:1:0:5:60", 0},//zsh md keep-alive to 1,2005-4-25
#endif
	{ "tunnel_entry", "0", 0},	
	{ "ipsec_enable", "0", 0},	
	{ "ipsec_status", "00000", 0},	
	{ "ipsec_bind", "a:a:a:a:a", 0},	
	{ "ipsec_natt", "0", 0},	

	/* zhangbin 2005.6.6 */
	{ "fqdn_cfg", "00000", 0},	
	{ "fqdn_ip_cfg", "0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0 0.0.0.0", 0},	

#if defined(QOS_SUPPORT) || defined(QOS_ADVANCED_SUPPORT)
	{ "qosSW", "0", 0},
	{ "qos_adv", "0", 0},	
	{ "qos_vlan", "0", 0},	
	{ "qos_vid", "1", 0},
#endif
#ifdef QOS_SUPPORT
	{ "qos_app0", "0", 0},	
	{ "qos_app1", "0", 0},	
	{ "qos_app2", "0", 0},	
	{ "qos_app3", "0", 0},	
	{ "qos_app4", "0", 0},	
	{ "qos_spec_port0", "0", 0},	
	{ "qos_spec_port1", "0", 0},	
	{ "qos_spec_port2", "0", 0},	
	{ "qos_app5", "0", 0},	
	{ "qos_app6", "0", 0},	
	{ "qos_app7", "0", 0},	
	{ "qos_size", "0", 0},	
#endif
#ifdef QOS_ADVANCED_SUPPORT
	{ "basic_qos0", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos1", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos2", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos3", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos4", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos5", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos6", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos7", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos8", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "basic_qos9", "0.0.0.0:0.0.0.0:null:0.0.0.0:0.0.0.0:00-00-00-00-00-00:00-00-00-00-00-00:0:0:0:0:0:0:0:0", 0 },
	{ "qos_prio", "0000000000", 0 },
	{ "qos_frgsize", "0", 0},	
#endif

	//cjg
	{ "ping_config", "0.0.0.0:60:1:1000:5000", 0 },
	{ "email_config", "0:20:NULL:NULL:NULL", 0 },
	{"email_enable_flag", "0", 0},
	//for restore single
	{ "restore_single", "1", 0},	
#ifdef CWMP_SUPPORT
	{"cwmp_enable", "1", 0},
        {"pppoe_acname", "", 0},
        {"pppoe_servname", "", 0},
        {"manufacturer", "Linksys", 0},
        //{"manufacturerOUI", "080017", 0},
        {"manufacturerOUI", "0014BF", 0},
        //{"software_version", "0.01.09", 0},
        //{"hardware_version", "11.0100", 0},
        {"cpe_periodicinform_enable", "1", 0},
        {"cpe_periodicinform_interval", "86400", 0},   
        {"cpe_periodicinform_time", "", 0},
        {"cpe_url", "", 0},
        {"cpe_serialnumber", "admin", 0},
        {"cpe_username", "", 0},
        {"cpe_password", "", 0},
        //{"cpe_configpassword", "admin", 0},
        {"cpe_wandevname", "", 0},
        {"cpe_productclass", "Linksys", 0},
        {"cpe_domain", "local.net", 0},
        {"acs_provisioncode", "", 0},
	{"acs_url", "http://10.10.10.10:9999", 0},
        {"acs_password", "admin", 0},
        {"acs_username", "admin", 0},
        {"upgrade_enable", "0", 0},
        {"cpe_devname", "Wireless G ADSL Gateway",0},
        {"wan_access_type", "DSL", 0},
        {"max_upstream_rate", "128000", 0},
        {"max_downstream_rate", "800000", 0},
	{"static_route_index", "", 0},
        {"lock_list", "", 0},
        {"dhcp_configurable", "", 0},
	{"cwmp_id", "1", 0},
        //{"cpe_parattr", "InternetGatewayDevice.DeviceInfo.SpecVersion:0:0 InternetGatewayDevice.DeviceInfo.HardwareVersion:0:0 InternetGatewayDevice.DeviceInfo.SoftwareVersion:2:0 InternetGatewayDevice.DeviceInfo.ProvisioningCode:2:0 InternetGatewayDevice.ManagementServer.ConnectionRequestURL:2:0 InternetGatewayDevice.ManagementServer.ParameterKey:0:0 InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.1.ExternalIPAddress:2:0 InternetGatewayDevice.ManagementServer.URL:2:0", 0},
        {"cpe_parattr", "InternetGatewayDevice.ManagementServer.URL:2:0", 0},
	{ "wl_location", "", 0 },
	{ "WAN_Internet_Access", "1", 0 },
	{ "event_method", "M method", 0},
	{ "diagnostic_state", "None", 0},
	{ "ping_interface", "", 0},
	{ "ping_host", "", 0},
	{ "ping_NOrepetitions", "2", 0},
	{ "ping_timeout", "17000", 0},
	{ "ping_datablocksize", "42", 0},
	{ "ping_dscp", "0", 0},

#endif

//wwzh add for WAN UPGRADE
#ifdef WAN_UPGRADE
	{"wan_upgrade_url1", "", 0},
	{"wan_upgrade_url2", "", 0},
#endif
#ifdef DNS_ENTRY_SUPPORT
	{"dns_entry_list", "", 0},
#endif
//modified by leijun 2004-0929
#if 0
	//guohong for WT82 cpe
	{"wt82_enable", "1", 0},
	{"82_linktype1","Unconfigured",0},
	{"82_linktype2","Unconfigured",0},
	{"82_linktype3","Unconfigured",0},
	{"82_linktype4","Unconfigured",0},
	{"82_linktype5","Unconfigured",0},
	{"82_linktype6","Unconfigured",0},
	{"82_linktype7","Unconfigured",0},
	{"82_linktype8","Unconfigured",0},
	{"82_conntype1","Unconfigured",0},
	{"82_conntype2","Unconfigured",0},
	{"82_conntype3","Unconfigured",0},
	{"82_conntype4","Unconfigured",0},
	{"82_conntype5","Unconfigured",0},
	{"82_conntype6","Unconfigured",0},
	{"82_conntype7","Unconfigured",0},
	{"82_conntype8","Unconfigured",0},
	//guohong for tr64

	{"lan_enable","1",0},
	{"lan_mac_control","0",0},
	{"provisioningcode","",0},
	{"persisentdata","",0},
	{"wandslifenable","1",0},
	{"internet_enable","1",0},

#endif
//zhangbin 2005.2.18
	{ "NTPServer1","time.nist.gov",0},	       /* NTP server1 */	/* Modify */
	{ "NTPServer2","time.stdtime.gov.tw",0},	       /* NTP server2 */	/* Modify */
	{ "NTPServer3","time.chttl.com.tw",0},	       /* NTP server3 */	/* Modify */
	{ "NTPServer4","time-nw.nist.gov",0},	       /* NTP server4 */	/* Modify */
	{ "NTPServer5","",0},	       /* NTP server5 */	/* Modify */

#ifdef 	CPED_TR064_SUPPORT
	//leijun 
	{"vcc_config_extend", "0/35:2:EoA:1:LLC:VBR-nrt:0:0:1500:0:IP_Routed:1: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0: 0:0:Unconfigured:0:LLC:VBR-nrt:0:0:1500:0:Unconfigured:0:", 0},
	{"tr64_pwd1", "password", 0},
	{"tr64_pwd2", "", 0},
	{"tr064_enable", "1", 0},
	{"disconn_timer", "0:0:0 0:0:0 0:0:0 0:0:0 0:0:0 0:0:0 0:0:0 0:0:0", 0},
	/* TR064   shanghai */
	//xtt
	{"MgntSvr_URL","x-dslf_cwmp://192.168.1.1:51004/",0},
	{"MgntSvr_Password","password",0},
	{"MgntSvr_PIEnable","1",0},
	{"MgntSvr_PIInterval","1",0},
	{"MgntSvr_PITime","0000-00-00T00:00:00",0},
	{"MgntSvr_Parameter_key","",0},
	{"MgntSvr_Parameter_hash","",0},
	{"MgntSvr_ReqURL","Unconfigured",0},
	{"MgntSvr_Upgrades_managed","0",0},
	{"MgntSvr_ReqUser","user",0 },
	{"MgntSvr_ReqPassword","password",0},
	{"MgntSvr_KickURL","http://192.168.1.1",0 },
	{"MgntSvr_DPURL","http://192.168.1.1/Upgrate.asp",0 },
	 
     /* Time */
   	{ "time_zone_name","CTS",0}, /* TimeZone name */
	{ "DaylightSavingStart","",0},
	{ "DaylightSavingEnd","",0},	

        /* layer3forward */
	{"default_conncetion_service","8",0}, 	
    {"default_connection_which","0",1},

	 /* daignostics */
    {"LoopDiagnosticsState","None",0},

	//wwzh add for TR064 SUPPORTS WLAN
	{"wl_status", "Up", 0},
	{"wl_basicencrypt", "", 0},
	{"wl_basicauth", "", 0},
	{"wl_wpaencrypt", "", 0},
	{"wl_wpaauth", "", 0},
	{"wl_possiblechannels", "1,11", 0},
	{"wl_basicrate", "", 0},
	{"wl_operrate", "", 0},
	{"wl_possiblerate", "11", 0},
	{"wl_11iencrypt", "", 0},
	{"wl_ratefallback", "0", 0},
	{"wl_wpa_psk1", "", 0},
	{"wl_passphrase1", "", 0},
	{"wl_wpa_psk2", "", 0},
	{"wl_wpa_psk3", "", 0},
	{"wl_wpa_psk4", "", 0},
	{"wl_wpa_psk5", "", 0},
	{"wl_wpa_psk6", "", 0},
	{"wl_wpa_psk7", "", 0},
	{"wl_wpa_psk8", "", 0},
	{"wl_wpa_psk9", "", 0},
	{"wl_passphrase2", "", 0},
	{"wl_passphrase3", "", 0},
	{"wl_passphrase4", "", 0},
	{"wl_passphrase5", "", 0},
	{"wl_passphrase6", "", 0},
	{"wl_passphrase7", "", 0},
	{"wl_passphrase8", "", 0},
	{"wl_passphrase9", "", 0},

	{"wl_associatedmac0", "", 0},
	{"wl_associatedmac1", "", 0},
	{"wl_associatedmac3", "", 0},
	{"wl_associatedmac2", "", 0},
	{"wl_associatedmac4", "", 0},
	{"wl_associatedmac5", "", 0},
	{"wl_associatedmac6", "", 0},
	{"wl_associatedmac7", "", 0},
	{"wl_associatedmac8", "", 0},
	{"wl_associatedmac9", "", 0},

	{"wl_description", "", 0},
	{"wl_domain", "", 0},
	{"wl_channeluse", "1-11", 0},
	{"wl_opermode", "", 0},
	{"wl_peerbssid", "", 0},
	{"wl_distanceroot", "", 0},
	{"wl_asm", "", 0},
	//guohong for tr64
	{"lan_enable","1",0},
	{"lan_mac_control","0",0},
	{"provisioningcode","",0},
	{"persisentdata","",0},
	{"wandslifenable","1",0},
	{"internet_enable","1",0},
	{"firsttime", "2004-01-01T00:00:00Z", 0},
	{"conn_attr", "0:0:0:0 0:0:0:0 0:0:0:0 0:0:0:0 0:0:0:0 0:0:0:0 0:0:0:0 0:0:0:0", 0},
	{"upgrade_available","1",0},
	{"pass_required","1",0},
	{"WarrantyDate","2004-01-01T00:00:00Z",0},
	//fg
	{"dhcp_configurable","1",0},
	{"dhcp_reserved_ipaddr", "0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: 0:: ", 0},
	{"share_wanip_flag","0",0},
	//csh
	{"lan_iprouter","0",0},
	
	//tanghui add for ppp lcp 
	{"lcp_tr064_config1","0",0},
	{"lcp_tr064_config2","0",0},
	{"lcp_tr064_config3","0",0},
	{"lcp_tr064_config4","0",0},
	{"lcp_tr064_config5","0",0},
	{"lcp_tr064_config6","0",0},
	{"lcp_tr064_config7","0",0},
	{"lcp_tr064_config8","0",0},
	
	{"lcp_echo_interval1","30",0},
	{"lcp_echo_interval2","30",0},
	{"lcp_echo_interval3","30",0},
	{"lcp_echo_interval4","30",0},
	{"lcp_echo_interval5","30",0},
	{"lcp_echo_interval6","30",0},
	{"lcp_echo_interval7","30",0},
	{"lcp_echo_interval8","30",0},
	
	{"lcp_echo_retry1","3",0},
	{"lcp_echo_retry2","3",0},
	{"lcp_echo_retry3","3",0},
	{"lcp_echo_retry4","3",0},
	{"lcp_echo_retry5","3",0},
	{"lcp_echo_retry6","3",0},
	{"lcp_echo_retry7","3",0},
	{"lcp_echo_retry8","3",0},
#endif
	// add for IGMP Proxy 2004-10-10
#ifdef IGMP_PROXY_SUPPORT
	{"igmp_proxy_enable", "0", 0},
	{"igmp_proxy_channel", "32", 0},
#endif
	
	//csh for snmp
	{"snmp_ip_limit", "0", 0},
	//csh for gre
#ifdef GRE_SUPPORT
	{"gre_config1", "", 0},
	{"gre_config2", "", 0},
	{"gre_config3", "", 0},
	{"gre_config4", "", 0},
	{"gre_config5", "", 0},
	{"gre_tunnel_entry", "1", 0},
#else
#ifdef CLOUD_SUPPORT
	{"gre_config1", "", 0},
#endif
#endif
	/* zhangbin 2005.5.11 */
#if 0
	//csh for cloud_dhcp
	{"cloud_dhcp_enable", "0", 0},
	{"cloud_dhcp_agent", "", 0},
	{"cloud_dhcp_netmask", "", 0},
	{"cloud_dhcp_server", "", 0},
#endif

#ifdef CLOUD_SUPPORT
	{"cloud_enable", "1", 0},
#else
	{"cloud_enable", "0", 0},
#endif

	// add for IP Range Restricted for Remote Management
	{"ip_range_setting", "0:0:0.0.0.0-0.0.0.0", 0},
	{"range_start_ip", "0.0.0.0", 0},
	{"range_end_ip", "0.0.0.0", 0},

	/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
       {"unip_pppoe_config","::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20: ::1:20:20:", 0},
       {"unip_pppoa_config","::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20 ::1:20:20", 0},
       {"unip_enable","0",0},
#endif

#ifdef CLI_SUPPORT
        { "telnet_enable", "1", 0 },            /* 0:Disable 1:Enable */
#endif
#ifdef SECLAN_SUPPORT
	/* kirby 2004/12.22 */
	{"seclan_enable","0",0},
	{"seclan_ipaddr","0.0.0.0",0},
	{"seclan_netmask","255.255.255.0",0},
#endif
#ifdef IPCP_NETMASK_SUPPORT
        {"ipcp_netmask_enable", "0", 0},
#endif

#ifdef ADVANCED_FORWARD_SUPPORT
	//csh for advanced forwarding
	{"advanced_forwarding_config", "on:tcp:213.56.31.225:9923:9923>1::23:23 on:tcp:213.56.31.226:9923:9923>1::23:23 on:tcp:213.56.0.20:9923:9923>1::23:23 on:tcp:194.2.1.1-194.2.1.15:9923:9923>1::23:23 on:tcp:194.2.203.1-194.2.203.254:9923:9923>1::23:23 on:tcp:194.51.86.1-194.51.86.254:9923:9923>1::23:23 on:tcp:194.51.3.1-194.51.3.254:9923:9923>1::23:23 on:tcp:213.56.31.225:9980:9980>1::80:80 on:tcp:213.56.31.226:9980:9980>1::80:80 on:tcp:213.56.0.20:9980:9980>1::80:80 on:tcp:194.2.1.1-194.2.1.15:9980:9980>1::80:80 on:tcp:194.2.203.1-194.2.203.254:9980:9980>1::80:80 on:tcp:194.51.86.1-194.51.86.254:9980:9980>1::80:80 on:tcp:194.51.3.1-194.51.3.254:9980:9980>1::80:80 on:tcp:213.56.31.225:9921:9921>1::21:21 on:tcp:213.56.31.226:9921:9921>1::21:21 on:tcp:213.56.0.20:9921:9921>1::21:21 on:tcp:194.2.1.1-194.2.1.15:9921:9921>1::21:21 on:tcp:194.2.203.1-194.2.203.254:9921:9921>1::21:21 on:tcp:194.51.86.1-194.51.86.254:9921:9921>1::21:21 on:tcp:194.51.3.1-194.51.3.254:9921:9921>1::21:21 on:udp:213.56.31.225:9969:9969>1::69:69 on:udp:213.56.31.226:9969:9969>1::69:69 on:udp:213.56.0.20:9969:9969>1::69:69 on:udp:194.2.1.1-194.2.1.15:9969:9969>1::69:69 on:udp:194.2.203.1-194.2.203.254:9969:9969>1::69:69 on:udp:194.51.86.1-194.51.86.254:9969:9969>1::69:69 on:udp:194.51.3.1-194.51.3.254:9969:9969>1::69:69", 0},
	{"advanced_status", "1", 0},
#endif

#ifdef NOMAD_SUPPORT
	/* da_f 2005.3.18 add. start...
	 * VPN User Account: username passwd change_stat active_stat */
	{"vpn_account_1", "3.1415 admin 0 0", 0},
	{"vpn_account_2", "3.1415 admin 0 0", 0},
	{"vpn_account_3", "3.1415 admin 0 0", 0},
	{"vpn_account_4", "3.1415 admin 0 0", 0},
	{"vpn_account_5", "3.1415 admin 0 0", 0},
	/* VPN Client Status: online_offline start_time end_time */
	{"vpn_client_stat_1", "0 __ __", 0},
	{"vpn_client_stat_2", "0 __ __", 0},
	{"vpn_client_stat_3", "0 __ __", 0},
	{"vpn_client_stat_4", "0 __ __", 0},
	{"vpn_client_stat_5", "0 __ __", 0},
#endif
	/* da_f 2005.3.18 end */

#ifdef DHCP_DOMAIN_IP_MAPPING_SUPPORT
	{"DHCP_MAPPING_DOMAIN", "Sipura", 0},
#endif

	{ 0, 0, 0 },
};
