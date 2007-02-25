/**********************************************************************************************************************

  FILENAME:       public_security_keys.h

  DESCRIPTION:    Host-shared definitions of security key interface


   Copyright (c) 2002 Texas Instruments Inc. 
   All Rights Reserved This program is the confidential and proprietary 
   product of Texas Instruments Inc.  Any Unauthorized use, reproduction or 
   transfer of this program is strictly prohibited.


***********************************************************************************************************************/
#ifndef _PUBLIC_SECURITY_KEYS_H
#define _PUBLIC_SECURITY_KEYS_H

#define	MICHAEL_KEY_LEN		8
#define	MIC_LEN				8
#define TKIP_ENCRYPTION_KEY_LEN		16

#ifndef MAX_KEY_SIZE
#define MAX_KEY_SIZE (32)
#endif

#ifndef MAX_NUM_WEP_DEFAULT_KEY
#define MAX_NUM_WEP_DEFAULT_KEY (4)
#endif

#define WPA_IV_SIZE 4

#define ENTRY_INDEX_IGNORE 0
#define IGNORE_KEY_ID 0

#define MIC_RX_KEY_ENTRY_INDEX        1
#define MIC_TX_KEY_ENTRY_INDEX        2
#define MIC_TX_RX_KEY_ENTRY_INDEX     3

typedef enum
{
  KEY_ADD_OR_REPLACE = 1,
  KEY_REMOVE_V       = 2
}IntegerKeyAction_e;

typedef UINT16 KeyAction_e; // Demiurg 

typedef enum
{
  NO_KEY_V          =  0,
  KEY_SIZE_WEP_64   =  5,
  KEY_SIZE_WEP_128  = 13,
  KEY_SIZE_WEP_256  = 29,
  KEY_SIZE_TKIP     = MAX_KEY_SIZE
}IntegerKeySize_e;                         /* WEP keysizes reflect 3 bytes appended from IV */

typedef UINT8 KeySize_e;

typedef enum
{
    KEY_WEP_DEFAULT   = 0,
    KEY_WEP_ADDR      = 1,
    KEY_TKIP_GROUP    = 2,
    KEY_TKIP_PAIRWISE = 3,
    KEY_AES_GROUP     = 4,
    KEY_AES_PAIRWISE  = 5,
    KEY_LEAP_UNICAST  = 6,
    KEY_LEAP_GROUP    = 7,
    KEY_MICHAEL_GROUP     = 8,
    KEY_MICHAEL_PAIRWISE  = 9,
    
    MAX_KEY_TYPE = 0xFF
}IntegerKeyType_e;


typedef UINT8 KeyType_e;

#pragma pack(1)
typedef struct
{
  UINT8    	 MacAddr[6];
  KeyAction_e	 Action;
  UINT16   	 Reserved;
  KeySize_e	 KeySize;
  KeyType_e	 KeyType;
  UINT8	 	 EntryIndex;
  UINT8	 	 KeyId;
  UINT8   	 RxSeqCount[6];
  UINT8		 Key[MAX_KEY_SIZE];
} AcxCmd_SetKey_T;
#pragma pack()


#endif
