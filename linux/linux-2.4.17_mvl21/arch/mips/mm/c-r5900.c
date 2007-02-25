/* $Id: c-r5900.c,v 1.1.1.2 2005/03/28 06:55:56 sparq Exp $
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * r5900.c: R5900 processor specific MMU/Cache routines.
 *
 * Copyright (C) 2000 Sony Computer Entertainment Inc.
 * Copyright (C) 2001 Paul Mundt (lethal@chaoticdreams.org)
 *
 * This file is based on r4xx0.c:
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997, 1998 Ralf Baechle ralf@gnu.org
 *
 * To do:
 *
 *  - this code is a overbloated pig
 *  - many of the bug workarounds are not efficient at all, but at
 *    least they are functional ...
 */
#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>

#include <asm/bcache.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/bootinfo.h>
#include <asm/mmu_context.h>

/* CP0 hazard avoidance. */
#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
				     ".word 0x0000040f;\n\t" \
				     ".set reorder\n\t")

/* Primary cache parameters. */
static int icache_size, dcache_size; /* Size in bytes */
static int ic_lsize, dc_lsize;       /* LineSize in bytes */

#include <asm/r5900_cacheops.h>
#include <asm/r5900_cache.h>

#undef DEBUG_CACHE

static inline void r5900_flush_cache_all_d64i64(void)
{
	unsigned long flags;

	save_and_cli(flags);
	blast_dcache64(); blast_icache64();
	restore_flags(flags);
}

static void r5900_flush_cache_range_d64i64(struct mm_struct *mm,
					 unsigned long start,
					 unsigned long end)
{
	if(mm->context != 0) {
		unsigned long flags;

#ifdef DEBUG_CACHE
		printk("crange[%d,%08lx,%08lx]", (int)mm->context, start, end);
#endif
		save_and_cli(flags);
		blast_dcache64(); blast_icache64();
		restore_flags(flags);
	}
}

static void r5900_flush_icache_range_i64(unsigned long start,
					    unsigned long end)
{
	unsigned long flags;

#ifdef DEBUG_CACHE
	printk("irange[%08lx,%08lx]", start, end);
#endif
	save_and_cli(flags);
	blast_icache64();
	restore_flags(flags);
}


static void r5900_flush_cache_mm_d64i64(struct mm_struct *mm)
{
	if(mm->context != 0) {
#ifdef DEBUG_CACHE
		printk("cmm[%d]", (int)mm->context);
#endif
		r5900_flush_cache_all_d64i64();
	}
}

static void r5900_flush_cache_page_d64i64(struct vm_area_struct *vma,
					unsigned long page)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long flags;
	pgd_t *pgdp;
	pmd_t *pmdp;
	pte_t *ptep;
	int text;

	/*
	 * If ownes no valid ASID yet, cannot possibly have gotten
	 * this page into the cache.
	 */
	if(mm->context == 0)
		return;

#ifdef DEBUG_CACHE
	printk("cpage[%d,%08lx]", (int)mm->context, page);
#endif
	save_and_cli(flags);
	page &= PAGE_MASK;
	pgdp = pgd_offset(mm, page);
	pmdp = pmd_offset(pgdp, page);
	ptep = pte_offset(pmdp, page);

	/* If the page isn't marked valid, the page cannot possibly be
	 * in the cache.
	 */
	if(!(pte_val(*ptep) & _PAGE_VALID))
		goto out;

	text = (vma->vm_flags & VM_EXEC);
	/*
	 * Doing flushes for another ASID than the current one is
	 * too difficult since stupid R4k caches do a TLB translation
	 * for every cache flush operation.  So we do indexed flushes
	 * in that case, which doesn't overly flush the cache too much.
	 */
	if(mm == current->mm) {
		blast_dcache64_page(page);
		if(text)
			blast_icache64_page(page);
	} else {
		/* Do indexed flush, too much work to get the (possible)
		 * tlb refills to work correctly.
		 */
		page = (KSEG0 + (page & (dcache_size - 1)));
		blast_dcache64_page_indexed(page);
		if(text)
			blast_icache64_page_indexed(page);
	}
out:
	restore_flags(flags);
}

static void r5900_flush_icache_page_i64(struct vm_area_struct *vma,
					struct page * page)
{
	if (!(vma->vm_flags & VM_EXEC))
		return;

	flush_cache_all();
}

static void r5900_flush_page_to_ram_d64i64(struct page * page)
{
	unsigned long addr = (unsigned long) page_address(page) & PAGE_MASK;
	if((addr >= KSEG0 && addr < KSEG1) || (addr >= KSEG2)) {
		unsigned long flags;

#ifdef DEBUG_CACHE
		printk("cram[%08lx]", addr);
#endif
		save_and_cli(flags);
		blast_dcache64_page(addr);
		restore_flags(flags);
	}
}

/*
 * Writeback and invalidate the primary cache dcache before DMA.
 */
static void
r5900_dma_cache_wback_inv_pc(unsigned long addr, unsigned long size)
{
	if (size >= dcache_size)
		flush_cache_all();

	bc_wback_inv(addr, size);
}

static void
r5900_dma_cache_inv_pc(unsigned long addr, unsigned long size)
{
	if (size >= dcache_size)
		flush_cache_all();

	bc_inv(addr, size);
}

/*
 * While we're protected against bad userland addresses we don't care
 * very much about what happens in that case.  Usually a segmentation
 * fault will dump the process later on anyway ...
 */
