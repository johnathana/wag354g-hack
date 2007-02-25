#include <asm/avalanche/titan/titan_clk_cntl.h>

/*****************************************************************************
 * Clock Control
 *****************************************************************************/

#define MIN(x,y)               ( ((x) <  (y)) ? (x) : (y) )
#define MAX(x,y)               ( ((x) >  (y)) ? (x) : (y) )
#define ABS(x)                 ( ((signed)(x) > 0) ? (x) : (-(x)) )
#define CEIL(x,y)              ( ((x) + (y) / 2) / (y) )

#define CLKC_NUM               6
#define CLKC_CLKCR(x)          (AVALANCHE_CLOCK_CONTROL_BASE + 0x20 + (0x20 * (x)))
#define CLKC_CLKPLLCR(x)       (AVALANCHE_CLOCK_CONTROL_BASE + 0x30 + (0x20 * (x)))

#define CLKC_PRE_DIVIDER        0x001F0000
#define CLKC_POST_DIVIDER       0x0000001F

#define CLKC_PLL_STATUS         0x1
#define CLKC_PLL_FACTOR         0x0000F000

#define BOOTCR_PLL_BYPASS       (1 << 5)
#define BOOTCR_MIPS_ASYNC_MODE  (1 << 25)
#define BOOTCR_VBUS_MUXSEL      (1 << 27)

#define MIPS_PLL_SELECT         0x01800000
#define VBUS_PLL_SELECT       0x0000C000
#define USB_PLL_SELECT          0x00030000
#define ETH_INPUT_SELECT        0x000C0000
#define DSP_INPUT_SELECT        0x00300000
#define USB_MODE_SELECT         0x00400000
#define VBUS_MUX_SELECT         0x08000000

#define MIPS_AUDCLKI_SELECT     0x00000000
#define MIPS_REFCLKI_SELECT     0x00800000
#define MIPS_ALTIN_SELECT       0x01000000

#define VBUS_AUDCLKI_SELECT   0x00000000
#define VBUS_REFCLKI_SELECT   0x00004000
#define VBUS_ALTIN_SELECT     0x00008000
#define VBUS_MIPSPLL_SELECT   0x0000C000

#define USB_AUDCLKI_SELECT      0x00000000
#define USB_REFCLKI_SELECT      0x00010000
#define USB_ALTIN_SELECT        0x00020000
#define USB_MIPSPLL_SELECT      0x00030000



#define  SYS_MAX                CLK_MHZ(150)
#define  SYS_MIN                CLK_MHZ(1)

#define VBUS_MAX                SYS_MAX/2
#define VBUS_MIN                SYS_MIN

#define  MIPS_SYNC_MAX          SYS_MAX
#define  MIPS_ASYNC_MAX         CLK_MHZ(165)
#define  MIPS_MIN               CLK_MHZ(1)

#define  USB_MAX                CLK_MHZ(100)
#define  USB_MIN                CLK_MHZ(1)


#define  LCD_MAX                CLK_MHZ(100)
#define  LCD_MIN                CLK_MHZ(1)

#define  PLL_MUL_MAXFACTOR      15
#define  MAX_DIV_VALUE          32
#define  MIN_DIV_VALUE          1

#define  MIN_PLL_INP_FREQ       CLK_MHZ(8)
#define  MAX_PLL_INP_FREQ       CLK_MHZ(100)


/* These are hard coded wait values  */
#define  DIVIDER_LOCK_TIME      (10100)
#define  PLL_LOCK_TIME          (10100 * 75)

#define KSEG1_BASE              0xa0000000

#ifndef KSEG_INV_MASK
#define KSEG_INV_MASK               0x1FFFFFFF /* Inverted mask for kseg address */
#endif

#ifndef PHYS_ADDR
#define PHYS_ADDR(addr)             ((addr) & KSEG_INV_MASK)
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(addr)            (PHYS_ADDR(addr)|KSEG1_BASE)
#endif

