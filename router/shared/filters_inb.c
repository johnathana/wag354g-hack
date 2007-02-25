
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

/* 
   Format:
     filter_rule{1...10}=$STAT:1$NAME:test1$

	Below for new UI:
	
	$STAT:0$NAME:$DENY:0$$	==> Disable/Allow
	$STAT:2$NAME:$DENY:1$$	==> Enable/Allow
	$STAT:1$NAME:$DENY:1$$	==> Enable/Deny
	$STAT:0$NAME:$DENY:1$$	==> Disable/Deny

	Below for old UI:

	$STAT:1$NAME:$$	==> Deny (Disable Internet Access for Listed PCs)
	$STAT:2$NAME:$$	==> Allow (Enable Internet Access for Listed PCs)

   Format:
     filter_tod{1...10} = hr:min hr:min wday
     filter_tod_buf{1...10} = sun mon tue wed thu fri sat	 //only for web page read
   Example:
     Everyday and 24-hour
     filter_todXX = 0:0 23:59 0-0
     filter_tod_bufXX = 7	(for web)

     From 9:55 to 22:00 every sun, wed and thu
     filter_todXX = 9:55 22:00 0,3-4
     filter_tod_bufXX = 1 0 1 1 0 0 0	(for web)

   Format:
     filter_ip_grp{1...10} = ip1 ip2 ip3 ip4 ip5 ip6 ip_r1-ipr2 ip_r3-ip_r4
     filter_ip_mac{1...10} = 00:11:22:33:44:55 00:12:34:56:78:90
 
   Format:
     filter_port=udp:111-222 both:22-33 disable:22-333 tcp:11-22222

   Converting Between AM/PM and 24 Hour Clock:
    Converting from AM/PM to 24 hour clock:
     12:59 AM -> 0059 	 (between 12:00 AM and 12:59 AM, subtract 12 hours)
     10:00 AM -> 1000    (between 1:00 AM and 12:59 PM, a straight conversion)
     10:59 PM -> 2259    (between 1:00 PM and 11:59 PM, add 12 hours)
    Converting from 24 hour clock to AM/PM
     0059 -> 12:59 AM    (between 0000 and 0059, add 12 hours)
     0100 -> 1:00  AM    (between 0100 and 1159, straight converion to AM)
     1259 -> 12:59 PM    (between 1200 and 1259, straight converion to PM)
     1559 -> 3:59  PM    (between 1300 and 2359, subtract 12 hours)
     
*/

int filter_inb_id  = 1;
extern int day_all, week0, week1, week2, week3, week4, week5, week6;
extern int start_week, end_week;
extern int time_all, start_hour, start_min, start_time, end_hour, end_min, end_time;
extern int tod_data_null;

void
validate_filter_inb_mix(webs_t wp, char *value, struct variable *v)
{
	int i=0;
	char buf[1000] = "", *cur = buf, wan_ipaddr[20];
	char *lip = "", *lip_type = "", *port_type = "", *port_range_start = "", *port_range_end = "", *prot = "";

	struct variable filter_inb_mix_variables[] = {
		{ longname: "Filter Inbound LAN IP address", argv: ARGV("0", "254") },
		{ longname: "Filter Inbound from Port", argv: ARGV("0", "65535") },
		{ longname: "Filter Inbound to Port", argv: ARGV("0", "65535") },
		{ longname: "Filter Inbound Protocol",NULL },
	 	{ longname: "Filter Inbound WAN IP Address1", argv: ARGV("0","255") },
	 	{ longname: "Filter Inbound WAN IP Address1", argv: ARGV("0","255") },
	 	{ longname: "Filter Inbound WAN IP Address1", argv: ARGV("0","255") },
	 	{ longname: "Filter Inbound WAN IP Address1", argv: ARGV("0","254") },
	}, *which;
	char _filter_inb_mix[] = "filter_mix_grpXXX", wan_host[] = "inbwanXXX",prot_num[] = "inbproXXX", 		     port_sel[] = "inbportXXX", port_start[] = "inbstartXXX", port_end[] = "inbendXXX", lan_type[] = "alllanXXX", lan_host[] = "inblanXXX";

	which = &filter_inb_mix_variables[0];

	for (i=0; i<4 ; i++) {
 	snprintf(wan_host, sizeof(wan_host), "inbwan%d", i);
 	snprintf(prot_num, sizeof(prot_num), "inbpro%d", i);
 	snprintf(port_sel, sizeof(port_sel), "inbport%d", i);
 	snprintf(port_start, sizeof(port_start), "inbstart%d", i);
 	snprintf(port_end, sizeof(port_end), "inbend%d", i);
	snprintf(lan_type, sizeof(lan_type), "alllan%d", i);
 	snprintf(lan_host, sizeof(lan_host), "inblan%d", i);

	get_merge_ipaddr(wan_host,wan_ipaddr);
	prot = websGetVar(wp, prot_num, NULL);
	port_type = websGetVar(wp, port_sel, "0");
	port_range_start = websGetVar(wp, port_start, "0");
	port_range_end   = websGetVar(wp, port_end, "0");
	lip_type = websGetVar(wp, lan_type, "0");
	lip = websGetVar(wp, lan_host, "0");

	if(atoi(port_range_start) > atoi(port_range_end))
		SWAP(port_range_start,port_range_end);

	cur += snprintf(cur, buf + sizeof(buf) - cur, "%s$FROM:%s$PROT:%s$PTYPE:%d$PORT:%d-%d$LTYPE:%d$TO:%s",
			cur == buf ? "" : "<&nbsp;>", wan_ipaddr, prot,atoi(port_type),
			atoi(port_range_start), atoi(port_range_end), atoi(lip_type), lip);
	}
	if(strcmp(buf, ""))
		strcat(buf,"<&nbsp;>");

 	snprintf(_filter_inb_mix, sizeof(_filter_inb_mix), "filter_mix_grp%s", nvram_safe_get("filter_inb_id"));
	nvram_set(_filter_inb_mix, buf);

 }

