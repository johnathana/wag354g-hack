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

/** \file   pal_sys.h
    \brief  PAL SoC level API header file

    This file defines data types and services (macros as well as functions)
    that are applicable to the abstracted h/w system (SoC/Board).

    (C) Copyright 2004, Texas Instruments, Inc

    \author     PSP Architecture Team
    \version    1.0
 */

#ifndef __PAL_SYS_H__
#define __PAL_SYS_H__

/*****************************************************************************
 * Reset Control Module
 *****************************************************************************/

typedef enum PAL_SYS_RESET_CTRL_tag
{
    IN_RESET        = 0,
    OUT_OF_RESET
} PAL_SYS_RESET_CTRL_T;

typedef enum PAL_SYS_SYSTEM_RST_MODE_tag
{
    RESET_SOC_WITH_MEMCTRL      = 1,    /* SW0 bit in SWRCR register */
    RESET_SOC_WITHOUT_MEMCTRL   = 2     /* SW1 bit in SWRCR register */
} PAL_SYS_SYSTEM_RST_MODE_T;

typedef enum PAL_SYS_SYSTEM_RESET_STATUS_tag
{
    HARDWARE_RESET = 0,
    SOFTWARE_RESET0,            /* Caused by writing 1 to SW0 bit in SWRCR register */
    WATCHDOG_RESET,
    SOFTWARE_RESET1             /* Caused by writing 1 to SW1 bit in SWRCR register */
} PAL_SYS_SYSTEM_RESET_STATUS_T;

void PAL_sysResetCtrl(unsigned int reset_module,PAL_SYS_RESET_CTRL_T reset_ctrl);
PAL_SYS_RESET_CTRL_T PAL_sysGetResetStatus(unsigned int reset_module);
void PAL_sysSystemReset(PAL_SYS_SYSTEM_RST_MODE_T mode);
PAL_SYS_SYSTEM_RESET_STATUS_T PAL_sysGetSysLastResetStatus(void);

typedef void (*REMOTE_VLYNQ_DEV_RESET_CTRL_FN)(unsigned int reset_module, 
                                               PAL_SYS_RESET_CTRL_T reset_ctrl);

/*****************************************************************************
 * Power Control Module
 *****************************************************************************/

typedef enum PAL_SYS_POWER_CTRL_tag
{
    POWER_CTRL_POWER_UP = 0,
    POWER_CTRL_POWER_DOWN
} PAL_SYS_POWER_CTRL_T;

typedef enum PAL_SYS_SYSTEM_POWER_MODE_tag
{
    GLOBAL_POWER_MODE_RUN       = 0,    /* All system is up */
    GLOBAL_POWER_MODE_IDLE,             /* MIPS is power down, all peripherals working */
    GLOBAL_POWER_MODE_STANDBY,          /* Chip in power down, but clock to ADSKL subsystem is running */
    GLOBAL_POWER_MODE_POWER_DOWN        /* Total chip is powered down */
} PAL_SYS_SYSTEM_POWER_MODE_T;

void PAL_sysPowerCtrl(unsigned int power_module,  PAL_SYS_POWER_CTRL_T power_ctrl);
PAL_SYS_POWER_CTRL_T PAL_sysGetPowerStatus(unsigned int power_module);
void PAL_sysSetGlobalPowerMode(PAL_SYS_SYSTEM_POWER_MODE_T power_mode);
PAL_SYS_SYSTEM_POWER_MODE_T PAL_sysGetGlobalPowerMode(void);

/*****************************************************************************
 * Wakeup Control
 *****************************************************************************/

typedef enum PAL_SYS_WAKEUP_INTERRUPT_tag
{
    WAKEUP_INT0 = 1,
    WAKEUP_INT1 = 2,
    WAKEUP_INT2 = 4,
    WAKEUP_INT3 = 8
} PAL_SYS_WAKEUP_INTERRUPT_T;

typedef enum PAL_SYS_WAKEUP_CTRL_tag
{
    WAKEUP_DISABLED = 0,
    WAKEUP_ENABLED
} PAL_SYS_WAKEUP_CTRL_T;

typedef enum PAL_SYS_WAKEUP_POLARITY_tag
{
    WAKEUP_ACTIVE_HIGH = 0,
    WAKEUP_ACTIVE_LOW
} PAL_SYS_WAKEUP_POLARITY_T;

void PAL_sysWakeupCtrl(PAL_SYS_WAKEUP_INTERRUPT_T wakeup_int,
                         PAL_SYS_WAKEUP_CTRL_T wakeup_ctrl,
                         PAL_SYS_WAKEUP_POLARITY_T wakeup_polarity);

/*****************************************************************************
 * GPIO Control
 *****************************************************************************/

typedef enum PAL_SYS_GPIO_PIN_MODE_tag
{
    FUNCTIONAL_PIN = 0,
    GPIO_PIN = 1
} PAL_SYS_GPIO_PIN_MODE_T;

typedef enum PAL_SYS_GPIO_PIN_DIRECTION_tag
{
    GPIO_OUTPUT_PIN = 0,
    GPIO_INPUT_PIN = 1
} PAL_SYS_GPIO_PIN_DIRECTION_T;

typedef enum { GPIO_FALSE, GPIO_TRUE } PAL_SYS_GPIO_BOOL_T;

void PAL_sysGpioInit(void);
int PAL_sysGpioCtrl(unsigned int gpio_pin,
                      PAL_SYS_GPIO_PIN_MODE_T pin_mode,
                      PAL_SYS_GPIO_PIN_DIRECTION_T pin_direction);
int PAL_sysGpioOutBit(unsigned int gpio_pin, int value);
int PAL_sysGpioInBit(unsigned int gpio_pin);
int PAL_sysGpioOutValue(unsigned int out_val, unsigned int set_mask, 
                           unsigned int reg_index);
int PAL_sysGpioInValue(unsigned int *in_val, unsigned int reg_index);


/*****************************************************************************
 * CLKC Control
 *****************************************************************************/

void PAL_sysClkcInit(void* param);
int PAL_sysClkcSetFreq(PAL_SYS_CLKC_ID_T clk_id, unsigned int output_freq);
int PAL_sysClkcGetFreq(PAL_SYS_CLKC_ID_T clk_id);

/*****************************************************************************
 * MISC
 *****************************************************************************/

unsigned int PAL_sysGetChipVersionInfo(void);

/*****************************************************************************
 * CACHE
 *****************************************************************************/

int PAL_sysCacheInvalidate(PAL_OsMemAddrSpace addrSpace,
                             void *mem_start_ptr,
                             unsigned int num_bytes);

int PAL_sysCacheFlush(PAL_OsMemAddrSpace addrSpace,
                        	void *mem_start_ptr,
                        	unsigned int num_bytes);

int PAL_sysCacheFlushAndInvalidate(PAL_OsMemAddrSpace addrSpace,
                             void *mem_start_ptr,
                             unsigned int num_bytes);

#include "pal_sysWdtimer.h"
#include "pal_sysTimer16.h"
#endif
