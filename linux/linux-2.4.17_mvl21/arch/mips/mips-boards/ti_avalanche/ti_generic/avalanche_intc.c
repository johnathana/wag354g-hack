/*
 * Nitin Dhingra, iamnd@ti.com
 * Copyright (C) 2002 Texas Instruments, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Routines for generic manipulation of the interrupts found on the Texas
 * Instruments avalanche board
 *
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <asm/irq.h>
#include <asm/mips-boards/prom.h>
#include <asm/avalanche/generic/avalanche_intc.h>
#include <asm/avalanche/avalanche_map.h>

#include <asm/gdb-stub.h>



#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
#include <asm/avalanche/generic/vlynq_hal.h>     
extern VLYNQ_DEV vlynqDevice0;                   
extern VLYNQ_DEV vlynqDevice1;                   
#endif


/* Interrupt Line #'s  (Avalanche peripherals) */

/*******************************************************************************
* Avalanche primary interrupts
*******************************************************************************/

//irq_cpustat_t irq_stat [NR_CPUS];

/*
   The irq_desc table is used in the x86 architecture.  There are currently
   features available that we are not using - JAH
*/


irq_desc_t irq_desc[AVALANCHE_INT_END + 1];
// extern irq_desc_t irq_desc[80];
unsigned long irq_err_count = 0;

struct avalanche_ictrl_regs         *avalanche_hw0_icregs;  /* Interrupt control regs (primary)   */
struct avalanche_exctrl_regs        *avalanche_hw0_ecregs;  /* Exception control regs (secondary) */
struct avalanche_ipace_regs         *avalanche_hw0_ipaceregs;
struct avalanche_channel_int_number *avalanche_hw0_chregs;  /* Channel control registers          */

/* Action for UNIFIED SECONDARY INTERRUPT */
static struct irqaction unified_secondary_action;

extern asmlinkage void mipsIRQ(void);

//unsigned int local_bh_count[NR_CPUS];
//unsigned int local_irq_count[NR_CPUS];


/*
 *   The avalanche/MIPS interrupt line numbers are used to represent the
 *   interrupts within the irqaction arrays.  The index notation is
 *   is as follows:
 *
 *           0-7    MIPS CPU Exceptions  (HW/SW)
 *           8-47   Primary Interrupts   (Avalanche)
 *          48-79   Secondary Interrupts (Avalanche)
 *          80-111  Low Vlynq Interrupts (Vlynq)
 *         112-143  High Vlynq Interrupts(Vlynq)
 *
 */


static struct irqaction *hw0_irq_action_primary[AVINTNUM(AVALANCHE_INT_END_PRIMARY)] =
{
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL
};

static struct irqaction *hw0_irq_action_secondary[32] =
{
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*
  This remaps interrupts to exist on other channels than the default
   channels.  essentially we can use the line # as the index for this
   array
*/


static unsigned long line_to_channel[AVINTNUM(AVALANCHE_INT_END_PRIMARY)];
unsigned long uni_secondary_interrupt = 0;

static struct irqaction r4ktimer_action = {
	NULL, 0, 0, "R4000 timer/counter", NULL, NULL,
};

#if defined (CONFIG_MIPS_SEAD2)
static struct irqaction seaduart0_action = {
        NULL, 0, 0, "SEAD UART0 (ttys0)",NULL,NULL,
};


static struct irqaction seaduart1_action = {
        NULL, 0, 0, "SEAD UART1 (ttys1)",NULL,NULL,
};

#endif /* CONFIG_MIPS_SEAD2 */

static struct irqaction *irq_action[8] = {
        NULL,              /* SW int 0 */
        NULL,              /* SW int 1 */
        NULL,              /* HW int 0 */
#if defined(CONFIG_MIPS_SEAD2)
        &seaduart0_action, /* HW int 1 */
        &seaduart1_action, /* HW int 2 */
#else
        NULL,
        NULL,
#endif /*CONFIG_MIPS_SEAD2 */
        NULL,              /* HW int 3 */
        NULL,              /* HW int 4 */
        &r4ktimer_action   /* HW int 5 */
};


void disable_irq(unsigned int irq_nr)
{
        unsigned long flags;
        unsigned long chan_nr=0;
        unsigned long int_bit=0;

	if(irq_nr >= AVALANCHE_INT_END)
	{
		printk("whee, invalid irq_nr %d\n", irq_nr);
		panic("IRQ, you lose...");
	}

	save_and_cli(flags);

        if(irq_nr <  MIPS_EXCEPTION_OFFSET)
        {
            /* disable mips exception */

            int_bit = read_32bit_cp0_register(CP0_STATUS) & ~(1 << (8+irq_nr));
            change_cp0_status(ST0_IM,int_bit);
            restore_flags(flags);
            return;
        }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	/* Vlynq irq_nr are 72-145 in the system and are placed after the interrupts  
	 * managed by the interrupt controller. 
	 */
        if(irq_nr >= AVALANCHE_INTC_END)
	{
	    if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	        /* Vlynq interrupts 32-63 */
                vlynq_interrupt_disable(&vlynqDevice1,VLYNQ_REMOTE_DVC,irq_nr-AVALANCHE_INT_END_LOW_VLYNQ);
	    else
	        /* Vlynq interupts 0-31 */
                vlynq_interrupt_disable(&vlynqDevice0,VLYNQ_REMOTE_DVC,irq_nr-AVALANCHE_INTC_END);

	    goto ret_from_disable_irq;
	}
#endif
	/* The following section pertains to interrupt controller and interpretations
	 * shall be made in this perspective.
	 */
	irq_nr = AVINTNUM(irq_nr);

        /* irq_nr represents the line number for the interrupt.  We must
         * disable the channel number associated with that line number.
         */

        if(irq_nr >= AVALANCHE_INT_END_PRIMARY_REG2)
          chan_nr = irq_nr;
        else
          chan_nr = line_to_channel[irq_nr];

        /* disable the interrupt channel bit */

        /* primary interrupt #'s 0-31 */
        if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
            avalanche_hw0_icregs->intecr1 = (1 << chan_nr);

        /* primary interrupt #'s 32-39 */
        else if ((chan_nr <  AVALANCHE_INT_END_PRIMARY_REG2) &&
		 (chan_nr >= AVALANCHE_INT_END_PRIMARY_REG1))
            avalanche_hw0_icregs->intecr2 = (1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));

