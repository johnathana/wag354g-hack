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
 *   MODULE:  whalParams.h
 *   PURPOSE: Holds all the whal parameters
 *		The whal parameters are seperate into the following category:
 *		DmaParams	holds Rx and Tx queue and buffers parameters
 *		IdParams	holds the AP MACs (Bssid, AP MAC address) and the SSID string
 *		WlanParams	holds information elements for beacon (SSID, Rates, Channel)
 *		GenParams	holds general parameters such as fragment threshold 
 * 
 ****************************************************************************/

#ifndef _WHAL_PARAMS_H
#define _WHAL_PARAMS_H

#include "802_11Defs.h"
#include "AcxInfoElements.h"

#define MAX_FRAGMENTATION_THRESHOLD		2312




/* 
 * General params
 * --------------
 */

typedef struct
{
	UINT16	FragmentThreshold;
	UINT16	RtsThreshold;
	UINT8	ShortRetryLimit;
	UINT8	LongRetryLimit;
	UINT16	BeaconInterval;
	UINT8	DtimInterval;
	UINT16	BeaconCapabilities;
	UINT16	ProbResCapabilities;
	UINT32	MaxTxMsduLifetime;

#if( WLAN_ADAPTOR_VERSION == WLAN_ADAPTOR_VERSION_1130)
	UINT8	Band;
#endif
	UINT8	StartRadioChannel;
	UINT8	CurrRegDomain;
    UINT8   PowerLevel;

#if( WLAN_ADAPTOR_VERSION == WLAN_ADAPTOR_VERSION_1130)
	UINT16	BasicRateSet;
#else
	UINT8	BasicRateSet;		/* bitmap 1,2,5.5,11,22 */
    UINT8   SupportedRateSet;	/* bitmap 1,2,5.5,11,22 */
#endif

	UINT8	BssType;
	UINT8	ACX100_GenTxRate;
	UINT8	ACX100_GenTxPreample;
	UINT8	RateFallback;
	UINT16	RxConfigOption;
	UINT16	RxFilterOption;
	UINT32	RxStatusHdrLen;

	int		UseMboxInterrupt;

	UINT16	CWmin;

	UINT8   enEnergyDetect;


	int		Support4x;
	int		SupportTIcapTemplate;

	UINT32	PbccDynamicEnable;
	UINT32	PbccDynamicInterval;
	UINT32	PbccDynamicIgnoreMcast;

	UINT32	ERPProtectionEnable; /* dynamic ERP protection indication 
								    (ON only for DYNAMIC ERP algorithm) */
	UINT32	ERPProtectionType;
#if 0
	UINT32	ERPProtectionInterval;
#endif	
	UINT32	ERPProtectionIgnoreMcast;
} GenParams_T;


/*
 * 802.11 Info elements for the AP
 * -------------------------------
 */
typedef struct
{
	dot11_SSID_T		WlanElm_Ssid;
	dot11_supRates_T	WlanElm_SupportedRates;
	dot11_extSupRates_T	WlanElm_ExtSupportedRates;
#ifndef NO_PBCC_PATCH
	dot11_supRates_T	WlanElm_SupportedRates_Probe;
	dot11_supRates_T	WlanElm_SupportedRates_Beacon;
#endif
	dot11_dsParams_T	WlanElm_DsParams;
	dot11_tim_T			WlanElm_Tim;
    dot11_CountryInfo_T WlanElm_CountryInformation;
	dot11_rsne_T		WlanElm_Rsne;
	int Cf_Enabled;
	dot11_cfParams_T	WlanElm_Cf;
	dot11_TIcap_T   	WlanElm_TIcapSupport;
	dot11_ERP_T			WlanElm_ERP;
	int ERP_Enabled;
} WlanElmParams_T;

/*
 * IdParams_T - MAC addresses and SSID
 * -----------------------------------
 */
#ifndef MAX_SSID_SIZE
	#define MAX_SSID_SIZE		32
#endif
#define MAC_ADDR_SIZE		6

typedef struct
{
	UINT8   BssId[MAC_ADDR_SIZE];
	UINT8   SrcMacAddr[MAC_ADDR_SIZE];
	UINT8   BcastMacAddr[MAC_ADDR_SIZE];
	UINT8	Ssid[MAX_SSID_SIZE];
} IdParams_T;

/*
 * DmaParams_T - Rx/Tx Queues and Bufs params
 * ------------------------------------------
 */

