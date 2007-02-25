/* $Id: seadint.h,v 1.1.1.2 2005/03/28 06:56:48 sparq Exp $
 *
 * seadint.h
 *
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999 MIPS Technologies, Inc.
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
 * Defines for the Sead interrupt controller.
 *
 */
#ifndef _MIPS_SEADINT_H
#define _MIPS_SEADINT_H

#include <linux/config.h>

/* Avalanche Interrupt number */
#define AVINTNUM(x) ((x) - MIPS_EXCEPTION_OFFSET)
/* Linux Interrupt number     */ 
#define LNXINTNUM(x)((x) + MIPS_EXCEPTION_OFFSET)
/* Number of IRQ supported on hw interrupt 0. */

//#define SEADINT_UART0      3            /* TTYS0 interrupt on SEAD */
//#define SEADINT_UART1      4            /* TTYS1 interrupt on SEAD */


#define MIPS_EXCEPTION_OFFSET 8
#define SEADINT_END_PRIMARY      (40 + MIPS_EXCEPTION_OFFSET)
#define SEADINT_END_SECONDARY    (32 + MIPS_EXCEPTION_OFFSET)

#define SEADINT_END_PRIMARY_REG1 (31 + MIPS_EXCEPTION_OFFSET)
#define SEADINT_END_PRIMARY_REG2 (39 + MIPS_EXCEPTION_OFFSET)


#define SEADINT_END (AVINTNUM(SEADINT_END_PRIMARY) + AVINTNUM(SEADINT_END_SECONDARY)  \
                     + MIPS_EXCEPTION_OFFSET - 1)

/* 
 * Sead registers are memory mapped on 64-bit aligned boundaries and 
 * only word access are allowed.
 */
//struct sead_ictrl_regs {
//        volatile unsigned long intraw;
//        long dummy1;
//        volatile unsigned long intseten;
//        long dummy2;
//        volatile unsigned long intrsten;
//        long dummy3;
//        volatile unsigned long intenable;
//        long dummy4;
//        volatile unsigned long intstatus;
//        long dummy5;
//};

struct sead_ictrl_regs /* Avalanche Interrupt control registers */
{
  volatile unsigned long intsr1;    /* Interrupt Status/Set Register 1   */ /* 0x00 */      
  volatile unsigned long intsr2;    /* Interrupt Status/Set Register 2   */ /* 0x04 */
  volatile unsigned long unused1;                                           /* 0x08 */
  volatile unsigned long unused2;                                           /* 0x0C */
  volatile unsigned long intcr1;    /* Interrupt Clear Register 1        */ /* 0x10 */
  volatile unsigned long intcr2;    /* Interrupt Clear Register 2        */ /* 0x14 */
  volatile unsigned long unused3;                                           /* 0x18 */
  volatile unsigned long unused4;                                           /* 0x1C */
  volatile unsigned long intesr1;   /* Interrupt Enable (Set) Register 1 */ /* 0x20 */
  volatile unsigned long intesr2;   /* Interrupt Enable (Set) Register 2 */ /* 0x24 */
  volatile unsigned long unused5;                                           /* 0x28 */
  volatile unsigned long unused6;                                           /* 0x2C */
  volatile unsigned long intecr1;   /* Interrupt Enable Clear Register 1 */ /* 0x30 */
  volatile unsigned long intecr2;   /* Interrupt Enable Clear Register 2 */ /* 0x34 */
  volatile unsigned long unused7;                                           /* 0x38 */
  volatile unsigned long unused8;                                           /* 0x3c */
  volatile unsigned long pintir;    /* Priority Interrupt Index Register */ /* 0x40 */
  volatile unsigned long intmsr;    /* Priority Interrupt Mask Index Reg.*/ /* 0x44 */
  volatile unsigned long unused9;                                           /* 0x48 */
  volatile unsigned long unused10;                                          /* 0x4C */
  volatile unsigned long intpolr1;  /* Interrupt Polarity Mask register 1*/ /* 0x50 */
  volatile unsigned long intpolr2;  /* Interrupt Polarity Mask register 2*/ /* 0x54 */
};

