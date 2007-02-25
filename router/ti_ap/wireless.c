
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

/*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: wireless.c,v 1.1.1.3 2005/08/25 06:28:12 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <net/if.h>
#include <error.h>
#include <signal.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <broadcom.h>

/* zhangbin 2005.5.8 */
#undef CLOUD_SUPPORT

#define ASSOCLIST_TMP	"/tmp/.wl_assoclist"
#define OLD_NAME_IP	"/tmp/.old_name_ip"
#define ASSOCLIST_CMD	"wl assoclist"

#define LEASES_NAME_IP	"/tmp/.leases_name_ip"

/////wwzh
#include <ti_wireless/tiwlan_drv.h>      
#include <ti_wireless/ti_types.h>        
#include <ti_wireless/pform_types.h>     
#include <ti_wireless/whalDefs.h>        
#include <ti_wireless/app_driver_api.h>  
#include <ti_wireless/mgmtAccess.h>      
/////end by wwzh
//leijun
#include "iwlib.h"
#include "dda_wext_ioctl.h"
#include "dda_wpaauth_ioctl.h"


/*
  WEP Format: 
  wl_wep_buf=111:371A82447F:FBEA2AB7D4:1C9D814E6C:B4695172B4:2  (only for UI read)
  wl_wep_gen=111:371A82447F:FBEA2AB7D4:1C9D814E6C:B4695172B4:2  (only for UI read)
  wl_key=2
  wl_key1=371A82447F
  wl_key2=FBEA2AB7D4
  wl_key3=1C9D814E6C
  wl_key4=B4695172B4
  wl_passphrase=111 (only for UI)
  wl_wep_bit=64 (only for UI)

 */

extern unsigned char key128[4][13];
extern unsigned char key64[4][5];

struct wl_client_mac {
	unsigned char hostname[32];
	char ipaddr[20];
	char hwaddr[20];
	int status;		// 0:offline 1:online
	int check;
} wl_client_macs[MAX_LEASES];

struct lease_table {
	unsigned char hostname[32];
	char ipaddr[20];
	char hwaddr[20];
} *dhcp_lease_table;

int generate_key;
extern void gen_key(char *genstr, int weptype);
int nv_count;
extern struct variable variables[];
/* channel info structure */  //from 11.9
typedef struct {
        uint    chan;           /* channel number */
        uint    freq;           /* in Mhz */
} chan_info_t;

static chan_info_t chan_info[] = {
        /* A channels */
        /* 11a usa low */
        { 36,   5180},
        { 40,   5200},
        { 44,   5220},
        { 48,   5240},
        { 52,   5260},
        { 56,   5280},
        { 60,   5300},
        { 64,   5320},

        /* 11a Europe */
        { 100,  5500},
        { 104,  5520},
        { 108,  5540},
        { 112,  5560},
        { 116,  5580},
        { 120,  5600},
        { 124,  5620},
        { 128,  5640},
        { 132,  5660},
        { 136,  5680},
        { 140,  5700},

        /* 11a usa high */
        { 149,  5745},
        { 153,  5765},
        { 157,  5785},
        { 161,  5805},

        /* 11a japan */
        { 184,  4920},
        { 188,  4940},
        { 192,  4960},
        { 196,  4980},
        { 200,  5000},
        { 204,  5020},
        { 208,  5040},
        { 212,  5060},
        { 216,  5080}
};

void
validate_security_mode(webs_t wp, char *value, struct variable *v)
{
	char *security_mode_last = websGetVar(wp, "security_mode_last", NULL);	
	char *wl_wep_last = websGetVar(wp, "wl_wep_last", NULL);	
	int from_index_page = 0;
	char *wl_wep = NULL;

	//If you don't press "Edit Security Setting" to set some value, and direct select to enable "Wireless Security".
	//It'll returned, due to security_mode_buf is space.
	if(!strcmp(value, "enabled")){
	        if(nvram_match("security_mode_last", ""))	// from index.asp and first time
			return;
		else{
			if(!security_mode_last){			// from index.asp
				 from_index_page = 1;
				 value = nvram_safe_get("security_mode_last");
				 wl_wep = nvram_safe_get("wl_wep_last");
			}
			else{					// from WL_WPATable.asp page
				 value = websGetVar(wp, "security_mode_last", NULL);	
				 wl_wep = nvram_safe_get("wl_wep_last");
			}
		}
	}
	
	if (!valid_choice(wp, value, v))
		return;

	if(!strcmp(value,"disabled")){
		nvram_set("wl_auth_mode", "disabled");
		nvram_set("wl_wep", "off");
	}
	else if(!strcmp(value,"psk")){
		nvram_set("wl_auth_mode", "psk");
		if(wl_wep)
			nvram_set("wl_wep", wl_wep);
	}
	else if(!strcmp(value,"wpa")){
		nvram_set("wl_auth_mode", "wpa");
		if(wl_wep)
			nvram_set("wl_wep", wl_wep);
	}
	else if(!strcmp(value,"radius")){
		nvram_set("wl_auth_mode", "radius");
		nvram_set("wl_wep", "restricted");
	}
	else if(!strcmp(value,"wep")){
		nvram_set("wl_auth_mode", "disabled");
		nvram_set("wl_wep", "restricted");
	}

	if(security_mode_last)
		nvram_set("security_mode_last", security_mode_last);
	
	if(wl_wep_last)
		nvram_set("wl_wep_last", wl_wep_last);

	nvram_set(v->name, value);
}

void
validate_wl_key(webs_t wp, char *value, struct variable *v)
{
	char *c;

	switch (strlen(value)) {
	case 5:
	case 13:			
		break;
	case 10:
	case 26:
		for (c = value; *c; c++) {
			if (!isxdigit(*c)) {
				websDebugWrite(wp, "Invalid <b>%s</b>: character %c is not a hexadecimal digit<br>", v->longname, *c);
				return;
			}
		}
		break;
	default:
		websDebugWrite(wp, "Invalid <b>%s</b>: must be 5 or 13 ASCII characters or 10 or 26 hexadecimal digits<br>", v->longname);
		return;
	}

	nvram_set(v->name, value);
}

void
validate_wl_wep(webs_t wp, char *value, struct variable *v)
{
	if (!valid_choice(wp, value, v))
		return;
	/*	// not to check, spec for linksys
	if (!strcmp(value, "on") || !strcmp(value, "wep")) {
		char wl_key[] = "wl_keyXXX";

		snprintf(wl_key, sizeof(wl_key), "wl_key%s", nvram_safe_get("wl_key"));
		if (!strlen(nvram_safe_get(wl_key))) {
			websDebugWrite(wp, "Invalid <b>%s</b>: must first specify a valid <b>Network Key</b><br>", v->longname);
			return;
		}
	}
	*/
	nvram_set(v->name, value);
}

