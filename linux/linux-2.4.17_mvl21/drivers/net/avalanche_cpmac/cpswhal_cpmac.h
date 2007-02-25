/************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE: cphal.h
 *
 *  DESCRIPTION:
 *      User include file, contains data definitions shared between the CPHAL
 *      and the upper-layer software.
 *
 *  HISTORY:
 *      Date      Modifier  Ver    Notes
 *      28Feb02   Greg      1.00   Original
 *      06Mar02   Greg      1.01   Documentation enhanced
 *      18Jul02   Greg      1.02   Many updates (OAM additions, general reorg)
 *      22Nov02   Mick      RC2    Additions from Denis' input on Control
 *
 *  author  Greg Guyotte
 *  version 1.02
 *  date    18-Jul-2002
 *****************************************************************************/
#ifndef _INC_CPHAL_H
#define _INC_CPHAL_H

#ifdef _CPHAL_CPMAC
#include "ec_errors_cpmac.h"
#endif

#ifdef _CPHAL_AAL5
#include "ec_errors_cpaal5.h"
#endif

#ifdef _CPHAL_CPSAR
#include "ec_errors_cpsar.h"
#endif

#ifdef _CPHAL_AAL2
#include "ec_errors_cpaal2.h"
#endif

#ifndef __ADAM2
typedef char           bit8;
typedef short          bit16;
typedef int            bit32;

typedef unsigned char  bit8u;
typedef unsigned short bit16u;
typedef unsigned int   bit32u;

/*
typedef char           INT8;
typedef short          INT16;
typedef int            INT32;
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
*/
/*typedef unsigned int   size_t;*/
#endif

#ifdef _CPHAL

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==2)
#endif

#ifndef NULL
#define NULL 0
#endif

#endif

#define        VirtToPhys(a)                 (((int)a)&~0xe0000000)
#define        VirtToVirtNoCache(a)          ((void*)((VirtToPhys(a))|0xa0000000))
#define        VirtToVirtCache(a)            ((void*)((VirtToPhys(a))|0x80000000))
#define        PhysToVirtNoCache(a)          ((void*)(((int)a)|0xa0000000))
#define        PhysToVirtCache(a)            ((void*)(((int)a)|0x80000000))
/*
#define        DataCacheHitInvalidate(a)     {__asm__(" cache  17, (%0)" :   : "r" (a));}
#define        DataCacheHitWriteback(a)      {__asm__(" cache  25, (%0)" :   : "r" (a));}
*/

#define PARTIAL 1     /**< Used in @c Close() and @c ChannelTeardown() */
#define FULL 2        /**< Used in @c Close() and @c ChannelTeardown() */

/* Channel Teardown Defines */
#define RX_TEARDOWN 2
#define TX_TEARDOWN 1
#define BLOCKING_TEARDOWN 8
#define FULL_TEARDOWN 4
#define PARTIAL_TEARDOWN 0

#define MAX_DIR 2
#define DIRECTION_TX 0
#define DIRECTION_RX 1
#define TX_CH 0
#define RX_CH 1
#define HAL_ERROR_DEVICE_NOT_FOUND    1
#define HAL_ERROR_FAILED_MALLOC       2
#define HAL_ERROR_OSFUNC_SIZE         3
#define HAL_DEFAULT  0xFFFFFFFF
#define VALID(val)    (val!=HAL_DEFAULT)

/*
ERROR REPORTING

HAL Module Codes.  Each HAL module reporting an error code
should OR the error code with the respective Module error code
from the list below.
*/
#define EC_AAL5                                EC_HAL|EC_DEV_AAL5
#define EC_AAL2                                EC_HAL|EC_DEV_AAL2
#define EC_CPSAR                               EC_HAL|EC_DEV_CPSAR
#define EC_CPMAC                               EC_HAL|EC_DEV_CPMAC
#define EC_VDMA                                EC_HAL|EC_DEV_VDMA
#define EC_VLYNQ                               EC_HAL|EC_DEV_VLYNQ
#define EC_CPPI                                EC_HAL|EC_DEV_CPPI

/*
HAL Function Codes.  Each HAL module reporting an error code
should OR the error code with one of the function codes from
the list below.
*/
#define EC_FUNC_HAL_INIT                       EC_FUNC(1)
#define EC_FUNC_CHSETUP                        EC_FUNC(2)
#define EC_FUNC_CHTEARDOWN                     EC_FUNC(3)
#define EC_FUNC_RXRETURN                       EC_FUNC(4)
#define EC_FUNC_SEND                           EC_FUNC(5)
#define EC_FUNC_RXINT                          EC_FUNC(6)
#define EC_FUNC_TXINT                          EC_FUNC(7)
#define EC_FUNC_AAL2_VDMA                      EC_FUNC(8)
#define EC_FUNC_OPTIONS                        EC_FUNC(9)
#define EC_FUNC_PROBE                          EC_FUNC(10)
#define EC_FUNC_OPEN                           EC_FUNC(11)
#define EC_FUNC_CONTROL                        EC_FUNC(12)
#define EC_FUNC_DEVICE_INT                     EC_FUNC(13)
#define EC_FUNC_STATUS                         EC_FUNC(14)
#define EC_FUNC_TICK                           EC_FUNC(15)
#define EC_FUNC_CLOSE                          EC_FUNC(16)
#define EC_FUNC_SHUTDOWN                       EC_FUNC(17)
#define EC_FUNC_DEVICE_INT_ALT                 EC_FUNC(18) /* +GSG 030306 */

/*
HAL Error Codes.  The list below defines every type of error
used in all HAL modules. DO NOT CHANGE THESE VALUES!  Add new
values in integer order to the bottom of the list.
*/
#define EC_VAL_PDSP_LOAD_FAIL                 EC_ERR(0x01)|EC_CRITICAL
#define EC_VAL_FIRMWARE_TOO_LARGE             EC_ERR(0x02)|EC_CRITICAL
#define EC_VAL_DEVICE_NOT_FOUND               EC_ERR(0x03)|EC_CRITICAL
#define EC_VAL_BASE_ADDR_NOT_FOUND            EC_ERR(0x04)|EC_CRITICAL
#define EC_VAL_RESET_BIT_NOT_FOUND            EC_ERR(0x05)|EC_CRITICAL
#define EC_VAL_CH_INFO_NOT_FOUND              EC_ERR(0x06)
#define EC_VAL_RX_STATE_RAM_NOT_CLEARED       EC_ERR(0x07)|EC_CRITICAL
#define EC_VAL_TX_STATE_RAM_NOT_CLEARED       EC_ERR(0x08)|EC_CRITICAL
#define EC_VAL_MALLOC_DEV_FAILED              EC_ERR(0x09)
#define EC_VAL_OS_VERSION_NOT_SUPPORTED       EC_ERR(0x0A)|EC_CRITICAL
#define EC_VAL_CPSAR_VERSION_NOT_SUPPORTED    EC_ERR(0x0B)|EC_CRITICAL
#define EC_VAL_NULL_CPSAR_DEV                 EC_ERR(0x0C)|EC_CRITICAL

#define EC_VAL_LUT_NOT_READY                  EC_ERR(0x0D)
#define EC_VAL_INVALID_CH                     EC_ERR(0x0E)
#define EC_VAL_NULL_CH_STRUCT                 EC_ERR(0x0F)
#define EC_VAL_RX_TEARDOWN_ALREADY_PEND       EC_ERR(0x10)
#define EC_VAL_TX_TEARDOWN_ALREADY_PEND       EC_ERR(0x11)
#define EC_VAL_RX_CH_ALREADY_TORNDOWN         EC_ERR(0x12)
#define EC_VAL_TX_CH_ALREADY_TORNDOWN         EC_ERR(0x13)
#define EC_VAL_TX_TEARDOWN_TIMEOUT            EC_ERR(0x14)
#define EC_VAL_RX_TEARDOWN_TIMEOUT            EC_ERR(0x15)
#define EC_VAL_CH_ALREADY_TORNDOWN            EC_ERR(0x16)
#define EC_VAL_VC_SETUP_NOT_READY             EC_ERR(0x17)
#define EC_VAL_VC_TEARDOWN_NOT_READY          EC_ERR(0x18)
#define EC_VAL_INVALID_VC                     EC_ERR(0x19)
#define EC_VAL_INVALID_LC                     EC_ERR(0x20)
#define EC_VAL_INVALID_VDMA_CH                EC_ERR(0x21)
#define EC_VAL_INVALID_CID                    EC_ERR(0x22)
#define EC_VAL_INVALID_UUI                    EC_ERR(0x23)
#define EC_VAL_INVALID_UUI_DISCARD            EC_ERR(0x24)
#define EC_VAL_CH_ALREADY_OPEN                EC_ERR(0x25)

