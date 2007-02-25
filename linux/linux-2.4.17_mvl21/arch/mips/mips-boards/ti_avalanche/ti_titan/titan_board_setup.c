/*
 * Suraj S Iyer, ssiyer@ti.com
 * Copyright (C) 2001 Texas Instruments, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Texas Instruments 1050 specific setup.
 */
#include <linux/config.h>
#include <linux/mm.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>

#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/titan/titan_clk_cntl.h>


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
#include <asm/avalanche/generic/vlynq_hal.h>  
   
#define SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR	30 	/* MSB - 1 bit */
#define SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR	31 	/* MSB bit */
#define SYS_VLYNQ_OPTIONS 		        0x7F; 	/* all options*/

#define VLYNQ0_REMOTE_WINDOW1_OFFSET           (0x0C000000)
#define VLYNQ0_REMOTE_WINDOW1_SIZE             (0x500)


#define VLYNQ1_REMOTE_WINDOW1_OFFSET           (0x0C000000)
#define VLYNQ1_REMOTE_WINDOW1_SIZE             (0x500)

int    vlynq_init_status[2] = {0, 0};
extern VLYNQ_DEV vlynqDevice0, vlynqDevice1;

void  vlynq_dev_init()
{
    volatile unsigned int *reset_base = (unsigned int *) AVALANCHE_RESET_CONTROL_BASE;

    /* Bring vlynq out of reset if not already done */
    *reset_base |= (1 << AVALANCHE_LOW_VLYNQ_RESET_BIT) | (1 << AVALANCHE_HIGH_VLYNQ_RESET_BIT); 
    vlynq_delay(256);

    /* Initialize the MIPS host vlynq driver for a given vlynq interface */
    vlynqDevice0.dev_idx = 0;			/* first vlynq module - this parameter is for reference only */
    vlynqDevice0.module_base = AVALANCHE_LOW_VLYNQ_CONTROL_BASE; 	/*  vlynq0 module base address */
#if defined(CONFIG_AVALANCHE_VLYNQ_CLK_LOCAL)
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_LOCAL;   
#else
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_REMOTE;   
#endif
    vlynqDevice0.clk_div = 0x01; 			/* board/hardware specific */
    vlynqDevice0.state =  VLYNQ_DRV_STATE_UNINIT; 	/* uninitialized module */

    /* Populate vlynqDevice0.local_mem & Vlynq0.remote_mem based on system configuration */ 
    /*Local memory configuration */
    vlynqDevice0.local_mem.Txmap= AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE;
    vlynqDevice0.remote_mem.RxOffset[0]= VLYNQ0_REMOTE_WINDOW1_OFFSET; /* This is specific to the board on the other end */
    vlynqDevice0.remote_mem.RxSize[0]=VLYNQ0_REMOTE_WINDOW1_SIZE;

    /* Local interrupt configuration */
    vlynqDevice0.local_irq.intLocal = VLYNQ_INT_LOCAL; 	/* Host handles vlynq interrupts*/
    vlynqDevice0.local_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* vlynq root isr used */
    vlynqDevice0.local_irq.map_vector = SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR;
    vlynqDevice0.local_irq.intr_ptr = 0; /* Since remote interrupts part of vlynq root isr this is unused */

    /* Remote interrupt configuration */
    vlynqDevice0.remote_irq.intLocal = VLYNQ_INT_REMOTE; 	/* MIPS handles interrupts */
    vlynqDevice0.remote_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* Not significant since MIPS handles interrupts */
    vlynqDevice0.remote_irq.map_vector = SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR;
    vlynqDevice0. remote_irq.intr_ptr = AVALANCHE_INTC_BASE; /* Not significant since MIPS handles interrupts */

    /* Initialize the MIPS host vlynq driver for a given vlynq interface */
    vlynqDevice1.dev_idx = 1;			/* first vlynq module - this parameter is for reference only */
    vlynqDevice1.module_base = AVALANCHE_HIGH_VLYNQ_CONTROL_BASE; 	/* titan vlynq1 module base address */
#if defined(CONFIG_AVALANCHE_VLYNQ_CLK_LOCAL)
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_LOCAL;   
#else
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_REMOTE;   
#endif
    vlynqDevice1.clk_div = 0x01; 			/* board/hardware specific */
    vlynqDevice1.state =  VLYNQ_DRV_STATE_UNINIT; 	/* uninitialized module */

    /* Populate vlynqDevice0.local_mem & Vlynq0.remote_mem based on system configuration */ 
    /*Local memory configuration */
    vlynqDevice1.local_mem.Txmap= AVALANCHE_HIGH_VLYNQ_MEM_MAP_BASE;
    vlynqDevice1.remote_mem.RxOffset[0]=VLYNQ1_REMOTE_WINDOW1_OFFSET; /* This is specific to the board on the other end */
    vlynqDevice1.remote_mem.RxSize[0]= VLYNQ1_REMOTE_WINDOW1_SIZE;


    /* Local interrupt configuration */
    vlynqDevice1.local_irq.intLocal = VLYNQ_INT_LOCAL; 	/* Host handles vlynq interrupts*/
    vlynqDevice1.local_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* vlynq root isr used */
    vlynqDevice1.local_irq.map_vector = SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR;
    vlynqDevice1.local_irq.intr_ptr = AVALANCHE_INTC_BASE; /* Since remote interrupts part of vlynq root isr this is unused */

    /* Remote interrupt configuration */
    vlynqDevice1.remote_irq.intLocal = VLYNQ_INT_REMOTE; 	/* MIPS handles interrupts */
    vlynqDevice1.remote_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* Not significant since MIPS handles interrupts */
    vlynqDevice1.remote_irq.map_vector = SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR;
    vlynqDevice1. remote_irq.intr_ptr = AVALANCHE_INTC_BASE; /* Not significant since MIPS handles interrupts */

}
#endif


