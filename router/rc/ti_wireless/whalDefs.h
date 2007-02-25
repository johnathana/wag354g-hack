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
**|     Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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
/***************************************************************************
**                                                                        **
**   MODULE:    whalDefs.h                                                **
**   PURPOSE:   Wireless HAL Common Definitions                           **
**                                                                        **
***************************************************************************/
#ifndef _WHALDEFS_H_
#define _WHALDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif


#define OK  0
#define NOK 1

#define INVALID_PARAMETER1      -101
#define INVALID_PARAMETER2      -102
#define INVALID_PARAMETER3      -103
#define INVALID_PARAMETER4      -104

#define IO_ERROR                -2


#define WHAL_MAC_ADDR_LEN       6
#define WHAL_SER_NUM_LEN        256   
#define WHAL_MAX_FILE_NUM       2

#define WHAL_FILE_NAME_LEN      64
#define ACX_NUM_OF_ANTENNAS     2
#define ACX_CCK_MODULATION      0
#define ACX_PLCP_MODULATION     1

#define DOT11_MAX_DTIM_PERIOD               255

#define DOT11_OPEN_SYSTEM                   1
#define DOT11_SHARED_KEY                    2

#define DOT11_MAX_WEP_DEFAULT_KEY           4

#define DOT11_WEP_KEY_MAPPING_LEN           10

#define DOT11_RTS_TRESHOLD_MAX              4096 /* 2347 if not 4X */

#define DOT11_MIN_SHORT_RETRY_LIMIT         1
#define DOT11_MAX_SHORT_RETRY_LIMIT         255

#define DOT11_MIN_LONG_RETRY_LIMIT          1
#define DOT11_MAX_LONG_RETRY_LIMIT          255

#define DOT11_MIN_FRAGMENTATION_TRESHOLD    256
#define DOT11_MAX_FRAGMENTATION_TRESHOLD    4096 /* 2346 if not 4X */

#define DOT11_MIN_TX_MSDUL_LIFETIME         1
#define DOT11_MIN_RX_LIFETIME               1
#define DOT11_MANUFACTURER_ID_LEN           129
#define DOT11_PRODUCT_ID_LEN                129
#define DOT11_MANUFACTURER_OUI_LEN          3
#define DOT11_MANUFACTURER_NAME_LEN         129
#define DOT11_MANUFACTURER_PRODUCT_NAME_LEN 129
#define DOT11_MANUFACTURER_PRODUCT_NAME_LEN 129
#define DOT11_MANUFACTURER_PRODUCT_VER_LEN  129
#define DOT11_MANUFACTURER_VER_LEN          129
#define DOT11_MAX_DEFAULT_KEY               3
#define DOT11_MAX_KEY_MAPPING_LEN           10

#define ACX_BSS_ID_LEN                  6
#define ACX_SSI_MAX_LEN                 32 
#define ACX_MAX_CMD_PARAM_LEN           384
#define ACX_MAC_ADDR_LEN                6
#define ACX_MAX_WEP_KEY_LEN             232

#define ACX_MAX_CMD_PARAMS_LEN          384
#define ACX_MAX_INF_MESSAGE_LEN          44
#define DOT11_DEFS_KEY_NAL_LEN            4


#define ACX_BSS_ID_LEN                  6
#define ACX_SSI_MAX_LEN                 32 
#define ACX_MAX_CMD_PARAM_LEN           384
#define ACX_MAC_ADDR_LEN                6
#define ACX_MAX_WEP_KEY_LEN             232

#define IMMEDIATELY                     1
#define DELAYED                         0

#define	DOT11_WPA_STA_TYPE_WPA					0x01
#define	DOT11_WPA_STA_TYPE_WEP_LEGACY			0x02
#define	DOT11_WPA_STA_TYPE_WEP_8021X			0x04

#define	DOT11_WPA_PSK_PARAM_NONE					0
#define	DOT11_WPA_PSK_PARAM_HEX						1
#define	DOT11_WPA_PSK_PARAM_ASCII					2

#define	DOT11_CIPHER_WEP_40			0x2
#define	DOT11_CIPHER_WEP_104		0x4
#define	DOT11_CIPHER_WEP_232		0x8



/***********************
 * TestApp defaults    *
 ***********************/
#define TA_STARTUP_DEFAULT            TA_STARTUP_OPT_NORMAL
#define TA_RADIOSCOPE_COMOPT_DEFAULT  TA_RADIOSCOPE_COMOPT_CBR115200

#ifdef __cplusplus
#endif
#endif /* #ifndef _WHALDEFS_H_ */

