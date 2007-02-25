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
**|     Copyright (c) 1998 - 2004 Texas Instruments Incorporated         |**
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

#ifndef __SWITCH_CSL_MDIO_H_
#define __SWITCH_CSL_MDIO_H_


/*******************************************************************************
* mdioUserAccess    
*
* DESCRIPTION:
*           accesses the given MDIO mdio channel with a given command
*
* INPUTS:
*      [IN] Uint32 instance
*      [IN] Uint32 channelNumber 
*      [IN] Uint32 method 
*      [IN] Uint32 regadr 
*      [IN] Uint32 phyadr
*      [IN] Uint32 data
*
* RETURNS: 
*       handle to the switch driver.
*
*******************************************************************************/


Uint32 mdioRead(Uint32 instance,Uint32 channelNumber, Uint32 regadr, Uint32 phyadr);


/*******************************************************************************
* mdioWrite()    
*
* DESCRIPTION:
*      accesses the given MDIO mdio channel with a given command
*
* INPUTS:
*      [IN] Uint32 instance
*      [IN] Uint32 channelNumber 
*      [IN] Uint32 method 
*      [IN] Uint32 regadr 
*      [IN] Uint32 phyadr
*      [IN] Uint32 data
*
* RETURNS: 
*       NONE
*
*******************************************************************************/

void mdioWrite(Uint32 instance,Uint32 channelNumber, Uint32 regadr, Uint32 phyadr, Uint32 data);


#endif /* __SWITCH_CSL_MDIO_H_ */
