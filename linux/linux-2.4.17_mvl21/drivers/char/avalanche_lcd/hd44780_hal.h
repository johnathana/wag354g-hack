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
**|         Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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

/**************************************************************************
 * FILE PURPOSE :   HAL header for Hitachi HD44780 controller.
 **************************************************************************
 * FILE NAME    :   hd_44780_hal.h
 *
 * DESCRIPTION  :
 *  HAL header for Hitachi HD44780 controller`.
 *
 *  CALL-INs:
 *
 *  CALL-OUTs:
 *
 *  User-Configurable Items:
 *
 *  (C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/

#ifndef _HD47780_HAL_H_
#define _HD47780_HAL_H_

/* 
 * Define void* os_platform_malloc(unsigned int), os_platform_free(void*), void os_platform_cli(unsigned int),
 * unsigned int os_platform_sti(unsigned int)
 */
#include "ti_lidd_cmd.h"

/**********************************************************************
 * Returns: NULL in case of error, otherwise a handle to be used in sub-
 * sequent calls. 
 *********************************************************************/
TI_HD47780_T* ti_hd47780_init(unsigned int* cntl_reg, 
		              unsigned int* data_reg,
			      unsigned char row,
			      unsigned char col, 
			      unsigned int cpufreq);

/**********************************************************************
 * Returns: -1 for error otherwise 0 for success. 
 *********************************************************************/
int ti_hd47780_cleanup(TI_HD47780_T*);

/**********************************************************************
 * Returns: -1 for error, other 0 for success. 
 *
 *     cmd                                  val
 *
 *     TI_LIDD_CLEAR_SCREEN                 none
 *     TI_LIDD_CURSOR_HOME                  none
 *     TI_LIDD_DISPLAY                      0 - off, 1 - on
 *     TI_LIDD_GOTO_XY                      [row - 2 bytes][col - 2 bytes]
 *     TI_LIDD_BLINK                        0 - off, 1 - on 
 *     TI_LIDD_CURSOR_STATE                 0 - not visible, 1 - visible.
 *     TI_LIDD_SHIFT                        1 - Right shift, 0 - left shift.
 *     TI_LIDD_CURSOR_SHIFT                 1 - Right, 0 - left
 *     TI_LIDD_WR_CHAR                      character.
 *     TI_LIDD_RD_CHAR                      place holder for character. 
 *     TI_LIDD_CURSOR_MOVE                  1 - Right, 0 - Left 
 *     TI_LIDD_DISPLAY_MOVE                 1 - Right, 0 - Left.
 *     TI_LIDD_LINE_WRAP                    0 - off, 1 - on.
 *
 *********************************************************************/
int ti_hd47780_ioctl(TI_HD47780_T*, unsigned int cmd, unsigned int val);

#endif /* _HD47780_HAL_H_ */
