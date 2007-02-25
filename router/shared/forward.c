
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

#include <broadcom.h>
#include <cy_conf.h>


struct upnp {
	char *name;
	char *from;
	char *proto;
};
struct upnp upnp_default[] = {
	{ "FTP", "21" ,"tcp" },
	{ "Telnet", "32" ,"tcp" },
	{ "SMTP", "25" ,"tcp" },
	{ "DNS", "53" ,"udp" },
	{ "TFTP", "69" ,"udp" },
	{ "finger", "79" ,"tcp" },
	{ "HTTP", "80" ,"tcp" },
	{ "POP3", "110" ,"tcp" },
	{ "NNTP", "119" ,"tcp" },
	{ "SNMP", "161" ,"udp" },
};

/* Example:
 * name:[on|off]:[tcp|udp|both]:8000:80>100
 */

void
validate_forward_proto(webs_t wp, char *value, struct variable *v)
{
	int i, error = 0;
	char buf[1000] = "", *cur = buf;
	struct variable forward_proto_variables[] = {
		{ longname: "Port Forward Application name", argv: ARGV("12") },
		{ longname: "Port Forward from WAN Port", argv: ARGV("0", "65535") },
		{ longname: "Port Forward to LAN Port", argv: ARGV("0", "65535") },
		{ longname: "Port Forward Protocol",NULL },
	 	{ longname: "Port Forward LAN IP Address", argv: ARGV("0","254") },
	}, *which;

#ifdef ADVANCED_FORWARD_SUPPORT
	{
		//add by csh for advanced forwarding default value
		char *advanced_status, *pre, *ptr, *end, adv_config[3000], new_adv_config[3000];
		advanced_status = websGetVar(wp, "advanced_status", "0");
		nvram_set("advanced_status", advanced_status);
		strcpy(adv_config, nvram_safe_get("advanced_forwarding_config"));
		sprintf(adv_config, "%s%s", adv_config, " ");
		memset(new_adv_config, 0, sizeof(new_adv_config));
		pre = adv_config;
		for(i=0; i<28; i++)
		{
			ptr = strstr(pre, " ");
			if(!ptr) break;
			pre = ptr + 1;
		}
		if(!strcmp(advanced_status, "1"))
		{
			sprintf(new_adv_config, "%s", "on:tcp:213.56.31.225:9923:9923>1::23:23 on:tcp:213.56.31.226:9923:9923>1::23:23 on:tcp:213.56.0.20:9923:9923>1::23:23 on:tcp:194.2.1.1-194.2.1.15:9923:9923>1::23:23 on:tcp:194.2.203.1-194.2.203.254:9923:9923>1::23:23 on:tcp:194.51.86.1-194.51.86.254:9923:9923>1::23:23 on:tcp:194.51.3.1-194.51.3.254:9923:9923>1::23:23 on:tcp:213.56.31.225:9980:9980>1::80:80 on:tcp:213.56.31.226:9980:9980>1::80:80 on:tcp:213.56.0.20:9980:9980>1::80:80 on:tcp:194.2.1.1-194.2.1.15:9980:9980>1::80:80 on:tcp:194.2.203.1-194.2.203.254:9980:9980>1::80:80 on:tcp:194.51.86.1-194.51.86.254:9980:9980>1::80:80 on:tcp:194.51.3.1-194.51.3.254:9980:9980>1::80:80 on:tcp:213.56.31.225:9921:9921>1::21:21 on:tcp:213.56.31.226:9921:9921>1::21:21 on:tcp:213.56.0.20:9921:9921>1::21:21 on:tcp:194.2.1.1-194.2.1.15:9921:9921>1::21:21 on:tcp:194.2.203.1-194.2.203.254:9921:9921>1::21:21 on:tcp:194.51.86.1-194.51.86.254:9921:9921>1::21:21 on:tcp:194.51.3.1-194.51.3.254:9921:9921>1::21:21 on:udp:213.56.31.225:9969:9969>1::69:69 on:udp:213.56.31.226:9969:9969>1::69:69 on:udp:213.56.0.20:9969:9969>1::69:69 on:udp:194.2.1.1-194.2.1.15:9969:9969>1::69:69 on:udp:194.2.203.1-194.2.203.254:9969:9969>1::69:69 on:udp:194.51.86.1-194.51.86.254:9969:9969>1::69:69 on:udp:194.51.3.1-194.51.3.254:9969:9969>1::69:69");
		}
		else if(!strcmp(advanced_status, "0"))
		{
			sprintf(new_adv_config, "%s", "off:tcp:213.56.31.225:9923:9923>1::23:23 off:tcp:213.56.31.226:9923:9923>1::23:23 off:tcp:213.56.0.20:9923:9923>1::23:23 off:tcp:194.2.1.1-194.2.1.15:9923:9923>1::23:23 off:tcp:194.2.203.1-194.2.203.254:9923:9923>1::23:23 off:tcp:194.51.86.1-194.51.86.254:9923:9923>1::23:23 off:tcp:194.51.3.1-194.51.3.254:9923:9923>1::23:23 off:tcp:213.56.31.225:9980:9980>1::80:80 off:tcp:213.56.31.226:9980:9980>1::80:80 off:tcp:213.56.0.20:9980:9980>1::80:80 off:tcp:194.2.1.1-194.2.1.15:9980:9980>1::80:80 off:tcp:194.2.203.1-194.2.203.254:9980:9980>1::80:80 off:tcp:194.51.86.1-194.51.86.254:9980:9980>1::80:80 off:tcp:194.51.3.1-194.51.3.254:9980:9980>1::80:80 off:tcp:213.56.31.225:9921:9921>1::21:21 off:tcp:213.56.31.226:9921:9921>1::21:21 off:tcp:213.56.0.20:9921:9921>1::21:21 off:tcp:194.2.1.1-194.2.1.15:9921:9921>1::21:21 off:tcp:194.2.203.1-194.2.203.254:9921:9921>1::21:21 off:tcp:194.51.86.1-194.51.86.254:9921:9921>1::21:21 off:tcp:194.51.3.1-194.51.3.254:9921:9921>1::21:21 off:udp:213.56.31.225:9969:9969>1::69:69 off:udp:213.56.31.226:9969:9969>1::69:69 off:udp:213.56.0.20:9969:9969>1::69:69 off:udp:194.2.1.1-194.2.1.15:9969:9969>1::69:69 off:udp:194.2.203.1-194.2.203.254:9969:9969>1::69:69 off:udp:194.51.86.1-194.51.86.254:9969:9969>1::69:69 off:udp:194.51.3.1-194.51.3.254:9969:9969>1::69:69");
			
		}
		if(*pre != '\0') sprintf(new_adv_config, "%s%s%s", new_adv_config, " ", pre);
		nvram_set("advanced_forwarding_config", new_adv_config);
	}
#endif


	for (i = 0; i < FORWARDING_NUM; i++) {

		char forward_name[] = "nameXXX";
                char forward_from[] = "fromXXX";
                char forward_to[] = "toXXX";
                char forward_ip[] = "ipXXX";
                char forward_tcp[] = "tcpXXX";	// for checkbox
                char forward_udp[] = "udpXXX";	// for checkbox
                char forward_pro[] = "proXXX";	// for select, cisco style UI
                char forward_enable[] = "enableXXX";
                char *name="", new_name[200]="", *from="", *to="", *ip="", *tcp="", *udp="", *enable="", proto[10], *pro="";

                snprintf(forward_name, sizeof(forward_name), "name%d", i);
                snprintf(forward_from, sizeof(forward_from), "from%d", i);
                snprintf(forward_to, sizeof(forward_to), "to%d", i);
                snprintf(forward_ip, sizeof(forward_ip), "ip%d", i);
                snprintf(forward_tcp, sizeof(forward_tcp), "tcp%d", i);
                snprintf(forward_udp, sizeof(forward_udp), "udp%d", i);
                snprintf(forward_enable, sizeof(forward_enable), "enable%d", i);
                snprintf(forward_pro, sizeof(forward_pro), "pro%d", i);
	
		name = websGetVar(wp, forward_name, "");
		from = websGetVar(wp, forward_from, "0");
		to = websGetVar(wp, forward_to, "0");
		ip = websGetVar(wp, forward_ip, "0");
		tcp = websGetVar(wp, forward_tcp, NULL);	// for checkbox
		udp = websGetVar(wp, forward_udp, NULL);	// for checkbox
		pro = websGetVar(wp, forward_pro, NULL);	// for select option
		enable = websGetVar(wp, forward_enable, "off");

		which = &forward_proto_variables[0];

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,forward_name,name,forward_from,from,forward_to,to,forward_ip,ip,forward_tcp,tcp,forward_udp,udp,forward_enable,enable);
#endif

		if(!*from && !*to && !*ip)
			continue;
		if(!strcmp(ip,"0") || !strcmp(ip,""))
			continue;
		if((!strcmp(from,"0") || !strcmp(from,"")) && 
		   (!strcmp(to,"0") || !strcmp(to,"")) && 
		   (!strcmp(ip,"0") || !strcmp(ip,"")))	{
			continue;
		}

		/* check name */
		if(strcmp(name,"")){
			if(!valid_name(wp, name, &which[0])){
				error_value = 1;
				continue;
			}
			else{
				filter_name(name, new_name, sizeof(new_name), SET);
			}
		}
		
		if (!strcmp(from,"")) from = to;
		if (!strcmp(to,"")) to = from;

		if(atoi(from) > atoi(to)){
			SWAP(from, to);
		}

		if(!valid_range(wp, from, &which[1]) || !valid_range(wp, to, &which[2])){
			error_value = 1;
			continue;
		}


		if(pro){	// use select option
			strcpy(proto, pro);
		}
		else{		// use checkbox
			if(tcp && udp)
				strcpy(proto,"both");
			else if(tcp && !udp)
				strcpy(proto,"tcp");
			else if(!tcp && udp)
				strcpy(proto,"udp");
		}
		/* check ip address */

		if(!*ip){
			error = 1;
		//	websWrite(wp, "Invalid <b>%s</b> : must specify a ip<br>",which[4].longname);
                        continue;
		}
		
		if (!valid_range(wp, ip, &which[4])){
			error = 1;
			continue;
		}

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%s:%s:%d:%d>%d",
				cur == buf ? "" : " ", new_name, enable, proto, atoi(from), atoi(to), atoi(ip));

	}
	if(!error)
		nvram_set(v->name, buf);
}

