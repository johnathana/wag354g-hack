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
**|     Copyright (c) 2003 Texas Instruments Incorporated                |**
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

/*********************************************************************************
 *  ------------------------------------------------------------------------------
 *   Module      : vlynq_hal.h
 *   Description :
 *   This header file provides the set of functions exported by the 
 *   VLYNQ HAL. This file is included from the SOC specific VLYNQ driver wrapper.
 *  ------------------------------------------------------------------------------
 *********************************************************************************/

#ifndef _VLYNQ_HAL_H_
#define _VLYNQ_HAL_H_

#include "haltypes.h"
#include "vlynq_hal_params.h"

#ifndef PRIVATE 
#define PRIVATE static
#endif

#ifndef GLOBAL
#define GLOBAL
#endif

/* Enable/Disable debug feature */
#undef VLYNQ_DEBUG 

#ifdef VLYNQ_DEBUG  /* This needs to be OS abstracted - for testing use vxworks/linux calls */
#define debugPrint(format,args...)    
#else 
#define debugPrint(format,args...)  
#endif

/* Error defines */
#define VLYNQ_SUCCESS               0

#define VLYNQ_ERRCODE_BASE          0 /* Chosen by system */
#define VLYNQ_INVALID_ARG          -(VLYNQ_ERRCODE_BASE+1)
#define VLYNQ_INVALID_DRV_STATE    -(VLYNQ_ERRCODE_BASE+2)
#define VLYNQ_INT_CONFIG_ERR       -(VLYNQ_ERRCODE_BASE+3)
#define VLYNQ_LINK_DOWN            -(VLYNQ_ERRCODE_BASE+4)
#define VLYNQ_MEMALLOC_FAIL        -(VLYNQ_ERRCODE_BASE+5)
#define VLYNQ_ISR_NON_EXISTENT     -(VLYNQ_ERRCODE_BASE+6)
#define VLYNQ_INTVEC_MAP_NOT_FOUND -(VLYNQ_ERRCODE_BASE+7)

/* Vlynq Defines and Macros */

#define VLYNQ_NUM_INT_BITS              32 /* 32 bit interrupt staus register */

/* Base address of module */
#define VLYNQ_BASE                      (pdev->module_base)

#define VLYNQ_REMOTE_REGS_OFFSET        0x0080

#define VLYNQ_REV_OFFSET                0x0000
#define VLYNQ_CTRL_OFFSET               0x0004
#define VLYNQ_STATUS_OFFSET             0x0008
#define VLYNQ_INT_STAT_OFFSET           0x0010
#define VLYNQ_INT_PEND_OFFSET           0x0014
#define VLYNQ_INT_PTR_OFFSET            0x0018
#define VLYNQ_TXMAP_OFFSET              0x001c

#define VLYNQ_RX0MAP_SIZE_REG_OFFSET    0x0020
#define VLYNQ_RX0MAP_OFFSET_REG_OFFSET  0x0024

#define VLYNQ_CHIP_VER_OFFSET           0x0040
#define VLYNQ_IVR_REGS_OFFSET           0x0060

#define VLYNQ_INT_PENDING_REG_PTR       0x14
#define VLYNQ_R_INT_PENDING_REG_PTR     VLYNQ_REMOTE_REGS_OFFSET + 0x14

#define VLYNQ_REV_REG       *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_REV_OFFSET))
#define VLYNQ_CTRL_REG      *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_CTRL_OFFSET))
#define VLYNQ_STATUS_REG    *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_STATUS_OFFSET))
#define VLYNQ_INT_STAT_REG  *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_INT_STAT_OFFSET))
#define VLYNQ_INT_PEND_REG  *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_INT_PEND_OFFSET))
#define VLYNQ_INT_PTR_REG   *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_INT_PTR_OFFSET))
#define VLYNQ_TXMAP_REG     *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_TXMAP_OFFSET))

