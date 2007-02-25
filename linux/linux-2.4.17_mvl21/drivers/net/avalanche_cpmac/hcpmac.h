/** @file***********************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE:
 *
 *  DESCRIPTION:
 *      This file contains definitions for the HAL EMAC API
 *
 *  HISTORY:
 *  xxXxx01 Denis      1.00  Original Version created.
 *  22Jan02 Denis/Mick 1.01  Modified for HAL EMAC API
 *  24Jan02 Denis/Mick 1.02  Speed Improvements
 *  28Jan02 Denis/Mick 1.16  Made function calls pointers
 *  28Jan02 Mick       1.18  Split into separate modules
 *  @author Michael Hanrahan
 *  @version 1.02
 *  @date    24-Jan-2002
 *****************************************************************************/
#ifndef _INC_HCPMAC
#define _INC_HCPMAC

/** \namespace CPMAC_Version
This documents version 01.07.04 of the CPMAC CPHAL.
*/
const char *pszVersion_CPMAC="CPMAC 01.07.08 "__DATE__" "__TIME__;

/* CHECK THESE LOCATIONS */
#define TEARDOWN_VAL       0xfffffffc
#define CB_OFFSET_MASK     0xFFFF0000


#define MAX_CHAN 8
#define MAX_QUEUE 1

typedef struct
  {
  bit32 HNext;      /*< Hardware's pointer to next buffer descriptor  */
  bit32 BufPtr;     /*< Pointer to the data buffer                    */
  bit32 Off_BLen;   /*< Contains buffer offset and buffer length      */
  bit32 mode;       /*< SOP, EOP, Ownership, EOQ, Teardown, Q Starv, Length */
  void *Next;
  void *OsInfo;
  void *Eop;
#ifdef __CPHAL_DEBUG
  bit32 DbgSop;
  bit32 DbgData;
  bit32 DbgFraglist;
#endif
  }HAL_TCB;

typedef volatile struct hal_private
  {
  bit32 HNext;      /*< Hardware's pointer to next buffer descriptor     */
  bit32 BufPtr;     /*< Pointer to the data buffer                       */
  bit32 Off_BLen;   /*< Contains buffer offset and buffer length         */
  bit32 mode;       /*< SOP, EOP, Ownership, EOQ, Teardown Complete bits */
  void *DatPtr;
  void *Next;
  void *OsInfo;
  void *Eop;
  }HAL_RCB;

#define MAX_NEEDS 512                                             /*MJH+030409*/
/*  HAL  */

