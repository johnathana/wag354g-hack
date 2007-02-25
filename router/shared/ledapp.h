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

#define SYS_READY	"2,3"
#define SYS_BOOTING	"2,2"
#define SYS_FAIL	"2,4"

#define PPP_OFF		"3,1"
#define PPP_CON		"3,2"
#define PPP_ACT	"3,3"
#define PPP_FAIL	"3,4"

#define WLAN_OFF	"4,1"
#define WLAN_READY	"4,2"
#define WLAN_ACT	"4,3"


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

int adsl_led_init();
int diag_led_ctl(char *ctrl_led);

#endif
