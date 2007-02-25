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
#include "DDC_switch.h"
#include "CSL_switchPrvt.h"

#ifdef LINUX_COMPILATION
#include "linuxpal.h"
#else
#ifdef VX_COMPILATION
#include "vxWorkspal.h"
#endif
#endif

/* global parameters */
/* Maximum number of drivers that are running concurrent*/

static PSWITCH_DRV_HANDLE swDrv = PAL_NULL;

/* Find switch according to the instance Id*/

__inline PCSL_SWITCH_DRV_HANDLE getSwitchFromList(HANDLE id) 
{ 
    PCSL_SWITCH_DRV_HANDLE handle = PAL_NULL;

    if(swDrv)
        for(handle = swDrv->switchesList;(handle != PAL_NULL) && (handle->initParams.instanceId != id);handle = handle->next); 
    
    return handle;
}

/*******************************************************************************
* SwitchDrvCreate()    
*
* DESCRIPTION:
*           Creates instance of the switch driver. The driver is multiopenable. 
*           After the call the driver is inactive state.
*
* INPUTS:
*      [IN] PSWITCH_INIT   - switchInit
*
* RETURNS: 
*       handle to the switch driver.
*
*******************************************************************************/
HANDLE SwitchDrvCreate(void)
{
   
   /* This is no error. Totally correct situation. The driver can be created twice (or more). */
   
   if(!swDrv)
   {
      swDrv = (PSWITCH_DRV_HANDLE) PAL_MALLOC(sizeof(SWITCH_DRV_HANDLE));
      if(!swDrv)
         return PAL_NULL;
      swDrv->numberOfSwitches = 0;
      swDrv->switchesList = PAL_NULL;
   }
   
   return (HANDLE) swDrv;
}


/*********************************************************************************************************
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
***********************************************************************************************************/
Int32 SwitchDrvDelete( HANDLE drvHandle )
{
    PCSL_SWITCH_DRV_HANDLE handle = PAL_NULL;
    PCSL_SWITCH_DRV_HANDLE tmp = PAL_NULL;
    
   /* Is it a good handle?*/

    PAL_RESULT_ASSERT((HANDLE)swDrv != drvHandle);
    
    /* check for the space for new switch info*/
    
    for(handle = swDrv->switchesList;handle != PAL_NULL;handle = tmp)
    {
        cslSwitchRmv(handle);
        tmp = handle->next;
        PAL_FREE(handle);
    }
   
    PAL_FREE(swDrv);
    
    return PAL_True;
}

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
PAL_Result SwitchAdd(HANDLE hDrv,PSWITCH_INIT pSwitchInfo  )
{
    PCSL_SWITCH_DRV_HANDLE  swHead;
    /* Is it a good handle */

    PAL_RESULT_ASSERT((HANDLE)swDrv == hDrv);

    swHead = swDrv->switchesList;

    swDrv->switchesList = (PCSL_SWITCH_DRV_HANDLE)PAL_MALLOC(sizeof( CSL_SWITCH_DRV_HANDLE));
    if(swDrv->switchesList == NULL)
       return PAL_False;

    swDrv->switchesList->initParams.instanceId = pSwitchInfo->instanceId;
    swDrv->switchesList->initParams.channelNumber = pSwitchInfo->channelNumber;
    swDrv->switchesList->initParams.internalPortNum = pSwitchInfo->internalPortNum;
    swDrv->switchesList->initParams.miiBase = pSwitchInfo->miiBase;
    swDrv->switchesList->initParams.switchType = pSwitchInfo->switchType;
    if(cslConnectSwitch(swDrv->switchesList) == PAL_False)
       return PAL_False;

    /* check for mandatory functionality */
    if(
       (swDrv->switchesList->cslSwitchClbks.cslOperStatus2regVal == PAL_NULL) || 
       (swDrv->switchesList->cslSwitchClbks.cslPortNum2PhyAddr   == PAL_NULL) ||
       (swDrv->switchesList->cslSwitchClbks.cslPortNum2RegAddr   == PAL_NULL) ||
       (swDrv->switchesList->cslSwitchClbks.cslRegVal2operStatus == PAL_NULL) ||
       (swDrv->switchesList->cslSwitchClbks.cslSwitchGetVersion  == PAL_NULL) 
      )
    {
        PAL_FREE(swDrv->switchesList);
        swDrv->switchesList = swHead;
        return PAL_False;
    }

    swDrv->switchesList->next = swHead;

    swDrv->numberOfSwitches++;
    

    return PAL_True;
}