int
save_inb_policy(webs_t wp)
{
	char *f_id, *f_name, *f_status, *f_status2;
	char buf[256] = "";
	struct variable filter_variables[] = {
		{ longname: "Filter ID", argv: ARGV("1","10") },
		{ longname: "Filter Status", argv: ARGV("0","1","2") },
		{ longname: "Filter Status", argv: ARGV("deny","allow") },

	}, *which;
	char filter_buf[] = "filter_inb_ruleXXX";
	which = &filter_variables[0];

	f_id = websGetVar(wp, "f_id_inb", NULL);
	f_name = websGetVar(wp, "f_name_inb", NULL);
	f_status = websGetVar(wp, "f_status_inb", NULL);	// 0=>Disable / 1,2=>Enable
	f_status2 = websGetVar(wp, "f_status2_inb", NULL);	// deny=>Deny / allow=>Allow
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): f_id=[%s] f_name=[%s] f_status=[%s]",__FUNCTION__,f_id_inb,f_name_inb,f_status_inb);
#endif
	if(!f_id || !f_name || !f_status || !f_status2){
		error_value = 1;
		return -1;
	}
	if(!valid_range(wp, f_id, &which[0])){
		error_value = 1;
		return -1;
        }
	if(!valid_choice(wp, f_status, &which[1])){
		error_value = 1;
		return -1;
        }
	if(!valid_choice(wp, f_status2, &which[2])){
		error_value = 1;
		return -1;
        }

	validate_filter_inb_tod(wp);

 	snprintf(filter_buf, sizeof(filter_buf), "filter_inb_rule%s", nvram_safe_get("filter_inb_id"));

	// Add $DENY to decide that users select Allow or Deny, if status is Disable	// 2003/10/21
	snprintf(buf,sizeof(buf),"$STAT:%s$NAME:%s$DENY:%d$$", f_status, f_name, !strcmp(f_status2,"deny") ? 1 : 0);

	nvram_set(filter_buf,buf);

	return 0;
}