void
validate_auth_mode(webs_t wp, char *value, struct variable *v)
{
	if (!valid_choice(wp, value, v))
		return;
/*
	if (!strcmp(value, "radius") || !strcmp(value, "wpa")) {
		if (nvram_match("wl_radius_ipaddr", "")) {
			websDebugWrite(wp, "Invalid <b>%s</b>: must first specify a valid <b>RADIUS Server</b><br>", v->longname);
			return;
		}
	}

	if (!strcmp(value, "psk")) {
		if (nvram_match("wl_wpa_psk", "")) {
			websDebugWrite(wp, "Invalid <b>%s</b>: must first specify a valid <b>WPA Pre-Shared Key</b><br>", v->longname);
			return;
		}
	} 

	if (!strcmp(value, "wpa") || !strcmp(value, "psk")) {
		if (!nvram_match("wl_wep", "tkip") && !nvram_match("wl_wep", "aes")) &&
		    !nvram_match("wl_wep", "tkip+aes")) {
			websDebugWrite(wp, "Invalid <b>%s</b>: <b>Data Encryption</b> mode must be TKIP or AES<br>", v->longname);
			return;
		}
		nvram_set("wl_auth", "0");
	} else {
		if (!nvram_match("wl_wep", "off") && !nvram_match("wl_wep", "wep")) {
			websDebugWrite(wp, "Invalid <b>%s</b>: <b>Data Encryption</b> mode must be Off or WEP or TKIP+AES<br>", v->longname);
			return;
		}
	}			
*/
	nvram_set(v->name, value);
}

void
validate_wpa_psk(webs_t wp, char *value, struct variable *v)
{
	int len = strlen(value);
	char *c;

	if (len == 64) {
		for (c = value; *c; c++) {
			if (!isxdigit((int) *c)) {
				websDebugWrite(wp, "Invalid <b>%s</b>: character %c is not a hexadecimal digit<br>", v->longname, *c);
				return;
			}
		}
	} else if (len < 8 || len > 63) {
		websDebugWrite(wp, "Invalid <b>%s</b>: must be between 8 and 63 ASCII characters or 64 hexadecimal digits<br>", v->longname);
		return;
	}

	nvram_set(v->name, value);
}

/* Hook to write wl_* default set through to wl%d_* variable set */
void
wl_unit(webs_t wp, char *value, struct variable *v)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	struct nvram_tuple cur[100], *last = cur, *t;

	/* Do not write through if no interfaces are present */
	if (atoi(value) < 0)
		return;

	/* Set prefix */
	snprintf(prefix, sizeof(prefix), "wl%d_", atoi(value));

	/* Write through to selected variable set */
	for (; v >= variables && !strncmp(v->name, "wl_", 3); v--) {
		/* Do not interleave get and set (expensive on Linux) */
		a_assert(last < &cur[ARRAYSIZE(cur)]);
		last->name = v->name;
		last->value = nvram_safe_get(v->name);
		last++;
	}

	for (t = cur; t < last; t++)
		nvram_set(strcat_r(prefix, &t->name[3], tmp), t->value);
}

void
validate_wl_wep_key(webs_t wp, char *value, struct variable *v)
{
	char buf[200] = "";
	struct variable wl_wep_variables[] = {
		{ longname: "Passphrase", argv: ARGV("16") },
		{ longname: "WEP Key", argv: ARGV("5","10") },		//for 64 bit
		{ longname: "WEP Key", argv: ARGV("13","26") },		//for 128 bit
		{ longname: "Default TX Key", argv: ARGV("1","4") },
	}, *which;

	char *wep_bit="", *wep_passphrase="", *wep_key1="", *wep_key2="", *wep_key3="", *wep_key4="", *wep_tx="";
	char new_wep_passphrase[50]="", new_wep_key1[30]="", new_wep_key2[30]="", new_wep_key3[30]="", new_wep_key4[30]="";
	int index;

	which = &wl_wep_variables[0];

	wep_bit = websGetVar(wp, "wl_wep_bit", NULL);	//64 or 128
	if(!wep_bit)	return ;
	if(strcmp(wep_bit,"64") && strcmp(wep_bit,"128"))	return ;
	
	wep_passphrase = websGetVar(wp, "wl_passphrase", "");
	//if(!wep_passphrase)	return ;

	//strip_space(wep_passphrase);
	if(strcmp(wep_passphrase,"")){
		if(!valid_name(wp, wep_passphrase, &which[0])){
			error_value = 1;
		}
		else{
		        filter_name(wep_passphrase, new_wep_passphrase, sizeof(new_wep_passphrase), SET);
                }
	}

	wep_key1 = websGetVar(wp, "wl_key1", "");
	wep_key2 = websGetVar(wp, "wl_key2", "");
	wep_key3 = websGetVar(wp, "wl_key3", "");
	wep_key4 = websGetVar(wp, "wl_key4", "");
	wep_tx = websGetVar(wp, "wl_key", NULL);

	if(!wep_tx){
		error_value = 1;
		return ;
	}

	index = (atoi(wep_bit) == 64) ? 1 : 2;

	if(strcmp(wep_key1,"")){
		if(!valid_wep_key(wp, wep_key1, &which[index])){
			error_value = 1;
       		}
		else{
		        filter_name(wep_key1, new_wep_key1, sizeof(new_wep_key1), SET);
                }
		 
	}
	if(strcmp(wep_key2,"")){
		if(!valid_wep_key(wp, wep_key2, &which[index])){
			error_value = 1;
       		}
		else{
		        filter_name(wep_key2, new_wep_key2, sizeof(new_wep_key2), SET);
                }
	}
	if(strcmp(wep_key3,"")){
		if(!valid_wep_key(wp, wep_key3, &which[index])){
			error_value = 1;
       		}
		else{
		        filter_name(wep_key3, new_wep_key3, sizeof(new_wep_key3), SET);
                }
	}
	if(strcmp(wep_key4,"")){
		if(!valid_wep_key(wp, wep_key4, &which[index])){
			error_value = 1;
       		}
		else{
		        filter_name(wep_key4, new_wep_key4, sizeof(new_wep_key4), SET);
                }
	}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s): passphrase=[%s] key1=[%s] key2=[%s] key3=[%s] key4=[%s] tx=[%s]",__FUNCTION__,wep_bit,wep_passphrase,wep_key1,wep_key2,wep_key3,wep_key4,wep_tx);
#endif

	if(!error_value){
		snprintf(buf, sizeof(buf),"%s:%s:%s:%s:%s:%s",new_wep_passphrase,new_wep_key1,new_wep_key2,new_wep_key3,new_wep_key4,wep_tx);
		nvram_set("wl_wep_bit",wep_bit);
		nvram_set("wl_wep_buf", buf);

		nvram_set("wl_passphrase",wep_passphrase);
		nvram_set("wl_key",wep_tx);
		nvram_set("wl_key1",wep_key1);
		nvram_set("wl_key2",wep_key2);
		nvram_set("wl_key3",wep_key3);
		nvram_set("wl_key4",wep_key4);

		if(!strcmp(wep_key1,"") && !strcmp(wep_key2,"") && !strcmp(wep_key3,"") && !strcmp(wep_key4,""))	// Allow null wep
			nvram_set("wl_wep","off");
		else
			nvram_set("wl_wep","restricted");
	}

}

void
validate_wl_auth(webs_t wp, char *value, struct variable *v)
{
	if (!valid_choice(wp, value, v))
		return;
	/*  // not to check , spec for linksys
	if (atoi(value) == 1) {
		char wl_key[] = "wl_keyXXX";

		snprintf(wl_key, sizeof(wl_key), "wl_key%s", nvram_safe_get("wl_key"));
		if (!strlen(nvram_safe_get(wl_key))) {
			websDebugWrite(wp, "Invalid <b>%s</b>: must first specify a valid <b>Network Key</b><br>", v->longname);
			return;
		}
	}
	*/
	nvram_set(v->name, value);
}

