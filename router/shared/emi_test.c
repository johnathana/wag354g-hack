
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

#include <broadcom.h>

#ifdef EMI_TEST

#define TMP_FILE "/tmp/.emi_test"

char *command, *bground;

int
emi_test(int flag)
{
	char buf[254];

	command = websGetVar(wp, "command", NULL);
	bground = websGetVar(wp, "background", NULL);

	if(flag == 1){		// exec
		if(!command)	return 0;

		snprintf(buf, sizeof(buf), "%s > %s 2>&1 %c",command, TMP_FILE, bground ? '&' : ' ');
		system(buf);
	}
	else if(flag == 2){	// delete
		unlink(TMP_FILE);
		system("killall -9 epi_ttcp");
		system("killall -9 wl");
	}

	return 0;
}

int
emi_test_exec(webs_t wp)
{
	int ret;

	ret = emi_test(1);

	return ret; 
}

int
emi_test_del(webs_t wp)
{
	int ret;

	ret = emi_test(2);

	return ret; 
}


int
ej_dump_emi_test_log(int eid, webs_t wp, int argc, char_t **argv)
{
	char line[256];
	FILE *fp;
	int ret = 0;

	ret += websWrite(wp,"<font color=blue size=4>Command:</font><br>\n");
	ret += websWrite(wp,"<font color=black size=2>%s</font><br><br>\n",command ? command : " ");

	ret += websWrite(wp,"<font color=blue size=4>Result:</font><br>\n");

	if ((fp = fopen(TMP_FILE, "r")) != NULL) {		// show result
		while( fgets(line, sizeof(line), fp) != NULL ) {
			ret += websWrite(wp,"%s<br>\n",line);
		}
		fclose(fp);
	}

	return ret;
}

#endif
