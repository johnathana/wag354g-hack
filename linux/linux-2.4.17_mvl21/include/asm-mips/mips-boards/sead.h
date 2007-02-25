/* $Id: sead.h,v 1.1.1.2 2005/03/28 06:56:48 sparq Exp $
 *
 * sead.h
 *
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999 MIPS Technologies, Inc.
 *
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2000 Texas Instruments Inc.
 *
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Defines of the Sead board specific address-MAP, registers, etc.
 *
 */
#ifndef _MIPS_SEAD_H
#define _MIPS_SEAD_H

#include <asm/addrspace.h>

/*
 * SEAD board SDRAM base address.  This is used to setup the 
 * bootmem tables
 */

#define SEAD_SDRAM_BASE 0x14000000UL

/*
 * SEAD board vectors
 */

#define SEAD_VECS (KSEG1ADDR(SEAD_SDRAM_BASE))
#define SEAD_VECS_KSEG0 (KSEG0ADDR(SEAD_SDRAM_BASE))

/* 
 * Sead RTC-device indirect register access.
 */
//#define SEAD_RTC_ADR_REG       (KSEG1ADDR(0x1f000800))
//#define SEAD_RTC_DAT_REG       (KSEG1ADDR(0x1f000808))
#ifdef CONFIG_MIPS_SEAD
#define SEAD_RTC_ADR_REG      NULL
#define SEAD_RTC_DAT_REG      NULL
#else
#define SEAD_RTC_ADR_REG       (KSEG1ADDR(0x1f000800))
#define SEAD_RTC_DAT_REG       (KSEG1ADDR(0x1f000808))
#endif /* CONFIG_MIPS_SEAD */

/*
 * Sead interrupt controller register base (primary) 
 */
#define SEAD_ICTRL_REGS_BASE  (KSEG1ADDR(0x08612400))
//#define SEAD_ICTRL_REGS_BASE  (KSEG1ADDR(0x08612800))

/*
 * Sead exception controller register base (secondary) 
 */
#define SEAD_ECTRL_REGS_BASE  (SEAD_ICTRL_REGS_BASE+0x80)


/*
 * Sead Interrupt Channel Control register base
 */
#define SEAD_CHCTRL_REGS_BASE (SEAD_ICTRL_REGS_BASE + 0x200)


/*
 * Sead UART register base.
 */

#define AVALANCHE_UART0_REGS_BASE (KSEG1ADDR(0x08610E00))  /* AVALANCHE UART 0 */
#define AVALANCHE_UART1_REGS_BASE (KSEG1ADDR(0x08610F00))  /* AVALANCHE UART 1 */
#define SEAD_UART0_REGS_BASE    (KSEG1ADDR(0x1C700000))    /* SEAD UART DEV 0 */
#define SEAD_UART1_REGS_BASE    (KSEG1ADDR(0x1C800000))    /* SEAD UART DEV 1 */

#define SEAD_BASE_BAUD ( 3686400 / 16 ) 
#define AVALANCHE_BASE_BAUD ( 3686400 / 16 ) 

#define SEADINT_UART0      3
#define SEADINT_UART1	   4

/* 
 * Sead DMA controller base
 */

#define AVALANCHE_DMA0_CTRL_BASE       (KSEG1ADDR(0x08611400)) /* DMA 0 (channels 0-3) */

/*
 * Sead display register base.
 */
#if defined (CONFIG_MIPS_SEAD2) && !defined (CONFIG_MIPS_EVM3)
#define SEAD_ASCII_DISPLAY_WORD_BASE    (KSEG1ADDR(0x1C4001F8)) /* How is this used??? JAH */
#define SEAD_ASCII_DISPLAY_POS_BASE     (KSEG1ADDR(0x1C4001C0))
#endif /* CONFIG_MIPS_SEAD && !CONFIG_MIPS_EVM3 */


#if defined (CONFIG_MIPS_EVM3)
#define EVM3_ASCII_DISPLAY_POS_BASE     (KSEG1ADDR(0x1D000038))
#define EVM3_ASCII_DISPLAY_WORD_BASE    (KSEG1ADDR(0x1D00003F)) /* How is this used??? JAH */
#endif /* !CONFIG_MIPS_EVM3 */


/* ASCII display positions */
#if defined (CONFIG_MIPS_SEAD2) && !defined (CONFIG_MIPS_EVM3)
#define SEAD_ASCIIPOS0          	0x1C4001C0
#define SEAD_ASCIIPOS1          	0x1C4001C8
#define SEAD_ASCIIPOS2          	0x1C4001D0
#define SEAD_ASCIIPOS3          	0x1C4001D8
#define SEAD_ASCIIPOS4          	0x1C4001E0
#define SEAD_ASCIIPOS5          	0x1C4001E8
#define SEAD_ASCIIPOS6          	0x1C4001F0
#define SEAD_ASCIIPOS7          	0x1C4001F8
#endif /* CONFIG_MIPS_SEAD && !CONFIG_MIPS_EVM3 */

