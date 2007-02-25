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

#ifndef __SWITCH_CSL_KENDIN_8995x_H__
#define __SWITCH_CSL_KENDIN_8995x_H__


/* According to operation type and port number we can retreive the PHY address */
/*******************************************************************************
*
* cslKendin8995xRegVal2OperStatus - extract the user operational status out of register value.
*
* Enable/Disable a selected port
*
* INPUTS:
*     [In]    OPER_TYPE oper 
*     [In]    UINT_16   data
* 
*
*
* RETURNS: 
*       operational status to be returned to user
*
*
*/

Int32 cslKendin8995xRegVal2OperStatus (OPER_TYPE_E oper, Uint16 data);

/*******************************************************************************
*
* cslKendin8995xOperStatus2RegVal - constructs the register value to be written out of data and user request
*
* Enable/Disable a selected port
*
* INPUTS:
*     [In]    OPER_TYPE     oper 
*     [In]    Int32        operValue
*     [In]    UINT_16       data
* 
*
*
* RETURNS: 
*       The final register value
*
*
*/

Uint16 cslKendin8995xOperStatus2RegVal (OPER_TYPE_E oper,Int32 operValue,Uint16 data);


/*******************************************************************************
*
* cslKendin8995xSwitchGetVersion - Get switch specific data
*
* 
*
* INPUTS:
*     [In]    PSWITCH_DRV_HANDLE hDrv
*     [In]    PSW_VER_INFO pSwitchInfo
* 
*
*
* RETURNS: 
*       The final register value
*
*
*/
void cslKendin8995xSwitchGetVersion(PCSL_SWITCH_DRV_HANDLE swDrv,PSW_VER_INFO pSwitchInfo);

/*******************************************************************************
*
* cslKendin8995xprepareReadStatSequence - Kendin8995x doesn't 
*                                         support statistics counters
*
*
*/

Int32 cslKendin8995xPrepareReadStatSequence(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 port);

/*******************************************************************************
*
* cslKendin8995xgetPreparedCounters - Kendin8995x doesn't 
*                                     support statistics counters
*
*/


Int32 cslKendin8995xGetPreparedCounters(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 statIndex,Uint32* counter);


/*******************************************************************************
*
* cslKendin8995xPortNum2RegAddr - returns the register address
*
* Retreive register address from port and operator type.
*
* INPUTS:
*     [In]   OPER_TYPE_E    type
*     [In]   Uint32         portNum
*
*
* RETURNS: 
*       Register address
*
*
*/

Uint32 cslKendin8995xPortNum2RegAddr(OPER_TYPE_E type, Uint32 portNum);

/*******************************************************************************
*
* cslKendin8995xPortNum2PHYAddr - returns the PHY address
*
* Retreive PHY address from port and operator type.
*
* INPUTS:
*     [In]   OPER_TYPE_E    type
*     [In]   Uint32         portNum
*
*
* RETURNS: 
*       Register address
*
*
*/


Uint32 cslKendin8995xPortNum2PHYAddr(OPER_TYPE_E type, Uint32 portNum);

/*******************************************************************************
*
* cslKendin8995xSpecialCapsSet - Kendin8995x Dosen't support VLAN
*                                     
*
*/
Int32 cslKendin8995xSpecialCapsSet(PCSL_SWITCH_DRV_HANDLE swHead, Uint32 cmd, Uint32 arg);
/*******************************************************************************
*
* cslKendin8995xStart - 

*
*/
Int32 cslKendin8995xStart (PCSL_SWITCH_DRV_HANDLE swHead);



#endif 


