/***************************************************************************
**+----------------------------------------------------------------------+**
**|                                ****                                  |**
**|                                ****                                  |**
**|                                ******o***                            |**
**|                          ********_///_****                           |**
**|                           ***** /_//_/ ****                          |**
**|                            ** ** (__/ ****                           |**
**|                                *********                             |**
**|                                 ****                                 |**
**|                                  ***                                 |**
**|                                                                      |**
**|     Copyright (c) 2003 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                           |**
**|                                                                      |**
**| Permission is hereby granted to licensees of Texas Instruments       |**
**| Incorporated (TI) products to use this computer program for the sole |**
**| purpose of implementing a licensee product based on TI products.     |**
**| No other rights to reproduce, use, or disseminate this computer      |**
**| program, whether in part or in whole, are granted.                   |**
**|                                                                      |**
**| TI makes no representation or warranties with respect to the         |**
**| performance of this computer program, and specifically disclaims     |**
**| any responsibility for any damages, special or consequential,        |**
**| connected with the use of this program.                              |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/

/***************************************************************************
 *  ------------------------------------------------------------------------------
 *   Module      : vlynq_hal.c
 *   Description : This file implements VLYNQ HAL API.
 *  ------------------------------------------------------------------------------
 ***************************************************************************/


#include "vlynq_hal.h"

/**** Local Function prototypes *******/
PRIVATE INT32 vlynqInterruptInit(VLYNQ_DEV *pdev);
PRIVATE void  vlynq_configClock(VLYNQ_DEV  *pdev);

/*** Second argument must be explicitly type casted to 
 * (VLYNQ_DEV*) inside the following functions */
PRIVATE void vlynq_local_module_isr(void *arg1, void *arg2, void *arg3);
PRIVATE void vlynq_remote_module_isr(void *arg1, void *arg2, void *arg3);


volatile int vlynq_delay_value = 0;

/* Code adopted from original vlynq driver */
void vlynq_delay(UINT32 clktime)
{
    int i = 0;
    volatile int    *ptr = &vlynq_delay_value;
    *ptr = 0;

    /* We are assuming that the each cycle takes about 
     * 23 assembly instructions. */
    for(i = 0; i < (clktime + 23)/23; i++)
    {
        *ptr = *ptr + 1;
    }
}


/* ----------------------------------------------------------------------------
 *  Function : vlynq_configClock()
 *  Description: Configures clock settings based on input parameters
 *  Adapted from original vlyna driver from Cable
 */
PRIVATE void vlynq_configClock(VLYNQ_DEV * pdev)
{
    UINT32 tmp;

    switch( pdev->clk_source)
    {
        case VLYNQ_CLK_SOURCE_LOCAL:  /* we output the clock, clk_div in range [1..8]. */
            tmp = ((pdev->clk_div - 1) << 16) |  VLYNQ_CTL_CLKDIR_MASK ;
            VLYNQ_CTRL_REG = tmp;
            VLYNQ_R_CTRL_REG = 0ul;
            break;
        case VLYNQ_CLK_SOURCE_REMOTE: /* we need to set the clock pin as input */
            VLYNQ_CTRL_REG = 0ul;
            tmp = ((pdev->clk_div - 1) << 16) |  VLYNQ_CTL_CLKDIR_MASK ;
            VLYNQ_R_CTRL_REG = tmp;
            break;
        default:   /* do nothing about the clock, but clear other bits. */
            tmp = ~(VLYNQ_CTL_CLKDIR_MASK | VLYNQ_CTL_CLKDIV_MASK);
            VLYNQ_CTRL_REG &= tmp;
            break;
   }
}

 /* ----------------------------------------------------------------------------
 *  Function : vlynq_link_check()
 *  Description: This function checks the current VLYNQ for a link.
 *  An arbitrary amount of time is allowed for the link to come up .
 *  Returns 0 for "no link / failure " and 1 for "link available".
 * -----------------------------------------------------------------------------
 */
GLOBAL UINT32 vlynq_link_check( VLYNQ_DEV * pdev)
{
    /*sleep for 64 cycles, allow link to come up*/
    vlynq_delay(64);  
      
    /* check status register return OK if link is found. */
    if (VLYNQ_STATUS_REG & VLYNQ_STS_LINK_MASK) 
    {
        return 1;   /* Link Available */
    }
    else
    {
        return 0;   /* Link Failure */
    }
}

/* ----------------------------------------------------------------------------
 *  Function : vlynq_init()
 *  Description: Initialization function accepting paramaters for VLYNQ module
 *  initialization. The Options bitmap decides what operations are performed
 *  as a part of initialization. The Input parameters  are obtained through the
 *  sub fields of VLYNQ_DEV structure.
 */

