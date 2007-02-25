/*
 * linux/include/asm-arm/arch-ixp1200/hardware.h
 *
 * Copyright (C) 1998-1999 Russell King.
 *
 * This file contains the hardware definitions of the IXP-1200 Eval Board
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <linux/config.h>
#include <asm/arch/memory.h>

/*
 * Generic IXP1200 support
 */

#include "ixp1200.h"

/* NOTE: The documentation says the virtual 0xe0000000
   is reserved for cache flush, since the MMU traps 
   this directly and returns 0 when this virtual 
   memory offset is accessed. But it turns out that
   this still induces some wait cycles for SDRAM.
   It is apparently better to read from 0xa0000000.
*/



/* This is kludgy. This is used in proc-sa110.S.
   We will give it the address of a register 
   which will always read out zero. In our
   case giving it the pci isr status register
   works cos the code is called outside isr
   context
*/

#define UNCACHEABLE_ADDR	(ARMCSR_BASE+0x180)

#define	PCIBIOS_MIN_IO		0x54000000
#define PCIBIOS_MIN_MEM		0x60000000

#define PCIO_BASE		0x9c700000

#define pcibios_assign_all_busses() 1

#endif
