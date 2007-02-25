/*------------------------------------------------------------------------------*/
/*                                                                             	*/
/*   Copyright (C) 1999-2003 by Texas Instruments, Inc.  All rights reserved.  	*/
/*   Copyright (C) 2001-2003 Telogy Networks.	    						   	*/
/*                                                                              */
/*     IMPORTANT - READ CAREFULLY BEFORE PROCEEDING TO USE SOFTWARE.           	*/
/*                                                                             	*/
/*  This document is displayed for you to read prior to using the software     	*/
/*  and documentation.  By using the software and documentation, or opening    	*/
/*  the sealed packet containing the software, or proceeding to download the   	*/
/*  software from a Bulletin Board System(BBS) or a WEB Server, you agree to   	*/
/*  abide by the following Texas Instruments License Agreement. If you choose  	*/
/*  not to agree with these provisions, promptly discontinue use of the        	*/
/*  software and documentation and return the material to the place you        	*/
/*  obtained it.                                                               	*/
/*                                                                             	*/
/*                               *** NOTE ***                                  	*/
/*                                                                             	*/
/*  The licensed materials contain MIPS Technologies, Inc. confidential        	*/
/*  information which is protected by the appropriate MIPS Technologies, Inc.  	*/
/*  license agreement.  It is your responsibility to comply with these         	*/
/*  licenses.                                                                  	*/
/*                                                                             	*/
/*                   Texas Instruments License Agreement                       	*/
/*                                                                             	*/
/*  1. License - Texas Instruments (hereinafter "TI"), grants you a license    	*/
/*  to use the software program and documentation in this package ("Licensed   	*/
/*  Materials") for Texas Instruments broadband products.                      	*/
/*                                                                             	*/
/*  2. Restrictions - You may not reverse-assemble or reverse-compile the      	*/
/*  Licensed Materials provided in object code or executable format.  You may  	*/
/*  not sublicense, transfer, assign, rent, or lease the Licensed Materials    	*/
/*  or this Agreement without written permission from TI.                      	*/
/*                                                                             	*/
/*  3. Copyright - The Licensed Materials are copyrighted. Accordingly, you    	*/
/*  may either make one copy of the Licensed Materials for backup and/or       	*/
/*  archival purposes or copy the Licensed Materials to another medium and     	*/
/*  keep the original Licensed Materials for backup and/or archival purposes.  	*/
/*                                                                             	*/
/*  4. Runtime and Applications Software - You may create modified or          	*/
/*  derivative programs of software identified as Runtime Libraries or         	*/
/*  Applications Software, which, in source code form, remain subject to this  	*/
/*  Agreement, but object code versions of such derivative programs are not    	*/
/*  subject to this Agreement.                                                 	*/
/*                                                                             	*/
/*  5. Warranty - TI warrants the media to be free from defects in material    	*/
/*  and workmanship and that the software will substantially conform to the    	*/
/*  related documentation for a period of ninety (90) days after the date of   	*/
/*  your purchase. TI does not warrant that the Licensed Materials will be     	*/
/*  free from error or will meet your specific requirements.                   	*/
/*                                                                             	*/
/*  6. Remedies - If you find defects in the media or that the software does   	*/
/*  not conform to the enclosed documentation, you may return the Licensed     	*/
/*  Materials along with the purchase receipt, postage prepaid, to the         	*/
/*  following address within the warranty period and receive a refund.         	*/
/*                                                                             	*/
/*  TEXAS INSTRUMENTS                                                          	*/
/*  Application Specific Products, MS 8650                                     	*/
/*  c/o ADAM2 Application Manager                                              	*/
/*  12500 TI Boulevard                                                         	*/
/*  Dallas, TX 75243  - U.S.A.                                                 	*/
/*                                                                             	*/
/*  7. Limitations - TI makes no warranty or condition, either expressed or    	*/
/*  implied, including, but not limited to, any implied warranties of          	*/
/*  merchantability and fitness for a particular purpose, regarding the        	*/
/*  licensed materials.                                                        	*/
/*                                                                             	*/
/*  Neither TI nor any applicable licensor will be liable for any indirect,    	*/
/*  incidental or consequential damages, including but not limited to loss of  	*/
/*  profits.                                                                   	*/
/*                                                                             	*/
/*  8. Term - The license is effective until terminated.   You may terminate   	*/
/*  it at any other time by destroying the program together with all copies,   	*/
/*  modifications and merged portions in any form. It also will terminate if   	*/
/*  you fail to comply with any term or condition of this Agreement.           	*/
/*                                                                             	*/
/*  9. Export Control - The re-export of United States origin software and     	*/
/*  documentation is subject to the U.S. Export Administration Regulations or  	*/
/*  your equivalent local regulations. Compliance with such regulations is     	*/
/*  your responsibility.                                                       	*/
/*                                                                             	*/
/*                         *** IMPORTANT NOTICE ***                            	*/
/*                                                                             	*/
/*  Texas Instruments (TI) reserves the right to make changes to or to         	*/
/*  discontinue any semiconductor product or service identified in this        	*/
/*  publication without notice. TI advises its customers to obtain the latest  	*/
/*  version of the relevant information to verify, before placing orders,      	*/
/*  that the information being relied upon is current.                         	*/
/*                                                                             	*/
/*  TI warrants performance of its semiconductor products and related          	*/
/*  software to current specifications in accordance with TI's standard        	*/
/*  warranty. Testing and other quality control techniques are utilized to     	*/
/*  the extent TI deems necessary to support this warranty. Unless mandated    	*/
/*  by government requirements, specific testing of all parameters of each     	*/
/*  device is not necessarily performed.                                       	*/
/*                                                                             	*/
/*  Please be aware that Texas Instruments products are not intended for use   	*/
/*  in life-support appliances, devices, or systems. Use of a TI product in    	*/
/*  such applications without the written approval of the appropriate TI       	*/
/*  officer is prohibited. Certain applications using semiconductor devices    	*/
/*  may involve potential risks of injury, property damage, or loss of life.   	*/
/*  In order to minimize these risks, adequate design and operating            	*/
/*  safeguards should be provided by the customer to minimize inherent or      	*/
/*  procedural hazards. Inclusion of TI products in such applications is       	*/
/*  understood to be fully at the risk of the customer using TI devices or     	*/
/*  systems.                                                                   	*/
/*                                                                             	*/
/*  TI assumes no liability for TI applications assistance, customer product   	*/
/*  design, software performance, or infringement of patents or services       	*/
/*  described herein. Nor does TI warrant or represent that license, either    	*/
/*  expressed or implied, is granted under any patent right, copyright, mask   	*/
/*  work right, or other intellectual property right of TI covering or         	*/
/*  relating to any combination, machine, or process in which such             	*/
/*  semiconductor products or services might be or are used.                   	*/
/*                                                                             	*/
/*  All company and/or product names are trademarks and/or registered          	*/
/*  trademarks of their respective manaufacturers.                             	*/
/*------------------------------------------------------------------------------*/
/*                                                                             	*/
/*  Now that you've gotten here you are probably serious about understanding   	*/
/*  the way Adam2 uses the flash. All flash operations are contained in this   	*/
/*  module. There are 7 interfaces to this module. FWBGetBlockSize, FWBErase,  	*/
/*  FWBOpen, FWBWriteByte, and FWBClose, FWBUnLock, FWBLock.                   	*/
/*                                                                             	*/
/*  FWBGetBlockSize(base);       - returns the Flash Block size for the        	*/
/*                                 requested region so that all users of the   	*/
/*                                 flash understand the blocking factor.       	*/
/*                                                                             	*/
/*  FWBErase(base,size,verbose); - Erases from base address of size in blocks. 	*/
/*                                                                             	*/
/*  FWBOpen(Base);               - Will always be called prior to FWBWriteByte,	*/
/*                                 and is responsible for unlocking the region 	*/
/*                                 and preparing for flash writes.             	*/
/*                                                                             	*/
/*  FWBWriteByte(address,data);  - Is called to write data to the flash        	*/
/*                                 address, remember that a FWBClose will be   	*/
/*                                 called prior to any use of the written data.	*/
/*                                                                             	*/
/*  FWBClose()                   - Will complete any pending flash writes to a 	*/
/*                                 region, and lock the flash.                 	*/
/*                                                                             	*/
/*  FWBUnLock(base,size)         - Unlocks the blocks specified                	*/
/*                                                                             	*/
/*  FWBLock(base,size)           - Lock the blocks specified                   	*/
/*                                                                             	*/
/*  The use of FWBOpen, FWBWriteByte, and FWBClose allows for efficient block  	*/
/*  writes. Remember it is possible to only get One FWBWriteByte in-between a  	*/
/*  FWBOpen and a FWBClose.                                                    	*/
/*                                                                         and a FWBClose.                                                    	*/
/*                                                                             	*/
/*  During the Boot process the FWBGetBlockSize is called. This block can be   	*/
/*  called at any time, and returns the size of the flash blocks for the       	*/
/*  requested region. It is possible to have different size flash blocks in    	*/
/*  different memory regions.                                                  	*/
/*                                                                             	*/
/*  Any time the Kernel wishes to erase a block of flash, it will call the     	*/
/*  FWBErase routine. The routine is assumed to erase in blocks. That is       	*/
/*  FWBErase(CS0_BASE,1,0); will erase the block at CS0_BASE. The same can be  	*/
/*  said for FWBErase(CS0_BASE,FWBGetBlockSize(CS0_BASE),0); So that           	*/
/*  FWBErase(CS0_BASE,FWBGetBlockSize(CS0_BASE)+1,0); and                      	*/
/*  FWBErase(CS0_BASE+1,FWBGetBlockSize(CS0_BASE),0); actually erases 2 blocks.	*/
/*                                                                             	*/
/*  It is important to not that the FWBXxxxx routines must always leave the    	*/
/*  flash in a read mode. Failure to do this will result in a system hang.     	*/
/*  sys_printf() is also unuasable whilest the flash in not in a read mode      */
/*                                                                             	*/
/*  And last but not least, all FWB... routines return TRUE if successfull and 	*/
/*  FALSE if unsuccessfull                                                     	*/
/*                                                                             	*/
/*------------------------------------------------------------------------------*/

