/* support for multi-pppoe via dns setting --- japan request */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <syslog.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <code_pattern.h>
#include <cy_conf.h>

#if defined(MPPPOE_SUPPORT) || defined(IPPPOE_SUPPORT)
extern char pdns[16], sdns[16];
extern char domainname[64];
#endif

#ifdef MPPPOE_SUPPORT
#define WAN_MPPPOE_IFNAME  "/tmp/status/wan_mpppoe_ifname"
#define WAN_MPPPOE_GATEWAY  "/tmp/status/wan_mpppoe_gateway"
#endif

#ifdef IPPPOE_SUPPORT
#define WAN_IPPPOE_IFNAME  "/tmp/status/wan_ipppoe_ifname"
#define WAN_IPPPOE_GATEWAY  "/tmp/status/wan_ipppoe_gateway"
#endif

#ifdef DEBUG_MPPPOE
#define printf(fmt, args...)	syslog(LOG_DEBUG, fmt, ##args);
#endif

//#define MPPPOE_DNAME	"hinet.net"
//#define MPPPOE_GW	"192.168.100.254"
//#define MPPPOE_IF	"eth1"
/*
   nvram need:  mpppoe_dname = domain name search string
		mpppoe_ifname= multi-pppoe interface
		mpppoe_gateway = multi-pppoe gateway
*/


struct rtrec {
        struct rtrec *prev,*next;
        unsigned char network[4];
        unsigned long ttl;
};

static struct rtrec *rtrec_head=NULL, *rtrec_tail=NULL;
static int rtrec_count=0;
void check_link();
void dump_link();
void remove_ttl();
struct rtrec *find_link();
void add_link();
void del_link();


void route_add_mpppoe ( char *name, struct in_addr *addr4, unsigned long now, unsigned long ttl) 
{
	int whichconn = atoi(nvram_safe_get("wan_active_connection"));
	int which = whichconn;
	char word[280], *next;
	char *dname;
	char encap[15];

	FILE *fp = fopen("/var/run/fucking", "a+");
	fprintf(fp, "enter route add mpppoe\n");
	
	if(which == 8)
		return;

	memset(encap, 0, sizeof(encap));
	check_cur_encap(which, encap);
	if(strcmp(encap, "pppoe") && strcmp(encap, "1483bridged"))
		return;
	
	/*
	memset(word, 0, sizeof(word));
	if(!strcmp(encap, "pppoe"))
	{
	#ifdef MPPPOE_SUPPORT
		foreach(word, nvram_safe_get("mpppoe_enable"), next)
		{
			if(which -- == 0)
				break;
		}
	#else 
		return;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#ifdef IPPPOE_SUPPORT
		foreach(word, nvram_safe_get("ipppoe_enable"), next)
		{
			if(which -- == 0)
				break;
		}
	#else 
		return;
	#endif
	}
	if(!strcmp(word, "0"))
		return;
	
	which = whichconn;
	memset(word, 0, sizeof(word));
	if(!strcmp(encap, "pppoe"))
	{
	#if defined(MPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("mpppoe_config"), next)
		{
			if(which -- == 0)
				break;
		}
	#else
		return;
	#endif
	}
	else if(!strcmp(encap, "1483bridged"))
	{
	#if defined(IPPPOE_SUPPORT)
		foreach(word, nvram_safe_get("ipppoe_config"), next)
		{
			if(which -- == 0)
				break;
		}
	#else
		return;
	#endif
	}

	dname = rindex(word, ':') + 1;
	*/
	//char *dname = nvram_safe_get("mpppoe_dname");
	fprintf(fp, "get name %s, ip=%s \n", name, inet_ntoa(*addr4));
	//printf("get name %s, ip=%s \n", name, inet_ntoa(*addr4));
	//if ( dname[0] && strstr(name,dname) ) {
	if ( domainname[0] && strstr(name,domainname) ) {
		unsigned char network[4];
		char readbuf[256], wan_gateway[18], wan_ifname[6];
		
		strncpy(network,(unsigned char *)addr4,4);
		network[3]=0;		// bulid a network address

		fprintf(fp, "get name %s, ip=%s \n", name, inet_ntoa(*addr4));
                //printf("get name %s, ip=%s \n", name, inet_ntoa(*addr4));

		memset(word, 0, sizeof(word));
		memset(wan_gateway, 0, sizeof(wan_gateway));
		memset(wan_ifname, 0, sizeof(wan_ifname));
	
		which = whichconn;
		
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			if(!file_to_buf(WAN_MPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_MPPPOE_GATEWAY);
				return;
			}
		#else
			return;
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			if(!file_to_buf(WAN_IPPPOE_GATEWAY, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_GATEWAY);
				return;
			}
		#else
			return;
		#endif
		}

		foreach(word, readbuf, next)
		{
			if(which -- == 0)
			{
				strncpy(wan_gateway, word, sizeof(wan_gateway));
				break;
			}
		}
		
		memset(word, 0, sizeof(word));
		which = whichconn;
		
		if(!strcmp(encap, "pppoe"))
		{
		#if defined(MPPPOE_SUPPORT)
			if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
				return;
			}
		#else
			return;
		#endif
		}
		else if(!strcmp(encap, "1483bridged"))
		{
		#if defined(IPPPOE_SUPPORT)
			if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
				return;
			}
		#else
			return;
		#endif
		}

		foreach(word, readbuf, next)
		{
			if(which -- == 0)
			{
				strncpy(wan_ifname, word, sizeof(wan_ifname));
				break;
			}
		}

		check_link(network, wan_gateway, wan_ifname, now,ttl);
                dump_link();
		
		fprintf(fp, "route add -net %s netmask 255.255.255.0 gw %s dev %s",
			inet_ntoa(*(struct in_addr *)network), wan_gateway,
			wan_ifname);
		/*printf("route add -net %s netmask 255.255.255.0 gw %s dev %s",
			inet_ntoa(*(struct in_addr *)network), wan_gateway,
			wan_ifname);*/
		
                eval("/usr/bin/route","add","-net",inet_ntoa(*(struct in_addr *)network),
			"netmask","255.255.255.0","gw", wan_gateway, "dev", wan_ifname);
		
		/*
		printf("route add -net %s netmask 255.255.255.0 gw %s dev %s",
			inet_ntoa(*(struct in_addr *)network),nvram_safe_get("wan_gateway_1"),
			nvram_safe_get("wan_ifname_1"));
                eval ("route","add","-net",inet_ntoa(*(struct in_addr *)network),
			"netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
			"dev",nvram_safe_get("wan_ifname_1") );
		*/
	}
	fclose(fp);
}
		
