/**************************************************************************
 * FILE PURPOSE	:   IP FAST Path code	
 **************************************************************************
 * FILE NAME	:   ipfastpath.c
 *
 * DESCRIPTION	:
 *  The file contains routines that define the IP fast path. 
 *
 *	CALL-INs:
 *
 *	CALL-OUTs:
 *
 *	User-Configurable Items:
 *
 *	(C) Copyright 2004, Texas Instruments, Inc.
 *************************************************************************/

/***************************** Include Files ****************************/

#include <asm/system.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/config.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <linux/skbuff.h>
#include <linux/igmp.h>
#include <net/sock.h>
#include <net/arp.h>
#include <net/icmp.h>
#include <net/raw.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#include <linux/mroute.h>
#include <linux/netlink.h>

/* NSP Optimization: All the functions in this file lie in the fast path.
 * These have been isolated from the various other files and collected and
 * optimized together. */
#ifndef TI_SLOW_PATH

/* Define this MACRO only for debugging the amount of time each function takes. */
#undef NSP_IP_MEASUREMENTS

/* NSP Optimizations: This macro controls measuring the number of ticks each function
 * in the IP fast path takes. Only for debugging purposes. */
#ifdef NSP_IP_MEASUREMENTS

/* Display the warning message so that the user knows that measurements are
 * being compiled into the code. */
#warning "*********************************************************************"
#warning "IP Fastpath Measurements are being compiled into the code"
#warning "*********************************************************************"

/* Measurement: Macro */
#define rdtscl(dest) \
	__asm__ __volatile__("mfc0 %0,$9; nop" : "=r" (dest))

extern int ip_rcv_start;
extern int ip_rcv_end;
extern int ip_rcv_finish_start;
extern int ip_rcv_finish_end;
extern int ip_forward_start;
extern int ip_forward_end;
extern int ip_forward_finish_start;
extern int ip_forward_finish_end;
extern int ip_finish_output_start;
extern int ip_finish_output_end;
extern int ip_finish_output2_start;
extern int ip_finish_output2_end;

#endif /* NSP_IP_MEASUREMENTS */

/************************ EXTERN DECLARATIONS ****************************/

/* Defined in route.c */
extern struct rt_hash_bucket*   rt_hash_table;
extern struct rt_cache_stat     rt_cache_stat[NR_CPUS];
extern unsigned int			    rt_hash_mask;
extern int ip_route_exception(struct sk_buff *skb, u32 daddr, u32 saddr, u8 tos, struct net_device *dev);

/* Defined in ip_input.c */
extern int ip_process_option (struct sk_buff *skb, struct net_device *dev);
extern int ip_input_error (struct sk_buff *skb);

/* Defined in ip_forward.c */
extern int ip_forward_process_options (struct sk_buff *skb, struct ip_options* opt);
extern void ip_forward_error_handler (struct sk_buff *skb, int code, struct rtable* rt);

/* Defined in ip_output.c */
extern void ip_finish_output_error_handler(struct sk_buff *skb);

/* Static Functions */
static int ip_rcv_finish(struct sk_buff *skb);
static int ip_forward_finish(struct sk_buff *skb);
static int ip_finish_output2(struct sk_buff *skb);
static int ip_fastpath_route_input(struct sk_buff *skb, u32 daddr, u32 saddr, u8 tos, struct net_device *dev);

/**************************************************************************
 * FUNCTION NAME : ip_rcv
 **************************************************************************
 * DESCRIPTION   :
 *  This is the registered handler for receiving all IP packets. The function
 *  validates the IP header and then routes the packet. The function also
 *  passes control to the PRE-ROUTING hook.
***************************************************************************/

#ifdef CONFIG_STB_SUPPORT
#include <stb.h>
#define LAN2WAN 1
#define WAN2LAN 2
extern unsigned int stb_enabled;  /* stb support enabled ? */
extern unsigned int stb_devnum;  /* stb support num ? */
extern unsigned int stb_network;  /* stb server net */
extern unsigned int stb_netmask;  /* stb server netmask */
extern unsigned int lan_ip_addr;  /* lan_ipaddr*/
#endif

