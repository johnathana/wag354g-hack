/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2004 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/** \file   cpmacNetLxTxRx.c
    \brief  CPMAC Linux DDA Send/Receive Source file

    This file contains the device driver adaptation for Linux for CPMAC/CPGMAC 
    device based upon PSP Framework architecture.

    Acknowledgements: This DDA implementation for CP(G)MAC device is based upon
    Linux device driver for CP(G)MAC written using HAL 2.0.

    Notes:
    DDA/DDC Common features:

    The following flags should be defined by the make file for support of the features:

    (1) CPMAC_CACHE_WRITEBACK_MODE to support write back cache mode.

    (2) NOTE: THIS DRIVER DOES NOT SUPPORT MULTIFRAGMENTS AS OF NOW
        For future support - define CPMAC_MULTIFRAGMENT to support multifragments. 

    (3) CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY - to  support of multiple Tx complete 
        notifications. If this is defined the Tx complete DDA callback function 
        contains multiple packet Tx complete events.
        Note: BY DEFAULT THIS DRIVER HANDLES MULTIPLE TX COMPLETE VIA ITS CALLBACK IN THE SAME
        FUNCTION FOR SINGLE PACKET COMPLETE NOTIFY.

    (5) CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY - to support multiple Rx packets to be given
        to DDA layer. If this is defined, DDA should provide the callback function for
        multiple packets too.


    DDA specific features:

    (6) CPMAC_USE_CONFIG_SERVICE - to use configuration service to obtain configuration 
        information for the instance. The configuration information is string based and 
        the parsing functions from the config service are used. The config string is as follows:

        "instId=,BaseAddr=,IntrLine=,ResetLine=,BusFreq=,speed=,duplex=,promEn=,BroadEn=,MultiEn=,maxRxPktLen=,
        txNumBD=,txServiceMax=,rxNumBD=,rxServiceMax=,rxBufExtra=,
        mdioBaseAddr=,mdioIntrLine=,mdioResetLine=,mdioBusFreq=,mdioClkFreq=,mdioPhyMask=,mdioTickMsec="

        Example: "instId=0, BaseAddr=0xa8610000, IntrLine=19, ResetLine=17, BusFreq=62500000, 
                speed=100, duplex=1, promEn=1, BroadEn=1, MultiEn=1, maxRxPktLen=1522, 
                txNumBD=256, txServiceMax=64, rxNumBD=256, rxServiceMax=64, rxBufExtra=0
                mdioBaseAddr=0xA8611E00, mdioIntrLine=0, mdioResetLine=22, 
                mdioBusFreq=62500000, mdioClkFreq=2200000, mdioPhyMask=0x80000000, mdioTickMsec=10"

        Note: If speed = 9999 NO PHY mode selected

    (7) CPMAC_USE_ENV - to use ENV variables to get configuration information. 
        When config service is not being used, either hardcoded (static) values can be used for 
        unit testing or ENV variables can be used. The configuration information should be in 
        the following string format (without the string names):

        "instId:BaseAddr:IntrLine:ResetLine:BusFreq:speed:duplex:promEn:BroadEn:MultiEn:maxRxPktLen:
         txNumBD:txServiceMax:rxNumBD:rxServiceMax:rxBufExtra:
         mdioBaseAddr:mdioIntrLine:mdioResetLine:mdioBusFreq:mdioClkFreq:mdioPhyMask:mdioTickMsec:"

        Example: "0:a8610000:19:17:62500000:0:0:1:1:1:1522:96:48:16:8:0:A8611E00:0:22:62500000:2200000:80000000:10";
                 "1:a8612800:33:21:62500000:0:0:1:1:1:1522:96:48:16:8:0:A8611E00:0:22:62500000:2200000:55555555:10";

        Note: If speed = 9999 NO PHY mode selected

    (8) CPMAC_USE_TASKLET_MODE - to use tasklets to process packets. If not defined by the makefile
        default is to use Interrupt mode processing of packets (using DDC calls)

    (9) CPMAC_DDA_CACHE_INVALIDATE_FIX - to use the fix of invalidating the receive buffer before
        providing it to the DMA (to avoid possible data cache corruption if a dirty cache line is
        evicted and gets written after the RX DMA has written to the memory). If not defined, the receive
        buffer is invalidated after the DMA has been done (with possible data corruption). Since there
        is less probability of this condition occuring, the user of the driver can choose to use this
        fix or not. Note that by using this fix, the whole buffer is being invalidated rather than just
        the size of received packet, there is a performance hit if this fix is used.


    (C) Copyright 2004, Texas Instruments, Inc

    @author     (ver 1.6)   Suraj Iyer - Original Linux Driver for CPMAC
                (ver 0.1)   Anant Gole - Re-Coded as per PSPF architecture (converted to DDA)
    @version    0.1

                (ver 0.2)   Sharath Kumar - Incorporated Review comments.
    @version    0.2
    
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <linux/highmem.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <asm/irq.h>            /* For NR_IRQS only. */
#include <asm/bitops.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>