/** map takes on values between 1 to VLYNQ_MAX_MEMORY_REGIONS **/
#define VLYNQ_RXMAP_SIZE_REG(map) \
    *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_RX0MAP_SIZE_REG_OFFSET+( (map-1)<<3)))
    
/** map takes on values between 1 to VLYNQ_MAX_MEMORY_REGIONS **/
#define VLYNQ_RXMAP_OFFSET_REG(map) \
    *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_RX0MAP_OFFSET_REG_OFFSET+( (map-1)<<3)))

#define VLYNQ_CHIP_VER_REG  *((volatile UINT32 *)(VLYNQ_BASE+VLYNQ_CHIP_VER_OFFSET))

/* 0 =< ivr <= 31; currently ivr < VLYNQ_IVR_MAXIVR=8) */
#define VLYNQ_IVR_OFFSET(ivr)  \
    (VLYNQ_BASE + VLYNQ_IVR_REGS_OFFSET +((((unsigned)(ivr)) & 31) & ~3) )

#define VLYNQ_IVR_03TO00_REG  *((volatile UINT32*) (VLYNQ_IVR_OFFSET(0)) )
#define VLYNQ_IVR_07TO04_REG  *((volatile UINT32*) (VLYNQ_IVR_OFFSET(4)) )
/*** Can be extended for 11TO08...31TO28 when all 31 are supported**/

#define VLYNQ_IVR_INTEN(ivr)    (((UINT32)(0x80)) << ((((unsigned)(ivr)) % 4) * 8))
#define VLYNQ_IVR_INTTYPE(ivr)  (((UINT32)(0x40)) << ((((unsigned)(ivr)) % 4) * 8))
#define VLYNQ_IVR_INTPOL(ivr)   (((UINT32)(0x20)) << ((((unsigned)(ivr)) % 4) * 8))
#define VLYNQ_IVR_INTVEC(ivr)   (((UINT32)(0x1F)) << ((((unsigned)(ivr)) % 4) * 8))
#define VLYNQ_IVR_INTALL(ivr)   (((UINT32)(0xFF)) << ((((unsigned)(ivr)) % 4) * 8))



/*********************************
 * Remote VLYNQ register set     *
 *********************************/

#define VLYNQ_R_REV_OFFSET              0x0080
#define VLYNQ_R_CTRL_OFFSET             0x0084
#define VLYNQ_R_STATUS_OFFSET           0x0088
#define VLYNQ_R_INT_STAT_OFFSET         0x0090
#define VLYNQ_R_INT_PEND_OFFSET         0x0094
#define VLYNQ_R_INT_PTR_OFFSET          0x0098
#define VLYNQ_R_TXMAP_OFFSET            0x009c

#define VLYNQ_R_RX0MAP_SIZE_REG_OFFSET  0x00A0
#define VLYNQ_R_RX0MAP_OFFSET_REG_OFFSET 0x00A4

#define VLYNQ_R_CHIP_VER_OFFSET         0x00C0
#define VLYNQ_R_IVR_REGS_OFFSET         0x00E0

#define VLYNQ_R_REV_REG       *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_REV_OFFSET)) 
#define VLYNQ_R_CTRL_REG      *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_CTRL_OFFSET))
#define VLYNQ_R_STATUS_REG    *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_STATUS_OFFSET))
#define VLYNQ_R_INT_STAT_REG  *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_INT_STAT_OFFSET))
#define VLYNQ_R_INT_PEND_REG  *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_INT_PEND_OFFSET))
#define VLYNQ_R_INT_PTR_REG   *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_INT_PTR_OFFSET))
#define VLYNQ_R_TXMAP_REG     *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_TXMAP_OFFSET))

/** map takes on values between 1 to VLYNQ_MAX_MEMORY_REGIONS **/
#define VLYNQ_R_RXMAP_SIZE_REG(map) \
    *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_RX0MAP_SIZE_REG_OFFSET + ((map-1)<<3)))
    
/** map takes on values between 1 to VLYNQ_MAX_MEMORY_REGIONS **/
#define VLYNQ_R_RXMAP_OFFSET_REG(map) \
    *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_RX0MAP_OFFSET_REG_OFFSET + ((map-1)<<3)))

