
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
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

#include <broadcom.h>

#include <cy_conf.h>
#include "pvc.h"

#define	STATUS_RETRY_COUNT	5
#define STATUS_REFRESH_TIME1	5	
#define STATUS_REFRESH_TIME2	60

extern char vcc_config_buf[60];
extern char pppoe_config_buf[210];
extern char pppoa_config_buf[150];
extern char bridged_config_buf[100];
extern char routed_config_buf[100];
#ifdef CLIP_SUPPORT
extern char clip_config_buf[100];
#endif
extern int vcc_inited;
extern int pppoa_inited;
extern int pppoe_inited;
extern int bridged_inited;
extern int routed_inited;
#ifdef CLIP_SUPPORT
extern int clip_inited;
#endif
int retry_count = -1;
int refresh_time = STATUS_REFRESH_TIME2;

extern char connection[5];

/* da_f 2005.4.20 modify */
#ifdef PPP_DETAIL_STAT_SUPPORT
extern int check_ppp_authfail(int num);
extern int check_ppp_connserv_fail(int num);
extern int check_ppp_getip_fail(int num);
#endif


int ej_show_uptime(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char s[20];
	int uptime;
	int days, hours, minutes;

	if ((fp = fopen("/proc/uptime", "r")) == NULL)
		return websWrite(wp, "-");

	fscanf(fp, "%d", &uptime);
	fclose(fp);
	minutes = uptime / 60;
	hours = minutes / 60;
	days = hours / 24;
	minutes = minutes % 60;
	hours = hours % 24;

	sprintf(s, "%d day%s, %d:%02d", days, (days != 1) ? "s" : "", hours, minutes);

	return websWrite(wp, s);
}

int
ej_show_status_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

	do_ej("Status_Router1.asp",wp);

	return ret;
}

int
ej_show_status2_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	do_ej("Status_Router2.asp",wp);
#endif

	return ret;
}

