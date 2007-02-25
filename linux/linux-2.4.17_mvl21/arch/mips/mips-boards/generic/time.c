/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2000-2002 Texas Instruments, Inc.  All rights reserved.
 *
 * Nick Zajerko-McKee, nmckee@telogy.com
 * Copyright (C) 2002 Telogy Networks, Inc All rights reserved
 *
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Setting up the clock on the MIPS boards.
 *
 */

#include <linux/types.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/hardirq.h>
#include <asm/div64.h>
#define _INCLUDED_FROM_TIME_C
#include <linux/config.h>
#include <asm/hrtime.h>

#include <linux/interrupt.h>
#include <linux/mc146818rtc.h>
#include <linux/timex.h>

#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>

#if defined(CONFIG_MIPS_AVALANCHE_IIC)
extern void iic_write_data(unsigned char addr,unsigned char data);

/* AVALANCHE I2C routines */
extern unsigned char iic_read_byte(unsigned int dtype, unsigned int dev, unsigned int adr);
extern unsigned char iic_write_byte(unsigned char addr, unsigned char data);
extern int iic_writeb_addr(unsigned int dtype, unsigned int dev, unsigned int adr, unsigned int dat);
extern void init_iic(unsigned long cpu_freq);
#endif

#if defined(CONFIG_MIPS_AVALANCHE_SOC )
#include <asm/avalanche/avalanche_map.h>

/* AVALANCHE 7-segment display variables */
unsigned long av_heartbeat_count;

#if defined(CONFIG_MIPS_AVALANCHE_7SEG)   /*   P    S    P    L    5    1    t    i    t    a    n*/
static unsigned char seven_segarray[] =    {0x66,0x5b,0x66,0x0e,0x5b,0x03,0x0f,0x01,0x0f,0x3d,0x15};
#endif

/* AVALANCHE Global cpu frequency variable */
unsigned long cpu_freq;
unsigned int mips_counter_frequency = 0;

#else

static long last_rtc_update = 0;
#endif /* CONFIG_MIPS_AVALANCHE_SOC  */

extern volatile unsigned long wall_jiffies;

unsigned long missed_heart_beats = 0;

unsigned long r4k_offset; /* Amount to increment compare reg each time */
unsigned long r4k_cur;    /* What counter should be at next timer irq */
extern rwlock_t xtime_lock;
static unsigned int reschedule_int = 0;

#define ALLINTS (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5)

#if defined(CONFIG_MIPS_ATLAS)
static char display_string[] = "        LINUX ON ATLAS       ";
#endif
#if defined(CONFIG_MIPS_MALTA)
static char display_string[] = "        LINUX ON MALTA       ";
#endif


#if defined(CONFIG_MIPS_AVALANCHE_SOC )
#if defined(CONFIG_MIPS_AVALANCHE_7SEG)
#define MAX_AVALANCHE_DISPLAY_COUNT = sizeof(seven_segarray);
#endif
#else
static unsigned int display_count = 0;
#define MAX_DISPLAY_COUNT (sizeof(display_string) - 8)
static unsigned int timer_tick_count=0;
#endif /* CONFIG_MIPS_AVALANCHE_SOC */
#ifndef CONFIG_HIGH_RES_TIMERS /* TEST */
#define TMR_OFFSET 625000
#define MAX_TICK 800
#else
#define TMR_OFFSET 62500
#define MAX_TICK 8000
#endif

static inline void ack_r4ktimer(unsigned long newval)
{
	write_32bit_cp0_register(CP0_COMPARE, newval);
}
#ifndef CONFIG_HIGH_RES_TIMERS

/* Number of usecs that the last interrupt was delayed */
static int delay_at_last_interrupt;

#endif  /* CONFIG_HIGH_RES_TIMERS */
#ifndef CONFIG_HIGH_RES_TIMERS

/* Number of usecs that the last interrupt was delayed */
static int delay_at_last_interrupt;

#endif  /* CONFIG_HIGH_RES_TIMERS */


