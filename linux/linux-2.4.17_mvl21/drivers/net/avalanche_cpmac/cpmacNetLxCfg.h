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

/** \file   cpmacNetLxCfg.h
    \brief  CPMAC Linux DDA Configuration Header file

    This file is a configuration header for the device driver adaptation for 
    Linux for CPMAC/CPGMAC device based upon PSP Framework architecture.

    Acknowledgements: This DDA implementation for CP(G)MAC device is based upon
    Linux device driver for CP(G)MAC written using HAL 2.0.

    (C) Copyright 2004, Texas Instruments, Inc

    @author     Anant Gole
    @version    0.1

            (ver 0.2)   Sharath Kumar - Incorporated review comments
    @version    0.2
    
 */

#ifndef _CPMAC_LX_DDA_CFG_H_
#define _CPMAC_LX_DDA_CFG_H_

//#ifdef CONFIG_MIPS_AVALANCHE_MARVELL_6063
#define EGRESS_TRAILOR_LEN                  4
//#else
//#define EGRESS_TRAILOR_LEN                  0
//#endif

#ifdef CONFIG_MIPS_AR7WRD
#define CFG_START_LINK_SPEED                (SNWAY_NOPHY)
#else
#define CFG_START_LINK_SPEED                (SNWAY_AUTOALL) /* auto nego */
#endif

/* Defaut Configuration values required for passing on to DDC */
#define CPMAC_DEFAULT_MLINK_MASK                        0
#define CPMAC_DEFAULT_PASS_CRC                          FALSE
#define CPMAC_DEFAULT_QOS_ENABLE                        FALSE
#define CPMAC_DEFAULT_NO_BUFFER_CHAINING                FALSE
#define CPMAC_DEFAULT_COPY_MAC_CONTROL_FRAMES_ENABLE    FALSE
#define CPMAC_DEFAULT_COPY_SHORT_FRAMES_ENABLE          FALSE
#define CPMAC_DEFAULT_COPY_ERROR_FRAMES_ENABLE          FALSE
#define CPMAC_DEFAULT_PROMISCOUS_CHANNEL                0
#define CPMAC_DEFAULT_BROADCAST_CHANNEL                 0
#define CPMAC_DEFAULT_MULTICAST_CHANNEL                 0
#define CPMAC_DEFAULT_BUFFER_OFFSET                     0
#define CPMAC_DEFAULT_TX_PRIO_TYPE                      CPMAC_TXPRIO_FIXED
#define CPMAC_DEFAULT_TX_SHORT_GAP_ENABLE               FALSE
#define CPMAC_DEFAULT_TX_PACING_ENABLE                  FALSE
#define CPMAC_DEFAULT_MII_ENABLE                        TRUE
#define CPMAC_DEFAULT_TX_FLOW_ENABLE                    FALSE
#define CPMAC_DEFAULT_RX_FLOW_ENABLE                    FALSE
#define CPMAC_DEFAULT_LOOPBACK_ENABLE                   FALSE
#define CPMAC_DEFAULT_FULL_DUPLEX_ENABLE                TRUE
#define CPMAC_DEFAULT_TX_INTERRUPT_DISABLE              TRUE
#define CONFIG_CPMAC_MIB_TIMER_TIMEOUT                  5000 /* 5 seconds should be enough */

#define CPMAC_DEFAULT_PROMISCOUS_ENABLE                 0
#define CPMAC_DEFAULT_BROADCAST_ENABLE                  1
#define CPMAC_DEFAULT_MULTICAST_ENABLE                  1

/* Linux invalidate function. This macro is provided for easy configurability */
#define CPMAC_DDA_CACHE_INVALIDATE(addr, size)      dma_cache_inv((unsigned long)(addr), (size));    

/* Linux writeback function. This macro is provided for easy configurability */
#define CPMAC_DDA_CACHE_WRITEBACK(addr, size)       dma_cache_wback_inv((unsigned long)skb->data, skb->len);

/* NOT EXPLICIT SUPPORT PROVIDED AS OF NOW - Vlan support in the driver */
#define CPMAC_DDA_DEFAULT_VLAN_ENABLE       FALSE

/* System value for ticks per seconds */
#define CPMAC_DDA_TICKS_PER_SEC             HZ 

