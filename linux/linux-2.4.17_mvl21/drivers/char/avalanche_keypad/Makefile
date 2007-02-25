# File: drivers/char/avalanche_keypad/Makefile
#
# Makefile for the Linux keypad driver.
#

EXTRA_CFLAGS := -I$(TOPDIR)/include/asm/avalanche/titan -I$(TOPDIR)/include/asm/avalanche/generic/hal_modules

O_TARGET := avalanche_keypad.o
obj-$(CONFIG_MIPS_AVALANCHE_KEYPAD) += avalanche_keypad.o
list-multi      := avalanche_keypad.o



avalanche_keypad-objs := keypad_drv.o keypad_hal.o

include $(TOPDIR)/Rules.make

avalanche_keypad.o:      $(avalanche_keypad-objs)
	$(LD) -r -o $@ $(avalanche_keypad-objs)

clean:
	rm -f core *.o *.a *.s
