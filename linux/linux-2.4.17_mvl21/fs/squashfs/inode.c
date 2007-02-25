/*
 * Squashfs - a compressed read only filesystem for Linux
 *
 * Copyright (c) 2002 Phillip Lougher <phillip@lougher.demon.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * inode.c
 */
#define __TI_SQUASHFS_LZMA
//#define SQUASHFS_TRACE

#include <linux/types.h>
#include <linux/squashfs_fs.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/locks.h>
#include <linux/init.h>
#include <linux/dcache.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <asm/semaphore.h>

#ifdef __TI_SQUASHFS_LZMA
#include "LzmaWrapper.h"
#else
#include <linux/zlib_fs.h>
#endif

#include <linux/blkdev.h>
#include <linux/vmalloc.h>

#ifdef SQUASHFS_TRACE
#define TRACE(s, args...)				printk(KERN_NOTICE "SQUASHFS: "s, ## args)
#else
#define TRACE(s, args...)				{}
#endif

#define ERROR(s, args...)				printk(KERN_ERR "SQUASHFS error: "s, ## args)

#define SERROR(s, args...)				if(!silent) printk(KERN_ERR "SQUASHFS error: "s, ## args)
#define WARNING(s, args...)				printk(KERN_WARNING "SQUASHFS: "s, ## args)

static struct super_block *squashfs_read_super(struct super_block *, void *, int);
static void squashfs_put_super(struct super_block *);
static int squashfs_statfs(struct super_block *, struct statfs *);
static int squashfs_symlink_readpage(struct file *file, struct page *page);
static int squashfs_readpage(struct file *file, struct page *page);
static int squashfs_readdir(struct file *, void *, filldir_t);
static struct dentry *squashfs_lookup(struct inode *, struct dentry *);
static unsigned int read_data(struct super_block *s, char *buffer,
		unsigned int index, int length, unsigned int *next_index);
static int squashfs_get_cached_block(struct super_block *s, char *buffer,
		unsigned int block, unsigned int offset, int length,
		unsigned int *next_block, unsigned int *next_offset);
static struct inode *squashfs_iget(struct super_block *s, squashfs_inode inode);
static void squashfs_put_super(struct super_block *s);

DECLARE_MUTEX(read_data_mutex);

#ifdef __TI_SQUASHFS_LZMA
static char	*lzma_data;
#else
static z_stream stream;
#endif

static DECLARE_FSTYPE_DEV(squashfs_fs_type, "squashfs", squashfs_read_super);

static unsigned char squashfs_filetype_table[] = {
	DT_UNKNOWN, DT_DIR, DT_REG, DT_LNK, DT_BLK, DT_CHR
};

static struct super_operations squashfs_ops = {
	statfs: squashfs_statfs,
	put_super: squashfs_put_super,
};

static struct address_space_operations squashfs_symlink_aops = {
	readpage: squashfs_symlink_readpage
};

static struct address_space_operations squashfs_aops = {
	readpage: squashfs_readpage
};

static struct file_operations squashfs_dir_ops = {
	read: generic_read_dir,
	readdir: squashfs_readdir
};

static struct inode_operations squashfs_dir_inode_ops = {
	lookup: squashfs_lookup
};


