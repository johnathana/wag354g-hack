/*
 * include/asm-arm/arch-ixp1200/
 *
 * Copyright (C) 2001 MontaVista Software, Inc
 *
 * Generic IXP1200 hardware/register definitions
 *
 * April 2, 2000   Created : Uday Naik
 *  Nov 11, 2001   Massive cleanup/rewrite: Deepak Saxena
 *
 */

#ifndef __IXP1200_H_
#define __IXP1200_H_

/*
 * Physical location of various areas
 */
#define PHYS_SDRAM_CSR_BASE          0xff000000
#define PHYS_PREFETCH_SDRAM_BASE     0xd0000000
#define PHYS_NONPREFETCH_SDRAM_BASE  0xC0000000
#define PHYS_SDRAM_BASE              CONFIG_IXP1200_SDRAM_BASE
#define PHYS_SDRAM_SIZE		     CONFIG_IXP1200_SDRAM_SIZE * 1024 * 1024
#define PHYS_CACHE_FLUSH_BASE        0xa0000000
#define PHYS_MINI_CACHE_FLUSH_BASE   0xa0100000
#define PHYS_UENGCSR_BASE            0xb0000000
#define PHYS_ARMCSR_BASE	     0x90000000
#define PHYS_PCICSR_BASE             0x42000000
#define PHYS_PCICFG0_BASE	     0x53000000
#define PHYS_PCICFG1_BASE	     0x52000000
#define PHYS_FLASH_BASE      	     0x00000000
#define PHYS_PCI_IO		     0x54000000
#define PHYS_PCI_MEM		     0x60000000
#define PHYS_SRAM_CSR_BASE           0x38000000
#define PHYS_SRAM_SLOW_PORT_BASE     0x38400000
#define PHYS_PUSH_POPQ_BASE          0x20000000
#define PHYS_SDRAM_PACKET_BASE       0xC0000000 /* packets are in lower 8 MB */
#define PHYS_SRAM_BASE               0x10000000
#define PHYS_SRAM_READ_LOCK_BASE     0x12000000
#define PHYS_SRAM_WRITE_UNLOCK_BASE  0x14000000
#define PHYS_SRAM_CAM_UNLOCK_BASE    0x16000000
#define PHYS_SRAM_CLEAR_BITS         0x18000000
#define PHYS_SRAM_SET_BITS           0x18800000
#define PHYS_SRAM_TEST_CLEAR_BITS    0x19000000
#define PHYS_SRAM_TEST_SET_BITS      0x19800000

/* 
 * Now for the virtual mappings/sizes of above memory areas
 */
#define SRAM_PUSH_POPQ_BASE     0xd0000000
#define SRAM_PUSH_POPQ_SIZE     0x08000000

#define SDRAM_PACKET_BASE       0xd8000000
#define SDRAM_PACKET_SIZE       0x00800000

#define PCIMEM_BASE		0xe0000000
#define PCIMEM_SIZE		0x10000000

#define ARMCSR_BASE		0xf0000000
#define ARMCSR_SIZE		0x00100000

#define PCI_CSR_BASE            0xf0100000
#define PCI_CSR_SIZE            0x00100000

#define SRAM_CSR_BASE           0xf0200000
#define SRAM_CSR_SIZE           0x00100000

#define SDRAM_CSR_BASE          0xf0300000
#define SDRAM_CSR_SIZE          0x00100000

#define FLUSH_BASE              0xf0400000
#define CACHE_FLUSH_SIZE	0x00100000

#define FLUSH_BASE_MINICACHE	0xf0500000
#define MINI_CACHE_FLUSH_SIZE	0x00100000

#define MICROENGINE_FBI_BASE    0xf0600000
#define MICROENGINE_FBI_SIZE    0x00100000

#define PC_IO_BASE		0xf0700000
#define PCIO_SIZE		0x00100000

#define SRAM_SLOW_PORT_BASE     0xf0800000
#define SRAM_SLOW_PORT_SIZE     0x00300000

#define PCICFG0_BASE		0xf1000000
#define PCICFG0_SIZE		0x01000000

#define PCICFG1_BASE		0xf2000000
#define PCICFG1_SIZE		0x01000000

#define FLASH_BASE		0xf3000000
#define FLASH_SIZE		PHYS_FLASH_SIZE

#define SRAM_BASE               0xf4000000
#define SRAM_SIZE               0x00200000

#define SRAM_READ_LOCK          0xf5000000
#define SRAM_READ_LOCK_SIZE     0x00800000

