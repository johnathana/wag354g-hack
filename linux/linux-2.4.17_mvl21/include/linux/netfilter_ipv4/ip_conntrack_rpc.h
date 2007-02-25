/* RPC extension for IP connection tracking. */
/* Marcelo Barbosa Lima - marcelo.lima@dcc.unicamp.br */

#include <asm/param.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/stddef.h>
#include <linux/list.h>

#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>

/* Data in RPC packet is encoded in XDR */
#define IXDR_GET_INT32(buf) ((u_int32_t) ntohl((uint32_t)*buf))

#if 0
#define DEBUGRPC printk
#else
#define DEBUGRPC(format, args...)
#endif

/* Fast timeout, to deny DoS atacks */
#define EXP (60 * HZ)

/* Normal timeouts */
#define EXPIRES (180 * HZ)

 /* For future conections RPC, using client's cache bindings */
 /* I'll use ip_conntrack_lock to lock these lists */

/* This identifies each request and stores protocol */

struct request_p {
   struct list_head list;

   u_int32_t xid;   
   u_int32_t ip;
   u_int16_t port;
	
   /* Protocol */
   u_int16_t proto;

   struct timer_list timeout;
};

struct expect_rpc {
   /* list */
   struct list_head list;

   /* binding */
   u_int32_t ip;
   u_int16_t port;
   u_int32_t ip_rmt;
   u_int16_t proto;
  
   /* Expect RPC conection for long time, but this time expires */
   struct timer_list timeout;  
};

inline int expect_rpc_cmp(const struct expect_rpc *l, u_int16_t port,
			  u_int32_t ip, u_int32_t ip_rmt, u_int16_t proto)
{
	return (l->ip == ip && l->port == port &&
		l->ip_rmt == ip_rmt && l->proto == proto);
}

inline int request_p_cmp(const struct request_p *p, u_int32_t xid, 
			 u_int32_t ip, u_int32_t port) {
	return(p->xid == xid && p->ip == ip && p->port);

}
