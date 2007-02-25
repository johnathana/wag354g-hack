CC = gcc
ZLIB = libztemp.a
ZLIB_VER = 1.1.3
LZMA=yes

ifeq ($(LZMA), yes)
#lzma stuff
ifeq ($(FLAT), yes)
LDFLAGS = -L../../tools/lib/ -Llzma -llzma
else
LDFLAGS = -L../../../tools/lib -Llzma -llzma
endif
LZMA_LIB = lzma/liblzma.a
LZMA_DEFINE = -D__TI_SQUASHFS_LZMA

else
# zlib stuff
ifeq ($(FLAT), yes)
LDFLAGS = -L../../tools/lib/ -lztemp
else
LDFLAGS = -L../../../tools/lib -lztemp
endif

endif

#CFLAGS = -Wall -O2 -g -I./include -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
CFLAGS = -Wall -O2 -g -I./include  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE $(LZMA_DEFINE) -U__REDIRECT

TARGETS = mksquashfs

all: $(TARGETS)

mksquashfs: mksquashfs.o read_fs.o $(LZMA_LIB)
	$(CXX) $(CFLAGS) $(LZMA_DEFINE) -o $@ mksquashfs.o read_fs.o $(LDFLAGS)

read_fs.o: read_fs.c read_fs.h lzma/liblzma.h

mksquashfs.o: mksquashfs.c mksquashfs.h lzma/liblzma.h

lzma/liblzma.a:
	make -C lzma liblzma.a

clean:
	make -C lzma clean
	rm -f $(TARGETS)
	rm -f *.o *~

.PHONY: all clean
