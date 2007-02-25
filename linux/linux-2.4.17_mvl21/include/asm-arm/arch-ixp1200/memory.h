/*
 * linux/include/asm-arm/arch-ixp1200/memory.h
 *
 * Copyright (C) 1996-1999 Russell King.
 * Copyright (C) 2001 MontaVista Software, Inc.
 *
 * 20-Mar-2000 Uday Naik 
 * 	Created for IXP1200
 * 26-Sep-2001 dsaxena 
 * 	Cleanup/port to 2.4.x kernel
 */

#ifndef __ASM_ARCH_MMU_H
#define __ASM_ARCH_MMU_H

/*
 * Task size: 3 Gig
 */
#define TASK_SIZE	(0xc0000000UL)
#define TASK_SIZE_26	(0x04000000UL)

#define TASK_UNMAPPED_BASE (TASK_SIZE / 3)

/*
 * Page offset: 3GB
 */
#define PAGE_OFFSET	(0xc0000000UL)

/*
 * Physical memory offset: 
 */
#define PHYS_OFFSET	(CONFIG_IXP1200_SDRAM_BASE)


/*
 * This macro only works for cached linux memory not the 
 * uncached packet SDRAM.
 */
#define __virt_to_phys__is_a_macro
#define __virt_to_phys(x) (((x - PAGE_OFFSET) + PHYS_OFFSET))
#define __phys_to_virt__is_a_macro
#define __phys_to_virt(x) (((x - PHYS_OFFSET) + PAGE_OFFSET))


/*
 * Bus addresses are based on the PCI inbound window which
 * is configured either by the external host or by us 
 * if we're running as the system controller
 */
#define __virt_to_bus__is_a_macro
#define __virt_to_bus(v)	\
	((__virt_to_phys(v) - 0xc0000000) + (*CSR_PCISDRAMBASE & 0xfffffff0))
#define __bus_to_virt__is_a_macro
#define __bus_to_virt(b)	\
	__phys_to_virt((((b - (*CSR_PCISDRAMBASE & 0xfffffff0)) + 0xc0000000)))

#define PHYS_TO_NID(x)	0

#endif

