#ifndef DDA_WEXT_H
#define DDA_WEXT_H


/* Linux wireless LAN compatibility code */

#include <linux/config.h>
#define CONFIG_NET_RADIO 1

#ifdef CONFIG_NET_RADIO
#include <linux/wireless.h>
#ifdef __KERNEL__
/* a boolean variable that is true when it's a runtime cofiguration*/
#include <asm/uaccess.h>
extern atomic_t after_go;
extern struct configured_params configured_params;
#if WIRELESS_EXT > 12
#include <net/iw_handler.h>
#else
#error Linux wireless extensions versions older than 13 are not supported
#endif /* WIRELESS_EXT > 12 */
#endif /* KERNEL */
#else
#error Linux wireless LAN is not enabled
#endif /* CONFIG_NET_RADIO */

int dda_wext_init(void);

/* utils */

#define LO_BYTE(x) ((x) & 0xff)
#define HI_BYTE(x) (((x) >> 8) & 0xff)

/* wireless extensions staff */
#define DDA_IOCTL_BASIC_PARAM (SIOCIWFIRSTPRIV + 0)
#define DDA_IOCTL_GET_BASIC_PARAM (SIOCIWFIRSTPRIV + 1)
#define DDA_IOCTL_COMPLEX_PARAM (SIOCIWFIRSTPRIV + 2)
#define DDA_IOCTL_GET_COMPLEX_PARAM (SIOCIWFIRSTPRIV + 3)
#define DDA_IOCTL_GO (SIOCIWFIRSTPRIV + 4)

enum {
	TIAP_PARAM_ERANGE = 1,	
	TIAP_PARAM_ERUNTIME,	
	TIAP_PARAM_ELOGICAL,	
	TIAP_PARAM_ERESOURCE, 
	TIAP_PARAM_EHWINIT,
	TIAP_PARAM_EPARAMNOTSET
};


#define TIAP_PARAM_COUNTRY_STRING_LEN                    (3)
#define TIAP_PARAM_MIN_REPLEV                           (-31)
#define TIAP_PARAM_MAX_REPLEV                           (31)
#define TIAP_PARAM_MAX_RADIO_CAL                        (0xF)
#define TIAP_PARAM_MIN_POWER_CONS                       (-50)
#define TIAP_PARAM_MAX_POWER_CONS                       (50)
#define TIAP_PARAM_MIN_TX_POW_LEVEL                     (1)
#define TIAP_PARAM_MAX_TX_POW_LEVEL                     (5)
#define TIAP_PARAM_RTS_TRESHOLD_DEFAULT                 (2347)
#define TIAP_PARAM_RTS_TRESHOLD_MAX                     (4096)
#define TIAP_PARAM_MIN_FRAGMENTATION_TRESHOLD           (256)
#define TIAP_PARAM_MAX_FRAGMENTATION_TRESHOLD           (4096)
#define TIAP_PARAM_FRAG_TRESH_DEFAULT	                (2346)
#define TIAP_PARAM_MAX_DTIM_PERIOD                      (255)
#define TIAP_PARAM_ACCESS_LIST_SIZE	                (64)
#define TIAP_PARAM_NUM_MULTIPLE_SSID                    (16)
#define TIAP_PARAM_MAX_RADIO_PARAM_POW_TBL              \
	TIAP_PARAM_MAX_RADIO_PARAM_POW_RATES * 		\
	TIAP_PARAM_MAX_RADIO_PARAM_POW_LEVELS
