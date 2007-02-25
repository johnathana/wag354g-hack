#ifndef _IPSEC_RC_H
#define _IPSEC_RC_H

#define IPSEC_TUNNELS 5

#define ENABLE  1
#define DISABLE 0

struct rc_entry_config
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
} rc_entry_config_ptr;

struct rc_ltype_config
{
	char l_ipaddr[20];
	char l_netmask[20];
} rc_ltype_config_ptr;

struct rc_rtype_config
{
	char rc_ipaddr[20];
	char rc_netmask[20];
} rc_rtype_config_ptr;

struct rc_sg_config
{
	char sg_ipaddr[20];
	char sg_domain[50];
} rc_sg_config_ptr;

struct rc_keytype_config
{
	char auto_pfs[3];
	char auto_presh[100];
	char auto_klife[20];
	char manual_enckey[100];
	char manual_authkey[100];
	char manual_ispi[100];
	char manual_ospi[100];
} rc_keytype_config_ptr;

struct rc_ipsecadv_config
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
} rc_ipsecadv_config_ptr;


#define rc_tunnel_status rc_entry_config_ptr.tunnel_status
#define rc_tunnel_name rc_entry_config_ptr.tunnel_name
#define rc_local_type rc_entry_config_ptr.local_type
#define rc_localgw_conn rc_entry_config_ptr.localgw_conn
#define rc_remote_type rc_entry_config_ptr.remote_type
#define rc_sg_type rc_entry_config_ptr.sg_type
#define rc_enc_type rc_entry_config_ptr.enc_type
#define rc_auth_type rc_entry_config_ptr.auth_type
#define rc_key_type rc_entry_config_ptr.key_type

#define rc_l_ipaddr rc_ltype_config_ptr.l_ipaddr
#define rc_l_netmask rc_ltype_config_ptr.l_netmask

#define rc_r_ipaddr rc_rtype_config_ptr.rc_ipaddr
#define rc_r_netmask rc_rtype_config_ptr.rc_netmask

#define rc_sg_ipaddr rc_sg_config_ptr.sg_ipaddr
#define rc_sg_domain rc_sg_config_ptr.sg_domain

#define rc_auto_pfs rc_keytype_config_ptr.auto_pfs
#define rc_auto_presh rc_keytype_config_ptr.auto_presh
#define rc_auto_klife rc_keytype_config_ptr.auto_klife
#define rc_manual_enckey rc_keytype_config_ptr.manual_enckey
#define rc_manual_authkey rc_keytype_config_ptr.manual_authkey
#define rc_manual_ispi rc_keytype_config_ptr.manual_ispi
#define rc_manual_ospi rc_keytype_config_ptr.manual_ospi

/* for ipsec advanced setting */
#define rc_ipsec_opmode rc_ipsecadv_config_ptr.ipsec_opmode
#define rc_ipsecp1_enc rc_ipsecadv_config_ptr.ipsecp1_enc  							
#define rc_ipsecp1_auth rc_ipsecadv_config_ptr.ipsecp1_auth
#define rc_ipsecp1_group rc_ipsecadv_config_ptr.ipsecp1_group
#define rc_ipsecp1_klife rc_ipsecadv_config_ptr.ipsecp1_klife 
#define rc_ipsecp2_group rc_ipsecadv_config_ptr.ipsecp2_group
#define rc_ipsecp2_klife rc_ipsecadv_config_ptr.ipsecp2_klife
#define rc_ipsec_netbios rc_ipsecadv_config_ptr.ipsec_netbios
#define rc_ipsec_antireplay rc_ipsecadv_config_ptr.ipsec_antireplay
#define rc_ipsec_keepalive rc_ipsecadv_config_ptr.ipsec_keepalive
#define rc_ipsec_blockip rc_ipsecadv_config_ptr.ipsec_blockip
#define rc_ipsec_retrytimes rc_ipsecadv_config_ptr.ipsec_retrytimes
#define rc_ipsec_blocksecs rc_ipsecadv_config_ptr.ipsec_blocksecs

#endif
