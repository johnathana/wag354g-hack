
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
#include <time.h>
#include <sys/klog.h>
#include <netdb.h>

#include <broadcom.h>

/* barry add 20031027 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PROC_DEV "/proc/net/dev"

int
ej_wl_packet_get(int eid, webs_t wp, int argc, char_t **argv)
{
	char line[256];
	FILE *fp;
	struct dev_info {
		char ifname[10];
		unsigned long rx_bytes;
		unsigned long rx_pks;
		unsigned long rx_errs;
		unsigned long rx_drops;
		unsigned long rx_fifo;
		unsigned long rx_frame;
		unsigned long rx_com;
		unsigned long rx_mcast;
		unsigned long tx_bytes;
		unsigned long tx_pks;
		unsigned long tx_errs;
		unsigned long tx_drops;
		unsigned long tx_fifo;
		unsigned long tx_colls;
		unsigned long tx_carr;
		unsigned long tx_com;
	}info;
	
	info.rx_pks = info.rx_errs = info.rx_drops = 0;
	info.tx_pks = info.tx_errs = info.tx_drops = info.tx_colls = 0;
	
	if ((fp = fopen(PROC_DEV, "r")) == NULL) {
       		websError(wp, 400, "Cann't open %s\n", PROC_DEV);
        }
	else{
/*
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth0:  674829    5501    0    0    0     0          0         0  1249130    1831    0    0    0     0       0          0
  eth1:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth2:       0       0    0    0    0   719          0         0     1974      16  295    0    0     0       0          0
   br0:  107114    1078    0    0    0     0          0         0   910094    1304    0    0    0     0       0          0

*/
		while( fgets(line, sizeof(line), fp) != NULL ) {
			int ifl = 0;
			if(!strchr(line,':'))	continue;
			while(line[ifl] != ':')
				ifl++;
			line[ifl]=0;    /* interface */
			if(strstr(line, nvram_safe_get("wl0_ifname"))){
		 		sscanf(line+ifl+1,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",&info.rx_bytes, &info.rx_pks, &info.rx_errs, &info.rx_drops, &info.rx_fifo, &info.rx_frame, &info.rx_com, &info.rx_mcast, &info.tx_bytes, &info.tx_pks , &info.tx_errs, &info.tx_drops, &info.tx_fifo, &info.tx_colls, &info.tx_carr, &info.tx_com);
			}
		
		}
		fclose(fp);
	}

	websWrite(wp,"SWRXgoodPacket=%ld;",info.rx_pks);	
	websWrite(wp,"SWRXerrorPacket=%ld;",info.rx_errs + info.rx_drops);	

	websWrite(wp,"SWTXgoodPacket=%ld;",info.tx_pks);	
	websWrite(wp,"SWTXerrorPacket=%ld;",info.tx_errs + info.tx_drops + info.tx_colls);	


	return 0;
}

int
StartContinueTx(char *value){
	int ret = 0;
	char buf[80];
	int channel;
	int rate;
	float rates;
	int gmode;
	int txant; //barry add 1117
	char *type;
        char *tx_gmode;
        char *tx_channel;
        char *tx_rate ;
        char *tx_ant ; //barry add 1117

        tx_gmode = websGetVar(wp, "wl_gmode", NULL);
        tx_channel = websGetVar(wp, "wl_channel", NULL);
        tx_rate = websGetVar(wp, "wl_rate", NULL);
        tx_ant = websGetVar(wp, "TXANT", NULL);

	printf("\ngmode=%s,channel=%s,rate=%s,txant=%s\n",tx_gmode,tx_channel,tx_rate,tx_ant);
	channel=atoi(tx_channel);
	rate=atoi(tx_rate);
	gmode=atoi(tx_gmode);
	txant=atoi(tx_ant);

	type = websGetVar(wp, "StartContinueTx", "");
	printf("\ngmode=%d,channel=%d,rate=%d,txant=%d\n",gmode,channel,rate,txant);
	printf("\nBarry StartContinueTx=%s\n",type);

	if(!strcmp(type, "0")){
		system("wl evm 0");
		system("wl up");
		system("nvram set wl_bcn=100");
		system("nvram set wl0_bcn=100");
	        if(check_hw_type() == BCM4702_CHIP) /* barry add for 4712 or 4702 RF test */
			system("wlconf eth2 up"); //For 4702
		else
			system("wlconf eth1 up");
		printf("\nStopContinueTx\n");
	}
	else if(!strcmp(type, "1")){
		/* Start Continue TX, EVM */
		//system("wl down");
		//system("wl clk 1");
		//system("wl gmode 0");
		system("nvram set wl_bcn=1");
		system("nvram set wl0_bcn=1");
	        if(check_hw_type() == BCM4702_CHIP) /* barry add for 4712 or 4702 RF test */
			system("wlconf eth2 up"); //For 4702
		else
			system("wlconf eth1 up");

		if(txant==0) 
		{
			system("wl antdiv 0");
			system("wl txant 3");
			system("wl antdiv");
			system("wl txant");
		}
		else
		{	
			system("wl antdiv 1");
			system("wl txant 3");
			system("wl antdiv");
			system("wl txant");
		}

		system("wl out");	
		system("wl clk");
		//sprintf(buf,"wl fqacurcy %d",channel);
		//system(buf);	
		
	//	channel=atoi(nvram_safe_get("wl_channel"));
	//	rate=atoi(nvram_safe_get("wl_rate"));
		rates=rate/1000000;
		printf("\nrate=%d, rates=%f\n",rate, rates);
		if (rates==5) rates=5.5;
		sprintf(buf,"wl evm %d %f &",channel,rates);
		system(buf);
		printf("\nStartContinueTx, exec:%s\n",buf);
		
	}
	else if(!strcmp(type, "2")){
		printf("\nOnly set ANT!\n");
		if(txant==0) 
		{
			system("wl antdiv 0");
			system("wl txant 3");
		}
		else
		{	
			system("wl antdiv 1");
			system("wl txant 3");
		}
	}
	return ret;
}

