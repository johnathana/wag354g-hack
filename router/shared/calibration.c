
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
#include <time.h>
#include <sys/klog.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <broadcom.h>

#define EEPROM_IMAGE_PATH	"/var/wlnv.bin"

static int
calibration(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	     char_t *url, char_t *path, char_t *query)
{
	char *serv, *file;
	char src[1024];
	int ret;

	/* URL example - http://192.168.1.1/calibration.cgi?tftp_serv=192.168.1.74&path=No24.bin */
	if((serv=websGetVar(wp, "tftp_serv", NULL)) == NULL || 
	   (file=websGetVar(wp, "path", NULL)) == NULL) {
		error_value = 1;
	}
	else {
		sprintf(src, "%s:%s", serv, file);
		printf("tftp get %s %s\n", src, EEPROM_IMAGE_PATH);

		/* transfer the image and burn to flash */
		if ((ret=eval("tftp", "get", src, EEPROM_IMAGE_PATH)) != 0 ||
		    (ret=eval("wlnv", EEPROM_IMAGE_PATH)) != 0) {
			error_value = 1;
		}
	}

	if(!error_value)
        	do_ej("Success.asp",wp);
	else
        	do_ej("Fail.asp",wp);

	websDone(wp, 200);
	error_value = 0;

	return 1;
}

int do_calibration(char *url, webs_t stream)
{
	char *path, *query;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): url=[%s]",__FUNCTION__,url);
#endif
	query = url;
	cprintf("url:%s\n", url);
	fprintf(stderr, "url:%s\n", url);
	path = strsep(&query, "?") ? : url;

	init_cgi(query);
	calibration(stream, NULL, NULL, 0, url, path, query);
	init_cgi(NULL);
	return 0;
}
