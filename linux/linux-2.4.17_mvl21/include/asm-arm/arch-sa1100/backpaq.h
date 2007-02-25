/*
 *
 * Definitions for Mercury BackPAQ
 *
 * Copyright 2000 Compaq Computer Corporation.
 *
 * Copying or modifying this code for any purpose is permitted,
 * provided that this copyright notice is preserved in its entirety
 * in all copies or modifications.  COMPAQ COMPUTER CORPORATION
 * MAKES NO WARRANTIES, EXPRESSED OR IMPLIED, AS TO THE USEFULNESS
 * OR CORRECTNESS OF THIS CODE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 *
 * Author: Jamey Hicks.
 *
 * Change: 6/8/01   Power management callback interface added.
 *                  Andrew Christian
 *
 */

#ifndef _INCLUDE_BACKPAQ_H_
#define _INCLUDE_BACKPAQ_H_

#include <linux/types.h>
#include <asm/hardware.h>

/********** Device management support **************/

enum {
	H3600_BACKPAQ_SUSPEND,
	H3600_BACKPAQ_RESUME,
	H3600_BACKPAQ_EJECT,
	H3600_BACKPAQ_INSERT
};

typedef int h3600_backpaq_request_t;  /* Mostly mapped directly from pm.h */

enum {
	H3600_BACKPAQ_UNKNOWN_DEV = 0,
	H3600_BACKPAQ_FPGA_DEV,
	H3600_BACKPAQ_CAMERA_DEV,
	H3600_BACKPAQ_AUDIO_DEV,
	H3600_BACKPAQ_ACCEL_DEV,
	H3600_BACKPAQ_GASGAUGE_DEV,
	H3600_BACKPAQ_FLASH_DEV,
};

typedef int h3600_backpaq_dev_t;
struct h3600_backpaq_device;
typedef int (*h3600_backpaq_callback)(struct h3600_backpaq_device *dev, h3600_backpaq_request_t rqst);

struct h3600_backpaq_device {
	h3600_backpaq_dev_t       type;
	unsigned long             id;
	h3600_backpaq_callback    callback;
	void                     *data;

	struct list_head          entry;
};

int                          h3600_backpaq_present( void );
struct h3600_backpaq_device* h3600_backpaq_register_device( h3600_backpaq_dev_t type,
							    unsigned long id,
							    h3600_backpaq_callback callback );
void                         h3600_backpaq_unregister_device(struct h3600_backpaq_device *device);

/********** OEM area of the EEPROM **********/

struct h3600_backpaq_eeprom {
	u8  major_revision;
	u8  minor_revision;
	u8  fpga_version; 
	u8  camera;
	u16 accel_offset_x;
	u16 accel_scale_x;
	u16 accel_offset_y;
	u16 accel_scale_y;
	u32 serial_number;
	u32 flash_start;
	u32 flash_length;
	u32 sysctl_start;
};

enum {
        BACKPAQ_EEPROM_FPGA_VERTEX_100   = 0,
        BACKPAQ_EEPROM_FPGA_VERTEX_100E  = 1,
        BACKPAQ_EEPROM_FPGA_VERTEX_200E  = 2,
        BACKPAQ_EEPROM_FPGA_VERTEX_300E  = 3,
	BACKPAQ_EEPROM_FPGA_MAXIMUM      = 4
};


#define BACKPAQ_EEPROM_CAMERA_ORIENTATION  0x80     /* High order bit set for landscape cameras */
#define BACKPAQ_EEPROM_CAMERA_TYPE_MASK    0x7f     /* All other bits for camera type */

/* Valid BackPAQ camera types */
enum { 
	BACKPAQ_CAMERA_SMAL_ORIGINAL_GREY  = 0,
	BACKPAQ_CAMERA_SMAL_ORIGINAL_COLOR = 1,
	BACKPAQ_CAMERA_SMAL_RELEASE_COLOR  = 2,
	BACKPAQ_CAMERA_SMAL_AUTOBRITE_09   = 3
};

