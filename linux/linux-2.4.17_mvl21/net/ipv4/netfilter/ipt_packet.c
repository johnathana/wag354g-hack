/* Kernel module to match NFMARK values. */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_packet.h>

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
	const struct ipt_packet_info *info = matchinfo;
	unsigned short proto = htons(info->proto);
	if((info->len > 0) && !((skb->len == info->len)^(info->invert & IPT_PACKET_INV_LEN)))
		return 0;
	//if((info->proto > 0) && !((skb->protocol == info->proto)^(info->invert & IPT_PACKET_INV_PROTO)))
	if((proto > 0) && !((skb->mac.raw >= skb->head) && (skb->mac.raw + ETH_HLEN <= skb->data) && (!memcmp(skb->mac.raw + 2*ETH_ALEN, &proto, 2))^(info->invert & IPT_PACKET_INV_PROTO)))
	//if((info->proto > 0) && !((!memcmp(skb->mac.raw + 2*ETH_ALEN, &info->proto, 2))^(info->invert & IPT_PACKET_INV_PROTO)))
		return 0;
#ifdef CONFIG_VLAN_8021Q
	if((info->vid > 0) && !((skb->vid == info->vid)^(info->invert & IPT_PACKET_INV_VID)))
		return 0;
	if((info->cos > 0) && !((skb->cos == info->cos)^(info->invert & IPT_PACKET_INV_COS)))
		return 0;
#endif
	return 1;	
}

static int
checkentry(const char *tablename,
           const struct ipt_ip *ip,
           void *matchinfo,
           unsigned int matchsize,
           unsigned int hook_mask)
{
	if (matchsize != IPT_ALIGN(sizeof(struct ipt_packet_info)))
		return 0;

	return 1;
}

static struct ipt_match packet_match
= { { NULL, NULL }, "packet", &match, &checkentry, NULL, THIS_MODULE };

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