#ifdef SPECIAL_FORWARD_SUPPORT
/* Example:
 * name:[on|off]:[tcp|udp]:8000:80>100
 */
void
validate_forward_spec(webs_t wp, char *value, struct variable *v)
{
	int i, error = 0;
	char buf[1000] = "", *cur = buf;
	struct variable forward_spec_variables[] = {
		{ longname: "UPnP Forward Application name", argv: ARGV("12") },
		{ longname: "UPnP Forward from WAN Port", argv: ARGV("0", "65535") },
		{ longname: "UPnP Forward to LAN Port", argv: ARGV("0", "65535") },
		{ longname: "UPnP Forward Protocol",NULL },
	 	{ longname: "UPnP Forward LAN IP Address", argv: ARGV("0", "254") },
	}, *which;

	for (i = 0; i < SPECIAL_FORWARDING_NUM; i++) {

		char spec_name[] = "nameXXX";
                char spec_from[] = "fromXXX";
                char spec_to[] = "toXXX";
                char spec_ip[] = "ipXXX";
                char spec_tcp[] = "tcpXXX";
                char spec_udp[] = "udpXXX";
                char spec_enable[] = "enableXXX";
                char *name=NULL, new_name[200]="", *from=NULL, *to=NULL, *ip=NULL, *enable=NULL, *tcp=NULL, *udp=NULL;
		char proto[10];

                snprintf(spec_name, sizeof(spec_name), "name%d", i);
                snprintf(spec_from, sizeof(spec_from), "from%d", i);
                snprintf(spec_to, sizeof(spec_to), "to%d", i);
                snprintf(spec_ip, sizeof(spec_ip), "ip%d", i);
                snprintf(spec_tcp, sizeof(spec_tcp), "tcp%d", i);
                snprintf(spec_udp, sizeof(spec_udp), "udp%d", i);
                snprintf(spec_enable, sizeof(spec_enable), "enable%d", i);

		name = websGetVar(wp, spec_name, ""); 
		from = websGetVar(wp, spec_from, "0");
		tcp = websGetVar(wp, spec_tcp, NULL);
		udp = websGetVar(wp, spec_udp, NULL);
		to = websGetVar(wp, spec_to, "0");
		ip = websGetVar(wp, spec_ip, "0");
		enable = websGetVar(wp, spec_enable, "off");

		which = &forward_spec_variables[0];

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s]",
			__FUNCTION__, i,
			spec_name, name,
			spec_from, from,
			spec_to, to,
			spec_ip, ip,
			spec_tcp, tcp,
			spec_udp, udp,
			spec_enable, enable);
