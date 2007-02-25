#
# Carsten Langgaard, carstenl@mips.com
# Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
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
# Makefile for the MIPS Atlas specific kernel interface routines
# under Linux.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

#EXTRA_CFLAGS += -DEXPORT_SYMTAB

#.S.s:
#	$(CPP) $(CFLAGS) $< -o $*.s
#.S.o:
#	$(CC) $(CFLAGS) -c $< -o $*.o

#all: ti_avalanche.o

O_TARGET := ti_avalanche.o

subdir-y += ti_generic
obj-y += ti_generic/ti_generic.o

subdir-$(CONFIG_MIPS_SANGAM) += ti_sangam
obj-$(CONFIG_MIPS_SANGAM)    += ti_sangam/ti_sangam.o

subdir-$(CONFIG_MIPS_TITAN) += ti_titan
obj-$(CONFIG_MIPS_TITAN)    += ti_titan/ti_titan.o


subdir-$(CONFIG_MIPS_PUMAS) += ti_puma-s
obj-$(CONFIG_MIPS_PUMAS)    += ti_puma-s/ti_puma-s.o

subdir-$(CONFIG_MIPS_AVALANCHE_ADAM2) += ti_adam2
obj-$(CONFIG_MIPS_AVALANCHE_ADAM2)    += ti_adam2/ti_adam2.o

#subdir-$(CONFIG_MIPS_AVALANCHE_PSPBOOT) += ti_pspboot
#obj-$(CONFIG_MIPS_AVALANCHE_PSPBOOT)    += ti_pspboot/ti_pspboot.o 

subdir-$(CONFIG_MIPS_AVALANCHE_PSPBOOT) += ti_pspboot ti_adam2
obj-$(CONFIG_MIPS_AVALANCHE_PSPBOOT)    += ti_pspboot/ti_pspboot.o ti_adam2/ti_adam2.o

include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s