/* Report time in RFC-822 format */
int
ej_localtime(int eid, webs_t wp, int argc, char_t **argv)
{
        time_t tm;
        char *output;
        time(&tm);

	ejArgs(argc, argv, "%s", &output);
        if(time(0) > (unsigned long)60*60*24*365)
	{//dom 2005/05/27 check ang change month to other language
		if(!strcmp(output, "<script>Capture(share.notava)</script>"))
		{
			char *mon[] = { "January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December" };
			char *mon2[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
			char *day[] = { "suns", "mon", "tue", "wed", "thu", "fri","sat" };
			char *day2[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			char *tok, time_tmp[strlen(rfctime(&tm))];
			int i, ret = 0;
			strcpy(time_tmp, rfctime(&tm));
			for ( i = 0; i < 7; i++ )
			{
				if ( strstr( time_tmp, day2[i] ) != NULL )
					ret = websWrite(wp, "<script>Capture(accinbtraffic.%s)</script>", day[i]);
			}
			for ( i = 0; i < 12; i++ )
			{
				if ( strstr( time_tmp, mon2[i] ) != NULL )
				{
					tok = strtok(time_tmp, ", ");
					tok = strtok(NULL, mon2[i]);
					ret += websWrite(wp, "%s", tok);
					ret += websWrite(wp, ", <script>Capture(share.%s)</script>", mon[i]);
					while ((tok = strtok(NULL, mon2[i])) != NULL)
						ret += websWrite(wp, "%s", tok);
					return ret;
				}
			}
		}
		return websWrite(wp, rfctime(&tm));
	}
        else
        {
//              ejArgs(argc, argv, "%s", &output);
		if(!strcmp(output, "<script>Capture(share.notava)</script>"))
			return websWrite(wp, output);
		return websWrite(wp, "Not Available");
        }
}	

int
ej_dhcp_remaining_time(int eid, webs_t wp, int argc, char_t **argv)
{
        int ret = 0;
	unsigned long now_time = 0L;
	unsigned long get_leases_time = 0L;
	unsigned long leases_time = 0L;
	unsigned long remain_time = 0L;
	struct sysinfo info;
	char string[80];
	int day=0, hour=0, min=0, sec=0;
	FILE *fp;

	if(nvram_invmatch("wan_proto","dhcp"))	return ret;
	
	leases_time = atol(nvram_safe_get("wan_lease"));

	if ((fp = fopen("/tmp/udhcpc.expires", "r")) != NULL) {
		fscanf(fp,"%s",string);
		get_leases_time = atol(string) - leases_time;
		fclose(fp);
	}

	if(get_leases_time ==0 || leases_time == 0)
		return websWrite(wp, "0");

	sysinfo(&info);
	now_time = info.uptime;

	remain_time = leases_time - (now_time - get_leases_time);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): now_time=[%ld] get_lease_time=[%ld] lease_time=[%ld] remain_time=[%ld]",__FUNCTION__,now_time,get_leases_time,leases_time,remain_time);
#endif

	if(remain_time < 0)
		return websWrite(wp, "0");
		
	if(leases_time){
		if (remain_time > 60*60*24) {  //days
			day = (int)remain_time / (60*60*24);
			remain_time %= 60*60*24;
		}
		if (remain_time > 60*60) {   //hours
			hour = (int)remain_time / (60*60);
			remain_time %= 60*60;
		}

		if (remain_time > 60) {      //miniutes
			min = (int)remain_time / 60;
			remain_time %= 60;
		}			     //seconds

		sec = (int)remain_time;
	}

	if(day)
		ret += websWrite(wp, "%d days, ", day);
	ret += websWrite(wp, "%d:%02d:%02d", hour, min, sec);

	return ret;
}
/*
int sar_getstats(char *stat_str)
{
    	FILE *fd;
    	int var=0;
    	int retval;
    	char *pbuff;
    	char *pos;
    
    	pbuff=(char *) malloc(2*1024);
    	if(!pbuff)
    	{
        	printf("out of memory\n");
        	return -1;
    	}

    	fd=fopen("/proc/avalanche/avsar_modem_stats", "r");
    	if(!fd)
      		return -1;

    	fread(pbuff, sizeof(char), 2*1024*sizeof(char), fd);
    	fclose(fd);

    	pos=strstr(pbuff, stat_str);
    	pos += strlen(stat_str);

    	if(pos)
        	retval = sscanf(pos," %d", &var);
    
    	free(pbuff);
    	return var;
}
*/
int
ej_status_connection_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type;
	//char readbuf[256];
	char *submit_button = websGetVar(wp, "submit_button", NULL);

	if (ejArgs(argc, argv, "%s", &type) < 1)
	{
		websError(wp, 400, "Insufficient args\n");					return -1;
	}
	
	if(gozila_action)  //form post
	{
		char *tmpconn = websGetVar(wp, "status_connection", NULL);
		int which_tmp;
		//char word[32], *next;
		if(tmpconn != NULL)
			strncpy(connection, tmpconn, sizeof(connection));
		which_tmp = atoi(connection);
		//2004.3.22 junzhao
		nvram_set("wan_post_connection", connection);
		
		if(!strcmp(submit_button, "Status_Router") || !strcmp(submit_button, "Status_DSL"))
		{
			vcc_config_init();
			if(!strcmp(encapmode, "1483bridged"))
				bridged_config_init();
			//add by junzhao 2004.3.10
			else if(!strcmp(encapmode, "routed"))
				routed_config_init();
		#ifdef CLIP_SUPPORT
			else if(!strcmp(encapmode, "clip"))
				clip_config_init();
		#endif
		}
	}
	else
	{
		//strncpy(connection, nvram_safe_get("wan_connection"), sizeof(connection));
		//junzhao 2004.3.22
		strncpy(connection, nvram_safe_get("wan_post_connection"), sizeof(connection));
		//strncpy(connection, "0", sizeof(connection));
		//strncpy(connection, nvram_safe_get("wan_post_connection"), sizeof(connection));
		//if(!vcc_inited)	 
		{
			vcc_config_init();
			if(!strcmp(encapmode, "1483bridged"))
				bridged_config_init();	
			//add by junzhao 2004.3.10
			else if(!strcmp(encapmode, "routed"))
				routed_config_init();
		#ifdef CLIP_SUPPORT
			else if(!strcmp(encapmode, "clip"))
				clip_config_init();
		#endif
		}
	}	
	
	which = atoi(connection);
	if(!strcmp(type, "select"))
	{
	#ifdef MULTIPVC_SUPPORT
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
	#else
		for(i=0 ; i<1 ; i++)
	#endif
		//single pvc
		//for(i=0; i<PVC_CONNECTIONS; i++)							ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
		//for(i=0; i<1; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	}
	else if(!strcmp(type, "upselect"))
	{
		char word[PVC_CONNECTIONS][6], readbuf[60];
		memset(word, 0, sizeof(word));
		memset(readbuf, 0, sizeof(readbuf));
		if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	        {
			printf("no buf in %s!\n", WAN_IFNAME);
	               	return 0;
	        }
		sscanf(readbuf, "%6[^:]:%6[^:]:%6[^:]:%6[^:]:%6[^:]:%6[^:]:%6[^:]:%6[^\n]", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);  
		
	#ifdef MULTIPVC_SUPPORT
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
	#else
		for(i=0 ; i<1 ; i++)
	#endif
		//for(i=0 ; i<PVC_CONNECTIONS; i++)
		//single pvc
		//for(i=0 ; i<1; i++)
		{
			if(strcmp(word[i], "Down"))
				ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
			else
				ret = websWrite(wp,"\t\t<!--<option value=\"%d\" %s> %d </option>-->\n",i,(i == which) ? "selected" : "",i+1);
		}		
	}
	return ret;	  
}
//junzhao 2004.6.14
int ej_pppoe_show_htm(int eid, webs_t wp, int argc, char_t **argv)
{
#if 1
	int count;
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	if(strcmp(encapmode, "pppoe"))
	{
		if(count == 0)
			return websWrite(wp, "<!--");
		else if(count == 1)
			return websWrite(wp, "-->");
	}
	else
	{
		if(count == 2)
			return websWrite(wp, "<!--");
		else if(count == 3)
			return websWrite(wp, "-->");
	}
#endif
	return 0;
}

int ej_status_htm(int eid, webs_t wp, int argc, char_t **argv)
{
	char *wantype, *name;
	int count;
	char readbuf[256], *tmpstr = readbuf;

	if(ejArgs(argc, argv, "%s %s %d", &wantype, &name, &count) < 3)
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

	if(!strcmp(wantype, "ppp"))  //ppp type htm
	{
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if(strcmp(encapmode, "pppoe") && strcmp(encapmode, "1483bridged"))
		{
			if(!strcmp(name, "multipppoe_hidden"))
			{
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			}
		}
		else
		{
			char word[3], *next;
			int which = atoi(nvram_safe_get("wan_post_connection"));
		        if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				foreach(word, nvram_safe_get("mpppoe_enable"), next)
				{
					if(which -- == 0 )
					{
						break;
					}
				}
			#else
				if(!strcmp(name, "multipppoe_hidden"))
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				foreach(word, nvram_safe_get("ipppoe_enable"), next)
				{
					if(which -- == 0 )
					{
						break;
					}
				}
			#else
				if(!strcmp(name, "multipppoe_hidden"))
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			#endif
			}
			
			if(!strcmp(word, "0"))
			{
				if(!strcmp(name, "multipppoe_hidden"))
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			}
		}
	#else
		if(!strcmp(name, "multipppoe_hidden"))
		{
			if(count == 0)
				return websWrite(wp, "<!--");
			else if(count == 1)
				return websWrite(wp, "-->");
		}
	#endif
		/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT	
             if(strcmp(encapmode, "pppoe") && strcmp(encapmode, "unpppoe") && strcmp(encapmode, "pppoa") && strcmp(encapmode, "unpppoa")) 
#else
		if(strcmp(encapmode, "pppoe") && strcmp(encapmode, "pppoa"))
#endif

		{
		#ifdef IPPPOE_SUPPORT 
			if(!strcmp(encapmode, "1483bridged"))
			{
				if(!strcmp(name, "connect"))
				{
					//FILE *fp;
					//char filename[18];
					char *ifname = NULL;
					int which = atoi(connection), i;
			
					if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	        			{
						printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	               	 			return 0;
	      	 	 		}
					//junzhao 2004.8.4
					for(i=0; i<PVC_CONNECTIONS; i++)
					{
						ifname = strsep(&tmpstr, " ");
						if(i == which)
							break;
					}
					/*
					foreach(word, readbuf, next)
					{
						if(which -- == 0 )
						{
							ifname = word;
							break;
						}
					}
					*/
					if(!strcmp(ifname, "Down"))
						return websWrite(wp, "disabled");
					if(check_ppp_link(PVC_CONNECTIONS + atoi(connection)))
						return websWrite(wp, "disabled");
					return 0;
				}
				else if(!strcmp(name, "disconnect"))
				{
					//FILE *fp;
					//char filename[18];
					char *ifname = NULL;
					int which = atoi(connection), i;

					if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	        			{
						printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	               		 		return 0;
	        			}
					
					//junzhao 2004.8.4
					for(i=0; i<PVC_CONNECTIONS; i++)
					{
						ifname = strsep(&tmpstr, " ");
						if(i == which)
							break;
					}

					/*
					foreach(word, readbuf, next)
					{
						if(which -- == 0 )
						{
							ifname = word;
							break;
						}
					}
					*/
					if(!strcmp(ifname, "Down"))
						return websWrite(wp, "disabled");
					if(!check_ppp_link(PVC_CONNECTIONS + atoi(connection)))
						return websWrite(wp, "disabled");
					return 0;
				}
			}
		#endif
			if(!strcmp(name, "hidden"))
			{
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			}
			else
				return 0;
		}
		else if(!strcmp(name, "connect"))
		{
			//FILE *fp;
			//char filename[18];
			char *ifname = NULL;
			int which = atoi(connection), i;
			
		#ifdef MPPPOE_SUPPORT 
			if(count == 1)
			{
				if(!strcmp(encapmode, "pppoe"))
				{
					if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	        			{
						printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	               	 			return 0;
	      	 	 		}
					//junzhao 2004.8.4
					for(i=0; i<PVC_CONNECTIONS; i++)
					{
						ifname = strsep(&tmpstr, " ");
						if(i == which)
							break;
					}

					/*
					foreach(word, readbuf, next)
					{
						if(which -- == 0 )
						{
							ifname = word;
							break;
						}
					}
					*/
					if(!strcmp(ifname, "Down"))
						return websWrite(wp, "disabled");
					if(check_ppp_link(PVC_CONNECTIONS + atoi(connection)))
						return websWrite(wp, "disabled");
				}
			}
			else
		#endif
			{
				if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	        		{
					printf("no buf in %s!\n", WAN_IFNAME);
	               	 		return 0;
	        		}
				//foreach(word, nvram_safe_get("wan_ifname"), next)
				//junzhao 2004.8.4
				for(i=0; i<PVC_CONNECTIONS; i++)
				{
					ifname = strsep(&tmpstr, " ");
					if(i == which)
						break;
				}

				/*
				foreach(word, readbuf, next)
				{
					if(which -- == 0 )
					{
						ifname = word;
						break;
					}
				}
				*/
				if(!strcmp(ifname, "Down"))
					return websWrite(wp, "disabled");
			
				if(check_ppp_link(atoi(connection)))
					return websWrite(wp, "disabled");
			}	
		}			
		else if(!strcmp(name, "disconnect"))
		{
			//FILE *fp;
			//char filename[18];
			char *ifname = NULL;
			int which = atoi(connection), i;
			
		#ifdef MPPPOE_SUPPORT
			if(count == 1)
			{
				if(!strcmp(encapmode, "pppoe"))
				{
					if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	        			{
						printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	               		 		return 0;
	        			}
					
					//junzhao 2004.8.4
					for(i=0; i<PVC_CONNECTIONS; i++)
					{
						ifname = strsep(&tmpstr, " ");
						if(i == which)
							break;
					}

					/*
					foreach(word, readbuf, next)
					{
						if(which -- == 0 )
						{
							ifname = word;
							break;
						}
					}
					*/
					if(!strcmp(ifname, "Down"))
						return websWrite(wp, "disabled");
					if(!check_ppp_link(PVC_CONNECTIONS + atoi(connection)))
						return websWrite(wp, "disabled");
				}
			}
			else
		#endif
			{
				if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	        		{
					printf("no buf in %s!\n", WAN_IFNAME);
	               	 		return 0;
	        		}
				//foreach(word, nvram_safe_get("wan_ifname"), next)
				//junzhao 2004.8.4
				for(i=0; i<PVC_CONNECTIONS; i++)
				{
					ifname = strsep(&tmpstr, " ");
					if(i == which)
						break;
				}

				/*
				foreach(word, readbuf, next)
				{
					if(which -- == 0 )
					{
						ifname = word;
						break;
					}
				}
				*/
				if(!strcmp(ifname, "Down"))
					return websWrite(wp, "disabled");
			
				if(!check_ppp_link(atoi(connection)))
					return websWrite(wp, "disabled");
			}
		}			
	}
	else if(!strcmp(wantype, "dhcp"))
	{
		if(!strcmp(name, "hidden"))
		{
			if((strcmp(encapmode, "1483bridged")) || (!strcmp(encapmode, "1483bridged") && !strcmp(dhcpenable, "0"))) 
			{
				if(count == 0)
					return websWrite(wp, "<!--");
				else if(count == 1)
					return websWrite(wp, "-->");
			}
			else
			{
				char *ifname = NULL;
				int which = atoi(connection), i;
				if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	        		{
					printf("no buf in %s!\n", WAN_IFNAME);
	               	 		return 0;
	        		}
				//foreach(word, nvram_safe_get("wan_ifname"), next)
				//junzhao 2004.8.4
				for(i=0; i<PVC_CONNECTIONS; i++)
				{
					ifname = strsep(&tmpstr, " ");
					if(i == which)
						break;
				}

				/*
				foreach(word, readbuf, next)	      
				{
					if(which -- == 0 )
					{
						ifname = word;
						break;
					}
				}
				*/
				if(!strcmp(ifname, "Down"))
				{
					if(count == 0)
						return websWrite(wp, "<!--");
					else if(count == 1)
						return websWrite(wp, "-->");
				}
			}
		}
		else if(!strcmp(name, "release"))
		{
			char *ip_addr = NULL;
			char ip_buf[256],*ip_str = ip_buf;
			int which = atoi(connection), i;
			if(!file_to_buf(WAN_IPADDR, ip_buf, sizeof(ip_buf)))
        		{
				printf("no buf in %s!\n", WAN_IPADDR);
               	 		return 0;
        		}
				
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				ip_addr = strsep(&ip_str, " ");
				if(i == which)
					break;
			}

			//printf("release:ip_addr=%s\n",ip_addr);
			
			if(!strcmp(ip_addr, "0.0.0.0"))
			{
				return websWrite(wp, "disabled");
			}
		}
		else if(!strcmp(name, "renew"))
		{
			char *ip_addr = NULL;
			char ipbuf[256],*ip_str = ipbuf;
			int which = atoi(connection), i;
			
			if(!file_to_buf(WAN_IPADDR, ipbuf, sizeof(ipbuf)))
        		{
				printf("no buf in %s!\n", WAN_IPADDR);
               	 		return 0;
        		}
				
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				ip_addr = strsep(&ip_str, " ");
				if(i == which)
					break;
			}

			//printf("renew:ip_addr=%s\n",ip_addr);

			if(strcmp(ip_addr, "0.0.0.0"))
			{
				return websWrite(wp, "disabled");
			}
		}

	}
			
	return 0;
}	

