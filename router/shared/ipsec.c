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
#include "ipsec.h"

char tunnel[3];
char lgw_conn[3];
int selchanged = 0;//conn change flag

char ipsec_status[20];
char entry_config_buf[100];
char rtype_config_buf[100];
char ltype_config_buf[100];
char sg_config_buf[100];
char keytype_config_buf[500];
char ipsecadv_config_buf[200];

char manualenc_hex[100];
char manualauth_hex[100];

int entry_config_inited;
int ipsec_local_inited;
int ipsec_remote_inited;
int ipsec_sg_inited;
/* zhangbin 2005.3.31 for remote id type in Cloud */
#ifdef CLOUD_SUPPORT
int ipsec_lid_inited;
int ipsec_rid_inited;
#endif
int ipsec_key_inited;
int ipsecadv_config_inited;

//status file name
#define WAN_IFNAME  "/tmp/status/wan_ifname"
#define WAN_IPADDR  "/tmp/status/wan_ipaddr"
#define PPP_FLAGS 0x80

char *local_type_l;
char *remote_type_l;
char *tunnel_st;

#undef ZB_DEBUG

extern int check_ppp_proc(int num);
extern int check_ppp_link(int num);
extern int killipsec(int bflag);

void StrToHex( char *source, char *target, int len )
{
	char ch[28], tmp1[3];
	int i,offset,i0;
	
	if( len % 2)
	{
		offset = 1;
		ch[0]='0';
	}
	else
		offset = 0;
	
	strcpy( (char *)&ch[offset], source );

	// transfer string to hex
	for( i=0 ; i < (len + offset); i+=2 )
	{
		strncpy( tmp1, &ch[i], 2 );
		tmp1[2]=0;
		sscanf( tmp1, "%2x", &i0 );
		(*target) = i0;
		target++;
	} 
}	


int
ej_tunnel_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type,*next,word[254];

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action)
	{
		char *tmptunnel = websGetVar(wp, "tunnel_entry", NULL);
		printf("tunnel %s\n", tunnel);
		printf("tmptunnel %s\n", tmptunnel);
		if((tmptunnel!=NULL) && (strcmp(tunnel, tmptunnel)))
		{
			printf("not the same tunnel\n");
			strncpy(tunnel, tmptunnel, sizeof(tunnel));
			selchanged = 1; //make the flag of change conn
			
			entry_config_inited = 0;
			ipsec_local_inited = 0;
			ipsec_remote_inited = 0;
			ipsec_sg_inited = 0;
			/* zhangbin 2005.3.31 for remote id type in Cloud */
			#ifdef CLOUD_SUPPORT
			ipsec_lid_inited = 0;
			ipsec_rid_inited = 0;
			#endif
			ipsec_key_inited = 0;
			ipsecadv_config_inited = 0;
			
		}
	}		
	else
	{
		strncpy(tunnel, nvram_safe_get("tunnel_entry"), sizeof(tunnel));
		entry_config_inited = 0;
		ipsec_local_inited = 0;
		ipsec_remote_inited = 0;
		ipsec_sg_inited = 0;
		/* zhangbin 2005.3.31 for remote id type in Cloud */
		#ifdef CLOUD_SUPPORT
		ipsec_lid_inited = 0;
		ipsec_rid_inited = 0;
		#endif
		ipsec_key_inited = 0;
		ipsecadv_config_inited = 0;
	}

	which = atoi(tunnel);

	if(!strcmp(type,"select"))
	{
			
		for(i=0 ; i < IPSEC_TUNNELS ; i++)
		{
			char buf[80]="";
			int which1;
			
			char *tunnel_status_local;
			char *tunnel_name_local;
			char *local_type_local;
			which1 = i;
			
			foreach(word, nvram_safe_get("entry_config"), next) 
			{
				if (which1-- == 0) 
				{
					tunnel_name_local = word;
					tunnel_status_local = strsep(&tunnel_name_local, ":");
			
					local_type_local = tunnel_name_local;
					tunnel_name_local = strsep(&local_type_local,":");
					//filter_name(tunnel_name_local,new_tunnel_name,sizeof(new_tunnel_name),GET)
					//snprintf(buf, sizeof(buf), "%s", new_tunnel_name);
					snprintf(buf, sizeof(buf), "%s", tunnel_name_local);
					//printf("tunnel_name_local=%s,buf=%s\n",tunnel_name_local,buf);
					break;

				}
			}
		
			printf("i=%d,which=%d,buf=%s\n",i,which,buf);	
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d (--%s)</option>\n",i,(i == which) ? "selected" : "",i+1,(!strcmp(buf,"(null)"))?"":buf);
		}
	}

	return ret;
}

#ifdef CLOUD_SUPPORT
int
ej_cloud_ipsec_init(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	char *t_entry_l;

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	t_entry_l = websGetVar(wp, "tunnel_entry", "0");
	strcpy(tunnel,t_entry_l);
	
	if(gozila_action)
	{
		entry_config_inited = 0;
		ipsec_local_inited = 0;
		ipsec_remote_inited = 0;
		ipsec_sg_inited = 0;
		ipsec_lid_inited = 0;
		ipsec_rid_inited = 0;
		ipsec_key_inited = 0;
		ipsecadv_config_inited = 0;
	}
	else
	{
		entry_config_inited = 0;
		ipsec_local_inited = 0;
		ipsec_remote_inited = 0;
		ipsec_sg_inited = 0;
		ipsec_lid_inited = 0;
		ipsec_rid_inited = 0;
		ipsec_key_inited = 0;
		ipsecadv_config_inited = 0;
	}
	return 0;
}
#endif

#if 0
int
ej_localgw_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type,*next,word[254];

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	
	
	if(!strcmp(type,"select"))
	{
		for(i=0 ; i < 8 ; i++)
		{
			int which1;
			char wan_ipaddress[20] = "",temp_string[80] = "";
			which1 = i;
			
			foreach(word, nvram_safe_get("wan_ipaddr"), next)
			{
				if (which1-- == 0)
				{
					strcpy(wan_ipaddress,word);
					if(strcmp(wan_ipaddress,"0.0.0.0"))
					{
						sprintf(temp_string,"<\% entry_config(\"local_gw\",%s);\%>",i);
						ret = websWrite(wp,"\t\t<option value=\"%d\" > %s</option>\n",i,wan_ipaddress);
					}
				}
			}
		}
	}

	return ret;
}

#endif
#if 0
int
ej_localgw_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which,select_which;
	char *type,*next,word[100],select_conn[5],readbuf[256];

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action)
	{
		char *tmpconn = websGetVar(wp, "localgw_conn", NULL);
		printf("lgw_conn %s\n", lgw_conn);
		printf("tmpconn %s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(lgw_conn, tmpconn)))
		{
			printf("not the same tunnel\n");
			strncpy(lgw_conn, tmpconn, sizeof(lgw_conn));
		}
	}		
	else
	{
		strncpy(lgw_conn, nvram_safe_get("localgw_conn"), sizeof(localgw_conn));
		
	}
	
	//which = atoi(lgw_conn);
	which = atoi(nvram_safe_get("wan_active_connection"));
	
	if(!strcmp(type,"select"))
	{
		if(which == 8)	
		{
			ret = websWrite(wp,"\t\t<option value=\"8\">No Wan Connection</option>\n");

		}
		else
//		for(i=0 ; i < 8 ; i++)
		{
			int which1;
			char wan_ifname[20] = "",wan_ipaddr[20] = "",temp_string[80] = "",ipbuf[256],pt[256];
			which1 = which;
			memset(wan_ifname,0,sizeof(wan_ifname));
			memset(wan_ipaddr,0,sizeof(wan_ipaddr));
			memset(ipbuf,0,sizeof(ipbuf));
			memset(word,0,sizeof(word));
			if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(pt)))
			{
				printf("no buf in %s!\n", WAN_IFNAME);
				return 0;
			}

			foreach(word, readbuf, next)
			{
				if (which1-- == 0)
				{
					strcpy(wan_ifname,word);
					//ret = websWrite(wp,"\t\t<option value=\"%d\"  %s>%d -- %s</option>\n",i,(which == i)?"selected":"",i+1 ,wan_ifname);
					break;
				}
			}
			which1 = which;
			if(!file_to_buf(WAN_IPADDR, ipbuf, sizeof(pt)))
			{
				printf("no buf in %s!\n", WAN_IPADDR);
				return 0;
			}

			foreach(word, ipbuf, next)
			{
				if (which1-- == 0)
				{
					strcpy(wan_ipaddr,word);
					//ret = websWrite(wp,"\t\t<option value=\"%d\"  %s>%d -- %s</option>\n",i,(which == i)?"selected":"",i+1 ,wan_ifname);
					break;
				}
			}
			
			ret = websWrite(wp,"\t\t<option value=\"%d\"  selected>conn:%d -- %s -- %s</option>\n",which,which+1 ,wan_ipaddr,wan_ifname);

		}
	}
	return ret;
}
#endif

int //zhangbin for multiple-pvc 2004.07.06
ej_localgw_select(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type,readbuf[256];

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action)
	{
		char *tmpconn = websGetVar(wp, "localgw_conn", NULL);
		printf("lgw_conn %s\n", lgw_conn);
		printf("tmpconn %s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(lgw_conn, tmpconn)))
		{
			printf("not the same lgw_conn\n");
			strncpy(lgw_conn, tmpconn, sizeof(lgw_conn));
		}
	}		
	else
	{
		strncpy(lgw_conn, nvram_safe_get("localgw_conn"), sizeof(localgw_conn));
		
	}
	
	//which = atoi(lgw_conn);
	which = atoi(nvram_safe_get("wan_active_connection"));
	printf("which=%d\n",which);
	
	if(!strcmp(type,"select"))
	{
		int selectable, ret = 0, which_to_select,wanconn_exist = 0;
		char wan_ifname[20],word[8][6],wan_ipaddr[8][20],ipbuf[256];
		
		if(which == 8)	
		{
			return websWrite(wp,"\t\t<option value=\"8\">No Wan Connection</option>\n");

		}
	
		memset(wan_ifname,0,sizeof(wan_ifname));
		memset(wan_ipaddr,0,sizeof(wan_ipaddr));
		memset(ipbuf,0,sizeof(ipbuf));
		memset(word,0,sizeof(word));
		
		if(!file_to_buf(WAN_IFNAME, readbuf, 256))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
			return 0;
		}
		sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);
		
		if(!file_to_buf(WAN_IPADDR, ipbuf, 256))
		{
			printf("no buf in %s!\n", WAN_IPADDR);
			return 0;
		}
		sscanf(ipbuf, "%s %s %s %s %s %s %s %s", wan_ipaddr[0], wan_ipaddr[1], wan_ipaddr[2], wan_ipaddr[3], wan_ipaddr[4], wan_ipaddr[5], wan_ipaddr[6], wan_ipaddr[7]);
		
		for(i=0 ; i < 8 ; i++)
		{
			selectable = 0;
			which_to_select = 0;
			
			if(strcmp(word[i], "Down"))
			{
				if(strstr(word[i], "ppp"))
				{
					//if(check_ppp_link(i) && check_ppp_proc(i))
					if( check_ppp_proc(i))
					{
						selectable = 1;
						which_to_select = i | PPP_FLAGS;   //ppp flag
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
				wanconn_exist = 1;
				if(gozila_action && !selchanged)
				{
					char *localgw_conn_l = websGetVar(wp,"localgw_conn",NULL);
					printf("now,lgw_conn=%s\n",localgw_conn_l);
					//ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s--%s)</option>\n", i, (atoi(websGetVar(wp,"localgw_conn",NULL)) == i)?"selected" : "", i, word[i],wan_ipaddr[i]);	
					ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", i, (atoi(websGetVar(wp,"localgw_conn",NULL)) == i)?"selected" : "", i+1, word[i]);	
				}
				else
				{
					char lgw_pvc[3];
					memset(lgw_pvc,0,sizeof(lgw_pvc));
					strcpy(lgw_pvc,localgw_conn);
					printf("localgw_conn=%s,lgw_pvc=%s\n",localgw_conn,lgw_pvc);
					//ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s--%s)</option>\n", i, (atoi(lgw_pvc) == i)?"selected" : "", i, word[i],wan_ipaddr[i]);	
					ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", i, (atoi(lgw_pvc) == i)?"selected" : "", i+1, word[i]);	
				}
			}
		}

		if(!wanconn_exist)
			ret += websWrite(wp,"\t\t<option value=\"8\">No WAN Connection</option>\n");

	}
	return ret;
}

//zhangbin for verifying the same tunnel names
int ej_name_array(int eid, webs_t wp, int argc, char_t **argv)
{	
	int tunnel_entry,i,ret = 0;

	if(gozila_action)
		tunnel_entry =  atoi(websGetVar(wp, "tunnel_entry", "0"));
	else
		tunnel_entry =  atoi(nvram_safe_get("tunnel_entry"));

	for(i = 0;i < 5;i++)
	{
		if(i != tunnel_entry)
		{
			int which = i;
			char word[180],*t_st,*t_name,*other,*next;
			foreach(word, nvram_safe_get("entry_config"), next) 
			{
				if (which-- == 0) 
				{
					t_name = word;
					t_st = strsep(&t_name, ":");
			
					other = t_name;
					t_name = strsep(&other,":");
					
					printf("t_st=%s,t_name=%s,other=%s\n",t_st,t_name,other);
					if(strcmp(t_name,""))
					{
						ret = websWrite(wp,"\t\tname[%d] = \"%s\";\n",i,t_name);
					}
					break;
				}
			}
		}
	}
			return ret;
}	

