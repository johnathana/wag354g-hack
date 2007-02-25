
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

/*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: upgrade.c,v 1.4 2006/05/11 07:14:46 dom Exp $
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

#include <broadcom.h>
//#include <cyutils.h>
#include <unistd.h>
#include <sys/reboot.h>

#define MIN_BUF_SIZE    4096
#define CODE_PATTERN_ERROR 9999
static int upgrade_ret;
static int restore_type;//zhangbin for backup&restore
static int upgrade_type;  //0: normal upgrade from lan, 1: test from wan side, 2: download from wan side 3: cancel the download request.


#ifdef WAN_UPGRADE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PROTOCOL	"HTTP/1.1"
#define MAX_SIZE		20480
//#define MODEL_NAMEA	"WAG54GV2_AnnexA_FCC_v"
//#define MODEL_NAMEB      "WAG54GV2_AnnexB_FCC_v"
#define	MODEL_NAMEA	"wag54gv2_annexa_fcc_v"
#define MODEL_NAMEB	"wag54gv2_annexb_fcc_v"
#define MODEL_NAME_LEN		21	//strlen(MODEL_NAME)
#define EXT_NAME		"_code.bin"
#define EXT_NAME_LEN		9    //strlen(EXT_NAME)

struct code_header{
	char magic[4];
	char res1[4];
	char fwdate[3];
	char fwvern[3];
	char id[4];
	unsigned char res2[14];
	unsigned char length[16];
} ;
#define CODE_PATTERN "WAG2"

#define BUFLEN (1 << 16)

#define MAGIC_NUMBER 0xC453DE23
#define 		BLOCKSIZE				(64 * 1024)
#define 		CODE_MTD				"/dev/mtdblock/1"

static unsigned long crctab[256] =
{
  0x0,
  0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
  0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
  0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
  0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
  0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
  0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
  0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
  0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
  0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
  0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
  0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
  0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
  0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
  0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
  0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
  0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
  0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
  0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
  0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
  0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
  0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
  0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
  0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
  0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
  0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
  0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
  0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
  0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
  0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
  0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
  0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
  0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
  0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
  0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
  0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
  0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
  0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
  0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
  0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
  0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
  0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
  0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
  0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
  0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
  0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
  0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
  0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
  0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
  0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
  0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
  0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

int cs_is_tagged(FILE *fp)
{
  char buf[8];

  fseek(fp, -8, SEEK_END);
  fread(buf, 8, 1, fp);
  if(*(unsigned long*)buf == MAGIC_NUMBER)
    return 1;
  return 0;
}

unsigned long cs_read_sum(FILE *fp)
{
  char buf[8];

  fseek(fp, -8, SEEK_END);
  fread(buf, 8, 1, fp);
  return *((unsigned long*)&buf[4]);
}

int cs_calc_sum(FILE *fp, unsigned long *res, int tagged)
{
  unsigned char buf[BUFLEN];
  unsigned long crc = 0;
  uintmax_t length = 0;
  size_t bytes_read;

  fseek(fp, 0, SEEK_SET);
//fseek(fp, 32 + 32 + 32 + 4, SEEK_SET);/*total+kernel+filesystem+codepattern*/
  while((bytes_read = fread(buf, 1, BUFLEN, fp)) > 0)
  {
    unsigned char *cp = buf;

    if(length + bytes_read < length)
      return 0;

    if(bytes_read != BUFLEN && tagged)
      bytes_read -= 8;

    length += bytes_read;
    while(bytes_read--)
      crc =(crc << 8) ^ crctab[((crc >> 24) ^ *cp++) & 0xFF];
  }

  if(ferror(fp))
    return 0;

  for(; length; length >>= 8)
    crc =(crc << 8) ^ crctab[((crc >> 24) ^ length) & 0xFF];

  crc = ~crc & 0xFFFFFFFF;

  *res = crc;

  return 1;
}

unsigned long cs_calc_buf_sum(char *buf, int size)
{
  unsigned long crc = 0;
  char *cp = buf;
  unsigned long length = size;

  while(size--)
    crc =(crc << 8) ^ crctab[((crc >> 24) ^ *cp++) & 0xFF];

  for(; length; length >>= 8)
    crc =(crc << 8) ^ crctab[((crc >> 24) ^ length) & 0xFF];

  crc = ~crc & 0xFFFFFFFF;

  return crc;
}

int cs_set_sum(FILE *fp, unsigned long sum, int tagged)
{
  unsigned long magic = MAGIC_NUMBER;

  if(tagged)
    fseek(fp, -8, SEEK_END);
  else
  {
    fseek(fp, 0, SEEK_END);
  }
  if(fwrite(&magic, 1, 4, fp) < 4)
    return 0;
  if(fwrite(&sum, 1, 4, fp) < 4)
    return 0;

  return 1;
}

void cs_get_sum(FILE *fp, unsigned long *sum)
{
  unsigned long magic = 0;

  fseek(fp, -8, SEEK_END);

  fread(&magic, 4, 1, fp);
  fread(sum, 4, 1, fp);
}

/* CHECK THE FIRMWARE DOWNLOADED IS RIGHT OR WRONG 
  * RETURN VALUE: 0 CHECKSUM IS RIGHT
  *				 -1 CHECKSUM IS WRONG
  * PARAMETER:	 FIRMWARE FILE NAME
  */
