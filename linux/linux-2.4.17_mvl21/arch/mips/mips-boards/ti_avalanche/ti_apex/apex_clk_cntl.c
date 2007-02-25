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

#include "pal.h"

/*****************************************************************************
 * Clock Control
 *****************************************************************************/

#define MIN(x,y)           ( ((x) <  (y)) ? (x) : (y) )
#define MAX(x,y)           ( ((x) >  (y)) ? (x) : (y) )
#define ABS(x)             ( ((signed)(x) > 0) ? (x) : (-(x)) )
#define CEIL(x,y)          ( ((x) + (y) / 2) / (y) )

#define CLKC_NUM            2
#define CLKC_PRE_DIVIDER   0x0000001F
#define CLKC_POST_DIVIDER  0x0000001F

#define CLKC_PLL_STATUS    0x1
#define CLKC_PLL_FACTOR    0x0000001F

#define BOOTCR_PLL_BYPASS       (1 << 5)
#define BOOTCR_MIPS_2TO1_MODE  	(1 << 24)
#define BOOTCR_MIPS_ASYNC_MODE  (1 << 25)

#define MIPS_PLL_SELECT         0x00030000
#define SYSTEM_PLL_SELECT       0x0000C000
#define EPHY_CLK_SELECT         0x00300000

#define MIPS_XTAL1_IN_SELECT    0x00000000
#define MIPS_XTAL2_IN_SELECT    0x00010000
#define MIPS_OSC_IN_SELECT      0x00020000

#define SYSTEM_XTAL1_IN_SELECT  0x00000000
#define SYSTEM_XTAL2_IN_SELECT  0x00004000
#define SYSTEM_OSC_IN_SELECT    0x00008000

#define EPHY_SYSPLL_SELECT      0x00000000
#define EPHY_XTAL1_IN_SELECT    0x00100000
#define EPHY_XTAL2_IN_SELECT    0x00200000
#define EPHY_OSC_IN_SELECT      0x00300000

#define SYS_MAX                 CLK_MHZ(150)
#define SYS_MIN                 CLK_MHZ(25)

#define MIPS_SYNC_MAX           SYS_MAX
#define MIPS_2TO1_MAX           CLK_MHZ(300)
#define MIPS_ASYNC_MAX          MIPS_2TO1_MAX
#define MIPS_MIN                CLK_MHZ(25)

#define EPHY_MAX                CLK_MHZ(25)
#define EPHY_MIN                CLK_MHZ(25)

#define PLL_MUL_MAXFACTOR       32
#define MAX_DIV_VALUE           32

#define  MIN_PLL_INP_FREQ       CLK_MHZ(12)
#define  MAX_PLL_INP_FREQ       CLK_MHZ(100)

#define  DIVIDER_LOCK_TIME      10100
#define  PLL_LOCK_TIME          1500


/*****************************************************************
 * Type:        CLKC_STRUCT_T
 *****************************************************************
 * Description: This type defines the hardware overlay of clock PLL
 * and control registers with respect to each clock.
 *
 ******************************************************************/

typedef struct  CLKC_STRUCT_tag
{
    UINT32 PLLCSR;    
    UINT32 PAD1[1];
    UINT32 SECCTL;        
    UINT32 PAD2[1];
    UINT32 PLLM;     
    UINT32 PREDIV;  
    UINT32 POSTDIV;  
    UINT32 EPHY_POSTDIV;  
    UINT32 PAD3[4];
    UINT32 WAKEUP; 
    UINT32 PAD4[1];
    UINT32 PLLCMD;
    UINT32 PLLSTAT;
    UINT32 ALNCTL;    
	UINT32 PAD5[1];
    UINT32 CKEN;    
    UINT32 CKSTAT;
    UINT32 SYSTAT;
	UINT32 PAD6[11];
} CLKC_STRUCT_T;

/*****************************************************************
 * Type:        PAL_SYS_CLKC_STRUCT_T
 *****************************************************************
 * Description: This type defines the hardware overlay of clock 
 * control module.
 *
 ******************************************************************/

typedef struct PAL_SYS_CLKC_STRUCT_tag
{
    CLKC_STRUCT_T clk[CLKC_NUM];
} PAL_SYS_CLKC_STRUCT_T;


