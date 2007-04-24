#include <httpd.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <shutils.h>
#include <code_pattern.h>
#include <cy_conf.h>
#include <utils.h>
#include <syslog.h>

#define sys_restart() kill(1, SIGHUP)
//#define sys_reboot() kill(1, SIGTERM)
#define sys_reboot() eval("reboot")
#define sys_stats(url) eval("stats", (url))
#define ARGV(args...) ((char *[]) { args, NULL })
#define STRUCT_LEN(name)    sizeof(name)/sizeof(name[0])
#define GOZILA_GET(name)	gozila_action ? websGetVar(wp, name, NULL) : nvram_safe_get(name);

#define SWAP(AA,BB)  { \
	char *CC; \
	CC = AA; \
	AA = BB; \
	BB = CC; \
}

/* for dhcp */
#define MAX_LEASES 254

/* for filter */
#define FILTER_IP_NUM 5
#define FILTER_PORT_NUM 5
#define FILTER_MAC_NUM 10
#define FILTER_MAC_PAGE 5
#define BLOCKED_SERVICE_NUM 2

/* for forward */
#define FORWARDING_NUM 10
#define SPECIAL_FORWARDING_NUM 10
#define UPNP_FORWARDING_NUM 15
#define PORT_TRIGGER_NUM 10

/* for static route */
#define STATIC_ROUTE_PAGE 20

/* for wireless */
#define WL_FILTER_MAC_PAGE 2
#define WL_FILTER_MAC_NUM 20
//#define WL_FILTER_MAC_COUNT 32

#define MAC_LEN 17
#define TMP_PASSWD "d6nw5v1x2pc7st9m"

extern int gozila_action;
extern int error_value;
extern int debug_value;
extern int filter_id;
extern int generate_key;
extern int clone_wan_mac;
extern char http_client_ip[20];
extern unsigned int ping_start_flag;
#ifdef HSIAB_SUPPORT
extern struct register_deviceResponse RegDevReply;
extern struct Sessioninfo *siptr;
extern struct deviceinfo di;
extern int register_status;
extern int new_device;
#endif

/*------songtao-----------*/
struct logmsg{
     char type[1000][2];
     int len;
     char level[1000];
     char contant[1000][125];

};
struct show_page
{
     int len;
     char level[30];
     char line[30][125];
     struct page *next;
     struct page *front;
};

/*-------------------------*/
struct variable {
	char *name;
	char *longname;
	void (*validate)(webs_t wp, char *value, struct variable *v);
	char **argv;
	int nullok;
};

struct onload {
        char *name;
        int (*go)(webs_t wp, char *arg);
};

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

struct apply_action {
	char *name;
	char *service;
	int sleep_time;
	int action;
        int (*go)(webs_t wp);
};

struct gozila_action {
	char *name;
	char *type;
	char *service;
	int sleep_time;
	int action;
        int (*go)(webs_t wp);
};

enum {SET, GET};

#ifdef HSIAB_SUPPORT
enum {
	DISABLE_HSIAB = 10,
	RESTART_HSIAB = 11,
     };
#endif

