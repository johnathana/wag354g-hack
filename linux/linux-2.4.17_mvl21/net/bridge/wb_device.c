/**************************************************************************
 * FILE PURPOSE	:  	WAN Bridge Device Implementation.
 **************************************************************************
 * FILE NAME	:   wb_device.c
 *
 * DESCRIPTION	:
 *  This file implements the WAN device for the WAN Bridge. Note that there
 *  can be any number of WAN devices in the system.
 *
 *	CALL-INs:
 *
 *	CALL-OUTs:
 *
 *	User-Configurable Items:
 *
 *	(C) Copyright 2004, Texas Instruments, Inc.
 *************************************************************************/
#include <linux/kernel.h>
#include <linux/if_vlan.h>
#include <net/wb.h>

#include <linux/etherdevice.h>
#include <net/arp.h>

static int wb_xmit(struct sk_buff *skb, struct net_device *dev);
static struct net_device_stats *wb_get_stats(struct net_device *dev);
static void str2eaddr(unsigned char *ea, unsigned char *str);

/**************************************************************************
 * FUNCTION NAME : vlan_hard_header
 **************************************************************************
 * DESCRIPTION   :
 *  Create the VLAN MAC header for an arbitrary protocol layer.
 *	    saddr=NULL	means use device source address
 *	    daddr=NULL	means leave destination address (eg unresolved arp)
 *  This is called when the SKB is moving down the stack towards the
 *  physical devices.
 *
 * RETURNS       :
 *      > 0     - Success.
 *      < 0     - Error.
 ***************************************************************************/
static int vlan_hard_header 
(
    struct sk_buff*     skb, 
    struct net_device*  dev,
    unsigned short      type, 
    void*               daddr, 
    void*               saddr,
    unsigned            len
)
{
    struct vlan_ethhdr*     ptr_vlan;
    WAN_BRIDGE_DEVICE_PRIV* ptr_wan_device_priv = (WAN_BRIDGE_DEVICE_PRIV *)dev->priv;

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: vlan_hard_header called.\n",0,0,0,0,0);

    /* Move back the skb pointer for the VLAN Header. */
	ptr_vlan = (struct vlan_ethhdr *)skb_push(skb, VLAN_ETH_HLEN);

	/* Set the protocol type. This is always 8021Q. */	
    ptr_vlan->h_vlan_proto = __constant_htons(ETH_P_8021Q);

    /* Set the protocol field. */
    ptr_vlan->h_vlan_encapsulated_proto = htons(type);

    /* Set the VLAN Tag and user priority. */
#if 0
    ptr_vlan->h_vlan_TCI = htons(ptr_wan_device_priv->ptr_wan_device->vlan_key);
#else
    ptr_vlan->h_vlan_TCI = htons((ptr_wan_device_priv->ptr_wan_device->user_priority << 13) | 
                                  ptr_wan_device_priv->ptr_wan_device->vlan_key);
#endif

	/* Set the source hardware address. */
	if(saddr)
		memcpy(ptr_vlan->h_source, saddr, dev->addr_len);
	else
		memcpy(ptr_vlan->h_source, dev->dev_addr, dev->addr_len);

	/* Anyway, the loopback-device should never use this function... */
	if (dev->flags & (IFF_LOOPBACK|IFF_NOARP)) 
	{
		memset (ptr_vlan->h_dest, 0, dev->addr_len);
		return (dev->hard_header_len);
	}

    /* Set the destination MAC address. */	
	if(daddr)
		memcpy(ptr_vlan->h_dest,daddr,dev->addr_len);

    /* Error: Return so. */
	return dev->hard_header_len;
}


/**************************************************************************
 * FUNCTION NAME : vlan_rebuild_header
 **************************************************************************
 * DESCRIPTION   :
 *	Rebuild the Ethernet MAC header. This is called after an ARP
 *	(or in future other address resolution) has completed on this
 *	sk_buff. We now let ARP fill in the other fields.
 *
 *	This routine CANNOT use cached dst->neigh!
 *	Really, it is used only when dst->neigh is wrong.
 *
 * NOTES         :
 *  This function to be verified. 
 ***************************************************************************/
