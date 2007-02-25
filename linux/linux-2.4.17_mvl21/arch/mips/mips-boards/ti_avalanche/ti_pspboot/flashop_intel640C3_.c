/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*   Copyright (C) 1999-2003 by Texas Instruments, Inc.  All rights reserved.  */
/*   Copyright (C) 2001-2003 Telogy Networks.	    						   */
/*                                                                             */
/*   NOTE: THIS VERSION OF CODE IS MAINTAINED BY TELOGY NETWORKS AND NOT TI!   */
/*                                                                             */
/*     IMPORTANT - READ CAREFULLY BEFORE PROCEEDING TO USE SOFTWARE.           */
/*                                                                             */
/*  This document is displayed for you to read prior to using the software     */
/*  and documentation.  By using the software and documentation, or opening    */
/*  the sealed packet containing the software, or proceeding to download the   */
/*  software from a Bulletin Board System(BBS) or a WEB Server, you agree to   */
/*  abide by the following Texas Instruments License Agreement. If you choose  */ 
/*  not to agree with these provisions, promptly discontinue use of the        */
/*  software and documentation and return the material to the place you        */
/*  obtained it.                                                               */
/*                                                                             */
/*                               *** NOTE ***                                  */
/*                                                                             */
/*  The licensed materials contain MIPS Technologies, Inc. confidential        */
/*  information which is protected by the appropriate MIPS Technologies, Inc.  */
/*  license agreement.  It is your responsibility to comply with these         */
/*  licenses.                                                                  */
/*                                                                             */
/*                   Texas Instruments License Agreement                       */
/*                                                                             */
/*  1. License - Texas Instruments (hereinafter "TI"), grants you a license    */
/*  to use the software program and documentation in this package ("Licensed   */
/*  Materials") for Texas Instruments broadband products.                      */
/*                                                                             */
/*  2. Restrictions - You may not reverse-assemble or reverse-compile the      */
/*  Licensed Materials provided in object code or executable format.  You may  */
/*  not sublicense, transfer, assign, rent, or lease the Licensed Materials    */
/*  or this Agreement without written permission from TI.                      */
/*                                                                             */
/*  3. Copyright - The Licensed Materials are copyrighted. Accordingly, you    */
/*  may either make one copy of the Licensed Materials for backup and/or       */
/*  archival purposes or copy the Licensed Materials to another medium and     */
/*  keep the original Licensed Materials for backup and/or archival purposes.  */
/*                                                                             */
/*  4. Runtime and Applications Software - You may create modified or          */
/*  derivative programs of software identified as Runtime Libraries or         */
/*  Applications Software, which, in source code form, remain subject to this  */
/*  Agreement, but object code versions of such derivative programs are not    */
/*  subject to this Agreement.                                                 */
/*                                                                             */
/*  5. Warranty - TI warrants the media to be free from defects in material    */
/*  and workmanship and that the software will substantially conform to the    */
/*  related documentation for a period of ninety (90) days after the date of   */
/*  your purchase. TI does not warrant that the Licensed Materials will be     */
/*  free from error or will meet your specific requirements.                   */
/*                                                                             */
/*  6. Remedies - If you find defects in the media or that the software does   */
/*  not conform to the enclosed documentation, you may return the Licensed     */
/*  Materials along with the purchase receipt, postage prepaid, to the         */
/*  following address within the warranty period and receive a refund.         */
/*                                                                             */
/*  TEXAS INSTRUMENTS                                                          */
/*  Application Specific Products, MS 8650                                     */
/*  c/o ADAM2 Application Manager                                              */
/*  12500 TI Boulevard                                                         */
/*  Dallas, TX 75243  - U.S.A.                                                 */
/*                                                                             */
/*  7. Limitations - TI makes no warranty or condition, either expressed or    */
/*  implied, including, but not limited to, any implied warranties of          */
/*  merchantability and fitness for a particular purpose, regarding the        */
/*  licensed materials.                                                        */
/*                                                                             */
/*  Neither TI nor any applicable licensor will be liable for any indirect,    */
/*  incidental or consequential damages, including but not limited to loss of  */
/*  profits.                                                                   */
/*                                                                             */
/*  8. Term - The license is effective until terminated.   You may terminate   */
/*  it at any other time by destroying the program together with all copies,   */
/*  modifications and merged portions in any form. It also will terminate if   */
/*  you fail to comply with any term or condition of this Agreement.           */
/*                                                                             */
/*  9. Export Control - The re-export of United States origin software and     */
/*  documentation is subject to the U.S. Export Administration Regulations or  */
/*  your equivalent local regulations. Compliance with such regulations is     */
/*  your responsibility.                                                       */
/*                                                                             */
/*                         *** IMPORTANT NOTICE ***                            */
/*                                                                             */
/*  Texas Instruments (TI) reserves the right to make changes to or to         */
/*  discontinue any semiconductor product or service identified in this        */
/*  publication without notice. TI advises its customers to obtain the latest  */
/*  version of the relevant information to verify, before placing orders,      */
/*  that the information being relied upon is current.                         */
/*                                                                             */
/*  TI warrants performance of its semiconductor products and related          */
/*  software to current specifications in accordance with TI's standard        */
/*  warranty. Testing and other quality control techniques are utilized to     */
/*  the extent TI deems necessary to support this warranty. Unless mandated    */
/*  by government requirements, specific testing of all parameters of each     */
/*  device is not necessarily performed.                                       */
/*                                                                             */
/*  Please be aware that Texas Instruments products are not intended for use   */
/*  in life-support appliances, devices, or systems. Use of a TI product in    */
/*  such applications without the written approval of the appropriate TI       */
/*  officer is prohibited. Certain applications using semiconductor devices    */
/*  may involve potential risks of injury, property damage, or loss of life.   */
/*  In order to minimize these risks, adequate design and operating            */
/*  safeguards should be provided by the customer to minimize inherent or      */
/*  procedural hazards. Inclusion of TI products in such applications is       */
/*  understood to be fully at the risk of the customer using TI devices or     */
/*  systems.                                                                   */
/*                                                                             */
/*  TI assumes no liability for TI applications assistance, customer product   */
/*  design, software performance, or infringement of patents or services       */
/*  described herein. Nor does TI warrant or represent that license, either    */
/*  expressed or implied, is granted under any patent right, copyright, mask   */
/*  work right, or other intellectual property right of TI covering or         */
/*  relating to any combination, machine, or process in which such             */
/*  semiconductor products or services might be or are used.                   */
/*                                                                             */
/*  All company and/or product names are trademarks and/or registered          */
/*  trademarks of their respective manaufacturers.                             */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  Now that you've gotten here you are probably serious about understanding   */
/*  the way Adam2 uses the flash. All flash operations are contained in this   */
/*  module. There are 7 interfaces to this module. FWBGetBlockSize, FWBErase,  */
/*  FWBOpen, FWBWriteByte, and FWBClose, FWBUnLock, FWBLock.                   */                                                                             
/*  FWBGetBlockSize(base);       - returns the Flash Block size for the        */
/*                                 requested region so that all users of the   */
/*                                 flash understand the blocking factor.       */
/*                                                                             */
/*  FWBErase(base,size,verbose); - Erases from base address of size in blocks. */
/*                                                                             */
/*  FWBOpen(Base);               - Will always be called prior to FWBWriteByte,*/
/*                                 and is responsible for unlocking the region */
/*                                 and preparing for flash writes.             */
/*                                                                             */
/*  FWBWriteByte(address,data);  - Is called to write data to the flash        */
/*                                 address, remember that a FWBClose will be   */
/*                                 called prior to any use of the written data.*/
/*                                                                             */
/*  FWBClose()                   - Will complete any pending flash writes to a */
/*                                 region, and lock the flash.                 */
/*                                                                             */
/*  FWBUnLock(base,size)         - Unlocks the blocks specified                */
/*                                                                             */
/*  FWBLock(base,size)           - Lock the blocks specified                   */
/*                                                                             */
/*  The use of FWBOpen, FWBWriteByte, and FWBClose allows for efficient block  */
/*  writes. Remember it is possible to only get One FWBWriteByte in-between a  */
/*  FWBOpen and a FWBClose.                                                    */
/*                                                                             */
/*  During the Boot process the FWBGetBlockSize is called. This block can be   */
/*  called at any time, and returns the size of the flash blocks for the       */
/*  requested region. It is possible to have different size flash blocks in    */
/*  different memory regions.                                                  */
/*                                                                             */
/*  Any time the Kernel wishes to erase a block of flash, it will call the     */
/*  FWBErase routine. The routine is assumed to erase in blocks. That is       */
/*  FWBErase(CS0_BASE,1,0); will erase the block at CS0_BASE. The same can be  */
/*  said for FWBErase(CS0_BASE,FWBGetBlockSize(CS0_BASE),0); So that           */
/*  FWBErase(CS0_BASE,FWBGetBlockSize(CS0_BASE)+1,0); and                      */
/*  FWBErase(CS0_BASE+1,FWBGetBlockSize(CS0_BASE),0); actually erases 2 blocks.*/
/*                                                                             */
/*  It is important to not that the FWBXxxxx routines must always leave the    */
/*  flash in a read mode. Failure to do this will result in a system hang.     */
/*  sys_printf () is also unuasable whilest the flash in not in a read mode    */
/*                                                                             */
/*  And last but not least, all FWB... routines return TRUE if successfull and */
/*  FALSE if unsuccessfull                                                     */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
#include "hw.h"
#include "stddef.h"
#include "flashop.h"
#include "platform.h"

