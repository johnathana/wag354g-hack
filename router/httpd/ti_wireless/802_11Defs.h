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
**|     Copyright (c) 1998-2002 Texas Instruments Incorporated           |**
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
 *   MODULE:  802_11Defs.h
 *   PURPOSE: Contains 802.11 defines/structures
 *
 ****************************************************************************/

#ifndef _802_11_INFO_DEFS_H
#define _802_11_INFO_DEFS_H

#include "pform_types.h"

/* FrameControl field of the 802.11 header
//
// bit 15    14     13     12     11     10      9     8     7-4     3-2      1-0
// +-------+-----+------+-----+-------+------+------+----+---------+------+----------+
// | Order | WEP | More | Pwr | Retry | More | From | To | Subtype | Type | Protocol |
// |       |     | Data | Mgmt|       | Frag | DS   | DS |         |      | Version  |
// +-------+-----+------+-----+-------+------+------+----+---------+------+----------+
//     1      1      1     1      1       1      1     1       4       2        2
*/

#define DOT11_FC_PROT_VERSION_MASK   ( 3 << 0 )
#define DOT11_FC_PROT_VERSION		 ( 0 << 0 )

#define DOT11_FC_TYPE_MASK         	 ( 3 << 2 )
typedef enum
{
  DOT11_FC_TYPE_MGMT         = ( 0 << 2 ),
  DOT11_FC_TYPE_CTRL         = ( 1 << 2 ),
  DOT11_FC_TYPE_DATA         = ( 2 << 2 )
} dot11_Fc_Type_T;

#define DOT11_FC_SUB_MASK         	( 0x0f << 4 )
typedef enum
{
  /* Management subtypes */
  DOT11_FC_SUB_ASSOC_REQ     = (    0 << 4 ),
  DOT11_FC_SUB_ASSOC_RESP    = (    1 << 4 ),
  DOT11_FC_SUB_REASSOC_REQ   = (    2 << 4 ),
  DOT11_FC_SUB_REASSOC_RESP  = (    3 << 4 ),
  DOT11_FC_SUB_PROBE_REQ     = (    4 << 4 ),
  DOT11_FC_SUB_PROBE_RESP    = (    5 << 4 ),
  DOT11_FC_SUB_BEACON        = (    8 << 4 ),
  DOT11_FC_SUB_ATIM          = (    9 << 4 ),
  DOT11_FC_SUB_DISASSOC      = (   10 << 4 ),
  DOT11_FC_SUB_AUTH          = (   11 << 4 ),
  DOT11_FC_SUB_DEAUTH        = (   12 << 4 ),

  /* Control subtypes */
  DOT11_FC_SUB_PS_POLL                = (   10 << 4 ),
  DOT11_FC_SUB_RTS                    = (   11 << 4 ),
  DOT11_FC_SUB_CTS                    = (   12 << 4 ),
  DOT11_FC_SUB_ACK                    = (   13 << 4 ),
  DOT11_FC_SUB_CF_END                 = (   14 << 4 ),
  DOT11_FC_SUB_CF_END_CF_ACK          = (   15 << 4 ),

  /* Data subtypes */
  DOT11_FC_SUB_DATA                   = (    0 << 4 ),
  DOT11_FC_SUB_DATA_CF_ACK            = (    1 << 4 ),
  DOT11_FC_SUB_DATA_CF_POLL           = (    2 << 4 ),
  DOT11_FC_SUB_DATA_CF_ACK_CF_POLL    = (    3 << 4 ),
  DOT11_FC_SUB_NULL_FUNCTION          = (    4 << 4 ),
  DOT11_FC_SUB_CF_ACK                 = (    5 << 4 ),
  DOT11_FC_SUB_CF_POLL                = (    6 << 4 ),
  DOT11_FC_SUB_CF_ACK_CF_POLL         = (    7 << 4 )
} dot11_Fc_Sub_Type_T;

