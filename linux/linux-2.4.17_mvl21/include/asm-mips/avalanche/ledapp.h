#ifndef __LED_APP__
#define __LED_APP__

#define CONF_FILE		"/etc/led.conf"
#define LED_PROC_FILE		"/proc/led_mod/led"

#define CONFIG_LED_MODULE

#define MAX_MOD_ID        25
#define MAX_STATE_ID   	  25
#define MAX_LED_ID 	  25

#define MOD_ADSL 1
#define DEF_ADSL_IDLE     1
#define DEF_ADSL_TRAINING 2
#define DEF_ADSL_SYNC     3
#define DEF_ADSL_ACTIVITY 4

#define MOD_WAN  2
#define DEF_WAN_IDLE      1
#define DEF_WAN_NEGOTIATE 2
#define DEF_WAN_SESSION   3

#define MOD_LAN  3
#define DEF_LAN_IDLE		  1
#define DEF_LAN_LINK_UP   2
#define DEF_LAN_ACTIVITY  3

#define MOD_WLAN 4
#define DEF_WLAN_IDLE     1
#define DEF_WLAN_LINK_UP  2
#define DEF_WLAN_ACTIVITY 3

#define MOD_USB  5
#define DEF_USB_IDLE		  1
#define DEF_USB_LINK_UP   2
#define DEF_USB_ACTIVITY  3

#define MOD_ETH  6
#define DEF_ETH_IDLE		  1
#define DEF_ETH_LINK_UP   2
#define DEF_ETH_ACTIVITY  3

typedef struct config_elem{
  unsigned char name;
  unsigned char state;
  unsigned char mode;
  unsigned char led;
            int param;
}config_elem_t;

typedef struct led_reg{
	unsigned int param;
  void (*init)(unsigned long param);
  void (*onfunc)(unsigned long param);
  void (*offfunc)(unsigned long param);
}led_reg_t;

#endif
