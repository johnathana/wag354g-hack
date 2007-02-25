/* vi: set sw=4 ts=4: */
/*
 * Mini syslogd implementation for busybox
 *
 * Copyright (C) 1999,2000,2001 by Lineo, inc.
 * Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2001 by Gennady Feldman <gfeldman@cachier.com>
 *
 * Maintainer: Gennady Feldman <gena01@cachier.com> as of Mar 12, 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/param.h>
#include <getopt.h>

#include "busybox.h"
/***********songtao 4.6.2004*******************/
#include <arpa/inet.h>
#include <netinet/in.h>
/*********************************************/
/* SYSLOG_NAMES defined to pull some extra junk from syslog.h */
#define SYSLOG_NAMES
#include <sys/syslog.h>
#include <sys/uio.h>
/************************songtao 4.7.2004***************************/
//songtao 525
char *time_zone;
struct time_zone_map *v;
char *abbre;
int argc_sys;
char **argv_sys;
char *t_buff;
char t_buff_all[36];
//songtao 525
char *localip;
	char *logipaddr;
        //int z;
       // char *srvr_addr=NULL;
       // struct sockaddr_in adr_srvr;
        int len_inet;
        int loop;
        //int s;
        char snmphead[]={0x30,0x82,0x00,0x5f,0x02,0x01,0x00,0x04,0x06,0x70,0x75,0x62,0x6c,0x69,0x63,0xa4,0x82,0x00,0x50,0x06,0x0a,0x2b,0x06,0x01,0x04,0x01,0x9e,0x73,0x02,0x02,0x01,0x40,0x04,0x0a,0x80,0x22,0xde,0x02,0x01,0x06,0x02,0x01,0x01,0x43,0x04,0x00,0x00,0x00,0x0c,0x30,0x82,0x00,0x2e,0x30,0x82,0x00,0x2a,0x06,0x0a,0x2b,0x06,0x01,0x04,0x01,0x9e,0x73,0x01,0x01,0x00,0x04,0x82,0x00,0x1a};
        char snmppkg[1000];
        int varlen=0;
        int i_len=0;
	char *localip;
	char *logipaddr;
        char lanipaddr[15];
        int  logloop=0 ;
        int  log_i,log_j;
        int  sockopt;	
	char logipaddr_ay[3];
	int  snmpflag=0;
        char type[2];
         int log_len=0;
		char color; 
    struct time_zone_map time_map[]={
        {"kwajalein","-12 1 0","IDLW "},	
        {"Midway Island,Samoa",    "-11 1 0","NT   "},	
        {"Hawaii",   "-10 1 0","HST  "},	
        {"Alaska",   "-09 1 1","YST  "},	
        {"Pacific Time","-08 1 1","PST  "},	
        {"Arizona","-07 1 0","MST  "},	
        {"Mountain Time","-07 2 1","PDT  "},	
        {"Mexico","-06 1 0","CST  "},	
        {"Central Time","-06 2 1","MDT  "},	
        {"Indiana East,Colobia,Panama","-05 1 0","CDT  "},	
        {"Eastern Time","-05 2 1","ACT  "},	
        {"Bolivia","-04 1 0","ACST "},	
        {"Atlantic Time","-04 2 1","AST  "},	
        {"Newfoundland","-03.5 1 1","NFT  "},	
        {"Guyana","-03 1 0","AWT  "},	
        {"Brazil East,Greenland","-03 2 1","ADT  "},	
        {"Mid-Atlantic","-02 1 0","NDT  "},	
        {"Azores","-01 1 2","WAT  "},	
        {"Gambia,Liberia,Morocco","+00 1 0","WET  "},	
        {"England","+00 2 2","GMT "},	
        {"Tunisia","+01 1 0","MEWT "},	
        {"France,Germany,Italy","+01 2 2","MEZ  "},	
        {"South Africa","+02 1 0","CEST "},	
        {"Greece,Ukraine,Romania,Turkey","+02 2 2","EET  "},	
        {"Iraq,Jordan,Kuwait","+03 1 0","BT   "},	
        {"Armenia","+04 1 0","SCT  "},	
        {"Pakistan,Russia","+05 1 0","IOT  "},	
        {"Bangladesh,Russia","+06 1 0","ALMT "},	
        {"Thailand,Russia","+07 1 0","CXT  "},	
        {"China,Hong Kong,Australia Western","+08 1 0","CCT  "},	
        {"Singapore,Taiwan,Russia","+08 2 1","WST  "},	
        {"Japan,Korea","+09 1 0","JST  "},	
        {"Guam,Russia","+10 1 0","GST  "},	
        {"Australia","+10 2 4","AEST "},	
        {"Solomon Islands","+11 1 0","AESST"},	
        {"Fiji","+12 1 0","NZST"},	
        {"New Zealand","+12 2 4","NZST "},	

	}; 
/*******************songtao end*************************/
/* Path for the file where all log messages are written */
#define __LOG_FILE "/var/log/messages"

/* Path to the unix socket */
static char lfile[BUFSIZ];

static char *logFilePath = __LOG_FILE;

/* interval between marks in seconds */
static int MarkInterval = 20 * 60;

/* localhost's name */
static char LocalHostName[32];