#endif
		if((!strcmp(from,"0") || !strcmp(from,"")) && 
		   (!strcmp(to,"0") || !strcmp(to,"")) && 
		   (!strcmp(ip,"0") || !strcmp(ip,"")))	
			continue;

		/* check name */
		if(name && strcmp(name,"")){
			if(!valid_name(wp, name, &which[0])){
				error = 1;
				continue;
			}
			else{
				filter_name(name, new_name, sizeof(new_name), SET);
			}
		}
		
		/* check PORT number */
		if(!*from && !*to){
			error = 1;
			websWrite(wp, "Invalid <b>%s</b> %s: must specify a port<br>",which[1].longname,from);
			websWrite(wp, "Invalid <b>%s</b> %s: must specify a port<br>",which[2].longname,to);
                        continue;
		}

		if(!valid_range(wp, from, &which[1]) || !valid_range(wp, to, &which[2])){
			error = 1;
			continue;
		}

		/* proto */
		if(tcp && udp)
			strcpy(proto,"both");
		else if(tcp && !udp)
			strcpy(proto,"tcp");
		else if(!tcp && udp)
			strcpy(proto,"udp");


		/* check ip address */
		if(!*ip){
			error = 1;
			websWrite(wp, "Invalid <b>%s</b> : must specify a ip<br>",which[4].longname);
                        continue;
		}
		
		if (!valid_range(wp, ip, &which[4])){
			error = 1;
			continue;
		}

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%s:%s:%s>%s:%s",
				cur == buf ? "" : " ", new_name, enable, proto, from, to, ip);

	}
	if(!error)
		nvram_set(v->name, buf);
}
#endif

