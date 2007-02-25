/******************************************************************************
* 
*Copyright(c) 2005, CyberTAN Technology
*
*All rights reserved.
*
*FILENAME          : nomad.h
*
*ABSTRACT          : support quickVPN function for linksys.This file supply the
*                    data and declare some functions for relative files. 
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.03.31
******************************************************************************/
#ifndef __NOMAD_H
#define __NOMAD_H

#ifdef NOMAD_SUPPORT

/* ************************* key define position *************************** */
/* MAX length of the bug */
#define MAX_NAME                  36 //MAX of Name byte number
#define MAX_IPADDR                20 //MAX of IP Address chars
#define MAX_NOMAD                 5  //MAX of quickvpn users

/* the return CODES for QuickVPN Client, please read the document Nomad
   Security Gateway Functional Specification for detail specification */
#define NOMAD_SUCCESS             0
#define NOMAD_UNKNOWN_USER        -1
#define NOMAD_NO_USED             -2
#define NOMAD_FAILED_PASSWD       -3
#define NOMAD_FAILED_HOSTNAME     -4
#define NOMAD_FAILED_CONN         -5
#define NOMAD_NO_CONN             -6
#define NOMAD_FAILED_VERSION      -7
#define NOMAD_CORRUPT_FILE        -8
#define NOMAD_EROPEN_FILE         -9
#define NOMAD_ERREAD_FILE         -10
#define NOMAD_EROPEN_VPN          -11
#define NOMAD_EROPEN_IPSEC        -12
#define NOMAD_ERCALL_IPSEC        -13
#define NOMAD_ERSERVER_DNS        -14
#define NOMAD_ERSEARCH_DNS        -15
#define NOMAD_ERDEF_FILE          -17
#define NOMAD_FAILED_PROCESS      -18
#define NOMAD_ERLONG_SECRET       -19
#define NOMAD_ERMSG_VERSION       -25
#define NOMAD_CONFLICT_SUBNET     -26
#define NOMAD_IP_USED             -27
#define NOMAD_SERVER_ERR          -111

/* Info message about Communication Types */ 
#define QV_START                  1 //start connection
#define QV_STOP                   2 //stop connection
#define QV_CHANGE                 3 //change password
#define QV_STATE                  4 //query user's state
/* ***************************************************************************/

/* ************************ Struct define position ************************* */
struct mymsgbuf                    //lay up some global variables for nomad
{
	int nRtCode;                   //the result Status message from the URL
	int nusrindex;                 //the index of RoadWarrior User Database
	char changpasswd[2];           //the flag of whether can change user password
	char pcinfo_ip[MAX_IPADDR];    //the ip address that StartConnection message take
	char packet_ip[MAX_IPADDR];    //the src ip address that https message arrived from
	char randompsk[MAX_NAME];      //RandomPSK
	char username[MAX_NAME];       //the user name of RoadWarrior User Database
	char xchpasswd[MAX_NAME];      //the password of RoadWarrior User Database
};

typedef struct vpnmsg              //suply some info for creating or deleting quickVPN tunnel
{
	char post_nat_ip[MAX_IPADDR];  //the src ip address that https message arrived from
	char pre_nat_ip[MAX_IPADDR];   //the ip address that StartConnection message take
	char random_psk[MAX_NAME];     //RandomPSK
	char username[MAX_NAME];       //the user name of RoadWarrior User Database
}quick_vpn_t;
/* ***************************************************************************/

/* *************************** Proc extern position ************************ */
extern void init_quickvpn_mkdir(void);
extern void send_authenticate( char* realm);
extern int valid_remote_ip(void);
extern int gui_stopnomad_conn(int n_index);
extern int random_seed(int number, int npend);
extern int clear_quickvpn(quick_vpn_t *del, int index);
extern int start_quickvpn(quick_vpn_t *p_vpn, int index);
extern int nomad_auth_check( char* dirname, char* authorization);
extern int b64_decode( const char* str, unsigned char* space, int size);
extern char *nomad_info_parse(char *path, int *nRtCode, int *len, int *nUrlType);
/* ***************************************************************************/

#endif	/* NOMAD_SUPPORT */

#endif	/* __NOMAD_H */
