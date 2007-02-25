
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
#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <broadcom.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <cyutils.h>

#ifdef HSIAB_SUPPORT
#include <hsiab.h>
#include <hsiabutils.h>

extern struct Sessioninfo si;
extern struct register_deviceResponse register_device(struct deviceinfo *di);       // a
extern int do_ssl;

struct register_deviceResponse RegDevReply;
struct Sessioninfo *siptr;
struct deviceinfo di;
int register_status;
int new_device;

static int
get_object_id(webs_t wp)
{
	char object_id[80];
	unsigned int hwaddr[6];

	if(nvram_invmatch("hsiab_device_id","") && nvram_match("hsiab_registered","1")){
		return websWrite(wp, "%s", nvram_safe_get("hsiab_device_id"));
	}

	sscanf(nvram_safe_get("et0macaddr"), "%x:%x:%x:%x:%x:%x", &hwaddr[0], &hwaddr[1], &hwaddr[2], &hwaddr[3], &hwaddr[4], &hwaddr[5]);

	snprintf(object_id, sizeof(object_id),"%02X%02X%02X%02X%02X%02X/%ld", hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5], time(0));
	
	return websWrite(wp,"%s", object_id);
}

static int
get_admin_url(webs_t wp)
{
	return websWrite(wp,"%s", RegDevReply.registerinfo.admin_url);
}

int
ej_get_hsiab_value(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	
        if (ejArgs(argc, argv, "%s", &type) < 1) {
                return FALSE;
        }
	
	if(!strcmp(type,"object_id"))
		get_object_id(wp);
	else if(!strcmp(type,"admin_url"))
		get_admin_url(wp);
	else if(!strcmp(type,"status")){
		if(nvram_match("hsiab_mode", "0"))
			websWrite(wp, "Disabled");
		else{
			websWrite(wp, "%s", is_exist(HSIAB_LINK) ? "Connected" : "Disconnected");
			if(nvram_invmatch("hsiab_registered", "1"))
				websWrite(wp, " (not yet register)");
		}
	}

	return TRUE;
	
}

int
init_hsiabd(void)
{
	FILE *fp;

	mkdir("/tmp/hsiab", 0777);
	
	if ((fp = fopen(SOAP_CONFIG, "w"))) {
		if(nvram_match("hsiab_provider","2")){
                        fprintf(fp,"REGISTER=%s/register/,urn:Register\n", nvram_safe_get("hsiab_manual_reg_ops"));
			fprintf(fp,"SESSION=%s/session/,urn:Session\n", nvram_safe_get("hsiab_manual_reg_ops"));
			fprintf(fp,"CONFIG=%s/config/,urn:Config\n", nvram_safe_get("hsiab_manual_reg_ops"));
		}
                else{
                        fprintf(fp,"REGISTER=%s/register/,urn:Register\n", nvram_safe_get("hsiab_register_ops"));
			fprintf(fp,"SESSION=%s/session/,urn:Session\n", nvram_safe_get("hsiab_session_ops"));
			fprintf(fp,"CONFIG=%s/config/,urn:Config\n", nvram_safe_get("hsiab_config_ops"));
		}
		fprintf(fp,"%cPROXY_HOST=%s\n", nvram_match("hsiab_proxy_host","") ? '#' : ' ' , nvram_safe_get("hsiab_proxy_host"));
		fprintf(fp,"%cPROXY_PORT=%s\n", nvram_match("hsiab_proxy_port","") ? '#' : ' ' , nvram_safe_get("hsiab_proxy_port"));
		fclose(fp);		
	}
	else{
		fprintf(stderr, "Cann't open %s\n", SOAP_CONFIG);
		return FALSE;
	}
	
	if ((fp = fopen(HOSTS_FILE, "w"))) {
		fprintf(fp, "66.103.68.25	test1.hotspot.boingo.com\n");
		fprintf(fp, "66.103.68.25	hsiab.boingo.com\n");
		fclose(fp);
	}
	else{
		fprintf(stderr, "Cann't open %s\n", HOSTS_FILE);
		return FALSE;
	}
	
	return TRUE;
}


