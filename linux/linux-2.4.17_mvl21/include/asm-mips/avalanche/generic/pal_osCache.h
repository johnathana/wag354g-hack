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

#ifndef __PAL_OSCACHE_H__
#define __PAL_OSCACHE_H__

#include "pal_defs.h"
#include "pal_osMem.h"

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
PAL_INLINE PAL_Result PAL_osCacheInvalidate(PAL_OsMemAddrSpace type, Uint32 start, Uint32 size);

/**
 * \brief   PAL OS Cache Flush
 * 
 *      This function flushes the cache content to the memory.
 * \param   type is cache type viz. data or instruction cache.
 * \param   start is start address of the memory region.
 * \param   size is size of memory region 
 * \return  PAL_Result
 */
PAL_INLINE PAL_Result PAL_osCacheFlush(PAL_OsMemAddrSpace type, Uint32 start, Uint32 size);

/*\}*/
/*\}*/

#endif /* _PAL_OSCACHE_H_ */
