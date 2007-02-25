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
 * FILE PURPOSE :   HAL code for Hitachi HD44780 controller.
 **************************************************************************
 * FILE NAME    :   hd_44780.c
 *
 * DESCRIPTION  :
 *  HAL code for Hitachi HD44780 controller.
 *
 *  CALL-INs:
 *
 *  CALL-OUTs:
 *
 *  User-Configurable Items:
 *
 *  (C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/

/*
 * A single HD47780 can be set in the following modes.
 * 
 *  8 x 1, 16 x 1, 16 x 2, 16 x 4, 20 x 1, 20 x 2, 20 X 4, 24 x 1, 24 x 2
 *
 * Different display set up affects the way we deal with the data location 
 * in the intenal data buffer (i.e. address in the data buffer).
 *
 */

typedef struct
{
    volatile unsigned int *cntl_reg;
    volatile unsigned int *data_reg;
    unsigned char          disp_row;
    unsigned char          disp_col;
    unsigned char          disp_cntl;
    unsigned char          entry_mode;

} TI_HD47780_T;


#include "hd44780_hal.h" 
#include "ti_lidd_cmd.h"

static int ti_hd47780_p_set_to_xy(TI_HD47780_T* p, int x, int y);
static unsigned int hd47780_cpufreq;




#define LCD_HD47780_BUSY 0x80

/* #define TI_LIDD_DELAY_US(delay) { volatile int count = 2 * (hd47780_cpufreq/1000000)*delay; while(count--);}  */
#define TI_LIDD_DELAY_US(delay, addr) { volatile int i; for(i=0;(*addr & LCD_HD47780_BUSY)&&(i<(hd47780_cpufreq/100000)*delay); i++); }

int ti_hd47780_p_set_to_xy(TI_HD47780_T* p, int x, int y)
{
    unsigned int cursor_offset = x + (y%2)*0x40;
    if(p->disp_row == 1) cursor_offset += (x%8)*(0x40 - 8); /* reqd for 1x16 */ 
    if((y%4) >= 2) cursor_offset += p->disp_col;

    /* load this offset into the lcd. */
    *p->cntl_reg = cursor_offset | 0x80;
    TI_LIDD_DELAY_US(50, p->cntl_reg);

    return (0);
}

TI_HD47780_T* ti_hd47780_init(unsigned int*  cntl_reg, unsigned int*  data_reg, 
		                      unsigned char  disp_row, unsigned char  disp_col,
		                      unsigned int   cpufreq_in)
{
    TI_HD47780_T *p_hd = os_platform_malloc(sizeof(TI_HD47780_T));
    char func_set = 0x20;

    if(!p_hd) return NULL;
 
    os_platform_memset(p_hd, 0, sizeof(TI_HD47780_T));

    p_hd->cntl_reg = cntl_reg;
    p_hd->data_reg = data_reg;
    p_hd->disp_row = disp_row;
    p_hd->disp_col = disp_col;
    hd47780_cpufreq        = cpufreq_in;

    if(!(p_hd->disp_row % 2))
        func_set |= 0x08;
    
    
    TI_LIDD_DELAY_US(200, p_hd->cntl_reg);
    
    if(*(p_hd->cntl_reg) & LCD_HD47780_BUSY)
    {
            os_platform_free(p_hd);
            return 0;
    }
    
    *p_hd->cntl_reg = func_set | 0x10;
    
    TI_LIDD_DELAY_US(50, p_hd->cntl_reg);

    return (p_hd);
}
 
int ti_hd47780_cleanup(TI_HD47780_T *p_obj)
{
    os_platform_free(p_obj);
    return (0);
}

int ti_hd47780_ioctl(TI_HD47780_T* p_obj, unsigned int cmd, unsigned int val)
{
    switch(cmd)
    {
        case TI_LIDD_CLEAR_SCREEN:
	    *p_obj->cntl_reg = 0x1;
	    TI_LIDD_DELAY_US(2000, p_obj->cntl_reg);
	    break;

	case TI_LIDD_CURSOR_HOME:
	    *p_obj->cntl_reg = 0x2;
	    TI_LIDD_DELAY_US(2000, p_obj->cntl_reg);
	    break;

	case TI_LIDD_GOTO_XY:
            ti_hd47780_p_set_to_xy(p_obj, (val & 0xff00) >> 8, (val & 0xff));
	    break;

	case TI_LIDD_DISPLAY:
	    if(val) 
	        p_obj->disp_cntl |= 0x4;
	    else
	        p_obj->disp_cntl &=~0x4;
	    *p_obj->cntl_reg      = p_obj->disp_cntl | 0x8;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_BLINK: /* Blink ON/OFF */
	    if(val)
	        p_obj->disp_cntl |= 0x1;
	    else
	        p_obj->disp_cntl &=~0x1;
	    *p_obj->cntl_reg     = 0x8 | p_obj->disp_cntl;
        
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_CURSOR_STATE: /* Cursor State ON/OFF */
	    if(val)  
	        p_obj->disp_cntl |= 0x2;
	    else
	        p_obj->disp_cntl &=~0x2;
	    *p_obj->cntl_reg      = 0x8 | p_obj->disp_cntl;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_DISPLAY_SHIFT: /* controls whether display will be shifted when                                   characters are read/written */
	    if(val)
	        p_obj->entry_mode |= 0x1;
	    else
	        p_obj->entry_mode &=~0x1;
	    *p_obj->cntl_reg       = 0x4 | p_obj->entry_mode;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_CURSOR_SHIFT:
	    if(val)
	        p_obj->entry_mode |= 0x2;
	    else
	        p_obj->entry_mode &=~0x2;
	    *p_obj->cntl_reg       = 0x4 | p_obj->entry_mode;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_CURSOR_MOVE:
	    if(val)
	        val = 0x4;
	    *p_obj->cntl_reg       = 0x10 | val;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_DISPLAY_MOVE:  /* moves the display LEFT/RIGHT */
	    if(val)
	        val = 0x4;
	    *p_obj->cntl_reg       = 0x18 | val;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_WR_CHAR:
	    *(p_obj->data_reg) = (char)val;
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_RD_CHAR:
	    *((unsigned char *)val) = *(p_obj->data_reg);
	    TI_LIDD_DELAY_US(50, p_obj->cntl_reg);
	    break;

	case TI_LIDD_RD_CMD:
	    *((unsigned int *)val) = *p_obj->cntl_reg;
	    break;
	    
	default:
	    break;
        
    }

    return (0);
}


