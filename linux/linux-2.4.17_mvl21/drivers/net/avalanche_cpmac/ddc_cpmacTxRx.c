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

/** \file   ddc_cpmacTxRx.c
    \brief  DDC CPMAC Send/Receive functionality Source file

    This file contains the send/receive core functionality for CPMAC/CPGMAC 
    device based upon PSP Framework architecture.

    Acknowledgements: This DDC implementation for CP(G)MAC device is based upon
    HAL 2.0 based device driver for CPMAC from BSTC. To benefit from the concepts 
    and implementation experience of HAL development, some code snippets have been 
    ported from the BSTC HAL implementation.
    
    Notes:
    DDA/DDC Common features:
    (1) CPMAC_CACHE_WRITEBACK_MODE should be defined by the make file to support
        write back cache mode
    (2) CPMAC_MULTIFRAGMENT should be defined by the make file to support
        multifragments
    (3) CPMAC_RX_RECYCLE_BUFFER - should be defined by the makefile for Receive 
        buffer recycling mechanism (as explained below)
        At initialization, DDC requests buffers from DDA layer and attaches them to RD BD's.
        When a packet is received, the buffer can be recycled along with the BD or a new buffer
        can be allocated. If buffer recycling is used, the upper layer has to call the DDC Buffer
        Return function and return the buffer/BD back to the RX pool.
        This macro controls this feature. If defined the upper layer will call the DDC return
        function and the BD/buffer will always be connected untill freed.
        If no defined, DDC will get a new buffer from the DDA layer.
    (4) CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY - should be defined by the makefile for 
        support of multiple Tx complete notifications. If this is defined the Tx complete
        DDA callback function contains multiple packet Tx complete events. 
        IMPORTANT NOTE: The configuration define CPMAC_DDC_MAX_TX_COMPLETE_PKTS_TO_NOTIFY should
        be greater than max tx service pkts.
    (5) CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY - should be defined by the makefile for 
        support of multiple Receive packet. If this is defined the multiple packet receive 
        DDA callback function contains multiple packet Rx packets. 
        IMPORTANT NOTE: The configuration define CPMAC_DDC_MAX_RX_COMPLETE_PKTS_TO_NOTIFY should
        be greater than max rx service pkts.

    DDC specific features:
    (1) CPMAC_DDC_USE_ASSEMBLY should be defined by the makefile if the compiler supports assembly
        code in the DDC implementation. Note that currently only MIPS assembly is supported here.

    (2) CPMAC_DDC_DEBUG should be defined by the makefile for compiling in debug statements

    (C) Copyright 2004, Texas Instruments, Inc

    @author     Anant Gole
    @version    0.1
                (ver 0.2)   Sharath Kumar - Incorporated review comments
    @version    0.2
 */

/* Includes */
#define CPMAC_DDC               /* Define this to get the right header contents */

#include "pal.h"            /* PAL SYS services required */
#include "ddc_netdev.h"         /* Network Device Interface - includes DDC Interface */
#include "ddc_cpmacDrv.h"       /* CPMAC Driver DDC Internal Data Structures */

#ifdef CPMAC_DDC_DEBUG /*  Used only for debug printing  */
/* Static Global Strings */
static char *CpmacTxHostErrorCodes[16] = 
{
    /* 0000 */ "No error",
    /* 0001 */ "SOP error",
    /* 0010 */ "Ownership bit not set in SOP buffer",
    /* 0011 */ "Zero Next Buffer Descriptor Pointer Without EOP",
    /* 0100 */ "Zero Buffer Pointer",
    /* 0101 */ "Zero Buffer Length",
    /* 0110 */ "Packet Length Error",
    /* 0111 */ "Reserved",
    /* 1000 */ "Reserved",
    /* 1001 */ "Reserved",
    /* 1010 */ "Reserved",
    /* 1011 */ "Reserved",
    /* 1100 */ "Reserved",
    /* 1101 */ "Reserved",
    /* 1110 */ "Reserved",
    /* 1111 */ "Reserved"
};

static char *CpmacRxHostErrorCodes[16] = 
{
    /* 0000 */ "No error",
    /* 0001 */ "Reserved",
    /* 0010 */ "Ownership bit not set in input buffer",
    /* 0011 */ "Reserved",
    /* 0100 */ "Zero Buffer Pointer",
    /* 0101 */ "Reserved",
    /* 0110 */ "Reserved",
    /* 0111 */ "Reserved",
    /* 1000 */ "Reserved",
    /* 1001 */ "Reserved",
    /* 1010 */ "Reserved",
    /* 1011 */ "Reserved",
    /* 1100 */ "Reserved",
    /* 1101 */ "Reserved",
    /* 1110 */ "Reserved",
    /* 1111 */ "Reserved"
};
#endif

/**
 * CPMAC DDC Periodic Timer (Tick) Function
 *  - calls PHY polling function
 *  - If status changed, invokes DDA callback to propogate PHY / Devicestatus
 * 
 * \note "tickArgs" is not used in this implementation
 */
PAL_Result cpmacTick (CpmacDDCObj *hDDC, Ptr tickArgs)
{
    /* Verify proper device state */
    if (hDDC->ddcObj.state != DDC_OPENED)
    {
        return (CPMAC_ERR_DEV_NOT_OPEN);
    }

    if( !(hDDC->initCfg.phyMode & SNWAY_NOPHY) )  /* If NOPHY specified do not check */
    {
        /* Opened and Phy available */
        int tickChange;

        tickChange = cpswHalCommonMiiMdioTic(hDDC->PhyDev);
        if(tickChange == 1) /*  MDIO indicated a change  */
        {
            cpmacUpdatePhyStatus(hDDC);
            hDDC->ddaIf->ddaNetIf.ddaFuncTable.ddaControlCb(hDDC->ddcObj.hDDA, CPMAC_DDA_IOCTL_STATUS_UPDATE, (Ptr) &hDDC->status, NULL);
        }
        else if ((hDDC->initCfg.phyMode & SNWAY_AUTOMDIX) && (tickChange & _MIIMDIO_MDIXFLIP))
        {
            avalanche_set_mdix_on_chip(hDDC->initCfg.baseAddress, tickChange & 0x1);
        }
    }

    return(CPMAC_SUCCESS);
}

/**
 * CPMAC DDC Packet processing function
 *  - Detects if there are host errors and invokes the DDA callback to inform
 *    the DDA layer about the hardware error.
 *
 */
