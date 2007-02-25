/*******************************************************************************  
 * FILE PURPOSE:     Code for I2C algorthmic module                                  
 *******************************************************************************  
 * FILE NAME:       i2c_algo.c                                                     
 *                                                                                
 * DESCRIPTION:     Source code for Avalanche I2C algorithmic module                  
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
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/sched.h>

#include <linux/i2c.h>



#include <asm/avalanche/avalanche_map.h>
#include "i2c-adap.h"
#include "i2c-algo.h"


#define  IIC_CLK_RATE         100000

#define  I2C_ALGO_AVALANCHE_IIC        120

#define  I2C_DATA_REG_HIGH       0x0
#define  I2C_DATA_REG_LOW        0x4
#define  I2C_CONFIGURATION       0x8
#define  I2C_DATA_REGISTER       0xC
#define  I2C_CLOCK_DIVIDER       0x10


/* Internal defines */
#define I2C_MAX_RETRY_COUNT         100

/*
 * Define the register masks for
 * data high register.
 */
#define I2C_READ_MODE_ENABLE             0x0300
#define I2C_WRITE_MODE_ENABLE            0x0200
#define I2C_END_BURST                    0x0400

/*
 * Define the register masks for
 * I2C_CONFIGURATION register.
 */

#define I2C_SPIKE_FILTER_MASK        0x7000
#define I2C_HAL_SPIKE_FILTER_BIT_POS     12
#define I2C_INTERRUPT_ENABLE         0x0001

/*
 * Define the register masks for
 * read data register.
 */

#define I2C_BUS_ERROR            0x8000
#define I2C_TRANSFER_COMPLETE    0x4000
#define I2C_WRITE_ALLOWED        0x2000     /*
                                             * this bit is high means that
                                             * device is busy and write is
                                             * not allowed. so it is a misnomer
                                             */


#define IIC_SINGLE_XFER		0
#define IIC_COMBINED_XFER	1

#define I2C_ERROR                -1

/* for ioctl */
#define I2C_SETCLK          0
#define I2C_GETCLK			1


/* ----- global defines ----------------------------------------------- */
#define DEB(x) if (i2c_debug>=1) x
#define DEB2(x) if (i2c_debug>=2) x
#define DEB3(x) if (i2c_debug>=3) x /* print several statistical values*/
#define DEBPROTO(x) if (i2c_debug>=9) x;
 	/* debug the protocol by showing transferred bits */
#define DEF_TIMEOUT 16



/* ----- global variables ---------------------------------------------	*/


/* module parameters:
 */
extern int i2c_debug;
static int iic_test=0;	/* see if the line-setting functions work	*/
static int iic_scan=0;	/* have a look at what's hanging 'round		*/

/* --- setting states on the bus with the right timing: ---------------	*/

#define get_clock(adap) adap->getclock(adap->data)
#define iic_outl(adap, reg, val) adap->setiic(adap->data, reg, val)
#define iic_inl(adap, reg) adap->getiic(adap->data, reg)


/* --- other auxiliary functions --------------------------------------	*/

static void iic_start(struct i2c_algo_iic_data *adap)
{
	struct iic_avalanche *adap_priv_data =adap->data;
	iic_outl(adap,adap_priv_data->iic_base + I2C_DATA_REG_LOW, 0xFF);
}

static void iic_stop(struct i2c_algo_iic_data *adap)
{
	int temp;
	struct iic_avalanche *adap_priv_data =adap->data;

        temp = iic_inl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH);
	iic_outl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH, temp | I2C_END_BURST);
}

static void iic_reset(struct i2c_algo_iic_data *adap)
{
    /* reset iic and bring it out of reset */
    avalanche_reset_ctrl(AVALANCHE_I2C_RESET_BIT, IN_RESET);     
    udelay(1);                                                        
    avalanche_reset_ctrl(AVALANCHE_I2C_RESET_BIT, OUT_OF_RESET); 

}


static int wait_for_bb(struct i2c_algo_iic_data *adap)
{
	struct iic_avalanche *adap_priv_data =adap->data;
	int timeout = I2C_MAX_RETRY_COUNT;
	int status;

	status = iic_inl(adap,adap_priv_data->iic_base+I2C_DATA_REGISTER);
	while (timeout-- && (status & I2C_WRITE_ALLOWED)) {
		udelay(100); /* How much is this? */
		status = iic_inl(adap,adap_priv_data->iic_base+I2C_DATA_REGISTER);
		if(status & I2C_BUS_ERROR){
                   return I2C_ERROR;
        	}
	}
	if (timeout<=0) {
		DEB2(printk( "Timeout, host is busy iic reset\n"));
		iic_reset(adap);
	}
	return(timeout<=0);
}