struct sead_exctrl_regs   /* Avalanche Exception control registers */
{
  volatile unsigned long exsr;      /* Exceptions Status/Set register    */  /* 0x80 */
  volatile unsigned long reserved;                                           /* 0x84 */
  volatile unsigned long excr;      /* Exceptions Clear Register         */  /* 0x88 */
  volatile unsigned long reserved1;                                          /* 0x8c */
  volatile unsigned long exiesr;    /* Exceptions Interrupt Enable (set) */  /* 0x90 */
  volatile unsigned long reserved2;                                          /* 0x94 */
  volatile unsigned long exiecr;    /* Exceptions Interrupt Enable (clear)*/ /* 0x98 */
};

struct sead_channel_int_number
{
  volatile unsigned long cintnr0;   /* Channel Interrupt Number Register */  /* 0x200 */
  volatile unsigned long cintnr1;   /* Channel Interrupt Number Register */  /* 0x204 */
  volatile unsigned long cintnr2;   /* Channel Interrupt Number Register */  /* 0x208 */
  volatile unsigned long cintnr3;   /* Channel Interrupt Number Register */  /* 0x20C */
  volatile unsigned long cintnr4;   /* Channel Interrupt Number Register */  /* 0x210 */
  volatile unsigned long cintnr5;   /* Channel Interrupt Number Register */  /* 0x214 */
  volatile unsigned long cintnr6;   /* Channel Interrupt Number Register */  /* 0x218 */
  volatile unsigned long cintnr7;   /* Channel Interrupt Number Register */  /* 0x21C */
  volatile unsigned long cintnr8;   /* Channel Interrupt Number Register */  /* 0x220 */
  volatile unsigned long cintnr9;   /* Channel Interrupt Number Register */  /* 0x224 */
  volatile unsigned long cintnr10;  /* Channel Interrupt Number Register */  /* 0x228 */
  volatile unsigned long cintnr11;  /* Channel Interrupt Number Register */  /* 0x22C */
  volatile unsigned long cintnr12;  /* Channel Interrupt Number Register */  /* 0x230 */
  volatile unsigned long cintnr13;  /* Channel Interrupt Number Register */  /* 0x234 */
  volatile unsigned long cintnr14;  /* Channel Interrupt Number Register */  /* 0x238 */
  volatile unsigned long cintnr15;  /* Channel Interrupt Number Register */  /* 0x23C */
  volatile unsigned long cintnr16;  /* Channel Interrupt Number Register */  /* 0x240 */
  volatile unsigned long cintnr17;  /* Channel Interrupt Number Register */  /* 0x244 */
  volatile unsigned long cintnr18;  /* Channel Interrupt Number Register */  /* 0x248 */
  volatile unsigned long cintnr19;  /* Channel Interrupt Number Register */  /* 0x24C */
  volatile unsigned long cintnr20;  /* Channel Interrupt Number Register */  /* 0x250 */
  volatile unsigned long cintnr21;  /* Channel Interrupt Number Register */  /* 0x254 */
  volatile unsigned long cintnr22;  /* Channel Interrupt Number Register */  /* 0x358 */
  volatile unsigned long cintnr23;  /* Channel Interrupt Number Register */  /* 0x35C */
  volatile unsigned long cintnr24;  /* Channel Interrupt Number Register */  /* 0x260 */
  volatile unsigned long cintnr25;  /* Channel Interrupt Number Register */  /* 0x264 */
  volatile unsigned long cintnr26;  /* Channel Interrupt Number Register */  /* 0x268 */
  volatile unsigned long cintnr27;  /* Channel Interrupt Number Register */  /* 0x26C */
  volatile unsigned long cintnr28;  /* Channel Interrupt Number Register */  /* 0x270 */
  volatile unsigned long cintnr29;  /* Channel Interrupt Number Register */  /* 0x274 */
  volatile unsigned long cintnr30;  /* Channel Interrupt Number Register */  /* 0x278 */
  volatile unsigned long cintnr31;  /* Channel Interrupt Number Register */  /* 0x27C */
  volatile unsigned long cintnr32;  /* Channel Interrupt Number Register */  /* 0x280 */
  volatile unsigned long cintnr33;  /* Channel Interrupt Number Register */  /* 0x284 */
  volatile unsigned long cintnr34;  /* Channel Interrupt Number Register */  /* 0x288 */
  volatile unsigned long cintnr35;  /* Channel Interrupt Number Register */  /* 0x28C */
  volatile unsigned long cintnr36;  /* Channel Interrupt Number Register */  /* 0x290 */
  volatile unsigned long cintnr37;  /* Channel Interrupt Number Register */  /* 0x294 */
  volatile unsigned long cintnr38;  /* Channel Interrupt Number Register */  /* 0x298 */
  volatile unsigned long cintnr39;  /* Channel Interrupt Number Register */  /* 0x29C */
};

