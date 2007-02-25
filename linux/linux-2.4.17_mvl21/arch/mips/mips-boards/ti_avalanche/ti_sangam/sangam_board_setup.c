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
#include "../../../../../../../include/cy_codepattern.h"


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
#include <asm/avalanche/generic/vlynq_hal.h>  
   
#define SYS_VLYNQ_LOCAL_INTERRUPT_VECTOR	30 	/* MSB - 1 bit */
#define SYS_VLYNQ_REMOTE_INTERRUPT_VECTOR	31 	/* MSB bit */
#define SYS_VLYNQ_OPTIONS 		        0x7F; 	/* all options*/

/* These defines are board specific */


#define VLYNQ0_REMOTE_WINDOW1_OFFSET           (0x0C000000)
#define VLYNQ0_REMOTE_WINDOW1_SIZE             (0x500)


#define VLYNQ1_REMOTE_WINDOW1_OFFSET           (0x0C000000)
#define VLYNQ1_REMOTE_WINDOW1_SIZE             (0x500)


extern VLYNQ_DEV vlynqDevice0, vlynqDevice1;
int    vlynq_init_status[2] = {0, 0};
static int reset_hack = 1;


void vlynq_ar7_dev_init()
{
    *(unsigned long*) AVALANCHE_GPIO_ENBL    |= (1<<18);
    vlynq_delay(20000);
    *(unsigned long*) AVALANCHE_GPIO_DIR     &= ~(1<<18);
    vlynq_delay(20000);
#if defined (CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW) /* AR7WRD */
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT&= ~(1<<18);
    vlynq_delay(50000);
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT|=  (1<<18);
    vlynq_delay(50000);
#else /* VWI WI WV */
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT|= (1<<18);
    vlynq_delay(50000);
    *(unsigned long*) AVALANCHE_GPIO_DATA_OUT&=~(1<<18);
    vlynq_delay(50000);
#endif

    /* Initialize the MIPS host vlynq driver for a given vlynq interface */
    vlynqDevice0.dev_idx = 0;			/* first vlynq module - this parameter is for reference only */
    vlynqDevice0.module_base = AVALANCHE_LOW_VLYNQ_CONTROL_BASE; 	/*  vlynq0 module base address */

#if defined(CONFIG_AVALANCHE_VLYNQ_CLK_LOCAL)
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_LOCAL;   
    vlynqDevice0.clk_div = 0x02; 			/* board/hardware specific */
#else
    vlynqDevice0.clk_source = VLYNQ_CLK_SOURCE_REMOTE;   
    vlynqDevice0.clk_div = 0x01; 			/* board/hardware specific */
#endif
    vlynqDevice0.state =  VLYNQ_DRV_STATE_UNINIT; 	/* uninitialized module */

    /* Populate vlynqDevice0.local_mem & Vlynq0.remote_mem based on system configuration */ 
    /*Local memory configuration */

                /* Demiurg : not good !*/
#if 0
    vlynqDevice0.local_mem.Txmap= AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE & ~(0xc0000000) ; /* physical address */
    vlynqDevice0.remote_mem.RxOffset[0]= VLYNQ0_REMOTE_WINDOW1_OFFSET; /* This is specific to the board on the other end */
    vlynqDevice0.remote_mem.RxSize[0]=VLYNQ0_REMOTE_WINDOW1_SIZE;
#endif

                /* Demiurg : This is how it should be ! */
                vlynqDevice0.local_mem.Txmap = PHYSADDR(AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE);
#define VLYNQ_ACX111_MEM_OFFSET     0xC0000000  /* Physical address of ACX111 memory */
#define VLYNQ_ACX111_MEM_SIZE       0x00040000  /* Total size of the ACX111 memory   */
#define VLYNQ_ACX111_REG_OFFSET     0xF0000000  /* PHYS_ADDR of ACX111 control registers   */
#define VLYNQ_ACX111_REG_SIZE       0x00022000  /* Size of ACX111 registers area, MAC+PHY  */
#if (DRAM_SIZE == 32) //leijun
  #define ACX111_VL1_REMOTE_SIZE 0x2000000
#elif (DRAM_SIZE == 16)
//#error "is 16M now"
  #define ACX111_VL1_REMOTE_SIZE 0x1000000
#endif

                vlynqDevice0.remote_mem.RxOffset[0]  =  VLYNQ_ACX111_MEM_OFFSET;
                vlynqDevice0.remote_mem.RxSize[0]    =  VLYNQ_ACX111_MEM_SIZE  ;
                vlynqDevice0.remote_mem.RxOffset[1]  =  VLYNQ_ACX111_REG_OFFSET;
                vlynqDevice0.remote_mem.RxSize[1]    =  VLYNQ_ACX111_REG_SIZE  ;
                vlynqDevice0.remote_mem.Txmap        =  0;
                vlynqDevice0.local_mem.RxOffset[0]   =  AVALANCHE_SDRAM_BASE;
                vlynqDevice0.local_mem.RxSize[0]     =  ACX111_VL1_REMOTE_SIZE;


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

     if(reset_hack != 1)
       printk("About to re-init the VLYNQ.\n");

    if(vlynq_init(&vlynqDevice0,VLYNQ_INIT_PERFORM_ALL)== 0)
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


void  vlynq_dev_init(void)
{
    volatile unsigned int *reset_base = (unsigned int *) AVALANCHE_RESET_CONTROL_BASE;

    *reset_base &= ~((1 << AVALANCHE_LOW_VLYNQ_RESET_BIT)); /* | (1 << AVALANCHE_HIGH_VLYNQ_RESET_BIT)); */

    vlynq_delay(20000);

    /* Bring vlynq out of reset if not already done */
    *reset_base |= (1 << AVALANCHE_LOW_VLYNQ_RESET_BIT); /* | (1 << AVALANCHE_HIGH_VLYNQ_RESET_BIT); */
    vlynq_delay(20000); /* Allowing sufficient time to VLYNQ to settle down.*/
    vlynq_ar7_dev_init();
}


extern REMOTE_VLYNQ_DEV_RESET_CTRL_FN p_remote_vlynq_dev_reset_ctrl;

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
                printk("Un-resetting the remote device.\n");
                vlynq_dev_init();
                printk("Re-initialized the VLYNQ.\n");
                reset_hack = 2;
            }
            else if(IN_RESET == reset_ctrl)
            {
#ifdef CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW /* AR7WRD and AR7VW*/
                *(unsigned long*) AVALANCHE_GPIO_DATA_OUT &= ~(1<<18);
#else

                *(unsigned long*) AVALANCHE_GPIO_DATA_OUT |= (1<<18);
#endif

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

extern SET_MDIX_ON_CHIP_FN_T p_set_mdix_on_chip_fn;


#define AUTO_MDIX_ENABLE_PIN        30
#define MDIX_NORMAL_MODE            0

#define AUTO_MDIX_GPIO_PIN          28
#define MDIX_MODE                   1
#define MDI_MODE                    0

#define AUTO_MDIX_CNTL_MASK        ( (1 << AUTO_MDIX_ENABLE_PIN) \
                                    |(1 << AUTO_MDIX_GPIO_PIN))
