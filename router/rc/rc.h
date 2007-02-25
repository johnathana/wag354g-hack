/*
 * Router rc control script
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: rc.h,v 1.5 2006/10/08 06:47:31 dom Exp $
 */

#ifndef _rc_h_
#define _rc_h_

#define __CONFIG_NETCONF__

#include "ledapp.h"
#include <cy_conf.h>

/* udhcpc scripts */
extern int udhcpc_main(int argc, char **argv);

/* ppp scripts */
extern int ipup_main(int argc, char **argv);
extern int ipdown_main(int argc, char **argv);
/*add by peny for tr069*/
extern void inform_start_dhcpc(char *wifname, char *ipaddr, int whichconn); /* da_f@2005.3.4 modify */

/* http functions */
extern int http_get(const char *server, char *buf, size_t count, off_t offset);
extern int http_post(const char *server, char *buf, size_t count);
extern int http_stats(const char *url);

/* init */
extern int console_init(void);
extern pid_t run_shell(int timeout, int nowait);
extern void signal_init(void);
extern void fatal_signal(int sig);

//junzhao
extern int checkadsl_main(int argc, char **argv);
int wanledctrl_main(int argc, char **argv);

/* interface */
extern int ifconfig(char *ifname, int flags, char *addr, char *netmask);
extern int route_add(char *name, int metric, char *dst, char *gateway, char *genmask);
extern int route_del(char *name, int metric, char *dst, char *gateway, char *genmask);

/* network */
extern void start_lan(void);
extern void stop_lan(void);
extern void start_wan(int status);
extern void start_wan_done(char *ifname);
extern void stop_wan(void);
/* services */
extern int start_dhcpd(void);
extern int stop_dhcpd(void);
//by songtao
extern int start_snmp(void);
extern int stop_snmp(void);
extern int read_snmpvar(void);
extern int start_dns(void);
extern int stop_dns(void);
extern int start_ntpc(void);
extern int stop_ntpc(void);
extern int start_services(void);
extern int stop_services(void);

#if 0
#if defined(CONFIG_CWMP)
extern int start_cwmp(void);
extern int stop_cwmp(void);
#endif
#endif

extern int config_vlan(void);
extern void config_loopback(void);

extern int start_nas(char *type);

/* zhangbin for ipsec  */
extern int start_ipsec(void);
extern int stop_ipsec(void);

/* firewall */
#ifdef __CONFIG_NETCONF__
extern int start_firewall(void);
extern int stop_firewall(void);
#else
#define start_firewall() do {} while (0)
#define stop_firewall() do {} while (0)
#endif

/* routes */
extern int set_routes(void);

////////////////////////////////////////////////////////////
#define BOOT 0 
#define REDIAL 1

#ifdef CLOUD_SUPPORT
#define CLOUD_WLANFACE "wlan0"
#define CLOUD_GREFACE "GRE"
#define CLOUD_LANFACE "br0"
#define CLOUD_VPNFACE "ipsec0"
#endif

extern int start_resetbutton(void);  
extern int stop_resetbutton(void);  

#ifdef CONFIG_ANTSEL
extern int start_antsel(void);  
extern int stop_antsel(void);  
#endif

extern int start_tftpd(void);
extern int stop_tftpd(void);

extern int start_cron(void);
extern int stop_cron(void);

extern int start_zebra(void);
extern int stop_zebra(void);

extern int start_redial(char *second, char *devname);
//extern int stop_redial(void);
extern int stop_redial(int which);

extern int start_ddns(void);
extern int stop_ddns(void);

extern int start_upnp(void);
//extern int stop_upnp(void); //leijun
extern void stop_upnp(int flag); //leijun
extern void stop_upnp_pak(void); //leijun

extern int start_ntp(void);
extern int stop_ntp(void);

extern int start_pptp(int status);
extern int stop_pptp(void);

extern int start_SysLog(void);
extern int stop_SysLog(void);

//extern int stop_dhcpc(void);
extern int stop_pppoe(void);
extern int stop_dhcpc(int whichconn);
extern int stop_pppox(int whichconn);

extern int start_httpd(void);
extern int stop_httpd(void);

extern int filtersync_main(void);
extern int filter_add(int seq);
extern int filter_del(int seq);
extern int resetbutton_main(int argc, char **argv);
extern int ntp_main(int argc, char **argv);
extern int ipupdate_main(int argc, char **argv);
extern int gpio_main(int argc, char **argv);
extern int redial_main(int argc, char **argv);

extern int init_nvram(void);
#ifdef WRITE_SN_SUPPORT
extern void nvram_unicode_check(char *check_str);
#endif
extern int check_image(void);
extern void unset_nvram(void);

extern int del_routes(char *route);

extern int start_single_service(void);

extern int write_boot(const char *path, const char *mtd);

//extern int init_mtu(char *wan_proto);
extern int init_mtu(char *devname, char *buf);
//extern int force_to_dial(void);
extern int force_to_dial(int num);
extern char *range(char *start, char *end);


extern int start_heartbeat(int status);
extern int stop_heartbeat(void);
extern int hb_connect_main(int argc, char **argv);
extern int hb_disconnect_main(int argc, char **argv);
extern int monitor_ps_main(int argc, char **argv);
extern int listen_main(int argc, char **argv);
extern int ddns_success_main(int argc, char **argv);


#endif /* _rc_h_ */