#if defined(CONFIG_MIPS_AVALANCHE_SOC )
/* nothing */
#else
/*
 * In order to set the CMOS clock precisely, set_rtc_mmss has to be
 * called 500 ms after the second nowtime has started, because when
 * nowtime is written into the registers of the CMOS clock, it will
 * jump to the next second precisely 500 ms later. Check the Motorola
 * MC146818A or Dallas DS12887 data sheet for details.
 *
 * BUG: This routine does not handle hour overflow properly; it just
 *      sets the minutes. Usually you won't notice until after reboot!
 */
static int set_rtc_mmss(unsigned long nowtime)
{
	int retval = 0;
	int real_seconds, real_minutes, cmos_minutes;
	unsigned char save_control, save_freq_select;

	save_control = CMOS_READ(RTC_CONTROL); /* tell the clock it's being set */
	CMOS_WRITE((save_control|RTC_SET), RTC_CONTROL);

	save_freq_select = CMOS_READ(RTC_FREQ_SELECT); /* stop and reset prescaler */
	CMOS_WRITE((save_freq_select|RTC_DIV_RESET2), RTC_FREQ_SELECT);

	cmos_minutes = CMOS_READ(RTC_MINUTES);

	/*
	 * since we're only adjusting minutes and seconds,
	 * don't interfere with hour overflow. This avoids
	 * messing with unknown time zones but requires your
	 * RTC not to be off by more than 15 minutes
	 */
	real_seconds = nowtime % 60;
	real_minutes = nowtime / 60;
	if (((abs(real_minutes - cmos_minutes) + 15)/30) & 1)
		real_minutes += 30;		/* correct for half hour time zone */
	real_minutes %= 60;

	if (abs(real_minutes - cmos_minutes) < 30) {
		CMOS_WRITE(real_seconds,RTC_SECONDS);
		CMOS_WRITE(real_minutes,RTC_MINUTES);
	} else {
		printk(KERN_WARNING
		       "set_rtc_mmss: can't update from %d to %d\n",
		       cmos_minutes, real_minutes);
 		retval = -1;
	}

	/* The following flags have to be released exactly in this order,
	 * otherwise the DS12887 (popular MC146818A clone with integrated
	 * battery and quartz) will not reset the oscillator and will not
	 * update precisely 500 ms later. You won't find this mentioned in
	 * the Dallas Semiconductor data sheets, but who believes data
	 * sheets anyway ...                           -- Markus Kuhn
	 */
	CMOS_WRITE(save_control, RTC_CONTROL);
	CMOS_WRITE(save_freq_select, RTC_FREQ_SELECT);

	return retval;
}

#endif /* CONFIG_MIPS_AVALANCHE_SOC  */


/*
 * There are a lot of conceptually broken versions of the MIPS timer interrupt
 * handler floating around.  This one is rather different, but the algorithm
 * is provably more robust.
 */
