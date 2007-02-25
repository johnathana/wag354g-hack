/******************************************************************************
 * FILE PURPOSE:    CPMAC Net Driver HAL support Source
 ******************************************************************************
 * FILE NAME:       cpmacHalLx.c
 *
 * DESCRIPTION:     CPMAC Network Device Driver Source
 *
 * REVISION HISTORY:
 *
 * Date           Description                               Author
 *-----------------------------------------------------------------------------
 * 27 Nov 2002    Initial Creation                          Suraj S Iyer  
 * 09 Jun 2003    Updates for GA                            Suraj S Iyer
 * 18 Dec 2003    Updated for 5.7                           Suraj S Iyer
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 *******************************************************************************/
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>     
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <linux/string.h>

#include "cpmacHalLx.h"
#include "cpmac.h"

/* PSP config headers */
#include "psp_config_parse.h"
#include "psp_config_mgr.h"

/* Probe Debug Section*/

/* debug */                                 
extern int cpmac_debug_mode;
#define dbgPrint if (cpmac_debug_mode) printk
#define errPrint printk

char CpmacSignature[] = "Cpmac driver";
static unsigned long irq_flags = 0;
OS_SETUP *p_os_setup = NULL;

extern int avalanche_request_intr_pacing(int, unsigned int, unsigned int);
extern int avalanche_free_intr_pacing(unsigned int blk_num);

/*----------------------------------------------------------------------------
 * Parameter extracting functionalities.
 *--------------------------------------------------------------------------*/
static int os_find_parm_u_int(void *info_ptr, const char *param, unsigned int *val)
{
    int ret_val = 0;

    if((ret_val = psp_config_get_param_uint(info_ptr, param, val)) == -1)
    {
        dbgPrint("Error: could not locate the requested \"%s\" param.\n",param);
        ret_val = -1;
    }

    return(ret_val);
}

static int os_find_parm_val(void *info_ptr, const char *param, void *val)
{
    int ret_val = 0;

    if(psp_config_get_param_string(info_ptr, param, val) == -1)
    {
        dbgPrint("Error: could not locate the requested \"%s\" param.\n",param);
        ret_val = -1;
    }

    return(ret_val);
}

static int os_find_device(int unit, const char *find_name, void *device_info)
{
    int ret_val = 0;

    if(psp_config_get((char *)find_name, unit, device_info) == -1)
    {
        dbgPrint("Error: could not locate the requested \"%s\" param.\n", find_name);
        ret_val = -1;
    }

    return(ret_val);
}

/*---------------------------------------------------------------------------
 * Memory related OS abstraction.
 *--------------------------------------------------------------------------*/
void os_free(void *mem_ptr)
{
    kfree(mem_ptr);
}

void  os_free_buffer(OS_RECEIVEINFO *osReceiveInfo, void *mem_ptr)
{
    dev_kfree_skb_any(osReceiveInfo);
}

void  os_free_dev(void *mem_ptr)
{
    kfree(mem_ptr);
}

void os_free_dma_xfer(void *mem_ptr)
{
    kfree(mem_ptr);
}

static void *os_malloc(unsigned int size)
{
    return(kmalloc(size, GFP_KERNEL));
}

static void *os_malloc_dma_xfer(unsigned int size, 
                                void         *mem_base, 
                                unsigned int mem_range)
{
    return(kmalloc(size, GFP_KERNEL));
}

static void *os_malloc_dev(unsigned int size)
{
    return(kmalloc(size, GFP_KERNEL));
}


/*----------------------------------------------------------------------------
 * CRITICAL SECTION ENABLING/DISABLING.
 *--------------------------------------------------------------------------*/
static void os_critical_on(void)
{
    save_and_cli(irq_flags);
}

static void os_critical_off(void)
{
    restore_flags(irq_flags);
}

/*----------------------------------------------------------------------------
 * Cache related abstraction
 *--------------------------------------------------------------------------*/
static void os_cache_invalidate(void *mem_ptr, int size)
{
    dma_cache_inv((unsigned long)mem_ptr, size);
}

static void os_cache_writeback(void *mem_ptr, int size)
{
    dma_cache_wback_inv((unsigned long)mem_ptr, size);
}

/*-----------------------------------------------------------------------------
 * Support functions.
 *---------------------------------------------------------------------------*/