int firmware_checksum(char *filename)
{
	int fd = 0;
	unsigned long sum = 0, res = 0;
	FILE *pFile = NULL;
	unsigned long total_len = 0;
	char total_len_buf[48];
	struct code_header *header;

	
	if((fd = open(filename, O_RDONLY)) == -1)
	{
		printf("OPEN FIRMWARE FILE FAILED\n");
		unlink(filename);
		return -1;
	}
	if(read(fd, total_len_buf, 48) != 48)
	{
		printf("READ FIRMWARE MAGIC FAILED\n");
		close(fd);
		return -1;
	}
	header = (struct code_header *)(total_len_buf);
	total_len = atol(header->length);


	if(strncmp(header->magic, CODE_PATTERN, 4))
	{
		printf("CODE PATTERN MISMATCH\n");
		close(fd);
		return -1;
	}
	close(fd);
	
	if((fd = open(filename, O_RDWR)) == -1)
	{
		printf("OPEN FIRMWARE FILE FAILED\n");
		return -1;
	}
	
	if(ftruncate(fd, total_len + 32 + 16 + 8) < 0)
	{
		printf("ftruncate FIRMWARE FILE FAILED\n");
		close(fd);
		return -1;
	}
	close(fd);

	
	
	if((pFile = fopen(filename, "rw+")) == NULL)
	{
		printf("FOPEN FIRMWARE FILE FAILED\n");
		unlink(filename);
		return -1;
	}

	if(cs_is_tagged(pFile) == 0)
	{
		printf("NO_CHECKSUM_ERROR\n");
		fclose(pFile);
		return -1;
	}

	if(cs_calc_sum(pFile, &sum, 1) == 0)
	{
		printf("CALC_CHECKSUM_ERROR\n");
		fclose(pFile);
		return -1;
	}

	cs_get_sum(pFile, &res);
	if(sum != res)
	{
		printf("CRC_ERROR\n");
		fclose(pFile);
		return -1;
	}
	fclose(pFile);
	return 0;
}

/* Write  the file to Flash */

int write_mtd(char *device, char *filename)
{
	int src, dst;
	int rd = 0, wr = 0;
	unsigned long size = 0, offset, wrote = 0;
	char *buf;
	struct stat sz;
	char tmp_buf[64];
	
	buf = malloc(BLOCKSIZE);
	if (buf == NULL)
		return -1;

	src = open(filename, O_RDONLY);
	dst = open(device, O_WRONLY);

	if ((src == -1) || (dst == -1))
		return -1;
	
	if ((src == 0) || (dst == 0))
	{
		close(src);
		close(dst);
		return -1;
	}

	fstat(src, &sz);
	size = sz.st_size;
	size -= (32 + 16);  //sub the size of the code header


	//delete the data of the code header of the firmware
	if(read(src, tmp_buf, 48) != 48)
	{
		printf("DELETE THE CODE HEADER OF THE FIRMWARE \n");
		close(src);
		close(dst);
		return -1;
	}
	while ((rd = read(src, buf, BLOCKSIZE)) > 0)
	{
		if ((wrote + rd) >= size)
			rd -= 8;
		if (rd < 0)
			rd = 0;
		
		offset = 0;
		while (((wr = write(dst, &buf[offset], (rd - offset))) > 0) && (rd - offset))
			offset += wr;
		wrote += offset;
		printf("copy_firmware_image: Wrote %lu bytes.\n", wrote);
	}

	close(dst);
	close(src);

	free(buf);

	if ((rd < 0) || (wr < 0))
	{
		printf("copy_firmware_image: failed!\n");
		if (rd < 0)
			return -1;
		else
			return -1;
	}
	printf("copy_firmware_image: successful!\n");
	return 0;
}

#endif

int 
//do_upgrade_cgi(char *url, FILE *stream)
do_upgrade_cgi(char *url, webs_t stream) //jimmy, https, 8/6/2003
{

if (upgrade_type == 0)
{
	if(upgrade_ret)
	{
		//zhangbin 2004.08.25
		if(restore_type)
        		do_ej("Fail_r_s.asp",stream);
		else
			do_ej("Fail_u_s.asp",stream);
		websDone(stream, 200);
	}
	else
	{
		//dom 2005/06/29
		if(restore_type)
			do_ej("Success_r_s.asp",stream);
		else
			do_ej("Success_u_s.asp",stream);
		websDone(stream, 200);
		cprintf("Ready to restart!\n");
		//nvram_set("restore_defaults", "1");
		//sys_commit();
		sleep(4);
		reboot(RB_AUTOBOOT);
	}
}
#ifdef WAN_UPGRADE
else if (upgrade_type == 1)
{
	cprintf("WAN TEST: upgrade_ret = %d\n", upgrade_ret);
	nvram_set("upgrade_wan_test", "1");
	if(upgrade_ret)
	{
		cprintf("WAN TEST FAILED\n");
		do_ej("Fail_wan_u.asp", stream);
	}
	else
		do_ej("Upgrade.asp", stream);
	nvram_set("upgrade_wan_test", "0");
	
	websDone(stream, 200);
}
else if (upgrade_type == 2)
{

	if(upgrade_ret)
	{
		do_ej("Fail_u_s.asp", stream);
		websDone(stream, 200);
	}
	else
	{
		do_ej("Success_u_s.asp", stream);
		websDone(stream, 200);
		cprintf("Ready to restart!\n");
		//nvram_set("restore_defaults", "1");
		//sys_commit();
		sleep(2);
		reboot(RB_AUTOBOOT);
	}
}
else if (upgrade_type == 3) //cancel download firmware from wan side
{
	printf(" NOTIFY THE USER because of NEW VERSION..............\n");
	nvram_set("new_version", "0");
	do_ej("Upgrade.asp", stream);
	websDone(stream, 200);
}
#endif
	/* Reboot if successful */
/*	if (upgrade_ret == 0){
		sleep(1);
		sys_reboot();
	}
	*/
	return 0;
}