/* entry_config = tunnel_status:tunnel_name:local_type:local_gw:remote_type:sg_type:enc_type:auth_type:key_type */

void entry_config_init()
{
	char *next;
	int which;
#ifdef CLOUD_SUPPORT
	int tunnel_nr;
#endif
	
	memset(entry_config_buf, 0, sizeof(entry_config_buf));
	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");
		
	which = atoi(tunnel);
#ifdef CLOUD_SUPPORT
	tunnel_nr = atoi(tunnel);
	printf("%s:which=%d,tunnel_nr=%d\n",__FUNCTION__,which,tunnel_nr);	
#endif

	foreach(entry_config_buf, nvram_safe_get("entry_config"), next) 
	{
		if (which-- == 0) 
		{
			tunnel_name = entry_config_buf;
			tunnel_status = strsep(&tunnel_name, ":");
			
			local_type = tunnel_name;
			tunnel_name = strsep(&local_type,":");
			
			localgw_conn = local_type;
			local_type = strsep(&localgw_conn,":");
				
			
			remote_type = localgw_conn;
			localgw_conn = strsep(&remote_type,":");
			/*remote_type = local_type;
			local_type = strsep(&remote_type,":");*/
			
			sg_type = remote_type;
			remote_type = strsep(&sg_type,":");
			
			enc_type = sg_type;
			sg_type = strsep(&enc_type,":");
			
			auth_type = enc_type;
			enc_type = strsep(&auth_type,":");
			
			key_type = auth_type;
			auth_type = strsep(&key_type,":");

		#ifdef ZB_DEBUG
			printf("tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s\n,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s\n,key_type=%s\n", tunnel_status,tunnel_name,local_type,localgw_conn,remote_type,sg_type,enc_type,auth_type,key_type);
		#endif 
		printf("localgw_conn=%s\n",localgw_conn);
			break;
		}
	}

	/* zhangbin 2005.3.31 for local & remote id type in Cloud */
#ifdef CLOUD_SUPPORT
	{
		char ipsec_lid_cfg[12],tmp_lid[5][3];

		memset(ipsec_lid_cfg,0,sizeof(ipsec_lid_cfg));
		memset(tmp_lid,0,sizeof(tmp_lid));
		strcpy(ipsec_lid_cfg,nvram_safe_get("ipsec_lid_cfg"));
		#ifdef ZB_DEBUG
		printf("ipsec_lid_cfg=%s\n",ipsec_lid_cfg);
		#endif
		sscanf(ipsec_lid_cfg,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_lid[0],tmp_lid[1],tmp_lid[2],tmp_lid[3],tmp_lid[4]);
	
		#ifdef ZB_DEBUG
		{
			int k;
			for(k = 0;k < IPSEC_TUNNELS;k++)	
				printf("tmp_lid[%d]=%s\n",k,tmp_lid[k]);
		}
		#endif
		memset(ipsec_local_id,0,sizeof(ipsec_local_id));
		strcpy(ipsec_local_id,tmp_lid[tunnel_nr]);
	}

	{
		char ipsec_rid_cfg[12],tmp_rid[5][3];

		memset(ipsec_rid_cfg,0,sizeof(ipsec_rid_cfg));
		memset(tmp_rid,0,sizeof(tmp_rid));
		strcpy(ipsec_rid_cfg,nvram_safe_get("ipsec_rid_cfg"));
		#ifdef ZB_DEBUG
		printf("ipsec_rid_cfg=%s\n",ipsec_rid_cfg);
		#endif
		sscanf(ipsec_rid_cfg,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_rid[0],tmp_rid[1],tmp_rid[2],tmp_rid[3],tmp_rid[4]);
	
		#ifdef ZB_DEBUG
		{
			int k;
			for(k = 0;k < IPSEC_TUNNELS;k++)	
				printf("tmp_rid[%d]=%s\n",k,tmp_rid[k]);
		}
		#endif
		memset(ipsec_remote_id,0,sizeof(ipsec_remote_id));
		strcpy(ipsec_remote_id,tmp_rid[tunnel_nr]);
	}

#endif
	entry_config_inited = 1;
}	

int asciitoint(char str)
{
	int i = 0;
	//printf("str = %c\n", str);
	if((str >= '0') && (str <= '9'))
		i = str - '0';
	if((str >= 'a') && (str <= 'f'))
		i = str - 'a' + 10;
	if((str >= 'A') && (str <= 'F'))                                                        i = str - 'A' + 10;
	
	return i;
}


u_int32_t asciitohex(char *str)
{
	int  buf[8];
	int i = 0;
	u_int32_t hexvalue = 0;

	while(*str != '\0')
	{
        	buf[i] = asciitoint(*str ++);                                                   i++;
	}
	hexvalue = (buf[0] << 4 | buf[1]) << 24 |
		   (buf[2] << 4 | buf[3]) << 16 |
		   (buf[4] << 4 | buf[5]) << 8  |
		   (buf[6] << 4 | buf[7]) ;
	
	return hexvalue;
}

int match_ipsec_route(char *remote, char *ifname)
{
	FILE *fp;
	char buf[512];
	struct in_addr remote_group;
	char gate_addr[16], net_addr[16], mask_addr[16], iface[8];
       	int iflags, metric, refcnt, use, mss, window, irtt,find_it = 0;
	
	memset(&remote_group, 0, sizeof(remote_group));
	fp = fopen("/proc/net/route", "r");
	if(NULL == fp) 
	{
                printf("can not open the /proc/net/route file\n");
                return -1;
	}
	memset(buf, 0, sizeof(buf));
	while (fgets(buf, 512, fp))
	{
		sscanf(buf, "%s\t%s\t%s\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d", iface, net_addr, gate_addr,  &iflags, &refcnt, &use, &metric, mask_addr, &mss, &window, &irtt);
		if(!strcmp(iface, ifname))
		{
			printf("iface=%s,ifname=%s,iflags=%d\n",iface,ifname,iflags);
			if(!(iflags & 0x1))
			{
				printf("0x1!\n");
				continue;
			}
			printf("before remote_group!\n");
			remote_group.s_addr =asciitohex(net_addr); 
			//printf("remote_group=%s\n",remote_group);
			//for manual keying,remote must not be "any"
			if(!strcmp(remote,inet_ntoa(remote_group)))
			{
				find_it = 1;	
				break;
			}
		}
	}
	fclose(fp);
	if(find_it == 1)
		return 0;
	else 
		return -1;
}			
	
