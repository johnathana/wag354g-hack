/*******************************************************************************   
 * FILE PURPOSE:    Keypad Module Driver Source                                       
 *******************************************************************************   
 * FILE NAME:       keypad_drv.c                                                   
 *                                                                                 
 * DESCRIPTION:     Source code for Linux Keypad Driver                             
 *                                                                                 
 * REVISION HISTORY:  
 *   
 * Date           Description                               Author
 *-----------------------------------------------------------------------------
 * 27 Aug 2003    Initial Creation                          Sharath Kumar  
 * 
 * 16 Dec 2003    Updates for 5.7                           Sharath Kumar                                                          
 *                                                                                 
 * (C) Copyright 2003, Texas Instruments, Inc                                      
 ******************************************************************************/   



#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/spinlock.h> 
#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/generic/avalanche_intc.h>


/* Need to define this type since this before including
 * keypad_hal.h
 */
typedef void KPAD_HAL_OBJ_T;


#include "asm/avalanche/generic/keypad.h"
#include "keypad_hal.h"


#define TI_KEYPAD_VERSION                 "0.1"


/*********************TYPEDEFs*****************************************/


/* Type definition of driver object
 */
typedef struct kpad_dev {                                                    
	KEY_EVENT  *buffer;    /* Circular buffer to log key events */                                                      
	wait_queue_head_t inq; /* Queue for processes waiting for key event */                                               
	void *kpad_handle;     /* handle for keypad hal */                               
	struct fasync_struct *async_queue; /* handle for async notification */ 
	devfs_handle_t handle; /* only used if devfs is there */             
	spinlock_t  lock;      /* mutex lock */               
	int write_index;       /* write pointer to circular buffer */                                                     
	int last_key_pressed;  /* Used in identifying the key during release event */ 
	int usage_count;       /* Indicates the module usage count */                                                  
}KPAD_DEV;                                                                  


/*****************STATIC Declarations *********************/

static void kpad_driver_isr(int isr,void *data, struct pt_regs *reg);



static devfs_handle_t devfs_dir_handle = NULL;
static KPAD_DEV  *keypad_dev = NULL;

static int row_map=ROW_MAP;
static int column_map=COLUMN_MAP;
static int debounce_time=DEBOUNCE_TIME;

/* KEYPAD proc entry pointer */      
static struct proc_dir_entry *p_kpad_proc;
                                    

/* Proc function to display driver version */                                                                       
static int                                                                                     
kpad_read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)        
{                                                                                              
	int len=0;                                                                                 
                                                                                               
	len += sprintf(buf +len,"\nTI Linux Keypad Driver Version %s\n",TI_KEYPAD_VERSION);         
	return len;                                                                                
}                                                                                              

static ssize_t tikpad_read( struct file * file, char * buf,
                                                  size_t count, loff_t *ppos )
{
	int read_index;
	int write_index;
	int max_events;
	int req_events;
	KPAD_DEV  *kpad_dev=file->private_data;

	read_index = *ppos; 
	req_events = count / sizeof(KEY_EVENT); 

	if(read_index == kpad_dev->write_index)
	{ 
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		/* buffer is empty, so wait */
		if (wait_event_interruptible(kpad_dev->inq, (read_index != kpad_dev->write_index)))
			return -ERESTARTSYS;
	}

	write_index = kpad_dev->write_index;
	if(write_index > read_index)
	{
		/* There is no wrap around */
		max_events = write_index - read_index;
		req_events = (req_events > max_events)? max_events: req_events;
       
		if (copy_to_user(buf, (char *)&kpad_dev->buffer[read_index], req_events * sizeof(KEY_EVENT))) 
			return -EFAULT;
	}
	else
	{
		int num_events = 0;

		/* There is wrap around. We may have to 
		 * perform two separate copies
		 */
		max_events = (BUFFER_SIZE - read_index) + write_index;
		req_events = (req_events > max_events)? max_events: req_events;
			
		num_events = ((BUFFER_SIZE - read_index) > req_events)?req_events:(BUFFER_SIZE - read_index); 
		if (copy_to_user(buf, (char *)&kpad_dev->buffer[read_index], num_events * sizeof(KEY_EVENT))) 
			return  -EFAULT;
        
		if(req_events > num_events)
		{
			/* Copy from start of the buffer */
			if (copy_to_user(buf, (char *)&kpad_dev->buffer[0], (req_events - num_events) * sizeof(KEY_EVENT))) 
				return  -EFAULT;
		}    
	}

	read_index = (read_index + req_events) % BUFFER_SIZE;
	*ppos = read_index;

	return (req_events * sizeof(KEY_EVENT));
}


