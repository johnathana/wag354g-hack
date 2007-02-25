/*
 * WFQ (PGPS) implementation with classes
 * 2001 Csaba Toth BME-SCH-KSZK tusi@sch.bme.hu
 *
 *
 * WFQ algorithm shortly:
 * 
 * Virtual time change: dV(t) = d(real time)/ sum(weight of all baclogged class)
 *   V(t) = 0, if there are not packets in the system
 *   change should be applied in each enqueue/dequeue before actually doing
 *   the enqueue/dequeue
 * in each enqueue:
 *   S(i,k) = max{ V(t), F(i-1,k) } where S(i,k) is the starting time of
 *   the i-th packet of the k-th class, F(i-1,k) is the finishing time of
 *   the (i-1)-th packet of the same class, or 0, if i == 0.
 *   F(i,k) = S(i,k) + (packet length / weight of the class) * 1/ifspeed.
 * in each dequeue:
 *   The packet with the smallest finishing time should be dequeued
 *
 * Full description can be found in:
 * A.B.Parekh and R.G.Gallegher: A Generalized Processor Sharing Approach
 * to Flow Control in Integrated Services Networks: The Single-Node Case
 * www.acm.org/pubs/articles/journals/ton/1993-1-3/p344-parekh/p344-parekh.pdf
 *
 * 2001.04.12 version 0.1
 */

#include <linux/module.h>
#include <net/ip.h>
#include <linux/vmalloc.h>
#include <net/pkt_sched.h>
#include <asm-i386/div64.h>

#ifndef CONFIG_X86_TSC
#error sch_wfq was tested only on x86 arhitectures having TSC
#endif

struct tc_wfq_opt
{
	unsigned int	default_fi;
        unsigned int	ifspeed;
        unsigned int	ticks_per_sec;
};

struct tc_wfq_class_opt
{
        unsigned int   fi;
};

struct wfq_ftime
{
	u64		ftime;
	struct wfq_ftime* next;
};

		
struct wfq_class {
        unsigned int		classid;
        struct Qdisc*		q;
        unsigned int		fi; /*weight of the flow the class represents*/
        struct tc_stats		stats;
        struct wfq_class*	next; /* next class of the parent qdisc */
        struct wfq_class*	prev; /* prev class of the parent qdisc */
	struct wfq_ftime*	first; /* first packet in the class queue */
	struct wfq_ftime*	last; /* last packet in the class queue */
};

struct wfq_data {
        struct wfq_class*       first; /* first class in the chain */
        struct wfq_class*       last; /* last class in the chain */
        struct wfq_class*       default_class;
        struct tcf_proto*       filters;
        struct tc_stats         stats;
        unsigned int            sfi; /* sum of the fi of backlogged classes */
        u64      		vtime; /* V(t) */
        u64      		last_vu; /* last real time V(t) was updated */
        unsigned int            lenmulti; /* 1/ifspeed */
        unsigned int            ticks_per_sec;
        unsigned int            ifspeed;
};

struct wfq_class* wfq_find_class(struct Qdisc* q,u32 classid);
void wfq_class_walk(struct Qdisc *q, struct qdisc_walker* walker);

void wfq_update_vtime(struct Qdisc* q)
{
	u64 timenow;
	struct wfq_data* data = (struct wfq_data*)q->data;
	
	rdtscll(timenow);
	if( data->sfi == 0 )
		data->vtime = 0;	
	else {
		timenow -= data->last_vu;
		do_div(timenow,data->sfi);
		data->vtime += timenow;
	}

	data->last_vu = timenow;
}

