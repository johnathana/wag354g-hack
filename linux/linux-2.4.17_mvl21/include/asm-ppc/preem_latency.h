#ifndef _ASM_PREEM_LATENCY_H
#define _ASM_PREEM_LATENCY_H

#include <asm/time.h>

#define readclock_init()

#define readclock(x)	x = (unsigned) get_tbl()
#define clock_to_usecs(x)	mulhwu(tb_to_us,(x))
#define INTR_IENABLE            MSR_EE
#define INTERRUPTS_ENABLED(x)   (x & INTR_IENABLE)
#endif
