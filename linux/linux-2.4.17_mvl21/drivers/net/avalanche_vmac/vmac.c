/*
 * vmac.c --  AR5 Ethernet Driver
 *
 */

#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>     
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>


#include <asm/avalanche/avalanche_map.h>   
#include <asm/addrspace.h>


#ifndef TRUE         
#define TRUE (1==1)  
#endif

#ifndef FALSE         
#define FALSE (1==0)  
#endif


/* Just in case the SOC has single instance */
#ifndef AVALANCHE_HIGH_EMAC_BASE
#define AVALANCHE_HIGH_EMAC_BASE
#endif

#include <asm/mips-boards/prom.h>

#include "vmac.h"
#include "emacmdio.h"

/* Global Variables */
static struct net_device  *VmacDevices = NULL;
static struct net_device  **VmacDevices_head = NULL;
static int                VmacDevicesInstalled = 0;
static char		  VmacSignature[] = "VMAC";
static int                num_devices = CONFIG_MIPS_VMAC_PORTS;
static int                isrpace = 2;
static u8                 *VmacPadBuffer;

/* Pointer to stats proc entry */
struct proc_dir_entry *gp_stats_file = NULL;

static const  unsigned long io_base[2] = { AVALANCHE_LOW_EMAC_BASE, AVALANCHE_HIGH_EMAC_BASE };
static char vmac_version[] = "3.0.0";

/* Debug Print Levels */
#define CONFIG_VMAC_DEBUG 0 /* Debug level (0=no msgs, 5=verbose) */

/* Under heavy loads, the interrupts will be called extremly close together.
* When doing routing, this is not a problem as the stack will drop the skb's
* if they cannot process them. But when doing bridging, we don't give the
* kernel time to free skbs when using kfree_skb_any. Thus we use kfree_skb
* even though we are in a hard interupt. This is wrong, but it works for now.
*/
#define vmac_kfree_skb_any(x)	dev_kfree_skb(x)

#if defined(CONFIG_VMAC_DEBUG) && (CONFIG_VMAC_DEBUG > 0)
#define DPRINTK(level, format, args...)  do { if (CONFIG_VMAC_DEBUG >= (level)) \
                                                  printk(VMAC format, ##args); } while(0)
#else
#define DPRINTK(level, format, args...)  while(0)
#endif


#ifdef MODULE
MODULE_DESCRIPTION ("Avalanche EMAC Device Driver");
MODULE_AUTHOR("JAB");
MODULE_LICENSE("GPL");
MODULE_PARM(num_devices,"i");
MODULE_PARM(isrpace,"i");
#endif

/* Forward declarations */
static void	vmac_handle_tx(struct net_device *dev);
static int	vmac_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static void	vmac_set_rx_mode (struct net_device *dev);
static int vmac_write_proc(struct file *fp, const char * buf, unsigned long count, void * data);
//void xdump( u_char*  cp, int  length, char*  prefix );

static unsigned char str2hexnum(unsigned char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0; /* foo */
}

static void str2eaddr(unsigned char *ea, unsigned char *str)
{
	int i;
	unsigned char num;
	for(i = 0; i < 6; i++) {
		if((*str == '.') || (*str == ':'))
			str++;
		num = str2hexnum(*str++) << 4;
		num |= (str2hexnum(*str++));
		ea[i] = num;
	}
}

static void
vmac_HandleTxInt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev;
	VmacPrivateInfo *priv;
        int int_index;

	dev = (struct net_device *) dev_id;
	priv = (VmacPrivateInfo *) dev->priv;

        int_index = VMAC_INTSTS(dev->base_addr) & priv->int_mask;
	if(int_index & TCR_FREEZE)
  		vmac_tcr_freeze(dev);
	if(int_index & TCRA_THRESH)
  		vmac_tcra_thresh(dev);
	if(int_index & POSTED_TCRA)
  		vmac_txirq(dev);
} /* vmac_HandleTxInt */



static void
vmac_HandleRxInt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev;
	VmacPrivateInfo *priv;
	int int_index;

	dev = (struct net_device *) dev_id;
	priv = (VmacPrivateInfo *) dev->priv;


        int_index = VMAC_INTSTS(dev->base_addr) & priv->int_mask;
	if(int_index & RCR_FREEZE)
  		vmac_rcr_freeze(dev);
	if(int_index & RCRA_THRESH)
  		vmac_rcra_thresh(dev);
	if(int_index & POSTED_RCRA)
        {
        
//   		vmac_rxirq(dev);
/* after acking and  disabling interrupts for rx  */   
	    VMAC_INTSTS(dev->base_addr) = POSTED_RCRA;  // VW_TODO: What does the SAR do? 
	    VMAC_INTMASK(dev->base_addr) &= ~POSTED_RCRA;
           
/* schedule tasklet here  */
           tasklet_schedule(&priv->vmac_tasklet);

        }
       
} /* vmac_HandleRxInt */


