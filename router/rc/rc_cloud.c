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
#include <bcmnvram.h>
#include <shutils.h>
#include <utils.h>
#include <cy_conf.h>
#include <code_pattern.h>
#include <rc.h>

int start_cloud()
{
#ifdef CLOUD_SUPPORT
	FILE *cloud_fp;
	if(!(cloud_fp = fopen("/proc/Cybertan/cloud_enable","w")))
	{
		cprintf("Error to write cloud_enable!\n");
		return -1;
		
	}
	
	fprintf(cloud_fp,"%d",1);
	fclose(cloud_fp);
#endif
	return 0;

}

int stop_cloud()
{
#ifdef CLOUD_SUPPORT
	FILE *cloud_fp;
	if(!(cloud_fp = fopen("/proc/Cybertan/cloud_enable","w")))
	{
		cprintf("Error to write cloud_enable!\n");
		return -1;
		
	}
	
	fprintf(cloud_fp,"%d",0);
	fclose(cloud_fp);
#endif
	return 0;

}


int enable_cloud()
{
#ifdef CLOUD_SUPPORT
	stop_dhcpd();
	start_dhcpd();
#endif	

	return 0;
}

int disable_cloud()
{
	return 0;
}

