#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>

#include <broadcom.h>

#include "pvc.h"

#define HTTPS_PORT 17890;

#define y_printf(format, args...)
//#define LJZ_DEBUG
char connection_forward[5];
char connection_trigger[5];
char connection_firewall[5];
char connection_manage[5];
char connection_dmz[5];
char connection_routing[5];
extern char connection[5];  // seleted connetion index (global)
extern int connchanged ;//connection change flag
extern char * re_ip_local;
extern char * re_ip_des;
extern char vcc_config_buf[60];
extern char pppoe_config_buf[210];
extern char pppoa_config_buf[150];
extern char bridged_config_buf[100];
extern char routed_config_buf[100];

#ifdef CLIP_SUPPORT
char clip_config_buf[100];
#endif

#ifdef MPPPOE_SUPPORT
char mpppoe_config_buf[280];
#endif

#ifdef IPPPOE_SUPPORT
char ipppoe_config_buf[280];
#endif


extern int vcc_inited;
extern int pppoa_inited;
extern int pppoe_inited;
extern int bridged_inited;
extern int routed_inited;

#ifdef CLIP_SUPPORT
int clip_inited;
#endif
#ifdef MPPPOE_SUPPORT
int mpppoe_inited;
#endif

#ifdef IPPPOE_SUPPORT
int ipppoe_inited;
#endif

//junzhao add 2004.3.10
char vpi_tmp[5];
char vci_tmp[8];
//char multiplex_tmp[5];
char qostype_tmp[8];
char pcr_tmp[8];
char scr_tmp[8];
char autodetect_tmp[3];
char applyonboot_tmp[3];

/*
char username_tmp[64];
char password_tmp[64];
char servicename_tmp[64];
char idletime_tmp[8];
char redialperiod_tmp[8];
char demand_tmp[3];
*/
extern int clone_wan_mac;

extern int error_value;



extern char cur_connection_forward[];
extern char cur_connection_trigger[];

static char range_buf[500]="";


int
ej_connection_table_forward(int eid,webs_t wp,int argc,char_t **argv)
{
	int i, ret = 0, which;
	char *type;

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	y_printf("this is ej_forword.\n");
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "wan_connection_forward", NULL);
//		printf("connection %s\n", connection);
		y_printf("!tmpconn ==%s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(connection_forward, tmpconn)))
		{
			printf("not the same conn\n");
			strncpy(connection_forward, tmpconn, sizeof(connection_forward));
			connchanged = 1; //make the flag of change conn
			vcc_inited = 0;
			pppoa_inited = 0;
			pppoe_inited = 0;
			bridged_inited = 0;
			routed_inited = 0;
	#ifdef CLIP_SUPPORT
			clip_inited = 0;
	#endif	
	#ifdef MPPPOE_SUPPORT
			mpppoe_inited = 0;
	#endif
	#ifdef IPPPOE_SUPPORT
			ipppoe_inited = 0;
	#endif
		}
		//junzhao 2004.4.1
		else
		{
			y_printf("\n!enter save_vcc\n");
			save_vcc_tmpbuf(wp);
		}
	}		
	else //first appear
	{
		strncpy(connection_forward, nvram_safe_get("wan_connection_forward"), sizeof(connection_forward));
		vcc_inited = 0;
		pppoa_inited = 0;
		pppoe_inited = 0;
		bridged_inited = 0;
		routed_inited = 0;
#ifdef CLIP_SUPPORT
		clip_inited = 0;
#endif
#ifdef MPPPOE_SUPPORT
		mpppoe_inited = 0;
#endif
#ifdef IPPPOE_SUPPORT
		ipppoe_inited = 0;
#endif
	}

	which = atoi(connection_forward);
