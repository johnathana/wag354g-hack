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
**|     Copyright (c) 1998,1999,2000 Texas Instruments Incorporated      |**
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

/***************************************************************************/
/*																									*/
/*		MODULE:	app_driver_api.h															*/
/*    PURPOSE:	Driver's interface to the application								*/
/*																									*/
/***************************************************************************/
#ifndef _APP_DRIVER_API_H
#define _APP_DRIVER_API_H

#include "pform_types.h"
#include "memMngr.h"
#include "802_11Defs.h"
#include "whalNVRAM.h"

typedef struct 
{
	/* wdrv tasks priority */
	UINT32 RxTaskPriority;
	UINT32 TxTaskPriority;
	UINT32 WpaTaskPriority;
	UINT32 MgmtTaskPriority;
	UINT32 MainSmTaskPriority;

	/* wdrv tasks stack size */
	UINT32 RxTaskStackSize;
	UINT32 TxTaskStackSize;
	UINT32 WpaTaskStackSize;
	UINT32 MgmtTaskStackSize;
	UINT32 MainSmTaskStackSize;

} WDRV_TASKS_CONFIG;

typedef struct 
{
	/* driver tasks configuration */
	WDRV_TASKS_CONFIG wdrvTasksConfig;
	/* Add here more drvice configuration parameters */
} WDRV_CONFIG;

#define RL_ROOT		1
#define RL_TEST		2
#define RL_TCB			3
#define RL_IDLE		4
#define RL_DB			5
#define RL_MAINSM	   6
#define RL_MGMT	   7
#define RL_HAL	      8
#define RL_TX        9
#define RL_SM	   10
#define RL_RX	   11
#define RL_RX_SNIFF	   12
#define RL_TX_SNIFF	   13
#define RL_BRIDGE	   14
#define RL_WPA			15
#define RL_HTTP	16
#define RL_4X		17
#define RL_MEM		18


#ifndef TIWLAN_NO_REPORT_LEVEL
#define REPORT_LEVEL(l)    ((1<<(l)) & wdrv_utilsReportLevel)

#else

#define REPORT_LEVEL(l) 0

#endif

#define WDRV_UTILS_GETSHORT(x) ( ((unsigned char)(*((unsigned char *)(x)))   << 8) + \
							((unsigned char)(*((unsigned char *)(x)+1))) )


#define WDRV_UTILS_GETLONG(x)  ( ((unsigned char)(*((unsigned char *)(x)))   << 24) + \
							((unsigned char)(*((unsigned char *)(x)+1)) << 16) + \
							((unsigned char)(*((unsigned char *)(x)+2)) <<  8) + \
							((unsigned char)(*((unsigned char *)(x)+3))) )




/* v2.11 */
#define WDRV_UTILS_MAX(a,b)  (((a) > (b)) ? (a) : (b) )
#define WDRV_UTILS_MIN(a,b)  (((a) < (b)) ? (a) : (b))

typedef unsigned long   WDRV_UTILS_REPORT_LEVEL;

typedef enum {
   PRI_EMERGENCY = 1,
   PRI_ALERT,
   PRI_CRITICAL,
   PRI_ERROR,
   PRI_WARNING,
   PRI_NOTICE,
   PRI_INFORMATION,
   PRI_DEBUG
} Priority_T;

/* Alarm callback - called by driver with relevant info     */
typedef  void (*wdrv_reportFunc)(char *buffer);

#ifndef _REPORT_LEVEL_C_
extern WDRV_UTILS_REPORT_LEVEL wdrv_utilsReportLevel;
#else
WDRV_UTILS_REPORT_LEVEL  wdrv_utilsReportLevel = 0;
#endif

/* User init ack callback function */
typedef void (*wdrv_UsrInitFuncCB)(STATUS status);
/* MSDU annuncment callback function */
typedef void (*wdrv_msduRcvdCBFunc)(mem_MSDU_T *pMsdu);

