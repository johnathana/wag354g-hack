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

#include <broadcom.h>
#include <cymac.h>
#ifdef HTTPS_SUPPORT
void validate_authproto(webs_t wp, char *value, struct variable *v)
{
        int i, error=0;
        char buf[100] = "";

        char *adv_select = "authprotocol";
        char *select;

        select = websGetVar(wp, adv_select, NULL);
        if(select)
         {
          if(!strcmp(select, "0"))
          {
             nvram_set("authprotocol","MD5");
           }
          else if(!strcmp(select,"1"))
           {
             nvram_set("authprotocol","SHA");
           }
       }

}
//Lai add 2005-07-04
void validate_privproto(webs_t wp, char *value, struct variable *v)
{
        int i, error = 0;
        char buf[100]= "";
        char *adv_select = "privprotocol";
        char *select;
                                                                                
        select = websGetVar (wp, adv_select, NULL);
        if (select)
        {
                if (!strcmp (select, "0"))
                {
                        nvram_set ("privprotocol", "None");
                        printf ("This is None\n");
                }
                else if (!strcmp (select, "1"))
                {
                        nvram_set ("privprotocol", "DES");
                        printf ("This is DES\n");
                }
                else
                {
                }
        }
}

//Lai add 2005-07-04
int ej_set_authproto(int eid, webs_t wp, int argc, char_t **argv)
{
        char *arg;
        char *temp="0";
                                                                                
        int count;
        int ret=0;
        
        if(ejArgs(argc, argv, "%s %d",&arg, &count) <2 )
        {
                websError(wp, 400, "Insufficient args\n");
                printf("Insufficient args!!!!\n");
                return -1;
        }
                                                                                
        if(!strcmp(arg,"authprotocol"))
        {
                if(gozila_action )
                {
                        temp = websGetVar(wp, "authprotocol", "0");
                        if(((count == 0) && (!strcmp(temp, "0"))) || ((count ==
1) && (!strcmp(temp, "1"))))
                                return ret=websWrite(wp, "selected");
                }
                else
                {
                        char *authp=nvram_get("authprotocol");
                        if(!strcmp(authp,"MD5"))
                                temp="0";
                        else if (!strcmp(authp,"SHA"))
                                temp="1";
                        if(((count == 0) && (!strcmp(temp, "0"))) || ((count ==
1) && (!strcmp(temp, "1"))))
                                return ret=websWrite(wp, "selected");
                }
        }
                                                                                
        if(!strcmp(arg,"privprotocol"))
        {
                if(gozila_action )
                {
                        temp = websGetVar(wp, "privprotocol", "0");
                        if(((count == 0) && (!strcmp(temp, "0"))) || ((count ==
1) && (!strcmp(temp, "1"))))
                                return ret=websWrite(wp, "selected");
                }
                else
                {
                        char *privp=nvram_get("privprotocol");;
                        if(!strcmp(privp,"None"))
                                temp="0";
                        else if (!strcmp(privp,"DES"))
                                temp="1";
                        if(((count == 0) && (!strcmp(temp, "0"))) || ((count ==
1) && (!strcmp(temp, "1"))))
                                return ret=websWrite(wp, "selected");
                }
        }
        return 0;
}
#endif       

//Lai add 2005-07-08
void validate_snmpv3_support (webs_t wp, char *value, struct variable *v)
{
        int i, error = 0;
        char buf[100]= "";
        char *adv_select = "snmpv3_support";
        char *select;
                                                                                
        select = websGetVar (wp, adv_select, NULL);
        if (select)
        {
                if (!strcmp (select, "0"))
                {
                        nvram_set ("snmpv3_support", "Disable");
                }
                else if (!strcmp (select, "1"))
                {
                        nvram_set ("snmpv3_support", "Enable");
                }
                else
                {
                }
        }
}


