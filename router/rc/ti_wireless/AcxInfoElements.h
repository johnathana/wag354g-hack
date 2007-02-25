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
**|     Copyright (c) 1998-2001 Texas Instruments Incorporated           |**
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

/****************************************************************************
 *
 *   MODULE:  whalAcxInfoElements.h                
 *   PURPOSE: Contains information element defines/structures 
 *				used by the ACX100 and host               
 *   
 ****************************************************************************/

#ifndef _WHAL_ACX_INFO_ELEMENTS_H
#define _WHAL_ACX_INFO_ELEMENTS_H
#include "whal.h"

/*
 * Information element identification field
 * ----------------------------------------
 */
#define ACX_ID		0x0000	/* ACX100 Info elements */
#define DOT11_ID	0x1000	/* 802.11 Info elements */

#define ACX_TIMER         			(ACX_ID | 0x0001)
#define ACX_PWRMGMT_OPT				(ACX_ID | 0x0002)
#define	ACX_ELM_ID_QUEUE_CONF	(ACX_ID | 0x0003)
#define ACX_ELM_ID_MEM_BLOCKSIZE (ACX_ID | 0x0004)
#define	ACX_ELM_ID_MEM_CONF		(ACX_ID | 0x0005)
#define	ACX_RATE_FALLBACK		(ACX_ID | 0x0006)
#define	ACX_ELM_ID_WEP_OPT		(ACX_ID | 0x0007)
#define	ACX_ELM_ID_MEM_MAP		(ACX_ID | 0x0008)  
/* Reserved                                   0x0009 */
#define	ACX_AID						(ACX_ID | 0x000A)
/* Reserved                                   0x000B */
#define	ACX_CONFIG					(ACX_ID | 0x000C)
#define ACX_ELM_ID_FW_REV		(ACX_ID | 0x000D)
#define	ACX_ELM_ID_FCS_ERROR_CNT (ACX_ID | 0x000E)
#define	ACX_MEDIUM_USAGE			(ACX_ID | 0x000F)
#define	ACX_ELM_ID_RX_CONFIG	(ACX_ID | 0x0010)
#define ACX_QUEUE_THRESH            (ACX_ID | 0x0011)
#define ACX_ELM_ID_BSS_IN_PS    (ACX_ID | 0x0012)
#define ACX_ELM_ID_STATISTICS   (ACX_ID | 0x0013)
#define ACX_INT_CONFIG          (ACX_ID | 0x0014)
#define ACX_FEATURE_CONFIG		(ACX_ID | 0x0015)
#define MAX_ACX_IE					((ACX_FEATURE_CONFIG +1)& ~ACX_ID)  /* SEE NOTE */

#define DOT11_ELM_ID_STATION_ID	(DOT11_ID | 0x0001) 
#define DOT11_PWRMGMT_MODE			(DOT11_ID | 0x0002)
/* Reserved                                     0x0003 */
/* Reserved                                     0x0004 */
#define DOT11_SHORT_RETRY_LIMIT	(DOT11_ID | 0x0005)
#define DOT11_LONG_RETRY_LIMIT	(DOT11_ID | 0x0006)
#define DOT11_WEP_DEFAULT_KEYS	(DOT11_ID | 0x0007)
#define DOT11_MAX_LIFETIME		(DOT11_ID | 0x0008)
#define DOT11_GROUP_ADDR			(DOT11_ID | 0x0009)
#define DOT11_CUR_REG_DOMAIN	(DOT11_ID | 0x000A)
#define DOT11_CUR_ANT			    (DOT11_ID | 0x000B)
/* Reserved                                     0x000C */
#define DOT11_CUR_PWR_LVL			(DOT11_ID | 0x000D)
#define DOT11_CUR_CCA_MODE			(DOT11_ID | 0x000E)
#define DOT11_ED_THRESHOLD			(DOT11_ID | 0x000F)  
#define DOT11_WEP_DEFAULT_KEY_ID (DOT11_ID | 0x0010)
#define DOT11_CUR_5GHZ_REG_DOMAIN   (DOT11_ID | 0x0011)
#define MAX_DOT11_IE				((DOT11_WEP_DEFAULT_KEY_ID + 1) & ~DOT11_ID)