#ifdef BB_FEATURE_REMOTE_LOG
#include <netinet/in.h>
/* udp socket for logging to remote host */
static int remotefd = -1;
/* where do we log? */
static char *RemoteHost;
/* what port to log to? */
static int RemotePort = 514;
/* what remote facility to use? -1 is use same as for local logging */
static int RemoteFacility = -1;
/* To remote log or not to remote log, that is the question. */
static int doRemoteLog = FALSE;
static int local_logging = FALSE;
#endif

/* circular buffer variables/structures */
#ifdef BB_FEATURE_IPC_SYSLOG

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* our shared key */
static const long KEY_ID = 0x414e4547; /*"GENA"*/

// Semaphore operation structures
static struct shbuf_ds {
	int size;		// size of data written
	int head;		// start of message list
	int tail;		// end of message list
	char data[1];		// data/messages
} *buf = NULL;			// shared memory pointer

static struct sembuf SMwup[1] = {{1, -1, IPC_NOWAIT}}; // set SMwup
static struct sembuf SMwdn[3] = {{0, 0}, {1, 0}, {1, +1}}; // set SMwdn

static int 	shmid = -1;	// ipc shared memory id
static int 	s_semid = -1;	// ipc semaphore id
int	data_size = 16000; // data size
int	shm_size = 16000 + sizeof(*buf); // our buffer size
static int circular_logging = FALSE;

/*
 * sem_up - up()'s a semaphore.
 */
static inline void sem_up(int semid)
{
	if ( semop(semid, SMwup, 1) == -1 )
		perror_msg_and_die("semop[SMwup]");
}

/*
 * sem_down - down()'s a semaphore
 */
static inline void sem_down(int semid)
{
	if ( semop(semid, SMwdn, 3) == -1 )
		perror_msg_and_die("semop[SMwdn]");
}


void ipcsyslog_cleanup(void){
	printf("Exiting Syslogd!\n");
	if (shmid != -1)
		shmdt(buf);

	if (shmid != -1)
		shmctl(shmid, IPC_RMID, NULL);
	if (s_semid != -1)
		semctl(s_semid, 0, IPC_RMID, 0);
}

void ipcsyslog_init(void){
	if (buf == NULL){
	    if ((shmid = shmget(KEY_ID, shm_size, IPC_CREAT | 1023)) == -1)
		    	perror_msg_and_die("shmget");


	    if ((buf = shmat(shmid, NULL, 0)) == NULL)
    			perror_msg_and_die("shmat");


	    buf->size=data_size;
	    buf->head=buf->tail=0;

	    // we'll trust the OS to set initial semval to 0 (let's hope)
	    if ((s_semid = semget(KEY_ID, 2, IPC_CREAT | IPC_EXCL | 1023)) == -1){
	    	if (errno == EEXIST){
		   if ((s_semid = semget(KEY_ID, 2, 0)) == -1)
		    perror_msg_and_die("semget");
		}else
    			perror_msg_and_die("semget");
	    }
	}else{
		printf("Buffer already allocated just grab the semaphore?");
	}
}

