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

/** \file   cslr_cpmac.h
    \brief  CPMAC CSL Header - Register Layer Abstraction

    Register layer abstraction of CPMAC peripheral device.

    (C) Copyright 2003, Texas Instruments, Inc

    @author     Anant Gole
    @version    0.1
                (ver 0.2)   Sharath Kumar - Incorporated review comments
    @version    0.2
 */


#ifndef __CSLR_CPMAC_H__
#define __CSLR_CPMAC_H__

#include "csl_defs.h"

/**
 * \brief CPMAC Peripheral Device Register Memory Layout structure
 *
 * The structure instance variable points to CP(G)MAC register space in
 * SOC memory map directly.
 * This is a template only, no memory is ever allocated for this!
 */
typedef struct  
{
    CSL_Reg32   Tx_IdVer;
    CSL_Reg32   Tx_Control;
    CSL_Reg32   Tx_Teardown;
    CSL_Reg32   Reserved1;
    CSL_Reg32   Rx_IdVer;
    CSL_Reg32   Rx_Control;
    CSL_Reg32   Rx_Teardown;
    CSL_Reg32   Reserved2[57];      /* Padding for holes in memory map */
    CSL_Reg32   Rx_MBP_Enable;
    CSL_Reg32   Rx_Unicast_Set;
    CSL_Reg32   Rx_Unicast_Clear;
    CSL_Reg32   Rx_Maxlen;
    CSL_Reg32   Rx_Buffer_Offset;
    CSL_Reg32   Rx_FilterLowThresh;
    CSL_Reg32   Reserved3[2];       /* Padding for holes in memory map */
    CSL_Reg32   Rx_FlowThresh[8];   /* RX FlowThreshold registers 0-7 combined into an array */
    CSL_Reg32   Rx_FreeBuffer[8];   /* RX FreeBuffer registers 0-7 combined into an array */
    CSL_Reg32   MacControl;
    CSL_Reg32   MacStatus;
    CSL_Reg32   EMControl;
    CSL_Reg32   TxFifoControl;      /* CP(G)MAC added register */
    CSL_Reg32   Tx_IntStat_Raw;
    CSL_Reg32   Tx_IntStat_Masked;
    CSL_Reg32   Tx_IntMask_Set;
    CSL_Reg32   Tx_IntMask_Clear;
    CSL_Reg32   Mac_In_Vector;
    CSL_Reg32   Mac_EOI_Vector;
    CSL_Reg32   Mac_Cfig;           /* CP(G)MAC added register */
    CSL_Reg32   Reserved5;          /* Padding for holes in memory map */
    CSL_Reg32   Rx_IntStat_Raw;
    CSL_Reg32   Rx_IntStat_Masked;
    CSL_Reg32   Rx_IntMask_Set;
    CSL_Reg32   Rx_IntMask_Clear;
    CSL_Reg32   Mac_IntStat_Raw;
    CSL_Reg32   Mac_IntStat_Masked;
    CSL_Reg32   Mac_IntMask_Set;
    CSL_Reg32   Mac_IntMask_Clear;
    CSL_Reg32   Reserved51[8];      /* CP(G)MAC modified register memory map */
    CSL_Reg32   MacSrcAddr_Lo;      /* CP(G)MAC modified register */
    CSL_Reg32   MacSrcAddr_Hi;      /* CP(G)MAC modified register */
    CSL_Reg32   MacHash1;
    CSL_Reg32   MacHash2;
    CSL_Reg32   BoffTest;
    CSL_Reg32   Tpace_Test;         /* CP(G)MAC modified register */
    CSL_Reg32   Rx_Pause;
    CSL_Reg32   Tx_Pause;
    CSL_Reg32   Reserved6[4];       /* Padding for holes in memory map */
    CSL_Reg32   RxGoodFrames;
    CSL_Reg32   RxBroadcastFrames;
    CSL_Reg32   RxMulticastFrames;
    CSL_Reg32   RxPauseFrames;
    CSL_Reg32   RxCRCErrors;
    CSL_Reg32   RxAlignCodeErrors;
    CSL_Reg32   RxOversizedFrames;
    CSL_Reg32   RxJabberFrames;
    CSL_Reg32   RxUndersizedFrames;
    CSL_Reg32   RxFragments;
    CSL_Reg32   RxFilteredFrames;
    CSL_Reg32   RxQosFilteredFrames;
    CSL_Reg32   RxOctets;
    CSL_Reg32   TxGoodFrames;
    CSL_Reg32   TxBroadcastFrames;
    CSL_Reg32   TxMulticastFrames;
    CSL_Reg32   TxPauseFrames;
    CSL_Reg32   TxDeferredFrames;
    CSL_Reg32   TxCollisionFrames;
    CSL_Reg32   TxSingleCollFrames;
    CSL_Reg32   TxMultCollFrames;
    CSL_Reg32   TxExcessiveCollisions;
    CSL_Reg32   TxLateCollisions;
    CSL_Reg32   TxUnderrun;
    CSL_Reg32   TxCarrierSenseErrors;
    CSL_Reg32   TxOctets;
    CSL_Reg32   Reg64octetFrames;
    CSL_Reg32   Reg65t127octetFrames;
    CSL_Reg32   Reg128t255octetFrames;
    CSL_Reg32   Reg256t511octetFrames;
    CSL_Reg32   Reg512t1023octetFrames;
    CSL_Reg32   Reg1024tUPoctetFrames;
    CSL_Reg32   NetOctets;
    CSL_Reg32   RxSofOverruns;
    CSL_Reg32   RxMofOverruns;
    CSL_Reg32   RxDmaOverruns;
    CSL_Reg32   Reserved7[28];      /* CP(G)MAC modified register */
    CSL_Reg32   RX_FIFO_Processor_TestAccess[64];
    CSL_Reg32   TX_FIFO_Processor_TestAccess[64];
    CSL_Reg32   MacAddr_Lo;         /* CP(G)MAC added register */
    CSL_Reg32   MacAddr_Hi;         /* CP(G)MAC added register */
    CSL_Reg32   MacIndex;           /* CP(G)MAC added register */
    CSL_Reg32   Reserved8[61];      /* CP(G)MAC modified register */
    CSL_Reg32   Tx_HDP[8];          /* TX HDP registers 0-7 combined into an array */
    CSL_Reg32   Rx_HDP[8];          /* RX HDP registers 0-7 combined into an array */
    CSL_Reg32   Tx_CP[8];           /* TX Completion Pointer registers 0-7 combined into an array */
    CSL_Reg32   Rx_CP[8];           /* RX Completion Pointer registers 0-7 combined into an array */
    CSL_Reg32   Reserved9[64];      /* Padding for holes in memory map */
    CSL_Reg32   Stateram_Test_Access[32];
    CSL_Reg32   Reserved10[32];     /* Padding for holes in memory map */
} CSL_CpmacRegs;

