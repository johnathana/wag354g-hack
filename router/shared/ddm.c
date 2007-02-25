
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
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cyutils.h>

#include "broadcom.h"
#include "pvc.h"
extern char connection[5];  // seleted connetion index (global)
char global_wan_ip[20];
char global_wan_netmask[20];

#define nvram_safe_get(name) (nvram_get(name)?:"")

//================================================================================================
//extern int auth_fail;
int auth_fail;
#if 0
int string_rep(char *buff, int index, char *word)
{
	char *ptrstart = buff;
	char *ptrend;
	int i;
	for(i=0; i<index-1; i++)
	{
		ptrstart = strstr(ptrstart, ":");
		if(ptrstart != NULL)
			ptrstart++;
		else
			return -1;
	}
	ptrend = strstr(ptrstart, ":");
	if(ptrend == NULL)
	{
		memcpy(ptrstart, word, strlen(word));
		ptrstart += strlen(word) + 1;
		*ptrstart = '\0';
	}
	else
	{
		char *buf = strdup(ptrend);
		memcpy(ptrstart, word, strlen(word));
		ptrstart += strlen(word);
		memcpy(ptrstart, buf, strlen(buf));
		ptrstart += strlen(buf);
		*ptrstart = '\0';
		free(buf);
		buf = NULL;
	}
	return 0;
}
/*
char *string_sep(char *word, int index)
{
	char *tmpstart = word;
	char *tmpend = NULL;
	int i;
	for(i=0; i<index-1; i++)
	{
		tmpstart = strstr(tmpstart, ":");
		if(tmpstart != NULL)
			tmpstart++;
		else
			return tmpstart;
	}	
	tmpend = strstr(tmpstart, ":");
	if(tmpend != NULL)
		*tmpend = '\0';
	return tmpstart;
}
*/

int string_sep(char *word, int index, char *buf)
{
	char *tmpstart = word;
	char *tmpend = NULL;
	int i;
	for(i=1; i<index; i++)
	{
		tmpstart = strstr(tmpstart, ":");
		if(tmpstart != NULL)
			tmpstart++;
		else
			return -1;
	}	
	tmpend = strstr(tmpstart, ":");
	if(tmpend != NULL)
	{
		strncpy(buf, tmpstart, tmpend-tmpstart);
		buf[tmpend-tmpstart] = '\0';
	}	
	else	
		strcpy(buf, tmpstart);

	return 0;
}
#endif
void do_ddm_post(char *url, webs_t stream, int len, char *boundary) //jimmy, https, 8/6/2003
{
	printf("running in do_ddm_post\r\n");
	printf("post_auth_fail = [%d]\r\n",auth_fail);	
//	if( post_auth_fail =1 )
//	{
//		post_auth_fail = 0;
//		printf("post_auth_fail = [%d]\r\n",post_auth_fail);	
//		return ;
//	}	
	DDM_POST_REQUEST(url,stream,len);
}

