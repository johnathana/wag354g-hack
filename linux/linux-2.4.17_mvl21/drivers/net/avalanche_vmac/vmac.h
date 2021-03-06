/* vmac.h */

#ifndef __AVMAC_H
#define __AVMAC_H


/* prototypes */

void            xdump( u_char*  cp, int  length, char*  prefix );

int      vmac_probe(struct net_device *dev);
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
static int		vmac_read_proc(char* page, char **start, off_t offset, int count,
								int *eof, void *data);

static void     vmac_update_stats(struct net_device *dev);
static struct net_device_stats *vmac_get_stats(struct net_device *dev);

static int      vmac_irq_request(struct net_device *dev);
static void     vmac_free_irq(struct net_device *dev);
static void	vmac_txirq(struct net_device *dev);
static void	vmac_tcra_thresh(struct net_device *dev);
static void	vmac_tcr_freeze(struct net_device *dev);
static void	vmac_rxirq(struct net_device *dev);
static void	vmac_rcra_thresh(struct net_device *dev);
static void	vmac_rcr_freeze(struct net_device *dev);
static void     vmac_linkirq(int irq, void *dev_id, struct pt_regs *regs);
static void     vmac_mgtirq(int irq, void *dev_id, struct pt_regs *regs);
static void		vmac_do_mdio_tic(unsigned long ptr);
static void		vmac_do_mdio_statechange(struct net_device *dev);

static void     *m_alloc(int size, int a_shift);
static void     m_free(void *ptr, int size);

#include <asm/avalanche/avalanche_regs.h>
#include <asm/avalanche/vmac_bits.h>

#define VMAC_ERROR "VMAC ERRROR:"
#define VMAC "VMAC: "


#if defined(CONFIG_MIPS_EVM3D) || defined(CONFIG_MIPS_AR5D01)
#define TRANSMIT_INT_THRESH	65
#define NUM_TX_COMP      128
#define NUM_RX_COMP      128
#define NUM_TX_BUF       144          /* number of Transmit buffer descriptors */
#define NUM_RX_BUF       144          /* number of Receive buffer descriptors */
#define BURST_THRESHOLD	50
#else /* CONFIG_MIPS_EVM3 || CONFIG_MIPS_ACPEP */
#define TRANSMIT_INT_THRESH	130
#define NUM_TX_COMP      256
#define NUM_RX_COMP      256
#define NUM_TX_BUF       300          /* number of Transmit buffer descriptors */
#define NUM_RX_BUF       300          /* number of Receive buffer descriptors */
#define BURST_THRESHOLD	100
#endif  /* CONFIG_MIPS_EVM3D */


#define MIN_FRAME_SIZE   64
#define MAX_FRAME_SIZE 1600           /* make sure rec desc size is plenty big */
                                      /* VW_TODO: Optimize later */
#define MDIO_TIC_DELAY	5			  /* Number of jiffies between timer tics */

/* Ethernet PHY register definitions */
#define PHYRESET         (1<<15)    // Ethernet Phy reset
#define PHYLOOPBACK      (1<<14)    // Ethernet Phy loopback
#define PHYFULLDUPLEX    (1<<8)     // Set Phy to Full Duplex


#if defined(__LITTLE_ENDIAN_BITFIELD)
#define BITFIELD2(b1, b2)                            b1; b2;
#define BITFIELD3(b1, b2, b3)                        b1; b2; b3;
#define BITFIELD4(b1, b2, b3, b4)                    b1; b2; b3; b4;
#define BITFIELD5(b1, b2, b3, b4, b5)                b1; b2; b3; b4; b5;
#define BITFIELD6(b1, b2, b3, b4, b5, b6)            b1; b2; b3; b4; b5; b6;
#define BITFIELD7(b1, b2, b3, b4, b5, b6, b7)        b1; b2; b3; b4; b5; b6; b7;
#define BITFIELD8(b1, b2, b3, b4, b5, b6, b7, b8)    b1; b2; b3; b4; b5; b6; b7; b8;

