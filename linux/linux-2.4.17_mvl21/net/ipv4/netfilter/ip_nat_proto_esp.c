/*------------------------------------------------------------------------------

 Copyright (c) 2003 Texas Instruments Incorporated

 ALL RIGHTS RESERVED



"This computer program is subject to a separate license agreement

 signed by Texas Instruments Incorporated and the licensee, and is

 subject to the restrictions therein.  No other rights to reproduce,

 use, or disseminate this computer program, whether in part or in

 whole, are granted."

 DESCRIPTION: NAT/Conntrack extension to support ESP

------------------------------------------------------------------------------*/
#include <linux/types.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if.h>

#include <linux/netfilter_ipv4/ip_nat.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_nat_protocol.h>

#include <linux/config.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mukesh Kumar <mkumar@telogy.com>");
MODULE_DESCRIPTION("Netfilter NAT protocol helper module for ESP");

#define DUMP_TUPLE_ESP(x) printk("SRC::%u.%u.%u.%u:%x -> DST::%u.%u.%u.%u:%x\n", \
                       NIPQUAD((x)->src.ip), ntohl((x)->src.u.esp.spi), \
                       NIPQUAD((x)->dst.ip), ntohl((x)->dst.u.esp.spi))

static int
esp_in_range(const struct ip_conntrack_tuple *tuple,
	     enum ip_nat_manip_type maniptype,
	     const union ip_conntrack_manip_proto *min,
	     const union ip_conntrack_manip_proto *max)
{
  return 1;
}

static int
esp_unique_tuple(struct ip_conntrack_tuple *tuple,
		 const struct ip_nat_range *range,
		 enum ip_nat_manip_type maniptype,
		 const struct ip_conntrack *conntrack)
{
    if( maniptype == IP_NAT_MANIP_SRC)
    {
      tuple->dst.u.esp.spi = conntrack->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.esp.spi;
      tuple->src.u.esp.spi = conntrack->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.esp.spi;
    }
    else
    {
      tuple->dst.u.esp.spi = conntrack->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.esp.spi;
      tuple->src.u.esp.spi = conntrack->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.esp.spi;
    }

        DUMP_TUPLE_ESP(tuple);
        return 1;
}

static void
esp_manip_pkt(struct iphdr *iph, size_t len,
	      const struct ip_conntrack_manip *manip,
	      enum ip_nat_manip_type maniptype)
{
  unsigned int *spi = (unsigned int *)((u_int32_t *)iph + iph->ihl);
  return;
}

static unsigned int
esp_print(char *buffer,
	  const struct ip_conntrack_tuple *match,
	  const struct ip_conntrack_tuple *mask)
{
	return 0;
}

static unsigned int
esp_print_range(char *buffer, const struct ip_nat_range *range)
{
  	return 0;
}

struct ip_nat_protocol ip_nat_protocol_esp
= { { NULL, NULL }, "ESP", IPPROTO_ESP,
    esp_manip_pkt,
    esp_in_range,
    esp_unique_tuple,
    esp_print,
    esp_print_range
};

/* nat helper struct */
static int __init init(void)
{
       if (ip_nat_protocol_register(&ip_nat_protocol_esp))
                return -EIO;

        return 0;
}

static void __exit fini(void)
{
        ip_nat_protocol_unregister(&ip_nat_protocol_esp);
}

module_init(init);
module_exit(fini);