//=================================================================================================
//                     process ddm ui request                                                     =
//=================================================================================================                     
void DDM_POST_REQUEST(char *url, webs_t stream, int reqLen)
{
	int NO_Equal = 2;
	int i=0;
	char xmlBuff[1000];
	char *ptr_xml=&xmlBuff[0];	
	char *argv_name[100],*argv_value[100];
	xmlBuff[reqLen]='\0';
	
//==================================================================================================
//                     reading XML data to Buffer				         	   =
//==================================================================================================                     
	while(reqLen--)		
	{
		xmlBuff[i] = fgetc(stream);
		i++;
	}	
//--------------------------------------------------------------------
	if( auth_fail == 1 )
	{
//		post_auth_fail = 0;
		printf("post_auth_fail = [%d]\r\n",auth_fail);	
		return ;
	}	
//--------------------------------------------------------------------
	
//====================================================================================================
//			parsing XML data                                                             =
//====================================================================================================			
	while (*ptr_xml != '\0')
	{	
start_point:
		while( *ptr_xml != '<')
			ptr_xml++;
		*ptr_xml = '\0';
		ptr_xml++;		
		if(*ptr_xml == '\0')
			break;
		if(*ptr_xml == '/')
		{
			while(*ptr_xml != '>')
				ptr_xml++;
			if(*(ptr_xml+1) == '\0')
				break;	
			goto start_point;
		}	
		argv_name[NO_Equal] = ptr_xml;
		
getValue:
		while(*ptr_xml != '>')
			ptr_xml++;
		*ptr_xml = '\0';
		ptr_xml++;
		if(*ptr_xml == '\0')
			break;
		argv_value[NO_Equal] = ptr_xml;

		while(*ptr_xml != '<')
			ptr_xml++;
		*ptr_xml = '\0';
		ptr_xml++;
		if(*ptr_xml == '\0')
			break;
		if(*ptr_xml == '/')
		{
			NO_Equal++;
			while(*ptr_xml != '>')
				ptr_xml++;
			if(*(ptr_xml+1) == '\0')
				break;
		}	
		else
		{
			argv_name[NO_Equal] = ptr_xml;
			goto getValue;
		}	
	}
//==============================================================================================================
//			To select function from ddm ui                                                         =
//==============================================================================================================
			if(!strcmp(url,"verizon/set_ppp_user_info.xml"))
			{	
				cprintf("set ppp_username\r\n");
				save_ppp_info(argv_value);
			}	
			else if(!strcmp(url,"verizon/set_cpe_ip_info.xml"))
			{
				cprintf("set cpe_configuration\r\n");
				save_cpe_conf(argv_value);
			}	
			else if(!strcmp(url,"verizon/set_admin_info.xml"))
			{
				cprintf("set admin passwd\r\n");
				save_admin_pass(argv_value);
			}	
			else if(!strcmp(url,"verizon/set_drop_conn.xml"))
			{
				char word[140], *next;
				int error_flag = 0 ;
				char ddm_encapmode[20];
				int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
				memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
				foreach(word, nvram_safe_get("vcc_config"), next)
				{
					if(whichconn-- == 0)
						break;
				}		
				string_sep(word, 3, ddm_encapmode);
				/*
				if(!strcmp(nvram_safe_get("wan_proto"),"pppoe") && !strcmp(nvram_safe_get("ppp_username"), argv_value[2]) )*/
				if(!strcmp(ddm_encapmode, "pppoe"))
				{	
					char pppoe_username[64];
					memset(pppoe_username, 0, sizeof(pppoe_username));
					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("pppoe_config"), next)
					{
						if(whichconn-- == 0)
							break;
					}
					string_sep(word, 1, pppoe_username);
					if(!strcmp(pppoe_username, argv_value[2]))
					{	
						cprintf("set drope pppoe connection\r\n");
						nvram_set("action_service","stop_pppoe");
						//junzhao 2004.4.19
						while(1)
						{
							char tmpbuf[3];
							memset(tmpbuf, 0, sizeof(tmpbuf));
							if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
								break;
							if(!strcmp(tmpbuf, "0"))
								break;
							sleep(1);
						}
						
						kill(1,SIGUSR1);
					}
					else
					{
						cprintf("not exec drop pppoe connection\r\n");
						{
							nvram_set("DDM_Error_No","060101");
							nvram_set("DDM_Error_Desc","PPPoE User Name is wrong");
						}	
					}
				}
				else
				{	
					nvram_set("DDM_Error_No","060105");
					nvram_set("DDM_Error_Desc","WAN connection now is not PPPoE");
					error_flag = 1;
				}
			}	
			else if(!strcmp(url,"verizon/set_make_conn.xml"))
			{	
				char word[140], *next;
				int error_flag = 0 ;
				char ddm_encapmode[20];
				int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
				memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
				foreach(word, nvram_safe_get("vcc_config"), next)
				{
					if(whichconn-- == 0)
						break;
				}		
				string_sep(word, 3, ddm_encapmode);
				/*
				if(!strcmp(nvram_safe_get("wan_proto"),"pppoe") && !strcmp(nvram_safe_get("ppp_username"), argv_value[2]) )*/
				if(!strcmp(ddm_encapmode, "pppoe"))
				{	
					char pppoe_username[64];
					char pppoe_password[64];
					memset(pppoe_username, 0, sizeof(pppoe_username));
					memset(pppoe_password, 0, sizeof(pppoe_password));
					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("pppoe_config"), next)
					{
						if(whichconn-- == 0)
							break;
					}
					string_sep(word, 1, pppoe_username);
					string_sep(word, 2, pppoe_password);
					if(!strcmp(pppoe_username, argv_value[2]))
					{		
						cprintf("set make pppoe connection\r\n");
						nvram_set("action_service","start_pppoe");
						//junzhao 2004.4.19
						while(1)
						{
							char tmpbuf[3];
							memset(tmpbuf, 0, sizeof(tmpbuf));
							if(!file_to_buf("/var/run/being_busy", tmpbuf, sizeof(tmpbuf)))	
								break;
							if(!strcmp(tmpbuf, "0"))
								break;
							sleep(1);
						}
						kill(1,SIGUSR1);
					}
					else if( (!strcmp(pppoe_username,"" )) && !error_flag )
					{
						nvram_set("DDM_Error_No","070102");
						nvram_set("DDM_Error_Desc","PPPoE User Name is empty");
						error_flag = 1;
					}	
					else if( (!strcmp(pppoe_password,"" )) && !error_flag )
					{
						nvram_set("DDM_Error_No","070103");
						nvram_set("DDM_Error_Desc","PPPoE Password is empty");
						error_flag = 1;
					}		
					else
					{
						nvram_set("DDM_Error_No","070101");
						nvram_set("DDM_Error_Desc","PPPoE User Name is wrong");
						error_flag = 1;
					}	
				}
				else
				{	
					nvram_set("DDM_Error_No","070105");
					nvram_set("DDM_Error_Desc","Please change WAN connection type to PPPoE");
					error_flag = 1;

					/*
					int error_flag = 0 ;
					if( strcmp(nvram_safe_get("ppp_username"), argv_value[2]) )
					{
						nvram_set("DDM_Error_No","070101");
						nvram_set("DDM_Error_Desc","PPPoE User Name is wrong");
						error_flag = 1;
					}								
					else if( (!strcmp(nvram_safe_get("ppp_username"),"" )) && !error_flag )
					{
						nvram_set("DDM_Error_No","070102");
						nvram_set("DDM_Error_Desc","PPPoE User Name is empty");
						error_flag = 1;
					}								
					else if( (!strcmp(nvram_safe_get("ppp_passwd"),"" )) && !error_flag )
					{
						nvram_set("DDM_Error_No","070103");
						nvram_set("DDM_Error_Desc","PPPoE Password is empty");
						error_flag = 1;
					}								
					else if( strcmp(nvram_safe_get("wan_proto"),"pppoe" ) && !error_flag )
					{
						nvram_set("DDM_Error_No","070105");
						nvram_set("DDM_Error_Desc","Please change WAN connection type to PPPoE");
						error_flag = 1;
					}
					*/					
				}	
			}	
			else
			{	
				cprintf("nothing to setting ..........\r\n");
			}	
	
}
//=============================================
//  process set_ppp_user_info.xml 	      	
//						
//	XML TAG			VALUE
//
//	ppp_userid 		argv_value[2]
//	ppp_passwd		argv_value[3]
//=============================================	
void save_ppp_info(char * argv_value[])
{
	int ok_save = 1;  // 0: have error , 1: ok save data
	printf("save ppp username&passwd\r\n");
	if( strlen( argv_value[2] ) < 65 )
		ok_save = 1;
	else
	{
		nvram_set("DDM_Error_No","050101");	
		nvram_set("DDM_Error_Desc","PPPoE User Name length should be less than 64 characters");	
		ok_save = 0;
	}	
	if( (strlen( argv_value[3] ) < 65) && ok_save )
		ok_save = 1;
	else
	{
		if( ok_save )
		{	
			nvram_set("DDM_Error_No","050101");	
			nvram_set("DDM_Error_Desc","PPPoE User Password length should be less than 64 characters");	
			ok_save = 0;
		}	
	}
	if( ok_save )
	{
		int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		int which = atoi(connection);
		char word[140], *next;
		foreach(word, nvram_safe_get("pppoe_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}
		string_rep(word, 1, argv_value[2]);
		string_rep(word, 2, argv_value[3]);
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		sprintf(connection, "%d", whichconn);
		nvram_modify_status("pppoe_config", word);
 		nvram_commit();
		sprintf(connection, "%d", which);
		
		//nvram_set("ppp_username",argv_value[2]);
		//nvram_set("ppp_passwd",argv_value[3]);	
	}	
}	
//=============================================	
// process set_cpe_ip_info.xml //
//
//	XML TAG			VALUE
//
//	lan_ip_addr		argv_value[2]
//	lan_subnet_mask		argv_value[3]
//	lan_ip_assgn_start_addr	argv_value[4]
//	lan_ip_assgn_stop_addr	argv_value[5]
//	wan_ip_type		argv_value[6]
//	wan_ip_addr		argv_value[7]
//	wan_subnet_mask		argv_value[8]
//	wan_gateway_ip		argv_value[9]
//	wan_dns1		argv_value[10]
//	wan_dns2		argv_value[11]
//	wan_dns3		argv_value[12]
//	wan_dns4		argv_value[13]
//==============================================	
void save_cpe_conf(char * argv_value[])
{
	char *dhcp_start = "";
	char *dhcp_end = "";	
	int flag = 0;
	int NO_SAVE = 0; // 1: not save(incorrect input) 0: save(correct input)
	int field[10];
	
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
//=======================================================================================================

	if ( strcmp(argv_value[2],"") )
	{	
		if( field[0]=(!check_ip_format(argv_value[2],1)) )         // check lan ip address
		{	
			NO_SAVE = 1;
			nvram_set("DDM_Error_No","030101");	
			nvram_set("DDM_Error_Desc","the value of LAN IP Address should be in the range [1~223].[0~255].[0~255].[1~254]");	
		}	
		else
			nvram_set("lan_ipaddr",argv_value[2]);	
	}

//========================================================================================================		
	if ( strcmp(argv_value[3],"") )
	{	
		if( field[1]=(!check_ip_format(argv_value[3],2)) )    // check lan subnet mask
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030201");	
				nvram_set("DDM_Error_Desc","the value of LAN Subnet Mask  should be in the range [255].[255].[255].[0|128|192|224|240|248|252]");		
			}	
		}
		else	
 			nvram_set("lan_netmask",argv_value[3]);
	}

//=========================================================================================================	
	if ( strcmp(argv_value[4],"") )
	{	
		if( field[2]=(!check_ip_format(argv_value[4],4)) )     // check lan dhcp start address
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030301");	
				nvram_set("DDM_Error_Desc","the value of start address is in the wrong subnet");	
			}	
		}	
		else
		{	
			int count = 0;
			dhcp_start = argv_value[4];
			while ( count < 3 )
			{
			        if(*dhcp_start == '.')
					count++;
				dhcp_start++;
				if(*dhcp_start == '\0')
					break;
			}
			flag = 1;
		}
	}	
	if ( strcmp(argv_value[5],"") )
	{	
		if( field[3]=(!check_ip_format(argv_value[5],4)) )   // check lan dhcp end address
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030401");	
				nvram_set("DDM_Error_Desc","the value of stop address is in the wrong subnet");	
			}	
		}	
		else
		{	
			int count = 0;
			dhcp_end = argv_value[5];
			while ( count < 3 )
			{
			        if(*dhcp_end == '.')
					count++;
				dhcp_end++;
				if(*dhcp_end == '\0')
					break;
			}	
			flag = 1;
		}
	}	
	if(flag)
	{
			int start_addr;
			int end_addr;
			int dhcp_num;	

		if ( strcmp(dhcp_start,"") )
		{	
			start_addr = atoi(dhcp_start);
		}	
		else
		{
			start_addr = atoi( nvram_safe_get("dhcp_start") );
		}		
			
		if ( strcmp(dhcp_end,"") )
		{
			end_addr = atoi(dhcp_end);
		}
		else
		{
			end_addr = ( atoi(nvram_safe_get("dhcp_start")) + atoi( nvram_safe_get("dhcp_num")) - 1);
		}		
		if( end_addr > start_addr )
		{
			char tmp[15]="";
			snprintf(tmp,sizeof(tmp),"%d",start_addr);
			nvram_set("dhcp_start",tmp);
			dhcp_num = (end_addr - start_addr + 1);	
			tmp[15]="";
			snprintf(tmp,sizeof(tmp),"%d",dhcp_num);
			nvram_set("dhcp_num",tmp);			
		}	
		
	}
