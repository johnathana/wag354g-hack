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
/**************************************************************************/
/*                                                                        */
/*   MODULE:    whalNVRAM.h                                               */
/*   PURPOSE:   Wireless HAL NVRAM header file                            */
/*                                                                        */
/*   This file contains definitions of wireless NVRAM structure and       */
/*   access routines.                                                     */
/**************************************************************************/
#ifndef _WHAL_NVRAM_H_
#define _WHAL_NVRAM_H_

#include "pform_types.h"
#include "whalDefs.h"
#include "whal.h"
#include <mgmtAccess.h>

#ifdef __cplusplus
extern "C" {
#endif
   
#define WLAN_SER_NUM_LEN                  256
#define WLAN_OPER_RATE_SET_LEN            126
#define WLAN_FILE_NAME_LEN                68
#define WLAN_MAC_ADDR_LEN                 6
#define WLAN_PROD_PASSWORD_LEN            20
#define WLAN_FILES_NUM                    2
#define DOT11_WEP_KEY_MAPPING_LEN         10
#define DOT11_MANUFACTURER_OUI_LENGTH     3
#define WHAL_SSID_STRING_LEN              32
#define NUM_MULTIPLE_SSID                 16
#define MAX_AUTH_ALGORITHM_SUPPORTED      2
#define MIB_OPEN_SYSTEM                   1
#define MIB_SHARED_KEY                    2
#define WEP_KEY_LEN_MAX                   29
#define CCK_PBCC_RATES_MAX                5
#define OFDM_RATES_MAX                    8
#define PROD_ID_LEN                       129
#define WHAL_AREA_SIZE                    128
#define DOT11_MAX_PSK_PWD_LEN				63
#define DOT11_MAX_RADIUS_SECRET_LEN			63
#define COUNTRY_STRING_LENGTH             3
#define NUM_OF_TX_POWER_LEVELS            5
#define MAX_NUM_OF_RANGES_A               8
#define MAX_NUM_OF_RANGES_B               4
   
#define NVRAM_INIT_FAILED                 (-2)
#define SEM_CREATE_FAILED                 (-3)
#define NVRAM_WRITE_FAILED                (-4)
#define NVRAM_READ_FAILED                 (-5)
#define CALLBACK_NOT_DEFINED              (-6)
#define USER_INIT_NOT_PERFORMED           (-7)
   
typedef enum {
	MGMT_ACCESS_BLACK_LIST, 
	MGMT_ACCESS_WHITE_LIST 
} mgmt_access_list_type_t;

typedef enum {
    WLAN_BAND_B,
    WLAN_BAND_A
}wlan_bandType;

#pragma pack(1)
   typedef struct
   {
      UINT16 authAlgo;
      BOOL enable;
   }tAUTH_ALGO_ENTRY;
#pragma pack()

#pragma pack(1)
   typedef struct
   {
      UINT8 keyLen;
      UINT8	keyValue[WEP_KEY_LEN_MAX];
   }tWEP_KEY_TYPE;
#pragma pack()
   
#pragma pack(1)
   typedef struct
   {
      UINT8 pbMacAddress[6];
      BOOL  wMappingWEPOn;
      tWEP_KEY_TYPE   MappingValue;
      UINT8 bAlignment;
   }tWEP_KEY_MAPPING_ENTRY;
#pragma pack()

#pragma pack(1)
   typedef struct
   {
      UINT8         startCh;            /* start channel */
      UINT8         stopCh;             /* stop channel  */
      UINT8	        stepOpt;            /* Default step value "a"=4, "b"=1 */
      INT8          powerLimit;         /* Power limit in dBm (default = 200 dBm) */
   }tChannelRangeElementStruct;
#pragma pack()


/*************************
 * TestApp support       *
 *************************/
/*  RadioScope baudRate/Communication options */
#define TA_RADIOSCOPE_COMOPT_ETH           ((UINT32)('E'))    /* future support */
#define TA_RADIOSCOPE_COMOPT_USB           ((UINT32)('U'))    /* future support */
#define TA_RADIOSCOPE_COMOPT_CBR9600       ((UINT32)(9600))
#define TA_RADIOSCOPE_COMOPT_CBR19200      ((UINT32)(19200))
#define TA_RADIOSCOPE_COMOPT_CBR38400      ((UINT32)(38400))
#define TA_RADIOSCOPE_COMOPT_CBR57600      ((UINT32)(57600))  /* default */
#define TA_RADIOSCOPE_COMOPT_CBR115200     ((UINT32)(115200)) /* future use */

/* TestApp Startup options (consecutive numbers) */
#define TA_STARTUP_OPT_NORMAL        ((UINT8)(0))  /* Normal startup (default) */
#define TA_STARTUP_OPT_ACXONLY       ((UINT8)(1))  /* Initializing ACX HW */
#define TA_STARTUP_OPT_RADIOSCOPE    ((UINT8)(2))  /* Initializing ACX HW, if OK starting RadioScope */

#define DEF_WONE_0_USAGE_TIME    20 /* Time Unit */
#define DEF_WONE_1_USAGE_TIME    20 /* Time Unit */


#define DEFAULT_BAND	RADIO_BAND_2_4GHZ

#pragma pack(1)
   typedef struct
   {
       SINT8   dot11CckRate [CCK_PBCC_RATES_MAX];
       UINT8   dot11CckRateLength;
   }tDOT11_CCK_RATE;
#pragma pack()


#pragma pack(1)
   typedef struct
   {
       SINT8   dot11OfdmRate [OFDM_RATES_MAX];
       UINT8   dot11OfdmRateLength;
   }tDOT11_OFDM_RATE;
#pragma pack()

#pragma pack(1)
typedef struct
{
/*===================================
Wone Capability Table
===================================*/
/* 802.11b rates used in Legacy Supported Rates IE */
tDOT11_CCK_RATE           dot11OperationalRateSet ;
/* 802.11g additional rates used in Legacy Supported Rates IE */
tDOT11_OFDM_RATE          dot11OperationalAdditionalRateSet;
/* 802.11g OFDM rates used in Extended Supported Rates IE */
tDOT11_OFDM_RATE          dot11OperationalOFDMRateSet;
UINT8                     dot11channel;
UINT8                     dot11Band;    /* new field */
UINT8                     dot11PBCCAlgorithmMode;
UINT8                     RateAdaptation;
UINT8                     dot11PreambleMode;    
UINT8                     dot11SlotTimeMode;
UINT8                     dot11ERPProtectionMode;
UINT8                     dot11ERPProtectionType;
UINT8                     dot11ERPProtectionTI;
UINT8                     dot11Mode;
UINT8                     Lastdot11Mode;
UINT8                     CwMinMode;
UINT8                     usageTime;
}tWONE_NVRAM_STRUCT;
#pragma pack()

	/*================
	  vHCF Parameters 
	==================*/

#define  MAX_VHCF_CLASSIFIERS  2

typedef enum
{
	VHCF_PROT_NONE = 0,
	VHCF_PROT_TCP  = 1,
	VHCF_PROT_UDP  = 2
}VhcfProtocol_e;

#pragma pack(1)

typedef struct
{
	VhcfProtocol_e	Protocol;
	UINT16	DestPort;
	UINT32	DestIp;
}tVHCF_CASSIFIER;

typedef struct
{
  BOOL		vhcfEnable;
  tVHCF_CASSIFIER  vhcfClassifier[MAX_VHCF_CLASSIFIERS];
}tVHCF_PARAMS;

#pragma pack()



#pragma pack(1)
   typedef struct
   {
   /*======================================
   System usage area 
   Don't change ofsets of this area!!!
      ======================================*/
      UINT8   dot11MacAddress[6];
      SINT16  SwOperStatus;                           /* SW upgrade operat. status*/
      SINT8   SerialNumber[WLAN_SER_NUM_LEN];         /* Serial Number string     */
      UINT32  SwAdminStatus;                          /* SW upgrade admin status  */
      UINT32  SwServerAddr;                           /* SW upgrade server address*/
      SINT8   SwServerFilename[WLAN_FILE_NAME_LEN];   /* SW upgrade file name     */
      /* SW images file name      */
      SINT8   SwFilename[WLAN_FILES_NUM][WLAN_FILE_NAME_LEN];
      UINT32  SwImageOffset[WLAN_FILES_NUM];          /* SW flash images offset   */
      UINT32  SwImageLength[WLAN_FILES_NUM];          /* SW flash images length   */
      UINT32  SwImageCrc[WLAN_FILES_NUM];             /* SW flash images CRC      */
      UINT8   HostMacAddress[WLAN_MAC_ADDR_LEN];      /* Host MAC address         */
      UINT8   pb_password[WLAN_PROD_PASSWORD_LEN];    /* Production password      */
      UINT32  ReportLevel;
      
      /*======================================
      Station Configuration parameters 
      ======================================*/
      SINT8   dot11SsID[WHAL_SSID_STRING_LEN];
      UINT8   dot11SsIDLength;
      UINT32  dot11AuthenticationResponceTimeOut;
      BOOL	dot11PrivacyOptionImplemented;

      /* 802.11b rates used in Legacy Supported Rates IE */
/*      UINT8   wOperRateLength;
	  char    dot11OperationalRateSet[CCK_PBCC_RATES_MAX];  */
      /* 802.11g additional rates used in Legacy Supported Rates IE */
/*      UINT8   wOperAdditionalRateLength;
	  char    dot11OperationalAdditionalRateSet[OFDM_RATES_MAX];*/
      /* 802.11g OFDM rates used in Extended Supported Rates IE */
/*      UINT8   wOperOFDMRateLength;
      char    dot11OperationalOFDMRateSet[OFDM_RATES_MAX];*/

      UINT16  dot11BeaconPeriod;
      UINT8   dot11DTIMPeriod;
      
	  UINT8   RespRateMode;

      /*======================================
      Authentication Algorithms Table
      ======================================*/
      tAUTH_ALGO_ENTRY  dot11AuthenticationAlgorithmTable[MAX_AUTH_ALGORITHM_SUPPORTED];
      
      /*=====================================
      WEP Default Keys Table
      =====================================*/
      tWEP_KEY_TYPE pbWEP_DefsKeysVal[DOT11_DEFS_KEY_NAL_LEN];
      
      /*====================================
      WEP Mappings Table
      ====================================*/
      tWEP_KEY_MAPPING_ENTRY KeyMapping[DOT11_MAX_KEY_MAPPING_LEN];
      
      /*====================================
      dot11PrivacyTable   Table
      ====================================*/
      BOOL    dot11PrivacyInvoked;
      UINT8   dot11WEPDefaultKeyID;
      UINT32  dot11WEPKeyMappingLength;
      BOOL    dot11ExcludeUnencripted;
      
      /*===================================
      dot11OperationTable
      ===================================*/
      UINT16  dot11RTSTreshold;
      UINT8   dot11ShortRetryLimit;
      UINT8   dot11LongRetryLimit;
      UINT16  dot11FragmentationTreshold;
      UINT32  dot11MaxTransmitMSDULifetime;
      UINT32  dot11MaxReceiveLifetime;
      SINT8   dot11ManufacturerID[PROD_ID_LEN];
      SINT8   dot11ProductID[PROD_ID_LEN];
      
      /*===================================
      dot11ResourceInfo Table
      ===================================*/
      UINT8   dot11manufacturerOUI[DOT11_MANUFACTURER_OUI_LENGTH];
      SINT8   dot11manufacturerName[PROD_ID_LEN];
      SINT8   dot11manufacturerProductName[PROD_ID_LEN];
      SINT8   dot11manufacturerProductVersion[PROD_ID_LEN];
      
      /*===================================
      dot11MultiDomainCapability Table
      ===================================*/
 /*     UINT8    dot11channel;*/
      BOOL     dot11Agile;
/*      UINT8    dot11PBCCAlgorithmMode;*/
      UINT8    RateFallback;
/*      UINT8    RateAdaptation;*/
/*      UINT8    dot11PreambleMode;*/
/*      UINT8    dot11SlotTimeMode;*/
/*      UINT8    dot11ERPProtectionMode;*/
/*      UINT8    dot11ERPProtectionType;*/
/*      UINT8    dot11ERPProtectionTI;*/
        UINT16   dot11SlotTimeThreshold;
/*      UINT8    dot11Mode;*/
/*	  UINT8	   Lastdot11Mode;  needed for WEB applicaiton */
/*	  UINT8    CwMinMode;*/
	  UINT32   dot11DynamicAlgTimeout;
	  UINT32   RadioCalibrateOption;
	  UINT16   RadioCalibrateInterval;
      UINT8    enableEnergyDetect;

      /*====================================
      WPA   Parameters
      ====================================*/
      UINT32	wpaMode;
      UINT32	passPhraseMode;
      UINT8		pskHex[32];
      UINT32	defaultCipher;
      UINT32	GTKInterval;
 
	  SINT8		pskPwd[DOT11_MAX_PSK_PWD_LEN];
      UINT8		pskPwdLength;
      
	  SINT8		radiusSecret[DOT11_MAX_RADIUS_SECRET_LEN];
      UINT8		radiusSecretLength;
      UINT32	radiusPort;

      UINT8		radiusIpAddr[4];
      UINT8		defaultGateway[4];

      /*===================================
      Workaround configuration Table
      ===================================*/
      UINT8    enableRestartWA;
	  UINT8    RxFreeMemOptionWA;
	  UINT8    BeaconNotSentOptionWA;
	  UINT8    TxStuckOptionWA;
	  UINT16   ConsFcsErrOptionWA;

      
      UINT32  uiNVRAMMagic;    
      UINT32  uiConsolePwdMagic;
      UINT8   whalArea[WHAL_AREA_SIZE];
		 
		 /*****************************************
		 MAC access list
     *****************************************/
		 UINT8 accessListType;
         UINT8 accessListEnabled;
		 UINT8 accessList[MGMT_ACCESS_LIST_SIZE][MAC_ADDR_LEN];


      /* Added for 4X */
	  UINT8		CurrFeatureSet;
	  BOOL		enable4x;
	  BOOL		enableConcat;
	  BOOL		enableAckEmul;	  
      BOOL      enable4xMixMode;

      /******************************************
       * TestApp support, allocated 256 bytes   *
       ******************************************/
      UINT32    ta_radioscope_com_option;    /* See TA_RADIOSCOPE_COMOPT_xxx macros */
      UINT8     ta_startup_option;       /* See TA_STARTUP_OPT_xxx macros    */
      UINT8     ta_rsvd0[3];             /* for future use */
      UINT8     ta_rsvd1[256-2*sizeof(UINT32)];

      /* SSID - Hidden and Multiple */
      UINT16    hiddenSsid;
      UINT16    multipleSsidFlag;
      UINT8     multipleSsidTable[NUM_MULTIPLE_SSID][WHAL_SSID_STRING_LEN];

      /*****************************************
      Wone information
      *****************************************/

      tWONE_NVRAM_STRUCT wone_info[MAX_WONE_NET];
      UINT8 numSupWone;    /* Number of supported wone object. Default is 1  */

      /*****************************************
      * 802.11 d & h                           *
      ******************************************/

      UINT8     dot11MultiDomainCapabilityEnable;   /* disabling d ==> disabling h */
      UINT8     dot11SpectrumManagementRequired;    /* enabling h ==> enabling d */
      UINT8     whalEnableLegacyStations;
      UINT8     dot11CurrentRegDomain;
      INT8      dot11LocalPowerConstraint;
      UINT8     whalMaxTxPowerLevel;                        
      UINT8     txPowerLevels_a[NUM_OF_TX_POWER_LEVELS];      /* levels of dBm for A band*/
      UINT8     txPowerLevels_g[NUM_OF_TX_POWER_LEVELS];      /* levels of dBm for G band*/
      char      dot11CountryString[COUNTRY_STRING_LENGTH];
      UINT8     dot11PrivateRegulatoryDomain;
      UINT8     numOfRanges_802_11_a;                       /* number of elements in private Reg Domain 'A' */
      tChannelRangeElementStruct rangeElements_a[MAX_NUM_OF_RANGES_A];
      UINT8     numOfRanges_802_11_b;                       /* number of elements in private Reg Domain 'B' */
      tChannelRangeElementStruct rangeElements_b[MAX_NUM_OF_RANGES_B];
      
      /******************************************
      * WLAN Operating state (Shutdown feature) *
      *******************************************/
	  
      BOOL		WlanServiceState;
	  
      /******************
        vHCF Parameters:
      *******************/
	  tVHCF_PARAMS  vhcfParams;

     /******************
      *  boot counter:  *
      ******************/
      UINT32 bootCounter;


   }tWHAL_NVRAM_STRUCT;



#pragma pack()
   
#define WHAL_NVRAM_MAGIC            0x4E56578d	/* magic before 4x = 0x4E565757 */
#define WHAL_CONSOLE_PWD_MAGIC      0x43507765  /* ASCII for "CPwd" */
   
#define DEF_WLAN_OPERATING_STATE	TRUE
#define DEF_NUM_WONE_SUP 1

#define DEF_DEVICE_MAC              "\x00\x50\xF1\x12\x00\x00"
#define DEF_SSID                    "TI"
#define DEF_HIDDEN_SSID             0
#define DEF_MULTIPLE_SSID_FLAG      0
#define DEF_SSID_LENGTH             2
#define DEF_AUTH_RESP_TO_MS         2000
#define DEF_PRIV_IMPLEMENTED        FALSE
/* 802.11b default rates */
#define DEF_OPER_RATE_LEN           4
#define DEF_OPER_RATE_SET           "\x82\x84\x8B\x96"
#define DEF_OPER_RATE_LEN_PLUS_PBCC 5
#define DEF_OPER_RATE_SET_PLUS_PBCC "\x82\x84\x8B\x96\x2C"

/* 802.11g default Additional rates */
#define DEF_OPER_ADDITIONAL_RATE_LEN_B_ONLY 0
#define DEF_OPER_ADDITIONAL_RATE_SET_B_ONLY ""

#define DEF_OPER_ADDITIONAL_RATE_LEN_G_ONLY 8
#define DEF_OPER_ADDITIONAL_RATE_SET_G_ONLY "\x8C\x12\x98\x24\xB0\x48\x60\x6C"

#define DEF_OPER_ADDITIONAL_RATE_LEN_MIXED  8
#define DEF_OPER_ADDITIONAL_RATE_SET_MIXED  "\x0C\x12\x18\x24\x30\x48\x60\x6C"

#define DEF_OPER_ADDITIONAL_RATE_LEN_A 8
#define DEF_OPER_ADDITIONAL_RATE_SET_A "\x8C\x12\x98\x24\xB0\x48\x60\x6C"
#define DEF_A_CHANNEL 44

/* 802.11g default OFDM rates */
#define DEF_OPER_OFDM_RATE_LEN      0
#define DEF_OPER_OFDM_RATE_SET      ""

#define DEF_BEACON_PERIOD           200
#define DEF_DTIM_PERIOD             0x02
#define DEF_AUTH_ALGO0              MIB_OPEN_SYSTEM 
#define DEF_AUTH_ALG0_ENABLE        TRUE 
#define DEF_AUTH_ALGO1              MIB_SHARED_KEY
#define DEF_AUTH_ALG1_ENABLE        FALSE   
#define DEF_PRIV_INVOKED            FALSE
#define DEF_WEP_KEY_LEN				5
#define DEF_WEP_KEY_ID              0
#define DEF_WEP_KEY_MAPPING_LEN     10
#define DEF_EXCLUDE_UNENCRIPTED     FALSE
#define DEF_RTS_TRESHOLD            2347	/* 4096 if 4x enabled */
#define DEF_SHORT_RETRY_LIMIT       7
#define DEF_LONG_RETRY_LIMIT        4
#define DEF_FRAGMENTATION_TRESHOLD  2346	/* 4096 if 4x enabled */
#define DEF_MAX_TX_MPDU_LIFETIME    512
#define DEF_MAX_RX_MPDU_LIFETIME    512
#define DEF_MANUF_ID                "TI"
#define DEF_PROD_ID                 "AP"
#define DEF_MANUF_OUI               "BC"
#define DEF_MANUF_NAME              "Texas Instruments BCIL"
#define DEF_MANUF_PROD_NAME         "AP"
#define DEF_MANUF_PROD_VER          "001"
#define DEF_CHANNEL					   6 
#define DEF_AGILE					      FALSE
#define DEF_PBCC_ALGORITHM_MODE_B  				PBCC_ALGO_DISABLED
#define DEF_PBCC_ALGORITHM_MODE_B_PLUS_AND_MIXED		PBCC_ALGO_ENHANCED_DYNAMIC 
#define DEF_PBCC_ALGORITHM_MODE_G_ONLY			PBCC_ALGO_DISABLED 
#define DEF_RATE_FALLBACK	        0
#define DEF_RATE_ADAPTATION	        1
#define DEF_PREAMBLE_MODE_B             PREAMBLE_MODE_LONG
#define DEF_PREAMBLE_MODE_G_AND_MIXED   PREAMBLE_MODE_LOCAL_STATIONS
#define DEF_SLOTTIME_MODE_B_ONLY   		SLOTTIME_MODE_DISABLED
#define DEF_SLOTTIME_MODE_G_AND_MIXED   SLOTTIME_MODE_LOCAL_STATIONS
#define DEF_SLOTTIME_THRESHOLD_B_AND_MIXED	0
#define DEF_SLOTTIME_THRESHOLD_G_ONLY		3
#define DEF_PROTECTION_MODE_B_ONLY      PROTECTION_MODE_DISABLED
#define DEF_PROTECTION_MODE_G_AND_MIXED PROTECTION_MODE_DYNAMIC
#define DEF_PROTECTION_TYPE_B_ONLY	    PROTECTION_TYPE_RTS_CTS
#define DEF_PROTECTION_TYPE_G_AND_MIXED PROTECTION_TYPE_CTS_TOSELF
#define DEF_PROTECTION_TI			    PROTECTION_TI_DISABLED
#define DEF_DOT11_MODE				DOT11_MODE_MIXED
#define DEF_DOT11_A_MODE				DOT11_MODE_A_ONLY
#define DEF_CWMIN_MODE				DOT11_CWMIN_DOT11_MODE
#define DEF_DYN_ALG_TIMEOUT			100 /* msec */
#define DEF_RADIO_CALIBRATE_OPTION	0x0000000F
#define DEF_RADIO_CALIBRATE_INTERVAL	300
#define DEF_RESP_RATE_MODE			ASSOCRESP_RATE_AP_AND_STA
#define DEF_FEATURE_SET				DOT11_FEATURE_SET_1
#define DEF_ENABLE_4X			    FALSE 
#define DEF_ENABLE_CONCAT		    FALSE 
#define DEF_ENABLE_ACK_EMUL		    FALSE /* temporary */ 
#define DEF_ENABLE_4X_MIX_MODE      FALSE   
#define DEF_ENERGY_DETECT			TRUE
   
#define DEF_ENABLE_RESTART_WA		TRUE
#define DEF_WA_RX_FREE_MEM_OPTION	1
#define DEF_WA_BCN_NOT_SENT_OPTION	3
#define DEF_WA_TX_STUCK_OPTION		5
#define DEF_WA_CONS_FCS_ERR_OPTION	1
#define DEF_FW_CONS_FCS_ERR_THRESHOLD	0
   
		/*====================================
					WPA   Section
		====================================*/
#define DEF_WPA_MODE				DOT11_WPA_STA_TYPE_WEP_LEGACY
#define DEF_PSK_MODE				DOT11_WPA_PSK_PARAM_NONE
#define DEF_PSK_HEX					0
#define DEF_CIPHER					DOT11_CIPHER_WEP_40
#define DEF_GTK_INTERVAL			60 * 60 * 1000		/* 1 hour */
#define DEF_PSK_PWD					"PSK"
#define DEF_PSK_PWD_LENGTH            3
#define DEF_RADIUS_SECRET			"RADIUS-SECRET"
#define DEF_RADIUS_SECRET_LEN            13
#define DEF_RADIUS_PORT					 1812

#define DEF_RADIUS_IP_ADDR_1			192
#define DEF_RADIUS_IP_ADDR_2			168
#define DEF_RADIUS_IP_ADDR_3			0
#define DEF_RADIUS_IP_ADDR_4			3

#define DEF_GATEWAY_1					192
#define DEF_GATEWAY_2					168
#define DEF_GATEWAY_3					0
#define DEF_GATEWAY_4					2


#define DEF_REPORT_LEVEL			   0 
#define DEF_CONSOLE_PASSWORD        "wlan"
   
   void whal_nvram_UserInit(UINT8 **pAddr, UINT16 *pwLen);
   int whal_nvram_setDefaults(UINT8 woneIndex);
   int whal_nvram_setDefaultsPartial(UINT8 woneIndex, UINT8 dot11Mode);
   int whal_nvram_getAuthenticationResponceTimeOut(UINT32 *puiOutput);
   int whal_nvram_setAuthenticationResponceTimeOut(UINT32 uiInput,
      UINT8 bSaveDelay);
   int whal_nvram_setPrivacyOptionImplemented(BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_getPrivacyOptionImplemented(BOOL *puiOutput);

   /* 802.11b rates access functions  */
   /* Always exist used in the Legacy Supported Rates IE */
   int whal_nvram_setOperationalRateSetLength(UINT8 woneIndex,UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getOperationalRateSetLength(UINT8 woneIndex,UINT8 *pwOutput);
   int whal_nvram_setOperationalRateSet(UINT8 woneIndex,char *pbInputArray,UINT8 wLen,UINT8 bSaveDelay);
   int whal_nvram_getOperationalRateSet(UINT8 woneIndex,char *pbOutputArray, UINT8 wLen);

   /* 802.11g Additional rates access functions  */
   /* If exists, it is added to the Legacy Supported Rates IE */
   int whal_nvram_setOperationalAdditionalRateSetLength(UINT8 woneIndex, UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getOperationalAdditionalRateSetLength(UINT8 woneIndex, UINT8 *pwOutput);
   int whal_nvram_setOperationalAdditionalRateSet(UINT8 woneIndex, char *pbInputArray,UINT8 wLen,UINT8 bSaveDelay);
   int whal_nvram_getOperationalAdditionalRateSet(UINT8 woneIndex, char *pbOutputArray, UINT8 wLen);

   /* 802.11g OFDM rates access functions */
   /* If exists, used in the Extended Supported Rates IE */
   int whal_nvram_setOperationalOFDMRateSetLength(UINT8 woneIndex, UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getOperationalOFDMRateSetLength(UINT8 woneIndex, UINT8 *pwOutput);
   int whal_nvram_setOperationalOFDMRateSet(UINT8 woneIndex, char *pbInputArray,UINT8 wLen,UINT8 bSaveDelay);
   int whal_nvram_getOperationalOFDMRateSet(UINT8 woneIndex, char *pbOutputArray, UINT8 wLen);

   int whal_nvram_setBeaconPeriod(UINT16 wInput,UINT8 bSaveDelay);
   int whal_nvram_getBeaconPeriod(UINT16 *pwOutput);
   int whal_nvram_setDTIMPeriod(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getDTIMPeriod(UINT8 *pwOutput);
   int whal_nvram_setAuthenticationAlgorithm(UINT32 index, UINT16 uiInput,UINT8 bSaveDelay);
   int whal_nvram_getAuthenticationAlgorithm(UINT32 index, UINT16 *puiOutput);
   int whal_nvram_setAuthenticationAlgorithmsEnable(UINT32 index, BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_getAuthenticationAlgorithmsEnable(UINT32 index, BOOL *puiOutput);
   int whal_nvram_setWEP_DefsKeysVal(UINT16 wInd,
      UINT8	len, 
      UINT8 *pbInputArray,
      UINT8 bSaveDelay);
   int whal_nvram_getWEP_DefsKeysVal(UINT16 wInd,UINT8 *len, UINT8 *pbOutputArray);
   int whal_nvram_setKeyMapping(UINT32 wInd,
      tWEP_KEY_MAPPING_ENTRY *pbInputArray,
      UINT8 bSaveDelay);
   int whal_nvram_getKeyMapping(UINT32 wInd,tWEP_KEY_MAPPING_ENTRY *pbOutputArray);
   int whal_nvram_setPrivacyInvoked(BOOL wInput,UINT8 bSaveDelay);
   int whal_nvram_getPrivacyInvoked(BOOL *pwOutput);
   int whal_nvram_setWEPDefaultKeyID(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getWEPDefaultKeyID(UINT8 *pwOutput);
   int whal_nvram_setWEPKeyMappingLength(UINT32 uiInput,UINT8 bSaveDelay);
   int whal_nvram_getWEPKeyMappingLength(UINT32 *puiOutput);
   int whal_nvram_setExcludeUnencripted(BOOL wInput,UINT8 bSaveDelay);
   int whal_nvram_getExcludeUnencripted(BOOL *pwOutput);
   int whal_nvram_setMacAddress(UINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getMacAddress(UINT8 *pbOutputArray);
   int whal_nvram_setSsid(UINT8 *pbInputArray, UINT8 len, UINT8 bSaveDelay);
   int whal_nvram_getSsid(UINT8 *pbOutputArray, UINT8 *len);
   int whal_nvram_setHiddenSsidStatus(UINT16 wInput, UINT8 bSaveDelay);
   int whal_nvram_getHiddenSsidStatus(UINT16 *pwOutput);
   int whal_nvram_setMultipleSsidStatus(UINT16 wInput, UINT8 bSaveDelay);
   int whal_nvram_getMultipleSsidStatus(UINT16 *pwOutput);
   int whal_nvram_setMultipleSsidTable(char *pbInputArray, UINT32 len, UINT8 bSaveDelay);
   int whal_nvram_getMultipleSsidTable(char *pbOutputArray, UINT32 *len);
   int whal_nvram_setRTSTreshold(UINT16 wInput,UINT8 bSaveDelay);
   int whal_nvram_getRTSTreshold(UINT16 *pwOutput);
   int whal_nvram_setShortRetryLimit(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getShortRetryLimit(UINT8 *pwOutput);
   int whal_nvram_setLongRetryLimit(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getLongRetryLimit(UINT8 *pwOutput);
   int whal_nvram_setFragmentationTreshold(UINT16 wInput,UINT8 bSaveDelay);
   int whal_nvram_getFragmentationTreshold(UINT16 *pwOutput);
   int whal_nvram_setMaxTransmitMSDULifetime(UINT32 uiInput,UINT8 bSaveDelay);
   int whal_nvram_getMaxTransmitMSDULifetime(UINT32 *puiOutput);
   int whal_nvram_setMaxReceiveLifetime(UINT32 uiInput,UINT8 bSaveDelay);
   int whal_nvram_getMaxReceiveLifetime(UINT32 *puiOutput);
   int whal_nvram_setManufacturerID(SINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getManufacturerID(SINT8 *pbOutputArray);
   int whal_nvram_setProductID(SINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getProductID(SINT8 *pbOutputArray);
   int whal_nvram_setManufacturerOUI(UINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getManufacturerOUI(UINT8 *pbOutputArray);
   int whal_nvram_setManufacturerName(SINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getManufacturerName(SINT8 *pbOutputArray);
   int whal_nvram_setManufacturerProductName(SINT8 *pbInputArray,UINT8 bSaveDelay);
   int whal_nvram_getManufacturerProductName(SINT8 *pbOutputArray);
   int whal_nvram_setManufacturerProductVersion(SINT8 *pbInputArray,
      UINT8 bSaveDelay);
   int whal_nvram_getManufacturerProductVersion(SINT8 *pbOutputArray);
   int whal_nvram_getReportLevel(UINT32 *level);
   int whal_nvram_setReportLevel(UINT32 level, UINT8 bSaveDelay);
   
   int whal_nvram_setChannel(UINT8 woneIndex, UINT8 channel, UINT8 bSaveDelay);
   int whal_nvram_getChannel(UINT8 woneIndex, UINT8 *channel);
   int whal_nvram_setBand(UINT8 woneIndex, UINT8 band, UINT8 bSaveDelay);
   int whal_nvram_getBand(UINT8 woneIndex, UINT8 *band);
   int whal_nvram_setUsageTime(UINT8 woneIndex, UINT8 usageTime, UINT8 bSaveDelay);
   int whal_nvram_getUsageTime(UINT8 woneIndex, UINT8 *usageTime);

   int whal_nvram_setNumWoneSup(UINT8 numSupWone, UINT8 bSaveDelay);
   int whal_nvram_getNumWoneSup(UINT8 *numSupWone);
   int whal_nvram_setVhcfParams(tVHCF_PARAMS * vhcfParamsPtr, UINT8 bSaveDelay);
   int whal_nvram_getVhcfParams(tVHCF_PARAMS * vhcfParamsPtr);
   int whal_nvram_setAgile(BOOL value, UINT8 bSaveDelay);
   int whal_nvram_getAgile(BOOL *value);
   int whal_nvram_setPreambleMode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getPreambleMode(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setPBCCAlgorithmMode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getPBCCAlgorithmMode(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setPBCCAlgorithmPeriod(UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getPBCCAlgorithmPeriod(UINT8 *value);
   int whal_nvram_setPBCCAlgorithmIgnoreMcast(UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getPBCCAlgorithmIgnoreMcast(UINT8 *value);
   int whal_nvram_setRateFallback(UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getRateFallback(UINT8 *value);
   int whal_nvram_setRateAdaptation(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getRateAdaptation(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setSlotTimeMode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getSlotTimeMode(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setSlotTimeThreshold(UINT16 value, UINT8 bSaveDelay);
   int whal_nvram_getSlotTimeThreshold(UINT16 *value);
   int whal_nvram_setERPProtectionMode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getERPProtectionMode(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setERPProtectionType(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getERPProtectionType(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setERPProtectionTI(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getERPProtectionTI(UINT8 woneIndex, UINT8 *value);
   
   int whal_nvram_setDot11Mode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getDot11Mode(UINT8 woneIndex, UINT8 *value);
   int whal_nvram_setLastDot11Mode(UINT8 woneIndex, UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getLastDot11Mode(UINT8 woneIndex, UINT8 *value);

   int whal_nvram_setRadioCalibrateOption(UINT32 value, UINT8 bSaveDelay);
   int whal_nvram_getRadioCalibrateOption(UINT32 *value);
   int whal_nvram_setRadioCalibrateInterval(UINT16 value, UINT8 bSaveDelay);
   int whal_nvram_getRadioCalibrateInterval(UINT16 *value);

   int whal_nvram_setFeatureSet(UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getFeatureSet(UINT8 *value);

   int whal_nvram_setRespRateMode(UINT8 value, UINT8 bSaveDelay);
   int whal_nvram_getRespRateMode(UINT8 *value);

   int whal_nvram_destroyMagicNumber(UINT8 bSaveDelay);
   int whal_nvram_getConsolePassword(UINT8 *pbPassword);
   int whal_nvram_setConsolePassword(UINT8 *pbPassword,UINT8 bSaveDelay);
   
   int whal_nvram_getWhalArea(UINT8 *whalArea);
   int whal_nvram_setWhalArea(UINT8 *whalArea,UINT8 bSaveDelay);
   
   int whal_nvram_setEnableRestartWA(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getEnableRestartWA(UINT8 *pwOutput);

   int whal_nvram_setWAoptions(UINT8 rxFreeMem,UINT8 bcnNotSent,UINT8 txStuck, UINT16 consFcsErr, UINT8 bSaveDelay);
   int whal_nvram_getWAoptions(UINT8 *rxFreeMem,UINT8 *bcnNotSent,UINT8 *txStuck, UINT16 *consFcsErr);

   int whal_nvram_setEnableEnergyDetect(UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getEnableEnergyDetect(UINT8 *pwOutput);

   /* 4x support */
   int whal_nvram_setEnable4x(BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_getEnable4x(BOOL *puiOutput);

   int whal_nvram_setEnableConcat(BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_getEnableConcat(BOOL *puiOutput);

   int whal_nvram_setEnableAckEmul(BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_getEnableAckEmul(BOOL *puiOutput);

   int whal_nvram_setCwMinMode(UINT8 woneIndex, UINT8 wInput,UINT8 bSaveDelay);
   int whal_nvram_getCwMinMode(UINT8 woneIndex, UINT8 *pwOutput);

   int whal_nvram_set4xMixMode(BOOL uiInput,UINT8 bSaveDelay);
   int whal_nvram_get4xMixMode(BOOL *puiOutput);

   /* 802.11 d & h */
   int whal_nvram_setMultiDomainCapability(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getMultiDomainCapability(UINT8 *pValue);
   int whal_nvram_setSpectrumManagementRequired(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getSpectrumManagementRequired(UINT8 *pValue);
   int whal_nvram_setEnableLegacyStations(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getEnableLegacyStations(UINT8 *pValue);
   int whal_nvram_setCurrentRegDomain(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getCurrentRegDomain(UINT8 *pValue);
   int whal_nvram_setLocalPowerConstraint(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getLocalPowerConstraint(UINT8 *pValue);
   int whal_nvram_setWhalMaxTxPowerLevel(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getWhalMaxTxPowerLevel(UINT8 *pValue);
   int whal_nvram_setTxPowerLevel(UINT8 band, UINT32 level, INT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getTxPowerLevel(UINT8 band, UINT32 level, INT8 *pValue);
   int whal_nvram_setCountryString(char *uiInput, UINT8 bSaveDelay);
   int whal_nvram_getCountryString(char *uiOutput);
   int whal_nvram_setPrivateRegDomain(UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getPrivateRegDomain(UINT8 *pValue);
   int whal_nvram_setNumOfRanges4RegDomain(wlan_bandType bandType, UINT8 uiInput, UINT8 bSaveDelay);
   int whal_nvram_getNumOfRanges4RegDomain(wlan_bandType bandType, UINT8 *pValue);
   int whal_nvram_setRangeElement4RegDomain(wlan_bandType bandType, 
                                            UINT32 index,
                                            tChannelRangeElementStruct uiInput,
                                            UINT8 bSaveDelay);
   int whal_nvram_getRangeElement4RegDomain(wlan_bandType bandType, 
                                            UINT32 index,
                                            tChannelRangeElementStruct *pStruct);


   /*********************
    * TestApp functions *
    *********************/
   int whal_nvram_get_ta_startup_option( UINT8 *pbOutput);
   int whal_nvram_set_ta_startup_option( UINT8 bInput, UINT8 bSaveDelay);

   int whal_nvram_get_ta_radioscope_com_option( UINT32 *puOutput);
   int whal_nvram_set_ta_radioscope_com_option( UINT32 uInput, UINT8 bSaveDelay);

   /***********************
   * Mgmt Access Routines *
   ************************/
   int whal_nvram_setAccessList(char ** list,  UINT8 bSaveDelay);
   int whal_nvram_getAccessList(char *pList);

   int whal_nvram_getAccessListEnable(UINT8 *value);
   int whal_nvram_setAccessListEnable(UINT8 value, UINT8 bSaveDelay);
   
   int whal_nvram_getAccessListType(UINT8 *value);
   int whal_nvram_setAccessListType(UINT8 value, UINT8 bSaveDelay);

		/*====================================
					WPA   Section
		====================================*/
   int whal_nvram_setWpaMode(UINT32 mode, UINT8 bSaveDelay);
   int whal_nvram_getWpaMode(UINT32 *mode);

   int whal_nvram_setPassPhraseMode(UINT32 mode, UINT8 bSaveDelay);
   int whal_nvram_getPassPhraseMode(UINT32 *mode);

   int whal_nvram_setPskHex(UINT8 *value, UINT8 bSaveDelay);
   int whal_nvram_getPskHex(UINT8 *value);

   int whal_nvram_setDefaultCipher(UINT32 value, UINT8 bSaveDelay);
   int whal_nvram_getDefaultCipher(UINT32 *value);

   int whal_nvram_setGTKInterval(UINT32 value, UINT8 bSaveDelay);
   int whal_nvram_getGTKInterval(UINT32 *value);

   int whal_nvram_setPskPwd(UINT8 *pbInputArray, UINT8 len, UINT8 bSaveDelay);
   int whal_nvram_getPskPwd(UINT8 *pbOutputArray, UINT8 *len);

   int whal_nvram_setRadiusSecret(UINT8 *pbInputArray, UINT8 len, UINT8 bSaveDelay);
   int whal_nvram_getRadiusSecret(UINT8 *pbOutputArray, UINT8 *len);

   int whal_nvram_setRadiusPort(UINT32 value, UINT8 bSaveDelay);
   int whal_nvram_getRadiusPort(UINT32 *value);

   int whal_nvram_setRadiusIpAddr(UINT8 addr1, UINT8 addr2, UINT8 addr3, UINT8 addr4, UINT8 bSaveDelay);
   int whal_nvram_getRadiusIpAddr(UINT8 *addr1, UINT8 *addr2, UINT8 *addr3, UINT8 *addr4);

   int whal_nvram_setDefaultGateway(UINT8 addr1, UINT8 addr2, UINT8 addr3, UINT8 addr4, UINT8 bSaveDelay);
   int whal_nvram_getDefaultGateway(UINT8 *addr1, UINT8 *addr2, UINT8 *addr3, UINT8 *addr4);

   /*****************************************************
   * Boot Counter routines
   * Note: Never don't associate these routines 
   *       with CLI (Shell) commands!!
   *****************************************************/
   extern int whal_nvram_UpdateBootCounter(void);
   extern int resetBootCounter(void);
   extern int whal_nvram_getBootCounter(void);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _WHAL_NVRAM_H_ */