#ifndef REG8_ADDR
#define REG8_ADDR(addr)             (volatile UINT8 *)(PHYS_TO_K1(addr))
#define REG8_DATA(addr)             (*(volatile UINT8 *)(PHYS_TO_K1(addr)))
#define REG8_WRITE(addr, data)      REG8_DATA(addr) = data;
#define REG8_READ(addr, data)       data = (UINT8) REG8_DATA(addr);
#endif

#ifndef REG16_ADDR
#define REG16_ADDR(addr)            (volatile unsigned short *)(PHYS_TO_K1(addr))
#define REG16_DATA(addr)            (*(volatile unsigned short *)(PHYS_TO_K1(addr)))
#define REG16_WRITE(addr, data)     REG16_DATA(addr) = data;
#define REG16_READ(addr, data)      data = (unsigned short) REG16_DATA(addr);
#endif

#ifndef REG32_ADDR
#define REG32_ADDR(addr)            (volatile unsigned int *)(PHYS_TO_K1(addr))
#define REG32_DATA(addr)            (*(volatile unsigned int *)(PHYS_TO_K1(addr)))
#define REG32_WRITE(addr, data)     REG32_DATA(addr) = data;
#define REG32_READ(addr, data)      data = (unsigned int) REG32_DATA(addr);
#endif


/****************************************************************************
 * DATA PURPOSE:    PRIVATE Variables
 **************************************************************************/
static unsigned int *clk_pll_src[CLKC_NUM];
static unsigned int clk_to_pll[CLKC_NUM];
static unsigned int mips_pll_out;
static unsigned int audclk_inp;
static unsigned int refclk_inp;
static unsigned int alt_inp;
static unsigned int present_min;
static unsigned int present_max;

/* Forward References */
static unsigned int find_gcd(unsigned int min, unsigned int max);
static unsigned int compute_prediv( unsigned int divider, unsigned int min, unsigned int max);
static void get_val(unsigned int base_freq, unsigned int output_freq,unsigned int *multiplier, unsigned int *divider);
static unsigned int get_base_frequency(AVALANCHE_CLKC_ID_T clk_id);
static void find_approx(unsigned int *,unsigned int *,unsigned int);
static int lcd_clk_set_freq(unsigned int output_freq);
static unsigned int lcd_clk_get_freq(void);
static int validate_arg(AVALANCHE_CLKC_ID_T clk_id,int output_freq);
static void usb_clk_check(void);
static int set_pll_div(AVALANCHE_CLKC_ID_T clk_id, unsigned int output_freq);
static unsigned int get_pll_div(AVALANCHE_CLKC_ID_T clk_id);


/****************************************************************************
 * FUNCTION: avalanche_clkc_init
 ****************************************************************************
 * Description: The routine initializes the internal variables depending on
 *              on the sources selected for different clocks.
 ***************************************************************************/
