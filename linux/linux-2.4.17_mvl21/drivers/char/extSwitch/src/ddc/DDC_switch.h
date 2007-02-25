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

#ifndef __SWITCH_INTERFACE_H__
#define __SWITCH_INTERFACE_H__

#include "pal_defs.h"

/****************************************************************************************/
/* eswitchInterface.h - External  Switch Interface 										*/
/*
DESCRIPTION
  
   This file contains the interface to the basic Switch functionality. 
  
  SWITCH MANAGEMENT

   SwitchGetVersion()   -   Returns the switch Vendor ID / Version
   SwitchDrvCreate()    -   Creates instance of the switch driver. After the call the driver is inactive state.
   SwitchDrvStart()     -   Starts the driver. After the call the driver is in working state.
   SwitchDrvStop()      -   Stops the driver. Move the driver into inactive state.
   SwitchDrvDelete()    -   Deletes instance of the switch driver.
   
   
  PORT MANAGEMENT

   SpecCapsSet()   -   Passes a special configuration of the port
   SpecCapsGet()   -   Retreives a special configuration of the port
   PortStatusSet()   -   Sets the port status ,- disable/listening/learning/forwarding (if possible)
   PortStatusGet()   -   Gets the port status ,- disable/listening/learning/forwarding 
   PortLinkGet()     -   Gets port link status(Up/down)
   PortFCGet()       -   Gets flow control status(On/off)
   PortFCSet()       -   Sets flow control (On/off)
   PortDuplexGet()   -   Gets port duplex status (Full/Half)
   PortDuplexSet()   -   Sets port duplex (Full/Half)
   PortSpeedGet()    -   Gets port speed status(10/100)
   PortSpeedSet()    -   Sets port speed (10/100)     
   PortAutoGet()     -   Gets port auto status (ENABLE/DISABLE/RESTART)
   PortAutoSet()     -   Sets port auto (ENABLE/DISABLE/RESTART)
    
  STATISTICS

   StatPortGet()     -   retreives the statistic from the given port
   StatPortReset()   -   resets port statistic for the given port.
      
  VLAN MANAGEMENT

   VlanCreate()            -   Creates VLAN with the given ID.
   VlanDelete()            -   Deletes VLAN.
   VlanPortAdd()           -   Adds port to VLAN.
   VlanPortDelete()        -   Deletes port from VLAN.
   VlanCPUAdd()            -   Adds CPU to VLAN.
   VlanCPUAdd()            -   Deletes CPU from VLAN.
   VlanDefaultSet()        -   Sets default VLAN for the given port.
   VlanPortUpdate()        -   Updates port status in VLAN.
   VlanUpdate()            -   Updates VLAN.
   VlantSetPortPriority()  -   Sets the priority of a port.
   VlanGetPortPriority()   -   Gets the priority of a port.
********************************************************************************/


/***********************************************************************************************
            SWITCH MANAGEMENT
***********************************************************************************************/


typedef  Uint32 HANDLE ; 

/* Switch Identification */
typedef struct 
{
    Uint32     Model;
    Uint32     maxNumberOfPorts;
    Uint32	   maxNumberOfVLANs;
    Uint32     maxNumberOfPriorities; /* port based */
    Bool       is802_1_Q_capable;
    Bool       is802_1_P_capable;
	/* ...
	   ... */
}SW_VER_INFO,*PSW_VER_INFO;

/* */

typedef enum
{
    SWITCH_MARVELL_6060,
    SWITCH_MARVELL_6063,
    SWITCH_KENDIN_8995X,
    SWITCH_ADM_6995,
    SWITCH_RTL_8335
}SUPPORTED_SWITCHES_E;

/* Switch Identification */
typedef struct SWITCH_DEF
{
    Uint32 internalPortNum;    /* this is the "CPU"/internal port number needed for start configuration */
    Uint32 miiBase;            /* Base access for MDIO */ 
    Uint32 channelNumber;      /* channel number for accesing the PHY*/
    Uint32 instanceId;         /* intstance Id, which is referred to when application asks to turn to the driver */
    SUPPORTED_SWITCHES_E switchType;
}SWITCH_INIT,*PSWITCH_INIT;


/*******************************************************************************
* SwitchGetVersion
*
* DESCRIPTION:
*       This function returns the version of the switch.
*
* INPUTS:
*       [In] HANDLE hDrv
*       [Out] PSW_VER_INFO pSwitchInfo
*
*******************************************************************************/
PAL_Result SwitchGetVersion(HANDLE hDrv, PSW_VER_INFO pSwitchInfo  );

