/*--------------------------------------------------------------------------------
 * Copyright 2002-2004, Texas Instruments Incorporated
 *
 * This program has been added from its original operation by Texas Instruments
 * to do the following:
 *       net/sched/sch_priowrr.c : 4-band priority scheduler.
 *
 *   THIS MODIFIED SOFTWARE AND DOCUMENTATION ARE PROVIDED
 *   "AS IS," AND TEXAS INSTRUMENTS MAKES NO REPRESENTATIONS 
 *   OR WARRENTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
 *   TO, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY 
 *   PARTICULAR PURPOSE OR THAT THE USE OF THE SOFTWARE OR 
 *   DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS, 
 *   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS. 
 *   See The GNU General Public License for more details.
 *
 * These changes are covered under version 2 of the GNU General Public License, 
 *  dated June 1991.
 *------------------------------------------------------------------------------
 *
 * net/sched/sch_priowrr.c : 4-band priority scheduler;
 *
 * This scheduler mechanism has 4 queues - 
 * 		Two Expedited Forwarding (EF) queues : High Priority queues, with 
 * non-preemptable service (strict priority).
 * 		Two Best Effort (BE) queues : Medium and Low Prority queues, with 
 * Weighed Round Robin (WRR) service. Using the scheduling table based on
 * weight maps, the packets are serviced, sharing the remaining link bandwidth
 * after EF service.
 * 			
 * Enqueuing
 * 		We have a common pool of buffers for all the 4 queues. The number of 
 * buffers in the pool corresponds to "tx_queue_len". We need to allocate buffers
 * to each queue based on the weight map (or qlen map if configured). The qlen 
 * limit should be configured to prevent enqueuing of only high priority traffic.
 * Thus the qlen "limit" is
 * 		-Low for for EF queues, high for Medium Priority queue (for fast service).
 * 		-High for Medium Priority queue (to reduce the number of tail-drops)
 * 		-Average for Low Prority queue (Best Effort service)
 * 		
 * Queue Length map is also configurable, Q(EF1)<=Q(EF2)<=Q(M)<=Q(L)<=tx_queue_len; Also 
 * Sum(Q(EF1), Q(EF2), Q(M), Q(L))<=tx_queue_len.
 *
 * Dequeuing
 * 		The EF1 and EF2 (High priority) queues are serviced to extinction, no preemption. 
 * The Medium and Low priority queues are serviced (after EF queue) based on 
 * WRR Queue scheduler.
 * 		-Weight map is configurable, 1<=L<=M<=N<=10 where N=Sum(L, M); 
 * 		where L and M are weights for Low and Medium Priority queues.
 * 		-The scheduling table (ST) is precomputed based on the weight map in an 
 * 		interleaved fashion. This is an O(1) scheduler.
 *
 * Traffic Control and priowrr scheduler:-
 * 		The "tc" utility is used to enable this scheduler for nas/ppp interfaces, 
 * configure the scheduler and configure the filters for the scheduler.
 *
 * 1. Attaching this scheduler to nas/ppp interface:-
 * 	tc qdisc add dev ppp0 root handle 1: priowrr [weightmap wt1 wt2 wt3 wt4] [qlenmap qlen1 qlen2 qlen3 qlen4]
 * 		Where wt1 and wt2 are always 100, sum(wt3,wt4) <= 100.
 *		sum(qlen1, qlen2, qlen3, qlen4) <= tx_queue_len.
 *
 *	Default values: weightmap 100 100 70 30 qlenmap 15 20 30 35.
 *
 * 2. Attaching filters to this scheduler.
 * 	tc filter add dev nas0 parent 1: protocol ip u32 match ip src <ip> ..... classid 1:3
 * Note that classid corresponds to the queue - meaning 1:3 corresponds to 
 * Medium Priority queue (BEQ1).
 *------------------------------------------------------------------------------*/

#include <linux/config.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/if_ether.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/notifier.h>
#include <net/ip.h>
#include <net/route.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/pkt_sched.h>
#define PRIOWRR_DEBUG
#ifdef PRIOWRR_DEBUG
#include <linux/proc_fs.h>
#endif
#include <linux/atmdev.h>

