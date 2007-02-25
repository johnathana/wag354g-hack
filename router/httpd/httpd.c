/* milli_httpd - pretty small HTTP server
** A combination of
** micro_httpd - really small HTTP server
** and
** mini_httpd - small HTTP server
**
** Copyright © 1999,2000 by Jef Poskanzer <jef@acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdarg.h>

#include <cy_conf.h>
#include "httpd.h"
#include <bcmnvram.h>
#include <code_pattern.h>
#include <utils.h>
#include <shutils.h>

//#include <openssl/ssl.h>
#include "common.h"
#include "server.h"

#ifdef NOMAD_SUPPORT
#include "nomad.h"//xiaoqin add 2005.04.05
#endif

#include <error.h>
#include <sys/signal.h>
/*---------------------------------------------------------------*/
//cjg:2004-6-13
#include <signal.h>
#include <syslog.h>
volatile unsigned int remote_enable = 0;
#define REMOTE_TIMEOUT (20 * 60)
#define LOCAL_TIMEOUT (30 * 60)
volatile unsigned int remote_timeout = 0; /*the remote connection is timeout*/
volatile unsigned int remote_in_process = 0;/*the remote connection is active*/
volatile unsigned int local_timeout = 0; /*the local connection is timeout*/
volatile unsigned int local_in_process = 0;/*the local connection is active*/
volatile unsigned int current_ip = 0x00000000;
volatile unsigned int accessing_ip = 0x00000000;

unsigned int remote_pvc_ip = 0x00000000;
char re_ip_des[20];
char re_ip_local[20];
/*-----------------------------------------------------------------*/
#define SERVER_NAME "httpd"
//#define SERVER_PORT 80
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define TIMEOUT	6

/****************** xiaoqin add start 2005.04.05 ******************/
#ifdef NOMAD_SUPPORT
struct mymsgbuf nomadinfo;//lay up some global variables for nomad
char g_chremoteip[MAX_NOMAD][MAX_IPADDR];//restore remote client's IP
#endif
/*********************** xiaoqin add end **************************/

/* A multi-family sockaddr. */
typedef union {
    struct sockaddr sa;
    struct sockaddr_in sa_in;
    } usockaddr;

/* Globals. */
#ifdef HTTPS_SUPPORT
 static SSL * ssl;
#endif
#define DEFAULT_HTTPS_PORT 443
#define DEFAULT_HTTP_PORT 80
extern int do_ssl;
int server_port;
char pid_file[80];

//static FILE *conn_fp;
static webs_t conn_fp; // jimmy, https, 8/4/2003
static char auth_userid[AUTH_MAX];
static char auth_passwd[AUTH_MAX];
static char auth_realm[AUTH_MAX];
int post;

char http_client_ip[20];
extern char *get_mac_from_ip(char *ip);

/* Forwards. */
static int initialize_listen_socket( usockaddr* usaP );
/*modified by zls 2003-1124*/
static int auth_check( char* dirname, char* authorization ,struct mime_handler* handler);
/**added by zls 2004-1112*/
#ifdef MULTIUSER_SUPPORT
static int auth_check_multi(char* dirname,char* authinfo,char* authpass,struct mime_handler* handler);
#endif
//static int auth_check( char* dirname, char* authorization );
void send_authenticate( char* realm );
static void send_error( int status, char* title, char* extra_header, char* text );
static void send_headers( int status, char* title, char* extra_header, char* mime_type );
int b64_decode( const char* str, unsigned char* space, int size );
static int match( const char* pattern, const char* string );
static int match_one( const char* pattern, int patternlen, const char* string );
static void handle_request(void);

extern void ERR_print_errors_fp(FILE *fp);

#ifdef SETUP_WIZARD_FT_SUPPORT
static unsigned int  http_auth_success = 0; //auth failed, or successfully
#endif
//wwzh add for WAN UPGRADE NOTIFY at 2004-06-16
#ifdef WAN_UPGRADE
	extern void wan_upgrade_notify(int sig);
#endif
//junzhao 2004.4.14
void signal_wakeup(int sig)
{
	printf("httpd: receive SIGUSR1 signal!!\n");
	return;
}