#define MAX_INFO_ELE MAX_DOT11_IE + 1 	/* the largest of any type of info ele		*/
#define ID_TYPE_MASK 0xF000				/* the first 4 bits define the table to use */
#define ID_INDEX	0x0FFF


/*
 * Information element header
 * --------------------------
 */
typedef struct 
{
	UINT16	Id;
	UINT16	Len;
} ElmHdr_T;

/*
 * Queue configuration information element
 * ---------------------------------------
 */
#pragma pack(1)
typedef struct 
{
	UINT32	QueueAddr;
	UINT8	QueuePri;
	UINT8	Reserved1;  /* Pads */
	UINT16	Reserved2;
} TxQueueConfig_T;
#pragma pack()

#define RX_QUEUE_AUTO_RESET	1

#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT32	QueueAreaSize;
	UINT32  RxQueueAddress;
	UINT8	RxQueueOptions;
	UINT8	NumTxQueues;
	UINT16	Pad;
	UINT32	QueueIndicatorAddress;
	UINT32	HostQueueIndicatorAddress;
	TxQueueConfig_T TxQueueConfig[/* 2 */NUM_TX_QUEUES];
} AcxElm_QueueConfig_T;
#pragma pack()

/*
 * Memory configuration information element
 * ----------------------------------------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT32	DMAConfig;
	UINT32	*HostReceiveIndDesc;
	UINT32	*RxMemBlockHead;
	UINT32	*TxMemBlockHead;
	UINT16	RxMemBlockNum;
	UINT16	TxMemBlockNum;
} AcxElm_MemoryConfigOptions_T;
#pragma pack()


/*
 * Memory configuration information element
 * ----------------------------------------
 */
#define IDX_WEP_CACHE_START	3
#define IDX_WEP_CACHE_END	4
#define IDX_TEMPLATE_START	5
#define IDX_QUEUE_START		7
#define IDX_POOL_START		9
#define MEM_MAP_NUM_FILEDS	10
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;				
	UINT8	*CodeStart;
	UINT8	*CodeEnd;
	UINT8	*WEPCacheStart;
	UINT8	*WEPCacheEnd;
	UINT8	*PacketTemplateStart;
	UINT8	*PacketTemplateEnd;
	UINT8	*QueueMemoryStart;
	UINT8	*QueueMemoryEnd;
	UINT8	*PacketMemoryPoolStart;
	UINT8	*PacketMemoryPoolEnd;
} AcxElm_MemoryMap_T;
#pragma pack()

/*
 * Rx configuration (filter) information element
 * ---------------------------------------------
 */
#define RX_CFG_PROMISCUOUS				0x0008	/* promiscuous - receives all valid frames */
#define RX_CFG_BSSID					0x0020	/* receives frames from any BSSID */
#define RX_CFG_MAC						0x0010	/* receives frames destined to any MAC address */
#ifdef	USE_OLD_WRONG__FILTER_CONFIG
#define RX_CFG_ENABLE_ANY_DEST_MAC		0x0010	
#define RX_CFG_ENABLE_ONLY_MY_BSSID		0x0000  
#define RX_CFG_ENABLE_ANY_BSSID			0x0020  
#else
#define RX_CFG_ENABLE_ANY_DEST_MAC		0x0000	
#define RX_CFG_ENABLE_ONLY_MY_BSSID		0x0020  
#define RX_CFG_ENABLE_ANY_BSSID			0x0000  
#endif
#define RX_CFG_DISABLE_BCAST			0x0200	/* discards all broadcast frames */
#define RX_CFG_ENABLE_ONLY_MY_SSID		0x0400

/* defines for ConfigOptions */
#define CFG_RX_RAW			0x0002	/* 1 = write all data from baseband to frame buffer */
										/* including PHY header */