/*
 * The mapping is as follows :
 * 
 * 		TOS				Linux Priority		Queue Priority Band
 * 	   -----			--------------		-------------------
 * Normal Service		0	Best Effort				2 
 * Maximize Throughput	4	Bulk					1
 * Minimize Delay		6	Interactive				0
 *
 * So what changed in this new mapping is just the mapping of "Best Effort"
 * packets into band #2 instead of 1.
 * Note that the commented line is the Linux default mappings.
 */
static const u8 prio2band[TC_PRIO_MAX+1] =
{ 3, 3, 2, 3, 2, 3, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 };
/*{ 2, 2, 1, 2, 1, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 };*/
/*{ 1, 2, 2, 2, 1, 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 };*/

/* Scheduling Table (ST) for WRR scheduler.
 * The Scheduling Table is precomputed for each service slot, that distributes
 * the packets from each of the two Best Effort (BE) queues (Medium and Low) 
 * proportional with the associated values of their weights (wt3, wt4) and 
 * uniformly interleaving them. */
static u8 sch_table[TCQ_MAX_ST];

struct priowrr_sched_data
{
	int bands;
	int wted_idx;
	struct tcf_proto *filter_list;	
	u8  weight2band[TCQ_PRIOWRR_BANDS];
	u8  qlen2band[TCQ_PRIOWRR_BANDS];
	struct Qdisc *queues[TCQ_PRIOWRR_BANDS];
};
#ifdef PRIOWRR_DEBUG
struct priowrr_stats_data
{
	int enqueue_stats;
	int dequeue_stats;
	int qlen, maxqlen;
	int drops, flushed;
};
struct priowrr_stats_data priowrr_stats[] = {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}};

static int priowrr_read_proc(char *buffer, char **start, off_t offset,
			     int length, int *eof, void *data)
{
	int len, i;

	len = sprintf(buffer, "Enqueue    Dequeue    Maxqlen    Drops    Flushed\n");
   	for (i=0; i<TCQ_PRIOWRR_BANDS; i++)
	{
		len += sprintf(buffer+len, "%7d    %7d    %7d    %5d    %7d\n", 
			priowrr_stats[i].enqueue_stats, priowrr_stats[i].dequeue_stats,
			priowrr_stats[i].maxqlen, priowrr_stats[i].drops, priowrr_stats[i].flushed);
	}
	len -= offset;

	if (len > length)
		len = length;
	if(len < 0)
		len = 0;

	*start = buffer + offset;
	*eof = 1;

	return len;
}
#endif

static __inline__ unsigned priowrr_classify(struct sk_buff *skb, struct Qdisc *sch)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	struct tcf_result res;
	u32 band;

	/* skb->priority will be set to 0xFFFF by setsockopt for IP_TOS with a value of "0xC0xx" */
	if (skb->priority == 0xFFFF)
		return PRIOWRR_EFQ1; /* EF1 */

	band = skb->priority;
	if (TC_H_MAJ(skb->priority) != sch->handle) {
		if (!q->filter_list || tc_classify(skb, q->filter_list, &res)) {
			if (TC_H_MAJ(band))
				band = 0;
			return prio2band[band&TC_PRIO_MAX];
		}
		band = res.classid;
	}
	band = TC_H_MIN(band) - 1;
	return band < q->bands ? band : prio2band[0];
}