void _FlashErrorOut(char *str,int sts);
int _FWBWriteBlock(void);
int _FlashOperationComplete(bit32u adr, bit16u dat, int timeoutloops);

#define MIN_BOOT_BLOCK_SIZE (64 * 1024)

#define FLASH_MAX_WAIT 		125
#define MAXBUF 				2


#define MEM_READ(addr)         (MCONTENTS_OF(addr))
#define MEM_WRITE(addr,val)    (MCONTENTS_OF(addr) = (val))

#define MEM_WRITE16(addr,val)  ((*((volatile unsigned short *)(addr)))=(val))
#define MEM_READ16(addr)       (*((volatile unsigned short *)(addr)))

/* The following are for the Intel 28FXXXC3 Flash Chips */
#define FLASH_READ_CMD		0x00FF
#define FLASH_READ_CFG_CMD	0x0090
#define FLASH_READ_STAT_CMD	0x0070
#define FLASH_ERROR_MASK	0x003A
#define FLASH_WRITE_CMD		0x0040
#define FLASH_CLEAR_SR_CMD	0x0050
#define FLASH_ERASE_SECTOR	0x0020 /* CMD 1 for erasing a sector */
#define FLASH_LOCK_CMD		0x0060
#define FLASH_UNLOCK		0x00D0
#define FLASH_LOCK			0x0001
#define FLASH_COMMIT_CMD	0x00D0
#define FLASH_WSMS			0x0080 /* Write state machine state */