static int
initialize_listen_socket( usockaddr* usaP )
    {
    int listen_fd;
    int i;

    memset( usaP, 0, sizeof(usockaddr) );
    usaP->sa.sa_family = AF_INET;
    usaP->sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
    usaP->sa_in.sin_port = htons( server_port );

    listen_fd = socket( usaP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
	{
	perror( "socket" );
	return -1;
	}
    (void) fcntl( listen_fd, F_SETFD, 1 );
    i = 1;
    if ( setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i) ) < 0 )
	{
	perror( "setsockopt" );
	return -1;
	}
    if ( bind( listen_fd, &usaP->sa, sizeof(struct sockaddr_in) ) < 0 )
	{
	perror( "bind" );
	return -1;
	}
    if ( listen( listen_fd, 1024 ) < 0 )
	{
	perror( "listen" );
	return -1;
	}
    return listen_fd;
    }
#ifdef MULTIUSER_SUPPORT
//modified by zls
static int
 auth_check_multi(char* dirname,char* authinfo,char* authpass,struct mime_handler* handler){
  int loop=0;
  char validate_userid[4];
  static char user_temp[AUTH_MAX];
  static char passwd_temp[AUTH_MAX];
  for(loop=0;loop<AUTH_SUM;loop++){
        sprintf(validate_userid,"%d",loop);
        nvram_set("validate_userid",validate_userid);
        handler->auth(user_temp,passwd_temp,dirname);
        if(strcmp(authinfo,user_temp)==0 && strcmp(authpass,passwd_temp)==0){
           return loop+1;
        }
  }
   return 0;
}
#endif
static int
auth_check( char* dirname, char* authorization,struct mime_handler* handler )
    {
    char authinfo[500];
    char* authpass;
    int l;
    static int count = 0;
    static volatile unsigned int First_Reboot = 1;
	
    /* Is this directory unprotected? */
#if 0
    if ( !strlen(auth_passwd) )
	/* Yes, let the request go through. */
	return 1;
#endif
    /* Basic authorization info? */
    if ( !authorization || strncmp( authorization, "Basic ", 6 ) != 0 ) {
	if(First_Reboot == 1)
		First_Reboot = 0;
    	send_authenticate( dirname );
	return 0;
    }

    /* Decode it. */
    l = b64_decode( &(authorization[6]), authinfo, sizeof(authinfo) );
    authinfo[l] = '\0';
    /* Split into user and password. */
    authpass = strchr( authinfo, ':' );
    if ( authpass == (char*) 0 ) {
	/* No colon?  Bogus auth info. */
	send_authenticate( dirname );
    
	return 0;
    }
   *authpass++ = '\0';
   //junzhao
#ifndef  MULTIUSER_SUPPORT
   if(strcmp(auth_userid,authinfo) == 0 && strcmp(auth_passwd,authpass) == 0){
        if(((remote_timeout == 1) && do_ssl) || (local_timeout == 1) || (First_Reboot == 1)){
	goto  out;
	}
   	return 1;
   }
#else
 //modified by zls 2004-1112 from here to out:
   if(!strcmp(authinfo,""))//username cannot be empty
        goto out;
   else
        user_id=auth_check_multi(dirname,authinfo,authpass,handler);
/* Is this the right user and password? */
    if (user_id){
        if(((remote_timeout == 1) && do_ssl) || (local_timeout == 1) || (First_Reboot == 1)){
           goto out;
        }
        sprintf(user_id_set,"%d",user_id -1);
        nvram_set("login_userid",user_id_set);
        return user_id;
    }
#endif
out:   
    send_authenticate( dirname );
    if(do_ssl){
    	if(count >= 2){
    		remote_timeout = 0;
		count = 0;
    	}else
	    count ++;
    }else{
	    local_timeout = 0;
    }
    if(First_Reboot) First_Reboot = 0;
    return 0;
    }

void send_authenticate( char* realm )
{
    char header[10000];
#ifdef SETUP_WIZARD_FT_SUPPORT
	http_auth_success = 0;
#endif
    (void) snprintf(
	//zhangbin 2004.11.27		    
	//header, sizeof(header), "WWW-Authenticate: Basic realm=\"%s\"", realm );
	header, sizeof(header), "WWW-Authenticate: Basic");
    send_error( 401, "Unauthorized", header, "Authorization required." );
}


static void
send_error( int status, char* title, char* extra_header, char* text )
    {

    // jimmy, https, 8/4/2003, fprintf -> wfprintf, fflush -> wfflush
    send_headers( status, title, extra_header, "text/html" );
    (void) wfprintf( conn_fp, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title, status, title );
    (void) wfprintf( conn_fp, "%s\n", text );
    (void) wfprintf( conn_fp, "</BODY></HTML>\n" );
    (void) wfflush( conn_fp );
    } 