#define EC_VAL_RCB_MALLOC_FAILED              EC_ERR(0x26)
#define EC_VAL_RX_BUFFER_MALLOC_FAILED        EC_ERR(0x27)
#define EC_VAL_OUT_OF_TCBS                    EC_ERR(0x28)
#define EC_VAL_NO_TCBS                        EC_ERR(0x29)
#define EC_VAL_NULL_RCB                       EC_ERR(0x30)|EC_CRITICAL
#define EC_VAL_SOP_ERROR                      EC_ERR(0x31)|EC_CRITICAL
#define EC_VAL_EOP_ERROR                      EC_ERR(0x32)|EC_CRITICAL
#define EC_VAL_NULL_TCB                       EC_ERR(0x33)|EC_CRITICAL
#define EC_VAL_CORRUPT_RCB_CHAIN              EC_ERR(0x34)|EC_CRITICAL
#define EC_VAL_TCB_MALLOC_FAILED              EC_ERR(0x35)

#define EC_VAL_DISABLE_POLLING_FAILED         EC_ERR(0x36)
#define EC_VAL_KEY_NOT_FOUND                  EC_ERR(0x37)
#define EC_VAL_MALLOC_FAILED                  EC_ERR(0x38)
#define EC_VAL_RESET_BASE_NOT_FOUND           EC_ERR(0x39)|EC_CRITICAL
#define EC_VAL_INVALID_STATE                  EC_ERR(0x40)
#define EC_VAL_NO_TXH_WORK_TO_DO              EC_ERR(0x41)
#define EC_VAL_NO_TXL_WORK_TO_DO              EC_ERR(0x42)
#define EC_VAL_NO_RX_WORK_TO_DO               EC_ERR(0x43)
#define EC_VAL_NOT_LINKED                     EC_ERR(0x44)
#define EC_VAL_INTERRUPT_NOT_FOUND            EC_ERR(0x45)
#define EC_VAL_OFFSET_NOT_FOUND               EC_ERR(0x46)
#define EC_VAL_MODULE_ALREADY_CLOSED          EC_ERR(0x47)
#define EC_VAL_MODULE_ALREADY_SHUTDOWN        EC_ERR(0x48)
#define EC_VAL_ACTION_NOT_FOUND               EC_ERR(0x49)
#define EC_VAL_RX_CH_ALREADY_SETUP            EC_ERR(0x50)
#define EC_VAL_TX_CH_ALREADY_SETUP            EC_ERR(0x51)
#define EC_VAL_RX_CH_ALREADY_OPEN             EC_ERR(0x52)
#define EC_VAL_TX_CH_ALREADY_OPEN             EC_ERR(0x53)
#define EC_VAL_CH_ALREADY_SETUP               EC_ERR(0x54)
#define EC_VAL_RCB_NEEDS_BUFFER               EC_ERR(0x55) /* +GSG 030410 */
#define EC_VAL_RCB_DROPPED                    EC_ERR(0x56) /* +GSG 030410 */
#define EC_VAL_INVALID_VALUE                  EC_ERR(0x57)

/**
@defgroup shared_data Shared Data Structures

The data structures documented here are shared by all modules.
*/

/**
 *  @ingroup shared_data
 *  This is the fragment list structure.  Each fragment list entry contains a
 *  length and a data buffer.
 */
typedef struct
  {
   bit32u   len;    /**< Length of the fragment in bytes (lower 16 bits are valid).  For SOP, upper 16 bits is the buffer offset. */
   void     *data;  /**< Pointer to fragment data. */
   void     *OsInfo; /**< Pointer to OS defined data. */
  }FRAGLIST;

#if defined (_CPHAL_CPMAC)
#define CB_PASSCRC_BIT     (1<<26)

