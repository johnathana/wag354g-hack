/******************************************************************************
 * FILE PURPOSE:    TNETD73xx Common Header File
 ******************************************************************************
 * FILE NAME:       tnetd73xx.h
 *
 * DESCRIPTION:     shared typedef's, constants and API for TNETD73xx
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

/*
 *
 *
 *   These are const, typedef, and api definitions for tnetd73xx.
 *
 *   NOTES:
 *   1. This file may be included into both C and Assembly files.
 *       - for .s files, please do #define _ASMLANGUAGE in your ASM file to
 *           avoid C data types (typedefs) below;
 *       - for .c files, you don't have to do anything special.
 *
 *   2. This file has a number of sections for each SOC subsystem. When adding
 *       a new constant, find the subsystem you are working on and follow the
 *       name pattern. If you are adding another typedef for your interface, please,
 *       place it with other typedefs and function prototypes.
 *
 *   3. Please, DO NOT add any macros or types that are local to a subsystem to avoid
 *       cluttering. Include such items directly into the module's .c file or have a
 *       local .h file to pass data between smaller modules. This file defines only
 *       shared items.
 */

#ifndef __TNETD73XX_H__
#define __TNETD73XX_H__

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#ifndef _ASMLANGUAGE        /* This part not for assembly language */

extern unsigned int tnetd73xx_mips_freq;
extern unsigned int tnetd73xx_vbus_freq;

#include "tnetd73xx_err.h"

#endif /* _ASMLANGUAGE */


/*******************************************************************************************
*   Emerald core specific
******************************************************************************************** */

#ifdef  BIG_ENDIAN
#elif defined(LITTLE_ENDIAN)
#else
#error Need to define endianism
#endif

#ifndef KSEG_MSK
#define KSEG_MSK                    0xE0000000 /* Most significant 3 bits denote kseg choice */
#endif

#ifndef KSEG_INV_MASK
#define KSEG_INV_MASK               0x1FFFFFFF /* Inverted mask for kseg address */
#endif

#ifndef KSEG0_BASE
#define KSEG0_BASE                  0x80000000
#endif

#ifndef KSEG1_BASE
#define KSEG1_BASE                  0xA0000000
#endif

#ifndef KSEG0
#define KSEG0(addr)                 (((UINT32)(addr) & ~KSEG_MSK) | KSEG0_BASE)
#endif

#ifndef KSEG1
#define KSEG1(addr)                 (((UINT32)(addr) & ~KSEG_MSK) | KSEG1_BASE)
#endif

#ifndef KUSEG
#define KUSEG(addr)                 ((UINT32)(addr) & ~KSEG_MSK)
#endif

#ifndef PHYS_ADDR
#define PHYS_ADDR(addr)             ((addr) & KSEG_INV_MASK)
#endif

#ifndef PHYS_TO_K0
#define PHYS_TO_K0(addr)            (PHYS_ADDR(addr)|KSEG0_BASE)
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(addr)            (PHYS_ADDR(addr)|KSEG1_BASE)
#endif

#ifndef REG8_ADDR
#define REG8_ADDR(addr)             (volatile UINT8 *)(PHYS_TO_K1(addr))
#define REG8_DATA(addr)             (*(volatile UINT8 *)(PHYS_TO_K1(addr)))
#define REG8_WRITE(addr, data)      REG8_DATA(addr) = data;
#define REG8_READ(addr, data)       data = (UINT8) REG8_DATA(addr);
#endif

#ifndef REG16_ADDR
#define REG16_ADDR(addr)            (volatile UINT16 *)(PHYS_TO_K1(addr))
#define REG16_DATA(addr)            (*(volatile UINT16 *)(PHYS_TO_K1(addr)))
#define REG16_WRITE(addr, data)     REG16_DATA(addr) = data;
#define REG16_READ(addr, data)      data = (UINT16) REG16_DATA(addr);
#endif

#ifndef REG32_ADDR
#define REG32_ADDR(addr)            (volatile UINT32 *)(PHYS_TO_K1(addr))
#define REG32_DATA(addr)            (*(volatile UINT32 *)(PHYS_TO_K1(addr)))
#define REG32_WRITE(addr, data)     REG32_DATA(addr) = data;
#define REG32_READ(addr, data)      data = (UINT32) REG32_DATA(addr);
#endif

