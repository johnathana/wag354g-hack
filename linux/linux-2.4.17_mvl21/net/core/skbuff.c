/*
 *	Routines having to do with the 'struct sk_buff' memory handlers.
 *
 *	Authors:	Alan Cox <iiitac@pyr.swan.ac.uk>
 *			Florian La Roche <rzsfl@rz.uni-sb.de>
 *
 *	Version:	$Id: skbuff.c,v 1.1.1.2 2005/03/28 06:57:04 sparq Exp $
 *
 *	Fixes:	
 *		Alan Cox	:	Fixed the worst of the load balancer bugs.
 *		Dave Platt	:	Interrupt stacking fix.
 *	Richard Kooijman	:	Timestamp fixes.
 *		Alan Cox	:	Changed buffer format.
 *		Alan Cox	:	destructor hook for AF_UNIX etc.
 *		Linus Torvalds	:	Better skb_clone.
 *		Alan Cox	:	Added skb_copy.
 *		Alan Cox	:	Added all the changed routines Linus
 *					only put in the headers
 *		Ray VanTassle	:	Fixed --skb->lock in free
 *		Alan Cox	:	skb_copy copy arp field
 *		Andi Kleen	:	slabified it.
 *
 *	NOTE:
 *		The __skb_ routines should be called with interrupts 
 *	disabled, or you better be *real* sure that the operation is atomic 
 *	with respect to whatever list is being frobbed (e.g. via lock_sock()
 *	or via disabling bottom half handlers, etc).
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

/*
 *	The functions in this file will not compile correctly with gcc 2.4.x
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/cache.h>
#include <linux/rtnetlink.h>
#include <linux/init.h>
#include <linux/highmem.h>

#include <net/protocol.h>
#include <net/dst.h>
#include <net/sock.h>
#include <net/checksum.h>

#include <asm/uaccess.h>
#include <asm/system.h>

/* NSP Optimizations: This definition controls the static memory allocation optimization. */
#define TI_STATIC_ALLOCATIONS

int sysctl_hot_list_len = 512;

static kmem_cache_t *skbuff_head_cache;

#undef DEBUG_SKB 

/* NSP Optimizations: This structure definition defines the data structure that is used
 * for static buffer allocations.  */
#ifdef TI_STATIC_ALLOCATIONS
/* Max. Reserved headroom in front of each packet so that the headers can be added to
 * a packet. Worst case scenario would be PPPoE + 2684 LLC Encapsulation + Ethernet
 * header. */
#define MAX_RESERVED_HEADROOM       20

/* This is the MAX size of the static buffer for pure data. */
#define MAX_SIZE_STATIC_BUFFER      1600

typedef struct DRIVER_BUFFER
{
	/* Pointer to the allocated data buffer. This is the static data buffer 
     * allocated for the TI-Cache. 60 bytes out of the below buffer are required
     * by the SKB shared info. We always reserve at least MAX_RESERVED_HEADROOM bytes 
     * so that the packets always have sufficient headroom. */
	char					ptr_buffer[MAX_SIZE_STATIC_BUFFER + MAX_RESERVED_HEADROOM + 60];

	/* List of the driver buffers. */
	struct DRIVER_BUFFER*	ptr_next;
}DRIVER_BUFFER;

typedef struct DRIVER_BUFFER_MCB
{
	/* List of the driver buffers. */
	DRIVER_BUFFER*	ptr_available_driver_buffers;

    /* The number of available buffers. */
    int                     num_available_buffers;
}DRIVER_BUFFER_MCB;

DRIVER_BUFFER_MCB	driver_mcb;
int                 hybrid_mode = 0;

#endif /* TI_STATIC_ALLOCATIONS */

static union {
	struct sk_buff_head	list;
	char			pad[SMP_CACHE_BYTES];
} skb_head_pool[NR_CPUS];

#ifdef TI_STATIC_ALLOCATIONS
int num_of_buffers(char* buf)
{
   unsigned long flags;
   int  len = 0;
   
   local_irq_save(flags);
   
   len = sprintf(buf,"Numbers of free Buffers %d\n",driver_mcb.num_available_buffers);
   /* Criticial Section: Unlock interrupts. */
   local_irq_restore(flags);
   return len;
}
int read_num_of_free_buff()
{
   return driver_mcb.num_available_buffers;
}
#endif 
/*
 *	Keep out-of-line to prevent kernel bloat.
 *	__builtin_return_address is not used because it is not always
 *	reliable. 
 */

/**
 *	skb_over_panic	- 	private function
 *	@skb: buffer
 *	@sz: size
 *	@here: address
 *
 *	Out of line support code for skb_put(). Not user callable.
 */
 
void skb_over_panic(struct sk_buff *skb, int sz, void *here)
{
	printk("skput:over: %p:%d put:%d dev:%s", 
		here, skb->len, sz, skb->dev ? skb->dev->name : "<NULL>");
	BUG();
}

/**
 *	skb_under_panic	- 	private function
 *	@skb: buffer
 *	@sz: size
 *	@here: address
 *
 *	Out of line support code for skb_push(). Not user callable.
 */
 

void skb_under_panic(struct sk_buff *skb, int sz, void *here)
{
        printk("skput:under: %p:%d put:%d dev:%s",
                here, skb->len, sz, skb->dev ? skb->dev->name : "<NULL>");
	BUG();
}

static __inline__ struct sk_buff *skb_head_from_pool(void)
{
	struct sk_buff_head *list;
        struct sk_buff *skb = NULL;
        unsigned long flags;

        local_irq_save(flags);

        list = &skb_head_pool[smp_processor_id()].list;

	if (skb_queue_len(list)) {

		skb = __skb_dequeue(list);
	}
        local_irq_restore(flags);
        return skb;
}

/**************************************************************************
 * FUNCTION NAME : skb_head_to_pool
 **************************************************************************
 * DESCRIPTION   :
 *  This function was optimized to put the SKB back into the pool, the 
 *  orignal code would keep only 'sysctl_hot_list_len' entries in the list
 *  the remaining skb's would be released back to the SLAB.
***************************************************************************/
static __inline__ void skb_head_to_pool(struct sk_buff *skb)
{
#ifdef TI_STATIC_ALLOCATIONS
	struct sk_buff_head* list;
    unsigned long        flags;

    local_irq_save(flags);
    list = &skb_head_pool[smp_processor_id()].list;
    if (skb_queue_len(list) < sysctl_hot_list_len) 
    {
    __skb_queue_head(list, skb);
	local_irq_restore(flags);
    return;
    }else
    {
       local_irq_restore(flags);
       kmem_cache_free(skbuff_head_cache, skb);
       return;
    }
#else
	struct sk_buff_head *list;
    unsigned long flags;

    local_irq_save(flags);

    list = &skb_head_pool[smp_processor_id()].list;

	if (skb_queue_len(list) < sysctl_hot_list_len) 
    {
        __skb_queue_head(list, skb);
		local_irq_restore(flags);
		return;
	}
    local_irq_restore(flags);
	kmem_cache_free(skbuff_head_cache, skb);
#endif /* TI_STATIC_ALLOCATIONS */
}

