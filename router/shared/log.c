
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
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/klog.h>
#include <netdb.h>

#include <broadcom.h>
#include <support.h>
#include "pvc.h"

#define LOG_BUF	16384	// max buf, total have 64 entries
#define IFNAMSIZ 16

extern int status;
extern int cl_page;
extern int refresh;
extern int selet_flags;
extern struct logmsg *head;
extern struct show_page *cur_page;
extern struct show_page *show_page_head;
extern struct show_page *show_page_tail;
extern struct logmsg *cur_line; 
void syslog_refresh()
{       status=1;
	refresh=1;
	printf("refresh the page!\n");
}
void syslog_pagepre()
{       
	status=2;
	printf("back page!\n");
	
}
void syslog_pagenext()
{
	status=3;
	printf("front page!\n");
}
void syslog_clear()
{
        cl_page=1;
	printf("clear the page!\n");
}
static void bail(const char *on_what){
	  fputs(strerror(errno),stderr);
	  fputs(":",stderr);
	  fputs(on_what,stderr);
	  fputc('\n',stderr);
	//   exit(1);

}

int ej_entry_logmsg(int eid,webs_t wp,int argc, char_t **argv)
{    
    	char *arg;
        char *temp="0";
        int count;
        int ret=0;

       if(ejArgs(argc, argv, "%d", &count) <1 )
        {
          websError(wp, 400, "Insufficient args\n");
          return -1;
        }
       if(gozila_action )
        {
          temp = websGetVar(wp, "sel_sysLogType", NULL);
          selet_flags=atoi(temp);
	  printf("sel_sysLogType= %s\n", temp);
          if(((count == 0) && (!strcmp(temp, "0"))) || ((count == 1) && (!strcmp(temp, "1")))||((count==2)&&(!strcmp(temp,"2")))||((count==3)&&(!strcmp(temp,"3")))||((count==4)&&(!strcmp(temp,"4")))||((count==5)&&(!strcmp(temp,"5"))))
                 return ret=websWrite(wp, "selected");
         }
         else
         {
	  printf("sel_sysLogType= %s\n", temp);
          if(((count == 0) && (!strcmp(temp, "0"))) || ((count == 1) && (!strcmp(temp, "1")))||((count==2)&&(!strcmp(temp,"2")))||((count==3)&&(!strcmp(temp,"3")))||((count==4)&&(!strcmp(temp,"4")))||((count==5)&&(!strcmp(temp,"5"))))
           return ret=websWrite(wp, "selected");
         }

	 return 0;
}
int
ej_dump_syslog(int eid,webs_t wp,int argc,char_t **argv)
{
	FILE *fp;
	char line[254];
	int i = 0;
	int j = 0;
        char temp;
	int local=0;
	char level=' ';
	int selet_type=0;
	int selet_yes=0;
	static int old_selet_type=5;
	int sel_sysLogType=0;
	
//////////////////////////////////////////////////////////////////////////////
//                              function code      

////////////////////////////////////////////////////////////////////////////////
///////


///////////////////      	
        if(cl_page)
	 {      
		cl_page=0;
		websWrite(wp,"%s<br>","");
		if(head!=NULL){
		  free(head);
		  head=NULL;
		}
		while(show_page_head!=NULL)
		{       cur_page=show_page_head;
			show_page_head=show_page_head->next;
			free(cur_page);
		}
		if(show_page_head!=NULL)
			printf("the show_page_head not NULL!!!!!\n");
		eval("rm","/var/log/messages");
		sleep(1);
	}
//////////// /////////////////////////////////////////////////////////////////
///   code for  if uer press button of refresh 
///   open the message file and read 
///   the refresh from 1 change to  2
//////////////////////////////////////////////////////////////////////////////	
 if(refresh==1){
       refresh=2;     
        printf("init open!!!!!\n");
	if(head!=NULL)
	{
	        free(head);
		head=NULL;
        }
        else
	printf("the head it's NULL!!!\n");	
        while(show_page_head!=NULL)
	{    
		cur_page=show_page_head;
		show_page_head=show_page_head->next;
		free(cur_page);
	}
       if((fp = fopen("/var/log/messages", "r")) == NULL) {

		  websWrite(wp,"%s<br>","");
		   refresh=0;
		   return -1;
		        }
/*  init socket for use to snmppkg translate */		       
////////////////////////////////////////////////////////////////////      
///////////////////////////////////////	     
      
/* ////////////////// init socket  end //////////////////////////////// */
      head =  (struct logmsg*)malloc(sizeof(struct logmsg));                                                           
        if(head!=NULL)printf("head ok!!!!!!!\n");	
      memset(head,0,sizeof(struct logmsg));
      cur_line = head;
      cur_line->len=0;
      while(cur_line->len<1000){
 	while( (temp=fgetc(fp))!=EOF)
          {
	   if (temp != 10){
	      if((local!=26)&&(local!=27))
               cur_line->contant[i][j++] = temp;
	      else if(local==26){
	       cur_line->type[i][0]=temp;
	      }//atoi(temptype);}
	      else if(local==27)
               cur_line->level[i]=temp;	      
	      local++;
	   }
	   else{ local=0;
   		 break;
	 	}
	    }	//end while last	  
	if(temp==EOF)
	    break;
	j = 0;
	i++;
	cur_line->len++ ;
	//close socket
       }//end second while
     }
//////////////////////////////////////////////////////////////////////////////
////////////// end the press refresh button///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////////selet display type => system ,vpn ,access,..//////////////
    sel_sysLogType =selet_flags ;
    printf("the sel_sysLogType=%d\n",sel_sysLogType);
    if(sel_sysLogType==0)
      {
         selet_yes=0;	     
         selet_type=0;	     
       }
    else if(sel_sysLogType==1)
      {
	 selet_yes=1;
         selet_type=1;	 
      }
    else if(sel_sysLogType==2)
     {   selet_yes=1;
	 selet_type=2;    
     }
    else if(sel_sysLogType==3)
     {   selet_yes=1;
	 selet_type=3;    
     }
    else if(sel_sysLogType==4)
    {    selet_yes=1;
	 selet_type=4;   
    }
   else  if(sel_sysLogType==5)
   {
	   selet_yes=1;
	   selet_type=5;
   }	
  if((old_selet_type!=selet_type)||(refresh==2)){
	  status=1;
        while(show_page_head!=NULL)
              {
                      cur_page=show_page_head;
                show_page_head=show_page_head->next;
                free(cur_page);
              }
	  show_page_head=NULL;
    	
	  old_selet_type=selet_type;
          show_page_head= create_showpage(head,old_selet_type,selet_yes);
  }
	

//////////////////////////////////////////////////////////////////////////////
////////////////////start display web/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////// 

     printf("the status=%d\n",status);
     if(show_page_head==NULL){
	        websWrite(wp,"%s<br>","");
       printf("the head it's NULL \n");}
     if(show_page_head!=NULL)
      {
        switch(status)
        {
          case 1:
           cur_page = show_page_head;
           break;
	  case 2:
	   if(cur_page->front!=NULL)
	      cur_page=cur_page->front;
	   break;
	  case 3:
	   if(cur_page->next!=NULL)
	   cur_page=cur_page->next;	
	   break;
	  case 0:
	   exit(1);	  
	}      
        if(status>=1&&status<=3){
         for(i=0; i < cur_page->len; i++){
         level=cur_page->level[i];                             
          switch(level){
            case 'r':
	        websWrite(wp,"<FONT Color=red>%s</FONT><br>",cur_page->line[i]);
	      break;
	    case 'b':
	        websWrite(wp,"<FONT Color=green>%s</FONT><br>",cur_page->line[i]);
	      break;  
	    default:
	       websWrite(wp,"%s<br>",cur_page->line[i]);					      
	     break;
           }//end the switch
	 }//end the for loop
       }//end the if(status>1&&status<3)
      }//end the if(head!=NULL)
//////////////////////////////////////////////////////////////////////////////
////////////////////////////end display web///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////			
	if(refresh==2)
	 {
		 fclose(fp);
		 refresh=0;
	 }
         printf("this point it's overend\n");
	return 0;
}
//////////////////////////////////////////////////////////////////////