int
validate_filter_inb_tod(webs_t wp)
{
	char buf[256] = "";
	char tod_buf[20];
	struct variable filter_tod_variables[] = {
		{ longname: "Tod name", argv: ARGV("20") },
		{ longname: "Tod Status", argv: ARGV("0", "1","2") },

	}, *which;


	char *day_all, *week0, *week1, *week2, *week3, *week4, *week5, *week6;
	char *time_all, *start_hour, *start_min, *start_time, *end_hour, *end_min, *end_time;
	int _start_hour, _start_min, _end_hour, _end_min;
	char time[20];
	int week[7];
	int i, flag=-1, dash=0;
	char filter_tod[] = "filter_inb_todXXX";
	char filter_tod_buf[] = "filter_inb_tod_bufXXX";

	which = &filter_tod_variables[0];

	day_all = websGetVar(wp, "day_all_inb", "0");
	week0 = websGetVar(wp, "week0_inb", "0");
	week1 = websGetVar(wp, "week1_inb", "0");
	week2 = websGetVar(wp, "week2_inb", "0");
	week3 = websGetVar(wp, "week3_inb", "0");
	week4 = websGetVar(wp, "week4_inb", "0");
	week5 = websGetVar(wp, "week5_inb", "0");
	week6 = websGetVar(wp, "week6_inb", "0");
	time_all = websGetVar(wp, "time_all_inb", "0");
	start_hour = websGetVar(wp, "start_hour_inb", "0");
	start_min = websGetVar(wp, "start_min_inb", "0");
	start_time = websGetVar(wp, "start_time_inb", "0");
	end_hour = websGetVar(wp, "end_hour_inb", "0");
	end_min = websGetVar(wp, "end_min_inb", "0");
	end_time = websGetVar(wp, "end_time_inb", "0");

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): day_all=[%s] week=[%s %s %s %s %s %s %s] time_all=[%s] start=[%s:%s:%s] end=[%s:%s:%s]",__FUNCTION__,filter_id,day_all,week0,week1,week2,week3,week4,week5,week6,time_all,start_hour,start_min,start_time,end_hour,end_min,end_time);
#endif
	//if(atoi(time_all) == 0)
	//	if(!start_hour || !start_min || !start_time || !end_hour || !end_min || !end_time)
	//		return 1;

	if(atoi(day_all) == 1){
		strcpy(time,"0-6");
		strcpy(tod_buf,"7");
	}
	else{
		week[0] = atoi(week0);
		week[1] = atoi(week1);
		week[2] = atoi(week2);
		week[3] = atoi(week3);
		week[4] = atoi(week4);
		week[5] = atoi(week5);
		week[6] = atoi(week6);
		strcpy(time,"");

		for(i = 0 ; i < 7 ; i ++){
			if(week[i] == 1){
				if(i == 6){
					 if(dash == 0 && flag == 1)
                                                sprintf(time+strlen(time),"%c",'-');
					 sprintf(time+strlen(time),"%d",i);
				}
				else if(flag == 1 && dash == 0){
					sprintf(time+strlen(time),"%c",'-');
					dash = 1;
				}
				else if(dash ==0){
					sprintf(time+strlen(time),"%d",i);
					flag = 1;
					dash = 0;
				}
			}
			else{
				if(!strcmp(time,""))	continue;
				if(dash == 1)
					sprintf(time+strlen(time),"%d",i-1);
				if(flag != 0)
					sprintf(time+strlen(time),"%c",',');
				flag = 0;
				dash = 0;
			}
		}
		if(time[strlen(time)-1] == ',')
			time[strlen(time)-1] = '\0';

		snprintf(tod_buf,sizeof(tod_buf),"%s %s %s %s %s %s %s",week0, week1, week2, week3, week4, week5, week6);
	}
	if(atoi(time_all) == 1){
		_start_hour = 0;
		_start_min = 0;
		_end_hour = 23;
		_end_min = 59;
	}
	else{
		_start_hour = atoi(start_time) ? 12 + atoi(start_hour) : atoi(start_hour);
		_start_min = atoi(start_min);
		_end_hour = atoi(end_time) ? 12 + atoi(end_hour) : atoi(end_hour);
		_end_min = atoi(end_min);
	}

	sprintf(buf,"%d:%d %d:%d %s",_start_hour,_start_min,_end_hour,_end_min,time);
 	snprintf(filter_tod, sizeof(filter_tod), "filter_inb_tod%s", nvram_safe_get("filter_inb_id"));
 	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_inb_tod_buf%s", nvram_safe_get("filter_inb_id"));

	nvram_set(filter_tod, buf);
	nvram_set(filter_tod_buf, tod_buf);

	return 0;

}

int
delete_inb_policy(webs_t wp, int which)
{
	char filter_rule[] = "filter_inb_ruleXXX";
	char filter_tod[] = "filter_inb_todXXX";
	char filter_tod_buf[] = "filter_inb_tod_bufXXX";
	char filter_mix_grp[] = "filter_mix_grpXXX";

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): which=[%d]",__FUNCTION__,which);
#endif

 	snprintf(filter_rule, sizeof(filter_rule), "filter_inb_rule%d", which);
 	snprintf(filter_tod, sizeof(filter_tod), "filter_inb_tod%d", which);
 	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_inb_tod_buf%d", which);
 	snprintf(filter_mix_grp, sizeof(filter_mix_grp), "filter_mix_grp%d", which);

	nvram_set(filter_rule,"");
	nvram_set(filter_tod,"");
	nvram_set(filter_tod_buf,"");
	nvram_set(filter_mix_grp,"$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>$FROM:0.0.0.0$PROT:tcp$PTYPE:0$PORT:0-0$LTYPE:0$TO:0<&nbsp;>");

	nvram_set("filter_inb_summary", "1");

	return 1;
}

int
single_delete_inb_policy(webs_t wp)
{
	int ret = 0;
	char *id = nvram_safe_get("filter_inb_id");

	ret = delete_inb_policy(wp, atoi(id));

	return ret;
}

