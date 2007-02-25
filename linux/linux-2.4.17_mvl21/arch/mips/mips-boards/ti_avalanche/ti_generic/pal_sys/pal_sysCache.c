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

/** \file   pal_sys_cache.c
    \brief  PAL cache code file

	FOR DOCUMENTATION OF FUNCTIONS: Refer file pal_sys.h
	
    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */

#include "pal.h"

int PAL_sysCacheInvalidate(PAL_OsMemAddrSpace addrSpace,
                                  void *mem_start_ptr,
                                  unsigned int num_bytes)
{
    unsigned int memEndAddr = num_bytes + ((unsigned int)mem_start_ptr & 0xf);
    unsigned int i;

    if(addrSpace == PAL_OSMEM_ADDR_DAT)
    {
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
           
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 17, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));
        }
    } 
	else if(addrSpace == PAL_OSMEM_ADDR_PRG) 
	{
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 16, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));
        }        
    } 
	else 
	{
        return -1;
    }

    return 0;            
}

int PAL_sysCacheFlush(PAL_OsMemAddrSpace addrSpace,
                             void *mem_start_ptr,
                             unsigned int num_bytes)
{
#ifdef MIPS_4KEC
    unsigned int memEndAddr = num_bytes + ((unsigned int)mem_start_ptr & 0xf);
    unsigned int i;

    if(addrSpace == PAL_OSMEM_ADDR_DAT)
    {
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 25, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));
        }
    } else if(addrSpace == PAL_OSMEM_ADDR_PRG) {
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 24, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));
        }        
    } else {
        return -1;
    }
#endif
	return 0;
}

int PAL_sysCacheFlushAndInvalidate(PAL_OsMemAddrSpace addrSpace,
                             void *mem_start_ptr,
                             unsigned int num_bytes)
{
#ifdef MIPS_4KEC
    unsigned int memEndAddr = num_bytes + ((unsigned int)mem_start_ptr & 0xf);
    unsigned int i;

    if(addrSpace == PAL_OSMEM_ADDR_DAT)
    {
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 21, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));
        }
    } else if(addrSpace == PAL_OSMEM_ADDR_PRG) {
        for(i=0; i<memEndAddr; i+=16, mem_start_ptr+=16)
        {
	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 24, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));

	        __asm__ __volatile__(
		        ".set noreorder\n\t"
		        ".set mips3\n\t"
		        "cache 16, (%0)\n\t"
		        ".set mips0\n\t"
		        ".set reorder"
		        :
		        : "r" (mem_start_ptr));			
        }        
    } else {
        return -1;
    }
#endif
	return 0;
}
