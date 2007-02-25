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
**|             Copyright (c) 2003 Texas Instruments Incorporated        |**
**|             Copyright (C) 1999-2003 Igor Pavlov                      |**
**|                                                                      |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/



#include "Decoder.h"
#include "lzma_decoder.h"

#define MyMin(a,b) a < b ? a : b
#define MyMax(a,b) a > b ? a : b

#define NULL   0

#ifndef E_INVALIDARG
#define E_INVALIDARG    -1;
#endif

#define RETURN_IF_NOT_S_OK(x) { int __result_ = (x); if(__result_ != 0) return __result_; }

/**********************************	Dictionary Size	***********************************/
#define DEF_DICTIONARY_SIZE               0x200000   
#define DEF_LITERAL_POS_STATE_BITS        0
#define DEF_LITERAL_CONTEXT_BITS          3
#define DEF_POS_STATE_BITS                2


static void *freememstart;
extern void (*prnt)(unsigned int, char *);      /* Gen reference to Yamon print function */
void pprintf(char *ptr);          		/* Generate our own printf */
void *mmalloc(int);


void pprintf(char *ptr)
{
  unsigned int *tempptr = (unsigned int  *)0x90000534;
  prnt = ( void (*)(unsigned int, char *)) *tempptr;
  (*prnt)(0,ptr);
}
    
   
void *mmalloc(int size)
{       
    unsigned char *p;
    void *r;
                
    r = freememstart;
    p = (unsigned char *)r;
  
    p = p + size;
    freememstart = (void *)p;
        
    return(r);
} 
 


/********************************  RangeCoder	********************************************/


void CRangeDecoder_Normalize(CRangeDecoder *rDecoder)
{
   while (rDecoder->Range < kTopValue)
   {
      rDecoder->Code = (rDecoder->Code << 8) | *(rDecoder->local_ptr++);
      rDecoder->Range <<= 8;
   }
}

      
void CRangeDecoder_Init(CRangeDecoder *rDecoder, unsigned char *stream)
{
   int i=0;
   rDecoder->local_ptr = stream;
   rDecoder->Code = 0;
   rDecoder->Range = (unsigned int)-1;

   for(i = 0; i < 5; i++)
   {
      rDecoder->Code = (rDecoder->Code << 8) | *(rDecoder->local_ptr++);
   }
}


void CRangeDecoder_ReleaseStream(CRangeDecoder *rDecoder)
{ 
  /* Stream.ReleaseStream(); */ 
} 

    
UINT32 CRangeDecoder_GetThreshold(CRangeDecoder *rDecoder, UINT32 total)
{
   return (rDecoder->Code) / ((rDecoder->Range) /= total);
}

      
void CRangeDecoder_Decode(CRangeDecoder *rDecoder, UINT32 start, UINT32 size, UINT32 total)
{
   rDecoder->Code -= start * (rDecoder->Range);
   rDecoder->Range *= size;

   while (rDecoder->Range < kTopValue)
   {
      rDecoder->Code = (rDecoder->Code << 8) | *(rDecoder->local_ptr++);
      rDecoder->Range <<= 8;
   }
}   


UINT32 CRangeDecoder_DecodeDirectBits(CRangeDecoder *rDecoder, UINT32 numTotalBits)
{
   UINT32 t,i = 0;
   UINT32 range = rDecoder->Range;
   UINT32 code = rDecoder->Code;        
   UINT32 result = 0;
   for (i = numTotalBits; i > 0; i--)
   {
      range >>= 1;
      t = (code - range) >> 31;
      code -= range & (t - 1);
      result = (result << 1) | (1 - t);
            
      if (range < kTopValue)
      {
         code = (code << 8) | *(rDecoder->local_ptr++);
         range <<= 8; 
      }
   }
   rDecoder->Range = range;
   rDecoder->Code = code;

   return result;
}

      
UINT32 CRangeDecoder_DecodeBit(CRangeDecoder *rDecoder, UINT32 size0, UINT32 numTotalBits)
{
   UINT32 newBound = ((rDecoder->Range) >> numTotalBits) * size0;
   UINT32 symbol;
   if (rDecoder->Code < newBound)
   {
      symbol = 0;
      rDecoder->Range = newBound;
   }
   else
   {
      symbol = 1;
      rDecoder->Code -= newBound;
      rDecoder->Range -= newBound;
   }

   while (rDecoder->Range < kTopValue)
   {
      rDecoder->Code = (rDecoder->Code << 8) | *(rDecoder->local_ptr++);
      rDecoder->Range <<= 8;
   }

   return symbol;
}


