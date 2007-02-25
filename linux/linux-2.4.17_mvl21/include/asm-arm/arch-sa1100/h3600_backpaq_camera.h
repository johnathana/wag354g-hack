/*
 * Driver for the Compaq iPAQ Mercury Backpaq camera
 * Video4Linux interface
 *
 * Copyright 2001 Compaq Computer Corporation.
 *
 * Use consistent with the GNU GPL is permitted,
 * provided that this copyright notice is
 * preserved in its entirety in all copies and derived works.
 *
 * COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
 * AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
 * FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 * Author: Andrew Christian 
 *         <andyc@handhelds.org>
 *         4 May 2001
 *
 * Driver for Mercury BackPAQ camera
 *
 * Issues to be addressed:
 *    1. Writing to the FPGA when we need to do a functionality change
 *    2. Sampling the pixels correctly and building a pixel array
 *    3. Handling different pixel formats correctly
 *    4. Changing the contrast, brightness, white balance, and so forth.
 *    5. Specifying a subregion (i.e., setting "top, left" and SUBCAPTURE)
 */

#ifndef _H3600_BACKPAQ_CAMERA_H
#define _H3600_BACKPAQ_CAMERA_H

#include <linux/videodev.h>

#define H3600_BACKPAQ_CAMERA_DEFAULT_CLOCK_DIVISOR   0x100  /* 5 fps */
#define H3600_BACKPAQ_CAMERA_DEFAULT_INTERRUPT_FIFO  0x20   /* 32 deep */

#define H3600_BACKPAQ_CAMERA_DEFAULT_SPECIAL_MODES   3 /* 0x13 */
#define H3600_BACKPAQ_CAMERA_DEFAULT_AUTOBRIGHT      4 /* 0x64 */
#define H3600_BACKPAQ_CAMERA_DEFAULT_GAIN_FORMAT     0 /* 0x70 -  8-bit, no gain */
#define H3600_BACKPAQ_CAMERA_DEFAULT_POWER_SETTING   4 /* 0x84 */
#define H3600_BACKPAQ_CAMERA_DEFAULT_POWER_MGMT    0xf /* 0x9f */

struct h3600_backpaq_camera_params {
	/* FPGA settings */
/*	unsigned short integration_time;  */ /* Mapped to "brightness" */
	unsigned short clock_divisor;      /* 0x100 = 5 fps */
	unsigned short interrupt_fifo; 

	/* Imager settings */
	unsigned char  power_setting;      /* Normally "4" */
	unsigned char  gain_format;        /* 0x8 = 12-bit mode [not allowed]
	                                      0x0 = 8-bit mode  (normal)
					      0x1 = 8-bit gain of 2
					      0x2 = 8-bit gain of 4
					      0x3 = 8-bit gain of 8
					      0x4 = 8-bit gain of 16 */
	unsigned char  power_mgmt;         /* See docs : uses bits 0 through 3 */
	unsigned char  special_modes;      /* See docs : uses bits 0 through 2 */
	unsigned char  autobright;         /* See docs */
	
	/* Driver image processing settings */
	unsigned char  read_polling_mode;  /* Force "read" to use polling mode */
	unsigned char  flip;               /* Set to TRUE to invert image */
};

/*
   Private IOCTL to control camera parameters and image flipping
 */
#define H3600CAM_G_PARAMS   _IOR ('v', BASE_VIDIOCPRIVATE+0, struct h3600_backpaq_camera_params)
#define H3600CAM_S_PARAMS   _IOWR('v', BASE_VIDIOCPRIVATE+1, struct h3600_backpaq_camera_params)
#define H3600CAM_RESET      _IO  ('v', BASE_VIDIOCPRIVATE+2 )

#endif /*  _H3600_BACKPAQ_CAMERA_H */

