/*
 * GPIO char driver
 *
 * Copyright 2004, CyberTAN Corporation
 * All Rights Reserved.
 *
 * GPL License
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <typedefs.h>

#include <asm/avalanche/avalanche_map.h>

#define KSEG1_BASE              0xa0000000

#ifndef KSEG_INV_MASK
#define KSEG_INV_MASK               0x1FFFFFFF /* Inverted mask for kseg address */
#endif

#ifndef PHYS_ADDR
#define PHYS_ADDR(addr)             ((addr) & KSEG_INV_MASK)
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(addr)            (PHYS_ADDR(addr)|KSEG1_BASE)
#endif

#ifndef REG32_ADDR
#define REG32_ADDR(addr)            (volatile unsigned int *)(PHYS_TO_K1(addr))
#define REG32_DATA(addr)            (*(volatile unsigned int *)(PHYS_TO_K1(addr)))
#define REG32_WRITE(addr, data)     REG32_DATA(addr) = data;
#define REG32_READ(addr, data)      data = (unsigned int) REG32_DATA(addr);
#endif

static int gpio_major;
static devfs_handle_t gpio_dir;
static struct {
	char *name;
	devfs_handle_t handle;
} gpio_file[] = {
	{ "in", NULL },
	{ "out", NULL },
	{ "enable", NULL },
	{ "dir", NULL }
};

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
static int
gpio_open(struct inode *inode, struct file * file)
{
	if (MINOR(inode->i_rdev) > ARRAYSIZE(gpio_file))
		return -ENODEV;

	MOD_INC_USE_COUNT;
	return 0;
}

static int
gpio_release(struct inode *inode, struct file * file)
{
	MOD_DEC_USE_COUNT;
	return 0;
}


static ssize_t
gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	UINT32 pin_value;

	switch (MINOR(file->f_dentry->d_inode->i_rdev)) {
	case 0:
		REG32_READ(AVALANCHE_GPIO_DATA_IN, pin_value);
		break;
	case 1:
		REG32_READ(AVALANCHE_GPIO_DATA_OUT, pin_value);
		break;
	case 2:
		REG32_READ(AVALANCHE_GPIO_ENBL, pin_value);
		break;
	case 3:
		REG32_READ(AVALANCHE_GPIO_DIR, pin_value);
		break;
	default:
		return -ENODEV;
	}

	if (put_user(pin_value, (u32 *) buf))
		return -EFAULT;

	return sizeof(pin_value);
}

static ssize_t
gpio_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	UINT32 pin_value;

	if (get_user(pin_value, (u32 *) buf))
		return -EFAULT;

	switch (MINOR(file->f_dentry->d_inode->i_rdev)) {
	case 0:
		return -EACCES;
	case 1:
		REG32_WRITE(AVALANCHE_GPIO_DATA_OUT, pin_value);
		break;
	case 2:
		REG32_WRITE(AVALANCHE_GPIO_ENBL, pin_value);
		break;
	case 3:
		REG32_WRITE(AVALANCHE_GPIO_DIR, pin_value);
		break;
	default:
		return -ENODEV;
	}

	return sizeof(pin_value);
}

static struct file_operations gpio_fops = {
	owner:		THIS_MODULE,
	open:		gpio_open,
	release:	gpio_release,
	read:		gpio_read,
	write:		gpio_write,
};

static int __init
gpio_init(void)
{
	int i;

	if ((gpio_major = devfs_register_chrdev(0, "gpio", &gpio_fops)) < 0)
		return gpio_major;

	gpio_dir = devfs_mk_dir(NULL, "gpio", NULL);

	for (i = 0; i < ARRAYSIZE(gpio_file); i++) {
		gpio_file[i].handle = devfs_register(gpio_dir,
						     gpio_file[i].name,
						     DEVFS_FL_DEFAULT, gpio_major, i,
						     S_IFCHR | S_IRUGO | S_IWUGO,
						     &gpio_fops, NULL);
	}

	return 0;
}

static void __exit
gpio_exit(void)
{
	int i;

	for (i = 0; i < ARRAYSIZE(gpio_file); i++)
		devfs_unregister(gpio_file[i].handle);
	devfs_unregister(gpio_dir);
	devfs_unregister_chrdev(gpio_major, "gpio");
}

module_init(gpio_init);
module_exit(gpio_exit);