/* CPMAC CPHAL STATUS */
#define CPMAC_STATUS_LINK               (1 << 0)
#define CPMAC_STATUS_LINK_DUPLEX        (1 << 1)  /* 0 - HD, 1 - FD */
#define CPMAC_STATUS_LINK_SPEED         (1 << 2)  /* 0 - 10, 1 - 100 */

/*   ADAPTER CHECK Codes */

#define CPMAC_STATUS_ADAPTER_CHECK             (1 << 7)
#define CPMAC_STATUS_HOST_ERR_DIRECTION        (1 << 8)
#define CPMAC_STATUS_HOST_ERR_CODE             (0xF << 9)
#define CPMAC_STATUS_HOST_ERR_CH               (0x7 << 13)

#define _CPMDIO_DISABLE                 (1 << 0)
#define _CPMDIO_HD                      (1 << 1)
#define _CPMDIO_FD                      (1 << 2)
#define _CPMDIO_10                      (1 << 3)
#define _CPMDIO_100                     (1 << 4)
#define _CPMDIO_NEG_OFF                 (1 << 5)
#define _CPMDIO_LOOPBK                  (1 << 16)
#define _CPMDIO_AUTOMDIX                (1 << 17)   /* Bit 16 and above not used by MII register */
#define _CPMDIO_NOPHY                   (1 << 20)
#endif

/**
 *  @ingroup shared_data
 *  Channel specific configuration information.  This structure should be
 *  populated by upper-layer software prior to calling @c ChannelSetup().  Any
 *  configuration item that can be changed on a per channel basis should
 *  be represented here.  Each module may define this structure with additional
 *  module-specific members.
 */
typedef struct
  {
   int Channel;        /**< Channel number. */
   int Direction;      /**< DIRECTION_RX(1) or DIRECTION_TX(0). */
   OS_SETUP *OsSetup;  /**< OS defined information associated with this channel. */

#if defined(_CPHAL_AAL5) || defined (_CPHAL_CPSAR) || defined (_CPHAL_CPMAC)
   int RxBufSize;      /**< Size (in bytes) for each Rx buffer.*/
   int RxBufferOffset; /**< Number of bytes to offset rx data from start of buffer (must be less than buffer size). */
   int RxNumBuffers;   /**< The number of Rx buffer descriptors to allocate for Ch. */
   int RxServiceMax;   /**< Maximum number of packets to service at one time. */

   int TxNumBuffers;   /**< The number of Tx buffer descriptors to allocate for Ch. */
   int TxNumQueues;    /**< Number of Tx queues for this channel (1-2). Choosing 2 enables a low priority SAR queue. */
   int TxServiceMax;   /**< Maximum number of packets to service at one time. */
#endif

#if defined(_CPHAL_AAL5) || defined(_CPHAL_CPSAR)
   int CpcsUU;      /**< The 2-byte CPCS UU and CPI information. */
   int Gfc;         /**< Generic Flow Control. */
   int Clp;         /**< Cell Loss Priority. */
   int Pti;         /**< Payload Type Indication. */
#endif

#if defined(_CPHAL_AAL2) || defined(_CPHAL_AAL5) || defined(_CPHAL_CPSAR)
   int DaMask;      /**< Specifies whether credit issuance is paused when Tx data not available. */
   int Priority;    /**< Priority bin this channel will be scheduled within. */
   int PktType;     /**< 0=AAL5,1=Null AAL,2=OAM,3=Transparent,4=AAL2. */
   int Vci;         /**< Virtual Channel Identifier. */
   int Vpi;         /**< Virtual Path Identifier. */
   int FwdUnkVc;    /**< Enables forwarding of unknown VCI/VPI cells to host. 1=enable, 0=disable. */

   /* Tx VC State */
   int TxVc_CellRate;  /**< Tx rate, set as clock ticks between transmissions (SCR for VBR, CBR for CBR). */
   int TxVc_QosType;   /**< 0=CBR,1=VBR,2=UBR,3=UBRmcr. */
   int TxVc_Mbs;       /**< Min Burst Size in cells.*/
   int TxVc_Pcr;       /**< Peak Cell Rate for VBR in clock ticks between transmissions. */

   bit32 TxVc_AtmHeader; /**< ATM Header placed on firmware gen'd OAM cells for this Tx Ch (must be big endian with 0 PTI). */
   int TxVc_OamTc;     /**< TC Path to transmit OAM cells for TX connection (0,1). */
   int TxVc_VpOffset;  /**< Offset to the OAM VP state table. */
   /* Rx VC State */
   int RxVc_OamCh;     /**< Ch to terminate rx'd OAM cells to be forwarded to the host. */
   int RxVc_OamToHost; /**< 0=do not pass, 1=pass. */
   bit32 RxVc_AtmHeader; /**< ATM Header placed on firmware gen'd OAM cells for this Rx conn (must be big endian with 0 PTI). */
   int RxVc_OamTc;     /**< TC Path to transmit OAM cells for RX connection (0,1). */
   int RxVc_VpOffset;  /**< Offset to the OAM VP state table. */
   /* Tx VP State */
   int TxVp_OamTc;     /**< TC Path to transmit OAM cells for TX VP connection (0,1). */
   bit32 TxVp_AtmHeader; /**< ATM Header placed on firmware gen'd VP OAM cells for this Tx VP conn (must be big endian with 0 VCI). */
   /* Rx VP State  */
   int RxVp_OamCh;     /**< Ch to terminate rx'd OAM cells to be forwarded to the host. */
   int RxVp_OamToHost; /**< 0=do not pass, 1=pass. */
   bit32 RxVp_AtmHeader; /**< ATM Header placed on firmware gen'd OAM cells for this Rx VP conn (must be big endian with 0 VCI). */
   int RxVp_OamTc;     /**< TC Path to transmit OAM cells for RX VP connection (0,1). */
   int RxVp_OamVcList; /**< Indicates all VC channels associated with this VP channel (one-hot encoded). */
#endif


#ifdef _CPHAL_VDMAVT
   bit32u RemFifoAddr; /* Mirror mode only. */
   bit32u FifoAddr;
   bit32  PollInt;
   bit32  FifoSize;
   int    Ready;
#endif

  }CHANNEL_INFO;

