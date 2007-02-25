# File: drivers/char/avalanche_lcd/Makefile
#
# Makefile for the Linux lcd driver.
#


EXTRA_CFLAGS := -I$(TOPDIR)/include/asm/avalanche/titan -I$(TOPDIR)/include/asm/avalanche/generic/hal_modules

O_TARGET := avalanche_lcd.o
obj-$(CONFIG_MIPS_AVALANCHE_LCD) += avalanche_lcd.o
list-multi      := avalanche_lcd.o



avalanche_lcd-objs := lcd_drv.o  lidd_hal.o  hd44780_hal.o

include $(TOPDIR)/Rules.make

avalanche_lcd.o:      $(avalanche_lcd-objs)
	$(LD) -r -o $@ $(avalanche_lcd-objs)

clean:
	rm -f core *.o *.a *.s
