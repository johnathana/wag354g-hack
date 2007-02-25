#ifndef	__DRIVER_CHAR_SCAN_KEYB_H
#define	__DRIVER_CHAR_SCAN_KEYB_H
/*
 *	$Id: scan_keyb.h,v 1.1.1.2 2005/03/28 06:56:02 sparq Exp $
 *	Copyright (C) 2000 YAEGASHI Takeshi
 *	Generic scan keyboard driver
 */

int register_scan_keyboard(int (*scan)(unsigned char *buffer),
			   const unsigned char *table,
			   int length);

void __init scan_kbd_init(void);

#endif
