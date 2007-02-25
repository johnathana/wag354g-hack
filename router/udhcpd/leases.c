/* 
 * leases.c -- tools to manage DHCP leases 
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 */

#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcmnvram.h>

#include "debug.h"
#include "dhcpd.h"
#include "files.h"
#include "options.h"
#include "leases.h"
#include "arpping.h"
#include "get_time.h"

#include <shutils.h>

/* da_f 2005.6.21 add */
#define cprintf(str, arg...)

unsigned char blank_chaddr[] = {[0 ... 15] = 0};

/* clear every lease out that chaddr OR yiaddr matches and is nonzero */
void clear_lease(u_int8_t *chaddr, u_int32_t yiaddr)
{
	unsigned int i, j;
	
	for (j = 0; j < 16 && !chaddr[j]; j++);
	
	for (i = 0; i < server_config.max_leases; i++)
		if ((j != 16 && !memcmp(leases[i].chaddr, chaddr, 16)) ||
		    (yiaddr && leases[i].yiaddr == yiaddr)) {
			memset(&(leases[i]), 0, sizeof(struct dhcpOfferedAddr));
		}
}


/* add a lease into the table, clearing out any old ones */
struct dhcpOfferedAddr *add_lease(u_int8_t *chaddr, u_int32_t yiaddr, unsigned long lease)
{
	struct dhcpOfferedAddr *oldest;
	
	/* clean out any old ones */
	clear_lease(chaddr, yiaddr);
		
	oldest = oldest_expired_lease();
	
	if (oldest) {
		memcpy(oldest->chaddr, chaddr, 16);
		oldest->yiaddr = yiaddr;
		oldest->expires = get_time(0) + lease;
	}
	
	return oldest;
}


/* true if a lease has expired */
int lease_expired(struct dhcpOfferedAddr *lease)
{
	return (lease->expires < (unsigned long) get_time(0));
}	


/* Find the oldest expired lease, NULL if there are no expired leases */
struct dhcpOfferedAddr *oldest_expired_lease(void)
{
	struct dhcpOfferedAddr *oldest = NULL;
	unsigned long oldest_lease = get_time(0);
	unsigned int i;

	
	for (i = 0; i < server_config.max_leases; i++)
		if (oldest_lease > leases[i].expires) {
			oldest_lease = leases[i].expires;
			oldest = &(leases[i]);
		}
	return oldest;
		
}


/* Find the first lease that matches chaddr, NULL if no match */
struct dhcpOfferedAddr *find_lease_by_chaddr(u_int8_t *chaddr)
{
	unsigned int i;

	for (i = 0; i < server_config.max_leases; i++)
		if (!memcmp(leases[i].chaddr, chaddr, 16)) return &(leases[i]);
	
	return NULL;
}


/* Find the first lease that matches yiaddr, NULL is no match */
struct dhcpOfferedAddr *find_lease_by_yiaddr(u_int32_t yiaddr)
{
	unsigned int i;

	for (i = 0; i < server_config.max_leases; i++)
		if (leases[i].yiaddr == yiaddr) {

#ifdef MY_DEBUG		// by honor
	struct in_addr in;
	in.s_addr = yiaddr;
	LOG(LOG_DEBUG,"The ip [%s] is in lease table", inet_ntoa(in));
#endif
			return &(leases[i]);
		}
	
	return NULL;
}
//guohong
int is_in_machost_table(u_int32_t ip,int *index)
{
    int i,j;


    if (ip == 0)  return 0;
    
    for (i =0; i<MAX_NUM;i++)
    	{
            if( mac_ip[i].ipaddr == ip)   
            	{
            	       * index = i;
                       return 2;
            	}
    	}
    for (i =0; i<MAX_NUM;i++)
    	{
            if( hostname_ip[i].ipaddr == ip)   
            	{
            	       * index = i;
                       return 1;
            	}
    	}
    *index = 0;
    return 0;
}

