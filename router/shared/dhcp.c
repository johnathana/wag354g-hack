
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
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>

#include <broadcom.h>
#include "pvc.h"

#define DHCP_MAX_COUNT 254

/* Dump leases in <tr><td>hostname</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
int
ej_dumpleases(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct lease_t lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
	char sigusr1[] = "-XX";
	int ret = 0;
	int count = 0;
	char *ipaddr, mac[20]="", expires_time[50]="";


        /* Write out leases file */
        sprintf(sigusr1, "-%d", SIGUSR1);
        eval("killall", sigusr1, "udhcpd");


	/* Parse leases file */
	if ((fp = fopen("/tmp/udhcpd.leases", "r"))) {
	    while (fread(&lease, sizeof(lease), 1, fp)) {
		strcpy(mac,"");

		for (i = 0; i < 6; i++) {
			sprintf(mac+strlen(mac),"%02X", lease.chaddr[i]);
			if (i != 5) sprintf(mac+strlen(mac),":");
		}
		mac[17] = '\0';
		if(!strcmp(mac,"00:00:00:00:00:00"))
			continue;
		
		addr.s_addr = lease.yiaddr;

		ipaddr = inet_ntoa(addr);

		expires = ntohl(lease.expires);

		strcpy(expires_time,"");
		if (!expires){
			continue;
			strcpy(expires_time,"expired");
		}
		else {
			if (expires > 60*60*24) {
				sprintf(expires_time+strlen(expires_time),"%ld days, ",expires / (60*60*24));
				expires %= 60*60*24;
			}
			if (expires > 60*60) {
				sprintf(expires_time+strlen(expires_time),"%02ld:",expires / (60*60));	// hours
				expires %= 60*60;
			}
			else{
				sprintf(expires_time+strlen(expires_time),"00:");	// no hours
			}
			if (expires > 60) {
				sprintf(expires_time+strlen(expires_time),"%02ld:",expires / 60);	// minutes
				expires %= 60;
			}
			else{
				sprintf(expires_time+strlen(expires_time),"00:");	// no minutes
			}

			sprintf(expires_time+strlen(expires_time),"%02ld:",expires);		// seconds

			expires_time[strlen(expires_time)-1]='\0';
		}
ret += websWrite(wp, "%c'%s','%s','%s','%s','%d'\n",count ? ',' : ' ', 
			!*lease.hostname ? "&nbsp;" : lease.hostname, 
			ipaddr, 
			mac, 
			expires_time, 
			get_single_ip(inet_ntoa(addr),3));
                count++;
	   }
	fclose(fp);
	}

	return ret;
}

/* Delete leases */
int
delete_leases(webs_t wp)
{
        FILE *fp_w;
        char sigusr1[] = "-XX";
        int i;
        int ret = 0;

	if(nvram_match("lan_proto","static"))
		return ret;

	unlink("/tmp/.delete_leases");

        if (!(fp_w = fopen("/tmp/.delete_leases", "w"))) {
                websError(wp, 400, "Write leases error\n");
                return -1;
        }

	for(i = 0 ; i < DHCP_MAX_COUNT ; i ++){
		char name[] = "d_XXX";
		char *value;
                snprintf(name, sizeof(name), "d_%d", i);
                value = websGetVar(wp, name, NULL);
		if(!value)	continue;
		fprintf(fp_w,"%d.%d.%d.%s\n",get_single_ip(nvram_safe_get("lan_ipaddr"),0),
					     get_single_ip(nvram_safe_get("lan_ipaddr"),1),
					     get_single_ip(nvram_safe_get("lan_ipaddr"),2),
					     value);	

	}
	fclose(fp_w);
	
      	sprintf(sigusr1, "-%d", SIGUSR2);
       	eval("killall", sigusr1, "udhcpd");	// call udhcpd to delete ip from lease table

        return ret;
}


void
dhcp_check(webs_t wp, char *value, struct variable *v)
{
	return ;	// The udhcpd can valid lease table when re-load udhcpd.leases.	by honor 2003-08-05
}

int
dhcp_renew(webs_t wp)
{
	int ret;
        char sigusr[] = "-XX";

	sprintf(sigusr, "-%d", SIGUSR1);
	ret = eval("killall", sigusr, "udhcpc");

	return ret;
}

