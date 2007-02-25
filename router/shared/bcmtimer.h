/*
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Low resolution timer interface. Timer handlers may be called 
 * in a deferred manner in a different task context after the 
 * timer expires or in the task context from which the timer
 * was created, depending on the implementation.
 *
 * $Id: bcmtimer.h,v 1.1.1.2 2005/03/28 06:58:28 sparq Exp $
 */
#ifndef __bcmtimer_h__
#define __bcmtimer_h__

/* ANSI headers */
#include <time.h>

/* timer ID */
typedef unsigned int bcm_timer_module_id;
typedef unsigned int bcm_timer_id;

/* timer callback */
typedef void (*bcm_timer_cb)(bcm_timer_id id, int data);

/* OS-independant interfaces, applications should call these functions only */
int bcm_timer_module_init(int timer_entries, bcm_timer_module_id *module_id);
int bcm_timer_module_cleanup(bcm_timer_module_id module_id);
int bcm_timer_create(bcm_timer_module_id module_id, bcm_timer_id *timer_id);
int bcm_timer_delete(bcm_timer_id timer_id);
int bcm_timer_gettime(bcm_timer_id timer_id, struct itimerspec *value);
int bcm_timer_settime(bcm_timer_id timer_id, const struct itimerspec *value);
int bcm_timer_connect(bcm_timer_id timer_id, bcm_timer_cb func, int data);
int bcm_timer_cancel(bcm_timer_id timer_id);

#endif	/* #ifndef __bcmtimer_h__ */
