/**************************************************************************
 * FILE PURPOSE	:  	WAN Bridge Implementation.
 **************************************************************************
 * FILE NAME	:   wb_main.c
 *
 * DESCRIPTION	:
 *  This file implements the WAN Bridge Code.
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
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <linux/if_vlan.h>
#include <asm/uaccess.h>
#include <net/wb.h>

/* Global definitions. */
const int VLAN_TYPE = __constant_htons(ETH_P_8021Q);

/* Static Definitions. */
static void wb_kfree_skb(struct sk_buff *skb);

/**************************************************************************
 * FUNCTION NAME : my_memcpy
 **************************************************************************
 * DESCRIPTION   :
 *  Utility function which copies data from the source to the destination.
 *  Except the addresses are decremented. Used to copy the MAC Addresses to 
 *  the correct location.
 ***************************************************************************/
static void my_memcpy (char *dest, char *src, int num)
{
    int index = 0;
    for (index = 0; index < num; index++)
        *(--dest) = *(--src);
    return;
}

/**************************************************************************
 * FUNCTION NAME : wb_lookup
 **************************************************************************
 * DESCRIPTION   :
 *  The 'lookup' function is called from the WAN Bridge receive processing
 *  function to demultiplex the packet. Each WAN Bridge device is related
 *  with a specific key, the function returns the WAN Bridge device matching
 *  'key'. The 'key' needs to be filled by the source interface.
 *  
 * RETURNS       :
 *      NULL        - Error: Could not get the related WAN bridge device.
 *      Non NULL    - Pointer to the WAN Bridge device.
 ***************************************************************************/
static WAN_BRIDGE_DEVICE* wb_lookup (WAN_BRIDGE* ptr_wan_bridge, int key)
{
    return (WAN_BRIDGE_DEVICE*)ptr_wan_bridge->vlan_hash_bucket[key];
}

/**************************************************************************
 * FUNCTION NAME : wb_netif_rx
 **************************************************************************
 * DESCRIPTION   :
 *  The function passes the received skb up the net stack.
 ***************************************************************************/
static void wb_netif_rx (struct sk_buff *skb, WAN_BRIDGE_DEVICE* ptr_wan_device)
{
    struct net_device_stats*    stats;

    /* Increment the receive statistics for the WAN Bridge device. */
    stats = &((WAN_BRIDGE_DEVICE_PRIV *)ptr_wan_device->ptr_net_device->priv)->stats;
    stats->rx_packets++;
    stats->rx_bytes+=skb->len;

    /* Pass the packet up the IP stack. */
    netif_rx (skb);
    return;
}

/**************************************************************************
 * FUNCTION NAME : wb_receive_frame
 **************************************************************************
 * DESCRIPTION   :
 *  This is the exported API which the source interface needs to call to 
 *  pass the packet to the WAN Bridge. This function now assumes that the
 *  skb->data points to the start of the ethernet data. There should be no
 *  translations done on the skb. 
 *
 * RETURNS       :
 *      < 0     - Error.
 *       0      - Success.
 *
 * NOTES         :
 *  The function will clean packet memory. The source interface should not 
 *  worry about the SKB after calling this function.
 ***************************************************************************/