#define TIAP_PARAM_MAX_RADIO_PARAM_POW_RATES    	(4)
#define TIAP_PARAM_MAX_RADIO_PARAM_POW_LEVELS    	(5)
#define TIAP_PARAM_MAX_VHCF_CLASSIFIERS                 (2)
#define TIAP_PARAM_RATES_MAX                            (8)
#define TIAP_PARAM_MAX_STATIONS                         (64)
#define TIAP_PARAM_MAX_NUM_OF_RANGES_A                  (8)
#define TIAP_PARAM_MAX_NUM_OF_RANGES_G                  (4)
#define TIAP_PARAM_NUM_TXPOW_LEVELS                     (5)
#define TIAP_PARAM_MAX_AUTH_ALG_SUPPORTED               (2)
#define TIAP_PARAM_MIB_OPEN_SYSTEM                      (1)
#define TIAP_PARAM_MIB_SHARED_KEY                       (2)
#define TIAP_PARAM_MIN_DTIM_PERIOD			(1)
#define TIAP_PARAM_WONE_SINGLE_BAND			(1)
#define TIAP_PARAM_WONE_DUAL_BAND			(2)
#define TIAP_PARAM_RADIO_BAND_2_4GHZ 			(0)
#define TIAP_PARAM_RADIO_BAND_5GHZ 			(1)
#define TIAP_PARAM_RADIO_BAND_2_4_AND_5GHZ 		(2)
#define TIAP_PARAM_CCK_PBCC_RATES_MAX                   (5)
#define TIAP_PARAM_OFDM_RATES_MAX			(8)

#define TIAP_PARAM_PBCC_ALGO_DISABLED                   (0)
#define TIAP_PARAM_PBCC_ALGO_ENABLED                    (1)
#define TIAP_PARAM_PBCC_ALGO_LOCAL_STATIONS           	(2)
#define TIAP_PARAM_PBCC_ALGO_DYNAMIC                    (3)
#define TIAP_PARAM_PBCC_ALGO_ENHANCED_DYNAMIC         	(4)

#define TIAP_PARAM_PREAMBLE_MODE_LONG                   (0)
#define TIAP_PARAM_PREAMBLE_MODE_SHORT                  (1)
#define TIAP_PARAM_PREAMBLE_MODE_LOCAL_STATIONS 	(2)

#define TIAP_PARAM_SLOTTIME_MODE_DISABLED             	(0)
#define TIAP_PARAM_SLOTTIME_MODE_ENABLED               	(1)
#define TIAP_PARAM_SLOTTIME_MODE_LOCAL_STATIONS    	(2)
#define TIAP_PARAM_SLOTTIME_MODE_ENHANCED_DYNAMIC  	(3)

#define TIAP_PARAM_PROTECTION_MODE_DISABLED            	(0)
#define TIAP_PARAM_PROTECTION_MODE_ENABLED             	(1)
#define TIAP_PARAM_PROTECTION_MODE_LOCAL_STATIONS   	(2)
#define TIAP_PARAM_PROTECTION_MODE_DYNAMIC             	(3)
#define TIAP_PARAM_PROTECTION_MODE_ENHANCED_DYNAMIC 	(4)

#define TIAP_PARAM_PROTECTION_TYPE_RTS_CTS             	(0)
#define TIAP_PARAM_PROTECTION_TYPE_CTS_TOSELF           (1)
#define TIAP_PARAM_PROTECTION_TYPE_TI                   (2)

#define TIAP_PARAM_PROTECTION_TI_DISABLED              	(0)
#define TIAP_PARAM_PROTECTION_TI_ENABLED              	(1)

#define TIAP_PARAM_DOT11_CWMIN_DISABLED        		(0)
#define TIAP_PARAM_DOT11_CWMIN_ENABLED                 	(1)
#define TIAP_PARAM_DOT11_CWMIN_DOT11_MODE      		(2)

#define TIAP_PARAM_MIN_KEY_ID              		(0)
#define TIAP_PARAM_MAX_KEY_ID              		(3)

#define TIAP_PARAM_MAX_WD_INTVL				(30)

/* default values */
#define TIAP_PARAM_DEF_A_CHANNEL			(44)
#define TIAP_PARAM_DEF_CHANNEL				(11)
#define TIAP_PARAM_DEF_WONE_BEACON_PERIOD		(200)

enum {
	TIAP_PARAM_RATE_NOT_SUPPORTED,
	TIAP_PARAM_RATE_SUPPORTED,
	TIAP_PARAM_RATE_BASIC_SUPPORTED
};

enum {
	TIAP_PARAM_RETRY_LONG,
	TIAP_PARAM_RETRY_SHORT,
	TIAP_PARAM_LIFETIME_TX,
	TIAP_PARAM_LIFETIME_RX
};


