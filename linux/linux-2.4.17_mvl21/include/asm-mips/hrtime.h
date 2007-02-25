/*
 *
 * File: include/asm-mips/hrtime.h
 * Copyright (C) 1999 by the University of Kansas Center for Research, Inc.  
 * Copyright (C) 2001 by MontaVista Software.
 *
 * This software was developed by the Information and
 * Telecommunication Technology Center (ITTC) at the University of
 * Kansas.  Partial funding for this project was provided by Sprint. This
 * software may be used and distributed according to the terms of the GNU
 * Public License, incorporated herein by reference.  Neither ITTC nor
 * Sprint accept any liability whatsoever for this product.
 *
 * This project was developed under the direction of Dr. Douglas Niehaus.
 * 
 * Authors: Balaji S., Raghavan Menon
 *	    Furquan Ansari, Jason Keimig, Apurva Sheth
 *
 * Please send bug-reports/suggestions/comments to utime@ittc.ukans.edu
 * 
 * Further details about this project can be obtained at
 *    http://hegel.ittc.ukans.edu/projects/utime/ 
 *    or in the file Documentation/utime.txt
 */
/*
 * This code purloined from the utime progect for high res timers.
 * Principle modifier George Anzinger george@mvista.com
 */
#ifndef _MIPS_HRTIME_H
#define _MIPS_HRTIME_H
#ifdef __KERNEL__

#include <linux/config.h>	/* for CONFIG_APM etc... */
#include <asm/types.h>		/* for u16s */
#include <asm/io.h>
#include <asm/asm.h>
#include <asm/delay.h>
#if 0
#define IF_ALL_PERIODIC(a) a
#else
#define IF_ALL_PERIODIC(a)
#endif



/*
 * The high-res-timers option is set up to self configure with different 
 * platforms.  It is up to the platform to provide certian macros which
 * override the default macros defined in system without (or with disabled)
 * high-res-timers.
 *
 * To do high-res-timers at some fundamental level the timer interrupt must
 * be seperated from the time keeping tick.  A tick can still be generated
 * by the timer interrupt, but it may be surrounded by non-tick interrupts.
 * It is up to the platform to determine if a particular interrupt is a tick,
 * and up to the timer code (in timer.c) to determine what time events have
 * expired.
 *
 * Macros:
 * update_jiffies()  This macro is to compute the new value of jiffie and 
 *                   sub_jiffie.  If high-res-timers are not available it
 *                   may be assumed that this macro will be called once
 *                   every 1/HZ and so should reduce to:
 *
 * 	(*(u32 *)&jiffies_32)++;
 *
 * sub_jiffie, in this case will always be zero, and need not be addressed.
 * It is assumed that the sub_jiffie is in platform defined units and runs
 * from 0 to a value which represents 1/HZ on that platform.  (See conversion
 * macro requirements below.)
 * If high-res-timers are available, this macro will be called each timer
 * interrupt which may be more often than 1/HZ.  It is up to the code to 
 * determine if a new jiffie has just started and pass this info to:
 *
 * new_jiffie() which should return true if the last call to update_jiffie()
 *              moved the jiffie count (as apposed to just the sub_jiffie).
 *              For systems without high-res-timers the kernel will predefine
 *              this to be 0 which will allow the compiler to optimize the code
 *              for this case.
 *
 * schedule_next_int(jiffie_f,sub_jiffie_v,always) is a macro that the platform should 
 *                                 provide that will set up the timer interrupt 
 *                                 hardware to interrupt at the absolute time
 *                                 defined by jiffie_f,sub_jiffie_v where the 
 *                                 units are 1/HZ and the platform defined 
 *                                 sub_jiffie unit.  This function must 
 *                                 determine the actual current time and the 
 *                                 requested offset and act accordingly.  A 
 *                                 sub_jiffie_v value of -1 should be 
 *                                 understood to mean the next even jiffie 
 *                                 regardless of the jiffie_f value.  If 
 *                                 the current jiffie is not jiffie_f, it 
 *                                 may be assumed that the requested time 
 *                                 has passed and an immeadiate interrupt 
 *                                 should be taken.  If high-res-timers are 
 *                                 not available, this macro should evaluate 
 *                                 to nil.  This macro may return 1 if always
 *                                 if false AND the requested time has passed.
 *                                 "Always" indicates that an interrupt is required
 *                                 even if the time has already passed.
 */


/*
 * no of usecs less than which events cannot be scheduled
 */
#define TIMER_DELTA  5
#if 0 /* Defined in config.in */
#define CONFIG_HIGH_RES_RESOLUTION 1000    // nano second resolution 
                                           // we will use for high res.
#endif
#define USEC_PER_JIFFIES  (1000000/HZ)
/*
 * This is really: x*(CLOCK_TICK_RATE+HZ/2)/1000000
 * Note that we can not figure the constant part at
 * compile time because we would loose precision.
 */
