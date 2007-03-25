/*
 * TCP connection tracking
 */

/*
 * Changes:
 *	Jozsef Kadlecsik:	Real stateful connection tracking
 *				Modified state transitions table
 *				Window scaling support
 */

#define __NO_VERSION__
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/netfilter.h>
#include <linux/module.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/spinlock.h>

#include <net/checksum.h>
#include <net/tcp.h>

#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_protocol.h>
#include <linux/netfilter_ipv4/lockhelp.h>

#if 0
#define DEBUGP printk
#define DDEBUGP
#else
#define DEBUGP(format, args...)
#endif

/* Protects conntrack->proto.tcp */
static DECLARE_RWLOCK(tcp_lock);

/* Logging options */ 
int ip_ct_tcp_log_invalid_scale = 1;
int ip_ct_tcp_log_out_of_window = 1;

/* "Be conservative in what you do, 
    be liberal in what you accept from others." */
int ip_ct_tcp_be_liberal = 0;

/* When connection is picked up from the middle, how many packets are required
   to pass in each direction when we assume we are in sync - if any side uses
   window scaling, we lost the game. 
   If it is set to zero, we disable picking up already esteblished connections. */
int ip_ct_tcp_loose = 3;

/* Max number of the retransmitted packets without receiving an (acceptable) 
   ACK from the destination. If this number is reached, a shorter timer 
   will be started. */
int ip_ct_tcp_max_retrans = 3;

/* FIXME: Examine ipfilter's timeouts and conntrack transitions more
   closely.  They're more complex. --RR */

/* Actually, I believe that neither ipmasq (where this code is stolen
   from) nor ipfilter do it exactly right.  A new conntrack machine taking
   into account packet loss (which creates uncertainty as to exactly
   the conntrack of the connection) is required.  RSN.  --RR */

static const char *tcp_conntrack_names[] = {
	"NONE",
	"SYN_SENT",
	"SYN_RECV",
	"ESTABLISHED",
	"FIN_WAIT",
	"CLOSE_WAIT",
	"LAST_ACK",
	"TIME_WAIT",
	"CLOSE",
	"LISTEN"
};

#define SECS * HZ
#define MINS * 60 SECS
#define HOURS * 60 MINS
#define DAYS * 24 HOURS

unsigned long ip_ct_tcp_timeout_syn_sent =	 2 MINS;
unsigned long ip_ct_tcp_timeout_syn_recv =	60 SECS;
unsigned long ip_ct_tcp_timeout_established =	 5 DAYS;
unsigned long ip_ct_tcp_timeout_fin_wait =	 2 MINS;
unsigned long ip_ct_tcp_timeout_close_wait =	 3 DAYS;
unsigned long ip_ct_tcp_timeout_last_ack =	30 SECS;
unsigned long ip_ct_tcp_timeout_time_wait =	 2 MINS;
unsigned long ip_ct_tcp_timeout_close =		10 SECS;

/* RFC1122 says the R2 limit should be at least 100 seconds.
   Linux uses 15 packets as limit, which corresponds 
   to ~13-30min depending on RTO. */
unsigned long ip_ct_tcp_timeout_max_retrans =     5 MINS;

static unsigned long * tcp_timeouts[]
= { 0,					/*	TCP_CONNTRACK_NONE */
    &ip_ct_tcp_timeout_syn_sent,	/*	TCP_CONNTRACK_SYN_SENT,	*/
    &ip_ct_tcp_timeout_syn_recv,	/*	TCP_CONNTRACK_SYN_RECV,	*/
    &ip_ct_tcp_timeout_established,	/*	TCP_CONNTRACK_ESTABLISHED,	*/
    &ip_ct_tcp_timeout_fin_wait,	/*	TCP_CONNTRACK_FIN_WAIT,	*/
    &ip_ct_tcp_timeout_close_wait,	/*	TCP_CONNTRACK_CLOSE_WAIT,	*/
    &ip_ct_tcp_timeout_last_ack,	/*	TCP_CONNTRACK_LAST_ACK,	*/
    &ip_ct_tcp_timeout_time_wait,	/*	TCP_CONNTRACK_TIME_WAIT,	*/
    &ip_ct_tcp_timeout_close,		/*	TCP_CONNTRACK_CLOSE,	*/
    0,					/*	TCP_CONNTRACK_LISTEN */
};

#define sNO TCP_CONNTRACK_NONE
#define sSS TCP_CONNTRACK_SYN_SENT
#define sSR TCP_CONNTRACK_SYN_RECV
#define sES TCP_CONNTRACK_ESTABLISHED
#define sFW TCP_CONNTRACK_FIN_WAIT
#define sCW TCP_CONNTRACK_CLOSE_WAIT
#define sLA TCP_CONNTRACK_LAST_ACK
#define sTW TCP_CONNTRACK_TIME_WAIT
#define sCL TCP_CONNTRACK_CLOSE
#define sLI TCP_CONNTRACK_LISTEN
#define sIV TCP_CONNTRACK_MAX