//=================================================================================================
//=================================================================================================	
		
	if ( strcmp(argv_value[6] ,"") )
	{	
//translate wan type input chars --> lowercase		
		char *ptr=argv_value[6];
		char word[140], *next;
		
		while(*ptr != '\0')
		{
			*ptr = tolower(*ptr);
			ptr++;
		}	
//----------------------------------------------------------------------------------------	
//	if((!strcmp(argv_value[6],"pppoe") || !strcmp(argv_value[6],"dhcp") || !strcmp(argv_value[6],"static") || !strcmp(argv_value[6],"pptp") || !strcmp(argv_value[6],"heartbeat")))          // check wan proto 
		/*if(  !strcmp(argv_value[6],"pppoe") || !strcmp(argv_value[6],"dhcp") || !strcmp(argv_value[6],"static")  )          // check wan proto 
		{
			nvram_set("wan_proto",argv_value[6]);
			field[4]=1;
		}*/
		
		foreach(word, nvram_safe_get("vcc_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}

		if(!strcmp(argv_value[6],"pppoe") || !strcmp(argv_value[6], "pppoa"))          // check wan proto 
		{
			//junzhao 2004.4.16
			if(!string_rep(word, 3, argv_value[6])
			   && !string_rep(word, 8, "1")
			   && !string_rep(word, 9, "1"))
			{
				int which = atoi(connection);
				whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
				sprintf(connection, "%d", whichconn);
				nvram_modify_status("vcc_config", word);
 				nvram_commit();
				sprintf(connection, "%d", which);
			}
			field[4]=1;
		}
		else if(!strcmp(argv_value[6],"dhcp") || !strcmp(argv_value[6], "static"))         
		{
			//junzhao 2004.4.16
			if(!string_rep(word, 3, "1483bridged")
			   && !string_rep(word, 8, "1")
			   && !string_rep(word, 9, "1"))
			{
				int which = atoi(connection);
				whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
				sprintf(connection, "%d", whichconn);
				nvram_modify_status("vcc_config", word);
 				nvram_commit();
				sprintf(connection, "%d", which);
			}
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			foreach(word, nvram_safe_get("bridged_config"), next)
			{
				if(whichconn-- == 0)
					break;
			}
			if(!strcmp(argv_value[6], "dhcp"))
				string_rep(word, 1, "1");
			else
				string_rep(word, 1, "0");
			{
			int which = atoi(connection);
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			sprintf(connection, "%d", whichconn);
			nvram_modify_status("bridged_config", word);
 			nvram_commit();
			sprintf(connection, "%d", which);
			field[4]=1;
			}
		}
		else 
		{
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;	
				nvram_set("DDM_Error_No","030501");	
				nvram_set("DDM_Error_Desc","the value of WAN IP TYPE is [ DHCP | Static | PPPoE | PPPoA ]");	
			}	
		}
	}	
//========================================================================================================	
//	cprintf("wan ip%d\r\n",NO_SAVE);
//========================================================================================================
	if ( strcmp(argv_value[7],"") )
	{	
	    if(!strcmp(argv_value[6], "static"))
	    {
		if( field[5]=(!check_ip_format(argv_value[7],1)) )      // check wan ip address
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030601");	
				nvram_set("DDM_Error_Desc","the value of WAN IP Address is [1~223].[0~255].[0~255].[1~254]");	
			}	
		}	
		else
		{
			char word[140], *next;
			int which;
			foreach(word, nvram_safe_get("bridged_config"), next)
			{
				if(whichconn-- == 0)
					break;
			}
			string_rep(word, 2, argv_value[7]);
			which = atoi(connection);
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			sprintf(connection, "%d", whichconn);
			nvram_modify_status("bridged_config", word);
 			nvram_commit();
			sprintf(connection, "%d", which);
			memset(global_wan_ip, 0, sizeof(global_wan_ip));				strncpy(global_wan_ip, argv_value[7], sizeof(global_wan_ip));	
			//nvram_set("wan_ipaddr",argv_value[7]);
		}
	    }
	}
	
//========================================================================================================
//	cprintf("wan ip subnet mask%d\r\n",NO_SAVE);
//========================================================================================================
		
	if ( strcmp(argv_value[8],"") )
	{	
	    if(!strcmp(argv_value[6], "static"))
	    {
		if( field[6]=(!check_ip_format(argv_value[8],3)) )  // check wan ip subnet mask
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030701");	
				nvram_set("DDM_Error_Desc","the value of WAN IP Subnet Mask is [0|128|192|224|240|248|252|255]*4");	
			}	
		}	
		else
		{
			char word[140], *next;
			int which;
			foreach(word, nvram_safe_get("bridged_config"), next)
			{
				if(whichconn-- == 0)
					break;
			}
			string_rep(word, 3, argv_value[8]);
			which = atoi(connection);
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			sprintf(connection, "%d", whichconn);
			nvram_modify_status("bridged_config", word);
 			nvram_commit();
			sprintf(connection, "%d", which);	
			memset(global_wan_netmask, 0, sizeof(global_wan_netmask));				
			strncpy(global_wan_netmask, argv_value[8], sizeof(global_wan_netmask));			
			//nvram_set("wan_netmask",argv_value[8]);
		}
	    }
	}		
	
//============================================================================================================
//	cprintf("wan ip gateway%d\r\n",NO_SAVE);
//============================================================================================================
		
	if ( strcmp(argv_value[9],"") )
	{	
	    if(!strcmp(argv_value[6], "static"))
	    {
		if( field[7]=(!check_ip_format(argv_value[9],4,2)) )  // check wan gateway ip
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030801");	
				nvram_set("DDM_Error_Desc","the value of  WAN Gateway IP is in the wrong subnet");	
			}	
		}	
		else
		{
			char word[140], *next;
			int which;
			foreach(word, nvram_safe_get("bridged_config"), next)
			{
				if(whichconn-- == 0)
					break;
			}
			string_rep(word, 4, argv_value[9]);
			which = atoi(connection);
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			sprintf(connection, "%d", whichconn);
			nvram_modify_status("bridged_config", word);
 			nvram_commit();
			sprintf(connection, "%d", which);
			//nvram_set("wan_gateway",argv_value[9]);
		}
	    }
	}		