void mips_timer_interrupt(struct pt_regs *regs)
{
	int cpu = smp_processor_id();
	int irq = 7;

	if (r4k_offset == 0)
		goto null;

	irq_enter(cpu, irq);
	do {
		kstat.irqs[cpu][irq]++;
#ifdef CONFIG_HIGH_RES_TIMERS
		reschedule_int = 1;
#endif		
		do_timer(regs);

#ifdef CONFIG_HIGH_RES_TIMERS
        	if (!new_jiffie())
		{
#endif
#if defined (CONFIG_MIPS_EVM3)
            if (xtime.tv_usec < 100000/HZ)
              {
                if(avalanche_heartbeat_count >=  SEVEN_SEG_MAX_POS)
                    avalanche_heartbeat_count = 0;
                iic_write_data(SEVEN_SEGMENT_DISP, seven_segarray[evm3_heartbeat_count]);
                avalanche_heartbeat_count++;
              }
#endif /* CONFIG_MIPS_EVM3 */

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
            /*nothing*/
#else
		/* Historical comment/code:
 		 * RTC time of day s updated approx. every 11
 		 * minutes.  Because of how the numbers work out
 		 * we need to make absolutely sure we do this update
 		 * within 500ms before the * next second starts,
 		 * thus the following code.
 		 */
		read_lock(&xtime_lock);
		if ((time_status & STA_UNSYNC) == 0
		    && xtime.tv_sec > last_rtc_update + 660
		    && xtime.tv_usec >= 500000 - (tick >> 1)
		    && xtime.tv_usec <= 500000 + (tick >> 1)) {
			if (set_rtc_mmss(xtime.tv_sec) == 0)
				last_rtc_update = xtime.tv_sec;
			else
				/* do it again in 60 s */
	    			last_rtc_update = xtime.tv_sec - 600;
		}
		read_unlock(&xtime_lock);

		if ((timer_tick_count++ % HZ) == 0) {
		    mips_display_message(&display_string[display_count++]);
		    if (display_count == MAX_DISPLAY_COUNT)
		        display_count = 0;
		}
#endif /* CONFIG_MIPS_AVALANCHE_SOC  */
#ifdef CONFIG_HIGH_RES_TIMERS
		}
		else
		{
        		jiffies_intr = 0;
		}
		if(reschedule_int)
		{
#endif		
		r4k_cur += r4k_offset;
		ack_r4ktimer(r4k_cur);
#ifdef CONFIG_HIGH_RES_TIMERS		
		}
#endif	
	} while (((unsigned long)read_32bit_cp0_register(CP0_COUNT)
	         - r4k_cur) < 0x7fffffff);
	irq_exit(cpu, irq);

	if (softirq_pending(cpu))
		do_softirq();

	return;

null:
	ack_r4ktimer(0);
}


#ifdef CONFIG_HIGH_RES_TIMERS
/*
 * ALL_PERIODIC mode is used if we MUST support the NMI watchdog.  In this
 * case we must continue to provide interrupts even if they are not serviced.
 * In this mode, we leave the chip in periodic mode programmed to interrupt
 * every jiffie.  This is done by, for short intervals, programming a short
 * time, waiting till it is loaded and then programming the 1/HZ.  The chip
 * will not load the 1/HZ count till the short count expires.  If the last
 * interrupt was programmed to be short, we need to program another short
 * to cover the remaining part of the jiffie and can then just leave the
 * chip alone.
 */
IF_ALL_PERIODIC(static int last_was_long = 0;)
int _schedule_next_int(unsigned long jiffie_f,long sub_jiffie_in, int always)
{
	long sub_jiff_offset = 0; 
	unsigned long r4k_compare;	
   	IF_ALL_PERIODIC( if ((sub_jiffie_in == -1) && last_was_long) return 0);
    /* 
         * First figure where we are in time. 
	 * A note on locking.  We are under the timerlist_lock here.  This
	 * means that interrupts are off already, so don't use irq versions.
	*/
	if_SMP( read_lock(&xtime_lock));
	sub_jiff_offset = quick_update_jiffies_sub(jiffie_f);
    if_SMP( read_unlock(&xtime_lock));

	if ((IF_ALL_PERIODIC( last_was_long =) (sub_jiffie_in == -1 ))) {
              		sub_jiff_offset = cycles_per_jiffies - sub_jiff_offset;
		    }else{
		sub_jiff_offset = sub_jiffie_in - sub_jiff_offset;
	}
	
	/*
	* If time is already passed, just return saying so.
	*/
	if (! always && (sub_jiff_offset <  high_res_test_val)){
		IF_ALL_PERIODIC( last_was_long = 0);
		return 1;
	}
	/* 
	 * This handles the case we're we are in negative offset or
	 * that the offset is much bigger than a jiffie (shouldn't happen)
	 * or for the case that the jiffy offset is very very small
	 */
	if((sub_jiff_offset > r4k_offset) || (sub_jiff_offset < MIPS_GUARD_TIME))
	{
#if 0		
		printk("\nERROR %ld value in schedule_next_int() %lu\n",sub_jiff_offset, (unsigned long)read_32bit_cp0_register(CP0_COUNT));
#endif		
		return 1;
	}
	reschedule_int = 0;
	r4k_compare= (unsigned long)read_32bit_cp0_register(CP0_COUNT);
	r4k_compare += sub_jiff_offset;

	/* 
	 * Handle the case we're we are close to a "normal interrupt" 
	 * by changing it to occur in the future
	 */
	if(r4k_compare > r4k_cur)
	{
		r4k_cur += r4k_offset;
	}
	ack_r4ktimer(r4k_compare);
	return 0;
}

