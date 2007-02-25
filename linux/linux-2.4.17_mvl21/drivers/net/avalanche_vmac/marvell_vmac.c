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
#include <linux/if_ether.h>
#include <asm/cache.h> 

//#include <asm/avalanche/avalanche.h> /* VW_TODO: Remove this */  
//#include <asm/avalanche/avalanche_int.h>
#include <asm/mips-boards/prom.h>


#include <asm/avalanche/avalanche_map.h>   
#include <asm/addrspace.h>


#include "marvell_vmac.h"
#include "marvell_mdio.h"


#include "wlanCountersApi.h"
#include <asm/avalanche/generic/mibIoctl.h>

/* Just in case the SOC has single instance */
#ifndef AVALANCHE_HIGH_EMAC_BASE
#define AVALANCHE_HIGH_EMAC_BASE
#endif

#undef DEBUG


#define WAN_PORT                4
#define LAN0_PORT               0
#define LAN1_PORT               1
#define LAN2_PORT               2
#define LAN3_PORT               3


#define MARVELL_WAN_HEADER_MASK          0x2010
#define MARVELL_LAN_HEADER_MASK          0x100f




/* Global Variables */
static struct net_device  *VmacDevices = NULL;
static int                VmacDevicesInstalled = 0;
static char		  VmacSignature[] = "VMAC";
static int                num_devices = CONFIG_MIPS_VMAC_PORTS;
static int                isrpace = 2;

static const  unsigned long io_base[CONFIG_MIPS_VMAC_PORTS] = { AVALANCHE_LOW_EMAC_BASE, AVALANCHE_HIGH_EMAC_BASE };
static char vmac_version[] = "3.0.0";

int NUMBER_OF_EMAC_PHYS = 0;
int PHY_ADDRESS = 0;

int vmac_debug=0;

/* Pointer to stats proc entry */
struct proc_dir_entry *gp_stats_file = NULL;


static struct proc_dir_entry *port_wan_config_file = NULL;
static struct proc_dir_entry *port_lan_config_file[4];
static struct proc_dir_entry *port_config_dir = NULL;
static struct proc_dir_entry *port_status_dir = NULL;
static struct proc_dir_entry *port_wan_status_file = NULL;
static struct proc_dir_entry *port_lan_status_file[4];



/* Under heavy loads, the interrupts will be called extremly close together.
* When doing routing, this is not a problem as the stack will drop the skb's
* if they cannot process them. But when doing bridging, we don't give the
* kernel time to free skbs when using kfree_skb_any. Thus we use kfree_skb
* even though we are in a hard interupt. This is wrong, but it works for now.
*/
#define vmac_kfree_skb_any(x)	dev_kfree_skb(x)

#define DPRINTK(level, format, args...)  

#ifdef MODULE
MODULE_DESCRIPTION ("Avalanche EMAC Device Driver");
MODULE_LICENSE("GPL");
MODULE_PARM(num_devices,"i");
MODULE_PARM(vmac_debug,"i");
MODULE_PARM(isrpace,"i");
#endif

/* Forward declarations */
static void	vmac_handle_tx(struct vmac_common *tc);
static int	vmac_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static void	vmac_set_rx_mode (struct net_device *dev);
static int vmac_write_proc(struct file *fp, const char * buf, unsigned long count, void * data);
static int vmac_p_read_rfc2665_stats(char* buf, char **start, off_t offset, int count, int *eof, void *data);
static int      vmac_init_module(void);
static void     vmac_cleanup_module(void);

static int      vmac_InitTxBuffdesc(struct net_device *dev);
static int      vmac_InitRxBuffdesc(struct net_device *dev);
static int      vmac_Inittxcomp(struct net_device *dev,int num);
static int      vmac_Initrxcomp(struct net_device *dev,int num);
static int      vmac_InitRxFBL(struct net_device *dev, int size);
static int      vmac_reset(struct net_device *dev);
static int	vmac_init_one(struct net_device * dev);
static int	vmac_Open(struct net_device *dev);
static int	vmac_Close(struct net_device *);
static int      vmac_sendTx(struct sk_buff *skb, struct net_device *dev);
static int	vmac_read_proc(char* page, char **start, off_t offset, int count,
								int *eof, void *data);

static void     vmac_update_stats(struct net_device *dev);
static struct 	net_device_stats *vmac_get_stats(struct net_device *dev);

static int      vmac_irq_request(struct net_device *dev);
static void	vmac_txirq(struct vmac_common *tc);
static void	vmac_tcra_thresh(struct vmac_common *tc);
static void	vmac_tcr_freeze(struct vmac_common *tc);
#ifdef AVALANCHE_VMAC_TASKLET_MODE
static int vmac_rxirq(struct vmac_common *tc); 
#else
static void	vmac_rxirq(struct vmac_common *tc);
#endif
static void	vmac_rcra_thresh(struct vmac_common *tc);
static void	vmac_rcr_freeze(struct vmac_common *tc);
static void     vmac_linkirq(int irq, void *dev_id, struct pt_regs *regs);
static void     vmac_mgtirq(int irq, void *dev_id, struct pt_regs *regs);
static void	vmac_do_mdio_tic(unsigned long ptr);
static void 	vmac_do_mdio_statechange(struct net_device *dev, u32 *PhyState);

static void     *m_alloc(int size, int a_shift);

/*void xdump( u_char*  cp, int  length, char*  prefix );*/

extern int marvellInit (void);
extern int marvell_receive(unsigned char *data_ptr, unsigned int *packet_length);
extern int marvell_read_counters(char* buf, char **start, off_t offset, int count,int *eof, void *data);
extern int vmac_mib(char* buf, char **start, off_t offset, int count,int *eof, void *data);
extern int marvell_port_read_status(char* buf, char **start, off_t offset, int count,int *eof, void *data);
extern int marvell_port_read_config(char* buf, char **start, off_t offset, int count,int *eof, void *data);
extern int marvell_port_write_config(struct file *file, const char *buffer,unsigned long count, void *data);


extern int marvell_read_database(char* buf, char **start, off_t offset, int count,int *eof, void *data);



#define isprint(a) ((a >=' ')&&(a <= '~')) 
#if 1
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
vmac_HandleTxInt(int irq, void *tc_instance, struct pt_regs *regs)
{
	struct vmac_common *tc = tc_instance;
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
        int int_index;

        int_index = VMAC_INTSTS(dev->base_addr) & tc->int_mask;
	if(int_index & TCR_FREEZE)
  		vmac_tcr_freeze(tc);
	if(int_index & TCRA_THRESH)
  		vmac_tcra_thresh(tc);
	if(int_index & POSTED_TCRA)
  		vmac_txirq(tc);
} /* vmac_HandleTxInt */

static void
vmac_HandleRxInt(int irq, void *tc_instance, struct pt_regs *regs)
{
	struct vmac_common *tc = tc_instance;
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
	int int_index;

        int_index = VMAC_INTSTS(dev->base_addr) & tc->int_mask;
	if(int_index & RCR_FREEZE)
  		vmac_rcr_freeze(tc);
	if(int_index & RCRA_THRESH)
  		vmac_rcra_thresh(tc);

#ifdef AVALANCHE_VMAC_TASKLET_MODE
	if(int_index & POSTED_RCRA)
        {
        
	    /* after acking and  disabling interrupts for rx  */   
	    VMAC_INTSTS(dev->base_addr) = POSTED_RCRA;  // VW_TODO: What does the SAR do? 
	    VMAC_INTMASK(dev->base_addr) &= ~POSTED_RCRA;
           
	   /* schedule tasklet here  */
           tasklet_schedule(&tc->vmac_tasklet);

        }
#else /* !AVALANCHE_VMAC_TASKLET_MODE */
	if(int_index & POSTED_RCRA)
  		vmac_rxirq(tc);
#endif
} /* vmac_HandleRxInt */

	