typedef struct hal_device
  {
  OS_DEVICE *OsDev;
  OS_FUNCTIONS *OsFunc;
    /*OS_SETUP *OsSetup;*/                                     /* -GSG 030508 */
  int inst;
  bit32u rxbufseq;


  bit32 dev_base;
  bit32  offset;

  bit32u ResetBase; /* GSG 10/20 */
  int ResetBit;
  void *OsOpen;
  bit32u IntVec;
  PHY_DEVICE *PhyDev;
  bit32u EmacDuplex;
  bit32u EmacSpeed;
  bit32u PhyNum;
  bit32u MLinkMask;
  bit32u PhyMask;
  bit32u MdixMask;

  bit32u Linked;
  DEVICE_STATE State;
  unsigned char *MacAddr;
  HAL_FUNCTIONS *HalFuncPtr; /* GSG 11/20 changed name to match cppi */
  HAL_FUNCTIONS *HalFunc;
/*  unsigned int CpuFreq;*/                                       /*MJH-030402*/
  unsigned int MdioConnect;
  unsigned int HostErr;

/************************************************************************/
/*                                                                      */
/*      R E G I S T E R S                                               */
/*                                                                      */
/************************************************************************/

  bit32u RxMbpEnable;
  bit32u RxUnicastSet;
  bit32u RxUnicastClear;
  bit32u RxMaxLen;
  bit32u RxFilterLowThresh;
  bit32u Rx0FlowThresh;
  bit32u MacControl;
  bit32u MacStatus;
  bit32u MacHash1;
  bit32u MacHash2;

/************************************************************************/
/*                                                                      */
/*      O P T I O N S                                                   */
/*                                                                      */
/************************************************************************/

  char *DeviceInfo;
  bit32u interrupt;


  bit32u RxPassCrc;
  bit32u RxCaf;
  bit32u RxCef;
  bit32u RxBcast;
  bit32u RxBcastCh;
  HAL_RCB *RcbPool[MAX_CHAN];
  bit32 RxActQueueCount[MAX_CHAN];
  HAL_RCB *RxActQueueHead[MAX_CHAN];
  HAL_RCB *RxActQueueTail[MAX_CHAN];
  bit32 RxActive[MAX_CHAN];
  HAL_TCB *TcbPool[MAX_CHAN][MAX_QUEUE];
  bit32 TxActQueueCount[MAX_CHAN][MAX_QUEUE];
  HAL_TCB *TxActQueueHead[MAX_CHAN][MAX_QUEUE];
  HAL_TCB *TxActQueueTail[MAX_CHAN][MAX_QUEUE];
  bit32 TxActive[MAX_CHAN][MAX_QUEUE];
  bit32 TxTeardownPending[MAX_CHAN];
  bit32 RxTeardownPending[MAX_CHAN];
  bit32 ChIsOpen[MAX_CHAN][2];
  bit32 ChIsSetup[MAX_CHAN][2];
  FRAGLIST *fraglist;
  char *TcbStart[MAX_CHAN][MAX_QUEUE];
  char *RcbStart[MAX_CHAN];
  bit32 RcbSize[MAX_CHAN];
/*  STAT_INFO Stats;  */
  bit32  Inst;
  bit32u BuffersServicedMax;
  CHANNEL_INFO ChData[MAX_CHAN];
  bit32u MdioClockFrequency;                                      /*MJH+030402*/
  bit32u MdioBusFrequency;                                        /*MJH+030402*/
  bit32u CpuFrequency;                                            /*MJH+030402*/
  bit32u CpmacFrequency;                                          /*MJH+030403*/
  bit32u CpmacSize;                                               /*MJH+030425*/
  int debug;
  bit32u NeedsCount;                                              /*MJH+030409*/
  HAL_RECEIVEINFO *Needs[MAX_NEEDS];                              /*MJH+030409*/
  int MaxFrags;
  int TxIntThreshold[MAX_CHAN];                   /* MJH 040621  NSP Performance Update */
  int TxIntThresholdMaster[MAX_CHAN];             /* MJH 040827  NSP Performance Update */
  int TxIntDisable;                     /* MJH 040621  NSP Performance Update */  
  }HALDEVICE;

#define STATS_MAX 36

#define MACCONTROL_MASK    (TX_PTYPE|TX_PACE|TX_FLOW_EN|RX_FLOW_EN|CTRL_LOOPBACK)
#define RX_MBP_ENABLE_MASK                                       \
        (RX_PASS_CRC|RX_QOS_EN|RX_NO_CHAIN|                      \
         RX_CMF_EN|RX_CSF_EN|RX_CEF_EN|RX_CAF_EN|RX_PROM_CH_MASK|     \
         RX_BROAD_EN|RX_BROAD_CH_MASK|RX_MULT_EN|RX_MULT_CH_MASK)


#define MBP_UPDATE(Mask, On)                      \
         if(On)   HalDev->RxMbpEnable |=  Mask;   \
         else     HalDev->RxMbpEnable &= ~Mask

#define CONTROL_UPDATE(Mask, On)                  \
         if(On)   HalDev->MacControl |=  Mask;    \
         else     HalDev->MacControl &= ~Mask


#define UPDATE_TX_PTYPE(Value)       CONTROL_UPDATE(TX_PTYPE,Value)
#define UPDATE_TX_PACE(Value)        CONTROL_UPDATE(TX_PACE,Value)
#define UPDATE_MII_EN(Value)         CONTROL_UPDATE(MII_EN,Value)
#define UPDATE_TX_FLOW_EN(Value)     CONTROL_UPDATE(TX_FLOW_EN,Value)
#define UPDATE_RX_FLOW_EN(Value)     CONTROL_UPDATE(RX_FLOW_EN,Value)
#define UPDATE_CTRL_LOOPBACK(Value)  CONTROL_UPDATE(CTRL_LOOPBACK,Value)
#define UPDATE_FULLDUPLEX(Value)     CONTROL_UPDATE(FULLDUPLEX,(Value))