#include <linux/types.h>
#include "flashop.h"
#include "hw.h"
#include "platform.h"

typedef __u8 uint8;

#define MIN_BOOT_BLOCK_SIZE (128 * 1024)
#define MEM_READ(addr)         	(*((volatile unsigned int *)(addr)))
#define MEM_WRITE(addr,val)    	((*((volatile unsigned int *)(addr)))=(val))

#define MEM_WRITE16(addr,val)  	((*((volatile unsigned short *)(addr)))=(val))
#define MEM_READ16(addr)       	(*((volatile unsigned short *)(addr)))


#define sys_printf printk

void _FlashErrorOut(char *str,int sts);
int _FWBWriteBlock(void);
int _FlashOperationComplete(bit32u adr,bit32u dat,int timeoutloops);

/*---------------------------------------------------------*/
#define FLASH_MAX_WAIT 125

bit32u FWBBaseAdr;
int    FWBvalid;
int    FWBopen=FALSE;
char   FWBwdata[64];
#if 1
#define FLASH_ADDR(off) ((unsigned long)flashutl_base + (off))
#define WBUFSIZE 32
#define FLASH_TRIES 4000000
#define CMD_ADDR ((unsigned long)0xFFFFFFFF)
#define DONE	0x80
#define AMD_CMD 0xaaa
#define BLOCK_BASE 0
#define BLOCK_LIM 1
#define MIN(a, b)               (((a)<(b))?(a):(b))

