/******************************************************************************
 * FILE PURPOSE:    TNETD73xx Misc modules API Header
 ******************************************************************************
 * FILE NAME:       tnetd73xx_misc.h
 *
 * DESCRIPTION:     Clock Control, Reset Control, Power Management, GPIO
 *                  FSER Modules API 
 *                  As per TNETD73xx specifications
 *
 * REVISION HISTORY:
 * 27 Nov 02 - Sharath Kumar     PSP TII  
 * 14 Feb 03 - Anant Gole        PSP TII
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __TNETD73XX_MISC_H__
#define __TNETD73XX_MISC_H__

/*****************************************************************************
 * Reset Control Module
 *****************************************************************************/
 
typedef enum TNETD73XX_RESET_MODULE_tag
{
    RESET_MODULE_UART0      = 0,
    RESET_MODULE_UART1      = 1,
    RESET_MODULE_I2C        = 2,
    RESET_MODULE_TIMER0     = 3,
    RESET_MODULE_TIMER1     = 4,
    RESET_MODULE_GPIO       = 6,
    RESET_MODULE_ADSLSS     = 7,
    RESET_MODULE_USBS       = 8,
    RESET_MODULE_SAR        = 9,
    RESET_MODULE_VDMA_VT    = 11,
    RESET_MODULE_FSER       = 12,
    RESET_MODULE_VLYNQ1     = 16,
    RESET_MODULE_EMAC0      = 17,
    RESET_MODULE_DMA        = 18,
    RESET_MODULE_BIST       = 19,
    RESET_MODULE_VLYNQ0     = 20,
    RESET_MODULE_EMAC1      = 21,
    RESET_MODULE_MDIO       = 22,
    RESET_MODULE_ADSLSS_DSP = 23,
    RESET_MODULE_EPHY       = 26
} TNETD73XX_RESET_MODULE_T;

typedef enum TNETD73XX_RESET_CTRL_tag
{
    IN_RESET        = 0,
    OUT_OF_RESET
} TNETD73XX_RESET_CTRL_T;

typedef enum TNETD73XX_SYS_RST_MODE_tag
{
    RESET_SOC_WITH_MEMCTRL      = 1,    /* SW0 bit in SWRCR register */
    RESET_SOC_WITHOUT_MEMCTRL   = 2     /* SW1 bit in SWRCR register */
} TNETD73XX_SYS_RST_MODE_T;

typedef enum TNETD73XX_SYS_RESET_STATUS_tag
{
    HARDWARE_RESET = 0,
    SOFTWARE_RESET0,            /* Caused by writing 1 to SW0 bit in SWRCR register */
    WATCHDOG_RESET,
    SOFTWARE_RESET1             /* Caused by writing 1 to SW1 bit in SWRCR register */
} TNETD73XX_SYS_RESET_STATUS_T;

void tnetd73xx_reset_ctrl(TNETD73XX_RESET_MODULE_T reset_module, 
                                TNETD73XX_RESET_CTRL_T reset_ctrl);
TNETD73XX_RESET_CTRL_T tnetd73xx_get_reset_status(TNETD73XX_RESET_MODULE_T reset_module);
void tnetd73xx_sys_reset(TNETD73XX_SYS_RST_MODE_T mode);
TNETD73XX_SYS_RESET_STATUS_T tnetd73xx_get_sys_last_reset_status(void);
                    
/*****************************************************************************
 * Power Control Module
 *****************************************************************************/

typedef enum TNETD73XX_POWER_MODULE_tag
{
    POWER_MODULE_USBSP      = 0,
    POWER_MODULE_WDTP       = 1,
    POWER_MODULE_UT0P       = 2,
    POWER_MODULE_UT1P       = 3,
    POWER_MODULE_IICP       = 4,
    POWER_MODULE_VDMAP      = 5,
    POWER_MODULE_GPIOP      = 6,
    POWER_MODULE_VLYNQ1P    = 7,
    POWER_MODULE_SARP       = 8,
    POWER_MODULE_ADSLP      = 9,
    POWER_MODULE_EMIFP      = 10,
    POWER_MODULE_ADSPP      = 12,
    POWER_MODULE_RAMP       = 13,
    POWER_MODULE_ROMP       = 14,
    POWER_MODULE_DMAP       = 15,
    POWER_MODULE_BISTP      = 16,
    POWER_MODULE_TIMER0P    = 18,
    POWER_MODULE_TIMER1P    = 19,
    POWER_MODULE_EMAC0P     = 20,
    POWER_MODULE_EMAC1P     = 22,
    POWER_MODULE_EPHYP      = 24,
    POWER_MODULE_VLYNQ0P    = 27,
} TNETD73XX_POWER_MODULE_T;

typedef enum TNETD73XX_POWER_CTRL_tag
{
    POWER_CTRL_POWER_UP = 0,
    POWER_CTRL_POWER_DOWN
} TNETD73XX_POWER_CTRL_T;

typedef enum TNETD73XX_SYS_POWER_MODE_tag
{
    GLOBAL_POWER_MODE_RUN       = 0,    /* All system is up */
    GLOBAL_POWER_MODE_IDLE,             /* MIPS is power down, all peripherals working */
    GLOBAL_POWER_MODE_STANDBY,          /* Chip in power down, but clock to ADSKL subsystem is running */
    GLOBAL_POWER_MODE_POWER_DOWN        /* Total chip is powered down */
} TNETD73XX_SYS_POWER_MODE_T;

