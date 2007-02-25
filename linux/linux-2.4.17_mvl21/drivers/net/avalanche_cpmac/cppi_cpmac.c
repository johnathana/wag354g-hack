/*************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002,2003 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE: cppi.c
 *
 *  DESCRIPTION:
 *      This file contains shared code for all CPPI modules.
 *
 *  HISTORY:
 *   7Aug02 Greg RC1.00  Original Version created.
 *  27Sep02 Mick RC1.01  Merged for use by CPMAC/CPSAR
 *  16Oct02 Mick RC1.02  Performance Tweaks (see cppihist.txt)
 *  12Nov02 Mick RC1.02  Updated to use cpmac_reg.h
 *  09Jan03 Mick RC3.01  Removed modification to RxBuffer ptr
 *  28Mar03 Mick 1.03    RxReturn now returns error if Malloc Fails
 *  10Apr03 Mick 1.03.02 Added Needs Buffer Support
 *  11Jun03 Mick 1.06.02 halSend() errors corrected
 *  23Aug04 Mick 1.07.08 cpmac only - Send: bypass threshold check if TxInts re-enabled
 *
 *  @author  Greg Guyotte
 *  @version 1.00
 *  @date    7-Aug-2002
 *****************************************************************************/
/* each CPPI module must modify this file, the rest of the
                     code in cppi.c should be totally shared *//* Each CPPI module MUST properly define all constants shown below */

/* CPPI registers */

/* the following defines are not CPPI specific */

static int TxInt(HAL_DEVICE *HalDev, int Ch, int Queue, int *MoreWork);                     

static void FreeRx(HAL_DEVICE *HalDev, int Ch)
  {
   HAL_RCB *rcb_ptr;                                                /*+GSG 030303*/
   int rcbSize = (sizeof(HAL_RCB)+0xf)&~0xf;                        /*+GSG 030303*/
   int Num = HalDev->ChData[Ch].RxNumBuffers, i;                    /*+GSG 030303*/

   /* Free Rx data buffers attached to descriptors, if necessary */
   if (HalDev->RcbStart[Ch] != 0)                                   /*+GSG 030303*/
     {                                                              /*+GSG 030303*/
      for(i=0;i<Num;i++)                                            /*+GSG 030303*/
        {                                                           /*+GSG 030303*/
         rcb_ptr = (HAL_RCB *)(HalDev->RcbStart[Ch] + (i*rcbSize)); /*+GSG 030303*/

         /* free the data buffer */
         if (rcb_ptr->DatPtr != 0)
       {

        HalDev->OsFunc->FreeRxBuffer((void *)rcb_ptr->OsInfo, (void *)rcb_ptr->DatPtr);
        rcb_ptr->OsInfo=0;                                        /*MJH+030522*/
        rcb_ptr->DatPtr=0;                                        /*MJH+030522*/
       }
    }                                                           /*+GSG 030303*/
     }                                                              /*+GSG 030303*/

   /* free up all desciptors at once */
   HalDev->OsFunc->FreeDmaXfer(HalDev->RcbStart[Ch]);

   /* mark buffers as freed */
   HalDev->RcbStart[Ch] = 0;
  }

static void FreeTx(HAL_DEVICE *HalDev, int Ch, int Queue)
  {

/*+GSG 030303*/

   /* free all descriptors at once */
   HalDev->OsFunc->FreeDmaXfer(HalDev->TcbStart[Ch][Queue]);

   HalDev->TcbStart[Ch][Queue] = 0;
  }

/* return of 0 means that this code executed, -1 means the interrupt was not
   a teardown interrupt */
static int RxTeardownInt(HAL_DEVICE *HalDev, int Ch)
  {
   bit32u base = HalDev->dev_base;

   /* check to see if the interrupt is a teardown interrupt */
   if (((CPMAC_RX_INT_ACK( base , Ch ))  & TEARDOWN_VAL) == TEARDOWN_VAL)
     {
      /* finish channel teardown */

      /* Free channel resources on a FULL teardown */
      if (HalDev->RxTeardownPending[Ch] & FULL_TEARDOWN)
        {
         FreeRx(HalDev, Ch);
        }

      /* bug fix - clear Rx channel pointers on teardown */
      HalDev->RcbPool[Ch] = 0;
      HalDev->RxActQueueHead[Ch]  = 0;
      HalDev->RxActQueueCount[Ch] = 0;
      HalDev->RxActive[Ch] = FALSE;

      /* write completion pointer */
      (CPMAC_RX_INT_ACK( base , Ch ))  = TEARDOWN_VAL;

      /* use direction bit as a teardown pending bit! May be able to
         use only one teardown pending integer in HalDev */

      HalDev->RxTeardownPending[Ch] &= ~RX_TEARDOWN;

      HalDev->ChIsOpen[Ch][DIRECTION_RX] = 0;

      HalDev->ChIsOpen[Ch][DIRECTION_RX] = 0;
      CPMAC_RX_INTMASK_CLEAR(HalDev->dev_base) = (1<<Ch);
      if ((HalDev->RxTeardownPending[Ch] & BLOCKING_TEARDOWN) == 0)
    {

     HalDev->OsFunc->TeardownComplete(HalDev->OsDev, Ch, DIRECTION_RX);
    }
      HalDev->RxTeardownPending[Ch] = 0;

      return (EC_NO_ERRORS);
     }
   return (-1);
  }

/* return of 0 means that this code executed, -1 means the interrupt was not
   a teardown interrupt.  Note: this code is always called with Queue == 0 (hi priority). */
