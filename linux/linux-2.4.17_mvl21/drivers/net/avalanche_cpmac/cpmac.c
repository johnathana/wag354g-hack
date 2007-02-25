/******************************************************************************
 * FILE PURPOSE:    CPMAC Linux Network Device Driver Source
 ******************************************************************************
 * FILE NAME:       cpmac.c
 *
 * DESCRIPTION:     CPMAC Network Device Driver Source
 *
 * REVISION HISTORY:
 *
 * Date           Description                               Author
 *-----------------------------------------------------------------------------
 * 27 Nov 2002    Initial Creation                          Suraj S Iyer  
 * 09 Jun 2003    Updates for GA                            Suraj S Iyer
 * 30 Sep 2003    Updates for LED, Reset stats              Suraj S Iyer
 * 
 * (C) Copyright 2003, Texas Instruments, Inc
 *******************************************************************************/
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>     
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include <linux/skbuff.h>

#include <asm/mips-boards/prom.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/config.h>

#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/generic/if_port.h>

extern void build_psp_config(void);
extern void psp_config_cleanup(void);

#include "cpmacHalLx.h"
#include "cpmac.h"

static struct net_device *last_cpmac_device = NULL;
static int    cpmac_devices_installed = 0;

void xdump( u_char*  cp, int  length, char*  prefix );

unsigned int  cpmac_cpu_freq = 0;

char cpmac_version[] = "1.5";

char l3_align_array[]            = {0x02, 0x01, 0x00, 0x03};
#define L3_ALIGN(i)                l3_align_array[i]

char add_for_4byte_align[]       = {0x04, 0x03, 0x02, 0x05};
#define ADD_FOR_4BYTE_ALIGN(i)     add_for_4byte_align[i]


#define TPID                       0x8100
#define IS_802_1Q_FRAME(byte_ptr)  (*(unsigned short*)byte_ptr == TPID)
#define TPID_START_OFFSET          12
#define TCI_START_OFFSET           14
#define TCI_LENGTH                 2
#define TPID_LENGTH                2
#define TPID_END_OFFSET            (TPID_START_OFFSET + TPID_LENGTH)
#define TCI_END_OFFSET             (TCI_START_OFFSET  + TCI_LENGTH)
#define IS_VALID_VLAN_ID(byte_ptr) ((*(unsigned short*)byte_ptr) && 0xfff != 0)
#define MAX_CLASSES                8
#define MAX_USER_PRIORITY          8
#define CONTROL_802_1Q_SIZE        (TCI_LENGTH + TPID_LENGTH)

unsigned char user_priority_to_traffic_class_map[MAX_CLASSES][MAX_USER_PRIORITY] = 
{ 
  {0, 0, 0, 1, 1, 1, 1, 2}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 1}, 
  {0, 0, 0, 1, 1, 2, 2, 3}, 
  {0, 1, 1, 2, 2, 3, 3, 4}, 
  {0, 1, 1, 2, 3, 4, 4, 5}, 
  {0, 1, 2, 3, 4, 5, 5, 6}, 
  {0, 1, 2, 3, 4, 5, 6, 7}
};

#define GET_802_1P_CHAN(x,y) user_priority_to_traffic_class_map[x][(y & 0xe0)]

#if defined(CONFIG_MIPS_SEAD2)
unsigned long temp_base_address[2] = {0xa8610000, 0xa8612800};
unsigned long temp_reset_value[2] = { 1<< 17,1<<21};
#define RESET_REG_PRCR   (*(volatile unsigned int *)((0xa8611600 + 0x0)))
#define VERSION(base)   (*(volatile unsigned int *)(((base)|0xa0000000) + 0x0))
#endif

MODULE_AUTHOR("Maintainer: Suraj S Iyer <ssiyer@ti.com>");
MODULE_DESCRIPTION("Driver for TI CPMAC");

static int cfg_link_speed = 0;
MODULE_PARM(cfg_link_speed, "i");
MODULE_PARM_DESC(cfg_link_speed, "Fixed speed of the Link: <100/10>");

static char *cfg_link_mode = NULL;
MODULE_PARM(cfg_link_mode, "1-3s");
MODULE_PARM_DESC(cfg_link_mode, "Fixed mode of the Link: <fd/hd>");

int cpmac_debug_mode = 0;
MODULE_PARM(debug_mode, "i");
MODULE_PARM_DESC(debug_mode, "Turn on the debug info: <0/1>. Default is 0 (off)");

#define dbgPrint if (cpmac_debug_mode) printk
#define errPrint printk

static int g_cfg_start_link_params = CFG_START_LINK_SPEED;
static int g_init_enable_flag      = 0;
static int cfg_start_link_speed;
static int cpmac_max_frame_size;

static struct net_device *g_dev_array[2];
static struct proc_dir_entry *gp_stats_file = NULL;

//-----------------------------------------------------------------------------
// Statistics related private functions.
//----------------------------------------------------------------------------- 
static int  cpmac_p_update_statistics(struct net_device *p_dev, char *buf, int limit, int *len);
static int  cpmac_p_read_rfc2665_stats(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int  cpmac_p_read_link(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int  cpmac_p_read_stats(char* buf, char **start, off_t offset, int count, int *eof, void *data);
static int  cpmac_p_write_stats (struct file *fp, const char * buf, unsigned long count, void * data);
static int  cpmac_p_reset_statistics (struct net_device *p_dev);
static int  cpmac_p_get_version(char *buf, char **start, off_t offset, int count, int *eof, void *data);

static int  cpmac_p_detect_manual_cfg(int, char*, int);
static int  cpmac_p_process_status_ind(CPMAC_PRIVATE_INFO_T *p_cpmac_priv);

//-----------------------------------------------------------------------------
// Timer related private functions.
//-----------------------------------------------------------------------------
static int  cpmac_p_timer_init(CPMAC_PRIVATE_INFO_T *p_cpmac_priv);
// static int  cpmac_timer_cleanup(CPMAC_PRIVATE_INFO_T *p_cpmac_priv);
static void cpmac_p_tick_timer_expiry(unsigned long p_cb_param);
inline static int cpmac_p_start_timer(struct timer_list *p_timer, unsigned int delay_ticks);
static int  cpmac_p_stop_timer(struct timer_list *p_timer);

//------------------------------------------------------------------------------
// Device configuration and setup related private functions.
//------------------------------------------------------------------------------
static int cpmac_p_probe_and_setup_device(CPMAC_PRIVATE_INFO_T *p_cpmac_priv, unsigned long *p_dev_flags);
static int cpmac_p_setup_driver_params(CPMAC_PRIVATE_INFO_T *p_cpmac_priv);
inline static int cpmac_p_rx_buf_setup(CPMAC_RX_CHAN_INFO_T *p_rx_chan);

//-----------------------------------------------------------------------------
// Net device related private functions.
//-----------------------------------------------------------------------------
static int  cpmac_dev_init(struct net_device *p_dev);
static int  cpmac_dev_open( struct net_device *dev );
static int  cpmac_dev_close(struct net_device *p_dev);
static void cpmac_dev_mcast_set(struct net_device *p_dev);
static int cpmac_dev_set_mac_addr(struct net_device *p_dev,void * addr);
static int  cpmac_dev_tx( struct sk_buff *skb, struct net_device *p_dev);
static struct net_device_stats *cpmac_dev_get_net_stats (struct net_device *dev);

static int cpmac_p_dev_enable( struct net_device *p_dev);


static unsigned char str2hexnum(unsigned char c)
{
        if(c >= '0' && c <= '9')
                return c - '0';
        if(c >= 'a' && c <= 'f')
                return c - 'a' + 10;
        if(c >= 'A' && c <= 'F')
                return c - 'A' + 10;
        return 0; 
}

static void str2eaddr(unsigned char *ea, unsigned char *str)
{
    int i;
    unsigned char num;
    for(i = 0; i < 6; i++) {
        if((*str == '.') || (*str == ':'))
            str++;
        num = str2hexnum(*str++) << 4;
        num |= (str2hexnum(*str++));
        ea[i] = num;
    }
}

//-----------------------------------------------------------------------------
// Statistics related private functions.
//-----------------------------------------------------------------------------
static int cpmac_p_update_statistics(struct net_device *p_dev, char *buf, int limit, int *p_len)
{
    int                 ret_val   = -1;
    unsigned long rx_hal_errors   = 0;
    unsigned long rx_hal_discards = 0;
    unsigned long tx_hal_errors   = 0;
    unsigned long ifOutDiscards   = 0;
    unsigned long ifInDiscards    = 0;
    unsigned long ifOutErrors     = 0;
    unsigned long ifInErrors      = 0;

    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cpmac_priv->drv_hal;
    CPMAC_DEVICE_MIB_T   *p_device_mib = p_cpmac_priv->device_mib;
    CPMAC_DRV_STATS_T    *p_stats      = p_cpmac_priv->stats;
    CPMAC_DEVICE_MIB_T   local_mib;
    CPMAC_DEVICE_MIB_T   *p_local_mib  = &local_mib;

    struct net_device_stats *p_net_dev_stats = &p_cpmac_priv->net_dev_stats;

    int                 len      = 0;
    int  dev_mib_elem_count      = 0;

    /* do not access the hardware if it is in the reset state. */
    if(!test_bit(0, &p_cpmac_priv->set_to_close))
    {
        if(p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "StatsDump", "Get",
 			                 p_local_mib) != 0)
        {
            errPrint("The stats dump for %s is failing.\n", p_dev->name);
            return(ret_val);
        }

        p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "StatsClear", "Set", NULL);

        dev_mib_elem_count = sizeof(CPMAC_DEVICE_MIB_T)/sizeof(unsigned long);

        /* Update the history of the stats. This takes care of any reset of the 
         * device and stats that might have taken place during the life time of 
         * the driver. 
         */
        while(dev_mib_elem_count--)
        {
            *((unsigned long*) p_device_mib + dev_mib_elem_count) += 
            *((unsigned long*) p_local_mib  + dev_mib_elem_count); 
        }
    }

    /* RFC2665, section 3.2.7, page 9 */
    rx_hal_errors          = p_device_mib->ifInFragments       +
	                     p_device_mib->ifInCRCErrors       +
	                     p_device_mib->ifInAlignCodeErrors +
	                     p_device_mib->ifInJabberFrames;

    /* RFC2233 */
    rx_hal_discards        = p_device_mib->ifRxDMAOverruns;

    /* RFC2665, section 3.2.7, page 9 */
    tx_hal_errors          = p_device_mib->ifExcessiveCollisionFrames +
                             p_device_mib->ifLateCollisions           +
                             p_device_mib->ifCarrierSenseErrors       +
                             p_device_mib->ifOutUnderrun;

    /* if not set, the short frames (< 64 bytes) are considered as errors */
    if(!p_cpmac_priv->flags & IFF_PRIV_SHORT_FRAMES)
        rx_hal_errors += p_device_mib->ifInUndersizedFrames;

    /* if not set, the long frames ( > 1518) are considered as errors 
     * RFC2665, section 3.2.7, page 9. */
    if(!p_cpmac_priv->flags & IFF_PRIV_JUMBO_FRAMES)
        rx_hal_errors += p_device_mib->ifInOversizedFrames;

    /* if not in promiscous, then non addr matching frames are discarded */
    /* CPMAC 2.0 Manual Section 2.8.1.14 */
    if(!p_dev->flags & IFF_PROMISC)
    {
        ifInDiscards  +=  p_device_mib->ifInFilteredFrames;
    }

     /* total rx discards = hal discards + driver discards. */
    ifInDiscards  = rx_hal_discards + p_net_dev_stats->rx_dropped;
    ifInErrors    = rx_hal_errors;

    ifOutErrors   = tx_hal_errors;
    ifOutDiscards = p_net_dev_stats->tx_dropped;

    /* Let us update the net device stats struct. To be updated in the later releases.*/
    p_cpmac_priv->net_dev_stats.rx_errors  = ifInErrors;
    p_cpmac_priv->net_dev_stats.collisions = p_device_mib->ifCollisionFrames;

    if(buf == NULL || limit == 0)
    {
        return(0);
    }

    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %ld\n", "ifSpeed",                                 (long)p_cpmac_priv->link_speed);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "dot3StatsDuplexStatus",                   (long)p_cpmac_priv->link_mode);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifAdminStatus",                           (long)(p_dev->flags & IFF_UP ? 1:2));
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOperStatus",                            (long)(((p_dev->flags & IFF_UP) && netif_carrier_ok(p_dev)) ? 1:2));
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifLastChange",                            p_stats->start_tick);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInDiscards",                            ifInDiscards);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInErrors",                              ifInErrors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutDiscards",                           ifOutDiscards);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutErrors",                             ifOutErrors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInGoodFrames",                          p_device_mib->ifInGoodFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInBroadcasts",                          p_device_mib->ifInBroadcasts);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInMulticasts",                          p_device_mib->ifInMulticasts);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInPauseFrames",                         p_device_mib->ifInPauseFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInCRCErrors",                           p_device_mib->ifInCRCErrors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInAlignCodeErrors",                     p_device_mib->ifInAlignCodeErrors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInOversizedFrames",                     p_device_mib->ifInOversizedFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInJabberFrames",                        p_device_mib->ifInJabberFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInUndersizedFrames",                    p_device_mib->ifInUndersizedFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInFragments",                           p_device_mib->ifInFragments);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInFilteredFrames",                      p_device_mib->ifInFilteredFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInQosFilteredFrames",                   p_device_mib->ifInQosFilteredFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifInOctets",                              p_device_mib->ifInOctets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutGoodFrames",                         p_device_mib->ifOutGoodFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutBroadcasts",                         p_device_mib->ifOutBroadcasts);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutMulticasts",                         p_device_mib->ifOutMulticasts);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutPauseFrames",                        p_device_mib->ifOutPauseFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifDeferredTransmissions",                 p_device_mib->ifDeferredTransmissions);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifCollisionFrames",                       p_device_mib->ifCollisionFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifSingleCollisionFrames",                 p_device_mib->ifSingleCollisionFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifMultipleCollisionFrames",               p_device_mib->ifMultipleCollisionFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifExcessiveCollisionFrames",              p_device_mib->ifExcessiveCollisionFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifLateCollisions",                        p_device_mib->ifLateCollisions);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutUnderrun",                           p_device_mib->ifOutUnderrun);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifCarrierSenseErrors",                    p_device_mib->ifCarrierSenseErrors);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifOutOctets",                             p_device_mib->ifOutOctets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if64OctetFrames",                         p_device_mib->if64OctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if65To127POctetFrames",                   p_device_mib->if65To127OctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if128To255OctetFrames",                   p_device_mib->if128To255OctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if256To511OctetFrames",                   p_device_mib->if256To511OctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if512To1023OctetFrames",                  p_device_mib->if512To1023OctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "if1024ToUpOctetFrames",                   p_device_mib->if1024ToUPOctetFrames);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifNetOctets",                             p_device_mib->ifNetOctets);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifRxSofOverruns",                         p_device_mib->ifRxSofOverruns);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifRxMofOverruns",                         p_device_mib->ifRxMofOverruns);
    if(len <= limit)
        len+= sprintf(buf + len, "%-35s: %lu\n", "ifRxDMAOverruns",                         p_device_mib->ifRxDMAOverruns);

    *p_len = len;

    return(0);
}