/*
 * \brief CPMAC register overlay pointer
 *
 * Can be used in DDC layer directly for performance considersations.
 */
typedef volatile CSL_CpmacRegs *CSL_CpmacRegsOvly;

/**
 *  \brief CPMAC Peripheral Device Register Enumerations
 *
 *  Register Enumarations for CP(G)MAC peripheral
 */
typedef enum  
{
    Tx_IdVer = 0,
    Tx_Control,
    Tx_Teardown,
    Rx_IdVer = 4,
    Rx_Control,
    Rx_Teardown,
    Rx_MBP_Enable = 64,
    Rx_Unicast_Set,
    Rx_Unicast_Clear,
    Rx_Maxlen,
    Rx_Buffer_Offset,
    Rx_FilterLowThresh,
    Rx0_FlowThresh = 72,
    Rx1_FlowThresh,
    Rx2_FlowThresh,
    Rx3_FlowThresh,
    Rx4_FlowThresh,
    Rx5_FlowThresh,
    Rx6_FlowThresh,
    Rx7_FlowThresh,
    Rx0_FreeBuffer,
    Rx1_FreeBuffer,
    Rx2_FreeBuffer,
    Rx3_FreeBuffer,
    Rx4_FreeBuffer,
    Rx5_FreeBuffer,
    Rx6_FreeBuffer,
    Rx7_FreeBuffer,
    MacControl,
    MacStatus,
    EMControl,
    TxFifoControl,
    Tx_IntStat_Raw,
    Tx_IntStat_Masked,
    Tx_IntMask_Set,
    Tx_IntMask_Clear,
    Mac_In_Vector,
    Mac_EOI_Vector,
    Mac_Cfig,
    Rx_IntStat_Raw = 100,
    Rx_IntStat_Masked,
    Rx_IntMask_Set,
    Rx_IntMask_Clear,
    Mac_IntStat_Raw,
    Mac_IntStat_Masked,
    Mac_IntMask_Set,
    Mac_IntMask_Clear,
    MacSrcAddr_Lo = 116,
    MacSrcAddr_Hi,
    MacHash1,
    MacHash2,
    BoffTest,
    Tpace_Test,
    RxPause,
    TxPause,
    RxGoodFrames = 128,
    RxBroadcastFrames,
    RxMulticastFrames,
    RxPauseFrames,
    RxCRCErrors,
    RxAlignCodeErrors,
    RxOversizedFrames,
    RxJabberFrames,
    RxUndersizedFrames,
    RxFragments,
    RxFilteredFrames,
    RxQosFilteredFrames,
    RxOctets,
    TxGoodFrames,
    TxBroadcastFrames,
    TxMulticastFrames,
    TxPauseFrames,
    TxDeferredFrames,
    TxCollisionFrames,
    TxSingleCollFrames,
    TxMultCollFrames,
    TxExcessiveCollisions,
    TxLateCollisions,
    TxUnderrun,
    TxCarrierSenseErrors,
    TxOctets,
    Reg64octetFrames,
    Reg65t127octetFrames,
    Reg128t255octetFrames,
    Reg256t511octetFrames,
    Reg512t1023octetFrames,
    Reg1024tUPoctetFrames,
    NetOctets,
    RxSofOverruns,
    RxMofOverruns,
    RxDmaOverruns,
    RX_FIFO_Processor_TestAccess = 192,     /* First word of RX FIFO */
    TX_FIFO_Processor_TestAccess = 256,     /* First word of TX FIFO */
    MacAddr_Lo = 320,
    MacAddr_Hi,
    MacIndex,
    Tx0_HDP = 384,
    Tx1_HDP,
    Tx2_HDP,
    Tx3_HDP,
    Tx4_HDP,
    Tx5_HDP,
    Tx6_HDP,
    Tx7_HDP,
    Rx0_HDP,
    Rx1_HDP,
    Rx2_HDP,
    Rx3_HDP,
    Rx4_HDP,
    Rx5_HDP,
    Rx6_HDP,
    Rx7_HDP,
    Tx0_CP,
    Tx1_CP,
    Tx2_CP,
    Tx3_CP,
    Tx4_CP,
    Tx5_CP,
    Tx6_CP,
    Tx7_CP,
    Rx0_CP,
    Rx1_CP,
    Rx2_CP,
    Rx3_CP,
    Rx4_CP,
    Rx5_CP,
    Rx6_CP,
    Rx7_CP,
    Stateram_Test_Access = 448     /* First word of State RAM */
} CSL_CpmacRegIds;

