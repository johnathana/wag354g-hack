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
#include <asm/avalanche/generic/pal.h>


void avalanche_soc_platform_init(void)
{

    /* GPIO INIT */
    PAL_sysGpioInit();
    
    PAL_sysResetCtrl(AVALANCHE_UART0_RESET_BIT, OUT_OF_RESET);
    PAL_sysResetCtrl(AVALANCHE_GPIO_RESET_BIT, OUT_OF_RESET);
    PAL_sysResetCtrl(AVALANCHE_MDIO_RESET_BIT, OUT_OF_RESET);

    /* Bring Timer 16 modules out of reset */
    PAL_sysResetCtrl(AVALANCHE_TIMER0_RESET_BIT, OUT_OF_RESET);
    PAL_sysResetCtrl(AVALANCHE_TIMER1_RESET_BIT, OUT_OF_RESET);

    /* Bring McDMA out of reset */
    PAL_sysResetCtrl(AVALANCHE_MCDMA_RESET_BIT, OUT_OF_RESET);    

    PAL_sysResetCtrl(AVALANCHE_LOW_CPMAC_RESET_BIT,IN_RESET);
    PAL_sysResetCtrl(AVALANCHE_HIGH_CPMAC_RESET_BIT,IN_RESET);
    

    /* hack for now, to be cleaned up. */
    *(unsigned int *)0xa861090c = 0xf3fc7ff0;

    /* GPIO PIN 14 */
    PAL_sysGpioCtrl(14, GPIO_PIN, GPIO_OUTPUT_PIN);
    
    /* Set the GPIO pin output connected to power LED to 0 */
    PAL_sysGpioOutBit(14, 0);
    
    /* CLK CTRl INIT */                                                                
    {
        PAL_SYS_Tnetv1020Init clkc;
    
        clkc.xtal1in = XTAL1_FREQ;
        clkc.xtal2in = XTAL2_FREQ;
        clkc.oscin   = OSC_FREQ;

        PAL_sysClkcInit(&clkc);
    }
    

   /* initialize vbus frequency */
    avalanche_set_vbus_freq((PAL_sysClkcGetFreq(CLKC_SYS)/2));


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
//    p_remote_vlynq_dev_reset_ctrl = remote_vlynq_dev_reset_ctrl;
    vlynq_dev_init();
#endif


}

#ifdef CONFIG_AVALANCHE_INTC_PACING
unsigned int avalanche_get_intc_input_freq(void)
{
    return PAL_sysClkcGetFreq(CLKC_SYS);
}
#endif