#define  DOT11_FC_TYPESUBTYPE_MASK    ( DOT11_FC_TYPE_MASK | DOT11_FC_SUB_MASK )
typedef enum
{
  DOT11_FC_ASSOC_REQ           = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_ASSOC_REQ           ),
  DOT11_FC_ASSOC_RESP          = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_ASSOC_RESP          ),
  DOT11_FC_REASSOC_REQ         = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_REASSOC_REQ         ),
  DOT11_FC_REASSOC_RESP        = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_REASSOC_RESP        ),
  DOT11_FC_PROBE_REQ           = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_PROBE_REQ           ),
  DOT11_FC_PROBE_RESP          = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_PROBE_RESP          ),
  DOT11_FC_BEACON              = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_BEACON              ),
  DOT11_FC_ATIM                = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_ATIM                ),
  DOT11_FC_DISASSOC            = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_DISASSOC            ),
  DOT11_FC_AUTH                = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_AUTH                ),
  DOT11_FC_DEAUTH              = ( DOT11_FC_TYPE_MGMT | DOT11_FC_SUB_DEAUTH              ),
  DOT11_FC_PS_POLL             = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_PS_POLL             ),
  DOT11_FC_RTS                 = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_RTS                 ),
  DOT11_FC_CTS                 = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_CTS                 ),
  DOT11_FC_ACK                 = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_ACK                 ),
  DOT11_FC_CF_END              = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_CF_END              ),
  DOT11_FC_CF_END_CF_ACK       = ( DOT11_FC_TYPE_CTRL | DOT11_FC_SUB_CF_END_CF_ACK       ),
  DOT11_FC_DATA                = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_DATA                ),
  DOT11_FC_DATA_CF_ACK         = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_DATA_CF_ACK         ),
  DOT11_FC_DATA_CF_POLL        = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_DATA_CF_POLL        ),
  DOT11_FC_DATA_CF_ACK_CF_POLL = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_DATA_CF_ACK_CF_POLL ),
  DOT11_FC_DATA_NULL_FUNCTION  = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_NULL_FUNCTION       ),
  DOT11_FC_CF_ACK              = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_CF_ACK              ),
  DOT11_FC_CF_POLL             = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_CF_POLL             ),
  DOT11_FC_CF_ACK_CF_POLL      = ( DOT11_FC_TYPE_DATA | DOT11_FC_SUB_CF_ACK_CF_POLL      )
} dot11_Fc_Type_Sub_Type_T;

typedef enum
{
  DOT11_FC_TO_DS               = ( 1 << 8  ),
  DOT11_FC_FROM_DS             = ( 1 << 9  ),
  DOT11_FC_MORE_FRAG           = ( 1 << 10 ),
  DOT11_FC_RETRY               = ( 1 << 11 ),
  DOT11_FC_PWR_MGMT            = ( 1 << 12 ),
  DOT11_FC_MORE_DATA           = ( 1 << 13 ),
  DOT11_FC_WEP                 = ( 1 << 14 ),
  DOT11_FC_ORDER               = ( 1 << 15 )
} dot11_Fc_Other_T;

typedef enum
{
  DOT11_CAPABILITY_ESS               = ( 1 ),
  DOT11_CAPABILITY_IESS              = ( 1 << 1 ),
  DOT11_CAPABILITY_CF_POLLABE        = ( 1 << 2 ),
  DOT11_CAPABILITY_CF_POLL_REQ       = ( 1 << 3 ),
  DOT11_CAPABILITY_PRIVACY           = ( 1 << 4 )
} dot11_Capability_T;

#define  DOT11_FC_TO_DS_SHIFT        8
#define  DOT11_FC_FROM_DS_SHIFT      9
#define  DOT11_FC_MORE_FRAG_SHIFT   10
#define  DOT11_FC_RETRY_SHIFT       11
#define  DOT11_FC_PWR_MGMT_SHIFT    12
#define  DOT11_FC_MORE_DATA_SHIFT   13
#define  DOT11_FC_WEP_SHIFT         14
#define  DOT11_FC_ORDER_SHIFT    	15

#define IS_WEP_ON(fc) ((TRUE << DOT11_FC_WEP_SHIFT) & (fc))