extern struct h3600_backpaq_eeprom h3600_backpaq_eeprom_shadow;

/*********** Memory-mapped CPLD interface **********/
/* In the future the offset from Bank2Virt will be extracted from EEPROM */
//#define _BackpaqSysctlBase       (H3600_BANK_2_VIRT + 0x2000000) 
#define _BackpaqSysctlBase       (H3600_BANK_2_VIRT + h3600_backpaq_eeprom_shadow.sysctl_start)

#define _BackpaqSysctlFirmwareVersion    0x04
#define _BackpaqSysctlFPGAControl        0x08
#define _BackpaqSysctlFPGAStatus         0x0c
#define _BackpaqSysctlFPGAProgram        0x10
#define _BackpaqSysctlPCMCIAPower        0x14
#define _BackpaqSysctlFlashControl       0x18
#define _BackpaqSysctlGenControl         0x1c

#define SYSCTL_OFFSET(s,x) (*((volatile s*) (_BackpaqSysctlBase + x)))

#define BackpaqSysctlFirmwareVersion  SYSCTL_OFFSET(u16,_BackpaqSysctlFirmwareVersion)
#define BackpaqSysctlFPGAControl      SYSCTL_OFFSET(u16,_BackpaqSysctlFPGAControl)
#define BackpaqSysctlFPGAStatus       SYSCTL_OFFSET(u16,_BackpaqSysctlFPGAStatus)
#define BackpaqSysctlFPGAProgram      SYSCTL_OFFSET(u16,_BackpaqSysctlFPGAProgram)
#define BackpaqSysctlPCMCIAPower      SYSCTL_OFFSET(u16,_BackpaqSysctlPCMCIAPower)
#define BackpaqSysctlFlashControl     SYSCTL_OFFSET(u16,_BackpaqSysctlFlashControl)
#define BackpaqSysctlGenControl       SYSCTL_OFFSET(u16,_BackpaqSysctlGenControl)

//struct mercury_backpaq_sysctl {
//	volatile u16 hardware_version;   /* 0x00 */
//	  u16 pad1;
//	volatile u16 firmware_version;   /* 0x04 */
//	  u16 pad2;
//	volatile u16 fpga_ctl;           /* 0x08 */
//	  u16 pad3;
//	volatile u16 fpga_status;        /* 0x0c */
//	  u16 pad4;
//	volatile u16 fpga_program;       /* 0x10 */
//	  u16 pad5;
//	volatile u16 pcmcia_power;       /* 0x14 */
//	  u16 pad6;
//	volatile u16 flash_control;      /* 0x18 */ 
//	  u16 pad7;
//	volatile u16 gencontrol;         /* 0x1c */ 
//	  u16 pad8;
//	  u32 pad[4];  /* 0x20, 0x24, 0x28, 0x2c */
//	long socket[2]; /* readable image at 0x30 and 0x34 */ 
//};

#define _BackpaqSocketBase               (H3600_BANK_4_VIRT) /* see h3600.c */
#define SOCKET_OFFSET(s,x) (*((volatile s *) (_BackpaqSocketBase + x)))

