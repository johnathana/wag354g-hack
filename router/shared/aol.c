
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
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>

#include <broadcom.h>

#ifdef AOL_SUPPORT

int
ej_aol_value_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type;
	char *hidden1, *hidden2;
	char *aol_block_traffic, *aol_block_traffic2;
	int init, hidden;

        if (ejArgs(argc, argv, "%s", &type) < 1) {
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	aol_block_traffic = nvram_safe_get("aol_block_traffic");
	aol_block_traffic2 = GOZILA_GET("aol_block_traffic2");

	if(gozila_action){	// select disabel or enable
		if(atoi(aol_block_traffic2) == 1)	// =1
			hidden = 1;
		else	// =0
			hidden = 0;
	}
	else{	// enter the page
		if(atoi(aol_block_traffic) != 0)	// =1 or =2(only for setup wizard)
			hidden = 1;
		else	// =0
			hidden = 0;
	}

	if(hidden){
		hidden1 = "<!--";
		hidden2 = "-->";
		init = 1;
	}
	else{
		hidden1 = " ";
		hidden2 = " ";
		init = 0;
	}

	if(!strcmp(type,"hidden1"))
		return websWrite(wp,"%s",hidden1);
	else if(!strcmp(type,"hidden2"))
		return websWrite(wp,"%s",hidden2);
	else if(!strcmp(type,"init"))
		return websWrite(wp,"%d",init);
	else
		return 0;

}

void
validate_aol(webs_t wp, char *value, struct variable *v)
{
	if(valid_choice(wp, value, v))
		nvram_set(v->name, value);

	if(atoi(value)==0)	// disable
		nvram_set("aol_block_traffic","0");
	else{			// enable
		if(nvram_match("aol_block_traffic","0"))
			nvram_set("aol_block_traffic","1");
		else if(nvram_match("aol_block_traffic","2"))
			nvram_set("aol_block_traffic","2");
		else
			nvram_set("aol_block_traffic","1");
	}
}

int
ej_aol_settings_show(int eid, webs_t wp, int argc, char_t **argv)
{

	do_ej("aol.asp",wp);	

	return 0;
}
#endif
