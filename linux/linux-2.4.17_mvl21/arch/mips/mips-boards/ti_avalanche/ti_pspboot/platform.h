#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <linux/init.h>  
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <asm/string.h>  
#include <stdio.h>       


/* Important: The definition of ENV_SPACE_SIZE should match with that in 
 * PSPBoot. (/psp_boot/inc/psbl/env.h)
 */
#ifdef CONFIG_MIPS_AVALANCHE_TICFG
#define ENV_SPACE_SIZE      (10 * 1024)
#endif

#ifdef CONFIG_MIPS_TNETV1050SDB 
#define TNETV1050SDB
#define DUAL_FLASH
#endif

#ifdef CONFIG_MIPS_AR7DB 
#define TNETD73XX_BOARD
#define AR7DB
#endif

#ifdef CONFIG_MIPS_AR7RD 
#define TNETD73XX_BOARD
#define AR7RD
#endif

#ifdef CONFIG_MIPS_AR7WRD
#define TNETD73XX_BOARD
#define AR7WRD
#endif

#ifdef CONFIG_MIPS_AR7VWI
#define TNETD73XX_BOARD
#define AR7VWi
#endif

/* Merging from the DEV_DSL-PSPL4.3.2.7_Patch release. */
#ifdef CONFIG_MIPS_AR7VW
#define TNETD73XX_BOARD
#define AR7WRD
#endif

#ifdef CONFIG_MIPS_AR7WI
#define TNETD73XX_BOARD
#define AR7Wi
#endif

#ifdef CONFIG_MIPS_AR7V
#define TNETD73XX_BOARD
#define AR7V
#endif

#ifdef CONFIG_MIPS_AR7V
#define TNETD73XX_BOARD
#define AR7V
#endif

#ifdef CONFIG_MIPS_WA1130
#define AVALANCHE
#define WLAN
#endif

#define sys_printf  printk
#define _malloc(x)  vmalloc((x))
#define _free(x)    vfree((x))

#endif
