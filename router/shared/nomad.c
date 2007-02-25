/******************************************************************************
* 
*Copyright(c) 2005, CyberTAN Technology
*
*All rights reserved.
*
*FILENAME          : nomad.c
*
*ABSTRACT          : support quickVPN function for linksys.This file supply the
*                    Communication between the RoadWarrior Client, and the SG. 
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
******************************************************************************/

/******************************* head file include ***************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <bcmnvram.h>

#include "cy_conf.h"
#include "nomad.h"
/*****************************************************************************/

#ifdef NOMAD_SUPPORT

/*************************** data and declare ********************************/
typedef FILE * webs_t;//use only for this file

/* declare functions and Data */
extern struct mymsgbuf nomadinfo;                //save some global variable
extern char g_chremoteip[MAX_NOMAD][MAX_IPADDR]; //save remote RoadWarrior Client IP

extern int wfputs(char *buf, FILE *fp);
extern int wfprintf(FILE *fp, char *fmt,...);
/*****************************************************************************/


/******************************************************************************
*
*NAME              : nomad_info_parse()
*DESCRIPTION       : parse the info msg that receive from RoadWarrior Client
*
*INPUTS            : input1       ---path
*OUTPUTS(O)        : output1      ---nRtCode
*                    output2      ---len
*                    output3      ---nUrlType
*RETURN            : (char *)file
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
char *nomad_info_parse(char *path, int *nRtCode, int *len, int *nUrlType)
{
	char *nomad_version, *nomad_user, *nomad_status, *nomad_passwd;
	char *nomad_ip = NULL;
	char *file = &(path[1]);
	
	/* judge nomad's particular information */
	if(NULL != strstr(path, ".htm?version="))
	{
		file = strsep(&path, "?");
		/* Start connection */
		if(0 == strncasecmp(file, "/StartConnection.htm", 20))
		{
			if((NULL != strstr(path, "version=")) && (NULL != strstr(path, "IP="))
					&& (NULL != strstr(path, "USER=")))//assert
			{
				/* parse start conn info msg */
				nomad_version = strsep(&path, "?");
				nomad_version += 8;

				if('1' != *nomad_version)
					*nRtCode = NOMAD_ERMSG_VERSION;
				nomad_ip = strsep(&path, "?");
				nomad_ip += 3;

				nomad_user = path + 5;
				file = "nomad.lxb";
				*nUrlType = QV_START;
				*len = strlen(file);
			}

			else //error msg
				*nRtCode = NOMAD_NO_CONN;
		}

		/* stop connection */
		else if(0 == strncasecmp(file, "/StopConnection.htm", 19))
		{
			if((NULL != strstr(path, "version=")) && (NULL != strstr(path, "status="))
					&& (NULL != strstr(path, "IP=")) && (NULL != strstr(path, "USER=")))//assert
			{

				/* parse stop conn info msg */
				nomad_version = strsep(&path, "?");
				nomad_version += 8;
				if('1' != *nomad_version)
					*nRtCode = NOMAD_ERMSG_VERSION;
				nomad_status = strsep(&path, "?");
				nomad_ip = strsep(&path, "?");
				nomad_ip += 3;
				nomad_user = path + 5;
				file = "nomad.lxb";
				*nUrlType = QV_STOP;
				*len = strlen(file);
			}

			else //error msg
				*nRtCode = NOMAD_NO_CONN;
		}

		/* change password msg */
		else if(0 == strncasecmp(file, "/ChangePasswd.htm", 17))
		{
			if((NULL != strstr(path, "version=")) && (NULL != strstr(path, "PASSWORD="))
					&& (NULL != strstr(path, "USER=")))//assert
			{
				/* parse change passwd info msg */
				nomad_version = strsep(&path, "?");
				nomad_version += 8;
				if('1' != *nomad_version)
					*nRtCode = NOMAD_ERMSG_VERSION;
				nomad_passwd = strsep(&path, "?");
				nomad_passwd += 9;
				if(strlen(nomad_passwd) >= (MAX_NAME - 4))
					*nRtCode = NOMAD_ERLONG_SECRET;
				else
					strcpy(nomadinfo.xchpasswd, nomad_passwd);
				nomad_user = path + 5;
				file = "nomad.lxb";
				*nUrlType = QV_CHANGE;
				*len = strlen(file);
			}
			else //error msg
				*nRtCode = NOMAD_NO_CONN;
		}

		else //It isn't nomad info
		{
			file++;
			*len = strlen(file);
			*nRtCode = 1;
		}
		/*printf("file = %s IP = %s user = %s true_ip = %s\n",
				file, nomad_ip, nomad_user, nomadinfo.packet_ip);*/ //xiaoqindebug
		if(NULL != nomad_ip)
		{
			strcpy(nomadinfo.pcinfo_ip, nomad_ip);
		}

	}
	else //It isn't nomad info
	{
		*nRtCode = 1;
	}
	
	return file;
}


