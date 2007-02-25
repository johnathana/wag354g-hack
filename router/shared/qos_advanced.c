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
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>

#include "broadcom.h"

#if 0
#define ZB_DEBUG
#define ZB_PRINTF printf
#else
#define ZB_PRINTF(format, args...)
#endif
#define QOS_NUM 10


int ej_qos_config_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	char *arg;
	int count;
	int ret = 0;
	int which,i;
	char word[100];

	memset(word,0,sizeof(word));

	if(ejArgs(argc, argv, "%s %d", &arg, &count) < 2)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
		
	if(!strcmp(arg,"qosSW"))
	{
		char *qosSW = nvram_safe_get("qosSW");	
		if(((count == 1) && (!strcmp(qosSW, "1"))) || ((count == 0) && (!strcmp(qosSW, "0"))))
			return websWrite(wp, "checked");
		
	}
	else if(!strcmp(arg,"qos_frg"))
	{
		char *qos_frg = nvram_safe_get("qos_frg");	
		ZB_PRINTF("qos_frg=%s\n",qos_frg);
		if(!strcmp(qos_frg, "1"))
			return websWrite(wp, "checked");
		else
			return websWrite(wp, "");
		
	}
	else if(!strcmp(arg,"qos_frgsize"))
	{
		return websWrite(wp, nvram_safe_get("qos_frgsize"));
		
	}
	else
	{
	for(i = 0; i < QOS_NUM; i++)
	{
		char cfbuf[10],*next;

		memset(cfbuf,0,sizeof(cfbuf));
		sprintf(cfbuf,"basic_qos%d",i);	
		which = i;
		foreach(word, nvram_safe_get(cfbuf), next)
		{
			char dstip[16], dstmask[16], srcip[16], srcmask[16], srcmac[18], dstmac[18], pro[2], srcport[6], dstport[6], fqdn[30],ethtype[6],vid[6],packlen[10];
			int j;

			if(which -- == 0)
			{
				memset(dstip, 0, sizeof(dstip));
				memset(dstmask, 0, sizeof(dstmask));
				memset(srcip, 0, sizeof(srcip));
				memset(srcmask, 0, sizeof(srcmask));
				memset(srcmac, 0, sizeof(srcmac));
				memset(dstmac, 0, sizeof(dstmac));
				memset(pro, 0, sizeof(pro));
				memset(srcport, 0, sizeof(srcport));
				memset(dstport, 0, sizeof(dstport));
				memset(fqdn, 0, sizeof(fqdn));
				memset(ethtype, 0, sizeof(ethtype));
				memset(vid, 0, sizeof(vid));
				memset(packlen, 0, sizeof(packlen));
				sscanf(word, "%16[^:]:%16[^:]:%16[^:]:%16[^:]:%18[^:]:%18[^:]:%2[^:]:%6[^:]:%6[^:]:%30[^:]:%6[^:]:%6[^:]:%10[^\n]", dstip, dstmask, srcip, srcmask, srcmac, dstmac, pro, srcport, dstport, fqdn, ethtype, vid, packlen);
			}

			for(j=0; j<4; j++)
			{
				ret += websWrite(wp, "\tdocument.basicqos.dstip_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstip, j));
				ret += websWrite(wp, "\tdocument.basicqos.dstmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstmask, j));
				ret += websWrite(wp, "\tdocument.basicqos.srcip_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcip, j));
				ret += websWrite(wp, "\tdocument.basicqos.srcmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcmask, j));
			}
			ret += websWrite(wp, "\tdocument.basicqos.srcmac_%d.value=\"%s\";\n", i, srcmac);
			ret += websWrite(wp, "\tdocument.basicqos.srcmac_%d.value=\"%s\";\n", i, dstmac);
			ret += websWrite(wp, "\tdocument.basicqos.pro_%d[%d].selected=true;\n", i, atoi(pro));
			ret += websWrite(wp, "\tdocument.basicqos.srcport_%d.value=\"%s\";\n", i, srcport);
			ret += websWrite(wp, "\tdocument.basicqos.dstport_%d.value=\"%s\";\n", i, dstport);
			ret += websWrite(wp, "\tdocument.basicqos.fqdn_%d.value=\"%s\";\n", i, fqdn);
			ret += websWrite(wp, "\tif(document.basicqos.pro_%d.selectedIndex==0)\n", i);
			ret += websWrite(wp, "\t{\n\t\tdocument.basicqos.dstport_%d.disabled=true;\n", i);
			ret += websWrite(wp, "\t\tdocument.basicqos.srcport_%d.disabled=true;\n\t}\n", i);
			ret += websWrite(wp, "\telse\n", i);
			ret += websWrite(wp, "\t{\n\t\tdocument.basicqos.dstport_%d.disabled=false;\n", i);		
			ret += websWrite(wp, "\t\tdocument.basicqos.srcport_%d.disabled=false;\n\t}\n", i);		
			ret += websWrite(wp, "\tdocument.basicqos.ethtype_%d.value=\"%s\";\n", i, ethtype);
			ret += websWrite(wp, "\tdocument.basicqos.vid_%d.value=\"%s\";\n", i, vid);
		}
		return ret;
	}
	}
	return 0;
}

int ej_dump_verify_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, j, ret = 0;

	for(i = 0; i < QOS_NUM; i++)
	{
		for(j = 0; j < 4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.dstip_%d_%d,0,253,\"DstIp\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		ret += websWrite(wp, "\tif((our_range(document.basicqos.dstip_%d_1,1,253) || our_range(document.basicqos.dstip_%d_2,1,253) || our_range(document.basicqos.dstip_%d_3,1,253)) && our_range(document.basicqos.dstip_%d_0,0,0))\n\t{\n", i, i, i, i);
		ret += websWrite(wp, "\t\talert(\"First ip should > 0.\");\n");
		ret += websWrite(wp, "\t\tdocument.basicqos.dstip_%d_0.focus();\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.dstmask_%d_%d,0,255,\"DstNetmask\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		ret += websWrite(wp, "\tif((our_range(document.basicqos.dstmask_%d_1,1,255) || our_range(document.basicqos.dstmask_%d_2,1,255) || our_range(document.basicqos.dstmask_%d_3,1,255) || our_range(document.basicqos.dstmask_%d_0,1,255)) && !valid_mask(document.basicqos, 'document.basicqos.dstmask_%d',BCST_OK))\n\t{\n", i, i, i, i, i);
		ret += websWrite(wp, "\t\tdocument.basicqos.dstmask_%d_0.focus();\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.srcip_%d_%d,0,253,\"SrcIp\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		ret += websWrite(wp, "\tif((our_range(document.basicqos.srcip_%d_1,1,253) || our_range(document.basicqos.srcip_%d_2,1,253) || our_range(document.basicqos.srcip_%d_3,1,253)) && our_range(document.basicqos.srcip_%d_0,0,0))\n\t{\n", i, i, i, i);
		ret += websWrite(wp, "\t\talert(\"First ip should > 0.\");\n");
		ret += websWrite(wp, "\t\tdocument.basicqos.srcip_%d_0.focus();\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		for(j=0; j<4; j++)
		{
			ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.srcmask_%d_%d,0,255,\"SrcNetmask\"))\n\t{\n", i, j);
			ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		}		
		ret += websWrite(wp, "\tif((our_range(document.basicqos.srcmask_%d_1,1,255) || our_range(document.basicqos.srcmask_%d_2,1,255) || our_range(document.basicqos.srcmask_%d_3,1,255) || our_range(document.basicqos.srcmask_%d_0,1,255)) && !valid_mask(document.basicqos, 'document.basicqos.srcmask_%d',BCST_OK))\n\t{\n", i, i, i, i, i);
		ret += websWrite(wp, "\t\tdocument.basicqos.srcmask_%d_0.focus();\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_macs_17(document.basicqos.srcmac_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_macs_17(document.basicqos.dstmac_%d))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.dstport_%d,0,65535,\"DstPort\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n", i);
		ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.srcport_%d,0,65535,\"SrcPort\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_ethtype(document.basicqos.ethtype_%d,1,4095))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");

		ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.vid_%d,0,4095,\"VID\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.plmin_%d,0,65535,\"Packetsize\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");
		ret += websWrite(wp, "\tif(!my_valid_range(document.basicqos.plmax_%d,0,65535,\"Packetsize\"))\n\t{\n", i);
		ret += websWrite(wp, "\t\treturn false;\n\t}\n");

	}		
	ret += websWrite(wp, "\treturn true;\n");
	return ret;
}	

int ej_dump_init_setting(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0;
	char qos_name[20], prio_name[12],word[500];
	char dstip[16], dstmask[16], fqdn[20], srcip[16], srcmask[16], srcmac[18], dstmac[18], pro[2], srcport[6], dstport[6],ethtype[8],alg[3],vid[8],pkt_min[8],pkt_max[8];

	memset(prio_name, 0, sizeof(prio_name));
	strcpy(prio_name,nvram_safe_get("qos_prio"));

	for(i = 0; i < QOS_NUM; i++)
	{
		int j;

		memset(word,0,sizeof(word));
		memset(qos_name, 0, sizeof(qos_name));
		sprintf(qos_name,"basic_qos%d",i);
		memset(dstip, 0, sizeof(dstip));
		memset(dstmask, 0, sizeof(dstmask));
		memset(srcip, 0, sizeof(srcip));
		memset(srcmask, 0, sizeof(srcmask));
		memset(srcmac, 0, sizeof(srcmac));
		memset(dstmac, 0, sizeof(dstmac));
		memset(pro, 0, sizeof(pro));
		memset(srcport, 0, sizeof(srcport));
		memset(dstport, 0, sizeof(dstport));
		memset(fqdn, 0, sizeof(fqdn));
		memset(ethtype, 0, sizeof(ethtype));
		memset(alg, 0, sizeof(alg));
		memset(vid, 0, sizeof(vid));
		memset(pkt_min, 0, sizeof(pkt_min));
		memset(pkt_max, 0, sizeof(pkt_max));

		strcpy(word,nvram_safe_get(qos_name));
		sscanf(word, "%16[^:]:%16[^:]:%20[^:]:%16[^:]:%16[^:]:%18[^:]:%18[^:]:%3[^:]:%6[^:]:%6[^:]:%8[^:]:%3[^:]:%8[^:]:%8[^:]:%8[^\n]", dstip, dstmask, fqdn, srcip, srcmask, srcmac, dstmac, pro, srcport, dstport, ethtype, alg, vid, pkt_min, pkt_max);

		for(j = 0; j < 4; j++)
		{
			ret += websWrite(wp, "\tdocument.basicqos.dstip_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstip, j));
			ret += websWrite(wp, "\tdocument.basicqos.dstmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(dstmask, j));
			ret += websWrite(wp, "\tdocument.basicqos.srcip_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcip, j));
			ret += websWrite(wp, "\tdocument.basicqos.srcmask_%d_%d.value=\"%d\";\n", i, j, get_single_ip(srcmask, j));
		}

		ret += websWrite(wp, "\tdocument.basicqos.fqdn_%d.value=\"%s\";\n", i, strcmp(fqdn,"null")?fqdn:"");
		ret += websWrite(wp, "\tdocument.basicqos.srcmac_%d.value=\"%s\";\n", i, srcmac);
		ret += websWrite(wp, "\tdocument.basicqos.dstmac_%d.value=\"%s\";\n", i, dstmac);
		ret += websWrite(wp, "\tdocument.basicqos.srcport_%d.value=\"%s\";\n", i, srcport);
		ret += websWrite(wp, "\tdocument.basicqos.dstport_%d.value=\"%s\";\n", i, dstport);

		ret += websWrite(wp, "\tdocument.basicqos.pro_%d[%d].selected=true;\n", i, atoi(pro));
		ret += websWrite(wp, "\tif(document.basicqos.pro_%d.selectedIndex==0)\n", i);
		ret += websWrite(wp, "\t{\n\t\tdocument.basicqos.dstport_%d.disabled=true;\n", i);
		ret += websWrite(wp, "\t\tdocument.basicqos.srcport_%d.disabled=true;\n\t}\n", i);
		ret += websWrite(wp, "\telse\n");
		ret += websWrite(wp, "\t{\n\t\tdocument.basicqos.dstport_%d.disabled=false;\n", i);		
		ret += websWrite(wp, "\t\tdocument.basicqos.srcport_%d.disabled=false;\n\t}\n", i);		

		ret += websWrite(wp, "\tdocument.basicqos.ethtype_%d.value=\"%s\";\n", i, ethtype);
		ret += websWrite(wp, "\tdocument.basicqos.alg_%d[%d].selected=true;\n", i, atoi(alg));
		ret += websWrite(wp, "\tdocument.basicqos.vid_%d.value=\"%s\";\n", i, vid);
		ret += websWrite(wp, "\tdocument.basicqos.plmin_%d.value=\"%s\";\n", i, pkt_min);
		ret += websWrite(wp, "\tdocument.basicqos.plmax_%d.value=\"%s\";\n", i, pkt_max);
		ret += websWrite(wp, "\tdocument.basicqos.prio_%d[%c].selected=true;\n", i, prio_name[i]);

		ZB_PRINTF("prio_name[%d]=%c,prio_name=%s\n",i,prio_name[i],prio_name);
	}
	return ret;
}

void validate_qos_config(webs_t wp, char *value, struct variable *v)
{
	char word[500], qos_prio[16];
	char *qosSW_l,*qos_frg,*qos_frgsize,*dstip,*dstmask,*srcip,*srcmask;
	int i;

	memset(qos_prio,0,sizeof(qos_prio));
	
	ZB_PRINTF("enter validate_qos_config\n");

	//sprintf(qos_conf,"basicqos_%d",which_conn);

	qosSW_l = websGetVar(wp, "qosSW", "0");
	qos_frg = websGetVar(wp, "hid_qos_frg", "0");
	qos_frgsize = websGetVar(wp, "qos_frgsize", "0");
	
	ZB_PRINTF("enter validate_qos_config2\n");
	
	nvram_set("qosSW", qosSW_l);
	
	ZB_PRINTF("qos_frg=%s\n",qos_frg);

	//sprintf(qosprio_conf,"qos_prio_%d",which_conn)
	//strcpy(qos_prio,nvram_safe_get(qosprio_conf));

	dstmask = malloc(20);
	srcip = malloc(20);
	dstip = malloc(20);
	srcmask = malloc(20);

	for(i = 0;i < QOS_NUM;i++)
	{
		char tmpbuf[20],temp[20],nr_qos[12];
		char *srcmac,*dstmac,*pro,*dstport,*srcport,*fqdn,*fqdn1,*ethtype,*alg,*vid,*pkt_min,*pkt_max,*prio,*val;
		int j;
		memset(temp,0,sizeof(temp));
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"srcmac_%d",i);
		srcmac =  websGetVar(wp,tmpbuf,"0");

		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"dstmac_%d",i);
		dstmac =  websGetVar(wp,tmpbuf,"0");
		
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"pro_%d",i);
		pro =  websGetVar(wp,tmpbuf,"0");

		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"dstport_%d",i);
		dstport =  websGetVar(wp,tmpbuf,"0");
#ifdef ZB_DEBUG
		//printf("dstport=%s\n",dstport);
#endif

		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"srcport_%d",i);
		srcport =  websGetVar(wp,tmpbuf,"0");
#ifdef ZB_DEBUG
		//printf("srcport=%s\n",srcport);
#endif
		
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"fqdn_%d",i);
		fqdn1 =  websGetVar(wp,tmpbuf,"0");
		fqdn =  strcmp(fqdn1,"")?fqdn1:"null";

		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"ethtype_%d",i);
		ethtype =  websGetVar(wp,tmpbuf,"0");
	
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"alg_%d",i);
		alg =  websGetVar(wp,tmpbuf,"0");
	
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"vid_%d",i);
		vid =  websGetVar(wp,tmpbuf,"0");
		
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"plmin_%d",i);
		pkt_min =  websGetVar(wp,tmpbuf,"0");
	
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"plmax_%d",i);
		pkt_max =  websGetVar(wp,tmpbuf,"0");
	
		memset(tmpbuf,0,sizeof(tmpbuf));
		sprintf(tmpbuf,"prio_%d",i);
		prio =  websGetVar(wp,tmpbuf,"0");

		qos_prio[i] = *prio;
		
		memset(temp, 0, sizeof(temp));
		memset(dstip, 0, sizeof(dstip));
		for(j=0 ; j<4 ; j++)
		{
               		snprintf(temp, sizeof(temp), "dstip_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(dstip,val);
                        	if(j<3)
					strcat(dstip,".");
                	}
                	else
			{
				free(dstip);
                        	return ;
			}
        	}

		memset(dstmask, 0, sizeof(dstmask));
		memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "dstmask_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(dstmask,val);
                        	if(j<3)
					strcat(dstmask,".");
                	}
                	else
			{
				free(dstmask);
                        	return ;
			}
        	}

		memset(srcip, 0, sizeof(srcip));
		memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "srcip_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(srcip, val);
                        	if(j<3)
					strcat(srcip, ".");
                	}
                	else
			{
				free(srcip);
                        	return ;
			}
        	}

		memset(srcmask, 0, sizeof(srcmask));
        	memset(temp, 0, sizeof(temp));
        	for(j=0 ; j<4 ; j++)
		{
                	snprintf(temp, sizeof(temp), "srcmask_%d_%d", i, j);
                	val = websGetVar(wp, temp , NULL);
                	if(val)
			{
                        	strcat(srcmask, val);
                        	if(j<3)
					strcat(srcmask, ".");
                	}
                	else
			{
				free(srcmask);
                        	return ;
			}
        	}
		
		if(strcmp(srcip, "0.0.0.0") && !strcmp(srcmask, "0.0.0.0"))
			strcpy(srcmask, "255.255.255.255");
		if(strcmp(dstip, "0.0.0.0") && !strcmp(dstmask, "0.0.0.0"))
			strcpy(dstmask, "255.255.255.255");
		
		memset(word, 0, sizeof(word));
		sprintf(word, "%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s", dstip, dstmask, fqdn, srcip, srcmask, srcmac, dstmac, pro, srcport, dstport, ethtype, alg, vid, pkt_min, pkt_max);
		//cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s", cur == buf ? "" : " ", word);
		sprintf(nr_qos,"basic_qos%d",i);
		nvram_set(nr_qos,word);
	} /* loop end */
	nvram_set("qos_prio",qos_prio);
	nvram_set("qos_frg",qos_frg);
	nvram_set("qos_frgsize",qos_frgsize);
	//nvram_commit(); //no need?	

	if(dstip)
		free(dstip);
	if(dstmask)
		free(dstmask);
	if(srcip)
		free(srcip);
	if(srcmask)
		free(srcmask);
	return;
}

