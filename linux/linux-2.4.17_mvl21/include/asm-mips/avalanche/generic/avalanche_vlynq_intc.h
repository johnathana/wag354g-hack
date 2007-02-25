#ifndef _AVALANCHE_VLYNQ_INTC_H_
#define _AVALANCHE_VLYNQ_INTC_H_

#include <linux/config.h>
#include <asm/irq.h>
#include <asm/avalanche/generic/pal_vlynq.h>
#include <asm/avalanche/generic/avalanche_intc.h>

int avalanche_vlynq_get_sys_irq_num(PAL_VLYNQ_HND *vlynq, int irq);

#define VLYNQINTNUM(vlynq, irq) avalanche_vlynq_get_sys_irq_num(vlynq, irq)

void avalanche_vlynq_disable_irq(unsigned int irq);
void avalanche_vlynq_enable_irq(unsigned int irq);

int avalanche_vlynq_request_irq(unsigned int irq, void (*handler)(int, void *, struct pt_regs *),
		                unsigned long irqflags, const char * devname, void *dev_id);
void avalanche_vlynq_free_irq(unsigned int irq, void *dev_id);

int avalanche_vlynq_set_irq_polarity(unsigned int irq, unsigned int pol);
int avalanche_vlynq_get_irq_polarity(unsigned int irq);

int avalanche_vlynq_set_irq_type(unsigned int irq, unsigned val);
int avalanche_vlynq_get_irq_type(unsigned int irq);

int avalanche_vlynq_irq_list(char *buf);

#endif
