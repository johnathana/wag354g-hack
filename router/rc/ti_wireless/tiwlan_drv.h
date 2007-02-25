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
**|     Copyright (c) 1998,1999,2003 Texas Instruments Incorporated      |**
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

/****************************************************************************/
/*																		                                    	*/
/*		MODULE:	tiwlan_drv.h										                             	*/
/*		PURPOSE: Kernel mode portion of WLAN AP user mode driver header.	    */
/*    CREATOR: Alexander Sirotkin                                           */
/*             demiurg@ti.com                                               */
/*																		                                     	*/
/****************************************************************************/

#ifndef TIWLAN_DRV_INCLUDE_FILE
#define TIWLAN_DRV_INCLUDE_FILE


#include <linux/if_ether.h>
#include <asm/types.h>
#include <app_driver_api.h>
#include <whalParams.h>
#include <ti_types.h>
//#define OFDM_RATES_MAX 1024
//#define MGMT_ACCESS_LIST_SIZE 1024

#define TIWLAN_DRV_MAJOR 42 // TODO change to 242
#define TIWLAN_DRV_NAME "tiwlan_drv"
#define TIWLAN_DRV_IF_NAME "tiwlan0"
#define TIWLAN_DRV_DEVNAME "/dev/tiwlan"

//#define TIWLAN_CM_SOCKET_PATH "/var/tmp/ap.ctl"
#define TIWLAN_CM_SOCKET_PATH "/var/tmp/cm_logic.ctl"
#define TIWLAN_PTY_NAME_PATH "/var/tmp/tiwlan.cli.pty.name.txt"

#define TIWLAN_DRV_NAME_WIRELESS_PROTO "IEEE 802.11-DS"

#define TIWLAN_DRV_IOCTL_MAGIC 'w'
#define TIWLAN_DRV_HEADER_EXTRA_LENGTH   256

// TODO : where the heck should I place this ?
#define MAC_ADDR_LEN 6

/* implemented by Felix R. */
#if (TIWLAN_APDK_VER == 1130)
#define TIWLAN_ACX111_MEM_SIZE 0x20000
#define TIWLAN_ACX111_REG_SIZE 0x2000
#endif
/* end of implemented by Felix R. */

typedef enum {
	TIWLAN_IOCTL_DATA_U8,
	TIWLAN_IOCTL_DATA_U16,
	TIWLAN_IOCTL_DATA_U32,
	TIWLAN_IOCTL_DATA_CHAR_PTR,
	TIWLAN_IOCTL_DATA_MEM_BASE,
	TIWLAN_IOCTL_DATA_NONE,
	TIWLAN_IOCTL_DATA_PACKET,
	TIWLAN_IOCTL_DATA_WEP_KEYS,
	TIWLAN_IOCTL_DATA_SERVICES,   /*added by Felix R.*/
	TIWLAN_IOCTL_DATA_CACHE,      /*added by Felix R.*/
	TIWLAN_IOCTL_DATA_STATISTICS, /*added by Felix R.*/
	TIWLAN_IOCTL_DATA_AP_RATES,   /*added by Felix R.*/
	TIWLAN_IOCTL_DATA_RATES,    /*added by Felix R.*/
	TIWLAN_IOCTL_DATA_SECURITY  /*added by Igor T.*/
} tiwlan_ioctl_data_t;

#define TIWLAN_MAX_WEP_KEY_LEN 70


typedef struct {
	UINT16 index;
	UINT8 len;
	UINT8 value[TIWLAN_MAX_WEP_KEY_LEN];
} tiwlan_ioctl_wep_keys_t;


/* implemented by Felix R. */

typedef enum {
	TIWLAN_SERVICES_ERROR,
	TIWLAN_SERVICES_DEAUTH,
	TIWLAN_SERVICES_DEASSOC,
	TIWLAN_SERVICES_PREAMBLE_MODE,
	TIWLAN_SERVICES_SLOTTIME_MODE,
	TIWLAN_SERVICES_PBCC_MODE,
	TIWLAN_SERVICES_ERP_MODE,
	TIWLAN_SERVICES_ERP_TYPE
} tiwlan_ioctl_services_opcode_t;