//============================================================================================================
	{
	char dns_ok[20]="";
	char *dns_ok_ptr = &dns_ok[0];
//============================================================================================================	
	char dns[3][16] = {"","",""} ;
	char *dns_p = &dns[0][0];	
	char *dns_ptr2 = &dns[0][0];	
	get_wan_dns(dns_p);
//----------------------------------------------------------------

	if ( strcmp(argv_value[10],"") )
	{	
		if( !check_ip_format(argv_value[10],1) )   // check wan dns1
		{	
			if( !NO_SAVE )   
			{	
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030901");	
				nvram_set("DDM_Error_Desc","the value of WAN DNS1 should be in the range [1~223].[0~255].[0~255].[1~254]");	
			}	
		}	
		else 
		{				
			strcat(dns_ok_ptr,argv_value[10]);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,dns_ptr2+16);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,dns_ptr2+32);
			nvram_set("wan_dns",dns_ok_ptr);
			//dns_to_resolv(0);
		}	
	}	
	memset(dns_ok_ptr,'\0',sizeof(dns_ok));
	dns_p = &dns[0][0];	
	dns_ptr2 = &dns[0][0];	
	get_wan_dns(dns_p);
	if ( strcmp(argv_value[11],"") )
	{	
		if( !check_ip_format(argv_value[11],1) )   // check wan dns2
		{	
			if( !NO_SAVE )   
			{
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030A01");	
				nvram_set("DDM_Error_Desc","the value of WAN DNS2 should be in the range [1~223].[0~255].[0~255].[1~254]");	
			}	
		}	
		else
		{
			strcat(dns_ok_ptr,dns_ptr2);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,argv_value[11]);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,dns_ptr2+32);
			nvram_set("wan_dns",dns_ok_ptr);
			//dns_to_resolv(0);
		}	

	}
	memset(dns_ok_ptr,'\0',sizeof(dns_ok));
	dns_p = &dns[0][0];	
	dns_ptr2 = &dns[0][0];	
	get_wan_dns(dns_p);
	if ( strcmp(argv_value[12],"") )
	{	
		if( field[8]=(!check_ip_format(argv_value[12],1)) )   // check wan dns3
		{	
			if( !NO_SAVE )   
			{
				NO_SAVE = 1;
				nvram_set("DDM_Error_No","030B01");	
				nvram_set("DDM_Error_Desc","the value of WAN DNS3 should be in the range [1~223].[0~255].[0~255].[1~254]");	
			}	
		}	
		else
		{	
			strcat(dns_ok_ptr,dns_ptr2);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,dns_ptr2+16);
			strcat(dns_ok_ptr," ");
			strcat(dns_ok_ptr,argv_value[12]);
			nvram_set("wan_dns",dns_ok_ptr);
			//dns_to_resolv(0);
		}	
	}	

//================================================================================================================
	if( strcmp(argv_value[13],"") && !NO_SAVE )    // check wan dns4 
	{	
		NO_SAVE = 1;
		nvram_set("DDM_Error_No","030C01");	
		nvram_set("DDM_Error_Desc","WAN DNS4 is not supported");	
	}	
//==================================================================================================================	
	sys_restart();
	}
}
//=============================================
// process set_admin_info.xml  
// 	
// 	XML TAG			VALUE
//
// 	adminid			argv_value[2]
// 	old_adminpwd		argv_value[3]
// 	new_adminpwd		argv_value[4]
//============================================= 	
void save_admin_pass(char *argv_value[])
{
	cprintf("set admin passwd ok user:[%d],pass:[%d]\r\n",strlen(argv_value[2]),strlen(argv_value[4]));
	if(!strcmp(nvram_safe_get("http_passwd"),argv_value[3]))
	{	
		int ok_save = 1;  // 0: have error , 1: ok save data
		if( strlen( argv_value[2] ) < 65 )
			ok_save = 1;	
		else
		{
			nvram_set("DDM_Error_No","0A0201");	
			nvram_set("DDM_Error_Desc","New User Name length should be less than 64 characters");	
			ok_save = 0;
		}	
		if( (strlen( argv_value[4] ) < 65) && ok_save )
			ok_save = 1;	
		else
		{
			if( ok_save )
			{	
				nvram_set("DDM_Error_No","0A0301");	
				nvram_set("DDM_Error_Desc","New Password length should be less than 64 characters");	
				ok_save = 0;
			}	
		}
		if( ok_save )
		{
			nvram_set("http_username",argv_value[2]);	
			nvram_set("http_passwd",argv_value[4]);				
			nvram_set("DDM_pass_flag","1");				
		}	
		cprintf("set admin passwd ok user:[%d],pass:[%d]\r\n",strlen(argv_value[2]),strlen(argv_value[4]));
	}
	else
	{	
		cprintf("not set admin passwd\r\n");
		nvram_set("DDM_Error_No","0A0101");
		nvram_set("DDM_Error_Desc","Old Password is wrong");
	}	
}