/* write message to buffer */
void circ_message(const char *msg){
	int l=strlen(msg)+1; /* count the whole message w/ '\0' included */

	sem_down(s_semid);

	/*
	 * Circular Buffer Algorithm:
	 * --------------------------
	 *
	 * Start-off w/ empty buffer of specific size SHM_SIZ
	 * Start filling it up w/ messages. I use '\0' as separator to break up messages.
	 * This is also very handy since we can do printf on message.
	 *
	 * Once the buffer is full we need to get rid of the first message in buffer and
	 * insert the new message. (Note: if the message being added is >1 message then
	 * we will need to "remove" >1 old message from the buffer). The way this is done
	 * is the following:
	 *	When we reach the end of the buffer we set a mark and start from the beginning.
	 *	Now what about the beginning and end of the buffer? Well we have the "head"
	 *	index/pointer which is the starting point for the messages and we have "tail"
	 *	index/pointer which is the ending point for the messages. When we "display" the
	 *	messages we start from the beginning and continue until we reach "tail". If we
	 *	reach end of buffer, then we just start from the beginning (offset 0). "head" and
	 *	"tail" are actually offsets from the beginning of the buffer.
	 *
	 * Note: This algorithm uses Linux IPC mechanism w/ shared memory and semaphores to provide
	 * 	 a threasafe way of handling shared memory operations.
	 */
	if ( (buf->tail + l) < buf->size ){
		/* before we append the message we need to check the HEAD so that we won't
		   overwrite any of the message that we still need and adjust HEAD to point
		   to the next message! */
		if ( buf->tail < buf->head){
			if ( (buf->tail + l) >= buf->head ){
			  /* we need to move the HEAD to point to the next message
			   * Theoretically we have enough room to add the whole message to the
			   * buffer, because of the first outer IF statement, so we don't have
			   * to worry about overflows here!
			   */
			   int k= buf->tail + l - buf->head; /* we need to know how many bytes
			   					we are overwriting to make
								enough room */
			   char *c=memchr(buf->data+buf->head + k,'\0',buf->size - (buf->head + k));
			   if (c != NULL) {/* do a sanity check just in case! */
			   	buf->head = c - buf->data + 1; /* we need to convert pointer to
								  offset + skip the '\0' since
								  we need to point to the beginning
								  of the next message */
				/* Note: HEAD is only used to "retrieve" messages, it's not used
					when writing messages into our buffer */
			   }else{ /* show an error message to know we messed up? */
			   	printf("Weird! Can't find the terminator token??? \n");
			   	buf->head=0;
			   }
			}
		} /* in other cases no overflows have been done yet, so we don't care! */

		/* we should be ok to append the message now */
		strncpy(buf->data + buf->tail,msg,l); /* append our message */
		buf->tail+=l; /* count full message w/ '\0' terminating char */
	}else{
		/* we need to break up the message and "circle" it around */
		char *c;
		int k=buf->tail + l - buf->size; /* count # of bytes we don't fit */
		
		/* We need to move HEAD! This is always the case since we are going
		 * to "circle" the message.
		 */
		c=memchr(buf->data + k ,'\0', buf->size - k);
		
		if (c != NULL) /* if we don't have '\0'??? weird!!! */{
			/* move head pointer*/
			buf->head=c-buf->data+1; 
			
			/* now write the first part of the message */			
			strncpy(buf->data + buf->tail, msg, l - k - 1);
			
			/* ALWAYS terminate end of buffer w/ '\0' */
			buf->data[buf->size-1]='\0'; 
			
			/* now write out the rest of the string to the beginning of the buffer */
			strcpy(buf->data, &msg[l-k-1]);

			/* we need to place the TAIL at the end of the message */
			buf->tail = k + 1;
		}else{
			printf("Weird! Can't find the terminator token from the beginning??? \n");
			buf->head = buf->tail = 0; /* reset buffer, since it's probably corrupted */
		}
		
	}
	sem_up(s_semid);
}
#endif
/* Note: There is also a function called "message()" in init.c */
/* Print a message to the log file. */
static void message (char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
static void message (char *fmt, ...)
{
	int fd;
	struct flock fl;
	va_list arguments;

	fl.l_whence = SEEK_SET;
	fl.l_start  = 0;
	fl.l_len    = 1;

#ifdef BB_FEATURE_IPC_SYSLOG
	if ((circular_logging == TRUE) && (buf != NULL)){
			char b[1024];
			va_start (arguments, fmt);
			vsprintf (b, fmt, arguments);
			va_end (arguments);
			circ_message(b);

	}else
#endif
	if ((fd = device_open (logFilePath,
						   O_WRONLY | O_CREAT | O_NOCTTY | O_APPEND |
						   O_NONBLOCK)) >= 0) {
		fl.l_type = F_WRLCK;
		fcntl (fd, F_SETLKW, &fl);
		va_start (arguments, fmt);
		vdprintf (fd, fmt, arguments);
		va_end (arguments);
		fl.l_type = F_UNLCK;
		fcntl (fd, F_SETLKW, &fl);
		close (fd);
	} else {
		/* Always send console messages to /dev/console so people will see them. */
		if ((fd = device_open (_PATH_CONSOLE,
							   O_WRONLY | O_NOCTTY | O_NONBLOCK)) >= 0) {
			va_start (arguments, fmt);
			vdprintf (fd, fmt, arguments);
			va_end (arguments);
			close (fd);
		} else {
			fprintf (stderr, "Bummer, can't print: ");
			va_start (arguments, fmt);
			vfprintf (stderr, fmt, arguments);
			fflush (stderr);
			va_end (arguments);
		}
	}
}

static void logMessage (int pri, char *msg)
{
	time_t now;
	char *timestamp;
	char *klogdtmp;
	char klogdstr[255];
	int klog_i=0;
	static char res[20] = "";
	CODE *c_pri, *c_fac;
	char msg_buf[1024],*p,*pt,*msg_tmp,msg_new[1024];

	if (pri != 0) {
		for (c_fac = facilitynames;
				c_fac->c_name && !(c_fac->c_val == LOG_FAC(pri) << 3); c_fac++);
		for (c_pri = prioritynames;
				c_pri->c_name && !(c_pri->c_val == LOG_PRI(pri)); c_pri++);
		if (c_fac->c_name == NULL || c_pri->c_name == NULL)
			snprintf(res, sizeof(res), "<%d>", pri);
		else
			snprintf(res, sizeof(res), "%s.%s", c_fac->c_name, c_pri->c_name);
	}

	if (strlen(msg) < 16 || msg[3] != ' ' || msg[6] != ' ' ||
			msg[9] != ':' || msg[12] != ':' || msg[15] != ' ') {
//		time(&now);
//		timestamp = ctime(&now) ;
//	timestamp[15] = '\0';
        date_main_sys( argc_sys,  argv_sys);
		timestamp = t_buff_all;
		timestamp[31] = '\0';
	
		
	} else {
		//timestamp = msg;
        date_main_sys( argc_sys,  argv_sys);
		timestamp = t_buff_all;
		timestamp[31] = '\0';
		msg += 16;
	}

	/* todo: supress duplicates */

#ifdef BB_FEATURE_REMOTE_LOG
	/* send message to remote logger */
	if ( -1 != remotefd){
static const int IOV_COUNT = 2;
		struct iovec iov[IOV_COUNT];
		struct iovec *v = iov;

		memset(&res, 0, sizeof(res));
		if (RemoteFacility == -1)
			snprintf(res, sizeof(res), "<%d>", pri);
		else
			snprintf(res, sizeof(res), "<%d>", LOG_PRI(pri) | RemoteFacility);
		v->iov_base = res ;
		v->iov_len = strlen(res);          
		v++;

		v->iov_base = msg;
		v->iov_len = strlen(msg);          

		if ( -1 == writev(remotefd,iov, IOV_COUNT)){
			error_msg_and_die("syslogd: cannot write to remote file handle on" 
					"%s:%d",RemoteHost,RemotePort);
		}
	}
	if (local_logging == TRUE)
#endif
	{
		/* now spew out the message to wherever it is supposed to go */
//zhangbin
		memset(msg_buf,0,sizeof(msg_buf));
		memset(msg_new,0,sizeof(msg_new));
		memcpy(msg_buf,msg,sizeof(msg_buf));
		/*-------cjg:2004-4-20---------------------------*/
#if 1
		if(((p = strstr(msg_buf, "1n")) != NULL) /*||
		   ((p = strstr(msg_buf, "4r")) != NULL) ||
	       ((p = strstr(msg_buf, "3n")) != NULL) ||
		    ((p = strstr(msg_buf, "3r")) != NULL)*/)
		{
			msg = msg_buf + 2;
			message("%s %s\n", timestamp, msg);
		}
		/*-------------2004-4-20--------------------------*/
		else
#endif   
		if((p = strstr(msg_buf,"IKE")) != NULL)
		{
			if((pt = strchr(msg_buf,'{')) != NULL)
			{
				*pt = '<';
				*(pt + 1) = '<';
			}

			msg = msg_buf + 2;
			message("%s %s\n", timestamp, msg);
#if 0
			printf("msg=%s\n",msg);
			if(!strncmp(msg_buf,": (b)",5))		
			{
				msg_tmp = msg_buf + 5;
			//	sprintf(msg_new,"<FONT color=blue>%s</FONT>",msg_tmp);
			//	strcpy(msg_buf,msg_new);
				msg = msg_tmp;
				message("<FONT color=blue>%s %s</FONT>\n",timestamp,msg);
			}
			else if(!strncmp(msg_buf,": (r)",5))		
			{
				msg_tmp = msg_buf + 5;
//				sprintf(msg_new,"<FONT color=red>%s</FONT>",msg_tmp);
				msg = msg_tmp;
				//strcpy(msg_buf,msg_new);
				message("<FONT color=red>%s %s</FONT>\n",timestamp,msg);
			}
			else if(!strncmp(msg_buf,": I",3))
			{
				msg = msg_buf + 2;
				message("%s %s\n", timestamp, msg);
			}
#endif
		}
		else

		//message("%s %s %s %s\n", timestamp, LocalHostName, res, msg);
		{	
        	if(klogdtmp=strstr(msg,"klogd"))
			{
		        for(klog_i=0;klog_i<(strlen(klogdtmp)-7);klog_i++)
				{
		             klogdstr[klog_i]=klogdtmp[klog_i+7];
			    }
	           klogdstr[strlen(klogdtmp)-7]='\0';
    	       msg=klogdstr;
		 	}
		
			message("%s %s\n", timestamp, msg);
		}
	}

}

static void quit_signal(int sig)
{
	logMessage(LOG_SYSLOG | LOG_INFO, "System log daemon exiting.");
	unlink(lfile);
#ifdef BB_FEATURE_IPC_SYSLOG
	ipcsyslog_cleanup();
#endif

	exit(TRUE);
}

static void domark(int sig)
{
	if (MarkInterval > 0) {
		logMessage(LOG_SYSLOG | LOG_INFO, "-- MARK --");
		alarm(MarkInterval);
	}
}

/* This must be a #define, since when DODEBUG and BUFFERS_GO_IN_BSS are
 * enabled, we otherwise get a "storage size isn't constant error. */
#define BUFSIZE 1023
static int serveConnection (int conn)
{
	RESERVE_BB_BUFFER(tmpbuf, BUFSIZE + 1);
	int    n_read;
	/*********songtao 4.6 .2004***********************/
	int s,z;
	char *srvr_addr=NULL;
        struct sockaddr_in	adr_srvr;
	char pkgsnmp[300];
	int  pkg_i=0;
	int  pkg_j=0;
	/*--send snmppkg defined by songtao 2004.4.5----*/
	n_read = read (conn, tmpbuf, BUFSIZE );

	if (n_read > 0) {

		int           pri = (LOG_USER | LOG_NOTICE);
		char          line[ BUFSIZE + 1 ];
		unsigned char c;

		char *p = tmpbuf, *q = line;
		char *pt; //zhangbin -2004.4.15

		tmpbuf[ n_read - 1 ] = '\0';
/*--------add send snmppkg code by songtao 2004.4.5---------------------------*/
             
		    srvr_addr=lanipaddr;
		    //srvr_addr="192.168.1.6";

		    printf("lanipaddr=%s,len=%d\n",srvr_addr,strlen(srvr_addr));
		    logipaddr=logipaddr_ay;      
			
		    memset(&adr_srvr,0,sizeof adr_srvr);
		    adr_srvr.sin_family=AF_INET;
		    adr_srvr.sin_port=htons(162);
		    adr_srvr.sin_addr.s_addr=inet_addr(srvr_addr);
		    if(adr_srvr.sin_addr.s_addr==INADDR_NONE)
		    bail("bad address.");
		    len_inet=sizeof adr_srvr;
		    s=socket(AF_INET,SOCK_DGRAM,0);
                    if(s==-1)
	               bail("socket()");
                    if(!strcmp(logipaddr,"255"))
	             {
		         sockopt = 1;
		         setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *) &sockopt, sizeof(sockopt));
		     }
/*------------------------------------------------------------------*/	
		while (p && (c = *p) && q < &line[ sizeof (line) - 1 ]) {
			if (c == '<') {
			/* Parse the magic priority number. */
				pri = 0;
				while (isdigit (*(++p))) {
					pri = 10 * pri + (*p - '0');
				}
				if (pri & ~(LOG_FACMASK | LOG_PRIMASK)){
					pri = (LOG_USER | LOG_NOTICE);
				}
			} else if (c == '\n') {
				*q++ = ' ';
			} else if (iscntrl (c) && (c < 0177)) {
				*q++ = '^';
				*q++ = c ^ 0100;
			} else {
				*q++ = c;
			}
			p++;
		}
		*q = '\0';
		/* Now log it */
		/* Now log it-----songtao 4.7.2004 */
/////////////////////////////////1111//////////////////////////////////
        printf("the line =%s\n",line);
		type[0]=line[18];
        type[1]='\0';
		color=line[19];
        snmpflag=atoi(type); 
	   if((snmpflag==1)||(snmpflag==2)||(snmpflag==3)||(snmpflag==4)){  
		 if((color=='r')||(color=='b')||(color=='n')){
		   for(pkg_i=0;pkg_i<strlen(line);pkg_i++)
		 {  
			//zhangbin -2004.4.15 
			 if((pt = strchr(line,'{')) != NULL)
			{
				*pt = '<';
				*(pt + 1) = '<';
			}

			 if((pkg_i!=18)&&(pkg_i!=19))
			 { pkgsnmp[pkg_j]=line[pkg_i];
			   pkg_j++;}
		 }
		 pkgsnmp[pkg_j]='\0'; 
		 varlen=strlen(pkgsnmp)-18;
         snmphead[3]=69+varlen;
         snmphead[18]=54+varlen;
         snmphead[52]=20+varlen;
         snmphead[56]=16+varlen;
         snmphead[72]=0+varlen;
	     for(i_len=0;i_len<loop;i_len++)
	         snmppkg[i_len]=snmphead[i_len];
	     i_len=0;
         for(i_len=0;i_len<varlen;i_len++)
		    snmppkg[loop+i_len]=pkgsnmp[i_len+18];
   		 z=sendto(s,snmppkg,loop+varlen,0,(struct sockaddr *)&adr_srvr,len_inet);
		    memset(snmppkg,0,sizeof(snmppkg));
		    memset(pkgsnmp,0,sizeof(pkgsnmp));
			pkg_j=0;
         if(z<0)
	        bail("sendto(2)");
		 logMessage (pri, line);
		 close(s);
		 /************songtao end**********/
		}
	   }		
		
/////////////////////////////////2222///////////////////////////////////		
		
        type[0]=line[23];
        type[1]='\0';
		color=line[24];
        snmpflag=atoi(type); 
	   if((snmpflag==1)||(snmpflag==2)||(snmpflag==3)||(snmpflag==4)){  
		 if((color=='r')||(color=='b')||(color=='n')){
		   for(pkg_i=0;pkg_i<strlen(line);pkg_i++)
		 {   if((pkg_i!=23)&&(pkg_i!=24))
			 { pkgsnmp[pkg_j]=line[pkg_i];
			   pkg_j++;}
		 }
		 pkgsnmp[pkg_j]='\0'; 
		 varlen=strlen(pkgsnmp)-23;
         snmphead[3]=69+varlen;
         snmphead[18]=54+varlen;
         snmphead[52]=20+varlen;
         snmphead[56]=16+varlen;
         snmphead[72]=0+varlen;
	     for(i_len=0;i_len<loop;i_len++)
	         snmppkg[i_len]=snmphead[i_len];
	     i_len=0;
         for(i_len=0;i_len<varlen;i_len++)
		    snmppkg[loop+i_len]=pkgsnmp[i_len+23];
   		 z=sendto(s,snmppkg,loop+varlen,0,(struct sockaddr *)&adr_srvr,len_inet);
         if(z<0)
	        bail("sendto(2)");
		 logMessage (pri, line);
		 close(s);
		 /************songtao end**********/
		}
	   }
	}
	RELEASE_BB_BUFFER (tmpbuf);
	return n_read;
}