int
//sys_upgrade(char *url, FILE *stream, int *total)
sys_upgrade(char *url, webs_t stream, int *total, int type) //jimmy, https, 8/6/2003
{
	char upload_fifo[] = "/tmp/uploadXXXXXX";
	FILE *fifo = NULL;
	char *write_argv[4];
	pid_t pid;
	char *buf = NULL;
	int count, ret = 0;
	long flags = -1;
	int size = BUFSIZ;
	int i=0;


#ifdef BACKUP_RESTORE_SUPPORT
	if(type == 1){
		write_argv[0] = "restore";
		write_argv[1] = upload_fifo;
		write_argv[2] = "/dev/mtd/3";
		write_argv[3] = NULL;
	}else
#endif
	{
#if 0
		write_argv[0] = "write";
		write_argv[1] = upload_fifo;
		write_argv[2] = "linux";
		write_argv[3] = NULL;
#else
		write_argv[0] = "upgrade_firmware";
		write_argv[1] = upload_fifo;
		write_argv[2] = "0";
		write_argv[3] = NULL;
#endif
	}	

	if (url)
	{
		write_argv[1] = url;
		write_argv[2] = "1";
		_eval(write_argv, NULL,  0, &pid);
		waitpid(pid, &ret, 0);
		goto err;
	};
	
	//diag_led(DIAG, START_LED);  // blink the diag led
//	C_led(1);
#ifdef HTTPS_SUPPORT
	if(do_ssl)
		ACTION("ACT_WEBS_UPGRADE");
	else
#endif
		ACTION("ACT_WEB_UPGRADE");
	

	/* Feed write from a temporary FIFO */
#if 0
	if (!mktemp(upload_fifo) ||
		mkfifo(upload_fifo, S_IRWXU) < 0||
		(ret = _eval(write_argv, NULL, 0, &pid)) ||
		!(fifo = fopen(upload_fifo, "w"))) {
			if (!ret)
				ret = errno;
			goto err;
	}
#endif
	if(!(fifo = fopen(upload_fifo, "w"))) {
		goto err;
	}
	/* Set nonblock on the socket so we can timeout */
	if(!do_ssl){
		if ((flags = fcntl(fileno(stream), F_GETFL)) < 0 ||
			fcntl(fileno(stream), F_SETFL, flags | O_NONBLOCK) < 0) {
				ret = errno;
				goto err;
		}
	}

	/*
	 ** The buffer must be at least as big as what the stream file is
	 ** using so that it can read all the data that has been buffered 
	 ** in the stream file. Otherwise it would be out of sync with fn
	 ** select specially at the end of the data stream in which case
	 ** the select tells there is no more data available but there in 
	 ** fact is data buffered in the stream file's buffer. Since no
	 ** one has changed the default stream file's buffer size, let's
	 ** use the constant BUFSIZ until someone changes it.
	 **/

	if (size < MIN_BUF_SIZE)
                size = MIN_BUF_SIZE;
        if ((buf = malloc(size)) == NULL) {
                ret = ENOMEM;
                goto err;
        }

	 /* Pipe the rest to the FIFO */
	cprintf("Upgrading\n");
	while (total && *total) {
#ifdef HTTPS_SUPPORT
		if(do_ssl){
			if (size > *total) size=*total;
			count = wfread(buf, 1, size, stream);
		}else
#endif
		{
			if (waitfor(fileno(stream), 5) <= 0){
			         cprintf("waitfor timeout 5 secs\n");
			         break;
			}
			count = safe_fread(buf, 1, size, stream);
	                if (!count && (ferror(stream) || feof(stream)))
				                        break;
		}

	*total -= count;
		safe_fwrite(buf, 1, count, fifo);
		cprintf(".");
		i++;
	}
	fclose(fifo);
	fifo = NULL;

#ifdef MULTILANG_SUPPORT //dom 2005/08/12
	if (!chdir("/tmp/new_www"))
	{
		eval("/sbin/erase", "4");
		chdir("/www");
	}
#endif

	/* Wait for write to terminate */
	_eval(write_argv, NULL,  0, &pid);
	waitpid(pid, &ret, 0);
	cprintf("done\n");
	
	if(!do_ssl){
		/* Reset nonblock on the socket */
		if (fcntl(fileno(stream), F_SETFL, flags) < 0) {
			ret = errno;
			goto err;
		}
	}
	
err:
	if (buf)
		free(buf);
	if (fifo)
		fclose(fifo);
	unlink(upload_fifo);
	
	//diag_led(DIAG, STOP_LED);
//	C_led(0);
	ACTION("ACT_IDLE");


	return ret;
}


//wwzh
#ifdef WAN_UPGRADE

/* return value : 0 success, 1 error */
int parse_firmware_url(char *ipaddr, int *port, char *pathname, char *url)
{
	char *ptr;
	char value[255];
	int len =0;
	struct hostent *host;
	struct sockaddr_in addr;
	
	ptr = strstr(url, "http://");
	if (ptr != NULL)
	{
		ptr += strlen("http://");
		memset((void *)value, 0, sizeof(value));
		strcpy(value, ptr);
	}
	else
		return -1;
		//strcpy(value, url);
	ptr = strchr(value, '/');
	if (ptr != NULL)
	{
		strcpy(pathname, ptr);
		*ptr = '\0';
	}
	else
		strcpy(pathname, "/");
	len = strlen(pathname);
	if (pathname[len - 1] != '/')
	{
		pathname[len] = '/';
		pathname[len + 1] = '\0';
	}
	ptr = strstr(value, ":");
	if (ptr == NULL)
	{
		*port = 80;
	}
	else
	{
		*ptr = '\0';
		*port = atoi(ptr + 1);	
	}
	host = gethostbyname(value);
	if (host == NULL)
		return -1;
	memcpy(&addr.sin_addr, host->h_addr, sizeof(addr.sin_addr));
	strcpy(ipaddr, inet_ntoa(addr.sin_addr));
	return 0;
	
}

int create_download_connect(char *ipaddr, int port)
{
	int ret;
	struct sockaddr_in serveraddr;
	int fd;
	int max;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		printf("Error: create download socket failed\n");
		return -1;
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	max = MAX_SIZE;
	ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&max, sizeof(int));
	if (ret < 0)
	{
		printf("SETSOCKOPT SNDBUF ERROR\n");
	}
	ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret == -1)
	{
		printf("Error: download connect failed\n");
		close(fd);
		return -1;
	}
	return fd;
}

int envelope_http_get_packet(char *packet, char *ipaddr, char *pathname)
{
	int len;
	
	len = sprintf(packet, "GET %s %s\r\nAccept:  */*\r\nAccept-Encoding: gzip, deflate\r\nHOST:%s\r\nConnection: Keep-Alive\r\n\r\n", 
				pathname, PROTOCOL, ipaddr);
	
	return len;
}

int get_firmware_location_page(char *page, char *url)
{
	int fd;
	char ipaddr[20];
	int port;
	char pathname[63];
	int ret;
	char packet[127];
	int len;
	int i;
	
	ret = parse_firmware_url(ipaddr,  &port, pathname, url);
	if (ret == -1)
		return -1;
	fd = create_download_connect(ipaddr,  port);
	if (fd == -1)
		return -1;
	len = envelope_http_get_packet(packet, ipaddr, pathname);
	packet[len] = '\0';
	ret = send(fd, packet, len, 0);
	if (ret == -1)
	{
		close(fd);
		return -1;
	}
	len = recv(fd, page, MAX_SIZE, 0);
	//len = recv(fd, page, MAX_SIZE, MSG_WAITALL);
	if (len <= 0)
	{
		close(fd);
		return -1;
	}
	page[len] = '\0';
	
	for (i = 0; i < len; i++)
		page[i] = tolower(page[i]);
	
	close(fd);
	return len;
}

#if 0
int get_latest_firmware_version(char *page, char *latest_version)
{
	char *ptr;
	char version[63];
	char *version_start, *version_end;
	int len;
	

	memset((void *)latest_version, 0, sizeof(latest_version));
	while((ptr = strstr(page, CODE_MODEL_NAME)))
	{
		version_start = ptr + CODE_MODEL_NAME_LEN;
		version_end = strstr(version_start, EXT_NAME);
		if (version_end != NULL)
		{
			len = version_end - version_start;
			memcpy(version,  version_start, len);
			version[len] = '\0';
			printf("version = %s\n", version);
			if (strcmp(version, latest_version) >  0)
			{
				memset((void *)latest_version, 0, sizeof(latest_version));
				memcpy(latest_version, version, len);
				latest_version[len] = '\0';
				printf("latest_version = %s\n", latest_version);
			}
			version_start = NULL;
			version_end = NULL;
		}
		page = strstr(ptr, "</a>");
		/*
		if (page == NULL)
			page = strstr(ptr, "</A>");
		*/
	}
	if (!strcmp(latest_version, ""))
		return -1;
	return 0;
}
#else
int get_latest_firmware_version(char *page, char *latest_version_ddm, char *latest_version_noddm)
{
	char *ptr = NULL;
	char version[63];
	char *version_start, *version_end;
	int len;
	int ddm = 0;  //0 no ddm, 1 ddm
	char buffer[1023];
	char *href_start, *href_end;
	int type = -1;  

	type = atoi(nvram_get("hardware_type")); //0 show AnnexA, 1 show AnnexB
	
	
	while((href_start = strstr(page, "href=")))
	{
		href_end = strstr(page, "</a>");
		if (href_end == NULL)
		{
			page += strlen("href=");
			continue;
		}
		else
			page = href_end + strlen("</a>");
	
		len = href_end - href_start;
		memset((void *)buffer, 0, sizeof(buffer));
		memcpy(buffer, href_start, len);
		buffer[len] = '\0';

		if (type == 0)
		{
			ptr = strstr(buffer, MODEL_NAMEA);
			if (ptr == NULL)
				continue;
		}
		else if (type == 1)
		{
			ptr = strstr(buffer, MODEL_NAMEB);
			if (ptr == NULL)
				continue;
		}
		version_start = ptr + MODEL_NAME_LEN;
		version_end = strstr(version_start, EXT_NAME);
		if (version_end != NULL)
		{
			len = version_end - version_start;
			memcpy(version,  version_start, len);
			version[len] = '\0';
			ptr = strchr(version, 'v');
			if (ptr != NULL)
				ddm = 1 ; //ddm code
			else
				ddm = 0; //no ddm code
				
			
			if (ddm == 0)
			{
				if (strcmp(version, latest_version_noddm) >  0)
				{
					memcpy(latest_version_noddm, version, len);
					latest_version_noddm[len] = '\0';
					
				}
			}
			else if (ddm == 1)
			{
				if (strcmp(version, latest_version_ddm) >  0)
				{
					memcpy(latest_version_ddm, version, len);
					latest_version_ddm[len] = '\0';
					
				}
			}
			version_start = NULL;
			version_end = NULL;
		}
		
	}
	
	if ((latest_version_ddm[0] == '\0') && (latest_version_noddm[0] == '\0'))
		return -1;
	return 0;
}