void wdrv_init(wdrv_reportFunc reportCB, wdrv_UsrInitFuncCB initCB,
               wdrv_msduRcvdCBFunc msduRcvdCBFunc, WDRV_CONFIG *wdrvConfig); 
void wdrv_start(void);
void wdrv_utilsSetReportLevel(WDRV_UTILS_REPORT_LEVEL level);
void wdrv_utilsClearReportLevel(WDRV_UTILS_REPORT_LEVEL level);
WDRV_UTILS_REPORT_LEVEL wdrv_utilsGetReportValue();
void wdrv_utilsSetReportValue(WDRV_UTILS_REPORT_LEVEL value);


/*************************************
	Network Interface functions
 STATUS  wdrv_send( mem_MSDU_T *pMsdu );
**************************************/
STATUS wdrv_bssBridgePktFromEthrToWlan(mem_MSDU_T *pMsdu);
#define wdrv_send wdrv_bssBridgePktFromEthrToWlan


/*************************************
	Mgmt functions
**************************************/
void wdrv_dbPrintNumOfStations(void);
void wdrv_dbPrintStaTable(void);
void wdrv_dbPrintRatesTable(void);
void wdrv_dbPrintHashTable(void);
void wdrv_mgmtDeAuthMng(char *mac, UINT16 reason);
void wdrv_mgmtDisAssocMng(char *mac, UINT16 reason);
void wdrv_mgmtSetLog(BOOL mode);
BOOL wdrv_mgmtLogEnable(void);
void wdrv_mgmtSetAging(BOOL mode);
BOOL wdrv_mgmtAgingEnable(void);
void wdrv_mgmtPrintCounters(void);
void wdrv_mgmtGetState(UINT16 index, UINT32 *state);
void wdrv_dbPrintFourXTable(void);

/*************************************
	Mgmt -test functions
**************************************/
void wdrv_mgmtTstNull(UINT16 fc, UINT32 SA, UINT32 DA, UINT32 BSSID);
void wdrv_mgmtTstAuth1(UINT32 SA, UINT16 algoNum);
void wdrv_mgmtTstAuth3(UINT32 SA, UINT16 algoNum, UINT32 challenge);
void wdrv_mgmtTstAssoc(UINT32 SA, BOOL pbcc, BOOL privacy, BOOL fourX);
void wdrv_mgmtTstReassoc(UINT32 SA, BOOL pbcc, BOOL privacy, BOOL fourX);
void wdrv_mgmtTstDeAuth(UINT32 SA, UINT16 reason);
void wdrv_mgmtTstDisAssoc(UINT32 SA, UINT16 reason);
void wdrv_mgmtTstClass3(UINT32 SA, UINT16 index);
void wdrv_mgmtPrintChallenge(UINT16 index);
void wdrv_mgmtReset(void);
/*void wdrv_mgmtPrintFrame(mem_MSDU_T *msduPtr);*/

/*************************************
	WPA functions
**************************************/
void wdrv_wpaPrintStationRsnIe(void);

void wdrv_wpaTstVector(void);
void wdrv_wpaTstAssocStation(UINT8 SA, BOOL rsn, UINT8 rsnIeLen, UINT8 rsnIeValue);
void wdrv_wpaTstReAssocStation(UINT8 SA, BOOL rsn, UINT8 rsnIeLen, UINT8 rsnIeValue);

void wdrv_dbSetStationTxMicKey(UINT16 index, UINT8 *keyValue);	
void wdrv_dbSetStationRxMicKey(UINT16 index, UINT8 *keyValue);	
void wdrv_dbGetStationSuiteType(UINT16 index, UINT8 *cipher);	
void wdrv_dbSetStationSuiteType(UINT16 index, UINT8 cipher);	

BOOL wdrv_wpaGetStationSecAttack(UINT16 index);	

