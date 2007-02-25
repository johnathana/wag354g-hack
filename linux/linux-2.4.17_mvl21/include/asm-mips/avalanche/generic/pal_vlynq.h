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
 * FILE PURPOSE:    PAL VLYNQ Header File
 ******************************************************************************
 * FILE NAME:       pal_vlynq.h
 *
 * DESCRIPTION:     PAL VLYNQ Header File
 *
 * (C) Copyright 2004, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __PAL_VLYNQ_H__
#define __PAL_VLYNQ_H__

#include "pal_vlynqOs.h"

#define LATEST_VLYNQ_REV 0x00010206

#ifndef  NULL
#define  NULL 0
#endif 

/* The vlynq handle. */
typedef void PAL_VLYNQ_HND;

/* 
 * The parameters: 
 * 
 * #define MAX_ROOT_VLYNQ     The number of vlynq ports on the SoC. 
 * #define MAX_VLYNQ_COUNT    The maximum number of vlynq interfaces with in the system.
 * 
 * are system specific and should be defined outside this module.  
 *
 */

#ifndef MAX_ROOT_VLYNQ
#error "Please define MAX_ROOT_VLYNQ - The number of vlynq ports on the SoC."
#endif

#ifndef MAX_VLYNQ_COUNT
#error  "Please define MAX_VLYNQ_COUNT - The maximum number of vlynq interfaces with in the system."
#endif
 
#define MAX_VLYNQ_REGION             4
#define MAX_DEV_PER_VLYNQ            MAX_VLYNQ_REGION
#define MAX_DEV_COUNT                2
#define TYPICAL_NUM_ISR_PER_IRQ      1
#define MAX_IRQ_PER_CHAIN            32
#define MAX_IRQ_PER_VLYNQ            8

#include "pal_vlynqDev.h"
#include "pal_vlynqIoctl.h"

typedef enum pal_vlynq_clock_dir_enum
{
    pal_vlynq_clk_in = 0,            /* Sink   */
    pal_vlynq_clk_out                /* Source */
} PAL_VLYNQ_CLOCK_DIR_ENUM_T;

typedef enum pal_vlynq_endian_enum
{
    pal_vlynq_ignore_en = 0,
    pal_vlynq_little_en = 0x2f2f2f2f,
    pal_vlynq_big_en    = 0xf2f2f2f2

} PAL_VLYNQ_ENDIAN_ENUM_T;

typedef enum pal_vlynq_rtm_cfg_enum
{
    no_rtm_cfg = 0, 
    rtm_auto_select_sample_val, 
    rtm_force_sample_val 

} PAL_VLYNQ_RTM_CFG_ENUM_T;


typedef enum
{
    pal_vlynq_init_success             = 0,
    pal_vlynq_init_no_link_on_reset,
    pal_vlynq_init_no_mem,
    pal_vlynq_init_clk_cfg,
    pal_vlynq_init_no_link_on_clk_cfg,
    pal_vlynq_init_internal_problem,
    pal_vlynq_init_invalid_param,
    pal_vlynq_init_local_high_rev,
    pal_vlynq_init_peer_high_rev,
    pal_vlynq_init_rtm_cfg,
    pal_vlynq_init_no_link_on_rtm_cfg

} PAL_VLYNQ_INIT_ERR_ENUM_T;

