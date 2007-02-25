/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_forward.c,v 1.1.1.2 2005/03/28 06:57:03 sparq Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */
//-------------------------------------------------------------------------------------
// Copyright 2004, Texas Instruments Incorporated
//
// This program has been modified from its original operation by Texas Instruments
// to do the following:
// 
// Performance optimizations
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

/* The definition controls the orignal Linux Code and new optimized code.
 * Define this MACRO to switch back to the orignal Linux Code. */
#ifndef TI_SLOW_PATH

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/if_bridge.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include "br_private.h"

/* Define this MACRO only for debugging the amount of time each function takes. */
#undef TI_BRIDGE_FAST_PATH_MEASUREMENTS

/* NSP Optimizations: This macro controls measuring the number of ticks each function
 * in the bridge fast path takes. Only for debugging purposes. */
#ifdef TI_BRIDGE_FAST_PATH_MEASUREMENTS

/* Display the warning message so that the user knows that measurements are
 * being compiled into the code. */
#warning "*********************************************************************"
#warning "BRIDGE Fastpath Measurements are being compiled into the code"
#warning "*********************************************************************"

/* Measurement: Macro */
#define rdtscl(dest) \
	__asm__ __volatile__("mfc0 %0,$9; nop" : "=r" (dest))

extern int br_dev_xmit_start;
extern int br_dev_xmit_end;
extern int br_send_frame_start;
extern int br_send_frame_end;

#endif /* TI_BRIDGE_FAST_PATH_MEASUREMENTS */

/*************************** Static Declarations ***********************/
static void br_send_frame_error (struct sk_buff *skb);

/*************************** Static Declarations ***********************/
extern void br_flood(struct net_bridge *br, struct sk_buff *skb, int clone);
extern void br_send_frame(struct net_bridge_port *to, struct sk_buff *skb);

/*************************** Functions *********************************/

__inline__ unsigned long __timeout(struct net_bridge *br)
{
	unsigned long timeout;

	timeout = jiffies - br->ageing_time;
	if (br->topology_change)
		timeout = jiffies - br->forward_delay;

	return timeout;
}

__inline__ int has_expired(struct net_bridge *br, struct net_bridge_fdb_entry *fdb)
{
	if (!fdb->is_static && time_before_eq(fdb->ageing_timer, __timeout(br)))
		return 1;

	return 0;
}

/**************************************************************************
 * FUNCTION NAME : br_mac_hash
 **************************************************************************
 * DESCRIPTION   :
 *  The function calculates the hash key on the basis of the MAC Address.
 * 
 * RETURNS       :
 *  Computed hash key. 
 ***************************************************************************/
__inline__ int br_mac_hash(unsigned char *mac)
{
#if 0
	unsigned long x;

	x = mac[0];
	x = (x << 2) ^ mac[1];
	x = (x << 2) ^ mac[2];
	x = (x << 2) ^ mac[3];
	x = (x << 2) ^ mac[4];
	x = (x << 2) ^ mac[5];

	x ^= x >> 8;

	return x & (BR_HASH_SIZE - 1);
#else
    unsigned long x;

    x = mac[3];
	x = (x << 2) ^ mac[4];
	x = (x << 2) ^ mac[5];
	x ^= x >> 8;
	return x & (BR_HASH_SIZE - 1);
#endif
}

/**************************************************************************
 * FUNCTION NAME : should_deliver
 **************************************************************************
 * DESCRIPTION   :
 *  The function checks if a packet can be delivered or not on an interface.
 *  Bridge rules state that the packet cannot be forwarded on the same port
 *  as on which it was received and the destination port should be in the
 *  correct forwarding state.
 * 
 * RETURNS       :
 *      0   - Do not forward / Deliver
 *      1   - Forward / Deliver 
 ***************************************************************************/
static inline int should_deliver(struct net_bridge_port *p, struct sk_buff *skb)
{

	if (skb->dev == p->dev || ((p->state & BR_STATE_FORWARDING) == 0))
		return 0;
	return 1;
}

