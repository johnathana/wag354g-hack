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
 * FILE PURPOSE:    Bootloader error codes.
 *******************************************************************************
 * FILE NAME:       errno.h
 *
 * DESCRIPTION:     Bootloader error codes.
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef SBL_ERRNO_H_
#define SBL_ERRNO_H_

#define EC_FILE_DUMP           0x01000000
#define EC_DUMP_MEMORY         0x02000000
#define EC_INSPECT_MEMORY      0x03000000

#define EC_NO_ERRORS           0x00000000
#define EC_INVALID_FILENAME    0x00000001
#define EC_FILE_NOT_FOUND      0x00000002
#define EC_INVALID_HEX_NUMBER  0x00000003
#define EC_INVALID_RANGE       0x00000004
#define EC_USER_ABORT          0x00000005
#define EC_INCORRECT_ARGUMENTS 0x00000006

typedef enum {
	SBL_SUCCESS         =  0,
	SBL_FILECONTINUE   =   1,	
	SBL_EFAILURE        = -1,
	SBL_ERESCRUNCH      = -2,
	SBL_ELKDN           = -3,
	SBL_ETIMEOUT        = -4,
	SBL_EINVREQUEST     = -5,
	SBL_EDUPLICATEIP    = -6,
	SBL_EENVNOTLISTED   = -7,
	SBL_ETOOLONG        = -8,
	SBL_EINVALID        = -9,
	SBL_ECKSUM	    	= -10,
	SBL_EFILESRC	    = -12,
	SBL_EINVFORMAT	    = -13,
	SBL_EMODNOTSUPPORTED= -14, 
} Status;

#endif /* SBL_ERRNO_H_ */
