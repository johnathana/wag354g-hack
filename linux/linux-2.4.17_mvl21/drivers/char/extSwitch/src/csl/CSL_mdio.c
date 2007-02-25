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
#include "_tistdtypes1.h"
#include "CSL_mdio.h"


#define         MDIO_USERACCESS_GO     (1 << 31)
#define         MDIO_USERACCESS_WRITE  (1 << 30)
#define         MDIO_USERACCESS_READ   (0 << 30)
#define         MDIO_USERACCESS_ACK    (1 << 29)
#define         MDIO_USERACCESS_REGADR (0x1F << 21)
#define         MDIO_USERACCESS_PHYADR (0x1F << 16)
#define         MDIO_USERACCESS_DATA   (0xFFFF)

#define pMDIO_USERACCESS(base, channel) ((volatile Uint32 *)(base+(0x80+(channel*8))))
#define MDIO_USERACCESS(base, channel)      (*(pMDIO_USERACCESS(base, channel)))

#define mdioWaitForAccessComplete(base, channel) while((MDIO_USERACCESS(base, channel) & MDIO_USERACCESS_GO)!=0)


/*******************************************************************************
* mdioUserAccess    
*
* DESCRIPTION:
*           accesses the given MDIO mdio channel with a given command
*
* INPUTS:
*      [IN] Uint32 miiBase
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

static void mdioUserAccess(Uint32 instance,Uint32 channelNumber, Uint32 method, Uint32 regadr, Uint32 phyadr, Uint32 data)
{
  Uint32  control;

  control =  MDIO_USERACCESS_GO |
             (method) |
             (((regadr) << 21) & MDIO_USERACCESS_REGADR) |
             (((phyadr) << 16) & MDIO_USERACCESS_PHYADR) |
             ((data) & MDIO_USERACCESS_DATA);

  MDIO_USERACCESS(instance,channelNumber) = control;
}



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


Uint32 mdioRead(Uint32 instance,Uint32 channelNumber, Uint32 regadr, Uint32 phyadr)
{

  mdioWaitForAccessComplete(instance,channelNumber);
  mdioUserAccess(instance,channelNumber, MDIO_USERACCESS_READ, regadr, phyadr, 0);
  mdioWaitForAccessComplete(instance,channelNumber);

  return(MDIO_USERACCESS(instance,channelNumber) & MDIO_USERACCESS_DATA);
}

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

void mdioWrite(Uint32 instance,Uint32 channelNumber, Uint32 regadr, Uint32 phyadr, Uint32 data)
{
  mdioWaitForAccessComplete(instance,channelNumber);  /* Wait until UserAccess ready */
  mdioUserAccess(instance, channelNumber , MDIO_USERACCESS_WRITE, regadr, phyadr, data);
}