#endif
int download_firmware(char *savename, char *url, char *latest_version)
{
	int len = 0;
	char *header_start, *header_end;
	char buff[1024];
	FILE *file = NULL;
	int total_bytes = 0;
	char *ptr, *data;
	char getname[127];
	char pathname[127];
	char ipaddr[20];
	int port;
	int ret;
	int fd;
	int download_len = 0;
	char *protocol, *number, *result, *cp;
	char rcv_buffer[MAX_SIZE];
	int type = -1; 

	type = atoi(nvram_get("hardware_type")); //0 show AnnexA, 1 show AnnexB
	
	memset((void *)buff, 0, sizeof(buff));
	memset((void *)getname, 0, sizeof(getname));
	memset((void *)ipaddr, 0, sizeof(ipaddr));

	 ret = parse_firmware_url(ipaddr, &port, pathname, url);
	//ret = parse_download_url(ipaddr, &port, getname);
	if (ret == -1)
		return -1;

	fd = create_download_connect(ipaddr, port);
	if (fd == -1)
		return -1;
	if (type == 0)
		len = sprintf(getname, "%s%s%s%s", pathname, MODEL_NAMEA, latest_version, EXT_NAME);
	else if (type == 1)
		len = sprintf(getname, "%s%s%s%s", pathname, MODEL_NAMEB, latest_version, EXT_NAME);
	getname[len] = '\0';
	len = envelope_http_get_packet(buff, ipaddr, getname);
	buff[len] = '\0';

	ret = send(fd, buff, strlen(buff), 0);
	if (ret == -1)
	{
		close(fd);
		printf("Error: send GET download file failed\n");
		return -1;
	}

	file = fopen(savename, "wb");
	if (file == NULL)
	{
		close(fd);
		printf("Error: open %s to save file of download failed\n", savename);
		return -1;
	}
	memset((void *)rcv_buffer, 0, MAX_SIZE);
	ret = recv(fd, rcv_buffer, sizeof(rcv_buffer), 0);
	if (ret < 0)
	{
		close(fd);
		fclose(file);
		printf("Error: download recv file failed\n");
		return -1;
	}

	data = strdup(rcv_buffer);
	protocol = number = data;
	strsep(&number, " ");
	while(*number == ' ')
		number++;
	result = number;
	strsep(&result, " ");
	while(*result == ' ')
		result++;
	cp = result;
	strsep(&cp, "\r\n");


	if ((strncasecmp(result, "OK", 2) == 0) && (strncasecmp(number, "200", 2) == 0) 
			&& (strncasecmp(protocol, PROTOCOL, 8) == 0))
	{
		free(data);
		header_start = strstr(rcv_buffer, protocol);
		ptr = strstr(rcv_buffer, "Content-Length:");
		if (ptr == NULL)
		{
			close(fd);
			fclose(file);
			printf("Error: don't find Content-Length string\n");
			return -1;
		}
		ptr += 15;
		ptr += strspn(ptr, "\t");
		total_bytes = strtoul(ptr, NULL, 0);
		header_end = strstr(header_start, "\r\n\r\n");
		if (header_end == NULL)
		{
			close(fd);
			fclose(file);
			printf("Error: don't find two LR/CR\n");
			return -1;
		}
		header_end += strlen("\r\n\r\n");
		len = header_end - header_start;
		ret -= len;
		fwrite(header_end, 1, ret,  file);
		total_bytes -= ret;
		download_len = ret;
		while (total_bytes)
		{
			printf("total_bytes ============ %d\n", total_bytes);
			memset((void *)rcv_buffer, 0, MAX_SIZE);
			len = 0;
			len = recv(fd, rcv_buffer, MAX_SIZE, 0);
			if (len <= 0)
				break;
			download_len += len;
			total_bytes -= len;
			if (total_bytes < 0)
			{
				fwrite(rcv_buffer, 1, total_bytes, file);
				break;
			}
			else
				fwrite(rcv_buffer, 1, len, file);
		
		}
		close(fd);
		fclose(file);
		return 0;
	}
	else
	{
		free(data);
		close(fd);
		fclose(file);
		printf("Error: DOWNLOAD URL RETURN ERROR\n");
		return -1;
	}
	return -1;
}

