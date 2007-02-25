
ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -I$(SRCBASE)/router/rc -Wall -I$(SRCBASE)/
CFLAGS	+= -s -O2
LDFLAGS	+= -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram -L$(TOP)/shared -lshared

#OBJS := misc.o recvfrom.o tftpd.o tftpsubs.o xmalloc.o xstrdup.o bsdsignal.o remap.o upgrade.o ledapp.o
OBJS := misc.o recvfrom.o tftpd.o tftpsubs.o xmalloc.o xstrdup.o bsdsignal.o remap.o upgrade.o

ifeq ($(WL_OLD_CODE),y)
OBJS += ledapp.o
else
OBJS += ledapp_new.o
endif

OBJS += $(TOP)/rc/writeimage.o  $(TOP)/rc/mtd.o $(TOP)/rc/crc.o $(TOP)/rc/http.o 


all:

clean:

install: all


tftpd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


$(OBJS): $(CY_DEPS)