static int TxTeardownInt(HAL_DEVICE *HalDev, int Ch, int Queue)
  {
   bit32u base = HalDev->dev_base;
   HAL_TCB *Last, *Curr, *First;                                    /*+GSG 030303*/
   int i;

   if (((CPMAC_TX_INT_ACK( base , Ch ))  & TEARDOWN_VAL) == TEARDOWN_VAL)
     {
      /* perform all actions for both queues (+GSG 040212) */
      for (i=0; i<HalDev->ChData[Ch].TxNumQueues; i++)
        {
         /* return outstanding buffers to OS                             +RC3.02*/
         Curr = HalDev->TxActQueueHead[Ch][i];                     /*+GSG 030303*/
         First = Curr;                                                 /*+GSG 030303*/
         while (Curr)                                                  /*+GSG 030303*/
           {                                                           /*+GSG 030303*/
            /* Pop TCB(s) for packet from the stack */                 /*+GSG 030303*/
            Last = Curr->Eop;                                          /*+GSG 030303*/
            HalDev->TxActQueueHead[Ch][i] = Last->Next;            /*+GSG 030303*/
                                                                    /*+GSG 030303*/
            /* return to OS */                                         /*+GSG 030303*/
            HalDev->OsFunc->SendComplete(Curr->OsInfo);                /*+GSG 030303*/
                                                                    /*+GSG 030303*/
            /* Push Tcb(s) back onto the stack */                      /*+GSG 030303*/
            Curr = Last->Next;                                         /*+GSG 030303*/
            Last->Next = HalDev->TcbPool[Ch][i];                   /*+GSG 030303*/
            HalDev->TcbPool[Ch][i] = First;                        /*+GSG 030303*/
                                                                    /*+GSG 030303*/
            /* set the first(SOP) pointer for the next packet */       /*+GSG 030303*/
            First = Curr;                                              /*+GSG 030303*/
           }                                                           /*+GSG 030303*/
        }

      /* finish channel teardown */

      if (HalDev->TxTeardownPending[Ch] & FULL_TEARDOWN)
        {
         FreeTx(HalDev, Ch, 0);

         if (HalDev->ChData[Ch].TxNumQueues == 2)
           FreeTx(HalDev, Ch, 1);
        } /* if FULL teardown */

      /* perform all actions for both queues (+GSG 040212) */
      for (i=0; i<HalDev->ChData[Ch].TxNumQueues; i++)
        {
         /* bug fix - clear Tx channel pointers on teardown */
         HalDev->TcbPool[Ch][i] = 0;
         HalDev->TxActQueueHead[Ch][i]  = 0;
         HalDev->TxActQueueCount[Ch][i] = 0;
         HalDev->TxActive[Ch][i]        = FALSE;
        }

      /* write completion pointer, only needed for the high priority queue */
      (CPMAC_TX_INT_ACK( base , Ch ))  = TEARDOWN_VAL;

      /* no longer pending teardown */
      HalDev->TxTeardownPending[Ch] &= ~TX_TEARDOWN;

      HalDev->ChIsOpen[Ch][DIRECTION_TX] = 0;

      HalDev->ChIsOpen[Ch][DIRECTION_TX] = 0;
      CPMAC_TX_INTMASK_CLEAR(HalDev->dev_base) = (1<<Ch);
      if ((HalDev->TxTeardownPending[Ch] & BLOCKING_TEARDOWN) == 0)
        {

         HalDev->OsFunc->TeardownComplete(HalDev->OsDev, Ch, DIRECTION_TX);
        }
      HalDev->TxTeardownPending[Ch] = 0;

      return (EC_NO_ERRORS);
     }
   return (-1);
  }

/* +GSG 030421 */
static void AddToRxQueue(HAL_DEVICE *HalDev, HAL_RCB *FirstRcb, HAL_RCB *LastRcb, int FragCount, int Ch)
  {
   if (HalDev->RxActQueueHead[Ch]==0)
     {

      HalDev->RxActQueueHead[Ch]=FirstRcb;
      HalDev->RxActQueueTail[Ch]=LastRcb;
      if (!HalDev->RxActive[Ch])
        {
         /* write Rx Queue Head Descriptor Pointer */
         ((CPMAC_RX_HDP(  HalDev->dev_base  ,  Ch  )) )  = VirtToPhys(FirstRcb) - HalDev->offset;
         HalDev->RxActive[Ch]=TRUE;
        }
     }
    else
     {
      register HAL_RCB *OldTailRcb;
      register bit32u rmode;
      
      HalDev->OsFunc->CriticalOn();
      OldTailRcb=HalDev->RxActQueueTail[Ch];
      OldTailRcb->Next=(void *)FirstRcb;
      OldTailRcb=VirtToVirtNoCache(OldTailRcb);
      OldTailRcb->HNext=VirtToPhys(FirstRcb) - HalDev->offset;
      HalDev->RxActQueueTail[Ch]=LastRcb;
      rmode=OldTailRcb->mode;
      if (rmode&CB_EOQ_BIT)
        {
        rmode&=~CB_EOQ_BIT;
        ((CPMAC_RX_HDP(  HalDev->dev_base  ,  Ch  )) )  = VirtToPhys(FirstRcb) - HalDev->offset;
        OldTailRcb->mode=rmode;
        }   
      HalDev->OsFunc->CriticalOff();
     }
  }

/**
 *  @ingroup CPHAL_Functions
 *  This function is called to indicate to the CPHAL that the upper layer
 *  software has finished processing the receive data (given to it by
 *  osReceive()).  The CPHAL will then return the appropriate receive buffers
 *  and buffer descriptors to the available pool.
 *
 *  @param   HalReceiveInfo   Start of receive buffer descriptor chain returned to
 *                      CPHAL.
 *  @param   StripFlag  Flag indicating whether the upper layer software has
 *                      retained ownership of the receive data buffers.
 *<BR>
 * 'FALSE' means  that the CPHAL can reuse the receive data buffers.
 *<BR>
 * 'TRUE' : indicates the data buffers were retained by the OS
 *<BR>
 * NOTE:  If StripFlag is TRUE, it is the responsibility of the upper layer software to free the buffers when they are no longer needed.
 *
 *  @return EC_NO_ERRORS (ok). <BR>
 *          Possible Error Codes:<BR>
 *          @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *          @ref EC_VAL_RCB_NEEDS_BUFFER "EC_VAL_RCB_NEEDS_BUFFER"<BR>
 *          @ref EC_VAL_RCB_DROPPED "EC_VAL_RCB_DROPPED"<BR>
 */
