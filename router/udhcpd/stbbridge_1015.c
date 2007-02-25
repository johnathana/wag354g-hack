#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <bcmnvram.h>
#include "cy_conf.h"

#include <shutils.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>


#include "debug.h"
#include "dhcpd.h"
#include "arpping.h"
#include "socket.h"
#include "options.h"
#include "files.h"
#include "leases.h"
#include "packet.h"
#include "serverpacket.h"
#include "pidfile.h"
#include "get_time.h"


#include "stbbridge.h"
#define  WAN_IPADDR "/tmp/status/wan_ipaddr"
extern struct stb_info stbinfo[MAXSTBDEV];
extern char stbmatchtypes[MAXSTBTYPE][256];
extern char wanipaddr[20];
void stbinfo_init()
{
	int i;
	for(i=0; i<MAXSTBDEV; i++)
	{
		memset(&stbinfo[i], 0, sizeof(struct stb_info));
	}
	return;
}

struct stb_info *get_new_stbinfo()
{
	int i;
	for(i=0; i<MAXSTBDEV; i++)
	{
		struct stb_info *oldstbinfo = &stbinfo[i];
		if(oldstbinfo->xid == 0)
			return oldstbinfo;
	}
	return &stbinfo[0];
}

struct stb_info *lookup_stbinfo(unsigned int id)
{
	int i;
	for(i=0; i<MAXSTBDEV; i++)
	{
		struct stb_info *oldstbinfo = &stbinfo[i];
		if(oldstbinfo->xid == id)
			return oldstbinfo;
	}
	return NULL;
}

void stbmatchtable_init()
{
	int enable = atoi(nvram_safe_get("stb_matchenable"));
	int i;
	char *matchtype = nvram_safe_get("stb_matchtype"), *tmpptr;

	memset(stbmatchtypes, 0, sizeof(stbmatchtypes));
	for(i=0; i<MAXSTBTYPE; i++)
	{
		tmpptr = strsep(&matchtype, "?");
		if(!(enable & (1<<i) || !tmpptr[0]))
			continue;
		strcpy(stbmatchtypes[i], tmpptr);
	}
/*	
	char *buf = nvram_safe_get("stb_matchtype");
	char *tmp = strsep(&buf, "#");
	int num = atoi(tmp);
	
	if(num)
	{
		int i;
		struct stb_value *tmpstb;
		stbmatchtable = malloc((num+1) * sizeof(struct stb_value));
		if(stbmatchtable == NULL)
			return;
		tmpstb = stbmatchtable;
		memset(stbmatchtable, 0, sizeof(stbmatchtable));
		for(i=0; i!=num; i++)
		{
			tmp = strsep(&buf, " ");
			strcpy(tmpstb->value, tmp);
			tmpstb += sizeof(struct stb_value);
		}	
	}
*/	
	return;
}

int relay_send(int fd,struct dhcpMessage *payload,u_int32_t dest_ip,int dest_port)
{
        int result;
        struct sockaddr_in to;
        bzero(&to, sizeof(to));
        to.sin_family = AF_INET;
        to.sin_port = htons(dest_port);
        to.sin_addr.s_addr = dest_ip;
        result = sendto(fd,payload,sizeof(struct dhcpMessage),0,(struct sockaddr *)&to,sizeof(to));
	return result;
	
}
void stb_enable_inkernel(int i)
{
        FILE *fp;

        if (!(fp = fopen("/proc/Cybertan/lan_ip_addr", "w"))) {
	                cprintf("error to open /proc/Cybertan/lan_ip_addr");
		                return;
			        }
        fprintf(fp, "%08x", inet_addr(nvram_get("lan_ipaddr")));
        fclose(fp);
        if (!(fp = fopen("/proc/Cybertan/lan_netmask", "w"))) {
	                cprintf("error to open /proc/Cybertan/lan_netmask");
		                return;
			        }
        fprintf(fp, "%08x", inet_addr(nvram_get("lan_netmask")));
        fclose(fp);
        if (!(fp = fopen("/proc/Cybertan/stb_enabled", "w"))) {
	                cprintf("error to open /proc/Cybertan/stb_enabled");
		                return;
			        }
       if(i==1)
	      fprintf(fp, "%d",1);
       else if(i==0)
	      fprintf(fp, "%d",0);
	       
        fclose(fp);

	
	return ;
}
int stbinfo2kernel(int flag,struct stb_info *message, u_int8_t *hdaddr)
{
     FILE *fp;
     int i;
     char buff[7],tmp;
     
     
     ether_etoa(hdaddr,buff);
     for(i=0;i<6;i++){
	   tmp= buff[i];
           buff[i]=buff[5-i];
	   buff[5-i]=tmp;
     } 
     cprintf("the buff=%s\n",buff);
     if (!(fp = fopen("/proc/Cybertan/stb_info", "w"))) {
        cprintf("error to open /proc/Cybertan/stb_stb_info");
        return 0;
        }
     fprintf(fp,"%0x %0x %0x %s",flag,message->ipaddr,message->netmsk, ether_etoa(hdaddr,buff));
     cprintf("%0x %0x %0x %s\n",flag,message->ipaddr,message->netmsk, ether_etoa(hdaddr,buff));
     fclose(fp);
        return 1;
}


