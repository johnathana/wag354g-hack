#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := adam2.o

obj-$(CONFIG_MIPS_AVALANCHE_ADAM2)    += adam2_proc.o 
obj-$(CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL)    += adam2_env.o setenv.o unsetenv.o getenv.o getienv.o fixenv.o

include $(TOPDIR)/Rules.make

.S.s:
	$(CPP) $(CFLAGS) $< -o $*.s
.S.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

clean:
	rm -f core *.o *.a *.s