static void
vmac_rx_tasklet(struct net_device *dev)
{

	VmacPrivateInfo *priv;
        int pending;


	priv = (VmacPrivateInfo *) dev->priv;
       
/* process rx packets  */
        pending = vmac_rxirq(dev);

        
/* schedule tasklet again
 * if there are packets to be processed
 *  else enable interrupts for rx
 */
    if(pending)
        tasklet_schedule(&priv->vmac_tasklet);

    else
	VMAC_INTMASK(dev->base_addr) |= POSTED_RCRA;

} 




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_reset(struct net_device * dev)
 *
 *  Description: Resets the VMAC
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int vmac_reset(struct net_device *dev)
{
	VmacPrivateInfo *priv;

	u16  result2;
	unsigned int result;

	priv = (VmacPrivateInfo *) dev->priv;

	/* Reset the VMAC in the PRCR register */ 
	if(priv->vmac_num == 0) {
            /* Put Vmac0 into reset */
		avalanche_reset_ctrl(AVALANCHE_LOW_EMAC_RESET_BIT, IN_RESET); 
                udelay(1);

            /* Bring Vmac0 out of reset */    
		avalanche_reset_ctrl(AVALANCHE_LOW_EMAC_RESET_BIT, OUT_OF_RESET); 
	}
#if 0 
	else {
            /* Put Vmac1 into reset */
            AVALANCHE_PRCR &= ~( AV_RST_ETH_MAC1 ); udelay(1);
            /* Bring Vmac1 out of reset */    
            AVALANCHE_PRCR |= ( AV_RST_ETH_MAC1 ); udelay(1); 
	}
#endif

	/* Reset the mac */
	VMAC_DMACONFIG(dev->base_addr) = SRESET; udelay(10);		/* Soft Reset of DMA SM & MAC */
	VMAC_DMACONFIG(dev->base_addr) = 0; udelay(10);			/* Remove Reset */

	if(VMAC_DMACONFIG(dev->base_addr) != 0) {
		printk(VMAC_ERROR "Could not soft reset Avalanche VMAC\n");
		return -ENODEV;
	}

	return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_InitTxBuffdesc(struct net_device *dev)
 *
 *  Description: Initialize transmit buffer descriptors
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int __init
vmac_InitTxBuffdesc(struct net_device *dev)
{
  int i;  
  VmacPrivateInfo *priv;
  void *txfbl_ptr;
  tx_fbld_t *prev = NULL;
  tx_fbld_t *fbl_ptr;
  
  priv = (VmacPrivateInfo *) dev->priv;
  priv->txfbl_num = 0;   /* Initialize number of free buffers */
    
  for(i=0; i < NUM_TX_BUF; i++)  
    {
      if((txfbl_ptr = kmalloc(sizeof(tx_fbld_t), GFP_KERNEL | GFP_DMA)) == NULL)
        {
          printk(VMAC_ERROR "Could not allocate TX FBL descriptor\n");
          return -ENODEV;
        }
      
      fbl_ptr = (tx_fbld_t *) txfbl_ptr;
      
      if(i==0)
        priv->txfbl_head = prev = (tx_fbld_t *) txfbl_ptr;
      else
        {
          prev->next = (tx_fbld_t *) txfbl_ptr;
          prev = (tx_fbld_t *) txfbl_ptr;
        }

      fbl_ptr->mode = 0;
      fbl_ptr->BufPtr = NULL;
      fbl_ptr->skb   = NULL;
      priv->txfbl_num++;
    }
  
  priv->txfbl_tail = (tx_fbld_t *) txfbl_ptr;
  fbl_ptr->next = NULL;
  
  return 0;
} 


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_InitRxBuffdesc(struct net_device *dev)
 *
 *  Description: Initialize receive buffer descriptors
 *               Moved to Open function to fix open issues with data on port
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int __init
vmac_InitRxBuffdesc (struct net_device *dev)
{
  int i;  
  VmacPrivateInfo *priv;
  rx_fbld_t			*rxfbl_descriptor;  /* free buffer descriptor pointer */
  rx_fbld_t			*desc_ptr = NULL;	/* free buffer pointer */
  struct sk_buff	*skb = NULL;		/* working skb */

  priv = (VmacPrivateInfo *) dev->priv;
  

  /* Get vmac rx buffers setup */
  if ((rxfbl_descriptor  = kmalloc ((NUM_RX_BUF * sizeof (rx_fbld_t)), GFP_KERNEL | GFP_DMA)) == NULL)
    {
      printk (VMAC_ERROR "Could not allocate RX Buff descriptors\n");
      return -ENODEV;
    }
  printk("VMAC: Base BuffDesc address %p, size %d\n", rxfbl_descriptor, NUM_RX_BUF);

  priv->rxfbl = rxfbl_descriptor;  		/* fbl_id = 0, only 1 choice for VMAC */
  desc_ptr = priv->rxfbl;

  priv->rx_burstRCRcount=0;
  priv->rx_burstbufcount=0;
  priv->rx_burstbufstart=0;
  priv->rx_burstbufstop=0;

  for (i = 0; i < NUM_RX_BUF; i++)
    {
		/* Alloc new skb for the buffer */
		/* TBD: If the following errors out, we have to clean up the skb's that
		* have been allocated already. */
		if((skb = dev_alloc_skb(MAX_FRAME_SIZE)) == NULL)
		{
			printk(VMAC_ERROR "unable to alloc new skb, PDU length = %d\n",
					MAX_FRAME_SIZE);
			return -ENODEV;
		}

		desc_ptr[i].size = 0;
		desc_ptr[i].BufPtr = virt_to_bus ((int *) skb->data);
		desc_ptr[i].Fbl_id = 0;
		desc_ptr[i].offset = 0;
		desc_ptr[i].skb = skb;
    }
	desc_ptr[i-1].next = 0;                      /* take care of the last buffer */

  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    *
    *  Function: int vmac_Inittxcomp(struct net_device *dev,int num)
    *
    *  Description: Initialize transmit completion ring
    *
    *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static int __init
vmac_Inittxcomp(struct net_device *dev,int num)
  {
  int  i,j;
  unsigned long iTmp;
  void *pTmp;
  VmacPrivateInfo *priv;
  
  priv = (VmacPrivateInfo *) dev->priv;
  
  //Make num power of two
  num *= 2;
  num--;

  for(i=0,j = 0x80000000;i<32;i++,j>>=1)
    if (j & num) 
      num &= j;


  /* Allocate memory so that it is on  a power of size alignment */

  iTmp = num * 16 * 2;                                 /* size of Tx comp ring */
  pTmp = m_alloc(iTmp, 15);                             /* Align on a 2^8 boundary TBD */
  dma_cache_wback_inv((unsigned long)pTmp,iTmp);       /* Invalidate this memory */
  priv->txcr = (tx_compring *) pTmp;                   /* software pinter to txcr */
  priv->txcr_start = priv->txcr;                       /* first structure of completion ring */
  priv->txcr_end = (priv->txcr+(num-1));               /* last structure of completion ring  */
  memset(priv->txcr, 0, iTmp);                         /* Clear memory */

  /* allocate Tx completion ring (num entries) */

  VMAC_TCRPTR(dev->base_addr) = virt_to_phys(priv->txcr);

  DPRINTK(5,"TCR start = %x\n",VMAC_TCRPTR(dev->base_addr));
  
  VMAC_TCRSIZE(dev->base_addr) = num - 1;
  VMAC_TCRTOTENT(dev->base_addr) = num;
  VMAC_TCRENTINC(dev->base_addr) = num;
  
  return 0;
  
  } /* End init_txcomp() */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_txirq(int irq, void *dev_id, struct pt_regs *reg)
 *
 *  Description: Avalanche VMAC Tx interrupt
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_txirq(struct net_device *dev)
{
//	VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;
 
	VMAC_INTSTS(dev->base_addr) = POSTED_TCRA;  // Needs to be cleared in VMAC
	vmac_handle_tx(dev);
//	DPRINTK(5,"POSTED_TCRA interrupt\n");
} /* vmac_txirq */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_tcra_thresh(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Tx Completion Ring Threshold Reached 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_tcra_thresh(struct net_device *dev)
{
//	VmacPrivateInfo *tp = (VmacPrivateInfo *) dev->priv;
//	tx_compring *tx_ring_ptr;
//	tx_fbld_t  *CurrentTxFBLD;
//	unsigned int i,num_entries;

	VMAC_INTSTS(dev->base_addr) = TCRA_THRESH;
	vmac_handle_tx(dev);
}

static void
vmac_handle_tx(struct net_device *dev)
{
	VmacPrivateInfo *tp = (VmacPrivateInfo *) dev->priv;
	tx_compring *tx_ring_ptr;
	tx_fbld_t  *CurrentTxFBLD;
	unsigned int i,num_entries;

	num_entries = VMAC_TCRPENDENT(dev->base_addr);
	for(i=0;i<num_entries;i++)
	{
		tx_ring_ptr = (tx_compring *)tp->txcr;
		do{
			dma_cache_inv((unsigned long)tx_ring_ptr, CACHE_LINE_SIZE);
		}while(tx_ring_ptr->sof_list == 0);
		CurrentTxFBLD = (tx_fbld_t *) bus_to_virt(tx_ring_ptr->sof_list);

		DPRINTK(5,"TxCR [%p]: fbd[%p]:%d buff[%p]\n",tx_ring_ptr,CurrentTxFBLD, 
			(CurrentTxFBLD->mode&0xffff), (char *) CurrentTxFBLD->BufPtr);

		tp->good_tx_sw_frames++;
		tp->stats.tx_packets++;
		tp->stats.tx_packets += CurrentTxFBLD->skb->len;

		vmac_kfree_skb_any(CurrentTxFBLD->skb);

		/* Free the buffer pointed to by the the Tx Descriptor */
		Put_TxDesc(CurrentTxFBLD,tp);

		/* Increment the Tx Completion buffer pointer */
		tx_ring_ptr->sof_list=0;
		if(++tx_ring_ptr > tp->txcr_end)
			tp->txcr = tp->txcr_start;
		else
			tp->txcr = tx_ring_ptr; 
	}

	/* Increment the TX FBL register */
	VMAC_TCRENTINC(dev->base_addr) = num_entries; /* Free TCR Entry */ 
} /* vmac_tcra_thresh */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_tcr_freeze(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Tx Completion Ring Freeze 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_tcr_freeze(struct net_device *dev)
{
//	VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;
 
	VMAC_INTSTS(dev->base_addr) = TCR_FREEZE;  // Needs to be cleared in VMAC

	DPRINTK(5,"tcra_freeze interrupt\n");
} /* vmac_tcr_freeze */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_rxirq(struc net_device *dev)
 *
 *  Description: Avalanche VMAC Rx interrupt
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int
vmac_rxirq(struct net_device *dev)
{
	VmacPrivateInfo  *priv;
	rx_compring *rx_ring_ptr;
	unsigned int pdu_len;
	rx_fbld_t * first_buff_desc = NULL; 
	struct sk_buff *skb = NULL;  
	struct sk_buff *skbcopy = NULL;  
	unsigned char *data_ptr; 
	unsigned int i,num_entries;
	static int num_called=0;
	int retval;

	priv = (VmacPrivateInfo *) dev->priv;
  
//	VMAC_INTSTS(dev->base_addr) = POSTED_RCRA;  // VW_TODO: What does the SAR do? 

	num_entries = VMAC_RCRPENDENT(dev->base_addr) - priv->rx_burstRCRcount;
	
        for(i=0;i<num_entries  && i < CFG_RX_NUM_BUF_SERVICE;i++)
	{
		rx_ring_ptr =(rx_compring *)priv->rxcr;
		do{
			dma_cache_inv((unsigned long)rx_ring_ptr, CACHE_LINE_SIZE);
		}while(((unsigned long*)rx_ring_ptr)[0]==0);

		/* check for errors in the Rx packet */ 
		if(rx_ring_ptr->valid)
		{
			num_called++;
			/* parse the completion ring */
			first_buff_desc = (rx_fbld_t *)bus_to_virt(rx_ring_ptr->sof_list);
			dma_cache_inv((unsigned long)first_buff_desc,CACHE_LINE_SIZE); // *** Need to do this ? ***

			pdu_len = first_buff_desc->size;
			data_ptr = (unsigned char *)(phys_to_virt(first_buff_desc->BufPtr));
			dma_cache_inv((unsigned long)data_ptr,pdu_len+2);
			skb = first_buff_desc->skb;
			if(skb->data != data_ptr)
			{
				printk("Oops. Skb data pointers do not match\n");
				udelay(1);
			}

			if(!rx_ring_ptr->error_ind || pdu_len != rx_ring_ptr->frame_len)
			{	/* Packet looks ok. */
				skbcopy = dev_alloc_skb (pdu_len + 2);
				if (skbcopy) {
					skbcopy->dev = dev;
					skb_reserve (skbcopy, 2);	/* 16 byte align the IP fields. */

					eth_copy_and_sum (skbcopy, data_ptr, pdu_len, 0);
					skb_put (skbcopy, pdu_len);

					skbcopy->protocol = eth_type_trans (skbcopy, dev);
					netif_rx (skbcopy);
					dev->last_rx = jiffies;
					if(!(*data_ptr & 0x01) )                                   
    					    priv->mib_counters.inUnicastPktsLow++; //unicast  packet 
                                                           
				} else {
					priv->mib_counters.inDiscardPkts++;
				  //	printk (KERN_WARNING
				  //			"%s: Memory squeeze, dropping packet.\n",
				  //			dev->name);
				  }

				vmac_kfree_skb_any(skb);
#if 0
				skb_put(skb,first_buff_desc->size);
//				skb_trim(skb,pdu_len+2);

printk("\nRxPacket = %d, len=%d\n", num_called, skb->len);
xdump(skb->data, skb->len, "RxPacket: ");
				/* Write metadata, pass skb to upper layers */
				skb->dev = dev;
				skb->protocol = eth_type_trans(skb,dev);
				skb->ip_summed = CHECKSUM_UNNECESSARY; /* VW_TODO:Needs to be changed */

				retval= netif_rx(skb);
				printk("Netif retval = %d\n", retval);
#endif
			}
			else				/* Dump error skb. And get new one */
			{
				if(pdu_len != rx_ring_ptr->frame_len){
					printk("Lengths don't match: pdu_len=%d, frame len = %d\n", pdu_len, rx_ring_ptr->frame_len);
				}else{
/*
					if(rx_ring_ptr->ch_off)
						printk("ch_off: ");
					if(rx_ring_ptr->error_ind)
						printk("Error ind: ");
					if(rx_ring_ptr->ext_sts)
						printk("Extended status: ");
					printk("Valid bufdesc, but errored: ");
					if(rx_ring_ptr->errorbits & 0x01)
						printk("bufstarv ");
					if(rx_ring_ptr->errorbits & 0x02)
						printk("crcerr ");
					if(rx_ring_ptr->errorbits & 0x04)
						printk("ovf_err ");
					if(rx_ring_ptr->errorbits & 0x08)
						printk("code_err ");
					if(rx_ring_ptr->errorbits & 0x10)
						printk("0x10 ");
					if(rx_ring_ptr->errorbits & 0x20)
						printk("jabber ");
					if(rx_ring_ptr->errorbits & 0x40)
						printk("0x40 ");
					if(rx_ring_ptr->errorbits & 0x80)
						printk("0x80 ");
					if(rx_ring_ptr->errorbits & 0x100)
						printk("truncated ");
					if(rx_ring_ptr->errorbits & 0x200)
						printk("no_match ");
					if(rx_ring_ptr->errorbits & 0x400)
						printk("mc_frame ");
					if(rx_ring_ptr->errorbits & 0x800)
						printk("rxcrc ");
					if(rx_ring_ptr->errorbits & 0x1000)
						printk("runt ");
					if(rx_ring_ptr->errorbits & 0x2000)
						printk("0x2000 ");
					if(rx_ring_ptr->errorbits & 0x4000)
						printk("pefcef ");
					if(rx_ring_ptr->errorbits & 0x8000)
						printk("cef ");
					printk("\n");
*/
				}
				vmac_kfree_skb_any(skb);
				priv->frame_error++;
			}

			/* allocate new skb, and place it in pool */
			if((skb = dev_alloc_skb(MAX_FRAME_SIZE)) == NULL){
				printk(VMAC_ERROR "rx irq: unable to alloc new skb\n");
			}
			first_buff_desc->BufPtr=virt_to_phys((int *)skb->data);
			first_buff_desc->skb=skb;

			/* Free RCR entries to the "queue" */
			((unsigned long*)rx_ring_ptr)[0]= 0;
			priv->rx_burstRCRcount++;

			/* Return free buffers to the "queue" */
			priv->rx_burstbufcount++;
			if(priv->rx_burstbufstart)	// Queue has data in it
			{
				priv->rx_burstbufstop->next=virt_to_phys((int*)first_buff_desc);
				priv->rx_burstbufstop=first_buff_desc;
			} else {			// Queue is empty
				priv->rx_burstbufstart=first_buff_desc;
				priv->rx_burstbufstop=first_buff_desc;
			}
		} else {
/*
			if(rx_ring_ptr->ch_off)
				printk("ch_off: ");
			if(rx_ring_ptr->error_ind)
				printk("Error ind: ");
			if(rx_ring_ptr->ext_sts)
				printk("Extended status: ");
			printk("No valid bufdesc: ");
			if(rx_ring_ptr->errorbits & 0x01)
				printk("bufstarv ");
			if(rx_ring_ptr->errorbits & 0x02)
				printk("crcerr ");
			if(rx_ring_ptr->errorbits & 0x04)
				printk("ovf_err ");
			if(rx_ring_ptr->errorbits & 0x08)
				printk("code_err ");
			if(rx_ring_ptr->errorbits & 0x10)
				printk("0x10 ");
			if(rx_ring_ptr->errorbits & 0x20)
				printk("jabber ");
			if(rx_ring_ptr->errorbits & 0x40)
				printk("0x40 ");
			if(rx_ring_ptr->errorbits & 0x80)
				printk("0x80 ");
			if(rx_ring_ptr->errorbits & 0x100)
				printk("truncated ");
			if(rx_ring_ptr->errorbits & 0x200)
				printk("no_match ");
			if(rx_ring_ptr->errorbits & 0x400)
				printk("mc_frame ");
			if(rx_ring_ptr->errorbits & 0x800)
				printk("rxcrc ");
			if(rx_ring_ptr->errorbits & 0x1000)
				printk("runt ");
			if(rx_ring_ptr->errorbits & 0x2000)
				printk("0x2000 ");
			if(rx_ring_ptr->errorbits & 0x4000)
				printk("pefcef ");
			if(rx_ring_ptr->errorbits & 0x8000)
				printk("cef ");
			printk("\n");
*/
			/* Free RCR Entry */
			((unsigned long*)rx_ring_ptr)[0]= 0;
			priv->rx_burstRCRcount++;
		}
		/* Increment the Rx Completion ring pointer */
		if(++rx_ring_ptr > priv->rxcr_end)
			priv->rxcr = priv->rxcr_start;
		else
			priv->rxcr = rx_ring_ptr;
	}

	if(priv->rx_burstRCRcount > BURST_THRESHOLD)
	{
		/* Setup the addresses for the hardware to use */
		priv->rx_burstbufstart= (rx_fbld_t*) virt_to_phys(priv->rx_burstbufstart);
		priv->rx_burstbufstop = (rx_fbld_t*) virt_to_phys(priv->rx_burstbufstop);

		/* Verify that the hardware is ready to send the buffers */
		while( ( VMAC_FBLADDCNT(dev->base_addr) & FBL_RDY ) == 0 ); /* Wait for ready */
//			udelay(1);

		/* Free RCR Entry */
		VMAC_RCRENTINC(dev->base_addr) = priv->rx_burstRCRcount;

		/* return buffers to the Free buffer list */
		VMAC_FBLADDCNT(dev->base_addr) = priv->rx_burstbufcount;
		VMAC_FBLADDSTART(dev->base_addr) = (u32)priv->rx_burstbufstart;
		VMAC_FBLADDEND(dev->base_addr) = (u32)priv->rx_burstbufstop;

		/* Reset the "queue" */
		priv->rx_burstRCRcount=0;
		priv->rx_burstbufcount=0;
		priv->rx_burstbufstart=0;
		priv->rx_burstbufstop=0;
	}

        if(CFG_RX_NUM_BUF_SERVICE >= num_entries)
            return 0;

        else
            return 1; 

//	printk("RX num_called = %d\n",num_called);
} /* vmac_rxirq */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_rcra_thresh(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Rx Completion Ring Threshold Reached 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_rcra_thresh(struct net_device *dev)
{
//	VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;
 
	VMAC_INTSTS(dev->base_addr) = RCRA_THRESH;  // Needs to be cleared in VMAC
	DPRINTK(5,"rcra_thresh interrupt\n");
} /* vmac_rcra_thresh */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_rcr_freeze(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Rx Completion Ring Freeze 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_rcr_freeze(struct net_device *dev)
{
//	VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;

	VMAC_INTSTS(dev->base_addr) = RCR_FREEZE;  // Needs to be cleared in VMAC
	DPRINTK(5,"rcra_freeze interrupt\n");
} /* vmac_rcr_freeze */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_linkirq(int irq, void *dev_id, struct pt_regs *reg)
 *
 *  Description: Avalanche VMAC link change interrupt
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_linkirq(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev;
	VmacPrivateInfo *priv;

	dev = (struct net_device *) dev_id;
	priv = (VmacPrivateInfo *) dev->priv;

	VMAC_INTSTS(dev->base_addr) = LINK_CHANGE;  // Needs to be cleared in VMAC

	DPRINTK(1,"An ethernet link change interrupt has ocurred\n");
} /* vmac_linkirq */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_mgtirq(int irq, void *dev_id, struct pt_regs *reg)
 *
 *  Description: Avalanche VMAC management interrupt
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_mgtirq(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev;
	VmacPrivateInfo *priv;

	dev = (struct net_device *) dev_id;
	priv = (VmacPrivateInfo *) dev->priv;

	VMAC_INTSTS(dev->base_addr) = LNKEVT;  // Needs to be cleared in VMAC

	DPRINTK(5,"An ethernet management interrupt has ocurred\n");
} /* vmac_Mgtirq */


 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    *
    *  Function: int vmac_Initrxcomp(struct atm_dev *dev, int num)
    *
    *  Description: Initialize receive completion ring
    *
    *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static int __init
vmac_Initrxcomp(struct net_device *dev, int num)
  {
  int  i,j;
  unsigned long iTmp;
  void *pTmp; 
  VmacPrivateInfo *priv;
  
  priv = (VmacPrivateInfo *) dev->priv;
  
  //Make num power of two
  num *= 2;
  num--;

  for(i = 0,j = 0x80000000; i < 32 ; i++, j >>= 1)
    if (j & num) 
      num &= j;

  
  /* Allocate memory so that it is on a power of size alignment */

  iTmp = num * 16 * 2;
  pTmp = m_alloc(iTmp,15); /* Align on a 2^10 boundary TBD */
  dma_cache_wback_inv((unsigned long) pTmp, iTmp); /* Invalidate this memory */
  priv->rxcr = (rx_compring *)pTmp;	           /* software pointer to rxcr */
  priv->rxcr_start = priv->rxcr;                   /* first structure of completion ring */
  priv->rxcr_end = (priv->rxcr+(num-1));           /* last structure of completion ring  */

  memset(priv->rxcr, 0, iTmp);  /* Clear memory */

  /* allocate Rx completion ring (num entries) */

  VMAC_RCRPTR(dev->base_addr) = virt_to_bus(priv->rxcr);
  VMAC_RCRSIZE(dev->base_addr) = num - 1;
  VMAC_RCRTOTENT(dev->base_addr) = num;
  VMAC_RCRENTINC(dev->base_addr) = num;
  
  return 0;

} /* End init_rxcomp() */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_InitRXFBL(struct net_dev *dev, int size, int buffnum)
 *
 *  Description: Initialize the Rx free buffer list.  The pointers
 *               for this buffer are maintained by the Avalanche EMAC.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int
vmac_InitRxFBL(struct net_device *dev, int size)
{
  VmacPrivateInfo *priv;
  int i;

  priv = (VmacPrivateInfo *)dev->priv;

  priv->rxfbl_initialized = 1;           /* Set flage to initialize Once */

  /* set Rx Buffer Size */
  VMAC_FBL0BUFSIZE(dev->base_addr) = size;

  /* Free the RxFBL for input */
  for(i=0; i < NUM_RX_BUF; i++){  /* Rx_enable in DMA State Machine must be enabled */

     while( ( VMAC_FBLADDCNT(dev->base_addr) & FBL_RDY ) == 0 ) /* Wait for ready */
      udelay(1);

    VMAC_FBLADDCNT(dev->base_addr) = 1;
    VMAC_FBLADDSTART(dev->base_addr) = virt_to_bus( &priv->rxfbl[i] );
    VMAC_FBLADDEND(dev->base_addr)   = virt_to_bus( &priv->rxfbl[i] );
  }
  
  return 0;
}


static int vmac_irq_request(struct net_device *dev)                                       
{                                                                                         
        VmacPrivateInfo *tp = (VmacPrivateInfo *)dev->priv;                               
                                                                                          
        tp->txfbl_lock = SPIN_LOCK_UNLOCKED;                                              
                                                                                          
        /* Select the proper irq lines based on emac devices */                           
        switch(tp->vmac_num)                                                              
        {                                                                                 
                case 0:                                                                   
                        /* Interrupt line # for EMAC A */                                 
                        tp->tx_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_TX_INT );                      
                        tp->rx_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_RX_INT );                      
                        tp->lnk_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_LNK_CHG_INT );            
                        tp->mgt_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_MGT_INT );                
                        break;                                        
#if 0                    
                case 1:                                                                   
                        /* Interrupt line # for EMAC B */                                 
                        tp->tx_irq = LNXINTNUM( AVALANCHE_MAC_TXB );                      
                        tp->rx_irq = LNXINTNUM( AVALANCHE_MAC_RXB );                      
                        tp->lnk_irq = LNXINTNUM( AVALANCHE_ETH_MACB_LNK_CHG );            
                        tp->mgt_irq = LNXINTNUM( AVALANCHE_ETH_MACB_MGT );                
                        break;   
#endif                                                         
                default:                                                                  
                        printk(VMAC_ERROR "IRQ_REQUEST: invalid vmac number\n");          
                        return 0;                                                         
        }                                                                                 
                                                                                          
        tasklet_init(&tp->vmac_tasklet,vmac_rx_tasklet,(unsigned long)dev);
        
        /* Register Ethernet Transmit interrupt  */                                       
        if(request_irq(tp->tx_irq, vmac_HandleTxInt, SA_INTERRUPT, VmacSignature, dev))   
                printk(VMAC_ERROR "Could not register vmac_tx_irq\n");                    
                                                                                          
        /* Register Ethernet Receive interrupt  */                                        
        if(request_irq(tp->rx_irq, vmac_HandleRxInt, SA_INTERRUPT, VmacSignature, dev))   
                printk(VMAC_ERROR "Could not register vmac_rx_irq\n");                    
                                                                                          
        /* Register Ethernet link change interrupt  */                                    
        if(request_irq(tp->lnk_irq, vmac_linkirq, SA_INTERRUPT, VmacSignature, dev))      
                printk(VMAC_ERROR "Could not register vmac_link_irq\n");                  
                                                                                          
        /* Register Ethernet management change interrupt  */                              
        if(request_irq(tp->mgt_irq, vmac_mgtirq, SA_INTERRUPT, VmacSignature, dev))       
                printk(VMAC_ERROR "Could not register vmac_link_irq\n");                  
           
                                                                               
        return 0;                                                                         
}                                                                                         


static void vmac_free_irq(struct net_device *dev)
{
	VmacPrivateInfo *tp;

	tp = (VmacPrivateInfo *)dev->priv;
	
        free_irq( tp->tx_irq, dev );
	free_irq( tp->rx_irq, dev );
	free_irq( tp->lnk_irq, dev );
	free_irq( tp->mgt_irq, dev );
}


/* for mib2 */                                                     
static void vmac_mib2_refresh(unsigned long device)                
{                                                                  
        struct net_device *dev = (struct net_device *)device;      
        VmacPrivateInfo  *tp;                                      
	int flags;             
    
        tp = (VmacPrivateInfo *) dev->priv;                        
	
	spin_lock_irqsave(&tp->stats_lock, flags);
        vmac_refresh_stats(dev);                  
                 
	if(tp->stats_tmr_run) {
            tp->stats_timer.expires = jiffies + VMAC_MIB_REFRESH_PERIOD; 
            add_timer(&tp->stats_timer);
	}                               
	spin_unlock_irqrestore(&tp->stats_lock, flags);
                                                                   
                                                                   
}                                                                  
                                                                   



/*  This is a test function only */
int printtcb(struct net_device *dev)
{
  VmacPrivateInfo  *priv;

  tx_fbld_t *fbl_ptr;
  
  priv = (VmacPrivateInfo *) dev->priv;

  fbl_ptr = priv->txfbl_head;
  
  if(fbl_ptr != NULL)
    {
      do
        {
          printk("tx_fbld_ptr = %p\n", fbl_ptr);
          fbl_ptr = fbl_ptr->next;
        }while(fbl_ptr != NULL);      
    }
  
  return 0;
  
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_init_one(struct atm_dev *dev)
 *
 *  Description: Device Initialization
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int __init
vmac_init_one(struct net_device *dev)
{
	VmacPrivateInfo  *tp;
	struct timer_list	*tic_timer;
	u32			phy_mode;
	u32			cpu_freq;
	char			*cpu_freq_ptr;

	tp = (VmacPrivateInfo *) dev->priv;

	printk("Texas Instruments VMAC driver  ver:[%s]\n",vmac_version);
	if (dev == NULL)
	{
		printk (VMAC_ERROR "no valid dev structure\n");
		return -ENODEV;
	}

	/* Get CPU Frequency */  
	cpu_freq_ptr = prom_getenv("cpufrequency");
	if(!cpu_freq_ptr) {
		printk("Couldn't find cpufrequency environment variable\n");
		cpu_freq = CONFIG_CPU_FREQUENCY_AVALANCHE * 1000000 ;
	} else {
		cpu_freq = simple_strtol(cpu_freq_ptr,NULL,0);
	}

	if(vmac_InitTxBuffdesc(dev) < 0)		       	/* Initialize Transmit Buffer descripters */
		return -ENODEV;			      		/* Host only */
	if(vmac_InitRxBuffdesc(dev) < 0)		       	/* Initialize Receive Buffer descripters */
		return -ENODEV;		      			/* Host and Vmac */
	if(vmac_reset(dev) != 0)				/* Take Vmac out of reset,Reset MAC, Reset Phy */
		return -ENODEV;					/* Must preceed Tx/Rx completion ring inits    */
	if (vmac_Inittxcomp (dev, NUM_TX_COMP) < 0)		/* Initialize Transmit completion ring */
		return -ENODEV;
	if (vmac_Initrxcomp (dev, NUM_RX_COMP) < 0)		/* Initialize Receive completion ring */
		return -ENODEV;

	/* Set MAC Control register. This will be done in the state change function
	 * too
	 */
//	VMAC_MACCONTROL(dev->base_addr) = CEF | CAF | NO_LOOP | SPEED_100 | CLRP | NO_CHAIN ;
	VMAC_MACCONTROL(dev->base_addr) = CEF | NO_LOOP | SPEED_100 | CLRP | NO_CHAIN ;

	/* Setup MDIO library */
	EmacMdioInit(dev->base_addr, &(tp->phy_state), cpu_freq, 0);

	/* Set the phy to autonegotiate with the complete range */
	phy_mode= NWAY_FD100 | NWAY_HD100 | NWAY_FD10 | NWAY_HD10 | NWAY_AUTO;
	EmacMdioSetPhyMode(dev->base_addr, &(tp->phy_state), phy_mode);

	/* Determine the Phy address */
	tp->phy_addr = EmacMdioGetPhyNum(dev->base_addr, &(tp->phy_state));

	/* Initilize MDIO tic timer */
	tic_timer=&(tp->mdio_tic_timer);
	init_timer(tic_timer);
	tic_timer->function = vmac_do_mdio_tic;			/* Assign function to call */
	tic_timer->data = (unsigned long) dev;			/* Timer gets dev pointer to access data */
	tic_timer->expires = jiffies + MDIO_TIC_DELAY + 30;	/* Delay the first tic to let things ralax */
	tp->mdio_tic_run = 1;
	tp->mdio_tic_lock = RW_LOCK_UNLOCKED;
	add_timer(tic_timer);					/* Start the timer going */



	/* Initilize stats  timer */
	tic_timer=&(tp->stats_timer);
	init_timer(tic_timer);
	tic_timer->function = vmac_mib2_refresh;			/* Assign function to call */
	tic_timer->data = (unsigned long) dev;			/* Timer gets dev pointer to access data */
	tic_timer->expires = jiffies + VMAC_MIB_REFRESH_PERIOD;	/* Delay the first tic to let things ralax */
	tp->stats_tmr_run = 1;
	tp->stats_tmr_lock = RW_LOCK_UNLOCKED;
	add_timer(tic_timer);					/* Start the timer going */



	VMAC_INTSTS(dev->base_addr) = 0x3ff;			/* Clear all interrupts */

	tp->nextDevice = VmacDevices;
	VmacDevices = dev;      
	VmacDevicesInstalled++;

        DPRINTK(2,"Leaving vmac_init_one\n");
	return 0;
}  /* vmac_init_one */


static int
vmac_Open(struct net_device *dev )
{
	VmacPrivateInfo *priv;
	int i;
	unsigned char mac_addr[ETH_ALEN];
	char *mac_name=NULL;
	char *mac_string=NULL;
	mac_addr_t mac_addr_hi, mac_addr_low;

	priv = (VmacPrivateInfo *) dev->priv;

	DPRINTK(1,"Entering Vmac_Open (\n");

	/* Assign MAC Address currently stored in adam2 environment variable */
	switch(priv->vmac_num)
	{
		case 0:
			mac_name="maca";
			break;
		case 1:
			mac_name="macb";
			break;
	}

	if(mac_name)
		mac_string=prom_getenv(mac_name);

	if(!mac_string)
	{
		mac_string="08.00.28.32.06.02";
		printk(VMAC "Error getting mac from Adam enviroment for %s\n",dev->name);
		printk(VMAC "Using default mac address: %s\n",mac_string);
		if(mac_name)
		{
			printk(VMAC "Use Adam2 command:\n");
			printk(VMAC "    setenv %s xx.xx.xx.xx.xx.xx\n",mac_name);
			printk(VMAC "to set mac address\n");
		}
	}

	str2eaddr(mac_addr,mac_string);
	for (i=0; i <= ETH_ALEN; i++)
	{
		dev->dev_addr[i] = mac_addr[i];					/* This sets the hardware address */
	}

	mac_addr_hi.byte4 = mac_addr[0];      /* High Order Byte of MAC address */
	mac_addr_hi.byte3 = mac_addr[1];
	mac_addr_hi.byte2 = mac_addr[2];
	mac_addr_hi.byte1 = mac_addr[3];

	mac_addr_low.byte2 = mac_addr[4];
	mac_addr_low.byte1 = mac_addr[5];     /* Low Order Byte of MAC address */

	/* Store the MAC address to the device registers */
	VMAC_MACADDRHI(dev->base_addr) = (mac_addr_hi.byte1<<24)|(mac_addr_hi.byte2<<16)
		|(mac_addr_hi.byte3<<8)|mac_addr_hi.byte4;

	VMAC_MACADDRLO(dev->base_addr) = (mac_addr_low.byte1<<8)|mac_addr_low.byte2;


	/* Do Rx enable */
	/* Configure the Rx DMA state machine */
	VMAC_RDMASTATE0(dev->base_addr) = 0x00000200;

	/* Do Tx enable */ 
	/* Set maximum Tx DMA burst length to 16 words */
	VMAC_TDMASTATE7(dev->base_addr) = 0x00100000;

	/* Enable Rx and Tx DMA */
	VMAC_DMACONFIG(dev->base_addr) = TX_ENABLE|RX_ENABLE;

	/* Initialize Rx FBL, Must be done with Rx DMA enabled */
	/* Check to see if Rx FBL already initialized */
	if(!priv->rxfbl_initialized)
	{
		if(vmac_InitRxFBL(dev, MAX_FRAME_SIZE) < 0)
			return -ENODEV;
	}

	/* Set ISR Pacing */
	VMAC_TXISRPACE(dev->base_addr) = isrpace;  // 2 is optimal. 0 should give 1 int/frame.
	VMAC_RXISRPACE(dev->base_addr) = isrpace;  // Currently not documented in spec.

	/* Request IRQ's */
	if(vmac_irq_request(dev) < 0)  /* request interrupt */
		return -EBUSY;

	/* Set Transmit Threshold Level */
	VMAC_TCRINTTHRESH(dev->base_addr) = TRANSMIT_INT_THRESH;

	/* Enable Interrupts via Interrupt Mask Register */
//	priv->int_mask = POSTED_RCRA | TCRA_THRESH;
	
	priv->int_mask = POSTED_RCRA | POSTED_TCRA | TCRA_THRESH;
	VMAC_INTMASK(dev->base_addr) = priv->int_mask;

	/* Clear Interrupt Status Register */      
	VMAC_INTSTS(dev->base_addr) = 0x3ff;

	MOD_INC_USE_COUNT;

	DPRINTK(1,"Leaving Vmac_Open  \n");
	return 0;

} /* vmac_Open */


static int
vmac_Close(struct net_device *dev)
{
  
  DPRINTK(1,"Entering Vmac_Close \n");
  
  vmac_free_irq(dev);
  
  MOD_DEC_USE_COUNT;

  DPRINTK(1,"Leaving Vmac_Close \n");

  return 0;

} /* vmac_Close */

/* TODO Add netif_stop_queue support */
/* TODO Rewrite Get_TxDesc to be better */
static int
vmac_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	VmacPrivateInfo *tp = (VmacPrivateInfo *) dev->priv;
	tx_fbld_t *tx_desc;
	u32 count=0;
	static int num_called=0;

	if(tp->txfbl_num < 4)
	{
		/* The host has run out of transmit buffer descriptors */
		printk(KERN_ERR "VMAC: start_xmit: No buffers for transmit\n");
		tp->mib_counters.outDiscardPkts++;
		return 1;
	}
	num_called++;

//printk("\nTxPacket = %d, len=%d\n", num_called, skb->len);
//xdump(skb->data, skb->len, "TxPacket: ");

	tx_desc=Get_TxDesc(tp);

	tx_desc->mode = 0xc0000000 | (u16)(skb->len + 4);
	tx_desc->BufPtr=(unsigned char *)virt_to_bus(skb->data);
	tx_desc->offset = (u32)(skb->data) & 0x03;
	tx_desc->skb = skb;

	while(( ( VMAC_TXPADDCNT(dev->base_addr) & 0x80000000 ) == 0 ) && count < 10000)
	{
		count++;
		udelay(1);
	}
	if( count >0 ) {
		printk(KERN_ERR "TX Wait %d\n",count);
		memset(tx_desc, 0, 16);
		Put_TxDesc( tx_desc, tp);
		tp->mib_counters.outDiscardPkts++;
		return 1;
	}


	VMAC_TXPADDCNT(dev->base_addr) = 0x00000001;
	VMAC_TXPADDSTART(dev->base_addr) = virt_to_bus( tx_desc );
	wmb();
	VMAC_TXPADDEND(dev->base_addr)  = virt_to_bus( tx_desc );

	                                                                       
       if(!(*skb->data & 0x01) )                                       
               tp->mib_counters.outUnicastPktsLow++; //unicast  packet 

//	printk("Transmit Num called = %d\n", num_called);

	return 0;
}

static int 
vmac_proc_version(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len=0;
  
  len += sprintf(buf+len, "Texas Instruments VMAC driver version: %s\n", vmac_version);
  return len;
}


static int 
vmac_link_status(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  
  struct net_device *dev;
  VmacPrivateInfo *priv;
  struct net_device  * vmac_dev_list[num_devices];
  
  int i;
  int					linkstate;	/* returns 0 for no link, !0 for link */
  int					linkspeed;	/* returns 0 for 10Mbs, !0 for 100Mbs */
  int					linkduplex;	/* returns 0 for half-duplex */
  int					phy;		/* what phy are we using? */
    
        len += sprintf(buf+len, "VMAC devices = %d\n",VmacDevicesInstalled);

  dev = VmacDevices;  
  priv = (VmacPrivateInfo *) dev->priv;
  
  /* Reverse the the device link list to list eth0,eth1...in correct order */
  for(i=0; i<num_devices; i++)
    {
      vmac_dev_list[num_devices-(i+1)]=dev;
      dev = priv->nextDevice;
    }
        
  for(i=0;i<num_devices; i++)
    {
      dev = vmac_dev_list[i];
      
      priv = (VmacPrivateInfo *) dev->priv;

      /* Link up, check the states */
      linkstate=EmacMdioGetLinked(dev->base_addr, &(priv->phy_state));
      linkspeed=EmacMdioGetSpeed(dev->base_addr, &(priv->phy_state));
      linkduplex=EmacMdioGetDuplex(dev->base_addr, &(priv->phy_state));
      phy= EmacMdioGetPhyNum(dev->base_addr, &(priv->phy_state));

      /*  This prints them out from high to low because of how the devices are linked */
      if(linkstate)
        len += sprintf(buf+len,"eth%d: Link State: %s    Phy %d, Speed = %s, Duplex = %s\n",
                       priv->vmac_num,(linkstate)?"UP":"DOWN", 
                       phy,(linkspeed)?"100":"10",(linkduplex)?"Full":"Half");
      else
        len += sprintf(buf+len,"eth%d: Link State: DOWN\n",priv->vmac_num);
      
      dev = priv->nextDevice;
    }
  
  return len;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: void vmac_init_module(void)
 *
 *  Description: Avalanche VMAC entrance function
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int __init
vmac_init_module(void)
{
	struct net_device *dev;
	VmacPrivateInfo *priv;
	int result;  
	size_t dev_size;
	int i;

	DPRINTK(2,"Entering vmac_init_module\n");
        
	/* Setup proc entry */
	gp_stats_file = create_proc_entry("avalanche/vmac_stat",0644,NULL);

        if(gp_stats_file)
        {
            gp_stats_file->read_proc  = vmac_read_proc;
            gp_stats_file->write_proc = vmac_write_proc;
        }        

        create_proc_read_entry("avalanche/vmac_ver" ,0,NULL,vmac_proc_version,NULL);
        create_proc_read_entry("avalanche/vmac_link" ,0,NULL,vmac_link_status,NULL);

	/* Set up Pad Buffer */
	VmacPadBuffer = (u8 *) kmalloc( ETH_ZLEN, ( GFP_KERNEL | GFP_DMA ));
	if ( VmacPadBuffer == NULL ) 
	{
		printk( VMAC " Could not allocate memory for pad buffer.\n" );
		return -ENOMEM;
	}

	dev_size = sizeof(struct net_device) + sizeof(VmacPrivateInfo);

	/* VW_TODO: Need a discovery mechanism,EEPROM? */
	for(i = 0; i < num_devices; i++)
	{
		dev = (struct net_device *) kmalloc(dev_size, GFP_KERNEL);
		if (dev == NULL)
		{
			printk (VMAC_ERROR "unable to malloc for vmac\n");
			return -ENODEV;
		}

		/* Initialize the device structur */
		memset(dev, 0, dev_size);

		dev->priv = ((void *) dev) + sizeof(struct net_device);
		priv = (VmacPrivateInfo *) dev->priv;
		priv->vmac_num = i;
                priv->rxfbl_initialized = 0;
		priv->good_tx_sw_frames=0;
		priv->good_rx_sw_frames=0;

		/* Device Setup */  
		ether_setup(dev);								/* assign some of the fields */ 
		dev->addr_len = 6;								/* Hardware Address Length in Bytes */ 
		dev->open = &vmac_Open;
		dev->do_ioctl = vmac_ioctl;
		dev->hard_start_xmit = &vmac_start_xmit;
		dev->stop = &vmac_Close;
		dev->get_stats = &vmac_get_stats;				/*&Vmac_GetStats*/
		/* dev->set_multicast_list = &Vmac_SetMulticastList; */
		dev->base_addr = io_base[priv->vmac_num]; // io_base[] is a static array of base offsets

		dev->init=vmac_init_one;

		result = register_netdev(dev); /*register_netdev calls vmac_Init */
		if(result) 
		{
			printk(VMAC_ERROR "Vmac: Could not register device.\n");
			kfree(dev);      
		} 
		else
		{
			printk("VMAC: %s installed \n",dev->name);
		}
    }
	DPRINTK(2,"Leaving vmac_init_module\n");
	DPRINTK(2,"Found %d device(s).\n", VmacDevicesInstalled);

	return ((VmacDevicesInstalled >=0) ? 0:-ENODEV);
} /* vmac_init_module */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: void vmac_probe(strict net_device *dev)
 *
 *  Description: when build into the kernel, inits the devices
 *		basically, just calles the init modules stuff from above
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//extern int vmac_probe(struct net_device *dev);
int __init vmac_probe(struct net_device *dev)
{
	static int probe_count=0;
	VmacPrivateInfo *tp;
	int result;  
	int returnval=-ENODEV;

	if(probe_count==0){
		/* Setup proc entry */
		create_proc_read_entry("avalanche/vmac_stat",0,NULL,vmac_read_proc,NULL);
		create_proc_read_entry("avalanche/vmac_ver",0,NULL,vmac_proc_version,NULL);
		create_proc_read_entry("avalanche/vmac_link" ,0,NULL,vmac_link_status,NULL);
	}

	if(probe_count >= num_devices){		/* All done, stop autoprobe */
		return -ENODEV;
	}

	/* Alloc priv struct */
	tp=kmalloc(sizeof(VmacPrivateInfo),GFP_KERNEL);
	if(!tp){
		printk(VMAC_ERROR "unable to kmalloc priv structure. Killing autoprobe.\n");
		return -ENODEV;
	}
	memset(tp, 0, sizeof(VmacPrivateInfo));
	dev->priv=tp;
	tp->vmac_num=probe_count;				/* Assign interface number */

	/* Device Setup */  
	ether_setup(dev);					/* assign some of the fields */ 
	dev->addr_len = 6;					/* Hardware Address Length in Bytes */ 
	dev->open = &vmac_Open;
	dev->hard_start_xmit = &vmac_start_xmit;		/* &vmac_dTx; */
	dev->stop = &vmac_Close;
	dev->get_stats = vmac_get_stats;			/*&Vmac_GetStats*/
	dev->set_multicast_list = vmac_set_rx_mode;
	dev->do_ioctl = vmac_ioctl;
	dev->base_addr = io_base[tp->vmac_num];

	result = vmac_init_one(dev);
	if(result == 0){
		printk(VMAC "found VMAC #%d \n", probe_count);
		returnval = 0;
	}
	probe_count++;						/* Increment probe count for next pass */

	return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: void vmac_cleanup_module(void)
 *
 *  Description: Avalanche VMAC exit function
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void 
vmac_cleanup_module(void)
{
  struct net_device *dev;
  VmacPrivateInfo *priv;
  tx_fbld_t *temp_ptr;
//  int i;
  
  DPRINTK(1,"Entering vmac_cleanup_module\n");  

  while(VmacDevicesInstalled)
    {
      dev = VmacDevices;
      priv = (VmacPrivateInfo *) dev->priv;

      /* Free Tx buffer list */
      DPRINTK(5,"freeing up Tx descriptor list..\n");
      while(priv->txfbl_head->next != priv->txfbl_tail)
        {
          temp_ptr = priv->txfbl_head->next;
          kfree(priv->txfbl_head);
          priv->txfbl_head = temp_ptr;
        }
      kfree(priv->txfbl_head);   /* free the last node */

      /* Free Rx buffer associated with each descriptor */
      DPRINTK(5,"freeing up Rx data buffer buffers..\n");

      /* TODO: Must add code to clean up data pointers here */

//	  skb_pool=priv->skbpool;

//      for (i = 0; i < NUM_RX_BUF; i++)
//       {
//         kfree(phys_to_virt(priv->rxfbl[i].BufPtr));        
//		 vmac_kfree_skb_any(skb_pool[i].skb_address);
//       }     

      /* Free Rx descriptor list */
      DPRINTK(5,"freeing up Rx descriptor list..\n");
      kfree(priv->rxfbl);
//	  kfree(priv->skbpool);
      
      DPRINTK(5,"freeing up Tx completion ring..\n");
      m_free(priv->txcr_start,(NUM_TX_COMP*16*2));       /* free Tx completion ring */
      /* VW_TODO: fix rxcr cleanup problem */
      //      printk(VMAC "freeing up Rx completion ring..\n");   
      //      m_free(priv->rxcr,(NUM_RX_COMP*16*2));             /* free Rx completion ring */
      
	  /* Cleanup mdio_tic_timer */
	  DPRINTK(5,"Stopping mdio tic timer..");
	  write_lock_irq(&(priv->mdio_tic_lock));
	  priv->mdio_tic_run=0;
	  write_unlock_irq(&(priv->mdio_tic_lock));
	  del_timer_sync(&(priv->mdio_tic_timer));
	  DPRINTK(5,"..Done\n");


	  /* Cleanup stats_timer */
	  DPRINTK(5,"Stopping stats timer..");
	  write_lock_irq(&(priv->stats_tmr_lock));
	  priv->stats_tmr_run=0;
	  write_unlock_irq(&(priv->stats_tmr_lock));
	  del_timer_sync(&(priv->stats_timer));
	  DPRINTK(5,"..Done\n");

      unregister_netdev(dev);
      VmacDevices = priv->nextDevice;
      kfree(dev);                                       /* free dev & dev->priv mem */
      VmacDevicesInstalled--;
    }

  /* remove proc entry */
  remove_proc_entry("vmac_stat",NULL);
  remove_proc_entry("avalanche/vmac_stat",NULL);
  remove_proc_entry("avalanche/vmac_ver",NULL);
  remove_proc_entry("avalanche/vmac_link",NULL);
  
  DPRINTK(1,"Leaving vmac_cleanup_module\n");

  return;
} /* cleanup_module */

static void vmac_set_rx_mode (struct net_device *dev)
{
	VmacPrivateInfo *tp = dev->priv;

	/* Dummy things to remove compiler warnings */
	tp=NULL;

	if(dev->flags & IFF_PROMISC){ /* Promisc mode */
        	VMAC_MACCONTROL(dev->base_addr) |= CAF;
	} else {
        	VMAC_MACCONTROL(dev->base_addr) &= ~CAF;
	}
}


static int vmac_ioctl (struct net_device *dev, struct ifreq *rq, int cmd)
{
	VmacPrivateInfo *priv = dev->priv;
	unsigned long flags;
	int ret = 0;
        TI_SNMP_CMD_T ti_snmp_cmd;



        if(cmd == SIOCDEVPRIVATE)
        {
            /* Now copy the user data */
            if(copy_from_user((char *)&ti_snmp_cmd,(char *)rq->ifr_data, sizeof(TI_SNMP_CMD_T)))
                return -EFAULT;
	   
            

            switch (ti_snmp_cmd.cmd) {
	        case SIOCGINTFCOUNTERS:
	            /* update vmac statistics */       		
	            spin_lock_irqsave(&priv->stats_lock, flags);
	            vmac_refresh_stats(dev); 
	            spin_unlock_irqrestore(&priv->stats_lock, flags);
            
                    /* Now copy the dev counters to the user data */
                    if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&priv->mib_counters, sizeof(struct mib2_ifCounters)))
		        ret = -EFAULT;
                break;

	       case SIOCGINTFPARAMS:
	       {
	           struct mib2_ifParams    localParams;
            
                   if(EmacMdioGetSpeed(dev->base_addr, &(priv->phy_state)))
	               localParams.ifSpeed = 100000000;
	           else
		       localParams.ifSpeed = 10000000;		

                   localParams.ifHighSpeed = (localParams.ifSpeed)/1000000;
                        
	           if(dev->flags & IFF_UP)	
	               localParams.ifOperStatus = MIB2_STATUS_UP;
	           else
	               localParams.ifOperStatus = MIB2_STATUS_DOWN;


                   if(dev->flags & IFF_PROMISC)
                       localParams.ifPromiscuousMode           = TRUE;
                   else
                       localParams.ifPromiscuousMode           = FALSE;
            
	           /* Now copy the counters to the user data */
                   if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&localParams, sizeof(struct mib2_ifParams)))
		       ret = -EFAULT;
	       }
               break; 
	
	      case  SIOCGETHERCOUNTERS:
	      {
	          struct mib2_phyCounters localCounters;
 	          struct vmac_stat_regs *vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);
                  localCounters.ethAlignmentErrors        = vmac_stats->rx_align_code_errors;
                  localCounters.ethFCSErrors              = vmac_stats->rx_crc_errors; 
                  localCounters.ethSingleCollisions       = vmac_stats->single_col_tx_frames;
                  localCounters.ethMultipleCollisions     = vmac_stats->mul_col_tx_frames;
                  localCounters.ethSQETestErrors          = vmac_stats->sqe_test_errors;
                  localCounters.ethDeferredTxFrames       = vmac_stats->deferred_tx_frames;
                  localCounters.ethLateCollisions         = vmac_stats->late_collisions;
                  localCounters.ethExcessiveCollisions    = vmac_stats->excessive_collisions;
                  localCounters.ethInternalMacTxErrors    = vmac_stats->tx_error_frames;
                  localCounters.ethCarrierSenseErrors     = vmac_stats->carrier_sense_errors; 
                  localCounters.ethTooLongRxFrames        = vmac_stats->oversized_rx_frames;
                  localCounters.ethInternalMacRxErrors    = priv->mib_counters.inErrorPkts;
                  localCounters.ethSymbolErrors           = 0;
	    
	          /* Now copy the counters to the user data */
                  if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&localCounters, sizeof(struct mib2_phyCounters)))
		      ret = -EFAULT;
	    }
	    break;
	
	   case SIOCGETHERPARAMS:
	   {
	       struct mib2_ethParams localParams;
            
               if(EmacMdioGetDuplex(dev->base_addr, &(priv->phy_state)))
                   localParams.ethDuplexStatus = MIB2_FULL_DUPLEX;
	       else
                   localParams.ethDuplexStatus = MIB2_HALF_DUPLEX;
 
               /* Now copy the counters to the user data */
               if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&localParams, sizeof(struct mib2_ethParams)))
	           ret = -EFAULT;

	    }
	    break;

	    default:
		ret = -EOPNOTSUPP;
	    break;
	}
    }
    else
        ret = -EOPNOTSUPP;

    return ret;
}