LOCAL void cpmacProcessHostError(CpmacDDCObj *hDDC)
{
    Uint32  channel = 0;
    Uint32  vector = 0;
    Uint32  status = 0;

    /* The MacStatus register bits starting from Rx error channel have been mapped to hwErrInfo LSB 16 bits */
    status = hDDC->regs->MacStatus;

    /* TX: Reading the channel and cause (vector variable being re-used) */
    channel = CSL_FMK(CPMAC_MACSTATUS_TXERRCH, status);
    hDDC->status.hwErrInfo = channel << 16; /* Tx error channel in MSB 16 bits */
    vector = CSL_FMK(CPMAC_MACSTATUS_TXERRCODE, status);

    if (vector) 
    {
         hDDC->status.hwStatus = CPMAC_DDC_TX_HOST_ERROR;
         LOGERR("\nERROR:DDC: DDC_cpmacPktProcess:%d: Ch=%d, CPMAC_DDC_TX_HOST_ERROR. Cause=%s", 
            hDDC->ddcObj.instId, hDDC->status.hwErrInfo, CpmacTxHostErrorCodes[vector]);
    }

    /* RX: Reading the channel and cause (vector variable being re-used) */
    channel = CSL_FMK(CPMAC_MACSTATUS_RXERRCH, status);
    hDDC->status.hwErrInfo |= channel; /* Rx error channel in LSB 16 bits */
    vector = CSL_FMK(CPMAC_MACSTATUS_RXERRCODE, status);

    if (vector) 
    {
        hDDC->status.hwStatus = CPMAC_DDC_RX_HOST_ERROR;
        LOGERR("\nERROR:DDC: DDC_cpmacPktProcess:%d: Ch=%d, CPMAC_DDC_RX_HOST_ERROR. Cause=%s", 
            hDDC->ddcObj.instId, hDDC->status.hwErrInfo, CpmacRxHostErrorCodes[vector]);
    }

    /* Inform DDA layer about this critical failure */
    hDDC->ddaIf->ddaNetIf.ddaFuncTable.ddaControlCb(hDDC->ddcObj.hDDA, CPMAC_DDA_IOCTL_STATUS_UPDATE, (Ptr) &hDDC->status, NULL);
}

/**
 * CPMAC DDC Packet processing function
 *  - Reads the device interrupt status and invokes TX/RX BD processing function
 *  - Also detects if there are host errors and invokes the DDA callback to inform
 *    the DDA layer about the hardware error.
 *
 * \note "pktsPending" will contain number of packets still to be processed (TX + RX)
 */
Int cpmacPktProcess (CpmacDDCObj *hDDC, Int *pktsPending, Ptr pktArgs)
{
    Uint32  channel = 0;
    Uint32  vector = 0;
    Uint32  status = 0;

    vector = hDDC->regs->Mac_In_Vector;

    /* Handle Packet Transmit completion */
    if (vector & CPMAC_MAC_IN_VECTOR_TX_INT_OR)
    {
        Bool isEOQ;
       
        channel = (vector & 0x7);
        cpmacTxBDProc(hDDC, channel, &status,&isEOQ);
        if(hDDC->txInterruptDisable == True)
        {
           if( !status && isEOQ)
            /* Disable Channel interrupt */
            hDDC->regs->Tx_IntMask_Clear = (1 << channel); 
        }
        
        *pktsPending = status;
    }
    
    /* Handle RX packets first -  the thought process in this is that the received packets
     * will be handled immediately reducing the latency (- but an equally opposite argument 
     * can also be made)
     */
    if (vector & CPMAC_MAC_IN_VECTOR_RX_INT_OR)
    {
        channel = (vector >> 8) & 0x7;
        CpmacRxBDProc(hDDC, channel, &status);
        *pktsPending |= status;
    }

    /* Handle host errors - being handled last does not mean its of least priority */
    if (vector & CPMAC_MAC_IN_VECTOR_HOST_INT)
    {
        cpmacProcessHostError(hDDC);
    }
    return (CPMAC_SUCCESS);

}

/**
 *  CPMAC function indicating the cause of interrupt (to be called by the DDA)
 *  DDA calls this function on occurance of interrupt to know the cause for the interrupt(Rx/Tx).
 */
void cpmacGetInterruptCause(CpmacDDCObj *hDDC,Bool *rxPending,Bool *txPending,Ptr causeArgs)
{
    Uint32  vector = 0;
   
    vector = hDDC->regs->Mac_In_Vector;

    /* Check Packet Transmit completion */
    if (vector & CPMAC_MAC_IN_VECTOR_TX_INT_OR)
    {
        *txPending = True;
    }
    else
    {
        *txPending = False;
    }

    /* Check RX packets */
    if (vector & CPMAC_MAC_IN_VECTOR_RX_INT_OR)
    {
        *rxPending = True;
    }
    else
    {
        *rxPending = False;
    }


}

/**
 * CPMAC DDC TxPacket Completion processing function
 *  - Invokes TX  BD processing function
 *  - Also detects if there are host errors and invokes the DDA callback to inform
 *    the DDA layer about the hardware error.
 *
 * \note "pktsPending" will contain number of Tx completion packets still to be processed
 */
Int cpmacTxPktCompletionProcess (CpmacDDCObj *hDDC, Int *pktsPending,Ptr pktArgs)
{
    Uint32  channel = 0;
    Uint32  vector = 0;
    Uint32  status = 0;

    vector = hDDC->regs->Mac_In_Vector;

    /* Handle Packet Transmit completion */
    if (vector & CPMAC_MAC_IN_VECTOR_TX_INT_OR)
    {
        Bool isEOQ;
    
        channel = (vector & 0x7);
        cpmacTxBDProc(hDDC, channel, &status, &isEOQ);
        *pktsPending = status;
    }

    /* Handle host errors - being handled last does not mean its of least priority */
    if (vector & CPMAC_MAC_IN_VECTOR_HOST_INT)
    {
        cpmacProcessHostError(hDDC);
    }

    return (CPMAC_SUCCESS);
}

/**
 * CPMAC DDC RxPacket processing function
 *  - Invokes RX BD processing function
 *  - Also detects if there are host errors and invokes the DDA callback to inform
 *    the DDA layer about the hardware error.
 *
 * \note "pktsPending" will contain number of Rx packets still to be processed
 */