static void
send_headers( int status, char* title, char* extra_header, char* mime_type )
    {
    time_t now;
    char timebuf[100];

    // jimmy, https, 8/4/2003, fprintf -> wfprintf
    wfprintf( conn_fp, "%s %d %s\r\n", PROTOCOL, status, title );
    wfprintf( conn_fp, "Server: %s\r\n", SERVER_NAME );
    now = time( (time_t*) 0 );
    strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    wfprintf( conn_fp, "Date: %s\r\n", timebuf );
    if ( extra_header != (char*) 0 )
	wfprintf( conn_fp, "%s\r\n", extra_header );
    if ( mime_type != (char*) 0 )
	wfprintf( conn_fp, "Content-Type: %s\r\n", mime_type );
    wfprintf( conn_fp, "Connection: close\r\n" );
    wfprintf( conn_fp, "\r\n" );
    }

/* Base-64 decoding.  This represents binary data as printable ASCII
** characters.  Three 8-bit binary bytes are turned into four 6-bit
** values, like so:
**
**   [11111111]  [22222222]  [33333333]
**
**   [111111] [112222] [222233] [333333]
**
** Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
*/

static int b64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };

/* Do base-64 decoding on a string.  Ignore any non-base64 bytes.
** Return the actual number of bytes generated.  The decoded size will
** be at most 3/4 the size of the encoded, and may be smaller if there
** are padding characters (blanks, newlines).
*/
int b64_decode( const char* str, unsigned char* space, int size )
    {
    const char* cp;
    int space_idx, phase;
    int d, prev_d=0;
    unsigned char c;

    space_idx = 0;
    phase = 0;
    for ( cp = str; *cp != '\0'; ++cp )
	{
	d = b64_decode_table[(int)*cp];
	if ( d != -1 )
	    {
	    switch ( phase )
		{
		case 0:
		++phase;
		break;
		case 1:
		c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 2:
		c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 3:
		c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
		if ( space_idx < size )
		    space[space_idx++] = c;
		phase = 0;
		break;
		}
	    prev_d = d;
	    }
	}
    return space_idx;
    }


/* Simple shell-style filename matcher.  Only does ? * and **, and multiple
** patterns separated by |.  Returns 1 or 0.
*/
int
match( const char* pattern, const char* string )
    {
    const char* or;

    for (;;)
	{
	or = strchr( pattern, '|' );
	if ( or == (char*) 0 )
	    return match_one( pattern, strlen( pattern ), string );
	if ( match_one( pattern, or - pattern, string ) )
	    return 1;
	pattern = or + 1;
	}
    }


static int
match_one( const char* pattern, int patternlen, const char* string )
    {
    const char* p;

    for ( p = pattern; p - pattern < patternlen; ++p, ++string )
	{
	if ( *p == '?' && *string != '\0' )
	    continue;
	if ( *p == '*' )
	    {
	    int i, pl;
	    ++p;
	    if ( *p == '*' )
		{
		/* Double-wildcard matches anything. */
		++p;
		i = strlen( string );
		}
	    else
		/* Single-wildcard matches anything but slash. */
		i = strcspn( string, "/" );
	    pl = patternlen - ( p - pattern );
	    for ( ; i >= 0; --i )
		if ( match_one( p, pl, &(string[i]) ) )
		    return 1;
	    return 0;
	    }
	if ( *p != *string )
	    return 0;
	}
    if ( *string == '\0' )
	return 1;
    return 0;
    }


int
//do_file(char *path, FILE *stream)
do_file(char *path, webs_t stream) //jimmy, https, 8/4/2003
{
	FILE *fp;
	int c;

	if (!(fp = fopen(path, "r")))
		return -1;
	while ((c = getc(fp)) != EOF)
		//fputc(c, stream);
		wfputc(c, stream); // jimmy, https, 8/4/2003
	fclose(fp);
	return 0;
}

