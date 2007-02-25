#ifndef STBBRIDGE_H
#define STBBRIDGE_H

#define MAXSTBDEV 10
#define MAXSTBTYPE 4
#define MAX_CHAN   8
struct stb_info
{
	u_int32_t ipaddr;
	u_int32_t netmsk;

	u_int32_t xid;
	//u_int8_t mac[16];
};

extern void stbinfo_init();
extern struct stb_info *get_new_stbinfo();
extern struct stb_info *lookup_stbinfo(unsigned int id);
extern void stbmatchtable_init();
extern int relay_send(int fd, struct dhcpMessage *payload, u_int32_t dest_ip, int dest_port);
extern void stb_enable_inkernel(int i);
extern int stbinfo2kernel(int flag,struct stb_info *message,u_int8_t *hdaddr);
extern int stbnet2kernel(struct stb_info *message);
extern void stbinfo_del(unsigned int id);
int _getwanip(int index);
	
extern int getifname(int index, char *ifname);
#endif