Int cpmacRxPktProcess (CpmacDDCObj *hDDC, Int *pktsPending, Ptr pktArgs)
{
    Uint32  channel = 0;
    Uint32  vector = 0;
    Uint32  status = 0;

    vector = hDDC->regs->Mac_In_Vector;

    /* Handle RX packets first -  the thought process in this is that the received packets
     * will be handled immediately reducing the latency (- but an equally opposite argument 
     * can also be made)
     */
    if (vector & CPMAC_MAC_IN_VECTOR_RX_INT_OR)
    {
        channel = (vector >> 8) & 0x7;
        CpmacRxBDProc(hDDC, channel, &status);
        *pktsPending = status;
    }

    /* Handle host errors - being handled last does not mean its of least priority */
    if (vector & CPMAC_MAC_IN_VECTOR_HOST_INT)
    {
        cpmacProcessHostError(hDDC);
    }

    return (CPMAC_SUCCESS);
}


/**
 * CPMAC DDC Signal Packet processing end to hardware
 *  - programs the EOI vector register so that if there are pending packets in hardware queue
 *    an interrupt can be generated by the hardware
 */
/* Packet processing end */
Int cpmacPktProcessEnd (CpmacDDCObj *hDDC, Ptr procArgs)
{
    hDDC->regs->Mac_EOI_Vector = 0;
    return (CPMAC_SUCCESS);    
}


#ifndef CPMAC_MULTIFRAGMENT    
/***********************************************************************************************************
 * SINGLE-FRAGMENT SUPPORT HERE
 **********************************************************************************************************/

/**
 * CPMAC DDC Send/Transmit function
 *  - Queues the packet provided by DDA into hardware queue
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 *
 * \note If "sendArgs" is True (non zero) CRC is calculated by DDA or upper layer and not by hardware
 *       and is part of the packet data send to this function
 */

PAL_Result DDC_cpmacSend(CpmacDDCObj *hDDC, DDC_NetPktObj *pkt, Int  channel, Ptr sendArgs)
{
    PAL_Result      retVal = CPMAC_SUCCESS;
    CpmacTxBD       *currBD;
    CpmacTxCppiCh   *txCppi;
    DDC_NetBufObj   *bufList;
    Uint32          lockKey;
	
    /* Verify proper device state */
    if (hDDC->ddcObj.state != DDC_OPENED)
        return (CPMAC_ERR_DEV_NOT_OPEN);
    
    /* Validate channel number and get channel control structure */    
    if (channel > CPMAC_MAX_TX_CHANNELS)
        return (CPMAC_ERR_TX_CH_INVALID);

    if (hDDC->txIsOpen[channel] != True)
        return (CPMAC_ERR_TX_CH_NOT_OPEN);

    /* Check ethernet link state. If not linked, return error */
    if (!hDDC->status.PhyLinked)
        return (CPMAC_ERR_TX_NO_LINK);

    txCppi = hDDC->txCppi[channel];
    bufList = pkt->bufList;     /* Get handle to the buffer array */

    /* Check packet size and if < CPMAC_MIN_ETHERNET_PKT_SIZE, pad it up */
    if (pkt->pktLength < CPMAC_MIN_ETHERNET_PKT_SIZE)
    {
      bufList->length += (CPMAC_MIN_ETHERNET_PKT_SIZE - pkt->pktLength);
      pkt->pktLength = CPMAC_MIN_ETHERNET_PKT_SIZE;
    }

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Send Critical Section */
    
    /* Only one Tx BD for the packet to be sent */
    currBD = txCppi->bdPoolHead;
    if (currBD == NULL)
    {
#ifdef CPMAC_DDC_GETSTATS
        txCppi->outOfTxBD++;
#endif
        retVal = CPMAC_ERR_TX_OUT_OF_BD;
        goto Exit_DDC_cpmacSend;
    }
    txCppi->bdPoolHead = currBD->next;

    /* Populate the BD contents to be added to the TX list */
    currBD->bufToken = bufList->bufToken;
    currBD->buffPtr  = PAL_CPMAC_VIRT_2_PHYS((Int32 *)bufList->dataPtr);
    currBD->off_bLen = bufList->length;
    currBD->hNext    = 0;
    currBD->next     = 0;
    currBD->mode     = (CPMAC_CPPI_SOP_BIT | CPMAC_CPPI_OWNERSHIP_BIT | CPMAC_CPPI_EOP_BIT | pkt->pktLength);

    if ((Bool)sendArgs == True) 
        currBD->mode |= CPMAC_CPPI_PASS_CRC_BIT;

    /* Flush the packet from cache if write back cache is present */
    PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);

    /* Send the packet */
    if (txCppi->activeQueueHead == 0)
    {
        txCppi->activeQueueHead = currBD;
        txCppi->activeQueueTail = currBD;
        if (txCppi->queueActive != True)
        {
            hDDC->regs->Tx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(currBD);
            txCppi->queueActive = True;
        }
#ifdef CPMAC_DDC_GETSTATS
        ++txCppi->queueReinit;
#endif
    }
    else
    {
        register volatile CpmacTxBD *tailBD;
        register Uint32 frameStatus;

        tailBD = txCppi->activeQueueTail;
        tailBD->next = currBD;
        txCppi->activeQueueTail = currBD;

        tailBD = PAL_CPMAC_VIRT_NOCACHE(tailBD);
        tailBD->hNext = (Int32) PAL_CPMAC_VIRT_2_PHYS(currBD);
        frameStatus = tailBD->mode;
        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            frameStatus &= ~(CPMAC_CPPI_EOQ_BIT);
            tailBD->mode = frameStatus;
			if(hDDC->txInterruptDisable == True)
			{
				volatile unsigned int txCompletionPtr;
	      		txCompletionPtr = hDDC->regs->Tx_CP[channel];
				hDDC->regs->Tx_CP[channel] = txCompletionPtr;
			}

            hDDC->regs->Tx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(currBD);
            
#ifdef CPMAC_DDC_GETSTATS
            ++txCppi->endOfQueueAdd;
#endif
                
        }
        else
        {
            if(hDDC->txInterruptDisable == True)
            {
                /* Enable Channel interrupt */
                hDDC->regs->Tx_IntMask_Set = (1 << channel);
                        
            }
        }
    }
    
#ifdef CPMAC_DDC_GETSTATS
    txCppi->activeQueueCount++;
#endif 
        
Exit_DDC_cpmacSend:
	
    
    if(hDDC->txInterruptDisable == True)
    {
        if( --hDDC->txIntThreshold[channel] <=  0)
        {
            Bool isEOQ;
            int status;
            cpmacTxBDProc(hDDC, channel, &status,&isEOQ);
            hDDC->txIntThreshold[channel] = hDDC->txCppi[channel]->chInfo.serviceMax;
        }
    }

    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */
    
    return (retVal);    
}