#define DPRINT		
#define ASSERT(x)

#define dprintk		 
#define FLASH_BASE ((unsigned long)(CS0_BASE))

#if 1

flash_desc_t*   flashutl_desc   = NULL;
flash_cmds_t*	flashutl_cmd	= NULL;
char*		flashutl_base	= NULL;


unlock_cmd_t unlock_cmd_amd = {
#ifdef MIPSEB
/* addr: */	{ 0x0aa8,	0x0556},
#else
/* addr: */	{ 0x0aaa,	0x0554},
#endif
/* data: */	{ 0xaa,		0x55}
};

unlock_cmd_t unlock_cmd_sst = {
#ifdef MIPSEB
/* addr: */	{ 0xaaa8,	0x5556},
#else
/* addr: */	{ 0xaaaa,	0x5554},
#endif
/* data: */	{ 0xaa,		0x55}
};
#define AMD_CMD 0xaaa
#define SST_CMD 0xaaaa

/* intel unlock block cmds */
#define INTEL_UNLOCK1   0x60
#define INTEL_UNLOCK2   0xD0


flash_cmds_t flash_cmds[] = {
//        type  needu   preera  eraseb  erasech write   wbuf    clcsr   rdcsr   rdid    confrm  read 
        { BSC,  0,      0x00,   0x20,   0x00,   0x40,   0x00,   0x50,   0x70,   0x90,   0xd0,   0xff },
        { SCS,  0,      0x00,   0x20,   0x00,   0x40,   0xe8,   0x50,   0x70,   0x90,   0xd0,   0xff },
        { AMD,  1,      0x80,   0x30,   0x10,   0xa0,   0x00,   0x00,   0x00,   0x90,   0x00,   0xf0 },
	{ SST,  1,      0x80,   0x50,   0x10,   0xa0,   0x00,   0x00,   0x00,   0x90,   0x00,   0xf0 },
        { 0 }
};

uint blk8x8k[] = { 0x00000000,
		   0x00002000,
		   0x00004000,
		   0x00006000,
		   0x00008000,
		   0x0000a000,
		   0x0000c000,
		   0x0000e000,
		   0x00010000
};

/* Funky AMD arrangement for 29xx800's */
uint amd800[] = { 0x00000000,		/* 16KB */
		  0x00004000,		/* 32KB */
		  0x0000c000,		/* 8KB */
		  0x0000e000,		/* 8KB */
		  0x00010000,		/* 8KB */
		  0x00012000,		/* 8KB */
		  0x00014000,		/* 32KB */
		  0x0001c000,		/* 16KB */
		  0x00020000
};

/* AMD arrangement for 29xx160's */
uint amd4112[] = { 0x00000000,		/* 32KB */
		   0x00008000,		/* 8KB */
		   0x0000a000,		/* 8KB */
		   0x0000c000,		/* 16KB */
		   0x00010000
};
uint amd2114[] = { 0x00000000,		/* 16KB */
		   0x00004000,		/* 8KB */
		   0x00006000,		/* 8KB */
		   0x00008000,		/* 32KB */
		   0x00010000
};


