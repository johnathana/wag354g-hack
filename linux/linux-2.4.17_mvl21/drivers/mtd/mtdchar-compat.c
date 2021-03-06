/*
 * $Id: mtdchar-compat.c,v 1.1.1.2 2005/03/28 06:56:09 sparq Exp $
 *
 * Character-device access to raw MTD devices.
 *
 */


#include <linux/mtd/compatmac.h>
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/slab.h>

#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static void mtd_notify_add(struct mtd_info* mtd);
static void mtd_notify_remove(struct mtd_info* mtd);

static struct mtd_notifier notifier = {
	add:	mtd_notify_add,
	remove:	mtd_notify_remove,
};

static devfs_handle_t devfs_dir_handle;
static devfs_handle_t devfs_rw_handle[MAX_MTD_DEVICES];
static devfs_handle_t devfs_ro_handle[MAX_MTD_DEVICES];
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static loff_t mtd_lseek (struct file *file, loff_t offset, int orig)
#else
static int mtd_lseek (struct inode *inode, struct file *file, off_t offset, int orig)
#endif
{
	struct mtd_info *mtd=(struct mtd_info *)file->private_data;

	switch (orig) {
	case 0:
		/* SEEK_SET */
		file->f_pos = offset;
		break;
	case 1:
		/* SEEK_CUR */
		file->f_pos += offset;
		break;
	case 2:
		/* SEEK_END */
		file->f_pos =mtd->size + offset;
		break;
	default:
		return -EINVAL;
	}

	if (file->f_pos < 0)
		file->f_pos = 0;
	else if (file->f_pos >= mtd->size)
		file->f_pos = mtd->size - 1;

	return file->f_pos;
}



static int mtd_open(struct inode *inode, struct file *file)
{
	int minor = MINOR(inode->i_rdev);
	int devnum = minor >> 1;
	struct mtd_info *mtd;

	DEBUG(MTD_DEBUG_LEVEL0, "MTD_open\n");

	if (devnum >= MAX_MTD_DEVICES)
		return -ENODEV;

	/* You can't open the RO devices RW */
	if ((file->f_mode & 2) && (minor & 1))
		return -EACCES;

	mtd = get_mtd_device(NULL, devnum);
	if (!mtd)
		return -ENODEV;
	if (MTD_ABSENT == mtd->type) {
		put_mtd_device(mtd);
		return -ENODEV;
	}

	MOD_INC_USE_COUNT;

	file->private_data = mtd;
		
	/* You can't open it RW if it's not a writeable device */
	if ((file->f_mode & 2) && !(mtd->flags & MTD_WRITEABLE)) {
		put_mtd_device(mtd);
		MOD_DEC_USE_COUNT;
		return -EACCES;
	}
		
	return 0;
} /* mtd_open */

/*====================================================================*/

static release_t mtd_close(struct inode *inode,
				 struct file *file)
{
	struct mtd_info *mtd;

	DEBUG(MTD_DEBUG_LEVEL0, "MTD_close\n");

	mtd = (struct mtd_info *)file->private_data;
	
	if (mtd->sync)
		mtd->sync(mtd);
	
	put_mtd_device(mtd);

	MOD_DEC_USE_COUNT;
	release_return(0);
} /* mtd_close */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
#define FILE_POS *ppos
#else
#define FILE_POS file->f_pos
#endif

/* FIXME: This _really_ needs to die. In 2.5, we should lock the
   userspace buffer down and use it directly with readv/writev.
*/
#define MAX_KMALLOC_SIZE 0x20000

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static ssize_t mtd_read(struct file *file, char *buf, size_t count,loff_t *ppos)
#else
static int mtd_read(struct inode *inode,struct file *file, char *buf, int count)
#endif
{
	struct mtd_info *mtd = (struct mtd_info *)file->private_data;
	size_t retlen=0;
	size_t total_retlen=0;
	int ret=0;
#ifndef NO_MM
	int len;
	char *kbuf;
#endif	
	DEBUG(MTD_DEBUG_LEVEL0,"MTD_read\n");

	if (FILE_POS + count > mtd->size)
		count = mtd->size - FILE_POS;

	if (!count)
		return 0;
	
	/* FIXME: Use kiovec in 2.3 or 2.2+rawio, or at
	 * least split the IO into smaller chunks.
	 */
#ifdef NO_MM	
	ret = MTD_READ(mtd, FILE_POS, count, &retlen, buf);
	if (!ret) {
		FILE_POS += retlen;
		ret = retlen;
	}
	total_retlen = ret;
#else
	while (count) {
		if (count > MAX_KMALLOC_SIZE) 
			len = MAX_KMALLOC_SIZE;
		else
			len = count;

		kbuf=kmalloc(len,GFP_KERNEL);
		if (!kbuf)
			return -ENOMEM;
		
		ret = MTD_READ(mtd, FILE_POS, len, &retlen, kbuf);
		if (!ret) {
			FILE_POS += retlen;
			if (copy_to_user(buf, kbuf, retlen)) {
			        kfree(kbuf);
				return -EFAULT;
			}
			else
				total_retlen += retlen;

			count -= retlen;
			buf += retlen;
		}
		else {
			kfree(kbuf);
			return ret;
		}
		
		kfree(kbuf);
	}
	
#endif	
	return total_retlen;
} /* mtd_read */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static ssize_t mtd_write(struct file *file, const char *buf, size_t count,loff_t *ppos)
#else
static read_write_t mtd_write(struct inode *inode,struct file *file, const char *buf, count_t count)
#endif
{
	struct mtd_info *mtd = (struct mtd_info *)file->private_data;
	size_t retlen;
	size_t total_retlen=0;
	int ret=0;
#ifndef NO_MM
	int len;
	char *kbuf;
#endif

	DEBUG(MTD_DEBUG_LEVEL0,"MTD_write\n");
	
	if (FILE_POS == mtd->size)
		return -ENOSPC;
	
	if (FILE_POS + count > mtd->size)
		count = mtd->size - FILE_POS;

	if (!count)
		return 0;

#ifdef NO_MM	
	ret = MTD_WRITE(mtd, FILE_POS, count, &retlen, buf);
	if (!ret) {
		FILE_POS += retlen;
		ret = retlen;
	}
	total_retlen = ret;
#else
	while (count) {
		if (count > MAX_KMALLOC_SIZE) 
			len = MAX_KMALLOC_SIZE;
		else
			len = count;

		kbuf=kmalloc(len,GFP_KERNEL);
		if (!kbuf) {
			printk("kmalloc is null\n");
			return -ENOMEM;
		}

		if (copy_from_user(kbuf, buf, len)) {
			kfree(kbuf);
			return -EFAULT;
		}
		
	        ret = (*(mtd->write))(mtd, FILE_POS, len, &retlen, kbuf);
		if (!ret) {
			FILE_POS += retlen;
			total_retlen += retlen;
			count -= retlen;
			buf += retlen;
		}
		else {
			kfree(kbuf);
			return ret;
		}
		
		kfree(kbuf);
	}
#endif		
	return total_retlen;
} /* mtd_write */

/*======================================================================

    IOCTL calls for getting device parameters.

======================================================================*/
static void mtd_erase_callback (struct erase_info *instr)
{
	wake_up((wait_queue_head_t *)instr->priv);
}

