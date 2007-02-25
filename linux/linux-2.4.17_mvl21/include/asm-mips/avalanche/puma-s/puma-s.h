#ifndef _PUMAS_H_
#define _PUMAS_H_

#include <linux/config.h>
#include <asm/addrspace.h>

/*----------------------------------------------------
 * Puma - s Module Base Addresses
 *--------------------------------------------------*/
#define AVALANCHE_DOCSIS_BASE                 (KSEG1ADDR(0x01000000)) /* AVALANCHE ADSL Mem Base */
#define AVALANCHE_USB_SLAVE_BASE              (KSEG1ADDR(0x03400000)) /* AVALANCHE USB SLAVE */
#define AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE      (KSEG1ADDR(0x04000000)) /* AVALANCHE VLYNQ 0 Mem map */
#define AVALANCHE_LOW_EMAC_BASE               (KSEG1ADDR(0x08610000)) /* AVALANCHE CPMAC 0 */
#define AVALANCHE_EMIF_CONTROL_BASE           (KSEG1ADDR(0x08610800)) /* AVALANCHE EMIF */
#define AVALANCHE_GPIO_BASE                   (KSEG1ADDR(0x08610900)) /* AVALANCHE GPIO */
#define AVALANCHE_CLOCK_CONTROL_BASE          (KSEG1ADDR(0x08610A00)) /* AVALANCHE Clock Control */
#define AVALANCHE_WATCHDOG_TIMER_BASE         (KSEG1ADDR(0x08610B00)) /* AVALANCHE Watch Dog Timer */  
#define AVALANCHE_TIMER0_BASE                 (KSEG1ADDR(0x08610C00)) /* AVALANCHE Timer 1 */  
#define AVALANCHE_TIMER1_BASE                 (KSEG1ADDR(0x08610D00)) /* AVALANCHE Timer 2 */  
#define AVALANCHE_UART0_REGS_BASE             (KSEG1ADDR(0x08610E00)) /* AVALANCHE UART 0 */
#define AVALANCHE_UART1_REGS_BASE             (KSEG1ADDR(0x08610F00)) /* AVALANCHE UART 0 */
#define AVALANCHE_I2C_BASE                    (KSEG1ADDR(0x08611000)) /* AVALANCHE I2C */
#define AVALANCHE_USB_SLAVE_CONTROL_BASE      (KSEG1ADDR(0x08611200)) /* AVALANCHE USB DMA */
#define AVALANCHE_MCDMA0_CTRL_BASE            (KSEG1ADDR(0x08611400)) /* AVALANCHE MC DMA 0 (channels 0-3) */
#define AVALANCHE_RESET_CONTROL_BASE          (KSEG1ADDR(0x08611600)) /* AVALANCHE Reset Control */
#define AVALANCHE_BIST_CONTROL_BASE           (KSEG1ADDR(0x08611700)) /* AVALANCHE BIST Control */
#define AVALANCHE_LOW_VLYNQ_CONTROL_BASE      (KSEG1ADDR(0x08611800)) /* AVALANCHE VLYNQ0 Control */
#define AVALANCHE_DEVICE_CONFIG_LATCH_BASE    (KSEG1ADDR(0x08611A00)) /* AVALANCHE Device Config Latch */
#define AVALANCHE_INTC_BASE                   (KSEG1ADDR(0x08612400)) /* AVALANCHE INTC  */

#define AVALANCHE_SDRAM_BASE                  0x14000000UL


/*----------------------------------------------------
 * Puma-s Interrupt Map (Primary Interrupts)
 *--------------------------------------------------*/

#define AVALANCHE_UNIFIED_SECONDARY_INT            0
#define AVALANCHE_EXT_INT_0                        1
#define AVALANCHE_EXT_INT_1                        2
#define AVALANCHE_EXT_INT_2                        3
#define AVALANCHE_EXT_INT_3                        4
#define AVALANCHE_TIMER_0_INT                      5
#define AVALANCHE_TIMER_1_INT                      6
#define AVALANCHE_UART0_INT                        7
#define AVALANCHE_UART1_INT                        8
#define AVALANCHE_DMA_INT0                         9
#define AVALANCHE_DMA_INT1                        10
/* Line# 11 to 18 are reserved                    */
#define AVALANCHE_LOW_EMAC_TX_INT                 19
#define AVALANCHE_LOW_EMAC_RX_INT                 20
#define AVALANCHE_LOW_VLYNQ_INT                   21
/* Line# 22 is reserved                           */
/* Line# 23 is reserved                           */
#define AVALANCHE_USB_SLAVE_INT                   24
/* Line# 25 to 28 are reserved                    */
#define AVALANCHE_I2C_INT                         29
#define AVALANCHE_DMA_INT2                        30
#define AVALANCHE_DMA_INT3                        31
/* Line# 32 to 38 is reserved                     */
#define AVALANCHE_DOCSIS_INT             39


/* Secondary Interrupts */
#define AVALANCHE_LOW_EMAC_LNK_CHG_INT            43
#define AVALANCHE_LOW_EMAC_MGT_INT                44
#define AVALANCHE_EMIF_INT                        47



