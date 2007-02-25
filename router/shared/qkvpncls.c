/******************************************************************************
* 
*Copyright(c) 2005, CyberTAN Technology
*
*All rights reserved.
*
*FILENAME          : qkvpncls.c
*
*ABSTRACT          : support quickVPN function for linksys.This file supply the
*                    interface for clearing quickvpn when WAN down. 
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.11
******************************************************************************/

/***************************** head file include *****************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shutils.h>
#include <bcmnvram.h>

#include "cy_conf.h"
/*****************************************************************************/

#ifdef NOMAD_SUPPORT

/******************************************************************************
*
*NAME              : wandown_clear_quickvpn()
*DESCRIPTION       : run shell file to clear quickVPN connection
*
*INPUTS            : input1       ---index
*OUTPUTS(O)        : NONE
*RETURN            : int
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 11, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.11
**************************************************************************** */
int wandown_clear_quickvpn(int index)
{
	char del_fname[24];
	FILE *fp_del;

	memset(del_fname, 0x0, sizeof(del_fname));

	/* open the shell file */
	sprintf(del_fname, "%s%d", "/tmp/quickvpn/del_", index);
	fp_del = fopen(del_fname, "r");
	if(NULL == fp_del)
	{
		printf("ERROR, no such file:%s\n", del_fname);//xiaoqindebug
		return -9;
	}
	fclose(fp_del);

	/* run shell file */
	eval("/bin/sh", del_fname);
	eval("rm", del_fname);
	
	return 0;
}

/******************************************************************************
*
*NAME              : wandown_stopnomad_conn()
*DESCRIPTION       : the interface of stopping VPN connection when WAN had down
*
*INPUTS            : NONE
*OUTPUTS(O)        : NONE
*RETURN            : int(nRvalue)
*
*modification history
*------------------------------------------------------------------------------
* 1.00, Apr 08, 2005, xiaoqin written.
*------------------------------------------------------------------------------
*
*AUTHOR            : xiaoqin, CyberTAN Technology(GZ), 2005.04.08
**************************************************************************** */
int wandown_stopnomad_conn(void)
{
	time_t ltimep;
	struct tm *sttimsys;
	char *value, *temp;
	char vpnname[20], vpnsavebuf[16];
	int nRvalue = 1;
	int i;

	/* Get the system time */
	ltimep = time(NULL);
	sttimsys = localtime(&ltimep);
	
	/* MAX of user loop */
	for(i = 1; i <= 5; i++)
	{
		/* get the current account's state of connection type */
		sprintf(vpnname, "vpn_client_stat_%d", i);
		value = nvram_get(vpnname);
		if('1' == *value)//online
		{
			temp = strsep(&value, " ");//first strsep:temp == online
			temp = strsep(&value, " ");//second strsep:temp == start time
			sprintf(vpnsavebuf, "0 %s %02d:%02d", temp, sttimsys->tm_hour, sttimsys->tm_min);
			nvram_set(vpnname, vpnsavebuf);
	
			/* clear QuickVPN */
			nRvalue = wandown_clear_quickvpn(i);
		}
	}

	/* quickvpn stop time has changed, commit nvram */
	if(1 != nRvalue)
		nvram_commit();
	
	return nRvalue;//no useful
}
#endif /* NOMAD_SUPPORT */