#define VLYNQ_R_CHIP_VER_REG  *((volatile UINT32 *)(VLYNQ_BASE + VLYNQ_R_CHIP_VER_OFFSET)

#define VLYNQ_R_IVR_OFFSET(ivr)  \
    (VLYNQ_BASE + VLYNQ_R_IVR_REGS_OFFSET +((((unsigned)(ivr)) & 31) & ~3))
 

/*** Can be extended for 11TO08...31TO28 when all 31 are supported**/
#define VLYNQ_R_IVR_03TO00_REG  *((volatile UINT32*) (VLYNQ_R_IVR_OFFSET(0)) )
#define VLYNQ_R_IVR_07TO04_REG  *((volatile UINT32*) (VLYNQ_R_IVR_OFFSET(4)) )


/****End of remote register set definition******/


/*** Masks for individual register fields ***/

#define VLYNQ_MODULE_ID_MASK        0xffff0000
#define VLYNQ_MAJOR_REV_MASK        0x0000ff00
#define VLYNQ_MINOR_REV_MASK        0x000000ff

    
#define VLYNQ_CTL_ILOOP_MASK        0x00000002
#define VLYNQ_CTL_INT2CFG_MASK      0x00000080
#define VLYNQ_CTL_INTVEC_MASK       0x00001f00
#define VLYNQ_CTL_INTEN_MASK        0x00002000
#define VLYNQ_CTL_INTLOCAL_MASK     0x00004000
#define VLYNQ_CTL_CLKDIR_MASK       0x00008000
#define VLYNQ_CTL_CLKDIV_MASK       0x00070000
#define VLYNQ_CTL_MODE_MASK         0x00e00000


#define VLYNQ_STS_LINK_MASK         0x00000001  /* Link is active */
#define VLYNQ_STS_MPEND_MASK        0x00000002  /* Pending master requests */
#define VLYNQ_STS_SPEND_MASK        0x00000004  /* Pending slave requests */
#define VLYNQ_STS_NFEMPTY0_MASK     0x00000008  /* Master data FIFO not empty */
#define VLYNQ_STS_NFEMPTY1_MASK     0x00000010  /* Master command FIFO not empty */
#define VLYNQ_STS_NFEMPTY2_MASK     0x00000020  /* Slave data FIFO not empty */
#define VLYNQ_STS_NFEMPTY3_MASK     0x00000040  /* Slave command FIFO not empty */
#define VLYNQ_STS_LERROR_MASK       0x00000080  /* Local error, w/c */
#define VLYNQ_STS_RERROR_MASK       0x00000100  /* remote error w/c */
#define VLYNQ_STS_OFLOW_MASK        0x00000200
#define VLYNQ_STS_IFLOW_MASK        0x00000400
#define VLYNQ_STS_MODESUP_MASK      0x00E00000  /* Highest mode supported */
#define VLYNQ_STS_SWIDTH_MASK       0x07000000  /* Used for reading the width of VLYNQ bus */
#define VLYNQ_STS_DEBUG_MASK        0xE0000000 

#define VLYNQ_CTL_INTVEC_SHIFT      0x08
#define VLYNQ_CTL_INTEN_SHIFT       0x0D
#define VLYNQ_CTL_INT2CFG_SHIFT     0x07
#define VLYNQ_CTL_INTLOCAL_SHIFT    0x0E

#define VLYNQ_CTL_INTFIELDS_CLEAR_MASK  0x7F80

#define VLYNQ_CHIPVER_DEVREV_MASK   0xffff0000
#define VLYNQ_CHIPVER_DEVID_MASK    0x0000ffff

#define VLYNQ_IVR_INTEN_MASK        0x80
#define VLYNQ_IVR_INTTYPE_MASK      0x40
#define VLYNQ_IVR_INTPOL_MASK       0x20


/**** Helper macros ****/

