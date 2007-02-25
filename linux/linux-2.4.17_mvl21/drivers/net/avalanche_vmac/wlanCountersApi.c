/*******************************************************************************************
*
********************************************************************************************
* wlanCountersApi.c
*
*   This interface support the counters information of WLAN's LAN and WAN ports
*
*
******************************************************************************************** */

#include <linux/netdevice.h>

#include "marvell_vmac.h"
#include "wlanCountersApi.h"

struct UINT_64_t
{
	unsigned long  high;        /* the high (msb) 4 bytes of the uint */
	unsigned long  low;         /* the low (lsb) 4 bytes of the uint  */
};


struct mandatory64_t
{
	struct UINT_64_t inBytes;
	struct UINT_64_t inUnicastPkts;
	struct UINT_64_t inMulticastPkts;
	struct UINT_64_t inBroadcastPkts;
	struct UINT_64_t outBytes;
	struct UINT_64_t outUnicastPkts;
	struct UINT_64_t outMulticastPkts;
	struct UINT_64_t outBroadcastPkts;
};

/* the global 64 bit counter array */
struct mandatory64_t wlan_cnt64[2] = { {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}, 
				{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} };

/* Global Functions */ 
int vmac_mib(char* buf, char **start, off_t offset, int count,int *eof, void *data);

/* Extern Functions Declarations */ 
extern struct vmac_priv *vmac_get_private_structure(int dev_port);

/**************************************************************************
 *              void wlanMib2SetAdminStatus(int dev_port, int state)     *
 **************************************************************************
 * DESCRIPTION: Set Device management operation state                     *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN              			  *
 *              state - management state:                                 *
 *                 MIB2_STATUS_UP - to pass data                          *
 *                 MIB2_STATUS_DOWN - not to pass data                    *
 *                 MIB2_STATUS_TEST - as MIB2_STATUS_UP if test mode is   *
 *                                    implemented                         *
 * OUTPUT:      no.                                                       *
 * RETURN:      no.                                                       *
 *                                                                        *
 **************************************************************************/
void wlanMib2SetAdminStatus(int dev_port, int state)
{
	struct vmac_priv *tp;

	/* Get the Driver's private Info data Structure for dev port */
	tp = (struct vmac_priv *) vmac_get_private_structure(dev_port);

	/* Set the Admin State */
	switch( state )
	{
		case MIB2_STATUS_UP:
			/* Activate the interface to send data */
			tp->mib_counters.ifOperStatus = 1;
			break;

		case MIB2_STATUS_DOWN:
			/* Deactivate the Rx and Tx */
			tp->mib_counters.ifOperStatus = 0;
			break;

		case MIB2_STATUS_TEST:
			/* Testing */
			break;
		default:
			break;
	}
	return;	
}

/**************************************************************************
 *              int wlanMib2GetOperStatus(int dev_port)                  *
 **************************************************************************
 * DESCRIPTION: Get Device physical connection state                      *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN             				  *
 * OUTPUT:      no.                                                       *
 * RETURN:      MIB2_STATUS_UP if connected, otherwise MIB2_STATUS_DOWN.  *
 *                                                                        *
 **************************************************************************/

int wlanMib2GetOperStatus(int dev_port)
{
	struct vmac_priv *tp;

	/* Get the Driver's private Info data Structure for dev port */
	tp = (struct vmac_priv *) vmac_get_private_structure(dev_port);

	if( tp == NULL )
		return MIB2_STATUS_DOWN;	

	if (tp->mib_counters.ifOperStatus == 1)
		return MIB2_STATUS_UP;
	
	if (tp->mib_counters.ifOperStatus == 0)
		return MIB2_STATUS_DOWN;
	
	return MIB2_STATUS_TEST;
}

/**************************************************************************
 * unsigned long wlanMib2GetCounter(int dev_port, MIB2_CounterType type) *
 **************************************************************************
 * DESCRIPTION: Get Device counter value of specified type                *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN             				  *
 *              type - counter type                                       *
 * OUTPUT:      no                                                        *
 * RETURN:      counted value, 0 if counter not supported                 *
 *                                                                        *
 **************************************************************************/

