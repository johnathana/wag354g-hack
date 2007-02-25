/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The Internet Protocol (IP) module.
 *
 * Version:	$Id: ip_input.c,v 1.1.1.3 2005/08/25 06:27:04 sparq Exp $
 *
 * Authors:	Ross Biro, <bir7@leland.Stanford.Edu>
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald Becker, <becker@super.org>
 *		Alan Cox, <Alan.Cox@linux.org>
 *		Richard Underwood
 *		Stefan Becker, <stefanb@yello.ping.de>
 *		Jorge Cwik, <jorge@laser.satlink.net>
 *		Arnt Gulbrandsen, <agulbra@nvg.unit.no>
 *		
 *
 * Fixes:
 *		Alan Cox	:	Commented a couple of minor bits of surplus code
 *		Alan Cox	:	Undefining IP_FORWARD doesn't include the code
 *					(just stops a compiler warning).
 *		Alan Cox	:	Frames with >=MAX_ROUTE record routes, strict routes or loose routes
 *					are junked rather than corrupting things.
 *		Alan Cox	:	Frames to bad broadcast subnets are dumped
 *					We used to process them non broadcast and
 *					boy could that cause havoc.
 *		Alan Cox	:	ip_forward sets the free flag on the
 *					new frame it queues. Still crap because
 *					it copies the frame but at least it
 *					doesn't eat memory too.
 *		Alan Cox	:	Generic queue code and memory fixes.
 *		Fred Van Kempen :	IP fragment support (borrowed from NET2E)
 *		Gerhard Koerting:	Forward fragmented frames correctly.
 *		Gerhard Koerting: 	Fixes to my fix of the above 8-).
 *		Gerhard Koerting:	IP interface addressing fix.
 *		Linus Torvalds	:	More robustness checks
 *		Alan Cox	:	Even more checks: Still not as robust as it ought to be
 *		Alan Cox	:	Save IP header pointer for later
 *		Alan Cox	:	ip option setting
 *		Alan Cox	:	Use ip_tos/ip_ttl settings
 *		Alan Cox	:	Fragmentation bogosity removed
 *					(Thanks to Mark.Bush@prg.ox.ac.uk)
 *		Dmitry Gorodchanin :	Send of a raw packet crash fix.
 *		Alan Cox	:	Silly ip bug when an overlength
 *					fragment turns up. Now frees the
 *					queue.
 *		Linus Torvalds/ :	Memory leakage on fragmentation
 *		Alan Cox	:	handling.
 *		Gerhard Koerting:	Forwarding uses IP priority hints
 *		Teemu Rantanen	:	Fragment problems.
 *		Alan Cox	:	General cleanup, comments and reformat
 *		Alan Cox	:	SNMP statistics
 *		Alan Cox	:	BSD address rule semantics. Also see
 *					UDP as there is a nasty checksum issue
 *					if you do things the wrong way.
 *		Alan Cox	:	Always defrag, moved IP_FORWARD to the config.in file
 *		Alan Cox	: 	IP options adjust sk->priority.
 *		Pedro Roque	:	Fix mtu/length error in ip_forward.
 *		Alan Cox	:	Avoid ip_chk_addr when possible.
 *	Richard Underwood	:	IP multicasting.
 *		Alan Cox	:	Cleaned up multicast handlers.
 *		Alan Cox	:	RAW sockets demultiplex in the BSD style.
 *		Gunther Mayer	:	Fix the SNMP reporting typo
 *		Alan Cox	:	Always in group 224.0.0.1
 *	Pauline Middelink	:	Fast ip_checksum update when forwarding
 *					Masquerading support.
 *		Alan Cox	:	Multicast loopback error for 224.0.0.1
 *		Alan Cox	:	IP_MULTICAST_LOOP option.
 *		Alan Cox	:	Use notifiers.
 *		Bjorn Ekwall	:	Removed ip_csum (from slhc.c too)
 *		Bjorn Ekwall	:	Moved ip_fast_csum to ip.h (inline!)
 *		Stefan Becker   :       Send out ICMP HOST REDIRECT
 *	Arnt Gulbrandsen	:	ip_build_xmit
 *		Alan Cox	:	Per socket routing cache
 *		Alan Cox	:	Fixed routing cache, added header cache.
 *		Alan Cox	:	Loopback didn't work right in original ip_build_xmit - fixed it.
 *		Alan Cox	:	Only send ICMP_REDIRECT if src/dest are the same net.
 *		Alan Cox	:	Incoming IP option handling.
 *		Alan Cox	:	Set saddr on raw output frames as per BSD.
 *		Alan Cox	:	Stopped broadcast source route explosions.
 *		Alan Cox	:	Can disable source routing
 *		Takeshi Sone    :	Masquerading didn't work.
 *	Dave Bonn,Alan Cox	:	Faster IP forwarding whenever possible.
 *		Alan Cox	:	Memory leaks, tramples, misc debugging.
 *		Alan Cox	:	Fixed multicast (by popular demand 8))
 *		Alan Cox	:	Fixed forwarding (by even more popular demand 8))
 *		Alan Cox	:	Fixed SNMP statistics [I think]
 *	Gerhard Koerting	:	IP fragmentation forwarding fix
 *		Alan Cox	:	Device lock against page fault.
 *		Alan Cox	:	IP_HDRINCL facility.
 *	Werner Almesberger	:	Zero fragment bug
 *		Alan Cox	:	RAW IP frame length bug
 *		Alan Cox	:	Outgoing firewall on build_xmit
 *		A.N.Kuznetsov	:	IP_OPTIONS support throughout the kernel
 *		Alan Cox	:	Multicast routing hooks
 *		Jos Vos		:	Do accounting *before* call_in_firewall
 *	Willy Konynenberg	:	Transparent proxying support
 *
 *  
 *
 * To Fix:
 *		IP fragmentation wants rewriting cleanly. The RFC815 algorithm is much more efficient
 *		and could be made very efficient with the addition of some virtual memory hacks to permit
 *		the allocation of a buffer that can then be 'grown' by twiddling page tables.
 *		Output fragmentation wants updating along with the buffer management to use a single 
 *		interleaved copy algorithm so that fragmenting has a one copy overhead. Actual packet
 *		output should probably do its own fragmentation at the UDP/RAW layer. TCP shouldn't cause
 *		fragmentation anyway.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
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
#include <net/sock.h>
#include <net/arp.h>
#include <net/icmp.h>
#include <net/raw.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#include <linux/mroute.h>
#include <linux/netlink.h>