#define VLYNQ_RESETCB(arg) \
   if( pdev->reset_cb != NULL)   \
   {                             \
      (pdev->reset_cb)(pdev, (arg));  \
   }
    
#define VLYNQ_STATUS_FLD_WIDTH(sts) (((sts) & VLYNQ_STS_SWIDTH_MASK) >> 24 )
#define VLYNQ_CTL_INTVEC(x)         (((x) & 31) << 8 )

#define VLYNQ_INRANGE(x,hi,lo)      (((x) <= (hi)) && ((x) >= (lo)))
#define VLYNQ_OUTRANGE(x,hi,lo)     (((x) > (hi)) || ((x) < (lo)))

#define VLYNQ_ALIGN4(x)             (x)=(x)&(~3)   


/*************************************
 *             Enums                 *
 *************************************/

/* Initialization options define what operations are
 * undertaken during vlynq module initialization */
typedef enum
{
    /* Init host local memory regions.This allows
     * local host access remote memory regions */
    VLYNQ_INIT_LOCAL_MEM_REGIONS = 0x01,
    /* Init host remote memory regions.This allows
     * remote device access local memory regions */
    VLYNQ_INIT_REMOTE_MEM_REGIONS =0x02,
    /* Init local interrupt config*/
    VLYNQ_INIT_LOCAL_INTERRUPTS   =0x04,
    /* Init remote interrupt config*/
    VLYNQ_INIT_REMOTE_INTERRUPTS  =0x08,
    /* Check link during initialization*/
    VLYNQ_INIT_CHECK_LINK         =0x10,
    /* configure clock during init */
    VLYNQ_INIT_CONFIG_CLOCK       =0x20,
    /* Clear errors during init */    
    VLYNQ_INIT_CLEAR_ERRORS       =0x40,
    /* All options */
    VLYNQ_INIT_PERFORM_ALL        =0x7F
}VLYNQ_INIT_OPTIONS;


/* VLYNQ_DEV_TYPE identifies local or remote device */
typedef enum
{
    VLYNQ_LOCAL_DVC  = 0,           /* vlynq local device (SOC's vlynq module) */
    VLYNQ_REMOTE_DVC = 1            /* vlynq remote device (remote vlynq module) */
}VLYNQ_DEV_TYPE;


/* VLYNQ_CLK_SOURCE identifies the vlynq module clock source */
typedef enum
{
    VLYNQ_CLK_SOURCE_NONE   = 0,    /* do not initialize clock generator*/
    VLYNQ_CLK_SOURCE_LOCAL  = 1,    /* clock is generated by local machine  */
    VLYNQ_CLK_SOURCE_REMOTE = 2     /* clock is generated by remote machine */
}VLYNQ_CLK_SOURCE;


/* VLYNQ_DRV_STATE indicates the current driver state */
typedef enum
{
    VLYNQ_DRV_STATE_UNINIT = 0,     /* driver is uninitialized  */
    VLYNQ_DRV_STATE_ININIT = 1,     /* VLYNQ is being initialized */
    VLYNQ_DRV_STATE_RUN    = 2,     /* VLYNQ is running properly  */
    VLYNQ_DRV_STATE_HOLD   = 3,     /* driver stopped temporarily */
    VLYNQ_DRV_STATE_ERROR  = 4      /* driver stopped on unrecoverable error */
}VLYNQ_DRV_STATE;


/* VLYNQ_BUS_WIDTH identifies the vlynq module bus width */
typedef enum
{
   VLYNQ_BUS_WIDTH_3 =  3,
   VLYNQ_BUS_WIDTH_5 =  5,
   VLYNQ_BUS_WIDTH_7 =  7,
   VLYNQ_BUS_WIDTH_9 =  9
}VLYNQ_BUS_WIDTH;


/* VLYNQ_LOCAL_INT_CONFIG indicates whether the local vlynq 
 * interrupts are processed by the host or passed on to the 
 * remote device.
 */