typedef enum {
    TIAP_PARAM_BSS_BR_BLOCK,
    TIAP_PARAM_BSS_BR_ENABLE,
    TIAP_PARAM_BSS_BR_DROP
} tiap_param_bss_br_state_t;	

typedef enum {
    TIAP_PARAM_SECURITY_WPA	 = 1,
    TIAP_PARAM_SECURITY_WEP	 = 2,
    TIAP_PARAM_SECURITY_802_1X	 = 4
} tiap_param_security_type_t;

typedef enum {
  TIAP_PARAM_RD_FCC     = 0x10,
  TIAP_PARAM_RD_IC      = 0x20,
  TIAP_PARAM_RD_ETSI    = 0x30,
  TIAP_PARAM_RD_SPAIN   = 0x31,
  TIAP_PARAM_RD_FRANCE  = 0x32,
  TIAP_PARAM_RD_MKK     = 0x40,
  TIAP_PARAM_RD_MKK1    = 0x41,
  TIAP_PARAM_RD_US      = 0x50,
  TIAP_PARAM_RD_WORLD   = 0x51,
  TIAP_PARAM_RD_DEFAULT = TIAP_PARAM_RD_WORLD
} tiap_param_cur_reg_domain_type_t;




/* state machine states */
typedef enum {
	TIAP_PARAM_SM_STATE_IDLE	= 0,
	TIAP_PARAM_SM_STATE_AUTH_KEY  	= 1,
	TIAP_PARAM_SM_STATE_AUTHENTICATED = 2,
	TIAP_PARAM_SM_STATE_WAIT_FOR_ASS_RES = 3,
	TIAP_PARAM_SM_STATE_ASSOCIATED	= 4,
	TIAP_PARAM_SM_STATE_AUTHORIZED	= 5,
	TIAP_PARAM_SM_NUM_STATES	= 6
} tiap_param_sm_states;


enum {
	/* standard */
	TIAP_PARAM_ESSID=1,
	TIAP_PARAM_RTS,
	TIAP_PARAM_FRAG,
	TIAP_PARAM_TXPOW,
	TIAP_PARAM_RETRY,

	/*basic*/
	TIAP_PARAM_REPLEV,		/* 6 */
	TIAP_PARAM_PRIV_OPTION,
	TIAP_PARAM_BEACON_PERIOD,
	TIAP_PARAM_DTIM_PERIOD,
	TIAP_PARAM_BSS_BRIDGE_STATE,
	TIAP_PARAM_WONE_TIME_MODE,
	TIAP_PARAM_RATE_FALLBACK,
	TIAP_PARAM_WONE_NUM,
	TIAP_PARAM_RADIO_CAL,
	TIAP_PARAM_RADIO_CAL_INT,       
	TIAP_PARAM_ENERGY_DETECT,	/* 16 */
	TIAP_PARAM_HIDDEN_SSID,
	TIAP_PARAM_MULTI_DOMAIN_CAPAB,
	TIAP_PARAM_SPECTRUM_MGMT,
	TIAP_PARAM_LEGACY_STA,
	TIAP_PARAM_LOCAL_POWER_CONS,
	TIAP_PARAM_MAX_TX_POWER_LEVEL,
	TIAP_PARAM_COUNTRY_STR,
	TIAP_PARAM_OPER_STATE,
	TIAP_PARAM_SECURITY_TYPE,
	TIAP_PARAM_RADAR_DETECT_MODE,	/* 26 */
	TIAP_PARAM_AP_BRIDGE_MODE,
	/*complex*/
	TIAP_PARAM_AUTH_TYPE,
	TIAP_PARAM_WD,
	TIAP_PARAM_ACCESS_LIST,
	TIAP_PARAM_4X,			
	TIAP_PARAM_MULTI_SSID,
	TIAP_PARAM_CFG_POWER_TABLE,
	TIAP_PARAM_CHANNEL_RANGE,
	TIAP_PARAM_VHCF,
	TIAP_PARAM_WONE_ELEM,		/* 36 */
	TIAP_PARAM_STA_TBL,
	TIAP_PARAM_PRIV_REG_DOMAIN,
	TIAP_PARAM_CURR_REG_DOMAIN,
	TIAP_PARAM_INTERF_WD_CONFIG,
	TIAP_PARAM_NET_STATS	
};

