/*****************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002,2003 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE:
 *
 *  DESCRIPTION:
 *      This file contains documentation for the CPMAC
 *
 *  HISTORY:
 *  @author Michael Hanrahan/Greg Guyotte
 *  @version 1.00
 *  @date    03-Dec-2002
 *****************************************************************************/
#ifndef _DOX_CPMAC_H
#define _DOX_CPMAC_H
/**
@page CPMAC_Implementation_Details Version

@copydoc CPMAC_Version
*/

/**
@page cpmac_intro Introduction

The CPMAC implementation will support 8 channels for transmit and 8 channel for
receive.  Each of the 8 transmit channels has 1 queue associated with it.  It is
recommended that only 1 channel is used for @c Receive() per processor.
*/

/**
@page cpmac_details API Implementation Details
@par osReceive
@p Mode parameter
- The Upper 16 bits of Mode match Word 3 of the Rx Buffer Descriptor

@par halSend
@p Mode parameter
-  Bits 0-7 contain the Channel Number
-  Bits 8-25 are reserved
-  Bit  26 - if 0, the CRC will be calculated, if 1 the CRC will be Passed
-  Bits 27-31 : reserved
@section cpmac_keys Control Keys

@par StateChange
CPHAL calls the OS when a state change is detected.
OS should check the CPMAC Status. See the Control Key 'Status' for more details.

@par Status
OS calls the CPHAL to obtain Status information. The Returned status is as follows

@par MaxFrags
The OS may "Set" or "Get" this value.  This defines the maximum
number of fragments that can be received by the CPMAC Rx port.  The default
value for CPMAC is 2.  This provides enough space to receive a maximum
length packet (1,518 bytes) with the default buffer size of 1518 and any
amount of RxBufferOffset.  If the buffer size is configured to be smaller,
the OS *MUST* modify this parameter according to the following formula:
((System Max packet length)/(RxBufSize)) + 1.  (The extra 1 fragment is to
allow for RxBufferOffset)

@code
// Following defined in "cpswhal_cpmac.h"
// CPMAC CPHAL STATUS
#define CPMAC_STATUS_LINK               (1 << 0)
#define CPMAC_STATUS_LINK_DUPLEX        (1 << 1)  // 0 - HD, 1 - FD
#define CPMAC_STATUS_LINK_SPEED         (1 << 2)  // 0 - 10, 1 - 100

// ADAPTER CHECK Codes
#define CPMAC_STATUS_ADAPTER_CHECK             (1 << 7)
#define CPMAC_STATUS_HOST_ERR_DIRECTION        (1 << 8)    // 0 - Tx, 1 - Rx
#define CPMAC_STATUS_HOST_ERR_CODE             (0xF << 9)  See CPMAC Guide
#define CPMAC_STATUS_HOST_ERR_CH               (0x7 << 13) See CPMAC Guide
@endcode

@code
void osStateChange(OS_DEVICE *OsDev)
  {
  int status;
  OsDev->HalFunc->Control(OsDev->HalDev, "Status", hcGet, &status);
  if(status & CPMAC_STATUS_ADAPTER_CHECK)
    {
    printf("[osStateChange[%d]]  HAL notified OS of AdapterCheck (Link Status 0x%08X)\n", OsDev->port, status);
    adaptercheck(OsDev->port);
    }
  else
    {
    printf("[osStateChange[%d]]  HAL notified OS of State Change (Link Status %s)\n", OsDev->port, (status & CPMAC_STATUS_LINK) ? "Up" : "Down");
    if(status & CPMAC_STATUS_LINK)
      {
      printf("Speed %s, Duplex %s\n",
      status & CPMAC_STATUS_LINK_SPEED ? "100" : "10",
      status & CPMAC_STATUS_LINK_DUPLEX ? "FD" : "HD");
      }
    }
@endcode

@par Tick
     The CPHAL calls the OS to set the interval for calling halTick()<BR>
     Note: Predefined value hcTick now recommended for use.
@code
***  Example Code ***

*** CPHAL code ***
int Ticks;
HalDev->OsFunc->Control(HalDev->OsDev, hcTick, hcSet, &Ticks);

*** OS code ***
  ..
  if(osStrcmpi(pszKey, hcTick) == 0)
  {
    if(osStrcmpi(pszAction, hcSet) == 0)
    {
      // Enable the Tick Interval
      if(*(unsigned int *) ParmValue)
        printf("osTickSet: Interval = %d ticks\n", Interval);
    }
    else
    if(osStrcmpi(pszAction, hcClear) == 0)
    {
        // Request disabling of the Tick Timer, ParmValue is ignored
    }
  }
@endcode

@par The following information can be obtained by the OS via 'Get'

- StatsDump : OS supplies pointer to an 36 element unsigned int array
CPHAL will populate the array with the current Statistics values.<BR>
Note: all hcXXXX values are predefined and should be used by the OS.

- hcPhyNum : Returns the PHY number.
- hcCpmacBase : Returns the base-address of the CPMAC device
- hcCpmacSize : Returns size of the CPMAC memory map


@par Phy Register Communication

halControl() is used to read and write the Phy Registers via the key hcPhyAccess

Both reading and writing the Phy registers involve setting the Value parameter of halControl()
<BR>
Value is a 32-bit value with bits partioned as follows
<BR>

   0 -  4  Phy Number   <BR>
   5 -  9  Phy Register <BR>
  10 - 15  reserved     <BR>
  16 - 31  Data  (write only)
<BR>


<B>Reading the Phy register</B>

@code
    bit32u Value;
    bit32u RegAddr;
    bit32u PhyNum;
    bit32u PhyRegisterData;

    //  Read Phy 31, register 20

    PhyNum  = 31;
    RegAddr = 20;

    Value = (RegAddr << 5);
    Value |= (PhyNum & 0x1F);

    rc  = HalFunc->Control(HalDev, hcPhyAccess, hcGet, (bit32u *) &Value)
    If(rc == 0)
    {
        // Value is overwriten with the value in Register 20 of Phy number 31.
        PhyRegisterData = Value;
    }
@endcode

<B>Writing the Phy register</B>
@code
    bit32u Value;
    bit32u RegAddr;
    bit32u PhyNum;
    bit32u PhyRegisterData;

    //  Reset Phy 23

    PhyNum  = 23;
    RegAddr = 0;
    PhyRegisterData = 0x8000;  // Reset bit set

    Value = (RegAddr << 5);
    Value |= (PhyNum & 0x1F);
    Value |= (PhyRegisterData << 16);

    rc  = HalFunc->Control(HalDev, hcPhyAccess, hcSet, (bit32u *) &Value)

    // Check is reset if done

    PhyNum  = 23;
    RegAddr = 0;

    Value = (RegAddr << 5);
    Value |= (PhyNum & 0x1F);

    rc  = HalFunc->Control(HalDev, hcPhyAccess, hcGet, (bit32u *) &Value)

    If(rc == 0)
    {
        // Value is overwriten with the value in Register 0 of Phy number 23.
        PhyRegisterData = Value;
        if((PhyRegisterData & 0x8000) == 0)
           ResetIsComplete;
    }

@endcode
<B>
*** Example Showing turning values off/on ***
<BR>
</B>

@code

int On=1;
int Off=0;
   # Turn On loopback
   OsDev->HalFunc->Control(OsDev->HalDev, "CTRL_LOOPBACK", hcSet, (int*) &On);

   #  Turn off RX Flow
   OsDev->HalFunc->Control(OsDev->HalDev, "RX_FLOW_EN", hcSet, (int*) &Off);
@endcode

@par CPMAC Configurable Parameters

- RX_PASS_CRC        : See MBP_Enable description
- RX_QOS_EN          : See MBP_Enable description
- RX_NO_CHAIN        : See MBP_Enable description
- RX_CMF_EN          : See MBP_Enable description
- RX_CSF_EN          : See MBP_Enable description
- RX_CEF_EN          : See MBP_Enable description
- RX_CAF_EN          : See MBP_Enable description
- RX_PROM_CH         : See MBP_Enable description
- RX_BROAD_EN        : See MBP_Enable description
- RX_BROAD_CH        : See MBP_Enable description
- RX_MULT_EN         : See MBP_Enable description
- RX_MULT_CH         : See MBP_Enable description

- TX_PTYPE           : See MacControl description
- TX_PACE            : See MacControl description
- TX_FLOW_EN         : See MacControl description
- RX_FLOW_EN         : See MacControl description
- CTRL_LOOPBACK      : See MacControl description

- RX_MAXLEN          : See CPMAC Guide
- RX_FILTERLOWTHRESH : See CPMAC Guide
- RX0_FLOWTHRESH     : See CPMAC Guide
- RX_UNICAST_SET     : See CPMAC Guide
- RX_UNICAST_CLEAR   : See CPMAC Guide

@par Multicast Support
- RX_MULTI_ALL       : When used with hcSet, sets all the Hash Bits. When used
with hcClear clears all the Hash Bits.
- RX_MULTI_SINGLE    : When used with hcSet, adds the Hashed Mac Address. When used
with hcClear deletes the Hashed Mac Address.
Note: Support will be added to keep track of Single additions and deletions.

@code
*** Example Code ***

*** OS code ***
  bit8u  MacAddress[6];
  MacAddress[0] = 0x80;
  MacAddress[1] = 0x12;
  MacAddress[2] = 0x34;
  MacAddress[3] = 0x56;
  MacAddress[4] = 0x78;
  MacAddress[5] = 0x78;
  OsDev->HalFunc->Control(OsDev->HalDev, "RX_MULTI_SINGLE", hcSet, (bit8u*) &MacAddress);
  OsDev->HalFunc->Control(OsDev->HalDev, "RX_MULTI_SINGLE", hcClear, (bit8u*) &MacAddress);
  OsDev->HalFunc->Control(OsDev->HalDev, "RX_MULTI_ALL", hcSet, NULL);
  OsDev->HalFunc->Control(OsDev->HalDev, "RX_MULTI_ALL", hcClear, NULL);
@endcode
@par MdioConnect Fields
<BR>
- "MdioConnect"     : The OS can set the Phy connection using this key. The default connection is Auto-Negotiation ON, All modes possible.


- _CPMDIO_HD       <----- Allow Half Duplex, default is 1 (On)
- _CPMDIO_FD       <----- Allow Full Duplex, default is 1 (On)
- _CPMDIO_10       <----- Allow 10  Mbs, default is 1 (On)
- _CPMDIO_100      <----- Allow 100 Mbs, default is 1 (On)
- _CPMDIO_NEG_OFF  <----- Turn off Auto Negotiation, default is 0 (Auto Neg is on)
- _CPMDIO_NOPHY    <----- Set for use with Marvel-type switch, default is 0 (Phy present)
- _CPMDIO_AUTOMDIX  <---- Enables Auto Mdix (in conjunction with MdixMask), default is 1 (On)

Note: When _CPMDIO_NOPHY is set, CPMAC will report being linked at 100/FD. Reported PhyNum will be 0xFFFFFFFF

@par Setting CPMAC for use with a Marvel-type Switch
@code
     bit32u MdioConnect;

     MdioConnect = _CPMDIO_NOPHY;
     OsDev->HalFunc->Control(OsDev->HalDev, "MdioConnect", hcSet, (bit32u*) &MdioConnect);
@endcode

@par OS Support for MDIO
@p The OS will need to supply the following values which the CPHAL will request via halControl()
<BR>
- MdioBusFrequency : The frequency of the BUS that MDIO is on (requested via hcMdioBusFrequency)
<BR>
- MdioClockFrequency : The desired Clock Frequency that MDIO qill operate at (requested via hcMdioClockFrequency)
*/

