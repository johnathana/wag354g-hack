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

/*******************************************************************************
 * FILE PURPOSE:    PSPboot configuration parameters.
 *******************************************************************************
 * FILE NAME:       sysconf.h
 *
 * DESCRIPTION:     PSPboot configuration parameters.
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef _SYSCONF_H_
#define _SYSCONF_H_

#include "platform.h"

#define MEG(x)      ((x) << 20)
#define MHZ(x)      ((x) * 1000 * 1000)
#define KHZ(x)      ((x) * 1000)

#define CACHE_WT     1
#define CACHE_NONE   2
#define CACHE_WB     3

/* Pre-processor macro to get the board name string into the code
 * This depends of BOARD=XXX and XXX=XXX being defined externally 
 * (probably through a makefile). The BOARDNAME macro will return the 
 * string "XXX"
 */
#define BRDSTR(E)   #E
#define MKSTR(X)    BRDSTR(X)
#define BOARDNAME   MKSTR(BOARD)

/* Cable Modem 401B board: Avalanche I SoC */
#if defined (TNETC401B) 

#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* Cable Modem 520 board: Puma S SoC */
#elif defined (TNETC520)

#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* Cable Modem 620 board: Puma S SoC */
#elif defined (TNETC620)

#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(8)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* Cable Modem 420 board: Puma S SoC */
#elif defined (TNETC420)

#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(2)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* Cable Modem 405T board: Avalanche D SoC */
#elif defined (TNETC405T)

#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(2)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* AR7 Verification and Debug Board. */
#elif defined(AR7VDB)   
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(32)
#define CONF_FLASH_SZ       MEG(16)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WB

/* AR7 development boards */
#elif defined(AR7DB) 
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WB

#elif defined(AR7RD) 
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(2)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#elif defined(AR7WRD) 
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(32)
#define CONF_FLASH_SZ       MEG(8)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#elif defined(AR7Wi)
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#elif defined(AR7V)
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#elif defined (AR7VWi) 
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#elif defined (AR7L0)
#define TNETD73XX_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AFEXTAL_FREQ   MHZ(25)
#define CONF_XTAL3IN_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(150)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(2)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB        

/* TNETV1050 Verification and Debug Board */
#elif defined(TNETV1050VDB)   
#define TNETV1050_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AUDIO_FREQ     KHZ(8192)
#define CONF_ALT_FREQ       MHZ(12)
#define CONF_CPU_FREQ       KHZ(162500)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(64)
#define CONF_FLASH_SZ       MEG(16)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WT

/* TNETV1050 Software Development Board */
#elif defined(TNETV1050SDB)   
#define TNETV1050_BOARD
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_AUDIO_FREQ     KHZ(8192)
#define CONF_ALT_FREQ       MHZ(12)
#define CONF_CPU_FREQ       KHZ(162500)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(64)
#define CONF_FLASH_SZ       MEG(8)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WB

/* Wireless LAN Software Development AP */
#elif defined(WLAN)   
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       FWBGet_flash_type()
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* Wireless LAN Gateway Cable Modem */
#elif defined(TNETC621)   
#define CONF_REFXTAL_FREQ   MHZ(25)
#define CONF_CPU_FREQ       MHZ(125)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(16)
#define CONF_FLASH_SZ       FWBGet_flash_type()
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WT

/* MIPS SEAD2 emulation platform */
#elif defined(SEAD2)   
#define CONF_REFXTAL_FREQ   MHZ(32)
#define CONF_CPU_FREQ       MHZ(32)
#define CONF_SYS_FREQ       MHZ(32)
#define CONF_SDRAM_SZ       MEG(32)
#define CONF_FLASH_SZ       MEG(32)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WT

/* Apex Verification and debug board */
#elif defined(TNETV1020VDB)   
#define TNETV1020_BOARD
#define CONF_XTAL1IN_FREQ   MHZ(25)
#define CONF_XTAL2IN_FREQ   MHZ(25)
#define CONF_OSCIN_FREQ     MHZ(25)
#define CONF_CPU_FREQ       MHZ(275)
#define CONF_SYS_FREQ       KHZ(137500)
#define CONF_SDRAM_SZ       MEG(32)
#define CONF_FLASH_SZ       MEG(16)
#define CONF_NUM_MAC_PORT   2
#define CONF_CACHE          CACHE_WB

/* WLAN Apex board */
#elif defined(TNETWA115VAG)   
#define TNETV1020_BOARD
#define CONF_XTAL1IN_FREQ   MHZ(25)
#define CONF_XTAL2IN_FREQ   MHZ(25)
#define CONF_OSCIN_FREQ     MHZ(25)
#define CONF_CPU_FREQ       MHZ(250)
#define CONF_SYS_FREQ       MHZ(125)
#define CONF_SDRAM_SZ       MEG(8)
#define CONF_FLASH_SZ       MEG(4)
#define CONF_NUM_MAC_PORT   1
#define CONF_CACHE          CACHE_WB

#else
#error "Fatal Error: No board type defined"
#endif /* ifdef AVALANCHE_EVM3 */

#endif /* _SYSCONF_H_ */