static unsigned int read_data(struct super_block *s, char *buffer,
		unsigned int index, int length, unsigned int *next_index)
{
	squashfs_sb_info *msBlk = &s->u.squashfs_sb;
	struct buffer_head *bh[((SQUASHFS_FILE_MAX_SIZE - 1) >> msBlk->devblksize_log2) + 2];
	unsigned short c_byte;
	unsigned int offset = index & ((1 << msBlk->devblksize_log2) - 1);
	unsigned int cur_index = index >> msBlk->devblksize_log2;
	int bytes, avail_bytes, b, k;
	char *c_buffer;
	unsigned int compressed;

	/* if(!(bh[0] = sb_bread(s, cur_index))) */
	if(!(bh[0] = bread(s->s_dev, cur_index, s->s_blocksize)))
		goto read_failure;

	if(length)
		c_byte = length;
	else {
		if(msBlk->devblksize - offset == 1) {
			if(msBlk->swap)
				((unsigned char *) &c_byte)[1] = *((unsigned char *) (bh[0]->b_data + offset));
			else
				((unsigned char *) &c_byte)[0] = *((unsigned char *) (bh[0]->b_data + offset));
			brelse(bh[0]);
			/* if(!(bh[0] = sb_bread(s, ++cur_index))) */
			if(!(bh[0] = bread(s->s_dev, ++cur_index, s->s_blocksize)))
				goto read_failure;
			if(msBlk->swap)
				((unsigned char *) &c_byte)[0] = *((unsigned char *) bh[0]->b_data); 
			else
				((unsigned char *) &c_byte)[1] = *((unsigned char *) bh[0]->b_data); 
			offset = 1;
		}
		else {
			if(msBlk->swap) {
				((unsigned char *) &c_byte)[1] = *((unsigned char *) (bh[0]->b_data + offset));
				((unsigned char *) &c_byte)[0] = *((unsigned char *) (bh[0]->b_data + offset + 1)); 
			} else
				c_byte = *((unsigned short *) (bh[0]->b_data + offset));
			offset += 2;
		}
		if(SQUASHFS_CHECK_DATA(msBlk->sBlk.flags)) {
			if(offset == msBlk->devblksize) {
				brelse(bh[0]);
				/* if(!(bh[0] = sb_bread(s, ++cur_index))) */
				if(!(bh[0] = bread(s->s_dev, ++cur_index, s->s_blocksize)))
					goto read_failure;
				offset = 0;
			}
			if(*((unsigned char *) (bh[0]->b_data + offset)) != SQUASHFS_MARKER_BYTE) {
				ERROR("Metadata block marker corrupt @ %x\n", index);
				brelse(bh[0]);
				return 0;
			}
			offset ++;
		}
	}

	bytes = msBlk->devblksize - offset;
	c_buffer = (compressed = SQUASHFS_COMPRESSED(c_byte)) ? msBlk->read_data : buffer;
	c_byte = SQUASHFS_COMPRESSED_SIZE(c_byte);

	TRACE("Block @ 0x%x, %scompressed size %d\n", index, compressed ? "" : "un", (unsigned int) c_byte);

	for(b = 1; bytes < c_byte; b++) {
		/* if(!(bh[b] = sb_bread(s, ++cur_index))) */
		if(!(bh[b] = bread(s->s_dev, ++cur_index, s->s_blocksize)))
					
			goto block_release;
		bytes += msBlk->devblksize;
	}

	if(compressed)
		down(&read_data_mutex);

	for(bytes = 0, k = 0; k < b; k++) {
		avail_bytes = (c_byte - bytes) > (msBlk->devblksize - offset) ? msBlk->devblksize - offset : c_byte - bytes;
		memcpy(c_buffer + bytes, bh[k]->b_data + offset, avail_bytes);
		bytes += avail_bytes;
		offset = 0;
		brelse(bh[k]);
	}

	/*
	 * uncompress block
	 */
	if(compressed) {
#ifdef __TI_SQUASHFS_LZMA
		int out_size;
		int lzma_err;
		int i;
		out_size=msBlk->read_size;

//		printk("start of c_buffer: ");
//		for(i=0;i<16;i++)
//			printk("%2.2x ", c_buffer[i]);
//		printk("\n");

		if((lzma_err=lzma_inflate(c_buffer, c_byte, buffer, &out_size))){
			ERROR("lzma returned unexpected result 0x%x\n", lzma_err);
			bytes = 0;
		} else
			bytes = out_size;
//		printk("out size = %d, processed = %d\n", msBlk->read_size, out_size);
#else
		int zlib_err;

		stream.next_in = c_buffer;
		stream.avail_in = c_byte;
		stream.next_out = buffer;
		stream.avail_out = msBlk->read_size;
		if(((zlib_err = zlib_fs_inflateInit(&stream)) != Z_OK) ||
				((zlib_err = zlib_fs_inflate(&stream, Z_FINISH)) != Z_STREAM_END) ||
				((zlib_err = zlib_fs_inflateEnd(&stream)) != Z_OK)) {
			ERROR("zlib_fs returned unexpected result 0x%x\n", zlib_err);
			bytes = 0;
		} else
			bytes = stream.total_out;
#endif
		up(&read_data_mutex);
	}

	if(next_index)
		*next_index = index + c_byte + (length ? 0 : (SQUASHFS_CHECK_DATA(msBlk->sBlk.flags) ? 3 : 2));

	return bytes;

block_release:
	while(--b >= 0) brelse(bh[b]);

read_failure:
	ERROR("sb_bread failed reading block 0x%x\n", cur_index);
	return 0;
}


