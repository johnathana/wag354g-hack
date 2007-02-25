
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

#include <cy_conf.h>
#include <code_pattern.h>
#include "broadcom.h"
#include "pvc.h"

//#define LJZ_DEBUG

char connection[5];  // seleted connetion index (global)
int connchanged = 0;//connection change flag

char vcc_config_buf[60];

//junzhao 2005.5.16 for ':' setting
char *pppoe_config_buf = NULL;
char *pppoa_config_buf = NULL;

char bridged_config_buf[100];
char routed_config_buf[100];

#ifdef CLIP_SUPPORT
char clip_config_buf[100];
#endif

#ifdef MPPPOE_SUPPORT
//junzhao 2005.5.16 for ':' setting
char *mpppoe_config_buf = NULL;
#endif

#ifdef IPPPOE_SUPPORT
//junzhao 2005.5.16 for ':' setting
char *ipppoe_config_buf = NULL;
#endif

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
//junzhao 2005.5.16 for ':' setting
char *unip_pppoe_config_buf = NULL;
char *unip_pppoa_config_buf = NULL;
#endif

int vcc_inited;
int pppoa_inited;
int pppoe_inited;
int bridged_inited;
int routed_inited;

#ifdef CLIP_SUPPORT
int clip_inited;
#endif
#ifdef MPPPOE_SUPPORT
int mpppoe_inited;
#endif

#ifdef IPPPOE_SUPPORT
int ipppoe_inited;
#endif

/* kirby  */
#ifdef  UNNUMBERED_SUPPORT
int unip_pppoa_inited;
int unip_pppoe_inited;
#endif


//junzhao add 2004.3.10
char vpi_tmp[5];
char vci_tmp[8];
//char multiplex_tmp[5];
char qostype_tmp[8];
char pcr_tmp[8];
char scr_tmp[8];
char autodetect_tmp[3];
char applyonboot_tmp[3];

/*
char username_tmp[64];
char password_tmp[64];
char servicename_tmp[64];
char idletime_tmp[8];
char redialperiod_tmp[8];
char demand_tmp[3];
*/
extern int clone_wan_mac;

extern int error_value;

//junzhao 2005.5.16 for ':' setting
void final_free_buf()
{
	if(pppoe_config_buf)
	{
		free(pppoe_config_buf);
		pppoe_config_buf = NULL;
	}
	if(pppoa_config_buf)
	{
		free(pppoa_config_buf);
		pppoa_config_buf = NULL;
	}
#ifdef MPPPOE_SUPPORT
	if(mpppoe_config_buf)
	{
		free(mpppoe_config_buf);
		mpppoe_config_buf = NULL;
	}
#endif
#ifdef IPPPOE_SUPPORT
	if(ipppoe_config_buf)
	{
		free(ipppoe_config_buf);
		ipppoe_config_buf = NULL;
	}
#endif
#ifdef  UNNUMBERED_SUPPORT
	if(unip_pppoe_config_buf)
	{
		free(unip_pppoe_config_buf);
		unip_pppoe_config_buf = NULL;
	}
	if(unip_pppoa_config_buf)
	{
		free(unip_pppoa_config_buf);
		unip_pppoa_config_buf = NULL;
	}
#endif
	return;
}

//junzhao add 2004.3.10
int save_vcc_tmpbuf(webs_t wp)
{
	memset(vpi_tmp, 0, sizeof(vpi_tmp));
	memset(vci_tmp, 0, sizeof(vci_tmp));
	//memset(multiplex_tmp, 0, sizeof(multiplex_tmp));
	memset(qostype_tmp, 0, sizeof(qostype_tmp));
	memset(pcr_tmp, 0, sizeof(pcr_tmp));
	memset(scr_tmp, 0, sizeof(scr_tmp));
	memset(autodetect_tmp, 0, sizeof(autodetect_tmp));
	memset(applyonboot_tmp, 0, sizeof(applyonboot_tmp));

	strncpy(vpi_tmp, websGetVar(wp, "wan_vpi", "0"), sizeof(vpi_tmp));
	strncpy(vci_tmp, websGetVar(wp, "wan_vci", "0"), sizeof(vci_tmp));
	//strncpy(multiplex_tmp, websGetVar(wp, "wan_multiplex", "llc"), sizeof(multiplex_tmp));
	strncpy(qostype_tmp, websGetVar(wp, "wan_qostype", "ubr"), sizeof(qostype_tmp));
	strncpy(pcr_tmp, websGetVar(wp, "wan_pcr", "0"), sizeof(pcr_tmp));
	strncpy(scr_tmp, websGetVar(wp, "wan_scr", "0"), sizeof(scr_tmp));
	strncpy(autodetect_tmp, websGetVar(wp, "wan_autodetect", "0"), sizeof(autodetect_tmp));
	strncpy(applyonboot_tmp, websGetVar(wp, "wan_applyonboot", "0"), sizeof(applyonboot_tmp));
	//zhangbin 
#ifdef DNSPROXY_BUTTON_SUPPORT
	nvram_set("dns_proxy","1");
#endif
	return 0;
}	

/*
int save_vcc_pppoe_tmpbuf(webs_t wp)
{
	char *encapmode = websGetVar(wp, "wan_encapmode", "");
	save_vcc_tmpbuf(wp);
	if(!strcmp(encapmode, "pppoe"))
	{
		memset(servicename_tmp, 0, sizeof(servicename_tmp));
		memset(username_tmp, 0, sizeof(username_tmp));
		memset(password_tmp, 0, sizeof(password_tmp));
		memset(demand_tmp, 0, sizeof(demand_tmp));
		memset(idletime_tmp, 0, sizeof(idletime_tmp));
		memset(redialperiod_tmp, 0, sizeof(redialperiod_tmp));
		strncpy(servicename_tmp, websGetVar(wp, "pppoe_servicename", ""), sizeof(servicename_tmp));
		strncpy(username_tmp, websGetVar(wp, "pppoe_username", ""), sizeof(username_tmp));
		strncpy(password_tmp, websGetVar(wp, "pppoe_password", ""), sizeof(password_tmp));
		strncpy(demand_tmp, websGetVar(wp, "pppoe_demand", ""), sizeof(demand_tmp));
		strncpy(idletime_tmp, websGetVar(wp, "pppoe_idletime", ""), sizeof(idletime_tmp));
		strncpy(redialperiod_tmp, websGetVar(wp, "pppoe_redialperiod", ""), sizeof(redialperiod_tmp));
	}
	return 0;
}	
*/

int ram_modify_status(char *filename, char *str)
{
	int i, which_conn;
        char word[64], *next;
        char buf[512], *cur=buf;
	char readbuf[512];
        char old[PVC_CONNECTIONS][60];
	
	which_conn = atoi(connection);
	memset(word, 0, sizeof(word));
        memset(buf, 0, sizeof(buf));
	
        for(i=0; i<PVC_CONNECTIONS; i++)
                memset(old[i], 0, sizeof(old[i]));

	if(!file_to_buf(filename, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", filename);
		return 0;
	}
	
        i=0;
        foreach(word, readbuf, next)
        {
                strcpy(old[i], word);
                i++;
        }
        snprintf(old[which_conn], sizeof(old[0]), "%s", str);
	
	for(i=0 ; i<PVC_CONNECTIONS; i++)
        {
                if(strcmp(old[i],""))
                        cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
                                cur == buf ? "" : " ", old[i]);
        }

#ifdef LJZ_DEBUG
	printf("filename %s commit buf=%s\n", filename, buf);
#endif	

	if(!buf_to_file(filename, buf))
		return 0;
	
	return 1;
}

void nvram_modify_status(char *type, char *str)
{
	int i, which_conn;
	char word[150], *next;
	char buf[1200], *cur=buf;
	char old[PVC_CONNECTIONS][150];

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));

	which_conn = atoi(connection);
	for(i=0; i<PVC_CONNECTIONS; i++)
		memset(old[i], 0, sizeof(old[i]));

	i=0;
	foreach(word, nvram_safe_get(type), next)
	{								                	strcpy(old[i], word);
		i++;									}
	snprintf(old[which_conn], sizeof(old[0]), "%s", str);
						
	for(i=0 ; i<PVC_CONNECTIONS; i++)
        {
                if(strcmp(old[i],""))
	        	cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
        }

#ifdef LJZ_DEBUG
        printf("in httpd: type %s ----- commit buf=%s\n", type, buf);
#endif

        nvram_set(type, buf);
 	//nvram_commit();
        return;
}

int
ej_check_proto_fordns(int eid, webs_t wp, int argc, char_t **argv)
{
	//int count;
	char *wan_encapmode;
	/*
        if (ejArgs(argc, argv, "%d", &count) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	*/
	if(gozila_action && !connchanged)
	{
		wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
	}
	else
	{
		wan_encapmode = encapmode;
		connchanged = 0;
	}
	
	if(!strcmp(wan_encapmode, "routed"))
	{
		/*
		if(count == 1)
			return websWrite(wp, "%s", "<!--");
		else if(count == 2)
			return websWrite(wp, "%s", "-->");
		*/
		return websWrite(wp, "%s", "disabled");
	}
	
#ifdef CLIP_SUPPORT
	if(!strcmp(wan_encapmode, "clip"))
	{
		/*
		if(count == 1)
			return websWrite(wp, "%s", "<!--");
		else if(count == 2)
			return websWrite(wp, "%s", "-->");
		*/
		return websWrite(wp, "%s", "disabled");
	}
#endif
	
	if(!strcmp(wan_encapmode, "1483bridged"))
	{
		if(!strcmp(dhcpenable, "0"))
		{
			/*
			if(count == 1)
				return websWrite(wp, "%s", "<!--");
			else if(count == 2)
				return websWrite(wp, "%s", "-->");
			*/
			return websWrite(wp, "%s", "disabled");
		}	
	}
	return 0;
}	

int
ej_connection_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type;

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "wan_connection", NULL);
		printf("connection %s\n", connection);
		printf("tmpconn %s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(connection, tmpconn)))
		{
			printf("not the same conn\n");
			strncpy(connection, tmpconn, sizeof(connection));
			connchanged = 1; //make the flag of change conn
			vcc_inited = 0;
			pppoa_inited = 0;
			pppoe_inited = 0;
			bridged_inited = 0;
			routed_inited = 0;
		#ifdef CLIP_SUPPORT
			clip_inited = 0;
		#endif	
		#ifdef MPPPOE_SUPPORT
			mpppoe_inited = 0;
		#endif
		#ifdef IPPPOE_SUPPORT
			ipppoe_inited = 0;
		#endif
	
			/* kirby for unip 2004/11.25 */
		#ifdef UNNUMBERED_SUPPORT 
		      unip_pppoe_inited = 0;
		      unip_pppoa_inited = 0;
		#endif

		}
		//junzhao 2004.4.1
		else
			save_vcc_tmpbuf(wp);
	}		
	else //first appear
	{
		strncpy(connection, nvram_safe_get("wan_connection"), sizeof(connection));
		vcc_inited = 0;
		pppoa_inited = 0;
		pppoe_inited = 0;
		bridged_inited = 0;
		routed_inited = 0;
	#ifdef CLIP_SUPPORT
		clip_inited = 0;
	#endif
	#ifdef MPPPOE_SUPPORT
		mpppoe_inited = 0;
	#endif
	#ifdef IPPPOE_SUPPORT
		ipppoe_inited = 0;
	#endif
	
		/* kirby for unip 2004/11.25 */
	#ifdef UNNUMBERED_SUPPORT 
	      unip_pppoe_inited = 0;
	      unip_pppoa_inited = 0;
	#endif

	}

	which = atoi(connection);

	if(!strcmp(type,"select"))
	{
	#ifdef MULTIPVC_SUPPORT
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
	#else
		for(i=0 ; i<1 ; i++)
	#endif
		//for(i=0 ; i < PVC_CONNECTIONS ; i++)
		//single pvc
		//for(i=0 ; i < 1 ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	}

	return ret;
}

//add by lzh;
int 
ej_pvc_connection_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type;
	char cur_connect[5];


        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "pvc_connection", NULL);
		strncpy(cur_connect, tmpconn, sizeof(cur_connect));
		cprintf("current single port forward connection:%s\n",cur_connect);
		if (atoi(cur_connect) < PVC_CONNECTIONS)
		{
			nvram_set("singleportforward_connection",cur_connect);
			nvram_commit();
		}	
	}		
	else //first appear
	{
#if 0 
		strncpy(cur_connect, nvram_safe_get("upnp_active_connection"), sizeof(cur_connect));
		nvram_set("singleportforward_connection",cur_connect);
#else
		strncpy(cur_connect, nvram_safe_get("singleportforward_connection"),sizeof(cur_connect));
#endif
	}

	which = atoi(cur_connect);
	if(!strcmp(type,"select"))
	{
	#ifdef MULTIPVC_SUPPORT
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
	#else
		for(i=0 ; i<1 ; i++)
	#endif
		//for(i=0 ; i < PVC_CONNECTIONS ; i++)
		//single pvc
		//for(i=0 ; i < 1 ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	}

	return ret;
}

int 
ej_upnp_connection_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0, which;
	char *type;
	char cur_connect[5];


        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "upnp_pvc_connection", NULL);
		strncpy(cur_connect, tmpconn, sizeof(cur_connect));
		cprintf("select upnp connection:%s\n",cur_connect);
		if (atoi(cur_connect) < PVC_CONNECTIONS)
		{
			nvram_set("upnp_active_connection",cur_connect);
			nvram_commit();
		}
	}		
	else //first appear
	{
		strncpy(cur_connect,nvram_safe_get("upnp_active_connection"),sizeof(cur_connect));
	}

	which = atoi(cur_connect);
	if(!strcmp(type,"select"))
	{
	#ifdef MULTIPVC_SUPPORT
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
	#else
		for(i=0 ; i<1 ; i++)
	#endif
		//for(i=0 ; i < PVC_CONNECTIONS ; i++)
		//single pvc
		//for(i=0 ; i < 1 ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	}

	return ret;
}
//end add by lzh;

int
ej_vpivci_getvalue(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0;
//	char *which_conn;
	char origin_conn[5];
	int currentconn;
	char word[32], readbuf[256], *next, *errstr=NULL;

/*
        if (ejArgs(argc, argv, "%s", &which_conn) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
*/
	memset(origin_conn, 0, sizeof(origin_conn));
	strncpy(origin_conn, connection, sizeof(origin_conn));
	
	if(gozila_action)
		currentconn =  atoi(websGetVar(wp, "wan_connection", "0"));
	else
		currentconn =  atoi(nvram_safe_get("wan_connection"));
        
	if(!file_to_buf(WAN_PVC_ERRSTR, readbuf, sizeof(readbuf)))
        {
               	printf("no buf in %s!\n", WAN_PVC_ERRSTR);
                return 0;
        }
	//file_to_buf(WAN_PVC_ERRSTR, readbuf, sizeof(readbuf));

	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		int which = i;
		if(i == currentconn)
		{
			ret = websWrite(wp, "\tvpivci_pairs[%d] = %s.%s;\n", i, "0", "0");
			continue;
		}
		memset(connection, 0, sizeof(connection));
		sprintf(connection, "%d", i);
		vcc_config_init();
		
		if(!strcmp(applyonboot, "0"))
		{
			ret = websWrite(wp, "\tvpivci_pairs[%d] = %s.%s;\n", i, "0", "0");
			continue;
		}
		
		foreach(word, readbuf, next)
		{
			if(which-- == 0)
			{
				errstr = word;
				break;
			}
		}
		
		if(!strcmp(errstr, "saved") || !strcmp(errstr, "OK"))
			ret = websWrite(wp, "\tvpivci_pairs[%d] = %s.%s;\n", i, vpi, vci);
		else
			ret = websWrite(wp, "\tvpivci_pairs[%d] = %s.%s;\n", i, "0", "0");
	}
	strncpy(connection, origin_conn, sizeof(connection));
	vcc_config_init();	
	return ret;
}
	
