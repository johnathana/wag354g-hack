#include <linux/types.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/atomic.h>
#include <asm/avalanche/generic/adam2_env.h>
#include <asm/avalanche/generic/ticfg.h>

/*
#define _TICFG_DEBUG_
*/

#ifdef _TICFG_DEBUG_
#define DPRINTK(args...) do { printk(args); } while(0);
#else
#define DPRINTK(args...) do { } while(0);
#endif

#define TICFG_DEVNAME        "ticfg"
#define TICFG_ENV_DIR        "env"

static int ticfg_open(struct inode *inode, struct file *filp);
static int ticfg_release(struct inode *inode, struct file *filp);
static ssize_t ticfg_read(struct file *filp, char *buff,
			  size_t count, loff_t *offp);
static ssize_t ticfg_write(struct file *filp, const char *buff,
			   size_t count, loff_t *offp);
static int ticfg_ioctl(struct inode *inode, struct file *file,
		     u_int cmd, u_long arg);

typedef struct {
	struct mtd_info *mtd;
	int count;
	int size;
	wait_queue_head_t wq;
} ticfg_dev_t;

static ticfg_dev_t ticfg_dev;

static devfs_handle_t devfs_handle = NULL;

static atomic_t ticfg_busy = ATOMIC_INIT(0);
static spinlock_t ticfg_busy_lock = SPIN_LOCK_UNLOCKED;

static struct proc_dir_entry *ticfg_proc_dir;
static struct proc_dir_entry *ticfg_proc_env_dir;

struct file_operations ticfg_fops = {
	open: ticfg_open,
	read: ticfg_read,
	write: ticfg_write,
	release: ticfg_release,
	ioctl: ticfg_ioctl
};

/* Acquire the mutual exclusion lock for the configuration region. 
 *
 * We do it this way because we do not know which context it will be
 * called from and because we need to allow sleepable calls when we
 * own the lock.  These requirements rule out the usual spinlocks and
 * semaphores.
 */
void ticfg_lock_region(void)
{
	unsigned long flags;
	int available=0;

	while(!available) {
		spin_lock_irqsave(&ticfg_busy_lock, flags);
		if(!atomic_read(&ticfg_busy)) {
			available=1;
			atomic_set(&ticfg_busy,1);
		}
		spin_unlock_irqrestore(&ticfg_busy_lock, flags);
	}
}

/* Release the mutual exclusion lock for the configuration region */
void ticfg_unlock_region(void)
{
	unsigned long flags;

	spin_lock_irqsave(&ticfg_busy_lock, flags);
	atomic_set(&ticfg_busy,0);
	spin_unlock_irqrestore(&ticfg_busy_lock, flags);

}

static void mtd_erase_callback (struct erase_info *instr)
{
	DPRINTK("ticfg: mtd_erase_callback\n");
	wake_up((wait_queue_head_t *)instr->priv);
}

static int ticfg_erase_mtd_region(struct mtd_info *mtd, unsigned long offset, unsigned long len)
{
	struct erase_info *erase;

	int ret=0;
	DECLARE_WAITQUEUE(wait,current);
	wait_queue_head_t wait_q;

	DPRINTK("ticfg: mtd_erase_mtd_region(offset=%lu len=%lu)\n", offset, len);

	init_waitqueue_head(&wait_q);
	
	erase=kmalloc(sizeof(struct erase_info), GFP_KERNEL);
	if (!erase) 
		return -ENOMEM;
	memset(erase,0,sizeof(struct erase_info));
	
	erase->mtd=mtd;
	erase->callback=mtd_erase_callback;
	erase->addr=offset;
	erase->len=len;
	erase->priv=(u_long)&wait_q;

	ret=mtd->erase(mtd, erase);
	if( !ret ) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue( &wait_q, &wait);
		if( erase->state!=MTD_ERASE_DONE &&
		    erase->state!=MTD_ERASE_FAILED) {
			schedule();
		}
		
		remove_wait_queue(&wait_q, &wait);
		set_current_state(TASK_RUNNING);
		
		ret=(erase->state==MTD_ERASE_FAILED)?-EIO:0;
	} else {
		printk(KERN_ERR "Failed to erase mtd, region [0x%x,0x%x]\n", 
		       erase->addr,erase->len);
	}
	
	kfree(erase);

	return ret;
}


static int ticfg_read_mtd_region(struct mtd_info *mtd, off_t offset, size_t count, 
				 size_t *bytes_read, char *kbuf)
{
	int ret=0;
	int retlen;

	DPRINTK("ticfg: read_mtd_region() offset=%lu count=%lu\n", (u_long) offset, (u_long) count);

	*bytes_read=0;
	while(count) {
		ret=MTD_READ(mtd, offset, count, &retlen, kbuf);
		if(!ret) {
			offset+=retlen;
			count-=retlen;
			kbuf+=retlen;
			*bytes_read+=retlen;
		}
		else {
			return ret;
		}
	}
	
	return 0;		
	
}


