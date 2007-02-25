/*
 * NVRAM variable manipulation (Linux user mode half)
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: nvram_linux.c,v 1.1.1.2 2005/03/28 06:58:10 sparq Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <shutils.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <nvram_convert.h>

#define PATH_DEV_NVRAM "/dev/nvram"

/* Globals */
static int nvram_fd = -1;
static char *nvram_buf = NULL;

/************************************/
//wwzh
#define MAX_LEN 8192
static char nvram_value[MAX_LEN];

/***********************************/

static char lock_list[MAX_LEN];

int lock_list_init(void)
{
	int ret = -1;
	
	ret = strncpy(lock_list, nvram_safe_get("lock_list"), sizeof(lock_list));

	return ret;
}

int
nvram_init(void *unused)
{
	if ((nvram_fd = open(PATH_DEV_NVRAM, O_RDWR)) < 0)
		goto err;

//wwzh
#if 0
	/* Map kernel string buffer into user space */
	if ((nvram_buf = mmap(NULL, NVRAM_SPACE, PROT_READ, MAP_SHARED, nvram_fd, 0)) == MAP_FAILED) {
		close(nvram_fd);
		nvram_fd = -1;
		goto err;
	}
#endif

	lock_list_init();

	return 0;

 err:
	perror(PATH_DEV_NVRAM);
	return errno;
}

char *
nvram_get(const char *name)
{
	size_t count = strlen(name) + 1;
	//char tmp[100], *value;
	//unsigned long *off = (unsigned long *) tmp;

	char *off;
	memset((void *)nvram_value, 0, MAX_LEN);
	off = (char *)nvram_value;

	if (nvram_fd < 0)
		if (nvram_init(NULL))
			return NULL;
//wwzh
#if 0
	if (count > sizeof(tmp)) {
		if (!(off = malloc(count)))
			return NULL;
	}
#endif

	/* Get offset into mmap() space */
	strcpy((char *) off, name);
	
	count = read(nvram_fd, off, count);
	
	return off;
//wwzh
#if 0
	if (count == sizeof(unsigned long))
		value = &nvram_buf[*off];
	else
		value = NULL;

	if (count < 0)
		perror(PATH_DEV_NVRAM);

	if (off != (unsigned long *) tmp)
		free(off);

	return value;
#endif
}

int
nvram_getall(char *buf, int count)
{
	int ret;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	if (count == 0)
		return 0;

	/* Get all variables */
	*buf = '\0';

	ret = read(nvram_fd, buf, count);

	if (ret < 0)
		perror(PATH_DEV_NVRAM);

	return (ret == count) ? 0 : ret;
}

static int
_nvram_set(const char *name, const char *value)
{
	size_t count = strlen(name) + 1;
	char tmp[100], *buf = tmp;
	int ret;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	/* Unset if value is NULL */
	if (value)
		count += strlen(value) + 1;

	if (count > sizeof(tmp)) {
		if (!(buf = malloc(count)))
			return -ENOMEM;
	}

	if (value)
		sprintf(buf, "%s=%s", name, value);
	else
		strcpy(buf, name);

	ret = write(nvram_fd, buf, count);

	if (ret < 0)
		perror(PATH_DEV_NVRAM);

	if (buf != tmp)
		free(buf);

	return (ret == count) ? 0 : ret;
}


int
nvram_id_set(const char *name, const char *value, int id)
{
	extern struct nvram_convert nvram_converts[];
	struct nvram_convert *v; 	 
        int ret; 	 
        int  i = 0, j; 	 
        char buf[512] = "", var[64] = "", *next; 	 
        int access_list[64]; 
	char *p = ""; 	 

	//strcpy(buf, nvram_safe_get("lock_list"));
	if (!strcmp(lock_list, "lock_list") || !strcmp(lock_list, ""))
		goto set;

	foreach(var, lock_list, next) {
		if (strstr(var, name)) { 	 
                	p = strtok(var, ":"); 	 
                        printf("\"%s\" is locked, access list=", p); 	 
                        while ((p = strtok(NULL, ":"))) { 	 
                        	access_list[i] = atoi(p); 	 
                                printf(" %s", p); 	 
                                i++; 	 
                        } 	 
                        for (j = 0; j < i; j++) { 	 
                        	if (access_list[j] == id) { 	 
                                	printf("\nThis id=%d is allowed to modify!\n", id); 	 
                                        goto set; 	 
                                } 	 
                        } 	 
                        printf("\nThis id=%d is not allowed to modify!\n", id); 	 
                        return -1; 	 
                } 	 
	} 	 
  	 
set: 	 
	ret = _nvram_set(name, value); 	 
  	 
        for(v = nvram_converts ; v->name ; v++) { 	 
        	if(!strcmp(v->name, name)){ 	 
                	if(strcmp(v->wl0_name,""))      _nvram_set(v->wl0_name, value); 	 
                        if(strcmp(v->d11g_name,""))     _nvram_set(v->d11g_name, value); 	 
                } 	 
        } 	 
  	 
	if(!strcmp(name, "lock_list"))
		lock_list_init();

        return ret;	
}