static int priowrr_enqueue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	struct Qdisc *qdisc;
	int ret;
	u32 band;

	/* Set skb->cb[47] to 1 or 3 depending on the band number, so that the
	 * driver can shift skb->cb[47] by 1 to retrieve the queueid (0/1).
	 * Set skb->priority to band number, so that the qdisc below priowrr 
	 * can also enqueue accondingly. */
	band = priowrr_classify(skb, sch);
	qdisc = q->queues[band];
    skb->cb[47] = (char)((band == 0) || (band == 1)) ? 1 : 3; 
	skb->priority = ((band == 0)||(band == 1)) ? (band + 7) : band;
	
	if ((ret = qdisc->enqueue(skb, qdisc)) == 0) 	
	{
		sch->stats.bytes += skb->len;
		sch->stats.packets++;
		sch->q.qlen++;
#ifdef PRIOWRR_DEBUG
		priowrr_stats[band].enqueue_stats++;
		priowrr_stats[band].qlen++;
		if (priowrr_stats[band].qlen > priowrr_stats[band].maxqlen)
			priowrr_stats[band].maxqlen = priowrr_stats[band].qlen;
#endif
		return 0;
	}
	sch->stats.drops++;
	priowrr_stats[band].drops++;
	return ret;
}


static int priowrr_requeue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	struct Qdisc *qdisc;
	int ret;

	qdisc = q->queues[priowrr_classify(skb, sch)];

	if ((ret = qdisc->ops->requeue(skb, qdisc)) == 0) {
		sch->q.qlen++;
		return 0;
	}
	sch->stats.drops++;
	return ret;
}

static struct sk_buff *priowrr_dequeue(struct Qdisc* sch)
{
	struct sk_buff *skb;
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	int prio, wt_limit;
	struct Qdisc *qdisc;

	/* Strict Priority for EFQ's and WRR for the rest. */
	for (prio = 0; prio < PRIOWRR_BEQ1; prio++)
	{
	qdisc = q->queues[prio];
	skb = qdisc->dequeue(qdisc);
	if (skb) {
		sch->q.qlen--;
#ifdef PRIOWRR_DEBUG
			priowrr_stats[prio].dequeue_stats++;
			priowrr_stats[prio].qlen--;
#endif
			return skb;
		}
	}
		
	wt_limit = q->wted_idx;
	do
	{
		prio = sch_table[q->wted_idx++];
		if (q->wted_idx>=TCQ_MAX_ST)
			q->wted_idx = 0;
		qdisc = q->queues[prio];
		skb = qdisc->dequeue(qdisc);		
		if (skb) {
			sch->q.qlen--;
#ifdef PRIOWRR_DEBUG
			priowrr_stats[prio].dequeue_stats++;
			priowrr_stats[prio].qlen--;
#endif
			return skb;
		}
	}
	while (q->wted_idx!=wt_limit);
	return NULL;
}

static int priowrr_drop(struct Qdisc* sch)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	int prio;
	struct Qdisc *qdisc;

	for (prio = q->bands-1; prio >= 0; prio--) {
		qdisc = q->queues[prio];
		if (qdisc->ops->drop(qdisc)) {
			sch->q.qlen--;
			return 1;
		}
	}
	return 0;
}


static void priowrr_reset(struct Qdisc* sch)
{
	int prio;
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;

	for (prio=0; prio<q->bands; prio++) {
		priowrr_stats[prio].flushed += q->queues[prio]->q.qlen;
		qdisc_reset(q->queues[prio]);
		priowrr_stats[prio].qlen = q->queues[prio]->q.qlen;
		
	}
	sch->q.qlen = 0;
}

static void priowrr_destroy(struct Qdisc* sch)
{
	int prio;
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;

	for (prio=0; prio<q->bands; prio++) {
		qdisc_destroy(q->queues[prio]);
		q->queues[prio] = &noop_qdisc;
	}
	MOD_DEC_USE_COUNT;
#ifdef PRIOWRR_DEBUG
	remove_proc_entry("net/priowrr", NULL);
#endif
}

static void priowrr_create_sch_table(int wt, int qid, int offset_flag)
{
	int i, j, offset;
	
	offset = 0;
	if ((offset_flag==0)&&(qid==PRIOWRR_BEQ2))
		offset = 2;
	
	for (i=0; (i<wt)&&(offset<TCQ_MAX_ST); i++)
	{
		if (offset_flag == 1)
		{
			for (j=offset; (sch_table[j]!=0)&&(j<TCQ_MAX_ST); j++);
			if (j < TCQ_MAX_ST)
			{
				offset = j;
				sch_table[offset] = qid;
			}
			continue;
		}		
		sch_table[offset++] = qid;

		if (qid==PRIOWRR_BEQ2)
			offset += 2;
		else if (((i+1)%2)==0)
			offset++;
	}		
}