static int squashfs_get_cached_block(struct super_block *s, char *buffer,
		unsigned int block, unsigned int offset, int length,
		unsigned int *next_block, unsigned int *next_offset)
{
	squashfs_sb_info *msBlk = &s->u.squashfs_sb;
	int n, i, bytes, return_length = length;
	unsigned int next_index;

	TRACE("Entered squashfs_get_cached_block [%x:%x]\n", block, offset);

	for(;;) {
		for(i = 0; i < SQUASHFS_CACHED_BLKS; i++) 
			if(msBlk->block_cache[i].block == block)
				break; 
		
		down(&msBlk->block_cache_mutex);
		if(i == SQUASHFS_CACHED_BLKS) {
			/* read inode header block */
			for(i = msBlk->next_cache, n = SQUASHFS_CACHED_BLKS; n ; n --, i = (i + 1) % SQUASHFS_CACHED_BLKS)
				if(msBlk->block_cache[i].block != SQUASHFS_USED_BLK)
					break;
			if(n == 0) {
				up(&msBlk->block_cache_mutex);
				sleep_on(&msBlk->waitq);
				continue;
			}
			msBlk->next_cache = (i + 1) % SQUASHFS_CACHED_BLKS;

			if(msBlk->block_cache[i].block == SQUASHFS_INVALID_BLK) {
				if(!(msBlk->block_cache[i].data = (unsigned char *)
							kmalloc(SQUASHFS_METADATA_SIZE, GFP_KERNEL))) {
					ERROR("Failed to allocate cache block\n");
					up(&msBlk->block_cache_mutex);
					return 0;
				}
			}
	
			msBlk->block_cache[i].block = SQUASHFS_USED_BLK;
			up(&msBlk->block_cache_mutex);
			if(!(msBlk->block_cache[i].length = read_data(s, msBlk->block_cache[i].data, block, 0,
							&next_index))) {
				ERROR("Unable to read cache block [%x:%x]\n", block, offset);
				return 0;
			}
			down(&msBlk->block_cache_mutex);
			wake_up(&msBlk->waitq);
			msBlk->block_cache[i].block = block;
			msBlk->block_cache[i].next_index = next_index;
			TRACE("Read cache block [%x:%x]\n", block, offset);
		}

		if(msBlk->block_cache[i].block != block) {
			up(&msBlk->block_cache_mutex);
			continue;
		}

		if((bytes = msBlk->block_cache[i].length - offset) >= length) {
			memcpy(buffer, msBlk->block_cache[i].data + offset, length);
			if(msBlk->block_cache[i].length - offset == length) {
				*next_block = msBlk->block_cache[i].next_index;
				*next_offset = 0;
			} else {
				*next_block = block;
				*next_offset = offset + length;
			}
	
			up(&msBlk->block_cache_mutex);
			return return_length;
		} else {
			memcpy(buffer, msBlk->block_cache[i].data + offset, bytes);
			block = msBlk->block_cache[i].next_index;
			up(&msBlk->block_cache_mutex);
			length -= bytes;
			offset = 0;
			buffer += bytes;
		}
	}
}


static struct inode *squashfs_iget(struct super_block *s, squashfs_inode inode)
{
	struct inode *i = new_inode(s);
	squashfs_sb_info *msBlk = &s->u.squashfs_sb;
	squashfs_super_block *sBlk = &msBlk->sBlk;
	unsigned int block = SQUASHFS_INODE_BLK(inode) + sBlk->inode_table_start;
	unsigned int offset = SQUASHFS_INODE_OFFSET(inode);
	unsigned int next_block, next_offset;
	squashfs_base_inode_header inodeb;

	TRACE("Entered squashfs_iget\n");

	if(msBlk->swap) {
		squashfs_base_inode_header sinodeb;

		if(!squashfs_get_cached_block(s, (char *) &sinodeb, block,  offset,
					sizeof(sinodeb), &next_block, &next_offset))
			goto failed_read;
		SQUASHFS_SWAP_BASE_INODE_HEADER(&inodeb, &sinodeb, sizeof(sinodeb));
	} else
		if(!squashfs_get_cached_block(s, (char *) &inodeb, block,  offset,
					sizeof(inodeb), &next_block, &next_offset))
			goto failed_read;

	i->i_nlink = 1;

	i->i_mtime = sBlk->mkfs_time;
	i->i_atime = sBlk->mkfs_time;
	i->i_ctime = sBlk->mkfs_time;
	i->i_uid = msBlk->uid[((inodeb.inode_type - 1) / SQUASHFS_TYPES) * 16 + inodeb.uid];
	i->i_ino = SQUASHFS_MK_VFS_INODE(block - sBlk->inode_table_start, offset);

	if(inodeb.guid == SQUASHFS_GUIDS)
		i->i_gid = i->i_uid;
	else
		i->i_gid = msBlk->guid[inodeb.guid];

	i->i_mode = inodeb.mode;

