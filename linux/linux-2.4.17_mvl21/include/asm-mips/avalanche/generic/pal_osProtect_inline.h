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

/** \file   pal_osProtect_inline.h
    \brief  OsPROTECT Services Source File
    
    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_OSPROTECT_INLINE_H__
#define __PAL_OSPROTECT_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include "pal_osCfg.h"
#include <linux/spinlock.h>
#include <linux/slab.h>

/**
 * \defgroup PalOSProtect PAL OS Protect Interface
 * 
 * PAL OS Protect Interface
 * \{
 */

/** \name PAL OS Protect Interface
 *  PAL OS Protect Interface
 * \{
 */


/**
 * \brief   PAL OS Protect Entry
 */
PAL_INLINE void PAL_osProtectEntry(Int level, Uint32* cookie)
{
	Uint flags;
    if(level == PAL_OSPROTECT_INTERRUPT) 
	{
        local_irq_save(flags);  
		*cookie = flags;
	}
}
/**
 * \brief   PAL OS Protect Exit
 */
PAL_INLINE void PAL_osProtectExit(Int level, Uint32 cookie)
{
    if(level == PAL_OSPROTECT_INTERRUPT) 
	{
        local_irq_restore(cookie);
	}
}

/*\}*/
/*\}*/

#endif