#define CFG_RX_FCS			0x0004	/* 1 = write FCS to end of frame in memory */
										/* 0 = do not write FCS to memory */
#define CFG_RX_ALL_GOOD		0x0008	/* promiscuous mode, receive all good frames */
#define CFG_UNI_FILTER_EN	0x0010	/* local MAC address filter enable */
#define CFG_BSSID_FILTER_EN 0x0020  /* BSSID filter enable */
#define CFG_MC_FILTER_EN	0x0040	/* 0 = receive all multicast */
										/* 1 = use one or both multicast address filters */
#define CFG_MC_ADDR0_EN		0x0080	/* 1 = receive frames from mc_addr0 */
										/* 0 = do not use this filter */
#define CFG_MC_ADDR1_EN		0x0100  /* 1 = receive frames from mc_addr1 */
										/* 0 = do not use this filter  */
#define CFG_BC_REJECT_EN	0x0200	/* 0 = receive all broadcast 1 = filter all broadcast */
#define CFG_SSID_FILTER_EN  0x0400  /*  SSID Filter Enable */
#define CFG_INT_FCS_ERROR	0x0800	/* 1 = give rx complete interrupt for FCS errors */
#define CFG_INT_WEP_ONLY	0x1000	/* 1 = only give rx header interrupt if frame is encrypted */
#define CFG_WR_RX_STATUS	0x2000  /* 1 = write three receive status words to top of rx’d MPDU */
										/* 0 = do not write three
										 * status words (added rev 1.5)
										 * */
#define CONFIG_OPTIONS_DEFAULT	(CFG_WR_RX_STATUS | CFG_UNI_FILTER_EN ) /* this value is what ConfigOptions will be */
										/* Initialized to by the ACX100 */

/* defines for Filter Options */
/* Rx Filter Enables    The rx filter enables control what type of receive frames will be
   rejected or received by the rx hardware   1 = frame is written to memory,
   0 = not written to memory, rejected */

#define CFG_RX_RSV_EN		0x0001	/* reserved types and subtypes */
#define CFG_RX_RCTS_ACK		0x0002	/* rts, cts, ack frames */
#define CFG_RX_PRSP_EN		0x0004	/* probe response */
#define CFG_RX_PREQ_EN		0x0008	/* probe request */
#define CFG_RX_MGMT_EN		0x0010	/* type = management */
#define CFG_RX_FCS_ERROR	0x0020	/* frames with FCS errors */
#define CFG_RX_DATA_EN		0x0040	/* type = data */
#define CFG_RX_CTL_EN		0x0080	/* type = control */
#define CFG_RX_CF_EN		0x0100	/* contention free frames */
#define CFG_RX_BCN_EN		0x0200	/* beacons */
#define CFG_RX_AUTH_EN		0x0400	/* authentication, deauthentication */
#define CFG_RX_ASSOC_EN		0x0800	/* association related frames (all 5 subtypes – assoc req/resp,
									 * reassoc req/resp, disassoc)  */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T  ElmHdr;
	UINT16	ConfigOptions;
	UINT16	FilterOptions;
} AcxElm_RxConfig_T;
#pragma pack()

/*
 * Bss Power Save
 * --------------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	BssInPs;
	UINT8		Pad;
} AcxElm_BssPowerSave_T;  
#pragma pack()

/*
 * Station Id information element
 * ------------------------------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	StationId[6];
} AcxElm_StationId_T;  
#pragma pack()

/*
 * Rate fallback information element
 * ---------------------------------
 */
#define RATE_FALLBACK_DISABLED	0
#define RATE_FALLBACK_3_RETRIES	3
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	RateFallback;
	UINT8		Pad;
} AcxElm_RateFallback_T;  
#pragma pack()

/*
 * Current Regulatory domain
 * -------------------------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	Domain;
	UINT8		Pad;
} AcxElm_CurrRegDomain_T;  
#pragma pack()

/*
 * Retry limit
 * -----------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	RetryLimit;
	UINT8		Pad;
} AcxElm_RetryLimit_T;  
#pragma pack()



/*
 * Power Level
 * -----------
 */