typedef struct
{
    /* 1 => the VLYNQ module is on the SOC running the code, 0 => otherwise.*/
    Uint8                  on_soc;               
    PAL_VLYNQ_INIT_ERR_ENUM_T error_status;
    char                   error_msg[50];

    /* Virtual Base Address of the module. */
    Uint32                 base_addr;            

    /* The number of millsecs that the software should allow for initialization to complete. */
    Uint32                 init_timeout_in_ms;   
    
    /* The clock divisor for the local VLYNQ module. */
    Uint8                  local_clock_div;      

    /* The clock direction; sink or source for the local VLYNQ module. */
    PAL_VLYNQ_CLOCK_DIR_ENUM_T local_clock_dir;      

    /* 1 => interrupts are being handled locally or 0 => sent over the bus. */
    Uint8                  local_intr_local;     

    /* The IRQ vector# to be used on the local VLYNQ module. Valid values are 0 to 31. Should be unique. */
    Uint8                  local_intr_vector;    

    /* 1 => enable the local irq vector or => disable. */
    Uint8                  local_intr_enable;    

    /* Valid only if intr_local is set.
       1 => write to the local VLYNQ interrupt pending register; 
       0 => write to the location refered by local address pointer. */ 
    Uint8                  local_int2cfg;        

    /* Address to which the irq should be written to; valid only if int2cfg is not set. */ 
    Uint32                 local_intr_pointer;   

    /* Endianess of the local VLYNQ module. */
    PAL_VLYNQ_ENDIAN_ENUM_T  local_endianness;     

    /* The physical portal address of the local VLYNQ. */
    Uint32                 local_tx_addr;        

    /* The RTM configuration for the local VLYNQ. */
    PAL_VLYNQ_RTM_CFG_ENUM_T local_rtm_cfg_type; 

    /* The RTM sample value for the local VLYNQ, valid only if forced rtm cfg type is selected. */
    Uint8                  local_rtm_sample_value;  

    /* TX Fast path 0 => slow path, 1 => fast path. */
    Bool                   local_tx_fast_path;

    /* The clock divisor for the peer VLYNQ module. */
    Uint8                  peer_clock_div;       

    /* The clock direction; sink or source for the peer VLYNQ module. */
    PAL_VLYNQ_CLOCK_DIR_ENUM_T peer_clock_dir;       

    /* 1 => interrupts are being handled by peer VLYNQ or 0 => sent over the bus. */
    Uint8                  peer_intr_local;      

    /* 1 => enable the peer irq vector or => disable. */
    Uint8                  peer_intr_enable;     

    /* The IRQ vector# to be used on the peer VLYNQ module. Valid values are 0 to 31. Should be unique. */
    Uint8                  peer_intr_vector;     

    /* Valid only if intr_local is set.
       1 => write to the local VLYNQ interrupt pending register; 
       0 => write to the location refered by local address pointer. */ 
    Uint8                  peer_int2cfg;         

    /* Address to which the irq should be written to; valid only if int2cfg is not set. */ 
    Uint32                 peer_intr_pointer;    

    /* Endianess of the local VLYNQ module. */
    PAL_VLYNQ_ENDIAN_ENUM_T  peer_endianness;      

    /* The physical portal address of the peer VLYNQ. */
    Uint32                 peer_tx_addr;         

    /* The RTM configuration for the peer VLYNQ. */
    PAL_VLYNQ_RTM_CFG_ENUM_T peer_rtm_cfg_type; 

    /* The RTM sample value for the local VLYNQ, valid only if forced rtm cfg type is selected. */
    Uint8                  peer_rtm_sample_value;  

    /* TX Fast path 0 => slow path, 1 => fast path. */
    Bool                   peer_tx_fast_path;


} PAL_VLYNQ_CONFIG_T;

typedef enum 
{
   pal_vlynq_high_irq_pol = 0,
   pal_vlynq_low_irq_pol    

}PAL_VLYNQ_IRQ_POL_ENUM_T;

typedef enum 
{
   pal_vlynq_level_irq_type = 0,
   pal_vlynq_edge_irq_type    

}PAL_VLYNQ_IRQ_TYPE_ENUM_T;

/* Error Codes */

#define PAL_VLYNQ_OK                    0
#define PAL_VLYNQ_INVALID_PARAM        -1
#define PAL_VLYNQ_INTERNAL_ERROR       -2

PAL_Result PAL_sysVlynqInit(void);

PAL_Result PAL_vlynqAddDevice(PAL_VLYNQ_HND *vlynq, PAL_VLYNQ_DEV_HND *vlynq_dev,
			      Bool peer);
PAL_Result PAL_vlynqRemoveDevice(PAL_VLYNQ_HND *vlynq, PAL_VLYNQ_DEV_HND *vlynw_dev);

PAL_Result PAL_vlynqMapRegion(PAL_VLYNQ_HND *vlynq, Bool remote, Uint32 region_id, 
		                Uint32 region_offset, Uint32 region_size, 
				PAL_VLYNQ_DEV_HND *vlynq_dev);
PAL_Result PAL_vlynqUnMapRegion(PAL_VLYNQ_HND *vlynq, Bool remote, Uint32 region_id, 
				   PAL_VLYNQ_DEV_HND *vlynq_dev);

PAL_Result PAL_vlynqGetDevBase(PAL_VLYNQ_HND     *vlynq,
		                  Uint32            offset,
				  Uint32            *base_addr,
                                  PAL_VLYNQ_DEV_HND *vlynq_dev);