#define SRAM_WRITE_UNLOCK       0xf5800000
#define SRAM_WRITE_UNLOCK_SIZE  0x00800000

#define SRAM_CAM_UNLOCK         0xf6000000
#define SRAM_CAM_UNLOCK_SIZE    0x00800000

#define SRAM_CLEAR_BITS         0xf6800000
#define SRAM_CLEAR_BITS_SIZE    0x00800000

#define SRAM_SET_BITS                0xf7000000
#define SRAM_SET_BITS_SIZE           0x00800000

#define SRAM_TEST_CLEAR_BITS         0xf7800000
#define SRAM_TEST_CLEAR_BITS_SIZE    0x00800000

#define SRAM_TEST_SET_BITS           0xf8000000
#define SRAM_TEST_SET_BITS_SIZE      0x00800000


/* define FLUSH BASE PHYS to be same as SA1100 for now */
#define FLUSH_BASE_PHYS              0xa0000000

/* Location and max size of compressed initrd*/
#define INITRD_LOCATION              (PHYS_SDRAM_BASE + 0x01000000)
#define INITRD_SIZE                  (0x00800000)

#ifndef __ASSEMBLY__

#define IXP1200_PCI_IO(x)	((volatile unsigned long *)(PCI_CSR_BASE+(x)))
#define IXP1200_IO(x)		((volatile unsigned long *)(ARMCSR_BASE+(x)))
#define IXP1200_REG(x)		((volatile unsigned long *)(x))

#else

#define IXP1200_PCI_IO(x)		(x)
#define IXP1200_IO(x)		        (x)

#endif

/*
 * PCI Control/Status Registers
 */
#define CSR_PCI_VEN_DEV_ID	IXP1200_PCI_IO(0x0000)
#define CSR_PCICMD		IXP1200_PCI_IO(0x0004)
#define CSR_CLASSREV		IXP1200_PCI_IO(0x0008)
#define CSR_PCICACHELINESIZE	IXP1200_PCI_IO(0x000c)
#define CSR_PCICSRBASE		IXP1200_PCI_IO(0x0010)
#define CSR_PCICSRIOBASE	IXP1200_PCI_IO(0x0014)
#define CSR_PCISDRAMBASE	IXP1200_PCI_IO(0x0018)
#define CSR_PCISUBSYS		IXP1200_PCI_IO(0x002c)
#define CSR_PCIOUTBINTMASK	IXP1200_PCI_IO(0x0034)
#define CSR_PCIINTLAT		IXP1200_PCI_IO(0x003c)
#define CSR_MBOX0               IXP1200_PCI_IO(0x0050)
#define CSR_MBOX1               IXP1200_PCI_IO(0x0054)
#define CSR_MBOX2               IXP1200_PCI_IO(0x0058)
#define CSR_MBOX3               IXP1200_PCI_IO(0x005c)
#define CSR_DOORBELL            IXP1200_PCI_IO(0x0060)
#define CSR_DOORBELL_SETUP      IXP1200_PCI_IO(0x0064)
#define CSR_CAPPTREXT		IXP1200_PCI_IO(0x0070)
#define CSR_PWRMGMT		IXP1200_PCI_IO(0x0074)
#define CSR_IXPRESET		IXP1200_PCI_IO(0x007c)
#define CSR_CSRBASEMASK		IXP1200_PCI_IO(0x00f8)
#define CSR_SDRAMBASEMASK	IXP1200_PCI_IO(0x0100)
#define CSR_SDRAMBASEOFFSET	IXP1200_PCI_IO(0x0104)
#define CSR_I2O_INFREEHEAD	IXP1200_PCI_IO(0x0120)
#define CSR_I2O_INPOSTTAIL	IXP1200_PCI_IO(0x0124)
#define CSR_I2O_OUTPOSTHEAD	IXP1200_PCI_IO(0x0128)
#define CSR_I2O_OUTFREETAIL	IXP1200_PCI_IO(0x012c)
#define CSR_I2O_INFREECOUNT	IXP1200_PCI_IO(0x0130)
#define CSR_I2O_OUTPOSTCOUNT	IXP1200_PCI_IO(0x0134)
#define CSR_I2O_INPOSTCOUNT	IXP1200_PCI_IO(0x0138)
#define CSR_SA_CONTROL		IXP1200_PCI_IO(0x013c)

/*
 * SA Control Register Values
 */
