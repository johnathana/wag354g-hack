#
# Broadcom Linux Router Makefile
# 
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.20.2.2 2006/08/24 06:43:43 johnny Exp $
#

include .config
ifneq ($(wildcard ../cy_conf.mak),)
  include ../cy_conf.mak
endif
ifneq ($(wildcard ../cy_codepattern.mak),)
  include ../cy_codepattern.mak
endif

KERNELDIR=$(SRCBASE)/linux/linux-2.4.17_mvl21


ifeq ($(PROJECT),AG241)
CONFIG_WLCONF=n
endif

###add for wireless APDK VERSION by wwzh
#5.5.4 5.5.20
#leijun modify 2005-01-04
ifeq ($(FLASH_SIZE), 8)
ifeq ($(WL_OLD_CODE),y)
APDK_VERSION := 5.5.4
else
APDK_VERSION := 5.7.0.4_8
endif
else
ifeq ($(FLASH_SIZE), 4)
ifeq ($(WL_OLD_CODE),y)
APDK_VERSION := 5.5.20
else
APDK_VERSION := 5.7.0.4_4
endif
endif
endif

ifeq ($(CWMP_SUPPORT),1)
CONFIG_CWMP = y
endif

#wwzh add for WAN UPGRADE at 2004-06-16
ifeq ($(CONFIG_WAN_UPGRADE), y)
	CFLAGS += -DWAN_UPGRADE -DWAN_UPGRADE_NOTIFY_INTERVAL=86400
endif

ifeq ($(CONFIG_DNS_ENTRY), y)
	CFLAGS += -DDNS_ENTRY_SUPPORT
endif

export CFLAGS
#Lai add for snmpd 2005-06-09
export FLASH_SIZE
SNMPD = snmpd
#
# Paths
#

# Source bases
export PLATFORM LINUXDIR LIBDIR USRLIBDIR
export TOP := $(shell pwd)
export SRCBASE := $(shell (cd $(TOP)/.. && pwd -P))

#
# Cross-compile environment variables
#

# Build platform
export BUILD := i386-pc-linux-gnu
export HOSTCC := gcc

# uClibc wrapper
ifeq ($(CONFIG_UCLIBC),y)
export PLATFORM := $(PLATFORM)-uclibc
endif

ifeq ($(PLATFORM),mipsel)
#export CROSS_COMPILE := mipsel-linux-
export CROSS_COMPILE := mips_fp_le-
export CONFIGURE := ./configure mipsel-linux --build=$(BUILD)
export TOOLCHAIN := $(shell cd $(dir $(shell which $(CROSS_COMPILE)gcc))/../mipsel-linux && pwd -P)
endif

ifeq ($(PLATFORM),mipsel-uclibc)
export CROSS_COMPILE := mipsel-uclibc-
export CONFIGURE := ./configure mipsel-linux --build=$(BUILD)
export TOOLCHAIN := $(shell cd $(dir $(shell which $(CROSS_COMPILE)gcc))/.. && pwd -P)
endif


export CC := $(CROSS_COMPILE)gcc
export AR := $(CROSS_COMPILE)ar
export AS := $(CROSS_COMPILE)as
export LD := $(CROSS_COMPILE)ld
export NM := $(CROSS_COMPILE)nm
export RANLIB := $(CROSS_COMPILE)ranlib
export STRIP := $(CROSS_COMPILE)strip
export SIZE := $(CROSS_COMPILE)size

#
# Install and target directories
#

export PLATFORMDIR := $(TOP)/$(PLATFORM)
export INSTALLDIR := $(PLATFORMDIR)/install
export TARGETDIR := $(PLATFORMDIR)/target

#
# Configuration
#
ifeq ($(IGMP_PROXY_SUPPORT),1)
CONFIG_IGMPPROXY=y
endif

CONFIG_IPTABLES=y
obj-$(CONFIG_HTTPD) += httpd
obj-$(CONFIG_RC) += rc
obj-$(CONFIG_SHARED) += shared
obj-$(CONFIG_WWW) += www
obj-$(CONFIG_GLIBC) += lib
obj-$(CONFIG_UCLIBC) += lib
#leijun modify 2005-01-04
ifeq ($(WL_OLD_CODE),y)
#obj-$(CONFIG_WLCONF) += wlconf
obj-$(CONFIG_WLCONF) += cm_logic wl_ioctl
else
obj-$(CONFIG_WLCONF) += ti_ap
endif
obj-$(CONFIG_BRIDGE) += bridge
obj-$(CONFIG_BUSYBOX) += busybox
obj-$(CONFIG_DNSMASQ) += dnsmasq
obj-$(CONFIG_IPTABLES) += iptables
obj-$(CONFIG_LIBIPT) += iptables
obj-$(CONFIG_NAS) += nas
obj-$(CONFIG_NETCONF) += netconf
obj-$(CONFIG_NTP) += ntpclient
obj-$(CONFIG_NVRAM) += nvram
obj-$(CONFIG_PPP) += ppp
obj-$(CONFIG_UDHCPD) += udhcpd
#obj-$(CONFIG_UPNP) += upnp
obj-$(CONFIG_UPNP) += upnpd
obj-$(CONFIG_UTILS) += utils
obj-$(CONFIG_ETC) += etc
obj-$(CONFIG_VLAN) += vlan
obj-$(CONFIG_PING) += ping
obj-$(CONFIG_EMAILALERT) += email_alert
obj-$(CONFIG_UPGRADEFIRMWARE) += upgrade_firmware

