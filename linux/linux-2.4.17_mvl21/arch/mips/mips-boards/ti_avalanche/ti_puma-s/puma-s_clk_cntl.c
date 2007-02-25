/*
 * Clock control
 */
#include <asm/avalanche/avalanche_map.h>

#define CLKC_CLKCR(x)          (AVALANCHE_CLOCK_CONTROL_BASE + 0x20 + (0x20 * (x)))
#define CLKC_CLKPLLCR(x)       (AVALANCHE_CLOCK_CONTROL_BASE + 0x30 + (0x20 * (x)))

#define BOOTCR_PLL_BYPASS      0x00000020
#define PLL_MUL_MAXFACTOR      15

#define PLL_LOCK_TIME          10100 * 75
#define DIVIDER_LOCK_TIME      10100

#define MAX_PRE_DIV_VALUE      32
#define MAX_DIV_VALUE          16


#define DIV_MASK               0xFFFFFFF0
#define PRE_DIV_MASK           0xFFE0FFFF
#define PRE_DIV_BIT_POS        16

#define BOOTCR_SYSPLSEL_XTAL   (1 << 17)
#define BOOTCR_USBPLSEL_XTAL   (1 << 18)

#define USB_OUT_FREQ           CLK_MHZ(48)
#define USB_PLL1               1
#define USB_DIV1               1
#define USB_PLL2               2
#define USB_DIV2               2


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


/* Enum defining the types of BBIF Clock sources available */
typedef enum AVALANCHE_BBIF_CLK_SEL_tag
{

    BBIF_CLK_ONE_HALF_VBUS   = 0,
    BBIF_CLK_VBUS            = 1,
    BBIF_CLK_BBCLK           = 2

} AVALANCHE_BBIF_CLK_SEL_T;


/* Enum defining the types of USB Clock sources available */
typedef enum AVALANCHE_USB_CLK_SEL_tag
{
    USB_CLK_USBCLKI          = 0,
    USB_CLK_REFCLKI          = 1

} AVALANCHE_USB_CLK_SEL_T;

/* Enum defining the types of PCI Clock sources available */
typedef enum AVALANCHE_PCI_CLK_SEL_tag
{
    PCI_CLK_ONE_HALF_VBUS    = 0,
    PCI_CLK_PCLK             = 1

} AVALANCHE_PCI_CLK_SEL_T;

/* Enum defining the types of Codec PCM Clock sources available */
typedef enum AVALANCHE_PCM_CLK_SEL_tag
{
    PCM_CLK_ONE_HALF_VBUS    = 0,
    PCM_CLK_PCLK             = 1

} AVALANCHE_PCM_CLK_SEL_T;


/* Enum defining the types of DSP Subsystem Clock sources available */
typedef enum AVALANCHE_DSP_CLK_SEL_tag
{
    DSP_CLK_DSPCLKI          = 0,
    DSP_CLK_REFCLKI          = 1

} AVALANCHE_DSP_CLK_SEL_T;

typedef enum AVALANCHE_CRYSTAL_ID_tag
{
    REFCLKI = 0,
    USBCLKI
}AVALANCHE_CRYSTAL_ID_T;


static UINT32 crystal_freq[5];

/* forward declaraions */
static INT32 avalanche_set_pll(AVALANCHE_CLKC_ID_T clk_id, UINT32 mult);
static UINT32 avalanche_get_usb_clk(void);
static int avalanche_set_usb_freq(void);
static UINT32 avalanche_get_sys_freq(void);

/*******************************************************************************
 * FUNCTION:    avalanche_clkc_init
 *******************************************************************************
 * DESCRIPTION: Function initialize the clock control module.
 *
 * RETURNS:     N/A
 *
 ******************************************************************************/
void avalanche_clkc_init
(
    UINT32 ref_clk,
    UINT32 usb_clk
)
{
    crystal_freq[REFCLKI] = ref_clk;
    crystal_freq[USBCLKI] = usb_clk;
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
	  if(clk_id != CLKC_USB || output_freq != CLK_MHZ(48))
	  {
		  return -1;
	  }

	  return avalanche_set_usb_freq();
}

/****************************************************************************
 * FUNCTION: avalanche_clkc_get_freq
 ****************************************************************************
 * Description: The above routine is called to get the output_frequency of the
 *              selected clock( clk_id)
 ***************************************************************************/