void wdrv_dbSetBroadcastSuiteType(UINT8 cipher);	
void wdrv_dbGetBroadcastSuiteType(UINT8 *cipher);	
void wdrv_dbSetBroadcastTkipKey(UINT8 *keyValue,UINT8 index);
void wdrv_dbGetBroadcastTkipKey(UINT8 *keyValue,UINT8 *index);
void wdrv_dbSetBroadcastMicKey(UINT8 *keyValue);	
void wdrv_dbGetBroadcastMicKey(UINT8 *keyValue);	
void wdrv_dbSetBroadcastWepKey(UINT8 index,UINT16 keySize,UINT8 *keyValue);
void wdrv_dbGetBroadcastWepKey(UINT8 index,UINT16 *keySize,UINT8 *keyValue);
void wdrv_dbSetWepDefaultKeyId(UINT8 index);
void wdrv_dbGetWepDefaultKeyId(UINT8 *index);



void wdrv_dbSetWpaMode(UINT32 mode);	
void wdrv_dbGetWpaMode(UINT32 *mode);	

void wdrv_dbSetPassPhraseMode(UINT32 mode);	
void wdrv_dbGetPassPhraseMode(UINT32 *mode);	

void wdrv_dbSetPskHex(UINT8 *value);	
void wdrv_dbGetPskHex(UINT8 *value);	

void wdrv_dbSetDefaultCipher(UINT32 value);	
void wdrv_dbGetDefaultCipher(UINT32 *value);	

void wdrv_dbSetGTKInterval(UINT32 value);	
void wdrv_dbGetGTKInterval(UINT32 *value);	

void wdrv_dbSetPskPwd(char *value, UINT8 len);
void wdrv_dbGetPskPwd(char *value);

void wdrv_dbSetRadiusSecret(char *value, UINT8 len);
void wdrv_dbGetRadiusSecret(char *value);

void wdrv_dbSetRadiusPort(UINT32 value);	
void wdrv_dbGetRadiusPort(UINT32 *value);	

void wdrv_dbSetRadiusIpAddr(UINT8 addr1, UINT8 addr2, UINT8 addr3, UINT8 addr4);	
void wdrv_dbGetRadiusIpAddr(UINT8 *addr1, UINT8 *addr2, UINT8 *addr3, UINT8 *addr4);	

void wdrv_dbSetDefaultGateway(UINT8 addr1, UINT8 addr2, UINT8 addr3, UINT8 addr4);	
void wdrv_dbGetDefaultGateway(UINT8 *addr1, UINT8 *addr2, UINT8 *addr3, UINT8 *addr4); 

BOOL wdrv_dbIsPrivacyEnabled();

void wdrv_wpaSetMicFailureReportInRx(BOOL value);
BOOL wdrv_wpaGetMicFailureReportInRx(void);

void wdrv_wpaSetMicFailureReportFromStation(BOOL value);
BOOL wdrv_wpaGetMicFailureReportFromStation(void);

void wdrv_dbGenerateTxMicFailure(char *mac);	


/*************************************
	Database functions
**************************************/

#define MAX_STATIONS		64	/* number of stations supported*/

/* state machine states */
typedef enum {
	STATE_IDLE			= 0,
	STATE_AUTH_KEY		= 1,
	STATE_AUTHENTICATED = 2,
	STATE_ASSOCIATED	= 3,
	STATE_AUTHORIZED	= 4,
	NUM_STATES			= 5,
} states;

extern void   wdrv_dbRemoveStaIndex(UINT16 idx);
extern STATUS wdrv_dbFindStaIndex(char *mac, UINT16 *idx);
extern STATUS wdrv_dbFindAndInsertStaIndex(char *mac, UINT8 woneIndex, UINT16 *idx);
extern BOOL   wdrv_dbIsTableIndexExists(UINT16 index);
extern void   wdrv_dbGetStaMacAddress(UINT16 index, char *macAddressPtr);
extern void   wdrv_dbGetActiveRate(UINT16 index, UINT8 *activeRate);
extern void   wdrv_dbGetStationCapabilities(UINT16 index, UINT16 *capabilities);
extern STATUS wdrv_dbSetStaSsidByIndex(UINT16 index, char *ssid, int SsidLen);
extern void   wdrv_dbGetStaSsidByIndex(UINT16 index, char *ssid, int *SsidLen);