/**
 *  \brief CPMAC Addressing Type
 *
 *  Addressing type based upon cfig register. For CPMAC peripheral cfig register reads a value of 0
 *  i.e Type 0 addressing
 */
typedef enum
{   
    RX_ADDR_TYPE0 = 0,      /**< Type 0 addressing - old style used in (CPMAC) */
    RX_ADDR_TYPE1 = 1,      /**< Type 1 addressing - new CPGMAC style */
    RX_ADDR_TYPE2 = 2,      /**< Type 2 addressing - new CPGMAC "filtering" style */
    RX_ADDR_TYPE3 = 3       /**< TODO: Type 3 addressing  - new CPGMAC "filtering" style */
} CpmacRxAddrType;
    

/* 
 * The following are CPMAC registers which have been removed from the CPGMAC
 * register map. Thus we access them using macros to avoid having more CSL register
 * overlay structures for older CPMAC register map.
 */

/* Make sure REG32_DATA is not defined before */
#undef REG32_DATA

#define REG32_DATA(addr)                        (*(volatile Uint32 *)(addr))
#define CPMAC_MACADDRLO(baseAddr, chNum)        REG32_DATA((baseAddr) + 0x1B0 + ((chNum) * 4))
#define CPMAC_MACADDRMID(baseAddr)              REG32_DATA((baseAddr) + 0x1D0)
#define CPMAC_MACADDRHI(baseAddr)               REG32_DATA((baseAddr) + 0x1D4)

