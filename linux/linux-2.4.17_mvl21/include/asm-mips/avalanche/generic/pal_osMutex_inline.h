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

/** \file   pal_osMutex_inline.h
    \brief  OsMUTEX Services Source File

    This file declares OS abstraction services for mutually exclusive
    locks or binary semaphores. All services run in the context of the
    calling thread or program. OsMUTEX does not spawn a thread of its own 
    to implement the APIs declared here.

    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_OSMUTEX_INLINE_H__
#define __PAL_OSMUTEX_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include "pal_osCfg.h"
#include <asm/semaphore.h>
#include <linux/slab.h>

/**
 * \defgroup PalOSMutex PAL OS Mutex Interface
 * 
 * PAL OS Mutex Interface
 * \{
 */

/** \name PAL OS Mutex Interface
 *  PAL OS Mutex Interface
 * \{
 */

/**
 * \brief   PAL OS Mutex Init
 * Nothing to initialize for Linux.
 */
PAL_INLINE PAL_Result PAL_osMutexInit(Ptr param)
{
	return PAL_SOK;
}

/**
 * \brief   PAL OS Mutex Create
 */
PAL_INLINE PAL_Result PAL_osMutexCreate(	const Char* name,
				PAL_OsMutexAttrs *attrs, 
				PAL_OsMutexHandle* hMutex)
{
    *hMutex = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    if(*hMutex == NULL) 
	{
        return PAL_OS_ERROR_NO_RESOURCES;
	}

    init_MUTEX((struct semaphore *)*hMutex);
    
    return PAL_SOK;
}

/**
 * \brief   PAL OS Mutex Delete
 * This does not care about any pending threads. The onus is 
 * on the user.
 */
PAL_INLINE PAL_Result PAL_osMutexDelete(PAL_OsMutexHandle hMutex)
{
    kfree(hMutex);
    return PAL_SOK;
}

/**
 * \brief   PAL OS Mutex Lock
 * The wait should always be infinite for Linux else error is returned.
 */
PAL_INLINE PAL_Result PAL_osMutexLock(PAL_OsMutexHandle hMutex, Int32 mSecTimeout)
{
	if(mSecTimeout != PAL_OSMUTEX_NO_TIMEOUT)
		return PAL_OS_ERROR_NOT_SUPPORTED;
	
    down_interruptible((struct semaphore*)hMutex);
    return PAL_SOK;
}

/**
 * \brief   PAL OS Mutex Unlock
 */
PAL_INLINE PAL_Result PAL_osMutexUnlock(PAL_OsMutexHandle hMutex)
{
    up((struct semaphore*)hMutex);
    return PAL_SOK;
}

/**
 * \brief   PAL OS Mutex Report
 * No reporting supported by the kernel.
 */
PAL_INLINE PAL_Result PAL_osMutexReport(PAL_OsMutexHandle hMutex, PAL_OsMutexReport *report, Char* buf)
{
    return PAL_OS_ERROR_NOT_SUPPORTED;
}
		
/*\}*/
/*\}*/
#endif

