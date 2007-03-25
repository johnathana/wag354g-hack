#include "../../cy_conf.h"

extern int diag_led(int type, int act);
extern int C_led(int i);
extern int get_single_ip(char *ipaddr, int which);
extern char *get_mac_from_ip(char *ip);
//junzhao
extern unsigned int sar_getstats(char *stat_str);
extern void my_lock(int fd, int type);
extern void my_unlock(int fd);
//extern int file_to_buf_withlock(char *path, char *buf, int len);
//extern int buf_to_file_withlock(char *path, char *buf);
//extern struct dns_lists *get_dns_list(void);
extern struct dns_lists *get_dns_list(int index);
//extern int dns_to_resolv(void);
//extern int dns_to_resolv(int index);
extern char *get_wan_face(void);
extern int check_wan_link(int num);
extern char *get_complete_lan_ip(char *ip);
extern int get_int_len(int num);
extern int file_to_buf(char *path, char *buf, int len);
extern int buf_to_file(char *path, char *buf);
#ifdef HSIAB_SUPPORT
extern char *send_command(char *send_value, char *get_value);
extern struct database * open_DB1(int *c);
#endif
extern pid_t* find_pid_by_name( char* pidName);
extern int find_pid_by_ps(char* pidName);
extern int *find_all_pid_by_ps(char* pidName);
extern char *find_name_by_proc(int pid);
extern int get_ppp_pid(char *file);
extern long convert_ver(char *ver);
extern int check_flash(void);
extern int check_action(void);
extern int check_now_boot(void);
extern int check_hw_type(void);
extern int is_exist(char *filename);

extern void encode(char *buf, int len);
extern void decode(char *buf, int len);
#ifdef NOMAD_SUPPORT
extern int wandown_stopnomad_conn(void);
#endif
#ifdef GATEWAY_BEYOND_SUBNET_SUPPORT
extern int legal_ip_netmask(char *sip, char *smask, char *dip);
#endif

int ct_openlog(const char *ident, int option, int facility, char *log_name);
void ct_syslog(int level, int enable, const char *fmt,...);


enum { DMZ, SESSION, DIAG };

enum { START_LED, STOP_LED };

typedef enum { ACT_IDLE, 
	       ACT_TFTP_UPGRADE, 
	       ACT_WEB_UPGRADE, 
	       ACT_WEBS_UPGRADE, 
	       ACT_SW_RESTORE, 
	       ACT_HW_RESTORE } ACTION;

enum { UNKNOWN_BOOT = -1, PMON_BOOT, CFE_BOOT };

enum { BCM4702_CHIP, BCM4712_CHIP };

enum { FIRST, SECOND };

enum { SYSLOG_LOG=1, SYSLOG_DEBUG, CONSOLE_ONLY, LOG_CONSOLE, DEBUG_CONSOLE };

#define ACTION(cmd)	buf_to_file(ACTION_FILE, cmd)
#define MAX_CHAN 8
struct dns_lists {
        int num_servers;
        char dns_server[2*2*MAX_CHAN][16];
};

#define PPP_PSEUDO_IP	"10.64.64.64"
#define PPP_PSEUDO_NM	"255.255.255.255"
#define PPP_PSEUDO_GW	"10.112.112.112"

#define PING_TMP	"/tmp/ping.log"
#define TRACEROUTE_TMP	"/tmp/traceroute.log"
/* add by kirby 2004/7/2 */
#define  DIAGLOG_TMP     "/tmp/diagnostics.log"
#define MAX_BUF_LEN	254

#define RESOLV_FILE	"/tmp/resolv.conf"
#define HOSTS_FILE	"/tmp/hosts"

#define LOG_FILE	"/var/log/mess"

#define ACTION_FILE	"/tmp/action"


#define split(word, wordlist, next, delim) \
	for (next = wordlist, \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL ; \
	     strlen(word); \
	     next = next ? : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL)