int wan_upgrade_first()
{
	int ret;
	char latest_version[63];
	char latest_version_ddm[63];
	char latest_version_noddm[63];
	char *ptr;
	char url1[127], url2[127];
	char page[MAX_SIZE];
	int flag = 0; // 1 show url1, 2 show url2
	char current_version[63];
	int ddm = -1; //0 show noddm, 1 show ddm
	
	nvram_set("no_wan_upgrade_version", "0");
	memset((void *)current_version, 0, sizeof(current_version));
	memset((void *)page, 0, MAX_SIZE);
	memset((void *)latest_version, 0, sizeof(latest_version));
	memset((void *)latest_version_ddm, 0, sizeof(latest_version_ddm));
	memset((void *)latest_version_noddm, 0, sizeof(latest_version_noddm));
	
	strcpy(current_version, nvram_get("software_version"));
	ptr = strchr(current_version, 'v');
	if (ptr != NULL)
		ddm = 1; //the current version includes ddm
	else
		ddm = 0; //noddm
		
	strcpy(url1, nvram_get("wan_upgrade_url1"));
	ret = get_firmware_location_page(page, url1);
	flag = 1;
	if (ret == -1)
	{
		memset((void *)page, 0, MAX_SIZE);
		strcpy(url2, nvram_get("wan_upgrade_url2"));
		ret = get_firmware_location_page(page, url2);
		flag = 2;
		if (ret == -1)
			return -1;
	}
	ret = get_latest_firmware_version(page, latest_version_ddm, latest_version_noddm);
	/*
	if (strcmp(latest_version, current_version) <= 0)
		ret = -1;
	*/
	if (ret == -1)
	{
		if (flag == 1)
		{
			memset((void *)page, 0, MAX_SIZE);
			strcpy(url2, nvram_get("wan_upgrade_url2"));
			flag = 2;
			ret = get_firmware_location_page(page, url2);
			if (ret == -1)
				return -1;
			ret = get_latest_firmware_version(page, latest_version_ddm, latest_version_noddm);
			if (ret == -1)
				return -1;
		}
		else
			return -1;
	}
	
	if (ddm == 1)
		strcpy(latest_version, latest_version_ddm);
	else if (ddm == 0)
		strcpy(latest_version, latest_version_noddm);
	printf("latest_version ==== %s\n", latest_version);

	if (!strcmp(latest_version, ""))
		return -1;
	ptr = strchr(latest_version, '_');
	if (ptr != NULL)
		*ptr = '\0';  //delete serial number of firmware verion file 
	if (strcmp(latest_version, current_version) <= 0)
	{
		nvram_set("no_wan_upgrade_version", "1");
		return 1;
	}
	else
		nvram_set("no_wan_upgrade_version", "0");
	if (flag == 1)
		nvram_set("upgrading_wan_url", url1);
	else
		nvram_set("upgrading_wan_url", url2);

	nvram_set("latest_version", latest_version);
	return 0;
	
}
//notify the user when there is a new version 
void wan_upgrade_notify(int sig)
{
	int ret;
	ret = wan_upgrade_first();
	if (ret == 0)
		nvram_set("new_version", "1");
	else
		nvram_set("new_version", "0");
	nvram_commit();
}


int wan_upgrade_second()
{
	char latest_version[63];
	char filename[63];
	int len;
	int ret;
	char url[127];
	int type = -1;

	type = atoi(nvram_get("hardware_type")); //0 show AnnexA, 1 show AnnexB
	strcpy(latest_version, nvram_get("latest_version"));
	strcpy(url, nvram_get("upgrading_wan_url"));
	if (type == 0)
		len = sprintf(filename, "/tmp/%s%s%s", MODEL_NAMEA, latest_version, EXT_NAME);
	else if (type == 1)
		len = sprintf(filename, "/tmp/%s%s%s", MODEL_NAMEB, latest_version, EXT_NAME);
	filename[len] = '\0';
	nvram_set("wan_upgrade_filename", filename);
	ret = download_firmware(filename, url, latest_version);
	return ret;
}
int wan_upgrade_test(char *url, webs_t stream, int *total, int type)
{

	char *buf = NULL;
	int count, ret = 0;
	long flags = -1;
	int size = BUFSIZ;
	int i=0;

	/* Set nonblock on the socket so we can timeout */
	if(!do_ssl){
		if ((flags = fcntl(fileno(stream), F_GETFL)) < 0 ||
			fcntl(fileno(stream), F_SETFL, flags | O_NONBLOCK) < 0) {
				ret = errno;
				goto err;
		}
	}

	/*
	 ** The buffer must be at least as big as what the stream file is
	 ** using so that it can read all the data that has been buffered 
	 ** in the stream file. Otherwise it would be out of sync with fn
	 ** select specially at the end of the data stream in which case
	 ** the select tells there is no more data available but there in 
	 ** fact is data buffered in the stream file's buffer. Since no
	 ** one has changed the default stream file's buffer size, let's
	 ** use the constant BUFSIZ until someone changes it.
	 **/

	if (size < MIN_BUF_SIZE)
                size = MIN_BUF_SIZE;
        if ((buf = malloc(size)) == NULL) {
                ret = ENOMEM;
                goto err;
        }

	 
	while (total && *total) {
#ifdef HTTPS_SUPPORT
		if(do_ssl){
			if (size > *total) size=*total;
			count = wfread(buf, 1, size, stream);
		}else
#endif
		{
		#if 0
			if (waitfor(fileno(stream), 5) <= 0){
			         cprintf("waitfor timeout 5 secs\n");
			         break;
			}
		#endif
			count = safe_fread(buf, 1, size, stream);
	                if (!count && (ferror(stream) || feof(stream)))
				                        break;
		}
		*total -= count;
		i++;
	}
	
	if(!do_ssl){
		/* Reset nonblock on the socket */
		if (fcntl(fileno(stream), F_SETFL, flags) < 0) {
			ret = errno;
			goto err;
		}
	}
	
err:
	if (buf)
		free(buf);
	if (wan_upgrade_first() == -1)
		return -1;
	return ret;
	
}
										