//junzhao 2004.4.16
int ej_hwaddr_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	char hwaddr[20];
	if(ejArgs(argc, argv, "%s", &arg) < 1)
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	memset(hwaddr, 0, sizeof(hwaddr));
	if(!strcmp(arg, "lan"))
		strncpy(hwaddr, nvram_safe_get("lan_hwaddr"), sizeof(hwaddr));
	else if(!strcmp(arg, "wan"))
		strncpy(hwaddr, nvram_safe_get("wan_def_hwaddr"), sizeof(hwaddr));
	hwaddr_convert(hwaddr);
	return websWrite(wp, "%s", hwaddr);
}

int ej_nvram_wanstatus_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	char readbuf[512], *tmpstr = readbuf;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

	if(!strcmp(arg, "login_type"))
	{
		if((!strcmp(encapmode, "pppoe")) || (!strcmp(encapmode, "pppoa")) || (!strcmp(encapmode, "bridgedonly")))
		{
			if(!strcmp(encapmode, "pppoe"))
			{
				int ret = websWrite(wp, "PPPoE");
			//junzhao 2004.9.7
			#ifdef VZ7
				char pppoe_dhcp_enable[20];
	  			memset(pppoe_dhcp_enable, 0, sizeof(pppoe_dhcp_enable));
	  			sprintf(pppoe_dhcp_enable, "pppoe_dhcp_enable%d", atoi(connection));
	   			if(nvram_match(pppoe_dhcp_enable, "0"))
					return ret;
				else
				{
					char readbuf[130], *tmp = readbuf, *tmpptr;
					int i;
					if(!file_to_buf(WAN_DHCPD_IPADDR, readbuf, sizeof(readbuf)))
						return ret;
					for(i=0; i<PVC_CONNECTIONS; i++)
					{		
						tmpptr = strsep(&tmp, " ");
						if(i == atoi(connection))
							break;
					}
					if(!strcmp(tmpptr, "0.0.0.0"))
					{
						if ( count != 10)
							ret = websWrite(wp, " (Couldn't find any DHCP Server)");
						else
							ret = websWrite(wp, "<script>Capture(srccode.coudnf)</script>");
					}
					else
					{
						if ( count != 10 )
							ret = websWrite(wp, " (Find DHCP Server at %s)",  tmpptr);
						else
							ret = websWrite(wp, "<script>Capture(srccode.fdhcpat)</script> %s", tmpptr);
					}
				}
			#endif
				return ret;
			}
			else if(!strcmp(encapmode, "pppoa"))
				return websWrite(wp, "PPPoA");
			else if(!strcmp(encapmode, "bridgedonly"))
			{
				if ( count != 10 )
					return websWrite(wp, "Bridged Mode Only");
				else
					return websWrite(wp, "<script>Capture(share.bridgeonly)</script>");
			}
		}
		/* kirby for unip 2004/11.29 */
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encapmode, "unpppoe"))
		{
			if ( count != 10 )
		  		return websWrite(wp, "IP Unnumbered over PPPoE");
			else
				return websWrite(wp, "<script>Capture(share.unnumpppoe)</script>");
		}
              else if(!strcmp(encapmode, "unpppoa"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "IP Unnumbered over PPPoA");
                        else
                                return websWrite(wp, "<script>Capture(share.unnumpppoa)</script>");
                }