int
dhcp_release(webs_t wp)
{
	int ret;
        char sigusr[] = "-XX";

	sprintf(sigusr, "-%d", SIGUSR2);
	ret = eval("killall", sigusr, "udhcpc");

	return ret;
}

//added by leijun 2004-05-20
#define	MAX_DHCP_RESERVED_IP	10

void change_mac_value(char *src, char *dst, webs_t wp, struct variable *v)
{
	char *tmp_pt;
	if(strcmp(src, "") && strcmp(src,"00:00:00:00:00:00") && strcmp(src,"000000000000"))
	{
		if(strlen(src) == 12)
		{
			tmp_pt = (char *)&dst[0];
			while (*src) 
			{
				strncpy(tmp_pt, src, 2);
				tmp_pt += 2;
				*tmp_pt ++ = ':';
	               src += 2;
	        }
			dst[17] = '\0';
		}
		else if(strlen(src) == 17)
		{
			strcpy(dst, src);
		}
		if (!valid_hwaddr(wp, dst, v))
		{
			strcpy(dst, "");
		}
	}
}

void validate_dhcp_reservedIp(webs_t wp, char *value, struct variable *v)
{
	//dhcp_mac_enable:dhcp_mac_ip:dhcp_mac
	int i, index = 0;
	char tmp_buff[1024];
	
	char tmp_type[10] = "mac";
	
	for (i = 0; i < MAX_DHCP_RESERVED_IP*2; i ++) 
	{
		char tmp_mac_host[80] = "";
		char tmp_enable_name[32], tmp_ip_name[32], tmp_name[32];
		char *t_name_val = "", *t_ip_val = "", *t_enable_val = "";
		int i_enable = 0, i_ip_addr = 0;
		
		if (i == MAX_DHCP_RESERVED_IP) 
		{
			strcpy(tmp_type, "host");
			index = 0;
		}
	
		sprintf(tmp_name, "dhcp_%s%d", tmp_type, index);
		sprintf(tmp_ip_name, "dhcp_%s_ip%d", tmp_type, index);
		sprintf(tmp_enable_name, "dhcp_%s_enable%d", tmp_type, index);
		
		t_name_val = websGetVar(wp, tmp_name, "");
		t_ip_val = websGetVar(wp, tmp_ip_name, "0");
		t_enable_val = websGetVar(wp, tmp_enable_name, "off");
//		printf("%s-%s-%s\n", t_name_val, t_ip_val, t_enable_val);
		/////valid enalbe;
		if (!strcmp(t_enable_val, "off")) i_enable = 0;
		else i_enable = 1;
		
		i_ip_addr = atoi(t_ip_val);
		///////////valid enable ip;
		if (!((i_ip_addr >= 0) && (i_ip_addr <= 254))) t_ip_val = "";
		/////valid mac or hostname
		if (i < MAX_DHCP_RESERVED_IP) //mac addr
		{
			change_mac_value(t_name_val, tmp_mac_host, wp, v);
		}
		else { //hostname
			char *tmp_ch = t_name_val;
			int flag = 0;
			int len = strlen(t_name_val) - 1;
			while(1)
			{
				if (t_name_val[len] == ' ') 
				{
					len --;
				}else break;
			}
			t_name_val[len + 1] = '\0';
			while(*tmp_ch)
			{
				if (((*tmp_ch >= '0') && (*tmp_ch <= '9')) || 
					((*tmp_ch >= 'a') && (*tmp_ch <= 'z')) || 
					((*tmp_ch >= 'A') && (*tmp_ch <= 'Z')) || (*tmp_ch == '-') || (*tmp_ch == '_'))
				{
					if ((*tmp_ch >= '0') && (*tmp_ch <= '9')) flag ++;
					tmp_ch ++;
				}else 
				{
					flag = -1;
					break;
				}
			}
			if (flag >= strlen(t_name_val)) flag = -1;
			if (flag == -1)t_name_val = "";
			strcpy(tmp_mac_host, t_name_val);
		}
	
		if (i == 0) sprintf(tmp_buff, "%d:%s:%s", i_enable, t_ip_val, tmp_mac_host);
		else sprintf(tmp_buff, "%s %d:%s:%s", tmp_buff, i_enable, t_ip_val, tmp_mac_host);
		index ++;
	}
	printf("%s\n", tmp_buff);
	nvram_set("dhcp_reserved_ip", tmp_buff);
	
	//share wan ip setting
	{
		char *tmp_wanip_mac = NULL, *tmp_wanip_if = NULL, *tmp_wanip_lease = NULL, *tmp_wanip = NULL;
		char tmp_mac_value[80] = "";	
		unsigned int tmp_index = 0;
		tmp_wanip = websGetVar(wp, "share_wanip", "");
		tmp_wanip_mac = websGetVar(wp, "dhcp_mac10", "");
		tmp_wanip_if = websGetVar(wp, "share_wanip_if", "");
		//share wan ip lease time
		tmp_wanip_lease = websGetVar(wp, "share_wanip_lease", "10");
//		printf("tmp_wanip_mac = %s\n", tmp_wanip_mac);
		if ((tmp_wanip_if) && (tmp_wanip_if != ""))
		{
			tmp_index = atoi(tmp_wanip_if);
		}
		change_mac_value(tmp_wanip_mac, tmp_mac_value, wp, v);
		sprintf(tmp_buff, "%s$%d$%s", tmp_mac_value, tmp_index, tmp_wanip_lease);
		printf("content = %s\n", tmp_buff);
		nvram_set("share_wanip_content", tmp_buff);
		if (atoi(tmp_wanip) == 1) nvram_set("dmz_enable", "0");
	}
}

