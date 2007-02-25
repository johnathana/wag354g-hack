  
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
/* kirby    2004/7 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include	<errno.h>
#include <sys/types.h>
#include <sys/socket.h>	/* basic socket definitions */
#include <sys/time.h>	/* timeval{} for select() */

#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>


#define   DIAGLOG_FILE        "/tmp/diagnostics.log"
#define   KDEBUG         

/* define work mode */
#define   WMODE_UNKNOWN 0
#define   WMODE_BRIDGE     1
#define   WMODE_ROUTE       2
#define   WMODE_PPPOE        3
#define   WMODE_PPPOA        4
#define   WMODE_IPOA           5

/* define Bridge work type*/
#define   BGDHCP          0
#define   BGSTATIC       1

/* define mac boradcast addr */
#define MAC_BCAST_ADDR		(unsigned char *) "\xff\xff\xff\xff\xff\xff"

/* ping param */
#define PING_TMP	"/tmp/ping.log"
#define PING_SIZE    "60"
#define PING_NUMBER  "5"
#define PING_INTERVAL "1000"
#define PING_TIMEOUT  "5000"

/* skip space in option */
#define  SKIPSPACE(arg)     do{\
                                                    while(*arg == ' ')\
					                     *arg++;\
				                   }while(0)

FILE*  diagfp = NULL;
//xtt added for dhcp 07-05-2004
#define MAX_CHAN 8
pid_t udhcpc_pid[MAX_CHAN];
//xtt end/* struct for ARP package */
#define E_P_A 3
#define E_P   2
#define ETHERNET 1

#define P_A 2
#define PPP_ok 1

struct arpMsg {
	struct ethhdr ethhdr;	 		/* Ethernet header */
	u_short htype;				/* hardware type (must be ARPHRD_ETHER) */
	u_short ptype;				/* protocol type (must be ETH_P_IP) */
	u_char  hlen;				       /* hardware address length (must be 6) */
	u_char  plen;				       /* protocol address length (must be 4) */
	u_short operation;			/* ARP opcode */
	u_char  sHaddr[6];			/* sender's hardware address */
	u_char  sInaddr[4];			/* sender's IP address */
	u_char  tHaddr[6];			/* target's hardware address */
	u_char  tInaddr[4];			/* target's IP address */
	u_char  pad[18];			       /* pad for min. Ethernet payload (60 bytes) */
};

/* struct for param from diagstart */
struct diagParam{
       u_char wmode;           /* work mode */
	u_char wtype;             /* work type: DHCP or Static */   
	u_char* gip;                   /* gateway ip address */
	u_char* ownip;              /* router ip address */
	char* user;                /* pppoe username  */
	char* password;        /* pppoe password */
	char* muser;             /* mpppoe username*/
	char* mpassword;     /* mpppoe password */
  	char* multiplex;        /* pppoa/pppoe multiplex */
	int conn;  /* interface name */
	int demand;     /* demand enable or disable */
	char* whostname;        /*wan host name*/
	u_char ownmac[6];    /* router mac address */
};

struct diagParam  dparam;
	
/* check status function */
int check_adsl(void);
int check_atm(void);
int Check_IP_Layer(int conn);
int check_ethernet_static(unsigned char* sip,unsigned char* dip,unsigned char* smac,int conn);
int check_ehternet_dhcp(unsigned char* sip,unsigned char* dip,int conn,char* host);
int start_diag_redial(int conn);
int  check_pppoa(char* user,char* password,int conn);
int  check_pppoe(char* user,char* password,int conn);
int check_ip(char* dip);

void dumpparam(void);