int wfq_enqueue_requeue(struct sk_buff* skb, struct Qdisc* q,char cmd)
{
	struct tcf_result result;
	int res;
	struct wfq_data* data=(struct wfq_data*)q->data;
	struct wfq_class* cl;
	u64 starttime;
	u64 fintime = 0;
	int err;
	struct wfq_ftime* ptr;

	if( !data->filters || (res=tc_classify(skb,data->filters,&result))<0)
		cl = data->default_class;
	else {
		if(result.class)
			cl=(struct wfq_class*)result.class;
		else
			cl=wfq_find_class(q,result.classid);

		if(!cl)
			cl = data->default_class;
	}
	wfq_update_vtime(q);
	if( (ptr = (struct wfq_ftime*)kmalloc(sizeof(*ptr),GFP_ATOMIC)) == NULL )
			return -ENOMEM;
	ptr->next = NULL;
	switch(cmd) {
	case 'e':
		if( (err = cl->q->enqueue(skb,cl->q)) )
			break;
		if( cl->stats.qlen == 0 ) {
			data->sfi+=cl->fi;
			starttime = data->vtime;
			cl->first = cl->last = ptr;
		} else {
			starttime = cl->last->ftime > data->vtime ? cl->last->ftime : data->vtime;
			cl->last->next = ptr;
			cl->last = ptr;
		}
		fintime = starttime + ((skb->len*data->lenmulti)/cl->fi);
		break;
	case 'r':
		err = -ENOSYS;
		if( !cl->q->ops->requeue || (err = cl->q->ops->requeue(skb,cl->q)) )
			break;
		if( cl->stats.qlen == 0 ) {
			data->sfi+=cl->fi;
			cl->first = cl->last = ptr;
		} else {
			ptr->next = cl->first;
			cl->first = ptr;
		}
		break;
	default:
		return -ENOSYS;
	}
	ptr->ftime = fintime;
	if( err ) {
		kfree(ptr);
		cl->stats.drops++;
		return(err);
	}
	cl->stats.bytes += skb->len;
	cl->stats.packets++;
	cl->stats.qlen++;
	cl->stats.backlog += skb->len;
	return 0;
}

int wfq_enqueue(struct sk_buff* skb, struct Qdisc* q)
{
	int err;
	q->stats.bytes += skb->len;
	q->stats.packets++;
	err = wfq_enqueue_requeue(skb,q,'e');
	if( err ) {
		q->stats.drops++;
		return err;
	}
	q->q.qlen++;
	q->stats.qlen++;
	q->stats.backlog += skb->len;
	return 0;
}

int wfq_requeue(struct sk_buff* skb, struct Qdisc* q)
{
	int err;
	err = wfq_enqueue_requeue(skb,q,'r');
	if( err ) {
		q->stats.drops++;
		return err;
	}
	q->q.qlen++;
	q->stats.qlen++;
	q->stats.backlog += skb->len;
	return 0;
}

struct wfq_walker_find_minftime {
	struct qdisc_walker	w;
	struct wfq_class*	cl;
	u64			min;
};

static int wfq_walker_findmin(struct Qdisc* q,unsigned long class,struct qdisc_walker* walker)
{
	struct wfq_walker_find_minftime* arg = (struct wfq_walker_find_minftime*)walker;
	struct wfq_class* cl = (struct wfq_class*)class;
	if( cl && cl->first && cl->first->ftime < arg->min ) {
		arg->cl = cl;
		arg->min = cl->first->ftime;
	}
	return 0;
}

struct sk_buff* wfq_dequeue(struct Qdisc* q)
{
	struct sk_buff* skb;
	struct wfq_data* data=(struct wfq_data*)q->data;
	struct wfq_walker_find_minftime arg;
	struct wfq_ftime* ptr;

	if( q->q.qlen == 0 )
		return NULL;

	arg.w.fn = wfq_walker_findmin;
	arg.min = 0xffffffffffffffffUL;
	arg.cl = NULL;
	wfq_class_walk(q,&arg.w);
	if( (arg.cl == NULL) || ((skb=arg.cl->q->dequeue(arg.cl->q)) == NULL) )
		return NULL;

	arg.cl->stats.qlen--;
	arg.cl->stats.backlog -= skb->len;
	q->stats.qlen--;
	q->stats.backlog -= skb->len;
	q->q.qlen--;
	wfq_update_vtime(q);
	ptr = arg.cl->first;
	arg.cl->first = arg.cl->first->next;
	if( arg.cl->first == NULL )
		arg.cl->last = NULL;
	kfree(ptr);
	
	if( arg.cl->stats.qlen == 0 )
		data->sfi -= arg.cl->fi;
	return skb;

}


int wfq_init(struct Qdisc* q,struct rtattr* arg)
{

	struct wfq_data* data=(struct wfq_data*)q->data;
	struct tc_wfq_opt *ctl = RTA_DATA(arg);
	struct timeval tv1,tv2;
	u64 ts1,ts2;
	unsigned long stop;

	if( arg == NULL || arg->rta_len < RTA_LENGTH(sizeof(*ctl)) )
		return -EINVAL;

        memset(data, 0, sizeof(*data));
        if( (data->default_class = vmalloc(sizeof(struct wfq_class))) == NULL )
		return -ENOMEM;
	MOD_INC_USE_COUNT;
        memset(data->default_class, 0, sizeof(*(data->default_class)));
	data->default_class->classid = q->handle;
	data->default_class->fi = ctl->default_fi;
	data->default_class->first = data->default_class->last = NULL;
	if (!(data->default_class->q = qdisc_create_dflt(q->dev, &pfifo_qdisc_ops)))
		data->default_class->q = &noop_qdisc;
	data->ifspeed = ctl->ifspeed;
	do_gettimeofday(&tv1);
	rdtscll(ts1);
	stop = jiffies+HZ/10;
	while(time_before(jiffies,stop))
		barrier();
	do_gettimeofday(&tv2);
	rdtscll(ts2);
	ts2 -= ts1; ts2 = ts2 * 1000000;
	do_div(ts2,((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec)));
	data->ticks_per_sec = ts2;
	data->lenmulti = data->ticks_per_sec / data->ifspeed;

	return 0;
}

