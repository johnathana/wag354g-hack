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
**|     Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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
 *   MODULE:  whal.h
 *   PURPOSE: whal interface
 *
 ****************************************************************************/

#ifndef _WHAL_H
#define _WHAL_H

typedef void *MGR_HANDLE;

typedef struct
{
	UINT32		time;				
	UINT8		Ctl;				
	UINT8		Rate;	       
	UINT8		Status;				
	UINT8		SNR;
	UINT8		SIR;
    UINT8       woneIndex;
    UINT8       Mic;
} RxAttr_T; 

#define MIN_NUM_OF_BDs_IN_MSDU 2

#define TXATTR_PROTECTION_NONE       0
#define TXATTR_PROTECTION_RTS_CTS    1
#define TXATTR_PROTECTION_CTS_TOSELF 2
#define TXATTR_PROTECTION_TI_TRICK   3

/*
 *  WONE Feature definitions
 */
#define MAX_WONE_NET		2
#define FIRST_WONE_NET		0
#define SECOND_WONE_NET		1
#define DEF_WONE_NET		FIRST_WONE_NET





typedef struct
{
	UINT16		Rate;
	UINT16		RateWithFallback; /* used in TNET1130 only */
	UINT8		Broadcast;
	UINT8		ShortPreamble;
	UINT8		ProtectionType; /* used in TNET1130 only */
    UINT8       Tkip; /* used in TNET1130 only */
} TxAttr_T; 

/* TODO: remove the define below and unremark BcastTxQid */

#define MAX_NUM_Q_PER_WONE  3


#if( WLAN_ADAPTOR_VERSION == WLAN_ADAPTOR_VERSION_1130)

  typedef int TxQueueId_T;
  #define NUM_TX_QUEUES (MAX_WONE_NET * MAX_NUM_Q_PER_WONE)

#else

  typedef enum 
  {
	UcastTxQid    =0 ,
	BcastTxQid    =1,
    NUM_TX_QUEUES = MAX_WONE_NET * MAX_NUM_Q_PER_WONE
  } TxQueueId_T;

#endif

typedef enum
{
	WONE_LOW_PRIOR_QUEUE  = 0,
	WONE_HIGH_PRIOR_QUEUE = 1,
	WONE_PS_BCAST_QUEUE   = 2,
 b }WoneQueues_e;


/*  EDCF QoS parameters configured to the FW per Tx queue.  */
/*  ======================================================  */
typedef struct    
{				  
    UINT8		  acID; /* Use AccessCategory_e values. This tells us which EDCFCfg is 
	                       being read or written from the NUM_ACCESS_CATEGORIES available */
    UINT8		  trafficType;  /* Use TrafficAccessType_e values */
    UINT8		  aifsn;    /* slots number interval to start tx. */
    UINT8		  cwMin;
    UINT16		  cwMax;
    UINT16		  txopLimit;    /* For future use, default - 0 (send a single MPDU per TXOP), in 32 usec units */
    UINT32		  dot11EDCATableMSDULifetime;  /* For future use, default - 512 TUs. */
    UINT16        rxTimeout;    /* For future use, default - 0 (no timeout) */
    UINT8         ackPolicy;    /* For future use, Use ACKPolicy_e values */
    UINT8         shortRetryLimit;   /* For future use, default - 7 */
    UINT8         longRetryLimit;    /* For future use, default - 4 */
    BOOL	      retryPremption;    /* For future use. */
    BOOL          txopContinuation;  /* For future use. */
}edcf_info_t;





/*
 * Rate definitions 
 */
#define RATE_1				0x0A		/* 1 megabit */
#define RATE_2				0x14		/* 2 megabit */
#define RATE_5_5			0x37		/* 5.5 CCK */
#define RATE_5_5_PBCC		0xB7		/* 5.5 PBCC */
#define RATE_11				0x6E		/* 11 CCK */
#define RATE_11_PBCC		0xEE		/* 11 PBCC */
#define RATE_22_PBCC		0xDC		/* 22 PBCC */

/*
 * New Rate definitions 
 */