#obj-$(CONFIG_CPED) += cpe
obj-$(CONFIG_CPED_TR064) += cpe
obj-$(CONFIG_DHCPRELAY) += dhcprelay
obj-$(CONFIG_CWMP) += cwmp
obj-$(CONFIG_CWMP) += gsoap
obj-$(CONFIG_LIBMNG) += libmng
obj-$(CONFIG_IGMPPROXY) += igmp-proxy
obj-$(CONFIG_WL_CALIBRATION_ONBOARD) += ti_hsk

#Lai
#obj-y+=telnetd
#obj-$(CONFIG_L2TP) += rp-l2tp
#obj-y+=pppd-2.4.3

##################################################################
CONFIG_OTHERS=y

ifeq ($(HSIAB_SUPPORT),1)
CONFIG_OPENSSL=y
CONFIG_HTTPD_R=y
CONFIG_HSIAB=y
CONFIG_SSLCLIENT=y
obj-$(CONFIG_HTTPD_R) += httpd_r
obj-$(CONFIG_HSIAB) += hsiab
obj-$(CONFIG_SSLCLIENT) += sslclient
endif
ifeq ($(HTTPS_SUPPORT),1)
CONFIG_OPENSSL=y
endif
ifeq ($(SNMP_SUPPORT),1)
CONFIG_OPENSSL=y
endif
ifeq ($(MDNS_SUPPORT),1)
CONFIG_MDNS=y
obj-$(CONFIG_MDNS) += mDNS
endif
ifeq ($(EMI_TEST),1)
CONFIG_EMITEST=y
CONFIG_WLTEST=y
obj-$(CONFIG_EMITEST) += emi
obj-$(CONFIG_WLTEST) += wltest
endif
ifeq ($(SNMP_SUPPORT),1)
obj-$(CONFIG_SNMP) += snmp
endif
ifeq ($(HEARTBEAT_SUPPORT),1)
obj-$(CONFIG_HEARTBEAT) += bpalogin
endif
ifeq ($(PARENTAL_CONTROL_SUPPORT),1)
#obj-$(CONFIG_PARENTAL_CONTROL) += artemis
obj-y += artemis
endif
ifeq ($(CWMP_SUPPORT),1)
CONFIG_OPENSSL=y
endif


#zhangbin 2004.11.18
ifeq ($(CLI_SUPPORT),1)
# cli module kirby
#obj-$(CONFIG_PARSER) += parser
obj-y += parser
#obj-$(CONFIG_TELNETD) += telnetd
#obj-y += telnetd

#obj-y += cli
endif

#zhangbin 2004.11.18
ifeq ($(CLI_SUPPORT),1)
# cli module kirby
#obj-$(CONFIG_PARSER) += parser
obj-y += parser
#obj-$(CONFIG_TELNETD) += telnetd
#obj-y += telnetd

#obj-y += cli
endif

obj-$(CONFIG_TFTPD) += tftpd
obj-$(CONFIG_CRON) += cron
obj-$(CONFIG_PPTP) += pptp-client
obj-$(CONFIG_PPPD) += pppd
obj-$(CONFIG_ZEBRA) += zebra
obj-$(CONFIG_DDNS) += ipupdate
obj-$(CONFIG_OPENSSL) += openssl
obj-$(CONFIG_OTHERS) += others
#add by peny 

obj-$(CONFIG_SNMPD) += snmpd

obj-$(CONFIG_FREESWAN) += freeswan
obj-$(CONFIG_TC) += iproute2
#obj-y += artemis
obj-$(CONFIG_VCONF) += vconfig

obj-clean := $(foreach obj,$(obj-y) $(obj-n),$(obj)-clean)
obj-install := $(foreach obj,$(obj-y),$(obj)-install)

export CONFIG_OPENSSL

#
# Basic rules
#

all: clean_target $(obj-y) $(LINUXDIR)/.config
#all: clean_target $(obj-y)
        # Also build kernel
	#( . $(LINUXDIR)/.config ; \
	if [ "$${CONFIG_EMBEDDED_RAMDISK}" = "" ] ; then \
	    $(MAKE) -C $(LINUXDIR) zImage ; \
	fi ; \
	if [ "$${CONFIG_MODULES}" = "y" ] ; then \
	    $(MAKE) -C $(LINUXDIR) modules ; \
	fi

clean: $(obj-clean)
	rm -rf $(TARGETDIR)
	rm -rf $(INSTALLDIR)
	rm -f $(PLATFORMDIR)/linux.trx $(PLATFORMDIR)/vmlinuz $(PLATFORMDIR)/target.cramfs
	rm -rf $(PLATFORMDIR)/target.squshfs
	rm -rf $(SRCBASE)/image/target*
	rm -rf $(SRCBASE)/image/upgrade*
	rm -rf $(SRCBASE)/image/rom*
	rm -rf $(SRCBASE)/image/code.bin

clean_target: 
	rm -rf $(TARGETDIR)
	rm -f $(PLATFORMDIR)/linux.trx $(PLATFORMDIR)/vmlinuz $(PLATFORMDIR)/target.cramfs

distclean mrproper: clean
	rm -f .config $(LINUXDIR)/.config

