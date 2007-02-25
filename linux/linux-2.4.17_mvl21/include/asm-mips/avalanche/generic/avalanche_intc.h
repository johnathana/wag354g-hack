 /*
 * Nitin Dhingra, iamnd@ti.com
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
#ifndef _AVALANCHE_INTC_H
#define _AVALANCHE_INTC_H

#define MIPS_EXCEPTION_OFFSET 8

#include <asm/avalanche/avalanche_map.h>

/******************************************************************************
 Avalanche Interrupt number
******************************************************************************/
#define AVINTNUM(x) ((x) - MIPS_EXCEPTION_OFFSET)

/*******************************************************************************
*Linux Interrupt number
*******************************************************************************/
#define LNXINTNUM(x)((x) + MIPS_EXCEPTION_OFFSET)



#define AVALANCHE_INT_END_PRIMARY      (40 + MIPS_EXCEPTION_OFFSET)
#define AVALANCHE_INT_END_SECONDARY    (32 + AVALANCHE_INT_END_PRIMARY)

#define AVALANCHE_INT_END_PRIMARY_REG1 32
#define AVALANCHE_INT_END_PRIMARY_REG2 40


#if 0
#define AVALANCHE_INTC_END (AVINTNUM(AVALANCHE_INT_END_PRIMARY) + \
			    AVINTNUM(AVALANCHE_INT_END_SECONDARY)  \
                                    + MIPS_EXCEPTION_OFFSET) 
#endif

#define AVALANCHE_INTC_END AVALANCHE_INT_END_SECONDARY

#if defined(CONFIG_MIPS_AVALANCHE_VLYNQ)
#define AVALANCHE_INT_END_LOW_VLYNQ  (AVALANCHE_INTC_END + 32)
#define AVALANCHE_INT_END_VLYNQ      (AVALANCHE_INTC_END + 32 * CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS) 
#define AVALANCHE_INT_END             AVALANCHE_INT_END_VLYNQ
#else
#define AVALANCHE_INT_END             AVALANCHE_INTC_END
#endif

/*
 * Avalanche interrupt controller register base (primary)
 */
#define AVALANCHE_ICTRL_REGS_BASE  AVALANCHE_INTC_BASE

/******************************************************************************
 * Avalanche exception controller register base (secondary)
 ******************************************************************************/
#define AVALANCHE_ECTRL_REGS_BASE  (AVALANCHE_ICTRL_REGS_BASE + 0x80)


/******************************************************************************
 *  Avalanche Interrupt pacing register base (secondary)
 ******************************************************************************/
#define AVALANCHE_IPACE_REGS_BASE  (AVALANCHE_ICTRL_REGS_BASE + 0xA0)



/******************************************************************************
 * Avalanche Interrupt Channel Control register base
 *****************************************************************************/
#define AVALANCHE_CHCTRL_REGS_BASE (AVALANCHE_ICTRL_REGS_BASE + 0x200)


struct avalanche_ictrl_regs /* Avalanche Interrupt control registers */
{
  volatile unsigned long intsr1;    /* Interrupt Status/Set Register 1   0x00 */
  volatile unsigned long intsr2;    /* Interrupt Status/Set Register 2   0x04 */
  volatile unsigned long unused1;                                      /*0x08 */
  volatile unsigned long unused2;                                      /*0x0C */
  volatile unsigned long intcr1;    /* Interrupt Clear Register 1        0x10 */
  volatile unsigned long intcr2;    /* Interrupt Clear Register 2        0x14 */
  volatile unsigned long unused3;                                      /*0x18 */
  volatile unsigned long unused4;                                      /*0x1C */
  volatile unsigned long intesr1;   /* Interrupt Enable (Set) Register 1 0x20 */
  volatile unsigned long intesr2;   /* Interrupt Enable (Set) Register 2 0x24 */
  volatile unsigned long unused5;                                      /*0x28 */
  volatile unsigned long unused6;                                      /*0x2C */
  volatile unsigned long intecr1;   /* Interrupt Enable Clear Register 1 0x30 */
  volatile unsigned long intecr2;   /* Interrupt Enable Clear Register 2 0x34 */
  volatile unsigned long unused7;                                     /* 0x38 */
  volatile unsigned long unused8;                                     /* 0x3c */
  volatile unsigned long pintir;    /* Priority Interrupt Index Register 0x40 */
  volatile unsigned long intmsr;    /* Priority Interrupt Mask Index Reg 0x44 */
  volatile unsigned long unused9;                                     /* 0x48 */
  volatile unsigned long unused10;                                    /* 0x4C */
  volatile unsigned long intpolr1;  /* Interrupt Polarity Mask register 10x50 */
  volatile unsigned long intpolr2;  /* Interrupt Polarity Mask register 20x54 */
  volatile unsigned long unused11;                                    /* 0x58 */
  volatile unsigned long unused12;                                   /*0x5C */
  volatile unsigned long inttypr1;  /* Interrupt Type     Mask register 10x60 */
  volatile unsigned long inttypr2;  /* Interrupt Type     Mask register 20x64 */
};

