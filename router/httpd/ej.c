/*
 * Tiny Embedded JavaScript parser
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ej.c,v 1.1.1.2 2005/03/28 06:57:48 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "httpd.h"

ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
static char * get_arg(char *args, char **next);
//static void call(char *func, FILE *stream);
static void call(char *func, webs_t stream);
int findend(char * str);
int process_asp_file(FILE *src, FILE *dst);

/* Look for unquoted character within a string */
static char *
unqstrstr(char *haystack, char *needle)
{
	char *cur;
	int q;

	for (cur = haystack, q = 0;
	     cur < &haystack[strlen(haystack)] && !(!q && !strncmp(needle, cur, strlen(needle)));
	     cur++) {
		if (*cur == '"')
			q ? q-- : q++;
	}
	return (cur < &haystack[strlen(haystack)]) ? cur : NULL;
}

static char *
get_arg(char *args, char **next)
{
	char *arg, *end;

	/* Parse out arg, ... */
	if (!(end = unqstrstr(args, ","))) {
		end = args + strlen(args);
		*next = NULL;
	} else
		*next = end + 1;

	/* Skip whitespace and quotation marks on either end of arg */
	for (arg = args; isspace((int)*arg) || *arg == '"'; arg++);
	for (*end-- = '\0'; isspace((int)*end) || *end == '"'; end--)
		*end = '\0';

	return arg;
}

static void
//call(char *func, FILE *stream)
call(char *func, webs_t stream) //jimmy, https, 8/4/2003
{
	char *args, *end, *next;
	int argc;
	char * argv[16];
	struct ej_handler *handler;

	//printf("%s\n",func);

	while(*func==' ')*func++;

	/* Parse out ( args ) */
	if (!(args = strchr(func, '(')))
		return;
	if (!(end = unqstrstr(func, ")")))
		return;
	*args++ = *end = '\0';

	/* Set up argv list */
	for (argc = 0; argc < 16 && args; argc++, args = next) {
		if (!(argv[argc] = get_arg(args, &next)))
			break;
	}

	/* Call handler */
	for (handler = &ej_handlers[0]; handler->pattern; handler++) {
		//if (strncmp(handler->pattern, func, strlen(handler->pattern)) == 0)
		if (strcmp(handler->pattern, func) == 0)
			handler->output(0, stream, argc, argv);
	}
}

int
//do_ej(char *path, FILE *stream)
do_ej(char *path, webs_t stream)	// jimmy, https, 8/4/2003
{
	FILE *fp;
	FILE *tmp = tmpfile();
	char buf[4096];
	int len=0;

	if (!(fp = fopen(path, "r")))
		return -1;


	if(tmp == NULL || do_ssl==1){
		process_asp_file(fp,stream);
		if(tmp) fclose(tmp);
	}else{
		process_asp_file(fp,tmp);
		fflush(tmp);
		rewind(tmp);
		while(!feof(tmp)){
			len = fread(buf,sizeof(char),sizeof(buf),tmp);
			wfwrite(buf,len,1,stream);
		}
		fclose(tmp);
	}
	fclose(fp);

		

	//wwzh add for WAN UPGRADE NOTIFY at 2004-06-16
#ifdef WAN_UPGRADE
	{
		int ii;
		ii = atoi(nvram_get("upgrade_wan_test"));
		if (ii == 1)
		{
			nvram_set("upgrade_wan_test", "0");
			nvram_commit();
		}
	}
#endif
	return 0;
}
int
findend(char *str)
{
	char *end;
	int n = strlen(str);

	end = str + strlen(str) + 1;
	while(end--){
		if(*end == ';')
			return n;
		n--;
	}
	return n;
}


int process_asp_file(FILE *src, FILE *dst){

	char *line = NULL;
	size_t len_t = 0;
	ssize_t read;
	int tempc;
	int n=0;

	while((read = getdelim(&line,&len_t,'<',src)!=-1)){
		tempc = getc(src);
		if(tempc == '%'){
			line[strlen(line)-1]='\0';
			wfputs(line,dst);
			read = getdelim(&line,&len_t,'%',src);
			if(read ==-1) return -1;

			n = findend(line);
			line[n]='\0';
			call(line,(webs_t)dst);

			read = getdelim(&line,&len_t,'>',src);

		}else{
			ungetc(tempc,src);
			wfputs(line,dst);
		}
	}
	if(line)
		free(line);
	return 0;
}


int
ejArgs(int argc, char **argv, char *fmt, ...)
{
	va_list	ap;
	int arg;
	char *c;

	if (!argv)
		return 0;

	va_start(ap, fmt);
	for (arg = 0, c = fmt; c && *c && arg < argc;) {
		if (*c++ != '%')
			continue;
		switch (*c) {
		case 'd':
			*(va_arg(ap, int *)) = atoi(argv[arg]);
			break;
		case 's':
			*(va_arg(ap, char **)) = argv[arg];
			break;
		}
		arg++;
	}
	va_end(ap);

	return arg;
}