static int mtd_ioctl(struct inode *inode, struct file *file,
		     u_int cmd, u_long arg)
{
	struct mtd_info *mtd = (struct mtd_info *)file->private_data;
	int ret = 0;
	u_long size;
	
	DEBUG(MTD_DEBUG_LEVEL0, "MTD_ioctl\n");

	size = (cmd & IOCSIZE_MASK) >> IOCSIZE_SHIFT;
	if (cmd & IOC_IN) {
		ret = verify_area(VERIFY_READ, (char *)arg, size);
		if (ret) return ret;
	}
	if (cmd & IOC_OUT) {
		ret = verify_area(VERIFY_WRITE, (char *)arg, size);
		if (ret) return ret;
	}
	
	switch (cmd) {
	case MEMGETREGIONCOUNT:
		if (copy_to_user((int *) arg, &(mtd->numeraseregions), sizeof(int)))
			return -EFAULT;
		break;

	case MEMGETREGIONINFO:
	{
		struct region_info_user ur;

		if (copy_from_user(	&ur, 
					(struct region_info_user *)arg, 
					sizeof(struct region_info_user))) {
			return -EFAULT;
		}

		if (ur.regionindex >= mtd->numeraseregions)
			return -EINVAL;
		if (copy_to_user((struct mtd_erase_region_info *) arg, 
				&(mtd->eraseregions[ur.regionindex]),
				sizeof(struct mtd_erase_region_info)))
			return -EFAULT;
		break;
	}

	case MEMGETINFO:
		if (copy_to_user((struct mtd_info *)arg, mtd,
				 sizeof(struct mtd_info_user)))
			return -EFAULT;
		break;

	case MEMERASE:
	{
		struct erase_info *erase=kmalloc(sizeof(struct erase_info),GFP_KERNEL);
		if (!erase)
			ret = -ENOMEM;
		else {
			wait_queue_head_t waitq;
			DECLARE_WAITQUEUE(wait, current);

			init_waitqueue_head(&waitq);

			memset (erase,0,sizeof(struct erase_info));
			if (copy_from_user(&erase->addr, (u_long *)arg,
					   2 * sizeof(u_long))) {
				kfree(erase);
				return -EFAULT;
			}
			erase->mtd = mtd;
			erase->callback = mtd_erase_callback;
			erase->priv = (unsigned long)&waitq;

			/*
			  FIXME: Allow INTERRUPTIBLE. Which means
			  not having the wait_queue head on the stack.
			  
			  If the wq_head is on the stack, and we
			  leave because we got interrupted, then the
			  wq_head is no longer there when the
			  callback routine tries to wake us up.
			*/
			ret = mtd->erase(mtd, erase);
			if (!ret) {
				set_current_state(TASK_UNINTERRUPTIBLE);
				add_wait_queue(&waitq, &wait);
				if (erase->state != MTD_ERASE_DONE &&
				    erase->state != MTD_ERASE_FAILED)
					schedule();
				remove_wait_queue(&waitq, &wait);
				set_current_state(TASK_RUNNING);

				ret = (erase->state == MTD_ERASE_FAILED)?-EIO:0;
			}
			kfree(erase);
		}
		break;
	}

	case MEMWRITEOOB:
	{
		struct mtd_oob_buf buf;
		void *databuf;
		ssize_t retlen;
		
		if (copy_from_user(&buf, (struct mtd_oob_buf *)arg, sizeof(struct mtd_oob_buf)))
			return -EFAULT;
		
		if (buf.length > 0x4096)
			return -EINVAL;

		if (!mtd->write_oob)
			ret = -EOPNOTSUPP;
		else
			ret = verify_area(VERIFY_READ, (char *)buf.ptr, buf.length);

		if (ret)
			return ret;

		databuf = kmalloc(buf.length, GFP_KERNEL);
		if (!databuf)
			return -ENOMEM;
		
		if (copy_from_user(databuf, buf.ptr, buf.length)) {
			kfree(databuf);
			return -EFAULT;
		}

		ret = (mtd->write_oob)(mtd, buf.start, buf.length, &retlen, databuf);

		if (copy_to_user((void *)arg + sizeof(u_int32_t), &retlen, sizeof(u_int32_t)))
			ret = -EFAULT;

		kfree(databuf);
		break;

	}

	case MEMREADOOB:
	{
		struct mtd_oob_buf buf;
		void *databuf;
		ssize_t retlen;

		if (copy_from_user(&buf, (struct mtd_oob_buf *)arg, sizeof(struct mtd_oob_buf)))
			return -EFAULT;
		
		if (buf.length > 0x4096)
			return -EINVAL;

		if (!mtd->read_oob)
			ret = -EOPNOTSUPP;
		else
			ret = verify_area(VERIFY_WRITE, (char *)buf.ptr, buf.length);

		if (ret)
			return ret;

		databuf = kmalloc(buf.length, GFP_KERNEL);
		if (!databuf)
			return -ENOMEM;
		
		ret = (mtd->read_oob)(mtd, buf.start, buf.length, &retlen, databuf);

		if (copy_to_user((void *)arg + sizeof(u_int32_t), &retlen, sizeof(u_int32_t)))
			ret = -EFAULT;
		else if (retlen && copy_to_user(buf.ptr, databuf, retlen))
			ret = -EFAULT;
		
		kfree(databuf);
		break;
	}

	case MEMLOCK:
	{
		unsigned long adrs[2];

		if (copy_from_user(adrs ,(void *)arg, 2* sizeof(unsigned long)))
			return -EFAULT;

		if (!mtd->lock)
			ret = -EOPNOTSUPP;
		else
			ret = mtd->lock(mtd, adrs[0], adrs[1]);
		break;
	}

	case MEMUNLOCK:
	{
		unsigned long adrs[2];

		if (copy_from_user(adrs, (void *)arg, 2* sizeof(unsigned long)))
			return -EFAULT;

		if (!mtd->unlock)
			ret = -EOPNOTSUPP;
		else
			ret = mtd->unlock(mtd, adrs[0], adrs[1]);
		break;
	}

		
	default:
		DEBUG(MTD_DEBUG_LEVEL0, "Invalid ioctl %x (MEMGETINFO = %x)\n", cmd, MEMGETINFO);
		ret = -ENOTTY;
	}

	return ret;
} /* memory_ioctl */