bit32u FWBBaseAdr, FWBBaseUnLockAdr;
int    FWBvalid;
int    FWBopen=FALSE;
char   FWBwdata[MAXBUF];

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

void UnLock(bit32u adr)
{
	FWBBaseUnLockAdr=adr;
	MEM_WRITE16(adr,FLASH_LOCK_CMD);
	MEM_WRITE16(adr,FLASH_UNLOCK);
	MEM_WRITE16(adr,FLASH_READ_CMD);
}

void Lock(void)
{
	MEM_WRITE16(FWBBaseUnLockAdr,FLASH_LOCK_CMD);
	MEM_WRITE16(FWBBaseUnLockAdr,FLASH_LOCK);
    MEM_WRITE16(FWBBaseUnLockAdr,FLASH_READ_CMD);
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

/* Returns TRUE for success and FALSE for failure */

int FWBLock(bit32u from,bit32 size)
{
  return(TRUE);
}

int FWBGetBlockSize(bit32u base)
{
    int itmp = 0;

    base&=0x1fffffff;
    base|=0xa0000000;
  	
    if ((base>=CS0_BASE)&&(base<(CS0_BASE+CS0_SIZE)))
    {
    	itmp=(64*1024);   		/* Strata flash 8/16 bit */
		if (base<(CS0_BASE+(64*1024)))  /* Bot Boot Block */
	    	itmp=(8*1024);

//	if (base>=((CS0_BASE+CS0_SIZE)-(64*1024)))  /* Top Boot Block */
//	    itmp=(8*1024);
//	if (EMIF_ASYNC_CS0&0x2)
//	    itmp*=2;              /* Dual devices */
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


/* Start at base and erase at least size bytes */
int FWBErase(bit32u base,int size, int verbose)
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
	to |= (blocksize-1);

	if (verbose) sys_printf("FlashEraseBlock(%08x,%08x);\n",from,to);
	for(i=from,sts=0;(i<to)&&(!sts);)
    {
    	UnLock(i);
	    MEM_WRITE16(i,FLASH_ERASE_SECTOR);
    	MEM_WRITE16(i,FLASH_COMMIT_CMD);
	    sts=_FlashOperationComplete(i,0xffff,FLASH_MAX_WAIT*5000000);
    	Lock();
	    _FlashErrorOut("Erase Block",sts);
    	
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
  	for(i=0;i<MAXBUF;i++) 
  	  FWBwdata[i]=(char)0xff;

	FWBBaseAdr=0xffffffff;
	FWBvalid=0;
	FWBopen=TRUE;
  	return(TRUE);
}

int FWBWriteByte(bit32u adr, bit8 cVal)
{
	int sts,i;
	bit16u *isp,*idp;

   	if (!FWBopen) 
		return(FALSE);
	sts=TRUE;
 	if((adr&(~(MAXBUF-1)))!=FWBBaseAdr)
		sts=FWBClose();
	if (FWBvalid==0)
    {
    	for(i=0,isp=(bit16u *)(adr&~(MAXBUF-1)),idp=(bit16u *)FWBwdata;i<(MAXBUF/2);i++)
			*idp++=*isp++;
    }
	/*sys_printf("FWBWriteByte(%x,%x);\n",adr,cVal&0x0ff);                         */
	FWBBaseAdr=adr&(~(MAXBUF-1));
	i=adr&(MAXBUF-1);
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
			MEM_WRITE16(FWBBaseAdr,FLASH_READ_CMD);
			MEM_WRITE16(FWBBaseAdr,FLASH_CLEAR_SR_CMD);
		}
	}
	_FlashErrorOut("Program Block",sts);

	for(i=0;i<MAXBUF;i++) 
    	FWBwdata[i]=(char)0xff;
	FWBBaseAdr=0xffffffff;
	FWBvalid=0;
	return(sts==0);
}

/*Used to decode Flash error values */

void _FlashErrorOut(char *str,int sts)
{
	if (sts)
		sys_printf ("Flash '%s' operation Failed: sts=%08x\n",str,sts);
}

/* Returns 0 for success and !=0 for error */

int _FWBWriteBlock(void)
{
	bit16u *uip ;
	int sts;
	
	UnLock(FWBBaseAdr);

	uip = (bit16u *)FWBwdata;	
	MEM_WRITE16(FWBBaseAdr,FLASH_WRITE_CMD);
	MEM_WRITE16(FWBBaseAdr,*uip);

	Lock();
	sts=_FlashOperationComplete(FWBBaseAdr,*uip,FLASH_MAX_WAIT*650);
	return(sts);
}

/* Returns 0 for success and !=0 for failure */

int _FlashOperationComplete(bit32u adr, bit16u dat,int timeoutloops)
{
	bit16u fnd;
	bit32u sts;
  	int icount = 0;
  	int err;

  	do
 	{
    	if (++icount>timeoutloops)
      	{
      		MEM_WRITE16(adr,FLASH_READ_CMD);    /* Reset to read mode */
      		return(0xeeee0001);
      	}
    	sts=MEM_READ16(adr);
    	if ((sts&FLASH_WSMS)==FLASH_WSMS)
      	{
			/* Check for errors */
			err = (sts&FLASH_ERROR_MASK);
			MEM_WRITE16(adr,FLASH_READ_CMD);    /* Reset to read mode */
			if (!err)
        	{
				fnd=MEM_READ16(adr);
				if (fnd==dat) 
	              return(0);	
    	    }
       		else
				MEM_WRITE16(adr,FLASH_CLEAR_SR_CMD);  /* Reset error bits */
			
        MEM_WRITE16(adr,FLASH_READ_CMD);    /* Reset to read mode */
			return(sts);
		}
    }while(1);
}

