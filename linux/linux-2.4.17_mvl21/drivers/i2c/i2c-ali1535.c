/*
    i2c-ali1535.c - Part of lm_sensors, Linux kernel modules for hardware
                    monitoring
    Copyright (c) 2000  Frodo Looijaard <frodol@dds.nl>, 
                        Philip Edelbrock <phil@netroedge.com>, 
                        Mark D. Studebaker <mdsxyz123@yahoo.com>,
                        Dan Eaton <dan.eaton@rocketlogix.com> and 
                        Stephen Rousset<stephen.rousset@rocketlogix.com> 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
    This is the driver for the SMB Host controller on
    Acer Labs Inc. (ALI) M1535 South Bridge.

    The M1535 is a South bridge for portable systems.
    It is very similar to the M15x3 South bridges also produced
    by Acer Labs Inc.  Some of the registers within the part
    have moved and some have been redefined slightly. Additionally,
    the sequencing of the SMBus transactions has been modified
    to be more consistent with the sequencing recommended by
    the manufacturer and observed through testing.  These
    changes are reflected in this driver and can be identified
    by comparing this driver to the i2c-ali15x3 driver.
    For an overview of these chips see http://www.acerlabs.com

    The SMB controller is part of the 7101 device, which is an
    ACPI-compliant Power Management Unit (PMU).

    The whole 7101 device has to be enabled for the SMB to work.
    You can't just enable the SMB alone.
    The SMB and the ACPI have separate I/O spaces.
    We make sure that the SMB is enabled. We leave the ACPI alone.

    This driver controls the SMB Host only.

    This driver does not use interrupts.
*/


/* Note: we assume there can only be one ALI1535, with one SMBus interface */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/semaphore.h>
#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/i2c.h>
#define LM_DATE "20011118"
#define LM_VERSION "2.6.2"
#include <linux/init.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

#ifndef DECLARE_MUTEX
#define DECLARE_MUTEX(name)  struct semaphore name = MUTEX
#endif /* def DECLARE_MUTEX */

/* ALI1535 SMBus address offsets */
#define SMBHSTSTS (0 + ali1535_smba)
#define SMBHSTTYP (1 + ali1535_smba)
#define SMBHSTPORT (2 + ali1535_smba)
#define SMBHSTCMD (7 + ali1535_smba)
#define SMBHSTADD (3 + ali1535_smba)
#define SMBHSTDAT0 (4 + ali1535_smba)
#define SMBHSTDAT1 (5 + ali1535_smba)
#define SMBBLKDAT (6 + ali1535_smba)

/* PCI Address Constants */
#define SMBCOM    0x004
#define SMBREV    0x008
#define SMBCFG    0x0D1
#define SMBBA     0x0E2
#define SMBHSTCFG 0x0F0
#define SMBCLK    0x0F2

/* Other settings */
#define MAX_TIMEOUT 500		/* times 1/100 sec */
#define ALI1535_SMB_IOSIZE 32

/* 
*/
#define ALI1535_SMB_DEFAULTBASE 0x8040

/* ALI1535 address lock bits */
#define ALI1535_LOCK	0x06 < dwe >

/* ALI1535 command constants */
#define ALI1535_QUICK      0x00
#define ALI1535_BYTE       0x10
#define ALI1535_BYTE_DATA  0x20
#define ALI1535_WORD_DATA  0x30
#define ALI1535_BLOCK_DATA 0x40
#define ALI1535_I2C_READ   0x60

#define	ALI1535_DEV10B_EN	0x80	/* Enable 10-bit addressing in */
                                        /*  I2C read                   */
#define	ALI1535_T_OUT		0x08	/* Time-out Command (write)    */
#define	ALI1535_A_HIGH_BIT9	0x08	/* Bit 9 of 10-bit address in  */
                                        /* Alert-Response-Address      */
                                        /* (read)                      */
#define	ALI1535_KILL		0x04	/* Kill Command (write)        */
#define	ALI1535_A_HIGH_BIT8	0x04	/* Bit 8 of 10-bit address in  */
                                        /*  Alert-Response-Address     */
                                        /*  (read)                     */

