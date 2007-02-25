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

/** \file   pal_osList_inline.h
    \brief  OsLIST Services Header File

    This file defines APIs for handling simple bi-directional linked
    list that works with arbitrary data objects in a thread-safe manner.
    Movement of elements within the OsLIST is FIFO mode. 

    Only requirement on Elements strung onto the OsLIST is that they
    must begin with an OsLIST header comprising of a forward and reverse
    pointers. No Memory allocation or freeing is performed by these APIs.
    Memory allocation/freeing must be handled outside by the caller

    \note The current Linux implementation puts the onus on thead safety
    on the user of the List API
    
    (C) Copyright 2004, Texas Instruments, Inc

    @author     PSP Architecture Team
    @version    1.0
 */

#ifndef __PAL_OSLIST_INLINE_H__
#define __PAL_OSLIST_INLINE_H__

#include "pal_os.h"
#include "pal_defs.h"
#include "pal_osCfg.h"
/**
 * \brief   PAL OS List Append
 */

PAL_INLINE void PAL_osListAppend (Ptr head, Ptr newNode)
{
	Uint32 cookie;	
	PAL_OsListNodeHeader *headPtr = head;
	PAL_OsListNodeHeader *newNodePtr = newNode;

	/* Protect the list while updating */
	PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
	
    newNodePtr->next = headPtr;
    newNodePtr->prev = headPtr->prev;
    ((PAL_OsListNodeHeader *)(headPtr->prev))->next = newNodePtr;
    headPtr->prev = newNodePtr;

	PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
}

/**
 * \brief   PAL OS List Insert
 */

PAL_INLINE void PAL_osListInsert (Ptr node, Ptr newNode)
{
	Uint32 cookie;			
	PAL_OsListNodeHeader *nodePtr = node;
	PAL_OsListNodeHeader *newNodePtr = newNode;
	
	/* Protect the list while updating */
	PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);
	
    newNodePtr->prev = nodePtr;
    newNodePtr->next = nodePtr->next;
    nodePtr->next = newNodePtr;
    ((PAL_OsListNodeHeader *)(newNodePtr->next))->prev = newNodePtr;
	
	PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);	
}

/**
 * \brief   PAL OS List Remove
 */

PAL_INLINE Ptr PAL_osListRemove(Ptr node)
{
	Uint32 cookie;					
    PAL_OsListNodeHeader *tmpNode;
    PAL_OsListNodeHeader *nodePtr = node;            
	
    if (nodePtr->next == nodePtr)
	{
        return nodePtr;
	}
    
	/* Protect the list while updating */
	PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);	
	
    tmpNode = nodePtr->next;
    ((PAL_OsListNodeHeader *)(nodePtr->prev))->next = nodePtr->next;
    ((PAL_OsListNodeHeader *)(nodePtr->next))->prev = nodePtr->prev;

	PAL_OSLIST_MKNODE(nodePtr);

	PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
	
    return tmpNode;
}

/**
 * \brief   PAL OS List Prepend
 */

PAL_INLINE Ptr PAL_osListPrepend (Ptr head, Ptr newNode)
{
	Uint32 cookie;							
    PAL_OsListNodeHeader *headPtr = head;
    PAL_OsListNodeHeader *newNodePtr = newNode;

    if(newNodePtr)
    {
			
		/* Protect the list while updating */
		PAL_osProtectEntry(PAL_OSPROTECT_INTERRUPT, &cookie);				
		
        if(!headPtr)
        {
            newNodePtr->next = newNodePtr->prev = newNodePtr;
        }
        else
        {
            newNodePtr->next = headPtr;
            newNodePtr->prev  = headPtr->prev;
            headPtr->prev = newNodePtr;
            ((PAL_OsListNodeHeader *)(newNodePtr->prev))->next = newNodePtr;
        }

       	PAL_osProtectExit(PAL_OSPROTECT_INTERRUPT, cookie);
		
        return newNodePtr;
    }
    else 
    {
        return headPtr;
    }
}

/*\}*/
/*\}*/

#endif