typedef enum
{
    VLYNQ_INT_REMOTE = 0,   /* Interrupt packets sent to remote, intlocal=0 */
    VLYNQ_INT_LOCAL  = 1    /* Interrupts are handled locally, intlocal=1 */
}VLYNQ_LOCAL_INT_CONFIG;        


/* VLYNQ_REMOTE_INT_CONFIG indicates whether the remote 
 * interrupts are to be handled by the SOC system ISR 
 * or via the vlynq root ISR
 */
typedef enum 
{
    VLYNQ_INT_ROOT_ISR   = 0,   /* remote ints handled via vlynq root ISR */
    VLYNQ_INT_SYSTEM_ISR = 1    /* remote ints handled via system ISR */
}VLYNQ_REMOTE_INT_CONFIG;


/* VLYNQ_INTR_POLARITY - vlynq interrupt polarity setting */
typedef enum
{
    VLYNQ_INTR_ACTIVE_HIGH = 0,
    VLYNQ_INTR_ACTIVE_LOW  = 1
}VLYNQ_INTR_POLARITY;


/* VLYNQ_INTR_TYPE  - vlynq interrupt type */
typedef enum
{
    VLYNQ_INTR_LEVEL  = 0,
    VLYNQ_INTR_PULSED = 1
}VLYNQ_INTR_TYPE;


/* VLYNQ_RESET_MODE - vlynq reset mode */
typedef enum
{
   VLYNQ_RESET_ASSERT,      /* hold device in reset state */
   VLYNQ_RESET_DEASSERT,    /* release device from reset state */
   VLYNQ_RESET_INITFAIL,    /* handle the device in case driver initialization fails */
   VLYNQ_RESET_LINKESTABLISH,  /* handle the device in case driver established link */
   VLYNQ_RESET_INITFAIL2,   /* Driver initialization failed but VLYNQ link exist. */
   VLYNQ_RESET_INITOK       /* Driver initialization finished OK. */
}VLYNQ_RESET_MODE;
 


/*************************************
 *             Typedefs              *
 *************************************/

struct VLYNQ_DEV_t; /*forward declaration*/

/*--------Function Pointers defintions -----------*/

/* prototype for interrupt handler definition */
typedef void (*VLYNQ_INTR_CNTRL_ISR)(void *arg1,void *arg2,void *arg3);

typedef void 
(*VLYNQ_RESET_REMOTE)(struct VLYNQ_DEV_t *pDev, VLYNQ_RESET_MODE mode);

typedef void 
(*VLYNQ_REPORT_CB)( struct VLYNQ_DEV_t *pDev,   /* This VLYNQ */
                    VLYNQ_DEV_TYPE  aSrcDvc,    /* Event Cause -local/remote? */
                    UINT32  dwStatRegVal);      /* Value of the relevant status register */


/*-------Structure Definitions------------*/

typedef struct VLYNQ_MEMORY_MAP_t
{
    UINT32 Txmap;
    UINT32 RxOffset[VLYNQ_MAX_MEMORY_REGIONS];
    UINT32 RxSize[VLYNQ_MAX_MEMORY_REGIONS];
}VLYNQ_MEMORY_MAP;


/**VLYNQ_INTERRUPT_CNTRL - defines the vlynq module interrupt
 * settings in vlynq Control register  */ 
typedef struct VLYNQ_INTERRUPT_CNTRL_t
{
    /* vlynq interrupts handled by host or remote - maps to 
     * intLocal bit in vlynq control register */
    VLYNQ_LOCAL_INT_CONFIG intLocal;

    /* remote interrupts handled by vlynq isr or host system
     * interrupt controller - maps to the int2Cfg in vlynq 
     * control register */
    VLYNQ_REMOTE_INT_CONFIG intRemote;
    
    /* bit in pending/set register used for module interrupts*/
    UINT32 map_vector;
    
    /* used only if remote interrupts are to be handled by system ISR*/    
    UINT32 intr_ptr;

}VLYNQ_INTERRUPT_CNTRL;