/*
 *	SNMP management statistics
 */

struct ip_mib ip_statistics[NR_CPUS*2];

/*
 *	Process Router Attention IP option
 */ 
int ip_call_ra_chain(struct sk_buff *skb)
{
	struct ip_ra_chain *ra;
	u8 protocol = skb->nh.iph->protocol;
	struct sock *last = NULL;

	read_lock(&ip_ra_lock);
	for (ra = ip_ra_chain; ra; ra = ra->next) {
		struct sock *sk = ra->sk;

		/* If socket is bound to an interface, only report
		 * the packet if it came  from that interface.
		 */
		if (sk && sk->num == protocol 
		    && ((sk->bound_dev_if == 0) 
			|| (sk->bound_dev_if == skb->dev->ifindex))) {
			if (skb->nh.iph->frag_off & htons(IP_MF|IP_OFFSET)) {
				skb = ip_defrag(skb);
				if (skb == NULL) {
					read_unlock(&ip_ra_lock);
					return 1;
				}
			}
			if (last) {
				struct sk_buff *skb2 = skb_clone(skb, GFP_ATOMIC);
				if (skb2)
					raw_rcv(last, skb2);
			}
			last = sk;
		}
	}

	if (last) {
		raw_rcv(last, skb);
		read_unlock(&ip_ra_lock);
		return 1;
	}
	read_unlock(&ip_ra_lock);
	return 0;
}

/* Handle this out of line, it is rare. */
static int ip_run_ipprot(struct sk_buff *skb, struct iphdr *iph,
			 struct inet_protocol *ipprot, int force_copy)
{
	int ret = 0;

	do {
		if (ipprot->protocol == iph->protocol) {
			struct sk_buff *skb2 = skb;
			if (ipprot->copy || force_copy)
				skb2 = skb_clone(skb, GFP_ATOMIC);
			if(skb2 != NULL) {
				ret = 1;
				ipprot->handler(skb2);
			}
		}
		ipprot = (struct inet_protocol *) ipprot->next;
	} while(ipprot != NULL);

	return ret;
}