struct sead_interrupt_line_to_channel 
{
  unsigned long int_line0;    /* Start of primary interrupts */
  unsigned long int_line1;
  unsigned long int_line2;
  unsigned long int_line3;
  unsigned long int_line4;
  unsigned long int_line5;
  unsigned long int_line6;
  unsigned long int_line7;
  unsigned long int_line8;
  unsigned long int_line9;
  unsigned long int_line10;
  unsigned long int_line11; 
  unsigned long int_line12; 
  unsigned long int_line13; 
  unsigned long int_line14; 
  unsigned long int_line15; 
  unsigned long int_line16; 
  unsigned long int_line17; 
  unsigned long int_line18; 
  unsigned long int_line19; 
  unsigned long int_line20; 
  unsigned long int_line21;
  unsigned long int_line22; 
  unsigned long int_line23; 
  unsigned long int_line24; 
  unsigned long int_line25; 
  unsigned long int_line26; 
  unsigned long int_line27; 
  unsigned long int_line28; 
  unsigned long int_line29; 
  unsigned long int_line30; 
  unsigned long int_line31; 
  unsigned long int_line32;
  unsigned long int_line33; 
  unsigned long int_line34; 
  unsigned long int_line35; 
  unsigned long int_line36; 
  unsigned long int_line37; 
  unsigned long int_line38; 
  unsigned long int_line39;  
};

/* Interrupt Line #'s  (Avalanche peripherals) */

/*------------------------------*/
/* Avalanche primary interrupts */
/*------------------------------*/
#define UNIFIED_SECONDARY_INTERRUPT  0
#define AVALANCHE_EXT_INT_0          1
#define AVALANCHE_EXT_INT_1          2
#define AVALANCHE_EXT_INT_2          3
#define AVALANCHE_EXT_INT_3          4
#define AVALANCHE_TIMER_0_INT        5
#define AVALANCHE_TIMER_1_INT        6
#define AVALANCHE_UART0_INT          7
#define AVALANCHE_UART1_INT          8
#define AVALANCHE_PDMA_INT0          9
#define AVALANCHE_PDMA_INT1          10
#define AVALANCHE_HDLC_TXA           11
#define AVALANCHE_HDLC_TXB           12
#define AVALANCHE_HDLC_RXA           13
#define AVALANCHE_HDLC_RXB           14
#define AVALANCHE_ATM_SAR_TXA        15
#define AVALANCHE_ATM_SAR_TXB        16
#define AVALANCHE_ATM_SAR_RXA        17
#define AVALANCHE_ATM_SAR_RXB        18
#define AVALANCHE_MAC_TXA            19
#define AVALANCHE_MAC_RXA            20
#define AVALANCHE_DSP_SUB0           21
#define AVALANCHE_DSP_SUB1           22
#define AVALANCHE_DES_INT            23
#define AVALANCHE_USB_INT            24
#define AVALANCHE_PCI_INTA           25
#define AVALANCHE_PCI_INTB           26
#define AVALANCHE_PCI_INTC           27
/*  Line #28  Reserved               */
#define AVALANCHE_I2CM_INT           29
#define AVALANCHE_PDMA_INT2          30
#define AVALANCHE_PDMA_INT3          31
#define AVALANCHE_CODEC              32
#define AVALANCHE_MAC_TXB            33
#define AVALANCHE_MAC_RXB            34
/*  Line #35  Reserved               */
/*  Line #36  Reserved               */
/*  Line #37  Reserved               */
/*  Line #38  Reserved               */
/*  Line #39  Reserved               */