#define TIAP_PARAM_NUM_PARAMS TIAP_PARAM_NET_STATS

#define dda_wext_rt_ret_err(ret, err)		\
do { 						\
	ret = TIAP_PARAM_ERUNTIME;				\
	printk(KERN_DEBUG 			\
		"%s: run-time parameter %d is not allowed\n", \
		__func__, err);				\
} while(0)

#define dda_wext_rt_ret(ret)			\
do { 						\
	ret = TIAP_PARAM_ERUNTIME;				\
	printk(KERN_DEBUG 			\
		"%s: run-time parameter is not allowed.\n", __func__); \
} while(0)

#define dda_wext_init_ret_err(ret, err)		\
do { 						\
	ret = TIAP_PARAM_ERANGE;				\
	printk(KERN_DEBUG 			\
		"%s: initialization parameter %d is not allowed\n", \
		__func__, err);				\
} while(0)

#define dda_wext_init_ret(ret)			\
do { 						\
	ret = TIAP_PARAM_ERANGE;				\
	printk(KERN_DEBUG 			\
		"%s: initialization parameter is not allowed.\n", __func__); \
} while(0)


#define dda_wext_convert_rates(type, tr, tv)	\
do { 						\
for(i=0,k=0; i<wone->elem.type.len; i++)	\
	if(wone->elem.type.rates[i] == TIAP_PARAM_RATE_SUPPORTED)		\
		init_cfg->wone_info[index].tr.rates[k++] = tv[i];		\
	else if(wone->elem.type.rates[i] == TIAP_PARAM_RATE_BASIC_SUPPORTED)	\
		init_cfg->wone_info[index].tr.rates[k++] = tv[i] | 0x80; 	\
} while(0)


typedef struct {
	__u16 algo;
	__u8 enable;
} tiap_param_auth_algo_t;

typedef enum { 
	TIAP_PARAM_WD_ENABLE, 
	TIAP_PARAM_WD_RXFREE, 
	TIAP_PARAM_WD_BEACONS,
    TIAP_PARAM_WD_TXSTUCK,
    TIAP_PARAM_WD_FCSERR,
	TIAP_PARAM_WD_ACI,
	TIAP_PARAM_WD_NB

} tiap_param_wd_cmd_t;


typedef struct {
	tiap_param_wd_cmd_t cmd;
       	union {
		__u16 val16;
		__u8  val8;
	} value;
} tiap_param_wd_t;


/* Interference Watch Dog state machine configuration enumerator*/
typedef enum {

	TIAP_PARAM_PD_LOW,
	TIAP_PARAM_PD_HIGH,
	TIAP_PARAM_ACI_TIMEOUT,
	TIAP_PARAM_ACI_EVENT_THR,
	TIAP_PARAM_NB_TIMEOUT,
	TIAP_PARAM_ACI_RATE_THR,
	TIAP_PARAM_MEDIUM_BUSY_THR
}tiap_param_interf_cmd_t;

/* Interference Watch Dog state machine configuration parameter type*/
typedef struct {
	tiap_param_interf_cmd_t cmd;
	union {
		__u16 val16;
		__u32 val32;
	} value;
}tiap_param_interference_wd_t;

typedef enum {
	TIAP_PARAM_ACCESS_SET_TYPE,
	TIAP_PARAM_ACCESS_LST,
	TIAP_PARAM_ACCESS_ADD,
	TIAP_PARAM_ACCESS_DEL_BY_INDEX,
	TIAP_PARAM_ACCESS_DEL_BY_ADDR,
	TIAP_PARAM_ACCESS_RESET
} tiap_param_access_list_cmd_t;

typedef struct {
	tiap_param_access_list_cmd_t cmd;
	__s8 enabled;
	__s8 type;
	__s8 index;
	__s8 list[TIAP_PARAM_ACCESS_LIST_SIZE][ETH_ALEN];
} tiap_param_access_list_t;


typedef enum {
	TIAP_PARAM_4X_FEATSET,
	TIAP_PARAM_4X_4X,
	TIAP_PARAM_4X_CAT,
	TIAP_PARAM_4X_PKTBURST,
	TIAP_PARAM_4X_MIXMOD
} tiap_param_4x_cmd_t;