static void
handle_request(void)
{
    char line[10000], *cur;
    char *method, *path, *protocol, *authorization, *boundary;
    char *cp;
    char *file;
    int len;
    struct mime_handler *handler;
    int cl = 0, flags;
    unsigned remote_flag = 0; /*cjg: 0:normal, 1: normal remote, 2: upgrade*/
/****************** xiaoqin add start 2005.04.05 ******************/
#ifdef NOMAD_SUPPORT
	int nrwReturnCode = 1;/* RoadWarrior Return Codes */
	int nUrlType = 0;     /* Connection type: 1:start, 2:stop, 3:change passwd */
#endif
/*********************** xiaoqin add end **************************/
    /* Initialize the request variables. */
    struct in_addr remote_in;
    authorization = boundary = NULL;
    bzero( line, sizeof line );
    
    if(!do_ssl){
	if (waitfor(fileno(conn_fp), TIMEOUT) <= 0){
		fprintf(stderr, "waitfor timeout %d secs\n", TIMEOUT);
		return;
	}
    }
#if 0
    //junzhao 2005.5.13 set nonblock mode
    
    if ((flags = fcntl(fileno(conn_fp), F_GETFL)) < 0) 
    {
	perror("fcntl");
	return;
    }
    if(fcntl(fileno(conn_fp), F_SETFL, flags|O_NONBLOCK) < 0)
    {
	perror("fcntl");
	return;
    }
#endif
/*cjg:*/
    if(do_ssl){
 /*   	if(current_ip == 0x00000000){
    		current_ip = accessing_ip;
    	}else if(current_ip != accessing_ip){
	    send_error(400, "Only one session is accepted!\n", (char *)0, "Please try again after 20 minutes!");
	    return;
    	}
*/
	       	if(current_ip == 0x00000000){
    		current_ip = accessing_ip;
    	}
		remote_in.s_addr = current_ip;

	if(nvram_match("log_enable","1")){
		remote_in.s_addr = current_ip;
		openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
		syslog(LOG_CRIT,"1nRemote access from %s\n", inet_ntoa(remote_in));
		closelog();
	}
    }	
    /* Parse the first line of the request. */
    if ( wfgets( line, sizeof(line), conn_fp ) == (char *)0 ) {	//jimmy,https,8/4/2003
	    send_error( 400, "Bad Request", (char*) 0, "No request found." );
	    return;
    }
    /* To prevent http receive https packets, cause http crash (by honor 2003/09/02) */	
    if ( strncasecmp(line, "GET", 3) && strncasecmp(line, "POST", 4)) {
	fprintf(stderr, "Bad Request!\n");
	return;
    }

    method = path = line;
    strsep(&path, " ");
    while (*path == ' ') path++;
    protocol = path;
    strsep(&protocol, " ");
    while (*protocol == ' ') protocol++;
    cp = protocol;
    strsep(&cp, " ");
    
    if ( !method || !path || !protocol ) {
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    cur = protocol + strlen(protocol) + 1;
    
    /* Parse the rest of the request headers. */
    //while ( fgets( cur, line + sizeof(line) - cur, conn_fp ) != (char*) 0 )
    while ( wfgets( cur, line + sizeof(line) - cur, conn_fp ) != 0 ) //jimmy,https,8/4/2003
    {
		
	if ( strcmp( cur, "\n" ) == 0 || strcmp( cur, "\r\n" ) == 0 ){
	    break;
	}
	else if ( strncasecmp( cur, "Authorization:", 14 ) == 0 )
	    {
	    cp = &cur[14];
	    cp += strspn( cp, " \t" );
	    authorization = cp;
	    cur = cp + strlen(cp) + 1;
	    }
	else if (strncasecmp( cur, "Content-Length:", 15 ) == 0) {
		cp = &cur[15];
		cp += strspn( cp, " \t" );
		cl = strtoul( cp, NULL, 0 );
	}
	else if ((cp = strstr( cur, "boundary=" ))) {
            boundary = &cp[9];
	    for( cp = cp + 9; *cp && *cp != '\r' && *cp != '\n'; cp++ );
	    *cp = '\0';
	    cur = ++cp;
	}

	}
    
    //junzhao 2004.5.13 restore origin flags
#if 0
    if(fcntl(fileno(conn_fp), F_SETFL, flags) < 0)
    {
	perror("fcntl");
	return;
    }
#endif
    if(authorization !=  NULL){
	if(do_ssl){
		remote_in_process = 1;
		alarm(REMOTE_TIMEOUT);
	}else{
		local_in_process = 1;
		alarm(LOCAL_TIMEOUT);
	}
    } else{
		if(do_ssl)	
			remote_timeout = 0;
		else
			local_timeout = 0;

    }   
    if ( strcasecmp( method, "get" ) != 0 && strcasecmp(method, "post") != 0 ) {
	send_error( 501, "Not Implemented", (char*) 0, "That method is not implemented." );
	return;
    }
    if ( path[0] != '/' ) {
	send_error( 400, "Bad Request", (char*) 0, "Bad filename." );
	return;
    }
    file = &(path[1]);
    len = strlen( file );
    if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 ) {
	send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
	return;
    }

/****************** xiaoqin add start 2005.04.05 ******************/
#ifdef NOMAD_SUPPORT
	/* get the remote client IP */
	strcpy(nomadinfo.packet_ip, re_ip_local);
	/* parse the connection message */
	file = nomad_info_parse(path, &nrwReturnCode, &len, &nUrlType);
#endif
/*********************** xiaoqin add end **************************/
	
    if ( file[0] == '\0' || file[len-1] == '/' )
	file = "index.asp";
//wwzh add for WAN UPGRADE at 2004-06-16
#ifdef WAN_UPGRADE
{
	int w_flag = 0;
	w_flag = atoi(nvram_get("new_version"));
	if (w_flag == 1)
	{
		nvram_set("upgrade_wan_test", "1");
		nvram_set("no_wan_upgrade_version", "0");
		
		file = "Upgrade.asp";
		
	}
}
#endif

    for (handler = &mime_handlers[0]; handler->pattern; handler++) {
	    if (match(handler->pattern, file)) {
/********************* xiaoqin modify start 2005.04.05 **********************/
			if (handler->auth) {
			#ifndef MULTIUSER_SUPPORT //modified by zls 2004/11/22
		    	handler->auth(auth_userid, auth_passwd, auth_realm);
			#endif

			#ifdef NOMAD_SUPPORT
				if(0 == strcmp(file, "nomad.lxb"))//match nomad info
				{
					/* authorization check */
					if(1 == nrwReturnCode)
						nrwReturnCode = nomad_auth_check(auth_realm, authorization);
				}
				else//dismatch
				{
					if (!auth_check(auth_realm, authorization,handler))
						return;
				}
			#else
				if (!auth_check(auth_realm, authorization,handler))
					return;
			#endif
			}
/******************************* modify end *********************************/
		#ifdef SETUP_WIZARD_FT_SUPPORT
			http_auth_success++;
			if (http_auth_success == 1)
			{
				char vcc_config[1023];
				char pppoa_config[1023];
				char *ptr;
				int v_ii = 0;
				int p_ii = 0;
				char v_config[127];
				char p_config[127];
				char p_password[123];
				char p_username[123];
				char tmp[12];
				char *p_next;
				char *v_next;
				int v_flag = 0;
				
				strcpy(vcc_config, nvram_safe_get("vcc_config"));
				printf("vcc_config == %s\n", vcc_config);
				foreach(v_config, vcc_config, v_next)
				{
					printf("v_config = %s\n", v_config);
					ptr = strstr(v_config, "pppoa");
					if (ptr != NULL)
					{
						strcpy(pppoa_config, nvram_safe_get("pppoa_config"));
						printf("pppoa_config = %s\n", pppoa_config);
						foreach(p_config, pppoa_config, p_next)
						{
							if (p_ii == v_ii)
							{
								v_flag = 1;
								break;
							}
							p_ii++;
						}
						if (v_flag == 1)
						{
							printf("p_config = %s\n", p_config);
							string_sep(p_config, 1, p_username);
							string_sep(p_config, 2, p_password);
							printf("username %s: password %s\n", p_username, p_password);
							if ((!strcmp(p_username, "")) || (!strcmp(p_password, "")))
							{
								//zhangbin 2005.4.11
								if(!strcmp(file,"index.asp"))

									file = "setupwizard.asp";
							}
						}
						break;
					}
					v_ii++;
				}
			}
			if (http_auth_success > 1024)
				http_auth_success = 2;
		#endif
	//wwzh add for WAN UPGRADE NOTIFY at 2004-06-16
		#ifdef WAN_UPGRADE
		{
		    int ii;
		    ii = atoi(nvram_get("new_version"));
		    if (ii == 1)
		    {
		    	nvram_set("new_version", "0"); 
		    	nvram_commit();
		    }
		}
		#endif

		    post=0;
		    if (strcasecmp(method, "post") == 0 ) {
			post=1;
			if(do_ssl)
				remote_flag = 1;
			else
				remote_flag = 0;
                    }			    
		    if (handler->input) {
			    handler->input(file, conn_fp, cl, boundary);
#if defined(linux)
			    if ((flags = fcntl(fileno(conn_fp), F_GETFL)) != -1 &&
				fcntl(fileno(conn_fp), F_SETFL, flags | O_NONBLOCK) != -1) {
				    /* Read up to two more characters */
				    if (fgetc(conn_fp) != EOF)
					    (void) fgetc(conn_fp);
				    fcntl(fileno(conn_fp), F_SETFL, flags);
			    }
#endif				
		    }
		    send_headers( 200, "Ok", handler->extra_header, handler->mime_type );
/************************** xiaoqin add start 2005.04.05 *********************/
		#ifdef NOMAD_SUPPORT
			if(1 != nrwReturnCode)//has got a Return Code
			{
				int ret = 0;
				if(NOMAD_SUCCESS == nrwReturnCode)
					nrwReturnCode = valid_remote_ip();//check valid IP
				nomadinfo.nRtCode = nrwReturnCode;
				/* call the Communication function */
				ret = handler->output(file, conn_fp, nUrlType);
				break;
			}
		#endif
/******************************** add end ************************************/
		    if (handler->output){
			    int ret = 0;
			    if((ret = handler->output(file, conn_fp, remote_flag)) == -1)
				    send_error( 404, "Not Found", (char*) 0, "File not found." );
		    }
		    break;
	    }
    }

    if (!handler->pattern) 
	    send_error( 404, "Not Found", (char*) 0, "File not found." );
}