void
validate_d11_channel(webs_t wp, char *value, struct variable *v)
{
	char *country = nvram_safe_get("wl_country");
	int channel = atoi(value), min = 0, max = 0, i;

	if ((!strcmp(v->name, "d11b_channel")) || (!strcmp(v->name, "d11g_channel"))) {
		if (!strcmp(country, "Japan") || !strcmp(country, "Thailand")) {
			min = 1; max = 14;
		} else if (!strcmp(country, "Jordan")) {
			min = 10; max = 13;
		} else if (!strcmp(country, "Israel")) {
			min = 5; max = 7;
		} else {
			min = 1; max = 13;
		}

		if (channel < min || channel > max) {
			websDebugWrite(wp, "Invalid <b>%s</b>: valid %s channels are %d-%d<br>", v->longname, country, min, max);
			return;
		}
	 } else if (!strcmp(v->name, "d11a_channel")) {
                for (i = 0; i < ARRAYSIZE(chan_info); i++) {
                        if (chan_info[i].chan == channel)
                                break;
                }

                if (i >= ARRAYSIZE(chan_info)) {
                        websDebugWrite(wp, "Invalid <b>%s</b>: valid %s channels are ", v->longname, country);
                        for (i = 0; i < ARRAYSIZE(chan_info); i++) {
                                channel = chan_info[i].chan;
                                websWrite(wp, "%d%s", channel, channel == 216 ? "<br>" : "/");
                        }
                        return;
                }
        }


	nvram_set(v->name, value);
}
#ifdef SUPPORT_11b
void
validate_d11b_rate(webs_t wp, char *value, struct variable *v)
{
	char *country = nvram_safe_get("wl_country");
	int channel = atoi(nvram_safe_get("d11b_channel"));

	if (!strcmp(country, "Japan") && channel == 14) {
		if (atoi(value) > 2000000) {
			websDebugWrite(wp, "Invalid <b>%s</b>: valid rates in Japan on channel 14 are Auto, 1 Mbps, and 2 Mbps<br>", v->longname);
			return;
		}
	}

	if (!valid_choice(wp, value, v))
		return;

	nvram_set(v->name, value);
}

void
validate_d11b_rateset(webs_t wp, char *value, struct variable *v)
{
	char *country = nvram_safe_get("wl_country");
	int channel = atoi(nvram_safe_get("d11b_channel"));

	if (!strcmp(country, "Japan") && channel == 14) {
		if (!strcmp(value, "all")) {
			websDebugWrite(wp, "Invalid <b>%s</b>: valid rate set in Japan on channel 14 is Default<br>", v->longname);
			return;
		}
	}

	if (!valid_choice(wp, value, v))
		return;

	nvram_set(v->name, value);
}
#endif

char
*wl_filter_mac_get(char *type, int which)
{
	static char word[50];
	char *wordlist, *next;
	int temp;

	if(!strcmp(nvram_safe_get("wl_active_add_mac"),"1")){
		//cprintf("%s(): wl_active_add_mac = 1\n",__FUNCTION__);
		wordlist = nvram_safe_get("wl_active_mac");
	}
	else{
		//cprintf("%s(): wl_active_add_mac = 0\n",__FUNCTION__);
		wordlist = nvram_safe_get("wl_mac_list");
	}
	
	if(!wordlist)	return "";	
	
	temp = which;

	foreach(word, wordlist, next) {
		if (which-- == 0) {

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): mac=[%s]",__FUNCTION__,temp,word);
#endif
			return word;
		}
	}
	return "";

}

/* Example:
 * 00:11:22:33:44:55=1 00:12:34:56:78:90=0 (ie 00:11:22:33:44:55 if filterd, and 00:12:34:56:78:90 is not)
 * wl_maclist = "00:11:22:33:44:55"
 */
void
validate_wl_hwaddrs(webs_t wp, char *value, struct variable *v)
{
	int i;
	char buf[1000] = "", *cur = buf;
	char *wordlist;
	unsigned char m[6];
	wordlist = nvram_safe_get("wl_mac_list");
	if(!wordlist)	return;
//wwzh

	tiwlan_SetAccessDelEntry();


	for (i=0; i<WL_FILTER_MAC_NUM * WL_FILTER_MAC_PAGE ; i++) {
		char filter_mac[] = "wl_macXXX";
		char *mac=NULL, *mac1=NULL;

 		snprintf(filter_mac, sizeof(filter_mac), "%s%d","wl_mac", i);

		mac = websGetVar(wp, filter_mac, NULL);


		if(!mac || !strcmp(mac,"0") || !strcmp(mac,"")){
			continue;
		}
		//strip_space(mac);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): [%s]=[%s]",__FUNCTION__,i,filter_mac,mac);
#endif
		if(strlen(mac) == 12){
			sscanf(mac,"%02X%02X%02X%02X%02X%02X",(uint *)&m[0],(uint *)&m[1],(uint *)&m[2],(uint *)&m[3],(uint *)&m[4],(uint *)&m[5]);	
			mac1 = malloc(20);
			sprintf(mac1,"%02X:%02X:%02X:%02X:%02X:%02X",m[0],m[1],m[2],m[3],m[4],m[5]);
		}
		else if(strlen(mac) == 17){
			sscanf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",(uint *)&m[0],(uint *)&m[1],(uint *)&m[2],(uint *)&m[3],(uint *)&m[4],(uint *)&m[5]);	
			mac1 = malloc(20);
			sprintf(mac1,"%02X:%02X:%02X:%02X:%02X:%02X",m[0],m[1],m[2],m[3],m[4],m[5]);
		}

		if (!valid_hwaddr(wp, mac1, v)){
			error_value = 1;
			if(mac1)	free(mac1);
			continue;
		}
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", mac1);

	}


	if(!error_value){
		nvram_set(v->name, buf);
		nvram_set("wl_maclist", buf);
		nvram_set("wl_active_mac", "");
	}
}

int
ej_wireless_filter_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret=0;
	char *type;
	int item;
#if LANGUAGE == JAPANESE
	int box_len=20;
#else
	int box_len=17;
#endif

	char *mac_mess="MAC";

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	if(!strcmp(type,"input")){
		for(i=0 ; i<WL_FILTER_MAC_NUM/2 ; i++){
			
			item = 0*WL_FILTER_MAC_NUM + i+1 ;
ret += websWrite(wp," \
	<TR> \n\
          <TD align=right height=30 width=\"54\"> <FONT face=Arial size=-1><B>%s %02d:</B></FONT></TD> \n\
          <TD align=left height=30><b>&nbsp; </b> \n\
                <INPUT maxLength=17 onBlur=valid_macs_all(this) size=%d name=\"wl_mac%d\" value=\"%s\"></TD> \n\
          <TD align=left height=30 width=\"16\">&nbsp;</TD>\n",mac_mess, item, box_len, item-1, wl_filter_mac_get("mac",item-1));
ret += websWrite(wp," \
          <TD align=right height=30 width=\"54\"> <FONT face=Arial size=-1><B>%s %02d:</B></FONT></TD> \n\
          <TD align=left height=30>&nbsp;  \n\
                <INPUT maxLength=17 onBlur=valid_macs_all(this) size=%d name=\"wl_mac%d\" value=\"%s\"></TD> \n\
        </TR>\n",mac_mess, item + (WL_FILTER_MAC_NUM/2), box_len, item + (WL_FILTER_MAC_NUM/2) -1, wl_filter_mac_get("mac",item+(WL_FILTER_MAC_NUM/2)-1));

	 }
//wwzh
#if 0
ret += websWrite(wp," \
	<TR> \n\
	<TD align=right height=30 colspan=5>\n\
		<HR color=#c0c0c0 align=right>\n\
	</TD></TR>\n");
		for(i=0 ; i<WL_FILTER_MAC_NUM/2 ; i++){
			
			item = 1*WL_FILTER_MAC_NUM + i+1 ;
ret += websWrite(wp," \
	<TR> \n\
          <TD align=right height=30 width=\"54\"> <FONT face=Arial size=-1><B>%s %02d:</B></FONT></TD> \n\
          <TD align=left height=30><b>&nbsp; </b> \n\
                <INPUT maxLength=17 onBlur=valid_macs_all(this) size=%d name=\"wl_mac%d\" value=\"%s\"></TD> \n\
          <TD align=left height=30 width=\"16\">&nbsp;</TD>\n",mac_mess, item, box_len, item-1, wl_filter_mac_get("mac",item-1));
ret += websWrite(wp," \
          <TD align=right height=30 width=\"54\"> <FONT face=Arial size=-1><B>%s %02d:</B></FONT></TD> \n\
          <TD align=left height=30>&nbsp;  \n\
                <INPUT maxLength=17 onBlur=valid_macs_all(this) size=%d name=\"wl_mac%d\" value=\"%s\"></TD> \n\
        </TR>\n",mac_mess, item + (WL_FILTER_MAC_NUM/2), box_len, item + (WL_FILTER_MAC_NUM/2) -1 , wl_filter_mac_get("mac",item+(WL_FILTER_MAC_NUM/2)-1));

		}
#endif //end by wwzh

	}
	
	//cprintf("%s():set wl_active_add_mac = 0\n",__FUNCTION__);
	nvram_set("wl_active_add_mac","0");
	return ret;
}