int ip_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt)
{
	register struct iphdr*  iph;
    __u32                   total_length;
    int                     ip_header_length;
    
#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_rcv_start);
#endif
	//zhangbin
#ifdef CONFIG_NET_QOS
	if(!iph->tos)
	{
		iph->tos = skb->cos;
		iph->check = 0;
		iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
	}
#endif	

#if 0
	/* PANKAJ TODO: When the interface is in promisc. mode, drop all the crap that it receives, 
     * do not try to analyse it. This check is redundant because we should never receive
     * any such frames. Basic validations have been done at the bridge already and we never run
     * any of our devices in promiscuous mode. */
	if (skb->pkt_type & PACKET_OTHERHOST)
		goto drop;
#endif

    /* Increment the stats. */
	IP_INC_STATS_BH(IpInReceives);

    /* Get the pointer to the IP header. */
	iph = skb->nh.iph;

    /* Get the IP header length.  */
    ip_header_length = iph->ihl << 2;

    /* Get the total length of the received packet as specified by the IP
     * header. */
    total_length = ntohs(iph->tot_len); 

    /* Basic Validations :
     *  1) Version should be 4.
     *  2) IP Header length cannot be less than 20 bytes.
     *  3) Length of received packet cannot be less than the total length
     *     mentioned in the IP packet.  */
    if ((iph->version == 4) && (ip_header_length >= 20) && (skb->len >= total_length))
    {
        /* PANKAJ: TODO: Checksum calculations. This needs to be done only if the packet 
         * is meant for us. No need to do this while forwarding packets. */
#if 0
    	if (ip_fast_csum((u8 *)iph, iph->ihl) != 0)
	    	goto inhdr_error; 
#endif

    	/* Our transport medium may have padded the buffer out. Now we know it
	     * is IP we can trim to the true length of the frame.
    	 * Note this now means skb->len holds ntohs(iph->tot_len).	*/
        if (skb->len > total_length) 
        {
	        __pskb_trim(skb, total_length);
    		if (skb->ip_summed == CHECKSUM_HW)
	    	    skb->ip_summed = CHECKSUM_NONE;
        }

#ifdef NSP_IP_MEASUREMENTS
        rdtscl(ip_rcv_end);
#endif
	       
#ifdef CONFIG_STB_SUPPORT
	if((stb_enabled) && (stb_devnum) && (stb_network) && (stb_netmask))
	  {
                struct stb_dev_info *tmp;
                int forward = 0;
                printk("skb->nh.iph->daddr %x skb->nh.iph->saddr %x\n", skb->nh.iph->daddr, skb->nh.iph->saddr);
                if(((skb->nh.iph->daddr & stb_netmask) == (stb_network & stb_netmask)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->saddr, NULL, NULL)))
                {
                       my_printk("lan2wan\n");
                       forward = LAN2WAN;
                }
                else if(((skb->nh.iph->saddr & stb_netmask) == (stb_network & stb_netmask)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->daddr, NULL, NULL)))
                {
                       my_printk("wan2lan\n");
                       forward = WAN2LAN;
                }
                if(forward == WAN2LAN)
	         {
	               int err, i;
	               struct net_device *lan_dev = NULL;
                       if(!(lan_dev = dev_get_by_name("br0")))
                             goto out;
                        skb->dev = lan_dev;
                        skb->len += ETH_HLEN;
                        skb->data -= ETH_HLEN;
                        memcpy(skb->data, &(tmp->stb_macaddr[0]), ETH_ALEN);
                        dev_queue_xmit(skb);
                        dev_put(lan_dev);
                        goto out;
                }
      		else if(forward == LAN2WAN)
                {
                        int err, i;
                        struct rtable *rt;
                        struct iphdr *iph = skb->nh.iph;
                        struct rt_key key = {
                              dst: iph->daddr,
                              src: 0,
                              oif: 0,
                              tos: RT_TOS(iph->tos)
                           };

                        if((err = ip_route_output_key(&rt, &key)))
                         {
                            if(net_ratelimit())
                               printk("couldn't route pkt(err:%i)", err);
                            goto out;
                         }
			
                        dst_release(skb->dst);
	                skb->dst = NULL;
	                my_printk("after ip_route_output_key\n");
                        {
                                struct hh_cache *hh;
                                struct dst_entry *dst;
                                my_printk("before output really\n");
                                dst = skb->dst = &rt->u.dst;
                                skb->dev = skb->dst->dev;
                                hh = dst->hh;
                                if(hh)
	                        {
                                        read_lock_bh(&hh->hh_lock);
                                        memcpy(skb->data-16, hh->hh_data, 16);
                                        read_unlock_bh(&hh->hh_lock);
                                        skb_push(skb, hh->hh_len);
                                        hh->hh_output(skb);
                                }
                                else if(dst->neighbour)
	                                dst->neighbour->output(skb);
                                else
	                         {
                                        if(net_ratelimit())
                                                printk("no neighbour cache!\n");
                                }
                        }
                        goto out;
                }
        }