#ifdef  _LINK_KSEG0_                /* Application is linked into KSEG0 space */
#define VIRT_ADDR(addr)             PHYS_TO_K0(PHYS_ADDR(addr))
#endif

#ifdef  _LINK_KSEG1_                /* Application is linked into KSEG1 space */
#define VIRT_ADDR(addr)             PHYS_TO_K1(PHYS_ADDR(addr))
#endif

#if !defined(_LINK_KSEG0_) && !defined(_LINK_KSEG1_)
#error  You must define _LINK_KSEG0_ or _LINK_KSEG1_ to compile the code.
#endif

/* TNETD73XX chip definations */

#define FREQ_1MHZ                       1000000
#define TNETD73XX_MIPS_FREQ             tnetd73xx_mips_freq /* CPU clock frequency */
#define TNETD73XX_VBUS_FREQ             tnetd73xx_vbus_freq /* originally (TNETD73XX_MIPS_FREQ/2) */

#ifdef AR7SEAD2
#define TNETD73XX_MIPS_FREQ_DEFAULT     25000000       /* 25 Mhz for sead2 board crystal */
#else
#define TNETD73XX_MIPS_FREQ_DEFAULT     125000000      /* 125 Mhz */
#endif
#define TNETD73XX_VBUS_FREQ_DEFAULT     (TNETD73XX_MIPS_FREQ_DEFAULT / 2) /* Sync mode */



/* Module base addresses */
#define TNETD73XX_ADSLSS_BASE               PHYS_TO_K1(0x01000000)      /* ADSLSS Module */
#define TNETD73XX_BBIF_CTRL_BASE            PHYS_TO_K1(0x02000000)      /* BBIF Control */
#define TNETD73XX_ATMSAR_BASE               PHYS_TO_K1(0x03000000)      /* ATM SAR */
#define TNETD73XX_USB_BASE                  PHYS_TO_K1(0x03400000)      /* USB Module */
#define TNETD73XX_VLYNQ0_BASE               PHYS_TO_K1(0x04000000)      /* VLYNQ0 Module */
#define TNETD73xx_EMAC0_BASE                PHYS_TO_K1(0x08610000)      /* EMAC0 Module*/
#define TNETD73XX_EMIF_BASE                 PHYS_TO_K1(0x08610800)      /* EMIF Module */
#define TNETD73XX_GPIO_BASE                 PHYS_TO_K1(0x08610900)      /* GPIO control */
#define TNETD73XX_CLOCK_CTRL_BASE           PHYS_TO_K1(0x08610A00)      /* Clock Control */
#define TNETD73XX_WDTIMER_BASE              PHYS_TO_K1(0x08610B00)      /* WDTIMER Module */
#define TNETD73XX_TIMER0_BASE               PHYS_TO_K1(0x08610C00)      /* TIMER0 Module */
#define TNETD73XX_TIMER1_BASE               PHYS_TO_K1(0x08610D00)      /* TIMER1 Module */
#define TNETD73XX_UARTA_BASE                PHYS_TO_K1(0x08610E00)      /* UART A */
#define TNETD73XX_UARTB_BASE                PHYS_TO_K1(0x08610F00)      /* UART B */
#define TNETD73XX_I2C_BASE                  PHYS_TO_K1(0x08611000)      /* I2C Module */
#define TNETD73XX_USB_DMA_BASE              PHYS_TO_K1(0x08611200)      /* USB Module */
#define TNETD73XX_MCDMA_BASE                PHYS_TO_K1(0x08611400)      /* MC-DMA */
#define TNETD73xx_VDMAVT_BASE               PHYS_TO_K1(0x08611500)      /* VDMAVT Control */
#define TNETD73XX_RST_CTRL_BASE             PHYS_TO_K1(0x08611600)      /* Reset Control */
#define TNETD73xx_BIST_CTRL_BASE            PHYS_TO_K1(0x08611700)      /* BIST Control */
#define TNETD73xx_VLYNQ0_CTRL_BASE          PHYS_TO_K1(0x08611800)      /* VLYNQ0 Control */
#define TNETD73XX_DCL_BASE                  PHYS_TO_K1(0x08611A00)      /* Device Configuration Latch */
#define TNETD73xx_VLYNQ1_CTRL_BASE          PHYS_TO_K1(0x08611C00)      /* VLYNQ1 Control */
#define TNETD73xx_MDIO_BASE                 PHYS_TO_K1(0x08611E00)      /* MDIO Control */
#define TNETD73XX_FSER_BASE                 PHYS_TO_K1(0x08612000)      /* FSER Control */
#define TNETD73XX_INTC_BASE                 PHYS_TO_K1(0x08612400)      /* Interrupt Controller */
#define TNETD73xx_EMAC1_BASE                PHYS_TO_K1(0x08612800)      /* EMAC1 Module*/
#define TNETD73XX_VLYNQ1_BASE               PHYS_TO_K1(0x0C000000)      /* VLYNQ1 Module */

