#ifndef _INC_CPCOMMON_C
#define _INC_CPCOMMON_C

#ifdef _CPHAL_CPMAC
#include "cpremap_cpmac.c"
#endif

#ifdef _CPHAL_AAL5
#include "cpremap_cpaal5.c"
#endif

#ifdef _CPHAL_CPSAR
#include "cpremap_cpsar.c"
#endif

#ifdef _CPHAL_AAL2
#include "cpremap_cpaal2.c"
#endif

/**
@defgroup Common_Config_Params Common Configuration Parameters

This section documents the configuration parameters that are valid across
all CPHAL devices.
@{
*/
/** This is the debug level.  The field is bit defined, such that the user
should set to 1 all the bits corresponding to desired debug outputs.  The following 
are the meanings for each debug bit:
- bit0 (LSB): CPHAL Function Trace
- b1 : OS Function call trace
- b2 : Critical section entry/exit
- b3 : Memory allocation/destruction
- b4 : Detailed information in Rx path
- b5 : Detailed information in Tx path
- b6 : Extended error information
- b7 : General info
*/
static const char pszDebug[]        = "debug";
/** CPU Frequency. */
/*static const char pszCpuFreq[]      = "CpuFreq";*/               /*MJH-030403*/
/** Base address for the module. */
static const char pszBase[]         = "base";
/** Reset bit for the module. */
static const char pszResetBit[]     = "reset_bit";
/** Reset base address for the module. */
static const char pszResetBase[]    = "ResetBase";
/** Interrupt line for the module. */
static const char pszIntLine[]      = "int_line";
/** VLYNQ offset for the module.  Disregard if not using VLYNQ. */
static const char pszOffset[]       = "offset";
/** The OS may "Get" this parameter, which is a pointer
    to a character string that indicates the version of CPHAL. */
static const char pszVer[]          = "Version";
/*@}*/

/**
@defgroup Common_Control_Params Common Keys for [os]Control()

This section documents the keys used with the OS @c Control() interface that
are required by CPHAL devices.

@{
*/
/** Used to wait for an integer number of clock ticks, given as an integer
    pointer in the @p Value parameter.  No actions are defined. */
static const char pszSleep[]             = "Sleep";
/** Requests the OS to flush it's IO buffers.  No actions are defined. */
static const char pszSioFlush[]          = "SioFlush";
/*@}*/

static const char pszStateChange[]       = "StateChange";
static const char pszStatus[]            = "Status";

static const char pszGET[]               = "Get";
static const char pszSET[]               = "Set";
static const char pszCLEAR[]             = "Clear";
static const char pszNULL[]              = "";
static const char pszLocator[]           = "Locator";
static const char pszOff[]               = "Off";
static const char pszOn[]                = "On";
static const char hcMaxFrags[]           = "MaxFrags";

#ifdef _CPHAL_CPMAC

/*  New method for string constants */
const char hcClear[]  = "Clear";
const char hcGet[]    = "Get";
const char hcSet[]    = "Set";

const char hcTick[]   = "Tick";

static const CONTROL_KEY KeyCommon[] =
   {
     {""                   , enCommonStart},
     {pszStatus            , enStatus},
     {pszOff               , enOff},
     {pszOn                , enOn},
     {pszDebug             , enDebug},
     {hcCpuFrequency       , enCpuFreq},                           /*MJH~030403*/
     {""                   , enCommonEnd}
   };
#endif

/**
@defgroup Common_Statistics Statistics

A broad array of module statistics is available.  Statistics values are accessed
through the @c Control() interface of the CPHAL.  There are 5 different levels
of statistics, each of which correspond to a unique set of data.  Furthermore,
certain statistics data is indexed by using a channel number and Tx queue number.
The following is a brief description of each statistics level, along with the
indexes used for the level:

- Level 0:  Hardware Statistics (index with channel)
- Level 1:  CPHAL Software Statistics (channel, queue)
- Level 2:  CPHAL Flags (channel, queue)
- Level 3:  CPHAL Channel Configuration (channel)
- Level 4:  CPHAL General Configuration (no index)

The caller requests statistics information by providing a Key string to the
@c Control() API in the following format: "Stats;[Level #];[Ch #];[Queue #]".
The only valid Action parameter for statistics usage is "Get".

Code Examples:
@code
unsigned int *StatsData;

# Get Level 0 stats for Channel 1
HalFunc->Control(OsDev->HalDev, "Stats;0;1", "Get", &StatsData);

# Get Level 2 stats for Channel 0, Queue 0
HalFunc->Control(OsDev->HalDev, "Stats;2;0;0", "Get", &StatsData);

# Get Level 4 stats
HalFunc->Control(OsDev->HalDev, "Stats;4", "Get", &StatsData);
@endcode

The information returned in the Value parameter of @c Control() is an
array of pointers to strings.  The pointers are arranged in pairs.
The first pointer is a pointer to a name string for a particular statistic.
The next pointer is a pointer to a string containing the representation of
the integer statistic value corresponding to the first pointer.  This is followed
by another pair of pointers, and so on, until a NULL pointer is encountered.  The
following is example code for processing the statistics data.  Note that the OS
is responsible for freeing the memory passed back through the Value parameter of
@c Control().

@code
unsigned int *StatsData;

# Get Level 0 stats for Channel 1
HalFunc->Control(OsDev->HalDev, "Stats;0;1", "Get", &StatsData);

# output Statistics data
PrintStats(StatsData);

# the upper layer is responsible for freeing stats info
free(&StatsPtr);

...

void PrintStats(unsigned int *StatsPtr)
  {
   while(*StatsPtr)
     {
      printf("%20s:",  (char *)*StatsPtr);
      StatsPtr++;
      printf("%11s\n", (char *)*StatsPtr);
      StatsPtr++;
     }
   MySioFlush();
  }
@endcode

Within each statistics level, there are several statistics defined.  The statistics that
are common to every CPPI module are listed below.  In addition, each module may define
extra statistics in each level, which will be documented within the module-specific
documentation appendices.

- Level 0 Statistics
  - All level 0 statistics are module-specific.
- Level 1 Statistics (CPHAL Software Statistics)
  - DmaLenErrors: Incremented when the port DMA's more data than expected (per channel). (AAL5 Only)
  - TxMisQCnt: Incremented when host queues a packet for transmission as the port finishes
transmitting the previous last packet in the queue (per channel and queue).
  - RxMisQCnt: Incremented when host queues adds buffers to a queue as the port finished the
reception of the previous last packet in the queue (per channel).
  - TxEOQCnt: Number of times the port has reached the end of the transmit queue (per channel and queue).
  - RxEOQCnt: Number of times the port has reached the end of the receive queue (per channel).
  - RxPacketsServiced: Number of received packets (per channel).
  - TxPacketsServiced: Number of transmitted packets (per channel and queue).
  - RxMaxServiced: Maximum number of packets that the CPHAL receive interrupt has serviced at a time (per channel).
  - TxMaxServiced: Maximum number of packets that the CPHAL transmit interrupt has serviced at a time (per channel and queue).
  - RxTotal: Total number of received packets, all channels.
  - TxTotal: Total number of transmitted packets, all channels and queues.
- Level 2 Statistics (CPHAL Flags)
  - RcbPool: Pointer to receive descriptor pool (per channel).
  - RxActQueueCount: Number of buffers currently available for receive (per channel).
  - RxActQueueHead: Pointer to first buffer in receive queue (per channel).
  - RxActQueueTail: Pointer to last buffer in receive queue (per channel).
  - RxActive: 0 if inactive (no buffers available), or 1 if active (buffers available).
  - RcbStart: Pointer to block of receive descriptors.
  - RxTeardownPending: 1 if Rx teardown is pending but incomplete, 0 otherwise.
  - TcbPool: Pointer to transmit descriptor pool (per channel and queue).
  - TxActQueueCount: Number of buffers currently queued to be transmitted (per channel and queue).
  - TxActQueueHead: Pointer to first buffer in transmit queue (per channel and queue).
  - TxActQueueTail: Pointer to last buffer in transmit queue (per channel and queue).
  - TxActive: 0 if inactive (no buffers to send), or 1 if active (buffers queued to send).
  - TcbStart: Pointer to block of transmit descriptors.
  - TxTeardownPending: 1 if Tx teardown is pending but incomplete, 0 otherwise.
- Level 3 Statistics (CPHAL Channel Configuration)
  - RxBufSize: Rx buffer size.
  - RxBufferOffset: Rx buffer offset.
  - RxNumBuffers: Number of Rx buffers.
  - RxServiceMax: Maximum number of receive packets to service at a time.
  - TxNumBuffers: Number of Tx buffer descriptors.
  - TxNumQueues: Number of Tx queues to use.
  - TxServiceMax: Maximum number of transmit packets to service at a time.
- Level 4 Statistics (CPHAL General Configuration)
  - Base Address: Base address of the module.
  - Offset (VLYNQ): VLYNQ relative module offset.
  - Interrupt Line: Interrupt number.
  - Debug: Debug flag, 1 to enable debug.
  - Inst: Instance number.
*/

/* 
   Data Type 0 = int display
   Data Type 1 = hex display
   Data Type 2 = channel structure, int display
   Data Type 3 = queue index and int display
   Data Type 4 = queue index and hex display
*/
#if (defined(_CPHAL_AAL5) || defined(_CPHAL_CPMAC)) /* +GSG 030307 */
static STATS_TABLE StatsTable0[] =
  {
#ifdef _CPHAL_AAL5
   /* Name ,        Data Ptr,   Data Type */
   {"Crc Errors",          0,          0},
   {"Len Errors",          0,          0},
   {"Abort Errors",        0,          0},
   {"Starv Errors",        0,          0}
#endif
#ifdef _CPHAL_CPMAC
   {"Rx Good Frames",      0,          0}
#endif
  };

static STATS_TABLE StatsTable1[] =
  {
   /* Name ,        Data Ptr,   Data Type */
   {"DmaLenErrors",        0,          0},
   {"TxMisQCnt",           0,          3},
   {"RxMisQCnt",           0,          0},
   {"TxEOQCnt",            0,          3},
   {"RxEOQCnt",            0,          0},
   {"RxPacketsServiced",   0,          0},
   {"TxPacketsServiced",   0,          3},
   {"RxMaxServiced",       0,          0},
   {"TxMaxServiced",       0,          3},
   {"RxTotal",             0,          0},
   {"TxTotal",             0,          0},
  };

static STATS_TABLE StatsTable2[] =
  {
   /* Name ,        Data Ptr,   Data Type */
   {"RcbPool",             0,          1},
   {"RxActQueueCount",     0,          0},
   {"RxActQueueHead",      0,          1},
   {"RxActQueueTail",      0,          1},
   {"RxActive",            0,          0},
   {"RcbStart",            0,          1},
   {"RxTeardownPending",   0,          0},
   {"TcbPool",             0,          4},
   {"TxActQueueCount",     0,          3},
   {"TxActQueueHead",      0,          4},
   {"TxActQueueTail",      0,          4},
   {"TxActive",            0,          3},
   {"TcbStart",            0,          4},
   {"TxTeardownPending",   0,          0}
  };

static STATS_TABLE StatsTable3[] =
  {
   /* Name ,        Data Ptr,   Data Type */
   {"RxBufSize",           0,          2},
   {"RxBufferOffset",      0,          2},
   {"RxNumBuffers",        0,          2},
   {"RxServiceMax",        0,          2},
   {"TxNumBuffers",        0,          2},
   {"TxNumQueues",         0,          2},
   {"TxServiceMax",        0,          2},
#ifdef _CPHAL_AAL5
   {"CpcsUU",              0,          2},
   {"Gfc",                 0,          2},
   {"Clp",                 0,          2},
   {"Pti",                 0,          2},
   {"DaMask",              0,          2},
   {"Priority",            0,          2},
   {"PktType",             0,          2},
   {"Vci",                 0,          2},
   {"Vpi",                 0,          2},
   {"CellRate",            0,          2},
   {"QosType",             0,          2},
   {"Mbs",                 0,          2},
   {"Pcr",                 0,          2}
#endif
  };

static STATS_TABLE StatsTable4[] =
  {
   {"Base Address",        0,          1},
   {"Offset (VLYNQ)",      0,          0},
   {"Interrupt Line",      0,          0},
   {"Debug",               0,          0},
   {"Instance",            0,          0},
#ifdef _CPHAL_AAL5
   {"UniNni",              0,          0}
#endif
  };

static STATS_DB StatsDb[] =
  {
    {(sizeof(StatsTable0)/sizeof(STATS_TABLE)), StatsTable0},
    {(sizeof(StatsTable1)/sizeof(STATS_TABLE)), StatsTable1},
    {(sizeof(StatsTable2)/sizeof(STATS_TABLE)), StatsTable2},
    {(sizeof(StatsTable3)/sizeof(STATS_TABLE)), StatsTable3},
    {(sizeof(StatsTable4)/sizeof(STATS_TABLE)), StatsTable4}
  };
#endif /* +GSG 030307 */

#ifdef _CPHAL_CPMAC /* +RC 3.02 */
static void resetWait(HAL_DEVICE *HalDev)
  {                                                                  /*+RC3.02*/
  const int TickReset=64;
  osfuncSleep((int*)&TickReset);
  }                                                                  /*+RC3.02*/
#endif /* +RC 3.02 */

/* I only define the reset base function for the modules
   that can perform a reset.  The AAL5 and AAL2 modules
   do not perform a reset, that is done by the shared module
   CPSAR */
#if defined(_CPHAL_CPSAR) || defined(_CPHAL_CPMAC) || defined(_CPHAL_VDMAVT)
/*
 *  Determines the reset register address to be used for a particular device.
 *  It will search the current device entry for Locator information.  If the
 *  device is a root device, there will be no Locator information, and the
 *  function will find and return the root reset register.  If a Locator value
 *  is found, the function will search each VLYNQ device entry in the system
 *  looking for a matching Locator.  Once it finds a VLYNQ device entry with
 *  a matching Locator, it will extract the "ResetBase" parameter from that
 *  VLYNQ device entry (thus every VLYNQ entry must have the ResetBase parameter).
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())
 *  @param  ResetBase Pointer to integer address of reset register.
 *
 *  @return 0 OK, Non-zero not OK
 */
static int ResetBaseGet(HAL_DEVICE *HalDev, bit32u *ResetBase)
  {
   char *DeviceInfo = HalDev->DeviceInfo;
   char *MyLocator, *NextLocator;
   int Inst=1;
   bit32u error_code;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpcommon]ResetBaseGet(HalDev:%08x, ResetBase:%08x)\n", (bit32u)HalDev, ResetBase);
      osfuncSioFlush();
     }
#endif

   error_code = HalDev->OsFunc->DeviceFindParmValue(DeviceInfo, "Locator", &MyLocator);
   if (error_code)
     {
      /* if no Locator value, device is on the root, so get the "reset" device */
      error_code = HalDev->OsFunc->DeviceFindInfo(0, "reset", &DeviceInfo);
      if  (error_code)
        {
         return(EC_VAL_DEVICE_NOT_FOUND);
        }

      error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, "base", ResetBase);
      if (error_code)
        {
         return(EC_VAL_BASE_ADDR_NOT_FOUND);
        }

      *ResetBase = ((bit32u)PhysToVirtNoCache(*ResetBase));

      /* found base address for root device, so we're done */
      return (EC_NO_ERRORS);
     }
    else
     {
      /* we have a Locator value, so the device is remote */

      /* Find a vlynq device with a matching locator value */
      while ((HalDev->OsFunc->DeviceFindInfo(Inst, "vlynq", &DeviceInfo)) == EC_NO_ERRORS)
        {
         error_code = HalDev->OsFunc->DeviceFindParmValue(DeviceInfo, "Locator", &NextLocator);
         if (error_code)
           {
            /* no Locator value for this VLYNQ, so move on */
            continue;
           }
         if (HalDev->OsFunc->Strcmpi(MyLocator, NextLocator)==0)
           {
            /* we have found a VLYNQ with a matching Locator, so extract the ResetBase */
            error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, "ResetBase", ResetBase);
            if (error_code)
              {
               return(EC_VAL_BASE_ADDR_NOT_FOUND);
              }
            *ResetBase = ((bit32u)PhysToVirtNoCache(*ResetBase));

            /* found base address for root device, so we're done */
            return (EC_NO_ERRORS);
           }
         Inst++;
        } /* while */
     } /* else */

   return (EC_NO_ERRORS);
  }
#endif

#ifndef _CPHAL_AAL2 /* + RC 3.02 */
static bit32u ConfigGetCommon(HAL_DEVICE *HalDev)
  {
   bit32u ParmValue;
   bit32 error_code;
   char *DeviceInfo = HalDev->DeviceInfo;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpcommon]ConfigGetCommon(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszBase, &ParmValue);
   if (error_code)
     {
      return(EC_FUNC_HAL_INIT|EC_VAL_BASE_ADDR_NOT_FOUND);
     }
   HalDev->dev_base = ((bit32u)PhysToVirtNoCache(ParmValue));

