/*
Experimental ethernet netdevice using ATM AAL5 as underlying carrier
(RFC1483 obsoleted by RFC2684) for Linux 2.4
Author: Marcell GAL, 2000, XDSL Ltd, Hungary
*/

//-------------------------------------------------------------------------------------
// Copyright 2004, Texas Instruments Incorporated
//
// This program has been modified from its original operation by Texas Instruments
// to do the following:
// 
// 1) Performance optimizations
// 2) WAN Bridge Support.
//
// THIS MODIFIED SOFTWARE AND DOCUMENTATION ARE PROVIDED
// "AS IS," AND TEXAS INSTRUMENTS MAKES NO REPRESENTATIONS 
// OR WARRENTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
// TO, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY 
// PARTICULAR PURPOSE OR THAT THE USE OF THE SOFTWARE OR 
// DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS, 
// COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS. 
// See The GNU General Public License for more details.
//
// These changes are covered under version 2 of the GNU General Public License, 
// dated June 1991.
//-------------------------------------------------------------------------------------

#include <linux/module.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <net/arp.h>
#include <linux/rtnetlink.h>
#include <linux/ip.h>
#include <linux/atmbr2684.h>

#include "ipcommon.h"

//#define BR2684_DEBUG
#ifdef BR2684_DEBUG
#include <linux/string.h>	/* for xdump */
#include <linux/ctype.h>	/* for xdump */
#endif

/* For WAN BRIDGE Support. */
#include <net/wb.h>

extern int keep_alive;

/*
 * Define this to use a version of the code which interacts with the higher
 * layers in a more intellegent way, by always reserving enough space for
 * our header at the begining of the packet.  However, there may still be
 * some problems with programs like tcpdump.  In 2.5 we'll sort out what
 * we need to do to get this perfect.  For now we just will copy the packet
 * if we need space for the header
 */
/* #define FASTER_VERSION */

#ifdef DEBUG
#define DPRINTK(format, args...) printk(KERN_DEBUG "br2684: " format, ##args)
#else
#define DPRINTK(format, args...)
#endif

#ifdef SKB_DEBUG
static void skb_debug(const struct sk_buff *skb)
{
#define NUM2PRINT 50
	char buf[NUM2PRINT * 3 + 1];	/* 3 chars per byte */
	int i = 0;
	for (i = 0; i < skb->len && i < NUM2PRINT; i++) {
		sprintf(buf + i * 3, "%2.2x ", 0xff & skb->data[i]);
	}
	printk(KERN_DEBUG "br2684: skb: %s\n", buf);
}
#else
#define skb_debug(skb)	do {} while (0)
#endif


/* NSP Optimizations: This macro controls measuring the amount of time taken to 
 * execute the functions in the fast path. This should only be defined for debugging. */
#undef TI_BR2684_MEASUREMENTS

#ifdef TI_BR2684_MEASUREMENTS

/* Display the warning message so that the user knows that measurements are
 * being compiled into the code. */
#warning "*********************************************************************"
#warning "BR2684 Measurements are being compiled into the code"
#warning "*********************************************************************"

/* Measurement: Macro */
#define rdtscl(dest) \
	__asm__ __volatile__("mfc0 %0,$9; nop" : "=r" (dest))

extern int br2684_push_start;
extern int br2684_push_end;

#endif /* TI_BR2684_MEASUREMENTS */


static unsigned char llc_oui_pid_pad[] =
    { 0xAA, 0xAA, 0x03, 0x00, 0x80, 0xC2, 0x00, 0x07, 0x00, 0x00 };
#define PADLEN	(2)
static unsigned char llc_oui_ethertype[] =
    { 0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00 };

enum br2684_encaps {
	e_vc  = BR2684_ENCAPS_VC,
	e_llc = BR2684_ENCAPS_LLC,
	e_vc_routed  = BR2684_ENCAPS_VC_ROUTED,
	e_llc_routed = BR2684_ENCAPS_LLC_ROUTED,
};

struct br2684_vcc {
	struct atm_vcc  *atmvcc;
	struct br2684_dev *brdev;
	/* keep old push,pop functions for chaining */
	void (*old_push)(struct atm_vcc *vcc,struct sk_buff *skb);
	/* void (*old_pop)(struct atm_vcc *vcc,struct sk_buff *skb); */
	enum br2684_encaps encaps;
	struct list_head brvccs;
#ifdef CONFIG_ATM_BR2684_IPFILTER
	struct br2684_filter filter;
#endif /* CONFIG_ATM_BR2684_IPFILTER */
#ifndef FASTER_VERSION
	unsigned copies_needed, copies_failed;
#endif /* FASTER_VERSION */
};

struct br2684_dev {
	struct net_device net_dev;
	struct list_head br2684_devs;
	int number;
	struct list_head brvccs; /* one device <=> one vcc (before xmas) */
	struct net_device_stats stats;
	int mac_was_set;

    /* Added for WAN Bridge support. */
    void *ptr_bridge;
};

                                                                                                                                                             
static int BuildAndSendArpResp(struct sk_buff * skb, struct br2684_dev *brdev, struct br2684_vcc *brvcc);
__u16 swap16(__u16 orig_16);
#ifdef BR2684_DEBUG
void xdump( u_char*  cp, int  length, char*  prefix );
#endif
//#define BR2684_DEBUG

/* NSP Optimizations: These functions optimize the receive path of the 2684
 * layer. */
#ifndef TI_SLOW_PATH
static int maccmp(unsigned char* addr1, unsigned char* addr2);
static unsigned short br2684_type_trans(struct sk_buff *skb, struct net_device *dev);
#endif /* TI_SLOW_PATH */

/*
 * This lock should be held for writing any time the list of devices or
 * their attached vcc's could be altered.  It should be held for reading
 * any time these are being queried.  Note that we sometimes need to
 * do read-locking under interrupt context, so write locking must block
 * the current CPU's interrupts
 */
static rwlock_t devs_lock = RW_LOCK_UNLOCKED;

static LIST_HEAD(br2684_devs);

//cjg:2004-5-11;add for pppoe-passthrough
volatile unsigned char wan_dev_name[5];

static inline struct br2684_dev *BRPRIV(const struct net_device *net_dev)
{
	return list_entry(net_dev, struct br2684_dev, net_dev);
}

static inline struct br2684_dev *list_entry_brdev(const struct list_head *le)
{
	return list_entry(le, struct br2684_dev, br2684_devs);
}

static inline struct br2684_vcc *BR2684_VCC(const struct atm_vcc *atmvcc)
{
	return (struct br2684_vcc *) (atmvcc->user_back);
}

static inline struct br2684_vcc *list_entry_brvcc(const struct list_head *le)
{
	return list_entry(le, struct br2684_vcc, brvccs);
}

