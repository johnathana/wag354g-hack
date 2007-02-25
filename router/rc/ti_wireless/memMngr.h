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
**|     Copyright (c) 1998 - 2003 Texas Instruments Incorporated         |**
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

/*************************************************************************
 *
 *	MODULE:	memMngr.h
 *  PURPOSE:	Driver memory management
 *
 **************************************************************************/
#ifndef _UTILS_MEM_H_
#define _UTILS_MEM_H_

#include "pform_types.h"

#define DATA_BUF_LEN  2036
#define DATA_OFFSET_IN_FIRST_BUF 64

#define WLAN_DRV_NULL_MEM_HANDLE  0xffffffff

#define memMgr_BufLength(BufAddr) ( ((mem_BD_T *)BufAddr)->length   )
#define memMgr_BufOffset(BufAddr) ( ((mem_BD_T *)BufAddr)->dataOffset )
#define memMgr_BufDataBase(BufAddr)   ( ((mem_BD_T *)BufAddr)->data )
#define memMgr_BufNext(BufAddr)   ( ((mem_BD_T *)BufAddr)->nextBDPtr )
#define memMgr_BufData(BufAddr)  (((mem_BD_T *)BufAddr)->data + ((mem_BD_T *)BufAddr)->dataOffset )

#define memMgr_MsduHdrLen(MsduAddr)		( ((mem_MSDU_T *)MsduAddr)->headerLen )
#define memMgr_MsduFirstLen(MsduAddr)	( ((mem_MSDU_T *)MsduAddr)->firstBDPtr->length )
#define memMgr_MsduHandle(MsduAddr)		( ((mem_MSDU_T *)MsduAddr)->handle )
#define memMgr_MsduHdrAddr(MsduAddr)	( memMgr_BufData(((mem_MSDU_T *)MsduAddr)->firstBDPtr))
#define memMgr_MsduPktAddr(MsduAddr)	( memMgr_BufDataBase(((mem_MSDU_T *)MsduAddr)->firstBDPtr) + DATA_OFFSET_IN_FIRST_BUF)
#define memMgr_MsduNextAddr(MsduAddr)	( ((mem_MSDU_T *)MsduAddr)->firstBDPtr->nextBDPtr )
#define memMgr_MsduDataAddr(MsduAddr)	( ((mem_MSDU_T *)MsduAddr)->firstBDPtr )
#define memMgr_MsduDataSize(MsduAddr)	( ((mem_MSDU_T *)MsduAddr)->dataLen )

typedef void (*ap_FreeMemFunc)(unsigned char*);

typedef struct mem_DataBuf_T mem_DataBuf_T;
struct mem_DataBuf_T {
	/* PUBLIC - For the use of the User */
	UINT8			data[DATA_BUF_LEN];	/* pointer to the Data. */

	/* READ ONLY */
	/* The user MUST not change the following fields */
	UINT32			handle;				/* Hanlde of this Data Buffer Structure */
  mem_DataBuf_T*  nextDataBuf; /* pointer to the next free DataBuf
                                  when this DataBuf is in Free mode */
	UINT32			refCount;			/* number of instances of this Data Buf */
};

typedef struct mem_BD_T mem_BD_T;
struct mem_BD_T {
   /* READ ONLY */
	/* The user MUST not change the following fields */
	UINT32			handle;			/* Hanlde of this BD Data Structure */
	UINT32			refCount;		/* number of instances of this BD */
	mem_DataBuf_T* dataBuf;       /* pointer to the Data Buffer */
	/* PUBLIC - For the use of the User */
   char*          data;          /* Pointer to the Data */
	UINT16			dataOffset;		/* offset of the data */
	UINT16			length;			/* the length of the data */
   ap_FreeMemFunc freeFunc;		/* pointer to the Data Buffer free function */
	UINT32			freeArgs[5];	/* arguments to be send with the free function */
	mem_BD_T*		nextBDPtr;		/* pointer to the next BD */
};

typedef struct mem_MSDU_T mem_MSDU_T;
struct mem_MSDU_T {
   /* READ ONLY */
   /* The user MUST not change the following fields */
   UINT32			handle;			 /* handle of this MSDU data structure */
   mem_MSDU_T*     nextMSDU;   	 /* pointer to the next Free MSDU when
                                     this MSDU Buffer is in Free mode */
   /* PUBLIC - For the use of the User */
   UINT32         headerLen;      /* the length of the 802.11 header */
   mem_BD_T*		firstBDPtr;	    /* pointer to the first BD */
   mem_BD_T*		lastBDPtr;		 /* pointer to the first BD */
   ap_FreeMemFunc	freeFunc;		 /* pointer to the Data Buffer free function */
   UINT32			freeArgs[5];	 /* arguments to be send with the free function */

   mem_MSDU_T*    nextConcatMSDU; /* pointer to the next MSDU in concatenated MPDU. */
   UINT16			dataLen;		    /* total length of this MSDU - updated by the user */
   UINT16         reserved;       /* padding, of 16bit */
};