/* BBIF Registers */
#define TNETD73XX_BBIF_ADSLADR              (TNETD73XX_BBIF_CTRL_BASE + 0x0)

/* Device Configuration Latch Registers */
#define TNETD73XX_DCL_BOOTCR                (TNETD73XX_DCL_BASE + 0x0)
#define TNETD73XX_DCL_DPLLSELR              (TNETD73XX_DCL_BASE + 0x10)
#define TNETD73XX_DCL_SPEEDCTLR             (TNETD73XX_DCL_BASE + 0x14)
#define TNETD73XX_DCL_SPEEDPWDR             (TNETD73XX_DCL_BASE + 0x18)
#define TNETD73XX_DCL_SPEEDCAPR             (TNETD73XX_DCL_BASE + 0x1C)

/* GPIO Control */
#define TNETD73XX_GPIODINR                  (TNETD73XX_GPIO_BASE + 0x0)
#define TNETD73XX_GPIODOUTR                 (TNETD73XX_GPIO_BASE + 0x4)
#define TNETD73XX_GPIOPDIRR                 (TNETD73XX_GPIO_BASE + 0x8)
#define TNETD73XX_GPIOENR                   (TNETD73XX_GPIO_BASE + 0xC)
#define TNETD73XX_CVR                       (TNETD73XX_GPIO_BASE + 0x14)
#define TNETD73XX_DIDR1                     (TNETD73XX_GPIO_BASE + 0x18)
#define TNETD73XX_DIDR2                     (TNETD73XX_GPIO_BASE + 0x1C)

/* Reset Control  */
#define TNETD73XX_RST_CTRL_PRCR             (TNETD73XX_RST_CTRL_BASE + 0x0)
#define TNETD73XX_RST_CTRL_SWRCR            (TNETD73XX_RST_CTRL_BASE + 0x4)
#define TNETD73XX_RST_CTRL_RSR              (TNETD73XX_RST_CTRL_BASE + 0x8)

/* Power Control  */
#define TNETD73XX_POWER_CTRL_PDCR           (TNETD73XX_CLOCK_CTRL_BASE + 0x0)
#define TNETD73XX_POWER_CTRL_PCLKCR         (TNETD73XX_CLOCK_CTRL_BASE + 0x4)
#define TNETD73XX_POWER_CTRL_PDUCR          (TNETD73XX_CLOCK_CTRL_BASE + 0x8)
#define TNETD73XX_POWER_CTRL_WKCR           (TNETD73XX_CLOCK_CTRL_BASE + 0xC)