static int cpmac_p_read_rfc2665_stats(char* buf, char **start, off_t offset, 
		               int count, int *eof, void *data)
{
    int limit = count - 80;
    int len   = 0;
    struct net_device *p_dev = (struct net_device*)data;

    cpmac_p_update_statistics(p_dev, buf, limit, &len);

    *eof = 1;

    return len;
}

static int cpmac_p_read_link(char *buf, char **start, off_t offset, int count, 
                             int *eof, void *data)
{
    int len = 0;

    struct net_device    *p_dev;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv;
    struct net_device    *cpmac_dev_list[cpmac_devices_installed];
    CPMAC_DRV_HAL_INFO_T *p_drv_hal;

    int                  i;
    int                  phy;            /* what phy are we using? */

    len += sprintf(buf+len, "CPMAC devices = %d\n",cpmac_devices_installed);

    p_dev        = last_cpmac_device;

    /* Reverse the the device link list to list eth0,eth1...in correct order */
    for(i=0; i< cpmac_devices_installed; i++)
    {
        cpmac_dev_list[cpmac_devices_installed -(i+1)] = p_dev;
        p_cpmac_priv = p_dev->priv;
        p_dev = p_cpmac_priv->next_device;
    }

    for(i=0; i< cpmac_devices_installed; i++)
    {
        p_dev        = cpmac_dev_list[i];
        p_cpmac_priv = p_dev->priv;
        p_drv_hal    = p_cpmac_priv->drv_hal;

        /*  This prints them out from high to low because of how the devices are linked */
        if(netif_carrier_ok(p_dev))
        {
            p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "PhyNum", "Get", &phy);


            len += sprintf(buf+len,"eth%d: Link State: %s    Phy:0x%x, Speed = %s, Duplex = %s\n",
                           p_cpmac_priv->instance_num, "UP", phy, 
                           (p_cpmac_priv->link_speed == 100000000) ? "100":"10",
                           (p_cpmac_priv->link_mode  == 2) ? "Half":"Full");

        }
        else
            len += sprintf(buf+len,"eth%d: Link State: DOWN\n",p_cpmac_priv->instance_num);

        p_dev = p_cpmac_priv->next_device;
    }

    return len;

}

static int cpmac_p_read_stats(char* buf, char **start, off_t offset, int count,
                              int *eof, void *data)
{
    struct net_device *p_dev = last_cpmac_device;
    int                len   = 0;
    int                limit = count - 80;
    int i;
    struct net_device     *cpmac_dev_list[cpmac_devices_installed];
    CPMAC_PRIVATE_INFO_T  *p_cpmac_priv;
    CPMAC_DEVICE_MIB_T    *p_device_mib;

    /* Reverse the the device link list to list eth0,eth1...in correct order */
    for(i=0; i< cpmac_devices_installed; i++)
    {
        cpmac_dev_list[cpmac_devices_installed - (i+1)] = p_dev;
        p_cpmac_priv = p_dev->priv;
        p_dev = p_cpmac_priv->next_device;
    }

    for(i=0; i< cpmac_devices_installed; i++)
    {
        p_dev = cpmac_dev_list[i];

        if(!p_dev)
          goto proc_error;

        /* Get Stats */
        cpmac_p_update_statistics(p_dev, NULL, 0, NULL);

        p_cpmac_priv = p_dev->priv;
        p_device_mib = p_cpmac_priv->device_mib;

        /* Transmit stats */
        if(len<=limit)
          len+= sprintf(buf+len, "\nCpmac %d, Address %lx\n",i+1, p_dev->base_addr);
        if(len<=limit)
          len+= sprintf(buf+len, " Transmit Stats\n");
        if(len<=limit)
          len+= sprintf(buf+len, "   Tx Valid Bytes Sent        :%lu\n",p_device_mib->ifOutOctets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Frames (Hardware)  :%lu\n",p_device_mib->ifOutGoodFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Frames (Software)  :%lu\n",p_cpmac_priv->net_dev_stats.tx_packets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Broadcast Frames   :%lu\n",p_device_mib->ifOutBroadcasts);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Tx Multicast Frames   :%lu\n",p_device_mib->ifOutMulticasts);
        if(len<=limit)
          len+= sprintf(buf+len, "   Pause Frames Sent          :%lu\n",p_device_mib->ifOutPauseFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Collisions                 :%lu\n",p_device_mib->ifCollisionFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Tx Error Frames            :%lu\n",p_cpmac_priv->net_dev_stats.tx_errors);
        if(len<=limit)
          len+= sprintf(buf+len, "   Carrier Sense Errors       :%lu\n",p_device_mib->ifCarrierSenseErrors);
        if(len<=limit)
          len+= sprintf(buf+len, "\n");


        /* Receive Stats */
        if(len<=limit)
          len+= sprintf(buf+len, "\nCpmac %d, Address %lx\n",i+1,p_dev->base_addr);
        if(len<=limit)
          len+= sprintf(buf+len, " Receive Stats\n");
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Valid Bytes Received    :%lu\n",p_device_mib->ifInOctets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Frames (Hardware)  :%lu\n",p_device_mib->ifInGoodFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Frames (Software)  :%lu\n",p_cpmac_priv->net_dev_stats.rx_packets);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Broadcast Frames   :%lu\n",p_device_mib->ifInBroadcasts);
        if(len<=limit)
          len+= sprintf(buf+len, "   Good Rx Multicast Frames   :%lu\n",p_device_mib->ifInMulticasts);
        if(len<=limit)
          len+= sprintf(buf+len, "   Pause Frames Received      :%lu\n",p_device_mib->ifInPauseFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx CRC Errors              :%lu\n",p_device_mib->ifInCRCErrors);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Align/Code Errors       :%lu\n",p_device_mib->ifInAlignCodeErrors);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Jabbers                 :%lu\n",p_device_mib->ifInOversizedFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Filtered Frames         :%lu\n",p_device_mib->ifInFilteredFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Fragments               :%lu\n",p_device_mib->ifInFragments);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Undersized Frames       :%lu\n",p_device_mib->ifInUndersizedFrames);
        if(len<=limit)
          len+= sprintf(buf+len, "   Rx Overruns                :%lu\n",p_device_mib->ifRxDMAOverruns);
    }


    return len;

 proc_error:
    *eof=1;
    return len;
}

