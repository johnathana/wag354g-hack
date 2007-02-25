/*******************************************************************************   
 * FILE PURPOSE:    LCD Module Driver Header file                                     
 *******************************************************************************   
 * FILE NAME:       lcd.h                                                   
 *                                                                                 
 * DESCRIPTION:     Header file for Linux LCD Driver                             
 *                                                                                 
 * REVISION HISTORY:  
 *   
 * Date           Description                               Author
 *-----------------------------------------------------------------------------
 * 27 Aug 2003    Initial Creation                          Sharath Kumar  
 * 
 * 16 Dec 2003    Updates for 5.7                           Sharath Kumar                                                          
 *                                                                                 
 * (C) Copyright 2003, Texas Instruments, Inc                                      
 ******************************************************************************/   
#ifndef _TI_LCD_H_
#define _TI_LCD_H_


/* LCD configuration params  */
#define    MAX_ROWS                          (4)  
#define    MAX_COLS                         (40)  
#define    DEFAULT_ROWS                      (2)  
#define    DEFAULT_COLS                     (24)  
#define    NO_LCD_DEVICES                    (1)  



typedef struct lcd_pos {
	int row;
	int column;
} LCD_POS;


/* Defines for IOCTLs */
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

#endif /* _TI_LCD_H_ */