/* Caller should hold read_lock(&devs_lock) */
static struct br2684_dev *br2684_find_dev(const struct br2684_if_spec *s)
{
	struct list_head *lh;
	struct br2684_dev *brdev;
	switch (s->method) {
	case BR2684_FIND_BYNUM:
		list_for_each(lh, &br2684_devs) {
			brdev = list_entry_brdev(lh);
			if (brdev->number == s->spec.devnum)
				return brdev;
		}
		break;
	case BR2684_FIND_BYIFNAME:
		list_for_each(lh, &br2684_devs) {
			brdev = list_entry_brdev(lh);
			if (!strncmp(brdev->net_dev.name, s->spec.ifname,
			    sizeof brdev->net_dev.name))
				return brdev;
		}
		break;
	}
	return NULL;
}

/*
 * Send a packet out a particular vcc.  Not to useful right now, but paves
 * the way for multiple vcc's per itf.  Returns true if we can send,
 * otherwise false
 */
static int br2684_xmit_vcc(struct sk_buff *skb, struct br2684_dev *brdev,
	struct br2684_vcc *brvcc)
{
	struct atm_vcc *atmvcc;
	int minheadroom = 0;
	struct ether_header *   pEther;
	struct ether_arp  *	pArpPacket;

	/****
	 *  If 2684 routed, and if an ARP, then spoof a Success/RARP back up to IP stack
	 ****/

	if((brvcc->encaps ==  e_llc_routed) || (brvcc->encaps == e_vc_routed))
	{
		pEther = (struct ether_header *)(skb->data);

		/* check if ether type field == ARP (0x0806) */
		if ((pEther->ether_type) == swap16(ETHERTYPE_ARP))
		{
			pArpPacket = (struct ether_arp *) (((unsigned char *)pEther) + sizeof(struct ether_header));
			if ((pArpPacket->ea_hdr.ar_op) == swap16(ARPOP_REQUEST))
			{
				/* if so, build and send ARP response */
				if(!(BuildAndSendArpResp(skb, brdev, brvcc)))
				{
					/* Something went wrong */
					printk("\nBR2684 Routed: Couldn't respond to ARP Req!\n");
					dev_kfree_skb(skb);
					return 0;
				}
				return 1;	/* Successfully sent ARP response back up stack */
			}
			else
			{
				printk("\nBR2684 Routed -- Rx unexpected ARP! (0x%x)\n",pArpPacket->ea_hdr.ar_op);
				dev_kfree_skb(skb);	/* this should _never_ happen, but free skb just in case */
				return 0;
			}
		}
	}
	
#ifdef FASTER_VERSION
	if (brvcc->encaps == e_llc)
		memcpy(skb_push(skb, 8), llc_oui_pid_pad, 8);
	/* last 2 bytes of llc_oui_pid_pad are managed by header routines;
	   yes, you got it: 8 + 2 = sizeof(llc_oui_pid_pad)
	 */
	if (brvcc->encaps == e_llc_routed)
		memcpy(skb_push(skb, 8), llc_oui_ethertype, 8);
#else
	switch(brvcc->encaps) {
		case BR2684_ENCAPS_LLC:
			minheadroom = 10;
			break;
		case BR2684_ENCAPS_VC:
			minheadroom = 2;
			break;
		case BR2684_ENCAPS_LLC_ROUTED: {
			minheadroom = 8;
			skb_pull(skb,ETH_HLEN);		/* remove ETH header for routed mode */
			break;
			}
		case BR2684_ENCAPS_VC_ROUTED: {
			minheadroom = 0;
			skb_pull(skb,ETH_HLEN);		/* remove ETH header for routed mode */
			break;
			}
	}
	if (skb_headroom(skb) < minheadroom) {
		struct sk_buff *skb2 = skb_realloc_headroom(skb, minheadroom);
		brvcc->copies_needed++;
		dev_kfree_skb(skb);
		if (skb2 == NULL) {
			brvcc->copies_failed++;
			return 0;
		}
		skb = skb2;
	}
	skb_push(skb, minheadroom);
	if (brvcc->encaps == e_llc)
		memcpy(skb->data, llc_oui_pid_pad, 10);
	if (brvcc->encaps == e_vc)
		memset(skb->data, 0, 2);
	if (brvcc->encaps == e_llc_routed)
		memcpy(skb->data, llc_oui_ethertype, 8);
#endif /* FASTER_VERSION */
	skb_debug(skb);

	ATM_SKB(skb)->vcc = atmvcc = brvcc->atmvcc;
	DPRINTK("atm_skb(%p)->vcc(%p)->dev(%p)\n", skb, atmvcc, atmvcc->dev);

/* ***IMPORTANT*** 
 * Commented the following check to stop dropping the packet due to a 
 * low value of "sndbuf" as against the driver's queueing capacity. 
 * This causes the scheduler not to become the bottle-neck for xmit.
 * All the packets are sent to the driver, and driver is expected to 
 * call "netif_stop_queue" when its queue becomes full and also call
 * "netif_wake_queue" once the queue has few slots. */
#if 0	
	if (!atm_may_send(atmvcc, skb->truesize)) {
		/* we free this here for now, because we cannot know in a higher 
			layer whether the skb point it supplied wasn't freed yet.
			now, it always is.
		*/
		dev_kfree_skb(skb);
		return 0;
		}
#endif
	atomic_add(skb->truesize, &atmvcc->tx_inuse);
	ATM_SKB(skb)->iovcnt = 0;
	ATM_SKB(skb)->atm_options = atmvcc->atm_options;
	brdev->stats.tx_packets++;
	brdev->stats.tx_bytes += skb->len;
#ifdef BR2684_DEBUG
	printk("\n");
	xdump(skb->data, skb->len,"2684TX->SAR: ");
	printk("\n");
#endif
	atmvcc->send(atmvcc, skb);
	return 1;
}

static inline struct br2684_vcc *pick_outgoing_vcc(struct sk_buff *skb,
	struct br2684_dev *brdev)
{
	return list_empty(&brdev->brvccs) ? NULL :
	    list_entry_brvcc(brdev->brvccs.next); /* 1 vcc/dev right now */
}
/********** Start by zhangbin **************/
/* Add padding for Huawei DSLAM
 * 2006.04.24
 */
#define PADDING_LEN	60
/********** End by zhangbin ****************/

static int br2684_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct br2684_dev *brdev = BRPRIV(dev);
	struct br2684_vcc *brvcc;

	/********** Start by zhangbin **************/
	/* Add padding for Huawei DSLAM
	 * 2006.04.24
	 */
	int tmp_len;
	/********** End by zhangbin ****************/

	DPRINTK("br2684_start_xmit, skb->dst=%p\n", skb->dst);
	
	/*************** Start by zhangbin *******************/
	/* Add padding for Huawei DSLAM
	 * 2006.04.24
	 */
