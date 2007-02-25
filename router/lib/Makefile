#
# C library Makefile
# 
# Copyright 2001-2003, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.2 2005/03/28 06:57:53 sparq Exp $
#
include $(TOP)/.config
ifneq ($(wildcard ../../cy_conf.mak),)
  include ../../cy_conf.mak
endif
ifneq ($(wildcard ../../cy_codepattern.mak),)
  include ../../cy_codepattern.mak
endif

# Extra libraries
lib-$(CONFIG_LIBCRYPT) += libcrypt
lib-$(CONFIG_LIBDL) += libdl
lib-$(CONFIG_LIBM) += libm
lib-$(CONFIG_LIBNSL) += libnsl
lib-$(CONFIG_LIBPTHREAD) += libpthread
lib-$(CONFIG_LIBRESOLV) += libresolv
lib-$(CONFIG_LIBUTIL) += libutil
#junzhao add
lib-$(CONFIG_LIBATM) += libatm

ifeq ($(CONFIG_UCLIBC),y)

# uClibc names
LINKER := ld-uClibc.so.0
LIBC := libc.so.0
MAP := uClibc.map

else

# GNU libc names
LINKER := $(foreach lib,ld.so.* ld-linux.so.*,$(notdir $(wildcard $(LIBDIR)/$(lib))))
LIBC := libc.so.6
MAP := libc.map

endif

# Nothing to do until installation
all:

ifeq ($(CONFIG_LIBOPT),y)

# Find all ELF binaries (including extra libraries)
BINARIES := $(shell find $(TARGETDIR) -path $(TARGETDIR)/lib -prune -o -type f -print | file -f - | grep ELF | cut -d':' -f1)
BINARIES += $(wildcard $(foreach lib,$(LINKER) $(lib-y),$(LIBDIR)/$(lib) $(LIBDIR)/$(lib).so.?))

ifneq ($(wildcard $(LIBDIR)/optinfo),)
# Try the toolchain directory first for precursors
OPTINFO := $(LIBDIR)/optinfo
else
# Fall back on platform directory
OPTINFO := $(PLATFORM)
endif

ifeq ($(CONFIG_UCLIBC),y)

# uClibc precursors
OBJS := $(wildcard $(foreach file,libgcc-need.a libc.a interp.o,$(OPTINFO)/$(file)))

# uClibc link parameters
ENTRY := __uClibc_init
LDFLAGS := -s -shared --warn-common --warn-once -z combreloc  -soname=$(LIBC) -init $(ENTRY)
LIBGCC := $(shell $(CC) -print-libgcc-file-name)

$(MAP): $(OPTINFO)/libc.a
	$(NM) -o --defined-only --no-sort $< | cut -d' ' -f3 > $@

# Find all unresolved symbols that uClibc could resolve
.symbols: $(MAP) $(BINARIES)
	rm -f $@
	for symbol in `$(NM) --dynamic -u --no-sort $(BINARIES) | sort -u` ; do \
	    grep -q "^$${symbol}$$" $(MAP) && echo "-u $${symbol}" >> $@ ; \
	    true ; \
	done

# Re-link uClibc
$(LIBC): .symbols $(OBJS)
	xargs -t $(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBGCC) < .symbols
else

# GNU libc precursors
OBJS := $(wildcard $(foreach file,soinit.os libc_pic.a sofini.os,$(OPTINFO)/$(file)))

# GNU libc link parameters
ENTRY := __libc_main
CFLAGS := -shared -Wl,-O1 -Wl,-dynamic-linker=$(LINKER) -Wl,--version-script=$(MAP) -Wl,-soname=$(LIBC) -nostdlib -nostartfiles -e $(ENTRY)

$(MAP): $(OPTINFO)/libc.map
	cp $< $@

# Find all unresolved symbols that GNU libc could resolve
.symbols: $(MAP) $(BINARIES)
	rm -f $@
	for symbol in `$(NM) --dynamic -u --no-sort $(BINARIES) | sort -u` ; do \
	    grep -q "$${symbol};" $(MAP) && echo "-u $${symbol}" >> $@ ; \
	done

# Re-link GNU libc
$(LIBC): .symbols $(OBJS) $(LIBDIR)/$(LINKER)
	xargs $(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIR)/$(LINKER) -lgcc < .symbols

endif

# Install optimized C library
install: $(LIBC)

else

# Install default C library
install: $(LIBDIR)/$(LIBC)

endif

#modify by junzhao 2004.3.9
# Also install extra libraries
install: $(wildcard $(foreach lib,$(LINKER) $(lib-y),$(LIBDIR)/$(lib) $(LIBDIR)/$(lib).so.?))
	install -d $(TARGETDIR)/lib
	install $^ $(TARGETDIR)/lib/
	install $(LIBDIR)/libuClibc-0.9.19.so $(TARGETDIR)/lib/libc.so.0
	install libm.so.0 $(TARGETDIR)/lib/libm.so.0
	install libatm.so.1 $(TARGETDIR)/lib/libatm.so.1
ifeq ($(CONFIG_CLIP),y)
	install atmarp $(TARGETDIR)/usr/sbin/atmarp
	install atmarpd $(TARGETDIR)/usr/sbin/atmarpd
endif
	$(STRIP) $(TARGETDIR)/lib/*.so.*
	rm -rf $(TARGETDIR)/lib/modules
	cp -a modules $(TARGETDIR)/lib/

#junzhao 2004.10.29 
ifeq ($(ADSL_TYPE),AnnexA)
	cp -a adslfw/annexa/ar0700xx.bin $(TARGETDIR)/lib/modules
else
ifeq ($(ADSL_TYPE),AnnexB)
	cp -a adslfw/annexb/ar0700xx.bin $(TARGETDIR)/lib/modules
endif
endif

ifneq ($(CONFIG_WLCONF),y)
	rm -rf $(TARGETDIR)/lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/tiwlan.o
	rm -rf $(TARGETDIR)/lib/modules/2.4.17_mvl21-malta-mips_fp_le/kernel/drivers/net/tiwlan_module_ksyms.o
endif

clean:
	rm -f $(LIBC) $(MAP) .symbols
