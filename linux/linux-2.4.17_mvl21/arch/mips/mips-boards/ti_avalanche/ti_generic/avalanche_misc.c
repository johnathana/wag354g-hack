#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/generic/avalanche_misc.h>
#include <linux/module.h>
#include <linux/spinlock.h>

static unsigned int avalanche_vbus_freq;

REMOTE_VLYNQ_DEV_RESET_CTRL_FN p_remote_vlynq_dev_reset_ctrl = NULL;

/*****************************************************************************
 * Reset Control Module.
 *****************************************************************************/
void avalanche_reset_ctrl(unsigned int module_reset_bit, 
                          AVALANCHE_RESET_CTRL_T reset_ctrl)
{
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;

    if(module_reset_bit >= 32 && module_reset_bit < 64)
        return;

    if(module_reset_bit >= 64)
    {
        if(p_remote_vlynq_dev_reset_ctrl)
            return(p_remote_vlynq_dev_reset_ctrl(module_reset_bit - 64, reset_ctrl));
        else
            return;
    }
    
    if(reset_ctrl == OUT_OF_RESET)
        *reset_reg |= 1 << module_reset_bit;
    else
        *reset_reg &= ~(1 << module_reset_bit);
}

AVALANCHE_RESET_CTRL_T avalanche_get_reset_status(unsigned int module_reset_bit)
{
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;

    return (((*reset_reg) & (1 << module_reset_bit)) ? OUT_OF_RESET : IN_RESET );
}

void avalanche_sys_reset(AVALANCHE_SYS_RST_MODE_T mode)
{
    volatile unsigned int *sw_reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_SWRCR;
    *sw_reset_reg =  mode;
}

#define AVALANCHE_RST_CTRL_RSR_MASK 0x3

AVALANCHE_SYS_RESET_STATUS_T avalanche_get_sys_last_reset_status()
{
    volatile unsigned int *sys_reset_status = (unsigned int*) AVALANCHE_RST_CTRL_RSR;

    return ( (AVALANCHE_SYS_RESET_STATUS_T) (*sys_reset_status & AVALANCHE_RST_CTRL_RSR_MASK) );
}


/*****************************************************************************
 * Power Control Module
 *****************************************************************************/
#define AVALANCHE_GLOBAL_POWER_DOWN_MASK    0x3FFFFFFF      /* bit 31, 30 masked */
#define AVALANCHE_GLOBAL_POWER_DOWN_BIT     30              /* shift to bit 30, 31 */


void avalanche_power_ctrl(unsigned int module_power_bit, AVALANCHE_POWER_CTRL_T power_ctrl)
{
    volatile unsigned int *power_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    if (power_ctrl == POWER_CTRL_POWER_DOWN)
        /* power down the module */
        *power_reg |= (1 << module_power_bit);
    else
        /* power on the module */
        *power_reg &= (~(1 << module_power_bit));
}

AVALANCHE_POWER_CTRL_T avalanche_get_power_status(unsigned int module_power_bit)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    return (((*power_status_reg) & (1 << module_power_bit)) ? POWER_CTRL_POWER_DOWN : POWER_CTRL_POWER_UP);
}

void avalanche_set_global_power_mode(AVALANCHE_SYS_POWER_MODE_T power_mode)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    *power_status_reg &= AVALANCHE_GLOBAL_POWER_DOWN_MASK;
    *power_status_reg |= ( power_mode << AVALANCHE_GLOBAL_POWER_DOWN_BIT);
}

AVALANCHE_SYS_POWER_MODE_T avalanche_get_global_power_mode(void)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    return((AVALANCHE_SYS_POWER_MODE_T) (((*power_status_reg) & (~AVALANCHE_GLOBAL_POWER_DOWN_MASK)) 
                                           >> AVALANCHE_GLOBAL_POWER_DOWN_BIT));
}

#if defined (CONFIG_AVALANCHE_GENERIC_GPIO)
/*****************************************************************************
 * GPIO  Control
 *****************************************************************************/

/****************************************************************************
 * FUNCTION: avalanche_gpio_ctrl
 ***************************************************************************/
