
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>

#include <broadcom.h>

#define WAN_IPADDR "/tmp/status/wan_ipaddr"

struct ddns_message {
	char *name;
	char *desc;
};

extern char connecton[5];

#if OEM == LINKSYS
#if LANGUAGE == JAPANESE
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
        {"dyn_good",	"DDNS ‚ÌXV‚ÍŠ®—¹‚µ‚Ü‚µ‚½"},
        {"dyn_noupdate","DDNS ‚ÌXV‚ÍŠ®—¹‚µ‚Ü‚µ‚½"},
	{"dyn_nohost",	"ƒzƒXƒg–¼‚ª‘¶İ‚µ‚Ü‚¹‚ñ"},
	{"dyn_notfqdn",	"ƒhƒƒCƒ“–¼‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ"},
	{"dyn_!yours",	"ƒ†[ƒU[–¼‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ"},
	{"dyn_abuse",	"ƒzƒXƒg–¼‚Í DDNS ƒT[ƒo‚É‚æ‚èƒuƒƒbƒN‚³‚ê‚Ä‚¢‚Ü‚·"},
	{"dyn_nochg",	"DDNS ‚ÌXV‚ÍŠ®—¹‚µ‚Ü‚µ‚½"},
	{"dyn_badauth",	"”FØ‚É¸”s‚µ‚Ü‚µ‚½ (ƒ†[ƒU[–¼‚Ü‚½‚ÍƒpƒXƒ[ƒh)"},
	{"dyn_badsys",	"ƒVƒXƒeƒ€ ƒpƒ‰ƒ[ƒ^‚ª•s³‚Å‚·"},
	{"dyn_badagent","‚±‚Ìƒ†[ƒU[ ƒG[ƒWƒFƒ“ƒg‚ÍƒuƒƒbƒN‚³‚ê‚Ä‚¢‚Ü‚·"},
	{"dyn_numhost",	"ƒzƒXƒg‚ª‘½‚·‚¬‚é‚©­‚È‚·‚¬‚Ü‚·"},
	{"dyn_dnserr",	"DNS ƒGƒ‰[”­¶"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","ƒŠƒNƒGƒXƒg‚³‚ê‚½‹@”\‚ÍŠñ•t‚µ‚½ê‡‚É‚Ì‚İ—LŒø‚Å‚·BŠñ•t‚ğ‚µ‚Ä‚­‚¾‚³‚¢B"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"DynDns ‚©‚ç‚Ì•s–¾‚ÈƒŠƒ^[ƒ“ ƒR[ƒh"},

	// Below is TZO error code
        {"tzo_good",	"Operation Complete"},
        {"tzo_noupdate","Operation Complete"},
	{"tzo_notfqdn",	"Invalid Domain Name"},
        {"tzo_notmail",	"Invalis Email"},
        {"tzo_notact",	"Invalid Action"},
        {"tzo_notkey",	"Invalid Key"},
        {"tzo_notip",	"Invalid IP address"},
        {"tzo_dupfqdn",	"Duplicate Domain Name"},
        {"tzo_fqdncre",	"Domain Name has already been created for this domain name"},
        {"tzo_expired",	"The account has expired"},
        {"tzo_error",	"An unexpected server error"},

	// Below is for all
	{"all_closed",	"DDNS ƒT[ƒo‚ÍŒ»İƒNƒ[ƒY‚µ‚Ä‚¢‚Ü‚·"},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"DDNS ‚Í–³Œø‚Å‚·"};
	{"all_noip",		"ƒCƒ“ƒ^[ƒlƒbƒg‚ÉÚ‘±‚³‚ê‚Ä‚¢‚Ü‚¹‚ñ"},
};
#else
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
    #ifdef MULTILANG_SUPPORT
	//Dom 2005/11/14 add to js
	{"dyn_good",	"<script>Capture(ddnsm.dyn_good)</script>"},
	{"dyn_noupdate","<script>Capture(ddnsm.dyn_noupdate)</script>"},
	{"dyn_nochg",	"<script>Capture(ddnsm.dyn_nochg)</script>"},
	{"dyn_badauth",	"<script>Capture(ddnsm.dyn_badauth)</script>"},
	{"all_noip",	"<script>Capture(ddnsm.all_noip)</script>"},
	{"dyn_!yours",	"<script>Capture(ddnsm.dyn_yours)</script>"},
	{"dyn_nohost",	"<script>Capture(ddnsm.dyn_nohost)</script>"},
	{"dyn_notfqdn",	"<script>Capture(ddnsm.dyn_notfqdn)</script>"},
	{"dyn_abuse",	"<script>Capture(ddnsm.dyn_abuse)</script>"},
	{"dyn_badsys",	"<script>Capture(ddnsm.dyn_badsys)</script>"},
	{"dyn_badagent","<script>Capture(ddnsm.dyn_badagent)</script>"},
	{"dyn_numhost",	"<script>Capture(ddnsm.dyn_numhost)</script>"},
	{"dyn_dnserr",	"<script>Capture(ddnsm.dyn_dnserr)</script>"},
	{"dyn_911",	"<script>Capture(ddnsm.dyn_911)</script>"},
	{"dyn_999",	"<script>Capture(ddnsm.dyn_999)</script>"},
	{"dyn_!donator","<script>Capture(ddnsm.dyn_donator)</script>"},
	{"dyn_strange",	"<script>Capture(ddnsm.dyn_strange)</script>"},
	{"dyn_uncode",	"<script>Capture(ddnsm.dyn_uncode)</script>"},

	// Below is TZO error code
	{"tzo_good",	"<script>Capture(ddnsm.tzo_good)</script>"},
	{"tzo_noupdate","<script>Capture(ddnsm.tzo_noupdate)</script>"},
	{"tzo_notfqdn",	"<script>Capture(ddnsm.tzo_notfqdn)</script>"},
	{"tzo_notmail",	"<script>Capture(ddnsm.tzo_notmail)</script>"},
	{"tzo_notact",	"<script>Capture(ddnsm.tzo_notact)</script>"},
	{"tzo_notkey",	"<script>Capture(ddnsm.tzo_notkey)</script>"},
	{"tzo_notip",	"<script>Capture(ddnsm.tzo_notip)</script>"},
	{"tzo_dupfqdn",	"<script>Capture(ddnsm.tzo_dupfqdn)</script>"},
	{"tzo_fqdncre",	"<script>Capture(ddnsm.tzo_fqdncre)</script>"},
	{"tzo_expired",	"<script>Capture(ddnsm.tzo_expired)</script>"},
	{"tzo_error",		"<script>Capture(ddnsm.tzo_error)</script>"},

	// Below is for all
	{"all_closed",	   "<script>Capture(ddnsm.all_closed)</script>"},
	{"all_resolving",  "<script>Capture(ddnsm.all_resolving)</script>"},
	{"all_errresolv",  "<script>Capture(ddnsm.all_errresolv)</script>"},
	{"all_connecting", "<script>Capture(ddnsm.all_connecting)</script>"},
	{"all_connectfail","<script>Capture(ddnsm.all_connectfail)</script>"},
	{"all_disabled",   "<script>Capture(ddnsm.all_disabled)</script>"},
    #else
	{"dyn_good",    "DDNS is updated successfully"},
	{"dyn_noupdate","DDNS is updated successfully"},
	{"dyn_nochg",   "DDNS is updated successfully"},
	{"dyn_badauth", "Authorization fails (username or passwords)"},
	{"all_noip",	"No Internet connection"},
	{"dyn_!yours",	"Username is not correct"},
	{"dyn_nohost",	"The hostname does not exist"},
	{"dyn_notfqdn",	"Domain Name is not correct"},
	{"dyn_abuse",	"The hostname is blocked by the DDNS server"},
	{"dyn_badsys",	"The system parameters are invalid"},
	{"dyn_badagent","This useragent has been blocked"},
	{"dyn_numhost",	"Too many or too few hosts found"},
	{"dyn_dnserr",	"DNS error encountered"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","A feature requested is only available to donators, please donate"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"Unknown return code"},

	// Below is TZO error code
        {"tzo_good",	"Operation Complete"},
	{"tzo_noupdate","Operation Complete"},
	{"tzo_notfqdn",	"Invalid Domain Name"},
        {"tzo_notmail",	"Invalis Email"},
        {"tzo_notact",	"Invalid Action"},
        {"tzo_notkey",	"Invalid Key"},
        {"tzo_notip",	"Invalid IP address"},
        {"tzo_dupfqdn",	"Duplicate Domain Name"},
        {"tzo_fqdncre",	"Domain Name has already been created for this domain name"},
        {"tzo_expired",	"The account has expired"},
        {"tzo_error",	"An unexpected server error"},

	// Below is for all
	{"all_closed",	   "DDNS server is currently closed"},
	{"all_resolving",  "Domain name resolve fail"},
	{"all_errresolv",  "Domain name resolve fail"},
	{"all_connecting", "Connect to server fail"},
	{"all_connectfail","Connect to server fail"},
	{"all_disabled",   "DDNS function is disabled"},
    #endif
};
#endif
#elif LANGUAGE == JAPANESE && OEM == PCI
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
        {"dyn_good",	"Dynamic DNS‚ÌXV‚ğ‚µ‚Ü‚µ‚½"},
        {"dyn_noupdate","Dynamic DNS‚ÌXV‚ğ‚µ‚Ü‚µ‚½"},
	{"dyn_nohost",	"ƒzƒXƒg–¼‚ª‚ ‚è‚Ü‚¹‚ñ"},
	{"dyn_notfqdn",	"ƒhƒƒCƒ“–¼‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ"},
	{"dyn_!yours",	"ƒ†[ƒU[–¼‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ"},
	{"dyn_abuse",	"ƒzƒXƒg–¼‚ÍDynamic DNSƒT[ƒo‚É~‚ß‚ç‚ê‚Ä‚¢‚Ü‚·r"},
	{"dyn_nochg",	"Dynamic DNS‚ÌXV‚ğ‚µ‚Ü‚µ‚½"},
	{"dyn_badauth",	"”FØ‚É¸”s‚µ‚Ü‚µ‚½(ƒ†[ƒU[–¼‚Ü‚½‚ÍƒpƒXƒ[ƒh)"},
	{"dyn_badsys",	"ƒVƒXƒeƒ€‚Ì’l‚ª–³Œø‚Å‚·"},
	{"dyn_badagent","ƒ†[ƒU[ƒAƒJƒEƒ“ƒg‚ª~‚ß‚ç‚ê‚Ä‚¢‚Ü‚·"},
	{"dyn_numhost",	"•¡”‚ÌƒzƒXƒg‚ªŒ©‚Â‚©‚è‚Ü‚µ‚½"},
	{"dyn_dnserr",	"DNSƒGƒ‰[‚Å‚·"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","‚±‚Ì‹@”\‚Í“o˜^‚µ‚½ƒ†[ƒU[‚Ì‚İ—LŒø‚Å‚·B“o˜^‚ğ‚µ‚Ä‚­‚¾‚³‚¢"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"•s–¾‚ÈƒR[ƒh‚ªDynamic DNS‚©‚ç•Ô‚³‚ê‚Ü‚µ‚½ "},

	// Below is for all
	{"all_closed",	"Dynamic DNSƒT[ƒo‚ÉÚ‘±‚Å‚«‚Ü‚¹‚ñ"},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"Dynamic DNS ‚Í–³Œø‚Å‚·"},
	{"all_noip",	"ƒCƒ“ƒ^[ƒlƒbƒg‚ÉÚ‘±‚³‚ê‚Ä‚¢‚Ü‚¹‚ñ"},
};

#elif LANGUAGE == KOREAN && OEM == REENET
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
        {"dyn_good",	"DDNS ¾÷µ¥ÀÌÆ®°¡ ¿Ï·áµÆ½À´Ï´Ù."},
        {"dyn_noupdate","DDNS ¾÷µ¥ÀÌÆ®°¡ ¿Ï·áµÆ½À´Ï´Ù."},
	{"dyn_nohost",	"Á¸ÀçÇÏÁö ¾Ê´Â È£½ºÆ® ÀÌ¸§ÀÔ´Ï´Ù."},
	{"dyn_notfqdn",	"µµ¸ŞÀÎ ÀÌ¸§ÀÌ ¿Ã¹Ù¸£Áö ¾Ê½À´Ï´Ù."},
	{"dyn_!yours",	"DDNS ID °¡ ¿Ã¹Ù¸£Áö ¾Ê½À´Ï´Ù."},
	{"dyn_abuse",	"È£½ºÆ® ÀÌ¸§ÀÌ DNS ¼­¹ö¿¡¼­ ¸·ÇôÀÖ½À´Ï´Ù."},
	{"dyn_nochg",	"DDNS ¾÷µ¥ÀÌÆ®°¡ ¿Ï·áµÆ½À´Ï´Ù."},
	{"dyn_badauth",	"ÀÎÁõ ½ÇÆĞ(ID ¶Ç´Â ºñ¹Ğ ¹øÈ£)"},
	{"dyn_badsys",	"½Ã½ºÅÛÀÇ ¸Å°³ º¯¼ö°¡ ¹«È¿ µÆ½À´Ï´Ù."},
	{"dyn_badagent","ÀÌ »ç¿ëÀÚ´Â »ç¿ë ÇÒ ¼ö ¾ø½À´Ï´Ù."},
	{"dyn_numhost",	"´õ ¸¹°Å³ª ´õ ÀûÀº È£½ºÆ®¸¦ Ã£¾Ò½À´Ï´Ù."},
	{"dyn_dnserr",	"DNS Ãæµ¹ ¿¡·¯ÀÔ´Ï´Ù."},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","¿ä±¸µÈ Æ¯Â¡Àº ¿ÀÁ÷ »ç¿ë °¡´ÉÀÚ¿¡°Ô ºÎ¿© µË´Ï´Ù. »ç¿ë °¡´ÉÄÉ ÇÏ½Ê½Ã¿À."},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"DDNS¿¡¼­ ¾Ë·ÁÁöÁö ¾ÊÀº ÄÚµå´Â µ¹¾Æ¿É´Ï´Ù."},

	// Below is for all
	{"all_closed",	"DDNS ¼­¹ö°¡ Áö±İ ´İÇôÀÖ½À´Ï´Ù."},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"DDNS ±â´ÉÀ» »ç¿ë ¾ÈÇÕ´Ï´Ù."},
	{"all_noip",	"¿¬°á µÇÁö ¾Ê¾Ò½À´Ï´Ù."},
};
#else
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
    #ifdef MULTILANG_SUPPORT
	//Dom 2005/11/14 add to js
	{"dyn_good",	"<script>Capture(ddnsm.dyn_good)</script>"},
	{"dyn_noupdate","<script>Capture(ddnsm.dyn_noupdate)</script>"},
	{"dyn_nochg",	"<script>Capture(ddnsm.dyn_nochg)</script>"},
	{"dyn_badauth",	"<script>Capture(ddnsm.dyn_badauth)</script>"},
	{"all_noip",	"<script>Capture(ddnsm.all_noip)</script>"},
	{"dyn_!yours",	"<script>Capture(ddnsm.dyn_yours)</script>"},
	{"dyn_nohost",	"<script>Capture(ddnsm.dyn_nohost)</script>"},
	{"dyn_notfqdn",	"<script>Capture(ddnsm.dyn_notfqdn)</script>"},
	{"dyn_abuse",	"<script>Capture(ddnsm.dyn_abuse)</script>"},
	{"dyn_badsys",	"<script>Capture(ddnsm.dyn_badsys)</script>"},
	{"dyn_badagent","<script>Capture(ddnsm.dyn_badagent)</script>"},
	{"dyn_numhost",	"<script>Capture(ddnsm.dyn_numhost)</script>"},
	{"dyn_dnserr",	"<script>Capture(ddnsm.dyn_dnserr)</script>"},
	{"dyn_911",	"<script>Capture(ddnsm.dyn_911)</script>"},
	{"dyn_999",	"<script>Capture(ddnsm.dyn_999)</script>"},
	{"dyn_!donator","<script>Capture(ddnsm.dyn_donator)</script>"},
	{"dyn_strange",	"<script>Capture(ddnsm.dyn_strange)</script>"},
	{"dyn_uncode",	"<script>Capture(ddnsm.dyn_uncode)</script>"},

	// Below is for all
	{"all_closed",	   "<script>Capture(ddnsm.all_closed)</script>"},
	{"all_resolving",  "<script>Capture(ddnsm.all_resolving)</script>"},
	{"all_errresolv",  "<script>Capture(ddnsm.all_errresolv)</script>"},
	{"all_connecting", "<script>Capture(ddnsm.all_connecting)</script>"},
	{"all_connectfail","<script>Capture(ddnsm.all_connectfail)</script>"},
	{"all_disabled",   "<script>Capture(ddnsm.all_disabled)</script>"},
    #else
	{"dyn_good",    "DDNS is updated successfully"},
	{"dyn_noupdate","DDNS is updated successfully"},
	{"dyn_nochg",   "DDNS is updated successfully"},
	{"dyn_badauth", "Authorization fails (username or passwords)"},
	{"all_noip",	"No Internet connection"},
	{"dyn_!yours",	"Username is not correct"},
	{"dyn_nohost",	"The hostname does not exist"},
	{"dyn_notfqdn",	"Domain Name is not correct"},
	{"dyn_abuse",	"The hostname is blocked by the DDNS server"},
	{"dyn_badsys",	"The system parameters are invalid"},
	{"dyn_badagent","This useragent has been blocked"},
	{"dyn_numhost",	"Too many or too few hosts found"},
	{"dyn_dnserr",	"DNS error encountered"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","A feature requested is only available to donators, please donate"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"Unknown return code"},

	// Below is for all
	{"all_closed",	   "DDNS server is currently closed"},
	{"all_resolving",  "Domain name resolve fail"},
	{"all_errresolv",  "Domain name resolve fail"},
	{"all_connecting", "Connect to server fail"},
	{"all_connectfail","Connect to server fail"},
	{"all_disabled",   "DDNS function is disabled"},
    #endif
};
#endif

char *
convert(char *msg)
{
	static char buf[200];
	struct ddns_message *m;

	for(m = ddns_messages ; m < &ddns_messages[STRUCT_LEN(ddns_messages)] ; m++) {
		if(!strcmp(m->name, msg)){
			snprintf(buf, sizeof(buf), "%s", m->desc);
			return buf;
		}
	}

	snprintf(buf, sizeof(buf), "%s", msg);
	return buf;
}

int
ej_show_ddns_status(int eid, webs_t wp, int argc, char_t **argv)
{
	char string[80]="";
	int ret;
	char *enable = websGetVar(wp, "ddns_enable", NULL);
	
	if(!enable)
		enable = nvram_safe_get("ddns_enable");		// for first time

	if(strcmp(nvram_safe_get("ddns_enable"),enable))	// change service
		return websWrite(wp," ");

	if(nvram_match("ddns_enable","0")) // only for no hidden page
		return websWrite(wp, "%s", convert("all_disabled"));

	if(!check_wan_link(0))
		return websWrite(wp, "%s", convert("all_noip"));

	if(file_to_buf("/tmp/ddns_msg", string, sizeof(string))){
		if(!strcmp(string, "")){
			if(nvram_match("ddns_status","1")){
				if(nvram_match("ddns_enable","1"))
					//modify by junzhao 2004.3.10
					ret = websWrite(wp, "<FONT color=black>%s</FONT>", convert("dyn_good"));	// dyndns
				else
					ret = websWrite(wp, "<FONT color=black>%s</FONT>", convert("tzo_good"));	
					//ret = websWrite(wp, "%s", convert("tzo_good"));	// TZO
			}
			else
				ret = websWrite(wp, "%s", convert("all_closed"));
		}
		else
		{
			//junzhao 2004.3.16
			if(!strcmp(string, "tzo_good") || !strcmp(string, "dyn_good") || !strcmp(string, "tzo_noupdate") || !strcmp(string, "dyn_noupdate"))
				ret = websWrite(wp, "<FONT color=black>%s</FONT>", convert(string));	
			else			
				ret = websWrite(wp, "%s", convert(string));
		}
	}

	return ret;
}

int
ddns_save_value(webs_t wp)
{
	char *enable, *username, *passwd, *hostname;
	struct variable ddns_variables[] = {
		{ longname: "DDNS enable", argv: ARGV("0","1","2") },
		{ longname: "DDNS password", argv: ARGV("30") },
	}, *which;
	int ret=-1;
	char _username[] = "ddns_username_X";
	char _passwd[] = "ddns_passwd_X";
	char _hostname[] = "ddns_hostname_X";
			
	which = &ddns_variables[0];

	enable = websGetVar(wp, "ddns_enable", NULL);
	if(!enable && !valid_choice(wp, enable, &which[0])){
		error_value = 1;
		return 1;
	}

	if(atoi(enable) == 0)	{	// Disable
		nvram_set("ddns_enable_buf", nvram_safe_get("ddns_enable"));
		nvram_set("ddns_enable", enable);
		return 1;
	}
	else if(atoi(enable) == 1){	// dyndns
		snprintf(_username, sizeof(_username),"%s","ddns_username");
		snprintf(_passwd, sizeof(_passwd),"%s","ddns_passwd");
		snprintf(_hostname, sizeof(_hostname),"%s","ddns_hostname");
	}
	else if(atoi(enable) == 2){	// tzo
		snprintf(_username, sizeof(_username),"ddns_username_%s",enable);
		snprintf(_passwd, sizeof(_passwd),"ddns_passwd_%s",enable);
		snprintf(_hostname, sizeof(_hostname),"ddns_hostname_%s",enable);
	}

	username = websGetVar(wp, _username, NULL);
	passwd = websGetVar(wp, _passwd, NULL);
	hostname = websGetVar(wp, _hostname, NULL);

	if(!username || !passwd || !hostname){
		error_value = 1;
		return 1;
	}
	
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s():en=[%s] user=[%s] pwd=[%s] host=[%s]",__FUNCTION__,enable,username,passwd,hostname);
#endif	

	nvram_set("ddns_enable_buf", nvram_safe_get("ddns_enable"));
	nvram_set("ddns_enable", enable);
	nvram_set("ddns_username_buf", nvram_safe_get(_username));
	nvram_set("ddns_passwd_buf", nvram_safe_get(_passwd));
	nvram_set("ddns_hostname_buf", nvram_safe_get(_hostname));
	if (strcmp(passwd, TMP_PASSWD))
		nvram_set(_passwd, passwd);
	nvram_set(_username, username);
	nvram_set(_hostname, hostname);

	return ret;
}

int
ddns_update_value(webs_t wp)
{
	return 1;
}

int
ej_show_ddns_ip(int eid, webs_t wp, int argc, char_t **argv)
{
/*	
	if(check_wan_link(0))
		return websWrite(wp,"%s",nvram_safe_get("wan_ipaddr"));
	else
		return websWrite(wp,"0.0.0.0");
*/
//	int which = atoi(connection);
//	int which = 0;
	char word[16], *next, *ipaddr;
	char readbuf[130];

//junzhao 2004.11.3 for ddns at active conn
	int which = atoi(nvram_safe_get("wan_active_connection"));
	if(which == 8)
		return websWrite(wp,"0.0.0.0");
		
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("read %s file fail\n", WAN_IPADDR);
		return -1;
	}
			
	//foreach(word, nvram_safe_get("wan_ipaddr"), next)
	foreach(word, readbuf, next)
	{
		if(which -- == 0)
		{
			ipaddr = word;
			break;
		}
	}
	printf("ddns ipaddr %s\n", ipaddr);	
	return websWrite(wp,"%s",ipaddr);
	
}



