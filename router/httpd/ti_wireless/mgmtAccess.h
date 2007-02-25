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
**|     Copyright (c) 1998,1999,2003 Texas Instruments Incorporated      |**
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

/****************************************************************************/
/*                                                                          */
/*      MODULE:	mgmtAccess.h                                                */
/*      PURPOSE: MAC access list functionality                              */
/*      CREATOR: Alexander Sirotkin                                         */
/*               demiurg@ti.com                                             */
/*      UPDATES: ZZ, 09,July,2003                                           */
/*                                                                          */
/****************************************************************************/


#ifndef __CORE_MGMT_MGMTACCESS_H__
#define __CORE_MGMT_MGMTACCESS_H__

//#include "whalNVRAM.h"
#define MGMT_ACCESS_LIST_SIZE 64

extern STATUS mgmtAccessInit(void);
extern STATUS mgmtAccessSetMAC(char *mac_addr);
extern STATUS mgmtAccessRemMAC(UINT16 index);
extern STATUS mgmtAccessRemMACbyAddr(char *mac_addr);
extern STATUS mgmtAccessGetList(char *list, int *pLength);
extern STATUS mgmtAccessCallback(char *mac_addr, int mac_addr_len); 
extern void   mgmtAccessListStatus(char *list, int max_list, int *pNumOfEntries);
STATUS mgmtAccessSetList(char *list, int len);

#endif // __CORE_MGMT_MGMTACCESS_H__