/* Statistics clear value */
#define CPMAC_NUM_STAT_REGS                     36
#define CPMAC_STAT_CLEAR                        0xFFFFFFFF

/* CPMAC All multicast set register value */
#define CPMAC_ALL_MULTI_REG_VALUE               0xFFFFFFFF

/* CPMAC number of Multicast bits that can be set/cleared - currently 64 bits - hash1/2 regs */
#define CPMAC_NUM_MULTICAST_BITS                64

/* CPMAC Teardown Value */
#define CPMAC_TEARDOWN_VALUE                    0xFFFFFFFC

/* TX / RX Control bits */
#define CPMAC_TX_CONTROL_TX_ENABLE_VAL          0x1
#define CPMAC_RX_CONTROL_RX_ENABLE_VAL          0x1

/* Host interrupt bits */
#define CPMAC_MAC_HOST_ERR_INTMASK_VAL          0x2
#define CPMAC_MAC_STAT_INT_INTMASK_VAL          0x1

/* Rx config masks */
#define CPMAC_RX_UNICAST_CLEAR_ALL              0xFF

/* Type 0 Address filtering Macros */
#define CSL_CPMAC_TYPE_0_MACSRCADDR0_MASK                    (0xFF)
#define CSL_CPMAC_TYPE_0_MACSRCADDR0_SHIFT                   0
#define CSL_CPMAC_TYPE_0_MACSRCADDR1_MASK                    (0xFF)
#define CSL_CPMAC_TYPE_0_MACSRCADDR1_SHIFT                   0

#define CSL_CPMAC_TYPE_0_MACSRCADDR2_MASK                    (0xFF<<24)
#define CSL_CPMAC_TYPE_0_MACSRCADDR2_SHIFT                   24
#define CSL_CPMAC_TYPE_0_MACSRCADDR3_MASK                    (0xFF<<16)
#define CSL_CPMAC_TYPE_0_MACSRCADDR3_SHIFT                   16
#define CSL_CPMAC_TYPE_0_MACSRCADDR4_MASK                    (0xFF<<8)
#define CSL_CPMAC_TYPE_0_MACSRCADDR4_SHIFT                   8
#define CSL_CPMAC_TYPE_0_MACSRCADDR5_MASK                    (0xFF)
#define CSL_CPMAC_TYPE_0_MACSRCADDR5_SHIFT                   0

/* Type 1 Address filtering Macros */
#define CSL_CPMAC_TYPE_1_MACSRCADDR0_MASK                    (0xFF<<8)
#define CSL_CPMAC_TYPE_1_MACSRCADDR0_SHIFT                   8
#define CSL_CPMAC_TYPE_1_MACSRCADDR1_MASK                    (0xFF)
#define CSL_CPMAC_TYPE_1_MACSRCADDR1_SHIFT                   0