/* SequenceControl field of the 802.11 header
//
// bit    15 - 4           3 - 0
// +-------------------+-----------+
// |  Sequence Number  | Fragment  |
// |                   |  Number   |
// +-------------------+-----------+
//         12                4
*/
typedef enum
{
  DOT11_SC_FRAG_NUM_MASK = ( 0xf   << 0 ),
  DOT11_SC_SEQ_NUM_MASK  = ( 0xfff << 4 )
} dot11_Sc_T;


typedef struct
{
  UINT8 addr[MAC_ADDR_LEN];
} macAddress_T;


#pragma pack(2)
typedef struct
{
  UINT16		fc;
  UINT16		duration;
  macAddress_T	address1;
  macAddress_T	address2;
  macAddress_T	address3;
  UINT16		seqCtrl;
  macAddress_T	address4;
} dot11_header_T;
#pragma pack()


/*
 *  Address 4 is not used for AP<->STA streams. 
 */
#pragma pack(2)
typedef struct
{
  UINT16		fc;
  UINT16		duration;
  macAddress_T	address1;
  macAddress_T	address2;
  macAddress_T	address3;
  UINT16		seqCtrl;
} dot11_AP_header_T;
#pragma pack()


#pragma pack(1)
typedef struct
{
  UINT8  	DSAP;
  UINT8     SSAP;
  UINT8     Control;
  UINT8     OUI[3];
  UINT16		Type;
} Wlan_LlcHeader_T;
#pragma pack()



#pragma pack(2)
typedef struct
{
   dot11_AP_header_T dot11Header;
   Wlan_LlcHeader_T  snapHeader;
} dot11_DataMsduHeader_T;
#pragma pack()




#pragma pack(2)
typedef struct
{
  UINT16		fc;
  UINT16		duration;
  macAddress_T	DA;
  macAddress_T	SA;
  macAddress_T	BSSID;
  UINT16		seqCtrl;
} dot11_mgmtHeader_T;
#pragma pack()



#define WLAN_HDR_LEN         24
#define WLAN_SNAP_HDR_LEN     8


#define WEP_KEY_LEN_MAX	29
#define WEP_KEY_LEN_MIN 5

#define FCS_SIZE	4

/* data body max length */
#define MAX_DATA_BODY_LENGTH				2312

/*
 * MANAGEMENT
 * -----------------
 */

/* mgmt body max length */
#define MAX_MGMT_BODY_LENGTH				2312

/* general mgmt frame structure */
typedef struct
{
	dot11_mgmtHeader_T	hdr;
	char				body[MAX_MGMT_BODY_LENGTH];
} dot11_mgmtFrame_T;

/* Capabilities Information Field - IN THE AIR
//
//  bit  15      14       13         12        11         10      9      8		7	-	0
// +----------+------+----------+---------+----------+---------+------+-----+---------------+
// |  Channel |      |  Short   | Privacy | CF Poll  |   CF    |      |     |	RESERVED	|	
// |  Agility | PBCC | Preamble |         | Request  | Pollable| IBSS | ESS |				|
// +----------+------+----------+---------+----------+---------+------+-----+---------------+	
//       1        1       1          1         1          1       1      1
*/

/* Capabilities Information Field - IN THE MGMT SOFTWARE AFTER THE SWAP
//
// bit 15 - 8         7        6       5          4         3          2       1      0
// +------------+----------+------+----------+---------+----------+---------+------+-----+
// |            |  Channel |      |  Short   | Privacy | CF Poll  |   CF    |      |     |
// |  Reserved  |  Agility | PBCC | Preamble |         | Request  | Pollable| IBSS | ESS |
// +------------+----------+------+----------+---------+----------+---------+------+-----+
//       8            1        1       1          1         1          1       1      1
*/