struct show_page* create_showpage(struct logmsg *head_node,int type,int yes)
{
         struct logmsg *ptr;
	 struct show_page *show_page_head_lo,*show_page_tail;
	 struct show_page *show_page_ptr,*show_page_ptr1;
	 int k,i,j,tmp,relen;
	 int pagemax=0;
	 k=0;
	 relen=0;
/////////////////////////////////////////////////////////////////
        ptr=head_node;
        if(ptr==NULL)
           return NULL;
        show_page_head_lo=show_page_tail=(struct show_page *)malloc(sizeof(struct show_page));
        if(!show_page_head_lo)
           return NULL;
        show_page_head_lo->next=NULL;
	show_page_head_lo->front=NULL;
        show_page_ptr=show_page_head_lo;
	printf("the ptr->len=%d\n",ptr->len);
	while(k<ptr->len){
            		
	    for(i=0;i<30;){
	     if(yes==1){
	      tmp=atoi(ptr->type[k]);	     
	      if(tmp==type){
	         show_page_ptr->level[i]=ptr->level[k];
	         for(j=0;j<125;j++)
	            show_page_ptr->line[i][j]=ptr->contant[k][j];
	        show_page_ptr->len=++i;
		k++;
		relen++;	
                 }//end if((type-ptr->line[k].type)==type)
	        else 
		  k++;
		printf("the k=%d",k);
		if(k>=ptr->len)
			break;
	        }
              else if(yes==0)
               { 
	        tmp=atoi(ptr->type[k]);	     
	        if((tmp==1)||(tmp==2)||(tmp==3)||(tmp==4)||(tmp==5)){
		    printf("the tmp=%d",tmp);   
		   relen++;
                   show_page_ptr->level[i]=ptr->level[k];
                   for(j=0;j<125;j++)
	              show_page_ptr->line[i][j]=ptr->contant[k][j];
                   show_page_ptr->len=++i;
		   k++;
	        }
		else
	           k++;		
		 
		 if(k>ptr->len)
			 break;
               }
	      }//end the for first
           i=0;
           pagemax++;
           if((k>=ptr->len)||(pagemax>10))
	   {       printf("the 2\n");
		   break;}
        else{
	     show_page_ptr1=(struct show_page*)malloc(sizeof(struct show_page));
	     memset(show_page_ptr1,0,sizeof(struct show_page));
	     show_page_ptr->next=show_page_ptr1;
	     show_page_ptr1->front=show_page_ptr;
             show_page_ptr1->next=NULL;
             show_page_tail  =show_page_ptr1;
             show_page_ptr=show_page_ptr1;
          }
        }//end while(1)
	printf("alllen=%d\n",relen);
	if(relen==0)
		show_page_head_lo=NULL;
        return show_page_head_lo;

}
/////////////////////////////////////////////////////////////////////////////
int
ej_dumpvpnlog(int eid, webs_t wp, int argc, char_t **argv)
{
     FILE *fp;
     char readbuf[15];
     int n,shmid;
     void *shmdata;
     char *buffer;
        if((fp = fopen("/tmp/shm","r")) == NULL)
        {
                printf("Cann't open /tmp/shm!\n");
                return -1;
        }
        if((n = fread(readbuf,10,1,fp)) < 0)
	        {
		                printf("can't read anything!\n");
			        }
        readbuf[n] = '\0';
        shmid = atoi(readbuf);
        shmdata = shmat(shmid,0,0);
        buffer = shmdata;
        websWrite(wp,"%s",buffer);
        return 0;
}

////////////////////////////////////////////////////////////////////////////
	
			  
