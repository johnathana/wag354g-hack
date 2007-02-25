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

/** \file   ddc_cpmac_ioctl.h
    \brief  DDC CPMAC Ioctl header file

    This file provides data structures that are required by the Ioctls. The 
    application or the driver can include this file to use the Ioctl's

    To use this file, _titypedefs.h needs to be also included
    (C) Copyright 2004, Texas Instruments, Inc

    @author     Anant Gole
    @version    0.1
 */

#ifndef __DDC_CPMAC_IOCTL_H__
#define __DDC_CPMAC_IOCTL_H__

/**
 *  \brief CPMAC Single Multicast Ioctl - CPMAC_DDC_IOCTL_MULTICAST_ADDR operations
 *
 *  Add/Del operations for adding/deleting a single multicast address
 */
typedef enum
{
    CPMAC_MULTICAST_ADD = 0,    /**< Add a single multicast address to the hardware mcast list */
    CPMAC_MULTICAST_DEL         /**< Delete a single multicast address from the hardware mcast list */
} CpmacSingleMultiOper;


/**
 *  \brief CPMAC All Multicast Ioctl - CPMAC_DDC_IOCTL_ALL_MULTI operations
 *
 *  Set/Clear all multicast operation
 */
typedef enum
{
    CPMAC_ALL_MULTI_SET = 0,
    CPMAC_ALL_MULTI_CLR
} CpmacAllMultiOper;


/**
 * \brief MII Read/Write PHY register
 *
 * Parameters to read/write a PHY register via MII interface
 *
 */
typedef struct
{
    Uint32      phyNum;             /**< Phy number to be read/written */
    Uint32      regAddr;            /**< Register to be read/written */
    Uint32      data;               /**< Data to be read/written */
} CpmacPhyParams;

/**
 * \brief MAC  Address params
 *
 * Parameters for Configuring Mac address
 *
 */
typedef struct
{
    Uint32      channel;            /**< Channel number to which this address params apply */
    String      macAddress;         /**< Mac address  */
} CpmacAddressParams;



/**
 * \brief Type 2/3 Addressing
 *
 * Parameters for programming CFIG 2/3 addressing mode
 *
 */
typedef struct
{
    Uint32      channel;            /**< Channel number to which this filtering params apply */
    String      macAddress;         /**< Mac address for filtering */
    Int         index;              /**< Index of filtering list to update */
    Bool        valid;              /**< Entry Valid */
    Int         match;              /**< Entry Matching  */
} CpmacType2_3_AddrFilterParams;


/**
 * \brief CPMAC Hardware Statistics
 *
 * Statistics counters provided by CPMAC Hardware. The names of the counters in this 
 * structure are of "MIB style" and corrospond directly to the hardware counters 
 * provided by CPMAC
 */
typedef struct
{
    Uint32   ifInGoodFrames;
    Uint32   ifInBroadcasts;
    Uint32   ifInMulticasts;
    Uint32   ifInPauseFrames;
    Uint32   ifInCRCErrors;
    Uint32   ifInAlignCodeErrors;
    Uint32   ifInOversizedFrames;
    Uint32   ifInJabberFrames;
    Uint32   ifInUndersizedFrames;
    Uint32   ifInFragments;
    Uint32   ifInFilteredFrames;
    Uint32   ifInQosFilteredFrames;
    Uint32   ifInOctets;
    Uint32   ifOutGoodFrames;
    Uint32   ifOutBroadcasts;
    Uint32   ifOutMulticasts;
    Uint32   ifOutPauseFrames;
    Uint32   ifDeferredTransmissions;
    Uint32   ifCollisionFrames;
    Uint32   ifSingleCollisionFrames;
    Uint32   ifMultipleCollisionFrames;
    Uint32   ifExcessiveCollisionFrames;
    Uint32   ifLateCollisions;
    Uint32   ifOutUnderrun;
    Uint32   ifCarrierSenseErrors;
    Uint32   ifOutOctets;
    Uint32   if64OctetFrames;
    Uint32   if65To127OctetFrames;
    Uint32   if128To255OctetFrames;
    Uint32   if256To511OctetFrames;
    Uint32   if512To1023OctetFrames;
    Uint32   if1024ToUPOctetFrames;
    Uint32   ifNetOctets;
    Uint32   ifRxSofOverruns;
    Uint32   ifRxMofOverruns;
    Uint32   ifRxDMAOverruns;
} CpmacHwStatistics;

#endif /* __DDC_CPMAC_IOCTL_H__ */