#define		PPP_FLAGS		0x80

int ej_connection_talbe(webs_t wp, int active_index)
{
	int i, selectable, ret = 0, which_to_select, has_match = 0;
	char word[PVC_CONNECTIONS][6], readbuf[60];
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_IFNAME);
	        return 0;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);  

	for(i=0 ; i<PVC_CONNECTIONS; i++)
	{
		selectable = 0;
		which_to_select = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				if(check_ppp_link(i) && check_ppp_proc(i))
				{
					selectable = 1;
					which_to_select = i | PPP_FLAGS;   //ppp flag
				}
			}
			else if(strstr(word[i], "nas"))
			{
				selectable = 1;
				which_to_select = i;
			}
		#ifdef CLIP_SUPPORT
			else if(strstr(word[i], "atm"))
			{
				selectable = 1;
				which_to_select = i;
			}
		#endif
		}
		if(selectable)
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>PVC %d(%s)</option>\n", which_to_select, (which_to_select == active_index)?"selected" : "", i+1, word[i]);
	}

#ifdef MPPPOE_SUPPORT
	memset(word, 0, sizeof(word));
	if(!file_to_buf(WAN_MPPPOE_IFNAME, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", WAN_MPPPOE_IFNAME);
	        return 0;
	}
	sscanf(readbuf, "%s %s %s %s %s %s %s %s", word[0], word[1], word[2], word[3], word[4], word[5], word[6], word[7]);  
	
	for(i=0 ; i<PVC_CONNECTIONS; i++)
	{
		selectable = 0;
		which_to_select = 0;
		if(strcmp(word[i], "Down"))
		{
			if(strstr(word[i], "ppp"))
			{
				if(check_ppp_link(i + 8) && check_ppp_proc(i + 8))
				{
					selectable = 1;
					which_to_select = (1 << 3) | i | PPP_FLAGS;   //ppp flag
				}
			}
		}
		#ifdef IPPPOE_SUPPORT
		else
		{
			char *next;
			int which = i;
			memset(word[i], 0, sizeof(word[i]));
			if(!file_to_buf(WAN_IPPPOE_IFNAME, readbuf, sizeof(readbuf)))
			{
				printf("no buf in %s!\n", WAN_IPPPOE_IFNAME);
	       		 	return 0;
			}
			memset(word[i], 0, sizeof(word[i]));
			foreach(word[i], readbuf, next)
			{
				if(which -- == 0)
					break;
				memset(word[i], 0, sizeof(word[i]));
			}
			if(strstr(word[i], "ppp"))
			{
				if(check_ppp_link(i + 8) && check_ppp_proc(i + 8))
				{
					selectable = 1;
					which_to_select = (1 << 4) | i | PPP_FLAGS;   //ppp flag
				}
			}
		}
		#endif		
		if(selectable)
			ret += websWrite(wp,"\t\t<option value=\"%d\" %s>%s %d(%s)</option>\n", which_to_select, (which_to_select == active_index)?"selected" : "", (which_to_select & (1<<3))?"MULTIPLE PPPOE":"IP PPPOE", i+1, word[i]);
	}
#endif	
	return ret;
}