/*
 * The TCP state transition table needs a few words...
 *
 * We are the man in the middle. All the packets go through us
 * but might get lost in transit to the destination.
 * It is assumed that the destinations can't receive segments 
 * we haven't seen.
 *
 * The checked segment is in window.
 *
 * The meaning of the states are:
 *
 * NONE:	initial state
 * SYN_SENT:	SYN-only packet seen 
 * SYN_RECV:	SYN-ACK packet seen
 * ESTABLISHED:	ACK packet seen
 * FIN_WAIT:	FIN packet seen
 * CLOSE_WAIT:	ACK seen (after FIN) 
 * LAST_ACK:	FIN seen (after FIN)
 * TIME_WAIT:	last ACK seen
 * CLOSE:	closed connection
 *
 * LISTEN state is not used.
 *
 */
static enum tcp_conntrack tcp_conntracks[2][6][TCP_CONNTRACK_MAX] = {
	{
/* ORIGINAL */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*syn*/	   { sSS, sSS, sCL, sCL, sCL, sCL, sCL, sSS, sSS, sIV },
/*
 *	sNO -> sSS	Initialize a new connection
 *	sSS -> sSS	Retransmitted SYN
 *	sSR -> sCL	Error: SYNs in window outside the SYN_SENT state 
 *			are errors. Receiver will either go back to the 
 *			LISTEN state or reply with RST.
 *	sES -> sCL
 *	sFW -> sCL
 *	sCW -> sCL
 *	sLA -> sCL
 *	sTW -> sSS	Reopened connection (RFC 1122).
 *	sCL -> sSS
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*synack*/ { sSR, sSR, sES, sCL, sCL, sCL, sCL, sCL, sCL, sIV },
/*
 *	sNO -> sSR	Assumed: hey, we've just started up!
 *	sSS -> sSR	Simultaneous open.
 *	sSR -> sES	Ditto.
 *	sES -> sCL	Error.
 *	sFW -> sCL
 *	sCW -> sCL
 *	sLA -> sCL
 *	sTW -> sCL
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*fin*/    { sTW, sIV, sFW, sFW, sLA, sLA, sLA, sTW, sCL, sIV },
/*
 *	sNO -> sTW	We assume TIME-WAIT state.
 *	sSS -> sIV	Client migth not send FIN in this state.
 *	sSR -> sFW	Close started.
 *	sES -> sFW	
 *	sFW -> sLA	FIN seen in both directions, waiting for
 *			the last ACK. 
 *			Migth be a retransmitted FIN as well...
 *	sCW -> sLA
 *	sLA -> sLA	Retransmitted FIN. Remain in the same state.
 *	sTW -> sTW
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*ack*/	   { sES, sIV, sES, sES, sCW, sCW, sTW, sTW, sCL, sIV },
/*
 *	sNO -> sES	Assumed.
 *	sSS -> sIV	ACK is invalid: we haven't seen a SYN/ACK yet.
 *	sSR -> sES	Established state is reached.
 *	sES -> sES	:-)
 *	sFW -> sCW	Normal close request answered by ACK.
 *	sCW -> sCW
 *	sLA -> sTW	Last ACK detected.
 *	sTW -> sTW	Retransmitted last ACK. Remain in the same state.
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*rst*/    { sCL, sCL, sCL, sCL, sCL, sCL, sCL, sCL, sCL, sIV },
/*none*/   { sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV }
	},
	{
/* REPLY */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*syn*/	   { sIV, sSS, sSR, sCL, sCL, sCL, sCL, sSS, sSS, sIV },
/*
 *	sNO -> sIV	Never reached.
 *	sSS -> sSS	Simultaneous open.
 *	sSR -> sSR	Simultaneous open, retransmitted SYN.
 *			We have seen a SYN/ACK, but it seems 
 *			it is delayed or got lost.
 *	sES -> sCL	Error.
 *	sFW -> sCL
 *	sCW -> sCL
 *	sLA -> sCL
 *	sTW -> sSS	Reopened connection.
 *	sCL -> sSS
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*synack*/ { sIV, sSR, sES, sCL, sCL, sCL, sCL, sCL, sCL, sIV },
/*
 *	sSS -> sSR	Standard open.
 *	sSR -> sES	Simultaneous open.
 *	sES -> sCL	Error.
 *	sFW -> sCL
 *	sCW -> sCL
 *	sLA -> sCL
 *	sTW -> sCL
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*fin*/    { sIV, sIV, sFW, sFW, sLA, sLA, sLA, sTW, sCL, sIV },
/*
 *	sSS -> sIV	Server might not send FIN in this state.
 *	sSR -> sFW	Close started.
 *	sES -> sFW
 *	sFW -> sLA	FIN seen in both directions.
 *	sCW -> sLA
 *	sLA -> sLA	Retransmitted FIN.
 *	sTW -> sTW
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*ack*/	   { sIV, sIV, sES, sES, sCW, sCW, sTW, sTW, sCL, sIV },
/*
 *	sSS -> sIV	ACK is invalid: we haven't seen a SYN/ACK yet.
 *	sSR -> sES	Simultaneous open.
 *	sES -> sES	:-)
 *	sFW -> sCW	Normal close request answered by ACK.
 *	sCW -> sCW
 *	sLA -> sTW	Last ACK detected.
 *	sTW -> sTW	Retransmitted last ACK.
 *	sCL -> sCL
 */
/* 	     sNO, sSS, sSR, sES, sFW, sCW, sLA, sTW, sCL, sLI	*/
/*rst*/    { sIV, sCL, sCL, sCL, sCL, sIV, sCL, sCL, sCL, sIV },
/*none*/   { sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV, sIV }
	}
};

