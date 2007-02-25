#
# *********************************************************
# *   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
# *********************************************************

# This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
# the contents of this file may not be disclosed to third parties,
# copied or duplicated in any form without the prior written
# permission of CyberTAN Inc.

# This software should be used as a reference only, and it not
# intended for production use!


# THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
#

include $(TOP)/.config
ifneq ($(wildcard $(SRCBASE)/cy_conf.mak),)
  include $(SRCBASE)/cy_conf.mak
endif

ifneq ($(wildcard $(SRCBASE)/cy_codepattern.mak),)
  include $(SRCBASE)/cy_codepattern.mak
endif

all:

install:
	rm -rf $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www
	install -m 644 $(wildcard *.asp) $(wildcard *.jpg) $(wildcard *.gif) $(wildcard *.css) $(wildcard *.js) $(wildcard *.htm) $(wildcard *.xml) $(INSTALLDIR)/www
	cp -a image $(INSTALLDIR)/www

ifneq ($(CONFIG_WLCONF),y)
	rm -rf $(INSTALLDIR)/www/W*
	rm -rf $(INSTALLDIR)/www/Status_Wireless.*
endif
#	cp -a help $(INSTALLDIR)/www

ifeq ($(RELCOUN),FRANCE)
	cp -a -f js/french/*.js $(INSTALLDIR)/www
	cp -a -f js/french/*.asp $(INSTALLDIR)/www
	cp -a -f js/french/*.gif $(INSTALLDIR)/www/image
	mkdir -p $(INSTALLDIR)/www/help
	cp -a -f js/french/help/*.asp $(INSTALLDIR)/www/help
else
ifeq ($(RELCOUN),GERMANY)
	cp -a -f js/german/*.js $(INSTALLDIR)/www
	cp -a -f js/german/*.asp $(INSTALLDIR)/www
	cp -a -f js/german/*.gif $(INSTALLDIR)/www/image
	mkdir -p $(INSTALLDIR)/www/help
	cp -a -f js/german/help/*.asp $(INSTALLDIR)/www/help
else
ifeq ($(RELCOUN),SWEDEN)
	cp -a -f js/sweden/*.js $(INSTALLDIR)/www
	cp -a -f js/sweden/*.asp $(INSTALLDIR)/www
	cp -a -f js/sweden/*.gif $(INSTALLDIR)/www/image
	mkdir -p $(INSTALLDIR)/www/help
	cp -a -f js/sweden/help/*.asp $(INSTALLDIR)/www/help
else
	cp -a -f js/english/*.js $(INSTALLDIR)/www
	cp -a -f js/english/*.asp $(INSTALLDIR)/www
	cp -a -f js/english/*.gif $(INSTALLDIR)/www/image
	mkdir -p $(INSTALLDIR)/www/help
	cp -a -f js/english/help/*.asp $(INSTALLDIR)/www/help
endif
endif
endif

ifneq ($(DHCP_DOMAIN_IP_MAPPING_SUPPORT),1)
	rm -rf $(INSTALLDIR)/www/VOIP.asp
endif

ifneq ($(VPN_SUPPORT),1)
	rm -rf $(INSTALLDIR)/www/ipsec*.asp
endif	

ifeq ($(DDM_SUPPORT),1)
	cp -a verizon $(INSTALLDIR)/www
else
	rm -rf $(INSTALLDIR)/www/verizon
endif

ifneq ($(MULTIPVC_SUPPORT),1)
	rm -rf $(INSTALLDIR)/www/PVC_Selection.asp
endif

ifneq ($(AOL_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/aol.asp
	rm -f $(INSTALLDIR)/www/aol_error.asp
endif

ifneq ($(HSIAB_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/HotSpot_*
	rm -f $(INSTALLDIR)/www/Config.asp
	rm -f $(INSTALLDIR)/www/New_device.asp
	rm -f $(INSTALLDIR)/www/Old_device.asp
	rm -f $(INSTALLDIR)/www/Register*
endif

ifneq ($(HEARTBEAT_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/index_heartbeat.asp
endif

ifneq ($(MPPPOE_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/index_mpppoe.asp
	rm -f $(INSTALLDIR)/www/Status_mpppoe.asp
endif

ifneq ($(CLIP_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/index_clip.asp
endif
ifneq ($(STB_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/Stb_Entry.asp
endif

ifneq ($(PARENTAL_CONTROL_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/Parental_Control.asp
endif

ifneq ($(BACKUP_RESTORE_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/Backup_Restore.asp
endif

ifneq ($(PORT_TRIGGER_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/PortTriggerTable.asp
endif

ifneq ($(UPNP_FORWARD_SUPPORT),1)
	rm -f $(INSTALLDIR)/www/Forward_UPnP.asp
endif

	$(MAKE) -C $(SRCBASE)/router/www/common/ install

webcomp:
	webcomp . list > webrom.c

clean:
