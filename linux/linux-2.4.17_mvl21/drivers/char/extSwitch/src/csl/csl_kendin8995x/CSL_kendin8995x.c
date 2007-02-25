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
#include "DDC_switch.h"
#include "CSL_switchPrvt.h"
#ifdef LINUX_COMPILATION
#include "linuxpal.h"
#else
#ifdef VX_COMPILATION
#include "vxWorkspal.h"
#endif
#endif


#define KENDIN_8995x_STATUS_SHIFT_MASK   1
#define KENDIN_8995x_STATUS_MASK         0x1

#define KENDIN_8995x_LINK_SHIFT_MASK     2
#define KENDIN_8995x_LINK_MASK           0x1

#define KENDIN_8995x_DUPLEX_SHIFT_MASK   8
#define KENDIN_8995x_DUPLEX_MASK         0x1

#define KENDIN_8995x_SPEED_SHIFT_MASK    13
#define KENDIN_8995x_SPEED_MASK          0x1

#define KENDIN_8995x_AUTO_ENABLE_SHIFT_MASK      12
#define KENDIN_8995x_AUTO_ENABLE_MASK            0x1

#define KENDIN_8995x_AUTO_RESTART_SHIFT_MASK     9
#define KENDIN_8995x_AUTO_RESTART_MASK           0x1


/* general commands*/
#define PHY_PHY_ADDR(portNum)       portNum



/*******************************************************************************
*
* cslKendin8995xStart - 

*
*/
Int32 cslKendin8995xStart (PCSL_SWITCH_DRV_HANDLE swHead)
{

    return PAL_True;
}

/*******************************************************************************
*
* cslKendin8995xRegVal2OperStatus - extract the user operational status out of register value.
*
* This code contains switch specific information. The main purpose is converting switch specific info to 
* the neutral code. As a neutral data types we're using external interfaces.
*
* INPUTS:
*     [In]    OPER_TYPE oper 
*     [In]    Uint16   data
* 
*
*
* RETURNS: 
*       operational status to be returned to user
*
*
*/

Int32 cslKendin8995xRegVal2OperStatus (OPER_TYPE_E oper, Uint16 data)
{
    Int32 retVal = -1;
    
    if (oper == OPER_STATUS)
    {
        /* status is two bits, which coincide with the interface value(enum) */

        retVal = (Int32)data & (KENDIN_8995x_STATUS_MASK << KENDIN_8995x_STATUS_SHIFT_MASK);
        return retVal;
    }

    else if (oper == OPER_LINK)
    {
        retVal = (Int32)data & (KENDIN_8995x_LINK_MASK << KENDIN_8995x_LINK_SHIFT_MASK);
        if(retVal == 0)
            return PORT_LINK_DOWN;
        return PORT_LINK_UP;
    }

    else if (oper == OPER_DUPLEX)
    {
        retVal = (Int32)data & (KENDIN_8995x_DUPLEX_MASK << KENDIN_8995x_DUPLEX_SHIFT_MASK);
        if(retVal == 0)
            return PORT_DUPLEX_HALF;
        return PORT_DUPLEX_FULL;
    }

    else if (oper == OPER_SPEED)
    {
        retVal = (Int32)data & (KENDIN_8995x_SPEED_MASK << KENDIN_8995x_SPEED_SHIFT_MASK);
        if(retVal == 0)
            return PORT_SPEED_10;
        return PORT_SPEED_100;
    }

    else if (oper == OPER_AUTO)
    {
        retVal = (Int32)data & (KENDIN_8995x_LINK_MASK << KENDIN_8995x_LINK_SHIFT_MASK);
        if (retVal == 0)
            return PORT_AUTO_NO;
        return PORT_AUTO_YES;
    }
    
    return retVal;
}

/*******************************************************************************
*
* cslKendin8995xOperStatus2RegVal - constructs the register value to be written out of data and user request
*
* Enable/Disable a selected port
*
* INPUTS:
*     [In]    OPER_TYPE     oper 
*     [In]    Int32        operValue
*     [In]    Uint16       data
* 
*
*
* RETURNS: 
*       The final register value
*
*
*/