/**************************************************************************
 * FUNCTION NAME : maccmp
 **************************************************************************
 * DESCRIPTION   :
 *  The function compares the mac addresses. This used to be a memcmp which
 *  used to compare the address in 6 iterations, using unsigned short 
 *  comparisons the work can now be done in 3 iterations.
 *
 * RETURNS       :
 *      0 - If MAC Addresses are the same
 *      1 - If they are not the same.
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
 * FUNCTION NAME : br_fdb_get
 **************************************************************************
 * DESCRIPTION   :
 *  The function searches the FDB database for a match on the MAC Address.
 *  If a match is found the FDB entry is marked as used.
 *
 * RETURNS       :
 *  FDB Entry   - Success.
 *  NULL        - Error.    
 ***************************************************************************/
struct net_bridge_fdb_entry *br_fdb_get(struct net_bridge *br, unsigned char *addr)
{
	struct net_bridge_fdb_entry *fdb;

    /* Lock the bridge. */
	read_lock_bh(&br->hash_lock);

    /* Calculate the bridge hash key using the mac address. */
    fdb = br->hash[br_mac_hash(addr)];

    /* Cycle through the FDB hash list. */
	while (fdb != NULL)
    {
        /* Compare the MAC Address. */
        if (!maccmp(fdb->addr.addr, addr))
        {
            /* YES. The MAC Address match. Check if can be used or not. */
			if (!has_expired(br, fdb)) 
            {
                /* YES. It can be used so increment the usage count. */
				atomic_inc(&fdb->use_count);
			}
            else
            {
                /* The FDB entry has expired. So we cannot use it any longer */
                fdb = NULL;
            }
            break;
		}

        /* Cycle to the next entry. */
    	fdb = fdb->next_hash;
	}

    /* Unlock the bridge. */
	read_unlock_bh(&br->hash_lock);
	return fdb;
}

/**************************************************************************
 * FUNCTION NAME : br_dev_xmit
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the registered transmit routine for the bridge management
 *  device. 
 ***************************************************************************/