void vcc_config_init()
{
	char *next;
	int which;
	
	memset(vcc_config_buf, 0, sizeof(vcc_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);
	
	foreach(vcc_config_buf, nvram_safe_get("vcc_config"), next) 
	{
		if (which-- == 0) 
		{
			vci = vcc_config_buf;
			vpi = strsep(&vci, ":");
			if (!vpi || !vci)
				break;
			encapmode = vci;
			vci = strsep(&encapmode, ":");
			if (!vci || !encapmode)
				break;
			multiplex = encapmode;
			encapmode = strsep(&multiplex, ":");
			if (!encapmode || !multiplex)
				break;
			qos = multiplex;
			multiplex = strsep(&qos, ":");
			if (!multiplex || !qos)
				break;
			pcr = qos;
			qos = strsep(&pcr, ":");
			if (!qos || !pcr)
				break;
			scr = pcr;
			pcr = strsep(&scr, ":");
			if (!pcr || !scr)
				break;
			autodetect = scr;
			scr = strsep(&autodetect, ":");
			if (!scr || !autodetect)
				break;
			applyonboot = autodetect;
			autodetect = strsep(&applyonboot, ":");
			if (!autodetect || !applyonboot)
				break;

		#ifdef LJZ_DEBUG
			printf("vpi=%s, vci=%s, encapmode=%s, multiplex=%s, qos=%s, pcr=%s, scr=%s, autodetect=%s, applyonboot=%s\n", vpi, vci, encapmode, multiplex, qos, pcr, scr, autodetect, applyonboot);
		#endif 
			break;
		}
	}
	vcc_inited = 1;
}	
/*
 * Example: 
 * vcc_config = vpi:vci:encapmode:multiplex:qos:pcr:scr:autodetect
 * vcc_config = 8:35:1483bridged:llc:ubr:0:0:1
 * <% vcc_config("vpi", 0); %> produces "8"
 * <% vcc_config("encapmode", 0); %> produces "selected" if vcc_config.encapmode = 1483bridged 
 * <% vcc_config("multiplex", 1); %> produces "checked" if vcc_config.multiplexing = vc
 * <% vcc_config("pcr", 1); %> produces "disabled" if vcc_config.qos = ubr
 * <% vcc_config("pcr", 0); %> produces "0" if vcc_config.qos = ubr
 */
int ej_vcc_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(vcc_inited == 0)
		vcc_config_init();	
	
	if(!strcmp(arg, "vpi"))
	{
		char *wan_encapmode;
		char *wan_autodetect;
		char *wan_vpi;
		char *wan_connection;
		char connection_tmp[3];
		//add by junzhao 2004.3.10
		if(gozila_action && !connchanged)
		{
			wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
			wan_autodetect = autodetect_tmp;
			//junzhao 2004.4.1
			wan_connection = websGetVar(wp, "wan_connection", NULL);
			//add by junzhao 2004.3.15
			if(!strcmp(vpi_tmp, "0") && !strcmp(vci_tmp, "0"))
				wan_vpi = vpi;
			else
				wan_vpi = vpi_tmp;
		}
		else
		{
			wan_encapmode = encapmode;
			wan_autodetect = autodetect;
			wan_vpi = vpi;
			//junzhao 2004.4.1
			memset(connection_tmp, 0, sizeof(connection_tmp));
			strncpy(connection_tmp, nvram_safe_get("wan_connection"), sizeof(connection_tmp));
			wan_connection = connection_tmp;
		}
		
		if(strcmp(wan_encapmode, "bridgedonly"))
		{
			if(!strcmp(wan_autodetect, "1"))
				//return websWrite(wp, "%s disabled", vpi);
				//add by junzhao 2004.3.10
				return websWrite(wp, "%s disabled", wan_vpi);
			else if(!strcmp(wan_autodetect, "0"))
				//return websWrite(wp, "%s", vpi);
				//add by junzhao 2004.3.10
				return websWrite(wp, "%s", wan_vpi);
		}
		else
		{
			//return websWrite(wp, "%s", vpi);
			if(!strcmp(wan_autodetect, "0"))
			{
				//add by junzhao 2004.4.14
				if(gozila_action && !connchanged)
				{
					int whichconnection = atoi(wan_connection);
					switch(whichconnection)
					{
						case 0:
						#if RELCOUN == GERMANY
							return websWrite(wp, "1");
						#endif
						case 2:
						case 3:
						case 4:
							return websWrite(wp, "0");
						default:	
							return websWrite(wp, "8");
					}
				}
				else
					return websWrite(wp, "%s", wan_vpi);
			}
			// junzhao 2004.4.1
			/*
			if(!strcmp(wan_autodetect, "0"))
			{
				int whichconnection = atoi(wan_connection);
				switch(whichconnection)
				{
					case 0:
					case 2:
					case 3:
					case 4:
						return websWrite(wp, "0");
					default:	
						return websWrite(wp, "8");
				}
			}
			*/
			else
				return websWrite(wp, "%s", "0");
		}
	}
	if(!strcmp(arg, "vci"))
	{
		char *wan_autodetect;
		char *wan_encapmode;
		//add by junzhao 2004.3.10
		char *wan_vci;
		char *wan_connection;
		char connection_tmp[3];
		if(gozila_action && !connchanged)
		{
			wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
			wan_autodetect = autodetect_tmp;
			//junzhao 2004.4.1
			wan_connection = websGetVar(wp, "wan_connection", NULL);
			//add by junzhao 2004.3.15
			if(!strcmp(vpi_tmp, "0") && !strcmp(vci_tmp, "0"))
				wan_vci = vci;
			else
				wan_vci = vci_tmp;
			//wan_vci = vci_tmp;
		}
		else
		{
			wan_encapmode = encapmode;
			wan_autodetect = autodetect;
			wan_vci = vci;
			//junzhao 2004.4.1
			memset(connection_tmp, 0, sizeof(connection_tmp));
			strncpy(connection_tmp, nvram_safe_get("wan_connection"), sizeof(connection_tmp));
			wan_connection = connection_tmp;
		}
		if(strcmp(wan_encapmode, "bridgedonly"))
		{
			if(!strcmp(wan_autodetect, "1"))
				//return websWrite(wp, "%s disabled", vci);
				return websWrite(wp, "%s disabled", wan_vci);
			else if(!strcmp(wan_autodetect, "0"))
				//return websWrite(wp, "%s", vci);
				return websWrite(wp, "%s", wan_vci);
		}
		else
		{
			//return websWrite(wp, "%s", vci);
			if(!strcmp(wan_autodetect, "0"))
			{
				//junzhao 2004.4.14
				if(gozila_action && !connchanged)
				{
					int whichconnection = atoi(wan_connection);
					switch(whichconnection)
					{
						case 0:
						#if RELCOUN == GERMANY
							return websWrite(wp, "32");
						#endif
						case 1:
							return websWrite(wp, "35");
						case 2:
						case 5:
							return websWrite(wp, "43");
						case 3:	
						case 6:	
							return websWrite(wp, "51");
						case 4:	
						case 7:	
							return websWrite(wp, "59");
					}
				}			
				else	
					return websWrite(wp, "%s", wan_vci);
			}
			// junzhao 2004.4.1
			/*
			if(!strcmp(wan_autodetect, "0"))
			{
				int whichconnection = atoi(wan_connection);
				switch(whichconnection)
				{
					case 0:
					case 1:
						return websWrite(wp, "35");
					case 2:
					case 5:
						return websWrite(wp, "43");
					case 3:	
					case 6:	
						return websWrite(wp, "51");
					case 4:	
					case 7:	
						return websWrite(wp, "59");
				}
			}
			*/
			else
				return websWrite(wp, "%s", "0");
		}
	}
	if(!strcmp(arg, "encapmode"))
	{
		char *wan_encapmode;
		if(gozila_action && !connchanged)
			wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
		else
			wan_encapmode = encapmode;
		
		if(count != 6)
		{
		#ifndef CLIP_SUPPORT
			if(((count == BRIDGED) && (!strcmp(wan_encapmode, "1483bridged"))) || ((count == ROUTED) && (!strcmp(wan_encapmode, "routed"))) || ((count == PPPOE) && (!strcmp(wan_encapmode, "pppoe"))) || ((count == PPPOA) && (!strcmp(wan_encapmode, "pppoa"))) || ((count == PPTP) && (!strcmp(wan_encapmode, "pptp"))) || ((count == BRIDGEDONLY) && (!strcmp(wan_encapmode, "bridgedonly"))))
		#else
	 		/* kirby for unip 2004/11.25 */
			//if(((count == BRIDGED) && (!strcmp(wan_encapmode, "1483bridged"))) || ((count == ROUTED) && (!strcmp(wan_encapmode, "routed"))) || ((count == PPPOE) && (!strcmp(wan_encapmode, "pppoe"))) || ((count == PPPOA) && (!strcmp(wan_encapmode, "pppoa"))) || ((count == PPTP) && (!strcmp(wan_encapmode, "pptp"))) || ((count == BRIDGEDONLY) && (!strcmp(wan_encapmode, "bridgedonly"))) || ((count == CLIP) && (!strcmp(wan_encapmode, "clip"))))
#ifdef UNNUMBERED_SUPPORT
			if(((count == BRIDGED) && (!strcmp(wan_encapmode, "1483bridged"))) || ((count == ROUTED) && (!strcmp(wan_encapmode, "routed"))) || ((count == PPPOE) && (!strcmp(wan_encapmode, "pppoe"))) || ((count == PPPOA) && (!strcmp(wan_encapmode, "pppoa"))) || ((count == PPTP) && (!strcmp(wan_encapmode, "pptp"))) || ((count == BRIDGEDONLY) && (!strcmp(wan_encapmode, "bridgedonly"))) || ((count == CLIP) && (!strcmp(wan_encapmode, "clip")))  || ((count == UNIP_PPPOE) && (!strcmp(wan_encapmode, "unpppoe"))) || ((count == UNIP_PPPOA) && (!strcmp(wan_encapmode, "unpppoa"))))	
#else

		if(((count == BRIDGED) && (!strcmp(wan_encapmode, "1483bridged"))) || ((count == ROUTED) && (!strcmp(wan_encapmode, "routed"))) || ((count == PPPOE) && (!strcmp(wan_encapmode, "pppoe"))) || ((count == PPPOA) && (!strcmp(wan_encapmode, "pppoa"))) || ((count == PPTP) && (!strcmp(wan_encapmode, "pptp"))) || ((count == BRIDGEDONLY) && (!strcmp(wan_encapmode, "bridgedonly"))) || ((count == CLIP) && (!strcmp(wan_encapmode, "clip"))))
#endif
		#endif
				return websWrite(wp, "selected");
		}
		else 
			return websWrite(wp, "%s", wan_encapmode);		
	}
	if(!strcmp(arg, "multiplex"))
	{
		//add by junzhao 2004.3.10
		char *wan_multiplex;
		if(gozila_action && !connchanged)
		//junzhao 2004.3.22
		{
			char *wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
			//char tmpword[20], *next, *saved;
			int which;
			if(!strcmp(wan_encapmode, "pppoa"))
			{
				which = atoi(connection);
				wan_multiplex = "vc";
				/*
				foreach(tmpword, nvram_safe_get("pppoa_saved"), next) 
				{
					if(which-- == 0) 
					{
						saved = tmpword;
						break;
					}
				}
				
				//if(!strcmp(saved, "1"))	
				//	wan_multiplex = multiplex;
				//else
					wan_multiplex = "vc";*/
			}
			else 
			{
				which = atoi(connection);
				wan_multiplex = "llc";
				/*foreach(tmpword, nvram_safe_get("fourtypes_saved"), next) 
				{
					if(which-- == 0) 
					{
						saved = tmpword;
						break;
					}
				}
				if(!strcmp(saved, "1"))	
					wan_multiplex = multiplex;
				else
					wan_multiplex = "llc";*/
			}
		}
		else
			wan_multiplex = multiplex;
		//if(((count == LLC) && (!strcmp(multiplex, "llc"))) || ((count == VC) && (!strcmp(multiplex, "vc"))))
		if(((count == LLC) && (!strcmp(wan_multiplex, "llc"))) || ((count == VC) && (!strcmp(wan_multiplex, "vc"))))
			return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "qos"))
	{	
		//add by junzhao 2004.3.10
		char *wan_qos;
		if(gozila_action && !connchanged)
			wan_qos = qostype_tmp;
		else
			wan_qos = qos;
		//if(((count == UBR) && (!strcmp(qos, "ubr"))) || ((count == CBR) && (!strcmp(qos, "cbr"))) || ((count == VBR) && (!strcmp(qos, "vbr"))))  
		if(((count == UBR) && (!strcmp(wan_qos, "ubr"))) || ((count == CBR) && (!strcmp(wan_qos, "cbr"))) || ((count == VBR) && (!strcmp(wan_qos, "vbr"))))  
			return websWrite(wp, "selected");
		}
	if(!strcmp(arg, "pcr"))
	{
		//add by junzhao 2004.3.10
		char *wan_qos;
		char *wan_pcr;
		if(gozila_action && !connchanged)
		{
			wan_qos = qostype_tmp;
			wan_pcr = pcr_tmp;
		}
		else
		{
			wan_qos = qos;
			wan_pcr = pcr;
		}
		if(count == 0)
		{	
			//if(strcmp(qos, "ubr"))
			if(strcmp(wan_qos, "ubr"))
				//return websWrite(wp, "%s", pcr);
				return websWrite(wp, "%s", wan_pcr);
			else
				return websWrite(wp, "%s", "0");
		}
		else if(count == 1)	
		{
			//if(!strcmp(qos, "ubr"))
			if(!strcmp(wan_qos, "ubr"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "scr"))
	{	
		//add by junzhao 2004.3.10
		char *wan_qos;
		char *wan_scr;
		if(gozila_action && !connchanged)
		{
			wan_qos = qostype_tmp;
			wan_scr = scr_tmp;
		}
		else
		{
			wan_qos = qos;
			wan_scr = scr;
		}
	
		if(count == 0)
		{
			//if(!strcmp(qos, "vbr"))
			if(!strcmp(wan_qos, "vbr"))
				//return websWrite(wp, "%s", scr);
				return websWrite(wp, "%s", wan_scr);
			else
				return websWrite(wp, "%s", "0");
		}
		else if(count == 1)
		{
			//if(strcmp(qos, "vbr"))
			if(strcmp(wan_qos, "vbr"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "autodetect"))
	{	
		//add by junzhao 2004.3.10
		char *wan_autodetect;
		char *wan_encapmode;
		if(gozila_action && !connchanged)
		{
			wan_autodetect = autodetect_tmp;
			wan_encapmode = websGetVar(wp, "wan_encapmode", NULL);
		}
		else
		{
			wan_autodetect = autodetect;
			wan_encapmode = encapmode;
		}
		
		if(!strcmp(wan_encapmode, "bridgedonly"))
		{
			/*
			if(((count == AUTOENABLE) && (!strcmp(autodetect, "1"))) || ((count == AUTODISABLE) && (!strcmp(autodetect, "0"))))
				return websWrite(wp, "checked disabled");
			else
				return websWrite(wp, "disabled");
				*/
			if(count == AUTOENABLE)
				return websWrite(wp, "disabled");
			else if(count == AUTODISABLE)
				return websWrite(wp, "checked disabled");
		}
		else
		{
			//if(((count == AUTOENABLE) && (!strcmp(autodetect, "1"))) || ((count == AUTODISABLE) && (!strcmp(autodetect, "0"))))
			if(((count == AUTOENABLE) && (!strcmp(wan_autodetect, "1"))) || ((count == AUTODISABLE) && (!strcmp(wan_autodetect, "0"))))
				return websWrite(wp, "checked");
		}
	}
	if(!strcmp(arg, "applyonboot"))
	{
		char *wan_applyonboot;
		if(gozila_action && !connchanged)
			wan_applyonboot = applyonboot_tmp;
		else
			wan_applyonboot = applyonboot;
	
		//if(!strcmp(applyonboot, "1"))
		if(!strcmp(wan_applyonboot, "1"))
			return websWrite(wp, "checked");
	}
	return -1;
}

void pppoa_config_init()
{
	char *ptr = nvram_safe_get("pppoa_config"), *tmp;
	int which, i;
	
	//memset(pppoa_config_buf, 0, sizeof(pppoa_config_buf));
	
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(pppoa_config_buf, nvram_safe_get("pppoa_config"), next) 
	{
		if (which-- == 0) 
		{
#endif
			
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(pppoa_config_buf)
				free(pppoa_config_buf);
			pppoa_config_buf = strdup(tmp);

			a_password = pppoa_config_buf;
			a_username = strsep(&a_password, ":");
			if (!a_username || !a_password)
				break;
			a_demand = a_password;
			a_password = strsep(&a_demand, ":");
			if (!a_demand || !a_password)
				break;
			a_idletime = a_demand;
			a_demand = strsep(&a_idletime, ":");
			if (!a_demand || !a_idletime)
				break;
			a_redialperiod = a_idletime;
			a_idletime = strsep(&a_redialperiod, ":");
			if (!a_idletime || !a_redialperiod)
				break;

		#ifdef LJZ_DEBUG
			printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s\n", a_username, a_password, a_demand, a_idletime, a_redialperiod);
		#endif 
			break;
		}
	}
	pppoa_inited = 1;
}

/* kirby for unip 2005/11.25 */
#ifdef UNNUMBERED_SUPPORT
void unip_pppoa_config_init()
{
	char *ptr = nvram_safe_get("unip_pppoa_config"), *tmp;
	int which, i;
	
	//memset(unip_pppoa_config_buf, 0, sizeof(unip_pppoa_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(unip_pppoa_config_buf, nvram_safe_get("unip_pppoa_config"), next) 
	{
		if (which-- == 0) 
		{
#endif
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(unip_pppoa_config_buf)
				free(unip_pppoa_config_buf);
			unip_pppoa_config_buf = strdup(tmp);

			ua_password = unip_pppoa_config_buf;
			ua_username = strsep(&ua_password, ":");
			if (!ua_username || !ua_password)
				break;
			ua_demand = ua_password;
			ua_password = strsep(&ua_demand, ":");
			if (!ua_demand || !ua_password)
				break;
			ua_idletime = ua_demand;
			ua_demand = strsep(&ua_idletime, ":");
			if (!ua_demand || !ua_idletime)
				break;
			ua_redialperiod = ua_idletime;
			ua_idletime = strsep(&ua_redialperiod, ":");
			if (!ua_idletime || !ua_redialperiod)
				break;
			break;
		}
	}
	unip_pppoa_inited = 1;
}

/*
 * Example: 
 * pppoa_config = username:password:demand:idletime:redialperiod
 * pppoa_config = adsl:adsl1234:1:30:180
 * <% pppoa_config("username", 0); %> produces "adsl"
 */
int ej_unip_pppoa_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(unip_pppoa_inited == 0)
		unip_pppoa_config_init();

	if(!strcmp(arg, "username"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_username[64];
		memset(new_username, 0, sizeof(new_username));
		filter_name(ua_username, new_username, sizeof(new_username), GET);
		return websWrite(wp, "%s", new_username);
		//return websWrite(wp, "%s", ua_username);
	}
	if(!strcmp(arg, "password"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_password[64];
		memset(new_password, 0, sizeof(new_password));
		filter_name(ua_password, new_password, sizeof(new_password), GET);
		return websWrite(wp, "%s", new_password);
		//return websWrite(wp, "%s", ua_password);
	}
	if(!strcmp(arg, "demand"))
	{
		if(((count == DEMANDENABLE) && (!strcmp(ua_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(ua_demand, "0"))))
			return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "idletime"))
	{
		if(count == 0)
		{
			if(!strcmp(ua_demand, "1"))
				return websWrite(wp, "%s", ua_idletime);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(ua_demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		if(count == 0)
		{
			if(!strcmp(ua_demand, "0"))
				return websWrite(wp, "%s", ua_redialperiod);			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(ua_demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

void unip_pppoe_config_init()
{
	char *ptr = nvram_safe_get("unip_pppoe_config"), *tmp;
	int which, i;
	
	//memset(unip_pppoe_config_buf, 0, sizeof(unip_pppoe_config_buf));
	
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(unip_pppoe_config_buf, nvram_safe_get("unip_pppoe_config"), next) 
	{
		if (which-- == 0) 
		{
#endif
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(unip_pppoe_config_buf)
				free(unip_pppoe_config_buf);
			unip_pppoe_config_buf = strdup(tmp);

			ue_password = unip_pppoe_config_buf;
			ue_username = strsep(&ue_password, ":");
			if (!ue_username || !ue_password)
				break;
			ue_demand = ue_password;
			ue_password = strsep(&ue_demand, ":");
			if (!ue_demand || !ue_password)
				break;
			ue_idletime = ue_demand;
			ue_demand = strsep(&ue_idletime, ":");
			if (!ue_demand || !ue_idletime)
				break;
			ue_redialperiod = ue_idletime;
			ue_idletime = strsep(&ue_redialperiod, ":");
			if (!ue_idletime || !ue_redialperiod)
				break;
			ue_servicename = ue_redialperiod;
			ue_redialperiod = strsep(&ue_servicename, ":");
			if (!ue_redialperiod || !ue_servicename)
				break;

			break;
		} 
	}
	unip_pppoe_inited = 1;
}

/*
 * Example: 
 * pppoe_config = username:password:demand:idletime:redialperiod:servicename
 * pppoe_config = adsl:adsl1234:1:30:180:163gd
 * <% pppoe_config("username", 0); %> produces "adsl"
 */
int ej_unip_pppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(unip_pppoe_inited == 0)
		unip_pppoe_config_init();

	if(!strcmp(arg, "servicename"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_servicename[64];
		memset(new_servicename, 0, sizeof(new_servicename));
		filter_name(ue_servicename, new_servicename, sizeof(new_servicename), GET);
		return websWrite(wp, "%s", new_servicename);
		//return websWrite(wp, "%s", ue_servicename);
	}
	if(!strcmp(arg, "username"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_username[64];
		memset(new_username, 0, sizeof(new_username));
		filter_name(ue_username, new_username, sizeof(new_username), GET);
		return websWrite(wp, "%s", new_username);
		//return websWrite(wp, "%s", ue_username);
	}
	if(!strcmp(arg, "password"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_password[64];
		memset(new_password, 0, sizeof(new_password));
		filter_name(ue_password, new_password, sizeof(new_password), GET);
		return websWrite(wp, "%s", new_password);
		//return websWrite(wp, "%s", ue_password);
	}
	if(!strcmp(arg, "demand"))
	{
			if(((count == DEMANDENABLE) && (!strcmp(ue_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(ue_demand, "0"))))
				return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "idletime"))
	{
		char *demand;
	
		demand = ue_demand;
		
		if(count == 0)
		{
			if(!strcmp(demand, "1"))
				return websWrite(wp, "%s", ue_idletime);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		char *demand;
	
		demand = ue_demand;

		if(count == 0)
		{
				if(!strcmp(demand, "0"))
					return websWrite(wp, "%s", ue_redialperiod);
				else
					return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

#endif

/*
 * Example: 
 * pppoa_config = username:password:demand:idletime:redialperiod
 * pppoa_config = adsl:adsl1234:1:30:180
 * <% pppoa_config("username", 0); %> produces "adsl"
 */
int ej_pppoa_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(pppoa_inited == 0)
		pppoa_config_init();

	if(!strcmp(arg, "username"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_username[64];
		memset(new_username, 0, sizeof(new_username));
		filter_name(a_username, new_username, sizeof(new_username), GET);
		return websWrite(wp, "%s", new_username);
		//return websWrite(wp, "%s", a_username);
	}
	if(!strcmp(arg, "password"))
	{
		//printf("kirby debug: ej pppoa password count == %d\n",count);
		if(count == 3)
			   return websWrite(wp, "%s", "");
		else if(count == 4)
			   return websWrite(wp, "%s", a_password[0]?"1":"0");
		else	
		{
			//junzhao 2005.5.16 for ':' setting
			char new_password[64];
			memset(new_password, 0, sizeof(new_password));
			filter_name(a_password, new_password, sizeof(new_password), GET);
			return websWrite(wp, "%s", new_password);
			//return websWrite(wp, "%s", a_password);
		}
	}

	/*if(!strcmp(arg, "password"))
		return websWrite(wp, "%s", a_password);*/
		
	if(!strcmp(arg, "demand"))
	{
		if(((count == DEMANDENABLE) && (!strcmp(a_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(a_demand, "0"))))
			return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "idletime"))
	{
		if(count == 0)
		{
			if(!strcmp(a_demand, "1"))
				return websWrite(wp, "%s", a_idletime);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(a_demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		if(count == 0)
		{
			if(!strcmp(a_demand, "0"))
				return websWrite(wp, "%s", a_redialperiod);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(a_demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

void pppoe_config_init()
{
	char *ptr = nvram_safe_get("pppoe_config"), *tmp;
	int which, i;
	
	//memset(pppoe_config_buf, 0, sizeof(pppoe_config_buf));
	
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(pppoe_config_buf, nvram_safe_get("pppoe_config"), next) 
	{
		if (which-- == 0) 
#endif
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(pppoe_config_buf)
				free(pppoe_config_buf);
			pppoe_config_buf = strdup(tmp);
			
			e_password = pppoe_config_buf;
			e_username = strsep(&e_password, ":");
			if (!e_username || !e_password)
				break;
			e_demand = e_password;
			e_password = strsep(&e_demand, ":");
			if (!e_demand || !e_password)
				break;
			e_idletime = e_demand;
			e_demand = strsep(&e_idletime, ":");
			if (!e_demand || !e_idletime)
				break;
			if(pppoe_config_buf)
				free(pppoe_config_buf);
			pppoe_config_buf = strdup(tmp);
			
			e_password = pppoe_config_buf;
			e_username = strsep(&e_password, ":");
			if (!e_username || !e_password)
				break;
			e_demand = e_password;
			e_password = strsep(&e_demand, ":");
			if (!e_demand || !e_password)
				break;
			e_idletime = e_demand;
			e_demand = strsep(&e_idletime, ":");
			if (!e_demand || !e_idletime)
				break;
			e_redialperiod = e_idletime;
			e_idletime = strsep(&e_redialperiod, ":");
			if (!e_idletime || !e_redialperiod)
				break;
			e_servicename = e_redialperiod;
			e_redialperiod = strsep(&e_servicename, ":");
			if (!e_redialperiod || !e_servicename)
				break;

		#ifdef LJZ_DEBUG
			printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s, servicename=%s\n", e_username, e_password, e_demand, e_idletime, e_redialperiod, e_servicename);
		#endif	
			break;
		} 
	}
	pppoe_inited = 1;
}

/*
 * Example: 
 * pppoe_config = username:password:demand:idletime:redialperiod:servicename
 * pppoe_config = adsl:adsl1234:1:30:180:163gd
 * <% pppoe_config("username", 0); %> produces "adsl"
 */
int ej_pppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(pppoe_inited == 0)
		pppoe_config_init();

	if(!strcmp(arg, "servicename"))
	{
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *servicename = websGetVar(wp, "pppoe_servicename", NULL);
			if(servicename != NULL)
				return websWrite(wp, "%s", servicename);
			else	
			{
				//junzhao 2005.5.16 for ':' setting
				char new_servicename[64];
				memset(new_servicename, 0, sizeof(new_servicename));
				filter_name(e_servicename, new_servicename, sizeof(new_servicename), GET);	
				return websWrite(wp, "%s", new_servicename);
				//return websWrite(wp, "%s", e_servicename);
			}
		}
		else
	#endif
		{
			//junzhao 2005.5.16 for ':' setting
			char new_servicename[64];
			memset(new_servicename, 0, sizeof(new_servicename));
			filter_name(e_servicename, new_servicename, sizeof(new_servicename), GET);	
			return websWrite(wp, "%s", new_servicename);
			//return websWrite(wp, "%s", e_servicename);
		}
	}
	if(!strcmp(arg, "username"))
	{
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *username = websGetVar(wp, "pppoe_username", NULL);
			if(username != NULL)
				return websWrite(wp, "%s", username);
			else	
			{
				//junzhao 2005.5.16 for ':' setting
				char new_username[64];
				memset(new_username, 0, sizeof(new_username));
				filter_name(e_username, new_username, sizeof(new_username), GET);
				return websWrite(wp, "%s", new_username);
				//return websWrite(wp, "%s", e_username);
			}
		}
		else
	#endif
		{
			//junzhao 2005.5.16 for ':' setting
			char new_username[64];
			memset(new_username, 0, sizeof(new_username));
			filter_name(e_username, new_username, sizeof(new_username), GET);
			return websWrite(wp, "%s", new_username);
			//return websWrite(wp, "%s", e_username);
		}
	}
	if(!strcmp(arg, "password"))
	{
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *password = websGetVar(wp, "pppoe_password", NULL);
			if(password != NULL)
				return websWrite(wp, "%s", password);
			else	
			{
				//junzhao 2005.5.16 for ':' setting
				char new_password[64];
				memset(new_password, 0, sizeof(new_password));
				filter_name(e_password, new_password, sizeof(new_password), GET);
				return websWrite(wp, "%s", new_password);
				//return websWrite(wp, "%s", e_password);
			}
		}
		else
	#endif
		{
			//junzhao 2005.5.16 for ':' setting
			char new_password[64];
			memset(new_password, 0, sizeof(new_password));
			filter_name(e_password, new_password, sizeof(new_password), GET);
			return websWrite(wp, "%s", new_password);
			//return websWrite(wp, "%s", e_password);
		}
	}
	if(!strcmp(arg, "demand"))
	{
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *demand = websGetVar(wp, "pppoe_demand", NULL);
			if(demand != NULL)
			{
				if(((count == DEMANDENABLE) && (!strcmp(demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(demand, "0"))))
					return websWrite(wp, "checked");
			}
			else
			{
				if(((count == DEMANDENABLE) && (!strcmp(e_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(e_demand, "0"))))
					return websWrite(wp, "checked");
			}	
		}
		else
	#endif
		{
			if(((count == DEMANDENABLE) && (!strcmp(e_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(e_demand, "0"))))
				return websWrite(wp, "checked");
		}
	}
	if(!strcmp(arg, "idletime"))
	{
		char *demand;
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			demand = websGetVar(wp, "pppoe_demand", NULL);
			if(demand == NULL)
				demand = e_demand;
		}
		else
	#endif
			demand = e_demand;
		
		if(count == 0)
		{
		#ifdef MPPPOE_SUPPORT
			if(gozila_action && !connchanged)
			{
				char *idletime = websGetVar(wp, "pppoe_idletime", NULL);
				if(idletime != NULL)
				{
					if(!strcmp(demand, "1"))
						return websWrite(wp, "%s", idletime);
					else
						return websWrite(wp, "%s", "20");
				}
				else
				{
					if(!strcmp(demand, "1"))
						return websWrite(wp, "%s", e_idletime);
					else
						return websWrite(wp, "%s", "20");
				}
			}
			else
		#endif
			{	
				if(!strcmp(demand, "1"))
					return websWrite(wp, "%s", e_idletime);
				else
					return websWrite(wp, "%s", "20");
			}
		}
		else if(count == 1)
		{
			if(!strcmp(demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		char *demand;
	#ifdef MPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			demand = websGetVar(wp, "pppoe_demand", NULL);
			if(demand == NULL)
				demand = e_demand;
		}
		else
	#endif
			demand = e_demand;

		if(count == 0)
		{
		#ifdef MPPPOE_SUPPORT
			if(gozila_action && !connchanged)
			{
				char *redialperiod = websGetVar(wp, "pppoe_redialperiod", NULL);
				if(redialperiod != NULL)
				{
					if(!strcmp(demand, "0"))
						return websWrite(wp, "%s", redialperiod);
					else
						return websWrite(wp, "%s", "20");
				}
				else
				{
					if(!strcmp(demand, "0"))
						return websWrite(wp, "%s", e_redialperiod);
					else
						return websWrite(wp, "%s", "20");
				}
			}
			else
		#endif
			{
				if(!strcmp(demand, "0"))
					return websWrite(wp, "%s", e_redialperiod);
				else
					return websWrite(wp, "%s", "20");
			}
		}
		else if(count == 1)
		{
			if(!strcmp(demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

void bridged_config_init()
{
	char *next;
	int which;

	memset(bridged_config_buf, 0, sizeof(bridged_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	foreach(bridged_config_buf, nvram_safe_get("bridged_config"), next) 
	{
		if(which-- == 0) 
		{
			b_ipaddr = bridged_config_buf;
			dhcpenable = strsep(&b_ipaddr, ":");
			if (!dhcpenable || !b_ipaddr)
				break;
			b_netmask = b_ipaddr;
			b_ipaddr = strsep(&b_netmask, ":");
			if (!b_ipaddr || !b_netmask)
				break;
			b_gateway = b_netmask;
			b_netmask = strsep(&b_gateway, ":");
			if (!b_netmask || !b_gateway)
				break;
			b_pdns = b_gateway;
			b_gateway = strsep(&b_pdns, ":");
			if (!b_gateway || !b_pdns)
				break;
			b_sdns = b_pdns;
			b_pdns = strsep(&b_sdns, ":");
			if (!b_pdns || !b_sdns)
				break;

		#ifdef LJZ_DEBUG
			printf("dhcpenable=%s, ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", dhcpenable, b_ipaddr, b_netmask, b_gateway, b_pdns, b_sdns);
		#endif
			break;
		}
	}
	bridged_inited = 1;
}
/*
 * Example: 
 * bridged_config = dhcp_enable:ipaddr:netmask:gateway:pdns:sdns
 * bridged_config = 1:10.128.32.5:255.255.240.0:10.128.32.1:61.144.56.100:202.96.128.68
 * <% bridged_config("dhcp_enable", 0); %> produces "checked" if bridged_config.dhcp_enable == 0
 * <% bridged_config("ipaddr", 0); %> produces "10" if bridged_config.dhcp_enable == 0
 * <% bridged_config("ipaddr", 4); %> produces "disabled" if bridged_config.dhcp_enable == 1
 */
int ej_bridged_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(bridged_inited == 0)
		bridged_config_init();

	if(!strcmp(arg, "dhcpenable"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(((count == DHCPENABLE) && (!strcmp(dhcp_enable, "1"))) || ((count == DHCPDISABLE) && (!strcmp(dhcp_enable, "0"))))
					return websWrite(wp, "checked");
			}
			else
			{
				if(((count == DEMANDENABLE) && (!strcmp(dhcpenable, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(dhcpenable, "0"))))
					return websWrite(wp, "checked");
			}	
		}
		else
	#endif
		{
		if(((count == DHCPENABLE) && (!strcmp(dhcpenable, "1"))) || ((count == DHCPDISABLE) && (!strcmp(dhcpenable, "0"))))
			return websWrite(wp, "checked");
		}
	}
	if(!strcmp(arg, "ipaddr"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(count < 4)
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%d", 0);
					else
					{
						char name[18], *ipaddr;
						memset(name, 0, sizeof(name));
						sprintf(name, "bridged_ipaddr_%d", count);
						ipaddr = websGetVar(wp, name, NULL);
						return websWrite(wp, "%s", ipaddr);
					}
				}
				else
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
			else
			{
				if(count < 4)
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%d", 0);
					return websWrite(wp, "%d", get_single_ip(b_ipaddr, count));
				}
				else
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
		}
		else
	#endif
		{
		if(count < 4)
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%d", 0);
			return websWrite(wp, "%d", get_single_ip(b_ipaddr, count));
		}
		else
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%s", "disabled");
		}
		}	
	}
	if(!strcmp(arg, "netmask"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(count < 4)
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%d", 0);
					else
					{
						char name[20], *netmask;
						memset(name, 0, sizeof(name));
						sprintf(name, "bridged_netmask_%d", count);
						netmask = websGetVar(wp, name, NULL);
						return websWrite(wp, "%s", netmask);
					}
				}
				else
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
			else
			{
				if(count < 4)
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%d", 0);
					return websWrite(wp, "%d", get_single_ip(b_netmask, count));
				}
				else
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
		}
		else
	#endif
		{
		if(count < 4)
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%d", 0);
			return websWrite(wp, "%d", get_single_ip(b_netmask, count));
		}
		else
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
		}
	}
	if(!strcmp(arg, "gateway"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(count < 4)
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%d", 0);
					else
					{
						char name[20], *gateway;
						memset(name, 0, sizeof(name));
						sprintf(name, "bridged_gateway_%d", count);
						gateway = websGetVar(wp, name, NULL);
						return websWrite(wp, "%s", gateway);
					}
				}
				else
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
			else
			{
				if(count < 4)
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%d", 0);
					return websWrite(wp, "%d", get_single_ip(b_gateway, count));
				}
				else
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
		}
		else
	#endif

		{
		if(count < 4)
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%d", 0);
			return websWrite(wp, "%d", get_single_ip(b_gateway, count));
		}
		else 
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%s", "disabled");
		}
		}	
	}
	if(!strcmp(arg, "pdns"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(count < 4)
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%d", 0);
					else
					{
						char name[18], *pdns;
						memset(name, 0, sizeof(name));
						sprintf(name, "bridged_pdns_%d", count);
						pdns = websGetVar(wp, name, NULL);
						return websWrite(wp, "%s", pdns);
					}
				}
				else
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
			else
			{
				if(count < 4)
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%d", 0);
					return websWrite(wp, "%d", get_single_ip(b_pdns, count));
				}
				else
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
		}
		else
	#endif
		{
		if(count < 4)
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%d", 0);
			return websWrite(wp, "%d", get_single_ip(b_pdns, count));
		}
		else
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%s", "disabled");
		}
		}	
	}
	if(!strcmp(arg, "sdns"))
	{
	#ifdef IPPPOE_SUPPORT
		if(gozila_action && !connchanged)
		{
			char *dhcp_enable = websGetVar(wp, "bridged_dhcpenable", NULL);
			if(dhcp_enable != NULL)
			{
				if(count < 4)
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%d", 0);
					else
					{
						char name[18], *sdns;
						memset(name, 0, sizeof(name));
						sprintf(name, "bridged_sdns_%d", count);
						sdns = websGetVar(wp, name, NULL);
						return websWrite(wp, "%s", sdns);
					}
				}
				else
				{
					if(!strcmp(dhcp_enable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
			else
			{
				if(count < 4)
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%d", 0);
					return websWrite(wp, "%d", get_single_ip(b_sdns, count));
				}
				else
				{
					if(!strcmp(dhcpenable, "1"))
						return websWrite(wp, "%s", "disabled");
				}
			}
		}
		else
	#endif
		{
		if(count < 4)
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%d", 0);
			return websWrite(wp, "%d", get_single_ip(b_sdns, count));
		}
		else
		{
			if(!strcmp(dhcpenable, "1"))
				return websWrite(wp, "%s", "disabled");
		}
		}	
	}
	return -1;
}

#ifdef CLIP_SUPPORT
void clip_config_init()
{
	char *next;
	int which;

	memset(clip_config_buf, 0, sizeof(clip_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	foreach(clip_config_buf, nvram_safe_get("clip_config"), next) 
	{
		if(which-- == 0) 
		{
			c_netmask = clip_config_buf;
			c_ipaddr = strsep(&c_netmask, ":");
			if (!c_ipaddr || !c_netmask)
				break;
			c_gateway = c_netmask;
			c_netmask = strsep(&c_gateway, ":");
			if (!c_netmask || !c_gateway)
				break;
			c_pdns = c_gateway;
			c_gateway = strsep(&c_pdns, ":");
			if (!c_gateway || !c_pdns)
				break;
			c_sdns = c_pdns;
			c_pdns = strsep(&c_sdns, ":");
			if (!c_pdns || !c_sdns)
				break;

		#ifdef LJZ_DEBUG
			printf("ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", c_ipaddr, c_netmask, c_gateway, c_pdns, c_sdns);
		#endif
			break;
		}
	}
	clip_inited = 1;
}

int ej_clip_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(clip_inited == 0)
		clip_config_init();

	if(!strcmp(arg, "ipaddr"))
	{
		return websWrite(wp, "%d", get_single_ip(c_ipaddr,count));
	}
	if(!strcmp(arg, "netmask"))
	{
		return websWrite(wp, "%d", get_single_ip(c_netmask, count));
	}
	if(!strcmp(arg, "gateway"))
	{
		return websWrite(wp, "%d", get_single_ip(c_gateway, count));
	}
	if(!strcmp(arg, "pdns"))
	{
		return websWrite(wp, "%d", get_single_ip(c_pdns, count));
	}
	if(!strcmp(arg, "sdns"))
	{
		return websWrite(wp, "%d", get_single_ip(c_sdns, count));
	}
	return -1;
}

void validate_clip_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[100], *next;
	char buf[800], *cur=buf;
	char old[PVC_CONNECTIONS][100];
	char *ipaddr, *netmask, *gateway, *pdns, *sdns;
	char *connection;
	char temp[30], *val=NULL;
	struct variable clip_config_variables[] = {
                { longname: "Clip Static IP Address", argv: NULL },
                { longname: "Clip Static Netmask", argv: NULL },
                { longname: "Clip Static Gateway", argv: NULL },
                { longname: "Clip Static Primary Dns", argv: NULL },
                { longname: "Clip Static Secondary Dns", argv: NULL },
        };

	#ifdef LJZ_DEBUG
		printf("enter validate_clip_config\n");
	#endif

	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	ipaddr = malloc(20);
	memset(ipaddr, 0, sizeof(ipaddr));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "clip_ipaddr", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(ipaddr,val);
                        if(i<3)
				strcat(ipaddr,".");
                }
                else
		{
			free(ipaddr);
                        return ;
		}
        }
	netmask = malloc(20);
	memset(netmask, 0, sizeof(netmask));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "clip_netmask", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(netmask, val);
                        if(i<3)
				strcat(netmask, ".");
                }
                else
		{
			free(netmask);
                        return ;
		}
        }
	gateway = malloc(20);
	memset(gateway, 0, sizeof(gateway));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "clip_gateway", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(gateway, val);
                        if(i<3)
				strcat(gateway, ".");
                }
                else
		{
			free(gateway);
                        return ;
		}
        }
	pdns = malloc(20);
	memset(pdns, 0, sizeof(pdns));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "clip_pdns", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(pdns, val);
                        if(i<3)
				strcat(pdns, ".");
                }
                else
		{
			free(pdns);
                        return ;
		}
        }
	sdns = malloc(20);
	memset(sdns, 0, sizeof(sdns));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "clip_sdns", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(sdns, val);
                        if(i<3)
				strcat(sdns, ".");
                }
                else
		{
			free(sdns);
                        return ;
		}
        }
	if (!*ipaddr)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
                return;
        }
        if (!*netmask)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Subnet Mask<br>", v->longname);
                return;
        }
        if (!*gateway)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Gateway Address<br>", v->longname);
                return;
        }
        if (!*pdns)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Primary DNS Address<br>", v->longname);
                return;
        }
	if (!valid_ipaddr(wp, ipaddr, &clip_config_variables[0]) || !valid_netmask(wp, netmask, &clip_config_variables[1]) || !valid_ipaddr(wp, gateway, &clip_config_variables[2]) || !valid_ipaddr(wp, pdns, &clip_config_variables[3]))
                return;
	if(*sdns)
	{
		if(!valid_ipaddr(wp, sdns, &clip_config_variables[4]))
			return;
	}

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));

	i = 0;
	foreach(word, nvram_safe_get("clip_config"), next)
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", ipaddr, netmask, gateway, pdns, sdns);

	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

#ifdef LJZ_DEBUG
	printf("clip_post_buf %s\n", buf);
#endif
	nvram_set(v->name, buf);

	//junzhao 2004.3.31
    {
	char dnsbuf[32];
	memset(dnsbuf, 0, sizeof(dnsbuf));
	sprintf(dnsbuf, "%s:%s", pdns, sdns);
	nvram_modify_status("wan_static_dns", dnsbuf);
	//junzhao 2004.4.13 for wan_dns
	nvram_set("wan_dns", "");
    }

	if(ipaddr)
		free(ipaddr);
	if(netmask)
		free(netmask);
	if(gateway)
		free(gateway);
	if(pdns)
		free(pdns);
	if(sdns)
		free(sdns);
	return;
}
#endif
		
