#ifndef STB_H
#define STB_H

#define MAX_STB_NUM  8
#define IPONLY 1
#define MACONLY 2
#define IPMAC 3

#define my_printk printk
struct stb_dev_info
{
	unsigned int stb_ipaddr;
	unsigned int stb_netmask;
	unsigned short used;
	unsigned char stb_macaddr[6];
};	

extern void stbinfo_init();
extern struct stb_dev_info *find_stbinfo(unsigned int type, unsigned int ipaddr, unsigned char *macaddr, struct stb_dev_info **freestb);

#endif
