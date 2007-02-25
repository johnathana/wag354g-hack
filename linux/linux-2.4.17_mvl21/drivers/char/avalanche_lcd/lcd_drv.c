/*******************************************************************************   
 * FILE PURPOSE:    LCD Module Driver Source                                       
 *******************************************************************************   
 * FILE NAME:       lcd_drv.c                                                   
 *                                                                                 
 * DESCRIPTION:     Source code for Linux LCD Driver                             
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
#include <linux/init.h>                  
#include <linux/types.h>                 
#include <linux/errno.h>                 
#include <linux/slab.h>                  
#include <linux/ioport.h>                
#include <linux/fcntl.h>                 
#include <linux/interrupt.h>             
#include <linux/sched.h>                 
#include <linux/proc_fs.h>
#include <linux/mm.h>                    
#include <asm/io.h>                      
#include <asm/uaccess.h>                 
#include <asm/system.h>                  
#include <linux/delay.h>                 
#include <linux/devfs_fs_kernel.h>       
#include <linux/wait.h>
#include <asm/avalanche/avalanche_map.h> 

typedef void LIDD_HAL_OBJ_T;

#include "lidd_hal.h"
#include <asm/avalanche/generic/lcd.h>


#define    TI_LCD_VERSION                 "0.1"
#define    MAX_LCD_SIZE                    (MAX_ROWS)*(MAX_COLS)


/* TYPEDEF for LCD dev object */
typedef struct lcd_dev {                                            
	LIDD_HAL_OBJ_T *hal_handle;/* handle for hal */             
	devfs_handle_t fs_handle; /* only used if devfs is there */ 
	struct semaphore sem; /* mutual exclusion semaphore */      
	int rows;                                                   
	int columns;                                                
}LCD_DEV;                                                          

static devfs_handle_t devfs_dir_handle = NULL;
static LCD_DEV  *lcd_dev;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int rows=DEFAULT_ROWS;
static int columns=DEFAULT_COLS;



/* LCD proc entry pointer */      
static struct proc_dir_entry *p_lcd_proc;


/* Proc function to display driver version */                                                                       
static int                                                                                     
lcd_read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)        
{                                                                                              
	int len=0;                                                                                 
                                                                                               
	len += sprintf(buf +len,"\nTI Linux LCD Driver Version %s\n",TI_LCD_VERSION);         
	return len;                                                                                
}                                                                                              

static ssize_t tilcd_read( struct file * file, char * buf,
                                                  size_t count, loff_t *ppos )
{
	LCD_DEV  *lcd_dev=file->private_data;
	int ret = -1;
	char temp_buf[MAX_LCD_SIZE];

	/* Limit the count to max LCD size if it is greater than that */
	count = (count > MAX_LCD_SIZE)? MAX_LCD_SIZE:count;
 
	if(down_interruptible(&lcd_dev->sem)) 
	{
		return -ERESTARTSYS;
	}

	ret = ti_lidd_hal_read(lcd_dev->hal_handle,temp_buf,count);

	if(ret > 0)
	{
		if(copy_to_user(buf,temp_buf,ret))
			ret = -EFAULT;
	}

	up(&lcd_dev->sem);
	return ret;
}


static ssize_t tilcd_write( struct file * file, const char * buf,
                                                    size_t count, loff_t *ppos )
{
	LCD_DEV  *lcd_dev=file->private_data;
	int ret = -1;
	char temp_buf[MAX_LCD_SIZE];

	if (down_interruptible(&lcd_dev->sem)) 
	{
		return -ERESTARTSYS;
	}
        
	/* Limit the count to max LCD size if it is greater than that */
	count = (count > MAX_LCD_SIZE)? MAX_LCD_SIZE:count;

	if(copy_from_user(temp_buf,buf,count))
		ret = -EFAULT;
	else
	{
		ret = ti_lidd_hal_write(lcd_dev->hal_handle,temp_buf,count);
	}

	up(&lcd_dev->sem);
	return ret;
}


