/******************************************************************************
 * FILE PURPOSE:    CPMAC Linux Device Driver HAL support Header
 ******************************************************************************
 * FILE NAME:       cpmacHalVx.h
 *
 * DESCRIPTION:     CPMAC Linux Device Driver Header
 *
 * REVISION HISTORY:
 *
 * Date           Description                               Author
 *-----------------------------------------------------------------------------
 * 27 Nov 2002    Initial Creation                          Suraj S Iyer  
 * 09 Jun 2003    Updates for GA                            Suraj S Iyer
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __CPMAC_HAL_LX_H
#define __CPMAC_HAL_LX_H


typedef struct net_device         OS_DEVICE;
typedef struct sk_buff            OS_RECEIVEINFO;
typedef struct sk_buff            OS_SENDINFO;

#ifdef DEBUG
typedef void                      HAL_RECEIVEINFO;
typedef void                      HAL_DEVICE;
typedef void                      OS_SETUP;
#endif

#define OS_SETUP   void
#define HAL_DEVICE void
#define HAL_RECEIVEINFO void

#define _CPHAL_CPMAC
 
#include "cpswhal_cpmac.h"
#include "cpmac.h"

int cpmac_drv_start(CPMAC_DRV_HAL_INFO_T *, CPMAC_TX_CHAN_INFO_T*,
                    CPMAC_RX_CHAN_INFO_T *, unsigned int);
int cpmac_drv_cleanup(CPMAC_DRV_HAL_INFO_T *);
int cpmac_drv_init(CPMAC_DRV_HAL_INFO_T*);
int cpmac_drv_close(CPMAC_DRV_HAL_INFO_T*);
int cpmac_drv_open(CPMAC_DRV_HAL_INFO_T*);
int cpmac_drv_init_module(CPMAC_DRV_HAL_INFO_T*, OS_DEVICE*, int);
int cpmac_drv_stop(CPMAC_DRV_HAL_INFO_T *p_drv_hal,CPMAC_TX_CHAN_INFO_T *p_tx_chan_info,
                   CPMAC_RX_CHAN_INFO_T *p_rx_chan_info,unsigned int flags);

#endif