/*******************************************************************************************************
* SwitchDrvStart()
*
* DESCRIPTION:
*        Start the switch. In our case the hardware switch initialization is performed here.
*
* INPUTS:
*       [In] hDrv -  driver handle
*       [In] swId - switch Id
*
* RETURNS: 
*       Int32         - Ok/Error
*
*********************************************************************************************************/
Int32 SwitchDrvStart( HANDLE drvHandle,HANDLE  swId)
{
    PCSL_SWITCH_DRV_HANDLE swHead;

   /* Is it a good handle?*/

    PAL_RESULT_ASSERT((HANDLE)swDrv == drvHandle);
    PAL_RESULT_ASSERT(swDrv != PAL_NULL);

    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    if(swHead->cslSwitchClbks.cslSwitchStart != PAL_NULL)
        swHead->cslSwitchClbks.cslSwitchStart(swHead);

    return PAL_True;
}

/**********************************************************************************************************
* SwitchDrvStop
*
* DESCRIPTION:
*       Stops the driver. Move the driver into inactive state. For now not implemented.
*
* INPUTS:
*       [In] hDrv -  driver handle
*       [In] swId - switch Id
*
* RETURNS: 
*       Int32         - Ok/Error
*
*************************************************************************************************************/
Int32 SwitchDrvStop( HANDLE hDrv,HANDLE  swId)
{

    PCSL_SWITCH_DRV_HANDLE swHead;

   /* Is it a good handle?*/

    PAL_RESULT_ASSERT((HANDLE)swDrv != hDrv);
    PAL_RESULT_ASSERT(swDrv != PAL_NULL);

    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    if(swHead->cslSwitchClbks.cslSwitchStop != PAL_NULL)
        swHead->cslSwitchClbks.cslSwitchStop(swHead);
    
    return PAL_True;
}

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
PAL_Result SwitchInfoGet(HANDLE hDrv, PSWITCH_INIT pSwitchInfo, Uint32 *numOfSwitches  )
{
    PCSL_SWITCH_DRV_HANDLE handle;
    Int32 idx = 0;

    /* Is it a good handle */

    PAL_RESULT_ASSERT((HANDLE)swDrv == hDrv);

    if(pSwitchInfo == PAL_NULL)
    {
        *numOfSwitches = swDrv->numberOfSwitches;
        return PAL_True;
    }

    /* retreive the switches info from the driver*/

    for(
        handle = swDrv->switchesList,idx = 0;
         (handle != PAL_NULL) && (idx < (*numOfSwitches)) ;
         handle = handle->next,idx++
       )
    {
        pSwitchInfo[idx].channelNumber = swDrv->switchesList->initParams.channelNumber;
        pSwitchInfo[idx].instanceId = swDrv->switchesList->initParams.instanceId;
        pSwitchInfo[idx].internalPortNum = swDrv->switchesList->initParams.internalPortNum;
        pSwitchInfo[idx].miiBase = swDrv->switchesList->initParams.miiBase;
        pSwitchInfo[idx].switchType = swDrv->switchesList->initParams.switchType;
    }

    /* write the real number of switches that was retreived */
    *numOfSwitches = idx;
    return PAL_True;
}



/*******************************************************************************
* SwitchGetVersion
*
* DESCRIPTION:
*       This function returns the version of the switch.
*
* INPUTS:
*       [In] HANDLE swId
*       [Out] PSW_VER_INFO pSwitchInfo
*
*******************************************************************************/
PAL_Result SwitchGetVersion(HANDLE swId, PSW_VER_INFO pSwitchInfo  )
{
    PCSL_SWITCH_DRV_HANDLE swHead;

    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    swHead->cslSwitchClbks.cslSwitchGetVersion(swHead,pSwitchInfo);
    return PAL_True;
}

