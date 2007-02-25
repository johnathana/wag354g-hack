#include "7z.h"
#include "PipedInOutStreams.h"

#include "LZMAEncoder.h"
#include "LZMADecoder.h"

extern "C"{
//bool compress_lzma_7z(unsigned algo, unsigned dictionary_size, unsigned num_fast_bytes) throw () {
int compress_lzma_7z_buf(char *source, unsigned source_size, char *dest, unsigned *dest_size){
	unsigned int algo = 0;		// 0 best
	unsigned int dictionary_size = 1 << 15;	// 20 default, 21 for other
	unsigned int num_fast_bytes = 0x20;	// 0x20 default, 0x40 for other
//	unsigned int num_fast_bytes = 32;
	unsigned dest_size_p=*dest_size;
	unsigned int size;

	try {
		NCompress::NLZMA::CEncoder cc;
		
		if (cc.SetDictionarySize(dictionary_size) != S_OK)
			return 0;

		if (cc.SetEncoderNumFastBytes(num_fast_bytes) != S_OK)
			return 0;

		if (cc.SetEncoderAlgorithm(algo) != S_OK)
			return 0;

		ISequentialInStream* in = new ISequentialInStream(source, source_size);
		ISequentialOutStream* out = new ISequentialOutStream(dest+4, dest_size_p-4);
//		ISequentialInStream* in = new PipedInStream();
//		ISequentialOutStream* out = new PipedOutStream();
		
		if (cc.WriteCoderProperties(out) != S_OK)
			return 0;

		if (cc.Code(in, out) != S_OK)
			return 0;

		/* Save off the size into the start of the buffer */
		size=source_size;
		memcpy(dest, &size, 4);

		*dest_size=out->size_get()+4;
		return 1;
	} catch (...) {
		return 0;
	}
}

int decompress_lzma_7z_buf(char *source, unsigned source_size, char *dest, unsigned *dest_size){
	unsigned dest_size_p=*dest_size;

	try {
		NCompress::NLZMA::CDecoder cc;

//		ISequentialInStream* in = new PipedInStream();
//		ISequentialOutStream* out = new PipedOutStream();
		ISequentialInStream* in = new ISequentialInStream(source+4, source_size-4);
		ISequentialOutStream* out = new ISequentialOutStream(dest, dest_size_p);

		if (cc.ReadCoderProperties(in) != S_OK)
			return 0;

		if (cc.Code(in, out) != S_OK)
			return 0;

		*dest_size=out->size_get();
		return 1;
	} catch (...) {
		return 0;
	}
}

}	/* Extern "c" */

bool compress_lzma_7z(unsigned algo, unsigned dictionary_size, unsigned num_fast_bytes) throw () {
	try {
		NCompress::NLZMA::CEncoder cc;
		
		if (cc.SetDictionarySize(dictionary_size) != S_OK)
			return false;

		if (cc.SetEncoderNumFastBytes(num_fast_bytes) != S_OK)
			return false;

		if (cc.SetEncoderAlgorithm(algo) != S_OK)
			return false;

		ISequentialInStream* in = new PipedInStream();
		ISequentialOutStream* out = new PipedOutStream();
		
		if (cc.WriteCoderProperties(out) != S_OK)
			return false;

		if (cc.Code(in, out) != S_OK)
			return false;

		return true;
	} catch (...) {
		return false;
	}
}

bool decompress_lzma_7z() throw () {
	try {
		NCompress::NLZMA::CDecoder cc;

		ISequentialInStream* in = new PipedInStream();
		ISequentialOutStream* out = new PipedOutStream();

		if (cc.ReadCoderProperties(in) != S_OK)
			return false;

		if (cc.Code(in, out) != S_OK)
			return false;

		return true;
	} catch (...) {
		return false;
	}
}