int wfq_reset_class(struct Qdisc* q,unsigned long class,struct qdisc_walker* walker)
{
	struct wfq_ftime* ptr;
	struct wfq_class* cl = (struct wfq_class*)class;
	while((ptr=cl->first)) {
		cl->first = cl->first->next;
		kfree(ptr);
	}
	cl->last = NULL;
	qdisc_reset(cl->q);
	memset(&cl->stats,0,sizeof(cl->stats));
	return 0;
}
		

void wfq_reset(struct Qdisc* q)
{
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct qdisc_walker w;

	w.fn = wfq_reset_class;
	wfq_class_walk(q,&w);

	q->q.qlen = 0;
	memset(&data->stats,0,sizeof(data->stats));
	data->sfi = 0;
	wfq_update_vtime(q);
}

void wfq_destroy(struct Qdisc* q)
{
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct wfq_class* cl;
	if( data->first != NULL ) {
		while(data->first != data->last ) {
			cl = data->last;
			data->last = data->last->prev;
			data->last->next = NULL;
			if( cl->q && cl->q->ops->destroy )
				cl->q->ops->destroy(cl->q);
			kfree(cl);
		}
		cl = data->last;
		if( cl && cl->q && cl->q->ops->destroy )
			cl->q->ops->destroy(cl->q);
		kfree(cl);
		data->last = data->first = NULL;
	}
	cl = data->default_class;
	if( cl->q && cl->q->ops->destroy )
		cl->q->ops->destroy(cl->q);
	vfree(cl);
	data->default_class = NULL;
	MOD_DEC_USE_COUNT;
	return;
}

int wfq_dump(struct Qdisc* q,struct sk_buff* skb)
{
	unsigned char    *b = skb->tail;
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct tc_wfq_opt qopt;

	qopt.ifspeed = data->ifspeed;
	qopt.ticks_per_sec = data->ticks_per_sec;
	qopt.default_fi = data->default_class->fi;
	RTA_PUT(skb,TCA_OPTIONS,sizeof(qopt),&qopt);
	RTA_PUT(skb,TCA_STATS,sizeof(q->stats),&q->stats);
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}

/*
 * ==============================================
 * class operation functions 
 * ============================================== 
 */

int wfq_class_graft(struct Qdisc* q, unsigned long cl, struct Qdisc* new, struct Qdisc** old)
{
	struct wfq_class* class=(struct wfq_class*)cl;
	*old=class->q;
	class->q=new;
	return 0;
}

struct Qdisc* wfq_class_leaf(struct Qdisc* q, unsigned long cl)
{
	return(((struct wfq_class*)cl)->q);
}

struct wfq_walker_find_class_s {
	struct qdisc_walker	w;
	unsigned int		target;
	struct wfq_class*	found;
};

static int wfq_walker_find_class(struct Qdisc* q,unsigned long class,struct qdisc_walker* walker)
{
	struct wfq_walker_find_class_s* data = (struct wfq_walker_find_class_s*)walker;
	struct wfq_class* cl = (struct wfq_class*)class;
	if( cl == NULL )
		return 0;
	if( cl->classid == data->target )
		data->found = cl;
	return 0;
}

struct wfq_class* wfq_find_class(struct Qdisc* q,u32 classid)
{
	struct wfq_walker_find_class_s arg;
	arg.w.fn = wfq_walker_find_class;
	arg.target = classid;
	arg.found = NULL;
	wfq_class_walk(q,&arg.w);
	return(arg.found);
}

unsigned long wfq_class_get(struct Qdisc* q, u32 classid)
{
	return((unsigned long)wfq_find_class(q,classid));
}

void wfq_class_put(struct Qdisc* q, unsigned long arg)
{
	return;
}