/*********************************************************************************************************
*
* PortStatusSet - enable the given port
*
* Enable/Disable a selected port
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*     [In]    PORT_STATUS     PortCfg  
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
        HANDLE swId,             /* switch handle */
        Uint32 PortNum,         /* port number */
        PORT_STATUS status       /* the port status */
    )
{
    
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    Uint32 regaddr,phyaddr;

    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */
    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_STATUS,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_STATUS,PortNum);
    
    /* here is read modify write sequence */
    
    phyRegRead(swHead,regaddr,phyaddr,data);

    /* data - In, data - Out  */
    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_STATUS,(Int32) status,data);

    phyRegWrite(swHead,regaddr,phyaddr,data);

    return PAL_True;
    
}

/*********************************************************************************************************
*
* PortStatusGet - Returns the port status
*
* The API gets the port status On/Off. 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
*
* RETURNS: PORT_ENABLE/PORT_DISABLE
*
*/

PORT_STATUS PortStatusGet
    (
        HANDLE     swId,    /* switch handle */
        Uint32    PortNum  /* logical port number */                         
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_STATUS portStatus;
    Uint32 regaddr,phyaddr;

    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */
    
    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_STATUS,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_STATUS,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    portStatus = (PORT_STATUS) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_STATUS,data);
    
    return portStatus;

}

/***********************************************************************************************************
*
* PortLinkGet - get port link status(Up/down)
*
* The function retrieves port link status. 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_LINK - link up/down 
*
*/

PORT_LINK PortLinkGet
    (
        HANDLE swId,    /* switch handle */
        Uint32 PortNum /* logical port number */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_LINK portLink;
    Uint32 regaddr,phyaddr;
    
        /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */
    
    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_LINK,PortNum);
    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_LINK,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    portLink = (PORT_LINK) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_LINK, data);
    
    return portLink;
}

/***********************************************************************************************************
* PortFCSet - set flow control (On/off)
*  
* Set flow control to on/off. all the ports in this trunk will be switched in this mode.
*
* * INPUTS:
*     [In]    HANDLE          swId    
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
        HANDLE swId,         /* switch handle */
        Uint32 PortNum, /* logical port number */
        PORT_FC FCStatus     /* flow control On/Off */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    Uint32 regaddr,phyaddr;

        /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */
    
    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_FC,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_FC,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_FC,(PORT_FC) FCStatus,data);

    phyRegWrite(swHead,regaddr,phyaddr,data);
    
    return PAL_True;
}

/*********************************************************************************************************    
* PortFCGet - get flow control status(On/off)
*
* The function retrieves Flow Control status of the given port.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_FC -  Flow control On / Flow control Off 
*
*/   

PORT_FC PortFCGet
    (    
        HANDLE swId,         /* switch handle */         
        Uint32 PortNum /* logical port number */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_FC portFC;
    Uint32 regaddr,phyaddr;
    
        /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_FC,PortNum);
    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_FC,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    portFC = (PORT_FC) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_FC,data);
    
    return portFC;

}

/*************************************************************************************************************    
* PortDuplexSet - set port duplex (Full/Half)
*
* Set the duplex state of the port to Full/Half. 
*
* INPUTS:
*     [In]    HANDLE          swId    
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
        HANDLE swId,         /* switch handle */
        Uint32 PortNum,
        PORT_DUPLEX duplexStatus               
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    Uint32 regaddr,phyaddr;

    /* Find the relevant switch head*/
 
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_DUPLEX,PortNum);
    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_DUPLEX_SET,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);

    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_DUPLEX_SET,(Int32) duplexStatus,data);

    phyRegWrite(swHead,regaddr,phyaddr,data);

    return PAL_True;

}

/*********************************************************************************************************    
* PortDuplexGet - get port duplex status (Full/Half)
*
* Retrieves Duplex status (half/full). 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: UND_PORT_DUPLEX - half/full
*
*  
*/

PORT_DUPLEX PortDuplexGet
    (
        HANDLE swId,         /* switch handle */
        Uint32 PortNum               
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_DUPLEX portDuplex;
    Uint32 regaddr,phyaddr;
    
    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_DUPLEX,PortNum);
    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_DUPLEX,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    portDuplex = (PORT_DUPLEX) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_DUPLEX,data);
    
    return portDuplex;

}