//	which = 1;
//	printf("which=%d",which);
	if(!strcmp(type,"select"))
	{
		for(i=0 ; i < PVC_CONNECTIONS ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	printf("ret==%d",ret);
	}
	printf("all right again,whitch ==%d \n",which);
	return ret;

}
int 
ej_connection_table_trigger(int eid,webs_t wp,int argc,char_t **argv)
{
	int i, ret = 0, which;
	char *type;

        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	printf("this is ej_connection_trigger.\n");
	printf("gozila_action=%d\n",gozila_action);
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "wan_connection_trigger", NULL);
//		printf("connection %s\n", connection);
		printf("!tmpconn ==%s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(connection_trigger, tmpconn)))
		{
			printf("not the same conn\n");
			strncpy(connection_trigger, tmpconn, sizeof(connection_forward));
			connchanged = 1; //make the flag of change conn
			vcc_inited = 0;
			pppoa_inited = 0;
			pppoe_inited = 0;
			bridged_inited = 0;
			routed_inited = 0;
	#ifdef CLIP_SUPPORT
			clip_inited = 0;
	#endif	
	#ifdef MPPPOE_SUPPORT
			mpppoe_inited = 0;
	#endif
	#ifdef IPPPOE_SUPPORT
			ipppoe_inited = 0;
	#endif
		}
		//junzhao 2004.4.1
		else
		{
			printf("\n!enter save_vcc\n");
			save_vcc_tmpbuf(wp);
		}
	}		
	else //first appear
	{
		strncpy(connection_trigger, nvram_safe_get("wan_connection_trigger"), sizeof(connection_trigger));
		vcc_inited = 0;
		pppoa_inited = 0;
		pppoe_inited = 0;
		bridged_inited = 0;
		routed_inited = 0;
#ifdef CLIP_SUPPORT
		clip_inited = 0;
#endif
#ifdef MPPPOE_SUPPORT
		mpppoe_inited = 0;
#endif
#ifdef IPPPOE_SUPPORT
		ipppoe_inited = 0;
#endif
	}

	which = atoi(connection_trigger);
//	which = 1;
//	printf("which=%d",which);
	if(!strcmp(type,"select"))
	{
	printf("webwriter:");	
	for(i=0 ; i < PVC_CONNECTIONS ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	printf("ret=%d\n",ret);
	}
	printf("all right again,whitch ==%d \n",which);
	return ret;

}

int 
ej_connection_table_firewall(int eid,webs_t wp,int argc,char_t **argv)
{
	int i, ret = 0, which;
	char *type;
	char from[]="100.0.0.1";
	char to[]="100.0.0.11";
	char var[256],*next;
	char *wordlist;
        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	printf("this is ej_connection_firewall\n");
	printf("gozila_action=%d\n",gozila_action);
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "wan_connection_firewall", NULL);
		printf("connection %s\n", connection_firewall);
		printf("!tmpconn ==%s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(connection_firewall, tmpconn)))
		{
			printf("not the same conn\n");
			strncpy(connection_firewall, tmpconn, sizeof(connection_firewall));
			printf("conn_firewall=%s\n",connection_firewall);
		}
		//junzhao 2004.4.1
		else
		{
			printf("\n!enter save_vcc\n");
			save_vcc_tmpbuf(wp);
		}
	}		
	else //first appear
	{
		strncpy(connection_firewall, nvram_safe_get("wan_connection_firewall"), sizeof(connection_firewall));
		vcc_inited =0;

                pppoa_inited = 0;
                pppoe_inited = 0;
                bridged_inited = 0;
                routed_inited = 0;
#ifdef CLIP_SUPPORT
                clip_inited = 0;
#endif
#ifdef MPPPOE_SUPPORT
                mpppoe_inited = 0;
#endif
#ifdef IPPPOE_SUPPORT
                ipppoe_inited = 0;
#endif








	
	printf("firewall = %s \n",connection_firewall);
	}