	switch((inodeb.inode_type - 1) % SQUASHFS_TYPES + 1) {
		case SQUASHFS_FILE_TYPE: {
			squashfs_reg_inode_header inodep;

			if(msBlk->swap) {
				squashfs_reg_inode_header sinodep;

				if(!squashfs_get_cached_block(s, (char *) &sinodep, block,  offset, sizeof(sinodep),
							&next_block, &next_offset))
					goto failed_read;
				SQUASHFS_SWAP_REG_INODE_HEADER(&inodep, &sinodep);
			} else
				if(!squashfs_get_cached_block(s, (char *) &inodep, block,  offset, sizeof(inodep),
							&next_block, &next_offset))
					goto failed_read;

			i->i_size = inodep.file_size;
			i->i_fop = &generic_ro_fops;
			i->i_data.a_ops = &squashfs_aops;
			i->i_mode |= S_IFREG;
			i->i_mtime = inodep.mtime;
			i->i_atime = inodep.mtime;
			i->i_ctime = inodep.mtime;
			i->u.squashfs_i.start_block = inodep.start_block;
			i->u.squashfs_i.block_list_start = next_block;
			i->u.squashfs_i.offset = next_offset;
			TRACE("File inode %x:%x, start_block %x, block_list_start %x, offset %x\n",
					SQUASHFS_INODE_BLK(inode), offset, inodep.start_block, next_block, next_offset);
			break;
		}
		case SQUASHFS_DIR_TYPE: {
			squashfs_dir_inode_header inodep;

			if(msBlk->swap) {
				squashfs_dir_inode_header sinodep;

				if(!squashfs_get_cached_block(s, (char *) &sinodep, block,  offset, sizeof(sinodep),
							&next_block, &next_offset))
					goto failed_read;
				SQUASHFS_SWAP_DIR_INODE_HEADER(&inodep, &sinodep);
			} else
				if(!squashfs_get_cached_block(s, (char *) &inodep, block,  offset, sizeof(inodep),
							&next_block, &next_offset))
					goto failed_read;

			i->i_size = inodep.file_size;
			i->i_op = &squashfs_dir_inode_ops;
			i->i_fop = &squashfs_dir_ops;
			i->i_mode |= S_IFDIR;
			i->i_mtime = inodep.mtime;
			i->i_atime = inodep.mtime;
			i->i_ctime = inodep.mtime;
			i->u.squashfs_i.start_block = inodep.start_block;
			i->u.squashfs_i.offset = inodep.offset;
			TRACE("Directory inode %x:%x, start_block %x, offset %x\n", SQUASHFS_INODE_BLK(inode), offset,
					inodep.start_block, inodep.offset);
			break;
		}
		case SQUASHFS_SYMLINK_TYPE: {
			squashfs_symlink_inode_header inodep;
	
			if(msBlk->swap) {
				squashfs_symlink_inode_header sinodep;

				if(!squashfs_get_cached_block(s, (char *) &sinodep, block,  offset, sizeof(sinodep),
							&next_block, &next_offset))
					goto failed_read;
				SQUASHFS_SWAP_SYMLINK_INODE_HEADER(&inodep, &sinodep);
			} else
				if(!squashfs_get_cached_block(s, (char *) &inodep, block,  offset, sizeof(inodep),
							&next_block, &next_offset))
					goto failed_read;

			i->i_size = inodep.symlink_size;
			i->i_op = &page_symlink_inode_operations;
			i->i_data.a_ops = &squashfs_symlink_aops;
			i->i_mode |= S_IFLNK;
			i->u.squashfs_i.start_block = next_block;
			i->u.squashfs_i.offset = next_offset;
			TRACE("Symbolic link inode %x:%x, start_block %x, offset %x\n",
				SQUASHFS_INODE_BLK(inode), offset, next_block, next_offset);
			break;
		 }
		 case SQUASHFS_BLKDEV_TYPE:
		 case SQUASHFS_CHRDEV_TYPE: {
			squashfs_dev_inode_header inodep;

			if(msBlk->swap) {
				squashfs_dev_inode_header sinodep;

				if(!squashfs_get_cached_block(s, (char *) &sinodep, block,  offset, sizeof(sinodep),
							&next_block, &next_offset))
					goto failed_read;
				SQUASHFS_SWAP_DEV_INODE_HEADER(&inodep, &sinodep);
			} else	
				if(!squashfs_get_cached_block(s, (char *) &inodep, block,  offset, sizeof(inodep),
							&next_block, &next_offset))
					goto failed_read;

			i->i_size = 0;
			i->i_mode |= (inodeb.inode_type == SQUASHFS_CHRDEV_TYPE) ? S_IFCHR : S_IFBLK;
			init_special_inode(i, i->i_mode, inodep.rdev);
			TRACE("Device inode %x:%x, rdev %x\n", SQUASHFS_INODE_BLK(inode), offset, inodep.rdev);
			break;
		 }
		 default:
			ERROR("Unknown inode type %d in squashfs_iget!\n", inodeb.inode_type);
				goto failed_read1;
	}
	
	return i;

failed_read:
	ERROR("Unable to read inode [%x:%x]\n", block, offset);

failed_read1:
	return NULL;
}


static struct super_block *squashfs_read_super(struct super_block *s,
		void *data, int silent)
{
	kdev_t dev = s->s_dev;
	squashfs_sb_info *msBlk = &s->u.squashfs_sb;
	squashfs_super_block *sBlk = &msBlk->sBlk;
	int i;

	TRACE("Entered squashfs_read_superblock\n");

	msBlk->devblksize = get_hardsect_size(dev);
	if(msBlk->devblksize < BLOCK_SIZE)
		msBlk->devblksize = BLOCK_SIZE;
	msBlk->devblksize_log2 = ffz(~msBlk->devblksize);
	set_blocksize(dev, msBlk->devblksize);
	s->s_blocksize = msBlk->devblksize;
	s->s_blocksize_bits = msBlk->devblksize_log2;

	init_MUTEX(&msBlk->read_page_mutex);
	init_MUTEX(&msBlk->block_cache_mutex);
	
	init_waitqueue_head(&msBlk->waitq);

	if(!read_data(s, (char *) sBlk, SQUASHFS_START, sizeof(squashfs_super_block) | SQUASHFS_COMPRESSED_BIT, NULL)) {
		SERROR("unable to read superblock\n");
		goto failed_mount;
	}

	/* Check it is a SQUASHFS superblock */
	msBlk->swap = 0;
	if((s->s_magic = sBlk->s_magic) != SQUASHFS_MAGIC) {
		if(sBlk->s_magic == SQUASHFS_MAGIC_SWAP) {
			squashfs_super_block sblk;
			WARNING("Mounting a different endian SQUASHFS filesystem on %s\n", bdevname(dev));
			SQUASHFS_SWAP_SUPER_BLOCK(&sblk, sBlk);
			memcpy(sBlk, &sblk, sizeof(squashfs_super_block));
			msBlk->swap = 1;
		} else  {
			SERROR("Can't find a SQUASHFS superblock on %s\n", bdevname(dev));
			goto failed_mount;
		}
	}

	/* Check the MAJOR & MINOR versions */
	if(sBlk->s_major != SQUASHFS_MAJOR || sBlk->s_minor > SQUASHFS_MINOR) {
		SERROR("Major/Minor mismatch, filesystem is (%d:%d), I support (%d: <= %d)\n",
				sBlk->s_major, sBlk->s_minor, SQUASHFS_MAJOR, SQUASHFS_MINOR);
		goto failed_mount;
	}

	TRACE("Found valid superblock on %s\n", bdevname(dev));
	TRACE("Inodes are %scompressed\n", SQUASHFS_UNCOMPRESSED_INODES(sBlk->flags) ? "un" : "");
	TRACE("Data is %scompressed\n", SQUASHFS_UNCOMPRESSED_DATA(sBlk->flags) ? "un" : "");
	TRACE("Check data is %s present in the filesystem\n", SQUASHFS_CHECK_DATA(sBlk->flags) ? "" : "not");
	TRACE("Filesystem size %d bytes\n", sBlk->bytes_used);
	TRACE("Block size %d\n", sBlk->block_size);
	TRACE("Number of inodes %d\n", sBlk->inodes);
	TRACE("Number of uids %d\n", sBlk->no_uids);
	TRACE("Number of gids %d\n", sBlk->no_guids);
	TRACE("sBlk->inode_table_start %x\n", sBlk->inode_table_start);
	TRACE("sBlk->directory_table_start %x\n", sBlk->directory_table_start);
	TRACE("sBlk->uid_start %x\n", sBlk->uid_start);

	s->s_flags |= MS_RDONLY;
	s->s_op = &squashfs_ops;

	/* Init inode_table block pointer array */
	if(!(msBlk->block_cache = (squashfs_cache *) kmalloc(sizeof(squashfs_cache) * SQUASHFS_CACHED_BLKS, GFP_KERNEL))) {
		ERROR("Failed to allocate block cache\n");
		goto failed_mount;
	}

	for(i = 0; i < SQUASHFS_CACHED_BLKS; i++)
		msBlk->block_cache[i].block = SQUASHFS_INVALID_BLK;

	msBlk->next_cache = 0;

	/* Allocate read_data block */
	msBlk->read_size = (sBlk->block_size < SQUASHFS_METADATA_SIZE) ? SQUASHFS_METADATA_SIZE : sBlk->block_size;
	if(!(msBlk->read_data = (char *) kmalloc(msBlk->read_size, GFP_KERNEL))) {
		ERROR("Failed to allocate read_data block\n");
		goto failed_mount1;
	}

	/* Allocate read_page block */
	if(sBlk->block_size > PAGE_CACHE_SIZE && 
	   !(msBlk->read_page = (char *) kmalloc(sBlk->block_size, GFP_KERNEL))) {
		ERROR("Failed to allocate read_page block\n");
		goto failed_mount2;
	}

	/* Allocate uid and gid tables */
	if(!(msBlk->uid = (squashfs_uid *) kmalloc((sBlk->no_uids +
		sBlk->no_guids) * sizeof(squashfs_uid), GFP_KERNEL))) {
		ERROR("Failed to allocate uid/gid table\n");
		goto failed_mount3;
	}
	msBlk->guid = msBlk->uid + sBlk->no_uids;
   
	if(msBlk->swap) {
		squashfs_uid suid[sBlk->no_uids + sBlk->no_guids];

		if(!read_data(s, (char *) &suid, sBlk->uid_start, ((sBlk->no_uids + sBlk->no_guids) *
				sizeof(squashfs_uid)) | SQUASHFS_COMPRESSED_BIT, NULL)) {
			SERROR("unable to read uid/gid table\n");
			goto failed_mount4;
		}
		SQUASHFS_SWAP_DATA(msBlk->uid, suid, (sBlk->no_uids + sBlk->no_guids), (sizeof(squashfs_uid) * 8));
	} else
		if(!read_data(s, (char *) msBlk->uid, sBlk->uid_start, ((sBlk->no_uids + sBlk->no_guids) *
				sizeof(squashfs_uid)) | SQUASHFS_COMPRESSED_BIT, NULL)) {
			SERROR("unable to read uid/gid table\n");
			goto failed_mount4;
		}

	if(!(s->s_root = d_alloc_root(squashfs_iget(s, sBlk->root_inode)))) {
		ERROR("Root inode create failed\n");
		goto failed_mount4;
	}

	TRACE("Leaving squashfs_read_super\n");
	return s;

failed_mount4:
	kfree(msBlk->uid);
failed_mount3:
	kfree(msBlk->read_page);
failed_mount2:
	kfree(msBlk->read_data);
failed_mount1:
	kfree(msBlk->block_cache);
failed_mount:
	return NULL;
}