static int vlan_rebuild_header(struct sk_buff *skb)
{
	struct vlan_ethhdr* ptr_vlan = (struct vlan_ethhdr*)skb->data;
	struct net_device *dev = skb->dev;

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: vlan_rebuild_header called.\n",0,0,0,0,0);

	switch (ptr_vlan->h_vlan_encapsulated_proto)
	{
#ifdef CONFIG_INET
	    case __constant_htons(ETH_P_IP):
        {
            return arp_find(ptr_vlan->h_dest, skb);
        }
#endif	
    	default:
        {
		    printk(KERN_DEBUG "%s: unable to resolve type %X addresses.\n", 
                              dev->name, (int)ptr_vlan->h_vlan_encapsulated_proto);
		    memcpy(ptr_vlan->h_source, dev->dev_addr, dev->addr_len);
		    break;
        }
	}
	return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_xmit
 **************************************************************************
 * DESCRIPTION   :
 *  Registered callback routine with the NET layer to transmit a packet.
 ***************************************************************************/
static int wb_xmit(struct sk_buff *skb, struct net_device *dev)
{   
    WAN_BRIDGE_DEVICE_PRIV* ptr_wan_device_priv = (WAN_BRIDGE_DEVICE_PRIV *)dev->priv;
	struct net_device_stats* stats = (struct net_device_stats *)&ptr_wan_device_priv->stats;

    /* Increment the statistics. */
	stats->tx_packets++;
	stats->tx_bytes+=skb->len;

    /* Use the API provided by the WAN bridge to xmit the packet. The WAN Bridge 
     * knows which is the source interface that needs to be used for transmission*/
    if (wb_xmit_frame (skb, ptr_wan_device_priv->ptr_wan_device) < 0)
        dev_kfree_skb(skb);
	return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_get_stats
 **************************************************************************
 * DESCRIPTION   :
 *  Registered callback routine with the NET layer to retreive statistics.
 ***************************************************************************/
static struct net_device_stats *wb_get_stats(struct net_device *dev)
{
    WAN_BRIDGE_DEVICE_PRIV* ptr_wan_device_priv = (WAN_BRIDGE_DEVICE_PRIV *)dev->priv;
	return &ptr_wan_device_priv->stats;
}

/**************************************************************************
 * FUNCTION NAME : wb_device_open
 **************************************************************************
 * DESCRIPTION   :
 *   Registered callback routine with the NET layer to open the network 
 *   device.
 ***************************************************************************/
static int wb_device_open (struct net_device *dev)
{
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_device_stop
 **************************************************************************
 * DESCRIPTION   :
 *   Registered callback routine with the NET layer to stop the network 
 *   device.
 ***************************************************************************/
static int wb_device_stop (struct net_device *dev)
{
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_device_init
 **************************************************************************
 * DESCRIPTION   :
 *  Registered callback routine with the NET layer for device initialization
 ***************************************************************************/
static int wb_device_init(struct net_device *dev)
{
    WAN_BRIDGE_DEVICE_PRIV* ptr_wan_device_priv = (WAN_BRIDGE_DEVICE_PRIV *)dev->priv;

    /* Before registering the device determine the type of the device. If the device 
     * is under a BRIDGE which supports VLAN, then create WAN Bridge VLAN Device else
     * create a normal ethernet device. */
    if ( (ptr_wan_device_priv->wb_mode == WAN_BRIDGE_NON_VLAN_MODE) ||
         (ptr_wan_device_priv->wb_mode == WAN_BRIDGE_HALF_BRIDGE_MODE) )
    {
        /* Setup the device as Normal Ethernet and register it with the NET. */
        wblogMsg (WB_LOG_DEBUG,"DEBUG: WAN Bridge Device is normal Ethernet.\n",0,0,0,0,0);

        /* Initialize the Linux NET Device structure. */
        ether_setup(dev);

        /* Fill up the device specific functions. */
	    dev->open               = wb_device_open;
    	dev->stop               = wb_device_stop;
	    dev->get_stats          = wb_get_stats;
    	dev->hard_start_xmit    = wb_xmit;

        /* Change the Device MTU to account for the VLAN Header. This way we tell the
         * IP stack to fragment packets till 1496, which accounts for the 4 byte VLAN
         * header. */
        dev->mtu                = dev->mtu - VLAN_HLEN;
    }
    else
    {
        /* Setup the device as VLAN Enabled and register it with the NET. */
        wblogMsg (WB_LOG_DEBUG,"DEBUG: WAN Bridge Device is VLAN Enabled.\n",0,0,0,0,0);

        /* Fill up the device specific functions. */
	    dev->open               = wb_device_open;
    	dev->stop               = wb_device_stop;
	    dev->get_stats          = wb_get_stats;
    	dev->hard_start_xmit    = wb_xmit;

        /* Fill up the VLAN specific information. Remember for us WAN Bridge devices
         * are always going to be VLAN devices. */
    	dev->change_mtu		    = NULL;                 /* eth_change_mtu; */
	    dev->hard_header	    = vlan_hard_header;     /* eth_header; */
    	dev->rebuild_header 	= vlan_rebuild_header;  /* eth_rebuild_header; */
	    dev->set_mac_address 	= NULL;                 /* eth_mac_addr; */
    	dev->hard_header_cache	= NULL;                 /* eth_header_cache; */
	    dev->header_cache_update= NULL;                 /* eth_header_cache_update; */
    	dev->hard_header_parse	= eth_header_parse;

	    dev->type		        = ARPHRD_ETHER;
    	dev->hard_header_len 	= VLAN_ETH_HLEN;
	    dev->mtu		        = 1500;
    	dev->addr_len		    = ETH_ALEN;
	    dev->tx_queue_len	    = 100;	/* Ethernet wants good queues */	    	
    	memset(dev->broadcast, 0xFF, ETH_ALEN);
    	dev->flags		        = IFF_BROADCAST|IFF_MULTICAST;
    }
    
    /* Setup the device and register it with the NET. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: WAN Device Initialization completed.\n",0,0,0,0,0);
	return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_device_initialize
 **************************************************************************
 * DESCRIPTION   :
 *  This is the standard API function that needs to be called to create a 
 *  WAN Bridge Device. The 'dev_name' is passed as a parameter to the 
 *  function the function assumes that all necessary validations have been 
 *  done before.  
 *
 * RETURNS       :
 *      Pointer to the WAN Bridge network device    - Success.
 *      NULL                                        - Error.
 ***************************************************************************/
struct net_device* wb_device_initialize(char *dev_name, WAN_BRIDGE_MODE wb_mode)
{
	int                     err;
    struct net_device*      ptr_net_device;
    unsigned char 		    mac_addr[ETH_ALEN];
    int                     i;
 
    /* Allocate memory for the NET device. */
    ptr_net_device = (struct net_device *)wb_malloc(sizeof(struct net_device));
    if (ptr_net_device == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"Error: Unable to allocate memory for the NET device.\n",0,0,0,0,0);
        return NULL;
    }
    memset ((void *)ptr_net_device, 0, sizeof(struct net_device));

    /* Register the Intialization and Uninitialization functions. */
	ptr_net_device->init    = wb_device_init;

	/* Allocate memory for the private structure. */
	ptr_net_device->priv = (WAN_BRIDGE_DEVICE_PRIV *)wb_malloc(sizeof(WAN_BRIDGE_DEVICE_PRIV));
	if (ptr_net_device->priv == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"Error: Unable to allocate memory for the NET device.\n",0,0,0,0,0);
        return NULL;
    }

    /* Initialize the private structure. */
	memset(ptr_net_device->priv, 0, sizeof(WAN_BRIDGE_DEVICE_PRIV));
    ((WAN_BRIDGE_DEVICE_PRIV *)ptr_net_device->priv)->wb_mode = wb_mode;

    /* This sets the hardware address */
	str2eaddr(mac_addr, wan_bridge_device_mac_string);
	for (i=0; i <= ETH_ALEN; i++)
    	ptr_net_device->dev_addr[i] = mac_addr[i];

    /* Set the device name as passed. */
    strcpy (ptr_net_device->name, dev_name);

    /* Register the netdevice. */ 
	err = register_netdev(ptr_net_device);
	if (err<0)
		return NULL;

    /* Return the net device structure. */
	return ptr_net_device;
}

/**************************************************************************
 * FUNCTION NAME : wb_device_uninitialize
 **************************************************************************
 * DESCRIPTION   :
 *  The API Function provided to uninitialize a network device. 
 ***************************************************************************/
void wb_device_uninitialize(struct net_device* ptr_net_device)
{
    /* Unregister the network device. */
    unregister_netdev(ptr_net_device);

    /* We now need to clean all the memory that we had allocated for the NET
     * Device. Clean the private structure and net device block. */
    wb_free (ptr_net_device->priv);
    wb_free (ptr_net_device);
    return;
}

/**************************************************************************
 * FUNCTION NAME : str2hexnum
 **************************************************************************
 * DESCRIPTION   :
 *
 * RETURNS		 :
***************************************************************************/
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

/**************************************************************************
 * FUNCTION NAME : str2eaddr
 **************************************************************************
 * DESCRIPTION   :
 *
 * RETURNS		 :
***************************************************************************/
static void str2eaddr(unsigned char *ea, unsigned char *str)
{
    int i;
    unsigned char num;
    for(i = 0; i < 6; i++) 
    {
        if((*str == '.') || (*str == ':'))
            str++;
        num = str2hexnum(*str++) << 4;
        num |= (str2hexnum(*str++));
        ea[i] = num;
    }
}

