/*
 * MTD utility functions
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: mtd.h,v 1.1.1.2 2005/03/28 06:58:27 sparq Exp $
 */

#ifndef _mtd_h_
#define _mtd_h_

/*
 * Open an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @param	flags	open() flags
 * @return	return value of open()
 */
extern int mtd_open(const char *mtd, int flags);

/*
 * Erase an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @return	0 on success and errno on failure
 */
extern int mtd_erase(const char *mtd);

/*
 * Write a file or a URL to an MTD device
 * @param	path	file to write or a URL
 * @param	mtd	path to or partition name of MTD device 
 * @return	0 on success and errno on failure
 */
extern int mtd_write(const char *path, const char *mtd);

extern int restore_nvram(const char *path, const char *mtd);

#endif /* _mtd_h_ */