#define DEBUG_HIGH_RES_TIMERS 0
void __init hrtimer_init(void)
{
#if DEBUG_HIGH_RES_TIMERS	
	unsigned long i = 10000;
#endif	
	        /*
	         * The init_hrtimers macro is in the choosen support package
		 * depending on the clock source.
		 */
	        init_hrtimers();
		printk("Initialized High Resolution timers\n");
#if DEBUG_HIGH_RES_TIMERS
		printk("arch_to_usec = %lu\n",arch_to_usec);
		printk("usec_to_arch = %lu\n",usec_to_arch);
		printk("arch_to_nsec = %lu\n",arch_to_nsec);
		printk("nsec_to_arch = %lu\n",nsec_to_arch);
		printk("cycles_per_jiffies = %lu\n",cycles_per_jiffies);
		printk("a->n = %u\n",arch_cycles_to_nsec(i));
		printk("n->a = %u\n",nsec_to_arch_cycles(i));
		printk("a->u = %u\n",arch_cycles_to_usec(i));
		printk("u->a = %u\n",usec_to_arch_cycles(i));
		
	  
#endif		
}
#else
#define hrtimer_init()
#endif /* ! CONFIG_HIGH_RES_TIMERS */ 


/*
 * Figure out the r4k offset, the amount to increment the compare
 * register for each time tick.
 * Use the RTC to calculate offset.
 */
static unsigned long __init cal_r4koff(void)
{
	unsigned long count;

#if defined(CONFIG_MIPS_AVALANCHE_SOC )
        unsigned char sec;
        char *cpu_freq_ptr;

        cpu_freq_ptr = prom_getenv("cpufrequency");
        if(!cpu_freq_ptr)
          {
            cpu_freq = CONFIG_CPU_FREQUENCY_AVALANCHE * 1000000 ;
          }
        else
          {
            cpu_freq = simple_strtol(cpu_freq_ptr,NULL,0);
          }

        sec = 0;
        count = cpu_freq / 2;

#if defined(CONFIG_MIPS_AVALANCHE_IIC)
        init_iic(cpu_freq);
#endif

#else /* !CONFIG_MIPS_AVALANCHE_SOC */
	unsigned int flags;

	__save_and_cli(flags);

	/* Start counter exactly on falling edge of update flag */
	while (CMOS_READ(RTC_REG_A) & RTC_UIP);
	while (!(CMOS_READ(RTC_REG_A) & RTC_UIP));

	/* Start r4k counter. */
	write_32bit_cp0_register(CP0_COUNT, 0);

	/* Read counter exactly on falling edge of update flag */
	while (CMOS_READ(RTC_REG_A) & RTC_UIP);
	while (!(CMOS_READ(RTC_REG_A) & RTC_UIP));

	count = read_32bit_cp0_register(CP0_COUNT);

	/* restore interrupts */
	__restore_flags(flags);

#endif /* CONFIG_MIPS_AVALANCHE_SOC  */
	return (count / HZ);
}

