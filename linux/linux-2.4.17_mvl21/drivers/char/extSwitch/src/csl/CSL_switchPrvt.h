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

#ifndef __SWITCH_PRVT_H__
#define __SWITCH_PRVT_H__

#include "CSL_mdio.h"
#include "DDC_switch.h"

/* Macroses for accessing the switch */

#define phyRegWrite(swHead, regadr, phyadr, data)\
        mdioWrite(swHead->initParams.miiBase, swHead->initParams.channelNumber,regadr, phyadr, data)
    
#define phyRegRead(swHead, regadr, phyadr, data)\
        data = mdioRead(swHead->initParams.miiBase, swHead->initParams.channelNumber, regadr, phyadr)
        
/* Internal enumeration for the statistic counters */

typedef enum
{
       InBytes = 0, 
       InBytesHigh,
       InUnicastPkts,
       InUnicastPktsHigh,
       InMulticastPkts,
       InMulticastPktsHigh,
       InBroadcastPkts,
       InBroadcastPktsHigh,
       InDiscardPkts,
       InErrorPkts,
       InUnknownProtPkts,
       OutBytes,
       OutBytesHigh,
       OutUnicastPkts,
       OutUnicastPktsHigh,
       OutMulticastPkts,
       OutMulticastPktsHigh,
       OutBroadcastPkts,
       OutBroadcastPktsHigh,
       OutDiscardPkts,
       OutErrorPkts,
       ethAlignmentErrors,    
       ethFCSErrors,          
       ethSingleCollisions,   
       ethMultipleCollisions, 
       ethSQETestErrors,      
       ethDeferredTxFrames,   
       ethLateCollisions,     
       ethExcessiveCollisions,
       ethInternalMacTxErrors,
       ethCarrierSenseErrors, 
       ethTooLongRxFrames,    
       ethInternalMacRxErrors,
       ethSymbolErrors

} SWITCH_STAT_E;


/* Operation type */

typedef enum
{
    OPER_STATUS,
    OPER_LINK,
    OPER_DUPLEX,
    OPER_DUPLEX_SET,
    OPER_SPEED,
    OPER_SPEED_SET,
    OPER_FC,
    OPER_AUTO,
    OPER_INGRESS,
    OPER_INGRESS_OFF,
    OPER_PORT_RECOGNITION_ON,
    OPER_TRANSMIT_ON_FIXED_PORT,
    OPER_VLANTUNNEL,
    OPER_RESET_STAT
}OPER_TYPE_E;

typedef  struct _csl_switch_drv_handle CSL_SWITCH_DRV_HANDLE;
typedef  struct _csl_switch_drv_handle *PCSL_SWITCH_DRV_HANDLE ; 
typedef  struct _csl_switch_vlan_table CSL_SWITCH_VLAN_TABLE;
typedef  struct _csl_switch_vlan_table *PCSL_SWITCH_VLAN_TABLE;

/* callbacks types for the switch */
/* base switch */
typedef Int32  (*CSL_SWITCH_STOP          )(CSL_SWITCH_DRV_HANDLE* hDrv);
typedef Int32  (*CSL_SWITCH_START          )(CSL_SWITCH_DRV_HANDLE* hDrv);

typedef Int32   (*CSL_REG_VAL2OPER_STATUS        )(OPER_TYPE_E oper, Uint16 data);   
typedef Uint16  (*CSL_OPER_STATUS2REG_VAL        )(OPER_TYPE_E oper,Int32 operValue,Uint16 dat);  
typedef void    (*CSL_SWITCH_GET_VERSION         )(CSL_SWITCH_DRV_HANDLE* hDrv,PSW_VER_INFO pSwitchInfo);   
typedef Int32   (*CSL_PREPARE_READ_STAT_SEQUENCE )(CSL_SWITCH_DRV_HANDLE* pDrvHead,Uint32 port);   
typedef Int32   (*CSL_GET_PREPARED_COUNTERS      )(CSL_SWITCH_DRV_HANDLE* pDrvHead,Uint32 statIndex,Uint32* counter);   
typedef Uint32  (*CSL_PORT_NUM2REG_ADDR          )(OPER_TYPE_E type, Uint32 portNum);
typedef Uint32  (*CSL_PORT_NUM2PHY_ADDR          )(OPER_TYPE_E type, Uint32 portNum);

/* advanced switch */
typedef Int32   (*CSL_VLN_DEFAULT_SET )(CSL_SWITCH_DRV_HANDLE* pSwHead, Uint32 portNum,PCSL_SWITCH_VLAN_TABLE pVlnEntry);
typedef Int32  (*CSL_VLN_UPDATE      )(CSL_SWITCH_DRV_HANDLE* pSwHead, PCSL_SWITCH_VLAN_TABLE pVlnEntry);
typedef Int32   (*CSL_VLN_PRIORITY_SET)(CSL_SWITCH_DRV_HANDLE* pSwHead, Uint32 port,Uint32 priority);
typedef Uint32  (*CSL_VLN_PRIORITY_GET)(CSL_SWITCH_DRV_HANDLE* pSwHead, Uint32 port);
typedef Int32   (*CSL_VLN_CAPS_SET    )(CSL_SWITCH_DRV_HANDLE* pSwHead, Uint32 cmd, Uint32 arg);
typedef Int32   (*CSL_DELETE_MAC_ADD  )(CSL_SWITCH_DRV_HANDLE* pSwHead, char MacAdd[6]);