GLOBAL INT32 vlynq_init(VLYNQ_DEV *pdev, VLYNQ_INIT_OPTIONS options)
{
    UINT32 map;
    UINT32 val=0,cnt,tmp;
    UINT32 counter=0;
    VLYNQ_INTERRUPT_CNTRL *intSetting=NULL;

    /* validate arguments */
    if( VLYNQ_OUTRANGE(pdev->clk_source, VLYNQ_CLK_SOURCE_REMOTE, VLYNQ_CLK_SOURCE_NONE) || 
        VLYNQ_OUTRANGE(pdev->clk_div, 8, 1) )
    {
      return VLYNQ_INVALID_ARG;
    }
   
    /** perform all sanity checks first **/
    if(pdev->state != VLYNQ_DRV_STATE_UNINIT)
        return VLYNQ_INVALID_DRV_STATE;

    /** Initialize local and remote register set addresses- additional
     * provision to access the registers directly if need be */
    pdev->local = (VLYNQ_REG_SET*)pdev->module_base;
    pdev->remote = (VLYNQ_REG_SET*) (pdev->module_base + VLYNQ_REMOTE_REGS_OFFSET);

    /* Detect faulty int configuration that might induce int pkt looping */
    if ( (options  & VLYNQ_INIT_LOCAL_INTERRUPTS) && (options & VLYNQ_INIT_REMOTE_INTERRUPTS) )
    {
        /* case when both local and remote are configured */
        if((pdev->local_irq.intLocal== VLYNQ_INT_REMOTE )  /* interrupts transfered to remote from local */
        && (pdev->remote_irq.intLocal== VLYNQ_INT_REMOTE)  /* interrupts transfered from remote to local */
        && ((pdev->local_irq.intRemote == VLYNQ_INT_ROOT_ISR) || (pdev->remote_irq.intRemote == VLYNQ_INT_ROOT_ISR)) )
        {
            return (VLYNQ_INT_CONFIG_ERR); 
        }
    }

    pdev->state = VLYNQ_DRV_STATE_ININIT;
    pdev->intCount = 0;
    pdev->isrCount = 0;

    /*** Its assumed that the vlynq module  has been brought out of reset
     * before invocation of vlynq_init. Since, this operation is board specific
     * it must be handled outside this generic driver */
   
    /* Assert reset the remote device, call reset_cb,
     * reset CB holds Reset according to the device needs. */
    VLYNQ_RESETCB(VLYNQ_RESET_ASSERT);

    /* Handle VLYNQ clock, HW default (Sense On Reset) is
     * usually input for all the devices. */        
    if (options & VLYNQ_INIT_CONFIG_CLOCK)
    {
        vlynq_configClock(pdev);
    }
 
    /* Call reset_cb again. It will release the remote device 
     * from reset, and wait for a while. */
    VLYNQ_RESETCB(VLYNQ_RESET_DEASSERT);

    if(options & VLYNQ_INIT_CHECK_LINK )
    {
        /* Check for link up during initialization*/
	while( counter < 25 )
	{
	/* loop around giving a chance for link status to settle down */
	counter++;
        if(vlynq_link_check(pdev))
        {
           /* Link is up exit loop*/
	   break;
        }

	vlynq_delay(4000);
	}/*end of while counter loop */

        if(!vlynq_link_check(pdev))
        {
            /* Handle this case as abort */
            pdev->state = VLYNQ_DRV_STATE_ERROR;
            VLYNQ_RESETCB( VLYNQ_RESET_INITFAIL);
            return VLYNQ_LINK_DOWN;
        }/* end of if not vlynq_link_check conditional block */
      
    }/*end of if options & VLYNQ_INIT_CHECK_LINK conditional block */


    if (options & VLYNQ_INIT_LOCAL_MEM_REGIONS)
    {
        /* Initialise local memory regions . This initialization lets
         * the local host access remote device memory regions*/   
        int i; 

        /* configure the VLYNQ portal window to a PHYSICAL
         * address of the local CPU */
        VLYNQ_ALIGN4(pdev->local_mem.Txmap);
        VLYNQ_TXMAP_REG = (pdev->local_mem.Txmap); 
        
        /*This code assumes input parameter is itself a physical address */
        for(i=0; i < VLYNQ_MAX_MEMORY_REGIONS ; i++)
        {
            /* Physical address on the remote */
            map = i+1;
            VLYNQ_R_RXMAP_SIZE_REG(map) =  0;
            if( pdev->remote_mem.RxSize[i])
            {
                VLYNQ_ALIGN4(pdev->remote_mem.RxOffset[i]);            
                VLYNQ_ALIGN4(pdev->remote_mem.RxSize[i]);
                VLYNQ_R_RXMAP_OFFSET_REG(map) = pdev->remote_mem.RxOffset[i];
                VLYNQ_R_RXMAP_SIZE_REG(map) = pdev->remote_mem.RxSize[i];
            }
        }
    }

    if(options & VLYNQ_INIT_REMOTE_MEM_REGIONS )
    {
        int i;

        /* Initialise remote memory regions. This initialization lets remote
         * device access local host memory regions. It configures the VLYNQ portal
         * window to a PHYSICAL address of the remote */
        VLYNQ_ALIGN4(pdev->remote_mem.Txmap);            
        VLYNQ_R_TXMAP_REG = pdev->remote_mem.Txmap;
       
        for( i=0; i<VLYNQ_MAX_MEMORY_REGIONS; i++)
        {
            /* Physical address on the local */
            map = i+1;
            VLYNQ_RXMAP_SIZE_REG(map) =  0;
            if( pdev->local_mem.RxSize[i])
            {
                VLYNQ_ALIGN4(pdev->local_mem.RxOffset[i]);            
                VLYNQ_ALIGN4(pdev->local_mem.RxSize[i]);
                VLYNQ_RXMAP_OFFSET_REG(map) =  (pdev->local_mem.RxOffset[i]);
                VLYNQ_RXMAP_SIZE_REG(map) =  (pdev->local_mem.RxSize[i]);
            }
        }
    }

    /* Adapted from original vlynq driver from cable - Calculate VLYNQ bus width */
    pdev->width = 3 +  VLYNQ_STATUS_FLD_WIDTH(VLYNQ_STATUS_REG) 
                  + VLYNQ_STATUS_FLD_WIDTH(VLYNQ_R_STATUS_REG);
   
    /* chance to initialize the device, e.g. to boost VLYNQ 
     * clock by modifying pdev->clk_div or and verify the width. */
    VLYNQ_RESETCB(VLYNQ_RESET_LINKESTABLISH);

    /* Handle VLYNQ clock, HW default (Sense On Reset) is
     * usually input for all the devices. */
    if(options & VLYNQ_INIT_CONFIG_CLOCK )
    {
        vlynq_configClock(pdev);
    }

    /* last check for link*/
    if(options & VLYNQ_INIT_CHECK_LINK )
    {
     /* Final Check for link during initialization*/
	while( counter < 25 )
	{
	/* loop around giving a chance for link status to settle down */
	counter++;
        if(vlynq_link_check(pdev))
        {
           /* Link is up exit loop*/
	   break;
        }

	vlynq_delay(4000);
	}/*end of while counter loop */

        if(!vlynq_link_check(pdev))
        {
            /* Handle this case as abort */
            pdev->state = VLYNQ_DRV_STATE_ERROR;
            VLYNQ_RESETCB( VLYNQ_RESET_INITFAIL);
            return VLYNQ_LINK_DOWN;
        }/* end of if not vlynq_link_check conditional block */
        
    } /* end of if options & VLYNQ_INIT_CHECK_LINK */

    if(options & VLYNQ_INIT_LOCAL_INTERRUPTS )
    {
        /* Configure local interrupt settings */
        intSetting = &(pdev->local_irq);

        /* Map local module status interrupts to interrupt vector*/
        val = intSetting->map_vector << VLYNQ_CTL_INTVEC_SHIFT ;
      
        /* enable local module status interrupts */
        val |= 0x01 << VLYNQ_CTL_INTEN_SHIFT;
      
        if ( intSetting->intLocal == VLYNQ_INT_LOCAL )
        {
            /*set the intLocal bit*/
            val |= 0x01 << VLYNQ_CTL_INTLOCAL_SHIFT;
        }
      
        /* Irrespective of whether interrupts are handled locally, program
         * int2Cfg. Error checking for accidental loop(when intLocal=0 and int2Cfg=1
         * i.e remote packets are set intPending register->which will result in 
         * same packet being sent out) has been done already
         */
      
        if (intSetting->intRemote == VLYNQ_INT_ROOT_ISR) 
        {
            /* Set the int2Cfg register, so that remote interrupt
             * packets are written to intPending register */
            val |= 0x01 << VLYNQ_CTL_INT2CFG_SHIFT;
    
            /* Set intPtr register to point to intPending register */
            VLYNQ_INT_PTR_REG = VLYNQ_INT_PENDING_REG_PTR ;
        }
        else
        {
            /*set the interrupt pointer register*/
            VLYNQ_INT_PTR_REG = intSetting->intr_ptr;
            /* Dont bother to modify int2Cfg as it would be zero */
        }

        /** Clear bits related to INT settings in control register **/
        VLYNQ_CTRL_REG = VLYNQ_CTRL_REG & (~VLYNQ_CTL_INTFIELDS_CLEAR_MASK);
      
        /** Or the bits to be set with Control register **/
        VLYNQ_CTRL_REG = VLYNQ_CTRL_REG | val;

        /* initialise local ICB */          
        if(vlynqInterruptInit(pdev)==VLYNQ_MEMALLOC_FAIL)
            return VLYNQ_MEMALLOC_FAIL;   

        /* Install handler for local module status interrupts. By default when 
         * local interrupt setting is initialised, the local module status are 
         * enabled and handler hooked up */
        if(vlynq_install_isr(pdev, intSetting->map_vector, vlynq_local_module_isr, 
                             pdev, NULL, NULL) == VLYNQ_INVALID_ARG)
            return VLYNQ_INVALID_ARG;
    } /* end of init local interrupts */

    if(options & VLYNQ_INIT_REMOTE_INTERRUPTS )
    {
        /* Configure remote interrupt settings from configuration */          
        intSetting = &(pdev->remote_irq);

        /* Map remote module status interrupts to remote interrupt vector*/
        val = intSetting->map_vector << VLYNQ_CTL_INTVEC_SHIFT ;
        /* enable remote module status interrupts */
        val |= 0x01 << VLYNQ_CTL_INTEN_SHIFT;
      
        if ( intSetting->intLocal == VLYNQ_INT_LOCAL )
        {
            /*set the intLocal bit*/
            val |= 0x01 << VLYNQ_CTL_INTLOCAL_SHIFT;
        }

        /* Irrespective of whether interrupts are handled locally, program
         * int2Cfg. Error checking for accidental loop(when intLocal=0 and int2Cfg=1
         * i.e remote packets are set intPending register->which will result in 
         * same packet being sent out) has been done already
        */ 

        if (intSetting->intRemote == VLYNQ_INT_ROOT_ISR) 
        {
            /* Set the int2Cfg register, so that remote interrupt
             * packets are written to intPending register */
            val |= 0x01 << VLYNQ_CTL_INT2CFG_SHIFT;
            /* Set intPtr register to point to intPending register */
            VLYNQ_R_INT_PTR_REG = VLYNQ_R_INT_PENDING_REG_PTR ;
        }
        else
        {
            /*set the interrupt pointer register*/
            VLYNQ_R_INT_PTR_REG = intSetting->intr_ptr;
            /* Dont bother to modify int2Cfg as it would be zero */
        }
    
        if( (intSetting->intLocal == VLYNQ_INT_REMOTE) && 
            (options & VLYNQ_INIT_LOCAL_INTERRUPTS) &&
            (pdev->local_irq.intRemote == VLYNQ_INT_ROOT_ISR) )
        {
            /* Install handler for remote module status interrupts. By default when 
             * remote interrupts are forwarded to local root_isr then remote_module_isr is
             * enabled and handler hooked up */
            if(vlynq_install_isr(pdev,intSetting->map_vector,vlynq_remote_module_isr,
                                 pdev, NULL, NULL) == VLYNQ_INVALID_ARG)
                return VLYNQ_INVALID_ARG;
        }

         
        /** Clear bits related to INT settings in control register **/
        VLYNQ_R_CTRL_REG = VLYNQ_R_CTRL_REG & (~VLYNQ_CTL_INTFIELDS_CLEAR_MASK);
      
        /** Or the bits to be set with the remote Control register **/
        VLYNQ_R_CTRL_REG = VLYNQ_R_CTRL_REG | val;
         
    } /* init remote interrupt settings*/

    if(options & VLYNQ_INIT_CLEAR_ERRORS )
    {
        /* Clear errors during initialization */
        tmp = VLYNQ_STATUS_REG  & (VLYNQ_STS_RERROR_MASK | VLYNQ_STS_LERROR_MASK);
        VLYNQ_STATUS_REG = tmp;
        tmp = VLYNQ_R_STATUS_REG & (VLYNQ_STS_RERROR_MASK | VLYNQ_STS_LERROR_MASK);
        VLYNQ_R_STATUS_REG = tmp;
    } 

    /* clear int status */
    val = VLYNQ_INT_STAT_REG;
    VLYNQ_INT_STAT_REG = val;
    
    /* finish initialization */
    pdev->state = VLYNQ_DRV_STATE_RUN;
    VLYNQ_RESETCB( VLYNQ_RESET_INITOK);
    return VLYNQ_SUCCESS;

}


