/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Board specific pci fixups.
 *
 * Copyright 2001 MontaVista Software Inc.
 * Author: MontaVista Software, Inc.
 *         	stevel@mvista.com or source@mvista.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/config.h>

#ifdef CONFIG_PCI

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/rc32300/rc32300.h>

void __init pcibios_fixup_resources(struct pci_dev *dev)
{
}

#if 0

static void pci_master_error_intr(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("RC32334 PCI master %s error\n", irq == GROUP11_IRQ_BASE+0 ?
		"write" : "read");
}

static void pci_parity_intr(int irq, void *dev_id, struct pt_regs *regs)
{
	printk("RC32334 PCI %s parity error\n", irq == GROUP11_IRQ_BASE+2 ?
	       "master" : "target");
}

static int __init init_pci_interrupts(void)
{
	request_irq(GROUP11_IRQ_BASE+0, pci_master_error_intr, SA_INTERRUPT,
		    "RC32334 PCI Master Write Error", NULL);
 	request_irq(GROUP11_IRQ_BASE+1, pci_master_error_intr, SA_INTERRUPT,
		    "RC32334 PCI Master Read Error", NULL);
 	request_irq(GROUP11_IRQ_BASE+2, pci_parity_intr, SA_INTERRUPT,
		    "RC32334 PCI Master Parity Error", NULL);
 	request_irq(GROUP11_IRQ_BASE+3, pci_parity_intr, SA_INTERRUPT,
		    "RC32334 PCI Target Parity Error", NULL);
}

static void __exit cleanup_pci_interrupts(void)
{
	free_irq(GROUP11_IRQ_BASE+0, NULL);
	free_irq(GROUP11_IRQ_BASE+1, NULL);
	free_irq(GROUP11_IRQ_BASE+2, NULL);
	free_irq(GROUP11_IRQ_BASE+3, NULL);
}

module_init(init_pci_interrupts);
module_exit(cleanup_pci_interrupts);

#endif

void __init pcibios_fixup(void)
{
	extern void (*ibe_board_handler)(struct pt_regs *regs);
	extern void (*dbe_board_handler)(struct pt_regs *regs);
	extern void rc32334_be_handler(struct pt_regs *regs);
	extern void rc32300_ack_irq(unsigned int irq_nr);

	ibe_board_handler = dbe_board_handler = rc32334_be_handler;

	// ack any bus errors
	rc32300_ack_irq(GROUP4_IRQ_BASE+4); // ack timer 4 rollover intr
	rc32300_ack_irq(GROUP4_IRQ_BASE+5); // ack timer 5 rollover intr
	rc32300_ack_irq(GROUP1_IRQ_BASE);   // ack bus error intr

	/*
	 * Enable CPU and IP Bus Error exceptions, and disable WatchDog.
	 */
	rc32300_outl(0x18, CPU_IP_BUSERR_CNTL);
}

void __init pcibios_fixup_irqs(void)
{
	struct pci_dev *dev;

	pci_for_each_dev(dev) {
		unsigned int slot;

		if (dev->bus->number != 0) {
			return;
		}

		slot = PCI_SLOT(dev->devfn);
		
		if (slot > 0 && slot <= 4) {
			unsigned char pin;
			pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);

			switch (pin) {
			case 1: /* pin A */
				dev->irq = RC32334_PCI_INTA_IRQ;
				break;
			case 2: /* pin B */
				dev->irq = RC32334_PCI_INTB_IRQ;
				break;
			case 3: /* pin C */
				dev->irq = RC32334_PCI_INTC_IRQ;
				break;
			case 4: /* pin D */
				dev->irq = RC32334_PCI_INTD_IRQ;
				break;
			default:
				dev->irq = 0xff; 
				break;
			}
#ifdef DEBUG
			printk("irq fixup: slot %d, pin %d, irq %d\n",
			       slot, pin, dev->irq);
#endif
			pci_write_config_byte(dev, PCI_INTERRUPT_LINE,
					      dev->irq);
		}
	}
}
#endif