/* 	Allocate a new skbuff. We do this ourselves so we can fill in a few
 *	'private' fields and also do memory statistics to find all the
 *	[BEEP] leaks.
 * 
 */

/**
 *	alloc_skb	-	allocate a network buffer
 *	@size: size to allocate
 *	@gfp_mask: allocation mask
 *
 *	Allocate a new &sk_buff. The returned buffer has no headroom and a
 *	tail room of size bytes. The object has a reference count of one.
 *	The return is the buffer. On a failure the return is %NULL.
 *
 *	Buffers may only be allocated from interrupts using a @gfp_mask of
 *	%GFP_ATOMIC.
 */
 
struct sk_buff *alloc_skb(unsigned int size,int gfp_mask)
{
	struct sk_buff *skb;
	u8 *data;

	if (in_interrupt() && (gfp_mask & __GFP_WAIT)) {
		static int count = 0;
		if (++count < 5) {
			printk(KERN_ERR "alloc_skb called nonatomically "
			       "from interrupt %p\n", NET_CALLER(size));
 			BUG();
		}
		gfp_mask &= ~__GFP_WAIT;
	}

	/* Get the HEAD */
	skb = skb_head_from_pool();
   if (skb == NULL) 
   {
      skb = kmem_cache_alloc(skbuff_head_cache, gfp_mask & ~__GFP_DMA);
      if (skb == NULL)
         goto nohead;
      
   }

	/* Get the DATA. Size must match skb_add_mtu(). */
	size = SKB_DATA_ALIGN(size);
	data = kmalloc(size + sizeof(struct skb_shared_info), gfp_mask);
	if (data == NULL)
		goto nodata;

	/* XXX: does not include slab overhead */ 
	skb->truesize = size + sizeof(struct sk_buff);

	/* Load the data pointers. */
	skb->head = data;
	skb->data = data;
	skb->tail = data;
	skb->end = data + size;

	/* Set up other state */
	skb->len = 0;
	skb->cloned = 0;
	skb->data_len = 0;

	atomic_set(&skb->users, 1); 
	atomic_set(&(skb_shinfo(skb)->dataref), 1);
	skb_shinfo(skb)->nr_frags = 0;
	skb_shinfo(skb)->frag_list = NULL;
	return skb;

nodata:
	skb_head_to_pool(skb);
nohead:
	return NULL;
}

/*
 *	Slab constructor for a skb head. 
 */ 
static inline void skb_headerinit(void *p, kmem_cache_t *cache, 
				  unsigned long flags)
{
	struct sk_buff *skb = p;

	skb->next = NULL;
	skb->prev = NULL;
	skb->list = NULL;
	skb->sk = NULL;
	skb->stamp.tv_sec=0;	/* No idea about time */
	skb->dev = NULL;
	skb->dst = NULL;
	memset(skb->cb, 0, sizeof(skb->cb));
	skb->pkt_type = PACKET_HOST;	/* Default type */
	skb->ip_summed = 0;
	skb->priority = 0;
	skb->security = 0;	/* By default packets are insecure */
	skb->destructor = NULL;

#ifdef CONFIG_NETFILTER
	skb->nfmark = skb->nfcache = 0;
	skb->nfct = NULL;
#ifdef CONFIG_NETFILTER_DEBUG
	skb->nf_debug = 0;
#endif
#endif
#ifdef CONFIG_NET_SCHED
	skb->tc_index = 0;
#endif

#ifdef CONFIG_NET_QOS
	//zhangbin
	skb->cos = 0;
#endif
#ifdef CONFIG_VLAN_8021Q
	//junzhao 2004.9.16
	skb->vid = 0;
#endif	
}

static void skb_drop_fraglist(struct sk_buff *skb)
{
	struct sk_buff *list = skb_shinfo(skb)->frag_list;

	skb_shinfo(skb)->frag_list = NULL;

	do {
		struct sk_buff *this = list;
		list = list->next;
		kfree_skb(this);
	} while (list);
}

static void skb_clone_fraglist(struct sk_buff *skb)
{
	struct sk_buff *list;

	for (list = skb_shinfo(skb)->frag_list; list; list=list->next)
		skb_get(list);
}

static void skb_release_data(struct sk_buff *skb)
{
	if (!skb->cloned ||
	    atomic_dec_and_test(&(skb_shinfo(skb)->dataref))) {
		if (skb_shinfo(skb)->nr_frags) {
			int i;
			for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
				put_page(skb_shinfo(skb)->frags[i].page);
		}

		if (skb_shinfo(skb)->frag_list)
			skb_drop_fraglist(skb);

		kfree(skb->head);
	}
}

/* NSP Optimizations: These function were added to retrieve and replace buffers from the 
 * static memory pools instead of the Linux Slab Manager. */
#ifdef TI_STATIC_ALLOCATIONS

static void skbuff_data_invalidate(void *pmem, int size)
{
  unsigned int i,Size=(((unsigned int)pmem)&0xf)+size;

  for (i=0;i<Size;i+=16,pmem+=16)
  {
    __asm__(" .set mips3 ");
    __asm__("cache  17, (%0)" : : "r" (pmem));
    __asm__(" .set mips0 ");
  }
}


/**************************************************************************
 * FUNCTION NAME : ti_release_skb
 **************************************************************************
 * DESCRIPTION   :
 *  This function is called from the ti_alloc_skb when there were no more
 *  data buffers available. The allocated SKB had to released back to the 
 *  data pool. The reason why this function was moved from the fast path 
 *  below was because '__skb_queue_head' is an inline function which adds
 *  a large code chunk on the fast path.
 *
 * NOTES         :
 *  This function is called with interrupts disabled. 
 **************************************************************************/
static void ti_release_skb (struct sk_buff_head* list, struct sk_buff* skb)
{
    __skb_queue_head(list, skb);
    return;
}