/*
 *  This structure contains each statistic value gathered by the CPHAL.
 *  Applications may access statistics data by using the @c StatsGet() routine.
 */
/* STATS */
#if defined(_CPHAL_AAL2) || defined(_CPHAL_AAL5) || defined(_CPHAL_CPSAR)
typedef struct
  {
   bit32u CrcErrors[16];
   bit32u LenErrors[16];
   bit32u DmaLenErrors[16];
   bit32u AbortErrors[16];
   bit32u StarvErrors[16];
   bit32u TxMisQCnt[16][2];
   bit32u RxMisQCnt[16];
   bit32u RxEOQCnt[16];
   bit32u TxEOQCnt[16][2];
   bit32u RxPacketsServiced[16];
   bit32u TxPacketsServiced[16][2];
   bit32u RxMaxServiced;
   bit32u TxMaxServiced[16][2];
   bit32u RxTotal;
   bit32u TxTotal;
  } STAT_INFO;
#endif

/*
 *  VDMA Channel specific configuration information
 */
#ifdef _CPHAL_AAL2
typedef struct
  {
   int Ch;           /**< Channel Number */
   int RemoteEndian; /**< Endianness of remote VDMA-VT device */
   int CpsSwap;      /**< When 0, octet 0 in CPS pkt located in LS byte of 16-bit word sent to rem VDMA device.  When 1, in MS byte. */
  }VdmaChInfo;
#endif

#ifndef _CPHAL
  typedef void HAL_DEVICE;
  typedef void HAL_PRIVATE;
  typedef void HAL_RCB;
  typedef void HAL_RECEIVEINFO;
#endif

/**
 *  @ingroup shared_data
 *  The HAL_FUNCTIONS struct defines the function pointers used by upper layer
 *  software.  The upper layer software receives these pointers through the
 *  call to xxxInitModule().
 */