int ej_dhcp_reserved_ip(int eid, webs_t wp, int argc, char_t **argv)
{
	char *type, tmp_name[32], tmp_ip[32], tmp_enable[32], tmp_lan_ip[32];
	char tmp_enable_value[10] = " ";

	char word[256];
	char *next, *wordlist;	
	int i = 0, host_flag = 0;
	
	if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
    	websError(wp, 400, "Insufficient args\n");
        return -1;
	}

	sprintf(tmp_lan_ip, "%d.%d.%d.", get_single_ip(nvram_safe_get("lan_ipaddr"),0),
					       get_single_ip(nvram_safe_get("lan_ipaddr"),1),
					       get_single_ip(nvram_safe_get("lan_ipaddr"),2));

	wordlist = nvram_safe_get("dhcp_reserved_ip");
//	printf("ip = %s\n", wordlist);
	if (!strcmp(type, "host"))
	{
		i = 0;
		host_flag = 1;
		if (wordlist)
		{
		foreach(word, wordlist, next)
		{
			if (i >= MAX_DHCP_RESERVED_IP) break;
			else i ++;
			wordlist = next;
		}
		}
	}
	if ((!strcmp(type, "host")) || (!strcmp(type, "mac")))
	{
	for (i = 0; i < MAX_DHCP_RESERVED_IP; i ++)
	{
		char *enable = NULL, *ip = NULL, *name = NULL;
		//get the values from dhcp_reserved_ip

		if (wordlist)
		{
		foreach(word, wordlist, next)
		{
			wordlist = next;
			ip = word;
			enable = strsep(&ip, ":");
			
			if ((enable) && (ip))
			{
				name = ip;
				ip = strsep(&name, ":");
			}
			if (!enable || !ip || !name)
			{
				enable = NULL;
				ip = NULL;
				name = NULL;
			}
			break;
		}
		}
		if (enable)
		{
			if (atoi(enable)) strcpy(tmp_enable_value, "checked");
			else strcpy(tmp_enable_value, "\"\"");
		}else strcpy(tmp_enable_value, "\"\"");
		
		sprintf(tmp_name, "dhcp_%s%d", type, i);
		sprintf(tmp_ip, "dhcp_%s_ip%d", type, i);
		sprintf(tmp_enable, "dhcp_%s_enable%d", type, i);

		websWrite(wp, "<TR align=center>\n");
		websWrite(wp, "\t<TD width=24 height=15><FONT style=\"FONT-SIZE: 10pt\" face=Arial><B>%02d</B></FONT></TD>\n", i + 1);
		websWrite(wp, "\t<TD width=132 height=25><FONT style=\"FONT-SIZE: 10pt\" face=Arial><INPUT maxLength=%d size=17 name=%s value=\"%s\" onblur=%s(this.form,this,this.form.%s) class=num></TD>\n"
				, (host_flag)?64:17, tmp_name, name? name : "", (host_flag)?"valid_host_name" : "valid_macs_all", tmp_enable);
		websWrite(wp, "\t<TD width=178 height=25>%s<input name=%s size=3 maxlength=3 value=\"%s\" onBlur=valid_my_ip(this.form,this,this.form.%s) class=num></TD>"
				, tmp_lan_ip, tmp_ip, ip? ip : "", tmp_enable);
		websWrite(wp, "\t<TD width=60 height=25><input type=checkbox name=%s value=1 %s onclick=valid_check(this.form.%s,this.form.%s,this)></TD></TR>",
				tmp_enable, tmp_enable_value, tmp_name, tmp_ip);
	}
	}else if (!strcmp(type, "share"))
	{
		int select_index = 0, tmp_lease;
		char tmp_mac_v[20] = "", *tmp_pt = NULL;
		tmp_pt = nvram_safe_get("share_wanip_content");
		if ((strcmp(tmp_pt, "share_wanip_content")) && (tmp_pt))
		{
			char *tmp_ch;
			tmp_ch = strchr(tmp_pt, '$');
			if (tmp_ch) 
			{
				strncpy(tmp_mac_v, tmp_pt, (int)(tmp_ch - tmp_pt));
				tmp_ch ++;
				sscanf(tmp_ch, "%d$%d", &select_index, &tmp_lease);
//				select_index = atoi(tmp_ch);
			}
		}
		printf("mac = %s, index = %d\n", tmp_mac_v, select_index);
		websWrite(wp, "<TR align=center>\n");
//		websWrite(wp, "\t<TD width=24 height=15><FONT style=\"FONT-SIZE: 10pt\" face=Arial><B>01</B></FONT></TD>\n");
		
		websWrite(wp, "\t<TD width=126 height=25><FONT style=\"FONT-SIZE: 10pt\" face=Arial><INPUT maxLength=17 size=17 name=dhcp_mac10 value=\"%s\" onblur=valid_sharewanip_mac(this.form,this) class=num></TD>\n",tmp_mac_v);
    	websWrite(wp, "\t<TD width=172 height=25><FONT style=\"FONT-SIZE: 10pt\" face=Arial><SELECT name=share_wanip_if onChange=\"\">\n");
    	websWrite(wp,"\t\t<option value=\"255\" %s>Auto Search</option>\n", (select_index == 255)?"selected" : "");    	
    	ej_connection_talbe(wp, select_index);
    	websWrite(wp, "</SELECT></TD>\n");
    	
    	websWrite(wp, "\t<TD width=100 height=25><FONT style=\"FONT-SIZE: 10pt\" face=Arial><INPUT maxLength=4 size=4 name=share_wanip_lease value=%d onBlur=valid_range(this,0,9999,\"Share-WAN-IP-Lease-Time\") class=num>&nbsp;minutes</TD></TR>\n",
    			tmp_lease);
	}
}