typedef struct
{
#if( WLAN_ADAPTOR_VERSION == WLAN_ADAPTOR_VERSION_1130)
	UINT16	NumStations;
	UINT8	RxBlocksRatio;
	UINT8	TxXfrBlocksRatio;
	UINT8	RxQueue_Priority;
	UINT8	RxQueue_Type;
#endif
	int		NumRxBlocks;
	int		NumTxBlocks;
	UINT32	RxBlockSize;
	UINT32	TxBlockSize;
	int		RxQueue_NumDesc;
	int		TxNumDesc[NUM_TX_QUEUES];
	int		TxQPriority[NUM_TX_QUEUES];
	UINT32	MemoryPoolStart;
} DmaParams_T;

/*
 * Wep parameters
 * --------------
 */
typedef struct
{
	char Key[WEP_KEY_LEN_MAX];
	int	 KeySize;
} WepKey_T;

typedef struct
{
	char MacAddr[6];
	int	 KeySize;
	char Key[WEP_KEY_LEN_MAX];
	int  WepOn;
} MappingKeys_T;

typedef struct
{
	WepKey_T		WepDefaultKey[4];
	UINT8			DefaultKeyId;
	UINT32			dot11WEPKeymappingLength;	
	MappingKeys_T	MappingKeys[10 /* DEF_WEP_KEY_MAPPING_LEN */];

	/*
		------- dot11PrivacyInvoked - Enable Privacy on Tx 
        "When this attribute is true, it shall indicate that the IEEE
        802.11 WEP mechanism is used for transmitting frames of type
        Data. The default value of this attribute shall be false."
	*/
	int dot11PrivacyInvoked;

	/*
		------- dot11ExcludeUnencrypted 
        "When this attribute is true, the STA shall not indicate at
        the MAC service interface received MSDUs that have the WEP
        subfield of the Frame Control field equal to zero. When this
        attribute is false, the STA may accept MSDUs that have the WEP
        subfield of the Frame Control field equal to zero. The default
        value of this attribute shall be false."
	*/

	int dot11ExcludeUnencrypted;
	/*
		------- dot11PrivacyOptionImplemented 
	    This attribute, when true, shall indicate that the IEEE
        802.11 WEP option is implemented.  The default value of
        this attribute shall be false."
	*/
	int dot11PrivacyOptionImplemented;

	int dot11WepExcludeCount;
	int dot11WEPUndecryptableCount;
	int dot11WEPICVErrorCount;
} WepParams_T;

/* 
 * Nv params
 * ---------
 */

typedef struct
{
	UINT8	Magic[2];
	UINT8	TraceEnable;
	UINT8	PlcpPatch;
	UINT8	FwNumber;
	UINT8	QueueSizesType;
	UINT8	LoadRadio;
	UINT8	RadioNumber;
	UINT8	UseIntrThreshold;
	UINT8	Line0Spare1[5];
	UINT8	TraceOut;
	UINT8	TraceFile;

	UINT8   ProbePatch;
	UINT8	ProbePatch_Rate;
	UINT8	ProbePatch_Caps;
	UINT8	ProbePatch_Spare;
	UINT8	ProbePatch_SuppRates[8];
	UINT8	Line1Spare1[4];

	UINT8	BeaconPatch;
	UINT8	BeaconPatch_Rate;
	UINT8	BeaconPatch_Caps;
	UINT8	BeaconPatch_Spare;
	UINT8	BeaconPatch_SuppRates[8];
	UINT8	Line2Spare1[4];

	UINT8	PcfPatch;
	UINT8	PcfPatch_Caps;
	UINT8	PcfPatch_Spare[2];
	UINT8	Line3Spare1[12];

	UINT8	Line4Spare1[16];
	UINT8	Line5Spare1[16];
	UINT8	Line6Spare1[16];
	UINT8	Line7Spare1[16];
} NvParams_T;

/*
 *					MAIN PARAMETERS STRUCTURE
 * ----------------------------------------------------------------
 */
typedef struct
{
	/* Rx/Tx queue parameters */
	DmaParams_T DmaParams;

	/* Id parameters */
	IdParams_T IdParams;

	/* Wlan elements parameters (for beacon and StartBss) */
	WlanElmParams_T WlanElmParams;

	/* General parameters */
	GenParams_T GenParams;

	/* Memory map info element - for debug */
	UINT32 Dummy;
	AcxElm_MemoryMap_T AcxElm_MemoryMap;

	/* Config Options (ACX100 EEPROM) - read after reset */
	AcxElm_ConfigOptions_T AcxElm_ConfigOptions;

	UINT32 Pad1;

	/* Fw version */
	AcxElm_AcxVersion_T AcxVersion;

	/* Wep area */
	WepParams_T WepParams;

	/* Nv area */
	NvParams_T NvParams;


} WhalParams_T;

extern int whal_ParamsInit();

/* DmaParams - Rx/Tx queue parameters */
extern void whal_ParamsInitDmaParams();
extern DmaParams_T *whal_ParamsGetDmaParams(UINT8 WoneIndex);

