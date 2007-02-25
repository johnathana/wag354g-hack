/* Kernel module to match RPC connection tracking information. */
/* Marcelo Barbosa Lima - marcelo.lima@dcc.unicamp.br */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/list.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ip_conntrack_rpc.h>
#include <linux/netfilter_ipv4/lockhelp.h>

EXPORT_NO_SYMBOLS;

extern struct list_head expect_rpc_list_tcp; 
extern struct module *ip_conntrack_rpc_tcp;
extern struct list_head expect_rpc_list_udp;
extern struct module *ip_conntrack_rpc_udp;
DECLARE_RWLOCK_EXTERN(ipct_rpc_tcp_lock);
DECLARE_RWLOCK_EXTERN(ipct_rpc_udp_lock);

#define ASSERT_READ_LOCK(x)					\
do {								\
	if (x == &expect_rpc_list_udp)				\
		MUST_BE_READ_LOCKED(&ipct_rpc_udp_lock);	\
	else if (x == &expect_rpc_list_tcp)			\
		MUST_BE_READ_LOCKED(&ipct_rpc_tcp_lock);	\
} while (0)
			
#define ASSERT_WRITE_LOCK(x)					\
do {								\
	if (x == &expect_rpc_list_udp)				\
		MUST_BE_WRITE_LOCKED(&ipct_rpc_udp_lock);	\
	else if (x == &expect_rpc_list_tcp)			\
		MUST_BE_WRITE_LOCKED(&ipct_rpc_tcp_lock);	\
} while (0)
#include <linux/netfilter_ipv4/listhelp.h>


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
	struct ip_conntrack *ct;
	enum ip_conntrack_info ctinfo;
	struct expect_rpc *rpc_bind;
	const u_int32_t *data;
	enum ip_conntrack_dir dir;
	const struct tcphdr *tcp;
	
	ct = ip_conntrack_get((struct sk_buff *)skb, &ctinfo);
	if (ct) {	
         dir = CTINFO2DIR(ctinfo);

	/* This does sanity check in UDP or TCP packets like their */
	/* respective modules and permits only get port procedure in */
	/* client communications with portmapper */
	 switch(ct->tuplehash[0].tuple.dst.protonum){	
	
	   case IPPROTO_UDP:
	 	if(offset == 0 && datalen < sizeof(struct udphdr)) {
		    *hotdrop = 1;
		    return 0;
	 	}	
		if(ntohs(ct->tuplehash[dir].tuple.dst.u.all) == 111){	

	           if((datalen - sizeof(struct udphdr)) != 56)
                        return 0;

		   data = (const u_int32_t *)hdr + 7;
		   if(IXDR_GET_INT32(data) != 3) 
			 return 0;
		   else  
			return (1 && (!offset)); 
		}
		if(ntohs(ct->tuplehash[!dir].tuple.dst.u.all) == 111) 
		   return (1 && (!offset));
		
	   case IPPROTO_TCP:
		if(offset == 0 && datalen < sizeof(struct tcphdr)) {
			*hotdrop = 1;
			return 0;
		}	
		if(ntohs(ct->tuplehash[dir].tuple.dst.u.all) == 111) { 
			tcp = hdr;
			if(datalen == (tcp->doff * 4)) return (1 && (!offset));

 	                /* Tests if packet len is ok */
                        if((datalen - (tcp->doff * 4)) != 60)
                             return 0;
			data = (const u_int32_t *)tcp + tcp->doff + 6;	

                   if(IXDR_GET_INT32(data) != 3) 
                         return 0;
                   else
                        return (1 && (!offset));
		}
		if(ntohs(ct->tuplehash[!dir].tuple.dst.u.all) == 111)
                   return (1 && (!offset));
	 }

	/* Client using UDP in communication with portmapper */
	WRITE_LOCK(&ipct_rpc_udp_lock);
	rpc_bind = LIST_FIND(&expect_rpc_list_udp, expect_rpc_cmp, struct expect_rpc *, 
 			    ntohs(ct->tuplehash[0].tuple.dst.u.all),
			    ct->tuplehash[1].tuple.src.ip, ct->tuplehash[0].tuple.src.ip,
			    ct->tuplehash[0].tuple.dst.protonum);

	if(rpc_bind) { 
	   if(del_timer(&rpc_bind->timeout)) {
	      rpc_bind->timeout.expires = jiffies + EXPIRES;
	      add_timer(&rpc_bind->timeout);
	   }
	   WRITE_UNLOCK(&ipct_rpc_udp_lock);
	/* Must not be a fragment */
	return (1 && (!offset));
	}
	WRITE_UNLOCK(&ipct_rpc_udp_lock);

	/* Client using TCP in communication with portmapper */
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	rpc_bind = LIST_FIND(&expect_rpc_list_tcp, expect_rpc_cmp, struct expect_rpc *, 
 			    ntohs(ct->tuplehash[0].tuple.dst.u.all),
			    ct->tuplehash[1].tuple.src.ip, ct->tuplehash[0].tuple.src.ip,
			    ct->tuplehash[0].tuple.dst.protonum);

	if (rpc_bind) {
	   if (del_timer(&rpc_bind->timeout)) {
	      rpc_bind->timeout.expires = jiffies + EXPIRES;
	      add_timer(&rpc_bind->timeout);
	   }
	   WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	   return (1 && (!offset));
	} 
	   WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	return 0;
	}
	return 0;
}


static struct ipt_match record_rpc_match
= { { NULL, NULL }, "record_rpc", &match, NULL, NULL, THIS_MODULE };

int __init init(void)
{
	__MOD_INC_USE_COUNT(ip_conntrack_module);
	__MOD_INC_USE_COUNT(ip_conntrack_rpc_udp);
	__MOD_INC_USE_COUNT(ip_conntrack_rpc_tcp);
	return ipt_register_match(&record_rpc_match);
}

void __exit cleanup(void)
{
	ipt_unregister_match(&record_rpc_match);
	__MOD_DEC_USE_COUNT(ip_conntrack_module);
	__MOD_DEC_USE_COUNT(ip_conntrack_rpc_tcp);
	__MOD_DEC_USE_COUNT(ip_conntrack_rpc_udp);
}

module_init(init);
module_exit(cleanup);