/* Function to set/get global data */
void wdrv_dbGetBssid(char *macAddressPtr);
void wdrv_dbSetSsid(char *value, UINT8 len);
void wdrv_dbGetSsid(char *value);

/* Hidden SSID */
void wdrv_dbGetHiddenSsidStatus(UINT16 *value);
void wdrv_dbSetHiddenSsidStatus(UINT16 status);

/* Multiple SSID */
void wdrv_dbGetElementFromMultipleSsidTable(char *value, UINT16 index);
void wdrv_dbGetElementFromMultipleSsidActualTable(char *value, UINT16 index);
void wdrv_dbSetElementToMultipleSsidTable(char *value, UINT8 len);
void wdrv_dbRemoveElementFromMultipleSsidTable(UINT8 index);
void wdrv_dbGetMultipleSsidStatus(UINT16 *value);
void wdrv_dbSetMultipleSsidStatus(UINT16 status);

/* 802.11 d & h */
void   wdrv_dbGetdot11MultiDomainCapabilityStatus(UINT8 *value);
void   wdrv_dbSetdot11MultiDomainCapabilityStatus(UINT8 status);
void   wdrv_dbGetdot11SpectrumManagementRequiredStatus(UINT8 *value);
void   wdrv_dbSetdot11SpectrumManagementRequiredStatus(UINT8 status);
void   wdrv_dbGetLegacyStatusEnableStatus(UINT8 *value);
void   wdrv_dbSetLegacyStatusEnableStatus(UINT8 status);
void   wdrv_dbGetdot11CurrentRegDomain(UINT8 band, UINT8 *pRegDomain);
STATUS wdrv_dbSetdot11CurrentRegDomain(UINT8 regDomain);
void   wdrv_dbGetdot11LocalPowerConstraint(UINT8 *value);
void   wdrv_dbSetdot11LocalPowerConstraint(UINT8 newValue);
void   wdrv_dbGetMaxTxPowerLevel(UINT8 *pMaxTxPowerLevel);
void   wdrv_dbSetMaxTxPowerLevel(UINT8 maxTxPowerLevel);
void   wdrv_dbGetTxPowerLevels(UINT8 band, UINT32 level, INT8 *pPowerLevel);
STATUS wdrv_dbSetTxPowerLevels(UINT8 band,
                               INT8 powerLevel1,
                               INT8 powerLevel2,
                               INT8 powerLevel3,
                               INT8 powerLevel4,
                               INT8 powerLevel5);
void   wdrv_dbGetdot11CountryString(char* pCountryString);
STATUS wdrv_dbSetdot11CountryString(char* pCountryString);
void   wdrv_dbGetPrivateRegulatoryDomain(UINT8 *pRegDomain);
void   wdrv_dbSetPrivateRegulatoryDomain(UINT8 regDomain);
void   wdrv_dbGetNumOfPrivateRanges(wlan_bandType bandType, UINT8 *pNumOfRanges);
STATUS wdrv_dbSetNumOfPrivateRanges(wlan_bandType bandType, UINT8 numOfRanges);
STATUS wdrv_dbGetPrivateRangeElement(wlan_bandType bandType,
                                     UINT8 RangeIndex,
                                     tChannelRangeElementStruct *pRangeElementStruct);
STATUS wdrv_dbGetRangeElement(wlan_bandType bandType,
                              IntegerRegulatoryDomains_e RegDomain, 
                              UINT8 RangeIndex,
                              tChannelRangeElementStruct *pRangeElementStruct);
