/*
 * $Id: avalanche-flash.c,v 1.1.1.2 2005/03/28 06:56:09 sparq Exp $
 *
 * Normal mappings of chips in physical memory
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/config.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/cfi.h>
#include <linux/reboot.h>

#define WINDOW_ADDR CONFIG_MTD_AVALANCHE_START
#define WINDOW_SIZE CONFIG_MTD_AVALANCHE_LEN
#define BUSWIDTH CONFIG_MTD_AVALANCHE_BUSWIDTH

#include <asm/mips-boards/prom.h>
extern char *prom_getenv(char *name);

static int create_mtd_partitions(void);
static void __exit avalanche_mtd_cleanup(void);
	
/* avalanche__partition_info gives details on the logical partitions that splits
 * the flash device into. If the size if zero we use up to the end of
 * the device. */
#define MAX_NUM_PARTITIONS 5
static struct mtd_partition avalanche_partition_info[MAX_NUM_PARTITIONS];
static int num_of_partitions = 0;

static struct mtd_info *avalanche_mtd_info;

int avalanche_mtd_ready=0;

__u8 avalanche_read8(struct map_info *map, unsigned long ofs)
{
	return __raw_readb(map->map_priv_1 + ofs);
}

__u16 avalanche_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 avalanche_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void avalanche_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	memcpy_fromio(to, map->map_priv_1 + from, len);
}

void avalanche_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void avalanche_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
}

void avalanche_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
}

void avalanche_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy_toio(map->map_priv_1 + to, from, len);
}

struct map_info avalanche_map = {
	name: "Physically mapped flash",
	size: WINDOW_SIZE,
	buswidth: BUSWIDTH,
	read8: avalanche_read8,
	read16: avalanche_read16,
	read32: avalanche_read32,
	copy_from: avalanche_copy_from,
	write8: avalanche_write8,
	write16: avalanche_write16,
	write32: avalanche_write32,
	copy_to: avalanche_copy_to
};

#ifdef CONFIG_MTD_CFI_INTELEXT
static int avalanche_reboot_notifier(struct notifier_block *nb, unsigned long val, void *v)
{
	struct cfi_private *cfi = avalanche_map.fldrv_priv;
	unsigned long b;

	cfi_send_gen_cmd(0xff, 0x55, 0, &avalanche_map, cfi,
		 cfi->device_type, NULL);
	 return(NOTIFY_OK);
}

static struct notifier_block avalanche_notifier_block = {
      avalanche_reboot_notifier, NULL, 0
};
#endif
		

#include <code_pattern.h>
#ifdef CONFIG_MTD_CFI_INTELEXT
//junzhao 2005.2.16 check flash type
extern int flash_type;
#endif

int __init avalanche_mtd_init(void)
{
	int i;
       	printk(KERN_NOTICE "avalanche flash device: 0x%lx at 0x%lx.\n", (unsigned long)WINDOW_SIZE, (unsigned long)WINDOW_ADDR);
	avalanche_map.map_priv_1 = (unsigned long)ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);

	if (!avalanche_map.map_priv_1) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
	
	avalanche_mtd_info = do_map_probe("cfi_probe", &avalanche_map);
	if (!avalanche_mtd_info)
	{
		avalanche_mtd_cleanup();
		return -ENXIO;
	}
	
	avalanche_mtd_info->module = THIS_MODULE;

	if (!create_mtd_partitions())
		add_mtd_device(avalanche_mtd_info);
	else{	
		add_mtd_partitions(avalanche_mtd_info, avalanche_partition_info, num_of_partitions);
//cjg
#ifdef CONFIG_MTD_CFI_INTELEXT
	//junzhao 2005.2.16
	if(flash_type == INTELC3)
	{
		for(i = 0;; i ++){
			struct mtd_info *info;
			info = get_mtd_device(0, i);
			if(!info)
				break;
			if((info->flags & MTD_WRITEABLE) &&
					(info->unlock)){
				int ret;
				printk(KERN_DEBUG "unlocking mtd device %s\n", info->name);
				ret = info->unlock(info, 0, info->size);
				if(ret)
					printk(KERN_ERR "failed (%d) to unlock mtd device %s\n", ret, info->name);
			}
			
			//junzhao add for restore flash status
			{
			size_t len;
			unsigned char buf[1] = "\0";
			MTD_READ(info, 0, 1, &len, buf);
			if(len == 1)
				printk("%s.........%02x\n", info->name, buf[0]);
			}

			put_mtd_device(info);
		}	
	}
#endif
	}
	avalanche_mtd_ready=1;
#ifdef CONFIG_MTD_CFI_INTELEXT
	register_reboot_notifier(&avalanche_notifier_block);
#endif
	
	
	return 0;
}

static char *strdup(char *str)
{
	int n = strlen(str)+1;
	char *s = kmalloc(n, GFP_KERNEL);
	if (!s) return NULL;
	return strcpy(s, str);
}


static int create_mtd_partitions(void)
{
	unsigned int offset;
	unsigned int size;
	unsigned int found;
	unsigned char *flash_base;
	unsigned char *flash_end;
	char *env_ptr;
	char *base_ptr;
	char *end_ptr;

	do {
		char	env_name[20];

		found = 0;

		/* get base and end addresses of flash file system from environment */
		sprintf(env_name, "mtd%1u", num_of_partitions);
printk("Looking for mtd device :%s:\n", env_name);
		env_ptr = prom_getenv(env_name);
		if(env_ptr == NULL) {
			/* No more partitions to find */
			break;
		}

		/* Extract the start and stop addresses of the partition */
		base_ptr = strtok(env_ptr, ",");
		end_ptr = strtok(NULL, ",");
		if ((base_ptr == NULL) || (end_ptr == NULL)) {	
			printk("JFFS2 ERROR: Invalid %s start,end.\n", env_name);
			break;
		}

		flash_base = (unsigned char*) simple_strtol(base_ptr, NULL, 0);
		flash_end = (unsigned char*) simple_strtol(end_ptr, NULL, 0);
		if((!flash_base) || (!flash_end)) {
			printk("JFFS2 ERROR: Invalid %s start,end.\n", env_name);
			break;
		}

		offset = virt_to_bus(flash_base) - WINDOW_ADDR;
		size = flash_end - flash_base;
		printk("Found a %s image (0x%x), with size (0x%x).\n",env_name, offset, size);

		/* Setup the partition info. We duplicate the env_name for the partiton name */
		avalanche_partition_info[num_of_partitions].name = strdup(env_name);
		avalanche_partition_info[num_of_partitions].offset = offset;
		avalanche_partition_info[num_of_partitions].size = size;
		avalanche_partition_info[num_of_partitions].mask_flags = 0;
		num_of_partitions++;
	} while (num_of_partitions < MAX_NUM_PARTITIONS);

	return num_of_partitions;
}

static void __exit avalanche_mtd_cleanup(void)
{
	avalanche_mtd_ready=0;
#ifdef CONFIG_MTD_CFI_INTELEXT
	unregister_reboot_notifier(&avalanche_notifier_block);
#endif
	
	if (avalanche_mtd_info) {
		del_mtd_partitions(avalanche_mtd_info);
		del_mtd_device(avalanche_mtd_info);
		map_destroy(avalanche_mtd_info);
	}

	if (avalanche_map.map_priv_1) {
		iounmap((void *)avalanche_map.map_priv_1);
		avalanche_map.map_priv_1 = 0;
	}
}

module_init(avalanche_mtd_init);
module_exit(avalanche_mtd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Snehaprabha Narnakaje");
MODULE_DESCRIPTION("Avalanche CFI map driver");