unsigned long wlanMib2GetCounter(int dev_port, MIB2_CounterType type)
{
	unsigned long lResult = 0L;
	struct vmac_priv *tp;

	/* Get the Driver's private Info data Structure for dev port */
	tp = (struct vmac_priv *) vmac_get_private_structure(dev_port);

	/* On error do nothing */
	if( tp == NULL )
		return lResult;	

	switch(type)
	{
		/* inoctets_low  */
    		case InBytes:
		/* inoctets_high */
    		case InBytesHigh:
    			lResult = tp->mib_counters.ifInOctets;
			if( lResult < wlan_cnt64[dev_port].inBytes.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].inBytes.high;
				wlan_cnt64[dev_port].inBytes.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].inBytes.low = lResult;
			}
			if( type==InBytesHigh)	
				lResult = wlan_cnt64[dev_port].inBytes.high;
			break;

		/* inunicasts */
    		case InUnicastPkts:
		/* inunicast_high */
    		case InUnicastPktsHigh:
			lResult = tp->mib_counters.ifInUnicasts;
			if( lResult < wlan_cnt64[dev_port].inUnicastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].inUnicastPkts.high;
				wlan_cnt64[dev_port].inUnicastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].inUnicastPkts.low = lResult;
			}
			if( type==InUnicastPktsHigh)	
				lResult = wlan_cnt64[dev_port].inUnicastPkts.high;
			break;			

		/* inmulticats */
    		case InMulticastPkts:
		/* inmulticast_high*/
    		case InMulticastPktsHigh:
			lResult = tp->mib_counters.ifInMulticasts;
			if( lResult < wlan_cnt64[dev_port].inMulticastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].inMulticastPkts.high;
				wlan_cnt64[dev_port].inMulticastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].inMulticastPkts.low = lResult;
			}
			if( type==InMulticastPktsHigh)	
				lResult = wlan_cnt64[dev_port].inMulticastPkts.high;
			break;	

		/* inbroadcasts */
    		case InBroadcastPkts:
		/* inbroadcast_high */
    		case InBroadcastPktsHigh:
			lResult = tp->mib_counters.ifInBroadcasts;
			if( lResult < wlan_cnt64[dev_port].inBroadcastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].inBroadcastPkts.high;
				wlan_cnt64[dev_port].inBroadcastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].inBroadcastPkts.low = lResult;
			}
			if( type==InBroadcastPktsHigh)	
				lResult = wlan_cnt64[dev_port].inBroadcastPkts.high;
			break;	

		/* indiscards */
    		case InDiscardPkts:
			lResult = tp->mib_counters.ifInDiscards;	
			break;

		/* inerrors */
		case InErrorPkts:
			lResult = tp->mib_counters.ifInErrors;	
			break;
	
		/* inunknown_protocols */
    		case InUnknownProtPkts:
			lResult = 0L;	
			break;

		/* outoctets_low */
    		case OutBytes:
		/* outoctets_high */
    		case OutBytesHigh:
   			lResult = tp->mib_counters.ifOutOctets;
			if( lResult < wlan_cnt64[dev_port].outBytes.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].outBytes.high;
				wlan_cnt64[dev_port].outBytes.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].outBytes.low = lResult;
			}
			if( type==OutBytesHigh)	
				lResult = wlan_cnt64[dev_port].outBytes.high;
			break;

		/* outunicasts */
    		case OutUnicastPkts:
	    	case OutUnicastPktsHigh:
			lResult = tp->mib_counters.ifOutUnicasts;
			if( lResult < wlan_cnt64[dev_port].outUnicastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].outUnicastPkts.high;
				wlan_cnt64[dev_port].outUnicastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].outUnicastPkts.low = lResult;
			}
			if( type==OutUnicastPktsHigh)	
				lResult = wlan_cnt64[dev_port].outUnicastPkts.high;
			break;		

		/* outmulticats */
	    	case OutMulticastPkts:
	    	case OutMulticastPktsHigh:
			lResult = tp->mib_counters.ifOutMulticasts;
			if( lResult < wlan_cnt64[dev_port].outMulticastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].outMulticastPkts.high;
				wlan_cnt64[dev_port].outMulticastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].outMulticastPkts.low = lResult;
			}
			if( type==OutMulticastPktsHigh)	
				lResult = wlan_cnt64[dev_port].outMulticastPkts.high;
			break;	

		/* outbroadcasts */
	    	case OutBroadcastPkts:
	    	case OutBroadcastPktsHigh:
			lResult = tp->mib_counters.ifOutBroadcasts;
			if( lResult < wlan_cnt64[dev_port].outBroadcastPkts.low)
			{
				/* wrap around 4G */
				++wlan_cnt64[dev_port].outBroadcastPkts.high;
				wlan_cnt64[dev_port].outBroadcastPkts.low = lResult;
			}
			else
			{
				wlan_cnt64[dev_port].outBroadcastPkts.low = lResult;
			}
			if( type==OutBroadcastPktsHigh)	
				lResult = wlan_cnt64[dev_port].outBroadcastPkts.high;
			break;	

		/* outdiscards */
	    	case OutDiscardPkts:
			lResult = tp->mib_counters.ifOutDiscards;	
			break;

		/* outerrors */
	    	case OutErrorPkts:
			lResult = tp->mib_counters.ifOutErrors;	
			break;

		default:
		       lResult = 0L;
	}

	return lResult;
}

/**************************************************************************
 *       unsigned long wlanMib2GetSpeed(int dev_port)                    *
 **************************************************************************
 * DESCRIPTION: Get Device current speed in Hz                            *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN             				  *
 * OUTPUT:      no.                                                       *
 * RETURN:      speed value                                               *
 *                                                                        *
 **************************************************************************/