flash_desc_t flashes[] = {
	{ 0x00b0, 0x00d0, 0x0200000, 2,	SCS, 0x10000, 32,  0, 31,  0, NULL,    "Intel 28F160S3/5 1Mx16" },
	{ 0x00b0, 0x00d4, 0x0400000, 2,	SCS, 0x10000, 64,  0, 63,  0, NULL,    "Intel 28F320S3/5 2Mx16" },
	{ 0x0089, 0x8890, 0x0200000, 2,	BSC, 0x10000, 32,  0, 30,  8, blk8x8k, "Intel 28F160B3 1Mx16 TopB" },
	{ 0x0089, 0x8891, 0x0200000, 2,	BSC, 0x10000, 32,  1, 31,  8, blk8x8k, "Intel 28F160B3 1Mx16 BotB" },
	{ 0x0089, 0x8896, 0x0400000, 2,	BSC, 0x10000, 64,  0, 62,  8, blk8x8k, "Intel 28F320B3 2Mx16 TopB" },
	{ 0x0089, 0x8897, 0x0400000, 2,	BSC, 0x10000, 64,  1, 63,  8, blk8x8k, "Intel 28F320B3 2Mx16 BotB" },
	{ 0x0089, 0x8898, 0x0800000, 2,	BSC, 0x10000, 128, 0, 126, 8, blk8x8k, "Intel 28F640B3 4Mx16 TopB" },
	{ 0x0089, 0x8899, 0x0800000, 2,	BSC, 0x10000, 128, 1, 127, 8, blk8x8k, "Intel 28F640B3 4Mx16 BotB" },
	{ 0x0089, 0x88C2, 0x0200000, 2,	BSC, 0x10000, 32,  0, 30,  8, blk8x8k, "Intel 28F160C3 1Mx16 TopB" },
	{ 0x0089, 0x88C3, 0x0200000, 2,	BSC, 0x10000, 32,  1, 31,  8, blk8x8k, "Intel 28F160C3 1Mx16 BotB" },
	{ 0x0089, 0x88C4, 0x0400000, 2,	BSC, 0x10000, 64,  0, 62,  8, blk8x8k, "Intel 28F320C3 2Mx16 TopB" },
	{ 0x0089, 0x88C5, 0x0400000, 2,	BSC, 0x10000, 64,  1, 63,  8, blk8x8k, "Intel 28F320C3 2Mx16 BotB" },
	{ 0x0089, 0x88CC, 0x0800000, 2,	BSC, 0x10000, 128, 0, 126, 8, blk8x8k, "Intel 28F640C3 4Mx16 TopB" },
	{ 0x0089, 0x88CD, 0x0800000, 2,	BSC, 0x10000, 128, 1, 127, 8, blk8x8k, "Intel 28F640C3 4Mx16 BotB" },
	{ 0x0089, 0x0014, 0x0400000, 2,	SCS, 0x20000, 32,  0, 31,  0, NULL,    "Intel 28F320J5 2Mx16" },
	{ 0x0089, 0x0015, 0x0800000, 2,	SCS, 0x20000, 64,  0, 63,  0, NULL,    "Intel 28F640J5 4Mx16" },
	{ 0x0089, 0x0016, 0x0400000, 2,	SCS, 0x20000, 32,  0, 31,  0, NULL,    "Intel 28F320J3 2Mx16" },
	{ 0x0089, 0x0017, 0x0800000, 2,	SCS, 0x20000, 64,  0, 63,  0, NULL,    "Intel 28F640J3 4Mx16" },
	{ 0x0089, 0x0018, 0x1000000, 2,	SCS, 0x20000, 128, 0, 127, 0, NULL,    "Intel 28F128J3 8Mx16" },
	{ 0x00b0, 0x00e3, 0x0400000, 2,	BSC, 0x10000, 64,  1, 63,  8, blk8x8k, "Sharp 28F320BJE 2Mx16 BotB" },
	{ 0x0001, 0x224a, 0x0100000, 2,	AMD, 0x10000, 16,  0, 13,  8, amd800,  "AMD 29DL800BT 512Kx16 TopB" },
	{ 0x0001, 0x22cb, 0x0100000, 2,	AMD, 0x10000, 16,  2, 15,  8, amd800,  "AMD 29DL800BB 512Kx16 BotB" },
	{ 0x0001, 0x22c4, 0x0200000, 2,	AMD, 0x10000, 32,  0, 30,  4, amd2114, "AMD 29lv160DT 1Mx16 TopB" },
	{ 0x0001, 0x2249, 0x0200000, 2,	AMD, 0x10000, 32,  1, 31,  4, amd4112, "AMD 29lv160DB 1Mx16 BotB" },
	{ 0x0001, 0x22f6, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  8, blk8x8k, "AMD 29lv320DT 2Mx16 TopB" },
	{ 0x0001, 0x22f9, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  8, blk8x8k, "AMD 29lv320DB 2Mx16 BotB" },
	{ 0x0020, 0x22CA, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  4, amd4112, "ST 29w320DT 2Mx16 TopB" },
	{ 0x0020, 0x22CB, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  4, amd2114, "ST 29w320DB 2Mx16 BotB" },
	{ 0x00C2, 0x00A7, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  4, amd4112, "MX29LV320T 2Mx16 TopB" },
	{ 0x00C2, 0x00A8, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  4, amd2114, "MX29LV320B 2Mx16 BotB" },
	{ 0x0004, 0x22F6, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  4, amd4112, "MBM29LV320TE 2Mx16 TopB" },
	{ 0x0004, 0x22F9, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  4, amd2114, "MBM29LV320BE 2Mx16 BotB" },
        { 0x0001, 0x227E, 0x0400000, 2, AMD, 0x10000, 64,  0, 62,  8, blk8x8k, "AMD 29lv320MT 2Mx16 TopB" },
        //{ 0x0001, 0x2201, 0x0400000, 2,       AMD, 0x10000, 64,  0, 62,  8, blk8x8k, "AMD 29lv320MT 2Mx16 TopB" },
        { 0x0001, 0x2200, 0x0400000, 2, AMD, 0x10000, 64,  1, 63,  8, blk8x8k, "AMD 29lv320MB 2Mx16 BotB" },
	{ 0x0098, 0x009A, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  4, amd4112, "TC58FVT321 2Mx16 TopB" },
	{ 0x0098, 0x009C, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  4, amd2114, "TC58FVB321 2Mx16 BotB" }, 
	{ 0x00C2, 0x22A7, 0x0400000, 2,	AMD, 0x10000, 64,  0, 62,  4, amd4112, "MX29LV320T 2Mx16 TopB" },
	{ 0x00C2, 0x22A8, 0x0400000, 2,	AMD, 0x10000, 64,  1, 63,  4, amd2114, "MX29LV320B 2Mx16 BotB" },
	{ 0x00BF, 0x2783, 0x0400000, 2,	SST, 0x10000, 64,  0, 63,  0, NULL,    "SST39VF320 2Mx16" },
	{ 0,      0,      0,         0,	OLD, 0,       0,   0, 0,   0, NULL,    NULL },
};


