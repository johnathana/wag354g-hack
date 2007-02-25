#ifndef _IPSEC_SERVICE_H
#define _IPSEC_SERVICE_H

#define IPSEC_TUNNELS 5

#define ENABLE  1
#define DISABLE 0


struct ser_entry_config
{
	char tunnel_status[3];
	char tunnel_name[25];
	char local_type[3];
	char localgw_conn[3];
	char remote_type[3];
	char sg_type[3];
	char enc_type[3];
	char auth_type[3];
	char key_type[3];
} ser_entry_config_ptr;

struct ser_ltype_config
{
	char l_ipaddr[20];
	char l_netmask[20];
} ser_ltype_config_ptr;

struct ser_rtype_config
{
	char r_ipaddr[20];
	char r_netmask[20];
} ser_rtype_config_ptr;

struct ser_sg_config
{
	char sg_ipaddr[20];
	char sg_domain[50];
} ser_sg_config_ptr;

struct ser_keytype_config
{
	char auto_pfs[3];
	char auto_presh[100];
	char auto_klife[20];
	char manual_enckey[100];
	char manual_authkey[100];
	char manual_ispi[100];
	char manual_ospi[100];
} ser_keytype_config_ptr;

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
	char ipsec_blocksecs[10];
}	ser_ipsecadv_config_ptr;


#define ser_tunnel_status ser_entry_config_ptr.tunnel_status
#define ser_tunnel_name ser_entry_config_ptr.tunnel_name
#define ser_local_type ser_entry_config_ptr.local_type
#define ser_localgw_conn ser_entry_config_ptr.localgw_conn
#define ser_remote_type ser_entry_config_ptr.remote_type
#define ser_sg_type ser_entry_config_ptr.sg_type
#define ser_enc_type ser_entry_config_ptr.enc_type
#define ser_auth_type ser_entry_config_ptr.auth_type
#define ser_key_type ser_entry_config_ptr.key_type

#define ser_l_ipaddr ser_ltype_config_ptr.l_ipaddr
#define ser_l_netmask ser_ltype_config_ptr.l_netmask

#define ser_r_ipaddr ser_rtype_config_ptr.r_ipaddr
#define ser_r_netmask ser_rtype_config_ptr.r_netmask

#define ser_sg_ipaddr ser_sg_config_ptr.sg_ipaddr
#define ser_sg_domain ser_sg_config_ptr.sg_domain

#define ser_auto_pfs ser_keytype_config_ptr.auto_pfs
#define ser_auto_presh ser_keytype_config_ptr.auto_presh
#define ser_auto_klife ser_keytype_config_ptr.auto_klife
#define ser_manual_enckey ser_keytype_config_ptr.manual_enckey
#define ser_manual_authkey ser_keytype_config_ptr.manual_authkey
#define ser_manual_ispi ser_keytype_config_ptr.manual_ispi
#define ser_manual_ospi ser_keytype_config_ptr.manual_ospi

/* for ipsec advanced setting */
#define ser_ipsec_opmode ser_ipsecadv_config_ptr.ipsec_opmode
#define ser_ipsecp1_enc ser_ipsecadv_config_ptr.ipsecp1_enc  							
#define ser_ipsecp1_auth ser_ipsecadv_config_ptr.ipsecp1_auth
#define ser_ipsecp1_group ser_ipsecadv_config_ptr.ipsecp1_group
#define ser_ipsecp1_klife ser_ipsecadv_config_ptr.ipsecp1_klife 
#define ser_ipsecp2_group ser_ipsecadv_config_ptr.ipsecp2_group
#define ser_ipsecp2_klife ser_ipsecadv_config_ptr.ipsecp2_klife
#define ser_ipsec_netbios ser_ipsecadv_config_ptr.ipsec_netbios
#define ser_ipsec_antireplay ser_ipsecadv_config_ptr.ipsec_antireplay
#define ser_ipsec_keepalive ser_ipsecadv_config_ptr.ipsec_keepalive
#define ser_ipsec_blockip ser_ipsecadv_config_ptr.ipsec_blockip
#define ser_ipsec_retrytimes ser_ipsecadv_config_ptr.ipsec_retrytimes
#define ser_ipsec_blocksecs ser_ipsecadv_config_ptr.ipsec_blocksecs

#endif