#define CSL_CPMAC_TYPE_1_MACSRCADDR2_MASK                    (0xFF<<24)
#define CSL_CPMAC_TYPE_1_MACSRCADDR2_SHIFT                   24
#define CSL_CPMAC_TYPE_1_MACSRCADDR3_MASK                    (0xFF<<16)
#define CSL_CPMAC_TYPE_1_MACSRCADDR3_SHIFT                   16
#define CSL_CPMAC_TYPE_1_MACSRCADDR4_MASK                    (0xFF<<8)
#define CSL_CPMAC_TYPE_1_MACSRCADDR4_SHIFT                   8
#define CSL_CPMAC_TYPE_1_MACSRCADDR5_MASK                    (0xFF)
#define CSL_CPMAC_TYPE_1_MACSRCADDR5_SHIFT                   0

/* CP(G)MAC address filtering bit macros */
#define CSL_CPGMAC_VALID_MASK                                   (0x1<<20)
#define CSL_CPGMAC_VALID_SHIFT                                  20
#define CSL_CPGMAC_MATCH_FILTER_MASK                            (0x1<<19)
#define CSL_CPGMAC_MATCH_FILTER_SHIFT                           19
#define CSL_CPGMAC_CHANNEL_MASK                                 (0x7<<16)
#define CSL_CPGMAC_CHANNEL_SHIFT                                16
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR0_MASK                    (0xFF<<8)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR0_SHIFT                   8
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR1_MASK                    (0xFF)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR1_SHIFT                   0

#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR2_MASK                    (0xFF<<24)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR2_SHIFT                   24
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR3_MASK                    (0xFF<<16)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR3_SHIFT                   16
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR4_MASK                    (0xFF<<8)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR4_SHIFT                   8
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR5_MASK                    (0xFF)
#define CSL_CPGMAC_TYPE_2_3_MACSRCADDR5_SHIFT                   0


/* RX MBP register bit positions */
#define CSL_CPMAC_RXMBP_PASSCRC_SHIFT               30
#define CSL_CPMAC_RXMBP_PASSCRC_MASK                (0x1 << 30)
#define CSL_CPMAC_RXMBP_QOSEN_SHIFT                 29
#define CSL_CPMAC_RXMBP_QOSEN_MASK                  (0x1 << 29)
#define CSL_CPMAC_RXMBP_NOCHAIN_SHIFT               28
#define CSL_CPMAC_RXMBP_NOCHAIN_MASK                (0x1 << 28)
#define CSL_CPMAC_RXMBP_CMFEN_SHIFT                 24
#define CSL_CPMAC_RXMBP_CMFEN_MASK                  (0x1 << 24)
#define CSL_CPMAC_RXMBP_CSFEN_SHIFT                 23
#define CSL_CPMAC_RXMBP_CSFEN_MASK                  (0x1 << 23)
#define CSL_CPMAC_RXMBP_CEFEN_SHIFT                 22
#define CSL_CPMAC_RXMBP_CEFEN_MASK                  (0x1 << 22)
#define CSL_CPMAC_RXMBP_CAFEN_SHIFT                 21
#define CSL_CPMAC_RXMBP_CAFEN_MASK                  (0x1 << 21)
#define CSL_CPMAC_RXMBP_PROMCH_SHIFT                16
#define CSL_CPMAC_RXMBP_PROMCH_MASK                 (0x7 << 16)
#define CSL_CPMAC_RXMBP_BROADEN_SHIFT               13
#define CSL_CPMAC_RXMBP_BROADEN_MASK                (0x1 << 13)
#define CSL_CPMAC_RXMBP_BROADCH_SHIFT               8
#define CSL_CPMAC_RXMBP_BROADCH_MASK                (0x7 << 8)
#define CSL_CPMAC_RXMBP_MULTIEN_SHIFT               5
#define CSL_CPMAC_RXMBP_MULTIEN_MASK                (0x1 << 5)
#define CSL_CPMAC_RXMBP_MULTICH_SHIFT               0
#define CSL_CPMAC_RXMBP_MULTICH_MASK                0x7