/**************************************************************************
 * FUNCTION NAME : ti_alloc_skb
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called to allocate memory from the static allocated 
 *  TI-Cached memory pool.
 *
 * RETURNS       :
 *      Allocated static memory buffer - Success
 *      NULL                           - Error.
 **************************************************************************/
struct sk_buff *ti_alloc_skb(unsigned int size,int gfp_mask)
{
    register struct sk_buff*    skb;
    unsigned long               flags;
	struct sk_buff_head*        list;
	DRIVER_BUFFER*	            ptr_node = NULL;

    /* Critical Section Begin: Lock out interrupts. */
    local_irq_save(flags);

    /* Get the SKB Pool list associated with the processor and dequeue the head. */
    list = &skb_head_pool[smp_processor_id()].list;
    skb = __skb_dequeue(list);

    /* Align the data size. */
    size = SKB_DATA_ALIGN(size);

    /* Did we get one. */
    if (skb != NULL)
    {
        /* YES. Now get a data block from the head of statically allocated block. */
        ptr_node = driver_mcb.ptr_available_driver_buffers;
	    if (ptr_node != NULL)
        {
            /* YES. Got a data block. Advance the free list pointer to the next available buffer. */
	        driver_mcb.ptr_available_driver_buffers = ptr_node->ptr_next;
            ptr_node->ptr_next = NULL;

            /* Decrement the number of available data buffers. */
            driver_mcb.num_available_buffers = driver_mcb.num_available_buffers - 1;
        }
        else
        {
          /* NO. Was unable to get a data block. So put the SKB back on the free list. 
           * This is slow path. */
#ifdef DEBUG_SKB 
           printk ("DEBUG: No Buffer memory available: Number of free buffer:%d.\n",
                   driver_mcb.num_available_buffers);
#endif
           ti_release_skb (list, skb);
        }
    }

    /* Critical Section End: Unlock interrupts. */
    local_irq_restore(flags);

    /* Did we get an SKB and data buffer. Proceed only if we were succesful in getting both else drop */
    if (skb != NULL && ptr_node != NULL)
    {
       	/* XXX: does not include slab overhead */ 
        skb->truesize = size + sizeof(struct sk_buff);
 
        /* Load the data pointers. */
     	skb->head = ptr_node->ptr_buffer;
        skb->data = ptr_node->ptr_buffer + MAX_RESERVED_HEADROOM;
	    skb->tail = ptr_node->ptr_buffer + MAX_RESERVED_HEADROOM;
        skb->end  = ptr_node->ptr_buffer + size + MAX_RESERVED_HEADROOM;

	    /* Set up other state */
    	skb->len    = 0;
    	skb->cloned = 0;
        skb->data_len = 0;
    
        /* Mark the SKB indicating that the SKB is from the TI cache. */
        skb->cb[45] = 1;

    	atomic_set(&skb->users, 1); 
    	atomic_set(&(skb_shinfo(skb)->dataref), 1);
        skb_shinfo(skb)->nr_frags = 0;
	    skb_shinfo(skb)->frag_list = NULL;
        return skb;
    }
    else
    {
        /* Control comes here only when there is no statically allocated data buffers
         * available. This case is handled using the mode selected
         *
         * 1. Hybrid Mode.
         *  In that case lets jump to the old allocation code. This way we
         *  can allocate a small number of data buffers upfront and the rest will hit
         *  this portion of the code, which is slow path. Note the number of hits here
         *  should be kept as low as possible to satisfy performance requirements. 
         *
         * 2. Pure Static Mode.
         *  Return NULL the user should have tuned the number of static buffers for
         *  worst case scenario. So return NULL and let the drivers handle the error. */
        if (hybrid_mode == 1)
        {
            /* Hybrid Mode: Old allocation. */
            return dev_alloc_skb(size);
        }
        else
        {
            /* Pure Static Mode: No buffers available. */
            return NULL;
        }
    }
}

/**************************************************************************
 * FUNCTION NAME : ti_skb_release_fragment
 **************************************************************************
 * DESCRIPTION   :
 *  This function is called to release fragmented packets. This is NOT in 
 *  the fast path and this function requires some work. 
 **************************************************************************/
static void ti_skb_release_fragment(struct sk_buff *skb)
{
    if (skb_shinfo(skb)->nr_frags)
    {
        /* PANKAJ TODO: This portion has not been tested. */
	    int i;
#ifdef DEBUG_SKB 
        printk ("DEBUG: Releasing fragments in TI-Cached code.\n");
#endif
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
		    printk ("DEBUG: Fragmented Page = 0x%p.\n", skb_shinfo(skb)->frags[i].page);
    }

    /* Check if there were any fragments present and if so clean all the SKB's. 
     * This is required to recursivly clean the SKB's. */
	if (skb_shinfo(skb)->frag_list)
	    skb_drop_fraglist(skb);

    return;
}

/**************************************************************************
 * FUNCTION NAME : ti_skb_release_data
 **************************************************************************
 * DESCRIPTION   :
 *  The function is called to release the SKB back into the TI-Cached static
 *  memory pool. 
 **************************************************************************/
static void ti_skb_release_data(struct sk_buff *skb)
{
    DRIVER_BUFFER*	ptr_node;
    unsigned long   flags;

    /* The SKB data can be cleaned only if the packet has not been cloned and we
     * are the only one holding a reference to the data. */
	if (!skb->cloned || atomic_dec_and_test(&(skb_shinfo(skb)->dataref)))
    {
        /* Are there any fragments associated with the SKB ?*/
		if ((skb_shinfo(skb)->nr_frags != 0) || (skb_shinfo(skb)->frag_list != NULL))
        {
            /* Slow Path: Try and clean up the fragments. */
            ti_skb_release_fragment (skb);
        }

        /* Cleanup the SKB data memory. This is fast path. */
        ptr_node = (DRIVER_BUFFER *)skb->head;

        /* Critical Section: Lock out interrupts. */
        local_irq_save(flags);

        /* Add the data buffer to the list of available buffers. */
        ptr_node->ptr_next = driver_mcb.ptr_available_driver_buffers;
	    driver_mcb.ptr_available_driver_buffers = ptr_node;

        /* Increment the number of available data buffers. */
        driver_mcb.num_available_buffers = driver_mcb.num_available_buffers + 1;

        /* Criticial Section: Unlock interrupts. */
        local_irq_restore(flags);
	}
    return;
}

#endif /* TI_STATIC_ALLOCATIONS */