/* After we issue a transaction on the IIC bus, this function
 * is called.  It puts this process to sleep until we get an interrupt from
 * from the controller telling us that the transaction we requested in complete.
 */
static int wait_for_pin(struct i2c_algo_iic_data *adap, int *status) {

	struct iic_avalanche *adap_priv_data =adap->data;
	int retry_count;
	int data_register;

    if(adap_priv_data->iic_irq) //interrupt mode, wait for interrupt
        adap->waitforpin();

    
        DEB2(printk("data_register = 0x%04X\n",*(int *) 0xa861100C));

    *status = wait_for_bb(adap);

}



reg_dump()
{

     //   printk("data reg high = 0x%04X\n",*(int *) 0xa8611000);
     //   printk("data reg low = 0x%04X\n", *(int *) 0xa8611004);
     //   printk("configuration = 0x%04X\n",*(int *) 0xa8611008);
        DEB2(printk("data_register = 0x%04X\n",*(int *) 0xa861100C));
     //   printk("clock_divider = 0x%04X\n",*(int *) 0xa8611010);

}




static int iic_init (struct i2c_algo_iic_data *adap)
{
	struct iic_avalanche *adap_priv_data =adap->data;
	short i;

	/* initialize i2c */
    int clk_divider;
    int retry_count;
    int temp;
    int inp_clock_rate = adap_priv_data->iic_clock;
    int status;
  

    /* reset the iic */
    iic_reset(adap);
    
    reg_dump();

    printk("iic brought out of reset\n"); 


    /*
     * Wait for the write_allowed bit to become low.
     * The I2C module cannot be configured till this bit is low.
     * After retrying for 9 clock cycles, the h/w will clear the
     * write allowed bit and will also set the error bit.
     */


    printk("waiting for write allowed bit\n");

    printk("base=%x, offset=%x \n",adap_priv_data->iic_base,I2C_DATA_REGISTER);

    printk("PRCR=%x\n",*(int *)(0xa8611600));

    for(retry_count = 0; retry_count <I2C_MAX_RETRY_COUNT;retry_count++)
    {
        if(!(iic_inl(adap,adap_priv_data->iic_base+I2C_DATA_REGISTER) & I2C_WRITE_ALLOWED))
        {
            break;
        }
        if((iic_inl(adap,adap_priv_data->iic_base+I2C_DATA_REGISTER) & I2C_BUS_ERROR))
        {
            return I2C_ERROR;
        }
    }

    if(retry_count >= I2C_MAX_RETRY_COUNT)
    {
        return I2C_ERROR;
    }

    printk("enabling interrupt bit\n");

    /* enable interrupt bit always */
    temp = iic_inl(adap,adap_priv_data->iic_base+I2C_CONFIGURATION);
    iic_outl(adap,adap_priv_data->iic_base+I2C_CONFIGURATION, temp | I2C_INTERRUPT_ENABLE);

    /*
     * Set no spike filtering
     */

    printk("spike filter setting\n");
    temp = iic_inl(adap,adap_priv_data->iic_base+I2C_CONFIGURATION);
    iic_outl(adap,adap_priv_data->iic_base+I2C_CONFIGURATION, temp &  (~I2C_SPIKE_FILTER_MASK));


    /*
     * Set the rate of serial clock to requested rate
     */

    /* calculate the clock divider */
    clk_divider = inp_clock_rate/IIC_CLK_RATE + \
                        ((inp_clock_rate % IIC_CLK_RATE) !=0);

    if(clk_divider > 0xFFFF)
    {
        return I2C_ERROR;
    }

    printk("clock divider setting\n");
    /* set the clock divider register */
    iic_outl(adap,adap_priv_data->iic_base+I2C_CLOCK_DIVIDER, 6250);



        if(wait_for_bb(adap))
           printk("i2c bus error\n");                    
                                

    reg_dump();
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_HIGH,0x200|0x50 );    
    mdelay(100); 
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,0);
    mdelay(100); 
    reg_dump();
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,100);
    mdelay(100); 
    reg_dump();
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,0x62);
    mdelay(100); 
    reg_dump();

    {int temp;
        temp = iic_inl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH);
	iic_outl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH, temp | I2C_END_BURST);
    }    
  
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_HIGH,0x200|0x50 );    
    mdelay(100); 
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,0);
    mdelay(100); 
    reg_dump();
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,100);
    mdelay(100); 
    reg_dump();
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_HIGH,0x300|0x50);      
    iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,0xFF);      

    mdelay(100);
    reg_dump();
    {int temp;
        temp = iic_inl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH);
	iic_outl(adap,adap_priv_data->iic_base + I2C_DATA_REG_HIGH, temp | I2C_END_BURST);
    }

    mdelay(100);    
    reg_dump();  
 
 

    return 0;
}