#ifndef _CPHAL_AAL5
#ifndef _CPHAL_AAL2
   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszResetBit, &ParmValue);
   if(error_code)
     {
      return(EC_FUNC_HAL_INIT|EC_VAL_RESET_BIT_NOT_FOUND);
     }
   HalDev->ResetBit = ParmValue;

   /* Get reset base address */
   error_code = ResetBaseGet(HalDev, &ParmValue);
   if (error_code)
     return(EC_FUNC_HAL_INIT|EC_VAL_RESET_BASE_NOT_FOUND);
   HalDev->ResetBase = ParmValue;
#endif
#endif

#ifndef _CPHAL_CPSAR
   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszIntLine,&ParmValue);
   if (error_code)
     {
      return(EC_FUNC_HAL_INIT|EC_VAL_INTERRUPT_NOT_FOUND);
     }
   HalDev->interrupt = ParmValue;
#endif

   /* only look for the offset if there is a Locator field, which indicates that
      the module is a VLYNQ module */
   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszLocator,&ParmValue);
   if (!error_code)
     {
      error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszOffset,&ParmValue);
      if (error_code)
        {
         return(EC_FUNC_HAL_INIT|EC_VAL_OFFSET_NOT_FOUND);
        }
      HalDev->offset = ParmValue;
     }
    else
      HalDev->offset = 0;

   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo, pszDebug, &ParmValue);
   if (!error_code) HalDev->debug = ParmValue;

   return (EC_NO_ERRORS);
  }
#endif /* +RC 3.02 */