/***********************************************************************************************************    
* PortSpeedSet - set port speed (10/100)
*
* Set the current port to work in the given speed. 
*
* INPUTS:
*     [In]    HANDLE          swId    
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
        HANDLE swId,         /* switch handle */
        Uint32 PortNum,
        PORT_SPEED  speedStatus
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    Uint32 regaddr,phyaddr;
    
    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_SPEED,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_SPEED_SET,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_SPEED_SET,(Int32) speedStatus,data);
    
    phyRegWrite(swHead,regaddr,phyaddr,data);
    
    return PAL_True;

}

/*********************************************************************************************************    
* PortSpeedGet - get port speed status(10/100)
*
* Retrieves current speed parameter for the given port.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_SPEED - 10/100 
*
*/

PORT_SPEED PortSpeedGet
    (
        HANDLE swId,         /* switch handle */
        Uint32 PortNum
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_SPEED portSpeed;
    Uint32 regaddr,phyaddr;
    
     /* Find the relevant switch head*/
   
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_SPEED,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_SPEED,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    portSpeed = (Int32) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_SPEED,data);
    
    return portSpeed;
}


/************************************************************************************************************    
* PortAutoSet - set port auto (ENABLE/DISABLE/RESTART)
*
* Set autonegotiation status for the given port, which is Yes/No/Restart. 
*
* INPUTS:
*     [In]    HANDLE          swId    
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
        HANDLE swId,         /* switch handle */
        Uint32 PortNum,
        PORT_AUTO  autoStatus
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    Uint32 regaddr,phyaddr;

    /* Find the relevant switch head*/
   
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */
    
    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_AUTO,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_AUTO,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_AUTO,(Int32) autoStatus,data);
    
    phyRegWrite(swHead,regaddr,phyaddr,data);
    
    return PAL_True;

}

/**********************************************************************************************************    
* PortAutoGet - get port auto status (ENABLE/DISABLE/RESTART)
*   
* Get the autonegotaiation for "speed/duplex" status, which is Yes/No. 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: PORT_AUTO - YES/NO
*
*/

PORT_AUTO PortAutoGet
    (
        HANDLE swId,         /* switch handle */
        Uint32 PortNum             
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data;
    PORT_AUTO portAuto;
    Uint32 regaddr,phyaddr;

        /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_AUTO,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_AUTO,PortNum);
    
    phyRegRead(swHead,regaddr,phyaddr,data);
    
    portAuto = (PORT_AUTO) swHead->cslSwitchClbks.cslRegVal2operStatus(OPER_AUTO,data);
    
    return portAuto;
    
}


/**********************************************************************************************************
            S T A T I S T I C S
***********************************************************************************************************/

/**********************************************************************************************************
*
* StatPortGet - retreives all the statistic from the given port
*
* Retreives the statistic from the given port.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 StatPortGet
    (
        HANDLE         swId,    /* switch handle */
        Uint32         PortNum, /* logical port number*/
        PMIB2_COUNTER  counters /* output structure with all the 
                                            relevant counters for the given port  */
    )
{
#define OFFSET_FROM_STRUCT(x)  x * (sizeof(Uint32))
    
    SWITCH_STAT_E  statIndex;
    PCSL_SWITCH_DRV_HANDLE swHead;
    
    /* Find the relevant switch head*/
    
    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    /* calculate the target address */

    if((swHead->cslSwitchClbks.cslPrepareReadStatSequence == PAL_NULL) || (swHead->cslSwitchClbks.cslGetPreparedCounters == PAL_NULL))
        return PAL_False;
    
    swHead->cslSwitchClbks.cslPrepareReadStatSequence(swHead,PortNum);

    for(statIndex = InBytes;statIndex <= ethSymbolErrors;statIndex++)
    {
        swHead->cslSwitchClbks.cslGetPreparedCounters(swHead,statIndex,(Uint32*)((Uint32)counters + OFFSET_FROM_STRUCT(statIndex)));
    }

    return PAL_True;
}

/**********************************************************************************************************
*
* StatPortReset - resets port statistic for the given port.
*
* Resets the statistic on given port.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 StatPortReset
    (
        HANDLE swId,         /* switch handle */
        Uint32 PortNum /* logical port to reset statistic to*/
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    Uint16 data = 0;
    Uint32 regaddr,phyaddr;

    swHead = getSwitchFromList(swId);

    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);

    regaddr = swHead->cslSwitchClbks.cslPortNum2RegAddr(OPER_RESET_STAT,PortNum);

    phyaddr = swHead->cslSwitchClbks.cslPortNum2PhyAddr(OPER_RESET_STAT,PortNum);
    
    data = swHead->cslSwitchClbks.cslOperStatus2regVal(OPER_RESET_STAT,PortNum,data);

    phyRegWrite(swHead,regaddr,phyaddr,data);

    return PAL_True;
}