#ifdef DNS_ENTRY_SUPPORT

#define 		MAX_ENTRY		254
#define		DNS_ENTRY_FILE	"/tmp/.dns_entry"

struct dns_entry{
	unsigned char domainname[254];
	//unsigned char mac[6];
	unsigned char ipaddr[16];
};

static struct dns_entry   dns_entry_list[MAX_ENTRY];
static int dns_entry_count;


int ej_dns_entry_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int i;
	char *type;
	char buff[256];
	FILE *file;
	char *ptr;
	char *domainname, *ipaddr;
	int ret;
	char radio[510];
	
        if (ejArgs(argc, argv, "%s", &type) < 1)
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }

	file = fopen(DNS_ENTRY_FILE, "r");
	if (file == NULL)
		return -1;
	memset((void *)&dns_entry_list[0], 0, sizeof(dns_entry_list));
	dns_entry_count = 0;
	while(fgets(buff, sizeof(buff), file))
	{
		ptr = strchr(buff, '\n');
		if (ptr != NULL)
			*ptr = '\0';
		domainname = ipaddr = buff;
		strsep(&ipaddr, "=");
		strcpy(dns_entry_list[dns_entry_count].domainname, domainname);
		strcpy(dns_entry_list[dns_entry_count].ipaddr, ipaddr);
		dns_entry_count++;
	}
	fclose(file);
	
	for (i = 0; i < dns_entry_count; i++)
	{
		memset((void *)radio, 0, sizeof(radio));
		if (i == 0)
			ret = sprintf(radio, "<input type=radio disabled=true name=dns_entry_select value=0 \
				onclick=\"SelEntry\(this.form, this.form.domainname%d, this.form.dnsip%d)\">",
					i, i);
		else
			ret = sprintf(radio, "<input type=radio  name=dns_entry_select value=0 \
				onclick=\"SelEntry\(this.form, this.form.domainname%d, this.form.dnsip%d)\">",
					i, i);
		radio[ret] = '\0';
		ret = websWrite(wp, "\
<TR>\n\
 <TD class=headrow align=middle height=25 width=132>\
  <B><font style=\"FONT-FAMILY: Arial; FONT-SIZE: 10pt\">\
   %s</font></B></TD>\n\
 <TD class=headrow align=middle height=25 width=178>\
  <B><font style=\"FONT-FAMILY: Arial; FONT-SIZE: 10pt\">\
   %s</font></B></TD>\n\
 <TD class=headrow align=middle height=25 width=60>\
  <B><font style=\"FONT-FAMILY: Arial; FONT-SIZE: 10pt\">\
  %s</font></B></TD>\n\
</TR>\n", 
	dns_entry_list[i].domainname, dns_entry_list[i].ipaddr, 
		radio);
		ret = websWrite(wp, "<input type=hidden name=domainname%d value=%s>\n",
					i, dns_entry_list[i].domainname);
		ret = websWrite(wp, "<input type=hidden name=dnsip%d value=%s>\n",
					i, dns_entry_list[i].ipaddr);
	}
	return 0;
	
}

