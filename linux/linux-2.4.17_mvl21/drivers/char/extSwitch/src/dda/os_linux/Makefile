# file for the Linux MV Switch device driver.

EXTRA_CFLAGS := -I$(TOPDIR)/drivers/char/extSwitch/include -I$(TOPDIR)/drivers/char/extSwitch/src/csl -I$(TOPDIR)/drivers/char/extSwitch/src/dda/os_linux -I$(TOPDIR)/drivers/char/extSwitch/src/ddc -I$(TOPDIR)/drivers/char/extSwitch/src/csl/csl_kendin8995x -I$(TOPDIR)/drivers/char/extSwitch/src/csl/csl_marvell6063 -DLINUX_COMPILATION 
                                                                                                   
ifeq ($(CONFIG_MIPS_AVALANCHE_MARVELL),y)                                                                                                       
O_TARGET := avalanche_marvell.o
obj-$(CONFIG_MIPS_AVALANCHE_MARVELL) := avalanche_marvell6063.o
list-multi      := avalanche_marvell6063.o
                                                                                                                                                                                                              
export-objs :=  DDA_switchLinux.o                                                                                                      
avalanche_marvell-objs := ../../csl/csl_marvell6063/CSL_marvell6063.o ../../csl/CSL_switch.o ../../csl/CSL_mdio.o ../../ddc/DDC_switch.o DDA_switchLinux.o 
                                                                                                       
include $(TOPDIR)/Rules.make
                                                                                                       
avalanche_marvell6063.o:      $(avalanche_marvell-objs)
	$(LD) -r -o $@ $(avalanche_marvell-objs)
endif  
                                                                                                       
ifeq ($(CONFIG_MIPS_AVALANCHE_KENDIN),y)                                                                                                       
O_TARGET := avalanche_kendin8995x.o
obj-$(CONFIG_MIPS_AVALANCHE_KENDIN) := avalanche_kendin8995x.o
list-multi      := avalanche_kendin8995x.o
                                                                                                                                                                                                              
export-objs :=  DDA_switchLinux.o                                                                                                      
avalanche_kendin-objs := ../../csl/csl_kendin8995x/CSL_kendin8995x.o ../../csl/CSL_switch.o ../../csl/CSL_mdio.o ../../ddc/DDC_switch.o DDA_switchLinux.o 
                                                                                                       
include $(TOPDIR)/Rules.make
                                                                                                       
avalanche_kendin8995x.o:      $(avalanche_kendin-objs)
	$(LD) -r -o $@ $(avalanche_kendin-objs)
endif                                                                                                           

clean:
	rm -f core *.o *.a *.s