#include <asm/mips-boards/prom.h>
#include <asm/avalanche/generic/if_port.h>
#include "pal.h"
 
#include "cpmacNetLx.h"     /* This will include required DDC headers */
#include "cpmacNetLxCfg.h"

#if defined (CONFIG_MIPS_AVALANCHE_STATIC_SKB)
struct sk_buff *ti_alloc_skb(unsigned int size,int gfp_mask);
#endif


/* Allocate RX buffer */
Ptr DDA_cpmac_net_alloc_rx_buf(CpmacNetDevice* hDDA, Int bufSize, DDC_NetDataToken *dataToken, 
                                  Uint32 channel, Ptr allocArgs)
{
    struct net_device *p_dev = hDDA->owner;
    struct sk_buff *p_skb;


#if defined (CONFIG_MIPS_AVALANCHE_STATIC_SKB)
    p_skb = ti_alloc_skb(hDDA->rxBufSize,GFP_ATOMIC);
#else
    p_skb = dev_alloc_skb(hDDA->rxBufSize);
#endif
    if(p_skb == NULL)
    {
#ifdef CPMAC_DDA_DEBUG  /* We dont want the error printf to appear on screen as it clogs the serial port */
        /* errPrint("Failed to allocate skb for %s.\n", p_dev->name); */
#endif
        return (NULL);
    }

    /* Set device pointer in skb and reserve space for extra bytes */
    p_skb->dev = p_dev;
    skb_reserve(p_skb, hDDA->rxBufOffset);

    /* Set the data token */
    *dataToken = (DDC_NetDataToken) p_skb;

#ifdef CPMAC_DDA_CACHE_INVALIDATE_FIX
    /* Invalidate buffer */
    CPMAC_DDA_CACHE_INVALIDATE((unsigned long)ret_ptr, bufSize);    
#endif


    return p_skb->data;
}

/* Free RX buffer */
PAL_Result DDA_cpmac_net_free_rx_buf(CpmacNetDevice* hDDA, Ptr buffer, DDC_NetDataToken dataToken, 
                                        Uint32 channel, Ptr freeArgs)
{
    dev_kfree_skb_any((struct sk_buff *) dataToken);
    return (CPMAC_SUCCESS);
}

#if 0
#define isprint(a) ((a >=' ')&&(a<= '~'))
void xdump( unsigned char*  cp, int  length, char*  prefix )
{
    int col, count;
    u_char prntBuf[120];
    u_char*  pBuf = prntBuf;
    count = 0;
    while(count < length){
        pBuf += sprintf( pBuf, "%s", prefix );
        for(col = 0;count + col < length && col < 16; col++){
            if (col != 0 && (col % 4) == 0)
                pBuf += sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "%02X ", cp[count + col] );
        }
        while(col++ < 16){      /* pad end of buffer with blanks */
            if ((col % 4) == 0)
                sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "   " );
        }
        pBuf += sprintf( pBuf, "  " );
        for(col = 0;count + col < length && col < 16; col++){
            if (isprint((int)cp[count + col]))
                pBuf += sprintf( pBuf, "%c", cp[count + col] );
            else
                pBuf += sprintf( pBuf, "." );
                }
        sprintf( pBuf, "\n" );
        // SPrint(prntBuf);
        printk(prntBuf);
        count += col;
        pBuf = prntBuf;
    }

}  /* close xdump(... */
#endif