#pragma pack(1)
typedef struct 
{
	ElmHdr_T	ElmHdr;
	UINT8    	PowerLevel;
	UINT8		Pad;
} AcxElm_PowerLevel_T;  
#pragma pack()


/*
 * Acx100 Firmaware version
 * ------------------------
 */
#define FW_VERSION_LEN	20
#define HW_VERSION_LEN	4

#pragma pack(1)
typedef struct
{
	ElmHdr_T	ElmHdr;
	UINT8    	FwVersion[FW_VERSION_LEN];
	UINT8		HwVersion[HW_VERSION_LEN];
} AcxElm_AcxVersion_T; 
#pragma pack()


/*
 * Config options information element
 * ----------------------------------
 */
#define ACX_MAX_ANTENNAS	2
#define ELM_ID_ANTENNAS		0
typedef struct
{
    UINT8	ElmId;
    UINT8	Num;
    UINT8	Table[ ACX_MAX_ANTENNAS ];
} WlanElm_Antennas_T;
    
/*
 * Power level information element
 * -------------------------------
 */
#define ACX_MAX_POWER_LEVELS	8
#define ELM_ID_POWER_LEVELS		1
typedef struct
{
    UINT8	ElmId;    
    UINT8	Num;
    UINT16	Table[ ACX_MAX_POWER_LEVELS ];
} WlanElm_SupportedPowerLevels_T;


/*
 * Supported rates information element
 * -----------------------------------
 */
#define SUPPORTED_RATES_MAX		8
#define ELM_ID_SUPPORTED_RATES	2
typedef struct
{
    UINT8	ElmId;    
    UINT8	Num;
    UINT8	Table[ SUPPORTED_RATES_MAX ];
} WlanElm_SupportedDataRates_T;

    
/*
 * reg domains information element
 * -------------------------------
 */
#define ACX_MAX_REG_DOMAINS		6
#define ELM_ID_REG_DOMAINS		3
typedef struct
{
    UINT8	ElmId;    
    UINT8	Num;
    UINT8	Table[ ACX_MAX_REG_DOMAINS ];
} WlanElm_RegDomaind_T;
 
 
/*
 * Product Id information element
 * -------------------------------
 */
#define DOT11_MAX_PRODUCT_ID_STRING_LENGTH 128
#define ELM_ID_PRODUCT_ID		4
typedef struct 
{
    UINT8	ElmId;    
    UINT8	Num;
    UINT8	Table[ DOT11_MAX_PRODUCT_ID_STRING_LENGTH ];
} WlanElm_ProductId_T;
    
/*
 * Manufacture Id information element
 * ----------------------------------
 */
#define DOT11_MAX_MANUF_ID_STRING_LENGTH   128
#define ELM_ID_MANUFACTOR_ID	5
typedef struct 
{
    UINT8	ElmId;    
    UINT8	Num;
    UINT8	Table[ DOT11_MAX_MANUF_ID_STRING_LENGTH ];
} WlanElm_ManufactorId_T;

#define ACX_MAX_NVS_VERSION_LENGTH		8

/*
 * Config Options information element
 * ----------------------------------
 */
#pragma pack(1)
typedef struct
{
#if 0 /* (!!!) not present */
    UINT16	ConfigOptionsID;    // 0x000C   
    UINT16  ConfigOptionsLength;
#endif
    char    NVSVersion[ ACX_MAX_NVS_VERSION_LENGTH ];
    UINT8   dot11MacAddress[6];
    UINT16  ProbeDelay;
    UINT32  EndofMemory;
    UINT8   dot11CCAModeSupported;  
    UINT8   dot11DiversitySupport;
    UINT8   dot11ShortPreambleOptionImplemented;
    UINT8   dot11PBCCOptionImplemented; 
    UINT8   dot11ChanneAgilityPresent;
    UINT8   dot11PHYType;
    UINT8   dot11TempType;
} AcxElm_ConfigOptionsFixed_T;
#pragma pack()