install package: $(filter-out lib-install,$(obj-install)) $(LINUXDIR)/.config
	echo $(obj-y)
	echo $(obj-install)
	sleep 3
        # Install binaries into target directory
	install -d $(TARGETDIR)
	for dir in $(wildcard $(patsubst %,$(INSTALLDIR)/%,$(obj-y))) ; do \
	    (cd $${dir} && tar cpf - .) | (cd $(TARGETDIR) && tar xpf -) \
	done
        # Install (and possibly optimize) C library
	$(MAKE) lib-install
        # Install modules into filesystem
	#( . $(LINUXDIR)/.config ; \
	if [ "$${CONFIG_MODULES}" = "y" ] ; then \
	    $(MAKE) -C $(LINUXDIR) modules_install DEPMOD=/bin/true INSTALL_MOD_PATH=$(TARGETDIR) ; \
	fi
        # Prepare filesystem
	cd $(TARGETDIR) && $(TOP)/misc/rootprep.sh
        # Make sure mkcramfs-2.0 is used
	#$(MAKE) -C $(LINUXDIR)/scripts/cramfs mkcramfs
	cd $(TARGETDIR)  &&  find . -iname "CVS" | xargs rm -rf
	#$(LINUXDIR)/scripts/cramfs/mkcramfs $(TARGETDIR) $(PLATFORMDIR)/target.cramfs
	#$(TARGETDIR)/../mksquashfs $(TARGETDIR) $(PLATFORMDIR)/target.squshfs -noappend
	#chmod 666 $(PLATFORMDIR)/target.squshfs
	#cp $(PLATFORMDIR)/target.squshfs $(SRCBASE)/image/
	$(TARGETDIR)/../mksquashfs $(TARGETDIR)  $(SRCBASE)/image/target.squshfs -noappend
	chmod 666 $(SRCBASE)/image/target.squshfs


#junzhao add 2004.3.26
upgrade:
	$(MAKE) -C $(SRCBASE)/image upgrade
	chmod 666 $(SRCBASE)/image/upgrade_code.bin

rom:
	$(MAKE) -C $(SRCBASE)/image rom

adam2:
	$(MAKE) -C $(SRCBASE)/Adam2/src
adam2-clean:
	$(MAKE) -C $(SRCBASE)/Adam2/src clean
	
pspboot:
	$(MAKE) -C $(SRCBASE)/psp_boot
pspboot-clean:
	$(MAKE) -C $(SRCBASE)/psp_boot clean

linux: symlinks $(LINUXDIR)/.config
	$(MAKE) -C $(LINUXDIR) ram_zimage
	cp $(LINUXDIR)/ram_zimage.bin $(SRCBASE)/image/
linux-clean: symlinks
	$(MAKE) -C $(LINUXDIR) clean
	rm -rf $(SRCBASE)/image/ram_zimage.*
linux-dep: symlinks
	$(MAKE) -C $(LINUXDIR) dep
linux-modules: symlinks
	$(MAKE) -C $(LINUXDIR) modules
	#install -D $(LINUXDIR)/drivers/net/mac/writemac.o $(SRCBASE)/router/lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/writemac.o
ifeq ($(WL_OLD_CODE),y)
	install -D $(LINUXDIR)/arch/mips/mips-boards/ti_avalanche/tiwlan_module_ksyms.o $(SRCBASE)/router/lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/tiwlan_module_ksyms.o
endif

symlinks:
	rm -rf $(LINUXDIR)
	ln -sf $(KERNELDIR) $(LINUXDIR)
	rm -rf $(LINUXDIR)/include/asm
	( cd $(LINUXDIR)/include; ln -sf asm-mips asm; rm -rf cy_codepattern.h; ln -sf $(SRCBASE)/include/cy_codepattern.h cy_codepattern.h)
	
        # Package kernel and filesystem
#	( . $(LINUXDIR)/.config ; \
	if [ "$${CONFIG_EMBEDDED_RAMDISK}" = "y" ] ; then \
	    cp $(PLATFORMDIR)/target.cramfs $(LINUXDIR)/arch/mips/ramdisk/$${CONFIG_EMBEDDED_RAMDISK_IMAGE} ; \
	    $(MAKE) -C $(LINUXDIR) zImage ; \
	else \
	    cp $(LINUXDIR)/arch/mips/brcm-boards/bcm947xx/compressed/vmlinuz $(PLATFORMDIR)/ ; \
	    trx -o $(PLATFORMDIR)/linux.trx $(PLATFORMDIR)/vmlinuz $(PLATFORMDIR)/target.cramfs ; \
	fi )
#REMOVE
        # Pad self-booting Linux to a 64 KB boundary
#	cp $(LINUXDIR)/arch/mips/brcm-boards/bcm947xx/compressed/zImage $(PLATFORMDIR)/
#	dd conv=sync bs=64k < $(PLATFORMDIR)/zImage > $(PLATFORMDIR)/linux.bin
        # Append filesystem to self-booting Linux
#	cat $(PLATFORMDIR)/target.cramfs >> $(PLATFORMDIR)/linux.bin
#endif
#
# Configuration rules
#

conf mconf:
	$(MAKE) -C config
	@./config/$@ ./config/Config
        # Also configure kernel
	$(MAKE) k$@

oldconf: .config
	$(MAKE) -C config
	@./config/conf -o ./config/Config
        # Also configure kernel
	$(MAKE) k$@

kconf:
	$(MAKE) -C $(LINUXDIR) config

kmconf:
	$(MAKE) -C $(LINUXDIR) menuconfig

koldconf: $(LINUXDIR)/.config
	$(MAKE) -C $(LINUXDIR) oldconfig

# Convenience
config: conf

menuconfig: symlinks mconf

oldconfig: oldconf

