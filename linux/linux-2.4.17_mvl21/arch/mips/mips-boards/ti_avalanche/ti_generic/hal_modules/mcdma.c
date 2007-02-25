/******************************************************************************
 * FILE PURPOSE:    Multi Channel Direct Memory Access (MC-DMA) Source
 ********************************************************************************
 * FILE NAME:       mcdma.c
 *
 * DESCRIPTION:     Platform and OS independent Implementation for MCDMA Controller
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#include "haltypes.h"
#include "mcdma.h"

#define OK                  0
#define ERR_DMA_ACTIVE      -1
#define ERR_INVALID_LENGTH  -2
#define ERR_INVALID_INPUT   -3

static UINT32 mcdma_base_address;

#define MCDMA_REG32_DATA(addr)              (*(volatile UINT32 *)(addr))
#define MCDMA_REG32_WRITE(addr, data)       MCDMA_REG32_DATA(addr) = data;
#define MCDMA_REG32_READ(addr, data)        data = (UINT32) MCDMA_REG32_DATA(addr);

/* MCDMA module definations */

#define MCDMA_NUM_CHANNELS    4
#define MCDMA_CH_OFFSET       0x40
#define MCDMA0                (mcdma_base_address + 0x0)
#define MCDMA1                (mcdma_base_address + 0x40)
#define MCDMA2                (mcdma_base_address + 0x80)
#define MCDMA3                (mcdma_base_address + 0xC0)

#define MCDMA_SRC_ADDR        0x0
#define MCDMA_DST_ADDR        0x4
#define MCDMA_CONTROL         0x8

#define MCDMA_START_BIT       0x80000000 /* bit 31 */
#define MCDMA_STOP_MASK       0x7FFFFFFF /* bit 31 */
#define MCDMA_ACTIVE_BIT      0x40000000 /* bit 30 */

#define MCDMA_BURST_MODE_SHIFT	20
#define MCDMA_SRC_MODE_SHIFT	22
#define MCDMA_DST_MODE_SHIFT	24

void mcdma_init(UINT32 base_addr)
{
    UINT32 ch;
    UINT32 addr;

    mcdma_base_address = base_addr;

    /* init all dma registers */
    for (ch=0; ch < MCDMA_NUM_CHANNELS; ch++)
    {
        addr =  (mcdma_base_address + (ch * MCDMA_CH_OFFSET) );
        MCDMA_REG32_WRITE((addr + MCDMA_CONTROL), 0);
        MCDMA_REG32_WRITE((addr + MCDMA_SRC_ADDR), 0);
        MCDMA_REG32_WRITE((addr + MCDMA_DST_ADDR), 0);
    }
}


void mcdma_control(MCDMA_CHANNEL_T mcdma_ch, MCDMA_CTRL_T mcdma_ctrl)
{
    UINT32 reg_value;
    UINT32 addr;

    addr =  (mcdma_base_address + (mcdma_ch * MCDMA_CH_OFFSET) );

    /* check if dma is active */
    MCDMA_REG32_READ((addr + MCDMA_CONTROL), reg_value);
    if (reg_value & MCDMA_ACTIVE_BIT)
    {
        /* DMA is active */
        return;
    }
             
    if (mcdma_ctrl == MCDMA_START)
    {
        MCDMA_REG32_READ((addr + MCDMA_CONTROL), reg_value);
        reg_value |= MCDMA_START_BIT;
        MCDMA_REG32_WRITE((addr + MCDMA_CONTROL), reg_value);
    }
    else
    {
        MCDMA_REG32_READ((addr + MCDMA_CONTROL), reg_value);
        reg_value &= MCDMA_STOP_MASK;
        MCDMA_REG32_WRITE((addr + MCDMA_CONTROL), reg_value);
    }
}

INT32 mcdma_setdmaparams (MCDMA_CHANNEL_T mcdma_ch,
                          UINT32 src_addr, 
                          UINT32 dst_addr, 
                          UINT32 length, 
                          MCDMA_BURST_MODE_T burst_mode, 
                          MCDMA_ADDR_MODE_T src_addr_mode,
                          MCDMA_ADDR_MODE_T dst_addr_mode)
{
    UINT32 reg_value;
    UINT32 addr;

    if (mcdma_ch < MCDMA_CHANNEL_0 || mcdma_ch > MCDMA_CHANNEL_3)
    {
        return (ERR_INVALID_INPUT);
    }
    addr =  (mcdma_base_address + (mcdma_ch * MCDMA_CH_OFFSET));
    
    /* check if dma is active */
    MCDMA_REG32_READ((addr + MCDMA_CONTROL), reg_value);
    if (reg_value & MCDMA_ACTIVE_BIT)
    {
        /* DMA is active */
        return (ERR_DMA_ACTIVE);
    }

    if (length <=0) 
    {
        /* Invalid length */
        return (ERR_INVALID_LENGTH);
    }
    
    reg_value = (length/4 << 2);    /* length should be in 32 bit words */
    reg_value=reg_value | (burst_mode<<MCDMA_BURST_MODE_SHIFT);
    reg_value=reg_value | (src_addr_mode<<MCDMA_SRC_MODE_SHIFT);
    reg_value=reg_value | (dst_addr_mode<<MCDMA_DST_MODE_SHIFT);
    MCDMA_REG32_WRITE((addr + MCDMA_CONTROL), reg_value);
    MCDMA_REG32_WRITE((addr + MCDMA_SRC_ADDR), src_addr);
    MCDMA_REG32_WRITE((addr + MCDMA_DST_ADDR), dst_addr);

    return (OK);
}