#define	ALI1535_D_HI_MASK	0x03	/* Mask for isolating bits 9-8 */
                                        /*  of 10-bit address in I2C   */ 
                                        /*  Read Command               */

/* ALI1535 status register bits */
#define ALI1535_STS_IDLE	0x04
#define ALI1535_STS_BUSY	0x08	/* host busy */
#define ALI1535_STS_DONE	0x10	/* transaction complete */
#define ALI1535_STS_DEV		0x20	/* device error */
#define ALI1535_STS_BUSERR	0x40	/* bus error    */
#define ALI1535_STS_FAIL	0x80    /* failed bus transaction */
#define ALI1535_STS_ERR		0xE0	/* all the bad error bits */

#define ALI1535_BLOCK_CLR	0x04	/* reset block data index */

/* ALI1535 device address register bits */
#define	ALI1535_RD_ADDR		0x01	/* Read/Write Bit in Device    */
                                        /*  Address field              */
                                        /*  -> Write = 0               */
                                        /*  -> Read  = 1               */
#define	ALI1535_SMBIO_EN	0x04	/* SMB I/O Space enable        */

#ifdef MODULE
static
#else
extern
#endif
int __init i2c_ali1535_init(void);
static int __init ali1535_cleanup(void);
static int ali1535_setup(void);
static s32 ali1535_access(struct i2c_adapter *adap, u16 addr,
			  unsigned short flags, char read_write,
			  u8 command, int size,
			  union i2c_smbus_data *data);
static void ali1535_do_pause(unsigned int amount);
static int ali1535_transaction(void);
static void ali1535_inc(struct i2c_adapter *adapter);
static void ali1535_dec(struct i2c_adapter *adapter);
static u32 ali1535_func(struct i2c_adapter *adapter);

#ifdef MODULE
extern int init_module(void);
extern int cleanup_module(void);
#endif				/* MODULE */

static struct i2c_algorithm smbus_algorithm = {
	/* name */ "Non-i2c SMBus adapter",
	/* id */ I2C_ALGO_SMBUS,
	/* master_xfer */ NULL, 
	/* smbus_access */ ali1535_access,
	/* slave_send */ NULL,
	/* slave_rcv */ NULL,
	/* algo_control */ NULL,
	/* functionality */ ali1535_func,
};

static struct i2c_adapter ali1535_adapter = {
	"unset",
	I2C_ALGO_SMBUS | I2C_HW_SMBUS_ALI1535,
	&smbus_algorithm,
	NULL,
	ali1535_inc,
	ali1535_dec,
	NULL,
	NULL,
};

static int __initdata ali1535_initialized;
static unsigned short ali1535_smba = 0;
DECLARE_MUTEX(i2c_ali1535_sem);


/* Detect whether a ALI1535 can be found, and initialize it, where necessary.
   Note the differences between kernels with the old PCI BIOS interface and
   newer kernels with the real PCI interface. In compat.h some things are
   defined to make the transition easier. */