void tnetd73xx_power_ctrl(TNETD73XX_POWER_MODULE_T power_module,  TNETD73XX_POWER_CTRL_T power_ctrl);
TNETD73XX_POWER_CTRL_T tnetd73xx_get_pwr_status(TNETD73XX_POWER_MODULE_T power_module);
void tnetd73xx_set_global_pwr_mode(TNETD73XX_SYS_POWER_MODE_T power_mode);
TNETD73XX_SYS_POWER_MODE_T tnetd73xx_get_global_pwr_mode(void);

/*****************************************************************************
 * Wakeup Control 
 *****************************************************************************/

typedef enum TNETD73XX_WAKEUP_INTERRUPT_tag
{
    WAKEUP_INT0 = 1,
    WAKEUP_INT1 = 2,
    WAKEUP_INT2 = 4,
    WAKEUP_INT3 = 8
} TNETD73XX_WAKEUP_INTERRUPT_T;

typedef enum TNETD73XX_WAKEUP_CTRL_tag
{
    WAKEUP_DISABLED = 0,
    WAKEUP_ENABLED
} TNETD73XX_WAKEUP_CTRL_T;

typedef enum TNETD73XX_WAKEUP_POLARITY_tag
{
    WAKEUP_ACTIVE_HIGH = 0,
    WAKEUP_ACTIVE_LOW
} TNETD73XX_WAKEUP_POLARITY_T;

void tnetd73xx_wakeup_ctrl(TNETD73XX_WAKEUP_INTERRUPT_T wakeup_int, 
                           TNETD73XX_WAKEUP_CTRL_T wakeup_ctrl, 
                           TNETD73XX_WAKEUP_POLARITY_T wakeup_polarity);

/*****************************************************************************
 * FSER  Control 
 *****************************************************************************/
 
typedef enum TNETD73XX_FSER_MODE_tag
{
    FSER_I2C    = 0,
    FSER_UART   = 1
} TNETD73XX_FSER_MODE_T;

void tnetd73xx_fser_ctrl(TNETD73XX_FSER_MODE_T fser_mode);

/*****************************************************************************
 * Clock Control 
 *****************************************************************************/

#define CLK_MHZ(x)    ( (x) * 1000000 )

typedef enum TNETD73XX_CLKC_ID_tag
{
    CLKC_SYS = 0,
    CLKC_MIPS,
    CLKC_USB,
    CLKC_ADSLSS
} TNETD73XX_CLKC_ID_T;

void tnetd73xx_clkc_init(UINT32 afeclk, UINT32 refclk, UINT32 xtal3in);
TNETD73XX_ERR tnetd73xx_clkc_set_freq(TNETD73XX_CLKC_ID_T clk_id, UINT32 output_freq);
UINT32 tnetd73xx_clkc_get_freq(TNETD73XX_CLKC_ID_T clk_id);

/*****************************************************************************
 * GPIO Control 
 *****************************************************************************/

typedef enum TNETD73XX_GPIO_PIN_tag
{
    GPIO_UART0_RD           = 0,
    GPIO_UART0_TD           = 1,
    GPIO_UART0_RTS          = 2,
    GPIO_UART0_CTS          = 3,
    GPIO_FSER_CLK           = 4,
    GPIO_FSER_D             = 5,
    GPIO_EXT_AFE_SCLK       = 6,
    GPIO_EXT_AFE_TX_FS      = 7,
    GPIO_EXT_AFE_TXD        = 8,
    GPIO_EXT_AFE_RS_FS      = 9,
    GPIO_EXT_AFE_RXD1       = 10,
    GPIO_EXT_AFE_RXD0       = 11,
    GPIO_EXT_AFE_CDIN       = 12,
    GPIO_EXT_AFE_CDOUT      = 13,
    GPIO_EPHY_SPEED100      = 14,
    GPIO_EPHY_LINKON        = 15,
    GPIO_EPHY_ACTIVITY      = 16,
    GPIO_EPHY_FDUPLEX       = 17,
    GPIO_EINT0              = 18,
    GPIO_EINT1              = 19,
    GPIO_MBSP0_TCLK         = 20,
    GPIO_MBSP0_RCLK         = 21,
    GPIO_MBSP0_RD           = 22,
    GPIO_MBSP0_TD           = 23,
    GPIO_MBSP0_RFS          = 24,
    GPIO_MBSP0_TFS          = 25,
    GPIO_MII_DIO            = 26,
    GPIO_MII_DCLK           = 27,
} TNETD73XX_GPIO_PIN_T;

typedef enum TNETD73XX_GPIO_PIN_MODE_tag
{
    FUNCTIONAL_PIN = 0,
    GPIO_PIN = 1
} TNETD73XX_GPIO_PIN_MODE_T;

typedef enum TNETD73XX_GPIO_PIN_DIRECTION_tag
{
    GPIO_OUTPUT_PIN = 0,
    GPIO_INPUT_PIN = 1
} TNETD73XX_GPIO_PIN_DIRECTION_T;
 
void tnetd73xx_gpio_init(void);
void tnetd73xx_gpio_ctrl(TNETD73XX_GPIO_PIN_T gpio_pin, 
                         TNETD73XX_GPIO_PIN_MODE_T pin_mode,
                         TNETD73XX_GPIO_PIN_DIRECTION_T pin_direction);
void tnetd73xx_gpio_out(TNETD73XX_GPIO_PIN_T gpio_pin, BOOL value);
BOOL tnetd73xx_gpio_in(TNETD73XX_GPIO_PIN_T gpio_pin);

/* TNETD73XX Revision */
UINT32 tnetd73xx_get_revision(void);

#endif /* __TNETD73XX_MISC_H__ */
