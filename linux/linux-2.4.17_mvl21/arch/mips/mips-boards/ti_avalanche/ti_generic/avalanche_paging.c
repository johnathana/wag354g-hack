/*
 *  -*- linux-c -*-
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2002 by Jeff Harrell (jharrell@ti.com)
 * Copyright (C) 2002 Texas Instruments, Inc.
 *
 */

/*
 * This file takes care of the "memory hole" issue that exists with the standard
 * linux kernel and the TI Avalanche ASIC.  The Avalanche ASIC requires an offset
 * of 0x14000000 due to the ASIC's memory map constraints.  This file corrects the
 * paging tables so that the only reflect valid memory (i.e. > 0x14000000)
 * 
 *  -JAH
 */
#include <linux/config.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/smp.h>
#include <linux/init.h>
#ifdef CONFIG_BLK_DEV_INITRD
#include <linux/blk.h>
#endif /* CONFIG_BLK_DEV_INITRD */
#include <linux/highmem.h>
#include <linux/bootmem.h>

#include <asm/processor.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/mmu_context.h>
#include <asm/io.h>
#include <asm/tlb.h>
#include <asm/cpu.h>

#ifdef CONFIG_DISCONTIGMEM
pg_data_t discontig_page_data[NR_NODES];
bootmem_data_t discontig_node_bdata[NR_NODES];
#endif

static unsigned long totalram_pages;
/* static unsigned long totalhigh_pages; */

#define START_PFN (NODE_DATA(0)->bdata->node_boot_start >> PAGE_SHIFT)
#define MAX_LOW_PFN (NODE_DATA(0)->bdata->node_low_pfn)

#define PFN_UP(x)  (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)        ((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)        ((x) << PAGE_SHIFT)

unsigned long bootmap_size;
unsigned long start_pfn, max_pfn;
extern unsigned long max_low_pfn;

extern char *prom_getenv(char *envname);

extern struct mips_cpu mips_cpu;
extern void prom_printf(char *fmt, ...);

/*
 * We have upto 8 empty zeroed pages so we can map one of the right colour
 * when needed.  This is necessary only on R4000 / R4400 SC and MC versions
 * where we have to avoid VCED / VECI exceptions for good performance at
 * any price.  Since page is never written to after the initialization we
 * don't have to care about aliases on other CPUs.
 */

static inline unsigned long setup_zero_pages(void)
{
	unsigned long order, size;
	struct page *page;
	if(mips_cpu.options & MIPS_CPU_VCE) 
		order = 3;
	else 
		order = 0;

	empty_zero_page = __get_free_pages(GFP_KERNEL, order);
	
	if (!empty_zero_page)
		panic("Oh boy, that early out of memory?");

	page = virt_to_page(empty_zero_page);
	
	while (page < virt_to_page(empty_zero_page + (PAGE_SIZE << order))) {
		set_bit(PG_reserved, &page->flags);
		set_page_count(page, 0);
		page++;
	}

	size = PAGE_SIZE << order;
	zero_page_mask = (size - 1) & PAGE_MASK;
	memset((void *)empty_zero_page, 0, size);

	return 1UL << order;
}

/*
 * paging_init() sets up the page tables
 *
 * This routines also unmaps the page at virtual kernel address 0, so
 * that we can trap those pesky NULL-reference errors in the kernel.
 */
void __init paging_init(void)
{
        unsigned long zones_size[MAX_NR_ZONES] = {0, 0, 0};
        unsigned long low, start_pfn;

	/* Initialize the entire pgd.  */
	pgd_init((unsigned long)swapper_pg_dir);
	pgd_init((unsigned long)swapper_pg_dir + PAGE_SIZE / 2);


        start_pfn = START_PFN;
        // max_dma = virt_to_phys((char *)MAX_DMA_ADDRESS) >> PAGE_SHIFT;
        low = MAX_LOW_PFN;

        /* Avalanche DMA-able memory 0x14000000+memsize */

        zones_size[ZONE_DMA] = low - start_pfn;
        
        free_area_init_node(0, NODE_DATA(0), 0, zones_size, __MEMORY_START, 0);

#ifdef CONFIG_DISCONTIGMEM
        zones_size[ZONE_DMA] = __MEMORY_SIZE_2ND >> PAGE_SHIFT;
        zones_size[ZONE_NORMAL] = 0;
        free_area_init_node(1, NODE_DATA(1), 0, zones_size, __MEMORY_START_2ND, 0);
#endif /* CONFIG_DISCONTIGMEM */
 	
}

extern char _ftext, _etext, _fdata, _edata, _end;
extern char __init_begin, __init_end;

