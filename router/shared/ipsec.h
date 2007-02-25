#ifndef _IPSEC_H
#define _IPSEC_H

#define IPSEC_TUNNELS 5

#define ENABLE  1
#define DISABLE 0

void entry_config_init();
void ltype_config_init();
void rtype_config_init();
void sg_config_init();
//void keytype_config_init();

struct entry_config
{
	char *tunnel_status;
	char *tunnel_name;
	char *local_type;
	char *localgw_conn;
	char *remote_type;
	char *sg_type;
	char *enc_type;
	char *auth_type;
	char *key_type;
} entry_config_ptr;

struct ltype_config
{
	char *l_ipaddr;
	char *l_netmask;
} ltype_config_ptr;

struct rtype_config
{
	char *r_ipaddr;
	char *r_netmask;
} rtype_config_ptr;

struct sg_config
{
	char *sg_ipaddr;
	char *sg_domain;
} sg_config_ptr;

/* zhangbin 2005.3.31 for remote id type id CLOUD*/
#ifdef CLOUD_SUPPORT
char ipsec_local_id[3];
char ipsec_remote_id[3];

struct lid_config
{
	char lid_ip[18];
	char lid_fqdn[80];
} lid_config_ptr;

struct rid_config
{
	char rid_ip[18];
	char rid_fqdn[80];
} rid_config_ptr;
#endif

#if 0
struct keytype_config
{
	char *auto_pfs;
	char *auto_presh;
	char *auto_klife;
	char *manual_enckey;
	char *manual_authkey;
	char *manual_ispi;
	char *manual_ospi;
}keytype_config_ptr;
#endif

struct keytype_config
{
	char auto_pfs[3];
	char auto_presh[100];
	char auto_klife[10];
	char manual_enckey[100];
	char manual_authkey[80];
	char manual_ispi[10];
	char manual_ospi[10];
}keytype_config_ptr;

struct ipsecadv_config
{
	char ipsec_opmode[3];
	char ipsecp1_enc[3];  							
	char ipsecp1_auth[3];
	char ipsecp1_group[3];
	char ipsecp1_klife[10]; 
	char ipsecp2_group[3];
	char ipsecp2_klife[10];
	char ipsec_netbios[3];
	char ipsec_antireplay[3];
	char ipsec_keepalive[3];
	char ipsec_blockip[3];
	char ipsec_retrytimes[10];
	char ipsec_blocksecs[12];
}	ipsecadv_config_ptr;

#define tunnel_status entry_config_ptr.tunnel_status
#define tunnel_name entry_config_ptr.tunnel_name
#define local_type entry_config_ptr.local_type
#define localgw_conn entry_config_ptr.localgw_conn
#define remote_type entry_config_ptr.remote_type
#define sg_type entry_config_ptr.sg_type
#define enc_type entry_config_ptr.enc_type
#define auth_type entry_config_ptr.auth_type
#define key_type entry_config_ptr.key_type

#define l_ipaddr ltype_config_ptr.l_ipaddr
#define l_netmask ltype_config_ptr.l_netmask

#define r_ipaddr rtype_config_ptr.r_ipaddr
#define r_netmask rtype_config_ptr.r_netmask

#define sg_ipaddr sg_config_ptr.sg_ipaddr
#define sg_domain sg_config_ptr.sg_domain

#define auto_pfs keytype_config_ptr.auto_pfs
#define auto_presh keytype_config_ptr.auto_presh
#define auto_klife keytype_config_ptr.auto_klife
#define manual_enckey keytype_config_ptr.manual_enckey
#define manual_authkey keytype_config_ptr.manual_authkey
#define manual_ispi keytype_config_ptr.manual_ispi
#define manual_ospi keytype_config_ptr.manual_ospi

/* for ipsec advanced setting */
#define ipsec_opmode ipsecadv_config_ptr.ipsec_opmode
#define ipsecp1_enc ipsecadv_config_ptr.ipsecp1_enc  							
#define ipsecp1_auth ipsecadv_config_ptr.ipsecp1_auth
#define ipsecp1_group ipsecadv_config_ptr.ipsecp1_group
#define ipsecp1_klife ipsecadv_config_ptr.ipsecp1_klife 
#define ipsecp2_group ipsecadv_config_ptr.ipsecp2_group
#define ipsecp2_klife ipsecadv_config_ptr.ipsecp2_klife
#define ipsec_netbios ipsecadv_config_ptr.ipsec_netbios
#define ipsec_antireplay ipsecadv_config_ptr.ipsec_antireplay
#define ipsec_keepalive ipsecadv_config_ptr.ipsec_keepalive
#define ipsec_blockip ipsecadv_config_ptr.ipsec_blockip
#define ipsec_retrytimes ipsecadv_config_ptr.ipsec_retrytimes
#define ipsec_blocksecs ipsecadv_config_ptr.ipsec_blocksecs

#ifdef CLOUD_SUPPORT
#define lid_ip lid_config_ptr.lid_ip
#define lid_fqdn lid_config_ptr.lid_fqdn
#define rid_ip rid_config_ptr.rid_ip
#define rid_fqdn rid_config_ptr.rid_fqdn
#endif

#endif