void avalanche_clkc_init(unsigned int audclk, unsigned int refclk,
                         unsigned int altclk)
{

     unsigned int choice;

     audclk_inp = audclk;
     refclk_inp = refclk;
     alt_inp    = altclk;

     /*
      * Source for MIPS Clock PLL :
      * 	-  Audio Input Clock
      * 	-  Ref Input Clock
      * 	-  Alternate Clock
      */
     choice = REG32_DATA(AVALANCHE_DCL_BOOTCR) & MIPS_PLL_SELECT;
     switch(choice)
        {
            case MIPS_AUDCLKI_SELECT:
                     clk_pll_src[CLKC_MIPS] = &audclk_inp;
                 break;

            case MIPS_REFCLKI_SELECT:
                    clk_pll_src[CLKC_MIPS] = &refclk_inp;
                 break;

            case MIPS_ALTIN_SELECT:
                    clk_pll_src[CLKC_MIPS] = &alt_inp;
                 break;

            default :
                    clk_pll_src[CLKC_MIPS] = 0;

        }
     /*
      * Source for System Clock PLL:
      * 	-  Audio Input Clock
      * 	-  Ref Input Clock
      * 	-  Alternate Clock
      * 	-  MIPS PLL Output
      */
     choice = REG32_DATA(AVALANCHE_DCL_BOOTCR) & VBUS_PLL_SELECT;
     switch(choice)
        {
            case VBUS_AUDCLKI_SELECT:
                     clk_pll_src[CLKC_VBUS] = &audclk_inp;
                 break;

            case VBUS_REFCLKI_SELECT:
                     clk_pll_src[CLKC_VBUS] = &refclk_inp;
                 break;

            case VBUS_ALTIN_SELECT:
                     clk_pll_src[CLKC_VBUS] = &alt_inp;
                 break;

            case VBUS_MIPSPLL_SELECT:
                     clk_pll_src[CLKC_VBUS] = &mips_pll_out;
                 break;

            default :
                     clk_pll_src[CLKC_VBUS] = 0;

        }


     /*
      * Input Clock Source for USB clock PLL :
      * 	-  Audio Input Clock
      * 	-  Ref Input Clock
      * 	-  Alternate Clock
      * 	-  MIPS PLL Output
      */

     choice = REG32_DATA(AVALANCHE_DCL_BOOTCR) & USB_PLL_SELECT;
     switch(choice)
        {
            case USB_AUDCLKI_SELECT:
                     clk_pll_src[CLKC_USB] = &audclk_inp;
                 break;

            case USB_REFCLKI_SELECT:
                    clk_pll_src[CLKC_USB] = &refclk_inp;
                 break;

            case USB_ALTIN_SELECT:
                    clk_pll_src[CLKC_USB] = &alt_inp;
                 break;

            case USB_MIPSPLL_SELECT:
                    clk_pll_src[CLKC_USB] = &mips_pll_out;
                 break;

            default :
                    clk_pll_src[CLKC_USB] = 0;

        }

    /* Perform the Clock to PLL mapping  */

    /* Map VBUS clock first based on VBUS Mux Select */
     clk_to_pll[CLKC_VBUS] =   \
         (BOOTCR_VBUS_MUXSEL & REG32_DATA(AVALANCHE_DCL_BOOTCR)) ? CLKC_MIPS: CLKC_VBUS; 

    /* Map MIPS clock  based on  MIPS ASYNC BIT */
     clk_to_pll[CLKC_MIPS] =   \
         (BOOTCR_MIPS_ASYNC_MODE & REG32_DATA(AVALANCHE_DCL_BOOTCR)) ? CLKC_MIPS: CLKC_VBUS; 

    /* USB clock is always mapped to USB PLL and LCD clock to VBUS PLL */
    clk_to_pll[CLKC_USB] = CLKC_USB;
    clk_to_pll[CLKC_LCD] = CLKC_VBUS;

   /* Mapping VBUSP clock to VBUS PLL */
   clk_to_pll[CLKC_VBUSP] = CLKC_VBUS;

}



/****************************************************************************
 * FUNCTION: avalanche_clkc_set_freq
 ****************************************************************************
 * Description: The above routine is called to set the output_frequency of the
 *              selected clock(using clk_id) to the  required value given
 *              by the variable output_freq.
 * 
 * Returns     0           if the required frequency is set exactly
 *             1           if the frequency set is not exact but an approximate value
 *                         of the requested frequency
 *
 *             -1          In case of an error.
 *
 ***************************************************************************/
int avalanche_clkc_set_freq
(
    AVALANCHE_CLKC_ID_T clk_id,
    unsigned int        output_freq
)
{
    int pll_id;
 
    if(CLKC_SYS == clk_id)
        clk_id = CLKC_VBUSP;

    if(clk_id >= CLKC_NUM)
        return -1;


    if( clk_id == CLKC_LCD)
        return lcd_clk_set_freq(output_freq);

    pll_id = clk_to_pll[clk_id];

    if(pll_id == CLKC_VBUS)
        return -1;

    /* To set USB clock to exactly 48 MHz */
    if( (clk_id == CLKC_USB) && (CLK_MHZ(48) == output_freq) )
        usb_clk_check();  

    return set_pll_div(pll_id,output_freq);
}


/****************************************************************************
 * FUNCTION: avalanche_clkc_get_freq
 ****************************************************************************
 * Description: The above routine is called to get the output_frequency of the
 *              selected clock( clk_id)
 * 
 * Returns    Frequency of the requested clock in case of valid argument
 *            A value of 0 is returned otherwise.
 ***************************************************************************/