int check_ip_format(char *argv_value,int index,int lanOwan)
{
//========================= read src ip address ===============================
	int count = 0;
	char *ip_addr,*a1[4],tmp[16]="";
	int force_bk = 0;
	if( strlen(argv_value) > 15 )
	{	
		return 0;
	}	
	while( count < (strlen(argv_value)) )
	{
		int alpha_flag = 0;
		tmp[count] = *(argv_value+count);
		if ( (*(argv_value+count) <'0') || (*(argv_value+count) >'9'))
		{
			if ( *(argv_value+count) == '.')
				alpha_flag = 0;
			else
				alpha_flag = 1;
		}	
		if( alpha_flag == 1 )
		{
			return 0;
		}	
		if ( count < 15 )
			count++;
		else 
			break;
		
		force_bk++;
		
		if( force_bk >= 18 )
			break;
	}		
	a1[0] = &tmp[0] ;
	ip_addr = &tmp[0] ;
//========================= read src ip address ===============================
	count = 0 ;
	force_bk = 0;
	while( force_bk < (strlen(argv_value)) )
	{
		if(*ip_addr == '.')
		{				
			if ( count==0 )
				a1[1] = ip_addr+1 ;
			if ( count==1 )
				a1[2] = ip_addr+1 ;
			if ( count==2 )
			{
				if ( force_bk == (strlen(argv_value)-1) )			
					return 0;
				else
					a1[3] = ip_addr+1 ;
			}	
			*ip_addr = '\0' ;
			count++;
		}	
		ip_addr++;
		force_bk++;
		if( force_bk >= 16 )
			break;
	}
	if( count != 3 )	
		return 0;
	
		switch ( index )
		{
			case 1:
					cprintf("case1\r\n");
					if ( check_ip_range(1,223,a1[0]) && check_ip_range(0,255,a1[1]) && check_ip_range(0,255,a1[2]) && check_ip_range(1,254,a1[3]))	
					{	
						cprintf("legal ip\r\n");
						return 1;
					}	
					else
					{	
						cprintf("illegal ip \r\n");
						return 0;
					}	
					break;
			case 2:
					{
					int f1 = 1, f2 = 1, f3 = 1, f4 = 1;
					cprintf("case2\r\n");
					f1 = check_ip_range( 255, 255, a1[0] );
					f2 = check_ip_range( 255, 255, a1[1] );
					f3 = check_ip_range( 255, 255, a1[2] );
					if( !strcmp(a1[3],"0") || !strcmp(a1[3],"128") || !strcmp(a1[3],"192") || !strcmp(a1[3],"224") || !strcmp(a1[3],"240") || !strcmp(a1[3],"248") || !strcmp(a1[3],"252" ))
					{
						f4 = 1;
						cprintf("legal Subnet Mask \r\n");
					}	
					else 
					{	
						f4 = 0;
					}
					if(f1 && f2 && f3 && f4)
					{	
						cprintf("legal ip\r\n");
						return 1;
					}	
					else 
					{	
						cprintf("illegal ip\r\n");
						return 0;
					}	
					break;
					}
			case 3:
					{
					int ff1 = 1, ff2 = 1, ff3 = 1, ff4 = 1;
					int ip_sub_mask_0 = atoi(a1[0]);
					if( !strcmp(a1[0],"0") ||!strcmp(a1[0],"128") || !strcmp(a1[0],"192") || !strcmp(a1[0],"224") || !strcmp(a1[0],"240") || !strcmp(a1[0],"248") || !strcmp(a1[0],"252") || !strcmp(a1[0],"255"))
					{	
						ff1 = 1;
						cprintf("Subnet Mask 0 OK\r\n");	
					}	
					else
					{	
						ff1 = 0;
					}
					{	
					int ip_sub_mask_1 = atoi(a1[1]);
					cprintf("case3\r\n");
					if( !strcmp(a1[1],"0") ||!strcmp(a1[1],"128") || !strcmp(a1[1],"192") || !strcmp(a1[1],"224") || !strcmp(a1[1],"240") || !strcmp(a1[1],"248") || !strcmp(a1[1],"252") || !strcmp(a1[1],"255"))
					{	
						ff2 = 1;
						cprintf("Subnet Mask 1 OK\r\n");	
					}	
					else
					{	
						ff2 = 0;
					}
					{	
					int ip_sub_mask_2 = atoi(a1[2]);
					if( !strcmp(a1[2],"0") ||!strcmp(a1[2],"128") || !strcmp(a1[2],"192") || !strcmp(a1[2],"224") || !strcmp(a1[2],"240") || !strcmp(a1[2],"248") || !strcmp(a1[1],"252") || !strcmp(a1[1],"255"))
					{	
						ff3 = 1;
						cprintf("Subnet Mask 2 OK\r\n");	
					}	
					else
					{	
						ff3 = 0;
					}
					{	
					int ip_sub_mask_3 = atoi(a1[3]);
					if( !strcmp(a1[3],"0") ||!strcmp(a1[3],"128") || !strcmp(a1[3],"192") || !strcmp(a1[3],"224") || !strcmp(a1[3],"240") || !strcmp(a1[3],"248") || !strcmp(a1[3],"252") || !strcmp(a1[3],"255"))
					{	
						ff4 = 1;
						cprintf("Subnet Mask 3 OK\r\n");	
					}	
					else
					{	
						ff4 = 0;
					}	
					if( ff1 && ff2 && ff3 && ff4 )
					{	
						cprintf("legal ip\r\n");
						return 1;
					}	
					else
					{	
						cprintf("illegal ip\r\n");
						return 0;
					}	
					break;
					}
					}
					}
					}
			case 4:
					if( !check_ip_range(1,254,a1[3]))
						return 0;
					{
					char addr_tmp[13]="";
					char *tmp ;
					tmp = &addr_tmp[0];
					strcat( tmp, a1[0] );
					strcat( tmp, "." );
					strcat( tmp, a1[1] );
					strcat( tmp, "." );
					strcat( tmp, a1[2] );
//------------------------------------------------------------------------------------------------------
					{
					char temp[20]="";
					char wan_ip_buf[20]="";
					char lan_ip_buf[20]="";
					char netmask_buf[20]="";
					char *ptr,*wan_ip=&wan_ip_buf,*lan_ip=lan_ip_buf,*netmask_ip=&netmask_buf ;
					ptr = &temp[0];
					if(lanOwan == 2)
					{
							//strcat(ptr,nvram_safe_get("wan_ipaddr"));
							//strcat(wan_ip,nvram_safe_get("wan_ipaddr"));
							//strcat(netmask_ip,nvram_safe_get("wan_netmask"));
							strcat(ptr,global_wan_ip);
							strcat(wan_ip,global_wan_ip);
							strcat(netmask_ip,global_wan_netmask);
							wan_ip = strrchr(wan_ip,'.');
							netmask_ip = strrchr(netmask_ip,'.');
						if( (atoi(a1[3])&atoi(netmask_ip+1)) == (atoi(wan_ip+1)&atoi(netmask_ip+1)) )
						{	
							fprintf(stderr,"correct wan subnetmask\r\n");
						}	
                                                else
						{	
							fprintf(stderr,"error wan subnetmask\r\n");
							return 0;
						}	
					}	
					else
					{	
							strcat(ptr,nvram_safe_get("lan_ipaddr"));
							strcat(lan_ip,nvram_safe_get("lan_ipaddr"));
							strcat(netmask_ip,nvram_safe_get("lan_netmask"));
							lan_ip = strrchr(lan_ip,'.');
							netmask_ip = strrchr(netmask_ip,'.');
						if( (atoi(a1[3])&atoi(netmask_ip+1)) == (atoi(lan_ip+1)&atoi(netmask_ip+1)) )
						{	
							fprintf(stderr,"correct lan subnetmask\r\n");
						}	
                                                else
						{	
							fprintf(stderr,"error lan subnetmask\r\n");
							return 0;
						}	
					}	
					temp[19]='\0';
					{
					char *ptr2;
					ptr2 = ptr;
					count = 0;
					while(*ptr2 != '\0')
					{
						if( *ptr2 == '.' )
						{
							count++;
							if( count == 3 )
								*ptr2 = '\0';
						}	
						if( count == 3 )
							break;
						ptr2++;		
					}	

//------------------------------------------------------------------------------------------------------
					if( !strcmp(tmp,ptr) )
					{
						cprintf("good ip address\r\n");
						return 1;
					}	
					else
					{	
						cprintf("bad ip address\r\n");
						return 0;
					}	

					break;
					}
					}
					}
			default:
					cprintf("default\r\n");
					break;	
		}	
}

/* Dump leases */
int ej_ddm_dumpleases(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	FILE *fp;
	struct lease_t lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
	char sigusr1[] = "-XX";
	int ret = 0;
	int count = 0;
	char *ipaddr, mac[20]="", expires_time[50]="";

        /* Write out leases file */
        sprintf(sigusr1, "-%d", SIGUSR1);
        eval("killall", sigusr1, "udhcpd");

	/* Parse leases file */
	if ((fp = fopen("/tmp/udhcpd.leases", "r"))) {
	    while (fread(&lease, sizeof(lease), 1, fp)) {
		strcpy(mac,"");

		for (i = 0; i < 6; i++) {
			sprintf(mac+strlen(mac),"%02X", lease.chaddr[i]);
			if (i != 5) sprintf(mac+strlen(mac),":");
		}
		mac[17] = '\0';
		if(!strcmp(mac,"00:00:00:00:00:00"))
			continue;
		
		addr.s_addr = lease.yiaddr;

		ipaddr = inet_ntoa(addr);

		expires = ntohl(lease.expires);

		strcpy(expires_time,"");
		if (!expires){
			continue;
			strcpy(expires_time,"expired");
		}
		else {
			if (expires > 60*60*24) {
				sprintf(expires_time+strlen(expires_time),"%ld days, ",expires / (60*60*24));
				expires %= 60*60*24;
			}
			if (expires > 60*60) {
				sprintf(expires_time+strlen(expires_time),"%02ld:",expires / (60*60));	// hours
				expires %= 60*60;
			}
			else{
				sprintf(expires_time+strlen(expires_time),"00:");	// no hours
			}
			if (expires > 60) {
				sprintf(expires_time+strlen(expires_time),"%02ld:",expires / 60);	// minutes
				expires %= 60;
			}
			else{
				sprintf(expires_time+strlen(expires_time),"00:");	// no minutes
			}

			sprintf(expires_time+strlen(expires_time),"%02ld:",expires);		// seconds

			expires_time[strlen(expires_time)-1]='\0';
		}
ret += websWrite(wp, "<lan_client_ip_table>\n<lan_client_details>\n<lan_client_ip_addr>%s</lan_client_ip_addr>\n<lan_client_mac_addr>%s</lan_client_mac_addr>\n<lan_client_ip_type>DHCP</lan_client_ip_type>\n<lan_client_ip_lease>Leased</lan_client_ip_lease>\n</lan_client_details>\n</lan_client_ip_table>\n",ipaddr, mac); 
                count++;
	   }
	fclose(fp);
	}
	if (ret == 0)
	{
		ret += websWrite(wp, "<lan_client_ip_table>\n<lan_client_details>\n<lan_client_ip_addr></lan_client_ip_addr>\n<lan_client_mac_addr></lan_client_mac_addr>\n<lan_client_ip_type></lan_client_ip_type>\n<lan_client_ip_lease></lan_client_ip_lease>\n</lan_client_details>\n</lan_client_ip_table>\n"); 

	}	
	return ret;
}
int ej_ddm_check_passwd(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	if(!strcmp(nvram_safe_get("DDM_pass_flag"),"1"))
		websWrite(wp,"Set");
	else
		websWrite(wp,"Not Set");
	return 0;
}	
int ej_ddm_show_ipend(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	int dhcp_start = atoi(nvram_safe_get("dhcp_start"));
	int dhcp_num = atoi(nvram_safe_get("dhcp_num"));
	int dhcp_end ;
	char tmp[5];
	dhcp_end = dhcp_start + dhcp_num ;
	dhcp_end = dhcp_end -1;
	snprintf(tmp,sizeof(tmp),"%d",dhcp_end);
	websWrite(wp,tmp);
	return 0;
}