int
add_dns_entry(webs_t wp)
{
	char buf[65535] = "";
	char entry[65535];
	int flag = 0;
	char *domainname;
	
	char *ipaddr;
	char var[124], *next;
	char *value, *name;
	FILE *file;
	
	domainname = websGetVar(wp, "add_domainname", NULL);	
	ipaddr = websGetVar(wp, "resolve_ip", NULL);


	memset((void *)buf, 0, sizeof(buf));
	memset((void *)entry, 0, sizeof(entry));
	strcpy(buf, nvram_get("dns_entry_list"));
	foreach(var, buf, next)
	{
		value = name = var;
		strsep(&value, "#");
		if (!strcmp(value, ipaddr))
		{
			strcat(entry, domainname);
			strcat(entry, "#");
			strcat(entry, ipaddr);
			flag = 1;
		}
		else
		{
			strcat(entry, name);
			strcat(entry, "#");
			strcat(entry, value);
		}
		strcat(entry, " ");
	}
	if (flag == 0)
	{
		strcat(entry, domainname);
		strcat(entry, "#");
		strcat(entry, ipaddr);
	}
	
	nvram_set("dns_entry_list", entry);
	file = fopen("/var/run/dnsmasq.pid", "r");
	if (file != NULL)
	{
		int pid;
		memset((void *)var, 0, sizeof(var));
		fgets(var, sizeof(var), file);
		pid = atoi(var);
	printf("ADD DNS ENTRY & SEND SIGALRM TO DNSMASQ\n");	
		kill(pid, SIGALRM);
		fclose(file);
	}
	nvram_commit();
}


int delete_dns_entry(webs_t wp)
{
	char buf[65535] = "";
	char entry[65535];
	int flag = 0;
	char *domainname, *ipaddr;
	int i;
	char var[124], *next;
	char *value, *name;
	FILE *file;
	char line[20], *ptr;
	
	domainname = websGetVar(wp, "add_domainname", NULL);	
	ipaddr = websGetVar(wp, "resolve_ip", NULL);

	

	memset((void *)buf, 0, sizeof(buf));
	memset((void *)entry, 0, sizeof(entry));
	strcpy(buf, nvram_get("dns_entry_list"));
	foreach(var, buf, next)
	{
		value = name = var;
		strsep(&value, "#");
		if (!strcmp(value, ipaddr))
		{
			  //this entry is added or modifed, so delete from dns_entry_list variable
			continue;
		}
		else
		{
			strcat(entry, name);
			strcat(entry, "#");
			strcat(entry, value);
		}
		strcat(entry, " ");
	}
	
	{
		flag = 0;
		file = fopen("/tmp/.delete_dns_entry", "r");
		if (file != NULL)
		{
			while(fgets(line, sizeof(line), file))
			{
				ptr = strchr(line, '\n');
				if (ptr != NULL)
					*ptr = '\0';
	
				if (!strcmp(line, ipaddr))
				{
					flag = 1;
					break;
				}
				memset((void *)line, 0, sizeof(line));
			}
			
			fclose(file);
		}
		file = fopen("/tmp/.delete_dns_entry", "a");
		if (file != NULL)
		{
			if (flag != 1)
			{
					memset((void *)var, 0, sizeof(var));
					sprintf(var, "%s\n", ipaddr);
					fputs(var, file);
			}
		}
		fclose(file);
	}
	
	nvram_set("dns_entry_list", entry);
	file = fopen("/var/run/dnsmasq.pid", "r");
	if (file != NULL)
	{
		int pid;
		memset((void *)var, 0, sizeof(var));
		fgets(var, sizeof(var), file);
		pid = atoi(var);
		
		kill(pid, SIGALRM);
		fclose(file);
	}
	
	nvram_commit();
	
}

#endif

int ej_dns_entry_support(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
#ifdef DNS_ENTRY_SUPPORT 
	if (count == 1)
		return websWrite(wp, "%s", "");
	if (count == 2)
		return websWrite(wp, "%s", "");
#else
	if (count == 1)
		return websWrite(wp, "%s", "<!--");
	if (count == 2)
		return websWrite(wp, "%s", "-->");
#endif
}