int ej_show_snmp_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *snmp_ip_addr_local;
	//if(!gozila_action || selchanged)
	if(!gozila_action )
	{
//		selchanged = 0;
		snmp_ip_addr_local = nvram_get("snmp_ip_limit");
	}
	else
		snmp_ip_addr_local = websGetVar(wp, "snmp_ip_addr", "0");
		

	//if(snmp_ip_addr_local[0] == '0') do_ej("snmp_all.asp",wp);
	if(snmp_ip_addr_local[0] == '1') 
		do_ej("snmp_ipaddr.asp",wp);
	if(snmp_ip_addr_local[0] == '2')
		do_ej("snmp_iprange.asp",wp);
	
	return ret;
}

//Lai modify 2005-07-08
int ej_snmp_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg, *snmp_ip_addr_l, *ptr, *temp;
	int count, i, ret;

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if(!strcmp(arg, "devname"))
	{
		//printf("csh1:\n" );
		if(gozila_action )
		{
		//printf("csh2:\n" );
			return ret=websWrite(wp, websGetVar(wp, "devname", NULL));
		}
		else
		{
		//printf("csh3:\n" );
			return ret=websWrite(wp, nvram_get("devname"));
		}

	}

	if(!strcmp(arg, "snmp_enable"))
	{
		if(gozila_action )
		{
			char *snmp_enable_l = websGetVar(wp, "snmp_enable", NULL);
			if(((count == 0) && !strcmp(snmp_enable_l, "0")) || ((count == 1) && !strcmp(snmp_enable_l, "1")))
			return ret=websWrite(wp, "checked");
		}
		else
		{
			char *snmp_enable_l = nvram_get("snmp_enable");
			if(((count == 0) && !strcmp(snmp_enable_l, "0")) || ((count == 1) && !strcmp(snmp_enable_l, "1")))
			return ret=websWrite(wp, "checked");
		}
	}
	//Lai add 2005-07-08
	if(!strcmp(arg, "snmpv3_support"))
	{
		if(gozila_action )
		{
			char *snmpv3_support = websGetVar(wp, "snmpv3_support", NULL);
			if(((count == 0) && !strcmp(snmpv3_support, "0")) || ((count == 1) && !strcmp(snmpv3_support, "1")))
			return ret=websWrite(wp, "checked");
		}
		else
		{
			char *snmpv3_support = nvram_get("snmpv3_support");
			if (!strcmp (snmpv3_support, "Disable"))
				temp = "0";
			else if (!strcmp (snmpv3_support, "Enable"))
				temp = "1";
			if(((count == 0) && !strcmp(temp, "0")) || ((count == 1) && !strcmp(temp, "1")))
			return ret=websWrite(wp, "checked");
		}
	}

	if(!strcmp(arg, "snmp_ip_addr"))
	{
		if(gozila_action )
		{
			snmp_ip_addr_l= websGetVar(wp, "snmp_ip_addr", "0");
			if(((count == 0) && (!strcmp(snmp_ip_addr_l, "0"))) || ((count == 1) && (!strcmp(snmp_ip_addr_l, "1"))) || ((count == 2) && (!strcmp(snmp_ip_addr_l, "2"))) ) 
				return ret=websWrite(wp, "selected");
		}
		else
		{
			char *snmp_ip_addr_r = nvram_get("snmp_ip_limit");
			if(((count == 0) && (snmp_ip_addr_r[0] == '0')) || ((count == 1) && (snmp_ip_addr_r[0] == '1')) || ((count == 2) && (snmp_ip_addr_r[0] == '2')) )
				return ret=websWrite(wp, "selected");
		}

	}
	if(!strcmp(arg,"ipaddr"))
	{
		if(gozila_action )
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipaddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipaddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipaddr2",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipaddr3",""));
			}
		}
		else
		{
			char snmp_ip_addr_r[100];
			char *addr;
			addr = NULL;
			
			strcpy(snmp_ip_addr_r, nvram_get("snmp_ip_limit"));
			if(snmp_ip_addr_r && snmp_ip_addr_r[0]=='1')
			{
				ptr = strchr(snmp_ip_addr_r, ':');
				if(ptr) *ptr = '.';
				sprintf(snmp_ip_addr_r, "%s%s", snmp_ip_addr_r, ".");
				ptr = strchr(snmp_ip_addr_r, '.') ;
				if(ptr) ptr++;
				for(i=1; ptr && i<=count; i++)
				{
					ptr = strchr(ptr, '.');
					if(ptr) ptr++;
				}
				addr = ptr;
				ptr = strchr(ptr, '.');
				if(ptr) *ptr = '\0';
			}
			return 	websWrite(wp,"%s",(addr==NULL?"":addr));

		}
	}

	if(!strcmp(arg,"iprange"))
	{
		if(gozila_action )
		{
			switch(count)
			{
				case 0:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipStartAddr0",""));
				case 1:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipStartAddr1",""));
				case 2:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipStartAddr2",""));
				case 3:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipStartAddr3",""));
				case 4:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipEndAddr0",""));
				case 5:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipEndAddr1",""));
				case 6:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipEndAddr2",""));
				case 7:
					return 	websWrite(wp,"%s",websGetVar(wp,"snmp_ipEndAddr3",""));

			}
		}
		else
		{
			char snmp_ip_addr_r[100];
			char *addr;
			addr = NULL;
			
			strcpy(snmp_ip_addr_r, nvram_get("snmp_ip_limit"));
			if(snmp_ip_addr_r && snmp_ip_addr_r[0]=='2')
			{
				sprintf(snmp_ip_addr_r, "%s%s", snmp_ip_addr_r, ".");
				ptr = strchr(snmp_ip_addr_r, ':');
				if(ptr) *ptr = '.';
				ptr = strchr(snmp_ip_addr_r, '-');
				if(ptr) *ptr = '.';

				ptr = strchr(snmp_ip_addr_r, '.') ;
				if(ptr) ptr++;
				for(i=1; ptr && i<=count; i++)
				{
					ptr = strchr(ptr, '.');
					if(ptr) ptr++;
				}
				addr = ptr;
				ptr = strchr(ptr, '.');
				if(ptr) *ptr = '\0';
			}
			//printf("addr=%s\n", addr);
			return 	websWrite(wp,"%s",(addr==NULL?"":addr));

		}
	}
	return 0;
}