#if 1
	//zhangbin
	//printk("%s:skb->len=%d\n",__FUNCTION__,skb->len);
	
	{
		if(skb->len < PADDING_LEN)
		{
			int tmp_padding,tmp_cnt;
			tmp_len = skb->len;
			tmp_padding = PADDING_LEN - tmp_len;
			if(pskb_expand_head(skb,0,tmp_padding,GFP_ATOMIC))
				return -ENOMEM;
			//printk("1:%s,tmp_padding=%d,new:skb->len=%d\n",__FUNCTION__,tmp_padding,skb->len);
			skb_put(skb,tmp_padding);
			//printk("2:%s,tmp_padding=%d,new:skb->len=%d\n",__FUNCTION__,tmp_padding,skb->len);
			
			for(tmp_cnt = tmp_len ;tmp_cnt < PADDING_LEN;tmp_cnt++)
			{
				skb->data[tmp_cnt] = '7';
			}
			
		}
		
	}
#endif
	/*************** End by zhangbin *********************/

	read_lock(&devs_lock);
	brvcc = pick_outgoing_vcc(skb, brdev);
	if (brvcc == NULL) {
		DPRINTK("no vcc attached to dev %s\n", dev->name);
		brdev->stats.tx_errors++;
		brdev->stats.tx_carrier_errors++;
		/* netif_stop_queue(dev); */
		dev_kfree_skb(skb);
		read_unlock(&devs_lock);
		return -EUNATCH;
	}
	if (!br2684_xmit_vcc(skb, brdev, brvcc)) {
		/*
		 * We should probably use netif_*_queue() here, but that
		 * involves added complication.  We need to walk before
		 * we can run
		 */
		/* don't free here! this pointer might be no longer valid!
		dev_kfree_skb(skb);
		*/
		brdev->stats.tx_errors++;
		brdev->stats.tx_fifo_errors++;
	}
	read_unlock(&devs_lock);
	return 0;
}

static struct net_device_stats *br2684_get_stats(struct net_device *dev)
{
	DPRINTK("br2684_get_stats\n");
	return &BRPRIV(dev)->stats;
}

#ifdef FASTER_VERSION
/*
 * These mirror eth_header and eth_header_cache.  They are not usually
 * exported for use in modules, so we grab them from net_device
 * after ether_setup() is done with it.  Bit of a hack.
 */
static int (*my_eth_header)(struct sk_buff *, struct net_device *,
	unsigned short, void *, void *, unsigned);
static int (*my_eth_header_cache)(struct neighbour *, struct hh_cache *);

static int
br2684_header(struct sk_buff *skb, struct net_device *dev,
	      unsigned short type, void *daddr, void *saddr, unsigned len)
{
	u16 *pad_before_eth;
	int t = my_eth_header(skb, dev, type, daddr, saddr, len);
	if (t > 0) {
		pad_before_eth = (u16 *) skb_push(skb, 2);
		*pad_before_eth = 0;
		return dev->hard_header_len;	/* or return 16; ? */
	} else
		return t;
}

static int
br2684_header_cache(struct neighbour *neigh, struct hh_cache *hh)
{
/* hh_data is 16 bytes long. if encaps is ether-llc we need 24, so
xmit will add the additional header part in that case */
	u16 *pad_before_eth = (u16 *)(hh->hh_data);
	int t = my_eth_header_cache(neigh, hh);
	DPRINTK("br2684_header_cache, neigh=%p, hh_cache=%p\n", neigh, hh);
	if (t < 0)
		return t;
	else {
		*pad_before_eth = 0;
		hh->hh_len = PADLEN + ETH_HLEN;
	}
	return 0;
}

/*
 * This is similar to eth_type_trans, which cannot be used because of
 * our dev->hard_header_len
 */
static inline unsigned short br_type_trans(struct sk_buff *skb,
					       struct net_device *dev)
{
	struct ethhdr *eth;
	unsigned char *rawp;
	eth = skb->mac.ethernet;

	if (*eth->h_dest & 1) {
		if (memcmp(eth->h_dest, dev->broadcast, ETH_ALEN) == 0)
			skb->pkt_type = PACKET_BROADCAST;
		else
			skb->pkt_type = PACKET_MULTICAST;
	}

	else if (memcmp(eth->h_dest, dev->dev_addr, ETH_ALEN))
		skb->pkt_type = PACKET_OTHERHOST;

	if (ntohs(eth->h_proto) >= 1536)
		return eth->h_proto;

	rawp = skb->data;

	/*
	 * This is a magic hack to spot IPX packets. Older Novell breaks
	 * the protocol design and runs IPX over 802.3 without an 802.2 LLC
	 * layer. We look for FFFF which isn't a used 802.2 SSAP/DSAP. This
	 * won't work for fault tolerant netware but does for the rest.
	 */
	if (*(unsigned short *) rawp == 0xFFFF)
		return htons(ETH_P_802_3);

	/*
	 * Real 802.2 LLC
	 */
	return htons(ETH_P_802_2);
}
#endif /* FASTER_VERSION */

/*
 * We remember when the MAC gets set, so we don't override it later with
 * the ESI of the ATM card of the first VC
 */
static int (*my_eth_mac_addr)(struct net_device *, void *);
static int br2684_mac_addr(struct net_device *dev, void *p)
{
	int err = my_eth_mac_addr(dev, p);
	if (!err)
		BRPRIV(dev)->mac_was_set = 1;
	return err;
}

#ifdef CONFIG_ATM_BR2684_IPFILTER
/* this IOCTL is experimental. */
static int br2684_setfilt(struct atm_vcc *atmvcc, unsigned long arg)
{
	struct br2684_vcc *brvcc;
	struct br2684_filter_set fs;

	if (copy_from_user(&fs, (void *) arg, sizeof fs))
		return -EFAULT;
	if (fs.ifspec.method != BR2684_FIND_BYNOTHING) {
		/*
		 * This is really a per-vcc thing, but we can also search
		 * by device
		 */
		struct br2684_dev *brdev;
		read_lock(&devs_lock);
		brdev = br2684_find_dev(&fs.ifspec);
		if (brdev == NULL || list_empty(&brdev->brvccs) ||
		    brdev->brvccs.next != brdev->brvccs.prev)  /* >1 VCC */
			brvcc = NULL;
		else
			brvcc = list_entry_brvcc(brdev->brvccs.next);
		read_unlock(&devs_lock);
		if (brvcc == NULL)
			return -ESRCH;
	} else
		brvcc = BR2684_VCC(atmvcc);
	memcpy(&brvcc->filter, &fs.filter, sizeof(brvcc->filter));
	return 0;
}

/* Returns 1 if packet should be dropped */
static inline int
packet_fails_filter(u16 type, struct br2684_vcc *brvcc, struct sk_buff *skb)
{
	if (brvcc->filter.netmask == 0)
		return 0;			/* no filter in place */
	if (type == __constant_htons(ETH_P_IP) &&
	    (((struct iphdr *) (skb->data))->daddr & brvcc->filter.
	     netmask) == brvcc->filter.prefix)
		return 0;
	if (type == __constant_htons(ETH_P_ARP))
		return 0;
	/* TODO: we should probably filter ARPs too.. don't want to have
	 *   them returning values that don't make sense, or is that ok?
	 */
	return 1;		/* drop */
}
#endif /* CONFIG_ATM_BR2684_IPFILTER */