int
add_active_mac(webs_t wp)
{
	char buf[1000] = "", *cur = buf;
	int i, count=0;

	nvram_set("wl_active_add_mac","1");
		
	for (i=0; i<MAX_LEASES ; i++) {
		char active_mac[] = "onXXX";
		char *index = NULL;

 		snprintf(active_mac, sizeof(active_mac), "%s%d","on", i);
		index = websGetVar(wp, active_mac, NULL);
		if(!index)	continue;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"[%s]=[%s]",active_mac,index);
#endif
		count++;
		
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", wl_client_macs[atoi(index)].hwaddr);
	}
	for (i=0; i<MAX_LEASES ; i++) {
		char active_mac[] = "offXXX";
		char *index;

 		snprintf(active_mac, sizeof(active_mac), "%s%d","off", i);
		index = websGetVar(wp, active_mac, NULL);
		if(!index)	continue;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"[%s]=[%s]",active_mac,index);
#endif
		count++;
		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", wl_client_macs[atoi(index)].hwaddr);
	}
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): count=[%d]",__FUNCTION__,count);
#endif
	nvram_set("wl_active_mac",buf);
	return 0;	
}


int
dhcp_lease_table_init(void)
{
	char sigusr1[] = "-XX";
	FILE *fp, *fp_w;
	struct lease_t lease;
	struct in_addr addr;
	char mac[20]="";
	int count = 0;

        sprintf(sigusr1, "-%d", SIGUSR1);
        eval("killall", sigusr1, "udhcpd");

	fp_w = fopen(LEASES_NAME_IP, "w");

	// Parse leases file 
	if ((fp = fopen("/tmp/udhcpd.leases", "r"))) {
		while (fread(&lease, sizeof(lease), 1, fp)) {
			snprintf(mac,sizeof(mac),"%02X:%02X:%02X:%02X:%02X:%02X",lease.chaddr[0],lease.chaddr[1],lease.chaddr[2],lease.chaddr[3],lease.chaddr[4],lease.chaddr[5]);
			if(!strcmp("00:00:00:00:00:00",mac))	continue;
		
			addr.s_addr = lease.yiaddr;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): hostname=[%s] hwaddr=[%s] ipaddr=[%s]",
			__FUNCTION__, count,
			lease.hostname, mac, inet_ntoa(addr));
#endif
			fprintf(fp_w,"%s %s %s\n",mac,inet_ntoa(addr),lease.hostname);
			count++;
		}
		fclose(fp);
	}
	fclose(fp_w);

	return count;
}

void
save_hostname_ip(void)
{
	FILE *fp, *fp_w;
	char line[80];
	char leases[3][50];
	int i=0, j=0,count;
	int match = 0;
	struct wl_client {
		unsigned char hostname[32];
		char ipaddr[20];
		char hwaddr[20];
	} wl_clients[MAX_LEASES];

	for(i=0 ; i<MAX_LEASES ; i++){				// init	value
		strcpy(wl_clients[i].hostname,"");
		strcpy(wl_clients[i].ipaddr,"");
		strcpy(wl_clients[i].hwaddr,"");
	}
	i = 0;
	if ((fp = fopen(OLD_NAME_IP, "r"))) {
		while( fgets(line, sizeof(line), fp) != NULL ) {
			// 00:11:22:33:44:55 192.168.1.100 honor
			strcpy(leases[0],"");
			strcpy(leases[1],"");
			strcpy(leases[2],"");
			if(sscanf(line,"%s %s %s",leases[0],leases[1],leases[2]) != 3)
				continue;
			snprintf(wl_clients[i].hwaddr, sizeof(wl_clients[i].hwaddr), "%s", leases[0]);
			snprintf(wl_clients[i].ipaddr, sizeof(wl_clients[i].ipaddr), "%s", leases[1]);
			snprintf(wl_clients[i].hostname, sizeof(wl_clients[i].hostname), "%s", leases[2]);
			i++;
		}
		fclose(fp);
		
	}
	count = i;

	for(i=0  ; i<nv_count ; i++){				// init	value
		if(wl_client_macs[i].status == 1 && strcmp(wl_client_macs[i].ipaddr,"")){	// online && have ip address
			for(j=0;j<MAX_LEASES;j++){
				match = 0;
				if(!strcmp(wl_clients[j].hwaddr,wl_client_macs[i].hwaddr)){
					snprintf(wl_clients[j].ipaddr, sizeof(wl_clients[j].ipaddr), "%s", wl_client_macs[i].ipaddr);
					snprintf(wl_clients[j].hostname, sizeof(wl_clients[j].hostname), "%s", wl_client_macs[i].hostname);
					match = 1;
					break;
				}

			}
			if(match == 0){
				snprintf(wl_clients[count].hwaddr, sizeof(wl_clients[i].hwaddr), "%s", wl_client_macs[i].hwaddr);
				snprintf(wl_clients[count].ipaddr, sizeof(wl_clients[i].ipaddr), "%s", wl_client_macs[i].ipaddr);
				snprintf(wl_clients[count].hostname, sizeof(wl_clients[i].hostname), "%s", wl_client_macs[i].hostname);
				count ++;
			}
		}
	}

	if ((fp_w = fopen(OLD_NAME_IP, "w"))) {
		for(i=0 ; i<MAX_LEASES ; i++){
			if(strcmp(wl_clients[i].hwaddr,""))
				fprintf(fp_w,"%s %s %s\n",wl_clients[i].hwaddr,wl_clients[i].ipaddr,wl_clients[i].hostname);
		}
		fclose(fp_w);
	}

}

