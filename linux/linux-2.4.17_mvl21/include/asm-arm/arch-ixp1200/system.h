/*
 * linux/include/asm-arm/arch-ixp1200/system.h
 *
 * Copyright (c) 1996,1997,1998 Russell King.
 *
 * Added IXP1200 support - Uday Naik
 */

#include <linux/config.h>

static inline void arch_idle(void)
{
	while (!current->need_resched && !hlt_counter) {
		cpu_do_idle(IDLE_CLOCK_SLOW);
		cpu_do_idle(IDLE_WAIT_FAST);
		cpu_do_idle(IDLE_CLOCK_FAST);
	}
}

extern __inline__ void arch_reset(char mode)
{
	
  cli();

  /* To do a reboot, we set the IXP1200 reset register */

  *CSR_IXPRESET = 0xffffffff;

}

#define arch_power_off()	do { } while (0)