/*
 * Sanity check for the adapter hardware - check the reaction of
 * the bus lines only if it seems to be idle.
 */
static int test_bus(struct i2c_algo_iic_data *adap, char *name) {
	return (0);
}

/* ----- Utility functions
 */

/* Verify the device we want to talk to on the IIC bus really exists. */
static inline int try_address(struct i2c_algo_iic_data *adap,
		       unsigned int addr, int retries)
{
#if 0
	struct iic_avalanche *adap_priv_data =adap->data;
	int i, ret = -1;
	short status;

	for (i=0;i<retries;i++) {
		iic_outl(adap, adap_priv_data->iic_base+I2C_DATA_REG_HIGH, addr);
		iic_start(adap);
		if (wait_for_pin(adap, &status) == 0) {
			iic_stop(adap);
			ret=1;
			break;	/* success! */
		}
		iic_stop(adap);
		udelay(adap->udelay);
	}
	DEB2(if (i) printk("try_address: needed %d retries for 0x%x\n",i,
	                   addr));

	return ret;

#endif

	return 1;

}


static int iic_sendbytes(struct i2c_adapter *i2c_adap,const char *buf,
                         int count,int xfer_type)
{
	struct i2c_algo_iic_data *adap = i2c_adap->algo_data;
	struct iic_avalanche *adap_priv_data =adap->data;
        int wrcount =0;
        int status;

	for(wrcount = 0; wrcount <count; wrcount++){
		wait_for_bb(adap);
		iic_outl(adap,adap_priv_data->iic_base+ I2C_DATA_REG_LOW,buf[wrcount]);
		wait_for_pin(adap, &status);
		if(status){
        		reg_dump();
	    		return -1;
		}
        	DEB2(printk("i2c data=%d  written\n",buf[wrcount]));
	}
 
	return wrcount;
}


static int iic_readbytes(struct i2c_adapter *i2c_adap, char *buf, int count,int xfer_type)
{
	struct i2c_algo_iic_data *adap = i2c_adap->algo_data;
	struct iic_avalanche *adap_priv_data =adap->data;
	int rdcount;
	int temp,status;

	for(rdcount = 0; rdcount <count; rdcount++){
		wait_for_bb(adap);
		iic_outl(adap,adap_priv_data->iic_base + I2C_DATA_REG_LOW,0xff);
		wait_for_pin(adap, &status);
		if(status)
	    		return -1;

		temp = iic_inl(adap,adap_priv_data->iic_base+I2C_DATA_REGISTER);
		buf[rdcount] = 0xff & temp;
        	DEB2(printk("i2c data = %d read\n",buf[rdcount]));
	}

	return rdcount;
}




/* Whenever we initiate a transaction, the first byte clocked
 * onto the bus after the start condition is the address (7 bit) of the
 * device we want to talk to.  This function manipulates the address specified
 * so that it makes sense to the hardware when written to the IIC peripheral.
 *
 * Note: 10 bit addresses are not supported in this driver, although they are
 * supported by the hardware.  This functionality needs to be implemented.
 */
static  int iic_doAddress(struct i2c_algo_iic_data *adap,
                                struct i2c_msg *msg, int retries)
{
	struct iic_avalanche *adap_priv_data =adap->data;
	unsigned short flags = msg->flags;
	unsigned int addr;
	int ret;
        int status;

	/* Ten bit addresses not supported right now */
	if ( (flags & I2C_M_TEN)  ) 
	{
		printk("ten bit address\n");
	} 

	/* We need to support do Address only for length = 0 */
	else
	{
		addr = msg->addr;

		if(msg->len)
		{
			if(msg->flags & I2C_M_RD)
				addr |= ( I2C_READ_MODE_ENABLE );
			else
				addr |= ( I2C_WRITE_MODE_ENABLE );
		}
		else /* For emulating 0 byte transfer its always safe to read */
			addr |= ( I2C_READ_MODE_ENABLE );

		iic_outl(adap, adap_priv_data->iic_base + I2C_DATA_REG_HIGH, addr);

		if(msg->len)
			return 0;

		iic_start(adap);

               if(wait_for_pin(adap,&status)){
			DEB2(printk("error in doAddress\n"));
			iic_stop(adap);
			return -1;  
               }
               iic_stop(adap);
	}
	return 0;
}

/* Description: Prepares the controller for a transaction (clearing status
 * registers, data buffers, etc), and then calls either iic_readbytes or
 * iic_sendbytes to do the actual transaction.
 *
 * Before we issue a transaction, we should
 * verify that the bus is not busy or in some unknown state.
 */
