/* RPC extension for IP connection tracking. */
/* Marcelo Barbosa Lima - marcelo.lima@dcc.unicamp.br */
/* Yes, my english is very poor :-) */

/* 25 Mar 2002  Harald Welte <laforge@gnumonks.org>
 * 		Ported to netfilter 'newnat' API
 */

#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <asm/param.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/stddef.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/ip_conntrack_rpc.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/lockhelp.h>

MODULE_LICENSE("GPL");

DECLARE_RWLOCK(ipct_rpc_tcp_lock);
#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&ipct_rpc_tcp_lock)
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&ipct_rpc_tcp_lock)
#include <linux/netfilter_ipv4/listhelp.h>

 /* For future conections RPC, using client's cache bindings */

LIST_HEAD(expect_rpc_list_tcp);
LIST_HEAD(request_p_list_tcp);

void delete_request_p(unsigned long request_p_ul) {
	struct request_p *p = (void *)request_p_ul;
	
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	LIST_DELETE(&request_p_list_tcp, p);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	kfree(p);
	return;

}

void delete_binding(unsigned long rpc_binding_ul) {
	struct expect_rpc *l = (void *)rpc_binding_ul;

	WRITE_LOCK(&ipct_rpc_tcp_lock);
	LIST_DELETE(&expect_rpc_list_tcp, l);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	kfree(l);
	return;
}

void exp_cl(struct expect_rpc * t) {
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	del_timer(&t->timeout);
	LIST_DELETE(&expect_rpc_list_tcp, t);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	kfree(t);
	return;
}


/* Clean timers kernel list in unregister to deny crash */
void clean_expect(struct list_head *list){
	struct list_head *first = list->prev;
	struct list_head *temp = list->next;
	struct list_head *aux;

	if(list_empty(list)) return;

	    while(first != temp) {
		aux = temp->next;
		exp_cl((struct expect_rpc *)temp);
		temp = aux;	
	    }
	exp_cl((struct expect_rpc *)temp);
	return;	
}

void req_cl(struct request_p * r){
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	del_timer(&r->timeout);
	LIST_DELETE(&request_p_list_tcp, r);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	kfree(r);
	return;
}

/* Clean timers kernel list in unregister to deny crash */
void clean_request(struct list_head *list){
	struct list_head *first = list->prev;
	struct list_head *temp = list->next;
	struct list_head *aux;

	if(list_empty(list)) return;

	    while(first != temp) {
		aux = temp->next;
		req_cl((struct request_p *)temp);
		temp = aux;	
	    }
	req_cl((struct request_p *)temp);
	return;
}


void alloc_request_p(u_int32_t xid, u_int16_t proto, u_int32_t ip,
		     u_int16_t port){
	struct request_p *req_p;
        
        /* Verifies if entry already exists */
        WRITE_LOCK(&ipct_rpc_tcp_lock);
        req_p = LIST_FIND(&request_p_list_tcp, request_p_cmp,
		struct request_p *, xid, ip, port);

        if(req_p){
		/* Refresh timeout */
		if (del_timer(&req_p->timeout)) {
		   req_p->timeout.expires = jiffies + EXP;
		   add_timer(&req_p->timeout);	
	        } 
		WRITE_UNLOCK(&ipct_rpc_tcp_lock);
		return;	

	}
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	
	/* Allocate new request_p */
	req_p = (struct request_p *) kmalloc(sizeof(struct request_p), GFP_ATOMIC);
        if(!req_p) {
 		DEBUGRPC("Can't allocate request_p\n");
		return;			
	}
	*req_p = ((struct request_p) {{ NULL, NULL }, xid, ip, port, proto, 
		{ { NULL, NULL }, jiffies + EXP, (unsigned long)req_p,
			NULL }}); 
      
        /*Initialize timer */
	init_timer(&req_p->timeout);
	req_p->timeout.function = delete_request_p;
        add_timer(&req_p->timeout); 

	/* Put in list */
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	list_prepend(&request_p_list_tcp, req_p);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock); 
	return; 

}

/* Update Internal expect list */
void binding(u_int32_t ip, u_int16_t port, u_int32_t ip_rmt, u_int16_t proto){
	struct expect_rpc *rpc_binding;

        /* Verifies if entry already exists */
        WRITE_LOCK(&ipct_rpc_tcp_lock);
        rpc_binding = LIST_FIND(&expect_rpc_list_tcp, expect_rpc_cmp,
		struct expect_rpc *, port, ip, ip_rmt, proto);

        if(rpc_binding){
		/* Refresh timeout */
		if (del_timer(&rpc_binding->timeout)) {
		   rpc_binding->timeout.expires = jiffies + EXPIRES;
		   add_timer(&rpc_binding->timeout);	
	        } 
		WRITE_UNLOCK(&ipct_rpc_tcp_lock);
		return;	

	}
	WRITE_UNLOCK(&ipct_rpc_tcp_lock);
	
	/* Allocate new binding */
	rpc_binding = (struct expect_rpc *) kmalloc(sizeof(struct expect_rpc), GFP_ATOMIC);
        if(!rpc_binding) {
 		DEBUGRPC("Can't allocate rpc_binding\n");
		return;			
	}
	*rpc_binding = ((struct expect_rpc) {{ NULL, NULL }, ip, port, ip_rmt, proto,
		{ { NULL, NULL }, jiffies + EXPIRES, (unsigned long)rpc_binding,
			NULL }}); 
      
        /*Initialize timer */
	init_timer(&rpc_binding->timeout);
	rpc_binding->timeout.function = delete_binding;
        add_timer(&rpc_binding->timeout); 

	/* Put in list */
	WRITE_LOCK(&ipct_rpc_tcp_lock);
	list_prepend(&expect_rpc_list_tcp, rpc_binding);
	WRITE_UNLOCK(&ipct_rpc_tcp_lock); 
	return; 

}