struct Qdisc * new_qdisc_create_dflt(struct net_device *dev, struct Qdisc_ops *ops, struct tc_fifo_qopt *opt, struct Qdisc *qdisc)
{
	struct Qdisc *sch = NULL;
	struct rtattr *rta = NULL;
	int size = sizeof(*sch) + ops->priv_size;
	int len = RTA_LENGTH(sizeof(*opt));

	if (qdisc == NULL)
	{
	sch = kmalloc(size, GFP_KERNEL);
	if (!sch)
		return NULL;
	memset(sch, 0, size);

	skb_queue_head_init(&sch->q);
	sch->ops = ops;
	sch->enqueue = ops->enqueue;
	sch->dequeue = ops->dequeue;
	sch->dev = dev;
	sch->stats.lock = &dev->queue_lock;
	atomic_set(&sch->refcnt, 1);
	}

	/* creating fifo subclasses, with the option for setting qlen limits. */
	rta = kmalloc(len, GFP_KERNEL);
	if (!rta)
		return NULL;
	memset(rta, 0, len);
	rta->rta_type = TCA_OPTIONS;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), opt, sizeof(*opt));
	
	if (qdisc == NULL)
	{
	if (!ops->init || ops->init(sch, rta) == 0)
		{
			kfree(rta);
		return sch;
		}
	kfree(sch);
	}
	else
	{
		if (!ops->change || ops->change(qdisc, rta) == 0)
		{
			kfree(rta);
			return qdisc;
		}
	}
	kfree(rta);
	return NULL;
}