#define SA_CNTL_INITCMPLETE		(1 << 0)
#define SA_CNTL_ASSERTSERR		(1 << 1)
#define SA_CNTL_RXSERR		        (1 << 3)
#define SA_CNTL_DISCARDTIMER		(1 << 8)
#define SA_CNTL_PCINRESET		(1 << 9)
#define SA_CNTL_I2O_256		        (0 << 10)
#define SA_CNTL_I20_512		        (1 << 10)
#define SA_CNTL_I2O_1024		(2 << 10)
#define SA_CNTL_I2O_2048		(3 << 10)
#define SA_CNTL_I2O_4096		(4 << 10)
#define SA_CNTL_I2O_8192		(5 << 10)
#define SA_CNTL_I2O_16384		(6 << 10)
#define SA_CNTL_I2O_32768		(7 << 10)
#define SA_CNTL_WATCHDOG		(1 << 13)
#define SA_CNTL_BE_BEO			(1 << 15)
#define SA_CNTL_BE_DEO			(1 << 16)
#define SA_CNTL_BE_BEI			(1 << 17)
#define SA_CNTL_BE_DEI			(1 << 18)
#define SA_CNTL_PCICFN1		        (1 << 30)
#define SA_CNTL_PCICFN0		        (1 << 31)

#define CSR_PCIADDR_EXTN	IXP1200_PCI_IO(0x0140)
#define CSR_DOORBELL_PCI	IXP1200_PCI_IO(0x0150)
#define CSR_DOORBELL_SA  	IXP1200_PCI_IO(0x0154)
#define CSR_IRQ_STATUS		IXP1200_PCI_IO(0x0180)
#define CSR_IRQ_RAWSTATUS	IXP1200_PCI_IO(0x0184)
#define CSR_IRQ_ENABLE		IXP1200_PCI_IO(0x0188)
#define CSR_IRQ_DISABLE		IXP1200_PCI_IO(0x018c)
#define CSR_IRQ_SOFT		IXP1200_PCI_IO(0x0190)
#define CSR_FIQ_STATUS		IXP1200_PCI_IO(0x0280)
#define CSR_FIQ_RAWSTATUS	IXP1200_PCI_IO(0x0284)
#define CSR_FIQ_ENABLE		IXP1200_PCI_IO(0x0288)
#define CSR_FIQ_DISABLE		IXP1200_PCI_IO(0x028c)
#define CSR_FIQ_SOFT		IXP1200_PCI_IO(0x0290)
#define CSR_TIMER1_LOAD		IXP1200_PCI_IO(0x0300)
#define CSR_TIMER1_VALUE	IXP1200_PCI_IO(0x0304)
#define CSR_TIMER1_CNTL		IXP1200_PCI_IO(0x0308)
#define CSR_TIMER1_CLR		IXP1200_PCI_IO(0x030c)
#define CSR_TIMER2_LOAD		IXP1200_PCI_IO(0x0320)
#define CSR_TIMER2_VALUE	IXP1200_PCI_IO(0x0324)
#define CSR_TIMER2_CNTL		IXP1200_PCI_IO(0x0328)
#define CSR_TIMER2_CLR		IXP1200_PCI_IO(0x032c)
#define CSR_TIMER3_LOAD		IXP1200_PCI_IO(0x0340)
#define CSR_TIMER3_VALUE	IXP1200_PCI_IO(0x0344)
#define CSR_TIMER3_CNTL		IXP1200_PCI_IO(0x0348)
#define CSR_TIMER3_CLR		IXP1200_PCI_IO(0x034c)
#define CSR_TIMER4_LOAD		IXP1200_PCI_IO(0x0360)
#define CSR_TIMER4_VALUE	IXP1200_PCI_IO(0x0364)
#define CSR_TIMER4_CNTL		IXP1200_PCI_IO(0x0368)
#define CSR_TIMER4_CLR		IXP1200_PCI_IO(0x036c)

/*
 * Timer Control Register Values
 */
#define TIMER_CNTL_ENABLE	(1 << 7)
#define TIMER_CNTL_AUTORELOAD	(1 << 6)
#define TIMER_CNTL_DIV1		(0)
#define TIMER_CNTL_DIV16	(1 << 2)
#define TIMER_CNTL_DIV256	(2 << 2)
#define TIMER_CNTL_CNTEXT	(3 << 2)


/*
 * UART Access Registers
 */
#define CSR_UARTDR		IXP1200_IO(0x3c00)
#define CSR_UARTSR		IXP1200_IO(0x3400)
#define CSR_UARTCR		IXP1200_IO(0x3800)

/* 
 * Miscalleneous stuff like CSR for SDRAM, SRAM, LED etc 
 */