/**
 * CPMAC DDC TX Buffer Descriptor processing
 *  - processes transmit completed packets and returns the handles to DDA layer
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 *
 * \note returns number of pkts processed and 1 in morePkts if pkt completion processing pending
 */
Int cpmacTxBDProc(CpmacDDCObj *hDDC,  Uint32 channel, Uint32 *morePkts, Bool * isEOQ)
{
    CpmacTxBD      *currBD;
    CpmacTxCppiCh  *txCppi;
    Uint32          frameStatus;
    Uint32          pktsProcessed = 0;
    Uint32          lockKey;

#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    Uint32          txCompleteCnt = 0;
    Uint32          *txCompletePtr;
#endif

    *morePkts = 0;
    *isEOQ = True;

    /* Here no need to validate channel number, since it is taken from the interrupt register 
       Instead channel structure should be validated */
    if (hDDC->txIsOpen[channel] == False)
        return (CPMAC_ERR_TX_CH_NOT_OPEN);

    if (hDDC->txTeardownPending[channel] == True)
    {
        return (CPMAC_SUCCESS);     /* Dont handle any packet completions */
    }
    txCppi = hDDC->txCppi[channel];
#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    txCompletePtr = &txCppi->txComplete[0];
#endif

#ifdef CPMAC_DDC_GETSTATS
     ++txCppi->procCount;
#endif 
    
    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Send Critical Section */

    /* Get first BD to process */
    currBD = txCppi->activeQueueHead;
    if (currBD == 0)
    {
        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */
        hDDC->regs->Tx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(txCppi->lastHwBDProcessed); 
#ifdef CPMAC_DDC_GETSTATS
        txCppi->noActivePkts++;
#endif 
        
        return(CPMAC_SUCCESS);
    }

    /* Invalidate BD */
    PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
    frameStatus = currBD->mode;

    while ( (currBD) &&
            ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) && 
             (pktsProcessed < txCppi->chInfo.serviceMax))
    {
        hDDC->regs->Tx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(currBD); 
        txCppi->activeQueueHead = currBD->next;

        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            if (currBD->next)     /* Misqueued Packet */
            {
                hDDC->regs->Tx_HDP[channel] = currBD->hNext;
				/*  Enable Channel Interrupts, to ensure packet goes out on wire */
				hDDC->regs->Tx_IntMask_Set = (1 << channel);
#ifdef CPMAC_DDC_GETSTATS
                ++txCppi->misQueuedPackets;
#endif     
            }
            else            /* End of queue */
            {
                txCppi->queueActive = False;
            }
        }
    
#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
        *txCompletePtr = (Uint32) currBD->bufToken;
        ++txCompletePtr;
        ++txCompleteCnt;
#else 
        /* Single packet TX complete notify - This function is called in the Send Critical Section Context */
        hDDC->ddaIf->ddaNetIf.ddaNettxCompleteCb(hDDC->ddcObj.hDDA, &currBD->bufToken, 1, (Ptr)channel);
#endif
        currBD->next = txCppi->bdPoolHead;
        txCppi->bdPoolHead = currBD;
#ifdef CPMAC_DDC_GETSTATS
         --txCppi->activeQueueCount;
#endif 
       
        pktsProcessed++;
    
        txCppi->lastHwBDProcessed = currBD;
        currBD = txCppi->activeQueueHead;
        if (currBD)
        {
            PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
            frameStatus = currBD->mode;
        }
    } /* End of while loop */
    
    
    if ( (currBD) && ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) )
    {
        *morePkts = 1;
    }

    if (currBD)  /* This check is same as  check for EOQ i.e framestatus & CPMAC_CPPI_EOQ_BIT */
    {
        *isEOQ = False;
    }
       
#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    /* Multiple packet TX complete notify - This function is NOT called in the Send Critical Section Context */
    hDDC->ddaIf->ddaNetIf.ddaNettxCompleteCb(hDDC->ddcObj.hDDA, &txCppi->txComplete[0], txCompleteCnt, (Ptr) channel);
#endif
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */

    return (pktsProcessed);
}

/**
 * CPMAC DDC Add Buffer to RX queue function
 *  - returns the BD to the Receive queue
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 */
void cpmacAddBDToRxQueue(CpmacDDCObj *hDDC, CpmacRxCppiCh *rxCppi, CpmacRxBD *currBD, Char *buffer, DDC_NetDataToken bufToken)
{
    /* Populate the hardware descriptor */
    currBD->hNext = 0;
    currBD->buffPtr = PAL_CPMAC_VIRT_2_PHYS(buffer);
    currBD->off_bLen = rxCppi->chInfo.bufSize;
    currBD->mode = CPMAC_CPPI_OWNERSHIP_BIT;
    currBD->next = 0;
    currBD->dataPtr = buffer;
    currBD->bufToken = bufToken;

    /* Write back  */
    PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);

    if (rxCppi->activeQueueHead == 0)
    {
        rxCppi->activeQueueHead = currBD;
        rxCppi->activeQueueTail = currBD;
        if (rxCppi->queueActive != False)
        {
            rxCppi->hDDC->regs->Rx_HDP[rxCppi->chInfo.chNum] = PAL_CPMAC_VIRT_2_PHYS(rxCppi->activeQueueHead);
            rxCppi->queueActive = True;
        }
    }
    else
    {
        CpmacRxBD   *tailBD;
        Uint32      frameStatus;
        Uint32      lockKey;

        PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Receive Critical Section */
        tailBD = rxCppi->activeQueueTail;
        rxCppi->activeQueueTail = currBD;
        tailBD->next = (Ptr) currBD;
        tailBD = PAL_CPMAC_VIRT_NOCACHE(tailBD);
        tailBD->hNext = PAL_CPMAC_VIRT_2_PHYS(currBD);
        frameStatus = tailBD->mode;
        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            hDDC->regs->Rx_HDP[rxCppi->chInfo.chNum] = PAL_CPMAC_VIRT_2_PHYS(currBD);
            frameStatus &= ~(CPMAC_CPPI_EOQ_BIT);
            tailBD->mode = frameStatus;
#ifdef CPMAC_DDC_GETSTATS
         ++rxCppi->endOfQueueAdd;
#endif      
        }
        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Receive Critical Section */
    }
    
 #ifdef CPMAC_DDC_GETSTATS
     ++rxCppi->recycledBD;  /* Maintain statistics of how many BD's were queued back - recycled */
#endif    

}