UINT32 CRangeDecoder_GetProcessedSize(CRangeDecoder *rDecoder)
{
  return 0;
}


/********************************  AriBitCoder	********************************************/


void CMyBitDecoder_Init(CMyBitDecoder *bitDecoder)
{
  bitDecoder->Probability = kBitModelTotal/2;
}


UINT32 CMyBitDecoder_Decode(CMyBitDecoder *bitDecoder, CRangeDecoder *rangeDecoder)
{
    UINT32 newBound = (rangeDecoder->Range >> kNumBitModelTotalBits) * (bitDecoder->Probability);
    if (rangeDecoder->Code < newBound)
    {
      rangeDecoder->Range = newBound;
      bitDecoder->Probability += (kBitModelTotal - bitDecoder->Probability) >> (bitDecoder->aNumMoveBits);
      if (rangeDecoder->Range < kTopValue)
      {
        rangeDecoder->Code = (rangeDecoder->Code << 8) | *(rangeDecoder->local_ptr++);;
        rangeDecoder->Range <<= 8;
      }
      return 0;
    }
    else
    {
      rangeDecoder->Range -= newBound;
      rangeDecoder->Code -= newBound;
      bitDecoder->Probability -= (bitDecoder->Probability) >> (bitDecoder->aNumMoveBits);
      if (rangeDecoder->Range < kTopValue)
      {
        rangeDecoder->Code = (rangeDecoder->Code << 8) | *(rangeDecoder->local_ptr++);
        rangeDecoder->Range <<= 8;
      }
      return 1;
    }
}


/********************************  BitTreeCoder	********************************************/


void CBitTreeDecoder_Init(CBitTreeDecoder *btDecoder)
{
  UINT32 i;

  for(i = 1; i < (1 << btDecoder->NumBitLevels); i++)
    CMyBitDecoder_Init(&(btDecoder->Models[i]));
}


UINT32 CBitTreeDecoder_Decode(CBitTreeDecoder *btDecoder, CMyRangeDecoder *rangeDecoder)
{
   UINT32 bitIndex, modelIndex = 1;
   RC_INIT_VAR

   for(bitIndex = btDecoder->NumBitLevels; bitIndex > 0; bitIndex--)
   {
     RC_GETBIT(btDecoder->numMoveBits, btDecoder->Models[modelIndex].Probability, modelIndex)
   }
   RC_FLUSH_VAR

   return modelIndex - (1 << btDecoder->NumBitLevels);
}

	
UINT8 CReverseBitTreeDecoder2_Create(CReverseBitTreeDecoder2 *rbtDecoder2, UINT32 numBitLevels)
{
   rbtDecoder2->NumBitLevels = numBitLevels;
   rbtDecoder2->Models = (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << numBitLevels));
   if (rbtDecoder2->Models != NULL)
     return (1);
   else
     return (-1);
}


void CReverseBitTreeDecoder2_Init(CReverseBitTreeDecoder2 *rbtDecoder2)
{
   UINT32 i;
   UINT32 numModels = 1 << (rbtDecoder2->NumBitLevels);

   for(i = 1; i < numModels; i++)
     CMyBitDecoder_Init(&(rbtDecoder2->Models[i]));
}


UINT32 CReverseBitTreeDecoder2_Decode(CReverseBitTreeDecoder2 *rbtDecoder2, CMyRangeDecoder *rangeDecoder)
{
   UINT32 modelIndex = 1;
   UINT32 symbol = 0;
   UINT32 bitIndex;
   RC_INIT_VAR
   for(bitIndex = 0; bitIndex < rbtDecoder2->NumBitLevels; bitIndex++)
   {
     RC_GETBIT2(rbtDecoder2->numMoveBits, rbtDecoder2->Models[modelIndex].Probability, modelIndex, ; , symbol |= (1 << bitIndex))
   }
   RC_FLUSH_VAR
   return symbol;
}