/*******************************************************************************
* SwitchGetHandle
*
* DESCRIPTION:
*       This function returns the version of the switch.
*
* INPUTS:
*      [IN] Uint32  instanceId 
*       
*
*******************************************************************************/
HANDLE SwitchGetHandle(Uint32  instanceId);

/*******************************************************************************
* SwitchDrvCreate    
*
* DESCRIPTION:
*           Creates instance of the switch driver. 
*           After the call the driver is inactive state.
*
* INPUTS:
*      [IN] PSWITCH_INIT   - switchInit
*
* RETURNS: 
*       handle to the switch driver.
*
*******************************************************************************/
HANDLE SwitchDrvCreate(void);

/*******************************************************************************
* SwitchDrvStart
*
* DESCRIPTION:
*        Starts the driver. Move the driver into active state. Here were the internal port would be 
*  configured (if needed).
*
* INPUTS:
*       [In] hDrv -  driver handle
*
* RETURNS: 
*       Int32         - Ok/Error
*
*******************************************************************************/
Int32 SwitchDrvStart( HANDLE hDrv,HANDLE  swId);

/*******************************************************************************
* SwitchDrvStop
*
* DESCRIPTION:
*       Stops the driver. Move the driver into inactive state.
*
* INPUTS:
*       [In] hDrv -  driver handle
*
* RETURNS: 
*       Int32         - Ok/Error
*
*******************************************************************************/
Int32 SwitchDrvStop( HANDLE hDrv,HANDLE  swId);

/*******************************************************************************
* SwitchDrvDelete
*
* DESCRIPTION:
*       Deinstantiate the driver.
*
* INPUTS:
*       [In] hDrv -  driver handle
*
* RETURNS: 
*       Int32         - Ok/Error
*
*******************************************************************************/
Int32 SwitchDrvDelete( HANDLE hDrv );

/*************************************************************************************************
* SwitchAdd
*
* DESCRIPTION:
*       Adds and software initialize another switch
*
* INPUTS:
*       [In] HANDLE hDrv
*       [In] PSW_VER_INFO pSwitchInfo
*
***************************************************************************************************/
PAL_Result SwitchAdd(HANDLE hDrv,PSWITCH_INIT pSwitchInfo  );

/****************************************************************************************************
* SwitchInfoGet - retreives the initialized switches info
*
* DESCRIPTION:
*        Retreives the initialized switches info.The driver expects the array of SWITCH_INIT elements. numOfSwitches
*        is a pointer to the array size. In case of pSwitchInfo is NULL, numOfSwitches is used as an output and contains
*        the number of active switches that are included in the current driver instance.
*        
*
* INPUTS:
*       [In]  HANDLE hDrv
*       [Out] PSWITCH_INIT pSwitchInfo
*       [In/Out]  Uint32 *numOfSwitches 
*
*******************************************************************************************************/
PAL_Result SwitchInfoGet(HANDLE hDrv, PSWITCH_INIT pSwitchInfo, Uint32 *numOfSwitches  );



/***********************************************************************************************
            PORT MANAGEMENT
***********************************************************************************************/
typedef enum
{
    PORT_LINK_UP = 1, 
    PORT_LINK_DOWN, 
} PORT_LINK;                                       

/*
    Duplex status descriptor
*/

typedef enum
{
    PORT_DUPLEX_HALF=1, 
    PORT_DUPLEX_FULL
} PORT_DUPLEX; 

/*
    Speed status descriptor
*/

typedef enum
{
    PORT_SPEED_10=1, 
    PORT_SPEED_100, 
    PORT_SPEED_1000
} PORT_SPEED; 

/*
    Flow Control status descriptor
*/

typedef enum
{
    /* autorecognition of the flow control (if exists)*/
    PORT_FC_AUTO=1, 

    /* flow control on*/
    PORT_FC_ON, 

    /* flow control off*/
    PORT_FC_OFF
} PORT_FC; 

/*
    AutoNegotiation for duplex and speed status descriptor
*/

typedef enum
{
    PORT_AUTO_YES=1, 
    PORT_AUTO_NO,  
    PORT_AUTO_RESTART
} PORT_AUTO;                                       

/* Port status */
typedef enum
{
    /* Port Disabled */
    PORT_DISABLE = 1,   

    /* Special frames only (BPDU e.t.c.) */
    PORT_BLOCK,		

    /* Packets are dropped. Macs are learned */
    PORT_LEARNING,  

    /* Forwarding - Normal switch mode */
    PORT_FORWARDING 
} PORT_STATUS;                                       

