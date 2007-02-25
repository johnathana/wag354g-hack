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
 * FILE PURPOSE:    Standard definitions
 *******************************************************************************
 * FILE NAME:       stddef.h
 *
 * DESCRIPTION:     Standard definitions
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef ___STDDEF_H___
#define ___STDDEF_H___

typedef char           bit8;
typedef short          bit16;
typedef int            bit32;

typedef unsigned char  bit8u;
typedef unsigned short bit16u;
typedef unsigned int   bit32u;
typedef char           INT8;
typedef short          INT16;
typedef int            INT32;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;


typedef unsigned char  BOOL;
typedef unsigned char  bool;

typedef int (*FUNC_PTR)(int, char**);

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==2)
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF    (-1)
#endif



#endif /* ___STDDEF_H___ */