static int ticfg_write_mtd_region(struct mtd_info *mtd, off_t offset, size_t count, 
			    size_t *bytes_written, char *kbuf)
{
	int ret=0;
	int retlen;
	
	DPRINTK("ticfg: write_mtd_region() offset=%lu count=%lu\n", (u_long) offset, (u_long) count);
                
	*bytes_written=0;
	while (count) {
		ret = MTD_WRITE(mtd, offset, count, &retlen, kbuf);
                if (!ret) {
                        offset += retlen;
                        count -= retlen;
                        kbuf += retlen;
                        *bytes_written += retlen;
                }
                else {
			return ret;
                }
        }

	return 0;
}

/* Read from the cfgman area of the configuration region */
static ssize_t ticfg_read(struct file *filp, char *buf, size_t count, loff_t *ppos)
{
	ticfg_dev_t *dev=(ticfg_dev_t *)filp->private_data;
	size_t retlen=0;
	char *kbuf;
	int ret=0;
	off_t offset;

	DPRINTK("ticfg: read() offset=%lu count=%lu\n", (u_long) *ppos, (u_long)  count);

	if( *ppos+count > CFGMAN_MTD_SIZE )
		count=CFGMAN_MTD_SIZE-*ppos;

	if( !count) return 0;

	kbuf=kmalloc(count,GFP_KERNEL);
	if(!kbuf) {
		return -ENOMEM;
	}

	ticfg_lock_region();

	offset=*ppos+CFGMAN_MTD_OFFSET;
	ret=ticfg_read_mtd_region(dev->mtd, offset, count, &retlen, kbuf);
	if(ret) {	
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}

	ticfg_unlock_region();

	*ppos+=retlen;
	if( copy_to_user(buf, kbuf, retlen)) {
		kfree(kbuf);
		return -EFAULT;
	}
 
	kfree(kbuf);
	return retlen;
}



/* Write to the cfgman area of the configuration region. */
static ssize_t ticfg_write(struct file *filp, const char *buf,
			   size_t count, loff_t *ppos)
{
	ticfg_dev_t *dev=(ticfg_dev_t *)filp->private_data;
	size_t retlen=0;
	int ret=0;
	off_t offset;

	DPRINTK("ticfg: write(0x%08lx, 0x%08lx, %lu, %lu)\n",
		(u_long) filp, (u_long) buf, (u_long) count,(u_long) *ppos);

	if( *ppos+count > CFGMAN_MTD_SIZE )
		count=CFGMAN_MTD_SIZE-*ppos;

	if( !count) return 0;

	ticfg_lock_region();

	offset=*ppos+CFGMAN_MTD_OFFSET;
	ret=ticfg_write_mtd_region(dev->mtd, offset, count, &retlen, (char *) buf);
	if(ret) {
		ticfg_unlock_region();
		return ret;
	}
	
	ticfg_unlock_region();
	
	*ppos+=retlen;
	return retlen;
}

/* Erase the cfgman area in the configuration region */
int ticfg_erase_cfgman(struct mtd_info *mtd)
{
	char *kbuf;
	int ret;
	int retlen;

	DPRINTK("ticfg: erase_cfgman()\n");

 	kbuf=kmalloc(ENV_VAR_MTD_SIZE,GFP_KERNEL);
	if(!kbuf)
		return -ENOMEM;

	ticfg_lock_region();

	/* save the current set of environment variables */
	ret=ticfg_read_mtd_region(mtd, ENV_VAR_MTD_OFFSET, ENV_VAR_MTD_SIZE, &retlen, kbuf);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}

	/* erase the entire region */
	ret=ticfg_erase_mtd_region(mtd, TICFG_MTD_OFFSET, TICFG_MTD_SIZE);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}

	/* restore the current set of environment variables */
	ret=ticfg_write_mtd_region(mtd, ENV_VAR_MTD_OFFSET, ENV_VAR_MTD_SIZE, &retlen, kbuf);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}	

	ticfg_unlock_region();
	kfree(kbuf);
	return 0;
}

/* Erase the environment variables area in the configuration region */
int ticfg_erase_env_vars(struct mtd_info *mtd)
{
	char *kbuf;
	int ret;
	int retlen;

	DPRINTK("ticfg: erase_envars()\n");

 	kbuf=kmalloc(CFGMAN_MTD_SIZE,GFP_KERNEL);
	if(!kbuf)
		return -ENOMEM;

	ticfg_lock_region();

	/* save the current configman configuration */
	ret=ticfg_read_mtd_region(mtd, CFGMAN_MTD_OFFSET, CFGMAN_MTD_SIZE, &retlen, kbuf);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}	

	/* erase the entire region */
	ret=ticfg_erase_mtd_region(mtd, TICFG_MTD_OFFSET, TICFG_MTD_SIZE);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}

	/* restore the current configman configuration */
	ret=ticfg_write_mtd_region(mtd, CFGMAN_MTD_OFFSET, CFGMAN_MTD_SIZE, &retlen, kbuf);
	if(ret) {
		ticfg_unlock_region();
		kfree(kbuf);
		return ret;
	}	
	
	ticfg_unlock_region();
	kfree(kbuf);
	return ret;
}