static void *m_alloc(int size, int a_shift)
{
	u_long addr;
	void *ptr;
	u_long a_size, a_mask;

	if (a_shift < 3)
		a_shift = 3;

	a_size	= ALIGN_SIZE(a_shift);
	a_mask	= ALIGN_MASK(a_shift);

        ptr = (void *) kmalloc(size + a_size, GFP_ATOMIC); 
	if (ptr) {
		addr	= (((u_long) ptr) + a_size) & a_mask;
		*((void **) (addr - sizeof(void *))) = ptr;
		ptr	= (void *) addr;
	}
	return ptr;
}

static void m_free(void *ptr, int size)
{
	u_long addr;

	if (ptr) {
		addr	= (u_long) ptr;
		ptr	= *((void **) (addr - sizeof(void *)));
		kfree(ptr);
	}
}


#define isprint(a) ((a >=' ')&&(a <= '~')) 
#if 0
int tcnter = 0, rcnter = 0;
void xdump( u_char*  cp, int  length, char*  prefix )
{
    int col, count;
    u_char prntBuf[120];
    u_char*  pBuf = prntBuf;
    count = 0;
    while(count < length){
        pBuf += sprintf( pBuf, "%s", prefix );
        for(col = 0;count + col < length && col < 16; col++){
            if (col != 0 && (col % 4) == 0)
                pBuf += sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "%02X ", cp[count + col] );
        }
        while(col++ < 16){      /* pad end of buffer with blanks */
            if ((col % 4) == 0)
                sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "   " );
        }
        pBuf += sprintf( pBuf, "  " );
        for(col = 0;count + col < length && col < 16; col++){
            if (isprint((int)cp[count + col]))
                pBuf += sprintf( pBuf, "%c", cp[count + col] );
            else
                pBuf += sprintf( pBuf, "." );
                }
        sprintf( pBuf, "\n" );
        // SPrint(prntBuf);
        printk(prntBuf);
        count += col;
        pBuf = prntBuf;
    }

}  /* close xdump(... */