/* We ignore MDIX_NORMAL_MODE as it is 0 and we get to set 0 from the below
   macros for the AUTO_MDIX_ENABLE_PIN. */
#define AUTO_MDIX_ON_WORD          (MDIX_MODE << AUTO_MDIX_GPIO_PIN)
#define AUTO_MDIX_OFF_WORD         (MDI_MODE << AUTO_MDIX_GPIO_PIN)

int sangam_set_mdix_on_chip(unsigned int base_addr, unsigned int operation)
{
    if((!p_set_mdix_on_chip_fn) || (base_addr != AVALANCHE_LOW_CPMAC_BASE))
        return (-1);

    if(operation)
        /* 0perating on bit 29 and 30. Writing 1 to bit 29 and 0 to bit 30. */
        avalanche_gpio_out_value(AUTO_MDIX_ON_WORD, AUTO_MDIX_CNTL_MASK,0);
    else
        /* 0perating on bit 29 and 30. Writing 0 to bit 29 and 0 to bit 30. */
        avalanche_gpio_out_value(AUTO_MDIX_OFF_WORD, AUTO_MDIX_CNTL_MASK,0);

    return (0);
}


void ar7_delay(int milisec)
{
#define MAX_CPU_FREQUENCY_IN_MHZ        150

    volatile unsigned int tick= milisec * 1000 * MAX_CPU_FREQUENCY_IN_MHZ;

    while(tick--);

}


