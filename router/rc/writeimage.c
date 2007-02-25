
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include <linux/mtd/mtd.h>

#include <sys/sysinfo.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <mtd.h>

#include <cyutils.h>
#include <cymac.h>
#include <utils.h>
#include <cy_conf.h>

#define WRITE_IMAGE "/tmp/write_image"

#define PMON_SIZE 256 * 1024
#define ROUNDUP(x, y) ((((unsigned int)(x)+((y)-1))/(y))*(y))
#define OFFSET(p, field) ((unsigned int)(&((p)->field)) - (unsigned int)(p))
#define MIN(a, b) \
({ typedef _ta = (a), _tb = (b); \
   _ta _a = (a); _tb _b = (b); \
   _a < _b ? _a : _b; \
})

extern int mtd_erase_sector(const char *mtd, unsigned long start, unsigned long end);


/* Write pmon from user space, and erease first 128K of kernel
 * Must check EST header.
 * Restore lan mac to new boot.
 * The boot filename must be boot.bin
 */

int
write_boot(const char *path, const char *mtd)
{
	int mtd_fd = -1;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	struct sysinfo info;

	struct boot_header boot;
	
	FILE *fp;
	char *buf = NULL;
	long count=0, len, off, ret_count;
	int ret = -1;
	char *lan_mac;
	int i;
	int MAC_START_ADDRESS;

	printf("Write_boot(%s,%s) doing....\n",path,mtd);

	if(check_now_boot() == PMON_BOOT){
		printf("Now boot is PMON\n");
		MAC_START_ADDRESS = PMON_MAC_START_ADDRESS;
	}
	else{
		printf("Now boot is CFE\n");
		MAC_START_ADDRESS = CFE_MAC_START_ADDRESS;
	}
	{//junzhao
	FILE *fp_w;
	fp_w = fopen(WRITE_IMAGE, "w");	
	fclose(fp_w);
	}//endjunzhao
	memset(&boot,0,sizeof(struct boot_header));

	/* Examine EST header */
	if ((fp = fopen(path, "r")))
		count = fread(&boot, 1, sizeof(struct boot_header), fp);
	if (count < 0) {
		perror(path);
		ret++ ;
		goto fail;
	}
	printf("Read header size = [%ld]\n",(unsigned long)count);

	if(memcmp(&boot.magic[0], &BOOT_PATTERN, 3)){
		        printf("Bad boot header!\n");
			ret++;
			goto fail;
	}

	/* Open MTD device and get sector size */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0 ){
		perror(mtd);
		ret++;
		goto fail;
	}
	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0 ||
	    mtd_info.erasesize < sizeof(struct boot_header)) {
		perror(mtd);
		ret++;
		goto fail;
	}

	/* See if we have enough memory to store the whole file */
	sysinfo(&info);
	if ((info.freeram + info.bufferram) >= PMON_SIZE)
		erase_info.length = ROUNDUP(PMON_SIZE, mtd_info.erasesize);
	else
		erase_info.length = mtd_info.erasesize;

	/* Allocate temporary buffer */
	if (!(buf = malloc(PMON_SIZE))) {
		perror("malloc");
		ret++;
		goto fail;
	}
	//memset(&buf,0,sizeof(buf));

	/* Write file or URL to MTD device */
	for (erase_info.start = 0; erase_info.start < PMON_SIZE ; erase_info.start += count) {
		len = MIN(erase_info.length, PMON_SIZE - erase_info.start);
		if (erase_info.start == 0) {
			count = sizeof(struct boot_header);
			off = 0;
			//memcpy(buf, &boot, sizeof(struct boot_header));
		} else
			count = off = 0;

		if (fp)
			count += fread(&buf[off], 1, len - off, fp);

		printf("Read data size = [%ld]\n",count - sizeof(struct boot_header));

		if (count < 0) {
			perror(path);
			ret++;
			goto fail;
		}
		if (count < len) {
			ret++;
			goto fail;
		}

		/* update mac to new boot */
		lan_mac = nvram_get("et0macaddr");
		printf("Restore mac [%s] to new boot, the location is 0x%x\n", lan_mac, MAC_START_ADDRESS);

		for(i=0;i<17;i++)
			buf[MAC_START_ADDRESS+i] = *(lan_mac+i);
		buf[MAC_START_ADDRESS+17] = '\0';
		
		/* Do it */
		printf("Erasing %s...\n",mtd);
                (void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0){
			perror(mtd);
			ret++;
                        goto fail;
		}

		printf("Writing data to %s ...\n",mtd);
		ret_count = write(mtd_fd, buf, count - sizeof(struct boot_header));	//skip the header size
		if (ret_count != count - sizeof(struct boot_header)) {
			perror(mtd);
			ret++;
			goto fail;
		}
	}

	if(strcmp(path,"/bin/boot.bin")){	// from tftp update boot.bin
		nvram_set("boot_ver","");	// The boot ver set to null, so we can update old boot.
		nvram_commit();
		printf("Erasing first 128K of kernel\n");	// Force to update code.bin, when update boot.bin.
		/* erase first 128k of kernel */
		if(mtd_erase_sector("linux",0,128*1024) != 0){	
			ret++;
			goto fail;
		}
	}
	else	// from code.bin update boot.bin
		;;

	ret = 0;

	printf("Done\n");

 fail:
	if (buf)
		free(buf);
	if (mtd_fd >= 0)
		close(mtd_fd);
	if (fp)
		fclose(fp);

	unlink(WRITE_IMAGE);
	return ret;
}