int ej_entry_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg,*sg_type_l;
	int count,which,temp;
	int ret=0;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(entry_config_inited == 0)
		entry_config_init();	
		
	which = atoi(tunnel);
	
	temp = which;

	if(!strcmp(arg,"dev_ip"))
	{
		char dev_ipaddr[20];
		strcpy(dev_ipaddr,nvram_safe_get("lan_ipaddr"));
		return websWrite(wp,"%d",get_single_ip(dev_ipaddr,count));
	}
	
	if(!strcmp(arg,"dev_netmask"))
	{
		char dev_netmask[20];
		strcpy(dev_netmask,nvram_safe_get("lan_netmask"));
		return websWrite(wp,"%d",get_single_ip(dev_netmask,count));
	}


	
	if(!strcmp(arg,"tunnel_status"))
	{
		if(gozila_action && !selchanged)
		{
			char *tunnel_status_l = websGetVar(wp,"tunnel_status",NULL);
					if(((count == ENABLE) && (!strcmp(tunnel_status_l, "1"))) || ((count == DISABLE) && (!strcmp(tunnel_status_l, "0"))))
				return websWrite(wp, "checked");
		}
		else
		{
		if(((count == ENABLE) && (!strcmp(tunnel_status, "1"))) || ((count == DISABLE) && (!strcmp(tunnel_status, "0"))))
			return ret=websWrite(wp, "checked");
		}
	}
	
	if(!strcmp(arg,"tunnel_name"))
	{
		if(gozila_action && !selchanged)
		{
			char *tunnel_name_l = websGetVar(wp,"tunnel_name","");
			return websWrite(wp, "%s",tunnel_name_l);
		}
		else
		{
			#ifdef ZB_DEBUG
				printf("tunnel_name=%s\n",tunnel_name);
			#endif

			return ret=websWrite(wp, "%s", (!strcmp(tunnel_name,"(null)"))?"":tunnel_name);
		}
		
	}
	
	if(!strcmp(arg, "local_type"))
	{
		if(gozila_action && !selchanged)
		{
			local_type_l = websGetVar(wp, "local_type", NULL);
			printf("local_type_l= %s\n", local_type_l);
			if(((count == 0) && (!strcmp(local_type_l, "0"))) || ((count == 1) && (!strcmp(local_type_l, "1")))) 
				return ret=websWrite(wp, "selected");
		}
		else
		{
			if(((count == 0) && (!strcmp(local_type, "0"))) || ((count == 1) && (!strcmp(local_type, "1"))))
			{ 
				printf("count=%d,local_type=%s\n",count,local_type);
				return ret=websWrite(wp, "selected");
			}
		}

	}

	if(!strcmp(arg,"localgw_conn"))
	{
		if(gozila_action && !selchanged)
		{
			char *localgw_conn_l = websGetVar(wp,"localgw_conn",NULL);
			if(((count == 1) && (!strcmp(localgw_conn_l, "1"))) || ((count == 0) && (!strcmp(localgw_conn_l, "0"))) || ((count == 2) && (!strcmp(localgw_conn_l,"2"))) || ((count == 3) && (!strcmp(localgw_conn_l,"3"))) || ((count == 4) && (!strcmp(localgw_conn_l,"4"))) || ((count == 5) && (!strcmp(localgw_conn_l,"5"))) || ((count == 6) && (!strcmp(localgw_conn_l,"6"))) || ((count == 7) && (!strcmp(localgw_conn_l,"7"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
		else{
			if(((count == 1) && (!strcmp(localgw_conn, "1"))) || ((count == 0) && (!strcmp(localgw_conn, "0"))) || ((count == 2) && (!strcmp(localgw_conn,"2"))) || ((count == 3) && (!strcmp(localgw_conn,"3"))) || ((count == 4) && (!strcmp(localgw_conn, "4"))) || ((count == 5) && (!strcmp(localgw_conn,"5"))) || ((count == 6) && (!strcmp(localgw_conn,"6"))) || ((count == 7) && (!strcmp(localgw_conn,"7"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
	}

	if(!strcmp(arg,"localgw_ip"))
	{
		char word[100],wan_ifname[20],wan_ip[20],readbuf[256];
		char *next;
		int which1 = count;
		int which2 = count;
	
		if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
		{
				printf("no buf in %s!\n", WAN_IFNAME);
				return 0;
		}

			//foreach(word, nvram_safe_get("wan_ifname"), next)
		foreach(word, readbuf, next)
		{
	//	foreach(word, nvram_safe_get("wan_ifname"), next)
	//	{
			if (which1-- == 0)
			{
				strcpy(wan_ifname,word);
			}
		}

		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
		{
				printf("no buf in %s!\n", WAN_IPADDR);
				return 0;
		}

		foreach(word, readbuf, next)
		//foreach(word, nvram_safe_get("wan_ipaddr"), next)
		{
			if (which2-- == 0)
			{
				strcpy(wan_ip,word);
			}
		}

		return websWrite(wp,"Connection:%d--IP:%s--%s",count+1,wan_ip,wan_ifname);
	}				

	if(!strcmp(arg, "remote_type"))
	{
		if(gozila_action && !selchanged)
		{
			remote_type_l = websGetVar(wp, "remote_type", NULL);
			if(((count == 0) && (!strcmp(remote_type_l, "0"))) || ((count == 1) && (!strcmp(remote_type_l, "1"))) || ((count == 2) && (!strcmp(remote_type_l, "2"))) || ((count == 3) && (!strcmp(remote_type_l, "3"))) || ((count == 4) && (!strcmp(remote_type_l, "4")))) 
				return ret=websWrite(wp, "selected");
		}
		else
		{
			if(((count == 0) && (!strcmp(remote_type, "0"))) || ((count == 1) && (!strcmp(remote_type, "1"))) || ((count == 2) && (!strcmp(remote_type, "2"))) || ((count == 3) && (!strcmp(remote_type, "3"))) || ((count == 4) && (!strcmp(remote_type, "4"))))
				return ret=websWrite(wp, "selected");
		}

	}

	if(!strcmp(arg, "sg_type"))
	{
		if(gozila_action && !selchanged)
		{
			sg_type_l = websGetVar(wp, "sg_type", NULL);
			if(((count == 0) && (!strcmp(sg_type_l, "0"))) || ((count == 1) && (!strcmp(sg_type_l, "1"))) || ((count == 2) && (!strcmp(sg_type_l, "2")))) 
				ret=websWrite(wp, "selected");
			return ret;
		}
		else
		{

			if(((count == 0) && (!strcmp(sg_type, "0"))) || ((count == 1) && (!strcmp(sg_type, "1"))) || ((count == 2) && (!strcmp(sg_type, "2")))) 
				ret= websWrite(wp, "selected");
			return ret;
		}

	}

#ifdef CLOUD_SUPPORT

	if(!strcmp(arg, "local_id"))
	{
		if(gozila_action && !selchanged)
		{
			char *local_id_l = websGetVar(wp, "local_id", NULL);
			printf("local_id_l=%s\n",local_id_l);
			if(((count == 0) && (!strcmp(local_id_l, "0"))) || ((count == 1) && (!strcmp(local_id_l, "1"))) || ((count == 2) && (!strcmp(local_id_l, "2")))) 
				ret=websWrite(wp, "selected");
			return ret;
		}
		else
		{

			printf("ipsec_local_id=%s\n",ipsec_local_id);
			if(((count == 0) && (!strcmp(ipsec_local_id, "0"))) || ((count == 1) && (!strcmp(ipsec_local_id, "1"))) || ((count == 2) && (!strcmp(ipsec_local_id, "2")))) 
				ret= websWrite(wp, "selected");
			return ret;
		}

	}

	if(!strcmp(arg, "remote_id"))
	{
		if(gozila_action && !selchanged)
		{
			char *remote_id_l = websGetVar(wp, "remote_id", NULL);
			printf("remote_id_l=%s\n",remote_id_l);
			if(((count == 0) && (!strcmp(remote_id_l, "0"))) || ((count == 1) && (!strcmp(remote_id_l, "1"))) || ((count == 2) && (!strcmp(remote_id_l, "2")))) 
				ret=websWrite(wp, "selected");
			return ret;
		}
		else
		{

			printf("ipsec_remote_id=%s\n",ipsec_remote_id);
			if(((count == 0) && (!strcmp(ipsec_remote_id, "0"))) || ((count == 1) && (!strcmp(ipsec_remote_id, "1"))) || ((count == 2) && (!strcmp(ipsec_remote_id, "2")))) 
				ret= websWrite(wp, "selected");
			return ret;
		}

	}
#endif

	if(!strcmp(arg,"enc_type"))
	{
		if(gozila_action && !selchanged)
		{
			char *enc_type_l = websGetVar(wp,"enc_type",NULL);
			if(((count == 1) && (!strcmp(enc_type_l, "1"))) || ((count == 0) && (!strcmp(enc_type_l, "0"))) || ((count == 2) && (!strcmp(enc_type_l,"2"))) || ((count == 3) && (!strcmp(enc_type_l,"3"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
		else{
			if(((count == 1) && (!strcmp(enc_type, "1"))) || ((count == 0) && (!strcmp(enc_type, "0"))) || ((count == 2) && (!strcmp(enc_type,"2"))) || ((count == 3) && (!strcmp(enc_type,"3"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
	}

	if(!strcmp(arg,"auth_type"))
	{
		if(gozila_action && !selchanged)
		{
			char *auth_type_l = websGetVar(wp,"auth_type",NULL);
			if(((count == 1) && (!strcmp(auth_type_l, "1"))) || ((count == 2) && (!strcmp(auth_type_l, "2"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
		else
		{
			if(((count == 1) && (!strcmp(auth_type, "1"))) || ((count == 2) && (!strcmp(auth_type, "2"))))
				ret=websWrite(wp, "selected");
			return ret;
		}
	}

	if(!strcmp(arg, "key_type"))
	{
		if(gozila_action && !selchanged)
		{
			char *key_type_l = websGetVar(wp, "key_type", NULL);
			if(((count == 0) && (!strcmp(key_type_l, "0"))) || ((count == 1) && (!strcmp(key_type_l, "1")))) 
				ret=websWrite(wp, "selected");
			return ret;
		}
		else
		{
			if(((count == 0) && (!strcmp(key_type, "0"))) || ((count == 1) && (!strcmp(key_type, "1")))) 
				ret=websWrite(wp, "selected");
			return ret;
		}

	}

	/* Autokey will display status,or else will not display 
	 * --- zhangbin 2005.7.1*/
	if(!strcmp(arg,"ipsec_tabst"))
	{
		if(!strcmp(key_type,"1"))
		{
			if(count == 0)
				return websWrite(wp,"%s","<!--");
			else if(count == 1)
				return websWrite(wp,"%s","-->");
		}
		else if(!strcmp(key_type,"0"))
			return websWrite(wp,"%s","");
	}

	if(!strcmp(arg,"tstatus"))
	{
		FILE *file;
		char buf[5];
		char state_file[50];


		if(!strcmp(key_type,"1"))
		{
#if 0
			printf("before match_ipsec_route!\n");
			if(match_ipsec_route(r_ipaddr,"ipsec0") == 0)
			{
			printf("strcpy Connected!\n");
				strcpy(ipsec_status,"Connected");
			}
			else
				strcpy(ipsec_status,"Disconnected");

		printf("will return ipsec_status!\n");
			
			return websWrite(wp, "%s", ipsec_status);
#endif
			/* Don't display in Manual key
			 * -----zhangbin  2005.7.1 */
			//return websWrite(wp, "%s","Disconnected");
			return websWrite(wp, "%s","");
		}

		sprintf(state_file,"/tmp/ipsecst/%s",tunnel_name);
	
		if((file = fopen(state_file,"r")) == NULL)
		{
			printf("cannot open %s!\n",state_file);
			strcpy(ipsec_status,"Disconnected");
			if ( count != 10)
				return ret=websWrite(wp, "%s", ipsec_status);
			else
				return ret=websWrite(wp, "<script>Capture(srccode.disconnected)</script>");
		}
		fread(buf,1,1,file) ;
		buf[1] = '\0';
		printf("buf=%s!\n",buf);
		fclose(file);

		if(!strcmp(buf,"1"))
			strcpy(ipsec_status,"Connected");
		else
			strcpy(ipsec_status,"Disconnected");

		printf("in buf:ipsec_status=%s\n",ipsec_status);
		if ( count != 10)
			return ret=websWrite(wp, "%s", ipsec_status);
		else
		{
			if (!strcmp(buf,"1"))
				return ret=websWrite(wp, "<script>Capture(srccode.connected)</script>");
			else
				return ret=websWrite(wp, "<script>Capture(srccode.disconnected)</script>");
		}
	}

	if(!strcmp(arg,"click"))
	{
		char connect_flag[3];
		printf("in click:ipsec_status=%s\n",ipsec_status);
		if(!strcmp(ipsec_status,"Connected"))
			strcpy(connect_flag,"0");
		else
			strcpy(connect_flag,"1");
		printf("connect_flage=%s\n",connect_flag);
		return ret=websWrite(wp, "%s", connect_flag);
	}

	if(!strcmp(arg,"connect"))
	{
		char connect_button[20];
		printf("in connect:ipsec_status=%s\n",ipsec_status);
		if(!strcmp(ipsec_status,"Connected"))
			strcpy(connect_button,"Disconnect");
		else
			strcpy(connect_button,"Connect");
		return ret=websWrite(wp, "%s", connect_button);
	}

	
	if(!strcmp(arg,"wanip"))
	{
		char *next,word[160],wan_ip[20],readbuf[256];
		int which,ret = 0;
		which = atoi(localgw_conn);
		printf("which=%d\n",which);
		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
		{
				printf("no buf in %s!\n", WAN_IPADDR);
				return 0;
		}

		foreach(word, readbuf, next)

//foreach(word,nvram_safe_get("wan_ipaddr"),next)
		{
			if(which-- == 0)
			{
				strcpy(wan_ip,word);
				ret = websWrite(wp,"%s",wan_ip);

				break;
			}
		}
		return 	ret;
	}
		
		
	return -1;
}

/*local_type=l_ipaddr:l_netmask*/

void ltype_config_init()
{
	char *next;
	int which;

	memset(ltype_config_buf, 0, sizeof(ltype_config_buf));
	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");
	which = atoi(tunnel);

	foreach(ltype_config_buf, nvram_safe_get("ltype_config"), next) 
	{
		if (which-- == 0) 
		{
			l_netmask = ltype_config_buf;				
			l_ipaddr = strsep(&l_netmask, ":");


#ifdef ZB_DEBUG
			printf("local l_ipaddr=%s, l_netmask=%s\n", l_ipaddr, l_netmask);
#endif 
			break;
		}
	}
	ipsec_local_inited = 1;
}

int ej_ltype_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_local_inited == 0)
		ltype_config_init();

	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action && !selchanged )
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"l_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"l_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"l_ipaddr2",""));
				case 3:
					/* zhangbin 2005.3.17 */
					#if 0
					if(!strcmp(local_type_l,"1"))
					{
						printf("l_ipaddr3 will be 0!\n");
						return 	websWrite(wp,"%s","0");
					}
					#endif
					return 	websWrite(wp,"%s",websGetVar(wp,"l_ipaddr3",""));
			}
		}
		else
			return websWrite(wp, "%d", get_single_ip(l_ipaddr,count));
	}

	if(!strcmp(arg, "netmask"))
	{
		if(gozila_action && !selchanged )
		{
			char *l_netmask0 = websGetVar(wp,"l_netmask0","");
			if(!strcmp(l_netmask0,""))
			{
				char *lan_netmask_l = nvram_safe_get("lan_netmask");
				printf("%s,l_netmask0=%s\n",__FUNCTION__,l_netmask0);
				return websWrite(wp,"%d",get_single_ip(lan_netmask_l,count));
			}
			else
			{
				switch(count)
				{
					case 0:
						return 	websWrite(wp,"%s",websGetVar(wp,"l_netmask0",""));
					case 1:
						return 	websWrite(wp,"%s",websGetVar(wp,"l_netmask1",""));
					case 2:
						return 	websWrite(wp,"%s",websGetVar(wp,"l_netmask2",""));
					case 3:
						return 	websWrite(wp,"%s",websGetVar(wp,"l_netmask3",""));

				}
			}
#if 0

			if(!strcmp(local_type_l,"1"))
			{
				/* zhangbin 2005.3.17 */
				//char *l_netmask_net = "255.255.255.0";
				char *l_netmask_net = nvram_safe_get("lan_netmask");
				printf("l_netmask_net=%s\n",l_netmask_net);
				return websWrite(wp, "%d",get_single_ip(l_netmask_net,count));
			}
			else
			{
			char *l_netmask_l = malloc(20);
			char temp[30],*val = NULL;
			int i;
			memset(l_netmask_l, 0, sizeof(l_netmask_l));
			memset(temp, 0, sizeof(temp));

			for(i=0 ; i<4 ; i++)
			{
				snprintf(temp, sizeof(temp), "%s%d", "l_netmask", i);
				val = websGetVar(wp, temp , NULL);
				if(val)
				{
					strcat(l_netmask_l, val);
					if(i<3) 
						strcat(l_netmask_l, ".");
				}
				else
				{	
					free(l_netmask_l);
					return -1;
				}
			}
			return websWrite(wp, "%d", get_single_ip(l_netmask_l, count));
			}
#endif
		}
		else
		{
			return websWrite(wp, "%d", get_single_ip(l_netmask, count));
		}
	}
	return -1;
}


/*remote_type=r_ipaddr:r_netmask*/

void rtype_config_init()
{
	char *next;
	int which;

	memset(rtype_config_buf, 0, sizeof(rtype_config_buf));

	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");

	which = atoi(tunnel);

	foreach(rtype_config_buf, nvram_safe_get("rtype_config"), next) 
	{
		if (which-- == 0) 
		{
			r_netmask = rtype_config_buf;				
			r_ipaddr = strsep(&r_netmask, ":");

#ifdef ZB_DEBUG
			printf("r_ipaddr=%s, r_netmask=%s\n", r_ipaddr, r_netmask);
#endif 
			break;
		}
	}
	ipsec_remote_inited = 1;
}

int ej_rtype_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_remote_inited == 0)
		rtype_config_init();

	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action && !selchanged)
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"r_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"r_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"r_ipaddr2",""));
				case 3:
					/* zhangbin 2005.3.17 */
					#if 0
					if(!strcmp(remote_type_l,"1"))
						return 	websWrite(wp,"%s","0");
					#endif
					return 	websWrite(wp,"%s",websGetVar(wp,"r_ipaddr3",""));
			}
		}
		else
		{	
			if(r_ipaddr == "")
				return websWrite(wp, "%d", get_single_ip("0.0.0.1",count));
			return websWrite(wp, "%d", get_single_ip(r_ipaddr,count));
		}
	}

	if(!strcmp(arg, "netmask"))
	{
		if(gozila_action && !selchanged)
		{
			/* zhangbin 2005.3.17 */
			char *r_netmask0 = websGetVar(wp,"r_netmask0","");
			if(!strcmp(r_netmask0,""))
			{
				return websWrite(wp,"%d",get_single_ip("255.255.255.0",count));
			}
			else
			{
				switch(count)
				{
					case 0:
						return 	websWrite(wp,"%s",websGetVar(wp,"r_netmask0",""));
					case 1:
						return 	websWrite(wp,"%s",websGetVar(wp,"r_netmask1",""));
					case 2:
						return 	websWrite(wp,"%s",websGetVar(wp,"r_netmask2",""));
					case 3:
						return 	websWrite(wp,"%s",websGetVar(wp,"r_netmask3",""));

				}
			}

#if 0
			if(!strcmp(remote_type_l,"1"))
			{
				return websWrite(wp, "%d", get_single_ip("255.255.255.0", count));
			}
			else
			{
			char *r_netmask_l = malloc(20);
			char temp[30],*val = NULL;
			int i;
			memset(r_netmask_l, 0, sizeof(r_netmask_l));
			memset(temp, 0, sizeof(temp));

			for(i=0 ; i<4 ; i++)
			{
				snprintf(temp, sizeof(temp), "%s%d", "r_netmask", i);
				val = websGetVar(wp, temp , NULL);
				if(val)
				{
					strcat(r_netmask_l, val);
					if(i<3) 
						strcat(r_netmask_l, ".");
				}
				else
				{	
					free(r_netmask_l);
					return -1;
				}
			}
			return websWrite(wp, "%d", get_single_ip(r_netmask_l, count));
			}
#endif
		}
		else
		{	
			if(r_netmask == "")
				return websWrite(wp,"%d",get_single_ip("255.255.255.0",count));
			return websWrite(wp, "%d", get_single_ip(r_netmask, count));
		}
	}

	return -1;
}

/* sg_config=sg_ipaddr:sg_domain */

void sg_config_init()
{
	char *next;
	int which;

	memset(sg_config_buf, 0, sizeof(sg_config_buf));

	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");

	which = atoi(tunnel);

	foreach(sg_config_buf, nvram_safe_get("sg_config"), next) 
	{
		if (which-- == 0) 
		{
			sg_domain = sg_config_buf;
			sg_ipaddr = strsep(&sg_domain,":");

#ifdef ZB_DEBUG
			printf("sg_ipaddr=%s,sg_domain=%s\n",sg_ipaddr,sg_domain);
#endif 
			break;
		}
	}
	ipsec_sg_inited = 1;
}

int ej_sg_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_sg_inited == 0)
		sg_config_init();

	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action && !selchanged)
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"sg_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"sg_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"sg_ipaddr2",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"sg_ipaddr3",""));
			}

		}
		else
		{	
			return websWrite(wp, "%d", get_single_ip(sg_ipaddr,count));
		}
	}

	if(!strcmp(arg,"sg_domain"))
	{
		if(gozila_action && !selchanged)
		{
			char *sg_domain_l = websGetVar(wp,"sg_domain","");
			return websWrite(wp,"%s",sg_domain_l);		
		}
		else
		{
			printf("sg_domain=%s\n",sg_domain);
			return websWrite(wp, "%s", sg_domain);
		}
	}
	return -1;
}

