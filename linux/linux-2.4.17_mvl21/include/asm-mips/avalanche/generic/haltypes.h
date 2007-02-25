/******************************************************************************
 * FILE PURPOSE:    Platform dependent type information Header
 ********************************************************************************
 * FILE NAME:       haltypes.h
 *
 * DESCRIPTION:     Platform dependent (tuned) types definations.
 *                  Intented to be used by HAL/Drivers etc.
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __HAL_TYPES_H__
#define __HAL_TYPES_H__

typedef char    INT8;
typedef short   INT16;
typedef int     INT32;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;

typedef int     BOOL;
typedef int     STATUS;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL 0
#endif

#endif /*  __HAL_TYPES_H__ */