static void  hal_drv_unregister_isr(OS_DEVICE *p_dev, int intr)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_dev->priv;
    CPMAC_ISR_INFO_T     *p_isr_cb_param = &p_cpmac_priv->cpmac_isr;
    intr                                 = LNXINTNUM(intr);

    free_irq(p_isr_cb_param->intr, p_isr_cb_param);

    dbgPrint("cpmac_hal_unregister called for the intr %d for unit %x and isr_cb_param %x.\n", 
             intr, p_cpmac_priv->instance_num, (unsigned int )&p_cpmac_priv->cpmac_isr);
}


static void hal_drv_register_isr(OS_DEVICE *p_dev, 
                                 CPMAC_HAL_ISR_FUNC_T hal_isr, int intr)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal      = p_cpmac_priv->drv_hal;
    CPMAC_ISR_INFO_T     *p_isr_cb_param = &p_cpmac_priv->cpmac_isr;
    intr                                 = LNXINTNUM(intr);
       
    dbgPrint("osRegister called for the intr %d for device %x and p_isr_cb_param %x.\n", 
             intr, (bit32u)p_dev, (bit32u)p_isr_cb_param);

    p_isr_cb_param->owner       = p_drv_hal;
    p_isr_cb_param->hal_isr     = hal_isr;
    p_isr_cb_param->intr        = intr;

    tasklet_init(&p_isr_cb_param->tasklet, cpmac_handle_tasklet, (unsigned long)p_isr_cb_param);
    dbgPrint("Success in registering irq %d for Cpmac unit# %d.\n", intr, p_cpmac_priv->instance_num); 
}

/*---------------------------------------------------------------------------
 * FUNCTIONS called by the CPMAC Net Device.
 *-------------------------------------------------------------------------*/
static int load_os_funcs(OS_FUNCTIONS *os_func)
{
    dbgPrint("os_init_module: Start\n"); 
    if( os_func == 0 )
    {
        return(sizeof(OS_FUNCTIONS));   
    }

    os_func->Control                    = cpmac_hal_control;
    os_func->CriticalOn                 = os_critical_on;
    os_func->CriticalOff                = os_critical_off;
    os_func->DataCacheHitInvalidate     = os_cache_invalidate;
    os_func->DataCacheHitWriteback      = os_cache_writeback;       
    os_func->DeviceFindInfo     = os_find_device;
    os_func->DeviceFindParmUint = os_find_parm_u_int;
    os_func->DeviceFindParmValue= os_find_parm_val;
    os_func->Free               = os_free;
    os_func->FreeRxBuffer       = os_free_buffer;
    os_func->FreeDev            = os_free_dev;
    os_func->FreeDmaXfer        = os_free_dma_xfer;
    os_func->IsrRegister        = hal_drv_register_isr;
    os_func->IsrUnRegister      = hal_drv_unregister_isr;
    os_func->Malloc             = os_malloc;
    os_func->MallocDev          = os_malloc_dev;
    os_func->MallocDmaXfer      = os_malloc_dma_xfer;
    os_func->MallocRxBuffer     = cpmac_hal_malloc_buffer;
    os_func->Memset             = memset;
    os_func->Printf             = printk;
    os_func->Receive            = cpmac_hal_receive;
    os_func->SendComplete       = cpmac_hal_send_complete;
    os_func->Strcmpi            = cpmac_ci_strcmp;
    os_func->TeardownComplete   = cpmac_hal_tear_down_complete;
    os_func->Strstr             = strstr;
    os_func->Strtoul            = simple_strtol;
    os_func->Sprintf            = sprintf;
    os_func->Strlen             = strlen;

    dbgPrint("os_init_module: Leave\n");             
  
    return(0);
}


int cpmac_drv_init(CPMAC_DRV_HAL_INFO_T *p_drv_hal)
{
    HAL_DEVICE       *p_hal_dev    = p_drv_hal->hal_dev;
    HAL_FUNCTIONS    *p_hal_funcs  = p_drv_hal->hal_funcs;

    return(p_hal_funcs->Init(p_hal_dev));
}