#ifdef CLOUD_SUPPORT

void lid_config_init()
{
	int cnt,which;
	char lid_cfg[300],*temp_cfg = lid_cfg;
	char *lid_tmp;
	
	which = atoi(tunnel);

	memset(lid_cfg,0,sizeof(lid_cfg));
	strcpy(lid_cfg, nvram_safe_get("lid_config"));

	for(cnt = 0; cnt < IPSEC_TUNNELS; cnt++)
	{
		lid_tmp = strsep(&temp_cfg, " ");
		if(cnt == which)
		{
			sscanf(lid_tmp, "%18[^:]:%60[^\n]", lid_ip, lid_fqdn);
			if(!strcmp(lid_fqdn,"0"))
				strcpy(lid_fqdn,"");
			printf("%s:%d,lid_ip=%s,lid_fqdn=%s\n",__FUNCTION__,which,lid_ip,lid_fqdn);
			break;
		}
	}
		
	/* zhangbin 2005.3.31 for remote id type in Cloud */
	ipsec_lid_inited = 1;

}

int ej_lid_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_lid_inited == 0)
		lid_config_init();

	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action && !selchanged)
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"lid_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"lid_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"lid_ipaddr2",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"lid_ipaddr3",""));
			}

		}
		else
		{	
			return websWrite(wp, "%d", get_single_ip(lid_ip,count));
		}

	}

	if(!strcmp(arg,"lid_fqdn"))
	{
		if(gozila_action && !selchanged)
		{
			char *lid_fqdn_l = websGetVar(wp,"lid_fqdn","");
			return websWrite(wp,"%s",lid_fqdn_l);		
		}
		else
		{
			printf("lid_fqdn=%s\n",lid_fqdn);
			return websWrite(wp, "%s", lid_fqdn);
		}
	}
	return -1;

}


void rid_config_init()
{
	int cnt,which;
	char rid_cfg[300],*temp_cfg = rid_cfg;
	char *rid_tmp;
	
	which = atoi(tunnel);

	memset(rid_cfg,0,sizeof(rid_cfg));
	strcpy(rid_cfg, nvram_safe_get("rid_config"));

	for(cnt = 0; cnt < IPSEC_TUNNELS; cnt++)
	{
		rid_tmp = strsep(&temp_cfg, " ");
		if(cnt == which)
		{
			sscanf(rid_tmp, "%18[^:]:%60[^\n]", rid_ip, rid_fqdn);
			if(!strcmp(rid_fqdn,"0"))
				strcpy(rid_fqdn,"");
			printf("%s:%d,rid_ip=%s,rid_fqdn=%s\n",__FUNCTION__,which,rid_ip,rid_fqdn);
			break;
		}
	}
		
	/* zhangbin 2005.3.31 for remote id type in Cloud */
	ipsec_rid_inited = 1;

}

int ej_rid_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_rid_inited == 0)
		rid_config_init();

	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action && !selchanged)
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"rid_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"rid_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"rid_ipaddr2",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"rid_ipaddr3",""));
			}

		}
		else
		{	
			return websWrite(wp, "%d", get_single_ip(rid_ip,count));
		}

	}

	if(!strcmp(arg,"rid_fqdn"))
	{
		if(gozila_action && !selchanged)
		{
			char *rid_fqdn_l = websGetVar(wp,"rid_fqdn","");
			return websWrite(wp,"%s",rid_fqdn_l);		
		}
		else
		{
			printf("rid_fqdn=%s\n",rid_fqdn);
			return websWrite(wp, "%s", rid_fqdn);
		}
	}
	return -1;

}
#endif
/* keytype= auto_pfs:auto_presh:auto_klife:manual_enckey:manual_authkey:manual_ispi:manual_ospi */

void keytype_config_init()
{
	char *next;
	int which;

	memset(keytype_config_buf, 0, sizeof(keytype_config_buf));

	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");

	which = atoi(tunnel);

	foreach(keytype_config_buf, nvram_safe_get("keytype_config"), next) 
	{
		if (which-- == 0) 
		{
#if 0
			auto_presh = keytype_config_buf;
			auto_pfs = strsep(&auto_presh, ":");
			auto_klife = auto_presh;				
			auto_presh = strsep(&auto_klife, ":");
			manual_enckey= auto_klife;
			auto_klife = strsep(&manual_enckey, ":");
			manual_authkey= manual_enckey;
			manual_enckey = strsep(&manual_authkey, ":");
			manual_ispi= manual_authkey;
			manual_authkey = strsep(&manual_ispi, ":");
			manual_ospi = manual_ispi;
			manual_ispi = strsep(&manual_ospi, ":");
#endif
			sscanf(keytype_config_buf, "%3[^:]:%100[^:]:%10[^:]:%100[^:]:%80[^:]:%10[^:]:%10[^\n]", auto_pfs,auto_presh,auto_klife,manual_enckey,manual_authkey,manual_ispi,manual_ospi);

#ifdef ZB_DEBUG
			printf("in keytype_init:auto_pfs=%s,auto_presh=%s,auto_klife=%s\n,manual_enckey=%s\n,manual_authkey=%s\n,manual_ispi=%s,manual_ospi=%s\n", auto_pfs,auto_presh,auto_klife,manual_enckey,manual_authkey,manual_ispi,manual_ospi);
#endif 
			break;
		}
	}
	ipsec_key_inited = 1;
}

int ej_keytype_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(ipsec_key_inited == 0)
		keytype_config_init();

	if(!strcmp(arg,"auto_pfs"))
	{
		/*if(gozila_action && !selchanged)
		{
			char *auto_pfs_l = websGetVar(wp,"auto_pfs",NULL);
			#ifdef ZB_DEBUG
				printf("auto_pfs_l=%s\n",auto_pfs_l);
			#endif
			if(((count == ENABLE) && (!strcmp(auto_pfs_l, "1"))) || ((count == DISABLE) && (!strcmp(auto_pfs_l, "0"))))
				return websWrite(wp, "checked");
		}
		else
		{*/
			if(((count == ENABLE) && (!strcmp(auto_pfs, "1"))) || ((count == DISABLE) && (!strcmp(auto_pfs, "0"))))
				return websWrite(wp, "checked");
	//	}
	}

	if(!strcmp(arg, "auto_presh"))
	{
	/*	if(gozila_action && !selchanged)
		{
			char *auto_presh_l = websGetVar(wp,"auto_presh","");
			return websWrite(wp, "%s",auto_presh_l);
		}
		else
		{*/
	/*if(auto_presh == NULL)	
		return websWrite(wp,"%s","");
	else*/
		char new_presh[100];
		filter_name(auto_presh,new_presh,sizeof(new_presh),GET);
#ifdef ZB_DEBUG
		printf("auto_presh=%s,new_presh=%s\n",auto_presh,new_presh);
#endif
		return websWrite(wp, "%s",(!strcmp(new_presh,"null"))?"":new_presh);
	//	}

	}

	if(!strcmp(arg,"auto_klife"))
	{
	/*	if(gozila_action && !selchanged)
		{
			char *auto_klife_l = websGetVar(wp,"auto_klife","");
			return websWrite(wp, "%s",auto_klife_l);
		}
		else
		{*/
	/*if(auto_klife == NULL)	
		return websWrite(wp,"%s","");
	else
	{*/
		selchanged = 0;
		return websWrite(wp, "%s",(!strcmp(auto_klife,"null"))?"3600":auto_klife);
	//	}
	}

	if(!strcmp(arg,"manual_enckey"))
	{
	/*	if(gozila_action && !selchanged)
		 {
			  char *manual_enckey_l = websGetVar(wp,"manual_enckey","");
			  return websWrite(wp, "%s",manual_enckey_l);
		 }
		 else
	  	{*/
/*	if(manual_enckey == NULL)
	{
		printf("manual_enckey=%s\n",manual_enckey);	
		return websWrite(wp,"%s","");
	}
	else
	{*/
#ifdef ZB_DEBUG
		printf("manual_enckey=%s\n",manual_enckey);	
#endif
		return websWrite(wp,"%s",(!strcmp(manual_enckey,"null"))?"":manual_enckey);

//		}
	}

	if(!strcmp(arg,"manualenc_hex"))
	{
	/*	if(gozila_action && !selchanged)
	  	{
			char *manual_enckey_l = websGetVar(wp,"manual_enckey","");
			  return websWrite(wp, "%s",manual_enckey_l);
	  	}
		else
		{*/
/*	if(manual_enckey == NULL)
		return websWrite(wp,"%s","");
	else{
*/
			if((strncmp(manual_enckey, "0x", 2) == 0) || (strncmp(manual_enckey, "0X", 2) == 0))
			{
				strcpy((char*)manualenc_hex, manual_enckey);
			}
#if 0
			else
			{
				memset( manualenc_hex, 0, 100 );
				StrToHex( (manual_enckey+2),(char*)manualenc_hex,  strlen(manual_enckey+2) );

			}
#endif
			return websWrite(wp,"%s",manualenc_hex);
	//	}
	}

	if(!strcmp(arg,"manualauth_hex"))
	{
	/*	if(gozila_action && !selchanged)
	  		{
			char *manual_enckey_l = websGetVar(wp,"manual_enckey","");
			return websWrite(wp, "%s",manual_enckey_l);
		}
		else
	  	{*/
/*	if(manual_authkey == NULL)
		return websWrite(wp,"%s","");
	else{
*/
			if((strncmp(manual_authkey, "0x", 2) == 0) || (strncmp(manual_authkey, "0X", 2) == 0))
			{
				strcpy((char*)manualauth_hex, manual_authkey);

			}
#if 0
			else
			{
				memset( manualauth_hex, 0, 100 );
				StrToHex( (manual_authkey+2),(char*)manualauth_hex,  strlen(manual_authkey+2) );

			}
#endif
			return websWrite(wp,"%s",manualauth_hex);
	//	}
	}

	if(!strcmp(arg,"manual_authkey"))
	{
	/*	if(gozila_action && !selchanged)
		{
		char *manual_authkey_l = websGetVar(wp,"manual_authkey","");
		return websWrite(wp, "%s",manual_authkey_l);
		}
		else
		{*/
/*	if(manual_authkey == NULL)	
		return websWrite(wp,"%s","");
	else*/
			return websWrite(wp,"%s",(!strcmp(manual_authkey,"null"))?"":manual_authkey);
	//	}

	}

	if(!strcmp(arg,"manual_ispi"))
	{
	/*	if(gozila_action && !selchanged)
	  	{
			char *manual_ispi_l = websGetVar(wp,"manual_ispi","");
			return websWrite(wp, "%s",manual_ispi_l);
		}
		else
		{*/
/*	if(manual_ispi == NULL)	
		return websWrite(wp,"%s","");
	else*/
#ifdef ZB_DEBUG
		printf("enter manual_ispi!\n");
		printf("manual_ispi=%s!\n",manual_ispi);
#endif
		return websWrite(wp,"%s",(!strcmp(manual_ispi,"null"))?"":manual_ispi);
	//	}

	}

	if(!strcmp(arg,"manual_ospi"))
	{
	/*	if(gozila_action && !selchanged)
	  	{
			char *manual_ospi_l = websGetVar(wp,"manual_ospi","");
			return websWrite(wp, "%s",manual_ospi_l);
		}
		else
	  	{*/
/*	if(manual_ospi == NULL)	
		return websWrite(wp,"%s","");
	else
	{*/
#ifdef ZB_DEBUG
		printf("enter manual_ospi!\n");
		printf("manual_ospi=%s!\n",manual_ospi);
#endif
			selchanged = 0;
			return websWrite(wp,"%s",(!strcmp(manual_ospi,"null"))?"":manual_ospi);
	//	}

	}