typedef struct {
	unsigned char                   mac_addr[ETH_ALEN];
	UINT8                           value;
	tiwlan_ioctl_services_opcode_t  service;
} tiwlan_ioctl_services_t;

typedef enum {
	TIWLAN_CACHE_INVALIDATE,
	TIWLAN_CACHE_WRITEBACK
}tiwlan_ioctl_cache_opcodes_t;

typedef struct {
	unsigned long	addr;
	__u32 size;
	tiwlan_ioctl_cache_opcodes_t op;
} tiwlan_ioctl_cache_t;


/* see drvDbHash.c for more details */
#define WHAL_SSID_STRING_LEN  32
/*typedef struct tiwlan_ioctl_statbl_t tiwlan_ioctl_statbl_t;
struct tiwlan_ioctl_statbl_t {
	tiwlan_ioctl_statbl_t *nextSta;
	tiwlan_ioctl_statbl_t *prevSta;
	BOOL  free;
	char  macAddr[MAC_ADDR_LEN];
	char  ssid[WHAL_SSID_STRING_LEN];
  int   ssidLength;
	UINT16  index;
};*/
#ifndef MAX_SSID_SIZE
	#define MAX_SSID_SIZE WHAL_SSID_STRING_LEN
#endif

#ifdef __KERNEL__
/* state machine states */
// Demiurg : ugly hack because of include files problem
typedef enum {
	STATE_IDLE			= 0,
	STATE_AUTH_KEY		= 1,
	STATE_AUTHENTICATED = 2,
	STATE_ASSOCIATED	= 3,
	STATE_AUTHORIZED	= 4,
	NUM_STATES			= 5,
} states;
#endif

typedef struct {
	char macAddr[MAC_ADDR_LEN];
	states state;
	UINT8 ssid[MAX_SSID_SIZE];
	UINT8 activeRate;
} tiwlan_ioctl_statable_t;

/* excerted drvDbHash.c */
typedef struct
{
	BOOL concatEn;
	BOOL ackEmulEn;
} station4x_cap_T;


/*#include <public_security_keys.h>

typedef struct
{
	UINT16  listenInterval;
	UINT16  capabilities;
	BOOL  fourXEnable;
	BOOL  fourXActive;        
	BOOL  TIprotectionSupported; 
	BOOL  CommonFeatureSet;
	station4x_cap_T fourX_cap;
	UINT32  ConcatMaxSize;
	BOOL    TIcapEn;
	UINT8 activeRate;     
	UINT16  activeRateAcx;    
	UINT16  currentActiveRateWithFallback;
	UINT8 ratesLen;
	UINT8 rates[DOT11_SUPPORTED_RATES_MAX];
	UINT8 extRatesLen;
	UINT8 extRates[DOT11_EXT_SUPPORTED_RATES_MAX];
	UINT16  bitWiseSupRates; 
	UINT8 apRatesLen;
	UINT8 apRates[DOT11_SUPPORTED_RATES_MAX];
	UINT8 apExtRatesLen;
	UINT8 apExtRates[DOT11_EXT_SUPPORTED_RATES_MAX];
	UINT8 WoneIdentifierIndex;
	UINT32  rxPacketCounter;

	dot11_rsne_T  rsnIe;


	UINT8   cipherSuiteType;
	UINT8       encryptionKeyLen;
#define TIWLAN_WPA_MAX_WEP_KEY_LEN	29
	UINT8       encryptionKey[TIWLAN_WPA_MAX_WEP_KEY_LEN];
	UINT8   txMicKey[MICHAEL_KEY_LEN];
	UINT8   rxMicKey[MICHAEL_KEY_LEN];
	BOOL    dataReceivedFromStation;
	BOOL    generateTxMicFailure;
} station_params_t;*/


#ifdef TI_BENCHMARK_TEST
typedef struct {
	__u32 bench_ptrs[3];
} tiwlan_ioctl_bench_t;
#endif


