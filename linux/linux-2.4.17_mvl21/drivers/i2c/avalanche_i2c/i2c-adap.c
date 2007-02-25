/******************************************************************************* 
 * FILE PURPOSE:     Code for I2C adapter module                                    
 ******************************************************************************* 
 * FILE NAME:       i2c-adap.c                                                   
 *                                                                               
 * DESCRIPTION:     Source code for Avalanche I2C adapter module                           
 *                                                                               
 * REVISION HISTORY:                                                             
 *                                                                               
 * Date           Description                               Author               
 *-----------------------------------------------------------------------------  
 * 6 Jan 2004    Initial Creation                          Sharath Kumar        
 *                                                                               
 * (C) Copyright 2003, Texas Instruments, Inc                                    
 ******************************************************************************/ 
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/init.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <linux/i2c.h>

#include <asm/avalanche/avalanche_map.h>


#include "i2c-adap.h"
#include "i2c-algo.h"

static struct iic_avalanche iic_avalanche_priv_data =
{
	AVALANCHE_I2C_BASE,	/* Base address */
	LNXINTNUM(AVALANCHE_I2C_INT),		/* Interrupt */
	0,		
	0,		/* Our address on the i2c bus */
	0		/* Sequential index for self referral */
};
#if (LINUX_VERSION_CODE < 0x020301)
static struct wait_queue *iic_wait = NULL;
#else
static wait_queue_head_t iic_wait;
#endif
static int iic_pending;
int i2c_debug = 1;

/* ----- global defines -----------------------------------------------	*/

#define I2C_HW_AVALANCHE_ID        160


#define DEB(x)	if (i2c_debug>=1) x
#define DEB2(x) if (i2c_debug>=2) x
#define DEB3(x) if (i2c_debug>=3) x
#define DEBE(x)	x	/* error messages 				*/


/* ----- local functions ----------------------------------------------	*/

static void iic_avalanche_setiic(void *data, int ctl, short val)
{
	outl(val,ctl);
}

static short iic_avalanche_getiic(void *data, int ctl)
{
	short val;

	val = inl(ctl);
	return (val);
}

static int iic_avalanche_getown(void *data)
{
	return (iic_avalanche_priv_data.iic_own);
}


static int iic_avalanche_getclock(void *data)
{
	return (iic_avalanche_priv_data.iic_clock);
}



/* Put this process to sleep.  We will wake up when the
 * IIC controller interrupts.
 */
static void iic_avalanche_waitforpin(void) {

   int timeout = 2;

   /* If interrupts are enabled (which they are), then put the process to
    * sleep.  This process will be awakened by two events -- either the
    * the IIC peripheral interrupts or the timeout expires.
    * If interrupts are not enabled then delay for a reasonable amount
    * of time and return.
    */
   if (iic_avalanche_priv_data.iic_irq > 0) {
	cli();
	if (iic_pending == 0) {
		interruptible_sleep_on_timeout(&iic_wait, timeout*HZ );
	} else
		iic_pending = 0;
	sti();
   } else {
      udelay(100);
   }
}


static void iic_avalanche_handler(int this_irq, void *dev_id, struct pt_regs *regs)
{

   iic_pending = 1;

   DEB2(printk("iic_avalanche_handler: in interrupt handler\n"));
   wake_up_interruptible(&iic_wait);
}


/* Request our
 * interrupt line and register its associated handler.
 */
static int iic_hw_resrc_init(void)
{

	if (iic_avalanche_priv_data.iic_irq > 0) {
	   if (request_irq(iic_avalanche_priv_data.iic_irq, iic_avalanche_handler, 0, "TI IIC", 0) < 0) {
	      iic_avalanche_priv_data.iic_irq = 0;
	   } else
	      DEB3(printk("Enabled IIC IRQ %d\n", iic_avalanche_priv_data.iic_irq));
	   }
	return 0;
}


static void iic_avalanche_release(void)
{
	if (iic_avalanche_priv_data.iic_irq > 0) {
		disable_irq(iic_avalanche_priv_data.iic_irq);
		free_irq(iic_avalanche_priv_data.iic_irq, 0);
	}
}


static int iic_avalanche_reg(struct i2c_client *client)
{
	return 0;
}


static int iic_avalanche_unreg(struct i2c_client *client)
{
	return 0;
}


static void iic_avalanche_inc_use(struct i2c_adapter *adap)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


static void iic_avalanche_dec_use(struct i2c_adapter *adap)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


/* ------------------------------------------------------------------------
 * Encapsulate the above functions in the correct operations structure.
 * This is only done when more than one hardware adapter is supported.
 */
static struct i2c_algo_iic_data iic_avalanche_data = {
	NULL,
	iic_avalanche_setiic,
	iic_avalanche_getiic,
	iic_avalanche_getown,
	iic_avalanche_getclock,
	iic_avalanche_waitforpin,
	80, 80, 100,		/*	waits, timeout */
};

static struct i2c_adapter iic_avalanche_ops = {
	"TI IIC adapter",
	I2C_HW_AVALANCHE_ID,
	NULL,
	&iic_avalanche_data,
	iic_avalanche_inc_use,
	iic_avalanche_dec_use,
	iic_avalanche_reg,
	iic_avalanche_unreg,
};

/* Called when the module is loaded.  This function starts the
 * cascade of calls up through the heirarchy of i2c modules (i.e. up to the
 *  algorithm layer and into to the core layer)
 */
static int __init iic_avalanche_init(void)
{

	struct iic_avalanche *piic = &iic_avalanche_priv_data;

	DEB2(printk(KERN_INFO "Initialize Avalanche IIC adapter module\n"));




        iic_avalanche_priv_data.iic_clock = avalanche_get_vbus_freq(); /* Clock frequency on the peripheral bus */ 

	iic_avalanche_data.data = (void *)piic;
	init_waitqueue_head(&iic_wait);
	if (iic_hw_resrc_init() == 0) {

                DEB2(printk("calling add bus\n"));

		if (i2c_avalanche_add_bus(&iic_avalanche_ops) < 0)
			return -ENODEV;
	} else {
		return -ENODEV;
	}
	DEB2(printk(KERN_INFO " found device at %#x irq %d.\n",
		piic->iic_base, piic->iic_irq));
	return 0;
}


static void iic_avalanche_exit(void)
{
	i2c_avalanche_del_bus(&iic_avalanche_ops);
        iic_avalanche_release();
}

EXPORT_NO_SYMBOLS;

/* If modules is NOT defined when this file is compiled, then the MODULE_*
 * macros will resolve to nothing
 */

MODULE_DESCRIPTION("I2C-Bus adapter routines for TI IIC bus adapter");
MODULE_LICENSE("GPL");

MODULE_PARM(base, "i");
MODULE_PARM(irq, "i");
MODULE_PARM(clock, "i");
MODULE_PARM(own, "i");
MODULE_PARM(i2c_debug,"i");


/* Called when module is loaded or when kernel is intialized.
 * If MODULES is defined when this file is compiled, then this function will
 * resolve to init_module (the function called when insmod is invoked for a
 * module).  Otherwise, this function is called early in the boot, when the
 * kernel is intialized.  Check out /include/init.h to see how this works.
 */
module_init(iic_avalanche_init);

/* Resolves to module_cleanup when MODULES is defined. */
module_exit(iic_avalanche_exit);