/*************************************************************************************************************
                                VLAN SUBMODULE
**************************************************************************************************************/

/*************************************************************************************************************
* 
* VlanCreate - Create VLAN.
*
* Create logical VLAN. No real updates are performed.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCreate
    (
        HANDLE swId,                        /* switch handle */
        Uint32              vlnId         /* VLAN Id */

    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;

    PAL_RESULT_ASSERT(vlnId < 0xFFF);
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
        return PAL_False;

    /* non-critical error VLAN is already exists*/

    elem = cslGetVLANEntry(swHead,vlnId);

    PAL_RESULT_ASSERT(elem == PAL_NULL);

    elem = cslCreateVLANEntry(swHead);

    if(elem == NULL)
       return PAL_False;
    
    elem->valid = 1;
    elem->vlanId = vlnId;

    return PAL_True;
}

/**************************************************************************************************************
*
* VlanDelete - Destroy the existing VLAN.
*
* Deletes the VLAN from VLAN table.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanDelete
    (
        HANDLE swId,                /* switch handle */
        Uint32              vlnId          
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;
    
    /* non-critical error VLAN doesn't exist*/

    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);

    elem->valid = 0;
    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);
    
    cslDestroyVLANEntry(swHead, elem);
    return PAL_True;
}

/*************************************************************************************************************
*
* VlanPortAdd - add port to VLAN.
*
* Adds port to the existing VLAN. 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         tag
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortAdd
    (
        HANDLE swId,                /* switch handle */
        Uint32              vlnId,         /* VLAN Id */
        Uint32              portNum,       /* logical port number */
        VLN_TAG              tag            /* port tagged/untagged */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(portNum < swHead->numberOfPorts);
    
    /* non-critical error VLAN doesn't exist*/
    
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;

    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);

    elem->portMask |= (1 << portNum);
    if (tag == VLN_PORT_UNTAG)
        elem->portTagMask &= (~(1 << portNum)); 
    
    else 
        elem->portTagMask |= (1 << portNum); 

    
    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);

    return PAL_True;
}

/*******************************************************************************************************
*
* VlanPortDelete - remove port from VLAN.
*
* Remove port from the existing VLAN. 
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         vlnId
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortDelete
    (
        HANDLE swId,                /* switch handle */
        Uint32              portNum,       /* port number */
        Uint32              vlnId         /* VLAN Id */
        
 
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(portNum < swHead->numberOfPorts);
    
    /* non-critical error VLAN doesn't exist*/
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;
    
    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);
    
    elem->portMask &= (~(1 << portNum));

    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);
    
    return PAL_True;
}

/**********************************************************************************************************
*
* VlanPortUpdate - updates port status in VLAN
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         PortNum 
*     [In]    Uint32         vlnId
*     [In]    VLN_TAG         tag
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanPortUpdate
    (
        HANDLE       swId,           /* switch handle */
        Uint32       portNum,        /* logical port number */
        Uint32       vlnId,          /* logical VLAN number in which the port should be updated*/
        VLN_TAG      tag             /* new tag state */
    )
{

    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(portNum < swHead->numberOfPorts);
    
    /* non-critical error VLAN doesn't exist*/
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;
    
    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);
    
    /* If this port is not a part of this VLAN return error */
    if(!(elem->portMask & (1 << portNum )))
       return PAL_False;
   /* elem->portMask |= (1 << portNum);*/
    if (tag == VLN_PORT_UNTAG)
        elem->portTagMask &= (~(1 << portNum)); 
    
    else 
        elem->portTagMask |= (1 << portNum); 

    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);

    return PAL_True;
}
   