void
validate_snmp_config(webs_t wp, char *value, struct variable *v)
{
	int i, error=0;
	char buf[100] = "";

	char *adv_select = "snmp_ip_addr";
	char *adv_ipaddr[4]={"snmp_ipaddr0", "snmp_ipaddr1", "snmp_ipaddr2", "snmp_ipaddr3"};
	char *adv_iprange[8]={"snmp_ipStartAddr0", "snmp_ipStartAddr1", "snmp_ipStartAddr2", "snmp_ipStartAddr3", "snmp_ipEndAddr0", "snmp_ipEndAddr1", "snmp_ipEndAddr2", "snmp_ipEndAddr3"};
	char *select, *ipaddr[4], *iprange[8];

	select = websGetVar(wp, adv_select, NULL);
	if(select)
	{
		if(!strcmp(select, "1")) 
		{
			//get ip 
			for(i=0; i<4; i++)
			{
				ipaddr[i] = websGetVar(wp, adv_ipaddr[i], "0");
				if(!ipaddr[i]) ipaddr[i] = "0";
			}
			//save ip in buf
			sprintf(buf, "1:%s.%s.%s.%s", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
		}
		else if(!strcmp(select, "2"))
		{
			//get start and end ip
			for(i=0; i<8; i++)
			{
				iprange[i] = websGetVar(wp, adv_iprange[i], "0");
				if(!ipaddr[i]) ipaddr[i] = "0";
			}

			//save start and end ip in buf
			sprintf(buf, "2:%s.%s.%s.%s-%s.%s.%s.%s", iprange[0], iprange[1], iprange[2], iprange[3], iprange[4], iprange[5], iprange[6], iprange[7]);
		}
		else
			sprintf(buf, "%s", "0");//seve '0' in buf
	
		printf("\ncsh**********buf=%s\n", buf);
		if(!error)
			nvram_set(v->name, buf);
	}
}