void avalanche_soc_platform_init(void)
{
    volatile PIN_SEL_REG_ARRAY_T *pin_sel_array = (PIN_SEL_REG_ARRAY_T*) AVALANCHE_PIN_SEL_BASE;
    volatile unsigned int *eswitch_control_reg  = (unsigned int*) AVALANCHE_ESWITCH_CONTROL;
    volatile TITAN_GPIO_CONTROL_T *gpio_cntl    = (TITAN_GPIO_CONTROL_T *) AVALANCHE_GPIO_BASE;
 
    /* Get entities on the board out of reset. */
    volatile unsigned int *reset_base = (unsigned int *) AVALANCHE_RESET_CONTROL_BASE;
    int count = 0;

    *reset_base |= (1 << AVALANCHE_GPIO_RESET_BIT) | (1 << AVALANCHE_ESWITCH_RESET_BIT) \
		   | (1 << AVALANCHE_LOW_EPHY_RESET_BIT) | (1 << AVALANCHE_HIGH_EPHY_RESET_BIT);

    pin_sel_array->reg[0] &= ~0x000ff000;  /*SSP bits:     b xxxx xxxx xxxx 1111 1111 xxxx xxxx xxxx */
    pin_sel_array->reg[0] |=  0x00055000;  /*SSP bits:     b xxxx xxxx xxxx 0101 0101 xxxx xxxx xxxx */
  
    pin_sel_array->reg[3] &= ~0xff000000;  /*Gpio bits:    b 1111 1111 xxxx xxxx xxxx xxxx xxxx xxxx */
    pin_sel_array->reg[3] |=  0x57000000;  /*Bit setting:  b 0101 0111 xxxx xxxx xxxx xxxx xxxx xxxx */  

    pin_sel_array->reg[4] &= ~0x00003fc0;  /*Gpio bits:    b xxxx xxxx xxxx xxxx xx11 1111 11xx xxxx */
    pin_sel_array->reg[4] |=  0x00001740;  /*Bit setting:  b xxxx xxxx xxxx xxxx xx01 0111 01xx xxxx */

    /* Code here to adjust the bias. Need to work on this. */
#if defined(CONFIG_1050_SDB)
    avalanche_gpio_ctrl(6, GPIO_PIN, GPIO_OUTPUT_PIN);
    avalanche_gpio_ctrl(7, GPIO_PIN, GPIO_OUTPUT_PIN);

    {
        int count = 30;
	while(count--)
	{
            avalanche_gpio_out(6, 1);
	    mdelay(50);
	    avalanche_gpio_out(6, 0);
	}
    }
#endif

#if defined(CONFIG_AVALANCHE_SSP)
    pin_sel_array->reg[7] &= ~0x000ff000;  /*Kpad bits:    b xxxx xxxx xxxx 1111 1111 xxxx xxxx xxxx */
    pin_sel_array->reg[7] |= ~0x00055000;  /*Bit setting:  b xxxx xxxx xxxx 0101 0101 xxxx xxxx xxxx */
#endif

    *reset_base |= 1 << AVALANCHE_KEYPAD_CTRL_RESET_BIT;
    pin_sel_array->reg[6] &= ~0xfcffc000;  /*KPad bits:    b 1111 11xx 1111 1111 11xx xxxx xxxx xxxx */
    pin_sel_array->reg[6] |=  0x54554000;  /*bit setting:  b 0101 01xx 0101 0101 01xx xxxx xxxx xxxx */

    pin_sel_array->reg[7] &= ~0x003fcc3f;  /*kPad bits:    b xxxx xxxx xx11 1111 11xx 11xx xx11 1111 */
    pin_sel_array->reg[7] |=  0x00154415;  /*Bit setting:  b xxxx xxxx xx01 0101 01xx 01xx xx01 0101 */

    *reset_base |= 1 << AVALANCHE_LCD_CTRL_RESET_BIT;
    pin_sel_array->reg[7]  &= ~0xcc000000;  /*LCD bits:     b 11xx 11xx xxxx xxxx xxxx xxxx xxxx xxxx */
    pin_sel_array->reg[7]  |=  0x44000000;  /*bit setting:  b 0100 0100 xxxx xxxx xxxx xxxx xxxx xxxx */

    pin_sel_array->reg[8]  &= ~0xff3cff3f;  /*LCD bits:     b 1111 1111 xx11 11xx 1111 1111 xx11 1111 */
    pin_sel_array->reg[8]  |=  0x55145515;  /*bit setting:  b 0101 0101 xx01 01xx 0101 0101 xx01 0101 */

    pin_sel_array->reg[9]  &= ~0xff0f;      /*LCD bits:     b xxxx xxxx xxxx xxxx 1111 1111 xxxx 1111 */
    pin_sel_array->reg[9]  |=  0x5505;      /*bit setting:  b xxxx xxxx xxxx xxxx 0101 0101 xxxx 0101 */

    pin_sel_array->reg[9]  &= ~0xf03f0000;  /*Eth LED bits: b 1111 xxxx xx11 1111 xxxx xxxx xxxx xxxx */
    pin_sel_array->reg[9]  |=  0x50150000;  /*bit setting:  b 0101 xxxx xx01 0101 xxxx xxxx xxxx xxxx */
   
    pin_sel_array->reg[10] &= ~0x3f;        /*Eth LED bits: b xxxx xxxx xxxx xxxx xxxx xxxx xx11 1111 */
    pin_sel_array->reg[10] |=  0x15;        /*bit setting:  b xxxx xxxx xxxx xxxx xxxx xxxx xx01 0101 */ 


#if defined CONFIG_MIPS_TNETV1050SDB
    // Warning: This is a temperory code and shall be removed in subsequent releases.
    // The same functionality shall be provided using the custom kernel API(s).
    prom_printf("Adjusting the bias for LCD....\n");
    gpio_enable();                             
    lcd_ctrl(0,30);                            
#endif


    /* Put CPMac0 and CPMac1 into reset mode */
    avalanche_reset_ctrl(AVALANCHE_LOW_CPMAC_RESET_BIT,IN_RESET);
    avalanche_reset_ctrl(AVALANCHE_HIGH_CPMAC_RESET_BIT,IN_RESET);


    /* CLK CTRL INIT */                                                                
    avalanche_clkc_init(AUDCLK_FREQ, REFCLK_FREQ, ALTCLK_FREQ);                   

    /* Set LCD clock to 50 MHz */
   avalanche_clkc_set_freq(CLKC_LCD,CLK_MHZ(50));

    /* initialize vbus frequency */
    avalanche_set_vbus_freq(avalanche_clkc_get_freq(CLKC_VBUS));
    
#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    vlynq_dev_init();
#endif

    /* The board comes up in the two CPMAC mode. */
    *eswitch_control_reg = 0x010000ce;                                                        
}


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
 * FUNCTION: avalanche_gpio_ctrl
 ***************************************************************************/