/* ----------------------------------------------------------------------------
 *  Function : vlynqInterruptInit()
 *  Description: This local function is used to set up the ICB table for the 
 *  VLYNQ_STATUS_REG vlynq module. The input parameter "pdev" points the vlynq
 *  device instance whose ICB is allocated.
 *  Return : returns VLYNQ_SUCCESS or vlynq error for failure
 * -----------------------------------------------------------------------------
 */
PRIVATE INT32 vlynqInterruptInit(VLYNQ_DEV *pdev)
{
    int i, numslots;

    /* Memory allocated statically.
     * Initialise ICB,free list.Indicate primary slot empty.
     * Intialise intVector <==> map_vector translation table*/
    for(i=0; i < VLYNQ_NUM_INT_BITS; i++)
    {
        pdev->pIntrCB[i].isr = NULL;  
        pdev->pIntrCB[i].next = NULL; /*nothing chained */
        pdev->vector_map[i] = -1;   /* indicates unmapped  */
    }

    /* In the ICB slots, [VLYNQ_NUM_INT_BITS i.e 32 to ICB array size) are expansion slots
     * required only when interrupt chaining/sharing is supported. In case
     * of chained interrupts the list starts from primary slot and the
     * additional slots are obtained from the common free area */

    /* Initialise freelist */

    numslots = VLYNQ_NUM_INT_BITS + VLYNQ_IVR_CHAIN_SLOTS;
    
    if (numslots > VLYNQ_NUM_INT_BITS)
    {
        pdev->freelist = &(pdev->pIntrCB[VLYNQ_NUM_INT_BITS]);
        
        for(i = VLYNQ_NUM_INT_BITS; i < (numslots-1) ; i++)
        {
            pdev->pIntrCB[i].next = &(pdev->pIntrCB[i+1]);
            pdev->pIntrCB[i].isr = NULL;
        }
        pdev->pIntrCB[i].next=NULL; /* Indicate end of freelist*/
        pdev->pIntrCB[i].isr=NULL;
    }  
    else
    {   
        pdev->freelist = NULL;
    }

    /** Reset mapping for IV 0-7 **/
    VLYNQ_IVR_03TO00_REG = 0;
    VLYNQ_IVR_07TO04_REG = 0;

    return VLYNQ_SUCCESS;
}