#endif	
        /* Pass through the pre-routing hook. */
	    return NF_HOOK(PF_INET, NF_IP_PRE_ROUTING, skb, dev, NULL, ip_rcv_finish);
    }
    else
    {
out:
        /* Failure: IP packet was corrupted, dropping the frame. */
        return ip_input_error (skb);
    }
}

/**************************************************************************
 * FUNCTION NAME : ip_rcv_finish
 **************************************************************************
 * DESCRIPTION   :
 *  This function is called after the PRE-ROUTING hook has been traversed.
 *  It decides the route and passes the packet to the appropriate routed 
 *  destination.
***************************************************************************/
static int ip_rcv_finish(struct sk_buff *skb)
{
    register struct iphdr *iph = skb->nh.iph;
	struct net_device *dev = skb->dev;

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_rcv_finish_start);
#endif

    /* Calculate the route if not done so already by checking the routing table. */
	if (skb->dst == NULL) 
    {
        /* Lookup the routing table. */
		if (ip_fastpath_route_input(skb, iph->daddr, iph->saddr, iph->tos, dev))
            return NET_RX_DROP;
	}

    /* Check if there were any IP otions in the packet. If so handle these options. */
	if (iph->ihl > 5)
    {
        /* Process options. */
        if (ip_process_option (skb, dev) < 0)
            return NET_RX_DROP;
    }

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_rcv_finish_end);
#endif

    /* Send the packet to the computed route. */
	return skb->dst->input(skb);
}

/**************************************************************************
 * FUNCTION NAME : rt_temp_hash_code
 **************************************************************************
 * DESCRIPTION   :
 *  Temporary function copied from route.c. This will be removed. 
***************************************************************************/
static __inline__ unsigned rt_temp_hash_code(u32 daddr, u32 saddr, u8 tos)
{
	unsigned int hash = ((daddr & 0xF0F0F0F0) >> 4) | ((daddr & 0x0F0F0F0F) << 4);
	hash ^= saddr ^ tos;
	hash ^= (hash >> 16);
	return (hash ^ (hash >> 8)) & rt_hash_mask;
}

/**************************************************************************
 * FUNCTION NAME : ip_fastpath_route_input
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the fast path lookup into the routing table. 
 ***************************************************************************/
static int ip_fastpath_route_input
(
    struct sk_buff*     skb,
    u32                 daddr,
    u32                 saddr,
    u8                  tos,
    struct net_device*  dev
)
{
	struct rtable*  rth;
	unsigned        hash;
	int             iif = dev->ifindex;

    /* Create the hash for searching the routing cache table */
	tos &= IPTOS_RT_MASK;
	hash = rt_temp_hash_code(daddr, saddr ^ (iif << 5), tos);

    /* Check if there exists a route to the specific destination. */
	read_lock(&rt_hash_table[hash].lock);
	for (rth = rt_hash_table[hash].chain; rth; rth = rth->u.rt_next) 
    {
        /* Is this a match ? */
		if (rth->key.dst == daddr &&
		    rth->key.src == saddr &&
		    rth->key.iif == iif &&
		    rth->key.oif == 0 &&
		    rth->key.tos == tos) 
        {
            /* YES. Perfect, return the routing destination. */
			rth->u.dst.lastuse = jiffies;
			dst_hold(&rth->u.dst);
			rth->u.dst.__use++;
			rt_cache_stat[smp_processor_id()].in_hit++;
			read_unlock(&rt_hash_table[hash].lock);
			skb->dst = (struct dst_entry*)rth;
			return 0;
		}
	}
	read_unlock(&rt_hash_table[hash].lock);

    /* No routing entry was present in the routing cache. Hit the slow path. 
     * The function cleans the SKB if there was no existing route. */
    return ip_route_exception(skb, daddr, saddr, tos, dev);
}

