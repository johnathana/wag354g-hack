# **************************************************************
# Makefile for building the IGMP Proxy module.
#	Build Targets 
#		all 	- Builds the IGMP Proxy executable. IGMPProxy
#		clean 	- Cleans all the object files. 
# **************************************************************

# Compiler. 
# CC = gcc

# Linker. 
# LD = ld

all:

# List of all object modules that make up the IGMPProxy.
OBJECTS = igmp_proxy.o listlib.o

igmp-proxy: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o IGMPProxy
		mips_fp_le-strip -s IGMPProxy	

igmp_proxy.o: igmp_proxy.c
	$(CC) $(CFLAGS) -c igmp_proxy.c

listlib.o: listlib.c
	$(CC) $(CFLAGS) -c listlib.c

clean:
