/*****************************************************************************
 * Clock Control
 *****************************************************************************/
#ifndef _PUMAS_CLK_CNTL_H_                 
#define _PUMAS_CLK_CNTL_H_                             
#include <asm/avalanche/generic/avalanche_misc.h>

#define CLK_MHZ(x)    ( (x) * 1000000 )

/* The order of ENUMs here should not be altered since
 * the register addresses are derived from the order
 */

typedef enum AVALANCHE_CLKC_ID_tag
{
    CLKC_VBUS,
    CLKC_MIPS,
    CLKC_USB,
    CLKC_SYS
} AVALANCHE_CLKC_ID_T;

void avalanche_clkc_init(unsigned int refclk, unsigned int xtal3in);
int avalanche_clkc_set_freq(AVALANCHE_CLKC_ID_T clk_id, unsigned int output_freq);
unsigned int avalanche_clkc_get_freq(AVALANCHE_CLKC_ID_T clk_id);

#endif /*_PUMAS_CLK_CNTL_H_ */