/* Memory locations controlled by the FPGA */ 
#define _BackpaqSocketFPGAFirmwareVersion      0x80     /* u16  r/w */
#define _BackpaqSocketCameraWritethru          0x84     /* u8     w */
#define _BackpaqSocketCameraFifoData           0x88     /* u32  r/w */
#define _BackpaqSocketCameraLiveMode           0x8c     /* u16  r/w */
#define _BackpaqSocketCameraIntegrationTime    0x90     /* u16  r/w */
#define _BackpaqSocketCameraClockDivisor       0x94     /* u16  r/w */
#define _BackpaqSocketCameraFifoInfo           0x98     /* u16  r   */
#define _BackpaqSocketCameraFifoDataAvail      0x9c     /* u16  r   */
#define _BackpaqSocketCameraFifoStatus         0xa0     /* u16  r   */
#define _BackpaqSocketCameraRowCount           0xa4     /* u16  r   */
#define _BackpaqSocketFPGAReset                0xa8     /* u8     w */
#define _BackpaqSocketCameraIntegrationCmd     0xac     /* u16  r/w */
#define _BackpaqSocketCameraInterruptFifo      0xb0     /* u16  r/w */
#define _BackpaqSocketFPGAInterruptMask        0xb4     /* u16  r/w */
#define _BackpaqSocketFPGAInterruptStatus      0xb8     /* u16  r   */
#define _BackpaqSocketFPGATest32Addr           0xbc     /* u32  r/w */
#define _BackpaqSocketAccelX1                  0xc0     /* u16  r   */
#define _BackpaqSocketAccelX2                  0xc4     /* u16  r   */
#define _BackpaqSocketAccelY1                  0xc8     /* u16  r   */
#define _BackpaqSocketAccelY2                  0xcc     /* u16  r   */
#define _BackpaqSocketGasGaugeCommand          0xd0     /* u16  r/w */
#define _BackpaqSocketGasGaugeResult           0xd4     /* u16  r   */
#define _BackpaqSocketGasGaugeWState           0xd8     /* u16  r   */
#define _BackpaqSocketGasGaugeRState           0xdc     /* u16  r   */
#define _BackpaqSocketCameraDecimationMode     0xe4     /* u16  r/w */

/* Memory locations controlled by the CPLD(s) for the PCMCIA slots */
#define _BackpaqSocketPCMCIABase_0            0x300
#define _BackpaqSocketPCMCIABase_1            0x380

#define BackpaqSocketFPGAFirmwareVersion     SOCKET_OFFSET(u16,_BackpaqSocketFPGAFirmwareVersion)
#define BackpaqSocketCameraWritethru         SOCKET_OFFSET(u8 ,_BackpaqSocketCameraWritethru)
#define BackpaqSocketCameraFifoData          SOCKET_OFFSET(u32,_BackpaqSocketCameraFifoData)
#define BackpaqSocketCameraLiveMode          SOCKET_OFFSET(u16,_BackpaqSocketCameraLiveMode)
#define BackpaqSocketCameraIntegrationTime   SOCKET_OFFSET(u16,_BackpaqSocketCameraIntegrationTime)
#define BackpaqSocketCameraClockDivisor      SOCKET_OFFSET(u16,_BackpaqSocketCameraClockDivisor)
#define BackpaqSocketCameraFifoInfo          SOCKET_OFFSET(u16,_BackpaqSocketCameraFifoInfo)
#define BackpaqSocketCameraFifoDataAvail     SOCKET_OFFSET(u16,_BackpaqSocketCameraFifoDataAvail)
#define BackpaqSocketCameraFifoStatus        SOCKET_OFFSET(u16,_BackpaqSocketCameraFifoStatus)
#define BackpaqSocketCameraRowCount          SOCKET_OFFSET(u16,_BackpaqSocketCameraRowCount)
#define BackpaqSocketFPGAReset               SOCKET_OFFSET(u8 ,_BackpaqSocketFPGAReset)
#define BackpaqSocketCameraIntegrationCmd    SOCKET_OFFSET(u16,_BackpaqSocketCameraIntegrationCmd)
#define BackpaqSocketCameraInterruptFifo     SOCKET_OFFSET(u16,_BackpaqSocketCameraInterruptFifo)
#define BackpaqSocketFPGAInterruptMask       SOCKET_OFFSET(u16,_BackpaqSocketFPGAInterruptMask)
#define BackpaqSocketFPGAInterruptStatus     SOCKET_OFFSET(u16,_BackpaqSocketFPGAInterruptStatus)
#define BackpaqSocketFPGATest32Addr          SOCKET_OFFSET(u32,_BackpaqSocketFPGATest32Addr)
#define BackpaqSocketAccelX1                 SOCKET_OFFSET(u16,_BackpaqSocketAccelX1)
#define BackpaqSocketAccelX2                 SOCKET_OFFSET(u16,_BackpaqSocketAccelX2)
#define BackpaqSocketAccelY1                 SOCKET_OFFSET(u16,_BackpaqSocketAccelY1)
#define BackpaqSocketAccelY2                 SOCKET_OFFSET(u16,_BackpaqSocketAccelY2)
#define BackpaqSocketGasGaugeCommand         SOCKET_OFFSET(u16,_BackpaqSocketGasGaugeCommand)
#define BackpaqSocketGasGaugeResult          SOCKET_OFFSET(u16,_BackpaqSocketGasGaugeResult)
#define BackpaqSocketGasGaugeWState          SOCKET_OFFSET(u16,_BackpaqSocketGasGaugeWState)
#define BackpaqSocketGasGaugeRState          SOCKET_OFFSET(u16,_BackpaqSocketGasGaugeRState)
#define BackpaqSocketCameraDecimationMode    SOCKET_OFFSET(u16,_BackpaqSocketCameraDecimationMode)

