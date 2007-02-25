#ifndef __AVALANCHE_MAP_H__
#define __AVALANCHE_MAP_H__

#include <linux/config.h>
#include <linux/module.h>

#include <asm/avalanche/generic/avalanche_misc.h>

#if defined(CONFIG_MIPS_SANGAM)
#include <asm/avalanche/sangam/sangam.h>
#include <asm/avalanche/sangam/sangam_clk_cntl.h>
#include <asm/avalanche/sangam/sangam_boards.h>
#endif

#if defined(CONFIG_MIPS_TITAN)
#include <asm/avalanche/titan/titan.h>
#include <asm/avalanche/titan/titan_clk_cntl.h>
#include <asm/avalanche/titan/titan_boards.h>
#endif

#if defined(CONFIG_MIPS_PUMAS)
#include <asm/avalanche/puma-s/puma-s.h>
#include <asm/avalanche/puma-s/puma-s_clk_cntl.h>
#include <asm/avalanche/puma-s/puma-s_boards.h>
#endif

#if defined(CONFIG_MIPS_AVALANCHE_INTC)
#include <asm/avalanche/generic/avalanche_intc.h>
#endif


#if defined(CONFIG_MIPS_AVALANCHE_QUICK_IIC)
#include <asm/avalanche/generic/avalanche_i2c.h>
#endif

#include <asm/avalanche/generic/hal_modules/haltypes.h>

#if defined(CONFIG_MIPS_AVALANCHE_LED)
#include <asm/avalanche/generic/led_ioctl.h>
#include <asm/avalanche/generic/led_config.h>
#include <asm/avalanche/generic/led_hal.h>
#endif

#endif /* __AVALANCHE_MAP_H__ */