/******************************************************************************
*
*NAME              : valid_remote_ip()
*DESCRIPTION       : Judge the RoadWarrior Client Ip whether valid or not
*
*INPUTS            : NONE
*OUTPUTS(O)        : NONE
*RETURN            : int(rwReturnCode)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
int valid_remote_ip(void)
{
	char chlanip[16], subnet_local[16], netmask[16], subnet_remote[16];
	char *chnvramget;
	char vpnname[20];
	char *value, *online;
	int a[4], b[4], c[4];
	int i;
	
	/* get the lan_ip and lan_mask from nvram */
	chnvramget = nvram_get("lan_ipaddr");
	strcpy(chlanip, chnvramget);
	chnvramget = nvram_get("lan_netmask");
	strcpy(netmask, chnvramget);
	
	/* get local subnet */
	sscanf(chlanip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	sscanf(netmask, "%d.%d.%d.%d", &b[0], &b[1], &b[2], &b[3]);
	sprintf(subnet_local, "%d.%d.%d.%d", a[0]&b[0], a[1]&b[1], a[2]&b[2], a[3]&b[3]);

	/* get remote subnet */
	sscanf(nomadinfo.pcinfo_ip, "%d.%d.%d.%d", &c[0], &c[1], &c[2], &c[3]);
	sprintf(subnet_remote, "%d.%d.%d.%d", c[0]&b[0], c[1]&b[1], c[2]&b[2], c[3]&b[3]);
	//printf("local = %s remote = %s\n", subnet_local, subnet_remote);//xiaoqindebug
	
	/* the same subnet return error */
	if(0 == strcmp(subnet_local, subnet_remote))
		return NOMAD_CONFLICT_SUBNET;

	/* the same IP with the other remote client return error */
	for(i = 0; i < MAX_NOMAD; i++)
	{
		/* itself */
		if(nomadinfo.nusrindex == i + 1)
			continue;

		/* get the state of connection type */
		sprintf(vpnname, "vpn_client_stat_%d", (i + 1));
		value = nvram_get(vpnname);
		online = strsep(&value, " ");
		/* when quickVPN had disconnected, ignore the remote IP */
		if(0 == strcmp(online, "0"))//Is disconnected
			continue;

		/* the same IP */
		if(0 == strcmp(nomadinfo.pcinfo_ip, g_chremoteip[i]))
			return NOMAD_IP_USED;
	}
	
	return NOMAD_SUCCESS;//valid
}

/******************************************************************************
*
*NAME              : nomad_auth_check()
*DESCRIPTION       : nomad authorization check
*
*INPUTS            : input1       ---dirname
*                    input2       ---authorization
*OUTPUTS(O)        : NONE
*RETURN            : int(rwReturnCode)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
int nomad_auth_check( char* dirname, char* authorization)
{
	char authinfo[500], chtemp[16];
	char* authpass;
	char *nomaduser, *nomadpasswd, *changeflg, *activeflg;
	char *vpnaccount;
	int nValue, i;
	

	/* Basic authorization info? */
	if ( !authorization || strncmp( authorization, "Basic ", 6 ) != 0 )
	{
    		send_authenticate( dirname );
		return -100;
	}

	/* Decode it. */
	nValue = b64_decode( &(authorization[6]), authinfo, sizeof(authinfo) );
	authinfo[nValue] = '\0';
	/* Split into user and password. */
	authpass = strchr( authinfo, ':' );
	if ( authpass == (char*) 0 ) 
	{
		/* No colon?  Bogus auth info. */
		send_authenticate( dirname );
		return -101;
	}

	/* get the user name */
	*authpass++ = '\0';
	strcpy(nomadinfo.username, authinfo);
	//printf("user = %s passwd = %s\n", authinfo, authpass);//xiaoqindebug

	/* authorization check */
	for(i = 0; i < MAX_NOMAD; i++)
	{
		/* get info from nvram */
		sprintf(chtemp, "vpn_account_%d", (i+1));
		vpnaccount = nvram_get(chtemp);

		/* get user name */
		nomaduser = strsep(&vpnaccount, " ");
		if(!vpnaccount || !nomaduser)
			continue;
		/* get user passwd */
		nomadpasswd = strsep(&vpnaccount, " ");
		if(!vpnaccount || !nomadpasswd)
			continue;
		/* get change passwd flag */
		changeflg = strsep(&vpnaccount, " ");
		if(!vpnaccount || !changeflg)
			continue;
		/* get active flag */
		activeflg = vpnaccount;

		/* judge whether active */
		if(0 == strcmp(activeflg, "0"))
			continue;
		/* judge user name */
		if(0 != strcmp(nomaduser, authinfo))
			continue;
		/* judge user passwd */
		if(0 != strcmp(nomadpasswd, authpass))
			return NOMAD_FAILED_PASSWD;

		nomadinfo.nusrindex = i + 1;
		strcpy(nomadinfo.changpasswd, changeflg);

		return NOMAD_SUCCESS;//success
	}

	return NOMAD_UNKNOWN_USER;
}