unsigned int avalanche_clkc_get_freq
(
    AVALANCHE_CLKC_ID_T clk_id
)
{
    int pll_id;
    int output_freq;

    if(clk_id >= CLKC_NUM)
        return 0;

    if(CLKC_SYS == clk_id)
        clk_id = CLKC_VBUSP;

    pll_id = clk_to_pll[clk_id];


    if ( CLKC_LCD == clk_id )
    {
	return lcd_clk_get_freq();
    }


    output_freq = get_pll_div(pll_id);
    
    if(clk_id == CLKC_VBUS)
         output_freq >>= 1;

    return output_freq;

}

static int set_pll_div
(
    AVALANCHE_CLKC_ID_T clk_id,
    unsigned int        output_freq
)
{
    unsigned int base_freq;
    unsigned int multiplier;
    unsigned int divider;
    unsigned int min_prediv;
    unsigned int max_prediv;
    unsigned int prediv;
    unsigned int postdiv;
    int ret =0;
    volatile unsigned int temp;


    if( validate_arg(clk_id,output_freq) == -1)
        return -1;


    /* To set USB clock to exactly 48 MHz */
    if( (clk_id == CLKC_USB) && (CLK_MHZ(48) == output_freq) )
        usb_clk_check();  

    base_freq = get_base_frequency(clk_id);

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
    if( divider == 0 )
    {
        return -1;
    }

    /*compute minimum and maximum predivider values */
    min_prediv = MAX(base_freq / MAX_PLL_INP_FREQ + 1, divider / MAX_DIV_VALUE + 1);
    max_prediv = MIN(base_freq / MIN_PLL_INP_FREQ, MAX_DIV_VALUE);

    /*adjust  the value of divider so that it not less than minimum predivider value*/
    if (divider < min_prediv)
    {
        temp = CEIL(min_prediv, divider);
        if ((temp * multiplier) > PLL_MUL_MAXFACTOR)
        {
            return -1  ;
        }
        else
        {
            multiplier = temp * multiplier;
            divider = min_prediv;
        }

    }

    /* compute predivider  and postdivider values */
    prediv = compute_prediv (divider, min_prediv, max_prediv);
    postdiv = CEIL(divider,prediv);

    /*return fail if postdivider value falls out of range */
    if(postdiv > MAX_DIV_VALUE)
    {
        return -1;
    }

 
    if( (base_freq * multiplier) != (output_freq * prediv * postdiv))
        ret = 1; /* Approximate frequency value  is set */

    /*write predivider and postdivider values*/
    /* pre-Divider and post-divider are 5 bit N+1 dividers */
    REG32_WRITE(CLKC_CLKCR(clk_id), ((prediv -1) & 0x1F) << 16 | ((postdiv -1) & 0x1F) );

    /*wait for divider output to stabilise*/
    for(temp =0; temp < DIVIDER_LOCK_TIME; temp++);

    /*write to PLL clock register*/

    if(clk_id == CLKC_VBUS)
    {
        /* but before writing put DRAM to hold mode */
        REG32_DATA(AVALANCHE_EMIF_SDRAM_CFG) |= 0x80000000;
    }
    /*Bring PLL into div mode */
    REG32_WRITE(CLKC_CLKPLLCR(clk_id), 0x4);

    /*compute the word to be written to PLLCR
     *corresponding to multiplier value
     */
    multiplier = (((multiplier - 1) & 0xf) << 12)| ((255 <<3) | 0x0e);

    /* wait till PLL enters div mode */
    while(REG32_DATA(CLKC_CLKPLLCR(clk_id)) & CLKC_PLL_STATUS)
        /*nothing*/;

    REG32_WRITE(CLKC_CLKPLLCR(clk_id), multiplier);

    while(!REG32_DATA(CLKC_CLKPLLCR(clk_id)) & CLKC_PLL_STATUS)
        /*nothing*/;


    /*wait for External pll to lock*/
    for(temp =0; temp < PLL_LOCK_TIME; temp++);

    if(clk_id == CLKC_VBUS)
    {
        /* Bring DRAM out of hold */
        REG32_DATA(AVALANCHE_EMIF_SDRAM_CFG) &= ~0x80000000;
    }

    return ret;
}