/** remember that hooking up of root ISR handler with the interrupt controller 
 *  is not done as a part of this driver. Typically, it must be done after
 *  invoking vlynq_init*/


 /* ----------------------------------------------------------------------------
 *  ISR with the SOC interrupt controller. This ISR typically scans
 *  the Int PENDING/SET register in the VLYNQ module and calls the
 *  appropriate ISR associated with the correponding vector number.
 * -----------------------------------------------------------------------------
 */
GLOBAL void vlynq_root_isr(void *arg)
{
    int    source;  /* Bit position of pending interrupt, start from 0 */
    UINT32 interrupts, clrInterrupts;
    VLYNQ_DEV * pdev;
    VLYNQ_INTR_CNTRL_ICB *entry;

    pdev=(VLYNQ_DEV*)(arg);          /*obtain the vlynq device pointer*/
 
    interrupts =  VLYNQ_INT_STAT_REG; /* Get the list of pending interrupts */
    VLYNQ_INT_STAT_REG = interrupts; /* clear the int CR register */
    clrInterrupts = interrupts;      /* save them for further analysis */

    debugPrint("vlynq_root_isr: dev %u. INTCR = 0x%08lx\n", pdev->dev_idx, clrInterrupts,0,0,0,0);

    /* Scan interrupt bits */
    source =0;
    while( clrInterrupts != 0)
    {
        /* test if bit is set? */
        if( 0x1ul & clrInterrupts)
        {   
            entry = &(pdev->pIntrCB[source]);   /* Get the ISR entry */
            pdev->intCount++;                   /* update interrupt count */    
            if(entry->isr != NULL)
            {
                do 
                {
                    pdev->isrCount++;   /* update isr invocation count */    
                    /* Call the user ISR and update the count for ISR */
		    entry->isrCount++;   
                    entry->isr(entry->arg1, entry->arg2, entry->arg3);
                    if (entry->next == NULL) break;
                    entry = entry->next;

                } while (entry->isr != NULL);
            }
            else
            {   
                debugPrint(" ISR not installed for vlynq vector:%d\n",source,0,0,0,0,0);
            }
        }
        clrInterrupts >>= 1;    /* Next source bit */
        ++source;
    } /* endWhile clrInterrupts != 0 */
}


 /* ----------------------------------------------------------------------------
 *  Function : vlynq_local__module_isr()
 *  Description: This ISR is attached to the local VLYNQ interrupt vector
 *  by the Vlynq Driver when local interrupts are being handled. i.e.
 *  intLocal=1. This ISR handles local Vlynq module status interrupts only
 *  AS a part of this ISR, user callback in VLYNQ_DEV structure
 *  is invoked.
 *  VLYNQ_DEV is passed as arg1. arg2 and arg3 are unused.
 * -----------------------------------------------------------------------------
 */