/****************************************************************************
 * DATA PURPOSE:    PRIVATE Variables
 **************************************************************************/
static volatile PAL_SYS_CLKC_STRUCT_T *pclk_regs;
static volatile int  *bootcr_reg;
static UINT32 *clk_src[4];
static UINT32 sys_pll_out;
static UINT32 xtal1_inp;
static UINT32 xtal2_inp;
static UINT32 osc_inp;

/* Forward References */
static UINT32 get_base_frequency(PAL_SYS_CLKC_ID_T clk_id);
static void clkc_delay(UINT32 counter);
static void get_val(UINT32 output_freq, UINT32 base_freq,UINT32 *multiplier, UINT32 *divider);

/****************************************************************************
 * FUNCTION: PAL_sysClkcInit
 ****************************************************************************
 * Description: The routine initializes the internal variables depending on
 *              on the sources selected for different clocks.
 ***************************************************************************/
void PAL_sysClkcInit(void* param)
{
     UINT32 choice;

     PAL_SYS_Tnetv1020Init* ptr = (PAL_SYS_Tnetv1020Init *)param;
	 
     xtal1_inp = ptr->xtal1in;
     xtal2_inp = ptr->xtal2in;
     osc_inp   = ptr->oscin;

     /* Initialize clk ctrl reg array */
     pclk_regs = ( volatile PAL_SYS_CLKC_STRUCT_T *)(AVALANCHE_CLOCK_CONTROL_BASE + 0x80); 
     bootcr_reg = (volatile int*)AVALANCHE_DCL_BOOTCR;
	 
     choice = (*bootcr_reg) & MIPS_PLL_SELECT;    
	 
     switch(choice)
        {
            case MIPS_XTAL1_IN_SELECT:
                     clk_src[CLKC_MIPS] = &xtal1_inp;
                 break;

            case MIPS_XTAL2_IN_SELECT:
                    clk_src[CLKC_MIPS] = &xtal2_inp;
                 break;

            case MIPS_OSC_IN_SELECT:
                    clk_src[CLKC_MIPS] = &osc_inp;
                 break;

            default:
                    clk_src[CLKC_MIPS] = 0;

        }

     choice = (*bootcr_reg) & SYSTEM_PLL_SELECT;  
     switch(choice)
        {
            case SYSTEM_XTAL1_IN_SELECT:
                     clk_src[CLKC_SYS] = &xtal1_inp;
                 break;

            case SYSTEM_XTAL2_IN_SELECT:
                     clk_src[CLKC_SYS] = &xtal2_inp;
                 break;

            case SYSTEM_OSC_IN_SELECT:
                     clk_src[CLKC_SYS] = &osc_inp;
                 break;

            default:
                     clk_src[CLKC_SYS] = 0;

        }

     choice = (*bootcr_reg) & EPHY_CLK_SELECT; 	 
     switch(choice)
        {                 
            case EPHY_SYSPLL_SELECT:
                     clk_src[CLKC_EPHY] = &sys_pll_out ;
                 break;
                
            case EPHY_XTAL1_IN_SELECT:
                     clk_src[CLKC_EPHY] = &xtal1_inp ;
                 break;

            case EPHY_XTAL2_IN_SELECT:
                    clk_src[CLKC_EPHY] = &xtal2_inp;
                 break;

            case EPHY_OSC_IN_SELECT:
                    clk_src[CLKC_EPHY] = &osc_inp;
                 break;

            default :
                    clk_src[CLKC_EPHY] = 0/*-1*/;
        }
}

/**
 * \brief PAL System Set Clock Frequency
 * \note this function may not be able to set the frequency requested if it
 * finds that the hardware does not allow it and/or the output desired is 
 * unfeasible. In all cases, the function just returns the clock frequency 
 * before reprogramming. It is the responsibility of the caller to call its 
 * 'get' counterpart after the 'set' to determine if the change actually did 
 * take place. 
 * \note This function waits for the PLLs to lock in a tight loop. Think twice
 * before calling this function in an interrupt context (for example)
 */