static int ticfg_ioctl(struct inode *inode, struct file *filp,
		     u_int cmd, u_long arg)
{
	ticfg_dev_t *dev=(ticfg_dev_t *)filp->private_data;
	int ret=0;

	DPRINTK("ticfg: ioctl(cmd 0x%08x, arg 0x%08lx)\n", cmd, arg);

	switch(cmd) {
	case TICFG_IOCTL_ERASE:
	{
		ret=ticfg_erase_cfgman(dev->mtd);
		if(ret)
			return ret;

		break;
	}
	default:
		break;
	}

	return 0;
}

static int ticfg_proc_read_env(char *page, char **start, off_t pos, int count,
			       int *eof, void *data)
{
	int len;
	int i;
	char *pvar;
	char *pval;

	if( pos > 0 ) {
		len=0;
		return 0;
	}

	/* Scan thru the flash, looking for each possible variable index */
	len=0;
	for(i=0; i<MAX_ENV_ENTRY; i++) {

		/* TODO: Make sure the size does not exceed the page size */
		if( (pvar=adam2_env_get_variable(i))!=NULL ) {

			if( (pval=adam2_env_get_value(pvar)) != NULL) {
				len += sprintf(page+len, "%s\t%s\n", pvar, pval);
				kfree(pval);
			}

			kfree(pvar);
		}
	}

	*eof=1;
	return len;
}

static int ticfg_proc_write_env(struct file *file, const char *buffer, 
				unsigned long count, void *data)
{
	char envdata[MAX_ENV_DATA_LEN];
	char *pvar;
	char *pval;

	/* TODO: Check for count > len */
	if( copy_from_user(envdata, buffer, count) ) {
		return -EFAULT;
	}

	/* extract the variable/value pair */
	if( count )
		envdata[count-1]='\0';
	else
		envdata[count]='\0';
	
	pvar=envdata;
	pval=strpbrk(envdata," \t");
	if(pval) *pval++='\0';

	if(pval) {
		/* write the variable/value pair to flash */
		if(adam2_env_set_variable(pvar, pval) != 0)
		{
			printk(KERN_WARNING "Defragginig environment variable region.\n");
			adam2_env_defrag();
			if( adam2_env_set_variable(pvar, pval) != 0 )
				printk(KERN_ERR "Failed to write %s to environment variable region.\n", pvar);
		}

	} else {
		/* We have no way to distinguish between unsetting a
		 * variable and setting a non-value variable.
		 *
		 * Consequently, we will treat any variable
		 * without a value as an unset request.
		 */
		adam2_env_unset_variable(pvar);
	}	

	return count;
}


static int ticfg_open(struct inode *inode, struct file *filp) 
{
	ticfg_dev_t *dev;

	DPRINTK("ticfg: open\n");

	MOD_INC_USE_COUNT;
	
	dev=(ticfg_dev_t *)filp->private_data;
	if(!dev) {
		dev=&ticfg_dev;
		filp->private_data=dev;
	}

	dev->mtd=get_mtd_device(NULL,CFGMAN_MTD);
        if(!dev->mtd) {
                printk(KERN_ERR "ticfg: can't get mtd%d\n", CFGMAN_MTD);
		return -EIO;
        }
	return 0;
}

static int ticfg_release(struct inode *inode, struct file *filp)
{
	ticfg_dev_t *dev=(ticfg_dev_t *)filp->private_data;

	DPRINTK("ticfg: release\n");

	if( dev->mtd->sync ) 
		dev->mtd->sync(dev->mtd);

	put_mtd_device(dev->mtd);
	
	return 0;
}


static int __init ticfg_init(void)
{
	DPRINTK("ticfg: init\n");

	devfs_handle=devfs_register(NULL, TICFG_DEVNAME, DEVFS_FL_AUTO_DEVNUM,
				    0,0,
				    S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 
				    &ticfg_fops, &ticfg_dev);
	
	memset( &ticfg_dev, 0, sizeof(ticfg_dev_t));

	ticfg_proc_dir=proc_mkdir(TICFG_DEVNAME,NULL);
	if( ticfg_proc_dir==NULL ) {
		printk(KERN_ERR "ticfg: Unable to create /proc/%s entry\n", TICFG_DEVNAME);
		return -ENOMEM;
	}
	
	ticfg_proc_env_dir=create_proc_entry(TICFG_ENV_DIR, 0644,ticfg_proc_dir);
	if( ticfg_proc_env_dir==NULL ) {
		printk(KERN_ERR "TICFG: Unable to create /proc/%s/%s entry\n", TICFG_DEVNAME, TICFG_ENV_DIR);
		remove_proc_entry("ticfg",NULL);
		return -ENOMEM;
	}
	ticfg_proc_env_dir->read_proc=ticfg_proc_read_env;
	ticfg_proc_env_dir->write_proc=ticfg_proc_write_env;

	return 0;
}

static void ticfg_cleanup(void)
{
	devfs_unregister(devfs_handle);

	remove_proc_entry(TICFG_ENV_DIR,ticfg_proc_dir);
	remove_proc_entry(TICFG_DEVNAME, NULL);
}

module_init(ticfg_init);
module_exit(ticfg_cleanup);