/********************************************************************************************************
*
* VlanCPUAdd - Adds CPU to VLAN.
*
* Adds CPU to VLAN.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         vlnId 
*     [In]    Uint32         tag
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCPUAdd
    (
        HANDLE swId,            /* switch handle */
        Uint32 vlnId,            /* logical VLAN number in which the port should be updated */
        VLN_TAG tag
    )
{

    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;
    
    /* non-critical error VLAN doesn't exist*/
    
    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);
    
    elem->portMask |= (1 << swHead->initParams.internalPortNum);

    if (tag == VLN_PORT_UNTAG)
        elem->portTagMask &= (~(1 << swHead->initParams.internalPortNum)); 

    else 
        elem->portTagMask |= (1 << swHead->initParams.internalPortNum); 

    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);

    return PAL_True;
}


/**************************************************************************************
*
* VlanCPUDelete  -   Deletes CPU from VLAN.
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         vlnId 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanCPUDelete
    (
        HANDLE swId,                /* switch handle */
        Uint32       vlnId          /* logical VLAN number in which the port should be updated */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    if(swHead->cslSwitchClbks.cslVlnUpdate == PAL_NULL)
       return PAL_False;
    
    /* non-critical error VLAN doesn't exist */
    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);
    
    elem->portMask &= (~(1 << swHead->initParams.internalPortNum));

    swHead->cslSwitchClbks.cslVlnUpdate(swHead,elem);

    return PAL_True;
}


/**************************************************************************************
*
* VlanDefaultSet  -   Sets default VLAN for the given port.
*
* Updates port to tagged/untagged in VLAN.
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32       PortNum
*     [In]    Uint32         vlnId 
*
* RETURNS: Int32 - Ok/Error
*
*/

Int32 VlanDefaultSet
    (
        HANDLE       swId,          /* driver handle */
        Uint32       portNum,       /* logical port number */
        Uint32       vlnId          /* logical VLAN number in which the port should be updated*/
        
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    PCSL_SWITCH_VLAN_TABLE elem;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(portNum < swHead->numberOfPorts);
    
    /* non-critical error VLAN doesn't exist*/
    
    elem = cslGetVLANEntry(swHead,vlnId);
    
    PAL_RESULT_ASSERT(elem != PAL_NULL);

    if(swHead->cslSwitchClbks.cslVlnDefaultSet != PAL_NULL)
        swHead->cslSwitchClbks.cslVlnDefaultSet(swHead,portNum,elem);

    return PAL_True;
}


/*******************************************************************************
*
* VlanSetPortPriority -  Set the priority of a port
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         portNum 
*     [In]    Uint32         priority
*
* RETURNS: PAL_True OR PAL_False
*
*/

Int32 VlanSetPortPriority 
    (
        HANDLE swId,       /* switch handle */
        Uint32 PortNum,    /* Port Number */
        Uint32 priority    /* switch specific */
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
        
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);
    
    if(swHead->cslSwitchClbks.cslVlnPrioritySet != PAL_NULL)
        swHead->cslSwitchClbks.cslVlnPrioritySet(swHead,PortNum,priority);
        
    return PAL_True;
}
    
/*******************************************************************************
*
* VlanGetPortPriority -  Get the priority of a port
*
* INPUTS:
*     [In]    HANDLE          swId    
*     [In]    Uint32         portNum 
*
* RETURNS: PAL_True OR PAL_False
*
*/

Uint32 VlanGetPortPriority 
    (
        HANDLE swId,                /* switch handle */
        Uint32 PortNum              /* Port Number */
    )
{

    PCSL_SWITCH_DRV_HANDLE swHead;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);
    PAL_RESULT_ASSERT(PortNum < swHead->numberOfPorts);
    
    if(swHead->cslSwitchClbks.cslVlnPriorityGet != PAL_NULL)
        return swHead->cslSwitchClbks.cslVlnPriorityGet(swHead,PortNum);

    return PAL_True;
}



/*******************************************************************************
*
* SpecCapsGet - gets special capabilities configuration of the switch
*
* INPUTS:
*     [In]    HANDLE                 swId,    
*     [Out]   PSWITCH_SPECIAL_CAPS   pSwitchSpecialCaps  
* 
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/

PAL_Result SpecCapsGet
    (
        HANDLE                  swId,             /* driver handle */ 
        PSWITCH_SPECIAL_CAPS    pSwitchSpecialCaps
            
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
    
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    pSwitchSpecialCaps->IngressMode         = swHead->specCaps.IngressMode; 
    pSwitchSpecialCaps->PortRecognitionOn   = swHead->specCaps.PortRecognitionOn;
    pSwitchSpecialCaps->TxOnFixedPort       = swHead->specCaps.TxOnFixedPort;
    pSwitchSpecialCaps->VlanTunnel          = swHead->specCaps.VlanTunnel;

    return PAL_True;
}