/*
 *	Free an skbuff by memory without cleaning the state. 
 */
void kfree_skbmem(struct sk_buff *skb)
{
	skb_release_data(skb);
	skb_head_to_pool(skb);
}

/**
 *	__kfree_skb - private function 
 *	@skb: buffer
 *
 *	Free an sk_buff. Release anything attached to the buffer. 
 *	Clean the state. This is an internal helper function. Users should
 *	always call kfree_skb
 */

void __kfree_skb(struct sk_buff *skb)
{
	if (skb->list) {
	 	printk(KERN_WARNING "Warning: kfree_skb passed an skb still "
		       "on a list (from %p).\n", NET_CALLER(skb));
		BUG();
	}

	dst_release(skb->dst);
	if(skb->destructor) {
#if defined(CONFIG_MIPS_AVALANCHE_SOC)
          /* nothing */
#else 
		if (in_irq()) {
			printk(KERN_WARNING "Warning: kfree_skb on hard IRQ %p\n",
				NET_CALLER(skb));
		}
#endif /* CONFIG_MIPS_AVALANCHE_SOC */
		skb->destructor(skb);
	}
#ifdef CONFIG_NETFILTER
	nf_conntrack_put(skb->nfct);
#endif

    /* NSP Optimization: Any module will call this function to clean the packet memory.
     * Check if the packet belongs to the TI-Cache and if so then put the packet back 
     * into the static memory pool. If not then release into the SLAB. */
#ifdef TI_STATIC_ALLOCATIONS
    /* Check if the SKB is from the TI cache. */
    if (skb->cb[45] == 1)
    {
    	skb_headerinit(skb, NULL, 0);  /* clean state */
	    ti_skb_release_data(skb);
    	skb_head_to_pool(skb);
    }
    else
    {
    	skb_headerinit(skb, NULL, 0);  /* clean state */
	    kfree_skbmem(skb);
    }
#else
    skb_headerinit(skb, NULL, 0);  /* clean state */
	kfree_skbmem(skb);
#endif  /* TI_STATIC_ALLOCATIONS */
}

/**
 *	skb_clone	-	duplicate an sk_buff
 *	@skb: buffer to clone
 *	@gfp_mask: allocation priority
 *
 *	Duplicate an &sk_buff. The new one is not owned by a socket. Both
 *	copies share the same packet data but not structure. The new
 *	buffer has a reference count of 1. If the allocation fails the 
 *	function returns %NULL otherwise the new buffer is returned.
 *	
 *	If this function is called from an interrupt gfp_mask() must be
 *	%GFP_ATOMIC.
 */

struct sk_buff *skb_clone(struct sk_buff *skb, int gfp_mask)
{
	struct sk_buff *n;

	n = skb_head_from_pool();
	if (!n) 
    {
        /* NSP Optimizations: We do not want the SKB's to ever be allocated from the dynamic 
         * memory pool. This causes the SLAB manager to be used. If this happens we can run 
         * into performance problems. Instead everytime print out a message and return ERROR.
         * These messages are important as they need to be monitored to determine MAX. number
         * of buffers that are required. */
		n = kmem_cache_alloc(skbuff_head_cache, gfp_mask);
		if (!n)
      {
			return NULL;
      }
	}

#define C(x) n->x = skb->x

	n->next = n->prev = NULL;
	n->list = NULL;
	n->sk = NULL;
	C(stamp);
	C(dev);
	C(h);
	C(nh);
	C(mac);
	C(dst);
	dst_clone(n->dst);
	memcpy(n->cb, skb->cb, sizeof(skb->cb));
	C(len);
	C(data_len);
	C(csum);
	n->cloned = 1;
	C(pkt_type);
	C(ip_summed);
	C(priority);
	atomic_set(&n->users, 1);
	C(protocol);
	C(security);
	C(truesize);
	C(head);
	C(data);
	C(tail);
	C(end);
	n->destructor = NULL;
#ifdef CONFIG_NETFILTER
	C(nfmark);
	C(nfcache);
	C(nfct);
#ifdef CONFIG_NETFILTER_DEBUG
	C(nf_debug);
#endif
#endif /*CONFIG_NETFILTER*/
#if defined(CONFIG_HIPPI)
	C(private);
#endif
#ifdef CONFIG_NET_SCHED
	C(tc_index);
#endif

#ifdef CONFIG_NET_QOS
	//zhangbin
	C(cos);
#endif
#ifdef CONFIG_VLAN_8021Q
	//junzhao 2004.9.16
	C(vid);
#endif	
	atomic_inc(&(skb_shinfo(skb)->dataref));
	skb->cloned = 1;
#ifdef CONFIG_NETFILTER
	nf_conntrack_get(skb->nfct);
#endif
	return n;
}

static void copy_skb_header(struct sk_buff *new, const struct sk_buff *old)
{
	/*
	 *	Shift between the two data areas in bytes
	 */
	unsigned long offset = new->data - old->data;

	new->list=NULL;
	new->sk=NULL;
	new->dev=old->dev;
	new->priority=old->priority;
	new->protocol=old->protocol;
	new->dst=dst_clone(old->dst);
	new->h.raw=old->h.raw+offset;
	new->nh.raw=old->nh.raw+offset;
	new->mac.raw=old->mac.raw+offset;

    /* PANKAJ: While copying the skb header don't inherit the cb[45]. */
    {
        /* Retain the control buffer value which decides the pool... Just another reason to 
         * use a seperate field in skb than cb[45]. */
        char retained_value = new->cb[45];
    	memcpy(new->cb, old->cb, sizeof(old->cb));
        new->cb[45] = retained_value;
    }

	atomic_set(&new->users, 1);
	new->pkt_type=old->pkt_type;
	new->stamp=old->stamp;
	new->destructor = NULL;
	new->security=old->security;
#ifdef CONFIG_NETFILTER
	new->nfmark=old->nfmark;
	new->nfcache=old->nfcache;
	new->nfct=old->nfct;
	nf_conntrack_get(new->nfct);
#ifdef CONFIG_NETFILTER_DEBUG
	new->nf_debug=old->nf_debug;
#endif
#endif
#ifdef CONFIG_NET_SCHED
	new->tc_index = old->tc_index;
#endif
	
#ifdef CONFIG_NET_QOS
	//zhangbin
	new->cos = old->cos;
#endif
#ifdef CONFIG_VLAN_8021Q
	//junzhao 2004.9.16
	new->vid = old->vid;
#endif
}