/* Clock Control */
#define TNETD73XX_CLK_CTRL_SCLKCR           (TNETD73XX_CLOCK_CTRL_BASE + 0x20)
#define TNETD73XX_CLK_CTRL_SCLKPLLCR        (TNETD73XX_CLOCK_CTRL_BASE + 0x30)
#define TNETD73XX_CLK_CTRL_MCLKCR           (TNETD73XX_CLOCK_CTRL_BASE + 0x40)
#define TNETD73XX_CLK_CTRL_MCLKPLLCR        (TNETD73XX_CLOCK_CTRL_BASE + 0x50)
#define TNETD73XX_CLK_CTRL_UCLKCR           (TNETD73XX_CLOCK_CTRL_BASE + 0x60)
#define TNETD73XX_CLK_CTRL_UCLKPLLCR        (TNETD73XX_CLOCK_CTRL_BASE + 0x70)
#define TNETD73XX_CLK_CTRL_ACLKCR0          (TNETD73XX_CLOCK_CTRL_BASE + 0x80)
#define TNETD73XX_CLK_CTRL_ACLKPLLCR0       (TNETD73XX_CLOCK_CTRL_BASE + 0x90)
#define TNETD73XX_CLK_CTRL_ACLKCR1          (TNETD73XX_CLOCK_CTRL_BASE + 0xA0)
#define TNETD73XX_CLK_CTRL_ACLKPLLCR1       (TNETD73XX_CLOCK_CTRL_BASE + 0xB0)

/* EMIF control */
#define TNETD73XX_EMIF_SDRAM_CFG              ( TNETD73XX_EMIF_BASE + 0x08 )                

/* UART */
#ifdef AR7SEAD2
#define TNETD73XX_UART_FREQ                 3686400
#else
#define TNETD73XX_UART_FREQ                 TNETD73XX_VBUS_FREQ
#endif

/* Interrupt Controller */

/* Primary interrupts */
#define TNETD73XX_INTC_UNIFIED_SECONDARY    0   /* Unified secondary interrupt */
#define TNETD73XX_INTC_EXTERNAL0            1   /* External Interrupt Line 0 */
#define TNETD73XX_INTC_EXTERNAL1            2   /* External Interrupt Line 1 */
#define TNETD73XX_INTC_RESERVED3            3   /* Reserved */
#define TNETD73XX_INTC_RESERVED4            4   /* Reserved */
#define TNETD73XX_INTC_TIMER0               5   /* TIMER 0 int */
#define TNETD73XX_INTC_TIMER1               6   /* TIMER 1 int */
#define TNETD73XX_INTC_UART0                7   /* UART 0 int */
#define TNETD73XX_INTC_UART1                8   /* UART 1 int */
#define TNETD73XX_INTC_MCDMA0               9   /* MCDMA 0 int */
#define TNETD73XX_INTC_MCDMA1               10  /* MCDMA 1 int */
#define TNETD73XX_INTC_RESERVED11           11  /* Reserved */
#define TNETD73XX_INTC_RESERVED12           12  /* Reserved */
#define TNETD73XX_INTC_RESERVED13           13  /* Reserved */
#define TNETD73XX_INTC_RESERVED14           14  /* Reserved */
#define TNETD73XX_INTC_ATMSAR               15  /* ATM SAR int */
#define TNETD73XX_INTC_RESERVED16           16  /* Reserved */
#define TNETD73XX_INTC_RESERVED17           17  /* Reserved */
#define TNETD73XX_INTC_RESERVED18           18  /* Reserved */
#define TNETD73XX_INTC_EMAC0                19  /* EMAC 0 int */
#define TNETD73XX_INTC_RESERVED20           20  /* Reserved */
#define TNETD73XX_INTC_VLYNQ0               21  /* VLYNQ 0 int */
#define TNETD73XX_INTC_CODEC                22  /* CODEC int */
#define TNETD73XX_INTC_RESERVED23           23  /* Reserved */
#define TNETD73XX_INTC_USBSLAVE             24  /* USB Slave int */
#define TNETD73XX_INTC_VLYNQ1               25  /* VLYNQ 1 int */
#define TNETD73XX_INTC_RESERVED26           26  /* Reserved */
#define TNETD73XX_INTC_RESERVED27           27  /* Reserved */
#define TNETD73XX_INTC_ETH_PHY              28  /* Ethernet PHY   */
#define TNETD73XX_INTC_I2C                  29  /* I2C int */
#define TNETD73XX_INTC_MCDMA2               30  /* MCDMA 2 int */
#define TNETD73XX_INTC_MCDMA3               31  /* MCDMA 3 int */
#define TNETD73XX_INTC_RESERVED32           32  /* Reserved */
#define TNETD73XX_INTC_EMAC1                33  /* EMAC 1 int */
#define TNETD73XX_INTC_RESERVED34           34  /* Reserved */
#define TNETD73XX_INTC_RESERVED35           35  /* Reserved */
#define TNETD73XX_INTC_RESERVED36           36  /* Reserved */
#define TNETD73XX_INTC_VDMAVTRX             37  /* VDMAVTRX */
#define TNETD73XX_INTC_VDMAVTTX             38  /* VDMAVTTX */
#define TNETD73XX_INTC_ADSLSS               39  /* ADSLSS */

