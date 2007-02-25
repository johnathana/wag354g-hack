#ifndef _IPT_PACKET_H
#define _IPT_PACKET_H

struct ipt_packet_info {
	unsigned long len;
	unsigned short proto;
#ifdef CONFIG_VLAN_8021Q
	unsigned short vid;
	unsigned short cos;
#endif
	unsigned short invert;
};

#define IPT_PACKET_INV_LEN 0x01
#define IPT_PACKET_INV_PROTO 0x02
#ifdef CONFIG_VLAN_8021Q
#define IPT_PACKET_INV_VID 0x04
#define IPT_PACKET_INV_COS 0x08
#endif
#endif /*_IPT_PACKET_H*/