static inline int ip_local_deliver_finish(struct sk_buff *skb)
{
	int ihl = skb->nh.iph->ihl*4;

#ifdef CONFIG_NETFILTER_DEBUG
	nf_debug_ip_local_deliver(skb);
#endif /*CONFIG_NETFILTER_DEBUG*/

	/* Pull out additionl 8 bytes to save some space in protocols. */
	if (!pskb_may_pull(skb, ihl+8))
		goto out;
	__skb_pull(skb, ihl);

#ifdef CONFIG_NETFILTER
	/* Free reference early: we don't need it any more, and it may
           hold ip_conntrack module loaded indefinitely. */
	nf_conntrack_put(skb->nfct);
	skb->nfct = NULL;
#endif /*CONFIG_NETFILTER*/

        /* Point into the IP datagram, just past the header. */
        skb->h.raw = skb->data;

	{
		/* Note: See raw.c and net/raw.h, RAWV4_HTABLE_SIZE==MAX_INET_PROTOS */
		int protocol = skb->nh.iph->protocol;
		int hash = protocol & (MAX_INET_PROTOS - 1);
		struct sock *raw_sk = raw_v4_htable[hash];
		struct inet_protocol *ipprot;
		int flag;

		/* If there maybe a raw socket we must check - if not we
		 * don't care less
		 */
		if(raw_sk != NULL)
			raw_sk = raw_v4_input(skb, skb->nh.iph, hash);

		ipprot = (struct inet_protocol *) inet_protos[hash];
		flag = 0;
		if(ipprot != NULL) {
			if(raw_sk == NULL &&
			   ipprot->next == NULL &&
			   ipprot->protocol == protocol) {
				int ret;

				/* Fast path... */
				ret = ipprot->handler(skb);

				return ret;
			} else {
				flag = ip_run_ipprot(skb, skb->nh.iph, ipprot, (raw_sk != NULL));
			}
		}

		/* All protocols checked.
		 * If this packet was a broadcast, we may *not* reply to it, since that
		 * causes (proven, grin) ARP storms and a leakage of memory (i.e. all
		 * ICMP reply messages get queued up for transmission...)
		 */
		if(raw_sk != NULL) {	/* Shift to last raw user */
			raw_rcv(raw_sk, skb);
			sock_put(raw_sk);
		} else if (!flag) {		/* Free and report errors */
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, 0);	
out:
			kfree_skb(skb);
		}
	}

	return 0;
}

/*
 * 	Deliver IP Packets to the higher protocol layers.
 */ 
int ip_local_deliver(struct sk_buff *skb)
{
    /* NSP Optimization: Get the IP header. The checksum calculations have been moved 
     * here from the core IP receive path for optimizations. Checksum is computed only 
     * for locally destined packets and not for forwarded packets. */
#ifndef  TI_SLOW_PATH
    struct iphdr *iph;

    iph = skb->nh.iph;
	if (ip_fast_csum((u8 *)iph, iph->ihl) != 0)
    {
    	IP_INC_STATS_BH(IpInHdrErrors);
        kfree_skb(skb);
        return NET_RX_DROP;
    }
#endif /* TI_SLOW_PATH */

	/*
	 *	Reassemble IP fragments.
	 */

	if (skb->nh.iph->frag_off & htons(IP_MF|IP_OFFSET)) {
		skb = ip_defrag(skb);
		if (!skb)
			return 0;
	}

	return NF_HOOK(PF_INET, NF_IP_LOCAL_IN, skb, skb->dev, NULL,
		       ip_local_deliver_finish);
}

/* NSP Optimization: The function has been added to process any options that
 * might be present in the IP header. The function is called from the
 * ip fastpath implementation. */
