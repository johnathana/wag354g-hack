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
**|         Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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

/*******************************************************************************
 * FILE PURPOSE:    TI Standard defines for primitive "C" types only.
 *******************************************************************************
 * FILE NAME:       tistdtypes.h
 *
 * DESCRIPTION:     
 * The purpose of this header file is to gather all the primitive "C"
 * data types into one consolidated file. This file is expected to be
 * included in the basic types file exported by other software components
 * such as for example CSL. This file will *not* specify the types
 * for aggregate data such as "C" structures or vectored quantities.
 * All the types defined in this header shall be bracked with _TI_STD_TYPES
 * symbol definition scope as a means of protection against multiple definitions
 * -----------------------------------------------------------------------------
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES

typedef unsigned short  Bool;   /* Boolean truth value - 0 or 1 only */
typedef int             Int;    /* Signed base integer quanity */
typedef unsigned int    Uns;    /* This is depricated support only */
typedef char            Char;
typedef char*           String;
typedef void*           Ptr;    /* Arbitrary pointer (works for pointing to any datum) */

/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */ 
typedef unsigned int    Uint32;
typedef unsigned short  Uint16;
typedef unsigned char   Uint8;

/* Signed integer definitions (32bit, 16bit, 8bit) follow... */ 
typedef int             Int32;
typedef short           Int16;
typedef char            Int8; 

#endif /* _TI_STD_TYPES */
