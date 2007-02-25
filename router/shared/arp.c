
/*
 *********************************************************
 *   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
 *********************************************************

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
 the contents of this file may not be disclosed to third parties,
 copied or duplicated in any form without the prior written
 permission of CyberTAN Inc.

 This software should be used as a reference only, and it not
 intended for production use!


 THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
 SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <string.h>
#include <ctype.h>

#include <broadcom.h>

#ifdef ARP_TABLE_SUPPORT

static int 
INET_resolve(char *name, struct sockaddr_in *sin, int hostfirst)
{
    sin->sin_family = AF_INET;
    sin->sin_port = 0;

       /* Look to see if it's a dotted quad. */
    if (inet_aton(name, &sin->sin_addr)) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): [%s] is ip",__FUNCTION__,name);
#endif
        return 0;
    }

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): [%s] isn't ip",__FUNCTION__,name);
#endif
    /* If we expect this to be a hostname, try hostname database first */

    return -1;
}

static int 
INET_input(int type, char *bufp, struct sockaddr *sap)
{
    return (INET_resolve(bufp, (struct sockaddr_in *) sap, 0));
}

/* Input an Ethernet address and convert to binary. */
static int
in_ether(char *bufp, struct sockaddr *sap)
{
    unsigned char *ptr;
    char c, *orig;
    int i;
    unsigned val;

    sap->sa_family = ARPHRD_ETHER;
    ptr = sap->sa_data;

    i = 0;
    orig = bufp;
    while ((*bufp != '\0') && (i < ETH_ALEN)) {
        val = 0;
        c = *bufp++;
        if (isdigit(c))
            val = c - '0';
        else if (c >= 'a' && c <= 'f')
            val = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            val = c - 'A' + 10;
        else {
#ifdef MY_DEBUG
            LOG(LOG_DEBUG, "in_ether(%s): invalid ether address!", orig);
#endif
            errno = EINVAL;
            return (-1);
        }
        val <<= 4;
        c = *bufp;
        if (isdigit(c))
            val |= c - '0';
        else if (c >= 'a' && c <= 'f')
            val |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            val |= c - 'A' + 10;
        else if (c == ':' || c == 0)
            val >>= 4;
        else {
#ifdef MY_DEBUG
            LOG(LOG_DEBUG, "in_ether(%s): invalid ether address!", orig);
#endif
            errno = EINVAL;
            return (-1);
        }
        if (c != 0)
            bufp++;
        *ptr++ = (unsigned char) (val & 0377);
        i++;

        /* We might get a semicolon here - not required. */
        if (*bufp == ':') {
            if (i == ETH_ALEN) {
#ifdef MY_DEBUG
                LOG(LOG_DEBUG, "in_ether(%s): trailing : ignored!", orig)
#endif
                    ;           /* nothing */
            }
            bufp++;
        }
    }

    /* That's it.  Any trailing junk? */
    if ((i == ETH_ALEN) && (*bufp != '\0')) {
#ifdef MY_DEBUG
        LOG(LOG_DEBUG, "in_ether(%s): trailing junk!", orig);
        errno = EINVAL;
        return (-1);
#endif
    }
	return 0;
}

/* Dump arp in <tr><td>MAC</td><td>IP</td><td>interface</td></tr> format */
int
ej_dump_arp_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	int count = 0;
	char *format;
	FILE *fp;
	char ip[50];		// ip address
    	char hwa[50];		// HW address / MAC
    	char mask[50];		// ntemask  
    	char line[80];
    	char dev[50];		// interface
    	int type;		// HW type
	int flags;		// flags

	if (ejArgs(argc, argv, "%s", &format) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	/* open arp table*/
	if ((fp = fopen("/proc/net/arp", "r")) == NULL) {
       		websError(wp, 400, "No arp table\n");
        	return -1;
        }

        /* Bypass header -- read until newline */
        if (fgets(line, sizeof(line), fp) != (char *) NULL) {
           /* Read the ARP cache entries. */
	   // IP address       HW type     Flags       HW address            Mask     Device
           // 192.168.1.1      0x1         0x2         00:90:4C:21:00:2A     *        eth0

           for (; fgets(line, sizeof(line), fp);) {
               if(sscanf(line, "%s 0x%x 0x%x %100s %100s %100s\n", ip, &type, &flags, hwa, mask, dev)!=6)
                   break;

#ifdef MY_DEBUG
               LOG(LOG_DEBUG,"%s(): ip[%s] type[%x] flags[%x] hwa[%s] mask[%s] dev[%s]\n",__FUNCTION__,ip,type,flags,hwa,mask,dev);
#endif
	       if (!(flags & ATF_COM)) {       //ATF_COM = 0x02   completed entry (ha valid)
                     if (flags & ATF_PUBL)     //ATF_PUBL = 0x08  publish entry
                          strcpy(hwa,"*"); 
                     else{
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): ip[%s] incomplete",__FUNCTION__,ip);
#endif
		 	  continue;
		     }
               }

	        if(count == 0)   // first time show Delete button
                     ret += websWrite(wp, "<th><font face=verdana size=2><input type=button name=action value='Delete' onClick=ARPAct(this.form,'del')></th></tr>\n");

 	       ret += websWrite(wp,"<tr align=middle bgColor=#cccccc><td>%s</td><td>%s</td><td>%s</td>",hwa,ip,((flags & ATF_PERM) ? "static" : "dynamic"));   /* ATF_PERM = 0x04  permanent entry              */
               ret += websWrite(wp,"<td><input type=checkbox name=d_arp value=%s></td></tr>",ip);

		count++;
	   }
        }


	if(count == 0)      //if not arp table, show None
             websWrite(wp,"</tr><tr align=middle bgColor=#cccccc><td>None</td><td>None</td><td>None</td></tr>");


	return ret;
}