void routed_config_init()
{
	char *next;
	int which;

	memset(routed_config_buf, 0, sizeof(routed_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	foreach(routed_config_buf, nvram_safe_get("routed_config"), next) 
	{
		if(which-- == 0) 
		{
			r_netmask = routed_config_buf;
			r_ipaddr = strsep(&r_netmask, ":");
			if (!r_ipaddr || !r_netmask)
				break;
			r_gateway = r_netmask;
			r_netmask = strsep(&r_gateway, ":");
			if (!r_netmask || !r_gateway)
				break;
			r_pdns = r_gateway;
			r_gateway = strsep(&r_pdns, ":");
			if (!r_gateway || !r_pdns)
				break;
			r_sdns = r_pdns;
			r_pdns = strsep(&r_sdns, ":");
			if (!r_pdns || !r_sdns)
				break;

		#ifdef LJZ_DEBUG
			printf("ipaddr=%s, netmask=%s, gateway=%s, pdns=%s, sdns=%s\n", r_ipaddr, r_netmask, r_gateway, r_pdns, r_sdns);
		#endif
			break;
		}
	}
	routed_inited = 1;
}
/*
 * Example: 
 * routed_config = ipaddr:netmask:gateway:pdns:sdns
 * routed_config = 10.128.32.5:255.255.240.0:10.128.32.1:61.144.56.100:202.96.128.68
 * <% routed_config("ipaddr", 0); %> produces "10" 
 */
int ej_routed_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(routed_inited == 0)
		routed_config_init();

	if(!strcmp(arg, "ipaddr"))
	{
		return websWrite(wp, "%d", get_single_ip(r_ipaddr,count));
	}
	if(!strcmp(arg, "netmask"))
	{
		return websWrite(wp, "%d", get_single_ip(r_netmask, count));
	}
	if(!strcmp(arg, "gateway"))
	{
		return websWrite(wp, "%d", get_single_ip(r_gateway, count));
	}
	if(!strcmp(arg, "pdns"))
	{
		return websWrite(wp, "%d", get_single_ip(r_pdns, count));
	}
	if(!strcmp(arg, "sdns"))
	{
		return websWrite(wp, "%d", get_single_ip(r_sdns, count));
	}
	return -1;
}

/*
 * Example: 
 * mtu_config = mtu_auto:mtu_value
 * mtu_config = 0:1500 
 * <% mtu_config("auto", 0); %> produces "selected" 
 * <% mtu_config("auto", 1); %> produces "" 
 */
int ej_mtu_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;
	char word[32], *next;
	char *mtu_auto=NULL, *mtu_value=NULL;
	int which;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	foreach(word, nvram_safe_get("mtu_config"), next) 
	{
		if(which-- == 0) 
		{
			mtu_value = word;
			mtu_auto = strsep(&mtu_value, ":");
			if (!mtu_auto || !mtu_value)
				break;

		#ifdef LJZ_DEBUG
			printf("mtu_auto %s mtu_value %s\n", mtu_auto, mtu_value);
		#endif
			break;
		}
	}
	
	if(!strcmp(arg, "auto"))
	{
		if(((!strcmp(mtu_auto, "1")) && (count == 1)) || ((!strcmp(mtu_auto, "0") && (count == 0)))) 
			return websWrite(wp, "%s", "selected");
	}
	else if(!strcmp(arg, "value"))
	{
		if(!strcmp(mtu_auto, "1"))
			return websWrite(wp, "%s disabled", mtu_value);
		else
			return websWrite(wp, "%s", mtu_value);
	}	
	return -1;
}

