/*
 * Router rc control script
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: rc.c,v 1.14 2006/10/08 06:47:31 dom Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <epivers.h>
#include <bcmnvram.h>
#include <mtd.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>
#include <bcmdevs.h>

#include <utils.h>
#include <cyutils.h>
#include <code_pattern.h>
#include <cy_conf.h>
#include <support.h>
#include "pvc.h"
#include "gpio.h"
#include "ledapp.h"
#include "promenv.h"

static void restore_defaults(void);
static void sysinit(void);
static void rc_signal(int sig);

extern struct nvram_tuple router_defaults[];

//extern int vc_fd;
extern int which_conn;
extern int sockfd_used[MAX_CHAN];
extern int vpivci_final_value[MAX_CHAN][2];
extern struct vcc_config *vcc_config_ptr;
extern struct pppoe_config *pppoe_config_ptr;
extern struct pppoa_config *pppoa_config_ptr;
extern struct bridged_config *bridged_config_ptr;
extern struct routed_config *routed_config_ptr;

/* kirby for unip 2004/11.22 */
#ifdef UNNUMBERED_SUPPORT
extern unip_start(void);
#endif

/* by michael */
#ifdef CWMP_SUPPORT
int factory_default = 0;
#endif

int firstboot = 1;
//2004.4.2 junzhao add
int secondenter = 0;
extern unsigned int firstsetlanip;

