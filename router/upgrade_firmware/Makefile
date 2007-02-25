#
# Router rc control scripts Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
#
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.2 2005/03/21 09:05:27 sparq Exp $
#

include $(TOP)/.config

ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -I$(SRCBASE)/
CFLAGS  += -I$(TOP)/iptables/include -I$(TOP)/iptables/include/libipq
CFLAGS	+= -s -O2

ifeq ($(CONFIG_WLCONF),y)
CFLAGS += -DWIRELESS_SUPPORT
endif

LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared 

OBJS := cfgmgr_cksum.o firmware_upgrade.o http.o

vpath %.c $(TOP)/shared $(SRCBASE)/rts/src

all:

clean:

install: 
	#install -d $(INSTALLDIR)/usr/sbin
	install upgrade_firmware  $(INSTALLDIR)/usr/sbin

upgrade_firmware: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(OBJS): $(CY_DEPS)
