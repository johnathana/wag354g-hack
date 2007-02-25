
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
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>

#include "broadcom.h"
#include "pvc.h"
//cjg
typedef struct ping_config{
	char *ipaddr;
	char *size;
	char *number;
	char *interval;
	char *timeout
}Ping_config_ptr;

Ping_config_ptr ping_config_ptr;
#define p_ipaddr ping_config_ptr.ipaddr
#define p_size   ping_config_ptr.size
#define p_number ping_config_ptr.number
#define p_interval ping_config_ptr.interval
#define p_timeout ping_config_ptr.timeout

char ping_config_buf[256];//cjg
//cjg
static int ping_inited;
static int g_fd = 0;
static unsigned char g_ip_addr[20], g_ip_size[20], g_ip_num[10], g_ip_interval[30], g_ip_timeout[30];
volatile static unsigned int ping_start_flag = 0;
int
ej_dump_ping_log(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret=0, count=0;
	FILE *fp;
	char line[254];
	char *tok;

	if (ping_start_flag && (fp = fopen(PING_TMP, "r")) != NULL) {		// show result
		while( fgets(line, sizeof(line), fp) != NULL ) {
			line[strlen(line)-1] = '\0';
			if(!strcmp(line,""))	continue;
#ifdef MULTILANG_SUPPORT
			ret += websWrite(wp, "<script>Capture(admping.pktsend)</script>:");
			tok = strtok(line, "Pkt Sent:");
			ret += websWrite(wp, "%s", tok);
			ret += websWrite(wp, " <script>Capture(admping.pktrecv)</script>:");
			tok = strtok(NULL, "Pkt Recv:");
			ret += websWrite(wp, "%s", tok);
			ret += websWrite(wp, " <script>Capture(admping.avgrtt)</script>:");
			while ((tok = strtok(NULL, "Avg Rtt:")) != NULL)
				ret += websWrite(wp, "%s", tok);
#else
			ret += websWrite(wp,"%s\n", line);
#endif
			count ++;
		}
		fclose(fp);
		ping_start_flag = 0;
	}
	else
	{
#ifdef MULTILANG_SUPPORT
			ret += websWrite(wp, "<script>Capture(admping.pktsend)</script>:0 
					      <script>Capture(admping.pktrecv)</script>:0 
					      <script>Capture(admping.avgrtt)</script>:0\n");
#else
			ret += websWrite(wp, "Pkt Sent:0 Pkt Recv:0 Avg Rtt:0\n");
#endif
	}
	 
	return ret;
}



int diag_ping_start(webs_t wp)
{
	char cmd[120];
	char *ping_ipaddr, *ping_size,  *ping_number, *ping_interval, *ping_timeout;
        char temp[30], *val = NULL;
        int i;

        struct variable ping_config_variables[] = {
                 {longname: "Ping IP Address", argv:NULL},
                 {longname: "Ping Size", argv:ARGV("60")},
                {longname: "Ping Number", argv:ARGV("1", "100")},
                {longname: "Ping Interval", argv:ARGV("1000")},
                {longname: "Ping Timeout", argv:ARGV("5000")}
        };
       // unlink("/tmp/ping.log");
        ping_ipaddr = malloc(20);
        memset(ping_ipaddr, 0, sizeof(ping_ipaddr));
        memset(temp, 0, sizeof(temp));
	for(i=0 ; i<4 ; i++)
	{
	       	snprintf(temp, sizeof(temp), "%s_%d", "ping_ipaddr", i);
		val = websGetVar(wp, temp , NULL);
		if(val)
		{
			strcat(ping_ipaddr,val);
			if(i<3)				
				strcat(ping_ipaddr,".");
		}								                else
		{
			free(ping_ipaddr);
			return 0;

		}

	}

	
	ping_size = websGetVar(wp, "ping_size", NULL);
	ping_number = websGetVar(wp, "ping_number", NULL);			        ping_interval = websGetVar(wp, "ping_interval", NULL);	
	ping_timeout = websGetVar(wp, "ping_timeout", NULL);
	if(!valid_range(wp, ping_number, &ping_config_variables[2]))
	      	return 0;	
	if(!valid_ipaddr(wp, ping_ipaddr, &ping_config_variables[0]))
		return 0;						
	snprintf(cmd, sizeof(cmd), "/usr/sbin/diagnostic -f %s -s %s -c %s -i %s -w %s %s",
		PING_TMP, 
		ping_size, 
		ping_number, 
		ping_interval, 
		ping_timeout, 
		ping_ipaddr);
	printf("cmd=%s\n", cmd);
	memset(g_ip_addr, 0, sizeof(g_ip_addr));
	memset(g_ip_size, 0, sizeof(g_ip_size));
	memset(g_ip_num, 0, sizeof(g_ip_num));
	memset(g_ip_interval, 0, sizeof(g_ip_interval));
	memset(g_ip_timeout, 0, sizeof(g_ip_timeout));
	strncpy(g_ip_addr, ping_ipaddr, strlen(ping_ipaddr));
	strncpy(g_ip_size, ping_size, strlen(ping_size));
	strncpy(g_ip_num, ping_number, strlen(ping_number));
	strncpy(g_ip_interval, ping_interval, strlen(ping_interval));
	strncpy(g_ip_timeout, ping_timeout, strlen(ping_timeout));
	system(cmd);
	ping_start_flag = 1;
/*	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if(fcntl(g_fd, F_SETLKW, &lock) < 0)
	{
		printf("can not lock the file\n");
		return 0;
	}*/
//	close(fd);


	return 0;
}
	

int ej_show_ping_status(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *type;
	struct flock lock;
	if (ejArgs(argc, argv, "%s", &type) < 1) {
		websError(wp, 400, "Insufficient args\n");
	 	return 0;
	}
	
	if(!strcmp(type,"refresh_time")){
	/*	if(g_fd != NULL)
		{
			printf("refresh_time\n");
			lock.l_type = F_RDLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = 0;
			lock.l_len = 0;
			if(fcntl(g_fd, F_SETLKW, &lock) < 0)
			{
				printf("ej-unlock error\n");
			}
			close(g_fd);
		}
		else
		{
			printf("ej-can not open log file\n");
		//	goto reopen;
		}*/
		ret += websWrite(wp,"%d", 60*1000);
	}
	return ret;

}
/*cjg*/
/*ping_config:ipaddr:size:number:interval:timeout*/
void ping_config_init()
{
	char *next;
/*	struct flock lock;
	int fd;
	fd = open("/tmp/ping_lock", O_CREAT);
	if(NULL == fd)
	{
		printf("can not create the ping_lock\n");
		return 0;
	}
	close(fd);
	//unlink(PING_TMP);
	g_fd = open("/tmp/ping_lock", O_RDONLY);
	if(NULL == g_fd)
	{
		printf("can not open the ping_lock\n");
		return 0;
	}
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if(fcntl(g_fd, F_SETLKW, &lock) < 0)
	{
		printf("can not lock the file\n");
		return 0; 
	}
printf("file is locked\n");*/
	if(!strcmp(nvram_safe_get("ping_config"), "ping_config"))
		nvram_set("ping_config", "0.0.0.0:60:1:1000:5000");
	if(ping_inited == 0){
		memset(ping_config_buf, 0, sizeof(ping_config_buf));
		memset(g_ip_addr, 0, sizeof(g_ip_addr));
		memset(g_ip_size, 0, sizeof(g_ip_size));
		memset(g_ip_num, 0, sizeof(g_ip_num));
		memset(g_ip_interval, 0, sizeof(g_ip_interval));
		memset(g_ip_timeout, 0, sizeof(g_ip_timeout));

		foreach(ping_config_buf, nvram_safe_get("ping_config"), next)
		{
			p_size = ping_config_buf;
			p_ipaddr = strsep(&p_size, ":");
			if(!p_size || !p_ipaddr)
				break;

			strncpy(g_ip_addr, p_ipaddr, strlen(p_ipaddr));
			p_number = p_size;
			p_size = strsep(&p_number, ":");
			if(!p_number || !p_size)
				break;
			strncpy(g_ip_size, p_size, strlen(p_size));
			p_interval = p_number;
			p_number = strsep(&p_interval, ":");
			if(!p_interval || !p_number)
				break;
			strncpy(g_ip_num, p_number, strlen(p_number));
			p_timeout = p_interval;
			p_interval = strsep(&p_timeout, ":");
			if(!p_timeout || !p_interval)
				break;
			strncpy(g_ip_interval, p_interval, strlen(p_interval));
			strncpy(g_ip_timeout, p_timeout, strlen(p_timeout));
			break;
			
		}
		ping_inited = 1;
	}
}

//cjg
int ej_ping_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	ping_config_init();
	if(!strcmp(arg, "ping_ipaddr"))
		return websWrite(wp, "%d", get_single_ip(g_ip_addr, count));
	if(!strcmp(arg, "ping_size"))
		return websWrite(wp, "%s", g_ip_size);
	if(!strcmp(arg, "ping_number"))
		return websWrite(wp, "%s", g_ip_num);
	if(!strcmp(arg, "ping_interval"))
		return websWrite(wp, "%s", g_ip_interval);
	if(!strcmp(arg, "ping_timeout"))
		return websWrite(wp, "%s", g_ip_timeout);

	return -1;

}