typedef struct {
	UINT32 mpdu;
	UINT32 msdu;
	UINT32 mcast_msdu;
} tiwlan_ioctl_stat_mmm_t;

typedef struct {
  tiwlan_ioctl_stat_mmm_t mmm;
	UINT32 failed_msdu;
	UINT32  retry_msdu;
} tiwlan_ioctl_stat_tx_t;

typedef struct {
  tiwlan_ioctl_stat_mmm_t mmm;
	UINT32 fcs_error_mpdu;
} tiwlan_ioctl_stat_rx_t;

typedef struct {
	tiwlan_ioctl_stat_rx_t rx;
	tiwlan_ioctl_stat_tx_t tx;
} tiwlan_ioctl_stat_t;


typedef enum {
	TIWLAN_802_11G_LEGACY_IE,
	TIWLAN_802_11G_EXTENDED_IE,
	TIWLAN_802_11A_LEGACY_IE
} tiwlan_ioctl_ap_rates_type;

typedef enum {
	NOT_SUPPORTED,
	SUPPORTED,
	BASIC_SUPPORTED
} tiwlan_ioctl_ap_rates_suptype;

typedef struct {
	tiwlan_ioctl_ap_rates_type type;
	UINT8 rates[OFDM_RATES_MAX];
} tiwlan_ioctl_ap_rates_t;

typedef struct {
	UINT8 supported, basic;
} tiwlan_ioctl_rates_t;

/* end of implemented by Felix R. */



#define TIWLAN_IOCTL_DATA_SIZE 72