        else  /* secondary interrupt #'s 0-31 */
            avalanche_hw0_ecregs->exiecr = (1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG2));

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_disable_irq:
#endif

	restore_flags(flags);
}




void enable_irq(unsigned int irq_nr)
{
        unsigned long flags;
        unsigned long chan_nr=0;
        unsigned long int_bit=0;

	if(irq_nr >= AVALANCHE_INT_END) {
		printk("whee, invalid irq_nr %d\n", irq_nr);
		panic("IRQ, you lose...");
	}

	save_and_cli(flags);


        if(irq_nr <  MIPS_EXCEPTION_OFFSET)
        {
            /* Enable MIPS exceptions */
            int_bit = read_32bit_cp0_register(CP0_STATUS);
            change_cp0_status(ST0_IM,int_bit | (1<<(8+irq_nr)));
            restore_flags(flags);
            return;
        }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	/* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
	 * managed by the interrupt controller. 
	 */
        if(irq_nr >= AVALANCHE_INTC_END)
	{
	    if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	        /* Vlynq interrupts 32-63 */
                vlynq_interrupt_enable(&vlynqDevice1,VLYNQ_REMOTE_DVC,irq_nr-AVALANCHE_INT_END_LOW_VLYNQ);
	    else
	        /* Vlynq interupts 0-31 */
                vlynq_interrupt_enable(&vlynqDevice0,VLYNQ_REMOTE_DVC,irq_nr-AVALANCHE_INTC_END);

	    goto ret_from_enable_irq;
	}
#endif
	/* The following section pertains to interrupt controller and interpretations
	 * shall be made in this perspective.
	 */
	irq_nr = AVINTNUM(irq_nr);

        /* irq_nr represents the line number for the interrupt.  We must
         * disable the channel number associated with that line number.
         */

        if(irq_nr >= AVALANCHE_INT_END_PRIMARY_REG2)
            chan_nr = irq_nr;
        else
            chan_nr = line_to_channel[irq_nr];

        /* enable the interrupt channel  bit */

        /* primary interrupt #'s 0-31 */
        if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
            avalanche_hw0_icregs->intesr1 = 1 << chan_nr;

        /* primary interrupt #'s 32 throuth 39 */
        else if ((chan_nr < AVALANCHE_INT_END_PRIMARY_REG2) &&
		 (chan_nr >= AVALANCHE_INT_END_PRIMARY_REG1))
            avalanche_hw0_icregs->intesr2 = (1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));

        /* secondary interrupt #'s 0-31 */
        else 
            avalanche_hw0_ecregs->exiesr = (1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG2));

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_enable_irq:
#endif

	restore_flags(flags);
}

static unsigned int startup_avalanche_irq(unsigned int irq)
{
	enable_irq(irq);
	return 0; /* never anything pending */
}

#define shutdown_avalanche_irq	     disable_irq
#define mask_and_ack_avalanche_irq   disable_irq

static void end_avalanche_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		enable_irq(irq);
}

static struct hw_interrupt_type avalanche_irq_type = {
	"TI AVALANCHE",
	startup_avalanche_irq,
	shutdown_avalanche_irq,
	enable_irq,
	disable_irq,
	mask_and_ack_avalanche_irq,
	end_avalanche_irq,
	NULL
};


