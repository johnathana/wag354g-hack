#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/types.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <linux/wireless.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <pty.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <shutils.h>

#define TICFG_BUF 10*1024
#define TICFG_ENV "/proc/ticfg/env"


static int macaddr_get(char *mac_addr)
{
        int fd;
        int count = 0, result = 0;
        unsigned char ticfg_buf[TICFG_BUF];
        char *str, *location;

        memset(ticfg_buf, 0, sizeof(ticfg_buf));
        fd = open(TICFG_ENV, O_RDONLY);
        if(fd < 0)
        {
                printf("can not open the ticfg/env file!\n");
                return -1;
        }
        count = read(fd, ticfg_buf, TICFG_BUF);
        //printf("count = %d\n", count);
        if(count > 0)
        {
                int offset = 0;
                str = ticfg_buf;
                location = strstr(str, "maca");
                if (!location)
                {
                        location = strstr(str, "HWA_0");
                        if (!location) return 0;
			location += strlen("HWA_0") + 1;
                }
		else
			location += strlen("maca") + 1;

                strncpy((char *)mac_addr, (const char *)(location), 18);
                mac_addr[17] = '\0';
		result = 1;
        }
        close(fd);
        return result;
}

int  reset_wireless_defaults(void)
{
	printf("restore wireless default successfully\n");
	return 0;
}
int start_wireless(void)
{
	int mode = 0;
	char hwaddr[20];
	mode = atoi(nvram_get("wl_gmode"));
#if 1 
        if (mode != -1)
        {
                if (macaddr_get(hwaddr) > 0)
                        nvram_set("wl0_hwaddr", hwaddr);
		else nvram_set("wl0_hwaddr", "00:50:F1:12:12:10");
                nvram_set("wl_firmware_version", "5.7.0.4");
        }
        else
        {
//                sleep(2);
//               tiwlan_SetEnable(0);
                nvram_set("wl0_hwaddr", "");
                nvram_set("wl_firmware_version", "");
        }
#ifdef OPTIMIZATION42  //WiFi Mode
        system("echo 1 > /proc/avalanche/optimization42");
        system("echo 1 > /proc/tiwlan/optimization42");
#endif
//        nvram_set("mac_write", "0");
#endif	
	
	eval("wlcfg", "start");
	return 0;
}
int stop_wireless(void)
{
	eval("wlcfg", "stop");
        nvram_set("wl0_hwaddr", "");
        nvram_set("wl_firmware_version", "");
#ifdef OPTIMIZATION42  //WiFi Mode
        system("echo 0 > /proc/avalanche/optimization42");
#endif
	return 0;
}
int restart_wireless(void)
{
	stop_wireless();
	start_wireless();
	return 0;
}