typedef enum {
	TIWLAN_CMD_GET_MAC_ADDR,
	TIWLAN_CMD_GET_MAC_ADDR_RES,
	TIWLAN_CMD_SET_MAC_ADDR,

	TIWLAN_CMD_GET_SSID,
	TIWLAN_CMD_GET_SSID_RES,
	TIWLAN_CMD_SET_SSID,

	TIWLAN_CMD_GET_CHANNEL,
	TIWLAN_CMD_GET_CHANNEL_RES,
	TIWLAN_CMD_SET_CHANNEL,

	TIWLAN_CMD_GET_FRAG,
	TIWLAN_CMD_GET_FRAG_RES,
	TIWLAN_CMD_SET_FRAG,

	TIWLAN_CMD_GET_RETRY,
	TIWLAN_CMD_GET_RETRY_RES,
	TIWLAN_CMD_SET_RETRY,

	TIWLAN_CMD_GET_RTS,
	TIWLAN_CMD_GET_RTS_RES,
	TIWLAN_CMD_SET_RTS,

	TIWLAN_CMD_IOCTL_SET_PREAMBLE,
	TIWLAN_CMD_SET_PREAMBLE,
	TIWLAN_CMD_GET_PREAMBLE_RES,
	TIWLAN_CMD_IOCTL_GET_PREAMBLE,
	TIWLAN_CMD_GET_PREAMBLE,

	TIWLAN_CMD_IOCTL_SET_PBCC,
	TIWLAN_CMD_SET_PBCC,
	TIWLAN_CMD_GET_PBCC_RES,
	TIWLAN_CMD_IOCTL_GET_PBCC,
	TIWLAN_CMD_GET_PBCC,

	TIWLAN_CMD_IOCTL_SET_BEACON_PERIOD,
	TIWLAN_CMD_SET_BEACON_PERIOD,
	TIWLAN_CMD_GET_BEACON_PERIOD_RES,
	TIWLAN_CMD_IOCTL_GET_BEACON_PERIOD,
	TIWLAN_CMD_GET_BEACON_PERIOD,

	TIWLAN_CMD_IOCTL_SET_DTIM_PERIOD,
	TIWLAN_CMD_SET_DTIM_PERIOD,
	TIWLAN_CMD_GET_DTIM_PERIOD_RES,
	TIWLAN_CMD_IOCTL_GET_DTIM_PERIOD,
	TIWLAN_CMD_GET_DTIM_PERIOD,

	TIWLAN_CMD_IOCTL_SET_WEP_KEYS,
	TIWLAN_CMD_SET_WEP_KEYS,
	TIWLAN_CMD_GET_WEP_KEYS_RES,
	TIWLAN_CMD_IOCTL_GET_WEP_KEYS,
	TIWLAN_CMD_GET_WEP_KEYS,

	TIWLAN_CMD_IOCTL_SET_WEP_KEY_ID,
	TIWLAN_CMD_SET_WEP_KEY_ID,
	TIWLAN_CMD_GET_WEP_KEY_ID_RES,
	TIWLAN_CMD_IOCTL_GET_WEP_KEY_ID,
	TIWLAN_CMD_GET_WEP_KEY_ID,

	TIWLAN_CMD_IOCTL_SET_MAX_BASIC_RATE,
	TIWLAN_CMD_SET_MAX_BASIC_RATE,
	TIWLAN_CMD_GET_MAX_BASIC_RATE_RES,
	TIWLAN_CMD_IOCTL_GET_MAX_BASIC_RATE,
	TIWLAN_CMD_GET_MAX_BASIC_RATE,

	TIWLAN_CMD_IOCTL_SET_MAX_SUPP_RATE,
	TIWLAN_CMD_SET_MAX_SUPP_RATE,
	TIWLAN_CMD_GET_MAX_SUPP_RATE_RES,
	TIWLAN_CMD_IOCTL_GET_MAX_SUPP_RATE,
	TIWLAN_CMD_GET_MAX_SUPP_RATE,

	TIWLAN_CMD_IOCTL_SET_AUTH_TYPE,
	TIWLAN_CMD_SET_AUTH_TYPE,
	TIWLAN_CMD_GET_AUTH_TYPE_RES,
	TIWLAN_CMD_IOCTL_GET_AUTH_TYPE,
	TIWLAN_CMD_GET_AUTH_TYPE,

	TIWLAN_CMD_SET_RES,
	TIWLAN_CMD_GET_ISR,
	TIWLAN_CMD_SET_PACKET,
	TIWLAN_CMD_GET_PACKET,

	TIWLAN_CMD_SET_USER_READY,
	TIWLAN_CMD_GET_MEM_BASE,
	//	TIWLAN_CMD_GET_MALLOC,
	TIWLAN_CMD_GET_PHYS_TO_VIRT,
	TIWLAN_CMD_GET_VIRT_TO_PHYS,
	TIWLAN_CMD_SET_MUNMAP,

	TIWLAN_CMD_IOCTL_SET_LOAD_CONF,
	TIWLAN_CMD_SET_LOAD_CONF,
	TIWLAN_CMD_GET_SAVE_CONF_RES,
	TIWLAN_CMD_IOCTL_GET_SAVE_CONF,
	TIWLAN_CMD_GET_SAVE_CONF,

	TIWLAN_CMD_IOCTL_SET_ACCESS,
	TIWLAN_CMD_SET_ACCESS,
	TIWLAN_CMD_GET_ACCESS_RES,
	TIWLAN_CMD_IOCTL_GET_ACCESS,
	TIWLAN_CMD_GET_ACCESS,

	/* implemented by Felix R. */
	TIWLAN_CMD_IOCTL_SET_4X,
	TIWLAN_CMD_SET_4X,
	TIWLAN_CMD_IOCTL_GET_4X,
	TIWLAN_CMD_GET_4X,
	TIWLAN_CMD_GET_4X_RES,

	TIWLAN_CMD_IOCTL_SET_SERVICES,
	TIWLAN_CMD_SET_SERVICES,
	TIWLAN_CMD_IOCTL_GET_SERVICES,
	TIWLAN_CMD_GET_SERVICES,
	TIWLAN_CMD_GET_SERVICES_RES,

	TIWLAN_CMD_IOCTL_GET_STATABLE,
	TIWLAN_CMD_GET_STATABLE,
	TIWLAN_CMD_GET_STATABLE_RES,

	TIWLAN_CMD_SET_CACHE,
/*#ifdef TI_BENCHMARK_TEST*/
	TIWLAN_CMD_BENCH_PTRS,
/*#endif*/
/*#ifdef TI_BMTRACE_TEST*/
	TIWLAN_CMD_BMTRACE_PTR,
/*#endif*/
/*#ifdef BATCH_K2U_INTERFACE*/
	TIWLAN_CMD_K2U_CONTROL_PTR,
/*#endif*/

	TIWLAN_CMD_IOCTL_SET_DOT11_MODE,
	TIWLAN_CMD_SET_DOT11_MODE,
	TIWLAN_CMD_IOCTL_GET_DOT11_MODE,
	TIWLAN_CMD_GET_DOT11_MODE,
	TIWLAN_CMD_GET_DOT11_MODE_RES,

	TIWLAN_CMD_IOCTL_GET_STATISTICS,
	TIWLAN_CMD_GET_STATISTICS,
	TIWLAN_CMD_GET_STATISTICS_RES,

	TIWLAN_CMD_IOCTL_SET_AP_RATES,
	TIWLAN_CMD_SET_AP_RATES,
	TIWLAN_CMD_IOCTL_GET_AP_RATES,
	TIWLAN_CMD_GET_AP_RATES,
	TIWLAN_CMD_GET_AP_RATES_RES,

	TIWLAN_CMD_IOCTL_SET_MAX_SUPP_BASIC_RATE,
	TIWLAN_CMD_SET_MAX_SUPP_BASIC_RATE,
	TIWLAN_CMD_GET_MAX_SUPP_BASIC_RATE_RES,
	TIWLAN_CMD_IOCTL_GET_MAX_SUPP_BASIC_RATE,
	TIWLAN_CMD_GET_MAX_SUPP_BASIC_RATE,

	/* end of implemented by Felix R. */

	/* implemented by Igor T. */

	TIWLAN_CMD_IOCTL_SET_SECURITY,
	TIWLAN_CMD_SET_SECURITY,
	TIWLAN_CMD_IOCTL_GET_SECURITY,
	TIWLAN_CMD_GET_SECURITY,
	TIWLAN_CMD_GET_SECURITY_RES,

	TIWLAN_CMD_IOCTL_SET_PSK_HEX,
	TIWLAN_CMD_SET_PSK_HEX,
	TIWLAN_CMD_IOCTL_GET_PSK_HEX,
	TIWLAN_CMD_GET_PSK_HEX,
	TIWLAN_CMD_GET_PSK_HEX_RES,

	TIWLAN_CMD_IOCTL_SET_PSK_PWD,
	TIWLAN_CMD_SET_PSK_PWD,
	TIWLAN_CMD_IOCTL_GET_PSK_PWD,
	TIWLAN_CMD_GET_PSK_PWD,
	TIWLAN_CMD_GET_PSK_PWD_RES,

	TIWLAN_CMD_IOCTL_SET_RADIUS,
	TIWLAN_CMD_SET_RADIUS,
	TIWLAN_CMD_IOCTL_GET_RADIUS,
	TIWLAN_CMD_GET_RADIUS,
	TIWLAN_CMD_GET_RADIUS_RES,

	TIWLAN_CMD_IOCTL_SET_POWER_LEVEL,
	TIWLAN_CMD_SET_POWER_LEVEL,
	TIWLAN_CMD_IOCTL_GET_POWER_LEVEL,
	TIWLAN_CMD_GET_POWER_LEVEL,
	TIWLAN_CMD_GET_POWER_LEVEL_RES,

	TIWLAN_CMD_IOCTL_SET_HIDDEN_SSID,
	TIWLAN_CMD_SET_HIDDEN_SSID,
	TIWLAN_CMD_IOCTL_GET_HIDDEN_SSID,
	TIWLAN_CMD_GET_HIDDEN_SSID,
	TIWLAN_CMD_GET_HIDDEN_SSID_RES,

	TIWLAN_CMD_IOCTL_SET_MULTI_SSID,
	TIWLAN_CMD_SET_MULTI_SSID,
	TIWLAN_CMD_IOCTL_GET_MULTI_SSID,
	TIWLAN_CMD_GET_MULTI_SSID,
	TIWLAN_CMD_GET_MULTI_SSID_RES,

	TIWLAN_CMD_IOCTL_SET_SERVICE_ENABLE,
	TIWLAN_CMD_SET_SERVICE_ENABLE,
	TIWLAN_CMD_IOCTL_GET_SERVICE_ENABLE,
	TIWLAN_CMD_GET_SERVICE_ENABLE,
	TIWLAN_CMD_GET_SERVICE_ENABLE_RES,

	TIWLAN_CMD_VLYNQ_RESET,
	TIWLAN_CMD_VLYNQ_ENABLE,

	TIWLAN_CMD_SET_OPTIMIZATION42,
	
	/* end of implemented by Igor T. */


} tiwlan_ioctl_cmd_opcode_t;

