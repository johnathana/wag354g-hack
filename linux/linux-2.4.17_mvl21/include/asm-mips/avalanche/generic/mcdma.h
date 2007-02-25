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
 * FILE PURPOSE:    Multi Channel Direct Memory Access (MC-DMA) Header
 ********************************************************************************
 * FILE NAME:       mcdma.h
 *
 * DESCRIPTION:     Platform and OS independent API for MCDMA Controller
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __MCDMA_H__
#define __MCDMA_H__


typedef enum MCDMA_CHANNEL_tag
{
    MCDMA_CHANNEL_0 = 0,
    MCDMA_CHANNEL_1,
    MCDMA_CHANNEL_2,
    MCDMA_CHANNEL_3,
    
} MCDMA_CHANNEL_T;

typedef enum MCDMA_CTRL_tag
{
    MCDMA_STOP = 0,
    MCDMA_START
    
} MCDMA_CTRL_T;

typedef enum MCDMA_ADDR_MODE_tag
{
    MCDMA_INCREMENTING = 0,
    MCDMA_FIXED = 2
    
} MCDMA_ADDR_MODE_T;

typedef enum MCDMA_BURST_MODE_tag
{
    MCDMA_1_WORD_BURST = 0,
    MCDMA_2_WORD_BURST = 1,
    MCDMA_4_WORD_BURST = 2,
    
} MCDMA_BURST_MODE_T;


void mcdma_init(UINT32 base_addr);
void mcdma_control(MCDMA_CHANNEL_T mcdma_ch, MCDMA_CTRL_T mcdma_ctrl);
INT32 mcdma_setdmaparams (MCDMA_CHANNEL_T mcdma_ch,
                          UINT32 src_addr, 
                          UINT32 dst_addr, 
                          UINT32 length, 
                          MCDMA_BURST_MODE_T burst_mode, 
                          MCDMA_ADDR_MODE_T src_addr_mode,
                          MCDMA_ADDR_MODE_T dst_addr_mode);

#endif /* __MCDMA_H__ */