int cpmac_drv_cleanup(CPMAC_DRV_HAL_INFO_T *p_drv_hal)
{
    HAL_DEVICE       *p_hal_dev    = p_drv_hal->hal_dev;
    HAL_FUNCTIONS    *p_hal_funcs  = p_drv_hal->hal_funcs;

    int              ret_val       = p_hal_funcs->Shutdown(p_hal_dev);

#if 0
    if(ret_val == 0)
        kfree(p_hal_funcs);
    else
        ret_val = -1;
#endif

    kfree(p_drv_hal->os_funcs); 
    
    return (ret_val); 
}

int cpmac_drv_tx_setup(HAL_FUNCTIONS        *p_hal_funcs,
                       HAL_DEVICE           *p_hal_dev,
                       CPMAC_TX_CHAN_INFO_T *p_tx_chan_info)
{
    int ret_val = 0;
    int count   = 0;
    CHANNEL_INFO chan_info;

    /* Let's setup the TX Channels. */
    for(count=0; count < p_tx_chan_info->cfg_chan; count++)
    {
        chan_info.Channel        = count;
        chan_info.Direction      = DIRECTION_TX;       
        chan_info.TxNumBuffers   = p_tx_chan_info->chan[count].num_BD;
        chan_info.TxServiceMax   = p_tx_chan_info->chan[count].service_max;
        chan_info.TxNumQueues    = 0;
        
        if((ret_val = p_hal_funcs->ChannelSetup(p_hal_dev, &chan_info,
                                                NULL)) != 0)
        {
            errPrint("Error in opening channel %d for TX.\n", count);
            ret_val = -1;
            break;
        }

        p_tx_chan_info->opened_chan++;
    }

    return(ret_val);
}

int cpmac_drv_rx_setup(HAL_FUNCTIONS         *p_hal_funcs,
                       HAL_DEVICE            *p_hal_dev,
                       CPMAC_RX_CHAN_INFO_T  *p_rx_chan_info)
{
    int ret_val = 0;
    CHANNEL_INFO chan_info;

    chan_info.Channel       = 0;
    chan_info.Direction     = DIRECTION_RX;
    chan_info.RxBufSize     = p_rx_chan_info->chan[0].buffer_size;
    chan_info.RxBufferOffset= p_rx_chan_info->chan[0].buffer_offset;
    chan_info.RxNumBuffers  = p_rx_chan_info->chan[0].num_BD;
    chan_info.RxServiceMax  = p_rx_chan_info->chan[0].service_max;

    if(p_hal_funcs->ChannelSetup(p_hal_dev, &chan_info, p_rx_chan_info) != 0) 
    {
        errPrint("Error in opening channel %d for RX.\n", 0);
        ret_val = -1;
    }

    return(ret_val);
}

int cpmac_drv_start(CPMAC_DRV_HAL_INFO_T *p_drv_hal,
                    CPMAC_TX_CHAN_INFO_T *p_tx_chan_info,
                    CPMAC_RX_CHAN_INFO_T *p_rx_chan_info,
                    unsigned int         flags)
{
    int ret_val = 0;
    HAL_FUNCTIONS *p_hal_funcs = p_drv_hal->hal_funcs;
    HAL_DEVICE    *p_hal_dev   = p_drv_hal->hal_dev;

    dbgPrint("It is in cpmac_drv_start for %x.\n", (unsigned int)p_drv_hal);

    if(flags & CHAN_SETUP)
    {
        if(cpmac_drv_tx_setup(p_hal_funcs, p_hal_dev, 
                  p_tx_chan_info)!=0)
        {
            errPrint("Failed to set up tx channel(s).\n");
            ret_val = -1;
        }
        else if(cpmac_drv_rx_setup(p_hal_funcs, p_hal_dev,
                                   p_rx_chan_info)!=0)
        {
            errPrint("Failed to set up rx channel.\n");
            ret_val = -1;
        }
        else
        {
            ret_val = 0;
        }
    }

    /* Error in setting up the Channels, quit. */
    if((ret_val == 0) && (ret_val = p_hal_funcs->Open(p_hal_dev)) != 0)
    {
        errPrint("failed to open the HAL!!!.\n");
        ret_val = -1;
    }

    return (ret_val);
} /* cpmac_drv_start */