#endif /* 0 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: void vmac_do_mdio_tic(unsigned long ptr)
 *
 *  Description: Called by a kernel timer. Calls the MDIO tic function
 *  every jiffie. The documentation states that the tic function should
 *  be called every 10 mSec, but the timer resolution is every 10 mSec.
 *	it should be ok.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void vmac_do_mdio_tic(unsigned long ptr)
{
  unsigned long flags;
	/* priv pointer is passed in as a long */
	struct net_device *dev;
	VmacPrivateInfo *priv;
	rwlock_t		*lock;
	dev = (struct net_device *) ptr;
	priv = (VmacPrivateInfo *) dev->priv;
	lock = &priv->mdio_tic_lock;

spin_lock_irqsave(&priv->tic_lock, flags);
//	outw(0x04, 0xa8612480);
//	xdump(0xa8612480, 4,"Secondary Status: ");
//	printk("vmac_do_mdia_tic\n");
	if( EmacMdioTic(dev->base_addr, &(priv->phy_state)) ) {
		vmac_do_mdio_statechange(dev);
	}
//	read_lock_irq(lock);
	if(priv->mdio_tic_run) {
		priv->mdio_tic_timer.expires=jiffies+MDIO_TIC_DELAY;
		add_timer(&(priv->mdio_tic_timer));
	}
//	read_unlock_irq(lock);
spin_unlock_irqrestore(&priv->tic_lock, flags);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Function: void vmac_do_mdio_statechange(struct net_device *dev)
*
* Description: Called from within the vmac_do_mdio_tic function when
* there is a state change. This will enable and disable the transmit
* functionality so that we don't send packets out of a dead port.
*/
static void vmac_do_mdio_statechange(struct net_device *dev)
{
	int					linkstate;	/* returns 0 for no link, !0 for link */
	int					linkspeed;	/* returns 0 for 10Mbs, !0 for 100Mbs */
	int					linkduplex;	/* returns 0 for half-duplex */
	int					phy;		/* what phy are we using? */
	VmacPrivateInfo		*priv;
	priv = (VmacPrivateInfo *) dev->priv;

	DPRINTK(1,"\n");
	DPRINTK(1,"MDIO statechange\n");
	linkstate=EmacMdioGetLinked(dev->base_addr, &(priv->phy_state));
	if(linkstate == 0)
	{
		/* Report carrier down */
		netif_carrier_off(dev);
		DPRINTK(1,"Link down\n\n");
		return;
	}

	/* Link up, check the states */
	linkspeed=EmacMdioGetSpeed(dev->base_addr, &(priv->phy_state));
	linkduplex=EmacMdioGetDuplex(dev->base_addr, &(priv->phy_state));

	/* Report carrier up */
	netif_carrier_on(dev);

	/* Set MACPHY register. Do we need this? According to the example
	 * in the lib docs, we do. Why doesn't the library do this then? */
	phy= EmacMdioGetPhyNum(dev->base_addr, &(priv->phy_state));
	priv->phy_addr = phy;
	VMAC_MDIOMACPHY(dev->base_addr) = LN | phy;              // 0x00000080

	/* Set the MACCONTROL register. Acording to the lib docs, we have to
	 * set SPEED_100 all the time, and the MAC will determine the correct
	 * speed to the phy. */
//	VMAC_MACCONTROL(dev->base_addr) |= CAF | NO_LOOP|SPEED_100;
	VMAC_MACCONTROL(dev->base_addr) |= NO_LOOP|SPEED_100;

	if( linkduplex )	/* full duplex */
	{
		VMAC_MACCONTROL(dev->base_addr) |= FULL_DUPLEX;
	} else {			/* half duplex */
		/* clear the FULL_DUPLEX bit */
		VMAC_MACCONTROL(dev->base_addr) &= ~FULL_DUPLEX;
	}

	DPRINTK(1,"Link up: Phy %d, Speed = %s, Duplex = %s\n", phy,
			(linkspeed)?"100":"10",(linkduplex)?"Full":"Half");
}

/*********************************************************
* Function: vmac_read_proc
*
* Description: This returns statistics through the proc filesystem.

*/

static int vmac_read_proc(char* buf, char **start, off_t offset, int count,
		int *eof, void *data)
{
    struct net_device *dev;
    int len=0;
    int limit = count - 80;
    int i,j;
    VmacPrivateInfo	*priv;
    struct vmac_stat_regs *vmac_stats;

    for(i=0; i<VmacDevicesInstalled; i++)
    {
        dev = VmacDevices;
        priv = (VmacPrivateInfo *)dev->priv;
        
        /* Reverse List order */
        for(j=i+1;j<VmacDevicesInstalled; j++)
        {
            priv = (VmacPrivateInfo *)dev->priv;
            dev = priv->nextDevice;
        }

        if(!dev)
          goto proc_error;

        /* Get Stats */
        vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);

        /* Transmit stats */
        if(len<=limit)
          len+= sprintf(buf+len, "\nVmac %d, Address %lx\n",i+1,dev->base_addr);
        if(len<=limit)
          len+= sprintf(buf+len, " Transmit Stats\n");
        if(len<=limit)
          len+= sprintf(buf+len, "   Tx Valid Bytes Sent        :%lu\n",vmac_stats->tx_octets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Frames (Hardware)  :%lu\n",vmac_stats->good_tx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Frames (Software)  :%lu\n",(long unsigned int)priv->good_tx_sw_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Broadcast Frames   :%lu\n",vmac_stats->broadcast_tx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Multicast Frames   :%lu\n",vmac_stats->multicast_tx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Pause Frames Sent          :%lu\n",vmac_stats->pause_tx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Collisions                 :%lu\n",vmac_stats->collisions);
        if(len<=limit)
          len+= sprintf(buf+len, "   Tx Error Frames            :%lu\n",vmac_stats->tx_error_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Carrier Sense Errors       :%lu\n",vmac_stats->carrier_sense_errors);
        if(len<=limit)
          len+= sprintf(buf+len, "\n");

        /* Receive Stats */
        if(len<=limit)
          len+= sprintf(buf+len, "\nVmac %d, Address %lx\n",i+1,dev->base_addr);
        if(len<=limit)
          len+= sprintf(buf+len, " Receive Stats\n");
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Valid Bytes Received    :%lu\n",vmac_stats->rx_octets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Frames (Hardware)  :%lu\n",vmac_stats->good_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Frames (Software)  :%lu\n",(long unsigned int)priv->good_rx_sw_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Broadcast Frames   :%lu\n",vmac_stats->broadcast_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Multicast Frames   :%lu\n",vmac_stats->multicast_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Pause Frames Received      :%lu\n",vmac_stats->pause_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx CRC Errors              :%lu\n",vmac_stats->rx_crc_errors);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Align/Code Errors       :%lu\n",vmac_stats->rx_align_code_errors);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Jabbers                 :%lu\n",vmac_stats->rx_jabbers);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Filtered Frames         :%lu\n",vmac_stats->filtered_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Fragments               :%lu\n",vmac_stats->rx_fragments);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Undersized Frames       :%lu\n",vmac_stats->undersized_rx_frames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Overruns                :%lu\n",vmac_stats->rx_overruns);
    }
        
    return len;

 proc_error:
    *eof=1;
    return len;
}