int wb_receive_frame (struct sk_buff *skb, void* ptr_handle)
{
    WAN_BRIDGE*          ptr_wan_bridge;
    WAN_BRIDGE_DEVICE*   ptr_wan_device;
    int                  vlan_id;
    unsigned short       vlan_TCI;
    struct list_head*    ptr_temp;
    struct list_head*    ptr_prev = NULL;
    struct vlan_ethhdr*  ptr_vlan = NULL;
/*    struct vlan_hdr*     ptr_vlan = NULL; */
    struct sk_buff*      skb_cloned;

    /* Get the pointer to the WAN Bridge. */
    ptr_wan_bridge = (WAN_BRIDGE *)ptr_handle;
    if (ptr_wan_bridge == NULL)
    {
        /* Incorrect handle. Dropping the packet. */
        ptr_wan_bridge->rx_packet_dropped++;
        wb_kfree_skb (skb);
        return -1;
    }

    /* Determine the mode in which the bridge is operating. */
    switch (ptr_wan_bridge->wb_mode)
    {
        case WAN_BRIDGE_VLAN_MODE:
        {
            /* Get the VLAN Ethernet header.*/
            ptr_vlan = (struct vlan_ethhdr*)skb->data;

            /* Sanity Check : Make sure that the WAN Bridge receives only VLAN packets. */
            if (ptr_vlan->h_vlan_proto != VLAN_TYPE)
            {
                /* The WAN Bridge operates only on VLAN Packets. Dropping the packet */
                ptr_wan_bridge->non_vlan_packets_rxed++;
                wb_kfree_skb (skb);
                return -1;
            }
    
            /* Get the VLAN header and extract the VLAN Identifier. */
            vlan_TCI = ntohs(ptr_vlan->h_vlan_TCI);
            vlan_id  = (vlan_TCI & 0xFFF);

            /* Debug Message. */
            wblogMsg (WB_LOG_DEBUG, "DEBUG: Received Packet with VLAN: %d.\n",vlan_id,0,0,0,0);

            /* Jump the data pointer to the location just before the protocol identifier. */
            skb->data = skb->data + sizeof(struct vlan_ethhdr) - 2;

            /* Lookup the WAN Bridge device on which the packet is to be forwarded. */
            ptr_wan_device = wb_lookup (ptr_wan_bridge, vlan_id);
            if (ptr_wan_device != NULL)
            {
                /* Currently the skb is pointing to just before the protocol identifier in the
                 * ethernet header. We need to overwrite the VLAN header so that it appears as 
                 * a normal ethernet frame to the layers above. The protocol field is correct, 
                 * all we need to do is move the DST and SRC mac address. i.e. 12 bytes. */
                my_memcpy (skb->data, skb->data - VLAN_HLEN, 12);

                /* NSP Optimizations: The performance optimizations added required the following
                 * change in the code. Check if translations are required or not. This is dependant
                 * on where the WAN Bridge device is attached. 
                 *      Bridged WAN Bridge devices do not need translations, 
                 *      Routed WAN Bridge  devices NEED translations.*/
                if (ptr_wan_device->ptr_net_device->br_port == NULL)
                {
                    /* ROUTED Connection: Do the translations because this packet 
                     * goes directly to the IP stack. Initialize the skb correctly. */
                    skb->mac.raw    = skb->data - 12;
                    skb->data       = skb->data + 2;
                    skb->dev        = ptr_wan_device->ptr_net_device;
                    skb->pkt_type   = PACKET_HOST;
                    skb->protocol   = ptr_vlan->h_vlan_encapsulated_proto;
                }
                else
                {
                    /* BRIDGED Connection: This packet goes to the bridge. There is no
                     * need to do any initializations here. All we need to verify here 
                     * is that the skb->data points to the start of the packet. */
                    skb->data = skb->data - 12;
                    skb->dev = ptr_wan_device->ptr_net_device;
                }

                /* Pass the received frame up the network stack. */
                wb_netif_rx (skb, ptr_wan_device);

                /* Debug Message. Packet has been formatted without VLAN and is being sent up the stack. */
                wblogMsg (WB_LOG_DEBUG, "DEBUG: WAN Bridge Protocol=0x%x PacketType=%d WANDevice=%s.\n",
                          skb->protocol, skb->pkt_type, (int)ptr_wan_device->ptr_net_device->name,0,0);
            }
            else
            {
                /* This means that a packet was received with a VLAN tag for which there was 
                 * no registered interface. Since there was no forwarding information drop 
                 * the packet. */
                ptr_wan_bridge->rx_packet_dropped++;
                wb_kfree_skb (skb);
            }
            return 0;
        }
        case WAN_BRIDGE_NON_VLAN_MODE:
        {
            /* Process only if the WAN Bridge is ready else drop the packet. The WAN Bridge is ready
             * only if it has at least 1 WAN Bridge device connected to it. */
            if (ptr_wan_bridge->wb_state == WAN_BRIDGE_NOT_READY)
            {
                wb_kfree_skb (skb);
                return 0;
            }

            /* In NON VLAN Mode, there is no other way to demultiplex the packet, so lets flood the
             * packet on all the interfaces. */            
            list_for_each(ptr_temp, &ptr_wan_bridge->device_list)
            {
                /* Optimization: Do not clone all the time, but instead clone 1 time less than the
                 * number of WAN Bridge devices. Send the orignal packet on the first WAN Bridge 
                 * device. */
                if (ptr_prev == NULL)
                {
                    ptr_prev = ptr_temp;
                    continue;
                }

                /* Clone the SKB ! */
                if ((skb_cloned = skb_clone(skb, GFP_ATOMIC)) == NULL) 
                {
                    wb_kfree_skb (skb);
                    ptr_wan_bridge->memory_errors++;
                    return -1;
                }
    
                /* Get the WAN device information. */
                ptr_wan_device = (WAN_BRIDGE_DEVICE *)ptr_temp;

                /* Fill up the necessary fields and pass the packet to the IP stack
                 * indicating that as if the packet was received on WAN Device.  */
                skb_cloned->dev       = ptr_wan_device->ptr_net_device;
                skb_cloned->pkt_type  = PACKET_HOST;

                /* NSP Optimizations: Do the translation only if the WAN Bridge device is not 
                 * bridged. */
                if (ptr_wan_device->ptr_net_device->br_port == NULL)   
                    skb_cloned->protocol  = eth_type_trans(skb_cloned, ptr_wan_device->ptr_net_device);

                /* Pass the received frame up the network stack. */
                wb_netif_rx (skb_cloned, ptr_wan_device);
 
                /* DEBUG Message. */
                wblogMsg (WB_LOG_DEBUG,"DEBUG: Cloned. WAN Bridge Protocol=0x%x WANDevice=%s.\n",
                          skb_cloned->protocol, (int)ptr_wan_device->ptr_net_device->name,0,0,0);
            }

            /* Send the orignal packet to the first WAN Bridge device. */
            if (ptr_prev != NULL)
            {
                /* Get the WAN device information. */
                ptr_wan_device = (WAN_BRIDGE_DEVICE *)ptr_prev;

                /* Fill up the necessary fields and pass the packet to the IP stack
                 * indicating that as if the packet was received on WAN Device.  */
                skb->dev       = ptr_wan_device->ptr_net_device;
                skb->pkt_type  = PACKET_HOST;

                /* NSP Optimizations: Do the translation only if the WAN Bridge device is not 
                 * bridged. */
                if (ptr_wan_device->ptr_net_device->br_port == NULL)   
                    skb->protocol  = eth_type_trans(skb, ptr_wan_device->ptr_net_device);
            
                /* Pass the received frame up the network stack. */
                wb_netif_rx (skb, ptr_wan_device);

                /* DEBUG Message. */
                wblogMsg (WB_LOG_DEBUG,"DEBUG: Orignal WAN Bridge Protocol=0x%x WANDevice=%s.\n",
                          skb->protocol, (int)ptr_wan_device->ptr_net_device->name,0,0,0);
            }
            return 0;
        }
#ifdef WAN_BRIDGE_HALF_BRIDGE_MODE_SUPPORT
        case WAN_BRIDGE_HALF_BRIDGE_MODE:
        {
            /* In the case of HALF Bridge packets, we have received a packet we now need
             * to append the Ethernet header and pass the packet to the WAN Bridge device
             * which in turn will pass the packet to the Local Bridge */
            wblogMsg (WB_LOG_DEBUG,"DEBUG:Rx Half Bridge Mode\n",0,0,0,0,0);
            skb_push(skb, ETH_HLEN);

            /* Reverse the Destination and Source Mac Address. */
            memcpy((void*)skb->data,(void*)&ptr_wan_device->ptr_wan_bridge->ethernet_header.h_source,6);
            memcpy((void*)skb->data+6,(void*)&ptr_wan_device->ptr_wan_bridge->ethernet_header.h_dest,6);
            memcpy((void*)skb->data+12,(void*)&ptr_wan_device->ptr_wan_bridge->ethernet_header.h_proto,2);

            /* Pass the packet up. */
            skb->dev       = ptr_wan_device->ptr_net_device;
            skb->pkt_type  = PACKET_HOST;
            skb->protocol  = eth_type_trans(skb, ptr_wan_device->ptr_net_device);

            /* Increment the receive statistics for the WAN Bridge device. */
            stats = &((WAN_BRIDGE_DEVICE_PRIV *)ptr_wan_device->ptr_net_device->priv)->stats;
            stats->rx_packets++;
            stats->rx_bytes+=skb->len;

            /* Debug Message. Packet has been formatted without VLAN and is being sent up the stack. */
            wblogMsg (WB_LOG_DEBUG, "DEBUG: WAN Bridge Protocol=0x%x PacketType=%d WANDevice=%s.\n",
                      skb->protocol, skb->pkt_type, (int)ptr_wan_device->ptr_net_device->name,0,0);

            /* Pass the packet up the IP stack. */
            netif_rx (skb);

            return 0;
        }
#endif
        default:
        {
            /* Error: Incorrect Mode. Drop the packet. */
            ptr_wan_bridge->rx_packet_dropped++;
            wb_kfree_skb (skb);
            return 0;
        }
    }
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_xmit_frame
 **************************************************************************
 * DESCRIPTION   :
 * This is the API Function that has been provided for sending a frame. 
 * This needs to be called by all the WAN Devices.
 *  
 * RETURNS       :
 *      0       - Success.
 *      < 0     - Error
 ***************************************************************************/
int wb_xmit_frame (struct sk_buff *skb, WAN_BRIDGE_DEVICE* ptr_wan_device)
{
    struct vlan_ethhdr *veth;
    int                 device_vlan_info = 0;

#ifdef WAN_BRIDGE_HALF_BRIDGE_MODE_SUPPORT
    /* Determine the mode in which the bridge is operating. */
    if (ptr_wan_device->ptr_wan_bridge->wb_mode == WAN_BRIDGE_HALF_BRIDGE_MODE)
    {
        /* TODO: This is a special case for handling for half bridge mode. 
         * For all other types of bridge modes, the packet is already formatted. 
         * and simply needs to be put out on the appropriate interface. */
        wblogMsg (WB_LOG_DEBUG,"DEBUG: Tx Half Bridge Mode\n",0,0,0,0,0);

        /* Did we learn the MAC address ? If not learnt it. */
        if (ptr_wan_device->ptr_wan_bridge->mac_address_learnt == 0)
        {
            memcpy((void *)&ptr_wan_device->ptr_wan_bridge->ethernet_header, (void *)skb->data, ETH_HLEN);
            ptr_wan_device->ptr_wan_bridge->mac_address_learnt = 1;
        }

        /* Pull out the ethernet header as the PPP will add the header. */
        skb_pull(skb, ETH_HLEN);
    }
#endif

    /* Determine the mode in which the bridge is operating. If operating in the VLAN Mode
     * verify that a VLAN header is present. If there is no VLAN header, add one. This
     * will be possible if the WAN bridge is attached to the local bridge and the LAN 
     * interfaces operate in a non-VLAN mode. Unfortunately we cannot use the skb->protocol
     * field anymore as the 'packet_sendmsg' overwrites our protocol value. Even if a VLAN 
     * header exists it should have the same VLAN Id as the WAN Bridge device, if not we change
     * the header. */
    veth = (struct vlan_ethhdr *)skb->data;
    wblogMsg (WB_LOG_DEBUG,"DEBUG: VEthernet is 0x%x.\n", veth->h_vlan_proto,0,0,0,0);
    if (ptr_wan_device->ptr_wan_bridge->wb_mode == WAN_BRIDGE_VLAN_MODE)
    {
        /* Bridge is in VLAN Mode. Does the packet have a VLAN Header ?*/
        if (veth->h_vlan_proto != VLAN_TYPE)
        {
            wblogMsg (WB_LOG_DEBUG,"DEBUG: VLAN Mode but no VLAN Header. Adding\n",0,0,0,0,0);

            /* We need to append the VLAN header. */
	        if (skb_headroom(skb) < VLAN_HLEN) 
            {
                struct sk_buff *sk_tmp = skb; 
                skb = skb_realloc_headroom(sk_tmp, VLAN_HLEN);
    	    	kfree_skb(sk_tmp);
                if (skb == NULL) 
                {
	        	    ptr_wan_device->ptr_wan_bridge->tx_packet_dropped++;
                    return -ENOMEM;
                }
	        }

            /* Get a pointer to the VLAN header. */
            veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

            /* Move the mac addresses to the beginning of the new header. */
            memmove(skb->data, skb->data + VLAN_HLEN, 12);

            /* first, the ethernet type */
            veth->h_vlan_proto = VLAN_TYPE;
        
            /* Set the VLAN Tag and user priority bits. */
            veth->h_vlan_TCI = htons((ptr_wan_device->user_priority << 13) | ptr_wan_device->vlan_key);
        }
        else
        {
            /* Bridge is in VLAN Mode and the packet being transmitted has a VLAN Header. Verify
             * if this matches the WAN Bridge devices VLAN information. */
            device_vlan_info = htons((ptr_wan_device->user_priority << 13) | ptr_wan_device->vlan_key);
            if (device_vlan_info != veth->h_vlan_TCI)
            {
                wblogMsg (WB_LOG_DEBUG,"DEBUG: Changing VLAN Header from 0x%x to 0x%x \n",
                          (int)veth->h_vlan_TCI,(int)device_vlan_info,0,0,0);
                veth->h_vlan_TCI = device_vlan_info;
            }
        }
    }

    /* Once we have the device information, send it to the source. */
    skb->dev = ptr_wan_device->ptr_wan_bridge->ptr_src_device;
    /* ptr_wan_device->ptr_wan_bridge->ptr_src_device->hard_start_xmit(skb, skb->dev); */
    dev_queue_xmit (skb);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_add
 **************************************************************************
 * DESCRIPTION   :
 *  This is the API exported to relate a key to the WAN Bridge device. When
 *  a packet is received, the key needs to be filled in by the source 
 *  interface. The WAN Bridge demultiplexes the packet on the basis of 
 *  the key. This is called when a WAN Bridge device is attached to the 
 *  bridge.
 *  
 * RETURNS       :
 *      0       - Success.
 *      < 0     - Error
 ***************************************************************************/
int wb_add (WAN_BRIDGE* ptr_wan_bridge, WAN_BRIDGE_DEVICE* ptr_wan_device, int vlan_key)
{
    /* Check if there is an existing VLAN device. */
    if (ptr_wan_bridge->vlan_hash_bucket[vlan_key] != 0)
    {
        wblogMsg (WB_LOG_FATAL, "ERROR: Duplicate VLAN:%d already present in bridge:%s\n",
                  vlan_key,(int)ptr_wan_bridge->name,0,0,0);
        return -1;
    }

    /* Create the entry. */
    ptr_wan_bridge->vlan_hash_bucket[vlan_key] = (unsigned int)ptr_wan_device;
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_delete
 **************************************************************************
 * DESCRIPTION   :
 *  This is the API exported to delete a key which is related to the WAN 
 *  bridge device. This function is called when an interface is deleted from
 *  the WAN bridge. 
 *  
 * RETURNS       :
 *      0       - Success.
 *      < 0     - Error
 ***************************************************************************/
int wb_delete (WAN_BRIDGE* ptr_wan_bridge, WAN_BRIDGE_DEVICE* ptr_wan_device, int vlan_key)
{
    /* Check if there is an existing VLAN device. */
    if (ptr_wan_bridge->vlan_hash_bucket[vlan_key] == 0)
    {
        wblogMsg (WB_LOG_FATAL, "ERROR: VLAN:%d does NOT exist on bridge:%s\n",
                  vlan_key,(int)ptr_wan_bridge->name,0,0,0);
        return -1;
    }

    /* Delete the entry. */
    ptr_wan_bridge->vlan_hash_bucket[vlan_key] = 0;
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_delete
 **************************************************************************
 * DESCRIPTION   :
 *  Wrapper Utility function which deletes the skb. 
 ***************************************************************************/
static void wb_kfree_skb(struct sk_buff *skb)
{
    dev_kfree_skb (skb);
    return;
} 