/* Delete an entry from the ARP cache. */
int 
arp_del(char *host)
{
    struct arpreq req;
    struct sockaddr sa;
    int sockfd = 0;                 
    int err;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): socket error",__FUNCTION__);
#endif
        exit(-1);
    }

    memset((char *) &req, 0, sizeof(req));

    if (INET_input(0, host, &sa) < 0) {
#ifdef MY_DEBUG
	//LOG(LOG_DEBUG,"[%s]",herror(host));
#endif
	return (-1);
    }
    

    // If a host has more than one address, use the correct one! 
    memcpy((char *) &req.arp_pa, (char *) &sa, sizeof(struct sockaddr));

    req.arp_flags = ATF_PERM;

    //strcpy(req.arp_dev, device);

    // Call the kernel. 
    if ((err = ioctl(sockfd, SIOCDARP, &req) < 0)) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): delete [%s] fail",__FUNCTION__,host);
#endif
	    return (-1);
   }
   else{
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): delete [%s] success",__FUNCTION__,host);
#endif
   }

   return (0);
}

/* Delete leases */
int
arp_add(char *host,char *hwaddr)
{
    struct arpreq req;
    struct sockaddr sa;
    int sockfd = 0;                 

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): socket error",__FUNCTION__);
#endif
        exit(-1);
    }

    memset((char *) &req, 0, sizeof(req));

    if (INET_input(0, host, &sa) < 0) {
#ifdef MY_DEBUG
	//LOG(LOG_DEBUG,"[%s]",herror(host));
#endif
	return (-1);
    }

    // If a host has more than one address, use the correct one! 
    memcpy((char *) &req.arp_pa, (char *) &sa, sizeof(struct sockaddr));

    if (in_ether(hwaddr,&req.arp_ha) < 0) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): illegal MAC Address :[%s]",__FUNCTION__,hwaddr);
#endif
	return (-1);
    }
    

    req.arp_flags = ATF_PERM | ATF_COM;

    //strcpy(req.arp_dev, device);

    // Call the kernel. 
    if (ioctl(sockfd, SIOCSARP, &req) < 0) {
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): add [%s] fail",__FUNCTION__,host);
#endif
	    return (-1);
   }
   else{
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): add [%s] success",__FUNCTION__,host);
#endif
   }

   return (0);


}

/* Delete leases */
int
delete_arp_table(webs_t wp)
{
	int i;
	struct in_addr in;
	struct params p_arp;

	get_params("d_arp",&p_arp); 

	if(!p_arp.num) return 0;

	for(i = 0 ; i < p_arp.num ; i ++){
#ifdef MY_DEBUG 
	LOG(LOG_DEBUG,"%s(): delete [%lu] ",__FUNCTION__,p_arp.value[i]);	
#endif
	   in.s_addr=p_arp.value[i];
	   arp_del(inet_ntoa(in));
	}


	return 0;
}
/* Delete leases */
int
add_arp_table(webs_t wp)
{
        char *arp_ip = websGetVar(wp, "arp_ip", NULL);
        char *arp_mac = websGetVar(wp, "arp_mac", NULL);

	strip_space(arp_ip);
	strip_space(arp_mac);

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): add ip[%s] mac[%s]",__FUNCTION__,arp_ip,arp_mac);	
#endif

	if(!arp_ip || !arp_mac)
		return -1;

	if(!strcmp(arp_ip,"") || !strcmp(arp_mac,""))
		return -1;

        if (!legal_ipaddr(arp_ip)){
		return -1;
        }
	
        if (!legal_hwaddr(arp_mac)){
		return -1;
        }

	arp_add(arp_ip,arp_mac);

	return 0;
}

#endif