/*-----------------------------------*/
/* Avalanche Secondary Interrupts    */
/*-----------------------------------*/
#define PRIMARY_INTS                 40

#define AVALANCHE_HDLC_STATUS        (0 + PRIMARY_INTS)
#define AVALANCHE_SAR_STATUS         (1 + PRIMARY_INTS)
/*  Line #02  Reserved               */
#define AVALANCHE_ETH_MACA_LNK_CHG   (3 + PRIMARY_INTS)
#define AVALANCHE_ETH_MACA_MGT       (4 + PRIMARY_INTS)
#define AVALANCHE_PCI_STATUS_INT     (5 + PRIMARY_INTS)
/*  Line #06  Reserved               */
#define AVALANCHE_EXTERN_MEM_INT     (7 + PRIMARY_INTS)
#define AVALANCHE_DSP_A_DOG          (8 + PRIMARY_INTS)
#define AVALANCHE_DSP_B_DOG          (9 + PRIMARY_INTS)
/*  Line #10-#20 Reserved            */
#define AVALANCHE_ETH_MACB_LNK_CHG   (21 + PRIMARY_INTS)
#define AVALANCHE_ETH_MACB_MGT       (22 + PRIMARY_INTS)
#define AVALANCHE_AAL2_STATUS        (23 + PRIMARY_INTS)
/*  Line #24-#31 Reserved            */

#define AVALANCHEINT_UART0        LNXINTNUM(AVALANCHE_UART0_INT)
#define AVALANCHEINT_UART1        LNXINTNUM(AVALANCHE_UART1_INT)
#define SEADINT_UART0    3   /* TTYS0 interrupt on SEAD */
#define SEADINT_UART1    4   /* TTYS1 interrupt on SEAD */

//#define JIMK_INT_CTRLR

#ifdef JIMK_INT_CTRLR
/*-----------------------------------*/
/* Jim Kennedy's Interrupt Controller*/
/*-----------------------------------*/

/* to clear the interrupt write the bit back to the status reg */

#define JIMK_INT_STATUS (*(volatile unsigned int *)(0xA8612400))
#define JIMK_INT_MASK   (*(volatile unsigned int *)(0xA8612404))
//#define JIMK_SAR_STATUS  (1<<0)
//#define JIMK_SAR_TX_A    (1<<1)
//#define JIMK_SAR_TX_B    (1<<2)
//#define JIMK_SAR_RX_A    (1<<3)
//#define JIMK_SAR_RX_B    (1<<4)
//#define JIMK_AAL2_STATUS (1<<5)
//#define JIMK_UART0_INT   (1<<11)


#define JIMK_EMAC0_TX      (1<<0)
#define JIMK_EMAC0_RX      (1<<1)
#define JIMK_EMAC0_STATUS  (1<<2)
#define JIMK_EMAC0_LINK    (1<<3)
#define JIMK_EMAC0_MDIO    (1<<4)
#define JIMK_EMAC1_TX      (1<<5)
#define JIMK_EMAC1_RX      (1<<6)
#define JIMK_EMAC1_STATUS  (1<<7)
#define JIMK_EMAC1_LINK    (1<<8)
#define JIMK_EMAC1_MDIO    (1<<9)
#define JIMK_DMA_0         (1<<10)
#define JIMK_DMA_1         (1<<11)
#define JIMK_DMA_2         (1<<12)
#define JIMK_DMA_3         (1<<13)
#define JIMK_I2C           (1<<14)
#define JIMK_UART1_INT     (1<<15)



 

#ifdef SEAD_USB_DEVELOPMENT
#define JIMK_USB_INT     (1<<0)
#endif /* SEAD_USB_DEVELOPMENT */

#endif /* JIMK_INT_CTRLR */

extern void avalanche_int_set(int channel, int line);
extern void seadint_init(void);


#endif /* !(_MIPS_SEADINT_H) */