/**
 * CPMAC DDC RX Buffer Descriptor processing
 *  - processes received packets and passes them to DDA layer
 *  - requeues the buffer descriptor to the receive pool
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 */
Int CpmacRxBDProc(CpmacDDCObj *hDDC,  Uint32 channel, Int32 *morePkts)
{
    CpmacRxCppiCh   *rxCppi;
    CpmacRxBD       *currBD, *lastBD;
    Uint32          frameStatus;
    Char            *newBuffer;
    DDC_NetDataToken newBufToken;
    DDC_NetBufObj   *rxBufObj;
    Uint32          pktsProcessed;
    DDC_NetPktObj   *currPkt;
    Uint32          recycle_pkt;

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
    Uint32          rxCompleteCnt = 0;
#endif


    
    /* Here no need to validate channel number, since it is taken from the interrupt register 
       Instead channel structure should be validated */
    if (hDDC->rxIsOpen[channel] == False)
    {
        *morePkts = 0;
        return (CPMAC_ERR_RX_CH_NOT_OPEN);
    }

    /* Check if channel teardown pending */
    rxCppi = hDDC->rxCppi[channel];
    if (hDDC->rxTeardownPending[channel] == True)
    {
        *morePkts = 0;
        return (0);
    }

#ifdef CPMAC_DDC_GETSTATS
   ++rxCppi->procCount;
#endif   
  

    *morePkts = 0;
    pktsProcessed = 0;

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
    currPkt = &rxCppi->pktQueue[0];
#else
    currPkt = &rxCppi->pktQueue;
#endif

    currBD = rxCppi->activeQueueHead;
    PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
    frameStatus = currBD->mode;

    while ( (currBD) &&
            ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) && 
            (pktsProcessed < rxCppi->chInfo.serviceMax) )
    {
	    recycle_pkt = 0;

        /* Allocate new buffer */
        newBuffer = (Ptr)(hDDC->ddaIf->ddaNetIf.ddaNetAllocRxBufCb(hDDC->ddcObj.hDDA, rxCppi->chInfo.bufSize, &newBufToken, NULL));
        if (newBuffer == NULL)
        {
            /* No buffer available. Return error with packets pending */
#ifdef CPMAC_DDC_GETSTATS
            ++rxCppi->outOfRxBuffers;
#endif
	    /* 
	     * This was discussed at length. Not sure why this did not make it here earlier. Nevertheless, 
	     * the code is being now implemented. Since, we can not get new buffer, we are recycling the 
	     * current buffer. As discussed many times, this is a simple solution. 
	     */ 
	    newBuffer   = currBD->dataPtr;
	    newBufToken = currBD->bufToken;   

	    recycle_pkt = 1;
        }
	else
        {
            /* Populate received packet data structure */
            rxBufObj = &currPkt->bufList[0];
            rxBufObj->dataPtr  = (Char *)currBD->dataPtr;
            rxBufObj->length   = currBD->off_bLen & CPMAC_RX_BD_BUF_SIZE;
            rxBufObj->bufToken = currBD->bufToken;
            currPkt->pktToken = currPkt->bufList->bufToken;
            currPkt->numBufs = 1;
            currPkt->pktLength = (frameStatus & CPMAC_RX_BD_PKT_LENGTH_MASK);
	}

        /* Acknowledge RX interrupt for the channel */
        hDDC->regs->Rx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(currBD); 
        
#ifdef CPMAC_DDC_GETSTATS
        ++rxCppi->processedBD;  /* Maintain statistics of how many BD's were processed */
#endif

        lastBD = currBD;
        currBD = lastBD->next;
        rxCppi->activeQueueHead = currBD;

        /* Check if end of RX queue ? */
        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            if (currBD)     /* Misqueued packet */
            {
#ifdef CPMAC_DDC_GETSTATS
                ++rxCppi->misQueuedPackets;
#endif            
                hDDC->regs->Rx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(lastBD->hNext);
            }
            else            /* End of queue */
            {
#ifdef CPMAC_DDC_GETSTATS
                ++rxCppi->endOfQueue;
#endif             

                rxCppi->queueActive = False;    /* Clear Software RX queue */
            }
        }

	if(recycle_pkt)
	    goto Recycle_CpmacRxBDProc;
    
#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
        /* Only queue the packet here - and give it to DDA layer before returning */
        ++currPkt;
        ++rxCompleteCnt;
#else
        /* Return the packet to the user - BD ptr passed in last parameter for potential *future* use */
        hDDC->ddaIf->ddaNetIf.ddaNetrxCb(hDDC->ddcObj.hDDA, currPkt, (Ptr)channel, (Ptr) lastBD);
#endif

Recycle_CpmacRxBDProc:

        ++pktsProcessed;

        /* Recycle BD */
        cpmacAddBDToRxQueue(hDDC, rxCppi, lastBD, newBuffer, newBufToken);

        if (currBD)
        {
            PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
            frameStatus = currBD->mode;
        }
    } /* End of while receive packet processing loop */


    if ( (currBD) && ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) )
    {
        *morePkts = 1;
    }


#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
        /* Return the packet to the user - channel number passed in last parameter for potential *future* use */
        if (rxCompleteCnt > 0) {
            hDDC->ddaIf->ddaNetIf.ddaNetrxMultipleCb(hDDC->ddcObj.hDDA, &rxCppi->pktQueue[0], rxCompleteCnt, (Ptr) channel);

        }
#endif


    return (pktsProcessed); 
}

#else   

/***********************************************************************************************************
 * MULTI-FRAGMENT SUPPORT HERE
 **********************************************************************************************************/

/**
 * CPMAC DDC Send/Transmit function
 *  - Queues the packet provided by DDA into hardware queue
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 *
 * \note If "sendArgs" is True (non zero) CRC is calculated by DDA or upper layer and not by hardware
 *       and should be part of the packet data send to this function
 */
