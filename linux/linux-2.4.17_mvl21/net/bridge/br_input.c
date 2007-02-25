/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_input.c,v 1.1.1.2 2005/03/28 06:57:03 sparq Exp $
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
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#include <asm/uaccess.h>

unsigned char bridge_ula[6] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };

/*************************** EXTERN DECLARATIONS ***********************/
extern void br_flood(struct net_bridge *br, struct sk_buff *skb, int clone);
extern void br_send_frame(struct net_bridge_port *to, struct sk_buff *skb);

/*************************** STATIC DECLARATIONS ***********************/
static void br_input_error(struct sk_buff* skb);

/**************************************************************************
 * FUNCTION NAME : br_pass_frame_up
 **************************************************************************
 * DESCRIPTION   :
 *  The function passes the received frame up the IP Stack through the
 *  bridge interface.
***************************************************************************/
static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
    /* Increment the statistics. */
	br->statistics.rx_packets++;
	br->statistics.rx_bytes += skb->len;

    /* Initialize the device and push the frame up the stack. */
	skb->dev      = &br->dev;
	skb->pkt_type = PACKET_HOST;

    /* TODO: Do we need to do this? Simply setting the protocol field should 
     * sufficient? There is a memcmp in the function which needs to be avoided.
     * We have already made sure that the packet is for us. */
	skb->protocol = eth_type_trans(skb, &br->dev);

    /* Pass packet to the IP stack.*/
    netif_rx(skb);
    return;
}

/**************************************************************************
 * FUNCTION NAME : __br_handle_frame
 **************************************************************************
 * DESCRIPTION   :
 *  The function receives and does the bridge processing. The function
 *  is called with the bridge locked.  
***************************************************************************/
static void __br_handle_frame(struct sk_buff* skb, struct net_bridge* br)
{	
	register unsigned char*                 dest;
	register struct net_bridge_fdb_entry*   fdb_entry;
	register struct net_bridge_port*        source_port;

    /* Initialize the destination address and source port on which the packet arrived. */
	dest        = skb->data;
	source_port = skb->dev->br_port;

    /* Basic Validations. Do not process any packets whose mac address has the highest order
     * byte set. */
	if (skb->data[6] & 1)
    {
		br_input_error(skb);
        return;
    }

#if 0
    /* Spanning Tree Protocol. */
	if (br->stp_enabled && !memcmp(dest, bridge_ula, 5) && !(dest[5] & 0xF0))
    {
	    if (!dest[5]) 
        {
		    br_stp_handle_bpdu(skb);
    		return;
	    }
    }
#endif

    /* Learn the source mac address only if in the correct state. */
	if (source_port->state & (BR_STATE_LEARNING | BR_STATE_FORWARDING))
		br_fdb_insert(br, source_port, &skb->data[6], 0);

    /* Do not forward if we the port is not in the correct state. */
	if (source_port->state & BR_STATE_FORWARDING)
    {

        /* Check if the packet is a broadcast/multicast packet. */
	    if (dest[0] & 1) 
        {
            /* Flood the packet on all interfaces and pass the frame to the IP stack also. */

		    br_flood(br, skb, 1);
    	    br_pass_frame_up(br, skb);
	    	return;
    	}

        /* Check the FDB table for a match. */
	    fdb_entry = br_fdb_get(br, dest);
    	if (fdb_entry != NULL) 
        {

            if (fdb_entry->is_local == 1)
            {
                /* Hit. Local entry so pass the packet to the IP stack. */

	            br_pass_frame_up(br, skb);
            }
            else
            {
                /* Hit, but non local so push the packet on the appropriate interface. */

        		br_send_frame(fdb_entry->dst, skb);
            }

            br_fdb_put(fdb_entry);
    		return;
	    }

        /* No hit. Flood packet on all interfaces. Do not send the packet to the IP stack. */
	    br_flood(br, skb, 0);
    }
    else
    {
        /* Port was not in the forwarding state. So drop the packet and clean memory. */

		br_input_error(skb);
    }
    return;
}

/**************************************************************************
 * FUNCTION NAME : br_handle_frame
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the registered handler to which all packets shall be 
 *  pushed to. It locks the bridge and calls the actual bridge handler 
 *  routine.
***************************************************************************/
void br_handle_frame(struct sk_buff *skb)
{
	struct net_bridge *br;
    /* Get the bridge handle. */
	br = skb->dev->br_port->br;
	read_lock(&br->lock);

    /* Process the received frame. */
	__br_handle_frame(skb, br);

    /* Unlock the bridge. */
	read_unlock(&br->lock);
    return;
}

/**************************************************************************
 * FUNCTION NAME : br_input_error
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called on the bridge receive path when an error is 
 *  detected. It cleans up packet memory.
 ***************************************************************************/
static void br_input_error(struct sk_buff *skb)
{
    kfree_skb(skb);
    return;
}

#else /* End of Performance optimizations */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#include <asm/uaccess.h>

unsigned char bridge_ula[6] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };

static int br_pass_frame_up_finish(struct sk_buff *skb)
{
	netif_rx(skb);

	return 0;
}

