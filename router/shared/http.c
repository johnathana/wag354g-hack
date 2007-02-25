/*
 * Generic HTTP routines
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: http.c,v 1.7 2006/09/08 03:41:42 dom Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>//Emily
#include <netinet/in.h>
#include <arpa/inet.h>
#include <shutils.h>

static char *
base64enc(const char *p, char *buf, int len)
{
        char al[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	char *s = buf;

        while(*p) {
		if (s >= buf+len-4)
			break;
                *(s++) = al[(*p >> 2) & 0x3F];
                *(s++) = al[((*p << 4) & 0x30) | ((*(p+1) >> 4) & 0x0F)];
                *s = *(s+1) = '=';
                *(s+2) = 0;
                if (! *(++p)) break;
                *(s++) = al[((*p << 2) & 0x3C) | ((*(p+1) >> 6) & 0x03)];
                if (! *(++p)) break;
                *(s++) = al[*(p++) & 0x3F];
        }

	return buf;
}

enum {
	METHOD_GET,
	METHOD_POST
};

char *gethdr(char *buf, size_t bufsiz, FILE *fp, int *istrunc)
{
	char *s, *hdrval;
	int c;

	*istrunc = 0;

	/* retrieve header line */
	if (fgets( buf, bufsiz, fp) == NULL )
		return NULL;

	/* see if we are at the end of the headers */
	for (s = buf; *s == '\r'; ++s)
		;

	if (s[0] == '\n')
		return NULL;

	/* convert the header name to lower case */
	for (s = buf; isalnum(*s) || *s == '-'; ++s)
		*s = tolower(*s);

	/* verify we are at the end of the header name */
	if (*s != ':')
		return "Err";
	//error_msg_and_die("bad header line: %s", buf);

	/* locate the start of the header value */
	for (*s++ = '\0'; *s == ' ' || *s == '\t'; ++s)
		;
	hdrval = s;

	/* locate the end of header */
	while (*s != '\0' && *s != '\r' && *s != '\n')
		++s;

	/* end of header found */
	if (*s != '\0') {
		*s = '\0';
	return hdrval;
	}

	/* Rats! The buffer isn't big enough to hold the entire header value. */
	while (c = getc(fp), c != EOF && c != '\n')
		;
	*istrunc = 1;
	return hdrval;
}

int CONNOK(void)
{
	FILE *in;
	char pbuf[10];
	char *tok;

	if ((in = fopen("/tmp/conn_log", "rb")) != NULL )
	{
		fgets(pbuf, sizeof(pbuf), in);
		if ( pbuf[10] == '0') return 0;
	}
	return 1;
}