#endif
		else if(!strcmp(encapmode, "1483bridged"))
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "DHCP");
			if ( count != 10 )
				return websWrite(wp, "Static");
			else
				return websWrite(wp, "<script>Capture(share.statics)</script>");
		}
		else if(!strcmp(encapmode, "routed"))
		{
			if ( count != 10 )
				return websWrite(wp, "Static");
			else
				return websWrite(wp, "<script>Capture(share.statics)</script>");
		}
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "Static");
                        else
                                return websWrite(wp, "<script>Capture(share.statics)</script>");
                }
	#endif
	}
//junzhao 2004.6.14
	else if(!strcmp(arg, "srvname"))	
	{
		char word[64], *ptr;
		int whichconn = atoi(connection), i;
		memset(word, 0, sizeof(word));
		if(count == 0)
		{
			if(!file_to_buf(WAN_PPPOE_SRVNAME, readbuf, sizeof(readbuf)))
	       		{
				printf("no buf in %s!\n", WAN_PPPOE_SRVNAME);
	               	 	return 0;
	        	}
		}
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		else if(count == 1)
		{
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_SRVNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_SRVNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_SRVNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_SRVNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}		
			//junzhao 2004.11.2
			else
				return 0;
		}
	#endif	
		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			ptr = strsep(&tmpstr, " ");
			if(i == whichconn)
			{
				if(strcmp(ptr, "None"))
					return websWrite(wp, "%s", ptr);
				return 0;
			}
		}

		/*
		foreach(word, readbuf, next)
		{
			if(whichconn-- == 0)
			{
				if(strcmp(word, "None"))
					return websWrite(wp, "%s", word);
				return 0;
			}
		}
		*/	
	}
	else if(!strcmp(arg, "acname"))	
	{
		char word[64], *ptr;
		int whichconn = atoi(connection), i;
		memset(word, 0, sizeof(word));
		if(count == 0)
		{
			if(!file_to_buf(WAN_PPPOE_ACNAME, readbuf, sizeof(readbuf)))
	       		{
				printf("no buf in %s!\n", WAN_PPPOE_ACNAME);
	               	 	return 0;
	        	}
		}
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		else if(count == 1)
		{
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_ACNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_ACNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_ACNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_ACNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}		
			//junzhao 2004.11.2
			else
				return 0;
		}
	#endif	
		
		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			ptr = strsep(&tmpstr, " ");
			if(i == whichconn)
			{
				if(strcmp(ptr, "None"))
					return websWrite(wp, "%s", ptr);
				return 0;
			}
		}

		/*
		foreach(word, readbuf, next)
		{
			if(whichconn-- == 0)
			{
				if(strcmp(word, "None"))
					return websWrite(wp, "%s", word);
				return 0;
			}
		}
		*/
	}	
	else if(!strcmp(arg, "wan_interface"))	
	{
		char word[32], *waninterface = NULL;
		int whichconn = atoi(connection), i;
		memset(word, 0, sizeof(word));
		
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if( count == 1 )
		{
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
		}
		else
	#endif	
		{
		if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_IFNAME);
	               	 return 0;
	        }
	        }
		//foreach(word, nvram_safe_get("wan_ifname"), next)
		/*
		foreach(word, readbuf, next)
		{
			if(whichconn-- == 0)
			{
				waninterface = word;
				break;
			}
		}
		*/
		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			waninterface = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}

		if(!strcmp(waninterface, "Down"))		
		{
			if ( count != 10 )
				return websWrite(wp, "%s", waninterface);
			else
				return websWrite(wp, "%s", "<script>Capture(share.down)</script>");
		}
		else
		{
			if(strstr(waninterface, "ppp"))
			{
				char *tmp = &waninterface[3];
				if(check_ppp_link(atoi(tmp)))
				{
					if ( count != 10 )
						return websWrite(wp, "%s", "Up");
					else
						return websWrite(wp, "%s", "<script>Capture(share.up)</script>");
				}
/* da_f 2005.4.20 modify */
#ifdef PPP_DETAIL_STAT_SUPPORT
				else if(check_ppp_authfail(atoi(tmp)))
					return websWrite(wp, "%s", "PPP authentication failed");
				else if(check_ppp_getip_fail(atoi(tmp)))
					return websWrite(wp, 
							"Cannot get IP from %s server",
							(strcmp(encapmode, "pppoe"))?"PPPoA":"PPPoE");
				else if(check_ppp_connserv_fail(atoi(tmp)))
					return websWrite(wp, 
							"Cannot connect to %s server",
							(strcmp(encapmode, "pppoe"))?"PPPoA":"PPPoE");
#endif
				else
                                {
                                        if ( count != 10 )
                                                return websWrite(wp, "%s", "Down");
                                        else
                                                return websWrite(wp, "%s", "<script>Capture(share.down)</script>");
                                }
			}
			else
                        {
                                if ( count != 10 )
                                        return websWrite(wp, "%s", "Up");
                                else
                                        return websWrite(wp, "%s", "<script>Capture(share.up)</script>");
                        }
		}							
	}
	else if(!strcmp(arg, "wan_hwaddr"))
	{
		char word[32], *wanhwaddr = NULL;
		//int whichconn = atoi(connection);
		int whichconn = 0, i;
		
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_HWADDR, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_HWADDR);
	               	return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_hwaddr"), next)
		{
			if(whichconn-- == 0)
			{
				wanhwaddr = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wanhwaddr = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		return websWrite(wp, "%s", wanhwaddr);
	}	
	else if(!strcmp(arg, "wan_ipaddr"))	
	{
		char word[32], *wanipaddr = NULL;
		int whichconn = atoi(connection), i;
		
		//add by junzhao 2004.3.10
		if(count == 0)
		{
		if(!strcmp(encapmode, "1483bridged") && !strcmp(dhcpenable, "0"))
			return websWrite(wp, "%s", b_ipaddr);
		else if(!strcmp(encapmode, "routed"))
			return websWrite(wp, "%s", r_ipaddr);
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
			return websWrite(wp, "%s", c_ipaddr);
	#endif
 		/* kirby for unip 2004/12.2 */ 
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encapmode, "unpppoe"))
			return websWrite(wp, "%s", "Unnumbered IP");
		else if(!strcmp(encapmode, "unpppoa"))
			return websWrite(wp, "%s", "Unnumbered IP");	  
#endif
	#ifdef IPCP_NETMASK_SUPPORT
		//zhangbin for unip
		else if(nvram_match("ipcp_netmask_enable", "1"))
		{
			//if(!strcmp(wanconn_status,"UP"))
			{
				//char *unip_addr = nvram_safe_get("lan_ipaddr");
				//printf("wanconn_status=%s\n",wanconn_status);
				return websWrite(wp, "%s", "Unnumbered" );
			}
		}
	#endif

		}	
		memset(word, 0, sizeof(word));
		
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if(count == 1)
		{
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_IPADDR);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_IPADDR);
	               	 		return 0;
	        		}
			#else
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
		}
		else
	#endif	
		{
		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_IPADDR);
	               	 return 0;
	        }
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_ipaddr"), next)
		{
			if(whichconn-- == 0)
			{
				wanipaddr = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wanipaddr = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		if(!strstr(wanipaddr, "10.64.64"))		
			return websWrite(wp, "%s", wanipaddr);
		else
			return websWrite(wp, "0.0.0.0");
	}
	else if(!strcmp(arg, "wan_netmask"))	
	{
		char word[32], *wanipaddr = NULL, *wannetmask = NULL;
		int whichconn = atoi(connection), i;
		
		//add by junzhao 2004.3.10
		if(count == 0)
		{
		if(!strcmp(encapmode, "1483bridged") && !strcmp(dhcpenable, "0"))
			return websWrite(wp, "%s", b_netmask);
		else if(!strcmp(encapmode, "routed"))
			return websWrite(wp, "%s", r_netmask);
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
			return websWrite(wp, "%s", c_netmask);
	#endif
		/* kirby for unip 2004/12.2 */ 
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encapmode, "unpppoe"))
			return websWrite(wp, "%s", "Unnumbered IP Netmask");
		else if(!strcmp(encapmode, "unpppoa"))
			return websWrite(wp, "%s", "Unnumbered IP Netmask");	  
#endif
	#ifdef IPCP_NETMASK_SUPPORT
		//zhangbin for unip
		else if(nvram_match("ipcp_netmask_enable", "1"))
		{
			//if(!strcmp(wanconn_status,"UP"))
			{
				//char *unip_mask = nvram_safe_get("lan_netmask");
				//printf("wanconn_status2=%s\n",wanconn_status);
				return websWrite(wp, "%s", "Unnumbered");
			}
		}
	#endif
		
		}
		memset(word, 0, sizeof(word));
		
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if(count == 1)
		{
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_IPADDR);
	               			return 0;
	        		}
			#else 
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_IPADDR);
	               			return 0;
	        		}
			#else 
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
			/*
			foreach(word, readbuf, next)
			{
				if(whichconn-- == 0)
				{
					wanipaddr = word;
					break;
				}
			}
			*/

			//junzhao 2004.8.4
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				wanipaddr = strsep(&tmpstr, " ");
				if(i == whichconn)
					break;
			}
			if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
				return websWrite(wp, "0.0.0.0");
		
			whichconn = atoi(connection);
			memset(word, 0, sizeof(word));
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_NETMASK, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_NETMASK);
	               			return 0;
	        		}
			#else 
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_NETMASK, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_NETMASK);
	               			return 0;
	        		}
			#else 
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
			/*
			foreach(word, readbuf, next)
			{
				if(whichconn-- == 0)
				{
					wannetmask = word;
					break;
				}
			}
			*/

			//junzhao 2004.8.4
			tmpstr = readbuf;
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				wannetmask = strsep(&tmpstr, " ");
				if(i == whichconn)
					break;
			}
			return websWrite(wp, "%s", wannetmask);
		}
		else
	#endif
		{
		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_IPADDR);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_ipaddr"), next)
		{
			if(whichconn-- == 0)
			{
				wanipaddr = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wanipaddr = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
			return websWrite(wp, "0.0.0.0");
		
		whichconn = atoi(connection);
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_NETMASK, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_NETMASK);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_netmask"), next)
		{
			if(whichconn-- == 0)
			{
				wannetmask = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		tmpstr = readbuf;
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wannetmask = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		return websWrite(wp, "%s", wannetmask);
		}
	}
	else if(!strcmp(arg, "wan_gateway"))	
	{
		char word[32], *wanipaddr = NULL, *wangateway = NULL;
		int whichconn = atoi(connection), i;
		
		//add by junzhao 2004.3.10
		if(count == 0)
		{
		if(!strcmp(encapmode, "1483bridged") && !strcmp(dhcpenable, "0"))
			return websWrite(wp, "%s", b_gateway);
		else if(!strcmp(encapmode, "routed"))
			return websWrite(wp, "%s", r_gateway);
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
			return websWrite(wp, "%s", c_gateway);
	#endif
 		/* kirby for unip 2004/12.2 */ 
#ifdef UNNUMBERED_SUPPORT
              else if(!strcmp(encapmode, "unpppoe"))
			return websWrite(wp, "%s", "Unnumbered IP Gateway");
		else if(!strcmp(encapmode, "unpppoa"))
			return websWrite(wp, "%s", "Unnumbered IP Gateway");	  
#endif
	#ifdef IPCP_NETMASK_SUPPORT
		//zhangbin for unip
		else if(nvram_match("ipcp_netmask_enable", "1"))
		{
			//if(!strcmp(wanconn_status,"UP"))
			{
				//char *unip_gateway = nvram_safe_get("lan_ipaddr");
				//printf("wanconn_status3=%s\n",wanconn_status);
				return websWrite(wp, "%s", "Unnumbered");
			}
		}
	#endif

		}

	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if(count == 1)
		{
			memset(word, 0, sizeof(word));
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_IPADDR);
	       	        		return 0;
	       			}
			#else 
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_IPADDR, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_IPADDR);
	       	        		return 0;
	       			}
			#else 
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
			/*
			foreach(word, readbuf, next)
			{
				if(whichconn-- == 0)
				{
					wanipaddr = word;
					break;
				}
			}
			*/

			//junzhao 2004.8.4
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				wanipaddr = strsep(&tmpstr, " ");
				if(i == whichconn)
					break;
			}
			if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
				return websWrite(wp, "0.0.0.0");
		
			whichconn = atoi(connection);
		
			memset(word, 0, sizeof(word));
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_GATEWAY);
	 	              		return 0;
	       	 		}
			#else 
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_GATEWAY);
	 	              		return 0;
	       	 		}
			#else 
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
			/*
			foreach(word, readbuf, next)
			{
				if(whichconn-- == 0)
				{
					wangateway = word;
					break;
				}
			}
			*/

			//junzhao 2004.8.4
			tmpstr = readbuf;
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				wangateway = strsep(&tmpstr, " ");
				if(i == whichconn)
					break;
			}
			return websWrite(wp, "%s", wangateway);
		}
		else
	#endif
		{
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_IPADDR);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_ipaddr"), next)
		{
			if(whichconn-- == 0)
			{
				wanipaddr = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		tmpstr = readbuf;
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wanipaddr = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
			return websWrite(wp, "0.0.0.0");
		
		whichconn = atoi(connection);
		
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_GATEWAY);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_gateway"), next)
		{
			if(whichconn-- == 0)
			{
				wangateway = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		tmpstr = readbuf;
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			wangateway = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		return websWrite(wp, "%s", wangateway);
		}
	}
	else if(!strcmp(arg, "wan_dns"))
	{
	#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
		if(count > 2)
		{
			char word[128], *wandns = NULL;
			char wandns1[32], wandns2[32];
			int whichconn = atoi(connection), i;
			memset(word, 0, sizeof(word));
			memset(wandns1, 0, sizeof(wandns1));
			memset(wandns2, 0, sizeof(wandns2));
			if(!strcmp(encapmode, "pppoe"))
			{
			#if defined(MPPPOE_SUPPORT)
				if(!file_to_buf(WAN_MPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_MPPPOE_GET_DNS);
	 	              		return 0;
	       	 		}
			#else 
				return 0;
			#endif
			}
			else if(!strcmp(encapmode, "1483bridged"))
			{
			#if defined(IPPPOE_SUPPORT)
				if(!file_to_buf(WAN_IPPPOE_GET_DNS, readbuf, sizeof(readbuf)))
	       			{
					printf("no buf in %s!\n", WAN_IPPPOE_GET_DNS);
	 	              		return 0;
	       	 		}
			#else 
				return 0;
			#endif
			}
			//junzhao 2004.11.2
			else
				return 0;
			/*
			foreach(word, readbuf, next)
			{
				if(whichconn-- == 0)
				{
					wandns = word;
					break;
				}
			}
			*/

			//junzhao 2004.8.4
			for(i=0; i<PVC_CONNECTIONS; i++)
			{
				wandns = strsep(&tmpstr, " ");
				if(i == whichconn)
					break;
			}
			sscanf(wandns, "%32[^:]:%32[^\n]", wandns1, wandns2);  
			if(count == 3)
				return websWrite(wp, "%s", wandns1);
			else if(count == 4)
				return websWrite(wp, "%s", wandns2);
			else if(count == 5)
				return websWrite(wp, "%s", "0.0.0.0");
		}
		else
	#endif
		{
		struct dns_lists *dns_list = NULL;
		int ret;
		
		//modify by junzhao 2004.3.10
		dns_list = get_dns_list(atoi(connection));
		if(dns_list != NULL)
		{
			if(strlen(dns_list->dns_server[count]) == 0)
				ret = websWrite(wp, "%s", "0.0.0.0");
				//return websWrite(wp, "%s", "0.0.0.0");
			else
				ret = websWrite(wp, "%s", dns_list->dns_server[count]);
			//junzhao 2004.10.11
			free(dns_list);
			return ret;
		}
		else
			return websWrite(wp, "%s", "0.0.0.0");
		}
	}
	
	return 0;
}