PAL_Result DDC_cpmacSend(CpmacDDCObj *hDDC, DDC_NetPktObj *pkt, Int  channel, Ptr sendArgs)
{
    PAL_Result      retVal = CPMAC_SUCCESS;
    CpmacTxBD       *currBD, *sopBD;
    CpmacTxCppiCh   *txCppi;
    DDC_NetBufObj   *bufList;
    Uint32          lockKey, cnt, mode;

    /* Verify proper device state */
    if (hDDC->ddcObj.state != DDC_OPENED)
        return (CPMAC_ERR_DEV_NOT_OPEN);
   
    /* Check ethernet link state. If not linked, return error */
    if (!hDDC->status.PhyLinked)
        return (CPMAC_ERR_TX_NO_LINK);
    
    /* Validate channel number and get channel control structure */    
    if (channel > CPMAC_MAX_TX_CHANNELS)
        return (CPMAC_ERR_TX_CH_INVALID);

    if (hDDC->txIsOpen[channel] != True)
        return (CPMAC_ERR_TX_CH_NOT_OPEN);

    txCppi = hDDC->txCppi[channel];
    bufList = pkt->bufList;     /* Get handle to the buffer array */

    /* Check packet size and if < CPMAC_MIN_ETHERNET_PKT_SIZE, pad it up */
    if (pkt->pktLength < CPMAC_MIN_ETHERNET_PKT_SIZE)
    {
      bufList[pkt->numBufs-1].length += (CPMAC_MIN_ETHERNET_PKT_SIZE - pkt->pktLength);
      pkt->pktLength = CPMAC_MIN_ETHERNET_PKT_SIZE;
    }

    /* \note: The critical section is required to protect the "txCppi" data structure which is being
     *        used by this function and by cpmacTxBDProc(). Since the function requires to protect
     *        the resources in many places in the function below, it would be wise to get and release
     *        the crit section only once rather than doing it in many places below. Interrupt protection
     *        is sought here so that this function or the cpmacTxBDProc() function can be invoked in any
     *        context and still will be safe to do so.
     */
    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Send Critical Section */

    currBD = txCppi->bdPoolHead;
    if (currBD == NULL)
    {
#ifdef CPMAC_DDC_GETSTATS
        txCppi->outOfTxBD++;
#endif           
        retVal = CPMAC_ERR_TX_OUT_OF_BD;
        goto Exit_DDC_cpmacSend;
    }
    sopBD = currBD;
    mode = (CPMAC_CPPI_SOP_BIT | CPMAC_CPPI_OWNERSHIP_BIT | pkt->pktLength);

    if ((Bool)sendArgs == True) mode |= CPMAC_CPPI_PASS_CRC_BIT;

    /* Multiple Tx BD for the packet to be sent */
    for (cnt=0; cnt < pkt->numBufs; cnt++)
    {
        /* Populate the BD contents to be added to the TX list */
        currBD->bufToken = bufList->bufToken;
        currBD->buffPtr  = PAL_CPMAC_VIRT_2_PHYS((Int32 *)bufList->dataPtr);
        currBD->off_bLen = bufList->length;
        currBD->mode     = mode;

	
        if (cnt == (pkt->numBufs -1))   /* last fragment */
        {
            txCppi->bdPoolHead = currBD->next;
            currBD->mode    |= CPMAC_CPPI_EOP_BIT; 
            currBD->hNext   = 0;
            currBD->next    = 0;
            sopBD->eopBD    = currBD;
            PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
        }
        else                            /* start or middle of packet */
        {
            mode = CPMAC_CPPI_OWNERSHIP_BIT;
            currBD->hNext    = PAL_CPMAC_VIRT_2_PHYS(currBD->next);
            PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
            currBD = currBD->next;
            if (currBD == NULL)
            {
#ifdef CPMAC_DDC_GETSTATS
                txCppi->outOfTxBD++;
#endif            

                retVal = CPMAC_ERR_TX_OUT_OF_BD;
                goto Exit_DDC_cpmacSend;
            }
        }
	bufList++;
    }

    /* Send the packet */
    if (txCppi->activeQueueHead == 0)
    {
        txCppi->activeQueueHead = sopBD;
        txCppi->activeQueueTail = currBD;
        if (txCppi->queueActive != True)
        {
            hDDC->regs->Tx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(sopBD);
            txCppi->queueActive = True;
        }
#ifdef CPMAC_DDC_GETSTATS
        ++txCppi->queueReinit;
#endif        

    }
    else
    {
        register volatile CpmacTxBD *tailBD;
        register Uint32 frameStatus;

        tailBD = txCppi->activeQueueTail;
        tailBD->next = sopBD;
        txCppi->activeQueueTail = currBD;

        tailBD = PAL_CPMAC_VIRT_NOCACHE(tailBD);
        tailBD->hNext = (Int32) PAL_CPMAC_VIRT_2_PHYS(sopBD);
        frameStatus = tailBD->mode;
        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            hDDC->regs->Tx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(sopBD);
            frameStatus &= ~(CPMAC_CPPI_EOQ_BIT);
            tailBD->mode = frameStatus;
#ifdef CPMAC_DDC_GETSTATS
            ++txCppi->endOfQueueAdd;
#endif             

        }
    }
#ifdef CPMAC_DDC_GETSTATS
     txCppi->activeQueueCount++;
#endif      

    
Exit_DDC_cpmacSend:
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */
    return (retVal);    
}

/**
 * CPMAC DDC TX Buffer Descriptor processing
 *  - processes transmit completed packets and returns the handles to DDA layer
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 *
 * \note returns number of pkts processed and 1 in morePkts if pkt completion processing pending
 */
Int cpmacTxBDProc(CpmacDDCObj *hDDC,  Uint32 channel, Uint32 *morePkts, Bool * isEOQ)
{
    CpmacTxBD      *currBD, *sopBD, *eopBD;
    CpmacTxCppiCh  *txCppi;
    Uint32          frameStatus;
    Uint32          pktsProcessed = 0;
    Uint32          lockKey;

#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    Uint32          txCompleteCnt = 0;
    Uint32          *txCompletePtr;
#endif
  
    *morePkts = 0;
    *isEOQ=True;

    /* Here no need to validate channel number, since it is taken from the interrupt register 
       Instead channel structure should be validated */
    if (hDDC->txIsOpen[channel] == False)
        return (CPMAC_ERR_TX_CH_NOT_OPEN);

    if (hDDC->txTeardownPending[channel] == True)
        return (CPMAC_SUCCESS);     /* Dont handle any packet completions */

    txCppi = hDDC->txCppi[channel];
#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    txCompletePtr = &txCppi->txComplete[0];
#endif

#ifdef CPMAC_DDC_GETSTATS
     ++txCppi->procCount;
#endif      

    PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Send Critical Section */

    /* Get first BD to process */
    currBD = txCppi->activeQueueHead;
    if (currBD == 0)
    {
        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */
        hDDC->regs->Tx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(txCppi->lastHwBDProcessed); 

#ifdef CPMAC_DDC_GETSTATS
        txCppi->noActivePkts++;
#endif           

        return(CPMAC_SUCCESS);
    }

    /* Invalidate BD */
    PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
    frameStatus = currBD->mode;

    while ( (currBD) &&
            ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) && 
            (pktsProcessed < txCppi->chInfo.serviceMax) )
    {
        sopBD = currBD;
        eopBD = currBD->eopBD;

        hDDC->regs->Tx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(eopBD); 
        txCppi->activeQueueHead = eopBD->next;

        if (eopBD != currBD)
        {
            PAL_CPMAC_CACHE_INVALIDATE(eopBD, CPMAC_BD_LENGTH_FOR_CACHE);
        }

        if (eopBD->mode & CPMAC_CPPI_EOQ_BIT)
        {
            if (eopBD->next)     /* Misqueued Packet */
            {
                hDDC->regs->Tx_HDP[channel] = eopBD->hNext;
#ifdef CPMAC_DDC_GETSTATS
                ++txCppi->misQueuedPackets;
#endif                 

            }
            else            /* End of queue */
            {
                txCppi->queueActive = False;
            }
        }
#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
        *txCompletePtr = (Uint32) sopBD->bufToken;
        ++txCompletePtr;
        ++txCompleteCnt;
#else 
        /* Single packet TX complete notify - This function is called in the Send Critical Section Context */
        hDDC->ddaIf->ddaNetIf.ddaNettxCompleteCb(hDDC->ddcObj.hDDA, &sopBD->bufToken, 1, (Ptr)channel);
#endif
        eopBD->next = txCppi->bdPoolHead; 
        txCppi->bdPoolHead = /*eopBD*/sopBD; 

#ifdef CPMAC_DDC_GETSTATS
         --txCppi->activeQueueCount;
#endif         
 
        pktsProcessed++;
    
        txCppi->lastHwBDProcessed = eopBD;
        currBD = txCppi->activeQueueHead;
        if (currBD)
        {
            PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
            frameStatus = currBD->mode;
        }
    } /* End of while loop */
    
    
    if ( (currBD) && ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) )
    {
        *morePkts = 1;
    }

    if (currBD) 
    {
        *isEOQ = False;
    }

