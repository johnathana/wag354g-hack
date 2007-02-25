/*
 * Flash memory access on EPXA based devices
 *
 * (C) 2000 Nicolas Pitre <nico@cam.org>
 *  Copyright (C) 2001 Altera Corporation
 *
 * $Id: epxa10db-flash.c,v 1.1.1.2 2005/03/28 06:56:09 sparq Exp $ 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/hardware.h>


MODULE_AUTHOR("Clive Davies");
MODULE_DESCRIPTION("Altera epxa10db mtd flash map");
MODULE_LICENSE("GPL");

static __u8 epxa10db_read8(struct map_info *map, unsigned long ofs)
{
	return *(__u8 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + ofs));
}

static __u16 epxa10db_read16(struct map_info *map, unsigned long ofs)
{
	return *(__u16 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + ofs));
}

static __u32 epxa10db_read32(struct map_info *map, unsigned long ofs)
{
	return *(__u32 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + ofs));
}

static void epxa10db_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	memcpy(to, (void *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + from)), len);
}

static void epxa10db_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	*(__u8 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + adr)) = d;
}

static void epxa10db_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	*(__u16 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + adr)) = d;
}

static void epxa10db_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	*(__u32 *)(FLASH_VADDR(EXC_EBI_BLOCK0_BASE + adr)) = d;
}

static void epxa10db_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy((void *)FLASH_VADDR(EXC_EBI_BLOCK0_BASE + to), from, len);
}



static struct map_info epxa10db_map = {
	name:		"EPXA10DB flash",
	read8:		epxa10db_read8,
	read16:		epxa10db_read16,
	read32:		epxa10db_read32,
	copy_from:	epxa10db_copy_from,
	write8:		epxa10db_write8,
	write16:	epxa10db_write16,
	write32:	epxa10db_write32,
	copy_to:	epxa10db_copy_to
};





static int fixup_epxa10db_partitions(struct mtd_info *master, struct mtd_partition **pparts);

static struct mtd_partition *parsed_parts;
static struct mtd_info *mymtd;

static int __init epxa10db_mtd_init(void)
{
	struct mtd_partition *parts=NULL;
	int nb_parts = 0;
	int parsed_nr_parts = 0;
	char *part_type="";
	int i;
	char* master_name="master";
	
	epxa10db_map.buswidth =  2;
	epxa10db_map.size = 0x01000000;
	printk(KERN_NOTICE "EPXA10DB flash: probing %d-bit flash bus\n", epxa10db_map.buswidth*8);
	mymtd = do_map_probe("cfi_probe", &epxa10db_map);
	if (!mymtd)
		return -ENXIO;
	mymtd->module = THIS_MODULE;



	/*
	 * Dynamic partition selection stuff (might override the static ones)
	 */
	if (parsed_nr_parts == 0) {
		int ret = fixup_epxa10db_partitions(mymtd, &parsed_parts);
		if (ret > 0) {
			part_type = "Epxa10db fixup";
			parsed_nr_parts = ret;
		}
	}

	if (parsed_nr_parts > 0) {
		parts = parsed_parts;
		nb_parts = parsed_nr_parts;
	}

	if (nb_parts == 0) {
		printk(KERN_NOTICE "EPXA10DB flash: no partition info available, registering whole flash at once\n");
		add_mtd_device(mymtd);
	} else {
		printk(KERN_NOTICE "Using %s partition definition\n", part_type);
		add_mtd_partitions(mymtd, parts, nb_parts);
	}
	
        /* unlock the flash device */

	for (i=0; i<mymtd->numeraseregions;i++){
		int j;
		for(j=0;j<mymtd->eraseregions[i].numblocks;j++){
			mymtd->unlock(mymtd,mymtd->eraseregions[i].offset + j * mymtd->eraseregions[i].erasesize,4);
		}
	}

	/* 
	 * Register mymtd as an mtd device so that we can read the 
	 * protection data 
	 */
	mymtd->name=master_name;
	add_mtd_device(mymtd);


	return 0;
}

static void __exit epxa10db_mtd_cleanup(void)
{
	if (mymtd) {
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);
		if (parsed_parts)
			kfree(parsed_parts);
	}
}


/* 
 * This will do for now, once we decide which bootldr we're finally 
 * going to use then we'll remove this function and do it properly
 *
 * Partions are currently (as offsets from base of flash):
 * 0x00000000 - 0x003FFFFF - bootloader (!)
 * 0x00400000 - 0x00FFFFFF - Flashdisk
 */

static int fixup_epxa10db_partitions(struct mtd_info *master, struct mtd_partition **pparts)
{
	struct mtd_partition *parts;
	int ret, i;
	int npartitions = 0;
	char *names; 
	const char *name = "jffs";

	printk("Fixing up epxa10db partitions\n");
	npartitions=1;
	parts = kmalloc(npartitions*sizeof(*parts)+strlen(name), GFP_KERNEL);
	if (!parts) {
		ret = -ENOMEM;
		goto out;
	}
	i=0;
	names = (char *)&parts[npartitions];	
	parts[i].name = names;
	names += strlen(name) + 1;
	strcpy(parts[i].name, name);

	parts[i].size = 0x00C00000;
	parts[i].offset = 0x00400000;
	parts[i].mask_flags = 0;
		

 out:
	*pparts = parts;
	return npartitions;
}

module_init(epxa10db_mtd_init);
module_exit(epxa10db_mtd_cleanup);