which = atoi(connection_firewall);
 //  which = 1;
 //  printf("which=%d",which);
	 if(!strcmp(type,"select"))
	 {
	 printf("webwriter:");	 
	 for(i=0 ; i < PVC_CONNECTIONS ; i++)
			 ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	 printf("ret=%d\n",ret);
	 }
	 printf("all right again,whitch ==%d \n",which);









	return ret;

}

 void
	validate_choice_firewall(webs_t wp, char *value, struct variable *v)
{
	char name[30];
	char *name_index;	
	strcpy(name,v->name);
	name_index = name;
        printf("this is choice firewall\n");
	
	printf("num =%s\n",connection_firewall);
//	name_index = v->name;
    //    printf("v->name=%s\n",v->name);
	printf("ind=%s\n",connection_firewall);
        name_index = strcat(name_index,connection_firewall);
	printf("name_index = %s\n",name_index);
	nvram_set("wan_connection_firewall",connection_firewall);
//	*(name_index+sizeof(v->name)) = connection_firewall;
	if (valid_choice(wp, value, v))
//		nvram_set(v->name, value);
		nvram_set(name_index,value);
}
 int 
 ej_nvram_match_firewall(int eid,webs_t wp,int argc,char_t **argv)
 {

	 char *name, *match, *output;
	 char name1[30];
	 char *name_index=name1;
//	 name_index = v->name;
//	 name_index = strcat(name_index,connection_firewall);
	 printf("this is firewall_match\n");
 	 if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	 }
         name_index = name;
         name_index = strcat(name_index,connection_firewall);
 	printf("nameindex = %s   ",name_index);

	 printf("this is nvram_match_firewall,name=%s,match=%s,output=%s",name,match,output);
	 if (nvram_match(name_index, match))
 {
 printf("Entering match\n");
 printf("name =%s\n",name_index);
		 return websWrite(wp, output);
 }
 printf("out\ of match\n");
	 return 0;

 
}


 int 
 ej_connection_table_dmz(int eid,webs_t wp,int argc,char_t **argv)
 {
	 int i, ret = 0, which;
	 char *type;
	char buf[256];
	char var1[256],var2_buf[20], *next1;
	char  *next2;
	char *var2=var2_buf;
	char *name1, *enable1, *proto1, *port1, *ip1;
	char *name2, *enable2, *proto2, *port2, *ip2;
	char buff[256],name1_buf[20],name2_buf[20];
	char *name1_index = name1_buf;
	char *name2_index = name2_buf;
//	char *wordlist=":on:both:1:2>11";
	char wordlist_buf1[20],wordlist_buf2[20];
	char *wordlist1=wordlist_buf1;
	char *wordlist2=wordlist_buf2;
	char lan_cclass[]="10.128.32.";
	int  a1 = 0, a2 = 0;
	int sig=0;
 	 if (ejArgs(argc, argv, "%s", &type) < 1) 
	 {
			  websError(wp, 400, "Insufficient args\n");
			  return -1;
		 }
	 printf("this is ej_connection_dmzl\n");
	 printf("gozila_action=%d\n",gozila_action);
	 if(gozila_action) //for form post 
	 {
		 char *tmpconn = websGetVar(wp, "wan_connection_dmz", NULL);
		 printf("connection %s\n", connection_dmz);
		 printf("!tmpconn ==%s\n", tmpconn);
		 if((tmpconn!=NULL) && (strcmp(connection_dmz, tmpconn)))
		 {
			 printf("not the same conn\n");
			 strncpy(connection_dmz, tmpconn, sizeof(connection_dmz));
			 printf("conn_firewall=%s\n",connection_dmz);
		 }
		 //junzhao 2004.4.1
		 else
		 {
			 printf("\n!enter save_vcc\n");
			 save_vcc_tmpbuf(wp);
		 }
	 }		 
	 else //first appear
	 {
		 strncpy(connection_dmz, nvram_safe_get("wan_connection_dmz"), sizeof(connection_dmz));

 
 
 

	 }
 
	 which = atoi(connection_dmz);
 //  which = 1;
 //  printf("which=%d",which);
	 if(!strcmp(type,"select"))
	 {
	 printf("webwriter:");	 
	 for(i=0 ; i < PVC_CONNECTIONS ; i++)
			 ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	 printf("ret=%d\n",ret);
	 }
	 printf("all right again,whitch ==%d \n",which);









	

	 return ret;
 
 }

  void
	 validate_choice_dmz(webs_t wp, char *value, struct variable *v)
 {
	 char name[30];
	 char *name_index;	 
	 strcpy(name,v->name);
	 name_index = name;
		 printf("this is choice dmz\n");
	 
	 printf("num =%s\n",connection_dmz);
 
	 
	 name_index = strcat(name_index,connection_dmz);
//	 printf("name_index = %s,value=%d\n",name_index,value);
	 nvram_set("wan_connection_dmz",connection_dmz);
 //  *(name_index+sizeof(v->name)) = connection_firewall;
	 if (valid_choice(wp, value, v))
 	
{
		printf("name_index=%s,value=%d\n",name_index,value);
		 nvram_set(name_index,value);
}
 }
  int 
 ej_nvram_match_dmz(int eid,webs_t wp,int argc,char_t **argv)
 {

	 char *name, *match, *output;
	 char name1[30];
	 char *name_index=name1;
//	 name_index = v->name;
//	 name_index = strcat(name_index,connection_firewall);
	 printf("this is firewall_match\n");
 	 if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	 }
         name_index = name;
         name_index = strcat(name_index,connection_dmz);
 	printf("nameindex = %s   ",name_index);

	 printf("this is nvram_match_firewall,name=%s,match=%s,output=%s",name,match,output);
	 if (nvram_match(name_index, match))
 {
 printf("Entering match\n");
 printf("name =%s\n",name_index);
		 return websWrite(wp, output);
 }
 printf("out\ of match\n");
	 return 0;

 
}
 
 int
 ej_nvram_get_dmz(int eid, webs_t wp, int argc, char_t **argv)
 {
	 char *name, *c;
	 int ret = 0;













	
	char *name_index;
printf("it is nvram_get dmz\n");
	 if (ejArgs(argc, argv, "%s", &name) < 1) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	 }
 
