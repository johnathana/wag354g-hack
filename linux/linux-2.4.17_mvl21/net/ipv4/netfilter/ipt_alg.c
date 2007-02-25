/* Kernel module to match NFMARK values. */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ip_conntrack_lock)
#define DEBUGP(format, args...) printk(format, ##args)

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_alg.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/listhelp.h>

DECLARE_RWLOCK_EXTERN(ip_conntrack_lock);

static int check_spec_alg(const struct sk_buff *skb, char *name)
{
	struct ip_conntrack_tuple tuple;
	struct ip_conntrack_expect *exp;
	struct list_head *exp_entry, *next;
	struct ip_conntrack *ct, *relate_to;
	struct ip_conntrack_protocol *proto;
	int i = 0;
	
	if(name == NULL || !name[0]) 
		return 0;
	
	if(skb->nh.iph->protocol != IPPROTO_TCP 
	   && skb->nh.iph->protocol != IPPROTO_UDP)
		return 0;
	
	if(!skb->nfct)
		return 0;
	
	printk("enter check_spec_alg2: algtype %s\n", name);
	ct = (struct ip_conntrack *)(skb->nfct->master);
	
	//printk("protonum %x  %x:%x --> %x:%x\n", skb->nh.iph->protocol, skb->nh.iph->saddr, skb->h.th->source, skb->nh.iph->daddr, skb->h.th->dest);
	
	if((ct && ct->helper && strstr(ct->helper->name, name)))
	{
		printk("enter check_spec_alg found algtype %s\n", name);
		return 1;
	}

#if 0
	{
	int i = 0;
	for(;i < 40; i ++)
	{
		if(!(i % 16))
		{
			printk("%08x\n", i);
		}
		printk("%02x ", skb->data[i]);
	}
	printk("\n");
	}

	printk("ct %s exist\n", ct?"is":"isnot");
	printk("ct->status %x, ct->master %p\n", ct->status, ct->master);
#endif
	exp = ct->master;
	if(!exp)
		return 0;
	relate_to = exp->expectant;
	//printk("relate_to %p\n", relate_to);
	if((relate_to && relate_to->helper && strstr(relate_to->helper->name, name)))
	{
		//printk("found ok!!!!!!!!\n");
		return 1;
	}
	return 0;
}

static int
match(const struct sk_buff *skb,
      const struct net_device *in,
      const struct net_device *out,
      const void *matchinfo,
      int offset,
      const void *hdr,
      u_int16_t datalen,
      int *hotdrop)
{
	const struct ipt_alg_info *info = matchinfo;
	return (check_spec_alg(skb, (char *)info->name))^info->invert;
}

static int
checkentry(const char *tablename,
           const struct ipt_ip *ip,
           void *matchinfo,
           unsigned int matchsize,
           unsigned int hook_mask)
{
	if (matchsize != IPT_ALIGN(sizeof(struct ipt_alg_info)))
		return 0;

	return 1;
}

static struct ipt_match packet_match
= { { NULL, NULL }, "alg", &match, &checkentry, NULL, THIS_MODULE };

static int __init init(void)
{
	return ipt_register_match(&packet_match);
}

static void __exit fini(void)
{
	ipt_unregister_match(&packet_match);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