typedef struct 
{

    CSL_SWITCH_STOP                 cslSwitchStop;  
    CSL_SWITCH_START                cslSwitchStart; 
    CSL_REG_VAL2OPER_STATUS         cslRegVal2operStatus;   
    CSL_OPER_STATUS2REG_VAL         cslOperStatus2regVal;   
    CSL_SWITCH_GET_VERSION          cslSwitchGetVersion;    
    CSL_PREPARE_READ_STAT_SEQUENCE  cslPrepareReadStatSequence;
    CSL_GET_PREPARED_COUNTERS       cslGetPreparedCounters;     
    CSL_PORT_NUM2REG_ADDR           cslPortNum2RegAddr;     
    CSL_PORT_NUM2PHY_ADDR           cslPortNum2PhyAddr;   

/* advanced switch management */

    CSL_VLN_DEFAULT_SET             cslVlnDefaultSet;
    CSL_VLN_UPDATE                  cslVlnUpdate;
    CSL_VLN_PRIORITY_SET            cslVlnPrioritySet;
    CSL_VLN_PRIORITY_GET            cslVlnPriorityGet;
    CSL_VLN_CAPS_SET                cslVlnCapsSet;
    CSL_DELETE_MAC_ADD              cslDeleteMacAddEntry;
}CSL_SWITCH_CLBKS,*PCSL_SWITCH_CLBKS;


struct _csl_switch_vlan_table
{
    Bool valid;
    Uint32 vlanId;
    Uint32 portMask;
    Uint32 portTagMask;
    PCSL_SWITCH_VLAN_TABLE next;
};



/* Switch Identification */
struct _csl_switch_drv_handle
{
    CSL_SWITCH_CLBKS        cslSwitchClbks;
    SWITCH_INIT             initParams;
    SWITCH_SPECIAL_CAPS     specCaps;
    Uint32                  numberOfPorts;
    Uint32                  additionalInfo;
    PCSL_SWITCH_VLAN_TABLE  pVlanTable;
    PCSL_SWITCH_DRV_HANDLE  next;
};

/* Switch driver identificator*/
typedef struct 
{
    
    Uint32 numberOfSwitches;
    PCSL_SWITCH_DRV_HANDLE switchesList;

} SWITCH_DRV_HANDLE,*PSWITCH_DRV_HANDLE;


/*******************************************************************************
* cslConnectSwitch - switch initialization
*
*  This API does the callback structure initialization. Here the type of the switch is considered.
*
* INPUTS:
*       [In/Out] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

Int32 cslConnectSwitch(PCSL_SWITCH_DRV_HANDLE swHead);

/*******************************************************************************
* cslSwitchRmv - switch deinitialization
*
*  
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

PAL_Result cslSwitchRmv(PCSL_SWITCH_DRV_HANDLE swHead);

/*******************************************************************************
* cslGetVLANEntry - gets VLAN entry
*
*  Retreives vlnId entry from VLAN table. List manager.
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*       [In] Uint32 vlnId
*
*******************************************************************************/

PCSL_SWITCH_VLAN_TABLE cslGetVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 vlnId);

/*******************************************************************************
* cslCreateVLANEntry - creates and adds VLAN entry to the VLAN table
*
*  Creates and adds VLAN entry to the logical VLAN table. List manager.
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

PCSL_SWITCH_VLAN_TABLE cslCreateVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead);

/*******************************************************************************
* cslDestroyVLANEntry - destroys VLAN entry
*
*  Removes VLAN entry from VLAN table. List manager.
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

Int32 cslDestroyVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead, PCSL_SWITCH_VLAN_TABLE elem);

/*******************************************************************************
*
* cslMarvell6063PrioritySet - Sets the default priority of the packets from this port
*   
*   Sets the default priority of the packets from this port.
*
* INPUTS:
*     [In]   PCSL_SWITCH_DRV_HANDLE pDrvHead
*     [In]   Uint32                 portNum
*     [In]   Uint32                 priority
*
*
* RETURNS: 
*       Status
*
*
*/

Int32 cslMarvell6063SpecialCapsSet(PCSL_SWITCH_DRV_HANDLE swHead, Uint32 cmd, Uint32 arg);



/*******************************************************************************
*
* cslMarvell6063Start - Does the primary initialization of the switch.
*
* Performs the initialization of the switch and switch specific SW data.
*
* INPUTS:
*       PCSL_SWITCH_DRV_HANDLE swHead
*
*
* RETURNS: 
*       operational status to be returned to user
*
*
*/

Int32 cslMarvell6063Start (PCSL_SWITCH_DRV_HANDLE swHead);

#endif /* __SWITCH_PRVT_H__ */