static struct file_operations mtd_fops = {
#if LINUX_VERSION_CODE >= 0x20300	/* Someone knows when these made their debut? */
	owner:		THIS_MODULE,
#endif
#if LINUX_VERSION_CODE >=0x20200
	llseek:		mtd_lseek,     	/* lseek */
#else
	lseek:		mtd_lseek,     	/* lseek */
#endif
	read:		mtd_read,	/* read */
	write: 		mtd_write, 	/* write */
	ioctl:		mtd_ioctl,	/* ioctl */
	open:		mtd_open,	/* open */
	release:	mtd_close,	/* release */
};


#ifdef CONFIG_DEVFS_FS
/* Notification that a new device has been added. Create the devfs entry for
 * it. */

static void mtd_notify_add(struct mtd_info* mtd)
{
	char name[8];

	if (!mtd)
		return;

	sprintf(name, "%d", mtd->index);
	devfs_rw_handle[mtd->index] = devfs_register(devfs_dir_handle, name,
			DEVFS_FL_DEFAULT, MTD_CHAR_MAJOR, mtd->index*2,
			S_IFCHR | S_IRUGO | S_IWUGO,
			&mtd_fops, NULL);

	sprintf(name, "%dro", mtd->index);
	devfs_ro_handle[mtd->index] = devfs_register(devfs_dir_handle, name,
			DEVFS_FL_DEFAULT, MTD_CHAR_MAJOR, mtd->index*2+1,
			S_IFCHR | S_IRUGO | S_IWUGO,
			&mtd_fops, NULL);
}

static void mtd_notify_remove(struct mtd_info* mtd)
{
	if (!mtd)
		return;

	devfs_unregister(devfs_rw_handle[mtd->index]);
	devfs_unregister(devfs_ro_handle[mtd->index]);
}
#endif

#if LINUX_VERSION_CODE < 0x20212 && defined(MODULE)
#define init_mtdchar init_module
#define cleanup_mtdchar cleanup_module
#endif

mod_init_t init_mtdchar(void)
{
#ifdef CONFIG_DEVFS_FS
	if (devfs_register_chrdev(MTD_CHAR_MAJOR, "mtd", &mtd_fops))
	{
		printk(KERN_NOTICE "Can't allocate major number %d for Memory Technology Devices.\n",
		       MTD_CHAR_MAJOR);
		return -EAGAIN;
	}

	devfs_dir_handle = devfs_mk_dir(NULL, "mtd", NULL);

	register_mtd_user(&notifier);
#else
	if (register_chrdev(MTD_CHAR_MAJOR, "mtd", &mtd_fops))
	{
		printk(KERN_NOTICE "Can't allocate major number %d for Memory Technology Devices.\n",
		       MTD_CHAR_MAJOR);
		return -EAGAIN;
	}
#endif

	return 0;
}

mod_exit_t cleanup_mtdchar(void)
{
#ifdef CONFIG_DEVFS_FS
	unregister_mtd_user(&notifier);
	devfs_unregister(devfs_dir_handle);
	devfs_unregister_chrdev(MTD_CHAR_MAJOR, "mtd");
#else
	unregister_chrdev(MTD_CHAR_MAJOR, "mtd");
#endif
}

module_init(init_mtdchar);
module_exit(cleanup_mtdchar);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("Direct character-device access to MTD devices");