int get_irq_list(char *buf)
{
	int i, len = 0;
        int num = 0;
	struct irqaction *action;

        /* MIPS exceptions */
	for (i = 0; i < MIPS_EXCEPTION_OFFSET; i++, num++)
	{
	    action = irq_action[i];

	    if (!action)
	        continue;

	    len += sprintf(buf+len, "%3d: %8d %c %s", num, kstat.irqs[0][num],
			   (action->flags & SA_INTERRUPT) ? '+' : ' ', action->name);

	    for (action=action->next; action; action = action->next) {
	        len += sprintf(buf+len, ",%s %s", (action->flags & SA_INTERRUPT) ? " +" : "",
			       action->name);
	    }

	    len += sprintf(buf+len, " [MIPS interrupt]\n");
	}


        /* hw0 interrupts */
        for (i = 0; i < AVINTNUM(AVALANCHE_INTC_END); i++,num++)
        {
            if(i < AVINTNUM(AVALANCHE_INT_END_PRIMARY))
                action = hw0_irq_action_primary[i];
            else
                action = hw0_irq_action_secondary[i-AVINTNUM(AVALANCHE_INT_END_PRIMARY)];

            if (!action)
	        continue;

	    len += sprintf(buf+len, "%3d: %8d %c %s", num, kstat.irqs[0][ LNXINTNUM(i) ],
			   (action->flags & SA_INTERRUPT) ? '+' : ' ', action->name);

	    for (action=action->next; action; action = action->next)
	    {
	        len += sprintf(buf+len, ",%s %s", (action->flags & SA_INTERRUPT) ? " +" : "",
		               action->name);
	    }

            if(i < AVINTNUM(AVALANCHE_INT_END_PRIMARY))
                len += sprintf(buf+len, " [hw0 (Avalanche Primary)]\n");
            else
                len += sprintf(buf+len, " [hw0 (Avalanche Secondary)]\n");
	}

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
        /* vlynq interrupts */
        for (i = AVALANCHE_INTC_END; i < AVALANCHE_INT_END; i++,num++)
        {
            VLYNQ_DEV *p;

            int irq;

            if(i >= AVALANCHE_INT_END_LOW_VLYNQ)
            {
                irq  = i - AVALANCHE_INT_END_LOW_VLYNQ;
                p    = &vlynqDevice1;
            }

            else
            {
                irq = i - AVALANCHE_INTC_END; 
                p = &vlynqDevice0;
            } 
	    
            
            action = irq_desc[i].action;

            if (!action)
                continue;

            len += sprintf(buf+len, "%3d: %8d %c %s", num, vlynq_interrupt_get_count(p, irq), 
                           (action->flags & SA_INTERRUPT) ? '+' : ' ', action->name);

            for (action=action->next; action; action = action->next)
            {
                len += sprintf(buf+len, ",%s %s", (action->flags & SA_INTERRUPT) ? " +" : "",
                               action->name);
            }

            if(i < AVALANCHE_INT_END_LOW_VLYNQ)
                len += sprintf(buf+len, " [hw0 (Low Vlynq)]\n");
            else
                len += sprintf(buf+len, " [hw0 (High Vlynq)]\n");
        }

#endif
	return len;
}


int request_irq(unsigned int irq, void (*handler)(int, void *, struct pt_regs *),
		unsigned long irqflags, const char * devname, void *dev_id)
{
        struct irqaction *action;

        /* At this time, we do not check for any irqflags and frankly do not 
	 * envisage shared interrupts. Support for complete shared interrupts
	 * requires some work.
	 */

	/* Unified secondary interrupt is internal to the interrupt controller
	 * by virtue of hardware design. All the secondary interrupts are fed
	 * into the primary interrupt map by unified secondary interrupt, we 
	 * do not want any body to over ride that. Will some body require that
	 * except for debugging purposes. 
         */

        if (irq >=  AVALANCHE_INT_END || irq == LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT))
        {
	    printk("Error: Trying to register %s IRQ %d.\n", 
	           (irq == LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT)) ? \
		   "unified secondary" : "out of range", irq); 
	    return -EINVAL;
        }

	if (!handler)
        {
	    printk("Error: No handler specified for IRQ %d.\n", irq); 
	    return -EINVAL;
        }

	action = (struct irqaction *)kmalloc(sizeof(struct irqaction), GFP_KERNEL);
	if(!action)
        {
	    printk("Error: Could not allocate memory for IRQ %d.\n", irq);
	    return -ENOMEM;
        }

	action->handler = handler;
	action->flags = irqflags;
	action->mask = 0;
	action->name = devname;
	action->dev_id = dev_id;
	action->next = 0;

	if(irq_desc[irq].action)
	    action->next = irq_desc[irq].action;

	irq_desc[irq].action = action;

        if(irq <  MIPS_EXCEPTION_OFFSET)
        {
            irq_action[irq] = action;
            enable_irq(irq);
            return 0;
        }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	/* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
	 * managed by the interrupt controller. 
	 */
        if(irq >= AVALANCHE_INTC_END)
	{
	    if(irq >= AVALANCHE_INT_END_LOW_VLYNQ)
	        /* Vlynq interrupts 32-63 */
                vlynq_install_isr(&vlynqDevice1, irq - AVALANCHE_INT_END_LOW_VLYNQ, 
				  handler, irq, dev_id, NULL);
	    else
	        /* Vlynq interupts 0-31 */
                vlynq_install_isr(&vlynqDevice0, irq - AVALANCHE_INTC_END, 
				  handler, irq, dev_id, NULL);

	    goto ret_from_request_irq;
	}
#endif

        if(irq < AVALANCHE_INT_END_PRIMARY)
            hw0_irq_action_primary[line_to_channel[AVINTNUM(irq)]] = action;
        else
            hw0_irq_action_secondary[irq - AVALANCHE_INT_END_PRIMARY] = action;

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_request_irq:
#endif

	enable_irq(irq);

	return 0;
}