typedef struct {
	tiap_param_4x_cmd_t cmd;
	__u8 value;
} tiap_param_4x_t;


typedef enum {
	TIAP_PARAM_MULTI_SSID_SET,
	TIAP_PARAM_MULTI_SSID_ADD,
	TIAP_PARAM_MULTI_SSID_DEL,
	TIAP_PARAM_MULTI_SSID_RESET
} tiap_param_multi_ssid_cmd_t;


typedef union {
	__s8 ssid[TIAP_PARAM_NUM_MULTIPLE_SSID][IW_ESSID_MAX_SIZE+1];
	__u8 ssid_id;
} tiap_param_multi_ssid_tbl_t;

typedef struct {
	__u8 enabled;
	tiap_param_multi_ssid_cmd_t cmd;
	tiap_param_multi_ssid_tbl_t tbl;
} tiap_param_multi_ssid_t;

enum {
	TIAP_PARAM_PWRTBL = 1
};



typedef struct {
	__u32 type;
	__s8 param[TIAP_PARAM_MAX_RADIO_PARAM_POW_TBL];
} tiap_param_radio_pwrtbl_t;

typedef struct {
	__u32 mask;
	tiap_param_radio_pwrtbl_t tbl;
} tiap_param_radio_t;

typedef struct {
	__u8 start;
	__u8 stop;
	__u8 step;
	__s8 power_limit;
} tiap_param_channel_rang_elem_t;



typedef struct {
	__u8 band;
	tiap_param_cur_reg_domain_type_t type;
} tiap_param_cur_reg_domain_t;


typedef struct {
	__u8 type;
	union {
		tiap_param_channel_rang_elem_t a[TIAP_PARAM_MAX_NUM_OF_RANGES_A];
		tiap_param_channel_rang_elem_t g[TIAP_PARAM_MAX_NUM_OF_RANGES_G];
	} rng;
	__u8 ranges_nr;
} tiap_param_channel_rang;

enum {
	TIAP_PARAM_RANG_A,
	TIAP_PARAM_RANG_G
};

typedef enum {
	vhcf_prot_none,
	vhcf_prot_tcp,
	vhcf_prot_udp
} tiap_param_vhcf_proto_t;

typedef struct {
	tiap_param_vhcf_proto_t proto;
	__u16  dest_port;
	__u32  dest_ip;
} tiap_param_vhcf_class_t;



typedef struct {
	__u8    enable;
	tiap_param_vhcf_class_t classifier[TIAP_PARAM_MAX_VHCF_CLASSIFIERS];
} tiap_param_vhcf_t;


typedef struct {
	__u8 rates[TIAP_PARAM_RATES_MAX];
	__u8 len;
} tiap_param_rates_t;


enum {
	TIAP_PARAM_D11MODE_MIXED,
	TIAP_PARAM_D11MODE_B,
	TIAP_PARAM_D11MODE_B_PLUS,
	TIAP_PARAM_D11MODE_OFDM,
	TIAP_PARAM_D11MODE_A
};

typedef union
{
	tiap_param_rates_t b_legacy;
	tiap_param_rates_t ofdm_legacy;
	tiap_param_rates_t ofdm_extend;
	__u8 ch;
	__u8 band;
	__u8 pbcc_mode;
	__u8 rate_adapt;
	__u8 pream_mode;
	__u8 slot_mode;
	__u8 erp_mode;
	__u8 erp_type;
	__u8 erp_ti;
	__u8 dot11_mode;
	__u8 cwmin_mode;
	__u8 usage_time;
} tiap_param_wone_elem_t;

typedef enum {
	wone_oper_rate,
	wone_oper_add,
	wone_oper_ofdm,
	wone_ch,
	wone_band,
	wone_pbcc,
	wone_rate_adapt,
	wone_pream_mode,
	wone_slot_mode,
	wone_erp_mode,
	wone_erp_type,
	wone_erp_ti,
	wone_dot11_mode,
	wone_cwmin_mode,
	wone_usage_time 
} tiap_param_wone_elem_type_t;