#ifdef BB_FEATURE_REMOTE_LOG
static void init_RemoteLog (void){

  struct sockaddr_in remoteaddr;
  struct hostent *hostinfo;
  int len = sizeof(remoteaddr);

  memset(&remoteaddr, 0, len);

  remotefd = socket(AF_INET, SOCK_DGRAM, 0);

  if (remotefd < 0) {
    error_msg_and_die("syslogd: cannot create socket");
  }

  hostinfo = xgethostbyname(RemoteHost);

  remoteaddr.sin_family = AF_INET;
  remoteaddr.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
  remoteaddr.sin_port = htons(RemotePort);

  /* 
     Since we are using UDP sockets, connect just sets the default host and port 
     for future operations
  */
  if ( 0 != (connect(remotefd, (struct sockaddr *) &remoteaddr, len))){
    error_msg_and_die("syslogd: cannot connect to remote host %s:%d", RemoteHost, RemotePort);
  }

}
#endif

static void doSyslogd (void) __attribute__ ((noreturn));
static void doSyslogd (void)
{
	struct sockaddr_un sunx;
	socklen_t addrLength;


	int sock_fd;
	fd_set fds;

	/* Set up signal handlers. */
	signal (SIGINT,  quit_signal);
	signal (SIGTERM, quit_signal);
	signal (SIGQUIT, quit_signal);
	signal (SIGHUP,  SIG_IGN);
	signal (SIGCHLD,  SIG_IGN);
#ifdef SIGCLD
	signal (SIGCLD,  SIG_IGN);
#endif
	signal (SIGALRM, domark);
	alarm (MarkInterval);
////////////songtao 4.6 .2004
                 
    logipaddr=nvram_get("log_ipaddr");
	//logipaddr="6";
    for(log_len=0;log_len<strlen(logipaddr);log_len++)
         logipaddr_ay[log_len]=logipaddr[log_len];
    logipaddr_ay[log_len]='\0';	
    printf("log_ipaddr=%s\n",logipaddr);
    loop=sizeof(snmphead);
               
    snmphead[3]=69;
    snmphead[18]=54;
    snmphead[52]=20;
    snmphead[56]=16;
    snmphead[72]=0;          
//////////////////////songtao end
	/* Create the syslog file so realpath() can work. */
	if (realpath (_PATH_LOG, lfile) != NULL)
		unlink (lfile);

	memset (&sunx, 0, sizeof (sunx));
	sunx.sun_family = AF_UNIX;
	strncpy (sunx.sun_path, lfile, sizeof (sunx.sun_path));
	if ((sock_fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
		perror_msg_and_die ("Couldn't get file descriptor for socket " _PATH_LOG);

	addrLength = sizeof (sunx.sun_family) + strlen (sunx.sun_path);
	if ((bind (sock_fd, (struct sockaddr *) &sunx, addrLength)) || (listen (sock_fd, 5)))
		perror_msg_and_die ("Could not connect to socket " _PATH_LOG);

	if (chmod (lfile, 0666) < 0)
		perror_msg_and_die ("Could not set permission on " _PATH_LOG);

	FD_ZERO (&fds);
	FD_SET (sock_fd, &fds);

#ifdef BB_FEATURE_IPC_SYSLOG
	if (circular_logging == TRUE ){
	   ipcsyslog_init();
	}
#endif

        #ifdef BB_FEATURE_REMOTE_LOG
        if (doRemoteLog == TRUE){
          init_RemoteLog();
        }
        #endif

	logMessage (LOG_SYSLOG | LOG_INFO, "syslogd started: " BB_BANNER);
//////songtao 4.6.2004
    localip=nvram_get("lan_ipaddr");
    for(log_i=0;log_i<strlen(localip);log_i++)
         {
           lanipaddr[log_i]=localip[log_i];
           if(localip[log_i]=='.')
             logloop++;
           if(logloop==3)
             break;
  		}										         
     log_i++;
     printf("the 1 logipaddr_ay=%s\n",logipaddr_ay);
     for(log_j=0;log_j<sizeof(logipaddr_ay);log_j++)
         lanipaddr[log_i+log_j]=logipaddr_ay[log_j];
     lanipaddr[log_i-- +log_j--]='\0';
/*     srvr_addr=lanipaddr;
     printf("lanipaddr=%s,len=%d\n",lanipaddr,strlen(srvr_addr));
     logipaddr=logipaddr_ay;      
		
     memset(&adr_srvr,0,sizeof adr_srvr);
     adr_srvr.sin_family=AF_INET;
     adr_srvr.sin_port=htons(162);
     adr_srvr.sin_addr.s_addr=inet_addr(srvr_addr);
     if(adr_srvr.sin_addr.s_addr==INADDR_NONE)
          bail("bad address.");
     len_inet=sizeof adr_srvr;
     s=socket(AF_INET,SOCK_DGRAM,0);
     if(s==-1)
            bail("socket()");
     if(!strcmp(logipaddr,"255"))
      {
         printf("2222222\n");
         sockopt = 1;
         setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *) &sockopt, sizeof(sockopt));
      }*/
///////songtao end
	for (;;) {

		fd_set readfds;
		int    n_ready;
		int    fd;

		memcpy (&readfds, &fds, sizeof (fds));

		if ((n_ready = select (FD_SETSIZE, &readfds, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR) continue; /* alarm may have happened. */
			perror_msg_and_die ("select error");
		}

		for (fd = 0; (n_ready > 0) && (fd < FD_SETSIZE); fd++) {
			if (FD_ISSET (fd, &readfds)) {

				--n_ready;

				if (fd == sock_fd) {
					int   conn;

					//printf("New Connection request.\n");
					if ((conn = accept (sock_fd, (struct sockaddr *) &sunx, &addrLength)) < 0) {
						perror_msg_and_die ("accept error");
					}

					FD_SET(conn, &fds);
					//printf("conn: %i, set_size: %i\n",conn,FD_SETSIZE);
			  	} else {
					//printf("Serving connection: %i\n",fd);
					  if ( serveConnection(fd) <= 0 ) {
					    close (fd);
					    FD_CLR(fd, &fds);
            }
				} /* fd == sock_fd */
			}/* FD_ISSET() */
		}/* for */
	} /* for main loop */
}

extern int syslogd_main(int argc, char **argv)
{
	int opt;
	int doFork = TRUE;

	char *p;
    argc_sys=argc;
	argv_sys=argv;
	/* do normal option parsing */
	time_zone=nvram_get("time_zone");
	while ((opt = getopt(argc, argv, "m:nO:R:F:LC")) > 0) {
		switch (opt) {
			case 'm':
				MarkInterval = atoi(optarg) * 60;
				break;
			case 'n':
				doFork = FALSE;
				break;
			case 'O':
				logFilePath = strdup(optarg);
				break;
#ifdef BB_FEATURE_REMOTE_LOG
			case 'R':
				RemoteHost = strdup(optarg);
				if ( (p = strchr(RemoteHost, ':'))){
					RemotePort = atoi(p+1);
					*p = '\0';
				}
				doRemoteLog = TRUE;
				break;
			case 'F':
				RemoteFacility = atoi(optarg);
				if ((RemoteFacility < 0) | (RemoteFacility > 7)) {
					show_usage();
					return(EXIT_FAILURE);
				}
				/* convert to LOG_LOCAL* value */
				RemoteFacility = (RemoteFacility << 3) + LOG_LOCAL0;
				break;
			case 'L':
				local_logging = TRUE;
				break;
#endif
#ifdef BB_FEATURE_IPC_SYSLOG
			case 'C':
				circular_logging = TRUE;
				break;
#endif
			default:
				show_usage();
		}
	}

#ifdef BB_FEATURE_REMOTE_LOG
	/* If they have not specified remote logging, then log locally */
	if (doRemoteLog == FALSE) {
		local_logging = TRUE;
		/* -F was specified without -R */
		if (RemoteFacility != -1) {
			show_usage();
			return(EXIT_FAILURE);
		}
	}
#endif


	/* Store away localhost's name before the fork */
	gethostname(LocalHostName, sizeof(LocalHostName));
	if ((p = strchr(LocalHostName, '.'))) {
		*p++ = '\0';
	}

	umask(0);

	if (doFork == TRUE) {
		if (daemon(0, 1) < 0)
			perror_msg_and_die("daemon");
	}
    for(v =time_map  ; v < &time_map[37]; v++) {
       if(!strcmp(v->time_zone, time_zone))
         {
            abbre=v->abbre;
            printf("ok-abbre!!!!!!!!!!!!!!!!!!\n");
         }
	   printf("the v->abbre=%s,time_zone=%s,v->time_zone\n",v->abbre,time_zone,v->time_zone);
	   
      }
	
	doSyslogd();

	return EXIT_SUCCESS;
}
/*****************songtao 4.6.2004****************************************/
int
nvram_init(void *unused)
{
         if ((nvram_fd = open(PATH_DEV_NVRAM, O_RDWR)) < 0)
                goto err;
         return 0;
 err:
         perror(PATH_DEV_NVRAM);
         return errno;
}

 char *
 nvram_get(const char *name)
{
       size_t count = strlen(name) + 1;
       char *off;
       memset((void *)nvram_value, 0, MAX_LEN);
       off = (char *)nvram_value;
       if (nvram_fd < 0)
          if (nvram_init(NULL))
             return NULL;
       strcpy((char *) off, name);
       count = read(nvram_fd, off, count);
       return off;
}

void bail(const char *on_what){
      fputs(strerror(errno),stderr);
      fputs(":",stderr);
      fputs(on_what,stderr);
      fputc('\n',stderr);
 }
////////////////////////////////////////sogntao525////////////////////////////
static struct tm *date_conv_time(struct tm *tm_time, const char *t_string)
{
	    int nr;

		    nr = sscanf(t_string, "%2d%2d%2d%2d%d",
					                &(tm_time->tm_mon),
		       		                &(tm_time->tm_mday),
			     	                &(tm_time->tm_hour),
								    &(tm_time->tm_min), &(tm_time->tm_year));

			    if (nr < 4 || nr > 5) {
					        error_msg_and_die(invalid_date, t_string);
							    }

				    /* correct for century  - minor Y2K problem here? */
			    if (tm_time->tm_year >= 1900)
			        tm_time->tm_year -= 1900;
					    /* adjust date */
    			    tm_time->tm_mon -= 1;


    return (tm_time);

}
static struct tm *date_conv_ftime(struct tm *tm_time, const char *t_string)
{
	    struct tm t;

		    /* Parse input and assign appropriately to tm_time */

		    if (t=*tm_time,sscanf(t_string, "%d:%d:%d",
		             &t.tm_hour, &t.tm_min, &t.tm_sec) == 3) {
                    /* no adjustments needed */
				    } else if (t=*tm_time,sscanf(t_string, "%d:%d",
                     &t.tm_hour, &t.tm_min) == 2) {
                    /* no adjustments needed */
    } else if (t=*tm_time,sscanf(t_string, "%d.%d-%d:%d:%d",
                      &t.tm_mon,
                      &t.tm_mday,
                      &t.tm_hour,
                      &t.tm_min, &t.tm_sec) == 5) {

                      t.tm_mon -= 1;  /* Adjust dates from 1-12 to 0-11 */

    } else if (t=*tm_time,sscanf(t_string, "%d.%d-%d:%d",
		              &t.tm_mon,
			          &t.tm_mday,
			          &t.tm_hour, &t.tm_min) == 4) {

                      t.tm_mon -= 1;  /* Adjust dates from 1-12 to 0-11 */
		
    } else if (t=*tm_time,sscanf(t_string, "%d.%d.%d-%d:%d:%d",
				                      &t.tm_year,
									  &t.tm_mon,
				                      &t.tm_mday,
				                      &t.tm_hour,
				                      &t.tm_min, &t.tm_sec) == 6) {

		                              t.tm_year -= 1900;  /* Adjust years */
				                      t.tm_mon -= 1;  /* Adjust dates from 1-12 to 0-11 */

						    } else if (t=*tm_time,sscanf(t_string, "%d.%d.%d-%d:%d",
				                      &t.tm_year,
			                          &t.tm_mon,
			                          &t.tm_mday,
				                      &t.tm_hour, &t.tm_min) == 5) {
							        t.tm_year -= 1900;  /* Adjust years */
				     		        t.tm_mon -= 1;  /* Adjust dates from 1-12 to 0-11 */
    } else {
        error_msg_and_die(invalid_date, t_string);
    }
	    *tm_time = t;
    return (tm_time);
}

int date_main_sys(int argc, char **argv)
{
    char *date_str = NULL;
    char *date_fmt = NULL;
    int c;
    int set_time = 0;
    int rfc822 = 0;
    int utc = 0;
    int use_arg = 0;
    time_t tm;
    struct tm tm_time;
	//songtao 525
	char t_buff_t[11];
	int  t_buff_i=0;
	int t_buff_len=0;
	int all=0;
	char zone;
	//
    while ((c = getopt(argc, argv, "Rs:ud:")) != EOF) {
	        switch (c) {
            case 'R':
                rfc822 = 1;
                break;
            case 's':
                set_time = 1;
                if ((date_str != NULL) || ((date_str = optarg) == NULL)) {
	                    show_usage();
		                }
                break;
            case 'u':
                utc = 1;
                if (putenv("TZ=UTC0") != 0)
	                    error_msg_and_die(memory_exhausted);
                break;
            case 'd':
                use_arg = 1;
                if ((date_str != NULL) || ((date_str = optarg) == NULL))
	                    show_usage();
                break;
            default:
                show_usage();
        }
    }
    if ((date_fmt == NULL) && (optind < argc) && (argv[optind][0] == '+'))
	        date_fmt = &argv[optind][1];   /* Skip over the '+' */
    else if (date_str == NULL) {
	        set_time = 1;
		        date_str = argv[optind];
    }
    time(&tm);
    memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
    /* Zero out fields - take her back to midnight! */
    if (date_str != NULL) {
	        tm_time.tm_sec = 0;
	        tm_time.tm_min = 0;
	        tm_time.tm_hour = 0;
	}
    if (date_str != NULL) {

		   if (strchr(date_str, ':') != NULL) {
		      date_conv_ftime(&tm_time, date_str);
		   } else {
		      date_conv_time(&tm_time, date_str);
	 }
     tm = mktime(&tm_time);
	     if (tm < 0)
	         error_msg_and_die(invalid_date, date_str);
	     if ( utc ) {
	        if (putenv("TZ=UTC0") != 0)
	             error_msg_and_die(memory_exhausted);
     }
        if (set_time) {
	           if (stime(&tm) < 0) {
	               perror_msg("cannot set date");
		            }
	          }
	    }
    if (date_fmt == NULL) {
	        date_fmt = (rfc822 ? (utc? "%a, %_d %b %Y %H:%M:%S GMT": "%a, %_d %b %Y %H:%M:%S %z"): "%a %b %e %H:%M:%S %Z %Y");

	    } else if (*date_fmt == '\0') {
	        /* Imitate what GNU 'date' does with NO format string! */
	        printf("\n");
	        return EXIT_SUCCESS;
	    }
    if (strncmp(date_fmt, "%f", 2) == 0) {
		        date_fmt = "%Y.%m.%d-%H:%M:%S";
				    }

	    /* Print OUTPUT (after ALL that!) */
	    t_buff = xmalloc(201);
	    strftime(t_buff, 200, date_fmt, &tm_time);
		if(t_buff[24]=='1')
			zone='a';
		else if(t_buff[24]=='2')
			zone='b';
		for(t_buff_i=0;t_buff_i<11;t_buff_i++)	
	      t_buff_t[t_buff_i]=t_buff[t_buff_i];
		t_buff_t[11]='\0';
		printf("the t_buff_t=%s\n",t_buff_t);
		for(t_buff_i=0;t_buff_i<13;t_buff_i++)
		   t_buff[t_buff_i]=t_buff[11+t_buff_i];
			printf("the date0=%s\n",t_buff);

        for(t_buff_i=13;t_buff_i<=23;t_buff_i++)
			t_buff[t_buff_i]=t_buff_t[t_buff_i-13];
			
	     t_buff_len=strlen(t_buff);
		 t_buff_all[0]=zone;
		 for(t_buff_i=0;t_buff_i<t_buff_len;t_buff_i++)    
         {  if(t_buff_i<=8)
			  t_buff_all[t_buff_i+1]=t_buff[t_buff_i];
			if(t_buff_i>=12)
			  t_buff_all[t_buff_i+2+1]=t_buff[t_buff_i];	
		 }
		 t_buff_all[10]=abbre[0];
		 t_buff_all[11]=abbre[1];
		 t_buff_all[12]=abbre[2];
		 t_buff_all[13]=abbre[3];
		 t_buff_all[14]=abbre[4];

	   	   
			printf("the date=%s\n",t_buff_all);
    return EXIT_SUCCESS;
}
	
						
				
	
/******************songtao end**********************************************/
/*
Local Variables
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
