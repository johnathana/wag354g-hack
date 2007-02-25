/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*   Copyright (C) 1999-2002 by Texas Instruments, Inc.  All rights reserved.  */
/*   Copyright (C) 2001-2002 Telogy Networks.	    						   */
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

#ifndef _STANDALONE
#include "platform.h"
#include "flashop.h"
#include "hw.h"
#include "stddef.h"
#include "errno.h"
#include "debug.h"
#else
#include <psbl/stddef.h>
#include <psbl/stdio.h>
#include <psbl/flashop.h>
#include <psbl/hw.h>
#include <psbl/errno.h>
#include <psbl/debug.h>
#endif

void _FlashErrorOut(char *str,int sts);
int _FWBWriteBlock(void);
int _FlashOperationComplete(bit32u adr,bit32u dat,int timeoutloops);

#define MCONTENTS_OF(addr)      (*((volatile unsigned int *)(addr)))
#define MEM_READ(addr)         (MCONTENTS_OF(addr))
#define MEM_WRITE(addr,val)    (MCONTENTS_OF(addr) = (val))


#define FLASH_MAX_WAIT 200

/* The following are for the Intel 28FXXXC3 Flash Chips */
#define FLASH_READ_CMD		0x00FF00FF
#define FLASH_READ_CFG_CMD	0x00900090
#define FLASH_READ_STAT_CMD	0x00700070
#define FLASH_ERROR_MASK	0x001A001A
#define FLASH_WRITE_CMD		0x00400040
#define FLASH_CLEAR_SR_CMD	0x00500050
#define FLASH_ERASE_SECTOR	0x00200020 /* CMD 1 for erasing a sector */
#define FLASH_LOCK_CMD		0x00600060
#define FLASH_UNLOCK		0x00D000D0
#define FLASH_LOCK			0x00010001
#define FLASH_COMMIT_CMD	0x00D000D0
#define FLASH_WSMS			0x00800080 /* Write state machine state */

#define INTEL_FLASH_ID		0x00890089
#define INTEL_FLASH_8K		0x88C088C0
#define INTEL_FLASH_16K		0x88C288C2
#define INTEL_FLASH_32K		0x88C488C4
#define FLASH_SECTOR_SIZE	   (128*1024)	/* How big is a "normal" sector */
#define FLASH_ENV_SECTOR_SIZE  (16*1024)	/* How big is a environment sector */
#define EPLD_FLASH_WP		   0x9C800008	/* EPLD register for Flash write protect */

#ifdef DUAL_FLASH
#define FLASH2_OFFSET       0x0d000000 
#endif

bit32u FWBBaseAdr;
int    FWBvalid;
int    FWBopen=FALSE;
char   FWBwdata[64];
static bit32u flash_size[2]; /* We have at most 2 banks */
bit32u total_flash;

/* This function returns back a 0 if failure - no valid flash detected */
int FWBGet_flash_type()
{
	bit32u	flash_id;
	bit32u  flash_type;
  
	/* Figure out if we have the right flash chips */
	MEM_WRITE(CS0_BASE,FLASH_READ_CMD);
	MEM_WRITE(CS0_BASE,FLASH_READ_CFG_CMD);
	flash_id = MEM_READ(CS0_BASE);
	flash_type = MEM_READ(CS0_BASE+4);
	MEM_WRITE(CS0_BASE,FLASH_READ_CMD);

	if(flash_id != INTEL_FLASH_ID)
	{
		return(0); /* NOT what we expected */
	}
	
	switch(flash_type)
	{
		case INTEL_FLASH_8K:
				flash_size[0] = 2*1024*1024;
				break;
				
		case INTEL_FLASH_16K:
				flash_size[0] = 4*1024*1024;
				break;
				
		case INTEL_FLASH_32K:
				flash_size[0] = 8*1024*1024;
				break;
				
		default: /* Unknown type */
				return(0);				
	}
	
	total_flash = flash_size[0];
		
	/* Check if we have second bank populated */

#ifdef DUAL_FLASH    

	MEM_WRITE(CS0_BASE+FLASH2_OFFSET,FLASH_READ_CMD);
	MEM_WRITE(CS0_BASE+FLASH2_OFFSET,FLASH_READ_CFG_CMD);
	flash_id = MEM_READ(CS0_BASE+FLASH2_OFFSET);
	flash_type = MEM_READ(CS0_BASE+4+FLASH2_OFFSET);
	MEM_WRITE(CS0_BASE+FLASH2_OFFSET,FLASH_READ_CMD);
	
	/* Ok we do have some Intel flash... */
	if(flash_id == INTEL_FLASH_ID)
	{
	
		switch(flash_type)
		{
			case INTEL_FLASH_8K:
					flash_size[1] = 2*1024*1024;
					break;
					
			case INTEL_FLASH_16K:
					flash_size[1] = 4*1024*1024;
					break;
					
			case INTEL_FLASH_32K:
					flash_size[1] = 8*1024*1024;
					break;
					
			default: /* Unknown type */
					flash_size[1] = 0;
					break;
		}		
		total_flash += flash_size[1];			
	}
#endif
	return(total_flash);
}

