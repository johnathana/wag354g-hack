#
# Carsten Langgaard, carstenl@mips.com
# Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
#
# ########################################################################
#
# This program is free software; you can distribute it and/or modify it
# under the terms of the GNU General Public License (Version 2) as
# published by the Free Software Foundation.
# 
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
#
# #######################################################################
#
# Makefile for the MIPS Malta specific kernel interface routines
# under Linux.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

ifneq ($(wildcard $(TOPDIR)/../../cy_codepattern.mak),)
  include $(TOPDIR)/../../cy_codepattern.mak
endif

EXTRA_CFLAGS += -DAR7WRD -I$(TOPDIR)/../../include

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

O_TARGET := ti_pspboot.o

obj-y := env.o  platform.o

obj-$(CONFIG_MIPS_TNETV1050SDB) += flashop_ic3.o

ifeq ($(FLASHTYPE), AMD)
obj-$(CONFIG_MIPS_SANGAM)       += flashop_amd_16.o
endif
ifeq ($(FLASHTYPE), INTELJ3)
obj-$(CONFIG_MIPS_SANGAM)       += flashop_intel640_.o
endif
ifeq ($(FLASHTYPE), INTELC3)
obj-$(CONFIG_MIPS_SANGAM)       += flashop_intel640C3_.o
endif

#obj-$(CONFIG_MIPS_SANGAM)       += flashop_amd_16.o
#obj-y				+= flashop_intel640_.o
#obj-y				+= flashop_intel640C3_.o

obj-$(CONFIG_MIPS_WA1130) += flashop_is1050_16.o


include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s