/* RPC using TCP - help */
static int help(const struct iphdr *iph, size_t len,
			 struct ip_conntrack *ct,
			 enum ip_conntrack_info ctinfo)
{
	struct tcphdr *tcph = (void *)iph + iph->ihl * 4;
	const u_int32_t *data = (const u_int32_t *)tcph + tcph->doff;

        /* Translstion's buffer for XDR */
        u_int16_t port_buf;

	size_t tcplen = len - iph->ihl * 4;

	/* Important for tracking */
	int dir = CTINFO2DIR(ctinfo);
	u_int32_t xid;
	struct request_p *req_p;

        /* This works for packets like handshake packets, ignore */
        if (len == ((tcph->doff + iph->ihl) * 4)) return NF_ACCEPT;

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED) {
		DEBUGRPC("RPC_TCP: Conntrackinfo = %u\n", ctinfo);
		return NF_ACCEPT;
	}

        /* Not whole TCP header? */
        if (tcplen < sizeof(struct tcphdr) || tcplen < tcph->doff*4) {
                DEBUGRPC("RPC: tcplen = %u\n", (unsigned)tcplen);
                return NF_ACCEPT;
        }

        /* Checksum invalid?  Ignore. */
        if (tcp_v4_check(tcph, tcplen, iph->saddr, iph->daddr,
                         csum_partial((char *)tcph, tcplen, 0))) {
                DEBUGRPC("RPC_help: bad csum: %p %u %u.%u.%u.%u %u.%u.%u.%u\n",
                         tcph, tcplen, IP_PARTS(iph->saddr),
                         IP_PARTS(iph->daddr));
                return NF_ACCEPT;
        }


        /* If packet is client packet */
        if(!dir){

		/* Tests if packet len is ok */
                if((tcplen - (tcph->doff * 4)) != 60)
                        return NF_ACCEPT;

	        /* Jump RPC Packet length in XDR */
		data++;

		/* Get XID */
		xid = *data;

		/* Jump until procedure number */
		data = data + 5;
		/* Is one get port procedure? */
		if(IXDR_GET_INT32(data) == 3){ 
		   data++;

		   /* Jump Credentials and Verfifier */
		   data = data + IXDR_GET_INT32(data) + 2;
		   data = data + IXDR_GET_INT32(data) + 2;

		   /* Get and store protocol in RPC client parameters */
		   data = data + 2;
		   alloc_request_p(xid, IXDR_GET_INT32(data), ct->tuplehash[dir].tuple.src.ip,
				ct->tuplehash[dir].tuple.src.u.all);
		}
		return NF_ACCEPT;
	}

	/* Tests if packet len is ok */
	if((tcplen - (tcph->doff * 4)) != 32)
		return NF_ACCEPT;

        /* Jump RPC Packet length in XDR */
	data++;

	/* XID is important */
        req_p = LIST_FIND(&request_p_list_tcp, request_p_cmp,
		struct request_p *, *data, ct->tuplehash[!dir].tuple.src.ip,
		ct->tuplehash[!dir].tuple.src.u.all);

	/* Drop packets for accurate tracking */
	if(!req_p) return NF_DROP;
        data = data++;

        /* Verifies if packet is really a reply packet */
	if(IXDR_GET_INT32(data) != 1) {
		DEBUGRPC("RPC_TCP: Packet with problems");
		return NF_ACCEPT;
        }
	data++;

       	/* Is status accept? */
        if(IXDR_GET_INT32(data)) return NF_ACCEPT;
        data++;

       /* Get Verifier lentgth. Jump verifier */
       	data = data + IXDR_GET_INT32(data) + 2;

       	/* Is accpet status "sucess"? */
       	if(IXDR_GET_INT32(data)) return NF_ACCEPT;
     	data++;

       	/* Get server port number */          
	port_buf = (u_int16_t) IXDR_GET_INT32(data);

        /* All right. This binding must be in internal expect list */
        /* If port == 0, service is not registred */
        if(port_buf)
         binding(ct->tuplehash[dir].tuple.src.ip, port_buf,
	       ct->tuplehash[!dir].tuple.src.ip, req_p->proto); 
	req_cl(req_p);
        return NF_ACCEPT;
}

struct ip_conntrack_helper rpc_tcp = { { NULL, NULL },
				       "rpc-tcp",
				       0, THIS_MODULE, 1, 0,
				       { { 0, { __constant_htons(111) } },
					 { 0, { 0 }, IPPROTO_TCP } },
				       { { 0, { 0xFFFF } },
					 { 0, { 0 }, 0xFFFF } },
				       help };

int init_module(void)
{
	printk("ip_conntrack_rpc_tcp.o\n");
	return ip_conntrack_helper_register(&rpc_tcp);
}

void cleanup_module(void)
{
	clean_expect(&expect_rpc_list_tcp);
	clean_request(&request_p_list_tcp);
	ip_conntrack_helper_unregister(&rpc_tcp);
}

struct module *ip_conntrack_rpc_tcp = THIS_MODULE;
EXPORT_SYMBOL(expect_rpc_list_tcp);
EXPORT_SYMBOL(ip_conntrack_rpc_tcp);
EXPORT_SYMBOL(ipct_rpc_tcp_lock);