//cjg
void validate_ping_config(webs_t wp, char *value, struct variable *v)	
{	
	char *ping_ipaddr, *ping_size,  *ping_number, *ping_interval, *ping_timeout;
	char temp[30], *val = NULL;
	int i;
	char buf[1000];

	struct variable ping_config_variables[] = {
		{longname: "Ping IP Address", argv:NULL},
		{longname: "Ping Size", argv:ARGV("60")},
		{longname: "Ping Number", argv:ARGV("1", "100")},
		{longname: "Ping Interval", argv:ARGV("1000")},
		{longname: "Ping Timeout", argv:ARGV("5000")}
	};
	unlink("/tmp/ping.log");
	ping_ipaddr = malloc(20);
	memset(ping_ipaddr, 0, sizeof(ping_ipaddr));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
        	snprintf(temp, sizeof(temp), "%s_%d", "ping_ipaddr", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                	strcat(ping_ipaddr,val);
                        if(i<3) 
				strcat(ping_ipaddr,".");
                }
                else
		{	
			free(ping_ipaddr);
                        return ;
		}
        }
	ping_size = websGetVar(wp, "ping_size", NULL);
	ping_number = websGetVar(wp, "ping_number", NULL);
	ping_interval = websGetVar(wp, "ping_interval", NULL);
	ping_timeout = websGetVar(wp, "ping_timeout", NULL);
	if(!valid_range(wp, ping_number, &ping_config_variables[2]))
		return;
	if(!valid_ipaddr(wp, ping_ipaddr, &ping_config_variables[0]))
		return;
				
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s:%s:%s:%s:%s", 
			ping_ipaddr, 
			ping_size, 
			ping_number, 
			ping_interval,
			ping_timeout);	
	if(ping_ipaddr)
		free(ping_ipaddr);
	nvram_set(v->name, buf);
	ping_inited = 0;
	return;
}