int br_dev_xmit(struct sk_buff *skb, struct net_device *dev)
{
	register struct net_bridge*  br;
    unsigned char*               dest;
	struct net_bridge_fdb_entry* dst;

#ifdef TI_BRIDGE_FAST_PATH_MEASUREMENTS
    rdtscl(br_dev_xmit_start);
#endif

    /* Get the bridge handle. */
	br = dev->priv;

    /* Lock the bridge down. */
	read_lock(&br->lock);

    /* Increment the statistics. */
	br->statistics.tx_packets++;
	br->statistics.tx_bytes += skb->len;

    /* Initialize the SKB and pull the ethernet header. */
	dest = skb->mac.raw = skb->data;

#if 0
    /* Check if the packet is a broadcast / multicast packet. */
	if (dest[0] & 1)
    {
        /* YES. Flood the packet on all the interfaces. */
		br_flood(br, skb, 0);
        read_unlock(&br->lock);
		return 0;
	}
#endif

    /* Check the FDB table for a hit? Broadcast/ Multicast address are not
     * stored in the FDB so they will be flooded... Hence the above check is
     * redundant. */
	if ((dst = br_fdb_get(br, dest)) != NULL)
    {
        /* HIT. Send the frame on that interface. */
		br_send_frame(dst->dst, skb);
		br_fdb_put(dst);
	}
    else
    {
        /* No hit: Flood on all interfaces. */
	    br_flood(br, skb, 0);
    }

#ifdef TI_BRIDGE_FAST_PATH_MEASUREMENTS
    rdtscl(br_dev_xmit_end);
#endif

    /* Unlock the bridge. */
    read_unlock(&br->lock);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : br_fdb_put
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called once the work with the FDB entry is over and the
 *  entry is now marked as NOT used.
 ***************************************************************************/
void br_fdb_put(struct net_bridge_fdb_entry *ent)
{
	if (atomic_dec_and_test(&ent->use_count))
		kfree(ent);
}

/**************************************************************************
 * FUNCTION NAME : br_send_frame
 **************************************************************************
 * DESCRIPTION   :
 *  The function sends the frame on a specified bridge port. 
 ***************************************************************************/
void br_send_frame(struct net_bridge_port *to, struct sk_buff *skb)
{
    struct net_bridge *br;

#ifdef TI_BRIDGE_FAST_PATH_MEASUREMENTS
    rdtscl(br_send_frame_start);
#endif

    /* Check if the packet can be sent out or not ? */
 	if (should_deliver(to, skb)) 
    {
        /* Initialize the SKB fields. */
    	skb->dev = to->dev;

        /* Get the bridge information. */
        br = skb->dev->br_port->br;
 
        /* Check if bridge filtering is enabled. */
        if (br->br_filter_active) 
        {
            /* YES. Check with the filter if the packet can be sent out or not? */
            int result = br_filter_frame(skb);
            if(result)
            {
                /* Drop the frame. */
                br_send_frame_error (skb);
                return;
            }
        }

        /* PANKAJ TODO: Push the packet directly to the hardware drivers. Since we never 
         * run QOS on any of the interfaces which reside below the bridge. What do we do
         * if the interface is down? Do we need to add the check.*/

        skb->dev->hard_start_xmit(skb, skb->dev);

	}
    else
    {
        /* Error: Packet cannot be sent out on the interface. */
        br_send_frame_error (skb);
    }

#ifdef TI_BRIDGE_FAST_PATH_MEASUREMENTS
    rdtscl(br_send_frame_end);
#endif
    return;
}

/**************************************************************************
 * FUNCTION NAME : br_flood
 **************************************************************************
 * DESCRIPTION   :
 *  The function floods the packets on all interfaces attached to the bridge.
 ***************************************************************************/
void br_flood(struct net_bridge *br, struct sk_buff *skb, int clone)
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;

    /* Check if we need to clone the packets. This is required if the packet
     * has to be tossed up the IP stack also. */
	if (clone) 
    {
		struct sk_buff *skb2;
		if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) 
        {
            /* Error: Cloning the packet failed. */
			br->statistics.tx_dropped++;
            br_send_frame_error(skb);
			return;
		}
		skb = skb2;
	}

    /* Initialize the previous and current ports. */
	prev = NULL;
	p = br->port_list;

    /* Cycle through the list of bridge ports. */
	while (p != NULL) 
    {
        /* Verify if the packet can be sent out or not. */
		if (should_deliver(p, skb)) 
        {
            /* YES. Clone the packet only if necessary. We make 1 less clone of the packet, the
             * last interface gets the orignal packet for transmission. */
			if (prev != NULL) 
            {
				struct sk_buff *skb2;

                /* Clone the packet. */
				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) 
                {
                    /* Error: Cloning the packet failed. */
					br->statistics.tx_dropped++;
					br_send_frame_error(skb);
					return;
				}

                /* Send the cloned frame on the interface. */
				br_send_frame(prev, skb2);
			}
			prev = p;
		}
		p = p->next;
	}
    
    /* Check if there was a last interface, if yes send the orignal SKB on this interface. */
	if (prev != NULL) 
    {
		br_send_frame(prev, skb);
		return;
	}

    /* This case will be reached only if there were no ports attached to the bridge. Clean
     * up the memory. */
	br_send_frame_error(skb);
    return;
}

/**************************************************************************
 * FUNCTION NAME : br_send_frame_error
 **************************************************************************
 * DESCRIPTION   :
 *  The packet could not be sent out because of an error condition. Clean
 *  packet memory. This is slow path.
 ***************************************************************************/
static void br_send_frame_error (struct sk_buff *skb)
{
    /* Cannot send the frame. Clean memory. */
	kfree_skb(skb);
    return;
}

