/***************************************************************************
**+----------------------------------------------------------------------+**
**|                                ****                                  |**
**|                                ****                                  |**
**|                                ******o***                            |**
**|                          ********_///_****                           |**
**|                           ***** /_//_/ ****                          |**
**|                            ** ** (__/ ****                           |**
**|                                *********                             |**
**|                                 ****                                 |**
**|                                  ***                                 |**
**|                                                                      |**
**|     Copyright (c) 1998 - 2003 Texas Instruments Incorporated         |**
**|                        ALL RIGHTS RESERVED                           |**
**|                                                                      |**
**| Permission is hereby granted to licensees of Texas Instruments       |**
**| Incorporated (TI) products to use this computer program for the sole |**
**| purpose of implementing a licensee product based on TI products.     |**
**| No other rights to reproduce, use, or disseminate this computer      |**
**| program, whether in part or in whole, are granted.                   |**
**|                                                                      |**
**| TI makes no representation or warranties with respect to the         |**
**| performance of this computer program, and specifically disclaims     |**
**| any responsibility for any damages, special or consequential,        |**
**| connected with the use of this program.                              |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/

/***************************************************************************/
/*                                                                         */
/*    MODULE:  pform_types.h                                               */
/*    PURPOSE: Platform types definitions                                  */
/*                                                                         */
/***************************************************************************/


#ifndef _PFORM_TYPES_H
#define _PFORM_TYPES_H

#ifndef __linux__
#include "types\vxTypesOld.h"
#else
#include <ti_types.h>
#endif /* __linux__ */

#define MAC_ADDR_LEN    6   /* MAC Address length in bytes  */

#define TRUE   1
#define FALSE  0

#define OK     0
#define NOK    1

#define OFF    0
#define ON     1

typedef signed char  SINT8;
typedef signed short SINT16;
typedef signed long  SINT32;

typedef unsigned char   BYTE;

typedef union
{
    BYTE byte[MAC_ADDR_LEN];
    struct
    {
       USHORT word1;
       USHORT word2;
       USHORT word3;
    } a;
} L2ADDRESS;

/*
 * PCI Stuff
 * ---------
 */
typedef struct ttPCI_ACX111_DEV_INFO
{
   UINT     slotNum;
   UINT16   devID;
   UINT16   venID;
   UINT32   MemAddr;
   UINT32   MemSize;
   UINT32   RegAddr;
   UINT32   RegSize;
} tPCI_ACX111_DEV_INFO;


#endif