int ali1535_setup(void)
{
	int error_return = 0;
	unsigned char temp;

	struct pci_dev *ALI1535_dev;

	/* First check whether we can access PCI at all */
	if (pci_present() == 0) {
		printk("i2c-ali1535.o: Error: No PCI-bus found!\n");
		error_return = -ENODEV;
		goto END;
	}

	/* Look for the ALI1535, M7101 device */
	ALI1535_dev = NULL;
	ALI1535_dev = pci_find_device(PCI_VENDOR_ID_AL,
				      PCI_DEVICE_ID_AL_M7101, 
				      ALI1535_dev); 

	if (ALI1535_dev == NULL) {
		printk("i2c-ali1535.o: Error: Can't detect ali1535!\n");
		error_return = -ENODEV;
		goto END;
	}

/* Check the following things:
	- SMB I/O address is initialized
	- Device is enabled
	- We can use the addresses
*/

/* Determine the address of the SMBus area */
	pci_read_config_word(ALI1535_dev, SMBBA, &ali1535_smba);
	ali1535_smba &= (0xffff & ~(ALI1535_SMB_IOSIZE - 1));
	if (ali1535_smba == 0) {
		printk
		    ("i2c-ali1535.o: ALI1535_smb region uninitialized - upgrade BIOS?\n");
		error_return = -ENODEV;
	}

	if (error_return == -ENODEV)
		goto END;

	if (check_region(ali1535_smba, ALI1535_SMB_IOSIZE)) {
		printk
		    ("i2c-ali1535.o: ALI1535_smb region 0x%x already in use!\n",
		     ali1535_smba);
		error_return = -ENODEV;
	}

	if (error_return == -ENODEV)
		goto END;

	/* check if whole device is enabled */
	pci_read_config_byte(ALI1535_dev, SMBCFG, &temp);
	if ((temp & ALI1535_SMBIO_EN) == 0) {
		printk
		    ("i2c-ali1535.o: SMB device not enabled - upgrade BIOS?\n");
		error_return = -ENODEV;
		goto END;
	}

/* Is SMB Host controller enabled? */
	pci_read_config_byte(ALI1535_dev, SMBHSTCFG, &temp);
	if ((temp & 1) == 0) {
		printk
		    ("i2c-ali1535.o: SMBus controller not enabled - upgrade BIOS?\n");
		error_return = -ENODEV;
		goto END;
	}

/* set SMB clock to 74KHz as recommended in data sheet */
	pci_write_config_byte(ALI1535_dev, SMBCLK, 0x20);

	/* Everything is happy, let's grab the memory and set things up. */
	request_region(ali1535_smba, ALI1535_SMB_IOSIZE, "ali1535-smb");

#ifdef DEBUG
/*
  The interrupt routing for SMB is set up in register 0x77 in the
  1533 ISA Bridge device, NOT in the 7101 device.
  Don't bother with finding the 1533 device and reading the register.
  if ((....... & 0x0F) == 1)
     printk("i2c-ali1535.o: ALI1535 using Interrupt 9 for SMBus.\n");
*/
	pci_read_config_byte(ALI1535_dev, SMBREV, &temp);
	printk("i2c-ali1535.o: SMBREV = 0x%X\n", temp);
	printk("i2c-ali1535.o: ALI1535_smba = 0x%X\n", ali1535_smba);
#endif				/* DEBUG */

      END:
	return error_return;
}


/* Internally used pause function */
void ali1535_do_pause(unsigned int amount)
{
	current->state = TASK_INTERRUPTIBLE;
	schedule_timeout(amount);
}