static void br2684_close_vcc(struct br2684_vcc *brvcc)
{
	DPRINTK("removing VCC %p from dev %p\n", brvcc, brvcc->brdev);
	write_lock_irq(&devs_lock);
	list_del(&brvcc->brvccs);
	write_unlock_irq(&devs_lock);
	brvcc->atmvcc->user_back = NULL;	/* what about vcc->recvq ??? */
	brvcc->old_push(brvcc->atmvcc, NULL);	/* pass on the bad news */
	kfree(brvcc);
	MOD_DEC_USE_COUNT;
}


#ifdef TI_SLOW_PATH
/* when AAL5 PDU comes in: */
static void br2684_push(struct atm_vcc *atmvcc, struct sk_buff *skb)
{
	struct br2684_vcc *brvcc = BR2684_VCC(atmvcc);
	struct br2684_dev *brdev = brvcc->brdev;
	int plen = sizeof(llc_oui_pid_pad) + ETH_HLEN;
	struct ether_header	*pEther;

	DPRINTK("br2684_push\n");

	if (skb == NULL) {	/* skb==NULL means VCC is being destroyed */
		br2684_close_vcc(brvcc);
               	if (list_empty(&brdev->brvccs)) {
                       read_lock(&devs_lock);
                       list_del(&brdev->br2684_devs);
                       read_unlock(&devs_lock);
                       unregister_netdev(&brdev->net_dev);
                       kfree(brdev);
               	}
		return;
	}

	skb_debug(skb);
	atm_return(atmvcc, skb->truesize);
	DPRINTK("skb from brdev %p\n", brdev);
	
	switch(brvcc->encaps) {
		case BR2684_ENCAPS_LLC: {
		/*	if (brvcc->encaps == e_llc) { */
			/* let us waste some time for checking the encapsulation.
			   Note, that only 7 char is checked so frames with a valid FCS
			   are also accepted (but FCS is not checked of course) */
			if (memcmp(skb->data, llc_oui_pid_pad, 7)) {
				brdev->stats.rx_errors++;
				dev_kfree_skb(skb);
				return;
			}
			break;
		}
		case BR2684_ENCAPS_VC: {
			plen = PADLEN + ETH_HLEN;	/* pad, dstmac,srcmac, ethtype */
			/* first 2 chars should be 0 */
			if (*((u16 *) (skb->data)) != 0) {
				brdev->stats.rx_errors++;
				dev_kfree_skb(skb);
				return;
			}
			break;
		}
		case BR2684_ENCAPS_LLC_ROUTED: {
			plen = sizeof(llc_oui_ethertype);	/*2684 routed LLC has its own header, but no eth_hdr*/
			if (memcmp(skb->data, llc_oui_ethertype, 8)) {
				brdev->stats.rx_errors++;
				dev_kfree_skb(skb);
				return;
			}
			break;
		}
		case BR2684_ENCAPS_VC_ROUTED: {
			plen = 0;			/* 2684 routed VC doesn't have its own header or an eth_hdr*/
			/*?? no header, what to check? ...assume we're okay?*/
			break;
		}
	}
	if (skb->len < plen) {
		brdev->stats.rx_errors++;
		dev_kfree_skb(skb);	/* dev_ not needed? */
		return;
	}

#ifdef FASTER_VERSION
	/* FIXME: tcpdump shows that pointer to mac header is 2 bytes earlier,
	   than should be. What else should I set? */
	skb_pull(skb, plen);
	skb->mac.raw = ((char *) (skb->data)) - ETH_HLEN;
	skb->pkt_type = PACKET_HOST;
#ifdef CONFIG_BR2684_FAST_TRANS
	skb->protocol = ((u16 *) skb->data)[-1];
#else				/* some protocols might require this: */
	skb->protocol = br_type_trans(skb, &brdev->net_dev);
#endif /* CONFIG_BR2684_FAST_TRANS */
#else
	if((brvcc->encaps == BR2684_ENCAPS_LLC_ROUTED) || (brvcc->encaps == BR2684_ENCAPS_VC_ROUTED))
	{
		/****
		 *  remove br2684 encaps & add ETH header for routed mode
		 ****/
		skb_pull(skb, plen);
		if (skb_headroom(skb) < ETH_HLEN) {
			struct sk_buff *skb2 = skb_realloc_headroom(skb, ETH_HLEN);
			brvcc->copies_needed++;
			dev_kfree_skb(skb);
			if (skb2 == NULL) {
				brvcc->copies_failed++;
				return;
			}
			skb = skb2;
		}
		skb_push(skb, ETH_HLEN);		/* make room to add in the Eth header */
		pEther = (struct ether_header *) skb->data;
		memcpy(pEther->ether_dhost, brdev->net_dev.dev_addr, 6);
		memcpy(pEther->ether_shost, rfc2684_routed_mac, 6);	/* Set to "fake" source MAC address */
		pEther->ether_type = swap16(ETHERTYPE_IP);		/* hard coded to IP type - may be bad!!*/
	}	
	else
		skb_pull(skb, plen - ETH_HLEN);

    /* NSP Optimzation -- Call the translation only if the NAS interface is NOT bridged. */
    /*if ((&brdev->net_dev)->br_port == NULL)*/
	skb->protocol = eth_type_trans(skb, &brdev->net_dev);

#endif /* FASTER_VERSION */
#ifdef CONFIG_ATM_BR2684_IPFILTER
	if (packet_fails_filter(skb->protocol, brvcc, skb)) {
		brdev->stats.rx_dropped++;
		dev_kfree_skb(skb);
		return;
	}
#endif /* CONFIG_ATM_BR2684_IPFILTER */
	skb->dev = &brdev->net_dev;
	ATM_SKB(skb)->vcc = atmvcc;	/* needed ? */
	DPRINTK("received packet's protocol: %x\n", ntohs(skb->protocol));
	skb_debug(skb);
	if (!(brdev->net_dev.flags & IFF_UP)) { /* sigh, interface is down */
		brdev->stats.rx_dropped++;
		dev_kfree_skb(skb);
		return;
	}
	brdev->stats.rx_packets++;
	brdev->stats.rx_bytes += skb->len;
#ifdef BR2684_DEBUG
	printk("\n");
	xdump(skb->data, skb->len,"2684RX->IP: ");
	printk("\n");
#endif

    /* Added for WAN Bridge support.  */ 
    if (brdev->ptr_bridge == NULL)
    {
        /* In normal case pass the packet directly to the NET layer. */
	    netif_rx(skb);
    }
    else
    {
        /* In WAN Bridge mode pass the packet to th bridge. */
        wb_receive_frame (skb, brdev->ptr_bridge);
    }
}
#else