Bool PAL_vlynqGetLinkStatus(PAL_VLYNQ_HND *vlynq);

Int32 PAL_vlynqGetNumRoot(void);

PAL_VLYNQ_HND* PAL_vlynqGetRoot(Int32 index);

PAL_VLYNQ_HND* PAL_vlynqGetNext(PAL_VLYNQ_HND *this);

Int32 PAL_vlynqIsLast(PAL_VLYNQ_HND *this);

Int32 PAL_vlynqGetChainLength(PAL_VLYNQ_HND *this);

PAL_Result PAL_vlynqGetBaseAddr(PAL_VLYNQ_HND *vlynq, Uint32 *base);

PAL_VLYNQ_HND *PAL_vlynqGetRootAtBase(Uint32 base_addr);

PAL_VLYNQ_HND *PAL_vlynqGetRootVlynq(PAL_VLYNQ_HND *vlynq);
               
PAL_Result PAL_vlynqChainAppend(PAL_VLYNQ_HND *this, PAL_VLYNQ_HND *to);

PAL_Result PAL_vlynqChainUnAppend(PAL_VLYNQ_HND *this, PAL_VLYNQ_HND *from);

PAL_Result PAL_vlynqMapIrq(PAL_VLYNQ_HND *vlynq, Uint32 hw_intr_line,
		           Uint32 irq, PAL_VLYNQ_DEV_HND *vlynq_dev);

PAL_Result PAL_vlynqUnMapIrq(PAL_VLYNQ_HND *vlynq, Uint32 irq, 
		                PAL_VLYNQ_DEV_HND *vlynq_dev);

PAL_Result PAL_vlynqAddIsr(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		           PAL_VLYNQ_DEV_ISR_FN  dev_isr,
			   PAL_VLYNQ_DEV_ISR_PARAM_GRP_T* isr_param);

PAL_Result PAL_vlynqRemoveIsr(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		              PAL_VLYNQ_DEV_ISR_PARAM_GRP_T *isr_param);

PAL_Result PAL_vlynqRootIsr(PAL_VLYNQ_HND *p_vlynq);

PAL_Result PAL_vlynqGetIrqCount(PAL_VLYNQ_HND *vlynq, Uint32 irq, Uint32 *count);

PAL_VLYNQ_HND* PAL_vlynqGetForIrq(PAL_VLYNQ_HND *root, Uint32 irq);

PAL_Result PAL_vlynqSetIrqPol(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		              PAL_VLYNQ_IRQ_POL_ENUM_T pol);

PAL_Result PAL_vlynqGetIrqPol(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		              PAL_VLYNQ_IRQ_POL_ENUM_T *pol);

PAL_Result PAL_vlynqSetIrqType(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		               PAL_VLYNQ_IRQ_TYPE_ENUM_T type);

PAL_Result PAL_vlynqGetIrqType(PAL_VLYNQ_HND *vlynq, Uint32 irq,
		               PAL_VLYNQ_IRQ_TYPE_ENUM_T *type);

PAL_Result PAL_vlynqDisableIrq(PAL_VLYNQ_HND *vlynq, Uint32 irq);

PAL_Result PAL_vlynqEnableIrq(PAL_VLYNQ_HND *vlynq, Uint32 irq);

PAL_Result PAL_vlynqConfigClock(PAL_VLYNQ_HND                *p_vlynq,
	  	                PAL_VLYNQ_CLOCK_DIR_ENUM_T   local_clock_dir,
		                PAL_VLYNQ_CLOCK_DIR_ENUM_T   peer_clock_dir,
				Uint8                        local_clock_div,
				Uint8                        peer_clock_div);

PAL_VLYNQ_HND *PAL_vlynqInit(PAL_VLYNQ_CONFIG_T *pal_vlynq_config);

PAL_Result PAL_vlynqCleanUp(PAL_VLYNQ_HND *vlynq);

Int32 PAL_vlynqDump(PAL_VLYNQ_HND *vlynq, Uint32 dump_type, char *buf, Int32 limit, Int32 *eof);

PAL_Result PAL_vlynqIoctl(PAL_VLYNQ_HND *vlynq_hnd, Uint32 cmd, Uint32 cmd_val);

#endif /* __PAL_VLYNQ_H__ */