void free_irq(unsigned int irq, void *dev_id)
{
	struct irqaction *action;

	if (irq >= AVALANCHE_INT_END || irq == LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT)) 
        {
	    printk("Error: Trying to free %s IRQ %d \n",
		    (irq == LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT)) ? \
                    "unified secondary": "out of range", irq);
	    return;
	}

        if(irq < MIPS_EXCEPTION_OFFSET)
        {
            action = irq_action[irq];
            irq_action[irq] = NULL;
            irq_desc[irq].action = NULL;
            disable_irq(irq);
            kfree(action);
            return;
        }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	/* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
	 * managed by the interrupt controller. 
	 */
        if(irq >= AVALANCHE_INTC_END)
	{
	    if(irq >= AVALANCHE_INT_END_LOW_VLYNQ)
	        /* Vlynq interrupts 32-63 */
                vlynq_uninstall_isr(&vlynqDevice1, irq - AVALANCHE_INT_END_LOW_VLYNQ, irq, dev_id, NULL);
	    else
	        /* Vlynq interupts 0-31 */
                vlynq_uninstall_isr(&vlynqDevice0, irq - AVALANCHE_INTC_END, irq, dev_id, NULL);

            action = irq_desc[irq].action;

	    goto ret_from_free_irq;
	}
#endif


        if(irq < AVALANCHE_INT_END_PRIMARY) 
        {
            action = hw0_irq_action_primary[line_to_channel[AVINTNUM(irq)]];
            hw0_irq_action_primary[line_to_channel[AVINTNUM(irq)]] = NULL;
        }
        else 
        {
            action = hw0_irq_action_secondary[irq - AVALANCHE_INT_END_PRIMARY_REG2];
            hw0_irq_action_secondary[irq - AVALANCHE_INT_END_PRIMARY_REG2] = NULL;
        }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_free_irq:
#endif
        if(!action)
            return;

        irq_desc[irq].action = NULL;
	disable_irq(irq);
	kfree(action);
}

void avalanche_unified_secondary_irq_handler(int unified_sec_irq, void * dev_id,
                                             struct pt_regs *regs)
{
    int irq = AVALANCHE_INT_END_PRIMARY;
    unsigned int status = avalanche_hw0_ecregs->exsr;
    int cpu = smp_processor_id(); ;

    /* clear secondary interrupt */
    avalanche_hw0_ecregs->excr = status;

    while(status)
    {
        if(status & 0x1)
        {
            struct irqaction *action =  hw0_irq_action_secondary[irq-AVALANCHE_INT_END_PRIMARY];
            if(!action)
	        return;

            irq_enter(cpu,AVINTNUM(irq));
            kstat.irqs[0][irq]++;
            action->handler(irq, action->dev_id, regs);
            irq_exit(cpu,AVINTNUM(irq));
        }

        status >>= 1;
        irq++;
    }
}

#ifdef CONFIG_REMOTE_DEBUG
extern void breakpoint(void);
extern int remote_debug;
#endif

void init_IRQ(void) __init;
void init_IRQ(void)
{
        int i,j;
        struct irqaction *action;

        avalanche_hw0_icregs = (struct avalanche_ictrl_regs *)AVALANCHE_ICTRL_REGS_BASE;
        avalanche_hw0_ecregs = (struct avalanche_exctrl_regs *)AVALANCHE_ECTRL_REGS_BASE;

#ifdef CONFIG_AVALANCHE_INTC_PACING
        avalanche_hw0_ipaceregs = (struct avalanche_ipace_regs *)AVALANCHE_IPACE_REGS_BASE;
#endif

        avalanche_hw0_chregs = (struct avalanche_channel_int_number *)AVALANCHE_CHCTRL_REGS_BASE;

        /*  Disable interrupts and clear pending
         */

        avalanche_hw0_icregs->intecr1 = 0xffffffff;    /* disable interrupts 0:31  */
        avalanche_hw0_icregs->intcr1  = 0xffffffff;    /* clear interrupts 0:31    */
        avalanche_hw0_icregs->intecr2 = 0xff;          /* disable interrupts 32:39 */
        avalanche_hw0_icregs->intcr2  = 0xff;          /* clear interrupts 32:39   */
        avalanche_hw0_ecregs->exiecr  = 0xffffffff;    /* disable secondary interrupts 0:31 */
        avalanche_hw0_ecregs->excr    = 0xffffffff;    /* clear secondary interrupts 0:31 */

#ifdef CONFIG_AVALANCHE_INTC_PACING
	avalanche_hw0_ipaceregs->ipacep = (avalanche_get_vbus_freq()/1000000)*4;
#endif
        /* Channel to line mapping, Line to Channel mapping */
        for(i = MIPS_EXCEPTION_OFFSET, j =0; i < AVALANCHE_INT_END_PRIMARY; i++, j++)
            avalanche_int_set(j,i);


	/* Now safe to set the exception vector. */
	set_except_vector(0, mipsIRQ);

        /* Setup the IRQ description array.  These will be mapped
         *  as flat interrupts numbers.  The mapping is as follows
         *
         *           0 - 7    MIPS CPU Exceptions  (HW/SW)
         *           8 - 47   Primary Interrupts   (Avalanche)
         *           48- 79   Secondary Interrupts (Avalanche)
	 *           80- 111  Low Vlynq Interrupts (Vlynq)
	 *          112- 143  High Vlynq Interrupts(Vlynq)
         */

	for (i = 0; i < AVALANCHE_INT_END; i++)
        {
            irq_desc[i].status	= IRQ_DISABLED;
            irq_desc[i].action	= 0;
            irq_desc[i].depth	= 1;
            irq_desc[i].handler	= &avalanche_irq_type;
        }

	/* Unified secondary interrupt is internal to the interrupt controller
	 * by virtue of hardware design. All the secondary interrupts are fed
	 * into the primary interrupt map by unified secondary interrupt, we 
	 * do not want any body to over ride that. Will some body require that
	 * except for debugging purposes. So, we add our internal unified 
         * secondary handler.
         *
	 * Adding for unified secondary interrupt handler. */
	action = &unified_secondary_action;
	if(!action)
	    return;

	action->handler = avalanche_unified_secondary_irq_handler;
	action->flags = 0;
	action->mask = 0;
	action->name = "unified secondary";
	action->dev_id = NULL;
	action->next = 0;

        irq_desc[LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT)].action = action;
        hw0_irq_action_primary[line_to_channel[AVALANCHE_UNIFIED_SECONDARY_INT]] = action;