typedef struct
  {
  int  (*ChannelSetup)    (HAL_DEVICE *HalDev, CHANNEL_INFO *Channel, OS_SETUP *OsSetup);
  int  (*ChannelTeardown) (HAL_DEVICE *HalDev, int Channel, int Mode);
  int  (*Close)           (HAL_DEVICE *HalDev, int Mode);
  int  (*Control)         (HAL_DEVICE *HalDev, const char *Key, const char *Action, void *Value);
  int  (*Init)            (HAL_DEVICE *HalDev);
  int  (*Open)            (HAL_DEVICE *HalDev);
  int  (*PacketProcessEnd) (HAL_DEVICE *HalDev);
  int  (*Probe)           (HAL_DEVICE *HalDev);
  int  (*RxReturn)        (HAL_RECEIVEINFO *HalReceiveInfo, int StripFlag);
  int  (*Send)            (HAL_DEVICE *HalDev, FRAGLIST *FragList, int FragCount, int PacketSize, OS_SENDINFO *OsSendInfo, bit32u Mode);
  int  (*Shutdown)        (HAL_DEVICE *HalDev);
  int  (*Tick)            (HAL_DEVICE *HalDev);

#ifdef _CPHAL_AAL5
  int  (*Kick) (HAL_DEVICE *HalDev, int Queue);
  void (*OamFuncConfig)   (HAL_DEVICE *HalDev, unsigned int OamConfig);
  void (*OamLoopbackConfig)   (HAL_DEVICE *HalDev, unsigned int OamConfig, unsigned int *LLID, unsigned int CorrelationTag);
  volatile bit32u* (*RegAccess)(HAL_DEVICE *HalDev, bit32u RegOffset);
  STAT_INFO*  (*StatsGetOld)(HAL_DEVICE *HalDev);
#endif
  } HAL_FUNCTIONS;

/**
 *  @ingroup shared_data
 *  The OS_FUNCTIONS struct defines the function pointers for all upper layer
 *  functions accessible to the CPHAL.  The upper layer software is responsible
 *  for providing the correct OS-specific implementations for the following
 *  functions. It is populated by calling InitModule() (done by the CPHAL in
 *  xxxInitModule().
 */
typedef struct
  {
  int   (*Control)(OS_DEVICE *OsDev, const char *Key, const char *Action, void *Value);
  void  (*CriticalOn)(void);
  void  (*CriticalOff)(void);
  void  (*DataCacheHitInvalidate)(void *MemPtr, int Size);
  void  (*DataCacheHitWriteback)(void *MemPtr, int Size);
  int   (*DeviceFindInfo)(int Inst, const char *DeviceName, void *DeviceInfo);
  int   (*DeviceFindParmUint)(void *DeviceInfo, const char *Parm, bit32u *Value);
  int   (*DeviceFindParmValue)(void *DeviceInfo, const char *Parm, void *Value);
  void  (*Free)(void *MemPtr);
  void  (*FreeRxBuffer)(OS_RECEIVEINFO *OsReceiveInfo, void *MemPtr);
  void  (*FreeDev)(void *MemPtr);
  void  (*FreeDmaXfer)(void *MemPtr);
  void  (*IsrRegister)(OS_DEVICE *OsDev, int (*halISR)(HAL_DEVICE*, int*), int InterruptBit);
  void  (*IsrUnRegister)(OS_DEVICE *OsDev, int InterruptBit);
  void* (*Malloc)(bit32u size);
  void* (*MallocDev)(bit32u Size);
  void* (*MallocDmaXfer)(bit32u size, void *MemBase, bit32u MemRange);
  void* (*MallocRxBuffer)(bit32u size, void *MemBase, bit32u MemRange,
                       OS_SETUP *OsSetup, HAL_RECEIVEINFO *HalReceiveInfo,
                       OS_RECEIVEINFO **OsReceiveInfo, OS_DEVICE *OsDev);
  void* (*Memset)(void *Dest, int C, bit32u N);
  int   (*Printf)(const char *Format, ...);
  int   (*Receive)(OS_DEVICE *OsDev,FRAGLIST *FragList,bit32u FragCount,
                 bit32u PacketSize,HAL_RECEIVEINFO *HalReceiveInfo, bit32u Mode);
  int   (*SendComplete)(OS_SENDINFO *OsSendInfo);
  int   (*Sprintf)(char *S, const char *Format, ...);
  int   (*Strcmpi)(const char *Str1, const char *Str2);
  unsigned int (*Strlen)(const char *S);
  char* (*Strstr)(const char *S1, const char *S2);
  unsigned long  (*Strtoul)(const char *Str, char **Endptr, int Base);
  void  (*TeardownComplete)(OS_DEVICE *OsDev, int Ch, int Direction);
  } OS_FUNCTIONS;

