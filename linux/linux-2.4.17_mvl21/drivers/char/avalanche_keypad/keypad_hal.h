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
 * FILE PURPOSE :   Keypad controller HAL header.
 **************************************************************************
 * FILE NAME    :   keypad_hal.h
 *
 * DESCRIPTION  :
 *  Keypad controller HAL header.
 *
 *  CALL-INs:
 *
 *  CALL-OUTs:
 *
 *  User-Configurable Items:
 *
 *  (C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/

#ifndef _TI_KPAD_CNTL_HAL_H_
#define _TI_KPAD_CNTL_HAL_H_

/* Note: please define the functions os_platform_malloc(), os_platform_free() 
 * and typedef void KPAD_HAL_OBJ_T in the driver code. This hides HAL obj
 * details from the world.
 */       

/* 
 * The keypad columns are configured as inputs, can occupy the GPIOs from 
 * 8 to 15. The keypad rows are configured as outputs, and can occupy the
 * GPIOs from 0 to 7.
 *
 * The column map should be of the order XX00 and the row map should be of
 * the order 00XX.
 */
typedef struct 
{
    unsigned int base_address;
    unsigned int debounce_time;   /* in ms */
    unsigned int module_freq;       /* in HZ */
    unsigned int column_map;
    unsigned int row_map;

} KEYPAD_DEV_INFO_T;

#define TI_KEY_PRESS_DETECT      1
#define TI_KEY_RELEASE_DETECT    2
#define TI_KEY_DEBOUNCE_VAL      3

/*****************************************************************************
 * Returns: NULL in case of error, otherwise valid keypad hal handle which 
 *          should be used in subsequent calls. 
 ****************************************************************************/
KPAD_HAL_OBJ_T *ti_kpad_hal_init(KEYPAD_DEV_INFO_T*);

/*****************************************************************************
 * Returns: -1 in case of error, otherwise 0. 
 ****************************************************************************/
int ti_kpad_hal_start(KPAD_HAL_OBJ_T*);
int ti_kpad_hal_close(KPAD_HAL_OBJ_T*);
int ti_kpad_hal_cleanup(KPAD_HAL_OBJ_T*);

/******************************************************************************
 * Returns: -1 in case of error or a value > 0 when success. The return value 
 *          of type integer is coded like this.
 *
 *          -----------------------
 *          | 00 | 00 | col | row |
 *          -----------------------
 *
 * This function is called to identify the key that was pressed. It is 
 * recommended that scanning is carried out in non-interrupt mode. The proce-
 * ssing is exhaustive. 
 *
 *****************************************************************************/
int ti_kpad_hal_key_scan(KPAD_HAL_OBJ_T*);

/*****************************************************************************
 * Returns: The event that has happened on the device. 
 *          0   - No event. 
 *          1   - Key pressed. The interrupt is acknowledged and disabled. 
 *          2   - Key released. The interrupt is acknowledged and disabled. 
 *
 * This routine gurantees that key release event follows key press. And that 
 * no other key press would be reported until the previously detected key press 
 * released. It follows a strict sequence.
 *
 * For identification of key pressed, please call scan function. For key 
 * release, no need for identification, and it can be safely assumed that the 
 * key previously reported as pressed has been released.
 ****************************************************************************/
int ti_kpad_hal_isr(KPAD_HAL_OBJ_T*);

/******************************************************************************
 * Returns: 0 on success and -1 otherwise.
 *
 *          cmd                      val
 *
 *          TI_KEY_PRESS_DETECT      none.
 *          TI_KEY_RELEASE_DETECT    none.
 *          TI_KEY_DEBOUNCE_VAL      val in milli-secs.
 *
 * Recommended that the device be enabled for TI_KEY_RELEASE_DETECT after scan.
 *             that the device be enabled for TI_KEY_PRESS_DETECT after key release.
 *
 * When av_kpad_cntrl_hal_open is called the device is enabled for TI_KEY_PRESS_DETECT. 
 *
 *****************************************************************************/
int ti_kpad_hal_ioctl(KPAD_HAL_OBJ_T*, unsigned int cmd, unsigned int val);

#endif
