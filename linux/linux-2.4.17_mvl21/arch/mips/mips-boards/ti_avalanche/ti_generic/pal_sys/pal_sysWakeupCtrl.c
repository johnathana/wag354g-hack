/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2004 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/** \file   pal_sys_wakeup_ctrl.c
    \brief  PAL wakeup control code file
	
    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */
#include <pal.h>

/***********************************************************************
 *    Wakeup Control Module for TNETV1050 Communication Processor
 ***********************************************************************/

#define AVALANCHE_WAKEUP_POLARITY_BIT   16

void PAL_sysWakeupCtrl(PAL_SYS_WAKEUP_INTERRUPT_T wakeup_int,
                         PAL_SYS_WAKEUP_CTRL_T      wakeup_ctrl,
                         PAL_SYS_WAKEUP_POLARITY_T  wakeup_polarity)
{
    volatile unsigned int *wakeup_status_reg = (unsigned int*) AVALANCHE_WAKEUP_CTRL_WKCR;

    /* enable/disable */
    if (wakeup_ctrl == WAKEUP_ENABLED)
	{
        /* enable wakeup */
        *wakeup_status_reg |= wakeup_int;
	}
    else
	{
        /* disable wakeup */
        *wakeup_status_reg &= (~wakeup_int);
	}

    /* set polarity */
    if (wakeup_polarity == WAKEUP_ACTIVE_LOW)
	{
        *wakeup_status_reg |=  (wakeup_int << AVALANCHE_WAKEUP_POLARITY_BIT);
	}
    else
	{
        *wakeup_status_reg &= ~(wakeup_int << AVALANCHE_WAKEUP_POLARITY_BIT);
	}
}

