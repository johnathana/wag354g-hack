/* $Id: avalanche_jump.S,v 1.1.1.2 2005/03/28 06:55:55 sparq Exp $
 *  
 * TI EVM3 exception vector jump table
 *
 *   Jeff Harrell (jharrell@ti.com)
 *   Copyright(C) Texas Instruments, 2000-2001
*/               

#include <linux/config.h>
#include <linux/threads.h>

#include <asm/asm.h>
#include <asm/cacheops.h>
#include <asm/current.h>
#include <asm/offset.h>
#include <asm/processor.h>
#include <asm/regdef.h>
#include <asm/cachectl.h>
#include <asm/mipsregs.h>
#include <asm/stackframe.h>
/* #include <asm/bootinfo.h> */

        .text

        .set noreorder
        .set noat
        
        /*-----------------*/
        /* tlb miss vector */
        /*-----------------*/
        
        LEAF(jump_tlb_miss)
        .set mips2
        lui     k0,0x9400
        ori     k0,0
	jr	k0
	 nop       
        END(jump_tlb_miss)

        /*------------------------*/
        /* unused tlb miss vector */
        /*------------------------*/
        
        LEAF(jump_tlb_miss_unused)
        .set mips2
        lui     k0,0x9400
        ori     k0,0x80
	jr	k0
	 nop       
        END(jump_tlb_miss_unused)

        /*--------------------*/
        /* cache error vector */
        /*--------------------*/
        
        LEAF(jump_cache_error)
        .set mips2
        lui     k0,0x9400
        ori     k0,0x100
	jr	k0
	 nop       
        END(jump_cache_error)

        /*--------------------*/
        /* general exception  */
        /*--------------------*/

        
        LEAF(jump_general_exception)
        .set mips2
        lui     k0,0x9400
        ori     k0,0x180
	jr	k0
	 nop       
        END(jump_general_exception)

        /*---------------------*/
        /* dedicated interrupt */
        /*---------------------*/
        
        LEAF(jump_dedicated_interrupt)
        .set mips2
        lui     k0,0x9400
        ori     k0,0x200
	jr	k0
	 nop       
        END(jump_dedicated_interrupt)
        
        .set at










