/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2005 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifdef _STANDALONE
#include <psbl/stddef.h>
#include <psbl/stdio.h>
#include <psbl/flashop.h>
#include <psbl/hw.h>
#else 
#include "platform.h"
#include "stddef.h"
#include "flashop.h"
#include "hw.h"
#endif

void _FlashErrorOut(char *str,int sts);
int _FWBWriteBlock(void);
int _FlashOperationComplete(bit32u adr,bit16u dat,int timeoutloops);

#define MCONTENTS_OF(addr)      (*((volatile bit16u *)(addr)))
#define MEM_READ(addr)         (MCONTENTS_OF(addr))
#define MEM_WRITE(addr,val)    (MCONTENTS_OF(addr) = (val))

#define FLASH_MAX_WAIT 125

bit32u FWBBaseAdr;
int    FWBvalid;
int    FWBopen=FALSE;
char   FWBwdata[32];


int FWBUnLock(bit32u adr,bit32 size)
  {
  bit16u *usp,ustmp;
  int sts;

  usp=(bit16u *)adr;
  ustmp=*usp;
  MEM_WRITE(adr,0x0060);
  MEM_WRITE(adr,0x00d0);
  sts=_FlashOperationComplete(adr,ustmp,700000*FLASH_MAX_WAIT);
  _FlashErrorOut("UnLock Block",sts);
  return(sts==0);
  }

/*Returns TRUE for success and FALSE for failure                                  */

int FWBLock(bit32u from,bit32 size)
  {
  bit16u *usp,ustmp;
  int sts;
  bit32u ad,to,blocksize;

  if (size==0) return(TRUE);
  to=from+size;
  blocksize=FWBGetBlockSize(from);
  from&=~(blocksize-1);
  for(sts=0,ad=from;(ad<to)&&(!sts)&&(blocksize);ad+=blocksize)
    {
    usp=(bit16u *)ad;
    ustmp=*usp;
    MEM_WRITE(ad,0x0060);
    MEM_WRITE(ad,0x0001);
    sts&=_FlashOperationComplete(ad,ustmp,650*FLASH_MAX_WAIT);
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
#ifdef _AVALANCHE_    /*EVM                                                  */
  if ((base>=CS3_BASE)&&(base<(CS3_BASE+CS3_SIZE)))
    {
    itmp=(128*1024);   /*Strata flash 8/16 bit                               */
    if (EMIF_ASYNC_CS3&0x2)
      itmp*=2;              /*Dual devices                                   */
    return(itmp);
    }
#endif
  if ((base>=CS0_BASE)&&(base<(CS0_BASE+CS0_SIZE)))
    {
    itmp=(128*1024);   /*Strata flash 8/16 bit                               */
    if (EMIF_ASYNC_CS0&0x2)
      itmp*=2;              /*Dual devices                                   */
    return(itmp);
    }
  return(0);
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
    MEM_WRITE(i,0x0020);
    MEM_WRITE(i,0x00d0);
    sts=_FlashOperationComplete(i,0xffff,FLASH_MAX_WAIT*5000000);
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
  for(i=0;i<(sizeof FWBwdata);i++) 
    FWBwdata[i]=0xff;
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
  if((adr&(~0x1f))!=FWBBaseAdr)
    sts=FWBClose();
  if (FWBvalid==0)
    {
    for(i=0,isp=(bit16u *)(adr&~0x1f),idp=(bit16u *)FWBwdata;i<16;i++)
      *idp++=*isp++;
    }
  /*sys_printf("FWBWriteByte(%x,%x);\n",adr,cVal&0x0ff);                         */
  FWBBaseAdr=adr&(~0x1f);
  i=adr&0x1f;
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
      MEM_WRITE(FWBBaseAdr,0x00ff);
      MEM_WRITE(FWBBaseAdr,0x0050);
      }
    }

  _FlashErrorOut("Program Block",sts);

  for(i=0;i<(sizeof FWBwdata);i++) 
    FWBwdata[i]=0xff;
  FWBBaseAdr=0xffffffff;
  FWBvalid=0;
  return(sts==0);
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


/*Used to decode Flash error values                                          */

void _FlashErrorOut(char *str,int sts)
  {

  if (sts)
    sys_printf("Flash '%s' operation Failed: sts=%08x\n",str,sts);
  }

/*Returns 0 for success and !=0 for error                                    */

int _FWBWriteBlock(void)
  {
  int icount,i,done;
  bit16u *usp,ustmp;

  MEM_WRITE(FWBBaseAdr,0x00e8);
  icount=0;
  do
    {
    ustmp=MEM_READ(FWBBaseAdr);
    done = ((ustmp&0x0080)==0x0080);
    if ((!done)&&(icount>(FLASH_MAX_WAIT*650)))
      return(0xeeee0002);
    icount++;
    }while(!done);
  MEM_WRITE(FWBBaseAdr,0x000f);
  for(usp=(bit16u *)FWBwdata,i=0;i<16;i++,usp++)
    MEM_WRITE(FWBBaseAdr+(i*2),*usp);
  MEM_WRITE(FWBBaseAdr,0x00d0);
  usp=(bit16u *)FWBwdata;
  return(_FlashOperationComplete(FWBBaseAdr,*usp,FLASH_MAX_WAIT*650));
  }

/*Returns 0 for success and !=0 for failure                                  */

int _FlashOperationComplete(bit32u adr,bit16u dat,int timeoutloops)
  {
  bit16u sts,fnd;
  int icount;
  int err;

  icount=0;
  do
    {
    if (++icount>timeoutloops)
      {
      MEM_WRITE(adr,0x00ff);    /*Reset to read mode                     */
      return(0xeeee0001);
      }
    sts=MEM_READ(adr);
    if ((sts&0x0080)==0x0080)
      {
      err = (sts&0x0030);
      MEM_WRITE(adr,0x00ff);    /*Reset to read mode                     */
      if (!err)
        {
        fnd=MEM_READ(adr);
        if (fnd==dat) 
          return(0);
        }
       else
        MEM_WRITE(adr,0x0050);  /*Reset error bits                       */
      return(sts);
      }
    }while(1);
  }