/**************************************************************************
 * FUNCTION NAME : br2684_receive_error
 **************************************************************************
 * DESCRIPTION   :
 *  This function is called from the push routine if there is an error in
 *  the received 2684 header.
 *
 * NOTES         :
 *  The function cleans the packet memory.
 ***************************************************************************/
static void br2684_receive_error (struct br2684_dev* brdev, struct sk_buff *skb)
{
    /* Increment stats. */
    brdev->stats.rx_errors++;

    /* Clean the packet memory. */
    kfree_skb (skb);
    return;
}

/**************************************************************************
 * FUNCTION NAME : br2684_close_connection
 **************************************************************************
 * DESCRIPTION   :
 *  This function is called when the 'push' routine receives a NULL skb 
 *  which indicates that the connection is to be closed.
 ***************************************************************************/
static void br2684_close_connection (struct br2684_vcc* brvcc, struct br2684_dev* brdev)
{
    /* As the orignal comment said - skb NULL means close the connection. */
	br2684_close_vcc(brvcc);
    if (list_empty(&brdev->brvccs)) 
    {
        read_lock(&devs_lock);
        list_del(&brdev->br2684_devs);
        read_unlock(&devs_lock);
        unregister_netdev(&brdev->net_dev);
        kfree(brdev);
    }
    return;
}

/**************************************************************************
 * FUNCTION NAME : br2684_push
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the registered call back routine called from 2684. The
 *  flow comes here for all received packets on either Static Routed, Bridged
 *  and PPPoE connections.
 *
 * NOTES         :
 *  The function assumes that the skb->data is 4 byte aligned. This is a fair
 *  assumption to make as the DSL/SAR driver should always allocate on the 4
 *  byte boundary.
 ***************************************************************************/
static void br2684_push(struct atm_vcc *atmvcc, struct sk_buff *skb)
{
	struct br2684_vcc*      brvcc = BR2684_VCC(atmvcc);
	struct br2684_dev*      brdev = brvcc->brdev;
	int                     plen;
    int                     bridged_encapsulation = 0;
    int                     header_error = 0;

#ifdef TI_BR2684_MEASUREMENTS
    rdtscl(br2684_push_start);
#endif
    
    /* Process the packet */
	if (skb != NULL) 
    {
        /* PANKAJ TODO: This decrements the rx use of the VCC. Do we actually use the
         * number of bytes used by the Rx. Currently the driver does 'atm_charge' we can
         * remove this later on. But if the driver changes then I need to fix PPPoA also
         * as it might also do the same. */
    	atm_return(atmvcc, skb->truesize);

        /* Processing is based on the encapsulation. */
        switch (brvcc->encaps)
        {
            case BR2684_ENCAPS_LLC:
            {

                /* Bridged LLC Encapsulation. Validate it. Removed the memcmp which did a 
                 * 7 byte comparison to a faster comparison. For portabiltity this should
                 * have been done with a ntohl and ntohs macros. */
                if ( (*((unsigned int *)(skb->data))     != 0x0003AAAA) ||
                     (*((unsigned short *)(skb->data+4)) != 0xC280) )
                {
                    /* Error: Header is not correct. */
                    header_error = 1;
                }
                else
                {
                    /* Set the protocol header length */
                    plen = sizeof(llc_oui_pid_pad);
                    bridged_encapsulation = 1;
                }
                break;
            }
            case BR2684_ENCAPS_VC:
            {

                /* Bridged VC Encapsulation. Validate it. */
			    if (*((u16 *) (skb->data)) != 0) 
                {
                    /* Error: Header is not correct. */
                    header_error = 1;
    			}
                else
                {
                    /* Set the protocol header length */
                    plen = PADLEN;
                    bridged_encapsulation = 1;
                }
                break;
            }
            case BR2684_ENCAPS_LLC_ROUTED:
            {

                /* Routed LLC Encapsulation. Validate it. Removed the memcmp which did an 
                 * 8 byte comparison to a faster comparison. For portabiltity this should
                 * have been done with a ntohl and ntohs macros. */
                if ( (*((unsigned int *)(skb->data))   != 0x0003AAAA) ||
                     (*((unsigned int *)(skb->data+4)) != 0x00080000) )
                {
                    /* Error: Header is not correct. */
                    header_error = 1;
                }
                else
                {
                    /* Set the protocol header length */
                    plen = sizeof(llc_oui_ethertype);
                }
                break;
            }
            case BR2684_ENCAPS_VC_ROUTED:
            {

                /* Routed VC Encapsulation. Set the protocol header length */
                plen = 0;
                break;
            }
        }

        /* By the time the control comes here. The variable 'plen' contains the length
         * of the protocol header. Pulling 'plen' from the skb shall now allow it to point 
         * to the start of the actual data. Actual data is different for bridged/routed 
         * encapsulations. */
    	if ((skb_pull(skb, plen) != NULL) && (header_error == 0))
        {
            /* Determine type of encapsulation. */
            if (bridged_encapsulation == 1)
            {
                /* Bridged Encapsulation. Start of data --> Ethernet header */

                /* Translate the packet only if the NAS interface is NOT bridged i.e. ROUTED. */
                if ((&brdev->net_dev)->br_port == NULL)
                {
                    /* Translate the packet only if the NAS interface is NOT WAN Bridged. */
                    if (brdev->ptr_bridge == NULL)
                        skb->protocol = br2684_type_trans(skb, &brdev->net_dev);
                }
#if 0
                /* PANKAJ TODO: Call the translation only if the NAS interface is ROUTED and
                 * it does NOT reside below the WAN bridge. */
                if ((&brdev->net_dev)->br_port == NULL || (brdev->ptr_bridge != NULL))
    	            skb->protocol = br2684_type_trans(skb, &brdev->net_dev);
#endif
            }
            else
            {
                /* Routed Encapsulation. Start of data --> IP Packet. */
        	    skb->mac.raw    = (char *)skb->data;
            	skb->pkt_type   = PACKET_HOST;
                skb->protocol   = swap16(ETHERTYPE_IP);		/* hard coded to IP type - may be bad!!*/
            }

            /* Fill up the device information on which the packet was received. */
            skb->dev = &brdev->net_dev;

            /* PANKAJ TODO: I dont think this is required. We dont bring the interface down. */
#if 0
        	ATM_SKB(skb)->vcc = atmvcc;	/* needed ? */
        	if (!(brdev->net_dev.flags & IFF_UP)) 
            { 
                /* sigh, interface is down */
	        	brdev->stats.rx_dropped++;
		        dev_kfree_skb(skb);
    		    return;
    	    }
#endif

            /* Increment the statistics. */
    	    brdev->stats.rx_packets++;
    	    brdev->stats.rx_bytes += skb->len;

#ifdef BR2684_DEBUG 
            printk("received packet's protocol: %x\n", ntohs(skb->protocol));
    	    printk("\n");
    	    xdump(skb->data, skb->len,"2684RX->IP: ");
        	printk("\n");
#endif

#ifdef TI_BR2684_MEASUREMENTS
            rdtscl(br2684_push_end);
#endif

            /* Added for WAN Bridge support.  */
            if (brdev->ptr_bridge == NULL)
            {
                /* In normal case pass the packet directly to the NET layer. */
                netif_rx(skb);
            }
            else
            {
                /* In WAN Bridge mode pass the packet to th bridge. */
                wb_receive_frame (skb, brdev->ptr_bridge);
            }
        }
        else
        {
             /* There was either a header error or the packet was simply too small. Either
              * case drop the packet and clean memory. */
             br2684_receive_error (brdev, skb);
             return;
        }
    }
    else
    {

        /* As the orignal comment said - skb NULL means close the connection. */
       if (keep_alive == 1)
          return;
       br2684_close_connection (brvcc, brdev);
    }
    return;
}