static void r5900_flush_cache_sigtramp(unsigned long addr)
{
	/*
	 * FIXME: What's the point of daddr and iaddr? I don't see any need
	 * for doing the flush on both the addresses _and_ the addresses + the
	 * line size. Am I missing something?
	 */
	unsigned long daddr, iaddr;

	daddr = addr & ~(dc_lsize - 1);

	protected_writeback_dcache_line(daddr);
	protected_writeback_dcache_line(daddr + dc_lsize);
	iaddr = addr & ~(ic_lsize - 1);
	protected_flush_icache_line(iaddr);
	protected_flush_icache_line(iaddr + ic_lsize);
}

static void r5900_update_mmu_cache(struct vm_area_struct * vma,
				   unsigned long address, pte_t pte)
{
	unsigned long flags;
	pgd_t *pgdp;
	pmd_t *pmdp;
	pte_t *ptep;
	int idx, pid;

	pid = (get_entryhi() & 0xff);

#ifdef DEBUG_TLB
	if((pid != (vma->vm_mm->context & 0xff)) || (vma->vm_mm->context == 0)) {
		printk("update_mmu_cache: Wheee, bogus tlbpid mmpid=%d tlbpid=%d\n",
		       (int) (vma->vm_mm->context & 0xff), pid);
	}
#endif

	save_and_cli(flags);
	address &= (PAGE_MASK << 1);
	set_entryhi(address | (pid));
	pgdp = pgd_offset(vma->vm_mm, address);
	BARRIER;
	tlb_probe();
	BARRIER;
	pmdp = pmd_offset(pgdp, address);
	idx = get_index();
	ptep = pte_offset(pmdp, address);
	BARRIER;
	if ((signed long)pte_val(*ptep) < 0) {
	    /* scratchpad RAM mapping */
	    address &= (PAGE_MASK << 2);	/* must be 16KB aligned */
	    set_entryhi(address | (pid));
	    BARRIER;
	    tlb_probe();
	    BARRIER;
	    idx = get_index();
	    BARRIER;
	    set_entrylo0(0x80000006);	/* S, D, V */
	    set_entrylo1(0x00000006);	/*    D, V */
	    set_entryhi(address | (pid));
	} else {
	    set_entrylo0(pte_val(*ptep++) >> 6);
	    set_entrylo1(pte_val(*ptep) >> 6);
	    set_entryhi(address | (pid));
	}
	BARRIER;
	if(idx < 0) {
		tlb_write_random();
	} else {
		tlb_write_indexed();
	}
	BARRIER;
	set_entryhi(pid);
	BARRIER;
	restore_flags(flags);
}

/* Detect and size the various r4k caches. */
static void __init probe_icache(unsigned long config)
{
	icache_size = 1 << (12 + ((config >> 9) & 7));
	ic_lsize = 64;	/* fixed */

	printk("Primary instruction cache %dkb, linesize %d bytes\n",
	       icache_size >> 10, 64);
}

static void __init probe_dcache(unsigned long config)
{
	dcache_size = 1 << (12 + ((config >> 6) & 7));
	dc_lsize = 64;	/* fixed */

	printk("Primary data cache %dkb, linesize %d bytes\n",
	       dcache_size >> 10, 64);
}

static void __init setup_noscache_funcs(void)
{
	_clear_page = r5900_clear_page_d16;
	_copy_page = r5900_copy_page_d16;
	_flush_cache_all = r5900_flush_cache_all_d64i64;
	_flush_cache_mm = r5900_flush_cache_mm_d64i64;
	_flush_cache_range = r5900_flush_cache_range_d64i64;
	_flush_cache_page = r5900_flush_cache_page_d64i64;

	___flush_cache_all = _flush_cache_all;
	_flush_icache_range = r5900_flush_icache_range_i64;
	_flush_icache_page = r5900_flush_icache_page_i64; /* FIXME */

	_flush_page_to_ram = r5900_flush_page_to_ram_d64i64;
	_dma_cache_wback_inv = r5900_dma_cache_wback_inv_pc;
	_dma_cache_inv = r5900_dma_cache_inv_pc;
}

static inline void setup_scache(unsigned int config)
{
	setup_noscache_funcs();
}

void __init ld_mmu_r5900(void)
{
	unsigned long config = read_32bit_cp0_register(CP0_CONFIG);

	/*
	 * Display CP0 config reg. to verify the workaround 
	 * for branch prediction bug is done, or not.
	 * R5900 has a problem of branch prediction.
	 */
	printk("  Branch Prediction  : %s\n",  
		(config & CONF_R5900_BPE)? "on":"off");
	printk("  Double Issue       : %s\n",  
		(config & CONF_R5900_DIE)? "on":"off");

	clear_cp0_config(CONF_CM_CMASK);
	set_cp0_config(CONF_CM_CACHABLE_NONCOHERENT);

	probe_icache(config);
	probe_dcache(config);
	setup_scache(config);

	_flush_cache_sigtramp = r5900_flush_cache_sigtramp;
	update_mmu_cache = r5900_update_mmu_cache;

	flush_cache_all();
	write_32bit_cp0_register(CP0_WIRED, 0);

	/*
	 * You should never change this register:
	 *   - On R4600 1.7 the tlbp never hits for pages smaller than
	 *     the value in the c0_pagemask register.
	 *   - The entire mm handling assumes the c0_pagemask register to
	 *     be set for 4kb pages.
	 */
	write_32bit_cp0_register(CP0_PAGEMASK, PM_4K);
	flush_tlb_all();
}