#ifdef CPMAC_MULTIPACKET_TX_COMPLETE_NOTIFY
    /* Multiple packet TX complete notify - This function is NOT called in the Send Critical Section Context */
    hDDC->ddaIf->ddaNetIf.ddaNettxCompleteCb(hDDC->ddcObj.hDDA, &txCppi->txComplete[0], txCompleteCnt, (Ptr) channel);
#endif
    PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Send Critical Section */

    return (pktsProcessed);
}

/**
 * CPMAC DDC Add Buffer to RX queue function
 *  - returns the BD to the Receive queue
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 */
void cpmacAddBDToRxQueue(CpmacDDCObj *hDDC, CpmacRxCppiCh *rxCppi, CpmacRxBD *sopBD, CpmacRxBD *eopBD,
                    Uint32 *buffer, DDC_NetDataToken *bufToken, Uint32 numBD)
{
    CpmacRxBD   *currBD, *tailBD;
    Uint32      lockKey;
    Uint32      frameStatus;
    Uint32      cnt;

    /* Set the last BD in the list */
    eopBD->hNext = 0;
    eopBD->next = 0;    

    currBD = sopBD;
    for (cnt=0; cnt < numBD; cnt++)
    {
        /* Update the hardware descriptor */
        currBD->buffPtr = PAL_CPMAC_VIRT_2_PHYS(buffer[cnt]);
        currBD->off_bLen = rxCppi->chInfo.bufSize;
        currBD->mode = CPMAC_CPPI_OWNERSHIP_BIT;
        currBD->dataPtr = (Ptr) buffer[cnt];
        currBD->bufToken = (DDC_NetDataToken) bufToken[cnt];

        /* Write back  */
        PAL_CPMAC_CACHE_WRITEBACK_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
        currBD = currBD->next;
    }

    if (rxCppi->activeQueueHead == 0)
    {
        rxCppi->activeQueueHead = sopBD;
        rxCppi->activeQueueTail = eopBD;
        if (rxCppi->queueActive != False)
        {
            rxCppi->hDDC->regs->Rx_HDP[rxCppi->chInfo.chNum] = PAL_CPMAC_VIRT_2_PHYS(rxCppi->activeQueueHead);
            rxCppi->queueActive = True;
        }
    }
    else
    {
        PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &lockKey);  /* Start of Receive Critical Section */
        tailBD = rxCppi->activeQueueTail;
        rxCppi->activeQueueTail = eopBD;
        tailBD->next = (Ptr) sopBD;
        tailBD = (CpmacRxBD *) PAL_CPMAC_VIRT_NOCACHE(tailBD);
        tailBD->hNext = PAL_CPMAC_VIRT_2_PHYS(sopBD);
        frameStatus = tailBD->mode;
        if (frameStatus & CPMAC_CPPI_EOQ_BIT)
        {
            hDDC->regs->Rx_HDP[rxCppi->chInfo.chNum] = PAL_CPMAC_VIRT_2_PHYS(sopBD);
            frameStatus &= ~(CPMAC_CPPI_EOQ_BIT);
            tailBD->mode = frameStatus;
#ifdef CPMAC_DDC_GETSTATS
             ++rxCppi->endOfQueueAdd;
#endif 

        }
        PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT,lockKey);     /* End of Receive Critical Section */
    }

#ifdef CPMAC_DDC_GETSTATS
    rxCppi->recycledBD += numBD;  /* Maintain statistics of how many BD's were queued back - recycled */
#endif     

}


/**
 * CPMAC DDC RX Buffer Descriptor processing
 *  - processes received packets and passes them to DDA layer
 *  - requeues the buffer descriptor to the receive pool
 *  - If the queue is stalled due to sync issues, re-trigger the hardware
 */