int ej_nvram_dslstatus_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	FILE *fp;

        ejArgs(argc, argv, "%s %d", &arg, &count);
	/*
        if (ejArgs(argc, argv, "%s %d", &arg, &count) < 2) 
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	*/
	
	if(!strcmp(arg, "dslstatus"))
	{
		char buf[16];
		fp = fopen("/proc/avalanche/avsar_modem_training", "r");
		if(fp == NULL)
		{
			if ( count != 10 )
				return websWrite(wp, "%s", "DOWN");
			else
				return websWrite(wp, "<script>Capture(share.down)</script>");
		}
		if(fread(buf, sizeof(char), sizeof(buf)*sizeof(char), fp) <= 0)
		{
			fclose(fp);
			if ( count != 10 )
				return websWrite(wp, "%s", "DOWN");
			else
				return websWrite(wp, "<script>Capture(share.down)</script>");
		}
		fclose(fp);
		if(!strncmp(buf, "SHOWTIME", strlen("SHOWTIME")))
			//return websWrite(wp, "%s", "Up");
			//2004.4.16
		{
			if(( count == 0) || ( count == 10 ))
			{
				if ( count != 10 )
					return websWrite(wp, "%s", "UP");
				else
					return websWrite(wp, "<script>Capture(share.up)</script>");
			}
			else if(count == 1)
				return websWrite(wp, "%s", "SYNC");
		}
		else
		{
			if(count == 1)
				return websWrite(wp, "%s", "NO SYNC");
			
			if(!strncmp(buf, "INIT", strlen("INIT")))
				return websWrite(wp, "%s", "TRAINING");
			else
                        {
                                if ( count != 10 )
                                        return websWrite(wp, "%s", "DOWN");
                                else
                                        return websWrite(wp, "<script>Capture(share.down)</script>");
                        }
		}
	}
	if(!strcmp(arg, "usrate"))
	{
		//2004.4.16
		int usrate = sar_getstats("US Connection Rate:");
		if(count == 0)
			return websWrite(wp, "%ld K", usrate);
		else if(count == 1)
		{
			if(usrate != 0)
				return websWrite(wp, "%ld", 1000 * usrate);
		}	
		else if(count == 2)
			return websWrite(wp, "%ld", usrate);
		//return websWrite(wp, "%d", sar_getstats("US Connection Rate:"));
	}
	if(!strcmp(arg, "usmargin"))
		return websWrite(wp, "%d", sar_getstats("US Margin:"));
	if(!strcmp(arg, "dsrate"))
	{
		//2004.4.16
		int dsrate = sar_getstats("DS Connection Rate:");
		if(count == 0)
			return websWrite(wp, "%ld K", dsrate);
		else if(count == 1)
		{
			if(dsrate != 0)
				return websWrite(wp, "%ld", 1000 * dsrate);
		}	
		else if(count == 2)
			return websWrite(wp, "%ld", dsrate);
		//return websWrite(wp, "%d", sar_getstats("DS Connection Rate:"));
	}
	if(!strcmp(arg, "dsmargin"))
		return websWrite(wp, "%d", sar_getstats("DS Margin:"));
	//junzhao 2004.7.28
	if(!strcmp(arg, "usatten"))
		return websWrite(wp, "%d", sar_getstats("US Line Attenuation:"));
	if(!strcmp(arg, "uspower"))
		return websWrite(wp, "%d", sar_getstats("US Transmit Power :"));
	if(!strcmp(arg, "dsatten"))
		return websWrite(wp, "%d", sar_getstats("DS Line Attenuation:"));
	if(!strcmp(arg, "dspower"))
		return websWrite(wp, "%d", sar_getstats("DS Transmit Power:"));
	
	if(!strcmp(arg, "trainedmode"))
	{
		int i = sar_getstats("Trained Mode:");
		if(i == 2)
			return websWrite(wp, "%s", "T1413");
		else if(i == 3)
			return websWrite(wp, "%s", "GDMT");
		else if(i == 4)
			return websWrite(wp, "%s", "GLITE");
		else if(i == 16) 
			
			return websWrite(wp, "%s", "ADSL2+");
		else
		{
			if ( count != 10 )
				return websWrite(wp, "%s", "NOT TRAINED");
			else
				return websWrite(wp, "<script>Capture(share.nottrained)</script>");
		}
	}	
	if(!strcmp(arg, "trainedpath"))
	{
		if ( count != 10 )
			return websWrite(wp, "%s", (sar_getstats("Trained Path:"))?"INTERLEAVED":"FAST");
		else
			return websWrite(wp, "%s", (sar_getstats("Trained Path:"))?"<script>Capture(share.interleaved)</script>":"<script>Capture(share.fast)</script>");
	}
	if(!strcmp(arg, "CRC:") || !strcmp(arg, "FEC:") || !strcmp(arg, "NCD:") || !strcmp(arg, "LCD:") || !strcmp(arg, "HEC:"))
	{
		//printf("%d %d %d %d\n", sar_mul_getstats(arg, 0, 0), sar_mul_getstats(arg, 0, 1), sar_mul_getstats(arg, 1, 0), sar_mul_getstats(arg, 1, 1));
		if(count == 0)
			return websWrite(wp, "%d", sar_mul_getstats(arg, 0, 0));
		else if(count == 1)
			return websWrite(wp, "%d", sar_mul_getstats(arg, 0, 1));
		else if(count == 2)
			return websWrite(wp, "%d", sar_mul_getstats(arg, 1, 0));
		else if(count == 3)
			return websWrite(wp, "%d", sar_mul_getstats(arg, 1, 1));
	}
	if(!strcmp(arg, "oamstats"))
	{
		if(count == 0)
			return websWrite(wp, "%d", sar_getstats("Near End F4 Loop Back Count:"));
		else if(count == 1)
			return websWrite(wp, "%d", sar_getstats("Far End F4 Loop Back Count:"));
		else if(count == 2)
			return websWrite(wp, "%d", sar_getstats("Near End F5 Loop Back Count:"));
		else if(count == 3)
			return websWrite(wp, "%d", sar_getstats("Far End F5 Loop Back Count:"));
	}
	if(!strcmp(arg, "dslver"))
	{
		char result[32];
		memset(result, 0, sizeof(result));
		sar_get_ver("PDSP", result);
		return websWrite(wp, "%s", result);
	}	
	if(!strcmp(arg, "annex"))
	{
		char result[16];
		memset(result, 0, sizeof(result));
		sar_get_annex(result);
		return websWrite(wp, "%s", result);
	}	
	
	if(!strcmp(arg, "lasttime"))
	{
		time_t now = time(NULL);
		unsigned int uptime = strtoul(nvram_safe_get("adsluptime"), NULL, 0);
		if(uptime != 0 && uptime != ERANGE)
			return websWrite(wp, "%ld", now - uptime);
		else
			return websWrite(wp, "%ld", 0);
	}
	if(!strcmp(arg, "pvcstatus"))
	{
		char word[32], *status = NULL;
		int whichconn = atoi(connection), i;
		char readbuf[256], *tmpstr = readbuf;
		
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_PVC_STATUS, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_PVC_STATUS);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_pvc_status"), next)
		{
			if(whichconn-- == 0)
			{
				status = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			status = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		if ( count != 10 )
			return websWrite(wp, "%s", status);
		else
		{
			if(!strcmp(status, "Down"))
				return websWrite(wp, "<script>Capture(share.down)</script>");
			else if(!strcmp(status, "Applied"))
				return websWrite(wp, "<script>Capture(share.applied)</script>");
			else
				return websWrite(wp, "%s", status);
		}
	}

	if(!strcmp(arg, "pvcerrstr"))
	{
		char word[64], *errstr = NULL;
		int whichconn = atoi(connection), i;
		char readbuf[256], *tmpstr = readbuf;
		
		memset(word, 0, sizeof(word));
		if(!file_to_buf(WAN_PVC_ERRSTR, readbuf, sizeof(readbuf)))
	       	{
			printf("no buf in %s!\n", WAN_PVC_ERRSTR);
	               	 return 0;
	        }
		/*
		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_pvc_errstr"), next)
		{
			if(whichconn-- == 0)
			{
				errstr = word;
				break;
			}
		}
		*/

		//junzhao 2004.8.4
		for(i=0; i<PVC_CONNECTIONS; i++)
		{
			errstr = strsep(&tmpstr, " ");
			if(i == whichconn)
				break;
		}
		if(strcmp(errstr, "Down"))
		{
			//add by junzhao 2004.3.12
			if(!replace('_', ' ', errstr))
				return websWrite(wp, " -- %s", errstr);
		}
	}

	if(!strcmp(arg, "encapmode"))
	{
		if(!strcmp(encapmode, "pppoe"))
			return websWrite(wp, "RFC 2516 PPPoE");
		else if(!strcmp(encapmode, "pppoa"))
			return websWrite(wp, "RFC 2364 PPPoA");
		else if(!strcmp(encapmode, "bridgedonly"))
                {
                        if ( count != 10 )
				return websWrite(wp, "Bridged Mode Only");
                        else
                                return websWrite(wp, "<script>Capture(share.bridgeonly)</script>");
                }
		else if(!strcmp(encapmode, "1483bridged"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "RFC 1483 Bridged");
                        else
                                return websWrite(wp, "RFC<script>Capture(share.bridged)</script>");
                }
		else if(!strcmp(encapmode, "routed"))
                {       
                        if ( count != 10 )
                                return websWrite(wp, "RFC 1483 Routed");
                        else
                                return websWrite(wp, "RFC<script>Capture(share.routed)</script>");
                }
	#ifdef CLIP_SUPPORT
		else if(!strcmp(encapmode, "clip"))
			return websWrite(wp, "IPoA");	
	#endif
		//return websWrite(wp, "%s", encapmode);
	}
	if(!strcmp(arg, "multiplex"))
	{
		if(!strcmp(multiplex, "llc"))
			return websWrite(wp, "LLC");	
		else if(!strcmp(multiplex, "vc"))
			return websWrite(wp, "VC");			
		//return websWrite(wp, "%s", multiplex);
	}
	if(!strcmp(arg, "qos"))
	{
		if(!strcmp(qos, "ubr"))
			return websWrite(wp, "UBR");	
		else if(!strcmp(qos, "cbr"))
			return websWrite(wp, "CBR");			
		else if(!strcmp(qos, "vbr"))
			return websWrite(wp, "VBR");			
		//return websWrite(wp, "%s", qos);
	}
	if(!strcmp(arg, "pcr"))
	{
		if(strcmp(qos, "ubr"))
			return websWrite(wp, "%s", pcr);
		else						                                   	return websWrite(wp, "%s", "0");		
	}
	if(!strcmp(arg, "scr"))
	{
		if(!strcmp(qos, "vbr"))
			return websWrite(wp, "%s", scr);
		else						                                   	return websWrite(wp, "%s", "0");		
	}
	if(!strcmp(arg, "autodetect"))
	{
		if(!strcmp(autodetect, "1"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "%s", "Enable");
                        else
                                return websWrite(wp, "<script>Capture(share.enable)</script>");
                }
		else if(!strcmp(autodetect, "0"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "%s", "Disable");
                        else
                                return websWrite(wp, "<script>Capture(share.disable)</script>");
                }
	}
	//add by junzhao 2004.3.15
	if(!strcmp(arg, "applyonboot"))
	{
		if(!strcmp(applyonboot, "1"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "%s", "Yes");
                        else
                                return websWrite(wp, "<script>Capture(share.yes)</script>");
                }
		else if(!strcmp(applyonboot, "0"))
                {
                        if ( count != 10 )
                                return websWrite(wp, "%s", "No");
                        else
                                return websWrite(wp, "<script>Capture(share.no)</script>");
                }
	}
	
	if(!strcmp(arg, "vpi"))
		return websWrite(wp, "%s", vpi);
	if(!strcmp(arg, "vci"))
		return websWrite(wp, "%s", vci);
	return -1;
}
			