# Default configurations
.config:
	cp config/defconfig $@
	$(MAKE) oldconfig

$(LINUXDIR)/.config:
ifeq ($(PROJECT), WAG354G)
	cp $(LINUXDIR)/NSP_WAG354G.config $@
else
ifeq ($(PROJECT), WAG354GV2)
	cp $(LINUXDIR)/NSP_WAG354G.config $@
else
ifeq ($(PROJECT), HG200)
	cp $(LINUXDIR)/NSP_WAG354G.config $@
else
ifeq ($(FLASH_SIZE), 4)
	cp $(LINUXDIR)/NSP_WAG54GV2.config $@
else
	cp $(LINUXDIR)/NSP_WAG54GV2_intel.config $@
endif
endif # HG200
endif # WAG354GV2
endif # WAG354G
	$(MAKE) -C $(LINUXDIR) oldconfig

#
# Overrides
#

httpd: nvram shared openssl hsiab
	@echo PLATFORM=$(PLATFORM)
	@echo LINUXDIR=$(LINUXDIR)
	@echo LIBDIR=$(LIBDIR)
	@echo USRLIBDIR=$(USRLIBDIR)
	@echo TOP=$(TOP)
	@echo SRCBASE=$(SRCBASE)
	@echo BUILD=$(BUILD)
	@echo HOSTCC=$(HOSTCC)
	@echo CROSS_COMPILE=$(CROSS_COMPILE)
	@echo TOOLCHAIN=$(TOOLCHAIN)
	sleep 1
	$(MAKE) -C httpd
	#sleep 10

ifeq ($(CWMP_SUPPORT),1)
rc: nvram shared openssl hsiab libmng
else
rc: nvram shared openssl hsiab
endif

www www-%:
	$(MAKE) -C www/$(WEB_PAGE) $* INSTALLDIR=$(INSTALLDIR)/www

bridge:
	$(MAKE) -C bridge brctl/brctl

bridge-install:
	install -D bridge/brctl/brctl $(INSTALLDIR)/bridge/usr/sbin/brctl
	$(STRIP) $(INSTALLDIR)/bridge/usr/sbin/brctl

busybox:
	cd busybox && rm -f Config.h && ln -sf configs/$(CONFIG_BUSYBOX_CONFIG).h Config.h
	$(MAKE) -C busybox STRIPTOOL=$(STRIP)

busybox-install:
	$(MAKE) -C busybox STRIPTOOL=$(STRIP) PREFIX=$(INSTALLDIR)/busybox install

dnsmasq-install:
	install -D dnsmasq/dnsmasq $(INSTALLDIR)/dnsmasq/usr/sbin/dnsmasq
	$(STRIP) $(INSTALLDIR)/dnsmasq/usr/sbin/dnsmasq

iptables:
	$(MAKE) -C iptables BINDIR=/usr/sbin LIBDIR=/usr/lib KERNEL_DIR=$(LINUXDIR)