typedef enum
{ 
  DOT11_CAPS_ESS             = ( 1 << 0 ),
  DOT11_CAPS_IBSS            = ( 1 << 1 ),
  DOT11_CAPS_CF_POLLABLE     = ( 1 << 2 ),
  DOT11_CAPS_CF_POLL_REQUEST = ( 1 << 3 ),
  DOT11_CAPS_PRIVACY         = ( 1 << 4 ),
  DOT11_CAPS_SHORT_PREAMBLE  = ( 1 << 5 ),
  DOT11_CAPS_PBCC            = ( 1 << 6 ),
  DOT11_CAPS_CHANNEL_AGILITY = ( 1 << 7 ),
  DOT11_CAPS_SLOTTIME        = ( 1 << 10 ),
} dot11_capabilities_T;

typedef enum 
{
	/* ESS */
	CAP_ESS_MASK			= 1,
	CAP_ESS_SHIFT			= 0,

	/* IBSS */
	CAP_IBSS_MASK			= 1,
	CAP_IBSS_SHIFT			= 1,

	/* CF Pollable */
	CAP_CF_POLL_MASK		= 1,
	CAP_CF_POLL_SHIFT		= 2,
	
	/* CF Poll request */
	CAP_CF_REQ_MASK			= 1,
	CAP_CF_REQ_SHIFT		= 3,
	
	/* Privacy */
	CAP_PRIVACY_MASK		= 1,
	CAP_PRIVACY_SHIFT		= 4,

	/* Short Preamble*/
	CAP_PREAMBLE_MASK		= 1,
	CAP_PREAMBLE_SHIFT		= 5,
	
	/* PBCC */
	CAP_PBCC_MASK			= 1,
	CAP_PBCC_SHIFT			= 6,
	
	/* Agile */
	CAP_AGILE_MASK			= 1,
	CAP_AGILE_SHIFT			= 7,
	
	/* Short SlotTime */
	CAP_SLOTTIME_MASK		= 1,
	CAP_SLOTTIME_SHIFT		= 10,
	
	/* Enhanced Security  - bit 11*/
	CAP_RSN_MASK			= 1,
	CAP_RSN_SHIFT			= 11,
} wdrv_mgmtCapabilities_T;


/*
 * 802.11 Information elements
 * ---------------------------
 */

typedef struct
{
  UINT8 eleId;
  UINT8 eleLen;
} dot11_eleHdr_T;

/* fixed fields lengths, except of currentAP & timestamp*/
#define FIX_FIELD_LEN		2

/* SSID Information Element */
#define DOT11_SSID_ELE_ID	0
#define DOT11_SSID_MAX		32
typedef struct 
{
  dot11_eleHdr_T	hdr;
  char				serviceSetId[ DOT11_SSID_MAX ];
} dot11_SSID_T;


/* Max number of rates supported by the AP and saved in NVRAM */
#define CCK_PBCC_RATES_MAX				5
#define OFDM_RATES_MAX					8


/* Supported rates Information Element */
#define DOT11_SUPPORTED_RATES_ELE_ID		1
#define DOT11_SUPPORTED_RATES_MAX			14 /* 802.11gD5.0 => 14; 802.11gD6.1 => 8 */
typedef struct 
{
  dot11_eleHdr_T hdr;
  UINT8 rates[ DOT11_SUPPORTED_RATES_MAX ];
} dot11_supRates_T;


/* Country Information - Information Element */
#define DOT11_COUNTRY_INFORMATION_ELE_ID		7
#define DOT11_COUNTRY_INFORMATION_MAX			24 /* This should be updated when more than 24 are needed */
#define DOT11_COUNTRY_STRING_LENGTH             3
typedef struct
{
    UINT8 firstChannelNumber;
    UINT8 numberOfChannels;
    UINT8 maxTransmitPowerLevel;
} channelInformation_CountryInfo;

#pragma pack(1)
typedef struct 
{
  dot11_eleHdr_T hdr;
  UINT8 CountryString[DOT11_COUNTRY_STRING_LENGTH];
  channelInformation_CountryInfo channelInfo[ DOT11_COUNTRY_INFORMATION_MAX ];
  UINT8 padding;
} dot11_CountryInfo_T;
#pragma pack()


