/***************************************************************************
**+----------------------------------------------------------------------+**
**|                                ****                                  |**
**|                                ****                                  |**
**|                                ******o***                            |**
**|                          ********_///_****                           |**
**|                           ***** /_//_/ ****                          |**
**|                            ** ** (__/ ****                           |**
**|                                *********                             |**
**|                                 ****                                 |**
**|                                  ***                                 |**
**|                                                                      |**
**|     Copyright (c) 2003 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                           |**
**|                                                                      |**
**| Permission is hereby granted to licensees of Texas Instruments       |**
**| Incorporated (TI) products to use this computer program for the sole |**
**| purpose of implementing a licensee product based on TI products.     |**
**| No other rights to reproduce, use, or disseminate this computer      |**
**| program, whether in part or in whole, are granted.                   |**
**|                                                                      |**
**| TI makes no representation or warranties with respect to the         |**
**| performance of this computer program, and specifically disclaims     |**
**| any responsibility for any damages, special or consequential,        |**
**| connected with the use of this program.                              |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/

/* This file defines Vlynq module parameters*/

#ifndef _VLYNQ_HAL_PARAMS_H
#define _VLYNQ_HAL_PARAMS_H

 /* number of VLYNQ memory regions supported */
#define VLYNQ_MAX_MEMORY_REGIONS 0x04
  
 /* Max.number of external interrupt inputs supported by VLYNQ module */
#define VLYNQ_IVR_MAXIVR         0x08

#define VLYNQ_CLK_DIV_MAX  0x08
#define VLYNQ_CLK_DIV_MIN  0x01


/*** the total number of entries allocated for ICB would be
 * 32(for 32 bits in IntPending register) + VLYNQ_IVR_CHAIN_SLOTS*/
#define VLYNQ_IVR_CHAIN_SLOTS 10


#endif /* _VLYNQ_HAL_PARAMS_H */