#define BackpaqSocketPCMCIABase_0            SOCKET_OFFSET(u32,_BackpaqSocketPCMCIABase_0)
#define BackpaqSocketPCMCIABase_1            SOCKET_OFFSET(u32,_BackpaqSocketPCMCIABase_1)

/* Hardware versions */
/* ...This needs to be changed to have some useful bit fields */
/* #define BACKPAQ_HARDWARE_VERSION_1 0x00bc *//* Only run if we see this hardware version */

/* FPGA control (sysctl->fpga_ctl) 0x08 */
#define BACKPAQ_FPGACTL_M0      (1 << 0)
#define BACKPAQ_FPGACTL_M1      (1 << 1)
#define BACKPAQ_FPGACTL_M2      (1 << 2)
#define BACKPAQ_FPGACTL_PROGRAM (1 << 3)
/* #define BACKPAQ_FPGACTL_RESET   (1 << 4) */

/* FPGA Status [read-only] (systctl->fpgadout) 0x0c */
#define BACKPAQ_FPGASTATUS_DONE   (1 << 0)
#define BACKPAQ_FPGASTATUS_INITL  (1 << 1)
#define BACKPAQ_FPGASTATUS_EMPTY  (1 << 2)

#define BACKPAQ_OPTION_OPT_ON (1 << 0)
#define BACKPAQ_OPTION_DC_IN  (1 << 1)
#define BACKPAQ_OPTION_MCHG   (1 << 2)
#define BACKPAQ_OPTION_EBAT   (1 << 3)
#define BACKPAQ_OPTION_ETMI   (1 << 4)
#define BACKPAQ_OPTION_ETMO   (1 << 5)

#define BACKPAQ_GASGAUGE_BDIR (1 << 0)
#define BACKPAQ_GASGAUGE_BSCL (1 << 1)
#define BACKPAQ_GASGAUGE_BSDA (1 << 2)

/* Power control (sysctl->pcmcia_power) 0x14 */
#define BACKPAQ_PCMCIA_AEN0   (1 << 0)
#define BACKPAQ_PCMCIA_AEN1   (1 << 1)
#define BACKPAQ_PCMCIA_A5VEN  (1 << 2)
#define BACKPAQ_PCMCIA_A3VEN  (1 << 3)
#define BACKPAQ_PCMCIA_BEN0   (1 << 4)
#define BACKPAQ_PCMCIA_BEN1   (1 << 5)
#define BACKPAQ_PCMCIA_B5VEN  (1 << 6)
#define BACKPAQ_PCMCIA_B3VEN  (1 << 7)
#define BACKPAQ_PWRCTL_50VEN  (1 << 8)
#define BACKPAQ_PWRCTL_25VEN  (1 << 9)
#define BACKPAQ_PWRCTL_18VEN  (1 << 10)

