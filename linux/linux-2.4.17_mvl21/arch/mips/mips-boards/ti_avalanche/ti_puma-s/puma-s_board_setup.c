/*
 * Jeff Harrell, jharrell@ti.com
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
 * Texas Instruments Sangam specific setup.
 */
#include <linux/config.h>
#include <asm/avalanche/avalanche_map.h>
#include <linux/delay.h>

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)

#include <asm/avalanche/generic/vlynq_hal.h>  
   
#define SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR	30 	/* MSB - 1 bit */
#define SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR	31 	/* MSB bit */
#define SYS_VLYNQ_OPTIONS 		        0x7F; 	/* all options*/

#define VLYNQ0_REMOTE_WINDOW1_OFFSET           (0x0C000000)
#define VLYNQ0_REMOTE_WINDOW1_SIZE             (0x500)


extern VLYNQ_DEV vlynqDevice0;
int vlynq_init_status[2] = {0, 0};
static int reset_hack = 1;
VLYNQ_DEV newVlynqDevice0;


void  vlynq_dev_init(int remote_reset_hack)
{

    volatile unsigned int *reset_base = (unsigned int *) AVALANCHE_RESET_CONTROL_BASE;
    VLYNQ_DEV *pDev = &newVlynqDevice0;

    if(remote_reset_hack)
    {
       pDev = &vlynqDevice0;
    }

    *reset_base &= ~((1 << AVALANCHE_LOW_VLYNQ_RESET_BIT));

    vlynq_delay(20000);

    /* Bring vlynq out of reset if not already done */
    *reset_base |= (1 << AVALANCHE_LOW_VLYNQ_RESET_BIT); 

    *(unsigned long*) AVALANCHE_GPIO_ENBL    &= ~(1<<26);
    vlynq_delay(20000);
    *(unsigned long*) AVALANCHE_GPIO_ENBL    |= (1<<13);
    vlynq_delay(20000);
    *(unsigned long*) AVALANCHE_GPIO_DIR     &= ~(1<<13);
    vlynq_delay(20000);
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT|= (1<<13);  
    vlynq_delay(20000);
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT&= ~(1<<13);
    vlynq_delay(20000);


    /* Initialize the MIPS host vlynq driver for a given vlynq interface */
    pDev->dev_idx = 0;			/* first vlynq module - this parameter is for reference only */
    pDev->module_base = AVALANCHE_LOW_VLYNQ_CONTROL_BASE; 	/*  vlynq0 module base address */
#if defined(CONFIG_AVALANCHE_VLYNQ_CLK_LOCAL)
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_LOCAL;   
#else
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_REMOTE;   
#endif
    pDev->clk_div = 0x01; 			/* board/hardware specific */
    pDev->state =  VLYNQ_DRV_STATE_UNINIT; 	/* uninitialized module */

    /* Populate vlynqDevice0.local_mem & Vlynq0.remote_mem based on system configuration */ 
    /*Local memory configuration */

                 /* Demiurg : not good !*/
#if 0
    pDev->local_mem.Txmap= AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE;
    pDev->remote_mem.RxOffset[0]= VLYNQ0_REMOTE_WINDOW1_OFFSET; /* This is specific to the board on the other end */
    pDev->remote_mem.RxSize[0]=VLYNQ0_REMOTE_WINDOW1_SIZE;
#endif

                /* Demiurg : This is how it should be ! */
                pDev->local_mem.Txmap = PHYSADDR(AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE);
#define VLYNQ_ACX111_MEM_OFFSET     0xC0000000  /* Physical address of ACX111 memory */
#define VLYNQ_ACX111_MEM_SIZE       0x00040000  /* Total size of the ACX111 memory   */
#define VLYNQ_ACX111_REG_OFFSET     0xF0000000  /* PHYS_ADDR of ACX111 control registers   */
#define VLYNQ_ACX111_REG_SIZE       0x00022000  /* Size of ACX111 registers area, MAC+PHY  */
#define ACX111_VL1_REMOTE_SIZE 0x1000000
                pDev->remote_mem.RxOffset[0]  =  VLYNQ_ACX111_MEM_OFFSET;
                pDev->remote_mem.RxSize[0]    =  VLYNQ_ACX111_MEM_SIZE  ;
                pDev->remote_mem.RxOffset[1]  =  VLYNQ_ACX111_REG_OFFSET;
                pDev->remote_mem.RxSize[1]    =  VLYNQ_ACX111_REG_SIZE  ;
                pDev->remote_mem.Txmap        =  0;
                pDev->local_mem.RxOffset[0]   =  AVALANCHE_SDRAM_BASE;
                pDev->local_mem.RxSize[0]     =  ACX111_VL1_REMOTE_SIZE;

    /* Local interrupt configuration */
    pDev->local_irq.intLocal = VLYNQ_INT_LOCAL; 	/* Host handles vlynq interrupts*/
    pDev->local_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* vlynq root isr used */
    pDev->local_irq.map_vector = SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR;
    pDev->local_irq.intr_ptr = 0; /* Since remote interrupts part of vlynq root isr this is unused */

    /* Remote interrupt configuration */
    pDev->remote_irq.intLocal = VLYNQ_INT_REMOTE; 	/* MIPS handles interrupts */
    pDev->remote_irq.intRemote = VLYNQ_INT_ROOT_ISR;  	/* Not significant since MIPS handles interrupts */
    pDev->remote_irq.map_vector = SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR;
    pDev-> remote_irq.intr_ptr = AVALANCHE_INTC_BASE; /* Not significant since MIPS handles interrupts */

   if(reset_hack != 1)
       printk("about to reinit.\n");

   if(vlynq_init(pDev,VLYNQ_INIT_PERFORM_ALL)== 0)
    {

        /* Suraj added the following to keep the 1130 going. */
        vlynq_interrupt_vector_set(&vlynqDevice0, 0 /* intr vector line running into 1130 vlynq */,
                                   0 /* intr mapped onto the interrupt register on remote vlynq and this vlynq */,
                                   VLYNQ_REMOTE_DVC, 0 /* polarity active high */, 0 /* interrupt Level triggered */);

        /* System wide interrupt is 80 for 1130, please note. */
        vlynq_init_status[0] = 1;
        reset_hack = 2;
    }
    else
    {
        if(reset_hack == 1)
            prom_printf("VLYNQ INIT FAILED: Please try cold reboot. \n");
        else
            printk("Failed to initialize the VLYNQ interface at insmod.\n");
    }
}