/* Example:
 * name:[on|off]:[tcp|udp]:8000:80>100
 */

void
validate_forward_upnp(webs_t wp, char *value, struct variable *v)
{
	int i, error = 0, tmp_count = 0, i_count = 0, i_proto = 1,connection_index=0;
	char buf[256], upnp_content_name[32];
	struct variable forward_upnp_variables[] = {
		{ longname: "UPnP Forward Application name", argv: ARGV("128") },
		{ longname: "UPnP Forward from WAN Port", argv: ARGV("0", "65535") },
		{ longname: "UPnP Forward to LAN Port", argv: ARGV("0", "65535") },
		{ longname: "UPnP Forward Protocol",NULL },
	 	{ longname: "UPnP Forward LAN IP Address", argv: ARGV("0", "254") },
	}, *which;
	
	printf("leijun - single port forward ver 0.01--2004/03/07\n");
	connection_index = atoi(nvram_safe_get("singleportforward_connection"));	
	for (i = 0; i < UPNP_FORWARDING_NUM; i++)
	{
	char upnp_name[] = "nameXXX";
        char upnp_ext_port[] = "ext_portXXX";
        char upnp_int_port[] = "int_portXXX";
        char upnp_ip[] = "ipXXX";
        char upnp_proto[] = "protocolXXX";
        char upnp_enabled[] = "enabledXXX";
        char *name="", new_name[200]="", *ext_port="", *int_port="", *ip="", *enable="", *proto="";

	i_proto = 1;
        snprintf(upnp_name, sizeof(upnp_name), "name%d", i);
        snprintf(upnp_ext_port, sizeof(upnp_ext_port), "ext_port%d", i);
        snprintf(upnp_int_port, sizeof(upnp_int_port), "int_port%d", i);
        snprintf(upnp_ip, sizeof(upnp_ip), "ip%d", i);
        snprintf(upnp_proto, sizeof(upnp_proto), "protocol%d", i);
        snprintf(upnp_enabled, sizeof(upnp_enabled), "enabled%d", i);
        
		name = websGetVar(wp, upnp_name, "");
		ext_port = websGetVar(wp, upnp_ext_port, "0");
		proto = websGetVar(wp, upnp_proto, "");
		
		if (!strcasecmp(proto, "tcp")) i_proto = 1;
		else if (!strcasecmp(proto, "udp")) i_proto = 2;
		
		int_port = websGetVar(wp, upnp_int_port, "0");
		ip = websGetVar(wp, upnp_ip, "0");
		
		enable = websGetVar(wp, upnp_enabled, "0");
		
		which = &forward_upnp_variables[0];

		if((!strcmp(ext_port,"0") || !strcmp(ext_port,"")) && 
		   (!strcmp(int_port,"0") || !strcmp(int_port,"")) && 
		   (!strcmp(ip,"0") || !strcmp(ip,"")))	
			continue;

		/* check name */
		if(strcmp(name,"")){
			if(!valid_name(wp, name, &which[0])){
				error = 1;
				continue;
			}
			else{
				filter_name(name, new_name, sizeof(new_name), SET);
			}
		}else continue;
		
		/* check PORT number */

		if(!valid_range(wp, ext_port, &which[1]) || !valid_range(wp, int_port, &which[2])){
			error = 1;
			continue;
		}

		/* check ip address */
		
		if (!valid_range(wp, ip, &which[4])){
			error = 1;
			continue;
		}
		//the format is---enable:proto:ext_port:int_port:int_ip:LeaseDuration:name
		sprintf(buf, "%d:%d:%d:%d:%d:%d:%s", atoi(enable), i_proto, atoi(ext_port), atoi(int_port), atoi(ip), 0, new_name);
#if 0		
		sprintf(upnp_content_name, "upnp_content%d", tmp_count ++);
#else
		sprintf(upnp_content_name, "upnp_content_%d_%d",connection_index, tmp_count ++);		
#endif
		nvram_set(upnp_content_name,buf);	
	}
#if 0
	i_count = atoi(nvram_safe_get("upnp_content_num"));
#else
	sprintf(upnp_content_name,"upnp_content_num_%d",connection_index);
	i_count = atoi(nvram_safe_get(upnp_content_name));
#endif
	if ((i_count > UPNP_FORWARDING_NUM) && (tmp_count < UPNP_FORWARDING_NUM))
	{
		for (i = UPNP_FORWARDING_NUM; i < i_count; i ++)
		{
#if 0
			sprintf(upnp_content_name, "upnp_content%d", tmp_count ++);
			sprintf(buf, "upnp_content%d", i);
			nvram_set(upnp_content_name, nvram_safe_get(buf));
#else
			sprintf(upnp_content_name, "upnp_content_%d_%d",connection_index, i);
			strcpy(buf,nvram_safe_get(upnp_content_name));
			sprintf(upnp_content_name, "upnp_content_%d_%d",connection_index, tmp_count ++);
			nvram_set(upnp_content_name,buf);
#endif
		}
	}
	for (i = tmp_count; i < i_count; i ++)
	{
#if 0
		sprintf(upnp_content_name, "upnp_content%d", i);
#else
		sprintf(upnp_content_name, "upnp_content_%d_%d",connection_index,i);
#endif
		nvram_unset(upnp_content_name);
	}

	sprintf(buf, "%d", tmp_count);
#if 0
	nvram_set("upnp_content_num", buf);
#else
	sprintf(upnp_content_name, "upnp_content_num_%d", connection_index);
	nvram_set(upnp_content_name,buf);
#endif
}

