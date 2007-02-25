/* Kernel module to match one of a list of Source IP addresses:  IP addresses are in
   the same place so we can treat them as equal */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>

#include <linux/netfilter_ipv4/ipt_mip.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#if 0 
#define duprintf(format, args...) printk(format , ## args)
#else
#define duprintf(format, args...)
#endif

/* Returns 1 if the port is matched by the test, 0 otherwise. */

static inline int
msip_match(const u_int32_t *iplist, u_int8_t count, u_int8_t flags,  u_int32_t src)
{
    /* JAH_TODO...Eventually need to optimize the search here */

    unsigned int i;
    u_int32_t tmp_ip;

    for (i=0; i<count; i++)
    {
        tmp_ip = iplist[i];

        /* compare the IP addresses */
        
        if(src == ntohl((unsigned int)tmp_ip))
        {
            if(tmp_ip == 0)
                duprintf("zero value in table [%u.%u.%u.%u]\n", HIPQUAD(tmp_ip));
            else
            {
                if(flags & IPT_MIP_INV_MATCH)
                {
                    duprintf("MIP Match (Inv):[src IP %u.%u.%u.%u] [table:%u.%u.%u.%u]: value was on list dont drop\n",HIPQUAD(src),NIPQUAD(tmp_ip));  
                    return 0; /* Value was on the list, dont do match */                 
                }
                else
                {
                    duprintf("Multiple IP[Match]: src IP %u.%u.%u.%u  table:[%u.%u.%u.%u]\n", HIPQUAD(src),NIPQUAD(tmp_ip));            
                    return 1;
                }
            }
        }
        
    }
    if(flags & IPT_MIP_INV_MATCH)
     {
         duprintf("MIP[Match (Inv)]:src IP %u.%u.%u.%u : value not on list do match (drop) \n",HIPQUAD(src));  
         return 1; /* Value was not on the list...do a match */
     }
    else
      return 0;  /* did not match any values contained within the table */

    
}


/*-----------------------------------*/
/* Match function                    */
/*-----------------------------------*/
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
    const struct iphdr *iph = skb->nh.iph;
    const struct ipt_multiip *multiinfo = matchinfo;

    return (msip_match(multiinfo->multip, multiinfo->count, multiinfo->flags,  ntohl(iph->saddr)));
}

/*---------------------------------------------------------*/
/* checkentry function                                     */
/*                                                         */
/* Called when user tries to insert an entry of this type. */
/*---------------------------------------------------------*/
static int
checkentry(const char *tablename,
	   const struct ipt_ip *ip,
	   void *matchinfo,
	   unsigned int matchsize,
	   unsigned int hook_mask)
{
    /* verify size */
    if (matchsize != IPT_ALIGN(sizeof(struct ipt_multiip)))
      return 0;
    
    return 1;
}


static struct ipt_match multiip_match
= { { NULL, NULL }, "mip", &match, &checkentry, NULL, THIS_MODULE };



static int __init init(void)
{
	return ipt_register_match(&multiip_match);
}


static void __exit fini(void)
{
	ipt_unregister_match(&multiip_match);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