/* Mac Control register bit fields */
#define CSL_CPMAC_MACCONTROL_TXSHORTGAPEN_SHIFT     10
#define CSL_CPMAC_MACCONTROL_TXSHORTGAPEN_MASK      (0x1 << 10)
#define CSL_CPMAC_MACCONTROL_TXPTYPE_SHIFT          9
#define CSL_CPMAC_MACCONTROL_TXPTYPE_MASK           (0x1 << 9)
#define CSL_CPMAC_MACCONTROL_GIGABITEN_SHIFT        7
#define CSL_CPMAC_MACCONTROL_GIGABITEN_MASK         (0x1 << 7)
#define CSL_CPMAC_MACCONTROL_TXPACEEN_SHIFT         6
#define CSL_CPMAC_MACCONTROL_TXPACEEN_MASK          (0x1 << 6)
#define CSL_CPMAC_MACCONTROL_MIIEN_SHIFT            5
#define CSL_CPMAC_MACCONTROL_MIIEN_MASK             (0x1 << 5)
#define CSL_CPMAC_MACCONTROL_TXFLOWEN_SHIFT         4
#define CSL_CPMAC_MACCONTROL_TXFLOWEN_MASK          (0x1 << 4)
#define CSL_CPMAC_MACCONTROL_RXFLOWEN_SHIFT         3
#define CSL_CPMAC_MACCONTROL_RXFLOWEN_MASK          (0x1 << 3)
#define CSL_CPMAC_MACCONTROL_LOOPBKEN_SHIFT         1
#define CSL_CPMAC_MACCONTROL_LOOPBKEN_MASK          (0x1 << 1)
#define CSL_CPMAC_MACCONTROL_FULLDUPLEXEN_SHIFT     0
#define CSL_CPMAC_MACCONTROL_FULLDUPLEXEN_MASK      (0x1)

/* MacStatus register */
#define CSL_CPMAC_MACSTATUS_TXERRCODE_MASK          0xF00000
#define CSL_CPMAC_MACSTATUS_TXERRCODE_SHIFT         20
#define CSL_CPMAC_MACSTATUS_TXERRCH_MASK            0x7
#define CSL_CPMAC_MACSTATUS_TXERRCH_SHIFT           16
#define CSL_CPMAC_MACSTATUS_RXERRCODE_MASK          0xF000
#define CSL_CPMAC_MACSTATUS_RXERRCODE_SHIFT         12
#define CSL_CPMAC_MACSTATUS_RXERRCH_MASK            0x7
#define CSL_CPMAC_MACSTATUS_RXERRCH_SHIFT           8

/* CPMAC RX Max packet length mask */
#define CSL_CPMAC_RX_MAX_LEN_SHIFT              0
#define CSL_CPMAC_RX_MAX_LEN_MASK               0xFFFF

/* CPMAC RX Max packet length mask */
#define CSL_CPMAC_RX_BUFFER_OFFSET_SHIFT        0
#define CSL_CPMAC_RX_BUFFER_OFFSET_MASK         0xFFFF

/* MAC_IN_VECTOR (0x180) register bit fields */
#define CPMAC_MAC_IN_VECTOR_STATUS_INT          (1 << 19)
#define CPMAC_MAC_IN_VECTOR_HOST_INT            (1 << 18)
#define CPMAC_MAC_IN_VECTOR_RX_INT_OR           (1 << 17)
#define CPMAC_MAC_IN_VECTOR_TX_INT_OR           (1 << 16)
#define CPMAC_MAC_IN_VECTOR_RX_INT_VEC          (7 << 8)
#define CPMAC_MAC_IN_VECTOR_TX_INT_VEC          (7)

/* CPPI bit positions */
#define CPMAC_CPPI_SOP_BIT                      0x80000000  /*(1 << 31)*/
#define CPMAC_CPPI_EOP_BIT                      0x40000000  /*(1 << 30*/
#define CPMAC_CPPI_OWNERSHIP_BIT                0x20000000  /*(1 << 29)*/
#define CPMAC_CPPI_EOQ_BIT                      0x10000000  /*(1 << 28)*/
#define CPMAC_CPPI_TEARDOWN_COMPLETE_BIT        0x8000000   /*(1 << 27)*/
#define CPMAC_CPPI_PASS_CRC_BIT                 0x4000000   /*(1 << 26)*/


#endif /* __CSLR_CPMAC_H__ */