static int cpmac_p_write_stats (struct file *fp, const char * buf, unsigned long count, void * data)
{
	char local_buf[31];
        int  ret_val = 0;

	if(count > 30)
	{
		printk("Error : Buffer Overflow\n");
		printk("Use \"echo 0 > cpmac_stat\" to reset the statistics\n");
		return -EFAULT;
	}

	copy_from_user(local_buf,buf,count);
	local_buf[count-1]='\0'; /* Ignoring last \n char */
        ret_val = count;
	
	if(strcmp("0",local_buf)==0)
	{
            struct net_device *p_dev = last_cpmac_device;
            int i;
            struct net_device     *cpmac_dev_list[cpmac_devices_installed];
            CPMAC_PRIVATE_INFO_T  *p_cpmac_priv;

            /* Valid command */
	    printk("Resetting statistics for CPMAC interface.\n");

            /* Reverse the the device link list to list eth0,eth1...in correct order */
            for(i=0; i< cpmac_devices_installed; i++)
            {
                cpmac_dev_list[cpmac_devices_installed - (i+1)] = p_dev;
                p_cpmac_priv = p_dev->priv;
                p_dev = p_cpmac_priv->next_device;
            }

            for(i=0; i< cpmac_devices_installed; i++)
             {
                p_dev = cpmac_dev_list[i];
                if(!p_dev)
                {
                    ret_val = -EFAULT;
                    break;
                }

                cpmac_p_reset_statistics(p_dev);
            }
	}
	else
	{
		printk("Error: Unknown operation on cpmac statistics\n");
		printk("Use \"echo 0 > cpmac_stats\" to reset the statistics\n");
		return -EFAULT;
	}
	
	return ret_val;
}

static int cpmac_p_reset_statistics(struct net_device *p_dev)
{
    int                       ret_val  = 0;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cpmac_priv->drv_hal;

    memset(p_cpmac_priv->device_mib, 0, sizeof(CPMAC_DEVICE_MIB_T));
    memset(p_cpmac_priv->stats, 0, sizeof(CPMAC_DRV_STATS_T));
    memset(&p_cpmac_priv->net_dev_stats, 0, sizeof(struct net_device_stats));

    p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "StatsClear", "Set", NULL);

    return(ret_val);
}

static int cpmac_p_get_version(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
    int  len                           = 0;
    int  limit                         = count - 80;
    char *hal_version                  = NULL;
    struct net_device *p_dev           = last_cpmac_device;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cpmac_priv->drv_hal;

    p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "Version", "Get", &hal_version);

    len += sprintf(buf+len, "Texas Instruments CPMAC driver version: %s\n", cpmac_version);

    if(len <= limit && hal_version)
        len += sprintf(buf+len, "Texas Instruments CPMAC HAL version: %s\n", hal_version);

    return len;
}

static struct net_device_stats *cpmac_dev_get_net_stats (struct net_device *p_dev)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = (CPMAC_PRIVATE_INFO_T *) p_dev->priv;

    cpmac_p_update_statistics(p_dev, NULL, 0, NULL);

    return &p_cpmac_priv->net_dev_stats;
}

static int cpmac_p_detect_manual_cfg(int link_speed, char* link_mode, int debug)
{
    char *pSpeed = NULL;

    if(debug == 1)
    {
        cpmac_debug_mode = 1;
        dbgPrint("Enabled the debug print.\n");
    }

    if(!link_speed && !link_mode)
    {
        dbgPrint("No manual link params, defaulting to auto negotiation.\n");
        return (0);
    }

    if(!link_speed || (link_speed != 10 && link_speed != 100))
    {
        dbgPrint("Invalid or No value of link speed specified, defaulting to auto speed.\n");
        pSpeed = "auto";
    }
    else if(link_speed == 10)
    {
        g_cfg_start_link_params &= ~(_CPMDIO_100);
        pSpeed = "10 Mbps";
    }
    else
    {
	g_cfg_start_link_params &= ~(_CPMDIO_10);
        pSpeed = "100 Mbps";
    }

    if(!link_mode || (!strcmp(link_mode, "fd") && !strcmp(link_mode, "hd")))
    {
        dbgPrint("Invalid or No value of link mode specified, defaulting to auto mode.\n");
    }
    else if(!strcmp(link_mode, "hd"))
    {
        g_cfg_start_link_params &= ~(_CPMDIO_FD);
    }
    else
    {
        g_cfg_start_link_params &= ~(_CPMDIO_HD);
    }

    dbgPrint("Link is manually set to the speed of %s speed and %s mode.\n", 
              pSpeed, link_mode ? link_mode : "auto");

    return(0);
}

//------------------------------------------------------------------------------
// Call back from the HAL.
//------------------------------------------------------------------------------
static int cpmac_p_process_status_ind(CPMAC_PRIVATE_INFO_T *p_cpmac_priv)
{
    struct net_device    *p_dev        = p_cpmac_priv->owner;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cpmac_priv->drv_hal;
    int status;

    p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "Status", "Get", &status);

    /* We do not reflect the real link status if in loopback.
     * After all, we want the packets to reach the hardware so
     * that Send() should work. */
    if(p_dev->flags & IFF_LOOPBACK)
    {
        dbgPrint("Maintaining the link up loopback for %s.\n", p_dev->name);
	netif_carrier_on(p_dev);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
        avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_ON);
#endif

        return(0);
    }

    if(status & CPMAC_STATUS_ADAPTER_CHECK) /* ???? */
    {
        ; /* what to do ? */
    }    
    else if(status)
    {
        if(!netif_carrier_ok(p_dev))
	{
            netif_carrier_on(p_cpmac_priv->owner);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
            avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_ON);
#endif
	    dbgPrint("Found the Link for the CPMAC instance %s.\n", p_dev->name);
        }

        if(netif_running(p_dev) & netif_queue_stopped(p_dev))
        {
            netif_wake_queue(p_dev);
        }

	p_cpmac_priv->link_speed = status & CPMAC_STATUS_LINK_SPEED ? 100000000:10000000;
	p_cpmac_priv->link_mode  = status & CPMAC_STATUS_LINK_DUPLEX? 3:2;

    }
    else
    {
        if(netif_carrier_ok(p_dev))
	{
	    /* do we need to register synchronization issues with stats here. */
	    p_cpmac_priv->link_speed        = 100000000;
	    p_cpmac_priv->link_mode         = 1;

	    netif_carrier_off(p_dev);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
            avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_OFF);
#endif

	    dbgPrint("Lost the Link for the CPMAC for %s.\n", p_dev->name);
	}

        if(!netif_queue_stopped(p_dev))
        {
            netif_stop_queue(p_dev);     /* So that kernel does not keep on xmiting pkts. */
        }
    }

    return(0);
}

//-----------------------------------------------------------------------------
// Timer related private functions.
//-----------------------------------------------------------------------------
static int cpmac_p_timer_init(CPMAC_PRIVATE_INFO_T *p_cpmac_priv)
{
    struct timer_list *p_timer = p_cpmac_priv->timer;

    init_timer(p_timer);
    
    p_timer                    = p_cpmac_priv->timer + TICK_TIMER;
    p_timer->expires           = 0;
    p_timer->data              = (unsigned long)p_cpmac_priv;
    p_timer->function          = cpmac_p_tick_timer_expiry;

    return(0);
}

#if 0
static int cpmac_timer_cleanup(CPMAC_PRIVATE_INFO_T *p_cpmac_priv)
{
    struct timer_list *p_timer;

    p_timer = p_cpmac_priv->timer + TICK_TIMER;
   
    /* use spin lock to establish synchronization with the dispatch */
    if(p_timer->function) del_timer_sync(p_timer);
    p_timer->function = NULL;

    return (0);
}
#endif

static int cpmac_p_start_timer(struct timer_list *p_timer, unsigned int delay_ticks)
{
    p_timer->expires        = jiffies + delay_ticks;

    if(p_timer->function)
    {
        add_timer(p_timer);
    }

    return(0);
} 

static void cpmac_p_tick_timer_expiry(unsigned long p_cb_param)
{
    CPMAC_PRIVATE_INFO_T   *p_cpmac_priv = (CPMAC_PRIVATE_INFO_T*) p_cb_param;
    CPMAC_DRV_HAL_INFO_T   *p_drv_hal    = p_cpmac_priv->drv_hal; 
    struct timer_list       *p_timer     = p_cpmac_priv->timer + TICK_TIMER;

    if(test_bit(0, &p_cpmac_priv->set_to_close))
    {
        return;
    }

    p_drv_hal->hal_funcs->Tick(p_drv_hal->hal_dev);
  
    cpmac_p_start_timer(p_timer, p_cpmac_priv->delay_ticks);
}

static int cpmac_p_stop_timer(struct timer_list *p_timer)
{
    /* Ideally we need to a set flag indicating not to start the timer again
       before del_timer_sync() is called up. But here we assume that the 
       caller has set the p_cpmac_priv->set_to_close (ok for now). */
    del_timer_sync(p_timer);
    
    return(0);
}