/* IOCTL_SET_SECURITY/IOCTL_GET_SECURITY parameters.
   Implemented by Igor T. based on Felix R. document
 */
typedef enum {
  TIWLAN_SECURITY_NONE,
  TIWLAN_SECURITY_WEP,
  TIWLAN_SECURITY_802_1X,
  TIWLAN_SECURITY_WPA
} tiwlan_security_type_t;

typedef enum {
  TIWLAN_WEPAUTH_OPEN,
  TIWLAN_WEPAUTH_SHARED,
  TIWLAN_WEPAUTH_BOTH
} tiwlan_wepauth_type_t;

typedef struct {
  UINT8 type;          /* 0=none, 1=WEP, 2=802.1x, 3=WPA (tiwlan_security_type_t) */
  UINT8 wpa_mode;      /* 0=802.1x, 1=PSK */
  UINT8 wep_privacy;   /* 0=OFF, 1=ON */
  UINT8 wep_auth_type; /* 0=Open, 1=Shared, 2=Both (tiwlan_wepauth_type_t) */
	UINT32 interval;
} tiwlan_ioctl_security_t;

/* RADIUS configuration
 */
typedef struct {
	UINT32 serv_ip_addr;
	UINT16 port; /* default: 1812 */
	UINT8 secret[63];
} tiwlan_ioctl_radius_t;


