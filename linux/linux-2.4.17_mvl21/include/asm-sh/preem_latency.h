/*
 * include/asm-sh/preem_latency.h
 * timing support for preempt-stats patch
 */

#ifndef _ASM_PREEM_LATENCY_H
#define _ASM_PREEM_LATENCY_H

#include <asm/io.h>
#include <asm/timex.h>

#if defined(__sh3__)
#define TMU1_TCNT     0xfffffea4      /* Long access */
#elif defined(__SH4__)
#define TMU1_TCNT     0xffd80018      /* Long access */
#endif

#define readclock_init()
#define readclock(v) v = (unsigned long)ctrl_inl(TMU1_TCNT)
#define clock_to_usecs(clocks)  ((clocks)/(CLOCK_TICK_RATE/1000000))
#define TICKS_PER_USEC CLOCK_TICK_RATE/1000000

#define CLOCK_COUNTS_DOWN

#define INTERRUPTS_ENABLED(x) ((x & 0x000000f0) == 0)   

#endif /* _ASM_PREEM_LATENCY_H */