/******************************************************************************
*
*NAME              : random_seed()
*DESCRIPTION       : Get one random number
*
*INPUTS            : input1       ---number
*                    input2       ---npend
*OUTPUTS(O)        : NONE
*RETURN            : int(random number)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
int random_seed(int number, int npend)
{
	srandom((unsigned int)time(NULL) + npend);
	return random() % number;
}

/******************************************************************************
*
*NAME              : create_random_psk()
*DESCRIPTION       : create random_PSK
*
*INPUTS            : NONE
*OUTPUTS(O)        : NONE
*RETURN            : NONE
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
void create_random_psk(void)
{
	int i;
	int npos;
	/* random range */
	char cSeed_buf[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	/* create random_PSK length */
	int nLen = 10 + random_seed(23, 0);
	/* create random_PSK */
	for (i = 0; i < nLen; i++)
	{
		npos = random_seed(strlen(cSeed_buf), (i + 3));
		nomadinfo.randompsk[i] = cSeed_buf[npos];
	}
	nomadinfo.randompsk[nLen] = '\0';
}


/******************************************************************************
*
*NAME              : nomad_startconn_success()
*DESCRIPTION       : Send the success msg of start connection to client
*
*INPUTS            : input1       ---stream
*OUTPUTS(O)        : NONE
*RETURN            : NONE
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
void nomad_startconn_success(webs_t stream)
{
	char chlanip[16], subnet[16], netmask[16];
	char *chnvramget; 
	int a[4], b[4];

	/* get the lan_ip and lan_mask from nvram */
	chnvramget = nvram_get("lan_ipaddr");
	strcpy(chlanip, chnvramget);
	chnvramget = nvram_get("lan_netmask");
	strcpy(netmask, chnvramget);

	/* send msg */
	wfputs("version=1\r\n", stream);
	wfputs("msgtype=configuration\r\n", stream);
	wfprintf(stream, "conn %s_rw_rw\r\n", nomadinfo.username);
	wfprintf(stream, "presharedkey=%s\r\n", nomadinfo.randompsk);
	
	/* get the local subnet */
	sscanf(chlanip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	sscanf(netmask, "%d.%d.%d.%d", &b[0], &b[1], &b[2], &b[3]);
	sprintf(subnet, "%d.%d.%d.%d", a[0]&b[0], a[1]&b[1], a[2]&b[2], a[3]&b[3]);
	
	/* send msg */
	wfprintf(stream, "rightsubnet=%s/%s\r\n", subnet, netmask);
	wfprintf(stream, "dnsserver=%s\r\n", chlanip);
	wfprintf(stream, "domain=%s\r\n", nvram_get("wan_domain"));
}