/* Another internally used function */
int ali1535_transaction(void)
{
	int temp;
	int result = 0;
	int timeout = 0;

#ifdef DEBUG
	printk
	    ("i2c-ali1535.o: Transaction (pre): STS=%02x, TYP=%02x, CMD=%02x, ADD=%02x, DAT0=%02x, "
	     "DAT1=%02x\n", inb_p(SMBHSTSTS), inb_p(SMBHSTTYP),
	     inb_p(SMBHSTCMD), inb_p(SMBHSTADD), inb_p(SMBHSTDAT0),
	     inb_p(SMBHSTDAT1));
#endif

	/* get status */
	temp = inb_p(SMBHSTSTS);

	/* Make sure the SMBus host is ready to start transmitting */
	/* Check the busy bit first */
	if (temp & ALI1535_STS_BUSY) {
/*
   If the host controller is still busy, it may have timed out in the previous transaction,
   resulting in a "SMBus Timeout" printk.
   I've tried the following to reset a stuck busy bit.
	1. Reset the controller with an KILL command.
	   (this doesn't seem to clear the controller if an external device is hung)
	2. Reset the controller and the other SMBus devices with a T_OUT command.
	   (this clears the host busy bit if an external device is hung,
	   but it comes back upon a new access to a device)
	3. Disable and reenable the controller in SMBHSTCFG
   Worst case, nothing seems to work except power reset.
*/
/* Abort - reset the host controller */
/*
#ifdef DEBUG
    printk("i2c-ali1535.o: Resetting host controller to clear busy condition\n",temp);
#endif
    outb_p(ALI1535_KILL, SMBHSTTYP);
    temp = inb_p(SMBHSTSTS);
    if (temp & ALI1535_STS_BUSY) {
*/

/*
   Try resetting entire SMB bus, including other devices -
   This may not work either - it clears the BUSY bit but
   then the BUSY bit may come back on when you try and use the chip again.
   If that's the case you are stuck.
*/
		printk
		    ("i2c-ali1535.o: Resetting entire SMB Bus to clear busy condition (%02x)\n",
		     temp);
		outb_p(ALI1535_T_OUT, SMBHSTTYP);
		temp = inb_p(SMBHSTSTS);
	}
/*
  }
*/

	/* now check the error bits and the busy bit */
	if (temp & (ALI1535_STS_ERR | ALI1535_STS_BUSY)) {
		/* do a clear-on-write */
		outb_p(0xFF, SMBHSTSTS);
		if ((temp = inb_p(SMBHSTSTS)) &
		    (ALI1535_STS_ERR | ALI1535_STS_BUSY)) {
			/* this is probably going to be correctable only by a power reset
			   as one of the bits now appears to be stuck */
			/* This may be a bus or device with electrical problems. */
			printk
			    ("i2c-ali1535.o: SMBus reset failed! (0x%02x) - controller or device on bus is probably hung\n",
			     temp);
			return -1;
		}
	} else {
		/* check and clear done bit */
		if (temp & ALI1535_STS_DONE) {
			outb_p(temp, SMBHSTSTS);
		}
	}

	/* start the transaction by writing anything to the start register */
	outb_p(0xFF, SMBHSTPORT);

	/* We will always wait for a fraction of a second! */
	timeout = 0;
	do {
		ali1535_do_pause(1);
		temp = inb_p(SMBHSTSTS);
	} while (((temp & ALI1535_STS_BUSY) && !(temp & ALI1535_STS_IDLE))
		 && (timeout++ < MAX_TIMEOUT));

	/* If the SMBus is still busy, we give up */
	if (timeout >= MAX_TIMEOUT) {
		result = -1;
		printk("i2c-ali1535.o: SMBus Timeout!\n");
	}

	if (temp & ALI1535_STS_FAIL) {
		result = -1;
#ifdef DEBUG
		printk("i2c-ali1535.o: Error: Failed bus transaction\n");
#endif
	}

/*
  Unfortunately the ALI SMB controller maps "no response" and "bus collision"
  into a single bit. No reponse is the usual case so don't do a printk.
  This means that bus collisions go unreported.
*/
	if (temp & ALI1535_STS_BUSERR) {
		result = -1;
#ifdef DEBUG
		printk
		    ("i2c-ali1535.o: Error: no response or bus collision ADD=%02x\n",
		     inb_p(SMBHSTADD));
#endif
	}

/* haven't ever seen this */
	if (temp & ALI1535_STS_DEV) {
		result = -1;
		printk("i2c-ali1535.o: Error: device error\n");
	}

/* 
   check to see if the "command complete" indication is set
 */
	if (!(temp & ALI1535_STS_DONE)) {
		result = -1;
		printk("i2c-ali1535.o: Error: command never completed\n");
	}
#ifdef DEBUG
	printk
	    ("i2c-ali1535.o: Transaction (post): STS=%02x, TYP=%02x, CMD=%02x, ADD=%02x, "
	     "DAT0=%02x, DAT1=%02x\n", inb_p(SMBHSTSTS), inb_p(SMBHSTTYP),
	     inb_p(SMBHSTCMD), inb_p(SMBHSTADD), inb_p(SMBHSTDAT0),
	     inb_p(SMBHSTDAT1));
#endif

/* 
    take consequent actions for error conditions
 */
        if (!(temp & ALI1535_STS_DONE)) {
	  /* issue "kill" to reset host controller */
	  outb_p(ALI1535_KILL,SMBHSTTYP);
	  outb_p(0xFF,SMBHSTSTS);
	}	  
	else if (temp & ALI1535_STS_ERR) {
	  /* issue "timeout" to reset all devices on bus */
	  outb_p(ALI1535_T_OUT,SMBHSTTYP);
	  outb_p(0xFF,SMBHSTSTS);
	}
        
	return result;
}