static int tcp_pkt_to_tuple(const void *datah, size_t datalen,
			    struct ip_conntrack_tuple *tuple)
{
	const struct tcphdr *hdr = datah;

	tuple->src.u.tcp.port = hdr->source;
	tuple->dst.u.tcp.port = hdr->dest;

	return 1;
}

static int tcp_invert_tuple(struct ip_conntrack_tuple *tuple,
			    const struct ip_conntrack_tuple *orig)
{
	tuple->src.u.tcp.port = orig->dst.u.tcp.port;
	tuple->dst.u.tcp.port = orig->src.u.tcp.port;
	return 1;
}

/* Print out the per-protocol part of the tuple. */
static unsigned int tcp_print_tuple(char *buffer,
				    const struct ip_conntrack_tuple *tuple)
{
	return sprintf(buffer, "sport=%hu dport=%hu ",
		       ntohs(tuple->src.u.tcp.port),
		       ntohs(tuple->dst.u.tcp.port));
}

/* Print out the private part of the conntrack. */
static unsigned int tcp_print_conntrack(char *buffer,
					const struct ip_conntrack *conntrack)
{
	enum tcp_conntrack state;

	READ_LOCK(&tcp_lock);
	state = conntrack->proto.tcp.state;
	READ_UNLOCK(&tcp_lock);

	return sprintf(buffer, "%s ", tcp_conntrack_names[state]);
}

static unsigned int get_conntrack_index(const struct tcphdr *tcph)
{
	if (tcph->rst) return 4;
	else if (tcph->syn) return (tcph->ack ? 1 : 0);
	else if (tcph->fin) return 2;
	else if (tcph->ack) return 3;
	else return 5;
}

/* From ipt_LOG.c... */
/* Use lock to serialize, so printks don't overlap */
static spinlock_t log_lock = SPIN_LOCK_UNLOCKED;