/******************************************************************************
*
*NAME              : nomad_startconn_error()
*DESCRIPTION       : Send the error msg of start connection to client
*
*INPUTS            : input1       ---nReturnCode
*                    input2       ---stream
*OUTPUTS(O)        : NONE
*RETURN            : NONE
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
void nomad_startconn_error(int nReturnCode, webs_t stream)
{
	/* send msg */
	wfputs("Version=1\r\n", stream);
	wfputs("msgtype=status\r\n", stream);
	wfprintf(stream, "status=%d\r\n", nReturnCode);
	/* judge nReturnCode, send relative msg */
	switch(nReturnCode)
	{
		case NOMAD_UNKNOWN_USER:
			wfputs("message=Unknown User\r\n", stream);
			break;
		case NOMAD_NO_USED:
			wfputs("message=Not Currently Used\r\n", stream);
			break;
		case NOMAD_FAILED_PASSWD:
			wfputs("message=Password Failure\r\n", stream);
			break;
		case NOMAD_FAILED_HOSTNAME:
			wfputs("message=Cannot Resolve Hostname of SG\r\n", stream);
			break;
		case NOMAD_FAILED_CONN:
			wfputs("message=Cannot Connect to SG\r\n", stream);
			break;
		case NOMAD_NO_CONN:
			wfputs("message=Connection not found on SG\r\n", stream);
			break;
		case NOMAD_FAILED_VERSION:
			wfputs("message=Incompatible Versions. Server Version is > RW Client Version\r\n", stream);
			break;
		case NOMAD_CORRUPT_FILE:
			wfputs("message=Initialization File is Corrupt\r\n", stream);
			break;
		case NOMAD_EROPEN_FILE:
			wfputs("message=Unable to open the Initialization File\r\n", stream);
			break;
		case NOMAD_ERREAD_FILE:
			wfputs("message=Read error for Initialization File\r\n", stream);
			break;
		case NOMAD_EROPEN_VPN:
			wfputs("message=Unable to open vpn  config  file returned via wget\r\n", stream);
			break;
		case NOMAD_EROPEN_IPSEC:
			wfputs("message=Unable to open ipsec.conf file for writing\r\n", stream);
			break;
		case NOMAD_ERCALL_IPSEC:
			wfputs("message=Error was returned from call to ipsec.exe\r\n", stream);
			break;
		case NOMAD_ERSERVER_DNS:
			wfputs("message=Error returned while configuring DNS Server\r\n", stream);
			break;
		case NOMAD_ERSEARCH_DNS:
			wfputs("message=Error returned while configuring the DNS Search List\r\n", stream);
			break;
		case NOMAD_ERDEF_FILE:
			wfputs("message=Working Directory incorrectly defined in Initialization File\r\n", stream);
			break;
		case NOMAD_FAILED_PROCESS:
			wfputs("message=CreateProcess call failed\r\n", stream);
			break;
		case NOMAD_ERLONG_SECRET:
			wfputs("message=Shared Secret too long\r\n", stream);
			break;
		case NOMAD_ERMSG_VERSION:
			wfputs("message=Server cannot handle request because version of message is incorrect\r\n", stream);
			break;
		case NOMAD_CONFLICT_SUBNET:
			wfputs("message=Conflict between protected subnet and client subnet\r\n", stream);
			break;
		case NOMAD_IP_USED:
			wfputs("message=Client's IP Addresss already used on Server\r\n", stream);
			break;
		default:
			wfputs("message=Server defined error\r\n", stream);
			break;
	}
}


/******************************************************************************
*
*NAME              : nomad_systime_proc()
*DESCRIPTION       : restore the system time in nvram for GUI replace
*
*INPUTS            : input1       ---noperation
*                    input2       ---clear
*OUTPUTS(O)        : NONE
*RETURN            : NONE
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
void nomad_systime_proc(int noperation, quick_vpn_t *clear)
{
	time_t ltimep;
	struct tm *sttimsys;
	char vpnname[20], vpnsavebuf[16];

	/* Get the system time */
	ltimep = time(NULL);
	sttimsys = localtime(&ltimep);

	sprintf(vpnname, "vpn_client_stat_%d", nomadinfo.nusrindex);
	if(QV_START == noperation) //restore VPN start time
	{
		sprintf(vpnsavebuf, "1 %02d:%02d --", sttimsys->tm_hour, sttimsys->tm_min);
		strcpy(g_chremoteip[nomadinfo.nusrindex-1], nomadinfo.pcinfo_ip);
		nvram_set(vpnname, vpnsavebuf);
	}
	else if(QV_STOP == noperation) //restore VPN stop time
	{
		char *temp, *value;
		value = nvram_get(vpnname);
		temp = strsep(&value, " ");//first strsep:temp == online flag
		temp = strsep(&value, " ");//second strsep:temp == quickvpn start time
		sprintf(vpnsavebuf, "0 %s %02d:%02d", temp, sttimsys->tm_hour, sttimsys->tm_min);
		strcpy(g_chremoteip[nomadinfo.nusrindex-1], "offline");
		nvram_set(vpnname, vpnsavebuf);
	}
	/* If username is already connected then disconnect the original connection */
	else if(QV_STATE == noperation)
	{
		char *temp, *value;
		value = nvram_get(vpnname);
		temp = strsep(&value, " ");
		if(0 == strcmp(temp, "1"))
		{
			clear_quickvpn(clear, nomadinfo.nusrindex);
			sleep(2);//wait for pluto process delete complete
		}
	}
	else //this case can not happen for ever
	{
		printf("************This may be a BIG bug!**********\n");//xiaoqindebug
	}
	nvram_commit();
}