int
summary_delete_inb_policy(webs_t wp)
{
	int i, ret = 0;

	for(i=1 ; i<=10 ; i++){
		char filter_sum[] = "sum_inbXXX";
		char *sum;
	 	snprintf(filter_sum, sizeof(filter_sum), "sum_inb%d", i);
		sum = websGetVar(wp, filter_sum, NULL);
		if(sum)
			ret += delete_inb_policy(wp, i);
	}

	return ret;
}

void
validate_filter_inb_policy(webs_t wp, char *value, struct variable *v)
{
	char *f_id = websGetVar(wp, "f_id_inb", NULL);

	if(f_id)
		nvram_set("filter_inb_id", f_id);
	else
		nvram_set("filter_inb_id", "1");

	save_inb_policy(wp);

}

/*   Format: 21:21:tcp:FTP(&nbsp;)500:1000:both:TEST1
 *
 */
int
ej_filter_inb_policy_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0;

	for(i = 1 ; i <= 10 ; i ++ ){
		char filter[] = "filter_inb_ruleXXX";
		char *data="";
        	char name[50]="";
 		snprintf(filter, sizeof(filter), "filter_inb_rule%d", i);
		data = nvram_safe_get(filter);

		if(data && strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","");	// get name value
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(%d): data=[%s] name=[%s]",__FUNCTION__,i,data,name);
#endif
		}
		ret += websWrite(wp,"<option value=%d %s>%d ( %s ) </option>\n",
			i,
			(atoi(nvram_safe_get("filter_inb_id")) == i ? "selected" : ""),
			i,
			name);
	}
	return ret;
}

