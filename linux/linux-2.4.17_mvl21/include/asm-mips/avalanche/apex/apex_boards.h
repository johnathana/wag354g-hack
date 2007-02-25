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

#ifndef _APEX_BOARDS_H
#define _APEX_BOARDS_H

// Let us define board specific information here. 

#if defined(CONFIG_MIPS_APEX_1020VDB) || defined(TNETV1020) || defined(CONFIG_MIPS_APEX_115VAG) 
#define XTAL1_FREQ                                  25000000
#define XTAL2_FREQ                                  25000000
#define OSC_FREQ                                    25000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x00007800
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0xffffffff /* Only applicable to 115 VAG. We need to edit this for other boards. */
#define AVALANCHE_LOW_CPMAC_HAS_EXT_SWITCH          0
#define AVALANCHE_HIGH_CPMAC_HAS_EXT_SWITCH         1
#endif



#if defined CONFIG_MIPS_SEAD2
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0xAAAAAAAA
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x55555555
#define AVALANCHE_LOW_CPMAC_HAS_EXT_SWITCH          0
#define AVALANCHE_HIGH_CPMAC_HAS_EXT_SWITCH         0
#include <asm/mips-boards/sead.h>
#endif


#endif
