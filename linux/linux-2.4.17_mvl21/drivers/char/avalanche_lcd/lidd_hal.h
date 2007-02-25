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
 * FILE PURPOSE :   HAL header for LCD LIDD controller.
 **************************************************************************
 * FILE NAME    :   lidd_hal.h
 *
 * DESCRIPTION  :
 *  HAL code for LCD LIDD controller.
 *
 *  CALL-INs:
 *
 *  CALL-OUTs:
 *
 *  User-Configurable Items:
 *
 *  (C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/

#ifndef _LIDD_HAL_H_
#define _LIDD_HAL_H_

/* 
 * Define void* os_platform_malloc(unsigned int), os_platform_free(void*), void os_platform_cli(unsigned int),
 * unsigned int os_platform_sti(unsigned int)
 */
#include "ti_lidd_cmd.h"

typedef struct 
{
    unsigned int  base_addr;
    unsigned int  vbus_freq;     /* in Hz. */
    unsigned int  cpu_freq;      /* in Hz. */
    unsigned int  disp_row;      /* total number of row. */
    unsigned int  disp_col;      /* total number of col. */
    unsigned int  line_wrap;     /* whether to wrap the line. */
    unsigned int  cursor_blink;  
    unsigned int  cursor_show;
    unsigned int  lcd_type;      /* 0 = Sync MPU68, 1 = Async MPU68, 2 = Sync MPU80, 3 = Aync MPU80, 4 = Hitachi (Async) */
    unsigned int  num_lcd;       /* num of hd44780 or equivalnet lcd. The valid values are 1 or 2.*/
} TI_LIDD_INFO_T;

/**********************************************************************
 * Returns: NULL in case of error, otherwise a handle to be used in sub-
 * sequent calls. 
 *********************************************************************/
LIDD_HAL_OBJ_T* ti_lidd_hal_init(TI_LIDD_INFO_T*);

/**********************************************************************
 * Returns: -1 for error otherwise 0 for success. 
 *********************************************************************/
int ti_lidd_hal_open(LIDD_HAL_OBJ_T*);
int ti_lidd_hal_close(LIDD_HAL_OBJ_T*);
int ti_lidd_hal_cleanup(LIDD_HAL_OBJ_T*);

/***********************************************************************
 * Returns: -1 for error, otherwise the number of bytes that were 
 *          actually written.
 *
 *          The write begins at the current address location.
 *
 * Note: Here, the character array is not assumed to be NULL terminted.
 *
 **********************************************************************/
int ti_lidd_hal_write(LIDD_HAL_OBJ_T*, char*, unsigned int size);

/***********************************************************************
 * Returns: -1 for error, otherwise the number of bytes that were 
 *          read.
 *
 *          The read begins at the current address location.
 *
 * Note: Here, the character array is not assumed to be NULL terminted.
 *
 **********************************************************************/
int ti_lidd_hal_read(LIDD_HAL_OBJ_T*, char*, unsigned int size);

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
int ti_lidd_hal_ioctl(LIDD_HAL_OBJ_T*, unsigned int cmd, unsigned int val);

#endif /* _LIDD_HAL_H_ */