#define UPDATE_RX_PASS_CRC(Value)    MBP_UPDATE(RX_PASS_CRC,  Value)
#define UPDATE_RX_QOS_EN(Value)      MBP_UPDATE(RX_QOS_EN,    Value)
#define UPDATE_RX_NO_CHAIN(Value)    MBP_UPDATE(RX_NO_CHAIN,  Value)
#define UPDATE_RX_CMF_EN(Value)      MBP_UPDATE(RX_CMF_EN,    Value)
#define UPDATE_RX_CSF_EN(Value)      MBP_UPDATE(RX_CSF_EN,    Value)
#define UPDATE_RX_CEF_EN(Value)      MBP_UPDATE(RX_CEF_EN,    Value)
#define UPDATE_RX_CAF_EN(Value)      MBP_UPDATE(RX_CAF_EN,    Value)
#define UPDATE_RX_BROAD_EN(Value)    MBP_UPDATE(RX_BROAD_EN,  Value)
#define UPDATE_RX_MULT_EN(Value)     MBP_UPDATE(RX_MULT_EN,   Value)

#define UPDATE_RX_PROM_CH(Value)                         \
        HalDev->RxMbpEnable &=  ~RX_PROM_CH_MASK;        \
        HalDev->RxMbpEnable |=   RX_PROM_CH(Value)

#define UPDATE_RX_BROAD_CH(Value)                        \
           HalDev->RxMbpEnable &=  ~RX_BROAD_CH_MASK;    \
           HalDev->RxMbpEnable |=   RX_BROAD_CH(Value)

#define UPDATE_RX_MULT_CH(Value)                         \
           HalDev->RxMbpEnable &=  ~RX_MULT_CH_MASK;     \
           HalDev->RxMbpEnable |=   RX_MULT_CH(Value)



typedef enum
  {
   /*  CPMAC */
    enCpmacStart=0,
    enStats0,
    enStats1,
    enStats2,
    enStats3,
    enStats4,
    enStatsDump,
    enStatsClear,
    enRX_PASS_CRC,
    enRX_QOS_EN,
    enRX_NO_CHAIN,
    enRX_CMF_EN,
    enRX_CSF_EN,
    enRX_CEF_EN,
    enRX_CAF_EN,
    enRX_PROM_CH,
    enRX_BROAD_EN,
    enRX_BROAD_CH,
    enRX_MULT_EN,
    enRX_MULT_CH,

    enTX_PTYPE,
    enTX_PACE,
    enMII_EN,
    enTX_FLOW_EN,
    enRX_FLOW_EN,
    enCTRL_LOOPBACK,

    enRX_MAXLEN,
    enRX_FILTERLOWTHRESH,
    enRX0_FLOWTHRESH,
    enRX_UNICAST_SET,
    enRX_UNICAST_CLEAR,
    enMdioConnect,
    enMAC_ADDR_GET,
    enTick,
    enRX_MULTICAST,
    enRX_MULTI_ALL,
    enRX_MULTI_SINGLE,
    enVersion,
    enCpmacEnd                                   /* Last entry */
  }INFO_KEY_CPMAC;

static const char pszVersion[]                 = "Version";
static const char pszStats0[]                  = "Stats0";
static const char pszStats1[]                  = "Stats1";
static const char pszStats2[]                  = "Stats2";
static const char pszStats3[]                  = "Stats3";
static const char pszStats4[]                  = "Stats4";
static const char pszStatsDump[]               = "StatsDump";
static const char pszStatsClear[]              = "StatsClear";

/********************************************************************
**
**  RX MBP ENABLE
**
********************************************************************/
static const char pszRX_PASS_CRC[]             = "RX_PASS_CRC";
static const char pszRX_QOS_EN[]               = "RX_QOS_EN";
static const char pszRX_NO_CHAIN[]             = "RX_NO_CHAIN";
static const char pszRX_CMF_EN[]               = "RX_CMF_EN";
static const char pszRX_CSF_EN[]               = "RX_CSF_EN";
static const char pszRX_CEF_EN[]               = "RX_CEF_EN";
static const char pszRX_CAF_EN[]               = "RX_CAF_EN";
static const char pszRX_PROM_CH[]              = "RX_PROM_CH";
static const char pszRX_BROAD_EN[]             = "RX_BROAD_EN";
static const char pszRX_BROAD_CH[]             = "RX_BROAD_CH";
static const char pszRX_MULT_EN[]              = "RX_MULT_EN";
static const char pszRX_MULT_CH[]              = "RX_MULT_CH";


/********************************************************************
**
**  MAC CONTROL
**
********************************************************************/
static const char pszTX_PTYPE[]                = "TX_PTYPE";
static const char pszTX_PACE[]                 = "TX_PACE";
static const char pszMII_EN[]                  = "MII_EN";
static const char pszTX_FLOW_EN[]              = "TX_FLOW_EN";
static const char pszRX_FLOW_EN[]              = "RX_FLOW_EN";
static const char pszCTRL_LOOPBACK[]           = "CTRL_LOOPBACK";