int
flash_init(void* base_addr, char *flash_str)
{
	uint16 flash_vendid = 0;
	uint16 flash_devid = 0;
	uint16* flash = (uint16*)base_addr;
	int idx;
	
	flashutl_base = (uint8*)base_addr;


	/* 
	 * Some flashes have different unlock addresses, try each it turn
	 */
	idx = sizeof(flash_cmds)/sizeof(flash_cmds_t) - 2;
	flashutl_cmd = &flash_cmds[idx--];
	//flashutl_cmd = &flash_cmds[1];

	while(flashutl_cmd->type) {

		if (flashutl_cmd->read_id)
			cmd(flashutl_cmd->read_id, CMD_ADDR);

#ifdef MIPSEB
		flash_vendid = *(flash + 1);
		flash_devid = *flash;	
#else
		flash_vendid = *flash;
		flash_devid = *(flash + 1);
#endif

		flashutl_desc = flashes;
		while (flashutl_desc->mfgid != 0 &&
			   !(flashutl_desc->mfgid == flash_vendid &&
			 flashutl_desc->devid == flash_devid)) {
			flashutl_desc++;
		}
		if (flashutl_desc->mfgid != 0)
			break;

		flashutl_cmd = &flash_cmds[idx--];
	}

	if (flashutl_desc->mfgid == 0) {
		flashutl_desc = NULL;
		flashutl_cmd = NULL;
	} else {
		flashutl_cmd = flash_cmds;
		while (flashutl_cmd->type != 0 && flashutl_cmd->type != flashutl_desc->type)
			flashutl_cmd++;
		if (flashutl_cmd->type == 0)
			flashutl_cmd = NULL;
	}

	if (flashutl_cmd != NULL) {
		flash_reset();
	}

	if (flashutl_desc == NULL) {
		if (flash_str)
			strcpy(flash_str, "UNKNOWN FLASH");
		DPRINT(("Flash type UNKNOWN\n"));
		return 1;
	}
	
	if (flash_str)
		strcpy(flash_str, flashutl_desc->desc);

	return 0;
}
int FlashWriteRange(unsigned short *dst, unsigned short *src, unsigned int numbytes)
{
	unsigned long off = (unsigned long)dst - (unsigned long)(CS0_BASE);
	int err;
	err = flash_write(off, src, numbytes);
	if(err)
		DPRINT("Flash write failed\n");
	else
		DPRINT("Flash write succeeded\n");
	return err;
}
	
int
flash_write(unsigned long off, uint16 *src, uint nbytes)
{
	uint16* dest;
	uint16 st, data;
	uint i, len;

	unsigned long a;

	a = (unsigned long)off;
	if (a >= flashutl_desc->size)
	           return 1;

	a = block(a, BLOCK_BASE);

	/* Ensure blocks are unlocked (for intel chips) */
	if (flashutl_cmd->type == BSC) {
	      scmd( INTEL_UNLOCK1, a);
	      scmd( INTEL_UNLOCK2, a);
	}
	
	 
	dprintk("flash_write(%lx,%x)\n",off,nbytes);

	ASSERT(flashutl_desc != NULL);

	if (off >= flashutl_desc->size)
		return 1;

	dest = (uint16*)FLASH_ADDR(off);
	st = 0;

	dprintk("\tflash type=[%x] write_buf=[%x] (off & (WBUFSIZE - 1))=[%lx]\n", flashutl_desc->type,flashutl_cmd->write_buf,(off & (WBUFSIZE - 1)));

	while (nbytes) {
		dprintk("\tnbytes=[%d]\n",nbytes);
		if ((flashutl_desc->type == SCS) &&
		    flashutl_cmd->write_buf &&
		    ((off & (WBUFSIZE - 1)) == 0)) {
			/* issue write command */
			dprintk("\tSCS\n");
			if (flashutl_cmd->write_buf)
				cmd(flashutl_cmd->write_buf, off);
			if ((st = flash_poll(off, DONE)))
				continue;

			len = MIN(nbytes, WBUFSIZE);

#ifndef MIPSEB
                        /* write (length - 1) */
			cmd((len / 2) - 1, off);

			/* write data */
			for (i = 0; i < len; i += 2, dest++, src++)
				*dest = *src;
#else
			/* 
			 * BCM4710 endianness is word consistent but
			 * byte/short scrambled. This write buffer
			 * mechanism appears to be sensitive to the
			 * order of the addresses hence we need to
			 * unscramble them. We may also need to pad
			 * the source with two bytes of 0xffff in case
			 * an odd number of shorts are presented.
			 */

			/* write (padded length - 1) */
			cmd((ROUNDUP(len, 4) / 2) - 1, off);

			/* write data (plus pad if necessary) */
			for (i = 0; i < ROUNDUP(len, 4); i += 4, dest += 2, src += 2) {
				dprintk("\ti=%d\n",i);
				*(dest + 1) = ((i + 2) < len) ? *(src + 1) : 0xffff;
				dprintk("\t\t*(dest + 1)=[%c]\n",*(dest + 1));
				*dest = *src;
				dprintk("\t\t*dest=[%c]\n",*dest);
			}
#endif

			if (flashutl_cmd->confirm)
				cmd(flashutl_cmd->confirm, off);

			if ((st = flash_poll(off, DONE)))
				break;
		} else {
			dprintk("\tnot SCS\n");
			 /* issue write command */
			if (flashutl_cmd->write_word)
				cmd(flashutl_cmd->write_word, CMD_ADDR);

                        /* write data */
			len = MIN(nbytes, 2);
			data = *src++;
			*dest++ = data;

			/* poll for done */
			if ((st = flash_poll(off, data)))
				break;
		}

		nbytes -= len;
		off += len;
	}

	flash_reset();

	return st;
}
int flash_eraseblk(unsigned long addr)
{
	unsigned long a;
	uint16 st;
	a = (unsigned long)addr;
	if(a >= flashutl_desc->size)
		return 1;
	a = block(a, BLOCK_BASE);
	if(flashutl_cmd->type == BSC)
	{
		scmd((unsigned char)INTEL_UNLOCK1, a);
		scmd((unsigned char)INTEL_UNLOCK2, a);
	}

	if(flashutl_cmd->pre_erase)
		cmd(flashutl_cmd->pre_erase, CMD_ADDR);
	if(flashutl_cmd->erase_block)
		cmd(flashutl_cmd->erase_block, a);
	if(flashutl_cmd->confirm)
		scmd(flashutl_cmd->confirm, a);

	st = flash_poll(a, 0xffff);
	flash_reset();
	if(st)
	{
		DPRINT(("Erase of block 0x%08lx - 0x%08lx failed\n",
			a, block((unsigned long)addr, BLOCK_LIM)));
		return st;
	}
	DPRINT(("Erase of block 0x%08lx - 0x%08lx done\n",
			a, block((unsigned long)addr, BLOCK_LIM)));

	return 0;

}
/* Writes a single command to the flash. */
static void
scmd(uint16 cmd, unsigned long off)
{
	dprintk("scmd(%x,%lx)\n",cmd,off);
	ASSERT(flashutl_base != NULL);
	
	/*  cmd |= cmd << 8; */

	*(uint16*)(flashutl_base + off) = cmd;
}

