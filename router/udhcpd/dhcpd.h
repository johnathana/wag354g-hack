/* dhcpd.h */
#ifndef _DHCPD_H
#define _DHCPD_H

#include <netinet/ip.h>
#include <netinet/udp.h>

#include "libbb_udhcp.h"
#include "leases.h"

/************************************/
/* Defaults _you_ may want to tweak */
/************************************/

/* the period of time the client is allowed to use that address */
#define LEASE_TIME              (60*60*24*10) /* 10 days of seconds */

/* where to find the DHCP server configuration file */
#define DHCPD_CONF_FILE         "/etc/udhcpd.conf"

/*****************************************************************/
/* Do not modify below here unless you know what you are doing!! */
/*****************************************************************/

/* DHCP protocol -- see RFC 2131 */
#define SERVER_PORT		67
#define CLIENT_PORT		68

#define DHCP_MAGIC		0x63825363

/* DHCP option codes (partial list) */
#define DHCP_PADDING		0x00
#define DHCP_SUBNET		0x01
#define DHCP_TIME_OFFSET	0x02
#define DHCP_ROUTER		0x03
#define DHCP_TIME_SERVER	0x04
#define DHCP_NAME_SERVER	0x05
#define DHCP_DNS_SERVER		0x06
#define DHCP_LOG_SERVER		0x07
#define DHCP_COOKIE_SERVER	0x08
#define DHCP_LPR_SERVER		0x09
#define DHCP_HOST_NAME		0x0c
#define DHCP_BOOT_SIZE		0x0d
#define DHCP_DOMAIN_NAME	0x0f
#define DHCP_SWAP_SERVER	0x10
#define DHCP_ROOT_PATH		0x11
#define DHCP_IP_TTL		0x17
#define DHCP_MTU		0x1a
#define DHCP_BROADCAST		0x1c
#define DHCP_NTP_SERVER		0x2a
#define DHCP_WINS_SERVER	0x2c
#define DHCP_REQUESTED_IP	0x32
#define DHCP_LEASE_TIME		0x33
#define DHCP_OPTION_OVER	0x34
#define DHCP_MESSAGE_TYPE	0x35
#define DHCP_SERVER_ID		0x36
#define DHCP_PARAM_REQ		0x37
#define DHCP_MESSAGE		0x38
#define DHCP_MAX_SIZE		0x39
#define DHCP_T1			0x3a
#define DHCP_T2			0x3b
#define DHCP_VENDOR		0x3c
#define DHCP_CLIENT_ID		0x3d
#define DHCP_VENDOR_SPEC        0x2b  

#define DHCP_END		0xFF


#define BOOTREQUEST		1
#define BOOTREPLY		2

#define ETH_10MB		1
#define ETH_10MB_LEN		6

#define DHCPDISCOVER		1
#define DHCPOFFER		2
#define DHCPREQUEST		3
#define DHCPDECLINE		4
#define DHCPACK			5
#define DHCPNAK			6
#define DHCPRELEASE		7
#define DHCPINFORM		8

#define BROADCAST_FLAG		0x8000

#define OPTION_FIELD		0
#define FILE_FIELD		1
#define SNAME_FIELD		2

/* miscellaneous defines */
#define MAC_BCAST_ADDR		(unsigned char *) "\xff\xff\xff\xff\xff\xff"
#define OPT_CODE 0
#define OPT_LEN 1
#define OPT_DATA 2

#ifdef STB_SUPPORT 
#define STB2KER_ADD 1
#define STB2KER_DEL 0
#endif

struct option_set {
	unsigned char *data;
	struct option_set *next;
};

struct server_config_t {
	u_int32_t server;		/* Our IP, in network order */
	u_int32_t start;		/* Start address of leases, network order */
	u_int32_t end;			/* End of leases, network order */
	struct option_set *options;	/* List of DHCP options loaded from the config file */
	char *interface;		/* The name of the interface to use */
	int ifindex;			/* Index number of the interface to use */
	unsigned char arp[6];		/* Our arp address */
	unsigned long lease;		/* lease time in seconds (host order) */
	unsigned long max_leases; 	/* maximum number of leases (including reserved address) */
	char remaining; 		/* should the lease file be interpreted as lease time remaining, or
			 		 * as the time the lease expires */
	unsigned long auto_time; 	/* how long should udhcpd wait before writing a config file.
					 * if this is zero, it will only write one on SIGUSR1 */
	unsigned long decline_time; 	/* how long an address is reserved if a client returns a
				    	 * decline message */
	unsigned long conflict_time; 	/* how long an arp conflict offender is leased for */
	unsigned long offer_time; 	/* how long an offered address is reserved */
	unsigned long min_lease; 	/* minimum lease a client can request*/
	char *lease_file;
	char *pidfile;
	char *notify_file;		/* What to run whenever leases are written */
	u_int32_t siaddr;		/* next server bootp option */
	char *sname;			/* bootp server name */
	char *boot_file;		/* bootp boot file option */
	int single_pc_mode; //add by leijun
	int share_wanip_flag;	
	int share_wanip_comm;

#ifdef STB_SUPPORT
	int stb_support;  //junzhao
	u_int32_t stb_server;
#endif

};	