int avalanche_gpio_ctrl(unsigned int gpio_pin,
                        AVALANCHE_GPIO_PIN_MODE_T pin_mode,
                        AVALANCHE_GPIO_PIN_DIRECTION_T pin_direction)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *gpio_ctrl = (unsigned int*)AVALANCHE_GPIO_ENBL;

    if(gpio_pin >= 32)
        return(-1);

    spin_lock_irqsave(&closeLock, closeFlag);

    if(pin_mode == GPIO_PIN)
    {
        *gpio_ctrl |= (1 << gpio_pin);

	gpio_ctrl = (unsigned int*)AVALANCHE_GPIO_DIR;
        
        if(pin_direction == GPIO_INPUT_PIN)
            *gpio_ctrl |=  (1 << gpio_pin);
        else
            *gpio_ctrl &= ~(1 << gpio_pin);
    }
    else /* FUNCTIONAL PIN */
    {
        *gpio_ctrl &= ~(1 << gpio_pin);
    }

    spin_unlock_irqrestore(&closeLock, closeFlag);  

    return (0);
}

/****************************************************************************
 * FUNCTION: avalanche_gpio_out
 ***************************************************************************/
int avalanche_gpio_out_bit(unsigned int gpio_pin, int value)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *gpio_out = (unsigned int*) AVALANCHE_GPIO_DATA_OUT;
 
    if(gpio_pin >= 32)
        return(-1);
    
    spin_lock_irqsave(&closeLock, closeFlag);
    if(value == TRUE)
        *gpio_out |= 1 << gpio_pin;
    else
	*gpio_out &= ~(1 << gpio_pin);
    spin_unlock_irqrestore(&closeLock, closeFlag);

    return(0);
}

/****************************************************************************
 * FUNCTION: avalanche_gpio_in
 ***************************************************************************/
int avalanche_gpio_in_bit(unsigned int gpio_pin)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *gpio_in = (unsigned int*) AVALANCHE_GPIO_DATA_IN;
    int ret_val = 0;
 
    if(gpio_pin >= 32)
        return(-1);

    spin_lock_irqsave(&closeLock, closeFlag); 
    ret_val = ((*gpio_in) & (1 << gpio_pin));
    spin_unlock_irqrestore(&closeLock, closeFlag);
 
    return (ret_val);
}

/****************************************************************************
 * FUNCTION: avalanche_gpio_out_val
 ***************************************************************************/
int avalanche_gpio_out_value(unsigned int out_val, unsigned int out_mask, 
                           unsigned int reg_index)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *gpio_out = (unsigned int*) AVALANCHE_GPIO_DATA_OUT;

    if(reg_index > 0)
        return(-1);

    spin_lock_irqsave(&closeLock, closeFlag);
    *gpio_out &= ~out_mask;
    *gpio_out |= out_val;
    spin_unlock_irqrestore(&closeLock, closeFlag);

    return(0);
}

/****************************************************************************
 * FUNCTION: avalanche_gpio_in_value
 ***************************************************************************/
int avalanche_gpio_in_value(unsigned int* in_val, unsigned int reg_index)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *gpio_in = (unsigned int*) AVALANCHE_GPIO_DATA_IN;
 
    if(reg_index > 0)
        return(-1);

    spin_lock_irqsave(&closeLock, closeFlag);
    *in_val = *gpio_in;
    spin_unlock_irqrestore(&closeLock, closeFlag);

    return (0);
}
//add by lejun
void sys_led_init(void)
{
        avalanche_gpio_ctrl(4, GPIO_PIN, GPIO_OUTPUT_PIN);
        avalanche_gpio_out_bit(4, FALSE);

}

void wan_led_init(void)
{
//      int i,j;
        avalanche_gpio_ctrl(2, GPIO_PIN, GPIO_OUTPUT_PIN);
        avalanche_gpio_ctrl(3, GPIO_PIN, GPIO_OUTPUT_PIN);

        avalanche_gpio_out_bit(2, FALSE);
        avalanche_gpio_out_bit(3, FALSE);
#if 0
        for(j = 0; j < 10; j ++)
        {
                printk("************************test now\n");
                avalanche_gpio_out_bit(2, TRUE);
                avalanche_gpio_out_bit(3, FALSE);
                for (i = 0; i < 0x1000000; i ++);
                avalanche_gpio_out_bit(2, FALSE);
                avalanche_gpio_out_bit(3, TRUE);
                for (i = 0; i < 0x1000000; i ++);
        }
#endif
}
//over
/****************************************************************************
 * FUNCTION: avalanche_gpio_init
 ***************************************************************************/
void avalanche_gpio_init(void)
{
    spinlock_t closeLock;
    unsigned int closeFlag;
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;
    spin_lock_irqsave(&closeLock, closeFlag);
    *reset_reg |= (1 << AVALANCHE_GPIO_RESET_BIT);
    spin_unlock_irqrestore(&closeLock, closeFlag);  
    //add by leijun
    sys_led_init();
    wan_led_init();
    //
}


