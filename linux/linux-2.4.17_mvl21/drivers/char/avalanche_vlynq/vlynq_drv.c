/******************************************************************************
 * FILE PURPOSE:    Vlynq Linux Device Driver Source
 ******************************************************************************
 * FILE NAME:       vlynq_drv.c
 *
 * DESCRIPTION:     Vlynq Linux Device Driver Source
 *
 * REVISION HISTORY:
 *
 * Date           Description                       Author
 *-----------------------------------------------------------------------------
 * 17 July 2003   Initial Creation                  Anant Gole
 * 17 Dec  2003   Updates                           Sharath Kumar
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 *******************************************************************************/
 
#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/smp_lock.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/capability.h>
#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/generic/avalanche_intc.h>


#define    TI_VLYNQ_VERSION                 "0.2"

/* debug on ? */
#define VLYNQ_DEBUG 

/* Macro for debug and error printf's */
#ifdef VLYNQ_DEBUG
#define DBGPRINT  printk
#else
#define DBGPRINT(x)  
#endif

#define ERRPRINT  printk


/* include Vlynq HAL header file */
#include "vlynq_hal.h" 

/* Define the max vlynq ports this driver will support. 
   Device name strings are statically added here */
#define MAX_VLYNQ_PORTS 2


/* Type define for VLYNQ private structure */
typedef struct vlynqPriv{
    int irq;
    VLYNQ_DEV *vlynqDevice;
}VLYNQ_PRIV;

extern int vlynq_init_status[2];

/* Extern Global variable for vlynq devices used in initialization of the vlynq device
 * These variables need to be populated/initialized by the system as part of initialization
 * process. The vlynq enumerator can run at initialization and populate these globals
 */

VLYNQ_DEV vlynqDevice0;
VLYNQ_DEV vlynqDevice1;

/* Defining dummy macro AVALANCHE_HIGH_VLYNQ_INT to take
 * care of compilation in case of single vlynq device 
 */

#ifndef AVALANCHE_HIGH_VLYNQ_INT
#define  AVALANCHE_HIGH_VLYNQ_INT 0
#endif



/* vlynq private object */
VLYNQ_PRIV vlynq_priv[CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS] = {
    { LNXINTNUM(AVALANCHE_LOW_VLYNQ_INT),&vlynqDevice0},
    { LNXINTNUM(AVALANCHE_HIGH_VLYNQ_INT),&vlynqDevice1},
};




/* =================================== all the operations */

static int
vlynq_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static struct file_operations vlynq_fops = {
    owner:      THIS_MODULE,
    ioctl:      vlynq_ioctl,
};

/* Vlynq device object */
static struct miscdevice vlynq_dev [MAX_VLYNQ_PORTS] = {
    { MISC_DYNAMIC_MINOR , "vlynq0", &vlynq_fops },
    { MISC_DYNAMIC_MINOR , "vlynq1", &vlynq_fops },
};


/* Proc read function */
static int
vlynq_read_link_proc(char *buf, char **start, off_t offset, int count, int *eof, void *unused)
{
    int instance;
    int len = 0;
 
    len += sprintf(buf +len,"VLYNQ Devices : %d\n",CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS);

    for(instance =0;instance < CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS;instance++)
    {
        int link_state;
        char *link_msg[] = {" DOWN "," UP "};
       
        if(vlynq_init_status[instance] == 0)
            link_state = 0; 

        else if (vlynq_link_check(vlynq_priv[instance].vlynqDevice))
            link_state = 1;

        else
            link_state = 0;    

        len += sprintf(buf + len, "VLYNQ %d: Link state: %s\n",instance,link_msg[link_state]);

    }
    /* Print info about vlynq device 1 */
   
    return len;
}


/* Proc function to display driver version */                                                                       
static int                                                                                     
vlynq_read_ver_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)        
{                                                                                              
	int instance;                                                                              
	int len=0;                                                                                 
                                                                                               
	len += sprintf(buf +len,"\nTI Linux VLYNQ Driver Version %s\n",TI_VLYNQ_VERSION);         
	return len;                                                                                
}                                                                                              




/* Wrapper for vlynq ISR */
static void lnx_vlynq_root_isr(int irq, void * arg, struct pt_regs *regs)
{
   vlynq_root_isr(arg);
}

/* =================================== init and cleanup */

int vlynq_init_module(void)
{
    int ret;
    int unit = 0;
    int instance_count = CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS;
    volatile int *ptr;


    DBGPRINT("Vlynq CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS=%d\n", CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS);
    /* If num of configured vlynq ports > supported by driver return error */
    if (instance_count > MAX_VLYNQ_PORTS)
    {
        ERRPRINT("ERROR: vlynq_init_module(): Max %d supported\n", MAX_VLYNQ_PORTS);
        return (-1);
    }

    /* register the misc device */
    for (unit = 0; unit < CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS; unit++)
    {
        ret = misc_register(&vlynq_dev[unit]);

        if(ret < 0)
        {
            ERRPRINT("ERROR:Could not register vlynq device:%d\n",unit);
            continue;
        }
        else 
            DBGPRINT("Vlynq Device %s registered with minor no %d as misc device. Result=%d\n", 
                vlynq_dev[unit].name, vlynq_dev[unit].minor, ret);
#if 0
            
        DBGPRINT("Calling vlynq init\n");

        /* Read the global variable for VLYNQ device structure and initialize vlynq driver */
        ret = vlynq_init(vlynq_priv[unit].vlynqDevice,VLYNQ_INIT_PERFORM_ALL );
#endif

        if(vlynq_init_status[unit] == 0)
        {
            printk("VLYNQ %d : init failed\n",unit); 
            continue;
        }
         
        /* Check link before proceeding */
        if (!vlynq_link_check(vlynq_priv[unit].vlynqDevice))
        {
 	    DBGPRINT("\nError: Vlynq link not available.trying once before  Exiting");
        }
        else
        {
            DBGPRINT("Vlynq instance:%d Link UP\n",unit);
        
            /* Install the vlynq local root ISR */
           request_irq(vlynq_priv[unit].irq,lnx_vlynq_root_isr,0,vlynq_dev[unit].name,vlynq_priv[unit].vlynqDevice);
        } 
    }

    /* Creating proc entry for the devices */
    create_proc_read_entry("avalanche/vlynq_link", 0, NULL, vlynq_read_link_proc, NULL);
    create_proc_read_entry("avalanche/vlynq_ver", 0, NULL, vlynq_read_ver_proc, NULL);
  
    return 0;
}

void vlynq_cleanup_module(void)
{
    int unit = 0;
    
    for (unit = 0; unit < CONFIG_MIPS_AVALANCHE_VLYNQ_PORTS; unit++)
    {
        DBGPRINT("vlynq_cleanup_module(): Unregistring misc device %s\n",vlynq_dev[unit].name);
        misc_deregister(&vlynq_dev[unit]);
    }

    remove_proc_entry("avalanche/vlynq_link", NULL);
    remove_proc_entry("avalanche/vlynq_ver", NULL);
}


module_init(vlynq_init_module);
module_exit(vlynq_cleanup_module);

