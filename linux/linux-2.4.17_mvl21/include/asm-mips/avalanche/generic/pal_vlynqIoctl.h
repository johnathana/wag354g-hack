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

/******************************************************************************
 * FILE PURPOSE:    PAL VLYNQ IOCTL Header File
 ******************************************************************************
 * FILE NAME:       pal_vlynqIoctl.h
 *
 * DESCRIPTION:     PAL VLYNQ IOCTL Header File
 *
 * (C) Copyright 2004, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __PAL_VLYNQ_IOCTL_H__
#define __PAL_VLYNQ_IOCTL_H__


/*
 * The 32 bit IOCTL Command.
 * ------------------------------------------------------------------------
 * |           |          |          |       |          |      |          |
 * | 31:Bit Op | 30: R/W* | 29: Peer | 28-24 | 23-16    | 15-8 | 7-0      |
 * |           |          |          | Resv  | Major id | Resv | Minor id |
 * |           |          |          |       |          |      |          |
 * ------------------------------------------------------------------------
 * 
 * The IOCTL operations have been broadly divided into two categories of o-
 * perations; Bit Op(eration) and non-Bit Op(eration). 
 *
 * Bit Op(eration) means manipulation of certain bits in certain register of 
 * VLYNQ control modules. The register and its contents (bits) are specified
 * by the hardware reference manual. For Bit Op=1, the manjor id are the re-
 * gister id(s) and the minor id(s) are the bits identified within that reg. 
 *
 * Non Bit Op(eration), when Bit Op=0, means raw 32 bits accesses of certain 
 * VLYNQ register or any other operation as specified in the implementation.
 * For raw 32 bit accesses PAL_VLYNQ_IOCTL_REG_CMD is the major command (id) 
 * and reg id(s) refer(s) to minor id(s).
 *
 * R/W*: 0 - Write, 1 - Read.
 * Peer: When, set means that operation has to be carried out on remote or 
 *       peer vlynq.
 */

#define PAL_VLYNQ_IOCTL_BIT_CMD            (1 << 31)
#define PAL_VLYNQ_IOCTL_READ_CMD           (1 << 30)
#define PAL_VLYNQ_IOCTL_REMOTE_CMD         (1 << 29)


#define PAL_VLYNQ_IOCTL_MAJOR_VAL(val)     ((val & 0xff) << 16)
#define PAL_VLYNQ_IOCTL_MAJOR_DE_VAL(cmd)  ((cmd >> 16) & 0xff)

#define PAL_VLYNQ_IOCTL_MINOR_VAL(val)     (val & 0xff)
#define PAL_VLYNQ_IOCTL_MINOR_DE_VAL(cmd)  (cmd & 0xff)

/* Major commads; if bit option is not selected. */
#define PAL_VLYNQ_IOCTL_REG_CMD            (0x00)  /* Vlynq register access. */
#define PAL_VLYNQ_IOCTL_PREP_LINK_DOWN     (0x01)  /* Prepare to teardown the link. */
#define PAL_VLYNQ_IOCTL_PREP_LINK_UP       (0x02)  /* Setup vlynq, now the link is up. */
#define PAL_VLYNQ_IOCTL_CLEAR_INTERN_ERR   (0x03)  /* Clear internal interrupt errors. */

/* Control Register bits. The minor id(s) for control reg. */
#define PAL_VLYNQ_IOCTL_CNT_RESET_CMD      (0x00)
#define PAL_VLYNQ_IOCTL_CNT_ILOOP_CMD      (0x01)
#define PAL_VLYNQ_IOCTL_CNT_AOPT_CMD       (0x02)  /* Write */
#define PAL_VLYNQ_IOCTL_CNT_INT2CFG_CMD    (0x07)
#define PAL_VLYNQ_IOCTL_CNT_INTVEC_CMD     (0x08)
#define PAL_VLYNQ_IOCTL_CNT_INT_EN_CMD     (0x0d)
#define PAL_VLYNQ_IOCTL_CNT_INT_LOC_CMD    (0x0e)
#define PAL_VLYNQ_IOCTL_CNT_CLK_DIR_CMD    (0x0f)  
#define PAL_VLYNQ_IOCTL_CNT_CLK_DIV_CMD    (0x10)  /* Write */
#define PAL_VLYNQ_IOCTL_CNT_CLK_MOD_CMD    (0x15)
#define PAL_VLYNQ_IOCTL_CNT_TX_FAST_CMD    (0x15)
#define PAL_VLYNQ_IOCTL_CNT_RTM_SELECT_CMD (0x16)
#define PAL_VLYNQ_IOCTL_CNT_RTM_SAMPLE_CMD (0x17)
#define PAL_VLYNQ_IOCTL_CNT_CLK_SLKPU_CMD  (0x1e)  /* Write */
#define PAL_VLYNQ_IOCTL_CNT_PMEM_CMD       (0x1f)  /* Write */

