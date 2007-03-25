
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
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <utils.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <cy_conf.h>
#include <shutils.h>
#ifdef HSIAB_SUPPORT
#include <hsiabutils.h>
#endif

extern int errno;
#define WAN_IFNAME "/tmp/status/wan_ifname"
#define WAN_ENCAP "/tmp/status/wan_encap"
#define MAX_CHAN 8

//junzhao 2004.3.22
#define TICFG_BUF 10*1024
#define TICFG_ENV "/proc/ticfg/env"
#ifdef STB_SUPPORT
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
}
#endif


int string_rep(char *buff, int index, char *word)
{
	char *ptrstart = buff;
	char *ptrend;
	int i;
	for(i=0; i<index-1; i++)
	{
		ptrstart = strstr(ptrstart, ":");
		if(ptrstart != NULL)
			ptrstart++;
		else
			return -1;
	}
	ptrend = strstr(ptrstart, ":");
	if(ptrend == NULL)
	{
		memcpy(ptrstart, word, strlen(word));
		ptrstart += strlen(word) + 1;
		*ptrstart = '\0';
	}
	else
	{
		char *buf = strdup(ptrend);
		memcpy(ptrstart, word, strlen(word));
		ptrstart += strlen(word);
		memcpy(ptrstart, buf, strlen(buf));
		ptrstart += strlen(buf);
		*ptrstart = '\0';
		free(buf);
		buf = NULL;
	}
	return 0;
}

int string_sep(char *word, int index, char *buf)
{
	char *tmpstart = word;
	char *tmpend = NULL;
	int i;
	for(i=1; i<index; i++)
	{
		tmpstart = strstr(tmpstart, ":");
		if(tmpstart != NULL)
			tmpstart++;
		else
			return -1;
	}	
	tmpend = strstr(tmpstart, ":");
	if(tmpend != NULL)
	{
		strncpy(buf, tmpstart, tmpend-tmpstart);
		buf[tmpend-tmpstart] = '\0';
	}	
	else	
		strcpy(buf, tmpstart);

	return 0;
}
#if 0
int check_cur_encap(int which, char *encap)
{
	char word[60], *next;
	int i;
	char *tmpstart = word;
	char *tmpend = NULL;
	
	memset(word, 0, sizeof(word));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(which -- == 0)
			break;
	}

	for(i=0; i<2; i++)
	{
		tmpstart = strstr(tmpstart, ":");
		if(tmpstart != NULL)
			tmpstart++;
		else
			return -1;
	}	
	tmpend = strstr(tmpstart, ":");
	if(tmpend != NULL)
	{
		strncpy(encap, tmpstart, tmpend-tmpstart);
		encap[tmpend-tmpstart] = '\0';
	}	
	else	
		strcpy(encap, tmpstart);

	return 0;
}	
#endif

#if 1
int check_cur_encap(int which, char *encap)
{
	//char word[60], *next;
	int i;
	char *tmpstart, *ptr = nvram_safe_get("vcc_config"), *tmpptr = NULL;
	char *tmpend = NULL;
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == which)
			break;
	}
	//printf("encapmode %s.......\n", tmpptr);
	
	/*
	memset(word, 0, sizeof(word));
	foreach(word, nvram_safe_get("vcc_config"), next)
	{
		if(which -- == 0)
			break;
	}
	*/
	tmpstart = tmpptr;
	for(i=0; i<2; i++)
	{
		tmpstart = strstr(tmpstart, ":");
		if(tmpstart != NULL)
			tmpstart++;
		else
			return -1;
	}	
	tmpend = strstr(tmpstart, ":");
	if(tmpend != NULL)
	{
		strncpy(encap, tmpstart, tmpend-tmpstart);
		encap[tmpend-tmpstart] = '\0';
	}	
	else	
		strcpy(encap, tmpstart);

	return 0;
}	
#endif

int check_encap(int which, char *encap)
{
	char readbuf[80];
	int i;
	char *ptr = readbuf, *tmpptr = NULL;
	
	file_to_buf(WAN_ENCAP, readbuf, sizeof(readbuf));
	
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		tmpptr = strsep(&ptr, " ");
		if(i == which)
			break;
	}
	
	strcpy(encap, tmpptr);

	return 0;
}	


int check_adsl_status()
{
	FILE *fp = NULL;
	char buf[16];
	int ret = 0;
	
	fp = fopen("/proc/avalanche/avsar_modem_training", "r");
	if(fp == NULL)
	{
		printf("no avsar_status file!!\n");
		return ret;
	}
	memset(buf, 0, sizeof(buf));
	if(fread(buf, sizeof(char), sizeof(buf)*sizeof(char), fp) < 0)
	{
		printf("the file has no status\n");
		fclose(fp);
		return ret;
	} 
	//printf("read buf ---- %s\n", buf);
	if(!strncmp(buf, "SHOWTIME", strlen("SHOWTIME"))){
		ret = 1;
                //syslog(LOG_INFO,"1bADSL:%s.",buf);
		
	}
	//else
        //        syslog(LOG_INFO,"1rADSL:%s.",buf);
	
	
	fclose(fp);
	return ret;
}

//junzhao 2004.4.16
int hwaddr_convert(char *hwaddr)
{
	char *tmp = hwaddr;
	int i;
	for(i=0; i<5; i++)
	{
		tmp = strstr(tmp, ":");
		if(tmp==NULL)
			break;
		*tmp = '-';
		tmp++;
	}
	return 0;
}

int macaddr_get(char *mac_addr)
{
	int fd;
	int count = 0;
	unsigned char ticfg_buf[TICFG_BUF];
	char *str, *location;
	
	memset(ticfg_buf, 0, sizeof(ticfg_buf));
	fd = open(TICFG_ENV, O_RDONLY);
	if(fd < 0)
	{
		printf("can not open the ticfg/env file!\n");
		return -1;
	}
	count = read(fd, ticfg_buf, TICFG_BUF);
	//printf("count = %d\n", count);
	if(count > 0)
	{
		int offset = 0;
		str = ticfg_buf;
		location = strstr(str, "maca");
		
		offset = (unsigned long)location - (unsigned long)ticfg_buf;
		
		offset += strlen("maca") + 1;
		//printf("%08x-%08x-%08x\n", ticfg_buf, location, offset);
		//for(i = 0; i < 18; i ++)
		//	printf("%02x ", ticfg_buf[offset + i]);
		strncpy((char *)mac_addr, (const char *)(ticfg_buf + offset), 18);
		mac_addr[17] = '\0';
		printf("\nmac_addr:%s\n", mac_addr);
	}
	close(fd);
	return 0;
}

unsigned int sar_getstats(char *stat_str)
{
    	FILE *fd;
    	unsigned int var = 0;
    	int retval;
    	char *pbuff;
    	char *pos;
    
    	pbuff=(char *) malloc(2*1024);
    	if(!pbuff)
    	{
        	printf("out of memory\n");
        	return -1;
    	}

    	fd=fopen("/proc/avalanche/avsar_modem_stats", "r");
    	if(!fd)
	{
    		free(pbuff);
      		return -1;
	}
	
    	fread(pbuff, sizeof(char), 2*1024*sizeof(char), fd);
    	fclose(fd);

    	pos=strstr(pbuff, stat_str);
	
	//junzhao add 2004.10.25 for not found....
	if(!pos)
	{
    		free(pbuff);
		return var;
	}
	
    	pos += strlen(stat_str);

    	if(pos)
        	retval = sscanf(pos," %u", &var);
    
    	free(pbuff);
    	return var;
}

//path 0:FAST 1:INTERLEAVE 
//direction 0:tx 1:rx
int sar_mul_getstats(char *stat_str, int path, int direction) 
{
    	FILE *fd;
    	int var=0;
    	int retval;
    	char *pbuff;
    	char *pos;
	char flagstr[40];
    	memset(flagstr, 0, sizeof(flagstr));
	
    	pbuff=(char *) malloc(2*1024);
    	if(!pbuff)
    	{
        	printf("out of memory\n");
        	return -1;
    	}

    	fd=fopen("/proc/avalanche/avsar_modem_stats", "r");
    	if(!fd)
	{
    		free(pbuff);
      		return -1;
	}
	
    	fread(pbuff, sizeof(char), 2*1024*sizeof(char), fd);
    	fclose(fd);

	if(path == 0 && direction == 0)
		sprintf(flagstr, "%s", "Upstream (TX) Fast path");
	else if(path == 1 && direction == 0)
		sprintf(flagstr, "%s", "Upstream (TX) Interleave path");
	else if(path == 0 && direction == 1)
		sprintf(flagstr, "%s", "Downstream (RX) Fast path");
	else if(path == 1 && direction == 1)
		sprintf(flagstr, "%s", "Downstream (RX) Interleave path");
	
    	pos=strstr(pbuff, flagstr);

	if(!pos)
	{
    		free(pbuff);
		return var;
	}
	
    	pos += strlen(flagstr);
	
    	pos=strstr(pos, stat_str);
    	pos += strlen(stat_str);

    	if(pos)
        	retval = sscanf(pos," %d", &var);
    
    	free(pbuff);
    	return var;
}

