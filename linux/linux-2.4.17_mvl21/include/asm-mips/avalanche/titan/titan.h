#ifndef _TITAN_H
#define _TITAN_H

#include <linux/config.h>
#include <asm/addrspace.h>

/*----------------------------------------------------
 * Titan's Module Base Addresses
 *--------------------------------------------------*/
#define AVALANCHE_USB_SLAVE_BASE              (KSEG1ADDR(0x03400000)) /* AVALANCHE USB SLAVE */
#define AVALANCHE_DSP_SUBSYSTEM_MEM_BASE      (KSEG1ADDR(0x04000000)) /* AVALANCHE DSP Mem Base */
#define AVALANCHE_EMIF_CONTROL_BASE           (KSEG1ADDR(0x08610800)) /* AVALANCHE EMIF */
#define AVALANCHE_GPIO_BASE                   (KSEG1ADDR(0x08610900)) /* AVALANCHE GPIO */
#define AVALANCHE_CLOCK_CONTROL_BASE          (KSEG1ADDR(0x08610A00)) /* AVALANCHE Clock Control */
#define AVALANCHE_WATCHDOG_TIMER_BASE         (KSEG1ADDR(0x08610B00)) /* AVALANCHE Watch Dog Timer */  
#define AVALANCHE_TIMER0_BASE                 (KSEG1ADDR(0x08610C00)) /* AVALANCHE Timer 1 */  
#define AVALANCHE_TIMER1_BASE                 (KSEG1ADDR(0x08610D00)) /* AVALANCHE Timer 2 */  
#define AVALANCHE_UART0_REGS_BASE             (KSEG1ADDR(0x08610E00)) /* AVALANCHE UART 0 */
#define AVALANCHE_SSP_BASE                    (KSEG1ADDR(0x08611000)) /* AVALANCHE I2C */
#define AVALANCHE_USB_SLAVE_CONTROL_BASE      (KSEG1ADDR(0x08611200)) /* AVALANCHE USB DMA */
#define AVALANCHE_HIGH_VLYNQ_CONTROL_BASE     (KSEG1ADDR(0x08611300)) /* AVALANCHE VLYNQ1 Control */
#define AVALANCHE_MCDMA0_CTRL_BASE            (KSEG1ADDR(0x08611400)) /* AVALANCHE MC DMA 0 (channels 0-3) */
#define AVALANCHE_RESET_CONTROL_BASE          (KSEG1ADDR(0x08611600)) /* AVALANCHE Reset Control */
#define AVALANCHE_DSP_SUBSYSTEM_CONTROL_BASE  (KSEG1ADDR(0x08611700)) /* AVALANCHE BIST */
#define AVALANCHE_DEVICE_CONFIG_LATCH_BASE    (KSEG1ADDR(0x08611A00)) /* AVALANCHE Device Config Latch */
#define AVALANCHE_LOW_VLYNQ_CONTROL_BASE      (KSEG1ADDR(0x08611C00)) /* AVALANCHE VLYNQ0 Control */
#define AVALANCHE_USB_MASTER_CONTROL_BASE     (KSEG1ADDR(0x08612100)) /* AVALANCHE USB Master Control */        
#define AVALANCHE_KEYPAD_CONTROL_BASE         (KSEG1ADDR(0x08612200)) /* AVALANCHE Keypad Control */
#define AVALANCHE_LCD_CONTROL_BASE            (KSEG1ADDR(0x08612300)) /* AVALANCHE LCD Control */
#define AVALANCHE_INTC_BASE                   (KSEG1ADDR(0x08612400)) /* AVALANCHE INTC  */
#define AVALANCHE_ESWITCH_BASE                (KSEG1ADDR(0x08640000)) /* AVALANCHE Eswitch base */
#define AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE      (0x0C000000) /* AVALANCHE VLYNQ 0 Mem map */
#define AVALANCHE_HIGH_VLYNQ_MEM_MAP_BASE     (0x40000000) /* AVALANCHE VLYNQ 1 Mem map */

#define AVALANCHE_SDRAM_BASE                  0x14000000UL