Int CpmacRxBDProc(CpmacDDCObj *hDDC,  Uint32 channel, Int32 *morePkts)
{
    CpmacRxCppiCh   *rxCppi;
    CpmacRxBD       *currBD, *lastBD, *sopBD;
    Uint32          frameStatus;
    Uint32          pktsProcessed;
    Uint32          fragCount;
    Uint32          pktLen;
    DDC_NetPktObj   *currPkt;
    Uint32          cnt;
    Uint32          recycle_pkt;

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
    Uint32          rxCompleteCnt = 0;
#endif
    /* Here no need to validate channel number, since it is taken from the interrupt register 
       Instead channel structure should be validated */
    if (hDDC->rxIsOpen[channel] == False)
    {
        *morePkts = 0;
        return (CPMAC_ERR_RX_CH_NOT_OPEN);
    }

    /* Check if channel teardown pending */
    rxCppi = hDDC->rxCppi[channel];
    if (hDDC->rxTeardownPending[channel] == True)
    {
        *morePkts = 0;
        return (0);
    }

#ifdef CPMAC_DDC_GETSTATS
    ++rxCppi->procCount;
#endif


    *morePkts = 0;
    pktsProcessed = 0;

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
    currPkt = &rxCppi->pktQueue[0];
#else
    currPkt = &rxCppi->pktQueue;
#endif

    currBD = rxCppi->activeQueueHead;
    PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
    frameStatus = currBD->mode;
    while ( (currBD) &&
            ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) && 
            (pktsProcessed < rxCppi->chInfo.serviceMax) )
    {
        DDC_NetBufObj   *rxBufObj = currPkt->bufList;
	recycle_pkt               = 0;

        /* Find number of fragments in a packet and populate packet structures */
        sopBD = currBD;
        pktLen = (frameStatus & CPMAC_RX_BD_PKT_LENGTH_MASK);
	fragCount = 0;
        do
        {
            /* Populate received packet data structure */
            rxBufObj->dataPtr  = (Char *)currBD->dataPtr;
            rxBufObj->length   = currBD->off_bLen & CPMAC_RX_BD_BUF_SIZE;
            rxBufObj->bufToken = currBD->bufToken;

            lastBD = currBD;
            currBD = lastBD->next;
            if (currBD)
            {
                PAL_CPMAC_CACHE_INVALIDATE(currBD, CPMAC_BD_LENGTH_FOR_CACHE);
                frameStatus = currBD->mode;
            }
            ++fragCount;
            ++rxBufObj;
        } while ( (currBD) && ((lastBD->mode & CPMAC_CPPI_EOP_BIT) == 0) );
#ifdef CPMAC_DDC_GETSTATS
        /* Debug - count number of multifragment packets */
        if (fragCount > 1) 
        {
            ++rxCppi->numMultiFragPkts;
        }
#endif        


        /* complete packet contents */
        currPkt->pktToken = currPkt->bufList->bufToken;
        currPkt->numBufs = fragCount;
        currPkt->pktLength = pktLen;

        /* For each fragment allocate DDA buffers . If failure, free DDA buffers and return */
        for (cnt = 0; cnt < fragCount; cnt++)
        {
            /* Allocate new buffer */
            rxCppi->rxBufferPtr[cnt] = (Uint32)(hDDC->ddaIf->ddaNetIf.ddaNetAllocRxBufCb(hDDC->ddcObj.hDDA, rxCppi->chInfo.bufSize, (DDC_NetDataToken *)&rxCppi->rxDataToken[cnt], NULL));
	        /* 
	         * This was discussed at length. Not sure why this did not make it here earlier. Nevertheless, 
	         * the code is being now implemented. Since, we can not get new buffer, we are recycling the 
	         * all the buffers. (After freeing up the newly allocated ones).
             * As discussed many times, this is a simple solution. 
	         */ 
            if (rxCppi->rxBufferPtr[cnt] == (Uint32)NULL)
            {
		        CpmacRxBD *rewindBD = sopBD;

                { /* Return the successfully allocated buffers as we will process this packet later */
                    int cnt2;
                    for (cnt2=0; cnt2 < fragCount; cnt2++)
		            {
			            if(cnt2 < cnt)
                            hDDC->ddaIf->ddaNetIf.ddaNetFreeRxBufCb(hDDC->ddcObj.hDDA, (Ptr)rxCppi->rxBufferPtr[cnt2], 
                                                                            (DDC_NetDataToken) rxCppi->rxDataToken[cnt2], NULL);
            			rxCppi->rxBufferPtr[cnt2] = rewindBD->dataPtr;
			            rxCppi->rxDataToken[cnt2] = rewindBD->bufToken;
			            rewindBD                  = rewindBD->next;
		            }
                }

                /* No buffer available. Return error with packets pending */
#ifdef CPMAC_DDC_GETSTATS
                ++rxCppi->outOfRxBuffers;
#endif               
                recycle_pkt = 1;
            }
        }

        /* Now we have buffers for all fragments - so we proceed */
#ifdef CPMAC_DDC_GETSTATS
         rxCppi->processedBD += fragCount;  /* Maintain statistics of how many BD's were processed */
#endif  

        rxCppi->activeQueueHead = currBD;

        /* acknowledge hardware interrupt for the given packet BD */
        hDDC->regs->Rx_CP[channel] = PAL_CPMAC_VIRT_2_PHYS(lastBD); 

        /* Check if end of RX queue ? */
        if (lastBD->mode & CPMAC_CPPI_EOQ_BIT)
        {
            if (currBD)     /* Misqueued packet */
            {
#ifdef CPMAC_DDC_GETSTATS
                ++rxCppi->misQueuedPackets;
#endif             

                hDDC->regs->Rx_HDP[channel] = PAL_CPMAC_VIRT_2_PHYS(lastBD->hNext);
            }
            else            /* End of queue */
            {
#ifdef CPMAC_DDC_GETSTATS
                ++rxCppi->endOfQueue;
#endif             
                rxCppi->queueActive = False;    /* Clear Software RX queue */
            }
        }

	if(recycle_pkt)
	    goto Recycle_CpmacRxBDProc;

        /* Callback netrx  - give packet to DDA OR queue it in multipacket queue */
#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
        /* Only queue the packet here - and give it to DDA layer before returning */
        ++currPkt;
        ++rxCompleteCnt;
#else
        /* Return the packet to the user - BD ptr passed in last parameter for potential *future* use */
        hDDC->ddaIf->ddaNetIf.ddaNetrxCb(hDDC->ddcObj.hDDA, &rxCppi->pktQueue, (Ptr)channel, (Ptr) sopBD);
#endif

Recycle_CpmacRxBDProc:

        ++pktsProcessed;

        /* Requeue BD */
        cpmacAddBDToRxQueue(hDDC, rxCppi, sopBD, lastBD, &rxCppi->rxBufferPtr[0], (DDC_NetDataToken *)&rxCppi->rxDataToken[0], fragCount);
    } /* End of while receive packet processing loop */

    if ( (currBD) && ((frameStatus & CPMAC_CPPI_OWNERSHIP_BIT) == 0) )
    {
        *morePkts = 1;
    }

#ifdef CPMAC_MULTIPACKET_RX_COMPLETE_NOTIFY
        /* Return the packet to the user - channel number passed in last parameter for potential *future* use */
        if (rxCompleteCnt > 0)
            hDDC->ddaIf->ddaNetIf.ddaNetrxMultipleCb(hDDC->ddcObj.hDDA, &rxCppi->pktQueue[0], rxCompleteCnt, (Ptr) channel);
#endif
    return (pktsProcessed); 
}


#endif  /* CPMAC_MULTIFRAGMENT */