static int halRxReturn(HAL_RECEIVEINFO *HalReceiveInfo,
            int StripFlag)
  {
   int Ch, i;
   HAL_RCB *LastRcb;
   HAL_DEVICE *HalDev;
   int RcbSize;
   int FragCount;

   Ch = HalReceiveInfo->mode&0x0ff;
   HalDev = (HAL_DEVICE *)HalReceiveInfo->Off_BLen;
   FragCount = HalReceiveInfo->mode>>8;
      
   if (HalDev->State != enOpened)
     return(EC_CPMAC |EC_FUNC_RXRETURN|EC_VAL_INVALID_STATE);

   LastRcb=(HAL_RCB *)HalReceiveInfo->Eop;
   LastRcb->HNext=0;
   LastRcb->Next=0;
   RcbSize = HalDev->ChData[Ch].RxBufSize;

   if (FragCount>1)
     {
      LastRcb->Off_BLen=RcbSize;
      LastRcb->mode=CB_OWNERSHIP_BIT;
     }

   HalReceiveInfo->Off_BLen=RcbSize;
   HalReceiveInfo->mode=CB_OWNERSHIP_BIT;

   /* If OS has kept the buffers for this packet, attempt to alloc new buffers */
   if (StripFlag)
     {
      int rc=0;                                                      /*MJH+030417*/
      int GoodCount=0;                                               /*GSG+030421*/       
      HAL_RCB *TempRcb;
      char *pBuf;
      HAL_RCB *CurrHeadRcb = HalReceiveInfo, *LastGoodRcb=0;         /* +GSG 030421 */

      TempRcb = HalReceiveInfo;
      for (i=0; i<FragCount; i++)
        {
         if (TempRcb == 0)
           {
            dbgPrintf("Rx Return error while allocating new buffers\n");
            dbgPrintf("Rcb = %08x, Rcb->Eop = %08x, FragCount = %d:%d\n",
              (bit32u)HalReceiveInfo, (bit32u)HalReceiveInfo->Eop, FragCount,i);
            osfuncSioFlush();

            return(EC_CPPI|EC_FUNC_RXRETURN|EC_VAL_CORRUPT_RCB_CHAIN);
           }

         pBuf= (char *) HalDev->OsFunc->MallocRxBuffer(RcbSize,0,
                             0xF,HalDev->ChData[Ch].OsSetup,
                             (void *)TempRcb,
                             (void *)&TempRcb->OsInfo,
                             (void *) HalDev->OsDev);
         if (!pBuf)
           {
            /* malloc failed, add this RCB to Needs Buffer List */
            (HAL_RCB *)TempRcb->Eop = TempRcb;                                  /* GSG +030430 */
            TempRcb->mode=1<<8|Ch;
            TempRcb->Off_BLen=(bit32u)HalDev;
            
            if(HalDev->NeedsCount < MAX_NEEDS)                                  /* +MJH 030410 */
              {                                                                 /* +MJH 030410 */
               HalDev->Needs[HalDev->NeedsCount] = (HAL_RECEIVEINFO *) TempRcb; /* +MJH 030410 */
               HalDev->NeedsCount++;                                            /* +MJH 030410 */
               rc = (EC_CPPI|EC_FUNC_RXRETURN|EC_VAL_RCB_NEEDS_BUFFER);         /* ~MJH 030417 */
              }                                                                 /* +MJH 030410 */
             else                                                               /* +MJH 030410 */
               rc = (EC_CPPI|EC_FUNC_RXRETURN|EC_VAL_RCB_DROPPED);              /* ~MJH 030417 */

            /* requeue any previous RCB's that were ready to go before this one */
            if (GoodCount > 0)                                                  /* +GSG 030421 */
              {                                                                 /* +GSG 030421 */
               LastGoodRcb->HNext=0;                                            /* +GSG 030430 */
               LastGoodRcb->Next=0;                                             /* +GSG 030430 */
               osfuncDataCacheHitWritebackAndInvalidate((void *)LastGoodRcb, 16);            /* +GSG 030430 */

               AddToRxQueue(HalDev, CurrHeadRcb, LastGoodRcb, GoodCount, Ch);   /* +GSG 030421 */
               GoodCount = 0;                                                   /* +GSG 030421 */
              }                                                                 /* +GSG 030421 */

            CurrHeadRcb = TempRcb->Next;                                        /* +GSG 030421 */
           }
          else                                                                  /* +GSG 030421 */
           {                                                                    /* +GSG 030421 */
            /* malloc succeeded, requeue the RCB to the hardware */
            TempRcb->BufPtr=VirtToPhys(pBuf) - HalDev->offset;
            TempRcb->DatPtr=pBuf;
            /* Emerald fix 10/29 */
            osfuncDataCacheHitWritebackAndInvalidate((void *)TempRcb, 16);

            /* i store the last good RCB in case the malloc fails for the
               next fragment.  This ensures that I can go ahead and return
               a partial chain of RCB's to the hardware */
            LastGoodRcb = TempRcb;                                              /* +GSG 030421 */
            GoodCount++;                                                        /* +GSG 030421 */
           }                                                                    /* +GSG 030421 */
         TempRcb = TempRcb->Next;
        } /*  end of Frag loop */
      /* if there any good RCB's to requeue, do so here */
      if (GoodCount > 0)                                                        /* +GSG 030421 */
        {
         AddToRxQueue(HalDev, CurrHeadRcb, LastGoodRcb, GoodCount, Ch);          /* +GSG 030421 */
        }
      return(rc);                                                               /* ~GSG 030421 */
     }
   else
     {
      /*  Not Stripping */
      /* Emerald */
      /* Write Back SOP and last RCB */
      osfuncDataCacheHitWritebackAndInvalidate((void *)HalReceiveInfo, 16);

      if (FragCount > 1)
        {
         osfuncDataCacheHitWritebackAndInvalidate((void *)LastRcb, 16);
        }
      /* if not stripping buffers, always add to queue */
      AddToRxQueue(HalDev, HalReceiveInfo, LastRcb, FragCount, Ch); /*MJH~030520*/
     }

   return(EC_NO_ERRORS);
  }

/* +MJH 030410
   Trys to liberate an RCB until liberation fails.
   Note: If liberation fails then RxReturn will re-add the RCB to the
         Needs list.
*/
static void NeedsCheck(HAL_DEVICE *HalDev)
{
    HAL_RECEIVEINFO* HalRcb;
    int rc;
    HalDev->OsFunc->CriticalOn();
    while(HalDev->NeedsCount)
      {
      HalDev->NeedsCount--;
      HalRcb = HalDev->Needs[HalDev->NeedsCount];
      rc =  halRxReturn(HalRcb, 1);
      /* short circuit if RxReturn starts to fail */
      if (rc != 0)
        break;
      }
    HalDev->OsFunc->CriticalOff();
}

/*
 *  This function allocates transmit buffer descriptors (internal CPHAL function).
 *  It creates a high priority transmit queue by default for a single Tx
 *  channel.  If QoS is enabled for the given CPHAL device, this function
 *  will also allocate a low priority transmit queue.
 *
 *  @param   HalDev   CPHAL module instance. (set by cphalInitModule())
 *  @param   Ch       Channel number.
 *
 *  @return  0 OK, Non-Zero Not OK
 */
static int InitTcb(HAL_DEVICE *HalDev, int Ch)
  {
   int i, Num = HalDev->ChData[Ch].TxNumBuffers;
   HAL_TCB *pTcb=0;
   char *AllTcb;
   int  tcbSize, Queue;
   int SizeMalloc;

   tcbSize = (sizeof(HAL_TCB)+0xf)&~0xf;
   SizeMalloc = (tcbSize*Num)+0xf;

   for (Queue=0; Queue < HalDev->ChData[Ch].TxNumQueues; Queue++)
     {
      if (HalDev->TcbStart[Ch][Queue] == 0)
        {

         /* malloc all TCBs at once */
         AllTcb = (char *)HalDev->OsFunc->MallocDmaXfer(SizeMalloc,0,0xffffffff);
         if (!AllTcb)
           {
            return(EC_CPPI|EC_FUNC_HAL_INIT|EC_VAL_TCB_MALLOC_FAILED);
           }

         HalDev->OsFunc->Memset(AllTcb, 0, SizeMalloc);

         /* keep this address for freeing later */
         HalDev->TcbStart[Ch][Queue] = AllTcb;
        }
       else
        {
         /* if the memory has already been allocated, simply reuse it! */
         AllTcb = HalDev->TcbStart[Ch][Queue];
        }

      /* align to cache line */
      AllTcb = (char *)(((bit32u)AllTcb + 0xf) &~ 0xf); /*PITS #143  MJH~030522*/

      /* default High priority transmit queue */
      HalDev->TcbPool[Ch][Queue]=0;
      for(i=0;i<Num;i++)
        {
         /*pTcb=(HAL_TCB *) OsFunc->MallocDmaXfer(sizeof(HAL_TCB),0,0xffffffff); */
         pTcb= (HAL_TCB *)(AllTcb + (i*tcbSize));
         pTcb->mode=0;
         pTcb->BufPtr=0;
         pTcb->Next=HalDev->TcbPool[Ch][Queue];
         pTcb->Off_BLen=0;
         HalDev->TcbPool[Ch][Queue]=pTcb;
        }
      /*HalDev->TcbEnd = pTcb;*/
     }

   return(EC_NO_ERRORS);
  }