/*******************************************************************************
*
* SpecCapsSet - sets special capabilities configuration of the switch
*
*
* INPUTS:
*     [In]    HANDLE                 swId    
*     [In]    PSWITCH_SPECIAL_CAPS   pSwitchSpecialCaps  
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/

Int32 SpecCapsSet
    (
        HANDLE                  swId,    /* driver handle */ 
        PSWITCH_SPECIAL_CAPS    pSwitchSpecialCaps  /* the port configuration structure */
    )
{

    PCSL_SWITCH_DRV_HANDLE swHead;
        
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    if(pSwitchSpecialCaps->IngressMode != INGRESS_VLN_MODE_8021Q_DISABLE)
       swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_INGRESS,pSwitchSpecialCaps->IngressMode);
    else
       swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_INGRESS_OFF,pSwitchSpecialCaps->IngressMode);

    swHead->specCaps.IngressMode = pSwitchSpecialCaps->IngressMode;
    swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_PORT_RECOGNITION_ON,pSwitchSpecialCaps->PortRecognitionOn);
    swHead->specCaps.PortRecognitionOn = pSwitchSpecialCaps->PortRecognitionOn;
    swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_TRANSMIT_ON_FIXED_PORT,pSwitchSpecialCaps->TxOnFixedPort);
    swHead->specCaps.TxOnFixedPort = pSwitchSpecialCaps->TxOnFixedPort;
    swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_VLANTUNNEL,pSwitchSpecialCaps->VlanTunnel);
    swHead->specCaps.VlanTunnel = pSwitchSpecialCaps->VlanTunnel;

    return PAL_True;
}

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
    )
{
    PCSL_SWITCH_DRV_HANDLE swHead;
        
    swHead = getSwitchFromList(swId);
    
    PAL_RESULT_ASSERT(swHead != PAL_NULL);

    swHead->cslSwitchClbks.cslVlnCapsSet(swHead, (Uint32)OPER_PORT_RECOGNITION_ON,TrailerMode);
    swHead->specCaps.PortRecognitionOn = TrailerMode;
    return PAL_True;
}

/*******************************************************************************
*
* DeleteMacAddrFromAtu - Delete MAC address from the Address table
*
*
* INPUTS:
*     [In]    HANDLE                 swId    
*     [In]    char[6]                MAC address
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/
Int32 DeleteMacAddrFromAtu
(
 HANDLE                  swId,          /* driver handle */ 
 unsigned char           MacAdd[6]      /* Mac Address   */
 )
{
    PCSL_SWITCH_DRV_HANDLE swHead;

   swHead = getSwitchFromList(swId);
   PAL_RESULT_ASSERT(swHead != PAL_NULL);
   
   if(swHead->cslSwitchClbks.cslDeleteMacAddEntry != PAL_NULL)
      swHead->cslSwitchClbks.cslDeleteMacAddEntry(swHead,MacAdd);
   
   return PAL_True;
   
}

/*******************************************************************************
*
* SwitchSet2Default - Disable VLAN and enable autonegotiation for all port.
*                     Call this function on software reboot.
*
* INPUTS:
*     [In]    HANDLE                 swId    
*
* RETURNS: 
*       Int32 - Ok/Error
*
*
*/
int SwitchSet2Default(HANDLE  swId)
{
   SWITCH_SPECIAL_CAPS    switchSpecialCaps;
   int i;
   PCSL_SWITCH_DRV_HANDLE swHead;
   
   swHead = getSwitchFromList(swId);
   PAL_RESULT_ASSERT(swHead != PAL_NULL);
   
   switchSpecialCaps.VlanTunnel = PAL_False;
   switchSpecialCaps.PortRecognitionOn = PAL_False;
   switchSpecialCaps.TxOnFixedPort = PAL_False;
   switchSpecialCaps.IngressMode = INGRESS_VLN_MODE_8021Q_DISABLE;
   SpecCapsSet(0,&switchSpecialCaps);
   
   for (i=0 ; i< swHead->numberOfPorts; i++)
      PortAutoSet(swId,i,PORT_AUTO_YES);
   
   return PAL_True;
}