int
StopContinueTx(char *value){
	int ret = 0;
	char *type;

	type = websGetVar(wp, "StopContinueTx", "");
	if(!strcmp(type, "0")){
		//
	}
	else if(!strcmp(type, "1")){
		/* Stop Continue TX, EVM */
		system("wl evm 0");
		system("wl up");
		system("nvram set wl_bcn=100");
		system("nvram set wl0_bcn=100");
	        if(check_hw_type() == BCM4702_CHIP) /* barry add for 4712 or 4702 RF test */
			system("wlconf eth2 up"); //For 4702
		else
			system("wlconf eth1 up");
		printf("\nStopContinueTx\n");

	}

	return ret;
	
}

int
Check_TSSI(char *value){
	int atten_bb;
	int atten_radio;
	int atten_ctl;
	int idelay=0;
        //int tssi_check ;
        char *wl_atten_bb=NULL;
        char *wl_atten_radio;
        char *wl_atten_ctl ;
        char *wl_delay;
        //char *wl_tssi_check ;
	FILE *fp;
	int icck,iofdm;
	char ori[80];
	char buf[80];
	char buf2[80];
	char buf3[80];
	char cck[80],ofdm[80];
        
	wl_atten_radio = websGetVar(wp, "WL_atten_radio", NULL);
        wl_atten_ctl = websGetVar(wp, "WL_atten_ctl", NULL);
        //wl_tssi_check = websGetVar(wp, "WL_tssi_check", NULL);
        wl_delay = websGetVar(wp, "WL_delay", NULL);

	nvram_set("wl_atten_bb",value);
	nvram_set("wl_atten_radio",wl_atten_radio);
	nvram_set("wl_atten_ctl",wl_atten_ctl);
	nvram_set("wl_delay",wl_delay);
	//nvram_set("wl_tssi_check",wl_tssi_check);

	printf("\nBarry delay=%s\n",wl_delay);

	atten_bb=atoi(nvram_safe_get("wl_atten_bb"));
	atten_radio=atoi(nvram_safe_get("wl_atten_radio"));
	atten_ctl=atoi(nvram_safe_get("wl_atten_ctl"));
	idelay=atoi(nvram_safe_get("wl_delay"));
        //tssi_check=atoi(nvram_safe_get("wl_tssi_check"));

	//printf("\nwl_atten_bb=%s,wl_atten_radio=%s,wl_atten_ctl=%s,wl_tssi_check=%s\n", wl_atten_bb, wl_atten_radio, wl_atten_ctl,wl_tssi_check);
	printf("\nwl_atten_bb=%s,wl_atten_radio=%s,wl_atten_ctl=%s\n", wl_atten_bb, wl_atten_radio, wl_atten_ctl);

	memset(buf,0,sizeof(buf));
	sprintf(buf,"wl atten %s %s %s",value,wl_atten_radio,wl_atten_ctl);
	printf("\nsystem=%s\n",buf);
	system(buf);

	/* wait for 2 seconds */
	printf("\nWill delay %d seconds\n",idelay);
	if(idelay != 0)
		sleep(idelay);

	system("wl tssi > /tmp/get_tssi");
	memset(buf,0,sizeof(buf));
	memset(cck,0,sizeof(cck));
	memset(cck,0,sizeof(ofdm));
	if((fp=fopen("/tmp/get_tssi", "r")))
	{
		fgets(buf, sizeof(buf),fp);
		strcpy(ori,buf);
		printf("\nGot:\n%s\nlen=%d\n",buf,strlen(buf));
	}
	else
		printf("\nFile error!\n");

	memset(buf2,0,sizeof(buf2));
	strcpy(buf2,strtok(buf,","));
	memset(buf3,0,sizeof(buf3));
	strcpy(buf3,strtok(buf2,"CCK"));
	strcpy(buf3,strtok(NULL,"CCK"));
	printf("\nCCK:\n%s\n",buf3);
	strcpy(cck,buf3);
	icck=atoi(buf3);

	memset(buf2,0,sizeof(buf2));
	strcpy(buf2,strtok(ori,","));
	strcpy(buf2,strtok(NULL,","));
	memset(buf3,0,sizeof(buf3));
	strcpy(buf3,strtok(buf2,"OFDM"));
	strcpy(buf3,strtok(NULL,"OFDM"));
	printf("\nOFDM:\n%s\n",buf3);
	strcpy(ofdm,buf3);
	iofdm=atoi(buf3);

	printf("\nCCK=%d,OFDM=%d\n",icck,iofdm);
	printf("\nCCK=%s,OFDM=%s\n",cck,ofdm);

	nvram_set("wl_cck",cck);
	nvram_set("wl_ofdm",ofdm);
	nvram_set("wl_tssi_result",cck);
	nvram_commit();

//	if(tssi_check == icck)
//	{
//		printf("\nMatch!\n");
//		nvram_set("wl_tssi_result","1");
//	}
//	else
//	{
//		printf("\nDisMatch!\n");
//		nvram_set("wl_tssi_result","0");
//	}

	return 1;

}