int FWBUnLock(bit32u adr,bit32 size)
  {
  bit32u *uip,uitmp;
  int sts;

  uip=(bit32u *)adr;
  uitmp=*uip;
  MEM_WRITE(adr,FLASH_LOCK_CMD);
  MEM_WRITE(adr,FLASH_UNLOCK);
  sts=_FlashOperationComplete(adr,uitmp,700000*FLASH_MAX_WAIT);
  _FlashErrorOut("UnLock Block",sts);
  return(sts==0);
  }

/*Returns TRUE for success and FALSE for failure                                  */

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
    MEM_WRITE(ad,FLASH_LOCK_CMD);
    MEM_WRITE(ad,FLASH_LOCK);
    sts&=_FlashOperationComplete(ad,uitmp,650*FLASH_MAX_WAIT);
    _FlashErrorOut("Lock Block",sts);
    blocksize=FWBGetBlockSize(ad);
    }
  return(sts==0);
  }


int FWBGetBlockSize(bit32u base)
{
	int itmp = 0;

  	base&=0x1fffffff;
  	base|=0xa0000000;
	
#ifdef DUAL_FLASH    
	if(base  >= (CS0_BASE+FLASH2_OFFSET)) /* bank 2 */
	{
		base -=	(CS0_BASE+FLASH2_OFFSET);
	    if(base < flash_size[1])
		{
			if( base >= (flash_size[1] -FLASH_SECTOR_SIZE))
			{
				itmp = FLASH_ENV_SECTOR_SIZE;
			}
			else
			{
				itmp = FLASH_SECTOR_SIZE;
			}
		}
	}
#endif
    if ((base >= CS0_BASE) && (base<(CS0_BASE+flash_size[0])))
	{
		base -=	CS0_BASE;
	    if( base >= (flash_size[0] -FLASH_SECTOR_SIZE)) {
		    itmp = FLASH_ENV_SECTOR_SIZE;
	    }
		else{
		    itmp = FLASH_SECTOR_SIZE;
		}
	}
    return(itmp);
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

#ifdef DUAL_FLASH    
	/* Does the new block belong in the next bank? */
	if( (base < (CS0_BASE + flash_size[0]))
		&& (new_blk >= (CS0_BASE + flash_size[0])))
	{
		new_blk = CS0_BASE+FLASH2_OFFSET;			
	}
#endif
	return(new_blk);
}

/* Start at base and erase at least size bytes */
int FWBErase(bit32u base,int size, int verbose)
{
  	bit32u i,sts,blocksize,from;
	bit32u erased_bytes = 0;

  	FWBopen=FALSE;

  	if (size==0) 
    	return(0);
  	size--;

  	from = base;

	/* Check if we have a valid flash address */
  	blocksize = FWBGetBlockSize(base);
  
  	if ( (FWBGetBlockSize(from)==0)  || (blocksize == 0) )
  	{
    	sys_printf ("Invalid flash address %x\n",from);
    	return(0);
    }

	/*MEM_WRITE(EPLD_FLASH_WP,1);*/ /* Enable write on EPLD */

  	from&=~(blocksize-1);

  	if (verbose) sys_printf ("FlashEraseBlock(%08x);\n",from);

  	for(i=from,sts=0;(erased_bytes <size)&&(!sts);)
    {
		MEM_WRITE(i,FLASH_LOCK_CMD);
		MEM_WRITE(i,FLASH_UNLOCK);
		MEM_WRITE(i,FLASH_READ_CMD);
    	MEM_WRITE(i,FLASH_ERASE_SECTOR);
    	MEM_WRITE(i,FLASH_COMMIT_CMD);
  		MEM_WRITE(i,FLASH_READ_STAT_CMD);

    	sts = _FlashOperationComplete(i,0xffffffff,FLASH_MAX_WAIT*5000000);

    	_FlashErrorOut("Erase Block",sts);

    	/*sys_printf ("%08x\n",i);*/
		if (verbose) sys_printf (".");

		MEM_WRITE(i,FLASH_READ_CMD);
		erased_bytes += FWBGetBlockSize(i);
    	i=FWBGetNextBlock(i);

    	if (i == 0)
      	{
      		  if (verbose) sys_printf ("invalid flash address %x\n",i);
			/*MEM_WRITE(EPLD_FLASH_WP,0);*/
      		return(0);
      	}
    }
	/*MEM_WRITE(EPLD_FLASH_WP,0);*/
  	return(sts==0);
}

