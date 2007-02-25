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
**|     Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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

/*******************************************************************************************
*	tnetd53xx_vlynq.h		SHARED TYPEDEFs, CONSTs, and API for TNETD53xx VLYNQ interface
********************************************************************************************
*	PLEASE, READ THE NOTES BELOW TO BETTER UNDERSTAND HOW THIS FILE IS STRUCTURED.
*
*	These are const, typedef, and api definitions for VLYNQ interface.
*
*	NOTES:
*	1. This file may be included into both C and Assembly files.
*		- for .s files, please do #define _ASMLANGUAGE in your ASM file to
*			avoid C data types (typedefs) below;
*		- for .c files, you don't have to do anything special.
*
*	2. This file has a number of sections for each Avalanch sub-system. When adding
*		a new constant, find the subsystem you are working on and follow the
*		name pattern. If you are adding another typedef for your interface, please,
*		place it with other typedefs and function prototypes.
*
*	3. Please, DO NOT add any macros or types that are local to a subsystem to avoid
*		cluttering. Include such items directly into the module's .c file or have a
*		local .h file to pass data between smaller modules. This file defines only
*		shared items.
* HISTORY:
*	Based on 9Oct01 Greg	1.00  - Joseph Gabbay - Original.
*
********************************************************************************************/

#ifndef	___TNETD53XX_VLYNQ_H___
#define	___TNETD53XX_VLYNQ_H___
#ifndef	_ASMLANGUAGE

/******************************************************************************
* VLYNQ driver definitions, VLYNQ Module is available on PUMA and newer chips.
* Future designs may include more than one VLYNQ module.
*******************************************************************************/

/* Define the following to use workarounds for PG1.2 bugs in the hardware
*/
#ifndef TNETD53XX_PG12_BUGS
#define TNETD53XX_PG12_BUGS
#endif

/**************************************
* Base address of VLYNQ modules.
**&************************************/
#define TNETD53XX_VLYNQ_IVR_MAXIVR           7      /* current VLYNQ modules support up to 8 IRQs */
#define TNETD53XX_VLYNQ_MAX_MEMORY_REGIONS   4      /* Number of VLYNQ memory regions */
#define TNETD53XX_PUMA_VLBASE_1           0x08611800	/* PUMA, PUMA-S have one module. */


/**************************************
* Description of the VLYNQ module.
***************************************/
#define TNETD53XX_VLYNQ_REMOTE_OFFSET     0x80
#define TNETD53XX_VLYNQ_REMOTE_REG(base, r)  ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_REMOTE_OFFSET + (r)) )
#define TNETD53XX_VLYNQ_REV_OFFSET        0x00
#define TNETD53XX_VLYNQ_CTL_OFFSET        0x04
#define TNETD53XX_VLYNQ_STS_OFFSET        0x08
#define TNETD53XX_VLYNQ_THRESH_OFFSET     0x0c
#define TNETD53XX_VLYNQ_INTCR_OFFSET      0x10
#define TNETD53XX_VLYNQ_INTSR_OFFSET      0x14
#define TNETD53XX_VLYNQ_INTPTR_OFFSET     0x18
#define TNETD53XX_VLYNQ_TXMAP_OFFSET      0x1c
#define TNETD53XX_VLYNQ_RXMAPSIZE_OFFSET(map)    (0x20 + (( ((unsigned)((map)-1)) & 3) << 3 )) /* 1 =< map <= TNETD53XX_VLYNQ_MAX_MEMORY_REGIONS */
#define TNETD53XX_VLYNQ_RXMAPOFFSET_OFFSET(map)  (TNETD53XX_VLYNQ_RXMAPSIZE_OFFSET(map) + 4)
#define TNETD53XX_VLYNQ_CHIPVER_OFFSET    0x40
#define TNETD53XX_VLYNQ_IVR_OFFSET(ivr)   (0x60 + ((((unsigned)(ivr)) & 31) & ~3))   /* 0 =< ivr <= 31 (currently ivr <= TNETD53XX_VLYNQ_IVR_MAXIVR) */
#define TNETD53XX_VLYNQ_IVR2FIELD(fld, ivr)   \
    (((unsigned long)(fld)) << ((((unsigned)(ivr)) % 4) * 8))


