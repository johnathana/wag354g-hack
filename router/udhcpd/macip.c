#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "packet.h"
#include "debug.h"
#include "dhcpd.h"
#include "options.h"
#include "leases.h"

#include <shutils.h>

/* da_f 2005.6.21 add */
#define cprintf(str, arg...)

struct hostnameip hostname_ip[MAX_NUM];
struct macip mac_ip[MAX_NUM];
struct ShareWANIP share_wan_ip[2];

int share_wanip_count = 0;
 int mac_ip_count = 0;
 int hostname_ip_count = 0;
 u_int32_t single_pc_mode_gw = 0;

#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

	


char str_sep[] =  "$";


u_int32_t  find_ip_by_mac(u_int8_t *hwaddr)
{
	int i = 0;

	for (i = 0; i < mac_ip_count; i++)
	{
		if (!memcmp(mac_ip[i].macaddr, hwaddr, MAC_LEN))
		{
			return mac_ip[i].ipaddr;
		}
	}
	return 0;
}

u_int32_t find_ip_by_hostname(u_int8_t *name)
{
	int i = 0;
	for (i = 0; i < hostname_ip_count; i++)
	{
		if (!strcmp(hostname_ip[i].hostname, name))
		{
			return hostname_ip[i].ipaddr;
		}
	}
	return 0;
}
static int ether_atoe(const char *a, unsigned char *e)
{
	char *c = (char *) a;
	int i = 0;

	memset(e, 0, MAC_LEN);
	for (;;) {
		e[i++] = (unsigned char) strtoul(c, &c, 16);
		if (!*c++ || i == MAC_LEN)
			break;
	}
	return (i == MAC_LEN);
}

int read_macip(char *line, char *value)
{
	char  var[63];
	char *next, *mac, *ipaddr;
	int i = 0;
	char tmp[12];
	
	//memset((void *)&mac_ip, 0, sizeof(mac_ip));
	i = 0;
	//cprintf("line=%s\n",line);
	foreach(var, line, next)
	{
		mac = ipaddr = var;
		strsep(&ipaddr, str_sep);
		ether_atoe(mac, tmp);
		
		memcpy(&mac_ip[mac_ip_count].macaddr[0], tmp, MAC_LEN);
		mac_ip[mac_ip_count].ipaddr = inet_addr(ipaddr);
		mac_ip_count++;
	}
	
	return 1;
}

int read_hostnameip(char *line, char *value)
{
	char  var[63];
	char *next, *hostname, *ipaddr;
	
	//memset((void *)&hostname_ip, 0, sizeof(hostname_ip));
	foreach(var, line, next)
	{
		hostname = ipaddr = var;
		strsep(&ipaddr, str_sep);
		
		strcpy(hostname_ip[hostname_ip_count].hostname, hostname);
		hostname_ip[hostname_ip_count].ipaddr = inet_addr(ipaddr);
		hostname_ip_count++;
	}
	
	return 1;
}

#define		PPP_FLAGS		0x80