//void check_link(unsigned char *network, unsigned long now, unsigned long ttl)
void check_link(unsigned char *network,  char *wan_gateway, char *wan_ifname, unsigned long now, unsigned long ttl)
{
        struct rtrec *pt;


        if ( (pt=find_link(network)) == NULL) { //a new network, create it
                add_link(network,now+ttl);
 		printf("route add -net %s netmask 255.255.255.0 gw %s dev %s",
                        inet_ntoa(*(struct in_addr *)network), wan_gateway,
                        wan_ifname);
                eval ("/usr/bin/route","add","-net",inet_ntoa(*(struct in_addr *)network),
                        "netmask","255.255.255.0","gw", wan_gateway,
                        "dev", wan_ifname);

		/*
                printf("route add -net %s netmask 255.255.255.0 gw %s dev %s",
                        inet_ntoa(*(struct in_addr *)network),nvram_safe_get("wan_gateway_1"),
                        nvram_safe_get("wan_ifname_1"));
                eval ("route","add","-net",inet_ntoa(*(struct in_addr *)network),
                        "netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
                        "dev",nvram_safe_get("wan_ifname_1") );
		*/
        }
        else {  // a exist network, renew it
                del_link(pt);
                add_link(network,now+ttl > pt->ttl ? now+ttl : pt->ttl);
        }
        remove_ttl(wan_gateway, wan_ifname, now);
}



// remove link & route if ttl timeout

void remove_ttl(char *wan_gateway, char *wan_ifname, unsigned long now)
{
        struct rtrec *pt=rtrec_head,*tmp;

        while (pt) {
                if (pt->ttl < now) {
			printf("route del -net %s netmask 255.255.255.0 gw %s dev %s",inet_ntoa(*(struct in_addr *)pt->network), wan_gateway, wan_ifname);

                	eval ("/usr/bin/route","del","-net",inet_ntoa(*(struct in_addr *)pt->network),
                        	"netmask","255.255.255.0","gw", wan_gateway,
                        	"dev", wan_ifname);

			/*
               		printf("route del -net %s netmask 255.255.255.0 gw %s dev %s",
                        	inet_ntoa(*(struct in_addr *)pt->network),nvram_safe_get("wan_gateway_1"),
                        	nvram_safe_get("wan_ifname_1"));

                	eval ("route","del","-net",inet_ntoa(*(struct in_addr *)pt->network),
                        	"netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
                        	"dev",nvram_safe_get("wan_ifname_1") );
			*/
                        tmp=pt->next;
                        del_link(pt);
                        pt=tmp;
                } else
                        pt= pt->next;
        }
}


struct rtrec *find_link(unsigned char *network)
{
        struct rtrec *pt=rtrec_head;

        while (pt) {
                if ( !memcmp(pt->network,network,4)) {
                        return pt;
                } else
                        pt= pt->next;
        }
        return NULL;
}

void add_link(unsigned char *network, unsigned long timeout)
{

        struct rtrec *new;

        if ( (new=malloc(sizeof(struct rtrec))) == NULL ) {
                printf("malloc error when add_link!!! \n");
                return;
        }
        printf("add link... \n");
        strncpy(new->network,network,4);
        new->ttl=timeout;
        if (rtrec_head)
                rtrec_head->prev=new;
        new->next= rtrec_head;
        new->prev= NULL;
        rtrec_head= new;
        if (!rtrec_tail)
                rtrec_tail=new;

        rtrec_count++;
}

void del_link (struct rtrec *del)
{

        if (!del)
                return;

        if (del->prev)
                del->prev->next = del->next;
        else
                rtrec_head=del->next;

        if (del->next)
                del->next->prev = del->prev;
        else
                rtrec_tail=del->prev;

        free(del);

        rtrec_count--;
}

void dump_link ()
{

        struct rtrec *pt;
        printf("-----dump table-----\n");
        for (pt=rtrec_head; pt ; pt=pt->next) {
                printf("%s %lu %s\n",inet_ntoa(*(struct in_addr *)pt->network),
                                        pt->ttl,ctime(&(pt->ttl)) );
        }
        printf("=========%d=========\n", rtrec_count);
}