int getifname(int index, char *ifname)
{
        char readbuf[60], *str = readbuf, *tmpstr;
        int i;
        if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
              return -1;
        for(i=0; i<MAX_CHAN; i++)
        {
          tmpstr = strsep(&str, " ");
          if(i == index)
                  break;
        }
        if(!strcmp(tmpstr, "Down"))
                  return -1;
        if(!strstr(tmpstr, "ppp") || check_ppp_link(index))
         {
              strcpy(ifname, tmpstr);
               return 0;
         }
        return -1;
	
//	return 1;
}
int stbnet2kernel(struct stb_info *message)
{
     FILE *fp;
     u_int32_t stb_network;
     cprintf("start\n");
    if (!(fp = fopen("/proc/Cybertan/stb_netmask", "w"))) {
         cprintf("error to open /proc/Cybertan/stb_netmask");
         return 0;
       }
     fprintf(fp, "%0x", message->netmsk);
     fclose(fp);
     cprintf("mild\n");
     stb_network = message->ipaddr&message->netmsk;// «≤ª”–Œ Ã‚Â£ø
     if (!(fp = fopen("/proc/Cybertan/stb_network", "w"))) {
           cprintf("error to open /proc/Cybertan/stb_network");
           return 0;
        }
     fprintf(fp, "%0x", stb_network);
     fclose(fp);
     cprintf("end\n");
     return 1;
}
void stbinfo_del(unsigned int id)
{
     int i;
     for(i=0; i<MAXSTBDEV; i++)
         {
           struct stb_info *oldstbinfo = &stbinfo[i];
           if(oldstbinfo->xid == id)
             {
                memset(&stbinfo[i], 0, sizeof(struct stb_info));
		cprintf("del  ok!!!!!\n");
                return;
             }
          }
      return ;
}
int _getwanip(int index)
{       int i;
        char content_buf[256],*tmp_content, *pt;
	//int  index;
//	index = atoi(nvram_safe_get("wan_active_connection"));
        if ((index < 0) || (index > 8)) index = 8;
	cprintf("the index =%d\n",index);
        if (index == 8) return 0;
        else {
	   if(!file_to_buf(WAN_IPADDR, content_buf, sizeof(content_buf)))
	     {
	       printf("no buf in %s!\n", WAN_IPADDR);
	       return 0;
	     }
	   tmp_content = content_buf;
	  cprintf("the content_buf =%s\n",content_buf);
	   for (i = 0; i <= index; i++)
             {
               pt = strchr(tmp_content, ' ');
               if (pt)
                {
                  if (i < index) tmp_content = pt + 1;
		  
                      *pt = '\0';
		      cprintf("tmp_conten=%s\n",tmp_content);
                 }else break;
             }
                if (!strcmp(tmp_content, "0.0.0.0")){ 
	            cprintf("the tmp_content it's NULL\n");return 0;}
        }

        strcpy(wanipaddr, tmp_content);
	cprintf("the wanipaddr=%s\n",wanipaddr);
	
	return 1;
}