#define TNETD53XX_VLYNQ_REV(base)         ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_REV_OFFSET) )
#define TNETD53XX_VLYNQ_REV_MODULE_ID     0xffff0000
#define TNETD53XX_VLYNQ_REV_MAJOR         0x0000ff00
#define TNETD53XX_VLYNQ_REV_MINOR         0x000000ff


#define TNETD53XX_VLYNQ_CTL(base)         ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_CTL_OFFSET) )
#define TNETD53XX_VLYNQ_CTL_RESET         0x00000001	/* reserved, according to documentation */
#define TNETD53XX_VLYNQ_CTL_ILOOP         0x00000002
#define TNETD53XX_VLYNQ_CTL_INT2CFG       0x00000080
#define TNETD53XX_VLYNQ_CTL_INTVEC        0x00001F00
#define TNETD53XX_VLYNQ_CTL_INTENABLE     0x00002000
#define TNETD53XX_VLYNQ_CTL_INTLOCAL      0x00004000
#define TNETD53XX_VLYNQ_CTL_CLKDIR        0x00008000
#define TNETD53XX_VLYNQ_CTL_CLKDIV        0x00070000
#define TNETD53XX_VLYNQ_CTL_MODE          0x00E00000

#define TNETD53XX_VLYNQ_STS(base)         ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_STS_OFFSET) )
#define TNETD53XX_VLYNQ_STS_LINK          0x00000001	 /* Link is active */
#define TNETD53XX_VLYNQ_STS_MPEND         0x00000002   /* Pending master requests */
#define TNETD53XX_VLYNQ_STS_SPEND         0x00000004   /* Pending slave requests */
#define TNETD53XX_VLYNQ_STS_NFEMPTY0      0x00000008   /* Master data FIFO not empty */
#define TNETD53XX_VLYNQ_STS_NFEMPTY1      0x00000010   /* Master command FIFO not empty */
#define TNETD53XX_VLYNQ_STS_NFEMPTY2      0x00000020   /* Slave data FIFO not empty */
#define TNETD53XX_VLYNQ_STS_NFEMPTY3      0x00000040   /* Slave command FIFO not empty */
#define TNETD53XX_VLYNQ_STS_LERROR        0x00000080	 /* Local error, w/c */
#define TNETD53XX_VLYNQ_STS_RERROR        0x00000100	 /* remote error w/c */
#define TNETD53XX_VLYNQ_STS_OFLOW         0x00000200
#define TNETD53XX_VLYNQ_STS_IFLOW         0x00000400
#define TNETD53XX_VLYNQ_STS_MODESUP       0x00E00000   /* Highest mode supported */
#define TNETD53XX_VLYNQ_STS_SWIDTH        0x07000000   /* Use for reading the width of VLYNQ bus */
#define TNETD53XX_VLYNQ_STS_DEBUG         0xE0000000

#define TNETD53XX_VLYNQ_THRESH(base)      ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_THRESH_OFFSET) )
#define TNETD53XX_VLYNQ_INTCR(base)       ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_INTCR_OFFSET ) )
#define TNETD53XX_VLYNQ_INTSR(base)       ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_INTSR_OFFSET ) )
#define TNETD53XX_VLYNQ_INTPTR(base)      ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_INTPTR_OFFSET) ) /* should be set to TNETD53XX_VLYNQ_INTSR_OFFSET */
#define TNETD53XX_VLYNQ_TXMAP(base)       ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_TXMAP_OFFSET) )

#define TNETD53XX_VLYNQ_RXMAPSIZE(base, map) \
                                          ( (volatile UINT32 *)((base) + TNETD53XX_VLYNQ_RXMAPSIZE_OFFSET(map)))

#define TNETD53XX_VLYNQ_RXMAPOFFSET(base, map) \
                                          ( (volatile UINT32 *)((base) + TNETD53XX_VLYNQ_RXMAPOFFSET_OFFSET(map)))


#define TNETD53XX_VLYNQ_CHIPVER(base)     ( (volatile  UINT32 *)((base) + TNETD53XX_VLYNQ_CHIPVER_OFFSET) )
#define TNETD53XX_VLYNQ_CHIPVER_DEVREV    0xffff0000
#define TNETD53XX_VLYNQ_CHIPVER_DEVID     0x0000ffff

#define TNETD53XX_VLYNQ_IVR(base, ivr)     \
                                          ( (volatile UINT32 *)((base) + TNETD53XX_VLYNQ_IVR_OFFSET(ivr)))