/* Return -1 on error. See smbus.h for more information */
s32 ali1535_access(struct i2c_adapter * adap, u16 addr,
		   unsigned short flags, char read_write, u8 command,
		   int size, union i2c_smbus_data * data)
{
	int i, len;
	int temp;
	int timeout;
	s32 result = 0;

	down(&i2c_ali1535_sem);
/* make sure SMBus is idle */
	temp = inb_p(SMBHSTSTS);
	for (timeout = 0;
	     (timeout < MAX_TIMEOUT) && !(temp & ALI1535_STS_IDLE);
	     timeout++) {
		ali1535_do_pause(1);
		temp = inb_p(SMBHSTSTS);
	}
	if (timeout >= MAX_TIMEOUT) {
		printk("i2c-ali1535.o: Idle wait Timeout! STS=0x%02x\n",
		       temp);
	}

/* clear status register (clear-on-write) */
	outb_p(0xFF, SMBHSTSTS);

	switch (size) {
	case I2C_SMBUS_PROC_CALL:
		printk
		    ("i2c-ali1535.o: I2C_SMBUS_PROC_CALL not supported!\n");
		result = -1;
		goto EXIT;
	case I2C_SMBUS_QUICK:
		outb_p(((addr & 0x7f) << 1) | (read_write & 0x01),
		       SMBHSTADD);
		size = ALI1535_QUICK;
                outb_p(size, SMBHSTTYP);	/* output command */
                break;
	case I2C_SMBUS_BYTE:
		outb_p(((addr & 0x7f) << 1) | (read_write & 0x01),
		       SMBHSTADD);
		size = ALI1535_BYTE;
                outb_p(size, SMBHSTTYP);	/* output command */
		if (read_write == I2C_SMBUS_WRITE)
			outb_p(command, SMBHSTCMD);
		break;
	case I2C_SMBUS_BYTE_DATA:
		outb_p(((addr & 0x7f) << 1) | (read_write & 0x01),
		       SMBHSTADD);
		size = ALI1535_BYTE_DATA;
                outb_p(size, SMBHSTTYP);	/* output command */
		outb_p(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE)
			outb_p(data->byte, SMBHSTDAT0);
		break;
	case I2C_SMBUS_WORD_DATA:
		outb_p(((addr & 0x7f) << 1) | (read_write & 0x01),
		       SMBHSTADD);
		size = ALI1535_WORD_DATA;
                outb_p(size, SMBHSTTYP);	/* output command */
		outb_p(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE) {
			outb_p(data->word & 0xff, SMBHSTDAT0);
			outb_p((data->word & 0xff00) >> 8, SMBHSTDAT1);
		}
		break;
	case I2C_SMBUS_BLOCK_DATA:
		outb_p(((addr & 0x7f) << 1) | (read_write & 0x01),
		       SMBHSTADD);
		size = ALI1535_BLOCK_DATA;
                outb_p(size, SMBHSTTYP);	/* output command */
		outb_p(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE) {
			len = data->block[0];
			if (len < 0) {
				len = 0;
				data->block[0] = len;
			}
			if (len > 32) {
				len = 32;
				data->block[0] = len;
			}
			outb_p(len, SMBHSTDAT0);
			outb_p(inb_p(SMBHSTTYP) | ALI1535_BLOCK_CLR, SMBHSTTYP);	/* Reset SMBBLKDAT */
			for (i = 1; i <= len; i++)
				outb_p(data->block[i], SMBBLKDAT);
		}
		break;
	}

	if (ali1535_transaction())	/* Error in transaction */
	  {
		result = -1;
		goto EXIT;
          }

	if ((read_write == I2C_SMBUS_WRITE) || (size == ALI1535_QUICK))
	  {
		result = 0;
		goto EXIT;
          }

	switch (size) {
	case ALI1535_BYTE:	/* Result put in SMBHSTDAT0 */
		data->byte = inb_p(SMBHSTDAT0);
		break;
	case ALI1535_BYTE_DATA:
		data->byte = inb_p(SMBHSTDAT0);
		break;
	case ALI1535_WORD_DATA:
		data->word = inb_p(SMBHSTDAT0) + (inb_p(SMBHSTDAT1) << 8);
		break;
	case ALI1535_BLOCK_DATA:
		len = inb_p(SMBHSTDAT0);
		if (len > 32)
			len = 32;
		data->block[0] = len;
		outb_p(inb_p(SMBHSTTYP) | ALI1535_BLOCK_CLR, SMBHSTTYP);	/* Reset SMBBLKDAT */
		for (i = 1; i <= data->block[0]; i++) {
			data->block[i] = inb_p(SMBBLKDAT);
#ifdef DEBUG
			printk
			    ("i2c-ali1535.o: Blk: len=%d, i=%d, data=%02x\n",
			     len, i, data->block[i]);
#endif	/* DEBUG */
		}
		break;
	}
EXIT:
	up(&i2c_ali1535_sem);
	return result;
}