static void restore_all_pvc_defaults(void)
{
	int i,j,upnp_num=0;
	char upnp_content_name[32],upnp_content[256];

	upnp_num = atoi(nvram_safe_get("upnp_content_num"));
	for (i=0;i<MAX_CHAN;i++)
	{
		sprintf(upnp_content_name,"upnp_content_num_%d",i);
		sprintf(upnp_content,"%d",upnp_num);
		nvram_set(upnp_content_name,upnp_content);
		for (j=0;j<upnp_num;j++)
		{
			sprintf(upnp_content_name,"upnp_content%d",j);
			strcpy(upnp_content,nvram_safe_get(upnp_content_name));
			sprintf(upnp_content_name,"upnp_content_%d_%d",i,j);
			nvram_set(upnp_content_name,upnp_content);
		}	
	}	
}	
static void
restore_defaults(void)
{
	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
		//{ "lan_ifnames", "eth0 eth2 eth3 eth4", 0 },
		{ "lan_ifnames", "eth0", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple *linux_overrides;
	struct nvram_tuple *t, *u;
	int restore_defaults;
	uint boardflags;
#ifdef CWMP_SUPPORT
	char bak_name[256];
#endif


	/* Restore defaults if told to or OS has changed */
	//add by junzhao
	restore_defaults = !nvram_match("restore_defaults", "0") || nvram_invmatch("os_name", "linux") || !nvram_match("restore_single", "1");
	if (restore_defaults)
		cprintf("\nRestoring defaults...\n");

	/* Delete dynamically generated variables */
	/* Choose default lan/wan i/f list. */
	boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
	if (boardflags & BFL_ENETVLAN)
		linux_overrides = vlan;
	else
		linux_overrides = generic;

#ifdef CWMP_SUPPORT
	if (restore_defaults) {
		factory_default = 1;
		nvram_set("lock_list", "");
	}
#endif

	/* Restore defaults */
	for (t = router_defaults; t->name; t++) {
		//if (restore_defaults || !nvram_get(t->name)) {
		if (restore_defaults || !nvram_get(t->name) || nvram_match(t->name, t->name)) {
		//if (restore_defaults || nvram_match(t->name, t->name)) {
			for (u = linux_overrides; u && u->name; u++) {
				if (!strcmp(t->name, u->name)) {
					nvram_set(u->name, u->value);
#ifdef CWMP_SUPPORT
					sprintf(bak_name, "%s_bak", u->name);
					nvram_unset(bak_name);
#endif
					break;
				}
			}
			if (!u || !u->name) {
				nvram_set(t->name, t->value);
#ifdef CWMP_SUPPORT
				sprintf(bak_name, "%s_bak", t->name);
				nvram_unset(bak_name);
#endif
			}
		}
	}

	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", EPI_VERSION_STR);


	
	//wwzh add for wireless factory defaults
	if (restore_defaults)
	{
	//junzhao 2004.4.7
	#ifdef WIRELESS_SUPPORT
		reset_wireless_defaults();
	#endif
		//add by lzh 2004/9/15
		restore_all_pvc_defaults();
	}

	/* Commit values */
	if (restore_defaults) {
		unset_nvram();
		nvram_commit();
		cprintf("done\n");
	}
}

static int noconsole = 0;

static void
sysinit(void)
{
	//char buf[PATH_MAX];
	//struct utsname name;
	//struct stat tmp_stat;
	time_t tm = 0;
	int ret;

	/* /proc */
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

	/* /tmp */
	mount("ramfs", "/tmp", "ramfs", MS_MGC_VAL, NULL);

	/* /var */
	mkdir("/tmp/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/tmp", 0777);


#ifdef MULTILANG_SUPPORT
    {
        /* /tmp/lang */
	mkdir("/tmp/lang", 0777);
//	ret = mount("/dev/mtdblock/4", "/tmp/www", "cramfs", MS_RDONLY, NULL);
	ret = mount("/dev/mtdblock/4", "/tmp/lang", "squashfs", 0, NULL);
	cprintf("ret = %d\n", ret);
	if(ret != 0) 
	{
		rmdir("/tmp/lang");
		cprintf("www -> /www\n");
		perror("mount");
        }
        else
	{//Dom 2005/06/06 fixed link for old asp and new js
		mkdir("/tmp/new_www", 0777);
		mkdir("/tmp/new_www/image", 0777);
		system("ln -sf /www/*.* /tmp/new_www");
		system("ln -sf /www/image/* /tmp/new_www/image");
		system("ln -sf /tmp/lang/* /tmp/new_www");
		system("ln -sf /tmp/lang/*.gif /tmp/new_www/image");
		cprintf("www -> /tmp/new_www\n");
	}
    }
#endif


	/* Setup console */
	if (console_init())
		noconsole = 1;
	klogctl(8, NULL, atoi(nvram_safe_get("console_loglevel")));

	/* Set a sane date */
	stime(&tm);

//	cprintf("now insmod tiatm\n");
#if 1
   //adsl pair selection
   {
	char buf[]="pair_selection\t0";
	int fd=open("/proc/ticfg/env", O_RDWR|O_APPEND);
	if(fd > 0)
	{
		write(fd, buf, sizeof(buf));
		close(fd);
	}
    }
#endif
   	//usleep(50);
	//eval("insmod", "tiatm");

	dprintf("done\n");
}

/* States */
enum {
	RESTART,
	STOP,
	START,
	TIMER,
	USER,
	POLLING,  //junzhao for checkadsl
	IDLE,
};
static int state = START;
static int signalled = -1;

/* Signal handling */
static void
rc_signal(int sig)
{
	//junzho for succession receive signal
	//if (state == IDLE) 
	{	
		if (sig == SIGHUP) {
			printf("signalling RESTART\n");
			signalled = RESTART;
		}
		else if (sig == SIGUSR2) {
			printf("signalling START\n");
			signalled = START;
			secondenter = 0;
		}
		else if (sig == SIGINT) {
			printf("signalling STOP\n");
			signalled = STOP;
		}
		else if (sig == SIGALRM) {
			printf("signalling TIMER\n");
			signalled = TIMER;
		}
		else if (sig == SIGUSR1) {		// Receive from WEB
			printf("signalling USER1\n");
			signalled = USER;
                }
		//add by lijunzhao for checkadsl active or not
		else if (sig == SIGPROF) {		
			printf("signalling POLLING\n");
			signalled = POLLING;
                }
		 
	}
}

/* for adsl finished traing and start wan */
int
do_timer(void)
{
	int i;
	/* kirby for unip */
#ifdef UNNUMBERED_SUPPORT
	FILE* fp;
	char dbuf[100];
	char pppip[20];
	char pppmask[20];	 
	char* token = NULL;
	char* running;
	char d[]="@";
	/**/
#endif

	start_checkadsl("2");
	
	//junzhao 2004.4.14 set signal handler flag!!
	{
	char tmpbuf[] = "1";
	buf_to_file("/var/run/being_busy", tmpbuf);
	}
	
	nvram_set("wan_enable_last", nvram_safe_get("wan_active_connection"));
	if(secondenter) //second enter here
	{
		ram_modify_status(WAN_ENCAP, "None");
		//junzhao 2004.8.26
		do
		{
			if(nvram_match("internet_enable", "0"))
				break;
			which_conn = atoi(nvram_safe_get("wan_connection"));
			start_wan(BOOT);
			printf("waiting to see if we must restart next conn!!!\n");
			for(i=0; i<20000000; i++);
			printf("waiting done!!!\n");
		    {
			int tmp = atoi(nvram_safe_get("wan_active_connection"));
			if(tmp < MAX_CHAN && which_conn < tmp)
			{
				which_conn = tmp;
				//for(i=0; i<10000000; i++);
				start_wan(BOOT);
		        }
		    }
		 }while(0);
	}
	else //first boot enter here
	{
		int i, j;
		
		//junzhao 2004.7.28 for time record
		nvram_set("wanuptime0", "0");
		nvram_set("wanuptime1", "0");
		nvram_set("wanuptime2", "0");
		nvram_set("wanuptime3", "0");
		nvram_set("wanuptime4", "0");
		nvram_set("wanuptime5", "0");
		nvram_set("wanuptime6", "0");
		nvram_set("wanuptime7", "0");
		
		//junzhao 2004.8.25
		do
		{
			if(nvram_match("internet_enable", "0"))
				break;

		#ifdef MULTIPVC_SUPPORT
			for(i=0; i<MAX_CHAN; i++)
		#else
			for(i=0; i<1; i++)
		#endif
			//for(i=0; i<MAX_CHAN; i++)
			{
				//junzhao 2004.8.16
				if(sockfd_used[i] < 0)
				{
					which_conn = i;
					start_wan(BOOT);
					for(j=0; j<20000000; j++);
				}
			}
			which_conn = 0;
			if(firstboot)
				firstboot = 0;   //have pass the first boot
			if(!secondenter)
				secondenter = 1;
		}while(0);
	}

	for(i=0; i<150000000; i++);
	//junzhao 2004.7.10
	//sleep(2);
	defaultgw_table_sort();
	

	/* kirby for unip 2004/11.22*/
	//sleep(4);
#ifdef UNNUMBERED_SUPPORT
	nvram_set("unip_enable","0");
	printf("Start Unnumbered IP Mode\n");
	/* only active pvc set UNIP mode, this will be applied */
        if(unip_start() < 0)
	  	   printf("Unnumbered IP mode failed\n");
#endif

	//junzhao 2004.4.19 finally commit to flash!!!
	nvram_commit();

	//junzhao 2004.7.10
	//sleep(3);
	//defaultgw_table_sort();

	//junzhao 2004.4.14 clear signal handler flag!!
	{
	char tmpbuf[] = "0";
	buf_to_file("/var/run/being_busy", tmpbuf);
	}
	
	//junzhao 2004.4.23 to check httpd!!
	{
	FILE *fp = fopen("/var/run/httpd_nosignal", "r");
	if(fp == NULL)
		eval("killall", "-SIGUSR1", "httpd");
	else
		fclose(fp);
		unlink("/var/run/httpd_nosignal");
	}
	
	//junzhao 2004.7.10
	//sleep(2);
	//defaultgw_table_sort();
	
	//junzhao 2004.4.14 send a signal to httpd to wake it up
	//eval("killall", "-SIGUSR1", "httpd");
	return 0;
}

#ifdef CONFIG_DSL_MODULA_SEL
static void check_dsl_modulation(void)
{
	char env[254];
        char *nv = nvram_safe_get("dsl_modulation");

	if (prom_getenv("modulation", env) && !strcmp(env, nv)) {
		cprintf("DSL modulation is the same, modulation = %s\n", env);
		return ;
	}
	cprintf("DSL modulation ENV = %s, NV = %s\n", env, nv);
	/* Reboot is required if DSL modulation changed. */
	eval("reboot");
}
#endif 

/* Main loop */
static void
main_loop(void)
{
	sigset_t sigset;
	pid_t shell_pid = 0;
	uint boardflags;
	char buff[12];
	//junzhao 2004.4.12 version printing!
	printf("Firmware Version: %s%s\n", CYBERTAN_VERSION, MINOR_VERSION); 
	//cjg:2004-4-27
	
	//junzhao 2004.11.9
	memset(buff, 0, sizeof(buff));
	//junzhao 2005.1.13
	if(nvram_match("boot_type", "adam2"))
		sprintf(buff, "%d.%d.%d", MonitorMajorRev, MonitorMinorRev, TelogyMonitorRev);
	else //pspboot
		sprintf(buff, "%d.%d.%d", MonitorMajorRev_PSBL, MonitorMinorRev_PSBL, TelogyMonitorRev_PSBL);
	nvram_set("boot_ver", buff);

	nvram_set("software_version", CYBERTAN_VERSION);
	nvram_set("add_software_version", SERIAL_NUMBER);//guohong additional software ver
	sprintf(buff, "%d", HARDWARE_TYPE);
	nvram_set("hardware_type", buff);   
	
	//junzhao 2004.9.9
	nvram_set("release_date", RELEASE_DATE);

	/* Basic initialization */
	sysinit();

	/* Setup signal handlers */
	signal_init();
	signal(SIGHUP, rc_signal);
	signal(SIGUSR1, rc_signal);	// Start single service from WEB, by honor
	signal(SIGUSR2, rc_signal);
	signal(SIGINT, rc_signal);
	signal(SIGALRM, rc_signal);
	//junzhao add by check adsl active
	signal(SIGPROF, rc_signal);
	sigemptyset(&sigset);

	/* Give user a chance to run a shell before bringing up the rest of the system */
	
	if (!noconsole)
		run_shell(1, 0);

#ifdef MULTILANG_SUPPORT
#include <flash_layout.h>
	if (prom_getenv("multi_lang", buff) && strcmp(buff, "1") != 0) {
		prom_setenv_va("mtd0", "0x%08x,0x%08x", FL_FS_OFFSET, FL_LANG_OFFSET);
		prom_setenv_va("mtd1", "0x%08x,0x%08x", FL_KERN_OFFSET, FL_LANG_OFFSET);
		prom_setenv_va("mtd2", "0x%08x,0x%08x", FL_BOOT_OFFSET, FL_KERN_OFFSET);
		prom_setenv_va("mtd3", "0x%08x,0x%08x", FL_CONF_OFFSET, FL_END_ADDR);
		prom_setenv_va("mtd4", "0x%08x,0x%08x", FL_LANG_OFFSET, FL_CONF_OFFSET);
		prom_setenv("multi_lang", "1");

		dprintf("Since The MTD size is reset, the system is going to reboot.\n");
		eval("reboot");
	}
#endif
	
#if defined(CONFIG_WL_CALIBRATION_ONBOARD)
	enable_gpioin(GPIO_7);
	if((get_gpioin() & (1 << GPIO_7)) != 0)
		eval("tihsk");
#endif
	/* Convert deprecated variables */
	convert_deprecated();

	/* init nvram , by honor*/
        init_nvram();

#ifdef WRITE_SN_SUPPORT
	/* nvram unicode check, by dom*/
	nvram_unicode_check("get_sn");
#endif

	/* Restore defaults if necessary */
	restore_defaults();

	stop_httpd();
	/* Loop forever */
	
	if(!adsl_led_init())
		printf("adsl_led_init fail\n");

#ifdef CONFIG_DSL_MODULA_SEL
	/* Synchronize the DSL modulation setting before module inserted */
	prom_setenv("modulation", nvram_safe_get("dsl_modulation"));
#else
	/* Remove it for using AUTO mode */
	prom_unsetenv("modulation");
#endif
	prom_setenv("eoc_vendor_id", "4c696e6b73797300");	/* Linksys */
	prom_setenv("eoc_vendor_revision", "0");		/* Rev minus one */

	//2005.1.17 delay insmod tiatm.o
	cprintf("now insmod tiatm\n");
	eval("insmod", "tiatm");
	
	for (;;) {
		switch (state) {
	//add by junzhao for check adsl active
		case POLLING:	
			dprintf("POLLING\n");
			start_polling_check();
                        state = IDLE;
			break;
		case TIMER:
			dprintf("TIMER\n");
			do_timer();
                        state = IDLE;
			break;
		case USER: // Restart single service from WEB of tftpd, by honor
                        cprintf("USER1\n");
                        start_single_service();
                        state = IDLE;
                        break;
		case RESTART:
			cprintf("RESTART\n");
			cprintf("START\n");
			cprintf("hello ap!\n");
			/* Fall through */
		case STOP:
			cprintf("STOP\n");

#ifdef CONFIG_DSL_MODULA_SEL
			check_dsl_modulation();
#endif

			//which_conn = atoi(nvram_safe_get("wan_connection"));
			//junzhao 2004.9.8
		        {
			int cur_conn = atoi(nvram_safe_get("wan_connection"));
			int tmp = atoi(nvram_safe_get("wan_active_connection"));
			if(tmp < MAX_CHAN && cur_conn < tmp)
			{
				which_conn = tmp;
				stop_pvc_wan();
				sleep(1);
			}
			
			which_conn = cur_conn;
			stop_pvc_wan();
			}
			
			//junzhao 2004.7.29 restart firewall
			//stop_firewall();
			//start_firewall();
			
			stop_services();
			
			stop_checkadsl();
			   			
			{
				char buf[]="0";
				int fd=open("/proc/Cybertan/bridge_only_enable", O_RDWR);
				if(fd > 0)
				{
					write(fd, buf, sizeof(buf));
					close(fd);
				}
   			 }
		/*
		        {
			int tmp = atoi(nvram_safe_get("wan_active_connection"));
			if(tmp < MAX_CHAN && which_conn < tmp)
			{
				which_conn = tmp;
				stop_pvc_wan();
			}
			}
		*/
			stop_lan();	
			stop_resetbutton();
		//wwzh add for WAN UPGRADE NOTIFY USER
		#ifdef WAN_UPGRADE
			stop_version_notify();
		#endif
			if (state == STOP) {
				state = IDLE;
				break;
			}
			/* Fall through */
		case START:
			//junzhao
			diag_led_ctl(SYS_READY);
			start_resetbutton();
		//wwzh add for WAN UPGRADE NOTIFY USER
		#ifdef WAN_UPGRADE
			start_version_notify();
		#endif
			start_lan();	

			if(firstboot == 1)
			{
			#ifdef WIRELESS_SUPPORT
				start_wireless();
			#endif
				statusfile_init();
			}else
			{
				//#ifdef CLOUD_SUPPORT
				#if defined(CLOUD_SUPPORT) && defined(WIRELESS_SUPPORT)
					restart_wireless();
				#endif
				
				//add by lzh;
				cprintf("firstsetlanip=%d,lan_detect_dhcp=%s\n",firstsetlanip,nvram_safe_get("lan_detect_dhcp"));
				if (nvram_match("auto_search_ip","1") && nvram_match("lan_detect_dhcp","0") && nvram_match("lan_proto","static"))
				{
					if (firstsetlanip == 1) 
					{	
						cprintf("begin to wait\n");
						sleep(5);
						cprintf("End waiting\n");
					}
				}
			}

			{
				int which, which_tmp;
				char wan_active_connection[3];
				which_tmp = atoi(nvram_safe_get("wan_connection"));
				memset(wan_active_connection, 0, sizeof(wan_active_connection));
				strncpy(wan_active_connection, nvram_safe_get("wan_active_connection"), sizeof(wan_active_connection));
				
				{
				int i;
				char *tmpstr = nvram_safe_get("wan_static_dns");
				char *staticdns;
				
				//junzhao 2004.8.4
				for(i=0; i<MAX_CHAN; i++)
				{
					staticdns = strsep(&tmpstr, " ");
					if(!strcmp(staticdns, "0.0.0.0:0.0.0.0"))
						continue;
					which_conn = i;
					ram_modify_status(WAN_GET_DNS, staticdns);
				}
				}
				
				which = atoi(wan_active_connection);
				if(which == MAX_CHAN)
					which = 0;
				dns_to_resolv(which);
				
				which_conn = which_tmp;
				
				if(!strcmp(wan_active_connection, "8"))
					nvram_set("wan_post_connection", "0");
				else
					nvram_set("wan_post_connection", wan_active_connection);
			}

			start_services();
			start_adslpolling();
		#ifdef WIRELESS_SUPPORT
			start_management_via_wlan();
		#endif
			
		/*
		case TIMER:
			dprintf("TIMER\n");
			do_timer();
		*/
			/* Fall through */
		case IDLE:
			cprintf("IDLE\n");
			state = IDLE;
			/* Wait for user input or state change */
			while (signalled == -1) {
				if (!noconsole && (!shell_pid || kill(shell_pid, 0) != 0))
					shell_pid = run_shell(0, 1);
				else
					sigsuspend(&sigset);
			}
			state = signalled;
			signalled = -1;
			break;
		default:
			dprintf("UNKNOWN\n");
			return;
		}
	}
}


#define CONVERT_NV(old, new) \
	if(nvram_get(old)) \
		nvram_set(new, nvram_safe_get(old)); \

int
init_nvram(void)
{
	/* broadcom 3.11.48.7 change some nvram name */
	CONVERT_NV("d11g_channel","wl_channel");
	CONVERT_NV("d11g_rateset","wl_rateset");
	CONVERT_NV("d11g_rts","wl_rts");
	CONVERT_NV("d11g_bcn","wl_bcn");
	CONVERT_NV("d11g_mode","wl_gmode");
	CONVERT_NV("d11g_rate","wl_rate");
	CONVERT_NV("d11g_frag","wl_frag");
	CONVERT_NV("d11g_dtim","wl_dtim");
//wwzh 2004-03-07-19-25
	nvram_set("wl_firmware_version", "");
	nvram_set("wl0_hwaddr","");	// When disbale wireless, we must get null wireless mac */

	//nvram_set("wan_get_dns","");
	nvram_set("filter_id","1");
#ifdef FIREWALL_LEVEL_SUPPORT
	nvram_set("filter_inb_id","1");
#endif
	nvram_set("wl_active_add_mac","0");
	nvram_set("ddns_change","");
	nvram_set("action_service","");
	nvram_set("ddns_interval","60");

	//if(!nvram_get("wl_macmode1")){
	//	if(nvram_match("wl_macmode","disabled"))
	//		nvram_set("wl_macmode1","disabled");
	//	else
	//		nvram_set("wl_macmode1","other");
	//}
	if(nvram_match("wl_gmode","5"))	// Mixed mode had been changed to 5
		nvram_set("wl_gmode","1");

	if(nvram_match("wl_gmode","4")) // G-ONLY mode had been changed to 2, after 1.40.1 for WiFi G certication
                nvram_set("wl_gmode","2");
	
	nvram_set("wl_country","Worldwide");	// The country always Worldwide

#ifndef AOL_SUPPORT
	nvram_set("aol_block_traffic","0");
	nvram_set("aol_block_traffic1","0");
	nvram_set("aol_block_traffic2","0");
#endif
	nvram_set("ping_ip","");
	nvram_set("ping_times","");
	nvram_set("traceroute_ip","");

	nvram_set("filter_port", "");	// The name have been disbaled from 1.41.3
	nvram_set("AutoDetect_dhcp","1"); //add by lzh;
	return 0;
}

void
unset_nvram(void)
{
#ifndef MPPPOE_SUPPORT
	nvram_unset("ppp_username_1");
	nvram_unset("ppp_passwd_1");
	nvram_unset("ppp_idletime_1");
	nvram_unset("ppp_demand_1");
	nvram_unset("ppp_redialperiod_1");
	nvram_unset("ppp_service_1");
	nvram_unset("mpppoe_enable");
	nvram_unset("mpppoe_dname");
#endif
#ifndef HTTPS_SUPPORT
        nvram_unset("remote_mgt_https");
#endif
#ifndef HSIAB_SUPPORT
        nvram_unset("hsiab_mode");
        nvram_unset("hsiab_provider");
        nvram_unset("hsiab_device_id");
        nvram_unset("hsiab_device_password");
        nvram_unset("hsiab_admin_url");
        nvram_unset("hsiab_registered");
        nvram_unset("hsiab_configured");
        nvram_unset("hsiab_register_ops");
        nvram_unset("hsiab_session_ops");
        nvram_unset("hsiab_config_ops");
        nvram_unset("hsiab_manual_reg_ops");
        nvram_unset("hsiab_proxy_host");
        nvram_unset("hsiab_proxy_port");
        nvram_unset("hsiab_conf_time");
        nvram_unset("hsiab_stats_time");
        nvram_unset("hsiab_session_time");
        nvram_unset("hsiab_sync");
        nvram_unset("hsiab_config");
#endif
	
#ifndef HEARTBEAT_SUPPORT
        nvram_unset("hb_server_ip");
        nvram_unset("hb_server_domain");
#endif
			
#ifndef PARENTAL_CONTROL_SUPPORT
        nvram_unset("artemis_enable");
        nvram_unset("artemis_SVCGLOB");
        nvram_unset("artemis_HB_DB");
        nvram_unset("artemis_provisioned");
#endif

#ifndef WL_STA_SUPPORT
        nvram_unset("wl_ap_ssid");
        nvram_unset("wl_ap_ip");
#endif

#ifndef UPNP_FORWARD_SUPPORT
	{
	int i;
	for (i = 0; i < MAX_NVPARSE; i++) {
		del_forward_port(i);
	}
	}
#endif
	
	
#ifndef SYSLOG_SUPPORT
	{
	struct support_list *s;
	char buf[80];
	for(s = supports ; s < &supports[SUPPORT_COUNT] ; s++) {
		snprintf(buf, sizeof(buf), "LOG_%s", s->name);
		if(nvram_get(buf))
			nvram_unset(buf);
	}
	}
#endif

}

#ifdef WRITE_SN_SUPPORT
void
nvram_unicode_check(char *check_str)
{
	char *nv = nvram_safe_get(check_str);
	char new_nv[strlen(nv)];

	if (!strchr(nv, 0xff)) return;
	strncpy(new_nv, nv, (strchr(nv, 0xff) - nv));
	nvram_set(check_str, new_nv);
	nvram_commit();
}
#endif

int
check_image(void)
{
	int ret = 0;
	eval("insmod","writemac","flag=get_flash"); //Barry adds for set flash_type 2003 09 08
        nvram_set("firmware_version",CYBERTAN_VERSION);
        nvram_set("Intel_firmware_version",INTEL_FLASH_SUPPORT_VERSION_FROM);
        nvram_set("bcm4712_firmware_version",BCM4712_CHIP_SUPPORT_VERSION_FROM);
	eval("rmmod","writemac");

	return ret;
}

//wwzh add for WAN UPGRADE NOTIFY USER
#ifdef WAN_UPGRADE
void notify_httpd(int signo)
{
	int pid;
	char buff[24];
	FILE *file;

	file = fopen("/var/run/httpd.pid", "r");
	if (file == NULL)
		return;
	fgets(buff, sizeof(buff), file);
	pid = atoi(buff);
	kill(pid, SIGUSR2);
	fclose(file);
}
void init_notify_sigaction(void) 
{ 
	struct sigaction act; 
	act.sa_handler = notify_httpd; 
	act.sa_flags = 0; 
	sigemptyset(&act.sa_mask); 
	sigaction(SIGPROF, &act, NULL); 
} 

void init_notify_time() 
{ 
	struct itimerval value; 
	value.it_value.tv_sec = WAN_UPGRADE_NOTIFY_INTERVAL; 
	value.it_value.tv_usec = 0; 
	value.it_interval = value.it_value; 
	setitimer(ITIMER_PROF, &value, NULL); 
} 
int version_notify_main(int argc, char **argv)
{
	/* Run it under background */

	switch (fork()) {
	case -1:
		exit(0);
		break;
	case 0:		
		/* child process */
		(void) setsid();
		break;
	default:	
		/* parent process should just die */
		_exit(0);
	}

	init_notify_sigaction(); 
	init_notify_time();
	while(1) 
		pause();
	return 0;
}

int start_version_notify(void)
{
	int ret = 0;
	
	ret = eval("killall", "-9", "version_notify");
	unlink("/tmp/version_notify");
	symlink("/sbin/rc", "/tmp/version_notify");
	ret = eval("/tmp/version_notify");
	return ret;
}
int stop_version_notify(void)
{
	int ret;
	
	ret = eval("killall", "-9", "version_notify");
	unlink("/tmp/version_notify");
	return ret;
}
#endif


int
main(int argc, char **argv)
{
//	start_wan(0);	
//	eval("/bin/sh", "/var/shtest.sh");
#if 1
	char *base = strrchr(argv[0], '/');
	
	base = base ? base + 1 : argv[0];

	/* init */
	if (strstr(base, "init")) {
		main_loop();
		return 0;
	}

	/* Set TZ for all rc programs */
	setenv("TZ", nvram_safe_get("time_zone"), 1);

	/* ppp */
	if (strstr(base, "ip-up"))
		return ipup_main(argc, argv);
	else if (strstr(base, "ip-down"))
		return ipdown_main(argc, argv);
	else if (strstr(base, "ntpd"))
		return ntp_main(argc, argv);
	else if (strstr(base, "resetbutton"))
                return resetbutton_main(argc, argv);

	/* udhcpc [ deconfig bound renew ] */
	else if (strstr(base, "udhcpc"))
		return udhcpc_main(argc, argv);
	/* erase [device] */
	else if (strstr(base, "erase")) {
		if (argv[1])
			return mtd_erase(argv[1]);
		else {
			fprintf(stderr, "usage: erase [device]\n");
			return EINVAL;
		}
	}
	/* wwzh add for WAN UPGRADE NOTIFY */
#ifdef WAN_UPGRADE
	else if (strstr(base, "version_notify")){
		return version_notify_main(argc, argv);
	}
#endif
	/* write [path] [device] */
	else if (strstr(base, "write")) {
		if (argc >= 3)
			return mtd_write(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: write [path] [device]\n");
			return EINVAL;
		}
	}
	/* rc [stop|start|restart ] */
	else if (strstr(base, "rc")) {
		if (argv[1]) {
			if (strncmp(argv[1], "start", 5) == 0)
				return kill(1, SIGUSR2);
			else if (strncmp(argv[1], "stop", 4) == 0)
				return kill(1, SIGINT);
			else if (strncmp(argv[1], "restart", 7) == 0)
				return kill(1, SIGHUP);
		} else {
			fprintf(stderr, "usage: rc [start|stop|restart]\n");
			return EINVAL;
		}
	}
	//xtt add for diagtool PPPoE&PPPoA 07-08-2004
	else if (strstr(base, "diag_redial"))
		return diag_redial_main(argc, argv);
	//xtt end
	else if (strstr(base, "redial"))
		return redial_main(argc, argv);
	else if (strstr(base, "checkadsl"))
		return checkadsl_main(argc, argv);
	else if (strstr(base, "adslpolling"))
		return adslpolling_main(argc, argv);
	else if (strstr(base, "wanledctrl"))
		return wanledctrl_main(argc, argv);
	else if (strstr(base, "checkrate"))
		return checkrate_main(argc, argv);
	else if (strstr(base, "ddns_success"))
		return ddns_success_main(argc, argv);
	else if (strstr(base, "monitor_ps"))
		return monitor_ps_main(argc, argv);
#ifdef FIREWALL_LEVEL_SUPPORT
	else if (strstr(base, "filterinbsync"))
		return filterinbsync_main();
        /* filterinb [add|del] number */
        else if (strstr(base, "filterinb")) {
                if (argv[1] && argv[2]) {
                        int num=0;
                        if( (num=atoi(argv[2])) > 0 ){
                                if (strcmp(argv[1], "add") == 0)
                                        return filterinb_add(num);
                                else if (strcmp(argv[1], "del") == 0)
                                        return filterinb_del(num);
                        }
                }
                else {
        	        fprintf(stderr, "usage: filterinb [add|del] number\n");
                        return EINVAL;
		}
        }
#endif

#ifdef BACKUP_RESTORE_SUPPORT //zhangbin
	/* restore [path] nvram */
	else if (strstr(base, "restore")) {
		return nvram_restore(argv[1], argv[2]);
	}
#endif

//wwzh
	//////////////////////////////////////////////////////
	//
	else if (strstr(base, "filtersync"))
		return filtersync_main();
        /* filter [add|del] number */
        else if (strstr(base, "filter")) {
                if (argv[1] && argv[2]) {
                        int num=0;
                        if( (num=atoi(argv[2])) > 0 ){
                                if (strcmp(argv[1], "add") == 0)
                                        return filter_add(num);
                                else if (strcmp(argv[1], "del") == 0)
                                        return filter_del(num);
                        }
                }
                else {
        	        fprintf(stderr, "usage: filter [add|del] number\n");
                        return EINVAL;
		}
        }
		
#if 0
	else if (strstr(base, "resetbutton"))
                return resetbutton_main(argc, argv);
        else if (strstr(base, "ntpd"))
                return ntp_main(argc, argv);
//        else if (strstr(base, "ipupdated"))
//                return ipupdate_main(argc, argv);
	else if (strstr(base, "write_boot"))
		return write_boot("/tmp/boot.bin","pmon");
	
#ifdef DEBUG_IPTABLE
        else if (strstr(base, "iptable_range"))
                return range_main(argc, argv);
        else if (strstr(base, "iptable_rule"))
                return rule_main(argc, argv);
#endif
	 else if (strstr(base, "hb_connect"))
                 return hb_connect_main(argc, argv);
         else if (strstr(base, "hb_disconnect"))
                 return hb_disconnect_main(argc, argv);
	
	else if (strstr(base, "gpio"))
		return gpio_main(argc, argv);
	else if (strstr(base, "listen"))
		return listen_main(argc, argv);
	else if (strstr(base, "monitor_ps"))
		return monitor_ps_main(argc, argv);
	else if (strstr(base, "ddns_success"))
		return ddns_success_main(argc, argv);

#endif //wwzh								
	return EINVAL;
#endif 
}