static int tikpad_ioctl( struct inode * inode, struct file * file,
				             unsigned int cmd, unsigned long arg )
{
	KPAD_DEV  *kpad_dev=file->private_data;
	int ret;
	int flags;

	spin_lock_irqsave(&kpad_dev->lock, flags); 

	if (cmd == TI_KEY_DEBOUNCE_VALUE)
		ret=  ti_kpad_hal_ioctl(kpad_dev->kpad_handle,cmd,arg);

	else
		ret = -EINVAL;

	spin_unlock_irqrestore(&kpad_dev->lock, flags);
	return ret;

}


static int tikpad_open( struct inode * inode, struct file * file )
{
	KPAD_DEV  *kpad_dev;
	int flags;

	file->private_data = keypad_dev;
	kpad_dev=file->private_data;

	spin_lock_irqsave(&kpad_dev->lock, flags); 
	if(!kpad_dev->usage_count)
	{
		ti_kpad_hal_start(kpad_dev->kpad_handle);

		/* code for registering irq */
		request_irq(LNXINTNUM(AVALANCHE_KPAD_CNTL_INT),kpad_driver_isr,0,"keypad",kpad_dev);
	}
	
	kpad_dev->usage_count++;
	file->f_pos = kpad_dev->write_index;
	spin_unlock_irqrestore(&kpad_dev->lock, flags);
	
	return 0;
}

static int tikpad_release( struct inode * inode, struct file * file )
{
	KPAD_DEV  *kpad_dev=file->private_data;
	int flags;

	spin_lock_irqsave(&kpad_dev->lock, flags); 
	keypad_dev->usage_count--;
	if(!keypad_dev->usage_count)
	{
		kpad_dev->write_index = 0;
		ti_kpad_hal_stop(kpad_dev->kpad_handle);
		free_irq(LNXINTNUM(AVALANCHE_KPAD_CNTL_INT),NULL);
	}
	
	spin_unlock_irqrestore(&kpad_dev->lock, flags);
	return 0;
}

int tikpad_fasync(int fd, struct file *file, int mode)
{
	KPAD_DEV *p_kpad_dev=file->private_data;
	return fasync_helper(fd, file, mode, &p_kpad_dev->async_queue);
}

static int tikpad_flush(struct file *file)
{
	/* remove the current process from async queue */
	tikpad_fasync(-1, file, 0);
	return 0;
}

struct file_operations tikpad_fops = {
	owner:  THIS_MODULE,
	read:   tikpad_read,
	ioctl:	tikpad_ioctl,
	fasync: tikpad_fasync,
	flush:  tikpad_flush,
	open:	tikpad_open,
	release:  tikpad_release
};

static inline void notify_event(KPAD_DEV  *kpad_dev)
{
	wake_up_interruptible(&kpad_dev->inq);

	/* Now there is some data to read
	 * Send SIGIO signal to waiting processes */
	if (kpad_dev->async_queue)
		kill_fasync(&kpad_dev->async_queue, SIGIO, POLL_IN);

}


static inline void kpad_add_event(KPAD_DEV *kpad_dev,int key_pressed, int key_press_time)
{	int flags;

	spin_lock_irqsave(&kpad_dev->lock, flags); 
	
	kpad_dev->buffer[kpad_dev->write_index].row = key_pressed & 0xff;
	kpad_dev->buffer[kpad_dev->write_index].column = (key_pressed & 0xff00) >> 8;
	kpad_dev->buffer[kpad_dev->write_index].key_press_time = key_press_time;
	kpad_dev->write_index = (kpad_dev->write_index + 1) % BUFFER_SIZE;
	notify_event(kpad_dev);

	spin_unlock_irqrestore(&kpad_dev->lock, flags);
	
}