/**
 *	skb_copy	-	create private copy of an sk_buff
 *	@skb: buffer to copy
 *	@gfp_mask: allocation priority
 *
 *	Make a copy of both an &sk_buff and its data. This is used when the
 *	caller wishes to modify the data and needs a private copy of the 
 *	data to alter. Returns %NULL on failure or the pointer to the buffer
 *	on success. The returned buffer has a reference count of 1.
 *
 *	As by-product this function converts non-linear &sk_buff to linear
 *	one, so that &sk_buff becomes completely private and caller is allowed
 *	to modify all the data of returned buffer. This means that this
 *	function is not recommended for use in circumstances when only
 *	header is going to be modified. Use pskb_copy() instead.
 */
 
struct sk_buff *skb_copy(const struct sk_buff *skb, int gfp_mask)
{
	struct sk_buff *n;
	int headerlen = skb->data-skb->head;

	/*
	 *	Allocate the copy buffer
	 */
	n=alloc_skb(skb->end - skb->head + skb->data_len, gfp_mask);
	if(n==NULL)
		return NULL;

	/* Set the data pointer */
	skb_reserve(n,headerlen);
	/* Set the tail pointer and length */
	skb_put(n,skb->len);
	n->csum = skb->csum;
	n->ip_summed = skb->ip_summed;

	if (skb_copy_bits(skb, -headerlen, n->head, headerlen+skb->len))
		BUG();

	copy_skb_header(n, skb);

	return n;
}

/* Keep head the same: replace data */
int skb_linearize(struct sk_buff *skb, int gfp_mask)
{
	unsigned int size;
	u8 *data;
	long offset;
	int headerlen = skb->data - skb->head;
	int expand = (skb->tail+skb->data_len) - skb->end;

	if (skb_shared(skb))
		BUG();

	if (expand <= 0)
		expand = 0;

	size = (skb->end - skb->head + expand);
	size = SKB_DATA_ALIGN(size);
	data = kmalloc(size + sizeof(struct skb_shared_info), gfp_mask);
	if (data == NULL)
		return -ENOMEM;

	/* Copy entire thing */
	if (skb_copy_bits(skb, -headerlen, data, headerlen+skb->len))
		BUG();

	/* Offset between the two in bytes */
	offset = data - skb->head;

	/* Free old data. */

    /* PANKAJ: Debugging.... */
    if (skb->cb[45] == 1)
    {
        /* Release the old data back to the pool. */
        ti_skb_release_data(skb);

        /* We have now allocated data from the dynamic memory pool. Use the skb to indicate so. */
        skb->cb[45] = 0;
    }
    else
    {
    	skb_release_data(skb);
    }

	skb->head = data;
	skb->end  = data + size;

	/* Set up new pointers */
	skb->h.raw += offset;
	skb->nh.raw += offset;
	skb->mac.raw += offset;
	skb->tail += offset;
	skb->data += offset;

	/* Set up shinfo */
	atomic_set(&(skb_shinfo(skb)->dataref), 1);
	skb_shinfo(skb)->nr_frags = 0;
	skb_shinfo(skb)->frag_list = NULL;

	/* We are no longer a clone, even if we were. */
	skb->cloned = 0;

	skb->tail += skb->data_len;
	skb->data_len = 0;
	return 0;
}


/**
 *	pskb_copy	-	create copy of an sk_buff with private head.
 *	@skb: buffer to copy
 *	@gfp_mask: allocation priority
 *
 *	Make a copy of both an &sk_buff and part of its data, located
 *	in header. Fragmented data remain shared. This is used when
 *	the caller wishes to modify only header of &sk_buff and needs
 *	private copy of the header to alter. Returns %NULL on failure
 *	or the pointer to the buffer on success.
 *	The returned buffer has a reference count of 1.
 */

struct sk_buff *pskb_copy(struct sk_buff *skb, int gfp_mask)
{
	struct sk_buff *n;

	/*
	 *	Allocate the copy buffer
	 */
	n=alloc_skb(skb->end - skb->head, gfp_mask);
	if(n==NULL)
		return NULL;

	/* Set the data pointer */
	skb_reserve(n,skb->data-skb->head);
	/* Set the tail pointer and length */
	skb_put(n,skb_headlen(skb));
	/* Copy the bytes */
	memcpy(n->data, skb->data, n->len);
	n->csum = skb->csum;
	n->ip_summed = skb->ip_summed;

	n->data_len = skb->data_len;
	n->len = skb->len;

	if (skb_shinfo(skb)->nr_frags) {
		int i;

		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			skb_shinfo(n)->frags[i] = skb_shinfo(skb)->frags[i];
			get_page(skb_shinfo(n)->frags[i].page);
		}
		skb_shinfo(n)->nr_frags = i;
	}

	if (skb_shinfo(skb)->frag_list) {
		skb_shinfo(n)->frag_list = skb_shinfo(skb)->frag_list;
		skb_clone_fraglist(n);
	}

	copy_skb_header(n, skb);

	return n;
}

/**
 *	pskb_expand_head - reallocate header of &sk_buff
 *	@skb: buffer to reallocate
 *	@nhead: room to add at head
 *	@ntail: room to add at tail
 *	@gfp_mask: allocation priority
 *
 *	Expands (or creates identical copy, if &nhead and &ntail are zero)
 *	header of skb. &sk_buff itself is not changed. &sk_buff MUST have
 *	reference count of 1. Returns zero in the case of success or error,
 *	if expansion failed. In the last case, &sk_buff is not changed.
 *
 *	All the pointers pointing into skb header may change and must be
 *	reloaded after call to this function.
 */

int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail, int gfp_mask)
{
	int i;
	u8 *data;
	int size = nhead + (skb->end - skb->head) + ntail;
	long off;

	if (skb_shared(skb))
		BUG();

	size = SKB_DATA_ALIGN(size);

	data = kmalloc(size + sizeof(struct skb_shared_info), gfp_mask);
	if (data == NULL)
		goto nodata;

	/* Copy only real data... and, alas, header. This should be
	 * optimized for the cases when header is void. */
	memcpy(data+nhead, skb->head, skb->tail-skb->head);
	memcpy(data+size, skb->end, sizeof(struct skb_shared_info));

	for (i=0; i<skb_shinfo(skb)->nr_frags; i++)
		get_page(skb_shinfo(skb)->frags[i].page);

	if (skb_shinfo(skb)->frag_list)
		skb_clone_fraglist(skb);

    /* PANKAJ: Debugging.... */
    if (skb->cb[45] == 1)
    {
        /* Release the old data back to the pool. */
        ti_skb_release_data(skb);

        /* We have now allocated data from the dynamic memory pool. Use the skb to indicate so. */
        skb->cb[45] = 0;
    }
    else
    {
    	skb_release_data(skb);
    }

	off = (data+nhead) - skb->head;

	skb->head = data;
	skb->end  = data+size;

	skb->data += off;
	skb->tail += off;
	skb->mac.raw += off;
	skb->h.raw += off;
	skb->nh.raw += off;
	skb->cloned = 0;
	atomic_set(&skb_shinfo(skb)->dataref, 1);
	return 0;

nodata:
	return -ENOMEM;
}

