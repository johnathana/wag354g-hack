/*
 * linux/include/asm-arm/arch-ebsa285/dma.h
 *
 * Architecture DMA routines
 *
 * Copyright (C) 1998,1999 Russell King
 * Copyright (C) 1998,1999 Philip Blundell
 *
 * Added IXP1200 EVB support : Uday Naik
 *
 */

#ifndef __ASM_ARCH_DMA_H
#define __ASM_ARCH_DMA_H

/*
 * This is the maximum DMA address that can be DMAd to.
 */
#define MAX_DMA_ADDRESS		0xffffffff

/*
 * The IXP1200 has 2 internal DMA channels 
 */

#define MAX_DMA_CHANNELS	0

#endif /* _ASM_ARCH_DMA_H */