PRIVATE void vlynq_local_module_isr(void *arg1,void *arg2, void *arg3)
{
    VLYNQ_REPORT_CB func;
    UINT32 dwStatRegVal;
    VLYNQ_DEV * pdev;

    pdev = (VLYNQ_DEV*) arg1;
    /* Callback function is read from the device pointer that is passed as an argument */
    func = pdev->report_cb;

    /* read local status register */
    dwStatRegVal = VLYNQ_STATUS_REG;

    /* clear pending events */
    VLYNQ_STATUS_REG = dwStatRegVal;
   
    /* invoke user callback */
    if( func != NULL)
        func( pdev, VLYNQ_LOCAL_DVC, dwStatRegVal);

}

 /* ----------------------------------------------------------------------------
 *  Function : vlynq_remote_module_isr()
 *  Description: This ISR is attached to the remote VLYNQ interrupt vector
 *  by the Vlynq Driver when remote interrupts are being handled locally. i.e.
 *  intLocal=1. This ISR handles local Vlynq module status interrupts only
 *  AS a part of this ISR, user callback in VLYNQ_DEV structure
 *  is invoked.
 *  The parameters  irq,regs ar unused.
 * -----------------------------------------------------------------------------
 */
PRIVATE void vlynq_remote_module_isr(void *arg1,void *arg2, void *arg3)
{
    VLYNQ_REPORT_CB func;
    UINT32 dwStatRegVal;
    VLYNQ_DEV * pdev;

   
    pdev = (VLYNQ_DEV*) arg1;
   
    /* Callback function is read from the device pointer that is passed as an argument */
   func = pdev->report_cb;

    /* read local status register */
    dwStatRegVal = VLYNQ_R_STATUS_REG;

    /* clear pending events */
    VLYNQ_R_STATUS_REG = dwStatRegVal;

    /* invoke user callback */
    if( func != NULL)
        func( pdev, VLYNQ_REMOTE_DVC, dwStatRegVal);

}

/* ----------------------------------------------------------------------------
 *  Function : vlynq_interrupt_get_count()
 *  Description: This function returns the number of times a particular intr
 *  has been invoked. 
 *
 *  It returns 0, if erroneous map_vector is specified or if the corres isr 
 *  has not been registered with VLYNQ.
 */
