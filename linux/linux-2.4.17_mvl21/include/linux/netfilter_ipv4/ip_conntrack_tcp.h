#ifndef _IP_CONNTRACK_TCP_H
#define _IP_CONNTRACK_TCP_H
/* TCP tracking. */

#ifndef __KERNEL__
#error Only in kernel.
#endif

enum tcp_conntrack {
	TCP_CONNTRACK_NONE,
	TCP_CONNTRACK_SYN_SENT,
	TCP_CONNTRACK_SYN_RECV,
	TCP_CONNTRACK_ESTABLISHED,
	TCP_CONNTRACK_FIN_WAIT,
	TCP_CONNTRACK_CLOSE_WAIT,
	TCP_CONNTRACK_LAST_ACK,
	TCP_CONNTRACK_TIME_WAIT,
	TCP_CONNTRACK_CLOSE,
	TCP_CONNTRACK_LISTEN,
	TCP_CONNTRACK_MAX
};

struct ip_ct_tcp_state {
	u_int32_t	td_end;		/* max of seq + len */
	u_int32_t	td_maxend;	/* max of ack + max(win, 1) */
	u_int32_t	td_maxwin;	/* max(win) */
	u_int8_t	td_scale;	/* window scale factor */
	u_int8_t	loose;		/* used when connection picked up from the middle */
};

struct ip_ct_tcp
{
	enum tcp_conntrack state;	/* state of the connection */
	struct ip_ct_tcp_state seen[2];	/* connection parameters per direction */
	/* For detecting aborted connections */
	enum ip_conntrack_dir last_dir;	/* Direction of the last packet */
	u_int32_t	last_seq;	/* Last sequence number seen in dir */
	size_t		last_end;	/* Last seq + len */
	u_int8_t	retrans;	/* Number of retransmitted packets */
};

/* For NAT, when it mangles the packet */
extern void ip_conntrack_tcp_update(struct ip_conntrack *conntrack, int dir,   
                                    struct iphdr *iph, size_t newlen,
                                    struct tcphdr *tcph);


#endif /* _IP_CONNTRACK_TCP_H */