extern unsigned long next_intr;
extern rwlock_t xtime_lock;

extern int _schedule_next_int(unsigned long jiffie_f,long sub_jiffie_in, int always);
extern unsigned long _sub_jiffie;
#define sub_jiffie() _sub_jiffie
#define schedule_next_int(a,b,c)  _schedule_next_int(a,b,c)
extern long cycles_per_jiffies;
//extern unsigned long remainder_of_usec;
extern unsigned int volatile latch_reload;
extern unsigned long volatile last_update;
extern int jiffies_intr;
extern int high_res_test_val;

#define update_jiffies() update_jiffies_sub()
#define new_jiffie() jiffies_intr
#define high_res_test() high_res_test_val = -  cycles_per_jiffies;
#define high_res_end_test() high_res_test_val = 0;

#ifdef _INCLUDED_FROM_TIME_C
int jiffies_intr = 0;
int high_res_test_val = 0;
unsigned long volatile last_update;
unsigned long  _sub_jiffie;
 
#endif
/*
 * Now go ahead and include the clock specific file 
 * These asm files have extern inline functions to do a lot of
 * stuff as well as the conversion routines.
 */

#include <asm/hrtime-MCC.h>

extern int jiffies_intr;

// extern volatile unsigned long lost_ticks;
/*
 * We stole this routine from the Utime code, but there it
 * calculated microseconds and here we calculate sub_jiffies
 * which have (in this case) units of TSC count.  (If there
 * is no TSC, see hrtime-M386.h where a different unit
 * is used.  This allows the more expensive math (to get
 * standard units) to be done only when needed.  Also this
 * makes it as easy (and as efficient) to calculate nano
 * as well as micro seconds.
 */
extern unsigned long r4k_cur, r4k_offset;
extern inline void arch_update_jiffies (unsigned long update) 
{
        int _new_jiffie = 0;
        /*
         * update is the delta in sub_jiffies
         */
        //kgdb_ts(update, _sub_jiffie);
        _sub_jiffie += update;
        while (_sub_jiffie > cycles_per_jiffies){
                _sub_jiffie -= cycles_per_jiffies; 
                _new_jiffie++;
        }
	if (_new_jiffie) {
		jiffies_intr += _new_jiffie;
		jiffies_64 += _new_jiffie;
	}
}
#define SC_32_TO_USEC (SC_32(1000000)/ \
                      (unsigned long)CLOCK_TICK_RATE)



/*
 * This routine is always called under the write_lockirq(xtime_lock)
 */
extern inline void update_jiffies_sub(void)
{
	unsigned long cycles_update;

	cycles_update = get_cpuctr();
	arch_update_jiffies(cycles_update);
}

/*
 * quick_update_jiffies_sub returns the sub_jiffie offset of 
 * current time from the "ref_jiff" jiffie value.  We do this
 * with out updating any memory values and thus do not need to
 * take any locks, if we are careful.
 *
 * I don't know how to eliminate the lock in the SMP case, so..
 */
#if defined (CONFIG_SMP) 
static inline void get_rat_jiffies(unsigned long *jiffies_f,long * _sub_jiffie_f,unsigned long *update)
{
	unsigned long flags;

        read_lock_irqsave(&xtime_lock, flags);
        *jiffies_f = jiffies;
        *_sub_jiffie_f = _sub_jiffie;
        *update = quick_get_cpuctr();
        read_unlock_irqrestore(&xtime_lock, flags);
}
#ifndef if_SMP 
#define if_SMP(a) a
#endif
#else
#define if_SMP(a) 
static inline void get_rat_jiffies(unsigned long *jiffies_f,long *_sub_jiffie_f,unsigned long *update)
{
        unsigned long last_update_f;
        do {
                *jiffies_f = jiffies;
                last_update_f = last_update;
                barrier();
                *_sub_jiffie_f = _sub_jiffie;
                *update = quick_get_cpuctr();
                barrier();
        }while (*jiffies_f != jiffies || last_update_f != last_update);
}
#endif /* CONFIG_SMP */

/*
 * If smp, this must be called with the read_lockirq(&xtime_lock) held.
 * No lock is needed if not SMP.
 */

extern inline long quick_update_jiffies_sub(unsigned long ref_jiff)
{
	unsigned long update;
	unsigned long rtn;
        unsigned long jiffies_f;
        long  _sub_jiffie_f;


        get_rat_jiffies( &jiffies_f,&_sub_jiffie_f,&update);

        rtn = _sub_jiffie_f + (unsigned long) update;
        rtn += (jiffies_f - ref_jiff) * cycles_per_jiffies;
        return rtn;

}
#endif				/* __KERNEL__ */
 #endif				/* _MIPS_HRTIME_H */
