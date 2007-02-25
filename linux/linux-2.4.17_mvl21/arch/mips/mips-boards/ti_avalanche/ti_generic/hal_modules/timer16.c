/******************************************************************************
 * FILE PURPOSE:    16 bit Timer Module Source
 ********************************************************************************
 * FILE NAME:       timer16.c
 *
 * DESCRIPTION:     Platform and OS independent Implementation for 16 bit timer module
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII
 *
 * 
 * This timer module has a prescalar and a counter which divide the input reference
 * frequency and an interrupt is generated at the end of the count value.
 * 
 *                        ref_freq 
 * Interrupt freq = ---------------------
 *                  (prescalar * counter)
 * 
 * This timer supports timer values in uSecs. Thus
 * 
 *           prescalar * counter * 1 MHZ
 * uSecs =   --------------------------
 *                  ref_freq
 * 
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#include "haltypes.h"
#include "timer16.h"

#define TIMER16_CNTRL_PRESCALE_ENABLE       0x8000
#define TIMER16_CNTRL_PRESCALE              0x003C
#define TIMER16_CNTRL_MODE                  0x0002

#define TIMER16_MINPRESCALE                 2
#define TIMER16_MAXPRESCALE                 8192
#define TIMER16_MIN_LOAD_VALUE              1
#define TIMER16_MAX_LOAD_VALUE              0xFFFF

#define MHZ                                 1000000

/* set min clock divisor to a little higher value
 * so that we are not close to the edge.
 * so multiply by factor 2
 */
#define TIMER16_MAX_CLK_DIVISOR (TIMER16_MAX_LOAD_VALUE * TIMER16_MAXPRESCALE)
#define TIMER16_MIN_CLK_DIVISOR (TIMER16_MIN_LOAD_VALUE * TIMER16_MINPRESCALE * 2)

#define ERROR   -1
#define OK       0


/****************************************************************************
 * FUNCTION: timer16_get_freq_range
 ****************************************************************************
 * Description: The routine is called to obtain the values of maximum and
 *              minimum possible timer values  that the timer module can
 *              support.
 ***************************************************************************/

void timer16_get_freq_range
(
    UINT32  refclk_freq,
    UINT32 *p_max_usec,
    UINT32 *p_min_usec
)
{
    UINT32 refclk_mhz = (refclk_freq / MHZ);
    
    /* Min usecs */
    if(p_min_usec)
    {
        if ( refclk_mhz < TIMER16_MIN_CLK_DIVISOR )
        {
            *p_min_usec = TIMER16_MIN_CLK_DIVISOR / refclk_mhz;
        }
        else
        {
            *p_min_usec = 1;
        }
    }        

    if(p_max_usec)
        *p_max_usec = (TIMER16_MAX_CLK_DIVISOR) / refclk_mhz ;
}


/****************************************************************************
 * FUNCTION: timer16_set_params
 ****************************************************************************
 * Description: The routine is called to configure the timer mode and
 *              time period (in micro seconds).
 ***************************************************************************/
INT32 timer16_set_params
(
    UINT32                    base_address,
    UINT32                    refclk_freq,
    TIMER16_MODE_T            mode,
    UINT32                    usec
)
{
    volatile TIMER16_STRUCT_T *p_timer;
    UINT32    prescale;
    UINT32    count;
    UINT32    refclk_mhz = (refclk_freq / MHZ);

    if( (base_address == 0) || (usec == 0) )
        return ERROR;

    if ( (mode != TIMER16_CNTRL_ONESHOT) && (mode != TIMER16_CNTRL_AUTOLOAD) )
        return ERROR;
        
    /* The min time period is 1 usec and since the reference clock freq is always going 
       to be more than "min" divider value, minimum value is not checked.
       Check the max time period that can be derived from the timer in micro-seconds
    */
    if (usec > ( (TIMER16_MAX_CLK_DIVISOR) / refclk_mhz ))
    {
        return ERROR; /* input argument speed, out of range */
    }

    p_timer = (TIMER16_STRUCT_T *)(base_address);
    count = refclk_mhz * usec;

    for(prescale = 0; prescale < 12; prescale++)
    {
        count = count >> 1;
        if( count <= TIMER16_MAX_LOAD_VALUE )
        {
            break;
        }
    }

    /*write the load counter value*/
    p_timer->load_reg = count;

    /* write prescalar and mode to control reg */
    p_timer->ctrl_reg = mode | TIMER16_CNTRL_PRESCALE_ENABLE | (prescale << 2 );

    return OK;
}

/****************************************************************************
 * FUNCTION: timer16_ctrl
 ****************************************************************************
 * Description: The routine is called to start/stop the timer
 *
 ***************************************************************************/
void timer16_ctrl(UINT32 base_address, TIMER16_CTRL_T status)
{
    volatile TIMER16_STRUCT_T *p_timer;

    if( base_address )
    {
        p_timer = (TIMER16_STRUCT_T *)(base_address);

        if( status == TIMER16_CTRL_START )
        {
            p_timer->ctrl_reg |= TIMER16_CTRL_START;
        }
        else
        {
             p_timer->ctrl_reg &=   ~(TIMER16_CTRL_START);
        }
    }

}

