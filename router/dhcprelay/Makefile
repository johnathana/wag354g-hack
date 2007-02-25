
ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CC = mipsel-uclibc-gcc
SRCS   = dhcrelay.c
OBJS   = dhcrelay.o
PROG   = dhcrelay

DEBUG  = -g

#zhangbin 2005.5.11
#ifdef CLOUD_SUPPORT
#INCLUDES = -I. -Iincludes -DCLOUD_SUPPORT
#else
INCLUDES = -I. -Iincludes 
#endif

#DHCPLIB = common/libdhcp.a
DHCPLIB = libdhcp.a
CFLAGS += $(DEBUG) $(PREDEFINES) $(INCLUDES) $(COPTS)


all: 	$(DHCPLIB) $(PROG)
$(DHCPLIB):
	make -C common CC=mipsel-uclibc-gcc
 
clean:
	rm -f $(OBJS) dhclient.o
	rm -f $(PROG) $(CATMANPAGES) $(SEDMANPAGES) *~ #*
	rm -rf *.o 
	rm -rf common/*.a
	
dhcrelay:	$(DHCPLIB) dhcrelay.o
	$(CC) $(LDFLAGS) -o $(PROG) dhcrelay.o common/$(DHCPLIB) $(LIBS)
	mipsel-uclibc-strip -s dhcrelay
# Dependencies (semi-automatically-generated)

dhcrelay.o:  dhcrelay.c includes/dhcpd.h \
 includes/cdefs.h includes/osdep.h includes/site.h \
 includes/cf/netbsd.h includes/dhcp.h \
 includes/tree.h includes/hash.h includes/inet.h \
 includes/sysconf.h includes/version.h
