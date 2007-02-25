/* $Id: avalanche_dma.h,v 1.1.1.2 2005/03/28 06:56:46 sparq Exp $
 * linux/include/asm/dma.h: Defines for using and allocating dma channels.
 *
 * Texas Instruments Avalanche ASIC DMA support
 * Jeff Harrell (jharrell@ti.com) 07/27/2000 
 *
 */

#ifndef __ASM_MIPS_AVALANCHE_DMA_H
#define __ASM_MIPS_AVALANCHE_DMA_H

#include <linux/config.h>
#include <asm/io.h>			/* need byte IO */
#include <linux/spinlock.h>		/* And spinlocks */
#include <linux/delay.h>
#include <asm/system.h>
#include <asm/avalanche/avalanche_map.h>


#ifdef HAVE_REALLY_SLOW_DMA_CONTROLLER
#define dma_outb	outb_p
#else
#define dma_outl	outl
#endif /* HAVE_REALLY_SLOW_DMA_CONTROLLER */

#define dma_inl		inl


#define MAX_DMA_CHANNELS	5  /* includes one channel for the cascade device */

/*
 * The maximum address in KSEG0 that we can perform a DMA transfer to on this
 * platform.  This describes only the PC style part of the DMA logic like on
 * Deskstations or Acer PICA but not the much more versatile DMA logic used
 * for the local devices on Acer PICA or Magnums.
 */
#define MAX_DMA_ADDRESS         (PAGE_OFFSET + 0x1fffffff)

/* DMA controller registers */

/* DMA channel 0 */
#define DMA_CHANNEL0            0x0
#define DMA0_SOURCE_ADDR        AVALANCHE_MCDMA0_CTRL_BASE + 0x0  /* DMA channel 0 source address */
#define DMA0_DEST_ADDR          AVALANCHE_MCDMA0_CTRL_BASE + 0x4  /* DMA channel 0 destination address */
#define DMA0_TRANS_CTRL         AVALANCHE_MCDMA0_CTRL_BASE + 0x8  /* DMA channel 0 transfer control */
                                                               
/* DMA channel 1 */                                            
#define DMA_CHANNEL1            0x1                            
#define DMA1_SOURCE_ADDR        AVALANCHE_MCDMA0_CTRL_BASE + 0x40 /* DMA channel 1 source address */
#define DMA1_DEST_ADDR          AVALANCHE_MCDMA0_CTRL_BASE + 0x44 /* DMA channel 1 destination address */
#define DMA1_TRANS_CTRL         AVALANCHE_MCDMA0_CTRL_BASE + 0x48 /* DMA channel 1 transfer control */
                                                               
/* DMA channel 2 */                                            
#define DMA_CHANNEL2            0x2                            
#define DMA2_SOURCE_ADDR        AVALANCHE_MCDMA0_CTRL_BASE + 0x80 /* DMA channel 2 source address */
#define DMA2_DEST_ADDR          AVALANCHE_MCDMA0_CTRL_BASE + 0x84 /* DMA channel 2 destination address */
#define DMA2_TRANS_CTRL         AVALANCHE_MCDMA0_CTRL_BASE + 0x88 /* DMA channel 2 transfer control */
                                                               
/* DMA channel 3 */                                            
#define DMA_CHANNEL3            0x3                            
#define DMA3_SOURCE_ADDR        AVALANCHE_MCDMA0_CTRL_BASE + 0xC0 /* DMA channel 3 source address */
#define DMA3_DEST_ADDR          AVALANCHE_MCDMA0_CTRL_BASE + 0xC4 /* DMA channel 3 destination address */
#define DMA3_TRANS_CTRL         AVALANCHE_MCDMA0_CTRL_BASE + 0xC8 /* DMA channel 3 transfer control */

/* DMA transfer control bits */
#define DMA_BURST_MODE_1_WORD        0<<(20+0) /* Single word burst mode */
#define DMA_BURST_MODE_2_WORD        1<<(20+1) /* Double word burst mode */
#define DMA_BURST_MODE_4_WORD        1<<(20+2) /* Quad word burst mode   */