struct avalanche_exctrl_regs   /* Avalanche Exception control registers */
{
  volatile unsigned long exsr;      /* Exceptions Status/Set register    0x80 */
  volatile unsigned long reserved;                                     /*0x84 */
  volatile unsigned long excr;      /* Exceptions Clear Register         0x88 */
  volatile unsigned long reserved1;                                    /*0x8c */
  volatile unsigned long exiesr;    /* Exceptions Interrupt Enable (set) 0x90 */
  volatile unsigned long reserved2;                                    /*0x94 */
  volatile unsigned long exiecr;    /* Exceptions Interrupt Enable(clear)0x98 */
};
struct avalanche_ipace_regs
{

  volatile unsigned long ipacep;    /* Interrupt pacing register         0xa0 */
  volatile unsigned long ipacemap;  /*Interrupt Pacing Map Register      0xa4 */
  volatile unsigned long ipacemax;  /*Interrupt Pacing Max Register      0xa8 */
};
struct avalanche_channel_int_number
{
  volatile unsigned long cintnr0;   /* Channel Interrupt Number Register0x200 */
  volatile unsigned long cintnr1;   /* Channel Interrupt Number Register0x204 */
  volatile unsigned long cintnr2;   /* Channel Interrupt Number Register0x208 */
  volatile unsigned long cintnr3;   /* Channel Interrupt Number Register0x20C */
  volatile unsigned long cintnr4;   /* Channel Interrupt Number Register0x210 */
  volatile unsigned long cintnr5;   /* Channel Interrupt Number Register0x214 */
  volatile unsigned long cintnr6;   /* Channel Interrupt Number Register0x218 */
  volatile unsigned long cintnr7;   /* Channel Interrupt Number Register0x21C */
  volatile unsigned long cintnr8;   /* Channel Interrupt Number Register0x220 */
  volatile unsigned long cintnr9;   /* Channel Interrupt Number Register0x224 */
  volatile unsigned long cintnr10;  /* Channel Interrupt Number Register0x228 */
  volatile unsigned long cintnr11;  /* Channel Interrupt Number Register0x22C */
  volatile unsigned long cintnr12;  /* Channel Interrupt Number Register0x230 */
  volatile unsigned long cintnr13;  /* Channel Interrupt Number Register0x234 */
  volatile unsigned long cintnr14;  /* Channel Interrupt Number Register0x238 */
  volatile unsigned long cintnr15;  /* Channel Interrupt Number Register0x23C */
  volatile unsigned long cintnr16;  /* Channel Interrupt Number Register0x240 */
  volatile unsigned long cintnr17;  /* Channel Interrupt Number Register0x244 */
  volatile unsigned long cintnr18;  /* Channel Interrupt Number Register0x248 */
  volatile unsigned long cintnr19;  /* Channel Interrupt Number Register0x24C */
  volatile unsigned long cintnr20;  /* Channel Interrupt Number Register0x250 */
  volatile unsigned long cintnr21;  /* Channel Interrupt Number Register0x254 */
  volatile unsigned long cintnr22;  /* Channel Interrupt Number Register0x358 */
  volatile unsigned long cintnr23;  /* Channel Interrupt Number Register0x35C */
  volatile unsigned long cintnr24;  /* Channel Interrupt Number Register0x260 */
  volatile unsigned long cintnr25;  /* Channel Interrupt Number Register0x264 */
  volatile unsigned long cintnr26;  /* Channel Interrupt Number Register0x268 */
  volatile unsigned long cintnr27;  /* Channel Interrupt Number Register0x26C */
  volatile unsigned long cintnr28;  /* Channel Interrupt Number Register0x270 */
  volatile unsigned long cintnr29;  /* Channel Interrupt Number Register0x274 */
  volatile unsigned long cintnr30;  /* Channel Interrupt Number Register0x278 */
  volatile unsigned long cintnr31;  /* Channel Interrupt Number Register0x27C */
  volatile unsigned long cintnr32;  /* Channel Interrupt Number Register0x280 */
  volatile unsigned long cintnr33;  /* Channel Interrupt Number Register0x284 */
  volatile unsigned long cintnr34;  /* Channel Interrupt Number Register0x288 */
  volatile unsigned long cintnr35;  /* Channel Interrupt Number Register0x28C */
  volatile unsigned long cintnr36;  /* Channel Interrupt Number Register0x290 */
  volatile unsigned long cintnr37;  /* Channel Interrupt Number Register0x294 */
  volatile unsigned long cintnr38;  /* Channel Interrupt Number Register0x298 */
  volatile unsigned long cintnr39;  /* Channel Interrupt Number Register0x29C */
};

struct avalanche_interrupt_line_to_channel
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


/* Interrupt Line #'s  (Sangam peripherals) */

extern void avalanche_int_set(int channel, int line);

#if defined(CONFIG_AVALANCHE_INTC_PACING)
int avalanche_request_intr_pacing(int irq_nr, unsigned int blk_num,
		             unsigned int pace_value);
int avalanche_free_intr_pacing(unsigned int blk_num);
#endif

#if defined(CONFIG_AVALANCHE_INTC_TYPE)
int avalanche_intr_type_set(unsigned int irq_nr, unsigned long type_val);
int avalanche_intr_type_get(unsigned int irq_nr);
#endif

int avalanche_intr_polarity_set(unsigned int irq_nr, unsigned long type_val);
int avalanche_intr_polarity_get(unsigned int irq_nr);


#endif /* _AVALANCHE_INTC_H */ *



