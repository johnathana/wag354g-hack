#
# Linux netconf Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.2 2005/03/28 06:58:10 sparq Exp $
#

CFLAGS	+= -I. -I$(TOP)/shared -I$(TOP)/iptables/include -I$(SRCBASE)/include -I$(LINUXDIR)/include -Wall
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L. -L$(TOP)/iptables/libiptc

all: libnetconf.so netconf

install: all
	install -D libnetconf.so $(INSTALLDIR)/usr/lib/libnetconf.so
	$(STRIP) $(INSTALLDIR)/usr/lib/libnetconf.so

clean:
	rm -f *.o *.a *.so netconf

libnetconf.so: netconf_linux.o
	$(LD) -shared -o $@ $^ $(LDFLAGS) -liptc

netconf: main.o libnetconf.so
	$(CC) -o $@ $< $(LDFLAGS) -lnetconf