void __init mem_init(void)
{
	int codesize, reservedpages, datasize, initsize;
	int tmp;


	max_mapnr = num_physpages = MAX_LOW_PFN - START_PFN;
	high_memory = (void *)__va(MAX_LOW_PFN * PAGE_SIZE);

#if !defined(CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL)
	/* free up the memory associated with Adam2 -
	 * that is the, after the first page that is 
	 * reserved all the way up to the start of the kernel
	 */
	free_bootmem_node(NODE_DATA(0), (__MEMORY_START+PAGE_SIZE),
			  (__pa(&_ftext))-(__MEMORY_START+PAGE_SIZE) );

	printk("Freeing Adam2 reserved memory [0x%08x,0x%08x]\n", 
	       (__MEMORY_START+PAGE_SIZE),  (__pa(&_ftext))-(__MEMORY_START+PAGE_SIZE) );

#endif

	/* this will put all low memory onto the freelists */
	totalram_pages += free_all_bootmem_node(NODE_DATA(0));

        /* Setup zeroed pages */
	totalram_pages -= setup_zero_pages();	


#ifdef CONFIG_DISCONTIGMEM
	totalram_pages += free_all_bootmem_node(NODE_DATA(1));
#endif
	reservedpages = 0;
	for (tmp = 0; tmp < num_physpages; tmp++)
		/*
		 * Only count reserved RAM pages
		 */
		if (PageReserved(mem_map+tmp))
			reservedpages++;

	codesize =  (unsigned long) &_etext - (unsigned long) &_ftext;
	datasize =  (unsigned long) &_edata - (unsigned long) &_fdata;
	initsize =  (unsigned long) &__init_end - (unsigned long) &__init_begin;

	printk("Memory: %luk/%luk available (%dk kernel code, %dk reserved, %dk data, %dk init)\n",
		(unsigned long) nr_free_pages() << (PAGE_SHIFT-10),
		max_mapnr << (PAGE_SHIFT-10),
		codesize >> 10,
		reservedpages << (PAGE_SHIFT-10),
		datasize >> 10,
		initsize >> 10);

}

/* fixes paging routines for avalanche  (utilized in /arch/mips/kernel/setup.c) */

void avalanche_bootmem_init(void)
{
        unsigned int memsize,memory_end,memory_start;
        char *memsize_str;

	memsize_str = prom_getenv("memsize");
	if (!memsize_str) {
		memsize = 0x02000000;
	} else {
		memsize = simple_strtol(memsize_str, NULL, 0);
        }


        memory_start = (unsigned long)PAGE_OFFSET+__MEMORY_START;
	memory_end = memory_start + memsize;
  
        /*
         * Find the highest memory page fram number we have available 
         */

         max_pfn = PFN_DOWN(__pa(memory_end));
  
         /*
          * Determine the low and high memory ranges 
          */

         max_low_pfn = max_pfn;
  
         /*
          * Partially used pages are not usable - thus we are
          * rounding upwards:
          */

         start_pfn = PFN_UP(__pa(&_end));
  
         /*
          * Find a proper area for the bootmem bitmap. After this
          * bootstrap step all allocations (until the page allocator is
          * intact)  must be done via bootmem_alloc().
          */

         bootmap_size = init_bootmem_node(NODE_DATA(0), start_pfn,
                                          __MEMORY_START>>PAGE_SHIFT, max_low_pfn);
  

         /* 
          * Register fully available low RAM pages with the bootmem allocator.
          */
  
         {
           unsigned long curr_pfn, last_pfn, pages;
    
           /*
            * We are rounding up the start address of usable memory:
            */
           curr_pfn = PFN_UP(__MEMORY_START);
    
           /*
            * ... and at the end of the usable range downwards:
            */
           last_pfn = PFN_DOWN(__pa(memory_end));
    
           if (last_pfn > max_low_pfn)
             last_pfn = max_low_pfn;
           
           pages = last_pfn - curr_pfn;


           free_bootmem_node(NODE_DATA(0), PFN_PHYS(curr_pfn),
                             PFN_PHYS(pages));
         }

         /*
          * Reserve the kernel text and
          * Reserve the bootmem bitmap. We do this in two steps (first step
          * was init_bootmem()), because this catches the (definitely buggy)
          * case of us accidentally initializing the bootmem allocator with
          * an invalid RAM area.
          */
         reserve_bootmem_node(NODE_DATA(0), __MEMORY_START+PAGE_SIZE,
                              (PFN_PHYS(start_pfn)+bootmap_size+PAGE_SIZE-1)-__MEMORY_START);
        
         /*
          * reserve physical page 0 - it's a special BIOS page on many boxes,
          * enabling clean reboots, SMP operation, laptop functions.
          */
         reserve_bootmem_node(NODE_DATA(0), __MEMORY_START, PAGE_SIZE);
}

extern char __init_begin, __init_end;

void free_initmem(void)
{
	unsigned long addr;
        //	prom_free_prom_memory ();
    
	addr = (unsigned long) &__init_begin;
	while (addr < (unsigned long) &__init_end) {
		ClearPageReserved(virt_to_page(addr));
		set_page_count(virt_to_page(addr), 1);
		free_page(addr);
		totalram_pages++;
		addr += PAGE_SIZE;
	}
	printk("Freeing unused kernel memory: %dk freed\n",
	       (&__init_end - &__init_begin) >> 10);
}

void si_meminfo(struct sysinfo *val)
{
	val->totalram = totalram_pages;
	val->sharedram = 0;
	val->freeram = nr_free_pages();
	val->bufferram = atomic_read(&buffermem_pages);
	val->totalhigh = 0;
	val->freehigh = nr_free_highpages();
	val->mem_unit = PAGE_SIZE;

	return;
}









