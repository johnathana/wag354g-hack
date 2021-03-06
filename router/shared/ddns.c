
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
        {"dyn_good",	"DDNS の更新は完了しました"},
        {"dyn_noupdate","DDNS の更新は完了しました"},
	{"dyn_nohost",	"ホスト名が存在しません"},
	{"dyn_notfqdn",	"ドメイン名が正しくありません"},
	{"dyn_!yours",	"ユーザー名が正しくありません"},
	{"dyn_abuse",	"ホスト名は DDNS サーバによりブロックされています"},
	{"dyn_nochg",	"DDNS の更新は完了しました"},
	{"dyn_badauth",	"認証に失敗しました (ユーザー名またはパスワード)"},
	{"dyn_badsys",	"システム パラメータが不正です"},
	{"dyn_badagent","このユーザー エージェントはブロックされています"},
	{"dyn_numhost",	"ホストが多すぎるか少なすぎます"},
	{"dyn_dnserr",	"DNS エラー発生"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","リクエストされた機能は寄付した場合にのみ有効です。寄付をしてください。"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"DynDns からの不明なリターン コード"},

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
	{"all_closed",	"DDNS サーバは現在クローズしています"},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"DDNS は無効です"};
	{"all_noip",		"インターネットに接続されていません"},
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
        {"dyn_good",	"Dynamic DNSの更新をしました"},
        {"dyn_noupdate","Dynamic DNSの更新をしました"},
	{"dyn_nohost",	"ホスト名がありません"},
	{"dyn_notfqdn",	"ドメイン名が正しくありません"},
	{"dyn_!yours",	"ユーザー名が正しくありません"},
	{"dyn_abuse",	"ホスト名はDynamic DNSサーバに止められていますr"},
	{"dyn_nochg",	"Dynamic DNSの更新をしました"},
	{"dyn_badauth",	"認証に失敗しました(ユーザー名またはパスワード)"},
	{"dyn_badsys",	"システムの値が無効です"},
	{"dyn_badagent","ユーザーアカウントが止められています"},
	{"dyn_numhost",	"複数のホストが見つかりました"},
	{"dyn_dnserr",	"DNSエラーです"},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","この機能は登録したユーザーのみ有効です。登録をしてください"},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"不明なコードがDynamic DNSから返されました "},

	// Below is for all
	{"all_closed",	"Dynamic DNSサーバに接続できません"},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"Dynamic DNS は無効です"},
	{"all_noip",	"インターネットに接続されていません"},
};

#elif LANGUAGE == KOREAN && OEM == REENET
struct ddns_message ddns_messages[] = {
	// Below is DynDNS error code
        {"dyn_good",	"DDNS ｾ�ｵ･ﾀﾌﾆｮｰ｡ ｿﾏｷ盞ﾆｽﾀｴﾏｴﾙ."},
        {"dyn_noupdate","DDNS ｾ�ｵ･ﾀﾌﾆｮｰ｡ ｿﾏｷ盞ﾆｽﾀｴﾏｴﾙ."},
	{"dyn_nohost",	"ﾁｸﾀ酩ﾏﾁ� ｾﾊｴﾂ ﾈ｣ｽｺﾆｮ ﾀﾌｸｧﾀﾔｴﾏｴﾙ."},
	{"dyn_notfqdn",	"ｵｵｸﾞﾀﾎ ﾀﾌｸｧﾀﾌ ｿﾃｹﾙｸ｣ﾁ� ｾﾊｽﾀｴﾏｴﾙ."},
	{"dyn_!yours",	"DDNS ID ｰ｡ ｿﾃｹﾙｸ｣ﾁ� ｾﾊｽﾀｴﾏｴﾙ."},
	{"dyn_abuse",	"ﾈ｣ｽｺﾆｮ ﾀﾌｸｧﾀﾌ DNS ｼｭｹ�ｿ｡ｼｭ ｸｷﾇ�ﾀﾖｽﾀｴﾏｴﾙ."},
	{"dyn_nochg",	"DDNS ｾ�ｵ･ﾀﾌﾆｮｰ｡ ｿﾏｷ盞ﾆｽﾀｴﾏｴﾙ."},
	{"dyn_badauth",	"ﾀﾎﾁ� ｽﾇﾆﾐ(ID ｶﾇｴﾂ ｺ�ｹﾐ ｹ�ﾈ｣)"},
	{"dyn_badsys",	"ｽﾃｽｺﾅﾛﾀﾇ ｸﾅｰｳ ｺｯｼ�ｰ｡ ｹｫﾈｿ ｵﾆｽﾀｴﾏｴﾙ."},
	{"dyn_badagent","ﾀﾌ ｻ鄙�ﾀﾚｴﾂ ｻ鄙� ﾇﾒ ｼ� ｾ�ｽﾀｴﾏｴﾙ."},
	{"dyn_numhost",	"ｴ� ｸｹｰﾅｳｪ ｴ� ﾀ�ﾀｺ ﾈ｣ｽｺﾆｮｸｦ ﾃ｣ｾﾒｽﾀｴﾏｴﾙ."},
	{"dyn_dnserr",	"DNS ﾃ豬ｹ ｿ｡ｷｯﾀﾔｴﾏｴﾙ."},
	{"dyn_911",	"An unexpected error (1)"},
	{"dyn_999",	"An unexpected error (2)"},
	{"dyn_!donator","ｿ莖ｸｵﾈ ﾆｯﾂ｡ﾀｺ ｿﾀﾁ� ｻ鄙� ｰ｡ｴﾉﾀﾚｿ｡ｰﾔ ｺﾎｿｩ ｵﾋｴﾏｴﾙ. ｻ鄙� ｰ｡ｴﾉﾄﾉ ﾇﾏｽﾊｽﾃｿﾀ."},
	{"dyn_strange",	"Strange server response, are you connecting to the right server?"},
	{"dyn_uncode",	"DDNSｿ｡ｼｭ ｾﾋｷﾁﾁ�ﾁ� ｾﾊﾀｺ ﾄﾚｵ蟠ﾂ ｵｹｾﾆｿﾉｴﾏｴﾙ."},

	// Below is for all
	{"all_closed",	"DDNS ｼｭｹ�ｰ｡ ﾁ�ｱﾝ ｴﾝﾇ�ﾀﾖｽﾀｴﾏｴﾙ."},
	{"all_resolving",	"Domain name resolve fail"},
	{"all_errresolv",	"Domain name resolve fail"},
	{"all_connecting",	"Connect to server fail"},
	{"all_connectfail",	"Connect to server fail"},
	{"all_disabled",	"DDNS ｱ箒ﾉﾀｻ ｻ鄙� ｾﾈﾇﾕｴﾏｴﾙ."},
	{"all_noip",	"ｿｬｰ� ｵﾇﾁ� ｾﾊｾﾒｽﾀｴﾏｴﾙ."},
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