/* Flash control (sysctl->flash_control) 0x18 */
#define BACKPAQ_REGC_FLASH_STATUS0 (1 << 0)
#define BACKPAQ_REGC_FLASH_STATUS1 (1 << 1)
#define BACKPAQ_REGC_ACCEL_X       (1 << 2)
#define BACKPAQ_REGC_ACCEL_Y       (1 << 3)
#define BACKPAQ_REGC_AUD_AGC       (1 << 4)
#define BACKPAQ_REGC_BATT_SDA      (1 << 5)
#define BACKPAQ_REGC_BATT_SDA_DIR  (1 << 6)

#define BACKPAQ_FLASH_ENABLE0 (1 << 4)
#define BACKPAQ_FLASH_ENABLE1 (1 << 5)
#define BACKPAQ_FLASH_VPPEN   (1 << 8)

/* General control register (sysctl->gencontrol) 0x1c */
#define BACKPAQ_REGC_CAM_CLKEN  (1 << 9)
#define BACKPAQ_REGC_AUDIO_CLKEN (1 << 10)
#define BACKPAQ_REGC_AUDIO_AMPEN (1 << 11)

/* Socket register information */
/* Interrupt bits */

#define BACKPAQ_SOCKET_INT_FIFO    (1 << 0)
#define BACKPAQ_SOCKET_INT_VBLANK  (1 << 1)
#define BACKPAQ_SOCKET_INT_WTBUSY  (1 << 2)  /* Write through register is tied up */

#define BACKPAQ_SOCKET_VS1    (1 << 0)
#define BACKPAQ_SOCKET_VS2    (1 << 1)
#define BACKPAQ_SOCKET_STSCHG (1 << 2)
#define BACKPAQ_SOCKET_BVD1   (1 << 2) /* same pin as STSCHG */
#define BACKPAQ_SOCKET_INPACK (1 << 3)
#define BACKPAQ_SOCKET_SPKR   (1 << 4)
#define BACKPAQ_SOCKET_BVD2   (1 << 4) /* same pin as SPKR */
#define BACKPAQ_SOCKET_SKTSEL (1 << 5)
#define BACKPAQ_SOCKET_CD0    (1 << 6)
#define BACKPAQ_SOCKET_CD1    (1 << 7)

#define BACKPAQ_SOCKET_FPGA_RESET (1 << 0)

#define BACKPAQ_GASGAUGE_READ  0x100 
#define BACKPAQ_GASGAUGE_WRITE 0x080 

#define BACKPAQ_GASGAUGE_RSTATE_IDLE 1
#define BACKPAQ_GASGAUGE_WSTATE_IDLE 1

/* Lower 8 bits is the bus width (16 or 32) */
/* Upper 8 bits is the number of bytes to read per read count */ 
#define BACKPAQ_CAMERA_FIFO_INFO_WIDTH 0x00ff   
#define BACKPAQ_CAMERA_FIFO_INFO_BPC   0xff00   



struct backpaq_audio {
   short reg40;
   short pad1;
   short reg44;
   short pad2;
   short reg48;
   short pad3;
   short reg4c;
   short pad4;
   short reg50;
   short pad5;
};

extern struct backpaq_audio *backpaq_audio;

#define BACKPAQ_REG44_L3_DATA_IN (1 << 0)
#define BACKPAQ_REG44_L3_AGCSTAT (1 << 1)
#define BACKPAQ_REG44_L3_OVERFL (1 << 2)
#define BACKPAQ_REG44_RESET     (1 << 3)
#define BACKPAQ_REG44_L3_MODE   (1 << 4)
#define BACKPAQ_REG44_L3_CLOCK  (1 << 5)
#define BACKPAQ_REG44_L3_DIR    (1 << 6)
#define BACKPAQ_REG44_L3_DATA_OUT (1 << 7)
#define BACKPAQ_REG44_L3_BCLK_EN (1 << 8)
#define BACKPAQ_REG44_L3_QMUTE  (1 << 9)

#define BACKPAQ_REG4C_TXD_FIFO_BUSY (1 << 0)
#define BACKPAQ_REG4C_TXD_FIFO_FULL (1 << 0)

#endif
