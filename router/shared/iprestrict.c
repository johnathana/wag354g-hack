#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <httpd.h>
#include <errno.h>
#endif /* WEBS */

#include <proto/ethernet.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>

#include "broadcom.h"
#include <cyutils.h>
#include <support.h>
#include <cy_conf.h>
#include "pvc.h"

//extern char vcc_config_buf[256];
//extern int vcc_inited;

//int gozila_action = 0;
//int error_value = 0;


// For IP Range Restricted 
void validate_ip_range_setting(webs_t wp, char *value, struct variable *v)
{
	char *ip_range_setting;
	int flag;
	char buf_start[256], buf_end[256], buf_range[256];
	char *range_ip_01, *range_ip_02, *range_ip_03, *range_ip_04;
	char *range_ip_11, *range_ip_12, *range_ip_13, *range_ip_14;

	
	flag  =  atoi(websGetVar(wp, "ip_restrict", "0"));
	printf("flag = %d \n", flag);

	if(flag == 0){
		ip_range_setting = "0:0:0.0.0.0-0.0.0.0";
		nvram_set("range_start_ip", "0.0.0.0");
		nvram_set("range_end_ip", "0.0.0.0");
		nvram_set("ip_range_setting", ip_range_setting);
		nvram_set("range_all","0");
		printf("flag = 0  ip_range_setting = %s \n", ip_range_setting);	
	}else if(flag == 1){
		memset(buf_start, 0, sizeof(buf_start));
		memset(buf_range, 0, sizeof(buf_range));

		range_ip_01 = websGetVar(wp, "range_ip_01", "0");
		range_ip_02 = websGetVar(wp, "range_ip_02", "0");
		range_ip_03 = websGetVar(wp, "range_ip_03", "0");
		range_ip_04 = websGetVar(wp, "range_ip_04", "0");

		sprintf(buf_start, "%s.%s.%s.%s", range_ip_01, range_ip_02, range_ip_03, range_ip_04);
		sprintf(buf_range, "0:1:%s.%s.%s.%s-%s.%s.%s.%s", range_ip_01, range_ip_02, range_ip_03, range_ip_04, range_ip_01, range_ip_02, range_ip_03, range_ip_04);

		nvram_set("range_start_ip", buf_start);
		nvram_set("range_end_ip", buf_start);	//The end  same as start
		nvram_set("ip_range_setting", buf_range);
		nvram_set("range_all","1");
 
		printf("flag = 1  ip_range_setting = %s \n", ip_range_setting);	
	}else if(flag == 2){
		memset(buf_start, 0, sizeof(buf_start));
		memset(buf_end, 0, sizeof(buf_end));
		memset(buf_range, 0, sizeof(buf_range));

		range_ip_01 = websGetVar(wp, "range_ip_01", "0");
		range_ip_02 = websGetVar(wp, "range_ip_02", "0");
		range_ip_03 = websGetVar(wp, "range_ip_03", "0");
		range_ip_04 = websGetVar(wp, "range_ip_04", "0");
		range_ip_11 = websGetVar(wp, "range_ip_11", "0");
		range_ip_12 = websGetVar(wp, "range_ip_12", "0");
		range_ip_13 = websGetVar(wp, "range_ip_13", "0");
		range_ip_14 = websGetVar(wp, "range_ip_14", "0");


		sprintf(buf_start, "%s.%s.%s.%s", range_ip_01, range_ip_02, range_ip_03, range_ip_04);
		sprintf(buf_end, "%s.%s.%s.%s", range_ip_11, range_ip_12, range_ip_13, range_ip_14);
		sprintf(buf_range, "0:2:%s.%s.%s.%s-%s.%s.%s.%s", range_ip_01, range_ip_02, range_ip_03, range_ip_04, range_ip_11, range_ip_12, range_ip_13, range_ip_14);

		nvram_set("range_start_ip", buf_start);
		nvram_set("range_end_ip", buf_end);
		nvram_set("ip_range_setting", buf_range);
		nvram_set("range_all","2");

		printf("flag = 2  ip_range_setting = %s \n", ip_range_setting);	
	}

}