void
get_hostname_ip(char *type, char *filename){
	FILE *fp;
	char line[80];
	char leases[3][50];
	int i;
	int j;
	
	if ((fp = fopen(filename, "r"))) {			// find out hostname and ip
		while( fgets(line, sizeof(line), fp) != NULL ) {
			strcpy(leases[0],"");
			strcpy(leases[1],"");
			strcpy(leases[2],"");
			// 00:11:22:33:44:55 192.168.1.100 honor
			if (sscanf(line,"%s %s %s",leases[0],leases[1],leases[2]) != 3)
				continue;


			for(i=0 ; i<MAX_LEASES ; i++){
				for (j = 0; j < strlen(wl_client_macs[i].hwaddr); j++)
					wl_client_macs[i].hwaddr[j] = toupper(wl_client_macs[i].hwaddr[j]);


				if(!strcmp(leases[0], wl_client_macs[i].hwaddr)){


			//wwzh

					memset((void *)wl_client_macs[i].ipaddr, 0, sizeof(wl_client_macs[i].ipaddr));
					memset((void *)wl_client_macs[i].hostname, 0, sizeof(wl_client_macs[i].hostname));
					strcpy(wl_client_macs[i].ipaddr, leases[1]);
					strcpy(wl_client_macs[i].hostname, leases[2]);
				
					break;
				}
			}
		}
	}
	fclose(fp);

}


/* 0: success                                                                 
 * !=0 Error                                                                  
 */                                                                           
                                                                              
static int do_ioctl(tiwlan_ioctl_cmd_opcode_t opcode,                         
        tiwlan_ioctl_data_t type, unsigned long size, void *data,             
                tiwlan_ioctl_cmd_t *cmd)                                      
{                                                                             
        int result, dev_fd;                                                   
                                                                              
        if ((dev_fd = open("/dev/tiwlan", O_RDWR)) < 0)                       
        {                                                                     
                printf("Unable to open device log file!\n");                  
                return -1;                                                    
        }                                                                     
        cmd->opcode = opcode;                                                 
        cmd->data_type = type;                                                
        cmd->data_size = size;                                                
                                                                              
	                                                                                
        if ((type == TIWLAN_IOCTL_DATA_PACKET) || (type == TIWLAN_IOCTL_DATA_CHAR_PTR))                                                                         
        {                                                                       
                cmd->packet = (char *)data;                                     
        }                                                                       
        else                                                                    
        {                                                                       
                memcpy((void *)cmd->data, data, cmd->data_size);                
        }                                                                       
                                                                                
        result = ioctl(dev_fd, TIWLAN_DRV_IOCTL_IOCTL, cmd);                    
        close(dev_fd);                                                          
        return result;                                                          
}     

int tiwlan_SetAccessDelEntry(void)
{
	
	char maclist[1024];                          
 	unsigned char buf[18];                       
 	char *next;                                  
 	char *ch;                                    
 	char var[20];                                
 	unsigned char macaddr[6];                    
 	int result = 0;                              
 	tiwlan_ioctl_access_cmd_t delmac;            
 	tiwlan_ioctl_cmd_t cmd;                      
                                              
 	memset((void *)maclist, 0, sizeof(maclist)); 
                                              
 	strcpy(maclist, nvram_get("wl_maclist"));   

	foreach(var, maclist, next)                                        
        {                                                                  
                memset((void *)buf, 0, sizeof(buf));                       
                ch = strtok(var, ":");                                     
                if (ch != NULL)                                            
                {                                                          
                        strcpy(buf, ch);                                   
                        while ((ch = strtok(NULL, ":")) != NULL)           
                                strcat(buf, ch);                           
                        memset((void *)macaddr, 0, sizeof(macaddr));       
                        strtohex(macaddr, buf, strlen(buf));               
                        memset((void *)&delmac, 0, sizeof(delmac));        

			delmac.cmd = TIWLAN_ACCESS_DEL_ENTRY_BY_ADDRESS;                   
                        memcpy(&delmac.access_list[0][0], macaddr, ETH_ALEN);   
                                                                                
                        result = do_ioctl(TIWLAN_CMD_IOCTL_SET_ACCESS, TIWLAN_IOCTL_DATA_PACKET, sizeof(tiwlan_ioctl_access_cmd_t), (void *)&delmac, &cmd);     	                if (result != 0)                                        
                                printf("Set Access Add Entry Error!\n");        
			usleep(10);
                }                                                               
        }                                                                       
        return result;                                                          
}                                                                          
//wwzh


int swap(int x, int y)
{
	struct wl_client_mac wl_tmp;
	memset((void *)&wl_tmp, 0, sizeof(wl_tmp));

	strcpy(wl_tmp.hostname, wl_client_macs[x].hostname);
	strcpy(wl_tmp.ipaddr, wl_client_macs[x].ipaddr);
	strcpy(wl_tmp.hwaddr, wl_client_macs[x].hwaddr);
	wl_tmp.status = wl_client_macs[x].status;
	wl_tmp.check = wl_client_macs[x].check;

	memset((void *)&wl_client_macs[x], 0, sizeof(struct wl_client_mac));

	strcpy(wl_client_macs[x].hostname, wl_client_macs[y].hostname);
	strcpy(wl_client_macs[x].ipaddr, wl_client_macs[y].ipaddr);
	strcpy(wl_client_macs[x].hwaddr, wl_client_macs[y].hwaddr);
	wl_client_macs[x].status = wl_client_macs[y].status;
	wl_client_macs[x].check = wl_client_macs[y].check;

	memset((void *)&wl_client_macs[y], 0, sizeof(struct wl_client_mac));
	
	strcpy(wl_client_macs[y].hostname, wl_tmp.hostname);
	strcpy(wl_client_macs[y].ipaddr, wl_tmp.ipaddr);
	strcpy(wl_client_macs[y].hwaddr, wl_tmp.hwaddr);
	wl_client_macs[y].status = wl_tmp.status;
	wl_client_macs[y].check = wl_tmp.check;
	return 0;
}

int wireless_client_tables_sort(void)
{
	int i, j, s;
	char *ptr;
	int x, y;

	for (i = 0; i < nv_count; i++)
	{
		s = i;
		for (j = i + 1; j < nv_count; j++)
		{
			ptr = strrchr(wl_client_macs[j].ipaddr, '.');
			if (ptr)
			{
				ptr = ptr + 1;
				x = atoi(ptr);
			}
			else
			{
				x = 0;
			}
			ptr = strrchr(wl_client_macs[s].ipaddr, '.');
			if (ptr)
			{
				ptr = ptr + 1;
				y = atoi(ptr);
			}
			else
			{
				y = 0;
			}	
			if (x < y)
				s = j;			
		}
		swap(i, s);
	}
}