GLOBAL UINT32 vlynq_interrupt_get_count(VLYNQ_DEV *pdev,
					UINT32 map_vector)
{
    VLYNQ_INTR_CNTRL_ICB *entry;
    UINT32 count = 0;

    if (map_vector > (VLYNQ_NUM_INT_BITS-1)) 
        return count;
   
    entry = &(pdev->pIntrCB[map_vector]);

    if (entry)
        count = entry->isrCount;

    return (count);
}


/* ----------------------------------------------------------------------------
 *  Function : vlynq_install_isr()
 *  Description: This function installs ISR for Vlynq interrupt vector
 *  bits(in IntPending register). This function should be used only when 
 *  Vlynq interrupts are being handled locally(remote may be programmed to send
 *  interrupt packets).Also, the int2cfg should be 1 and the least significant
 *  8 bits of the Interrupt Pointer Register must point to Interrupt 
 *  Pending/Set Register).
 *  If host int2cfg=0 and the Interrupt Pointer register contains 
 *  the address of the interrupt set register in the interrupt controller 
 *  module of the local device , then the ISR for the remote interrupt must be 
 *  directly registered with the Interrupt controller and must not use this API
 *  Note: this function simply installs the ISR in ICB It doesnt modify
 *  any register settings
 */
GLOBAL INT32 
vlynq_install_isr(VLYNQ_DEV *pdev,
                  UINT32 map_vector,
                  VLYNQ_INTR_CNTRL_ISR isr,
                  void *arg1, void *arg2, void *arg3)
{
    VLYNQ_INTR_CNTRL_ICB *entry;

    if ( (map_vector > (VLYNQ_NUM_INT_BITS-1)) || (isr == NULL) ) 
        return VLYNQ_INVALID_ARG;
   
    entry = &(pdev->pIntrCB[map_vector]);

    if(entry->isr == NULL)
    {
        entry->isr = isr;
        entry->arg1 = arg1;
        entry->arg2 = arg2;
        entry->arg3 = arg3;
        entry->next = NULL;
    }
    else
    {
        /** No more empty slots,return error */
        if(pdev->freelist == NULL)
            return VLYNQ_MEMALLOC_FAIL;
        
        while(entry->next != NULL)
        {
            entry = entry->next;
        }

        /* Append new node to the chain */
        entry->next = pdev->freelist;   
        /* Remove the appended node from freelist */
        pdev->freelist = pdev->freelist->next;  
        entry= entry->next;
         
        /*** Set the ICB fields ***/
        entry->isr = isr;
        entry->arg1 = arg1;
        entry->arg2 = arg2;
        entry->arg3 = arg3;
        entry->next = NULL;
    }
   
    return VLYNQ_SUCCESS;
}



/* ----------------------------------------------------------------------------
 *  Function : vlynq_uninstall_isr
 *  Description: This function is used to uninstall a previously
 *  registered ISR. In case of shared/chained interrupts, the 
 *  void * arg parameter must uniquely identify the ISR to be
 *  uninstalled.
 *  Note: this function simply uninstalls the ISR in ICB
 *  It doesnt modify any register settings
 */
GLOBAL INT32 
vlynq_uninstall_isr(VLYNQ_DEV *pdev,
                    UINT32 map_vector,
                    void *arg1, void *arg2, void *arg3) 
{
    VLYNQ_INTR_CNTRL_ICB *entry,*temp;

    if (map_vector > (VLYNQ_NUM_INT_BITS-1)) 
        return VLYNQ_INVALID_ARG;
   
    entry = &(pdev->pIntrCB[map_vector]);

    if(entry->isr == NULL ) 
        return VLYNQ_ISR_NON_EXISTENT;

    if ( (entry->arg1 == arg1) && (entry->arg2 == arg2) && (entry->arg3 == arg3) )
    {
        if(entry->next == NULL)
        {
            entry->isr=NULL;
            return VLYNQ_SUCCESS;
        }
        else
        {
            temp =  entry->next;
            /* Copy next node in the chain to prim.slot */
            entry->isr = temp->isr;
            entry->arg1 = temp->arg1;
            entry->arg2 = temp->arg2;
            entry->arg3 = temp->arg3;
            entry->next = temp->next;
            /* Free the just copied node */
            temp->isr = NULL;
            temp->arg1 = NULL;
            temp->arg2 = NULL;
            temp->arg3 = NULL;
            temp->next = pdev->freelist;
            pdev->freelist = temp;
            return VLYNQ_SUCCESS;
        }
    }
    else
    {
        temp = entry;
        while ( (entry = temp->next) != NULL)
        {
            if ( (entry->arg1 == arg1) && (entry->arg2 == arg2) && (entry->arg3 == arg3) )
            {
                /* remove node from chain */
                temp->next = entry->next; 
                /* Add the removed node to freelist */
                entry->isr = NULL;
                entry->arg1 = NULL;
                entry->arg2 = NULL;
                entry->arg3 = NULL;
                entry->next = pdev->freelist;
                entry->isrCount = 0;
                pdev->freelist  = entry;
                return VLYNQ_SUCCESS;
            }
            temp = entry;
        }
    
        return VLYNQ_ISR_NON_EXISTENT;
    }
}