/*
 *  This function allocates receive buffer descriptors (internal CPHAL function).
 *  After allocation, the function 'queues' (gives to the hardware) the newly
 *  created receive buffers to enable packet reception.
 *
 *  @param   HalDev   CPHAL module instance. (set by cphalInitModule())
 *  @param   Ch    Channel number.
 *
 *  @return  0 OK, Non-Zero Not OK
 */
static int InitRcb(HAL_DEVICE *HalDev, int Ch)
  {
   int i, Num = HalDev->ChData[Ch].RxNumBuffers;
   int Size = HalDev->ChData[Ch].RxBufSize;
   HAL_RCB *pRcb;
   char *pBuf;
   char *AllRcb;
   int  rcbSize;
   int  DoMalloc = 0;
   int SizeMalloc;
   int MallocSize;

   rcbSize = (sizeof(HAL_RCB)+0xf)&~0xf;
   SizeMalloc = (rcbSize*Num)+0xf;

   if (HalDev->RcbStart[Ch] == 0)
     {
      DoMalloc = 1;

      /* malloc all RCBs at once */
      AllRcb= (char *)HalDev->OsFunc->MallocDmaXfer(SizeMalloc,0,0xffffffff);
      if (!AllRcb)
        {
         return(EC_CPPI|EC_FUNC_HAL_INIT|EC_VAL_RCB_MALLOC_FAILED);
        }

      HalDev->OsFunc->Memset(AllRcb, 0, SizeMalloc);

      /* keep this address for freeing later */
      HalDev->RcbStart[Ch] = AllRcb;
     }
    else
     {
      /* if the memory has already been allocated, simply reuse it! */
      AllRcb = HalDev->RcbStart[Ch];
     }

   /* align to cache line */
   AllRcb = (char *)(((bit32u)AllRcb + 0xf)&~0xf);  /*PITS #143  MJH~030522*/

   HalDev->RcbPool[Ch]=0;
   for(i=0;i<Num;i++)
     {
      pRcb = (HAL_RCB *)(AllRcb + (i*rcbSize));

      if (DoMalloc == 1)
        {

         MallocSize = Size;                                       /*~3.01 */
         pBuf= (char *) HalDev->OsFunc->MallocRxBuffer(MallocSize,0,0xF,HalDev->ChData[Ch].OsSetup, (void *)pRcb, (void *)&pRcb->OsInfo, (void *) HalDev->OsDev);
         if(!pBuf)
             {
            return(EC_CPPI|EC_FUNC_HAL_INIT|EC_VAL_RX_BUFFER_MALLOC_FAILED);
             }
           /* -RC3.01 pBuf = (char *)(((bit32u)pBuf+0xF) & ~0xF); */
         pRcb->BufPtr=VirtToPhys(pBuf) - HalDev->offset;
         pRcb->DatPtr=pBuf;
        }
      pRcb->mode=(1<<8)|Ch; /* One Frag for Ch */
      pRcb->Next=(void *)HalDev->RcbPool[Ch];
      pRcb->Off_BLen=(bit32u)HalDev;
      HalDev->RcbPool[Ch]=pRcb;
     }

   /* Give all of the Rx buffers to hardware */

   while(HalDev->RcbPool[Ch])
     {
      pRcb=HalDev->RcbPool[Ch];
      HalDev->RcbPool[Ch]=pRcb->Next;
      pRcb->Eop=(void*)pRcb;
      pRcb->mode=(1<<8)|Ch;
      halRxReturn((HAL_RECEIVEINFO *)pRcb, 0);
     }

   return(EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  This function transmits the data in FragList using available transmit
 *  buffer descriptors.  More information on the use of the Mode parameter
 *  is available in the module-specific appendices.  Note:  The OS should
 *  not call Send() for a channel that has been requested to be torndown.
 *
 *  @param   HalDev      CPHAL module instance. (set by cphalInitModule())
 *  @param   FragList    Fragment List structure.
 *  @param   FragCount   Number of fragments in FragList.
 *  @param   PacketSize  Number of bytes to transmit.
 *  @param   OsSendInfo  OS Send Information structure. <BR>
 *  @param   Mode        32-bit value with the following bit fields: <BR>
 *                       31-16: Mode  (used for module specific data). <BR>
 *                       15-08: Queue (transmit queue to send on). <BR>
 *                       07-00: Channel (channel number to send on).
 *
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_NOT_LINKED "EC_VAL_NOT_LINKED"<BR>
 *           @ref EC_VAL_INVALID_CH "EC_VAL_INVALID_CH"<BR>
 *           @ref EC_VAL_OUT_OF_TCBS "EC_VAL_OUT_OF_TCBS"<BR>
 *           @ref EC_VAL_NO_TCBS "EC_VAL_NO_TCBS"<BR>
 */
static int halSend(HAL_DEVICE *HalDev,FRAGLIST *FragList,
                      int FragCount,int PacketSize, OS_SENDINFO *OsSendInfo,
                      bit32u Mode)
  {
  HAL_TCB *tcb_ptr, *head;
  int i;
  int rc = EC_NO_ERRORS;
  int Ch = Mode & 0xFF;
  int Queue = (Mode>>8)&0xFF;
  /*int DoThresholdCheck=1;  */ /* Used when TxIntDisable is set and TxInts are re-enabled */
  
  if (HalDev->State != enOpened)
     return(EC_CPPI|EC_FUNC_SEND|EC_VAL_INVALID_STATE);

  if (!HalDev->Linked)
    {
     rc = EC_CPPI|EC_FUNC_SEND|EC_VAL_NOT_LINKED;
     return(rc);
    }

  if (HalDev->ChIsOpen[Ch][DIRECTION_TX] == 0)                    /*MJH~030611*/ /*PITS 148*/
    return(EC_CPMAC |EC_FUNC_SEND|EC_VAL_INVALID_CH);              /*+GSG 030303*/

  HalDev->OsFunc->CriticalOn();

  /* Setup Tx mode and size */
  if (PacketSize<60)
    {
     FragList[FragCount-1].len += (60 - PacketSize);              /*MJH~030506*//*PITS 132*/
     PacketSize = 60;                                             /*MJH~030506*/
    }
  Mode &= CB_PASSCRC_BIT;

  tcb_ptr = head = HalDev->TcbPool[Ch][Queue];

  if (tcb_ptr)
    {

     Mode|=PacketSize|CB_SOF_BIT|CB_OWNERSHIP_BIT;

     for (i=0; i<FragCount; i++)

       {
        /* Setup Tx mode and size */
        tcb_ptr->Off_BLen = FragList[i].len;
        
        tcb_ptr->mode     = Mode;
        tcb_ptr->BufPtr   = VirtToPhys((bit32 *)FragList[i].data) - HalDev->offset;
        tcb_ptr->OsInfo = OsSendInfo;

        if (i == (FragCount - 1))
          {
           /* last fragment */
           tcb_ptr->mode |= CB_EOF_BIT;

           /* since this is the last fragment, set the TcbPool pointer before
              nulling out the Next pointers */

           HalDev->TcbPool[Ch][Queue] = tcb_ptr->Next;

           tcb_ptr->Next = 0;
           tcb_ptr->HNext = 0;

           /* In the Tx Interrupt handler, we will need to know which TCB is EOP,
              so we can save that information in the SOP */
           head->Eop = tcb_ptr;

           /* Emerald fix 10/29 */
       osfuncDataCacheHitWritebackAndInvalidate((void *)tcb_ptr, 16);

          }
         else
          {
           Mode=CB_OWNERSHIP_BIT;
           tcb_ptr->HNext    = VirtToPhys((bit32 *)tcb_ptr->Next) - HalDev->offset;
        
           /* Emerald fix 10/29 */
           osfuncDataCacheHitWritebackAndInvalidate((void *)tcb_ptr, 16);

           tcb_ptr = tcb_ptr->Next;  /* what about the end of TCB list?? */

           if (tcb_ptr == 0)
             {
              rc = EC_CPPI|EC_FUNC_SEND|EC_VAL_OUT_OF_TCBS;
              goto ExitSend;
             }
          }
       } /* for */

     /* put it on the high priority queue */
     if (HalDev->TxActQueueHead[Ch][Queue] == 0)
       {
        HalDev->TxActQueueHead[Ch][Queue]=head;
        HalDev->TxActQueueTail[Ch][Queue]=tcb_ptr;
/*+GSG 030303*//*+GSG 030303*/
        if (!HalDev->TxActive[Ch][Queue])
          {

              bit32u base = HalDev->dev_base;
              
              /* write CPPI TX HDP */
              (CPMAC_TX_HDP( base , Ch ))  = VirtToPhys(head)  - HalDev->offset;
              HalDev->TxActive[Ch][Queue]=TRUE;

          }
       }
      else
       {
	    register volatile HAL_TCB *pTailTcb;   
        register bit32u tmode;
        register bit32u pCurrentTcb;
        
        HalDev->TxActQueueTail[Ch][Queue]->Next=head;
        /* Emerald fix 10/29 */
        
        pTailTcb=(HAL_TCB *)VirtToVirtNoCache(&HalDev->TxActQueueTail[Ch][Queue]->HNext);
        pCurrentTcb=VirtToPhys(head)  - HalDev->offset;
        pTailTcb->HNext=pCurrentTcb;
        HalDev->TxActQueueTail[Ch][Queue]=tcb_ptr;
/*+GSG 030303*/
        tmode=pTailTcb->mode;  
        if (tmode&CB_EOQ_BIT)
          {
           bit32u base = HalDev->dev_base;

           tmode&=~CB_EOQ_BIT;
           pTailTcb->mode=tmode;
           ((CPMAC_TX_HDP( base , Ch )) )  = pCurrentTcb;
          }

        else
          {
           if(HalDev->TxIntDisable)
             {
              /*  Enable Interrupts, to ensure packet goes out on wire */
              CPMAC_TX_INTMASK_SET(HalDev->dev_base) = (1<<Ch); 
              halPacketProcessEnd(HalDev); /* Allow Interrupt to be seen at the OS */
              /*DoThresholdCheck = 0; */       /* Disable Threshold Check */

             }       
          }

       }
     rc = EC_NO_ERRORS;
     goto ExitSend;
    } /* if (tcb_ptr) */
   else
    {
     rc = EC_CPPI|EC_FUNC_SEND|EC_VAL_NO_TCBS;
     goto ExitSend;
    }
ExitSend:

/* 15 June 2004 - NSP Performance Update : If Tx Ints are disabled then process them here */
/* 29 June 2004 - NSP Performance Update : Moved to end at request of BCIL */
/* 23 Aug  2004 - NSP Performance Update : If Tx Ints are re-enabled do not do Threshold check */

   if(HalDev->TxIntDisable /*&& DoThresholdCheck*/)
   {
      if(--HalDev->TxIntThreshold[Ch] <= 0)
      {
          int MoreWork;
          TxInt(HalDev, Ch, 0, &MoreWork);
          HalDev->TxIntThreshold[Ch] = HalDev->TxIntThresholdMaster[Ch];
      }
   }
    HalDev->OsFunc->CriticalOff();

   return(rc);
  }

/*
 *  This function processes receive interrupts.  It traverses the receive
 *  buffer queue, extracting the data and passing it to the upper layer software via
 *  osReceive().  It handles all error conditions and fragments without valid data by
 *  immediately returning the RCB's to the RCB pool.
 *
 *  @param   HalDev   CPHAL module instance. (set by cphalInitModule())
 *  @param   Ch       Channel Number.
 *  @param   MoreWork Flag that indicates that there is more work to do when set to 1.
 *
 *  @return  0 if OK, non-zero otherwise.
 */
static int RxInt(HAL_DEVICE *HalDev, int Ch, int *MoreWork)
  {
  HAL_RCB *CurrentRcb, *SopRcb, *EofRcb, *EopRcb;
  bit32u RxBufStatus,PacketsServiced, RxPktLen = 0, RxSopStatus,
    FrmFrags, TotalFrags, FrmLen;
  int base = HalDev->dev_base, Ret;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;
  int RxServiceMax = HalDev->ChData[Ch].RxServiceMax;
  int FragIndex; /* +GSG 030508 */
  
  if(HalDev->NeedsCount) /* +MJH 030410 */
    NeedsCheck(HalDev);  /* +MJH 030410 */

  /* Handle case of teardown interrupt */
  if (HalDev->RxTeardownPending[Ch] != 0)
    {
     Ret = RxTeardownInt(HalDev, Ch);
     if (Ret == 0)
       {                                                              /*+GSG 030303*/
        *MoreWork = 0;
        return (EC_NO_ERRORS);
       }                                                              /*+GSG 030303*/
    }

  /* Examine first RCB on the software active queue */
  CurrentRcb=HalDev->RxActQueueHead[Ch];
  osfuncDataCacheHitInvalidate((void*)CurrentRcb, 16);
  RxBufStatus=CurrentRcb->mode;
  PacketsServiced=0;
  
  /* Process received packets until we find hardware owned descriptors
     or until we hit RxServiceMax */
  while((CurrentRcb)&&((RxBufStatus&CB_OWNERSHIP_BIT)==0)&&
    (PacketsServiced<RxServiceMax)) /* ~GSG 030307 */
    {

     PacketsServiced++; /* ~GSG 030307 */
     SopRcb=CurrentRcb;
     RxSopStatus=RxBufStatus;
     RxPktLen = RxSopStatus&CB_SIZE_MASK;
    
     FrmFrags=0;
     TotalFrags=0;
     FragIndex=0;
     FrmLen=0;
     EofRcb=0;

/* +GSG 030508 *//* +GSG 030508 */

     /* Loop through all fragments that comprise current packet.  Build
        fraglist and exit when the end of the packet is reached, or the
        end of the descriptor list is reached. */
     do
       {
	    bit32u DmaLen;
	   

        DmaLen=CurrentRcb->Off_BLen;

        FrmLen+=DmaLen;
        TotalFrags++;
        if (!EofRcb)
          {
           HalDev->fraglist[FragIndex].data=((char *)CurrentRcb->DatPtr);       /* ~GSG 030508 */

           HalDev->fraglist[FragIndex].len=DmaLen;                              /* ~GSG 030508 */

           /* GSG 12/9 */
           HalDev->fraglist[FragIndex].OsInfo = CurrentRcb->OsInfo;             /* ~GSG 030508 */

           /* Upper layer must do the data invalidate */ 

           FrmFrags++;
           FragIndex++;                                                         /* ~GSG 030508 */
           if (FrmLen>=RxPktLen)
             EofRcb=CurrentRcb;
          }
        EopRcb=CurrentRcb;
        CurrentRcb=EopRcb->Next;
        if (CurrentRcb)
          {
           osfuncDataCacheHitInvalidate((void*)CurrentRcb,16);
          }
       }while(((EopRcb->mode&CB_EOF_BIT)==0)&&(CurrentRcb));
     
     /* Write the completion pointer for interrupt acknowledgement*/
     (CPMAC_RX_INT_ACK( base , Ch ))  = VirtToPhys(EopRcb) - HalDev->offset;

     EopRcb->Next=0;

     if (CurrentRcb == 0)
       { 
	    /* If we are out of RCB's we must not send this packet
	       to the OS. */
        int RcbSize = HalDev->ChData[Ch].RxBufSize;

        if (TotalFrags>1)
          {
           EopRcb->Off_BLen=RcbSize;
           EopRcb->mode=CB_OWNERSHIP_BIT;
           osfuncDataCacheHitWritebackAndInvalidate((void *)EopRcb, 16);
          }

        SopRcb->Off_BLen=RcbSize;
        SopRcb->mode=CB_OWNERSHIP_BIT;   
        osfuncDataCacheHitWritebackAndInvalidate((void *)SopRcb, 16);
      
        ((CPMAC_RX_HDP( base , Ch )) )  = VirtToPhys(SopRcb);
       }
      else
       { 
	    /* Dequeue packet and send to OS */
        int mode;

        /* setup SopRcb for the packet */
        SopRcb->Eop=(void*)EopRcb;

        /* dequeue packet */
        HalDev->RxActQueueHead[Ch]=CurrentRcb;
        
        if (EopRcb->mode&CB_EOQ_BIT)
          {
	       /* Next pointer is non-null and EOQ bit is set, which
	          indicates misqueue packet in CPPI protocol. */

           ((CPMAC_RX_HDP( base , Ch )) )  = EopRcb->HNext;
          }
        
        mode = (SopRcb->mode & 0xFFFF0000) | Ch;

        SopRcb->mode=(FrmFrags<<8)|Ch;
        SopRcb->Off_BLen=(bit32u)HalDev;
        
        /* send packet up the higher layer driver */
        OsFunc->Receive(HalDev->OsDev,HalDev->fraglist,FragIndex,RxPktLen,             /* ~GSG 030508 */
                       (HAL_RECEIVEINFO *)SopRcb,mode);
    
        RxBufStatus=CurrentRcb->mode;          
       }
    } /* while loop */
      
  if ((CurrentRcb)&&((RxBufStatus&CB_OWNERSHIP_BIT)==0)) /*~GSG 030307*/
    {
     *MoreWork = 1;
    }
   else
    {
     *MoreWork = 0;
    }

  return (EC_NO_ERRORS);
}

/*
 *  This function processes transmit interrupts.  It traverses the
 *  transmit buffer queue, detecting sent data buffers and notifying the upper
 *  layer software via osSendComplete().  (for SAR, i originally had this split
 *  into two functions, one for each queue, but joined them on 8/8/02)
 *
 *  @param   HalDev   CPHAL module instance. (set by cphalInitModule())
 *  @param   Queue    Queue number to service (always 0 for MAC, Choose 1 for SAR to service low priority queue)
 *  @param   MoreWork Flag that indicates that there is more work to do when set to 1.
 *
 *  @return  0 if OK, non-zero otherwise.
 */
int TxInt(HAL_DEVICE *HalDev, int Ch, int Queue, int *MoreWork)
  {
  HAL_TCB *CurrentTcb,*LastTcbProcessed,*FirstTcbProcessed;
  int PacketsServiced;
  bit32u TxFrameStatus;
  int base;
  int TxServiceMax = HalDev->ChData[Ch].TxServiceMax;
  OS_FUNCTIONS *OsFunc = HalDev->OsFunc;

/*+GSG 030303*//*+GSG 030303*/

  /* load the module base address */
  base = HalDev->dev_base;
  
  /* Handle case of teardown interrupt.  This must be checked at
     the top of the function rather than the bottom, because
     the normal data processing can wipe out the completion
     pointer which is used to determine teardown complete. */
  if (HalDev->TxTeardownPending[Ch] != 0)
    {
	 int Ret;
	 
     Ret = TxTeardownInt(HalDev, Ch, Queue);
     if (Ret == 0)
       {                                                              /*+GSG 030303*/
        *MoreWork = 0; /* bug fix 1/6 */                              /*+GSG 030303*/
        return (EC_NO_ERRORS);
       }                                                              /*+GSG 030303*/
    }

  OsFunc->CriticalOn(); /*  240904 */

  CurrentTcb = HalDev->TxActQueueHead[Ch][Queue];
  FirstTcbProcessed=CurrentTcb;

  if (CurrentTcb==0)
    {
     /* I saw this error a couple of times when multi-channels were added */
     dbgPrintf("[cppi TxInt()]TxH int with no TCB in queue!\n");
     dbgPrintf("              Ch=%d, CurrentTcb = 0x%08x\n", Ch, (bit32u)CurrentTcb);
     dbgPrintf("              HalDev = 0x%08x\n", (bit32u)HalDev);
     osfuncSioFlush();
     OsFunc->CriticalOff(); 
     return(EC_CPPI|EC_FUNC_TXINT|EC_VAL_NULL_TCB);
    }

  osfuncDataCacheHitInvalidate((void *)CurrentTcb, 16);
  TxFrameStatus=CurrentTcb->mode;
  PacketsServiced=0;

  /* should the ownership bit check be inside of the loop?? could make it a
     while-do loop and take this check away */
  if ((TxFrameStatus&CB_OWNERSHIP_BIT)==0)
    {
    do
      {
      /* Pop TCB(s) for packet from the stack */
      LastTcbProcessed=CurrentTcb->Eop;

      /* new location for acknowledge */
      /* Write the completion pointer */
      (CPMAC_TX_INT_ACK( base , Ch ))  = VirtToPhys(LastTcbProcessed) - HalDev->offset;

      HalDev->TxActQueueHead[Ch][Queue] = LastTcbProcessed->Next;

/*+GSG 030303*//*+GSG 030303*/

      osfuncDataCacheHitInvalidate((void *)LastTcbProcessed, 16);

      if (LastTcbProcessed->mode&CB_EOQ_BIT)
        {
         if (LastTcbProcessed->Next)
           {
            /* Misqueued packet */

               (CPMAC_TX_HDP( base , Ch ))  = LastTcbProcessed->HNext;

           }
          else
           {
       /* Tx End of Queue */

            HalDev->TxActive[Ch][Queue]=FALSE;
           }
        }

      OsFunc->SendComplete(CurrentTcb->OsInfo);

      /* Push Tcb(s) back onto the stack */
      CurrentTcb = LastTcbProcessed->Next;

      LastTcbProcessed->Next=HalDev->TcbPool[Ch][Queue];

      HalDev->TcbPool[Ch][Queue]=FirstTcbProcessed;

      PacketsServiced++;

      TxFrameStatus=CB_OWNERSHIP_BIT;
      /* set the first(SOP) pointer for the next packet */
      FirstTcbProcessed = CurrentTcb;
      if (CurrentTcb)
        {
         osfuncDataCacheHitInvalidate((void *)CurrentTcb, 16);
         TxFrameStatus=CurrentTcb->mode;
        }

      }while(((TxFrameStatus&CB_OWNERSHIP_BIT)==0)
             &&(PacketsServiced<TxServiceMax));

    if (((TxFrameStatus&CB_OWNERSHIP_BIT)==0)
             &&(PacketsServiced==TxServiceMax))
      {
       *MoreWork = 1;
      }
     else
      {
       *MoreWork = 0;
      }
    }
    OsFunc->CriticalOff(); 

  return(EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  This function performs a teardown for the given channel.  The value of the
 *  Mode parameter controls the operation of the function, as documented below.
 *
 *  Note: If bit 3 of Mode is set, this call is blocking, and will not return
 *  until the teardown interrupt has occurred and been processed. While waiting
 *  for a blocking teardown to complete, ChannelTeardown() will signal the OS
 *  (via Control(.."Sleep"..)) to allow the OS to perform other tasks if
 *  necessary.  If and only if bit 3 of Mode is clear, the CPHAL will call the
 *  OS TeardownComplete() function to indicate that the teardown has completed.
 *
 *  @param   HalDev  CPHAL module instance. (set by xxxInitModule())
 *  @param   Ch      Channel number.
 *  @param   Mode    Bit 0 (LSB): Perform Tx teardown (if set).<BR>
 *                   Bit 1: Perform Rx teardown (if set). <BR>
 *                   Bit 2: If set, perform full teardown (free buffers/descriptors).
 *                          If clear, perform partial teardown (keep buffers). <BR>
 *                   Bit 3 (MSB): If set, call is blocking.
 *                            If clear, call is non-blocking.
 *
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_INVALID_CH "EC_VAL_INVALID_CH"<BR>
 *           @ref EC_VAL_TX_TEARDOWN_ALREADY_PEND "EC_VAL_TX_TEARDOWN_ALREADY_PEND"<BR>
 *           @ref EC_VAL_RX_TEARDOWN_ALREADY_PEND "EC_VAL_RX_TEARDOWN_ALREADY_PEND"<BR>
 *           @ref EC_VAL_TX_CH_ALREADY_TORNDOWN "EC_VAL_TX_CH_ALREADY_TORNDOWN"<BR>
 *           @ref EC_VAL_RX_CH_ALREADY_TORNDOWN "EC_VAL_RX_CH_ALREADY_TORNDOWN"<BR>
 *           @ref EC_VAL_TX_TEARDOWN_TIMEOUT "EC_VAL_TX_TEARDOWN_TIMEOUT"<BR>
 *           @ref EC_VAL_RX_TEARDOWN_TIMEOUT "EC_VAL_RX_TEARDOWN_TIMEOUT"<BR>
 *           @ref EC_VAL_LUT_NOT_READY "EC_VAL_LUT_NOT_READY"<BR>
 */
static int halChannelTeardown(HAL_DEVICE *HalDev, int Ch, bit32 Mode)
  {
   int DoTx, DoRx, Sleep=2048, timeout=0;                         /*MJH~030306*/
   bit32u base = HalDev->dev_base;

/* Set the module, used for error returns */

   DoTx = (Mode & TX_TEARDOWN);
   DoRx = (Mode & RX_TEARDOWN);

   if (HalDev->State < enInitialized)
     return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_INVALID_STATE);

   if ((Ch < 0) || (Ch > (MAX_CHAN-1) ))
     {
     return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_INVALID_CH);
     }

   /* set teardown pending bits before performing the teardown, because they
      will be used in the int handler (this is done for AAL5) */
   if (DoTx)
     {
      if (HalDev->TxTeardownPending[Ch] != 0)
        return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_TX_TEARDOWN_ALREADY_PEND);

      /* If a full teardown, this also means that the user must
      setup all channels again to use them */
      if (Mode & FULL_TEARDOWN)
        HalDev->ChIsSetup[Ch][DIRECTION_TX] = 0;

      if (HalDev->State < enOpened)
        {
         /* if the hardware has never been opened, the channel has never actually
         been setup in the hardware, so I just need to reset the software flag
         and leave */
         HalDev->ChIsSetup[Ch][DIRECTION_TX] = 0;
         return (EC_NO_ERRORS);
    }
       else
    {
         if (HalDev->ChIsOpen[Ch][DIRECTION_TX] == 0)
       {
            return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_TX_CH_ALREADY_TORNDOWN);
       }

         /* set teardown flag */
         HalDev->TxTeardownPending[Ch] = Mode;
    }
     }

   if (DoRx)
     {
      if (HalDev->RxTeardownPending[Ch] != 0)
        return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_RX_TEARDOWN_ALREADY_PEND);

      if (Mode & FULL_TEARDOWN)
        HalDev->ChIsSetup[Ch][DIRECTION_RX] = 0;

      if (HalDev->State < enOpened)
        {
         HalDev->ChIsSetup[Ch][DIRECTION_RX] = 0;
         return (EC_NO_ERRORS);
    }
       else
    {
         if (HalDev->ChIsOpen[Ch][DIRECTION_RX] == 0)
           return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_RX_CH_ALREADY_TORNDOWN);

         HalDev->RxTeardownPending[Ch] = Mode;
    }
     }

   /* Perform Tx Teardown Duties */
   if ((DoTx) && (HalDev->State == enOpened))
     {
      /* Request TX channel teardown */
      (CPMAC_TX_TEARDOWN( base ))  = Ch;

      /* wait until teardown has completed */
      if (Mode & BLOCKING_TEARDOWN)
        {
     timeout = 0;
         while (HalDev->ChIsOpen[Ch][DIRECTION_TX] == TRUE)
       {
            osfuncSleep(&Sleep);

            timeout++;
        if (timeout > 100000)
          {
               return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_TX_TEARDOWN_TIMEOUT);
          }
       }
    }
     } /* if DoTx */

   /* Perform Rx Teardown Duties */
   if ((DoRx) && (HalDev->State == enOpened))
     {

      /* perform CPMAC specific RX channel teardown */
      CPMAC_RX_TEARDOWN(base) = Ch;

      if (Mode & BLOCKING_TEARDOWN)
        {
     timeout = 0;
         while (HalDev->ChIsOpen[Ch][DIRECTION_RX] == TRUE)
       {
            osfuncSleep(&Sleep);

            timeout++;
        if (timeout > 100000)
          {
               return(EC_CPMAC |EC_FUNC_CHTEARDOWN|EC_VAL_RX_TEARDOWN_TIMEOUT);
          }
       }
        }
     } /* if DoRx */

   return (EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  This function closes the CPHAL module.  The module will be reset.
 *  The Mode parameter should be used to determine the actions taken by
 *  Close().
 *
 *  @param   HalDev   CPHAL module instance. (set by xxxInitModule())
 *  @param   Mode     Indicates actions to take on close.  The following integer
 *                    values are valid: <BR>
 *                    1:  Does not free buffer resources, init parameters remain
 *                        intact.  User can then call Open() without calling Init()
 *                        to attempt to reset the device and bring it back to the
 *                        last known state.<BR>
 *                    2:  Frees the buffer resources, but keeps init parameters.  This
 *                        option is a more aggressive means of attempting a device reset.
 *                    3:  Frees the buffer resources, and clears all init parameters. <BR>
 *                        At this point, the caller would have to call to completely
 *                        reinitialize the device (Init()) before being able to call
 *                        Open().  Use this mode if you are shutting down the module
 *                        and do not plan to restart.
 *
 *  @return  EC_NO_ERRORS (ok).<BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           Any error code from halChannelTeardown().<BR>
 */
static int halClose(HAL_DEVICE *HalDev, bit32 Mode)
  {
   int Ch, Inst, Ret;
   OS_DEVICE *TmpOsDev;
   OS_FUNCTIONS *TmpOsFunc;
   HAL_FUNCTIONS *TmpHalFunc;
   char *TmpDeviceInfo;

   int Ticks;                                                     /*MJH~030306*/

   /* Verify proper device state */
   if (HalDev->State != enOpened)
     return (EC_CPMAC  | EC_FUNC_CLOSE|EC_VAL_INVALID_STATE);

   /* Teardown all open channels */
   for (Ch = 0; Ch <= (MAX_CHAN-1) ; Ch++)
     {
      if (HalDev->ChIsOpen[Ch][DIRECTION_TX] == TRUE)
        {
         if (Mode == 1)
           {
            Ret = halChannelTeardown(HalDev, Ch, TX_TEARDOWN | PARTIAL_TEARDOWN | BLOCKING_TEARDOWN);
            if (Ret) return (Ret);
           }
          else
           {
            Ret = halChannelTeardown(HalDev, Ch, TX_TEARDOWN | FULL_TEARDOWN | BLOCKING_TEARDOWN);
            if (Ret) return (Ret);
           }
        }

      if (HalDev->ChIsOpen[Ch][DIRECTION_RX] == TRUE)
        {
         if (Mode == 1)
           {
            Ret = halChannelTeardown(HalDev, Ch, RX_TEARDOWN | PARTIAL_TEARDOWN | BLOCKING_TEARDOWN);
            if (Ret) return (Ret);
           }
          else
           {
            Ret = halChannelTeardown(HalDev, Ch, RX_TEARDOWN | FULL_TEARDOWN | BLOCKING_TEARDOWN);
            if (Ret) return (Ret);
           }
        }
     }

   /* free fraglist in HalDev */
   HalDev->OsFunc->Free(HalDev->fraglist);
   HalDev->fraglist = 0;

   /* unregister the interrupt */
   HalDev->OsFunc->IsrUnRegister(HalDev->OsDev, HalDev->interrupt);

   Ticks = 0;    /* Disable Tick Timer */                           /*MJH+030306*/
   HalDev->OsFunc->Control(HalDev->OsDev, hcTick, hcClear, &Ticks); /*MJH+030306*/

   /* Free the Phy Information Structure */
   if(HalDev->PhyDev)
   {
      HalDev->OsFunc->Free(HalDev->PhyDev);                               /*MJH+030513*/
      HalDev->PhyDev = 0;                                         /*MJH+030522*/
   }

   /* Perform CPMAC specific closing functions */
   CPMAC_MACCONTROL(HalDev->dev_base) &= ~MII_EN;
   CPMAC_TX_CONTROL(HalDev->dev_base) &= ~TX_EN;
   CPMAC_RX_CONTROL(HalDev->dev_base) &= ~RX_EN;

   /* put device back into reset */
   (*(volatile bit32u *)(HalDev->ResetBase)) &=~ (1<<HalDev->ResetBit);
   Ticks = 64;                                                    /*MJH~030306*/
   osfuncSleep(&Ticks);

   /* If mode is 3, than clear the HalDev and set next state to DevFound*/
   if (Mode == 3)
     {
      /* I need to keep the HalDev parameters that were setup in InitModule */
      TmpOsDev = HalDev->OsDev;
      TmpOsFunc = HalDev->OsFunc;
      TmpDeviceInfo = HalDev->DeviceInfo;

      TmpHalFunc = HalDev->HalFuncPtr;
      Inst = HalDev->Inst;

      /* Clear HalDev */

      HalDev->OsFunc->Memset(HalDev, 0, sizeof(HAL_DEVICE));

      /* Restore key parameters */
      HalDev->OsDev = TmpOsDev;
      HalDev->OsFunc = TmpOsFunc;
      HalDev->DeviceInfo = TmpDeviceInfo;

      HalDev->HalFuncPtr = TmpHalFunc;
      HalDev->Inst = Inst;

      HalDev->State = enDevFound;
     }
    else
     {
      HalDev->State = enInitialized;
     }

   return(EC_NO_ERRORS);
  }
