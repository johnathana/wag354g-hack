
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
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h> 
#include <net/if_arp.h>
#include <stdarg.h>


#include <broadcom.h>
#include <cyutils.h>
#include <code_pattern.h>

/* Format:
 * type = SET :  " " => "&nbsp;" , ":" => "&semi;"
 * type = GET :  "&nbsp;" => " " , "&semi;" => ":"
 * Example:
 * name1 = test 123:abc
 * filter_name("name1", new_name, SET); new_name="test&nbsp;123&semi;abc"
 * name2 = test&nbsp;123&semi;abc
 * filter_name("name2", new_name, GET); new_name="test 123:abc"
 */
int
filter_name(char *old_name, char *new_name, size_t size, int type)
{
        int i, j, match;

        struct pattern {
                char ch;
                char *string;
        };

        struct pattern patterns[] = {
                { ' ', "&nbsp;" },
                { ':', "&semi;" },
        };

        struct pattern *v;

	strcpy(new_name,"");	

        switch(type){
        case SET:
                for(i=0 ; *(old_name+i) ; i++){	
                        match = 0;
                        for(v = patterns ; v < &patterns[STRUCT_LEN(patterns)] ; v++) {
                                if(*(old_name+i) == v->ch){
					if(strlen(new_name) + strlen(v->string) > size){	// avoid overflow
                                                cprintf("%s(): overflow\n",__FUNCTION__);
                                                new_name[strlen(new_name)] = '\0';
                                                return 1;
                                        }
                                        sprintf(new_name+strlen(new_name), "%s", v->string);
                                        match = 1;
                                        break;
                                }
                        }
                        if(!match){
				if(strlen(new_name) + 1 > size){
                                        cprintf("%s(): overflow\n",__FUNCTION__);	// avoid overflow
                                        new_name[strlen(new_name)] = '\0';
                                        return 1;
                                }
                                sprintf(new_name+strlen(new_name),"%c",*(old_name+i));
			}
                }

                break;
        case GET:
                for(i=0,j=0 ; *(old_name+j) ; j++){
                        match = 0;
                        for(v = patterns ; v < &patterns[STRUCT_LEN(patterns)] ; v++) {
                                if(!memcmp(old_name+j, v->string, strlen(v->string))){
                                        *(new_name+i) = v->ch ;
                                        j = j + strlen(v->string) - 1;
                                        match = 1;
                                        break;
                                }
                        }
                        if(!match)
                                *(new_name+i) = *(old_name+j);

                        i++;
                }
                *(new_name+i)='\0';
                break;
        default:
                cprintf("%s():Invalid type!\n",__FUNCTION__);
                break;
        }
        //cprintf("%s():new_name=[%s]\n", __FUNCTION__, new_name);

	return 1;
}

int
ej_compile_date(int eid, webs_t wp, int argc, char_t **argv)
{
	char year[4], mon[3], day[2];
	char string[20];

	sscanf(__DATE__, "%s %s %s", mon, day, year);
	snprintf(string, sizeof(string), "%s. %s, %s", mon, day, year);

	return  websWrite(wp, "%s", string);
}

int
ej_compile_time(int eid, webs_t wp, int argc, char_t **argv)
{
	return  websWrite(wp, "%s", __TIME__);
}

int
ej_get_firmware_version(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp,"%s%s", CYBERTAN_VERSION, MINOR_VERSION);
}

int
ej_get_web_page_name(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, "%s.asp", websGetVar(wp, "submit_button", "index"));
}

int
ej_get_model_name(int eid, webs_t wp, int argc, char_t **argv)
{
	//return websWrite(wp,"%s",MODEL_NAME);
	return websWrite(wp,"%s", nvram_safe_get("router_name"));
}

int
ej_show_logo(int eid, webs_t wp, int argc, char_t **argv)
{
#if OEM == LINKSYS
	return 0;
#elif OEM == ALLNET
	return websWrite(wp,"<CENTER><IMG SRC=./image/Allnet.jpg></CENTER><BR>");
#else
	return 0;
#endif
}

#ifdef SYSLOG_SUPPORT
void 
LOG(int level, const char *fmt,...)
{
        char buf[2000];
        va_list args;
        openlog("web", LOG_PID, LOG_DAEMON);
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);                   //bk - otherwise ppc segfaults
        va_start(args, fmt);    //bk
        //vfprintf(stderr,fmt, args);
        //fflush(stderr);
        syslog(level, "%s", buf);
        va_end(args);
}
#endif

int
protocol_to_num(char *proto)
{
	if(!strcmp(proto, "icmp"))
		return 1;
	else if(!strcmp(proto, "tcp"))
		return 6;
	else if(!strcmp(proto, "udp"))
		return 17;
	else if(!strcmp(proto, "both"))
		return 23;
	else
		return 0;
}

char *
num_to_protocol(int num)
{
	switch(num){
		case 1:
			return "icmp";
		case 6:
			return "tcp";
		case 17:
			return "udp";
		case 23:
			return "both";
		default:
			return "unknown";
	}
}