/* Make private copy of skb with writable head and some headroom */

struct sk_buff *
skb_realloc_headroom(struct sk_buff *skb, unsigned int headroom)
{
	struct sk_buff *skb2;
	int delta = headroom - skb_headroom(skb);

	if (delta <= 0)
		return pskb_copy(skb, GFP_ATOMIC);

	skb2 = skb_clone(skb, GFP_ATOMIC);
	if (skb2 == NULL ||
	    !pskb_expand_head(skb2, SKB_DATA_ALIGN(delta), 0, GFP_ATOMIC))
		return skb2;

	kfree_skb(skb2);
	return NULL;
}


/**
 *	skb_copy_expand	-	copy and expand sk_buff
 *	@skb: buffer to copy
 *	@newheadroom: new free bytes at head
 *	@newtailroom: new free bytes at tail
 *	@gfp_mask: allocation priority
 *
 *	Make a copy of both an &sk_buff and its data and while doing so 
 *	allocate additional space.
 *
 *	This is used when the caller wishes to modify the data and needs a 
 *	private copy of the data to alter as well as more space for new fields.
 *	Returns %NULL on failure or the pointer to the buffer
 *	on success. The returned buffer has a reference count of 1.
 *
 *	You must pass %GFP_ATOMIC as the allocation priority if this function
 *	is called from an interrupt.
 */
 

struct sk_buff *skb_copy_expand(const struct sk_buff *skb,
				int newheadroom,
				int newtailroom,
				int gfp_mask)
{
	struct sk_buff *n;

	/*
	 *	Allocate the copy buffer
	 */
 	 
	n=alloc_skb(newheadroom + skb->len + newtailroom,
		    gfp_mask);
	if(n==NULL)
		return NULL;

	skb_reserve(n,newheadroom);

	/* Set the tail pointer and length */
	skb_put(n,skb->len);

	/* Copy the data only. */
	if (skb_copy_bits(skb, 0, n->data, skb->len))
		BUG();

	copy_skb_header(n, skb);
	return n;
}

/* Trims skb to length len. It can change skb pointers, if "realloc" is 1.
 * If realloc==0 and trimming is impossible without change of data,
 * it is BUG().
 */

int ___pskb_trim(struct sk_buff *skb, unsigned int len, int realloc)
{
	int offset = skb_headlen(skb);
	int nfrags = skb_shinfo(skb)->nr_frags;
	int i;

	for (i=0; i<nfrags; i++) {
		int end = offset + skb_shinfo(skb)->frags[i].size;
		if (end > len) {
			if (skb_cloned(skb)) {
				if (!realloc)
					BUG();
				if (!pskb_expand_head(skb, 0, 0, GFP_ATOMIC))
					return -ENOMEM;
			}
			if (len <= offset) {
				put_page(skb_shinfo(skb)->frags[i].page);
				skb_shinfo(skb)->nr_frags--;
			} else {
				skb_shinfo(skb)->frags[i].size = len-offset;
			}
		}
		offset = end;
	}

	if (offset < len) {
		skb->data_len -= skb->len - len;
		skb->len = len;
	} else {
		if (len <= skb_headlen(skb)) {
			skb->len = len;
			skb->data_len = 0;
			skb->tail = skb->data + len;
			if (skb_shinfo(skb)->frag_list && !skb_cloned(skb))
				skb_drop_fraglist(skb);
		} else {
			skb->data_len -= skb->len - len;
			skb->len = len;
		}
	}

	return 0;
}

/**
 *	__pskb_pull_tail - advance tail of skb header 
 *	@skb: buffer to reallocate
 *	@delta: number of bytes to advance tail
 *
 *	The function makes a sense only on a fragmented &sk_buff,
 *	it expands header moving its tail forward and copying necessary
 *	data from fragmented part.
 *
 *	&sk_buff MUST have reference count of 1.
 *
 *	Returns %NULL (and &sk_buff does not change) if pull failed
 *	or value of new tail of skb in the case of success.
 *
 *	All the pointers pointing into skb header may change and must be
 *	reloaded after call to this function.
 */

/* Moves tail of skb head forward, copying data from fragmented part,
 * when it is necessary.
 * 1. It may fail due to malloc failure.
 * 2. It may change skb pointers.
 *
 * It is pretty complicated. Luckily, it is called only in exceptional cases.
 */