int main(int argc, char* argv[])
{
      int ret;
      char* argcur;	
	  	
      printf("running!\n");
      /* init param struct */	  
      memset(&dparam,0,sizeof(dparam));	  

      if(argc <=1){
	  	printf("diagtool error: param incompelete\n");
              return 0;
      }
      /* phrase any option */
      argc --;
      argv ++;	  

      while (argc >= 1 && **argv == '-') {
	  	argcur = *argv;   /* get current option string */
		argcur ++;           /* skip "-" */
		switch(*argcur){
			case 'm':          /* get work mode */
				argv++;
				argc --;
			       argcur = *argv;
				#ifdef  KDEBUG
				      printf("kdebug: Work mode: %s\n",argcur);
				#endif
				if(!strncmp(argcur,"1483bridged",11))
					dparam.wmode = WMODE_BRIDGE;
				else if(!strncmp(argcur,"routed",6))
					dparam.wmode = WMODE_ROUTE;
				else if(!strncmp(argcur,"pppoe",5))
					dparam.wmode = WMODE_PPPOE;
				else if(!strncmp(argcur,"pppoa",5))
					dparam.wmode = WMODE_PPPOA;
				else if(!strncmp(argcur,"clip",4))
					dparam.wmode = WMODE_IPOA;
				else dparam.wmode = WMODE_UNKNOWN;
				break;
			case 't':
				argv++;
				argc --;
			       argcur = *argv;
				#ifdef  KDEBUG
				      printf("kdebug: 1483bridge work type: %s\n",argcur);
				#endif
				/* if work mode is 1483bridge */
				if(!strncmp(argcur,"dhcp",4))
					dparam.wtype = BGDHCP;
				else dparam.wtype = BGSTATIC;
				break;
			case 'g':
				argv++;
				argc --;
			       argcur = *argv;
				#ifdef  KDEBUG
				      printf("kdebug: Gateway IP address: %s\n",argcur);
				#endif
				dparam.gip = argcur;
				break;
			case 'a':
				argv++;
				argc --;
			       argcur = *argv;
				#ifdef  KDEBUG
				      printf("kdebug: Wan IP address: %s\n",argcur);
				#endif
				dparam.ownip = argcur;
				break;	
			case 'u':
                            argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: User Name: %s\n",argcur);
				#endif
				dparam.user = argcur;
				break;
			case 'p':
				argv++;
				argc --;
			       argcur = *argv;
				#ifdef  KDEBUG
				      printf("kdebug: User Password: %s\n",argcur);
				#endif
				dparam.password= argcur;
				break;
			case 'c':
                            argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: Wan Mac Address: %s\n",argcur);
				#endif
				sscanf(argcur,"%x:%x:%x:%x:%x:%x",&dparam.ownmac[0],&dparam.ownmac[1],
					   &dparam.ownmac[2],&dparam.ownmac[3],&dparam.ownmac[4],&dparam.ownmac[5]);
				break;
			case 'i':	
				argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: Wan interface name: %s\n",argcur);
				#endif
				dparam.conn = atoi(argcur);
				break;
			case 'h':
				argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: Wan hostname: %s\n",argcur);
				#endif
				dparam.whostname= argcur;
				break;
			case 'v':
				argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: multiplex: %s\n",argcur);
				#endif
				dparam.multiplex= argcur;
				break;
			case 'd':
				argv++;
				argc --;			
			       argcur = *argv;
                            #ifdef  KDEBUG
				      printf("kdebug: demand: %s\n",argcur);
				#endif
				dparam.demand= atoi(argcur);
				break;
			case 'r':
				argv++;
				argc --;			 
			       argcur = *argv;
                               #ifdef  KDEBUG
				      printf("kdebug: : mpppoe username%s\n",argcur);
				#endif
				dparam.muser  = argcur;
				break;
                        case 's':
				argv++;
				argc --;			 
			       argcur = *argv;
                               #ifdef  KDEBUG
				      printf("kdebug: : mpppoe password%s\n",argcur);
				#endif
				dparam.mpassword = argcur;
				break;
                        
			default:
				printf("Unknown option\n");
				break;
		}
		argv++;
		argc --;
      }

      //dumpparam();	  
  
      /* create diag log file in /tmp */	  
      if((diagfp = fopen(DIAGLOG_FILE,"w")) == NULL){
	  	printf("Can not create diag log file in tmp\n");
		exit(1);
      }

      /* check ADSL */
      ret = check_adsl();    
      if(ret < 0){
	  	fprintf(diagfp,"ADSL link: Failed<BR>");
		fclose(diagfp);
		exit(1);
      }
      else{
	  	fprintf(diagfp,"ADSL link: Successful<BR>");
		
      }	

      /* check ATM (it not be checked now always success) */	  
      ret = check_atm();
      if(ret < 0){
	  	fprintf(diagfp," ATM: Failed<BR>");
		fclose(diagfp);
		exit(1);
      }
      else{
	  	fprintf(diagfp," ATM: Successful<BR>");  
      }


      /* check ethernet */	  
      /* Only 1483bridge mode need to do it */	  
      if(dparam.wmode == WMODE_BRIDGE){
	     /* if work type is DHCP */
             printf("Now check 1483_bridge\n");	      
	     if(dparam.wtype == BGDHCP){
      			printf("Now check 1483_bridge DHCP\n");	      
		 	ret = check_ehternet_dhcp(dparam.ownip,dparam.gip,dparam.conn,dparam.whostname);
			if(ret < 0){
	  	              fprintf(diagfp,"Primary Ethernet : Can not find DHCP Server<BR>");
		              if(!dparam.demand){
				  fclose(diagfp)	;  	
		                  exit(1);
		              }	
	    			goto SECOND_CHECK;		      
                        }
                        else{
			      fprintf(diagfp,"Primary Ethernet: Successful<BR>"); 	 
			}
	     }
	     else if(dparam.wtype == BGSTATIC){
      			printf("Now check 1483_bridge static ip\n");	      
		 	ret = check_ethernet_static(dparam.ownip,dparam.gip,dparam.ownmac,dparam.conn);
			/* get check result */	 
	                if(ret < 0){
	  	              fprintf(diagfp,"Primary  Ethernet: Failed<BR>");
		              if(!dparam.demand){	   
 			          fclose(diagfp)	; 	  	
		                  exit(1);
		              }
    				goto SECOND_CHECK;			      
                        }
                        else{
			      fprintf(diagfp,"Primary Ethernet: Successful<BR>"); 	 
			}
	     }
	     else  {   /* it is incorrect type */
		 	printf("1483bridge: unknown work type\n");	 
			ret = -1;
	     }	
	   
      }

      /* check pppoe */	  
      /* Only PPPOE mode need to do it*/	  
      if(dparam.wmode == WMODE_PPPOE)	{
      		printf("Now check PPPoE mode\n");	      
	  	ret = check_pppoe(dparam.user, dparam.password, dparam.conn);
		if(ret < 0){
			fprintf(diagfp,"Primary PPPOE: Find Server Failed<BR>");
			if(!dparam.demand){	  
			     fclose(diagfp)	;	
			     exit(1);
			}	 
    				goto SECOND_CHECK;			      
		}
		else if(ret == E_P_A){
			fprintf(diagfp,"Primary PPPoE Ethernet: Successful<BR>");
			fprintf(diagfp,"Primary PPPoE: Successful<BR>");
		}
      		else if(ret == E_P){
			fprintf(diagfp,"Primary PPPoE: Authentication Failed<BR>");
			if(!dparam.demand){	  
			     fclose(diagfp); 
			     exit(1);
			}	 
    				goto SECOND_CHECK;			      
		}
      		else if(ret == ETHERNET){
			fprintf(diagfp,"Primary PPPoE: PPP LCP Failed<BR>");
			if(!dparam.demand){	
			     fclose(diagfp); 	
			     exit(1);
			}	 
    				goto SECOND_CHECK;			      
		}
      		else
		{
			fprintf(diagfp, "Primary PPPoE: Unknow Error<BR>");
			if(!dparam.demand){
			     fclose(diagfp); 	
			     exit(1);
			}	 
    				goto SECOND_CHECK;			      
		}

      }
 
      /* check PPPOA  */
      if(dparam.wmode == WMODE_PPPOA){
      		printf("Now check PPPoA mode\n");	      
	  	ret = check_pppoa(dparam.user, dparam.password, dparam.conn);
		if(ret < 0){
			fprintf(diagfp,"PPPOA: PPP LCP Failed<BR>");
			fclose(diagfp); 
			exit(1);
		}
		else if(ret == P_A){
			fprintf(diagfp,"PPPOA: Successful<BR>");
			
		}
      		else if(ret == PPP_ok){
			fprintf(diagfp,"PPPOA: Authentication Failed<BR>");
			fclose(diagfp); 
			exit(1);
		}
      		else
		{
			fprintf(diagfp, "PPPOA: Unknow Error<BR>");
			fclose(diagfp); 
			exit(1);
		}
      }
      
      /* check IP */	  
      if((dparam.wmode == WMODE_BRIDGE) ||(dparam.wmode == WMODE_ROUTE)||(dparam.wmode == WMODE_IPOA)){
	    	if(dparam.wtype == BGDHCP){
      			printf("Now check 1483_bridge DHCP IP Layer\n");	      
			ret = Check_IP_Layer(dparam.conn);
		}
		else{
      			printf("Now check 1483_bridge STATIC IP Layer\n");	      
			ret = check_ip(dparam.gip);
		}

		if(ret < 0){
	     	       fprintf(diagfp,"Primary IP Layer: Failed<BR>");
			fclose(diagfp); 	   
			exit(1);
              }
              else{
			fprintf(diagfp,"Primary IP Layer: Successful<BR>");  
			
              }
      }
      else if((dparam.wmode == WMODE_PPPOE) ||(dparam.wmode == WMODE_PPPOA)){
      		printf("Now check PPPoE&PPPoA IP Layer\n");	      
		ret = Check_IP_Layer(dparam.conn);	
	      if(ret < 0){
	     	       fprintf(diagfp,"Primary IP Layer: Failed<BR>");
			fclose(diagfp); 	   
			exit(1);
              }
              else{
			fprintf(diagfp,"Primary IP Layer: Successful<BR>");  
			
              }
	  	
      }
      else{
	  	printf("This mode does not do IP check\n");
      }
SECOND_CHECK:
      /* if second pppoe enbale check it */
      if((dparam.wmode == WMODE_BRIDGE)&&(dparam.demand)){
	      printf("Now check IPPPoE mode\n");
	     /* check pppoe */	  
	      ret = check_pppoe(dparam.user, dparam.password, (dparam.conn + 8));
	     if(ret < 0){
		        fprintf(diagfp,"Second PPPoE: Find Server Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
             }
	     else if(ret == E_P_A){
	  	        fprintf(diagfp,"Second PPPoE Ethernet: Successful<BR>");			
			fprintf(diagfp,"Second PPPoE: Successful<BR>");
             }
      	     else if(ret == E_P){
		        fprintf(diagfp,"Second PPPoE: Authentication Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
      	     else if(ret == ETHERNET){
		        fprintf(diagfp,"Second PPPoE: PPP LCP Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
      	     else
	     {
		        fprintf(diagfp, "Second PPPoE: Unknow Error<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
              /* check ip */
	      printf("Now check PPPoE&PPPoA IP Layer\n");	      
	      ret = Check_IP_Layer(dparam.conn + 8);	
	      if(ret < 0){
	     	       fprintf(diagfp,"Second PPPoE IP Layer: Failed<BR>");
		       fclose(diagfp); 	 		   
		       exit(1);
              }
              else{
			fprintf(diagfp,"Second PPPoE IP Layer: Successful<BR>");  
			
              }		  
 
      }

       if((dparam.wmode == WMODE_PPPOE)&&(dparam.demand)){
	      printf("Now check MPPPoE mode\n");
	     /* check pppoe */	  
	      ret = check_pppoe(dparam.muser, dparam.mpassword, (dparam.conn + 8));
	     if(ret < 0){
		        fprintf(diagfp,"Second PPPoE: Find Server Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
             }
	     else if(ret == E_P_A){
	  	        fprintf(diagfp,"Second PPPoE Ethernet: Successful<BR>");			
			fprintf(diagfp,"Second PPPoE: Successful<BR>");
             }
      	     else if(ret == E_P){
		        fprintf(diagfp,"Second PPPoE: Authentication Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
      	     else if(ret == ETHERNET){
		        fprintf(diagfp,"Second PPPoE: PPP LCP Failed<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
      	     else
	     {
		        fprintf(diagfp, "Second PPPoE: Unknow Error<BR>");
			fclose(diagfp); 	 	
			exit(1);	
	      }
              /* check ip */
	      printf("Now check PPPoE&PPPoA IP Layer\n");	      
	      ret = Check_IP_Layer(dparam.conn + 8);	
	      if(ret < 0){
	     	       fprintf(diagfp,"Second PPPoE IP Layer: Failed<BR>");
		       fclose(diagfp); 	 		   
		       exit(1);
              }
              else{
			fprintf(diagfp,"Second PPPoE IP Layer: Successful<BR>");  
			
              }		  
 
      }
  
      fclose(diagfp);            	  
      exit(0);
}

/* read adsl status info in  /proc/avalanche/avsar_modem_training*/
/* if adsl ok return 1 or return -1 */
int check_adsl(void)
{
      FILE* fp = NULL;
      char buf[16];	  

      /* open adsl status file */
      fp = fopen("/proc/avalanche/avsar_modem_training", "r");
      if(fp == NULL){
	  	printf("Can not open adsl status tmp file\n");
		return -1;
      }

      memset(buf,0,sizeof(buf));
      /* read status */	  
      if(fread(buf, sizeof(char), sizeof(buf)*sizeof(char), fp) <= 0){
	  	printf("Can not read any data in adsl status file\n");
		fclose(fp);   /* close file */
		return -1;
	}
	fclose(fp);

	/* phrase adsl status */
	if(!strncmp(buf, "SHOWTIME", strlen("SHOWTIME"))){ /* adsl up */
		return 1;
	}
	else{  /* adsl down */
		return -1;
	}
}

//
int check_atm(void)
{
       return 1;
}

/* check ethernet if work mode in 1483bridge with DHCP */
int check_ehternet_dhcp(unsigned char* sip,unsigned char* dip,int conn,char* host)
{
	char cmd[100];
	char pidfile[32];
	char ifname[10];
	char udhcpc_pid_char[4];
	int udhcpc_pid_no;
	char logfile_name[20];
	sprintf(ifname,"nas%d",conn);
	printf("now D_DHCP ethernet\n");
	memset(pidfile,0,sizeof(pidfile));
	sprintf(pidfile, "/var/run/udhcpc%d.pid", conn);
	file_to_buf(pidfile,udhcpc_pid_char,4);
	udhcpc_pid_no = atoi(udhcpc_pid_char);
	{
	char *dhcp_argv[] = 
	{  	
		"udhcpc",
	    	"-i", ifname,
		//"-p", "/var/run/udhcpc.pid",
		"-p", pidfile,
		"-s", "/tmp/udhcpc",
		host && *host ? "-H" : NULL,
		host && *host ? host : NULL,
		NULL
	}; 
	printf("now to kill udhcpc %d\n",udhcpc_pid_no);
	sprintf(cmd, "kill %d",udhcpc_pid_no);
	system(cmd);
	printf("kill udhcpc sucess and to restart\n");
       /*open log file*/
	sprintf(logfile_name,"/tmp/D_DHCP.log");	
	sprintf(cmd,"rm -f %s",logfile_name);
	system(cmd);
	_eval(dhcp_argv, NULL, 0, &udhcpc_pid[conn]);
	sleep(3);
	file_to_buf(logfile_name,udhcpc_pid_char,4);
	printf("dhcp offer --%s\n",udhcpc_pid_char);
	if(atoi(udhcpc_pid_char) == 1)
		return 1;
	}
	return -1;//1 -- success   -1 -- failed
}

/* check ethernet if work mode in 1483bridge with static IP */
/* method: send arp package if return MAC addr it is OK */
/* this code  come from arpping.c*/
int check_ethernet_static(unsigned char* sip,unsigned char* dip,unsigned char* smac,int conn)
{
       int	timeout = 1;		// we need to reduce time
	int 	optval = 1;
	int	s;			       /* socket */
	int	rv = 1;			/* return value */
	struct sockaddr addr;		/* for interface name */
	struct arpMsg	arp;
	fd_set		fdset;
	struct timeval	tm;
	time_t		prevTime;
	struct in_addr ipaddr;	// add by honor
	char* running;
	char* token;
	int maclen,i,j;
	char wan_ifname[10];

      	printf("Now in check_ethernet_static\n");	      
       /* check param */

	if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) {
		printf( "Could not open raw socket\n");
		return -1;
	}
	
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		printf("Could not setsocketopt on raw socket\n");
		close(s);
		return -1;
	}

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);	/* MAC DA */
	memcpy(arp.ethhdr.h_source, smac, 6);		/* MAC SA */
	arp.ethhdr.h_proto = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);		/* ARP op code */

	printf("dump packge\n");
	*((u_int *) arp.sInaddr) = inet_addr(sip);			/* source IP address */
       printf("ARP request source ip: %d.%d.%d.%d\n",arp.sInaddr[0],arp.sInaddr[1],arp.sInaddr[2],arp.sInaddr[3]);	
	memcpy(arp.sHaddr, smac, 6);			/* source hardware address */
	printf("ARP request source mac: %x:%x:%x:%x:%x:%x\n",arp.sHaddr[0],arp.sHaddr[1],arp.sHaddr[2],
		                                                                      arp.sHaddr[3],arp.sHaddr[4],arp.sHaddr[5]);	
	*((u_int *) arp.tInaddr) = inet_addr(dip);		/* target IP address */
	printf("ARP request des ip: %d.%d.%d.%d\n", arp.tInaddr[0], arp.tInaddr[1], arp.tInaddr[2], arp.tInaddr[3]);	

       sprintf(wan_ifname, "nas%d", conn);
	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, wan_ifname);
	printf("ARP request interface name : %s\n",addr.sa_data);
	
	//strcpy(addr.sa_data, interface);
	if (sendto(s, &arp, sizeof(arp), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		rv = -1;
		printf("sendto error\n");
	}	
	
	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	time(&prevTime);
	while (timeout > 0) {
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec = timeout;
		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0) {
			//DEBUG(LOG_ERR, "Error on ARPING request: %s", strerror(errno));
			printf("Error on ARPING request: %s", strerror(errno));
			if (errno != EINTR) rv = -1;
		} 
		else if (FD_ISSET(s, &fdset)) {
			if (recv(s, &arp, sizeof(arp), 0) < 0 ) {
				rv = -1;
				printf("recv error\n"); 
			}	
			printf("ARP reply target mac: %x:%x:%x:%x:%x:%x\n",arp.tHaddr[0],arp.tHaddr[1],arp.tHaddr[2],
		                                                                      arp.tHaddr[3],arp.tHaddr[4],arp.tHaddr[5]);	
			printf("ARP reply source ip: %d.%d.%d.%d\n",arp.sInaddr[0],arp.sInaddr[1],arp.sInaddr[2],arp.sInaddr[3]);	
			if (arp.operation == htons(ARPOP_REPLY) && 
			    bcmp(arp.tHaddr, smac, 6) == 0 && 
			    *((u_int *) arp.sInaddr) == (inet_addr(dip))) {
				//DEBUG(LOG_INFO, "Valid arp reply receved for this address");
				printf("Valid arp reply receved for this address\n");
				rv = 1;
				break;
			}
		}
		timeout -= time(NULL) - prevTime;
		time(&prevTime);
	}
	close(s);
	//ipaddr.s_addr = yiaddr;	// add by honor
	//DEBUG(LOG_DEBUG, "%salid arp replies for this address (%s)", rv ? "No v" : "V", inet_ntoa(ipaddr));	 // modify by honor
	return rv;
      
}

/* this function is for debug only*/
void dumpparam(void)
{
      char dump[20];

      /* dump gateway ip */	  
      printf("%s\n",dparam.gip);	  
      /* dump own ip */	  
      printf("%s\n",dparam.ownip);	  	  
      /* dump own mac address */	  
      printf("%x:%x:%x:%x:%x:%x\n",dparam.ownmac[0],dparam.ownmac[1],
                dparam.ownmac[2],dparam.ownmac[3],dparam.ownmac[4],dparam.ownmac[5]);	  	  	  
}

/* check ip layer */
/* method : invoke ping in system and read log file */
int check_ip(char* dip)
{
       char cmd[100];
	FILE* fp;   
	char sp[3],rp[3];
	int isp,irp;
	int i = 0,j=0;
	int sr = 0;
	int ret = -1;
	

      	printf("Now in check_ip\n");	      
       /* check param */
	if(!dip || !(strcmp(dip,"0.0.0.0")))   
	    return ret;

       memset(cmd,0,sizeof(cmd));
       /* prepare to invoke ping in /usr/sbin/diagnostics */
	snprintf(cmd, sizeof(cmd), "/usr/sbin/diagnostic -f %s -s %s -c %s -i %s -w %s %s",
		PING_TMP, 
		PING_SIZE, 
		PING_NUMBER, 
		PING_INTERVAL, 
		PING_TIMEOUT, 
		dip);   

	 printf("ping cmd = %s\n",cmd);  
	 /* run ping */
	 system(cmd);

        printf("Ansy log start\n");
	 /* read ping log file */
	 if((fp = fopen(PING_TMP,"r")) == NULL){
	 	printf("Do not open ping log file in /tmp\n");
		return ret;
	 }
     
        /* read log */
	 if(fgets(cmd, sizeof(cmd), fp) != NULL ) {
			cmd[strlen(cmd)-1] = '\0';
	 }
	 else{
	 	printf("Log file read error\n");
		fclose(fp);
		return ret;
	 }
        printf("log : %s\n",cmd);
	 /* phrase log */
        /* if correct log is "Pkt Sent:4 PktRecv:4 Avg Rtt:5.0"  */
	 memset(sp,0,sizeof(sp));
	 memset(rp,0,sizeof(rp));
        while(cmd[i] != '\0'){
		if(cmd[i] != ':'){
			i++;
			continue;
		}	
		/* read send package number*/
		i++;    /* skip ":" */
		j = 0;
		if(!sr){
		      while(cmd[i] >='0' && cmd[i] <='9'){
			     sp[j++] = cmd[i++];	
		      	}
			sr = 1;   
			printf("send package : %s\n",sp);  
		}
		/* read recv package number */
		else{
			while(cmd[i] >='0' && cmd[i] <='9'){
			     rp[j++] = cmd[i++];	
		      	}
			printf("receive package : %s\n",rp);
			break;
		}
        }
	 /* if send package == recv package ip ok*/	
	 isp = atoi(sp);
	 irp = atoi(rp);
	 printf("isp = %d, irp = %d\n", isp,irp);
	 if((isp!=0)&&(irp!=0)){
	 	printf("IP layer ok\n");
		ret = 1;
	 }
	 else{
	 	printf("IP layer failed\n");
	 }
	 fclose(fp);
	 return ret;
}

/* check pppoa */
int  check_pppoa(char* user,char* password,int conn)
{
	/*
	unsigned char packet[64];
	char *buf;
	struct PPPoALhdr *lhdr;
	struct PPPoAVhdr *vhdr;
	fd_set readable;
	int cc, i;
	struct timeval timeout;
	
	if(fd < 0)
		return -1;
	
	memset(packet, 0 ,sizeof(packet));
	
	if(!strcmp(dparam.multiplex, "llc")) //LLC
	{
		lhdr = (struct PPPoALhdr *)packet;
		lhdr->llc = htons(0xFEFE);
		lhdr->ctrl = 0x03;
	lhdr->nlpid = 0xCF;
		lhdr->ppp_proto = htons(0xC021);
		buf = packet + sizeof(struct PPPoALhdr);
	}

	else if(!strcmp(dparam.multiplex, "vc"))//VC
	{
		vhdr = (struct PPPoAVhdr *)packet;
		vhdr->ppp_proto = htons(0xC021);
		buf = packet + sizeof(struct PPPoAVhdr);
	}

	*(buf)++ = 1;
	*(buf)++ = 0x05;
	*(buf)++ = 0;
	*(buf)++ = 0x04;
	
	printf("before write\n");
	for(i=0; i<sizeof(packet); i++)
		printf("%02x...", packet[i]);
	
	if(send(fd, (unsigned char *)packet, sizeof(packet), 0) < 0)
	{
		printf("write fail\n");
		close(fd);
		return -1;
	}

	FD_ZERO(&readable);
	FD_SET(fd, &readable);
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	
	printf("begin waiting\n");
	while(1)
	{
		cc = select(fd+1, &readable, NULL, NULL, &timeout);
		if(cc < 0)
		{
			printf("select error\n");
			close(fd);
			return -1;
		}
		else if(cc == 0)
		{
			printf("time out\n");
			close(fd);
			return -1;
		}
		else
		{
			printf("have new message\n");
			if(!FD_ISSET(fd, &readable))
			{
				printf("which fd readable??\n");
				continue;
			}
			else 
			{
				printf("before read\n");
				memset(packet, 0 ,sizeof(packet));
				if((cc = recv(fd, packet, sizeof(packet), 0))< 0)
				{
					printf("read error\n");
					close(fd);
					return -1;
				}	
				for(i=0; i<sizeof(packet); i++)
					printf("%02x...", packet[i]);
				
				if(!strcmp(dparam.multiplex, "llc")) //LLC
				{
					printf("here is LLC process\n");
					lhdr = (struct PPPoALhdr *)packet;
					if(lhdr->llc != htons(0xFEFE) || 
					    (lhdr->ctrl != 0x03) || 
					    (lhdr->nlpid != 0xCF))
					{
						printf("wrong llc-aal5 header\n");
						continue;
					}
					if(lhdr->ppp_proto != htons(0xC021))
					{
						printf("wrong ppp proto(not lcp)\n");
						continue;
					}
								
				}
				else if(!strcmp(dparam.multiplex, "vc"))//VC
				{
					printf("here is VC process\n");
					vhdr = (struct PPPoAVhdr *)packet;
					if(vhdr->ppp_proto != htons(0xC021))
					{
						printf("wrong ppp proto(not lcp)\n");
						continue;
					}
				}
				
				printf("ok get lcp packet!!!\n");
				break;
				
			}	
		
		}	
	}
	close(fd);	
	*/
	char cmd[50];
	FILE *fp;
	char log_file[30];
	
      	printf("Now in check_pppoa\n");	      
	memset(log_file,0,sizeof(log_file));
	memset(cmd,0,sizeof(cmd));
/*		
	sprintf(log_file,"%s","/tmp/D_PPP_P.log");
	sprintf(cmd,"rm -f %s",log_file);
	system(cmd);
	sprintf(log_file,"%s","/tmp/D_AUTH_A.log");
	sprintf(cmd,"rm -f %s",log_file);
	system(cmd);
*/
	if(start_diag_redial(conn) == 99)
		return P_A;//P&A ok
	else
	{
		sleep(2);

		sprintf(log_file,"%s","/tmp/D_PPP_P.log");
		if(fp = fopen(log_file,"r") == NULL)
		{
			return -1;//E&P&A Failed
		}
		sprintf(log_file,"%s","/tmp/D_AUTH_A.log");
		if(fp = fopen(log_file,"r") != NULL)
		{
			return P_A;//P&A ok
		}
		else
		{
			return PPP_ok;//P ok
			
		}
		
	}
}

/* check pppoe */
int check_pppoe(char* user,char* passowrd,int conn)
{
	char cmd[50];
	FILE *fp;
	char log_file[30];
	
      	printf("Now in check_pppoe\n");	      
	memset(log_file,0,sizeof(log_file));
	memset(cmd,0,sizeof(cmd));
	
	if(start_diag_redial(conn) == 99)
	{
		return E_P_A;//E&P&A ok
	}
	else
	{
		sleep(2);

		sprintf(log_file,"%s","/tmp/D_PPPOE_E.log");
		if(fp = fopen(log_file,"r") == NULL)
		{
			return -1;//E&P&A Failed
		}
		sprintf(log_file,"%s","/tmp/D_AUTH_A.log");
		if(fp = fopen(log_file,"r") != NULL)
		{
			return E_P_A;//E&P&A ok
		}
		else
		{
			sprintf(log_file,"%s","/tmp/D_PPP_P.log");
			if(fp = fopen(log_file,"r") != NULL)
			{
				return E_P;//E&P ok
			}
			else
				return ETHERNET;//E ok
			
		}
		
	}
        return 1;
}

/*xtt add for pppoe & pppoa*/
int start_diag_redial(int conn)
{
	int ret;
	pid_t pid;
	char cmd[100];
	char devname[10];

	//check link status
	char link_status[20];
	FILE *fp = NULL;
      	printf("Now in start_diag_redial\n");	 
        printf("dparam.conn =%d\n", conn);	
	memset(link_status, 0 ,sizeof(link_status));
	sprintf(link_status,"/tmp/ppp/link%d",conn);
	printf("%s\n",link_status);
	if((fp = fopen(link_status, "r")) != NULL)
	{
		fclose(fp);
		printf("PPPoE link is no problem\n");
		return 99;
	}
	printf("-----------------Diagnostic Redial-------------------\n");

	sprintf(devname,"ppp%d",conn);
	{
	char *redial_argv[] = { "/tmp/ppp/diag_redial",
				"0",
				devname,
			      NULL
	};
	
	printf("enter start_redial %s  %s\n", "0", devname);
	symlink("/sbin/rc", "/tmp/ppp/diag_redial");

	sprintf(cmd,"rm -f %s %s %s","/tmp/D_PPP_P.log", "/tmp/D_AUTH_A.log", "/tmp/D_PPPOE_E.log");
	system(cmd);
	ret = _eval(redial_argv, NULL, 0, &pid); 
	return ret;
	}	
}

int Check_IP_Layer(int conn)
{

			int i;
			int ret;
			char *ip = NULL;
			char ip_gateway[150];
			char *temp;
      			printf("Now in Check_IP_Layer\n");	      
			memset(ip_gateway,0,sizeof(ip_gateway));
			if(conn > 7)
			{
				if(dparam.wmode == WMODE_BRIDGE)
					file_to_buf("/tmp/status/wan_ipppoe_gateway",ip_gateway,sizeof(ip_gateway));
				else
					file_to_buf("/tmp/status/wan_mpppoe_gateway",ip_gateway,sizeof(ip_gateway));					
			}
			else
			{
				file_to_buf("/tmp/status/wan_gateway",ip_gateway,sizeof(ip_gateway));
			}				
			printf("ip_gateway is %s\n", ip_gateway);
			temp = ip_gateway;
			for(i = 0;i <= dparam.conn; i++)
			{
				ip = strsep(&temp, " ");
				printf("gateway ip is %s\n", ip);
			}
			if(ip)
				ret = check_ip(ip);
			else
				ret = -1;
			return ret;
}
