#ifndef _LINUX_ATMBR2684_H
#define _LINUX_ATMBR2684_H

#include <linux/atm.h>
#include <linux/if.h>		/* For IFNAMSIZ */

/*
 * Type of media we're bridging (ethernet, token ring, etc)  Currently only
 * ethernet is supported
 */
#define BR2684_MEDIA_ETHERNET	(0)	/* 802.3 */
#define BR2684_MEDIA_802_4	(1)	/* 802.4 */
#define BR2684_MEDIA_TR		(2)	/* 802.5 - token ring */
#define BR2684_MEDIA_FDDI	(3)
#define BR2684_MEDIA_802_6	(4)	/* 802.6 */

/*
 * Is there FCS inbound on this VC?  This currently isn't supported.
 */
#define BR2684_FCSIN_NO		(0)
#define BR2684_FCSIN_IGNORE	(1)
#define BR2684_FCSIN_VERIFY	(2)

/*
 * Is there FCS outbound on this 2684_ENCAPS_LLC        (1)
 * #define BR2684_ENCAPS_AUTODETECT (2)
 */
#define BR2684_FCSOUT_NO	(0)
#define BR2684_FCSOUT_SENDZERO	(1)
#define BR2684_FCSOUT_GENERATE	(2)

/*
 * Does this  include LLC encapsulation?
 */
#define BR2684_ENCAPS_LLC	 (0)	/* Bridged LLC */
#define BR2684_ENCAPS_VC	 (1)	/* Bridged VC-mux */
#define BR2684_ENCAPS_AUTODETECT (2)	/* Unsuported */
#define BR2684_ENCAPS_LLC_ROUTED (3)	/* Routed LLC */
#define BR2684_ENCAPS_VC_ROUTED  (4)	/* Routed VC-Mux */

/*
 * This is for the ATM_NEWBACKENDIF call - these are like socket families:
 2684_ENCAPS element of the structure is the backend number and the rest
 * is per-backend specific
 */
struct atm_newif_br2684 {
	atm_backend_t	backend_num;	/* ATM_BACKEND_BR2684 */
	int		media;		/* BR2684_MEDIA_* */
	char		ifname[IFNAMSIZ];
	int		mtu;
};

/*
 * This structure is used to specify a br2684 interface - either by a
 * positive integer (returned by ATM_NEWBACKENDIF) or the interfaces name
 */
#define BR2684_FIND_BYNOTHING	(0)
#define BR2684_FIND_BYNUM	(1)
#define BR2684_FIND_BYIFNAME	(2)
struct br2684_if_spec {
	int method;			/* BR2684_FIND_* */
	union {
		char		ifname[IFNAMSIZ];
		int		devnum;
	} spec;
};

/*
 * This is for the ATM_SETBACKEND call - these are like socket families:
 * the first element of the structure is the backend number and the rest
 * is per-backend specific
 */
struct atm_backend_br2684 {
	atm_backend_t	backend_num;	/* ATM_BACKEND_BR2684 */
	struct br2684_if_spec ifspec;
	int	fcs_in;		/* BR2684_FCSIN_* */
	int	fcs_out;	/* BR2684_FCSOUT_* */
	int	fcs_auto;	/* 1: fcs_{in,out} disabled if no FCS rx'ed */
	int	encaps;		/* BR2684_ENCAPS_* */
	int	has_vpiid;	/* 1: use vpn_id - Unsupported */
	__u8	vpn_id[7];
	int	send_padding;	/* unsupported */
	int	min_size;	/* we will pad smaller packets than this */
};

/*
 * The BR2684_SETFILT ioctl is an experimental mechanism for folks
 * terminating a large number of IP-only vcc's.  When netfilter allows
 * efficient per-if in/out filters, this support will be removed
 */
struct br2684_filter {
	__u32	prefix;		/* network byte order */
	__u32	netmask;	/* 0 = disable filter */
};

struct br2684_filter_set {
	struct br2684_if_spec ifspec;
	struct br2684_filter filter;
};

#define BR2684_SETFILT	_IOW( 'a', ATMIOC_BACKEND + 0, \
				struct br2684_filter_set)

#define ETHERTYPE_ARP   0x0806
#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_IPX   0x8138

#define ARPOP_REQUEST   1
#define ARPOP_REPLY	2
#define ARP_REVREQUEST  3
#define ARP_REVREPLY    4

#define ISC_MAX_ETH_SIZE        1514

__u8      rfc2684_routed_mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

/******
* Ethernet Header
******/
struct ether_header
{
	__u8	ether_dhost[6];		// Ethernet detination address
	__u8	ether_shost[6];		// Ethernet source address
	__u16	ether_type;		// Ethernet Frame type
};

/******
 * * Common ARP request /reply header
 * *******/
struct arpheader
{
	__u16     ar_hrd;		// format of hardware address
	__u16     ar_pro;		// format of protocol address
	__u8      ar_hln;		// length of hardware address
	__u8      ar_pln;		// length of protocol address
	__u16     ar_op;		// ARP/RARP operation
};

/******
 * * Ethernet ARP structure
 * ******/
struct ether_arp
{
	struct arpheader ea_hdr;	// fixed-size header
	__u8	arp_sha[6];		// sender hardware address
	__u8	arp_spa[4];		// sender protocol address
	__u8	arp_tha[6];		// target hardware adress
	__u8	arp_tpa[4];		// target protocol address
};

/******
* BR2684 Routed Header
******/
struct routed2684_header
{
	__u8	routed2684_llc[3];	// Routed 2684 LLC (0xAA-AA-03)
	__u8	routed2684_oui[3];	// Routed 2684 OUI (0x00-00-00)
	__u16	ethertype;		// Ethernet Frame type
};

#endif /* _LINUX_ATMBR2684_H */
