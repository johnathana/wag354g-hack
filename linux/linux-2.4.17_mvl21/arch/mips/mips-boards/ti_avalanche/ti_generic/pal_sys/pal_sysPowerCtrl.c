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

/** \file   pal_sys_power_ctrl.c
    \brief  PAL power control code file
	
    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */
#include <pal.h>

/*****************************************************************************
 * Power Control Module
 *****************************************************************************/

#define AVALANCHE_GLOBAL_POWER_DOWN_MASK    0x3FFFFFFF /* bit 31, 30 masked */
#define AVALANCHE_GLOBAL_POWER_DOWN_BIT     30         /* shift to bit 30, 31 */

void PAL_sysPowerCtrl(unsigned int module_power_bit, PAL_SYS_POWER_CTRL_T power_ctrl)
{
    volatile unsigned int *power_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    if (power_ctrl == POWER_CTRL_POWER_DOWN)
	{
        /* power down the module */
        *power_reg |= (1 << module_power_bit);
	}
    else
	{
        /* power on the module */
        *power_reg &= (~(1 << module_power_bit));
	}
}

PAL_SYS_POWER_CTRL_T PAL_sysGetPowerStatus(unsigned int module_power_bit)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    return (((*power_status_reg) & (1 << module_power_bit)) ? POWER_CTRL_POWER_DOWN : POWER_CTRL_POWER_UP);
}

void PAL_sysSetGlobalPowerMode(PAL_SYS_SYSTEM_POWER_MODE_T power_mode)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    *power_status_reg &= AVALANCHE_GLOBAL_POWER_DOWN_MASK;
    *power_status_reg |= ( power_mode << AVALANCHE_GLOBAL_POWER_DOWN_BIT);
}

PAL_SYS_SYSTEM_POWER_MODE_T PAL_sysGetGlobalPowerMode(void)
{
    volatile unsigned int *power_status_reg = (unsigned int*)AVALANCHE_POWER_CTRL_PDCR;

    return((PAL_SYS_SYSTEM_POWER_MODE_T) (((*power_status_reg) & (~AVALANCHE_GLOBAL_POWER_DOWN_MASK)) >> AVALANCHE_GLOBAL_POWER_DOWN_BIT));
}

