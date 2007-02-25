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

#include <pal.h>

/**********************************************************************
 *  GPIO Control
 **********************************************************************/

typedef struct 
{
    int pinSelReg;
    int shift;
    int func;

} GPIO_CFG;

GPIO_CFG gptable[]= {
		      /* PIN_SEL_REG, START_BIT, GPIO_CFG_MUX_VALUE */
	              {4,24,1},
		      {4,26,1},
		      {4,28,1},
		      {4,30,1},
		      {5,6,1},
		      {5,8,1},
		      {5,10,1},
		      {5,12,1},
		      {7,14,3},
		      {7,16,3},
		      {7,18,3},
		      {7,20,3},
		      {7,22,3},
		      {7,26,3},
		      {7,28,3},
		      {7,30,3},
                      {8,0,3},
		      {8,2,3},
		      {8,4,3},
		      {8,10,3},
		      {8,14,3},
		      {8,16,3},
		      {8,18,3},
		      {8,20,3},
		      {9,8,3},
		      {9,10,3},
		      {9,12,3},
		      {9,14,3},
		      {9,18,3},
		      {9,20,3},
		      {9,24,3},
		      {9,26,3},
		      {9,28,3},
		      {9,30,3},
		      {10,0,3},
		      {10,2,3},
		      {10,8,3},
		      {10,10,3},
		      {10,12,3},
		      {10,14,3},
		      {13,12,3},
		      {13,14,3},
		      {13,16,3},
		      {13,18,3},
		      {13,24,3},
		      {13,26,3},
		      {13,28,3},
		      {13,30,3},
		      {14,2,3},
		      {14,6,3},
		      {14,8,3},
		      {14,12,3}
};
		      

/****************************************************************************
 * FUNCTION: PAL_sysGpioCtrl
 ***************************************************************************/
int PAL_sysGpioCtrl(unsigned int gpio_pin, PAL_SYS_GPIO_PIN_MODE_T enable,
                        PAL_SYS_GPIO_PIN_DIRECTION_T pin_direction)
{
    int reg_index = 0;
    int mux_status;
    GPIO_CFG  gpio_cfg;
    volatile PIN_SEL_REG_ARRAY_T *pin_sel_array = (PIN_SEL_REG_ARRAY_T*) AVALANCHE_PIN_SEL_BASE;
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;
	
    if (gpio_pin > 51 )
        return(-1);

    gpio_cfg = gptable[gpio_pin];
    mux_status = (pin_sel_array->reg[gpio_cfg.pinSelReg] >> gpio_cfg.shift) & 0x3;

    if(!((mux_status == 0 /* tri-stated */ ) || (mux_status == gpio_cfg.func /*GPIO functionality*/)))
    {
        return(-1); /* Pin have been configured for non GPIO funcs. */
    }

    /* Set the pin to be used as GPIO. */
    pin_sel_array->reg[gpio_cfg.pinSelReg] |= ((gpio_cfg.func & 0x3) << gpio_cfg.shift);

    /* Check whether gpio refers to the first GPIO reg or second. */
    if(gpio_pin > 31)
    {
	reg_index = 1;
	gpio_pin -= 32;
    }

    if(enable)
        gpio_cntl->enable[reg_index] |=  (1 << gpio_pin); /* Enable */
    else
	gpio_cntl->enable[reg_index] &= ~(1 << gpio_pin);

    if(pin_direction)
        gpio_cntl->dir[reg_index] |=  (1 << gpio_pin); /* Input */
    else
	gpio_cntl->dir[reg_index] &= ~(1 << gpio_pin);

    return(0);

}/* end of function gpio_ctrl */


int PAL_sysGpioOutBit(unsigned int gpio_pin, BOOL value)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;
    unsigned int reg_index = 0;
    
    if(gpio_pin > 51)
        return (-1);

    if(gpio_pin > 31)
    {
        gpio_pin -= 32;
        reg_index = 1;
    }	

    if(value)
        gpio_cntl->data_out[reg_index] |=  (1 << gpio_pin);
    else
	gpio_cntl->data_out[reg_index] &=  ~(1 << gpio_pin);
    
    return (0);
}/* end of function gpio_out */


/*
 * Return Values : 
 *
 *  0/1 - Read Data 
 *  Negative Values - Error Code.
 */

int PAL_sysGpioInBit(unsigned int gpio_pin)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;
    unsigned int reg_index = 0;
    
    if(gpio_pin > 51)
        return (-1);

    if(gpio_pin > 31)
    {
        gpio_pin -= 32;
        reg_index = 1;
    }	

    return ((gpio_cntl->data_in[reg_index] >> gpio_pin) & 0x1);

}/* end of function gpio_in */


int PAL_sysGpioOutValue(unsigned int out_val, unsigned int set_mask, 
                             unsigned int reg_index)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;

    if(reg_index > 1)
        return (-1);

    gpio_cntl->data_out[reg_index] &= ~set_mask;
    gpio_cntl->data_out[reg_index] |= out_val & set_mask;
	
    return (-1);	    
}

int PAL_sysGpioInValue(unsigned int *in_val, unsigned int reg_index)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;

    if(reg_index > 1)
       return (-1);

    *in_val = gpio_cntl->data_in[reg_index];

    return (0);
}