void ali1535_inc(struct i2c_adapter *adapter)
{
	MOD_INC_USE_COUNT;
}

void ali1535_dec(struct i2c_adapter *adapter)
{

	MOD_DEC_USE_COUNT;
}

u32 ali1535_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE |
	    I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |
	    I2C_FUNC_SMBUS_BLOCK_DATA;
}

int __init i2c_ali1535_init(void)
{
	int res;
	printk("i2c-ali1535.o version %s (%s)\n", LM_VERSION, LM_DATE);
#ifdef DEBUG
/* PE- It might be good to make this a permanent part of the code! */
	if (ali1535_initialized) {
		printk
		    ("i2c-ali1535.o: Oops, ali1535_init called a second time!\n");
		return -EBUSY;
	}
#endif
	ali1535_initialized = 0;
	if ((res = ali1535_setup())) {
		printk
		    ("i2c-ali1535.o: ALI1535 not detected, module not inserted.\n");
		ali1535_cleanup();
		return res;
	}
	ali1535_initialized++;
	sprintf(ali1535_adapter.name, "SMBus ALI1535 adapter at %04x",
		ali1535_smba);
	if ((res = i2c_add_adapter(&ali1535_adapter))) {
		printk
		    ("i2c-ali1535.o: Adapter registration failed, module not inserted.\n");
		ali1535_cleanup();
		return res;
	}
	ali1535_initialized++;
	printk
	    ("i2c-ali1535.o: ALI1535 SMBus Controller detected and initialized\n");
	return 0;
}

int __init ali1535_cleanup(void)
{
	int res;
	if (ali1535_initialized >= 2) {
		if ((res = i2c_del_adapter(&ali1535_adapter))) {
			printk
			    ("i2c-ali1535.o: i2c_del_adapter failed, module not removed\n");
			return res;
		} else
			ali1535_initialized--;
	}
	if (ali1535_initialized >= 1) {
		release_region(ali1535_smba, ALI1535_SMB_IOSIZE);
		ali1535_initialized--;
	}
	return 0;
}

#ifdef RLX
EXPORT_SYMBOL(ali1535_smba);
EXPORT_SYMBOL(ali1535_access);
EXPORT_SYMBOL(i2c_ali1535_sem);
#else
EXPORT_NO_SYMBOLS;
#endif

#ifdef MODULE

MODULE_AUTHOR
    ("Frodo Looijaard <frodol@dds.nl>, Philip Edelbrock <phil@netroedge.com>,
      Mark D. Studebaker <mdsxyz123@yahoo.com> and Dan Eaton <dan.eaton@rocketlogix.com>");
MODULE_DESCRIPTION("ALI1535 SMBus driver");

int init_module(void)
{
	return i2c_ali1535_init();
}

int cleanup_module(void)
{
	return ali1535_cleanup();
}

#endif				/* MODULE */