int wireless_get_active_mac()
{
#if 0
	tiwlan_ioctl_cmd_t cmd;                                         
        tiwlan_ioctl_statable_t station_table[MAX_STATIONS];                    
        int result = -1;                                                        
        int i = 0;    
	int j = 0;
	char buf[24];
	char macaddr[20];
        memset((void *)&station_table, 0, (sizeof(tiwlan_ioctl_statable_t) * MAX_STATIONS));                                                                    
        result = do_ioctl(TIWLAN_CMD_IOCTL_GET_STATABLE, TIWLAN_IOCTL_DATA_PACKET, (sizeof(tiwlan_ioctl_statable_t) * MAX_STATIONS), (void *)&(station_table[0]), &cmd);                                                                        
        if (result != 0)                                                        
                printf("Get MAC TABLES Error!\n");                              
#else
	tiap_param_statbl_t station_table[TIAP_PARAM_MAX_STATIONS];
	int	ioctl_sock, result, i, j;
	char buf[24],macaddr[20];

	if((ioctl_sock = iw_sockets_open()) < 0)
	{
		printf("IOCTL Open Socket failed.\n");
		return 0;
	}

	if ((result = exec_priv_ioctl_get(TIAP_PARAM_STA_TBL, station_table, 
			sizeof(tiap_param_statbl_t)*TIAP_PARAM_MAX_STATIONS,ioctl_sock, "wlan0")))
	{
		printf("IOCTL Get Station Table failed.\n");
		close(ioctl_sock);
		return 0;
	}
	close(ioctl_sock);

#endif
  for (i = 0; i < MAX_STATIONS; i++)
	{
		int match = 0;
		char tmp[32];
		memset((void *)buf, 0, sizeof(buf));
		memset((void *)macaddr, 0, sizeof(macaddr));
		strcpy(tmp, station_table[i].ssid);
		if (!strcmp(tmp, ""))
			break;

		 sprintf(macaddr, "%02x:%02x:%02x:%02x:%02x:%02x", (station_table[i].hwaddr[0] & 0xff), (station_table[i].hwaddr[1] & 0xff), (station_table[i].hwaddr[2] & 0xff), (station_table[i].hwaddr[3] & 0xff), (station_table[i].hwaddr[4] & 0xff), (station_table[i].hwaddr[5] & 0xff));                                                        	
		for (j = 0; j < strlen(macaddr); j++)
			macaddr[j] = toupper(macaddr[j]);

		for(j =0 ; j < nv_count ; j++)
		{
			if(!strcmp(wl_client_macs[j].hwaddr, macaddr))
			{
				wl_client_macs[j].status = 1;	// online
				wl_client_macs[j].check = 1;	// checked	
				match = 1;
	
				break;
			}
		}
		if(match == 0)
		{
			snprintf(wl_client_macs[nv_count].hwaddr, sizeof(wl_client_macs[nv_count].hwaddr), "%s", macaddr);
			wl_client_macs[nv_count].status = 1;	// online
			wl_client_macs[nv_count].check = 0;	// no checked
			nv_count++;
		}
	}
}

#ifdef CLOUD_SUPPORT
#define CLOUD_RELAY_FILE	"/tmp/.cloud_leases_file"
#endif

int
ej_wireless_active_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i,ret=0,flag=0;
	char *type;
	char word[256], *next;
	FILE *fp;
	char list[2][20];
	char line[80];
	int dhcp_table_count;
	char cmd[80];

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	if(!strcmp(type,"online")){
		for(i=0  ; i<MAX_LEASES ; i++){				// init	value
			strcpy(wl_client_macs[i].hostname,"");
			strcpy(wl_client_macs[i].ipaddr,"");
			strcpy(wl_client_macs[i].hwaddr,"");
			wl_client_macs[i].status = -1;
			wl_client_macs[i].check = 0;
		}	

		nv_count = 0;						// init mac list
		foreach(word, nvram_safe_get("wl_mac_list"), next) {
			snprintf(wl_client_macs[nv_count].hwaddr, sizeof(wl_client_macs[nv_count].hwaddr),"%s", word);	
			wl_client_macs[nv_count].status = 0;	// offline (default)
			wl_client_macs[nv_count].check = 1;	// checked
			nv_count++;
		}
		
		wireless_get_active_mac();

//wwzh
#if 0	
		snprintf(cmd,sizeof(cmd),"%s > %s", ASSOCLIST_CMD, ASSOCLIST_TMP);
		system(cmd);		// get active wireless mac


		if ((fp = fopen(ASSOCLIST_TMP, "r"))) {
			while( fgets(line, sizeof(line), fp) != NULL ) {
				int match = 0;
				strcpy(list[0],"");
				strcpy(list[1],"");
				if(sscanf(line,"%s %s",list[0],list[1]) != 2)	// assoclist 00:11:22:33:44:55
					continue;
				if(strcmp(list[0],"assoclist"))
					continue;
				for(i=0 ; i < nv_count ; i++){
					if(!strcmp(wl_client_macs[i].hwaddr, list[1])){
						wl_client_macs[i].status = 1;	// online
						wl_client_macs[i].check = 1;	// checked	
						match = 1;
	
						break;
					}
				}
				if(match == 0){
					snprintf(wl_client_macs[nv_count].hwaddr, sizeof(wl_client_macs[nv_count].hwaddr), "%s", list[1]);
					wl_client_macs[nv_count].status = 1;	// online
					wl_client_macs[nv_count].check = 0;	// no checked
					nv_count++;
				}
			}
			fclose(fp);
		}
#endif //end by wwzh


		if(!strcmp(type,"online")){
			dhcp_table_count = dhcp_lease_table_init();		// init dhcp lease table and get count
			get_hostname_ip("online",LEASES_NAME_IP);
		}
		save_hostname_ip();	
	}

//wwzh

	wireless_client_tables_sort();

	if(!strcmp(type,"offline")){
		get_hostname_ip("offline",OLD_NAME_IP);
	}

	
	if(!strcmp(type,"online")){

		//zhangbin 2005.04.22
		#ifdef CLOUD_SUPPORT
		if(nvram_match("cloud_dhcp_enable","1"))
		{
			FILE *fp;
			char line[300];
			char leases[4][80];
			
			memset(line,0,sizeof(line));
			memset(leases,0,sizeof(leases));

			fp = fopen(CLOUD_RELAY_FILE,"r");
			if(NULL == fp)
			{
				printf("Can't open %s!\n",CLOUD_RELAY_FILE);
				return 0;
					
			}
			while(fgets(line,sizeof(line),fp) != NULL)
			{
				if(sscanf(line,"%s %s %s %s",leases[0],leases[1],leases[2],leases[3]) != 4)
					continue;
	
				ret = websWrite(wp, "\
 <TR align=middle bgColor=#cccccc> \n\
    <TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    ",leases[3], leases[0], leases[1] ); 

				
			}
				
		}
		else
		{
		#endif


		for(i=0 ; i<nv_count ; i++){
			if(wl_client_macs[i].status != 1)	continue;

//wwzh
#if 0
{
	char buff[1024];
	memset((void *)buff, 0, sizeof(buff));

	sprintf(buff ,"<TR align=middle bgColor=#cccccc><TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n<TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n<TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n",wl_client_macs[i].hostname, wl_client_macs[i].ipaddr,wl_client_macs[i].hwaddr ); 
	printf("BBBUUUFFF: %s\n", buff);
}	
#endif

			ret = websWrite(wp, "\
 <TR align=middle bgColor=#cccccc> \n\
    <TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    ",wl_client_macs[i].hostname, wl_client_macs[i].ipaddr,wl_client_macs[i].hwaddr ); 
		}
//zhangbin 2005.04.22
	#ifdef CLOUD_SUPPORT
	  }
	#endif

	}
	else if(!strcmp(type,"offline")){
		for(i=0 ; i<nv_count ; i++){
			if(wl_client_macs[i].status != 0)       continue;
			ret = websWrite(wp, "\
 <TR align=middle bgColor=#cccccc> \n\
    <TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"141\"><input type=checkbox name=off%d value=%d %s></TD> \n\
 </TR>\n",wl_client_macs[i].hostname, wl_client_macs[i].ipaddr, wl_client_macs[i].hwaddr, flag++, i, wl_client_macs[i].check ? "checked" : "");
		
		}
	}
	//if(dhcp_lease_table)	free(dhcp_lease_table);
	return 0;
}