//#define MAX_SSID_SIZE 16
typedef enum {
	 TIWLAN_MULTI_SSID_NONE,
	 TIWLAN_MULTI_SSID_ADD,
	 TIWLAN_MULTI_SSID_DELETE
} tiwlan_ioctl_multi_ssid_op_t;

typedef struct {
	UINT8 enabled; /* TRUE/FALSE */
	UINT8 cmd;     /* 1=add, 2=delete (tiwlan_ioctl_multi_ssid_op_t) */
	UINT16 ssid_id;/* 1-based SSID index for ioctl_get and ioctl_set:delete */
	char  ssid[MAX_SSID_SIZE]; /* SSID value for ioctl_set:add */
} tiwlan_ioctl_multi_ssid_t;

/* end of implemented by Igor T. */


#define TIWLAN_CONF_BUF_SIZE 3426 //3162 // 3157 // 3153 // 2350 //2175

#define TIWLAN_IOCTL_CMD_SIZE sizeof(tiwlan_ioctl_cmd_t)

#define TIWLAN_DRV_IOCTL_IOCTL _IOWR(TIWLAN_DRV_IOCTL_MAGIC, 0, tiwlan_ioctl_cmd_t)


typedef enum {
 TIWLAN_ACCESS_SET_TYPE,
 TIWLAN_ACCESS_ADD_ENTRY,
 TIWLAN_ACCESS_DEL_ENTRY_BY_INDEX,
 TIWLAN_ACCESS_DEL_ENTRY_BY_ADDRESS
} tiwlan_ioctl_access_opcode_t;


typedef struct {
	tiwlan_ioctl_access_opcode_t cmd;  /* add, delete_by_index, delete_by_address */
  char enabled;
	char type;
  char index; /* Index for SET: delete_by_index */
	char access_list[MGMT_ACCESS_LIST_SIZE][ETH_ALEN]; /* Note: only access_list[0] is relevant for set request */
} tiwlan_ioctl_access_cmd_t;