/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_vector_set()
 *  description:configures interrupt vector mapping,interrupt type
 *  polarity -all in one go.
 */
GLOBAL INT32 
vlynq_interrupt_vector_set(VLYNQ_DEV *pdev,                 /* vlynq device */
                           UINT32 int_vector,               /* int vector on vlynq device */
                           UINT32 map_vector,               /* bit for this interrupt */
                           VLYNQ_DEV_TYPE dev_type,         /* local or remote device */
                           VLYNQ_INTR_POLARITY pol,         /* polarity of interrupt */
                           VLYNQ_INTR_TYPE type)            /* pulsed/level interrupt */
{
    volatile UINT32 * vecreg;
    UINT32 val=0;
    UINT32 bytemask=0XFF;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */

    /* validate the number of interrupts supported */
    if (int_vector >= VLYNQ_IVR_MAXIVR) 
        return VLYNQ_INVALID_ARG;
        
    if(map_vector > (VLYNQ_NUM_INT_BITS - 1) ) 
        return VLYNQ_INVALID_ARG;

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /* Update the intVector<==> bit position translation table */
    pdev->vector_map[map_vector] = int_vector;

    /* val has been initialised to zero. we only have to turn on appropriate bits*/
    if(type == VLYNQ_INTR_PULSED)
        val |= VLYNQ_IVR_INTTYPE_MASK;
        
    if(pol == VLYNQ_INTR_ACTIVE_LOW)
        val |= VLYNQ_IVR_INTPOL_MASK;

    val |= map_vector;

    /** clear the correct byte position and then or val **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    /** write to correct byte position in vecreg*/
    *vecreg = (*vecreg) | (val << ( (int_vector % 4)*8) ) ;

    /* Setting a interrupt vector, leaves the interrupt disabled 
     * which  must be enabled subsequently */

    return VLYNQ_SUCCESS;
}


/* ----------------------------------------------------------------------------
 *  Function : vlynq_interrupt_vector_cntl()
 *  Description:enables/disable interrupt
 */
GLOBAL INT32 vlynq_interrupt_vector_cntl( VLYNQ_DEV *pdev,
                                          UINT32 int_vector,
                                          VLYNQ_DEV_TYPE dev_type,
                                          UINT32 enable)
{
    volatile UINT32 *vecReg;
    UINT32 val=0;
    UINT32 intenMask=0x80;

    /* validate the number of interrupts supported */
    if (int_vector >= VLYNQ_IVR_MAXIVR) 
        return VLYNQ_INVALID_ARG;

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecReg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecReg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /** Clear the correct byte position and then or val **/
    *vecReg = (*vecReg) & ( ~(intenMask << ( (int_vector %4)*8) ) );

    if(enable)
    {
        val |= VLYNQ_IVR_INTEN_MASK; 
        /** Write to correct byte position in vecReg*/
        *vecReg = (*vecReg) | (val << ( (int_vector % 4)*8) ) ;
    }

    return VLYNQ_SUCCESS;

}/* end of function vlynq_interrupt_vector_cntl */



/* ----------------------------------------------------------------------------
 *  Function : vlynq_interrupt_vector_map()
 *  Description:Configures interrupt vector mapping alone
 */
GLOBAL INT32 
vlynq_interrupt_vector_map( VLYNQ_DEV *pdev,
                            VLYNQ_DEV_TYPE dev_type,
                            UINT32 int_vector,
                            UINT32 map_vector)
{
    volatile UINT32 * vecreg;
    UINT32 val=0;
    UINT32 bytemask=0x1f;   /* mask to turn off bits corresponding to int vector */ 

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */

    /* validate the number of interrupts supported */
    if (int_vector >= VLYNQ_IVR_MAXIVR) 
        return VLYNQ_INVALID_ARG;
        
    if(map_vector > (VLYNQ_NUM_INT_BITS - 1) ) 
        return VLYNQ_INVALID_ARG;

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /* Update the intVector<==> bit position translation table */
    pdev->vector_map[map_vector] = int_vector;

    /** val has been initialised to zero. we only have to turn on
     * appropriate bits*/
    val |= map_vector;

    /** clear the correct byte position and then or val **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    /** write to correct byte position in vecreg*/
    *vecreg = (*vecreg) | (val << ( (int_vector % 4)*8) ) ;

    return VLYNQ_SUCCESS;
}


/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_set_polarity()
 *  description:configures interrupt polarity .
 */
GLOBAL INT32 
vlynq_interrupt_set_polarity( VLYNQ_DEV *pdev ,
                              VLYNQ_DEV_TYPE dev_type,
                              UINT32 map_vector,
                              VLYNQ_INTR_POLARITY pol)
{
    volatile UINT32 * vecreg;
    INT32 int_vector;
    UINT32 val=0;
    UINT32 bytemask=0x20; /** mask to turn off bits corresponding to int polarity */

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];

    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /* val has been initialised to zero. we only have to turn on
     * appropriate bits, if need be*/

    /** clear the correct byte position and then or val **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    if( pol == VLYNQ_INTR_ACTIVE_LOW)
    {
        val |= VLYNQ_IVR_INTPOL_MASK;
        /** write to correct byte position in vecreg*/
        *vecreg = (*vecreg) | (val << ( (int_vector % 4)*8) ) ;
    }

    return VLYNQ_SUCCESS;
}