static int squashfs_statfs(struct super_block *s, struct statfs *buf)
{
	squashfs_super_block *sBlk = &s->u.squashfs_sb.sBlk;

	TRACE("Entered squashfs_statfs\n");
	buf->f_type = SQUASHFS_MAGIC;
	buf->f_bsize = sBlk->block_size;
	buf->f_blocks = ((sBlk->bytes_used - 1) >> sBlk->block_log) + 1;
	buf->f_bfree = buf->f_bavail = 0;
	buf->f_files = sBlk->inodes;
	buf->f_ffree = 0;
	buf->f_namelen = SQUASHFS_NAME_LEN;
	return 0;
}


static int squashfs_symlink_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	int index = page->index << PAGE_CACHE_SHIFT, length = 0, bytes;
	int block = inode->u.squashfs_i.start_block;
	int offset = inode->u.squashfs_i.offset;

	TRACE("Entered squashfs_symlink_readpage, page index %d, start block %x, offset %x\n",
		page->index, inode->u.squashfs_i.start_block, inode->u.squashfs_i.offset);

	while(length < index) {
		char buffer[PAGE_CACHE_SIZE];

		if(length += bytes = squashfs_get_cached_block(inode->i_sb, buffer, block, offset,
					PAGE_CACHE_SIZE, &block, &offset), !bytes) {
			ERROR("Unable to read symbolic link [%x:%x]\n", block, offset);
			goto skip_read;
		}
	}

	if(length != index) {
		ERROR("(squashfs_symlink_readpage) length != index\n");
		return 0;
	}

	bytes = (inode->i_size - length) > PAGE_CACHE_SIZE ? PAGE_CACHE_SIZE : inode->i_size - length;
	if(!squashfs_get_cached_block(inode->i_sb, page_address(page), block, offset, bytes, &block, &offset))
		ERROR("Unable to read symbolic link [%x:%x]\n", block, offset);

skip_read:
	memset(page_address(page) + bytes, 0, PAGE_CACHE_SIZE - bytes);
	flush_dcache_page(page);
	SetPageUptodate(page);
	UnlockPage(page);

	return 0;
}


#define SIZE 1024
static int squashfs_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	squashfs_sb_info *msBlk = &inode->i_sb->u.squashfs_sb;
	squashfs_super_block *sBlk = &msBlk->sBlk;
	unsigned char block_list[SIZE];
	unsigned short *block_listp;
	int index = sBlk->block_log > PAGE_CACHE_SHIFT ?
		page->index >> (sBlk->block_log - PAGE_CACHE_SHIFT) :
		page->index << (PAGE_CACHE_SHIFT - sBlk->block_log);
	int block = inode->u.squashfs_i.start_block, i = 0;
	int bytes = inode->u.squashfs_i.block_list_start;
	int offset = inode->u.squashfs_i.offset;
	int file_blocks = ((inode->i_size - 1) >> sBlk->block_log) + 1;
	
	TRACE("Entered squashfs_readpage, page index %d, start block %x\n", page->index,
		inode->u.squashfs_i.start_block);

	if(index > file_blocks)
		goto skip_read;

	for(;;) {
		int blocks = (index + 1 - i) > (SIZE >> 1) ? SIZE >> 1 : (index + 1 - i);
		if(msBlk->swap) {
			unsigned char sblock_list[SIZE];
			if(!squashfs_get_cached_block(inode->i_sb, (char *) sblock_list, bytes, offset, blocks << 1, &bytes, &offset)) {
				ERROR("Unable to read block list [%d:%x]\n", bytes, offset);
				goto skip_read;
			}
			SQUASHFS_SWAP_SHORTS(((unsigned short *)block_list), ((unsigned short *)sblock_list), blocks);
		} else
			if(!squashfs_get_cached_block(inode->i_sb, (char *) block_list, bytes, offset, blocks << 1, &bytes, &offset)) {
				ERROR("Unable to read block list [%d:%x]\n", bytes, offset);
				goto skip_read;
			}
		for(block_listp = (unsigned short *) block_list; i < index && blocks; i ++, block_listp ++, blocks --)
			block += SQUASHFS_COMPRESSED_SIZE(*block_listp);
		if(blocks)
			break;
	}

	if(sBlk->block_log > PAGE_CACHE_SHIFT) {
		int mask = (1 << (sBlk->block_log - PAGE_CACHE_SHIFT)) - 1;
		int start_index = page->index & ~mask;
		int end_index = start_index | mask;
		int byte_offset = 0;

		down(&msBlk->read_page_mutex);
		if(!(bytes = read_data(inode->i_sb, msBlk->read_page, block, *block_listp, NULL))) {
			ERROR("1. Unable to read page, block %x, size %x, name %s\n", block, (int) *block_listp, file->f_dentry->d_name.name);
			goto skip_read;
		}

		for(i = start_index; i <= end_index && byte_offset < bytes; i++, byte_offset += PAGE_CACHE_SIZE) {
			int available_bytes = (bytes - byte_offset) > PAGE_CACHE_SIZE ? PAGE_CACHE_SIZE : bytes - byte_offset;

			TRACE("bytes %d, i %d, byte_offset %d, available_bytes %d\n", bytes, i, byte_offset, available_bytes);

			if(i == page->index)  {
				memcpy(page_address(page), msBlk->read_page + byte_offset, available_bytes);
				memset(page_address(page) + available_bytes, 0, PAGE_CACHE_SIZE - available_bytes);
				flush_dcache_page(page);
				SetPageUptodate(page);
				UnlockPage(page);
			}  else {
				struct page *push_page;

				if(push_page = grab_cache_page_nowait(page->mapping, i)) {
					memcpy(page_address(push_page), msBlk->read_page + byte_offset, available_bytes);
					memset(page_address(push_page) + available_bytes, 0, PAGE_CACHE_SIZE - available_bytes);
					flush_dcache_page(push_page);
					SetPageUptodate(push_page);
					UnlockPage(push_page);
					page_cache_release(push_page);
				}
			}
		}
		up( &msBlk->read_page_mutex);

		return 0;

	} else if(sBlk->block_log == PAGE_CACHE_SHIFT) {
		if(!(bytes = read_data(inode->i_sb, page_address(page), block, *block_listp, NULL)))
			ERROR("2. Unable to read page, block %x, size %x, name %s\n", block, (int) *block_listp, file->f_dentry->d_name.name);

	} else {
		int i_end = index + (1 << (PAGE_CACHE_SHIFT - sBlk->block_log));
		char *p = (char *) page_address(page);
		int byte;

		if(i_end > file_blocks)
			i_end = file_blocks;

		while(index < i_end) {
			if(!(byte = read_data(inode->i_sb, p, block, *block_listp, NULL))) {
				ERROR("3. Unable to read page, block %x, size %x, name %s\n", block, (int) *block_listp, file->f_dentry->d_name.name);
				goto skip_read;
			}
			block += SQUASHFS_COMPRESSED_SIZE(*block_listp);
			p += byte;
			bytes += byte;
			index ++;
			block_listp ++;
		}
	}