int
nvram_set(const char *name, const char *value)
{
/*
	 extern struct nvram_convert nvram_converts[];
         struct nvram_convert *v;
         int ret;

         ret = _nvram_set(name, value);

         for(v = nvram_converts ; v->name ; v++) {
                 if(!strcmp(v->name, name)){
                         if(strcmp(v->wl0_name,""))      _nvram_set(v->wl0_name, value);
                         if(strcmp(v->d11g_name,""))     _nvram_set(v->d11g_name, value);
                 }
         }

         return ret;
*/
	return (nvram_id_set(name, value, -1));			
}

int
nvram_unset(const char *name)
{
	return _nvram_set(name, NULL);
}

int
nvram_commit(void)
{
	int ret;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	ret = ioctl(nvram_fd, NVRAM_MAGIC, NULL);

	if (ret < 0)
		perror(PATH_DEV_NVRAM);

	return ret;
}

int file2nvram(char *filename, char *varname) {
   FILE *fp;
   int c,count;
   int i=0,j=0;
   char mem[10000],buf[30000];

   if ( !(fp=fopen(filename,"rb") ))
        return 0;

   count=fread(mem,1,sizeof(mem),fp);
   fclose(fp);
   for (j=0;j<count;j++) {
        if  (i > sizeof(buf)-3 )
                break;
        c=mem[j];
        if (c >= 32 && c <= 126 && c != '\\' && c != '~')  {
                buf[i++]=(unsigned char) c;
        } else if (c==0) {
		buf[i++]='~';
        } else {
                buf[i++]='\\';
                sprintf(buf+i,"%02X",c);
                i+=2;
        }
   }
   if (i==0) return 0;
   buf[i]=0;
   //fprintf(stderr,"================ > file2nvram %s = [%s] \n",varname,buf); 
   nvram_set(varname,buf);

}

int nvram2file(char *varname, char *filename) {
   FILE *fp;
   int c,tmp;
   int i=0,j=0;
   char *buf;
   char mem[10000];
   
   if ( !(fp=fopen(filename,"wb") ))
        return 0;
        
   buf=strdup(nvram_safe_get(varname));
   //fprintf(stderr,"=================> nvram2file %s = [%s] \n",varname,buf);
   while (  buf[i] && j < sizeof(mem)-3 ) {
        if (buf[i] == '\\')  {
                i++;
                tmp=buf[i+2];
                buf[i+2]=0;
                sscanf(buf+i,"%02X",&c);
                buf[i+2]=tmp;
                i+=2;
                mem[j]=c;j++;
        } else if (buf[i] == '~') {
		mem[j]=0;j++;
		i++;
        } else {
                mem[j]=buf[i];j++;
                i++;
        }       
   }
   if (j<=0) return j;
   j=fwrite(mem,1,j,fp);
   fclose(fp);
   free(buf);
   return j;
}  

int nvram_set_notify(void (*func)(int))
{
	int ret = 0;
	struct sigaction sa;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	/* Install sigio_handler as the handler for SIGIO. */
	memset (&sa, 0, sizeof (sa));
	//sa.sa_handler = (__sighandler_t) func;
	sa.sa_handler = func;
	sigaction (SIGIO, &sa, NULL);

	/* allow the process to receive SIGIO */
	fcntl(nvram_fd, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only 
	 *            O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(nvram_fd, F_SETFL, FASYNC);

	return ret;
}