GLOBAL INT32 vlynq_interrupt_get_polarity( VLYNQ_DEV *pdev ,
                                           VLYNQ_DEV_TYPE dev_type,
                                           UINT32 map_vector)
{
    volatile UINT32 * vecreg;
    INT32 int_vector;
    UINT32 val=0;

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];

    if (map_vector > (VLYNQ_NUM_INT_BITS-1))
        return(-1);

    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /** read the information into val **/
    val = (*vecreg) & ((VLYNQ_IVR_INTPOL_MASK << ( (int_vector %4)*8) ) );
    
    return (val ? (VLYNQ_INTR_ACTIVE_LOW) : (VLYNQ_INTR_ACTIVE_HIGH));
}


/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_set_type()
 *  description:configures interrupt type .
 */
GLOBAL INT32 vlynq_interrupt_set_type( VLYNQ_DEV *pdev,
                                       VLYNQ_DEV_TYPE dev_type,
                                       UINT32 map_vector,
                                       VLYNQ_INTR_TYPE type)
{
    volatile UINT32 * vecreg;
    UINT32 val=0;
    INT32 int_vector;

    /** mask to turn off bits corresponding to interrupt type */
    UINT32 bytemask=0x40;

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];
    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */
    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /** val has been initialised to zero. we only have to turn on
     * appropriate bits if need be*/

     /** clear the correct byte position and then or val **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    if( type == VLYNQ_INTR_PULSED)
    {
        val |= VLYNQ_IVR_INTTYPE_MASK;
        /** write to correct byte position in vecreg*/
        *vecreg = (*vecreg) | (val << ( (int_vector % 4)*8) ) ;
    }

    return VLYNQ_SUCCESS;
}

/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_get_type()
 *  description:returns interrupt type .
 */
GLOBAL INT32 vlynq_interrupt_get_type( VLYNQ_DEV *pdev, VLYNQ_DEV_TYPE dev_type,
                                       UINT32 map_vector)
{
    volatile UINT32 * vecreg;
    UINT32 val=0;
    INT32 int_vector;

    if (map_vector > (VLYNQ_NUM_INT_BITS-1))
        return(-1);

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];
    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */
    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /** Read the correct bit position into val **/
    val = (*vecreg) & ((VLYNQ_IVR_INTTYPE_MASK << ( (int_vector %4)*8) ) );

    return (val ? (VLYNQ_INTR_PULSED) : (VLYNQ_INTR_LEVEL));
}

/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_enable()
 *  description:Enable interrupt by writing to IVR register.
 */
GLOBAL INT32 vlynq_interrupt_enable( VLYNQ_DEV *pdev,
                                     VLYNQ_DEV_TYPE dev_type,
                                     UINT32 map_vector)
{
    volatile UINT32 * vecreg;
    UINT32 val=0;
    INT32 int_vector;

    /** mask to turn off bits corresponding to interrupt enable */
    UINT32 bytemask=0x80;

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];
    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */

    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /** val has been initialised to zero. we only have to turn on
    *  bit corresponding to interrupt enable*/
    val |= VLYNQ_IVR_INTEN_MASK;

    /** clear the correct byte position and then or val **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    /** write to correct byte position in vecreg*/
    *vecreg = (*vecreg) | (val << ( (int_vector % 4)*8) ) ;

    return VLYNQ_SUCCESS;
}


/* ----------------------------------------------------------------------------
 *  function : vlynq_interrupt_disable()
 *  description:Disable interrupt by writing to IVR register.
 */
GLOBAL INT32 
vlynq_interrupt_disable( VLYNQ_DEV *pdev,
                         VLYNQ_DEV_TYPE dev_type,
                         UINT32 map_vector)
{
    volatile UINT32 * vecreg;
    INT32 int_vector;

    /** mask to turn off bits corresponding to interrupt enable */
    UINT32 bytemask=0x80;

    /* get the int_vector from map_vector */
    int_vector = pdev->vector_map[map_vector];
    if(int_vector == -1) 
        return VLYNQ_INTVEC_MAP_NOT_FOUND;

    /* use the lower 8 bits of val to set the value , shift it to 
     * appropriate byte position in the ivr and write it to the 
     * corresponding register */
    if (dev_type == VLYNQ_LOCAL_DVC)
    {
        vecreg = (volatile UINT32 *) (VLYNQ_IVR_OFFSET(int_vector));
    }
    else
    {
        vecreg = (volatile UINT32 *) (VLYNQ_R_IVR_OFFSET(int_vector));  
    }

    /* We disable the interrupt by simply turning off the bit
     * corresponding to Interrupt enable. 
     * Clear the interrupt enable bit in the correct byte position **/
    *vecreg = (*vecreg) & ( ~(bytemask << ( (int_vector %4)*8) ) );

    /* Dont have to set any bit positions */

    return VLYNQ_SUCCESS;

}




