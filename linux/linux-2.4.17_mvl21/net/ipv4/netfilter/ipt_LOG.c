/*
 * This is a module which is used for logging packets.
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/spinlock.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#include <net/route.h>
#include <linux/netfilter_ipv4/ipt_LOG.h>


static unsigned int
ipt_log_target(struct sk_buff **pskb,
	       unsigned int hooknum,
	       const struct net_device *in,
	       const struct net_device *out,
	       const void *targinfo,
	       void *userinfo)
{
	return IPT_CONTINUE;
}

static int ipt_log_checkentry(const char *tablename,
			      const struct ipt_entry *e,
			      void *targinfo,
			      unsigned int targinfosize,
			      unsigned int hook_mask)
{
	return 1;
}

static struct ipt_target ipt_log_reg
= { { NULL, NULL }, "LOG", ipt_log_target, ipt_log_checkentry, NULL, 
    THIS_MODULE };

static int __init init(void)
{
	if (ipt_register_target(&ipt_log_reg))
		return -EINVAL;

	return 0;
}

static void __exit fini(void)
{
	ipt_unregister_target(&ipt_log_reg);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