/* Extended Supported rates Information Element */
#define DOT11_EXT_SUPPORTED_RATES_ELE_ID		50
#define DOT11_EXT_SUPPORTED_RATES_MAX			14 /* 255 in standard, save memory ? */
typedef struct 
{
  dot11_eleHdr_T hdr;
  UINT8 rates[ DOT11_EXT_SUPPORTED_RATES_MAX ];
} dot11_extSupRates_T;


/* Rates constants used in dot11_supRates_T & dot11_extSupRates_T IE */
#define DOT11_RATE_1						2
#define DOT11_RATE_2						4
#define DOT11_RATE_5_5						11
#define DOT11_RATE_6						12
#define DOT11_RATE_9						18
#define DOT11_RATE_11						22
#define DOT11_RATE_12						24
#define DOT11_RATE_18						36
#define DOT11_RATE_22						44
#define DOT11_RATE_24						48
#define DOT11_RATE_33						66
#define DOT11_RATE_36						72
#define DOT11_RATE_48						96
#define DOT11_RATE_54						108


/* ERP Information Element */
#define DOT11_ERP_ELE_ID				42
#define DOT11_ERP_ELE_LEN				1
#define DOT11_ERP_NONERP_PRESENT		0x01
#define DOT11_ERP_USE_PROTECTION		0x02
typedef struct 
{
  dot11_eleHdr_T    hdr;
  UINT8             erp;
} dot11_ERP_T;


/* SSID Information Element */
#define DOT11_TI_CAP_ELE_ID		0xDD
#define DOT11_4X_FEATURE_SET_ZERO_LEN 4 
#define DOT11_TI_CAP_MAX_LEN		40
#define DOT11_OUI_LEN		3
#define DOT11_4X_TYPE		0

typedef enum
{
	DOT11_FEATURE_SET_0 = 0,
	DOT11_FEATURE_SET_1 = 1,
	DOT11_LAST_FEATURE_SET
} dot11_FeatureSet_e;

typedef struct 
{
  dot11_eleHdr_T	hdr;
  char				cap[DOT11_TI_CAP_MAX_LEN];
} dot11_TIcap_T;


/* DS params Information Element */
#define DOT11_DS_PARAMS_ELE_ID		3
#define DOT11_DS_PARAMS_ELE_LEN		1
typedef struct 
{
  dot11_eleHdr_T hdr;
  UINT8  currChannel;
} dot11_dsParams_T;

/* tim Information Element */
#define DOT11_TIM_ELE_ID	5
#define DOT11_PARTIAL_VIRTUAL_BITMAP_MAX	251
typedef struct 
{
	dot11_eleHdr_T	hdr;
	UINT8			dtimCount;
    UINT8			dtimPeriod;
    UINT8			bmapControl;
    UINT8			partialVirtualBmap[DOT11_PARTIAL_VIRTUAL_BITMAP_MAX];
} dot11_tim_T;

/* tim Information Element */
#define DOT11_CF_ELE_ID				4
#define DOT11_CF_PARAMS_ELE_LEN		6
typedef struct 
{
	dot11_eleHdr_T	hdr;
	UINT8			cfpCount;
    UINT8			cfpPeriod;
    UINT16			cfpMaxDuration;
    UINT16			cfpDurRemain;
} dot11_cfParams_T;

/* Challenge text Information Element */
#define DOT11_CHALLENGE_TEXT_ELE_ID		16
#define DOT11_CHALLENGE_TEXT_MAX		128
typedef struct 
{
	dot11_eleHdr_T	hdr;
	UINT8			text[ DOT11_CHALLENGE_TEXT_MAX ];
} dot11_Chlg_T;

/* RSNE Information Element */
#define DOT11_RSNE_MS_ELE_ID		0xDD		/* Microsoft RSN Ele Id*/
#define DOT11_RSNE_MAX				255	

#define SUITE_LEN	4

typedef struct 
{
	dot11_eleHdr_T hdr;
	UINT8 rsne[ DOT11_RSNE_MAX ];
} dot11_rsne_T;

