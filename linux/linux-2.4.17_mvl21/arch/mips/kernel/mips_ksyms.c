/*
 * Export MIPS-specific functions needed for loadable modules.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996, 1997, 1998, 2000, 2001 by Ralf Baechle
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/in6.h>
#include <linux/pci.h>
#include <linux/ide.h>

#include <asm/bootinfo.h>
#include <asm/checksum.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/semaphore.h>
#include <asm/sgi/sgihpc.h>
#include <asm/softirq.h>
#include <asm/uaccess.h>
#ifdef CONFIG_BLK_DEV_FD
#include <asm/floppy.h>
#endif

extern void *__bzero(void *__s, size_t __count);
extern long __strncpy_from_user_nocheck_asm(char *__to,
                                            const char *__from, long __len);
extern long __strncpy_from_user_asm(char *__to, const char *__from,
                                    long __len);
extern long __strlen_user_nocheck_asm(const char *s);
extern long __strlen_user_asm(const char *s);
extern long __strnlen_user_nocheck_asm(const char *s);
extern long __strnlen_user_asm(const char *s);

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
extern char *prom_getenv(char *envname);
#include<asm/avalanche/avalanche_map.h>
#endif

#if defined(CONFIG_MIPS_AVALANCHE) || defined(CONFIG_MIPS_AVALANCHE_D)
extern void iic_write_data(unsigned char addr,unsigned char data);
extern unsigned char iic_read_byte(unsigned int dtype, unsigned int dev, unsigned int adr);
extern int iic_writeb_addr(unsigned int dtype, unsigned int dev, unsigned int adr, unsigned int dat);
#endif /* CONFIG_MIPS_AVALANCHE || CONFIG_MIPS_AVALANCHE_D */

EXPORT_SYMBOL(mips_machtype);
EXPORT_SYMBOL(EISA_bus);

/*
 * String functions
 */
EXPORT_SYMBOL_NOVERS(memcmp);
EXPORT_SYMBOL_NOVERS(memset);
EXPORT_SYMBOL_NOVERS(memcpy);
EXPORT_SYMBOL_NOVERS(memmove);
EXPORT_SYMBOL(simple_strtol);
EXPORT_SYMBOL_NOVERS(strcat);
EXPORT_SYMBOL_NOVERS(strchr);
EXPORT_SYMBOL_NOVERS(strlen);
EXPORT_SYMBOL_NOVERS(strpbrk);
EXPORT_SYMBOL_NOVERS(strncat);
EXPORT_SYMBOL_NOVERS(strnlen);
EXPORT_SYMBOL_NOVERS(strrchr);
EXPORT_SYMBOL_NOVERS(strstr);
EXPORT_SYMBOL_NOVERS(strtok);

EXPORT_SYMBOL(_clear_page);
EXPORT_SYMBOL(enable_irq);
EXPORT_SYMBOL(disable_irq);
EXPORT_SYMBOL(kernel_thread);

/*
 * Userspace access stuff.
 */
EXPORT_SYMBOL_NOVERS(__copy_user);
EXPORT_SYMBOL_NOVERS(__bzero);
EXPORT_SYMBOL_NOVERS(__strncpy_from_user_nocheck_asm);
EXPORT_SYMBOL_NOVERS(__strncpy_from_user_asm);
EXPORT_SYMBOL_NOVERS(__strlen_user_nocheck_asm);
EXPORT_SYMBOL_NOVERS(__strlen_user_asm);
EXPORT_SYMBOL_NOVERS(__strnlen_user_nocheck_asm);
EXPORT_SYMBOL_NOVERS(__strnlen_user_asm);


/* Networking helper routines. */
EXPORT_SYMBOL(csum_partial_copy);

/*
 * Functions to control caches.
 */
EXPORT_SYMBOL(_flush_page_to_ram);
EXPORT_SYMBOL(_flush_cache_all);

EXPORT_SYMBOL(invalid_pte_table);

/*
 * Semaphore stuff
 */
EXPORT_SYMBOL(__down);
EXPORT_SYMBOL(__down_interruptible);
EXPORT_SYMBOL(__down_trylock);
EXPORT_SYMBOL(__up);

/*
 * Architecture specific stuff.
 */
#ifdef CONFIG_MIPS_JAZZ
EXPORT_SYMBOL(vdma_alloc);
EXPORT_SYMBOL(vdma_free);
EXPORT_SYMBOL(vdma_log2phys);
#endif

#ifdef CONFIG_SGI_IP22
EXPORT_SYMBOL(hpc3c0);
#endif

/*
 * Kernel hacking ...
 */
#include <asm/branch.h>
#include <linux/sched.h>

#ifdef CONFIG_VT
EXPORT_SYMBOL(screen_info);
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
EXPORT_SYMBOL(ide_ops);
#endif

EXPORT_SYMBOL(get_wchan);

/* export this symbol for getting environment variables from modules  *JAH* */

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
EXPORT_SYMBOL(prom_getenv);
#endif

#if defined(CONFIG_MIPS_AVALANCHE) || defined(CONFIG_MIPS_AVALANCHE_D)
EXPORT_SYMBOL(iic_write_data);  /* I2C functions from /arch/mips/mips-boards/time.c */
EXPORT_SYMBOL(iic_read_byte);
EXPORT_SYMBOL(iic_writeb_addr);
#endif /* CONFIG_MIPS_AVALANCHE || CONFIG_MIPS_AVALANCHE_D */