//cjg
void get_client_ip(int conn_fp)
{
/*	struct sockaddr_in sa;
	int len = sizeof(struct sockaddr_in);
	if(getpeername(conn_fp, (struct sockaddr *)&sa, &len) == 0){
		accessing_ip = sa.sin_addr.s_addr;
	}
	*/
	struct sockaddr_in sa,sb;
	int len = sizeof(struct sockaddr_in);
    	struct in_addr ip;
    	struct in_addr ip1;
	
	if(getsockname(conn_fp, (struct sockaddr *)&sb, &len) == 0){
		ip.s_addr = sb.sin_addr.s_addr;
		remote_pvc_ip = sb.sin_addr.s_addr;
		strcpy(re_ip_des , inet_ntoa(ip));
	//	printf("re_destination=%s,%x\n",re_ip_des,(remote_pvc_ip));
	}
	if(getpeername(conn_fp, (struct sockaddr *)&sa, &len) == 0){
		accessing_ip = sa.sin_addr.s_addr;
		ip1.s_addr = sa.sin_addr.s_addr;
		strcpy(re_ip_local , inet_ntoa(ip1));
	//	printf("re1_local=%s\n",re_ip_local);
	}

}

void    // add by honor 2003-04-16
get_client_ip_mac(int conn_fp)
{
        struct sockaddr_in sa;
        int len = sizeof(struct sockaddr_in);
        char *m;

        getpeername(conn_fp, (struct sockaddr *)&sa, &len);
	if(strcmp(nvram_safe_get("http_client_ip"), inet_ntoa(sa.sin_addr)))
        	nvram_set("http_client_ip", inet_ntoa(sa.sin_addr));
	//fprintf(stderr, "Client ip=[%s]\n", inet_ntoa(sa.sin_addr));
	m = get_mac_from_ip(inet_ntoa(sa.sin_addr));
	if(m){
		if(strcmp(nvram_safe_get("http_client_mac"), m))
			nvram_set("http_client_mac", m);
	}
}