static int tilcd_ioctl( struct inode * inode, struct file * file,
				             unsigned int cmd, unsigned long arg )
{
	LCD_DEV  *lcd_dev=file->private_data;
	int ret = 0;
	LCD_POS lcd_pos;
	unsigned long hal_arg = arg;
	unsigned int read_val;

	if (down_interruptible(&lcd_dev->sem)) 
	{
		return -ERESTARTSYS;
	}

	if(cmd == TI_LIDD_GOTO_XY)
	{
		if(copy_from_user(&lcd_pos,(char *)arg,sizeof(LCD_POS)))
			ret = -EFAULT;
	    
		hal_arg = lcd_pos.row | (lcd_pos.column << 8);
	}     

	/* The commands below require something to be written to user buffer */
	if(cmd == TI_LIDD_RD_CMD || cmd == TI_LIDD_RD_CHAR)
	{
		hal_arg = (unsigned long )&read_val;
	}

	ret = ti_lidd_hal_ioctl(lcd_dev->hal_handle,cmd,hal_arg);
	
	switch(cmd)
	{
		case TI_LIDD_RD_CMD:
		case TI_LIDD_RD_CHAR:
			if(copy_to_user((char *)arg,&read_val,sizeof(int)))
				ret = -EFAULT;
		break;

                default:
	}
        
	up(&lcd_dev->sem);
	return ret;
}


static int tilcd_open( struct inode * inode, struct file * file )
{
	LCD_DEV  *lcd_dev=file->private_data;

	ti_lidd_hal_open(lcd_dev->hal_handle);
	return 0;
}

static int tilcd_release( struct inode * inode, struct file * file )
{
	ti_lidd_hal_close(lcd_dev->hal_handle);
	return 0;
}



struct file_operations tilcd_fops = {
	read:   tilcd_read,
	write:  tilcd_write,
	ioctl:	tilcd_ioctl,
	open:	tilcd_open,
	release:  tilcd_release
};

static int __init tilcd_init(void)
{
	char devname[10];
	TI_LIDD_INFO_T lcd_info;

	lcd_dev = kmalloc(sizeof(LCD_DEV), GFP_KERNEL);
	
	if(!lcd_dev)
		return -ENOMEM;

	memset(lcd_dev,0,sizeof(LCD_DEV));
	lcd_info.base_addr = AVALANCHE_LCD_CONTROL_BASE;
	lcd_info.vbus_freq = avalanche_get_vbus_freq();
	lcd_info.cpu_freq = 2 * avalanche_get_vbus_freq();
	lcd_dev->rows = lcd_info.disp_row=rows;
	lcd_dev->columns = lcd_info.disp_col=columns;
	lcd_info.line_wrap = 1;
	lcd_info.cursor_blink = 1;
	lcd_info.cursor_show = 1;
	lcd_info.lcd_type = 4; /* HITACHI */
	lcd_info.num_lcd = NO_LCD_DEVICES;
	lcd_dev->hal_handle = ti_lidd_hal_init(&lcd_info);
	if(!lcd_dev->hal_handle)
	{
		printk("hal not initialized\n");
		return -EIO;
	}
	
	devfs_dir_handle = devfs_mk_dir(NULL, "lcd", NULL);
	sprintf(devname, "0");
	lcd_dev->fs_handle=devfs_register(devfs_dir_handle, devname, DEVFS_FL_AUTO_DEVNUM, 0, 0,
    						S_IFCHR | S_IRUGO | S_IWUGO, &tilcd_fops,lcd_dev);
	
	sema_init(&lcd_dev->sem,1);
	/* Creating proc entry for the devices */                                                
	p_lcd_proc = create_proc_read_entry("avalanche/lcd_ver", 0, NULL, lcd_read_proc, NULL);
	return 0;
}

static void tilcd_exit(void)
{
	devfs_unregister(lcd_dev->fs_handle);
	ti_lidd_hal_cleanup(lcd_dev->hal_handle);
	devfs_unregister(devfs_dir_handle);
	kfree(lcd_dev);
	remove_proc_entry("avalanche/lcd_ver",NULL);
}

MODULE_PARM(rows,"i");
MODULE_PARM(columns,"i");
MODULE_DESCRIPTION("Driver for TI LCD");
MODULE_AUTHOR("Maintainer: Sharath Kumar <krs@ti.com>");


module_init(tilcd_init);
module_exit(tilcd_exit);