int get_wanip_by_index(struct ShareWANIP *cur_wanip)
{
	int tmp_index = cur_wanip->index;
	char readbuf[128], readbuf1[128];
	char ip_addrs[8][32];
	int have_ppp_flag = 0, ppp_index = 0;
	
	memset(ip_addrs, 0, sizeof(ip_addrs));
	cur_wanip->ipaddr = 0;
	cur_wanip->gwaddr = 0;
	//get the wan ip by index, if not active ,the ip = 0;
	if (tmp_index < 0) return 0;
	if (tmp_index == 255) //get the first wan interface ip
	{
		tmp_index = atoi((char*)nvram_get("wan_active_connection"));
		if ((tmp_index < 0) || (tmp_index > 8)) return 0;
		if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
		{
			printf("no buf in %s!\n", WAN_IFNAME);
	        return 0;
		}
		sscanf(readbuf, "%s %s %s %s %s %s %s %s", ip_addrs[0], ip_addrs[1], ip_addrs[2], ip_addrs[3], ip_addrs[4], ip_addrs[5], ip_addrs[6], ip_addrs[7]);  
		if(!strcmp(ip_addrs[tmp_index], "Down")) return 0;
		if(strstr(ip_addrs[tmp_index], "ppp")) tmp_index |= 0x80;
	}
	if (tmp_index & PPP_FLAGS) have_ppp_flag = 1;
	else have_ppp_flag = 0;
	
	tmp_index &= 0x3f;
	if (tmp_index < 8) //8 pvc
	{
		if(!file_to_buf(WAN_IPADDR, readbuf, sizeof(readbuf)))	
		{		
			printf("no buf in %s!\n", WAN_IFNAME);
			return 0;	
		}
		if(!file_to_buf(WAN_GATEWAY, readbuf1, sizeof(readbuf1)))	
		{		
			printf("no buf in %s!\n", WAN_GATEWAY);
			return 0;	
		}
		if (have_ppp_flag) ppp_index = tmp_index;
	}
#if defined(MPPPOE_SUPPORT)
	else if (tmp_index < 0x10)//multi-pppoe
	{
		tmp_index &= 0x07;
		if(!file_to_buf(WAN_MPPPOE_IPADDR, readbuf, sizeof(readbuf)))	
		{		
			printf("no buf in %s!\n", WAN_MPPPOE_IPADDR);	        
			return 0;	
		}
		if(!file_to_buf(WAN_MPPPOE_GATEWAY, readbuf1, sizeof(readbuf1)))	
		{		
			printf("no buf in %s!\n", WAN_MPPPOE_GATEWAY);
			return 0;	
		}
		if (have_ppp_flag) ppp_index = tmp_index + 8;
	}
	#if defined(IPPPOE_SUPPORT)
	else if (tmp_index < 0x18)//ippppoe
	{
		tmp_index &= 0x07;
		if(!file_to_buf(WAN_IPPPOE_IPADDR, readbuf, sizeof(readbuf)))			
		{				
			printf("no buf in %s!\n", WAN_IPPPOE_IPADDR);	       		 	
			return 0;			
		}
		if(!file_to_buf(WAN_IPPPOE_GATEWAY, readbuf1, sizeof(readbuf1)))			
		{				
			printf("no buf in %s!\n", WAN_IPPPOE_GATEWAY);	       		 	
			return 0;			
		}
		if (have_ppp_flag) ppp_index = tmp_index + 8;
	}
	#endif
#endif
	else return 0;

	sscanf(readbuf, "%s %s %s %s %s %s %s %s", ip_addrs[0], ip_addrs[1], ip_addrs[2], ip_addrs[3], ip_addrs[4], ip_addrs[5], ip_addrs[6], ip_addrs[7]);  		
	if (strcmp(ip_addrs[tmp_index], "0.0.0.0") == 0) return 0;
	else 
	{
		if (have_ppp_flag)
		{
			if(!(check_ppp_link(ppp_index) && check_ppp_proc(ppp_index))) return 0;
		}
		cur_wanip->ipaddr = inet_addr(ip_addrs[tmp_index]);
		sscanf(readbuf1, "%s %s %s %s %s %s %s %s", ip_addrs[0], ip_addrs[1], ip_addrs[2], ip_addrs[3], ip_addrs[4], ip_addrs[5], ip_addrs[6], ip_addrs[7]);  		
		cur_wanip->gwaddr = inet_addr(ip_addrs[tmp_index]);
	}
	return 1;
}

int read_mac_index(char *line, void *arg)
{
	char  var[63];
	char *next, *mac, *index, *tmp_lease;
	char tmp[12];
	
	if (strlen(line) > 20)
	{
		foreach(var, line, next)
		{
			mac = index = var;
			strsep(&index, str_sep);
			ether_atoe(mac, tmp);
			tmp_lease = index;
			strsep(&tmp_lease, str_sep);
			if (strlen(mac) >= 12)
			{
				memcpy(&share_wan_ip[share_wanip_count].macaddr[0], tmp, MAC_LEN);
				share_wan_ip[share_wanip_count].index = atoi(index);
				share_wan_ip[share_wanip_count].ipaddr = 0;
				share_wan_ip[share_wanip_count].lease = atoi(tmp_lease) * 60;
				if (share_wan_ip[share_wanip_count].lease == 0)
					share_wan_ip[share_wanip_count].lease = 86400;
				get_wanip_by_index(&share_wan_ip[share_wanip_count]);
				share_wanip_count++;
			}
		}
	}
	server_config.share_wanip_flag = 1;
	return 1;
}
void delete_route_by(u_int32_t dst_ip)
{
#if 1
	struct in_addr tmp_addrs;
	char tmp_buff[32];

	if (dst_ip == 0) return;
	tmp_addrs.s_addr = dst_ip;
	sprintf(tmp_buff, "%s", inet_ntoa(tmp_addrs));
	cprintf(" %s\n", tmp_buff);
	eval("route", "del", tmp_buff, "dev", "br0");
//	system(tmp_buff);
	
#endif
}

u_int32_t check_is_share_wanip_mac(u_int8_t *hwaddr)
{
	int i;
	if (server_config.share_wanip_flag)
	{
		for (i = 0; i < share_wanip_count; i ++)
		{
			if (!memcmp(share_wan_ip[i].macaddr, hwaddr, MAC_LEN))
				return share_wan_ip[i].lease;
		}
	}
	return 0;
}

