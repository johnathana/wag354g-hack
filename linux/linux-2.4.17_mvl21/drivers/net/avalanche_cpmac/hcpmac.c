/******************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002-2004 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE:
 *
 *  DESCRIPTION:
 *      This file contains the code for the HAL EMAC Bridge Test
 *
 *  HISTORY:
 *  xxXxx01 Denis            RC1.00  Original Version created.
 *  22Jan02 Denis/Mick       RC1.01  Modified for HAL EMAC API
 *  24Jan02 Denis/Mick       RC1.02  Speed Improvements
 *  28Jan02 Denis/Mick       RC1.16  Made function calls pointers
 *  28Jan02 Mick             RC1.18  Split into separate modules
 *  29Jan02 Mick             RC1.19  Hal include file cleaned up
 *  15Jul02 Michael Hanrahan RC1.20  Synch'd with Linux Version
 *  23Sep02 Michael Hanrahan RC1.21  Added CPPI.C
 *  16Oct02 Michael Hanrahan RC1.22  Added CAF etc to Options.Conf
 *  09Jan03 Michael Hanrahan RC3.01  Fixed incorrect MDIO check
 *  01Feb03 Michael Hanrahan RC3.02  Updated for GPIO/PBUSFREQ
 *  29Mar03 Michael Hanrahan 1.03 Corrected  ChannelConfigGet
 *  29Mar03 Michael Hanrahan 1.03 Removed user setting of TxNumQueues
 *  23Aug04 Michael Hanrahan 1.7.8 Support for Setting Mac Address
 *  @author Michael Hanrahan
 *  @version 1.02
 *  @date    24-Jan-2002
 *****************************************************************************/
#define _HAL_CPMAC
#define _CPHAL_CPMAC
#define _CPHAL
#define __CPHAL_CPMDIO

#include "dox_cpmac.h"  /*  Documentation information */

/*  OS Data Structure definitions  */

typedef void OS_PRIVATE;
typedef void OS_DEVICE;
typedef void OS_SENDINFO;
typedef void OS_RECEIVEINFO;
typedef void OS_SETUP;

/*  HAL Data Structure definitions  */

typedef struct _phy_device PHY_DEVICE;
typedef struct hal_device  HAL_DEVICE;
typedef struct hal_private HAL_PRIVATE;
typedef struct hal_private HAL_RECEIVEINFO;

#include "cpcommon_cpmac.h"
#include "cpswhal_cpmac.h"
#include "cpmdio.h"
#include "hcpmac.h"
#include "cpmac_reg.h"


#define EC_MODULE

/* MDIO Clock Frequency Default Value */

/* Rcb/Tcb Constants */

#define CB_SOF_BIT         (1<<31)
#define CB_EOF_BIT         (1<<30)
#define CB_SOF_AND_EOF_BIT (CB_SOF_BIT|CB_EOF_BIT)
#define CB_OWNERSHIP_BIT   (1<<29)
#define CB_EOQ_BIT         (1<<28)
#define CB_SIZE_MASK       0x0000ffff
#define RCB_ERRORS_MASK    0x03fe0000

static char *channel_names[]   = CHANNEL_NAMES; /* GSG 11/22 (may change this implementation) */

#define scFound(Module) if (HalDev->State != enDevFound)    return (Module|EC_FUNC_CHSETUP|EC_VAL_INVALID_STATE)
#define scInit(Module)  if (HalDev->State <  enInitialized) return (Module|EC_FUNC_CHSETUP|EC_VAL_INVALID_STATE)
#define scOpen(Module)  if (HalDev->State <  enOpened)      return (Module|EC_FUNC_CHSETUP|EC_VAL_INVALID_STATE)



/********************************************************************
**
**  L O C A L  F U N C T I O N S
**
********************************************************************/
static int halIsr(HAL_DEVICE *HalDev, int *MorePackets);
static int cpmacRandom(HAL_DEVICE *HalDev);
static int cpmacRandomRange(HAL_DEVICE *HalDev, int min, int max);
static int halPacketProcessEnd(HAL_DEVICE *HalDev);

#include "cpcommon_cpmac.c"                                                /*~RC3.02*/
#include "cppi_cpmac.c"
#include "cpmdio.c"                                                  /*~RC3.02*/

static int MacAddressSave(HAL_DEVICE *HalDev, unsigned char *MacAddr)
  {
  int i;
  int inst             = HalDev->inst;

  HalDev->MacAddr = MacAddr;

  if(HalDev->debug)
    {
    dbgPrintf("MacAddrSave[%d]: ", inst);
    for (i=0;i<6;i++)
       dbgPrintf("%X", HalDev->MacAddr[i]);
    dbgPrintf("\n");
    osfuncSioFlush();
    }
  return(EC_NO_ERRORS);
  }
static int MacAddressSet(HAL_DEVICE *HalDev)
  {
  unsigned char *macadr = &HalDev->MacAddr[0];
  int base             = HalDev->dev_base;

  scOpen(EC_CPMAC);
  CPMAC_MACADDRLO_0(base) = macadr[5];
  CPMAC_MACADDRMID(base)  = macadr[4];
  CPMAC_MACADDRHI(base)   = (macadr[0])|(macadr[1]<<8)|(macadr[2]<<16)|(macadr[3]<<24);
  if(HalDev->debug)
    {
     dbgPrintf("MacAddrSet: MacAddr(%d) %X %X %X\n", HalDev->inst, CPMAC_MACADDRLO_0(base),
                                                     CPMAC_MACADDRMID(base),
                                                     CPMAC_MACADDRHI(base));

     dbgPrintf("Start MAC: %d\n",HalDev->dev_base);
     osfuncSioFlush();
    }
  return(EC_NO_ERRORS);
  }


/*
  Updates the MacHash registers
*/
static void MacHashSet(HAL_DEVICE *HalDev)
  {
  if(HalDev->State <  enOpened)
    return;

  CPMAC_MACHASH1(HalDev->dev_base) = HalDev->MacHash1;
  CPMAC_MACHASH2(HalDev->dev_base) = HalDev->MacHash2;
  if (DBG(11))
    dbgPrintf("CPMAC[%X]: MacHash1 0x%08X, MacHash2 0x%08X\n", HalDev->dev_base, CPMAC_MACHASH1(HalDev->dev_base), CPMAC_MACHASH2(HalDev->dev_base));
  }

/*
  Reads the MacControl register and updates
  the changable bits. (See MACCONTROL_MASK)
*/
static void RxMBP_EnableSet(HAL_DEVICE *HalDev)
  {
   bit32u RxMbpEnable;
   if(HalDev->State <  enOpened)
     return;
   RxMbpEnable  = CPMAC_RX_MBP_ENABLE(HalDev->dev_base);
   RxMbpEnable &= ~RX_MBP_ENABLE_MASK; /* Clear out updatable bits */
   RxMbpEnable |= HalDev->RxMbpEnable;
   CPMAC_RX_MBP_ENABLE(HalDev->dev_base) = RxMbpEnable;
  }
/*
  Reads the MacControl register and updates
  the changable bits. (See MACCONTROL_MASK)
*/
static void MacControlSet(HAL_DEVICE *HalDev)
  {
   bit32u MacControl;
   if(HalDev->State <  enOpened)
     return;
   MacControl  = CPMAC_MACCONTROL(HalDev->dev_base);
   MacControl &= ~MACCONTROL_MASK; /* Clear out updatable bits */
   MacControl |= HalDev->MacControl;
   if(!(MacControl & MII_EN)) /* If Enable is not set just update register  */
      CPMAC_MACCONTROL(HalDev->dev_base) = MacControl;
   else
     {
     if(MacControl & CTRL_LOOPBACK)    /*  Loopback Set  */
       {
       /* mii_en is set and loopback is needed,
          clear mii_en, set loopback, then set mii_en
       */
       MacControl &= ~MII_EN;  /* Clear MII_EN  */
       CPMAC_MACCONTROL(HalDev->dev_base) = MacControl;
       CPMAC_MACCONTROL(HalDev->dev_base) |= MII_EN; /* Set MII_EN  */
       HalDev->Linked = 1; /* if in loopback the logically linked */
       }
     else  /* If Loopback not set just update */
       {
       CPMAC_MACCONTROL(HalDev->dev_base) = MacControl;
       }
     }
    if(DBG(0))
      dbgPrintf("[halMacControlSet]MacControl:%08X\n", CPMAC_MACCONTROL(HalDev->dev_base));
  }
