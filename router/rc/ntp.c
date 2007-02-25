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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>
#include <wait.h>
#include <fcntl.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>

#include <ntp.h>
#include <cy_conf.h>
#include <utils.h>

#define NTP_M_TIMER "3600" 
#define NTP_N_TIMER "60" 
#define SMTP_DEV	"/dev/smtp_server_dev"
#define JAN_1970	0x83aa7e90 
int time_sleep;

typedef struct daylight {
	int year;
	int month;
	int day;
} DAYLIGHT;

void get_ntp_params(char **s,char *tz,int *en,DAYLIGHT *start,DAYLIGHT *end);
void get_day_from_str(DAYLIGHT *lightday,char *str);
int is_daylight(struct tm tm,DAYLIGHT *start,DAYLIGHT *end);
int get_tz_index(char *tz);

/* for NTP */
int do_ntp(void)
{
	DAYLIGHT start,end;
	unsigned long g_timestamp = 0;
	char	*ntp_servs[5];
	char	buf[1024],tz_buf[20];
	int		i,ret,mt,cnt;
	int		offset;
	struct	timeval tv;
	struct	timezone tz;
	struct	tm tm;
	int 	daylight_time;

	char *ntpclient_argv[] = {"ntpclient","-h",buf,"-l","-s",NULL};

	int fd,tzindex=0;  
	char timezone[10],timestamp[10];
	char *pos;

//get ntp parameters value from nvram
	get_ntp_params(ntp_servs,tz_buf,&daylight_time,&start,&end);
	offset = atoi(tz_buf)*3600;
	pos = strchr(tz_buf,'.');
	if( pos && *(pos+1)!='0' ){
		i = atoi(pos+1);
		if( i<10 ) i *= 10;
		while(i>100) i /= 10;
		offset += i*3600/100;
	}
	cnt = time(NULL)%5;
	if(cnt < 0)	cnt += 5;
	if (nvram_match("ntp_mode", "manual") && nvram_invmatch("ntp_server", ""))
		strcpy(buf,nvram_safe_get("ntp_server"));
	else{
		strcpy(buf,ntp_servs[cnt]);
		for(i=0;i<5;i++){
			if( i== cnt) continue;
			sprintf(buf,"%s %s",buf,ntp_servs[i]);
		}
		printf("NTP-Server List: %s\n",buf);	   
	}		   
//get UTC time from NtpServers
	ret = _eval(ntpclient_argv, NULL, 20, NULL);

//update localtime
   	gettimeofday(&tv,&tz);
	g_timestamp = tv.tv_sec + JAN_1970;
	memcpy(&tm, localtime(&tv.tv_sec), sizeof(struct tm));
	mt = tm.tm_year+1900<2000?0:1;

	if( mt && !ret ) {
		tv.tv_sec += offset;
		memcpy(&tm, localtime(&tv.tv_sec), sizeof(struct tm));
	
#ifdef CPED_TR064_SUPPORT
		if( daylight_time && is_daylight(tm,&start,&end) ) {
#else
		if( daylight_time ) {
#endif
			tv.tv_sec += 3600;
			memcpy(&tm, localtime(&tv.tv_sec), sizeof(struct tm));
		}
		settimeofday(&tv,&tz);
//write TZInfo into SMTP-DEV
		tzindex = get_tz_index(tz_buf);
		if( tzindex==-1 ){
			printf("TimeZone not found in TzEntry,TimeZone[%s]\n",tz_buf);   
			return 0;
		}
		fd = open(SMTP_DEV, O_RDWR);
		printf("*****TzIndex=%d******\n", tzindex);
		if(fd < 0){
			printf("Can not open the smtp device\n");		
		}
		else {
			memset(timezone, 0, sizeof(timezone));		
			memset(timestamp, 0,  sizeof(timestamp));
			sprintf(timestamp, "6%08x", g_timestamp);
			timestamp[strlen(timestamp)] = 0;
			write(fd, timestamp, sizeof(timestamp));
			printf("timestamp:%s\n", timestamp);
			sprintf(timezone, "7%08x", tzindex);
			timezone[strlen(timezone)] = 0;
			write(fd, timezone, sizeof(timezone));
			close(fd);
		}	
#ifndef NTPD_TEST
// firewall.c : synchronize the filter rules by TOD scheduling
		dprintf("NTP:filtersync\n");
		filtersync_main();
#endif
#ifdef FIREWALL_LEVEL_SUPPORT
		dprintf("ntp: filterinbsync\n");
		filterinbsync_main();
#endif
		time_sleep = atoi(nvram_get("timer_interval"));
	}
	else {
		time_sleep = atoi(NTP_N_TIMER);
	//	nvram_set("timer_interval",NTP_N_TIMER);
	}
	return 0;
}

#ifdef NTPD_TEST
int main(int argc, char **argv)
#else
int ntp_main(int argc, char **argv)
#endif
{
	int status,interval;
	pid_t pid;

	time_sleep = atoi(NTP_N_TIMER);

	while(1) {
		pid = fork();
		if(pid<0) {
			perror("fork failed");
			exit(1);
		}
		if( !pid ) {
			if(check_action() == ACT_IDLE){	// Don't execute during upgrading
				eval("killall","-9","ntpclient");	
				do_ntp();
			}
			else
				fprintf(stderr, "ntp: nothing to do...\n");
			exit(0);
		}
		else 
			waitpid(pid, &status, 0);

		interval=time_sleep;
		sleep(interval);	
	}
}

int stop_ntp(void)
{
	int ret = eval("killall","-9", "ntpd");
	dprintf("done\n");
	return ret;
}

int start_ntp(void)
{
	int ret;
	pid_t pid;
	char *ntp_argv[] = { "ntpd", NULL };

   ret = _eval(ntp_argv, NULL, 0, &pid);
   dprintf("done\n");
   return ret;
}

//the following functions added by juven.cui,2004-09-01

/* get ntp parameters from nvram */
void get_ntp_params(char **servs,char *tzbuf,int *daylight_used,DAYLIGHT *start,DAYLIGHT *end)
{
	*servs++ = strdup(nvram_get("NTPServer1"));
	*servs++ = strdup(nvram_get("NTPServer2"));
	*servs++ = strdup(nvram_get("NTPServer3"));
	*servs++ = strdup(nvram_get("NTPServer4"));
	*servs   = strdup(nvram_get("NTPServer5"));
	strcpy(tzbuf,nvram_get("time_zone"));
	*daylight_used = atoi(nvram_get("daylight_time"));
#ifdef CPED_TR064_SUPPORT
	get_day_from_str(start,nvram_get("DaylightSavingStart"));
	get_day_from_str(end,nvram_get("DaylightSavingEnd"));
#endif
}

/* str format likes "2001-07-01T00:00:00" */
void get_day_from_str(DAYLIGHT *lightday,char *str)
{
	char *p0=str,*p1=str;
	char tmp[8];
	if(!strcmp(str,""))
		return;
	p1 = strchr(p0,'-');
	strncpy(tmp,p0,p1-p0);
	tmp[p1-p0]=0;
	lightday->year=atoi(tmp);
	p0=p1+1;
	p1 = strchr(p0,'-');
	strncpy(tmp,p0,p1-p0);
	tmp[p1-p0]=0;
	lightday->month=atoi(tmp);
	p0=p1+1;
	p1 = strchr(p0,'T');
	strncpy(tmp,p0,p1-p0);
	tmp[p1-p0]=0;
	lightday->day=atoi(tmp);
}

/* check time now betwen start & end or not */
int is_daylight(struct tm now,DAYLIGHT *start,DAYLIGHT *end)
{
	int curmon=now.tm_mon+1;
	int curday=now.tm_mday;
	int startmon=start->month;
	int endmon=end->month;
	
	if( startmon>endmon || (startmon==endmon&&start->day>end->day) ){
		curmon += 12;
		endmon += 12;
	}
	if( curmon<startmon||curmon>endmon ) return 0;
	if( curmon==startmon && curday<start->day ) return 0;
	if( curmon==endmon && curday>end->day ) return 0;
	return 1;
}

int get_tz_index(char *tzbuf)
{
	int i,near_index=-1,tz;
	char tmp[8],*p0,*p1;

	strtok(tzbuf," ");
	tz=atoi(tzbuf);
	p0 = strchr(tzbuf,'.');
	if( p0 && strcmp(p0,".5")>=0 )
		tz += *tzbuf=='-'?-1:1;
	for(i=0;i<tzEntrySize;i++){
		p0 = tzEntry[i].name;
		p1 = strchr(p0,' ');
		if(!p1) p1=p0+strlen(p0);
		strncpy(tmp,p0,p1-p0);
		tmp[p1-p0] = 0;
		if(!strcmp(tzbuf,tmp)) break;
		if( near_index==-1 && tz==atoi(tmp) ) near_index=i;
	}
	if( i==tzEntrySize ) return near_index;
	return i;
}