#define ACX_RATE_1			0x0001
#define ACX_RATE_2			0x0002
#define ACX_RATE_5_5		0x0004
#define ACX_RATE_6			0x0008
#define ACX_RATE_9			0x0010
#define ACX_RATE_11			0x0020
#define ACX_RATE_12			0x0040
#define ACX_RATE_18			0x0080
#define ACX_RATE_22			0x0100
#define ACX_RATE_24			0x0200
#define ACX_RATE_36			0x0400
#define ACX_RATE_48			0x0800
#define ACX_RATE_54			0x1000
#define ACX_RATE_PBCC		0x4000
#define ACX_RATE_PREAMBLE	0x8000
#define ACX_RATE_MASK		0x1FFF
#define ACX_RATE_NUM		13
#define ACX_RATE_ALL_OFDM_MASK	(ACX_RATE_6 | ACX_RATE_9 | \
                             ACX_RATE_12 | ACX_RATE_18 | \
                             ACX_RATE_24 | ACX_RATE_36 | \
                             ACX_RATE_48 | ACX_RATE_54)

/* The watch dog interval is not exactly 500ms to avoid sync with the beacon period */
#define WATCH_DOG_TIMER_VAL 520

/* 
 * Callbacks 
 */
/* User init ack callback function */
typedef void (*whal_UsrInitFuncCB)(int retVal);

/* Callback for Tx status (Actual Tx Rate per Dest Mac address) */
typedef void (*TxStatus_Callback_T)(UINT8 *DestMacAddress, UINT16 ActualTxRate, UINT8 TxStatus);

/* Callback for Rx Data interrupt */
typedef void (*RxData_Callback_T)(int Val);

/* Callback for Device Error interrupt */
typedef void (*DeviceError_Callback_T)(void);

/* 
 * Initiation/Control api 
 */
extern int  whal_apiInit(whal_UsrInitFuncCB CbFunc, int retArg);
extern int  whal_apiUnload(int stage);
extern int  whal_apiConfig();
extern int  whal_apiStartBss();
extern int  whal_apiStopBss();
extern int  whal_apiAcxReset();
extern int whal_apiRxIntrEnable(int IntrOn);
extern int  wpa_restart();



/* 
 * Rx api 
 */
extern MGR_HANDLE whal_apiRxRecv(int *Len, RxAttr_T *pRxAttr);
extern void whal_apiRxClean();
extern int  whal_apiSetRxDataCB(RxData_Callback_T RxDataCB, int Threshold);

/* 
 * Tx api 
 */
extern int  whal_apiSendMsdu(TxQueueId_T TxQid, MGR_HANDLE Msdu_Handle, TxAttr_T *pTxAttr);
extern int  whal_apiTxCheckSize(TxQueueId_T TxQid, int Len);
extern int  whal_apiSetTxStatusCB(TxStatus_Callback_T TxStatusCB);
extern void whal_apiTxClean(TxQueueId_T TxQid);
extern int  whal_apiSetBssPs(int BssPsMode);
extern int  whal_apiSetTim(char BmapControl, char *PartialBmapVec, int PartialBmapLen);

/* 
 * 4X feature API 
 */
/* Callback for receive 4X wack info */
typedef void (*Recv4xWackInfo_Callback_T)(UINT8 *DestMacAddress, UINT8 Recv4xWackInfo);
/* user Callback for TX_DATA  */
typedef void (*TxDataUser_Callback_T)();

extern int  whal_apiEnableTxDataIntr();
extern int  whal_apiDisableTxDataIntr();
extern int  whal_apiSetTxDataUserCB(TxDataUser_Callback_T TxDataUserCB);
extern int  whal_apiEnable4xWackFeature();
extern int  whal_apiDisable4xWackFeature();
extern int  whal_apiSetRecv4xWackInfoCB(Recv4xWackInfo_Callback_T Recv4xWackInfoCB);
extern int  whal_apiSetSend4xWackInfo(UINT8  Send4xWackInfo);
extern int  whal_apiGetSend4xWackInfo(UINT8 *Send4xWackInfo);
extern int  whal_apiDisable4xTemplate();
extern int  whal_apiEnable4xTemplate();
extern int  whal_apiSet4xCwMin(UINT16 CwMin);
extern int  whal_apiGet4xCwMin(UINT16 *CwMin);
extern int  whal_apiSet4xPreambleCaps(int PreambleVal);
extern void whal_apiPrint4xWackParams();

/* 
 * Device Error
 */
extern int whal_apiEnableDeviceErrorIntr(void *external_DeviceError_Callback);
extern int whal_apiDisableDeviceErrorIntr(void);

