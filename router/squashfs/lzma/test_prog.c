#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "liblzma.h"
#include "small_decode/LzmaWrapper.h"

#define IN_SIZE	(32*1024)

int in_size=IN_SIZE;
char	in_buf[IN_SIZE];

int	out_size=2*IN_SIZE;
char	out_buf[2*IN_SIZE];

int	deflate_size=2*IN_SIZE;
char	deflate_buf[2*IN_SIZE];

int	deflate2_size=2*IN_SIZE;
char	deflate2_buf[2*IN_SIZE];

void	init_mem()
{
	int i;
	for(i=0; i< IN_SIZE; i++){
		in_buf[i]=(char)rand();
	}
}

int write_buffer(int fd, const void *buf, size_t count)
{
	int sent;
	int	num_retries=50;
	while(count){
		sent = write(fd, buf, count);
		if(sent < 0){
			return -1;
		}
		count -= sent;
	}
	return 0;
}

void write_stats(char *filename, const void *buf, size_t size)
{
	int i;
	unsigned int *p;
	p=(unsigned int *)buf;

	printf("Writing to file %s, size is %d\n", filename, size);
	for(i=0; i< 4; i++){
		printf("%2.2d: %8.8x %8.8x %8.8x %8.8x\n", i, p[4*i], p[4*i+1], p[4*i+2], p[4*i+3]);
	}
	printf("\n\n");
}

int write_file(char *filename, const void *buf, size_t size)
{
	int	fd;

	fd=open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if(fd<0)
		return fd;
	if(write_buffer(fd, buf, size)<0)
		return -1;
	close(fd);
	write_stats(filename, buf, size);
}

int main(int argc, char **argv)
{
	unsigned char *internal_data;
	int		internal_size;
	int	res;

	init_mem();
	write_file("testfile_in", in_buf, in_size);

	/* compress the in file */
	compress_lzma_7z_buf(in_buf, in_size, out_buf, &out_size);
	write_file("testfile_lzma", out_buf, out_size);

	/* uncompress the out file */
	decompress_lzma_7z_buf(out_buf, out_size, deflate_buf, &deflate_size);
	write_file("testfile_decompress", deflate_buf, deflate_size);

	/* uncompress the out file with the other decompressor */
	internal_size=lzma_workspace_size();
	internal_data=(unsigned char*)malloc(internal_size);
	lzma_init(internal_data, internal_size);

	res=lzma_inflate(out_buf, out_size, deflate2_buf, &deflate2_size);
	write_file("testfile_decompress2", deflate2_buf, deflate2_size);
	printf("lzma result = %d\n", res);

	free(internal_data);

	return 0;
}
