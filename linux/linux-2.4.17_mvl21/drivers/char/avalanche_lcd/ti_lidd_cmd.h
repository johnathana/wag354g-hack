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
 * FILE PURPOSE :   IOCTL commands for LCD LIDD controller.
 **************************************************************************
 * FILE NAME    :   ti_lidd_cmd.h
 *
 * DESCRIPTION  :
 *  IOCTL commands for LCD LIDD controller.
 *
 *  CALL-INs:
 *
 *  CALL-OUTs:
 *
 *  User-Configurable Items:
 *
 *  (C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/
#ifndef _TI_LIDD_CMD_H_
#define _TI_LIDD_CMD_H_

#define TI_LIDD_CLEAR_SCREEN   1
#define TI_LIDD_CURSOR_HOME    2
#define TI_LIDD_GOTO_XY        3       
#define TI_LIDD_DISPLAY        4
#define TI_LIDD_BLINK          5
#define TI_LIDD_CURSOR_STATE   6
#define TI_LIDD_DISPLAY_SHIFT  7
#define TI_LIDD_CURSOR_SHIFT   8
#define TI_LIDD_CURSOR_MOVE    9
#define TI_LIDD_DISPLAY_MOVE   10
#define TI_LIDD_WR_CHAR        11
#define TI_LIDD_RD_CHAR        12
#define TI_LIDD_LINE_WRAP      13
#define TI_LIDD_RD_CMD         14

#define RIGHT                  1
#define LEFT                   0
#define ON                     1
#define OFF                    0

extern void* os_platform_malloc(unsigned int size);
extern void  os_platform_free(void*);
extern void* os_platform_memset(void*, int, unsigned int);

#ifndef NULL
#define NULL    0x0
#endif

#endif /* _TI_LIDD_CMD_H_ */    