static int priowrr_tune(struct Qdisc *sch, struct rtattr *opt)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	struct tc_priowrr_qopt *qopt = RTA_DATA(opt);
	int band, i, wt_afq, wt_bfq, total;	

	if (opt->rta_len < RTA_LENGTH(sizeof(*qopt)))
		return -EINVAL;

	/* Don't overwrite the old values if they are not defined. */
	if ((qopt->weightmap[PRIOWRR_BEQ1]==0) && (qopt->weightmap[PRIOWRR_BEQ2]==0))
	{
		qopt->weightmap[PRIOWRR_EFQ1] = q->weight2band[PRIOWRR_EFQ1];
		qopt->weightmap[PRIOWRR_EFQ2] = q->weight2band[PRIOWRR_EFQ2];
		qopt->weightmap[PRIOWRR_BEQ1] = q->weight2band[PRIOWRR_BEQ1];
		qopt->weightmap[PRIOWRR_BEQ2] = q->weight2band[PRIOWRR_BEQ2];
	}
	if ((qopt->qlenmap[PRIOWRR_EFQ1] == 0) || (qopt->qlenmap[PRIOWRR_EFQ2] == 0))
	{
		qopt->qlenmap[PRIOWRR_EFQ1] = q->qlen2band[PRIOWRR_EFQ1];
		qopt->qlenmap[PRIOWRR_EFQ2] = q->qlen2band[PRIOWRR_EFQ2];
		qopt->qlenmap[PRIOWRR_BEQ1] = q->qlen2band[PRIOWRR_BEQ1];
		qopt->qlenmap[PRIOWRR_BEQ2] = q->qlen2band[PRIOWRR_BEQ2];
	}
	
	total = 0;
	for (i=0; i<TCQ_PRIOWRR_BANDS; i++) {
		if ((i != 0) && (qopt->weightmap[i] > TCQ_MAX_WT))
			return -EINVAL;
		total += qopt->weightmap[i];
	}
	if (total > ((TCQ_PRIOWRR_BANDS - 1)*TCQ_MAX_WT))
		return -EINVAL;

	/* Only the queue lengths for EF queues are defined. Queue lengths for 
	 * AF and BF Queues are directly proportional to their weights. */	
	total = sch->dev->tx_queue_len - (qopt->qlenmap[PRIOWRR_EFQ1] + qopt->qlenmap[PRIOWRR_EFQ2]);
	if (qopt->qlenmap[PRIOWRR_BEQ1] == 0)
		qopt->qlenmap[PRIOWRR_BEQ1] = (total*qopt->weightmap[PRIOWRR_BEQ1])/100;
	if (qopt->qlenmap[PRIOWRR_BEQ2] == 0)
		qopt->qlenmap[PRIOWRR_BEQ2] = total - qopt->qlenmap[PRIOWRR_BEQ1];

	total = 0;
	for (i=0; i<TCQ_PRIOWRR_BANDS; i++)
		total += qopt->qlenmap[i];
	if (total > sch->dev->tx_queue_len)
		return -EINVAL;

	sch_tree_lock(sch);
	q->bands = TCQ_PRIOWRR_BANDS;
	q->wted_idx	= 0;
	memcpy(q->weight2band, qopt->weightmap, TCQ_PRIOWRR_BANDS);
	memcpy(q->qlen2band, qopt->qlenmap, TCQ_PRIOWRR_BANDS);
	sch_tree_unlock(sch);

	printk("  weightmap:");
	for (i=0; i<q->bands; i++)
		printk(" %d", q->weight2band[i]);
	printk("\n  qlenmap:");
	for (i=0; i<q->bands; i++)
		printk(" %d", q->qlen2band[i]);

	/* Create scheduling table */
	wt_afq = (q->weight2band[PRIOWRR_BEQ1]*10)/100;
	wt_bfq = (q->weight2band[PRIOWRR_BEQ2]*10)/100;
	if ((wt_afq+wt_bfq) > TCQ_MAX_ST)
		return -EINVAL;
	memset(sch_table, 0, TCQ_MAX_ST);
	
	if ((wt_afq-wt_bfq) > (TCQ_MAX_ST/2))
	{
		priowrr_create_sch_table(wt_bfq, PRIOWRR_BEQ2, 0);
		priowrr_create_sch_table(wt_afq, PRIOWRR_BEQ1, 1);
	}
	else
	{
		priowrr_create_sch_table(wt_afq, PRIOWRR_BEQ1, 0);
		priowrr_create_sch_table(wt_bfq, PRIOWRR_BEQ2, 1);
	}
	printk("\n  sch_table:");
	for (i=0; i<(wt_afq+wt_bfq); i++)
		printk(" %d", sch_table[i]);
	printk("\n");

	for (band=0; band<q->bands; band++) {
		struct tc_fifo_qopt pfifo_opt;
			memset(&pfifo_opt, 0, sizeof(pfifo_opt));
			pfifo_opt.limit = q->qlen2band[band];
		if (q->queues[band] == &noop_qdisc) {
			struct Qdisc *child;
			child = new_qdisc_create_dflt(sch->dev, &pfifo_qdisc_ops, &pfifo_opt, NULL);
			if (child) {
				sch_tree_lock(sch);
				child = xchg(&q->queues[band], child);

				if (child != &noop_qdisc)
					qdisc_destroy(child);
				sch_tree_unlock(sch);
			}
		}
		else
		{
			/* Tuning the qlen limits for the queues */
			if (!new_qdisc_create_dflt(sch->dev, &pfifo_qdisc_ops, &pfifo_opt, q->queues[band]))
				return -EINVAL;
		}
	}
	
#ifdef PRIOWRR_DEBUG
	for (i=0; i<TCQ_PRIOWRR_BANDS; i++)
	{
		priowrr_stats[i].enqueue_stats = 0;
		priowrr_stats[i].dequeue_stats = 0;
		priowrr_stats[i].qlen = 0;
		priowrr_stats[i].maxqlen = 0;
		priowrr_stats[i].drops = 0;
		priowrr_stats[i].flushed = 0;
		
	}
#endif
	return 0;
}