static const char pszRX_MAXLEN[]               = "RX_MAXLEN";
static const char pszRX_FILTERLOWTHRESH[]      = "RX_FILTERLOWTHRESH";
static const char pszRX0_FLOWTHRESH[]          = "RX0_FLOWTHRESH";
static const char pszRX_UNICAST_SET[]          = "RX_UNICAST_SET";
static const char pszRX_UNICAST_CLEAR[]        = "RX_UNICAST_CLEAR";
static const char pszMdioConnect[]             = "MdioConnect";
static const char pszMacAddr[]                 = "MacAddr";
static const char pszTick[]                    = "Tick";

/********************************************************************
**
**  MULTICAST
**
********************************************************************/

static const char pszRX_MULTICAST[]            = "RX_MULTICAST";
static const char pszRX_MULTI_ALL[]            = "RX_MULTI_ALL";
static const char pszRX_MULTI_SINGLE[]         = "RX_MULTI_SINGLE";

/*
static const char* pszGFHN = "GFHN";
*/

static const CONTROL_KEY KeyCpmac[] =
   {
     {""                           , enCpmacStart},
     {pszStats0                    , enStats0},
     {pszStats1                    , enStats1},
     {pszStats2                    , enStats2},
     {pszStats3                    , enStats3},
     {pszStats4                    , enStats4},
     {pszStatsClear                , enStatsClear},
     {pszStatsDump                 , enStatsDump},
     {pszRX_PASS_CRC               , enRX_PASS_CRC},
     {pszRX_QOS_EN                 , enRX_QOS_EN},
     {pszRX_NO_CHAIN               , enRX_NO_CHAIN},
     {pszRX_CMF_EN                 , enRX_CMF_EN},
     {pszRX_CSF_EN                 , enRX_CSF_EN},
     {pszRX_CEF_EN                 , enRX_CEF_EN},
     {pszRX_CAF_EN                 , enRX_CAF_EN},
     {pszRX_PROM_CH                , enRX_PROM_CH},
     {pszRX_BROAD_EN               , enRX_BROAD_EN},
     {pszRX_BROAD_CH               , enRX_BROAD_CH},
     {pszRX_MULT_EN                , enRX_MULT_EN},
     {pszRX_MULT_CH                , enRX_MULT_CH},

     {pszTX_PTYPE                  , enTX_PTYPE},
     {pszTX_PACE                   , enTX_PACE},
     {pszMII_EN                    , enMII_EN},
     {pszTX_FLOW_EN                , enTX_FLOW_EN},
     {pszRX_FLOW_EN                , enRX_FLOW_EN},
     {pszCTRL_LOOPBACK             , enCTRL_LOOPBACK},
     {pszRX_MAXLEN                 , enRX_MAXLEN},
     {pszRX_FILTERLOWTHRESH        , enRX_FILTERLOWTHRESH},
     {pszRX0_FLOWTHRESH            , enRX0_FLOWTHRESH},
     {pszRX_UNICAST_SET            , enRX_UNICAST_SET},
     {pszRX_UNICAST_CLEAR          , enRX_UNICAST_CLEAR},
     {pszMdioConnect               , enMdioConnect},
     {pszRX_MULTICAST              , enRX_MULTICAST},
     {pszRX_MULTI_ALL              , enRX_MULTI_ALL},
     {pszRX_MULTI_SINGLE           , enRX_MULTI_SINGLE},
     {pszTick                      , enTick},
     {pszVersion                   , enVersion},
     {""                           , enCpmacEnd}
   };

const char hcCpuFrequency[]       = "CpuFreq";
const char hcCpmacFrequency[]     = "CpmacFrequency";
const char hcMdioBusFrequency[]   = "MdioBusFrequency";
const char hcMdioClockFrequency[] = "MdioClockFrequency";
const char hcCpmacBase[]          = "CpmacBase";
const char hcPhyNum[]             = "PhyNum";
const char hcSize[]               = "size";
const char hcCpmacSize[]          = "CpmacSize";
const char hcPhyAccess[]          = "PhyAccess";
const char hcLinked[]             = "Linked";
const char hcFullDuplex[]         = "FullDuplex";
const char hcMdixMask[]           = "MdixMask";
const char hcMdioMdixSwitch[]     = "MdixSet";
#endif