void CReverseBitTreeDecoder_Init(CReverseBitTreeDecoder *rbtDecoder)
{
  UINT32 i;

  for(i = 1; i < (1 << (rbtDecoder->NumBitLevels)); i++)
     CMyBitDecoder_Init(&(rbtDecoder->Models[i]));
}


UINT32 CReverseBitTreeDecoder_Decode(CReverseBitTreeDecoder *rbtDecoder, CMyRangeDecoder *rangeDecoder)
{
   UINT32 modelIndex = 1;
   UINT32 symbol = 0;
   UINT32 bitIndex;
   RC_INIT_VAR

   for(bitIndex = 0; bitIndex < (rbtDecoder->NumBitLevels); bitIndex++)
   {
     RC_GETBIT2(rbtDecoder->numMoveBits, rbtDecoder->Models[modelIndex].Probability, modelIndex, ; , symbol |= (1 << bitIndex))
   }
   RC_FLUSH_VAR

   return symbol;
}



/********************************  LenCoder	********************************************/


void CDecoderLength_Initialize(CDecoderLength *decoderLength)
{
  int i;

  decoderLength->_choice.aNumMoveBits = kNumMoveBits;  
  decoderLength->_choice2.aNumMoveBits = kNumMoveBits;  

  for(i=0; i < kNumPosStatesMax; i++)
  {
    decoderLength->_lowCoder[i].numMoveBits 	= kNumMoveBits;
    decoderLength->_lowCoder[i].NumBitLevels 	= kNumLenBits;
    decoderLength->_lowCoder[i].Models	     	= (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << kNumLenBits));

    decoderLength->_midCoder[i].numMoveBits 	= kNumMoveBits;
    decoderLength->_midCoder[i].NumBitLevels 	= kNumMidBits;
    decoderLength->_midCoder[i].Models	     	= (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << kNumMidBits));
  }

  decoderLength->_highCoder.numMoveBits 	= kNumMoveBits;
  decoderLength->_highCoder.NumBitLevels 	= kNumHighBits;
  decoderLength->_highCoder.Models		= (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << kNumHighBits));
}


void CDecoderLength_Create(CDecoderLength *decoderLength, UINT32 numPosStates)
{ 
  decoderLength->_numPosStates = numPosStates; 
}


void CDecoderLength_Init(CDecoderLength *decoderLength)
{
  UINT32 posState;

//Ramesh ???
  CDecoderLength_Initialize(decoderLength);

  CMyBitDecoder_Init(&(decoderLength->_choice));

  for (posState = 0; posState < decoderLength->_numPosStates; posState++)
  {
    CBitTreeDecoder_Init(&(decoderLength->_lowCoder[posState]));
    CBitTreeDecoder_Init(&(decoderLength->_midCoder[posState]));
  }

  CMyBitDecoder_Init(&(decoderLength->_choice2));
  CBitTreeDecoder_Init(&(decoderLength->_highCoder));
}


UINT32 CDecoderLength_Decode(CDecoderLength *decoderLength, CMyRangeDecoder *rangeDecoder, UINT32 posState)
{
  if (CMyBitDecoder_Decode(&(decoderLength->_choice),rangeDecoder) == 0)  
  {
    return (CBitTreeDecoder_Decode(&(decoderLength->_lowCoder[posState]),rangeDecoder));
  }
  if (CMyBitDecoder_Decode(&(decoderLength->_choice2),rangeDecoder) == 0)  
  {
    return (kNumLowSymbols + CBitTreeDecoder_Decode(&(decoderLength->_midCoder[posState]),rangeDecoder));
  }
  return (kNumLowSymbols + kNumMidSymbols + CBitTreeDecoder_Decode(&(decoderLength->_highCoder),rangeDecoder));
}



/********************************  LiteralCoder	********************************************/


void CDecoder2_Init(CDecoder2 *decoder2)
{
  int i,j;
  for (i = 0; i < 3; i++)
  {
    for (j = 1; j < (1 << 8); j++)
    {
      CMyBitDecoder_Init(&(decoder2->_decoders[i][j]));
      decoder2->_decoders[i][j].aNumMoveBits = kNumMoveBits;
    }
  }
}