u_int32_t get_share_wanip_gw(u_int8_t *hwaddr)
{
	int i;
	if (server_config.share_wanip_flag)
	{
		if (server_config.single_pc_mode)
			return single_pc_mode_gw;
		for (i = 0; i < share_wanip_count; i ++)
		{
			if (!memcmp(share_wan_ip[i].macaddr, hwaddr, MAC_LEN))
				return share_wan_ip[i].gwaddr;
		}
	}
	return 0;
}
u_int32_t get_cur_share_wanip(u_int8_t *hwaddr)
{
	int i;
	if (server_config.share_wanip_flag)
	{
		if (hwaddr == NULL)
		{
			return share_wan_ip[0].ipaddr;
		}
		for (i = 0; i < share_wanip_count; i ++)
		{
			if (!memcmp(share_wan_ip[i].macaddr, hwaddr, MAC_LEN))
				return share_wan_ip[i].ipaddr;
		}
	}
	return 0;
}

void clear_sharewanip_mac(void)
{
	share_wanip_count = 0;
	memset(&share_wan_ip[0].macaddr[0], 0, MAC_LEN);
}

//find the ip by mac, if a wan ip return 1, if a lan ip = 0, not found return -1
int find_share_wan_ip(u_int8_t *hwaddr, u_int32_t *dst_ip)
{
	int i = 0, result = -1;
	u_int32_t old_share_ip = 0;
	int tmp_index = -1;

	if (server_config.single_pc_mode)
	{
		if (memcmp(share_wan_ip[0].macaddr, hwaddr, MAC_LEN))
		{
			if (share_wanip_count) tmp_index = 1;
			else tmp_index = 0;
			memcpy(&share_wan_ip[tmp_index].macaddr[0], hwaddr, MAC_LEN);
			share_wan_ip[tmp_index].index = 255;
			share_wan_ip[tmp_index].ipaddr = 0;
			share_wanip_count += 1;
			single_pc_mode_gw = 0;
		}
	}
	for (i = 0; i < share_wanip_count; i ++)
	{
		old_share_ip = share_wan_ip[i].ipaddr;
		get_wanip_by_index(&share_wan_ip[i]);
		if (!memcmp(share_wan_ip[i].macaddr, hwaddr, MAC_LEN))
		{
			*dst_ip = share_wan_ip[i].ipaddr;
//			cprintf("find , dst ip = %ld\n", *dst_ip);
			if ((old_share_ip != *dst_ip) && (old_share_ip))
				delete_route_by(old_share_ip);

			if (*dst_ip > 0) 	result = 1;
			else result = 0;
		}
	}
	if (server_config.single_pc_mode)
	{
		if (tmp_index >= 0) 
		{
			single_pc_mode_gw = share_wan_ip[tmp_index].gwaddr;
			memset(&share_wan_ip[tmp_index].macaddr[0], 0, MAC_LEN);
			share_wanip_count -= 1;
			if (result == 0) result = -1;
		}
	}
	return result;
}

unsigned long str2hex(char *str)
{	
	unsigned long hexval = 0;	
	for(; *str; str ++)
	{		
		if(*str >= '0' && *str <= '9')
		{			
			hexval = hexval << 4 | (*str - '0');		
		}		
		if(*str >= 'a' && *str <= 'f')
		{			
			hexval = hexval << 4 | (*str - 'a' + 10);		
		}		
		if(*str >= 'A' && *str <= 'F')
		{			
			hexval = hexval << 4 | (*str - 'A' + 10);		
		}	
	}	
	return hexval;
}

void clear_share_wanip(void)
{
	int tmp_num = 0;
	FILE *fp;
	unsigned long tmp_wan_num = 0;
	char tmp_buf[32];

	if (server_config.share_wanip_flag)
	{
		if (!(fp = fopen("/proc/Cybertan/wan_ip_addr", "r"))) 
		{		
			cprintf("error to open /proc/Cybertan/wan_ip_addr");
		}
		else {		
			tmp_num = fread(tmp_buf, 1, 32, fp);		
			fclose(fp);		
			tmp_buf[tmp_num] = '\0';		
			tmp_wan_num = str2hex(tmp_buf);
			if (tmp_wan_num)
			{				
				char tmp_aa[32];					
				struct in_addr t_addr;
				
				t_addr.s_addr = htonl(tmp_wan_num);					
				strcpy(tmp_aa, inet_ntoa(t_addr));					
				eval("route", "del", tmp_aa, "dev", "br0");					
				if (!(fp = fopen("/proc/Cybertan/wan_ip_addr", "r"))) 
				{		
					cprintf("error to open /proc/Cybertan/wan_ip_addr");
				}else {
					fprintf(fp, "00000000");
					fclose(fp);
				}
			}
		}
	}
}