/**************************************************************************
 * FUNCTION NAME : maccmp
 **************************************************************************
 * DESCRIPTION   :
 *  The function compares the two MAC Addresses.
 *
 * RETURNS       :
 *  1   - If the MAC Address do not match.
 *  0   - If the MAC Address match.
 ***************************************************************************/
static __inline__ int maccmp(unsigned char* addr1, unsigned char* addr2)
{
    if ( (*((unsigned short *)addr1) == *((unsigned short *)addr2))     &&
         (*((unsigned short *)(addr1+2)) == *((unsigned short *)(addr2+2))) &&
         (*((unsigned short *)(addr1+4)) == *((unsigned short *)(addr2+4))) )
    {
        return 0;
    }
    return 1;
}

/**************************************************************************
 * FUNCTION NAME : br2684_type_trans
 **************************************************************************
 * DESCRIPTION   :
 *  The function does the translation of the ethernet header into the
 *  appropriate flags in the socket buffer.
 *
 * NOTES         :
 *  This function has been optimized, but the magical hacks to spot 
 *  802.2 packets has been removed.
 ***************************************************************************/
static unsigned short br2684_type_trans(struct sk_buff *skb, struct net_device *dev)
{
	struct ethhdr *eth;

    /* Raw Ethernet Data points to the start of the ethernet header. */	
	skb->mac.raw = skb->data;

    /* Remove the ethernet header. */
	skb_pull(skb,dev->hard_header_len);

    /* Get a pointer to the Ethernet header. */
	eth = skb->mac.ethernet;

    /* Check if the packet is a broadcast packet or not ? */
	if((*eth->h_dest & 1) == 0)
	{
        /* Fast Path: Unicast packets. Do the check first !*/
		if(maccmp(eth->h_dest,dev->dev_addr))
			skb->pkt_type=PACKET_OTHERHOST;
	}
	else
	{
        /* Packet could be BROADCAST or MULTICAST. Set the flag appropriately. */
		if(maccmp(eth->h_dest,dev->broadcast)==0)
			skb->pkt_type=PACKET_BROADCAST;
		else
			skb->pkt_type=PACKET_MULTICAST;
	}

    /* PANKAJ TODO: See the comments in the eth_type_trans. Do we run code to
     * spot IPX packets? Since we don't have a registered protocol handler for 
     * IPX this should not be a problem. In Bridged mode IPX frames still flow 
     * through as translations of the ethernet header are not required. */

    /* Return the protocol field. */
    return eth->h_proto;
}

#endif /* TI_SLOW_PATH */

static int br2684_regvcc(struct atm_vcc *atmvcc, unsigned long arg)
{
/* assign a vcc to a dev
Note: we do not have explicit unassign, but look at _push()
*/
	int err;
	struct br2684_vcc *brvcc;
	struct sk_buff_head copy;
	struct sk_buff *skb;
	struct br2684_dev *brdev;
	struct atm_backend_br2684 be;

	MOD_INC_USE_COUNT;
	if (copy_from_user(&be, (void *) arg, sizeof be)) {
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}
	write_lock_irq(&devs_lock);
	brdev = br2684_find_dev(&be.ifspec);
	if (brdev == NULL) {
		printk(KERN_ERR
		    "br2684: tried to attach to non-existant device\n");
		err = -ENXIO;
		goto error;
	}
	if (atmvcc->push == NULL) {
		err = -EBADFD;
		goto error;
	}
	if (!list_empty(&brdev->brvccs)) {	/* Only 1 VCC/dev right now */
		err = -EEXIST;
		goto error;
	}
	if (be.fcs_in != BR2684_FCSIN_NO || be.fcs_out != BR2684_FCSOUT_NO ||
	    be.fcs_auto || be.has_vpiid || be.send_padding || (be.encaps !=
	    BR2684_ENCAPS_VC && be.encaps != BR2684_ENCAPS_LLC && be.encaps != 
	    BR2684_ENCAPS_VC_ROUTED && be.encaps != BR2684_ENCAPS_LLC_ROUTED) ||
	    be.min_size != 0) {
		err = -EINVAL;
		goto error;
	}
	brvcc = kmalloc(sizeof(struct br2684_vcc), GFP_KERNEL);
	if (!brvcc) {
		err = -ENOMEM;
		goto error;
	}
	memset(brvcc, 0, sizeof(struct br2684_vcc));
	DPRINTK("br2684_regvcc vcc=%p, encaps=%d, brvcc=%p\n", atmvcc, be.encaps,
		brvcc);
	if (list_empty(&brdev->brvccs) && !brdev->mac_was_set) {
		unsigned char *esi = atmvcc->dev->esi;
		if (esi[0] | esi[1] | esi[2] | esi[3] | esi[4] | esi[5])
			memcpy(brdev->net_dev.dev_addr, esi,
			    brdev->net_dev.addr_len);
		else
			brdev->net_dev.dev_addr[2] = 1;
	}
	list_add(&brvcc->brvccs, &brdev->brvccs);
	write_unlock_irq(&devs_lock);
	brvcc->brdev = brdev;
	brvcc->atmvcc = atmvcc;
	atmvcc->user_back = brvcc;
	brvcc->encaps = (enum br2684_encaps) be.encaps;
	brvcc->old_push = atmvcc->push;
	barrier();
	atmvcc->push = br2684_push;
	skb_queue_head_init(&copy);
	skb_migrate(&atmvcc->recvq, &copy);
	while ((skb = skb_dequeue(&copy))) {
		/* marian: kernel panic (soft) on reboot 
		 * when queue does have pkts, e.g.
		 * smartbits pumps data and reboot
		 *
		 * BRPRIV(skb->dev)->stats.rx_bytes -= skb->len;
		 * BRPRIV(skb->dev)->stats.rx_packets--;
		 */
		br2684_push(atmvcc, skb);
	}
	return 0;
    error:
	write_unlock_irq(&devs_lock);
	MOD_DEC_USE_COUNT;
	return err;
}

/* Added this for WAN Bridge support. These IOTCL's will be called by the module to 
 * add/ remove the interface from the WAN Bridge. */
