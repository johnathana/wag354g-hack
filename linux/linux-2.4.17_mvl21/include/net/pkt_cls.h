/*--------------------------------------------------------------------------------
 * Copyright 2002-2004, Texas Instruments Incorporated
 *
 * This program has been modified from its original operation by Texas Instruments
 * to do the following:
 * 		 The function tc_classify has been modified to reset the skb->cb field,
 * 		 which carried the original source address required for classification.
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
 *------------------------------------------------------------------------------*/
 
#ifndef __NET_PKT_CLS_H
#define __NET_PKT_CLS_H


#include <linux/pkt_cls.h>

struct rtattr;
struct tcmsg;

/* Basic packet classifier frontend definitions. */

struct tcf_result
{
	unsigned long	class;
	u32		classid;
};

struct tcf_proto
{
	/* Fast access part */
	struct tcf_proto	*next;
	void			*root;
	int			(*classify)(struct sk_buff*, struct tcf_proto*, struct tcf_result *);
	u32			protocol;

	/* All the rest */
	u32			prio;
	u32			classid;
	struct Qdisc		*q;
	void			*data;
	struct tcf_proto_ops	*ops;
};

struct tcf_walker
{
	int	stop;
	int	skip;
	int	count;
	int	(*fn)(struct tcf_proto *, unsigned long node, struct tcf_walker *);
};

struct tcf_proto_ops
{
	struct tcf_proto_ops	*next;
	char			kind[IFNAMSIZ];

	int			(*classify)(struct sk_buff*, struct tcf_proto*, struct tcf_result *);
	int			(*init)(struct tcf_proto*);
	void			(*destroy)(struct tcf_proto*);

	unsigned long		(*get)(struct tcf_proto*, u32 handle);
	void			(*put)(struct tcf_proto*, unsigned long);
	int			(*change)(struct tcf_proto*, unsigned long, u32 handle, struct rtattr **, unsigned long *);
	int			(*delete)(struct tcf_proto*, unsigned long);
	void			(*walk)(struct tcf_proto*, struct tcf_walker *arg);

	/* rtnetlink specific */
	int			(*dump)(struct tcf_proto*, unsigned long, struct sk_buff *skb, struct tcmsg*);
};

/* Main classifier routine: scans classifier chain attached
   to this qdisc, (optionally) tests for protocol and asks
   specific classifiers.
 */

static inline int tc_classify(struct sk_buff *skb, struct tcf_proto *tp, struct tcf_result *res)
{
	int err = 0;
	u32 protocol = skb->protocol;

	for ( ; tp; tp = tp->next) {
		if ((tp->protocol == protocol ||
		     tp->protocol == __constant_htons(ETH_P_ALL)) &&
		    (err = tp->classify(skb, tp, res)) >= 0)
			return err;
	}
	/* The cb field previously carried the original source address, 
	 * required for classification .*/
	*(__u32 *)skb->cb  = 0;
	return -1;
}



extern int register_tcf_proto_ops(struct tcf_proto_ops *ops);
extern int unregister_tcf_proto_ops(struct tcf_proto_ops *ops);



#endif