/******************************************************************************
*
*NAME              : gui_stopnomad_conn()
*DESCRIPTION       : stop VPN connection interface for GUI
*
*INPUTS            : input1       ---n_index
*OUTPUTS(O)        : NONE
*RETURN            : int(rwReturnCode)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 04, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.04
**************************************************************************** */
int gui_stopnomad_conn(int n_index)
{
	int nRvalue = 0;
	char *name, *value;
	char chtemp[16];
	quick_vpn_t gui_stopvpn;

	if(n_index < 1 || n_index > MAX_NOMAD)
		return NOMAD_UNKNOWN_USER;
	memset(&gui_stopvpn, 0x0, sizeof(quick_vpn_t));

	/* get user name */
	nomadinfo.nusrindex = n_index;
	sprintf(chtemp, "vpn_account_%d", n_index);
	value = nvram_get(chtemp);
	name = strsep(&value, " ");
	if(NULL != name)
		strcpy(gui_stopvpn.username, name);
	else
		return NOMAD_UNKNOWN_USER;

	/* restore stop time */
	nomad_systime_proc(QV_STOP, &gui_stopvpn);
	/* clear QuickVPN */
	nRvalue = clear_quickvpn(&gui_stopvpn, n_index);
	
	return nRvalue;
}



/******************************************************************************
*
*NAME              : init_vpnstruct_data()
*DESCRIPTION       : initialize the struct for clear or start QuickVPN
*
*INPUTS            : input1       ---p_start_vpn
*OUTPUTS(O)        : NONE
*RETURN            : NONE
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 05, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.05
**************************************************************************** */
void init_vpnstruct_data(quick_vpn_t *p_start_vpn)
{
	strcpy(p_start_vpn->post_nat_ip, nomadinfo.packet_ip);
	strcpy(p_start_vpn->pre_nat_ip, nomadinfo.pcinfo_ip);
	create_random_psk(); //create Random_PSK
	strcpy(p_start_vpn->random_psk, nomadinfo.randompsk);
	strcpy(p_start_vpn->username, nomadinfo.username);
}


