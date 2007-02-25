#ifndef  __LIBLZMA_WRAPPER_H__
#define  __LIBLZMA_WRAPPER_H__

int compress_lzma_7z_buf(char *source, unsigned source_size, char *dest, unsigned *dest_size);
int decompress_lzma_7z_buf(char *source, unsigned source_size, char *dest, unsigned *dest_size);

#endif /*__LIBLZMA_WRAPPER_H__*/