typedef struct memMngr_Statistics_T memMngr_Statistics_T;
struct memMngr_Statistics_T {
   mem_MSDU_T* 	msduPool;    /* list of MSDU Buffer Desciptors */
   mem_BD_T* 		bdPool;      /* list of Data Buffer Descriptors */
   mem_DataBuf_T*	dataBufPool; /* list of Data Buffers */

   mem_MSDU_T* 	firstFreeMSDU; 	  /* pointer to the first free MSDU */
   mem_BD_T* 		firstFreeBD;      /* pointer to the first free BD */
   mem_DataBuf_T* 	firstFreeDataBuf; /* pointer to the first free Data Buffer */

   int             numFreeMSDU;
   int             numFreeBD;
   int             numFreeDataBuf;

};


/*************************************************************************
 *                        wlan_memMngrInit          		                     *
 *************************************************************************
DESCRIPTION: Init of the Memory Manager module

INPUT:
OUTPUT:
RETURN:      OK/NOK
**************************************************************************/
STATUS wlan_memMngrInit(void);

/*************************************************************************
 *                        wlan_memMngrAllocDataBuf                           *
 *************************************************************************
DESCRIPTION:This function allocates BDs and Data Buffers according to the
			required length. The memory manager will allocate the Data
			Buffers, update the buffer pointer in the BD structure and link
			the BDs when more than one Data Buffer is required.

INPUT:      len - the length of the required data buffer
OUTPUT:     BDPtr - a pointer in which this function will return a pointer
					to the allocated BD
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrAllocDataBuf(mem_BD_T** bdPtr, UINT32 len);

/*************************************************************************
 *                        wlan_memMngrAllocBDs                        		 *
 *************************************************************************
DESCRIPTION:This function allocates and returns a pointer to an array of BDs.
			This function does not allocate any memory buffers.

INPUT:      BDsNumber - number of required BDs
OUTPUT:     BDsPtr - a pointer in which this function will return a pointer
					 to an array of BD pointers
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrAllocBDs(UINT32 bdNumber, mem_BD_T** bdPtr);

/*************************************************************************
 *                        wlan_memMngrAllocMSDU                          *
 *************************************************************************
DESCRIPTION:This function allocates MPDU structure.

INPUT:		len - the length of the required data buffer
                    if len=0, than only MSDU buffer will be allocated
OUTPUT:     MSDUPtr - a pointer in which this function will return a pointer
					  to the MSDU structure
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrAllocMSDU (mem_MSDU_T** MSDUPtr, UINT32 len);

/*************************************************************************
 *                        wlan_memMngrAllocMSDUBufferOnly	             *
 *************************************************************************
DESCRIPTION:This function allocates MPDU structure - without Data Buffers

INPUT:
OUTPUT:     MSDUPtr - a pointer in which this function will return a pointer
					  to the MSDU structure
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrAllocMSDUBufferOnly(mem_MSDU_T** MSDUPtr);

/*************************************************************************
 *                        wlan_memMngrDuplicateMSDU                      *
 *************************************************************************
DESCRIPTION:This function duplicates the MSDU.

INPUT:      handle - handle of the MSDU the user want to duplicate
OUTPUT:     newHandle - pointer in which this function sets the handle of
                    the duplicated MSDU.
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrDuplicateMSDU(mem_MSDU_T *pDstMsdu, mem_MSDU_T *pSrcMsdu );

/*************************************************************************
 *                        wlan_memMngrFreeMSDU         	                     *
 *************************************************************************
DESCRIPTION:Free MSDU structure. This function will free all BDs and Data
			Buffers that are bind to this MSDU.

INPUT:      handle - handle of this MSDU
OUTPUT:
RETURN:     OK/NOK
**************************************************************************/
STATUS wlan_memMngrFreeMSDU(UINT32 handle);

/*************************************************************************
 *                        wlan_memMngrFreeBD          	                     *
 *************************************************************************
DESCRIPTION:Free BD structure. This function will free a list of BD
			structures and the Data Buffer that is being pointed by these BD
			if any. (e.g. - free MPDU)

INPUT:      handle - handle of this BD
OUTPUT:
RETURN:     freeFlag - return TRUE if this BD list was freed
                       return FALSE if this BD list was not freed (refCount>0)
**************************************************************************/
int wlan_memMngrFreeBD(UINT32 handle);

BOOL wlan_memMngrMsduIsFragmented(mem_MSDU_T *msduPtr);

void wlan_memMngrMsduReassemble(mem_MSDU_T *msduPtr, char *deFragBuffer);

void wlan_memMngrFillField(char **dataP, char *value, UINT32 len, mem_BD_T **BD);

STATUS wlan_memMngrPrintStatistics(memMngr_Statistics_T *ptr);

STATUS wlan_memMngrCopyMsdu(mem_MSDU_T *pDstMsdu, mem_MSDU_T *pSrcMsdu );
#endif /* _UTILS_MEM_H_ */

