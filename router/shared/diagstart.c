 
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
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>

#include "broadcom.h"
#include "shutils.h"

//#define  CLIP_SUPPORT
//#define  MPPPOE_SUPPORT
//#define  IPPPOE_SUPPORT
extern char connection[5];
#ifdef CLIP_SUPPORT
extern char clip_config_buf[100];
#endif
#include "pvc.h"

/* diag tool path */
#define  DIAGTOOLPATH     /tmp/diagtool


int diagtool_start(webs_t wp)
{
      char  cmd[300]; 
      char*  wmode;
      char*   wmac;	  
      char*   ifname;
      char   name[20];
      char   wan_ifname[10];
      char temp_connection[5];      
      int i = 0 ;
      int whichconn = -1;	
      char* next;	  
      char word[3];	
#ifdef IPPPOE_SUPPORT
      char* ipppoe_enable;	  
#endif
#ifdef MPPPOE_SUPPORT
      char* mpppoe_enable;	
#endif
      int wcon_temp;	  
	  
      printf("This is Diagnostics tool for router\n");
      strcpy(temp_connection,connection);
      whichconn = atoi(nvram_safe_get("wan_active_connection"));
      if(whichconn == 8)
	  whichconn = 0;     
      sprintf(connection,"%d",whichconn);
      /* ??? httpd would be killed */
      //sleep(2);
      vcc_config_init();
      pppoe_config_init();
      pppoa_config_init();
      bridged_config_init();
      routed_config_init();
#ifdef CLIP_SUPPORT
      clip_config_init();
#endif
#ifdef MPPPOE_SUPPORT
      mpppoe_config_init();
#endif
#ifdef IPPPOE_SUPPORT
      ipppoe_config_init();	  
#endif
      strcpy(connection,temp_connection);
      memset(cmd,0,sizeof(cmd));
 
      /* get encapmode */
      printf("wan encapmode : %s\n",encapmode);

      /* get wan interface connection */
      
  
      /* 1483 bridge */  
      if(!strcmp(encapmode,"1483bridged")){
           /* if work mode is 1483 bridge */
	    /* get wan hostname */   
           memset(name,0,sizeof(name));		   
           strncpy(name,nvram_safe_get("wan_hostname"),sizeof(name));		   
 	   printf("wan host name:  %s\n",name);	   

	   /* get ipppoe demand */
      #ifdef IPPPOE_SUPPORT
	   wcon_temp = whichconn;
	   foreach(word, nvram_safe_get("ipppoe_enable"), next) 
	   {
				if(wcon_temp-- == 0) 
				{
					ipppoe_enable = word;
					break;
				}
	   }
	
 	   if(i_username == NULL)
	         i_username = "0";
		
       	   if(i_password == NULL)
	         i_password = "0";			
      #endif
           /* get wan proto */
           if(!strcmp(dhcpenable,"1")){      
               printf("wan proto is DHCP\n");	  
	 	 /* get wan hostname */	   
		 /* set command for run diagnostics tool */	
		 /* diagtool -t 1483bridge -t dhcp -i conn -d demand*/
      #ifdef IPPPOE_SUPPORT
		 sprintf(cmd,"/usr/sbin/diagtool -m %s -t dhcp -i %d -d %s -u %s -p %s",encapmode,whichconn,ipppoe_enable,i_username,i_password);	   
      #else
		 sprintf(cmd,"/usr/sbin/diagtool -m %s -t dhcp -i %d",encapmode,whichconn);	   
      #endif		 
           }			   
           else{
	  	 printf("wan proto is Static\n");  
		 printf("wan IP address is %s\n",b_ipaddr);
		 printf("gateway IP address is %s\n",b_gateway);
		 /* get wan MAC addr */
		 wmac = nvram_safe_get("wan_def_hwaddr");
		 printf("wan1 Mac address is %s\n",wmac);
               
    	        /* diagtool -m  1483bridge -t static -a XXX.XXX.XXX.XXX -g XXX.XXX.XXX.XXX -c XX:XX:XX:XX:XX:XX -i interfacename -d demand*
		*/
		 if(b_ipaddr == NULL)
			 b_ipaddr = "0.0.0.0";

		if(b_gateway == NULL)
			 b_gateway = "0.0.0.0";
	#ifdef IPPPOE_SUPPORT
		 sprintf(cmd,"/usr/sbin/diagtool -m %s -t static -a %s -g %s -c %s -i %d -d %s -u %s -p %s",encapmode,b_ipaddr,b_gateway,wmac,whichconn,i_demand,i_username,i_password);	
	#else
		 sprintf(cmd,"/usr/sbin/diagtool -m %s -t static -a %s -g %s -c %s -i %d",encapmode,b_ipaddr,b_gateway,wmac,whichconn);	
	#endif		
           }	 
      }

      /* 1483 router */
      else if(!strcmp(encapmode,"routed")){
	     printf("wan IP address is %s\n",r_ipaddr);
	     printf("gateway IP address is %s\n",r_gateway);
		 
	     /* diagtool -m routed -g XXX.XXX.XXX.XXX */  	 
	     if(b_gateway == NULL)
		 	b_gateway = "0.0.0.0";
 	     sprintf(cmd,"/usr/sbin/diagtool -m %s -g %s",encapmode,b_gateway);	    	 
      }

      /* pppoe */
      else if(!strcmp(encapmode,"pppoe")){
	     /* get user and  password */	
	     printf("pppoe user is %s\n",e_username);
	     printf("pppoe password is %s\n",e_password);

	     /* get multiplex */		 
	     printf("multiplex : %s\n",multiplex);	 

	     /* get mpppoe demand */
	#ifdef MPPPOE_SUPPORT
 	     wcon_temp = whichconn;	  
             foreach(word, nvram_safe_get("mpppoe_enable"), next) 
  	     {
				if(wcon_temp-- == 0) 
				{
					mpppoe_enable = word;
					break;
				}
	     }
	#endif
             /* diagtool -m pppoe -u user -p password -v multiplex*/
	     if(e_username == NULL)
		    e_username = "0";
	     if(e_password == NULL)
		    e_password = "0";
	#ifdef MPPPOE_SUPPORT
	     if(m_username == NULL)
	            m_username = "0";
       	     if(m_password == NULL)
	            m_password = "0";		    
	     snprintf(cmd,sizeof(cmd),"/usr/sbin/diagtool -m %s -u %s -p %s -v %s -i %d -d %s -r %s -s %s",encapmode,e_username,e_password,multiplex, whichconn,mpppoe_enable,m_username,m_password);	    	 				 	
	#else
	     snprintf(cmd,sizeof(cmd),"/usr/sbin/diagtool -m %s -u %s -p %s -v %s -i %d",encapmode,e_username,e_password,multiplex, whichconn);	    	 				 	
	#endif    
      }

      /* pppoa */
      else if(!strcmp(encapmode,"pppoa")){
	     /* get user and  password */	
	     printf("pppoa user is %s\n",a_username);
	     printf("pppoa password is %s\n",a_password);

	     /* get multiplex */		 
	     printf("multiplex : %s\n",multiplex);	 
            /* diagtool -m pppoe -u user -p password -v multiplex*/
	     if(e_username == NULL)
		    e_username = "0";
	     if(e_password == NULL)
		    e_password = "0";		
	     snprintf(cmd,sizeof(cmd),"/usr/sbin/diagtool -m %s -u %s -p %s -v %s -i %d",encapmode,a_username,a_password,multiplex, whichconn);	    	 				 
      }
      /* ipoa */
#ifdef CLIP_SUPPORT
      else if(!strcmp(encapmode,"clip")){	
	     /* get internet ip address */
             printf("ipoa: internet ip address : %s\n",c_ipaddr);
	     /* get gateway ip address */
	     printf("ipoa: gateway ip address: %s\n",c_gateway);	 

	     if(c_ipaddr == NULL)
		 c_ipaddr = "0.0.0.0";
	     if(c_gateway == NULL)
		 c_gateway = "0.0.0.0";

	     snprintf(cmd,sizeof(cmd),"/usr/sbin/diagtool -m %s -a %s -g %s -i %d",encapmode, c_ipaddr,c_gateway,whichconn);	    	 				 	 
      }	  
#endif
      /* another work mode */	  
      else {
	     printf("unknown work mode\n");	
	     return 0;	 
      }
	  
      /* create process to run diagnostics tool */	  
      printf("command : %s\n",cmd);	  
      system(cmd);
      return 0;	  
}