/* CPMAC new Ioctl's created - using a value with a base that can be adjusted as per needs */
#define CPMAC_DDA_IOCTL_BASE                0

/* Filtering IOCTL */
#define CPMAC_DDA_PRIV_FILTERING            (CPMAC_DDA_IOCTL_BASE + 1)

/* Read/Write MII */
#define CPMAC_DDA_PRIV_MII_READ             (CPMAC_DDA_IOCTL_BASE + 2)
#define CPMAC_DDA_PRIV_MII_WRITE            (CPMAC_DDA_IOCTL_BASE + 3)

/* Get/Clear Statistics */
#define CPMAC_DDA_PRIV_GET_STATS            (CPMAC_DDA_IOCTL_BASE + 4)
#define CPMAC_DDA_PRIV_CLR_STATS            (CPMAC_DDA_IOCTL_BASE + 5)

/* External Switch configuration */
#define CPMAC_DDA_EXTERNAL_SWITCH           (CPMAC_DDA_IOCTL_BASE + 6)

/* Default max frame size = 1522 = 1500 byte data + 14 byte eth header + 4 byte checksum + 4 byte Vlan tag  + Feature */
#define CPMAC_DDA_DEFAULT_MAX_FRAME_SIZE    (1500 + 14 + 4 + 4 + EGRESS_TRAILOR_LEN)

/* Default extra bytes allocated for each RX buffer */
#define CPMAC_DDA_DEFAULT_EXTRA_RXBUF_SIZE  0

/* Default number of TX channels */
#define CPMAC_DDA_DEFAULT_NUM_TX_CHANNELS   1

/* Default TX channel number */
#define CPMAC_DDA_DEFAULT_TX_CHANNEL        0

/* Default TX number of BD's/Buffers */
#define CPMAC_DDA_DEFAULT_TX_NUM_BD         64

/* Default TX max service BD's */
#define CPMAC_DDA_DEFAULT_TX_MAX_SERVICE    20

/* Default number of RX channels */
#define CPMAC_DDA_DEFAULT_NUM_RX_CHANNELS   1

/* Default RX channel number */
#define CPMAC_DDA_DEFAULT_RX_CHANNEL        0

/* Default RX number of BD's/Buffers */
#if defined (CONFIG_MIPS_AR7RD)
#define CPMAC_DDA_DEFAULT_RX_NUM_BD         16
#else
#define CPMAC_DDA_DEFAULT_RX_NUM_BD         64
#endif


/* Default RX max service BD's */
#define CPMAC_DDA_DEFAULT_RX_MAX_SERVICE    8

/* 
 * Size of CPMAC peripheral footprint in memory that needs to be reserved in Linux 
 * Note that this value is actually a hardware memory footprint value taken from the specs
 * and ideally should have been in the csl files. Keeping it for convinience since CPMAC
 * peripheral footprint will not change unless the peripheral itself changes drastically 
 * and it will be called with a different name and will have a different driver anyway
 */
#define CPMAC_DDA_DEFAULT_CPMAC_SIZE        0x800

/* ENV variable names for obtaining MAC Addresses */
#define CPMAC_DDA_MAC_ADDR_A    "maca"
#define CPMAC_DDA_MAC_ADDR_B    "macb"
#define CPMAC_DDA_MAC_ADDR_C    "macc"
#define CPMAC_DDA_MAC_ADDR_D    "macd"
#define CPMAC_DDA_MAC_ADDR_E    "mace"
#define CPMAC_DDA_MAC_ADDR_F    "macf"

/* ENV variable names for obtaining config string when not using config service */
#define CPMAC_DDA_CONFIG_A      "MACCFG_A"
#define CPMAC_DDA_CONFIG_B      "MACCFG_B"
#define CPMAC_DDA_CONFIG_C      "MACCFG_C"
#define CPMAC_DDA_CONFIG_D      "MACCFG_D"
#define CPMAC_DDA_CONFIG_E      "MACCFG_E"
#define CPMAC_DDA_CONFIG_F      "MACCFG_F"

/* Maximum multicast addresses list to be handled by the driver - If this is not restricted
 * then the driver will spend considerable time in handling multicast lists 
 */
#define CPMAC_DDA_DEFAULT_MAX_MULTICAST_ADDRESSES   64

#endif /* _CPMAC_LX_DDA_CFG_H_ */

