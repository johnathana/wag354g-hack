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

/******************************************************************************
 * FILE PURPOSE:    Watchdog Timer Module Header
 ********************************************************************************
 * FILE NAME:       wdtimer.h
 *
 * DESCRIPTION:     Platform and OS independent API for watchdog timer module
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/


#ifndef __WDTIMER_H__
#define __WDTIMER_H__

/* Return Codes */
#define WDTIMER_RET_OK			0
#define WDTIMER_RET_ERR			-1
#define WDTIMER_ERR_INVAL		-2

/****************************************************************************
 * Type:        PLA_SYS_WDTIMER_STRUCT_T
 ****************************************************************************
 * Description: This type defines the hardware configuration of the 
 *              watchdog timer
 ***************************************************************************/
typedef struct PAL_SYS_WDTIMER_STRUCT_tag
{
    UINT32  kick_lock;
    UINT32  kick;
    UINT32  change_lock;
    UINT32  change ; 
    UINT32  disable_lock;
    UINT32  disable;
    UINT32  prescale_lock;
    UINT32  prescale;
} PAL_SYS_WDTIMER_STRUCT_T;


/****************************************************************************
 * Type:        PAL_SYS_WDTIMER_CTRL_T
 ****************************************************************************
 * Description: This type defines start and stop values for the timer. 
 *              
 ***************************************************************************/
typedef enum PAL_SYS_WDTIMER_CTRL_tag
{
    WDTIMER_CTRL_DISABLE = 0,
    WDTIMER_CTRL_ENABLE
} PAL_SYS_WDTIMER_CTRL_T ;

void PAL_sysWdtimerInit(UINT32 base_addr, UINT32 clk_freq);
INT32 PAL_sysWdtimerSetPeriod( UINT32 msec );
INT32 PAL_sysWdtimerCtrl(PAL_SYS_WDTIMER_CTRL_T wd_ctrl);
INT32 PAL_sysWdtimerKick(void);

#endif /* __WDTIMER_H__ */