extern struct server_config_t server_config;
extern struct dhcpOfferedAddr *leases;

#define			MAX_NUM		10
#define			MAC_LEN				6

struct macip{
	u_int32_t ipaddr;
	u_int8_t  macaddr[MAC_LEN];
};

struct hostnameip{
	u_int32_t ipaddr;
	u_int8_t hostname[64];
};

struct ShareWANIP
{
	u_int8_t	index; //if index == 0 ,auto to selece wan ip which is first getted. 
									//or, get the specical ifname.
	u_int8_t  macaddr[MAC_LEN]; 
	u_int32_t ipaddr; // the shared wan ip
	int lease;
	u_int32_t gwaddr;
};

extern struct hostnameip hostname_ip[MAX_NUM];
extern struct macip mac_ip[MAX_NUM];
extern struct ShareWANIP share_wan_ip[2];
extern int share_wanip_count;

#define 		SHARE_WANIP_INDEX	 1
int read_mac_index(char *line, void *arg);
int find_share_wan_ip(u_int8_t *hwaddr, u_int32_t *dst_ip);
void SendInfoToKernal(u_int32_t ip_addr, u_int8_t *hwaddr);
u_int32_t check_is_share_wanip_mac(u_int8_t *hwaddr);
u_int32_t get_share_wanip_gw(u_int8_t *hwaddr);
void clear_share_wanip(void);
void clear_sharewanip_mac(void);

#define WAN_IFNAME  "/tmp/status/wan_ifname"
#define WAN_IPADDR  "/tmp/status/wan_ipaddr"
#define WAN_NETMASK  "/tmp/status/wan_netmask"
#define WAN_GATEWAY  "/tmp/status/wan_gateway"
#define WAN_BROADCAST  "/tmp/status/wan_broadcast"
#define WAN_GET_DNS "/tmp/status/wan_get_dns"
#define WAN_HWADDR  "/tmp/status/wan_hwaddr"
#define WAN_PVC_STATUS "/tmp/status/wan_pvc_status"
#define WAN_PVC_ERRSTR "/tmp/status/wan_pvc_errstr"
#define WAN_ENABLE_TABLE "/tmp/status/wan_enable_table"
#define WAN_PPPOE_ACNAME "/tmp/status/wan_pppoe_acname"

#ifdef MPPPOE_SUPPORT
#define WAN_MPPPOE_IFNAME  "/tmp/status/wan_mpppoe_ifname"
#define WAN_MPPPOE_IPADDR  "/tmp/status/wan_mpppoe_ipaddr"
#define WAN_MPPPOE_NETMASK  "/tmp/status/wan_mpppoe_netmask"
#define WAN_MPPPOE_GATEWAY  "/tmp/status/wan_mpppoe_gateway"
#define WAN_MPPPOE_BROADCAST  "/tmp/status/wan_mpppoe_broadcast"
#define WAN_MPPPOE_GET_DNS "/tmp/status/wan_mpppoe_get_dns"
#endif

#ifdef IPPPOE_SUPPORT
#define WAN_IPPPOE_IFNAME  "/tmp/status/wan_ipppoe_ifname"
#define WAN_IPPPOE_IPADDR  "/tmp/status/wan_ipppoe_ipaddr"
#define WAN_IPPPOE_NETMASK  "/tmp/status/wan_ipppoe_netmask"
#define WAN_IPPPOE_GATEWAY  "/tmp/status/wan_ipppoe_gateway"
#define WAN_IPPPOE_BROADCAST  "/tmp/status/wan_ipppoe_broadcast"
#define WAN_IPPPOE_GET_DNS "/tmp/status/wan_ipppoe_get_dns"
#endif

#endif