typedef struct _tiwlan_ioctl_cmd {
	tiwlan_ioctl_cmd_opcode_t opcode;
	UINT32 data_size;
	tiwlan_ioctl_data_t data_type;
	char data[TIWLAN_IOCTL_DATA_SIZE];
	char * packet;
} tiwlan_ioctl_cmd_t;

typedef struct {
 UINT32 mem, reg, mem_size, reg_size;
} tiwlan_mem_base_t;


/* Added by IT:
   kernel-mode/user-mode handshake structure
*/
#ifdef BATCH_K2U_INTERFACE

#define TX_RING_SIZE 64
#define K2U_MAGIC    (('k'<<24) | ('2'<<16) | ('u'<<8) | 'i')

typedef struct {
	volatile int length;   /* Data length in buffer */
	volatile char *buffer; /* buffer pointer */
  void *msdu;            /* MSDUs */
} tiwlan_k2u_buf_t;

typedef struct {
	/* Common */
	volatile __u32 bpool_v2u;          /* Buffer pool: virtual-K1(phys) offset */
	volatile int in_user_mode;         /* 1=user-mode is aware that there is work to do */

	/* Transmit control */
  volatile int tx_buf_size;          /* tx buffer size */
  volatile int tx_kernel_idx;        /* idx where kernel will put the next packet. Owned by kernel */
	volatile int tx_user_idx;          /* idx where user mode has put the last empty buffer. Owned by user mode */
	tiwlan_k2u_buf_t tx_buf[TX_RING_SIZE];
	volatile int magic;                /* The magic is intentionally in the end */
} tiwlan_k2u_control_t;

#endif /* #ifdef BATCH_K2U_INTERFACE */
/* end of added by IgorT */

#ifdef __KERNEL__

#include <linux/ioctl.h>

#define TIWLAN_DO_IOCTL_TIMEOUT 100
#define	VENDOR_ID_TI		0x104c
#define DEVICE_ID_TI_WLAN	0x8400

typedef struct {
	int magic;
} tiwlan_net_dev_priv_t;

typedef enum {
	TIWLAN_MSG_ERROR,
	TIWLAN_MSG_ERESTARTSYS,
	TIWLAN_MSG_MEM_BASE,
	TIWLAN_MSG_IOCTL,
	TIWLAN_MSG_ISR,
	TIWLAN_MSG_PACKET,
	TIWLAN_MSG_OPTIMIZATION42
}	tiwlan_msg_type_t;


typedef struct {
	tiwlan_msg_type_t msg;
	struct list_head list;
	char * data;
} tiwlan_msg_entry_t;

typedef struct {
	spinlock_t lock;
	//	struct semaphore sem;
	wait_queue_head_t wait_queue;
	struct list_head head;
	struct list_head * first, * last;
	int size, count;
#ifdef TIWLAN_DEBUG_MSG_QUEUE
	atomic_t test;
#endif
} tiwlan_msg_queue_t;

typedef struct {
	struct sk_buff * skb;
	struct list_head head;
}  tiwlan_xmit_queue_t;

typedef struct {
	 struct sk_buff * skb;
	 struct list_head list;
} tiwlan_free_skb_entry_t;

#else

#define TIWLAN_MAX_FILE_NAME 128
#define TIWLAN_CLI_SOCKET_PATH "/var/tmp/cli.fd"
#define TIWLAN_FWVERSION_FILE "/var/tmp/wlan.firmware.version.txt"
#define TIWLAN_CLI_FIFO_PATH "/var/tmp/cli.fifo"
#define TIWLAN_CLI_OUTFILE_PATH "/var/tmp/cli.out"

#ifndef ti_dprintf
#define ti_dprintf(level, msg) do { \
printf msg; \
} while (0)
#endif // ti_dprintf

void wapp_utilsPrintf(unsigned long level, int priority,
											const char *format, ...);

int wlanNVRAMReadCB(unsigned char * pbSrc, unsigned short wLen);
int wlanNVRAMWriteCB(unsigned char * pbSrc, unsigned short wLen);

#endif // __KERNEL__

#endif // TIWLAN_DRV_INCLUDE_FILE