static void log_packet(struct iphdr *iph, struct tcphdr *tcph)
{
	/* Important fields:
	 * TOS, len, DF/MF, fragment offset, TTL, src, dst, options. */
	/* Max length: 40 "SRC=255.255.255.255 DST=255.255.255.255 " */
	printk("SRC=%u.%u.%u.%u DST=%u.%u.%u.%u ",
	       NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));

	/* Max length: 46 "LEN=65535 TOS=0xFF PREC=0xFF TTL=255 ID=65535 " */
	printk("LEN=%u TOS=0x%02X PREC=0x%02X TTL=%u ID=%u ",
	       ntohs(iph->tot_len), iph->tos & IPTOS_TOS_MASK,
	       iph->tos & IPTOS_PREC_MASK, iph->ttl, ntohs(iph->id));

	/* Max length: 6 "CE DF MF " */
	if (ntohs(iph->frag_off) & IP_CE)
		printk("CE ");
	if (ntohs(iph->frag_off) & IP_DF)
		printk("DF ");
	/* ... but conntrack don't see fragments */

	if (iph->ihl * 4 != sizeof(struct iphdr)) {
		unsigned int i;

		/* Max length: 127 "OPT (" 15*4*2chars ") " */
		printk("OPT (");
		for (i = sizeof(struct iphdr); i < iph->ihl * 4; i++)
			printk("%02X", ((u_int8_t *)iph)[i]);
		printk(") ");
	}

	/* Max length: 10 "PROTO=TCP " */
	printk("PROTO=TCP ");

	/* Max length: 20 "SPT=65535 DPT=65535 " */
	printk("SPT=%u DPT=%u ",
	       ntohs(tcph->source), ntohs(tcph->dest));
	/* Max length: 30 "SEQ=4294967295 ACK=4294967295 " */
	printk("SEQ=%u ACK=%u ",
	       ntohl(tcph->seq), ntohl(tcph->ack_seq));
	/* Max length: 13 "WINDOW=65535 " */
	printk("WINDOW=%u ", ntohs(tcph->window));
	/* Max length: 9 "RES=0x3F " */
	printk("RES=0x%02x ", (u_int8_t)(ntohl(tcp_flag_word(tcph) & TCP_RESERVED_BITS) >> 22));
	/* Max length: 36 "URG ACK PSH RST SYN FIN " */
	if (tcph->urg)
		printk("URG ");
	if (tcph->ack)
		printk("ACK ");
	if (tcph->psh)
		printk("PSH ");
	if (tcph->rst)
		printk("RST ");
	if (tcph->syn)
		printk("SYN ");
	if (tcph->fin)
		printk("FIN ");
	/* Max length: 11 "URGP=65535 " */
	printk("URGP=%u ", ntohs(tcph->urg_ptr));

	if (tcph->doff * 4 != sizeof(struct tcphdr)) {
		unsigned int i;

		/* Max length: 127 "OPT (" 15*4*2chars ") " */
		printk("OPT (");
		for (i =sizeof(struct tcphdr); i < tcph->doff * 4; i++)
			printk("%02X", ((u_int8_t *)tcph)[i]);
		printk(") ");
	}

	/* Proto    Max log string length */
	/* IP:      40+46+6+11+127 = 230 */
	/* TCP:     10+max(25,20+30+13+9+36+11+127) = 256 */
}

#define log_invalid_packet(iph, tcph, format, arg...)

/* TCP connection tracking based on 'Real Stateful TCP Packet Filtering
   in IP Filter' by Guido van Rooij.
   
   http://www.nluug.nl/events/sane2000/papers.html
   http://www.iae.nl/users/guido/papers/tcp_filtering.ps.gz
   
   The boundaries according to the article:
   
   	td_maxend = max(ack + max(win,1)) seen in reply packets
	td_maxwin = max(max(win, 1)) seen in sent packets
	td_end    = max(seq + len) seen in sent packets
   
   I. 	Upper bound for valid data:	seq + len <= sender.td_maxend
   II. 	Lower bound for valid data:	seq >= sender.td_end - receiver.td_maxwin
   III.	Upper bound for valid ack:	ack <= receiver.td_end
   IV.	Lower bound for valid ack:	ack >= receiver.td_end - MAXACKWINDOW
   	
   The upper bound limit for a valid ack is not ignored - 
   we doesn't have to deal with fragments. 
*/

#define SEGMENT_SEQ_PLUS_LEN(seq, len, iph, tcph)	(seq + len - (iph->ihl + tcph->doff)*4 \
							 + (tcph->syn ? 1 : 0) + (tcph->fin ? 1 : 0))

/* Fixme: what about big packets? */
#define MAXACKWINCONST			66000
#define MAXACKWINDOW(sender)		((sender)->td_maxwin > MAXACKWINCONST ? (sender)->td_maxwin : MAXACKWINCONST)

/*
 * Simplified tcp_parse_options routine from tcp_input.c
 */
static int8_t tcp_scale_option(struct iphdr *iph, struct tcphdr *tcph)
{
	unsigned char *ptr;
	int length = (tcph->doff*4) - sizeof(struct tcphdr);
	
	ptr = (unsigned char *)(tcph + 1);
	
	while (length > 0) {
		int opcode=*ptr++;
		int opsize;
		
		switch (opcode) {
			case TCPOPT_EOL:
				return 0;
			case TCPOPT_NOP:	/* Ref: RFC 793 section 3.1 */
				length--;
				continue;
			default:
				opsize=*ptr++;
				if (opsize < 2) /* "silly options" */
					return 0;
				if (opsize > length)
					break;	/* don't parse partial options */
					
				if (opcode == TCPOPT_WINDOW && opsize == TCPOLEN_WINDOW) {
					u_int8_t scale = *(u_int8_t *)ptr;
					
					if (scale > 14) {
						/* See RFC1323 for an explanation of the limit to 14 */
						if (ip_ct_tcp_log_invalid_scale && net_ratelimit())
							log_invalid_packet(iph, tcph, "Illegal window scaling value %u > 14 ignored\n",
									   scale);
						scale = 14;
					}
					return scale;
				}
				ptr += opsize - 2;
				length -= opsize;
		}
	}
	return -1;
}