/******************************************************************************
*
*NAME              : do_nomad_file()
*DESCRIPTION       : the entrance of Communication between the RoadWarrior
*                    Client and the SG
*
*INPUTS            : input1       ---path
*                    input2       ---stream
*                    input3       ---flag
*OUTPUTS(O)        : NONE
*RETURN            : int(error -1;success 0)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 05, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.05
**************************************************************************** */
int do_nomad_file(char *path, webs_t stream, int flag)
{
	int nReturnCode;
	quick_vpn_t start_vpn;

	/* initialize data */
	memset(&start_vpn, 0x0, sizeof(quick_vpn_t));
	nReturnCode = nomadinfo.nRtCode;
	//printf("nReturnCode = %d\n", nReturnCode);//xiaoqindebug

	if(QV_START == flag)//Start connection
	{
		if(NOMAD_SUCCESS == nReturnCode)//first Communication success
		{
			int nRvalue = 0;

			init_vpnstruct_data(&start_vpn);         //init struct
			nomad_systime_proc(QV_STATE, &start_vpn);//restore start time
			nomad_startconn_success(stream);         //Communication
			nRvalue = start_quickvpn(&start_vpn, nomadinfo.nusrindex);
			//printf("start_vpn:nRvalue = %d\n", nRvalue);//xiaoqindebug
			//while(1){receive the msg about pluto process};//delete
			
			/* start VPN failed */
			if(NOMAD_SUCCESS != nRvalue)
				nomad_startconn_error(nRvalue, stream);
			else ////second Communication success
			{
				//printf("Nomad Start_Connection is successful!\n");//xiaoqindebug
				nomad_systime_proc(QV_START, &start_vpn);
			}
		}
		else//start connection failed
		{
			nomad_startconn_error(nReturnCode, stream);
		}
	}//end QV_START == flag
	else if(QV_STOP == flag)//stop connection
	{
		int nRvalue = 0;
		char vpnname[20];
		char *value, *online;

		/* get the state of connection type */
		sprintf(vpnname, "vpn_client_stat_%d", nomadinfo.nusrindex);
		value = nvram_get(vpnname);
		online = strsep(&value, " ");
		if(0 == strcmp(online, "1"))//Is connected
		{
			/* judge whether the same remote PC */
			if(0 == strcmp(nomadinfo.pcinfo_ip, g_chremoteip[nomadinfo.nusrindex - 1]))
			{
				/* stop the QuickVPN */
				strcpy(start_vpn.username, nomadinfo.username);
				nomad_systime_proc(QV_STOP, &start_vpn);
				nRvalue = clear_quickvpn(&start_vpn, nomadinfo.nusrindex);
			}
			else //It's not a same remote PC
				printf("It's NOT the same PC, cann't stop the connection by other PC!\n");//xiaoqindebug
		}//else if : nomad vpn had already closed.
		
		/* send the successful msg */
		wfputs("version=1\r\n", stream);
		wfputs("msgtype=status\r\n", stream);
		wfprintf(stream, "status=%d\r\n", nRvalue);
		wfputs("message=Stop Connection\r\n", stream);
	}//end QV_STOP == flag
	else if(QV_CHANGE == flag) //change password
	{
		char vpnname[20];
		char *value, *online;
		
		/* get the state of connection type */
		sprintf(vpnname, "vpn_client_stat_%d", nomadinfo.nusrindex);
		value = nvram_get(vpnname);
		online = strsep(&value, " ");
		/* when quickVPN had disconnected, can not change the password */
		if(0 == strcmp(online, "0"))//Is disconnected
		{
			nReturnCode = NOMAD_NO_CONN;
			//printf("quickVPN had disconnected, can not change the password.\n");//xiaoqindebug
		}
		
		if(0 == strcmp(nomadinfo.changpasswd, "0")) //disallow change password
		{
			/* send the disallow change password message */
			wfputs("Version=1\r\n", stream);
			wfputs("msgtype=status\r\n", stream);
			wfprintf(stream, "status=%d\r\n", nReturnCode);
			wfputs("message=Disallow Change Password\r\n", stream);
		}
		else //allow change password
		{
			/* success or ignore the -27 error return code, because change password info
			   does not take IP info, otherwise change password mustn't consider IP info */
			if((NOMAD_SUCCESS == nReturnCode) || (NOMAD_IP_USED == nReturnCode))
			{
				char tmpname[16], savebuf[80];
				char *tmpvalue, *pname, *passwd;
				
				/* change the password and restore in nvram */
				sprintf(tmpname, "vpn_account_%d", nomadinfo.nusrindex);
				tmpvalue = nvram_get(tmpname);
				pname = strsep(&tmpvalue, " ");
				passwd = strsep(&tmpvalue, " ");
				sprintf(savebuf, "%s %s %s", pname, nomadinfo.xchpasswd, tmpvalue);
				nvram_set(tmpname, savebuf);
				nvram_commit();

				/* send the successful message */
				wfputs("version=1\r\n", stream);
				wfputs("msgtype=status\r\n", stream);
				wfputs("status=0\r\n", stream);
				wfputs("message=Password Changed\r\n", stream);
			}
			else /* Return code error, change password failed */
			{
				/* send the failed message */
				wfputs("Version=1\r\n", stream);
				wfputs("msgtype=status\r\n", stream);
				wfprintf(stream, "status=%d\r\n", nReturnCode);
				wfputs("message=Change Password Failed\r\n", stream);
			}
		}
	}//end QV_CHANGE == flag
	else //this case can not happen for ever
	{
		printf("*****This may be anything Wrong, BUG!*****\n");//xiaoqindebug
		return -1;
	}
	
	return 0;//success
}
#endif /* NOMAD_SUPPORT */