#if COUNTRY == JAPAN
	 ret = websWrite(wp, "%s",nvram_safe_get(name));
#else
	 name_index = name;
	 name_index = strcat(name_index,connection_dmz);

	 printf("......nvram_get,name=%s\n",name_index); 
	 for (c = nvram_safe_get(name_index); *c; c++) {
		 if (isprint((int) *c) &&
			 *c != '"' && *c != '&' && *c != '<' && *c != '>')
	 //  {		 
			 ret += websWrite(wp, "%c", *c);
			 //printf("ret=%d,*c=%s\n",*c);
	 //  }
		 else
			 ret += websWrite(wp, "&#%d", *c);
	 }
#endif
	 return ret;
 }
/*
 * Example: 
 * lan_ipaddr = 192.168.1.1
 * <% get_single_ip("lan_ipaddr","1"); %> produces "168"
 */
int
ej_get_single_ip_manage(int eid, webs_t wp, int argc, char_t **argv)
{
	char *name, *c;
	int ret = 0;
	int which;

	if (ejArgs(argc, argv, "%s %d", &name, &which) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	printf("nmame======%s\n",name);
//	
        name= strcat(name,connection_manage);
	c = nvram_safe_get(name);
	printf("c=%s\n",c);
	if(c){
		if(!strcmp(c, PPP_PSEUDO_IP) || !strcmp(c, PPP_PSEUDO_GW))
			c = "0.0.0.0";
		else if (!strcmp(c, PPP_PSEUDO_NM))
			c = "255.255.255.0";

		ret += websWrite(wp, "%d", get_single_ip(c,which));
#ifdef MY_DEBUG
	LOG(LOG_DEBUG,"%s(): name=[%s] which=[%d] ip=[%d]",__FUNCTION__,name,which,get_single_ip(c,which));
#endif
	}
	else
		ret += websWrite(wp, "0");

	return ret;
}


/*Restricted IP  range */
void
validate_filter_range_ip(webs_t wp, char *value, struct variable *v)
{
	char buf_start[256], buf_end[256], buf_range[256],buf_save[20];
	char *range_ip_1, *range_ip_2, *range_ip_3, *range_ip_4, *range_ip_5;
	char ip_name[10],ip_start[16],ip_end[14],ip_save[15];
	char *ip_index = ip_name;
	char *start_index=ip_start;
	char *end_index = ip_end;
	char *save_index = ip_save;
	memset(buf_start, 0, sizeof(buf_start));
	memset(buf_end, 0, sizeof(buf_end));
	memset(buf_range, 0, sizeof(buf_range));

	range_ip_1 = websGetVar(wp, "range_ip_1", "0");
	range_ip_2 = websGetVar(wp, "range_ip_2", "0");
	range_ip_3 = websGetVar(wp, "range_ip_3", "0");
	range_ip_4 = websGetVar(wp, "range_ip_4", "0");
	range_ip_5 = websGetVar(wp, "range_ip_5", "0");

	sprintf(buf_start, "%s.%s.%s.%s", range_ip_1, range_ip_2, range_ip_3, range_ip_4);
	sprintf(buf_end, "%s.%s.%s.%s", range_ip_1, range_ip_2, range_ip_3, range_ip_5);
	sprintf(buf_range, "%s %s %s %s-%s", range_ip_1, range_ip_2, range_ip_3, range_ip_4, range_ip_5);
	sprintf(buf_save, "%s.%s.%s.%s-%s", range_ip_1, range_ip_2, range_ip_3, range_ip_4, range_ip_5);
	snprintf(ip_index,10,"range_ip%d",atoi(connection_manage));
	snprintf(start_index,16,"range_start_ip%d",atoi(connection_manage));
	snprintf(end_index,14,"range_end_ip%d",atoi(connection_manage));
	snprintf(save_index,15,"range_save_ip%d",atoi(connection_manage));
	nvram_set(start_index, buf_start);
	nvram_set(end_index, buf_end);
	nvram_set(ip_index, buf_range);
	nvram_set(save_index, buf_save);
}

 void
 validate_range_dmz(webs_t wp, char *value, struct variable *v)
 {
	 char buf[20];
	 char buf1[20];
	 char *ip_index = buf1;
	 int range;
	printf("it is real range\n");
	 if (valid_range(wp, value, v)){
		printf("here\n"); 
		range = atoi(value);
		printf("range=%d,con=%s\n",range,connection_dmz);
		 snprintf(ip_index,12,"dmz_ipaddr%d",atoi(connection_dmz));
		 snprintf(buf, sizeof(buf),"%d", range);
		printf("buf=%s,ip=%s\n",buf,ip_index);
		 nvram_set(ip_index, buf);
	 }
 }
 int 
 ej_connection_table_manage(int eid,webs_t wp,int argc,char_t **argv)
 {
	 int i, ret = 0, which;
	 char *type;
 
		 if (ejArgs(argc, argv, "%s", &type) < 1) 
	 {
			  websError(wp, 400, "Insufficient args\n");
			  return -1;
		 }
	 printf("this is ej_connection_manage\n");
	 printf("gozila_action=%d\n",gozila_action);
	 if(gozila_action) //for form post 
	 {
		 char *tmpconn = websGetVar(wp, "wan_connection_manage", NULL);
		 printf("connection %s\n", connection_manage);
		 printf("!tmpconn ==%s\n", tmpconn);
		 if((tmpconn!=NULL) && (strcmp(connection_manage, tmpconn)))
		 {
			 printf("not the same conn\n");
			 strncpy(connection_manage, tmpconn, sizeof(connection_manage));
			 printf("conn_firewall=%s\n",connection_manage);
		 }
		 //junzhao 2004.4.1
		 else
		 {
			 printf("\n!enter save_vcc\n");
			 save_vcc_tmpbuf(wp);
		 }
	 }		 
	 else //first appear
	 {
		 strncpy(connection_manage, nvram_safe_get("wan_connection_manage"), sizeof(connection_manage));

 
 
 

	 }
 
	 which = atoi(connection_manage);
 //  which = 1;
 //  printf("which=%d",which);
	 if(!strcmp(type,"select"))
	 {
	 printf("webwriter:");	 
	 for(i=0 ; i < PVC_CONNECTIONS ; i++)
			 ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	 printf("ret=%d\n",ret);
	 }
	 printf("all right again,whitch ==%d \n",which);
	 return ret;
 
 }
  int 
 ej_nvram_match_manage(int eid,webs_t wp,int argc,char_t **argv)
 {

	 char *name, *match, *output;
	 char name1[30];
	 char *name_index=name1;
//	 name_index = v->name;
//	 name_index = strcat(name_index,connection_firewall);
	 printf("this is manage_match\n");
 	 if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		 websError(wp, 400, "Insufficient args\n");
		 return -1;
	 }
         name_index = name;
/*	if(!(atoi(connection_manage)<8&&atoi(connection_manage)>=0))
	{
		printf("heere......\n");
	}
	else
{
	strcpy(connection_manage,nvram_safe_get("wan_connection_manage"));
	printf("wan=%s\n\n",nvram_safe_get("wan_connection_manage"));
}
*/	printf("connection_manage=%s\n",connection_manage);
         name_index = strcat(name_index,connection_manage);
 	printf("nameindex = %s   ",name_index);

	 printf("this is nvram_match_manage,name=%s,match=%s,output=%s\n",name,match,output);
	 if (nvram_match(name_index, match))
 {
 printf("Entering match\n");
 printf("name =%s\n",name_index);
		 return websWrite(wp, output);
 }
 printf("out\ of match\n");
	 return 0;

 
}
 
 int 
 ej_nvram_match_manage1(int eid,webs_t wp,int argc,char_t **argv)
 {

	 char *name, *match, *output;
	 char name1[30];
	 char buf[10];
	 char buf_index =buf;
	 char *name_index=name1;
//	 name_index = v->name;
//	 name_index = strcat(name_index,connection_firewall);
	 printf("this is mdisable........\n");
 	 if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		 printf("it is ejArgs\n");
			websError(wp, 400, "Insufficient args\n");
		 return -1;
	 }
         printf("next is strcat|n");

         name_index = strcat(name_index,connection_manage);
 	printf("nameindex = %s   ",name_index);
	buf_index = nvram_safe_get(name_index);
	 printf("this is nvram_match_manage,name=%s,match=%s,output=%s",name,match,output);
	printf("buf_index=%d",atoi(buf_index));
	 if (atoi(buf_index)==0)
 {
 printf("Entering match\n");
 printf("name =%s\n",name_index);
		 return websWrite(wp, output);
 }
 printf("out\ of match\n");
	 return 0;

 
}
 