unsigned char * __pskb_pull_tail(struct sk_buff *skb, int delta)
{
	int i, k, eat;

	/* If skb has not enough free space at tail, get new one
	 * plus 128 bytes for future expansions. If we have enough
	 * room at tail, reallocate without expansion only if skb is cloned.
	 */
	eat = (skb->tail+delta) - skb->end;

	if (eat > 0 || skb_cloned(skb)) {
		if (pskb_expand_head(skb, 0, eat>0 ? eat+128 : 0, GFP_ATOMIC))
			return NULL;
	}

	if (skb_copy_bits(skb, skb_headlen(skb), skb->tail, delta))
		BUG();

	/* Optimization: no fragments, no reasons to preestimate
	 * size of pulled pages. Superb.
	 */
	if (skb_shinfo(skb)->frag_list == NULL)
		goto pull_pages;

	/* Estimate size of pulled pages. */
	eat = delta;
	for (i=0; i<skb_shinfo(skb)->nr_frags; i++) {
		if (skb_shinfo(skb)->frags[i].size >= eat)
			goto pull_pages;
		eat -= skb_shinfo(skb)->frags[i].size;
	}

	/* If we need update frag list, we are in troubles.
	 * Certainly, it possible to add an offset to skb data,
	 * but taking into account that pulling is expected to
	 * be very rare operation, it is worth to fight against
	 * further bloating skb head and crucify ourselves here instead.
	 * Pure masohism, indeed. 8)8)
	 */
	if (eat) {
		struct sk_buff *list = skb_shinfo(skb)->frag_list;
		struct sk_buff *clone = NULL;
		struct sk_buff *insp = NULL;

		do {
			if (list == NULL)
				BUG();

			if (list->len <= eat) {
				/* Eaten as whole. */
				eat -= list->len;
				list = list->next;
				insp = list;
			} else {
				/* Eaten partially. */

				if (skb_shared(list)) {
					/* Sucks! We need to fork list. :-( */
					clone = skb_clone(list, GFP_ATOMIC);
					if (clone == NULL)
						return NULL;
					insp = list->next;
					list = clone;
				} else {
					/* This may be pulled without
					 * problems. */
					insp = list;
				}
				if (pskb_pull(list, eat) == NULL) {
					if (clone)
						kfree_skb(clone);
					return NULL;
				}
				break;
			}
		} while (eat);

		/* Free pulled out fragments. */
		while ((list = skb_shinfo(skb)->frag_list) != insp) {
			skb_shinfo(skb)->frag_list = list->next;
			kfree_skb(list);
		}
		/* And insert new clone at head. */
		if (clone) {
			clone->next = list;
			skb_shinfo(skb)->frag_list = clone;
		}
	}
	/* Success! Now we may commit changes to skb data. */

pull_pages:
	eat = delta;
	k = 0;
	for (i=0; i<skb_shinfo(skb)->nr_frags; i++) {
		if (skb_shinfo(skb)->frags[i].size <= eat) {
			put_page(skb_shinfo(skb)->frags[i].page);
			eat -= skb_shinfo(skb)->frags[i].size;
		} else {
			skb_shinfo(skb)->frags[k] = skb_shinfo(skb)->frags[i];
			if (eat) {
				skb_shinfo(skb)->frags[k].page_offset += eat;
				skb_shinfo(skb)->frags[k].size -= eat;
				eat = 0;
			}
			k++;
		}
	}
	skb_shinfo(skb)->nr_frags = k;

	skb->tail += delta;
	skb->data_len -= delta;

	return skb->tail;
}

/* Copy some data bits from skb to kernel buffer. */

int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len)
{
	int i, copy;
	int start = skb->len - skb->data_len;

	if (offset > (int)skb->len-len)
		goto fault;

	/* Copy header. */
	if ((copy = start-offset) > 0) {
		if (copy > len)
			copy = len;
		memcpy(to, skb->data + offset, copy);
		if ((len -= copy) == 0)
			return 0;
		offset += copy;
		to += copy;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		int end;

		BUG_TRAP(start <= offset+len);

		end = start + skb_shinfo(skb)->frags[i].size;
		if ((copy = end-offset) > 0) {
			u8 *vaddr;

			if (copy > len)
				copy = len;

			vaddr = kmap_skb_frag(&skb_shinfo(skb)->frags[i]);
			memcpy(to, vaddr+skb_shinfo(skb)->frags[i].page_offset+
			       offset-start, copy);
			kunmap_skb_frag(vaddr);

			if ((len -= copy) == 0)
				return 0;
			offset += copy;
			to += copy;
		}
		start = end;
	}

	if (skb_shinfo(skb)->frag_list) {
		struct sk_buff *list;

		for (list = skb_shinfo(skb)->frag_list; list; list=list->next) {
			int end;

			BUG_TRAP(start <= offset+len);

			end = start + list->len;
			if ((copy = end-offset) > 0) {
				if (copy > len)
					copy = len;
				if (skb_copy_bits(list, offset-start, to, copy))
					goto fault;
				if ((len -= copy) == 0)
					return 0;
				offset += copy;
				to += copy;
			}
			start = end;
		}
	}
	if (len == 0)
		return 0;

fault:
	return -EFAULT;
}

/* Checksum skb data. */

unsigned int skb_checksum(const struct sk_buff *skb, int offset, int len, unsigned int csum)
{
	int i, copy;
	int start = skb->len - skb->data_len;
	int pos = 0;

	/* Checksum header. */
	if ((copy = start-offset) > 0) {
		if (copy > len)
			copy = len;
		csum = csum_partial(skb->data+offset, copy, csum);
		if ((len -= copy) == 0)
			return csum;
		offset += copy;
		pos = copy;
	}

	for (i=0; i<skb_shinfo(skb)->nr_frags; i++) {
		int end;

		BUG_TRAP(start <= offset+len);

		end = start + skb_shinfo(skb)->frags[i].size;
		if ((copy = end-offset) > 0) {
			unsigned int csum2;
			u8 *vaddr;
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			if (copy > len)
				copy = len;
			vaddr = kmap_skb_frag(frag);
			csum2 = csum_partial(vaddr + frag->page_offset +
					     offset-start, copy, 0);
			kunmap_skb_frag(vaddr);
			csum = csum_block_add(csum, csum2, pos);
			if (!(len -= copy))
				return csum;
			offset += copy;
			pos += copy;
		}
		start = end;
	}

	if (skb_shinfo(skb)->frag_list) {
		struct sk_buff *list;

		for (list = skb_shinfo(skb)->frag_list; list; list=list->next) {
			int end;

			BUG_TRAP(start <= offset+len);

			end = start + list->len;
			if ((copy = end-offset) > 0) {
				unsigned int csum2;
				if (copy > len)
					copy = len;
				csum2 = skb_checksum(list, offset-start, copy, 0);
				csum = csum_block_add(csum, csum2, pos);
				if ((len -= copy) == 0)
					return csum;
				offset += copy;
				pos += copy;
			}
			start = end;
		}
	}
	if (len == 0)
		return csum;

	BUG();
	return csum;
}

/* Both of above in one bottle. */

