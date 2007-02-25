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
 * FILE PURPOSE:    PSP Platform Definations
 *******************************************************************************
 * FILE NAME:       pal_defs.h
 *
 * DESCRIPTION:     Platform definations for PSP are defined here
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef __PAL_DEFS_H__
#define __PAL_DEFS_H__

/* Types */

/* Import the TI standard primitive "C" types defines */
#ifndef _TI_STD_TYPES
#include <_tistdtypes.h> 
#define _TI_STD_TYPES
#endif /* if _TI_STD_TYPES not defined */

/* PAL Result - return value of a function  */
typedef Int     PAL_Result;

/* Type Macros */
#define PAL_False       ((Bool)0)
#define PAL_True        ((Bool)(!PAL_False))

/* General Macros */
#define PAL_MAX(a,b)    ((a) > (b) ? (a) : (b))
#define PAL_MIN(a,b)    ((a) < (b) ? (a) : (b))

/* Array Dimension */
#define PAL_DIM(array)  (sizeof(array)/sizeof(array[0]))

/* Endianness */

#define PAL_MK_UNIT16(high8,low8)                               \
    ((Uint16)( ((Uint16)(high8) << 8) | (Uint16)(low8) ))

#define PAL_UNIT16_LOW8(a)                                      \
    ((Uint8)((a) & 0x00FF))

#define PAL_UNIT16_HIGH8(a)                                     \
    ((Uint8)(((a) >> 8) & 0x00FF))

#define PAL_MK_UNIT32(high16,low16)                             \
    ((Uint32)( ((Uint32)(high16) << 16) | (Uint32)(low16) ))

#define PAL_MK_UNIT32_FROM8S(high8,med_high8,med_low8,low8)     \
    PAL_MK_UNIT32(PAL_MK_UNIT16(high8,med_high8), PAL_MK_UNIT16(med_low8, low8))

#define PAL_UNIT32_LOW16(u32)                                   \
    ((Uint16)((u32) & 0xFFFF))

#define PAL_UNIT32_HIGH16(u32)                                  \
    ((Uint16)(((u32) >> 16) & 0xFFFF))

#define PAL_UNIT32_LOW8(u32)                                    \
    ((Uint8)((u32) & 0x00FF))

#define PAL_UNIT32_MED_LOW8(u32)                                \
    ((Uint8)(((u32) >> 8) & 0xFF))

#define PAL_UNIT32_MED_HIGH8(u32)                               \
    ((Uint8)(((u32) >> 16) & 0xFF))

#define PAL_UNIT32_HIGH8(u32)                                   \
    ((Uint8)(((u32) >> 24) & 0xFF))

#define PAL_SWAP_UNIT16(w)      \
    (PAL_MK_UNIT16(Uint16_LOW8(w), PAL_UNIT16_HIGH8(w)))

#define PAL_SWAP_UNIT32(u32)                \
    (PAL_MK_UNIT32_FROM8S(                  \
        PAL_UNIT32_LOW8(u32),               \
        PAL_UNIT32_MED_LOW8(u32),           \
        PAL_UNIT32_MED_HIGH8(u32),          \
        PAL_UNIT32_HIGH8(u32)))

/******************************************************************************
* Endian Utility Macros:-
* PAL_UNIT16_LE(w) converts a Little-Endian 16bit word to the current endian word
* PAL_UNIT16_BE(w) converts a Big-Endian 16bit word to the current endian word
* PAL_UNIT32_LE(d) converts a Little-Endian 32bit dword to the current endian dword
* PAL_UNIT32_BE(d) converts a Big-Endian 32bit dword to the current endian dword
*
*******************************************************************************/

typedef enum PAL_Endian_t {
	PAL_LITTLE_ENDIAN = 0,
	PAL_BIG_ENDIAN = 1
} PAL_Endian;

#if defined(BIG_ENDIAN)

#define PAL_UNIT16_LE(w)    PAL_SWAP_UNIT16(w)
#define PAL_UNIT16_BE(w)    (w)
#define PAL_UNIT32_LE(d)    PAL_SWAP_UNIT32(d)
#define PAL_UNIT32_BE(d)    (d)

#else

#define PAL_UNIT16_LE(w)    (w)
#define PAL_UNIT16_BE(w)    PAL_SWAP_UNIT16(w)
#define PAL_UNIT32_LE(d)    (d)
#define PAL_UNIT32_BE(d)    PAL_SWAP_UNIT32(d)

#endif /* Endian */

#endif /* __PAL_DEFS_H__ */