BYTE CDecoder2_DecodeNormal(CDecoder2 *decoder2,CMyRangeDecoder *rangeDecoder)
{
  UINT32 symbol = 1;
  RC_INIT_VAR

  do
  {
    RC_GETBIT(kNumMoveBits, decoder2->_decoders[0][symbol].Probability, symbol);
  }
  while (symbol < 0x100);
  RC_FLUSH_VAR

  return symbol;
}


BYTE CDecoder2_DecodeWithMatchByte(CDecoder2 *decoder2,CMyRangeDecoder *rangeDecoder, BYTE matchByte)
{
  UINT32 symbol = 1;
  RC_INIT_VAR

  do
  {
    UINT32 bit;
    UINT32 matchBit = (matchByte >> 7) & 1;
    matchByte <<= 1;

    RC_GETBIT2(kNumMoveBits, decoder2->_decoders[1 + matchBit][symbol].Probability, symbol, bit = 0, bit = 1)
    if (matchBit != bit)
    {
      while (symbol < 0x100)
      {
        RC_GETBIT(kNumMoveBits, decoder2->_decoders[0][symbol].Probability, symbol)
      }
      break;
    }
  }
  while (symbol < 0x100);
  RC_FLUSH_VAR

  return symbol;
}


void CDecoderLiteral_Create(CDecoderLiteral *decoderLiteral,UINT32 numPosBits, UINT32 numPrevBits)
{
  UINT32 numStates;
  decoderLiteral->_numPosBits = numPosBits;
  decoderLiteral->_posMask = (1 << numPosBits) - 1;
  decoderLiteral->_numPrevBits = numPrevBits;
  numStates = 1 << (decoderLiteral->_numPrevBits + decoderLiteral->_numPosBits);
  decoderLiteral->_coders = (CDecoder2 *)mmalloc(sizeof(CDecoder2) * numStates);
}


void CDecoderLiteral_Init(CDecoderLiteral *decoderLiteral)
{
  UINT32 i;
  UINT32 numStates = 1 << (decoderLiteral->_numPrevBits + decoderLiteral->_numPosBits);
  for (i = 0; i < numStates; i++)
  {
    CDecoder2_Init(&(decoderLiteral->_coders[i]));
  }
}


UINT32 CDecoderLiteral_GetState(CDecoderLiteral *decoderLiteral,UINT32 pos, BYTE prevByte)
{ 
  return ((pos & decoderLiteral->_posMask) << decoderLiteral->_numPrevBits) + (prevByte >> (8 - decoderLiteral->_numPrevBits)); 
}


BYTE CDecoderLiteral_DecodeNormal(CDecoderLiteral *decoderLiteral,CMyRangeDecoder *rangeDecoder, UINT32 pos, BYTE prevByte)
{ 
  UINT32 state;
  state = CDecoderLiteral_GetState(decoderLiteral,pos,prevByte);
  return CDecoder2_DecodeNormal(&(decoderLiteral->_coders[state]),rangeDecoder);
}


BYTE CDecoderLiteral_DecodeWithMatchByte(CDecoderLiteral *decoderLiteral,CMyRangeDecoder *rangeDecoder, UINT32 pos, BYTE prevByte, BYTE matchByte)
{ 
  UINT32 state;
  state = CDecoderLiteral_GetState(decoderLiteral,pos,prevByte);
  return CDecoder2_DecodeWithMatchByte(&(decoderLiteral->_coders[state]),rangeDecoder,matchByte);
}




/********************************  LZMA	********************************************/



void CState_Init(CState *state)
{ 
  state->Index = 0; 
}


void UpdateChar(CState *state)
{ 
  state->Index = kLiteralNextStates[state->Index]; 
}


void UpdateMatch(CState *state)
{ 
  state->Index = kMatchNextStates[state->Index]; 
}


void UpdateRep(CState *state)
{ 
  state->Index = kRepNextStates[state->Index]; 
}


void UpdateShortRep(CState *state)
{ 
  state->Index = kShortRepNextStates[state->Index]; 
}


void CBaseCoder_Init(CBaseCoder *baseCoder)
{
  int i;
  CState_Init(&(baseCoder->_state));
  baseCoder->_previousByte = 0;
  for(i = 0 ; i < kNumRepDistances; i++)
    baseCoder->_repDistances[i] = 0;
}