int ej_ddm_show_hwversion(int eid, webs_t wp, int argc, char_t **argv)
{
	char readbuf[8];
	FILE *fp;
	memset(readbuf, 0, sizeof(readbuf));
	if(!(fp=fopen("/proc/led_mod/hardware_version", "r")))
		strcpy(readbuf, " ");
	else
	{
		if(fread(readbuf, 1, sizeof(readbuf), fp))
		{	
			char *p = strstr(readbuf, "\n");
			if(p != NULL)
				*p = '\0';
		}
		else
			strcpy(readbuf, " ");
		fclose(fp);
	}
	websWrite(wp, "%s\n", readbuf);
	return 0;
}


//junzhao 2004.3.22
int ej_ddm_show_wanipaddr(int eid, webs_t wp, int argc, char_t **argv)
{
	char ddm_encapmode[20];
	char word[100], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char *wanipaddr;
	char readbuf[256];
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}						
	if(!strcmp(ddm_encapmode, "1483bridged"))
	{	
		char ddm_dhcpenable[3];
		memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("bridged_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 1, ddm_dhcpenable);
				break;
			}
		}			
		if(!strcmp(ddm_dhcpenable, "0"))
		{
			char wan_ipaddr[20];
			memset(wan_ipaddr, 0, sizeof(wan_ipaddr));
			string_sep(word, 2, wan_ipaddr);
			return websWrite(wp, "%s", wan_ipaddr);
		}
	}
	else if(!strcmp(ddm_encapmode, "routed"))
	{
		char wan_ipaddr[20];
		memset(wan_ipaddr, 0, sizeof(wan_ipaddr));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("routed_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 1, wan_ipaddr);
		return websWrite(wp, "%s", wan_ipaddr);
	}
#ifdef CLIP_SUPPORT
	else if(!strcmp(ddm_encapmode, "clip"))
	{
		char wan_ipaddr[20];
		memset(wan_ipaddr, 0, sizeof(wan_ipaddr));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("clip_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 1, wan_ipaddr);
		return websWrite(wp, "%s", wan_ipaddr);
	}	
#endif
	
	memset(word, 0, sizeof(word));
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			wanipaddr = word;
			break;
		}
	}
	if(!strstr(wanipaddr, "10.64.64"))		
		return websWrite(wp, "%s", wanipaddr);
	else
		return websWrite(wp, "0.0.0.0");
	return 0;
}

int ej_ddm_show_wannetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[100], *next, *wanipaddr = NULL, *wannetmask = NULL;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char ddm_encapmode[20];
	char readbuf[256];
	
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}						
	if(!strcmp(ddm_encapmode, "1483bridged"))
	{	
		char ddm_dhcpenable[3];
		memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("bridged_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 1, ddm_dhcpenable);
				break;
			}
		}			
		if(!strcmp(ddm_dhcpenable, "0"))
		{
			char wan_netmask[20];
			memset(wan_netmask, 0, sizeof(wan_netmask));
			string_sep(word, 3, wan_netmask);
			return websWrite(wp, "%s", wan_netmask);
		}
	}
	else if(!strcmp(ddm_encapmode, "routed"))
	{
		char wan_netmask[20];
		memset(wan_netmask, 0, sizeof(wan_netmask));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("routed_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 2, wan_netmask);
		return websWrite(wp, "%s", wan_netmask);
	}	
#ifdef CLIP_SUPPORT
	else if(!strcmp(ddm_encapmode, "clip"))
	{
		char wan_netmask[20];
		memset(wan_netmask, 0, sizeof(wan_netmask));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("clip_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 2, wan_netmask);
		return websWrite(wp, "%s", wan_netmask);
	}	
#endif
	memset(word, 0, sizeof(word));
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			wanipaddr = word;
			break;
		}
	}						
	if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
		return websWrite(wp, "0.0.0.0");
		
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_NETMASK, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_NETMASK);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			wannetmask = word;
			break;
		}
	}						
	return websWrite(wp, "%s", wannetmask);
}

int ej_ddm_show_wangateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[100], *next, *wanipaddr = NULL, *wangateway = NULL;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char ddm_encapmode[20];
	char readbuf[256];
	
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}						
	if(!strcmp(ddm_encapmode, "1483bridged"))
	{	
		char ddm_dhcpenable[3];
		memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("bridged_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 1, ddm_dhcpenable);
				break;
			}
		}			
		if(!strcmp(ddm_dhcpenable, "0"))
		{
			char wan_gateway[20];
			memset(wan_gateway, 0, sizeof(wan_gateway));
			string_sep(word, 4, wan_gateway);
			return websWrite(wp, "%s", wan_gateway);
		}
	}
	else if(!strcmp(ddm_encapmode, "routed"))
	{
		char wan_gateway[20];
		memset(wan_gateway, 0, sizeof(wan_gateway));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("routed_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 3, wan_gateway);
		return websWrite(wp, "%s", wan_gateway);
	}	
#ifdef CLIP_SUPPORT
	else if(!strcmp(ddm_encapmode, "clip"))
	{
		char wan_gateway[20];
		memset(wan_gateway, 0, sizeof(wan_gateway));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("clip_config"), next)
		{
			if(whichconn-- == 0)
				break;
		}		
		string_sep(word, 3, wan_gateway);
		return websWrite(wp, "%s", wan_gateway);
	}	
#endif
	memset(word, 0, sizeof(word));
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IPADDR);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			wanipaddr = word;
			break;
		}
	}						
	if(strstr(wanipaddr, "10.64.64") || !strcmp(wanipaddr, "0.0.0.0"))
		return websWrite(wp, "0.0.0.0");
		
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_GATEWAY, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_GATEWAY);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			wangateway = word;
			break;
		}
	}						
	return websWrite(wp, "%s", wangateway);
}

//junzhao 2004.4.19
int ej_ddm_show_pppoe_acname(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[64], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char readbuf[512];
	
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_PPPOE_ACNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_PPPOE_ACNAME);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			break;
		}
	}						
	
	if(strcmp(word, "None"))	
		return websWrite(wp, "%s", word);
}