unsigned int skb_copy_and_csum_bits(const struct sk_buff *skb, int offset, u8 *to, int len, unsigned int csum)
{
	int i, copy;
	int start = skb->len - skb->data_len;
	int pos = 0;

	/* Copy header. */
	if ((copy = start-offset) > 0) {
		if (copy > len)
			copy = len;
		csum = csum_partial_copy_nocheck(skb->data+offset, to, copy, csum);
		if ((len -= copy) == 0)
			return csum;
		offset += copy;
		to += copy;
		pos = copy;
	}

	for (i=0; i<skb_shinfo(skb)->nr_frags; i++) {
		int end;

		BUG_TRAP(start <= offset+len);

		end = start + skb_shinfo(skb)->frags[i].size;
		if ((copy = end-offset) > 0) {
			unsigned int csum2;
			u8 *vaddr;
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			if (copy > len)
				copy = len;
			vaddr = kmap_skb_frag(frag);
			csum2 = csum_partial_copy_nocheck(vaddr + frag->page_offset +
						      offset-start, to, copy, 0);
			kunmap_skb_frag(vaddr);
			csum = csum_block_add(csum, csum2, pos);
			if (!(len -= copy))
				return csum;
			offset += copy;
			to += copy;
			pos += copy;
		}
		start = end;
	}

	if (skb_shinfo(skb)->frag_list) {
		struct sk_buff *list;

		for (list = skb_shinfo(skb)->frag_list; list; list=list->next) {
			unsigned int csum2;
			int end;

			BUG_TRAP(start <= offset+len);

			end = start + list->len;
			if ((copy = end-offset) > 0) {
				if (copy > len)
					copy = len;
				csum2 = skb_copy_and_csum_bits(list, offset-start, to, copy, 0);
				csum = csum_block_add(csum, csum2, pos);
				if ((len -= copy) == 0)
					return csum;
				offset += copy;
				to += copy;
				pos += copy;
			}
			start = end;
		}
	}
	if (len == 0)
		return csum;

	BUG();
	return csum;
}

void skb_copy_and_csum_dev(const struct sk_buff *skb, u8 *to)
{
	unsigned int csum;
	long csstart;

	if (skb->ip_summed == CHECKSUM_HW)
		csstart = skb->h.raw - skb->data;
	else
		csstart = skb->len - skb->data_len;

	if (csstart > skb->len - skb->data_len)
		BUG();

	memcpy(to, skb->data, csstart);

	csum = 0;
	if (csstart != skb->len)
		csum = skb_copy_and_csum_bits(skb, csstart, to+csstart,
				skb->len-csstart, 0);

	if (skb->ip_summed == CHECKSUM_HW) {
		long csstuff = csstart + skb->csum;

		*((unsigned short *)(to + csstuff)) = csum_fold(csum);
	}
}

#if 0
/* 
 * 	Tune the memory allocator for a new MTU size.
 */
void skb_add_mtu(int mtu)
{
	/* Must match allocation in alloc_skb */
	mtu = SKB_DATA_ALIGN(mtu) + sizeof(struct skb_shared_info);

	kmem_add_cache_size(mtu);
}
#endif

void __init skb_init(void)
{
	int i;
    struct sk_buff *skb;
   
    char*           ptr_num_buffers;
    int             num_buffers;
    char*           ptr_approach;
    

	skbuff_head_cache = kmem_cache_create("skbuff_head_cache",
					      sizeof(struct sk_buff),
					      0,
					      SLAB_HWCACHE_ALIGN,
					      skb_headerinit, NULL);
	if (!skbuff_head_cache)
		panic("cannot create skbuff cache");

	for (i=0; i<NR_CPUS; i++)
		skb_queue_head_init(&skb_head_pool[i].list);

#ifdef TI_STATIC_ALLOCATIONS
    /* NSP Optimizations: Statically allocate memory for the TI-Cached Memory pool. */

   	/* Initialize the master control block.	*/
	memset ((void *)&driver_mcb, 0 , sizeof(driver_mcb));

    /* Debug Message.. */
    printk ("TI Optimizations: Allocating TI-Cached Memory Pool.\n");

    /* Read the number of buffers from an environment variable. */
    ptr_num_buffers = prom_getenv("StaticBuffer");
    if(ptr_num_buffers)
    {
       num_buffers = simple_strtoul(ptr_num_buffers, (char **)NULL, 10);
    }
    else
    {
       printk ("Warning: Number of buffers is not configured.Setting default to 512\n");
       num_buffers = 512;
    }
    printk ("Using %d Buffers for TI-Cached Memory Pool.\n", num_buffers);

    /* Check the approach. Hybrid or Pure Static. */
    ptr_approach = prom_getenv("StaticMode");
    if (ptr_approach != NULL)
    {
        printk ("DEBUG: Using Pure Static Mode. Make sure there are enough static buffers allocated.\n");
        hybrid_mode = 0;
    }
    else
    {
        printk ("DEBUG: Using Hybrid Mode.\n");
        hybrid_mode = 1;
    }

    /* PANKAJ TODO: Currently this is hardcoded, need to read from an environment 
     * variable. */
    for (i=0; i < num_buffers ; i++)
    {
        /* Allocate memory for the SKB. */
        skb = kmem_cache_alloc(skbuff_head_cache, GFP_KERNEL);
        if (skb == NULL)
        {
            printk ("Error: SKBuff allocation failed for SKB: %d.\n", i);
            return;
        }
        
        /* Add the SKB to the pool. */
        skb_head_to_pool(skb);
    }

	/* Initialize the master control block.	*/
	memset ((void *)&driver_mcb, 0 , sizeof(driver_mcb));

    /* Allocate memory for the TI-Cached Data buffers. */
	for (i=0; i< num_buffers ; i++)
	{
		DRIVER_BUFFER*	ptr_driver_buffer;

		/* Allocate memory for the driver buffer. */
		ptr_driver_buffer = (DRIVER_BUFFER *)kmalloc((sizeof(DRIVER_BUFFER)),GFP_KERNEL);
		if (ptr_driver_buffer == NULL)
		{
			printk ("Error: Unable to allocate memory for the driver buffers Failed:%d.\n",i);
			return;
		}

		/* Initialize the allocated block of memory. */
		memset ((void *)ptr_driver_buffer, 0, sizeof(DRIVER_BUFFER));

		/* Add the allocated block to the list of available blocks */
		if (driver_mcb.ptr_available_driver_buffers == NULL)
		{
			driver_mcb.ptr_available_driver_buffers = ptr_driver_buffer;
		}
		else
		{
			ptr_driver_buffer->ptr_next = driver_mcb.ptr_available_driver_buffers;
			driver_mcb.ptr_available_driver_buffers = ptr_driver_buffer;
		}

        /* Increment the number of available data buffers. */
        driver_mcb.num_available_buffers = driver_mcb.num_available_buffers + 1;

        /* Mick: Cache Invalidate. */
        skbuff_data_invalidate ((void *)ptr_driver_buffer->ptr_buffer,
                                MAX_SIZE_STATIC_BUFFER + MAX_RESERVED_HEADROOM + 60);
	}
    printk ("NSP Optimizations: Succesfully allocated TI-Cached Memory Pool.\n");

#endif /* TI_STATIC_ALLOCATIONS */

    return;
}