/**
@page cpmac_conf DeviceFindxxx() Parameters

These are some of the parameters that the CPMAC will request via the DeviceFindxxx() functions -
<BR>
- "Mlink"    : bit mask indicating what link status method Phy is using. Default is MDIO state machine (0x0)
- "PhyMask"  : bit mask indicating PhyNums used by this CPMAC (e.g 0x8000000, PhyNum is 31)
- "MdixMask" : bit mask indicating which Phys support AutoMdix. Default is 0x0 (None)
<BR>
@par Example cpmac definition from the options.conf for the Sangam VDB
<BR>
- cpmac( id=eth0, base=0xA8610000, size=0x800, reset_bit=17, int_line=19, PhyMask=0x80000000, MLink=0, MdixMask=0 )
*/

/**
@page auto_mdix Auto Mdix Support

Auto Mdix selection is controlled by two elements in the CPMAC. First the OS can turn Auto Midx On or Off by the use of the
MdioConnect field, _CPMDIO_AUTOMDIX. This is defaulted ON.  For actual Auto Mdix operation the Phy must also be Auto Mdix capable.
This is specified by the DeviceFindxxx() field, "MdixMask" (supplied as the variable hcMdixMask).
If both these fields are set then the CPMDIO state machine will be enabled for Auto Mdix checking.
If a switch to MDI or MDIX mode is needed, the CPMAC will signal this to the OS via Control() using
the hcMdioMdixSwitch key.

@par OS example for responding to a Mdix Switch Request
<BR>
@code
if(osStrcmpi(pszKey, hcMdioMdixSwitch) == 0)  // See if key is Mdix Switch Request
  {
      if(osStrcmpi(pszAction, hcSet) == 0)   // Only respond to Set requests
      {

          bit32u Mdix;

          Mdix = *(bit32u *) ParmValue;     // Extract requested Mode
                                            // 0 : MDI
                                            // 1 : MDIX
          if(Mdix)
            osSetPhyIntoMdixMode();         // Device specific logic
          else
            osSetPhyIntoMdiMode();          // Device specific logic
          rc = 0;                           // Set return code as Successfull
      }
@endcode
*/

