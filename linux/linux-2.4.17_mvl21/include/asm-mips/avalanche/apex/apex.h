#ifndef _APEX_H_
#define _APEX_H_

/*----------------------------------------------------
 * Apex's Module Base Addresses
 *--------------------------------------------------*/
#define AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE      (KSEG1_ADDR(0x04000000)) /* AVALANCHE VLYNQ 0 Mem map */
#define AVALANCHE_LOW_CPMAC_BASE              (KSEG1_ADDR(0x08610000)) /* AVALANCHE CPMAC 0 */
#define AVALANCHE_EMIF_CONTROL_BASE           (KSEG1_ADDR(0x08610800)) /* AVALANCHE EMIF */
#define AVALANCHE_GPIO_BASE                   (KSEG1_ADDR(0x08610900)) /* AVALANCHE GPIO */
#define AVALANCHE_CLOCK_CONTROL_BASE          (KSEG1_ADDR(0x08610A00)) /* AVALANCHE Clock Control */
#define AVALANCHE_TIMER0_BASE                 (KSEG1_ADDR(0x08610C00)) /* AVALANCHE Timer 1 */  
#define AVALANCHE_TIMER1_BASE                 (KSEG1_ADDR(0x08610D00)) /* AVALANCHE Timer 2 */  
#define AVALANCHE_UART0_REGS_BASE             (KSEG1_ADDR(0x08610E00)) /* AVALANCHE UART 0 */
#define AVALANCHE_I2C_BASE                    (KSEG1_ADDR(0x08611000)) /* AVALANCHE I2C */
#define AVALANCHE_MCDMA0_CTRL_BASE            (KSEG1_ADDR(0x08611400)) /* AVALANCHE MC DMA 0 (channels 0-3) */
#define AVALANCHE_RESET_CONTROL_BASE          (KSEG1_ADDR(0x08611600)) /* AVALANCHE Reset Control */
#define AVALANCHE_BIST_CONTROL_BASE           (KSEG1_ADDR(0x08611700)) /* AVALANCHE BIST Control */
#define AVALANCHE_LOW_VLYNQ_CONTROL_BASE      (KSEG1_ADDR(0x08611800)) /* AVALANCHE VLYNQ0 Control */
#define AVALANCHE_CODEC_INTF_BASE             (KSEG1_ADDR(0x08611900)) /* AVALANCHE VLYNQ0 Control */
#define AVALANCHE_DEVICE_CONFIG_LATCH_BASE    (KSEG1_ADDR(0x08611A00)) /* AVALANCHE Device Config Latch */
#define AVALANCHE_HIGH_VLYNQ_CONTROL_BASE     (KSEG1_ADDR(0x08611C00)) /* AVALANCHE VLYNQ1 Control */
#define AVALANCHE_MDIO_BASE                   (KSEG1_ADDR(0x08611E00)) /* AVALANCHE MDIO    */
#define AVALANCHE_WATCHDOG_TIMER_BASE         (KSEG1_ADDR(0x08611F00)) /* AVALANCHE Watch Dog Timer */  
#define AVALANCHE_INTC_BASE                   (KSEG1_ADDR(0x08612400)) /* AVALANCHE INTC  */
#define AVALANCHE_HIGH_CPMAC_BASE             (KSEG1_ADDR(0x08612800)) /* AVALANCHE CPMAC 1 */
#define AVALANCHE_TDM_BASE                    (KSEG1_ADDR(0x08620000)) /* AVALANCHE CPMAC 1 */
#define AVALANCHE_HIGH_VLYNQ_MEM_MAP_BASE     (KSEG1_ADDR(0x0C000000)) /* AVALANCHE VLYNQ 1 Mem map */

#define AVALANCHE_SDRAM_BASE                  0x14000000UL


/*----------------------------------------------------
 * Apex Interrupt Map (Primary Interrupts)
 *--------------------------------------------------*/

#define AVALANCHE_UNIFIED_SECONDARY_INT            0
#define AVALANCHE_EXT_INT_0                        1
#define AVALANCHE_EXT_INT_1                        2
#define AVALANCHE_EXT_INT_2                        3
#define AVALANCHE_EXT_INT_3                        4
#define AVALANCHE_TIMER_0_INT                      5
#define AVALANCHE_TIMER_1_INT                      6
#define AVALANCHE_UART0_INT                        7
#define AVALANCHE_DMA_INT0                         9
#define AVALANCHE_DMA_INT1                        10
#define AVALANCHE_TDM_TX_DMA_INT                  11
#define AVALANCHE_TDM_RX_DMA_INT                  12
#define AVALANCHE_TDM_TX_INT                      13 
#define AVALANCHE_TDM_RX_INT                      14
/* Line# 15 to 18 are reserved                    */
#define AVALANCHE_LOW_CPMAC_INT                   19
/* Line# 20 is reserved                           */
#define AVALANCHE_LOW_VLYNQ_INT                   21
/* Line# 22 to 24 is reserved                           */
#define AVALANCHE_HIGH_VLYNQ_INT                  25
/* Line# 26 to 28 are reserved                    */
#define AVALANCHE_I2C_INT                         29
#define AVALANCHE_DMA_INT2                        30
#define AVALANCHE_DMA_INT3                        31
#define AVALANCHE_CODEC_INTF_INT                  32
#define AVALANCHE_HIGH_CPMAC_INT                  33
/* Line# 34 to 39 is reserved                     */


#define AVALANCHE_EMIF_INT                        47



/*-----------------------------------------------------------
 * Apex's Reset Bits
 *---------------------------------------------------------*/

