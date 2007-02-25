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

/** \file   pal_osMem_inline.h
    \brief  OsMEM Services Source File

    This file implements the OsMEM services for Linux.

    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version   0.1
*/

#ifndef __PAL_OSMEM_INLINE_H__
#define __PAL_OSMEM_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include <asm/page.h>
#include <linux/slab.h>
#include <linux/mm.h>

/**
 * \brief PAL OS MEM Init
 */
PAL_INLINE PAL_Result PAL_osMemInit (Ptr param)
{
    return PAL_SOK;
}

/**
 * \brief PAL OS Memory Segment Define
 * For Linux, the whole available memory is considered into one segment.
 * No further segments are made in the memory. This function designates that
 * segment with id 0.
 */
PAL_INLINE PAL_Result PAL_osMemSegDefine (    const char * name,
                                Uint32 startAddr,
                                Uint32 numBytes,
                                PAL_OsMemAttrs * attrs,
                                Uint32 *segId)
{
    /* always return the segment id 0.
     * Only one segment is defined and used
     */
    *segId = 0;
    return PAL_SOK;
}

/**
 * \brief PAL OS Memory Segment Undefine
 * Only segment 0 is recognized.
 */
PAL_INLINE PAL_Result PAL_osMemSegUndefine (Uint32 segId)
{
    return PAL_SOK;
}

/**
 * \brief PAL OS Memory Alloc
 * Only segment 0 is recognized.
 * This function allocates only contiguous memory.
 * Alignment is not honored since Linux does not provide any 
 * facility for memory alignment from kmalloc. Only a 4 Byte alignement
 * can be expected for 32 bit CPUs and 8 byte for 64 Bit ones.
 * If more alignment is required, it should come from outside by rounding off
 * the memAddr and remembering the original for the call to PAL_osMemFree.
 */
PAL_INLINE PAL_Result PAL_osMemAlloc (
                Uint32 segId, 
                Uint32 numBytes, 
                Uint16 alignment, 
                Ptr* memAddr)
{
    *memAddr = kmalloc(numBytes, GFP_KERNEL);
	
     if(*memAddr == NULL)
	 {
        return PAL_OS_ERROR_NO_RESOURCES; 
	 }
    
     return PAL_SOK;    
}

/**
 * \brief PAL OS Memory Free
 * Only segment 0 is recognized.
 */            
PAL_INLINE PAL_Result PAL_osMemFree (Uint32 segId, Ptr memAddr, Uint32 numBytes)
{
    kfree(memAddr);
    return PAL_SOK;    
}

/**
 * \brief PAL OS Memory Copy
 * \note This will misbehave if presented with invalid arguments.
 */
PAL_INLINE PAL_Result PAL_osMemCopy (Ptr dest, const Ptr src, Uint32 numBytes)
{
    memcpy(dest, src, numBytes);
    return PAL_SOK;    
}

/**
 * \brief PAL OS Memory Set
 * This will crash if presented with invalid arguments.
 */
PAL_INLINE PAL_Result PAL_osMemSet (Ptr memAddr, Char fillVal, Uint32 numBytes)
{
    memset(memAddr, fillVal, numBytes);
    return PAL_SOK;
}

/**
 * \brief PAL OS Memory Lock
 * In linux, memory locking/Unlocking is supported only at page granularity.
 * This implementation, locks all the pages from memAddr to memAddr + byteLen
 * pages containing both addresses inclusive.
 */ 
PAL_INLINE PAL_Result PAL_osMemLock (Ptr memAddr, Uint32 byteLen, Uint32 *cookie)
{
    Uint32 temp;
    
    /*
     * Get the page associated with the memory address 
     * and set the reserved bit for that page 
     */
    for (temp = (Uint32)memAddr; temp < PAGE_ALIGN((Uint32)memAddr+byteLen); temp += PAGE_SIZE) 
	{
        SetPageReserved(virt_to_page(temp));
    }
    return PAL_SOK;    
}

/**
 * \brief PAL OS Memory Un-Lock
 * In linux, memory locking/Unlocking is supported only at page granularity.
 * This implementation, locks all the pages from memAddr to memAddr + byteLen
 * pages containing both addresses inclusive.
 */             
PAL_INLINE PAL_Result PAL_osMemUnlock (Ptr memAddr, Uint32 byteLen, Uint32 *cookie)
{
    Uint32 temp;
    
    /*
     * Get the page associated with the memory address 
     * and unset the reserved bit for that page 
     */
    for (temp = (Uint32)memAddr; temp < PAGE_ALIGN((Uint32)memAddr+byteLen); temp += PAGE_SIZE) 
	{
        ClearPageReserved(virt_to_page(temp));
    }
    return PAL_SOK;    
}

/**
 * \brief PAL OS Memory Virtual To Physical
 */ 
PAL_INLINE Uint32 PAL_osMemVirt2Phy (Ptr virtAddress)
{
    return PHYSADDR(virtAddress); 
}

/**
 * \brief PAL OS Memory Physical To Virtual
 */             
PAL_INLINE Ptr PAL_osMemPhy2Virt (Uint32 phyAddress)  
{
    return __va(phyAddress);
}

/**
 * \brief PAL OS Memory Report
 * No reporting is supported inherently by the Linux kernel.
 */ 
PAL_INLINE PAL_Result PAL_osMemReport (Uint32 segId, PAL_OsMemReport * report, Char *buf)
{
    return PAL_OS_ERROR_NOT_SUPPORTED;
}

#endif

