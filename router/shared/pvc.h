#ifndef PVC_H
#define PVC_H

#define PVC_CONNECTIONS 8
//zls
#define RIP_INTERFACE_SUM 9


//Encap Mode
#define BRIDGED  0
#define ROUTED 1
#define PPPOE  2
#define PPPOA  3
#define PPTP   4
#define BRIDGEDONLY  5
#ifdef CLIP_SUPPORT
#define CLIP   7
#endif

/* kirby for unip 2004/11.24 */
#ifdef  UNNUMBERED_SUPPORT
#define UNIP_PPPOE      8
#define UNIP_PPPOA      9
#endif

//Multiplexing Mode
#define LLC 0
#define VC 1

//Qos Type
#define UBR 0
#define CBR 1
#define VBR 2

//Autodetect
#define AUTOENABLE  1
#define AUTODISABLE 0

//Pppoe(a) Demand
#define DEMANDENABLE  1
#define DEMANDDISABLE 0

//Dhcp Enable
#define DHCPENABLE  1
#define DHCPDISABLE 0

//status file name
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
#define WAN_PPPOE_SRVNAME "/tmp/status/wan_pppoe_srvname"
//junzhao 2004.8.17
#ifdef VZ7
#define WAN_DHCPD_IPADDR "/tmp/status/wan_dhcpd_ipaddr"
#endif

#ifdef MPPPOE_SUPPORT
#define WAN_MPPPOE_IFNAME  "/tmp/status/wan_mpppoe_ifname"
#define WAN_MPPPOE_IPADDR  "/tmp/status/wan_mpppoe_ipaddr"
#define WAN_MPPPOE_NETMASK  "/tmp/status/wan_mpppoe_netmask"
#define WAN_MPPPOE_GATEWAY  "/tmp/status/wan_mpppoe_gateway"
#define WAN_MPPPOE_BROADCAST  "/tmp/status/wan_mpppoe_broadcast"
#define WAN_MPPPOE_GET_DNS "/tmp/status/wan_mpppoe_get_dns"
#define WAN_MPPPOE_ACNAME "/tmp/status/wan_mpppoe_acname"
#define WAN_MPPPOE_SRVNAME "/tmp/status/wan_mpppoe_srvname"
#endif

#ifdef IPPPOE_SUPPORT
#define WAN_IPPPOE_IFNAME  "/tmp/status/wan_ipppoe_ifname"
#define WAN_IPPPOE_IPADDR  "/tmp/status/wan_ipppoe_ipaddr"
#define WAN_IPPPOE_NETMASK  "/tmp/status/wan_ipppoe_netmask"
#define WAN_IPPPOE_GATEWAY  "/tmp/status/wan_ipppoe_gateway"
#define WAN_IPPPOE_BROADCAST  "/tmp/status/wan_ipppoe_broadcast"
#define WAN_IPPPOE_GET_DNS "/tmp/status/wan_ipppoe_get_dns"
#define WAN_IPPPOE_ACNAME "/tmp/status/wan_ipppoe_acname"
#define WAN_IPPPOE_SRVNAME "/tmp/status/wan_ipppoe_srvname"
#endif

void vcc_config_init();
void pppoe_config_init();
void pppoa_config_init();
void bridged_config_init();
void routed_config_init();

#ifdef CLIP_SUPPORT
void clip_config_init();
#endif

#ifdef MPPPOE_SUPPORT
void mpppoe_config_init();
#endif

#ifdef IPPPOE_SUPPORT
void ipppoe_config_init();
#endif


//global buffer
struct vcc_config
{
	char *vpi;
	char *vci;
	char *encapmode;
	char *multiplex;
	char *qos;
	char *pcr;
	char *scr;
	char *autodetect;
	char *applyonboot;
}vcc_config_ptr;

struct pppoe_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
	char *servicename;
}pppoe_config_ptr;

struct pppoa_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
}pppoa_config_ptr;

struct bridged_config
{
	char *dhcpenable;
	char *ipaddr;
	char *netmask;
	char *gateway;
	char *pdns;
	char *sdns;
}bridged_config_ptr;

struct routed_config
{
	char *ipaddr;
	char *netmask;
	char *gateway;
	char *pdns;
	char *sdns;
}routed_config_ptr;

#ifdef CLIP_SUPPORT
struct clip_config
{
	char *ipaddr;
	char *netmask;
	char *gateway;
	char *pdns;
	char *sdns;
}clip_config_ptr;
#endif