/*
 * Example: 
 * macclone_enable = 0 or 1
 * macclone_addr = hwaddr0:hwaddr1:hwaddr2:hwaddr3:hwaddr4:hwaddr5  
 * macclone_enable = 1
 * macclone_addr = 00:01:02:03:04:05  
 * <% macclone_config("macclone_enable", 1); %> produces "checked" 
 * <% macclone_config("macclone_addr", 1); %> produces "01" 
 */
int ej_macclone_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;
	char word[32], *next;
	char *enable=NULL, *hwaddr=NULL;
	int which;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	memset(word, 0, sizeof(word));
	if(!strcmp(arg, "macclone_enable"))
	{
		if(clone_wan_mac)
		{
			if(count == 1)
				return websWrite(wp, "checked");		
			else
				return 0;
		}
		foreach(word, nvram_safe_get("macclone_enable"), next) 
		{
			if(which-- == 0) 
			{
				enable = word;
			#ifdef LJZ_DEBUG
				printf("macclone enable %s\n", enable);
			#endif
				break;
			}
		}
		if(((!strcmp(enable, "1")) && (count == 1)) || ((!strcmp(enable, "0")) && (count == 0)))
			return websWrite(wp, "checked");		
	}
	else if(!strcmp(arg, "macclone_addr"))
	{
		char readbuf[140];
		//clone wan mac addr
		if(clone_wan_mac)
		{
			char *c = nvram_safe_get("http_client_mac");
			if(count < 6)
				return websWrite(wp, "%02x", get_single_mac(c, count));		
		}
		if(!file_to_buf(WAN_HWADDR, readbuf, sizeof(readbuf)))
	        {
			printf("no buf in %s!\n", WAN_HWADDR);
	               	return 0;
	        }
		foreach(word, readbuf, next) 
		//foreach(word, nvram_safe_get("wan_hwaddr"), next) 
		{
			if(which-- == 0) 
			{
				hwaddr = word;
			#ifdef LJZ_DEBUG
				printf("macclone hwaddr %s\n", hwaddr);
			#endif
				break;
			}
		}
		if(count < 6)
			return websWrite(wp, "%02x", get_single_mac(hwaddr, count));		
	}
	return 0;
}	

/*------- add by leijun 2004-0823 -----*/
#ifdef 	CPED_TR064_SUPPORT
struct pvc_link_conn_info
{
	char *name;
	char *link_type;
	char *conn_type;
};

