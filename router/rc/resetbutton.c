
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

/*
 * Reset button 
 *
 * Copyright 2002, Cybertan Corporation
 * All Rights Reserved.
 *
 * Description:
 *   This program checks the Reset-Button status periodically.
 *   There is two senaria, whenever the button is pushed
 *     o  Less than 3 seconds : reboot.
 *     o  Greater than 3 seconds : factory default restore, and reboot. 
 *
 *   The reset-button is connected to the GPIO pin, it has character
 *   device driver to manage it by file operation read/write.
 *
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Cybertan Corporation;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Cybertan Corporation.
 *
 */

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <utils.h>
#include <rc.h>
#include <stdarg.h>

#define BCM47XX_SOFTWARE_RESET  0x40		/* GPIO 6 */
#define RESET_WAIT		3		/* seconds */
#define RESET_WAIT_COUNT	RESET_WAIT * 10 /* 10 times a second */

#define NORMAL_INTERVAL		1		/* second */
#define URGENT_INTERVAL		100 * 1000	/* microsecond */	
						/* 1/10 second */
#define RESET_FILE		"/proc/led_mod/ar7reset"

#if 0 
#define DEBUG printf
#else
#define DEBUG(format, args...) 
#endif


static int mode = 0;		/* mode 1 : pushed */
static int count = 0;


static void
alarmtimer(unsigned long sec,unsigned long usec)
{
	struct itimerval itv;
 
	itv.it_value.tv_sec  = sec;
	itv.it_value.tv_usec = usec;

	itv.it_interval = itv.it_value;
	setitimer(ITIMER_REAL, &itv, NULL);
}

/*
void system_reboot(void)
{
	DEBUG("resetbutton: reboot\n");
	alarmtimer(0, 0);
	eval("reboot");
}
*/

void service_restart(void)
{
	DEBUG("resetbutton: restart\n");
	/* Stop the timer alarm */
	alarmtimer(0, 0);
	/* Reset the Diagnostic LED */
	/* Restart all of services */
	eval("rc", "restart");
}

void period_check(int sig)
{
	FILE *fp;
	unsigned int val = 0;
	char buff[7];

	if( (fp=fopen(RESET_FILE, "r")) ){
		memset((void *)buff, 0, sizeof(buff));
		fgets(buff, sizeof(buff), fp);
		val = atoi(buff);
		fclose(fp);
	} else
		perror(RESET_FILE);


	/*  The value is zero during button-pushed. */
	if (val == 1){
		DEBUG("resetbutton: mode=%d, count=%d\n", mode, count);

		if ( mode == 0){
			/* We detect button pushed first time */
			alarmtimer(0, URGENT_INTERVAL);
			mode = 1;
		}
		else{	/* Whenever it is pushed steady */
			if( ++count > RESET_WAIT_COUNT ){
				if(check_action() != ACT_IDLE){	// Don't execute during upgrading
					fprintf(stderr, "resetbutton: nothing to do...\n");
					alarmtimer(0, 0); /* Stop the timer alarm */
					return;
				}
				printf("resetbutton: factory default.\n");
				ACTION("ACT_HW_RESTORE");
				alarmtimer(0, 0); /* Stop the timer alarm */
				nvram_set("restore_defaults", "1");
				nvram_commit();
				kill(1, SIGTERM);

			}
		}
	}
	else{
		/* Although it's unpushed now, it had ever been pushed */
		if( mode == 1 ){
			if(check_action() != ACT_IDLE){	// Don't execute during upgrading
				fprintf(stderr, "resetbutton: nothing to do...\n");
				alarmtimer(0, 0); /* Stop the timer alarm */
				return;
			}
			mode = 0;
			diag_led_ctl(SYS_BOOTING);
			service_restart();
		}
	}
}


int resetbutton_main(int argc, char *argv[]){

	/* Run it under background */

	switch (fork()) {
	case -1:
		DEBUG("can't fork\n");
		exit(0);
		break;
	case 0:		
		/* child process */
		DEBUG("fork ok\n");
		(void) setsid();
		break;
	default:	
		/* parent process should just die */
		_exit(0);
	}

	/* set the signal handler */
	signal(SIGALRM, period_check);	

	/* set timer */
	alarmtimer(NORMAL_INTERVAL, 0);

	/* Most of time it goes to sleep */
	while(1)
		pause();
	
	return 0;
}