#ifdef ZB_DEBUG
	printf("end of keytyep_config_setting!\n");
#endif
	return -1;

	}


int ej_spi_array(int eid, webs_t wp, int argc, char_t **argv)
{	
	int tunnel_entry,i,ret = 0;

	if(gozila_action)
		tunnel_entry =  atoi(websGetVar(wp, "tunnel_entry", "0"));
	else
		tunnel_entry =  atoi(nvram_safe_get("tunnel_entry"));

	for(i = 0;i < 5;i++)
	{
		if(i != tunnel_entry)
		{
			int which = i;
			char *next;
			char word[250],pfs[3],presh[200],klife[20],enckey[100],authkey[100],ispi[8],ospi[8];
			foreach(word, nvram_safe_get("keytype_config"), next) 
			{
				if (which-- == 0) 
				{
					sscanf(word, "%3[^:]:%100[^:]:%20[^:]:%100[^:]:%100[^:]:%100[^:]:%100[^\n]", pfs,presh,klife,enckey,authkey,ispi,ospi);
					if(strcmp(ispi,"null") && strcmp(ospi,"null") )
					{
					 	ret = websWrite(wp,"\tin_spi[%d]=\"%s\";\n\tout_spi[%d] = \"%s\";\n",i,ispi,i,ospi);
						break;
					}
				}
			}
		}
	}
	return ret;
}

void validate_entry_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][60];
	char *tunnel_status_l,*tunnel_name_l,*local_type_l,*localgw_conn_l,*remote_type_l,*sg_type_l,*enc_type_l,*auth_type_l,*key_type_l;
	char *tunnel_l;
	char vpn_status[8],ipsec_bind[20],temp_bind[20],bind[5][5]; //new
#ifdef CLOUD_SUPPORT
	char *local_id_l;
	char *remote_id_l;
#endif
#if 0	
	struct variable entry_config_variables[] = {
		{ longname: "Tunnel Status", argv: ARGV("0", "1") },	
		{ longname: "Tunnel Name", argv: ARGV("20") },	
		{ longname: "Local Secure Group", argv: ARGV("0","1","2") },
		{ longname: "Local Security Gateway", argv: ARGV("0","1","2","3","4","5","6","7","8") },	
		{ longname: "Remote Secure Group", argv: ARGV("0","1","2","3","4") },	
		{ longname: "Remote Gateway", argv: ARGV("0","1","2") },
		{ longname: "Encryption Type", argv: ARGV("0","1","2","3") },	
		{ longname: "Authenticate Type", argv: ARGV("0","1","2") },
		{ longname: "Key Management", argv: ARGV("0","1") },

	};
#endif

#ifdef ZB_DEBUG
	printf("enter validate_entry_config\n");
#endif 

	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;

	tunnel_status_l = websGetVar(wp, "tunnel_status", NULL);
	tunnel_st = tunnel_status_l; //for passthrough saving.
	tunnel_name_l = websGetVar(wp, "tunnel_name", "");
	local_type_l = websGetVar(wp,"local_type",NULL);
	localgw_conn_l = websGetVar(wp,"localgw_conn",NULL);
	
	/* zhangbin for multi-conns 2004.4.6 */
	//localgw_conn_l = nvram_safe_get("wan_active_connection");
	
	remote_type_l = websGetVar(wp,"remote_type",NULL);
	sg_type_l = websGetVar(wp,"sg_type",NULL);
#ifdef CLOUD_SUPPORT
	local_id_l = websGetVar(wp,"local_id",NULL);
	remote_id_l = websGetVar(wp,"remote_id",NULL);
#endif
	enc_type_l = websGetVar(wp,"enc_type",NULL);
	auth_type_l = websGetVar(wp,"auth_type",NULL);
	key_type_l= websGetVar(wp,"key_type",NULL);

#ifdef ZB_DEBUG
	printf("in validate:tunnel_status_l=%s, tunnel_name_l=%s,local_type_l=%s\n,localgw_conn_l=%s,remote_type_l=%s,sg_type_l=%s,enc_type_l=%s\n,auth_type_l=%s,key_type_l=%s\n", tunnel_status_l,tunnel_name_l,local_type_l,localgw_conn_l,remote_type_l,sg_type_l,enc_type_l,auth_type_l,key_type_l);
	printf("%s:remote_id_l=%s\n",__FUNCTION__,remote_id_l);
	printf("%s:local_id_l=%s\n",__FUNCTION__,local_id_l);
#endif 


	memset(vpn_status,0,sizeof(vpn_status));
	memset(ipsec_bind,0,sizeof(ipsec_bind));
	strcpy(vpn_status,nvram_safe_get("ipsec_status"));	 //new
	strcpy(ipsec_bind,nvram_safe_get("ipsec_bind"));	 //new
#ifdef ZB_DEBUG
	printf("org:vpn_status=%s,ipsec_bind=%s\n",vpn_status,ipsec_bind);
	printf("***tunnel=%s\n",tunnel);
#endif 
	memset(bind,0,sizeof(bind));
	sscanf(ipsec_bind,"%5[^:]:%5[^:]:%5[^:]:%5[^:]:%5[^\n]",bind[0],bind[1],bind[2],bind[3],bind[4]);

#ifdef ZB_DEBUG
	{
		int cn ;
		for(cn=0;cn<5;cn++)
			printf("bind[%d]=%s\n",cn,bind[cn]);
	}
#endif

	//ipsec_bind[atoi(tunnel)]= *localgw_conn_l; //new
	strcpy(bind[atoi(tunnel)],localgw_conn_l);
	memset(temp_bind,0,sizeof(temp_bind));
	sprintf(temp_bind,"%s:%s:%s:%s:%s",bind[0],bind[1],bind[2],bind[3],bind[4]);
#ifdef ZB_DEBUG
	printf("temp_bind=%s\n",temp_bind);
#endif 
	nvram_set("ipsec_bind",temp_bind);    //new
		
	if(!strcmp(tunnel_st,"1"))
	{
		nvram_set("ipsec_enable","1");
		printf("ipsec_enable=%s\n",nvram_safe_get("ipsec_enable"));

		vpn_status[atoi(tunnel)] = '1'; //new
	}
	else
		vpn_status[atoi(tunnel)] = '0'; //new
	
#ifdef ZB_DEBUG
	printf("vpn_status=%s\n",vpn_status);
#endif 

	nvram_set("ipsec_status",vpn_status); //new
		

	#ifdef ZB_DEBUG
		printf("enter validate_entry_config6\n");
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	foreach(word, nvram_safe_get("entry_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s:%s:%s", tunnel_status_l,tunnel_name_l,local_type_l,localgw_conn_l,remote_type_l,sg_type_l,enc_type_l,auth_type_l,key_type_l);
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("entry buf %s\n", buf);
	#endif 
	
	nvram_set(v->name, buf);

#ifdef CLOUD_SUPPORT
	
	{
		char tmp_lid[IPSEC_TUNNELS][3];
		char lid_buffer[12];
		char new_lid_buffer[12];
		
		memset(tmp_lid,0,sizeof(tmp_lid));
		memset(lid_buffer,0,sizeof(lid_buffer));
		memset(new_lid_buffer,0,sizeof(new_lid_buffer));
		
		strcpy(lid_buffer,nvram_safe_get("ipsec_lid_cfg"));
		sscanf(lid_buffer,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_lid[0],tmp_lid[1],tmp_lid[2],tmp_lid[3],tmp_lid[4]);
#ifdef ZB_DEBUG
		printf("tunnel=%s\n",tunnel);
#endif
		strcpy(tmp_lid[atoi(tunnel)],local_id_l);
#ifdef ZB_DEBUG
		printf("%s:tmp_lid[%d]=%s\n",__FUNCTION__,atoi(tunnel),tmp_lid[atoi(tunnel)]);
#endif

		sprintf(new_lid_buffer,"%s:%s:%s:%s:%s",tmp_lid[0],tmp_lid[1],tmp_lid[2],tmp_lid[3],tmp_lid[4]);
		
#ifdef ZB_DEBUG
		printf("%s:new_lid_buffer=%s\n",__FUNCTION__,new_lid_buffer);
#endif

		nvram_set("ipsec_lid_cfg",new_lid_buffer);

	}

	{
		char tmp_rid[IPSEC_TUNNELS][3];
		char rid_buffer[12];
		char new_rid_buffer[12];
		
		memset(tmp_rid,0,sizeof(tmp_rid));
		memset(rid_buffer,0,sizeof(rid_buffer));
		memset(new_rid_buffer,0,sizeof(new_rid_buffer));
		
		strcpy(rid_buffer,nvram_safe_get("ipsec_rid_cfg"));
		sscanf(rid_buffer,"%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]",tmp_rid[0],tmp_rid[1],tmp_rid[2],tmp_rid[3],tmp_rid[4]);
		strcpy(tmp_rid[atoi(tunnel)],remote_id_l);
		printf("%s:tmp_rid[%d]=%s\n",__FUNCTION__,atoi(tunnel),tmp_rid[atoi(tunnel)]);

		sprintf(new_rid_buffer,"%s:%s:%s:%s:%s",tmp_rid[0],tmp_rid[1],tmp_rid[2],tmp_rid[3],tmp_rid[4]);
		
		printf("%s:new_rid_buffer=%s\n",__FUNCTION__,new_rid_buffer);

		nvram_set("ipsec_rid_cfg",new_rid_buffer);

	}
#endif
	return;
}

void validate_ltype_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][60];
	char *local_type_l,*l_ipaddr_l,*l_netmask_l = NULL;
	char *tunnel_l;
	char temp[30], *val=NULL;
#if 0
	struct variable ltype_config_variables[] = {
		{ longname: "Local IP", argv: NULL },	
		{ longname: "Local Netmask", argv: NULL },
		
	};
#endif
	#ifdef ZB_DEBUG
		printf("ltype_validate!\n");
	#endif 
		
	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;
	
	local_type_l = websGetVar(wp, "local_type", NULL);

	l_ipaddr_l = malloc(20);
	memset(l_ipaddr_l, 0, sizeof(l_ipaddr_l));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s%d", "l_ipaddr", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(l_ipaddr_l,val);
                        if(i<3) 
				strcat(l_ipaddr_l,".");
                }
                else
		{	
			free(l_ipaddr_l);
                        return ;
		}
    	}
#if 0	
	if(!strcmp(tunnel_st,"1"))
	{
		if (!*l_ipaddr_l) 
		{
                #ifdef ZB_DEBUG
			printf("!*l_ipaddr_l!\n");
		#endif
	
			websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
                	return;
	   	 }
		if (!valid_ipaddr(wp, l_ipaddr_l, &ltype_config_variables[0]))
		{
		#ifdef ZB_DEBUG
			printf("!valid_ipaddr!\n");
		#endif
		return;
		}
	
	}
#endif

		if(!strcmp(local_type_l,"0"))
		{
			l_netmask_l = NULL;
		}
		else if(!strcmp(local_type_l,"1"))
		{    
			l_netmask_l = malloc(20);
			memset(l_netmask_l, 0, sizeof(l_netmask_l));
			memset(temp, 0, sizeof(temp));
    
			for(i=0 ; i<4 ; i++)
			{
        		        snprintf(temp, sizeof(temp), "%s%d", "l_netmask", i);
                		val = websGetVar(wp, temp , NULL);
		                if(val)
				{
                		        strcat(l_netmask_l, val);
	                	        if(i<3) 
						strcat(l_netmask_l, ".");
	                	}
		                else
				{	
					free(l_netmask_l);
                        		return ;
				}
    			}
#if 0			
		       if (!valid_netmask(wp, l_netmask_l, &ltype_config_variables[1]))
			{
				#ifdef ZB_DEBUG
				printf("!valid_netmask!\n");
				#endif
				return;
			}
#endif		      
		}
			
	#ifdef ZB_DEBUG
		printf("enter validate_ltype_config6\n");
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	foreach(word, nvram_safe_get("ltype_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s", l_ipaddr_l,l_netmask_l);
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("ltype->buf: %s\n", buf);
	#endif 
	nvram_set(v->name, buf);
	if(l_ipaddr_l)	
		free(l_ipaddr_l);
		
	if(l_netmask_l)	
		free(l_netmask_l);
		
	return;
}

void validate_rtype_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][60];
	char *remote_type_l,*r_ipaddr_l,*r_netmask_l;
	char *tunnel_l;
	char temp[30], *val=NULL;
#if 0
	struct variable rtype_config_variables[] = {
		{ longname: "Remote IP", argv: NULL },	
		{ longname: "Remote Netmask", argv: NULL },
		
	};
