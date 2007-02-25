/***************************************************************************
 Copyright(c) 2001, Texas Instruments Incorporated. All Rights Reserved.

 FILE:  ec_errors.h

 DESCRIPTION:
   This file contains definitions and function declarations for
   error code support.

 HISTORY:
   14Dec00 MJH             Added masking to EC_CLASS etc macros
   17Sep02 GSG             Added HAL support (new class&devices)
   03Oct02 GSG             Removed C++ style comments
***************************************************************************/
#ifndef _INC_EC_ERRORS
#define _INC_EC_ERRORS

/*
  31    - CRITICAL
  30-28 - CLASS         (ie. DIAG, KERNEL, FLASH, etc)
  27-24 - INSTANCE      (ie. 1, 2, 3, etc )
  23-16 - DEVICE        (ie. EMAC, IIC, etc)
  15-08 - FUNCTION      (ie. RX, TX, INIT, etc)
  07-00 - ERROR CODE    (ie. NO_BASE, FILE_NOT_FOUND, etc )
*/

/*---------------------------------------------------------------------------
  Useful defines for accessing fields within error code
---------------------------------------------------------------------------*/
#define CRITICAL_SHIFT    31
#define CLASS_SHIFT       28
#define INST_SHIFT        24
#define DEVICE_SHIFT      16
#define FUNCTION_SHIFT     8
#define ERROR_CODE_SHIFT   0

#define CRITICAL_MASK     1
#define CLASS_MASK        0x07
#define DEVICE_MASK       0xFF
#define INST_MASK         0x0F
#define FUNCTION_MASK     0xFF
#define ERROR_CODE_MASK   0xFF

#define EC_CLASS(val)     ((val&CLASS_MASK)      << CLASS_SHIFT)
#define EC_DEVICE(val)    ((val&DEVICE_MASK)     << DEVICE_SHIFT)
#define EC_INST(val)      ((val&INST_MASK)       << INST_SHIFT)
#define EC_FUNC(val)      ((val&FUNCTION_MASK)   << FUNCTION_SHIFT)
#define EC_ERR(val)       ((val&ERROR_CODE_MASK) << ERROR_CODE_SHIFT)

/*---------------------------------------------------------------------------
   Operation classes
---------------------------------------------------------------------------*/
#define EC_HAL              EC_CLASS(0)
#define EC_DIAG             EC_CLASS(8)

/*---------------------------------------------------------------------------
   Device types
---------------------------------------------------------------------------*/
#define EC_DEV_EMAC         EC_DEVICE(1)
#define EC_DEV_IIC          EC_DEVICE(2)
#define EC_DEV_RESET        EC_DEVICE(3)
#define EC_DEV_ATMSAR       EC_DEVICE(4)
#define EC_DEV_MEM          EC_DEVICE(5)
#define EC_DEV_DES          EC_DEVICE(6)
#define EC_DEV_DMA          EC_DEVICE(7)
#define EC_DEV_DSP          EC_DEVICE(8)
#define EC_DEV_TMR          EC_DEVICE(9)
#define EC_DEV_WDT          EC_DEVICE(10)
#define EC_DEV_DCL          EC_DEVICE(11)
#define EC_DEV_BBIF         EC_DEVICE(12)
#define EC_DEV_PCI              EC_DEVICE(13)
#define EC_DEV_XBUS         EC_DEVICE(14)
#define EC_DEV_DSLIF            EC_DEVICE(15)
#define EC_DEV_USB                      EC_DEVICE(16)
#define EC_DEV_CLKC                     EC_DEVICE(17)
#define EC_DEV_RAPTOR       EC_DEVICE(18)
#define EC_DEV_DSPC                     EC_DEVICE(19)
#define EC_DEV_INTC                     EC_DEVICE(20)
#define EC_DEV_GPIO                     EC_DEVICE(21)
#define EC_DEV_BIST                     EC_DEVICE(22)
#define EC_DEV_HDLC                     EC_DEVICE(23)
#define EC_DEV_UART                     EC_DEVICE(24)
#define EC_DEV_VOIC                     EC_DEVICE(25)
/* 9.17.02 (new HAL modules) */
#define EC_DEV_CPSAR        EC_DEVICE(0x1A)
#define EC_DEV_AAL5         EC_DEVICE(0x1B)
#define EC_DEV_AAL2         EC_DEVICE(0x1C)
#define EC_DEV_CPMAC        EC_DEVICE(0x1D)
#define EC_DEV_VDMA         EC_DEVICE(0x1E)
#define EC_DEV_VLYNQ        EC_DEVICE(0x1F)
#define EC_DEV_CPPI         EC_DEVICE(0x20)
#define EC_DEV_CPMDIO       EC_DEVICE(0x21)

/*---------------------------------------------------------------------------
   Function types
---------------------------------------------------------------------------*/
#define EC_FUNC_READ_CONF   EC_FUNC(1)
#define EC_FUNC_INIT        EC_FUNC(2)

/*---------------------------------------------------------------------------
   Error codes
---------------------------------------------------------------------------*/
#define EC_CRITICAL         (1<<CRITICAL_SHIFT)
#define EC_NO_ERRORS        0
#define EC_VAL_NO_BASE      EC_ERR(1)
#define EC_VAL_NO_RESET_BIT EC_ERR(2)
#define EC_VAL_NO_RESET     EC_ERR(3)
#define EC_VAL_BAD_BASE     EC_ERR(4)
#define EC_VAL_MALLOCFAILED EC_ERR(5)
#define EC_VAL_NO_RESETBASE EC_ERR(6)
#define EC_DEVICE_NOT_FOUND EC_ERR(7)

/*---------------------------------------------------------------------------
   Function declarations
---------------------------------------------------------------------------*/
extern void ec_log_error( unsigned int );

#endif /* _INC_EC_ERRORS */