#define TNETD53XX_VLYNQ_IVR_03TO00(base)  TNETD53XX_VLYNQ_IVR((base),  0)
#define TNETD53XX_VLYNQ_IVR_07TO04(base)  TNETD53XX_VLYNQ_IVR((base),  4)
#define TNETD53XX_VLYNQ_IVR_11TO08(base)  TNETD53XX_VLYNQ_IVR((base),  8)
#define TNETD53XX_VLYNQ_IVR_15TO12(base)  TNETD53XX_VLYNQ_IVR((base), 12)
#define TNETD53XX_VLYNQ_IVR_19TO16(base)  TNETD53XX_VLYNQ_IVR((base), 16)
#define TNETD53XX_VLYNQ_IVR_23TO20(base)  TNETD53XX_VLYNQ_IVR((base), 20)
#define TNETD53XX_VLYNQ_IVR_27TO24(base)  TNETD53XX_VLYNQ_IVR((base), 24)
#define TNETD53XX_VLYNQ_IVR_31TO28(base)  TNETD53XX_VLYNQ_IVR((base), 28)

#define TNETD53XX_VLYNQ_IVR_INTEN(ivr)    TNETD53XX_VLYNQ_IVR2FIELD(0x80, (ivr))  /* set to enable IRQ */
#define TNETD53XX_VLYNQ_IVR_INTTYPE(ivr)  TNETD53XX_VLYNQ_IVR2FIELD(0x40, (ivr))  /* set: pulse IRQ, clear: level IRQ */
#define TNETD53XX_VLYNQ_IVR_INTPOL(ivr)   TNETD53XX_VLYNQ_IVR2FIELD(0x20, (ivr))  /* set: active low, clear: active high */
#define TNETD53XX_VLYNQ_IVR_INTVEC(ivr)   TNETD53XX_VLYNQ_IVR2FIELD(0x1F, (ivr))  /* Interrupt vector */
#define TNETD53XX_VLYNQ_IVR_INTALL(ivr)   TNETD53XX_VLYNQ_IVR2FIELD(0xFF, (ivr))  /* whole octet related to this vector */


/*********************************
 * Remote VLYNQ registers.
 *********************************/

#define TNETD53XX_VLYNQ_REMOTE_REV(base)         TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_REV_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_CTL(base)         TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_CTL_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_STS(base)         TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_STS_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_THRESH(base)      TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_THRESH_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_INTCR(base)       TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_INTCR_OFFSET )
#define TNETD53XX_VLYNQ_REMOTE_INTSR(base)       TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_INTSR_OFFSET )
#define TNETD53XX_VLYNQ_REMOTE_INTPTR(base)      TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_INTPTR_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_TXMAP(base)       TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_TXMAP_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_RXMAPSIZE(base, map)   TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_RXMAPSIZE_OFFSET(map))
#define TNETD53XX_VLYNQ_REMOTE_RXMAPOFFSET(base, map) TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_RXMAPOFFSET_OFFSET(map))
#define TNETD53XX_VLYNQ_REMOTE_CHIPVER(base)          TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_CHIPVER_OFFSET)
#define TNETD53XX_VLYNQ_REMOTE_IVR(base, ivr)    TNETD53XX_VLYNQ_REMOTE_REG((base), TNETD53XX_VLYNQ_IVR_OFFSET(ivr))
#define TNETD53XX_VLYNQ_REMOTE_IVR_03TO00(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 0)
#define TNETD53XX_VLYNQ_REMOTE_IVR_07TO04(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 4)
#define TNETD53XX_VLYNQ_REMOTE_IVR_11TO08(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 8)
#define TNETD53XX_VLYNQ_REMOTE_IVR_15TO12(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 12)
#define TNETD53XX_VLYNQ_REMOTE_IVR_19TO16(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 16)
#define TNETD53XX_VLYNQ_REMOTE_IVR_23TO20(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 20)
#define TNETD53XX_VLYNQ_REMOTE_IVR_27TO24(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 24)
#define TNETD53XX_VLYNQ_REMOTE_IVR_31TO28(base)  TNETD53XX_VLYNQ_REMOTE_IVR((base), 28)


#endif	/* _ASMLANGUAGE */
#endif	/*___TNETD53XX_VLYNQ_H___*/

