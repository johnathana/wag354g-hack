# File: drivers/net/avalanche_vmac/Makefile
#
# Makefile for the Linux network (VMAC) device drivers.
#


O_TARGET := avalanche_vmac.o
obj-$(CONFIG_MIPS_AVALANCHE_VMAC) += avalanche_vmac.o
list-multi      := avalanche_vmac.o

#EXTRA_CFLAGS += -DAVALANCHE_VMAC_TASKLET_MODE


ifeq ($(CONFIG_MIPS_WA1130),y)
avalanche_vmac-objs := marvell_mdio.o marvell_vmac.o marvell.o wlanCountersApi.o
else
avalanche_vmac-objs := emacmdio.o vmac.o
endif

include $(TOPDIR)/Rules.make

avalanche_vmac.o:      $(avalanche_vmac-objs)
	$(LD) -r -o $@ $(avalanche_vmac-objs)

clean:
	rm -f core *.o *.a *.s