/* 
 * ERP Protection
 */
extern int whal_apiSetERPLongPreambleOnly(UINT8 WoneIndex, UINT8 longPreambleOnly);
extern int whal_apiSetERPProtection(UINT8 WoneIndex, UINT8 nonERPpresent, UINT8 useProtection);
/* 
 * Params api 
 */
extern int whal_apiSetRadioChannel(int Channel);
extern int whal_apiSetFragmentThreshold(int FragSize);
extern int whal_apiSetRtsThreshold(int RtsSize);
extern int whal_apiSetShortRetryLimit(UINT8 Val);
extern int whal_apiSetLongRetryLimit(UINT8 Val);
extern int whal_apiSetMaxTxMsduLifetime(UINT32 Val);
extern int whal_apiSetPbcc		(UINT8 WoneIndex, int PbccVal);
extern int whal_apiSetSlotTime	(UINT8 WoneIndex, int SlotTimeVal);
#define WEP_ACTION_ADD	1
#define WEP_ACTION_DEL	2

extern int whal_apiSetMappingKey(int Action, char *MacAddr, int KeySize,int KeyType,int Index,int KeyId, char *Key);
extern int whal_apiSetDefaultKey(int WepAction, int KeyId,     int KeySize, char *Key);
extern int whal_apiSetDefaultKeyId(int KeyId);
extern int whal_apiSetWepExcludeUnencrypted(int Val);
extern int whal_apiSetWepOptionImplemented(int Val);
extern int whal_apiSetWepInvoke(int Val);

/* 
 * Counters 
 */
extern int whal_apiGetFcsError(UINT32 *Val);
extern int whal_apiGetFailedCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetRetryCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetMultipleRetryCount(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetAckFailCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetRtsFailCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetRtsSuccCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetTxFrameCount		(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetTxFragmentCount	(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetTxMulticastCount	(UINT8 WoneIndex, UINT32 *Val);
extern int whal_apiGetExcludeUnencryptedCount(UINT32 *Val);
extern int whal_apiGetUnencryptableCount(UINT32 *Val);
extern int whal_apiGetIcvErrorCount(UINT32 *Val);
extern int whal_apiGetFwVersion(char *FwVersionStr);

/*
 * Hal menu
 */
extern int whal_apiPrintFwVersion();
extern int whal_apiPrintTxInfo(int Qid, BOOL QprintEn);
extern int whal_apiPrintTxQueue(int Qid);
extern int whal_apiPrintTxIndicator();
extern int whal_apiPrintTxBufs();
extern int whal_apiPrintRxInfo();
extern int whal_apiPrintRxQueue(int Qid);
extern int whal_apiPrintRxBufs();
extern int whal_apiPrintIntr();
extern int whal_apiPrintMbox();
extern int whal_apiPrintAcxParams();
extern int whal_apiPrintAcxStatistics();
extern int whal_apiPrintRestartStatistics();
extern int whal_apiPrintRegs();

/*
 * Misc
 */
extern int		whal_apiRxPromiscuous(int Val);
extern int		whal_apiModeSniffer(int Val);
extern void		whal_apiPhyWrite(UINT32 PhyRegAddr, UINT32 DataVal);
extern UINT32	whal_apiPhyRead(UINT32 PhyRegAddr);
extern void		whal_apiMacWrite(UINT32 MacRegAddr, UINT32 DataVal);
extern UINT32	whal_apiMacRead(UINT32 MacRegAddr);

/*
 * Hal serial EEPROM
 */
void whal_apiDumpEEPROM(void);
void whal_apiReadEEPROM(int addr);
void whal_apiWriteEEPROM(int addr, int val);

/*
 * Hal nvram
 */
void whal_apiPrintFwVersions();
void whal_apiSelectFwNumber(int Val);
void whal_apiPrintRadioVersions();
void whal_apiSelectRadioNumber(int Val);
void whal_apiTraceEnable(int Val);
void whal_apiTracePrint(int Num, int From);
void whal_apiTracePrintRxStat( void );
void whal_apiSetNvByte(int offset, int Val);
void whal_apiSetQueueSizeType(int Val);

/*
 * ACX100 BIT
 */
int whal_apiAcxBIT_Fcc(int chID, int rate, int mode);
int whal_apiAcxBIT_Telec(int chID);
int whal_apiAcxBIT_ExitTest(void);

#endif