#pragma pack(1)
typedef struct
{
    AcxElm_ConfigOptionsFixed_T		ConfigOptionsFixed;

    WlanElm_Antennas_T				AntennaList;
    WlanElm_SupportedPowerLevels_T	SupportedPowerLevels;
    WlanElm_SupportedDataRates_T	SupportedDataRates;
    WlanElm_RegDomaind_T			SupportedRegDomains;
    WlanElm_ProductId_T				ProductId;
    WlanElm_ManufactorId_T			ManufactureId;
} AcxElm_ConfigOptions_T; 
#pragma pack()

/*
 * Interrupt config
 * ----------------
 */
#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT32		RxDataThreshold;    /* set by host, in bytes			*/
	UINT32		RxDataTimeout;      /* set by host, in uS				*/
    UINT32		RxDataWatermark;    /* current value queued, in bytes	*/
    UINT32		RxDataTime;         /* current time remaining in uS, if RxDataFrames is non-zero */
    UINT8		RxDataFrames;       /* current #frames queued			*/
} AcxElm_IntConfig_T;
#pragma pack()


/*
 * Default Keys information element
 * --------------------------------
 */
#define KEY_SIZE_232_BITS		232
#define KEY_SIZE_104_BITS		104
#define KEY_SIZE_040_BITS		40
#define KEY_SIZE_MAX_BYTES		32

#define WEP_ACTION_ADD	1
#define WEP_ACTION_DEL	2

#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT8	Action;				/* Add/Remove */
	UINT8	KeySize;			/* 40, 10-4, 232 bits */
	UINT8   DefaultKeyNumber;	/* 0..3 */
	UINT8   Key[KEY_SIZE_MAX_BYTES]; 
} WlanElm_WepDefaultKeys_T;
#pragma pack()

/*
 * WEP options information element
 * -------------------------------
 */
#define WEP_NUM_DEFAULT_KEYS	4
#define WEP_NUM_MAPPING_KEYS	10

#define DECRYPT_DISABLE			1
#define DECRYPT_ENABLE			0

#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT16	MaxNumKeys;
	UINT8	DecryptOpt;
	UINT8	Pad;
} AcxElm_WepOptions_T;
#pragma pack()

/*
 * Default Key Id information element
 * ----------------------------------
 */
#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT8   DefaultKeyId;
	UINT8   Pad;
} WlanElm_WepDefaultKeyId_T;
#pragma pack()

/*
 * Fcs Error Count
 * ---------------
 */
#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT32	  FcsErrorCount;
} WlanElm_FcsError_T;
#pragma pack()

/*
 * Max Tx Msdu lifetime
 * --------------------
 */
#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT32	  MaxTxMsduLifetime;
} WlanElm_MaxTxMsduLifetime_T;
#pragma pack()

/*
 * Memory block size
 * --------------------
 */
#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
	UINT16	  MemBlockSize;
} WlanElm_MemBlockSize_T;
#pragma pack()

/*
 * Statistics from _INFOELE_H for FW version 1.0.8
 * -----------------------------------------------
 */

#if 1

#ifndef uint32
#define uint32 UINT32
#endif

#define KEEP_MEM_STATS               /* Remove this define to remove memory stats */
   
#ifdef KEEP_MEM_STATS
typedef struct MemStatistics_t {
    uint32 HighMem;          /* High Water mark for memory (Highest ever) */
    uint32 LowMem;           /* Low water mark for memory (Lowest ever) */
    uint32 CurrentMem;       /* Current mem level */
} MemStatistics_t;
#endif

typedef struct
{
   uint32* Head;			/* the first memory block in the list */
   uint32* Tail;			/* the last block in the list */
#ifdef KEEP_MEM_STATS
   MemStatistics_t stat;    /* Statisich for the memory pool */ 
#endif
}MemListStruct;

typedef struct TxStatistics_t {
    uint32 TxInternalDescOverflow;
} TxStatistics_t;

