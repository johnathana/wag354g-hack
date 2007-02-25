CC=mipsel-uclibc-gcc
STRIP=mipsel-uclibc-strip

CFLAGS	+= -I. -I../shared -I$(SRCBASE)/include -Wall -I$(SRCBASE)/
LDFLAGS	+= -L../nvram -lnvram -L../shared -lshared -lm

#LIBS = -lm -L../nvram -lnvram  -L../shared -lshared
OBJ = miniserver.o xmlpharse.o soappost.o tools.o upnpd_log.o portmapping.o

#export FLTFLAGS = -s 40960
vpath %.c ../shared

all:

upnpd-igd: miniserver.o xmlpharse.o soappost.o tools.o upnpd_log.o portmapping.o
	$(CC) -Wall $(OBJ)  $(LDFLAGS) -o upnpd-igd $(LIBS)
	$(STRIP) -s upnpd-igd

miniserver.o:
	$(CC) -Wall -c miniserver.c
xmlpharse.o:
	$(CC) -Wall -c xmlpharse.c
soappost.o:
	$(CC) -Wall -c soappost.c
tools.o:
	$(CC) -Wall -c tools.c
upnpd_log.o:
	$(CC) -Wall -c upnpd_log.c
portmapping.o:
	$(CC) -Wall -c portmapping.c	

clean:

install: 
	install -d $(INSTALLDIR)/usr/sbin
	install upnpd-igd $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/etc
	cp igd $(INSTALLDIR)/etc/ -a