static int br2684_dev_do_ioctl (struct net_device *dev, struct ifreq *ifr, int cmd)
{
    struct br2684_dev* ptr_br2684 = (struct br2684_dev *)dev;
    switch (cmd)
    {
        case TIWAN_ADD_WAN_BRIDGE_IOCTL:
        {
            /* Ethernet has been added as the source interface below the WAN bridge. */
            ptr_br2684->ptr_bridge = (void *)ifr->ifr_ifru.ifru_data;
            break;
        }
        case TIWAN_DEL_WAN_BRIDGE_IOCTL:
        {
            /* Ethernet has been removed as the source interface from below the WAN bridge. */
            ptr_br2684->ptr_bridge = NULL;
            break;
        }
        default:
        {
            return -1;
        }
    }
    return 0;
}

static int br2684_create(unsigned long arg)
{
	int err;
	struct br2684_dev *brdev;
	struct atm_newif_br2684 ni;

	DPRINTK("br2684_create\n");
	/*
	 * We track module use by vcc's NOT the devices they're on.  We're
	 * protected here against module death by the kernel_lock, but if
	 * we need to sleep we should make sure that the module doesn't
	 * disappear under us.
	 */
	MOD_INC_USE_COUNT;
	if (copy_from_user(&ni, (void *) arg, sizeof ni)) {
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}
	if (ni.media != BR2684_MEDIA_ETHERNET || ni.mtu != 1500) {
		MOD_DEC_USE_COUNT;
		return -EINVAL;
	}
	if ((brdev = kmalloc(sizeof(struct br2684_dev), GFP_KERNEL)) == NULL) {
		MOD_DEC_USE_COUNT;
		return -ENOMEM;
	}
	memset(brdev, 0, sizeof(struct br2684_dev));
	INIT_LIST_HEAD(&brdev->brvccs);

	write_lock_irq(&devs_lock);
	brdev->number = list_empty(&br2684_devs) ? 1 :
	    list_entry_brdev(br2684_devs.prev)->number + 1;
	list_add_tail(&brdev->br2684_devs, &br2684_devs);
	write_unlock_irq(&devs_lock);

	if (ni.ifname[0] != '\0') {
		memcpy(brdev->net_dev.name, ni.ifname,
		    sizeof(brdev->net_dev.name));
		brdev->net_dev.name[sizeof(brdev->net_dev.name) - 1] = '\0';
	} else
		sprintf(brdev->net_dev.name, "nas%d", brdev->number);
	DPRINTK("registered netdev %s\n", brdev->net_dev.name);

	//cjg:2004-5-11;add for pppoe-passthrough
	memset(wan_dev_name, 0, sizeof(wan_dev_name));
	strncpy(wan_dev_name, brdev->net_dev.name, 4);
	wan_dev_name[4] = '\0';

	ether_setup(&brdev->net_dev);
	brdev->mac_was_set = 0;
#ifdef FASTER_VERSION
	my_eth_header = brdev->net_dev.hard_header;
	brdev->net_dev.hard_header = br2684_header;
	my_eth_header_cache = brdev->net_dev.hard_header_cache;
	brdev->net_dev.hard_header_cache = br2684_header_cache;
	brdev->net_dev.hard_header_len = sizeof(llc_oui_pid_pad) + ETH_HLEN;	/* 10 + 14 */
#endif
	my_eth_mac_addr = brdev->net_dev.set_mac_address;
	brdev->net_dev.set_mac_address = br2684_mac_addr;
	brdev->net_dev.hard_start_xmit = br2684_start_xmit;
	brdev->net_dev.get_stats = br2684_get_stats;

    /* Added IOCTL support for the WAN Bridge. */
    brdev->net_dev.do_ioctl = &br2684_dev_do_ioctl;

	/* open, stop, do_ioctl ? */
	err = register_netdev(&brdev->net_dev);
	MOD_DEC_USE_COUNT;
	if (err < 0) {
		printk(KERN_ERR "br2684_create: register_netdev failed\n");
		write_lock_irq(&devs_lock);
		list_del(&brdev->br2684_devs);
		write_unlock_irq(&devs_lock);
		kfree(brdev);
		return err;
	}
	return 0;
}

/*
 * This handles ioctls actually performed on our vcc - we must return
 * -ENOIOCTLCMD for any unrecognized ioctl
 */
static int br2684_ioctl(struct atm_vcc *atmvcc, unsigned int cmd,
	unsigned long arg)
{
	int err;
	switch(cmd) {
       case ATM_KEEPALIVE:
                keep_alive=1;
                return 0;
	case ATM_SETBACKEND:
	case ATM_NEWBACKENDIF: {
		atm_backend_t b;
		MOD_INC_USE_COUNT;
		err = get_user(b, (atm_backend_t *) arg);
		MOD_DEC_USE_COUNT;
		if (err)
			return -EFAULT;
		if (b != ATM_BACKEND_BR2684)
			return -ENOIOCTLCMD;
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (cmd == ATM_SETBACKEND)
			return br2684_regvcc(atmvcc, arg);
		else
			return br2684_create(arg);
		}
#ifdef CONFIG_ATM_BR2684_IPFILTER
	case BR2684_SETFILT:
		if (atmvcc->push != br2684_push)
			return -ENOIOCTLCMD;
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		MOD_INC_USE_COUNT;
		err = br2684_setfilt(atmvcc, arg);
		MOD_DEC_USE_COUNT;
		return err;
#endif /* CONFIG_ATM_BR2684_IPFILTER */
	}
	return -ENOIOCTLCMD;
}

/* Never put more than 256 bytes in at once */
static int br2684_proc_engine(loff_t pos, char *buf)
{
	struct list_head *lhd, *lhc;
	struct br2684_dev *brdev;
	struct br2684_vcc *brvcc;
	
	list_for_each(lhd, &br2684_devs) {
		brdev = list_entry_brdev(lhd);
		if (pos-- == 0)
			return sprintf(buf, "dev %.16s: num=%d, mac=%02X:%02X:"
			    "%02X:%02X:%02X:%02X (%s)\n", brdev->net_dev.name,
			    brdev->number,
			    brdev->net_dev.dev_addr[0],
			    brdev->net_dev.dev_addr[1],
			    brdev->net_dev.dev_addr[2],
			    brdev->net_dev.dev_addr[3],
			    brdev->net_dev.dev_addr[4],
			    brdev->net_dev.dev_addr[5],
			    brdev->mac_was_set ? "set" : "auto");
		list_for_each(lhc, &brdev->brvccs) {
			brvcc = list_entry_brvcc(lhc);
			if (pos-- == 0)
				return sprintf(buf, "  vcc %d.%d.%d: encaps=%d"
#ifndef FASTER_VERSION
				    ", failed copies %u/%u"
#endif /* FASTER_VERSION */
				    "\n", brvcc->atmvcc->dev->number,
				    brvcc->atmvcc->vpi, brvcc->atmvcc->vci,
				    brvcc->encaps
#ifndef FASTER_VERSION
				    , brvcc->copies_failed
				    , brvcc->copies_needed
#endif /* FASTER_VERSION */
				    );
#ifdef CONFIG_ATM_BR2684_IPFILTER
#define b1(var, byte)	((u8 *) &brvcc->filter.var)[byte]
#define bs(var)		b1(var, 0), b1(var, 1), b1(var, 2), b1(var, 3)
			if (brvcc->filter.netmask != 0 && pos-- == 0)
				return sprintf(buf, "    filter=%d.%d.%d.%d/"
				    "%d.%d.%d.%d\n", bs(prefix), bs(netmask));
#undef bs
#undef b1
#endif /* CONFIG_ATM_BR2684_IPFILTER */
		}
	}
	return 0;
}