int sar_get_ver(char *whatversion, char *result)
{
	char tmp[32];
	FILE *fp = fopen("/proc/avalanche/avsar_ver", "r");
    	if(!fp)
      		return -1;
	while(!feof(fp))
	{
		memset(tmp, 0, sizeof(tmp));
		if(fgets(tmp, sizeof(tmp), fp))
		{
			if(strstr(tmp, whatversion))
			{
				char *p = strstr(tmp, "[");
				if(p++)
				{
					char *q = strstr(p, "]");
					strncpy(result, p, q-p);
					result[q-p] = '\0';
					fclose(fp);
					return 0;
				}
			}
		}
	}
	fclose(fp);
	return -1;
}	

int sar_get_annex(char *result)
{
	char tmp[16];
	FILE *fp = fopen("/proc/avalanche/avsar_ver", "r");
    	if(!fp)
      		return -1;
	while(!feof(fp))
	{
		memset(tmp, 0, sizeof(tmp));
		if(fgets(tmp, sizeof(tmp), fp))
		{
			char *p;
			if(p = strstr(tmp, "Annex"))
			{
				strncpy(result, p, strlen(p) - 1);
				result[strlen(p) - 1] = '\0';
				fclose(fp);
				return 0;
			}
		}
	}
	fclose(fp);
	return -1;
}	

//junzhao 2004.8.2 add for lookup dnsname in flash conf and save parsed ip 
void dns_parsedip_save(char *dnsname, char *ipaddr)
{
	char conf_word[140];
	char conf_name[20], enable_name[22];
	int i, hasmodify = 0, notifyfirewall = 0;

	memset(conf_word, 0, sizeof(conf_word));
	memset(conf_name, 0, sizeof(conf_name));
	
	for(i = 0; i < 2 * MAX_CHAN; i++)
	{
		int j, which_enable;
		char *p, *ptr, *tmpptr;	
		memset(conf_word, 0, sizeof(conf_word));
		memset(conf_name, 0, sizeof(conf_name));
		memset(enable_name, 0, sizeof(enable_name));
		if(i < MAX_CHAN)
		{
			sprintf(conf_name, "pvc_select_conf_%d", i);
			sprintf(enable_name, "pvc_select_enable_%d", i);
		}
		else
		{
			sprintf(conf_name, "mul_select_conf_%d", i - MAX_CHAN);
			sprintf(enable_name, "mul_select_enable_%d", i - MAX_CHAN);
		}
		
		which_enable = atoi(nvram_safe_get(enable_name));
		
		ptr = nvram_safe_get(conf_name);
		
		for(j=0; j<MAX_CHAN; j++)
		{
			tmpptr = strsep(&ptr, " ");
			
			p = strstr(tmpptr, dnsname);	
			
			if((p) && (*(p-1) == ':') && (*(p+strlen(dnsname)) == ':'))//check fully domain name match
			{
				char dnsentry_name[16];
				memset(dnsentry_name, 0, sizeof(dnsentry_name));
				sprintf(dnsentry_name, "dnsentry_%d", i*16+j);
				if(nvram_invmatch(dnsentry_name, ipaddr))
				{	
					nvram_set(dnsentry_name, ipaddr);
					if(!hasmodify)
						hasmodify = 1;
					if(which_enable & (1<<j) && !notifyfirewall)
						notifyfirewall = 1;
				}
			}
		}
		/*
		foreach(conf_word, nvram_safe_get(conf_name), next)
		{
			if(j++ == 8)
				break;
			
			p = strstr(conf_word, dnsname);	
			
			if((p) && (*(p-1) == ':') && (*(p+strlen(dnsname)) == ':'))//check fully domain name match
			{
				char dnsentry_name[16];
				memset(dnsentry_name, 0, sizeof(dnsentry_name));
				sprintf(dnsentry_name, "dnsentry_%d", i*16+j);
				if(nvram_invmatch(dnsentry_name, ipaddr))
				{	
					nvram_set(dnsentry_name, ipaddr);
					if(!hasmodify)
						hasmodify = 1;
					if(which_enable & (1<<j) && notifyfirewall)
						notifyfirewall = 1;
				}
			}
		}
		*/
	}
	if(hasmodify)
		nvram_commit();
	if(notifyfirewall)
	{
		nvram_set("action_service", "pvc_select");
		//restart firewall!!!
		kill(1, SIGUSR1);
	}
	return;
}

//add by junzhao 2004.3.12
int replace(char ori, char new, char *buf)
{
	char *tmp1 = NULL, *tmp2 = NULL;
	if(buf == NULL)
		return -1;
	tmp1 = buf;
	while((tmp2 = strchr(tmp1, ori))!=NULL)
	{
		*tmp2 = new;
		tmp1 = tmp2 + 1;
	}
	return 0;
}

void my_lock(int fd, int type)
{
	struct flock lock;
	lock.l_type = type;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if(fcntl(fd, F_SETLKW, &lock) < 0)
		strerror(errno);
	
	return;
}

void my_unlock(int fd)
{
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if(fcntl(fd, F_SETLK, &lock) < 0)
		strerror(errno);

	return;
}

//junzhao 2004.11.2 for wanledctrl
int check_redial_proc(int num)
{
	int redial_proc = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/redial%d.pid", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		redial_proc = 1;
	}
	return redial_proc;
}
int check_ppp_begin(int num)
{
	int begin = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.begin", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		begin = 1;
	}
	return begin;
}
int check_udhcpc_down(int num)
{
	int wan_down = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/udhcpc%d.down", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_down = 1;
	}
	return wan_down;
}

//junzhao 2004.6.15
int check_ppp_authfail(int num)
{
	int authfail = 0;
	FILE *fp;
	char filename[30];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/authfail%d", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		authfail = 1;
	}
	return authfail;
}

int check_ppp_link(int num)
{
	int wan_link = 0;
	FILE *fp;
	char filename[18];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_link = 1;
	}
	return wan_link;
}

int check_ppp_down(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[22];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.down", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

int check_ppp_idle(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[22];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/tmp/ppp/link%d.idle", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

int check_ppp_proc(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[22];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/ppp%d.pid", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

/* da_f 2005.4.19 add.
 * check whether fail to connect to PPPoE and PPPoA server */
int check_ppp_connserv_fail(int num)
{
	int fail = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	snprintf(filename, sizeof(filename), "/tmp/ppp/ConnServFail%d", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		fail = 1;
	}
	return fail;
}

/* da_f 2005.4.19 add.
 * check whether fail to get IP */
int check_ppp_getip_fail(int num)
{
	int fail = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	snprintf(filename, sizeof(filename), "/tmp/ppp/GetIPFail%d", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		fail = 1;
	}
	return fail;
}

int check_udhcpc_proc(int num)
{
	int wan_proc = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/udhcpc%d.pid", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_proc = 1;
	}
	return wan_proc;
}

int check_udhcpc_link(int num)
{
	int wan_link = 0;
	FILE *fp;
	char filename[32];

	memset(filename, 0, sizeof(filename));
	sprintf(filename, "/var/run/udhcpc%d.up", num);

	if((fp = fopen(filename, "r")) != NULL)
	{
		fclose(fp);
		wan_link = 1;
	}
	return wan_link;
}


int check_wan_link(int num)
{
	int wan_link = 0;
	int which = num, i;
	char readbuf[60], *tmpstr = readbuf, *waninterface=NULL;

	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("read %s file fail\n", WAN_IFNAME);
		return wan_link;
	}
	/*
	foreach(word, readbuf, next)
	{
		if(which -- == 0)
		{
			waninterface = word;
			break;
		}
	}
	*/
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		waninterface = strsep(&tmpstr, " ");
		if(i == which)
			break;
	}
		
	if(!strcmp(waninterface, "Down"))
		return wan_link;
	if((strstr(waninterface, "ppp")) != NULL)
		wan_link = ((check_ppp_proc(num) == 1) && (check_ppp_link(num) == 1)) ? 1 : 0;
	else
		wan_link = ((check_udhcpc_proc(num) == 1) && (check_udhcpc_link(num) == 0)) ? 0 : 1;
	return wan_link;
}