iptables-install:
ifeq ($(CONFIG_IPTABLES),y)
	install -d $(INSTALLDIR)/iptables/usr/lib/iptables
	install iptables/extensions/*.so $(INSTALLDIR)/iptables/usr/lib/iptables
	$(STRIP) $(INSTALLDIR)/iptables/usr/lib/iptables/*.so
	install -D iptables/iptables $(INSTALLDIR)/iptables/usr/sbin/iptables
	$(STRIP) $(INSTALLDIR)/iptables/usr/sbin/iptables
	install -D iptables/iptables-restore $(INSTALLDIR)/iptables/usr/sbin/iptables-restore
	$(STRIP) $(INSTALLDIR)/iptables/usr/sbin/iptables-restore
else
        # So that generic rule does not take precedence
	@true
endif

iptables-clean:
	-$(MAKE) -C iptables KERNEL_DIR=$(LINUXDIR) distclean


netconf: iptables

ntpclient-install:
	install -D ntpclient/ntpclient $(INSTALLDIR)/ntpclient/usr/sbin/ntpclient
	$(STRIP) $(INSTALLDIR)/ntpclient/usr/sbin/ntpclient

ppp ppp-%:
	$(MAKE) -C ppp/pppoecd $* INSTALLDIR=$(INSTALLDIR)/ppp

udhcpd: shared nvram

udhcpd-install:
	install -D udhcpd/udhcpd $(INSTALLDIR)/udhcpd/usr/sbin/udhcpd
	install -D udhcpd/dumpleases $(INSTALLDIR)/udhcpd/usr/sbin/dumpleases
	$(STRIP) $(INSTALLDIR)/udhcpd/usr/sbin/udhcpd
	$(STRIP) $(INSTALLDIR)/udhcpd/usr/sbin/dumpleases
	cd $(INSTALLDIR)/udhcpd/usr/sbin && ln -sf udhcpd udhcpc

upnp: netconf nvram shared

#add by junzhao
upnpd: 
	$(MAKE) -C upnpd
upnpd-install: 
	install -d $(INSTALLDIR)/upnpd/usr/sbin
	install upnpd/upnpd-igd $(INSTALLDIR)/upnpd/usr/sbin
	install -d $(INSTALLDIR)/upnpd/etc
	rm -rf $(INSTALLDIR)/upnpd/etc/igd
	cp upnpd/igd $(INSTALLDIR)/upnpd/etc/ -a
upnpd-clean: 
	$(MAKE) -C upnpd clean
	
#Lai modify at 2005-06-09
snmpd:
	$(MAKE) -C $(SNMPD) clean
	$(MAKE) -C $(SNMPD) 
snmpd-install: 
	install -d $(INSTALLDIR)/snmpd/usr/share
	install -d $(INSTALLDIR)/snmpd/usr/lib
	install -d $(INSTALLDIR)/snmpd/usr/sbin
	install -d $(INSTALLDIR)/snmpd/usr/etc
	$(STRIP) -s $(SNMPD)/agent/.libs/snmpd 
	install $(SNMPD)/agent/.libs/snmpd $(INSTALLDIR)/snmpd/usr/sbin
	rm -rf  $(INSTALLDIR)/snmpd/usr/lib/*
	rm -rf  $(INSTALLDIR)/snmpd/usr/share
	rm -rf  $(INSTALLDIR)/snmpd/usr/etc/*
	$(STRIP) -s $(SNMPD)/agent/.libs/*.so 
	$(STRIP) -s $(SNMPD)/snmplib/.libs/*.so 
	cp $(SNMPD)/snmplib/.libs/libsnmp-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib
	cp $(SNMPD)/agent/.libs/libucdagent-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib
	cp $(SNMPD)/agent/.libs/libucdmibs-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib
	rm -rf $(SNMPD)/share/snmp/mibs
ifeq ($(FLASH_SIZE), 8)
	cp $(SNMPD)/mibsFor8M/mibs $(SNMPD)/share/snmp/mibs -a
else
ifeq ($(FLASH_SIZE), 4)
	cp $(SNMPD)/mibsFor4M/mibs $(SNMPD)/share/snmp/mibs -a
endif
endif	
	cp -r $(SNMPD)/local/snmpconf.dir/*.conf $(SNMPD)/share/snmp/snmpconf
	cp $(SNMPD)/share $(INSTALLDIR)/snmpd/usr/ -a
	cp $(SNMPD)/rc.d $(INSTALLDIR)/snmpd/usr/etc/ -a
	
	ln -sf libsnmp-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libsnmp.so
	ln -sf libsnmp-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libsnmp.so.0
	ln -sf libsnmp-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libsnmp.so.0.4.2.1
	ln -sf libucdagent-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdagent.so
	ln -sf libucdagent-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdagent.so.0
	ln -sf libucdagent-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdagent.so.0.4.2.1
	ln -sf libucdmibs-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdmibs.so
	ln -sf libucdmibs-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdmibs.so.0
	ln -sf libucdmibs-0.4.2.1.so $(INSTALLDIR)/snmpd/usr/lib/libucdmibs.so.0.4.2.1

snmpd-clean:
	$(MAKE) -C $(SNMPD) clean

wlconf: nvram shared

vlan:
	$(MAKE) -C vlan CROSS=$(CROSS_COMPILE) STRIPTOOL=$(STRIP)

upgrade_firmware:
	$(MAKE) -C upgrade_firmware

upgrade_firmware-install:
	#$(MAKE) -C ping install
	install -D upgrade_firmware/upgrade_firmware $(INSTALLDIR)/upgrade_firmware/sbin/upgrade_firmware

upgrade_firmware-clean:
	$(MAKE) -C upgrade_firmware clean

ping: nvram
	$(MAKE) -C ping

ping-install:
	#$(MAKE) -C ping install
	install -D ping/diagnostic $(INSTALLDIR)/ping/usr/sbin/diagnostic
	install -D ping/diagtool $(INSTALLDIR)/ping/usr/sbin/diagtool
	
ping-clean:
	$(MAKE) -C ping clean

dhcprelay:
	$(MAKE) -C dhcprelay

dhcprelay-install:
	install -d $(INSTALLDIR)/dhcprelay
	install -d $(INSTALLDIR)/dhcprelay/usr/sbin
	install -D dhcprelay/dhcrelay $(INSTALLDIR)/dhcprelay/usr/sbin/dhcrelay

dhcprelay-clean:
	$(MAKE) -C dhcprelay clean

cwmp: nvram shared libmng gsoap

cpe:
	$(MAKE) -C cpe

cpe-install:
	#$(MAKE) -C ping install
	rm -rf $(INSTALLDIR)/cpe/*
	install -d $(INSTALLDIR)/cpe/usr/sbin
	install -D cpe/cped $(INSTALLDIR)/cpe/usr/sbin/cped
	install -d $(INSTALLDIR)/cpe/etc
	install -d $(INSTALLDIR)/cpe/etc/cpe
	cp cpe/igd/* $(INSTALLDIR)/cpe/etc/cpe/ -a
	install -d $(INSTALLDIR)/cpe/usr/lib
	install -m 755 cpe/api/libtr064.so $(INSTALLDIR)/cpe/usr/lib
	$(STRIP) $(INSTALLDIR)/cpe/usr/lib/libtr064.so

cpe-clean:
	$(MAKE) -C cpe clean

igmp-proxy:
	$(MAKE) -C igmp-proxy
igmp-proxy-install:
	install -D igmp-proxy/IGMPProxy $(INSTALLDIR)/igmp-proxy/usr/sbin/IGMPProxy
igmp-proxy-clean:
	$(MAKE) -C igmp-proxy clean

email_alert:
	$(MAKE) -C email_alert

email-install:
	#$(MAKE) -C email_alert install
	install -D email_alert/email_alert $(INSTALLDIR)/email_alert/usr/sbin/email_alert

email_alert-clean:
	$(MAKE) -C email_alert clean
freeswan:
	$(MAKE) -C freeswan programs
freeswan-install:
	#install -D $(INSTALLDIR)/ipsec
	#install -D $(INSTALLDIR)/ipsec/bin
	install -D freeswan/pluto/pluto $(INSTALLDIR)/freeswan/bin/ipsec/pluto
	#add by zhs for netbios broadcast
	install -D freeswan/pluto/nbsrelay $(INSTALLDIR)/freeswan/bin/ipsec/nbsrelay
	install -D freeswan/pluto/_pluto_adns $(INSTALLDIR)/freeswan/bin/ipsec/_pluto_adns
	install -D freeswan/pluto/whack $(INSTALLDIR)/freeswan/bin/ipsec/whack
	install -D freeswan/klips/utils/eroute $(INSTALLDIR)/freeswan/bin/ipsec/eroute
	install -D freeswan/klips/utils/klipsdebug $(INSTALLDIR)/freeswan/bin/ipsec/klipsdebug
	#install -D freeswan/klips/utils/pf_key $(INSTALLDIR)/freeswan/bin/ipsec/pfkey
	install -D freeswan/klips/utils/spi $(INSTALLDIR)/freeswan/bin/ipsec/spi
	install -D freeswan/klips/utils/spigrp $(INSTALLDIR)/freeswan/bin/ipsec/spigrp
	install -D freeswan/klips/utils/tncfg $(INSTALLDIR)/freeswan/bin/ipsec/tncfg
	rm -rf $(INSTALLDIR)/freeswan/bin/ipsec/_updown
	find $(INSTALLDIR)/freeswan -name CVS | xargs rm -rf 
	$(STRIP) -s $(INSTALLDIR)/freeswan/bin/ipsec/*
#add by zhs for quickvpn,4-11-2005	
ifeq ($(NOMAD_SUPPORT),1)
	install -D freeswan/pluto/_updown_qv $(INSTALLDIR)/freeswan/bin/ipsec/_updown
else
	install -D freeswan/pluto/_updown $(INSTALLDIR)/freeswan/bin/ipsec/_updown
endif
freeswan-clean:
	$(MAKE) -C freeswan clean

iproute2:
	$(MAKE) -C iproute2
	
iproute2-install:
	install -D iproute2/tc/tc $(INSTALLDIR)/iproute2/usr/sbin/tc
	install -D iproute2/ip/ip $(INSTALLDIR)/iproute2/usr/sbin/ip
	$(STRIP) $(INSTALLDIR)/iproute2/usr/sbin/tc
	$(STRIP) $(INSTALLDIR)/iproute2/usr/sbin/ip

iproute2-clean:
	$(MAKE) -C iproute2 clean

vconfig:
	$(MAKE) -C vconfig
	
vconfig-install:
	install -D vconfig/vconfig $(INSTALLDIR)/vconfig/usr/sbin/vconfig
	$(STRIP) $(INSTALLDIR)/vconfig/usr/sbin/vconfig

vconfig-clean:
	$(MAKE) -C vconfig clean

#add by leijun
ti_ap:
	$(MAKE) -C ti_ap
	install -D ti_ap/lib/tiap_$(APDK_VERSION).o lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/tiap.o

ti_ap-install:
	install -D ti_ap/wlcfg $(INSTALLDIR)/ti_ap/usr/sbin/wlcfg
	install -D ti_ap/lib/wpa_authenticator_$(APDK_VERSION) $(INSTALLDIR)/ti_ap/usr/sbin/wpa_authenticator
	$(STRIP) $(INSTALLDIR)/ti_ap/usr/sbin/wlcfg
	cd $(INSTALLDIR)/ti_ap/usr/sbin && ln -sf wlcfg antsel

ti_ap-clean:
	$(MAKE) -C ti_ap clean
##########


cm_logic:
	$(MAKE) -C cm_logic
	install -D cm_logic/apdk_$(APDK_VERSION)/tiwlan.o lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/tiwlan.o

cm_logic-install:
	install -D cm_logic/cm_logic $(INSTALLDIR)/cm_logic/usr/sbin/cm_logic
	install -D cm_logic/wlan.sh $(INSTALLDIR)/cm_logic/usr/sbin/wlan.sh
	install -D cm_logic/apdk_$(APDK_VERSION)/user_drv $(INSTALLDIR)/cm_logic/usr/sbin/user_drv
	install -D cm_logic/apdk_$(APDK_VERSION)/ap.bin $(INSTALLDIR)/cm_logic/etc/ap.bin
	$(STRIP) $(INSTALLDIR)/cm_logic/usr/sbin/cm_logic

	#$(MAKE) -C cm_logic CROSS=$(CROSS_COMPILE) STRIPTOOL=$(STRIP) INSTALLDIR=$(INSTALLDIR) install

cm_logic-clean:
	$(MAKE) -C cm_logic clean
	rm -rf lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/*

wl_ioctl:
	$(MAKE) -C wl_ioctl

wl_ioctl-install:
	install -D wl_ioctl/wl_ioctl $(INSTALLDIR)/wl_ioctl/usr/sbin/wl_ioctl
	$(STRIP) $(INSTALLDIR)/wl_ioctl/usr/sbin/wl_ioctl
	#$(MAKE) -C wl_ioctl CROSS=$(CROSS_COMPILE) STRIPTOOL=$(STRIP) INSTALLDIR=$(INSTALLDIR) install

wl_ioctl-clean:
	$(MAKE) -C wl_ioctl clean

vlan-install:
	$(MAKE) -C vlan CROSS=$(CROSS_COMPILE) STRIPTOOL=$(STRIP) INSTALLDIR=$(INSTALLDIR) install

vlan-clean:
	$(MAKE) -C vlan clean

nas-install:
	cd $(INSTALLDIR)/nas/usr/sbin && ln -sf nas nas4not

#kirby
ifeq ($(CLI_SUPPORT),1)
parser:
	$(MAKE) -C parser
parser-install:
	mkdir -p $(INSTALLDIR)/parser/bin
	install -D parser/configurator/configurator $(INSTALLDIR)/parser/bin/configurator
	$(STRIP) -s  $(INSTALLDIR)/parser/bin/configurator

parser-clean:
	$(MAKE) -C parser clean
endif

########################################################################
# Below is added by CT

tftpd-install:
	[ ! -d tftpd ] || install -D tftpd/tftpd $(INSTALLDIR)/tftpd/usr/sbin/tftpd
	$(STRIP) $(INSTALLDIR)/tftpd/usr/sbin/tftpd

cron-install:
	#install -d $(INSTALLDIR)/cron/etc/cron.d
	install -D cron/cron $(INSTALLDIR)/cron/usr/sbin/cron
	$(STRIP) $(INSTALLDIR)/cron/usr/sbin/cron

pptp-client-install:
	install -D pptp-client/pptp $(INSTALLDIR)/pptp-client/usr/sbin/pptp
	$(STRIP) $(INSTALLDIR)/pptp-client/usr/sbin/pptp

pppd:
	$(MAKE) -C pppd/pppd/plugins
	$(MAKE) -C pppd/pppd
	
pppd-clean:
	$(MAKE) -C pppd/pppd clean
	$(MAKE) -C pppd/pppd/plugins clean

pppd-install:
	install -D pppd/pppd/pppd $(INSTALLDIR)/pppd/usr/sbin/pppd
	install -D pppd/pppd/plugins/libpppoatm.so $(INSTALLDIR)/pppd/usr/lib/libpppoatm.so
	install -D pppd/pppd/plugins/pppoe/libpppoe.so $(INSTALLDIR)/pppd/usr/lib/libpppoe.so
	$(STRIP) $(INSTALLDIR)/pppd/usr/sbin/pppd
	$(STRIP) $(INSTALLDIR)/pppd/usr/lib/libpppoatm.so
	$(STRIP) $(INSTALLDIR)/pppd/usr/lib/libpppoe.so
#Lai
pppd-2.4.3:
	$(MAKE) -C pppd-2.4.3/pppd/plugins
	$(MAKE) -C pppd-2.4.3/pppd

pppd-2.4.3-clean:
	$(MAKE) -C pppd-2.4.3/pppd clean
	$(MAKE) -C pppd-2.4.3/pppd/plugins clean

pppd-2.4.3-install:
	install -D pppd-2.4.3/pppd/pppd $(INSTALLDIR)/pppd-2.4.3/usr/sbin/pppd-2.4.3
	install -D pppd-2.4.3/pppd/plugins/pppoatm/pppoatm.so $(INSTALLDIR)/pppd-2.4.3/usr/lib/pppoatm.so
	install -D pppd-2.4.3/pppd/plugins/rp-pppoe/rp-pppoe.so $(INSTALLDIR)/pppd-2.4.3/usr/lib/rp-pppoe.so
	$(STRIP) $(INSTALLDIR)/pppd-2.4.3/usr/sbin/pppd-2.4.3
	$(STRIP) $(INSTALLDIR)/pppd-2.4.3/usr/lib/pppoatm.so
	$(STRIP) $(INSTALLDIR)/pppd-2.4.3/usr/lib/rp-pppoe.so


ipupdate-install:
	install -D ipupdate/ez-ipupdate $(INSTALLDIR)/ipupdate/usr/sbin/ez-ipupdate
	$(STRIP) $(INSTALLDIR)/ipupdate/usr/sbin/ez-ipupdate

zebra:
	$(MAKE) -C zebra CC=$(CC)

zebra-install:
	install -D zebra/zebra/zebra $(INSTALLDIR)/zebra/usr/sbin/zebra
	install -D zebra/ripd/ripd $(INSTALLDIR)/zebra/usr/sbin/ripd
	$(STRIP) $(INSTALLDIR)/zebra/usr/sbin/zebra
	$(STRIP) $(INSTALLDIR)/zebra/usr/sbin/ripd

openssl-install:
	install -D openssl/libcrypto.so $(INSTALLDIR)/openssl/usr/lib/libcrypto.so
	sleep 1
	$(STRIP) $(INSTALLDIR)/openssl/usr/lib/libcrypto.so
	install -D openssl/libssl.so $(INSTALLDIR)/openssl/usr/lib/libssl.so
	$(STRIP) $(INSTALLDIR)/openssl/usr/lib/libssl.so

openssl-clean:
	$(MAKE) -C openssl clean
#Lai
telnetd:
	$(MAKE) -C telnetd

telnetd-install:
	install -D telnetd/busybox $(INSTALLDIR)/telnetd/usr/sbin/telnetd
	$(STRIP) $(INSTALLDIR)/telnetd/usr/sbin/telnetd

rp-l2tp:
	$(MAKE) -C rp-l2tp

rp-l2tp-install:
	install -D rp-l2tp/l2tpd $(INSTALLDIR)/rp-l2tp/usr/sbin/l2tpd
	install -D rp-l2tp/handlers/l2tp-control $(INSTALLDIR)/rp-l2tp/usr/sbin/l2tp-control
	install -D rp-l2tp/handlers/sync-pppd.so $(INSTALLDIR)/rp-l2tp/usr/lib/l2tp/plugins/sync-pppd.so
	install -D rp-l2tp/handlers/cmd.so $(INSTALLDIR)/rp-l2tp/usr/lib/l2tp/plugins/cmd.so
#	install -D tcpdump-3.9.3/tcpdump $(INSTALLDIR)/rp-l2tp/usr/sbin/tcpdump
	$(STRIP) $(INSTALLDIR)/rp-l2tp/usr/sbin/l2tpd


ifeq ($(HSIAB_SUPPORT),1)
httpd_r-install:
	install -D httpd_r/httpd_r $(INSTALLDIR)/httpd_r/usr/sbin/httpd_r
	$(STRIP) $(INSTALLDIR)/httpd_r/usr/sbin/httpd_r

hsiab-install:
	install -D hsiab/libhsiab.so $(INSTALLDIR)/hsiab/usr/lib/libhsiab.so
	install -D hsiab/test/test_register $(INSTALLDIR)/hsiab/usr/sbin/test_register
	install -D hsiab/test/test_hsiab $(INSTALLDIR)/hsiab/usr/sbin/test_hsiab
	$(STRIP) $(INSTALLDIR)/hsiab/usr/lib/libhsiab.so
	$(STRIP) $(INSTALLDIR)/hsiab/usr/sbin/test_register
	$(STRIP) $(INSTALLDIR)/hsiab/usr/sbin/test_hsiab

sslclient: openssl
	$(MAKE) -C sslclient

sslclient-install:
	install -D sslclient/sslclient $(INSTALLDIR)/sslclient/usr/sbin/sslclient
	install -D sslclient/root.pem $(INSTALLDIR)/sslclient/etc/root.pem
	install -D sslclient/client.pem $(INSTALLDIR)/sslclient/etc/client.pem
	$(STRIP) $(INSTALLDIR)/sslclient/usr/sbin/sslclient
endif

ifeq ($(MDNS_SUPPORT),1)
mDNS:
	$(MAKE) -C mDNS/mDNSPosix os=linux

mDNS-clean:
	$(MAKE) -C mDNS/mDNSPosix clean

mDNS-install:
	install -D mDNS/mDNSPosix/build/mDNSResponderPosix $(INSTALLDIR)/mDNS/usr/sbin/mDNSResponderPosix
	$(STRIP) $(INSTALLDIR)/mDNS/usr/sbin/mDNSResponderPosix
endif

ifeq ($(EMI_TEST),1)
emi-install:
	install -d $(INSTALLDIR)/emi/usr/sbin/
	cp -f emi/*_* $(INSTALLDIR)/emi/usr/sbin/

wltest-install:
	install -D wltest/wltest $(INSTALLDIR)/wltest/usr/sbin/wltest
	$(STRIP) $(INSTALLDIR)/wltest/usr/sbin/wltest
endif

ifeq ($(HEARTBEAT_SUPPORT),1)
bpalogin-install:
	install -D bpalogin/bpalogin $(INSTALLDIR)/bpalogin/usr/sbin/bpalogin
	$(STRIP) $(INSTALLDIR)/bpalogin/usr/sbin/bpalogin
endif

ifeq ($(SNMP_SUPPORT),1)
snmp: openssl
	$(MAKE) -C snmp CC=$(CC) SHLIB_LD_CMD="$(AR) cr"

snmp-install:
	install -D snmp/agent/snmpd $(INSTALLDIR)/snmp/usr/sbin/snmpd
	$(STRIP) $(INSTALLDIR)/snmp/usr/sbin/snmpd
	install -D snmp/snmplib/libsnmp.so $(INSTALLDIR)/snmp/usr/lib/libsnmp.so
	$(STRIP) $(INSTALLDIR)/snmp/usr/lib/libsnmp.so
	install -D snmp/apps/snmptrap $(INSTALLDIR)/snmp/usr/sbin/snmptrap
	$(STRIP) $(INSTALLDIR)/snmp/usr/sbin/snmptrap
endif

#ifeq ($(PARENTAL_CONTROL_SUPPORT),1)
artemis: nvram
	[ ! -d artemis ] || $(MAKE) -C artemis deps
	[ ! -d artemis ] || $(MAKE) -C artemis CC=$(CC) AR=$(AR)

artemis-install:
	[ ! -d artemis ] || install -D artemis/LINUX/artemis $(INSTALLDIR)/artemis/usr/sbin/artemis
	[ ! -d artemis ] || install -D artemis/LINUX/artmain $(INSTALLDIR)/artemis/usr/sbin/artmain
	$(STRIP) $(INSTALLDIR)/artemis/usr/sbin/artemis
	$(STRIP) $(INSTALLDIR)/artemis/usr/sbin/artmain
#endif
#
# Generic rules
#

%:
	[ ! -d $* ] || $(MAKE) -C $*

%-clean:
	[ ! -d $* ] || $(MAKE) -C $* clean

%-install:
	[ ! -d $* ] || $(MAKE) -C $* install INSTALLDIR=$(INSTALLDIR)/$*

$(obj-y) $(obj-n) $(obj-clean) $(obj-install): dummy

.PHONY: all clean distclean mrproper install package
.PHONY: conf mconf oldconf kconf kmconf config menuconfig oldconfig
.PHONY: dummy
