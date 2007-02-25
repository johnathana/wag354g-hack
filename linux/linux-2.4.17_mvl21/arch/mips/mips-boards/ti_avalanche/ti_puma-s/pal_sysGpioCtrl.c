/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2004 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/** \file   pal_sys_gpio_ctrl.c
    \brief  PAL GPIO control code file

    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */
#include "pal.h"

/*****************************************************************************
 * GPIO  Control
 *****************************************************************************/

/****************************************************************************
 * FUNCTION: pal_sys_gpio_init
 ***************************************************************************/
void PAL_sysGpioInit(void)
{
    unsigned int cookie;
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
    *reset_reg |= (1 << AVALANCHE_GPIO_RESET_BIT);
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  
}

/****************************************************************************
 * FUNCTION: PAL_sysGpioCtrl
 ***************************************************************************/
int PAL_sysGpioCtrl(unsigned int gpio_pin,
                    PAL_SYS_GPIO_PIN_MODE_T pin_mode,
                    PAL_SYS_GPIO_PIN_DIRECTION_T pin_direction)
{
    unsigned int cookie;
    volatile unsigned int *gpio_ctrl = (unsigned int*)AVALANCHE_GPIO_ENBL;

    if(gpio_pin >= 32)
	{
        return(-1);
	}

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);

    if(pin_mode == GPIO_PIN)
    {
        *gpio_ctrl |= (1 << gpio_pin);

	    gpio_ctrl = (unsigned int*)AVALANCHE_GPIO_DIR;
        
        if(pin_direction == GPIO_INPUT_PIN)
		{
            *gpio_ctrl |=  (1 << gpio_pin);
		}
        else
		{
            *gpio_ctrl &= ~(1 << gpio_pin);
		}
    }
    else /* FUNCTIONAL PIN */
    {
        *gpio_ctrl &= ~(1 << gpio_pin);
    }

    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  

    return (0);
}

/****************************************************************************
 * FUNCTION: PAL_sysGpioOutBit
 ***************************************************************************/
int PAL_sysGpioOutBit(unsigned int gpio_pin, int value)
{
    unsigned int cookie;
    volatile unsigned int *gpio_out = (unsigned int*) AVALANCHE_GPIO_DATA_OUT;
 
    if(gpio_pin >= 32)
	{
        return(-1);
	}

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);

    if(value == TRUE)
	{
        *gpio_out |=   1 << gpio_pin;
	}
    else
	{
	    *gpio_out &= ~(1 << gpio_pin);
	}

    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  

    return(0);
}

/****************************************************************************
 * FUNCTION: PAL_sysGpioInBit
 ***************************************************************************/
int PAL_sysGpioInBit(unsigned int gpio_pin)
{
    unsigned int cookie;
    volatile unsigned int *gpio_in = (unsigned int*) AVALANCHE_GPIO_DATA_IN;
    int ret_val = 0;
 
    if(gpio_pin >= 32)
	{
        return(-1);
	}

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
    ret_val = ((*gpio_in) & (1 << gpio_pin)) >> gpio_pin;
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  
 
    return (ret_val);
}

/****************************************************************************
 * FUNCTION: PAL_sysGpioOutVal
 ***************************************************************************/
int PAL_sysGpioOutValue(unsigned int out_val, unsigned int out_mask, 
                           unsigned int reg_index)
{
    unsigned int cookie;
    volatile unsigned int *gpio_out = (unsigned int*) AVALANCHE_GPIO_DATA_OUT;

    if(reg_index > 0)
	{
        return(-1);
	}

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
    *gpio_out &= ~out_mask;
    *gpio_out |= out_val;
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  

    return(0);
}

/****************************************************************************
 * FUNCTION: PAL_sysGpioInValue
 ***************************************************************************/
int PAL_sysGpioInValue(unsigned int* in_val, unsigned int reg_index)
{
    unsigned int cookie;
    volatile unsigned int *gpio_in = (unsigned int*) AVALANCHE_GPIO_DATA_IN;
 
    if(reg_index > 0)
	{
        return(-1);
	}

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
    *in_val = *gpio_in;
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);  

    return (0);
}
