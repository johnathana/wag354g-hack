#ifndef _TITAN_CLK_CNTL_H_
#define _TITAN_CLK_CNTL_H_

#include <asm/avalanche/generic/avalanche_misc.h>
/*****************************************************************************
 * Clock Control 
 *****************************************************************************/

#define CLK_MHZ(x)    ( (x) * 1000000 )

/* The order of ENUMs here should not be altered since
 * the register addresses are derived from the order
 */

typedef enum AVALANCHE_CLKC_ID_tag
{
    CLKC_VBUS = 0,  /* Clock driving VBUS */
    CLKC_MIPS,      /* Clock to MIPS */
    CLKC_USB,       /* Clock to USB slave */
    CLKC_LCD,       /* Clock to LCD controller */
    CLKC_VBUSP,      /* Clock to VBUSP */
    CLKC_SYS        /* Clock to SYS */
} AVALANCHE_CLKC_ID_T;

void avalanche_clkc_init(unsigned int afeclk, unsigned int refclk, unsigned int xtal3in);
int avalanche_clkc_set_freq(AVALANCHE_CLKC_ID_T clk_id, unsigned int output_freq);
unsigned int avalanche_clkc_get_freq(AVALANCHE_CLKC_ID_T clk_id);

#endif