static int tcp_in_window(struct ip_ct_tcp *state, 
                         enum ip_conntrack_dir dir,
                         struct iphdr *iph, size_t len,
                         struct tcphdr *tcph)
{
	struct ip_ct_tcp_state *sender = &state->seen[dir];
	struct ip_ct_tcp_state *receiver = &state->seen[!dir];
	__u32 seq, ack, end, swin;
	__u16 win;
	int res;
	

	/*
	 * Get the required data from the packet.
	 */
	seq = ntohl(tcph->seq);
	ack = ntohl(tcph->ack_seq);
	win = ntohs(tcph->window);
	end = SEGMENT_SEQ_PLUS_LEN(seq, len, iph, tcph);
	
	DEBUGP("tcp_in_window: src=%u.%u.%u.%u:%hu dst=%u.%u.%u.%u:%hu seq=%u ack=%u win=%u end=%u\n",
		NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest),
		seq, ack, win, end);
	DEBUGP("tcp_in_window: sender end=%u maxend=%u maxwin=%u scale=%i receiver end=%u maxend=%u maxwin=%u scale=%i\n",
		sender->td_end, sender->td_maxend, sender->td_maxwin, sender->td_scale, 
		receiver->td_end, receiver->td_maxend, receiver->td_maxwin, receiver->td_scale);
		
	if (sender->td_end == 0) {
		/*
		 * Initialize sender data.
		 */
		if (tcph->syn && tcph->ack) {
			/*
			 * Outgoing SYN-ACK in reply to a SYN.
			 *
			 * Fixme: supporting simultaneous open is lost...
			 */
			int8_t scale = tcp_scale_option(iph, tcph);
			
			sender->td_end = 
			sender->td_maxend = end;
			sender->td_maxwin = (win == 0 ? 1 : win);
			if (scale < 0)
				sender->td_scale =
				receiver->td_scale = 0;
			else
				sender->td_scale = scale;
		} else {
			/*
			 * We are in the middle of a connection,
			 * its history is lost for us.
			 * Let's try to use the data from the packet.
		 	 */
			sender->td_end = end;
			sender->td_maxwin = (win == 0 ? 1 : win);
			sender->td_maxend = end + sender->td_maxwin;
			sender->td_scale = 0;
			sender->loose = ip_ct_tcp_loose;
		}
	}
	
	if (!(tcph->ack)) {
		/*
		 * If there is no ACK, just pretend it was set and OK.
		 */
		ack = receiver->td_end;
	} else if (((tcp_flag_word(tcph) & (TCP_FLAG_ACK|TCP_FLAG_RST)) == (TCP_FLAG_ACK|TCP_FLAG_RST)) 
		   && (ack == 0)) {
		/*
		 * Broken TCP stacks, that set ACK in RST packets as well
		 * with zero ack value.
		 */
		ack = receiver->td_end;
	}
	
	if (seq == end)
		/*
		 * Packets contains no data: we assume it is valid
		 * and check the ack value only.
		 */
		seq = end = sender->td_end;
		
	if (sender->loose)
		sender->loose--;

	DEBUGP("tcp_in_window: sender end=%u maxend=%u maxwin=%u scale=%i receiver end=%u maxend=%u maxwin=%u scale=%i\n",
		sender->td_end, sender->td_maxend, sender->td_maxwin, sender->td_scale, 
		receiver->td_end, receiver->td_maxend, receiver->td_maxwin, receiver->td_scale);
	DEBUGP("tcp_in_window: src=%u.%u.%u.%u:%hu dst=%u.%u.%u.%u:%hu seq=%u ack=%u win=%u end=%u\n",
		NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest),
		seq, ack, win, end);
	DEBUGP("tcp_in_window: I=%i II=%i III=%i IV=%i\n",
		before(end, sender->td_maxend + 1),
	    	after(seq, sender->td_end - receiver->td_maxwin - 1),
	    	before(ack, receiver->td_end + 1),
	    	after(ack, receiver->td_end - MAXACKWINDOW(sender)));
		
	if (sender->loose || receiver->loose ||
	    (before(end, sender->td_maxend + 1) &&
	     after(seq, sender->td_end - receiver->td_maxwin - 1) &&
	     before(ack, receiver->td_end + 1) &&
	     after(ack, receiver->td_end - MAXACKWINDOW(sender)))) {
	    	/*
		 * Take into account window scaling (RFC 1323).
		 */

		swin = win;
		
		if (!tcph->syn)
			swin <<= sender->td_scale;
		/*
		 * Update sender data.
		 */
		if (sender->td_maxwin < swin)
			sender->td_maxwin = swin;
		if (after(end, sender->td_end))
			sender->td_end = end;
		if (after(ack + swin, receiver->td_maxend - 1)) {
			receiver->td_maxend = ack + swin;
			if (win == 0)
				receiver->td_maxend++;
		}

		/* Check retransmissions */
		if (state->last_dir == dir
		    && state->last_seq == seq
		    && state->last_end == end)
			state->retrans++;
		else {
			state->last_dir = dir;
			state->last_seq = seq;
			state->last_end = end;
			state->retrans = 0;
		}
		
		res = 1;
	} else {
		if (ip_ct_tcp_log_out_of_window && net_ratelimit())
			log_invalid_packet(iph, tcph, "ip_conntrack_tcp: INVALID: Out of window data; %s\n",
				before(end, sender->td_maxend + 1) ?
					after(seq, sender->td_end - receiver->td_maxwin - 1) ?
						before(ack, receiver->td_end + 1) ?
							after(ack, receiver->td_end - MAXACKWINDOW(sender)) ? "BUG"
							: "ACK is under the lower bound (possibly overly delayed ACK)"
						: "ACK is over the upper bound (ACKed data has never seen yet)"
					: "SEQ is under the lower bound (retransmitted already ACKed data)"
				: "SEQ is over the upper bound (over the window of the receiver)");
		res = ip_ct_tcp_be_liberal && !tcph->rst;
	}

	DEBUGP("tcp_in_window: res=%i sender end=%u maxend=%u maxwin=%u receiver end=%u maxend=%u maxwin=%u\n",
		res, sender->td_end, sender->td_maxend, sender->td_maxwin, 
		receiver->td_end, receiver->td_maxend, receiver->td_maxwin);

	return res;
}