int avalanche_gpio_ctrl(unsigned int gpio_pin, AVALANCHE_GPIO_PIN_MODE_T enable,
                        AVALANCHE_GPIO_PIN_DIRECTION_T pin_direction)
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


int avalanche_gpio_out_bit(unsigned int gpio_pin, BOOL value)
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

int avalanche_gpio_in_bit(unsigned int gpio_pin)
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


int avalanche_gpio_out_value(unsigned int out_val, unsigned int set_mask, 
                             unsigned int reg_index)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;

    if(reg_index > 1)
        return (-1);

    gpio_cntl->data_out[reg_index] &= ~set_mask;
    gpio_cntl->data_out[reg_index] |= out_val & set_mask;
	
    return (-1);	    
}

int avalanche_gpio_in_value(unsigned int *in_val, unsigned int reg_index)
{
    volatile TITAN_GPIO_CONTROL_T   *gpio_cntl     = (TITAN_GPIO_CONTROL_T*) TITAN_GPIO_DATA_IN_0;

    if(reg_index > 1)
       return (-1);

    *in_val = gpio_cntl->data_in[reg_index];

    return (0);
}


#if defined CONFIG_MIPS_TNETV1050SDB

//***********************************************************************
//  This is a temperory code and shall be removed in subsequent releases.
//***********************************************************************

