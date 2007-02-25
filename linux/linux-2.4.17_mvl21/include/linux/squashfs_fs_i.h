#ifndef SQUASHFS_FS_I
#define SQUASHFS_FS_I
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
 * squashfs_fs_i.h
 */

typedef struct squashfs_inode_info {
	unsigned int	start_block;
	unsigned int	block_list_start;
	unsigned int	offset;
	} squashfs_inode_info;
#endif
