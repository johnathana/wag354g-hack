/******************************************************************************
 * FILE PURPOSE:    CPMAC Linux Network Device Driver Header
 ******************************************************************************
 * FILE NAME:       cpmac.h
 *
 * DESCRIPTION:     CPMAC Network Device Driver Header
 *
 * REVISION HISTORY:
 * Date            Name                   Details
 *----------------------------------------------------------------------------- 
 * 27 Nov 2002     Suraj S Iyer           Initial Create.
 * 09 Jun 2003     Suraj S Iyer           Preparing for GA.
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 *******************************************************************************/

#ifndef CPMAC_H
#define CPMAC_H

#include <linux/timer.h>
#include <linux/netdevice.h>
#include <asm/semaphore.h>
#include <linux/ctype.h>
#include <linux/interrupt.h>

#include "cpmacHalLx.h"
/*-----------------------------------------------------------------------------
 * Config macros. Use these to config the driver.
 *---------------------------------------------------------------------------*/
#define CPMAC_MAX_FRAME_SIZE         1518

#if defined(CONFIG_MIPS_AR7WRD) || defined(CONFIG_MIPS_AR7WI) || defined(CONFIG_MIPS_AR7VWI)|| defined(CONFIG_MIPS_AR7VW) 
#define CFG_RX_NUM_BUF_DESC            64 
#define CFG_RX_NUM_BUF_SERVICE         32 
#else                                     
#define CFG_RX_NUM_BUF_DESC            16 
#define CFG_RX_NUM_BUF_SERVICE         8  
#endif                                    

#define CFG_RX_BUF_OFFSET              0

#define CFG_TX_NUM_BUF_DESC            128
#define CFG_TX_NUM_BUF_SERVICE         20 
#define CFG_TX_BUF_OFFSET              0  /* Lets not change this. */
#define CFG_TX_TIMEOUT                 2000 /* ticks*/
#define CFG_TX_INT_DISABLE             1  /* Disable the Tx Complete interrupt */

#define CFG_JUMBO_FRAMES               1
#define CFG_SHORT_FRAMES               1
#define CFG_PROMISCOUS                 1
#define CFG_BROADCAST                  1
#define CFG_MULTICAST                  1
#define CFG_ALL_MULTI                  (1*(CFG_MULTICAST))
#define CFG_AUTO_NEGOTIATION           1

#if defined (CONFIG_MIPS_AVALANCHE_MARVELL)
#define EGRESS_TRAILOR_LEN             4 
#define CFG_START_LINK_SPEED           (_CPMDIO_NOPHY)
#undef  CPMAC_MAX_FRAME_SIZE
#define CPMAC_MAX_FRAME_SIZE          (1518 + EGRESS_TRAILOR_LEN)
#else
#define CFG_START_LINK_SPEED          (_CPMDIO_10 | _CPMDIO_100 | _CPMDIO_HD | _CPMDIO_FD) /* auto nego */
#endif

#define CFG_LOOP_BACK                  1
#define CFG_TX_FLOW_CNTL               0
#define CFG_RX_FLOW_CNTL               0
#define CFG_TX_PACING                  0
#define CFG_RX_PASS_CRC                0
#define CFG_QOS_802_1Q                 0
#define CFG_TX_NUM_CHAN                1


/*-----------------------------------------------------------------------------
 * Private macros.
 *---------------------------------------------------------------------------*/
#define MAX_TIMER               2
#define TX_TIMER                0
#define TICK_TIMER              0
#define MAX_TX_CHAN             8

#define CPMAC_LINK_OFF          0
#define CPMAC_LINK_ON           1
/*#define CPMAC_SPEED_100         2
#define CPMAC_SPEED_10          3
#define CPMAC_FULL_DPLX         4
#define CPMAC_HALF_DPLX         5*/
#define CPMAC_RX_ACTIVITY       2
#define CPMAC_TX_ACTIVITY       3

struct cpmac_timer_info;

typedef int  (*CPMAC_HAL_ISR_FUNC_T)(HAL_DEVICE*, int*);
typedef int  (*CPMAC_TIMEOUT_CB_T)(struct cpmac_timer_info*);

typedef struct cpmac_ability_info
{
    int promiscous;
    int broadcast;
    int multicast;
    int all_multi;
    int loop_back;
    int jumbo_frames;
    int short_frames;
    int auto_negotiation;
    int tx_flow_control;
    int rx_flow_control;
    int tx_pacing;
    int link_speed;
    int rx_pass_crc;
    int qos_802_1q;
    int tx_num_chan;
}
CPMAC_ABILITY_INFO_T;

#ifdef DEBUG
typedef struct cpmac_timer_info
{
    void               *owner;
    UINT32             delay_ticks;
    WDOG_ID            timer_id;
    UINT32             is_running;
    UINT32             timer_set_at;
    CPMAC_TIMEOUT_CB_T timeout_CB;
} CPMAC_TIMER_INFO_T;

