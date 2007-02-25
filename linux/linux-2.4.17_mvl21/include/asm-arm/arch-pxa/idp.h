/*
 *  linux/include/asm-arm/arch-pxa/idp.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Copyright (c) 2001 Cliff Brake, Accelent Systems Inc.
 *
 * 2001-09-13: Cliff Brake <cbrake@accelent.com>
 *             Initial code
 * 
 */


/*
 * Note: this file must be safe to include in assembly files
 */


#define IDP_FLASH_PHYS		(PXA_CS0_PHYS)
#define IDP_ALT_FLASH_PHYS	(PXA_CS1_PHYS)
#define IDP_MEDIAQ_PHYS		(PXA_CS3_PHYS)
#define IDP_CTRL_PORT_PHYS	(PXA_CS5_PHYS + 0x02C00000)
#define IDP_IDE_PHYS		(PXA_CS5_PHYS + 0x03000000)
#define IDP_ETH_PHYS		(PXA_CS5_PHYS + 0x03400000)
#define IDP_COREVOLT_PHYS	(PXA_CS5_PHYS + 0x03800000)
#define IDP_CPLD_PHYS		(PXA_CS5_PHYS + 0x03C00000)


/*
 * virtual memory map
 */

#define IDP_CTRL_PORT_BASE	(0xf0000000)
#define IDP_CTRL_PORT_SIZE	(1*1024*1024)

#define IDP_IDE_BASE		(IDP_CTRL_PORT_BASE + IDP_CTRL_PORT_SIZE)
#define IDP_IDE_SIZE		(1*1024*1024)

#define IDP_ETH_BASE		(IDP_IDE_BASE + IDP_IDE_SIZE)
#define IDP_ETH_SIZE		(1*1024*1024)
#define ETH_BASE		IDP_ETH_BASE //smc9194 driver compatibility issue

#define IDP_COREVOLT_BASE	(IDP_ETH_BASE + IDP_ETH_SIZE)
#define IDP_COREVOLT_SIZE	(1*1024*1024)

#define IDP_CPLD_BASE		(IDP_COREVOLT_BASE + IDP_COREVOLT_SIZE)
#define IDP_CPLD_SIZE		(1*1024*1024)

#if (IDP_CPLD_BASE + IDP_CPLD_SIZE) > 0xfc000000
#error Your custom IO space is getting a bit large !!
#endif

#define CPLD_P2V(x)		((x) - IDP_CPLD_PHYS + IDP_CPLD_BASE)
#define CPLD_V2P(x)		((x) - IDP_CPLD_BASE + IDP_CPLD_PHYS)

#ifndef __ASSEMBLY__
#  define __CPLD_REG(x)		(*((volatile unsigned long *)CPLD_P2V(x)))
#else
#  define __CPLD_REG(x)		CPLD_P2V(x)
#endif

/* board level registers in the CPLD: (offsets from CPLD_BASE) */

#define _IDP_CPLD_LED_CONTROL		(IDP_CPLD_PHYS + 0x00)
#define _IDP_CPLD_PERIPH_PWR		(IDP_CPLD_PHYS + 0x04)
#define _IDP_CPLD_CIR			(IDP_CPLD_PHYS + 0x08)
#define _IDP_CPLD_KB_COL_HIGH		(IDP_CPLD_PHYS + 0x0C)
#define _IDP_CPLD_KB_COL_LOW		(IDP_CPLD_PHYS + 0x10)
#define _IDP_CPLD_PCCARD_EN		(IDP_CPLD_PHYS + 0x14)
#define _IDP_CPLD_GPIOH_DIR		(IDP_CPLD_PHYS + 0x18)
#define _IDP_CPLD_GPIOH_VALUE		(IDP_CPLD_PHYS + 0x1C)
#define _IDP_CPLD_GPIOL_DIR		(IDP_CPLD_PHYS + 0x20)
#define _IDP_CPLD_GPIOL_VALUE		(IDP_CPLD_PHYS + 0x24)
#define _IDP_CPLD_MISC			(IDP_CPLD_PHYS + 0x28)
#define _IDP_CPLD_PCCARD0_STATUS	(IDP_CPLD_PHYS + 0x2C)
#define _IDP_CPLD_PCCARD1_STATUS	(IDP_CPLD_PHYS + 0x30)