/* the offset in bytes from the beginning of the RSN ie of the unicast suite as received from the station*/
#define UNICAST_SUITE_OFFSET	6
/* the offset in bytes from the beginning of the RSN ie of the auth suite as received from the station*/
#define AUTH_SUITE_OFFSET		12

/* enhanced security definitions */
#define	RSN_UNICAST_SUITE_MAX		2
#define	RSN_AUTH_SUITE_MAX			1

/* cipher suite */
#define	CYPHER_SUITE_WEP		0
#define	CYPHER_SUITE_TKIP		1
#define	CYPHER_SUITE_AES		2

/* auth suite */
#define	AUTH_SUITE_802_1X			0

#define	SUITE_NULL			0xFFFFFFFF

#define TEMPORAL_KEY_LEN	16
#define AES_BLOCK_SIZE		16

/* Management frames */
typedef struct {
	BOOL			wepOn;
	UINT16			*algoNumber;
	UINT16			*transSeqNumber;
	UINT16			*status;
	dot11_Chlg_T	*challenge;
} dot11_auth_T;

typedef struct {
	UINT16			*reason;
} dot11_deAuth_T;

#define FOURX_SUPPORT_CONCAT_MASK 0x1
#define FOURX_SUPPORT_CW_MIN_MASK 0x2
#define FOURX_SUPPORT_ACK_EMUL_MASK 0x4

typedef struct {
	UINT16				*capabilities;
	UINT16				*listenInterval;
	dot11_SSID_T		*SSID;
	dot11_supRates_T	*supportedRates;
	dot11_rsne_T		*rsnIe;
	dot11_extSupRates_T	*extSupportedRates;
	UINT8				fourXSupport; /* bitfield:  LSB0: concat, LSB1: CWmin, LSB2: AkcEmul */
	BOOL				TIouiSupport;
	BOOL				TIprotectionSupport;
	UINT8				CommonFeatureSet;
	UINT16				ConcatMaxSize;
} dot11_assocReq_T;

typedef struct {
	UINT16				*capabilities;
	UINT16				*status;
	UINT16				*AID;
	dot11_supRates_T	*supportedRates;
} dot11_assocRsp_T, dot11_reAssocRsp_T;

typedef struct {
	UINT16				*capabilities;
	UINT16				*listenInterval;
	char				*currentAP;
	dot11_SSID_T		*SSID;
	dot11_supRates_T	*supportedRates;
	dot11_rsne_T		*rsnIe;
	dot11_extSupRates_T	*extSupportedRates;
	UINT8				fourXSupport; /* bitfield:  LSB0: concat, LSB1: CWmin, LSB2: AkcEmul */
	BOOL				TIouiSupport;
	BOOL				TIprotectionSupport;
	UINT8				CommonFeatureSet;
	UINT16				ConcatMaxSize;
} dot11_reAssocReq_T;

typedef struct {
	UINT16			*reason;
} dot11_disAssoc_T;

typedef struct
{
	dot11_mgmtHeader_T	hdr;
	UINT8             timeStamp[ 8 ];
	UINT16            beaconInterval;
	UINT16            capabilities;

} dot11_beacon_fixedFields_T;

typedef struct 
{
  dot11_mgmtHeader_T	hdr;
  UINT8             timeStamp[ 8 ];
  UINT16            beaconInterval;
  UINT16            capabilities;

  char infoElements[ sizeof( dot11_SSID_T ) +
                     sizeof( dot11_supRates_T ) +
                     sizeof( dot11_dsParams_T ) +
					 sizeof( dot11_ERP_T) +
                     sizeof( dot11_extSupRates_T ) +
                     sizeof( dot11_CountryInfo_T) ];
} dot11_beacon;

typedef struct tDOT11_PROBE_RESPONSE_FRAME {
  
  dot11_mgmtHeader_T	hdr;
  UINT8             timeStamp[ 8 ];
  UINT16            beaconInterval;
  UINT16            capabilities;

  char infoElements[ sizeof( dot11_SSID_T ) + 
                     sizeof( dot11_supRates_T ) +
                     sizeof( dot11_dsParams_T ) +
					 sizeof( dot11_ERP_T) +
                     sizeof( dot11_extSupRates_T ) +
                     sizeof( dot11_TIcap_T ) +
                     sizeof( dot11_rsne_T)  +
                     sizeof( dot11_CountryInfo_T) ];
} dot11_probeRsp;