int Get_TSSI(char *value)
{
	char cck[80],ofdm[80];

	memset(cck,0,sizeof(cck));
	memset(ofdm,0,sizeof(ofdm));

	strcpy(cck,nvram_safe_get("wl_cck"));
	strcpy(ofdm,nvram_safe_get("wl_ofdm"));

	printf("\nCCK=%s,OFDM=%s\n",cck,ofdm);
	return 1;
}

int Enable_TSSI(char *value)
{
	int enabled;
	int atten_bb;
	int atten_radio;
	int atten_ctl;
	char buf[80];

	enabled=atoi(value);
	if(enabled)
	{
		atten_bb=atoi(nvram_safe_get("wl_atten_bb"));
		atten_radio=atoi(nvram_safe_get("wl_atten_radio"));
		atten_ctl=atoi(nvram_safe_get("wl_atten_ctl"));

		memset(buf,0,sizeof(buf));
		sprintf(buf,"wl atten %d %d %d",atten_bb,atten_radio,atten_ctl);
		system(buf);
	}
	return 1;
}

int
Change_Ant(char *value){
	int changeant; //barry add 1216

	printf("\nvalue=%s\n",value);
	changeant=atoi(value);

	if(changeant==0) 
	{
		system("wl antdiv 0");
		system("wl txant 3");
		system("wl antdiv");
		system("wl txant");
		printf("\nwl antdiv 0");
		printf("\nwl txant 3");
	}
	else
	{	
		system("wl antdiv 1");
		system("wl txant 3");
		system("wl antdiv");
		system("wl txant");
		printf("\nwl antdiv 1");
		printf("\nwl txant 3");
	}

	return 0;
}

int
StartContinueTx_4702(char *value){
	int ret = 0;
	char buf[80];
	int channel;
	int rate;
	float rates;
	int gmode;
	char *type;
        char *tx_gmode;
        char *tx_channel;
        char *tx_rate ;

        tx_gmode = websGetVar(wp, "wl_gmode", NULL);
        tx_channel = websGetVar(wp, "wl_channel", NULL);
        tx_rate = websGetVar(wp, "wl_rate", NULL);

	printf("\ngmode=%s,channel=%s,rate=%s\n",tx_gmode,tx_channel,tx_rate);
	channel=atoi(tx_channel);
	rate=atoi(tx_rate);
	gmode=atoi(tx_gmode);

	type = websGetVar(wp, "StartContinueTx", "");
	printf("\ngmode=%d,channel=%d,rate=%d\n",gmode,channel,rate);
	
	if(!strcmp(type, "0")){
		//
	}
	else if(!strcmp(type, "1")){
		/* Start Continue TX, EVM */
		//system("wl down");
		//system("wl clk 1");
		//system("wl gmode 0");
		system("nvram set wl_bcn=1");
		system("nvram set wl0_bcn=1");
		system("wlconf eth2 up");
	
		system("wl txant 0");	
		system("wl out");	
		system("wl clk");
		//sprintf(buf,"wl fqacurcy %d",channel);
		//system(buf);	
		
	//	channel=atoi(nvram_safe_get("wl_channel"));
	//	rate=atoi(nvram_safe_get("wl_rate"));
		rates=rate/1000000;
		printf("\nrate=%d, rates=%f\n",rate, rates);
		if (rates==5) rates=5.5;
		sprintf(buf,"wl evm %d %f &",channel,rates);
		system(buf);
		printf("\nStartContinueTx, exec:%s\n",buf);
		
	}
	return ret;
}

int
StopContinueTx_4702(char *value){
	int ret = 0;
	char *type;

	type = websGetVar(wp, "StopContinueTx", "");
	if(!strcmp(type, "0")){
		//
	}
	else if(!strcmp(type, "1")){
		/* Stop Continue TX, EVM */
		system("wl evm 0");
		system("wl up");
		system("nvram set wl_bcn=100");
		system("nvram set wl0_bcn=100");
		system("wlconf eth2 up");
		printf("\nStopContinueTx\n");

	}

	return ret;
	
}