/* Status register bits. The minor id(s) for status reg. */
#define PAL_VLYNQ_IOCTL_STS_LINK           (0x00)
#define PAL_VLYNQ_IOCTL_STS_MPEND          (0x01)
#define PAL_VLYNQ_IOCTL_STS_SPEND          (0x02)
#define PAL_VLYNQ_IOCTL_STS_NFEMP0         (0x03)
#define PAL_VLYNQ_IOCTL_STS_NFEMP1         (0x04)
#define PAL_VLYNQ_IOCTL_STS_NFEMP2         (0x05)
#define PAL_VLYNQ_IOCTL_STS_NFEMP3         (0x06)
#define PAL_VLYNQ_IOCTL_STS_LERR           (0x07)
#define PAL_VLYNQ_IOCTL_STS_RERR           (0x08)
#define PAL_VLYNQ_IOCTL_STS_OFLOW          (0x09)
#define PAL_VLYNQ_IOCTL_STS_IFLOW          (0x0A)
#define PAL_VLYNQ_IOCTL_STS_RTM            (0x0B)
#define PAL_VLYNQ_IOCTL_STS_RTM_VAL        (0x0C)
#define PAL_VLYNQ_IOCTL_STS_SWIDOUT        (0x14)
#define PAL_VLYNQ_IOCTL_STS_MODESUP        (0x15)
#define PAL_VLYNQ_IOCTL_STS_SWIDTH         (0x18)
#define PAL_VLYNQ_IOCTL_STS_SWIDIN         (0x18)
#define PAL_VLYNQ_IOCTL_STS_DEBUG          (0x1d)

/* The VLYNQ registers; the reg id(s). 
 *
 * For non Bit operations, they are minor id(s) for the raw 32 bit access 
 * done using the major cmd PAL_VLYNQ_IOCTL_REG_CMD.
 *
 * For bit operations, they are the major id(s) and identify the register
 * whose bits have to be manipulated. 
 * 
 */
#define PAL_VLYNQ_IOCTL_REVSION_REG        (0x00)
#define PAL_VLYNQ_IOCTL_CONTROL_REG        (0x04)
#define PAL_VLYNQ_IOCTL_STATUS_REG         (0x08)
#define PAL_VLYNQ_IOCTL_INT_PRIR_REG       (0x0c)
#define PAL_VLYNQ_IOCTL_INT_STS_REG        (0x10)  
#define PAL_VLYNQ_IOCTL_INT_PEND_REG       (0x14)
#define PAL_VLYNQ_IOCTL_INT_PTR_REG        (0x18)  
#define PAL_VLYNQ_IOCTL_TX_MAP_REG         (0x1c)
#define PAL_VLYNQ_IOCTL_RX1_SZ_REG         (0x20)
#define PAL_VLYNQ_IOCTL_RX1_OFF_REG        (0x24)
#define PAL_VLYNQ_IOCTL_RX2_SZ_REG         (0x28)
#define PAL_VLYNQ_IOCTL_RX2_OFF_REG        (0x3c)
#define PAL_VLYNQ_IOCTL_RX3_SZ_REG         (0x30)
#define PAL_VLYNQ_IOCTL_RX3_OFF_REG        (0x34)
#define PAL_VLYNQ_IOCTL_RX4_SZ_REG         (0x38)
#define PAL_VLYNQ_IOCTL_RX4_OFF_REG        (0x3c)
#define PAL_VLYNQ_IOCTL_CVR_REG            (0x3c)
#define PAL_VLYNQ_IOCTL_AUTO_NEG_REG       (0x40)
#define PAL_VLYNQ_IOCTL_MAN_NEG_REG        (0x44)
#define PAL_VLYNQ_IOCTL_NEG_STS_REG        (0x48) 
#define PAL_VLYNQ_IOCTL_ENDIAN_REG         (0x4c)
#define PAL_VLYNQ_IOCTL_IVR30_REG          (0x5c)
#define PAL_VLYNQ_IOCTL_IVR74_REG          (0x60)

/* Dumping options, they are not part of ioctl options. 
 * But are for the Dump API. */
#define PAL_VLYNQ_DUMP_ALL_ROOT         (0x10000)
#define PAL_VLYNQ_DUMP_RAW_DATA         (0x20000)
#define PAL_VLYNQ_DUMP_ALL_REGS         (0x30000)
#define PAL_VLYNQ_DUMP_STS_REG          (0x00008)
#define PAL_VLYNQ_DUMP_CNTL_REG         (0x00004)

#endif