/* Receive Packet */
PAL_Result DDA_cpmac_net_rx(CpmacNetDevice* hDDA, DDC_NetPktObj *pkt, Ptr rxArgs, Ptr arg)
{
    struct sk_buff *p_skb = (struct sk_buff *) pkt->pktToken;
#if defined(CONFIG_MIPS_AVALANCHE_MARVELL_6063)       
    struct net_device *p_cpmac_net_device= (struct net_device *)hDDA->owner;
#endif
    skb_put(p_skb, pkt->pktLength);

#ifndef CPMAC_DDA_CACHE_INVALIDATE_FIX
    /* Invalidate cache */
    CPMAC_DDA_CACHE_INVALIDATE(p_skb->data, pkt->pktLength);    
#endif

    /* Remove the header or trailer from the frame if coming from a Ethernet Switch */
#if defined(CONFIG_MIPS_AVALANCHE_MARVELL_6063)       
    /* Fetch the receiving port information */         
    p_cpmac_net_device->if_port = (unsigned char)p_skb->data[pkt->pktLength -(EGRESS_TRAILOR_LEN-1)] + AVALANCHE_MARVELL_BASE_PORT_ID;
    skb_trim(p_skb, pkt->pktLength - EGRESS_TRAILOR_LEN);                                                                       
#endif

#ifndef TI_SLOW_PATH
    if(p_skb->dev->br_port == NULL)
#endif
        p_skb->protocol = eth_type_trans(p_skb, hDDA->owner);

    p_skb->dev->last_rx = jiffies;
    netif_rx(p_skb);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(hDDA->ledHandle, CPMAC_RX_ACTIVITY);  
#endif

    hDDA->netDevStats.rx_packets++;
    hDDA->netDevStats.rx_bytes += pkt->pktLength;

    return(0);
}

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
/* 
 * Multiple packet receive 
 * 
 * This function get multiple received packets via the netPktList and it queues these packets into 
 * the higher layer queue
 *
 * Note that rxArgs contains "channel" and is ignored for this implementation
*/
PAL_Result DDA_cpmac_net_rx_multiple_cb(CpmacNetDevice* hDDA, DDC_NetPktObj *netPktList, Int numPkts, Ptr rxArgs)
{
    Uint32  cnt;

    for (cnt=0; cnt < numPkts; cnt++)
    {
        struct sk_buff *p_skb = (struct sk_buff *) netPktList->pktToken;
#if defined(CONFIG_MIPS_AVALANCHE_MARVELL_6063)
        struct net_device *p_cpmac_net_device= (struct net_device *)hDDA->owner;
#endif
        /* Set length of packet */
        skb_put(p_skb, netPktList->pktLength);
 

#ifndef CPMAC_DDA_CACHE_INVALIDATE_FIX
        /* Invalidate cache */
        CPMAC_DDA_CACHE_INVALIDATE(p_skb->data, p_skb->len);    
#endif

        /* Remove the header or trailer from the frame if coming from a Ethernet Switch */
#if defined(CONFIG_MIPS_AVALANCHE_MARVELL_6063)
        /* Fetch the receiving port information */
        p_cpmac_net_device->if_port = (unsigned char)p_skb->data[netPktList->pktLength - (EGRESS_TRAILOR_LEN-1)] + AVALANCHE_MARVELL_BASE_PORT_ID;
        skb_trim(p_skb, netPktList->pktLength - EGRESS_TRAILOR_LEN);
#endif


#ifndef TI_SLOW_PATH
    if(p_skb->dev->br_port == NULL)
#endif
        p_skb->protocol = eth_type_trans(p_skb, hDDA->owner); 

        p_skb->dev->last_rx = jiffies;
        netif_rx(p_skb);

        hDDA->netDevStats.rx_bytes += netPktList->pktLength;
        ++netPktList;
    }

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(hDDA->ledHandle, CPMAC_RX_ACTIVITY);  
#endif

    hDDA->netDevStats.rx_packets += numPkts;
    return (0);
}
#endif /*  CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY */


/* Transmit Complete Callback */
PAL_Result DDA_cpmac_net_tx_complete(CpmacNetDevice* hDDA, DDC_NetDataToken *netDataTokens,
                                         Int numTokens, Uint32 channel)
{
    Uint32  cnt;

    if(numTokens && netif_queue_stopped(hDDA->owner))
        netif_start_queue(hDDA->owner);

    for (cnt = 0; cnt < numTokens; cnt++)
    {
        struct sk_buff *skb = (struct sk_buff *) netDataTokens[cnt];
        if(skb == NULL) continue;

        hDDA->netDevStats.tx_packets++;
        hDDA->netDevStats.tx_bytes += skb->len;

        dev_kfree_skb(skb);
    }

    return(0);
}


/******************************************************************************
 *  Interrupt / Tasklet related functions
 *****************************************************************************/

/* DDA ISR */
void cpmac_hal_isr(int irq, void *dev_id, struct pt_regs *regs)
{
    CpmacNetDevice *hDDA = (CpmacNetDevice *)dev_id;

    ++hDDA->isrCount;

    if (!hDDA->setToClose)
    {
#ifdef CPMAC_USE_TASKLET_MODE
        tasklet_schedule(&hDDA->tasklet);
#else
        /* Interrupt mode packet processing */
        int pkts_pending = 0;
        hDDA->ddcIf->pktProcess(hDDA->hDDC, &pkts_pending, NULL);
        hDDA->ddcIf->pktProcessEnd(hDDA->hDDC, NULL);
#endif
    }
    else ; /* We are closing down, so dont process anything */
}

#ifdef CPMAC_USE_TASKLET_MODE
/* DDA taslet mode packet processing */
void cpmac_handle_tasklet(unsigned long data)
{
    CpmacNetDevice *hDDA = (CpmacNetDevice *)data;
    int pkts_pending = 0;

    if (!hDDA->setToClose)
    {
        /* Process packets - Call the DDC packet processing function */
        hDDA->ddcIf->pktProcess(hDDA->hDDC, &pkts_pending, NULL);

        /* If more packets reschedule the tasklet or call pktProcessEnd */
        if(!pkts_pending)
        {
            hDDA->ddcIf->pktProcessEnd(hDDA->hDDC, NULL);
        }
        else if (!test_bit(0, &hDDA->setToClose))
        {
            tasklet_schedule(&hDDA->tasklet);
        }
        else ; /* We are closing down, so dont process anything */
    }
}

#endif /* CPMAC_USE_TASKLET_MODE */

/* Transmit Function - Only single fragment supported */
int cpmac_dev_tx( struct sk_buff *skb, struct net_device *p_dev)
{
    PAL_Result retCode;
    DDC_NetBufObj txBuf;        /* Buffer object - Only single frame support */
    DDC_NetPktObj txPacket;     /* Packet object */
    CpmacNetDevice *hDDA  = p_dev->priv;
    
   /* Build the buffer and packet objects - Since only single fragment is supported, 
     * need not set length and token in both packet & object. Doing so for completeness sake & to
     * show that this needs to be done in multifragment case 
     */
    txPacket.bufList    = &txBuf;
    txPacket.numBufs    = 1;   /* Only single fragment supported */
    txPacket.pktLength  = skb->len;
    txPacket.pktToken   = (DDC_NetDataToken) skb;
    txBuf.length        = skb->len;
    txBuf.bufToken      = (DDC_NetDataToken) skb;
    txBuf.dataPtr       = skb->data;

    /* Flush data buffer if write back mode */
    CPMAC_DDA_CACHE_WRITEBACK((unsigned long)skb->data, skb->len);    

    p_dev->trans_start = jiffies;
   
    /* DDC Send : last param FALSE so that hardware calculates CRC */
    retCode = hDDA->ddcIf->ddcNetIf.ddcNetSend(hDDA->hDDC, &txPacket, CPMAC_DDA_DEFAULT_TX_CHANNEL, False);
    if (retCode != CPMAC_SUCCESS)
    {

        if(retCode == CPMAC_ERR_TX_OUT_OF_BD) 
            netif_stop_queue(hDDA->owner);
        
        hDDA->netDevStats.tx_errors++;  
        goto cpmac_dev_tx_drop_pkt; 
    }

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(hDDA->ledHandle, CPMAC_TX_ACTIVITY);
#endif

    return (0);

cpmac_dev_tx_drop_pkt:
    hDDA->netDevStats.tx_dropped++;
    return (-1);
}


