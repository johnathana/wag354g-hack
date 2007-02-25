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

/** \file   pal_osCache.h
    \brief  OsCACHE Services Header File
    
    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_OSCACHE_INLINE_H__
#define __PAL_OSCACHE_INLINE_H__

#include "pal_defs.h"
#include "pal_os.h"
#include <asm/io.h>

/**
 * \defgroup PalOSCache PAL OS Cache Interface
 * 
 * PAL OS Cache Interface
 * \{
 */

/** \name PAL OS Cache Interface
 *  PAL OS Cache Interface
 * \{
 */

/**
 * \brief   PAL OS Cache Invalidate
 * 
 *      This function invalidates the cache region. 
 * \param   type is cache type viz. data or instruction cache.
 * \param   start is start address of the memory region.
 * \param   size is size of memory region 
 * \return  PAL_Result
 */
PAL_INLINE PAL_Result PAL_osCacheInvalidate(PAL_OsMemAddrSpace type, Uint32 start, Uint32 size)
{
	Uint i;
    Uint32 memEndAddr = size + (start & 0xf);				
	if(type == PAL_OSMEM_ADDR_DAT)
	{
		dma_cache_inv(start, size);
	}
	else if(type == PAL_OSMEM_ADDR_PRG)
	{
        for(i=0; i<memEndAddr; i+=16, start+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 16, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (start));
        }			
	} 
	else
		return PAL_OS_ERROR_INVALID_PARAM;
			
	return PAL_SOK;	
}

/**
 * \brief   PAL OS Cache Flush
 * 
 *      This function flushes the cache content to the memory.
 * \param   type is cache type viz. data or instruction cache.
 * \param   start is start address of the memory region.
 * \param   size is size of memory region 
 * \return  PAL_Result
 */
PAL_INLINE PAL_Result PAL_osCacheFlush(PAL_OsMemAddrSpace type, Uint32 start, Uint32 size)
{
	Uint i;		
    Uint32 memEndAddr = size + (start & 0xf);		
	if(type == PAL_OSMEM_ADDR_DAT)
	{
		dma_cache_wback_inv(start, size);
	}
	else if(type == PAL_OSMEM_ADDR_PRG)
	{
		for(i=0; i<memEndAddr; i+=16, start+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 24, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (start));
        }        
	}
	else
	{
		return PAL_OS_ERROR_INVALID_PARAM;
	}
	return PAL_SOK;
}

/*\}*/
/*\}*/

#endif /* __PAL_OSCACHE_INLINE_H__ */
