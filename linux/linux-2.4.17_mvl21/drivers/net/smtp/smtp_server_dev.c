#include <linux/types.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/atomic.h>
#include <linux/in.h>
//cjg:2004-3-11
extern unsigned long LastTimeStamp;// updata by NTP server
extern unsigned int TimeZone;

extern int email_alert_enable; /* 0:disable; 1:enable */
/*!--2004-3-11:just for test*/
#define printk(fmt, args...) do{\
	debug_printk(fmt, ## args);\
	trace(fmt, ## args);\
}while(0)
/*-------2004-3-11--------!*/
//#define _SMTP_SERVER_DEV_DEBUG_
#if 0
#ifdef _SMTP_SERVER_DEV_DEBUG_
#define DPRINTK(args...) do{printk(args)}while(0);
#else
#define DPRINTK(args...) do{}while(0);
#endif
#endif

#define SMTP_SERVER_DEV "smtp_server_dev"
static atomic_t smtp_dev_busy = ATOMIC_INIT(0);
static spinlock_t smtp_dev_busy_lock = SPIN_LOCK_UNLOCKED;

void smtp_dev_lock(void)
{
	unsigned long flags;
	int available = 0;
	while(!available){
		spin_lock_irqsave(&smtp_dev_busy_lock, flags);
		if(!atomic_read(&smtp_dev_busy))
		{
			available = 1;
			atomic_set(&smtp_dev_busy, 1);
		}
		spin_unlock_irqrestore(&smtp_dev_busy_lock, flags);
	}
}

void smtp_dev_unlock(void)
{
	unsigned long flags;
	spin_lock_irqsave(&smtp_dev_unlock, flags);
	atomic_set(&smtp_dev_busy, 0);
	spin_unlock_irqrestore(*&smtp_dev_unlock, flags);
}

static int smtp_dev_open(struct inode *inode, struct file *filp)
{
	MOD_INC_USE_COUNT;
	return 0;
}

static int smtp_dev_release(struct inode *inode, struct file *filp)
{
	MOD_DEC_USE_COUNT;
	return 0;
}
static int my_atoi(char *str)
{
	int val = 0;
	for(;; str ++)
	{
		switch(*str)
		{
			case '0'...'9':
				val = 10 * val + (*str - '0');
				break;
			default:
				return val;
		}
	}
}

static unsigned int my_strtohex(char *str)
{
	unsigned int val = 0;

	while(*str)
	{
		if((*str >= '0') && (*str <= '9'))
		{
			val = (val << 4) | (*str - '0');
		}
		if((*str >= 'a') && (*str <= 'z'))
		{
			val = (val << 4) | (*str - 'a' + 10) ;
		}
		if((*str >= 'A') && (*str <= 'Z'))
		{
			val = (val << 4) | (*str - 'A' + 10);
		}
		str ++;
	}
	return val;
}	
extern __u32 SMTP_SERVER_IP;
//static char smtp_server_ip[20];

volatile unsigned char email2log[120];
volatile unsigned char email2ret[120];

extern unsigned char trace_buf;
extern int e_w_pos;
extern int g_email_thresholds;
extern int matched_count;
#define TRACE_BUFF_SIZE 1024
static ssize_t smtp_dev_write(struct file *filp, const char *buff, size_t count,loff_t *offp)
{
	char tmp[120]; 
	char *name = tmp;
	char *pos;
	ssize_t ret;
	if(count > sizeof(tmp))
	{
		if(!(name = kmalloc(count, GFP_KERNEL)))
			return -ENOMEM;
	}
	if(copy_from_user(name, buff, count))
	{
		ret = -EFAULT;
		goto done;
	}
	smtp_dev_lock();
	switch(*name)
	{
		case '1':
			memset(email2log, 0, sizeof(email2log));
			name ++;
			strncpy(email2log, name, sizeof(email2log));
			email2log[strlen(email2log)] = '\0';
			name --;
			break;
		case '2':
			memset(email2ret, 0, sizeof(email2ret));
			name ++;
			strncpy(email2ret, name, sizeof(email2ret));
			email2ret[strlen(email2ret)] = '\0';
			name --;
			/*!---------2004-3-11;just for test--------
			printk("1:this is a test\n");
			printk("2:hhhhhhhhhhhhhhhhahhhhhhhhhhahhhhhhhh\n");
			printk("3:yyyyyyyyyyyxxxxxxopoopppppppppppppp\n");
			printk("4:::::::::3dafdasffffffffasdfadsfasd\n");
			printk("5::::::::::::::::::::::::\n");
			send_mail("test", "test");
			-------------------------------------------!*/
			break;
		case '3':
			name ++;
			if(!strncmp(name, "1", 1))
			    email_alert_enable = 1;
			else
			{	
			    email_alert_enable = 0;
			    /*when disable the email_alert, we need clear the
			     * treace buffuer??*/
			    memset(&trace_buf, 0, TRACE_BUFF_SIZE);
			    e_w_pos = 0;
			    matched_count = 0;
			    g_email_thresholds = 20;
			}
			name --;
			break;
		case '4':
			name ++;
			g_email_thresholds = my_atoi(name);
			name --;
			break;
		case '5':
			name ++;
			SMTP_SERVER_IP = my_strtohex(name)/*my_atoi(tmp)*/;
			name --;
			break;
		case '6':
			name ++;
			printk("LastTimeStamp:%s\n", name);
			LastTimeStamp = my_strtohex(name);			
			name --;
			break;
		case '7':
			name ++;
			TimeZone = my_strtohex(name);
			name --;
			break;
	}
	smtp_dev_unlock();
done:
	if(name != tmp)
		kfree(name);
	return ret;
}


static devfs_handle_t devfs_handle = NULL;

struct file_operations smtp_dev_fops = {
	open:smtp_dev_open,
	write:smtp_dev_write,
	release:smtp_dev_release
};

void smtp_proc_init(void);

static int __init smtp_dev_init(void)
{
	devfs_handle = devfs_register(
			NULL, 
			SMTP_SERVER_DEV, 
			DEVFS_FL_AUTO_DEVNUM, 
			0, 0, 
			S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
			&smtp_dev_fops,
			NULL);
	smtp_proc_init();
	return 0;
}

static void smtp_dev_cleanup(void)
{
	devfs_unregister(devfs_handle);
}

//extern unsigned char sock_run_pos;
extern unsigned char *val_buf;
static int smtp_proc_info(char *buffer, char **start, off_t offset, int length)
{
	int len = 0;
	off_t begin = 0;
	off_t pos = 0;
	int size;
	
	size = sprintf(buffer, "LastTimeStamp:%08x - TimeZone:%08x\nw_pos = %d\nemail_enable: %d-\n%s\n%s\n%08x\n",
		       	LastTimeStamp,
			TimeZone,
			e_w_pos,
			email_alert_enable,
			(unsigned char *)email2log,
			(unsigned char *)email2ret,
			SMTP_SERVER_IP);
	pos += size;
	len += size;
	
	*start = buffer + offset;
	len -= offset;
	if(len > length)
		len = length;
	if(len < 0)
		len = 0;
	return len;
}
#include <linux/proc_fs.h>

void smtp_proc_init()
{
	proc_net_create("smtp_server_ip", 0, smtp_proc_info);
}

module_init(smtp_dev_init);
module_exit(smtp_dev_cleanup);
