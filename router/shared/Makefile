#
# Linux router shared code Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.7 2006/09/08 04:27:31 dom Exp $
#

include $(TOP)/.config

ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CFLAGS	+= -I. -I$(SRCBASE)/include -Wall -I$(SRCBASE)/ -I$(KERNELDIR)/include
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L.

ifeq ($(CONFIG_WLCONF),y)
CFLAGS += -DWIRELESS_SUPPORT
endif

OBJS := nvparse.o shutils.o wl.o wl_linux.o linux_timer.o defaults.o utils.o \
        support.o md5c.o gpio.o qkvpncls.o promenv.o

ifeq ($(CONFIG_DHCP_DOMAIN_IP_MAPPING),y)
OBJS += http.o
endif 

ifeq ($(WL_OLD_CODE),y)
OBJS += ledapp.o
else
OBJS += ledapp_new.o
endif

all: libshared.so

install: all
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 libshared.so $(INSTALLDIR)/usr/lib
	$(STRIP) $(INSTALLDIR)/usr/lib/libshared.so

clean:
	rm -f *.o *.so

libshared.so: $(OBJS)
	$(LD) -shared -o $@ $^
	mipsel-uclibc-strip -s libshared.so

*.o: $(CY_DEPS)