#endif
	#ifdef ZB_DEBUG
		printf("enter validate_rtype_config\n");
	#endif 
		
	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;
	
	remote_type_l = websGetVar(wp, "remote_type", NULL);

		if((!strcmp(remote_type_l,"2")) || (!strcmp(remote_type_l,"3")) || (!strcmp(remote_type_l,"4")))
		{
			r_ipaddr_l = NULL;
			r_netmask_l = NULL;
		}
		else 
		{
			r_ipaddr_l = malloc(20);
			memset(r_ipaddr_l, 0, sizeof(r_ipaddr_l));
			memset(temp, 0, sizeof(temp));
       			for(i=0 ; i<4 ; i++)
			{
        	        	snprintf(temp, sizeof(temp), "%s%d", "r_ipaddr", i);
	        	        val = websGetVar(wp, temp , NULL);
        	        	if(val)
				{
                	        	strcat(r_ipaddr_l,val);
	                        	if(i<3) 
						strcat(r_ipaddr_l,".");
                		}
	                	else
				{	
					free(r_ipaddr_l);
        		     		return ;
				}
   		 	}

			if(!strcmp(remote_type_l,"0"))
			{
				r_netmask_l = NULL;
			}
			else
			{
				r_netmask_l = malloc(20);
				memset(r_netmask_l, 0, sizeof(r_netmask_l));
				memset(temp, 0, sizeof(temp));
				for(i=0 ; i<4 ; i++)
				{
                			snprintf(temp, sizeof(temp), "%s%d", "r_netmask", i);
			                val = websGetVar(wp, temp , NULL);
        	        		if(val)
					{
                			        strcat(r_netmask_l, val);
                        			if(i<3) 
							strcat(r_netmask_l, ".");
        		        	}
		                	else
					{	
						free(r_netmask_l);
                			        return ;
					}
	    			}	
		/*	
				if (!*r_netmask_l)
	        	        {
        	        	        websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Subnet Mask<br>", v->longname);
                	        	return;
                		}
				if (!valid_netmask(wp, r_netmask_l, &rtype_config_variables[1]))
					return;
*/
			}
		}
		#ifdef ZB_DEBUG
			printf("enter validate_rtype_config6\n");
		#endif 
	
		memset(word, 0, sizeof(word));
		memset(buf, 0, sizeof(buf));
	
		for(i=0 ; i<IPSEC_TUNNELS ; i++)
			memset(old[i], 0, sizeof(old[i]));
	
		i=0;
		foreach(word, nvram_safe_get("rtype_config"), next) 
		{
			strcpy(old[i], word);
			i++;
		}
		snprintf(old[atoi(tunnel_l)], sizeof(old[0]), "%s:%s", r_ipaddr_l,r_netmask_l);
	
		for(i=0 ; i<IPSEC_TUNNELS ; i++)
		{
			if(strcmp(old[i],""))
				cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",cur == buf ? "" : " ", old[i]);
		}

		#ifdef ZB_DEBUG
			printf("rtype->buf %s\n", buf);
		#endif 
	
		nvram_set(v->name, buf);
	
		if(r_ipaddr_l)	
			free(r_ipaddr_l);
		
		if(r_netmask_l)	
			free(r_netmask_l);
		
    //	} /* end if tunnel_st*/
	
	return;
}

void validate_sg_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][60];
	char *sg_type_l,*sg_ipaddr_l = NULL,*sg_domain_l = NULL;
	char *tunnel_l;
	char temp[30], *val=NULL;
	struct variable sg_config_variables[] = {
		{ longname: "Security Gateway IP", argv: NULL },	
		{ longname: "Security Gateway Domain", argv: ARGV("100")},	
		
	};

	#ifdef ZB_DEBUG
		printf("enter validate_sg_config\n");
	#endif 
		
	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;

	sg_type_l = websGetVar(wp, "sg_type", NULL);
	
	if(!strcmp(sg_type_l,"0"))
	{
		sg_domain_l = NULL;
		sg_ipaddr_l = malloc(20);
		memset(sg_ipaddr_l, 0, sizeof(sg_ipaddr_l));
		memset(temp, 0, sizeof(temp));
       		for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s%d", "sg_ipaddr", i);
	                val = websGetVar(wp, temp , NULL);
        	        if(val)
			{
				strcat(sg_ipaddr_l,val);
	                        if(i<3) 
					strcat(sg_ipaddr_l,".");
                	}
	                else
			{	
				free(sg_ipaddr_l);
             			return ;
			}
   	 	}
   	}
   	else if(!strcmp(sg_type_l,"2"))
   	{
   		sg_ipaddr_l = NULL;
		sg_domain_l = websGetVar(wp, "sg_domain", "");
		
		if(!valid_name(wp, sg_domain_l, &sg_config_variables[1]))
			return;
   	}
	#ifdef ZB_DEBUG
		printf("enter validate_sg_config6\n");
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
#ifdef ZB_DEBUG
	printf("memset old!\n");	
#endif
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
#ifdef ZB_DEBUG
	printf("before foreach!\n");	
#endif
	foreach(word, nvram_safe_get("sg_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
#ifdef ZB_DEBUG
	printf("before sprintf!\n");	
#endif
	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s", sg_ipaddr_l,sg_domain_l);
	
#ifdef ZB_DEBUG
	printf("before for!\n");	
#endif
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("sg->buf %s\n", buf);
	#endif 
	nvram_set(v->name, buf);
	
	if(sg_ipaddr_l)	
		free(sg_ipaddr_l);
			
	return;
}
#ifdef CLOUD_SUPPORT

void validate_lid_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[300], *cur=buf;
	char old[IPSEC_TUNNELS][80];
	char *local_id_l,*lid_ip_l = NULL,*lid_fqdn_l = NULL;
	char *tunnel_l;

	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;

	local_id_l = websGetVar(wp, "local_id", NULL);
	
	if(!strcmp(local_id_l,"0")) //local id type is ipaddress
	{
		lid_fqdn_l = "0";
		lid_ip_l = "0";
   	}
	else if(!strcmp(local_id_l,"1")) //local id type is fqdn
   	{
   		lid_ip_l = "0";
		lid_fqdn_l = websGetVar(wp, "lid_fqdn", "");

   	}
	#ifdef ZB_DEBUG
		printf("lid_ip_l=%s,lid_fqdn_l=%s\n",lid_ip_l,lid_fqdn_l);
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));

	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	
	#ifdef ZB_DEBUG
	printf("before foreach!\n");	
	#endif 

	foreach(word, nvram_safe_get("lid_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}

	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s", lid_ip_l,lid_fqdn_l);
	
	#ifdef ZB_DEBUG
	printf("old[%d]=%s!\n",atoi(tunnel),old[atoi(tunnel)]);	
	#endif 

	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("lid->buf %s\n", buf);
	#endif 

	nvram_set(v->name, buf);
			
	return;

}

void validate_rid_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[300], *cur=buf;
	char old[IPSEC_TUNNELS][80];
	char *remote_id_l,*rid_ip_l = NULL,*rid_fqdn_l = NULL;
	char *tunnel_l;

	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;

	remote_id_l = websGetVar(wp, "remote_id", NULL);
	
	if(!strcmp(remote_id_l,"0")) //remote id type is ipaddress
	{
		rid_fqdn_l = "0";
		rid_ip_l = "0";
#if 0
		rid_ip_l = malloc(20);
		memset(rid_ip_l, 0, sizeof(rid_ip_l));
		memset(temp, 0, sizeof(temp));
       		for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s%d", "rid_ipaddr", i);
	                val = websGetVar(wp, temp , NULL);
        	        if(val)
			{
				strcat(rid_ip_l,val);
	                        if(i<3) 
					strcat(rid_ip_l,".");
                	}
	                else
			{	
				free(rid_ip_l);
             			return ;
			}
   	 	}
#endif
   	}
   	else if(!strcmp(remote_id_l,"1")) //remote id type is fqdn
   	{
   		rid_ip_l = "0";
		rid_fqdn_l = websGetVar(wp, "rid_fqdn", "");

   	}
	#ifdef ZB_DEBUG
		printf("rid_ip_l=%s,rid_fqdn_l=%s\n",rid_ip_l,rid_fqdn_l);
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));

	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	
	#ifdef ZB_DEBUG
	printf("before foreach!\n");	
	#endif 

	foreach(word, nvram_safe_get("rid_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}

	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s", rid_ip_l,rid_fqdn_l);
	
	#ifdef ZB_DEBUG
	printf("old[%d]=%s!\n",atoi(tunnel),old[atoi(tunnel)]);	
	#endif 

	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("rid->buf %s\n", buf);
	#endif 

	nvram_set(v->name, buf);
			
	return;

}
#endif
void validate_keytype_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[300], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][500],new_presh_l[100];
	char *auto_pfs_l,*auto_presh_l,*auto_klife_l,*manual_enckey_l,*manual_authkey_l,*manual_ispi_l,*manual_ospi_l; 
	char *tunnel_l,*key_type_l;
	struct variable keytype_config_variables[] = {
		{ longname: "Autokey PFS Status", argv: ARGV("0", "1")},	
		{ longname: "Autokey Preshared Key", argv: ARGV("100")},
		{ longname: "Autokey Lifetime", argv: ARGV("10")},
		{ longname: "Manualkey Encryption Key", argv: ARGV("100")},
		{ longname: "Manualkey Authentication Key", argv: ARGV("100")},
		{ longname: "Manualkey Inbound SPI", argv: ARGV("10")},
		{ longname: "Manualkey Outbound SPI", argv: ARGV("10")},
		
	};

	#ifdef ZB_DEBUG
		printf("enter validate_keytype_config\n");
	#endif 
		
	tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;
	
	key_type_l = websGetVar(wp, "key_type", NULL);
	
	if(!strcmp(key_type_l,"0"))
	{
		auto_pfs_l = websGetVar(wp,"auto_pfs",NULL);
		auto_presh_l = websGetVar(wp,"auto_presh","");
		filter_name(auto_presh_l,new_presh_l,sizeof(new_presh_l),SET);
#ifdef ZB_DEBUG
		printf("auto_presh_l=%s,new_presh_l=%s\n",auto_presh,new_presh_l);
#endif
		auto_klife_l = websGetVar(wp,"auto_klife","");

		if(!valid_choice(wp, auto_pfs_l, &keytype_config_variables[0]))
		{
			printf("auto_pfs_l invalide choice!\n");
			return;	
		}
		/*
		if(!valid_name(wp, auto_presh_l, &keytype_config_variables[1]))
		{
			printf("auto_presh_l invalide name!\n");
			return;	
		}*/
		if(!valid_name(wp, auto_klife_l, &keytype_config_variables[2]))
		{
			printf("auto_klife_l invalide name!\n");
			return;	
		}
		manual_enckey_l = "null";
		manual_authkey_l = "null";
		manual_ispi_l = "null";
		manual_ospi_l = "null";
	}
	else
	{
		auto_pfs_l = "0";
		auto_presh_l = "null";
		strcpy(new_presh_l,"null");
		auto_klife_l = "null";
		manual_enckey_l = websGetVar(wp,"manual_enckey","");
	//	printf("***********manual_enckey_l=%s\n",manual_enckey_l);
		manual_authkey_l = websGetVar(wp,"manual_authkey","");
	//	printf("***********manual_authkey_l=%s\n",manual_authkey_l);
		manual_ispi_l = websGetVar(wp,"manual_ispi","");
	//	printf("***********manual_ispi_l=%s\n",manual_ispi_l);
		manual_ospi_l = websGetVar(wp,"manual_ospi","");
	}

	#ifdef ZB_DEBUG
		printf("get validate_keytype_config local variables\n");
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	foreach(word, nvram_safe_get("keytype_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s", auto_pfs_l,new_presh_l,auto_klife_l,manual_enckey_l,manual_authkey_l,manual_ispi_l,manual_ospi_l);

#ifdef ZB_DEBUG
	printf("old[atoi(tunnel)]=%s\n",old[atoi(tunnel)]);	
#endif

	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("keytype->buf %s\n", buf);
	#endif 

	nvram_set(v->name, buf);
	return;
}

/* ipsecadv_config = ipsec_opmode:ipsecp1_enc:ipsecp1_auth:ipsecp1_group:ipsecp1_klife:ipsecp2_group:ipsecp2_klife:ipsec_netbios:ipsec_antireplay:ipsec_keepalive:ipsec_blockip:ipsec_retrytimes:ipsec_blocksecs */

void ipsecadv_config_init(int which)
{
	char *next;
	//int which;
	
	memset(ipsecadv_config_buf, 0, sizeof(ipsecadv_config_buf));
	
	if(!strcmp(tunnel, "")) 
		strcpy(tunnel, "0");
		
	printf("Enter ipsecadv_init!\n");
/*	which = atoi(tunnel);
	tunnel_local =websGetVar(wp,"tunnel_entry",NULL) ;
	which = atoi(tunnel_local);*/
	//printf("init:which=%d\n",which);
	
	foreach(ipsecadv_config_buf, nvram_safe_get("ipsecadv_config"), next) 
	{
		if (which-- == 0) 
		{
			sscanf(ipsecadv_config_buf, "%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%10[^:]:%12[^\n]", ipsec_opmode,ipsecp1_enc,ipsecp1_auth,ipsecp1_group,ipsecp1_klife,ipsecp2_group,ipsec_netbios,ipsec_antireplay,ipsec_keepalive,ipsec_blockip,ipsec_retrytimes,ipsec_blocksecs);
		#ifdef ZB_DEBUG
			printf("ipsec_opmode=%s,ipsecp1_enc=%s,ipsecp1_auth=%s,ipsecp1_group=%s,ipsecp1_klife=%s\n,ipsecp2_group=%s,ipsec_netbios=%s,ipsec_antireplay=%s,ipsec_keepalive=%s\n,ipsec_blockip=%s,ipsec_retrytimes=%s,ipsec_blocksecs=%s\n", ipsec_opmode,ipsecp1_enc,ipsecp1_auth,ipsecp1_group,ipsecp1_klife,ipsecp2_group,ipsec_netbios,ipsec_antireplay,ipsec_keepalive,ipsec_blockip,ipsec_retrytimes,ipsec_blocksecs);
		#endif 
			//printf("ipsecadv_config_buf=%s\n",ipsecadv_config_buf);
			break;
		}
        
	}
	ipsecadv_config_inited = 1;
}	

