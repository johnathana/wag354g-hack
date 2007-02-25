/* $Id: ds1307.h,v 1.1.1.2 2005/03/28 06:56:47 sparq Exp $
 *
 * ds1307.h
 *
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2000 Texas Instruments, Inc.
 *
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Dallas Semiconductor RTC (DS1307)
 *
 */
#ifndef __DS1307_H
#define __DS1307_H

/*--------------------------------
 * RTC clock registers
 *-------------------------------*/
#define RTC_SECONDS		0
#define RTC_MINUTES		1
#define RTC_HOURS		2

#define RTC_DAY_OF_WEEK		3
#define RTC_DAY_OF_MONTH	4
#define RTC_MONTH		5
#define RTC_YEAR		6

#define RTC_CONTROL             7

/*--------------------------------
 * RTC_SECONDS register bits
 *-------------------------------*/  
#define RTC_CLOCK_HALT   0x80
#define RTC_CLOCK_ENABLE 0x0


/*--------------------------------
 * RTC_HOURS register bits
 *--------------------------------*/
#define RTC_24_HR_MD     0x0
#define RTC_12_HR_MD     0x40
                                                 
/*--------------------------------
 * RTC_CONTROL register bits
 *-------------------------------*/
/* bits RS0:RS1 */
#define SQ_WAVE_1HZ     0x0
#define SQ_WAVE_4KHZ    0x1
#define SQ_WAVE_8KHZ    0x2
#define SQ_WAVE_32KHZ   0x3

#define SQ_WAVE_ENABLE  0x10
#define SQ_OUT_HI       0x80
#define SQ_OUT_LO       0x00

/*
 * This structure defines how to access various features of
 * different machine types and how to access them.
 */
struct rtc_ops {
	/* How to access the RTC register in a DS1287.  */
	unsigned char (*rtc_read_data)(unsigned long addr);
	void (*rtc_write_data)(unsigned char data, unsigned long addr);
	int (*rtc_bcd_mode)(void);
};

extern struct rtc_ops *rtc_ops;
/*
 * The struct used to pass data via the following ioctl. Similar to the
 * struct tm in <time.h>, but it needs to be here so that the kernel 
 * source is self contained, allowing cross-compiles, etc. etc.
 */

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

/*
 * ioctl calls that are permitted to the /dev/rtc interface, if 
 * CONFIG_RTC was enabled.
 */

#define RTC_AIE_ON	_IO('p', 0x01)	/* Alarm int. enable on		*/
#define RTC_AIE_OFF	_IO('p', 0x02)	/* ... off			*/
#define RTC_UIE_ON	_IO('p', 0x03)	/* Update int. enable on	*/
#define RTC_UIE_OFF	_IO('p', 0x04)	/* ... off			*/
#define RTC_PIE_ON	_IO('p', 0x05)	/* Periodic int. enable on	*/
#define RTC_PIE_OFF	_IO('p', 0x06)	/* ... off			*/

#define RTC_ALM_SET	_IOW('p', 0x07, struct rtc_time) /* Set alarm time  */
#define RTC_ALM_READ	_IOR('p', 0x08, struct rtc_time) /* Read alarm time */
#define RTC_RD_TIME	_IOR('p', 0x09, struct rtc_time) /* Read RTC time   */
#define RTC_SET_TIME	_IOW('p', 0x0a, struct rtc_time) /* Set RTC time    */
#define RTC_IRQP_READ	_IOR('p', 0x0b, unsigned long)	 /* Read IRQ rate   */
#define RTC_IRQP_SET	_IOW('p', 0x0c, unsigned long)	 /* Set IRQ rate    */
#define RTC_EPOCH_READ	_IOR('p', 0x0d, unsigned long)	 /* Read epoch      */
#define RTC_EPOCH_SET	_IOW('p', 0x0e, unsigned long)	 /* Set epoch       */

#ifndef BCD_TO_BIN
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)
#endif

#endif /* __DS1307_H */





