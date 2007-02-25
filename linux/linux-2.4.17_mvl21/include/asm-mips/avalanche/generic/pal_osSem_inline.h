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

/** \file   pal_osSem_inline.h
    \brief  OsSEM Services Source File

    
    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_OSSEM_INLINE_H__
#define __PAL_OSSEM_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include "pal_osCfg.h"
#include <asm/semaphore.h>
#include <linux/slab.h>

/**
 * \defgroup PalOSSem PAL OS Semaphore Interface
 * 
 * PAL OS Semaphore Interface
 * @{
 */

/** \name PAL OS Semaphore Interface
 *  PAL OS Semaphore Interface
 * @{
 */

/**
 * \brief   PAL OS Semaphore Init
 * Nothing to init for Linux
 */
PAL_INLINE PAL_Result PAL_osSemInit(Ptr param)
{
    return PAL_SOK;
}

/**
 * \brief   PAL OS Semaphore Create
 */
PAL_INLINE PAL_Result PAL_osSemCreate(const char* name,
                                  Int32 initVal,
                                  PAL_OsSemAttrs *attrs, 
                                  PAL_OsSemHandle* hSem)
{
    if ((*hSem = kmalloc(sizeof(struct semaphore), GFP_KERNEL)) == NULL) 
	{
        return PAL_OS_ERROR_NO_RESOURCES;
	}

    sema_init((struct semaphore*)*hSem, initVal); 
    return PAL_SOK;
}

/**
 * \brief   PAL OS Semaphore Delete
 */
PAL_INLINE PAL_Result PAL_osSemDelete(PAL_OsSemHandle hSem)
{
    kfree(hSem);
    return PAL_SOK;    
}

/**
 * \brief   PAL OS Semaphore Take
 */

PAL_INLINE PAL_Result PAL_osSemTake(PAL_OsSemHandle hSem, Int32 mSecTimeout)
{
	if (mSecTimeout != PAL_OSSEM_NO_TIMEOUT)
		return PAL_OS_ERROR_NOT_SUPPORTED;
				
    down_interruptible((struct semaphore*)hSem);
    return PAL_SOK;
}

/**
 * \brief   PAL OS Semaphore Give
 */
PAL_INLINE PAL_Result PAL_osSemGive(PAL_OsSemHandle hSem)
{
    up((struct semaphore*)hSem);
    return PAL_SOK;
}

/**
 * \brief   PAL OS Semaphore Report
 */
PAL_INLINE PAL_Result PAL_osSemReport(PAL_OsSemHandle hSem, PAL_OsSemReport *report, Char* buf)
{
    return PAL_OS_ERROR_NOT_SUPPORTED;
}

/*@}*/
/*@}*/
#endif