//end junzhao

int
diag_led_4702(int type, int act)
{
//wwzh
#if 0
	FILE *fp;	
	char string[10];
	int c = 0;
	
	int leds[3][2]={ { 1 , 6},
		         { 2 , 5},
		         { 4 , 3} };

	if( (fp=fopen("/proc/sys/diag", "r")) ){
		fgets(string, sizeof(string), fp);
		fclose(fp);
	}
	else{
		perror("/proc/sys/diag");
		return 0;
	}

	if(act == STOP_LED){
		c = (atoi(string) & leds[type][act]) + 48;
	}
	else if(act == START_LED){
		c = (atoi(string) | leds[type][act]) + 48;
	}
	fprintf(stderr, "diag led = [%d] -> [%c]\n",atoi(string), c);	

	if( (fp=fopen("/proc/sys/diag", "w")) ){
		fputc(c, fp);
		fclose(fp);
	}
	else
		perror("/proc/sys/diag");

#endif
	return 0;
}

int 
C_led_4702(int i)
{
//wwzh
#if 0
        FILE *fp;
        char string[10];
        int flg;

        memset(string,0,10);
        /* get diag before set */
        if( (fp=fopen("/proc/sys/diag", "r")) ){
              fgets(string, sizeof(string), fp);
              fclose(fp);
        }
        else
              perror("/proc/sys/diag");

        if(i) flg=atoi(string) | 0x10 ;
        else flg=atoi(string) & 0xef;

        memset(string,0,10);
        sprintf(string,"%d",flg);
        if( (fp=fopen("/proc/sys/diag", "w")) ){
                fputs(string, fp);
                fclose(fp);
        }
        else
              perror("/proc/sys/diag");
#endif
        return 0;
}

/*
unsigned int 
read_gpio(char *device)
{
	FILE *fp;
	unsigned int val;

        if( (fp=fopen(device, "r")) ){
            fread(&val, 4, 1, fp);
            fclose(fp);
	    //fprintf(stderr, "----- gpio %s = [%X]\n",device,val); 
	    return val;
        }
        else{
            perror(device);
            return 0;
        }
}

unsigned int 
write_gpio(char *device, unsigned int val)
{
        FILE *fp;
 
        if( (fp=fopen(device, "w")) ){
	    fwrite(&val, 4, 1, fp); 
            fclose(fp);
	    //fprintf(stderr, "----- set gpio %s = [%X]\n",device,val); 
            return 1;
        }
        else{
            perror(device);
            return 0;
        }
}
*/
	
int
diag_led_4712(int type, int act)
{
//wwzh
#if 0
	unsigned int control,in,outen,out;
	
#ifdef BCM94712AGR
	/* The router will crash, if we load the code into broadcom demo board. */
	return 1;
#endif
	control=read_gpio("/dev/gpio/control");
	in=read_gpio("/dev/gpio/in");
	out=read_gpio("/dev/gpio/out");
	outen=read_gpio("/dev/gpio/outen");

	
	switch (type) {
		case DIAG:	// GPIO 1
			write_gpio("/dev/gpio/control",control & 0xfd);
			write_gpio("/dev/gpio/outen",outen | 0x02);
			if (act == STOP_LED) { // stop blinking
				write_gpio("/dev/gpio/out",out | 0x02);
			} else if (act == START_LED) { // start blinking
				write_gpio("/dev/gpio/out",out & 0xfd);
			}
			//write_gpio("/dev/gpio/control",control);
			break;
		case DMZ:	// GPIO 7
                        write_gpio("/dev/gpio/control",control & 0x7f);
                        write_gpio("/dev/gpio/outen",outen | 0x80);
                        if (act == STOP_LED) {
                                write_gpio("/dev/gpio/out",out | 0x80);
                        } else if (act == START_LED) {
                                write_gpio("/dev/gpio/out",out & 0x7f);
                        }
                        //write_gpio("/dev/gpio/control",control);
                        break;
	#if 0 
		case WL:	// GPIO 0
                        write_gpio("/dev/gpio/control",control & 0xfe);
                        write_gpio("/dev/gpio/outen",outen | 0x01);
                        if (act == STOP_LED) {
                                write_gpio("/dev/gpio/out",out | 0x01);
                        } else if (act == START_LED) {
                                write_gpio("/dev/gpio/out",out & 0xfe);
                        }
                        //write_gpio("/dev/gpio/control",control);
                        break;
	#endif
	}	
#endif
	return 1;
}				
	
int C_led_4712(int i)
{
//wwzh
#if 0
	if (i==1) 
		return diag_led(DIAG,START_LED);
	else
		return diag_led(DIAG,STOP_LED);
#endif
}		

int 
C_led(int i)
{
#if 0
	if(check_hw_type() == BCM4702_CHIP)
		return C_led_4702(i);
	else
		return C_led_4712(i);
#endif
	return 0;
}

int
diag_led(int type, int act)
{
#if 0
	if(check_hw_type() == BCM4702_CHIP)
		return diag_led_4702(type, act);
	else
		return diag_led_4712(type, act);
#endif
	return 0;
}

char *
get_mac_from_ip(char *ip)
{
	FILE *fp;
        char line[100];

	char ipa[50];           // ip address
	char hwa[50];           // HW address / MAC
	char mask[50];          // ntemask   
	char dev[50];           // interface
	int type;               // HW type
	int flags;              // flags
	static char mac[20]; 
							

        if ((fp = fopen("/proc/net/arp", "r")) == NULL) return NULL;

        // Bypass header -- read until newline 
	if (fgets(line, sizeof(line), fp) != (char *) NULL) {
	      // Read the ARP cache entries.
	      // IP address       HW type     Flags       HW address            Mask     Device
	      // 192.168.1.1      0x1         0x2         00:90:4C:21:00:2A     *        eth0
		for (; fgets(line, sizeof(line), fp);) {
			if(sscanf(line, "%s 0x%x 0x%x %100s %100s %100s\n", ipa, &type, &flags, hwa, mask, dev)!=6)
				continue;
			//cprintf("ip1=[%s] ip2=[%s] mac=[%s] (flags & ATF_COM)=%d\n", ip, ipa, hwa, (flags & ATF_COM));
			if(strcmp(ip, ipa))
				continue;
			//if (!(flags & ATF_COM)) {       //ATF_COM = 0x02   completed entry (ha valid)
				strcpy(mac, hwa);
				fclose(fp);
				return mac;
			//}
		}
	}

	fclose(fp);
	return "";
}
//junzhao 2004.6.26
#define WAN_IFNAME "/tmp/status/wan_ifname"
#define WAN_GET_DNS "/tmp/status/wan_get_dns"
#if defined(MPPPOE_SUPPORT)
	#define WAN_MPPPOE_IFNAME "/tmp/status/wan_mpppoe_ifname"
	#define WAN_MPPPOE_GET_DNS "/tmp/status/wan_mpppoe_get_dns"
#endif
#if defined(IPPPOE_SUPPORT)
	#define WAN_IPPPOE_IFNAME "/tmp/status/wan_ipppoe_ifname"
	#define WAN_IPPPOE_GET_DNS "/tmp/status/wan_ipppoe_get_dns"
#endif

