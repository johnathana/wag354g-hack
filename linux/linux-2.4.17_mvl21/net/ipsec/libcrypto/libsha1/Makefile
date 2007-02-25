#
# SSLeay/crypto/sha/Makefile
#

DIR=    sha
TOP=    ../..
CC=     cc
CPP=    $(CC) -E
INCLUDES=
CFLAG=-g -I ../include -D__KERNEL__ -I/usr/src/linux/include
INSTALL_PREFIX=
OPENSSLDIR=     /usr/local/ssl
INSTALLTOP=/usr/local/ssl
MAKE=           make -f Makefile.ssl
MAKEDEPEND=	$(TOP)/util/domd $(TOP)
MAKEFILE=       Makefile.ssl
AR=             ar r
RANLIB=		ranlib
PERL=		perl


CFLAGS= $(INCLUDES) $(CFLAG)

ASM-$(ARCH_ASM):=1
ASM_X86:=$(ASM-i586)$(ASM-i686)
ifneq ($(strip $(ASM_X86)),)
SHA1_ASM_OBJ:=asm/sx86-elf.o
override CFLAGS += -DSHA1_ASM
endif

GENERAL=Makefile
TEST=shatest.c sha1test.c
APPS=

LIB=libsha1.a
LIBSRC=sha_dgst.c sha1dgst.c sha1_one.c hmac_sha1.c
LIBOBJ=sha_dgst.o sha1dgst.o sha1_one.o $(SHA1_ASM_OBJ) hmac_sha1.o

SRC= $(LIBSRC)

EXHEADER= sha.h
HEADER= sha_locl.h $(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

#top:
#	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

all:    lib

lib:	$(LIB)

$(LIB):    $(LIBOBJ)
	$(AR) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB)

# elf
asm/sx86-elf.o: asm/sx86unix.cpp
	$(CPP) -DELF -x c asm/sx86unix.cpp | as -o asm/sx86-elf.o

# solaris
asm/sx86-sol.o: asm/sx86unix.cpp
	$(CC) -E -DSOL asm/sx86unix.cpp | sed 's/^#.*//' > asm/sx86-sol.s
	as -o asm/sx86-sol.o asm/sx86-sol.s
	rm -f asm/sx86-sol.s

# a.out
asm/sx86-out.o: asm/sx86unix.cpp
	$(CPP) -DOUT asm/sx86unix.cpp | as -o asm/sx86-out.o

# bsdi
asm/sx86bsdi.o: asm/sx86unix.cpp
	$(CPP) -DBSDI asm/sx86unix.cpp | sed 's/ :/:/' | as -o asm/sx86bsdi.o

asm/sx86unix.cpp: asm/sha1-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) sha1-586.pl cpp $(PROCESSOR) >sx86unix.cpp)

files:
	$(PERL) $(TOP)/util/files.pl Makefile.ssl >> $(TOP)/MINFO

links:
	@$(TOP)/util/point.sh Makefile.ssl Makefile
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@for i in $(EXHEADER) ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	$(MAKEDEPEND) $(INCLUDES) $(DEPFLAG) $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f asm/sx86unix.cpp *.o *.obj $(LIB) tags core .pure .nfs* *.old *.bak fluff asm/*.o

# DO NOT DELETE THIS LINE -- make depend depends on it.