#ifdef _CPHAL_CPMAC /* +RC 3.02 */
static void StatsInit(HAL_DEVICE *HalDev)                             /* +() RC3.02 */
  {
   /* even though these statistics may be for multiple channels and
      queues, i need only configure the pointer to the beginning
      of the array, and I can index from there if necessary */

#ifdef _CPHAL_AAL5
   StatsTable0[0].StatPtr  = &HalDev->Stats.CrcErrors[0];
   StatsTable0[1].StatPtr  = &HalDev->Stats.LenErrors[0];
   StatsTable0[2].StatPtr  = &HalDev->Stats.AbortErrors[0];
   StatsTable0[3].StatPtr  = &HalDev->Stats.StarvErrors[0];

   StatsTable1[0].StatPtr  = &HalDev->Stats.DmaLenErrors[0];
   StatsTable1[1].StatPtr  = &HalDev->Stats.TxMisQCnt[0][0];
   StatsTable1[2].StatPtr  = &HalDev->Stats.RxMisQCnt[0];
   StatsTable1[3].StatPtr  = &HalDev->Stats.TxEOQCnt[0][0];
   StatsTable1[4].StatPtr  = &HalDev->Stats.RxEOQCnt[0];
   StatsTable1[5].StatPtr  = &HalDev->Stats.RxPacketsServiced[0];
   StatsTable1[6].StatPtr  = &HalDev->Stats.TxPacketsServiced[0][0];
   StatsTable1[7].StatPtr  = &HalDev->Stats.RxMaxServiced;
   StatsTable1[8].StatPtr  = &HalDev->Stats.TxMaxServiced[0][0];
   StatsTable1[9].StatPtr  = &HalDev->Stats.RxTotal;
   StatsTable1[10].StatPtr = &HalDev->Stats.TxTotal;
#endif

#if (defined(_CPHAL_AAL5) || defined(_CPHAL_CPMAC))
   StatsTable2[0].StatPtr  = (bit32u *)&HalDev->RcbPool[0];
   StatsTable2[1].StatPtr  = &HalDev->RxActQueueCount[0];
   StatsTable2[2].StatPtr  = (bit32u *)&HalDev->RxActQueueHead[0];
   StatsTable2[3].StatPtr  = (bit32u *)&HalDev->RxActQueueTail[0];
   StatsTable2[4].StatPtr  = &HalDev->RxActive[0];
   StatsTable2[5].StatPtr  = (bit32u *)&HalDev->RcbStart[0];
   StatsTable2[6].StatPtr  = &HalDev->RxTeardownPending[0];
   StatsTable2[7].StatPtr  = (bit32u *)&HalDev->TcbPool[0][0];
   StatsTable2[8].StatPtr  = &HalDev->TxActQueueCount[0][0];
   StatsTable2[9].StatPtr  = (bit32u *)&HalDev->TxActQueueHead[0][0];
   StatsTable2[10].StatPtr = (bit32u *)&HalDev->TxActQueueTail[0][0];
   StatsTable2[11].StatPtr = &HalDev->TxActive[0][0];
   StatsTable2[12].StatPtr = (bit32u *)&HalDev->TcbStart[0][0];
   StatsTable2[13].StatPtr = &HalDev->TxTeardownPending[0];

   StatsTable3[0].StatPtr  = &HalDev->ChData[0].RxBufSize;
   StatsTable3[1].StatPtr  = &HalDev->ChData[0].RxBufferOffset;
   StatsTable3[2].StatPtr  = &HalDev->ChData[0].RxNumBuffers;
   StatsTable3[3].StatPtr  = &HalDev->ChData[0].RxServiceMax;
   StatsTable3[4].StatPtr  = &HalDev->ChData[0].TxNumBuffers;
   StatsTable3[5].StatPtr  = &HalDev->ChData[0].TxNumQueues;
   StatsTable3[6].StatPtr  = &HalDev->ChData[0].TxServiceMax;
#ifdef _CPHAL_AAL5
   StatsTable3[7].StatPtr  = &HalDev->ChData[0].CpcsUU;
   StatsTable3[8].StatPtr  = &HalDev->ChData[0].Gfc;
   StatsTable3[9].StatPtr  = &HalDev->ChData[0].Clp;
   StatsTable3[10].StatPtr = &HalDev->ChData[0].Pti;
   StatsTable3[11].StatPtr = &HalDev->ChData[0].DaMask;
   StatsTable3[12].StatPtr = &HalDev->ChData[0].Priority;
   StatsTable3[13].StatPtr = &HalDev->ChData[0].PktType;
   StatsTable3[14].StatPtr = &HalDev->ChData[0].Vci;
   StatsTable3[15].StatPtr = &HalDev->ChData[0].Vpi;
   StatsTable3[16].StatPtr = &HalDev->ChData[0].TxVc_CellRate;
   StatsTable3[17].StatPtr = &HalDev->ChData[0].TxVc_QosType;
   StatsTable3[18].StatPtr = &HalDev->ChData[0].TxVc_Mbs;
   StatsTable3[19].StatPtr = &HalDev->ChData[0].TxVc_Pcr;
#endif
#endif

   StatsTable4[0].StatPtr  = &HalDev->dev_base;
   StatsTable4[1].StatPtr  = &HalDev->offset;
   StatsTable4[2].StatPtr  = &HalDev->interrupt;
   StatsTable4[3].StatPtr  = &HalDev->debug;
   StatsTable4[4].StatPtr  = &HalDev->Inst;
  }
#endif /* +RC 3.02 */

#ifndef _CPHAL_CPSAR /* +RC 3.02 */
#ifndef _CPHAL_AAL2 /* +RC 3.02 */
/*
 *  Returns statistics information.
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())
 *
 *  @return 0
 */
static int StatsGet(HAL_DEVICE *HalDev, void **StatPtr, int Index, int Ch, int Queue)
  {
   int Size;
   bit32u *AddrPtr;
   char *DataPtr;
   STATS_TABLE *StatsTable;
   int i, NumberOfStats;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpcommon]StatsGet(HalDev:%08x, StatPtr:%08x)\n",
                (bit32u)HalDev, (bit32u)StatPtr);
      osfuncSioFlush();
     }