#define AVALANCHE_LOW_CPMAC_BASE              (AVALANCHE_ESWITCH_BASE + 0)         /* AVALANCHE CPMAC 0 */
#define AVALANCHE_HIGH_CPMAC_BASE             (AVALANCHE_ESWITCH_BASE + 0x00800)   /* AVALANCHE CPMAC 1 */
#define AVALANCHE_ESWITCH_HDMA_BASE           (AVALANCHE_ESWITCH_BASE + 0x01000)   /* AVALANCHE HDMA    */
#define AVALANCHE_MDIO_BASE                   (AVALANCHE_ESWITCH_BASE + 0x02000)   /* AVALANCHE MDIO    */
#define AVALANCHE_ESWITCH_CONTROL             (AVALANCHE_ESWITCH_BASE + 0x02800)   /* AVALANCHE ESWITCH Control */
#define AVALANCHE_PDSP_INSTRUCTION_MEM        (AVALANCHE_ESWITCH_BASE + 0x08000)   /* AVALANCHE PDSP Intruction mem */
#define AVALANCHE_ETHERNET_PACKET_MEM         (AVALANCHE_ESWITCH_BASE + 0x10000)   /* AVALANCHE Eswitch packet mem */

/*----------------------------------------------------
 * Titan Interrupt Map (Primary Interrupts)
 *--------------------------------------------------*/

#define AVALANCHE_UNIFIED_SECONDARY_INT            0
#define AVALANCHE_EXT_INT_0                        1
#define AVALANCHE_EXT_INT_1                        2
#define AVALANCHE_EXT_INT_2                        3
#define AVALANCHE_EXT_INT_3                        4
#define AVALANCHE_TIMER_0_INT                      5
#define AVALANCHE_TIMER_1_INT                      6
#define AVALANCHE_UART0_INT                        7
/* Line#  8 is reserved                           */
#define AVALANCHE_DMA_INT0                         9
#define AVALANCHE_DMA_INT1                        10
/* Line# 11 to 18 are reserved                    */
#define AVALANCHE_LOW_CPMAC_INT                   19
/* Line# 20 is reserved                           */
#define AVALANCHE_DSP_GPIO_OUT_INT                21
#define AVALANCHE_DSP_SW_RESET_STATUS_INT         22
#define AVALANCHE_DSP_CODEC_INT                   23
#define AVALANCHE_USB_SLAVE_INT                   24
#define AVALANCHE_LOW_VLYNQ_INT                   25
#define AVALANCHE_HIGH_VLYNQ_INT                  26
#define AVALANCHE_SWITCH_HOST_DMA_INT             27
#define AVALANCHE_UNIFIED_PHY_INT                 28
#define AVALANCHE_SSP_INT                         29
#define AVALANCHE_DMA_INT2                        30
#define AVALANCHE_DMA_INT3                        31
#define AVALANCHE_DSP_CODEC_XFER_INT              32
#define AVALANCHE_HIGH_CPMAC_INT                  33
/* Line# 34 is reserved                           */
#define AVALANCHE_USB_MASTER_INT                  35
#define AVALANCHE_LCD_INT                         36
#define AVALANCHE_KPAD_CNTL_INT                   37
/* Line# 39 is reserved                           */

#define AVALANCHE_EMIF_INT                        46
#define AVALANCHE_DSP_WATCH_DOG_INT               47


/*-----------------------------------------------------------
 * Titan's Reset Bits
 *---------------------------------------------------------*/

#define AVALANCHE_UART0_RESET_BIT                  0
#define AVALANCHE_SSP_RESET_BIT                    2
#define AVALANCHE_TIMER0_RESET_BIT                 3
#define AVALANCHE_TIMER1_RESET_BIT                 4
#define AVALANCHE_GPIO_RESET_BIT                   6
#define AVALANCHE_ADSL_SS_RESET_BIT                7
#define AVALANCHE_USB_SLAVE_RESET_BIT              8
#define AVALANCHE_HDMA_RESET_BIT                  13
#define AVALANCHE_USB_MASTER_RESET_BIT            14
#define AVALANCHE_LOW_VLYNQ_RESET_BIT             15
#define AVALANCHE_HIGH_VLYNQ_RESET_BIT            16
#define AVALANCHE_LOW_CPMAC_RESET_BIT             17
#define AVALANCHE_MCDMA_RESET_BIT                 18
#define AVALANCHE_DSP_SUBSYSTEM_RESET_BIT         20
#define AVALANCHE_HIGH_CPMAC_RESET_BIT            21
#define AVALANCHE_MDIO_RESET_BIT                  22
#define AVALANCHE_LCD_CTRL_RESET_BIT              24
#define AVALANCHE_KEYPAD_CTRL_RESET_BIT           25
#define AVALANCHE_LOW_EPHY_RESET_BIT              26
#define AVALANCHE_ESWITCH_RESET_BIT               27
#define AVALANCHE_HIGH_EPHY_RESET_BIT             28