/* FPGA register virtual addresses */
#define IDP_CPLD_LED_CONTROL		__CPLD_REG(_IDP_CPLD_LED_CONTROL)	/* write only */
#define IDP_CPLD_PERIPH_PWR		__CPLD_REG(_IDP_CPLD_PERIPH_PWR)	/* write only */
#define IDP_CPLD_CIR			__CPLD_REG(_IDP_CPLD_CIR)		/* write only */
#define IDP_CPLD_KB_COL_HIGH		__CPLD_REG(_IDP_CPLD_KB_COL_HIGH)	/* write only */
#define IDP_CPLD_KB_COL_LOW		__CPLD_REG(_IDP_CPLD_KB_COL_LOW)	/* write only */
#define IDP_CPLD_PCCARD_EN		__CPLD_REG(_IDP_CPLD_PCCARD_EN)		/* write only */
#define IDP_CPLD_GPIOH_DIR		__CPLD_REG(_IDP_CPLD_GPIOH_DIR)		/* write only */
#define IDP_CPLD_GPIOH_VALUE		__CPLD_REG(_IDP_CPLD_GPIOH_VALUE)	/* write only */
#define IDP_CPLD_GPIOL_DIR		__CPLD_REG(_IDP_CPLD_GPIOL_DIR)		/* write only */
#define IDP_CPLD_GPIOL_VALUE		__CPLD_REG(_IDP_CPLD_GPIOL_VALUE)	/* write only */
#define IDP_CPLD_MISC			__CPLD_REG(_IDP_CPLD_MISC)		/* read only */
#define IDP_CPLD_PCCARD0_STATUS		__CPLD_REG(_IDP_CPLD_PCCARD0_STATUS)	/* read only */
#define IDP_CPLD_PCCARD1_STATUS		__CPLD_REG(_IDP_CPLD_PCCARD1_STATUS)	/* read only */


#ifndef __ASSEMBLY__

/* shadow registers for write only registers */
extern unsigned int idp_cpld_led_control_shadow;
extern unsigned int idp_cpld_periph_pwr_shadow;
extern unsigned int idp_cpld_cir_shadow;
extern unsigned int idp_cpld_kb_col_high_shadow;
extern unsigned int idp_cpld_kb_col_low_shadow;
extern unsigned int idp_cpld_pccard_en_shadow;
extern unsigned int idp_cpld_gpioh_dir_shadow;
extern unsigned int idp_cpld_gpioh_value_shadow;
extern unsigned int idp_cpld_gpiol_dir_shadow;
extern unsigned int idp_cpld_gpiol_value_shadow;

extern unsigned int idp_control_port_shadow;

/* 
 * macros to write to write only register
 *
 * none of these macros are protected from 
 * multiple drivers using them in interrupt context.
 */

#define WRITE_IDP_CPLD_LED_CONTROL(value, mask) \
{\
	idp_cpld_led_control_shadow = ((value & mask) | (idp_cpld_led_control_shadow & ~mask));\
	IDP_CPLD_LED_CONTROL = idp_cpld_led_control_shadow;\
}
#define WRITE_IDP_CPLD_PERIPH_PWR(value, mask) \
{\
	idp_cpld_periph_pwr_shadow = ((value & mask) | (idp_cpld_periph_pwr_shadow & ~mask));\
	IDP_CPLD_PERIPH_PWR = idp_cpld_periph_pwr_shadow;\
}
#define WRITE_IDP_CPLD_CIR(value, mask) \
{\
	idp_cpld_cir_shadow = ((value & mask) | (idp_cpld_cir_shadow & ~mask));\
	IDP_CPLD_CIR = idp_cpld_cir_shadow;\
}
#define WRITE_IDP_CPLD_KB_COL_HIGH(value, mask) \
{\
	idp_cpld_kb_col_high_shadow = ((value & mask) | (idp_cpld_kb_col_high_shadow & ~mask));\
	IDP_CPLD_KB_COL_HIGH = idp_cpld_kb_col_high_shadow;\
}
#define WRITE_IDP_CPLD_KB_COL_LOW(value, mask) \
{\
	idp_cpld_kb_col_low_shadow = ((value & mask) | (idp_cpld_kb_col_low_shadow & ~mask));\
	IDP_CPLD_KB_COL_LOW = idp_cpld_kb_col_low_shadow;\
}
#define WRITE_IDP_CPLD_PCCARD_EN(value, mask) \
{\
	idp_cpld_ = ((value & mask) | (idp_cpld_led_control_shadow & ~mask));\
	IDP_CPLD_LED_CONTROL = idp_cpld_led_control_shadow;\
}
#define WRITE_IDP_CPLD_GPIOH_DIR(value, mask) \
{\
	idp_cpld_gpioh_dir_shadow = ((value & mask) | (idp_cpld_gpioh_dir_shadow & ~mask));\
	IDP_CPLD_GPIOH_DIR = idp_cpld_gpioh_dir_shadow;\
}
#define WRITE_IDP_CPLD_GPIOH_VALUE(value, mask) \
{\
	idp_cpld_gpioh_value_shadow = ((value & mask) | (idp_cpld_gpioh_value_shadow & ~mask));\
	IDP_CPLD_GPIOH_VALUE = idp_cpld_gpioh_value_shadow;\
}
#define WRITE_IDP_CPLD_GPIOL_DIR(value, mask) \
{\
	idp_cpld_gpiol_dir_shadow = ((value & mask) | (idp_cpld_gpiol_dir_shadow & ~mask));\
	IDP_CPLD_GPIOL_DIR = idp_cpld_gpiol_dir_shadow;\
}
#define WRITE_IDP_CPLD_GPIOL_VALUE(value, mask) \
{\
	idp_cpld_gpiol_value_shadow = ((value & mask) | (idp_cpld_gpiol_value_shadow & ~mask));\
	IDP_CPLD_GPIOL_VALUE = idp_cpld_gpiol_value_shadow;\
}