static void local_remote_timeout_process()
{
	if(do_ssl){
		if(!remote_in_process){
			remote_timeout = 1;
			accessing_ip = 0x00000000;
		}
	}else if(!local_in_process){
		local_timeout = 1;
	}
}

int main(int argc, char **argv)
{
	usockaddr usa;
	int listen_fd;
	int conn_fd;
	socklen_t sz = sizeof(usa);
	int c;
	int r;

#ifdef HTTPS_SUPPORT
	BIO *sbio;
	SSL_CTX *ctx = NULL;
	BIO *ssl_bio;
#endif
	strcpy(pid_file, "/var/run/httpd.pid");
	server_port = DEFAULT_HTTP_PORT;

	while ((c = getopt(argc, argv, "Sp:h")) != -1)
                switch (c) {
                        case 'S':
				do_ssl = 1;
				remote_enable = 1;
				server_port = DEFAULT_HTTPS_PORT;
				strcpy(pid_file, "/var/run/httpsd.pid");
				break;
			case 'p':
				server_port = atoi(optarg);
				break;
			case 'h':
				fprintf(stderr, "Usage: %s [-S] [-p port]\n"
						"	-S : Support https (port 443)\n"
						"	-p port : which port to listen ?\n"
						, argv[0]);
				exit(0);
				break;
			default:
				break;
		}
	
	fprintf(stderr, "HTTPD start, %s port %d\n", do_ssl ? "ssl support":"", server_port);
        /* Ignore broken pipes */
        signal(SIGPIPE, SIG_IGN);	

	//junzhao 2004.4.14
	signal(SIGUSR1, signal_wakeup);
	
//wwzh add for WAN UPGRADE NOTIFY USER at 2004-06-16
#ifdef WAN_UPGRADE
	signal(SIGUSR2, wan_upgrade_notify);
#endif


//cjg:2004-6-12
#if 0	
        /* Build our SSL context*/
	if(do_ssl){
		ctx=initialize_ctx(KEYFILE,PASSWORD);
		load_dh_params(ctx,DHFILE);
	}
#else
#define CERT_FILE "/etc/cert.pem"
#define KEY_FILE "/etc/key.pem"
	if(do_ssl){
	#ifdef HTTPS_SUPPORT
		SSLeay_add_ssl_algorithms();
		SSL_load_error_strings();
		ctx = SSL_CTX_new(SSLv23_server_method());
		if(SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) == 0){
		cprintf("Cann't read %s\n", CERT_FILE);
		ERR_print_errors_fp(stderr);
		exit(1);
		}
		if(SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) == 0){
		cprintf("Cann't read %s\n", KEY_FILE);
		ERR_print_errors_fp(stderr);
		exit(1);
		}
		if(SSL_CTX_check_private_key(ctx) == 0){
			cprintf("Check private key fail\n");
			ERR_print_errors_fp(stderr);
			exit(1);
		}
	#endif
	}