#define DMA_SRC_ADDR_MODE_INCR       0<<(22+0) /* Source Address mode (increment) */
#define DMA_SRC_ADDR_MODE_FIXED      1<<(22+2) /* Source Address mode (fixed)     */

#define DMA_DST_ADDR_MODE_INCR       0<<(24+0) /* Destination Address mode (increment) */
#define DMA_DST_ADDR_MODE_FIXED      1<<(24+2) /* Destination Address mode (fixed)     */

#define DMA_CHANNEL_ACTIVE           1<<(30)   
#define DMA_TRANSFER_START           1<<(31)   /* 1=start transfer 0=stop transfer */
#define DMA_TRANSFER_LENGTH          (0x3FFFC)

#define DMA_SOURCE                   1        
#define DMA_DEST                     0


extern spinlock_t  dma_spin_lock;

static __inline__ unsigned long claim_dma_lock(void)
{
	unsigned long flags;
	spin_lock_irqsave(&dma_spin_lock, flags);
	return flags;
}

static __inline__ void release_dma_lock(unsigned long flags)
{
	spin_unlock_irqrestore(&dma_spin_lock, flags);
}

/* enable/disable a specific DMA channel */
static __inline__ void enable_dma(unsigned int dmanr)
{
  switch(dmanr)
    {
      case  (DMA_CHANNEL0):
        dma_outl((dma_inl(DMA0_TRANS_CTRL) | DMA_TRANSFER_START), DMA0_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL1):
        dma_outl((dma_inl(DMA1_TRANS_CTRL) | DMA_TRANSFER_START), DMA1_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL2):
        dma_outl((dma_inl(DMA2_TRANS_CTRL) | DMA_TRANSFER_START), DMA2_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL3):
        dma_outl((dma_inl(DMA3_TRANS_CTRL) | DMA_TRANSFER_START), DMA3_TRANS_CTRL);
        break;
    }
}

static __inline__ void disable_dma(unsigned int dmanr)
{
  switch(dmanr)
    {
      case  (DMA_CHANNEL0):
        dma_outl((dma_inl(DMA0_TRANS_CTRL) & ~DMA_TRANSFER_START), DMA0_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL1):
        dma_outl((dma_inl(DMA1_TRANS_CTRL) & ~DMA_TRANSFER_START), DMA1_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL2):
        dma_outl((dma_inl(DMA2_TRANS_CTRL) & ~DMA_TRANSFER_START), DMA2_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL3):
        dma_outl((dma_inl(DMA3_TRANS_CTRL) & ~DMA_TRANSFER_START), DMA3_TRANS_CTRL);
        break;
    }    
}

/* 
 *  function: set_dma_mode():   
 *      This function sets the mode of the dma channel. This function can set
 *      the Source/Destination modes as well as the burst mode of the Channel.
 *      NOTE: when using this routine make sure to pass in all settings for the
 *      Source/Destination mode and burst mode!
 */

static __inline__ void set_dma_mode(unsigned int dmanr, char mode)
{
   switch(dmanr)
    {
      case  (DMA_CHANNEL0):
        dma_outl(((dma_inl(DMA0_TRANS_CTRL) & ~0x03F00000) | mode), DMA0_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL1):
        dma_outl(((dma_inl(DMA1_TRANS_CTRL) & ~0x03f00000) | mode), DMA1_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL2):
        dma_outl(((dma_inl(DMA2_TRANS_CTRL) & ~0x03f00000) | mode), DMA2_TRANS_CTRL);
        break;
      case  (DMA_CHANNEL3):
        dma_outl(((dma_inl(DMA3_TRANS_CTRL) & ~0x03f00000) | mode), DMA3_TRANS_CTRL);
        break;
    }     
}