/**
@page cpmac_stats CPMAC Specific Statistics

Statistics level '0' contains all CPMAC specific statistics.


*/

/**
@page Example_Driver_Code

@section  example_intro Introduction
This section provides an in-depth code example for driver implementations.  The code
below illustrates the use of the CPMAC HAL, but is equally applicable to any CPHAL
implementation. Note: the CPHAl constants hcGet, hcSet etc., are currently available for use with teh CPMAC module.
Other modules should continue to use pszGET, etc. until these are made generally available.

@par Pull Model Example

@code

#define _CPHAL_CPMAC

typedef struct _os_device_s  OS_DEVICE;
typedef struct _os_receive_s OS_RECEIVEINFO;
typedef struct _os_send_s    OS_SENDINFO;
typedef struct _os_setup_s   OS_SETUP;

#include "cpswhal_cpmac.h"

#define dbgPrintf printf

typedef struct  _os_device_s
{
   HAL_DEVICE    *HalDev;
   HAL_FUNCTIONS *HalFunc;
   OS_FUNCTIONS  *OsFunc;
   OS_SETUP      *OsSetup;
   bit32u        Interrupt;
   int           (*halIsr)(HAL_DEVICE  *HalDev, int*);
   int           ModulePort;
   int           Protocol;
   int           LinkStatus;  // 0-> down, otherwise up
}os_device_s;

typedef struct  _os_receive_s
{
   HAL_RECEIVEINFO *HalReceiveInfo;
   char   *ReceiveBuffer;
   OS_DEVICE  *OsDev;
}os_receive_s;

typedef struct  _os_send_s
{
   OS_DEVICE  *OsDev;
}os_send_s;

typedef struct  _os_setup_s
{
   OS_DEVICE  *OsDev;
}os_setup_s;



void FlowForCphal(OS_DEVICE *OsDev)
{
  CHANNEL_INFO ChannelInfo;
  int nChannels = 200;
  int halFuncSize;
  int rc;

  //  Populate OsFunc structure
  rc =  osInitModule(OsDev);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from osInitModule:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }


  //  OS-Cphal handshake
  rc = halCpmacInitModule(&OsDev->HalDev, OsDev, &OsDev->HalFunc, OsDev->OsFunc,
                              sizeof(OS_FUNCTIONS), &halFuncSize, OsDev->ModulePort);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from halCpmacInitModule:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }

  // See if hardware module exists
  rc = OsDev->HalFunc->Probe(OsDev->HalDev);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from Probe:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }

  // Initialize hardware module
  rc = OsDev->HalFunc->Init(OsDev->HalDev);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from Init:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }

  // Setup Channel Information (Tranmsit, channel 0)
  ChannelInfo.Channel      = 0;
  ChannelInfo.Direction    = DIRECTION_TX;
  ChannelInfo.TxNumBuffers = nChannels;
  ChannelInfo.TxNumQueues  = 1;
  ChannelInfo.TxServiceMax = nChannels/3;

  rc = OsDev->HalFunc->ChannelSetup(OsDev->HalDev, &ChannelInfo, OsDev->OsSetup);

  // Setup Channel Information (Receive, channel 0)
  ChannelInfo.Channel        = 0;
  ChannelInfo.Direction      = DIRECTION_RX;
  ChannelInfo.RxBufSize      = 1518;
  ChannelInfo.RxBufferOffset = 0;
  ChannelInfo.RxNumBuffers   = 2*nChannels;
  ChannelInfo.RxServiceMax   = nChannels/3;

  rc = OsDev->HalFunc->ChannelSetup(OsDev->HalDev, &ChannelInfo, OsDev->OsSetup);

  // Open the hardware module
  rc = OsDev->HalFunc->Open(OsDev->HalDev);

  // Module now ready to Send/Receive data
}


int osInitModule(OS_FUNCTIONS **pOsFunc)
  {
   OS_FUNCTIONS *OsFunc;

   OsFunc = (OS_FUNCTIONS *) malloc(sizeof(OS_FUNCTIONS));
   if (!OsFunc)
     return (-1);

   *pOsFunc = OsFunc;

   OsFunc->CriticalOff               = osCriticalOff;
   OsFunc->CriticalOn                = osCriticalOn;
   OsFunc->DataCacheHitInvalidate    = osDataCacheHitInvalidate;
   OsFunc->DataCacheHitWriteback     = osDataCacheHitWriteback;
   OsFunc->DeviceFindInfo            = osDeviceFindInfo;
   OsFunc->DeviceFindParmUint        = osDeviceFindParmUint;
   OsFunc->DeviceFindParmValue       = osDeviceFindParmValue;
   OsFunc->Free                      = osFree;
   OsFunc->FreeDev                   = osFreeDev;
   OsFunc->FreeDmaXfer               = osFreeDmaXfer;
   OsFunc->FreeRxBuffer              = osFreeRxBuffer;
   OsFunc->IsrRegister               = osIsrRegister;
   OsFunc->IsrUnRegister             = osIsrUnRegister;
   OsFunc->Malloc                    = osMalloc;
   OsFunc->MallocDev                 = osMallocDev;
   OsFunc->MallocDmaXfer             = osMallocDmaXfer;
   OsFunc->MallocRxBuffer            = osMallocRxBuffer;


   OsFunc->Memset                    = memset;
   OsFunc->Printf                    = printf;
   OsFunc->Sprintf                   = sprintf;
   OsFunc->Strcmpi                   = osStrcmpi;
   OsFunc->Strlen                    = strlen;
   OsFunc->Strstr                    = strstr;
   OsFunc->Strtoul                   = strtoul;

   OsFunc->Control                   = osControl;
   OsFunc->Receive                   = osReceive;
   OsFunc->SendComplete              = osSendComplete;
   OsFunc->TeardownComplete          = osTearDownComplete;

   return(0);
  }


int osReceive(OS_DEVICE *OsDev,FRAGLIST *Fraglist,bit32u FragCount,bit32u PacketSize,HAL_RECEIVEINFO *halInfo, bit32u mode)
  {
  OS_RECEIVEINFO *skb = (OS_RECEIVEINFO *)Fraglist[0].OsInfo;
  dcache_i((char *)Fraglist->data, Fraglist->len);
  OsDev->HalFunc->RxReturn(halInfo,0);
  return(0);
  }

int osSendComplete(OS_SENDINFO *skb)
  {
  return(0);
  }


static void *osMallocRxBuffer(bit32u Size,void *MemBase, bit32u MemRange,
                              OS_SETUP *OsSetup, HAL_RECEIVEINFO *HalReceiveInfo,
                              OS_RECEIVEINFO **OsReceiveInfo, OS_DEVICE *OsDev )
  {
   void *HalBuffer;
   OS_RECEIVEINFO *OsPriv;

   HalBuffer=malloc(Size);
   if (!HalBuffer)
     {
      return(0);
     }

   // Malloc the OS block
   *OsReceiveInfo = malloc(sizeof(OS_RECEIVEINFO));
   if (!*OsReceiveInfo)
     {
      free(HalBuffer);
      return(0);
     }

   // Initialize the new buffer descriptor
   OsPriv                 = *OsReceiveInfo;
   OsPriv->OsDev          =  OsDev;
   OsPriv->ReceiveBuffer  =  HalBuffer;
   OsPriv->HalReceiveInfo =  HalReceiveInfo;

   return(HalBuffer);
  }


void SendBuffer(OS_DEVICE *OsDev, char *Buffer, int Size)
{
  FRAGLIST Fraglist;
  bit32u FragCount;

  tcb_pending++;
  Fraglist.len  = Size;
  Fraglist.data = (unsigned *) Buffer;
  FragCount     = 1;
  mode          = 0;   //  Channel 0

  dcache_wb(Fraglist.data, Fraglist.len);
  OsDev->HalFunc->Send(OsDev->HalDev, &Fraglist, FragCount, Size, (OS_SENDINFO *) Buffer, mode);
}


void osStateChange(OS_DEVICE *OsDev)
  {
  int status;
  int LinkStatus;
  OsDev->HalFunc->Control(OsDev->HalDev, "Status", hcGet, &status);
  if(status & CPMAC_STATUS_ADAPTER_CHECK)
    {
    //  Adapter Check, take appropiate action
    }
  else
    {
    LinkStatus = status & CPMAC_STATUS_LINK;
    if(LinkStatus != OsDev->LinkStatus)
      {
      dbgPrintf("\n%s:Link %s for inst %d Speed %s, Duplex %s\n",
                 __FUNCTION__,
                 LinkStatus ? "up" : "down",
                 OsDev->ModulePort,
                 status & CPMAC_STATUS_LINK_SPEED ? "100" : "10",
                 status & CPMAC_STATUS_LINK_DUPLEX ? "FD" : "HD");
      OsDev->LinkStatus = LinkStatus;
      }
    }
  }


int osControl(OS_DEVICE *OsDev, const char *pszKey, const char* pszAction, void *ParmValue)
  {
  int rc=-1;

  if (osStrcmpi(pszKey, hcCpuFrequency) == 0)
   {
   if(osStrcmpi(pszAction, hcGet) == 0)
     {
     *(bit32u*) ParmValue = cpufreq;
     rc = 0;
     }
   }
  if (osStrcmpi(pszKey, hcMdioBusFrequency) == 0)
   {
   if(osStrcmpi(pszAction, hcGet) == 0)
     {
     *(bit32u *)ParmValue = MdioBusFrequency;
     rc = 0;
     }
   }
if (osStrcmpi(pszKey, hcMdioClockFrequency) == 0)
   {
   if(osStrcmpi(pszAction, hcGet) == 0)
     {
     *(bit32u *)ParmValue = MdioClockFrequency;
     rc = 0;
     }
   }

  if (osStrcmpi(pszKey, hcTick) == 0)
   {
   if(osStrcmpi(pszAction, hcSet) == 0)
     {
       osTickSetInterval(OsDev, *(unsigned int *) ParmValue);
       rc = 0;
     }
   else
   if(osStrcmpi(pszAction, hcClear) == 0)
     {
       osTickDisable(OsDev);
       rc = 0;
     }
   }

  if (osStrcmpi(pszKey, "SioFlush") == 0)
    {
       MySioFlush();
       rc = 0;
    }

  if (osStrcmpi(pszKey, "StateChange") == 0)
    {
       osStateChange(OsDev);
       rc = 0;
    }

  if (osStrcmpi(pszKey, "Sleep") == 0)
    {
       osSleep(*(int *)ParmValue);
       rc = 0;
    }
  return(rc);
  }

@endcode


@par Push Model Example (Currently Eswitch ONLY)

@code

typedef struct _os_device_s  OS_DEVICE;
typedef struct _os_receive_s OS_RECEIVEINFO;
typedef struct _os_send_s    OS_SENDINFO;
typedef struct _os_setup_s   OS_SETUP;

#include "cpswhal.h"          //Get glogal HAL stuff
#include "cpswhaleswitch.h"   //Get device specific hal stuff


typedef struct  _os_device_s
{
   HAL_DEVICE    *HalDev;
   HAL_FUNCTIONS *HalFunc;
   OS_FUNCTIONS  *OsFunc;
   OS_SETUP      *OsSetup;
   bit32u        Interrupt;
   int           (*halIsr)(HAL_DEVICE  *HalDev, int*);
   int           ModulePort;
   int           Protocol;
   int           LinkStatus;  // 0-> down, otherwise up
}os_device_s;

typedef struct  _os_receive_s
{
   HAL_RECEIVEINFO *HalReceiveInfo;
   char   *ReceiveBuffer;
   OS_DEVICE  *OsDev;
}os_receive_s;

typedef struct  _os_send_s
{
   OS_DEVICE  *OsDev;
}os_send_s;

typedef struct  _os_setup_s
{
   OS_DEVICE  *OsDev;
}os_setup_s;



void FlowForCphal(OS_DEVICE *OsDev)
{
CHANNEL_INFO ChannelInfo;
  int nChannels = 200;
  int halFuncSize;
  int rc;

  //  Populate OsFunc structure
  rc =  osInitModule(OsDev);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from osInitModule:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }


  //  OS-Cphal handshake
  rc = cpswHalEswitchInitModule(&OsDev->HalDev, OsDev, &OsDev->HalFunc, OsDev->OsFunc,
                              sizeof(OS_FUNCTIONS), &halFuncSize, OsDev->ModulePort);

  if(rc)
  {
    sprintf(bufTmp, "%s: return code from cpswHalEswitchInitModule:'0x%08X'", __FUNCTION__, rc);
    errorout(bufTmp);
  }


  ChannelInfo.Channel      = 7;
  ChannelInfo.Direction    = DIRECTION_RX;
  ChanInfo.Receive = osReceiveSS;         //  Specify function to receive data for this channel

  rc = OsDev->HalFunc->ChannelSetup(OsDev->HalDev, &ChannelInfo, OsDev->OsSetup);

  MyConfig.debug=0;
  MyConfig.CpuFrequency = CpuFreq;
  MyConfig.EswitchFrequency = EswitchFreq;
  MyConfig.ResetBase  = 0xa8611600;
  MyConfig.MacAddress = MacAddr;

  MyConfig.EswitchResetBit= 27;
  MyConfig.Cpmac0ResetBit = 17;
  MyConfig.Cpmac1ResetBit = 21;
  MyConfig.MdioResetBit   = 22;
  MyConfig.Phy0ResetBit   = 26;
  MyConfig.Phy1ResetBit   = 28;
  MyConfig.HdmaResetBit   = 13;
  MyConfig.Cpmac0IntBit   = 19;
  MyConfig.Cpmac1IntBit   = 33;
  MyConfig.EswitchIntBit  = 27;
  MyConfig.EswitchBase    = 0xa8640000;
  MyConfig.EswitchBufferSize     = 64;
  MyConfig.EswitchHostBufCount   = 0;
  MyConfig.EswitchDefaultCamSize = 64;
  MyConfig.EswitchOverFlowCount  = 200;
  MyConfig.EswitchOverFlowSize   = 256;




  rc=EswitchConfig(HalDev,HalFunc,&MyConfig);


  // Open the hardware module
  rc = OsDev->HalFunc->Open(OsDev->HalDev);

  // Module now ready to Send/Receive data
}


int EswitchConfig(HAL_DEVICE *HalDev, HAL_FUNCTIONS *HalFunc, ESWITCH_CONFIG *Config)
{
  bit32u sts;
  sts = 0;

  sts |= cpswhalPushBin(hcdebug, Config->debug);
  sts |= cpswhalPushBin(hcCpuFrequency     , Config->CpuFrequency );
  sts |= cpswhalPushBin(hcEswitchFrequency , Config->EswitchFrequency );
  sts |= cpswhalPushBin(hcResetBase        , Config->ResetBase  );
  sts |= cpswhalPushBin(hcMacAddress       , Config->MacAddress );
  sts |= cpswhalPushBin(hcEswitchResetBit, Config->EswitchResetBit);
  sts |= cpswhalPushBin(hcCpmac0ResetBit , Config->Cpmac0ResetBit );
  sts |= cpswhalPushBin(hcCpmac1ResetBit , Config->Cpmac1ResetBit );
  sts |= cpswhalPushBin(hcMdioResetBit   , Config->MdioResetBit   );
  sts |= cpswhalPushBin(hcPhy0ResetBit   , Config->Phy0ResetBit   );
  sts |= cpswhalPushBin(hcPhy1ResetBit   , Config->Phy1ResetBit   );
  sts |= cpswhalPushBin(hcHdmaResetBit   , Config->HdmaResetBit   );
  sts |= cpswhalPushBin(hcCpmac0IntBit   , Config->Cpmac0IntBit   );
  sts |= cpswhalPushBin(hcCpmac1IntBit   , Config->Cpmac1IntBit   );
  sts |= cpswhalPushBin(hcEswitchIntBit  , Config->EswitchIntBit  );
  sts |= cpswhalPushBin(hcEswitchBase           , Config->EswitchBase    );
  sts |= cpswhalPushBin(hcEswitchBufferSize     , Config->EswitchBufferSize     );
  sts |= cpswhalPushBin(hcEswitchHostBufCount   , Config->EswitchHostBufCount   );
  sts |= cpswhalPushBin(hcEswitchDefaultCamSize , Config->EswitchDefaultCamSize );
  sts |= cpswhalPushBin(hcEswitchOverFlowCount  , Config->EswitchOverFlowCount  );
  sts |= cpswhalPushBin(hcEswitchOverFlowSize   , Config->EswitchOverFlowSize   );
  return(sts);
}



@endcode
*/

#endif