void avalanche_soc_platform_init(void)
{
    /* GPIO INIT */
    avalanche_gpio_init();
    
    avalanche_reset_ctrl(AVALANCHE_UART0_RESET_BIT, OUT_OF_RESET);
    avalanche_reset_ctrl(AVALANCHE_GPIO_RESET_BIT, OUT_OF_RESET);

//    avalanche_gpio_out_value(0xf3fc3ff0, 0xf3fc3ff0, 0);
    /* hack for now, to be cleaned up. */
    *(unsigned int *)0xa861090c = 0xf3fc3ff0;

//    REG32_WRITE(TNETD73XX_GPIOENR, 0xf3fc3ff0);

    avalanche_reset_ctrl(AVALANCHE_LOW_EPHY_RESET_BIT,IN_RESET);

    /* Also put CPMAC0 and CPMAC1 to reset */
    avalanche_reset_ctrl(AVALANCHE_LOW_CPMAC_RESET_BIT,IN_RESET);
    avalanche_reset_ctrl(AVALANCHE_HIGH_CPMAC_RESET_BIT,IN_RESET);

/*#if (!defined(CONFIG_MIPS_AR7WRD) && !defined(CONFIG_MIPS_AR7VWI) */
#if (!defined(CONFIG_MIPS_AVALANCHE_MARVELL))
    avalanche_reset_ctrl(AVALANCHE_LOW_EPHY_RESET_BIT,OUT_OF_RESET);

#else
    /* Reset the Marvell switch via  GPIO pin 17 */
    avalanche_gpio_ctrl(17,GPIO_PIN,GPIO_OUTPUT_PIN);
    avalanche_gpio_out_bit(17, 0);
    
    /* wait for 20 milisec */
    ar7_delay(20);
    

    avalanche_gpio_out_bit(17, 1);


#endif

                                                                                       
    /* CLK CTRl INIT */                                                                
    avalanche_clkc_init(AFECLK_FREQ, REFCLK_FREQ, OSC3_FREQ);                   


   /* initialize vbus frequency */
    avalanche_set_vbus_freq(avalanche_clkc_get_freq(CLKC_VBUS));

    {        
        volatile unsigned int cvr_reg = *(volatile unsigned int*) AVALANCHE_CVR;

        cvr_reg = (cvr_reg >> 20) & 0xF;

    /* Takes care of the CVR bug in the Sangam Chip as well. */
        if(cvr_reg >= 2)   
    {
        p_set_mdix_on_chip_fn = &sangam_set_mdix_on_chip;

        avalanche_gpio_ctrl(AUTO_MDIX_ENABLE_PIN, GPIO_PIN, GPIO_OUTPUT_PIN);
        avalanche_gpio_ctrl(AUTO_MDIX_GPIO_PIN, GPIO_PIN, GPIO_OUTPUT_PIN);

        prom_printf("This SOC has MDIX cababilities on chip.\n");
    } 
    }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    p_remote_vlynq_dev_reset_ctrl = remote_vlynq_dev_reset_ctrl;
    vlynq_dev_init();
#endif



}