int
ej_nvram_status_get(int eid, webs_t wp, int argc, char_t **argv)
{
        char *type;
	char *wan_ipaddr, *wan_netmask, *wan_gateway;
	char *status1="", *status2="", *hidden1, *hidden2, *button1="";
	char *wan_proto = nvram_safe_get("wan_proto");
	struct dns_lists *dns_list = NULL;
	int wan_link = check_wan_link(0);

        if (ejArgs(argc, argv, "%s", &type) < 1) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	if(nvram_match("wan_proto","pptp")){
		wan_ipaddr = wan_link ? nvram_safe_get("pptp_get_ip") : nvram_safe_get("wan_ipaddr");
		wan_netmask = wan_link ? nvram_safe_get("wan_netmask") : nvram_safe_get("wan_netmask");
		wan_gateway = wan_link ? nvram_safe_get("wan_gateway") : nvram_safe_get("pptp_server_ip");
	}
	else if(!strcmp(nvram_safe_get("wan_proto"),"pppoe")){
		wan_ipaddr = wan_link ? nvram_safe_get("wan_ipaddr") : "0.0.0.0";
		wan_netmask = wan_link ? nvram_safe_get("wan_netmask") : "0.0.0.0";
		wan_gateway = wan_link ? nvram_safe_get("wan_gateway") : "0.0.0.0";
	}
	else{
		wan_ipaddr = nvram_safe_get("wan_ipaddr");
		wan_gateway = nvram_safe_get("wan_gateway");
		wan_netmask = nvram_safe_get("wan_netmask");
	}

	//dns_list = get_dns_list();

	if(!strcmp(wan_proto, "pppoe") || !strcmp(wan_proto, "pptp") || !strcmp(wan_proto, "heartbeat")){
	    hidden1 = "";
	    hidden2 = "";
	    if (wan_link == 0){
		// submit_button old format is "Connect", new format is "Connect_pppoe" or "Connect_pptp" or "Connect_heartbeat"
		//if(submit_type && !strncmp(submit_type,"Connect",7) && retry_count != -1){
		if(retry_count != -1){
			status1 = "Status";
			status2 = "Connecting";
			button1 = "Disconnect";
		}
		else{
			status1 = "Status";
			status2 = "Disconnected";
			button1 = "Connect";
		}
	    }
	    else {
		retry_count = -1;
		status1 ="Status";
		status2 ="Connected";
		button1 = "Disconnect";
	    }
	}
	else{
	    status1 ="Disable";	// only for nonbrand
	    status2 ="&nbsp;";
	    hidden1 = "<!--";
	    hidden2 = "-->";
	}
	

	if(!strcmp(type,"wan_ipaddr"))
		return websWrite(wp,"%s",wan_ipaddr);
	else if(!strcmp(type,"wan_netmask"))
		return websWrite(wp,"%s",wan_netmask);
	else if(!strcmp(type,"wan_gateway"))
		return websWrite(wp,"%s",wan_gateway);
	else if(!strcmp(type,"wan_dns0"))
		return websWrite(wp,"%s",dns_list->dns_server[0]);
	else if(!strcmp(type,"wan_dns1"))
		return websWrite(wp,"%s",dns_list->dns_server[1]);
	else if(!strcmp(type,"wan_dns2"))
		return websWrite(wp,"%s",dns_list->dns_server[2]);
	else if(!strcmp(type,"status1"))
		return websWrite(wp,"%s",status1);
	else if(!strcmp(type,"status2"))
		return websWrite(wp,"%s",status2);
	else if(!strcmp(type,"button1"))
		return websWrite(wp,"%s",button1);
	else if(!strcmp(type,"hidden1"))
		return websWrite(wp,"%s",hidden1);
	else if(!strcmp(type,"hidden2"))
		return websWrite(wp,"%s",hidden2);

	if(dns_list)	free(dns_list);

	return 1;	
}

