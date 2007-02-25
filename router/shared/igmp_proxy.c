/*
 * IGMP Proxy Configuration
 * Author: Jackel
 * Date: 2004-09-21
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

void validate_igmp_proxy_config(webs_t wp, char *value, struct variable *v)
{
	char *channel, *enable;
	channel = websGetVar(wp, "igmp_proxy_channel", "0");
	enable = websGetVar(wp, "igmp_proxy_enable", "0");
	
	nvram_set("igmp_proxy_channel", channel);
	nvram_set("igmp_proxy_enable", enable);
	nvram_commit();
}

int ej_igmp_proxy_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	int ret = 0;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;	
	}

	if(!strcmp(arg, "igmp_proxy_enable"))
	{
		char *igmp_proxy_enable = nvram_safe_get("igmp_proxy_enable");
		
		if((count == 0) && (!strcmp(igmp_proxy_enable, "0")))
		{

			return websWrite(wp, "checked");
		}
		if((count == 1) && (!strcmp(igmp_proxy_enable, "1")))
		{

			return websWrite(wp, "checked");
		}
	}	
		return ret;
}

int write_connection_table(webs_t wp, int active_index)
{
	int i, selectable, ret = 0, which_to_select, has_match = 0, available = 0;
	char word[PVC_CONNECTIONS][6], readbuf[60];
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
	        return 0;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);

	for(i=0 ; i<PVC_CONNECTIONS; i++)
	{
		selectable = 0;
		which_to_select = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				//if(check_ppp_link(i))
				if(check_ppp_proc(i))
				{
					selectable = 1;
//					which_to_select = (1<<PPP_MASK) | i;   //ppp flag
					which_to_select = i;
				}
			}
			else if(strstr(word[i], "nas"))
			{
				selectable = 1;
				which_to_select = i;
			}
		#ifdef CLIP_SUPPORT
			else if(strstr(word[i], "atm"))
			{
				selectable = 1;
				which_to_select = i;
			}
		#endif
		}

		if(selectable)
		{
			available++;
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", which_to_select, (which_to_select == active_index)?"selected" : "", i+1, word[i]);
		}

		if(which_to_select == active_index)
			has_match = 1;
	}

#if defined(MPPPOE_SUPPORT)
   {
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	        return 0;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);

	for(i=0 ; i<PVC_CONNECTIONS; i++)
	{
		selectable = 0;
		which_to_select = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				//if(check_ppp_link(i + 8))
				if(check_ppp_proc(i + 8))
				{
					selectable = 1;
//					which_to_select = (1<<PPP_MASK) | (1<<MUL_MASK) | i;   //ppp flag
					which_to_select = i;
				}
			}
		}
		#if defined(IPPPOE_SUPPORT)
		else
		{
			char *next;
			int which = i;
			if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	       		 	return 0;
			}
			memset(word[i], 0, sizeof(word[i]));
			foreach(word[i], readbuf, next)
			{
				if(which -- == 0)
					break;
				memset(word[i], 0, sizeof(word[i]));
			}
			if(strstr(word[i], "ppp"))
			{
				//if(check_ppp_link(i + 8))
				if(check_ppp_proc(i + 8))
				{
					selectable = 1;
//					which_to_select = (1<<MUL_MASK) | i;   //ppp flag
					which_to_select = i;
				}
			}
		}
		#endif
		if(selectable)
		{
			available++;
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>%s %d(%s)</option>\n", which_to_select, (which_to_select == active_index)?"selected" : "", (which_to_select & (1<<PPP_MASK))?"MULTIPLE PPPOE":"IP PPPOE", i+1, word[i]);
		}
		if(which_to_select == active_index)
			has_match = 1;
	}
    }
#endif

	if(!has_match && available)
		ret += websWrite(wp,"\t\t<option value=\"32\" selected>Please Select:</option>\n");
	else if(!available)
		ret += websWrite(wp,"\t\t<option value=\"32\" selected>No Active Connection</option>\n");
	return ret;
}




int ej_igmp_proxy_channel(int eid, webs_t wp, int argc, char_t **argv)
{
        int last_select = 0;
        if(gozila_action)
                last_select = atoi(websGetVar(wp, "igmp_proxy_channel", "32"));
        else
                last_select = atoi(nvram_safe_get("igmp_proxy_channel"));
        return (write_connection_table(wp, last_select));
}




