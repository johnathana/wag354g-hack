/*
 * To get environment values where it stores on FLASH. This APIs are used 
 * to incorperate with TI's NSP.
 *
 * Copyright (C) 2004, CyberTAN Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. CYBERTAN
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include "promenv.h"

int prom_getenv(const char *key, char *val)
{
        int fd;
        int count = 0, len = 0;
        unsigned char data[TICFG_BUF];
	const char *ptr, *end;

	if ((fd = open(TICFG_ENV, O_RDONLY)) < 0) {
                printf("Can't open the %s file!\n", TICFG_ENV);
                return -1;
        }

        count = read(fd, data, TICFG_BUF);
        if(count > 0) {
                if ((ptr = strstr(data, key)) != NULL) {
			ptr += strlen(key) + 1;
			end = strchr(ptr, '\n');
			len = end - ptr;
			strncpy((char *)val, (const char *)(ptr), len);
		}
        }
	*(val + len) = '\0';
        close(fd);
        return 1;
}

int prom_setenv(char *key, char *val)
{
	char buf[256];
	int fd;

	if ((fd=open(TICFG_ENV, O_RDWR|O_APPEND)) < 0) {
                printf("Can't open the %s file!\n", TICFG_ENV);
		return -1;
	}

	if (val)
		sprintf(buf, "%s\t%s", key, val);
	else
		sprintf(buf, "%s", key);

	write(fd, buf, strlen(buf) + 1);
	close(fd);
	return 1;
}

int prom_setenv_va(char *env_nm, const char *format, ...) 
{
    va_list  ap;
    char env_val[1024];

    va_start(ap, format);
    vsprintf(env_val, format, ap);

    return prom_setenv(env_nm, env_val);
}