/* Writes a command to flash, performing an unlock if needed. */
static void
cmd(uint16 cmd, unsigned long off)
{
	int i;
	unlock_cmd_t *ul=NULL;
	unsigned long cmd_off;

	dprintk("cmd(%x,%lx)\n",cmd,off);
	dprintk("\tflashutl_cmd->need_unlock=[%x]\n",flashutl_cmd->need_unlock);


	ASSERT(flashutl_cmd != NULL);

	switch (flashutl_cmd->type) {
	case AMD:
		ul = &unlock_cmd_amd;
		cmd_off = AMD_CMD;
		break;
	case SST:
		ul = &unlock_cmd_sst;
		cmd_off = SST_CMD;
		break;
	default:
		cmd_off = 0;
		break;
	}
	
	if (flashutl_cmd->need_unlock) {
		for (i = 0; i < UNLOCK_CMD_WORDS; i++)
			*(uint16*)(flashutl_base + ul->addr[i]) = ul->cmd[i];
	}
	
	/* cmd |= cmd << 8; */

	if (off == CMD_ADDR) 
		off = cmd_off;
#ifdef MIPSEB
	off ^= 2;
#endif
	
	*(uint16*)(flashutl_base + off) = cmd;

}

static void
flash_reset(void) 
{
	dprintk("flash_reset\n");
	dprintk("\tflashutl_cmd->clear_csr=[%x] flashutl_cmd->read_array=[%x]\n",flashutl_cmd->clear_csr,flashutl_cmd->read_array);
	ASSERT(flashutl_desc != NULL);

	if (flashutl_cmd->clear_csr)
		scmd(flashutl_cmd->clear_csr, 0);
	if (flashutl_cmd->read_array)
		scmd(flashutl_cmd->read_array, 0);
}
static int
flash_poll(unsigned long off, uint16 data)
{
	volatile uint16* addr;
	int cnt = FLASH_TRIES;
	uint16 st;

	dprintk("flash_poll(%lx,%x)\n",off,data);
	dprintk("\tflashutl_desc->type=[%x]\n",flashutl_desc->type);

	ASSERT(flashutl_desc != NULL);
	
	if (flashutl_desc->type == AMD || flashutl_desc->type == SST) {
		/* AMD style poll checkes the address being written */
		addr = (volatile uint16*)FLASH_ADDR(off);
		while ((st = *addr) != data && cnt != 0)
			cnt--;
		if (cnt == 0) {
			DPRINT(("flash_poll: timeout, read 0x%x, expected 0x%x\n", st, data));
			return -1;
		}
	} else {
		/* INTEL style poll is at second word of the block being written */
		addr = (volatile uint16*)FLASH_ADDR(block(off, BLOCK_BASE));
		addr++;
		while (((st = *addr) & DONE) == 0 && cnt != 0)
			cnt--;
		if (cnt == 0) {
			DPRINT(("flash_poll: timeout, error status = 0x%x\n", st));
			return -1;
		}
	}
	
	return 0;
}

static unsigned long
block(unsigned long addr, int which)
{
	unsigned long b, l, sb;
	uint* sblocks;
	int i;
	dprintk("block(%lx,%d)\n",addr,which);
	dprintk("\tflashutl_desc->bsize=[%x]\n",flashutl_desc->bsize);
	
	ASSERT(flashutl_desc != NULL);
	ASSERT(addr < (unsigned long)flashutl_desc->size);
	
	b = addr / flashutl_desc->bsize;
	dprintk("\tb=[%lx]\n",b);
	/* check for an address a full size block */
	if (b >= flashutl_desc->ff && b <= flashutl_desc->lf) {
		if (which == BLOCK_LIM) b++;
		return (b * flashutl_desc->bsize);
	}

        /* search for the sub-block */
	if (flashutl_desc->ff == 0) {
		/* sub blocks are at the end of the flash */
		sb = flashutl_desc->bsize * (flashutl_desc->lf + 1);
	} else {
		/* sub blocks are at the start of the flash */
		sb = 0;
	}

	sblocks = flashutl_desc->subblocks;
	for (i = 0; i < flashutl_desc->nsub; i++) {
		b = sb + sblocks[i];
		l = sb + sblocks[i+1];
		if (addr >= b && addr < l) {
			if (which == BLOCK_BASE)
				return b;
			else
				return l;
		}
	}

	ASSERT(1);
	return 0;
}

