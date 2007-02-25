/*
 * UTIME: On-demand Microsecond Resolution Timers
 * ----------------------------------------------
 *
 * File: include/asm-mips/hrtime-MCC.h
 * Copyright (C) 1999 by the University of Kansas Center for Research, Inc.
 * Copyright (C) 2001 by MontaVista Software.
 * Copyright (C) 2002 by Telogy Networks
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
 * This code swiped from the utime project to support high res timers
 * Principle thief George Anzinger george@mvista.com
 *
 * Code based on 586/TSC implementation.
 */
#ifndef _ASM_HRTIME_MCC_H
#define _ASM_HRTIME_MCC_H

#ifdef __KERNEL__
#include <asm/sc_math.h>
#ifdef _INCLUDED_FROM_TIME_C
/*
 * The following four values are not used for machines 
 * without a TSC.  For machines with a TSC they
 * are caculated at boot time. They are used to 
 * calculate "cycles" to jiffies or usec.  Don't get
 * confused into thinking they are simple multiples or
 * divisors, however.  
 */
unsigned long arch_to_usec; 
unsigned long arch_to_nsec;
unsigned long usec_to_arch; 
unsigned long nsec_to_arch;
long cycles_per_jiffies = LATCH;
#endif

/*
 * This define avoids an ugly ifdef in time.c
 */
#define get_cpuctr_from_timer_interrupt()
#define timer_latch_reset(s)

/* NOTE: When trying to port this to other architectures define
 * this to be (void)(1) (ie. #define set_last_timer_cc() (void)(1))
 * otherwise sched.c would give an undefined reference
 */

extern void set_last_timer_cc(void);
extern unsigned long r4k_cur;
extern inline unsigned long get_cpuctr(void)
{
	unsigned long old = last_update;
        last_update =  read_32bit_cp0_register(CP0_COUNT);
	/* Did we wrap around ?*/
	if(last_update < old)
	{
		return ( (0xffffffff - old) + last_update);
	}
	return last_update - old;
}
extern inline unsigned long quick_get_cpuctr(void)
{
	unsigned long value;
        value =  read_32bit_cp0_register(CP0_COUNT);
	/* Did we wrap around ? */
	if(value < last_update)
	{
		return ( (0xffffffff - last_update) + value);
	}
	return value - last_update;
}
#define arch_hrtime_init(x) (x)

extern unsigned long usec_to_arch;
extern unsigned long nsec_to_arch;
extern unsigned long arch_to_usec;
extern unsigned long arch_to_nsec;

#define HR_TIME_SCALE_NSEC 22
#define HR_TIME_SCALE_USEC 14
extern inline int arch_cycles_to_usec(unsigned long update) 
{
	return (mpy_ex32(update ,arch_to_usec));
}
extern inline int arch_cycles_to_nsec(long update)
{
        return (mpy_sc_n(HR_TIME_SCALE_NSEC,  update, arch_to_nsec)+1);
}
/* 
 * And the other way...
 */
extern inline int usec_to_arch_cycles(unsigned long usec)
{
        return (mpy_sc_n(HR_TIME_SCALE_USEC,usec,usec_to_arch));
}
extern inline int nsec_to_arch_cycles(unsigned long nsec)
{
        return (mpy_sc_n(HR_TIME_SCALE_NSEC,nsec,nsec_to_arch));
}
#ifdef _INCLUDED_FROM_TIME_C

#include <asm/io.h>

#ifndef USEC_PER_SEC
#define USEC_PER_SEC 1000000UL
#endif

#define JIFFIES_TO_SEC HZ

/* 
 * This value is the guard time in processor ticks for scheduling the next
 * interrupt.  If this value is too small, you may see "stalls" in handling
 * high-resolution time requests.  If this value is too large, then you may
 * experience "undershoots" in timer requests.  By observation a value of 200
 * seemed reasonable.
 */
#ifndef MIPS_GUARD_TIME
#define MIPS_GUARD_TIME 200
#endif

#if 1
#define init_hrtimers() \
	cycles_per_jiffies = r4k_offset; \
	arch_to_usec = (  (0x80000000UL/ cycles_per_jiffies)* 2 * (USEC_PER_SEC/JIFFIES_TO_SEC)); \
	arch_to_nsec = div_sc_n(HR_TIME_SCALE_NSEC, \
		NSEC_PER_USEC * USEC_PER_SEC, \
		(cycles_per_jiffies * JIFFIES_TO_SEC )); \
	nsec_to_arch = div_sc_n(HR_TIME_SCALE_NSEC, \
		cycles_per_jiffies* HZ, \
		NSEC_PER_USEC*USEC_PER_SEC); \
	usec_to_arch = div_sc_n(HR_TIME_SCALE_USEC, \
		(cycles_per_jiffies * HZ), \
		USEC_PER_SEC); \
		
#endif
#if 0
/* Constants for a 4kc w/ a clock of 125 MHZ */
#define init_hrtimers() \
	        cycles_per_jiffies = r4k_offset; \
	        arch_to_usec = 68719477UL;\
	        arch_to_nsec = 67108863UL;\
	        nsec_to_arch = 262144UL;\
	        usec_to_arch = 1024000UL;\


#endif		
#endif   /* _INCLUDED_FROM_HRTIME_C */
#endif				/* __KERNEL__ */
#endif				/* _ASM_HRTIME-MCC_H */
