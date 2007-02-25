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

/** \file   pal_osWait_inline.h
    \brief  OsWAIT Services Soource File

    This file defines OS abstraction services for programmed interval-waits.
    
    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_OSWAIT_INLINE_H__
#define __PAL_OSWAIT_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include "pal_osCfg.h"
#include <linux/delay.h> 
#include <linux/sched.h> 

/**
 * \defgroup PalOSWait PAL OS Wait Interface
 * 
 * PAL OS Wait Interface
 * \{
 */

/** \name PAL OS Wait Interface
 * \{
 */

/**
 * \brief   PAL_osWaitMsecs()
 * PAL_osWaitTicks
 */
PAL_INLINE PAL_Result PAL_osWaitMsecs(Uint32 mSecs)
{
    Uint32 ticks = (HZ * mSecs)/1000;

    /*  If the wait period is less than 1 tick then busyloop */
    if(ticks)
	{
    	set_current_state(TASK_INTERRUPTIBLE);
    	schedule_timeout(ticks);
	}
    else
	{
        mdelay(mSecs); 
	}
		
    return PAL_SOK;
}

/**
 * \brief   PAL_osWaitTicks()
 */
PAL_INLINE PAL_Result PAL_osWaitTicks(Uint32 ticks)
{
    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(ticks);
    return PAL_SOK;
}

/*\}*/
/*\}*/

#endif