int ej_ipsecadv_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count,which;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	which = atoi(tunnel);

	if(ipsecadv_config_inited == 0)
		ipsecadv_config_init(which);	
		
	
	if(!strcmp(arg,"tunnel_entry"))
	{
		selchanged = 0;
		return websWrite(wp, "Tunnel %d",which+1);
	}
	
	if(!strcmp(arg,"ipsec_opmode"))
	{
		if(((count == 1) && (!strcmp(ipsec_opmode, "1"))) || ((count == 0) && (!strcmp(ipsec_opmode, "0"))))
			return websWrite(wp, "checked");
	}
	
	if(!strcmp(arg, "ipsecp1_enc"))
	{
		
			if(((count == 1) && (!strcmp(ipsecp1_enc, "1"))) || ((count == 2) && (!strcmp(ipsecp1_enc, "2"))))
			 
				return websWrite(wp, "selected");
	}
	
	if(!strcmp(arg, "ipsecp1_auth"))
	{
		
			if(((count == 1) && (!strcmp(ipsecp1_auth, "1"))) || ((count == 2) && (!strcmp(ipsecp1_auth, "2"))))
			 
				return websWrite(wp, "selected");
	}
	
	if(!strcmp(arg, "ipsecp1_group"))
	{
		
			if(((count == 1) && (!strcmp(ipsecp1_group, "1"))) || ((count == 2) && (!strcmp(ipsecp1_group, "2"))))
			 
				return websWrite(wp, "selected");
	}
	
	if(!strcmp(arg, "ipsecp1_klife"))
	{
		return websWrite(wp, "%s",ipsecp1_klife);
	}
	
	if(!strcmp(arg, "ipsecp2_enc"))
	{
		char ipsecp2_enc_l[10];
		if(!strcmp(enc_type, "0"))
			strcpy(ipsecp2_enc_l,"ESP_NULL");
		else if(!strcmp(enc_type, "1"))
			strcpy(ipsecp2_enc_l,"DES");
		else if(!strcmp(enc_type, "2"))
			strcpy(ipsecp2_enc_l,"3DES");
		else if(!strcmp(enc_type, "3"))
			strcpy(ipsecp2_enc_l,"AES");
		
		return websWrite(wp, "%s",ipsecp2_enc_l);
	}
	
	if(!strcmp(arg, "ipsecp2_auth"))
	{
		char ipsecp2_auth_l[8];
		if(!strcmp(auth_type, "1"))
			strcpy(ipsecp2_auth_l,"MD5");
		else if(!strcmp(auth_type, "2"))
			strcpy(ipsecp2_auth_l,"SHA");
				
		return websWrite(wp, "%s",ipsecp2_auth_l);
	}
	
	if(!strcmp(arg, "ipsecp2_pfs"))
	{
		char ipsecp2_pfs_l[8];
		if(!strcmp(auto_pfs, "1"))
			strcpy(ipsecp2_pfs_l,"ON");
		else if(!strcmp(auto_pfs, "0"))
			strcpy(ipsecp2_pfs_l,"OFF");
		
		return websWrite(wp, "%s",ipsecp2_pfs_l);
			
	}
	
	if(!strcmp(arg, "ipsecp2_group"))
	{
		
			if(((count == 1) && (!strcmp(ipsecp2_group, "1"))) || ((count == 2) && (!strcmp(ipsecp2_group, "2"))))
			 
				return websWrite(wp, "selected");
	}
	
	if(!strcmp(arg, "ipsecp2_klife"))
	{
		return websWrite(wp, "%s",auto_klife);
	}
	
	if(!strcmp(arg, "ipsec_natt"))
	{
		if(!strcmp(nvram_safe_get("ipsec_natt"),"1"))
			return websWrite(wp, "checked");
		else 
			return websWrite(wp, "");
	}

	if(!strcmp(arg, "ipsec_netbios"))
	{
		
			if(!strcmp(ipsec_netbios,"1"))			 
				return websWrite(wp, "checked");
			else 
				return websWrite(wp, "");
	}
	
	if(!strcmp(arg, "ipsec_keepalive"))
	{
		
			if(!strcmp(ipsec_keepalive,"1"))			 
				return websWrite(wp, "checked");
			else 
				return websWrite(wp, "");
	}
	
	if(!strcmp(arg, "ipsec_antireplay"))
	{
		
			if(!strcmp(ipsec_antireplay,"1"))			 
				return websWrite(wp, "checked");
			else 
				return websWrite(wp, "");
	}
	
	if(!strcmp(arg, "ipsec_blockip"))
	{
		
			if(!strcmp(ipsec_blockip,"1"))			 
				return websWrite(wp, "checked");
			else 
				return websWrite(wp, "");
	}
	
	if(!strcmp(arg, "ipsec_retrytimes"))
	{	
		if(!strcmp(ipsec_retrytimes,"0"))
			return websWrite(wp, "%s","");

		return websWrite(wp, "%s",ipsec_retrytimes);
	}
	
	if(!strcmp(arg, "ipsec_blocksecs"))
	{
		if(!strcmp(ipsec_blocksecs,"0"))
			return websWrite(wp, "%s","");

		return websWrite(wp, "%s",ipsec_blocksecs);
	}
		
	return -1;
	
}

void validate_ipsecadv_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[256], *next;
	char buf[1000], *cur=buf;
	char old[IPSEC_TUNNELS][60];
	char *ipsec_opmode_l,*ipsecp1_enc_l,*ipsecp1_auth_l,*ipsecp1_group_l,*ipsecp1_klife_l,*ipsecp2_group_l,*ipsec_netbios_l,*ipsec_antireplay_l,*ipsec_keepalive_l,*ipsec_blockip_l,*ipsec_retrytimes_l,*ipsec_blocksecs_l,*ipsec_natt_l;
#if 0		
	struct variable ipsecadv_config_variables[] = {
		{ longname: "IPSec Operate Mode", argv: ARGV("0", "1") },	
		{ longname: "IPSec Phase 1 Encryption KEY Type", argv: ARGV("0","1","2","3") },	
		{ longname: "IPSec Phase 1 Authentication KEY Type", argv: ARGV("1","2") },	
		{ longname: "IPSec Phase 1 DH Group", argv: ARGV("1","2") },	
		{ longname: "IPSec Phase 1 key Lifetime", argv: ARGV("10") },	
		{ longname: "IPSec Phase 2 DH Group", argv: ARGV("1","2") },	
	//	{ longname: "IPSec Phase 2 key Lifetime", argv: ARGV("10") },	
		{ longname: "IPSec Netbios Broadcast", argv: ARGV("0","1") },
		{ longname: "IPSec Anti-Replay", argv: ARGV("0","1") },
		{ longname: "IPSec Keep Alive", argv: ARGV("0","1") },
		{ longname: "IPSec Block IP", argv: ARGV("0","1") },
		{ longname: "IPSec IP Retry Times", argv: ARGV("10") },
		{ longname: "IPSec Block IP Secondes", argv: ARGV("10") },
		
	};
#endif
	#ifdef ZB_DEBUG
		printf("enter ipsecadv_config\n");
	#endif 
		
	/*tunnel_l = websGetVar(wp, "tunnel_entry", NULL);
	if(tunnel_l == NULL)
		return;*/
	
	ipsec_opmode_l = websGetVar(wp, "ipsec_opmode", NULL);
	ipsecp1_enc_l = websGetVar(wp, "ipsecp1_enc", NULL);
	ipsecp1_auth_l = websGetVar(wp,"ipsecp1_auth",NULL);
	ipsecp1_group_l = websGetVar(wp,"ipsecp1_group",NULL);
	ipsecp1_klife_l = websGetVar(wp,"ipsecp1_klife","");
	ipsecp2_group_l = websGetVar(wp,"ipsecp2_group",NULL);
	//ipsecp2_klife_l = websGetVar(wp,"ipsecp2_klife","");
	ipsec_natt_l = websGetVar(wp,"hid_ipsec_natt",NULL);
	ipsec_netbios_l = websGetVar(wp,"hid_ipsec_netbios",NULL);
	ipsec_antireplay_l = websGetVar(wp,"hid_ipsec_antireplay",NULL);
	ipsec_keepalive_l = websGetVar(wp,"hid_ipsec_keepalive",NULL);
	ipsec_blockip_l = websGetVar(wp,"hid_ipsec_blockip","0");
	if(!strcmp(ipsec_blockip_l,"1"))
	{		
		ipsec_retrytimes_l = websGetVar(wp,"ipsec_retrytimes","0");
		ipsec_blocksecs_l = websGetVar(wp,"ipsec_blocksecs","0");
	}
	else
	{		
		ipsec_retrytimes_l = "5";
		ipsec_blocksecs_l = "60";
	}

	#ifdef ZB_DEBUG
	printf("ipsec_opmode_l=%s,ipsecp1_enc_l=%s,ipsecp1_auth_l=%s,ipsecp1_group_l=%s\n,ipsecp1_klife_l=%s,ipsecp2_group_l=%s,ipsec_netbios_l=%s,ipsec_antireplay_l=%s,ipsec_keepalive_l=%s\n,ipsec_blockip_l=%s,ipsec_retrytimes_l=%s,ipsec_blocksecs_l=%s\n", ipsec_opmode_l,ipsecp1_enc_l,ipsecp1_auth_l,ipsecp1_group_l,ipsecp1_klife_l,ipsecp2_group_l,ipsec_netbios_l,ipsec_antireplay_l,ipsec_keepalive_l,ipsec_blockip_l,ipsec_retrytimes_l,ipsec_blocksecs_l);
		#endif 
#if 0	
	if(!valid_choice(wp, ipsec_opmode_l , &ipsecadv_config_variables[0]))
	{
		printf("!valid_ipsec_opmode_l!\n");
		return;
	}
	if(!valid_choice(wp, ipsecp1_enc_l , &ipsecadv_config_variables[1]))
	{
		printf("!valid_ipsecp1_enc_l!\n");
		return;
	}
	if(!valid_choice(wp, ipsecp1_auth_l , &ipsecadv_config_variables[2]))
	{
		printf("!valid_ipsecp1_auth_l!\n");
		return;
	}
	if(!valid_choice(wp, ipsecp1_group_l , &ipsecadv_config_variables[3]))
	{
		printf("!valid_ipsecp1_group_l!\n");
		return;
	}
	if(!valid_name(wp, ipsecp1_klife_l, &ipsecadv_config_variables[4]))
	{
		printf("ipsecp1_klife error!\n");
		return;
	}
	if(!valid_choice(wp, ipsecp2_group_l , &ipsecadv_config_variables[5]))
		return;
	if(!valid_name(wp, ipsecp2_klife_l, &ipsecadv_config_variables[6]))
		return;
	if(!valid_choice(wp, ipsec_netbios_l, &ipsecadv_config_variables[6]))
		return;
	if(!valid_choice(wp, ipsec_antireplay_l, &ipsecadv_config_variables[7]))
		return;
	if(!valid_choice(wp, ipsec_keepalive_l, &ipsecadv_config_variables[8]))
		return;
	if(!valid_choice(wp, ipsec_blockip_l, &ipsecadv_config_variables[9]))
		return;
	if(!strcmp(ipsec_blockip_l,"1"))
	{
		if(!valid_name(wp, ipsec_retrytimes_l, &ipsecadv_config_variables[10]))
		{
			printf("Invalide ipsec_retrytimes!\n");
			return;
		}
		if(!valid_name(wp, ipsec_blocksecs_l, &ipsecadv_config_variables[11]))
		{
			printf("Invalide ipsec_blocksecs!\n");
			return;
		}
	}
#endif	
	#ifdef ZB_DEBUG
		printf("enter ipsecadv_config6\n");
	#endif 
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	foreach(word, nvram_safe_get("ipsecadv_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(tunnel)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s", ipsec_opmode_l,ipsecp1_enc_l,ipsecp1_auth_l,ipsecp1_group_l,ipsecp1_klife_l,ipsecp2_group_l,ipsec_netbios_l,ipsec_antireplay_l,ipsec_keepalive_l,ipsec_blockip_l,ipsec_retrytimes_l,ipsec_blocksecs_l);
		
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
		printf("ipsecadv->buf %s\n", buf);
	#endif 
	
	nvram_set(v->name, buf);
	nvram_set("ipsec_natt",ipsec_natt_l);
	ipsecadv_config_inited = 0;
	return;
}