unsigned int avalanche_clkc_get_freq
(
    AVALANCHE_CLKC_ID_T clk_id
)
{
	switch(clk_id)
	{
		case CLKC_USB:
			return 	avalanche_get_usb_clk();

		case CLKC_VBUS:
			return avalanche_get_sys_freq() / 2;
		break;

		case CLKC_MIPS:
		case CLKC_SYS:
			return avalanche_get_sys_freq();

		default:
			return -1;
	}
}


/*******************************************************************************
 * FUNCTION:    avalanche_set_pll
 *******************************************************************************
 * DESCRIPTION: Function to set the pll of the clock specified.
 *
 ******************************************************************************/
static int avalanche_set_pll
(
    AVALANCHE_CLKC_ID_T clk_id,
    UINT32 mult
)
{
    volatile UINT32*    ptr;
    volatile UINT32*    ptr32;
    volatile UINT32     value32;
    volatile UINT32 temp;

    /* Check the parameter for validity */
    if( mult > PLL_MUL_MAXFACTOR)
    {
        return -1;
    }

    if(clk_id == CLKC_VBUS)
    {
        return (-1);
    }

    /* See if PLL is in bypass mode. In this case, we can't set it up. */
    ptr32   = (volatile UINT32*)PHYS_TO_K1(AVALANCHE_DCL_BOOTCR);
    value32 = *ptr32;

    if( value32 & BOOTCR_PLL_BYPASS )
    {
        /* Our hardware is in BYPASS mode, we can't do much */
        return(-1);
    }

    /*
     * The actual value that we have to write out is (mult-1). That is,
     * to set mutliplication factor to 5, we have to write out 4 to the PLL
     * register.
     */

    mult    = mult - 1;

    mult    = mult & 0xf;
    mult    = mult << 12;
    mult    = mult | ((255<<3)|0x0e);


    /* Bring PLL to DIV mode */
    ptr = (volatile UINT32*)(PHYS_TO_K1(CLKC_CLKPLLCR(clk_id)));
    *ptr    = 0x4;

    /* Wait to enter DIV mode */
    while( ((*ptr) & 0x1) != 0 )
    {
        /* do nothing */
    }


    /* Put requested PLL value in */
    *((volatile UINT32*)ptr)    = mult;

    /* Wait to exit DIV mode */
    while( ((*ptr) & 0x1) == 0 )
    {
        /* do nothing */
    }

    /*wait for External pll to lock*/
    for(temp =0; temp < PLL_LOCK_TIME; temp++);

    return 0;
}



/*******************************************************************************
 * FUNCTION:    avalanche_set_post_div
 *******************************************************************************
 * DESCRIPTION: Function to set the post div of the clock specified.
 *
 * RETURNS:     AVALANCHE_ERR_OK on success, TNETD53XX_ERR_ERROR on failure
 *
 ******************************************************************************/
static int avalanche_set_post_div
(
    AVALANCHE_CLKC_ID_T clk_id,
    UINT32 div
)
{
    volatile UINT32*    ptr;
    volatile UINT32 temp;

    /* Check the parameter for validity */
    if( div > MAX_DIV_VALUE)
    {
        return(-1);
    }

    if(clk_id == CLKC_VBUS)
    {
        return (-1);
    }

    /*
     * The actual value that we have to write out is (div-1). That is,
     * to set div factor to 5, we have to write out 4 to the div register.
     */

    div = div - 1;

    ptr = (volatile UINT32*)(PHYS_TO_K1(CLKC_CLKCR(clk_id)));

    *ptr    &= DIV_MASK;
    *ptr |= div;

    /*wait for divider output to stabilise*/
    for(temp =0; temp < DIVIDER_LOCK_TIME; temp++);

    return 0;
}

/*******************************************************************************
 * FUNCTION:    avalanche_set_pre_div
 *******************************************************************************
 * DESCRIPTION: Function to set the pre div of the clock specified.
 *
 * RETURNS:     AVALANCHE_ERR_OK on success, TNETD53XX_ERR_ERROR on failure
 *
 ******************************************************************************/