#endif

/***********************************************************************
 *
 *    Wakeup Control Module for TNETV1050 Communication Processor
 *
 ***********************************************************************/

#define AVALANCHE_WAKEUP_POLARITY_BIT   16

void avalanche_wakeup_ctrl(AVALANCHE_WAKEUP_INTERRUPT_T wakeup_int,
                           AVALANCHE_WAKEUP_CTRL_T      wakeup_ctrl,
                           AVALANCHE_WAKEUP_POLARITY_T  wakeup_polarity)
{
    volatile unsigned int *wakeup_status_reg = (unsigned int*) AVALANCHE_WAKEUP_CTRL_WKCR;

    /* enable/disable */
    if (wakeup_ctrl == WAKEUP_ENABLED)
        /* enable wakeup */
        *wakeup_status_reg |= wakeup_int;
    else
        /* disable wakeup */
        *wakeup_status_reg &= (~wakeup_int);

    /* set polarity */
    if (wakeup_polarity == WAKEUP_ACTIVE_LOW)
        *wakeup_status_reg |=  (wakeup_int << AVALANCHE_WAKEUP_POLARITY_BIT);
    else
        *wakeup_status_reg &= ~(wakeup_int << AVALANCHE_WAKEUP_POLARITY_BIT);
}

void avalanche_set_vbus_freq(unsigned int new_vbus_freq)
{
    avalanche_vbus_freq = new_vbus_freq;
}

unsigned int avalanche_get_vbus_freq()
{
    return(avalanche_vbus_freq);
}

unsigned int avalanche_get_chip_version_info()
{
    return(*(volatile unsigned int*)AVALANCHE_CVR);
}

SET_MDIX_ON_CHIP_FN_T p_set_mdix_on_chip_fn = NULL;

int avalanche_set_mdix_on_chip(unsigned int base_addr, unsigned int operation)
{
    if(p_set_mdix_on_chip_fn)
        return (p_set_mdix_on_chip_fn(base_addr, operation));
    else
        return(-1);
}

unsigned int avalanche_is_mdix_on_chip(void)
{
    return(p_set_mdix_on_chip_fn ? 1:0);
}

/* software abstraction for HAL */

#include <linux/slab.h>

void* os_platform_malloc(unsigned int size)
{
 return kmalloc(size,GFP_KERNEL);
}
extern void  os_platform_free(void *p)
{
kfree(p);
}


void* os_platform_memset(void*p, int num, unsigned int size)
{
        return memset(p,num,size);
}


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
extern int vlynq_init_status[2];
int vlynq_get_status(int vlynq_iface)
{
	return vlynq_init_status[vlynq_iface];
}
#endif
EXPORT_SYMBOL(avalanche_reset_ctrl);
EXPORT_SYMBOL(avalanche_get_reset_status);
EXPORT_SYMBOL(avalanche_sys_reset);
EXPORT_SYMBOL(avalanche_get_sys_last_reset_status);
EXPORT_SYMBOL(avalanche_power_ctrl);
EXPORT_SYMBOL(avalanche_get_power_status);
EXPORT_SYMBOL(avalanche_set_global_power_mode);
EXPORT_SYMBOL(avalanche_get_global_power_mode);
EXPORT_SYMBOL(avalanche_set_mdix_on_chip);
EXPORT_SYMBOL(avalanche_is_mdix_on_chip);
EXPORT_SYMBOL(os_platform_malloc);
EXPORT_SYMBOL(os_platform_free);
EXPORT_SYMBOL(os_platform_memset);



#if defined (CONFIG_AVALANCHE_GENERIC_GPIO)
EXPORT_SYMBOL(avalanche_gpio_init);
EXPORT_SYMBOL(avalanche_gpio_ctrl);
EXPORT_SYMBOL(avalanche_gpio_out_bit);
EXPORT_SYMBOL(avalanche_gpio_in_bit);
EXPORT_SYMBOL(avalanche_gpio_out_value);
EXPORT_SYMBOL(avalanche_gpio_in_value);
#endif

EXPORT_SYMBOL(avalanche_set_vbus_freq);
EXPORT_SYMBOL(avalanche_get_vbus_freq);

EXPORT_SYMBOL(avalanche_get_chip_version_info);
#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
EXPORT_SYMBOL(vlynq_get_status);
#endif