int
hsiab_register(webs_t wp)
{
	char *object_id = websGetVar(wp, "hsiab_object_id", NULL);
	char *ops_url = websGetVar(wp, "hsiab_ops_url", NULL);
	char *provider= websGetVar(wp, "hsiab_provider", NULL);
	char *submit_type= websGetVar(wp, "submit_type", NULL);
	char base_admin_url[80];
	char return_url[254], *cur = return_url;
	int i;

	if(submit_type && !strcmp(submit_type, "save")){
		nvram_set("hsiab_provider", provider);
		if(!strcmp(provider, "0")){
			nvram_set("hsiab_mode", "0");
			return DISABLE_HSIAB;
		}
		else{
			nvram_set("hsiab_mode", "1");
			if(nvram_match("hsiab_provider", "2"))
				nvram_set("hsiab_manual_reg_ops", ops_url);
			return RESTART_HSIAB;
		}
	}

	siptr = &si;

	di.vendor	=	strdup("Linksys");
	di.product	=	strdup(nvram_safe_get("router_name"));
	di.firmware	=	CYBERTAN_VERSION;
	di.hsiab_proto_version=	strdup("1.1");
    
	di.object_id	=	object_id;
	di.public_ip	=	strdup(nvram_safe_get("wan_ipaddr"));

	/* We want to confirm that the user use LAN or WAN (Remote management) to register. */
	if(legal_ip_netmask("lan_ipaddr", "lan_netmask", nvram_safe_get("http_client_ip")) == TRUE){	// Use LAN to register
		snprintf(return_url, sizeof(return_url), "%s://%s/apply.cgi?submit_button=finish_registration&action=Apply", 
			 do_ssl ? "https" : "http",  
			 nvram_safe_get("lan_ipaddr"));
	}
	else{	// Use WAN to register
		snprintf(return_url, sizeof(return_url), "%s://%s:%s/apply.cgi?submit_button=finish_registration&action=Apply",
			 do_ssl ? "https" : "http",  
			 nvram_safe_get("wan_ipaddr"), 
			 nvram_safe_get("http_wanport"));
	}
	
	di.hsiab_return_url=	cur; 
	
	fprintf(stderr, "Calling register_device and requesting device password.\n");
	fprintf(stderr,"object_id=[%s]\n", di.object_id);
        //printf("client_config_ip=[%s]\n", di.client_config_ip);	// remove from v1.3
        fprintf(stderr, "hsiab_return_url=[%s]\n", di.hsiab_return_url);
        fprintf(stderr, "public_ip=[%s]\n\n", di.public_ip);
        //printf("hsiab_ops_url=[%s]\n\n", di.ops_url);
	nvram_set("hsiab_provider", provider);
	if(nvram_match("hsiab_provider", "2"))
		nvram_set("hsiab_manual_reg_ops", ops_url);

	/* Generate new config file */
	init_hsiabd();
	unlink(HSIAB_MSG);

	if(!check_wan_link(0)){
		buf_to_file(HSIAB_MSG, "Register fail. No Internet connection!");
		register_status = -1;
		goto Exit;
	}

	RegDevReply = register_device(&di);
	if (strlen(siptr->fault) > 1) {
		buf_to_file(HSIAB_MSG, siptr->fault);
		register_status = -1;
		goto Exit;
	}
	register_status = 1;
	if(strstr(RegDevReply.registerinfo.admin_url, "newdevice=true"))
		new_device = 1;
	else
		new_device = 0;
	
	fprintf(stderr, "Got device password: %s\n", RegDevReply.registerinfo.device_password);
	fprintf(stderr, "Got admin_url: %s\n", RegDevReply.registerinfo.admin_url);
	//printf("Got new_device: %d\n", RegDevReply.registerinfo.new_device);		// remove from v1.3
	if(!RegDevReply.registerinfo.device_password || !strncmp(RegDevReply.registerinfo.device_password, "(null)", 6)){
		buf_to_file(HSIAB_MSG, "Register fail. Cann't get device password!");
		register_status = -1;
		goto Exit;
	}

	/* get base admin url from admin_url */
	for(i=0; RegDevReply.registerinfo.admin_url[i]!='?' && i<strlen(RegDevReply.registerinfo.admin_url);i++)
		base_admin_url[i] = RegDevReply.registerinfo.admin_url[i];
        base_admin_url[i] ='\0';
		 
	nvram_set("hsiab_device_id", di.object_id);
	nvram_set("hsiab_device_password", RegDevReply.registerinfo.device_password);
	
	nvram_set("hsiab_admin_url", base_admin_url);

	if(new_device)
		nvram_set("hsiab_mode","0");

	return TRUE;
	
Exit:	
	if(di.vendor)	free(di.vendor);
	if(di.product)	free(di.product);
	if(di.hsiab_proto_version)	free(di.hsiab_proto_version);
	if(di.public_ip)	free(di.public_ip);

	return FALSE;
}

int
hsiab_finish_registration(webs_t wp)
{
	fprintf(stderr, "HSIAB finish_registration\n");
	nvram_set("hsiab_registered","1");
	nvram_set("hsiab_configured","1");
	nvram_set("hsiab_mode","1");
	
	return TRUE;
}

