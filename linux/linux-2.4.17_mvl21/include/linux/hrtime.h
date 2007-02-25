#ifndef _HRTIME_H
#define _HRTIME_H


#define DEBUGx
#if defined(DEBUG) && defined(CONFIG_KGDB)
#define IF_DEBUG(a) a
#include <asm/kgdb.h>
#else
#define IF_DEBUG(a)
#endif
#ifndef kgdb_ts
#define kgdb_ts(data0,data1)
#endif

/*
 * This file is the glue to bring in the platform stuff.
 * We make it all depend on the CONFIG option so all archs
 * will work as long as the CONFIG is not set.  Once an 
 * arch defines the CONFIG, it had better have the 
 * asm/hrtime.h file in place.
 */
struct k_clock {
        int  res;                  /* in nano seconds */
        int ( *clock_set)(struct timespec *tp);
        int ( *clock_get)(struct timespec *tp);
        int ( *nsleep)(   int flags, 
                          struct timespec*new_setting,
                          struct itimerspec *old_setting);
        int ( *timer_set)(struct k_itimer *timr, int flags,
                          struct itimerspec *new_setting,
                          struct itimerspec *old_setting);
        int  ( *timer_del)(struct k_itimer *timr);
        void ( *timer_get)(struct k_itimer *timr,
                          struct itimerspec *cur_setting);
};

#include <linux/config.h>
#ifdef CONFIG_HIGH_RES_TIMERS
#include <asm/hrtime.h>
/*
 * The schedule_next_int function is to be defined by the "arch" code
 * when an "arch" is implementing the high-res part of POSIX timers.
 * The actual function will be called with the offset in "arch" (parm 2)
 * defined sub_jiffie units from the reference jiffie boundry (parm 1)to
 * the next required sub_jiffie timer interrupt. This value will be -1
 * if the next timer interrupt should be the next jiffie value.  The
 * "arch" code must determine how far out the interrupt is, based on
 * current jiffie, sub_jiffie time and set up the hardware to interrupt
 * at that time.  It is possible that the time will already have passed,
 * in which case the function should return true (no interrupt is
 * needed), otherwise the return should be 0.  The sub_jiffie interrupt
 * must not call do_timer(), but should just do 'mark_bh(TIMER_BH)' and
 * return. If the requested interrupt is "close" to a jiffie interrupt,
 * the two can be rolled into one by calling do_timer(), however, this
 * will advance "jiffie" so "close" should be in the order of the
 * interrupt overhead time.  Notes to me: Should we define a new
 * timer_bh with out the update_times() call.  The call wastes time, but
 * does not otherwise cause problems. GA
 */
#ifndef schedule_next_int
#define schedule_next_int(s,d) 0
#undef CONFIG_HIGH_RES_TIMERS
#endif  // schedule_next_int
/*
 * The sub_jiffie() macro should return the current time offset from the latest
 * jiffie.  This will be in "arch" defined units and is used to determine if
 * a timer has expired.  Since no sub_expire value will be used if "arch" 
 * has not defined the high-res package, 0 will work well here.
 *
 * In addition, to save time if there is no high-res package (or it is not
 * configured), we define the sub expression for the run_timer_list.
 */

#ifndef sub_jiffie
#undef CONFIG_HIGH_RES_TIMERS
#define sub_jiffie() 0
#endif  // sub_jiffie

/*
 * The high_res_test() macro should set up a test mode that will do a
 * worst case timer interrupt.  I.e. it may be that a call to 
 * schedule_next_int() could return -1 indicating that the time has
 * already expired.  This macro says to set it so that schedule_next_int()
 * will always set up a timer interrupt.  This is used during init to
 * calculate the worst case loop time from timer set up to int to 
 * the signal code.

 * high_res_end_test() cancels the above state and allows the no
 * interrupt return from schedule_next_int()
 */
#ifndef high_res_test
#define high_res_test()
#define high_res_end_test()
#endif


#define IF_HIGH_RES(a) a

#else   /*  CONFIG_HIGH_RES_TIMERS */
#define IF_HIGH_RES(a)
#define nsec_to_arch_cycles(a) 0

#endif   /*  CONFIG_HIGH_RES_TIMERS */

/*
 * Here is an SMP helping macro...
 */
#ifdef CONFIG_SMP
#define if_SMP(a) a
#else
#define if_SMP(a)
#endif
/*
 * These should have been defined in the platform hrtimers.h
 * If not (or HIGH_RES_TIMERS not configured) define the default.
 */
#ifndef update_jiffies
extern u64 jiffies_64;
#define update_jiffies() (*(u64 *)&jiffies_64)++
#endif
#ifndef new_jiffie
#define new_jiffie() 0
#endif
#ifndef schedule_next_int
#define schedule_next_int(a,b)
#endif
/*
 * If we included a high-res file, we may have gotten a more efficient 
 * u64/u32, u64%u32 routine.  The one in div64.h actually handles a 
 * u64 result, something we don't need, and, since it is more expensive
 * arch porters are encouraged to implement the div_long_long_rem().
 *
 * int div_long_long_rem(u64 dividend,int divisor,int* remainder)
 * which returns dividend/divisor.
 * 
 * Here we provide default code for those who, for what ever reason,
 * have not provided the above.
 */
#ifndef div_long_long_rem
#include <asm/div64.h>

#define div_long_long_rem(dividend,divisor,remainder) ({ \
                       u64 result = dividend;           \
                       *remainder = do_div(result,divisor); \
                       result; })

#endif   /* ifndef div_long_long_rem */

#endif   /* _HRTIME_H  */