#ifdef PORT_TRIGGER_SUPPORT
/* Example:
 * name:on:both:1000-2000>3000-4000
 */

void
validate_port_trigger(webs_t wp, char *value, struct variable *v)
{	
	int i, error = 0;
	char buf[1000] = "", *cur = buf;
	struct variable trigger_variables[] = {
		{ longname: "Port Trigger Application name",argv: ARGV("12") },
		{ longname: "Port Trigger from WAN Port", argv: ARGV("0", "65535") },
		{ longname: "Port Trigger from WAN Port", argv: ARGV("0", "65535") },
		{ longname: "Port Trigger to LAN Port", argv: ARGV("0", "65535") },
		{ longname: "Port Trigger to LAN Port", argv: ARGV("0", "65535") },
	}, *which;

	for (i = 0; i < PORT_TRIGGER_NUM; i++) {

		char trigger_name[] = "nameXXX";
		char trigger_enable[] = "enableXXX";
                char trigger_i_from[] = "i_fromXXX";
                char trigger_i_to[] = "i_toXXX";
                char trigger_o_from[] = "o_fromXXX";
                char trigger_o_to[] = "o_toXXX";
                char *name="", *enable, new_name[200]="", *i_from="", *i_to="", *o_from="", *o_to="";

                snprintf(trigger_name, sizeof(trigger_name), "name%d", i);
                snprintf(trigger_enable, sizeof(trigger_enable), "enable%d", i);
                snprintf(trigger_i_from, sizeof(trigger_i_from), "i_from%d", i);
                snprintf(trigger_i_to, sizeof(trigger_i_to), "i_to%d", i);
                snprintf(trigger_o_from, sizeof(trigger_o_from), "o_from%d", i);
                snprintf(trigger_o_to, sizeof(trigger_o_to), "o_to%d", i);
	
		name = websGetVar(wp, trigger_name, "");
		enable = websGetVar(wp, trigger_enable, "off");
		i_from = websGetVar(wp, trigger_i_from, NULL);
		i_to = websGetVar(wp, trigger_i_to, NULL);
		o_from = websGetVar(wp, trigger_o_from, NULL);
		o_to = websGetVar(wp, trigger_o_to, NULL);

		which = &trigger_variables[0];

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): %s=[%s] %s=[%s] %s=[%s] %s=[%s] %s=[%s]",__FUNCTION__,i,trigger_name,name,trigger_i_from,i_from,trigger_i_to,i_to,trigger_o_from,o_from,trigger_o_to,o_to);
#endif
		if(!i_from || !i_to || !o_from || !o_to)
			continue;

	
		if( (!strcmp(i_from,"0") || !strcmp(i_from,"")) && 
		    (!strcmp(i_to,"0") || !strcmp(i_to,"")) &&
		    (!strcmp(o_from,"0") || !strcmp(o_from,"")) && 
		    (!strcmp(o_to,"0") || !strcmp(o_to,"")) )	
			continue;

		if (!strcmp(i_from,"0") || !strcmp(i_from,"")) i_from = i_to;
		if (!strcmp(i_to,"0") || !strcmp(i_to,"")) i_to = i_from;
		if (!strcmp(o_from,"0") || !strcmp(o_from,"")) o_from = o_to;
		if (!strcmp(o_to,"0") || !strcmp(o_to,"")) o_to = o_from;
		
		if(atoi(i_from) > atoi(i_to))
			SWAP(i_from, i_to);
		
		if(atoi(o_from) > atoi(o_to))
			SWAP(o_from, o_to);

		if(strcmp(name,"")){
			if(!valid_name(wp, name, &which[0])){
				error = 1;
				continue;
			}
			else{
				filter_name(name, new_name, sizeof(new_name), SET);
			}
		}

		if(!valid_range(wp, i_from, &which[1]) || !valid_range(wp, i_to, &which[2])||
		   !valid_range(wp, o_from, &which[3]) || !valid_range(wp, o_to, &which[4])){
			error = 1;
			continue;
		}

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s:%s:both:%s-%s>%s-%s",
				cur == buf ? "" : " ", new_name, enable, i_from, i_to, o_from, o_to);

	}

	if(!error)
		nvram_set(v->name, buf);
	
}
#endif