skip_read:
	memset(page_address(page) + bytes, 0, PAGE_CACHE_SIZE - bytes);
	flush_dcache_page(page);
	SetPageUptodate(page);
	UnlockPage(page);

	return 0;
}


static int squashfs_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct inode *i = file->f_dentry->d_inode;
	squashfs_sb_info *msBlk = &i->i_sb->u.squashfs_sb;
	squashfs_super_block *sBlk = &msBlk->sBlk;
	int next_block = i->u.squashfs_i.start_block + sBlk->directory_table_start, next_offset =
		i->u.squashfs_i.offset, length = 0, dirs_read = 0, dir_count, bytes;
	squashfs_dir_header dirh;
	char buffer[sizeof(squashfs_dir_entry) + SQUASHFS_NAME_LEN + 1];
	squashfs_dir_entry *dire = (squashfs_dir_entry *) buffer;

	TRACE("Entered squashfs_readdir [%x:%x]\n", next_block, next_offset);

	while(length < i->i_size) {
		/* read directory header */
		if(msBlk->swap) {
			squashfs_dir_header sdirh;
			if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &sdirh, next_block,
						next_offset, sizeof(sdirh), &next_block, &next_offset), !bytes)
				goto failed_read;
			SQUASHFS_SWAP_DIR_HEADER(&dirh, &sdirh);
		} else
			if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &dirh, next_block,
						next_offset, sizeof(dirh), &next_block, &next_offset), !bytes)
				goto failed_read;

		dir_count = dirh.count + 1;
		while(dir_count--) {
			if(msBlk->swap) {
				squashfs_dir_entry sdire;
				if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &sdire, next_block,
							next_offset, sizeof(sdire), &next_block, &next_offset), !bytes)
					goto failed_read;
				SQUASHFS_SWAP_DIR_ENTRY(dire, &sdire);
			} else
				if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) dire, next_block,
							next_offset, sizeof(*dire), &next_block, &next_offset), !bytes)
					goto failed_read;

			if(length += bytes = squashfs_get_cached_block(i->i_sb, dire->name, next_block,
						next_offset, dire->size + 1, &next_block, &next_offset), !bytes)
				goto failed_read;

			if(file->f_pos >= length)
				continue;

			dire->name[dire->size + 1] = '\0';

			TRACE("Calling filldir(%x, %s, %d, %d, %x:%x, %d)\n", dirent,
			dire->name, dire->size + 1, (int) file->f_pos,
			dirh.start_block, dire->offset, squashfs_filetype_table[dire->type]);

			if(filldir(dirent, dire->name, dire->size + 1, file->f_pos, SQUASHFS_MK_VFS_INODE(dirh.start_block,
							dire->offset), squashfs_filetype_table[dire->type]) < 0) {
				TRACE("Filldir returned less than 0\n");
				return dirs_read;
			}

			file->f_pos = length;
			dirs_read ++;
		}
	}

	return dirs_read;

