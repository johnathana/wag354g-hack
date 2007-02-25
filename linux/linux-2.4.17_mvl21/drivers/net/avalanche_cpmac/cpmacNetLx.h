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

/** \file   cpmacNetLx.h
    \brief  CPMAC Linux DDA Header file

    This file is a header for  the device driver adaptation for Linux for 
    CPMAC/CPGMAC device based upon PSP Framework architecture.

    Acknowledgements: This DDA implementation for CP(G)MAC device is based upon
    Linux device driver for CP(G)MAC written using HAL 2.0.

    (C) Copyright 2004, Texas Instruments, Inc

    @author     Suraj Iyer - Original Linux Driver for CPMAC
                Anant Gole - Updated as per PSPF architecture (converted to DDA)
    @version    1.1
                Sharath Kumar - Incorporated Review comments.
    @version    1.2


    
 */

#ifndef _CPMAC_LX_DDA_H_
#define _CPMAC_LX_DDA_H_

#include "ddc_cpmac.h"
#include "cpmacNetLxCfg.h"

extern int cpmac_debug_mode;

#define dbgPrint if (cpmac_debug_mode) printk
#define errPrint printk

/* Misc Error codes */
#define CPMAC_DDA_INTERNAL_FAILURE  -1

/* LED codes required for controlling LED's */
#define CPMAC_LINK_OFF          0
#define CPMAC_LINK_ON           1
#define CPMAC_SPEED_100         2
#define CPMAC_SPEED_10          3
#define CPMAC_FULL_DPLX         4
#define CPMAC_HALF_DPLX         5
#define CPMAC_TX_ACTIVITY       6
#define CPMAC_RX_ACTIVITY       7

/**
 * \brief  CPMAC (DDA) Private Ioctl Structure  
 *
 * Private Ioctl commands provided by the CPMAC Linux Driver use this structure
 */
typedef struct
{
    unsigned int    cmd;    /**< Command */
    void            *data;  /**< Data provided with the command - depending upon command */
} CpmacDrvPrivIoctl;

/**
 * \brief  CPMAC DDA maintained statistics 
 *
 * Driver maintained statistics (apart from Hardware statistics)
 */
typedef struct
{
    unsigned long tx_discards;      /**< TX Discards */
    unsigned long rx_discards;      /**< RX Discards */
    unsigned long start_tick;       /**< Start tick */
} CpmacDrvStats;

/**
 * \brief CPMAC Private data structure
 * 
 * Each CPMAC device maintains its own private data structure and has a pointer 
 * to the net_device data structure representing the instance with the kernel. 
 * The private data structure contains a "owner" member pointing to the net_device
 * structure and the net_device data structure's "priv" member points back to this
 * data structure.
 */
typedef struct
{
    void                *owner;             /**< Pointer to the net_device structure */
    unsigned int        instanceNum;        /**< Instance Number of the device */
#ifdef CPMAC_USE_TASKLET_MODE
    struct tasklet_struct tasklet;          /**< Tasklet structure if processing packets in tasklets */
#endif
    struct net_device   *nextDevice;        /**< Next device pointer - for internal use */
    unsigned int        linkSpeed;          /**< Link Speed */
    unsigned int        linkMode;           /**< Link Mode */
    int                 setToClose;         /**< Flag to indicate closing of device */
    void*               ledHandle;          /**< Handle for LED control */

    /* DDC related parameters */
    CpmacDDCObj         *hDDC;              /**< Handle (pointer) to Cpmac DDC object */
    CpmacDDCIf          *ddcIf;             /**< Handle (pointer) to Cpmac DDC function table */
    CpmacDDCStatus      ddcStatus;          /**< Cpmac DDC data structure */

    /* Configuration parameters */
    char                macAddr[6];        /**< Mac (ethernet) address */
    CpmacInitConfig     initCfg;            /**< Init cfg parameters - contains rx and mac cfg structures */
    unsigned int        rxBufSize;          /**< RX Buffer Size - skb size to be allocated for RX */
    unsigned int        rxBufOffset;        /**> RX Buffer Offset - extra bytes to be reserved before RX data begins in a skb */
    /* TODO: VLAN TX not supported as of now */
    BOOL                vlanEnable;         /**< Vlan enable (TX: 8 priority ch's, RX: 1514 byte frames accepted) */
    /* Channel configuration - though only 1 TX/RX channel is supported, provision is made for max */
    CpmacChInfo         txChInfo[CPMAC_MAX_TX_CHANNELS];    /**< Tx Channel configuration */
    CpmacChInfo         rxChInfo[CPMAC_MAX_RX_CHANNELS];    /**< Rx Channel configuration */

    /* Periodic Timer required for DDC (MDIO) polling */
    struct timer_list   periodicTimer;      /**< Periodic timer required for DDC (MDIO) polling */
    Uint32              periodicTicks;      /**< Ticks for this timer */
    BOOL                timerActive;        /**< Periodic timer active ??? */

    struct timer_list   mibTimer;      /**< Periodic timer required for 64 bit MIB counter support */
    Uint32                mibTicks;      /**< Ticks for this timer */
    BOOL                mibTimerActive;        /**< Periodic timer active ??? */


    /* Statistics */
    CpmacHwStatistics       deviceMib;      /**< Device MIB - CPMAC hardware statistics counters */
    CpmacDrvStats           deviceStats;    /**< Device Statstics */
    struct net_device_stats netDevStats;    /**< Linux Network Device statistics */ 
    
    /* Statistics counters for debugging */
    Uint32              isrCount;           /**< Number of interrupts */

} CpmacNetDevice;

/* Function prototypes - only those functions that are in different C files and need
 * to be referenced from the other sources\
 */
int  cpmac_dev_tx( struct sk_buff *skb, struct net_device *p_dev);
void cpmac_hal_isr(int irq, void *dev_id, struct pt_regs *p_cb_param);
#ifdef CPMAC_USE_TASKLET_MODE
void cpmac_handle_tasklet(unsigned long data);
#endif
Ptr DDA_cpmac_net_alloc_rx_buf(CpmacNetDevice* hDDA, Int bufSize, 
                DDC_NetDataToken *dataToken, Uint32 channel, Ptr allocArgs);
PAL_Result DDA_cpmac_net_free_rx_buf(CpmacNetDevice* hDDA, Ptr buffer, 
                DDC_NetDataToken dataToken, Uint32 channel, Ptr freeArgs);
PAL_Result DDA_cpmac_net_rx(CpmacNetDevice* hDDA, DDC_NetPktObj *pkt, Ptr rxArgs, Ptr arg);
PAL_Result DDA_cpmac_net_tx_complete(CpmacNetDevice* hDDA, DDC_NetDataToken *netDataTokens,
                Int numTokens, Uint32 channel);
#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
PAL_Result DDA_cpmac_net_rx_multiple_cb(CpmacNetDevice* hDDA, DDC_NetPktObj *netPktList, Int numPkts, Ptr rxArgs);
#endif

#endif /* _CPMAC_LX_DDA_H_ */