#ifdef CONFIG_REMOTE_DEBUG
	if (remote_debug)
        {
            set_debug_traps();
            breakpoint();
        }
#endif

}

void avalanche_hw0_irqdispatch(struct pt_regs *regs)
{
    	struct irqaction *action;
	int cpu = smp_processor_id();
        int chan_nr=0, irq=0;
	unsigned int priority_irq_info = avalanche_hw0_icregs->pintir;

	/* This function refers to irqs in the interrupt controller perspective. */
	irq     = (priority_irq_info >> 16) & 0x3F;
	chan_nr = priority_irq_info & 0x3F;

	if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
	    avalanche_hw0_icregs->intcr1 = (1 << chan_nr);
	else if (chan_nr < AVALANCHE_INT_END_PRIMARY_REG2 )
            avalanche_hw0_icregs->intcr2 = 1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1);
	else
	    return;

//        irq = chan_nr;
        action =  hw0_irq_action_primary[chan_nr];

	if(action)
	{
            irq_enter(cpu,irq);
            kstat.irqs[0][LNXINTNUM(irq)]++;
            action->handler(LNXINTNUM(irq), action->dev_id, regs);
            irq_exit(cpu,irq);
	}
	else
	{
	    printk("No handler for hw0 irq: %i\n", irq);
	}

    
        if(softirq_pending(cpu))
            do_softirq();

	return;
}

#if defined(CONFIG_MIPS_SEAD2)
/* SEAD uart 0 interrupt */

void sead_uart0_irqdispatch(struct pt_regs *regs)
{
        struct irqaction *action;
        int irq = 3;
        int cpu = smp_processor_id();


        action = irq_action[irq];

        irq_enter(cpu,irq);
        kstat.irqs[0][irq]++;
        action->handler(irq, action->dev_id, regs);
        irq_exit(cpu,irq);
}

/* SEAD uart 1 interrupt */

void sead_uart1_irqdispatch(struct pt_regs *regs)
{
        struct irqaction *action;
        int irq = 4;
        int cpu = smp_processor_id();

        action = irq_action[irq];

        irq_enter(cpu,irq);
        kstat.irqs[0][irq]++;
        action->handler(irq, action->dev_id, regs);
        irq_exit(cpu,irq);
}
#endif /* CONFIG_MIPS_SEAD2 */


unsigned long probe_irq_on (void)
{
	return 0;
}


int probe_irq_off (unsigned long irqs)
{
	return 0;
}