static int UnicastSet(HAL_DEVICE *HalDev)
  {
  CPMAC_RX_UNICAST_SET(HalDev->dev_base)    = HalDev->RxUnicastSet;
  CPMAC_RX_UNICAST_CLEAR(HalDev->dev_base)  = HalDev->RxUnicastClear;
  return(EC_NO_ERRORS);
  }

  
static bit32u HashGet(bit8u *Address)
  {
  bit32u hash;
    bit8u  tmpval;
    int    i;

    hash = 0;
    for( i=0; i<2; i++ )
    {
        tmpval = *Address++;
        hash  ^= (tmpval>>2)^(tmpval<<4);
        tmpval = *Address++;
        hash  ^= (tmpval>>4)^(tmpval<<2);
        tmpval = *Address++;
        hash  ^= (tmpval>>6)^(tmpval);
    }

    return( hash & 0x3F );
  }

static void HashAdd(HAL_DEVICE *HalDev, bit8u *MacAddress)
{
  bit32u HashValue;
  bit32u HashBit;

  HashValue = HashGet(MacAddress);

  if(HashValue < 32)
    {
    HashBit = (1 << HashValue);
    HalDev->MacHash1 |= HashBit;
    }
    else
    {
    HashBit = (1 << (HashValue-32));
    HalDev->MacHash2 |= HashBit;
    }
}

static void HashDel(HAL_DEVICE *HalDev, bit8u *MacAddress)
{
  bit32u HashValue;
  bit32u HashBit;

  HashValue = HashGet(MacAddress);

  if(HashValue < 32)
    {
    HashBit = (1 << HashValue);
    HalDev->MacHash1 &= ~HashBit;
    }
    else
    {
    HashBit = (1 << (HashValue-32));
    HalDev->MacHash2 &= ~HashBit;
    }
}

/*  Replace with an array based on key, with a ptr to the code to do */
/* e.g.  [enRX_PASS_CRC] = {Set, MBP_UPDATE() } */
static void DuplexUpdate(HAL_DEVICE *HalDev)
{
  int base           = HalDev->dev_base;
  PHY_DEVICE *PhyDev = HalDev->PhyDev;

  if(HalDev->State <  enOpened)
    return;

   /* No Phy Condition */
   if(HalDev->MdioConnect & _CPMDIO_NOPHY)                        /*MJH+030805*/
   {
       /*  No Phy condition, always linked */
       HalDev->Linked     = 1;
       HalDev->EmacSpeed  = 1;
       HalDev->EmacDuplex = 1;
       HalDev->PhyNum     = 0xFFFFFFFF;  /* No Phy Num */
       CPMAC_MACCONTROL(base) |= FULLDUPLEX;                      /*MJH+030909*/
       osfuncStateChange();
       return;
   }

  if(HalDev->MacControl & CTRL_LOOPBACK)    /*  Loopback Set  */
    {
    HalDev->Linked = 1;
    return;
    }

  if (HalDev->MdioConnect & _CPMDIO_LOOPBK)
    {
    HalDev->Linked = cpMacMdioGetLoopback(HalDev->PhyDev);
    }
  else
    {
    HalDev->Linked = cpMacMdioGetLinked(HalDev->PhyDev);
    }
  if (HalDev->Linked)
    {
     /*  Retreive Duplex and Speed and the Phy Number  */
     if(HalDev->MdioConnect & _CPMDIO_LOOPBK)
       HalDev->EmacDuplex = 1;
     else
       HalDev->EmacDuplex = cpMacMdioGetDuplex(PhyDev);
     HalDev->EmacSpeed  = cpMacMdioGetSpeed(PhyDev);
     HalDev->PhyNum     = cpMacMdioGetPhyNum(PhyDev);

     if(HalDev->EmacDuplex)
       CPMAC_MACCONTROL(base) |= FULLDUPLEX;
     else
       CPMAC_MACCONTROL(base) &= ~FULLDUPLEX;
     if(HalDev->debug)
        dbgPrintf("%d: Phy= %d, Speed=%s, Duplex=%s\n",HalDev->inst,HalDev->PhyNum,(HalDev->EmacSpeed)?"100":"10",(HalDev->EmacDuplex)?"Full":"Half");
    }
  if(HalDev->debug)
     dbgPrintf("DuplexUpdate[%d]: MACCONTROL 0x%08X, %s\n", HalDev->inst, CPMAC_MACCONTROL(base),(HalDev->Linked)?"Linked":"Not Linked");
}
static void MdioSetPhyMode(HAL_DEVICE *HalDev)
  {
  unsigned int PhyMode;
  /* Verify proper device state */
  if (HalDev->State < enOpened)
    return;

  PhyMode = NWAY_AUTO|NWAY_FD100|NWAY_HD100|NWAY_FD10|NWAY_HD10;
  if(DBG(0))
    {
    dbgPrintf("halSetPhyMode1: MdioConnect:%08X ,", HalDev->MdioConnect);
    dbgPrintf("PhyMode:%08X Auto:%d, FD10:%d, HD10:%d, FD100:%d, HD100:%d\n", PhyMode,
           PhyMode&NWAY_AUTO, PhyMode&NWAY_FD10, PhyMode&NWAY_HD10, PhyMode&NWAY_FD100,
           PhyMode&NWAY_HD100);
    }


  if (  HalDev->MdioConnect & _CPMDIO_NEG_OFF)  /* ~RC3.01                */
    PhyMode &= ~(NWAY_AUTO);                    /* Disable Auto Neg       */
  if (!(HalDev->MdioConnect & _CPMDIO_HD))
    PhyMode &= ~(NWAY_HD100|NWAY_HD10);         /* Cannot support HD      */
  if (!(HalDev->MdioConnect & _CPMDIO_FD))
    PhyMode &= ~(NWAY_FD100|NWAY_FD10);         /* Cannot support FD      */
  if (!(HalDev->MdioConnect & _CPMDIO_10))
    PhyMode &= ~(NWAY_HD10|NWAY_FD10);          /* Cannot support 10 Mbs  */
  if (!(HalDev->MdioConnect & _CPMDIO_100))
    PhyMode &= ~(NWAY_HD100|NWAY_FD100);        /* Cannot support 100 Mbs */

  if(HalDev->MdioConnect & _CPMDIO_AUTOMDIX)   PhyMode |= NWAY_AUTOMDIX; /* Set AutoMdix */

  if (HalDev->CpmacFrequency <= 50000000)
    PhyMode &= ~(NWAY_FD100|NWAY_HD100); /* Cannot support 100 MBS */
  if(DBG(7))
    dbgPrintf("halNeg: PhyMode[0x%08X] %d\n", HalDev->dev_base, PhyMode);

  if(DBG(0))
    {
    dbgPrintf("halSetPhyMode2: MdioConnect:%08X ,", HalDev->MdioConnect);
    dbgPrintf("PhyMode:%08X Auto:%d, FD10:%d, HD10:%d, FD100:%d, HD100:%d\n", PhyMode,
               PhyMode&NWAY_AUTO, PhyMode&NWAY_FD10, PhyMode&NWAY_HD10, PhyMode&NWAY_FD100,
               PhyMode&NWAY_HD100);
    }


  cpMacMdioSetPhyMode(HalDev->PhyDev,PhyMode);
  DuplexUpdate(HalDev);
  }
static int StatsClear(HAL_DEVICE *HalDev)
{
  int i;
  MEM_PTR pStats;

  scOpen(EC_CPMAC);

  pStats = pCPMAC_RXGOODFRAMES(HalDev->dev_base);
  for (i=0;i<STATS_MAX;i++)
    {
    *(MEM_PTR)(pStats) =  0xFFFFFFFF;
    pStats++;
    }

  return(EC_NO_ERRORS);
}
static void StatsDump(HAL_DEVICE *HalDev, void *Value)
 {
 MEM_PTR ptrStats;
 MEM_PTR ptrValue;
 int i;
 ptrStats = pCPMAC_RXGOODFRAMES(HalDev->dev_base);
 ptrValue = (bit32u*) Value;
 for (i=0; i<STATS_MAX; i++)
  {
     *ptrValue = *ptrStats;
     if(DBG(4))
      {
      dbgPrintf("halStatsDump: Stat[%d:0x%08X] %d 0x%08X %d\n", i, ptrStats, *ptrStats, ptrValue, *ptrValue);
      osfuncSioFlush();
      }
     ptrStats++;
     ptrValue++;
  }
 }
