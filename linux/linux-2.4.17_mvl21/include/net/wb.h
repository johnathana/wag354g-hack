/**************************************************************************
 * FILE PURPOSE	:  	WAN Bridge Structures
 **************************************************************************
 * FILE NAME	:   wb.h
 *
 * DESCRIPTION	:
 * 	Stuctures and definations used by the WAN Bridge and WAN Devices.
 *
 *	(C) Copyright 2003, Texas Instruments, Inc.
 *************************************************************************/

#ifndef __WB__H__
#define __WB__H__

#include <linux/netdevice.h>
#include <linux/list.h>

/******************************* DEFINITIONS *****************************/

/* Currently we do NOT support HALF Bridge Mode. */
#undef WAN_BRIDGE_HALF_BRIDGE_MODE_SUPPORT

/* Log Levels for various messages in the system. */
#define WB_LOG_DEBUG                10
#define WB_LOG_FATAL                1

/* The max length of the WAN Bridge name */
#define MAX_LEN_WAN_BRIDGE_NAME     10

/* The MAX number of VLAN's supported. */
#define MAX_VLAN_SUPPORTED          1024

/* IOCTL Commands that need to be supported by the SOURCE INTERFACE Drivers. */
#define TIWAN_ADD_WAN_BRIDGE_IOCTL  _IOWR('a', 0, int)
#define TIWAN_DEL_WAN_BRIDGE_IOCTL  _IOWR('d', 0, int)

/* The global variable, which has the MAC address of all WAN Bridge devices. */
extern char* wan_bridge_device_mac_string;

/**************************************************************************
 * STRUCTURE -  WAN_BRIDGE_MODE
 **************************************************************************
 * The structure indicates the various modes in which the WAN Bridge can 
 * operate. Typically the mode will be VLAN mode, Non VLAN mode will be used
 * only for the special HALF bridge. 
 **************************************************************************/
typedef enum WAN_BRIDGE_MODE
{
	WAN_BRIDGE_NON_VLAN_MODE 	= 0x0,
	WAN_BRIDGE_VLAN_MODE        = 0x1,
    WAN_BRIDGE_HALF_BRIDGE_MODE = 0x2
}WAN_BRIDGE_MODE;

/**************************************************************************
 * STRUCTURE -  WAN_BRIDGE_STATE
 **************************************************************************
 * The structure indicates the various states of the WAN Bridge.
 **************************************************************************/
typedef enum WAN_BRIDGE_STATE
{
	WAN_BRIDGE_NOT_READY 	= 0x0,
	WAN_BRIDGE_READY        = 0x1
}WAN_BRIDGE_STATE;

/**************************************************************************
 * STRUCTURE -  WAN_BRIDGE_DEVICE_PRIV
 **************************************************************************
 * The Linux device support allows tagging any private information to the 
 * device structure. For the WAN Bridge Network devices, we tag this private
 * structure along. Just carries the basic statistics and a link back to the 
 * actual WAN BRIDGE Device structure. The link is required else there 
 * exists no relationship between the network device and WAN bridge device.
 **************************************************************************/
typedef struct WAN_BRIDGE_DEVICE_PRIV
{
    /* Pointer to the WAN Bridge device. */
    struct WAN_BRIDGE_DEVICE*   ptr_wan_device;

    /* The NET Device statistics. */
    struct net_device_stats     stats;

    /* The BRIDGE Mode to which this device belongs. This is a duplicate and
     * should not have been kept here except for the fact that the 
     * initialization function to register the net device is done before the WAN
     * bridge device above is created and initialized. Besides this is the only
     * structure that the init function has access to. */
    WAN_BRIDGE_MODE             wb_mode;
}WAN_BRIDGE_DEVICE_PRIV;

/**************************************************************************
 * STRUCTURE -  WAN_BRIDGE_DEVICE
 **************************************************************************
 * The structure contains information about the WAN net device structure 
 * that resides on top of the WAN bridge. The WAN bridge assumes that only
 * WAN BRIDGE Devices can be connected to it. 
 **************************************************************************/
typedef struct WAN_BRIDGE_DEVICE
{
    /* Links to the next and prev device attached to the bridge. */ 
    struct list_head        links;

    /* The associated network device. */
    struct net_device*      ptr_net_device;

    /* The VLAN key. */
    int                     vlan_key;

    /* The User Priority. */
    int                     user_priority;

    /* The associated WAN Bridge to which the device is attached. */
    struct WAN_BRIDGE*      ptr_wan_bridge;
}WAN_BRIDGE_DEVICE;

/**************************************************************************
 * STRUCTURE -  WAN_BRIDGE
 **************************************************************************
 * The structure contains information about the WAN Bridge. Typically 
 * information about all the devices attached to the bridge, the source 
 * device, statistics are maintained. The WAN bridge was maintained as a 
 * structure so that multiple WAN bridges can reside in the system.
 **************************************************************************/
typedef struct WAN_BRIDGE 
{
    /* Pointer to the prev and next WAN bridge */ 
    struct list_head        bridge_list;

    /* Pointer to the list of devices attached to the bridge. */ 
    struct list_head        device_list;

    /* The state of the WAN Bridge. */
    WAN_BRIDGE_STATE        wb_state;

    /* The mode of the WAN Bridge. */
    WAN_BRIDGE_MODE         wb_mode;

    /* The name of the WAN Bridge. */
    char                    name[MAX_LEN_WAN_BRIDGE_NAME];

    /* Interface on which the WAN Bridge is running. */
    struct net_device*      ptr_src_device;

    /* The hash bucket, which accounts for the max. number of VLAN tags supported. */
    unsigned int            vlan_hash_bucket[MAX_VLAN_SUPPORTED];

    /* HALF Bridge Mode Related fields. */
    int                     mac_address_learnt;
    struct ethhdr           ethernet_header;

    /**************** STATISTICS *****************/
    int                     memory_errors;
    int                     rx_packet_dropped;
    int                     tx_packet_dropped;
    int                     non_vlan_packets_rxed;
}WAN_BRIDGE;

/********************************* EXTERN FUNCTIONS **************************/

/* FILE : wb_config.c */
extern void *wb_malloc (int num_bytes);
extern void wb_free (void *ptr);
extern int	wblogMsg (int level,char* fmt,int arg1,int arg2,int arg3,int arg4,int arg5);

/* FILE : wb_device.c */
extern struct net_device* wb_device_initialize(char *dev_name, WAN_BRIDGE_MODE wb_mode);
extern void wb_device_uninitialize(struct net_device* ptr_net_device);

/* FILE : wb_main.c */
extern int wb_receive_frame (struct sk_buff *skb, void* ptr_handle);
extern int wb_xmit_frame (struct sk_buff *skb, WAN_BRIDGE_DEVICE* ptr_wan_device);
extern int wb_add (WAN_BRIDGE* ptr_wan_bridge, WAN_BRIDGE_DEVICE* ptr_wan_device, int key);
extern int wb_delete (WAN_BRIDGE* ptr_wan_bridge, WAN_BRIDGE_DEVICE* ptr_wan_device, int key);

#endif  /* __WB__H__ */