void validate_choice_manage(webs_t wp, char *value, struct variable *v)
{
	 char name[30];
	 char *name_index;	 
	 strcpy(name,v->name);
	 name_index = name;
	 y_printf("this is choice manage,v->name=%s\n",v->name);
	 
	 y_printf("num =%s\n",connection_manage);
 //  name_index = v->name;
	 //    printf("v->name=%s\n",v->name);
//	 printf("ind=%s\n",connection_manage);
	 name_index = strcat(name_index,connection_manage);
	 y_printf("name_index = %s\n",name_index);
	 nvram_set("wan_connection_manage",connection_manage);
 //  *(name_index+sizeof(v->name)) = connection_firewall;
	 if (valid_choice(wp, value, v))
	{
 // 	 nvram_set(v->name, value);
		 printf("it is valid_choice\n");
		 nvram_set(name_index,value);
	}
 }
  
   int
   ej_nvram_get_manage(int eid, webs_t wp, int argc, char_t **argv)
   {
	   char *name, *c;
	   int ret = 0;
	  
	  char *name_index;
  y_printf("it is nvram_get manage\n");
	   if (ejArgs(argc, argv, "%s", &name) < 1) {
		   websError(wp, 400, "Insufficient args\n");
		   return -1;
	   }
   
#if COUNTRY == JAPAN
	   ret = websWrite(wp, "%s",nvram_safe_get(name));
#else
	   name_index = name;
	   name_index = strcat(name_index,connection_manage);
  
	   y_printf("......nvram_get,name=%s\n",name_index); 
	   for (c = nvram_safe_get(name_index); *c; c++) {
		   if (isprint((int) *c) &&
			   *c != '"' && *c != '&' && *c != '<' && *c != '>')
	   //  {	   
			   ret += websWrite(wp, "%c", *c);
			   //printf("ret=%d,*c=%s\n",*c);
	   //  }
		   else
			   ret += websWrite(wp, "&#%d", *c);
	   }
#endif
	   return ret;
   }
   void
   validate_name_manage(webs_t wp, char *value, struct variable *v)
   {
   		char usename[22];
		char *name_index = usename;
snprintf(name_index,22,"http_remote_username%d",atoi(connection_manage));


		   nvram_set(name_index, value);
   }
   void