#endif

   StatsTable = StatsDb[Index].StatTable;
   NumberOfStats = StatsDb[Index].NumberOfStats;

   Size = sizeof(bit32u)*((NumberOfStats*2)+1);
   Size += (NumberOfStats*11);
   *StatPtr = (bit32u *)HalDev->OsFunc->Malloc(Size);

   AddrPtr = (bit32u *) *StatPtr;
   DataPtr = (char *)AddrPtr;
   DataPtr += sizeof(bit32u)*((NumberOfStats*2)+1);

   for (i=0; i<NumberOfStats; i++)
     {
      *AddrPtr++ = (bit32u)StatsTable[i].StatName;
      *AddrPtr++ = (bit32u)DataPtr;
      if (&StatsTable[i].StatPtr[Ch] != 0)
        {
         switch(StatsTable[i].DataType)
           {
            case 0:
              HalDev->OsFunc->Sprintf(DataPtr, "%d", (bit32u *)StatsTable[i].StatPtr[Ch]);
              break;
            case 1:
              HalDev->OsFunc->Sprintf(DataPtr, "0x%x", (bit32u *)StatsTable[i].StatPtr[Ch]);
              break;
            case 2:
              HalDev->OsFunc->Sprintf(DataPtr, "%d", *((bit32u *)StatsTable[i].StatPtr + (Ch * (sizeof(CHANNEL_INFO)/4))));
              break;
            case 3:
              HalDev->OsFunc->Sprintf(DataPtr, "%d", *((bit32u *)StatsTable[i].StatPtr + (Ch*MAX_QUEUE)+Queue));
              break;
            case 4:
              HalDev->OsFunc->Sprintf(DataPtr, "0x%x", *((bit32u *)StatsTable[i].StatPtr + (Ch*MAX_QUEUE)+Queue));
              break;
            default:
              /* invalid data type, due to CPHAL programming error */
              break;
           }
        }
       else
        {
         /* invalid statistics pointer, probably was not initialized */
        }
      DataPtr += HalDev->OsFunc->Strlen(DataPtr) + 1;
     }

   *AddrPtr = (bit32u) 0;

   return (EC_NO_ERRORS);
  }
#endif /* +RC 3.02 */
#endif /* +RC 3.02 */

#ifdef _CPHAL_CPMAC
static void gpioFunctional(int base, int bit)
  {                                                                  /*+RC3.02*/
  bit32u GpioEnr = base + 0xC;
  /*  To make functional, set to zero  */
  *(volatile bit32u *)(GpioEnr) &= ~(1 << bit);                      /*+RC3.02*/
  }                                                                  /*+RC3.02*/


/*+RC3.02*/
/* Common function, Checks to see if GPIO should be in functional mode */
static void gpioCheck(HAL_DEVICE *HalDev, void *moduleDeviceInfo)
  {                                                                  /*+RC3.02*/
  int rc;
  void *DeviceInfo;
  char *pszMuxBits;
  char pszMuxBit[20];
  char *pszTmp;
  char szMuxBit[20];
  char *ptr;
  int base;
  int reset_bit;
  int bit;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;

  rc = OsFunc->DeviceFindParmValue(moduleDeviceInfo, "gpio_mux",&pszTmp);
  if(rc) return;
  /*  gpio entry found, get GPIO register info and make functional  */

  /* temp copy until FinParmValue fixed */
  ptr = &szMuxBit[0];
  while ((*ptr++ = *pszTmp++));

  pszMuxBits = &szMuxBit[0];

  rc = OsFunc->DeviceFindInfo(0,"gpio",&DeviceInfo);
  if(rc) return;

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "base",&base);
  if(rc) return;

  rc = OsFunc->DeviceFindParmUint(DeviceInfo, "reset_bit",&reset_bit);
  if(rc) return;

  /* If GPIO still in reset, then exit  */
  if((VOLATILE32(HalDev->ResetBase) & (1 << reset_bit)) == 0)
    return;
  /*  format for gpio_mux is  gpio_mux = <int>;<int>;<int>...*/
  while (*pszMuxBits)
    {
    pszTmp = &pszMuxBit[0];
    if(*pszMuxBits == ';') pszMuxBits++;
    while ((*pszMuxBits != ';') && (*pszMuxBits != '\0'))
      {
      osfuncSioFlush();
      /*If value not a number, skip */
      if((*pszMuxBits < '0') || (*pszMuxBits > '9'))
        pszMuxBits++;
      else
        *pszTmp++ = *pszMuxBits++;
      }
    *pszTmp = '\0';
    bit = OsFunc->Strtoul(pszMuxBit, &pszTmp, 10);
    gpioFunctional(base, bit);
    resetWait(HalDev);  /* not sure if this is needed */
    }
  }                                                                  /*+RC3.02*/
#endif  /* CPMAC  */

#ifdef _CPHAL_AAL5
const char hcSarFrequency[] = "SarFreq";
#endif

#endif  /* _INC  */