UINT32 wdrv_dbGetNumberOfDomainElements(wlan_bandType bandType,
                                        IntegerRegulatoryDomains_e RegDomain);

STATUS wdrv_dbGetRegDomainInformation(wlan_bandType bandType, 
                                      UINT8 *currentRegDomain, 
                                      UINT8 *numOfRanges,
                                      UINT8 *isPrivateDomain);
BOOL   wdrv_dbIsStdRegDomain(UINT8 regDomain2Check);

/* This routine fills the channels information in the given buffer with the given length    */
/* It returns ERROR if the buffer is too short (e.g. channel information was enlarged       */
/* In this case, the buffer should be enlarged hardcoded.                                   */
/* The information is organized according to the 802.11d SPEC requirements. The length      */
/* returns the actual buffer size that was filled with channel information.                 */
/* The channel information has a triplet structure, as follow:                              */
/* 1. 1st channel number                                                                    */
/* 2. # of channels                                                                         */
/* 3. max transmit Power Level                                                              */
/* The information is taken according to the current band & current regulatory domain       */
STATUS wdrv_dbFillChannelsInformation(wlan_bandType bandType, UINT8 *pBuffer, UINT32 *length);

/* This routine tests if the channel is allowed according to the band and the current regulatory domain */
BOOL IsChannelAllowed(UINT8 woneIndex, UINT8 channel);

/* Set the 802.11b rates - used in Legacy Supported Rates IE */
void wdrv_dbSetStationConfigOperationalRateSet(UINT8 woneIndex, char *value, UINT8 len);
/* Set the 802.11g additional rates - used in Legacy Supported Rates IE */
void wdrv_dbSetStationConfigOperationalAdditionalRateSet(UINT8 woneIndex, char *value, UINT8 len);
/* Get the 802.11b rates + 802.11g rates used in Legacy Supported Rates IE */
void wdrv_dbGetStationConfigOperationalRateSet(UINT8 woneIndex, char *value, UINT8 *len);	

/* 802.11g OFDM rates  - used in Extended Supported Rates IE */
void wdrv_dbSetStationConfigOperationalOFDMRateSet(UINT8 woneIndex, char *value, UINT8 len);
void wdrv_dbGetStationConfigOperationalOFDMRateSet(UINT8 woneIndex, char *value, UINT8 *len);
/* All rates */
void wdrv_dbGetStationConfigOperationalAllRateSet(UINT8 woneIndex, char *value, UINT8 *len);

