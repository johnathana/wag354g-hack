#ifndef _IPT_MIP_H
#define _IPT_MIP_H
#include <linux/netfilter_ipv4/ip_tables.h>

enum ipt_multiip_flags
{
    IPT_MULTIIP_SOURCE
};

#define IPT_MULTIIP_NUM 16

/* Values for flags field */
#define IPT_MIP_INV_MATCH   0x80   /* Invert the match */
#define IPT_MIP_NORM_MATCH  0x00   /* Normal Match     */

struct ipt_multiip
{
    u_int8_t flags;                         /* TBD */
    u_int8_t count;                         /* Number of IPs */
    u_int32_t multip[IPT_MULTIIP_NUM];      /* Source IP address */
};


#endif /* _IPT_MIP_H */
    
     