int wfq_class_change(struct Qdisc *q, u32 classid, u32 parentid, struct rtattr **tca, unsigned long *arg)
{
	struct wfq_class* cl = (struct wfq_class*)*arg;
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct rtattr* opt = tca[TCA_OPTIONS-1];
	struct tc_wfq_class_opt* copt;

	if (parentid != q->handle || !classid)
		return -EINVAL;

	if(RTA_PAYLOAD(opt) < sizeof(*copt))
		return -EINVAL;

	copt = RTA_DATA(opt);
	if(cl) {
		cl->fi = copt->fi;
		return 0;
	}

	cl = kmalloc(sizeof(*cl), GFP_KERNEL);
	if( cl == NULL )
		return -ENOMEM;
	memset(cl,0,sizeof(*cl));
	cl->classid=TC_H_MAKE(q->handle,TC_H_MIN(classid));
	if( wfq_find_class(q,cl->classid) )
		return -EEXIST;
	if (!(cl->q = qdisc_create_dflt(q->dev, &pfifo_qdisc_ops)))
		cl->q = &noop_qdisc;
	cl->fi = copt->fi;
	if(data->last) {
		data->last->next = cl;
		cl->prev=data->last;
	} else
		data->first = cl;

	data->last=cl;
	*arg = (unsigned long)cl;

	return 0;
}

int wfq_class_delete(struct Qdisc *q, unsigned long class)
{
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct wfq_class* cl;

	for(cl=data->first;cl!=NULL&&cl!=(struct wfq_class*)class;cl=cl->next);
	if(cl) {
		if(cl==data->first)
			data->first=cl->next;
		if(cl==data->last)
			data->last=cl->prev;
		if(cl->next)
			cl->next->prev=cl->prev;
		if(cl->prev)
			cl->prev->next=cl->next;

		kfree(cl);
	}
	return 0;
}

void wfq_class_walk(struct Qdisc *q, struct qdisc_walker* walker)
{
	struct wfq_data* data = (struct wfq_data*)q->data;
	struct wfq_class* cl;

	walker->fn(q,(unsigned long)(data->default_class),walker);
	for(cl=data->first;cl!=NULL;cl=cl->next)
		walker->fn(q,(unsigned long)cl,walker);

}


struct tcf_proto** wfq_class_tcf_chain(struct Qdisc *q,unsigned long arg)
{
	struct wfq_data* data = (struct wfq_data*)q->data;
	if( arg )
		return NULL;
	return &data->filters;
}

unsigned long wfq_class_bind_tcf(struct Qdisc *q, unsigned long parent, u32 classid)
{
	return (wfq_class_get(q,classid));
}

void wfq_class_unbind_tcf(struct Qdisc* q, unsigned long cl)
{
	return (wfq_class_put(q,cl));
}

int wfq_class_dump(struct Qdisc* q, unsigned long cl, struct sk_buff* skb, struct tcmsg* tcm)
{
	unsigned char    *b = skb->tail;
	struct wfq_class* class = (struct wfq_class*)cl;
	struct tc_wfq_class_opt copt;

	copt.fi = class->fi;
	RTA_PUT(skb,TCA_OPTIONS,sizeof(copt),&copt);
	RTA_PUT(skb,TCA_STATS,sizeof(class->stats),&class->stats);
	tcm->tcm_parent = TC_H_ROOT;
	tcm->tcm_handle = class->classid;
	tcm->tcm_info = class->q->handle;
		
	return skb->len;

rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}

struct Qdisc_class_ops wfq_class_ops =
{
	wfq_class_graft,
	wfq_class_leaf,
	wfq_class_get,
	wfq_class_put,
	wfq_class_change,
	wfq_class_delete,
	wfq_class_walk,
	wfq_class_tcf_chain,
	wfq_class_bind_tcf,
	wfq_class_unbind_tcf,
	wfq_class_dump,
};
struct Qdisc_ops wfq_qdisc_ops =
{
        NULL,
        &wfq_class_ops,
        "wfq",
        sizeof(struct wfq_data),

        wfq_enqueue,
        wfq_dequeue,
        wfq_requeue,
        NULL, /* drop */

        wfq_init,
        wfq_reset,
        wfq_destroy,
        NULL,    /* change */

#ifdef CONFIG_RTNETLINK
        wfq_dump,
#endif
};
#ifdef MODULE
int init_module(void)
{
        return register_qdisc(&wfq_qdisc_ops);
}

void cleanup_module(void)
{
        unregister_qdisc(&wfq_qdisc_ops);
}
#endif