typedef struct {
	__u8 index;
	tiap_param_wone_elem_type_t type;
	tiap_param_wone_elem_t elem;
} tiap_param_wone_t;



typedef struct {
	__s8 hwaddr[ETH_ALEN];
	tiap_param_sm_states state;
	__s8 ssid[IW_ESSID_MAX_SIZE];
	__u8 active_rate;
} tiap_param_statbl_t;



typedef enum {
        txpower_g,
        txpower_a,
} tiap_param_txpower_hr;


typedef struct {
	__u8 hr;
	__s8 txpower[TIAP_PARAM_NUM_TXPOW_LEVELS];
} tiap_param_txpower_t;

enum {
	BAND_B,
	BAND_A,
	BAND_DEFAULT
};

typedef struct {
	__u8 band;
	__u8 domain;
} tiap_param_priv_reg_domain_t;

typedef struct _not_wext_ioctl_buf {
	int buf_len, cmd;
	__u32 param1, param2, param3, param4, param5, param6, param7, param8;
	__u8 mac[ETH_ALEN];
	char * buf;
} generic_ioctl_buf_t;

typedef enum {
	TIAP_GENERIC_IOCTL_GET_TX_INFO,
	TIAP_GENERIC_IOCTL_GET_TX_QUEUE,
	TIAP_GENERIC_IOCTL_TST_CHALLENGE,
	TIAP_GENERIC_IOCTL_TST_FINDSTA,
	TIAP_GENERIC_IOCTL_TST_FIND_INS,
	TIAP_GENERIC_IOCTL_GET_EER,		/*5*/
	TIAP_GENERIC_IOCTL_GET_MAC_REGREAD,
	TIAP_GENERIC_IOCTL_GET_PHY_REGREAD,
	TIAP_GENERIC_IOCTL_TRACE_PRINT,
	TIAP_GENERIC_IOCTL_WLAN_PARAMS,
	TIAP_GENERIC_IOCTL_UPLOAD_FW,
	TIAP_GENERIC_IOCTL_SET_BURST_LIMIT,
	TIAP_GENERIC_IOCTL_GET_BURST_LIMIT,
#if defined(CONFIG_MIPS_AR7VWI)
	TIAP_GENERIC_IOCTL_SET_GPIO_OWNER,
#endif
	#ifdef REPORT_DEBUG
	TIAP_GENERIC_IOCTL_GET_LOGGED_CHARS,
	TIAP_GENERIC_IOCTL_SET_LOGGED_CHARS
	#endif
} generic_ioctl_cmd_t;


typedef struct {
	__u32 rx_frames;
	__u32 tx_frames;
	__u32 rx_multicast_frames;
	__u32 tx_multicast_frames;
} tiap_param_net_bss_stats;

typedef struct {
	__u32 tx_mpdus;
	__u32 tx_msdus;
	__u32 tx_multicast_msdus;
	__u32 tx_failed_msdus;
	__u32 tx_retry_msdus;
	
	__u32 rx_mpdus;
	__u32 rx_msdus;
	__u32 rx_multicast_msdus;
	__u32 rx_fcs_error_mpdus;
	__u32 rx_mic_failure_msdus;
	__u32 rx_decrypt_error_mpdus;
} tiap_param_net_wireless_stats;


typedef struct {
	tiap_param_net_bss_stats bss;
	tiap_param_net_wireless_stats wireless;
} tiap_param_net_stats;

#define GENERIC_IOCTL_BUF_SIZE 20000

/* It's the most easiest way to compute the size of the largest struct.
   Since wireless extensions use the same size for both the real ioctl
   and the sub-ioctls, we must compute the largests size of all structs.
   In the future, if you need to add support for another struct, simply
   add it to the struct below.
 */
