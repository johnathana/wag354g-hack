
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>


#include "broadcom.h"
#define NO_PRINTF
#ifdef NO_PRINTF
#define printf(fmt, args...)
#endif
//cjg
typedef struct email_config{
	char *enable;
	char *thresholds;
	char *smtp_server;
	char *email_for_log;
	char *email_for_return;
}Email_config_ptr;

Email_config_ptr email_config_ptr;
#define email_enable email_config_ptr.enable
#define email_thresholds  email_config_ptr.thresholds
#define smtp_server email_config_ptr.smtp_server
#define email_for_log email_config_ptr.email_for_log
#define email_for_return email_config_ptr.email_for_return

char email_config_buf[256];//cjg
/*ping_config:ipaddr:size:number:interval:timeout*/
void email_config_init()
{
	char *next;
	memset(email_config_buf, 0, sizeof(email_config_buf));
	if(!strcmp(nvram_safe_get("email_config"), "email_config"))
	{
		nvram_set("email_config", "0:20:NULL:NULL:NULL");
	}
	foreach(email_config_buf, nvram_safe_get("email_config"), next)
	{
		email_thresholds = email_config_buf;
		email_enable = strsep(&email_thresholds, ":");
		if(!email_enable || !email_thresholds)
			break;
		smtp_server = email_thresholds;
		email_thresholds = strsep(&smtp_server, ":");
		if(!smtp_server || !email_thresholds)
			break;
		email_for_log = smtp_server;
		smtp_server = strsep(&email_for_log, ":");
		if(!email_for_log || !smtp_server)
			break;
		email_for_return = email_for_log;
		email_for_log = strsep(&email_for_return, ":");
		if(!email_for_return || !email_for_log)
			break;
		break;
		
	}
/*	printf("email_enable = %s \n email_thresholds =%s\n"
		"smtp_server = %s \n email_for_log = %s\n"
		"email_for_return = %s\n",
		email_enable,
		email_thresholds,
		smtp_server,
		email_for_log,
		email_for_return);
*/
}

//cjg
int ej_email_enable_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	printf("arg:%s - count:%d\n", arg, count);
	email_config_init();
	printf("count = %d - email_enable = %s\n", count, email_enable);
	if(!strcmp(arg, "email_enable"))
	{
		if((count == 1 && !strncmp(email_enable, "1", 1)) ||
		(count == 0 && !strncmp(email_enable, "0", 1)))
		return websWrite(wp, "checked");
	}
	return 0;
	
}

int ej_email_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;

	if(ejArgs(argc, argv, "%s", &arg) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	email_config_init();
	if(!strcmp(arg, "email_thresholds"))
		return websWrite(wp, "%s", email_thresholds);
	if(!strcmp(arg, "smtp_server"))
	{
		if(!strncmp(smtp_server, "NULL", 4))
			return websWrite(wp, "");
		else
			return websWrite(wp, "%s", smtp_server);
	}
	if(!strcmp(arg, "email_for_log"))
	{
		if(!strncmp(email_for_log, "NULL", 4))
			return websWrite(wp, "");
		else
			return websWrite(wp, "%s", email_for_log);
	}
	if(!strcmp(arg, "email_for_return"))
	{
		if(!strncmp(email_for_return, "NULL", 4))
			return websWrite(wp, "");
		else
			return websWrite(wp, "%s", email_for_return);
	}

	return -1;

}

int ej_smtp_server_resolv(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *smtp_server_ptr;
	struct in_addr sock;
	smtp_server_ptr = websGetVar(wp, "smtp_server", NULL);
	printf("ej_smtp_server_resolv:%s\n", smtp_server_ptr);
	if(smtp_server_ptr == NULL)
	{
		ret = websWrite(wp, "\tresolv_smtp_server = \"failure\";\n");
	}
	else
	{
		if(inet_aton(smtp_server_ptr, &sock) == 0)
		{
			if(gethostbyname(smtp_server_ptr) != NULL)
			{
				ret = websWrite(wp, "\tresolv_smtp_server = \"success\";\n");
			}
			else
			{
				ret = websWrite(wp, "\tresolv_smtp_server = \"failure\";\n");
			}
		}
		else
		{
			ret = websWrite(wp, "\tresolv_smtp_server = \"success\";\n");
		}
	}
	return ret;
}