static __inline__ void set_dma_addr(unsigned int dmanr, unsigned int a, unsigned int src_dest)
{
   switch(dmanr)
    {
      case  (DMA_CHANNEL0):
        if(src_dest == DMA_SOURCE)
          dma_outl( a, DMA0_SOURCE_ADDR);     /* DMA Channel 0 source address      */
        else
          dma_outl( a, DMA0_DEST_ADDR);       /* DMA Channel 0 destination address */
        break;
      case  (DMA_CHANNEL1):
        if(src_dest == DMA_SOURCE)
          dma_outl( a, DMA1_SOURCE_ADDR);     /* DMA Channel 1 source address      */
        else                                                                         
          dma_outl( a, DMA1_DEST_ADDR);     /* DMA Channel 1 destination address */
        break;
      case  (DMA_CHANNEL2):
        if(src_dest == DMA_SOURCE)
          dma_outl(a, DMA2_SOURCE_ADDR);      /* DMA Channel 2 source address      */
        else                                                                          
          dma_outl(a, DMA2_DEST_ADDR);      /* DMA Channel 2 destination address */
        break;
      case  (DMA_CHANNEL3):
        if(src_dest == DMA_SOURCE)
          dma_outl( a, DMA3_SOURCE_ADDR);      /* DMA Channel 3 source address      */
        else                                                                           
          dma_outl( a, DMA3_DEST_ADDR);      /* DMA Channel 3 destination address */
        break;
    }     
}

/* Set the transfer size for a dma channel */
static __inline__ void set_dma_count(unsigned int dmanr, unsigned int count)
{
  switch(dmanr)
    {
      case (DMA_CHANNEL0): 
        dma_outl(((dma_inl(DMA0_TRANS_CTRL) & ~0x3FFFF) | count), DMA0_TRANS_CTRL);
        break;
      case (DMA_CHANNEL1): 
        dma_outl(((dma_inl(DMA1_TRANS_CTRL) & ~0x3FFFF) | count), DMA1_TRANS_CTRL);
        break;
      case (DMA_CHANNEL2): 
        dma_outl(((dma_inl(DMA2_TRANS_CTRL) & ~0x3FFFF) | count), DMA2_TRANS_CTRL);
        break;
      case (DMA_CHANNEL3): 
        dma_outl(((dma_inl(DMA3_TRANS_CTRL) & ~0x3FFFF) | count), DMA3_TRANS_CTRL);
        break;
    }    
}


/* Get DMA residue count. After a DMA transfer, this
 * should return zero. Reading this while a DMA transfer is
 * still in progress will return unpredictable results.
 * If called before the channel has been used, it may return 1.
 * Otherwise, it returns the number of _bytes_ left to transfer.
 *
 * Assumes DMA flip-flop is clear.
 */
static __inline__ int get_dma_residue(unsigned int dmanr)
{

switch(dmanr)                                                                    
  {                                                                              
    case (DMA_CHANNEL0):
      return (dma_inl(DMA0_TRANS_CTRL) & DMA_TRANSFER_LENGTH);                                                                     
    case (DMA_CHANNEL1):                                                         
      return (dma_inl(DMA1_TRANS_CTRL) & DMA_TRANSFER_LENGTH);                                                                     
    case (DMA_CHANNEL2):                                                         
      return (dma_inl(DMA2_TRANS_CTRL) & DMA_TRANSFER_LENGTH);                                                                     
    case (DMA_CHANNEL3):                                                         
      return (dma_inl(DMA3_TRANS_CTRL) & DMA_TRANSFER_LENGTH);                                                                     
  }                                                                              
    return -1;

}




/* These are in kernel/dma.c: */
extern int request_dma(unsigned int dmanr, const char * device_id);	/* reserve a DMA channel */
extern void free_dma(unsigned int dmanr);	/* release it again */

#ifdef CONFIG_PCI
extern int isa_dma_bridge_buggy;
#else
#define isa_dma_bridge_buggy 	(0)
#endif

#endif /* __ASM_MIPS_DMA_H */