void wdrv_dbGetApCapabilities(UINT8 woneIndex, UINT16 *cap);
void wdrv_dbGetStationConfigBeaconPeriod(UINT16 *value);
void wdrv_dbGetStationConfigDtimPeriod(UINT8 *value);
void wdrv_dbSetStationConfigBeaconPeriod(UINT16 value);
void wdrv_dbSetStationConfigDtimPeriod(UINT8 value);
void wdrv_dbGetDefaultActiveRate(UINT8 woneIndex ,UINT8 *defaultRate);
void wdrv_dbGetDefaultActiveRateAcx(UINT8 woneIndex ,UINT16 *defaultRate, UINT16 *defaultRateWithFallback);
void wdrv_dbGetMaxActiveRate(UINT8 *activeRate);
void wdrv_dbSetAgile(BOOL value);
void wdrv_dbSetPreambleMode(UINT8 woneIndex ,UINT8 value);
void wdrv_dbGetPreambleMode(UINT8 woneIndex ,UINT8 *value);
void wdrv_dbSetRadioCalibrateOption(UINT32 value);
void wdrv_dbGetRadioCalibrateOption(UINT32 *value);
void wdrv_dbSetRadioCalibrateInterval(UINT16 value);
void wdrv_dbGetRadioCalibrateInterval(UINT16 *value);
void wdrv_dbSetPBCCAlgorithmMode(UINT8 woneIndex ,UINT8 value);
void wdrv_dbGetPBCCAlgorithmMode(UINT8 woneIndex ,UINT8 *value);
void wdrv_dbSetRateFallback(UINT8 value);
void wdrv_dbGetRateFallback(UINT8 *value);
void wdrv_dbSetRateAdaptation(UINT8 woneIndex, UINT8 value);
void wdrv_dbGetRateAdaptation(UINT8 woneIndex, UINT8 *value);
void wdrv_dbSetSlotTimeMode(UINT8 woneIndex ,UINT8 value);
void wdrv_dbGetSlotTimeMode(UINT8 woneIndex ,UINT8 *value);
void wdrv_dbSetERPProtectionMode(UINT8 woneIndex ,UINT8 value);
void wdrv_dbGetERPProtectionMode(UINT8 woneIndex ,UINT8 *value);
void wdrv_dbSetERPProtectionType(UINT8 woneIndex ,UINT8 value);
void wdrv_dbGetERPProtectionType(UINT8 woneIndex ,UINT8 *value);
void wdrv_dbSetDot11Mode(UINT8 woneIndex, UINT8 value);
void wdrv_dbGetDot11Mode(UINT8 woneIndex, UINT8 *value);
void wdrv_dbGetCwMinMode(UINT8 woneIndex, UINT8 *value);
void wdrv_dbSetCwMinMode(UINT8 woneIndex, UINT8 value);
void wdrv_dbGetCwMinWinSize(UINT16 *value);
void wdrv_dbSetCwMinWinSize(UINT16 value);
void wdrv_dbSetRespRateMode(UINT8 value);
void wdrv_dbGetRespRateMode(UINT8 *value);
void wdrv_dbSetPrivacyBitFlag(BOOL value);	
void wdrv_dbGetPrivacyBitFlag(BOOL *value);	
void wdrv_dbGet4xEnable(BOOL *value);
void wdrv_dbSet4xEnable(BOOL value);
void wdrv_dbGetConcatEnable(BOOL *value);
void wdrv_dbSetConcatEnable(BOOL value);
void wdrv_dbGetAckEmulEnable(BOOL *value);
void wdrv_dbSetAckEmulEnable(BOOL value);

/***************************
	Station Config table
****************************/
void wdrv_dbGetStationConfigAuthRspTimeout(UINT32 *value);	
void wdrv_dbGetStationConfigPrivacyOptionImplemented(BOOL *value);	
void wdrv_dbGetStationConfigDisassociateReason(UINT16 *value);
void wdrv_dbGetStationConfigDisassociateStation(char *value);
void wdrv_dbGetStationConfigDeauthenticateReason(UINT16 *value);
void wdrv_dbGetStationConfigDeauthenticateStation(char *value);
void wdrv_dbGetStationConfigDeauthenticateFailStatus(UINT16 *value);
void wdrv_dbGetStationConfigDeauthenticateFailStation(char *value);

void wdrv_dbSetStationConfigAuthRspTimeout(UINT32 value);	
void wdrv_dbSetStationConfigPrivacyOptionImplemented(BOOL value);	

/*************************************
	Authentication Algorithms table
**************************************/
void wdrv_dbGetAuthAlgo(UINT32 index, UINT16 *value);	
void wdrv_dbGetAuthAlgoEnable(UINT32 index, BOOL *value);	
void wdrv_dbSetAuthAlgo(UINT32 index, UINT16 value);	
void wdrv_dbSetAuthAlgoEnable(UINT32 index, BOOL value);	

/*************************************
	WEP Default Keys table
**************************************/
void wdrv_dbGetWEPDefaultKeys(UINT16 index, UINT8 *len, UINT8 *value);

void wdrv_dbSetWEPDefaultKeys(UINT16 index, UINT8 len, UINT8 *value);