int ej_remote_management_config(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	int ret = 0;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;	
	}


	if(!strcmp(arg, "remote_management"))
	{
		if(gozila_action)
		{
			char *remote_management = websGetVar(wp, "remote_management", "0");

			if(((count == 1) && !strcmp(remote_management, "1")) || ((count == 0) && !strcmp(remote_management, "0")))
			{
				//printf("remote_management = %s\n", remote_management);
				websWrite(wp, "checked");			
			}
		}
		else
		{
			char *remote_management = nvram_safe_get("remote_management");
			if(((count == 1) && !strcmp(remote_management, "1")) || ((count == 0) && !strcmp(remote_management, "0")))
			{
				//printf("remote_management = %s\n", remote_management);
				websWrite(wp, "checked");
			}
		}
	}

	if(!strcmp(arg, "http_remote_username"))
	{
		if(gozila_action)
		{
			char *http_remote_username = websGetVar(wp, "http_remote_username", "0");
				websWrite(wp, http_remote_username);			
		}
		else
		{
			char *http_remote_username = nvram_safe_get("http_remote_username");
				websWrite(wp, http_remote_username);
		}
	}

	if(!strcmp(arg, "http_remote_passwd"))
	{
		if(gozila_action)
		{
			char *http_remote_passwd = websGetVar(wp, "http_remote_passwd", "0");
			websWrite(wp, http_remote_passwd);			
		}
		else
		{
			char *http_remote_passwd = nvram_safe_get("http_remote_passwd");
				websWrite(wp, http_remote_passwd);
		}
	}

	if(!strcmp(arg, "http_remote_passwdConfirm"))
	{
		if(gozila_action)
		{
			char *http_remote_passwdConfirm = websGetVar(wp, "http_remote_passwdConfirm", "0");
			websWrite(wp, http_remote_passwdConfirm);			
		}
		else
		{
			char *http_remote_passwdConfirm = nvram_safe_get("http_remote_passwd");
				websWrite(wp, http_remote_passwdConfirm);
		}
	}

	if(!strcmp(arg, "http_wanport"))
	{
		if(gozila_action)
		{
			char *http_wanport = websGetVar(wp, "http_wanport", "0");
			websWrite(wp, http_wanport);			
		}
		else
		{
			char *http_wanport = nvram_safe_get("http_wanport");
				websWrite(wp, http_wanport);
		}
	}
	
	if(!strcmp(arg, "remote_upgrade"))
	{
		if(gozila_action)
		{
			char *remote_upgrade = websGetVar(wp, "remote_upgrade", "0");

			if(((count == 1) && !strcmp(remote_upgrade, "1")) || ((count == 0) && !strcmp(remote_upgrade, "0")))
			{
				//printf("remote_upgrade = %s\n", remote_upgrade);
				websWrite(wp, "checked");			
			}
		}
		else
		{
			char *remote_upgrade = nvram_safe_get("remote_upgrade");
			if(((count == 1) && !strcmp(remote_upgrade, "1")) || ((count == 0) && !strcmp(remote_upgrade, "0")))
			{
				//printf("remote_upgrade = %s\n", remote_upgrade);
				websWrite(wp, "checked");
			}
		}
	}


	return ret;
}

int ej_ip_restrict_config(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	int ret = 0;
	char *ip_restrict = "0";
	char *temp;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;	
	}


	if(!strcmp(arg, "ip_restrict"))
	{
		if(gozila_action)
		{	
			char *ip_restrict = websGetVar(wp, "ip_restrict", "0");
			//printf("ip_restrict = %s \n", ip_restrict);
			
	
			if(((count == 0) && (!strcmp(ip_restrict, "0"))) || ((count == 1) && (!strcmp(ip_restrict, "1"))) || ((count == 2) && (!strcmp(ip_restrict, "2"))))
			{	
				return websWrite(wp, "selected");
			}
		}else{
			temp = nvram_safe_get("ip_range_setting");
			//printf("temp = %s\n", temp);
			if(*(temp+2) == '1')
				ip_restrict = "1";
			else if(*(temp+2) == '2')
				ip_restrict = "2";

			if(((count == 0) && (!strcmp(ip_restrict, "0"))) || ((count == 1) && (!strcmp(ip_restrict, "1"))) || ((count == 2) && (!strcmp(ip_restrict, "2"))))
			{	
				return websWrite(wp, "selected");
			}
	
		}
	}	

	if(!strcmp(arg, "range_start_ip"))
	{
		if(gozila_action)
		{	
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_01",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_02",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_03",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_04",""));
			}

		}
		else
		{
			return websWrite(wp, "%d", get_single_ip(nvram_safe_get("range_start_ip"),count));
		}
	}

	if(!strcmp(arg, "range_end_ip"))
	{
		if(gozila_action)
		{	
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_11",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_12",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_13",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"range_ip_14",""));
			}

		}
		else
		{
			return websWrite(wp, "%d", get_single_ip(nvram_safe_get("range_end_ip"),count));
		}
	}

	return ret;
}

//for IP Range Restricted
int ej_show_ip_range_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *ip_restrict = "0";
	char *temp;

	if(gozila_action)	
		ip_restrict = websGetVar(wp, "ip_restrict", "0");	
	else{
		temp = nvram_safe_get("ip_range_setting");
		//printf("temp = %s\n", temp);
		if(*(temp+2) == '1')
			ip_restrict = "1";
		else if(*(temp+2) == '2')
			ip_restrict = "2";
						
		//printf("ip_restrict = %s\n", ip_restrict);
	}
        
	if(!strcmp(ip_restrict, "0"))
		do_ej("Management_all.asp", wp);
	else if(!strcmp(ip_restrict, "1"))
		do_ej("Management_ipaddr.asp", wp);
	else if(!strcmp(ip_restrict, "2"))
		do_ej("Management_range.asp", wp);

	return ret;
}