/* Example:
 * name:[on|off]:[tcp|udp|both]:8000:80>100
 */

int
ej_port_forward_table(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int  which;

	static char word[256];
	char *next, *wordlist;
	char *name, *from, *to, *proto, *ip ,*enable;
	static char new_name[200];
	int temp;
	
        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
#ifdef ADVANCED_FORWARD_SUPPORT
	if(!strcmp(type, "advanced_status"))
	{
		char status[3];
		strcpy(status, nvram_safe_get("advanced_status"));
		if((!strcmp(status, "1") && which == 1) || (!strcmp(status, "0") && which == 0))
			return websWrite(wp,"checked");
		else
			return websWrite(wp,"");
	}
#endif


	wordlist = nvram_safe_get("forward_port");
	temp = which;
	
	foreach(word, wordlist, next) {
		if (which-- == 0) {
			enable = word;
			name = strsep(&enable, ":");
		 	if (!name || !enable)
                                continue;
			proto = enable;
			enable = strsep(&proto, ":");
			if (!enable || !proto)
				continue;
			from = proto;
			proto = strsep(&from, ":");
			if (!proto || !from)
				continue;
			to = from;
			from = strsep(&to, ":");
			if (!to || !from)
				continue;
			ip = to;
			to = strsep(&ip, ">");
			if (!ip || !to)
				continue;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): name=[%s] from=[%s] to=[%s] proto=[%s] ip=[%s] enable=[%s]",__FUNCTION__,part,temp,name,from,to,proto,ip,enable);
#endif

			if (!strcmp(type, "name")){
				filter_name(name, new_name, sizeof(new_name), GET);
				return websWrite(wp,"%s",new_name);
			}
			else if (!strcmp(type, "from"))
				return websWrite(wp,"%s",from);
			else if (!strcmp(type, "to"))
				return websWrite(wp,"%s",to);
			else if (!strcmp(type, "tcp")){		// use checkbox
				if(!strcmp(proto, "udp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"checked");
			}
			else if (!strcmp(type, "udp")){		//use checkbox
				if(!strcmp(proto, "tcp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"checked");
			}
			else if (!strcmp(type, "sel_tcp")){		// use select
				if(!strcmp(proto, "udp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"selected");
			}
			else if (!strcmp(type, "sel_udp")){		//use select
				if(!strcmp(proto, "tcp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"selected");
			}
			else if (!strcmp(type, "sel_both")){		//use select
				if(!strcmp(proto, "both"))
					return websWrite(wp,"selected");
				else
					return websWrite(wp,"");
			}
			else if (!strcmp(type, "ip"))
				return websWrite(wp,"%s",ip);
			else if (!strcmp(type, "enable")){
				if(!strcmp(enable, "on"))
					return websWrite(wp,"checked");
				else
					return websWrite(wp,"");
			}
		}
	}
 	if( !strcmp(type, "from") || !strcmp(type, "to") || !strcmp(type, "ip"))
		return websWrite(wp,"0");
	else if (!strcmp(type, "sel_both"))
		return websWrite(wp,"selected");
	else
		return websWrite(wp,"");
}

#ifdef PORT_TRIGGER_SUPPORT
/* Example:
 * name:on:both:1000-2000>3000-4000
 */

int
ej_port_trigger_table(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int  which;
	
	static char word[256];
	char *next, *wordlist;
	char *name=NULL, *enable=NULL, *proto=NULL, *i_from=NULL, *i_to=NULL, *o_from=NULL, *o_to=NULL;
	static char new_name[200];
	int temp;

        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	wordlist = nvram_safe_get("port_trigger");
	temp = which;
	
	foreach(word, wordlist, next) {
		if (which-- == 0) {
			enable = word;
			name = strsep(&enable, ":");
		 	if (!name || !enable)
                                continue;
			proto = enable;
			enable = strsep(&proto, ":");
		 	if (!enable || !proto)
                                continue;
			i_from = proto;
			proto = strsep(&i_from, ":");
		 	if (!proto || !i_from)
                                continue;
			i_to = i_from;
			i_from = strsep(&i_to, "-");
		 	if (!i_from || !i_to)
                                continue;
			o_from = i_to;
			i_to = strsep(&o_from, ">");
			if (!i_to || !o_from)
				continue;
			o_to = o_from;
			o_from = strsep(&o_to, "-");
			if (!o_from || !o_to)
				continue;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): i_from=[%s] i_to=[%s] o_from=[%s] o_to=[%s] enable=[%s] proto=[%s]",__FUNCTION__,type,temp,i_from,i_to,o_from,o_to,enable,proto);
#endif

			if (!strcmp(type, "name")){
				if(strcmp(name, "")){
					filter_name(name, new_name, sizeof(new_name), GET);
					return websWrite(wp,"%s",new_name);
				}
			}
			else if (!strcmp(type, "enable")){
				if(!strcmp(enable, "on"))
					return websWrite(wp,"checked");
				else
					return websWrite(wp,"");
			}
			else if (!strcmp(type, "sel_tcp")){		// use select
				if(!strcmp(proto, "udp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"selected");
			}
			else if (!strcmp(type, "sel_udp")){		//use select
				if(!strcmp(proto, "tcp"))
					return websWrite(wp,"");
				else
					return websWrite(wp,"selected");
			}
			else if (!strcmp(type, "sel_both")){		//use select
				if(!strcmp(proto, "both"))
					return websWrite(wp,"selected");
				else
					return websWrite(wp,"");;
			}
			else if (!strcmp(type, "i_from"))
				return websWrite(wp,"%s",i_from);
			else if (!strcmp(type, "i_to"))
				return websWrite(wp,"%s",i_to);
			else if (!strcmp(type, "o_from"))
				return websWrite(wp,"%s",o_from);
			else if (!strcmp(type, "o_to"))
				return websWrite(wp,"%s",o_to);
		}
	}
 	if(!strcmp(type, "name"))
		return websWrite(wp,"");
	else
		return websWrite(wp,"0");
}
#endif

//written by leijun 2004-0309-15:45
int ej_forward_upnp(int eid, webs_t wp, int argc, char_t **argv)
{
	char tmp_name[32], upnp_content[256];
	char old_name[256] = " ", new_name[128] = " ";
	char *type;
	int which, i_enable = 0, i_proto = 0, i_ext_port = 0, i_int_port = 0, i_int_ip = 0, tmp = 0,pvc_connection=0;
	
    	if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
         	websError(wp, 400, "Insufficient args\n");
         	return -1;
    	}
#if 1	
	pvc_connection = atoi(nvram_safe_get("singleportforward_connection"));
	sprintf(tmp_name,"upnp_content_num_%d",pvc_connection);	
	if (atoi(nvram_safe_get(tmp_name)) <= which) return -1;
	
	sprintf(tmp_name,"upnp_content_%d_%d",pvc_connection,which);
	
#else
	if (atoi(nvram_safe_get("upnp_content_num")) <= which) return -1;
	sprintf(tmp_name,"upnp_content%d",which);
#endif

	if (!nvram_invmatch(tmp_name, ""))
		goto def;
	strcpy(upnp_content, nvram_safe_get(tmp_name));
	if (!strcmp(upnp_content, tmp_name)) return 0;

	if (strlen(upnp_content) > 10)
	{
		sscanf(upnp_content, "%d:%d:%d:%d:%d:%d:%s", &i_enable, &i_proto, &i_ext_port, &i_int_port, &i_int_ip, &tmp, old_name);
		
		filter_name(old_name, new_name, sizeof(new_name), GET);
		if (!strcasecmp(type, "ext_port"))
			return  websWrite(wp,"%d", i_ext_port);
		else if (!strcasecmp(type, "name"))
			return  websWrite(wp,"%s", new_name);
	
		else if (!strcasecmp(type, "sel_tcp")){		// use select
			if(i_proto == 2)
				return websWrite(wp,"");
			else
				return websWrite(wp,"selected");
		}
		else if (!strcasecmp(type, "sel_udp")){		//use select
			if(i_proto == 1)
				return websWrite(wp,"");
			else
				return websWrite(wp,"selected");
		}
		else if (!strcasecmp(type, "int_port"))
			return websWrite(wp, "%d", i_int_port);
	
		else if (!strcasecmp(type, "ip"))
			return  websWrite(wp, "%d", i_int_ip);
	
		else if (!strcasecmp(type, "enabled"))
		{
			if (i_enable)
				return websWrite(wp,"checked");
			else return websWrite(wp,"");
		}
	}
	return  websWrite(wp, " ");	
def:
 	if( !strcmp(type, "ext_port") || !strcmp(type, "int_port") || !strcmp(type, "ip") )
		return  websWrite(wp,"0");
	else return 1;
}

#ifdef SPECIAL_FORWARD_SUPPORT

/* Example:
 * name:[on|off]:[tcp|udp|both]:ext_port>ip:int_port
 */
int
ej_spec_forward_table(int eid, webs_t wp, int argc, char_t **argv)
{
	static char word[256];
	char *next, *wordlist, *type;
	char *name=NULL, *from=NULL, *to=NULL, *proto=NULL, *ip=NULL ,*enable=NULL;
	int temp, which;
	
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s():",__FUNCTION__);
#endif
        if (ejArgs(argc, argv, "%s %d", &type, &which) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	wordlist = nvram_safe_get("forward_spec");
	if(!wordlist)	goto def;
	temp = which;
	
	foreach(word, wordlist, next) {
		if (which-- == 0) {
			enable = word;
			name = strsep(&enable, ":");
		 	if (!name || !enable)
                                continue;
			proto = enable;
			enable = strsep(&proto, ":");
			if (!enable || !proto)
				continue;
			from = proto;
			proto = strsep(&from, ":");
			if (!proto || !from)
				continue;
			to = from;
			from = strsep(&to, ">");
			if (!to || !from)
				continue;
			ip = to;
			to = strsep(&ip, ":");
			if (!ip || !to)
				continue;
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%s,%d): name=[%s] from=[%s] to=[%s] proto=[%s] ip=[%s] enable=[%s]",__FUNCTION__,type,temp,name,from,to,proto,ip,enable);
#endif

			if (!strcmp(type, "name"))
				return  websWrite(wp,"%s",name);
			else if (!strcmp(type, "from"))
				return  websWrite(wp,"%s",from);
			else if (!strcmp(type, "to"))
				return  websWrite(wp,"%s",to);
			else if (!strcmp(type, "tcp")){
				if(!strcmp(proto,"tcp") || !strcmp(proto,"both"))
					return  websWrite(wp,"checked");				
				return  websWrite(wp," ");

			}
			else if (!strcmp(type, "udp")){
				if(!strcmp(proto,"udp") || !strcmp(proto,"both"))
					return  websWrite(wp,"checked");				
				return  websWrite(wp," ");

			}
			else if (!strcmp(type, "ip"))
				return  websWrite(wp,"%s",ip);
			else if (!strcmp(type, "enable"))
				if(!strcmp(enable,"on"))
					return  websWrite(wp,"checked");
				return  websWrite(wp," ");
		}
	}
def:
 	if( !strcmp(type, "from") || !strcmp(type, "to") || !strcmp(type, "ip") )
		return  websWrite(wp, "0");
	else
		return  1;
}
#endif
