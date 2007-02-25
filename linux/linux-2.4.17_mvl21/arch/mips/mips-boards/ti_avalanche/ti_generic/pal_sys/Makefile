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

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

O_TARGET := pal_sys.o

EXTRA_CFLAGS        += -I$(TOPDIR)/include/asm/avalanche/generic

obj-y += pal_sysMisc.o pal_sysPowerCtrl.o pal_sysResetCtrl.o pal_sysWakeupCtrl.o pal_sysTimer16.o pal_sysWdtimer.o pal_sysCache.o

include $(TOPDIR)/Rules.make