#define WRITE_IDP_CONTROL_PORT(value, mask) \
{\
	idp_control_port_shadow = ((value & mask) | (idp_control_port_shadow & ~mask));\
	(*((volatile unsigned long *)IDP_CTRL_PORT_BASE)) = idp_control_port_shadow;\
}

#endif


/*
 * Bit masks for various registers
 */


/* control port */
#define IDP_CONTROL_PORT_PCSLOT0_0	(1 << 0)
#define IDP_CONTROL_PORT_PCSLOT0_1	(1 << 1)
#define IDP_CONTROL_PORT_PCSLOT0_2	(1 << 2)
#define IDP_CONTROL_PORT_PCSLOT0_3	(1 << 3)
#define IDP_CONTROL_PORT_PCSLOT1_1	(1 << 4)
#define IDP_CONTROL_PORT_PCSLOT1_2	(1 << 5)
#define IDP_CONTROL_PORT_PCSLOT1_3	(1 << 6)
#define IDP_CONTROL_PORT_PCSLOT1_4	(1 << 7)
#define IDP_CONTROL_PORT_SERIAL1_EN	(1 << 9)
#define IDP_CONTROL_PORT_SERIAL2_EN	(1 << 10)
#define IDP_CONTROL_PORT_SERIAL3_EN	(1 << 11)
#define IDP_CONTROL_PORT_IRDA_FIR	(1 << 12)
#define IDP_CONTROL_PORT_IRDA_M0	(1 << 13)
#define IDP_CONTROL_PORT_IRDA_M1	(1 << 14)
#define IDP_CONTROL_PORT_I2S_PWR	(1 << 15)
#define IDP_CONTROL_PORT_FLASH_WP	(1 << 19)
#define IDP_CONTROL_PORT_MILL_EN	(1 << 20)
#define IDP_CONTROL_PORT_LCD_PWR	(1 << 21)
#define IDP_CONTROL_PORT_LCD_BKLEN	(1 << 22)
#define IDP_CONTROL_PORT_LCD_ENAVLCD	(1 << 23)

/*
 * Macros for LCD Driver
 */

#ifdef CONFIG_FB_PXA

#define FB_BACKLIGHT_ON() WRITE_IDP_CONTROL_PORT(IDP_CONTROL_PORT_LCD_BKLEN, IDP_CONTROL_PORT_LCD_BKLEN)
#define FB_BACKLIGHT_OFF() WRITE_IDP_CONTROL_PORT(0, IDP_CONTROL_PORT_LCD_BKLEN)

#define FB_PWR_ON() WRITE_IDP_CONTROL_PORT(IDP_CONTROL_PORT_LCD_PWR, IDP_CONTROL_PORT_LCD_PWR)
#define FB_PWR_OFF() WRITE_IDP_CONTROL_PORT(0, IDP_CONTROL_PORT_LCD_PWR)

#define FB_VLCD_ON() WRITE_IDP_CONTROL_PORT(IDP_CONTROL_PORT_LCD_ENAVLCD, IDP_CONTROL_PORT_LCD_ENAVLCD)
#define FB_VLCD_OFF() WRITE_IDP_CONTROL_PORT(0, IDP_CONTROL_PORT_LCD_ENAVLCD)

#endif


/*
 * Macros for LED Driver
 */

/* leds 0 = ON */
#define IDP_HB_LED	0x1
#define IDP_BUSY_LED	0x2

#define IDP_LEDS_MASK	(IDP_HB_LED | IDP_BUSY_LED)

#define IDP_WRITE_LEDS(value) 	WRITE_IDP_CPLD_LED_CONTROL(value, IDP_LEDS_MASK)

/*
 * macros for MTD driver
 */

#define FLASH_WRITE_PROTECT_DISABLE()	WRITE_IDP_CONTROL_PORT(0, IDP_CONTROL_PORT_FLASH_WP)
#define FLASH_WRITE_PROTECT_ENABLE()	WRITE_IDP_CONTROL_PORT(IDP_CONTROL_PORT_FLASH_WP, IDP_CONTROL_PORT_FLASH_WP)