/* for index */
extern int ej_show_index_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_compile_date(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_compile_time(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_wl_max_channel(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_wl_domain(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_clone_mac(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_ntp(webs_t wp, char *value, struct variable *v);
extern void validate_lan_ipaddr(webs_t wp, char *value, struct variable *v);
extern void validate_wan_ipaddr(webs_t wp, char *value, struct variable *v);
extern int clone_mac(webs_t wp);
//junzhao 2004.4.22
extern void validate_dhcpserver_ipaddr(webs_t wp, char *value, struct variable *v);

//junzhao 2004.6.1
extern void validate_pvc_selection_config(webs_t wp, char *value, struct variable *v);
extern int ej_dump_match_condition(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_write_connection_table(webs_t wp, int active_index);
//junzhao 2004.7.7
extern int ej_dump_valid_parts(int eid, webs_t wp, int argc, char_t **argv);
int ej_active_connection_table(int eid, webs_t wp, int argc, char_t **argv);

/* for status */
extern int ej_show_uptime(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_status(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_localtime(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_nvram_status_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dhcp_remaining_time(int eid, webs_t wp, int argc, char_t **argv);
extern int dhcp_renew(webs_t wp);
extern int dhcp_release(webs_t wp);

//added by leijun 2004-0517
extern void validate_dhcp_reservedIp(webs_t wp, char *value, struct variable *v);
extern int ej_dhcp_reserved_ip(int eid, webs_t wp, int argc, char_t **argv);

extern int stop_ppp(webs_t wp);
extern int ej_show_status_setting(int eid, webs_t wp, int argc, char_t **argv);
//cjg:2004-3-7:PM
extern int ej_ping_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_ping_config(webs_t wp, char *value, struct variable *v);
extern int ej_show_ping_status(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_email_config_setting(int eid, webs_t wp, int argc, char_t **argv);extern int ej_email_enable_setting(int eid, webs_t wp, int argc, char_t **argv);extern void validate_email_config(webs_t wp, char *value, struct variable *v);


//add by lijunzhao
//2004.6.14
extern int ej_pppoe_show_htm(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_nvram_dslstatus_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_status_htm(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_nvram_wanstatus_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_status_connection_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_check_proto_fordns(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_hwaddr_get(int eid, webs_t wp, int argc, char_t **argv);

extern int ej_mpppoe_support(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ipppoe_support(int eid, webs_t wp, int argc, char_t **argv);
//junzhao 2004.6.25
extern int ej_clip_support(int eid, webs_t wp, int argc, char_t **argv);

//junzhao 2004.4.22 for dhcprelay
extern int ej_lan_dhcp_setting(int eid, webs_t wp, int argc, char_t **argv);

//add by junzhao 2004.4.8
extern int ej_wireless_support(int eid, webs_t wp, int argc, char_t **argv);

//junzhao add 2004.3.10
extern int save_vcc_tmpbuf(webs_t wp);
#ifdef STB_SUPPORT
extern int ej_dump_stb_entry(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_telus_config_setting(int eid, webs_t wp, int argc, char_t **argv);

#endif


#ifdef ARP_TABLE_SUPPORT
extern int ej_dump_arp_table(int eid, webs_t wp, int argc, char_t **argv);
extern int add_arp_table(webs_t wp);
extern int delete_arp_table(webs_t wp);
#endif

/*for dhcp */
extern int ej_dumpleases(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_dhcp(webs_t wp, char *value, struct variable *v);
extern void dhcp_check(webs_t wp, char *value, struct variable *v);
extern int delete_leases(webs_t wp);
/*----songtao-----*/
extern int ej_dump_syslog(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_vpnlog(int eid, webs_t wp, int argc, char_t **argv);//zhangbin 2004.4.20
extern void syslog_refresh(void);
extern void syslog_pagepre(void);
extern void syslog_pagenext(void);
extern void syslog_clear(void);
extern int  ej_entry_logmsg(int eid, webs_t wp,int argc, char_t **argv);
//zhangbin 2004.4.20
extern int  ej_vpn_logmsg(int eid, webs_t wp,int argc, char_t **argv);
extern struct show_page* create_showpage(struct logmsg *head_node,int type,int yes);
extern struct show_page* create_vpnpage(struct logmsg *head_node,int type,int yes); //zhangbin  2004.4.20

/* for log */
//extern int ej_dumplog(int eid, webs_t wp, int argc, char_t **argv);
/* zhangbin for vpn-log */
extern int ej_dumpvpnlog(int eid, webs_t wp, int argc, char_t **argv);
#ifdef SYSLOG_SUPPORT
//extern int ej_dump_syslog(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_log_settings(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_log_settings(webs_t wp, char *value, struct variable *v);
extern int set_log_type(webs_t wp);
#endif
extern int log_onload(webs_t wp);

/* for upgrade */
extern void do_upgrade_post(char *url, webs_t stream, int len, char *boundary);
extern int do_upgrade_cgi(char *url,  webs_t stream);

extern int sys_restore(char *url, webs_t stream, int *total);
extern void do_restore_post(char *url, webs_t stream, int len, char *boundary);
extern void do_restore_cgi(char *url,  webs_t stream);

extern int macclone_onload(webs_t wp, char *arg);
#ifdef DDM_SUPPORT
/* for DDM */
extern void do_ddm_post(char *url, webs_t stream, int len, char *boundary);
extern int ej_ddm_dumpleases(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_check_passwd(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_error_no(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_error_desc(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_ipend(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_wanproto(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_lanproto(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_idletime(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_wanipaddr(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_wannetmask(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_wangateway(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_wandns(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_get_pppuser(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_get_pppstatus(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_hwversion(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_get_ppppassword(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ddm_show_pppoe_acname(int eid, webs_t wp, int argc, char_t **argv);
//junzhao 2004.6.14
extern int ej_ddm_show_pppoe_srvname(int eid, webs_t wp, int argc, char_t **argv);
#endif
/* for filter */
extern int ej_filter_init(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_summary_show(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_ip_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_port_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_dport_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_mac_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_policy_select(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_policy_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_tod_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_web_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_port_services_get(int eid, webs_t wp, int argc, char_t **argv);
/* da_f@2005.1.27 */
#ifdef FILTER_WAN_SELECT_SUPPORT
extern int ej_filter_checkbox_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_wan_group_ip_node_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_wan_range_ip_node_get(int eid, webs_t wp, int argc, char_t **argv);
#endif


/* da_f 2005.3.18 add. start... */
#ifdef NOMAD_SUPPORT
extern void validate_vpn_account(webs_t wp, char *value, struct variable *v);
extern void rm_vpn_cli_account(webs_t wp, char *value, struct variable *v);
extern void validate_vpn_cli_active(webs_t wp, char *value, struct variable *v);
extern void validate_vpn_cli_disconn(webs_t wp, char *value, struct variable *v);
extern int ej_vpn_account_list_get(int eid, webs_t wp, int argc, char_t **argv);
//extern int ej_vpn_account_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_vpn_usernames_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_vpn_clients_status(int eid, webs_t wp, int argc, char_t **argv);
extern int reboot_by_vpn(webs_t wp);
//extern void do_vpn_ej(char *url, webs_t stream);
#endif
/* da_f 2005.3.18 end */

extern void validate_filter_policy(webs_t wp, char *value, struct variable *v);
extern void validate_filter_ip_grp(webs_t wp, char *value, struct variable *v);
extern void validate_filter_mac_grp(webs_t wp, char *value, struct variable *v);
extern void validate_filter_dport_grp(webs_t wp, char *value, struct variable *v);
extern void validate_filter_port(webs_t wp, char *value, struct variable *v);
extern void validate_filter_web(webs_t wp, char *value, struct variable *v);
extern void validate_blocked_service(webs_t wp, char *value, struct variable *v);
#ifdef STB_SUPPORT
extern void validate_stbentry_config(webs_t wp, char *value, struct variable *v);
extern void validate_telus_config(webs_t wp, char *value, struct variable *v);
#endif
extern int filter_onload(webs_t wp);
extern int save_policy(webs_t wp);
extern int summary_delete_policy(webs_t wp);
extern int single_delete_policy(webs_t wp);
extern int save_services_port(webs_t wp);

#ifdef FIREWALL_LEVEL_SUPPORT
extern int ej_filter_inb_mix_get(int eid, webs_t wp, int argc, char_t **argv);
extern int filtersummary_inb_onload(webs_t wp, char *arg);
extern void validate_filter_inb_policy(webs_t wp, char *value, struct variable *v);
extern int save_inb_policy(webs_t wp);
extern int summary_delete_inb_policy(webs_t wp);
extern int single_delete_inb_policy(webs_t wp);
extern int ej_filter_inb_init(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_inb_summary_show(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_inb_policy_select(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_inb_policy_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_filter_inb_tod_get(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_filter_inb_mix(webs_t wp, char *value, struct variable *v);
#endif

/* for forward */
extern int ej_port_forward_table(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_forward_proto(webs_t wp, char *value, struct variable *v);
//#ifdef UPNP_FORWARD_SUPPORT //modified by leijun 0313_22:00
extern int ej_forward_upnp(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_forward_upnp(webs_t wp, char *value, struct variable *v);
//#endif
//
#ifdef ADVANCED_FORWARD_SUPPORT//add by zhs 
extern void validate_advanced_forwarding(webs_t wp, char *value,struct variable *v);
extern int ej_advanced_forwarding_init(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_adv_forward(int eid, webs_t wp, int argc, char_t **argv);
#endif//end by zhs

#ifdef SPECIAL_FORWARD_SUPPORT
extern int ej_spec_forward_table(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_forward_spec(webs_t wp, char *value, struct variable *v);
#endif
#ifdef PORT_TRIGGER_SUPPORT
extern int ej_port_trigger_table(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_port_trigger(webs_t wp, char *value, struct variable *v);
#endif

/* for dynamic route */
extern int ej_dump_route_table(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_dynamic_route(webs_t wp, char *value, struct variable *v);
extern void validate_nat_func(webs_t wp, char *value, struct variable *v);
extern int dynamic_route_onload(webs_t wp);

/*added by zls 2005-0112*/
extern int ej_nvram_match_routing(int eid,webs_t wp, int argc, char_t **argv);

#ifdef RIP_EXTEND_SUPPORT
extern int ej_rip_interface_table(int eid, webs_t wp, int argc, char_t **argv);
#endif

/* for static route */
extern int ej_static_route_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_static_route_table(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_static_route(webs_t wp, char *value, struct variable *v);
extern int delete_static_route(webs_t wp);

/* for wireless */
extern void validate_wl_key(webs_t wp, char *value, struct variable *v);
extern void validate_wl_wep(webs_t wp, char *value, struct variable *v);
extern void validate_wl_auth(webs_t wp, char *value, struct variable *v);
extern void validate_d11_channel(webs_t wp, char *value, struct variable *v);
extern int add_active_mac(webs_t wp);
extern int wl_active_onload(webs_t wp, char *arg);
extern int generate_key_64(webs_t wp);
extern int generate_key_128(webs_t wp);
extern int ej_get_wep_value(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_wl_active_mac(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_wl_value(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_wpa_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void wl_unit(webs_t wp, char *value, struct variable *v);
extern void validate_wpa_psk(webs_t wp, char *value, struct variable *v);
extern void validate_auth_mode(webs_t wp, char *value, struct variable *v);
extern void validate_security_mode(webs_t wp, char *value, struct variable *v);
extern int reget_wireless_station_info(int index, char *type, char *value);//
extern int wl_count;//
/* for ddns */
extern int ddns_save_value(webs_t wp);
extern int ddns_update_value(webs_t wp);

/* zhangbin foripsec*/
extern int ej_tunnel_select(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_localgw_select(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ipsec_show_summary(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_name_array(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_spi_array(int eid, webs_t wp, int argc, char_t **argv);
extern void entry_config_init();
extern int ej_entry_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ltype_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void rtype_config_init();
extern int ej_rtype_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void sg_config_init();
extern int ej_sg_config_setting(int eid, webs_t wp, int argc, char_t **argv);

#ifdef CLOUD_SUPPORT
extern void sg_config_init();
extern int ej_lid_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_rid_config_setting(int eid, webs_t wp, int argc, char_t **argv);
#endif

extern int ipsec_delete_tunnel(webs_t wp);
extern int ej_keytype_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void ipsecadv_config_init(int which);
extern int ej_ipsecadv_config_setting(int eid, webs_t wp, int argc, char_t **argv);

extern void validate_entry_config(webs_t wp, char *value, struct variable *v);
extern void validate_ltype_config(webs_t wp, char *value, struct variable *v);
extern void validate_rtype_config(webs_t wp, char *value, struct variable *v);
extern void validate_sg_config(webs_t wp, char *value, struct variable *v);
#ifdef CLOUD_SUPPORT
extern void validate_lid_config(webs_t wp, char *value, struct variable *v);
extern void validate_rid_config(webs_t wp, char *value, struct variable *v);
#endif
extern void validate_keytype_config(webs_t wp, char *value, struct variable *v);
extern void validate_ipsecadv_config(webs_t wp, char *value, struct variable *v);
extern void validate_ipsecadv_config(webs_t wp, char *value, struct variable *v);
extern int ej_show_ltype_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_rtype_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_sg_setting(int eid, webs_t wp, int argc, char_t **argv);
#ifdef CLOUD_SUPPORT
extern int ej_show_localid_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_remoteid_setting(int eid, webs_t wp, int argc, char_t **argv);
#endif
extern int ej_show_keytype_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_connect_setting(int eid, webs_t wp, int argc, char_t **argv);

extern int connect_ipsec(webs_t wp);
extern int disconnect_ipsec(webs_t wp);

/* zhangbin for qos*/
//extern void qos_config_init();
extern int ej_qos_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_qos_config(webs_t wp, char *value, struct variable *v);
#ifdef QOS_ADVANCED_SUPPORT
extern int ej_dump_init_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_verify_setting(int eid, webs_t wp, int argc, char_t **argv);
#endif

extern void validate_macmode(webs_t wp, char *value, struct variable *v);
extern void validate_wl_hwaddrs(webs_t wp, char *value, struct variable *v);
extern int ej_wireless_active_table(int eid, webs_t wp, int argc, char_t **argv);
//wwzh at 2004-03-07
extern int ej_wireless_connected_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_wireless_filter_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_wl_wep_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_wl_wep_key(webs_t wp, char *value, struct variable *v);


#ifdef DNS_ENTRY_SUPPORT
//wwzh add for DNS ENTRY TABLE
extern int ej_dns_entry_table(int eid, webs_t wp, int argc, char_t **argv);
extern int add_dns_entry(webs_t wp);
extern int delete_dns_entry(webs_t wp);
#endif

/* for ddns */
extern int ej_show_ddns_status(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_ddns_ip(int eid, webs_t wp, int argc, char_t **argv);
//
/* for test */
extern int ej_wl_packet_get(int eid, webs_t wp, int argc, char_t **argv);

#ifdef AOL_SUPPORT
/* for aol */
extern int ej_aol_value_get(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_aol_settings_show(int eid, webs_t wp, int argc, char_t **argv);
#endif

#ifdef EMI_TEST
extern int emi_test_exec(webs_t wp);
extern int emi_test_del(webs_t wp);
extern int ej_dump_emi_test_log(int eid, webs_t wp, int argc, char_t **argv);
#endif

#ifdef DIAG_SUPPORT
extern int ej_dump_ping_log(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_traceroute_log(int eid, webs_t wp, int argc, char_t **argv);
extern int diag_ping_start(webs_t wp);
extern int diag_ping_stop(webs_t wp);
extern int diag_ping_clear(webs_t wp);
extern int diag_traceroute_start(webs_t wp);
extern int diag_traceroute_stop(webs_t wp);
extern int diag_traceroute_clear(webs_t wp);
extern int ping_onload(webs_t wp, char *arg);
extern int traceroute_onload(webs_t wp, char *arg);
#endif

#ifdef HSIAB_SUPPORT
extern int ej_get_hsiab_value(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_hsiab_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_hsiab_config(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_hsiab_db(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_dump_hsiab_msg(int eid, webs_t wp, int argc, char_t **argv);
extern int hsiab_register(webs_t wp);
extern int hsiab_register_ok(webs_t wp);
extern int hsiab_finish_registration(webs_t wp);
#endif

//add by lijunzhao
//for PVC
extern int ej_connection_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_vcc_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_pppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_pppoa_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_bridged_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_routed_config_setting(int eid, webs_t wp, int argc, char_t **argv);
//add by lzh;
extern int ej_pvc_connection_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_upnp_connection_table(int eid, webs_t wp, int argc, char_t **argv);

#ifdef CLIP_SUPPORT
extern int ej_clip_config_setting(int eid, webs_t wp, int argc, char_t **argv);
#endif
#ifdef STB_SUPPORT
extern void validate_stbentry_config(webs_t wp, char *value, struct variable *v);
#endif

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
extern void validate_unip_pppoe_config(webs_t wp, char *value, struct variable *v);
extern void validate_unip_pppoa_config(webs_t wp, char *value, struct variable *v);
extern int ej_unip_pppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_unip_pppoa_config_setting(int eid, webs_t wp, int argc, char_t **argv);
#endif

extern int ej_mtu_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_macclone_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_vpivci_getvalue(int eid, webs_t wp, int argc, char_t **argv);

extern void validate_vcc_config(webs_t wp, char *value, struct variable *v);
extern void validate_pppoe_config(webs_t wp, char *value, struct variable *v);
extern void validate_pppoa_config(webs_t wp, char *value, struct variable *v);
extern void validate_bridged_config(webs_t wp, char *value, struct variable *v);
extern void validate_routed_config(webs_t wp, char *value, struct variable *v);
#ifdef CLIP_SUPPORT
extern void validate_clip_config(webs_t wp, char *value, struct variable *v);
#endif
extern void validate_mtu_config(webs_t wp, char *value, struct variable *v);	
extern void validate_macclone_config(webs_t wp, char *value, struct variable *v);	

#ifdef MPPPOE_SUPPORT
extern int ej_mpppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_mpppoe_enable_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_status2_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_mpppoe_config(webs_t wp, char *value, struct variable *v);
#endif

#ifdef IPPPOE_SUPPORT
extern int ej_ipppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_ipppoe_enable_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_status2_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_ipppoe_config(webs_t wp, char *value, struct variable *v);
#endif


/* for all */
extern int ej_onload(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_web_page_name(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_compile_date(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_compile_time(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_model_name(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_firmware_version(int eid, webs_t wp, int argc, char_t **argv);

extern int valid_ipaddr(webs_t wp, char *value, struct variable *v);
extern int valid_range(webs_t wp, char *value, struct variable *v);
extern int valid_hwaddr(webs_t wp, char *value, struct variable *v);
extern int valid_choice(webs_t wp, char *value, struct variable *v);
extern int valid_netmask(webs_t wp, char *value, struct variable *v);
extern int valid_name(webs_t wp, char *value, struct variable *v);
extern int valid_merge_ipaddrs(webs_t wp, char *value, struct variable *v);
extern int valid_wep_key(webs_t wp, char *value, struct variable *v);

extern int get_dns_ip(char *name, int which, int count);
extern int get_single_ip(char *ipaddr, int which);
extern int get_merge_ipaddr(char *name, char *ipaddr);
extern int get_merge_mac(char *name, char *macaddr);
extern char *rfctime(const time_t *timep);
extern int legal_ipaddr(char *value);
extern int legal_hwaddr(char *value);
extern int legal_netmask(char *value);
extern int legal_ip_netmask(char *sip, char *smask, char *dip);
extern int find_pattern(const char *data, size_t dlen,
                        const char *pattern, size_t plen,
                        char term, 
                        unsigned int *numoff,
                        unsigned int *numlen);

extern int find_match_pattern(char *name, size_t mlen,
                        const char *data,
                        const char *pattern,
                        char *def);

extern int find_each(char *name, int len,
	          char *data,
	          char *token,
	          int which,
	          char *def);

/* 
 * set type to 1 to replace ' ' with "&nbsp;" and ':' with "&semi;"
 * set type to 2 to replace "&nbsp;" with ' ' and "&semi;" with ':'
 */
extern int filter_name(char *old_name, char *new_name, size_t size, int type);

/* check the value for a digit (0 through 9) 
 * set flag to 0 to ignore zero-length values
 */
extern int ISDIGIT(char *value, int flag);

/* checks  whether  value  is  a  7-bit unsigned char value that fits into the ASCII character set 
 * set flag to 0 to ignore zero-length values
 */
extern int ISASCII(char *value, int flag);

extern int do_setup_wizard(char *url, webs_t stream);
extern int do_calibration(char *url, webs_t stream);

extern int ej_show_logo(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_sysinfo(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_miscinfo(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_get_backup_name(int eid, webs_t wp, int argc, char_t **argv);

extern struct servent * my_getservbyport(int port, const char *proto);
extern int get_single_mac(char *macaddr, int which);

extern int StopContinueTx(char *value);
extern int StartContinueTx(char *value);
extern int Check_TSSI(char *value);
extern int Get_TSSI(char *value);
extern int Enable_TSSI(char *value);

extern void LOG(int level, const char *fmt,...);

extern char *num_to_protocol(int num);
extern int protocol_to_num(char *proto);
#ifdef BACKUP_RESTORE_SUPPORT
extern  void do_backup(char *path, webs_t stream);
#endif

/* add kirby 2004/7/1 */
extern int diagtool_start(webs_t wp);
extern int ej_dump_diagtool_log(int eid,webs_t wp,int argc,char_t **argv);

/* add for IGMP Proxy 2004-10-10 */
#ifdef IGMP_PROXY_SUPPORT
extern int ej_igmp_proxy_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_igmp_proxy_channel(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_igmp_proxy_config(webs_t wp, char *value, struct variable *v);
#endif

//  For IP Range Restricted 2004-11-22
extern void validate_ip_range_setting(webs_t wp, char *value, struct variable *v);
extern int ej_ip_restrict_config(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_show_ip_range_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_remote_management_config(int eid, webs_t wp, int argc, char_t **argv);

extern int ej_passwd_match(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_l_passwd_match(int eid, webs_t wp, int argc, char_t **argv);

//csh for snmp
extern int ej_show_snmp_setting(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_snmp_config_setting(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_snmp_config(webs_t wp, char *value, struct variable *v);

//add by peny
#ifdef HTTPS_SUPPORT
extern int  ej_set_authproto(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_authproto(webs_t wp, char *value, struct variable *v);
//Lai add 2005-07-04
extern void validate_privproto (webs_t wp, char *value, struct variable *v);
//Lai add 2005-07-08
#endif
//Lai modify 2005-07-25
extern void validate_snmpv3_support (webs_t wp, char *value, struct variable *v);
extern void validate_trap_ipaddr(webs_t wp,char *value, struct variable *v);
//csh for gre
#if defined(GRE_SUPPORT) || defined(CLOUD_SUPPORT)
extern int ej_gre_config(int eid, webs_t wp, int argc, char_t **argv);
extern void validate_gre_config(webs_t wp, char *value, struct variable *v);
#endif
#ifdef GRE_SUPPORT
extern int ej_ipsec_gre_config(int eid, webs_t wp, int argc, char_t **argv);
extern int gre_delete_tunnel(webs_t wp);
#endif
#ifdef CLOUD_SUPPORT
extern int ej_cloud_init(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_index_init(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_cloud_ipsec_init(int eid, webs_t wp, int argc, char_t **argv);
#endif

#ifdef SECLAN_SUPPORT
/* kirby add for FFT */
extern void validate_seclan_ipaddr(webs_t wp, char *value, struct variable *v);
#endif