/* PMON default nvram address is from 0x400 to 0x2400,
 * We use 0x2000 to store 8's mac.
 * Please confirm the address 0x2000 to 0x2400 is full 0xFF.
 * Because the user space cann't directly access flash value, 
 * so i use kernel to write mac.
 * The mac filename must be mac.bin.
 *
 * mac.bin format :
 * new format:
 * byte 0 - 5  :  mac
 * old format: (no use)
 * byte 0      :  increment count
 * byte 1 - 4  :  ip address
 * byte 5      :  start position
 * byte 6 - 11 :  start mac
 * byte 12 - 15:  no used
 * byte 16 - 21:  end mac
 */

int
write_mac(const char *path)
{
	char string[]="string=xx:xx:xx:xx:xx:xx";
	unsigned char buf[80];
	unsigned char mac[18];
	FILE *fp;
	int count=0;

	//cprintf("Write mac start...\n");
	cprintf("Write %s start...\n", path);

	if ((fp = fopen(path, "r")))
		count = fread(buf, sizeof(char), 80, fp);
	fclose(fp);

	cprintf("%s size=[%d]\n", path, count);

	if (count < 0) {
		perror(path);
		return 1;
	}
	if(strstr(path,"mac.bin")) {
		buf[0] = buf[0] & 0xFE; //  *unmask the 0th bit (valid mac addr should leave this bit as 0)

		sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

		cprintf("Download lan mac=[%s]\n",mac);

		snprintf(string, sizeof(string),"string=%s",mac);

		eval("insmod","writemac","flag=set", string, "type=mac");

		sleep(2);
		//junzhao 2004.3.22
		nvram_set("mac_write", "1");

		nvram_commit();
	}
#ifdef WRITE_SN_SUPPORT
	else if(strstr(path,"sn.bin")) {

		if ( buf[count - 1] == 0x0a )
			snprintf( buf, count, "%s", buf );

		cprintf( "Download Serial No.=[%s]\n", buf );

		snprintf(string, sizeof(string),"string=%s",buf);

		eval("insmod","writemac","flag=set", string, "type=sn");
		
		sleep(2);
	}
#endif
	//junzhao 2004.3.19
	/*
	nvram_set("lan_def_hwaddr", mac);

	memset(mac, 0, sizeof(mac));
	sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]+1);
	cprintf("Download wan mac=[%s]\n",mac);
	nvram_set("wan_def_hwaddr", mac);
	
	nvram_commit();	
	*/
//	write_wl_mac(mac); /* barry add for WRT54G v1.1 single board, write wl mac to EEPROM ! */

//	cprintf("Write mac done...\n");
	cprintf("Write %s done...\n", path);

	return 0;
}

/*
 * Erase an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @param	start	start address to erase
 * @param	end	end address to erase
 * @return	0 on success and errno on failure
 */
int
mtd_erase_sector(const char *mtd, unsigned long start, unsigned long end)
{
	int mtd_fd;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	printf("mtd_erase_sector(%s,%ld,%ld) doing",mtd,start,end);

	/* Open MTD device */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0) {
		perror(mtd);
		return errno;
	}

	/* Get sector size */
	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(mtd);
		close(mtd_fd);
		return errno;
	}

	erase_info.length = mtd_info.erasesize;

	for (erase_info.start = start;
	     erase_info.start < end;
	     erase_info.start += mtd_info.erasesize) {
                (void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
			perror(mtd);
			close(mtd_fd);
			return errno;
		}
	}

	close(mtd_fd);
	return 0;
}