#define SMTP_DEV "/dev/smtp_server_dev"
int update_to_kernel_disable()/* disable email */
{
	int smtp_dev_fd;
	unsigned char e_disable[3];
	smtp_dev_fd = open(SMTP_DEV, O_RDWR);
	if(smtp_dev_fd < 0)		
	{
		printf("can not open  SMTP DEVICE\n");													
	}								                else
	{
		e_disable[0] = '3';
		e_disable[1] = '0'; /* 0 for disable */
		e_disable[2] = '\0';
		write(smtp_dev_fd, e_disable, 3);
		close(smtp_dev_fd);
	}
	return 0;
}
char *g_smtp_server = NULL, *g_for_log = NULL, *g_for_return = NULL;
extern int error_value;
int update_to_kernel_enable() /* enable email alert */
{
	 struct hostent *remote_host = NULL;
	 struct sockaddr_in sock;
	 int smtp_dev_fd;
         char tmp[9];
	 unsigned int ipaddr;
	 
	{
		smtp_dev_fd = open(SMTP_DEV, O_RDWR);
		if(smtp_dev_fd < 0)		
		{
			printf("can not open  SMTP DEVICE\n");													
		}								                else
		{
			char email_log[120];
			char email_ret[120];
			char e_enable[3];
			int size;					
			memset(email_log, 0, sizeof(email_log));
			memset(email_ret, 0, sizeof(email_ret));
			memset(tmp, 0, sizeof(tmp));
			printf("smtp_server:%s\n", g_smtp_server);
			memset(&sock, 0, sizeof(struct sockaddr_in));
			if(inet_aton(g_smtp_server, (struct in_addr *)&sock.sin_addr) == 0)
			{
				printf("inet_aton error\n");
				if((remote_host = gethostbyname(g_smtp_server)) == NULL)
				{
					printf("gethostbyname error\n");
					close(smtp_dev_fd);
					error_value = 1; /*can not resolv*/
					return -1;
				}
				else
			
				{
						
					memcpy(&sock.sin_addr, remote_host->h_addr_list[0],
					(size_t)remote_host->h_length);
				}
			}
			ipaddr = sock.sin_addr.s_addr;
			printf("write_to_kernel:%08x\n", ipaddr);
		 	memset(tmp, 0, sizeof(tmp));
	                sprintf(tmp, "%08x", ipaddr);
			tmp[strlen(tmp)] = '\0';
			write(smtp_dev_fd, tmp, sizeof(tmp));
			size = sprintf(email_log, "1%s", g_for_log);
			email_log[strlen(email_log)] = '\0';
			write(smtp_dev_fd, email_log, size + 1);
			e_enable[0] = '3';
			e_enable[1] = '1'; /*1 for enable*/
			e_enable[2] = '\0';
			write(smtp_dev_fd, e_enable, 3);
			size = sprintf(email_ret, "2%s", g_for_return);					email_ret[strlen(email_ret)] = '\0';
			write(smtp_dev_fd, email_ret, size + 1);

		        close(smtp_dev_fd);	
		}			
	}
	return 0;	
}

//cjg
void validate_email_config(webs_t wp, char *value, struct variable *v)	
{	
	char *e_enable, *e_thresholds,  *e_smtp_server, *e_for_log, *e_for_return;
	char buf[1000];

	e_enable = websGetVar(wp, "email_enable", NULL);
	e_thresholds = websGetVar(wp, "email_thresholds", NULL);
	e_smtp_server = websGetVar(wp, "smtp_server", NULL);
	e_for_log = websGetVar(wp, "email_for_log", NULL);
	e_for_return = websGetVar(wp, "email_for_return", NULL);			
	memset(buf, 0, sizeof(buf));
	email_config_init();
	/*
	printf("validate-email-config %s:%s:%s:%s:%s\n",
		       e_enable,
	       	       e_thresholds,
		       e_smtp_server,
		       e_for_log,
		       e_for_return);
*/
	
	if(!strncmp(e_enable, "0", 1))
	{
		sprintf(buf, "%s:%s:%s:%s:%s", 
			e_enable, 
			email_thresholds, 
			smtp_server, 
			email_for_log,
			email_for_return);	
		printf("disable email alert\n");
		update_to_kernel_disable();
		nvram_set(v->name, buf);
		nvram_set("email_enable_flag" ,"0");
	}
	else if(!strncmp(e_enable, "1", 1) && e_smtp_server != NULL &&
			e_for_log != NULL && e_for_return != NULL)
	{
			sprintf(buf, "%s:%s:%s:%s:%s", 
			e_enable, 
			e_thresholds, 
			(e_smtp_server == NULL)? "NULL":e_smtp_server, 
			(e_for_log == NULL) ? "NULL":e_for_log,
			(e_for_return == NULL)? "NULL":e_for_return);	
		printf("enable_email_alert\n");
//		g_smtp_server = e_smtp_server;
//		g_for_log = e_for_log;
//		g_for_return = e_for_return;
//		if(update_to_kernel_enable() == 0)
			nvram_set(v->name, buf);
			nvram_set("email_enable_flag", "1");
	}
	else
		error_value = 1; /* maybe some items is invalid */
	return;
}