/********************************  Decoder	********************************************/


void RareOutputStream_OutStreamInit(RareOutputStream *routStream, unsigned char *ptr)
{
   routStream->_buffer = ptr;
   routStream->_pos = 0;
}


void RareOutputStream_PutOneByte(RareOutputStream *routStream, BYTE b)
{
   routStream->_buffer[routStream->_pos++] = b;
}


unsigned char RareOutputStream_GetOneByte(RareOutputStream *routStream, unsigned int index)
{
   unsigned int pos = routStream->_pos + index;

   return routStream->_buffer[pos]; 
}


void RareOutputStream_CopyBackBlock(RareOutputStream *routStream, unsigned int distance, unsigned int len)
{
   unsigned int pos = routStream->_pos - distance - 1;

   for(; len > 0; len--)
   {
      routStream->_buffer[routStream->_pos++] = routStream->_buffer[pos++];
   }
}


void CDecoder_CDecoder(CDecoder *decoder)
{
  init_RangeCoder_Variables();
  init_AriBitCoder_Variables();
  init_LenCoder_Variables();
  init_LZMA_Variables();
  Create(decoder);
}


int Create(CDecoder *decoder)
{
  int i;
  for(i = 0; i < kNumPosModels; i++)
  {
      CReverseBitTreeDecoder2_Create(&(decoder->_posDecoders[i]), (((kStartPosModelIndex + i) >> 1) - 1));
  }

  return(0);
}


int Init(CDecoder *decoder, unsigned char *InPtr, unsigned char *OutPtr)
{
   int i,j;

   CRangeDecoder_Init(&(decoder->_rangeDecoder),InPtr);
   RareOutputStream_OutStreamInit(&(decoder->_outWindowStream), OutPtr);

   for(i = 0; i < kNumStates; i++)
   {
      for (j = 0; j <= decoder->_posStateMask; j++)
      {
 	 CMyBitDecoder_Init(&(decoder->_mainChoiceDecoders[i][j]));
 	 CMyBitDecoder_Init(&(decoder->_matchRepShortChoiceDecoders[i][j]));

         decoder->_mainChoiceDecoders[i][j].aNumMoveBits = kNumMoveBitsForMainChoice;
         decoder->_matchRepShortChoiceDecoders[i][j].aNumMoveBits = kNumMoveBitsForMainChoice;
      }

       CMyBitDecoder_Init(&(decoder->_matchChoiceDecoders[i]));
       CMyBitDecoder_Init(&(decoder->_matchRepChoiceDecoders[i]));
       CMyBitDecoder_Init(&(decoder->_matchRep1ChoiceDecoders[i]));
       CMyBitDecoder_Init(&(decoder->_matchRep2ChoiceDecoders[i]));

      decoder->_matchChoiceDecoders[i].aNumMoveBits = kNumMoveBitsForMainChoice;
      decoder->_matchRepChoiceDecoders[i].aNumMoveBits = kNumMoveBitsForMainChoice;
      decoder->_matchRep1ChoiceDecoders[i].aNumMoveBits = kNumMoveBitsForMainChoice;
      decoder->_matchRep2ChoiceDecoders[i].aNumMoveBits = kNumMoveBitsForMainChoice;
   }
   
   CDecoderLiteral_Init(&(decoder->_literalDecoder));
   
   for (i = 0; i < kNumLenToPosStates; i++)
   {
      decoder->_posSlotDecoder[i].numMoveBits = kNumMoveBitsForPosSlotCoder;
      decoder->_posSlotDecoder[i].NumBitLevels = kNumPosSlotBits;
      decoder->_posSlotDecoder[i].Models = (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << kNumPosSlotBits));
      CBitTreeDecoder_Init(&(decoder->_posSlotDecoder[i]));
   }    

   for(i = 0; i < kNumPosModels; i++)
   {
      decoder->_posDecoders[i].numMoveBits = kNumMoveBitsForPosCoders;
      CReverseBitTreeDecoder2_Init(&(decoder->_posDecoders[i]));
   }
   
   CDecoderLength_Init(&(decoder->_lenDecoder));
   CDecoderLength_Init(&(decoder->_repMatchLenDecoder));

   decoder->_posAlignDecoder.numMoveBits = kNumMoveBitsForAlignCoders;
   decoder->_posAlignDecoder.NumBitLevels = kNumAlignBits;
   decoder->_posAlignDecoder.Models = (CMyBitDecoder *)mmalloc(sizeof(CMyBitDecoder) * (1 << kNumAlignBits));
   CReverseBitTreeDecoder_Init(&(decoder->_posAlignDecoder));

   return(0);
}


