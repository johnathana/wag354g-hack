#ifndef __DDA_WPAAUTH_IOCTL_H
#define __DDA_WPAAUTH_IOCTL_H

#include "dda_wext_ioctl.h"

#define TIAP_WPA_MAX_RADIUS_SECRET_LEN 63
#define TIAP_WPA_MAX_PSK_PWD_LEN 63
#define TIAP_WPA_MIN_PSK_PWD_LEN 8

typedef enum {
	tiap_wpa_auth_psk,
	tiap_wpa_auth_local,
	tiap_wpa_auth_radius
} tiap_wpa_auth_hr_t;


typedef enum {
	TIAP_WPA_WEPAUTH_OPEN,
	TIAP_WPA_WEPAUTH_SHARED,
	TIAP_WPA_WEPAUTH_BOTH
} tiap_wpa_wepauth_type_t;

#define TIAP_WPA_WEP_KEY_LEN_MAX 	29
#define TIAP_WPA_DEFS_KEY_NAL_LEN 	4
#define TIAP_WPA_WEP_40_KEY_LEN 	5
#define TIAP_WPA_WEP_104_KEY_LEN 	13
#define TIAP_WPA_WEP_232_KEY_LEN 	29


enum {
	TIAP_WPA_PSK_PARAM_NONE,
	TIAP_WPA_PSK_PARAM_HEX,
	TIAP_WPA_PSK_PARAM_ASCII
};

#define TIAP_WPA_MIN_GTK_PERIOD		30

typedef struct {
	__u8 len; /*range: TIAP_WPA_WEP_40_KEY_LEN, TIAP_WPA_WEP_104_KEY_LEN,
                         TIAP_WPA_WEP_232_KEY_LEN default: TIAP_WPA_WEP_40_KEY_LEN*/
	__u8 key[TIAP_WPA_WEP_KEY_LEN_MAX]; /*range: any, default: {1,2,3,4,5}*/
} tiap_wpa_wep_key_t;

#ifndef wpa_cipher_t_defined
#define wpa_cipher_t_defined
/* Pairwise and group cipher suites */ 
typedef enum {
    TIAP_WPA_CIPHER_TKIP    = 0x1,
    TIAP_WPA_CIPHER_WEP_40  = 0x2,
    TIAP_WPA_CIPHER_WEP_104 = 0x4,
    TIAP_WPA_CIPHER_WEP_232 = 0x8,
    TIAP_WPA_CIPHER_WRAP    = 0x16,
    TIAP_WPA_CIPHER_CCMP    = 0x32,
} wpa_cipher_t;
#endif

typedef struct {
	  tiap_param_security_type_t type;
	  __u8 wep_privacy;   /*0=OFF, 1=ON*/
	  tiap_wpa_wepauth_type_t wep_auth_type;
	  tiap_wpa_wep_key_t keys[TIAP_WPA_DEFS_KEY_NAL_LEN];
	  __u8 wep_def_key_id;   /*0-3, default: 0*/
	  __u32 default_cipher;
	  __u32 interval;     /*in seconds, default: 3600 (1 hour)*/
} tiap_wpa_security_t;


typedef struct {
	__u32 serv_ip_addr;
	__u16 port;		/* default: 1812 */
	__s8 secret[TIAP_WPA_MAX_RADIUS_SECRET_LEN];
	__u8 secret_len;
} tiap_wpa_radius_t;

typedef struct {
	__s8 psk_passwd[TIAP_WPA_MAX_PSK_PWD_LEN];
	__u32 pass_phrase_mode;
	__u8 psk_passwd_len;
} tiap_wpa_auth_psk_t;


typedef struct {
	tiap_wpa_auth_psk_t psk;
	tiap_wpa_security_t sec;
	tiap_wpa_radius_t radius;
} tiap_wpa_auth_type_t;



typedef struct {
	tiap_wpa_auth_hr_t hr;
	tiap_wpa_auth_type_t type;
	char dev_name[IFNAMSIZ];
} tiap_wpa_auth_t;

#endif