#elif defined(__BIG_ENDIAN_BITFIELD)
#define BITFIELD2(b1, b2)                            b2; b1;
#define BITFIELD3(b1, b2, b3)                        b3; b2; b1;
#define BITFIELD4(b1, b2, b3, b4)                    b4; b3; b2; b1;
#define BITFIELD5(b1, b2, b3, b4, b5)                b5; b4; b3; b2; b1;
#define BITFIELD6(b1, b2, b3, b4, b5, b6)            b6; b5; b4; b3; b2; b1;
#define BITFIELD7(b1, b2, b3, b4, b5, b6, b7)        b7; b6; b5; b4; b3; b2; b1;
#define BITFIELD8(b1, b2, b3, b4, b5, b6, b7, b8)    b8; b7; b6; b5; b4; b3; b2; b1;

#else
#error unknown bitfield endianess
#endif

/* Memory Allocation defines (used with m_alloc, mfree */

#define ALIGN_SIZE(shift)	(1UL << shift)
#define ALIGN_MASK(shift)	(~(ALIGN_SIZE(shift)-1))

#define CACHE_LINE_SHIFT	5
#define CACHE_LINE_SIZE		ALIGN_SIZE(CACHE_LINE_SHIFT)
#define CACHE_LINE_MASK		ALIGN_MASK(CACHE_LINE_SHIFT)


/* VMAC Device structure Information */

/* Transmit buffer descriptors */
typedef struct tx_fbld_s
{
	unsigned int mode;
	unsigned char  *BufPtr;     /* Pointer to Data Buffer */
	struct tx_fbld_s *next;  /* pointer to next descriptor */
	BITFIELD2(
		u32 Fbl_id : 16,  /* free buffer list ID: N.A. For VMAC */
		u32 offset : 16  /* offset of valid data from start of buffer */
	)
	struct sk_buff *skb;     /* skb associated with the buffer descriptor */
	u32	temp2;		/* Temp words to make the complete structure 8 words big */
	u32	temp3;
	u32	temp4;
}tx_fbld_t;

/* Receive buffer descriptors */
typedef struct rx_fbld_s
{
	BITFIELD4(
		u32 size : 16,            /* valid byte count */
		u32 reserv1: 14,          /* Reserved         */
		u32 eop: 1,               /* End of Packet  */
		u32 sop: 1                /* Start of packet    */
	)
	unsigned int BufPtr;          /* pointer to data buffer */
	unsigned int next;            /* pointer to next descriptor */
	BITFIELD2(
		u32 Fbl_id : 16,          /* free buffer list ID: N.A. For VMAC */
		u32 offset : 16           /* offset of valid data from start of buffer */
	)
	struct sk_buff *skb;     /* skb associated with the buffer descriptor */
	u32	temp2;		/* Temp words to make the complete structure 8 words big */
	u32	temp3;
	u32	temp4;
}rx_fbld_t; 

/* Tx Completion Ring Entry */
typedef struct
{
  u32 status;
  u32 fblid_noofbufs;
  u32 sof_list;
  u32 eof_list;
}tx_compring; 

/* Rx Completion Ring Entry */
typedef struct
{
  BITFIELD8(
     u32 num_buffs : 8,  /* number of free buffers */
     u32 unused1   : 16, /* reserved               */
     u32 fbl_id    : 2,  /* fbl_id                 */
     u32 ch_off    : 1,  /* channel off            */
     u32 unused2   : 2,  /* reserved               */
     u32 ext_sts   : 1,  /* extended status        */
     u32 error_ind : 1,  /* frame error            */
     u32 valid     : 1   /* reserved               */
     )
  int sof_list;
  int eof_list;
  BITFIELD2(
     u32 errorbits : 16,         /* Error bits   */
     u32 frame_len : 16          /* Frame length */
  )
}rx_compring;