static unsigned long __init get_mips_time(void)
{
	unsigned int year, mon, day, hour, min, sec;

#if 0
        unsigned char clkdat[8];
        int regnum;

        for(regnum=0; regnum < 8; regnum++)
          {
            clkdat[regnum] = iic_read_byte(I2C_RTC,EVM3_RTC_I2C_ADDR,regnum);
          }

        sec   = (clkdat[0] & 0xf);
        sec  += (((clkdat[0] >> 4) & 0x7) * 10);
        min   = (clkdat[1] & 0xf);
        min  += (((clkdat[1] >> 4) & 0x7) * 10);
        hour  = (clkdat[2] & 0xf);
        hour += (((clkdat[2] >> 4) & 0x3) * 10);
        day   = (clkdat[4] & 0xf);
        day  += (((clkdat[4] >> 4) & 0x3) * 10);
        mon   = (clkdat[5] & 0xf);
        mon  += (((clkdat[5] >> 4) & 0x1) * 10);
        year  = (clkdat[6] & 0xf);
        year += (((clkdat[6] >> 4) & 0xf) * 10);

        /* check suraj */
#else 

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
        year=2002;
        mon=9;
        day=8;
        hour=12;
        min=0;
        sec=0;
        return mktime(year, mon, day, hour, min, sec);
#else /* !CONFIG_MIPS_AVALANCHE_SOC */

	unsigned char save_control;

	save_control = CMOS_READ(RTC_CONTROL);

	/* Freeze it. */
	CMOS_WRITE(save_control | RTC_SET, RTC_CONTROL);

	/* Read regs. */
	sec = CMOS_READ(RTC_SECONDS);
	min = CMOS_READ(RTC_MINUTES);
	hour = CMOS_READ(RTC_HOURS);

	if (!(save_control & RTC_24H))
	{
		if ((hour & 0xf) == 0xc)
		        hour &= 0x80;
	        if (hour & 0x80)
		        hour = (hour & 0xf) + 12;
	}
	day = CMOS_READ(RTC_DAY_OF_MONTH);
	mon = CMOS_READ(RTC_MONTH);
	year = CMOS_READ(RTC_YEAR);

	/* Unfreeze clock. */
	CMOS_WRITE(save_control, RTC_CONTROL);

	if ((year += 1900) < 1970)
	        year += 100;

	return mktime(year, mon, day, hour, min, sec);

#endif  /* CONFIG_MIPS_AVALANCHE_SOC */
#endif 
}

void __init time_init(void)
{
        unsigned int est_freq, flags;

#if !defined(CONFIG_MIPS_AVALANCHE_SOC)
        /* Set Data mode - binary. */ 
        CMOS_WRITE(CMOS_READ(RTC_CONTROL) | RTC_DM_BINARY, RTC_CONTROL);
#endif

	printk("calculating r4koff... ");
	r4k_offset = cal_r4koff();
	printk("%08lx(%d)\n", r4k_offset, (int) r4k_offset);

	est_freq = 2*r4k_offset*HZ;
	est_freq += 5000;    /* round */
	est_freq -= est_freq%10000;
	printk("CPU frequency %d.%02d MHz\n", est_freq/1000000,
	       (est_freq%1000000)*100/1000000);
  	hrtimer_init();
	r4k_cur = (read_32bit_cp0_register(CP0_COUNT) + r4k_offset);

	write_32bit_cp0_register(CP0_COMPARE, r4k_cur);
	change_cp0_status(ST0_IM, ALLINTS);

	/* Read time from the RTC chipset. */
	write_lock_irqsave (&xtime_lock, flags);
	xtime.tv_sec = get_mips_time();
	xtime.tv_usec = 0;
	write_unlock_irqrestore(&xtime_lock, flags);
}

/* This is for machines which generate the exact clock. */
#define USECS_PER_JIFFY (1000000/HZ)
#define USECS_PER_JIFFY_FRAC ((u32)((1000000ULL << 32) / HZ))

/* Cycle counter value at the previous timer interrupt.. */
#ifdef CONFIG_HIGH_RES_TIMERS
static unsigned long do_fast_gettimeoffset(void)
{
        long tmp = quick_get_cpuctr();
        long rtn = arch_cycles_to_usec(tmp + sub_jiffie());
        return rtn;
}	
#else
static unsigned int timerhi = 0, timerlo = 0;

/*
 * FIXME: Does playing with the RP bit in c0_status interfere with this code?
 */