int
ej_wireless_connected_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i,ret=0,flag=0;
	char *type;
	char word[256], *next;
	FILE *fp;
	char list[2][20];
	char line[80];
	int dhcp_table_count;
	char cmd[80];

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	if(!strcmp(type,"online")){
		for(i=0  ; i<MAX_LEASES ; i++){				// init	value
			strcpy(wl_client_macs[i].hostname,"");
			strcpy(wl_client_macs[i].ipaddr,"");
			strcpy(wl_client_macs[i].hwaddr,"");
			wl_client_macs[i].status = -1;
			wl_client_macs[i].check = 0;
		}	

		nv_count = 0;						// init mac list
		foreach(word, nvram_safe_get("wl_mac_list"), next) {
			snprintf(wl_client_macs[nv_count].hwaddr, sizeof(wl_client_macs[nv_count].hwaddr),"%s", word);	
			wl_client_macs[nv_count].status = 0;	// offline (default)
			wl_client_macs[nv_count].check = 1;	// checked
			nv_count++;
		}
		
		wireless_get_active_mac();
//wwzh
#if 0	
		snprintf(cmd,sizeof(cmd),"%s > %s", ASSOCLIST_CMD, ASSOCLIST_TMP);
		system(cmd);		// get active wireless mac


		if ((fp = fopen(ASSOCLIST_TMP, "r"))) {
			while( fgets(line, sizeof(line), fp) != NULL ) {
				int match = 0;
				strcpy(list[0],"");
				strcpy(list[1],"");
				if(sscanf(line,"%s %s",list[0],list[1]) != 2)	// assoclist 00:11:22:33:44:55
					continue;
				if(strcmp(list[0],"assoclist"))
					continue;
				for(i=0 ; i < nv_count ; i++){
					if(!strcmp(wl_client_macs[i].hwaddr, list[1])){
						wl_client_macs[i].status = 1;	// online
						wl_client_macs[i].check = 1;	// checked	
						match = 1;
	
						break;
					}
				}
				if(match == 0){
					snprintf(wl_client_macs[nv_count].hwaddr, sizeof(wl_client_macs[nv_count].hwaddr), "%s", list[1]);
					wl_client_macs[nv_count].status = 1;	// online
					wl_client_macs[nv_count].check = 0;	// no checked
					nv_count++;
				}
			}
			fclose(fp);
		}
#endif //end by wwzh


		if(!strcmp(type,"online")){
			dhcp_table_count = dhcp_lease_table_init();		// init dhcp lease table and get count
			get_hostname_ip("online",LEASES_NAME_IP);
		}
		save_hostname_ip();	
	}
//wwzh add for sort ipaddress 
	 wireless_client_tables_sort();


	if(!strcmp(type,"offline")){
		get_hostname_ip("offline",OLD_NAME_IP);
	}

	if(!strcmp(type,"online")){
		for(i=0 ; i<nv_count ; i++){
			if(wl_client_macs[i].status != 1)	continue;
			ret = websWrite(wp, "\
 <TR align=middle bgColor=#cccccc> \n\
    <TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"141\"><input type=checkbox name=on%d value=%d %s></TD> \n\
 </TR>\n",wl_client_macs[i].hostname, wl_client_macs[i].ipaddr,wl_client_macs[i].hwaddr , flag++, i, wl_client_macs[i].check ? "checked" : "");
		}
	}
	else if(!strcmp(type,"offline")){
		for(i=0 ; i<nv_count ; i++){
			if(wl_client_macs[i].status != 0)       continue;
			ret = websWrite(wp, "\
 <TR align=middle bgColor=#cccccc> \n\
    <TD height=\"20\" width=\"167\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"140\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"156\"><FONT face=Arial size=2>%s</FONT></TD> \n\
    <TD height=\"20\" width=\"141\"><input type=checkbox name=off%d value=%d %s></TD> \n\
 </TR>\n",wl_client_macs[i].hostname, wl_client_macs[i].ipaddr, wl_client_macs[i].hwaddr, flag++, i, wl_client_macs[i].check ? "checked" : "");
		
		}
	}
	//if(dhcp_lease_table)	free(dhcp_lease_table);
	return 0;
}
char 
*get_wep_value(char *type, char *_bit)
{
	static char word[200];
	char *next, *wordlist;
	char wl_wep[] = "wl_wep_XXXXXX";
	char *wl_passphrase, *wl_key1, *wl_key2, *wl_key3, *wl_key4, *wl_key_tx;

	if(generate_key){
 		snprintf(wl_wep, sizeof(wl_wep), "%s","wl_wep_gen");
	}
	else{
 		snprintf(wl_wep, sizeof(wl_wep), "%s","wl_wep_buf");
	}

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%s): wl_wep=[%s]",__FUNCTION__, type, _bit, wl_wep);
#endif
	wordlist = nvram_safe_get(wl_wep);
	//if(strcmp(wordlist,"") && !strcmp(_bit,"64")){
		foreach(word, wordlist, next) {
			wl_key1 = word;
			wl_passphrase = strsep(&wl_key1, ":"); 
			if(!wl_passphrase || !wl_key1)
				continue;
			wl_key2 = wl_key1;
			wl_key1 = strsep(&wl_key2, ":");
			if(!wl_key1 || !wl_key2)
				continue;
			wl_key3 = wl_key2;
			wl_key2 = strsep(&wl_key3, ":");
			if(!wl_key2 || !wl_key3)
                                continue;
			wl_key4 = wl_key3;
			wl_key3 = strsep(&wl_key4, ":");
			if(!wl_key3 || !wl_key4)
                                continue;
			wl_key_tx = wl_key4;
			wl_key4 = strsep(&wl_key_tx, ":");
			if(!wl_key4 || !wl_key_tx)
                                continue;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): wl_passphrase=[%s] wl_key1=[%s] wl_key2=[%s] wl_key3=[%s] wl_key4=[%s] wl_key_tx=[%s]",__FUNCTION__, wl_passphrase, wl_key1, wl_key2, wl_key3, wl_key4, wl_key_tx);
#endif
			if(!strcmp(type,"passphrase")){
				return wl_passphrase;	
			}
			else if(!strcmp(type,"key1")){
				return wl_key1;	
			}
			else if(!strcmp(type,"key2")){
				return wl_key2;	
			}
			else if(!strcmp(type,"key3")){
				return wl_key3;	
			}
			else if(!strcmp(type,"key4")){
				return wl_key4;	
			}
			else if(!strcmp(type,"tx")){
				return wl_key_tx;	
			}
		}
	return "";
}

int
ej_get_wep_value(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type, *bit;
	char *value="", new_value[50]="";

	if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	bit = GOZILA_GET("wl_wep_bit");

	value = get_wep_value(type, bit);
	
        filter_name(value, new_value, sizeof(new_value), GET);
	
	return websWrite(wp,"%s", new_value);
}

