#ifndef _IPT_MAC_H
#define _IPT_MAC_H
/*
struct ipt_mac_info {
    unsigned char srcaddr[ETH_ALEN];
    int invert;
};
*/

enum ipt_mac_flags
{
	IPT_MAC_SOURCE=1,
	IPT_MAC_DESTINATION
};

#define SRCMAC_MASK 0
#define DSTMAC_MASK 1

struct ipt_mac_info {
    u_int8_t flags;
    unsigned char srcaddr[ETH_ALEN];
    unsigned char dstaddr[ETH_ALEN];
    int invert;
};

#endif /*_IPT_MAC_H*/
