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
# $Id: Makefile,v 1.5 2005/08/25 09:04:16 sparq Exp $
#

include $(TOP)/.config

ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

ifeq ($(CWMP_SUPPORT),1)
CFLAGS  += -I$(TOP)/libmng/
endif
CFLAGS	+= -I. -Iti_wireless -I$(TOP)/shared -I$(SRCBASE)/include -Wall -I$(SRCBASE)/
CFLAGS  += -I$(TOP)/iptables/include -I$(TOP)/iptables/include/libipq
CFLAGS	+= -s -O2

ifeq ($(CONFIG_WLCONF),y)
CFLAGS += -DWIRELESS_SUPPORT
endif

ifeq ($(CWMP_SUPPORT),1)
LDFLAGS+=-L$(TOP)/libmng -lmng
endif
LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared  -L$(TOP)/lib -latm

#kirby 2004/12.23
ifeq ($(CLI_SUPPORT),1)
LDFLAGS += -lcrypt
endif

OBJS := rc.o init.o interface.o atm.o network.o services.o udhcpc.o ppp.o http.o stats.o mtd.o  crc.o  autodetect.o base64.o rc_ipsec.o ArpLanIp.o
OBJS += writeimage.o resetbutton.o ntp.o iptable.o ddns.o listen.o monitor_ps.o snmpvar.o

#csh 2005.3.11
ifeq ($(QOS_SUPPORT),1)
OBJS += rc_qos.o
endif
ifeq ($(QOS_ADVANCED_SUPPORT),1)
OBJS += rc_qos_advanced.o
endif

#kirby 2004/12.23
ifeq ($(CLI_SUPPORT),1)
OBJS += mketc.o
endif

ifeq ($(UNNUMBERED_SUPPORT),1)
OBJS += unip.o
endif

ifeq ($(IPCP_NETMASK_SUPPORT),1)
OBJS += unip_ipcp.o
endif

#kirby 2004/12.23
ifeq ($(SECLAN_SUPPORT),1)
OBJS += seclan.o
endif

ifeq ($(GRE_SUPPORT), 1)
OBJS += rc_gre_tunnel.o
else
ifeq ($(CLOUD_SUPPORT), 1)
OBJS += rc_gre_tunnel.o
OBJS += rc_cloud.o
endif
endif

ifdef HEARTBEAT_SUPPORT
OBJS += heartbeat.o
endif

ifeq ($(CONFIG_IPTABLES),y)
OBJS += firewall.o
endif

#junzhao 2004.4.7
#add by leijun 2005-01-04
ifeq ($(CONFIG_WLCONF),y)
ifeq ($(WL_OLD_CODE),y)
OBJS += wireless.o
else
OBJS += wl_new.o
endif
endif

vpath %.c $(TOP)/shared $(SRCBASE)/rts/src

all: rc

clean:
	rm -f *.o rc

install: all
	install -d $(INSTALLDIR)/sbin
	install rc $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/rc
	cd $(INSTALLDIR)/sbin && ln -sf rc init
	cd $(INSTALLDIR)/sbin && ln -sf rc erase
	cd $(INSTALLDIR)/sbin && ln -sf rc write
	cd $(INSTALLDIR)/sbin && ln -sf rc restore
	cd $(INSTALLDIR)/sbin && ln -sf rc stats
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug

	cd $(INSTALLDIR)/sbin && ln -sf rc filter
	cd $(INSTALLDIR)/sbin && ln -sf rc resetbutton
	cd $(INSTALLDIR)/sbin && ln -sf rc filtersync
ifeq ($(FIREWALL_LEVEL_SUPPORT),1)
	cd $(INSTALLDIR)/sbin && ln -sf rc filterinb
	cd $(INSTALLDIR)/sbin && ln -sf rc filterinbsync
endif
	cd $(INSTALLDIR)/sbin && ln -sf rc ntpd
	cd $(INSTALLDIR)/sbin && ln -sf rc ipupdated
	cd $(INSTALLDIR)/sbin && ln -sf rc redial
	cd $(INSTALLDIR)/sbin && ln -sf rc hb_connect
	cd $(INSTALLDIR)/sbin && ln -sf rc hb_disconnect
	cd $(INSTALLDIR)/sbin && ln -sf rc gpio
	cd $(INSTALLDIR)/sbin && ln -sf rc listen
	cd $(INSTALLDIR)/sbin && ln -sf rc monitor_ps
	cd $(INSTALLDIR)/sbin && ln -sf rc ddns_success


rc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mipsel-uclibc-strip -s rc

$(OBJS): $(CY_DEPS)