int
ej_show_status(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	int ret = 0;
        char *wan_proto = nvram_safe_get("wan_proto");
	char *submit_type = websGetVar(wp, "submit_type", NULL);
	int wan_link = 0;
	char buf[254];
	
        if(!strcmp(wan_proto,"pppoe") || !strcmp(wan_proto,"pptp") || !strcmp(wan_proto,"heartbeat")){

	     /* get type  [ refresh | reload ]*/
             if (ejArgs(argc, argv, "%s", &type) < 1) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
             }
	         /* get ppp status , if /tmp/ppp/link exist, the connection is enabled */
		 wan_link = check_wan_link(0);
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"before gozila_action=[%d] retry_count=[%d] submit_type=[%s] wan_link=[%d]",gozila_action,retry_count,submit_type,wan_link);
#endif


	     if(!strcmp(type,"init")){

	         /* press [ Connect | Disconnect ] button */
	         /* set retry count */
	         if(gozila_action)     
		    retry_count = STATUS_RETRY_COUNT;	// retry 3 times

	         /* set refresh time */
		// submit_type old format is "Disconnect", new format is "Disconnect_pppoe" or "Disconnect_pptp" or "Disconnect_heartbeat"
		if(submit_type && !strncmp(submit_type, "Disconnect", 10))	// Disconnect always 60 seconds to refresh
			retry_count = -1;

	         refresh_time = (retry_count <= 0) ? STATUS_REFRESH_TIME2 : STATUS_REFRESH_TIME1;

	     }
	     else if(!strcmp(type,"refresh_time")){

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"refresh gozila_action=[%d] retry_count=[%d] refresh_time=[%d]",gozila_action,retry_count,refresh_time);
#endif
		ret += websWrite(wp,"%d",refresh_time*1000);
             }
	     else if(!strcmp(type,"onload")){
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"onload retry_count=[%d]",retry_count);
#endif
	           if(retry_count == 0 || (!submit_type && !wan_link && gozila_action)){    //After refresh 2 times, if the status is disconnect, show Alert message.
	               ret += websWrite(wp,"ShowAlert(\"TIMEOUT\");");
	               retry_count = -1;
	  	   }
		   else if(file_to_buf("/tmp/ppp/log", buf, sizeof(buf))){
			ret += websWrite(wp, "ShowAlert(\"%s\");", buf);
                        retry_count = -1;
			unlink("/tmp/ppp/log");
	           }
        	   else{
	               ret += websWrite(wp,"Refresh();");
		   }

	           if(retry_count != -1)
	              retry_count--;
	       }
       }
        return ret;
}