#endif	

	/* Initialize listen socket */
	if ((listen_fd = initialize_listen_socket(&usa)) < 0) {
		fprintf(stderr, "can't bind to any address\n" );
		exit(errno);
	}

#if !defined(DEBUG)
	{
	FILE *pid_fp;
	/* Daemonize and log PID */
	if (daemon(1, 1) == -1) {
		perror("daemon");
		exit(errno);
	}
	if (!(pid_fp = fopen(pid_file, "w"))) {
		perror(pid_file);
		return errno;
	}
	fprintf(pid_fp, "%d", getpid());
	fclose(pid_fp);
	}
#endif
//cjg
	signal(SIGALRM, local_remote_timeout_process);
	if(do_ssl)	
		remote_timeout = 1;
	else
		local_timeout = 1;

/************************** xiaoqin add start 2005.04.05 *********************/
#ifdef NOMAD_SUPPORT
	/* initialize some data and create directory */
	memset(&nomadinfo, 0x0, sizeof(struct mymsgbuf));
	memset(g_chremoteip, 0x0, sizeof(g_chremoteip));
	init_quickvpn_mkdir();
#endif
/***************************** xiaoqin add end *******************************/

	/* Loop forever handling requests */
	for (;;) {
again:
		if(do_ssl){
			remote_in_process = 0;
		}
		else
			local_in_process = 0;
		if ((conn_fd = accept(listen_fd, &usa.sa, &sz)) < 0) {
			perror("accept");
			if(errno == EINTR)
				goto again;

			//junzhao 2005.5.16 for ':' setting
			final_free_buf();

			return errno;
		}

		if(check_action() == ACT_TFTP_UPGRADE ||		// We don't want user to use web during tftp upgrade.
		   check_action() == ACT_SW_RESTORE ||
		   check_action() == ACT_HW_RESTORE
		   ){
			fprintf(stderr, "http(s)d: nothing to do...\n");
			//zhangbnn
			continue;
			//return -1;
		}
		
	    if(do_ssl){
	    #ifdef HTTPS_SUPPORT
		if(check_action() == ACT_WEB_UPGRADE){	// We don't want user to use web (https) during web (http) upgrade.
			fprintf(stderr, "httpsd: nothing to do...\n");
			return -1;
		}

		remote_in_process = 0;
                sbio=BIO_new_socket(conn_fd,BIO_NOCLOSE);
                ssl=SSL_new(ctx);
                SSL_set_bio(ssl,sbio,sbio);
again1:
                if((r=SSL_accept(ssl)<=0)){
	                        //berr_exit("SSL accept error");
	                        fprintf(stderr, "SSL accept error\n");
				if(errno == EINTR)
					goto again1;
				close(conn_fd);
				continue;
		}

                conn_fp=(webs_t)BIO_new(BIO_f_buffer());
                ssl_bio=BIO_new(BIO_f_ssl());
                BIO_set_ssl(ssl_bio,ssl,BIO_CLOSE);
                BIO_push((BIO *)conn_fp,ssl_bio);
	    #endif
	    }else
	    {
		if(check_action() == ACT_WEBS_UPGRADE){	// We don't want user to use web (http) during web (https) upgrade.
			fprintf(stderr, "httpd: nothing to do...\n");
			
			//junzhao 2005.5.16 for ':' setting
			final_free_buf();

			return -1;
		}
	    
		//junzhao 2004.5.10 NONBLOCK MODE
		/*
		{
			long flag = 0;
			if(flag = fcntl(conn_fd, F_GETFL) < 0)
			{
				perror("fcntl");
				return errno;
			}
			flag |= O_NONBLOCK;
			if(fcntl(conn_fd, F_SETFL, flag) < 0)
			{
				perror("fcntl");
				return errno;
			}
		}
		*/
		if (!(conn_fp = fdopen(conn_fd, "r+"))) {
			perror("fdopen");
			
			//junzhao 2005.5.16 for ':' setting
			final_free_buf();

			return errno;
		}				
		
	    }
		get_client_ip(conn_fd);
		handle_request();
		//fflush(conn_fp);
		//fclose(conn_fp);
		wfflush(conn_fp); // jimmy, https, 8/4/2003
		wfclose(conn_fp); // jimmy, https, 8/4/2003
		close(conn_fd);
	}

	shutdown(listen_fd, 2);
	close(listen_fd);

	//junzhao 2005.5.16 for ':' setting
	final_free_buf();

	return 0;
}

