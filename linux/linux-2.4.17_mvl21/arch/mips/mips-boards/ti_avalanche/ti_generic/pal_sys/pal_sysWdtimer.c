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

/******************************************************************************
 * FILE PURPOSE:    Watchdog Timer Module Source file
 ********************************************************************************
 * FILE NAME:       wdtimer.c
 *
 * DESCRIPTION:     Platform and OS independent Implementation for watchdog timer module
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 * 
 * This watchdog timer module has prescalar and counter which divide the input
 *  reference frequency and upon expiration, the system is reset.
 * 
 *                        ref_freq 
 * Reset freq = ---------------------
 *                  (prescalar * counter)
 * 
 * This watchdog timer supports timer values in mSecs. Thus
 * 
 *           prescalar * counter * 1 KHZ
 * mSecs =   --------------------------
 *                  ref_freq
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#include "pal.h"

#define KHZ                         1000
#define KICK_VALUE                  1
#define KICK_LOCK_STATE             0x3
#define CHANGE_LOCK_STATE           0x3
#define DISABLE_LOCK_STATE          0x3
#define PRESCALE_LOCK_STATE         0x3

#define MAX_PRESCALE_REG_VALUE      0xFFFF  
#define MAX_CHANGE_REG_VALUE        0xFFFF
#define MAX_CLK_DIVISOR_VALUE       0xFFFE0001 /* MAX_PRESCALE_REG_VALUE * MAX_CHANGE_REG_VALUE */

#define MIN_PRESCALE_REG_VALUE      0x0
#define MIN_CHANGE_REG_VALUE        0x1

#define KICK_LOCK_1ST_STAGE         0x5555
#define KICK_LOCK_2ND_STAGE         0xAAAA
#define PRESCALE_LOCK_1ST_STAGE     0x5A5A
#define PRESCALE_LOCK_2ND_STAGE     0xA5A5
#define CHANGE_LOCK_1ST_STAGE       0x6666
#define CHANGE_LOCK_2ND_STAGE       0xBBBB
#define DISABLE_LOCK_1ST_STAGE      0x7777
#define DISABLE_LOCK_2ND_STAGE      0xCCCC
#define DISABLE_LOCK_3RD_STAGE      0xDDDD

/* private functions */
static INT32 write_kick_reg(UINT32);
static INT32 write_change_reg(UINT32);
static INT32 write_prescale_reg(UINT32);
static INT32 write_disable_reg(UINT32);

static UINT32 refclk_freq;
static volatile PAL_SYS_WDTIMER_STRUCT_T *p_wdtimer;

/****************************************************************************
 * PUBLIC FUNCTIONS
 ***************************************************************************/
/****************************************************************************
 * FUNCTION: PAL_sysWdtimerInit
 ****************************************************************************
 * Description: The routine initializes the watch dog timer module 
 ***************************************************************************/ 
void PAL_sysWdtimerInit (UINT32 base_addr, UINT32 vbus_clk_freq)
{
    p_wdtimer = (PAL_SYS_WDTIMER_STRUCT_T *)base_addr;
    refclk_freq = vbus_clk_freq;
}


/****************************************************************************
 * FUNCTION: PAL_sysWdtimerSetPeriod
 ****************************************************************************
 * Description: The routine sets the timeout period of watchdog timer 
 ***************************************************************************/ 
INT32 PAL_sysWdtimerSetPeriod
(
    UINT32 msec          /* period in miliseconds*/
)
{
    UINT32 change_value;
    UINT32 count_value;
    UINT32 prescale_value = 1;
    UINT32 refclk_khz = (refclk_freq / KHZ);
    
    /* The min time period is 1 msec and since the reference clock freq is always going 
       to be more than "min" divider value, minimum value is not checked.
       Check the max time period that can be derived from the timer in milli-secs
    */

    if ( (msec == 0) || (refclk_khz== 0) ||
         (msec > ((MAX_CLK_DIVISOR_VALUE) / refclk_khz)) )
    {
        return (WDTIMER_ERR_INVAL);
    }

    /* compute the prescale value and change register values of the timer 
     * corresponding to the variable  milsec; 
     * divide by 1000 since time period is in millisec
     */
    count_value = refclk_khz * msec; 
    
    if (count_value > MAX_CHANGE_REG_VALUE)
    {
        /* Add 1 to change value so that prescale value computed
         * will be within range 
         */
        change_value = count_value / MAX_PRESCALE_REG_VALUE + 1;
        prescale_value = count_value / change_value;
    }    
    else 
    {
        change_value =  count_value;
    }
                                                               
     /* write to prescale register*/
     if (write_prescale_reg(prescale_value - 1) == WDTIMER_RET_ERR)
         return WDTIMER_RET_ERR;  

    /* write to change register*/
    if (write_change_reg(change_value ) == WDTIMER_RET_ERR)
         return WDTIMER_RET_ERR;  
    
    
    return WDTIMER_RET_OK;
}

/****************************************************************************
 * FUNCTION: PAL_sysWdtimerCtrl
 ****************************************************************************
 * Description: The routine enables/disables the watchdog timer
 ***************************************************************************/ 
