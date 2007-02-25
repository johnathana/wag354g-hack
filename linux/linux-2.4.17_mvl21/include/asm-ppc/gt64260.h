/*
 * include/asm-ppc/gt64260.h
 * 
 * Prototypes, etc. for the Marvell/Galileo GT64260 host bridge routines.
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 *
 * Copyright 2001 MontaVista Software Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#ifndef __ASMPPC_GT64260_H
#define __ASMPPC_GT64260_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <asm/gt64260_defs.h>


extern u32     gt64260_base;
extern u32     gt64260_revision;
extern u8      gt64260_pci_exclude_bridge;

#ifndef	TRUE
#define	TRUE	(0 == 0)
#endif

#ifndef	FALSE
#define	FALSE	(0 != 0)
#endif

#define	GT64260_IRQ_MPSC0		40
#define	GT64260_IRQ_MPSC1		42

#define	GT64260_IRQ_SDMA		36

typedef struct {
	struct pci_controller	*hose_a;
	struct pci_controller	*hose_b;

	u32	mem_size;

	u32	pci_0_io_start_proc;
	u32	pci_0_io_start_pci;
	u32	pci_0_io_size;
	u32	pci_0_io_swap;

	u32	pci_0_mem_start_proc;
	u32	pci_0_mem_start_pci_hi;
	u32	pci_0_mem_start_pci_lo;
	u32	pci_0_mem_size;
	u32	pci_0_mem_swap;

	u32	pci_1_io_start_proc;
	u32	pci_1_io_start_pci;
	u32	pci_1_io_size;
	u32	pci_1_io_swap;

	u32	pci_1_mem_start_proc;
	u32	pci_1_mem_start_pci_hi;
	u32	pci_1_mem_start_pci_lo;
	u32	pci_1_mem_size;
	u32	pci_1_mem_swap;
} gt64260_bridge_init_info_t;

/*
 *****************************************************************************
 *
 *	I/O macros to access the 64260's registers
 *
 *****************************************************************************
 */

#define	GT64260_REG_READ(offs)					\
	(in_le32((volatile uint *)(gt64260_base + (offs))))
#define	GT64260_REG_WRITE(offs, d)					\
	(out_le32((volatile uint *)(gt64260_base + (offs)), (int)(d)))

#if 0 /* paranoid SMP version */
#define GT64260_REG_MODIFY(offs, data, mask) \
{	\
	unsigned long reg;	\
	spin_lock(&gt64260_lock);	\
	reg = GT64260_REG_READ(offs) & ~mask;	/* zero any bits we care about*/\
	GT64260_REG_WRITE(offs, (reg | (data & mask))); /* set bits from the data */\
	spin_unlock(&gt64260_lock);	\
}
#else
#define GT64260_REG_MODIFY(offs, data, mask) \
{	\
	unsigned long reg;	\
	reg = GT64260_REG_READ(offs) & ~mask;	/* zero any bits we care about*/\
	GT64260_REG_WRITE(offs, (reg | (data & mask))); /* set bits from the data */\
}
#endif


#define	GT64260_SET_REG_BITS(offs, bits) GT64260_REG_MODIFY(offs, bits, 0) 

#define	GT64260_RESET_REG_BITS(offs, bits) GT64260_REG_MODIFY(offs, 0, bits)


/*
 *****************************************************************************
 *
 *	Function Prototypes
 *
 *****************************************************************************
 */

int gt64260_set_base(u32 new_base);
int gt64260_get_base(u32 *base);
int gt64260_bridge_init(gt64260_bridge_init_info_t *info);
int gt64260_cpu_scs_set_window(u32 window,
			       u32 base_addr,
			       u32 size);
int gt64260_cpu_cs_set_window(u32 window,
			      u32 base_addr,
			      u32 size);
int gt64260_cpu_boot_set_window(u32 base_addr,
			        u32 size);
int gt64260_cpu_set_pci_io_window(u32 pci_bus,
			          u32 cpu_base_addr,
			          u32 pci_base_addr,
			          u32 size,
			          u32 swap);
int gt64260_cpu_set_pci_mem_window(u32 pci_bus,
			           u32 window,
			           u32 cpu_base_addr,
			           u32 pci_base_addr_hi,
			           u32 pci_base_addr_lo,
			           u32 size,
			           u32 swap_64bit);
int gt64260_cpu_prot_set_window(u32 window,
			        u32 base_addr,
			        u32 size,
			        u32 access_bits);
int gt64260_cpu_snoop_set_window(u32 window,
			         u32 base_addr,
			         u32 size,
			         u32  snoop_type);
void gt64260_cpu_disable_all_windows(void);
int gt64260_pci_bar_enable(u32 pci_bus, u32 enable_bits);
int gt64260_pci_slave_scs_set_window(struct pci_controller *hose,
				     u32 window,
				     u32 pci_base_addr,
				     u32 cpu_base_addr,
				     u32 size);
int gt64260_pci_slave_cs_set_window(struct pci_controller *hose,
				    u32 window,
				    u32 pci_base_addr,
				    u32 cpu_base_addr,
				    u32 size);
int gt64260_pci_slave_boot_set_window(struct pci_controller *hose,
				      u32 pci_base_addr,
				      u32 cpu_base_addr,
				      u32 size);
int gt64260_pci_slave_p2p_mem_set_window(struct pci_controller *hose,
				         u32 window,
				         u32 pci_base_addr,
				         u32 other_bus_base_addr,
				         u32 size);
int gt64260_pci_slave_p2p_io_set_window(struct pci_controller *hose,
				        u32 pci_base_addr,
				        u32 other_bus_base_addr,
				        u32 size);
int gt64260_pci_slave_dac_scs_set_window(struct pci_controller *hose,
				         u32 window,
				         u32 pci_base_addr_hi,
				         u32 pci_base_addr_lo,
				         u32 cpu_base_addr,
				         u32 size);
int gt64260_pci_slave_dac_cs_set_window(struct pci_controller *hose,
				        u32 window,
				        u32 pci_base_addr_hi,
				        u32 pci_base_addr_lo,
				        u32 cpu_base_addr,
				        u32 size);
int gt64260_pci_slave_dac_boot_set_window(struct pci_controller *hose,
				          u32 pci_base_addr_hi,
				          u32 pci_base_addr_lo,
				          u32 cpu_base_addr,
				          u32 size);
int gt64260_pci_slave_dac_p2p_mem_set_window(struct pci_controller *hose,
				             u32 window,
				             u32 pci_base_addr_hi,
				             u32 pci_base_addr_lo,
				             u32 other_bus_base_addr,
				             u32 size);
int gt64260_pci_acc_cntl_set_window(u32 pci_bus,
			            u32 window,
			            u32 base_addr_hi,
			            u32 base_addr_lo,
			            u32 size,
			            u32 features);
int gt64260_pci_snoop_set_window(u32 pci_bus,
			         u32 window,
			         u32 base_addr_hi,
			         u32 base_addr_lo,
			         u32 size,
			         u32 snoop_type);
int gt64260_pci_exclude_device(u8 bus, u8 devfn);

void gt64260_init_irq(void);
int gt64260_get_irq(struct pt_regs *regs);

void gt64260_mpsc_progress(char *s, unsigned short hex);

#endif /* __ASMPPC_GT64260_H */