/* Update sender->td_end after NAT successfully mangled the packet */
void ip_conntrack_tcp_update(struct ip_conntrack *conntrack, int dir,
                             struct iphdr *iph, size_t newlen,
                             struct tcphdr *tcph)
{
	__u32 end;
#ifdef DDEBUGP
	struct ip_ct_tcp_state *sender = &conntrack->proto.tcp.seen[dir];
	struct ip_ct_tcp_state *receiver = &conntrack->proto.tcp.seen[!dir];
#endif

	end = SEGMENT_SEQ_PLUS_LEN(ntohl(tcph->seq), newlen, iph, tcph);
	
	WRITE_LOCK(&tcp_lock);
	/*
	 * We have to worry for the ack in the reply packet only...
	 */
	if (after(end, conntrack->proto.tcp.seen[dir].td_end))
		conntrack->proto.tcp.seen[dir].td_end = end;
	WRITE_UNLOCK(&tcp_lock);
	DEBUGP("tcp_update: sender end=%u maxend=%u maxwin=%u scale=%i receiver end=%u maxend=%u maxwin=%u scale=%i\n",
		sender->td_end, sender->td_maxend, sender->td_maxwin, sender->td_scale, 
		receiver->td_end, receiver->td_maxend, receiver->td_maxwin, receiver->td_scale);
}

#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define	TH_ECE	0x40
#define	TH_CWR	0x80

/* Protect conntrack agaist unclean packets. Code taken from ipt_unclean.c.  */
static int unclean(struct iphdr *iph, size_t len, int new)
{
	struct tcphdr *tcph = (struct tcphdr *)((u_int32_t *)iph + iph->ihl);
	unsigned int tcplen = len - iph->ihl * 4;
	u_int8_t tcpflags;

	/* Not whole TCP header? */
	if (tcplen < sizeof(struct tcphdr) || tcplen < tcph->doff*4) {
		if (ip_ct_tcp_log_out_of_window && net_ratelimit())
			log_invalid_packet(iph, tcph, "ip_conntrack_tcp: INVALID: truncated packet.\n");
		return 1;
	}


	/* Checksum invalid?  Ignore. */
	/* FIXME: Source route IP option packets --RR */
	if (tcp_v4_check(tcph, tcplen, iph->saddr, iph->daddr,
			 csum_partial((char *)tcph, tcplen, 0))) {
		if (ip_ct_tcp_log_out_of_window && net_ratelimit())
			log_invalid_packet(iph, tcph, "ip_conntrack_tcp: INVALID: bad TCP checksum.\n");
		return 1;
	}

	/* CHECK: TCP flags. */
	tcpflags = (((u_int8_t *)tcph)[13] & ~(TH_ECE|TH_CWR));

	if (new)
	{
	   if (tcpflags != TH_SYN)
	   {
		if (ip_ct_tcp_log_out_of_window && net_ratelimit())
			log_invalid_packet(iph, tcph, "ip_conntrack_tcp: INVALID: invalid TCP flag combination.\n");
		return 1;
	   }
	}

	if (tcpflags != TH_SYN
	    && tcpflags != (TH_SYN|TH_ACK)
	    && tcpflags != TH_RST
	    && tcpflags != (TH_RST|TH_ACK)
	    && tcpflags != (TH_RST|TH_ACK|TH_PUSH)
	    && tcpflags != (TH_FIN|TH_ACK)
	    && tcpflags != TH_ACK
	    && tcpflags != (TH_ACK|TH_PUSH)
	    && tcpflags != (TH_ACK|TH_URG)
	    && tcpflags != (TH_ACK|TH_URG|TH_PUSH)
	    && tcpflags != (TH_FIN|TH_ACK|TH_PUSH)
	    && tcpflags != (TH_FIN|TH_ACK|TH_URG)
	    && tcpflags != (TH_FIN|TH_ACK|TH_URG|TH_PUSH)) {
		if (ip_ct_tcp_log_out_of_window && net_ratelimit())
			log_invalid_packet(iph, tcph, "ip_conntrack_tcp: INVALID: invalid TCP flag combination.\n");
		return 1;
	}


	return 0;
}