/* This function is board specific and should be ported for each board. */
void remote_vlynq_dev_reset_ctrl(unsigned int module_reset_bit,
                                 AVALANCHE_RESET_CTRL_T reset_ctrl)
{
    if(module_reset_bit >= 32)
        return;

    switch(module_reset_bit)
    {
        case 0:
            if(OUT_OF_RESET == reset_ctrl)
            { 
                if(reset_hack) return;

                vlynq_delay(20000);
                printk("Un-resetting the remote device. \n");
                vlynq_dev_init(1); 
                printk("Re-initialized the VLYNQ.\n");
                printk("Accessing the memory.%x.\n", *(unsigned int*)0xa4000004);
                reset_hack = 2;
            }
            else if(IN_RESET == reset_ctrl)
            {
                *(unsigned long*) AVALANCHE_GPIO_DATA_OUT |= (1<<13);
                vlynq_delay(20000);
                printk("Resetting the remote device.\n");
                reset_hack = 0;
            }
            else
                ;
        break;

        default:
        break;

    }
}
#endif


extern REMOTE_VLYNQ_DEV_RESET_CTRL_FN p_remote_vlynq_dev_reset_ctrl;

void avalanche_soc_platform_init(void)
{
    /* GPIO INIT */
    avalanche_gpio_init();
    
    avalanche_reset_ctrl(AVALANCHE_UART1_RESET_BIT,OUT_OF_RESET);
    avalanche_reset_ctrl(AVALANCHE_GPIO_RESET_BIT, OUT_OF_RESET);
    avalanche_reset_ctrl(AVALANCHE_LOW_EMAC_RESET_BIT, IN_RESET);

    /* CLK CTRl INIT */                                                                
    avalanche_clkc_init(REFCLK_FREQ, USBCLK_FREQ);                   

   /* initialize vbus frequency */                              
    avalanche_set_vbus_freq(avalanche_clkc_get_freq(CLKC_VBUS));

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    p_remote_vlynq_dev_reset_ctrl = remote_vlynq_dev_reset_ctrl;
    vlynq_dev_init(1);
#endif

}