#ifndef TI_SLOW_PATH
/**************************************************************************
 * FUNCTION NAME : ip_process_option
 **************************************************************************
 * DESCRIPTION   :
 *  Process the options if any in the received IP packet. This has been made
 *  as a seperate function since this is not in the fast path.
 *
 * RETURNS       :
 *      0       - Success.
 *      < 0     - Error.
 *
 * NOTES         :
 *  The function cleans the SKB on Error.
***************************************************************************/
int ip_process_option (struct sk_buff *skb, struct net_device *dev)
{
    struct ip_options *opt;
    struct iphdr *iph;

	/* It looks as overkill, because not all
	   IP options require packet mangling.
	   But it is the easiest for now, especially taking
	   into account that combination of IP options
	   and running sniffer is extremely rare condition.
	        --ANK (980813)
    */
	if (skb_cow(skb, skb_headroom(skb)))
	    goto drop;

    iph = skb->nh.iph;
    skb->ip_summed = 0;

    if (ip_options_compile(NULL, skb))
	    goto inhdr_error;

    opt = &(IPCB(skb)->opt);
	if (opt->srr) 
    {
	    struct in_device *in_dev = in_dev_get(dev);
		if (in_dev) {
            if (!IN_DEV_SOURCE_ROUTE(in_dev)) {
					if (IN_DEV_LOG_MARTIANS(in_dev) && net_ratelimit())
						printk(KERN_INFO "source route option %u.%u.%u.%u -> %u.%u.%u.%u\n",
						       NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
					in_dev_put(in_dev);
					goto drop;
				}
				in_dev_put(in_dev);
			}
			if (ip_options_rcv_srr(skb))
				goto drop;
    }
    return 0;

inhdr_error:
	IP_INC_STATS_BH(IpInHdrErrors);
drop:
    kfree_skb(skb);
    return -1;
}

/**************************************************************************
 * FUNCTION NAME : ip_input_error
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called from the fast path if there is any error that
 *  was detected in the received IP frame. It increments the stat counter 
 *  and cleans the packet.
***************************************************************************/
int ip_input_error (struct sk_buff *skb)
{
    IP_INC_STATS_BH(IpInHdrErrors);
    kfree_skb(skb);
    return NET_RX_DROP;
}

#endif /* TI_SLOW_PATH */


/* NSP Optimizations: The macro TI_SLOW_PATH controls all NSP
 * Optimizations in the IPV4. Defining this macro means re-enabling the
 * orignal linux packet flow through the IP stack. */
#ifdef TI_SLOW_PATH
static inline int ip_rcv_finish(struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	struct iphdr *iph = skb->nh.iph;

	/*
	 *	Initialise the virtual path cache for the packet. It describes
	 *	how the packet travels inside Linux networking.
	 */ 
	if (skb->dst == NULL) {
		if (ip_route_input(skb, iph->daddr, iph->saddr, iph->tos, dev))
			goto drop; 
	}

#ifdef CONFIG_NET_CLS_ROUTE
	if (skb->dst->tclassid) {
		struct ip_rt_acct *st = ip_rt_acct + 256*smp_processor_id();
		u32 idx = skb->dst->tclassid;
		st[idx&0xFF].o_packets++;
		st[idx&0xFF].o_bytes+=skb->len;
		st[(idx>>16)&0xFF].i_packets++;
		st[(idx>>16)&0xFF].i_bytes+=skb->len;
	}
#endif

	if (iph->ihl > 5) {
		struct ip_options *opt;

		/* It looks as overkill, because not all
		   IP options require packet mangling.
		   But it is the easiest for now, especially taking
		   into account that combination of IP options
		   and running sniffer is extremely rare condition.
		                                      --ANK (980813)
		*/

		if (skb_cow(skb, skb_headroom(skb)))
			goto drop;
		iph = skb->nh.iph;

		skb->ip_summed = 0;
		if (ip_options_compile(NULL, skb))
			goto inhdr_error;

		opt = &(IPCB(skb)->opt);
		if (opt->srr) {
			struct in_device *in_dev = in_dev_get(dev);
			if (in_dev) {
				if (!IN_DEV_SOURCE_ROUTE(in_dev)) {
					if (IN_DEV_LOG_MARTIANS(in_dev) && net_ratelimit())
						printk(KERN_INFO "source route option %u.%u.%u.%u -> %u.%u.%u.%u\n",
						       NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
					in_dev_put(in_dev);
					goto drop;
				}
				in_dev_put(in_dev);
			}
			if (ip_options_rcv_srr(skb))
				goto drop;
		}
	}

	return skb->dst->input(skb);

inhdr_error:
	IP_INC_STATS_BH(IpInHdrErrors);
drop:
        kfree_skb(skb);
        return NET_RX_DROP;
}

#ifdef CONFIG_STB_SUPPORT
#include <stb.h>
#define LAN2WAN 1
#define WAN2LAN 2
#define LAN2WAN_MULTICAST 3
#define WAN2LAN_MULTICAST 4
extern unsigned int stb_enabled;  /* stb support enabled ? */
extern unsigned int stb_devnum;  /* stb support num ? */
extern unsigned int stb_network;  /* stb server net */
extern unsigned int stb_netmask;  /* stb server netmask */
extern unsigned int lan_ip_addr;  /* lan_ipaddr*/

#include <net/ip_fib.h>
typedef struct {
	u32	datum;
} fn_key_t;

typedef struct {
	u32	datum;
} fn_hash_idx_t;

struct fib_node
{
	struct fib_node		*fn_next;
	struct fib_info		*fn_info;
#define FIB_INFO(f)	((f)->fn_info)
	fn_key_t		fn_key;
	u8			fn_tos;
	u8			fn_type;
	u8			fn_scope;
	u8			fn_state;
};

struct fn_zone
{
	struct fn_zone	*fz_next;	/* Next not empty zone	*/
	struct fib_node	**fz_hash;	/* Hash table pointer	*/
	int		fz_nent;	/* Number of entries	*/

	int		fz_divisor;	/* Hash divisor		*/
	u32		fz_hashmask;	/* (1<<fz_divisor) - 1	*/
#define FZ_HASHMASK(fz)	((fz)->fz_hashmask)

	int		fz_order;	/* Zone order		*/
	u32		fz_mask;
#define FZ_MASK(fz)	((fz)->fz_mask)
};

struct fn_hash
{
	struct fn_zone	*fn_zones[33];
	struct fn_zone	*fn_zone_list;
};
extern struct fib_table *main_table;
#endif
/*
 * 	Main IP Receive routine.
 */ 
int ip_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt)
{
	struct iphdr *iph;

	/* When the interface is in promisc. mode, drop all the crap
	 * that it receives, do not try to analyse it.
	 */
	if (skb->pkt_type == PACKET_OTHERHOST)
		goto drop;

	IP_INC_STATS_BH(IpInReceives);

	if ((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)
		goto out;

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		goto inhdr_error;

	iph = skb->nh.iph;

	/*
	 *	RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails the checksum.
	 *
	 *	Is the datagram acceptable?
	 *
	 *	1.	Length at least the size of an ip header
	 *	2.	Version of 4
	 *	3.	Checksums correctly. [Speed optimisation for later, skip loopback checksums]
	 *	4.	Doesn't have a bogus length
	 */

	if (iph->ihl < 5 || iph->version != 4)
		goto inhdr_error; 

	if (!pskb_may_pull(skb, iph->ihl*4))
		goto inhdr_error;

	iph = skb->nh.iph;

	if (ip_fast_csum((u8 *)iph, iph->ihl) != 0)
		goto inhdr_error; 
	//zhangbin 2004-12-16
#ifdef CONFIG_NET_QOS
	if(!iph->tos)
	{
		iph->tos = skb->cos;
		iph->check = 0;
		iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
	}
#endif	

	{
		__u32 len = ntohs(iph->tot_len); 
		if (skb->len < len || len < (iph->ihl<<2))
			goto inhdr_error;

		/* Our transport medium may have padded the buffer out. Now we know it
		 * is IP we can trim to the true length of the frame.
		 * Note this now means skb->len holds ntohs(iph->tot_len).
		 */
		if (skb->len > len) {
			__pskb_trim(skb, len);
			if (skb->ip_summed == CHECKSUM_HW)
				skb->ip_summed = CHECKSUM_NONE;
		}
	}
	
#ifdef CONFIG_STB_SUPPORT
        if((stb_enabled) && (stb_devnum) && (stb_network) && (stb_netmask))
	  {
                struct stb_dev_info *tmp;
                int forward = 0;
                my_printk("skb->nh.iph->daddr %x skb->nh.iph->saddr %x,devname=%s\n", skb->nh.iph->daddr, skb->nh.iph->saddr,skb->dev->name);
		/* zhangbin 2005.5.12 */
                //if(((skb->nh.iph->daddr & stb_netmask) == (stb_network & stb_netmask)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->saddr, NULL, NULL)))
                if((((skb->nh.iph->daddr & stb_netmask) == (stb_network & stb_netmask)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->saddr, NULL, NULL))))
                {
                	my_printk("lan2wan:daddr=%x saddr=%x,devname=%s\n", skb->nh.iph->daddr, skb->nh.iph->saddr,skb->dev->name);
                       forward = LAN2WAN;
                }
		else if(((MULTICAST(skb->nh.iph->daddr)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->saddr, NULL, NULL))))
                {
                	my_printk("lan2wan_mul:daddr=%x saddr=%x,devname=%s\n", skb->nh.iph->daddr, skb->nh.iph->saddr,skb->dev->name);
                       forward = LAN2WAN_MULTICAST;
                }

                else if(((skb->nh.iph->saddr & stb_netmask) == (stb_network & stb_netmask)) && (tmp = find_stbinfo(IPONLY, skb->nh.iph->daddr, NULL, NULL)))
		{
                	my_printk("1:wan2lan:daddr=%x saddr=%x,devname=%s\n", skb->nh.iph->daddr, skb->nh.iph->saddr,skb->dev->name);
                       forward = WAN2LAN;
		}
                else if((MULTICAST(skb->nh.iph->daddr) && strcmp(skb->dev->name,"br0") && strcmp(skb->dev->name,"eth0")))
                {
                	my_printk("wan2lan_mul:daddr=%x saddr=%x,devname=%s\n", skb->nh.iph->daddr, skb->nh.iph->saddr,skb->dev->name);
                       forward = WAN2LAN_MULTICAST;
                }

                if(forward == WAN2LAN)
	         {
	               int err, i;
	               struct net_device *lan_dev = NULL;
                       if(!(lan_dev = dev_get_by_name("br0")))
                             goto drop;
                        skb->dev = lan_dev;
                        skb->len += ETH_HLEN;
                        skb->data -= ETH_HLEN;
                        memcpy(skb->data, &(tmp->stb_macaddr[0]), ETH_ALEN);
			
                        dev_queue_xmit(skb);
                        dev_put(lan_dev);
                        goto out;
                }
		else if(forward == WAN2LAN_MULTICAST)
		{
		       int err, i;
	               struct net_device *lan_dev = NULL;
                       if(!(lan_dev = dev_get_by_name("br0")))
                             goto drop;
			skb->dev = lan_dev;
                        skb->len += ETH_HLEN;
                        skb->data -= ETH_HLEN;
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
                            goto drop;
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
					
				/* zhangbin 2005.5.12 */
				my_printk("dev->name=%s!\n",skb->dev->name);

                                hh = dst->hh;
                                if(hh)
	                        {
                                        read_lock_bh(&hh->hh_lock);
                                        memcpy(skb->data-16, hh->hh_data, 16);
                                        read_unlock_bh(&hh->hh_lock);
                                        skb_push(skb, hh->hh_len);
					
					/* zhangbin 2005.5.12 */
					my_printk("will hh_output!\n");

                                        hh->hh_output(skb);
                                }
                                else if(dst->neighbour)
				{
					
					/* zhangbin 2005.5.12 */
					my_printk("will neighbour->output!\n");

	                                dst->neighbour->output(skb);
				}
                                else
	                         {
                                        if(net_ratelimit())
                                                printk("no neighbour cache!\n");
                                }
                        }

                        goto out;
                }
		else if(forward == LAN2WAN_MULTICAST)
                {
 			int err, i;
			struct fn_hash *table = (struct fn_hash*)main_table->tb_data;
			struct fn_zone *fz;
			struct fib_node *f;
			struct fib_node **fp;
 		        struct net_device *wan_dev = NULL;
			
			//get default gateway
			fz = table->fn_zones[0];
			fp = fz->fz_hash;
			f = *fp;
			
			wan_dev = FIB_INFO(f)->fib_dev;

			my_printk("............ for multicast wan_dev %s ..................\n", wan_dev->name);
			
			skb->dev = wan_dev;
                        skb->len += ETH_HLEN;
       	                skb->data -= ETH_HLEN;
               	        dev_queue_xmit(skb);
                       	//dev_put(wan_dev);
                        goto out;
			
		}

        }
#endif
	return NF_HOOK(PF_INET, NF_IP_PRE_ROUTING, skb, dev, NULL,
		       ip_rcv_finish);

inhdr_error:
	IP_INC_STATS_BH(IpInHdrErrors);
drop:
        kfree_skb(skb);
out:
        return NET_RX_DROP;
}

#endif /* TI_SLOW_PATH */

