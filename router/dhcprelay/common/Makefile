SRC    = raw.c parse.c icmp.c dispatch.c conflex.c socket.c \
       	 packet.c memory.c print.c options.c inet.c convert.c \
	 tree.c tables.c hash.c alloc.c errwarn.c inet_addr.c dlpi.c \
	 tr.c ethernet.c
OBJ    = raw.o parse.o icmp.o dispatch.o conflex.o socket.o \
	 packet.o memory.o print.o options.o inet.o convert.o \
	 tree.o tables.o hash.o alloc.o errwarn.o inet_addr.o dlpi.o \
	 tr.o ethernet.o

DEBUG  = -g

#zhangbin 2005.5.11
#ifdef CLOUD_SUPPORT
#INCLUDES += -I.. -I../includes -DCLOUD_SUPPORT
#else
INCLUDES += -I.. -I../includes 
#endif

CFLAGS += $(PREDEFINES) $(INCLUDES) $(COPTS)

all:	libdhcp.a

libdhcp.a:	$(OBJ)
	rm -f libdhcp.a
	#$(AR) -cruv libdhcp.a $(OBJ)
	#$(RANLIB) libdhcp.a
	mipsel-uclibc-ar -cruv libdhcp.a $(OBJ)
	mipsel-uclibc-ranlib libdhcp.a

clean:
	-rm -f $(OBJ) libdhcp.a
	
# Dependencies (semi-automatically-generated)

raw.o: raw.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
parse.o: parse.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h \
 ../includes/dhcp.h ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h ../includes/dhctoken.h
nit.o: nit.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
icmp.o: icmp.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h ../includes/netinet/ip.h \
 ../includes/netinet/ip_icmp.h
dispatch.o: dispatch.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h \
 conflex.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h ../includes/dhctoken.h
upf.o: upf.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
bpf.o: bpf.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h ../includes/netinet/ip.h ../includes/netinet/udp.h \
 ../includes/netinet/if_ether.h
socket.o: socket.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
lpf.o: lpf.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
packet.o: packet.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h ../includes/netinet/ip.h \
 ../includes/netinet/udp.h ../includes/netinet/if_ether.h
memory.o: memory.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
print.o: print.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
options.o: options.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
inet.o: inet.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
convert.o: convert.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
tree.o: tree.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
tables.o: tables.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
hash.o: hash.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
alloc.o: alloc.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
errwarn.o: errwarn.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
inet_addr.o: inet_addr.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
dlpi.o: dlpi.c ../includes/dhcpd.h \
 ../includes/cdefs.h ../includes/osdep.h ../includes/site.h \
 ../includes/cf/netbsd.h ../includes/dhcp.h \
 ../includes/tree.h ../includes/hash.h ../includes/inet.h \
 ../includes/sysconf.h