/*-----------------------------------------------------------
 * Titan's Power Bits
 *---------------------------------------------------------*/
#define AVALANCHE_POWER_MODULE_USBSP               0 
#define AVALANCHE_POWER_MODULE_WDTP                1 
#define AVALANCHE_POWER_MODULE_UT0P                2 
#define AVALANCHE_POWER_MODULE_UT1P                3 
#define AVALANCHE_POWER_MODULE_VDMAP               5 
#define AVALANCHE_POWER_MODULE_GPIOP               6 
#define AVALANCHE_POWER_MODULE_VLYNQ1P             7 
#define AVALANCHE_POWER_MODULE_EMIFP              10 
#define AVALANCHE_POWER_MODULE_ADSPP              12 
#define AVALANCHE_POWER_MODULE_RAMP               13 
#define AVALANCHE_POWER_MODULE_ROMP               14 
#define AVALANCHE_POWER_MODULE_DMAP               15 
#define AVALANCHE_POWER_MODULE_BISTP              16 
#define AVALANCHE_POWER_MODULE_TIMER0P            18 
#define AVALANCHE_POWER_MODULE_TIMER1P            19 
#define AVALANCHE_POWER_MODULE_EMAC0P             20 
#define AVALANCHE_POWER_MODULE_EMAC1P             22 
#define AVALANCHE_POWER_MODULE_VLYNQ0P            27 




/*
 * Titan board vectors
 */

#define AVALANCHE_VECS       (KSEG1ADDR(AVALANCHE_SDRAM_BASE))
#define AVALANCHE_VECS_KSEG0 (KSEG0ADDR(AVALANCHE_SDRAM_BASE))

/*-----------------------------------------------------------------------------
 * Titan's system register.
 * 
 *---------------------------------------------------------------------------*/
#define AVALANCHE_DCL_BOOTCR          (KSEG1ADDR(0x08611A00))
#define AVALANCHE_EMIF_SDRAM_CFG      (AVALANCHE_EMIF_CONTROL_BASE + 0x8)
#define AVALANCHE_RST_CTRL_PRCR       (KSEG1ADDR(0x08611600))
#define AVALANCHE_RST_CTRL_SWRCR      (KSEG1ADDR(0x08611604))
#define AVALANCHE_RST_CTRL_RSR        (KSEG1ADDR(0x08611600))
#define AVALANCHE_POWER_CTRL_PDCR     (KSEG1ADDR(0x08610A00))
#define AVALANCHE_WAKEUP_CTRL_WKCR    (KSEG1ADDR(0x08610A0C))
#define AVALANCHE_CVR                 (KSEG1ADDR(0x08610924))

#define TITAN_GPIO_DATA_IN_0          (AVALANCHE_GPIO_BASE +  0x0)
#define TITAN_GPIO_DATA_IN_1          (AVALANCHE_GPIO_BASE +  0x4)
#define TITAN_GPIO_DATA_OUT_0         (AVALANCHE_GPIO_BASE +  0x8)
#define TITAN_GPIO_DATA_OUT_1         (AVALANCHE_GPIO_BASE +  0xc)
#define TITAN_GPIO_DIR_0              (AVALANCHE_GPIO_BASE + 0x10)    
#define TITAN_GPIO_DIR_1              (AVALANCHE_GPIO_BASE + 0x14)
#define TITAN_GPIO_ENBL_0             (AVALANCHE_GPIO_BASE + 0x18)
#define TITAN_GPIO_ENBL_1             (AVALANCHE_GPIO_BASE + 0x1c)    

typedef struct
{
    volatile unsigned int reg[21];
}
PIN_SEL_REG_ARRAY_T;

typedef struct
{
    unsigned int data_in [2];
    unsigned int data_out[2];
    unsigned int dir[2];
    unsigned int enable[2];

} TITAN_GPIO_CONTROL_T;

#define AVALANCHE_PIN_SEL_BASE        0xA861160C

/*
 * Yamon Prom print address.
 */
#define AVALANCHE_YAMON_FUNCTION_BASE             (KSEG1ADDR(0x10000500))
#define AVALANCHE_YAMON_PROM_PRINT_COUNT_ADDR     (AVALANCHE_YAMON_FUNCTION_BASE + 0x4)  /* print_count function */
#define AVALANCHE_YAMON_PROM_PRINT_ADDR           (AVALANCHE_YAMON_FUNCTION_BASE + 0x34)

#define AVALANCHE_BASE_BAUD       ( 3686400 / 16 )

#include "titan_boards.h"

#endif /*_TITAN_H */
