#ifndef SQUASHFS_FS_SB
#define SQUASHFS_FS_SB
/*
 * Squashfs
 *
 * Copyright (c) 2002 Phillip Lougher <phillip@lougher.demon.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * squashfs_fs_sb.h
 */

#include <linux/squashfs_fs.h>

typedef struct {
	unsigned int	block;
	int		length;
	unsigned int	next_index;
	char		*data;
	} squashfs_cache;

typedef struct squashfs_sb_info {
	squashfs_super_block	sBlk;
	int			devblksize;
	int			devblksize_log2;
	int			swap;
	squashfs_cache		*block_cache;
	int			next_cache;
	squashfs_uid		*uid;
	squashfs_uid		*guid;
	unsigned int		read_size;
	char			*read_data;
	char			*read_page;
	struct semaphore	read_page_mutex;
	struct semaphore	block_cache_mutex;
	wait_queue_head_t	waitq;
	} squashfs_sb_info;
#endif
