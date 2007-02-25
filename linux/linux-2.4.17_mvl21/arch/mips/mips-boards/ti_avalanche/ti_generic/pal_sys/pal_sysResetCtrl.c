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

/** \file   pal_sys_reset_ctrl.c
    \brief  PAL reset control code file
	
    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */
#include <pal.h>

REMOTE_VLYNQ_DEV_RESET_CTRL_FN p_remote_vlynq_dev_reset_ctrl = NULL;

/*****************************************************************************
 * Reset Control Module.
 *****************************************************************************/
void PAL_sysResetCtrl(unsigned int module_reset_bit, 
                        PAL_SYS_RESET_CTRL_T reset_ctrl)
{
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;

    if(module_reset_bit >= 32 && module_reset_bit < 64)
	{
        return;
	}

    if(module_reset_bit >= 64)
    {
        if(p_remote_vlynq_dev_reset_ctrl)
		{
            return(p_remote_vlynq_dev_reset_ctrl(module_reset_bit - 64, reset_ctrl));
		}
		else
		{
            return;
		}
    }
    
    if(reset_ctrl == OUT_OF_RESET)
	{
        *reset_reg |= 1 << module_reset_bit;
	}
    else
	{
        *reset_reg &= ~(1 << module_reset_bit);
	}
}

PAL_SYS_RESET_CTRL_T PAL_sysGetResetStatus(unsigned int module_reset_bit)
{
    volatile unsigned int *reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_PRCR;

    return (((*reset_reg) & (1 << module_reset_bit)) ? OUT_OF_RESET : IN_RESET );
}

void PAL_sysSystemReset(PAL_SYS_SYSTEM_RST_MODE_T mode)
{
    volatile unsigned int *sw_reset_reg = (unsigned int*) AVALANCHE_RST_CTRL_SWRCR;
    *sw_reset_reg =  mode;
}

#define AVALANCHE_RST_CTRL_RSR_MASK 0x3

PAL_SYS_SYSTEM_RESET_STATUS_T PAL_sysGetSysLastResetStatus()
{
    volatile unsigned int *sys_reset_status = (unsigned int*) AVALANCHE_RST_CTRL_RSR;

    return ( (PAL_SYS_SYSTEM_RESET_STATUS_T) (*sys_reset_status & AVALANCHE_RST_CTRL_RSR_MASK) );
}

