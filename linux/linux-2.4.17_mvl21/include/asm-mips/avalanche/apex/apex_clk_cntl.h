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

/*****************************************************************************
 * Clock Control
 *****************************************************************************/
#ifndef _APEX_CLK_CNTL_H_
#define _APEX_CLK_CNTL_H_

#define CLK_MHZ(x)    ( (x) * 1000000 )

/* The order of ENUMs here should not be altered since
 * the register addresses are derived from the order
 */

typedef enum PAL_SYS_CLKC_ID_tag   
{                                    
    CLKC_SYS = 0,                    
    CLKC_MIPS,                       
    CLKC_EPHY,
    CLKC_VBUS    
                    
} PAL_SYS_CLKC_ID_T;               

/**
 * This stucture is populated and passed on to the pal_sys_clkc_init function
 */
typedef struct PAL_SYS_Tnetv1020Init_tag
{
    UINT32 xtal1in;
    UINT32 xtal2in;
    UINT32 oscin;
} PAL_SYS_Tnetv1020Init;

#endif