int PAL_sysClkcSetFreq(PAL_SYS_CLKC_ID_T clk_id, unsigned int output_freq)
{
    UINT32 base_freq;
    UINT32 multiplier;
    UINT32 divider;
    UINT32 mips_max;

	if (clk_id == CLKC_VBUS)
	{
		return -1;
	}
	
    /* check if PLLs are bypassed*/
    if((*bootcr_reg) & BOOTCR_PLL_BYPASS)
    {
        return -1;
    }
	
    /*check if the requested output_frequency is in valid range*/
    switch( clk_id )
    {
        case CLKC_SYS:
            if( (output_freq < SYS_MIN) || (output_freq > SYS_MAX))
            {
                return -1;
            }
        break;

        case CLKC_MIPS:
            if((*bootcr_reg) & BOOTCR_MIPS_ASYNC_MODE) 
	        {
                mips_max = MIPS_ASYNC_MAX; 
            } 
			else 
			{
                if((*bootcr_reg) & BOOTCR_MIPS_2TO1_MODE) 
		        {
                    mips_max = MIPS_2TO1_MAX; 
                } 
				else 
				{
                    mips_max = MIPS_SYNC_MAX;
                }
            }
            if( (output_freq < MIPS_MIN) || (output_freq > mips_max) ) 
			{
                return -1;
            }            
        break;

        case CLKC_EPHY:
            if( (output_freq < SYS_MIN) || (output_freq > SYS_MAX) ) 
			{
                return -1;
            }
        break;

		default:
			return -1;
    }

    base_freq = get_base_frequency(clk_id);

    if (clk_id == CLKC_EPHY)
    {
        /* try to do something only if input is sys pll */
        if(base_freq == sys_pll_out)
	{ /* This means input is sys pll */ 

            UINT32 ephy_input;            
            UINT32 sys_clk_prediv;
            UINT32 divider;                
                
            /* read the output of sys pll */
            ephy_input = get_base_frequency(CLKC_SYS);
			sys_clk_prediv = pclk_regs->clk[CLKC_SYS].PREDIV
								& CLKC_PRE_DIVIDER;
				
            ephy_input = CEIL(ephy_input, sys_clk_prediv);
            ephy_input *= pclk_regs->clk[CLKC_SYS].PLLM & CLKC_PLL_FACTOR; 
        
            divider = CEIL(ephy_input, CLK_MHZ(25));
        
            /* program the divider in the EPHY post divider */
    
            /* set the GOEN for system pll */
			pclk_regs->clk[CLKC_SYS].SECCTL = 0x1;
    
            /* Set the divider (D2) for sys pll*/
			pclk_regs->clk[CLKC_SYS].EPHY_POSTDIV = divider & CLKC_POST_DIVIDER;
			
            /* Enable the divider */
            pclk_regs->clk[CLKC_SYS].EPHY_POSTDIV |= (1 << 15);			

            /* set aln bits to align SYSCLK1 ans SYSCLK2 */
			pclk_regs->clk[CLKC_SYS].ALNCTL = 0x3;
    
            /* set GOSET bit */
			pclk_regs->clk[CLKC_SYS].PLLCMD = 0x1;			
			
            /* wait for GOSTAT to go 0 */
			while (pclk_regs->clk[CLKC_SYS].PLLSTAT & 0x1);
       }
    } 
    else
    {
		if (CLKC_SYS == clk_id) 
		{
			if(!((*bootcr_reg) & BOOTCR_MIPS_ASYNC_MODE))
			{
				if((*bootcr_reg) & BOOTCR_MIPS_2TO1_MODE)
				{
					return PAL_sysClkcSetFreq(CLKC_MIPS, output_freq * 2);
				}
				else
				{
					clk_id = CLKC_MIPS;
				}
			}
		}
        /* check for minimum base frequency value */
        if( base_freq < MIN_PLL_INP_FREQ)
        {
            return -1;
        }

        get_val(output_freq, base_freq, &multiplier, &divider);

        /* check multiplier range  */
        if( (multiplier  > PLL_MUL_MAXFACTOR) || (multiplier <= 0) )
        {
            return -1;
        }

        /* check divider value */
        if( (divider  > MAX_DIV_VALUE) || (divider < 0) )
        {
            return -1;
        }

        /* Disable the PLL */
		pclk_regs->clk[clk_id].PLLCSR &= ~0x1;    
		
        /* Set the (pre) divider */
		pclk_regs->clk[clk_id].PREDIV = divider;
		
        /* Enable the divider */
		pclk_regs->clk[clk_id].PREDIV |= (1 << 15); 
		
        /* Set the frequency PLL */
    	pclk_regs->clk[clk_id].PLLM = multiplier;
		
        /* Wait 1500 clock cycles. Note: This is more than that */
        clkc_delay(PLL_LOCK_TIME);

        /* Wait for PLL to lock: This is only a precaution. The PLL 
         * should have locked after the fixed delay
         */
		while( !(pclk_regs->clk[clk_id].PLLCSR) & (1 << 5)); 
		
        /* TODO: Find out if we need to set the align register if post
         * divider is not set? Seems like its not necessary 
         */
    
        /* Ensure that the GOSTAT bit is ‘0’ to indicate that the PLL
         * output clock alignment is not in progress.
         */ 
        while (pclk_regs->clk[clk_id].PLLSTAT & 0x1);

        /* Enable the PLL */
        pclk_regs->clk[clk_id].PLLCSR |= 0x1;    		
    }
    return 0;
}