static int priowrr_init(struct Qdisc *sch, struct rtattr *opt)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	int i;

	printk("priowrr: %s\n", sch->dev->name);
	for (i=0; i<TCQ_PRIOWRR_BANDS; i++)
		q->queues[i] = &noop_qdisc;

	q->bands = TCQ_PRIOWRR_BANDS;
	memset(q->weight2band, 0, TCQ_PRIOWRR_BANDS);
	memset(q->qlen2band, 0, TCQ_PRIOWRR_BANDS);
	
	if (opt == NULL) {
		return -EINVAL;
	} else {
		int err;

		if ((err= priowrr_tune(sch, opt)) != 0)
			return err;
	}
	MOD_INC_USE_COUNT;
#ifdef PRIOWRR_DEBUG
	create_proc_read_entry("net/priowrr", 0, 0, priowrr_read_proc, NULL);
#endif
	printk("init: success\n");
	return 0;
}

static int priowrr_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	unsigned char	 *b = skb->tail;
	struct tc_priowrr_qopt opt;

	memcpy(&opt.weightmap, q->weight2band, TCQ_PRIOWRR_BANDS);
	memcpy(&opt.qlenmap, q->qlen2band, TCQ_PRIOWRR_BANDS);
	
	RTA_PUT(skb, TCA_OPTIONS, sizeof(opt), &opt);
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}

static int priowrr_graft(struct Qdisc *sch, unsigned long arg, struct Qdisc *new,
		      struct Qdisc **old)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	unsigned long band = arg - 1;

	if (band >= q->bands)
		return -EINVAL;

	if (new == NULL)
		new = &noop_qdisc;

	sch_tree_lock(sch);
	*old = q->queues[band];
	q->queues[band] = new;
	qdisc_reset(*old);
	sch_tree_unlock(sch);

	return 0;
}

static struct Qdisc *priowrr_leaf(struct Qdisc *sch, unsigned long arg)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	unsigned long band = arg - 1;

	if (band >= q->bands)
		return NULL;

	return q->queues[band];
}

static unsigned long priowrr_get(struct Qdisc *sch, u32 classid)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	unsigned long band = TC_H_MIN(classid);

	if (band - 1 >= q->bands)
		return 0;
	return band;
}

static unsigned long priowrr_bind(struct Qdisc *sch, unsigned long parent, u32 classid)
{
	return priowrr_get(sch, classid);
}


static void priowrr_put(struct Qdisc *q, unsigned long cl)
{
	return;
}

static int priowrr_change(struct Qdisc *sch, u32 handle, u32 parent, struct rtattr **tca, unsigned long *arg)
{
	unsigned long cl = *arg;
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;

	if (cl - 1 > q->bands)
		return -ENOENT;
	return 0;
}

static int priowrr_delete(struct Qdisc *sch, unsigned long cl)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	if (cl - 1 > q->bands)
		return -ENOENT;
	return 0;
}


static int priowrr_dump_class(struct Qdisc *sch, unsigned long cl, struct sk_buff *skb,
			   struct tcmsg *tcm)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;

	if (cl - 1 > q->bands)
		return -ENOENT;
	if (q->queues[cl-1])
		tcm->tcm_info = q->queues[cl-1]->handle;
	return 0;
}

static void priowrr_walk(struct Qdisc *sch, struct qdisc_walker *arg)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;
	int prio;

	if (arg->stop)
		return;

	for (prio = 0; prio < q->bands; prio++) {
		if (arg->count < arg->skip) {
			arg->count++;
			continue;
		}
		if (arg->fn(sch, prio+1, arg) < 0) {
			arg->stop = 1;
			break;
		}
		arg->count++;
	}
}

static struct tcf_proto ** priowrr_find_tcf(struct Qdisc *sch, unsigned long cl)
{
	struct priowrr_sched_data *q = (struct priowrr_sched_data *)sch->data;

	if (cl)
		return NULL;
	return &q->filter_list;
}

static struct Qdisc_class_ops priowrr_class_ops =
{
	priowrr_graft,
	priowrr_leaf,

