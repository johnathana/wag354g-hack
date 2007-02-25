CFLAGS = -Os
CXXFLAGS = $(CFLAGS)
AR=ar
RANLIB=ranlib


LIB_OBJS = 	7zlzma.o AriBitCoder.o CRC.o DeflateDecoder.o DeflateEncoder.o \
		HuffmanEncoder.o IInOutStreams.o InByte.o LenCoder.o LiteralCoder.o \
		LSBFDecoder.o LSBFEncoder.o LZMA.o LZMADecoder.o LZMAEncoder.o \
		OutByte.o PipedInOutStreams.o WindowIn.o WindowOut.o cmdline.o \
		getopt1.o getopt.o

TEST_PROG_OBJS = test_prog.o small_decode/LzmaDecode.o small_decode/LzmaWrapper.o

all:	liblzma.a test_prog

liblzma.a:	$(LIB_OBJS)
	$(AR) rc liblzma.a $(LIB_OBJS)
	$(RANLIB) liblzma.a

test_prog: liblzma.a $(TEST_PROG_OBJS)
	$(CXX) $(CFLAGS) -o testprog $(TEST_PROG_OBJS) -L. -llzma

7z:	liblzma.a 7z.o
	$(CXX) $(CXXFLAGS) -o 7z_test 7z.o -L. -llzma

clean:
	rm -f *.o *.a test_prog 7z_test testprog small_decode/*.o

7zlzma.o:		7zlzma.cc
AriBitCoder.o:		AriBitCoder.cc
CRC.o:			CRC.cc
DeflateDecoder.o:	DeflateDecoder.cc
DeflateEncoder.o:	DeflateEncoder.cc
HuffmanEncoder.o:	HuffmanEncoder.cc
IInOutStreams.o:	IInOutStreams.cc
InByte.o:		InByte.cc
LenCoder.o:		LenCoder.cc
LiteralCoder.o:		LiteralCoder.cc
LSBFDecoder.o:		LSBFDecoder.cc
LSBFEncoder.o:		LSBFEncoder.cc
LZMA.o:			LZMA.cc
LZMADecoder.o:		LZMADecoder.cc
LZMAEncoder.o:		LZMAEncoder.cc
OutByte.o:		OutByte.cc
PipedInOutStreams.o:	PipedInOutStreams.cc
WindowIn.o:		WindowIn.cc
WindowOut.o:		WindowOut.cc
7z.o:			7z.cc
cmdline.o:		cmdline.c
getopt1.o:		getopt1.c
getopt.o:		getopt.c
test_prog.o:		test_prog.c liblzma.h
small_decode/LzmaDecode.o:	small_decode/LzmaDecode.c small_decode/LzmaDecode.h
small_decode/LzmaWrapper.o:	small_decode/LzmaWrapper.c small_decode/LzmaWrapper.h small_decode/LzmaDecode.h
