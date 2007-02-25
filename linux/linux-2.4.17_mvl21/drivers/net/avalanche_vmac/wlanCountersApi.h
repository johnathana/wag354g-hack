/**************************************************************************/
/* wlanCountersApi.h                                                      */
/*                                                                        */
/*   This interface support the MIB-2 counters and management information */
/*   for LAN and WAN drivers                                              */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
#ifndef _WLAN_COUNTERS_API_H_
#define _WLAN_COUNTERS_API_H_

/**************************************************************************/
/*                 MIB 2 Admin/Oper Status Values                         */
/**************************************************************************/
/* Device is in operational status unless status is down. */
#define MIB2_STATUS_UP     	1
#define MIB2_STATUS_DOWN   	2
#define MIB2_STATUS_TEST   	3
#define MIB2_STATUS_UNKNOWN	4
#define MIB2_STATUS_DORMANT	5

/**************************************************************************/
/*                 MIB 2 byte/packet Counter Types                        */
/**************************************************************************/
typedef enum
{
    InBytes = 0,
    InBytesHigh,
    InUnicastPkts,
    InUnicastPktsHigh,
    InMulticastPkts,
    InMulticastPktsHigh,
    InBroadcastPkts,
    InBroadcastPktsHigh,
    InDiscardPkts,
    InErrorPkts,
    InUnknownProtPkts,
    OutBytes,
    OutBytesHigh,
    OutUnicastPkts,
    OutUnicastPktsHigh,
    OutMulticastPkts,
    OutMulticastPktsHigh,
    OutBroadcastPkts,
    OutBroadcastPktsHigh,
    OutDiscardPkts,
    OutErrorPkts
} MIB2_CounterType;


/**************************************************************************/
/*                 MIB 2 Function Prototypes                              */
/**************************************************************************/

/**************************************************************************
 *              void wlanMib2SetAdminStatus(int dev_port, int state)       *
 **************************************************************************
 * DESCRIPTION: Set Device management operation state                     *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 *              state - management state:                                 *
 *                 MIB2_STATUS_UP - to pass data                          *
 *                 MIB2_STATUS_DOWN - not to pass data                    *
 *                 MIB2_STATUS_TEST - as MIB2_STATUS_UP if test mode is   *
 *                                    implemented                         *
 * OUTPUT:      no.                                                       *
 * RETURN:      no.                                                       *
 *                                                                        *
 **************************************************************************/
void wlanMib2SetAdminStatus(int dev_port, int state);

/**************************************************************************
 *              int wlanMib2GetOperStatus(int dev_port)                    *
 **************************************************************************
 * DESCRIPTION: Get Device physical connection state                      *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 * OUTPUT:      no.                                                       *
 * RETURN:      MIB2_STATUS_UP if connected, otherwise MIB2_STATUS_DOWN.  *
 *                                                                        *
 **************************************************************************/
int wlanMib2GetOperStatus(int dev_port) ;

/**************************************************************************
 * unsigned long wlanMib2GetCounter(int dev_port, MIB2_CounterType type)   *
 **************************************************************************
 * DESCRIPTION: Get Device counter value of specified type                *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 *              type - counter type                                       *
 * OUTPUT:      no                                                        *
 * RETURN:      counted value, 0 if counter not supported                 *
 *                                                                        *
 **************************************************************************/
unsigned long wlanMib2GetCounter(int dev_port, MIB2_CounterType type);


/**************************************************************************
 *       unsigned long wlanMib2GetSpeed(int dev_port)                    *
 **************************************************************************
 * DESCRIPTION: Get Device speed in Hz                                    *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 * OUTPUT:      no.                                                       *
 * RETURN:      speed value                                               *
 *                                                                        *
 **************************************************************************/
unsigned long wlanMib2GetSpeed(int dev_port);


/**************************************************************************
 *              int wlanMib2GetPromiscuousMode(int dev_port)               *
 **************************************************************************
 * DESCRIPTION: Get Device promiscuous mode                               *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 * OUTPUT:      no.                                                       *
 * RETURN:      TRUE(1) or FALSE(0)                                       *
 *                                                                        *
 **************************************************************************/
int wlanMib2GetPromiscuousMode(int dev_port) ;

/**************************************************************************
 *              int wlanMib2UpdateHighCounters(int dev_port)               *
 **************************************************************************
 * DESCRIPTION: Updates 64 bit counters. Should be called periodically by *
 * the application. otherwise high counters will fail to run              *
 * Affected counters: inbytes, outbytes                                   *
 *                                                                        *
 * INPUT:       dev_port - port number for multiport devices              *
 * OUTPUT:      no.                                                       *
 * RETURN:      none                                                      *
 *                                                                        *
 **************************************************************************/
void wlanMib2UpdateHighCounters(int dev_port);

#endif /* _WLAN_COUNTERS_API_H_ */