validate_password_manage(webs_t wp, char *value, struct variable *v)
{
	   char password[22];
	   char *name_index =password;
	   snprintf(name_index,20,"http_remote_passwd%d",atoi(connection_manage));

	if (strcmp(value, TMP_PASSWD) && valid_name(wp, value, v))
	{	
		nvram_set(name_index, value);
#ifdef DDM_SUPPORT
		if( !strcmp(v->name,"http_passwd") )
		{	
			nvram_set("DDM_pass_flag","1");		
		}	
#endif		
	}	
}
   void
   validate_range_manage(webs_t wp, char *value, struct variable *v)
   {
	   char buf[20];
	   int range;
	   char port[14];
	   char *name_index =port;	
	   snprintf(name_index,14,"http_wanport%d",atoi(connection_manage));
	   if (valid_range(wp, value, v)){
		   range = atoi(value);
		   snprintf(buf, sizeof(buf),"%d", range);
		   nvram_set(name_index, buf);
	   }
   }
int
ej_nvram_flesh_manage(int eid,webs_t wp,int argc,char_t **argv)
{
	char *name,*c;
	char buf[19];
	char *buf_index = buf;
	char *tmp;
	char *tmpconn;
	int ret = 0;
	if(ejArgs(argc,argv,"%s",&name)<1) {
		websError(wp,400,"Insufficient args\n");
			return -1;
	}
		tmpconn = websGetVar(wp, "wan_connection_manage", NULL);
		 if((tmpconn!=NULL) && (strcmp(connection_manage, tmpconn)))
		 {

			 strncpy(connection_manage, tmpconn, sizeof(connection_manage));
		}
	snprintf(buf_index,19,"remote_management%d",atoi(connection_manage));
	tmp = nvram_safe_get(buf_index);
	ret = websWrite(wp, "%s",tmp);
	y_printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\it is nvram_r,ret=%d,%d,%s\n",ret,atoi(connection_manage),tmp);
	return ret;
}
int
ej_nvram_flesh_dmz(int eid,webs_t wp,int argc,char_t **argv)
{
	char *name,*c;
	char buf[19];
	char *buf_index = buf;
	char *tmp;
	char *tmpconn;
	int ret = 0;
	if(ejArgs(argc,argv,"%s",&name)<1) {
		websError(wp,400,"Insufficient args\n");
			return -1;
	}
		tmpconn = websGetVar(wp, "wan_connection_dmz", NULL);
		 if((tmpconn!=NULL) && (strcmp(connection_dmz, tmpconn)))
		 {

			 strncpy(connection_dmz, tmpconn, sizeof(connection_dmz));
		}
	snprintf(buf_index,12,"dmz_enable%d",atoi(connection_dmz));
	y_printf("buf_index=%s\n",buf_index);
	tmp = nvram_safe_get(buf_index);
	y_printf("tmp=%s\n",tmp);
	ret = websWrite(wp, "%s",tmp);
	y_printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\it is nvram_r,ret=%d,%d,%s\n",ret,atoi(connection_dmz),tmp);
	return ret;
}
int
ej_connection_table_routing(int eid,webs_t wp,int argc,char_t **argv)
{
	int i, ret = 0, which;
	char *type;
char var1[256], *wordlist1, *next1;
	char var2[256], *wordlist2, *next2;
	char from_ip[16], to_ip[14];
	char *start_ip=from_ip;
	char *end_ip = to_ip;
	char *from ,*to;
	int  a1 = 0, a2 = 0;


        if (ejArgs(argc, argv, "%s", &type) < 1) 
	{
             websError(wp, 400, "Insufficient args\n");
             return -1;
        }
	y_printf("this is ej_routing.\n");
	if(gozila_action) //for form post 
	{
		char *tmpconn = websGetVar(wp, "wan_connection_routing", NULL);
//		printf("connection %s\n", connection);
		y_printf("!tmpconn ==%s\n", tmpconn);
		if((tmpconn!=NULL) && (strcmp(connection_routing, tmpconn)))
		{
			y_printf("not the same conn\n");
			strncpy(connection_routing, tmpconn, sizeof(connection_routing));

		}
		//junzhao 2004.4.1
		else
		{
			y_printf("\n!enter save_vcc\n");
			save_vcc_tmpbuf(wp);
		}
	}		
	else //first appear
	{
		strncpy(connection_routing, nvram_safe_get("wan_connection_routing"), sizeof(connection_routing));

	}

	which = atoi(connection_routing);

	if(!strcmp(type,"select"))
	{
		for(i=0 ; i < PVC_CONNECTIONS ; i++)
			ret = websWrite(wp,"\t\t<option value=\"%d\" %s> %d </option>\n",i,(i == which) ? "selected" : "",i+1);
	}
	y_printf("all right again,whitch ==%d \n",which);

	return ret;

}