#define REG32_DATA(addr)            (*(volatile unsigned int *)(addr))    
#define REG32_WRITE(addr, data)     REG32_DATA(addr) = data               
#define REG32_READ(addr, data)      data = (unsigned int) REG32_DATA(addr)
                                                                          
                                                                          
/* Temperory code to be removed. : Shall be cleaned up in next release. */                        
void gpio_enable(void)                                                    
{                                                                         
                                                                          
        /* Set the Mux values for LCD Bias up/down TNI*/                  
        UINT32 temp_value;                                                
        REG32_READ(0xa861161c,temp_value);                                
        temp_value &= ~0x3C00;                                            
        temp_value |= 0x1400;                                             
        REG32_WRITE(0xa861161c,temp_value);                               
                                                                          
        REG32_READ(0xa8611600,temp_value);                                
        temp_value |= 0x40;                                               
        REG32_WRITE(0xa8611600,temp_value);                               
                                                                          
        /* Set GPIO 6 & 7 to output */                                    
        REG32_READ(0xa8610910,temp_value);                                
        temp_value &= ~0xC0;                                              
        REG32_WRITE(0xa8610910,temp_value);                               
                                                                          
        REG32_READ(0xa8610900,temp_value);                                
        temp_value |= 0xC0;                                               
        REG32_WRITE(0xa8610900,temp_value);                               
                                                                          
}                                                                         
     
                                                                          
/* Temperory code to be removed. : Shall be cleaned up in next release. */                        
void lcd_up(void)                                                         
{                                                                         
        UINT32 reg_value;                                                
	volatile unsigned int temp; 
                                                                          
        /* Pulse up */                                                    
        REG32_READ(0xa8610908,reg_value);                                 
        reg_value &= ~0x40;                                               
        REG32_WRITE(0xa8610908,reg_value);                                
                                   
	for(temp = 0; temp < 150 * 1000 * 25;temp++);                                       
//      sleep_on_timeout(&wait_queue,5);      
                                                                          
        /* Pulse down */                                                  
        REG32_READ(0xa8610908,reg_value);                                 
        reg_value |= 0x40;                                                
        REG32_WRITE(0xa8610908,reg_value);                                
                                                                          
                                                                          
}
                                                                         

/* Temperory code to be removed. : Shall be cleaned up in next release. */                        
void lcd_down(void)                           
{                                             
        UINT32 reg_value;                     
	volatile unsigned int temp; 
                                              
        /* Pulse up */                        
        REG32_READ(0xa8610908,reg_value);     
        reg_value &= ~0x80;                   
        REG32_WRITE(0xa8610908,reg_value);    
                                       
       
	for(temp = 0; temp < 150 * 1000 * 25;temp++);                                       
//      sleep_on_timeout(&wait_queue,5);      
                                              
        /* Pulse down */                      
        REG32_READ(0xa8610908,reg_value);     
        reg_value |= 0x80;                    
        REG32_WRITE(0xa8610908,reg_value);    
                                              
                                              
}                                             
                                              
/* Temperory code to be removed. : Shall be cleaned up in next release. */                        
void lcd_ctrl(int is_up, UINT8 cnt)           
{                                             
    UINT8 i;                                  
                                              
    for(i = 0; i < cnt; i++)                  
    {                                         
        if(is_up)                             
        {                                     
            lcd_up();                         
        }                                     
        else                                  
        {                                     
            lcd_down();                       
        }                                     
    }                                         
}                                             
#endif                                                  


EXPORT_SYMBOL(avalanche_gpio_ctrl);
EXPORT_SYMBOL(avalanche_gpio_in_bit);
EXPORT_SYMBOL(avalanche_gpio_out_bit);
EXPORT_SYMBOL(avalanche_gpio_out_value);
EXPORT_SYMBOL(avalanche_gpio_in_value);