struct pvc_link_conn_info pvc_info[] = {
	{"1483bridged", "EoA", "IP_Routed"},
	{"routed", "IPoA", "IP_Routed"},
	{"clip", "CIP", "IP_Routed"},
	{"pppoe", "PPPoE", "IP_Routed"},
	{"pppoa", "PPPoA", "IP_Routed"},
	{"bridgedonly", "EoA", "IP_Bridged"},
	{NULL}
};
int get_info_by_name(char *name, char *link_type, char *connect_type)
{
	int i = 0;
	if ((!name) || (!link_type) || (!connect_type)) return 0;
	
	while(pvc_info[i].name != NULL)
	{
		if (!strcasecmp(name, pvc_info[i].name))
		{
			strcpy(link_type, pvc_info[i].link_type);
			strcpy(connect_type, pvc_info[i].conn_type);
			return 1;
		}
		i ++;
	}
	return 0;
}
#endif
/*------- add by leijun 2004-0823 over -----*/

//#define LJZ_DEBUG
void validate_vcc_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[60], *next;
	char buf[480], *cur=buf;
	char old[PVC_CONNECTIONS][60];
	char *l_vpi, *l_vci, *l_encapmode, *l_multiplex, *l_qos, *l_pcr, *l_scr, *l_autodetect, *l_applyonboot;
	char *connection;
	struct variable vcc_config_variables[] = {
		{ longname: "VPI Value", argv: ARGV("0", "256") },	
		{ longname: "VCI Value", argv: ARGV("32", "65535") },	
	#ifndef CLIP_SUPPORT
		{ longname: "Encapmode", argv: ARGV("1483bridged", "routed", "pppoe", "pppoa", "pptp", "bridgedonly") },	
	#else
 		/* kirby for unip 2004/11.25 */
	       #ifdef UNNUMBERED_SUPPORT
	       { longname: "Encapmode", argv: ARGV("1483bridged", "routed", "pppoe", "pppoa", "pptp", "bridgedonly", "clip","unpppoe","unpppoa") },	
	       #else
		{ longname: "Encapmode", argv: ARGV("1483bridged", "routed", "pppoe", "pppoa", "pptp", "bridgedonly", "clip") },	
		#endif
	#endif
		{ longname: "Multiplexing", argv: ARGV("llc", "vc") },	
		{ longname: "Qos Type", argv: ARGV("ubr", "cbr", "vbr") },	
		{ longname: "Pcr Value", argv: ARGV("1", "65534") },	
		{ longname: "Scr Value", argv: ARGV("1", "65534") },	
		{ longname: "Autodetect Mode", argv: ARGV("0", "1") },	
		{ longname: "Apply on Boot", argv: ARGV("0", "1"), TRUE},	
	};

	#ifdef LJZ_DEBUG
		printf("enter validate_vcc_config\n");
	#endif 

#ifdef CPED_TR064_SUPPORT
	/* kirby 2004/09 TR064 */
    nvram_set("default_connection_which","0");
#endif
		
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_applyonboot = websGetVar(wp, "wan_applyonboot", "0");
	/*if(!strcmp(l_applyonboot, "0"))
	{
		memset(word, 0, sizeof(word));
		memset(buf, 0, sizeof(buf));
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
			memset(old[i], 0, sizeof(old[i]));
	
		i=0;
		foreach(word, nvram_safe_get("vcc_config"), next) 
		{
			strcpy(old[i], word);
			i++;
		}
		snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s:%s:%s", vpi, vci, encapmode, multiplex, qos, pcr, scr, autodetect, l_applyonboot);
	}
	else*/
	{	
		l_vpi = websGetVar(wp, "wan_vpi", "0");
		l_vci = websGetVar(wp, "wan_vci", "0");
		l_encapmode = websGetVar(wp, "wan_encapmode", "1483bridged");
		//junzhao 2004.10.26 for clip mode llc multiplex
		l_multiplex = websGetVar(wp, "wan_multiplex", "llc");
		l_qos = websGetVar(wp, "wan_qostype", NULL);
		l_pcr = websGetVar(wp, "wan_pcr", "0");
		l_scr = websGetVar(wp, "wan_scr", "0");
		l_autodetect = websGetVar(wp, "wan_autodetect", "0");
		if(strcmp(l_vpi, "0") || strcmp(l_vci, "0"))
		{
			if(!valid_range(wp, l_vpi, &vcc_config_variables[0]))
				return;
			if(!valid_range(wp, l_vci, &vcc_config_variables[1]))
				return;
		}
	
		if (!valid_choice(wp, l_encapmode, &vcc_config_variables[2]))
                	return;
		if (!valid_choice(wp, l_multiplex, &vcc_config_variables[3]))
                	return;
		if (!valid_choice(wp, l_qos, &vcc_config_variables[4]))
                	return;
		
		if(strcmp(l_qos, "ubr"))
		{
			if (!valid_range(wp, l_pcr, &vcc_config_variables[5]))
                		return;
		}
		if(!strcmp(l_qos, "vbr"))
		{
			if (!valid_range(wp, l_scr, &vcc_config_variables[6]))
                		return;
		}
		
		if (!valid_choice(wp, l_autodetect, &vcc_config_variables[7]))
                	return;
		if (!valid_choice(wp, l_applyonboot, &vcc_config_variables[8]))
                	return;
	
		memset(word, 0, sizeof(word));
		memset(buf, 0, sizeof(buf));
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
			memset(old[i], 0, sizeof(old[i]));
	
		i=0;
		foreach(word, nvram_safe_get("vcc_config"), next) 
		{
			strcpy(old[i], word);
			i++;
		}
		snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s:%s:%s", l_vpi, l_vci, l_encapmode, l_multiplex, l_qos, l_pcr, l_scr, l_autodetect, l_applyonboot);
	}
		
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	#ifdef LJZ_DEBUG
		printf("buf %s\n", buf);
	#endif 
	nvram_set(v->name, buf);
	
	//junzhao 2004.5.13
	if(!strcmp(l_encapmode, "bridgedonly"))
		nvram_set("lan_proto", "static");

#ifdef IPCP_NETMASK_SUPPORT
	//zhangbin for unip 2005.1.23
	if(nvram_match("ipcp_netmask_enable","1"))
	{
		nvram_set("nat_enable","1");
		nvram_set("wk_mode","gateway");
	}
	if((strcmp(l_encapmode, "pppoa")) && (strcmp(l_encapmode, "pppoe")))
		nvram_set("ipcp_netmask_enable","0");
#endif
	
	//junzhao 2004.4.2
	if(!strcmp(l_applyonboot, "1"))
	{
		if(atoi(connection) < atoi(nvram_safe_get("wan_active_connection")))		
			nvram_set("wan_active_connection", connection);
	}
	else
	{
		if(!strcmp(nvram_safe_get("wan_active_connection"), connection))
		{
			char readbuf[20], *next, word[3];
			int which = atoi(connection);
			int i = 0;
			if(!file_to_buf(WAN_ENABLE_TABLE, readbuf, sizeof(readbuf)))
				printf("no buf in %s!\n", WAN_ENABLE_TABLE);
        		foreach(word, readbuf, next)
        		{
                		if((i > which) && (!strcmp(word, "1")))
				{
					char tmp[3];
					memset(tmp, 0, sizeof(tmp));
					sprintf(tmp, "%d", i);
					nvram_set("wan_active_connection", tmp);
					break;
				}
				else
					i++;
        		}
			if(i==PVC_CONNECTIONS)
				nvram_set("wan_active_connection", "8");
		}	
	}	
	//change the status err str of this connection 
	//nvram_modify_status("wan_pvc_errstr", "saved");
	
	ram_modify_status(WAN_PVC_ERRSTR, "saved");

    //junzhao 2004.3.22	
    /*
     {
	char tmpword[20], *next, *saved;
	char config_name[20];
	int which = atoi(connection);
	if(!strcmp(l_encapmode, "pppoa"))
		sprintf(config_name, "pppoa_saved");
	else
		sprintf(config_name, "fourtypes_saved");
	foreach(tmpword, nvram_safe_get(config_name), next) 
	{
		if(which-- == 0) 
		{
			saved = tmpword;
			break;
		}
	}
	if(!strcmp(saved, "0"))
		nvram_modify_status(config_name, "1");
     }
     */
	
	//junzhao 2004.6.17
	nvram_set("autosearchflag", "0");

#ifdef 	CPED_TR064_SUPPORT
	//leijun 2004-0809
	{	
		int link_enable = 0, conn_enable = 0, enable_flag = 0;
		char link_type[12], conn_type[12];

		i=0;
		foreach(word, nvram_safe_get("vcc_config_extend"), next) 
		{
			strcpy(old[i ++], word);
		}
		get_info_by_name(l_encapmode, link_type, conn_type);
		if(!strcmp(l_applyonboot, "1")) 
		{
			enable_flag = 2;
			link_enable = 1;
			conn_enable = 1;
		}
		sprintf(old[atoi(connection)], "%s/%s:%d:%s:%d:LLC:VBR-nrt:0:0:1500:0:%s:%d:", l_vpi, l_vci, enable_flag, link_type, link_enable, conn_type, conn_enable);
		
		cur = &buf[0];
		for(i=0 ; i<PVC_CONNECTIONS ; i++)
		{
			if(strcmp(old[i],""))
				cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
					cur == buf ? "" : " ", old[i]);
		}
		nvram_set("vcc_config_extend", buf);
	}
	//leijun 2004-0809 over
#endif
	
	return;
}
			
//junzhao 2005.5.16 for ':' setting
void validate_pppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64], new_servicename[5*64];
	char *ptr, *tmp;
	int pppoe_config_size = 0, pppoe_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_username = websGetVar(wp, "pppoe_username", "");
	l_password = websGetVar(wp, "pppoe_password", "");
	l_demand = websGetVar(wp, "pppoe_demand", "1");
	l_idletime = websGetVar(wp, "pppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "pppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "pppoe_servicename", "");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	filter_name(l_servicename, new_servicename, sizeof(new_servicename), SET);

	pppoe_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + strlen(new_servicename) + 5;
	
	ptr = nvram_safe_get("pppoe_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(pppoe_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, pppoe_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod, new_servicename);
		}
		else
			old[i] = strdup(tmp);
		pppoe_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(pppoe_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, pppoe_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);
	
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}
#if 0
void validate_pppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[210], *next;
	char buf[1680], *cur=buf;
	char old[PVC_CONNECTIONS][210];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename;
	char *connection;
	struct variable pppoe_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
		{ longname: "Service Name", argv: ARGV("63") },
	};
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_username = websGetVar(wp, "pppoe_username", NULL);
	l_password = websGetVar(wp, "pppoe_password", NULL);
	l_demand = websGetVar(wp, "pppoe_demand", NULL);
	l_idletime = websGetVar(wp, "pppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "pppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "pppoe_servicename", NULL);
	
	//if(!valid_name(wp, l_servicename, &pppoe_config_variables[5]))
	//	return;
	if(!valid_name(wp, l_username, &pppoe_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &pppoe_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &pppoe_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &pppoe_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &pppoe_config_variables[4]))
			return;
	}
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("pppoe_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod, l_servicename);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//2004.3.31 junzhao
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	return;
}
#endif

//junzhao 2005.5.16 for ':' setting
void validate_pppoa_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64];
	char *ptr, *tmp;
	int pppoa_config_size = 0, pppoa_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_username = websGetVar(wp, "pppoa_username", "");
	l_password = websGetVar(wp, "pppoa_password", "");
	l_demand = websGetVar(wp, "pppoa_demand", "1");
	l_idletime = websGetVar(wp, "pppoa_idletime", "20");
	l_redialperiod = websGetVar(wp, "pppoa_redialperiod", "20");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);

	pppoa_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + 4;
	
	ptr = nvram_safe_get("pppoa_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(pppoa_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, pppoa_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod);
		}
		else
			old[i] = strdup(tmp);
		pppoa_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(pppoa_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, pppoa_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);
	
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}


#if 0
void validate_pppoa_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[150], *next;
	char buf[1200], *cur=buf;
	char old[PVC_CONNECTIONS][150];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod;
	char *connection;
	struct variable pppoa_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
	};
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[64*5], new_password[64*5];
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_username = websGetVar(wp, "pppoa_username", NULL);
	l_password = websGetVar(wp, "pppoa_password", NULL);
	l_demand = websGetVar(wp, "pppoa_demand", NULL);
	l_idletime = websGetVar(wp, "pppoa_idletime", "20");
	l_redialperiod = websGetVar(wp, "pppoa_redialperiod", "20");
	
	if(!valid_name(wp, l_username, &pppoa_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &pppoa_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &pppoa_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &pppoa_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &pppoa_config_variables[4]))
			return;
	}

	//junzhao 2005.5.16  for ':' setting
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("pppoa_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	//junzhao 2005.5.16 for ':' setting
	//snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod);
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//2004.3.31 junzhao
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	
	//junzhao 2004.3.22
     /*
     {
	char tmpword[20], *next, *pppoa_saved;
	int which = atoi(connection);
	foreach(tmpword, nvram_safe_get("pppoa_saved"), next) 
	{
		if(which-- == 0) 
		{
			pppoa_saved = tmpword;
			break;
		}
	}
	if(!strcmp(pppoa_saved, "0"))
		nvram_modify_status("pppoa_saved", "1");
     }
     */
	return;
}
#endif

/* kirby for unip 2004/11.25 */
#ifdef UNNUMBERED_SUPPORT
//junzhao 2005.5.16 for ':' setting
void validate_unip_pppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64], new_servicename[5*64];
	char *ptr, *tmp;
	int unip_pppoe_config_size = 0, unip_pppoe_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_username = websGetVar(wp, "unip_pppoe_username", "");
	l_password = websGetVar(wp, "unip_pppoe_password", "");
	l_demand = websGetVar(wp, "unip_pppoe_demand", "1");
	l_idletime = websGetVar(wp, "unip_pppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "unip_pppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "unip_pppoe_servicename", "");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	filter_name(l_servicename, new_servicename, sizeof(new_servicename), SET);

	pppoe_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + strlen(new_servicename) + 5;
	
	ptr = nvram_safe_get("unip_pppoe_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(unip_pppoe_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, unip_pppoe_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod, new_servicename);
		}
		else
			old[i] = strdup(tmp);
		unip_pppoe_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(unip_pppoe_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, unip_pppoe_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);
	
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}

#if 0
void validate_unip_pppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[210], *next;
	char buf[1680], *cur=buf;
	char old[PVC_CONNECTIONS][210];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename;
	char *connection;
	struct variable unip_pppoe_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
		{ longname: "Service Name", argv: ARGV("63") },
	};
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[64*5], new_password[64*5], new_servicename[64*5];
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_username = websGetVar(wp, "unip_pppoe_username", NULL);
	l_password = websGetVar(wp, "unip_pppoe_password", NULL);
	l_demand = websGetVar(wp, "unip_pppoe_demand", NULL);
	l_idletime = websGetVar(wp, "unip_pppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "unip_pppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "unip_pppoe_servicename", NULL);
	

	if(!valid_name(wp, l_username, &unip_pppoe_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &unip_pppoe_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &unip_pppoe_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &unip_pppoe_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &unip_pppoe_config_variables[4]))
			return;
	}
	
	//junzhao 2005.5.16  for ':' setting
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	filter_name(l_servicename, new_servicename, sizeof(new_servicename), SET);
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));

	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("unip_pppoe_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	
	//junzhao 2005.5.16 for ':' setting
	//snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod, l_servicename);
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod, new_servicename);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//2004.3.31 junzhao
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	return;
}
#endif

//junzhao 2005.5.16 for ':' setting
void validate_unip_pppoa_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64];
	char *ptr, *tmp;
	int unip_pppoa_config_size = 0, unip_pppoa_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_username = websGetVar(wp, "unip_pppoa_username", "");
	l_password = websGetVar(wp, "unip_pppoa_password", "");
	l_demand = websGetVar(wp, "unip_pppoa_demand", "1");
	l_idletime = websGetVar(wp, "unip_pppoa_idletime", "20");
	l_redialperiod = websGetVar(wp, "unip_pppoa_redialperiod", "20");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);

	pppoa_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + 4;
	
	ptr = nvram_safe_get("unip_pppoa_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(unip_pppoa_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, unip_pppoa_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod);
		}
		else
			old[i] = strdup(tmp);
		unip_pppoa_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(unip_pppoa_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, unip_pppoa_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);
	
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}