static ssize_t br2684_proc_read(struct file *file, char *buf, size_t count,
	loff_t *pos)
{
	unsigned long page;
	int len = 0, x, left;
	page = get_free_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;
	left = PAGE_SIZE - 256;
	if (count < left)
		left = count;
	read_lock(&devs_lock);
	for (;;) {
		x = br2684_proc_engine(*pos, &((char *) page)[len]);
		if (x == 0)
			break;
		if (x > left)
			/*
			 * This should only happen if the user passed in
			 * a "count" too small for even one line
			 */
			x = -EINVAL;
		if (x < 0) {
			len = x;
			break;
		}
		len += x;
		left -= x;
		(*pos)++;
		if (left < 256)
			break;
	}
	read_unlock(&devs_lock);
	if (len > 0 && copy_to_user(buf, (char *) page, len))
		len = -EFAULT;
	free_page(page);
	return len;
}

static struct file_operations br2684_proc_operations = {
	read: br2684_proc_read,
};

extern struct proc_dir_entry *atm_proc_root;	/* from proc.c */

extern int (*br2684_ioctl_hook)(struct atm_vcc *, unsigned int, unsigned long);

/* the following avoids some spurious warnings from the compiler */
#define UNUSED __attribute__((unused))

static int __init UNUSED br2684_init(void)
{
	struct proc_dir_entry *p;
	if ((p = create_proc_entry("br2684", 0, atm_proc_root)) == NULL)
		return -ENOMEM;
	p->proc_fops = &br2684_proc_operations;
	br2684_ioctl_hook = br2684_ioctl;
	return 0;
}

static void __exit UNUSED br2684_exit(void)
{
	struct br2684_dev *brdev;

	br2684_ioctl_hook = NULL;
	remove_proc_entry("br2684", atm_proc_root);
	while (!list_empty(&br2684_devs)) {
		brdev = list_entry_brdev(br2684_devs.next);
		unregister_netdev(&brdev->net_dev);
		list_del(&brdev->br2684_devs);
		kfree(brdev);
	}
}

module_init(br2684_init);
module_exit(br2684_exit);

static int BuildAndSendArpResp(struct sk_buff * skb, struct br2684_dev *brdev, struct br2684_vcc *brvcc)
{
	struct ether_header	*pEther;
	struct ether_header	*pOrigEther;
	struct ether_arp	*pArpPacket;
	struct ether_arp	*pArpOrigPacket;
	struct sk_buff		*skb2;

	skb2 = skb_realloc_headroom(skb, (sizeof(struct ether_header)));
	brvcc->copies_needed++;
	if (skb2 == NULL) {
		brvcc->copies_failed++;
		return 0;
	}

	/* ...info for (ARP) is all in the skb->data area, skb->mac.ethernet gives kernelpanic! */
	pOrigEther = (struct ether_header *) skb->data;
	pEther = (struct ether_header *) skb2->data;

	/* move ether header information first */
	memcpy(pEther->ether_dhost, pOrigEther->ether_shost, 6);
	memcpy(pEther->ether_shost, rfc2684_routed_mac, 6);	/* Set to "fake" source MAC address */
	pEther->ether_type = swap16(ETHERTYPE_ARP);

	/* create fields for Ethernet ARP structure */
	pArpPacket = (struct ether_arp *) ((unsigned char *)pEther + sizeof(struct ether_header));
	pArpOrigPacket = (struct ether_arp *) ((unsigned char *)pOrigEther + sizeof(struct ether_header));

	pArpPacket->ea_hdr.ar_hrd = pArpOrigPacket->ea_hdr.ar_hrd;
	pArpPacket->ea_hdr.ar_pro = pArpOrigPacket->ea_hdr.ar_pro;
	pArpPacket->ea_hdr.ar_hln = pArpOrigPacket->ea_hdr.ar_hln;
	pArpPacket->ea_hdr.ar_pln = pArpOrigPacket->ea_hdr.ar_pln;
	pArpPacket->ea_hdr.ar_op = swap16(ARPOP_REPLY);

	memcpy(pArpPacket->arp_sha, rfc2684_routed_mac, 6);		/* Send is "fake" MAC */
	memcpy(pArpPacket->arp_spa, pArpOrigPacket->arp_tpa, 4);
	memcpy(pArpPacket->arp_tha, pArpOrigPacket->arp_sha, 6);
	memcpy(pArpPacket->arp_tpa, pArpOrigPacket->arp_spa, 4);

	dev_kfree_skb(skb);	/* Free 'original' skb which contained ARP request */
	skb = skb2;		/* skb now points to 'new' ARP response */
	/* need skb->protocol & skb->dev to be set? (should already have been set since we just copied skb?) */
	skb->protocol = eth_type_trans(skb, &brdev->net_dev);
	skb->dev = &brdev->net_dev;
#ifdef BR2684_DEBUG
	printk("\n");
	xdump(skb->data, skb->len,"2684_RARP: ");
	printk("\n");
#endif
	netif_rx(skb);		/* send 'ARP response' up the rx i/f */
	return 1;
}

/******************************************************************************
*	Function:	swap16	- Convert PKT u16 into PC u16 (or reverse)
*	Call:
*		swapped_16 = swap16(u16 orig_16);
*	Arguments:
*		orig_16			- u16 field to convert.
*	Return Value:
*		swapped_16		- converted u16 field.
*	Description: 
+*****************************************************************************/

__u16 swap16(__u16 orig_16)
{
	__u8 *tmp_8_ptr = (__u8 *)&orig_16;
	__u16 swapped_16;

	swapped_16 = ((__u16)*tmp_8_ptr++ << 8);
	swapped_16 |= *tmp_8_ptr;

	return(swapped_16);
}

#ifdef BR2684_DEBUG
/*************************************************************
 * Function: 	xdump - nice hex dump to screen using printk
 *************************************************************/
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
#endif /* BR2684_DEBUG */

MODULE_AUTHOR("Marcell GAL");
MODULE_DESCRIPTION("RFC2684 bridged protocols over ATM/AAL5");
MODULE_LICENSE("GPL");
