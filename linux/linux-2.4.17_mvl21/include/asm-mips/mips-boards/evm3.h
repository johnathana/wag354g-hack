/* $Id: evm3.h,v 1.1.1.2 2005/03/28 06:56:47 sparq Exp $
 *
 * evm3.h
 *
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2000,2001 Texas Instruments Inc.
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
 * Defines of the EVM3 board specific address-MAP, registers, etc.
 *
 */
#ifndef _MIPS_EVM3_H
#define _MIPS_EVM3_H

#include <asm/addrspace.h>

/*
 * EVM3 board SDRAM base address.  This is used to setup the 
 * bootmem tables
 */

#define AVALANCHE_SDRAM_BASE 0x14000000UL

/*
 * EVM3 board vectors
 */

#define AVALANCHE_VECS (KSEG1ADDR(AVALANCHE_SDRAM_BASE))
#define AVALANCHE_VECS_KSEG0 (KSEG0ADDR(AVALANCHE_SDRAM_BASE))
/* 
 * Evm3 RTC-device indirect register access.
 */

#define EVM3_RTC_ADR_REG       (KSEG1ADDR(0x1f000800))
#define EVM3_RTC_DAT_REG       (KSEG1ADDR(0x1f000808))

/*
 * Evm3 interrupt controller register base (primary) 
 */

#define EVM3_ICTRL_REGS_BASE  (KSEG1ADDR(0x08612400))

/*
 * Evm3 exception controller register base (secondary) 
 */
#define EVM3_ECTRL_REGS_BASE  (EVM3_ICTRL_REGS_BASE+0x80)


/*
 * Evm3 Interrupt Channel Control register base
 */
#define EVM3_CHCTRL_REGS_BASE (EVM3_ICTRL_REGS_BASE + 0x200)


/*
 * Evm3 UART register base.
 */

#define AVALANCHE_UART0_REGS_BASE (KSEG1ADDR(0x08610E00))  /* AVALANCHE UART 0 */
#define AVALANCHE_UART1_REGS_BASE (KSEG1ADDR(0x08610F00))  /* AVALANCHE UART 1 */
#define AVALANCHE_BASE_BAUD ( 3686400 / 16 ) 
/* 
 * EVM3 DMA controller base
 */

#define AVALANCHE_DMA0_CTRL_BASE       (KSEG1ADDR(0x08611400)) /* DMA 0 (channels 0-3) */


/*
 * EVM3 display register base.
 */

#define EVM3_ASCII_DISPLAY_POS_BASE     (KSEG1ADDR(0x1D000038))
#define EVM3_ASCII_DISPLAY_WORD_BASE    (KSEG1ADDR(0x1D00003F)) /* How is this used??? JAH */


#define EVM3_ASCIIPOS0          	0x1D000038
#define EVM3_ASCIIPOS1          	0x1D000039
#define EVM3_ASCIIPOS2          	0x1D00003A
#define EVM3_ASCIIPOS3          	0x1D00003B
#define EVM3_ASCIIPOS4          	0x1D00003C
#define EVM3_ASCIIPOS5          	0x1D00003D
#define EVM3_ASCIIPOS6          	0x1D00003E
#define EVM3_ASCIIPOS7          	0x1D00003F

/*
 * Yamon Prom print address.
 */
#define EVM3_YAMON_FUNCTION_BASE             (KSEG1ADDR(0x10000500))
#define EVM3_YAMON_PROM_PRINT_COUNT_ADDR     (EVM3_YAMON_FUNCTION_BASE + 0x4)  /* print_count function */
#define EVM3_YAMON_PROM_PRINT_ADDR           (EVM3_YAMON_FUNCTION_BASE + 0x34)

/*
 * Evm3 Reset and PSU standby register.
 */
#define EVM3_SOFTRES_REG         (KSEG1ADDR(0x08611600))    /* Resets machine */
#define EVM3_PSUSTBY_REG         (KSEG1ADDR(0x08611600))    /* Turns off power supply unit */
#define EVM3_GORESET             0x1
#define EVM3_GOSTBY              0x1

/************************************************************************
 *  PERIPHERAL BUS LEDs (P-LED):
*************************************************************************/

/************************************************************************
 *      P-LED Register Addresses
*************************************************************************/

#define EVM3_PLED		  (KSEG1ADDR(0x01C500000)) /* 0x1D200000 P-LED         	*/


/************************************************************************
 *      Register field encodings
*************************************************************************/

/******** reg: PLED ********/
/* bits 7:0: VAL */
#define EVM3_PLED_VAL_MSK	        0xff

/* bit 0:  */
#define EVM3_PLED_BIT0_SHF	    	0
#define EVM3_PLED_BIT0_MSK	    	(1 << EVM3_PLED_BIT0_SHF)
#define EVM3_PLED_BIT0_ON           	EVM3_PLED_BIT0_MSK

/* bit 1:  */
#define EVM3_PLED_BIT1_SHF	    	1
#define EVM3_PLED_BIT1_MSK	    	(1 << EVM3_PLED_BIT1_SHF)
#define EVM3_PLED_BIT1_ON           	EVM3_PLED_BIT1_MSK

/* bit 2:  */
#define EVM3_PLED_BIT2_SHF	    	2
#define EVM3_PLED_BIT2_MSK	    	(1 << EVM3_PLED_BIT2_SHF)
#define EVM3_PLED_BIT2_ON           	EVM3_PLED_BIT2_MSK

/* bit 3:  */
#define EVM3_PLED_BIT3_SHF	    	3
#define EVM3_PLED_BIT3_MSK	    	(1 << EVM3_PLED_BIT3_SHF)
#define EVM3_PLED_BIT3_ON           	EVM3_PLED_BIT3_MSK

/* bit 4:  */
#define EVM3_PLED_BIT4_SHF	    	4
#define EVM3_PLED_BIT4_MSK	    	(1 << EVM3_PLED_BIT4_SHF)
#define EVM3_PLED_BIT4_ON           	EVM3_PLED_BIT4_MSK

/* bit 5:  */
#define EVM3_PLED_BIT5_SHF	    	5
#define EVM3_PLED_BIT5_MSK	    	(1 << EVM3_PLED_BIT5_SHF)
#define EVM3_PLED_BIT5_ON           	EVM3_PLED_BIT5_MSK

/* bit 6:  */
#define EVM3_PLED_BIT6_SHF	    	6
#define EVM3_PLED_BIT6_MSK	    	(1 << EVM3_PLED_BIT6_SHF)
#define EVM3_PLED_BIT6_ON           	EVM3_PLED_BIT6_MSK

/* bit 7:  */
#define EVM3_PLED_BIT7_SHF	    	7
#define EVM3_PLED_BIT7_MSK	    	(1 << EVM3_PLED_BIT7_SHF)
#define EVM3_PLED_BIT7_ON           	EVM3_PLED_BIT7_MSK

#endif /* !(_MIPS_EVM3_H) */