/*************************************
	WEP Key Mapping table
**************************************/
void wdrv_dbGetWEPKeyMappingAddress(UINT16 index, UINT8 *value);	
void wdrv_dbGetWEPKeyMappingWEPOn(UINT16 index, BOOL *value);	
void wdrv_dbGetWEPKeyMappingValue(UINT16 index, UINT8 *len, UINT8 *value);	

void wdrv_dbSetWEPKeyMappingAddress(UINT16 index, UINT8 *value);	
void wdrv_dbSetWEPKeyMappingWEPOn(UINT16 index, BOOL value);	
void wdrv_dbSetWEPKeyMappingValue(UINT16 index, UINT8 len, UINT8 *value);	

/*************************************
	Privacy table
**************************************/
void wdrv_dbGetPrivacyInvoked(BOOL *value);	
void wdrv_dbGetPrivacyWEPDefaultKeyId(UINT8 *value);	
void wdrv_dbGetPrivacyWEPKeyMappingLength(UINT32 *value);	
void wdrv_dbGetPrivacyExcludeUnencrypted(BOOL *value);	
void wdrv_dbGetPrivacyWEPICVErrorCount(UINT32 *value);	
void wdrv_dbGetPrivacyWEPExcludedCount(UINT32 *value);	
void wdrv_dbGetPrivacyTKIPMicFailure(UINT32 *value);	

void wdrv_dbSetPrivacyInvoked(BOOL value);	
void wdrv_dbSetPrivacyWEPDefaultKeyId(UINT8 value);	
void wdrv_dbSetPrivacyWEPKeyMappingLength(UINT32 value);	
void wdrv_dbSetPrivacyExcludeUnencrypted(BOOL value);	

/*************************************
	Counters table
**************************************/
void wdrv_dbGetCountersTransmittedFragmentCount(UINT32 *value);	
void wdrv_dbGetCountersMulticastTransmittedFrameCount(UINT32 *value);	
void wdrv_dbGetCountersFailedCount(UINT32 *value);	
void wdrv_dbGetCountersRetryCount(UINT32 *value);	
void wdrv_dbGetCountersMultipleRetryCount(UINT32 *value);	
void wdrv_dbGetCountersFrameDuplicateCount(UINT32 *value);	
void wdrv_dbGetCountersRTSSuccessCount(UINT32 *value);	
void wdrv_dbGetCountersRTSFailureCount(UINT32 *value);	
void wdrv_dbGetCountersACKFailureCount(UINT32 *value);	
void wdrv_dbGetCountersReceivedFragmentCount(UINT32 *value);	
void wdrv_dbGetCountersMulticastReceivedFrameCount(UINT32 *value);	
void wdrv_dbGetCountersFCSErrorCount(UINT32 *value);	
void wdrv_dbGetCountersTransmittedFrameCount(UINT32 *value);	
void wdrv_dbGetCountersWEPUndecryptableCount(UINT32 *value);	

/*************************************
	MultiDomainCapability table
**************************************/
void wdrv_dbGetChannel(UINT8 woneIndex ,UINT8 *value);	

void wdrv_dbSetChannel(UINT8 woneIndex ,UINT8 value);	

/*************************************
	Operation table
**************************************/
void wdrv_dbGetOperationMacAddress(char *value);	
void wdrv_dbGetOperationRTSThreshold(UINT16 *value);	
void wdrv_dbGetOperationShortRetryLimit(UINT8 *value);	
void wdrv_dbGetOperationLongRetryLimit(UINT8 *value);	
void wdrv_dbGetOperationFragmentationThreshold(UINT16 *value);	
void wdrv_dbGetOperationMaxTransmitMSDULifetime(UINT32 *value);	
void wdrv_dbGetOperationMaxReceiveLifetime(UINT32 *value);	
void wdrv_dbGetOperationManufacturerId(UINT8 *value);	
void wdrv_dbGetOperationProductId(UINT8 *value);	