//------------------------------------------------------------------------------
// Device configuration and setup related private functions.
//------------------------------------------------------------------------------
static int cpmac_p_probe_and_setup_device(CPMAC_PRIVATE_INFO_T *p_cpmac_priv, 
		                          unsigned long *p_dev_flags)
{
    CPMAC_DRV_HAL_INFO_T *p_drv_hal   = p_cpmac_priv->drv_hal;
    HAL_FUNCTIONS        *p_hal_funcs = p_drv_hal->hal_funcs;
    HAL_DEVICE           *p_hal_dev   = p_drv_hal->hal_dev;
    CPMAC_ABILITY_INFO_T *p_capability= p_cpmac_priv->ability_info;
    unsigned int         val          = 0;
    int                  channel      = 0;

    p_cpmac_priv->flags                  = 0;

    p_capability->promiscous             = CFG_PROMISCOUS;
    p_capability->broadcast              = CFG_BROADCAST;
    p_capability->multicast              = CFG_MULTICAST;
    p_capability->all_multi              = CFG_ALL_MULTI;
    p_capability->jumbo_frames           = CFG_JUMBO_FRAMES;
    p_capability->short_frames           = CFG_SHORT_FRAMES;
    p_capability->auto_negotiation       = CFG_AUTO_NEGOTIATION;
    p_capability->link_speed             = cfg_start_link_speed;
    p_capability->loop_back              = CFG_LOOP_BACK;
    p_capability->tx_flow_control        = CFG_TX_FLOW_CNTL;
    p_capability->rx_flow_control        = CFG_RX_FLOW_CNTL;
    p_capability->tx_pacing              = CFG_TX_PACING;
    p_capability->rx_pass_crc            = CFG_RX_PASS_CRC;
    p_capability->qos_802_1q             = CFG_QOS_802_1Q;
    p_capability->tx_num_chan            = CFG_TX_NUM_CHAN;

    /* Lets probe the device for the configured capabilities (netdev specific).*/ 

    /* Following are set in the set_multi_list, when indicated by the kernel 
     * Promiscous and all multi.
     */

    if(p_capability->broadcast)
    {
        channel = 0;
	val     = 1;
        if((p_hal_funcs->Control(p_hal_dev, pszRX_BROAD_EN, pszSet, &val) == 0) && 
           (p_hal_funcs->Control(p_hal_dev, pszRX_BROAD_CH, pszSet, &channel) == 0))
	    *p_dev_flags |= IFF_BROADCAST;
	else
	    p_capability->broadcast = 0; /* no broadcast capabilities */
    }

    if(p_capability->multicast)
    {
        val     = 1;
	channel = 0;
	if((p_hal_funcs->Control(p_hal_dev, pszRX_MULT_EN, pszSet, &val) == 0) &&
	   (p_hal_funcs->Control(p_hal_dev, pszRX_MULT_CH, pszSet, &channel) == 0))
	    *p_dev_flags |= IFF_MULTICAST;
	else
	{
	    p_capability->multicast = 0; 
	    p_capability->all_multi = 0; /* no multicast, no all-multi. */
	}
    }

    if(p_capability->loop_back)
    {
        ;  /* We do not put the device in loopback, if required use ioctl */
    }
    
    /* Lets probe the device for the configured capabilities (Non net device specific).*/

    if(p_capability->jumbo_frames)
    {
	val = 0;
        if(p_hal_funcs->Control(p_hal_dev, pszRX_NO_CHAIN, pszSet, &val) == 0)
            p_cpmac_priv->flags |= IFF_PRIV_JUMBO_FRAMES;
	else
	    p_capability->jumbo_frames = 0;
    }

    if(p_capability->short_frames)
    {
	val = 1;
        if(p_hal_funcs->Control(p_hal_dev, pszRX_CSF_EN, pszSet, &val) == 0)
            p_cpmac_priv->flags |= IFF_PRIV_SHORT_FRAMES;
	else
	    p_capability->short_frames = 0;
    }            

    val = g_cfg_start_link_params;

    if( avalanche_is_mdix_on_chip() )
    {
        val |= _CPMDIO_AUTOMDIX;
    }

    if(p_hal_funcs->Control(p_hal_dev,pszMdioConnect,pszSet, &val) !=0)
    {
        p_capability->link_speed = 0;
    }
    else
    {
	if(g_cfg_start_link_params & (_CPMDIO_100 | _CPMDIO_HD | _CPMDIO_FD | _CPMDIO_10))
            p_cpmac_priv->flags |= IFF_PRIV_AUTOSPEED;
	else if(g_cfg_start_link_params & (_CPMDIO_100 | _CPMDIO_HD))
	    p_cpmac_priv->flags |= IFF_PRIV_LINK100_HD;
	else if(g_cfg_start_link_params & (_CPMDIO_100 | _CPMDIO_FD))
	    p_cpmac_priv->flags |= IFF_PRIV_LINK100_FD;
	else if(g_cfg_start_link_params & (_CPMDIO_10 | _CPMDIO_HD))
	    p_cpmac_priv->flags |= IFF_PRIV_LINK10_HD;
	else if(g_cfg_start_link_params & (_CPMDIO_10 | _CPMDIO_FD))
	    p_cpmac_priv->flags |= IFF_PRIV_LINK10_FD;
	else
	    ;
    }

    if(p_capability->tx_flow_control)
    {
	val = 1;
        if(p_hal_funcs->Control(p_hal_dev,pszTX_FLOW_EN, pszSet, &val) ==0)
            p_cpmac_priv->flags |= IFF_PRIV_TX_FLOW_CNTL;
	else
            p_capability->tx_flow_control = 0;
    }

    if(p_capability->rx_flow_control)
    {
	val = 1;
        if(p_hal_funcs->Control(p_hal_dev, pszRX_FLOW_EN, pszSet, &val) ==0)
	    p_cpmac_priv->flags |= IFF_PRIV_RX_FLOW_CNTL;
	else
            p_capability->rx_flow_control = 0;
    }

    if(p_capability->tx_pacing)
    {
	val = 1;
        if(p_hal_funcs->Control(p_hal_dev, pszTX_PACE, pszSet, &val) ==0)
            p_cpmac_priv->flags |= IFF_PRIV_TX_PACING;
	else
	    p_capability->tx_pacing = 0;
    }

    if(p_capability->rx_pass_crc)
    {
	val = 1;
	if(p_hal_funcs->Control(p_hal_dev, pszRX_PASS_CRC, pszSet, &val) == 0)
	    p_cpmac_priv->flags |= IFF_PRIV_RX_PASS_CRC;
	else                                  
            p_capability->rx_pass_crc = 0;
    }

    if(p_capability->qos_802_1q)
    {
	val  = 1;
        if(p_hal_funcs->Control(p_hal_dev, pszRX_QOS_EN, pszSet, &val) == 0)
            p_cpmac_priv->flags |= IFF_PRIV_8021Q_EN;
	else
	{
	    p_capability->qos_802_1q = 0;
	    p_capability->tx_num_chan= 1;
        }
    }

    if(p_capability->tx_num_chan > 1)
    {
	int cfg_tx_num_chan = p_capability->tx_num_chan;
        val = 0;
#ifdef TEST
	if(p_hal_funcs->Control(p_hal_dev, pszTX_NUM_CH, pszGet, &val) == 0)
	    cfg_tx_num_chan = cfg_tx_num_chan > val ? val : cfg_tx_num_chan;
	else 
	    cfg_tx_num_chan = 1;
#endif
	p_capability->tx_num_chan = cfg_tx_num_chan;
    }

    return(0);
}

static int cpmac_p_setup_driver_params(CPMAC_PRIVATE_INFO_T *p_cpmac_priv)
{
   int i=0;
   int threshold = CFG_TX_NUM_BUF_SERVICE;

   char *tx_threshold_ptr = prom_getenv("threshold");
   
    CPMAC_TX_CHAN_INFO_T *p_tx_chan_info = p_cpmac_priv->tx_chan_info;
    CPMAC_RX_CHAN_INFO_T *p_rx_chan_info = p_cpmac_priv->rx_chan_info;
    CPMAC_ABILITY_INFO_T *p_capability   = p_cpmac_priv->ability_info;

    /* Timer stuff */
    p_cpmac_priv->timer_count      = 1; /* should be < or = the MAX TIMER */
    p_cpmac_priv->timer_created    = 0;
    p_cpmac_priv->timer_access_hal = 1;

    for(i=0; i < MAX_TIMER; i++)
        p_cpmac_priv->timer[i].function = NULL;

    p_cpmac_priv->enable_802_1q  = p_capability->qos_802_1q;  

    /* Tx channel related.*/
    p_tx_chan_info->cfg_chan     = p_capability->tx_num_chan;
    p_tx_chan_info->opened_chan  = 0;
    
    if(tx_threshold_ptr)
       threshold = simple_strtol(tx_threshold_ptr, (char **)NULL, 10);
    
    if((threshold <= 0) && tx_threshold_ptr) /* If threshold set to 0 then Enable the TX interrupt */
    {
       threshold  = CFG_TX_NUM_BUF_SERVICE;
       p_tx_chan_info->tx_int_disable = 0;
       
    }
    else
    {
       p_tx_chan_info->tx_int_disable = CFG_TX_INT_DISABLE;
    }

    for(i=0; i < MAX_TX_CHAN; i++)
    {
       


       p_tx_chan_info->chan[i].state         = CHAN_CLOSE; 
       p_tx_chan_info->chan[i].num_BD        = CFG_TX_NUM_BUF_DESC;  
       p_tx_chan_info->chan[i].buffer_size   = cpmac_max_frame_size;
       p_tx_chan_info->chan[i].buffer_offset = CFG_TX_BUF_OFFSET;



       p_tx_chan_info->chan[i].service_max   = threshold;
    }
    
    if (p_tx_chan_info->tx_int_disable)
       printk("Cpmac driver Disable TX complete interrupt setting threshold to %d.\n",threshold);
    else
       printk("Cpmac driver Enable TX complete interrupt\n");

    
    /* Assuming just one rx channel for now */
    p_rx_chan_info->cfg_chan            = 1;
    p_rx_chan_info->opened_chan         = 0;
    p_rx_chan_info->chan->state         = CHAN_CLOSE;
    p_rx_chan_info->chan->num_BD        = CFG_RX_NUM_BUF_DESC;
    p_rx_chan_info->chan->buffer_size   = cpmac_max_frame_size;
    p_rx_chan_info->chan->buffer_offset = CFG_RX_BUF_OFFSET;
    p_rx_chan_info->chan->service_max   = CFG_RX_NUM_BUF_SERVICE;

    /* Set as per RFC 2665 */
    p_cpmac_priv->link_speed     = 100000000; 
    p_cpmac_priv->link_mode      = 1;

    p_cpmac_priv->loop_back      = 0;

    return(0);
}

