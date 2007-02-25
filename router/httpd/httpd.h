/*
 * milli_httpd - pretty small HTTP server
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: httpd.h,v 1.1.1.2 2005/03/28 06:57:48 sparq Exp $
 */

#include <cy_conf.h>
#ifndef _httpd_h_
#define _httpd_h_

#if defined(DEBUG) && defined(DMALLOC)
#include <dmalloc.h>
#endif

   #include <common.h>
   #include <server.h>
//   typedef  BIO * webs_t;
//   #define wfgets(buf,len,fp)     BIO_gets(fp,buf,len)
//   #define wfputc(c,fp)           BIO_printf(fp,"%c",c)
//   #define wfputs(buf,fp)         BIO_puts(fp,buf)
//   #define wfprintf(fp, args...)  BIO_printf(fp, ## args)
//   #define wfwrite(buf,size,n,fp) BIO_write(fp,buf,n * size)
//   #define wfread(buf,size,n,fp)  BIO_read(fp,buf,n * size)
//   #define wfflush(fp)            BIO_flush(fp)
//   #define wfclose(fp)            BIO_free_all(fp)
//#else
//   typedef FILE * webs_t;
//   #define wfgets(buf,len,fp)     fgets(buf,len,fp)
//   #define wfputc(c,fp)           fputc(c,fp)
//   #define wfputs(buf,fp)         fputs(buf,fp)
//   #define wfprintf(fp, args...)  fprintf(fp, ## args)
//   #define wfwrite(buf,size,n,fp) fwrite(buf,size,n,fp)
//   #define wfread(buf,size,n,fp)  fread(buf,size,n,fp)
//   #define wfflush(fp)            fflush(fp)
//   #define wfclose(fp)            fclose(fp)
//#endif
#define y_printf(format, args...)
typedef FILE * webs_t;
extern char * wfgets(char *buf, int len, FILE *fp);
extern int wfputc(char c, FILE *fp);
extern int wfputs(char *buf, FILE *fp);
extern int wfprintf(FILE *fp, char *fmt,...);
extern size_t wfwrite(char *buf, int size, int n, FILE *fp);
extern size_t wfread(char *buf, int size, int n, FILE *fp);
extern int wfflush(FILE *fp);
extern int wfclose(FILE *fp);
  
int do_ssl;
/* Basic authorization userid and passwd limit */
#define AUTH_MAX 64
//zls
#define AUTH_SUM 8
/* Generic MIME type handler */
struct mime_handler {
	char *pattern;
	char *mime_type;
	char *extra_header;
	void (*input)(char *path, FILE *stream, int len, char *boundary);
	int  (*output)(char *path, FILE *stream, int flag/*cjg*/);
	void (*auth)(char *userid, char *passwd, char *realm);
};
extern struct mime_handler mime_handlers[];

/* CGI helper functions */
extern void init_cgi(char *query);
extern char * get_cgi(char *name);
extern void set_cgi(char *name, char *value);

/* Regular file handler */
extern int do_file(char *path, webs_t stream);

/* GoAhead 2.1 compatibility */
//typedef FILE * webs_t;
typedef char char_t;
#define T(s) (s)
#define __TMPVAR(x) tmpvar ## x
#define _TMPVAR(x) __TMPVAR(x)
#define TMPVAR _TMPVAR(__LINE__)
#define websWrite(wp, fmt, args...) ({ int TMPVAR = wfprintf(wp, fmt, ## args); wfflush(wp); TMPVAR; })
//#define websDebugWrite(wp, fmt, args...) ({ error_value = 1; wfputs("<!--", wp); int TMPVAR = wfprintf(wp, fmt, ## args); wfputs("-->", wp); wfflush(wp); TMPVAR; })
//junzhao
#define websDebugWrite(wp, fmt, args...) ({ error_value = 1; wfputs("<!--", wp); wfprintf(wp, fmt, ## args); wfputs("-->", wp); wfflush(wp); })
#define websError(wp, code, msg, args...) wfprintf(wp, msg, ## args)
#define websHeader(wp) wfputs("<html lang=\"en\">", wp)
#define websFooter(wp) wfputs("</html>", wp)
#define websDone(wp, code) wfflush(wp)
#define websGetVar(wp, var, default) (get_cgi(var) ? : default)
#define websSetVar(wp, var, value) set_cgi(var, value)
#define websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query) ({ do_ej(path, wp); fflush(wp); 1; })
#define websWriteData(wp, buf, nChars) ({ int TMPVAR = wfwrite(buf, 1, nChars, wp); wfflush(wp); TMPVAR; })
#define websWriteDataNonBlock websWriteData
#define a_assert(a)

extern int ejArgs(int argc, char_t **argv, char_t *fmt, ...);

/* GoAhead 2.1 Embedded JavaScript compatibility */
extern int do_ej(char *path, webs_t stream);
struct ej_handler {
	char *pattern;
	int (*output)(int eid, webs_t wp, int argc, char_t **argv);
};
extern struct ej_handler ej_handlers[];


#endif /* _httpd_h_ */