static int avalanche_set_pre_div
(
    AVALANCHE_CLKC_ID_T clk_id,
    UINT32 div
)
{
    volatile UINT32*    ptr;
    volatile UINT32 temp;

    /* Check the parameter for validity */
    if( div > MAX_PRE_DIV_VALUE)
    {
        return(-1);
    }

    if(clk_id == CLKC_VBUS)
    {
        return (-1);
    }

    /*
     * The actual value that we have to write out is (div-1). That is,
     * to set div factor to 5, we have to write out 4 to the div register.
     */

    div = div - 1;

    ptr = (volatile UINT32*)(PHYS_TO_K1(CLKC_CLKCR(clk_id)));

    *ptr    &= PRE_DIV_MASK;
    *ptr |= (div << PRE_DIV_BIT_POS);

    /*wait for divider output to stabilise*/
    for(temp =0; temp < DIVIDER_LOCK_TIME; temp++);

    return 0;
}


/*******************************************************************************
 * FUNCTION:    avalanche_set_usb_freq
 *******************************************************************************
 * DESCRIPTION: Function to set the USB clock.
 *
 * RETURNS:     AVALANCHE_ERR_OK on success, TNETD53XX_ERR_ERROR on failure
 *
 ******************************************************************************/
static int avalanche_set_usb_freq(void)
{
    avalanche_set_pll(USB_PLL1, 8); /* multiply input crystal(25MHz) by 8 */
    avalanche_set_post_div(USB_DIV1, 5); /* set Divider1 to 5 */
    avalanche_set_pre_div(USB_DIV2, 5); /* set Divider2 to  5 */
    avalanche_set_pll(USB_PLL2, 6); /* set Multiplier2 to 6 */
    return 0;
}

/*******************************************************************************
 * FUNCTION:    avalanche_get_sys_freq
 *******************************************************************************
 * DESCRIPTION: Function to get the system frequency
 *
 * RETURNS:     clock frequency on success, zero on error.
 *
 ******************************************************************************/
static UINT32 avalanche_get_sys_freq()
{
    UINT32  mips_pll_setting;
    UINT32  mips_div_setting;
    UINT16  pll_factor;
    UINT32  base_clk;

    if(REG32_DATA(AVALANCHE_DCL_BOOTCR) & BOOTCR_SYSPLSEL_XTAL)
    {
        base_clk = crystal_freq[REFCLKI];
    }
    else
    {
        base_clk = crystal_freq[USBCLKI];
    }


    {
        UINT32 pre_div;

        pre_div = ((REG32_DATA(CLKC_CLKCR(CLKC_VBUS)) & ~(PRE_DIV_MASK))
                                                            >> PRE_DIV_BIT_POS);
        pre_div += 1;

        base_clk = base_clk / pre_div;
    }

    if((REG32_DATA(AVALANCHE_DCL_BOOTCR) & BOOTCR_PLL_BYPASS))
    {
        return (base_clk);  /* PLLs bypassed.*/
    }

    mips_pll_setting = REG32_DATA(CLKC_CLKPLLCR(CLKC_VBUS));

    /* Get the PLL multiplication factor */
    pll_factor = ((mips_pll_setting & 0xF000) >>12) + 1;

    /* Check if we're in divide mode or multiply mode */
    if((mips_pll_setting & 0x1) == 1)
    {
        base_clk = (base_clk*pll_factor);
    }
    else        /* We're in divide mode */
    {
        if((mips_pll_setting & 0x0802) == 0x0802) /* See if PLLNDIV & PLLDIV are set */
        {
            if(pll_factor & 0x1)
            {
                /* clk = base_clk * k /2 */
                base_clk = ((base_clk * pll_factor) >> 1);
            }
            else
            {
                /* clk = base_clk * (k-1) / 4)*/
                base_clk = ((base_clk * (pll_factor - 1)) >>2);
            }
        }
        else
        {
            if(pll_factor < 0x10)
            {
                /* clk = base_clk /2 */
                base_clk = (base_clk >> 1);
            }
            else
            {
                /* clk = base_clk / 4 */
                base_clk = (base_clk >> 2);
            }
        }
    }

    /* Divisor is a 4 bit N+1 divisor */
    mips_div_setting = (0xF & REG32_DATA(CLKC_CLKCR(CLKC_VBUS)))+1;

    base_clk = base_clk / mips_div_setting;

    return(base_clk);
}

/*******************************************************************************
 * FUNCTION:    avalanche_get_usb_clk
 *******************************************************************************
 * DESCRIPTION: Function to get the usb clock frequency
 *
 * RETURNS:     clock frequecy on success, zero on error.
 *
 ******************************************************************************/