/* Resetting statistics  */
static void vmac_reset_stats(struct net_device *dev)
{
    VmacPrivateInfo	*priv;
    
    priv = (VmacPrivateInfo *)dev->priv;
    priv->good_rx_sw_frames = 0;
    priv->good_tx_sw_frames = 0;
    
    priv->mib_counters.inUnicastPktsLow = 0;
    priv->mib_counters.outUnicastPktsLow = 0;

    
    vmac_refresh_stats(dev);
   
   /* Clearing MIB counters */
    memset(&priv->mib_counters,0,sizeof(struct mib2_ifCounters));


    VMAC_MACCONTROL(dev->base_addr) |= CLRP; 
}




static int vmac_write_proc(struct file *fp, const char * buf, unsigned long count, void * data)
{
	char local_buf[31];
        int  ret_val = 0;

	if(count > 30)
	{
		printk("Error : Buffer Overflow\n");
		printk("Use \"echo 0 > vmac_stat\" to reset the statistics\n");
		return -EFAULT;
	}

	copy_from_user(local_buf,buf,count);
	local_buf[count-1]='\0'; /* Ignoring last \n char */
        ret_val = count;
	
	if(strcmp("0",local_buf)==0)
	{
            struct net_device *dev;
            int i;
            struct net_device  * vmac_dev_list[num_devices];
            VmacPrivateInfo	*priv;

            /* Valid command */
	    printk("Resetting statistics for VMAC interface.\n");

            dev = VmacDevices;  
            priv = (VmacPrivateInfo *) dev->priv;
  
            /* Reverse the the device link list to list eth0,eth1...in correct order */
            for(i=0; i<num_devices; i++)
            {
                vmac_dev_list[num_devices-(i+1)]=dev;
                dev = priv->nextDevice;
            }
        
            for(i=0;i<num_devices; i++)
            {
                dev =  vmac_dev_list[i];
                if(!dev)
                {
                    ret_val = -EFAULT;
                    break;
                }
                vmac_reset_stats(dev);
            }

	}
	else
	{
		printk("Error: Unknown operation on MAC statistics\n");
		printk("Use \"echo 0 > vmac_stat\" to reset the statistics\n");
		return -EFAULT;
	}
	
	return ret_val;
}