inline static int cpmac_p_rx_buf_setup(CPMAC_RX_CHAN_INFO_T *p_rx_chan)
{
    /* Number of ethernet packets & max pkt length */
    p_rx_chan->chan->tot_buf_size  = p_rx_chan->chan->buffer_size   + 
                                     2*(CONTROL_802_1Q_SIZE)        +
	                             p_rx_chan->chan->buffer_offset +
                                     ADD_FOR_4BYTE_ALIGN(p_rx_chan->chan->buffer_offset & 0x3);

    p_rx_chan->chan->tot_reserve_bytes = CONTROL_802_1Q_SIZE + 
                                         p_rx_chan->chan->buffer_offset +
	                                 L3_ALIGN(p_rx_chan->chan->buffer_offset & 0x3);

    return(0);
}

//-----------------------------------------------------------------------------
// Net device related private functions.
//-----------------------------------------------------------------------------

/***************************************************************
 *	cpmac_dev_init
 *
 *	Returns:
 *		0 on success, error code otherwise.
 *	Parms:
 *		dev	The structure of the device to be
 *			init'ed.
 *
 *	This function completes the initialization of the
 *	device structure and driver.  It reserves the IO
 *	addresses and assignes the device's methods.
 *
 *	
 **************************************************************/

static int cpmac_dev_init(struct net_device *p_dev)
{
    int retVal = -1;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    int instance_num                   = p_cpmac_priv->instance_num;
    unsigned long net_flags = 0;
    char *mac_name          = NULL;
    char *mac_string        = NULL;

    CPMAC_TX_CHAN_INFO_T *p_tx_chan_info;
    CPMAC_RX_CHAN_INFO_T *p_rx_chan_info;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal;
    int i;

    int mem_size =    sizeof(CPMAC_DRV_HAL_INFO_T)
		    + sizeof(CPMAC_TX_CHAN_INFO_T)
		    + sizeof(CPMAC_RX_CHAN_INFO_T)
		    + sizeof(CPMAC_ABILITY_INFO_T)
                    + sizeof(CPMAC_DEVICE_MIB_T)
                    + sizeof(CPMAC_DRV_STATS_T);


#if defined(CONFIG_MIPS_SEAD2)
    int prev_reset_val = RESET_REG_PRCR;
    /* Bring the module out of reset */
    RESET_REG_PRCR |= temp_reset_value[p_cpmac_priv->instance_num];     

    /* Read the version id of the device to check if the device really exists */
    if( VERSION(temp_base_address[p_cpmac_priv->instance_num]) == 0)
    {
        printk(" CPMAC:Device not found\n");
	RESET_REG_PRCR = prev_reset_val;
        return -ENODEV;
    }

    RESET_REG_PRCR = prev_reset_val;
#endif


    if((p_drv_hal = kmalloc(mem_size,  GFP_KERNEL)) == NULL)
    {
        errPrint("Failed to allocate memory; rewinding.\n");
        return(-1);
    }

    memset(p_drv_hal, 0, mem_size);

    /* build the cpmac private object */
    p_cpmac_priv->drv_hal       = p_drv_hal;
    p_cpmac_priv->tx_chan_info  = p_tx_chan_info
                                = (CPMAC_TX_CHAN_INFO_T*)((char*)p_drv_hal 
				  + sizeof(CPMAC_DRV_HAL_INFO_T));
    p_cpmac_priv->rx_chan_info  = p_rx_chan_info 
                                = (CPMAC_RX_CHAN_INFO_T*)((char *)p_tx_chan_info 
				  + sizeof(CPMAC_TX_CHAN_INFO_T));
    p_cpmac_priv->ability_info  = (CPMAC_ABILITY_INFO_T *)((char *)p_rx_chan_info 
		                  + sizeof(CPMAC_RX_CHAN_INFO_T));
    p_cpmac_priv->device_mib    = (CPMAC_DEVICE_MIB_T *)((char *)p_cpmac_priv->ability_info 
	                          + sizeof(CPMAC_ABILITY_INFO_T));
    p_cpmac_priv->stats         = (CPMAC_DRV_STATS_T *)((char *)p_cpmac_priv->device_mib   
		                  + sizeof(CPMAC_DEVICE_MIB_T));

    p_drv_hal->owner            = p_cpmac_priv;


    switch(instance_num)
    {

        case 0:
            mac_name="maca";

            /* Also setting port information */
            p_dev->if_port =  AVALANCHE_CPMAC_LOW_PORT_ID;            

            break;

        case 1:
            mac_name="macb";

            /* Also setting port information */
            p_dev->if_port =  AVALANCHE_CPMAC_HIGH_PORT_ID;            

            break;
    }

    if(mac_name)
        mac_string=prom_getenv(mac_name);

    if(!mac_string)
    {
        mac_string="08.00.28.32.06.02";
        printk("Error getting mac from Boot enviroment for %s\n",p_dev->name);
        printk("Using default mac address: %s\n",mac_string);
        if(mac_name)
        {
            printk("Use Bootloader command:\n");
            printk("    setenv %s xx.xx.xx.xx.xx.xx\n","<env_name>");
            printk("to set mac address\n");
        }
    }

    str2eaddr(p_cpmac_priv->mac_addr,mac_string);

    for (i=0; i <= ETH_ALEN; i++)
    {
        /* This sets the hardware address */
	p_dev->dev_addr[i] = p_cpmac_priv->mac_addr[i]; 
    }

    p_cpmac_priv->set_to_close          = 1;
    p_cpmac_priv->non_data_irq_expected = 0;

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    if((p_cpmac_priv->led_handle = avalanche_led_register("cpmac", instance_num)) == NULL)
    {
        errPrint("Could not allocate handle for CPMAC[%d] LED.\n", instance_num);
        goto cpmac_init_mod_error;
    }
#endif

    if(cpmac_drv_init_module(p_drv_hal, p_dev, instance_num) != 0)
    {
        errPrint("Could not initialize the HAL for %s.\n", p_dev->name);
        goto cpmac_init_mod_error;
    } 

    /* initialize the CPMAC device */
    if (cpmac_drv_init(p_drv_hal) == -1)
    {
	errPrint("HAL init failed for %s.\n", p_dev->name);
	goto cpmac_init_device_error;
    }

    if(cpmac_p_probe_and_setup_device(p_cpmac_priv, &net_flags) == -1)
    {
        errPrint("Failed to configure up %s.\n", p_dev->name);
        goto cpmac_init_device_error;
    }

    if(cpmac_p_setup_driver_params(p_cpmac_priv) == -1)
    {
        errPrint("Failed to set driver parameters for %s.\n", p_dev->name);
        goto cpmac_init_device_error;
    }

    cpmac_p_rx_buf_setup(p_rx_chan_info);

    /* initialize the timers for the net device */
    if(cpmac_p_timer_init(p_cpmac_priv) == -1)
    {
        errPrint("Failed to set timer(s) for %s.\n", p_dev->name);
        goto cpmac_timer_init_error;
    }

    p_dev->addr_len           = 6;

    p_dev->open               = &cpmac_dev_open;    /*  i.e. Start Device  */
    p_dev->hard_start_xmit    = &cpmac_dev_tx;
    p_dev->stop               = &cpmac_dev_close;
    p_dev->get_stats          = &cpmac_dev_get_net_stats;

    p_dev->set_multicast_list = &cpmac_dev_mcast_set;
    p_dev->set_mac_address    = cpmac_dev_set_mac_addr;
    /* Knocking off the default broadcast and multicast flags. Allowing the 
       device configuration to control the flags. */
    p_dev->flags &= ~(IFF_BROADCAST | IFF_MULTICAST);
    p_dev->flags |= net_flags;

    netif_carrier_off(p_dev);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_OFF);
#endif

    /* Tasklet is initialized at the isr registeration time. */
    p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "CpmacBase", "Get", &p_dev->base_addr);
    p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "CpmacSize", "Get", &p_cpmac_priv->dev_size);

    request_mem_region(p_dev->base_addr, p_cpmac_priv->dev_size, p_dev->name);

    retVal = 0;

    if(g_init_enable_flag)
        cpmac_p_dev_enable(p_dev);

    return(retVal);

cpmac_timer_init_error:
cpmac_init_device_error  :
    cpmac_drv_cleanup(p_drv_hal);

cpmac_init_mod_error:
    kfree(p_drv_hal);

    return (retVal);

} /* cpmac_dev_init */


/***************************************************************
 *	cpmac_p_dev_enable
 *
 *	Returns:
 *		0 on success, error code otherwise.
 *	Parms:
 *		dev	Structure of device to be opened.
 *
 *	This routine puts the driver and CPMAC adapter in a
 *	state where it is ready to send and receive packets.
 *	
 *
 **************************************************************/
int cpmac_p_dev_enable( struct net_device *p_dev)
{
    int ret_val = 0;
    int channel = 0;
 
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal      = p_cpmac_priv->drv_hal;
    CPMAC_RX_CHAN_INFO_T *p_rx_chan_info = p_cpmac_priv->rx_chan_info;
    int max_length                       = p_rx_chan_info->chan->tot_buf_size;

    p_cpmac_priv->set_to_close = 0;

    if((ret_val = cpmac_drv_start(p_drv_hal, p_cpmac_priv->tx_chan_info, 
				  p_cpmac_priv->rx_chan_info, CHAN_SETUP))==-1)
    {
        errPrint("%s error: failed to start the device.\n", p_dev->name);
        ret_val = -1;
    }
    else if(p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev,"RX_UNICAST_SET",  
					  "Set", &channel)!=0)
    {
        errPrint("%s error: device chan 0 could not be enabled.\n", p_dev->name);	
	ret_val = -1;
    }
    else if(p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, pszRX_MAXLEN, pszSet, &max_length) != 0)
    {
        errPrint(" CPMAC registers can't be written \n");
        ret_val = -1;
    }
    else if(p_drv_hal->hal_funcs->Control(p_drv_hal->hal_dev, "TxIntDisable", "Set", 
            &p_cpmac_priv->tx_chan_info->tx_int_disable) != 0)
    {    
        errPrint(" CPMAC registers can't be written \n");
        ret_val = -1;
    }
    else
    {
        ; // Every thing went OK. 
    }
    
    return(ret_val);
} /* cpmac_dev_enable */