static void ConfigApply(HAL_DEVICE *HalDev)
  {
  CPMAC_RX_MAXLEN(HalDev->dev_base)          = HalDev->RxMaxLen;
  CPMAC_RX_FILTERLOWTHRESH(HalDev->dev_base) = HalDev->RxFilterLowThresh;
  CPMAC_RX0_FLOWTHRESH(HalDev->dev_base)     = HalDev->Rx0FlowThresh;
  UnicastSet(HalDev);
  MacAddressSet(HalDev);
  RxMBP_EnableSet(HalDev);
  MacHashSet(HalDev);
  MacControlSet(HalDev);
  if(DBG(0))
     dbgPrintf("ValuesUpdate[%d]: MBP_ENABLE 0x%08X\n", HalDev->inst, CPMAC_RX_MBP_ENABLE(HalDev->dev_base));
  }
static int halStatus(HAL_DEVICE *HalDev)
{
   int status;

    if(HalDev->State <  enOpened)
      return (EC_CPMAC|EC_FUNC_STATUS|EC_VAL_INVALID_STATE);      /*MJH+030805*/

   /* No Phy Condition */
   if(HalDev->MdioConnect & _CPMDIO_NOPHY)                        /*MJH+030805*/
   {
       /*  No Phy condition, always linked */
       status = HalDev->Linked;
       status |= CPMAC_STATUS_LINK_DUPLEX;
       status |= CPMAC_STATUS_LINK_SPEED;
       return(status);
   }


   if (HalDev->HostErr)  /* Adapter Check  */
    {
    bit32u tmp;
    status = CPMAC_STATUS_ADAPTER_CHECK;
    if(HalDev->MacStatus & RX_HOST_ERR_CODE)
      {
      status |= CPMAC_STATUS_HOST_ERR_DIRECTION;
      tmp     = (HalDev->MacStatus & RX_HOST_ERR_CODE) >> 12; /* Code */
      status |= (tmp << 9); /* Code */
      tmp     = (HalDev->MacStatus & RX_ERR_CH) >> 8; /* Channel */
      status |= (tmp << 13);
      }
    else
    if(HalDev->MacStatus & TX_HOST_ERR_CODE)
      {
      status |= CPMAC_STATUS_HOST_ERR_DIRECTION;
      tmp     = (HalDev->MacStatus & TX_HOST_ERR_CODE) >> 20; /* Code */
      status |= (tmp << 9); /* Code */
      tmp     = (HalDev->MacStatus & TX_ERR_CH) >> 16; /* Channel */
      status |= (tmp << 13);
      }
    }
   else
    {
    status = HalDev->Linked;
    if(status)
      {
      status = CPMAC_STATUS_LINK;
      if(cpMacMdioGetDuplex(HalDev->PhyDev))
        status |= CPMAC_STATUS_LINK_DUPLEX;
      if(cpMacMdioGetSpeed(HalDev->PhyDev))
        status |= CPMAC_STATUS_LINK_SPEED;
      }
    }
   if(HalDev->debug)
      dbgPrintf("[halStatus] Link Status is %d for 0x%X\n", status, HalDev->dev_base);
   return(status);
}
static int InfoAccess(HAL_DEVICE *HalDev, int Key, int Action, void *ParmValue)
  {
  int rc = 0;
  int Update=0;

  switch (Key)
  {
    /********************************************************************/
    /*                                                                  */
    /*                      GENERAL                                     */
    /*                                                                  */
    /********************************************************************/

    case enVersion :
         if(Action==enGET)
           {
           *(const char **)ParmValue = pszVersion_CPMAC;
           }
    break;
    case enDebug  :
         if(Action==enSET)
           {
           HalDev->debug = *(unsigned int *)ParmValue;
           }
    break;

    case enStatus      :
         if(Action==enGET)
           {
           int status;
           status = halStatus(HalDev);
           *(int *)ParmValue = status;
           }
    break;
    /********************************************************************/
    /*                                                                  */
    /*                      RX_MBP_ENABLE                               */
    /*                                                                  */
    /********************************************************************/

    case enRX_PASS_CRC  :
         if(Action==enSET)
           {
           UPDATE_RX_PASS_CRC(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_QOS_EN  :
         if(Action==enSET)
           {
           UPDATE_RX_QOS_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_NO_CHAIN :
         if(Action==enSET)
           {
           UPDATE_RX_NO_CHAIN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_CMF_EN :
         if(Action==enSET)
           {
           UPDATE_RX_CMF_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_CSF_EN :
         if(Action==enSET)
           {
           UPDATE_RX_CSF_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_CEF_EN :
         if(Action==enSET)
           {
           UPDATE_RX_CEF_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_CAF_EN :
         if(Action==enSET)
           {
           UPDATE_RX_CAF_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_PROM_CH :
         if(Action==enSET)
           {
           UPDATE_RX_PROM_CH(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_BROAD_EN :
         if(Action==enSET)
           {
           UPDATE_RX_BROAD_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_BROAD_CH :
         if(Action==enSET)
           {
           UPDATE_RX_BROAD_CH(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_MULT_EN :
         if(Action==enSET)
           {
           UPDATE_RX_MULT_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_MULT_CH :
         if(Action==enSET)
           {
           UPDATE_RX_MULT_CH(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;

    /********************************************************************/
    /*                                                                  */
    /*                      MAC_CONTROL                                 */
    /*                                                                  */
    /********************************************************************/

    case enTX_PTYPE :
         if(Action==enSET)
           {
           UPDATE_TX_PTYPE(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enTX_PACE :
         if(Action==enSET)
           {
           UPDATE_TX_PACE(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enTX_FLOW_EN :
         if(Action==enSET)
           {
           UPDATE_TX_FLOW_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    case enRX_FLOW_EN :
         if(Action==enSET)
           {
           UPDATE_RX_FLOW_EN(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;

    case enCTRL_LOOPBACK :
         if(Action==enSET)
           {
           UPDATE_CTRL_LOOPBACK(*(unsigned int *)ParmValue);
           Update=1;
           }
    break;
    /********************************************************************/
    /*                                                                  */
    /*                      RX_UNICAST_SET                              */
    /*                                                                  */
    /********************************************************************/

    case enRX_UNICAST_SET :
         if(Action==enSET)
          {
           HalDev->RxUnicastSet   |=  (1 << *(unsigned int *)ParmValue);
           HalDev->RxUnicastClear &= ~(1 << *(unsigned int *)ParmValue);
           Update=1;
          }
    break;
    case enRX_UNICAST_CLEAR :
         if(Action==enSET)
          {
          HalDev->RxUnicastClear |=  (1 << *(unsigned int *)ParmValue);
          HalDev->RxUnicastSet   &= ~(1 << *(unsigned int *)ParmValue);
          Update=1;
          }
    break;

    case enRX_MAXLEN :
         if(Action==enSET)
           {
           HalDev->RxMaxLen    =   *(unsigned int *)ParmValue;
           Update=1;
           }
    break;

    case enRX_FILTERLOWTHRESH :
         if(Action==enSET)
           {
           HalDev->RxFilterLowThresh  =   *(unsigned int *)ParmValue;
           Update=1;
           }
    break;
    case enRX0_FLOWTHRESH :
         if(Action==enSET)
           {
           HalDev->Rx0FlowThresh =   *(unsigned int *)ParmValue;
           Update=1;
           }
    break;
    /********************************************************************/
    /*                                                                  */
    /*                      RX_MULTICAST                                */
    /*                                                                  */
    /********************************************************************/

    case enRX_MULTICAST :
    break;
    case enRX_MULTI_SINGLE :
         if(DBG(11))
          {
           int tmpi;
           bit8u *MacAddress;
           MacAddress = (bit8u *) ParmValue;
           dbgPrintf("CPMAC[%X]:  MacAddress '", HalDev->dev_base);
           for (tmpi=0; tmpi<6; tmpi++)
             dbgPrintf("%02X:", MacAddress[tmpi]);
             dbgPrintf("\n");
          }
         if(Action==enCLEAR)
           {
           HashDel(HalDev, ParmValue);
           Update=1;
           }
         else
         if(Action==enSET)
           {
           HashAdd(HalDev, ParmValue);
           Update=1;
           }
    break;
    case enRX_MULTI_ALL :
         if(Action==enCLEAR)
           {
           HalDev->MacHash1 = 0;
           HalDev->MacHash2 = 0;
           Update=1;
           }
         else
         if(Action==enSET)
           {
           HalDev->MacHash1 = 0xFFFFFFFF;
           HalDev->MacHash2 = 0xFFFFFFFF;
           Update=1;
           }
    break;

    /********************************************************************/
    /*                                                                  */
    /*                      MDIO                                        */
    /*                                                                  */
    /********************************************************************/

    case enMdioConnect :
         if(Action==enSET)
           {
           HalDev->MdioConnect = *(unsigned int *)ParmValue;
           MdioSetPhyMode(HalDev);
           }
           if(Action==enGET)
           {
           *(unsigned int *)ParmValue = HalDev->MdioConnect;
           }
    break;


    /********************************************************************/
    /*                                                                  */
    /*                      STATISTICS                                  */
    /*                                                                  */
    /********************************************************************/
    case enStatsClear :
           StatsClear(HalDev);
    break;
    case enStatsDump :
         if(Action==enGET)
           {
           StatsDump(HalDev, ParmValue);
           }
    break;

/*  Not implemented
    case enStats1 :
         if(Action==enGET)
           {
           StatsGet(HalDev, ParmValue, 1);
           }
    break;

    case enStats2 :
         if(Action==enGET)
           {
           StatsGet(HalDev, ParmValue, 2);
           }
    break;
    case enStats3 :
         if(Action==enGET)
           {
           StatsGet(HalDev, ParmValue, 3);
           }
    break;
    case enStats4 :
         if(Action==enGET)
           {
           StatsGet(HalDev, ParmValue, 4);
           }
    break;

*/

    default:
    rc = EC_CPMAC|EC_FUNC_OPTIONS|EC_VAL_KEY_NOT_FOUND;
    break;
  }

  /* Verify proper device state */
  if (HalDev->State == enOpened)
    switch (Update)
      {
      case 1 :
           ConfigApply(HalDev);
      break;
      default:
      break;
      }

  return (rc);
  }
static const char pszStats[]       = "Stats;";

static int halControl(HAL_DEVICE *HalDev, const char *pszKey, const char *pszAction, void *Value)
  {
  int i;
  int rc=0;
  int Action;
  int ActionFound;
  int KeyFound;

#ifdef __CPHAL_DEBUG
  if (DBG(1))
    {
    dbgPrintf("\nhalControl-HalDev:%08X,Action:%s,Key:%s\n", (bit32u)HalDev, pszAction, pszKey);
    }
#endif

  /* 23Aug04 - BCIL needs to set Mac Address  */
  if(HalDev->OsFunc->Strcmpi(pszKey, pszMacAddr) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
        unsigned char *MacAddr;  
        MacAddr = (unsigned char *) Value;  
        MacAddressSave(HalDev, MacAddr); 
        MacAddressSet(HalDev);       
        return(0);
      }
    else
      {
       return(-1);
      }
    }
     
  if(HalDev->OsFunc->Strcmpi(pszKey, hcLinked) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
        HalDev->Linked = *(int *)Value;
        return(0);
      }
    else
      {
       return(-1);
      }
    }
  
  if(HalDev->OsFunc->Strcmpi(pszKey, "TxIntDisable") == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
        HalDev->TxIntDisable = *(int *)Value;
        if(HalDev->TxIntDisable && (HalDev->State == enOpened))
          {
              /* if Opened and need TxIntDisabled, clear Ints for Channel 0 */
              CPMAC_TX_INTMASK_CLEAR(HalDev->dev_base) = 1;             
          }        
        return(0);
      }
    else
      {
       return(-1);
      }
    }

  if(HalDev->OsFunc->Strcmpi(pszKey, hcPhyAccess) == 0)
    {
    bit32u RegAddr;
    bit32u PhyNum;
    bit32u Data;
    bit32u ValueIn;

    ValueIn = *(bit32u*) Value;

    KeyFound=1;
    /* Cannot access MII if not opended */

    if(HalDev->State < enOpened)
      return(-1);

    if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
      {

      PhyNum  = (ValueIn & 0x1F);        /* Phynum 0-32 */
      RegAddr = (ValueIn >> 5) & 0xFF;   /* RegAddr in upper 11 bits */

      *(bit32u*)Value = _mdioUserAccessRead(HalDev->PhyDev, RegAddr, PhyNum);

      return(0);
      } /* end of hcGet */


    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
      PhyNum  = (ValueIn & 0x1F);        /* Phynum 0-32 */
      RegAddr = (ValueIn >> 5) & 0xFF;   /* RegAddr in upper 11 bits of lower 16 */

      Data    = ValueIn >> 16;           /* Data store in upper 16 bits */

      _mdioUserAccessWrite(HalDev->PhyDev, RegAddr, PhyNum, Data);
      return(0);
      }
    } /*  End of hcPhyAccess */

  if(HalDev->OsFunc->Strcmpi(pszKey, hcPhyNum) == 0)
    {
    KeyFound=1;
    if(!HalDev->Linked)
      return(-1);  /* if not linked the no Phy Connected */
    if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
      {
        *(int *)Value = HalDev->PhyNum;
      return(0);
      }
    }

  if(HalDev->OsFunc->Strcmpi(pszKey, hcCpmacSize) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
      {
        *(bit32u *)Value = HalDev->CpmacSize;
      return(0);
      }
    }

   if(HalDev->OsFunc->Strcmpi(pszKey, hcCpmacBase) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
      {
        *(int *)Value = HalDev->dev_base;
      return(0);
      }
    }

  if(HalDev->OsFunc->Strcmpi(pszKey, hcFullDuplex) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
        UPDATE_FULLDUPLEX(*(unsigned int *)Value);
        if(HalDev->State == enOpened)
           ConfigApply(HalDev);
      return(0);
      }
    else
      return(-1);
    }

  if(HalDev->OsFunc->Strcmpi(pszKey, pszDebug) == 0)
    {
    KeyFound=1;
    if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
      {
        ActionFound=1;
        HalDev->debug = *(int *)Value;
      }
    }

  if(HalDev->OsFunc->Strcmpi(pszKey, hcMaxFrags) == 0)
    {
     KeyFound=1;
     if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
       {
        ActionFound=1;

        if ((*(int *)Value) > 0)
          HalDev->MaxFrags = *(int *)Value;
         else
          rc = (EC_AAL5|EC_FUNC_CONTROL|EC_VAL_INVALID_VALUE);
      }

     if (HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
       {
        ActionFound=1;

        *(int *)Value = HalDev->MaxFrags;
       }
    }

  if(HalDev->OsFunc->Strstr(pszKey, pszStats) != 0)
    {
     KeyFound=1;
     if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
         {
       int Level;
       int Ch;
       char *TmpKey = (char *)pszKey;
       ActionFound=1;
       TmpKey += HalDev->OsFunc->Strlen(pszStats);
       Level   = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);
       TmpKey++;
       Ch      = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);
       TmpKey++;
       osfuncSioFlush();
#ifdef __CPHAL_DEBUG
  if (DBG(1))
    {
    dbgPrintf("\nhalControl-HalDev:%08X, Level:%d, Ch:%d\n", (bit32u)HalDev, Level, Ch);
    }
#endif
         StatsGet(HalDev, (void **)Value, Level, Ch, 0);
       osfuncSioFlush();
         }
    }


  if(HalDev->OsFunc->Strcmpi(pszAction, hcSet) == 0)
    Action = enSET;
  else
  if(HalDev->OsFunc->Strcmpi(pszAction, hcClear) == 0)
    Action = enCLEAR;
  else
  if(HalDev->OsFunc->Strcmpi(pszAction, hcGet) == 0)
    Action = enGET;
  else
    Action = enNULL;



  for(i=enCommonStart+1;i<enCommonEnd;i++)
    {
    if(HalDev->OsFunc->Strcmpi(KeyCommon[i].strKey, pszKey)==0)
      {
      rc = InfoAccess(HalDev, KeyCommon[i].enKey, Action, Value);
      }
    }
  for(i=enCpmacStart+1;i<enCpmacEnd;i++)
    {
    if(HalDev->OsFunc->Strcmpi(KeyCpmac[i].strKey, pszKey)==0)
      {
      rc = InfoAccess(HalDev, KeyCpmac[i].enKey, Action, Value);
      }
    }
/*
  if (KeyFound == 0)
    rc = (EC_MODULE|EC_FUNC_CONTROL|EC_VAL_KEY_NOT_FOUND);

  if (ActionFound == 0)
    rc = (EC_MODULE|EC_FUNC_CONTROL|EC_VAL_ACTION_NOT_FOUND);
*/

  return(rc);
  }
static bit32u ConfigGet(HAL_DEVICE *HalDev)
  {
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;
  char *DeviceInfo     = HalDev->DeviceInfo;
  int i                = HalDev->inst;
  bit32u Value;
  int Error;

  /* get the configuration parameters common to all modules */
  Error = ConfigGetCommon(HalDev);
  if (Error) return (EC_CPMAC|Error);

  if (HalDev->debug)
    {
      dbgPrintf("ConfigGet: haldev:0x%08X inst:%d base:0x%08X reset:%d\n", (bit32u) &HalDev, HalDev->inst, HalDev->dev_base, HalDev->ResetBit);
      osfuncSioFlush();
    }

  Error = OsFunc->DeviceFindParmUint(DeviceInfo, pszMdioConnect,&Value); /*MJH+030805*/
  if(!Error) HalDev->MdioConnect = Value;

  Error = OsFunc->DeviceFindParmUint(DeviceInfo, "PhyMask",&Value);
  if(!Error) HalDev->PhyMask = Value;

  Error = OsFunc->DeviceFindParmUint(DeviceInfo, "MLink",&Value);
  if(!Error) HalDev->MLinkMask = Value;

  Error = OsFunc->DeviceFindParmUint(DeviceInfo, hcMdixMask, &Value);
  if(!Error)
    HalDev->MdixMask = Value;
  else
    HalDev->MdixMask = 0;

  Error = OsFunc->DeviceFindParmUint(DeviceInfo, hcSize, &Value); /*MJH+030425*/
  if(!Error) HalDev->CpmacSize = Value;

  for(i=enCommonStart+1;i<enCommonEnd;i++)
    {
    Error = OsFunc->DeviceFindParmUint(DeviceInfo, KeyCommon[i].strKey, (bit32u*)&Value);
    if(!Error)
      {
      InfoAccess(HalDev, KeyCommon[i].enKey, enSET, (bit32u*)&Value);
      }
    }
  for(i=enCpmacStart+1;i<enCpmacEnd;i++)
    {
    Error = OsFunc->DeviceFindParmUint(DeviceInfo, KeyCpmac[i].strKey, (bit32u*)&Value);
    if(!Error)
      {
      InfoAccess(HalDev, KeyCpmac[i].enKey, enSET, (bit32u*)&Value);
      }
    }
   return (EC_NO_ERRORS);
  }


static void ConfigInit(HAL_DEVICE *HalDev)
  {
  if(HalDev->inst == 0)
    {
     HalDev->dev_base  = 0xA8610000;
     HalDev->ResetBit  = 17;
     HalDev->interrupt = 19;
     HalDev->MLinkMask = 0;
     HalDev->PhyMask   = 0xAAAAAAAA;
    }
  else
    {
     HalDev->dev_base  = 0xA8612800;
     HalDev->ResetBit  = 21;
     HalDev->interrupt = 33;                                         /*~RC3.02*/
     HalDev->MLinkMask = 0;
     HalDev->PhyMask   = 0x55555555;
    }
  HalDev->RxMaxLen    = 1518;
  HalDev->MaxFrags    = 2;
  HalDev->MdioConnect = _CPMDIO_HD|_CPMDIO_FD|_CPMDIO_10|_CPMDIO_100|_CPMDIO_AUTOMDIX;
  HalDev->debug=0xFFFFFFFF;
  HalDev->debug=0;
  }
/* Shuts down the EMAC device
 *
 *@param HalDev EMAC instance. This was returned by halOpen()
 *@param mode Indicates actions to tak on close.
 <br>
 *PARTIAL - Disable EMAC
 <br>
 *FULL    - Disable EMAC and call OS to free all allocated memory
 *
 *@retval
 *        0 OK
 <br>
 *        Non-Zero Not OK
 *
 */
static int halInit( HAL_DEVICE *HalDev)
  {
   int rc;

  /* Verify proper device state */
  if (HalDev->State != enDevFound)
    return(EC_CPMAC|EC_FUNC_HAL_INIT|EC_VAL_INVALID_STATE);

  /* Configure HAL defaults */
  ConfigInit(HalDev);

  /* Retrieve HAL configuration parameters from data store */
  rc = ConfigGet(HalDev);
  if (rc) return (rc);

  /* Updated 030403*/
  rc = HalDev->OsFunc->Control(HalDev->OsDev, hcCpuFrequency, hcGet, &HalDev->CpuFrequency); /*MJH+030403*/
  if(rc)
    HalDev->CpuFrequency = 20000000;  /*20 Mhz default */         /*MJH+030403*/

  rc = HalDev->OsFunc->Control(HalDev->OsDev, hcCpmacFrequency, hcGet, &HalDev->CpmacFrequency); /*MJH+030331*/
  if(rc)
    HalDev->CpmacFrequency = HalDev->CpuFrequency/2;               /*MJH~030404*/

  rc = HalDev->OsFunc->Control(HalDev->OsDev, hcMdioBusFrequency,  hcGet, &HalDev->MdioBusFrequency);  /*MJH+030402*/
  if(rc)
    HalDev->MdioBusFrequency = HalDev->CpmacFrequency;

  rc = HalDev->OsFunc->Control(HalDev->OsDev, hcMdioClockFrequency,  hcGet, &HalDev->MdioClockFrequency);  /*MJH+030402*/
  if(rc)
    HalDev->MdioClockFrequency = 2200000; /* 2.2 Mhz PITS #14 */


  /* update device state */
  HalDev->State = enInitialized;

  /* initialize statistics */
  StatsInit(HalDev);                                                  /* +RC3.02 */

  /*                                                                  -RC3.02
  StatsTable3[0].StatPtr  = &HalDev->ChData[0].RxBufSize;
  StatsTable3[1].StatPtr  = &HalDev->ChData[0].RxBufferOffset;
  StatsTable3[2].StatPtr  = &HalDev->ChData[0].RxNumBuffers;
  StatsTable3[3].StatPtr  = &HalDev->ChData[0].RxServiceMax;
  StatsTable3[4].StatPtr  = &HalDev->ChData[0].TxNumBuffers;
  StatsTable3[5].StatPtr  = &HalDev->ChData[0].TxNumQueues;
  StatsTable3[6].StatPtr  = &HalDev->ChData[0].TxServiceMax;
  */

  return(EC_NO_ERRORS);
  }
static int halProbe(HAL_DEVICE *HalDev)
  {
  int inst             = HalDev->inst;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;
  int error_code;

  if (HalDev->State != enConnected)
     return (EC_CPMAC|EC_FUNC_PROBE|EC_VAL_INVALID_STATE);

  if(HalDev->debug) dbgPrintf("halProbe: %d ",inst);

  error_code = OsFunc->DeviceFindInfo(inst,"cpmac",&HalDev->DeviceInfo);

  if(error_code)
     return (EC_CPMAC|EC_FUNC_PROBE|EC_VAL_DEVICE_NOT_FOUND );

  /* Set device state to DevFound */
  HalDev->State = enDevFound;
  return(EC_NO_ERRORS);
  }
static void ChannelConfigInit(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
  int Ch        = HalChn->Channel;
  int Direction = HalChn->Direction;
  int nTxBuffers = 256;

  if (Direction == DIRECTION_TX)
    {
    HalDev->ChData[Ch].TxNumBuffers   = nTxBuffers;
    HalDev->ChData[Ch].TxNumQueues    = 1;
    HalDev->ChData[Ch].TxServiceMax   = nTxBuffers/3;
    HalDev->TxIntThreshold[Ch]        = HalDev->ChData[Ch].TxServiceMax;
    HalDev->TxIntThresholdMaster[Ch]  = HalDev->TxIntThreshold[Ch];
    }

  if (Direction == DIRECTION_RX)
    {
    HalDev->ChData[Ch].RxNumBuffers   = nTxBuffers*2;
    HalDev->ChData[Ch].RxBufferOffset = 0;
    HalDev->ChData[Ch].RxBufSize      = 1518;
    HalDev->ChData[Ch].RxServiceMax   = nTxBuffers/3; /*Not a typo*/
    }
  }
static int ChannelConfigApply(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
   int Ch        = HalChn->Channel;
   int Direction = HalChn->Direction;

   if (DBG(11))
     {
      dbgPrintf("halChannelConfigApply[%d:%d] haldev:0x%08X inst:%d base:0x%08X reset:%d\n", Ch, Direction, (bit32u) &HalDev, HalDev->inst, HalDev->dev_base, HalDev->ResetBit);
      osfuncSioFlush();
     }

   if (Direction == DIRECTION_TX)
   {
     if (HalDev->ChIsOpen[Ch][Direction] == TRUE)
     {
      return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_TX_CH_ALREADY_OPEN);
     }

     /* Initialize Queue Data */
     HalDev->TxActQueueHead[Ch][0]  = 0;
     HalDev->TxActQueueCount[Ch][0] = 0;
     HalDev->TxActive[Ch][0]        = FALSE;

     /* Need to use a macro that takes channel as input */
     CPMAC_TX0_HDP(HalDev->dev_base)=0;

     /* Initialize buffer memory for the channel */
     InitTcb(HalDev, Ch);

     if(!HalDev->TxIntDisable)
       CPMAC_TX_INTMASK_SET(HalDev->dev_base) = (1<<Ch); /* GSG 11/22 */
   }
   else
   {
     if (HalDev->ChIsOpen[Ch][Direction] == TRUE)
     {
      return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_RX_CH_ALREADY_OPEN);
     }

     /* Initialize Queue Data */
     HalDev->RxActQueueHead[Ch]     = 0;
     HalDev->RxActQueueCount[Ch]    = 0;

     HalDev->RxActive[Ch]           = FALSE;

     /* Need to use a macro that takes channel as input */
     CPMAC_RX0_HDP(HalDev->dev_base)=0;

     /* Initialize buffer memory for the channel */
     InitRcb(HalDev, Ch);

     CPMAC_RX_INTMASK_SET(HalDev->dev_base)  = (1<<Ch); /* GSG 11/22 */
   }

   HalDev->ChIsOpen[Ch][Direction] = TRUE; /* channel is open */

   return (EC_NO_ERRORS);
  }

/* GSG 11/22
 * Retrieves channel parameters from configuration file.  Any parameters
 * which are not found are ignored, and the HAL default value will apply,
 * unless a new value is given through the channel structure in the call
 * to ChannelSetup.
 */
static int ChannelConfigGet(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
  int Ch        = HalChn->Channel;
  int Direction = HalChn->Direction;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;
  unsigned int rc, Value;
  void *ChInfo;

   rc=OsFunc->DeviceFindParmValue(HalDev->DeviceInfo, channel_names[Ch], &ChInfo);
   /*  Do not fail if Channel Info not available for RC2 */
   if (rc) return(0);
/*   if (rc) return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_CH_INFO_NOT_FOUND);*/

   /* i don't care if a value is not found because they are optional */
   if(Direction == DIRECTION_TX)
     {
     rc=OsFunc->DeviceFindParmUint(ChInfo, "TxNumBuffers", &Value);
     if (!rc) HalDev->ChData[Ch].TxNumBuffers = Value;

     /*rc=OsFunc->DeviceFindParmUint(ChInfo, "TxNumQueues", &Value);*/ /*MJH-030329*/
     /*if (!rc) HalDev->ChData[Ch].TxNumQueues = Value;*/              /*MJH-030329*/

     rc=OsFunc->DeviceFindParmUint(ChInfo, "TxServiceMax", &Value);
     if (!rc) 
       {
         HalDev->ChData[Ch].TxServiceMax = Value;
         HalDev->TxIntThreshold[Ch]      = HalDev->ChData[Ch].TxServiceMax;
         HalDev->TxIntThresholdMaster[Ch]  = HalDev->TxIntThreshold[Ch];
       }
     }
   if(Direction == DIRECTION_RX)
     {
     rc=OsFunc->DeviceFindParmUint(ChInfo, "RxNumBuffers", &Value);
     if (!rc) HalDev->ChData[Ch].RxNumBuffers = Value;

     rc=OsFunc->DeviceFindParmUint(ChInfo, "RxBufferOffset", &Value);
     if (!rc) HalDev->ChData[Ch].RxBufferOffset = Value;

     rc=OsFunc->DeviceFindParmUint(ChInfo, "RxBufSize", &Value);
     if (!rc) HalDev->ChData[Ch].RxBufSize = Value;

     rc=OsFunc->DeviceFindParmUint(ChInfo, "RxServiceMax", &Value);
     if (!rc) HalDev->ChData[Ch].RxServiceMax = Value;
     }
   return (EC_NO_ERRORS);
  }
#define ChannelUpdate(Field) if(HalChn->Field != 0xFFFFFFFF) HalDev->ChData[Ch].Field = HalChn->Field

static void ChannelConfigUpdate(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
  int Ch        = HalChn->Channel;
  int Direction = HalChn->Direction;
#ifdef __CPHAL_DEBUG
  if (DBG(1))
    {
    dbgPrintf("\nChnUpd-HalDev:%08X,Chn:%d:%d\n", (bit32u)HalDev, Ch, Direction); osfuncSioFlush();
    }
#endif
  if (Direction == DIRECTION_TX)
    {
    ChannelUpdate(TxNumBuffers);
    /*ChannelUpdate(TxNumQueues);*/                               /*MJH~030329*/
    ChannelUpdate(TxServiceMax);
    HalDev->TxIntThreshold[Ch]        = HalDev->ChData[Ch].TxServiceMax;
    HalDev->TxIntThresholdMaster[Ch]  = HalDev->TxIntThreshold[Ch];
    }
  else
  if (Direction == DIRECTION_RX)
    {
    ChannelUpdate(RxBufferOffset);
    ChannelUpdate(RxBufSize);
    ChannelUpdate(RxNumBuffers);
    ChannelUpdate(RxServiceMax);
#ifdef __CPHAL_DEBUG
  if (DBG(1))
    {
    dbgPrintf("\nRxNumBuffers %d\n",HalChn->RxNumBuffers); osfuncSioFlush();
    }
#endif
    }
  }
static int halChannelSetup(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn, OS_SETUP *OsSetup)
  {
  int Direction;
  int Ch;
  int rc;

  /* Verify proper device state */
  if (HalDev->State < enInitialized)
    return (EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_INVALID_STATE);

  /* We require the channel structure to be passed, even if it only contains
     the channel number */
  if (HalChn == NULL)
    {
     return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_NULL_CH_STRUCT);
    }

  Ch        = HalChn->Channel;
  Direction = HalChn->Direction;

  /* This should check on Maximum Channels for RX or TX,
     they might be different Mick 021124 */
  if ((Ch < 0) || (Ch > (MAX_CHAN-1)))
    {
     return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_INVALID_CH);
    }

  /* if channel is already open, this call is invalid */
  if (HalDev->ChIsOpen[Ch][Direction] == TRUE)
    {
     return(EC_CPMAC|EC_FUNC_CHSETUP|EC_VAL_CH_ALREADY_OPEN);
    }

  /* channel is closed, but might be setup.  If so, reopen the hardware channel. */
  if (HalDev->ChIsSetup[Ch][Direction] == FALSE)
    {
     /* Setup channel configuration */
     HalDev->ChData[Ch].Channel = Ch;

     /* Store OS_SETUP */
     HalDev->ChData[Ch].OsSetup = OsSetup;

     /*  Framework :
         Set Default Values
         Update with options.conf
     Apply driver updates
     */
     ChannelConfigInit(HalDev, HalChn);
     ChannelConfigGet(HalDev, HalChn);
     ChannelConfigUpdate(HalDev, HalChn);

     /* cppi.c needs to use Rx/TxServiceMax */
     HalDev->BuffersServicedMax  = 169;  /* TEMP */

     HalDev->ChIsSetup[Ch][Direction] = TRUE;
    }

  rc = EC_NO_ERRORS;

  /* If the hardware has been opened (is out of reset), then configure the channel
     in the hardware. NOTE: ChannelConfigApply calls the CPSAR ChannelSetup()! */
  if (HalDev->State == enOpened)
    {
     rc = ChannelConfigApply(HalDev, HalChn);
    }

  return (rc);
  }


static int miiInfoGet(HAL_DEVICE *HalDev, bit32u *miiBaseAddress, bit32u *miiResetBit)
  {
  int rc;
  void *DeviceInfo;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;

  /*  Only one instance of cpmdio   */
  rc = OsFunc->DeviceFindInfo(0,"cpmdio",&DeviceInfo);               /*~RC3.02*/

  if(rc)
     return (EC_DEV_CPMDIO|EC_FUNC_OPEN|EC_VAL_DEVICE_NOT_FOUND );

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "base",miiBaseAddress);
  if(rc)
      rc=EC_DEV_CPMDIO|EC_FUNC_OPEN|EC_VAL_NO_BASE;

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "reset_bit",miiResetBit);
  if(rc)
      rc=EC_DEV_CPMDIO|EC_FUNC_OPEN|EC_VAL_NO_BASE;


  /* See if need to make mdio functional in GPIO */
  gpioCheck(HalDev, DeviceInfo);

  if(DBG(0))
    dbgPrintf("miiBase: 0x%08X %u\n", *miiBaseAddress, *miiResetBit);
  return(rc);
  }
static void ephyCheck(HAL_DEVICE *HalDev)
  {                                                                  /*+RC3.02*/
  int rc;
  void *DeviceInfo;
  int mii_phy;
  int reset_bit;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;

  rc = OsFunc->DeviceFindInfo(0,"ephy",&DeviceInfo);
  if(rc) return;

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "mii_phy",&mii_phy);
  if(rc) return;

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "reset_bit",&reset_bit);
  if(rc) return;

  if (HalDev->PhyMask & (1 << mii_phy))
    {
    *(volatile bit32u *)(HalDev->ResetBase) |=  (1 << reset_bit);                      /*+RC3.02*/
    resetWait(HalDev);
    }
  }                                                                  /*+RC3.02*/
static void AutoNegotiate(HAL_DEVICE *HalDev)
  {
  int size;
  bit32u ModID,  RevMaj,  RevMin;
  PHY_DEVICE *PhyDev;
  bit32u miiBaseAddress;
  bit32u miiResetBit;

  /* Verify proper device state */
  if (HalDev->State < enOpened)
    return;

  miiInfoGet(HalDev, &miiBaseAddress, &miiResetBit);

  cpMacMdioGetVer(miiBaseAddress, &ModID,  &RevMaj,  &RevMin);
  if(HalDev->debug)
     dbgPrintf("Mdio Module Id %d, Version %d.%d\n", ModID,  RevMaj,  RevMin);

  size = cpMacMdioGetPhyDevSize();
  PhyDev = (PHY_DEVICE *) HalDev->OsFunc->Malloc( size );

  HalDev->PhyDev = PhyDev;

  ephyCheck(HalDev);

  cpMacMdioInit( PhyDev, miiBaseAddress, HalDev->inst, HalDev->PhyMask, HalDev->MLinkMask, HalDev->MdixMask, HalDev->ResetBase, miiResetBit, HalDev->MdioBusFrequency, HalDev->MdioClockFrequency, HalDev->debug, HalDev); /*MJH~030402*/
  MdioSetPhyMode(HalDev);

  return;
  }
static int halOpen(HAL_DEVICE *HalDev)
  {
  unsigned char *MacAddr;
  int i;
  int j;
  int rc, Ticks;

  if (HalDev->debug)
    {
      dbgPrintf("halOpen: haldev:0x%08X inst:%d base:0x%08X reset:%d\n", (bit32u) &HalDev, HalDev->inst, HalDev->dev_base, HalDev->ResetBit);
      osfuncSioFlush();
    }

   /* Verify proper device state */
   if (HalDev->State < enInitialized)
     return (EC_CPMAC|EC_FUNC_OPEN|EC_VAL_INVALID_STATE);


  /* take CPMAC out of reset - GSG 11/20*/
  if ((VOLATILE32(HalDev->ResetBase) & (1 << HalDev->ResetBit)) != 0)
    {
     /* perform normal close duties */
     CPMAC_MACCONTROL(HalDev->dev_base) &= ~MII_EN;
     CPMAC_TX_CONTROL(HalDev->dev_base) &= ~TX_EN;
     CPMAC_RX_CONTROL(HalDev->dev_base) &= ~RX_EN;

     /* disable interrupt masks */
     CPMAC_TX_INTMASK_CLEAR(HalDev->dev_base) = 0xFF;
     CPMAC_RX_INTMASK_CLEAR(HalDev->dev_base) = 0xFF;
    }

   /* take CPMAC out of reset */
   *(volatile bit32u *)(HalDev->ResetBase) &= ~(1 << HalDev->ResetBit);
   resetWait(HalDev);
   *(volatile bit32u *)(HalDev->ResetBase) |=  (1 << HalDev->ResetBit);
   resetWait(HalDev);

  /* After Reset clear the Transmit and Receive DMA Head Descriptor Pointers */

  CPMAC_TX0_HDP(HalDev->dev_base)=0;
  CPMAC_TX1_HDP(HalDev->dev_base)=0;
  CPMAC_TX2_HDP(HalDev->dev_base)=0;
  CPMAC_TX3_HDP(HalDev->dev_base)=0;
  CPMAC_TX4_HDP(HalDev->dev_base)=0;
  CPMAC_TX5_HDP(HalDev->dev_base)=0;
  CPMAC_TX6_HDP(HalDev->dev_base)=0;
  CPMAC_TX7_HDP(HalDev->dev_base)=0;

  /* Rx Init  */

  CPMAC_RX0_HDP(HalDev->dev_base) = 0;
  CPMAC_RX1_HDP(HalDev->dev_base) = 0;
  CPMAC_RX2_HDP(HalDev->dev_base) = 0;
  CPMAC_RX3_HDP(HalDev->dev_base) = 0;
  CPMAC_RX4_HDP(HalDev->dev_base) = 0;
  CPMAC_RX5_HDP(HalDev->dev_base) = 0;
  CPMAC_RX6_HDP(HalDev->dev_base) = 0;
  CPMAC_RX7_HDP(HalDev->dev_base) = 0;

  CPMAC_RX_BUFFER_OFFSET(HalDev->dev_base) = 0;

  /* Init Tx and Rx DMA */

  CPMAC_TX_CONTROL(HalDev->dev_base) |= TX_EN;
  CPMAC_RX_CONTROL(HalDev->dev_base) |= RX_EN;

  CPMAC_MAC_INTMASK_SET(HalDev->dev_base) |=2; /* Enable Adaptercheck Ints */
  HalDev->OsFunc->Control(HalDev->OsDev, pszMacAddr, hcGet, &MacAddr); /* GSG 11/22 */
  MacAddressSave(HalDev, MacAddr);

  HalDev->HostErr = 0;      /* Clear Adapter Check indicator */
  HalDev->State = enOpened; /* Change device state */

  /* Start MDIO Negotiation */
  AutoNegotiate(HalDev);

  /*  Enable the Os Timer  */
  Ticks = HalDev->CpuFrequency / 100;  /*  10 milli-secs */       /*MJH~030402*/
  HalDev->OsFunc->Control(HalDev->OsDev, pszTick, hcSet, &Ticks); /* GSG 11/22 */
  HalDev->OsFunc->IsrRegister(HalDev->OsDev, halIsr, HalDev->interrupt);

  /* GSG +030523 Malloc space for the Rx fraglist */
  HalDev->fraglist = HalDev->OsFunc->Malloc(HalDev->MaxFrags * sizeof(FRAGLIST));

  /* Any pre-open configuration */

  /* For any channels that have been pre-initialized, set them up now */
  /* Note : This loop should not use MAX_CHN, it should only
            loop through Channels Setup, memory should not be reserved
            until Channel is Setup
  */
  for(i=0; i<MAX_CHAN; i++) /* i loops through Channels */
  for(j=0; j<2; j++)        /* j loops through DIRECTION values, 0 and 1  */
    {
     if(HalDev->ChIsSetup[i][j]==TRUE)   /* If the Channel and Direction have been Setup */
       if(HalDev->ChIsOpen[i][j]==FALSE) /* but not opened, then Apply Values now */
         {
          CHANNEL_INFO HalChn;
          HalChn.Channel   = i;
          HalChn.Direction = j;
          rc = ChannelConfigApply(HalDev, &HalChn);
          if(rc != EC_NO_ERRORS)
            return(rc);
         }
    }  /* End of looping through Channel/Direction */

  ConfigApply(HalDev);  /* Apply Configuration Values to Device */
  CPMAC_MACCONTROL(HalDev->dev_base) |= MII_EN;  /* MAC_EN */
  if(DBG(0))
     dbgPrintf("[halOpen]MacControl:%08X\n", CPMAC_MACCONTROL(HalDev->dev_base));
  return(EC_NO_ERRORS);
  }

#define INT_PENDING (MAC_IN_VECTOR_TX_INT_OR | MAC_IN_VECTOR_RX_INT_OR | MAC_IN_VECTOR_HOST_INT)
static int halShutdown(HAL_DEVICE *HalDev)
  {
   int Ch, Queue;                                                  /*GSG+030514*/

   /* Verify proper device state */
   if (HalDev->State == enOpened)
     halClose(HalDev, 3); /* GSG ~030429 */

   /* Buffer/descriptor resources may still need to be freed if a Close
      Mode 1 was performed prior to Shutdown - clean up here */    /*GSG+030514*/
   for (Ch=0; Ch<MAX_CHAN; Ch++)
     {
      if (HalDev->RcbStart[Ch] != 0)
        FreeRx(HalDev,Ch);

      for(Queue=0; Queue<MAX_QUEUE; Queue++)
        {
         if (HalDev->TcbStart[Ch][Queue] != 0)
           FreeTx(HalDev,Ch,Queue);
        }
     }

   /* free the HalFunc */
   HalDev->OsFunc->Free(HalDev->HalFuncPtr);

   /* free the HAL device */
   HalDev->OsFunc->Free(HalDev);

   return(EC_NO_ERRORS);
  }
int halIsr(HAL_DEVICE *HalDev, int *MorePackets)
{
  bit32u IntVec;
  int Serviced;
  int PacketsServiced=0;
  int Channel;
  int TxMorePackets=0;
  int RxMorePackets=0;

  /* Verify proper device state - important because a call prior to Open would
      result in a lockup */
  if (HalDev->State != enOpened)
     return(EC_CPMAC|EC_FUNC_DEVICE_INT|EC_VAL_INVALID_STATE);

  IntVec = CPMAC_MAC_IN_VECTOR(HalDev->dev_base);

#ifdef __CPHAL_DEBUG
  if (DBG(0))
    {
    dbgPrintf("\nhalIsr: inst %d, IntVec 0x%X\n", HalDev->inst, IntVec); osfuncSioFlush();/* GSG 11/22 */
    }
#endif

  HalDev->IntVec = IntVec;
  if (IntVec & MAC_IN_VECTOR_TX_INT_OR)
   {
     int TxServiceMax=0;  /* Compiler complains if not initialized */
      
     Channel = (IntVec & 0x7);
     
     if(HalDev->TxIntDisable)
       {
         CPMAC_TX_INTMASK_CLEAR(HalDev->dev_base) = (1<<Channel);  /* Disable Interrupt for Channel */           
         TxServiceMax = HalDev->ChData[Channel].TxServiceMax;  
         HalDev->ChData[Channel].TxServiceMax = 10000;             /* Need to service all packets in the Queue */
       }      
     
     PacketsServiced |= TxInt(HalDev, Channel, 0, &TxMorePackets);
     
     if(HalDev->TxIntDisable)        
          HalDev->ChData[Channel].TxServiceMax  = TxServiceMax;         
    }
  
  if (IntVec & MAC_IN_VECTOR_RX_INT_OR)
    {
     Channel = (IntVec >> 8) & 0x7;
     Serviced = RxInt(HalDev, Channel, &RxMorePackets);
     PacketsServiced |= (Serviced<<16);
    }

  if (IntVec & MAC_IN_VECTOR_HOST_INT)
    {
    /* Adaptercheck */
    HalDev->HostErr = 1;
    HalDev->MacStatus = CPMAC_MACSTATUS(HalDev->dev_base);
    osfuncStateChange();                                          /*MJH+030328*/
    if(DBG(0))
      {
      dbgPrintf("Adaptercheck: %08x for base:%X\n",HalDev->MacStatus, (bit32u)HalDev->dev_base);
      osfuncSioFlush();
      }
    }
  *MorePackets = (TxMorePackets | RxMorePackets);
  return (PacketsServiced);
}

int halPacketProcessEnd(HAL_DEVICE *HalDev)
{
  int base             = HalDev->dev_base;
  CPMAC_MAC_EOI_VECTOR(base) = 0;
  return(0);
}



static int PhyCheck(HAL_DEVICE *HalDev)
  {
      return(cpMacMdioTic(HalDev->PhyDev));
  }
static int halTick(HAL_DEVICE *HalDev)
{
    int TickChange;

    if(HalDev->State <  enOpened)
      return (EC_CPMAC|EC_FUNC_TICK|EC_VAL_INVALID_STATE);

    /* if NO Phy no need to check Link */
    if(HalDev->MdioConnect & _CPMDIO_NOPHY)
      return(EC_NO_ERRORS);  /* No change in Phy State detected */

    TickChange = PhyCheck(HalDev);
    /* Phy State Change Detected */
    if(TickChange == 1)
    {
        /*  MDIO indicated a change  */
        DuplexUpdate(HalDev);
        osfuncStateChange();
        return(EC_NO_ERRORS);
    }

    /* if in AutoMdix mode, and Flip request received, inform OS */
    if( (HalDev->MdioConnect & _CPMDIO_AUTOMDIX)  &&
        (TickChange & _MIIMDIO_MDIXFLIP))
    {
       bit32u Mdix;
       Mdix = TickChange & 0x1;   /*  Mdix mode stored in bit 0 */
       HalDev->OsFunc->Control(HalDev->OsDev, hcMdioMdixSwitch, hcSet, &Mdix);
       return(EC_NO_ERRORS);
    }

    return(EC_NO_ERRORS);
}

int halCpmacInitModule(HAL_DEVICE **pHalDev, OS_DEVICE *OsDev, HAL_FUNCTIONS **pHalFunc,
                       OS_FUNCTIONS *OsFunc, int OsFuncSize,     int *HalFuncSize, int Inst)
  {
    HAL_DEVICE *HalDev;
    HAL_FUNCTIONS *HalFunc;

    if (OsFuncSize < sizeof(OS_FUNCTIONS))
      return (EC_CPMAC|EC_FUNC_HAL_INIT|EC_VAL_OS_VERSION_NOT_SUPPORTED);

    HalDev = (HAL_DEVICE *) OsFunc->MallocDev(sizeof(HAL_DEVICE));
    if (!HalDev)
      return (EC_CPMAC|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_DEV_FAILED);

    /* clear the HalDev area */
    OsFunc->Memset(HalDev, 0, sizeof(HAL_DEVICE));

    /* Initialize the size of hal functions */
    *HalFuncSize = sizeof (HAL_FUNCTIONS);

    HalFunc = (HAL_FUNCTIONS *) OsFunc->Malloc(sizeof(HAL_FUNCTIONS));
    if (!HalFunc)
      return (EC_CPMAC|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_FAILED);

    /* clear the function pointers */
    OsFunc->Memset(HalFunc, 0, sizeof(HAL_FUNCTIONS));

    HalDev->OsDev   = OsDev;
    HalDev->OsOpen  = OsDev;
    HalDev->inst    = Inst;
    HalDev->OsFunc  = OsFunc;
    HalDev->HalFunc = HalFunc;
    /* Remove the following from cppi, replace with HalFunc */
    HalDev->HalFuncPtr = HalFunc; /* GSG 11/20 changed name to match cppi */

    /****************************************************************/
    /*                 POPULATE HALFUNC                             */
    /****************************************************************/
    HalFunc->ChannelSetup     = halChannelSetup;
    HalFunc->ChannelTeardown  = halChannelTeardown; /* GSG 11/20 */
    HalFunc->Close            = halClose; /* GSG 11/20 */
    HalFunc->Control          = halControl; /* GSG 11/22 */
    HalFunc->Init             = halInit;
    HalFunc->Open             = halOpen;
    HalFunc->PacketProcessEnd = halPacketProcessEnd;
    HalFunc->Probe            = halProbe;
    HalFunc->RxReturn         = halRxReturn;
    HalFunc->Send             = halSend;
    HalFunc->Shutdown         = halShutdown;
    HalFunc->Tick             = halTick;

    /* HalFunc->Status        = halStatus;*/ /* GSG 11/22 */
    /* pass the HalDev and HalFunc back to the caller */

    *pHalDev  = HalDev;
    *pHalFunc = HalFunc;

    HalDev->State = enConnected;   /* Initialize the hardware state */

    if (HalDev->debug) HalDev->OsFunc->Printf("halCpmacInitModule: Leave\n");
    return(0);
  }

int cpmacRandomRange(HAL_DEVICE *HalDev, int min, int max)
{
    int iTmp;
    iTmp  = cpmacRandom(HalDev);
    iTmp %= ((max-min)+1);
    iTmp += min;
    return(iTmp);
}

int cpmacRandom(HAL_DEVICE *HalDev)
{
    int iTmp;
    iTmp = CPMAC_BOFFTEST(HalDev->dev_base);
    iTmp >>= 16;      /*  get rndnum field */
    iTmp &= (0x3FF); /* field is 10 bits wide */
    return(iTmp);
}