void SendInfoToKernal(u_int32_t ip_addr, u_int8_t *hwaddr)
{
	FILE *fp;
	char buff[128];
	struct in_addr addr;

	if (!(fp = fopen("/proc/Cybertan/lan_mac_addr", "w"))) {
		cprintf("error to open /proc/Cybertan/lan_mac_addr");
		return;
	}
	fprintf(fp, "%s", ether_etoa(hwaddr, buff));
	fclose(fp);

	if (!(fp = fopen("/proc/Cybertan/half_bridge_enable", "w"))) {
		cprintf("error to open /proc/Cybertan/half_bridge_enable");
		return;
	}
	fprintf(fp, "%d", 1);
	fclose(fp);
	
	if (!(fp = fopen("/proc/Cybertan/wan_ip_addr", "w"))) {
		cprintf("error to open /proc/Cybertan/wan_ip_addr");
		return;
	}
	fprintf(fp, "%08x", ntohl(ip_addr));
	fclose(fp);

	if (!(fp = fopen("/proc/Cybertan/lan_ip_addr", "w"))) 
	{
		cprintf("error to open /proc/Cybertan/lan_ip_addr");
		return;
	}
	fprintf(fp, "%08x", ntohl(inet_addr(nvram_get("lan_ipaddr"))));
	fclose(fp);
#if 1
	if (!(fp = fopen("/proc/Cybertan/local_lan_mac_addr", "w"))) {
		cprintf("error to open /proc/Cybertan/local_lan_mac_addr");
		return;
	}
	fprintf(fp, "%s", nvram_get("lan_hwaddr"));
	fclose(fp);

	if (!(fp = fopen("/proc/Cybertan/lan_netmask", "w"))) {
		cprintf("error to open /proc/Cybertan/lan_netmask");
		return;
	}
	fprintf(fp, "%08x", ntohl(inet_addr(nvram_get("lan_netmask"))));
	fclose(fp);
#endif
#if 1
	if ((server_config.share_wanip_comm == 1) && (server_config.share_wanip_flag == 1))
	{
		addr.s_addr = ip_addr;
	//	sprintf(buff, "route add %s dev br0", inet_ntoa(addr));
		sprintf(buff, "%s", inet_ntoa(addr));
		cprintf("$$$$$$$$$$$$wan ip = %s\n", buff);
		eval("route", "add", buff, "dev", "br0");
	}
#endif
#if 0
	//share wan ip , communicate to other lan device
	if ((server_config.share_wanip_comm == 0) && (server_config.share_wanip_flag == 1))
	{
		char tmp_buf[32], tmp_buf1[32];
		char *tmp_pt;
		sprintf(tmp_buf, "%s", nvram_get("lan_ipaddr"));
		strcpy(tmp_buf1, tmp_buf);
		tmp_pt = strrchr(tmp_buf1, '.');
		if (tmp_pt)
		{
			tmp_pt ++;
			strcpy(tmp_pt, "0/24");
		}
		eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-d", tmp_buf, "-j", "ACCEPT");
		eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-s", tmp_buf1, "-d", buff,  "-j", "DROP");
		eval("iptables", "-t", "nat", "-A", "PREROUTING", "-i", "br0", "-s", buff, "-d", tmp_buf1,  "-j", "DROP");
	}
#endif
}

//#define MACIP_DEBUG
#ifdef MACIP_DEBUG
int main(int argc, char **argv)
{
	char line[] = "00:01:02:0a:0f:02$192.168.1.1 00:01:02:0a:0f:03$192.168.1.2 00:01:02:0a:a:02$192.168.1.4";
	char aa[] =  "00:01:02:0a:0f:02";
	char var[23];
	int i;
	unsigned int ip;

	int macip_count=3;
	read_macip(line, var);
	for (i = 0; i < macip_count; i++)
	{
		printf("mac: %02x:%02x:%02x:%02x:%02x:%02x  ipaddr = %x\n", 
			mac_ip[i].macaddr[0], mac_ip[i].macaddr[1],mac_ip[i].macaddr[2],
			mac_ip[i].macaddr[3],mac_ip[i].macaddr[4],mac_ip[i].macaddr[5],
			mac_ip[i].ipaddr);
	}
	ip = find_ip_by_mac(aa);
	printf("ip = %x\n", ip);
	return 1;
}
#endif