static unsigned int get_pll_div
(
    AVALANCHE_CLKC_ID_T clk_id
)
{

    unsigned int  clk_ctrl_register;
    unsigned int  clk_pll_setting;
    unsigned int  clk_predivider;
    unsigned int  clk_postdivider;
    unsigned short  pll_factor;
    unsigned int  base_freq;
    unsigned int  divider;



    base_freq = get_base_frequency(clk_id);

    clk_ctrl_register = REG32_DATA(CLKC_CLKCR(clk_id));

    /* pre-Divider and post-divider are 5 bit N+1 dividers */
    clk_postdivider = (CLKC_POST_DIVIDER & clk_ctrl_register) + 1;
    clk_predivider = ((CLKC_PRE_DIVIDER & clk_ctrl_register) >> 16) + 1;

    divider =  clk_predivider * clk_postdivider;

    if( (REG32_DATA(AVALANCHE_DCL_BOOTCR) & BOOTCR_PLL_BYPASS))
    {
        return (CEIL(base_freq, divider));  /* PLLs bypassed.*/
    }


    else
    {
        /*  return the current clock speed based upon the PLL setting */
        clk_pll_setting = REG32_DATA(CLKC_CLKPLLCR(clk_id));

        /* Get the PLL multiplication factor */
        pll_factor = ((clk_pll_setting & CLKC_PLL_FACTOR) >> 12) + 1;

        /* Check if we're in divide mode or multiply mode */
        if((clk_pll_setting & 0x1)   == 0)
        {
            /* We're in divide mode */
            if(pll_factor <  0x10)
                return (CEIL(base_freq >> 1, divider));
            else
                return (CEIL(base_freq >> 2, divider));
        }

        else     /* We're in PLL mode */
        {
            /* See if PLLNDIV & PLLDIV are set */
            if((clk_pll_setting & 0x0800) && (clk_pll_setting & 0x2))
            {
                if( pll_factor & 0x1 )
                {
                    /* clk = base_freq * k/2  */
                    return(CEIL((base_freq * pll_factor) >> 1, divider));
                }
                else
                {
                    /* clk = base_freq * (k-1) / 4)*/
                    return(CEIL((base_freq * (pll_factor - 1)) >>2, divider));
                }
            }
            else
            {
                if(pll_factor < 0x10)
                {
                   /* clk = base_freq * k */
                   return(CEIL(base_freq * pll_factor, divider));
                }

                else
                {
                   /* clk = base_freq  */
                   return(CEIL(base_freq, divider));
                }
            }
        }
    return(0); /* Should never reach here */

    }

}


/* local helper functions */

 /****************************************************************************
 * FUNCTION: get_base_frequency
 ****************************************************************************
 * Description: The above routine is called to get base frequency of the clocks.
 ***************************************************************************/

static unsigned int get_base_frequency(AVALANCHE_CLKC_ID_T clk_id)
{
    /* update the current MIPs PLL output value, if the required
     * source is MIPS PLL
     */
     if ( clk_pll_src[clk_id] == &mips_pll_out)
     {
         *clk_pll_src[clk_id] = get_pll_div(CLKC_MIPS);
     }

      return (*clk_pll_src[clk_id]);

}

/****************************************************************************
 * FUNCTION: find_gcd
 ****************************************************************************
 * Description: The above routine is called to find gcd of 2 numbers.
 ***************************************************************************/
static unsigned int find_gcd
(
unsigned int min,
unsigned int max
)
{
    if (max % min == 0)
    {
        return min;
    }
    else
    {
        return find_gcd(max % min, min);
    }
}

/****************************************************************************
 * FUNCTION: compute_prediv
 ****************************************************************************
 * Description: The above routine is called to compute predivider value
 ***************************************************************************/