/* Returns verdict for packet, or -1 for invalid. */
static int tcp_packet(struct ip_conntrack *conntrack,
		      struct iphdr *iph, size_t len,
		      enum ip_conntrack_info ctinfo)
{
	enum tcp_conntrack new_state, old_state;
	enum ip_conntrack_dir dir;
	struct tcphdr *tcph = (struct tcphdr *)((u_int32_t *)iph + iph->ihl);


	/* Do not handle unclean packets, which could cause false
	   alarms from window tracking point of view */
	if (unclean(iph, len,0))
		return -1;	

	WRITE_LOCK(&tcp_lock);
	old_state = conntrack->proto.tcp.state;
	dir = CTINFO2DIR(ctinfo);
	
	new_state
		= tcp_conntracks
		[dir]
		[get_conntrack_index(tcph)][old_state];

	if (new_state == TCP_CONNTRACK_SYN_SENT
	    && old_state >= TCP_CONNTRACK_TIME_WAIT) {
		/* Attempt to reopen a closed connection.
		 * Delete this connection and look up again. */
		WRITE_UNLOCK(&tcp_lock);
		if (del_timer(&conntrack->timeout))
			conntrack->timeout.function((unsigned long)conntrack);
		return NF_REPEAT;
	} else if (!(new_state == TCP_CONNTRACK_MAX
	           || tcp_in_window(&conntrack->proto.tcp,
				    dir, iph, len, tcph)))
		new_state = TCP_CONNTRACK_MAX;

	DEBUGP("tcp_conntracks: src=%u.%u.%u.%u:%hu dst=%u.%u.%u.%u:%hu syn=%i ack=%i fin=%i rst=%i old=%i new=%i\n",
		NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest),
		(tcph->syn ? 1 : 0), (tcph->ack ? 1 : 0), (tcph->fin ? 1 : 0), (tcph->rst ? 1 : 0),
		old_state, new_state);

	/* Invalid */
	if (new_state == TCP_CONNTRACK_MAX) {
		DEBUGP("ip_conntrack_tcp: Invalid dir=%i index=%u conntrack=%u\n",
		       dir, get_conntrack_index(tcph),
		       old_state);
		WRITE_UNLOCK(&tcp_lock);
		return -1;
	}

	conntrack->proto.tcp.state = new_state;
	WRITE_UNLOCK(&tcp_lock);

	if (!(conntrack->status & IPS_SEEN_REPLY)) {
		/* If only reply is a RST, we can consider ourselves not to
		   have an established connection: this is a fairly common
		   problem case, so we can delete the conntrack
		   immediately.  --RR */
		if (tcph->rst) {
			if (del_timer(&conntrack->timeout))
				conntrack->timeout.function((unsigned long)conntrack);
				
			return NF_ACCEPT;
		} else {
			/* Set ASSURED if we see see valid ack in ESTABLISHED after SYN_RECV 
			 or a valid answer for an picked up connection */
			if ((old_state == TCP_CONNTRACK_SYN_RECV
			     || old_state == TCP_CONNTRACK_ESTABLISHED)
			    && new_state == TCP_CONNTRACK_ESTABLISHED)
				set_bit(IPS_ASSURED_BIT, &conntrack->status);
		}
	}
	ip_ct_refresh(conntrack, 
		conntrack->proto.tcp.retrans >= ip_ct_tcp_max_retrans
		&& *tcp_timeouts[new_state] > ip_ct_tcp_timeout_max_retrans ?
		ip_ct_tcp_timeout_max_retrans : *tcp_timeouts[new_state]);

	return NF_ACCEPT;
}