int CodeReal(CDecoder *decoder, unsigned char *InPtr,unsigned char *OutPtr, const UINT32 *inSize, const UINT32 *outSize,unsigned int *progress)
{
  CState state;
  int i;
  unsigned int nowPos32 = 0;
  unsigned int size = (outSize == NULL) ? (unsigned int)(int)(-1) : *outSize;
  int peviousIsMatch = -1;
  BYTE previousByte = 0;
  unsigned int repDistances[kNumRepDistances];

  RareOutputStream *stream 	= &(decoder->_outWindowStream);
  CMyRangeDecoder  *rDecoder 	= &(decoder->_rangeDecoder);
  CReverseBitTreeDecoder *rbtDecoder = &(decoder->_posAlignDecoder);
  CDecoderLength  *lenDecoder  = &(decoder->_lenDecoder);
  CDecoderLength  *repMatchLenDecoder  = &(decoder->_repMatchLenDecoder);
  CDecoderLiteral *literalDecoder = &(decoder->_literalDecoder);

  Init(decoder,InPtr,OutPtr);

  CState_Init(&state);

  for(i = 0 ; i < kNumRepDistances; i++)
    repDistances[i] = 0;

  while(nowPos32 < size)
  {
    unsigned int nextPos = MyMin(nowPos32 + (1 << 18), size);

    while(nowPos32 < nextPos)
    {
      unsigned int posState = (unsigned int)(nowPos32) & decoder->_posStateMask;
	
      if (CMyBitDecoder_Decode(&(decoder->_mainChoiceDecoders[state.Index][posState]), rDecoder) == kMainChoiceLiteralIndex)	
      {
        UpdateChar(&state);
        if(peviousIsMatch > 0)
        {
          BYTE matchByte = RareOutputStream_GetOneByte(stream, (0 - repDistances[0] - 1));
          previousByte = CDecoderLiteral_DecodeWithMatchByte(literalDecoder,rDecoder,(unsigned int)(nowPos32),previousByte,matchByte);
          peviousIsMatch = -1;
        }
        else
	{
          previousByte = CDecoderLiteral_DecodeNormal(literalDecoder,rDecoder, (unsigned int)(nowPos32), previousByte);
	}
	RareOutputStream_PutOneByte(stream, previousByte);
        nowPos32++;
      }
      else             
      {
        unsigned int distance, len;
        peviousIsMatch = 1;

        if(CMyBitDecoder_Decode(&(decoder->_matchChoiceDecoders[state.Index]), rDecoder) == kMatchChoiceRepetitionIndex)
        {
          if(CMyBitDecoder_Decode(&(decoder->_matchRepChoiceDecoders[state.Index]), rDecoder) == 0)
          {
            if(CMyBitDecoder_Decode(&(decoder->_matchRepShortChoiceDecoders[state.Index][posState]), rDecoder) == 0)
            {
              UpdateShortRep(&state);
              previousByte = RareOutputStream_GetOneByte(stream,(0 - repDistances[0] - 1));
	      RareOutputStream_PutOneByte(stream, previousByte);
              nowPos32++;
              continue;
            }
            distance = repDistances[0];
          }
          else
          {
            if(CMyBitDecoder_Decode(&(decoder->_matchRep1ChoiceDecoders[state.Index]), rDecoder) == 0)
              distance = repDistances[1];
            else 
            {
              if(CMyBitDecoder_Decode(&(decoder->_matchRep2ChoiceDecoders[state.Index]), rDecoder) == 0)
                distance = repDistances[2];
              else
              {
                distance = repDistances[3];
                repDistances[3] = repDistances[2];
              }
              repDistances[2] = repDistances[1];
            }
            repDistances[1] = repDistances[0];
            repDistances[0] = distance;
          }
	  len = CDecoderLength_Decode(repMatchLenDecoder, rDecoder, posState) + kMatchMinLen;

          UpdateRep(&state);
        }
        else
        {
	  unsigned int posSlot;
          len = CDecoderLength_Decode(lenDecoder, rDecoder, posState) + kMatchMinLen;
          UpdateMatch(&state);
          posSlot = CBitTreeDecoder_Decode(&(decoder->_posSlotDecoder[GetLenToPosState(len)]), rDecoder);
	  if (posSlot >= kStartPosModelIndex)
          {
            unsigned int numDirectBits = (posSlot >> 1) - 1;
            distance = ((2 | (posSlot & 1)) << numDirectBits);

            if (posSlot < kEndPosModelIndex)
              distance += CReverseBitTreeDecoder2_Decode(&(decoder->_posDecoders[posSlot - kStartPosModelIndex]), rDecoder);
            else
            {
              distance += (CRangeDecoder_DecodeDirectBits(rDecoder,(numDirectBits - kNumAlignBits)) << kNumAlignBits);
              distance += CReverseBitTreeDecoder_Decode(rbtDecoder, rDecoder);
            }
          }
          else
            distance = posSlot;

          repDistances[3] = repDistances[2];
          repDistances[2] = repDistances[1];
          repDistances[1] = repDistances[0];
          repDistances[0] = distance;
        }
        if (distance >= nowPos32 || distance >= decoder->_dictionarySizeCheck)
        {
          if (distance == (unsigned int)(-1) && size == (unsigned int)(int)(-1))
          {
            return 0;
          }
        }

        RareOutputStream_CopyBackBlock(stream,distance, len);
        nowPos32 += len;
        previousByte = RareOutputStream_GetOneByte(stream,(0 - 1));
      }
    }

  }

  return(0);
}


