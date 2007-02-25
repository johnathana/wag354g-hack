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

/** \file   ddc_cpmacCfg.h
    \brief  DDC CPMAC Configuration header file

    This file has the static (compile time) configuration parameters for the
    CPMAC DDC driver.

    (C) Copyright 2003, Texas Instruments, Inc

    @author     Anant Gole
    @version    0.1
 */

#ifndef __DDC_CPMAC_CFG_H__
#define __DDC_CPMAC_CFG_H__


/* Feature macros here */

/* If multi packet Tx complete notifications is enabled (via CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY), 
   Max number of Tx packets that can be notified - the actual number will depend upon 
   user configuration for parameter "maxPktsToProcess" */
#define CPMAC_DDC_MAX_TX_COMPLETE_PKTS_TO_NOTIFY    64

/* If multi packet Rx indication is enabled (via CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY)
   Max number of Rx packets that can be notified - the actual number will depend upon 
   user configuration for parameter "maxPktsToProcess" */
#define CPMAC_DDC_MAX_RX_COMPLETE_PKTS_TO_NOTIFY    64

/* Config macros */
#define CPMAC_MAX_INSTANCES                     6       /**< Max CPMAC instances */
#define CPMAC_MIN_ETHERNET_PKT_SIZE             60      /**< Minimum Ethernet packet size */

/* Max RX fragments calculation - 1500 byte packet and 64 byte buffer. Fragments=1500/64=24 */
#define CPMAC_MAX_RX_FRAGMENTS                  24      /**< Maximum RX fragments supported */

/* Theoratically TX max fragments are equal to 24 */
#define CPMAC_MAX_TX_FRAGMENTS                  8       /**< Maximum TX fragments supported */

/* CPMAC hardware specific */
#define CPMAC_RESET_CLOCKS_WAIT                 64      /**< Clocks to wait for reset operation */
#define CPMAC_MAX_TX_CHANNELS                   8       /**< Maximum TX Channels supported by the DDC */
#define CPMAC_MAX_RX_CHANNELS                   8       /**< Maximum RX Channels supported by the DDC */
#define CPMAC_MIN_FREQUENCY_FOR_10MBPS          5500000     /**< Minimum CPMAC bus frequency for 10   Mbps operation is 5.5 MHz (as per specs) */
#define CPMAC_MIN_FREQUENCY_FOR_100MBPS         55000000    /**< Minimum CPMAC bus frequency for 100  Mbps operation is 55  MHz (as per specs) */
#define CPMAC_MIN_FREQUENCY_FOR_1000MBPS        125000000   /**< Minimum CPMAC bus frequency for 1000 Mbps operation is 125 MHz (as per specs) */

/* Macros for Address conversions */
#ifdef CPMAC_DDC_MIPS_OPTIMIZED     /* \note: These need to be ported for a different platform other than MIPS */
#define PAL_CPMAC_VIRT_2_PHYS(addr)             (((Uint)(addr)) &~ 0xE0000000)
#define PAL_CPMAC_VIRT_NOCACHE(addr)            ((Ptr)((PAL_CPMAC_VIRT_2_PHYS(addr)) | 0xA0000000))
#else   /* Use PAL_sysXXX API's */
#define PAL_CPMAC_VIRT_2_PHYS(addr)             PAL_osMemVirt2Phy((Ptr)(addr))
#define PAL_CPMAC_VIRT_NOCACHE(addr)            PAL_osMemVirt2VirtNoCache((Ptr)(addr))
#endif

#ifndef CPMAC_DDC_USE_ASSEMBLY        /* If no assembly in DDC - access cache functions via PAL SYS layer */

#define PAL_CPMAC_CACHE_INVALIDATE(addr, size)  PAL_sysCacheInvalidate(PAL_OSMEM_ADDR_DAT, (Ptr)(addr), (Uint32)(size))

#ifdef CPMAC_CACHE_WRITEBACK_MODE
#define PAL_CPMAC_CACHE_WRITEBACK(addr, size)               PAL_sysCacheFlush(PAL_OSMEM_ADDR_DAT, (Ptr)(addr), (Uint32)(size))
#define PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(addr, size)    PAL_sysCacheFlush(PAL_OSMEM_ADDR_DAT, (Ptr)(addr), (Uint32)(size))
#else
#define PAL_CPMAC_CACHE_WRITEBACK               
#define PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE    
#endif

#else

#ifdef CPMAC_DDC_MIPS_OPTIMIZED
/* If assembly allowed in DDC - use the cache instruction 
 * \note: Currently we are supporting only the MIPS cache instruction. Any variants have to be taken
 * care of here.
 * ALSO NOTE THAT THE ASSEMBLY VERSION BELOW ASSUMES THAT ONLY 16 BYTES (OR THE SIZE OF ONE CACHE LINE)
 * IS BEING WRITTEN BACK. If DDC code changes in future this macro implementation has to take care of it.
 */
#define PAL_CPMAC_CACHE_INVALIDATE(addr, size)  __asm__(" cache  17, (%0)" : : "r" (addr))

#ifdef CPMAC_CACHE_WRITEBACK_MODE
#define PAL_CPMAC_CACHE_WRITEBACK(addr, size)               __asm__(" cache  25, (%0)" : : "r" (addr))
#define PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(addr, size)    __asm__(" cache  21, (%0)" : : "r" (addr))
#else
#define PAL_CPMAC_CACHE_WRITEBACK               
#define PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE    
#endif
#else
#error "Assembly code for any architecture other than MIPS not supported as yet"

#endif /* CPMAC_DDC_MIPS_OPTIMIZED */
#endif /* CPMAC_DDC_USE_ASSEMBLY */

/* Define LOCAL as static - within file */
#ifndef LOCAL
#define LOCAL   static
#endif

#endif /* __DDC_CPMAC_CFG_H__ */