typedef struct tDOT11_PROBE_REQUEST_FRAME {
    dot11_mgmtHeader_T	hdr;
    dot11_SSID_T		*SSID;
    dot11_supRates_T	*supportedRates;
} dot11_probeReq;

/* In this section are defined min & max values for console inputs, according to the MIB */

#define MIB_OPEN_SYSTEM		1
#define MIB_SHARED_KEY		2

#define MIN_AUTH_RESP_TO_MS		10		/* 10 mili */
#define MAX_AUTH_RESP_TO_MS		5000	/* 5 secs*/

#define MIN_KEY_ID		0		
#define MAX_KEY_ID		3		

#define MIN_GTK_PERIOD	30		

#define MIN_CHANNEL_ID		1		
#define MAX_CHANNEL_ID		14		

#define MAX_A_CHANNEL_ID		200 /* ????????????? */


#define MIN_BEACON_PERIOD		1		
#define MAX_BEACON_PERIOD		65535		

#define MIN_DTIM_PERIOD		1		
#define MAX_DTIM_PERIOD		255		

#define MIN_RTS_THRESHOLD		0		
#define MAX_RTS_THRESHOLD		4096		

#define MIN_SHORT_RETRY_LIMIT	1		
#define MAX_SHORT_RETRY_LIMIT	255		

#define MIN_LONG_RETRY_LIMIT	1		
#define MAX_LONG_RETRY_LIMIT	255		

#define MIN_FRAG_THRESHOLD		256		
#define MAX_FRAG_THRESHOLD		4096		

#define MIN_TRANSMIT_MSDU_LIFETIME		1		
#define MAX_TRANSMIT_MSDU_LIFETIME		0xFFFFFFFF		

#define MIN_RECEIVE_MSDU_LIFETIME		1		
#define MAX_RECEIVE_MSDU_LIFETIME		0xFFFFFFFF		

#define PBCC_ALGO_DISABLED				0
#define PBCC_ALGO_ENABLED				1
#define PBCC_ALGO_LOCAL_STATIONS		2
#define PBCC_ALGO_DYNAMIC				3
#define PBCC_ALGO_ENHANCED_DYNAMIC		4

#define PREAMBLE_MODE_LONG				0
#define PREAMBLE_MODE_SHORT				1
#define PREAMBLE_MODE_LOCAL_STATIONS	2

#define SLOTTIME_MODE_DISABLED			0
#define SLOTTIME_MODE_ENABLED			1
#define SLOTTIME_MODE_LOCAL_STATIONS	2
#define SLOTTIME_MODE_ENHANCED_DYNAMIC	3

#define PROTECTION_MODE_DISABLED		 0
#define PROTECTION_MODE_ENABLED			 1
#define PROTECTION_MODE_LOCAL_STATIONS	 2
#define PROTECTION_MODE_DYNAMIC			 3
#define PROTECTION_MODE_ENHANCED_DYNAMIC 4

#define PROTECTION_TYPE_RTS_CTS			0
#define PROTECTION_TYPE_CTS_TOSELF		1
#define PROTECTION_TYPE_TI				2

#define PROTECTION_TI_DISABLED			0
#define PROTECTION_TI_ENABLED			1

#define ASSOCRESP_RATE_ALL_AP			0
#define ASSOCRESP_RATE_AP_AND_STA		1

#define DOT11_MODE_MIXED				0
#define DOT11_MODE_B_ONLY				1
#define DOT11_MODE_B_ONLY_PLUS			2
#define DOT11_MODE_OFDM_ONLY			3
#define DOT11_MODE_A_ONLY			    4


#define DOT11_CWMIN_DISABLED		0
#define DOT11_CWMIN_ENABLED			1
#define DOT11_CWMIN_DOT11_MODE		2