#define AVALANCHE_UART0_RESET_BIT                  0
/* Reset bit  1 is reserved.                       */
#define AVALANCHE_I2C_RESET_BIT                    2
#define AVALANCHE_TIMER0_RESET_BIT                 3
#define AVALANCHE_TIMER1_RESET_BIT                 4
/* Reset bit  5 is reserved.                       */
#define AVALANCHE_GPIO_RESET_BIT                   6
/* Reset bit 7 to 9 is reserved.                      */
#define AVALANCHE_TDM_RESET_BIT                   10
/* Reset bit 11 to 15 are reserved                */
#define AVALANCHE_HIGH_VLYNQ_RESET_BIT            16
#define AVALANCHE_LOW_CPMAC_RESET_BIT             17
#define AVALANCHE_MCDMA_RESET_BIT                 18
#define AVALANCHE_BIST_RESET_BIT                  19
#define AVALANCHE_LOW_VLYNQ_RESET_BIT             20
#define AVALANCHE_HIGH_CPMAC_RESET_BIT            21
#define AVALANCHE_MDIO_RESET_BIT                  22
/* Reset bit 23 to 28 are reserved                */
#define AVALANCHE_CODEC_INTF_RESET_BIT            29
/* Reset bits 20 and 31 are reserved                */



/*-----------------------------------------------------------
 * Apex's Power Control Bits
 *---------------------------------------------------------*/
/* PDCR bit  0 is reserved.                       */
#define AVALANCHE_POWER_MODULE_WDTP                1
#define AVALANCHE_POWER_MODULE_UT0P                2
/* PDCR bit  3 is reserved.                       */
#define AVALANCHE_POWER_MODULE_IICP                4
/* PDCR bit  5 is reserved.                       */
#define AVALANCHE_POWER_MODULE_GPIOP               6
#define AVALANCHE_POWER_MODULE_VLYNQ1P             7
/* PDCR bits  8 and 9 are reserved.               */
#define AVALANCHE_POWER_MODULE_EMIFP              10
#define AVALANCHE_POWER_MODULE_TDM                11
/* PDCR bit  12 is reserved.                       */
#define AVALANCHE_POWER_MODULE_RAMP               13
#define AVALANCHE_POWER_MODULE_ROMP               14
#define AVALANCHE_POWER_MODULE_DMAP               15
#define AVALANCHE_POWER_MODULE_BISTP              16
#define AVALANCHE_POWER_MODULE_TIMER0P            18
#define AVALANCHE_POWER_MODULE_TIMER1P            19
#define AVALANCHE_POWER_MODULE_EMAC0P             20
#define AVALANCHE_POWER_MODULE_CODEC_INTF         21
#define AVALANCHE_POWER_MODULE_EMAC1P             22
#define AVALANCHE_POWER_MODULE_EPHYP              24
#define AVALANCHE_POWER_MODULE_VLYNQ0P            27





/*
 * Apex board vectors
 */

#define AVALANCHE_VECS       (KSEG1_ADDR(AVALANCHE_SDRAM_BASE))
#define AVALANCHE_VECS_KSEG0 (KSEG0_ADDR(AVALANCHE_SDRAM_BASE))

/*-----------------------------------------------------------------------------
 * Apex's system register.
 * 
 *---------------------------------------------------------------------------*/
#define AVALANCHE_DCL_BOOTCR          (KSEG1_ADDR(0x08611A00))
#define AVALANCHE_EMIF_SDRAM_CFG      (AVALANCHE_EMIF_CONTROL_BASE + 0x8)
#define AVALANCHE_RST_CTRL_PRCR       (KSEG1_ADDR(0x08611600))
#define AVALANCHE_RST_CTRL_SWRCR      (KSEG1_ADDR(0x08611604))
#define AVALANCHE_RST_CTRL_RSR        (KSEG1_ADDR(0x08611608))

#define AVALANCHE_POWER_CTRL_PDCR     (KSEG1_ADDR(0x08610A00))
#define AVALANCHE_WAKEUP_CTRL_WKCR    (KSEG1_ADDR(0x08610A0C))

#define AVALANCHE_GPIO_DATA_IN        (AVALANCHE_GPIO_BASE +  0x0)
#define AVALANCHE_GPIO_DATA_OUT       (AVALANCHE_GPIO_BASE +  0x4)
#define AVALANCHE_GPIO_DIR            (AVALANCHE_GPIO_BASE +  0x8)    
#define AVALANCHE_GPIO_ENBL           (AVALANCHE_GPIO_BASE +  0xC)
#define AVALANCHE_CVR                 (AVALANCHE_GPIO_BASE +  0x14)

/*
 * Yamon Prom print address.
 */
#define AVALANCHE_YAMON_FUNCTION_BASE             (KSEG1_ADDR(0x10000500))
#define AVALANCHE_YAMON_PROM_PRINT_COUNT_ADDR     (AVALANCHE_YAMON_FUNCTION_BASE + 0x4)  /* print_count function */
#define AVALANCHE_YAMON_PROM_PRINT_ADDR           (AVALANCHE_YAMON_FUNCTION_BASE + 0x34)

#define AVALANCHE_BASE_BAUD       ( 3686400 / 16 )

#define  AVALANCHE_GPIO_PIN_COUNT         32             
#define  AVALANCHE_GPIO_OFF_MAP           {0x000FF87C} /* {0xF34FFFC0}*/ /*TBU later */


#define AVALANCHE_MIPS_FREQ_DEFAULT     275000000
#define AVALANCHE_VBUS_FREQ_DEFAULT     137500000

#include "apex_boards.h"

#endif /*_APEX_H_ */
