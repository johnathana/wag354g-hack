/*
 *  Atm service
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <bcmnvram.h>
#include <atm.h>
#include <linux/atmdev.h>
#include <linux/atmppp.h>
#include <linux/atmbr2684.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/ppp_defs.h>
#include <linux/if_ppp.h>
#include <cy_conf.h>
#include <shutils.h>
#include <syslog.h>
#include "pvc.h"
#include <code_pattern.h>

#define MAX_SMSG_LEN 32
#define SOCK_DGRAM 1

//#define LJZ_DEBUG

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

int pppoa_load();
int establish_atmchan(int vpi_value, int vci_value);
int rfc2684_load(int autoway);
int create_br(int nrum);
int assign_vcc(int vpi_value, int vci_value, int encap, int qos_type, int pcr_value, int scr_value, int bufsize);
int PPPOAAutoDetect(int atm_fd);
int PPPOEAutoDetect();
#ifdef UNNUMBERED_SUPPORT
int unip_PPPOEAutoDetect();  /* kirby */
#endif
int DHCPAutoDetect();
int STATICAutoDetect(char *vpivci);						void nvram_modify_status(char *type, char *str);
int ram_modify_status(const char *filename, const char *str);
void nvram_modify_vpivci(int vpi_value, int vci_value);
int vpivci_checkused(int vpi_value, int vci_value);

#ifdef CLIP_SUPPORT
int clip_load();
struct clip_config *clip_config_ptr;
extern int atmarpd_index;
#endif

//for vpivci autodetect
//int vpivci_value[2*MAX_CHAN] = {0,35,8,35,0,43,0,51,0,59,8,43,8,51,8,59}; //selected vpivci value
#if RELCOUN == GERMANY
int vpivci_value[2*MAX_CHAN] = {1,32,8,35,0,43,0,51,0,59,8,43,8,51,8,59}; //selected vpivci value
#else
int vpivci_value[2*MAX_CHAN] = {0,35,8,35,0,43,0,51,0,59,8,43,8,51,8,59}; //selected vpivci value
#endif

//nas fd for every connection
int sockfd_used[MAX_CHAN]; 

//for vpivci final setting
int vpivci_final_value[MAX_CHAN][2]; 

//for last autodetect result
int wan_autoresult[MAX_CHAN];

//ppp pid for every connection
pid_t ppp_pid[MAX_CHAN];

//udhcpc pid for every connection
pid_t udhcpc_pid[MAX_CHAN];

int sndbuf = 8192;
extern struct vcc_config *vcc_config_ptr;
extern struct pppoe_config *pppoe_config_ptr;
extern struct pppoa_config *pppoa_config_ptr;
extern struct bridged_config *bridged_config_ptr;
extern struct routed_config *routed_config_ptr;
#ifdef CLIP_SUPPORT
extern struct clip_config *clip_config_ptr;
#endif
extern int which_conn;

int ram_modify_status(const char *filename, const char *str)
{
	int i;
        char word[64], *next;
        char buf[512], *cur=buf;
	char readbuf[512];
        char old[MAX_CHAN][60];
	
	memset(word, 0, sizeof(word));
        memset(buf, 0, sizeof(buf));
	
        for(i=0; i<MAX_CHAN; i++)
                memset(old[i], 0, sizeof(old[i]));

	if(!file_to_buf(filename, readbuf, sizeof(readbuf)))
	{
		printf("no buf in %s!\n", filename);
		return 0;
	}
	
        i=0;
        foreach(word, readbuf, next)
        {
                strcpy(old[i], word);
                i++;
        }
        snprintf(old[which_conn], sizeof(old[0]), "%s", str);
	
	for(i=0 ; i<MAX_CHAN; i++)
        {
                if(strcmp(old[i],""))
                        cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
                                cur == buf ? "" : " ", old[i]);
        }

#ifdef LJZ_DEBUG
	printf("filename %s commit buf=%s\n", filename, buf);
#endif	

	if(!buf_to_file(filename, buf))
		return 0;
	
	return 1;
}
										void nvram_modify_status(char *type, char *str)
{
	int i;
        char word[64], *next;
        char buf[512], *cur=buf;
        char old[MAX_CHAN][60];
	
	memset(word, 0, sizeof(word));
        memset(buf, 0, sizeof(buf));
        for(i=0; i<MAX_CHAN; i++)
                memset(old[i], 0, sizeof(old[i]));

        i=0;
        foreach(word, nvram_safe_get(type), next)
        {
                strcpy(old[i], word);
                i++;
        }
        snprintf(old[which_conn], sizeof(old[0]), "%s", str);
	
	for(i=0 ; i<MAX_CHAN; i++)
        {
                if(strcmp(old[i],""))
                        cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
                                cur == buf ? "" : " ", old[i]);
        }

#ifdef LJZ_DEBUG
	printf("type %s commit buf=%s\n", type, buf);
#endif	

        nvram_set(type, buf);
	//nvram_commit();
	return;
}
										void nvram_modify_vpivci(int vpi_value, int vci_value)
{
	int i;
        char word[256], *next;
        char buf[1000], *cur=buf;
        char old[MAX_CHAN][60];
	
	memset(word, 0, sizeof(word));
        memset(buf, 0, sizeof(buf));
        for(i=0; i<MAX_CHAN; i++)
                memset(old[i], 0, sizeof(old[i]));

        i=0;
        foreach(word, nvram_safe_get("vcc_config"), next)
        {
                strcpy(old[i], word);
                i++;
        }
        snprintf(old[which_conn], sizeof(old[0]), "%d:%d:%s:%s:%s:%s:%s:%s:%s", vpi_value, vci_value, encapmode, multiplex, qostype, vpcr, vscr, autodetect, applyonboot);
/////by songtao 2004 7.23
	syslog(LOG_INFO,"1nATM:pvc=%d,vpi=%d,vci=%d,QosType=%s",which_conn,vpi_value,vci_value,qostype);
        printf("1nATM:pvc=%d,vpi=%d,vci=%d,QosType=%s",which_conn,vpi_value,vci_value,qostype);
////////
	for(i=0 ; i<MAX_CHAN; i++)
        {
                if(strcmp(old[i],""))
                        cur += snprintf(cur, buf + sizeof(buf) - cur, "%s%s",
                                cur == buf ? "" : " ", old[i]);
        }

#ifdef LJZ_DEBUG
	printf("......commit buf=%s\n", buf);
#endif	
        nvram_set("vcc_config", buf);
	//nvram_commit();
	return;
}