unsigned long wlanMib2GetSpeed(int dev_port)
{
	unsigned long ulSpeed = 0;

	return ulSpeed;
}


/**************************************************************************
 *              int wlanMib2GetPromiscuousMode(int dev_port)              *
 **************************************************************************
 * DESCRIPTION: Get Device promiscuous mode                               *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN             				  *
 * OUTPUT:      no.                                                       *
 * RETURN:      TRUE(1) or FALSE(0)                                       *
 *                                                                        *
 **************************************************************************/
int wlanMib2GetPromiscuousMode(int dev_port)
{

	/* MAC is always in the promiscous mode when the Marvell switch 
	 * is connected */	
	return 1;
}

/**************************************************************************
 *              int wlanMib2UpdateHighCounters(int dev_port)              *
 **************************************************************************
 * DESCRIPTION: Updates 64 bit counters. Should be called periodically by *
 * the application. otherwise high counters will fail to run              *
 * Affected counters: inbytes, outbytes                                   *
 *                                                                        *
 * INPUT:       dev_port - 0 : WAN					  *
 * 			   1 : LAN             				  *
 * OUTPUT:      no.                                                       *
 * RETURN:      none                                                      *
 *                                                                        *
 **************************************************************************/
void wlanMib2UpdateHighCounters(int dev_port)
{
	/* force refresh for we won't miss the wrap around */
	wlanMib2GetCounter(dev_port, InBytes);
	wlanMib2GetCounter(dev_port, InUnicastPkts);
	wlanMib2GetCounter(dev_port, InMulticastPkts);
	wlanMib2GetCounter(dev_port, InBroadcastPkts);
	wlanMib2GetCounter(dev_port, OutBytes);
	wlanMib2GetCounter(dev_port, OutUnicastPkts);
	wlanMib2GetCounter(dev_port, OutMulticastPkts);
	wlanMib2GetCounter(dev_port, OutBroadcastPkts);

}

/**************************************************************************
 *              int testwlanMib2Counters(int dev_port, char* buf, int len)*
 **************************************************************************/
int testwlanMib2Counters(int dev_port, char* buf, int len)
{
	len += sprintf (buf+len, "InBytes: %lu\n", wlanMib2GetCounter(dev_port, InBytes));
	len += sprintf (buf+len, "InBytesHigh: %lu\n", wlanMib2GetCounter(dev_port, InBytesHigh));
	len += sprintf (buf+len, "InUnicastPkts: %lu\n", wlanMib2GetCounter(dev_port, InUnicastPkts));
	len += sprintf (buf+len, "InMulticastPkts: %lu\n", wlanMib2GetCounter(dev_port, InMulticastPkts));
	len += sprintf (buf+len, "InBroadcastPkts: %lu\n", wlanMib2GetCounter(dev_port, InBroadcastPkts));
	len += sprintf (buf+len, "InDiscardPkts: %lu\n", wlanMib2GetCounter(dev_port, InDiscardPkts));
	len += sprintf (buf+len, "InErrorPkts: %lu\n", wlanMib2GetCounter(dev_port, InErrorPkts));
	len += sprintf (buf+len, "InUnknownProtPkts: %lu\n", wlanMib2GetCounter(dev_port, InUnknownProtPkts));

	len += sprintf (buf+len, "OutBytes: %lu\n", wlanMib2GetCounter(dev_port, OutBytes));
	len += sprintf (buf+len, "OutBytesHigh: %lu\n", wlanMib2GetCounter(dev_port, OutBytesHigh));
	len += sprintf (buf+len, "OutUnicastPkts: %lu\n", wlanMib2GetCounter(dev_port, OutUnicastPkts));
	len += sprintf (buf+len, "OutMulticastPkts: %lu\n", wlanMib2GetCounter(dev_port, OutMulticastPkts));
	len += sprintf (buf+len, "OutBroadcastPkts: %lu\n", wlanMib2GetCounter(dev_port, OutBroadcastPkts));
	len += sprintf (buf+len, "OutDiscardPkts: %lu\n", wlanMib2GetCounter(dev_port, OutDiscardPkts));
	len += sprintf (buf+len, "OutErrorPkts: %lu\n", wlanMib2GetCounter(dev_port, OutErrorPkts));
	len += sprintf (buf+len, "Operation Status: %d\n", wlanMib2GetOperStatus(dev_port));
	return len;
}

/***********************************************************************************************
 *              vmac_mib(char* buf, char **start, off_t offset, int count,int *eof, void *data)*
 ***********************************************************************************************/
int vmac_mib(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	int len = 0;
	
#ifdef CONFIG_MIPS_WA1130
	len += sprintf(buf+len, "\nWAN\n");
	len = testwlanMib2Counters(4, buf, len);
#endif

	len += sprintf(buf+len, "\nLAN\n");
	len = testwlanMib2Counters(1, buf, len);
}