/* Refreshing stats to take care of 32 bit wrap-around for MIB counters  */
static void  vmac_refresh_stats(struct net_device *dev)
{
 	VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;
 	struct vmac_stat_regs *vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);
	unsigned int temp;
	static unsigned int prev_rx_unicast_count = 0;
	static unsigned int prev_tx_unicast_count = 0;
	

	temp = vmac_stats->rx_octets;
	if(priv->mib_counters.inBytesLow > temp)
	    priv->mib_counters.inBytesHigh++;

	priv->mib_counters.inBytesLow = temp;

	if(prev_rx_unicast_count > priv->mib_counters.inUnicastPktsLow)
	    priv->mib_counters.inUnicastPktsHigh++;
	
 	prev_rx_unicast_count = priv->mib_counters.inUnicastPktsLow;                                              
	
	temp = vmac_stats->multicast_rx_frames;
	if(priv->mib_counters.inMulticastPktsLow > temp)
	    priv->mib_counters.inMulticastPktsHigh++;
	
	priv->mib_counters.inMulticastPktsLow = temp;                                       

	temp = vmac_stats->broadcast_rx_frames;
	if(priv->mib_counters.inBroadcastPktsLow > temp)
	    priv->mib_counters.inBroadcastPktsHigh++;

	priv->mib_counters.inBroadcastPktsLow = temp;                                             
	
	temp = vmac_stats->tx_octets;
	if(priv->mib_counters.outBytesLow > temp)
	    priv->mib_counters.outBytesHigh++;

	priv->mib_counters.outBytesLow = temp;

	if(prev_tx_unicast_count > priv->mib_counters.outUnicastPktsLow)
	    priv->mib_counters.outUnicastPktsHigh++;
	
 	prev_tx_unicast_count = priv->mib_counters.outUnicastPktsLow;                                              
	
	temp = vmac_stats->multicast_tx_frames;
	if(priv->mib_counters.outMulticastPktsLow > temp)
	    priv->mib_counters.outMulticastPktsHigh++;
	
	priv->mib_counters.outMulticastPktsLow = temp;                                       

	temp = vmac_stats->broadcast_tx_frames;
	if(priv->mib_counters.outBroadcastPktsLow > temp)
	    priv->mib_counters.outBroadcastPktsHigh++;

	priv->mib_counters.outBroadcastPktsLow = temp;                                             
	
	priv->mib_counters.inErrorPkts  =  vmac_stats->rx_fragments
					+ vmac_stats->undersized_rx_frames
	  				+ vmac_stats->rx_crc_errors
	  				+ vmac_stats->rx_align_code_errors
	  				+ vmac_stats->rx_jabbers
	  				+ vmac_stats->rx_overruns;                                               
	priv->mib_counters.inUnknownProtPkts         = 0; /* to be decided */                                                     
	priv->mib_counters.outErrorPkts = vmac_stats->tx_error_frames;                                                               
