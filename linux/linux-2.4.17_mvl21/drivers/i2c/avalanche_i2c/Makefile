# Makefile for the Avalanche I2C  device driver.
#


EXTRA_CFLAGS := -DEXPORT_SYMTAB 

O_TARGET := avalanche_i2c.o
obj-$(CONFIG_MIPS_AVALANCHE_I2C) += avalanche_i2c.o
list-multi      := avalanche_i2c.o


avalanche_i2c-objs :=  i2c-adap.o i2c-algo.o

include $(TOPDIR)/Rules.make

avalanche_i2c.o:      $(avalanche_i2c-objs)
	$(LD) -r -o $@ $(avalanche_i2c-objs)

clean:
	rm -f core *.o *.a *.s