#if defined (CONFIG_MIPS_EVM3)  /* CONFIG_MIPS_EVM3 */
#define EVM3_ASCIIPOS0          	0x1D000038
#define EVM3_ASCIIPOS1          	0x1D000039
#define EVM3_ASCIIPOS2          	0x1D00003A
#define EVM3_ASCIIPOS3          	0x1D00003B
#define EVM3_ASCIIPOS4          	0x1D00003C
#define EVM3_ASCIIPOS5          	0x1D00003D
#define EVM3_ASCIIPOS6          	0x1D00003E
#define EVM3_ASCIIPOS7          	0x1D00003F
#endif /* !CONFIG_MIPS_EVM3 */



/*
 * Yamon Prom print address.
 */
#define SEAD_YAMON_FUNCTION_BASE             (KSEG1ADDR(0x10000500))
#define SEAD_YAMON_PROM_PRINT_COUNT_ADDR     (SEAD_YAMON_FUNCTION_BASE + 0x4)  /* print_count function */
#define SEAD_YAMON_PROM_PRINT_ADDR           (SEAD_YAMON_FUNCTION_BASE + 0x34)
/*
 * Sead Reset and PSU standby register.
 */
//#define SEAD_SOFTRES_REG       (KSEG1ADDR(0x1f000500))
//#define SEAD_PSUSTBY_REG       (KSEG1ADDR(0x1f000600))
//#define SEAD_GORESET           0x42
//#define SEAD_GOSTBY            0x4d
#define SEAD_SOFTRES_REG         (KSEG1ADDR(0x08611600))    /* Resets machine */
#define SEAD_PSUSTBY_REG         (KSEG1ADDR(0x08611600))    /* Turns off power supply unit */
#define SEAD_GORESET             0x1
#define SEAD_GOSTBY              0x1


/*
 * Galileo GT64120 system controller register base.
 */
#define SEAD_GT_BASE    (KSEG1ADDR(0x1be00000))

/************************************************************************
 *  PERIPHERAL BUS LEDs (P-LED):
*************************************************************************/

/************************************************************************
 *      P-LED Register Addresses
*************************************************************************/

#define SEAD_PLED		  (KSEG1ADDR(0x01C500000)) /* 0x1D200000 P-LED         	*/


/************************************************************************
 *      Register field encodings
*************************************************************************/

/******** reg: PLED ********/
/* bits 7:0: VAL */
#define SEAD_PLED_VAL_MSK	        0xff

/* bit 0:  */
#define SEAD_PLED_BIT0_SHF	    	0
#define SEAD_PLED_BIT0_MSK	    	(1 << SEAD_PLED_BIT0_SHF)
#define SEAD_PLED_BIT0_ON           	SEAD_PLED_BIT0_MSK

/* bit 1:  */
#define SEAD_PLED_BIT1_SHF	    	1
#define SEAD_PLED_BIT1_MSK	    	(1 << SEAD_PLED_BIT1_SHF)
#define SEAD_PLED_BIT1_ON           	SEAD_PLED_BIT1_MSK

/* bit 2:  */
#define SEAD_PLED_BIT2_SHF	    	2
#define SEAD_PLED_BIT2_MSK	    	(1 << SEAD_PLED_BIT2_SHF)
#define SEAD_PLED_BIT2_ON           	SEAD_PLED_BIT2_MSK

/* bit 3:  */
#define SEAD_PLED_BIT3_SHF	    	3
#define SEAD_PLED_BIT3_MSK	    	(1 << SEAD_PLED_BIT3_SHF)
#define SEAD_PLED_BIT3_ON           	SEAD_PLED_BIT3_MSK

/* bit 4:  */
#define SEAD_PLED_BIT4_SHF	    	4
#define SEAD_PLED_BIT4_MSK	    	(1 << SEAD_PLED_BIT4_SHF)
#define SEAD_PLED_BIT4_ON           	SEAD_PLED_BIT4_MSK

/* bit 5:  */
#define SEAD_PLED_BIT5_SHF	    	5
#define SEAD_PLED_BIT5_MSK	    	(1 << SEAD_PLED_BIT5_SHF)
#define SEAD_PLED_BIT5_ON           	SEAD_PLED_BIT5_MSK

/* bit 6:  */
#define SEAD_PLED_BIT6_SHF	    	6
#define SEAD_PLED_BIT6_MSK	    	(1 << SEAD_PLED_BIT6_SHF)
#define SEAD_PLED_BIT6_ON           	SEAD_PLED_BIT6_MSK

/* bit 7:  */
#define SEAD_PLED_BIT7_SHF	    	7
#define SEAD_PLED_BIT7_MSK	    	(1 << SEAD_PLED_BIT7_SHF)
#define SEAD_PLED_BIT7_ON           	SEAD_PLED_BIT7_MSK

#endif /* !(_MIPS_SEAD_H) */







