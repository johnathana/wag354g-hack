ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

install: 
	install -d $(INSTALLDIR)/etc
	install -d $(INSTALLDIR)/sbin
	install -d $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/lib

	install services $(INSTALLDIR)/etc
ifeq ($(WL_OLD_CODE),y)
	install led.conf $(INSTALLDIR)/etc/led.conf
else
	install led_new.conf $(INSTALLDIR)/etc/led.conf
endif

ifeq ($(CONFIG_CLI),y)
	install group $(INSTALLDIR)/etc
endif
	ln -sf /tmp/resolv.conf $(INSTALLDIR)/etc/resolv.conf
	ln -sf /tmp/hosts $(INSTALLDIR)/etc/hosts
	ln -sf /tmp/cron.d $(INSTALLDIR)/etc/cron.d
ifeq ($(CONFIG_CLI),y)
#kirby 2004/12.23
	ln -sf /tmp/passwd $(INSTALLDIR)/etc/passwd
endif

ifeq ($(SYSLOG_SUPPORT),1)
	install -m 755 rotatelog.sh $(INSTALLDIR)/usr/sbin
endif

clean:

