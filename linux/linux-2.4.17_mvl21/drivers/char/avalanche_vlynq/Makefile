#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := avalanche_vlynq.o

obj-y    +=  vlynq_drv.o  vlynq_hal.o 

EXTRA_CFLAGS := -I$(TOPDIR)/include/asm/avalanche/generic/hal_modules -I$(TOPDIR)/include/asm/avalanche/generic 

include $(TOPDIR)/Rules.make