struct dns_lists *
get_dns_list_total()
{
        char list[600];
        char *next, ifword[6], word[32];
        struct dns_lists *dns_list = NULL;
	int i = 0, match = 0;
	//char dnsbuf[32];
	char pdns[16], sdns[16];
	//int which = index;
	char dnsreadbuf[520], *tmpstrdns = dnsreadbuf;
	char ifreadbuf[60], *tmpstrif = ifreadbuf;
	char *ifptr, *dnsptr;
	char *cur = list;

        dns_list = (struct dns_lists *)malloc(sizeof(struct dns_lists));
        memset(dns_list, 0, sizeof(struct dns_lists));

        dns_list->num_servers = 0;
		
	if(!file_to_buf(WAN_IFNAME, ifreadbuf, sizeof(ifreadbuf)))
		return NULL;
	if(!file_to_buf(WAN_GET_DNS, dnsreadbuf, sizeof(dnsreadbuf)))
		return NULL;
	
	memset(word, 0, sizeof(word));
	memset(ifword, 0, sizeof(ifword));
	memset(list, 0, sizeof(list));
       	sprintf(list, "%s ", nvram_safe_get("wan_dns"));
	cur = list + strlen(list);
//junzhao 2004.8.4	
	for(i=0; i<MAX_CHAN; i++)
	{
		ifptr = strsep(&tmpstrif, " ");
		dnsptr = strsep(&tmpstrdns, " ");
		if(strcmp(ifptr, "Down"))
		{
			if(!strstr(ifptr, "ppp") || check_ppp_link(i))
			{
				//printf("dnsptr %d %s\n", i, dnsptr);
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(dnsptr, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
	}
		
	/*
	foreach(ifword, ifreadbuf, ifnext)
	{
		if(strcmp(ifword, "Down"))
		{
			if(!strstr(ifword, "ppp") || check_ppp_link(i))
			{
				int j = i;
				memset(word, 0, sizeof(word));
				foreach(word, dnsreadbuf, next)
				{
					if(j -- == 0)
						break;
				}
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(word, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
		memset(ifword, 0, sizeof(ifword));
		if(++i == 8)
		{
			i = 0;
			break;
		}
	}	
	*/
#ifdef MPPPOE_SUPPORT
	if(!file_to_buf(WAN_MPPPOE_IFNAME, ifreadbuf, sizeof(ifreadbuf)))
		return NULL;
	if(!file_to_buf(WAN_MPPPOE_GET_DNS, dnsreadbuf, sizeof(dnsreadbuf)))
		return NULL;
	//junzhao 2004.8.4
	tmpstrif = ifreadbuf;
	tmpstrdns = dnsreadbuf;
	for(i=0; i<MAX_CHAN; i++)
	{
		ifptr = strsep(&tmpstrif, " ");
		dnsptr = strsep(&tmpstrdns, " ");
		if(strcmp(ifptr, "Down"))
		{
			if(!strstr(ifptr, "ppp") || check_ppp_link(i))
			{
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(dnsptr, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
	}

/*
	foreach(word, ifreadbuf, next)
	{
		if(strcmp(word, "Down"))
		{
			if(!strstr(word, "ppp") || check_ppp_link(8+i))
			{
				int j = i;
				memset(word, 0, sizeof(word));
				foreach(word, dnsreadbuf, next)
				{
					if(j -- == 0)
						break;
				}
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(word, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
		memset(word, 0, sizeof(word));
		if(++i == 8)
		{
			i = 0;
			break;
		}
	}	
*/
#endif
#ifdef IPPPOE_SUPPORT
	if(!file_to_buf(WAN_IPPPOE_IFNAME, ifreadbuf, sizeof(ifreadbuf)))
		return NULL;
	if(!file_to_buf(WAN_IPPPOE_GET_DNS, dnsreadbuf, sizeof(dnsreadbuf)))
		return NULL;
	//junzhao 2004.8.4
	tmpstrif = ifreadbuf;
	tmpstrdns = dnsreadbuf;
	for(i=0; i<MAX_CHAN; i++)
	{
		ifptr = strsep(&tmpstrif, " ");
		dnsptr = strsep(&tmpstrdns, " ");
		if(strcmp(ifptr, "Down"))
		{
			if(!strstr(ifptr, "ppp") || check_ppp_link(i))
			{
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(dnsptr, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
	}

/*	
	foreach(word, ifreadbuf, next)
	{
		if(strcmp(word, "Down"))
		{
			if(!strstr(word, "ppp") || check_ppp_link(8+i))
			{
				int j = i;
				memset(word, 0, sizeof(word));
				foreach(word, dnsreadbuf, next)
				{
					if(j -- == 0)
						break;
				}
				memset(pdns, 0, sizeof(pdns));
				memset(sdns, 0, sizeof(sdns));
				sscanf(word, "%16[^:]:%16[^\n]", pdns, sdns);
        			sprintf(cur, "%s %s ", pdns, sdns);
				cur = list + strlen(list);
			}		
		}
		memset(word, 0, sizeof(word));
		if(++i == 8)
		{
			i = 0;
			break;
		}
	}	
*/
#endif
	list[strlen(list)] = '\0';
	printf(".............list %s..............\n", list);
	
        foreach(word, list, next) 
	{
                if(strcmp(word, "0.0.0.0") && strcmp(word, "")){
			match = 0;
			for(i=0 ; i<dns_list->num_servers ; i++){	// Skip same DNS
				if(!strcmp(dns_list->dns_server[i], word))	match = 1;
			}
			if(!match){
				snprintf(dns_list->dns_server[dns_list->num_servers], sizeof(dns_list->dns_server[dns_list->num_servers]), "%s", word);
				dns_list->num_servers ++ ;
			}
                }
                //if(dns_list->num_servers == 3)      break;	// We only need 3 counts DNS entry
        }
        return dns_list;
}

struct dns_lists *
get_dns_list(int index)
{
        char list[254];
        char *next, word[254];
        struct dns_lists *dns_list = NULL;
	int i, match = 0;
	char dnsbuf[32], *wan_get_dns = NULL;
	char pdns[16], sdns[16];
	int which = index;
	char readbuf[260], *tmpstr = readbuf;

        dns_list = (struct dns_lists *)malloc(sizeof(struct dns_lists));
        memset(dns_list, 0, sizeof(struct dns_lists));

        dns_list->num_servers = 0;

	// nvram_safe_get("wan_dns") ==> Set by user
	// nvram_safe_get("wan_get_dns") ==> Get from DHCP, PPPoE or PPTP
	// The nvram_safe_get("wan_dns") priority is higher than nvram_safe_get("wan_get_dns")
	
//junzhao
	
	memset(dnsbuf, 0, sizeof(dnsbuf));
	memset(pdns, 0, sizeof(pdns));
	memset(sdns, 0, sizeof(sdns));
        //foreach(dnsbuf, nvram_safe_get("wan_get_dns"), next) 
	if(!file_to_buf("/tmp/status/wan_get_dns", readbuf, sizeof(readbuf)))
	{
		printf("read %s file fail\n", "/tmp/status/wan_get_dns");
		sprintf(pdns, "0.0.0.0");
		sprintf(sdns, "0.0.0.0");
		//return NULL;
	}
	else
	{	
		/*
        	foreach(dnsbuf, readbuf, next) 
		{
			if(which-- == 0)
			{
				wan_get_dns = dnsbuf;
				break;
			}
		}
		sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
		*/
		//junzhao 2004.8.4
		for(i=0; i<MAX_CHAN; i++)
		{
			wan_get_dns = strsep(&tmpstr, " ");
			if(i == which)
				break;
		}
		sscanf(wan_get_dns, "%16[^:]:%16[^\n]", pdns, sdns);
	}
	
	
        snprintf(list, sizeof(list), "%s %s %s", nvram_safe_get("wan_dns"), pdns, sdns);

        foreach(word, list, next) {
                if(strcmp(word, "0.0.0.0") && strcmp(word, "")){
			match = 0;
			for(i=0 ; i<dns_list->num_servers ; i++){	// Skip same DNS
				if(!strcmp(dns_list->dns_server[i], word))	match = 1;
			}
			if(!match){
				snprintf(dns_list->dns_server[dns_list->num_servers], sizeof(dns_list->dns_server[dns_list->num_servers]), "%s", word);
				dns_list->num_servers ++ ;
			}
                }
                if(dns_list->num_servers == 3)      break;	// We only need 3 counts DNS entry
        }
        return dns_list;
}

struct dns_lists *
get_dns_list_every(int index, int way)
{
        char list[254];
        char *next, word[254];
        struct dns_lists *dns_list = NULL;
	int i, match = 0;
	char dnsbuf[32], *wan_get_dns = NULL;
	char pdns[16], sdns[16];
	int which = index;
	char readbuf[260], *tmpstr = readbuf;

        dns_list = (struct dns_lists *)malloc(sizeof(struct dns_lists));
        memset(dns_list, 0, sizeof(struct dns_lists));

        dns_list->num_servers = 0;

	// nvram_safe_get("wan_dns") ==> Set by user
	// nvram_safe_get("wan_get_dns") ==> Get from DHCP, PPPoE or PPTP
	// The nvram_safe_get("wan_dns") priority is higher than nvram_safe_get("wan_get_dns")
	
//junzhao
	
	memset(dnsbuf, 0, sizeof(dnsbuf));
	memset(pdns, 0, sizeof(pdns));
	memset(sdns, 0, sizeof(sdns));

	if(way == 0) //pvc
	{
		if(!file_to_buf("/tmp/status/wan_get_dns", readbuf, sizeof(readbuf)))
		{
			printf("read %s file fail\n", "/tmp/status/wan_get_dns");
			sprintf(pdns, "0.0.0.0");
			sprintf(sdns, "0.0.0.0");
			//return NULL;
		}
	}
	else if(way == 1) // multi pppoe
	{
		if(!file_to_buf("/tmp/status/wan_mpppoe_get_dns", readbuf, sizeof(readbuf)))
		{
			printf("read %s file fail\n", "/tmp/status/wan_mpppoe_get_dns");
			sprintf(pdns, "0.0.0.0");
			sprintf(sdns, "0.0.0.0");
			//return NULL;
		}
	}
	else if(way == 2) // ip pppoe
	{
		if(!file_to_buf("/tmp/status/wan_ipppoe_get_dns", readbuf, sizeof(readbuf)))
		{
			printf("read %s file fail\n", "/tmp/status/wan_ipppoe_get_dns");
			sprintf(pdns, "0.0.0.0");
			sprintf(sdns, "0.0.0.0");
			//return NULL;
		}
	}
	//junzhao 2004.8.4
	for(i=0; i<MAX_CHAN; i++)
	{
		wan_get_dns = strsep(&tmpstr, " ");
		if(i == which)
			break;
	}
	sscanf(wan_get_dns, "%16[^:]:%16[^\n]", pdns, sdns);

/*
        foreach(dnsbuf, readbuf, next) 
	{
		if(which-- == 0)
		{
			wan_get_dns = dnsbuf;
			break;
		}
	}		
	sscanf(dnsbuf, "%16[^:]:%16[^\n]", pdns, sdns);
*/
	
        snprintf(list, sizeof(list), "%s %s", pdns, sdns);

        foreach(word, list, next) 
	{
                if(strcmp(word, "0.0.0.0") && strcmp(word, "")){
			match = 0;
			for(i=0 ; i<dns_list->num_servers ; i++){	// Skip same DNS
			if(!strcmp(dns_list->dns_server[i], word))	match = 1;
			}
			if(!match)
			{	
				snprintf(dns_list->dns_server[dns_list->num_servers], sizeof(dns_list->dns_server[dns_list->num_servers]), "%s", word);
				dns_list->num_servers ++ ;
			}
       		}
             	if(dns_list->num_servers == 3)      break;	// We only need 3 counts DNS entry
        }
        return dns_list;
}

//junzhao
#if 0
int
dns_to_resolv(int index)
{
	FILE *fp_w;
	struct dns_lists *dns_list = NULL;
	int i = 0;

	/* Save DNS to resolv.conf */
	if (!(fp_w = fopen(RESOLV_FILE, "w"))) {
                perror(RESOLV_FILE);
                return errno;
        }

	dns_list = get_dns_list(index);

        for(i=0 ; i<dns_list->num_servers ; i++)
                fprintf(fp_w, "nameserver %s\n", dns_list->dns_server[i]);
	
#ifdef DNSMASQ_SUPPORT
	/* Put a pseudo DNS IP to trigger Connect On Demand */
	/*
	if(dns_list->num_servers == 0 && 
	   (nvram_match("wan_proto","pppoe") || nvram_match("wan_proto","pptp")) && 
	   nvram_match("ppp_demand","1"))
              fprintf(fp_w, "nameserver 1.1.1.1\n");
	 */
	if(dns_list->num_servers == 0)
	{
		if(!strcmp(encapmode, "pppoe"))
		{
			if(!strcmp(e_demand, "1"))
               			 fprintf(fp_w, "nameserver 1.1.1.1\n");
		}
		else if(!strcmp(encapmode, "pppoa"))
		{
			if(!strcmp(a_demand, "1"))
               			 fprintf(fp_w, "nameserver 1.1.1.1\n");
		}
	}
#endif

        fclose(fp_w);
        if(dns_list)    free(dns_list);

	eval("touch", "/tmp/hosts");
				
	return 1;
}
#endif

/* Example:
 * lan_ipaddr = 192.168.1.1
 * get_dns_ip("lan_ipaddr", 1); produces "168"
 */
int
get_single_ip(char *ipaddr, int which)
{
	int ip[4]={0,0,0,0};
	int ret;

	ret = sscanf(ipaddr,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);

	return ip[which];	
}

char *
get_complete_lan_ip(char *ip)
{
	static char ipaddr[20];

	int i[4]; 

	if(sscanf(nvram_safe_get("lan_ipaddr"),"%d.%d.%d.%d",&i[0],&i[1],&i[2],&i[3]) != 4)
                 return "0.0.0.0";

	snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%s", i[0],i[1],i[2],ip);

	return ipaddr;
}

char *
get_wan_face(void)
{
	static char wanface[IFNAMSIZ];
	
	if(nvram_match("wan_proto", "pptp") || nvram_match("wan_proto", "pppoe"))
		strncpy(wanface, "ppp+", IFNAMSIZ);
	else
		strncpy(wanface, nvram_safe_get("wan_ifname"), IFNAMSIZ);

	return wanface;
}

int
get_ppp_pid(char *file)
{
	char buf[80];
	int pid = -1;
	if(file_to_buf(file, buf, sizeof(buf))){
		char tmp[80], tmp1[80];
		snprintf(tmp, sizeof(tmp), "/var/run/%s.pid", buf);
		file_to_buf(tmp, tmp1, sizeof(tmp1));
		pid = atoi(tmp1);
	}
	return pid;
}
/*
int
check_wan_link(int num)
{
	int wan_link = 0;
	
	if(nvram_match("wan_proto", "pptp") || nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "heartbeat")){
		FILE *fp;
		char filename[80];
		char *name;

		if(num == 0)
			strcpy(filename, "/tmp/ppp/link");
#ifdef MPPPOE_SUPPORT
		else	// for multi pppoe
			strcpy(filename, "/tmp/ppp/link_1");
#endif
		if ((fp = fopen(filename, "r"))){
			int pid = -1;
			fclose(fp);
#ifdef HEARTBEAT_SUPPORT
			if(nvram_match("wan_proto", "heartbeat")){
				char buf[20];
				file_to_buf("/tmp/ppp/link", buf, sizeof(buf));
				pid = atoi(buf);
			}
			else
#endif
				pid = get_ppp_pid(filename);
			
			name = find_name_by_proc(pid);
			if(!strncmp(name, "pppoecd", 7) ||	// for PPPoE
			   !strncmp(name, "pppd", 4) ||		// for PPTP
			   !strncmp(name, "bpalogin", 8))	// for HeartBeat
				wan_link = 1;     //connect
			else{
				printf("The %s had been died, remove %s\n", nvram_safe_get("wan_proto"), filename);
				wan_link = 0;	// For some reason, the pppoed had been died, by link file still exist.
				unlink(filename);
			}
		}
	}
	else{
		if(nvram_invmatch("wan_ipaddr", "0.0.0.0"))
			wan_link = 1;
	}
				
	return wan_link;	
}
*/
int
get_int_len(int num)
{
	char buf[80];

	snprintf(buf, sizeof(buf), "%d", num);
	
	return strlen(buf);
}

//change by lijunzhao for lock
/*
int
file_to_buf_withlock(char *path, char *buf, int len)
{
	int fd;
	ssize_t n;
	memset(buf, 0, len);
	if((fd = open(path, O_RDONLY)) > 0)
	{
		my_lock(fd, F_RDLCK);
		if((n = read(fd, buf, len)) > 0)
		{
			buf[n] = '\0';
			//printf("has read %d bytes, buf is %s\n", n, buf);
			my_unlock(fd);
			close(fd);
			return 1;
		}
		my_unlock(fd);
		close(fd);
	}
	return 0;	
}

int
buf_to_file_withlock(char *path, char *buf)
{
	int fd;
	ssize_t n;
	if((fd = open(path, O_WRONLY)) > 0);
	{
		my_lock(fd, F_WRLCK);
		ftruncate(fd, 0);
		if((n = write(fd, buf, strlen(buf))) > 0)
		{
			my_unlock(fd);
			close(fd);
			return 1;
		}
		my_unlock(fd);
		close(fd);
	}
	return 0;
}
*/
int
file_to_buf(char *path, char *buf, int len)
{
#if 0
	FILE *fp;

	memset(buf, 0 , len);

	if ((fp = fopen(path, "r"))) {
		fgets(buf, len, fp);
		fclose(fp);
		return 1;
	}
#endif	
#if 1
	int fd;
	ssize_t n;
	memset(buf, 0, len);
	if((fd = open(path, O_RDONLY)) > 0)
	{
		my_lock(fd, F_RDLCK);
		if((n = read(fd, buf, len)) > 0)
		{
			buf[n] = '\0';
			//printf("has read %d bytes, buf is %s\n", n, buf);
			my_unlock(fd);
			close(fd);
			return 1;
		}
		my_unlock(fd);
		close(fd);
	}	
#endif
	return 0;
}

int
buf_to_file(char *path, char *buf)
{
#if 0
	FILE *fp;

	if ((fp = fopen(path, "w"))) {
		fprintf(fp, "%s", buf);
		fclose(fp);
		return 1;
	}
#endif
#if 1
	int fd;
	ssize_t n;
	if((fd = open(path, O_WRONLY|O_CREAT)) > 0);
	{
		my_lock(fd, F_WRLCK);
		ftruncate(fd, 0);
		if((n = write(fd, buf, strlen(buf))) > 0)
		{
			my_unlock(fd);
			close(fd);
			return 1;
		}
		my_unlock(fd);
		close(fd);
	}
#endif
	return 0;
}

#ifdef HSIAB_SUPPORT
static int hsiab_level=0;
char *
send_command(char *send_value, char *get_value)
{
	struct sockaddr_in serv_addr;
	int sockfd, retrycnt = 0;
	char buf[MAX_BUF_LEN];
	FILE *fp;

	// We want to confirm that hsiabd if exist
	if(nvram_invmatch("hsiab_mode","1"))	return "";
	if(!(fp = fopen("/var/run/hsiabd.pid", "r")))	return "";
	else	fclose(fp);
	
retry:
	if((sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0)
	{
		ct_syslog(LOG_ERR, hsiab_level, "Cann't create socket!" );
		if (retrycnt++ > MAX_RETRY_CNT)
			return "";
		goto retry;
	}
	
	bzero( (char *) &serv_addr, sizeof( serv_addr ) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
	serv_addr.sin_port = htons( HSIABD_PORT );
	
	connect( sockfd, (struct sockaddr *) &serv_addr, sizeof( serv_addr ) );
	
	strcpy(buf, send_value);
	ct_syslog(LOG_DEBUG, hsiab_level, "Client : SEND=[%s] %s\n", buf, get_value ? "(need to receive return value)" : "(not need to receive return value)");
	send( sockfd, buf, strlen( buf ), 0 );
	
	if(get_value){
		int len = recv( sockfd, get_value, MAX_BUF_LEN, 0 );
		get_value[len] = '\0';
		ct_syslog(LOG_DEBUG, hsiab_level, "Client : RECV=[%s]\n", get_value);
		close( sockfd );
		return get_value;
	}
	close( sockfd );

	return "";
}

struct database *
open_DB1(int *c)
{
	FILE *fp;
	struct database D, *myDB = NULL;
	int count;

	myDB = (struct database *) malloc(sizeof(struct database));
	count = *c = 0;
	
	if((fp = fopen(HSIAB_DB, "r"))){
		while (fread(&D, sizeof(struct database), 1, fp)) {
			if(D.key == 1){
				if(count > 0){
					myDB = realloc(myDB, sizeof(struct database) * (count+1));	
					if(!myDB)
						ct_syslog(LOG_ERR, hsiab_level, "realloc fail!\n");
				}
				memcpy(&myDB[count], &D, sizeof(struct database));
				count ++;
			}
		}
			
		fclose(fp);
		ct_syslog(LOG_DEBUG, hsiab_level, "Count of database is %d\n", count);
		*c = count;
		return myDB;
	}
	else
		ct_syslog(LOG_DEBUG, hsiab_level, "Cann't open %s\n", HSIAB_DB);

	return NULL;
}

#endif

#define READ_BUF_SIZE 254
/* from busybox find_pid_by_name.c */
pid_t* find_pid_by_name( char* pidName)
{
    DIR *dir;
    struct dirent *next;
    pid_t* pidList=NULL;
    int i=0;

    dir = opendir("/proc");

    while ((next = readdir(dir)) != NULL) {
        FILE *status;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        char name[READ_BUF_SIZE];

        /* Must skip ".." since that is outside /proc */
        if (strcmp(next->d_name, "..") == 0)
            continue;

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))
            continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (! (status = fopen(filename, "r")) ) {
            continue;
        }
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) {
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
        sscanf(buffer, "%*s %s", name);
	//printf("buffer=[%s] name=[%s]\n",buffer,name);
        if (strcmp(name, pidName) == 0) {
            pidList=realloc( pidList, sizeof(pid_t) * (i+2));
            pidList[i++]=strtol(next->d_name, NULL, 0);
        }
    }

    if (pidList)
        pidList[i]=0;
    else {
        pidList=realloc( pidList, sizeof(pid_t));
        pidList[0]=-1;
    }
    return pidList;

}

/* Find first process pid with same name from ps command */
int find_pid_by_ps(char* pidName)
{
        FILE * fp;
        int pid= -1 ;
        char line[254];

        if((fp = popen("ps", "r"))){
                while( fgets(line, sizeof(line), fp) != NULL ) {
                        if(strstr(line, pidName)){
                                sscanf(line, "%d", &pid);
                                printf("%s pid is %d\n", pidName, pid);
                                break;
                        }
                }
                pclose(fp);
        }
	
        return pid;
}

/* Find process name by pid from /proc directory */
char *find_name_by_proc(int pid)
{
        FILE *fp;
        char line[254];
	char filename[80];
	static char name[80];

	snprintf(filename, sizeof(filename), "/proc/%d/status", pid);

	if((fp = fopen(filename, "r"))){
		fgets(line, sizeof(line), fp);
        	/* Buffer should contain a string like "Name:   binary_name" */
		sscanf(line, "%*s %s", name);
        	fclose(fp);
		return name;
        }

        return "";
}

/* Find all process pid with same name from ps command */
int *find_all_pid_by_ps(char* pidName)
{
        FILE * fp;
        int pid= -1 ;
        char line[254];
	int *pidList = NULL;
	int i = 0;

        if((fp = popen("ps", "r"))){
                while( fgets(line, sizeof(line), fp) != NULL ) {
                        if(strstr(line, pidName)){
                                sscanf(line, "%d", &pid);
                                printf("%s pid is %d\n", pidName, pid);
            			pidList = realloc( pidList, sizeof(int) * (i+2));
				pidList[i++] = pid;
                        }
                }
                pclose(fp);
        }
	if (pidList)
		pidList[i]=0;
	else {
		pidList = realloc( pidList, sizeof(int));
		pidList[0] = -1;
	}

        return pidList;
}

#ifdef BACKUP_RESTORE_SUPPORT
void
encode(char *buf, int len)
{
	int i;
	char ch;

	for(i=0 ; i<len ; i++){
		ch = (buf[i] & 0x03) << 6;
	        buf[i] = (buf[i] >> 2);                
	        buf[i] &= 0x3f;
	        buf[i] |= ch;
		buf[i] = ~buf[i];
	}
}

void
decode(char *buf, int len)
{
	int i;
	char ch;
	
	for(i=0 ; i<len ; i++){
		ch = (buf[i] & 0xC0) >> 6;
	        buf[i] = (buf[i] << 2) | ch;
		buf[i] = ~buf[i];
	}
}
#endif

/*	v1.41.7 => 014107
 *	v1.2	=> 0102
 */
long
convert_ver(char *ver)
{
        char buf[10];
        int v[3];
	int ret;

        ret = sscanf(ver,"v%d.%d.%d", &v[0], &v[1], &v[2]);

	if(ret == 2){
        	snprintf(buf, sizeof(buf), "%02d%02d", v[0], v[1]);
		return atol(buf);
	}
	else if (ret == 3){
        	snprintf(buf, sizeof(buf), "%02d%02d%02d", v[0], v[1], v[2]);
		return atol(buf);
	}
	else
		return -1;
}

/* To avoid user to download old image that is not support intel flash to new hardware with intel flash.
 */
int
check_flash(void)
{
#if 0
	// The V2 image can support intel flash completely, so we don't want to check.
	if(check_hw_type() == BCM4712_CHIP)
		return FALSE;
	
	// The V1.X some images cann't support intel flash, so we want to avoid user to downgrade.
	if(nvram_match("skip_amd_check", "1")){
		if(strstr(nvram_safe_get("flash_type"), "Intel") && nvram_invmatch("skip_intel_check", "1"))
			return TRUE;
		else
			return FALSE;
	}
	else	// Cann't downgrade to old firmware version, no matter AMD or Intel flash
		return TRUE;
#endif
	return FALSE;
}

int
check_action(void)
{
	char buf[80] = "";
	
	if(file_to_buf(ACTION_FILE, buf, sizeof(buf))){
		if(!strcmp(buf, "ACT_TFTP_UPGRADE")){
			fprintf(stderr, "Upgrading from tftp now, quiet exit....\n");
			return ACT_TFTP_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_WEBS_UPGRADE")){
			fprintf(stderr, "Upgrading from web (https) now, quiet exit....\n");
			return ACT_WEBS_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_WEB_UPGRADE")){
			fprintf(stderr, "Upgrading from web (http) now, quiet exit....\n");
			return ACT_WEB_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_SW_RESTORE")){
			fprintf(stderr, "Receive restore command from web, quiet exit....\n");
			return ACT_SW_RESTORE;
		}
		else if(!strcmp(buf, "ACT_HW_RESTORE")){
			fprintf(stderr, "Receive restore commond from resetbutton, quiet exit....\n");
			return ACT_HW_RESTORE;
		}
	}
	//fprintf(stderr, "Waiting for upgrading....\n");
	return ACT_IDLE;
}

int
check_now_boot(void)
{
	char *ver = nvram_safe_get("pmon_ver");	

	// for 4712
	// The boot_ver value is lower v2.0 (no included)
	if(!strncmp(ver, "PMON", 4)){
		cprintf("The boot is PMON\n");
		return PMON_BOOT;
	}
	// for 4712
	// The boot_ver value is higher v2.0 (included)
	else if(!strncmp(ver, "CFE", 3)){
		cprintf("The boot is CFE\n");
		return CFE_BOOT;
	}
	else{
		cprintf("The boot is UNKNOWN\n");
		return UNKNOWN_BOOT;
	}
}

int
check_hw_type(void)
{
	if(nvram_match("boardtype", "bcm94710dev")){
		cprintf("The chipset is BCM4702\n");
		return BCM4702_CHIP;	
	}
	else{
		cprintf("The chipset is BCM4712\n");
		return BCM4712_CHIP;
	}
}

int is_exist(char *filename)
{
	FILE *fp;
	
	if((fp = fopen(filename, "r"))){
		fclose(fp);
		return 1;
	}
	return 0;
}

int
ct_openlog(const char *ident, int option, int facility, char *log_name)
{
	int level = atoi(nvram_safe_get(log_name));

	switch(level){
#ifdef SYSLOG_SUPPORT
		case SYSLOG_LOG:
			openlog(ident, option, facility);
			setlogmask(0xcf);
			break;
		case SYSLOG_DEBUG:
			openlog(ident, option, facility);
			break;
#endif
		case CONSOLE_ONLY:
			break;
#ifdef SYSLOG_SUPPORT
		case LOG_CONSOLE:
			openlog(ident, option, facility);
			setlogmask(0xcf);
			break;
		case DEBUG_CONSOLE:
			openlog(ident, option, facility);
			break;
#endif
	}
	return level;
}


void
ct_syslog(int level, int enable, const char *fmt,...)
{
        char buf[1000];
        va_list args;
	
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args); 

	switch(enable){
#ifdef SYSLOG_SUPPORT
		case SYSLOG_LOG:
		case SYSLOG_DEBUG:
   	     		syslog(level, "%s", buf);
			break;
		case LOG_CONSOLE:
		case DEBUG_CONSOLE:
   	     		syslog(level, "%s", buf);
			cprintf("[%d] %s\n", getpid(), buf);	// print to console
			break;
#endif
		case CONSOLE_ONLY:
			cprintf("[%d] %s\n", getpid(), buf);	// print to console
			break;
	}
}


int search_status(int which)
{
	char word[120],tstatus[3],tname[40],ltype[3],lconn[3],rtype[3],sgtype[3],enc[3],auth[3],ktype[3],*next;
	int i,bkeep = 0;
	FILE *fp;
	fp = fopen("/tmp/4test","w");
	
	for(i = 0;i < 5;i++)
	{
		int count = i,findit = 0;
		//if(i == j)
		//	break;
		memset(tstatus,0,sizeof(tstatus));
		memset(tname,0,sizeof(tname));
		memset(ltype,0,sizeof(ltype));
		memset(lconn,0,sizeof(lconn));
		memset(rtype,0,sizeof(rtype));
		memset(sgtype,0,sizeof(sgtype));
		memset(enc,0,sizeof(enc));
		memset(auth,0,sizeof(auth));
		memset(ktype,0,sizeof(ktype));
		memset(word,0,sizeof(word));
		
		foreach(word, nvram_safe_get("entry_config"), next) 
		{
			if (count-- == 0) 
			{
				sscanf(word, "%3[^:]:%40[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tstatus,tname,ltype, lconn,rtype, sgtype, enc, auth, ktype);
				if(atoi(lconn) == which)
				{
					#ifdef ZB_DEBUG
					fprintf(fp,"tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s,key_type=%s\n", tstatus,tname,ltype,lconn,rtype,sgtype,enc,auth,ktype);
					#endif 
					findit = 1;
				}
				fprintf(fp,"%d:findit=%d\n",i,findit);
				break;
			}
		}

		if(findit)
			continue;
		else
		{
			if(!strcmp(tstatus,"1"))
			{
				bkeep = 1;
			}
		}
	}
	if(!bkeep)
	{
		fprintf(fp,"bkeep=%d\n",bkeep);
		printf("No enabled tunnels for other pvcs!\n");
		if(killipsec(6) != 0)
		{
			printf("Cannot killall ipsec!\n");
			return -1;
		}
	}
	fclose(fp);
	return 0;

}

int killipsec(int bflag)
{
	if(bflag == 6) //wanna kill pluto
	{
		char ipsec_ifname[8];
		FILE *kill_fp;
		int t;
	
		printf("Wanna kill pluto!\n");	
		memset(ipsec_ifname,0,sizeof(ipsec_ifname));
		if((kill_fp = fopen("/tmp/killipsec","w")) == NULL)
		{
			printf("Create killipsec failed!\n");
			return -1;
		}
		
		fprintf(kill_fp,"#!/bin/sh\n");
#ifndef NOMAD_SUPPORT
		fprintf(kill_fp,"echo\n");
		fprintf(kill_fp,"/bin/ipsec/eroute --clear 2> /dev/null > /dev/null\n");
		fprintf(kill_fp,"echo\n");
		fprintf(kill_fp,"/bin/ipsec/spi --clear 2> /dev/null > /dev/null\n");
		fprintf(kill_fp,"echo\n");
		for(t = 0; t < 5; t++)
#else
		for(t = 1; t < 5; t++)//zhs md 0 to 1 for quickvpn,4-7-2005
#endif
		{
			sprintf(ipsec_ifname,"ipsec%d",t);
			fprintf(kill_fp,"/bin/ipsec/tncfg --detach --virtual %s 2> /dev/null > /dev/null\n",ipsec_ifname);
			fprintf(kill_fp,"echo\n");
			fprintf(kill_fp,"ifconfig %s down 2> /dev/null > /dev/null\n",ipsec_ifname);
			fprintf(kill_fp,"echo\n");
			fprintf(kill_fp,"ifconfig %s 0.0.0.0 2> /dev/null > /dev/null\n",ipsec_ifname);
		}
	
		fprintf(kill_fp,"rm /tmp/ipsecst/* 2> /dev/null > /dev/null\n");
#ifndef NOMAD_SUPPORT
		fprintf(kill_fp,"killall -9 pluto 2> /dev/null > /dev/null\n");
		fprintf(kill_fp,"echo\n");
		fprintf(kill_fp,"rm /var/run/pluto.pid 2> /dev/null > /dev/null\n");
#endif
		fprintf(kill_fp,"rm /tmp/eroute_* 2> /dev/null > /dev/null\n");
		fprintf(kill_fp,"rm /tmp/whack_* 2> /dev/null > /dev/null\n");

		fclose(kill_fp);
		eval("/bin/sh","/tmp/killipsec");
		
		{
			FILE *ipt_fd;
			char buffer[256];
			int bfs = 0;
			memset(buffer,0,sizeof(buffer));
			if((ipt_fd = fopen("/tmp/vpn-ipt","r")) != NULL)
			{
				bfs = 1;
				while(fgets(buffer,256,ipt_fd))
				{
					//if(strstr(buffer,"oif"))
					{
						char *p;
						if((p = strstr(buffer,"-I")) != NULL)
						{
							*(p+1) = 'D';
							printf("will execute -D!\n");
							system(buffer);
						}
					}
						
				}
				fclose(ipt_fd);
			}
			if(bfs)
			{
				remove("/tmp/vpn-ipt");
				printf("remove vpn-ipt finished!\n");
			}
		}

		//nvram_set("ipsec_enable","0");
		//nvram_commit();
	}
	else //only down specific ipsec interface
	{
		char auto_file[20];
		
		memset(auto_file,0,sizeof(auto_file));
		sprintf(auto_file,"/tmp/autode_%d",bflag);
		printf("eval %s\n",auto_file);
		eval("/bin/sh",auto_file);
	}
	return 0;
}
#ifdef NOMAD_SUPPORT
//zhs add for quickvpn
/**************************************
  *
  */
  void stop_pluto(void)
{
                                                                                                                            
#define STOP_PLTUO_TN "/tmp/stop_pluto"
        FILE *st_pl;
                                                                                                                            
        st_pl =fopen(STOP_PLTUO_TN,"w");
        if(NULL==st_pl)
        {
                printf("%s%s Open %s failed!\n",__FILE__,__FUNCTION__,STOP_PLTUO_TN);
                        return ;
        }
        fprintf(st_pl,"#!/bin/sh\n");
	fprintf(st_pl,"echo\n");
	fprintf(st_pl,"/bin/ipsec/eroute --clear 2> /dev/null > /dev/null\n");
	fprintf(st_pl,"echo\n");
	fprintf(st_pl,"/bin/ipsec/spi --clear 2> /dev/null > /dev/null\n");
        fprintf(st_pl,"killall pluto\n");
        fprintf(st_pl,"rm -f /var/run/pluto.*\n");
        fprintf(st_pl,"echo\n");
        fclose(st_pl);
        eval("/bin/sh",STOP_PLTUO_TN);
                                                                                                                            
}
#endif
int down_ipsec(char *wan_if,int which)
{
	int ret = 0;

	if (nvram_match("ipsec_enable", "1"))
	{
		int j = 0,num_tunn = 0;
		FILE *ter_fd;
		char word[120],*next;
		char tunnel_status[3],tunnel_name[40],local_type[3],localgw_conn[3],remote_type[3], sg_type[3], enc_type[3], auth_type[3], key_type[3];
		int auto_flag = 0;

		if((ter_fd = fopen("/tmp/down_tun","w")) == NULL)
		{
			printf("Open /tmp/down_tun failed!\n");
			return -1;
		}
	
		fprintf(ter_fd,"#!/bin/sh\n");

		for(;j< 5;j++)
		{
			int counter = j;
			memset(tunnel_status,0,sizeof(tunnel_status));
			memset(tunnel_name,0,sizeof(tunnel_name));
			memset(local_type,0,sizeof(local_type));
			memset(localgw_conn,0,sizeof(localgw_conn));
			memset(remote_type,0,sizeof(remote_type));
			memset(sg_type,0,sizeof(sg_type));
			memset(enc_type,0,sizeof(enc_type));
			memset(auth_type,0,sizeof(auth_type));
			memset(key_type,0,sizeof(key_type));
			memset(word,0,sizeof(word));
			foreach(word, nvram_safe_get("entry_config"), next) 
			{
				if (counter-- == 0) 
				{
					sscanf(word, "%3[^:]:%40[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^:]:%3[^\n]", tunnel_status,tunnel_name,local_type, localgw_conn,remote_type, sg_type, enc_type, auth_type, key_type);
#if 0
					if(atoi(localgw_conn) == which)
					{
					#ifdef ZB_DEBUG
						printf("tunnel_status=%s, tunnel_name=%s,local_type=%s,localgw_conn=%s,remote_type=%s,sg_type=%s,enc_type=%s,auth_type=%s,key_type=%s\n", tunnel_status,tunnel_name,local_type,localgw_conn,remote_type,sg_type,enc_type,auth_type,key_type);
					#endif 
						num_tunn = j;
					}	
#endif
					break;
				}
			}
	
			if(strcmp(tunnel_name,"") && !strcmp(tunnel_status,"1") && (atoi(localgw_conn) == which))
			{	
				if(!strcmp(key_type,"0"))
				{
					fprintf(ter_fd,"echo\n");
					fprintf(ter_fd,"/bin/ipsec/whack --terminate --name %s\n",tunnel_name);
					fprintf(ter_fd,"echo terminate!\n");
					fprintf(ter_fd,"/bin/ipsec/whack --unroute --name %s\n",tunnel_name);
					fprintf(ter_fd,"echo unroute!\n");
					fprintf(ter_fd,"rm -rf /tmp/ipsecst/%s 2> /dev/null > /dev/null\n",tunnel_name);
					fprintf(ter_fd,"echo rm!\n");
					fprintf(ter_fd,"exit\n");
					//auto_flag = 1;

				}
				else if(!strcmp(key_type,"1"))
				{
					char file_name[20];
					memset(file_name,0,sizeof(file_name));
					sprintf(file_name,"/tmp/deltun_%d 1",j);
					printf("eval %s\n",file_name);
					//eval("/bin/sh",file_name);
					system(file_name);
				}
			}
		}
		fclose(ter_fd);
			
		ret += eval("/bin/sh","/tmp/down_tun");
#if 1	
		{
			char de_file[20];
			memset(de_file,0,sizeof(de_file));
			printf("***which=%d\n",which);
			sprintf(de_file,"/tmp/autode_%d",which);
			ret += eval("/bin/sh",de_file);
			printf("Eval %s end!\n",de_file);
		}
#endif
		{
			FILE *old_fd;

			if((old_fd = fopen("/tmp/vpn-ipt","r")) != NULL)
			{
				if(strcmp(wan_if,"Down"))
				{
					FILE *new_fd;
					char buffer[256];
				
					if((new_fd = fopen("/tmp/new-ipt","w")) == NULL)
					{
						printf("create new-ipt failed!\n");
						return -1;
					}

					memset(buffer,0,sizeof(buffer));
					while(fgets(buffer,256,old_fd))
					{
						if(strstr(buffer,wan_if))
						{
#if 0
							char *p;
							if((p = strstr(buffer,"-I")) != NULL)
							{
								*(p+1) = 'D';
								printf("mangle -D!\n");
								system(buffer);
							}
#endif
						}
						else
						{
							fprintf(new_fd,"%s",buffer);
						}
					}
					fclose(new_fd);
				}
				fclose(old_fd);
			}

			remove("/tmp/vpn-ipt");
			rename("/tmp/new-ipt","/tmp/vpn-ipt");
		}
	
		if(search_status(which) != 0)
		{
			printf("killipsec Error!\n");
			return -1;
		}

	}
#ifdef NOMAD_SUPPORT
        wandown_stopnomad_conn();//add by xiaoqin for clear quickVPN when WAN interface down
        //add by zhs for quickVPN
        stop_pluto();
#endif
	return 0;
}

#ifdef GATEWAY_BEYOND_SUBNET_SUPPORT
int
legal_ip_netmask(char *sip, char *smask, char *dip)
{
	struct in_addr ipaddr, netaddr, netmask;
	int tag;

	inet_aton(nvram_safe_get(sip), &netaddr);
	inet_aton(nvram_safe_get(smask), &netmask);
	inet_aton(dip, &ipaddr);

	netaddr.s_addr &= netmask.s_addr;

	if (netaddr.s_addr != (ipaddr.s_addr & netmask.s_addr))
		tag = FALSE;
	else
		tag = TRUE;

#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): sip=[%s] smask=[%s] dip=[%s] (%s)",__FUNCTION__,nvram_safe_get(sip),nvram_safe_get(smask),dip,tag?"TRUE":"FALSE");
#endif
	
	return tag;
}
#endif