static int
wget(int method, const char *server, char *buf, size_t count, off_t offset)
{
	char url[PATH_MAX] = { 0 }, *s;
	char *host = url, *path = "", auth[128] = { 0 }, line[512];
	unsigned short port = 80;
	int fd, n, i ;
	FILE *fp;
	struct stat sbuf;
	struct sockaddr_in sin;
	int chunked = 1, len = 0;
	FILE *output;               /* socket to web server             */
	int got_clen = 0;           /* got content-length: from server  */
	int do_continue = 1;        /* continue a prev transfer (-c)    */
	long beg_range = 0L;        /*   range at which continue begins */
	char *fname_out, cmd[100];

	if (method == METHOD_GET )
		fname_out = "/tmp/http_voip";
	if (server == NULL || !strcmp(server, "")) {
		dprintf("wget: null server input\n");
		return (0);
	}
	strncpy(url, server, sizeof(url));

	/* Parse URL */
	if (!strncmp(url, "http://", 7)) {
		port = 80;
		host = url + 7;
	}
	if ((s = strchr(host, '/'))) {
		*s++ = '\0';
		path = s;
	}
	if ((s = strchr(host, '@'))) {
		*s++ = '\0';
		base64enc(host, auth, sizeof(auth));
		host = s;
	}
	if ((s = strchr(host, ':'))) {
		*s++ = '\0';
		port = atoi(s);
	}

	//Open the output file stream.
	if ( method == METHOD_GET )
		output = fopen(fname_out, (do_continue ? "a" : "w"));

	//Determine where to start transfer.
	if ( method == METHOD_GET )
	{
 		if (do_continue) {
			if (fstat(fileno(output), &sbuf) < 0)
				return 0 ; //perror_msg_and_die("fstat()");
			if (sbuf.st_size > 0)
				beg_range = sbuf.st_size;
			else
				do_continue = 0;
		}
	}

	/* Open socket */
	memset(&sin,0,sizeof(sin));
	if (!inet_aton(host, &sin.sin_addr)) return 0;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	dprintf("Connecting to %s:%u...\n", host, port);
	i = 0 ;
	while (1)
	{
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "diagnostic -f /tmp/ping.log -s 60 -c 1 -i 1000 -w 10 %s > /tmp/conn_log", host);
		system(cmd);
		i++;
		if (CONNOK())
		{
			system("rm -rf /tmp/conn_log");
			break;
		}
		if ( i == 15 )
		{
			system("rm -rf /tmp/conn_log");
			return 0;
		}
	}
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
	    connect(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0 ||
	    !(fp = fdopen(fd, "r+"))) {
		perror(host);
		if (fd >= 0)
			close(fd);
		return 0;
	}
	dprintf("connected!\n");

	memset(cmd, 0, sizeof(cmd));
	if (method == METHOD_POST)
		strcat(cmd, nvram_get("DHCP_MAPPING_PATH"));
	strcat(cmd, path);

	/* Send HTTP request */
	fprintf(fp, "%s /%s HTTP/1.1\r\n", method == METHOD_POST ? "POST" : "GET", cmd);
        fprintf(fp, "Host: %s\r\n", host);
        fprintf(fp,"User-Agent: Mozilla/5.0 (X11; U; Linux i686; rv:1.7.3) Gecko/20041020 Firefox/0.10.1\r\n");

	//if (strlen(auth))
	//	fprintf(fp, "Authorization: Basic %s\r\n", auth);

	if (offset)
		fprintf(fp, "Range: bytes=%ld-\r\n", offset);
	if (method == METHOD_POST) {
		fprintf(fp, "Content-Type: application/x-www-form-urlencoded\r\n");
		fprintf(fp, "Content-Length: %d\r\n", (int) strlen(buf) + (int) strlen(cmd));
		fprintf(fp, "Connection: keep-alive\r\n");
		fprintf(fp, "Referer: http://192.168.1.1/VOIP.asp\r\n");
		fprintf(fp, "Authorization: Basic YWRtaW46YWRtaW4=\r\n\r\n");
		fputs(buf, fp);
	}
        else
	{
		if (do_continue)
			fprintf(fp, "Range: bytes=%ld-\r\n", beg_range);
		fprintf(fp,"Connection: close\r\n\r\n");
	}

	/* Check HTTP response */
	dprintf("HTTP request sent, awaiting response...\n");
	if (fgets(line, sizeof(line), fp)) {
		//dprintf("%s", line);
		for (s = line; *s && !isspace((int)*s); s++);
		for (; isspace((int)*s); s++);
		switch (atoi(s)) {
		case 200:
			if (method == METHOD_GET) output = freopen(fname_out, "w", output);
			if (offset) goto done; else break;
		case 206:
			if (offset) break; else goto done;
		default:
			goto done;
		}
	}
	if ( method == METHOD_POST ) return atoi(s);

	/* Retrieve HTTP headers. */
	while ((s = gethdr(line, sizeof(line), fp, &n)) != NULL) {
		if (strcasecmp(line, "content-length") == 0){
			len = atol(s);
			got_clen = 1;
			continue;
		}
		if (strcasecmp(line, "transfer-encoding") == 0) {
			if (strcasecmp(s, "chunked") == 0) {
				chunked = got_clen = 1;
			} else
				printf("server wants to do %s transfer encoding", s);
		}
	}
	len = fread(line, 1, sizeof(line), fp);
	fwrite(line, sizeof(line), 1, output);

	do {
		memset(line,0,sizeof(line));
		len = fread(line, 1, sizeof(line), fp);
		fwrite(line, sizeof(line), 1, output);
	} while(len>0);
	
 done:
	/* Close socket */
	fflush(fp);
	fclose(fp);
	return len;
}

int
http_get(const char *server, char *buf, size_t count, off_t offset)
{
	return wget(METHOD_GET, server, buf, count, offset);
}

int
http_post(const char *server, char *buf, size_t count)
{
	/* No continuation generally possible with POST */
	return wget(METHOD_POST, server, buf, count, 0);
}