/**************************************************************************
 * FUNCTION NAME : ip_forward
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called if the packet is to be forwarded to another 
 *  destination. The function does the basic sanity checks and passes the
 *  packet to the netfilter's FORWARDING hook.  
***************************************************************************/
int ip_forward(struct sk_buff *skb)
{
	register struct iphdr*  iph;	/* Our header */
	register struct rtable* rt;	    /* Route we use */
	struct ip_options*      opt	= &(IPCB(skb)->opt);
    int                     retVal = 0;

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_forward_start);
#endif

    /* Determine if there are any options. If there are any then process the options. */
	if (opt->optlen != 0) 
        retVal = ip_forward_process_options(skb, opt);

    /* Get the IP header. */
    iph = skb->nh.iph;

    /* Forward the packet only if we are allowed to do so. */
    if ((retVal == 0) && (iph->ttl > 1))
    {
    	skb->ip_summed = CHECKSUM_NONE;	

        /* Get the route on which the packet can be forwarded. */
    	rt = (struct rtable*)skb->dst;

        /* Having picked a route we can now send the frame out after asking the 
         * firewall permission to do so. */
        skb->priority = rt_tos2priority(iph->tos);

#if 0
        /* PANKAJ TODO: This part needs to be investigated very carefully. Theoretically 
         * we should always have enough headroom. So that a copy is never required. */
    	if (skb_cow(skb, dev2->hard_header_len))
	        goto drop;
        iph = skb->nh.iph;
#endif

        /* Decrease ttl after skb cow done */
        ip_decrease_ttl(iph);

        /* Check if the route on which the packet is being transmitted has a MTU less than the
         * length of packet being transmitted. In such a case we need to fragment the packet but
         * if the DF bit in the IP header is set then send a FRAG NEEDED ICMP error message back
         * to the host. */
        if (((skb->len > rt->u.dst.pmtu) && (ntohs(iph->frag_off) & IP_DF)) == 0)
        {
#ifdef NSP_IP_MEASUREMENTS
            rdtscl(ip_forward_end);
#endif
            /* Forward the packet immediately. */
            return NF_HOOK(PF_INET, NF_IP_FORWARD, skb, skb->dev, rt->u.dst.dev, ip_forward_finish);
        }
        else
        {
            /* Cannot send the packet, because we needed fragmentation but the
             * packet said dont fragment. */
            ip_forward_error_handler (skb, ICMP_DEST_UNREACH, rt);
        }
    }
    else
    {
        /* No. The packet could not be sent out. Send the ICMP message if the
         * packet died. */
        if (iph->ttl > 1)
            ip_forward_error_handler (skb, ICMP_TIME_EXCEEDED, NULL);
    }

    return NET_RX_DROP;
}

/**************************************************************************
 * FUNCTION NAME : ip_forward_finish
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called after the packet has been decided to be forwarded
 *  by the netfilter. It processes any options if any. It also checks if 
 *  the function needs to fragment the packet before sending it out.
 ***************************************************************************/
static int ip_forward_finish(struct sk_buff *skb)
{
	struct ip_options * opt	= &(IPCB(skb)->opt);

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_forward_finish_start);
#endif

    /* Increment the statistics. */
	IP_INC_STATS_BH(IpForwDatagrams);

    /* Determine if there are any options. If there are any then process the options. */
	if (opt->optlen != 0) 
        ip_forward_options(skb);

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_forward_finish_end);
#endif

    /* Check if the packet can be sent out immediately or does it need to be fragmented. */
    return (ip_send(skb));
}