int
ej_show_wl_wep_setting(int eid, webs_t wp, int argc, char_t **argv)
{

	int ret = 0;
/*
	char *type;

	type = gozila_action ? websGetVar(wp, "wl_wep_bit", NULL) : nvram_safe_get("wl_wep_bit");

ret += websWrite(wp," \
              <TR> \n\
                <TH align=right width=150 bgColor=#6666cc height=25>&nbsp;Passphrase:&nbsp;&nbsp;</TH> \n\
                <TD align=left width=435 height=25>&nbsp;\n\
                	<INPUT maxLength=16 name=wl_passphrase size=20 value='%s'>&nbsp; \n\
			<INPUT type=hidden value=Null name=generateButton> \n\
			<INPUT type=button value='Generate' onclick=generateKey(this.form) name=wepGenerate></TD></TR>\n",get_wep_value("passphrase",type));

ret += websWrite(wp," \
              <TR> \n\
                <TH vAlign=center align=right width=150 bgColor=#6666cc height=25>Key 1:&nbsp;&nbsp;</TH> \n\
                <TD vAlign=bottom width=435 bgColor=#ffffff height=25>&nbsp;\n\
			<INPUT size=35 name=wl_key1 value='%s'></TD></TR>\n",get_wep_value("key1",type));

ret += websWrite(wp," \
              <TR> \n\
                <TH vAlign=center align=right width=150 bgColor=#6666cc height=25>Key 2:&nbsp;&nbsp;</TH> \n\
                <TD vAlign=bottom width=435 bgColor=#ffffff height=25>&nbsp;\n\
			<INPUT size=35 name=wl_key2 value='%s'></TD></TR>\n",get_wep_value("key2",type));
ret += websWrite(wp," \
              <TR> \n\
                <TH vAlign=center align=right width=150 bgColor=#6666cc height=25>Key 3:&nbsp;&nbsp;</TH> \n\
                <TD vAlign=bottom width=435 bgColor=#ffffff height=25>&nbsp;\n\
			<INPUT size=35 name=wl_key3 value='%s'></TD></TR>\n",get_wep_value("key3",type));
ret += websWrite(wp," \
              <TR> \n\
                <TH vAlign=center align=right width=150 bgColor=#6666cc height=25>Key 4:&nbsp;&nbsp;</TH> \n\
                <TD vAlign=bottom width=435 bgColor=#ffffff height=25>&nbsp;\n\
			<INPUT size=35 name=wl_key4 value='%s'></TD></TR>\n",get_wep_value("key4",type));
*/
	return ret;
}

void
validate_macmode(webs_t wp, char *value, struct variable *v)
{
	char *wl_macmode1, *wl_macmode;	

	wl_macmode = websGetVar(wp, "wl_macmode", NULL);
	wl_macmode1 = websGetVar(wp, "wl_macmode1", NULL);

	if(!wl_macmode1)	return;

	if(!strcmp(wl_macmode1,"disabled")){
		nvram_set("wl_macmode1","disabled");
		nvram_set("wl_macmode","disabled");
	}
	else if(!strcmp(wl_macmode1,"other")){
		if(!wl_macmode)	return;
		nvram_set("wl_macmode1","other");
                nvram_set("wl_macmode",wl_macmode);
	}
}


int
generate_wep_key(int key)
{
	int i;
	char buf[256];
	char *passphrase, *bit, *tx;
	char key1[26]="";
	char key2[26]="";
	char key3[26]="";
	char key4[26]="";

	bit = websGetVar(wp, "wl_wep_bit", NULL);
	passphrase = websGetVar(wp, "wl_passphrase", NULL);
	tx = websGetVar(wp, "wl_key", NULL);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s():bit=[%s] passphrase=[%s] tx=[%s]",__FUNCTION__,bit,passphrase,tx);
#endif

	if(!bit || !passphrase || !tx)	return 0;
	
	gen_key(passphrase,atoi(bit));	
	
	if(atoi(bit) == 64){
		for(i=0;i<5;i++)
			sprintf(key1+strlen(key1),"%02X",key64[0][i]);
		for(i=0;i<5;i++)
			sprintf(key2+strlen(key2),"%02X",key64[1][i]);
		for(i=0;i<5;i++)
			sprintf(key3+strlen(key3),"%02X",key64[2][i]);
		for(i=0;i<5;i++)
			sprintf(key4+strlen(key4),"%02X",key64[3][i]);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s():key1=[%s] key2=[%s] key3=[%s] key4=[%s]",__FUNCTION__,key1,key2,key3,key4);
#endif
		snprintf(buf,sizeof(buf),"%s:%s:%s:%s:%s:%s",passphrase,key1,key2,key3,key4,tx);
		//nvram_set("wl_wep_gen_64",buf);
		nvram_set("wl_wep_gen",buf);
	}
	else if(atoi(bit) == 128){
		for(i=0;i<13;i++)
			sprintf(key1+strlen(key1),"%02X",key128[0][i]);
		for(i=0;i<13;i++)
			sprintf(key2+strlen(key2),"%02X",key128[1][i]);
		for(i=0;i<13;i++)
			sprintf(key3+strlen(key3),"%02X",key128[2][i]);
		for(i=0;i<13;i++)
			sprintf(key4+strlen(key4),"%02X",key128[3][i]);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s():key1=[%s] key2=[%s] key3=[%s] key4=[%s]",__FUNCTION__,key1,key2,key3,key4);
#endif
		//cprintf("passphrase[%s]\n", passphrase);
		//filter_name(passphrase, new_passphrase, sizeof(new_passphrase), SET);
		//cprintf("new_passphrase[%s]\n", new_passphrase);
		snprintf(buf,sizeof(buf),"%s:%s:%s:%s:%s:%s", passphrase,key1,key2,key3,key4,tx);
		//nvram_set("wl_wep_gen_128",buf);
		nvram_set("wl_wep_gen",buf);
	}

	return 1;
}

int
generate_key_64(webs_t wp)
{
	int ret;
	
	generate_key = 1;
	ret = generate_wep_key(64);
	
	return ret;
}

int
generate_key_128(webs_t wp)
{
	int ret;
	
	generate_key = 1;
	ret = generate_wep_key(128);
	
	return ret;
}

int 
wl_active_onload(webs_t wp, char *arg)
{
	int ret = 0;

	if(!strcmp(nvram_safe_get("wl_active_add_mac"),"1")){
		ret += websWrite(wp, arg);
	}

	return ret;

}

// only for nonbrand
int
ej_get_wl_active_mac(int eid, webs_t wp, int argc, char_t **argv)
{
	char cmd[80], line[80];
	char list[2][20];
	FILE *fp;
	int ret = 0, count=0;

	snprintf(cmd,sizeof(cmd),"%s > %s", ASSOCLIST_CMD, ASSOCLIST_TMP);
	system(cmd);		// get active wireless mac

	if ((fp = fopen(ASSOCLIST_TMP, "r"))) {
		while( fgets(line, sizeof(line), fp) != NULL ) {
			strcpy(list[0],"");
			strcpy(list[1],"");
			if(sscanf(line,"%s %s",list[0],list[1]) != 2)	// assoclist 00:11:22:33:44:55
				continue;
			if(strcmp(list[0],"assoclist"))
				continue;
			ret += websWrite(wp,"%c'%s'",count ? ',' : ' ',list[1]);
			count++;
		}
	}

	return ret;
}

int
ej_get_wl_value(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *type;
	
        if (ejArgs(argc, argv, "%s", &type) < 1) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	if(!strcmp(type,"default_dtim")){
//#ifdef WiFi_11b_TEST
		ret = websWrite(wp,"1");	// This is a best value for 11b test
//#else
//		ret = websWrite(wp,"3");
//#endif
	}
	return ret;
	
}

int
ej_show_wpa_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
        char *type, *security_mode;
	
        if (ejArgs(argc, argv, "%s", &type) < 1) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
        security_mode = GOZILA_GET("security_mode"); 


	if(!strcmp(security_mode, "psk"))
		do_ej("WPA_Preshared.asp",wp);
#if UI_STYLE != CISCO
	else if(!strcmp(security_mode, "disabled"))	// Use in old linksys style UI
		do_ej("WPA_Preshared.asp",wp);
#endif
	else if(!strcmp(security_mode, "radius")){
		do_ej("Radius.asp",wp);
		do_ej("WEP.asp",wp);
	}
	else if(!strcmp(security_mode, "wpa"))
		do_ej("WPA_Radius.asp",wp);
	else if(!strcmp(security_mode, "wep"))
		do_ej("WEP.asp",wp);
	
	
	return ret;
}
