/******************************************************************************
 * FILE PURPOSE:    16 bit Timer Module Header
 ********************************************************************************
 * FILE NAME:       timer16.h
 *
 * DESCRIPTION:     Platform and OS independent API for 16 bit timer module
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 * 
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __TIMER16_H__
#define __TIMER16_H__


/****************************************************************************
 * Type:        TIMER16_STRUCT_T
 ****************************************************************************
 * Description: This type defines the hardware configuration of the timer
 *              
 ***************************************************************************/
typedef struct TIMER16_STRUCT_tag
{
    UINT32 ctrl_reg;
    UINT32 load_reg;
    UINT32 count_reg;
    UINT32 intr_reg;
} TIMER16_STRUCT_T;


/****************************************************************************
 * Type:        TIMER16_MODE_T
 ****************************************************************************
 * Description: This type defines different timer modes. 
 *              
 ***************************************************************************/
typedef enum TIMER16_MODE_tag
{
    TIMER16_CNTRL_ONESHOT  = 0,
    TIMER16_CNTRL_AUTOLOAD = 2
} TIMER16_MODE_T;



/****************************************************************************
 * Type:        TIMER16_CTRL_T
 ****************************************************************************
 * Description: This type defines start and stop values for the timer. 
 *              
 ***************************************************************************/
typedef enum TIMER16_CTRL_tag
{
    TIMER16_CTRL_STOP = 0,
    TIMER16_CTRL_START
} TIMER16_CTRL_T ;


void timer16_get_freq_range(UINT32  refclk_freq,
                            UINT32 *p_max_usec,
                            UINT32 *p_min_usec);                                

INT32 timer16_set_params(UINT32 base_address,
                         UINT32 refclk_freq,
                         TIMER16_MODE_T mode,
                         UINT32 usec);

void timer16_ctrl(UINT32 base_address, TIMER16_CTRL_T status);

#endif /* __TIMER16_H__ */