/* MACROS */
#define INRANGE(x,low,high)    (((x) >= (low)) && ((x) <= (high)))
#define OUTRANGE(x,low,high)   (((x) < (low)) || ((x) > (high)))
#define IS_CHID_OK(chID)       INRANGE((chID), MIN_CHANNEL_ID, MAX_CHANNEL_ID)

/* Bridging definitions */
#define WLAN_HDR_LEN         24
#define WLAN_SNAP_HDR_LEN     8
#define LLC_SNAP_HDR_LEN     20
#define IEEE802_3_HDR_LEN     14 
#define ETHERNET_HDR_LEN     14
#define SNAP_CHANNEL_ID    0xAA

#define RADIO_BAND_2_4GHZ 0        /*2.4 Ghz band */
#define RADIO_BAND_5GHZ 1          /*5 Ghz band */
#define RADIO_BAND_2_4_AND_5GHZ 2   /* 2.4 and 5 Ghz */


/* 4X Definitions */
  
typedef enum
{
    TI_CAP_4X_CONCATENATION = 1,
    TI_CAP_4X_CONT_WINDOW = 2,
    TI_CAP_4X_TCP_ACK_EMUL = 4,
    TI_CAP_TRICK_PACKET_ERP = 5

} WlanTIcap_T;

/* end */

typedef enum 
{
    NOT_4X_MSDU    = -1,
    CONCATENATION  = 1,
    ACK_EMULATION  = 2,
    MANAGMENT_4X   = 3
} Wlan4XType_T;

#define WLAN_4X_CONCAT_HDR_LEN   4



#pragma pack(2)
typedef struct
{
  UINT8  	    type;
  UINT8     	headerLen;
  UINT16		txFlags;
} Wdrv4xHeader_T;
#pragma pack()

#define WLAN_4X_CONCAT_MORE_BIT   0x0001 



#pragma pack(2)
typedef struct
{
    dot11_DataMsduHeader_T msduHeader;
    Wdrv4xHeader_T  header4x;
} dot114xMsdu_T;
#pragma pack()

#pragma pack(2)
typedef struct
{
  UINT16  	    len;
  macAddress_T	SaDa;
} Wdrv4xConcatHeader_T;
#pragma pack()

#ifndef _PUBLIC_CHANNELS_H
typedef enum
{
    /* regulatory domains */
    REG_DOMAIN_FCC     = 0x10,
    REG_DOMAIN_IC      = 0x20,
    REG_DOMAIN_ETSI    = 0x30,
    REG_DOMAIN_SPAIN   = 0x31,
    REG_DOMAIN_FRANCE  = 0x32,
    REG_DOMAIN_MKK     = 0x40,
    REG_DOMAIN_MKK1    = 0x41,
    REG_DOMAIN_US      = 0x50,
    REG_DOMAIN_WORLD   = 0x51,

    DEFAULT_DOMAIN     = REG_DOMAIN_WORLD
}IntegerRegulatoryDomains_e;
#endif /* _PUBLIC_CHANNELS_H */


#define WLAN_MAX_CONCAT_SIZE  4095
 

#define WLAN_HEADER_TYPE_CONCATENATION 0x01
#define WLAN_CONCAT_HEADER_LEN 2

#define WLAN_4X_LEN_FIELD_LEN    2
#define WLAN_SA_FIELD_LEN        6
#define WLAN_SA_FIELD_OFFSET    16
#define WLAN_BSSID_FIELD_OFFSET 10
#define WLAN_DA_FIELD_OFFSET     4
#define WLAN_CONCAT_HDR_LEN     (WLAN_4X_LEN_FIELD_LEN + WLAN_SA_FIELD_LEN)
#define WLAN_CONCAT_HDR_OFFSET  (WLAN_HDR_LEN - WLAN_CONCAT_HDR_LEN)


#define		CIPHER_NONE		 0
#define		CIPHER_WEP		 1
#define		CIPHER_TKIP		 2

#define		MAX_PSK_PWD_LEN				63
#define		MAX_RADIUS_SECRET_LEN		63


#endif   /* _802_11_INFO_DEFS_H */
