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
**|         Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
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

/*******************************************************************************
 * FILE PURPOSE:    Flash operations header
 *******************************************************************************
 * FILE NAME:       flashop.h
 *
 * DESCRIPTION:     Flash operations header
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef	_FLASHOP_H_
#define	_FLASHOP_H_

#include "stddef.h"

#include <linux/types.h>

typedef __u16	uint16;
#if 1
typedef enum _flash_type {OLD, BSC, SCS, AMD, SST} flash_type_t;

typedef struct _flash_cmds
{
        flash_type_t type;
        bool                   need_unlock;
        uint16                  pre_erase;
        uint16                  erase_block;
        uint16                  erase_chip;
        uint16                  write_word;
        uint16                  write_buf;
        uint16                  clear_csr;
        uint16                  read_csr;
        uint16                  read_id;
        uint16                  confirm;
        uint16                  read_array;

}flash_cmds_t;

#define UNLOCK_CMD_WORDS 2
typedef struct _unlock_cmd{
        unsigned int            addr[UNLOCK_CMD_WORDS];
        uint16          cmd[UNLOCK_CMD_WORDS];
}unlock_cmd_t;



typedef struct _flash_desc{
        uint16          mfgid;
        uint16          devid;
        uint            size;
        uint            width;
        flash_type_t    type;
        uint            bsize;
        uint            nb;
        uint            ff;
        uint            lf;
        uint            nsub;
        uint            *subblocks;
        char            *desc;
}flash_desc_t;

#endif


typedef int (*procref1)(bit32u base);
typedef int (*procref2)(bit32u base,int size);
typedef int (*procref3)(bit32u base);
typedef int (*procref4)(bit32u adr, bit8 cVal);
typedef int (*procref5)(void);
typedef int (*procref6)(bit32u base,int size,int verbose);
typedef unsigned int (*procref7)(bit32u base);

typedef struct _Adam2Flash 
          {
          procref1 FWBGetBlockSize; /* called to get block size */
          procref6 FWBErase;        /* called to erase blocks */
          procref3 FWBOpen;         /* called to open block writes */
          procref4 FWBWriteByte;    /* called to write block byte */
          procref5 FWBClose;        /* called to close block writes */
          procref2 FWBUnLock;       /* called to close block writes */
          procref2 FWBLock;         /* called to close block writes */
          procref7 FWBGetNextBlock;  /* called to get base of the next blk */
          }Adam2Flash;



int FWBGetBlockSize(unsigned int base);           /*Get Flash Block Size            */
int FWBErase(unsigned int base,int size,int verbose); /*Erase flash                     */
int FWBOpen(unsigned int base);                   /*Prepare for flash writes        */
int FWBWriteByte(unsigned int adr, char cVal);    /*Write byte to flash             */
int FWBClose(void);                         /*Write any pending data to flash */
int FWBUnLock(unsigned int adr,int size);       /*Unlock Flash block(s)           */
int FWBLock(unsigned int from,int size);        /*Lock Flash block(s)             */

unsigned int FWBGetNextBlock(unsigned int base);  /* Get base addr of next block    */
        
#endif /* _FLASHOP_H_ */