/* VLYNQ_INTR_CNTRL_ICB - defines the Interrupt control block which hold
 * the interrupt dispatch table. The vlynq_root_isr() indexes into this 
 * table to identify the ISR to be invoked
 */
typedef struct VLYNQ_INTR_CNTRL_ICB_t
{
    VLYNQ_INTR_CNTRL_ISR            isr;    /* Clear errors during initialization */
    void                            *arg1 ; /* Arg 1 for the ISR */
    void                            *arg2 ; /* Arg 2 for the ISR */
    void                            *arg3 ; /* Arg 3 for the ISR */
    UINT32  isrCount; /* number of ISR invocations so far */
    struct VLYNQ_INTR_CNTRL_ICB_t   *next;
}VLYNQ_INTR_CNTRL_ICB;

/* overlay of vlynq register set */
typedef struct VLYNQ_REG_SET_t
{
    UINT32 revision; /*offset : 0x00 */
    UINT32 control;  /* 0x04*/
    UINT32 status;   /* 0x08*/
    UINT32 pad1;     /* 0x0c*/
    UINT32 intStatus;   /*0x10*/
    UINT32 intPending;  /*0x14*/
    UINT32 intPtr;      /*0x18*/
    UINT32 txMap;       /*0x1C*/ 
    UINT32 rxSize1;     /*0x20*/
    UINT32 rxOffset1;   /*0x24*/
    UINT32 rxSize2;     /*0x28*/
    UINT32 rxOffset2;   /*0x2C*/
    UINT32 rxSize3;     /*0x30*/
    UINT32 rxOffset3;   /*0x34*/
    UINT32 rxSize4;     /*0x38*/
    UINT32 rxOffset4;   /*0x3C*/
    UINT32 chipVersion; /*0x40*/
    UINT32 pad2[8];
    UINT32 ivr30;       /*0x60*/
    UINT32 ivr74;       /*0x64*/
    UINT32 pad3[7];
}VLYNQ_REG_SET;
    

typedef struct VLYNQ_DEV_t
{
    /** module index:1,2,3... used for debugging purposes */
    UINT32 dev_idx; 
    
    /*VLYNQ module base address */
    UINT32 module_base;
   
    /* clock source selection */
    VLYNQ_CLK_SOURCE clk_source;
   
    /* Clock Divider.Val=1 to 8. VLYNQ_clk = VBUSCLK/clk_div */
    UINT32  clk_div;
   
    /* State of the VLYNQ driver, set to VLYNQ_DRV_STATE_UNINIT, when initializing */
    VLYNQ_DRV_STATE state;
   
    /* Valid VLYNQ bus width, filled by driver  */
    VLYNQ_BUS_WIDTH width;
   
    /* local memory mapping   */
    VLYNQ_MEMORY_MAP local_mem;
   
    /* remote memory mapping   */
    VLYNQ_MEMORY_MAP remote_mem;
   
    /* Local module interrupt params */
    VLYNQ_INTERRUPT_CNTRL  local_irq;
   
    /* remote module interrupt params */
    VLYNQ_INTERRUPT_CNTRL  remote_irq;

    /*** ICB related fields **/
   
    /* Sizeof of ICB = VLYNQ_NUM_INT_BITS(for 32 bits in IntPending) + 
     * expansion slots for shared interrupts*/
    VLYNQ_INTR_CNTRL_ICB  pIntrCB[VLYNQ_NUM_INT_BITS + VLYNQ_IVR_CHAIN_SLOTS];
    VLYNQ_INTR_CNTRL_ICB  *freelist;
   
   /* table holding mapping between intVector and the bit position the interrupt
    * is mapped to(mapVector)*/
    INT8 vector_map[32];
   
    /* user callback for vlynq events, NULL if unused */
    VLYNQ_REPORT_CB        report_cb;    
    
   /* user callback for resetting/realeasing remote device */
    VLYNQ_RESET_REMOTE     reset_cb;

    /*** Handles provided for direct access to register set if need be
     * Must be intialized to point to appropriate address during 
     * vlynq_init */
    volatile VLYNQ_REG_SET * local;
    volatile VLYNQ_REG_SET * remote;

    UINT32  intCount; /* number of interrupts generated so far */
    UINT32  isrCount; /* number of ISR invocations so far */
}VLYNQ_DEV;