/**************************************************************************
 * FUNCTION NAME : ip_finish_output
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called to send an IP packet initialize the skb fields and
 *  pass the packet through the POST ROUTING Hook.
 ***************************************************************************/
int ip_finish_output(struct sk_buff *skb)
{
	struct net_device *dev = skb->dst->dev;

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_finish_output_start);
#endif

    /* Initialize the skb fields.*/
	skb->dev            = dev;
	skb->protocol       = __constant_htons(ETH_P_IP);
    *(__u32 *)skb->cb   = skb->nh.iph->saddr;

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_finish_output_end);
#endif

    /* Pass the packet through the POST-Routing hook. */
	return NF_HOOK(PF_INET, NF_IP_POST_ROUTING, skb, NULL, dev, ip_finish_output2);
}

/**************************************************************************
 * FUNCTION NAME : mac_header_copy
 **************************************************************************
 * DESCRIPTION   :
 *  Copy the MAC header into the SKB.
 ***************************************************************************/
static void mac_header_copy (unsigned char *ptr_dest, unsigned char *ptr_src)
{
    /* Copy the destination and source MAC headers from the cache. */
    (*((unsigned int *)ptr_dest)       = *((unsigned int *)ptr_src));
    (*((unsigned int *)(ptr_dest+4))   = *((unsigned int *)(ptr_src+4)));
    (*((unsigned int *)(ptr_dest+8))   = *((unsigned int *)(ptr_src+8)));
    (*((unsigned int *)(ptr_dest+12))  = *((unsigned int *)(ptr_src+12)));
    return;
}

/**************************************************************************
 * FUNCTION NAME : ip_finish_output2
 **************************************************************************
 * DESCRIPTION   :
 *  Post Routing Hook has been passed, send the packet to layer 2. 
 ***************************************************************************/
static int ip_finish_output2(struct sk_buff *skb)
{
	struct dst_entry *dst   = skb->dst;
	struct hh_cache *hh     = dst->hh;

#ifdef NSP_IP_MEASUREMENTS
    rdtscl(ip_finish_output2_start);
#endif

    /* Check if there exists a hardware cache. */
	if (hh) 
    {
        /* YES. This is the fast path. Copy the ethernet header and push 
         * the packet out through hh->output which is dev_queue_xmit. */

        /* PANKAJ TODO: Do we need locking. */
#if 0
		read_lock_bh(&hh->hh_lock);
#endif

        /* PANKAJ TODO: The memcpy was copying data as 2 bytes i.e. 8 instructions to copy
         * the header. Copying it unsigned int does the job in 4 instructions. */
#if 0
  		memcpy(skb->data - 16, hh->hh_data, 16);
#else
        mac_header_copy((unsigned char *)(skb->data - 16), (unsigned char *)&hh->hh_data[0]);
#endif

        /* PANKAJ TODO: Do we need locking. The reason why this has been removed is because
         * this function could call the do_softirq which could cause the CPMAC Tasklet to 
         * execute and delay our transmission of the packet. */
#if 0
		read_unlock_bh(&hh->hh_lock);
#endif

        /* Account for the Ethernet header. */
	    skb_push(skb, hh->hh_len);

#ifdef NSP_IP_MEASUREMENTS
        rdtscl(ip_finish_output2_end);
#endif
        /* Send the packet to the device layer. */
		return hh->hh_output(skb);
	} 
    else 
    {
        /* No. This is the slow path. Do we have a registered neighbour */
        if (dst->neighbour)
        {
            /* Pass the packet to the neighbour. This is neigh_resolve_output. */
		    return dst->neighbour->output(skb);
        }
    }

    /* This case should never be reached. There was no header cache and no
     * neighbour. */
   ip_finish_output_error_handler (skb);
   return -EINVAL;
}

#endif /* TI_SLOW_PATH */
