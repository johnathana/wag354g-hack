#ifndef __PAL_VLYNQ_OS_H__
#define __PAL_VLYNQ_OS_H__

#include <linux/kernel.h>
#include <asm/string.h>
#include <pal.h>

/* To keep the ISR dispatch simple we align the signature of the ISR
 * to that stipulated by the OS.
 */
typedef void (*PAL_VLYNQ_DEV_ISR_FN)(int, void*, struct pt_regs*);
typedef struct pal_vlynq_dev_isr_param_grp_t
{
    int  arg1;
    void *arg2;
    void *arg3;

} PAL_VLYNQ_DEV_ISR_PARAM_GRP_T;

#define PAL_VLYNQ_DEV_ISR_PARM_NUM 3

#ifdef AVALANCHE_LOW_VLYNQ_CONTROL_BASE
#define LOW_VLYNQ_COUNT 1
#else
#define LOW_VLYNQ_COUNT 0
#endif

#ifdef AVALANCHE_HIGH_VLYNQ_CONTROL_BASE
#define HIGH_VLYNQ_COUNT 1
#else
#define HIGH_VLYNQ_COUNT 0
#endif

#define MAX_ROOT_VLYNQ (LOW_VLYNQ_COUNT + HIGH_VLYNQ_COUNT)

/* Board dependent, to be moved to board configuration, but later. */
#define MAX_VLYNQ_COUNT 4 

#endif
