#
# milli_httpd Makefile
#
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.2 2005/12/16 09:53:04 sparq Exp $
#

include $(TOP)/.config
ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CFLAGS	+= -I. -Iti_wireless -I$(TOP)/shared -I$(SRCBASE)/include -Wall -I$(SRCBASE)/ 
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2

ifeq ($(CONFIG_WLCONF),y)
CFLAGS += -DWIRELESS_SUPPORT
endif

#CFLAGS += -DQUICK_SETUP_WIZARD

LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared -lm

ifeq ($(HSIAB_SUPPORT),1)
CFLAGS += -I$(TOP)/hsiab
LDFLAGS += -L$(TOP)/openssl -L$(TOP)/hsiab -lcrypto -lssl -lhsiab
OBJS += hsiab.o
endif
ifeq ($(HTTPS_SUPPORT),1)
CFLAGS += -I$(TOP)/openssl/include  -I$(TOP)/openssl/include/openssl
CFLAGS += -I../rc/
LDFLAGS += -L$(TOP)/openssl -L../openssl -lcrypto -lssl
OBJS += common.o server.o
endif
#xiaoqin add start for nomad 2005.04.11
ifeq ($(NOMAD_SUPPORT),1)
CFLAGS += -I../rc/
OBJS += nomad.o quickvpn.o
endif
#xiaoqin add end
ifeq ($(BACKUP_RESTORE_SUPPORT),1)
#zhangbin 2004.0825
#OBJS  += $(SRCBASE)/../tools/src/code_header.o backup_restore.o 
OBJS  += backup_restore.o
endif
ifeq ($(DDM_SUPPORT),1)
OBJS  += ddm.o
endif
#ifeq ($(ARP_TABLE_SUPPORT),1)
#OBJS  += arp.o
#endif
ifeq ($(EMI_TEST),1)
OBJS  += emi_test.o
endif
ifeq ($(SETUP_WIZARD_SUPPORT),1)
OBJS  += setup_wizard.o
endif
ifeq ($(WL_CALIBRATION_ONBOARD_SUPPORT),1)
OBJS  += calibration.o
endif
ifeq ($(DEBUG_WEB),1)
CFLAGS += -DMY_DEBUG
endif
ifeq ($(WAKE_ON_LAN_SUPPORT),1)
OBJS  += wol.o
endif
ifeq ($(SAMBA_SUPPORT),1)
OBJS  += samba.o
endif
#ifeq ($(HW_QOS_SUPPORT),1)
#OBJS  += qos.o
#endif
ifeq ($(PERFORMANCE_SUPPORT),1)
OBJS  += performance.o
endif
ifeq ($(FIREWALL_LEVEL_SUPPORT),1)
OBJS  += filters_inb.o
endif

OBJS += common.o server.o
OBJS  += setup_wizard.o pvc.o ipsec.o email_alert.o ping_config.o ipsec_service.o vpn_setting.o diagstart.o iprestrict.o pvc_connection.o #pvc_routing.o Alg_timeout.o

#csh 2005.3.11
ifeq ($(QOS_SUPPORT),1)
OBJS += qos.o
endif
ifeq ($(QOS_ADVANCED_SUPPORT),1)
OBJS += qos_advanced.o
endif

#junzhao 2004.4.7
#leijun 2005-01-04
ifeq ($(CONFIG_WLCONF),y)
ifeq ($(WL_OLD_CODE),y)
OBJS += wireless.o
else
OBJS += ../ti_ap/wireless.o ../ti_ap/ap_ioctl.o ../ti_ap/iwlib.o
endif
endif

#add for IGMP Proxy 2004-10-10
ifeq ($(IGMP_PROXY_SUPPORT), 1)
OBJS += igmp_proxy.o
endif

ifeq ($(GRE_SUPPORT),1)
OBJS += gre_tunnel.o
else 
ifeq ($(CLOUD_SUPPORT), 1)
OBJS += gre_tunnel.o
endif
endif

ifeq ($(STB_SUPPORT),1)
OBJS += stb.o
endif

#zhs add
ifeq ($(ADVANCED_FORWARD_SUPPORT),1)
OBJS += advanced_forwarding.o
endif
#end by zhs

#vpath %.c $(TOP)/shared
vpath %.c ../shared

all: httpd

clean:
	rm -f *.o *~ httpd

install: all
	install -D httpd $(INSTALLDIR)/usr/sbin/httpd
	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd
	install -d $(INSTALLDIR)/etc
	install *.pem $(INSTALLDIR)/etc


httpd: cgi.o ej.o httpd.o broadcom.o \
       index.o status.o dhcp.o log.o vpnlog.o upgrade.o filters.o forward.o dynamic_route.o static_route.o hex.o ddns.o \
       find_pattern.o lib.o diag.o sysinfo.o wepkey.o md5c.o wl_test.o getservice.o management_snmp.o $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mipsel-uclibc-strip -s httpd

*.o: $(CY_DEPS)
