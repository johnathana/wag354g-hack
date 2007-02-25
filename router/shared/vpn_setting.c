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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <broadcom.h>
#include <cymac.h>

#include "ipsec.h"

extern int selchanged;
extern int sub_changed;
	
char *key_type_local; //flag for show_connect_setting

int ej_show_ltype_setting(int eid, webs_t wp, int argc, char_t **argv)
{

	int ret = 0;
	char *local_type_local;
	if(!gozila_action || selchanged)
	{
	//	selchanged = 0;
		local_type_local = local_type;
	}
	else
	{
		local_type_local = websGetVar(wp, "local_type", NULL);
	}

	if(!strcmp(local_type_local, "0")) 
		do_ej("ipsec_lipaddr.asp",wp);
	else if(!strcmp(local_type_local, "1")) 
		do_ej("ipsec_lsubnet.asp",wp);
	
	return ret;

}

 int
ej_show_rtype_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *remote_type_local;
	if(!gozila_action || selchanged)
	{
//		selchanged = 0;
		remote_type_local = remote_type;
	}
	else
		remote_type_local = websGetVar(wp, "remote_type", NULL);

	if(!strcmp(remote_type_local, "0")) 
		do_ej("ipsec_ripaddr.asp",wp);
	else if(!strcmp(remote_type_local, "1")) 
		do_ej("ipsec_rsubnet.asp",wp);
	else if(!strcmp(remote_type_local,"4"))	//ipsec remote type is host
		do_ej("ipsec_rhost.asp",wp);
	else if(!strcmp(remote_type_local,"3"))	//ipsec remote type is any
		do_ej("ipsec_rany.asp",wp);
	
	return ret;
}

 int
ej_show_sg_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *rg_type;

	if(!gozila_action || selchanged)
	{
//		selchanged = 0;
		rg_type = sg_type;
	}
	else
		rg_type = websGetVar(wp, "sg_type", NULL);

	if(!strcmp(rg_type,"0"))  //ipsec remote security gateway type is ipaddr
		do_ej("ipsec_sipaddr.asp",wp);
	else if(!strcmp(rg_type,"2"))  //ipsec remote security gateway type is domain
		do_ej("ipsec_sdomain.asp",wp);
	else if(!strcmp(rg_type,"1"))	//ipsec remote security gateway type is any
		do_ej("ipsec_sany.asp",wp);
	

	return ret;
}

/* zhangbin 2005.3.31 for remote id type in Cloud */
#ifdef CLOUD_SUPPORT
	
int
ej_show_localid_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *local_id_l;

	if(!gozila_action || selchanged)
	{
		local_id_l = ipsec_local_id;
	}
	else
		local_id_l = websGetVar(wp, "local_id", NULL);

	if(!strcmp(local_id_l,"0"))  //local id type is ipaddr
		do_ej("ipsec_lidip.asp",wp);
	else if(!strcmp(local_id_l,"1"))  //local id type is fqdn
		do_ej("ipsec_lidfqdn.asp",wp);

	return ret;
}

int
ej_show_remoteid_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *remote_id_l;

	if(!gozila_action || selchanged)
	{
		remote_id_l = ipsec_remote_id;
	}
	else
		remote_id_l = websGetVar(wp, "remote_id", NULL);

	if(!strcmp(remote_id_l,"0"))  //remote id type is ipaddr
		do_ej("ipsec_ridip.asp",wp);
	else if(!strcmp(remote_id_l,"1"))  //remote id type is fqdn
		do_ej("ipsec_ridfqdn.asp",wp);

	return ret;
}
#endif

 int
ej_show_keytype_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

	if(!gozila_action || selchanged)
	{
//		selchanged = 0;
		key_type_local = key_type;
	}
	else
		key_type_local = websGetVar(wp, "key_type", NULL);

	printf("in show_keytype:key_type_local=%s\n",key_type_local);
	if(!strcmp(key_type_local,"0"))  //ipsec key type is auto key(IKE)
		do_ej("ipsec_autokey.asp",wp);
	else if(!strcmp(key_type_local,"1"))  //ipsec key type is manual key
		do_ej("ipsec_manualkey.asp",wp);
	

	return ret;
}
 int
ej_show_connect_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
/*	char *key_type_local;

	if(!gozila_action || selchanged)
	{
		printf("gozila=%d,selchanged=%d\n",gozila_action,selchanged);
		key_type_local = key_type;
		printf("!gozila!\n");
	}
	else
	{
		printf("gozila=%d,selchanged=%d\n",gozila_action,selchanged);
		key_type_local = websGetVar(wp, "key_type", NULL);
		printf("local,key_type_local=%s\n",key_type_local);
	}
		printf("key_type_local=%s\n",key_type_local);*/
	if(!strcmp(key_type_local,"0"))  //ipsec key type is auto key(IKE)
		do_ej("ipsec_connect.asp",wp);
	else if(!strcmp(key_type_local,"1"))  // key type is manual key
		do_ej("ipsec_noconnect.asp",wp);
/*
	if(!strcmp(key_type,"0"))  //ipsec key type is auto key(IKE)
		do_ej("ipsec_connect.asp",wp);
	else if(!strcmp(key_type,"1"))  // key type is manual key
		do_ej("ipsec_noconnect.asp",wp);
*/
	return ret;
}