static unsigned long do_fast_gettimeoffset(void)
{
	u32 count;
	unsigned long res, tmp;

	/* Last jiffy when do_fast_gettimeoffset() was called. */
	static unsigned long last_jiffies=0;
	unsigned long quotient;

	/*
	 * Cached "1/(clocks per usec)*2^32" value.
	 * It has to be recalculated once each jiffy.
	 */
	static unsigned long cached_quotient=0;

	tmp = jiffies;

	quotient = cached_quotient;

	if (tmp && last_jiffies != tmp) {
		last_jiffies = tmp;
#ifdef CONFIG_CPU_MIPS32
		if (last_jiffies != 0) {
			unsigned long r0;
			do_div64_32(r0, timerhi, timerlo, tmp);
			do_div64_32(quotient, USECS_PER_JIFFY,
				    USECS_PER_JIFFY_FRAC, r0);
			cached_quotient = quotient;
		}
#else
		__asm__(".set\tnoreorder\n\t"
			".set\tnoat\n\t"
			".set\tmips3\n\t"
			"lwu\t%0,%2\n\t"
			"dsll32\t$1,%1,0\n\t"
			"or\t$1,$1,%0\n\t"
			"ddivu\t$0,$1,%3\n\t"
			"mflo\t$1\n\t"
			"dsll32\t%0,%4,0\n\t"
			"nop\n\t"
			"ddivu\t$0,%0,$1\n\t"
			"mflo\t%0\n\t"
			".set\tmips0\n\t"
			".set\tat\n\t"
			".set\treorder"
			:"=&r" (quotient)
			:"r" (timerhi),
			 "m" (timerlo),
			 "r" (tmp),
			 "r" (USECS_PER_JIFFY));
		cached_quotient = quotient;
#endif
	}

	/* Get last timer tick in absolute kernel time */
	count = read_32bit_cp0_register(CP0_COUNT);

	/* .. relative to previous jiffy (32 bits is enough) */
	count -= timerlo;

	__asm__("multu\t%1,%2\n\t"
		"mfhi\t%0"
		:"=r" (res)
		:"r" (count),
		 "r" (quotient));

	/*
 	 * Due to possible jiffies inconsistencies, we need to check
	 * the result so that we'll get a timer that is monotonic.
	 */
	if (res >= USECS_PER_JIFFY)
		res = USECS_PER_JIFFY-1;

	return res;
}
#endif  /* CONFIG_HIGH_RES_TIMERS */

void do_gettimeofday(struct timeval *tv)
{
	unsigned int flags;

	read_lock_irqsave (&xtime_lock, flags);
	*tv = xtime;
	tv->tv_usec += do_fast_gettimeoffset();

	/*
	 * xtime is atomically updated in timer_bh. jiffies - wall_jiffies
	 * is nonzero if the timer bottom half hasnt executed yet.
	 */
	if (jiffies - wall_jiffies)
		tv->tv_usec += USECS_PER_JIFFY;

	read_unlock_irqrestore (&xtime_lock, flags);

	if (tv->tv_usec >= USEC_PER_SEC) {
		tv->tv_usec -= USEC_PER_SEC;
		tv->tv_sec++;
	}
}

void do_settimeofday(struct timeval *tv)
{
	write_lock_irq (&xtime_lock);

	/* This is revolting. We need to set the xtime.tv_usec correctly.
	 * However, the value in this location is is value at the last tick.
	 * Discover what correction gettimeofday would have done, and then
	 * undo it!
	 */
	tv->tv_usec -= do_fast_gettimeoffset();

	if (tv->tv_usec < 0) {
		tv->tv_usec += USEC_PER_SEC;
		tv->tv_sec--;
	}

	xtime = *tv;
	time_adjust = 0;		/* stop active adjtime() */
	time_status |= STA_UNSYNC;
	time_maxerror = NTP_PHASE_LIMIT;
	time_esterror = NTP_PHASE_LIMIT;

	write_unlock_irq (&xtime_lock);
}