static int cpmac_dev_open(struct net_device *p_dev)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_dev->priv;
    CPMAC_ISR_INFO_T     *p_isr_cb_param = &p_cpmac_priv->cpmac_isr;

    if(!g_init_enable_flag)
        cpmac_p_dev_enable(p_dev);
   
    if(request_irq(p_isr_cb_param->intr, cpmac_hal_isr, SA_INTERRUPT, 
                   "Cpmac Driver", p_isr_cb_param))
    {
        errPrint("Failed to register the irq %d for Cpmac %s.\n",
                  p_isr_cb_param->intr, p_dev->name); 
        return (-1);          
    }
    
    netif_start_queue(p_dev);

    MOD_INC_USE_COUNT;
    p_cpmac_priv->stats->start_tick = jiffies;
    dbgPrint("Started the network queue for %s.\n", p_dev->name);    
    return(0);
}

/***************************************************************
 *	cpmac_p_dev_disable
 *  
 * 	Returns:
 *		An error code.
 *	Parms:
 *		dev	The device structure of the device to
 *			close.
 *
 *	This function shuts down the adapter.  
 *
 **************************************************************/
int cpmac_p_dev_disable(struct net_device *p_dev)
{
    int ret_val = 0;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cpmac_priv->drv_hal;

    set_bit(0, &p_cpmac_priv->set_to_close);
    set_bit(0, &p_cpmac_priv->non_data_irq_expected);
   
    /* The driver does not re-schedule the tasklet after kill is called. So, this 
       should take care of the bug in the kernel. */
    tasklet_kill(&p_cpmac_priv->cpmac_isr.tasklet);
 
    if(cpmac_drv_stop(p_drv_hal, p_cpmac_priv->tx_chan_info,
		      p_cpmac_priv->rx_chan_info, 
		      CHAN_TEARDOWN | FREE_BUFFER | BLOCKING | COMPLETE) == -1)
    {
        ret_val = -1;
    }
    else
    {
        /* hope that the HAL closes down the tick timer.*/

        dbgPrint("Device %s Closed.\n", p_dev->name);
        p_cpmac_priv->stats->start_tick = jiffies;

        p_cpmac_priv->link_speed        = 100000000;
        p_cpmac_priv->link_mode         = 1;
        netif_carrier_off(p_dev);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
        avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_OFF);
#endif

        clear_bit(0, &p_cpmac_priv->non_data_irq_expected);

    }
    
    return (ret_val);

} /* cpmac_dev_close */


/***************************************************************
 *	cpmac_dev_close
 *  
 * 	Returns:
 *		An error code.
 *	Parms:
 *		dev	The device structure of the device to
 *			close.
 *
 *	This function shuts down the adapter.  
 *
 **************************************************************/
static int cpmac_dev_close(struct net_device *p_dev)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_dev->priv;
    CPMAC_ISR_INFO_T     *p_isr_cb_param = &p_cpmac_priv->cpmac_isr;

    /* inform the upper layers. */
    netif_stop_queue(p_dev);

    if(!g_init_enable_flag)
        cpmac_p_dev_disable(p_dev);
    else
        free_irq(p_isr_cb_param->intr, p_isr_cb_param);

    MOD_DEC_USE_COUNT;

    return(0);
}

static void cpmac_dev_mcast_set(struct net_device *p_dev)
{
    CPMAC_PRIVATE_INFO_T    *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T    *p_drv_hal    = p_cpmac_priv->drv_hal;
    CPMAC_ABILITY_INFO_T    *p_capability = p_cpmac_priv->ability_info;
    HAL_FUNCTIONS           *p_hal_funcs  = p_drv_hal->hal_funcs;
    HAL_DEVICE              *p_hal_dev    = p_drv_hal->hal_dev;
    int                     val           = 1;
    int                     channel       = 0;

#if defined (CONFIG_MIPS_AVALANCHE_LED)    
    if(netif_carrier_ok(p_dev))
       avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_LINK_ON);
#endif

    if(p_dev->flags & IFF_PROMISC)
    {
        if(p_capability->promiscous)
        {       
            /* multi mode in the HAL, check this */
	    val = 0;
	    p_hal_funcs->Control(p_hal_dev, pszRX_MULTI_ALL, "Clear", &val);

	    val = 1;
            /* set the promiscous mode in the HAL */
            p_hal_funcs->Control(p_hal_dev, pszRX_CAF_EN, pszSet, &val);
            p_hal_funcs->Control(p_hal_dev, pszRX_PROM_CH, pszSet, &channel);

       	    dbgPrint("%s set in the Promisc mode.\n", p_dev->name);	
	}
        else
        {
	    errPrint("%s not configured for Promisc mode.\n", p_dev->name);
        }
    }
    else if(p_dev->flags & IFF_ALLMULTI)
    {
        if(p_capability->all_multi)
        {	
	    val = 0;
	    /* disable the promiscous mode in the HAL */
	    p_hal_funcs->Control(p_hal_dev, pszRX_CAF_EN, "Clear", &val);

	    val = 1;
            /* set the all multi mode in the HAL */
	    p_hal_funcs->Control(p_hal_dev, pszRX_MULTI_ALL, pszSet, &val);
	    p_hal_funcs->Control(p_hal_dev, pszRX_MULT_CH, pszSet, &channel);

            dbgPrint("%s has been set to the ALL_MULTI mode.\n", p_dev->name);
        }
        else
        {
	    errPrint("%s not configured for ALL MULTI mode.\n", p_dev->name);
        }
    }
    else if(p_dev->mc_count)
    {
        if(p_capability->multicast)
        {
	    struct dev_mc_list *p_dmi = p_dev->mc_list;
            int    count;

            val = 0;
            /* clear all the previous data, we are going to populate new ones.*/
	    p_hal_funcs->Control(p_hal_dev, pszRX_MULTI_ALL, "Clear", &val);
	    /* disable the promiscous mode in the HAL */
	    p_hal_funcs->Control(p_hal_dev, pszRX_CAF_EN, pszSet, &val);

	    for(count = 0; count < p_dev->mc_count; count++, p_dmi = p_dmi->next)
	    {
               p_hal_funcs->Control(p_hal_dev, "RX_MULTI_SINGLE", "Set", p_dmi->dmi_addr); 
	    }

            dbgPrint("%s configured for %d multicast addresses.\n", p_dev->name, p_dev->mc_count);
        }
        else
        {
	    errPrint("%s has not been configuted for multicast handling.\n", p_dev->name);
        }
    }
    else
    {
       val = 0;
       /* clear all the previous data, we are going to populate new ones.*/
       p_hal_funcs->Control(p_hal_dev, pszRX_MULTI_ALL, "Clear", &val);
       /* disable the promiscous mode in the HAL */
       p_hal_funcs->Control(p_hal_dev, pszRX_CAF_EN, pszSet, &val);
       dbgPrint("Dev set to  Unicast mode.\n");
    }
}

static int cpmac_dev_set_mac_addr(struct net_device *p_dev,void * addr)
{
    CPMAC_PRIVATE_INFO_T    *p_cpmac_priv = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T    *p_drv_hal    = p_cpmac_priv->drv_hal;
    HAL_FUNCTIONS           *p_hal_funcs  = p_drv_hal->hal_funcs;
    HAL_DEVICE              *p_hal_dev    = p_drv_hal->hal_dev;
    struct sockaddr *sa = addr;

   memcpy(p_cpmac_priv->mac_addr,sa->sa_data,p_dev->addr_len);
   memcpy(p_dev->dev_addr,sa->sa_data,p_dev->addr_len);
   p_hal_funcs->Control(p_hal_dev, pszMacAddr, pszSet, p_cpmac_priv->mac_addr);   

	return 0;

}

/* VLAN is handled by vlan/vconfig support. Here, we just check for the 
 * 802.1q configuration of the device and en-queue the packet accordingly.
 * We do not do any 802.1q processing here.
 */
static int cpmac_dev_tx( struct sk_buff *skb, struct net_device *p_dev)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv  = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal     = p_cpmac_priv->drv_hal;
    int channel                         = 0;
    int ret_val                         = 0;
    FRAGLIST             send_frag_list[1];

#ifdef CPMAC_8021Q_SUPPORT
    if(skb->len < TCI_END_OFFSET)
    {
        /* Whee, frame shorter than 14 bytes !! We need to copy 
	 * fragments to understand the frame. Too much work. 
	 * Hmm, dump it. */

        /* Free the buffer */
	goto cpmac_dev_tx_drop_pkt;
    }

    /* 802.1p/q stuff */
    if(IS_802_1Q_FRAME(skb->data + TPID_START_OFFSET)) 
    {
        /* IEEE 802.1q, section 8.8 and section 8.11.9 */
        if(!p_cpmac_priv->enable_802_1q) 
        {
           /* free the buffer */
           goto cpmac_dev_tx_drop_pkt;
        }		

        channel = GET_802_1P_CHAN(p_cpmac_priv->tx_chan_info->opened_chan,
              		          skb->data[TCI_START_OFFSET]);

    }
    /* sending a non 802.1q frame, when configured for 802.1q: dump it.*/
    else if(p_cpmac_priv->enable_802_1q)
    {
         /* free the buffer */
	 goto cpmac_dev_tx_drop_pkt;
    }
    else
    {
        ;/* it is the good old non 802.1q */
    }
#endif

    send_frag_list->len  = skb->len;
    send_frag_list->data = skb->data;

#ifdef CPMAC_TEST
    xdump(skb->data, skb->len, "send");
#endif

    dma_cache_wback_inv((unsigned long)skb->data, skb->len);    

    if(p_drv_hal->hal_funcs->Send(p_drv_hal->hal_dev, send_frag_list, 1, 
			          skb->len, skb, channel) != 0)
    {
	/* code here to stop the queue, when allowing tx timeout, perhaps next release.*/
        p_cpmac_priv->net_dev_stats.tx_errors++;  
#ifndef TI_SLOW_PATH
       /* Free the skb in case of Send return error */
        dev_kfree_skb_any(skb);
        p_cpmac_priv->net_dev_stats.tx_dropped++;
        return 0;
#endif
        goto cpmac_dev_tx_drop_pkt; 
    }

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_TX_ACTIVITY);
#endif

    return(ret_val);