#define CSR_SDRAM               IXP1200_REG(SDRAM_CSR_BASE)
#define CSR_SRAM                IXP1200_REG(SRAM_CSR_BASE)
#define CSR_IREG                IXP1200_REG(MICROENGINE_FBI_BASE+0x401e0)
#define PHYS_LED_BASE           0x38508000
#define CSR_LED                 IXP1200_REG(PHYS_LED_BASE - PHYS_SRAM_SLOW_PORT_BASE + SRAM_SLOW_PORT_BASE)

/* 
 * IRQ Status
 */
#define CSR_ARM_IRQ              IXP1200_IO(0x1400)

#define CSR_PLL_CFG		IXP1200_IO(0x0c00)

/* 
 * RTC
 */
#define CSR_RTC_DIV		IXP1200_IO(0x2000)
#define CSR_RTC_TINT		IXP1200_IO(0x2400)
#define CSR_RTC_TVAL		IXP1200_IO(0x2800)
#define CSR_RTC_CNTR		IXP1200_IO(0x2c00)
#define	CSR_RTC_ALM		IXP1200_IO(0x3000)


/* 
 * Below is a mask of all interrupts set in the IRQ register of the core 
 *
 * UART | SDRAM | SRAM | UENG | CINT | PCI | RTC 
 */

#define IXP1200_IRQS          (IRQ_MASK_UART | IRQ_MASK_SDRAM | IRQ_MASK_SRAM | IRQ_MASK_UENG | IRQ_MASK_CINT | IRQ_MASK_PCI_INTS | IRQ_MASK_RTC)

/* These are masks from the IRQ register in the SA core */

#define IRQ_MASK_UART           (1 << 8)
#define IRQ_MASK_RTC            (1 << 6)
#define IRQ_MASK_SDRAM          (1 << 7)
#define IRQ_MASK_SRAM           (1 << 5)
#define IRQ_MASK_UENG           (1 << 4)
#define IRQ_MASK_CINT           (1 << 3)
#define IRQ_MASK_PCI_INTS       (1 << 2)

/* These are from the IRQ register in the PCI ISR register */
#define	IRQ_MASK_SWI		(1 << 1)
#define IRQ_MASK_TIMER1		(1 << 4)
#define IRQ_MASK_TIMER2		(1 << 5)
#define IRQ_MASK_TIMER3		(1 << 6)
#define	IRQ_MASK_TIMER4		(1 << 7)
#define IRQ_MASK_DOORBELL	(1 << 15)
#define IRQ_MASK_DMA1		(1 << 16)
#define IRQ_MASK_DMA2		(1 << 17)
#define IRQ_MASK_PCI		(1 << 18)
#define	IRQ_MASK_DMA1NB		(1 << 20)
#define	IRQ_MASK_DMA2NB		(1 << 21)
#define	IRQ_MASK_BIST		(1 << 22)
#define	IRQ_MASK_RSERR		(1 << 23)
#define IRQ_MASK_I2O		(1 << 25)
#define	IRQ_MASK_PWRM		(1 << 26)
#define	IRQ_MASK_DTE		(1 << 27)
#define	IRQ_MASK_DPED		(1 << 28)
#define	IRQ_MASK_RMA		(1 << 29)
#define	IRQ_MASK_RTA		(1 << 30)
#define	IRQ_MASK_DPE		(1 << 31)

#define	IRQ_MASK_PCI_ERR	(IRQ_MASK_RSERR | \
				 IRQ_MASK_DTE   | \
				 IRQ_MASK_DPED  | \
				 IRQ_MASK_RMA   | \
				 IRQ_MASK_RTA   | \
				 IRQ_MASK_DPE)


#define IXP1200_SA_CONTROL_PNR   (1<<9)  /*PCI not Reset bit of SA_CONTROL*/
#define IXP1200_SA_CONTROL_PCF   (1<<31) /*PCI Centrolfunction bit*/
#define IXP1200_PCI_COMMAND_IOSE (1<<0)  /*I/O space Enable*/
#define IXP1200_PCI_COMMAND_MSE  (1<<1)  /*Mem space Enable*/
#define IXP1200_PCI_COMMAND_ME   (1<<2)  /*Master Enable*/
#define IXP1200_PCI_COMMAND_MWI  (1<<4)  /*Master write and Invalidate Enable*/
#define IXP1200_PCI_COMMAND_PEC  (1<<6)  /*Parity Error Control */
#define IXP1200_PCI_COMMAND_SE   (1<<8)  /*SERR# enable */

#endif