typedef struct RxStatistics_t {
    uint32 RxOutOfMem;
    uint32 RxHdrOverflow;
    uint32 RxHdrUseNext;
    uint32 RxDroppedFrame;
    uint32 RxFrmPtr_Err;
    uint32 RxXfrHintTrig;
} RxStatistics_t;


typedef struct DMAStatistics_t {
    uint32 RxDMARequested;
    uint32 RxDMAErrors;
    uint32 TxDMARequested;
    uint32 TxDMAErrors;
} DMAStatistics_t;

    
typedef struct IsrStatistics_t {
	uint32 Cmd_Cmplt;         /* Host command complete */
	uint32 FIQs;              /* fiqisr() */
	uint32 RxHeaders;         /* (INT_STS_ND & INT_TRIG_RX_HEADER) */
	uint32 RxCompletes;       /* (INT_STS_ND & INT_TRIG_RX_CMPLT) */
	uint32 RxMemOverflow;     /* (INT_STS_ND & INT_TRIG_NO_RX_BUF) */
	uint32 RxRdys;            /* (INT_STS_ND & INT_TRIG_S_RX_RDY) */
	uint32 IRQs;              /* irqisr() */
	uint32 ACXTxProcs;        /* (INT_STS_ND & INT_TRIG_TX_PROC) */
	uint32 DecryptDone;       /* (INT_STS_ND & INT_TRIG_DECRYPT_DONE) */
	uint32 DMA0Done;          /* (INT_STS_ND & INT_TRIG_DMA0) */
	uint32 DMA1Done;          /* (INT_STS_ND & INT_TRIG_DMA1) */
	uint32 ACXTxExchComplete; /* (INT_STS_ND & INT_TRIG_TX_EXC_CMPLT) */
	uint32 ACXCommands;       /* (INT_STS_ND & INT_TRIG_COMMAND) */
	uint32 ACXRxProcs;        /* (INT_STS_ND & INT_TRIG_RX_PROC) */
	uint32 HwPMModeChanges;	  /* (INT_STS_ND & INT_TRIG_PM_802) */
	uint32 HostAcknowledges;  /* (INT_STS_ND & INT_TRIG_ACKNOWLEDGE) */
	uint32 PCI_PM;            /* (INT_STS_ND & INT_TRIG_PM_PCI) */
	uint32 ACMWakeups;        /* (INT_STS_ND & INT_TRIG_ACM_WAKEUP) */

} IsrStatistics_t;

typedef struct WepStatistics_t
{ 
    uint32 WepAddrKeyCount;      /* Count of Wep address keys configured */ 
    uint32 WepDefaultKeyCount;   /* Count of default keys configured  */
    uint32 dot11DefKeyMIB;       /* Current default key being used for Tx */ 
    uint32 WepKeyNotFound;       /* count of number of times that Wep key not found on lookup  */
    uint32 WepDecryptFail;       /* count of number of times that Wep key decryption failed  */
} WepStatistics_t; 


typedef struct ACXStatisticsStruct 
{
	ElmHdr_T  ElmHdr;
    TxStatistics_t   tx;
    RxStatistics_t   rx;
	DMAStatistics_t  dma;
	IsrStatistics_t  isr;
    WepStatistics_t  wep;
    #ifdef KEEP_MEM_STATS
    MemStatistics_t  mem[2];           /* RX and TX stats must be copied from pools */
    #endif
} ACXStatisticsStruct; 

#endif	/* Statistics from _INFOELE_H for FW version 1.0.8 */


/*
 * Feature config
 * --------------
 */

/* Provides expandability for future features */
#define FEAT_ELP_ENABLE     0x01         /*Enable extreme low power mode*/
#define FEAT_PWR_CONS_TX    0x02         /*Pwr conservation during TX*/
#define FEAT_PWR_CONS_RX    0x04         /*Pwr conservation during RX*/
#define FEAT_4X             0x20         /* 4x throughput enhancement (WACK) */

#pragma pack(1)
typedef struct
{
	ElmHdr_T  ElmHdr;
    UINT32		Options;
} AcxElm_FeatureConfig_T;
#pragma pack()


#endif
