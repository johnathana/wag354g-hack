/* GPL -- By CyberTAN */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/config.h>
#include <asm/avalanche/generic/pal.h>
#include <linux/timer.h>

unsigned int avalanche_get_vbus_freq(void);
struct timer_list timer;

#define MSEC2TICK(msec)		(msec/10)

/* Restart watchdog every a half of period time */
static void wdtimer_kick(unsigned long msec)
{
    if (PAL_sysWdtimerKick() < 0) {
        printk("Kick watchdog error\n");
	return;
    }
    timer.function = wdtimer_kick;
    timer.data = msec;
    timer.expires = jiffies + MSEC2TICK(msec);
    add_timer(&timer);
}

/* Initialize, enable, and start the watchdog timer */ 
static int wdtimer_init(int msec)
{
    PAL_sysWdtimerInit(IS_OHIO_CHIP() ? 0xA8611F00 : AVALANCHE_WATCHDOG_TIMER_BASE,
		    avalanche_get_vbus_freq()); 
    if (PAL_sysWdtimerSetPeriod(msec) < 0) {
	printk("Can't set watchdog time period\n"); 
	return -1; 
    }
    if (PAL_sysWdtimerCtrl(WDTIMER_CTRL_ENABLE) < 0) {
        printk("Can't enable watchdog\n"); 
	return -1; 
    }
    /* Restart watchdog every a half of period time */
    wdtimer_kick(msec/2);	/* 1-Sec */
    return 0; 
}

int __init wdt_init(void)
{
    return wdtimer_init(2000);	/* 2-Sec */
}

static void __exit wdt_exit(void)
{
    if (PAL_sysWdtimerCtrl(WDTIMER_CTRL_DISABLE) < 0) {
        printk("Can't disable watchdog\n"); 
    }
    del_timer(&timer);
}

module_init(wdt_init);
module_exit(wdt_exit);