/* the end */
  
}


static struct net_device_stats *vmac_get_stats (struct net_device *dev)
{
	VmacPrivateInfo *tp = (VmacPrivateInfo *) dev->priv;
	unsigned long flags;

	spin_lock_irqsave(&tp->stats_lock, flags);
	vmac_update_stats(dev);
	spin_unlock_irqrestore(&tp->stats_lock, flags);

	return &tp->stats;
}

static void     vmac_update_stats(struct net_device *dev)
{
 VmacPrivateInfo *priv = (VmacPrivateInfo *) dev->priv;
 struct vmac_stat_regs *vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);


  /* Debug stats Rx Frames */
 /* Drivers stats appears correct, ifconfig & netstat suspect */
  
  /* Debug stats Tx Frames */


	priv->stats.tx_packets=vmac_stats->good_tx_frames;
	priv->stats.rx_packets=vmac_stats->good_rx_frames;
	priv->stats.tx_bytes=vmac_stats->tx_octets;
	priv->stats.rx_bytes=vmac_stats->rx_octets;

  /* ethernet stats */ 


  /* Error conditions */
  priv->stats.rx_errors = vmac_stats->rx_fragments
	  						+ vmac_stats->undersized_rx_frames
	  						+ vmac_stats->rx_crc_errors
	  						+ vmac_stats->rx_align_code_errors
	  						+ vmac_stats->rx_jabbers
	  						+ vmac_stats->rx_overruns;
  priv->stats.tx_errors = vmac_stats->tx_error_frames;
  //  priv->stats.rx_dropped = vmac_stats->rx_octets;
  //  priv->stats.tx_dropped = vmac_stats->rx_octets;
  priv->stats.multicast = vmac_stats->multicast_rx_frames;     /* 0x040c */
  priv->stats.collisions = vmac_stats->collisions;             /* 0x0478 */

  /* detailed rx_errors */
  //  priv->stats.rx_length_errors = vmac_stats->rx_octets;
  //  priv->stats.rx_over_errors = vmac_stats->rx_octets;
  priv->stats.rx_crc_errors = vmac_stats->rx_crc_errors;       /* 0x0410 */
  //  priv->stats.rx_frame_errors = vmac_stats->rx_octets;
  //  priv->stats.rx_fifo_errors = vmac_stats->rx_octets;
  //  priv->stats.rx_missed_errors = vmac_stats->rx_octets;

  /* detailed tx_errors */
  //  priv->stats.tx_aborted_errors = vmac_stats->rx_octets;
  //  priv->stats.tx_carrier_errors = vmac_stats->rx_octets;
  //  priv->stats.tx_fifo_errors = vmac_stats->rx_octets;
  //  priv->stats.tx_heartbeat_errors = vmac_stats->rx_octets;
  //  priv->stats.tx_window_errors = vmac_stats->rx_octets;

  
}

module_init (vmac_init_module);
module_exit (vmac_cleanup_module);

