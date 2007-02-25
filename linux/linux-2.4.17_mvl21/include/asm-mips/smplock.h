/* $Id: smplock.h,v 1.1.1.2 2005/03/28 06:56:46 sparq Exp $
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Default SMP lock implementation
 */
#include <linux/config.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

extern spinlock_t kernel_flag;

#ifdef CONFIG_PREEMPT_TIMES
#define lock_bkl() do { \
	current->preempt_count++;  \
	preempt_lock_start(3); \
	_raw_spin_lock(&kernel_flag); \
} while (0)
#else
#define lock_bkl() spin_lock(&kernel_flag)
#endif
#define unlock_bkl() spin_unlock(&kernel_flag)

#ifdef CONFIG_SMP
#define kernel_locked()		spin_is_locked(&kernel_flag)
#else
#ifdef CONFIG_PREEMPT
#define kernel_locked()         preempt_is_disabled()
#else
#define kernel_locked()         1
#endif
#endif

/*
 * Release global kernel lock and global interrupt lock
 */
#define release_kernel_lock(task, cpu) \
do { \
	if (task->lock_depth >= 0) \
		spin_unlock(&kernel_flag); \
	release_irqlock(cpu); \
	__sti(); \
} while (0)

/*
 * Re-acquire the kernel lock
 */
#define reacquire_kernel_lock(task) \
do { \
	if (task->lock_depth >= 0) \
		spin_lock(&kernel_flag); \
} while (0)

#ifdef CONFIG_PREEMPT_TIMES
#define lock_kernel() do { \
        if (current->lock_depth == -1) \
                lock_bkl(); \
        ++current->lock_depth; \
} while (0)

#define unlock_kernel() do { \
        if (--current->lock_depth < 0) \
                unlock_bkl(); \
} while (0)

#else	/* CONFIG_PREEMPT_TIMES */

/*
 * Getting the big kernel lock.
 *
 * This cannot happen asynchronously,
 * so we only need to worry about other
 * CPU's.
 */
extern __inline__ void lock_kernel(void)
{
#ifdef CONFIG_PREEMPT
	if (current->lock_depth == -1)
		spin_lock(&kernel_flag);
	++current->lock_depth;
#else
	if (!++current->lock_depth)
		spin_lock(&kernel_flag);
#endif
}

extern __inline__ void unlock_kernel(void)
{
	if (--current->lock_depth < 0)
		spin_unlock(&kernel_flag);
}
#endif	/* CONFIG_PREEMPT_TIMES */