/* Secondary interrupts */
#define TNETD73XX_INTC_SEC0                 40  /* Secondary */
#define TNETD73XX_INTC_SEC1                 41  /* Secondary */
#define TNETD73XX_INTC_SEC2                 42  /* Secondary */
#define TNETD73XX_INTC_SEC3                 43  /* Secondary */
#define TNETD73XX_INTC_SEC4                 44  /* Secondary */
#define TNETD73XX_INTC_SEC5                 45  /* Secondary */
#define TNETD73XX_INTC_SEC6                 46  /* Secondary */
#define TNETD73XX_INTC_EMIF                 47  /* EMIF */
#define TNETD73XX_INTC_SEC8                 48  /* Secondary */
#define TNETD73XX_INTC_SEC9                 49  /* Secondary */
#define TNETD73XX_INTC_SEC10                50  /* Secondary */
#define TNETD73XX_INTC_SEC11                51  /* Secondary */
#define TNETD73XX_INTC_SEC12                52  /* Secondary */
#define TNETD73XX_INTC_SEC13                53  /* Secondary */
#define TNETD73XX_INTC_SEC14                54  /* Secondary */
#define TNETD73XX_INTC_SEC15                55  /* Secondary */
#define TNETD73XX_INTC_SEC16                56  /* Secondary */
#define TNETD73XX_INTC_SEC17                57  /* Secondary */
#define TNETD73XX_INTC_SEC18                58  /* Secondary */
#define TNETD73XX_INTC_SEC19                59  /* Secondary */
#define TNETD73XX_INTC_SEC20                60  /* Secondary */
#define TNETD73XX_INTC_SEC21                61  /* Secondary */
#define TNETD73XX_INTC_SEC22                62  /* Secondary */
#define TNETD73XX_INTC_SEC23                63  /* Secondary */
#define TNETD73XX_INTC_SEC24                64  /* Secondary */
#define TNETD73XX_INTC_SEC25                65  /* Secondary */
#define TNETD73XX_INTC_SEC26                66  /* Secondary */
#define TNETD73XX_INTC_SEC27                67  /* Secondary */
#define TNETD73XX_INTC_SEC28                68  /* Secondary */
#define TNETD73XX_INTC_SEC29                69  /* Secondary */
#define TNETD73XX_INTC_SEC30                70  /* Secondary */
#define TNETD73XX_INTC_SEC31                71  /* Secondary */

/* These ugly macros are to access the -1 registers, like config1 */
#define MFC0_SEL1_OPCODE(dst, src)\
        .word (0x40000000 | ((dst)<<16) | ((src)<<11) | 1);\
        nop; \
        nop; \
        nop

#define MTC0_SEL1_OPCODE(dst, src)\
        .word (0x40800000 | ((dst)<<16) | ((src)<<11) | 1);\
        nop; \
        nop; \
        nop


/* Below are Jade core specific */
#define CFG0_4K_IL_MASK         0x00380000
#define CFG0_4K_IL_SHIFT        19
#define CFG0_4K_IA_MASK         0x00070000
#define CFG0_4K_IA_SHIFT        16
#define CFG0_4K_IS_MASK         0x01c00000
#define CFG0_4K_IS_SHIFT        22

#define CFG0_4K_DL_MASK         0x00001c00
#define CFG0_4K_DL_SHIFT        10
#define CFG0_4K_DA_MASK         0x00000380
#define CFG0_4K_DA_SHIFT        7
#define CFG0_4K_DS_MASK         0x0000E000
#define CFG0_4K_DS_SHIFT        13



#endif /* __TNETD73XX_H_ */