/* Ingress VLAN behaviour */
typedef enum
{
    /* VLAN check is on (VLAN must exist). Ingress check is on (ingress port must be part of the VLAN). 
        If not both drop the packet
    */
    INGRESS_VLN_MODE_8021Q_SECURE,

    /* VLAN check is on (VLAN must exist). If not drop the packet*/
    INGRESS_VLN_MODE_8021Q_CHECK,
   
    /* Use Base Port VLAN only */
    INGRESS_VLN_MODE_8021Q_DISABLE

}  INGRESS_VLN_MODE;                                       



/* Extended features - unique for each switch model */
typedef struct switch_special_caps
{
	/* VLAN tunneling on/off is the ability to forward packets to the dest port according to the address table
        even if this port is not part of the VLAN (if off the packet is dropped)
    */
    
    Bool                VlanTunnel;

	INGRESS_VLN_MODE	IngressMode;

    /* The ability to add the port Id to the packet itself, thus allowing the CPU to recognize source port*/

    Bool                PortRecognitionOn;

    /* The ability to send the frame to the given port */

    Bool                TxOnFixedPort;
    
    /* ...
       ... */
}SWITCH_SPECIAL_CAPS, *PSWITCH_SPECIAL_CAPS;

/*******************************************************************************
*
* PortConfig - Passes a full configuration structure to the port
*
* INPUTS:
*     [In]    HANDLE                  hDrv,    
*     [In]    Uint32                 PortNum, 
*     [Out]   PSWITCH_SPECIAL_CAPS    PortCfg  
* 
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/
Int32 SpecCapsGet 
    (
        HANDLE                  hDrv,             /* driver handle */ 
        PSWITCH_SPECIAL_CAPS    PortCfg  /* the port configuration structure */
    ); 

/*******************************************************************************
*
* PortConfig - Retreives a full configuration structure from the port
*
*
* INPUTS:
*     [In]    HANDLE                  hDrv    
*     [In]    Uint32                 PortNum 
*     [In]    PSWITCH_SPECIAL_CAPS    PortCfg  
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/

Int32 SpecCapsSet
    (
        HANDLE                  hDrv,    /* driver handle */ 
        PSWITCH_SPECIAL_CAPS    PortCfg  /* the port configuration structure */
    ); 

/*******************************************************************************
*
* TrailerSet
*
*
* INPUTS:
*     [In]    HANDLE                 swId    
*     [In]    Bool                   TrailerMode
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/
Int32 TrailerSet
    (
        HANDLE                  swId,          /* driver handle */ 
        Bool                    TrailerMode    /* trailer mode */
    );


/*******************************************************************************
*
* PortStatusSet - enable the given port
*
* Enable/Disable a selected port
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    PORT_Int32     PortCfg  
* 
*
* The status can be: 
*  PORT_ENABLE 
*  PORT_DISABLE
*  PORT_BLOCK   
*  PORT_LEARNING
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/

Int32 PortStatusSet
    (
        HANDLE hDrv,             /* driver handle */
        Uint32 PortNum,         /* port number */
        PORT_STATUS status       /* the port status */
    ); 

/*******************************************************************************
*
* PortStatusGet - Returns the port status
*
* The API gets the port status On/Off. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
*
* RETURNS: PORT_ENABLE/PORT_DISABLE
*
*/

PORT_STATUS PortStatusGet
    (
        HANDLE     hDrv,    /* driver handle */
        Uint32    PortNum  /* logical port number */                         
    ); 

/*******************************************************************************
*
* PortLinkGet - get port link status(Up/down)
*
* The function retrieves port link status. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_LINK - link up/down 
*
*/

PORT_LINK PortLinkGet
    (
        HANDLE hDrv,    /* driver handle */
        Uint32 portNum /* logical port number */
    ); 

/****************************************************************************
* PortFCSet - set flow control (On/off)
*  
* Set flow control to on/off. all the ports in this trunk will be switched in this mode.
*
* * INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    PORT_FC         FCStatus
*  
* The FCStatus can be:
*  PORT_FC_AUTO 
*  PORT_FC_YES 
*  PORT_FC_NO
*
* RETURNS: Int32 - Ok/Error    
*
*/

Int32 PortFCSet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32     portNum, /* logical port number */
        PORT_FC FCStatus     /* flow control On/Off */
    ); 

/****************************************************************************    
* PortFCGet - get flow control status(On/off)
*
* The function retrieves Flow Control status of the given port.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_FC -  Flow control On / Flow control Off 
*
*/   

PORT_FC PortFCGet
    (    
        HANDLE hDrv,         /* driver handle */         
        Uint32 portNum /* logical port number */
    );