int FWBOpen(bit32u base)
  {
  int i;

  FWBopen=FALSE;
  if (FWBGetBlockSize(base)==0) 
    return(FALSE);
  for(i=0;i<64;i++) 
    FWBwdata[i]=(char)0xff;
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
  /*sys_printf ("FWBWriteByte(%x,%x);\n",adr,cVal&0x0ff);                         */
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
      MEM_WRITE(FWBBaseAdr,FLASH_READ_CMD);
      MEM_WRITE(FWBBaseAdr,FLASH_CLEAR_SR_CMD);
      }
    }

  _FlashErrorOut("Program Block",sts);

  for(i=0;i<64;i++) 
    FWBwdata[i]=(char)0xff;
  FWBBaseAdr=0xffffffff;
  FWBvalid=0;
  return(sts==0);
  }

/*Used to decode Flash error values                                          */

void _FlashErrorOut(char *str,int sts)
  {

  if (sts)
    sys_printf ("Flash '%s' operation Failed: sts=%08x\n",str,sts);
  }

/*Returns 0 for success and !=0 for error                                    */

int _FWBWriteBlock(void)
  {
  int icount,i,done;
  bit32u *uip,uitmp;

  /*MEM_WRITE(EPLD_FLASH_WP,1);*/ /* Enable write on EPLD */
  MEM_WRITE(FWBBaseAdr,FLASH_LOCK_CMD);
  MEM_WRITE(FWBBaseAdr,FLASH_UNLOCK);
	
	for(uip=(bit32u *)FWBwdata,i=0;i<16;i++,uip++)
	{
		/* wait until the WSMS state is ready for more data */
	  	icount=0;
		do
	    {
    		uitmp=MEM_READ(FWBBaseAdr);
    		done = ((uitmp&FLASH_WSMS)==FLASH_WSMS);
    		if ((!done)&&(icount>(FLASH_MAX_WAIT*650)))
      		return(0xeeee0002);
    		icount++;
    	}while(!done);
		
	  	MEM_WRITE(FWBBaseAdr+(i*4),FLASH_WRITE_CMD);
    	MEM_WRITE(FWBBaseAdr+(i*4),*uip);
	}
  	icount=0;
	do
    {
   		uitmp=MEM_READ(FWBBaseAdr);
   		done = ((uitmp&FLASH_WSMS)==FLASH_WSMS);
   		if ((!done)&&(icount>(FLASH_MAX_WAIT*650)))
     		return(0xeeee0002);
    		icount++;
   	}while(!done);
  	uip=(bit32u *)FWBwdata;
  	MEM_WRITE(FWBBaseAdr,FLASH_READ_CMD);
  	MEM_WRITE(FWBBaseAdr,FLASH_LOCK_CMD);
  	MEM_WRITE(FWBBaseAdr,FLASH_LOCK);
  MEM_WRITE(FWBBaseAdr,FLASH_READ_STAT_CMD);
  /*MEM_WRITE(EPLD_FLASH_WP,0);*/ /* Disable write on EPLD */
  return(_FlashOperationComplete(FWBBaseAdr,*uip,FLASH_MAX_WAIT*650));
  }

/*Returns 0 for success and !=0 for failure                                  */

int _FlashOperationComplete(bit32u adr,bit32u dat,int timeoutloops)
{
  	bit32u sts,fnd;
  	int icount=0;
  	int err;
	int i;

  	do
 	{
    	if (++icount>timeoutloops)
      	{
      		MEM_WRITE(adr,FLASH_READ_CMD);    /*Reset to read mode      */
      		return(0xeeee0001);
      	}
    	sts=MEM_READ(adr);
    	if ((sts&FLASH_WSMS)==FLASH_WSMS)
      	{
	  		/* Check for errors */
	      	err = (sts&FLASH_ERROR_MASK);
          	MEM_WRITE(adr,FLASH_READ_CMD);    /*Reset to read mode       */

      		if (!err)
        	{
				for(i= 0; i < 20; i++)
        		{
        			fnd=MEM_READ(adr);
        			if (fnd==dat) 
          				return(0);	
        		}
        	}
       		else
        		MEM_WRITE(adr,FLASH_CLEAR_SR_CMD);  /*Reset error bits     */
        MEM_WRITE(adr,FLASH_READ_CMD);    /*Reset to read mode           */
      	return(sts);
      }
    }while(1);
  }

