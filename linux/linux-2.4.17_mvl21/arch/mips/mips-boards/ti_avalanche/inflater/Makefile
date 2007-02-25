#
# File intended to be called from top level Makefile during kernel build if compressed
# image for atlas is requested.
#

LOCALLINKFLAGS = -static -G 0 -Map inflate.map -R $(TOPDIR)/vmlinux

ifdef CONFIG_CPU_LITTLE_ENDIAN
tool-prefix	= mipsel-linux-
output-format	= elf32-tradlittlemips
else
tool-prefix	= mips-linux-
output-format	= elf32-tradbigmips
endif


ifdef CONFIG_KERNEL_COMPRESS_7ZIP
OBJ_FILES	= inflate.o Decoder.o
INFLATE_DEP	= inflate.c lzma_decoder.h Decoder.o
ZIMAGE_XZ	= zimage.7z
else
OBJ_FILES	= inflate.o
INFLATE_DEP	= inflate.c gzip.h
ZIMAGE_XZ	= zimage.gz
endif


# Image depends=====================================
ram_zimage:	$(OBJ_FILES) zimage.o Makefile ld.script
	$(LD) $(LOCALLINKFLAGS) -no-warn-mismatch -T ld.script $(OBJ_FILES) zimage.o -o inflate
	$(NM) inflate | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aU] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > ram_zimage.map
	$(OBJCOPY) -O srec inflate $(TOPDIR)/ram_zimage.sre


# File depends======================================
inflate.o:	$(INFLATE_DEP) Makefile

Decoder.o:	Decoder.c lzma_decoder.h Decoder.h Makefile

zimage.o:	$(ZIMAGE_XZ) zimage.script Makefile
	$(CROSS_COMPILE)ld -T zimage.script -r -b binary $(ZIMAGE_XZ) -o zimage.o

zimage.7z:	Makefile $(TOPDIR)/vmlinux
	$(CROSS_COMPILE)objcopy -S -O binary $(TOPDIR)/vmlinux zimage
	if [ -e ../../../../../../../../gtnsp/tools/bin/7zip ]; then \
		../../../../../../../../gtnsp/tools/bin/7zip zimage zimage.7z; \
	else	\
                if [ -e ../../../../../tools/7zip ]; then \
                        ../../../../../tools/7zip zimage zimage.7z; \
		else \
			7zip zimage zimage.7z; \
		fi; \
	fi

zimage.gz:	Makefile $(TOPDIR)/vmlinux
	$(CROSS_COMPILE)objcopy -S -O binary $(TOPDIR)/vmlinux zimage
	gzip -vf9 zimage

ld.script:	ld.script.in Makefile
	sed -e 's/@@OUTPUT_FORMAT@@/$(output-format)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

zimage.script:	zimage.script.in Makefile
	sed -e 's/@@OUTPUT_FORMAT@@/$(output-format)/' \
	    -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@

clean:	
	rm -f $(OBJ_FILES) ZIMAGE_XZ inflate inflate.sre System.map zimage.o ld.script zimage.script ram_zimage.map
	rm -f $(TOPDIR)/srec2bin
	-rm -f inflate.map zimage zimage.7z
