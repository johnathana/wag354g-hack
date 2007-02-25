#ifndef AUTODETECT_H
#define AUTODETECT_H

#define ETH_ALEN 6

/* PPPOE OP code */
#define CODE_PADI 0x09
#define CODE_PADO 0x07

/* LCP OP code */
#define CONFREQ 1
#define CONFACK 2

#define MYID 0x05
#define TAG_SERVICE_NAME 0x0101
#define TAG_HDE_SIZE 4
#define HDR_SIZE (sizeof(struct ethhdr) + 6)

struct PPPoEPacket
{
	struct ethhdr ethHdr;
	unsigned int ver:4;
	unsigned int type:4;
	unsigned int code:8;
	unsigned int session:16;
	unsigned int length:16;
	unsigned char payload[ETH_DATA_LEN];
};

struct PPPoETag
{
	unsigned int type:16;
	unsigned int length:16;
	unsigned char payload[ETH_DATA_LEN];
};

struct PPPoALhdr
{
	unsigned short llc;
	unsigned char ctrl;
	unsigned char nlpid;
	unsigned short ppp_proto;
};

struct PPPoAVhdr
{
	unsigned short ppp_proto;
};

#define ETHERTYPE_IP		0x0800
#define ETHERTYPE_ARP		0x0806
#define ARPHRD_ETHER		0x01

#define IPPACKET_SIZE		1500
#define MAGIC_COOKIE		0x63825363
#define DEFAULT_LEASETIME 	0xffffffff
#define BROADCAST_FLAG		0x8000

/* UDP port numbers for DHCP */
#define	DHCP_SERVER_PORT	67	/* from client to server */
#define DHCP_CLIENT_PORT	68	/* from server to client */

/* DHCP message OP code */
#define DHCP_BOOTREQUEST	1
#define DHCP_BOOTREPLY		2

/* DHCP message type */
#define	DHCP_DISCOVER		1
#define DHCP_OFFER	  	2

/* DHCP RETRANSMISSION TIMEOUT (microseconds) */
#define DHCP_INITIAL_RTO    ( 4*1000000)
#define DHCP_MAX_RTO        (64*1000000)

/* DHCP option and value (cf. RFC1533) */
enum
{
  padOption				=	0,
  subnetMask				=	1,
  timerOffset				=	2,
  routersOnSubnet			=	3,
  timeServer				=	4,
  nameServer				=	5,
  dns					=	6,
  logServer				=	7,
  cookieServer				=	8,
  lprServer				=	9,
  impressServer				=	10,
  resourceLocationServer		=	11,
  hostName				=	12,
  bootFileSize				=	13,
  meritDumpFile				=	14,
  domainName				=	15,
  swapServer				=	16,
  rootPath				=	17,
  extentionsPath			=	18,
  IPforwarding				=	19,
  nonLocalSourceRouting			=	20,
  policyFilter				=	21,
  maxDgramReasmSize			=	22,
  defaultIPTTL				=	23,
  pathMTUagingTimeout			=	24,
  pathMTUplateauTable			=	25,
  ifMTU					=	26,
  allSubnetsLocal			=	27,
  broadcastAddr				=	28,
  performMaskDiscovery			=	29,
  maskSupplier				=	30,
  performRouterDiscovery		=	31,
  routerSolicitationAddr		=	32,
  staticRoute				=	33,
  trailerEncapsulation			=	34,
  arpCacheTimeout			=	35,
  ethernetEncapsulation			=	36,
  tcpDefaultTTL				=	37,
  tcpKeepaliveInterval			=	38,
  tcpKeepaliveGarbage			=	39,
  nisDomainName				=	40,
  nisServers				=	41,
  ntpServers				=	42,
  vendorSpecificInfo			=	43,
  netBIOSnameServer			=	44,
  netBIOSdgramDistServer		=	45,
  netBIOSnodeType			=	46,
  netBIOSscope				=	47,
  xFontServer				=	48,
  xDisplayManager			=	49,
  dhcpRequestedIPaddr			=	50,
  dhcpIPaddrLeaseTime			=	51,
  dhcpOptionOverload			=	52,
  dhcpMessageType			=	53,
  dhcpServerIdentifier			=	54,
  dhcpParamRequest			=	55,
  dhcpMsg				=	56,
  dhcpMaxMsgSize			=	57,
  dhcpT1value				=	58,
  dhcpT2value				=	59,
  dhcpClassIdentifier			=	60,
  dhcpClientIdentifier			=	61,
  endOption				=	255
};

typedef struct dhcpMessage
{
  u_char  op;		/* message type */
  u_char  htype;	/* hardware address type */
  u_char  hlen;		/* hardware address length */
  u_char  hops;		/* should be zero in client's message */
  u_int   xid;		/* transaction id */
  u_short secs;		/* elapsed time in sec. from trying to boot */
  u_short flags;
  u_int   ciaddr;	/* (previously allocated) client IP address */
  u_int	  yiaddr;	/* 'your' client IP address */
  u_int	  siaddr;	/* should be zero in client's messages */
  u_int	  giaddr;	/* should be zero in client's messages */
  u_char  chaddr[16];	/* client's hardware address */
  u_char  sname[64];	/* server host name, null terminated string */
  u_char  file[128];	/* boot file name, null terminated string */
  u_char  options[312];	/* message options */
} __attribute__((packed)) dhcpMessage;

struct ipovly
{
	int ih_next,ih_prev;
	u_char ih_x1;
	u_char ih_pr;
	u_short ih_len;
	struct in_addr ih_src;
	struct in_addr ih_dst;
} __attribute__((packed));


struct packed_ether_header 
{
  u_int8_t  ether_dhost[ETH_ALEN];      /* destination eth addr */
  u_int8_t  ether_shost[ETH_ALEN];      /* source ether addr    */
  u_int16_t ether_type;                 /* packet type ID field */
} __attribute__((packed));

typedef struct udpipMessage
{
  struct packed_ether_header	ethhdr;
  char	 udpipmsg[IPPACKET_SIZE];
} __attribute__((packed)) udpipMessage;

typedef struct dhcpOptions
{
  u_char num;
  u_char len[256];
  void   *val[256];
} __attribute__((packed)) dhcpOptions;


#define IPDEFTTL 64

struct udphdr
{
  u_int16_t uh_sport;
  u_int16_t uh_dport;
  u_int16_t uh_ulen;
  u_int16_t uh_sum;
} __attribute__((packed));

typedef struct udpiphdr
{
  char ip[sizeof(struct iphdr)];
  char udp[sizeof(struct udphdr)];
} __attribute__((packed)) udpiphdr;

#endif