#else /* End of Performance optimizations */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/skbuff.h>
#include <linux/if_bridge.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

static inline int should_deliver(struct net_bridge_port *p, struct sk_buff *skb)
{
	if (skb->dev == p->dev ||
	    p->state != BR_STATE_FORWARDING)
		return 0;

	return 1;
}

static int __dev_queue_push_xmit(struct sk_buff *skb)
{
#if defined (CONFIG_MIPS_AVALANCHE_FAST_BRIDGE)
        struct net_device *dev = skb->dev;
#endif
 /*       struct net_bridge_port *p;
        struct net_bridge *br;
        int result = 0;
 
        p = skb->dev->br_port;
        br = p->br;
 
        if (br->br_filter_active) {
            result = br_filter_frame(skb);
 
            if(result)
                goto throw_out;
            else
                goto pushthru;
        }

pushthru:*/
	skb_push(skb, ETH_HLEN);

#if defined (CONFIG_MIPS_AVALANCHE_FAST_BRIDGE)
        if((dev->flags && IFF_UP) && 
           !( (netif_queue_stopped(dev)) || (skb_shinfo(skb)->frag_list) || (skb_shinfo(skb)->nr_frags) 
	   || (netdev_nit)))
	{
	    int cpu = smp_processor_id( );	

            if(dev->xmit_lock_owner == cpu)
		goto dev_queue_xmit_lb;

	    spin_lock(&dev->xmit_lock);
            dev->xmit_lock_owner = cpu;

            if(dev->hard_start_xmit(skb,dev))
            {
		dev->xmit_lock_owner = -1;
	        spin_unlock(&dev->xmit_lock);
		goto dev_queue_xmit_lb;
            }

	    dev->xmit_lock_owner = -1;
	    spin_unlock(&dev->xmit_lock);
	    return 0;
	}

dev_queue_xmit_lb:
#endif
	dev_queue_xmit(skb);
	return 0;

}

static int __br_forward_finish(struct sk_buff *skb)
{
	NF_HOOK(PF_BRIDGE, NF_BR_POST_ROUTING, skb, NULL, skb->dev,
			__dev_queue_push_xmit);

	return 0;
}

static void __br_deliver(struct net_bridge_port *to, struct sk_buff *skb)
{
	struct net_device *indev;

	indev = skb->dev;
	skb->dev = to->dev;

	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_OUT, skb, indev, skb->dev,
			__br_forward_finish);
}

static void __br_forward(struct net_bridge_port *to, struct sk_buff *skb)
{
	struct net_device *indev;

	indev = skb->dev;
	skb->dev = to->dev;

	NF_HOOK(PF_BRIDGE, NF_BR_FORWARD, skb, indev, skb->dev,
			__br_forward_finish);
}

/* called under bridge lock */
void br_deliver(struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_deliver(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called under bridge lock */
void br_forward(struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called under bridge lock */
static void br_flood(struct net_bridge *br, struct sk_buff *skb, int clone,
	void (*__packet_hook)(struct net_bridge_port *p, struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;

	if (clone) {
		struct sk_buff *skb2;

		if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
			br->statistics.tx_dropped++;
			return;
		}

		skb = skb2;
	}

	prev = NULL;

	p = br->port_list;
	while (p != NULL) {
		if (should_deliver(p, skb)) {
			if (prev != NULL) {
				struct sk_buff *skb2;

				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->statistics.tx_dropped++;
					kfree_skb(skb);
					return;
				}

				__packet_hook(prev, skb2);
			}

			prev = p;
		}

		p = p->next;
	}

	if (prev != NULL) {
		__packet_hook(prev, skb);
		return;
	}

	kfree_skb(skb);
}

/* called under bridge lock */
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb, int clone)
{
	br_flood(br, skb, clone, __br_deliver);
}

/* called under bridge lock */
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb, int clone)
{
	br_flood(br, skb, clone, __br_forward);
}

#endif