void keypad_bh(unsigned long data_ptr)
{
	KPAD_DEV  *kpad_dev = (*((KPAD_DEV **)data_ptr));
	int key_pressed;

	key_pressed=ti_kpad_hal_key_scan(kpad_dev->kpad_handle);
	if(key_pressed >= 0){
		kpad_add_event(kpad_dev,key_pressed,0);
		kpad_dev->last_key_pressed = key_pressed;       
	}

	/* enable interrupt */ 
	ti_kpad_hal_ioctl(kpad_dev->kpad_handle,TI_KEY_RELEASE_DETECT,0);
}

DECLARE_TASKLET(keypad_tasklet, keypad_bh, (unsigned long)&keypad_dev);

static void kpad_driver_isr(int isr,void *data, struct pt_regs *reg)
{
	KPAD_DEV  *kpad_dev=data;
	int ret;
	static unsigned int prev_jiffies;
    
	ret=ti_kpad_hal_isr(kpad_dev->kpad_handle);
	switch(ret)
	{
		case 1: /* key press event */

			prev_jiffies = jiffies;
			/* call bottom half */
			tasklet_schedule(&keypad_tasklet);         
		break;

		case 2: /* key release event */
		{
			int key_pressed = kpad_dev->last_key_pressed;
			int key_press_time = (jiffies - prev_jiffies) * (1000/HZ); // in milisecs

			kpad_add_event(kpad_dev,key_pressed,key_press_time);
			
		}
			/* enable interrupt */ 
			ti_kpad_hal_ioctl(kpad_dev->kpad_handle,TI_KEY_PRESS_DETECT,0); 
		break;
                      
		default:
			/* enable interrupt */ 
			ti_kpad_hal_ioctl(kpad_dev->kpad_handle,TI_KEY_PRESS_DETECT,0); 
	}
}

int __init
tikpad_init(void)
{
	char devname[10];
	KEYPAD_DEV_INFO_T kpad_info;

	keypad_dev = kmalloc(sizeof(KPAD_DEV), GFP_KERNEL);
	if(!keypad_dev)
		return -ENOMEM;

	keypad_dev->buffer=kmalloc(BUFFER_SIZE * sizeof(KEY_EVENT) , GFP_KERNEL);

	if(!keypad_dev->buffer)
        {
                kfree(keypad_dev);
		return -ENOMEM;
        }

	keypad_dev->write_index = 0;
	keypad_dev->usage_count = 0;
	kpad_info.row_map = row_map;
	kpad_info.column_map = column_map;
	kpad_info.debounce_time = debounce_time;
	kpad_info.base_address = AVALANCHE_KEYPAD_CONTROL_BASE;
	kpad_info.module_freq = avalanche_get_vbus_freq();

	keypad_dev->kpad_handle = ti_kpad_hal_init(&kpad_info);
	if(!keypad_dev->kpad_handle)
	{ 
		printk("Error: hal not initialized\n");
		return -EIO;
	}

	devfs_dir_handle = devfs_mk_dir(NULL, "keypad", NULL);
	sprintf(devname, "0");
	keypad_dev->handle=devfs_register(devfs_dir_handle, devname, DEVFS_FL_AUTO_DEVNUM, 0, 0,
		S_IFCHR | S_IRUGO | S_IWUGO, &tikpad_fops,NULL);
        
	keypad_dev->lock = SPIN_LOCK_UNLOCKED;
	init_waitqueue_head(&keypad_dev->inq);
	keypad_dev->async_queue = NULL;
	
	/* Creating proc entry for the devices */                                                
	p_kpad_proc = create_proc_read_entry("avalanche/keypad_ver", 0, NULL, kpad_read_proc, NULL);
	return 0;
}

void tikpad_exit(void)
{
	ti_kpad_hal_cleanup(keypad_dev->kpad_handle);
	devfs_unregister(keypad_dev->handle);
	devfs_unregister(devfs_dir_handle);
	kfree(keypad_dev->buffer);
	kfree(keypad_dev);
	remove_proc_entry("avalanche/keypad_ver", NULL);
}

MODULE_PARM(row_map,"i");
MODULE_PARM(column_map,"i");
MODULE_PARM(debounce_time,"i");
MODULE_DESCRIPTION("Driver for TI KEYPAD");
MODULE_AUTHOR("Maintainer: Sharath Kumar <krs@ti.com>");

module_init(tikpad_init);
module_exit(tikpad_exit);