char *
wfgets(char *buf, int len, FILE *fp)
{
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT
		return  (char *)BIO_gets((BIO *)fp, buf, len);
	#endif
	}
	else
	{
		return  fgets(buf, len, fp);
	}
}

int
wfputc(char c, FILE *fp)
{
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT	
		return BIO_printf((BIO *)fp, "%c", c);
	#endif
	}
	else
	{
		return fputc(c, fp);
	}
}

int
wfputs(char *buf, FILE *fp)
{
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT	
		return BIO_puts((BIO *)fp, buf);
	#endif
	}
	else
	{
		return fputs(buf, fp);
	}
}

int
wfprintf(FILE *fp, char *fmt,...)
{
	va_list args;
	char buf[1024];
	int ret;
	
	va_start(args, fmt);	
	vsnprintf(buf, sizeof(buf), fmt, args);
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT	
		ret = BIO_printf((BIO *)fp, "%s", buf);
	#endif
	}
	else
	{
		ret = fprintf(fp, "%s", buf);	
	}
	va_end(args);
	return ret;
}

size_t
wfwrite(char *buf, int size, int n, FILE *fp)
{
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT	
		return BIO_write((BIO *)fp, buf , n * size);
	#endif
	}
	else
	{
		return fwrite(buf,size,n,fp);
	}
}

size_t
wfread(char *buf, int size, int n, FILE *fp)
{
	if(do_ssl)
	{
	#ifdef HTTPS_SUPPORT	
		return BIO_read((BIO *)fp, buf , n * size);
	#endif
	}
	else
	{
		return fread(buf,size,n,fp);
	}
}

int
wfflush(FILE *fp)
{
	if(do_ssl){
	#ifdef HTTPS_SUPPORT	
		BIO_flush((BIO *)fp);
	#endif
		return 1;
	}
	else
		return fflush(fp);
}

int
wfclose(FILE *fp)
{
	if(do_ssl){
	#ifdef HTTPS_SUPPORT	
		BIO_free_all((BIO *)fp);
	#endif
		return 1;
	}
	else
		return fclose(fp);
}