	priowrr_get,
	priowrr_put,
	priowrr_change,
	priowrr_delete,
	priowrr_walk,

	priowrr_find_tcf,
	priowrr_bind,
	priowrr_put,

	priowrr_dump_class,
};

struct Qdisc_ops priowrr_qdisc_ops =
{
	NULL,
	&priowrr_class_ops,
	"priowrr",
	sizeof(struct priowrr_sched_data),

	priowrr_enqueue,
	priowrr_dequeue,
	priowrr_requeue,
	priowrr_drop,

	priowrr_init,
	priowrr_reset,
	priowrr_destroy,
	priowrr_tune,

	priowrr_dump,
};

extern struct atm_dev *atm_devs;
int flush_driver_queues(int vpi, int vci)
{
	struct tx_flush
	{
		struct atm_vcc *vcc;
		int queue_flag; /* 0, for queue 0, 1 for queue 1, 2 for both */
		int skip_num; 	/* optional parameter to skip "skip_num" of packets from head.*/
	} driver_info;
	struct atm_dev *dev;
	struct atm_vcc *vcc;
		
	memset((void *)&driver_info, 0, sizeof(struct tx_flush));
	
	for (dev = atm_devs; dev; dev = dev->next) {
		for (vcc = dev->vccs; vcc; vcc = vcc->next) {
			if ((vcc->vpi == vpi) && (vcc->vci == vci)) {
				driver_info.vcc = vcc;
				driver_info.queue_flag = 1;
				printk("ATM_FLUSH: vpi/vci %d/%d\n", vpi, vci);
				/* Call ATM driver's ioctl */
				vcc->dev->ops->ioctl (vcc->dev, ATM_FLUSHTX, (void *)&driver_info);
			}
		}
	}
	return 0;
}

#define MIN_MTU		68
/*
 * names[] : device name/s for a PVC. Note that there can be multiple interfaces sharing a single PVC.
 * asymmtu : Asymmetric MTU.
 * vpi,vci : VPI/VCI pair corresponding this PVC, required to call flush ioctl of ATM driver.
 *
 * 	Asymmetric MTU is used for fragmentation to reduce the serialization delay for voice traffic.
 * This is calculated based on the Upstream rate of DSL modem and Interpacket time/delay for 
 * voice packets. This delay is also known as Voice payload size in ms.
 * 
*/
spinlock_t qos_lock;

asmlinkage int sys_config_voice_qos(char *names[], int asymmtu, int vpi, int vci)
{
	int i = 0;
	struct net_device *dev;	
	long flags;
	
	spin_lock_irqsave(&qos_lock, flags);
	
	printk("sys_config_voice:\n");
	
	if ((asymmtu) && (asymmtu < MIN_MTU))
		asymmtu = MIN_MTU;
	
	for (i = 0; names[i] != NULL; i++)
	{
		dev = __dev_get_by_name(names[i]);
		if (!dev)
			continue;
	
		if (asymmtu == 0) {
			/* Reset MTU */
			dev->unused_alignment_fixer = dev->mtu;			
			printk("%s: Reset MTU - %d\n", names[i], dev->unused_alignment_fixer);
		}
		else {			
			/* Set MTU and Flush IP Tx Queues */
			dev->unused_alignment_fixer = ((asymmtu > dev->mtu) ? dev->mtu : asymmtu);
			qdisc_reset(dev->qdisc);			
			printk("%s: Set MTU - %d\n%s: Flush IP queues\n", names[i], dev->unused_alignment_fixer, names[i]);
		}
	}

	if (asymmtu){
		/* Now Flush Driver Tx queues */
		flush_driver_queues(vpi, vci);
	}
	spin_unlock_irqrestore(&qos_lock, flags);
			
	return 0;
}

#ifdef MODULE

int init_module(void)
{
	return register_qdisc(&priowrr_qdisc_ops);
}

void cleanup_module(void) 
{
	unregister_qdisc(&priowrr_qdisc_ops);
}

#endif
MODULE_LICENSE("GPL");