void avalanche_int_set(int channel, int line)
{
    if(line <  MIPS_EXCEPTION_OFFSET ||
       line >= AVALANCHE_INT_END_PRIMARY ||
       line == LNXINTNUM(AVALANCHE_UNIFIED_SECONDARY_INT)||
       channel == 0)        
        return;

  line = AVINTNUM(line);

  switch(channel)
  {
      case(0):
        /* Frankly, we will not hit this case, as we do not want to change the 
         * secondary default channel, we are ensuring this with above check.
         */
        avalanche_hw0_chregs->cintnr0 =  line;
        break;
      case(1):
        avalanche_hw0_chregs->cintnr1 =  line;
        break;
      case(2):
        avalanche_hw0_chregs->cintnr2 =  line;
        break;
      case(3):
        avalanche_hw0_chregs->cintnr3 =  line;
        break;
      case(4):
        avalanche_hw0_chregs->cintnr4 =  line;
        break;
      case(5):
        avalanche_hw0_chregs->cintnr5 =  line;
        break;
      case(6):
        avalanche_hw0_chregs->cintnr6 =  line;
        break;
      case(7):
        avalanche_hw0_chregs->cintnr7 =  line;
        break;
      case(8):
        avalanche_hw0_chregs->cintnr8 =  line;
        break;
      case(9):
        avalanche_hw0_chregs->cintnr9 =  line;
        break;
      case(10):
        avalanche_hw0_chregs->cintnr10 = line;
        break;
      case(11):
        avalanche_hw0_chregs->cintnr11 = line;
        break;
      case(12):
        avalanche_hw0_chregs->cintnr12 = line;
        break;
      case(13):
        avalanche_hw0_chregs->cintnr13 = line;
        break;
      case(14):
        avalanche_hw0_chregs->cintnr14 = line;
        break;
      case(15):
        avalanche_hw0_chregs->cintnr15 = line;
        break;
      case(16):
        avalanche_hw0_chregs->cintnr16 = line;
        break;
      case(17):
        avalanche_hw0_chregs->cintnr17 = line;
        break;
      case(18):
        avalanche_hw0_chregs->cintnr18 = line;
        break;
      case(19):
        avalanche_hw0_chregs->cintnr19 = line;
        break;
      case(20):
        avalanche_hw0_chregs->cintnr20 = line;
        break;
      case(21):
        avalanche_hw0_chregs->cintnr21 = line;
        break;
      case(22):
        avalanche_hw0_chregs->cintnr22 = line;
        break;
      case(23):
        avalanche_hw0_chregs->cintnr23 = line;
        break;
      case(24):
        avalanche_hw0_chregs->cintnr24 = line;
        break;
      case(25):
        avalanche_hw0_chregs->cintnr25 = line;
        break;
      case(26):
        avalanche_hw0_chregs->cintnr26 = line;
        break;
      case(27):
        avalanche_hw0_chregs->cintnr27 = line;
        break;
      case(28):
        avalanche_hw0_chregs->cintnr28 = line;
        break;
      case(29):
        avalanche_hw0_chregs->cintnr29 = line;
        break;
      case(30):
        avalanche_hw0_chregs->cintnr30 = line;
        break;
      case(31):
        avalanche_hw0_chregs->cintnr31 = line;
        break;
      case(32):
        avalanche_hw0_chregs->cintnr32 = line;
        break;
      case(33):
        avalanche_hw0_chregs->cintnr33 = line;
        break;
      case(34):
        avalanche_hw0_chregs->cintnr34 = line;
        break;
      case(35):
        avalanche_hw0_chregs->cintnr35 = line;
        break;
      case(36):
        avalanche_hw0_chregs->cintnr36 = line;
        break;
      case(37):
        avalanche_hw0_chregs->cintnr37 = line;
        break;
      case(38):
        avalanche_hw0_chregs->cintnr38 = line;
        break;
      case(39):
        avalanche_hw0_chregs->cintnr39 = line;
        break;
      default:
        printk("Error: Unknown Avalanche interrupt channel\n");
  }

  line_to_channel[line] = channel; 
}

#ifdef CONFIG_AVALANCHE_INTC_PACING

#define AVALANCHE_MAX_PACING_BLK   3
#define AVALANCHE_PACING_LOW_VAL   2
#define AVALANCHE_PACING_HIGH_VAL 63

int avalanche_request_intr_pacing(int irq_nr, unsigned int blk_num,
		             unsigned int pace_value)
{
    unsigned int  blk_offset;
    unsigned long flags;

    if(irq_nr <  MIPS_EXCEPTION_OFFSET ||
       irq_nr >= AVALANCHE_INT_END_PRIMARY)
        return (-1);

    if(blk_num > AVALANCHE_MAX_PACING_BLK)
        return(-1);

    if(pace_value > AVALANCHE_PACING_HIGH_VAL &&
       pace_value < AVALANCHE_PACING_LOW_VAL)
	return(-1);

    blk_offset = blk_num*8;

    save_and_cli(flags);

    /* disable the interrupt pacing, if enabled previously */
    avalanche_hw0_ipaceregs->ipacemax &= ~(0xff << blk_offset);

    /* clear the pacing map */
    avalanche_hw0_ipaceregs->ipacemap &= ~(0xff << blk_offset);

    /* setup the new values */
    avalanche_hw0_ipaceregs->ipacemap |= ((AVINTNUM(irq_nr))   << blk_offset);
    avalanche_hw0_ipaceregs->ipacemax |= ((0x80 | pace_value)  << blk_offset);

    restore_flags(flags);

    return(0);
}

int avalanche_free_intr_pacing(unsigned int blk_num)
{
    unsigned int  blk_offset;
    unsigned long flags;

    if(blk_num > AVALANCHE_MAX_PACING_BLK)
        return(-1);

    blk_offset = blk_num*8;

    save_and_cli(flags);

    /* disable the interrupt pacing, if enabled previously */
    avalanche_hw0_ipaceregs->ipacemax &= ~(0xff << blk_offset);

    /* clear the pacing map */
    avalanche_hw0_ipaceregs->ipacemap &= ~(0xff << blk_offset);

    restore_flags(flags);

    return(0);
}

int avalanche_request_pacing(int irq_nr, unsigned int blk_num, unsigned int pace_val)
{
    return(avalanche_request_intr_pacing(irq_nr,blk_num,pace_val));
}


#endif