#endif

#endif //wwzh


int FWBValid(bit32u base)
{
	base&=0x1fffffff;
	base|=0xa0000000;

	if (base < (CS0_BASE + MIN_BOOT_BLOCK_SIZE))		
		return(0);
	
	if (base >= (CS0_BASE + CS0_SIZE))
		return(0);

	return (1);
}
#if 0
int FWBUnLock(bit32u adr,bit32 size)
  {
  bit32u *uip,uitmp;
  int sts;

  uip=(bit32u *)adr;
  uitmp=*uip;
  MEM_WRITE(adr,0x00600060);
  MEM_WRITE(adr,0x00d000d0);
  sts=_FlashOperationComplete(adr,uitmp,700000*FLASH_MAX_WAIT);
  _FlashErrorOut("UnLock Block",sts);
  return(sts==0);
  }
#else
#define FLASH_READ_CMD		0x00FF
#define FLASH_LOCK_CMD		0x0060
#define FLASH_UNLOCK		0x00D0

bit32u FWBBaseUnLockAdr;
void UnLock(bit32u adr)
{
	FWBBaseUnLockAdr=adr;
	MEM_WRITE16(adr,FLASH_LOCK_CMD);
	MEM_WRITE16(adr,FLASH_UNLOCK);
	MEM_WRITE16(adr,FLASH_READ_CMD);
}


int FWBUnLock(bit32u adr,bit32 size)
{	
    bit32u i,sts,blocksize,from,to;


    if (size==0) 
	return(0);

    size--;
    from=adr;
    blocksize=FWBGetBlockSize(from);
    if (blocksize==0)
    {
	sys_printf("Invalid flash address %x\n",from);
	return(0);
    }
    from&=~(blocksize-1);
    to=adr+size;
    blocksize=FWBGetBlockSize(to);
    if (blocksize==0)
    {
	sys_printf("invalid flash address %x\n",to);
	return(0);
    }
    to |= (blocksize-1);

    sys_printf("FlashUnlockBlock(%08x,%08x);\n",from,to);
    for(i=from,sts=0;(i<to)&&(!sts);)
    {
	UnLock(i);
	//Lock();

	blocksize=FWBGetBlockSize(i);
	i+=blocksize;
	if (blocksize==0)
	{
	    sys_printf("invalid flash address %x\n",i);
	    return(0);
	}
    }
    return(sts==0);
}


#endif
/*Returns TRUE for success and FALSE for failure */

int FWBLock(bit32u from,bit32 size)
  {
  bit32u *uip,uitmp;
  int sts;
  bit32u ad,to,blocksize;

  if (size==0) return(TRUE);
  to=from+size;
  blocksize=FWBGetBlockSize(from);
  from&=~(blocksize-1);
  for(sts=0,ad=from;(ad<to)&&(!sts)&&(blocksize);ad+=blocksize)
    {
    uip=(bit32u *)ad;
    uitmp=*uip;
    MEM_WRITE(ad,0x00600060);
    MEM_WRITE(ad,0x00010001);
    sts&=_FlashOperationComplete(ad,uitmp,650*FLASH_MAX_WAIT);
    _FlashErrorOut("Lock Block",sts);
    blocksize=FWBGetBlockSize(ad);
    }
  return(sts==0);
  }

int FWBGetBlockSize(bit32u base)
  {
  int itmp;

  base&=0x1fffffff;
  base|=0xa0000000;
#if 0
#ifdef _AVALANCHE_    /*EVM*/
  if ((base>=CS3_BASE)&&(base<(CS3_BASE+CS3_SIZE)))
    {
    itmp=(128*1024);   /*Strata flash 8/16 bit*/
    if (EMIF_ASYNC_CS3&0x2){
      itmp*=2;              /*Dual devices*/
    } 
      return(itmp);
    }
#endif
#endif
  if ((base>=CS0_BASE)&&(base<(CS0_BASE+CS0_SIZE)))
    {
    	if(flashutl_cmd->type == BSC)
    	{
    		itmp=(64*1024);   		/* Strata flash 8/16 bit */
		if (base<(CS0_BASE+(64*1024)))  /* Bot Boot Block */
	    		itmp=(8*1024);
    	}
    	else if(flashutl_cmd->type == SCS)

    		itmp=(128*1024);   /*Strata flash 8/16 bit*/
  //  if (EMIF_ASYNC_CS0&0x2)
    //  itmp*=2;              /*Dual devices*/
    return(itmp);
    }
  return(0);
  }



/* This function returns back the next block/sector in flash */
bit32u FWBGetNextBlock(bit32u base)
{
	bit32u current_blk_sz;
	bit32u new_blk;

  	base&=0x1fffffff;
  	base|=0xa0000000;

	current_blk_sz = FWBGetBlockSize(base);
	
	if(current_blk_sz == 0)
	{
		return(0);
	}

	base &= ~(current_blk_sz-1);		
	new_blk = base + current_blk_sz;

	return(new_blk);
}