int
hsiab_register_ok(webs_t wp)
{
	fprintf(stderr, "HSIAB restart\n");
	nvram_set("hsiab_registered","1");
	nvram_set("hsiab_configured","1");

	return TRUE;
}

int
ej_show_hsiab_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	FILE *fp;
	int link = 0;
	
	if((fp = fopen(HSIAB_LINK,"r"))){
		link = 1;
		fclose(fp);
	}
	
ret += websWrite(wp,"<TR>\n\
    <TH align=right width=166 bgColor=#6666cc height=16>&nbsp;</TH>\n\
    <TD width=554 bgColor=white height=16 colspan=2><HR color=#CCCCCC></TD></TR>");
ret += websWrite(wp,"<TR>\n\
    <TH align=right width=166 bgColor=#6666cc height=25>HSIAB:&nbsp;&nbsp;</TH>\n\
    <TD width=554 bgColor=white height=25 colspan=2>&nbsp;&nbsp;<b>Status: </b>%s</TD></TR>", link ? "On-line" : "Off-line");
ret += websWrite(wp,"<TR>\n\
    <TH align=right width=166 bgColor=#6666cc height=25>&nbsp;</TH>\n\
    <TD width=554 bgColor=white height=25 colspan=2>&nbsp;&nbsp;<b>Object id: </b>%s</TD></TR>", nvram_safe_get("hsiab_device_id"));

ret += websWrite(wp,"<TR>\n\
    <TH align=right width=166 bgColor=#6666cc height=25>&nbsp;</TH>\n\
    <TD width=554 bgColor=white height=30 colspan=2>&nbsp;<INPUT type=radio value=1 name=hsiab_mode %s>\n\
      <B>Enable&nbsp; </B> <INPUT type=radio value=0 name=hsiab_mode %s>\n\
      <B>Disable&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</B>",
      nvram_match("hsiab_mode","1") ? "checked" : "", nvram_match("hsiab_mode","0") ? "checked" : "");

	if(nvram_match("hsiab_registered","1")){
ret += websWrite(wp,"\n\
    <br>&nbsp;&nbsp;<INPUT onclick=showRegister(this.form) type=button value=\"Re-register\">&nbsp;&nbsp;<INPUT onclick=self.open('Config.asp','Help','alwaysRaised,resizable,scrollbars,width=660,height=460').focus() type=button value=\"View Config\">&nbsp;&nbsp;<INPUT onclick=self.open('DB.asp','Help','alwaysRaised,resizable,scrollbars,width=800,height=500').focus() type=button value=\"View Status\">&nbsp;&nbsp;<INPUT onclick=\"document.location.href='https://hsiab.boingo.com/admin/'\" type=button value=\"HSIAB Admin\"> </TD></TR>",
    nvram_match("hsiab_mode","1") ? "checked" : "", nvram_match("hsiab_mode","0") ? "checked" : "");
	}
	else{
ret += websWrite(wp,"\n\
    <INPUT onclick=showRegister(this.form) type=button value=\"Register\"></TD></TR>");
	}

	return ret;
}

int
ej_show_hsiab_config(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	FILE *fp;
	char line[254];

        if ((fp = fopen(HSIAB_CONFIG_TXT, "r"))) {
		while( fgets(line, sizeof(line), fp) != NULL ) {
			ret += websWrite(wp, "%s<br>", line);
		}
		fclose(fp);
	}
	else
		fprintf(stderr, "Cann't open %s\n", HSIAB_CONFIG_TXT);

	return ret;	
}

int
ej_dump_hsiab_db(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	struct database *myDB;
	int count = 0, i;

	myDB = open_DB1(&count);

	for(i=0 ; i<count ; i++){

ret += websWrite(wp, "%c'%s','%s','%s','%s','%s','%s','%d','%d','%ld','%ld','%ld','%ld','%ld','%ld','%ld'\n", i ? ',' : ' ', 
			myDB[i].username, 
			myDB[i].mac, 
			myDB[i].ip, 
			myDB[i].adminkey, 
			myDB[i].userkey,
			myDB[i].state,
			myDB[i].pid,
			myDB[i].lock,
			myDB[i].start_time,
			myDB[i].stop_time,
			myDB[i].timestamp,
			myDB[i].packets_out,
			myDB[i].packets_in,
			myDB[i].bytes_out,
			myDB[i].bytes_in);
	}

	if(myDB)	free(myDB);
	return ret;	
}

int
ej_dump_hsiab_msg(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[MAX_BUF_LEN];
	int ret;
	
	file_to_buf(HSIAB_MSG, buf, sizeof(buf));

	ret = websWrite(wp, "%s", buf);

	return ret;
}

#endif