/************** MODULE SPECIFIC STUFF BELOW **************/

#ifdef _CPHAL_CPMAC

/*
int halCpmacInitModule(HAL_DEVICE **HalDev, OS_DEVICE *OsDev, HAL_FUNCTIONS *HalFunc, int (*osBridgeInitModule)(OS_FUNCTIONS *), void* (*osMallocDev) (bit32u), int *Size, int inst);
*/

int halCpmacInitModule(HAL_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 HAL_FUNCTIONS **HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);
#endif

#ifdef _CPHAL_AAL5
/*
 *  @ingroup shared_data
 *  The AAL5_FUNCTIONS struct defines the AAL5 function pointers used by upper layer
 *  software.  The upper layer software receives these pointers through the
 *  call to cphalInitModule().
 */
/*
typedef struct
  {
  int  (*ChannelSetup)(HAL_DEVICE *HalDev, CHANNEL_INFO *HalCh, OS_SETUP *OsSetup);
  int  (*ChannelTeardown)(HAL_DEVICE *HalDev, int Ch, int Mode);
  int  (*Close)(HAL_DEVICE *HalDev, int Mode);
  int  (*Init)(HAL_DEVICE *HalDev);
  int  (*ModeChange)(HAL_DEVICE *HalDev, char *DeviceParms);
  int  (*Open)(HAL_DEVICE *HalDev);
  int  (*InfoGet)(HAL_DEVICE *HalDev, int Key, void *Value);
  int  (*Probe)(HAL_DEVICE *HalDev);
  int  (*RxReturn)(HAL_RECEIVEINFO *HalReceiveInfo, int StripFlag);
  int  (*Send)(HAL_DEVICE *HalDev,FRAGLIST *FragList,int FragCount,
                 int PacketSize,OS_SENDINFO *OsSendInfo,int Ch, int Queue,
                 bit32u Mode);
  int  (*StatsClear)(HAL_DEVICE *HalDev);
  STAT_INFO*  (*StatsGet)(HAL_DEVICE *HalDev);
  int  (*Status)(HAL_DEVICE *HalDev);
  void (*Tick)(HAL_DEVICE *HalDev);
  int  (*Kick)(HAL_DEVICE *HalDev, int Queue);
  volatile bit32u* (*RegAccess)(HAL_DEVICE *HalDev, bit32u RegOffset);
  } AAL5_FUNCTIONS;
*/

int cpaal5InitModule(HAL_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 HAL_FUNCTIONS **HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);
#endif

#ifdef _CPHAL_AAL2
/**
 *  @ingroup shared_data
 *  The AAL2_FUNCTIONS struct defines the AAL2 function pointers used by upper layer
 *  software.  The upper layer software receives these pointers through the
 *  call to cphalInitModule().
 */
typedef struct
  {
  int  (*ChannelSetup)(HAL_DEVICE *HalDev, CHANNEL_INFO *HalCh, OS_SETUP *OsSetup);
  int  (*ChannelTeardown)(HAL_DEVICE *HalDev, int Ch, int Mode);
  int  (*Close)(HAL_DEVICE *HalDev, int Mode);
  int  (*Init)(HAL_DEVICE *HalDev);
  int  (*ModeChange)(HAL_DEVICE *HalDev, char *DeviceParms);
  int  (*Open)(HAL_DEVICE *HalDev);
  int  (*OptionsGet)(HAL_DEVICE *HalDev, char *Key, bit32u *Value);
  int  (*Probe)(HAL_DEVICE *HalDev);

  int  (*StatsClear)(HAL_DEVICE *HalDev);
  STAT_INFO*  (*StatsGet)(HAL_DEVICE *HalDev);
  int  (*Status)(HAL_DEVICE *HalDev);
  void (*Tick)(HAL_DEVICE *HalDev);
  int  (*Aal2UuiMappingSetup)(HAL_DEVICE *HalDev, int VC, int UUI,
                                    int VdmaCh, int UUIDiscard);
  int  (*Aal2RxMappingSetup)(HAL_DEVICE *HalDev, int VC, int CID,
                                   int LC);
  int  (*Aal2TxMappingSetup)(HAL_DEVICE *HalDev, int VC, int LC, int VdmaCh);
  int  (*Aal2VdmaChSetup)(HAL_DEVICE *HalDev, bit32u RemVdmaVtAddr,
                               VdmaChInfo *VdmaCh);
  volatile bit32u* (*RegAccess)(HAL_DEVICE *HalDev, bit32u RegOffset);
  int  (*Aal2ModeChange)(HAL_DEVICE *HalDev, int Vc, int RxCrossMode,
                           int RxMultiMode, int TxMultiMode, int SchedMode,
                           int TcCh);
  void (*Aal2VdmaEnable)(HAL_DEVICE *HalDev, int Ch);
  int  (*Aal2VdmaDisable)(HAL_DEVICE *HalDev, int Ch);
  } AAL2_FUNCTIONS;