int FWBErase(bit32u base,int size,int verbose)
  {
  bit32u i,sts,blocksize,from,to;

  FWBopen=FALSE;

  if (size==0) 
    return(0);
  size--;

  from=base;
  blocksize=FWBGetBlockSize(from);
  if (blocksize==0)
    {
    sys_printf("Invalid flash address %x\n",from);
    return(0);
    }
  from&=~(blocksize-1);

  to=base+size;
  blocksize=FWBGetBlockSize(to);
  if (blocksize==0)
    {
    sys_printf("invalid flash address %x\n",to);
    return(0);
    }
  to  |= (blocksize-1);

  if (verbose) sys_printf("FlashEraseBlock(%08x,%08x);\n",from,to);
  for(i=from,sts=0;(i<to)&&(!sts);)
    {
#if 0
    MEM_WRITE(i,0x00200020);
    MEM_WRITE(i,0x00d000d0);
    sts=_FlashOperationComplete(i,0xffffffff,FLASH_MAX_WAIT*5000000);
    _FlashErrorOut("Erase Block",sts);
#else
    sts = flash_eraseblk(i - CS0_BASE);
#endif
    if (verbose) sys_printf(".");
    blocksize=FWBGetBlockSize(i);
    i+=blocksize;
    if (blocksize==0)
      {
      if (verbose) sys_printf("invalid flash address %x\n",i);
      return(0);
      }
    }
  return(sts==0);
  }

int FWBOpen(bit32u base)
  {
  int i;

  FWBopen=FALSE;
  if (FWBGetBlockSize(base)==0) 
    return(FALSE);
  for(i=0;i<64;i++) 
    FWBwdata[i]=0xff;
  FWBBaseAdr=0xffffffff;
  FWBvalid=0;
  FWBopen=TRUE;
  return(TRUE);
  }

int FWBWriteByte(bit32u adr, bit8 cVal)
  {
  int sts,i;
  bit32u *isp,*idp;

  if (!FWBopen) 
    return(FALSE);
  sts=TRUE;
  if((adr&(~0x3f))!=FWBBaseAdr)
    sts=FWBClose();
  if (FWBvalid==0)
    {
    for(i=0,isp=(bit32u *)(adr&~0x3f),idp=(bit32u *)FWBwdata;i<16;i++)
      *idp++=*isp++;
    }
  /*sys_printf("FWBWriteByte(%x,%x);\n",adr,cVal&0x0ff); */
  FWBBaseAdr=adr&(~0x3f);
  i=adr&0x3f;
  FWBvalid++;
  FWBwdata[i]=cVal;
  return(sts);
  }

int FWBClose(void)
  {
  int sts,i;

  if (!FWBopen) 
    return(FALSE);
  if (FWBvalid==0) 
    return(TRUE);
  for(i=0,sts=1;(sts)&&(i<5);i++)
    {
    sts=_FWBWriteBlock();
    if (sts)
      {
      MEM_WRITE(FWBBaseAdr,0x00ff00ff);
      MEM_WRITE(FWBBaseAdr,0x00500050);
      }
    }

  _FlashErrorOut("Program Block",sts);

  for(i=0;i<64;i++) 
    FWBwdata[i]=0xff;
  FWBBaseAdr=0xffffffff;
  FWBvalid=0;
  return(sts==0);
  }

/*Used to decode Flash error values */

void _FlashErrorOut(char *str,int sts)
  {

  if (sts)
    sys_printf("Flash '%s' operation Failed: sts=%08x\n",str,sts);
  }

/*Returns 0 for success and !=0 for error */
#if 0
int _FWBWriteBlock(void)
  {
  int icount,i,done;
  bit32u *uip,uitmp;

  MEM_WRITE(FWBBaseAdr,0x00e800e8);
  icount=0;
  do
    {
    uitmp=MEM_READ(FWBBaseAdr);
    done = ((uitmp&0x00800080)==0x00800080);
    if ((!done)&&(icount>(FLASH_MAX_WAIT*650)))
      return(0xeeee0002);
    icount++;
    }while(!done);
  MEM_WRITE(FWBBaseAdr,0x000f000f);
  for(uip=(bit32u *)FWBwdata,i=0;i<16;i++,uip++)
    MEM_WRITE(FWBBaseAdr+(i*4),*uip);
  MEM_WRITE(FWBBaseAdr,0x00d000d0);
  uip=(bit32u *)FWBwdata;
  return(_FlashOperationComplete(FWBBaseAdr,*uip,FLASH_MAX_WAIT*650));
  }
#else


int _FWBWriteBlock(void)
{
	return flash_write((FWBBaseAdr - CS0_BASE), (uint16 *)FWBwdata, 64);
}


#endif
/*Returns 0 for success and !=0 for failure */

int _FlashOperationComplete(bit32u adr,bit32u dat,int timeoutloops)
  {
  bit32u sts,fnd;
  int icount;
  int err;

  icount=0;
  do
    {
    if (++icount>timeoutloops)
      {
      MEM_WRITE(adr,0x00ff00ff);    /*Reset to read mode */
      return(0xeeee0001);
      }
    sts=MEM_READ(adr);
    if ((sts&0x00800080)==0x00800080)
      {
      err = (sts&0x00300030);
      MEM_WRITE(adr,0x00ff00ff);    /*Reset to read mode */
      if (!err)
        {
        fnd=MEM_READ(adr);
        if (fnd==dat) 
          return(0);
        }
       else
        MEM_WRITE(adr,0x00500050);  /*Reset error bits */
      return(sts);
      }
    }while(1);
  }