typedef struct VLYNQ_ISR_ARGS_t
{
    int irq;
    void * arg;
    void * regset;
}VLYNQ_ISR_ARGS;


/****************************************
 *        Function Prototypes           *
 * API exported by generic vlynq driver *
 ****************************************/
/* Initialization function */ 
GLOBAL INT32 vlynq_init( VLYNQ_DEV *pdev, VLYNQ_INIT_OPTIONS options);

/* Check vlynq link */
GLOBAL UINT32 vlynq_link_check( VLYNQ_DEV * pdev);

/* Set interrupt vector in local or remote device */
GLOBAL INT32 vlynq_interrupt_vector_set( VLYNQ_DEV *pdev, 
                                         UINT32 int_vector,
                                         UINT32 map_vector, 
                                         VLYNQ_DEV_TYPE dev,
                                         VLYNQ_INTR_POLARITY pol,
                                         VLYNQ_INTR_TYPE type);


GLOBAL INT32 vlynq_interrupt_vector_cntl( VLYNQ_DEV *pdev,
                                          UINT32 int_vector,
                                          VLYNQ_DEV_TYPE dev,
                                          UINT32 enable);

GLOBAL UINT32 vlynq_interrupt_get_count( VLYNQ_DEV *pdev,
                                         UINT32 map_vector);

GLOBAL INT32 vlynq_install_isr( VLYNQ_DEV *pdev,
                                UINT32 map_vector,
                                VLYNQ_INTR_CNTRL_ISR isr,
                                void *arg1, void *arg2, void *arg3);

GLOBAL INT32 vlynq_uninstall_isr( VLYNQ_DEV *pdev,
                                  UINT32 map_vector,
                                  void *arg1, void *arg2, void *arg3);


GLOBAL void vlynq_root_isr(void *arg);

GLOBAL void vlynq_delay(UINT32 clktime);

/* The following functions, provide better granularity in setting
 * interrupt parameters. (for better support of linux INT Controller)
 * Note: The interrupt source is identified by "map_vector"- the bit 
 * position in interrupt status register*/

GLOBAL INT32 vlynq_interrupt_vector_map(VLYNQ_DEV * pdev,
                                        VLYNQ_DEV_TYPE dev,
                                        UINT32 int_vector,
                                        UINT32 map_vector);

GLOBAL INT32 vlynq_interrupt_set_polarity(VLYNQ_DEV * pdev,
                                          VLYNQ_DEV_TYPE dev,
                                          UINT32 map_vector,
                                          VLYNQ_INTR_POLARITY pol);

GLOBAL INT32 vlynq_interrupt_get_polarity( VLYNQ_DEV *pdev ,
                                           VLYNQ_DEV_TYPE dev_type,
                                           UINT32 map_vector);

GLOBAL INT32 vlynq_interrupt_set_type(VLYNQ_DEV * pdev,
                                      VLYNQ_DEV_TYPE dev,
                                      UINT32 map_vector,
                                      VLYNQ_INTR_TYPE type);

GLOBAL INT32 vlynq_interrupt_get_type( VLYNQ_DEV *pdev, 
                                       VLYNQ_DEV_TYPE dev_type,
                                       UINT32 map_vector);

GLOBAL INT32 vlynq_interrupt_enable(VLYNQ_DEV* pdev,
                                    VLYNQ_DEV_TYPE dev,
                                    UINT32 map_vector);

GLOBAL INT32 vlynq_interrupt_disable(VLYNQ_DEV * pdev,
                                     VLYNQ_DEV_TYPE dev,
                                     UINT32 map_vector);
                 

              
        

#endif /* _VLYNQ_HAL_H_ */