static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
	struct net_device *indev;

	br->statistics.rx_packets++;
	br->statistics.rx_bytes += skb->len;

	indev = skb->dev;
	skb->dev = &br->dev;
	skb->pkt_type = PACKET_HOST;
	skb_push(skb, ETH_HLEN);
	skb->protocol = eth_type_trans(skb, &br->dev);

	
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
			br_pass_frame_up_finish);
}


static void __br_handle_frame_orig(struct sk_buff *skb)
{
	struct net_bridge *br;
	unsigned char *dest;
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_port *p;
	int passedup;


	dest = skb->mac.ethernet->h_dest;
	
	p = skb->dev->br_port;
	br = p->br;
	passedup = 0;

	if (!(br->dev.flags & IFF_UP) ||
	    p->state == BR_STATE_DISABLED)
		goto freeandout;

	if (br->dev.flags & IFF_PROMISC) {
		struct sk_buff *skb2;

		skb2 = skb_clone(skb, GFP_ATOMIC);
		if (skb2) {
			passedup = 1;
			br_pass_frame_up(br, skb2);
		}
	}

	if (skb->mac.ethernet->h_source[0] & 1)
		goto freeandout;


	if (!passedup &&
	    (dest[0] & 1) &&
	    (br->dev.flags & IFF_ALLMULTI || br->dev.mc_list != NULL)) {
		struct sk_buff *skb2;

		skb2 = skb_clone(skb, GFP_ATOMIC);
		if (skb2) {
			passedup = 1;
			br_pass_frame_up(br, skb2);
		}
	}

	if (br->stp_enabled &&
	    !memcmp(dest, bridge_ula, 5) &&
	    !(dest[5] & 0xF0))
		goto handle_special_frame;

	if (p->state == BR_STATE_LEARNING ||
	    p->state == BR_STATE_FORWARDING)
		br_fdb_insert(br, p, skb->mac.ethernet->h_source, 0);

	if (p->state != BR_STATE_FORWARDING)
		goto freeandout;

	if (dest[0] & 1) {
		br_flood_forward(br, skb, 1);
		if (!passedup)
			br_pass_frame_up(br, skb);
		else
			kfree_skb(skb);
		return;
	}

	dst = br_fdb_get(br, dest);

	if (dst != NULL && dst->is_local) {
		if (!passedup)
			br_pass_frame_up(br, skb);
		else
			kfree_skb(skb);
		br_fdb_put(dst);
		return;
	}

	if (dst != NULL) {
		br_forward(dst->dst, skb);
		br_fdb_put(dst);
		return;
	}

	br_flood_forward(br, skb, 0);
	return;

 handle_special_frame:
	if (!dest[5]) {
		br_stp_handle_bpdu(skb);
		return;
	}

 freeandout:
	kfree_skb(skb);
}

static void __br_handle_frame(struct sk_buff *skb)
{
#if defined (CONFIG_MIPS_AVALANCHE_FAST_BRIDGE)

	struct net_bridge *br;
	unsigned char *dest;
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_port *p;
	dest = skb->mac.ethernet->h_dest;
	
	p = skb->dev->br_port;
	br = p->br;
	
	if((br->dev.flags &(IFF_UP)) && 
	   !((br->dev.flags &IFF_PROMISC)|| br->br_filter_active || (dest[0] & 1) ))  
	{
	        if (p->state == BR_STATE_FORWARDING)
		{
			dst = br_fdb_get(br, dest);

			if (dst != NULL)  
			{
				br_fdb_insert(br, p, skb->mac.ethernet->h_source, 0);
				if(!dst->is_local)
				{
		    		    br_forward(dst->dst, skb);
		    		    br_fdb_put(dst);
		    		    return;
				}
				else
				{
				    br_pass_frame_up(br, skb);
		    		    br_fdb_put(dst);
				    return;				 
				}
			}

		}
	}
#endif 
	__br_handle_frame_orig(skb);
}

#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
unsigned int wl_lan_ipaddr;
unsigned int wl_lan_enable = 1;


static int br_handle_frame_finish(struct sk_buff *skb)
{
	struct net_bridge *br;
	if (wl_lan_enable == 0)
	{
		if (!strcmp(skb->dev->name, "wlan0"))
		{
			unsigned char protocol;
			unsigned short port;
			struct tcphdr *tcp;
			struct iphdr *iph;

			iph = (struct iphdr*)(skb->data);
	
			if ((iph->protocol == 6) && (iph->daddr == wl_lan_ipaddr))
			{
	
				char *data;
				tcp = (struct tcphdr *)(skb->data + (iph->ihl << 2));
				data = (skb->data + (iph->ihl << 2));
				port = ntohs(tcp->dest);
				if (port == 80 || port == 161)
				{
					printk("WLAN0 DISCAD\n");
					kfree_skb(skb);
					return 0;
				}
			}

		}
	}
	br = skb->dev->br_port->br;
	read_lock(&br->lock);
	__br_handle_frame(skb);
	read_unlock(&br->lock);

	return 0;
}

void br_handle_frame(struct sk_buff *skb)
{	
	NF_HOOK(PF_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL,
			br_handle_frame_finish);
}

#endif