/* Sample appplications */
#if 0
void switchSampleStart(void)
{
    HANDLE hDrv;
    SWITCH_INIT switchInfo;

    hDrv = SwitchDrvCreate();

    switchInfo.channelNumber = 0;
    switchInfo.instanceId = 0;
    switchInfo.internalPortNum = 5;
    switchInfo.miiBase = 0xA8611E00;
    switchInfo.switchType = SWITCH_MARVELL_6063;

    SwitchAdd(hDrv,&switchInfo);
    SwitchDrvStart(hDrv,0);


#if 0
    switchInfo.channelNumber = 1;
    switchInfo.instanceId = 1;
    switchInfo.internalPortNum = 6;
    switchInfo.miiBase = 0;
    switchInfo.switchType = SWITCH_MARVELL_6060;

    SwitchAdd(hDrv,&switchInfo);
#endif
}

void PrintVlanMode(void)
{
    SWITCH_SPECIAL_CAPS    switchSpecialCaps;

    SpecCapsGet(0,&switchSpecialCaps);
    
    PAL_PRINT("switchSpecialCaps.VlanTunnel = %d switchSpecialCaps.IngressMode = %d \r\n",switchSpecialCaps.VlanTunnel,switchSpecialCaps.IngressMode);
}

void VlnModeOn(void)
{
    SWITCH_SPECIAL_CAPS    switchSpecialCaps;

    switchSpecialCaps.VlanTunnel = PAL_True;
    switchSpecialCaps.PortRecognitionOn = PAL_False;
    switchSpecialCaps.TxOnFixedPort = PAL_False;
    switchSpecialCaps.IngressMode = INGRESS_VLN_MODE_8021Q_CHECK;
    SpecCapsSet(0,&switchSpecialCaps);

}

void VlnModeOff(void)
{
    SWITCH_SPECIAL_CAPS    switchSpecialCaps;

    switchSpecialCaps.VlanTunnel = PAL_False;
    switchSpecialCaps.PortRecognitionOn = PAL_False;
    switchSpecialCaps.TxOnFixedPort = PAL_False;
    switchSpecialCaps.IngressMode = INGRESS_VLN_MODE_8021Q_DISABLE;
    SpecCapsSet(0,&switchSpecialCaps);

}


void PrintAllStats(        HANDLE         hDrv,    /* driver handle */
        Uint32         PortNum /* logical port number*/
)
{
    MIB2_COUNTER mibCnt;
    PAL_Result retVal;
    SWITCH_STAT_E  statIndex;
    retVal = StatPortGet(hDrv,PortNum,&mibCnt);
    
    for (statIndex = InBytes;statIndex <= ethSymbolErrors;statIndex++)
    {
        PAL_PRINT("index = %d value = %d \r\n",statIndex,*((Uint32*)((Uint32)(&mibCnt) + OFFSET_FROM_STRUCT(statIndex))));
    }
}



void DumpAllRegs(void)
{
    Uint32 regaddr,phyaddr,data;

    PAL_PRINT("Switch port registers \r\n");

    for (phyaddr = 0x18;phyaddr <0x1f;phyaddr++)
    {
        for (regaddr = 0; regaddr < 0x20;regaddr++)
        {
            data = mdioRead(0xA8611E00,0, regaddr, phyaddr);
            PAL_PRINT("phyaddr = 0x%x regaddr = 0x%x value = 0x%x \r\n",phyaddr,regaddr,data);
        }
    }

    PAL_PRINT("Switch global registers \r\n");

    for (regaddr = 0; regaddr < 0x20;regaddr++)
    {
        data = mdioRead(0xA8611E00,0, regaddr, phyaddr);
        PAL_PRINT("regaddr = 0x%x value = 0x%x \r\n",regaddr,data);
    }

    PAL_PRINT("Switch phy registers \r\n");

    for (phyaddr = 0x10;phyaddr <0x15;phyaddr++)
    {
        for (regaddr = 0; regaddr < 0x20;regaddr++)
        {
            data = mdioRead(0xA8611E00,0, regaddr, phyaddr);
            PAL_PRINT("phyaddr = 0x%x regaddr = 0x%x value = 0x%x \r\n",phyaddr,regaddr,data);
        }
    }
}
#endif