int cpaal2InitModule(HAL_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 AAL2_FUNCTIONS **HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);
#endif

#ifdef _CPHAL_VDMAVT
/**
 *  @ingroup shared_data
 *  The VDMA_FUNCTIONS struct defines the HAL function pointers used by upper layer
 *  software.  The upper layer software receives these pointers through the
 *  call to InitModule().
 *
 *  Note that this list is still under definition.
 */
typedef struct
  {
  bit32  (*Init)( HAL_DEVICE *VdmaVtDev);
    /*  bit32  (*SetupTxFifo)(HAL_DEVICE *VdmaVtDev, bit32u LclRem,
                  bit32u Addr, bit32u Size, bit32u PollInt);
    bit32  (*SetupRxFifo)(HAL_DEVICE *VdmaVtDev, bit32u LclRem,
                  bit32u Addr, bit32u Size, bit32u PollInt); */
  bit32  (*Tx)(HAL_DEVICE *VdmaVtDev);
  bit32  (*Rx)(HAL_DEVICE *VdmaVtDev);
  bit32  (*SetRemoteChannel)(HAL_DEVICE *VdmaVtDev, bit32u RemAddr,
                         bit32u RemDevID);
  bit32  (*ClearRxInt)(HAL_DEVICE *VdmaVtDev);
  bit32  (*ClearTxInt)(HAL_DEVICE *VdmaVtDev);
  bit32  (*Open)(HAL_DEVICE *VdmaVtDev);
  bit32  (*Close)(HAL_DEVICE *VdmaVtDev);
  int    (*Control)         (HAL_DEVICE *HalDev, const char *Key, const char *Action, void *Value);
  int    (*ChannelSetup)(HAL_DEVICE *VdmaVtDev, CHANNEL_INFO *HalCh, OS_SETUP *OsSetup);
  int    (*ChannelTeardown)(HAL_DEVICE *VdmaVtDev, int Ch, int Mode);
  int    (*Send)(HAL_DEVICE *VdmaVtDev,FRAGLIST *FragList,int FragCount,
                 int PacketSize,OS_SENDINFO *OsSendInfo,bit32u Mode);
  } VDMA_FUNCTIONS;

int VdmaInitModule(HAL_DEVICE **VdmaVt,
                 OS_DEVICE *OsDev,
                 VDMA_FUNCTIONS **VdmaVtFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);
#endif

/*
extern int cphalInitModule(MODULE_TYPE ModuleType, HAL_DEVICE **HalDev, OS_DEVICE *OsDev, HAL_FUNCTIONS *HalFunc,
                      int (*osInitModule)(OS_FUNCTIONS *), void* (*osMallocDev)(bit32u),
                      int *Size, int Inst);
*/


#ifdef _CPHAL_AAL5
extern const char hcSarFrequency[];
#endif

#ifdef _CPHAL_CPMAC
/* following will be common, once 'utl' added */
extern const char hcClear[];
extern const char hcGet[];
extern const char hcSet[];
extern const char hcTick[];

extern const char hcCpuFrequency[];
extern const char hcCpmacFrequency[];
extern const char hcMdioBusFrequency[];
extern const char hcMdioClockFrequency[];
extern const char hcCpmacBase[];
extern const char hcPhyNum[];
extern const char hcSize[];
extern const char hcCpmacSize[];
extern const char hcPhyAccess[];
extern const char hcMdixMask[];
extern const char hcMdioMdixSwitch[];
#endif

#endif  /*  end of _INC_    */