typedef struct
{
    void               *owner;
    unsigned int       num_cl_desc;
    CL_DESC            *cl_desc_tbl;
    M_CL_CONFIG        *m_cl_blk_config;
    NET_POOL           *net_pool;
    CL_POOL_ID         clPoolId;

} CPMAC_NET_MEM_INFO_T;

#endif

typedef struct
{
    void                  *owner;
    CPMAC_HAL_ISR_FUNC_T  hal_isr;
    struct tasklet_struct tasklet;
    int                   intr;

} CPMAC_ISR_INFO_T;

typedef struct cpmac_chan
{
   int num_BD;
   int buffer_size;
   int buffer_offset;
   int service_max;
   int state;
   int tot_buf_size;
   int tot_reserve_bytes;

} CPMAC_CHAN_T;

#define CHAN_CLOSE  0
#define CHAN_OPENED 1

typedef struct 
{
    int          cfg_chan;
    int          dev_chan;
    int          opened_chan;
    CPMAC_CHAN_T chan[1];
    int          enable_802_1q;

} CPMAC_RX_CHAN_INFO_T;

typedef struct
{
    int          cfg_chan;
    int          dev_chan;
    int          opened_chan;
    int          tx_int_disable;
    CPMAC_CHAN_T chan[MAX_TX_CHAN];
   
} CPMAC_TX_CHAN_INFO_T;



typedef struct
{
   void                        *owner;
   HAL_FUNCTIONS               *hal_funcs;
   HAL_DEVICE                  *hal_dev;
   OS_FUNCTIONS                *os_funcs; 
//   SEM_ID                      chan_teardown_sem;
   int                         non_data_irq_expected;
} CPMAC_DRV_HAL_INFO_T;


typedef struct
{
    unsigned long tx_discards;
    unsigned long rx_discards;
    unsigned long start_tick;

} CPMAC_DRV_STATS_T;

typedef struct 
{
    unsigned long   ifInGoodFrames;
    unsigned long   ifInBroadcasts;
    unsigned long   ifInMulticasts;
    unsigned long   ifInPauseFrames;
    unsigned long   ifInCRCErrors;
    unsigned long   ifInAlignCodeErrors;
    unsigned long   ifInOversizedFrames;
    unsigned long   ifInJabberFrames;
    unsigned long   ifInUndersizedFrames;
    unsigned long   ifInFragments;
    unsigned long   ifInFilteredFrames;
    unsigned long   ifInQosFilteredFrames;
    unsigned long   ifInOctets;
    unsigned long   ifOutGoodFrames;
    unsigned long   ifOutBroadcasts;
    unsigned long   ifOutMulticasts;
    unsigned long   ifOutPauseFrames;
    unsigned long   ifDeferredTransmissions;
    unsigned long   ifCollisionFrames;
    unsigned long   ifSingleCollisionFrames;
    unsigned long   ifMultipleCollisionFrames;
    unsigned long   ifExcessiveCollisionFrames;
    unsigned long   ifLateCollisions;
    unsigned long   ifOutUnderrun;
    unsigned long   ifCarrierSenseErrors;
    unsigned long   ifOutOctets;
    unsigned long   if64OctetFrames;
    unsigned long   if65To127OctetFrames;
    unsigned long   if128To255OctetFrames;
    unsigned long   if256To511OctetFrames;
    unsigned long   if512To1023OctetFrames;
    unsigned long   if1024ToUPOctetFrames;
    unsigned long   ifNetOctets;
    unsigned long   ifRxSofOverruns;
    unsigned long   ifRxMofOverruns;
    unsigned long   ifRxDMAOverruns;

} CPMAC_DEVICE_MIB_T;


typedef struct
{
    void                         *owner;
    int                          timer_count;
    int                          timer_created;
    struct timer_list            timer[1];
    CPMAC_DRV_HAL_INFO_T         *drv_hal;
    unsigned int                 num_of_intr;
    CPMAC_ISR_INFO_T             cpmac_isr;
    unsigned int                 link_speed;
    unsigned int                 link_mode;
    unsigned int                 enable_802_1q;
    unsigned int                 timer_access_hal;
    unsigned int                 loop_back;
    CPMAC_RX_CHAN_INFO_T         *rx_chan_info;
    CPMAC_TX_CHAN_INFO_T         *tx_chan_info;
    CPMAC_ABILITY_INFO_T         *ability_info;
    CPMAC_DEVICE_MIB_T           *device_mib;
    CPMAC_DRV_STATS_T            *stats;
    unsigned int                 flags;
    unsigned int                 delay_ticks;
    char                         mac_addr[6];
    struct net_device_stats      net_dev_stats; 
//    rwlock_t                     rw_lock;
    int                          set_to_close;
    struct net_device            *next_device;
    unsigned int                 instance_num;
    unsigned int                 non_data_irq_expected;
    unsigned long                dev_size;
    void*                        led_handle;
} CPMAC_PRIVATE_INFO_T;


/* Private flags */

/* bit 0 to 31, bit 32 is used to indicate set or reset */