/****************************************************************************    
* PortDuplexSet - set port duplex (Full/Half)
*
* Set the duplex state of the port to Full/Half. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    PORT_DUPLEX duplexStatus
*
* The duplexStatus can be:
*  PORT_DUPLEX_HALF 
*  PORT_DUPLEX_FULL
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 PortDuplexSet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum,
        PORT_DUPLEX duplexStatus               
    ); 

/****************************************************************************    
* PortDuplexGet - get port duplex status (Full/Half)
*
* Retrieves Duplex status (half/full). 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: UND_PORT_DUPLEX - half/full
*
*  
*/

PORT_DUPLEX PortDuplexGet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum               
    ); 

/****************************************************************************    
* PortSpeedSet - set port speed (10/100)
*
* Set the current port to work in the given speed. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    PORT_SPEED  speedStatus
*
* The speedStatus can be:
*  PORT_SPEED_10 
*  PORT_SPEED_100 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 PortSpeedSet   
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum,
        PORT_SPEED  speedStatus
    );  

/****************************************************************************    
* PortSpeedGet - get port speed status(10/100)
*
* Retrieves current speed parameter for the given port.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_SPEED - 10/100 
*
*/

PORT_SPEED PortSpeedGet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum
    ); 


/****************************************************************************    
* PortAutoSet - set port auto (ENABLE/DISABLE/RESTART)
*
* Set autonegotiation status for the given port, which is Yes/No/Restart. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    PORT_AUTO       autoStatus
*
* autoStatus can be:
*  PORT_AUTO_YES 
*  PORT_AUTO_NO  
*  PORT_AUTO_RESTART
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 PortAutoSet    
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum,
        PORT_AUTO  autoStatus
    );

/****************************************************************************    
* PortAutoGet - get port auto status (ENABLE/DISABLE/RESTART)
*   
* Get the autonegotaiation for "speed/duplex" status, which is Yes/No. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_AUTO - YES/NO
*
*/

PORT_AUTO PortAutoGet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 portNum             
    );


/***********************************************************************************************
            S T A T I S T I C S
***********************************************************************************************/


typedef struct
{
    Uint32     ethAlignmentErrors;    
    Uint32     ethFCSErrors;          
    Uint32     ethSingleCollisions;   
    Uint32     ethMultipleCollisions; 
    Uint32     ethSQETestErrors;      
    Uint32     ethDeferredTxFrames;   
    Uint32     ethLateCollisions;     
    Uint32     ethExcessiveCollisions;
    Uint32     ethInternalMacTxErrors;
    Uint32     ethCarrierSenseErrors; 
    Uint32     ethTooLongRxFrames;    
    Uint32     ethInternalMacRxErrors;
    Uint32     ethSymbolErrors;

} SWITCH_COUNTERS, *PSWITCH_COUNTERS;

/**************************************************************************/
/*                 MIB-2 byte/packet Counter Types                        */
/**************************************************************************/
typedef struct
{
   Uint32             InBytes; 
   Uint32             InBytesHigh;
   Uint32             InUnicastPkts;
   Uint32             InUnicastPktsHigh;
   Uint32             InMulticastPkts;
   Uint32             InMulticastPktsHigh;
   Uint32             InBroadcastPkts;
   Uint32             InBroadcastPktsHigh;
   Uint32             InDiscardPkts;
   Uint32             InErrorPkts;
   Uint32             InUnknownProtPkts;
   Uint32             OutBytes;
   Uint32             OutBytesHigh;
   Uint32             OutUnicastPkts;
   Uint32             OutUnicastPktsHigh;
   Uint32             OutMulticastPkts;
   Uint32             OutMulticastPktsHigh;
   Uint32             OutBroadcastPkts;
   Uint32             OutBroadcastPktsHigh;
   Uint32             OutDiscardPkts;
   Uint32             OutErrorPkts;
   SWITCH_COUNTERS     swCounters;
} MIB2_COUNTER, *PMIB2_COUNTER;


/**************************************************************************************
*
* StatPortGet - retreives the statistic from the given port
*
* Retreives the statistic from the given port.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 StatPortGet
    (
        HANDLE hDrv,         /* driver handle */
        Uint32                 PortNum, /* logical port number*/
        PMIB2_COUNTER            counters /* output structure with all the 
                                            relevant counters for the given port  */
    );

/**************************************************************************************
*
* StatPortReset - resets port statistic for the given port.
*
* Resets the statistic on given port.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 StatPortReset
    (
        HANDLE hDrv,         /* driver handle */
        Uint32 PortNum /* logical port to reset statistic to*/
    );