/* MAC Address Structure */
typedef struct
{
  BITFIELD4(
     u32 byte4 : 8,      /* 4th Byte of MAC addr, See Spec */
     u32 byte3 : 8,      /* 3rd Byte of MAC addr, See Spec */
     u32 byte2 : 8,      /* 2nd Byte of MAC addr, See Spec */
     u32 byte1 : 8       /* 1rst Byte of MAC addr, See Spec */
     )
}mac_addr_t; 

/* Driver Private Info data Structure */

typedef struct vmac_private_tag {
  struct net_device  *nextDevice;
  struct net_device_stats stats;   /* Ethernet statistics */
  struct timer_list	 mdio_tic_timer; /* Timer for lib tick */
  rwlock_t			 mdio_tic_lock;	 /* Spin lock for timer */
  int				 mdio_tic_run;	 /* Timer continuation flag */
  int                vmac_num;       /* Emac device number 0,1 */
  spinlock_t         stats_lock;	 /* Locks the status functions */
  spinlock_t         tic_lock;	 /* Locks the status functions */
  int                phy_addr;       /* Ethernet Phy address number   */           
  unsigned int	     tx_irq;	     /* Ethernet MAC TX IRQ number    */
  unsigned int	     rx_irq;	     /* Ethernet MAC RX IRQ number    */
  unsigned int       lnk_irq;        /* Ethernet link change int      */
  unsigned int       mgt_irq;        /* Ethernet management int       */
  u32			rx_burstRCRcount;
  u32			rx_burstbufcount;
  rx_fbld_t*		rx_burstbufstart;
  rx_fbld_t*		rx_burstbufstop;
  tx_fbld_t          *txfbl_head;    /* Transmit FBL descriptor head  */
  tx_fbld_t          *txfbl_tail;    /* Transmit FBL descriptor tail  */
  int                txfbl_num;      /* Number of Tx descriptor node */
  spinlock_t         txfbl_lock;     /* Spinlock for the Tx FBL       */
  rx_fbld_t          *rxfbl;         /* Rx free buffer descriptor list */
  tx_compring        *txcr;          /* Tx Completion Ring Ptr */
  tx_compring        *txcr_start;    /* first comp ring structure  */
  tx_compring        *txcr_end;      /* last comp ring structure    */
  rx_compring        *rxcr;          /* Rx Completion Ring Ptr */
  rx_compring        *rxcr_start;    /* first comp ring structure  */
  rx_compring        *rxcr_end;      /* last comp ring structure    */
  mac_addr_t         *mac_addr_hi;   /* MAC addr for device       */
  mac_addr_t         *mac_addr_low;  /* MAC addr for device       */
  u32				 phy_state;		 /* Storage for phy state  */
  u32				 good_tx_sw_frames;	/* Number of good frames */
  u32				 good_rx_sw_frames;	/* Number of good frames */
  u32				 frame_error;
	u32		int_mask;
	u32		skbtotal;
  int rxfbl_initialized;              /* RxFBL initialized */
} VmacPrivateInfo;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int Get_TxDesc()
 *
 *  Description: get a Tx FBL descriptor
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static inline tx_fbld_t *
Get_TxDesc(VmacPrivateInfo *priv)
{
	tx_fbld_t *head;
	unsigned long flags;

	spin_lock_irqsave(&priv->txfbl_lock,flags);
	if(!priv->txfbl_num){
		spin_unlock_irqrestore(&priv->txfbl_lock,flags);
		return NULL;
	}

	head = priv->txfbl_head;
	priv->txfbl_head = priv->txfbl_head->next;
	priv->txfbl_num--;

	spin_unlock_irqrestore(&priv->txfbl_lock,flags);
	return head;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int Put_TxDesc()
 *
 *  Description: put a Tx FBL descriptor
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static inline int 
Put_TxDesc(tx_fbld_t *tx_desc, VmacPrivateInfo *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->txfbl_lock,flags);
	priv->txfbl_tail->next  = tx_desc;
	priv->txfbl_tail  = tx_desc;
	priv->txfbl_tail->next  = NULL;
	priv->txfbl_num++;
	spin_unlock_irqrestore(&priv->txfbl_lock,flags);

	return 0;
}
#endif __AVMAC_H