cpmac_dev_tx_drop_pkt:

    p_cpmac_priv->net_dev_stats.tx_dropped++;
    ret_val = -1;
    return (ret_val);

} /*cpmac_dev_tx */


//------------------------------------------------------------------------------
// Public functions : Called by outsiders to this file.
//------------------------------------------------------------------------------


void *cpmac_hal_malloc_buffer(unsigned int size, void* mem_base, unsigned int mem_range,
	                      OS_SETUP *p_os_setup, HAL_RECEIVEINFO *HalReceiveInfo, 
 	                      OS_RECEIVEINFO **osReceiveInfo, OS_DEVICE *p_dev)
{
    CPMAC_RX_CHAN_INFO_T *p_rx_chan_info = (CPMAC_RX_CHAN_INFO_T *)p_os_setup;
    int                  tot_buf_size    = p_rx_chan_info->chan->tot_buf_size;
    int             tot_reserve_bytes    = p_rx_chan_info->chan->tot_reserve_bytes;
    struct sk_buff  *p_skb;
    void             *ret_ptr;

    /* use TI SKB private pool */
      p_skb = ti_alloc_skb(tot_buf_size,GFP_ATOMIC);

    if(p_skb == NULL)
    {
        dbgPrint("Failed to allocate skb for %s.\n", ((struct net_device*)p_dev)->name);
        return (NULL);
    }

    p_skb->dev = p_dev;
    skb_reserve(p_skb, tot_reserve_bytes);

    *osReceiveInfo = p_skb;

    ret_ptr = skb_put(p_skb, p_rx_chan_info->chan->buffer_size);

    return(ret_ptr);
}

void cpmac_hal_isr(int irq, void *p_param, struct pt_regs *regs)
{
    CPMAC_ISR_INFO_T      *p_cb_param    = (CPMAC_ISR_INFO_T*) p_param;
    CPMAC_DRV_HAL_INFO_T  *p_drv_hal     = p_cb_param->owner;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv   = p_drv_hal->owner;
    int                   pkts_to_handle = 0;

    if(p_cpmac_priv->non_data_irq_expected)
    {
        p_cb_param->hal_isr(p_drv_hal->hal_dev, &pkts_to_handle);
        p_drv_hal->hal_funcs->PacketProcessEnd(p_drv_hal->hal_dev);
    }
    else if(!p_cpmac_priv->set_to_close)
        tasklet_schedule(&((CPMAC_ISR_INFO_T*) p_param)->tasklet);
    else
        ; // back off from doing anything more. We are closing down.
}

void cpmac_handle_tasklet(unsigned long data)
{
    CPMAC_ISR_INFO_T     *p_cb_param   = (CPMAC_ISR_INFO_T*) data;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal    = p_cb_param->owner;
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_drv_hal->owner;
    int                  pkts_to_handle;

    p_cb_param->hal_isr(p_drv_hal->hal_dev, &pkts_to_handle);

    if(test_bit(0, &p_cpmac_priv->non_data_irq_expected) || !pkts_to_handle)
        p_drv_hal->hal_funcs->PacketProcessEnd(p_drv_hal->hal_dev);
    else if(!test_bit(0, &p_cpmac_priv->set_to_close))
        tasklet_schedule(&p_cb_param->tasklet);
    else
        ; // Back off from processing packets we are closing down.
}

int cpmac_hal_control(OS_DEVICE *p_dev, const char *key, 
	              const char *action, void *value)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_dev->priv;
    int ret_val = -1;

    if(key == NULL)
    {
        dbgPrint("Encountered NULL key.\n");
	return (-1);
    }

    if(cpmac_ci_strcmp(key, "Sleep") == 0 && value != NULL)
    {
        unsigned int clocks_per_tick  = cpmac_cpu_freq/HZ;
	unsigned int requested_clocks = *(unsigned int*)value;
	unsigned int requested_ticks  = (requested_clocks + clocks_per_tick - 1)/clocks_per_tick;
        mdelay(requested_ticks); 
	ret_val = 0;
    }
    else if(cpmac_ci_strcmp(key, "StateChange") == 0)
    {
        ret_val = cpmac_p_process_status_ind(p_cpmac_priv);
    }
    else if(cpmac_ci_strcmp(key, "Tick") == 0 && action != NULL)
    {
	if(cpmac_ci_strcmp(action, "Set") == 0 && value != NULL)
	{
            if(*(unsigned int*)value == 0)
            {
                cpmac_p_stop_timer(p_cpmac_priv->timer + TICK_TIMER);
                ret_val = 0;
            }
            else
            {
                unsigned int clocks_per_tick  = cpmac_cpu_freq/HZ;
	        unsigned int requested_clocks = *(unsigned int*)value;
	        unsigned int requested_ticks  = (requested_clocks + clocks_per_tick - 1)/clocks_per_tick;

                p_cpmac_priv->delay_ticks =  requested_ticks; /* save it for re-triggering */
	        ret_val = cpmac_p_start_timer(p_cpmac_priv->timer + TICK_TIMER, 
		  	                      p_cpmac_priv->delay_ticks);
            }
	}
        else if(cpmac_ci_strcmp(action, "Clear") == 0)
        {
            ret_val = cpmac_p_stop_timer(p_cpmac_priv->timer + TICK_TIMER);
        }
        else
            ;
    }
    else if(cpmac_ci_strcmp(key, "MacAddr") == 0 && action != NULL)
    {
        if(cpmac_ci_strcmp(action, "Get") == 0 && value != NULL)
	{
            *(char **)value = p_cpmac_priv->mac_addr; 
	    ret_val = 0;
	}
    }
    else if(cpmac_ci_strcmp(key, "CpuFreq") == 0)
    {
       if(cpmac_ci_strcmp(action, "Get") == 0 && value != NULL)
       {
           *(unsigned int *)value = cpmac_cpu_freq;
           dbgPrint("Cpu frequency for cpmacs is %u\n",cpmac_cpu_freq);
           ret_val = 0;
       }
    }
    else if(cpmac_ci_strcmp(key, "SioFlush") == 0)
    {
       ret_val = 0;
       dbgPrint("\n"); 
    }
    else if(cpmac_ci_strcmp(key, "CpmacFrequency") == 0)
    {
        /* For Sangam cpmac clock is off the PBUS */
        /* OS Needs to supply CORRECT frequency */
        if(cpmac_ci_strcmp(action, "Get") == 0 && value != NULL)
        {
            *(unsigned int *)value = avalanche_get_vbus_freq();
            ret_val = 0;
        }
    }
    /* For now, providing back the default values. */
    else if(cpmac_ci_strcmp(key, "MdioClockFrequency") == 0)
    {
        if(cpmac_ci_strcmp(action, "Get") == 0 && value != NULL)
        {
            *(unsigned int *)value = 2200000;  /*DEFAULT */
            ret_val = 0;
        }
    }
    /* For now, providing back the default values. */
    else if(cpmac_ci_strcmp(key, "MdioBusFrequency") == 0)
    {
        /* For Sangam MdioBusFreq is off the PBUS */
        if(cpmac_ci_strcmp(action, "Get") == 0 && value != NULL)
        {
            *(unsigned int *)value = avalanche_get_vbus_freq();
            ret_val = 0;
        }
    }
  
#if 0 
#if defined(CONFIG_AVALANCHE_AUTO_MDIX) 
    /* supporting Mdio Mdix switching */
    else if(cpmac_ci_strcmp(key, hcMdioMdixSwitch) == 0)
    {
        /* For Sangam Mdio-switching  action should be always "set"*/
        if(cpmac_ci_strcmp(action, hcSet) == 0 && value != NULL )
        {
           unsigned  int mdix =  *((unsigned int *) value) ;

           if(mdix)
              avalanche_set_phy_into_mdix_mode();

           else
              avalanche_set_phy_into_mdi_mode();  
  
           ret_val = 0;
        } 

    }
#endif
#endif
    else if(cpmac_ci_strcmp(key, hcMdioMdixSwitch) == 0)
    {
        /* For Sangam Mdio-switching  action should be always "set"*/
        if(cpmac_ci_strcmp(action, hcSet) == 0 && value != NULL )
        {
           unsigned  int mdix =  *((unsigned int *) value) ;

           avalanche_set_mdix_on_chip(0xa8610000 , mdix ? 1: 0);
  
           ret_val = 0;
        } 

    }

    return(ret_val);
}


int cpmac_hal_receive(OS_DEVICE *p_dev, FRAGLIST *fragList, 
                             unsigned int fragCount,
                             unsigned int packet_size, 
                             HAL_RECEIVEINFO *hal_receive_info,
                             unsigned int mode)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv  = p_dev->priv;
    CPMAC_DRV_HAL_INFO_T *p_drv_hal     = p_cpmac_priv->drv_hal;
    struct sk_buff       *p_skb         = fragList[0].OsInfo;
    p_skb->len                          = fragList[0].len;    

    /* invalidate the cache. */
    dma_cache_inv((unsigned long)p_skb->data, fragList[0].len);    
#ifdef CPMAC_TEST
    xdump(p_skb->data, p_skb->len, "recv");
#endif
#ifdef CPMAC_8021Q_SUPPORT
    /* 802.1q stuff, just does the basic checking here. */
    if(!p_cpmac_priv->enable_802_1q      &&
       p_skb->len > TCI_END_OFFSET       &&
       IS_802_1Q_FRAME(p_skb->data + TPID_START_OFFSET))
    {
         goto cpmac_hal_recv_frame_mismatch;
    }
#endif
    if(fragCount > 1) 	    
    {	    
       int     len;
       struct sk_buff *p_temp_skb;
       CPMAC_RX_CHAN_INFO_T *p_rx_chan_info = p_cpmac_priv->rx_chan_info;
       int     count;
       
       dbgPrint("Recv: It is multifragment for %s.\n", p_dev->name);
       
       p_skb = dev_alloc_skb(packet_size + 
          p_rx_chan_info->chan->tot_reserve_bytes); 
       if(p_skb == NULL) 
       {
          p_cpmac_priv->net_dev_stats.rx_errors++;
          goto cpmac_hal_recv_alloc_failed;
       }
       
       p_skb->dev = p_dev;
       skb_reserve(p_skb, p_rx_chan_info->chan->tot_reserve_bytes);
       
       for(count = 0; count < fragCount; count++)
       {
          p_temp_skb = fragList[count].OsInfo;
          len        = fragList[count].len;
          
          dma_cache_inv((unsigned long)p_temp_skb->data, len);
          
          memcpy(skb_put(p_skb, len), p_temp_skb->data, len);
          dev_kfree_skb_any(p_temp_skb);
       }
    }
    