int wan_upgrade_download(char *url, webs_t stream, int *total, int type)
{
	
	char *buf = NULL;
	int count, ret = 0;
	long flags = -1;
	int size = BUFSIZ;
	int i=0;


	/* Set nonblock on the socket so we can timeout */
	if(!do_ssl){
		if ((flags = fcntl(fileno(stream), F_GETFL)) < 0 ||
			fcntl(fileno(stream), F_SETFL, flags | O_NONBLOCK) < 0) {
				ret = errno;
				goto err;
		}
	}

	/*
	 ** The buffer must be at least as big as what the stream file is
	 ** using so that it can read all the data that has been buffered 
	 ** in the stream file. Otherwise it would be out of sync with fn
	 ** select specially at the end of the data stream in which case
	 ** the select tells there is no more data available but there in 
	 ** fact is data buffered in the stream file's buffer. Since no
	 ** one has changed the default stream file's buffer size, let's
	 ** use the constant BUFSIZ until someone changes it.
	 **/

	if (size < MIN_BUF_SIZE)
                size = MIN_BUF_SIZE;
        if ((buf = malloc(size)) == NULL) {
                ret = ENOMEM;
                goto err;
        }

	while (total && *total) {
#ifdef HTTPS_SUPPORT
		if(do_ssl){
			if (size > *total) size=*total;
			count = wfread(buf, 1, size, stream);
		}else
#endif
		{
		//wwzh 
		#if 0
			if (waitfor(fileno(stream), 5) <= 0){
			         cprintf("waitfor timeout 5 secs\n");
			         break;
			}
		#endif
			count = safe_fread(buf, 1, size, stream);
	                if (!count && (ferror(stream) || feof(stream)))
				                        break;
		}

		*total -= count;
		i++;
	}
	
	if(!do_ssl){
		/* Reset nonblock on the socket */
		if (fcntl(fileno(stream), F_SETFL, flags) < 0) {
			ret = errno;
			goto err;
		}
	}
	
err:
	if (buf)
		free(buf);
	if (wan_upgrade_second() == -1)
		return -1;
	return ret;
}


int wan_upgrade_cancel(char *url, webs_t stream, int *total, int type)
{
	
	char *buf = NULL;
	int count, ret = 0;
	long flags = -1;
	int size = BUFSIZ;
	int i=0;


	/* Set nonblock on the socket so we can timeout */
	if(!do_ssl){
		if ((flags = fcntl(fileno(stream), F_GETFL)) < 0 ||
			fcntl(fileno(stream), F_SETFL, flags | O_NONBLOCK) < 0) {
				ret = errno;
				goto err;
		}
	}

	/*
	 ** The buffer must be at least as big as what the stream file is
	 ** using so that it can read all the data that has been buffered 
	 ** in the stream file. Otherwise it would be out of sync with fn
	 ** select specially at the end of the data stream in which case
	 ** the select tells there is no more data available but there in 
	 ** fact is data buffered in the stream file's buffer. Since no
	 ** one has changed the default stream file's buffer size, let's
	 ** use the constant BUFSIZ until someone changes it.
	 **/

	if (size < MIN_BUF_SIZE)
                size = MIN_BUF_SIZE;
        if ((buf = malloc(size)) == NULL) {
                ret = ENOMEM;
                goto err;
        }

	while (total && *total) {
#ifdef HTTPS_SUPPORT
		if(do_ssl){
			if (size > *total) size=*total;
			count = wfread(buf, 1, size, stream);
		}else
#endif
		{
		//wwzh add for canceling failed
		#if 0
			if (waitfor(fileno(stream), 5) <= 0){
			         cprintf("waitfor timeout 5 secs\n");
			         break;
			}
		#endif
			count = safe_fread(buf, 1, size, stream);
	                if (!count && (ferror(stream) || feof(stream)))
				                        break;
		}

	*total -= count;
		i++;
	}
	
	if(!do_ssl){
		/* Reset nonblock on the socket */
		if (fcntl(fileno(stream), F_SETFL, flags) < 0) {
			ret = errno;
			goto err;
		}
	}
	
err:
	if (buf)
		free(buf);
	return ret;
}