/****************************************************************************
 * FUNCTION: PAL_sysClkcGetFreq
 ****************************************************************************
 * Description: The above routine is called to get the output_frequency of the
 *              selected clock( clk_id)
 ***************************************************************************/
int PAL_sysClkcGetFreq(PAL_SYS_CLKC_ID_T clk_id)
{
    UINT32  clk_pll_setting;
    UINT32  clk_predivider;
    UINT32  clk_postdivider;
    UINT16  pll_factor;
    UINT32  base_freq;
    int ret;

	if ((clk_id > CLKC_VBUS) || (clk_id < CLKC_SYS))
	{
		return -1;
	}
	
	if (CLKC_EPHY == clk_id)
	{
		return CLK_MHZ(25);
	}

	if(CLKC_VBUS == clk_id) 
	{
		return PAL_sysClkcGetFreq(CLKC_SYS)/2;
	}

	if (clk_id == CLKC_SYS)
	{
    	if ( !((*bootcr_reg) & BOOTCR_MIPS_ASYNC_MODE)) 
    	{
			if((*bootcr_reg) & BOOTCR_MIPS_2TO1_MODE)	
	    		return (PAL_sysClkcGetFreq(CLKC_MIPS))/2;
			else
				return (PAL_sysClkcGetFreq(CLKC_MIPS));		
    	}
	}

    base_freq = get_base_frequency(clk_id);
	
    /* pre-Divider and post-divider are 5 bit N+1 dividers */
    clk_predivider =  0x1F & (pclk_regs->clk[clk_id].PREDIV + 1);
    clk_postdivider = 0x1F & (pclk_regs->clk[clk_id].POSTDIV + 1);

    if( ((*bootcr_reg) & BOOTCR_PLL_BYPASS)) 
    {
        /* PLLs bypassed.*/
        return (CEIL(base_freq, (clk_predivider * clk_postdivider)));  
    }
    else 
    {
        /*  read the the PLL setting */
        clk_pll_setting = pclk_regs->clk[clk_id].PLLM;

        /* Get the PLL multiplication factor */
        pll_factor = clk_pll_setting & CLKC_PLL_FACTOR;
    }

    ret = base_freq / clk_predivider;
    ret *= pll_factor;
    ret = ret / clk_postdivider;

    return ret;            
}

/* local helper functions */
/****************************************************************************
 * FUNCTION: clkc_delay
 ****************************************************************************
 * Description: The above routine is called to insert delay cycles.
 ***************************************************************************/

static void clkc_delay(Uint32 counter)
{
    int i;
    for (i = 0; i < counter; i++);
}

/****************************************************************************
 * FUNCTION: get_base_frequency
 ****************************************************************************
 * Description: The above routine is called to get base frequency of the clocks.
 ***************************************************************************/

static UINT32 get_base_frequency(PAL_SYS_CLKC_ID_T clk_id)
{
    return (*clk_src[clk_id]);
}

/****************************************************************************
 * FUNCTION: get_val
 ****************************************************************************
 * Description: This routine is called to get values of divider and multiplier.
 ***************************************************************************/

static void get_val(UINT32 output_freq, UINT32 base_freq,UINT32 *multiplier, UINT32 *divider)
{

	*multiplier = output_freq/base_freq;
    *divider = 0;
    
}