/*-----------------------------------------------------------
 * Puma-s's Reset Bits
 *---------------------------------------------------------*/

#define AVALANCHE_UART0_RESET_BIT                  0
#define AVALANCHE_UART1_RESET_BIT                  1
#define AVALANCHE_I2C_RESET_BIT                    2
#define AVALANCHE_TIMER0_RESET_BIT                 3
#define AVALANCHE_TIMER1_RESET_BIT                 4
/* Reset bit  5 is reserved.                       */
#define AVALANCHE_GPIO_RESET_BIT                   6
#define AVALANCHE_DOCSIS_RESET_BIT                 7
#define AVALANCHE_USB_SLAVE_RESET_BIT              8
/* Reset bit 9 to 16 are reserved                 */
#define AVALANCHE_LOW_EMAC_RESET_BIT              17
#define AVALANCHE_MCDMA_RESET_BIT                 18
#define AVALANCHE_BIST_RESET_BIT                  19
#define AVALANCHE_LOW_VLYNQ_RESET_BIT             20
/* Reset bit  21 is reserved.                       */
#define AVALANCHE_MDIO_RESET_BIT                  22
/* Reset bit 23 to 31 are reserved                */


/*-----------------------------------------------------------
 * Puma-s's PDCR Bits
 *---------------------------------------------------------*/
#define AVALANCHE_POWER_MODULE_USBSP               0
#define AVALANCHE_POWER_MODULE_WDTP                1
#define AVALANCHE_POWER_MODULE_UT0P                2
#define AVALANCHE_POWER_MODULE_UT1P                3
#define AVALANCHE_POWER_MODULE_IICP                4
#define AVALANCHE_POWER_MODULE_GPIOP               6
#define AVALANCHE_POWER_MODULE_VLYNQ1P             7
#define AVALANCHE_POWER_MODULE_SARP                8
#define AVALANCHE_POWER_MODULE_DOCP                9
#define AVALANCHE_POWER_MODULE_EMIFP              10
#define AVALANCHE_POWER_MODULE_RAMP               13
#define AVALANCHE_POWER_MODULE_ROMP               14
#define AVALANCHE_POWER_MODULE_DMAP               15
#define AVALANCHE_POWER_MODULE_BISTP              16
#define AVALANCHE_POWER_MODULE_TIMER0P            18
#define AVALANCHE_POWER_MODULE_TIMER1P            19
#define AVALANCHE_POWER_MODULE_EMAC0P             20
#define AVALANCHE_POWER_MODULE_EPHYP              26
#define AVALANCHE_POWER_MODULE_VLYNQ0P            27




/*
 * Puma-s board vectors
 */

#define AVALANCHE_VECS       (KSEG1ADDR(AVALANCHE_SDRAM_BASE))
#define AVALANCHE_VECS_KSEG0 (KSEG0ADDR(AVALANCHE_SDRAM_BASE))

/*-----------------------------------------------------------------------------
 * Puma-s's system register.
 * 
 *---------------------------------------------------------------------------*/
#define AVALANCHE_DCL_BOOTCR          (KSEG1ADDR(0x08611A00))
#define AVALANCHE_EMIF_SDRAM_CFG      (AVALANCHE_EMIF_CONTROL_BASE + 0x8)
#define AVALANCHE_RST_CTRL_PRCR       (KSEG1ADDR(0x08611600))
#define AVALANCHE_RST_CTRL_SWRCR      (KSEG1ADDR(0x08611604))
#define AVALANCHE_RST_CTRL_RSR        (KSEG1ADDR(0x08611600))

#define AVALANCHE_POWER_CTRL_PDCR     (KSEG1ADDR(0x08610A00))
#define AVALANCHE_WAKEUP_CTRL_WKCR    (KSEG1ADDR(0x08610A0C))

#define AVALANCHE_GPIO_DATA_IN        (AVALANCHE_GPIO_BASE +  0x0)
#define AVALANCHE_GPIO_DATA_OUT       (AVALANCHE_GPIO_BASE +  0x4)
#define AVALANCHE_GPIO_DIR            (AVALANCHE_GPIO_BASE +  0x8)    
#define AVALANCHE_GPIO_ENBL           (AVALANCHE_GPIO_BASE +  0xC)
#define AVALANCHE_CVR                 (AVALANCHE_GPIO_BASE +  0x14)

/*
 * Yamon Prom print address.
 */
#define AVALANCHE_YAMON_FUNCTION_BASE             (KSEG1ADDR(0x10000500))
#define AVALANCHE_YAMON_PROM_PRINT_COUNT_ADDR     (AVALANCHE_YAMON_FUNCTION_BASE + 0x4)  /* print_count function */
#define AVALANCHE_YAMON_PROM_PRINT_ADDR           (AVALANCHE_YAMON_FUNCTION_BASE + 0x34)

#define AVALANCHE_BASE_BAUD       ( 3686400 / 16 )

#define  AVALANCHE_GPIO_PIN_COUNT         32             

#include "puma-s_boards.h"

#endif /*_PUMAS_H_ */