int vpivci_checkused(int vpi_value, int vci_value)
{
	int i;
	int result = 0;
	for(i=0; i<MAX_CHAN; i++)
	{
		if((vpi_value == vpivci_final_value[i][0]) && 
		   (vci_value == vpivci_final_value[i][1]))
		{
			result = 1;
			break;
		}
	}
	return result;
}

#ifdef CLIP_SUPPORT
int clip_load()
{
    //if((which_conn == 0) && nvram_match("autosearchflag", "1")) // xml auto search
    char autosearchflag[16];
    memset(autosearchflag, 0, sizeof(autosearchflag));
    sprintf(autosearchflag, "autosearchflag%d", which_conn);
    if(nvram_match(autosearchflag, "1"))
    {
	int i, atm_fd;
	int vpi_value = atoi(vvpi);
	int vci_value = atoi(vvci);
	char vpivci_str[10];

	if(vpivci_checkused(vpi_value, vci_value))
	{
		printf("------%d.%d has been used------\n",  vpi_value, vci_value);
		goto autosearch_clip;
	}

	memset(vpivci_str, 0, sizeof(vpivci_str));
	sprintf(vpivci_str, "%d.%d", vpi_value, vci_value);
	//eval("atmarp", "-s", c_gateway, vpivci_str);

	if(STATICAutoDetect(vpivci_str) < 0)
	{
		printf("clip autodetect %d.%d fail\n", vpi_value, vci_value);
		eval("atmarp", "-d", c_gateway);
		printf("haha, clip del %s\nwaiting..\n", c_gateway);
		sleep(1);
		goto autosearch_clip;
	}
	else
	{
		printf("detect clip gateway!!!\n");
		vpivci_final_value[which_conn][0] = vpi_value;
		vpivci_final_value[which_conn][1] = vci_value;
		nvram_modify_vpivci(vpi_value, vci_value);
		ram_modify_status(WAN_PVC_ERRSTR, "OK");
		return 0;
	}

autosearch_clip:
	convert_from_autolist();
	for(i=0; i<MAX_CHAN; i++)
	{
		if(vpivci_value[2*i] < 0 || vpivci_value[2*i+1] < 0)
			break;
		if(vpivci_value[2*i] == 0 && vpivci_value[2*i+1] == 0)
			continue;
		if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
		{
			printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
			continue;
		}

		memset(vpivci_str, 0, sizeof(vpivci_str));
		sprintf(vpivci_str, "%d.%d", vpivci_value[2*i], vpivci_value[2*i+1]);
		//eval("atmarp", "-s", c_gateway, vpivci_str);
		//printf("haha, clip add %s %s\nwaiting..\n", c_gateway, vpivci_str);
		//sleep(2);

		if(STATICAutoDetect(vpivci_str) < 0)
		{
			printf("clip autodetect %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
			eval("atmarp", "-d", c_gateway);
			printf("haha, clip del %s %s\nwaiting..\n", c_gateway, vpivci_str);
			sleep(1);
			continue;
		}
		else
		{
			printf("detect clip gateway!!!\n");
			vpivci_final_value[which_conn][0] = vpivci_value[2*i];
			vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
			nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
			ram_modify_status(WAN_PVC_ERRSTR, "OK");
			return 0;
		}
	}
	printf("has no vcc available\n");
     	
	{
	char ifname[5];
	memset(ifname, 0, sizeof(ifname));
	sprintf(ifname, "atm%d", which_conn);
	eval("ifconfig", ifname, "down");
	atmarpd_index &= ~(1<<which_conn);
	if(!atmarpd_index)
		eval("killall", "atmarpd");
        }

/*
	atmarpd_index &= ~(1<<which_conn);
	if(!atmarpd_index)
		eval("killall", "atmarpd");
*/	
	ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
	return -1;
    }
    else
    {
	int i, atm_fd;
	char word[40], lastsuccess[3], lastindex[3], indextmp[3], *next;
	int which = which_conn;
	char vpivci_str[10];

	memset(word, 0, sizeof(word));
	memset(lastsuccess, 0, sizeof(lastsuccess));
	memset(lastindex, 0, sizeof(lastindex));
	memset(indextmp, 0, sizeof(indextmp));

	foreach(word, nvram_safe_get("wan_autoresult"), next)
	{
		if(which-- == 0)
		{
			sscanf(word, "%3[^:]:%3[^\n]", lastsuccess, lastindex);
		#ifdef LJZ_DEBUG
			printf("lastsuccess %s, lastindex %s\n", lastsuccess, lastindex);
		#endif
			break;
		}
	}

clip_begin:
	for(i=0; i<MAX_CHAN; i++)
	{
		if((!strcmp(lastsuccess, "1")) && (strcmp(lastindex, "-1")))
		{
			strncpy(indextmp, lastindex, sizeof(indextmp));
			if(i != atoi(lastindex))
				continue;
		}

		if((!strcmp(lastsuccess, "1")) && (!strcmp(lastindex, "-1")))
		{
			if(i == atoi(indextmp))
				continue;
		}

		if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
		{
			printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
			continue;
		}

	#ifdef LJZ_DEBUG
		printf("vpivci_value %d.%d\n", vpivci_value[2*i], vpivci_value[2*i+1]);
	#endif

	#ifdef LJZ_DEBUG
		printf("before clipautodetect\n");
	#endif

		memset(vpivci_str, 0, sizeof(vpivci_str));
		sprintf(vpivci_str, "%d.%d", vpivci_value[2*i], vpivci_value[2*i+1]);
		//eval("atmarp", "-s", c_gateway, vpivci_str);
		//printf("haha, clip add %s %s\nwaiting..\n", c_gateway, vpivci_str);
		//sleep(2);
		if(STATICAutoDetect(vpivci_str) < 0)
		{
			printf("clip autodetect %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
			eval("atmarp", "-d", c_gateway);
			printf("haha, clip del %s %s\nwaiting..\n", c_gateway, vpivci_str);
			sleep(1);
			continue;
		}
		else
		{
			printf("detect clip gateway!!!\n");
			vpivci_final_value[which_conn][0] = vpivci_value[2*i];
			vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
			nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
			break;
		}
	}
	if(i==MAX_CHAN)
	{
		printf("has no vcc available\n");
		if(!strcmp(lastsuccess, "1"))
		{
			if(strcmp(lastindex, "-1"))
			{
				memset(lastindex, 0, sizeof(lastindex));
				strcpy(lastindex, "-1");
				goto clip_begin;
			}
		}
		//nvram_modify_status("wan_pvc_errstr", "no_vcc_available");
	 	ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
 	   
	    {
		char ifname[5];
		memset(ifname, 0, sizeof(ifname));
		sprintf(ifname, "atm%d", which_conn);
		eval("ifconfig", ifname, "down");
		atmarpd_index &= ~(1<<which_conn);
		if(!atmarpd_index)
			eval("killall", "atmarpd");
	    }

	/*
		atmarpd_index &= ~(1<<which_conn);
		if(!atmarpd_index)
			eval("killall", "atmarpd");
	*/	
		return -1;
	}

#ifdef LJZ_DEBUG
	printf("clipautodetect ok!!\n");
#endif
    {//success autodetect
	char buf[5];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d:%d", 1, i);
	nvram_modify_status("wan_autoresult", buf);
    }

	//nvram_modify_status("wan_pvc_errstr", "ok");
	ram_modify_status(WAN_PVC_ERRSTR, "OK");
	return 0;
    }
}
#endif
				
int pppoa_load()
{
    //if((which_conn == 0) && nvram_match("autosearchflag", "1")) // xml auto search
    char autosearchflag[16];
    memset(autosearchflag, 0, sizeof(autosearchflag));
    sprintf(autosearchflag, "autosearchflag%d", which_conn);
    if(nvram_match(autosearchflag, "1"))
    {
	int i, atm_fd;
	int vpi_value = atoi(vvpi);
	int vci_value = atoi(vvci);
	if(vpivci_checkused(vpi_value, vci_value))
	{
		printf("------%d.%d has been used------\n",  vpi_value, vci_value);
		goto autosearch_pppoa;
	}
	atm_fd = establish_atmchan(vpi_value, vpi_value);
	if(atm_fd < 0)
	{
		printf("establish_atmchan fail\n");
		goto autosearch_pppoa;
	}

	if(PPPOAAutoDetect(atm_fd) < 0)
	{
		printf("pppoa autodetect %d.%d fail\n", vpi_value, vci_value);
		close(atm_fd);
		goto autosearch_pppoa;
	}
	else
	{
		printf("detect pppoa server!!!\n");
		close(atm_fd);
		vpivci_final_value[which_conn][0] = vpi_value;
		vpivci_final_value[which_conn][1] = vci_value;
		nvram_modify_vpivci(vpi_value, vci_value);
		ram_modify_status(WAN_PVC_ERRSTR, "OK");
		return 0;
	}

autosearch_pppoa:
	convert_from_autolist();
	for(i=0; i<MAX_CHAN; i++)
	{
		if(vpivci_value[2*i] < 0 || vpivci_value[2*i+1] < 0) 
			break;
		if(vpivci_value[2*i] == 0 && vpivci_value[2*i+1] == 0)
			continue;
		if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
		{
			printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
			continue;
		}

		atm_fd = establish_atmchan(vpivci_value[2*i], vpivci_value[2*i+1]);
		if(atm_fd < 0)
		{
			printf("establish_atmchan fail\n");
			continue;
		}

		if(PPPOAAutoDetect(atm_fd) < 0)
		{
			printf("pppoa autodetect %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
			close(atm_fd);
			continue;
		}
		else
		{
			printf("detect pppoa server!!!\n");
			close(atm_fd);
			vpivci_final_value[which_conn][0] = vpivci_value[2*i];
			vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
			nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
			ram_modify_status(WAN_PVC_ERRSTR, "OK");
			return 0;
		}
	}
	printf("has no vcc available\n");
	ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
	return -1;
    }
    else
    {
	int i, atm_fd;
	char word[40], lastsuccess[3], lastindex[3], indextmp[3], *next;
	int which = which_conn;

	memset(word, 0, sizeof(word));
	memset(lastsuccess, 0, sizeof(lastsuccess));
	memset(lastindex, 0, sizeof(lastindex));
	memset(indextmp, 0, sizeof(indextmp));

	foreach(word, nvram_safe_get("wan_autoresult"), next)
	{
		if(which-- == 0)
		{
			sscanf(word, "%3[^:]:%3[^\n]", lastsuccess, lastindex);
		#ifdef LJZ_DEBUG
			printf("lastsuccess %s, lastindex %s\n", lastsuccess, lastindex);
		#endif
			break;
		}
	}

pppoa_begin:
	for(i=0; i<MAX_CHAN; i++)
	{
		if((!strcmp(lastsuccess, "1")) && (strcmp(lastindex, "-1")))
		{
			strncpy(indextmp, lastindex, sizeof(indextmp));
			if(i != atoi(lastindex))
				continue;
		}

		if((!strcmp(lastsuccess, "1")) && (!strcmp(lastindex, "-1")))
		{
			if(i == atoi(indextmp))
				continue;
		}

		if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
		{
			printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
			continue;
		}

	#ifdef LJZ_DEBUG
		printf("vpivci_value %d.%d\n", vpivci_value[2*i], vpivci_value[2*i+1]);
	#endif
		atm_fd = establish_atmchan(vpivci_value[2*i], vpivci_value[2*i+1]);
		if(atm_fd < 0)
		{
			printf("establish_atmchan fail\n");
			continue;
		}

	#ifdef LJZ_DEBUG
		printf("before pppoaautodetect\n");
	#endif
		if(PPPOAAutoDetect(atm_fd) < 0)
		{
			printf("pppoa autodetect %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
			close(atm_fd);
			continue;
		}
		else
		{
			printf("detect pppoa server!!!\n");
			close(atm_fd);
			vpivci_final_value[which_conn][0] = vpivci_value[2*i];
			vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
			nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
			break;
		}
	}
	if(i==MAX_CHAN)
	{
		printf("has no vcc available\n");
		if(!strcmp(lastsuccess, "1"))
		{
			if(strcmp(lastindex, "-1"))
			{
				memset(lastindex, 0, sizeof(lastindex));
				strcpy(lastindex, "-1");
				goto pppoa_begin;
			}
		}
		//nvram_modify_status("wan_pvc_errstr", "no_vcc_available");
		ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
		return -1;
	}

#ifdef LJZ_DEBUG
	printf("pppoaautodetect ok!!\n");
#endif
    {//success autodetect
	char buf[5];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d:%d", 1, i);
	nvram_modify_status("wan_autoresult", buf);
    }

	//nvram_modify_status("wan_pvc_errstr", "ok");
	ram_modify_status(WAN_PVC_ERRSTR, "OK");
	return 0;
    }
}

int establish_atmchan(int vpi_value, int vci_value)
{
	struct sockaddr_atmpvc addr;
	struct atm_qos qos;
	int fd;
	int err = -1;
	int traffic_type = 1;

    	if (!strcmp(qostype, "ubr"))
       		traffic_type = 1;
    	else if (!strcmp(qostype,"cbr"))
         	traffic_type = 2;
    	else if (!strcmp(qostype, "vbr"))
         	traffic_type = 3;

	memset(&addr, 0, sizeof(addr));

	addr.sap_family = AF_ATMPVC;
    	addr.sap_addr.itf = 0;
    	addr.sap_addr.vpi = vpi_value;
    	addr.sap_addr.vci = vci_value;
#ifdef LJZ_DEBUG
	printf("before socket\n");
#endif
	fd = socket(AF_ATMPVC, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		printf("socket create fail\n");
		return err;
	}

	memset(&qos, 0, sizeof(qos));

	qos.aal = ATM_AAL5;
	qos.txtp.traffic_class = traffic_type;
    	qos.txtp.max_sdu = 1500;
	if(!strcmp(qostype, "vbr") || !strcmp(qostype, "cbr"))
		qos.txtp.pcr = atoi(vpcr);
	else
		qos.txtp.pcr = 0;
	if(!strcmp(qostype, "vbr"))
		qos.txtp.scr = atoi(vscr);
	else
		qos.txtp.scr = 0;

    	qos.rxtp = qos.txtp;

#ifdef LJZ_DEBUG
	printf("before SOL_ATMQOS\n");
#endif
	if(setsockopt(fd, SOL_ATM, SO_ATMQOS, &qos, sizeof(qos)) < 0)
	{
		printf("setsockopt SO_ATMQOS fail\n");
		close(fd);
		return err;
	}
#ifdef LJZ_DEBUG
	printf("before connect\n");
#endif
	if(connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_atmpvc)) < 0)
	{
		printf("connect fail\n");
		close(fd);
		return err;
	}
	return fd;
}

void convert_from_autolist()
{
	int i;
	char buf[11], *next, vpi_string[4], vci_string[6];
	char autosearchlist[16], *tmp, *tmpstr;
	//memset(buf, 0, sizeof(buf));
	memset(autosearchlist, 0, sizeof(autosearchlist));
	sprintf(autosearchlist, "autosearchlist%d", which_conn);
	tmpstr = nvram_safe_get(autosearchlist);
	for(i=0; i<MAX_CHAN; i++)
	{	
		tmp = strsep(&tmpstr, " ");
		memset(vpi_string, 0, sizeof(vpi_string));
		memset(vci_string, 0, sizeof(vci_string));
		sscanf(tmp, "%4[^:]:%6[^\n]", vpi_string, vci_string);
		printf("%d: vpi %s vci %s\n", i, vpi_string, vci_string);
		if(!strcmp(vpi_string, "-1"))
			vpivci_value[2*i] = -1;
		else	
			vpivci_value[2*i] = atoi(vpi_string);
		if(!strcmp(vci_string, "-1"))
			vpivci_value[2*i+1] = -1;
		else
			vpivci_value[2*i+1] = atoi(vci_string);
	}
	//foreach(buf, nvram_safe_get("autosearchlist"), next)
	/*
	foreach(buf, nvram_safe_get(autosearchlist), next)
	{
		memset(vpi_string, 0, sizeof(vpi_string));
		memset(vci_string, 0, sizeof(vci_string));
		sscanf(buf, "%4[^:]:%6[^\n]", vpi_string, vci_string);
		//printf("%d: vpi %s vci %s\n", i, vpi_string, vci_string);
		if(!strcmp(vpi_string, "-1"))
			vpivci_value[2*i] = -1;
		else	
			vpivci_value[2*i] = atoi(vpi_string);
		if(!strcmp(vci_string, "-1"))
			vpivci_value[2*i+1] = -1;
		else
			vpivci_value[2*i+1] = atoi(vci_string);
		if(i++ == 7)
			break;
		memset(buf, 0, sizeof(buf));
	}
	*/
	//for(i=0; i<8; i++)
	//	printf("%d:%d ", vpivci_value[2*i], vpivci_value[2*i+1]);
	return;
}

//int autosearch_wt082(int vpivalue, int vcivalue, int vc_fd)
//int autosearch_wt082(int vpivalue, int vcivalue, int vc_fd)
#ifdef VZ7
int autosearch_wt082(int vpivalue, int vcivalue, int vc_fd, int times)
#else
int autosearch_wt082(int vpivalue, int vcivalue, int vc_fd)
#endif
{
	int static_detect = 0;
	//printf("autosearch_wt082  %d:%d\n", vpivalue, vcivalue);
	if(!strcmp(encapmode, "pppoe"))
	{
	#ifdef VZ7
	    if(times == 0)
            {
	#endif
		if(PPPOEAutoDetect() < 0)
		{
			if(vc_fd > 0)
			{
				close(vc_fd);
				vc_fd = -1;
			}
			return -1;
		}
		else
		{
			printf("detect pppoe server!!!\n");
			sockfd_used[which_conn] = vc_fd;
			vpivci_final_value[which_conn][0] = vpivalue;
			vpivci_final_value[which_conn][1] = vcivalue;
			nvram_modify_vpivci(vpivalue, vcivalue);
			return 0;
		}
	#ifdef VZ7
	    }
	    else
	    {
		int ret;
		//junzhao 2005.2.5
		//if(DHCPAutoDetect() < 0)
		if(_DHCPAutoDetect() < 0)
		{
			printf("detect dhcp server fail!!!\n");
			ret = -1;
		}
		else
		{
			printf("detect dhcp server!!!\n");
			ret = 0;
		}
		if(vc_fd > 0)
		{
			close(vc_fd);
			vc_fd = -1;
		}
		return ret;
	    }
	#endif
	}
	else if(!strcmp(encapmode, "1483bridged")) //1483bridged
	{
		if(!strcmp(dhcpenable, "1")) //autodetect dhcp
		{
			printf("now will begin dhcp autodetect\n");

			//junzhao 2005.2.5
			sockfd_used[which_conn] = vc_fd;
			vpivci_final_value[which_conn][0] = vpivalue;
			vpivci_final_value[which_conn][1] = vcivalue;
			
			if(DHCPAutoDetect() < 0)
			{
				//junzhao 2005.2.5
				stop_dhcpc(which_conn);
				if(vc_fd > 0)
				{
					close(vc_fd);
					vc_fd = -1;
				}
				return -1;
			}
			else
			{
				printf("detect dhcp server!!!\n");
				//junzhao 2005.2.5
				//sockfd_used[which_conn] = vc_fd;
				//vpivci_final_value[which_conn][0] = vpivalue;
				//vpivci_final_value[which_conn][1] = vcivalue;
				nvram_modify_vpivci(vpivalue, vcivalue);
				return 0;
			}
		}
		else //bridged static autodetect
			static_detect = 1;
	}
	else if(!strcmp(encapmode, "routed") || static_detect == 1)
	{
		//autodetect static
		printf("Static autodetect begin\n");
		if(STATICAutoDetect(NULL) < 0)
		{
			printf("Static autodetect fail\n");
			if(vc_fd > 0)
			{
				close(vc_fd);
				vc_fd = -1;
			}
			return -1;
		}
		else
		{
			printf("detect static gateway!!!\n");
			sockfd_used[which_conn] = vc_fd;
			vpivci_final_value[which_conn][0] = vpivalue;
			vpivci_final_value[which_conn][1] = vcivalue;
			nvram_modify_vpivci(vpivalue, vcivalue);
			return 0;
		}
	}

	return -1;
}


int rfc2684_load(int autoway)
{
	int encaps;
    	int traffic_type = 1;
   	int pcr_int, scr_int;
	int vpi_value, vci_value;
	int err = -1;
	int vc_fd = -1;

	//pppoa shouldn't come here!!!
#ifdef CLIP_SUPPORT
	if((!strcmp(encapmode, "pppoa")) || (!strcmp(encapmode, "clip")))
#else
	if(!strcmp(encapmode, "pppoa"))
#endif
	{
		printf("shouldn't come here\n");
		return -1;
	}

   	/* Set Program Arguments/Parameters except vpi and vci value */

    	if (!strcmp(qostype,"ubr"))
       		traffic_type = 1;
    	else if (!strcmp(qostype,"cbr"))
         	traffic_type = 2;
    	else if (!strcmp(qostype,"vbr"))
         	traffic_type = 3;

//	if(strcmp(vpcr, ""))
	if(vpcr != NULL)
    		pcr_int = atoi(vpcr);
	else
		pcr_int = 0;
//	if(strcmp(vscr, ""))
	if(vscr != NULL)
    		scr_int = atoi(vscr);
	else
		scr_int = 0;

#ifdef UNNUMBERED_SUPPORT
	if(((!strcmp(encapmode, "1483bridged"))||(!strcmp(encapmode, "pppoe"))||(!strcmp(encapmode, "unpppoe")) ||(!strcmp(encapmode, "bridgedonly"))) && (!strcmp(multiplex, "llc"))) /* kirby */
#else
	if(((!strcmp(encapmode, "1483bridged"))||(!strcmp(encapmode, "pppoe"))||(!strcmp(encapmode, "bridgedonly"))) && (!strcmp(multiplex, "llc")))
#endif
		encaps = 0;
#ifdef UNNUMBERED_SUPPORT
	else if(((!strcmp(encapmode, "1483bridged"))||(!strcmp(encapmode, "pppoe"))||(!strcmp(encapmode, "unpppoe"))||(!strcmp(encapmode, "bridgedonly"))) && (!strcmp(multiplex, "vc"))) /* kirby */
#else
	else if(((!strcmp(encapmode, "1483bridged"))||(!strcmp(encapmode, "pppoe"))||(!strcmp(encapmode, "bridgedonly"))) && (!strcmp(multiplex, "vc")))
#endif
		encaps = 1;
	else if(!strcmp(encapmode, "routed") && !strcmp(multiplex, "llc"))
		encaps = 3;
	else if(!strcmp(encapmode, "routed") && !strcmp(multiplex, "vc"))
		encaps = 4;
	else
		encaps = 0;

	if(autoway == AUTODISABLE) //hand set!!
	{
	#ifdef LJZ_DEBUG
		printf("enter hand set\n");
	#endif
		vpi_value = atoi(vvpi);
		vci_value = atoi(vvci);
	#ifdef LJZ_DEBUG
    		printf("vpi_value=%d, vci_value=%d\n", vpi_value, vci_value);
	#endif

		//junzhao 2004.8.26 assert vpi_vci pair not 0.0
		if(!vpi_value && !vci_value)
		{
			printf("vpivci value %d.%d not allowed, Please change!!\n", vpi_value, vci_value);
			ram_modify_status(WAN_PVC_ERRSTR, "VpiVci(0.0)_Not_Allowed");
			return -1;
		}
	
		// see if been used
		if(vpivci_checkused(vpi_value, vci_value))
		{
			printf("vpivci value %d.%d have been used! Please change!!\n", vpi_value, vci_value);
			//nvram_modify_status("wan_pvc_errstr", "vpivci_value_conflict");
			ram_modify_status(WAN_PVC_ERRSTR, "VpiVci_Value_Conflict");
			return -1;
		}

		/* create bridge interface */
    		err = create_br(which_conn);
		if(err < 0)
		{
			printf("create br fail\n");
			//nvram_modify_status("wan_pvc_errstr", "create_bridge_fail");
			ram_modify_status(WAN_PVC_ERRSTR, "Create_Bridge_Fail");
			return err;
		}
		/* assign vcc param */
		vc_fd = assign_vcc(vpi_value, vci_value, encaps, traffic_type, pcr_int, scr_int, sndbuf);
    		if(vc_fd < 0)
		{
    			printf("assign vcc fail\n");
			//nvram_modify_status("wan_pvc_errstr", "assign_vcc_fail");
			ram_modify_status(WAN_PVC_ERRSTR, "Assign_Vcc_Fail");
			return vc_fd;
		}
		printf("2684 load ok!\n");
	#ifdef VZ7		
	{
	   char pppoe_dhcp_enable[20];
	   memset(pppoe_dhcp_enable, 0, sizeof(pppoe_dhcp_enable));
	   sprintf(pppoe_dhcp_enable, "pppoe_dhcp_enable%d", which_conn);
	   if(nvram_match(pppoe_dhcp_enable, "1"))
           {		   
		if(strcmp(encapmode, "pppoe") == 0) //pppoe proto
		{
			if(PPPOEAutoDetect() < 0)
			{
				//junzhao 2004.8.17
				if(DHCPAutoDetect() < 0)
				{
					//junzhao 2005.2.5
					stop_dhcpc(which_conn);
					printf("dhcp server autodetect fail\n");
				}
				else
					printf("dhcp server autodetect success\n");
				if(vc_fd > 0)
				{
					close(vc_fd);
					vc_fd = -1;
				}
				return -2;	
			}
		}
	   }
	}
	#endif

		vpivci_final_value[which_conn][0]  = vpi_value;
		vpivci_final_value[which_conn][1]  = vci_value;
		sockfd_used[which_conn] = vc_fd;
		//nvram_modify_status("wan_pvc_errstr", "ok");
		ram_modify_status(WAN_PVC_ERRSTR, "OK");
		return vc_fd;
	}
	else //autodetect
	{
	    char autosearchflag[16];
	    memset(autosearchflag, 0, sizeof(autosearchflag));
	    sprintf(autosearchflag, "autosearchflag%d", which_conn);
	    //junzhao 2004.6.16 for autosearch on xml
	    //if((which_conn == 0) && nvram_match("autosearchflag", "1")) // xml auto search
	    if(nvram_match(autosearchflag, "1")) // xml auto search
	    {
	    	int i;
	    	vpi_value = atoi(vvpi);
		vci_value = atoi(vvci);
	#ifdef LJZ_DEBUG
    		printf("vpi_value=%d, vci_value=%d\n", vpi_value, vci_value);
	#endif
		// see if been used
		if(vpivci_checkused(vpi_value, vci_value))
		{
			printf("vpivci value %d.%d have been used! Please change!!\n", vpi_value, vci_value);
			goto autosearch_xml;
		}

		/* create bridge interface */
    		err = create_br(which_conn);
		if(err < 0)
		{
			printf("create br fail\n");
			ram_modify_status(WAN_PVC_ERRSTR, "Create_Bridge_Fail");
			return err;
		}
		/* assign vcc param */
		vc_fd = assign_vcc(vpi_value, vci_value, encaps, traffic_type, pcr_int, scr_int, sndbuf);
    		if(vc_fd < 0)
		{
    			printf("assign vcc fail\n");
			goto autosearch_xml;
		}
	#ifdef VZ7
		if(autosearch_wt082(vpi_value, vci_value, vc_fd, 0) == 0)
	#else
		if(autosearch_wt082(vpi_value, vci_value, vc_fd) == 0)
	#endif
		{
			ram_modify_status(WAN_PVC_ERRSTR, "OK");
			return vc_fd;
		}
autosearch_xml:
		convert_from_autolist();
		for(i=0; i<MAX_CHAN; i++)
		{
			if(vpivci_value[2*i] < 0 || vpivci_value[2*i+1] < 0)
				break;
			if(vpivci_value[2*i] == 0 && vpivci_value[2*i+1] == 0)
				continue;

			if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
			{
				printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
				continue;						                 }

			/* create bridge interface */
    			err = create_br(which_conn);
			if(err < 0)
			{
				printf("create br fail\n");
				ram_modify_status(WAN_PVC_ERRSTR, "Create_Bridge_Fail");
				return err;
			}
			printf("%d %d %d %d %d %d %d\n", vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);
			/* assign vcc param */
			vc_fd = assign_vcc(vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);

			if(vc_fd < 0)
			{
    				printf("assign vcc %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
				continue;
			}
		#ifdef VZ7
			if(autosearch_wt082(vpivci_value[2*i], vpivci_value[2*i+1], vc_fd, 0) == 0)
		#else
			if(autosearch_wt082(vpivci_value[2*i], vpivci_value[2*i+1], vc_fd) == 0)
		#endif
			{
				ram_modify_status(WAN_PVC_ERRSTR, "OK");
				return vc_fd;
			}
		}

		printf("have no vcc available\n");
			//nvram_modify_status("wan_pvc_errstr", "have_no_vcc_available");
		ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
	
		//junzhao 2005.2.5
		sockfd_used[which_conn] = -1;
		vpivci_final_value[which_conn][0] = -1;
		vpivci_final_value[which_conn][1] = -1;
		
	#ifdef VZ7
		if(!strcmp(encapmode, "pppoe"))
		{
			for(i=0; i<MAX_CHAN; i++)
			{
				if(vpivci_value[2*i] < 0 || vpivci_value[2*i+1] < 0)
					break;
				if(vpivci_value[2*i] == 0 && vpivci_value[2*i+1] == 0)
					continue;

				if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
				{
					printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
					continue;						                 }

				/* create bridge interface */
    				err = create_br(which_conn);
				if(err < 0)
				{
					printf("create br fail\n");
					break;
				}
				printf("%d %d %d %d %d %d %d\n", vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);
				/* assign vcc param */
				vc_fd = assign_vcc(vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);

				if(vc_fd < 0)
				{
    					printf("assign vcc %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
					continue;
				}
				if(autosearch_wt082(vpivci_value[2*i], vpivci_value[2*i+1], vc_fd, 1) == 0)
				{
					break;
				}
			}
		}
	#endif
		return -1;
	    }
	    else // httpd set
	    {
		int i;
		char word[40], lastsuccess[3], lastindex[3], indextmp[3], *next;
		int which = which_conn;

		memset(word, 0, sizeof(word));
		memset(lastsuccess, 0, sizeof(lastsuccess));
		memset(lastindex, 0, sizeof(lastindex));
		memset(indextmp, 0, sizeof(indextmp));

		foreach(word, nvram_safe_get("wan_autoresult"), next)
	        {
			if(which-- == 0)
			{
				sscanf(word, "%3[^:]:%3[^\n]", lastsuccess, lastindex);
			#ifdef LJZ_DEBUG
				printf("lastsuccess %s, lastindex %s\n", lastsuccess, lastindex);
			#endif
				break;
			}
		}

	#ifdef LJZ_DEBUG
		printf("enter autodetect\n");
	#endif

begin_search:
		for(i=0; i<MAX_CHAN; i++)
		{
			//sleep(5);
		#ifdef LJZ_DEBUG
			//printf("hey the index is %d\n", i);
		#endif

			if((!strcmp(lastsuccess, "1")) && (strcmp(lastindex, "-1")))
			{
				strncpy(indextmp, lastindex, sizeof(indextmp));
				if(i != atoi(lastindex))
				       continue;
			}

			if((!strcmp(lastsuccess, "1")) && (!strcmp(lastindex, "-1")))
			{
				if(i == atoi(indextmp))
				       continue;
			}

			if(vpivci_checkused(vpivci_value[2*i], vpivci_value[2*i+1]))
			{
				printf("------index %d(%d.%d) has been used------\n", i, vpivci_value[2*i], vpivci_value[2*i+1]);
				continue;						                 }

			/* create bridge interface */
    			err = create_br(which_conn);
			if(err < 0)
			{
				printf("create br fail\n");
				//nvram_modify_status("wan_pvc_errstr", "create_bridge_fail");
				ram_modify_status(WAN_PVC_ERRSTR, "Create_Bridge_Fail");
				return err;
			}
			printf("%d %d %d %d %d %d %d\n", vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);
			/* assign vcc param */
			vc_fd = assign_vcc(vpivci_value[2*i], vpivci_value[2*i+1], encaps, traffic_type, pcr_int, scr_int, sndbuf);

			if(vc_fd < 0)
			{
    				printf("assign vcc %d.%d fail\n", vpivci_value[2*i], vpivci_value[2*i+1]);
				continue;
			}

			if(!strcmp(encapmode, "pppoe"))
			{
				//pppoe autodetect
				printf("now will begin pppoe autodetect\n");
				if(PPPOEAutoDetect() < 0)
				{
					if(vc_fd > 0)
					{
						close(vc_fd);
						vc_fd = -1;
					}
					continue;
				}
				else
				{
					printf("detect pppoe server!!!\n");
					sockfd_used[which_conn] = vc_fd;
					vpivci_final_value[which_conn][0] = vpivci_value[2*i];
					vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
					nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
					break;
				}
			}
			/* kirby for unip 2004/11.26 */
#ifdef UNNUMBERED_SUPPORT
                     else if(!strcmp(encapmode, "unpppoe"))
			{
				//pppoe autodetect
				printf("now will begin unip pppoe autodetect\n");
				if(unip_PPPOEAutoDetect() < 0)
				{
					if(vc_fd > 0)
					{
						close(vc_fd);
						vc_fd = -1;
					}
					continue;
				}
				else
				{
					printf("detect unip pppoe server!!!\n");
					sockfd_used[which_conn] = vc_fd;
					vpivci_final_value[which_conn][0] = vpivci_value[2*i];
					vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
					nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
					break;
				}
			}
#endif
			else if(!strcmp(encapmode, "1483bridged")) //1483bridged
			{
				if(!strcmp(dhcpenable, "1")) //autodetect dhcp
				{
					printf("now will begin dhcp autodetect\n");
					//junzhao 2005.2.5
					sockfd_used[which_conn] = vc_fd;
					vpivci_final_value[which_conn][0] = vpivci_value[2*i];
					vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];

					if(DHCPAutoDetect() < 0)
					{
						//junzhao 2005.2.5
						stop_dhcpc(which_conn);
						if(vc_fd > 0)
						{
							close(vc_fd);
							vc_fd = -1;
						}
						continue;
					}
					else
					{
						printf("detect dhcp server!!!\n");
						//junzhao 2005.2.5
						//sockfd_used[which_conn] = vc_fd;
						//vpivci_final_value[which_conn][0] = vpivci_value[2*i];
						//vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
						nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
						break;
					}
				}
				else //bridged static autodetect
					goto static_detect;
			}
			else if(!strcmp(encapmode, "routed"))
			{
				//autodetect static
static_detect:			printf("Static autodetect begin\n");
				if(STATICAutoDetect(NULL) < 0)
				{
					printf("Static autodetect fail\n");
					if(vc_fd > 0)
					{
						close(vc_fd);
						vc_fd = -1;
					}
					continue;
				}
				else
				{
					printf("detect static gateway!!!\n");
					sockfd_used[which_conn] = vc_fd;
					vpivci_final_value[which_conn][0] = vpivci_value[2*i];
					vpivci_final_value[which_conn][1] = vpivci_value[2*i+1];
					nvram_modify_vpivci(vpivci_value[2*i], vpivci_value[2*i+1]);
					break;
				}
			}
		}
		if(i == MAX_CHAN)
		{
			printf("have no vcc available\n");
			if(!strcmp(lastsuccess, "1"))
			{
				if(strcmp(lastindex, "-1"))
				{
					memset(lastindex, 0, sizeof(lastindex));
					strcpy(lastindex, "-1");
					goto begin_search;
				}
			}
			//nvram_modify_status("wan_pvc_errstr", "have_no_vcc_available");
			ram_modify_status(WAN_PVC_ERRSTR, "Have_No_Vcc_Available");
			
			//junzhao 2005.2.5
			sockfd_used[which_conn] = -1;
			vpivci_final_value[which_conn][0] = -1;
			vpivci_final_value[which_conn][1] = -1;
			return -1;
		}
	    {//success autodetect
		char buf[5];
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%d:%d", 1, i);
		nvram_modify_status("wan_autoresult", buf);
	    }
		//nvram_modify_status("wan_pvc_errstr", "ok");
		ram_modify_status(WAN_PVC_ERRSTR, "OK");
		return vc_fd;
            }
	}
}

int create_br(int nrum)
{
    	int err;
    	int itfsock = -1;

#ifdef LJZ_DEBUG
	printf("interface num %d\n", nrum);
#endif

        itfsock = socket(PF_ATMPVC, SOCK_DGRAM, ATM_AAL5);
    	if (itfsock < 0)
	{
        	perror("socket creation");
		return -1;
	}
    	else 
    	{
        	/* create the device with ioctl: */
        	if( nrum>=0 && nrum<8)
		{
            		struct atm_newif_br2684 ni;
            		ni.backend_num = ATM_BACKEND_BR2684;
            		ni.media = BR2684_MEDIA_ETHERNET;
            		ni.mtu = 1500;
            		sprintf(ni.ifname, "nas%d", nrum);
            		err=ioctl(itfsock, ATM_NEWBACKENDIF, &ni);
		#ifdef LJZ_DEBUG	
            		printf("create:%d\n",err);
		#endif
            		if (err < 0)
			{
            			fprintf (stderr, "ioctl ATM_NEWBACKENDIF fail\n");
				return -1;
			}	
			if(itfsock > 0)
				close(itfsock);
        	} 
		else
		{
           		fprintf (stderr, "err: wrong itf number %d", nrum);
    			return -1;
		}
	}
    	return 0;
}

int assign_vcc(int vpi_value, int vci_value, int encap, int qos_type, int pcr_value, int scr_value, int bufsize)
{
    	struct ifreq ifr;
    	int fd = -1;
    	int err = 0, errno;
    	struct atm_qos qos;
   	struct sockaddr_atmpvc addr;
    	int skfd = -1;
    	struct atm_backend_br2684 be;
//	char astr[] = "0.0.35";
	
	printf("vpi:vci:encap:qos_type:pcr:scr:bufsize: %d %d %d %d %d %d %d\n", vpi_value, vci_value, encap, qos_type, pcr_value, scr_value, bufsize);
	
	memset(&addr, 0, sizeof(addr));
//	err=text2atm(astr,(struct sockaddr *)(&addr), sizeof(addr), T2A_PVC);
//         	printf("text2atm:%d\n",err);
#if 1
	addr.sap_family = AF_ATMPVC;
    	addr.sap_addr.itf = 0;
    	addr.sap_addr.vpi = vpi_value;
    	addr.sap_addr.vci = vci_value;
#endif
	
    	fprintf(stderr,"Communicating over ATM %d.%d.%d\n", 
			addr.sap_addr.itf, 
			addr.sap_addr.vpi, 
			addr.sap_addr.vci);

    	if ((fd = socket(PF_ATMPVC, SOCK_DGRAM, ATM_AAL5)) < 0)
	{
       		fprintf(stderr,"failed to create socket %d", errno);
		err = -1;
		goto finalerr;
	}

    	memset(&qos, 0, sizeof(qos));
    	qos.aal                     = ATM_AAL5;
    	qos.txtp.traffic_class      = qos_type;
    	qos.txtp.max_sdu            = 1524;
	if((qos_type == 2) || (qos_type == 3))
		qos.txtp.pcr = pcr_value;
	else
		qos.txtp.pcr = 0;
	if(qos_type == 3)
		qos.txtp.scr = scr_value;
	else
		qos.txtp.scr = 0;
    	qos.rxtp = qos.txtp;

        err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, 
			&bufsize ,sizeof(bufsize));
	if(err < 0)
	{
        	fprintf(stderr,"setsockopt SO_SNDBUF: (%d) %s\n",
			err, strerror(err));
		goto finalerr;
	}
    	err = setsockopt(fd, SOL_ATM, SO_ATMQOS, &qos, sizeof(qos));
	if(err < 0)
	{
        	fprintf(stderr,"setsockopt SO_ATMQOS %d", errno);
		goto finalerr;
	}
       	err = connect(fd, (struct sockaddr*)&addr, 
			sizeof(struct sockaddr_atmpvc));
       	if (err < 0)
       	{
           	fprintf(stderr,"Vic's failed to connect on socket: %d\n", err);
           	fprintf(stderr,"Closing socket Game over \n");
		goto finalerr;
       }
       
       /* attach the vcc to device: */
    	be.backend_num = ATM_BACKEND_BR2684;
    	be.ifspec.method = BR2684_FIND_BYIFNAME;
    	sprintf(be.ifspec.spec.ifname, "nas%d", which_conn);
    	be.fcs_in = BR2684_FCSIN_NO;
    	be.fcs_out = BR2684_FCSOUT_NO;
    	be.fcs_auto = 0;
/* mwh   be.encaps = encap ? BR2684_ENCAPS_VC : BR2684_ENCAPS_LLC; */
    	switch(encap) 
	{
		case 0:
			be.encaps = BR2684_ENCAPS_LLC;
			break;
		case 1:
			be.encaps = BR2684_ENCAPS_VC;
			break;
		case 2:
			fprintf(stderr, "Autodetect unsupported - defaulting to  Bridged LLC!! \n");
			be.encaps = BR2684_ENCAPS_LLC;
			break;
		case 3:
			be.encaps = BR2684_ENCAPS_LLC_ROUTED;
			break;
		case 4:
			be.encaps = BR2684_ENCAPS_VC_ROUTED;
			break;
		default:
			fprintf(stderr, "Bad encapsulation - defaulting to  Bridged LLC!! \n");
			be.encaps = BR2684_ENCAPS_LLC;
    	}
    	be.has_vpiid = 0;
    	be.send_padding = 0;
    	be.min_size = 0;
    	err = ioctl (fd, ATM_SETBACKEND, &be);
   	if(err < 0)
	{
		fprintf(stderr, "assign:%d\n",err);
		goto finalerr;
	}
	
    	skfd = socket(AF_INET, SOCK_DGRAM, 0);
    	sprintf(ifr.ifr_name, "nas%d", which_conn);
   	err=ioctl(skfd, SIOCGIFFLAGS, &ifr);
   	if(err < 0)
	{
    		fprintf(stderr, "SIOCGIFFLAGS:%d\n",err);
		goto finalerr;
	}
    	sprintf(ifr.ifr_name, "nas%d", which_conn);
    	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    	err=ioctl(skfd, SIOCSIFFLAGS, &ifr);
   	if(err < 0)
	{
    		fprintf(stderr, "SIOCGIFFLAGS:%d\n",err);
		goto finalerr;
	}
	
	if(skfd > 0)
    		close(skfd);
	return fd;
	
finalerr:
	if(fd > 0)
		close(fd);
	if(skfd > 0)
    		close(skfd);
    	return err;
}