int
stop_ppp(webs_t wp)
{
	unlink("/tmp/ppp/log");
	return unlink("/tmp/ppp/link");
}

/* Return WAN link state */
/*
static int
ej_link(int eid, webs_t wp, int argc, char_t **argv)
{
        char *name;
        int s;
        struct ifreq ifr;
        struct ethtool_cmd ecmd;
        FILE *fp;

        if (ejArgs(argc, argv, "%s", &name) < 1) {
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }

        // PPPoE connection status 
        if (nvram_match("wan_proto", "pppoe")) {
                if ((fp = fopen("/tmp/ppp/link", "r"))) {
                        fclose(fp);
                       return websWrite(wp, "Connected");
                } else
                        return websWrite(wp, "Disconnected");
        }

        // Open socket to kernel 
        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                websError(wp, 400, strerror(errno));
                return -1;
        }

        // Check for non-zero link speed 
        strncpy(ifr.ifr_name, nvram_safe_get(name), IFNAMSIZ);
        ifr.ifr_data = (void *) &ecmd;
        ecmd.cmd = ETHTOOL_GSET;
        if (ioctl(s, SIOCETHTOOL, &ifr) < 0) {
                close(s);
                websError(wp, 400, strerror(errno));
                return -1;
	 }

        // Cleanup 
        close(s);

        if (ecmd.speed)
                return websWrite(wp, "Connected");
        else
                return websWrite(wp, "Disconnected");
}

*/