union the_largest_struct {
	tiap_param_auth_algo_t		auth_tbl[TIAP_PARAM_MAX_AUTH_ALG_SUPPORTED];
	tiap_param_wd_t			wd;
	tiap_param_interference_wd_t interf_wd_conf;
	tiap_param_access_list_t	access_list;
	tiap_param_4x_t			_4x;
	tiap_param_multi_ssid_t		multi_ssid;
	tiap_param_radio_t		radio;
	tiap_param_channel_rang		ch_rang;
	tiap_param_vhcf_t		vhcf;
	tiap_param_wone_t		wone;
	tiap_param_statbl_t		sta_tbl[TIAP_PARAM_MAX_STATIONS];
	tiap_param_txpower_t		txpow;
	tiap_param_priv_reg_domain_t	priv_regdomain;
	tiap_param_cur_reg_domain_t	cur_regdomain;
	tiap_param_net_stats		net_stats;
};

/* Wireless extensions version 16 does not allow more then IW_PRIV_SIZE_MASK args.
   Each arg has it own size, since union the_largest_struct larger than IW_PRIV_SIZE_MASK,
   we do the following trick, we say that we have sizeof(union the_largest_struct)/sizeof(int)
   number of args, each of which has sizeof(int) (IW_PRIV_TYPE_INT|IW_PRIV_SIZE_FIXED).
 */
#define COMPLEX_PARAM_SIZE IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | (sizeof(union the_largest_struct)/sizeof(int))



struct configured_params{

	__u8				hwaddr;
	
	/*basic data types*/

	/*standard*/
	__u8               		essid;
	__u8             		rts;
	__u8             		frag;
	__u8             		retry_long, retry_short;
	__u8				lifetime_tx, lifetime_rx;
	__u8				txpow_a, txpow_g;
	
	/*private*/
	__u8               		replev;
	__u8               		priv_opt;
	__u8             		beacon_period;
	__u8               		dtim_period;
	__u8			    	bss_bridge_state;
	__u8               		rate_fallback;
	__u8             		radio_cal;
	__u8             		cal_interval;
	__u8               		energy_detect;
	__u8					pd_low_thresh;
	__u8					pd_high_thresh;
	__u8					aci_state_timeout;
	__u8					nb_state_timeout;
	__u8					aci_rate_thresh;
	__u8					aci_event_thresh;
	__u8					medium_busy_thresh;
	__u8             		hidden_ssid;
	__u8               		multi_domain_cap;
	__u8               		spectrum_mgmt;
	__u8               		legacy_sta;
	__u8               		local_pow_cons;
	__u8               		max_txpow_lev;
	__u8 				country_str;
	__u8              		oper_state;
	__u8               		wone_num;
	__u8               		wone_timemod;
	__u32				txop_limit;
	__u8		                radar_detect;
	__u8				bridge_mode;

	/*complex data types*/
	__u8				auth_tbl;
	__u8				wd_en, wd_rxfree, wd_beacons, wd_txstuck, wd_fcserrs, wd_aci, wd_nb;
	__u8				access_list;
	__u8				_4x_fs, _4x_4x, _4x_cat, _4x_pb, _4x_mm;
	__u8				multi_ssid;
	__u8				radio;
	__u8				ch_rang_a, ch_rang_g;
	__u8				vhcf;
	__u8				oper_rate[TIAP_PARAM_WONE_DUAL_BAND], oper_add[TIAP_PARAM_WONE_DUAL_BAND],
					oper_ofdm[TIAP_PARAM_WONE_DUAL_BAND], ch[TIAP_PARAM_WONE_DUAL_BAND],
					band[TIAP_PARAM_WONE_DUAL_BAND], pbcc[TIAP_PARAM_WONE_DUAL_BAND],
					rate_adapt[TIAP_PARAM_WONE_DUAL_BAND], pream_mode[TIAP_PARAM_WONE_DUAL_BAND],
					slot_mode[TIAP_PARAM_WONE_DUAL_BAND], erp_mode[TIAP_PARAM_WONE_DUAL_BAND],
					erp_type[TIAP_PARAM_WONE_DUAL_BAND], erp_ti[TIAP_PARAM_WONE_DUAL_BAND],
					dot11_mode[TIAP_PARAM_WONE_DUAL_BAND], cwmin_mode[TIAP_PARAM_WONE_DUAL_BAND],
					usage_time[TIAP_PARAM_WONE_DUAL_BAND];
	
	__u8				priv_regdomain;
	__u8				cur_regdomain;
	__u8				wpa_mode;
	__u8        fw;
};


#endif