static int iic_xfer(struct i2c_adapter *i2c_adap,
		    struct i2c_msg msgs[],
		    int num)
{
	struct i2c_algo_iic_data *adap = i2c_adap->algo_data;
	struct i2c_msg *pmsg;
	int i = 0;
	int ret=0, timeout;

	pmsg = &msgs[i];


	/* Wait for any pending transfers to complete */
	timeout = wait_for_bb(adap);
	if (timeout) {
		DEB2(printk("iic_xfer: Timeout waiting for host not busy\n");)
		return -EIO;
	}


	for(i=0; i<num; i++) {
		int status;
		pmsg = &msgs[i];
	
		/* Load address */
		status = iic_doAddress(adap, pmsg, i2c_adap->retries);
		if (status)
			return -EIO;

		if(pmsg->flags & I2C_M_RD) {
         		DEB2(printk( "  This one is a read\n"));
         		status = iic_readbytes(i2c_adap, pmsg->buf, pmsg->len, IIC_COMBINED_XFER);
         		if(status < 0)
				break;
			else
				ret += status;
      		}
      		else if(!(pmsg->flags & I2C_M_RD)) {
         		DEB2(printk("This one is a write\n"));
         		status = iic_sendbytes(i2c_adap, pmsg->buf, pmsg->len, IIC_COMBINED_XFER);
			if(status < 0)
             			break;
		}

	}

        iic_stop(adap);
	return ret; 
}


/* Implements device specific ioctls.  Higher level ioctls can
 * be found in i2c-core.c and are typical of any i2c controller (specifying
 * slave address, timeouts, etc).  These ioctls take advantage of any hardware
 * features built into the controller for which this algorithm-adapter set
 * was written.  
 */
static int algo_control(struct i2c_adapter *adapter,
	unsigned int cmd, unsigned long arg)
{

	struct i2c_algo_iic_data *adap = adapter->algo_data;
        struct iic_avalanche *adap_priv_data =  adap->data;
        int inp_clock_rate = adap_priv_data->iic_clock;
        int clk_divider;      
        int timeout;
  	char *buf;
  	int ret;

  	switch(cmd)
  	{
		case I2C_SETCLK:
			timeout = wait_for_bb(adap);
			if (timeout) {
				DEB2(printk("iic_xfer: Timeout waiting for host not busy\n");)
				ret = -EIO;
				break;
			}

			if(arg > 0 && arg < inp_clock_rate){
				/* calculate the clock divider */
				clk_divider = inp_clock_rate/arg + ((inp_clock_rate % arg) !=0);
         
				/* set the clock divider register */
				iic_outl(adap,adap_priv_data->iic_base+I2C_CLOCK_DIVIDER, clk_divider);
				ret = 0;
			}
		break;

		case I2C_GETCLK:
			clk_divider = iic_inl(adap,adap_priv_data->iic_base+I2C_CLOCK_DIVIDER);	
			*((int *)arg) =  (get_clock(adap) / clk_divider); 
			ret = 0;
		break;
	}
   	return 0;
}


static u32 iic_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL;
}

/* -----exported algorithm data: -------------------------------------	*/

static struct i2c_algorithm iic_algo = {
	"TI IIC algorithm",
	I2C_ALGO_AVALANCHE_IIC, /* to be defined */
	iic_xfer,		/* master_xfer	*/
	NULL,				/* smbus_xfer	*/
	NULL,				/* slave_xmit		*/
	NULL,				/* slave_recv		*/
	algo_control,			/* ioctl		*/
	iic_func,			/* functionality	*/
};


/*
 * registering functions to load algorithms at runtime
 */
int i2c_avalanche_add_bus(struct i2c_adapter *adap)
{
	int i;
	short status;
	struct i2c_algo_iic_data *iic_adap = adap->algo_data;

	if (iic_test) {
		int ret = test_bus(iic_adap, adap->name);
		if (ret<0)
			return -ENODEV;
	}

	DEB2(printk("i2c-algo-ti: hw routines for %s registered.\n",
	            adap->name));

	/* register new adapter to i2c module... */

	adap->id |= iic_algo.id;
	adap->algo = &iic_algo;

	adap->timeout = 100;	/* default values, should	*/
	adap->retries = 3;		/* be replaced by defines	*/
	adap->flags = 0;


        printk("calling i2c add adapter\n");
	i2c_add_adapter(adap);
        printk("calling i2c init\n");
	iic_init(iic_adap);
	return 0;
}


int i2c_avalanche_del_bus(struct i2c_adapter *adap)
{
	int res;
	if ((res = i2c_del_adapter(adap)) < 0)
		return res;
	DEB2(printk("i2c-algo-ti: adapter unregistered: %s\n",adap->name));

	return 0;
}


EXPORT_SYMBOL(i2c_avalanche_add_bus);
EXPORT_SYMBOL(i2c_avalanche_del_bus);

