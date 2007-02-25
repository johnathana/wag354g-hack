/*
 * BK Id: SCCS/s.i8259.h 1.9 11/04/01 21:07:38 paulus
 */

#ifndef _PPC_KERNEL_i8259_H
#define _PPC_KERNEL_i8259_H

#include <linux/irq.h>

extern struct hw_interrupt_type i8259_pic;

void i8259_init(void);
int i8259_irq(int);

#endif /* _PPC_KERNEL_i8259_H */