int avalanche_intr_polarity_set(unsigned int irq_nr, unsigned long polarity_val)
{
    unsigned long flags;
    unsigned long chan_nr=0;

    if(irq_nr  <  MIPS_EXCEPTION_OFFSET ||
       irq_nr  >= AVALANCHE_INT_END ||
       (irq_nr >= AVALANCHE_INT_END_PRIMARY && 
	irq_nr <  AVALANCHE_INT_END_SECONDARY))
    {
        printk("whee, invalid irq_nr %d\n", irq_nr);
#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	printk("Not one of the primary or vlynq avalanche interrupts.\n");
#else
        printk("Not one of the primary avalanche interrupts\n");
#endif
	panic("IRQ, you lose...");
	return(-1);
    }

    if(polarity_val > 1)
    {
        printk("Not a valid polarity value.\n");
	return(-1);
    }


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    /* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
     * managed by the interrupt controller. 
     */
    if(irq_nr >= AVALANCHE_INTC_END)
    {
        if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	    /* Vlynq interrupts 32-63 */
            vlynq_interrupt_set_polarity(&vlynqDevice1, VLYNQ_REMOTE_DVC, 
			                 irq_nr - AVALANCHE_INT_END_LOW_VLYNQ, polarity_val);
	else
	    /* Vlynq interupts 0-31 */
            vlynq_interrupt_set_polarity(&vlynqDevice0, VLYNQ_REMOTE_DVC, 
			                 irq_nr - AVALANCHE_INTC_END, polarity_val);

	goto ret_from_set_polarity;
    }
#endif

    irq_nr = AVINTNUM(irq_nr);

    chan_nr = line_to_channel[irq_nr];

    save_and_cli(flags);

    /* primary interrupt #'s 0-31 */
    if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
    {
        if(polarity_val)
            avalanche_hw0_icregs->intpolr1 |=  (1 << chan_nr);
        else
	    avalanche_hw0_icregs->intpolr1 &= ~(1 << chan_nr);
    }
    /* primary interrupt #'s 32 throuth 39 */
    else
    {
	if(polarity_val)
	    avalanche_hw0_icregs->intpolr2 |=
		(1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));
	else
	    avalanche_hw0_icregs->intpolr2 &=
		~(1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));
    }

    restore_flags(flags);

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_set_polarity:
#endif

    return(0);
}


int avalanche_intr_polarity_get(unsigned int irq_nr)
{
    unsigned long flags;
    unsigned long chan_nr=0;
    int           value;

    if(irq_nr  <  MIPS_EXCEPTION_OFFSET ||
       irq_nr  >= AVALANCHE_INT_END ||
       (irq_nr >= AVALANCHE_INT_END_PRIMARY && 
	irq_nr <  AVALANCHE_INT_END_SECONDARY))
    {
        printk("whee, invalid irq_nr %d\n", irq_nr);
#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
	printk("Not one of the primary or vlynq avalanche interrupts.\n");
#else
        printk("Not one of the primary avalanche interrupts\n");
#endif
        panic("IRQ, you lose...");
        return(-1);
    }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    /* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
     * managed by the interrupt controller. 
     */
    if(irq_nr >= AVALANCHE_INTC_END)
    {
        if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	    /* Vlynq interrupts 32-63 */
            value = vlynq_interrupt_get_polarity(&vlynqDevice1, VLYNQ_REMOTE_DVC, 
			                         irq_nr - AVALANCHE_INT_END_LOW_VLYNQ);
	else
	    /* Vlynq interupts 0-31 */
            value = vlynq_interrupt_get_polarity(&vlynqDevice0, VLYNQ_REMOTE_DVC, 
			                         irq_nr - AVALANCHE_INTC_END);

	goto ret_from_get_polarity;
    }
#endif

    irq_nr = AVINTNUM(irq_nr);

    chan_nr = line_to_channel[AVINTNUM(irq_nr)];

    save_and_cli(flags);

    /* primary interrupt #'s 0-31 */
    if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
    {
        value = avalanche_hw0_icregs->intpolr1;
        value = (value >> chan_nr) & 0x1;
    }
    /* primary interrupt #'s 32 throuth 39 */
    else
    {
        value = avalanche_hw0_icregs->intpolr2;
        value = (value >> (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1)) & 0x1;
    }

    restore_flags(flags);

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_get_polarity:
#endif

    return(value);
}

#if defined(CONFIG_AVALANCHE_INTC_TYPE)

/* Sets the trigger type: edge or level */
int avalanche_intr_type_set(unsigned int irq_nr, unsigned long type_val)
{
    unsigned long flags;
    unsigned long chan_nr=0;

    if(irq_nr  <  MIPS_EXCEPTION_OFFSET ||
       irq_nr  >= AVALANCHE_INT_END ||
       (irq_nr >= AVALANCHE_INT_END_PRIMARY && 
	irq_nr <  AVALANCHE_INT_END_SECONDARY))
    {
        printk("whee, invalid irq_nr %d\n", irq_nr);
	panic("IRQ, you lose...");
	return(-1);
    }

    if(type_val > 1)
    {
        printk("Not a valid polarity value.\n");
	return(-1);
    }

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
    /* Vlynq irq_nr are 80-143 in the system and are placed after the interrupts  
     * managed by the interrupt controller. 
     */
    if(irq_nr >= AVALANCHE_INTC_END)
    {
	/* Type values for VLYNQ are INTC are different. */
        if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	    /* Vlynq interrupts 32-63 */
            vlynq_interrupt_set_type(&vlynqDevice1, VLYNQ_REMOTE_DVC, 
			             irq_nr - AVALANCHE_INT_END_LOW_VLYNQ, !type_val);
	else
	    /* Vlynq interupts 0-31 */
            vlynq_interrupt_set_type(&vlynqDevice0, VLYNQ_REMOTE_DVC, 
			             irq_nr - AVALANCHE_INTC_END, !type_val);

	goto ret_from_set_type;
    }
#endif

    irq_nr = AVINTNUM(irq_nr);

    chan_nr = line_to_channel[AVINTNUM(irq_nr)];

    save_and_cli(flags);

    /* primary interrupt #'s 0-31 */
    if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
    {
        if(type_val)
            avalanche_hw0_icregs->inttypr1 |=  (1 << chan_nr);
        else
	    avalanche_hw0_icregs->inttypr1 &= ~(1 << chan_nr);
    }
    /* primary interrupt #'s 32 throuth 39 */
    else
    {
	if(type_val)
	    avalanche_hw0_icregs->inttypr2 |=
		(1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));
	else
	    avalanche_hw0_icregs->inttypr2 &=
		~(1 << (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1));
    }

    restore_flags(flags);

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)
ret_from_set_type:
#endif

    return(0);
}