static int count ;
/*******************************************************/
/*add by fg ,get the "dhcp_reserved_ipaddr" from Flash and translate to a comma 
//format IP list */
int GetResAdd(char *reserved_ip)
{

  char newReserv[512], pre[512],ptr[512],ipaddr[20],*tmp,*tmp1,out[512],*tmp_begin,*tmp_end;
        strcpy(newReserv, nvram_safe_get("dhcp_reserved_ipaddr"));
        strcpy(pre ,newReserv);
        tmp1=pre;
        tmp="0.0.0.0";
        if(tmp1==strcasestr(tmp1,"0::")){strcpy(reserved_ip,tmp);return 0;}
          else if(tmp=strcasestr(tmp1," 0::"))
                {
                tmp=tmp-2;*tmp='\0';
                }
        else {tmp=tmp1;tmp=tmp+(strlen(tmp1)-3);*tmp='\0';}
        strcpy(pre,tmp1);
        strcpy(ipaddr, nvram_safe_get("lan_ipaddr"));
        if(tmp= strrchr(ipaddr, '.'))
              {tmp++;  *tmp = '\0';}
        tmp1=ipaddr;
        strcpy(ptr,tmp1);
        sprintf(ipaddr,",%s",ptr);
        strcat(ptr,pre);
        strcpy(newReserv,ptr);
        while(tmp_begin=strstr(ptr,":: "))
        {  
        tmp_end = tmp_begin + strlen(":: ");
        *tmp_begin = '\0';
        sprintf(newReserv, "%s%s%s", ptr, ipaddr, tmp_end);
        strcpy(ptr,newReserv);
        } 
	strcpy(reserved_ip,newReserv);
	return 0;
}
 /*******************************************************/


/* find an assignable address, it check_expired is true, we check all the expired leases as well.
 * Maybe this should try expired leases by age... */
u_int32_t find_address(int check_expired) 
{

	u_int32_t addr, ret = 0;
        struct dhcpOfferedAddr *lease = NULL;
        int i;
		/* da_f 2005.6.21 modify. Do not need any more
        char reserved_ip[512],tmp[512];*/
        char find_addr[20];
        struct in_addr address;

	addr = ntohl(server_config.start); /* addr is in host order here */
	for (;addr <= ntohl(server_config.end); addr++) {

		/* Router IP , by honor*/
		if (ret == server_config.server) continue;
		
		/* ie, 192.168.55.0 */
		if (!(addr & 0xFF)) continue;
		/*add by fg for  "dhcp_reversed_ipaddr"*/
		/* da_f 2005.6.21 modify. Do not need any more.
		GetResAdd(reserved_ip);					  		 	
	   	address.s_addr =htonl(addr);
	   	strcpy(find_addr,inet_ntoa(address)); 
    		if(strstr(reserved_ip, find_addr))continue;  */
		/*add by fg for  "dhcp_reversed_ipaddr"*/

		/* ie, 192.168.55.255 */
		if ((addr & 0xFF) == 0xFF) continue;
                  
                if (is_in_machost_table(htonl(addr),&i))  
                {
                	if (i==1)
               	             cprintf("find a ip in host table!\n");
               	        if  ( i== 2)
               	        cprintf("find a ip in mac table!\n");
               	     continue;//skip the ip addresses in mac-ip or hostname-ip table
               	} 
		/* lease is not taken */
		ret = htonl(addr);
		if ((!(lease = find_lease_by_yiaddr(ret)) ||

		     /* or it expired and we are checking for expired leases */
		     (check_expired  && lease_expired(lease))) &&

		     /* and it isn't on the network */
	    	     !check_ip(ret)) {
			return ret;
			break;
		}
	}
	return 0;
}


/* check is an IP is taken, if it is, add it to the lease table */
int check_ip(u_int32_t addr)
{
	struct in_addr temp;
	
#ifdef MY_DEBUG		// by honor
	temp.s_addr = addr;
	LOG(LOG_DEBUG,"check_ip [%s]", inet_ntoa(temp));
#endif
	
	if (arpping(addr, server_config.server, server_config.arp, server_config.interface) == 0) {
		temp.s_addr = addr;
	 	LOG(LOG_INFO, "%s belongs to someone, reserving it for %ld seconds", 
	 		inet_ntoa(temp), server_config.conflict_time);
		add_lease(blank_chaddr, addr, server_config.conflict_time);
		return 1;
	} else return 0;
}