#define IFF_PRIV_SHORT_FRAMES   0x00010000
#define IFF_PRIV_JUMBO_FRAMES   0x00020000
#define IFF_PRIV_AUTOSPEED      0x00080000
#define IFF_PRIV_LINK10_HD      0x00100000
#define IFF_PRIV_LINK10_FD      0x00200000
#define IFF_PRIV_LINK100_HD     0x00400000
#define IFF_PRIV_LINK100_FD     0x00800000
#define IFF_PRIV_8021Q_EN       0x01000000
#define IFF_PRIV_NUM_TX_CHAN    0x02000000
#define IFF_PRIV_TX_FLOW_CNTL   0x04000000
#define IFF_PRIV_RX_FLOW_CNTL   0x08000000
#define IFF_PRIV_TX_PACING      0x10000000
#define IFF_PRIV_RX_PASS_CRC    0x20000000

#define PRIVCSFLAGS             0x200
#define PRIVCGFLAGS             0x201


#define BLOCKING      1
#define CHAN_TEARDOWN 2
#define CHAN_SETUP    4
#define COMPLETE      8
#define FREE_BUFFER   16


static const char pszStats0[]            = "Stats0";
static const char pszStats1[]            = "Stats1";
static const char pszStats2[]            = "Stats2";
static const char pszStats3[]            = "Stats3";
static const char pszStats4[]            = "Stats4";
static const char pszStatsDump[]         = "StatsDump";
static const char pszStatsClear[]        = "StatsClear";
static const char pszRX_PASS_CRC[]       = "RX_PASS_CRC";
static const char pszRX_QOS_EN[]         = "RX_QOS_EN";
static const char pszRX_NO_CHAIN[]       = "RX_NO_CHAIN";
static const char pszRX_CMF_EN[]         = "RX_CMF_EN";
static const char pszRX_CSF_EN[]         = "RX_CSF_EN";
static const char pszRX_CEF_EN[]         = "RX_CEF_EN";
static const char pszRX_CAF_EN[]         = "RX_CAF_EN";
static const char pszRX_PROM_CH[]        = "RX_PROM_CH";
static const char pszRX_BROAD_EN[]       = "RX_BROAD_EN";
static const char pszRX_BROAD_CH[]       = "RX_BROAD_CH";
static const char pszRX_MULT_EN[]        = "RX_MULT_EN";
static const char pszRX_MULT_CH[]        = "RX_MULT_CH";
static const char pszTX_PTYPE[]          = "TX_PTYPE";
static const char pszTX_PACE[]           = "TX_PACE";
static const char pszMII_EN[]            = "MII_EN";
static const char pszTX_FLOW_EN[]        = "TX_FLOW_EN";
static const char pszRX_FLOW_EN[]        = "RX_FLOW_EN";
static const char pszRX_MAXLEN[]         = "RX_MAXLEN";
static const char pszRX_FILTERLOWTHRESH[]   = "RX_FILTERLOWTHRESH";
static const char pszRX0_FLOWTHRESH[]   = "RX0_FLOWTHRESH";
static const char pszRX_UNICAST_SET[]    = "RX_UNICAST_SET";
static const char pszRX_UNICAST_CLEAR[]  = "RX_UNICAST_CLEAR";
static const char pszMdioConnect[]       = "MdioConnect";
static const char pszMacAddr[]           = "MacAddr";
static const char pszTick[]              = "Tick";
static const char pszRX_MULTICAST[]      = "RX_MULTICAST";
static const char pszRX_MULTI_ALL[]      = "RX_MULTI_ALL";
static const char pszRX_MULTI_SINGLE[]   = "RX_MULTI_SINGLE";

static const char pszSet[]               = "Set";
static const char pszGet[]               = "Get";
static const char pszClear[]             = "Clear";


void *cpmac_hal_malloc_buffer(unsigned int size, void *MemBase, unsigned int MemRange,
                              HAL_DEVICE *HalDev, HAL_RECEIVEINFO *HalReceiveInfo, 
                              OS_RECEIVEINFO **OsReceiveInfo, OS_DEVICE *OsDev);

void cpmac_hal_tear_down_complete(OS_DEVICE*, int, int);
int  cpmac_hal_control(OS_DEVICE *p_END_obj, const char *key, 
                   const char *action, void *value);
int  cpmac_hal_receive(OS_DEVICE *p_END_obj, FRAGLIST *fragList, 
               unsigned int FragCount, unsigned int pkt_len, 
               HAL_RECEIVEINFO *halReceiveInfo, 
               unsigned int mode);
int  cpmac_hal_send_complete(OS_SENDINFO*);

void  cpmac_hal_isr(int irq, void *p_param, struct pt_regs *p_cb_param);
void  cpmac_handle_tasklet(unsigned long data);

inline static int cpmac_ci_strcmp(const char *s1, const char *s2)
{
    while(*s1 && *s2)
    {
        if(tolower(*s1) != tolower(*s2))
            break;
        s1++;
        s2++;
    }

    return(tolower(*s1) - tolower(*s2));
}

#endif
