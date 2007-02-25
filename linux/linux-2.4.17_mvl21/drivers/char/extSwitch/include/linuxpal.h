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
 
 
#ifndef __LINUX_PAL_H_
#define __LINUX_PAL_H_

#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/string.h>
#include <linux/param.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/spinlock.h>

/*#include "stdlib.h"*/
#include "pal_defs.h"


#define PAL_RESULT_ASSERT(expr) \
    if(!(expr)) \
        return PAL_False
         
         
#define PAL_NULL NULL
 
#define PAL_MALLOC(expr) \
        kmalloc(expr,GFP_KERNEL)
         
#define PAL_FREE(expr) \
        kfree(expr)
 
#define PAL_PRINT printk
 
#endif /* __LINUX_PAL_H_*/

