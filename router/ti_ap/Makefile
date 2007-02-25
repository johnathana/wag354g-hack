#
# Router Wireless Interface Configuration Utility Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.5 2005/03/22 12:50:28 sparq Exp $
#

include $(TOP)/.config

ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -I$(SRCBASE)/
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS	+= -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared -lm

OBJS := ap.o mem.o hex.o ap_ioctl.o iwlib.o

ifeq ($(CONFIG_ANTSEL),y)
OBJS += antsel.o
endif

all:

clean:

wlcfg: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mipsel-uclibc-strip wlcfg
	#cp wlcfg /home/leijun
module:
	make -C AP-DK5.7.0.4/make
