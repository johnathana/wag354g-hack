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
 * FILE PURPOSE:    16 bit Timer Module Header
 ********************************************************************************
 * FILE NAME:       timer16.h
 *
 * DESCRIPTION:     Platform and OS independent API for 16 bit timer module
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __TIMER16_H__
#define __TIMER16_H__


/****************************************************************************
 * Type:        PAL_SYS_TIMER16_STRUCT_T
 ****************************************************************************
 * Description: This type defines the hardware configuration of the timer
 *              
 ***************************************************************************/
typedef struct PAL_SYS_TIMER16_STRUCT_tag
{
    UINT32 ctrl_reg;
    UINT32 load_reg;
    UINT32 count_reg;
    UINT32 intr_reg;
} PAL_SYS_TIMER16_STRUCT_T;


/****************************************************************************
 * Type:        PAL_SYS_TIMER16_MODE_T
 ****************************************************************************
 * Description: This type defines different timer modes. 
 *              
 ***************************************************************************/
typedef enum PAL_SYS_TIMER16_MODE_tag
{
    TIMER16_CNTRL_ONESHOT  = 0,
    TIMER16_CNTRL_AUTOLOAD = 2
} PAL_SYS_TIMER16_MODE_T;



/****************************************************************************
 * Type:        PAL_SYS_TIMER16_CTRL_T
 ****************************************************************************
 * Description: This type defines start and stop values for the timer. 
 *              
 ***************************************************************************/
typedef enum PAL_SYS_TIMER16_CTRL_tag
{
    TIMER16_CTRL_STOP = 0,
    TIMER16_CTRL_START
} PAL_SYS_TIMER16_CTRL_T ;


void PAL_sysTimer16GetFreqRange(UINT32  refclk_freq,
                            UINT32 *p_max_usec,
                            UINT32 *p_min_usec);                                

INT32 PAL_sysTimer16SetParams(UINT32 base_address,
                         UINT32 refclk_freq,
                         PAL_SYS_TIMER16_MODE_T mode,
                         UINT32 usec);

void PAL_sysTimer16Ctrl(UINT32 base_address, PAL_SYS_TIMER16_CTRL_T status);

#endif /* __TIMER16_H__ */