#if 0
void validate_unip_pppoa_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[150], *next;
	char buf[1200], *cur=buf;
	char old[PVC_CONNECTIONS][150];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod;
	char *connection;
	struct variable unip_pppoa_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
	};
	
	//junzhao 2005.5.16 for ':' setting
	char new_username[64*5], new_password[64*5];
	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_username = websGetVar(wp, "unip_pppoa_username", NULL);
	l_password = websGetVar(wp, "unip_pppoa_password", NULL);
	l_demand = websGetVar(wp, "unip_pppoa_demand", NULL);
	l_idletime = websGetVar(wp, "unip_pppoa_idletime", "20");
	l_redialperiod = websGetVar(wp, "unip_pppoa_redialperiod", "20");
	
	if(!valid_name(wp, l_username, &unip_pppoa_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &unip_pppoa_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &unip_pppoa_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &unip_pppoa_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &unip_pppoa_config_variables[4]))
			return;
	}

	//junzhao 2005.5.16  for ':' setting
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("unip_pppoa_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	
	//junzhao 2005.5.16  for ':' setting
	//snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod);
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//2004.3.31 junzhao
	nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	

	return;
}
#endif
#endif

void validate_bridged_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[100], *next;
	char buf[800], *cur=buf;
	char old[PVC_CONNECTIONS][100];
	char *l_dhcpenable, *ipaddr, *netmask, *gateway, *pdns, *sdns;
	char *connection;
	char temp[30], *val=NULL;
	struct variable bridged_config_variables[] = {
                { longname: "Bridged Dhcp Mode", argv: ARGV("0", "1") },
                { longname: "Bridged Static IP Address", argv: NULL },
                { longname: "Bridged Static Netmask", argv: NULL },
                { longname: "Bridged Static Gateway", argv: NULL },
                { longname: "Bridged Static Primary Dns", argv: NULL },
                { longname: "Bridged Static Secondary Dns", argv: NULL },
        };
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	l_dhcpenable = websGetVar(wp, "bridged_dhcpenable", NULL);
	
	if(!valid_choice(wp, l_dhcpenable, &bridged_config_variables[0]))
		return;
	/*
	ipaddr = malloc(20);
	netmask = malloc(20);
	gateway = malloc(20);
	pdns = malloc(20);
	sdns = malloc(20);
	memset(ipaddr, 0, sizeof(ipaddr));
	memset(netmask, 0, sizeof(netmask));
	memset(gateway, 0, sizeof(gateway));
	memset(pdns, 0, sizeof(pdns));
	memset(sdns, 0, sizeof(sdns));
	*/
	if((l_dhcpenable == NULL) || (!strcmp(l_dhcpenable, "1")))
	{
		//strcpy(ipaddr, "0.0.0.0");
		//strcpy(netmask, "0.0.0.0");
		//strcpy(gateway, "0.0.0.0");
		//strcpy(pdns, "0.0.0.0");
		//strcpy(sdns, "0.0.0.0");
		ipaddr = NULL;
		netmask = NULL;
		gateway = NULL;
		pdns = NULL;
		sdns = NULL;
	}
	else
	{
		ipaddr = malloc(20);
		memset(ipaddr, 0, sizeof(ipaddr));
		memset(temp, 0, sizeof(temp));
        	for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "bridged_ipaddr", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(ipaddr,val);
                        	if(i<3) 
					strcat(ipaddr,".");
                	}
                	else
			{	
				free(ipaddr);
                        	return ;
			}
        	}
		netmask = malloc(20);
		memset(netmask, 0, sizeof(netmask));
		memset(temp, 0, sizeof(temp));
        	for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "bridged_netmask", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(netmask, val);
                        	if(i<3) 
					strcat(netmask, ".");
                	}
                	else
			{	
				free(netmask);
                        	return ;
			}
        	}
		gateway = malloc(20);
		memset(gateway, 0, sizeof(gateway));
		memset(temp, 0, sizeof(temp));
        	for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "bridged_gateway", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(gateway, val);
                        	if(i<3) 
					strcat(gateway, ".");
                	}
                	else
			{	
				free(gateway);
                        	return ;
			}
        	}
		pdns = malloc(20);
		memset(pdns, 0, sizeof(pdns));
		memset(temp, 0, sizeof(temp));
        	for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "bridged_pdns", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(pdns, val);
                        	if(i<3) 
					strcat(pdns, ".");
                	}
                	else
			{	
				free(pdns);
                        	return ;
			}
        	}
		sdns = malloc(20);
		memset(sdns, 0, sizeof(sdns));
		memset(temp, 0, sizeof(temp));
        	for(i=0 ; i<4 ; i++)
		{
                	snprintf(temp, sizeof(temp), "%s_%d", "bridged_sdns", i);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(sdns, val);
                        	if(i<3) 
					strcat(sdns, ".");
                	}
                	else
			{	
				free(sdns);
                        	return ;
			}
        	}
		if (!*ipaddr) 
		{
                	websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
                	return;
        	}
        	if (!*netmask) 
		{
                	websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Subnet Mask<br>", v->longname);
                	return;
        	}
        	if (!*gateway) 
		{
                	websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Gateway Address<br>", v->longname);
                	return;
        	}
        	if (!*pdns)  
		{
                	websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Primary DNS Address<br>", v->longname);
                	return;
        	}
		if (!valid_ipaddr(wp, ipaddr, &bridged_config_variables[1]) || !valid_netmask(wp, netmask, &bridged_config_variables[2]) || !valid_ipaddr(wp, gateway, &bridged_config_variables[3]) || !valid_ipaddr(wp, pdns, &bridged_config_variables[4]))
                	return;
		if(*sdns)
		{	
			if(!valid_ipaddr(wp, sdns, &bridged_config_variables[5]))
				return;
		}
	}

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("bridged_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s", l_dhcpenable, ipaddr, netmask, gateway, pdns, sdns);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}
	
#ifdef LJZ_DEBUG
	printf("bridged post buf %s\n", buf);
#endif 
	nvram_set(v->name, buf);

	//junzhao 2004.3.31
	if(!strcmp(l_dhcpenable, "0"))
    	{
		char dnsbuf[32];
		memset(dnsbuf, 0, sizeof(dnsbuf));
		sprintf(dnsbuf, "%s:%s", pdns, sdns);
		nvram_modify_status("wan_static_dns", dnsbuf);
		//junzhao 2004.4.13 for wan_dns
		nvram_set("wan_dns", "");
    	}
	else
		nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
		
	
	if(ipaddr)	
		free(ipaddr);
	if(netmask)	
		free(netmask);
	if(gateway)	
		free(gateway);
	if(pdns)	
		free(pdns);
	if(sdns)	
		free(sdns);
	return;
}

void validate_routed_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[100], *next;
	char buf[800], *cur=buf;
	char old[PVC_CONNECTIONS][100];
	char *ipaddr, *netmask, *gateway, *pdns, *sdns;
	char *connection;
	char temp[30], *val=NULL;
	struct variable routed_config_variables[] = {
                { longname: "Routed Static IP Address", argv: NULL },
                { longname: "Routed Static Netmask", argv: NULL },
                { longname: "Routed Static Gateway", argv: NULL },
                { longname: "Routed Static Primary Dns", argv: NULL },
                { longname: "Routed Static Secondary Dns", argv: NULL },
        };

	#ifdef LJZ_DEBUG
		printf("enter validate_routed_config\n");
	#endif

	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	ipaddr = malloc(20);
	memset(ipaddr, 0, sizeof(ipaddr));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "routed_ipaddr", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(ipaddr,val);
                        if(i<3)
				strcat(ipaddr,".");
                }
                else
		{
			free(ipaddr);
                        return ;
		}
        }
	netmask = malloc(20);
	memset(netmask, 0, sizeof(netmask));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "routed_netmask", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(netmask, val);
                        if(i<3) 
				strcat(netmask, ".");
                }
                else
		{	
			free(netmask);
                        return ;
		}
        }
	gateway = malloc(20);
	memset(gateway, 0, sizeof(gateway));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "routed_gateway", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(gateway, val);
                        if(i<3)
				strcat(gateway, ".");
                }
                else
		{
			free(gateway);
                        return ;
		}
        }
	pdns = malloc(20);
	memset(pdns, 0, sizeof(pdns));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "routed_pdns", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(pdns, val);
                        if(i<3) 
				strcat(pdns, ".");
                }
                else
		{	
			free(pdns);
                        return ;
		}
        }
	sdns = malloc(20);
	memset(sdns, 0, sizeof(sdns));
	memset(temp, 0, sizeof(temp));
        for(i=0 ; i<4 ; i++)
	{
                snprintf(temp, sizeof(temp), "%s_%d", "routed_sdns", i);
                val = websGetVar(wp, temp , NULL);
                if(val)
		{
                        strcat(sdns, val);
                        if(i<3)
				strcat(sdns, ".");
                }
                else
		{
			free(sdns);
                        return ;
		}
        }
	if (!*ipaddr) 
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify an IP Address<br>", v->longname);
                return;
        }
        if (!*netmask)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Subnet Mask<br>", v->longname);
                return;
        }
        if (!*gateway) 
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Gateway Address<br>", v->longname);
                return;
        }
        if (!*pdns)
	{
                websDebugWrite(wp, "Invalid <b>%s</b>: must specify a Primary DNS Address<br>", v->longname);
                return;
        }
	if (!valid_ipaddr(wp, ipaddr, &routed_config_variables[0]) || !valid_netmask(wp, netmask, &routed_config_variables[1]) || !valid_ipaddr(wp, gateway, &routed_config_variables[2]) || !valid_ipaddr(wp, pdns, &routed_config_variables[3]))
                return;
	if(*sdns)
	{
		if(!valid_ipaddr(wp, sdns, &routed_config_variables[4]))
			return;
	}

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("routed_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s", ipaddr, netmask, gateway, pdns, sdns);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

#ifdef LJZ_DEBUG
	printf("routed_post_buf %s\n", buf);
#endif 
	nvram_set(v->name, buf);
	
	//junzhao 2004.3.31
    {
	char dnsbuf[32];
	memset(dnsbuf, 0, sizeof(dnsbuf));
	sprintf(dnsbuf, "%s:%s", pdns, sdns);
	nvram_modify_status("wan_static_dns", dnsbuf);
	//junzhao 2004.4.13 for wan_dns
	nvram_set("wan_dns", "");
    }

	if(ipaddr)
		free(ipaddr);
	if(netmask)
		free(netmask);
	if(gateway)
		free(gateway);
	if(pdns)
		free(pdns);
	if(sdns)
		free(sdns);
	return;
}

void validate_mtu_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[8], *next;
	char buf[64], *cur=buf;
	char old[PVC_CONNECTIONS][8];
	char *mtuauto, *mtuvalue;
	char *connection;

	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;

	mtuauto = websGetVar(wp, "mtu_auto", NULL);
	mtuvalue = websGetVar(wp, "mtu_value", "1492");

	if(mtuauto == NULL)
		return;
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("mtu_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s", mtuauto, mtuvalue);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

#ifdef LJZ_DEBUG
	printf("mtu_post_buf %s\n", buf);
#endif 
	nvram_set(v->name, buf);
	return;
}

void validate_macclone_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[20], *next;
	char buf[128], *cur=buf;
	char old[PVC_CONNECTIONS][20];
	char *maccloneenable;
	char *connection;
	struct variable macclone_config_variables[] = {
                {name: "macclone_enable", longname: "MAC Clone Enable", argv: ARGV("0", "1")},
                {name: "macclone_addr",  longname: "MAC Hardware Address", argv: NULL },
        };

	connection = websGetVar(wp, "status_connection", NULL);
	if(connection == NULL)
		return;
	
	maccloneenable = websGetVar(wp, "macclone_enable", NULL);
	if(maccloneenable == NULL)
		return;
	
	if(!valid_range(wp, maccloneenable, &macclone_config_variables[0]))
		return;
	
	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("macclone_enable"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s", maccloneenable);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

#ifdef LJZ_DEBUG
	printf("macclone_enable_post_buf %s\n", buf);
#endif 
	nvram_set("macclone_enable", buf);

	if(!strcmp(maccloneenable, "1"))
	{
		char macaddr[20];
		get_merge_mac(macclone_config_variables[1].name, macaddr);
		if(valid_hwaddr(wp, macaddr, &macclone_config_variables[1]))
		{
			cur = buf;
			memset(word, 0, sizeof(word));
			memset(buf, 0, sizeof(buf));
			for(i=0 ; i<PVC_CONNECTIONS ; i++)
				memset(old[i], 0, sizeof(old[i]));
	
			i = 0;
			foreach(word, nvram_safe_get("macclone_addr"), next) 
			{
				strcpy(old[i], word);
				i++;
			}
			snprintf(old[atoi(connection)], sizeof(old[0]), "%s", macaddr);
	
			for(i=0 ; i<PVC_CONNECTIONS ; i++)
			{
				if(strcmp(old[i],""))
					cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s", cur == buf ? "" : " ", old[i]);
			}

		#ifdef LJZ_DEBUG
			printf("macclone_addr_buf %s\n", buf);
		#endif 
			nvram_set("macclone_addr", buf);
		}	
	}	
	return;
}

#ifdef MPPPOE_SUPPORT
void mpppoe_config_init()
{
	char *ptr = nvram_safe_get("mpppoe_config"), *tmp;
	int which, i;
	
	//memset(mpppoe_config_buf, 0, sizeof(mpppoe_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(mpppoe_config_buf, nvram_safe_get("mpppoe_config"), next) 
	{
		if (which-- == 0) 
#endif
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(mpppoe_config_buf)
				free(mpppoe_config_buf);
			mpppoe_config_buf = strdup(tmp);

			m_password = mpppoe_config_buf;
			m_username = strsep(&m_password, ":");
			if (!m_username || !m_password)
				break;
			m_demand = m_password;
			m_password = strsep(&m_demand, ":");
			if (!m_demand || !m_password)
				break;
			m_idletime = m_demand;
			m_demand = strsep(&m_idletime, ":");
			if (!m_demand || !m_idletime)
				break;
			m_redialperiod = m_idletime;
			m_idletime = strsep(&m_redialperiod, ":");
			if (!m_idletime || !m_redialperiod)
				break;
			m_servicename = m_redialperiod;
			m_redialperiod = strsep(&m_servicename, ":");
			if (!m_redialperiod || !m_servicename)
				break;
			m_domainname = m_servicename;
			m_servicename = strsep(&m_domainname, ":");
			if (!m_servicename || !m_domainname)
				break;

		#ifdef LJZ_DEBUG
			printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s, servicename=%s, domainname=%s\n", m_username, m_password, m_demand, m_idletime, m_redialperiod, m_servicename, m_domainname);
		#endif	
			break;
		} 
	}
	mpppoe_inited = 1;
}

/*
 * Example: 
 * mpppoe_config = username:password:demand:idletime:redialperiod:servicename:domainname
 * mpppoe_config = adsl:adsl1234:1:30:180:servicename:domainname
 * <% mpppoe_config("username", 0); %> produces "adsl"
 */