/* reads the configured trigger type. */
int avalanche_intr_type_get(unsigned int irq_nr)
{
    unsigned long flags;
    unsigned long chan_nr=0;
    int value;

    if(irq_nr  <  MIPS_EXCEPTION_OFFSET ||
       irq_nr  >= AVALANCHE_INT_END ||
       (irq_nr >= AVALANCHE_INT_END_PRIMARY && 
	irq_nr <  AVALANCHE_INT_END_SECONDARY))
    {
        printk("whee, invalid irq_nr %d\n", irq_nr);
	panic("IRQ, you lose...");
	return(-1);
    }


#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)

    if(irq_nr >= AVALANCHE_INTC_END)
    {
        if(irq_nr >= AVALANCHE_INT_END_LOW_VLYNQ)
	    /* Vlynq interrupts 32-63 */
            value = !vlynq_interrupt_get_type(&vlynqDevice1, VLYNQ_REMOTE_DVC, 
			                     irq_nr - AVALANCHE_INT_END_LOW_VLYNQ);
	else
	    /* Vlynq interupts 0-31 */
            value = !vlynq_interrupt_get_type(&vlynqDevice0, VLYNQ_REMOTE_DVC, 
			                     irq_nr - AVALANCHE_INTC_END);

	goto ret_from_get_type;
    }
#endif

    irq_nr = AVINTNUM(irq_nr);

    chan_nr = line_to_channel[AVINTNUM(irq_nr)];

    save_and_cli(flags);

    /* primary interrupt #'s 0-31 */
    if(chan_nr < AVALANCHE_INT_END_PRIMARY_REG1)
    {
        value = avalanche_hw0_icregs->inttypr1;
        value = (value >> chan_nr) & 0x1;
    }
    /* primary interrupt #'s 32 throuth 39 */
    else
    {
        value = avalanche_hw0_icregs->inttypr2;
        value = (value >> (chan_nr - AVALANCHE_INT_END_PRIMARY_REG1)) & 0x1;
    }

    restore_flags(flags);

#if defined (CONFIG_MIPS_AVALANCHE_VLYNQ)    
ret_from_get_type:
#endif

    return value;
}
#endif

/* this space for rent */

int do_print_developers(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf+len, "\n");
    len += sprintf(buf+len, "\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "Salt Lake City Kernel Developement team (1999-2000)\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "Jeff Harrell		- Kernel ports, ADSL driver\n");
    len += sprintf(buf+len, "Brady Brown		- filesystem, kernel port\n");
    len += sprintf(buf+len, "Mike Hassler		- filesystem, timers\n");
    len += sprintf(buf+len, "Victor Wells		- Ram disk image, EMAC driver\n");
    len += sprintf(buf+len, "Kevin Mcdonald		- f/s, ramdisk compression\n");
    len += sprintf(buf+len, "Rosemary Rogers		- kitchen gnomes\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "Germantown Kernel Developement team (2001- )\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "Jeff Harrell		- Kernel ports, ADSL driver\n");
    len += sprintf(buf+len, "Victor Wells		- VMAC driver\n");
    len += sprintf(buf+len, "James Beaulieu		- VMAC driver, PCI\n");
    len += sprintf(buf+len, "Hari Krishna		- USB driver\n");
    len += sprintf(buf+len, "Boris Marenkov		- USB driver\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "India Kernel Development Team (2002-)\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "Nitin Dhingra		- Intc, Avalanche\n");
    len += sprintf(buf+len, "Suraj Iyer			- Intc, SEAD2, AR7 Port (2.4.17)\n");
    len += sprintf(buf+len, "Suraj Iyer			- CPMAC (2.4.17)\n");
    len += sprintf(buf+len, "Sharath Kumar		- SEAD2, AR7 Port (2.4.17)\n");
    len += sprintf(buf+len, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf+len, "\n");
    len += sprintf(buf+len, "\n");
    return len;
}

void avalanche_proc_entries(void)
{
    create_proc_read_entry("avalanche/developers",0,NULL,do_print_developers,NULL);
}

#if defined(CONFIG_AVALANCHE_INTC_TYPE)
EXPORT_SYMBOL(avalanche_intr_type_set);
EXPORT_SYMBOL(avalanche_intr_type_get);
#endif

#if defined(CONFIG_AVALANCHE_INTC_PACING)
EXPORT_SYMBOL(avalanche_request_intr_pacing);
EXPORT_SYMBOL(avalanche_free_intr_pacing);
EXPORT_SYMBOL(avalanche_request_pacing);
#endif

EXPORT_SYMBOL(avalanche_intr_polarity_set);
EXPORT_SYMBOL(avalanche_intr_polarity_get);