static unsigned int compute_prediv(unsigned int divider, unsigned int min,
                                   unsigned int max)
{
unsigned short prediv;

/* return the divider itself it it falls within the range of predivider*/
if (min <= divider && divider <= max)
{
    return divider;
}

/* find a value for prediv such that it is a factor of divider */
for (prediv = max; prediv >= min ; prediv--)
{
    if ( (divider % prediv) == 0 )
    {
        return prediv;
    }
}

/* No such factor exists,  return min as prediv */
return min;
}

/****************************************************************************
 * FUNCTION: get_val
 ****************************************************************************
 * Description: This routine is called to get values of divider and multiplier.
 ***************************************************************************/

static void get_val(unsigned int output_freq, unsigned int base_freq,
                    unsigned int *multiplier, unsigned int *divider)
{
    unsigned int temp_mul;
    unsigned int temp_div;
    unsigned int gcd;
    unsigned int min_freq;
    unsigned int max_freq;

    /* find gcd of base_freq, output_freq */
    min_freq = (base_freq < output_freq) ? base_freq : output_freq;
    max_freq = (base_freq > output_freq) ? base_freq : output_freq;
    gcd = find_gcd(min_freq , max_freq);

    if(gcd == 0)
        return;  /* ERROR */

    /* compute values of multiplier and divider */
    temp_mul = output_freq / gcd;
    temp_div = base_freq / gcd;


    /* set multiplier such that 1 <= multiplier <= PLL_MUL_MAXFACTOR */
    if( temp_mul > PLL_MUL_MAXFACTOR )
    {
        if((temp_mul / temp_div) > PLL_MUL_MAXFACTOR)
           return;

        find_approx(&temp_mul,&temp_div,base_freq);
    }

    *multiplier = temp_mul;
    *divider    = temp_div;
}

/****************************************************************************
 * FUNCTION: find_approx
 ****************************************************************************
 * Description: This function gets the approx value of num/denom.
 ***************************************************************************/

static void find_approx(unsigned int *num, unsigned int *denom, unsigned int base_freq)
{
    unsigned int num1;
    unsigned int denom1;
    unsigned int num2;
    unsigned int denom2;
    int closest;
    int prev_closest;
    unsigned int temp_num;
    unsigned int temp_denom;
    unsigned int normalize;
    unsigned int gcd;
    unsigned int output_freq;

    num1 = *num;
    denom1 = *denom;

    prev_closest = 0x7fffffff; /* maximum possible value */
    num2 = num1;
    denom2 = denom1;

    /* start with  max */
    for(temp_num = 15; temp_num >=1; temp_num--)
    {

        temp_denom = CEIL(temp_num * denom1, num1);
        output_freq = (temp_num * base_freq) / temp_denom;

        if(temp_denom < 1)
        {
            break;
        }
        else
        {
            normalize = CEIL(num1,temp_num);
            closest = (ABS((num1 * (temp_denom) ) - (temp_num * denom1)))  * normalize;
            if(closest < prev_closest && output_freq > present_min && output_freq <present_max)
            {
                prev_closest = closest;
                num2 = temp_num;
                denom2 = temp_denom;
            }

        }

    }

    gcd = find_gcd(num2,denom2);
    num2 = num2 / gcd;
    denom2 = denom2 /gcd;

    *num      = num2;
    *denom    = denom2;
}

static int lcd_clk_set_freq(unsigned int output_freq)
{
	unsigned int lcd_div=1;
	unsigned int base_freq;
	unsigned int vbus_pll_mul;
	unsigned int lcd_input;
	unsigned int vbus_pll_reg;
        int ret = 0;

	base_freq = get_base_frequency(CLKC_VBUS);
	vbus_pll_reg =  REG32_DATA(CLKC_CLKPLLCR(CLKC_VBUS));
	vbus_pll_mul = ((vbus_pll_reg & CLKC_PLL_FACTOR) >> 12)  + 1;
	lcd_input = base_freq * vbus_pll_mul;


	while (( (lcd_div + 1)  * output_freq) < lcd_input )
		lcd_div++;

	if(lcd_div >= 32)
		ret= -1;

	else
		REG32_WRITE(CLKC_CLKCR(CLKC_LCD),(lcd_div - 1) );

	/* Check for approximation */
	if((output_freq * lcd_div) != (lcd_input))
		ret = 1;
       

	return ret;
}