int ej_mpppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(mpppoe_inited == 0)
		mpppoe_config_init();

	if(!strcmp(arg, "username"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_username[64];
		memset(new_username, 0, sizeof(new_username));
		filter_name(m_username, new_username, sizeof(new_username), GET);
		return websWrite(wp, "%s", new_username);
		//return websWrite(wp, "%s", m_username);
	}
	if(!strcmp(arg, "password"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_password[64];
		memset(new_password, 0, sizeof(new_password));
		filter_name(m_password, new_password, sizeof(new_password), GET);
		return websWrite(wp, "%s", new_password);
		//return websWrite(wp, "%s", m_password);
	}
	if(!strcmp(arg, "servicename"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_servicename[64];
		memset(new_servicename, 0, sizeof(new_servicename));
		filter_name(m_servicename, new_servicename, sizeof(new_servicename), GET);
		return websWrite(wp, "%s", new_servicename);
		//return websWrite(wp, "%s", m_servicename);
	}
	if(!strcmp(arg, "domainname"))
		return websWrite(wp, "%s", m_domainname);

	if(!strcmp(arg, "demand"))
	{
		if(((count == DEMANDENABLE) && (!strcmp(m_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(m_demand, "0"))))
			return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "idletime"))
	{
		if(count == 0)
		{
			if(!strcmp(m_demand, "1"))
				return websWrite(wp, "%s", m_idletime);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(m_demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		if(count == 0)
		{
			if(!strcmp(m_demand, "0"))
				return websWrite(wp, "%s", m_redialperiod);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(m_demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

/*
 * Example: 
 * mpppoe_enable 
 * mpppoe_enable = 1
 */
int ej_mpppoe_enable_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;
	char word[3], *next, *mpppoe_enable;
	int which;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	if(gozila_action && !connchanged)
	{
		mpppoe_enable = websGetVar(wp, "mpppoe_enable", NULL);
		if(mpppoe_enable == NULL)
		{
			foreach(word, nvram_safe_get("mpppoe_enable"), next) 
			{
				if(which-- == 0) 
				{
					mpppoe_enable = word;
					break;
				}
			}
		}
	}
	else
	{	
		foreach(word, nvram_safe_get("mpppoe_enable"), next) 
		{
			if(which-- == 0) 
			{
				mpppoe_enable = word;
				break;
			}
		}
	}
	if(((!strcmp(mpppoe_enable, "1")) && (count == 1)) || ((!strcmp(mpppoe_enable, "0")) && (count == 0)))
		return websWrite(wp, "%s", "selected");
	else if((!strcmp(mpppoe_enable, "0")) && (count == 2))
		return websWrite(wp, "%s", "<!--");
	else if((!strcmp(mpppoe_enable, "0")) && (count == 3))
		return websWrite(wp, "%s", "-->");
		
	return -1;
}

//junzhao 2005.5.16 for ':' setting
void validate_mpppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64], new_servicename[5*64];
	char *ptr, *tmp;
	int mpppoe_config_size = 0, mpppoe_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename, *l_domainname, *l_enable;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_enable = websGetVar(wp, "mpppoe_enable", NULL);
	nvram_modify_status("mpppoe_enable", l_enable);
	if(!strcmp(l_enable, "0"))
	{
		error_value = 0;
		return;
	}

	l_username = websGetVar(wp, "mpppoe_username", "");
	l_password = websGetVar(wp, "mpppoe_password", "");
	l_demand = websGetVar(wp, "mpppoe_demand", "1");
	l_idletime = websGetVar(wp, "mpppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "mpppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "mpppoe_servicename", "");
	l_domainname = websGetVar(wp, "mpppoe_domainname", "");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	filter_name(l_servicename, new_servicename, sizeof(new_servicename), SET);

	mpppoe_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + strlen(new_servicename) 
			+ strlen(l_domainname) + 6;
	
	ptr = nvram_safe_get("mpppoe_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(mpppoe_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, mpppoe_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod, new_servicename, l_domainname);
		}
		else
			old[i] = strdup(tmp);
		mpppoe_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(mpppoe_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, mpppoe_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}
#if 0
void validate_mpppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[280], *next;
	char buf[2240], *cur=buf;
	char old[PVC_CONNECTIONS][280];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename, *l_domainname, *l_enable;
	char *connection;
	struct variable mpppoe_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
		{ longname: "Service Name", argv: ARGV("63") },	
		{ longname: "Domain Name", argv: ARGV("63") },	
	//	{ longname: "MPPPoE Enable", argv: ARGV("0", "1")},	
	};
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;
	
	l_enable = websGetVar(wp, "mpppoe_enable", NULL);
	//if (!valid_choice(wp, l_enable, &mpppoe_config_variables[7]))
         //       return;
	
	nvram_modify_status("mpppoe_enable", l_enable);
	if(!strcmp(l_enable, "0"))
	{
		error_value = 0;
		return;
	}
	l_username = websGetVar(wp, "mpppoe_username", NULL);
	l_password = websGetVar(wp, "mpppoe_password", NULL);
	l_demand = websGetVar(wp, "mpppoe_demand", NULL);
	l_idletime = websGetVar(wp, "mpppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "mpppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "mpppoe_servicename", NULL);
	l_domainname = websGetVar(wp, "mpppoe_domainname", NULL);
	
	if(!valid_name(wp, l_username, &mpppoe_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &mpppoe_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &mpppoe_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &mpppoe_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &mpppoe_config_variables[4]))
			return;
	}
	//if(!valid_name(wp, l_servicename, &mpppoe_config_variables[5]))
	//	return;
	//if(!valid_name(wp, l_domainname, &mpppoe_config_variables[6]))
	//	return;

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("mpppoe_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod, l_servicename, l_domainname);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//nvram_modify_status("mpppoe_enable", l_enable);
	
	//2004.3.31 junzhao
	//nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	return;
}
#endif
#endif

#ifdef IPPPOE_SUPPORT
void ipppoe_config_init()
{
	char *ptr = nvram_safe_get("ipppoe_config"), *tmp;
	int which, i;
	
	//memset(ipppoe_config_buf, 0, sizeof(ipppoe_config_buf));
	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

#if 0
	foreach(ipppoe_config_buf, nvram_safe_get("ipppoe_config"), next) 
	{
		if (which-- == 0) 
		{
#endif
	//junzhao 2005.5.16 for ':' setting
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if (which == i) 
		{
			if(ipppoe_config_buf)
				free(ipppoe_config_buf);
			ipppoe_config_buf = strdup(tmp);
		
			i_password = ipppoe_config_buf;
			i_username = strsep(&i_password, ":");
			if (!i_username || !i_password)
				break;
			i_demand = i_password;
			i_password = strsep(&i_demand, ":");
			if (!i_demand || !i_password)
				break;
			i_idletime = i_demand;
			i_demand = strsep(&i_idletime, ":");
			if (!i_demand || !i_idletime)
				break;
			i_redialperiod = i_idletime;
			i_idletime = strsep(&i_redialperiod, ":");
			if (!i_idletime || !i_redialperiod)
				break;
			i_servicename = i_redialperiod;
			i_redialperiod = strsep(&i_servicename, ":");
			if (!i_redialperiod || !i_servicename)
				break;
			i_domainname = i_servicename;
			i_servicename = strsep(&i_domainname, ":");
			if (!i_servicename || !i_domainname)
				break;

		#ifdef LJZ_DEBUG
			printf("username=%s, password=%s, demand=%s, idletime=%s, redialperiod=%s, servicename=%s, domainname=%s\n", i_username, i_password, i_demand, i_idletime, i_redialperiod, i_servicename, i_domainname);
		#endif	
			break;
		} 
	}
	ipppoe_inited = 1;
}

/*
 * Example: 
 * mpppoe_config = username:password:demand:idletime:redialperiod:servicename:domainname
 * mpppoe_config = adsl:adsl1234:1:30:180:servicename:domainname
 * <% mpppoe_config("username", 0); %> produces "adsl"
 */
int ej_ipppoe_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if(ipppoe_inited == 0)
		ipppoe_config_init();

	if(!strcmp(arg, "username"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_username[64];
		memset(new_username, 0, sizeof(new_username));
		filter_name(i_username, new_username, sizeof(new_username), GET);
		return websWrite(wp, "%s", new_username);
		//return websWrite(wp, "%s", i_username);
	}
	if(!strcmp(arg, "password"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_password[64];
		memset(new_password, 0, sizeof(new_password));
		filter_name(i_password, new_password, sizeof(new_password), GET);
		return websWrite(wp, "%s", new_password);
		//return websWrite(wp, "%s", i_password);
	}
	if(!strcmp(arg, "servicename"))
	{
		//junzhao 2005.5.16 for ':' setting
		char new_servicename[64];
		memset(new_servicename, 0, sizeof(new_servicename));
		filter_name(i_servicename, new_servicename, sizeof(new_servicename), GET);
		return websWrite(wp, "%s", new_servicename);
		//return websWrite(wp, "%s", i_servicename);
	}
	if(!strcmp(arg, "domainname"))
		return websWrite(wp, "%s", i_domainname);

	if(!strcmp(arg, "demand"))
	{
		if(((count == DEMANDENABLE) && (!strcmp(i_demand, "1"))) || ((count == DEMANDDISABLE) && (!strcmp(i_demand, "0"))))
			return websWrite(wp, "checked");
	}
	if(!strcmp(arg, "idletime"))
	{
		if(count == 0)
		{
			if(!strcmp(i_demand, "1"))
				return websWrite(wp, "%s", i_idletime);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(i_demand, "0"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	if(!strcmp(arg, "redialperiod"))
	{
		if(count == 0)
		{
			if(!strcmp(i_demand, "0"))
				return websWrite(wp, "%s", i_redialperiod);
			else
				return websWrite(wp, "%s", "20");
		}
		else if(count == 1)
		{
			if(!strcmp(i_demand, "1"))
				return websWrite(wp, "%s", "disabled");
		}	
	}
	return -1;
}

/*
 * Example: 
 * mpppoe_enable 
 * mpppoe_enable = 1
 */
int ej_ipppoe_enable_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int  count;
	char word[3], *next, *ipppoe_enable;
	int which;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(connection, "")) 
		strcpy(connection, "0");
	which = atoi(connection);

	if(gozila_action && !connchanged)
	{
		ipppoe_enable = websGetVar(wp, "ipppoe_enable", NULL);
		if(ipppoe_enable == NULL)
		{
			foreach(word, nvram_safe_get("ipppoe_enable"), next) 
			{
				if(which-- == 0) 
				{
					ipppoe_enable = word;
					break;
				}
			}
		}
	}
	else
	{	
		foreach(word, nvram_safe_get("ipppoe_enable"), next) 
		{
			if(which-- == 0) 
			{
				ipppoe_enable = word;
				break;
			}
		}
	}
	if(((!strcmp(ipppoe_enable, "1")) && (count == 1)) || ((!strcmp(ipppoe_enable, "0")) && (count == 0)))
		return websWrite(wp, "%s", "selected");
	else if((!strcmp(ipppoe_enable, "0")) && (count == 2))
		return websWrite(wp, "%s", "<!--");
	else if((!strcmp(ipppoe_enable, "0")) && (count == 3))
		return websWrite(wp, "%s", "-->");
		
	return -1;
}

//junzhao 2005.5.16 for ':' setting
void validate_ipppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char *buf = NULL, *cur;
	char *old[PVC_CONNECTIONS];
	char new_username[5*64], new_password[5*64], new_servicename[5*64];
	char *ptr, *tmp;
	int ipppoe_config_size = 0, ipppoe_config_totalsize = 0;

	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename, *l_domainname, *l_enable;
	char *connection;
	
	connection = websGetVar(wp, "wan_connection", "0");

	l_enable = websGetVar(wp, "ipppoe_enable", NULL);
	nvram_modify_status("ipppoe_enable", l_enable);
	if(!strcmp(l_enable, "0"))
	{
		error_value = 0;
		return;
	}

	l_username = websGetVar(wp, "ipppoe_username", "");
	l_password = websGetVar(wp, "ipppoe_password", "");
	l_demand = websGetVar(wp, "ipppoe_demand", "1");
	l_idletime = websGetVar(wp, "ipppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "ipppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "ipppoe_servicename", "");
	l_domainname = websGetVar(wp, "ipppoe_domainname", "");

	memset(new_username, 0, sizeof(new_username));
	memset(new_password, 0, sizeof(new_password));
	memset(new_servicename, 0, sizeof(new_servicename));
	filter_name(l_username, new_username, sizeof(new_username), SET);
	filter_name(l_password, new_password, sizeof(new_password), SET);
	filter_name(l_servicename, new_servicename, sizeof(new_servicename), SET);

	ipppoe_config_size = strlen(new_username) + strlen(new_password) 
			+ strlen(l_demand) + strlen(l_idletime) 
			+ strlen(l_redialperiod) + strlen(new_servicename) 
			+ strlen(l_domainname) + 6;
	
	ptr = nvram_safe_get("ipppoe_config");
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		tmp = strsep(&ptr, " ");
		if(i == atoi(connection))
		{
			old[i] = (char *)malloc(ipppoe_config_size + 1);
			if(!old[i])
				return;
			memset(old[i], 0, ipppoe_config_size + 1);
			sprintf(old[i], "%s:%s:%s:%s:%s:%s:%s", new_username, new_password, l_demand, l_idletime, l_redialperiod, new_servicename, l_domainname);
		}
		else
			old[i] = strdup(tmp);
		ipppoe_config_totalsize += strlen(old[i]);
	}
	
	buf = (char *)malloc(ipppoe_config_totalsize + PVC_CONNECTIONS);
	if(!buf) 
		goto finalfree;
	memset(buf, 0, ipppoe_config_totalsize + PVC_CONNECTIONS);
	cur = buf;
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(old[i])
			cur += sprintf(cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	printf(".............buf...... %s\n", buf);

finalfree:
	if(buf)
	{
		free(buf);
		buf = NULL;
	}
	for(i=0; i<PVC_CONNECTIONS; i++)
	{
		if(old[i])
		{
			free(old[i]);
			old[i] = NULL;
		}
	}
	return;
}


#if 0
void validate_ipppoe_config(webs_t wp, char *value, struct variable *v)
{
	int i;
	char word[280], *next;
	char buf[2240], *cur=buf;
	char old[PVC_CONNECTIONS][280];
	char *l_username, *l_password, *l_demand, *l_idletime, *l_redialperiod, *l_servicename, *l_domainname, *l_enable;
	char *connection;
	struct variable ipppoe_config_variables[] = {
		{ longname: "Username", argv: ARGV("63") },	
		{ longname: "Password", argv: ARGV("63") },	
		{ longname: "Connect on Demand", argv: ARGV("0", "1") },
		{ longname: "Max Idle Time", argv: ARGV("1", "9999") },
		{ longname: "Redial Period", argv: ARGV("20", "180") },
		{ longname: "Service Name", argv: ARGV("63") },	
		{ longname: "Domain Name", argv: ARGV("63") },	
	//	{ longname: "MPPPoE Enable", argv: ARGV("0", "1")},	
	};
	
	connection = websGetVar(wp, "wan_connection", NULL);
	if(connection == NULL)
		return;
	
	l_enable = websGetVar(wp, "ipppoe_enable", NULL);
	//if (!valid_choice(wp, l_enable, &mpppoe_config_variables[7]))
         //       return;
	
	nvram_modify_status("ipppoe_enable", l_enable);
	if(!strcmp(l_enable, "0"))
	{
		error_value = 0;
		return;
	}
	l_username = websGetVar(wp, "ipppoe_username", NULL);
	l_password = websGetVar(wp, "ipppoe_password", NULL);
	l_demand = websGetVar(wp, "ipppoe_demand", NULL);
	l_idletime = websGetVar(wp, "ipppoe_idletime", "20");
	l_redialperiod = websGetVar(wp, "ipppoe_redialperiod", "20");
	l_servicename = websGetVar(wp, "ipppoe_servicename", NULL);
	l_domainname = websGetVar(wp, "ipppoe_domainname", NULL);
	
	if(!valid_name(wp, l_username, &ipppoe_config_variables[0]))
		return;
	if(!valid_name(wp, l_password, &ipppoe_config_variables[1]))
		return;
	if(!valid_choice(wp, l_demand, &ipppoe_config_variables[2]))
		return;
	if(!strcmp(l_demand, "1"))
	{
		if(!valid_range(wp, l_idletime, &ipppoe_config_variables[3]))
			return;
	}
	else
	{
		if(!valid_range(wp, l_redialperiod, &ipppoe_config_variables[4]))
			return;
	}
	//if(!valid_name(wp, l_servicename, &mpppoe_config_variables[5]))
	//	return;
	//if(!valid_name(wp, l_domainname, &mpppoe_config_variables[6]))
	//	return;

	memset(word, 0, sizeof(word));
	memset(buf, 0, sizeof(buf));
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
		memset(old[i], 0, sizeof(old[i]));
	
	i = 0;
	foreach(word, nvram_safe_get("ipppoe_config"), next) 
	{
		strcpy(old[i], word);
		i++;
	}
	snprintf(old[atoi(connection)], sizeof(old[0]), "%s:%s:%s:%s:%s:%s:%s", l_username, l_password, l_demand, l_idletime, l_redialperiod, l_servicename, l_domainname);
	
	for(i=0 ; i<PVC_CONNECTIONS ; i++)
	{
		if(strcmp(old[i],""))
			cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
				cur == buf ? "" : " ", old[i]);
	}

	nvram_set(v->name, buf);
	//nvram_modify_status("mpppoe_enable", l_enable);
	
	//2004.3.31 junzhao
	//nvram_modify_status("wan_static_dns", "0.0.0.0:0.0.0.0");
	return;
}
#endif
#endif

int ej_write_connection_table(webs_t wp, int active_index)
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
					which_to_select = (1<<PPP_MASK) | i;   //ppp flag
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
					which_to_select = (1<<PPP_MASK) | (1<<MUL_MASK) | i;   //ppp flag
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
					which_to_select = (1<<MUL_MASK) | i;   //ppp flag
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
//junzhao 2004.6.1
int ej_active_connection_table(int eid, webs_t wp, int argc, char_t **argv)
{
/*
	int i, selectable, ret = 0, which_to_select, last_select = 0, has_match = 0;
	char word[PVC_CONNECTIONS][6], readbuf[60];*/
	int last_select = 0;
	if(gozila_action)
		last_select = atoi(websGetVar(wp, "active_connection_selection", "32"));
	else
		last_select = atoi(nvram_safe_get("active_connection_selection"));
	return (ej_write_connection_table(wp, last_select));
/*
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
				if(check_ppp_link(i))
				{
					selectable = 1;
					which_to_select = (1<<PPP_MASK) | i;   //ppp flag
				}
			}
			else if(strstr(word[i], "nas"))
			{
				selectable = 1;
				which_to_select = i;
			}
		}
		if(selectable)
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", which_to_select, (which_to_select == last_select)?"selected" : "", i, word[i]);
		if(which_to_select == last_select)
			has_match = 1;
	}

#if defined(MPPPOE_SUPPORT)
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
				if(check_ppp_link(i + 8))
				{
					selectable = 1;
					which_to_select = (1<<PPP_MASK) | (1<<MUL_MASK) | i;   //ppp flag
				}
			}
		}
		#if defined(IPPPOE_SUPPORT)
		else
		{
			char tmp[6], *next;
			int which = i;
			memset(tmp, 0, sizeof(tmp));
			if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	       		 	return 0;
			}
			foreach(tmp, readbuf, next)
			{
				if(which -- == 0)
					break;
			}
			if(strstr(tmp, "ppp"))
			{
				if(check_ppp_link(i + 8))
				{
					selectable = 1;
					which_to_select = (1<<MUL_MASK) | i;   //ppp flag
				}
			}
		}
		#endif
		if(selectable)
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>%s %d(%s)</option>\n", which_to_select, (which_to_select == last_select)?"selected" : "", (which_to_select & (1<<PPP_MASK))?"MULTIPLE PPPOE":"IP PPPOE", i, word[i]);

		if(which_to_select == last_select)
			has_match = 1;
	}
#endif
	if(!has_match)
		ret += websWrite(wp,"\t\t<option value=\"32\" selected>Please Select:</option>\n");

	return ret;*/
}

int ej_dump_valid_parts(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, j, ret = 0;

	for(i=0; i<10; i++)
	{
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.dstipaddr_%d_%d,0,253,\"DstIp\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.dstnetmask_%d_%d,0,255,\"DstNetmask\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.srcipaddr_%d_%d,0,253,\"SrcIp\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.srcnetmask_%d_%d,0,255,\"SrcNetmask\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		ret += websWrite(wp, "\tif(!my_valid_macs_17(document.pvcselect.srcmac_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.dstport_%d,0,65535,\"DstPort\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n", i);
		ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.srcport_%d,0,65535,\"SrcPort\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");

		//2004.9.16
		ret += websWrite(wp, "\tif(!my_valid_cos(document.pvcselect.cos_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_type(document.pvcselect.type_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_range(document.pvcselect.vid_%d,0,4095,\"VlanID\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_length(document.pvcselect.lengthmin_%d,document.pvcselect.lengthmax_%d))\n\t{\n", i, i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");

		ret += websWrite(wp, "\tif(!my_valid_dscp(document.pvcselect.tos_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
	}		
	ret += websWrite(wp, "\treturn true;\n");
	return ret;
}	

int ej_dump_match_condition(int eid, webs_t wp, int argc, char_t **argv)
{
	int which, i, ret = 0;
	//2004.9.16
	//char word[100], *next;
	char word[200], *next;
	
	int enable, which_conf;
	char conf_name[20], enable_name[22];

	if(gozila_action)
	{
		which_conf = atoi(websGetVar(wp, "active_connection_selection", "32"));
	}
	else
		which_conf = atoi(nvram_safe_get("active_connection_selection"));

	if(which_conf >= 1<<5)
	{
		for(i=0; i<10; i++)
		{
			int j;
			for(j=0; j<4; j++)
			{
				ret += websWrite(wp, "\tdocument.pvcselect.dstipaddr_%d_%d.value=\"%d\";\n", i, j, 0);
				ret += websWrite(wp, "\tdocument.pvcselect.dstnetmask_%d_%d.value=\"%d\";\n", i, j, 0);
				ret += websWrite(wp, "\tdocument.pvcselect.srcipaddr_%d_%d.value=\"%d\";\n", i, j, 0);
				ret += websWrite(wp, "\tdocument.pvcselect.srcnetmask_%d_%d.value=\"%d\";\n", i, j, 0);
			}
			ret += websWrite(wp, "\tdocument.pvcselect.srcmac_%d.value=\"%s\";\n", i, "00-00-00-00-00-00");
			ret += websWrite(wp, "\tdocument.pvcselect.srcport_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.dstport_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.tos_%d.value=\"%s\";\n", i, "0");

			//2004.9.16
			ret += websWrite(wp, "\tdocument.pvcselect.cos_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.type_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.vid_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.lengthmin_%d.value=\"%s\";\n", i, "0");
			ret += websWrite(wp, "\tdocument.pvcselect.lengthmax_%d.value=\"%s\";\n", i, "0");

			ret += websWrite(wp, "\tdocument.pvcselect.pro_%d[%d].selected=true;\n", i, 0);
			ret += websWrite(wp, "\tdocument.pvcselect.enable_%d.checked=false;\n", i);
			ret += websWrite(wp, "\tif(document.pvcselect.pro_%d.selectedIndex==0)\n", i);
			ret += websWrite(wp, "\t{\n\t\tdocument.pvcselect.dstport_%d.disabled=true;\n", i);
			ret += websWrite(wp, "\t\tdocument.pvcselect.srcport_%d.disabled=true;\n\t}\n", i);
			ret += websWrite(wp, "\telse\n", i);
			ret += websWrite(wp, "\t{\n\t\tdocument.pvcselect.dstport_%d.disabled=false;\n", i);		
			ret += websWrite(wp, "\t\tdocument.pvcselect.srcport_%d.disabled=false;\n\t}\n", i);		
		}
		return 0;
	}

	memset(conf_name, 0, sizeof(conf_name));
	memset(enable_name, 0, sizeof(enable_name));

	if((which_conf & (1<<MUL_MASK)) == 0)
	{
		sprintf(conf_name, "pvc_select_conf_%d", (which_conf & ((1<<MUL_MASK) - 1)));
		sprintf(enable_name, "pvc_select_enable_%d", (which_conf & ((1<<MUL_MASK) - 1)));
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else
	{
		sprintf(conf_name, "mul_select_conf_%d", (which_conf & ((1<<MUL_MASK) - 1)));
		sprintf(enable_name, "mul_select_enable_%d", (which_conf & ((1<<MUL_MASK) - 1)));
	}
#endif
	enable = atoi(nvram_safe_get(enable_name));

	for(i=0; i<10; i++)
	{
		//if((enable & (1<<i)) == 0)
		//	continue;
		which = i;
		foreach(word, nvram_safe_get(conf_name), next)
		{
			if(which -- == 0)
			{
				//2004.9.16
				//char dstipaddr[16], dstnetmask[16], srcipaddr[16], srcnetmask[16], srcmac[18], pro[2], srcport[6], dstport[6], tos[3];
				char dstipaddr[16], dstnetmask[16], srcipaddr[16], srcnetmask[16], srcmac[18], pro[2], srcport[6], dstport[6], tos[3], cos[3], type[5], vid[5], lengthmin[6], lengthmax[6];

				int j;
				memset(dstipaddr, 0, sizeof(dstipaddr));
				memset(dstnetmask, 0, sizeof(dstnetmask));
				memset(srcipaddr, 0, sizeof(srcipaddr));
				memset(srcnetmask, 0, sizeof(srcnetmask));
				memset(srcmac, 0, sizeof(srcmac));
				memset(pro, 0, sizeof(pro));
				memset(srcport, 0, sizeof(srcport));
				memset(dstport, 0, sizeof(dstport));
				memset(tos, 0, sizeof(tos));

				//2004.9.16
				memset(cos, 0, sizeof(cos));
				memset(type, 0, sizeof(type));
				memset(vid, 0, sizeof(vid));
				memset(lengthmin, 0, sizeof(lengthmin));
				memset(lengthmax, 0, sizeof(lengthmax));
				//2004.9.16
				//sscanf(word, "%16[^:]:%16[^:]:%16[^:]:%16[^:]:%18[^:]:%2[^:]:%6[^:]:%6[^:]:%3[^\n]", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos);
				sscanf(word, "%16[^:]:%16[^:]:%16[^:]:%16[^:]:%18[^:]:%2[^:]:%6[^:]:%6[^:]:%3[^:]:%3[^:]:%5[^:]:%5[^:]:%6[^:]:%6[^\n]", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos, cos, type, vid, lengthmin, lengthmax);

				for(j=0; j<4; j++)
				{
					ret += websWrite(wp, "\tdocument.pvcselect.dstipaddr_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstipaddr, j));
					ret += websWrite(wp, "\tdocument.pvcselect.dstnetmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstnetmask, j));
					ret += websWrite(wp, "\tdocument.pvcselect.srcipaddr_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcipaddr, j));
					ret += websWrite(wp, "\tdocument.pvcselect.srcnetmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcnetmask, j));
				}
				ret += websWrite(wp, "\tdocument.pvcselect.srcmac_%d.value=\"%s\";\n", i, srcmac);
				ret += websWrite(wp, "\tdocument.pvcselect.srcport_%d.value=\"%s\";\n", i, srcport);
				ret += websWrite(wp, "\tdocument.pvcselect.dstport_%d.value=\"%s\";\n", i, dstport);
				ret += websWrite(wp, "\tdocument.pvcselect.pro_%d[%d].selected=true;\n", i, atoi(pro));
				ret += websWrite(wp, "\tdocument.pvcselect.tos_%d.value=\"%s\";\n", i, tos);

				//2004.9.16
				ret += websWrite(wp, "\tdocument.pvcselect.cos_%d.value=\"%s\";\n", i, cos);
				ret += websWrite(wp, "\tdocument.pvcselect.type_%d.value=\"%s\";\n", i, type);
				ret += websWrite(wp, "\tdocument.pvcselect.vid_%d.value=\"%s\";\n", i, vid);
				ret += websWrite(wp, "\tdocument.pvcselect.lengthmin_%d.value=\"%s\";\n", i, lengthmin);
				ret += websWrite(wp, "\tdocument.pvcselect.lengthmax_%d.value=\"%s\";\n", i, lengthmax);
				
				ret += websWrite(wp, "\tif(document.pvcselect.pro_%d.selectedIndex==0)\n", i);
				ret += websWrite(wp, "\t{\n\t\tdocument.pvcselect.dstport_%d.disabled=true;\n", i);
				ret += websWrite(wp, "\t\tdocument.pvcselect.srcport_%d.disabled=true;\n\t}\n", i);
				ret += websWrite(wp, "\telse\n", i);
				ret += websWrite(wp, "\t{\n\t\tdocument.pvcselect.dstport_%d.disabled=false;\n", i);		
				ret += websWrite(wp, "\t\tdocument.pvcselect.srcport_%d.disabled=false;\n\t}\n", i);		


				if((enable & (1<<i)) == 0)
					ret += websWrite(wp, "\tdocument.pvcselect.enable_%d.checked=false;\n", i);
				else
					ret += websWrite(wp, "\tdocument.pvcselect.enable_%d.checked=true;\n", i);
				break;
			}
		}
	}
	return ret;
}

void validate_pvc_selection_config(webs_t wp, char *value, struct variable *v)
{
	int i,j, enable = 0;
	//2004.9.16
	//char word[100], buf[1000], *cur=buf, conf_name[20], enable_name[22];
	char word[200], buf[1600], *cur=buf, conf_name[20], enable_name[22];

	char *srcipaddr, *srcnetmask, *dstipaddr, *dstnetmask;
	char temp[30], *val=NULL;
	char enable_str[5];
	int which_to_select = atoi(websGetVar(wp, "active_connection_selection", "32"));

	if(which_to_select >= 32)
		return;

	memset(conf_name, 0, sizeof(conf_name));
	memset(enable_name, 0, sizeof(enable_name));
	if((which_to_select & (1<<MUL_MASK)) == 0)
	{
		sprintf(conf_name, "pvc_select_conf_%d", (which_to_select & ((1<<MUL_MASK) - 1)));
		sprintf(enable_name, "pvc_select_enable_%d", (which_to_select & ((1<<MUL_MASK) - 1)));
	}
#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
	else
	{
		sprintf(conf_name, "mul_select_conf_%d", (which_to_select & ((1<<MUL_MASK) - 1)));
		sprintf(enable_name, "mul_select_enable_%d", (which_to_select & ((1<<MUL_MASK) - 1)));
	}
#endif
	dstnetmask = malloc(20);
	srcipaddr = malloc(20);
	dstipaddr = malloc(20);
	srcnetmask = malloc(20);

	memset(buf, 0, sizeof(buf));

	for(i=0; i<10; i++)
	{
		char tmp_name[15], *srcmac, *pro, *srcport, *dstport, *tos, *ena;
		//2004.9.16
		char *cos, *type, *vid, *lengthmin, *lengthmax;

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "srcmac_%d", i);
		srcmac = websGetVar(wp, tmp_name, "0");

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "pro_%d", i);
		pro = websGetVar(wp, tmp_name, "0");

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "srcport_%d", i);
		srcport = websGetVar(wp, tmp_name, "0");

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "dstport_%d", i);
		dstport = websGetVar(wp, tmp_name, "0");

		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "tos_%d", i);
		tos = websGetVar(wp, tmp_name, "0");

		//2004.9.16
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "cos_%d", i);
		cos = websGetVar(wp, tmp_name, "0");
		
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "type_%d", i);
		type = websGetVar(wp, tmp_name, "0");
		
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "vid_%d", i);
		vid = websGetVar(wp, tmp_name, "0");
		
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "lengthmin_%d", i);
		lengthmin = websGetVar(wp, tmp_name, "0");
		
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "lengthmax_%d", i);
		lengthmax = websGetVar(wp, tmp_name, "0");

		
		memset(tmp_name, 0, sizeof(tmp_name));
		sprintf(tmp_name, "enable_%d", i);
		ena = websGetVar(wp, tmp_name, "0");

		if(!strcmp(ena, "1"))
			enable |= (1<<i);

		memset(temp, 0, sizeof(temp));
		memset(dstipaddr, 0, sizeof(dstipaddr));
		for(j=0 ; j<4 ; j++)
		{
               		snprintf(temp, sizeof(temp), "dstipaddr_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(dstipaddr,val);
                        	if(j<3)
					strcat(dstipaddr,".");
                	}
                	else
			{
				free(dstipaddr);
                        	return ;
			}
        	}

		memset(dstnetmask, 0, sizeof(dstnetmask));
		memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "dstnetmask_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(dstnetmask,val);
                        	if(j<3)
					strcat(dstnetmask,".");
                	}
                	else
			{
				free(dstnetmask);
                        	return ;
			}
        	}

		memset(srcipaddr, 0, sizeof(srcipaddr));
		memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "srcipaddr_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(srcipaddr, val);
                        	if(j<3)
					strcat(srcipaddr, ".");
                	}
                	else
			{
				free(srcipaddr);
                        	return ;
			}
        	}

        	memset(srcnetmask, 0, sizeof(srcnetmask));
        	memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "srcnetmask_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(srcnetmask, val);
                        	if(j<3)
					strcat(srcnetmask, ".");
                	}
                	else
			{
				free(srcnetmask);
                        	return ;
			}
        	}

		if (!*dstipaddr || !*dstnetmask || !*srcipaddr || !*srcnetmask)
                	return;

		memset(word, 0, sizeof(word));
		
		//2004.9.16
		//sprintf(word, "%s:%s:%s:%s:%s:%s:%s:%s:%s", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos);
		sprintf(word, "%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s", dstipaddr, dstnetmask, srcipaddr, srcnetmask, srcmac, pro, srcport, dstport, tos, cos, type, vid, lengthmin, lengthmax);

		cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s", cur == buf ? "" : " ", word);
	}

	printf("haha...my post buf %s, enable %x\n", buf, enable);
	nvram_set(conf_name, buf);
	memset(enable_str, 0, sizeof(enable_str));
	sprintf(enable_str, "%d", enable);
	nvram_set(enable_name, enable_str);

	if(dstipaddr)
		free(dstipaddr);
	if(dstnetmask)
		free(dstnetmask);
	if(srcipaddr)
		free(srcipaddr);
	if(srcnetmask)
		free(srcnetmask);

	return;
}