static UINT32 avalanche_get_usb_clk()
{
    UINT32 usb_pll_setting;
    UINT32 usb_div_setting;

    UINT32 usb_pll_setting1;
    UINT32 usb_div_setting1;

    UINT16 pll_factor;
    UINT32 base_clk;

    if(REG32_DATA(AVALANCHE_DCL_BOOTCR) & BOOTCR_USBPLSEL_XTAL)
    {
        base_clk = crystal_freq[REFCLKI];
    }
    else
    {
            base_clk = crystal_freq[USBCLKI];
    }


    /* Divisor is a 4 bit N+1 divisor */
    usb_div_setting = (0xF & REG32_DATA(CLKC_CLKCR(CLKC_USB)))+1;

    base_clk = base_clk / usb_div_setting;


    {
        UINT32 pre_div_setting;
        UINT32 pre_div_setting1;

        pre_div_setting = ((REG32_DATA(CLKC_CLKCR(USB_DIV1)) &
                                        ~(PRE_DIV_MASK)) >> PRE_DIV_BIT_POS);
        pre_div_setting += 1;

        base_clk = base_clk / pre_div_setting;

        usb_div_setting1 = (0xF & REG32_DATA(CLKC_CLKCR(USB_DIV2)))+1;

        base_clk = base_clk / usb_div_setting1;

        pre_div_setting1 = ((REG32_DATA(CLKC_CLKCR(USB_DIV2)) &
                                        ~(PRE_DIV_MASK)) >> PRE_DIV_BIT_POS);
        pre_div_setting1 += 1;

        base_clk = base_clk / pre_div_setting1;
    }

    if((REG32_DATA(AVALANCHE_DCL_BOOTCR) & BOOTCR_PLL_BYPASS))
    {
        return (base_clk);  /* PLLs bypassed.*/
    }

    usb_pll_setting = REG32_DATA(CLKC_CLKPLLCR(USB_PLL1));

    /* Get the PLL multiplication factor */
    pll_factor = ((usb_pll_setting & 0xF000) >>12) + 1;

    /* Check if we're in divide mode or multiply mode */
    if((usb_pll_setting & 0x1) == 1)
    {
        base_clk = base_clk*pll_factor;
    }
    else        /* We're in divide mode */
    {
        if((usb_pll_setting & 0x0802) == 0x0802) /* See if PLLNDIV & PLLDIV are set */
        {
            if(pll_factor & 0x1)
            {
                /* clk = base_clk * k /2 */
                base_clk = ((base_clk * pll_factor) >> 1);
            }
            else
            {
                /* clk = base_clk * (k-1) / 4)*/
                base_clk = ((base_clk * (pll_factor - 1)) >>2);
            }
        }
        else
        {
            if(pll_factor < 0x10)
            {
                /* clk = base_clk /2 */
                base_clk = (base_clk >> 1);
            }
            else
            {
                /* clk = base_clk / 4 */
                base_clk = (base_clk >> 2);
            }
        }
    }


    {
        usb_pll_setting1 = REG32_DATA(CLKC_CLKPLLCR(USB_PLL2));

        /* Get the PLL multiplication factor */
        pll_factor = ((usb_pll_setting1 & 0xF000) >>12) + 1;

        /* Check if we're in divide mode or multiply mode */
        if((usb_pll_setting1 & 0x1) == 1)
        {
            base_clk = (base_clk*pll_factor);
        }
        else        /* We're in divide mode */
        {
            if((usb_pll_setting1 & 0x0802) == 0x0802) /* See if PLLNDIV & PLLDIV are set */
            {
                if(pll_factor & 0x1)
                {
                    /* clk = base_clk * k /2 */
                    base_clk = ((base_clk * pll_factor) >> 1);
                }
                else
                {
                    /* clk = base_clk * (k-1) / 4)*/
                    base_clk = ((base_clk * (pll_factor - 1)) >>2);
                }
            }
            else
            {
                if(pll_factor < 0x10)
                {
                    /* clk = base_clk /2 */
                    base_clk = (base_clk >> 1);
                }
                else
                {
                    /* clk = base_clk / 4 */
                    base_clk = (base_clk >> 2);
                }
            }
        }
    }

    return(base_clk);
}

EXPORT_SYMBOL(avalanche_clkc_init);
EXPORT_SYMBOL(avalanche_clkc_set_freq);
EXPORT_SYMBOL(avalanche_clkc_get_freq);