int cpmac_drv_tx_teardown(HAL_FUNCTIONS        *p_hal_funcs,
                          HAL_DEVICE           *p_hal_dev,
                          CPMAC_TX_CHAN_INFO_T *p_tx_chan_info,
                          unsigned int         flags)
{
    int ret_val = 0;
    int count   = 0;

    /* Let's setup the TX Channels. */
    for(;  p_tx_chan_info->opened_chan > 0; 
        p_tx_chan_info->opened_chan--, count++)
    {
        if(p_hal_funcs->ChannelTeardown(p_hal_dev, count, flags) != 0)
        {
            errPrint("Error in tearing down channel %d for TX.\n", count);
            ret_val = -1;
            break;
        }
    }

    return(ret_val);
}


int cpmac_drv_rx_teardown(HAL_FUNCTIONS *p_hal_funcs,
                  HAL_DEVICE    *p_hal_dev,
              unsigned int  flags)
{
    int ret_val = 0;

    if(p_hal_funcs->ChannelTeardown(p_hal_dev, 0, flags) != 0) 
    {
        errPrint("Error in tearing down channel %d for RX.\n", 0);
        ret_val = -1;
    }

    return(ret_val);
}

int cpmac_drv_stop(CPMAC_DRV_HAL_INFO_T    *p_drv_hal,
                   CPMAC_TX_CHAN_INFO_T    *p_tx_chan_info,
                   CPMAC_RX_CHAN_INFO_T    *p_rx_chan_info,
                   unsigned int flags)
{
    HAL_DEVICE       *p_hal_dev    = p_drv_hal->hal_dev;
    HAL_FUNCTIONS    *p_hal_funcs  = p_drv_hal->hal_funcs;
    int ret_val                    = 0;

    if(flags & CHAN_TEARDOWN)
    {
        unsigned int chan_flags = 0;

        if(flags & FREE_BUFFER) chan_flags |= 0x4; /* full tear down */
        if(flags & BLOCKING)    chan_flags |= 0x8; /* blocking call  */

        dbgPrint("The teardown flags are %d.\n", flags);
        dbgPrint("The teardown chan flags are %d.\n", chan_flags);

        if(cpmac_drv_tx_teardown(p_hal_funcs, p_hal_dev, 
                                 p_tx_chan_info, chan_flags | 0x1) != 0)
        {
            ret_val = -1;
            errPrint("The tx channel teardown failed.\n");
        }
        else if(cpmac_drv_rx_teardown(p_hal_funcs, p_hal_dev, chan_flags | 0x2) != 0)
        {
            ret_val = -1;
            errPrint("The rx channel teardown failed.\n");
        }
        else
        {
            ;
        }
    }

    if(ret_val == 0)
    {
        int close_flags = 1;

        if(flags & FREE_BUFFER)  close_flags = 2;
//        if(flags & COMPLETE)     close_flags = 3;

        if(p_hal_funcs->Close(p_hal_dev, close_flags) != 0)
        {
            ret_val = -1;
        }
    }
        
    return(ret_val);
}

int cpmac_drv_init_module(CPMAC_DRV_HAL_INFO_T *p_drv_hal, OS_DEVICE *p_os_dev, int inst)
{
    int ret_val = -1;
    int hal_func_size;

    dbgPrint("Entering the CpmacInitModule for the inst %d \n", inst);

    if((p_drv_hal->os_funcs = kmalloc(sizeof(OS_FUNCTIONS), GFP_KERNEL)) == NULL)
    {
        errPrint("Failed to allocate memory for OS_FUNCTIONS.\n");
    }
    else if(load_os_funcs(p_drv_hal->os_funcs) != 0)
    {
        errPrint("Failed to load OS funcs.\n");
        os_free(p_drv_hal->os_funcs);
    }
    else if(halCpmacInitModule(&p_drv_hal->hal_dev, p_os_dev,
                               &p_drv_hal->hal_funcs, p_drv_hal->os_funcs, 
                               sizeof(*p_drv_hal->os_funcs),
                               &hal_func_size, inst) != 0)
    {
        errPrint("halCpmacInitModule failed for inst %d \n", inst);
        os_free(p_drv_hal->os_funcs);
    }
    else if(p_drv_hal->hal_funcs->Probe(p_drv_hal->hal_dev) != 0)
    {
        errPrint("halCpmacProbe failed for inst %d \n", inst);       
        os_free(p_drv_hal->os_funcs);
    }
    else
    {
        /* every thing went well. */
        ret_val = 0;
    }  

    return (ret_val);  
}