#if defined(CONFIG_MIPS_AVALANCHE_MARVELL)                                                                                                   
 /* Fetch the receiving port information from EGRESS TRAILOR Bytes*/         
     p_dev->if_port = (unsigned char)p_skb->data[packet_size -(EGRESS_TRAILOR_LEN-1)] + AVALANCHE_MARVELL_BASE_PORT_ID;
    skb_trim(p_skb, packet_size - EGRESS_TRAILOR_LEN);                                                                       
#else                                                                                                                        
    /* set length & tail */                                                                                                  
    skb_trim(p_skb, packet_size);                                                                                            
#endif 
 
#ifndef TI_SLOW_PATH
    /* TI Optimization: This is NOT required if the ethernet resides below the bridge. But is
    * required only if the ethernet is directly connected to the IP stack. */
    if (p_dev->br_port == NULL)   
#endif
       p_skb->protocol = eth_type_trans(p_skb, p_dev);

    netif_rx(p_skb);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
    avalanche_led_action(p_cpmac_priv->led_handle, CPMAC_RX_ACTIVITY);	
#endif

    p_cpmac_priv->net_dev_stats.rx_packets++;
    p_cpmac_priv->net_dev_stats.rx_bytes += packet_size;

    p_drv_hal->hal_funcs->RxReturn(hal_receive_info,1);

    return(0);

cpmac_hal_recv_alloc_failed:

#ifdef CPMAC_8021Q_SUPPORT
cpmac_hal_recv_frame_mismatch:
#endif
 
    fragCount--;

    do
    {
        dev_kfree_skb_any(fragList[fragCount].OsInfo);
    }
    while(fragCount--);

    p_cpmac_priv->net_dev_stats.rx_dropped++;

    return(-1);
} /*cpmac_receive*/


void cpmac_hal_tear_down_complete(OS_DEVICE*a, int b, int ch)
{
    dbgPrint("what to do with this.\n");
}


int  cpmac_hal_send_complete(OS_SENDINFO *p_skb)
{
    CPMAC_PRIVATE_INFO_T *p_cpmac_priv = p_skb->dev->priv;

    p_cpmac_priv->net_dev_stats.tx_packets++;
    p_cpmac_priv->net_dev_stats.tx_bytes += p_skb->len;
    
    dev_kfree_skb_any(p_skb);

    return(0);
}


int cpmac_reset(CPMAC_PRIVATE_INFO_T *p_cpmac_priv)
{
    // code here to reset the device/hal. Not now.

   netif_wake_queue(p_cpmac_priv->owner);
   return(0);
}

#ifdef CPMAC_TEST

#define isprint(a) ((a >=' ')&&(a<= '~'))
void xdump( u_char*  cp, int  length, char*  prefix )
{
    int col, count;
    u_char prntBuf[120];
    u_char*  pBuf = prntBuf;
    count = 0;
    while(count < length){
        pBuf += sprintf( pBuf, "%s", prefix );
        for(col = 0;count + col < length && col < 16; col++){
            if (col != 0 && (col % 4) == 0)
                pBuf += sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "%02X ", cp[count + col] );
        }
        while(col++ < 16){      /* pad end of buffer with blanks */
            if ((col % 4) == 0)
                sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "   " );
        }
        pBuf += sprintf( pBuf, "  " );
        for(col = 0;count + col < length && col < 16; col++){
            if (isprint((int)cp[count + col]))
                pBuf += sprintf( pBuf, "%c", cp[count + col] );
            else
                pBuf += sprintf( pBuf, "." );
                }
        sprintf( pBuf, "\n" );
        // SPrint(prntBuf);
        printk(prntBuf);
        count += col;
        pBuf = prntBuf;
    }

}  /* close xdump(... */
#endif


static int __init cpmac_dev_probe(void)
{
    int     retVal         = 0;
    int     unit;
    int     instance_count = CONFIG_MIPS_CPMAC_PORTS;

    cpmac_cpu_freq = avalanche_clkc_get_freq(CLKC_MIPS);

    build_psp_config();                       

    for(unit = 0; unit < instance_count; unit++)
    {
        struct net_device	        *p_dev;
        CPMAC_PRIVATE_INFO_T            *p_cpmac_priv;
        size_t		                dev_size;
	int                             failed;

        dev_size =    sizeof(struct net_device) 
	            + sizeof(CPMAC_PRIVATE_INFO_T);


        if((p_dev = (struct net_device *) kmalloc(dev_size, GFP_KERNEL)) == NULL)
        {
            dbgPrint( "Could not allocate memory for device.\n" );
            retVal = -ENOMEM;
            break;
        }            
               
        memset(p_dev, 0, dev_size );

        p_dev->priv                 = p_cpmac_priv
                                    = (CPMAC_PRIVATE_INFO_T*)(((char *) p_dev) + sizeof(struct net_device));
        p_cpmac_priv->owner         = p_dev;
	
        ether_setup(p_dev); 

	p_cpmac_priv->instance_num  = unit;
        p_dev->init                 = cpmac_dev_init;

        g_dev_array[p_cpmac_priv->instance_num] = p_dev;
        
#if defined CONFIG_MIPS_CPMAC_INIT_BUF_MALLOC
        g_init_enable_flag = 1;
        printk("Cpmac driver is allocating buffer memory at init time.\n");
#endif

	/* This section gives a default value by the number of PHY in order to
	 * replace the default MACRO. */
	{
	    char *mac_port = prom_getenv("MAC_PORT"); /* Internal: 0, External: 1 */
	    if(!mac_port || (0 != strcmp(mac_port, "0"))) {
		printk("Using the MAC with external PHY\n");
		cfg_start_link_speed = _CPMDIO_NOPHY;
		cpmac_max_frame_size = CPMAC_MAX_FRAME_SIZE + 4;
	    }
	    else {
		printk("Using the MAC with internal PHY\n");
		cfg_start_link_speed = CFG_START_LINK_SPEED;
		cpmac_max_frame_size = CPMAC_MAX_FRAME_SIZE;
	    }
	    g_cfg_start_link_params = cfg_start_link_speed;
	}

        cpmac_p_detect_manual_cfg(cfg_link_speed, cfg_link_mode, cpmac_debug_mode);

    	failed = register_netdev(p_dev);
        if (failed)
        {
            dbgPrint("Could not register device for inst %d because of reason \
                      code %d.\n", unit, failed);
            retVal = -1;
            kfree(p_dev);
            break;
        }           
        else
        {

            char proc_name[100];
            int  proc_category_name_len = 0;

	    p_cpmac_priv->next_device = last_cpmac_device;
            last_cpmac_device         = p_dev;

    	    dbgPrint(" %s irq=%2d io=%04x\n",p_dev->name, (int) p_dev->irq,
    	    	       (int) p_dev->base_addr);

	    strcpy(proc_name, "avalanche/");
            strcat(proc_name, p_dev->name);
            proc_category_name_len = strlen(proc_name);

            strcpy(proc_name + proc_category_name_len, "_rfc2665_stats");
            create_proc_read_entry(proc_name,0,NULL,cpmac_p_read_rfc2665_stats, p_dev);

    	}
    }

    if(retVal == 0)
    {
	/* To maintain backward compatibility with NSP. */
        gp_stats_file = create_proc_entry("avalanche/cpmac_stats", 0644, NULL);
        if(gp_stats_file)
        {
            gp_stats_file->read_proc  = cpmac_p_read_stats;
            gp_stats_file->write_proc = cpmac_p_write_stats;
        }
	create_proc_read_entry("avalanche/cpmac_link", 0, NULL, cpmac_p_read_link, NULL);
	create_proc_read_entry("avalanche/cpmac_ver", 0, NULL, cpmac_p_get_version, NULL);

    }
   
    cpmac_devices_installed  = unit;
    dbgPrint("Installed %d cpmac instances.\n", unit);
    return ( (unit >= 0 ) ? 0 : -ENODEV );

} /* init_module */


/***************************************************************
 *	cleanup_module
 *
 *	Returns:
 *		Nothing
 *	Parms:
 *		None
 *
 *	Goes through the CpmacDevices list and frees the device
 *	structs and memory associated with each device (lists
 *	and buffers).  It also ureserves the IO port regions
 *	associated with this device.
 *
 **************************************************************/

void cpmac_exit(void)
{
    struct net_device       *p_dev;
    CPMAC_PRIVATE_INFO_T    *p_cpmac_priv;

    while (cpmac_devices_installed) 
    {
        char proc_name[100];
        int  proc_category_name_len = 0;

        p_dev = last_cpmac_device;
        p_cpmac_priv = (CPMAC_PRIVATE_INFO_T *) p_dev->priv;

    	dbgPrint("Unloading %s irq=%2d io=%04x\n",p_dev->name, (int) p_dev->irq, (int) p_dev->base_addr);

        if(g_init_enable_flag)
            cpmac_p_dev_disable(p_dev);
      
        cpmac_drv_cleanup(p_cpmac_priv->drv_hal);

#if defined (CONFIG_MIPS_AVALANCHE_LED)
        avalanche_led_unregister(p_cpmac_priv->led_handle);
#endif
	strcpy(proc_name, "avalanche/");
        strcat(proc_name, p_dev->name);
        proc_category_name_len = strlen(proc_name);

        strcpy(proc_name + proc_category_name_len, "_rfc2665_stats");
        remove_proc_entry(proc_name, NULL);
        
        release_mem_region(p_dev->base_addr, p_cpmac_priv->dev_size);
        unregister_netdev(p_dev);
	last_cpmac_device = p_cpmac_priv->next_device;

        kfree(p_cpmac_priv->drv_hal);
        kfree(p_dev);
     
	cpmac_devices_installed--;
    }

    if(gp_stats_file)
        remove_proc_entry("avalanche/cpmac_stats", NULL);

    remove_proc_entry("avalanche/cpmac_link",  NULL);
    remove_proc_entry("avalanche/cpmac_ver",   NULL);

    psp_config_cleanup();
}
     
module_init(cpmac_dev_probe);
module_exit(cpmac_exit);