//junzhao 2004.6.14
int ej_ddm_show_pppoe_srvname(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[64], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char readbuf[512];
	
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_PPPOE_SRVNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_PPPOE_SRVNAME);
	        return 0;
	}
	foreach(word, readbuf, next)
	{
		if(whichconn-- == 0)
		{
			break;
		}
	}						
	
	if(strcmp(word, "None"))	
		return websWrite(wp, "%s", word);
}

//junzhao 2004.4.16
/*
int ej_ddm_lan_hwaddr(int eid, webs_t wp, int argc, char_t **argv)
{
	char lan_hwaddr[20], *tmp = lan_hwaddr;
	int i;
	memset(lan_hwaddr, 0, sizeof(lan_hwaddr));
	strncpy(lan_hwaddr, nvram_safe_get("lan_hwaddr"), sizeof(lan_hwaddr));
	for(i=0; i<5; i++)
	{
		tmp = strstr(tmp, ":");
		*tmp = '-';
		tmp++;
	}
	return websWrite(wp, "%s", lan_hwaddr);
}
*/
/*
int ej_ddm_lan_hwaddr(int eid, webs_t wp, int argc, char_t **argv)
{
	char lan_hwaddr[20];
	memset(lan_hwaddr, 0, sizeof(lan_hwaddr));
	strncpy(lan_hwaddr, nvram_safe_get("lan_hwaddr"), sizeof(lan_hwaddr));
	hwaddr_convert(lan_hwaddr);
	return websWrite(wp, "%s", lan_hwaddr);
}
*/
//junzhao 2004.4.16
int ej_ddm_get_ppppassword(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[140], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
//	char ddm_encapmode[20];
	char pppoe_password[64];
	memset(pppoe_password, 0, sizeof(pppoe_password));
//	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	foreach(word, nvram_safe_get("pppoe_config"), next)
	{
		if(whichconn-- == 0)
		{
			break;
		}
	}
	string_sep(word, 2, pppoe_password);
	return websWrite(wp, "%s", pppoe_password);

/*	
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}
	if(!strcmp(ddm_encapmode, "pppoe"))
	{
		char pppoe_password[64];
		memset(pppoe_password, 0, sizeof(pppoe_password));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoe_config"), next)
		{
			if(whichconn-- == 0)
			{
				break;
			}
		}
		string_sep(word, 2, pppoe_password);
		return websWrite(wp, "%s", pppoe_password);
	}	
	else if(!strcmp(ddm_encapmode, "pppoa"))
	{
		char pppoa_password[64];
		memset(pppoa_password, 0, sizeof(pppoa_password));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoa_config"), next)
		{
			if(whichconn-- == 0)
			{
				break;
			}
		}
		string_sep(word, 1, pppoa_password);
		return websWrite(wp, "%s", pppoa_password);
	}	
*/
}

int ej_ddm_get_pppuser(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[140], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
//	char ddm_encapmode[20];
	//2004.4.16
	char pppoe_username[64];
	memset(pppoe_username, 0, sizeof(pppoe_username));
	foreach(word, nvram_safe_get("pppoe_config"), next)
	{
		if(whichconn-- == 0)
		{
			break;
		}
	}
	string_sep(word, 1, pppoe_username);
	return websWrite(wp, "%s", pppoe_username);
	
	/*
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}
	if(!strcmp(ddm_encapmode, "pppoe"))
	{
		char pppoe_username[64];
		memset(pppoe_username, 0, sizeof(pppoe_username));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoe_config"), next)
		{
			if(whichconn-- == 0)
			{
				break;
			}
		}
		string_sep(word, 1, pppoe_username);
		return websWrite(wp, "%s", pppoe_username);
	}	
	else if(!strcmp(ddm_encapmode, "pppoa"))
	{
		char pppoa_username[64];
		memset(pppoa_username, 0, sizeof(pppoa_username));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoa_config"), next)
		{
			if(whichconn-- == 0)
			{
				break;
			}
		}
		string_sep(word, 1, pppoa_username);
		return websWrite(wp, "%s", pppoa_username);
	}
	*/
}

int ej_ddm_get_pppstatus(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[140], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	char ddm_encapmode[20];
	
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}
	if(!strcmp(ddm_encapmode, "pppoe"))
	{
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		if(check_ppp_link(whichconn))
			return websWrite(wp, "%s", "Connected");
		else
			return websWrite(wp, "%s", "Disconnected");
	}
}

int ej_ddm_show_wandns(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	char word[128], *next, *wandns = NULL;
	char wandns1[32], wandns2[32];
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	struct dns_lists *dns_list = NULL;
	char ddm_encapmode[20];
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
                websError(wp, 400, "Insufficient args\n");
                return -1;
        }
	
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}
	
	whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	dns_list = get_dns_list(whichconn);
	if(dns_list != NULL)
	{
		//junzhao 2004.3.31
		int ret;
		if(strlen(dns_list->dns_server[count]) == 0)
			//return websWrite(wp, "%s", "0.0.0.0");
			ret = websWrite(wp, "%s", "0.0.0.0");
		else
			ret = websWrite(wp, "%s", dns_list->dns_server[count]);
		//junzhao 2004.10.11
		free(dns_list);
		return ret;
		
		/*
		if(count < dns_list->num_servers)
			return websWrite(wp, "%s", dns_list->dns_server[count]);
		else
		{
			//if(count == dns_list->num_servers)
			if((!count) && (!dns_list->num_servers))
			{
				if(!strcmp(ddm_encapmode, "1483bridged")) 
				{
					char ddm_dhcpenable[3];
					memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("bridged_config"), next)
					{
						if(whichconn-- == 0)
						{
							string_sep(word, 1, ddm_dhcpenable);
							break;
						}
					}			
					if(!strcmp(ddm_dhcpenable, "0"))
					{
						char wan_dns1[20];
						memset(wan_dns1, 0 ,sizeof(wan_dns1));
						string_sep(word, 5, wandns1);
						if(strcmp(wandns1, "0.0.0.0")) 
							return websWrite(wp, "%s", wandns1);
					}
				}
				else if(!strcmp(ddm_encapmode, "routed"))
				{		
					char wan_dns1[20];
					memset(wan_dns1, 0, sizeof(wan_dns1));
					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("routed_config"), next)
					{
						if(whichconn-- == 0)
							break;
					}
					string_sep(word, 4, wan_dns1);		
					if(strcmp(wan_dns1, "0.0.0.0"))			
						return websWrite(wp, "%s", wan_dns1);
				}	
			}
			//else if(count - dns_list->num_servers == 1)
			else if((count == 1) && (!dns_list->num_servers))
			{
				if(!strcmp(ddm_encapmode, "1483bridged")) 
				{
					char ddm_dhcpenable[3];
					memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("bridged_config"), next)
					{
						if(whichconn-- == 0)
						{
							string_sep(word, 1, ddm_dhcpenable);
							break;
						}
					}			
					if(!strcmp(ddm_dhcpenable, "0"))
					{
						char wan_dns2[20];
						memset(wan_dns2, 0 ,sizeof(wan_dns2));
						string_sep(word, 6, wandns2);
						if(strcmp(wan_dns2, "0.0.0.0")) 
							return websWrite(wp, "%s", wan_dns2);
					}
				}
				else if(!strcmp(ddm_encapmode, "routed"))
				{			
					char wan_dns2[20];
					memset(wan_dns2, 0 ,sizeof(wan_dns2));

					whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
					foreach(word, nvram_safe_get("routed_config"), next)
					{
						if(whichconn-- == 0)
							break;
					}		
					string_sep(word, 5, wandns2);
					if(strcmp(wan_dns2, "0.0.0.0"))			
						return websWrite(wp, "%s", wan_dns2);
				}
			}
		}
		*/
	}
	return websWrite(wp, "%s", "0.0.0.0");		
}