#ifdef AVALANCHE_VMAC_TASKLET_MODE
static void
vmac_rx_tasklet(struct vmac_common * tc_instance)
{

	struct vmac_common *tc = tc_instance;
	struct net_device *dev = tc->priv[WAN_PORT]->dev;
        int pending;

       
	/* process rx packets  */
        pending = vmac_rxirq(tc);

        
	/* schedule tasklet again
 	 * if there are packets to be processed
 	 *  else enable interrupts for rx
 	 */
	if(pending)
        	tasklet_schedule(&tc->vmac_tasklet);

	else
		VMAC_INTMASK(dev->base_addr) |= POSTED_RCRA;

} 
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_reset(struct net_device * dev)
 *
 *  Description: Resets the VMAC
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int vmac_reset(struct net_device *dev)
{
	struct vmac_common	*tc;
	struct vmac_priv	*tp;

	u16  result2;

	result2=0;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	/* Reset the VMAC in the PRCR register */ 
	if(tc->vmac_num == 0) {
		/* Put Vmac0 into reset */
		avalanche_reset_ctrl(AVALANCHE_LOW_EMAC_RESET_BIT, IN_RESET); 
		udelay(1);	/* reset the VMAC peripheral */

            /* Bring Vmac0 out of reset */    
		avalanche_reset_ctrl(AVALANCHE_LOW_EMAC_RESET_BIT, OUT_OF_RESET); 
		udelay(1);	/* reset the VMAC peripheral */

	} 

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
	struct vmac_common	*tc;
	struct vmac_priv	*tp;
	void *txfbl_ptr;
	tx_fbld_t *prev = NULL;
	tx_fbld_t *fbl_ptr;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;
	tc->txfbl_num = 0;   /* Initialize number of free buffers */

	for(i=0; i < NUM_TX_BUF; i++)  
	{
		if((txfbl_ptr = kmalloc(sizeof(tx_fbld_t), GFP_KERNEL | GFP_DMA)) == NULL) {
			printk(VMAC_ERROR "Could not allocate TX FBL descriptor\n");
			return -ENODEV;
		}

		fbl_ptr = (tx_fbld_t *) txfbl_ptr;

		if(i==0)
			tc->txfbl_head = prev = (tx_fbld_t *) txfbl_ptr;
		else
		{
			prev->next = (tx_fbld_t *) txfbl_ptr;
			prev = (tx_fbld_t *) txfbl_ptr;
		}

		fbl_ptr->mode = 0;
		fbl_ptr->BufPtr = NULL;
		fbl_ptr->skb   = NULL;
		tc->txfbl_num++;
	}

	tc->txfbl_tail = (tx_fbld_t *) txfbl_ptr;
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
	struct vmac_priv	*tp;
	struct vmac_common	*tc;
	rx_fbld_t		*rxfbl_descriptor;  /* free buffer descriptor pointer */
	rx_fbld_t		*desc_ptr = NULL;	/* free buffer pointer */
	struct sk_buff	*skb = NULL;		/* working skb */

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;


	/* Get vmac rx buffers setup */
	if ((rxfbl_descriptor  = kmalloc ((NUM_RX_BUF * sizeof (rx_fbld_t)), GFP_KERNEL | GFP_DMA)) == NULL) {
		printk (VMAC_ERROR "Could not allocate RX Buff descriptors\n");
		return -ENODEV;
	}
	

	tc->rxfbl = rxfbl_descriptor;  		/* fbl_id = 0, only 1 choice for VMAC */
	desc_ptr = tc->rxfbl;

	tc->rx_burstRCRcount=0;
	tc->rx_burstbufcount=0;
	tc->rx_burstbufstart=0;
	tc->rx_burstbufstop=0;

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
	        
		skb_reserve(skb,L1_CACHE_BYTES); 

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
	struct vmac_priv	*tp;
	struct vmac_common	*tc;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	//Make num power of two
	num *= 2;
	num--;

	for(i=0,j = 0x80000000;i<32;i++,j>>=1)
		if (j & num) 
			num &= j;


	/* Allocate memory so that it is on  a power of size alignment */

	iTmp = num * 16 * 2;                                 /* size of Tx comp ring */
	pTmp = m_alloc(iTmp, 15);                             /* Align on a 2^8 boundary TBD */
	
	if(!pTmp)
		return -ENOMEM;

	dma_cache_wback_inv((unsigned long)pTmp,iTmp);       /* Invalidate this memory */
	tc->txcr = (tx_compring *) pTmp;                   /* software pinter to txcr */
	tc->txcr_start = tc->txcr;                       /* first structure of completion ring */
	tc->txcr_end = (tc->txcr+(num-1));               /* last structure of completion ring  */
	memset(tc->txcr, 0, iTmp);                         /* Clear memory */

	/* allocate Tx completion ring (num entries) */

	VMAC_TCRPTR(dev->base_addr) = virt_to_phys(tc->txcr);

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
vmac_txirq(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
	VMAC_INTSTS(dev->base_addr) = POSTED_TCRA;  /* Needs to be cleared in VMAC */
	vmac_handle_tx(tc);
/*	DPRINTK(5,"POSTED_TCRA interrupt\n"); */
} /* vmac_txirq */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_tcra_thresh(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Tx Completion Ring Threshold Reached 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_tcra_thresh(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */

	VMAC_INTSTS(dev->base_addr) = TCRA_THRESH;
	vmac_handle_tx(tc);
}

static void
vmac_handle_tx(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
	tx_compring *tx_ring_ptr;
	tx_fbld_t  *CurrentTxFBLD;
	unsigned int i,num_entries;

	num_entries = VMAC_TCRPENDENT(dev->base_addr);
	for(i=0;i<num_entries;i++)
	{
		tx_ring_ptr = (tx_compring *)tc->txcr;
		do{
			dma_cache_inv((unsigned long)tx_ring_ptr, CACHE_LINE_SIZE);
		}while(tx_ring_ptr->sof_list == 0);
		CurrentTxFBLD = (tx_fbld_t *) bus_to_virt(tx_ring_ptr->sof_list);

		DPRINTK(5,"TxCR [%p]: fbd[%p]:%d buff[%p]\n",tx_ring_ptr,CurrentTxFBLD, 
			(CurrentTxFBLD->mode&0xffff), (char *) CurrentTxFBLD->BufPtr);

		tc->good_tx_sw_frames++;
		tc->stats.tx_packets++;
		tc->stats.tx_bytes += CurrentTxFBLD->skb->len;

		vmac_kfree_skb_any(CurrentTxFBLD->skb);

		/* Free the buffer pointed to by the the Tx Descriptor */
		Put_TxDesc(CurrentTxFBLD,tc);

		/* Increment the Tx Completion buffer pointer */
		tx_ring_ptr->sof_list=0;
		if(++tx_ring_ptr > tc->txcr_end)
			tc->txcr = tc->txcr_start;
		else
			tc->txcr = tx_ring_ptr; 
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
vmac_tcr_freeze(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
	VMAC_INTSTS(dev->base_addr) = TCR_FREEZE;  /* Needs to be cleared in VMAC */
	DPRINTK(5,"tcra_freeze interrupt\n");
} /* vmac_tcr_freeze */


/* TODO Add netif_stop_queue support */
/* TODO Rewrite Get_TxDesc to be better */
static int
vmac_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
	struct vmac_priv *tp = (struct vmac_priv *) dev->priv;
	struct vmac_common *tc = (struct vmac_common *) tp->common;
	int pkt_len;

	tx_fbld_t *tx_desc;
	u32 count=0;
//	static int num_called=0;
//	static int tx_fbl_count = 0;
	int ret=1;
	int flags;

#define INGRESS_HEADER_LEN 2
	/* Add the Ingress Header */

        if(skb_headroom(skb) < INGRESS_HEADER_LEN)
	{
        	/* For now, just return error */
		goto tx_error;

#if 0
		struct sk_buff *temp_skb;
		temp_skb = skb_copy_expand(skb,4,0,GFP_ATOMIC);
		
		if(!temp_skb)
			goto tx_error;

		kfree_skb(skb);
		skb = temp_skb;
#endif


	}
	
	skb_push(skb, INGRESS_HEADER_LEN);

	if(tp->phy_mask[WAN_PORT] == 1)
	{
		skb->data[0] = (unsigned)(MARVELL_WAN_HEADER_MASK)  >> 8;
		skb->data[1] = MARVELL_WAN_HEADER_MASK & 0xff;	
	}
	else
	{
		skb->data[0] = (unsigned)(MARVELL_LAN_HEADER_MASK)  >> 8;
		skb->data[1] = MARVELL_LAN_HEADER_MASK & 0xff;	
	}

#if 0
	if(tc->txfbl_num < 4)
	{
		/* The host has run out of transmit buffer descriptors */
		if (tx_fbl_count == 0)
			printk(KERN_ERR "VMAC: start_xmit: No buffers for transmit\n");
		tp->mib_counters.ifOutDiscards++;
		tx_fbl_count++;
		ret = 1;
		goto tx_error;
	}
#endif

//	num_called++;

	if(( VMAC_TXPADDCNT(dev->base_addr) & 0x80000000 ) && tc->txfbl_num > 4)
	{

		tx_desc=Get_TxDesc(tc);

		/* Taking care of VMAC checksum */
		pkt_len = skb->len + 4;
	
		/* Taking care of undersized frames */
		pkt_len = (pkt_len < 66)? 66 : pkt_len;

#ifdef DEBUG
		printk("skb len = %d pktlen=%d\n", skb->len,pkt_len);
		xdump(skb->data, pkt_len, "TxPacket: "); 
#endif

		tx_desc->mode = 0xc0000000 | (u16)(pkt_len);
		tx_desc->BufPtr=(unsigned char *)virt_to_bus(skb->data);
		tx_desc->offset = (u32)(skb->data) & 0x03;
		tx_desc->skb = skb;

#if 0
	while(( ( VMAC_TXPADDCNT(dev->base_addr) & 0x80000000 ) == 0 ) && count < 10000)
	{
		count++;
		udelay(1);
	}
	if( count >0 ) {
		printk(KERN_ERR "TX Wait %d\n",count);
		memset(tx_desc, 0, 16);
		Put_TxDesc( tx_desc, tc);
		tp->mib_counters.ifOutDiscards++;
		ret = 1;
		goto tx_error;
	}
#endif

		/* Increment the MIB counters */
		tp->mib_counters.ifOutOctets += skb->len;
					
		if(!BC_MC_MAC_ADD((u8*)(skb->data + 2)))
			tp->mib_counters.ifOutUnicasts++;
		else 
		{
			if(MC_MAC_ADD((u8*)(skb->data + 2)) > 0)
				tp->mib_counters.ifOutMulticasts++;	
			else
				tp->mib_counters.ifOutBroadcasts++;
		}

		local_irq_save(flags);
		VMAC_TXPADDCNT(dev->base_addr) = 0x00000001;
		VMAC_TXPADDSTART(dev->base_addr) = virt_to_bus( tx_desc );
		wmb();
		VMAC_TXPADDEND(dev->base_addr)  = virt_to_bus( tx_desc );
		local_irq_restore(flags);

		ret = 0;
	} 

tx_error:
	return ret;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_rxirq(struc net_device *dev)
 *
 *  Description: Avalanche VMAC Rx interrupt
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef AVALANCHE_VMAC_TASKLET_MODE
static int
#else
static void
#endif
vmac_rxirq(struct vmac_common *tc)
{
	struct vmac_priv	*tp = NULL;
	struct net_device	*dev;
	rx_compring *rx_ring_ptr;
	unsigned int pdu_len;
	rx_fbld_t * first_buff_desc = NULL; 
	struct sk_buff *skb = NULL;  
	struct sk_buff *new_skb = NULL;  
	unsigned char *data_ptr; 
	unsigned int i,num_entries;
//	static int num_called=0;
	int		port;
	unsigned long	base_addr;	/* As we are dealing with multiple channels, we must save off this addr
					   so that we can play with the pointers to the dev struct. */

	base_addr=tc->priv[WAN_PORT]->dev->base_addr;	/* Phy 4 will always refer to the WAN port, and the base address
						   is suitable for use for the register pointers We will get the real
						   device in a little bit */

	num_entries = VMAC_RCRPENDENT(base_addr) - tc->rx_burstRCRcount;

#ifdef AVALANCHE_VMAC_TASKLET_MODE
	for(i=0;i<num_entries && i < CFG_RX_NUM_BUF_SERVICE;  i++)
#else
	VMAC_INTSTS(base_addr) = POSTED_RCRA; //acknowledge the interrupt


	for(i=0;i<num_entries;  i++)
#endif
	{
		rx_ring_ptr =(rx_compring *)tc->rxcr;
		do{
			dma_cache_inv((unsigned long)rx_ring_ptr, CACHE_LINE_SIZE);
		}while(((unsigned long*)rx_ring_ptr)[0]==0);

		/* check for errors in the Rx packet */ 
		if(rx_ring_ptr->valid)
		{
//			num_called++;
			/* parse the completion ring */
			first_buff_desc = (rx_fbld_t *)bus_to_virt(rx_ring_ptr->sof_list);
			dma_cache_inv((unsigned long)first_buff_desc,CACHE_LINE_SIZE); /* *** Need to do this ? *** */

			pdu_len = first_buff_desc->size;
			data_ptr = (unsigned char *)(phys_to_virt(first_buff_desc->BufPtr));
			dma_cache_inv((unsigned long)data_ptr,pdu_len+2);

			skb = first_buff_desc->skb;
			
			/* Find the port that it came from */
//			port=marvell_receive(data_ptr, &pdu_len);
			port= (data_ptr[1] & 0xf);

			if( (port >= 0 &&  port <= 5) && (tp=tc->priv[port]) && (dev=tp->dev) && (!rx_ring_ptr->error_ind) && (pdu_len == rx_ring_ptr->frame_len) )  
			{
				/* allocate new skb to place it in free buffer list */
				new_skb = dev_alloc_skb(MAX_FRAME_SIZE);			
				if(new_skb)
			       	{
	                        	skb_reserve(new_skb,L1_CACHE_BYTES); 
					skb->dev = dev;
					dev->if_port =  port;
					skb_put (skb, pdu_len);
					
#define		MARVELL_EGRESS_HEADER_LEN   2
					/* get rid of the Egress Header */
					skb_pull(skb,MARVELL_EGRESS_HEADER_LEN);
#if 0
					skb->protocol = eth_type_trans (skb, dev);
					/* hack for now.. the reason for
					 * pushing again is its been pulled
					 * twice once above and i eth_type_trans
				       	 */	
					skb_push(skb,MARVELL_EGRESS_HEADER_LEN);

#endif

					{
						struct ethhdr *eth;
						unsigned char *rawp;
	
						skb->mac.raw=((int)skb->data) ;
						skb_pull(skb,ETH_HLEN);
						eth= skb->mac.ethernet;
	
						if( !(*eth->h_dest&1))
						{
							if(memcmp(eth->h_dest,dev->dev_addr, ETH_ALEN))
								skb->pkt_type=PACKET_OTHERHOST;
		
						}
	
						/*
						 *		This ALLMULTI check should be redundant by 1.4
			 			 *	so don't forget to remove it.
	 		 			 *
						 *	Seems, you forgot to remove it. All silly devices
						 *	seems to set IFF_PROMISC.
			 			 */
	 
						else 
						{
							if(memcmp(eth->h_dest,dev->broadcast, 6)==0)
								skb->pkt_type=PACKET_BROADCAST;
							else
								skb->pkt_type=PACKET_MULTICAST;

						}
	
						skb->protocol = eth->h_proto;
					}

					netif_rx (skb);

                                        tc->good_rx_sw_frames++;

					/* xdump(data_ptr, pdu_len, "RxPacket: "); */

					/* Packet was successfully recieved. Increment the MIB counters */
					tp->mib_counters.ifInOctets += pdu_len;

					if(!BC_MC_MAC_ADD((u8*)(data_ptr + 2)))
						tp->mib_counters.ifInUnicasts++;
					else 
					{
						if(MC_MAC_ADD((u8*)(data_ptr + 2)) > 0)
							tp->mib_counters.ifInMulticasts++;	
						else
							tp->mib_counters.ifInBroadcasts++;
					}
					dev->last_rx = jiffies;
				} 
				else
				{

#ifdef DEBUG

					printk (KERN_WARNING "%s: Memory squeeze, dropping packet.\n", dev->name);
				
#endif
					tp->mib_counters.ifInDiscards++;
					
					/* Requeing  the same buffer into free buffer list */
					new_skb = skb; 
				}
				
			}
			else
			{
#ifdef DEBUG
				printk("ERROR in SKB ...discarding the packet\n");


				printk("port=%d, cond3=%d cond4=%d, VMAC_MACcontrol=%x,errorbits=%x\n,length=%x",port,(!rx_ring_ptr->error_ind),(pdu_len == rx_ring_ptr->frame_len),VMAC_MACCONTROL(dev->base_addr),rx_ring_ptr->errorbits,rx_ring_ptr->frame_len);

#endif

				tc->frame_error++;
				tp->mib_counters.ifInErrors++;
				
				/* Requeing  the same buffer into free buffer list */
				new_skb = skb; 

			}
			
			first_buff_desc->BufPtr=virt_to_phys((int *)new_skb->data);
			first_buff_desc->skb=new_skb;

			/* Free RCR entries to the "queue" */
			((unsigned long*)rx_ring_ptr)[0]= 0;
			tc->rx_burstRCRcount++;

			/* Return free buffers to the "queue" */
			tc->rx_burstbufcount++;
			if(tc->rx_burstbufstart)	/* Queue has data in it */
			{
				tc->rx_burstbufstop->next=virt_to_phys((int*)first_buff_desc);
				tc->rx_burstbufstop=first_buff_desc;
			} 
			else 
			{			/* Queue is empty */
				tc->rx_burstbufstart=first_buff_desc;
				tc->rx_burstbufstop=first_buff_desc;
			}
		} 
		else 
		{
			/* Error in packet */
			/* Free RCR Entry */
			((unsigned long*)rx_ring_ptr)[0]= 0;
			tc->rx_burstRCRcount++;
		}
		
		/* Increment the Rx Completion ring pointer */
		if(++rx_ring_ptr > tc->rxcr_end)
			tc->rxcr = tc->rxcr_start;
		else
			tc->rxcr = rx_ring_ptr;
	}

	if(tc->rx_burstRCRcount < BURST_THRESHOLD)
		return;
	else
	{
		/* Setup the addresses for the hardware to use */
		tc->rx_burstbufstart= (rx_fbld_t*) virt_to_phys(tc->rx_burstbufstart);
		tc->rx_burstbufstop = (rx_fbld_t*) virt_to_phys(tc->rx_burstbufstop);

		/* Verify that the hardware is ready to send the buffers */
		while( ( VMAC_FBLADDCNT(base_addr) & FBL_RDY ) == 0 ) /* Wait for ready */
			udelay(1);			

		/* Free RCR Entry */
		VMAC_RCRENTINC(base_addr) = tc->rx_burstRCRcount;

		/* return buffers to the Free buffer list */
		VMAC_FBLADDCNT(base_addr) = tc->rx_burstbufcount;
		VMAC_FBLADDSTART(base_addr) = (u32)tc->rx_burstbufstart;
		VMAC_FBLADDEND(base_addr) = (u32)tc->rx_burstbufstop;

		/* Reset the "queue" */
		tc->rx_burstRCRcount=0;
		tc->rx_burstbufcount=0;
		tc->rx_burstbufstart=0;
		tc->rx_burstbufstop=0;
	}


#ifdef AVALANCHE_VMAC_TASKLET_MODE
        if(CFG_RX_NUM_BUF_SERVICE >= num_entries)
            return 0;

        else
            return 1;
#endif
 

} /* vmac_rxirq */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int vmac_rcra_thresh(struct net_device *dev)
 *
 *  Description: Avalanche VMAC Rx Completion Ring Threshold Reached 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
vmac_rcra_thresh(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */
 
	VMAC_INTSTS(dev->base_addr) = RCRA_THRESH;  /* Needs to be cleared in VMAC */
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
vmac_rcr_freeze(struct vmac_common *tc)
{
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */

	VMAC_INTSTS(dev->base_addr) = RCR_FREEZE;  /* Needs to be cleared in VMAC */
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
vmac_linkirq(int irq, void *tc_instance, struct pt_regs *regs)
{
	struct vmac_common *tc = tc_instance;
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */

	VMAC_INTSTS(dev->base_addr) = LINK_CHANGE;  /* Needs to be cleared in VMAC */

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
vmac_mgtirq(int irq, void *tc_instance, struct pt_regs *regs)
{
	struct vmac_common *tc = tc_instance;
	struct net_device *dev = tc->priv[WAN_PORT]->dev; /* Phy 4 will always refer to the WAN port, and the base address
						 is suitable for use for the register pointers */

	VMAC_INTSTS(dev->base_addr) = LNKEVT;  /* Needs to be cleared in VMAC */

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
	struct vmac_common	*tc;
	struct vmac_priv	*tp;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	//Make num power of two
	num *= 2;
	num--;

	for(i = 0,j = 0x80000000; i < 32 ; i++, j >>= 1)
		if (j & num) 
			num &= j;


	/* Allocate memory so that it is on a power of size alignment */

	iTmp = num * 16 * 2;
	pTmp = m_alloc(iTmp,15); /* Align on a 2^10 boundary TBD */

	if(!pTmp)
		return -ENOMEM;

	dma_cache_wback_inv((unsigned long) pTmp, iTmp); /* Invalidate this memory */
	tc->rxcr = (rx_compring *)pTmp;	           /* software pointer to rxcr */
	tc->rxcr_start = tc->rxcr;                   /* first structure of completion ring */
	tc->rxcr_end = (tc->rxcr+(num-1));           /* last structure of completion ring  */

	memset(tc->rxcr, 0, iTmp);  /* Clear memory */

	/* allocate Rx completion ring (num entries) */

	VMAC_RCRPTR(dev->base_addr) = virt_to_bus(tc->rxcr);
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
	struct vmac_common	*tc;
	struct vmac_priv	*tp;
	int i;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	tc->rxfbl_initialized = 1;           /* Set flage to initialize Once */

	/* set Rx Buffer Size */
	VMAC_FBL0BUFSIZE(dev->base_addr) = size;

	/* Free the RxFBL for input */
	for(i=0; i < NUM_RX_BUF; i++){  /* Rx_enable in DMA State Machine must be enabled */

		while( ( VMAC_FBLADDCNT(dev->base_addr) & FBL_RDY ) == 0 ) /* Wait for ready */
			udelay(1);

		VMAC_FBLADDCNT(dev->base_addr) = 1;
		VMAC_FBLADDSTART(dev->base_addr) = virt_to_bus( &tc->rxfbl[i] );
		VMAC_FBLADDEND(dev->base_addr)   = virt_to_bus( &tc->rxfbl[i] );
	}

	return 0;
}


static int vmac_irq_request(struct net_device *dev)
{
	struct vmac_priv *tp = (struct vmac_priv *) dev->priv;
	struct vmac_common *tc = (struct vmac_common *) tp->common;

//	tc->txfbl_lock = SPIN_LOCK_UNLOCKED;

	/* Select the proper irq lines based on emac devices */
	switch(tc->vmac_num)
	{
		case 0:
			/* Interrupt line # for EMAC A */
			tc->tx_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_TX_INT);
			tc->rx_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_RX_INT);
			tc->lnk_irq = LNXINTNUM( AVALANCHE_LOW_EMAC_LNK_CHG_INT);
			tc->mgt_irq = LNXINTNUM(AVALANCHE_LOW_EMAC_MGT_INT);
			break;
		default:
			printk(VMAC_ERROR "IRQ_REQUEST: invalid vmac number\n");
			return 0;
	}


#ifdef AVALANCHE_VMAC_TASKLET_MODE
        tasklet_init(&tc->vmac_tasklet,vmac_rx_tasklet,(unsigned long)tc);
#endif

	/* Register Ethernet Transmit interrupt  */
	if(request_irq(tc->tx_irq, vmac_HandleTxInt, SA_INTERRUPT, VmacSignature, tc))
		printk(VMAC_ERROR "Could not register vmac_tx_irq\n");

	/* Register Ethernet Receive interrupt  */
	if(request_irq(tc->rx_irq, vmac_HandleRxInt, SA_INTERRUPT, VmacSignature, tc))
		printk(VMAC_ERROR "Could not register vmac_rx_irq\n");

	/* Register Ethernet link change interrupt  */
	if(request_irq(tc->lnk_irq, vmac_linkirq, SA_INTERRUPT, VmacSignature, tc))
		printk(VMAC_ERROR "Could not register vmac_link_irq\n");

	/* Register Ethernet management change interrupt  */
	if(request_irq(tc->mgt_irq, vmac_mgtirq, SA_INTERRUPT, VmacSignature, tc))
		printk(VMAC_ERROR "Could not register vmac_link_irq\n");

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
	struct 	vmac_common	*tc;
	struct 	vmac_priv	*tp;
	struct 	timer_list	*tic_timer;
	u32		phy_mode;
	u32		cpu_freq;
	char	*cpu_freq_ptr;
	int 	index, i;


	index = 0;
			
	if (dev == NULL)
	{
		printk (VMAC_ERROR "no valid dev structure\n");
		return -ENODEV;
	}

	printk("Texas Instruments VMAC driver  ver:[%s]\n", vmac_version);

        DPRINTK(2,"line = %d\n", __LINE__);
	tp = (struct vmac_priv *) dev->priv;
	tc = tp->common;

	/* Get CPU Frequency */  
	cpu_freq_ptr = prom_getenv("cpufrequency");
	if(!cpu_freq_ptr) 
	{
		printk("Couldn't find cpufrequency environment variable\n");
		cpu_freq = CONFIG_CPU_FREQUENCY_AVALANCHE * 1000000 ;
	} 
	else 
	{
		cpu_freq = simple_strtol(cpu_freq_ptr,NULL,0);
	}

        DPRINTK(2,"line = %d\n", __LINE__);

	if(vmac_InitTxBuffdesc(dev) < 0)		/* Initialize Transmit Buffer descripters */
		return -ENODEV;			      	/* Host only */
        DPRINTK(2,"line = %d\n", __LINE__);
	if(vmac_InitRxBuffdesc(dev) < 0)		/* Initialize Receive Buffer descripters */
		return -ENODEV;		      		/* Host and Vmac */
        DPRINTK(2,"line = %d\n", __LINE__);
	if(vmac_reset(dev) != 0)			/* Take Vmac out of reset,Reset MAC, Reset Phy */
		return -ENODEV;				/* Must preceed Tx/Rx completion ring inits    */
        DPRINTK(2,"line = %d\n", __LINE__);
	if (vmac_Inittxcomp (dev, NUM_TX_COMP) < 0)	/* Initialize Transmit completion ring */
		return -ENODEV;
        DPRINTK(2,"line = %d\n", __LINE__);
	if (vmac_Initrxcomp (dev, NUM_RX_COMP) < 0)	/* Initialize Receive completion ring */
		return -ENODEV;
        DPRINTK(2,"line = %d\n", __LINE__);

	/* Set MAC Control register. This will be done in the state change function as well */
	VMAC_MACCONTROL(dev->base_addr) =  CAF | NO_LOOP | SPEED_100 | CLRP | NO_CHAIN | FULL_DUPLEX | MAXLEN | LONG ;
        DPRINTK(2,"line = %d\n", __LINE__);

#ifdef CONFIG_MIPS_WA1130
	/* WA1130 board with 5 Ethernet port and phy located at address 0 to 4 */
	NUMBER_OF_EMAC_PHYS = 5;
	PHY_ADDRESS = 0;
        DPRINTK(2,"line = %d\n", __LINE__);
#endif
			
        DPRINTK(2,"line = %d\n", __LINE__);
	
	/* Setup MDIO library */
	for(i = 0; i < NUMBER_OF_EMAC_PHYS; i++)
	{
        	DPRINTK(2,"line = %d\n", __LINE__);
		EmacMdioInit(dev->base_addr, PHY_ADDRESS, &(tc->phy_state[index]), cpu_freq, 0);
		index++;
	}

	/* Set the phy to with the complete range */

	phy_mode= NWAY_FD100 | NWAY_HD100 | NWAY_FD10 | NWAY_HD10 | NWAY_AUTO;

	index = 0;


        DPRINTK(2,"line = %d\n", __LINE__);

	for(i = 0; i < NUMBER_OF_EMAC_PHYS; i++)
	{
		EmacMdioSetPhyMode(dev->base_addr, &(tc->phy_state[index]), phy_mode);
		index++;
	}
	
        DPRINTK(2,"line = %d\n", __LINE__);
	
	/* Initilize MDIO tic timer */
	tic_timer=&(tc->mdio_tic_timer);
	init_timer(tic_timer);
	tic_timer->function = vmac_do_mdio_tic;			/* Assign function to call */
	tic_timer->data = (unsigned long) dev;			/* Timer gets dev pointer to access data */
	tic_timer->expires = jiffies + MDIO_TIC_DELAY + 30;	/* Delay the first tic to let things ralax */
	tc->mdio_tic_run = 1;
	add_timer(tic_timer);					/* Start the timer going */

        DPRINTK(2,"line = %d\n", __LINE__);
	
	/* If NUMBER_OF_EMAC_PHYS > 1 we have to initialize the Marvell Switch */

	if (NUMBER_OF_EMAC_PHYS != 1)
	{
        	DPRINTK(2,"line = %d\n", __LINE__);
	
		if (marvellInit())
			printk("Marvell Switch Initialized\n");
		else
				printk("ERROR : Marvell Switch Init Failed...\n");
        	
		DPRINTK(2,"line = %d\n", __LINE__);
	
	}
	
	VMAC_INTSTS(dev->base_addr) = 0x3ff;			/* Clear all interrupts */
	tc->nextDevice = VmacDevices;
	VmacDevices = dev;      
	VmacDevicesInstalled++;
	
    	DPRINTK(2,"Leaving vmac_init_one\n");
	return 0;
}  /* vmac_init_one */

static int open_first_time = 0;

static int
vmac_Open(struct net_device *dev )
{
	struct vmac_common	*tc;
	struct vmac_priv	*tp;
	mac_addr_t mac_addr_hi, mac_addr_low;

	tp = (struct vmac_priv *) dev->priv;
	tc = tp->common;

	DPRINTK(1,"Entering Vmac_Open (\n");

	if (open_first_time == 1)
	{
		/* Device is up. Set the Operation status to enabled */
		tp->mib_counters.ifOperStatus = 1;			
		return 0;
	}

	open_first_time = 1;

	mac_addr_hi.byte4 = dev->dev_addr[0];      /* High Order Byte of MAC address */
	mac_addr_hi.byte3 = dev->dev_addr[1];
	mac_addr_hi.byte2 = dev->dev_addr[2];
	mac_addr_hi.byte1 = dev->dev_addr[3];

	mac_addr_low.byte2 = dev->dev_addr[4];
	mac_addr_low.byte1 = dev->dev_addr[5];     /* Low Order Byte of MAC address */

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
	if(!tc->rxfbl_initialized)
	{
		if(vmac_InitRxFBL(dev, MAX_FRAME_SIZE) < 0)
			return -ENODEV;
	}

	/* Set ISR Pacing */
	VMAC_TXISRPACE(dev->base_addr) = isrpace;  /* 2 is optimal. 0 should give 1 int/frame. */
	VMAC_RXISRPACE(dev->base_addr) = isrpace;  /* Currently not documented in spec. */

	/* Request IRQ's */
	if(vmac_irq_request(dev) < 0)  /* request interrupt */
		return -EBUSY;

	/* Set Transmit Threshold Level */
	VMAC_TCRINTTHRESH(dev->base_addr) = TRANSMIT_INT_THRESH;

	/* Enable Interrupts via Interrupt Mask Register */
	tc->int_mask = POSTED_RCRA | POSTED_TCRA | TCRA_THRESH;
	VMAC_INTMASK(dev->base_addr) = tc->int_mask;

	/* Clear Interrupt Status Register */      
	VMAC_INTSTS(dev->base_addr) = 0x3ff;

	MOD_INC_USE_COUNT;
	/* Device is up. Set the Operation status to enabled */
	tp->mib_counters.ifOperStatus = 1;
	DPRINTK(1,"Leaving Vmac_Open  \n");
	return 0;

} /* vmac_Open */


static int
vmac_Close(struct net_device *dev)
{
	struct vmac_priv *tp;
	  
	DPRINTK(1,"Entering Vmac_Close \n");
	
	if (dev == NULL)
		return 0;
	if ((tp = (struct vmac_priv *) dev->priv) == NULL)
		return 0;

	tp->mib_counters.ifOperStatus = 0;

	/*  vmac_free_irq(dev);*/
	MOD_DEC_USE_COUNT;

	DPRINTK(1,"Leaving Vmac_Close \n");
	return 0;

} /* vmac_Close */




static int 
vmac_proc_version(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len=0;
  
  len += sprintf(buf+len, "Texas Instruments VMAC driver version: %s\n", vmac_version);
  return len;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: void vmac_probe(strict net_device *dev)
 *
 *  Description: when build into the kernel, inits the devices
 *		basically, just calles the init modules stuff from above
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int __init vmac_probe(struct net_device *dev)
{
	static int probe_count=0;
	static struct vmac_common	*tc;
	struct vmac_priv		*tp;
	int result;  
	int returnval=-ENODEV;
	int i;
	unsigned char mac_addr[ETH_ALEN];
	char *mac_name=NULL;
	char *mac_string=NULL;

	if(probe_count==0){
		int i,index=0;
		
		/* Setup proc entry */
		create_proc_read_entry("avalanche/vmac_ver",0,NULL,vmac_proc_version,NULL);
                create_proc_read_entry("avalanche/vmac_rfc2665" ,0,NULL,vmac_p_read_rfc2665_stats,NULL);
	        create_proc_read_entry("avalanche/marvell_counters" ,0,NULL,marvell_read_counters,NULL);
	        create_proc_read_entry("avalanche/vmac_mib" ,0,NULL,vmac_mib,NULL);
	       
                /* Setup proc entry */
	        gp_stats_file = create_proc_entry("avalanche/vmac_stat",0644,NULL);

		/* Set up proc entries for setting/reading configuration */
		port_config_dir = proc_mkdir("sys/dev/marvell", NULL);
		port_status_dir = proc_mkdir("avalanche/marvell", NULL);
		
		port_wan_config_file = create_proc_entry("5", 0644, port_config_dir);
		port_wan_status_file = create_proc_entry("5", 0644, port_status_dir);
		
		if(port_wan_config_file)
		{
			port_wan_config_file->data = (void * ) WAN_PORT; 	
			port_wan_config_file->read_proc = marvell_port_read_config;	
			port_wan_config_file->write_proc = marvell_port_write_config;		   		
		}
		if(port_wan_status_file)
		{
			port_wan_status_file->data = (void * ) WAN_PORT; 	
			port_wan_status_file->read_proc =  marvell_port_read_status;	
		}

		for(i = LAN0_PORT; i<= LAN3_PORT ;i++,index++)
		{
			char str[10];

			sprintf(str,"%d",index + 1);
			port_lan_config_file[index] = create_proc_entry(str, 0644, port_config_dir);
			port_lan_config_file[index]->data = (void * ) i; 	
			port_lan_config_file[index]->read_proc = marvell_port_read_config;	
			port_lan_config_file[index]->write_proc = marvell_port_write_config;		   

			port_lan_status_file[index] = create_proc_entry(str, 0644, port_status_dir);
			port_lan_status_file[index]->data = (void * ) i; 	
			port_lan_status_file[index]->read_proc = marvell_port_read_status;	

		}
	    
                if(gp_stats_file)
                {
                     gp_stats_file->read_proc  = vmac_read_proc;
                     gp_stats_file->write_proc = vmac_write_proc;
                }
		
	}

	/* We are cheating here. We will call probe twice for the same hardware. One will be for the Wan
	 * interface, and the other for the Lan interface. It is a cheezy way of doing it, but for now it
	 * should work. This will get cleaned up in the complete rewrite */
	/* This routing will only work for a single device. It will not work properly for multiple devices */
	if(probe_count >= (num_devices * 2)){		/* All done, stop autoprobe */
		return -ENODEV;
	}

	if(probe_count == 0){
		tc=kmalloc(sizeof(struct vmac_common),GFP_KERNEL);
		if(!tc){
			printk(VMAC_ERROR "unable to kmalloc common structure. Killing autoprobe.\n");
			return -ENODEV;
		}
		memset(tc, 0, sizeof(struct vmac_common));
	}

	/* Alloc priv struct */
	tp=kmalloc(sizeof(struct vmac_priv),GFP_KERNEL);
	if(!tp){
		printk(VMAC_ERROR "unable to kmalloc priv structure. Killing autoprobe.\n");
		return -ENODEV;
	}
	memset(tp, 0, sizeof(struct vmac_priv));
	ether_setup(dev);				/* assign some of the fields */

	/* Change the device hard header length to 16 ....14 for ethernet header and 2 for marvell header */
	dev->hard_header_len 	= 16;

 
	dev->priv=tp;
	tp->common=tc;
	tp->dev=dev;
	tc->vmac_num=0;			/* We only support one set of hardware. */

	dev->open = &vmac_Open;
	dev->hard_start_xmit = &vmac_start_xmit;	/* &vmac_dTx; */
	dev->stop = &vmac_Close;
	dev->get_stats = vmac_get_stats;		/*&Vmac_GetStats*/
	dev->set_multicast_list = vmac_set_rx_mode;
	dev->do_ioctl = vmac_ioctl;
	dev->base_addr = io_base[tc->vmac_num];
	dev->addr_len = 6;				/* Hardware Address Length in Bytes */ 

	if(probe_count ==0 ) {		/* Wan port */
		strcpy(dev->name,"ewan0");

		tc->priv[WAN_PORT]=tp;		/* Wan0 is on phy 0 */
		tp->phy_mask[WAN_PORT]=1;	/* The Wan0 uses only phy 0 */

		result = vmac_init_one(dev);
		if(result == 0){
			printk(VMAC "found VMAC #%d \n", probe_count);
			returnval = 0;
		}
	} else {			/* Lan port */

//		dev->tx_queue_len = 0;
		strcpy(dev->name, "eth0");
		tc->priv[LAN1_PORT]=tp;		/* Lan0 has phys 0-3 */
		tc->priv[LAN2_PORT]=tp;		/* Lan0 has phys 0-3 */
		tc->priv[LAN3_PORT]=tp;		/* Lan0 has phys 0-3 */
		tc->priv[LAN0_PORT]=tp;		/* Lan0 has phys 0-3 */
		tp->phy_mask[LAN0_PORT]=1;	/* The lan0 uses phys 0-3 */
		tp->phy_mask[LAN1_PORT]=1;	/* The lan0 uses phys 0-3 */
		tp->phy_mask[LAN2_PORT]=1;	/* The lan0 uses phys 0-3 */
		tp->phy_mask[LAN3_PORT]=1;	/* The lan0 uses phys 0-3 */
		printk(VMAC "Configured lan ports \n");
	}

	/* Assign MAC Address currently stored in adam2 environment variable */
	switch(tp->phy_mask[WAN_PORT])
	{
		case 1:				/* Wan0 port */
			mac_name="maca";
			break;
		case 0:				/* Lan0 port */
			mac_name="macb";
			break;
	}


	if(mac_name)
		mac_string=prom_getenv(mac_name);

	if(!mac_string)
	{
		if(!strcmp(mac_name,"maca"))
			mac_string="08.00.28.32.06.02";

                else
			mac_string="08.00.28.32.06.03";

		printk(VMAC "Error getting mac from BootLoader enviroment for %s\n",dev->name);
		printk(VMAC "Using default mac address: %s\n",mac_string);
		if(mac_name)
		{
			printk(VMAC "Use BootLoader command:\n");
			printk(VMAC "    setenv %s xx.xx.xx.xx.xx.xx\n","<env_name>");
			printk(VMAC "to set mac address\n");
		}
	}

	str2eaddr(mac_addr,mac_string);
	for (i=0; i <= ETH_ALEN; i++)
	{
		dev->dev_addr[i] = mac_addr[i];					/* This sets the hardware address */
	}

	probe_count++;					/* Increment probe count for next pass */
	return 0;
}

static void vmac_set_rx_mode (struct net_device *dev)
{
	/* We want the MAC to be in Promiscous mode */
       	VMAC_MACCONTROL(dev->base_addr) |= CAF;
}


static int vmac_ioctl (struct net_device *dev, struct ifreq *rq, int cmd)
{
	int ret = 0;
        TI_SNMP_CMD_T ti_snmp_cmd;
        struct vmac_common	*tc;
	struct vmac_priv	*tp;

        tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;


        if(cmd == SIOCDEVPRIVATE)
        {
            int dev_port;

            /* Now copy the user data */
            if(copy_from_user((char *)&ti_snmp_cmd,(char *)rq->ifr_data, sizeof(TI_SNMP_CMD_T)))
                return -EFAULT;


            if(!strcmp(rq->ifr_name,"ewan0"))
                dev_port = WAN_PORT;

            else if(ti_snmp_cmd.port >= LAN0_PORT && ti_snmp_cmd.port <= LAN3_PORT)
                dev_port = ti_snmp_cmd.port;

            else     
                dev_port = 0;   
	   
            

            switch (ti_snmp_cmd.cmd) {
	        case SIOCGINTFCOUNTERS:
                {
                    struct mib2_ifCounters mib_counter;

	            /* update vmac statistics */       
                    mib_counter.inBytesLow = wlanMib2GetCounter(dev_port, InBytes); 
                    mib_counter.inBytesHigh = wlanMib2GetCounter(dev_port, InBytesHigh);
                    mib_counter.inUnicastPktsLow = wlanMib2GetCounter(dev_port, InUnicastPkts);
                    mib_counter.inUnicastPktsHigh = wlanMib2GetCounter(dev_port, InUnicastPktsHigh);
                    mib_counter.inMulticastPktsLow = wlanMib2GetCounter(dev_port, InMulticastPkts);
                    mib_counter.inMulticastPktsHigh = wlanMib2GetCounter(dev_port, InMulticastPktsHigh);
                    mib_counter.inBroadcastPktsLow = wlanMib2GetCounter(dev_port, InBroadcastPkts); 
                    mib_counter.inBroadcastPktsHigh = wlanMib2GetCounter(dev_port, InBroadcastPktsHigh);
                    mib_counter.inDiscardPkts = wlanMib2GetCounter(dev_port, InDiscardPkts); 
                    mib_counter.inErrorPkts = wlanMib2GetCounter(dev_port, InErrorPkts);
                    mib_counter.inUnknownProtPkts = wlanMib2GetCounter(dev_port, InUnknownProtPkts);
	

                    mib_counter.outBytesLow = wlanMib2GetCounter(dev_port, OutBytes); 
                    mib_counter.outBytesHigh = wlanMib2GetCounter(dev_port, OutBytesHigh);
                    mib_counter.outUnicastPktsLow = wlanMib2GetCounter(dev_port, OutUnicastPkts);
                    mib_counter.outUnicastPktsHigh = wlanMib2GetCounter(dev_port, OutUnicastPktsHigh);
                    mib_counter.outMulticastPktsLow = wlanMib2GetCounter(dev_port, OutMulticastPkts);
                    mib_counter.outMulticastPktsHigh = wlanMib2GetCounter(dev_port, OutMulticastPktsHigh);
                    mib_counter.outBroadcastPktsLow = wlanMib2GetCounter(dev_port, OutBroadcastPkts); 
                    mib_counter.outBroadcastPktsHigh = wlanMib2GetCounter(dev_port, OutBroadcastPktsHigh);
                    mib_counter.outDiscardPkts = wlanMib2GetCounter(dev_port, OutDiscardPkts); 
                    mib_counter.outErrorPkts = wlanMib2GetCounter(dev_port, OutErrorPkts);
	
            
                    /* Now copy the dev counters to the user data */
                    if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&mib_counter, sizeof(struct mib2_ifCounters)))
		        ret = -EFAULT;
                }
                break;

	       case SIOCGINTFPARAMS:
	       {
	           struct mib2_ifParams    localParams;
                   int linkspeed;

                   linkspeed=EmacMdioGetSpeed(dev->base_addr, &(tc->phy_state[dev_port]));
            
		   localParams.ifSpeed = ((linkspeed)?100000000:10000000);

                   localParams.ifHighSpeed = (localParams.ifSpeed)/1000000;
                        
	           localParams.ifOperStatus = ((dev->flags & IFF_UP)?MIB2_STATUS_UP:MIB2_STATUS_DOWN);

                   localParams.ifPromiscuousMode           = ((dev->flags & IFF_PROMISC)?TRUE:FALSE);
            
	           /* Now copy the counters to the user data */
                   if(copy_to_user((char *)ti_snmp_cmd.data, (char *)&localParams, sizeof(struct mib2_ifParams)))
		       ret = -EFAULT;
	       }
               break; 

#if 0	
	      case  SIOCGETHERCOUNTERS:
	      {
	          struct mib2_phyCounters localCounters;

	           /* to be updated */    

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
#endif
	
	   case SIOCGETHERPARAMS:
	   {
	       struct mib2_ethParams localParams;
               int linkduplex;          
                              
               linkduplex=EmacMdioGetDuplex(dev->base_addr, &(tc->phy_state[dev_port]));      
               localParams.ethDuplexStatus = ((linkduplex)?MIB2_FULL_DUPLEX:MIB2_HALF_DUPLEX);
 
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


#define isprint(a) ((a >=' ')&&(a <= '~')) 


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
	unsigned long 		flags;
	struct net_device 	*dev;
	struct vmac_priv  	*tp;
	struct vmac_common 	*tc;
	int 				index, i;

	index = 0;

	dev = (struct net_device *) ptr;
	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	spin_lock_irqsave(&tc->tic_lock, flags);

	for(i = 0; i < NUMBER_OF_EMAC_PHYS; i++)
	{
		if(EmacMdioTic(dev->base_addr, &(tc->phy_state[index]))) 
		{
			vmac_do_mdio_statechange(dev, &(tc->phy_state[index]));
		}
		index++;
	}

	if(tc->mdio_tic_run) 
	{
		tc->mdio_tic_timer.expires=jiffies+MDIO_TIC_DELAY;
		add_timer(&(tc->mdio_tic_timer));
	}
	spin_unlock_irqrestore(&tc->tic_lock, flags); /* Changed from stat_lock!!!! */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Function: void vmac_do_mdio_statechange(struct net_device *dev, u32 PhyState)
*
* Description: Called from within the vmac_do_mdio_tic function when
* there is a state change. This will enable and disable the transmit
* functionality so that we don't send packets out of a dead port.
*/
static void vmac_do_mdio_statechange(struct net_device *dev, u32 *PhyState)
{
	int			linkstate;	/* returns 0 for no link, !0 for link */
	int			linkspeed;	/* returns 0 for 10Mbs, !0 for 100Mbs */
	int			linkduplex;	/* returns 0 for half-duplex */
	int			phy;		/* what phy are we using? */
	struct vmac_priv	*tp;
	struct vmac_common 	*tc;

	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	DPRINTK(1,"\n");
	DPRINTK(1,"MDIO statechange\n");
	linkstate=EmacMdioGetLinked(dev->base_addr, PhyState);
	if(linkstate == 0)
	{
		/* Report carrier down */
		netif_carrier_off(dev);
		DPRINTK(1,"Link down\n\n");
		return;
	}

	/* Link up, check the states */
	linkspeed=EmacMdioGetSpeed(dev->base_addr, PhyState);
	linkduplex=EmacMdioGetDuplex(dev->base_addr, PhyState);

	/* Report carrier up */
	netif_carrier_on(dev);

	/* Set MACPHY register. Do we need this? According to the example
	 * in the lib docs, we do. Why doesn't the library do this then? */
	phy= EmacMdioGetPhyNum(dev->base_addr, PhyState);
	
	DPRINTK(1,"Link up: Phy %d, Speed = %s, Duplex = %s\n", phy,
			(linkspeed)?"100":"10",(linkduplex)?"Full":"Half");
}

int vmac_p_read_rfc2665_stats(char* buf, char **start, off_t offset, int count, int *eof, void *data)
{
	struct net_device *dev;
	int len=0;
	int limit = count - 80;
	uint device_num;
	int i;
 	struct vmac_stat_regs *vmac_stats;
	int linkspeed,linkduplex;

	struct vmac_priv	*tp;
	struct vmac_common 	*tc;

	dev=VmacDevices;
	if(!dev)
		goto proc_error;

	if(!offset)						/* Request for new stats */
	{
	}
//	printk("Offset = 0x%lx\n", offset);

	device_num= (offset >> 4) & 0x0f;			/* extract device number */
	for(i=0; i<device_num; i++)				/* Go to device number */
	{
		if(!dev)
			goto proc_error;
		tp = (struct vmac_priv *) dev->priv;
		tc = (struct vmac_common *) tp->common;
	
		if(!tc)
			goto proc_error;
		dev=tc->nextDevice;
		if(!dev)
			goto proc_error;
	}

	if(i >= VmacDevicesInstalled)				/* Last entry */
		*eof=1;
	else
	{
		if( (offset & 0x01) == 0)
			*start=(char*)( (i+1)<<4);		/* Next device, first info type */
		else
			*start=(char*)( (i<<4) + 1);		/* Same device, next info type */
	}

        vmac_update_stats(dev);

	/* Get Stats */
 	vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);
	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	linkspeed=EmacMdioGetSpeed(dev->base_addr, &(tc->phy_state[WAN_PORT]));  

	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: N/A\n", "ifSpeed");
       

	linkduplex=EmacMdioGetDuplex(dev->base_addr, &(tc->phy_state[WAN_PORT])); 

 
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: N/A\n", "dot3StatsDuplexStatus"); 

	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %d\n", "ifAdminStatus", ((dev->flags & IFF_UP)?1:2));

	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifOperStatus",(long)((dev->flags & IFF_UP)? 1:2));

    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInDiscards", tc->stats.rx_dropped);

	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInErrors", tc->stats.rx_errors);
    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutDiscards", tc->stats.tx_dropped);

	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutErrors", tc->stats.tx_errors);


	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInGoodFrames", tc->stats.rx_packets);
    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInBroadcasts", vmac_stats->broadcast_rx_frames);
    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInMulticasts", vmac_stats->multicast_rx_frames);
    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInPauseFrames", vmac_stats->pause_rx_frames);
    
	if(len <= limit)
		len+= sprintf(buf + len, "%-35s: %lu\n", "ifInCRCErrors", vmac_stats->rx_crc_errors);
    
	if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInAlignCodeErrors",vmac_stats-> rx_align_code_errors);
    
   if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInOversizedFrames",vmac_stats->oversized_rx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInJabberFrames",vmac_stats->rx_jabbers);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInUndersizedFrames",vmac_stats->undersized_rx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInFragments",vmac_stats->rx_fragments);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInFilteredFrames",vmac_stats->filtered_rx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInQosFilteredFrames",vmac_stats->filtered_rx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInOctets",vmac_stats->rx_octets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutGoodFrames",vmac_stats->good_tx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutBroadcasts",vmac_stats->broadcast_tx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutMulticasts", vmac_stats->multicast_tx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutPauseFrames",vmac_stats->pause_tx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifDeferredTransmissions",vmac_stats->deferred_tx_frames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifCollisionFrames", vmac_stats->collisions);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifExcessiveCollisionFrames", vmac_stats->excessive_collisions);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifLateCollisions", vmac_stats->late_collisions);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifCarrierSenseErrors", vmac_stats->carrier_sense_errors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutOctets", vmac_stats->tx_octets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if64OctetFrames", vmac_stats->frames_64octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if65To127POctetFrames",vmac_stats->frames_65_127octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if128To255OctetFrames", vmac_stats->frames_128_255octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if256To511OctetFrames", vmac_stats->frames_256_511octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if512To1023OctetFrames", vmac_stats->frames_256_511octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if1024ToUpOctetFrames", vmac_stats->frames_512_1023octet);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifNetOctets", vmac_stats->tx_octets + vmac_stats->rx_octets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifRxSofOverruns", vmac_stats-> rx_overruns);
		
    return len;

proc_error:
    *eof=1;
    return len;
}


/*********************************************************
* Function: vmac_read_proc
*
* Description: This returns statitics through the proc filesystem.
* This method uses the proc hack where it returns an int back through
* start to indicate the next offset. The offset is broken up like thus:
* bits 31:8 = 0
* bits 7:4  = device number
* bits 3:0  = information type. (0 transmit, 1 receive )
*/

static int vmac_read_proc(char* buf, char **start, off_t offset, int count,
		int *eof, void *data)
{
	struct net_device *dev;
	int len=0;
	int limit = count - 80;
	uint device_num;
	int i;
 	struct vmac_stat_regs *vmac_stats;

	struct vmac_priv	*tp;
	struct vmac_common 	*tc;

	dev=VmacDevices;
	if(!dev)
		goto proc_error;

	if(!offset)						/* Request for new stats */
	{
	}
//	printk("Offset = 0x%lx\n", offset);

	device_num= (offset >> 4) & 0x0f;			/* extract device number */
	for(i=0; i<device_num; i++)				/* Go to device number */
	{
		if(!dev)
			goto proc_error;
		tp = (struct vmac_priv *) dev->priv;
		tc = (struct vmac_common *) tp->common;
	
		if(!tc)
			goto proc_error;
		dev=tc->nextDevice;
		if(!dev)
			goto proc_error;
	}

	if(i >= VmacDevicesInstalled)				/* Last entry */
		*eof=1;
	else
	{
		if( (offset & 0x01) == 0)
			*start=(char*)( (i+1)<<4);		/* Next device, first info type */
		else
			*start=(char*)( (i<<4) + 1);		/* Same device, next info type */
	}

	/* Get Stats */
 	vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);
	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

		/* Transmit stats */
		if(len<=limit)
			len+= sprintf(buf+len, "\nVmac %d, Address %lx\n",VmacDevicesInstalled-i,dev->base_addr);
		if(len<=limit)
			len+= sprintf(buf+len, " Transmit Stats\n");
		if(len<=limit)
			len+= sprintf(buf+len, "   Tx Valid Bytes Sent        :%lu\n",vmac_stats->tx_octets);
		if(len<=limit)
			len+= sprintf(buf+len, "   Good Tx Frames (Hardware)  :%lu\n",vmac_stats->good_tx_frames);
		if(len<=limit)
			len+= sprintf(buf+len, "   Good Tx Frames (Software)  :%lu\n",(long unsigned int)tc->good_tx_sw_frames);
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
			len+= sprintf(buf+len, "   SQE Errors       	      :%lu\n",vmac_stats->sqe_test_errors);
		if(len<=limit)
			len+= sprintf(buf+len, "   Deferred Errors 	      :%lu\n",vmac_stats->deferred_tx_frames);
		if(len<=limit)
			len+= sprintf(buf+len, "   Late Collisions     	      :%lu\n",vmac_stats->late_collisions);
		if(len<=limit)
			len+= sprintf(buf+len, "   Excessive Collisions	      :%lu\n",vmac_stats->excessive_collisions);
		if(len<=limit)
			len+= sprintf(buf+len, "   Single Collisions	      :%lu\n",vmac_stats->single_col_tx_frames);
		if(len<=limit)
			len+= sprintf(buf+len, "   Multiple Collisions	      :%lu\n",vmac_stats->mul_col_tx_frames);
	
		if(len<=limit)
			len+= sprintf(buf+len, "\n");


		/* Receive Stats */
		if(len<=limit)
			len+= sprintf(buf+len, " Receive Stats\n");
		if(len<=limit)
			len+= sprintf(buf+len, "   Rx Valid Bytes Received    :%lu\n",vmac_stats->rx_octets);
		if(len<=limit)
			len+= sprintf(buf+len, "   Good Rx Frames (Hardware)  :%lu\n",vmac_stats->good_rx_frames);
		if(len<=limit)
			len+= sprintf(buf+len, "   Good Rx Frames (Software)  :%lu\n",(long unsigned int)tc->good_rx_sw_frames);
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
		if(len<=limit)
			len+= sprintf(buf+len, "   Rx Jabbers                 :%lu\n",vmac_stats->rx_jabbers);

		
	return len;

proc_error:
	*eof=1;
	return len;
}



/* Resetting statistics  */
static void vmac_reset_stats(struct net_device *dev)
{
    struct vmac_common	*tc;
    struct vmac_priv	*tp;
    
    tp = (struct vmac_priv *) dev->priv;
    tc = (struct vmac_common *) tp->common;
   
    tc->good_tx_sw_frames = 0;
    tc->good_rx_sw_frames = 0;
    

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
	    struct vmac_common	*tc;
	    struct vmac_priv	*tp;

            /* Valid command */
	    printk("Resetting statistics for VMAC interface.\n");

            dev = VmacDevices;  
	    tp = (struct vmac_priv *) dev->priv;
	    tc = (struct vmac_common *) tp->common;
  
            /* Reverse the the device link list to list eth0,eth1...in correct order */
            for(i=0; i<num_devices; i++)
            {
                vmac_dev_list[num_devices-(i+1)]=dev;
                dev = tc->nextDevice;
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

static struct net_device_stats *vmac_get_stats (struct net_device *dev)
{
	struct vmac_priv	*tp;
	struct vmac_common 	*tc;
	unsigned long flags;
	
	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	spin_lock_irqsave(&tc->stats_lock, flags);
	vmac_update_stats(dev); 
	spin_unlock_irqrestore(&tc->stats_lock, flags);

	return &tc->stats;
}

static void     vmac_update_stats(struct net_device *dev)
{
	struct vmac_priv	*tp;
	struct vmac_common 	*tc;
	struct vmac_stat_regs *vmac_stats = (struct vmac_stat_regs *)VMAC_STATS_BASE(dev->base_addr);
	
	tp = (struct vmac_priv *) dev->priv;
	tc = (struct vmac_common *) tp->common;

	/* Debug stats Rx Frames */
	/* Drivers stats appears correct, ifconfig & netstat suspect */
  	/* Debug stats Tx Frames */

	tc->stats.tx_packets=vmac_stats->good_tx_frames;
	tc->stats.rx_packets=vmac_stats->good_rx_frames;
	tc->stats.tx_bytes=vmac_stats->tx_octets;
	tc->stats.rx_bytes=vmac_stats->rx_octets;

	/* ethernet stats */ 

	/* Error conditions */
	tc->stats.rx_errors = vmac_stats->rx_fragments
				+ vmac_stats->undersized_rx_frames
	  			+ vmac_stats->rx_crc_errors
	  			+ vmac_stats->rx_align_code_errors
	  			+ vmac_stats->rx_jabbers
	  			+ vmac_stats->rx_overruns;
	tc->stats.tx_errors = vmac_stats->tx_error_frames;
	tc->stats.multicast = vmac_stats->multicast_rx_frames;     /* 0x040c */
	tc->stats.collisions = vmac_stats->collisions;             /* 0x0478 */

	tc->stats.rx_crc_errors = vmac_stats->rx_crc_errors;       /* 0x0410 */
}

/*********************************************************
* Function: vmac_get_private_structure
*
* Description: This returns the private data structure for 
* the given device port.
*/

struct vmac_priv *vmac_get_private_structure(int dev_port)
{
	struct net_device 	*dev;
	struct vmac_common	*tc;
	struct vmac_priv	*tp;

	if ((dev = VmacDevices) == NULL)
		return NULL;
	
	if ((tp = dev->priv) == NULL)
		return NULL;
	
	if ((tc = tp->common) == NULL)
		return NULL;

	tp = tc->priv[dev_port];
	return (tp);
}

