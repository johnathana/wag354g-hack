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


/** \file   pal.h
    \brief  Platform Include file

    This file includes all platform abstraction related headers.
    
    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef _PAL_H_
#define _PAL_H_

#include "pformCfg.h"   /* Platform specific file - includes SOC specific header */
#include "pal_defs.h"   /* Platform definitions - basic types - includes "_tistdtypes.h" */
#include "csl_defs.h"   /* CSL definitions - basic types and macros */

#include "pal_os.h"     /* PAL OS interfaces file - includes selected OS abstraction headers */

#include "pal_sys.h"    /* Platform System Interfaces file */

#endif /* _PAL_H_ */