static unsigned int  lcd_clk_get_freq(void)
{
	unsigned int lcd_div;
	unsigned int base_freq;
	unsigned int vbus_pll_mul;
	unsigned int vbus_pll;
	unsigned int lcd_input;

	base_freq = get_base_frequency(CLKC_VBUS);
	vbus_pll = REG32_DATA(CLKC_CLKPLLCR(CLKC_VBUS));
	vbus_pll_mul = ((vbus_pll & CLKC_PLL_FACTOR) >> 12)  + 1;
	lcd_input = base_freq * vbus_pll_mul;
	lcd_div = (REG32_DATA(CLKC_CLKCR(CLKC_LCD)) & 0x1f) + 1;
	return (lcd_input/lcd_div);
}

static int validate_arg(AVALANCHE_CLKC_ID_T clk_id,int output_freq)
{
    unsigned int bootcr;

    bootcr = REG32_DATA(AVALANCHE_DCL_BOOTCR); 

    /* check if PLLs are bypassed*/
    if(bootcr & BOOTCR_PLL_BYPASS)
    {
        return -1;
    }
         
    /* check if the requested output_frequency is in valid range */
    switch( clk_id )
    {
        case CLKC_VBUS:
            if((output_freq < VBUS_MIN) ||
               (output_freq > (bootcr & BOOTCR_VBUS_MUXSEL) ? (MIPS_ASYNC_MAX/2): (MIPS_SYNC_MAX/2)))
            {
                return -1;
            }
            present_min = VBUS_MIN;
            present_max = ( bootcr & BOOTCR_VBUS_MUXSEL) ? (MIPS_ASYNC_MAX/2):(MIPS_SYNC_MAX/2);
        break;

        case CLKC_MIPS:
            if((output_freq < MIPS_MIN) ||
               (output_freq > (bootcr & BOOTCR_MIPS_ASYNC_MODE) ? MIPS_ASYNC_MAX: MIPS_SYNC_MAX))
            {
                return -1;
            }
            present_min = MIPS_MIN;
            present_max = (bootcr & BOOTCR_MIPS_ASYNC_MODE) ? MIPS_ASYNC_MAX: MIPS_SYNC_MAX;
        break;

        case CLKC_USB:
            if( (output_freq < USB_MIN) || (output_freq > USB_MAX))
            {
                return -1;
            }
            present_min = USB_MIN;
            present_max = USB_MAX;
        break;

	case CLKC_LCD:
            if( (output_freq < LCD_MIN) || (output_freq > LCD_MAX))
            {
                return -1;
            }
        break;
       
        case CLKC_VBUSP:
            if( (output_freq < SYS_MIN) || (output_freq > SYS_MAX))
            {
                return -1;
            }
            present_min = SYS_MIN;
            present_max = SYS_MAX;
        break;
 
        default:
            return -1;

    }
    return 0;
}

static void usb_clk_check(void)
{
    unsigned int bootcr;

    bootcr = REG32_DATA(AVALANCHE_DCL_BOOTCR); 

    {

            if(clk_pll_src[CLKC_USB] == &mips_pll_out)
            {
                /* Now we require MIPS PLL also to be used  
                 * check if both MIPS and VBUS are not using
                 * MIPS PLL output. If so go ahead and set MIPS PLL output
                 * to multiple of 6 MHz
                 */
                 if( !(bootcr & BOOTCR_VBUS_MUXSEL) && !(bootcr & BOOTCR_MIPS_ASYNC_MODE) )
                 {
                     /* Can modify MIPS PLL contents  
                      * set MIPS PLL output to 150 MHz if 
                      * input crystal frequency is 25 MHz. Others
                      * We dont care. 
                      */
                     if(get_base_frequency(CLKC_MIPS) == CLK_MHZ(25)) 
                         set_pll_div(CLKC_MIPS,CLK_MHZ(150));
                 }
            }
    }
}


EXPORT_SYMBOL(avalanche_clkc_init);
EXPORT_SYMBOL(avalanche_clkc_set_freq);
EXPORT_SYMBOL(avalanche_clkc_get_freq);

