#ifndef _AVALANCHE_MISC_H_
#define _AVALANCHE_MISC_H_

#include<asm/avalanche/avalanche_map.h>

typedef enum AVALANCHE_ERR_t
{
    AVALANCHE_ERR_OK        = 0,    /* OK or SUCCESS */
    AVALANCHE_ERR_ERROR     = -1,   /* Unspecified/Generic ERROR */

    /* Pointers and args */
    AVALANCHE_ERR_INVARG        = -2,   /* Invaild argument to the call */
    AVALANCHE_ERR_NULLPTR       = -3,   /* NULL pointer */
    AVALANCHE_ERR_BADPTR        = -4,   /* Bad (out of mem) pointer */

    /* Memory issues */
    AVALANCHE_ERR_ALLOC_FAIL    = -10,  /* allocation failed */
    AVALANCHE_ERR_FREE_FAIL     = -11,  /* free failed */
    AVALANCHE_ERR_MEM_CORRUPT   = -12,  /* corrupted memory */
    AVALANCHE_ERR_BUF_LINK      = -13,  /* buffer linking failed */

    /* Device issues */
    AVALANCHE_ERR_DEVICE_TIMEOUT    = -20,  /* device timeout on read/write */
    AVALANCHE_ERR_DEVICE_MALFUNC    = -21,  /* device malfunction */

    AVALANCHE_ERR_INVID     = -30   /* Invalid ID */

} AVALANCHE_ERR;

/*****************************************************************************
 * Reset Control Module
 *****************************************************************************/

typedef enum AVALANCHE_RESET_CTRL_tag
{
    IN_RESET        = 0,
    OUT_OF_RESET
} AVALANCHE_RESET_CTRL_T;

typedef enum AVALANCHE_SYS_RST_MODE_tag
{
    RESET_SOC_WITH_MEMCTRL      = 1,    /* SW0 bit in SWRCR register */
    RESET_SOC_WITHOUT_MEMCTRL   = 2     /* SW1 bit in SWRCR register */
} AVALANCHE_SYS_RST_MODE_T;

typedef enum AVALANCHE_SYS_RESET_STATUS_tag
{
    HARDWARE_RESET = 0,
    SOFTWARE_RESET0,            /* Caused by writing 1 to SW0 bit in SWRCR register */
    WATCHDOG_RESET,
    SOFTWARE_RESET1             /* Caused by writing 1 to SW1 bit in SWRCR register */
} AVALANCHE_SYS_RESET_STATUS_T;

void avalanche_reset_ctrl(unsigned int reset_module,AVALANCHE_RESET_CTRL_T reset_ctrl);
AVALANCHE_RESET_CTRL_T avalanche_get_reset_status(unsigned int reset_module);
void avalanche_sys_reset(AVALANCHE_SYS_RST_MODE_T mode);
AVALANCHE_SYS_RESET_STATUS_T avalanche_get_sys_last_reset_status(void);

typedef void (*REMOTE_VLYNQ_DEV_RESET_CTRL_FN)(unsigned int reset_module, 
                                               AVALANCHE_RESET_CTRL_T reset_ctrl);

/*****************************************************************************
 * Power Control Module
 *****************************************************************************/

typedef enum AVALANCHE_POWER_CTRL_tag
{
    POWER_CTRL_POWER_UP = 0,
    POWER_CTRL_POWER_DOWN
} AVALANCHE_POWER_CTRL_T;

typedef enum AVALANCHE_SYS_POWER_MODE_tag
{
    GLOBAL_POWER_MODE_RUN       = 0,    /* All system is up */
    GLOBAL_POWER_MODE_IDLE,             /* MIPS is power down, all peripherals working */
    GLOBAL_POWER_MODE_STANDBY,          /* Chip in power down, but clock to ADSKL subsystem is running */
    GLOBAL_POWER_MODE_POWER_DOWN        /* Total chip is powered down */
} AVALANCHE_SYS_POWER_MODE_T;

void avalanche_power_ctrl(unsigned int power_module,  AVALANCHE_POWER_CTRL_T power_ctrl);
AVALANCHE_POWER_CTRL_T avalanche_get_power_status(unsigned int power_module);
void avalanche_set_global_power_mode(AVALANCHE_SYS_POWER_MODE_T power_mode);
AVALANCHE_SYS_POWER_MODE_T avalanche_get_global_power_mode(void);

/*****************************************************************************
 * Wakeup Control
 *****************************************************************************/

typedef enum AVALANCHE_WAKEUP_INTERRUPT_tag
{
    WAKEUP_INT0 = 1,
    WAKEUP_INT1 = 2,
    WAKEUP_INT2 = 4,
    WAKEUP_INT3 = 8
} AVALANCHE_WAKEUP_INTERRUPT_T;

typedef enum TNETV1050_WAKEUP_CTRL_tag
{
    WAKEUP_DISABLED = 0,
    WAKEUP_ENABLED
} AVALANCHE_WAKEUP_CTRL_T;

typedef enum TNETV1050_WAKEUP_POLARITY_tag
{
    WAKEUP_ACTIVE_HIGH = 0,
    WAKEUP_ACTIVE_LOW
} AVALANCHE_WAKEUP_POLARITY_T;

void avalanche_wakeup_ctrl(AVALANCHE_WAKEUP_INTERRUPT_T wakeup_int,
                           AVALANCHE_WAKEUP_CTRL_T wakeup_ctrl,
                           AVALANCHE_WAKEUP_POLARITY_T wakeup_polarity);

/*****************************************************************************
 * GPIO Control
 *****************************************************************************/

typedef enum AVALANCHE_GPIO_PIN_MODE_tag
{
    FUNCTIONAL_PIN = 0,
    GPIO_PIN = 1
} AVALANCHE_GPIO_PIN_MODE_T;

typedef enum AVALANCHE_GPIO_PIN_DIRECTION_tag
{
    GPIO_OUTPUT_PIN = 0,
    GPIO_INPUT_PIN = 1
} AVALANCHE_GPIO_PIN_DIRECTION_T;

typedef enum { GPIO_FALSE, GPIO_TRUE } AVALANCHE_GPIO_BOOL_T;

void avalanche_gpio_init(void);
int avalanche_gpio_ctrl(unsigned int gpio_pin,
                         AVALANCHE_GPIO_PIN_MODE_T pin_mode,
                         AVALANCHE_GPIO_PIN_DIRECTION_T pin_direction);
int avalanche_gpio_out_bit(unsigned int gpio_pin, int value);
int avalanche_gpio_in_bit(unsigned int gpio_pin);
int avalanche_gpio_out_value(unsigned int out_val, unsigned int set_mask, 
                             unsigned int reg_index);
int avalanche_gpio_in_value(unsigned int *in_val, unsigned int reg_index);

unsigned int avalanche_get_chip_version_info(void);

unsigned int avalanche_get_vbus_freq(void);
void         avalanche_set_vbus_freq(unsigned int);


typedef int (*SET_MDIX_ON_CHIP_FN_T)(unsigned int base_addr, unsigned int operation);
int avalanche_set_mdix_on_chip(unsigned int base_addr, unsigned int operation);
unsigned int avalanche_is_mdix_on_chip(void);
#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
int vlynq_get_status(int);
#endif

#endif
