# File: drivers/char/avalanche_led/Makefile
#
# Makefile for the Linux LED device driver.
#


O_TARGET := avalanche_led.o
obj-m := avalanche_led.o
list-multi      := avalanche_led.o

EXTRA_CFLAGS := -I$(TOPDIR)/include/asm/avalanche/generic

export-objs := led_drv.o led_wrapper.o

avalanche_led-objs := led_hal.o led_drv.o led_wrapper.o

include $(TOPDIR)/Rules.make

avalanche_led.o:      $(avalanche_led-objs)
	$(LD) -r -o $@ $(avalanche_led-objs)

clean:
	rm -f core *.o *.a *.s