void wdrv_dbSetOperationMacAddress(char *value);	
void wdrv_dbSetOperationRTSThreshold(UINT16 value);	
void wdrv_dbSetOperationShortRetryLimit(UINT8 value);	
void wdrv_dbSetOperationLongRetryLimit(UINT8 value);	
void wdrv_dbSetOperationFragmentationThreshold(UINT16 value);	
void wdrv_dbSetOperationMaxTransmitMSDULifetime(UINT32 value);	
void wdrv_dbSetOperationMaxReceiveLifetime(UINT32 value);	

/* Show submenu functions */
void wdrv_powerSavePrintDb(void);
void wdrv_bssBridgeShowStatistics( void );
void wdrv_bssBridgeGetLanSendUnicast(UINT32 *LanSendUnicast);
void wdrv_bssBridgeGetLanSendMulticast(UINT32 *LanSendMulticast);
void wdrv_bssBridgeGetLanRecvUnicast(UINT32 *LanRecvUnicast);
void wdrv_bssBridgeGetLanRecvMulticast(UINT32 *LanRecvMulticast);
void wdrv_txPrintRateAdaptation( void );
extern void wdrv_txPrintStatistics(void);



/* PBCC algorithm section */
void	wdrv_dbGetNumOfStationNotPBCCCapable(UINT16 *num);
void	wdrv_dbGetCurrentPBCCSupportMode(BOOL *support);

/* Preamble Time algorithm section */
void	wdrv_dbGetNumOfStationNotShortPreambleCapable(UINT16 *num);
void	wdrv_dbGetCurrentShortPreambleSupport(BOOL *support);

/* Short Slot Time algorithm section */
void	wdrv_dbGetNumOfStationNotSlotTimeCapable(UINT16 *num);
void	wdrv_dbGetCurrentSlotTimeSupport(BOOL *support);

/* ERP Protection algorithm section */
void 	wdrv_dbGetStationsNumForERP(UINT16 *nonERPStaNum, UINT16 *ERPStaNum);
void	wdrv_dbGetCurrentERPProtectionMode(BOOL *support);


/***************************
	RX Module 
****************************/
void wdrv_rxGetRxMsduReadyCount(UINT32* count);


/************************************************
      Access List
************************************************/

typedef int (*wdrv_accessCallback_t)(char * mac_addr, int mac_addr_len);
int wdrv_accessSetAuthCallback(wdrv_accessCallback_t);
int wdrv_accessSetAssocCallback(wdrv_accessCallback_t);


/***************************
	4x Functions 
****************************/
void wdrv_4xInit();
void wdrv_4xSetStatus();
void wdrv_txSetMaxConcatSize( UINT32 size );
int wdrv_txGetMaxConcatSize(void);
void wdrv_txSetConcatDebug(BOOL value);

 
/*************************************
	WPA functions
**************************************/

void setBcastState(BOOL authorized);
void wdrv_mgmtMacAuthorize(char *mac);
void wdrv_mgmtMacDeAuthorize(char *mac);
BOOL wdrv_mgmtIsStationAuthorized(UINT16 index);
BOOL wdrv_mgmtIsBroadcastAuthorized(void);



/* NVRAM */

typedef int (*WLAN_NVRAM_Read_Callback_T)(unsigned char *pbDest, unsigned short wLen);
typedef int (*WLAN_NVRAM_Write_Callback_T)(unsigned char *pbSource,unsigned  wLen);
int  whal_nvram_SetReadCB (WLAN_NVRAM_Read_Callback_T externalNVRAM_Read);
int  whal_nvram_SetWriteCB(WLAN_NVRAM_Write_Callback_T externalNVRAM_Write);

void 	wdrv_dbRestoreNvramDefaults(void);
BOOL	wdrv_dbIsNvramIdenticalToDatabase(void);
int 	whal_nvram_setDefaults(UINT8 woneIndex);


/*************************************/

#endif
