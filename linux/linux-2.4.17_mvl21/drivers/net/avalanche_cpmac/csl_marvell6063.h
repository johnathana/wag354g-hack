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

#ifndef __SWITCH_CSL_MARVELL_6063_H__
#define __SWITCH_CSL_MARVELL_6063_H__


/*******************************************************************************
*
* cslMarvell6063Start - Does the primary initialization of the switch.
*
* Performs the initialization of the switch and switch specific SW data.
*
* INPUTS:
*
*
* RETURNS: 
*       operational status to be returned to user
*
*
*/
Int32 cslMarvell6063Start (void);
Int32 cslMarvell6060Start (void);

/* According to operation type and port number we can retreive the PHY address */
/*******************************************************************************
*
* cslMarvell6063RegVal2OperStatus - extract the user operational status out of register value.
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

Int32 cslMarvell6063RegVal2OperStatus (OPER_TYPE_E oper, Uint16 data);

/*******************************************************************************
*
* cslMarvell6063OperStatus2RegVal - constructs the register value to be written out of data and user request
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

Uint16 cslMarvell6063OperStatus2RegVal (OPER_TYPE_E oper,Int32 operValue,Uint16 data);


/*******************************************************************************
*
* cslMarvell6063SwitchGetVersion - Get switch specific data
*
* 
*
* INPUTS:
*     [In]    PSW_VER_INFO pSwitchInfo
* 
*
*
* RETURNS: 
*       The final register value
*
*
*/

void cslMarvell6063SwitchGetVersion(PSW_VER_INFO pSwitchVerInfo);
void cslMarvell6060SwitchGetVersion(PSW_VER_INFO pSwitchVerInfo);

Int32 cslMarvell6063SpecialCapsSet(Uint32 cmd, Uint32 arg);
Int32 cslMarvell6060SpecialCapsSet(Uint32 cmd, Uint32 arg);

/*******************************************************************************
*
* cslMarvell6063prepareReadStatSequence - prepare the switch for reading statistic counters.
*
* Prepare the switch for reading statistic counters (if needed)
*
* INPUTS:
*     [In]    Uint32            port
* 
*
*
* RETURNS: 
*       Status
*
*
*/


Int32 cslMarvell6063PrepareReadStatSequence(Uint32 port);

/*******************************************************************************
*
* cslMarvell6063getPreparedCounters - read statistics counters.
*
* 
*
* INPUTS:
*     [In]    Uint32           statIndex
*     [Out]   Uint32*          counter
* 
*
*
* RETURNS: 
*       Status
*
*
*/


Int32 cslMarvell6063GetPreparedCounters(Uint32 statIndex,Uint32* counter);


/*******************************************************************************
*
* cslMarvell6063PortNum2RegAddr - returns the register address
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

Uint32 cslMarvell6063PortNum2RegAddr(OPER_TYPE_E type, Uint32 portNum);

/*******************************************************************************
*
* cslMarvell6063PortNum2PHYAddr - returns the PHY address
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


Uint32 cslMarvell6063PortNum2PHYAddr(OPER_TYPE_E type, Uint32 portNum);

/*******************************************************************************
*
* cslMarvell6063VlanUpdate - Updates the VLAN in VLAN table
*
* Writes the VLAN data to the VLAN table.
*
* INPUTS:
*     [In]   PCSL_SWITCH_VLAN_TABLE pVlnEntry
*
*
* RETURNS: 
*       Status
*
*
*/

Int32 cslMarvell6063VlanUpdate(PCSL_SWITCH_VLAN_TABLE pVlnEntry);


/*******************************************************************************
*
* cslMarvell6063VlanDefaultSet - Defines the default VLAN
*
* Defines the default VLAN. The meaning can change from implementation to implementation.
*
* INPUTS:
*     [In]   Uint32         portNum
*     [In]   PCSL_SWITCH_VLAN_TABLE pVlnEntry
*
*
* RETURNS: 
*       Status
*
*
*/


Int32 cslMarvell6063VlanDefaultSet(Uint32 portNum,PCSL_SWITCH_VLAN_TABLE pVlnEntry);



/*******************************************************************************
*
* cslMarvell6063PrioritySet - Sets the default priority of the packets from this port
*   
*   Sets the default priority of the packets from this port.
*
* INPUTS:
*     [In]   Uint32                 portNum
*     [In]   Uint32                 priority
*
*
* RETURNS: 
*       Status
*
*
*/


Int32 cslMarvell6063PrioritySet(Uint32 portNum, Uint32 priority);



/*******************************************************************************
*
* cslMarvell6063PriorityGet - Gets the default priority of the packets from this port
*   
*   Gets the default priority of the packets from this port.
*
* INPUTS:
*     [In]   Uint32                 portNum
*     [In]   Uint32                 priority
*
*
* RETURNS: 
*       Status
*
*
*/


Uint32 cslMarvell6063PriorityGet(Uint32 portNum);

/*******************************************************************************
*
* cslMarvell6063PurgeAtuEntry - Purge an entry from the address translation Unit table.
*
*
*
* INPUTS:
*     [In]   char MacAddress[6]
*
*
* RETURNS: 
*       Status
*
*
*/

Int32 cslMarvell6063PurgeAtuEntry(char MacAddress[6]);

#endif /* __SWITCH_CSL_MARVELL_6063_H__*/


