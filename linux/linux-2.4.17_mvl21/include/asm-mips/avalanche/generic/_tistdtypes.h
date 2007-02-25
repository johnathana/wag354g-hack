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

/** \file   _tistdtypes.h
    \brief  TI Standard defines for primitive "C" types only

    This file provides TI Standard defines for primitive "C" types only

    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

/* 
 * NOTE: This is soc specific file and will be moved to 
 * directory psp\soc\arch<name>\soc<name>\src
 * 
 * Each soc will have its own _tistdtypes.h file.
 */ 

#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES


/**
 * \defgroup TIBasicTypes TI Basic Types
 * 
 * All components - PAL, SRV, DDC, CSL shall use TI basic types 
 * to maintain compatibility of code with all systems. 
 * \n File _tistdtypes.h will adapt the types to the native compiler.
 */
/*@{*/

/**
 * \enum Bool - Boolean Type
 */
typedef enum 
{
    False = 0,
    True = 1
} Bool;

typedef int             Int;    /**< Signed base integer quanity */
typedef unsigned int    Uns;    /**< This is depricated support only */
typedef unsigned int    Uint;   /**< Unsigned base integer quantity */
typedef char            Char;   /**< Character quantity */
typedef char*           String; /**< Character pointer quantity */
typedef void*           Ptr;    /**< Arbitrary (void) pointer (works for pointing to any datum) */

/* Signed integer definitions (32bit, 16bit, 8bit) follow... */ 
typedef int             Int32;
typedef short           Int16;
typedef signed char     Int8; 

/* Unsigned integer definitions (32bit, 16bit, 8bit) follow... */ 
typedef unsigned int    Uint32;
typedef unsigned short  Uint16;
typedef unsigned char   Uint8;

/*@}*/
#endif /* _TI_STD_TYPES */
