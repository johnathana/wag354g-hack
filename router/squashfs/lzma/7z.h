#ifndef __7Z_H
#define __7Z_H

bool compress_lzma_7z(unsigned algo, unsigned dictionary_size, unsigned num_fast_bytes) throw ();
bool decompress_lzma_7z() throw ();

#endif

