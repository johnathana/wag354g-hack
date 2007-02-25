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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>


#include <broadcom.h>
#include <cyutils.h>
#include <code_pattern.h>

//extern struct code_header *init_code_header(void);
int init_code_header(struct code_header *pattern);//xiaoqin modify 2005.08.04

#if (FLASH_SIZE == 8)
	#define MTDBLOCK3_SPACE 0x20000
#elif (FLASH_SIZE == 4)
	#define MTDBLOCK3_SPACE 0x10000
#endif

#define FLSH_PATTERN "FLSH"

/* xiaoqin modify this function for memoryleak caused by variable "pattern" */
/* at 2005.08.04 */
void do_backup(char *path, webs_t stream)
{
	FILE *fp_r, *fp_w;
	//char buf[NVRAM_SPACE];
	char buf[MTDBLOCK3_SPACE];
	struct code_header *pattern;
	memset(buf, 0, sizeof(buf));

	pattern = malloc(sizeof(struct code_header));
	memset(pattern, 0, sizeof(struct code_header));

	init_code_header(pattern);
	
	/* Write code pattern */
	/* Open NVRAM file */
	if (!(fp_r = fopen("/dev/mtd/3", "r"))) {
		perror("/dev/mtd/3");
		if(pattern){
			free(pattern);
			pattern = NULL;
		}
		return;
	}

	/* Read NVRAM data */
	//fseek(fp_r, -NVRAM_SPACE, SEEK_END);
	fseek(fp_r, -MTDBLOCK3_SPACE, SEEK_END);
	//fread(buf, NVRAM_SPACE, 1, fp_r);
	fread(buf, MTDBLOCK3_SPACE, 1, fp_r);
	fclose(fp_r);
	
	encode(buf, sizeof(buf));
	
	if(!(fp_w = fopen("/tmp/config.bin", "w"))){
		perror("/tmp/config.bin");
		if(pattern){
			free(pattern);
			pattern = NULL;
		}
		return ;
	}

	/* Write code pattern */
	fwrite(pattern, 1, sizeof(struct code_header), fp_w);
	/*Write NVRAM data */
	fwrite(buf, 1, sizeof(buf), fp_w);
	fclose(fp_w);

	do_file("/tmp/config.bin", stream);

	if(pattern){
		free(pattern);
		pattern = NULL;
	}
}

int ej_get_backup_name(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;
	char buf[80];
	
	snprintf(buf, sizeof(buf), "%s_%s.cfg", MODEL_NAME, CYBERTAN_VERSION);

	ret = websWrite(wp, "%s", buf);

	return ret;
}

#define MAX_BUF	1024

/* xiaoqin modify this function for memoryleak caused by variable "pattern" */
/* at 2005.08.04 */
int init_code_header(struct code_header *pattern)
{
	int i;

	struct timeval tv;
	struct timezone tz;
	struct tm tm;
	unsigned char version_buf[64]; 
	unsigned char buf2[64];
	int cnt;

	//memcpy(pattern->magic, CODE_PATTERN, sizeof(pattern->magic));
	memcpy(pattern->magic, FLSH_PATTERN, sizeof(pattern->magic));
	memcpy(pattern->id, CODE_ID, sizeof(pattern->id));

	//fprintf(stderr, "%s is %ld bytes\n", input_file, input_size);

	//Make date string
	gettimeofday(&tv,&tz);
	memcpy(&tm, localtime(&tv.tv_sec), sizeof(struct tm));
	pattern->fwdate[0]=tm.tm_year+1900-2000;
	pattern->fwdate[1]=tm.tm_mon+1;
	pattern->fwdate[2]=tm.tm_mday;
	printf("\nMake date==>Year:%d,Month:%d,Day:%d,Hour:%d,Min:%d,Sec:%d\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);

	//Make version
	memset(version_buf,0,64);
	memset(buf2,0,64);
	memcpy(version_buf,CYBERTAN_VERSION ,strlen(CYBERTAN_VERSION));
	printf("\nFirmware version =>%s\n",version_buf);
	cnt=0;
	i=0;
	while(i<strlen(version_buf))
	{
		if(version_buf[i] == 0x2e)
			cnt++;
		i++;
	}
	strcpy(buf2,strtok(version_buf,".")); 
	pattern->fwvern[0]=(char)atoi(&buf2[1]);
	printf("\n1:%s,%d ",buf2,pattern->fwvern[0]);
	memset(buf2,0,64);
	strcpy(buf2,strtok(NULL,"."));
	pattern->fwvern[1]=(char)atoi(buf2);
	printf("2:%s,%d ",buf2,pattern->fwvern[1]);
	if(cnt==2)
	{
		memset(buf2,0,64);
		strcpy(buf2,strtok(NULL,"."));
		pattern->fwvern[2]=(char)atoi(buf2);
		printf("3:%s,%d\n",buf2,pattern->fwvern[2]);
	}
	else
	{
		pattern->fwvern[2]=0;
		printf("\n");
	}

	return 1;
}