Uint16 cslKendin8995xOperStatus2RegVal (OPER_TYPE_E oper,Int32 operValue,Uint16 data)
{
    Uint16 retVal = 0xffff;

    
    if (oper == OPER_STATUS)
    {
        retVal = (data & (~ ((Uint16)(KENDIN_8995x_STATUS_MASK << KENDIN_8995x_STATUS_SHIFT_MASK)))) | ((Uint16)operValue);

    }
    else if (oper == OPER_DUPLEX)
    {
        retVal = data & (~((Uint16)(KENDIN_8995x_DUPLEX_MASK << KENDIN_8995x_DUPLEX_SHIFT_MASK)));
        if(operValue == PORT_DUPLEX_FULL)
            retVal |= (KENDIN_8995x_DUPLEX_MASK << KENDIN_8995x_DUPLEX_SHIFT_MASK);

        /* assert software reset, otherwise the changes will be ignored */
        retVal |= (Uint16)(KENDIN_8995x_AUTO_RESTART_MASK << KENDIN_8995x_AUTO_RESTART_SHIFT_MASK);  

    }
    
    else if (oper == OPER_SPEED)
    {
        retVal = data & (~((Uint16)(KENDIN_8995x_SPEED_MASK << KENDIN_8995x_SPEED_SHIFT_MASK)));
        if(operValue == PORT_SPEED_100)
            retVal |= (KENDIN_8995x_SPEED_MASK << KENDIN_8995x_SPEED_SHIFT_MASK);

        /* assert software reset, otherwise the changes will be ignored */
        retVal |= (Uint16)(KENDIN_8995x_AUTO_RESTART_MASK << KENDIN_8995x_AUTO_RESTART_SHIFT_MASK); 
    }

    else if (oper == OPER_AUTO)
    {
        retVal = data & (~((Uint16)(KENDIN_8995x_AUTO_ENABLE_MASK << KENDIN_8995x_AUTO_ENABLE_SHIFT_MASK)));
        if(operValue == PORT_AUTO_YES)
        {
            retVal |= (Uint16)(KENDIN_8995x_AUTO_ENABLE_MASK << KENDIN_8995x_AUTO_ENABLE_SHIFT_MASK);
            retVal |= (Uint16)(KENDIN_8995x_AUTO_RESTART_MASK << KENDIN_8995x_AUTO_RESTART_SHIFT_MASK);
        }
        else if (operValue == PORT_AUTO_RESTART)
        {
            retVal |= (Uint16)(KENDIN_8995x_AUTO_RESTART_MASK << KENDIN_8995x_AUTO_RESTART_SHIFT_MASK);
        }

        /* assert software reset, otherwise the changes will be ignored */
        retVal |= (Uint16)(KENDIN_8995x_AUTO_RESTART_MASK << KENDIN_8995x_AUTO_RESTART_SHIFT_MASK); 
    }

    return retVal;
    
}


/*******************************************************************************
*
* cslKendin8995xSwitchGetVersion - returns the switch specific information
*
* 
*
* INPUTS:
*     [Out]   PSW_VER_INFO pSwitchInfo
* 
*
*
* RETURNS: 
*       Status
*
*
*/

void cslKendin8995xSwitchGetVersion(PCSL_SWITCH_DRV_HANDLE swDrv,PSW_VER_INFO pSwitchInfo)
{


    pSwitchInfo->is802_1_P_capable      = PAL_True; 
    pSwitchInfo->is802_1_Q_capable      = PAL_True;
    pSwitchInfo->maxNumberOfPorts       = 5;
    pSwitchInfo->maxNumberOfPriorities  = 0;
    pSwitchInfo->maxNumberOfVLANs       = 0;
    pSwitchInfo->Model                  = 0x95;
}

        
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

Uint32 cslKendin8995xPortNum2RegAddr(OPER_TYPE_E type, Uint32 portNum)
{
    Uint32 regAddr = 0;

    switch (type) 
            {  /* TBD :restart Auto-Negotiation,Power down */
                case OPER_STATUS: 
                    regAddr = 0x0; 
                    break; 
                case OPER_LINK: 
                    regAddr = 0x01; 
                    break; 
                case OPER_DUPLEX: 
                    regAddr = 0x0; 
                    break; 
                case OPER_SPEED: 
                    regAddr = 0x0; 
                    break; 
                case OPER_AUTO: 
                    regAddr = 0x0; 
                    break; 

                default: 
                   regAddr = 0xFFFF;
                    break; 
        } 

    return regAddr;
}

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


Uint32 cslKendin8995xPortNum2PHYAddr(OPER_TYPE_E type, Uint32 portNum)
{
    Uint32 phyAddr = 0;
    switch (type) 
        { 
            case OPER_STATUS: 
                phyAddr = portNum; 
                break; 
            case OPER_LINK: 
                phyAddr = portNum; 
                break; 
            case OPER_DUPLEX: 
                phyAddr = portNum; 
                break; 
            case OPER_SPEED: 
                phyAddr = portNum;
                break; 
            case OPER_AUTO: 
                phyAddr = portNum; 
                break; 
            default: 
               phyAddr = 0xFFFF;
                break; 
        }

    return phyAddr;
}


/*******************************************************************************
*
* cslKendin8995xPrepareReadStatSequence -  Kendin8995x doesn't 
*                                          support statistics counters
*
*/
Int32 cslKendin8995xPrepareReadStatSequence(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 port)
{
    return PAL_True;
}

/*******************************************************************************
*
* cslKendin8995xGetPreparedCounters - Kendin8995x doesn't 
*                                     support statistics counters
*
*/
Int32 cslKendin8995xGetPreparedCounters(PCSL_SWITCH_DRV_HANDLE swHead,Uint32 statIndex,Uint32* counter)
{    
        *counter  = 0;
        return PAL_True;
}
/*******************************************************************************
*
* cslKendin8995xSpecialCapsSet - Kendin8995x Dosen't support VLAN
*                                     
*
*/
Int32 cslKendin8995xSpecialCapsSet(PCSL_SWITCH_DRV_HANDLE swHead, Uint32 cmd, Uint32 arg)
{
    return PAL_True;
}