#ifdef MPPPOE_SUPPORT
struct mpppoe_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
	char *servicename;
	char *domainname;
}mpppoe_config_ptr;
#endif

#ifdef IPPPOE_SUPPORT
struct ipppoe_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
	char *servicename;
	char *domainname;
}ipppoe_config_ptr;
#endif

/* kirby for unip 2004/11.24 */
#ifdef  UNNUMBERED_SUPPORT
struct unip_pppoe_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
	char *servicename;
}unip_pppoe_config_ptr;

struct unip_pppoa_config
{
	char *username;
	char *password;
	char *demand;
	char *idletime;
	char *redialperiod;
}unip_pppoa_config_ptr;
#endif

#define vpi vcc_config_ptr.vpi
#define vci vcc_config_ptr.vci
#define encapmode vcc_config_ptr.encapmode
#define multiplex vcc_config_ptr.multiplex
#define qos vcc_config_ptr.qos
#define pcr vcc_config_ptr.pcr
#define scr vcc_config_ptr.scr
#define autodetect vcc_config_ptr.autodetect
#define applyonboot vcc_config_ptr.applyonboot

#define e_username pppoe_config_ptr.username
#define e_password pppoe_config_ptr.password
#define e_demand pppoe_config_ptr.demand
#define e_idletime pppoe_config_ptr.idletime
#define e_redialperiod pppoe_config_ptr.redialperiod
#define e_servicename pppoe_config_ptr.servicename

#define a_username pppoa_config_ptr.username
#define a_password pppoa_config_ptr.password
#define a_demand pppoa_config_ptr.demand
#define a_idletime pppoa_config_ptr.idletime
#define a_redialperiod pppoa_config_ptr.redialperiod

#define dhcpenable bridged_config_ptr.dhcpenable
#define b_ipaddr bridged_config_ptr.ipaddr
#define b_netmask bridged_config_ptr.netmask
#define b_gateway bridged_config_ptr.gateway
#define b_pdns bridged_config_ptr.pdns
#define b_sdns bridged_config_ptr.sdns

#define r_ipaddr routed_config_ptr.ipaddr
#define r_netmask routed_config_ptr.netmask
#define r_gateway routed_config_ptr.gateway
#define r_pdns routed_config_ptr.pdns
#define r_sdns routed_config_ptr.sdns

#ifdef CLIP_SUPPORT
#define c_ipaddr clip_config_ptr.ipaddr
#define c_netmask clip_config_ptr.netmask
#define c_gateway clip_config_ptr.gateway
#define c_pdns clip_config_ptr.pdns
#define c_sdns clip_config_ptr.sdns
#endif

#ifdef MPPPOE_SUPPORT
#define m_username mpppoe_config_ptr.username
#define m_password mpppoe_config_ptr.password
#define m_demand mpppoe_config_ptr.demand
#define m_idletime mpppoe_config_ptr.idletime
#define m_redialperiod mpppoe_config_ptr.redialperiod
#define m_servicename mpppoe_config_ptr.servicename
#define m_domainname mpppoe_config_ptr.domainname
#endif

#ifdef IPPPOE_SUPPORT
#define i_username ipppoe_config_ptr.username
#define i_password ipppoe_config_ptr.password
#define i_demand ipppoe_config_ptr.demand
#define i_idletime ipppoe_config_ptr.idletime
#define i_redialperiod ipppoe_config_ptr.redialperiod
#define i_servicename ipppoe_config_ptr.servicename
#define i_domainname ipppoe_config_ptr.domainname
#endif

/* kirby for unip 2004/11.24 */
#ifdef  UNNUMBERED_SUPPORT
#define ue_username unip_pppoe_config_ptr.username
#define ue_password unip_pppoe_config_ptr.password
#define ue_demand unip_pppoe_config_ptr.demand
#define ue_idletime unip_pppoe_config_ptr.idletime
#define ue_redialperiod unip_pppoe_config_ptr.redialperiod
#define ue_servicename unip_pppoe_config_ptr.servicename

#define ua_username unip_pppoa_config_ptr.username
#define ua_password unip_pppoa_config_ptr.password
#define ua_demand unip_pppoa_config_ptr.demand
#define ua_idletime unip_pppoa_config_ptr.idletime
#define ua_redialperiod unip_pppoa_config_ptr.redialperiod
#endif

//junzhao 2004.6.1
#define PPP_MASK 4
#define MUL_MASK 3

#endif
