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
#include "pal_defs.h"

#ifdef LINUX_COMPILATION
#include "linuxpal.h"
#else
#ifdef VX_COMPILATION
#include "vxWorkspal.h"
#endif
#endif

#include "DDC_switch.h"
#include "CSL_switchPrvt.h"

#ifdef CONFIG_MIPS_AVALANCHE_KENDIN
#include "CSL_kendin8995x.h"
#endif
#ifdef CONFIG_MIPS_AVALANCHE_MARVELL   
#include "CSL_marvell6063.h"
#endif


static int VlanCounter=0;
/*******************************************************************************
* cslConnectSwitch - switch initialization
*
*  This API does the callback structure initialization. Here the type of the switch is considered.
*
* INPUTS:
*       [In/Out] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

Int32 cslConnectSwitch(PCSL_SWITCH_DRV_HANDLE swHead)
{
   
   swHead->pVlanTable = (PCSL_SWITCH_VLAN_TABLE) PAL_MALLOC(sizeof(CSL_SWITCH_VLAN_TABLE));
   if(swHead->pVlanTable == NULL)
      return PAL_False;

   swHead->pVlanTable->next = PAL_NULL;
   swHead->pVlanTable->portMask = 0;
   swHead->pVlanTable->portTagMask = 0;
   swHead->pVlanTable->valid = 0;
   swHead->pVlanTable->vlanId = 0;
   
   switch (swHead->initParams.switchType)
   {
      
#ifdef CONFIG_MIPS_AVALANCHE_MARVELL 
   case SWITCH_MARVELL_6063:
      {
         swHead->cslSwitchClbks.cslGetPreparedCounters      = cslMarvell6063GetPreparedCounters;
         
         swHead->cslSwitchClbks.cslRegVal2operStatus        = cslMarvell6063RegVal2OperStatus;
         
         swHead->cslSwitchClbks.cslOperStatus2regVal        = cslMarvell6063OperStatus2RegVal;
         
         swHead->cslSwitchClbks.cslSwitchGetVersion         = cslMarvell6063SwitchGetVersion;
         
         swHead->cslSwitchClbks.cslPrepareReadStatSequence  = cslMarvell6063PrepareReadStatSequence;
         
         swHead->cslSwitchClbks.cslPortNum2RegAddr          = cslMarvell6063PortNum2RegAddr;
         
         swHead->cslSwitchClbks.cslPortNum2PhyAddr          = cslMarvell6063PortNum2PHYAddr;
         
         swHead->cslSwitchClbks.cslSwitchStart              = cslMarvell6063Start;
         
         swHead->cslSwitchClbks.cslVlnCapsSet               = cslMarvell6063SpecialCapsSet; 
         
         swHead->cslSwitchClbks.cslVlnDefaultSet            = cslMarvell6063VlanDefaultSet;
         
         swHead->cslSwitchClbks.cslVlnUpdate                = cslMarvell6063VlanUpdate;
         
         swHead->cslSwitchClbks.cslVlnPriorityGet           = cslMarvell6063PriorityGet;
         
         swHead->cslSwitchClbks.cslVlnPrioritySet           = cslMarvell6063PrioritySet;
         
         swHead->cslSwitchClbks.cslDeleteMacAddEntry           = cslMarvell6063PurgeAtuEntry;
         
         return PAL_True;
      }

   case SWITCH_MARVELL_6060:
      {
         
         swHead->cslSwitchClbks.cslGetPreparedCounters      = PAL_NULL;
         
         swHead->cslSwitchClbks.cslRegVal2operStatus        = cslMarvell6063RegVal2OperStatus;
         
         swHead->cslSwitchClbks.cslOperStatus2regVal        = cslMarvell6063OperStatus2RegVal;
         
         swHead->cslSwitchClbks.cslSwitchGetVersion         = cslMarvell6060SwitchGetVersion;
         
         swHead->cslSwitchClbks.cslPrepareReadStatSequence  = PAL_NULL;
         
         swHead->cslSwitchClbks.cslPortNum2RegAddr          = cslMarvell6063PortNum2RegAddr;
         
         swHead->cslSwitchClbks.cslPortNum2PhyAddr          = cslMarvell6063PortNum2PHYAddr;
         
         swHead->cslSwitchClbks.cslSwitchStart              = cslMarvell6060Start;
         
         swHead->cslSwitchClbks.cslVlnCapsSet               = cslMarvell6060SpecialCapsSet; 
         
         swHead->cslSwitchClbks.cslVlnDefaultSet            = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnUpdate                = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnPriorityGet           = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnPrioritySet           = PAL_NULL;
         
         swHead->cslSwitchClbks.cslDeleteMacAddEntry        = cslMarvell6063PurgeAtuEntry;
         
         return PAL_True;
      }
#endif
#if defined CONFIG_MIPS_AVALANCHE_KENDIN
   case SWITCH_KENDIN_8995X:
      {
         swHead->cslSwitchClbks.cslGetPreparedCounters      = cslKendin8995xGetPreparedCounters;
         
         swHead->cslSwitchClbks.cslRegVal2operStatus        = cslKendin8995xRegVal2OperStatus;
         
         swHead->cslSwitchClbks.cslOperStatus2regVal        = cslKendin8995xOperStatus2RegVal;
         
         swHead->cslSwitchClbks.cslSwitchGetVersion         = cslKendin8995xSwitchGetVersion;
         
         swHead->cslSwitchClbks.cslPrepareReadStatSequence  = PAL_NULL;
         
         swHead->cslSwitchClbks.cslPortNum2RegAddr          = cslKendin8995xPortNum2RegAddr;
         
         swHead->cslSwitchClbks.cslPortNum2PhyAddr          = cslKendin8995xPortNum2PHYAddr;
         
         swHead->cslSwitchClbks.cslSwitchStart              = cslKendin8995xStart;
         
         swHead->cslSwitchClbks.cslVlnCapsSet               = cslKendin8995xSpecialCapsSet ;
         
         swHead->cslSwitchClbks.cslVlnDefaultSet            = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnUpdate                = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnPriorityGet           = PAL_NULL;
         
         swHead->cslSwitchClbks.cslVlnPrioritySet           = PAL_NULL;
         
         swHead->cslSwitchClbks.cslDeleteMacAddEntry           = PAL_NULL;
         
         
         return PAL_True;
      }
#endif
   case SWITCH_ADM_6995:
   case SWITCH_RTL_8335:
   default:
      {
         return PAL_False;
      }
   }
}


/*******************************************************************************
* cslSwitchRmv - switch deinitialization
*
*  
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

PAL_Result cslSwitchRmv(PCSL_SWITCH_DRV_HANDLE swHead)
{
    PCSL_SWITCH_VLAN_TABLE tmp,tmp1;
    for (tmp1 = swHead->pVlanTable,tmp = swHead->pVlanTable->next;tmp != PAL_NULL;tmp1=tmp,tmp = tmp->next)
        PAL_FREE(tmp1);
    
    PAL_FREE(tmp1);

    return PAL_True;
}

/*******************************************************************************
* cslCreateVLANEntry - creates and adds VLAN entry to the VLAN table
*
*  Creates and adds VLAN entry to the logical VLAN table. List manager.
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

PCSL_SWITCH_VLAN_TABLE cslCreateVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead)
{
    PCSL_SWITCH_VLAN_TABLE tmp,elem;
    VlanCounter++;
    
    /* The 6063 support up to 64 different VLAN Identifier */
    if(VlanCounter > 64)
       return NULL;

    elem = (PCSL_SWITCH_VLAN_TABLE) PAL_MALLOC(sizeof(CSL_SWITCH_VLAN_TABLE));
    if(elem == NULL)
         return NULL;

    tmp = swHead->pVlanTable->next;
    swHead->pVlanTable->next = elem;
    elem->next = tmp;
    elem->portMask = 0;
    elem->portTagMask = 0;
    elem->valid = 0;
    elem->vlanId = 0;
    return elem;
}

/*******************************************************************************
* cslDestroyVLANEntry - destroys VLAN entry
*
*  Removes VLAN entry from VLAN table. List manager.
*
* INPUTS:
*       [In] PCSL_SWITCH_DRV_HANDLE swHead
*
*******************************************************************************/

Int32 cslDestroyVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead, PCSL_SWITCH_VLAN_TABLE elem)
{
    PCSL_SWITCH_VLAN_TABLE tmp;
    for (tmp = swHead->pVlanTable;tmp != PAL_NULL; tmp = tmp->next)
    {

        /* elem is not NULL!!*/
        if(elem == tmp->next)
        {
            tmp->next = tmp->next->next;
            PAL_FREE(elem);
            return PAL_True;
        }
    }

    return PAL_False;

}

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

PCSL_SWITCH_VLAN_TABLE cslGetVLANEntry(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 vlnId)
{

    PCSL_SWITCH_VLAN_TABLE tmp;
    for (tmp = swHead->pVlanTable;tmp != PAL_NULL; tmp = tmp->next)
    {
        if(tmp->vlanId == vlnId)
        {
            return tmp;
        }
    }

    return PAL_NULL;
}