int ej_ddm_show_wanproto(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	//junzhao 2004.3.23
	char ddm_encapmode[20];
	char word[64], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			//sscanf(word, "%5[^:]:%8[^:]:%20[^:]:%5[^:]:%8[^:]:%8[^:]:%8[^:]:%3[^:]:%3[^\n]", ddm_vpi, ddm_vci, ddm_encapmode, ddm_multiplex, ddm_qostype, ddm_pcr, ddm_scr, ddm_autodetect, ddm_applyonboot);
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}						

	if((!strcmp(ddm_encapmode, "pppoe")) || (!strcmp(ddm_encapmode, "pppoa")) || (!strcmp(ddm_encapmode, "bridgedonly")))
	{
		if(!strcmp(ddm_encapmode, "pppoe"))
			return websWrite(wp, "PPPoE");
		else if(!strcmp(ddm_encapmode, "pppoa"))
			return websWrite(wp, "PPPoA");
		else if(!strcmp(ddm_encapmode, "bridgedonly"))
			return websWrite(wp, "Bridged Mode Only");
	}
	else if(!strcmp(ddm_encapmode, "1483bridged"))
	{	
		char ddm_dhcpenable[3];
		memset(ddm_dhcpenable, 0, sizeof(ddm_dhcpenable));
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("bridged_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 1, ddm_dhcpenable);
				break;
			}
		}			
		if(!strcmp(ddm_dhcpenable, "1"))
			return websWrite(wp, "DHCP");
		else
			return websWrite(wp, "Static");
	}
	else if(!strcmp(ddm_encapmode, "routed"))
		return websWrite(wp, "STATIC");
#ifdef CLIP_SUPPORT
	else if(!strcmp(ddm_encapmode, "clip"))
		return websWrite(wp, "STATIC");
#endif
	/*
	if( !strcmp(nvram_safe_get("wan_proto"),"dhcp")) 
		websWrite(wp,"DHCP");
	else if( !strcmp(nvram_safe_get("wan_proto"),"static")) 
		websWrite(wp,"STATIC");
	else if( !strcmp(nvram_safe_get("wan_proto"),"pppoe")) 
		websWrite(wp,"PPPOE");
	else if( !strcmp(nvram_safe_get("wan_proto"),"heartbeat")) 
		websWrite(wp,"HeartBeat Signal");
	else if( !strcmp(nvram_safe_get("wan_proto"),"pptp")) 
		websWrite(wp,"PPTP");
	else if( !strcmp(nvram_safe_get("wan_proto"),"l2tp")) 
		websWrite(wp,"L2TP");
	*/
	return 0;
}

int ej_ddm_show_lanproto(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	if( !strcmp(nvram_safe_get("lan_proto"),"dhcp")) 
		websWrite(wp,"DHCP");
	else if( !strcmp(nvram_safe_get("lan_proto"),"static")) 
		websWrite(wp,"STATIC");
	return 0;
}	
int ej_ddm_show_idletime(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	//junzhao 2004.3.23
	char ddm_encapmode[20];
	char word[140], *next;
	int whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
	int idletime = 0 ;
	memset(ddm_encapmode, 0, sizeof(ddm_encapmode));
	
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(whichconn-- == 0)
		{
			string_sep(word, 3, ddm_encapmode);
			break;
		}
	}
	if(!strcmp(ddm_encapmode, "pppoe"))
	{
		char pppoe_idletime[8];
		char pppoe_demand[3];
		memset(pppoe_idletime, 0, sizeof(pppoe_idletime));
		memset(pppoe_demand, 0, sizeof(pppoe_demand));
		
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoe_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 3, pppoe_demand);
				break;
			}
		}
		if(!strcmp(pppoe_demand, "1"))
		{
			whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
			foreach(word, nvram_safe_get("pppoe_config"), next)
			{
				if(whichconn-- == 0)
				{
					string_sep(word, 4, pppoe_idletime);
					break;
				}
			}
			idletime = atoi(pppoe_idletime) * 60;
			websWrite(wp,"%d",idletime);
		}
		else
			websWrite(wp,"%d",idletime);
	}	
	else if(!strcmp(ddm_encapmode, "pppoa"))
	{
		char pppoa_idletime[8];
		char pppoa_demand[3];
		memset(pppoa_idletime, 0, sizeof(pppoa_idletime));
		memset(pppoa_demand, 0, sizeof(pppoa_demand));
		
		whichconn = atoi(nvram_safe_get("wan_ddm_connection"));
		foreach(word, nvram_safe_get("pppoa_config"), next)
		{
			if(whichconn-- == 0)
			{
				string_sep(word, 3, pppoa_demand);
				break;
			}
		}
		if(!strcmp(pppoa_demand, "1"))
		{
			string_sep(word, 4, pppoa_idletime);
			idletime = atoi(pppoa_idletime) * 60;
			websWrite(wp,"%d",idletime);
		}
		else
			websWrite(wp,"%d",idletime);
	}	
	else
		websWrite(wp,"%d",idletime);


	/*
	if( !strcmp(nvram_safe_get("wan_proto"),"pppoe") || !strcmp(nvram_safe_get("wan_proto"),"pptp") || !strcmp(nvram_safe_get("wan_proto"),"l2tp") || !strcmp(nvram_safe_get("wan_proto"),"heartbeat") ) 
	{	
		idletime = atoi(nvram_safe_get("ppp_idletime"));
		idletime = idletime * 60;
		websWrite(wp,"%d",idletime);
	}
	else  
	{	
		websWrite(wp,"%d",idletime);
	}
	*/	
	return 0;
}	
int ej_ddm_error_no(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	websWrite(wp,nvram_safe_get("DDM_Error_No"));
	nvram_set("DDM_Error_No","0");
	return 0;
}	
int ej_ddm_error_desc(int eid ,webs_t wp ,int argc ,char_t **argv)
{
	websWrite(wp,nvram_safe_get("DDM_Error_Desc"));
	nvram_set("DDM_Error_Desc","No Error");
	return 0;
}	
int check_ip_range(int start, int end, char *value_ip)
{
	int value = atoi(value_ip);
	if( value == 0 )
	{
		if( strcmp( value_ip, "0" ) )
			value = -1;
	}	
	if( (value >= start) && (value <= end) )
	{
		cprintf("check ip range --> legal [%d]\r\n",value);
		return 1;
	}	
	else
	{	
		cprintf("check ip range --> illegal [%d]\r\n",value);
		return 0;
	}	
}	

int get_wan_dns(char *wan_dns_addr)
{
	char src_dns[3][16] = {"","",""} ;
	char single_ip_tmp[5] = "";
	int dns_tmp = 0;
	int num_dns = 0;
	int value_dns = 0;
	int l = 0;
	int k = sizeof(char);
	for(num_dns = 0 ;num_dns < 3; num_dns++)
	{
		for(value_dns = 0; value_dns < 4; value_dns++)
		{
			dns_tmp = get_dns_ip("wan_dns",num_dns,value_dns);
			snprintf(single_ip_tmp,sizeof(single_ip_tmp),"%d",dns_tmp);
			strcat(src_dns[num_dns],single_ip_tmp);
			if(value_dns<3)
				strcat(src_dns[num_dns],".");
		}
		strcat(src_dns[num_dns],"\0");
		cprintf("\r\n");
	}	
	for( num_dns = 0; num_dns < 3; num_dns++ )
	{
		for( value_dns = 0; value_dns < 16; value_dns++ )
		{
			*( wan_dns_addr + ( l*k ) ) = src_dns[num_dns][value_dns];
			l++;
		}	
		cprintf("\r\n");
	}	
	return 0;
}	