int
ej_filter_inb_mix_get(int eid, webs_t wp, int argc, char_t **argv)
{

	char *type, *part;
	int ret = 0,len,rangeS,rangeE,WIP1,WIP2,WIP3,WIP4;
	char mix_grp[] = "filter_mix_grpXXX";
	char *data="";

	char *token = "<&nbsp;>";
    	char value_buf[50]="";
	char mix1[100]="",mix2[100]="",mix3[100]="",mix4[100]="";

        if (ejArgs(argc, argv, "%s %s", &type, &part) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	snprintf(mix_grp, sizeof(mix_grp), "filter_mix_grp%s", nvram_safe_get("filter_inb_id"));
	data = nvram_safe_get(mix_grp);

	find_each(mix1, sizeof(mix1), data, token, 0, "");
	strcat(mix1,"$");
	find_each(mix2, sizeof(mix2), data, token, 1, "");
	strcat(mix2,"$");
	find_each(mix3, sizeof(mix3), data, token, 2, "");
	strcat(mix3,"$");
	find_each(mix4, sizeof(mix4), data, token, 3, "");
	strcat(mix4,"$");

	if(!strcmp(type, "inblan0") || !strcmp(type, "inblan1") ||
	   !strcmp(type, "inblan2") || !strcmp(type, "inblan3")){
		if(!strcmp(type, "inblan0"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$TO:","");
		if(!strcmp(type, "inblan1"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$TO:","");
		if(!strcmp(type, "inblan2"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$TO:","");
		if(!strcmp(type, "inblan3"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$TO:","");
//		cprintf("%s: \n mix1=[%s]\n mix2=[%s]\n mix3=[%s]\n mix4=[%s]",__FUNCTION__,mix1,mix2,mix3,mix4);
		ret = websWrite(wp, "%s", value_buf);
	}
	else if(!strcmp(type, "inbpro0") || !strcmp(type, "inbpro1") ||
		!strcmp(type, "inbpro2") || !strcmp(type, "inbpro3")){
		if(!strcmp(type, "inbpro0"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$PROT:","");
		if(!strcmp(type, "inbpro1"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$PROT:","");
		if(!strcmp(type, "inbpro2"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$PROT:","");
		if(!strcmp(type, "inbpro3"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$PROT:","");

		if (!strcmp(value_buf, part)){
			ret = websWrite(wp, "%s", "selected");}
		else{
			ret = websWrite(wp, "%s", " ");}
	}
	else if(!strcmp(type, "alllan0") || !strcmp(type, "alllan1") ||
		!strcmp(type, "alllan2") || !strcmp(type, "alllan3")){
		if(!strcmp(type, "alllan0"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$LTYPE:","");
		if(!strcmp(type, "alllan1"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$LTYPE:","");
		if(!strcmp(type, "alllan2"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$LTYPE:","");
		if(!strcmp(type, "alllan3"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$LTYPE:","");

		if (!strcmp(value_buf, part)){
			ret = websWrite(wp, "%s", "checked");}
		else{
			ret = websWrite(wp, "%s", " ");}
	}
	else if(!strcmp(type, "inbport0") || !strcmp(type, "inbport1") ||
		!strcmp(type, "inbport2") || !strcmp(type, "inbport3")){
		if(!strcmp(type, "inbport0"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$PTYPE:","");
		if(!strcmp(type, "inbport1"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$PTYPE:","");
		if(!strcmp(type, "inbport2"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$PTYPE:","");
		if(!strcmp(type, "inbport3"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$PTYPE:","");

		if (!strcmp(value_buf, part)){
			ret = websWrite(wp, "%s", "checked");}
		else{
			ret = websWrite(wp, "%s", " ");}
	}
	else if(!strcmp(type, "inbport_range0") || !strcmp(type, "inbport_range1") ||
		!strcmp(type, "inbport_range2") || !strcmp(type, "inbport_range3")){
		if(!strcmp(type, "inbport_range0"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$PORT:","");
		if(!strcmp(type, "inbport_range1"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$PORT:","");
		if(!strcmp(type, "inbport_range2"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$PORT:","");
		if(!strcmp(type, "inbport_range3"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$PORT:","");

		sscanf(value_buf, "%d-%d", &rangeS,&rangeE);
		if(!strcmp(part, "0")){
			ret = websWrite(wp, "%d", rangeS);}
		else{
			ret = websWrite(wp, "%d", rangeE);}
	}
	else if(!strcmp(type, "inbwan0_") || !strcmp(type, "inbwan1_") ||
		!strcmp(type, "inbwan2_") || !strcmp(type, "inbwan3_")){
		if(!strcmp(type, "inbwan0_"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix1[0], "$FROM:","");
		if(!strcmp(type, "inbwan1_"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix2[0], "$FROM:","");
		if(!strcmp(type, "inbwan2_"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix3[0], "$FROM:","");
		if(!strcmp(type, "inbwan3_"))
			find_match_pattern(value_buf, sizeof(value_buf), &mix4[0], "$FROM:","");

		sscanf(value_buf, "%d.%d.%d.%d", &WIP1, &WIP2, &WIP3, &WIP4);
		if(!strcmp(part, "0")){
			ret = websWrite(wp, "%d", WIP1);}
		else if(!strcmp(part, "1")){
			ret = websWrite(wp, "%d", WIP2);}
		else if(!strcmp(part, "2")){
			ret = websWrite(wp, "%d", WIP3);}
		else if(!strcmp(part, "3")){
			ret = websWrite(wp, "%d", WIP4);}
	}
	return ret;
}

int
ej_filter_inb_policy_get(int eid, webs_t wp, int argc, char_t **argv)
{

	char *type, *part;
	int ret = 0;


        if (ejArgs(argc, argv, "%s %s", &type, &part) < 2) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	if(!strcmp(type,"f_inb_name")){
     		char name[50]="";
		char filter[] = "filter_inb_ruleXXX";
		char *data="";
 		snprintf(filter, sizeof(filter), "filter_inb_rule%s", nvram_safe_get("filter_inb_id"));
		data = nvram_safe_get(filter);
		if(strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","");	// get name value
			ret = websWrite(wp,"%s",name);
		}
	}
	else if(!strcmp(type,"f_inb_status")){
     		char status[50]="", deny[50]="";
		char filter[] = "filter_inb_ruleXXX";
		char *data="";
 		snprintf(filter, sizeof(filter), "filter_inb_rule%s", nvram_safe_get("filter_inb_id"));
		data = nvram_safe_get(filter);
		if(strcmp(data,"")){	// have data
			find_match_pattern(status, sizeof(status), data, "$STAT:","1");	// get status value
			find_match_pattern(deny, sizeof(deny), data, "$DENY:", "");	// get deny value
			if(!strcmp(deny,"")){	// old format
				if(!strcmp(status,"0") || !strcmp(status,"1"))
					strcpy(deny,"1");	// Deny
				else
					strcpy(deny,"0");	// Allow
			}
			if(!strcmp(part,"disable")){
				if(!strcmp(status,"0"))
					ret = websWrite(wp,"checked");
			}
			else if(!strcmp(part,"enable")){
				if(strcmp(status,"0"))
					ret = websWrite(wp,"checked");
			}
			else if(!strcmp(part,"deny")){
				if(!strcmp(deny,"1"))
					ret = websWrite(wp,"checked");
			}
			else if(!strcmp(part,"allow")){
				if(!strcmp(deny,"0"))
					ret = websWrite(wp,"checked");
			}
			else if(!strcmp(part,"onload_status")){
				if(!strcmp(deny,"1"))
					ret = websWrite(wp,"deny");
				else
					ret = websWrite(wp,"allow");

			}
			else if(!strcmp(part,"init")){
				if(!strcmp(status,"1"))
					ret = websWrite(wp,"disable");
				else if(!strcmp(status,"2"))
					ret = websWrite(wp,"enable");
				else
					ret = websWrite(wp,"disable");
			}
		}
		else{	// no data
			if(!strcmp(part,"disable"))
				ret = websWrite(wp,"checked");
			else if(!strcmp(part,"allow"))		// default policy is allow, 2003-10-21
				ret = websWrite(wp,"checked");
			else if(!strcmp(part,"onload_status"))	// default policy is allow, 2003-10-21
				ret = websWrite(wp,"allow");
			else if(!strcmp(part,"init"))
				ret = websWrite(wp,"disable");
		}
	}
	return ret;
}

int
filter_inb_tod_init(int which)
{
	char filter_tod[] = "filter_inb_todXXX";
	char filter_tod_buf[] = "filter_inb_tod_bufXXX";
	char  *tod_data, *tod_buf_data;
	char temp[3][20];
	int ret;

	tod_data_null = 0;
	day_all = week0 = week1 = week2 = week3 = week4 = week5 = week6 = 0;
	time_all = start_hour = start_min = start_time = end_hour = end_min = end_time = 0;
	start_week = end_week = 0;

	snprintf(filter_tod, sizeof(filter_tod), "filter_inb_tod%d", which);
	snprintf(filter_tod_buf, sizeof(filter_tod_buf), "filter_inb_tod_buf%d", which);


	/* Parse filter_tod{1...10} */
	tod_data = nvram_safe_get(filter_tod);
	if(!tod_data)	return -1;	// no data
	if(strcmp(tod_data,"")){
		sscanf(tod_data,"%s %s %s",temp[0],temp[1],temp[2]);
		sscanf(temp[0],"%d:%d",&start_hour,&start_min);
		sscanf(temp[1],"%d:%d",&end_hour,&end_min);
		ret = sscanf(temp[2],"%d-%d",&start_week,&end_week);
		if(ret == 1)	end_week = start_week;

		if(start_hour == 0 && start_min ==0 && end_hour == 23 && end_min== 59){	// 24 Hours
			time_all = 1;
			start_hour = end_hour = 0;
			start_min = start_time = end_min = end_time = 0;
		}
		else{	// check AM or PM
			time_all = 0;
			if(start_hour > 11 ){
				start_hour = start_hour - 12;
				start_time = 1;
			}
			if(end_hour > 11 ){
				end_hour = end_hour - 12;
				end_time = 1;
			}
		}
	}
	else{	// default Everyday and 24 Hours
		tod_data_null = 1;
		day_all = 1;
		time_all = 1;
	}

	if(tod_data_null == 0){
		/* Parse filter_tod_buf{1...10} */
		tod_buf_data = nvram_safe_get(filter_tod_buf);
		if(!strcmp(tod_buf_data,"7")){
				day_all = 1;
		}
		else if(strcmp(tod_buf_data,"")){
			sscanf(tod_buf_data,"%d %d %d %d %d %d %d",&week0, &week1, &week2, &week3, &week4, &week5, &week6);
			day_all = 0;
		}
	}

#ifdef MY_DEBUG
	if(tod_data_null == 1)
		LOG(LOG_DEBUG,"%s(%d): tod data null",__FUNCTION__,which);
	else
		LOG(LOG_DEBUG,"%s(%d):day_all=[%d] week=[%d %d %d %d %d %d %d] time_all=[%d] [%d %d %d %d %d %d]",__FUNCTION__,which,day_all,week0,week1,week2,week3,week4,week5,week6,time_all,start_hour,start_min,start_time,end_hour,end_min,end_time);
#endif
	return 0;
}

int
ej_filter_inb_tod_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int ret=0 ,i;

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	filter_inb_tod_init(atoi(nvram_safe_get("filter_inb_id")));

	if(!strcmp(type,"day_all_init")){
		if(day_all == 0)
			ret = websWrite(wp,"1");
		else
			ret = websWrite(wp,"0");
	}
	else if(!strcmp(type,"time_all_init")){
		if(time_all == 0)
			ret = websWrite(wp,"1");
		else
			ret = websWrite(wp,"0");
	}
	else if(!strcmp(type,"day_all_inb")){
		ret = websWrite(wp,"%s",day_all == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"start_week")){
		ret = websWrite(wp,"%d",start_week);
	}
	else if(!strcmp(type,"end_week")){
		ret = websWrite(wp,"%d",end_week);
	}
	else if(!strcmp(type,"week0_inb")){	// Sun
		ret = websWrite(wp,"%s",week0 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week1_inb")){	// Mon
		ret = websWrite(wp,"%s",week1 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week2_inb")){	// Tue
		ret = websWrite(wp,"%s",week2 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week3_inb")){	// Wed
		ret = websWrite(wp,"%s",week3 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week4_inb")){	// Thu
		ret = websWrite(wp,"%s",week4 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week5_inb")){	// Fri
		ret = websWrite(wp,"%s",week5 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"week6_inb")){	// Sat
		ret = websWrite(wp,"%s",week6 == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"time_all_en_inb")){
		ret = websWrite(wp,"%s",time_all == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"time_all_dis_inb")){
		ret = websWrite(wp,"%s",time_all == 0 ? "checked" : "");
	}
	else if(!strcmp(type,"time_all")){
		ret = websWrite(wp,"%s",time_all == 1 ? "checked" : "");
	}
	else if(!strcmp(type,"start_hour_24")){	// 00 -> 23
		for(i=0 ; i<24 ; i++){
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",i, i == start_hour+start_time*12 ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"start_min_15")){	// 0 15 30 45
		for(i=0 ; i<4 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*15, i*15 == start_min ? "selected" : "", i*15);
		}
	}
	else if(!strcmp(type,"end_hour_24")){	// 00 ->23
		for(i=0 ; i<24 ; i++){
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",i, i == end_hour+end_time*12 ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"end_min_15")){	// 0 15 30 45
		for(i=0 ; i<4 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*15, i*15 == end_min ? "selected" : "", i*15);
		}
	}
	else if(!strcmp(type,"start_hour_12_inb")){	// 1 -> 12
		for(i=1 ; i<=12 ; i++){
			int j;
			if(i == 12)	j = 0;
			else		j = i;
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",j, j == start_hour ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"start_min_5_inb")){	// 0 5 10 15 20 25 30 35 40 45 50 55
		for(i=0 ; i<12 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*5, i*5 == start_min ? "selected" : "", i*5);
		}
	}
	else if(!strcmp(type,"start_time_am_inb")){
		ret = websWrite(wp,"%s",start_time == 1 ? "" : "selected");
	}
	else if(!strcmp(type,"start_time_pm_inb")){
		ret = websWrite(wp,"%s",start_time == 1 ? "selected" : "");
	}
	else if(!strcmp(type,"end_hour_12_inb")){	// 1 -> 12
		for(i=1 ; i<=12 ; i++){
			int j;
			if(i == 12)     j = 0;
			else		j = i;
			ret = websWrite(wp,"<option value=%d %s>%d</option>\n",j, j == end_hour ? "selected" : "", i);
		}
	}
	else if(!strcmp(type,"end_min_5_inb")){	// 0 5 10 15 20 25 30 35 40 45 50 55
		for(i=0 ; i<12 ; i++){
			ret = websWrite(wp,"<option value=%02d %s>%02d</option>\n",i*5, i*5 == end_min ? "selected" : "", i*5);
		}
	}
	else if(!strcmp(type,"end_time_am_inb")){
		ret = websWrite(wp,"%s",end_time == 1 ? "" : "selected");
	}
	else if(!strcmp(type,"end_time_pm_inb")){
		ret = websWrite(wp,"%s",end_time == 1 ? "selected" : "");
	}

	return ret;
}

int
ej_filter_inb_summary_show(int eid, webs_t wp, int argc, char_t **argv)
{
	int i,ret;
#if LANGUAGE == JAPANESE
	char w[7][10] = { "?","?","?","?","?","?","?"};
	char week_d[7][10] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	char week_s[7][10] = { "??","??","??","??","??","??","??"};
	char am[] ="??";
	char pm[] ="??";
	char _24h[] ="24 ??";
	char everyday[] ="??";
#elif LANGUAGE == KOREAN && OEM == REENET	// <remove the line>
	char w[7][10] = { "일","월","화","수","목","금","토"};	// <remove the line>
	char week_d[7][10] = { "일","월","화","수","목","금","토"};	// <remove the line>
	char am[] ="오전";	// <remove the line>
	char pm[] ="오후";	// <remove the line>
	char _24h[] ="매일";	// <remove the line>
	char everyday[] ="24시간";	// <remove the line>
#else
	char w[7][2] = { "S","M","T","W","T","F","S"};
	char week_d[7][10] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	char week_s[7][10] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	char am[] ="AM";
	char pm[] ="PM";
	char _24h[] ="24 Hours.";
	char everyday[] ="Everyday";
#endif

	for(i=0 ; i<10 ; i++){
     		char name[50]="---";
     		char status[5]="---";
		char filter[] = "filter_inb_ruleXXX";
		char *data = "";
		char status_buf[50]="---";
		char day_buf[50]="---";
		char time_buf[50]="---";
 		snprintf(filter, sizeof(filter), "filter_inb_rule%d", i+1);
		data = nvram_safe_get(filter);
		if(data && strcmp(data,"")){
			find_match_pattern(name, sizeof(name), data, "$NAME:","&nbsp;");	// get name value
			find_match_pattern(status, sizeof(status), data, "$STAT:","---");	// get name value
		}

		filter_inb_tod_init(i+1);

ret = websWrite(wp," \
	<tr bgcolor=cccccc align=middle>\n\
        <td width=50><font face=Arial size=2>%d.</font></td>\n\
        <td width=200><font face=Arial size=2>%s</font></td>\n",i+1,name);
#if OEM == LINKSYS
ret = websWrite(wp,"\n\
	 <td height=30 width=150 bordercolor=#000000 bgcolor=#CCCCCC> \n\
              <table border=1 cellspacing=1 bordercolor=#000000 style=\"border-collapse: collapse\" width=124 bgcolor=#FFFFFF>\n\
                <tr>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                  <td width=17 bgcolor=\"%s\" style=\"border-style: solid; border-width: 1\">%s</td>\n\
                </tr>\n\
              </table></td>",tod_data_null == 0 && (day_all == 1 || week0 == 1) ? "#C0C0C0" : "#FFFFFF",w[0],
				tod_data_null == 0 && (day_all == 1 || week1 == 1) ? "#C0C0C0" : "#FFFFFF",w[1],
				tod_data_null == 0 && (day_all == 1 || week2 == 1) ? "#C0C0C0" : "#FFFFFF",w[2],
				tod_data_null == 0 && (day_all == 1 || week3 == 1) ? "#C0C0C0" : "#FFFFFF",w[3],
				tod_data_null == 0 && (day_all == 1 || week4 == 1) ? "#C0C0C0" : "#FFFFFF",w[4],
				tod_data_null == 0 && (day_all == 1 || week5 == 1) ? "#C0C0C0" : "#FFFFFF",w[5],
				tod_data_null == 0 && (day_all == 1 || week6 == 1) ? "#C0C0C0" : "#FFFFFF",w[6]);
	if(tod_data_null == 0){
		if(time_all == 1)
			strcpy(time_buf, _24h);
		else{
			snprintf(time_buf,sizeof(time_buf),"%02d:%02d %s - %02d:%02d %s",
				start_hour == 0 ? 12 : start_hour,
				start_min,
				start_time == 0 ? am : pm,
				end_hour == 0 ? 12 : end_hour,
				end_min,
				end_time == 0 ? am : pm);
		}
	}
#elif OEM == PCI || OEM == ELSA
	//if(!day_all){
	if(tod_data_null == 0){
		if(day_all == 1)
			strcpy(day_buf, everyday);
		else
			snprintf(day_buf,sizeof(day_buf),"%s - %s",week_s[start_week], week_s[end_week]);
#if LANGUAGE == ENGLISH	// <remove the line>
		snprintf(status_buf,sizeof(status_buf),"%s",atoi(status)==1 ? "Deny" : "Allow");
#else	// <remove the line>
		snprintf(status_buf,sizeof(status_buf),"%s",atoi(status)==1 ? "??" : "??");
#endif	// <remove the line>
	}
ret = websWrite(wp,"<td width=80><font face=Arial size=2>%s</font></td>\n",status_buf);
ret = websWrite(wp,"<td width=124><font face=Arial size=2>%s</font></td>\n",day_buf);
	if(tod_data_null == 0){
		if(time_all == 1)
			strcpy(time_buf, _24h);
		else
			snprintf(time_buf,sizeof(time_buf),"%02d:%02d - %02d:%02d",start_hour+start_time*12, start_min,
									   end_hour+end_time*12, end_min);
	}
#endif
ret = websWrite(wp," \
        <td width=150><font face=Arial size=2> %s </font> </td>\n\
        <td width=70><input type=checkbox name=sum_inb%d value=1 ></td>\n\
      </tr>\n",time_buf, i+1);
	}
	return ret;

}

int
ej_filter_inb_init(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *f_id = websGetVar(wp, "f_id_inb", NULL);

	if(f_id)	// for first time enter this page and don't press apply.
		nvram_set("filter_inb_id", f_id);
	else
		nvram_set("filter_inb_id", "1");

	nvram_set("filter_inb_summary", "0");
	return ret;
}

int
filtersummary_inb_onload(webs_t wp, char *arg)
{
	int ret = 0;

	if(!strcmp(nvram_safe_get("filter_inb_summary"),"1")){
		ret += websWrite(wp, arg);
	}

	return ret;
}