int search_and_stop(char *conn,int which)
{
	char word[120],tstatus[3],tname[40],ltype[3],lconn[3],rtype[3],sgtype[3],enc[3],auth[3],ktype[3],*next;
	int i,j = which,bkeep = 0,bsame = 0;
	
	for(i = 0;i < 5;i++)
	{
		int count = i;
		if(i == j)
			continue;
		memset(tstatus,0,sizeof(tstatus));
		memset(tname,0,sizeof(tname));
		memset(ltype,0,sizeof(ltype));
		memset(lconn,0,sizeof(lconn));
		memset(rtype,0,sizeof(rtype));
		memset(sgtype,0,sizeof(sgtype));
		memset(enc,0,sizeof(enc));
		memset(auth,0,sizeof(auth));
		memset(ktype,0,sizeof(ktype));
		memset(word,0,sizeof(word));
		
		foreach(word, nvram_safe_get("entry_config"), next) 
		{
			if (count-- == 0) 
			{
				sscanf(word, "%3[^:]:%40[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tstatus,tname,ltype, lconn,rtype, sgtype, enc, auth, ktype);
				#ifdef ZB_DEBUG
				printf("tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s,key_type=%s\n", tstatus,tname,ltype,lconn,rtype,sgtype,enc,auth,ktype);
				#endif 
				break;
			}
		}
		if(!strcmp(tstatus,"1"))
			bkeep = 1;
		if(!strcmp(conn,lconn))
			bsame = 1;
	}

	if(!bkeep)
	{
		printf("No other enabled tunnel!\n");
		if(killipsec(6) != 0)
		{
			printf("Cannot killall ipsec!\n");
			return -1;
		}
	}
	else
	{
		if(!bsame)
		{
			printf("No the same PVC(%s)'s tunnel!\n",conn);
			if(killipsec(atoi(conn)) != 0) 
			{
				printf("Cannot down ipsec!\n");
				return -1;
			}
		}
	}
	return 0;

}

int down_tunnel(int which)
{
	int ret = 0;

	//if (nvram_match("ipsec_enable", "1"))
	if(strstr(nvram_safe_get("ipsec_status"), "1"))
	{
		int counter = which;
		char word[120],tstatus[3],tname[40],ltype[3],lconn[3],rtype[3],sgtype[3],enc[3],auth[3],ktype[3],config[5][80];
		
		memset(word,0,sizeof(word));
		memset(tstatus,0,sizeof(tstatus));
		memset(tname,0,sizeof(tname));
		memset(ltype,0,sizeof(ltype));
		memset(lconn,0,sizeof(lconn));
		memset(rtype,0,sizeof(rtype));
		memset(sgtype,0,sizeof(sgtype));
		memset(enc,0,sizeof(enc));
		memset(auth,0,sizeof(auth));
		memset(ktype,0,sizeof(ktype));
		strcpy(word,nvram_safe_get("entry_config"));
		memset(config,0,sizeof(config));
		
		sscanf(word,"%s %s %s %s %s",config[0],config[1],config[2],config[3],config[4]);
		//foreach(word, nvram_safe_get("entry_config"), next) 
		//{
			//if (counter-- == 0) 
			//{
				sscanf(config[counter], "%3[^:]:%40[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tstatus,tname,ltype, lconn,rtype, sgtype, enc, auth, ktype);
				//#ifdef ZB_DEBUG
				printf("Test:tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s,key_type=%s\n", tstatus,tname,ltype,lconn,rtype,sgtype,enc,auth,ktype);
				//#endif 
				//break;
			//}
		//}
		if(!strcmp(tstatus,"1"))
		{
			if(!strcmp(ktype,"0"))
			{
				FILE *ter_fd;
			
				if((ter_fd = fopen("/tmp/down_tunnel","w")) == NULL)
				{
					printf("Open /tmp/down_tunnel failed!\n");
					return -1;
				}
				
				fprintf(ter_fd,"#!/bin/sh\n");
				fprintf(ter_fd,"echo\n");
				fprintf(ter_fd,"/bin/ipsec/whack --terminate --name %s\n",tname);
				fprintf(ter_fd,"echo\n");
				fprintf(ter_fd,"/bin/ipsec/whack --unroute --name %s\n",tname);
				fprintf(ter_fd,"echo\n");
				fprintf(ter_fd,"rm -rf /tmp/ipsecst/%s 2> /dev/null > /dev/null\n",tname);
		
				fclose(ter_fd);
				ret = eval("/bin/sh","/tmp/down_tunnel");

			}
			else if(!strcmp(ktype,"1"))
			{
				char file_name[20];
				memset(file_name,0,sizeof(file_name));
				sprintf(file_name,"/tmp/deltun_%d 0",which);
				printf("will execute %s\n",file_name);
				//eval("/bin/sh",file_name);
				system(file_name);
			}
		}
		
		if(search_and_stop(lconn,which) != 0)
		{
			printf("search_and_stop Error!\n");
			return -1;
		}

	}
	return ret;
}
	
int ipsec_modify_tunnel(int which,char *type, char *string)
{
	int i=0;
	char word[200],buf[512];
	char *next,*cur = buf;
	char old[IPSEC_TUNNELS][60];
		
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i=0;
	foreach(word, nvram_safe_get(type), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[which], sizeof(old[0]), "%s", string);
	
	for(i=0 ; i<IPSEC_TUNNELS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef ZB_DEBUG
	//	printf("type %s ,commit buf= %s\n", type,buf);
	#endif 
	
	nvram_set(type, buf);
	//nvram_commit(); //zhangbin-2004.4.15
	entry_config_inited = 0;
	ipsec_local_inited = 0;
	ipsec_remote_inited = 0;
	ipsec_sg_inited = 0;
	/* zhangbin 2005.3.31 for remote id type in Cloud */
	#ifdef CLOUD_SUPPORT
	ipsec_lid_inited = 0;
	ipsec_rid_inited = 0;
	#endif
	ipsec_key_inited = 0;
	ipsecadv_config_inited = 0;
	selchanged = 1;
	return 0;
}

int ipsec_delete_tunnel(webs_t wp)
{
	int ret = 0;
	char *id = websGetVar(wp,"tunnel_entry",NULL);

	if(!id)
		return -1;

	printf("atoi(id)=%d\n",atoi(id));

	//zhangbin:Before deleting,we should down this tunnel. --2004.0804
	down_tunnel(atoi(id));
	
	ret = ipsec_modify_tunnel(atoi(id),"entry_config","0::1:2:1:0:1:1:0");
	ret += ipsec_modify_tunnel(atoi(id),"ltype_config","0.0.0.0:0.0.0.0");
	ret += ipsec_modify_tunnel(atoi(id),"rtype_config","0.0.0.0:255.255.255.0");
	ret += ipsec_modify_tunnel(atoi(id),"sg_config","0.0.0.0:null"); 
	ret += ipsec_modify_tunnel(atoi(id),"keytype_config","0:null:3600:null:null:null:null"); 
	nvram_commit(); //zhangbin-2004.4.15
	selchanged = 1; //make the flag of change conn
			
	entry_config_inited = 0;
	ipsec_local_inited = 0;
	ipsec_remote_inited = 0;
	ipsec_sg_inited = 0;
	/* zhangbin 2005.3.31 for remote id type in Cloud */
	#ifdef CLOUD_SUPPORT
	ipsec_lid_inited = 0;
	ipsec_rid_inited = 0;
	#endif
	ipsec_key_inited = 0;
	ipsecadv_config_inited = 0;

	return ret;
}

int ej_ipsec_show_summary(int eid, webs_t wp, int argc, char_t **argv)
{
	int i;	
	char temp[100];		
	char tunnel_status_l[3],tunnel_name_l[30],local_type_l[3],localgw_conn_l[3],remote_type_l[3],enc_type_l[3],auth_type_l[3],l_ipaddr_l[20],l_netmask_l[20],r_ipaddr_l[20],r_netmask_l[20],sg_domain_l[40],sg_ipaddr_l[20],key_type_l[3],sg_type_l[3],*auto_pfs_l = NULL,*auto_presh_l;
	//zhangbin 2004.11.24
	char s_status[20];
	
	for(i = 0 ; i < IPSEC_TUNNELS ; i++)
	{
		int which1;
		char word[80],word1[80],word2[80],word3[80],word4[200],*next,*next1,*next2,*next3,*next4;
		which1 = i;
		
//		printf("enter tunnel\n");
		foreach(word, nvram_safe_get("entry_config"), next) 
		{
//			printf("which1=%d\n",which1);
			if (which1-- == 0) 
			{
				sscanf(word, "%3[^:]:%25[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tunnel_status_l,tunnel_name_l,local_type_l, localgw_conn_l,remote_type_l, sg_type_l, enc_type_l, auth_type_l, key_type_l);

				break;
			}
		}
			if(!strcmp(tunnel_status_l,"1"))
			{
				int which2;
				which2 = i;
				foreach(word1, nvram_safe_get("ltype_config"), next1) 
				{
					if(which2-- == 0) 
					{
						sscanf(word1, "%20[^:]:%20[^\n]", l_ipaddr_l,l_netmask_l);			
						break;
					}
				}
				which2 = i;

				foreach(word2, nvram_safe_get("rtype_config"), next2) 
				{
					if (which2-- == 0) 
					{
						sscanf(word2, "%20[^:]:%20[^\n]", r_ipaddr_l,r_netmask_l);
						break;
					}
				}
				which2 = i;
				foreach(word3, nvram_safe_get("sg_config"), next3) 
				{
					if (which2-- == 0) 
					{
						sscanf(word3, "%20[^:]:%50[^\n]", sg_ipaddr_l,sg_domain_l);
						break;
					}
				}
				which2 = i;
				foreach(word4, nvram_safe_get("keytype_config"),next4)
				{
					if (which2-- == 0) 
					{
						auto_presh_l = word4;	
						auto_pfs_l = strsep(&auto_presh_l, ":");
						break;
					}
				}
			//zhangbin 2004.11.24	
				{
					FILE *sfile;
					char sbuf[5];
					char sfile_name[50];

					memset(sfile_name,0,sizeof(sfile_name));
					memset(s_status,0,sizeof(s_status));
					memset(sbuf,0,sizeof(sbuf));

					if(!strcmp(key_type_l,"1"))
					{
						/* Don't display status
						 * in Manual key
						 *  ----zhangbin 2005.7.1 */
						//strcpy(s_status,"Connected");
						strcpy(s_status," ");
					}
					else
					{
						sprintf(sfile_name,"/tmp/ipsecst/%s",tunnel_name_l);
	
						if((sfile = fopen(sfile_name,"r")) == NULL)
						{
							printf("cannot open %s !\n",sfile_name);
							strcpy(s_status,"Disconnected");
						}
						else
						{
							fread(sbuf,1,1,sfile) ;
							sbuf[1] = '\0';
							printf("sbuf=%s!\n",sbuf);
							fclose(sfile);
	
							if(!strcmp(sbuf,"1"))
								strcpy(s_status,"Connected");
							else
								strcpy(s_status,"Disconnected");
						}
					}

				}

				sprintf(temp,"<tr bgcolor=cccccc align=middle><td><font face=verdana size=1.5>%d</font></td>",i+1);
				websWrite(wp,"%s",temp);
				sprintf(temp,"<td><font face=verdana size=1.5>%s</font></td>",tunnel_name_l);
				websWrite(wp,"%s",temp);
				if(!strcmp(s_status,"Connected"))
					sprintf( temp, "<td><font face=verdana size=1.5 color=red>Connected</font></td>");
				else
					sprintf( temp, "<td><font face=verdana size=1.5 color=red>Disconnected</font></td>");
				websWrite(wp,"%s",temp);
				if(!strcmp(local_type_l,"0"))
					sprintf( temp, "<td><font face=verdana size=1.5>%s</font></td>",l_ipaddr_l);
				else
					sprintf( temp, "<td><font face=verdana size=1.5>%s<br>%s</font></td>",l_ipaddr_l,l_netmask_l);
				websWrite(wp,"%s",temp);
				
				if(!strcmp(local_type_l,"0"))
					sprintf( temp, "<td><font face=verdana size=1.5>%s</font></td>",r_ipaddr_l);
				else if(!strcmp(local_type_l,"1"))
					sprintf( temp, "<td><font face=verdana size=1.5>%s<br>%s</font></td>",r_ipaddr_l,r_netmask_l);
				else if(!strcmp(local_type_l,"3"))
					sprintf( temp, "<td><font face=verdana size=1.5>Any</font></td>");
				else if(!strcmp(local_type_l,"4"))
					sprintf( temp, "<td><font face=verdana size=1.5>Host</font></td>");
				websWrite(wp,"%s",temp);
				
				if(!strcmp(sg_type_l,"0"))
					sprintf( temp, "<td><font face=verdana size=1.5>%s</font></td>",sg_ipaddr_l);
				else if(!strcmp(sg_type_l,"1"))
					sprintf( temp, "<td><font face=verdana size=1.5>Any</font></td>");
				else if(!strcmp(sg_type_l,"2"))
					sprintf( temp, "<td><font face=verdana size=1.5>%s</font></td>",sg_domain_l);
				websWrite(wp,"%s",temp);
				
				sprintf( temp, "<td><font face=verdana size=1.5>");
				if(!strcmp(enc_type_l,"0"))
					strcat(temp,"NULL");
				else if(!strcmp(enc_type_l,"1"))
					strcat(temp,"DES");
				else if(!strcmp(enc_type_l,"2"))
					strcat(temp,"3DES");
				else if(!strcmp(enc_type_l,"3"))
					strcat(temp,"AES");
				
				if(!strcmp(auth_type_l,"1"))
					strcat(temp," MD5");
				else if(!strcmp(auth_type_l,"2"))
					strcat(temp," SHA");
				if(!strcmp(key_type_l,"0"))
				{
					strcat(temp," ISAKMP");
					if(!strcmp(auto_pfs_l,"1"))
						strcat(temp," PFS");
				}
				else
					strcat(temp," MANUAL");
					
				strcat(temp, "</font></td>");
				websWrite(wp,"%s",temp);
				sprintf(temp,"%s","");
				
			}
		
	}
	return 0;
}