/* IdParams parameters */
extern void whal_ParamsInitIdParams();
extern UINT8 *whal_ParamsGetBssId();
extern UINT8 *whal_ParamsGetSrcMac();
extern UINT8 *whal_ParamsGetBcastMac();

/* WlanElm parameters */
extern void whal_ParamsInitWlanParams();
extern dot11_SSID_T		*whal_ParamsGetElm_Ssid();
extern dot11_supRates_T *whal_ParamsGetElm_SupportedRates(UINT8 WoneIndex);
extern dot11_CountryInfo_T *whal_ParamsGetElm_CountryInfo(UINT8 WoneIndex);
extern dot11_extSupRates_T *whal_ParamsGetElm_ExtSupportedRates(UINT8 WoneIndex);
extern dot11_ERP_T      *whal_ParamsGetElm_ERP(UINT8 WoneIndex);
extern dot11_dsParams_T	*whal_ParamsGetElm_DsParams(UINT8 WoneIndex);
extern dot11_TIcap_T *whal_ParamsGetElm_TIcapSupport();
extern dot11_tim_T		*whal_ParamsGetElm_Tim();
extern int whal_ParamsSetElm_Tim(char BmapControl, char *PartialBmapVec, int PartialBmapLen);
extern dot11_rsne_T		*whal_ParamsGetElm_Rsne();
extern dot11_cfParams_T *whal_ParamsGetElm_Cf();
extern int whal_ParamsSetElm_TIcapSupport();
extern int whal_ParamsResetElm_TIcapSupport();
extern int whal_ParamsSetElm_ERPProtection(UINT8 WoneIndex, UINT8 nonERPpresent, UINT8 useProtection);
extern int whal_ParamsSetElm_ERPLongPreambleOnly(UINT8 WoneIndex, UINT8 longPreambleOnly);

/* general AP parameters */
extern int    whal_ParamsInitGenParams();
extern GenParams_T *whal_ParamsGetGenParams(UINT8 WoneIndex);
extern UINT16 whal_ParamsGetBeaconInterval();
extern UINT8  whal_ParamsGetRadioChannel(UINT8 WoneIndex);
extern void   whal_ParamsSetRadioChannel(UINT8 WoneIndex, int Channel);
extern void   whal_ParamsSetFragmentThreshold(int FragSize);
extern void   whal_ParamsSetRtsThreshold(int RtsSize);
extern void   whal_ParamsSetShortRetryLimit(UINT8 Val);
extern void   whal_ParamsSetLongRetryLimit(UINT8 Val);

/* Memory map info element */
extern AcxElm_MemoryMap_T	*whal_ParamsGetMemMap();
extern void					 whal_ParamsPrintMemMap();

/* Config Options from the ACX100 EEPROM */
extern AcxElm_ConfigOptions_T *whal_ParamsGetConfigOptions();
extern void whal_ParamsAcxNvramPrint();

/* Acx100 version */
extern AcxElm_AcxVersion_T *whal_ParamsGetAcxVersion();
extern void whal_ParamsPrintFwVersion();

/* Wep params */
extern void whal_ParamsInitWepParams();
extern WepParams_T *whal_ParamsGetWepParams();
extern int  whal_ParamsGetWepDefaultKey(int KeyId, char *Key);
extern int  whal_ParamsGetWepDefaultKeySize(int KeyId, int *Size);
extern int  whal_ParamsGetWepDefaultKeyId(int *KeyId);
extern int  whal_ParamsGetWepKeyMappingSize(int *pVal);
extern int  whal_ParamsGetWepKeyMappingKeyVal(int KeyId, char *Key);
extern int  whal_ParamsGetWepKeyMappingKeySize(int KeyId, int *Size);
extern int  whal_ParamsGetWepKeyMappingMacAddr(int KeyId, char *MacAddr);
extern int  whal_ParamsGetWepKeyMappingWepOn(int KeyId);
extern int  whal_ParamsGetWepPrivacyOptionImplemented();
extern int  whal_ParamsGetWepPrivacyInvoked();
extern int  whal_ParamsGetWepExcludeUnencrypted();

/* Nv params */
extern void whal_ParamsInitNvParams();
extern NvParams_T *whal_ParamsGetNvParams();
extern void whal_ParamsSetNv(int Offset, UINT8 Val);

#ifndef NO_PBCC_CHECK
extern dot11_supRates_T  *whal_ParamsGetNv_ProbeRespSupportedRates();
extern dot11_supRates_T  *whal_ParamsGetNv_BeaconSupportedRates();
#endif

extern int genMacCompare(char *Mac1, char *Mac2);

#endif