/* Called when a new connection for this protocol found. */
static int tcp_new(struct ip_conntrack *conntrack,
		   struct iphdr *iph, size_t len)
{
	enum tcp_conntrack new_state;
	struct tcphdr *tcph = (struct tcphdr *)((u_int32_t *)iph + iph->ihl);
#ifdef DDEBUGP
	struct ip_ct_tcp_state *sender = &conntrack->proto.tcp.seen[0];
	struct ip_ct_tcp_state *receiver = &conntrack->proto.tcp.seen[1];
#endif


	/* Skip unclean packets */
	if (unclean(iph, len,1))
		return NF_DROP;

	/* Don't need lock here: this conntrack not in circulation yet */
	new_state
		= tcp_conntracks[0][get_conntrack_index(tcph)]
		[TCP_CONNTRACK_NONE];

	/* Invalid: delete conntrack */
	if (new_state == TCP_CONNTRACK_MAX) {
		DEBUGP("ip_conntrack_tcp: invalid new deleting.\n");
		return 0;
	}

	if (new_state == TCP_CONNTRACK_SYN_SENT) {
		int8_t scale = tcp_scale_option(iph, tcph);
		

		conntrack->proto.tcp.seen[0].td_end =
			SEGMENT_SEQ_PLUS_LEN(ntohl(tcph->seq), len, iph, tcph);
		conntrack->proto.tcp.seen[0].td_maxwin = ntohs(tcph->window);
		if (conntrack->proto.tcp.seen[0].td_maxwin == 0)
			conntrack->proto.tcp.seen[0].td_maxwin = 1;
		conntrack->proto.tcp.seen[0].td_maxend =
			conntrack->proto.tcp.seen[0].td_end;
		if (scale < 0)
			conntrack->proto.tcp.seen[0].td_scale = 0;
		else
			conntrack->proto.tcp.seen[0].td_scale = scale;
		conntrack->proto.tcp.seen[0].loose = 
		conntrack->proto.tcp.seen[1].loose = 0;
	} else {
		if (ip_ct_tcp_loose == 0)
			return 0;

		/*
		 * We are in the middle of a connection,
		 * its history is lost for us.
		 * Let's try to use the data from the packet.
		 */
		conntrack->proto.tcp.seen[0].td_end =
			SEGMENT_SEQ_PLUS_LEN(ntohl(tcph->seq), len, iph, tcph);
		conntrack->proto.tcp.seen[0].td_maxwin = ntohs(tcph->window);
		if (conntrack->proto.tcp.seen[0].td_maxwin == 0)
			conntrack->proto.tcp.seen[0].td_maxwin = 1;
		conntrack->proto.tcp.seen[0].td_maxend =
			conntrack->proto.tcp.seen[0].td_end + 
			conntrack->proto.tcp.seen[0].td_maxwin;
		conntrack->proto.tcp.seen[0].td_scale = 0;
		conntrack->proto.tcp.seen[0].loose = 
		conntrack->proto.tcp.seen[1].loose = ip_ct_tcp_loose;
	}
    
	conntrack->proto.tcp.seen[1].td_end = 0;
	conntrack->proto.tcp.seen[1].td_maxend = 0;
	conntrack->proto.tcp.seen[1].td_maxwin = 1;
	conntrack->proto.tcp.seen[1].td_scale = 0;      

	conntrack->proto.tcp.state = new_state;
 
	DEBUGP("tcp_in_window: new sender end=%u maxend=%u maxwin=%u scale=%i receiver end=%u maxend=%u maxwin=%u scale=%i\n",
		sender->td_end, sender->td_maxend, sender->td_maxwin, sender->td_scale, 
		receiver->td_end, receiver->td_maxend, receiver->td_maxwin, receiver->td_scale);
	return 1;
}

EXPORT_SYMBOL(ip_conntrack_tcp_update);

static int tcp_exp_matches_pkt(struct ip_conntrack_expect *exp,
			       struct sk_buff **pskb)
{
	struct iphdr *iph = (*pskb)->nh.iph;
	struct tcphdr *tcph = (struct tcphdr *)((u_int32_t *)iph + iph->ihl);
	unsigned int datalen;

	datalen = (*pskb)->len - iph->ihl*4 - tcph->doff*4;

	return between(exp->seq, ntohl(tcph->seq), ntohl(tcph->seq) + datalen);
}

struct ip_conntrack_protocol ip_conntrack_protocol_tcp
= { { NULL, NULL }, IPPROTO_TCP, "tcp",
    tcp_pkt_to_tuple, tcp_invert_tuple, tcp_print_tuple, tcp_print_conntrack,
    tcp_packet, tcp_new, NULL, tcp_exp_matches_pkt, NULL };