failed_read:
	ERROR("Unable to read directory block [%x:%x]\n", next_block, next_offset);
	return 0;
}


static struct dentry *squashfs_lookup(struct inode *i, struct dentry *dentry)
{
	const char *name =dentry->d_name.name;
	int len = dentry->d_name.len;
	struct inode *inode = NULL;
	squashfs_sb_info *msBlk = &i->i_sb->u.squashfs_sb;
	squashfs_super_block *sBlk = &msBlk->sBlk;
	int next_block = i->u.squashfs_i.start_block + sBlk->directory_table_start, next_offset =
		i->u.squashfs_i.offset, length = 0, dir_count, bytes;
	squashfs_dir_header dirh;
	char buffer[sizeof(squashfs_dir_entry) + SQUASHFS_NAME_LEN];
	squashfs_dir_entry *dire = (squashfs_dir_entry *) buffer;

	TRACE("Entered squashfs_lookup [%x:%x]\n", next_block, next_offset);

	while(length < i->i_size) {
		/* read directory header */
		if(msBlk->swap) {
			squashfs_dir_header sdirh;
			if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &sdirh, next_block, next_offset,
						sizeof(sdirh), &next_block, &next_offset), !bytes)
				goto failed_read;
			SQUASHFS_SWAP_DIR_HEADER(&dirh, &sdirh);
		} else
			if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &dirh, next_block, next_offset,
						sizeof(dirh), &next_block, &next_offset), !bytes)
				goto failed_read;

		dir_count = dirh.count + 1;
		while(dir_count--) {
			if(msBlk->swap) {
				squashfs_dir_entry sdire;
				if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) &sdire,
							next_block,next_offset, sizeof(sdire), &next_block, &next_offset), !bytes)
					goto failed_read;
				SQUASHFS_SWAP_DIR_ENTRY(dire, &sdire);
			} else
				if(length += bytes = squashfs_get_cached_block(i->i_sb, (char *) dire,
							next_block,next_offset, sizeof(*dire), &next_block, &next_offset), !bytes)
					goto failed_read;

			if(length += bytes = squashfs_get_cached_block(i->i_sb, dire->name,
						next_block, next_offset, dire->size + 1, &next_block, &next_offset), !bytes)
				goto failed_read;

			if((len == dire->size + 1) && !strncmp(name, dire->name, len)) {
				squashfs_inode ino = SQUASHFS_MKINODE(dirh.start_block, dire->offset);

				TRACE("calling squashfs_iget for directory entry %s, inode %x:%x\n",
						name, dirh.start_block, dire->offset);

				inode = squashfs_iget(i->i_sb, ino);

				goto exit_loop;
			}
		}
	}

exit_loop:
	d_add(dentry, inode);
	return ERR_PTR(0);

failed_read:
	ERROR("Unable to read directory block [%x:%x]\n", next_block, next_offset);
	goto exit_loop;
}


static void squashfs_put_super(struct super_block *s)
{
	if(s->u.squashfs_sb.block_cache) kfree(s->u.squashfs_sb.block_cache);
	if(s->u.squashfs_sb.read_data) kfree(s->u.squashfs_sb.read_data);
	if(s->u.squashfs_sb.read_page) kfree(s->u.squashfs_sb.read_page);
	if(s->u.squashfs_sb.uid) kfree(s->u.squashfs_sb.uid);
	s->u.squashfs_sb.block_cache = (void *) s->u.squashfs_sb.uid =
		s->u.squashfs_sb.read_data = s->u.squashfs_sb.read_page = NULL;
}


static int __init init_squashfs_fs(void)
{

#ifdef __TI_SQUASHFS_LZMA
	if(!(lzma_data = (char *) vmalloc(lzma_workspace_size()))) {
		ERROR("Failed to allocate lzma workspace\n");
		return -ENOMEM;
	}
	lzma_init(lzma_data, lzma_workspace_size());
#else
	if(!(stream.workspace = (char *) vmalloc(zlib_fs_inflate_workspacesize()))) {
		ERROR("Failed to allocate zlib workspace\n");
		return -ENOMEM;
	}
#endif
	return register_filesystem(&squashfs_fs_type);
}


static void __exit exit_squashfs_fs(void)
{
#ifdef __TI_SQUASHFS_LZMA
	vfree(lzma_data);
#else
	vfree(stream.workspace);
#endif
	unregister_filesystem(&squashfs_fs_type);
}


EXPORT_NO_SYMBOLS;

module_init(init_squashfs_fs);
module_exit(exit_squashfs_fs);
MODULE_DESCRIPTION("squashfs, a compressed read-only filesystem");
MODULE_AUTHOR("Phillip Lougher <phillip@lougher.demon.co.uk>");
MODULE_LICENSE("GPL");