int SetDictionarySize(CDecoder *decoder, UINT32 dictionarySize)
{
  if (decoder->_dictionarySize != dictionarySize)
  {
    unsigned int blockSize;
    decoder->_dictionarySize = dictionarySize;
    decoder->_dictionarySizeCheck = MyMax(decoder->_dictionarySize, (unsigned int)(1));
    blockSize = MyMax(decoder->_dictionarySizeCheck, (unsigned int)(1 << 12));
  }

  return(0);
}


int SetLiteralProperties(CDecoder *decoder, UINT32 numLiteralPosStateBits, UINT32 numLiteralContextBits)
{
  if (numLiteralPosStateBits > 8)
    return E_INVALIDARG;
  if (numLiteralContextBits > 8)
    return E_INVALIDARG;

  CDecoderLiteral_Create(&(decoder->_literalDecoder),numLiteralPosStateBits, numLiteralContextBits);

  return(0);
}


int SetPosBitsProperties(CDecoder *decoder, UINT32 numPosStateBits)
{
  unsigned int numPosStates;

  if (numPosStateBits > kNumPosStatesBitsMax)
    return E_INVALIDARG;

  numPosStates = 1 << numPosStateBits;
  CDecoderLength_Create(&(decoder->_lenDecoder), numPosStates);
  CDecoderLength_Create(&(decoder->_repMatchLenDecoder), numPosStates);
  decoder->_posStateMask = numPosStates - 1;

  return(0);
}



int cm_hwDecodeLZMA(unsigned char *pbDest, unsigned int uiDecompressedLength, unsigned char *pbSource, unsigned int uiCompressedLength, void *fptr)
{
   CDecoder MyDecoder;

   freememstart = fptr;

   CDecoder_CDecoder(&MyDecoder);
   SetDictionarySize(&MyDecoder,DEF_DICTIONARY_SIZE);
   SetLiteralProperties(&MyDecoder, DEF_LITERAL_POS_STATE_BITS, DEF_LITERAL_CONTEXT_BITS);
   SetPosBitsProperties(&MyDecoder, DEF_POS_STATE_BITS);

   /*======================================================
   Decode
   =======================================================*/
   return CodeReal(&MyDecoder, pbSource + 0x20,pbDest,&uiCompressedLength,&uiDecompressedLength,0);
}