INT32 PAL_sysWdtimerCtrl(PAL_SYS_WDTIMER_CTRL_T wd_ctrl)
{
    return write_disable_reg(wd_ctrl);
}

/****************************************************************************
 * FUNCTION: PAL_sysWdtimerKick
 ****************************************************************************
 * Description: The routine kicks(restarts) the watchdog timer
 ***************************************************************************/ 
INT32 PAL_sysWdtimerKick(void)
{
    return write_kick_reg(KICK_VALUE);
} 


/* private functions */

/****************************************************************************
 * FUNCTION: write_kick_reg
 ****************************************************************************
 * Description: The routine writes the specified value into kick register after unlocking it.
 ***************************************************************************/ 
static INT32 write_kick_reg(UINT32 kick_value)
{
    p_wdtimer->kick_lock = KICK_LOCK_1ST_STAGE;      /* Unlocking 1st stage. */

    if( (p_wdtimer->kick_lock & KICK_LOCK_STATE) == 0x1) 
    {
        p_wdtimer->kick_lock = KICK_LOCK_2ND_STAGE; /* Unlocking 2nd stage. */

        if( (p_wdtimer->kick_lock & KICK_LOCK_STATE) == 0x3)
        {
               p_wdtimer->kick  = kick_value;
               return WDTIMER_RET_OK;
        }
        else
        {
            return WDTIMER_RET_ERR; /*2nd stage unlocking failed. */
        }
    }
    else
    {
        return WDTIMER_RET_ERR; /*1st  stage unlocking failed. */
    }
}   


/****************************************************************************
 * FUNCTION: write_prescale_reg
 ****************************************************************************
 * Description: The routine writes the specified value into kick register after unlocking it.
 ***************************************************************************/ 
static INT32 write_prescale_reg(UINT32 prescale_value)
{
    p_wdtimer->prescale_lock = PRESCALE_LOCK_1ST_STAGE;      /* Unlocking 1st stage. */

    if( (p_wdtimer->prescale_lock & PRESCALE_LOCK_STATE) == 0x1) 
    {
        p_wdtimer->prescale_lock = PRESCALE_LOCK_2ND_STAGE; /* Unlocking 2nd stage. */

        if( (p_wdtimer->prescale_lock & PRESCALE_LOCK_STATE) == 0x3)
        {
               p_wdtimer->prescale  = prescale_value;
               return WDTIMER_RET_OK;
        }
        else
        {
            return WDTIMER_RET_ERR; /*2nd stage unlocking failed. */
        }
    }
    else
    {
        return WDTIMER_RET_ERR; /*1st  stage unlocking failed. */
    }
}   

/****************************************************************************
 * FUNCTION: write_change_reg
 ****************************************************************************
 * Description: The routine writes the specified value into kick register after unlocking it.
 ***************************************************************************/ 
static INT32 write_change_reg(UINT32 initial_timer_value)
{
    p_wdtimer->change_lock = CHANGE_LOCK_1ST_STAGE;      /* Unlocking 1st stage. */

    if( (p_wdtimer->change_lock & CHANGE_LOCK_STATE) == 0x1) 
    {
        p_wdtimer->change_lock = CHANGE_LOCK_2ND_STAGE; /* Unlocking 2nd stage. */

        if( (p_wdtimer->change_lock & CHANGE_LOCK_STATE) == 0x3)
        {
               p_wdtimer->change    = initial_timer_value;
               return WDTIMER_RET_OK;
        }   
        else
        {
            return WDTIMER_RET_ERR; /*2nd stage unlocking failed. */
        }
    }
    else
    {
        return WDTIMER_RET_ERR; /*1st stage unlocking failed. */
    }
}   


/****************************************************************************
 * FUNCTION: write_disable_reg
 ****************************************************************************
 * Description: The routine writes the specified value into kick register after unlocking it.
 ***************************************************************************/ 
static INT32 write_disable_reg(UINT32 disable_value)
{

    p_wdtimer->disable_lock = DISABLE_LOCK_1ST_STAGE;        /* Unlocking 1st stage. */

    if( (p_wdtimer->disable_lock & DISABLE_LOCK_STATE) == 0x1) 
    {
        p_wdtimer->disable_lock = DISABLE_LOCK_2ND_STAGE;   /* Unlocking 2nd stage. */

        if( (p_wdtimer->disable_lock & DISABLE_LOCK_STATE) == 0x2)
        {
             p_wdtimer->disable_lock = DISABLE_LOCK_3RD_STAGE;  /* Unlocking 3rd stage. */

             if( (p_wdtimer->disable_lock & DISABLE_LOCK_STATE) == 0x3)
             {
                  p_wdtimer->disable    = disable_value;
                  return WDTIMER_RET_OK;
             }    
             else
             {
                 return WDTIMER_RET_ERR; /*3rd stage unlocking failed. */
             }
        }
        else
        {
            return WDTIMER_RET_ERR; /* 2nd stage unlocking failed */
        }
    }
    else
    {
        return WDTIMER_RET_ERR; /*1st stage unlocking failed. */
    }

}   