#endif

void
//do_upgrade_post(char *url, FILE *stream, int len, char *boundary)
do_upgrade_post(char *url, webs_t stream, int len, char *boundary) //jimmy, https, 8/6/2003
{
	char buf[1024];
	int type = 0;
	char *filename;
	char *str;
	int url_upgrade = 0; /*0: normal upgrade; 1: url upgrade 2: url from wan side*/
	restore_type = 0;//zhangbin 2004.08.25
	upgrade_ret = EINVAL;
	
	
	// Let below files loaded to memory
	// To avoid the successful screen is blank after web upgrade.
	system("cat /www/Success_u_s.asp > /dev/null");
	system("cat /www/Fail_u_s.asp > /dev/null");

	
//	if(NULL != str)
//		printf("filename = %s\n", str);
	/* Look for our part */
	while (len > 0) {
		if (!wfgets(buf, MIN(len + 1, sizeof(buf)), stream))
			return;
		len -= strlen(buf);
		if (!strncasecmp(buf, "Content-Disposition:", 20)){
			if(strstr(buf, "name=\"file\"")){	// upgrade image
				type = 0;
				printf("type upgrade ****\n");
				if((filename = strstr(buf, "filename=")) != NULL)
				{
					str = filename;
					filename = strsep(&str, "\"");
					filename = str;
					str = strsep(&filename, "\"");
					filename = str;
					str = strsep(&filename, " ");
					filename = str;
					if(!strncmp(filename, "http://", 7))
						url_upgrade = 1;
					printf("str = %s\n", str);
					printf("url_upgrade = %d\n", url_upgrade);
				}
					
				break;
			}
#ifdef BACKUP_RESTORE_SUPPORT
			else if(strstr(buf, "name=\"restore\"")){	// restore configuration
				restore_type = 1; //zhang 2004.08.25
				type = 1;
				break;
			}
#endif
		}
	//wwzh
#ifdef WAN_UPGRADE
	     if (!strncasecmp(buf, "upgrade_wan_test", 16))
	    {
	    		printf("UPGRADE FROM WAN SIDE AND THE FIRST\n");
			url_upgrade = 2;
	    }
	   if (!strncasecmp(buf, "upgrade_wan_download", 20))
	   {
	   	     printf("UPGRADE FROM WAN SIDE AND BEGIN TO DOWNLOAD\n");
			 url_upgrade = 3;
	   }
	   if (!strncasecmp(buf, "upgrade_wan_cancel", 18))
	   {
	   	printf("Cancel WAN UPGRADE\n");
		url_upgrade = 4;
	   }
#endif

	}

	/* Skip boundary and headers */
	while (len > 0) {
		if (!wfgets(buf, MIN(len + 1, sizeof(buf)), stream))
			return;
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))
			break;
	}
	if (url_upgrade < 2)
	{
#if 1	
		//junzhao;2004-3-20
		eval("killall", "cron");
		eval("killall", "zebra");
		eval("killall", "ripd");
		eval("killall", "-9", "snmpd");
//	eval("killall", "snmpd");
		eval("killall", "upnpd-igd");
		eval("killall", "wanledctrl");
		eval("killall", "checkadsl");
		eval("killall", "cped");
		eval("killall", "redial");
		//eval("killall", "udhcpc");
	//	eval("killall", "ntpd");
	//	eval("killall", "ntpclient");
	//	eval("killall", "ez-ipupdate");
		eval("killall", "pluto");
	//	eval("killall", "email_alert");
		eval("killall", "dnsmasq");
		eval("killall", "adslpolling");
	//	eval("killall", "resetbutton");
		eval("killall", "syslogd");
		eval("killall", "klogd");
#endif
	}
	if(url_upgrade == 0)
	{	
		upgrade_type = 0;
		upgrade_ret = sys_upgrade(NULL, stream, &len, type);
	}
	else if(url_upgrade == 1)
	{
		upgrade_type = 0;
		upgrade_ret = sys_upgrade(filename, stream, &len, type);
	}
#ifdef WAN_UPGRADE
	else if (url_upgrade == 2)
	{
		upgrade_type = 1;
		upgrade_ret = wan_upgrade_test(NULL, stream, &len, type);
	}
	else if (url_upgrade == 3)
	{
		upgrade_type = 2;
		upgrade_ret = wan_upgrade_download(NULL, stream, &len, type);
		if (upgrade_ret == 0)
		{
			/* download successfully, begin writing the file to flash */
			int ret;
			char wan_upgrade_filename[63];

			strcpy(wan_upgrade_filename, nvram_get("wan_upgrade_filename"));
			printf("FILENAME = %s\n", wan_upgrade_filename);
			ret = firmware_checksum(wan_upgrade_filename);
			if (ret == -1)
				upgrade_ret = -1;
		
			ret = write_mtd(CODE_MTD, wan_upgrade_filename);
			if (ret == -1)
				upgrade_ret = -1;
		
			unlink(wan_upgrade_filename);
		}
	}
	else if (url_upgrade == 4)
	{
		upgrade_type = 3;
		upgrade_ret = wan_upgrade_cancel(NULL, stream, &len, type);
		printf("CANCEL UPGRADE DOWNLOAD\n");
	}
#endif

}

