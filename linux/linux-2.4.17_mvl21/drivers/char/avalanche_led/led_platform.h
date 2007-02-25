/******************************************************************************    
 * FILE PURPOSE:     - LED Platform specific Header file                                      
 ******************************************************************************    
 * FILE NAME:     led_platform.h                                                        
 *                                                                                 
 * DESCRIPTION:   Linux specific implementation for OS abstracted function calls 
 *                made by LED HAL module. This file has functions defined for
 *                Memory allocation calls, Mutex calls, String and Timer 
 *                operations.
 *                                                                 
 * REVISION HISTORY:                                                               
 * 11 Oct 03 - PSP TII                                                             
 *                                                                                 
 * (C) Copyright 2002, Texas Instruments, Inc                                      
 *******************************************************************************/  
                                                                                   
#ifndef __LED_PLATFORM__
#define __LED_PLATFORM__

#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/string.h>
#include <linux/param.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/spinlock.h>


#define os_malloc(x)         kmalloc(x,GFP_KERNEL)
#define os_memset            memset
#define os_free(x)           kfree(x)
#define os_strcmp            os_strcasecmp
#define os_strcpy            strcpy

#if defined(DEBUG)
#define log_msg              printk
#else
#define log_msg(x)
#endif

/*    defines for Mutex        */
typedef struct {
    spinlock_t lock;
    int flags;
}OS_SPINLOCK_T;

#define MUTEX_DECLARE(x)   static  OS_SPINLOCK_T	x
#define MUTEX_INIT(x)      x.lock = SPIN_LOCK_UNLOCKED
#define MUTEX_GET(x)       spin_lock_irqsave(&x.lock, x.flags)
#define MUTEX_RELEASE(x)   spin_unlock_irqrestore(&x.lock, x.flags)         



/* String handling functions  not defined in asm/string.h */
static __inline__ int os_strlen(char *str)
{
    int i;
    for(i=0;str[i];i++);
    return i;
}


#define LOWER(x)    ((x < 'a') ? (x - 'A' + 'a'):(x))                                    
#define ISALPHA(x)   ((( x >= 'a') && (x <= 'z')) || (( x >= 'A') && (x <= 'Z')))        
#define COMP(x,y)   ((x == y) || ((ISALPHA(x) && ISALPHA(y)) && (LOWER(x) == LOWER(y)))) 


static __inline__ int os_strcasecmp(char *str1, char *str2)
{                                         
    int i;                            
    
    for(i=0;str1[i] && str2[i];i++)
    {
        char x,y;
        
        x = str1[i];
        y = str2[i];

        if(!COMP(x,y))
            break;
    }              

    return(str1[i] || str2[i]);
}                                         
                                          


/* Functions for timer related operations */
static __inline__ void * os_timer_init(void (*func)(int))
{
    struct timer_list *ptimer;
    ptimer = (struct timer_list *) kmalloc(sizeof(struct timer_list),GFP_KERNEL);
    init_timer( ptimer );
    (void *)ptimer->function = (void *)func;
    return (void *)ptimer;
}

static __inline__ int os_timer_add(void *timer_handle,int milisec,int arg)
{
    struct timer_list *ptimer=timer_handle;
    ptimer->expires = ((HZ * milisec)/1000) + jiffies;
    ptimer->data = arg;
    add_timer(ptimer);
    return 0;

}

static __inline__ int os_timer_delete(void *timer_handle)
{
    struct timer_list *ptimer=timer_handle;
    del_timer(ptimer);
    kfree(ptimer);
    return 0;
}


#endif /* __LED_PLATFORM__ */