/***********************************************************************************************
                                VLAN SUBMODULE
************************************************************************************************/

/* group ID type */

typedef Int32 VLN_GROUP_ID;


/* VLAN Tagging */
typedef enum 
{
    VLN_PORT_UNTAG,
    VLN_PORT_TAG
} VLN_TAG;


/**************************************************************************************
* 
* VlanCreate - Create VLAN.
*
* Create logical VLAN. No real updates are performed.\
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCreate
    (
        HANDLE hDrv,                        /* driver handle */
        Uint32              vlnId         /* VLAN Id */

    );

/**************************************************************************************
*
* VlanDelete - Destroy the existing VLAN.
*
* Deletes the VLAN
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanDelete
    (
        HANDLE hDrv,                /* driver handle */
        Uint32              vlnId          
    );

/**************************************************************************************
*
* VlanPortAdd - add port to VLAN.
*
* Adds port to the existing VLAN. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         tag
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortAdd
    (
        HANDLE hDrv,                /* driver handle */
        Uint32              vlnId,         /* VLAN Id */
        Uint32              portNum,       /* logical port number */
        VLN_TAG              tag            /* port tagged/untagged */
    );

/**************************************************************************************
*
* VlanPortDelete - remove port from VLAN.
*
* Remove port from the existing VLAN. 
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortDelete
    (
        HANDLE hDrv,                /* driver handle */
        Uint32              portNum,       /* port number */
        Uint32              vlnId         /* VLAN Id */
        
 
    );

/**************************************************************************************
*
* VlanPortUpdate - updates port status in VLAN
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         vlnId
*     [In]    VLN_TAG         tag
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortUpdate
    (
        HANDLE        hDrv,           /* driver handle */
        Uint32       PortNum,        /* logical port number */
        Uint32       vlnId,          /* logical VLAN number in which the port should be updated*/
        VLN_TAG       tag             /* new tag state */
    );
   

/**************************************************************************************
*
* VlanCPUAdd - Adds CPU to VLAN.
*
* Adds CPU to VLAN.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         vlnId 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCPUAdd
    (
        HANDLE hDrv,           /* driver handle */
        Uint32 vlnId,          /* logical VLAN number in which the port should be updated*/
        VLN_TAG tag
    );


/**************************************************************************************
*
* VlanCPUDelete  -   Deletes CPU from VLAN.
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         vlnId 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCPUDelete
    (
        HANDLE hDrv,                /* driver handle */
        Uint32       vlnId          /* logical VLAN number in which the port should be updated*/
    );


/**************************************************************************************
*
* VlanDefaultSet  -   Sets default VLAN for the given port.
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32       PortNum
*     [In]    Uint32         vlnId 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanDefaultSet
    (
        HANDLE hDrv,                /* driver handle */
        Uint32       PortNum,        /* logical port number */
        Uint32       vlnId          /* logical VLAN number in which the port should be updated*/
        
    );


/*******************************************************************************
*
* VlanUpdate - Update logical VLAN.
*
* Update of the logical VLAN.
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32       priority
*     [In]    Uint32         vlnId 
*
* RETURNS: OK OR ERROR
*
*/

Int32 VlanUpdate 
    (
        HANDLE hDrv,                /* driver handle */
        Uint32                                priority,    /* 0 ..7 */
        Uint32                                vlnId       /* VLAN Number */
    );

/*******************************************************************************
*
* VlanSetPortPriority -  Set the priority of a port
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         portNum 
*     [In]    Uint32         priority
*
* RETURNS: OK OR ERROR
*
*/

Int32 VlanSetPortPriority 
    (
        HANDLE  hDrv,       /* driver handle */
        Uint32 PortNum,    /* Port Number */
        Uint32 priority    /* switch specific */
    );
    
/*******************************************************************************
*
* VlanGetPortPriority -  Get the priority of a port
*
* INPUTS:
*     [In]    HANDLE          hDrv    
*     [In]    Uint32         portNum 
*
* RETURNS: OK OR ERROR
*
*/

Uint32 VlanGetPortPriority 
    (
        HANDLE hDrv,                /* driver handle */
        Uint32                                PortNum     /* Port Number */
    );

/* Purge an entry from the address translation Unit table. */
Int32 DeleteMacAddrFromAtu
(
 HANDLE                  swId,          /* driver handle */ 
 unsigned char           MacAdd[6]      /* Mac Address   */
 );

int SwitchResetValuesSet( HANDLE  swId);

#endif /*__SWITCH_INTERFACE_H__*/ 
  

