# Generated automatically from Makefile.top:Makefile.in by configure.
#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= .
VERSION		= 4.2.1


#
# Paths
#
prefix		= /usr
exec_prefix	= /usr
bindir		= /usr/bin
sbindir		= /usr/sbin
libdir		= /usr/lib
datadir		= /usr/share
includedir	= /usr/include/ucd-snmp/ucd-snmp
mandir		= /usr/man
man1dir		= $(mandir)/man1
man3dir		= $(mandir)/man3
man5dir		= $(mandir)/man5
man8dir		= $(mandir)/man8
snmplibdir	= $(datadir)/snmp
mibdir		= $(snmplibdir)/mibs
persistentdir	= /var/ucd-snmp


#
# Programs
#
INSTALL		= $(LIBTOOL) --mode=install /usr/bin/install -c
SED		= sed
LN_S		= ln -s
#AUTOCONF	= 
#AUTOHEADER	= 
AUTOCONF	= autoconf
AUTOHEADER	= autoheader

#
# Compiler arguments
#
CFLAGS		= -DINET6  -Dlinux
EXTRACPPFLAGS	= -x c
LDFLAGS		=  
LIBTOOL		= $(SHELL) $(top_builddir)/libtool 
EXEEXT		= 

# Misc Compiling Stuff
#CC	        = mipsel-uclibc-gcc
# version number is prefixed by a 0 for a better shared library version number
LIB_LD_CMD      = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) -rpath $(libdir) -release 0.$(VERSION) -o
LIB_EXTENSION   = la
LIB_VERSION     =
LIB_LDCONFIG_CMD = $(LIBTOOL) --mode=finish $(libdir)
LINK		= $(LIBTOOL) --mode=link $(CC)
# RANLIB 	= ranlib
RANLIB		= :

# libtool definitions
.SUFFIXES: .c .o .lo
.c.lo:
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
#
# Makefile.in (at the root of ucd-snmp)
#

top_builddir	= .
TARG		=	bin/snmpget$(EXEEXT) bin/snmpgetnext$(EXEEXT) bin/snmpset$(EXEEXT) 	\
			bin/snmptranslate$(EXEEXT) bin/snmpwalk$(EXEEXT) bin/snmpbulkwalk$(EXEEXT) \
			bin/snmptest$(EXEEXT) bin/snmptrapd$(EXEEXT) bin/snmpnetstat$(EXEEXT) bin/snmpd$(EXEEXT)

#SUBDIRS		= snmplib agent apps local ov man
#INSTALLDIRS	= snmplib agent apps local man mibs
SUBDIRS		= snmplib agent local ov
INSTALLDIRS	= snmplib agent local mibs
TESTDIRS	= testing

#CPP		= mipsel-uclibc-gcc -E 						\
			-I$(srcdir)/agent/mibgroup -I. -I$(srcdir)	\
			-DDONT_INC_STRUCTS -DBINDIR=$(bindir) 		\
			$(EXTRACPPFLAGS)


CPP = $(CC) -E -I$(srcdir)/agent/mibgroup -I. -I$(srcdir) -DDONT_INC_STRUCTS -DBINDIR=$(bindir) $(EXTRACPPFLAGS)

INSTALLHEADERS=ucd-snmp-config.h $(srcdir)/version.h

all:

test:   all
	( cd testing; $(MAKE) test )

sedscript: sedscript.in config.h $(srcdir)/agent/mibgroup/mibdefs.h
	$(CPP) -DPREFIX=$(prefix) -DLIBDIR=$(libdir) -DDATADIR=$(datadir) $(srcdir)/sedscript.in | egrep '^s[/#]' | sed 's/REMOVEME//g;s# */#/#g;s/ *#/#/g;s#/ *#/#g;s/# g/#g/;' > sedscript


ucd-snmp-config.h: config.h
	@egrep -v "IN_UCD_SNMP_SOURCE|SYSTEM_INCLUDE|MACHINE_INCLUDE|mib_module_config" config.h > ucd-snmp-config.h

subdirs:
	for i in $(SUBDIRS) ; do	\
        ( cd $$i ; $(MAKE) ) ;		\
	done


test: all testdirs

testdirs:
	for i in $(TESTDIRS) ; do	\
        ( cd $$i ; $(MAKE) ) ;		\
	done


distall: ${srcdir}/configure ${srcdir}/config.h 


install:    all installdirs
	for i in $(INSTALLDIRS) ; do	\
        ( cd $$i ; $(MAKE) install ) ;	\
	done
	@for i in $(INSTALLHEADERS) ; do \
		$(INSTALL) $$i $(includedir) ; \
		echo "install:  installed $$i in $(includedir)";\
	done

installdirs:
	@$(SHELL) $(srcdir)/mkinstalldirs $(snmplibdir) $(mibdir) $(includedir)
	@-$(SHELL) $(srcdir)/mkinstalldirs $(persistentdir)

apps/snmpget$(EXEEXT) apps/snmpgetnext$(EXEEXT) apps/snmpset$(EXEEXT) apps/snmptranslate$(EXEEXT) apps/snmpwalk$(EXEEXT) apps/snmpbulkwalk$(EXEEXT) apps/snmptest$(EXEEXT) apps/snmptrapd$(EXEEXT) apps/snmpnetstat/snmpnetstat$(EXEEXT) agent/snmpd$(EXEEXT): makeall


depend:
	cd snmplib; $(MAKE) depend
#	cd apps; $(MAKE) depend
	cd agent; $(MAKE) depend
	cd testing; $(MAKE) depend

nosysdepend:
	cd snmplib; $(MAKE) nosysdepend
#	cd apps; $(MAKE) nosysdepend
	cd agent; $(MAKE) nosysdepend
	cd testing; $(MAKE) nosysdepend

makefileindepend:
	cd snmplib; $(MAKE) makefileindepend
#	cd apps; $(MAKE) makefileindepend
	cd agent; $(MAKE) makefileindepend
	cd testing; $(MAKE) makefileindepend


clean:

distclean: clean configclean

configclean:
	rm -f config.cache config.status config.log config.h
	rm -f Makefile snmplib/Makefile				\
		agent/Makefile agent/mibgroup/Makefile		\
		apps/Makefile  apps/snmpnetstat/Makefile	\
		man/Makefile mibs/Makefile ov/Makefile		\
		local/Makefile testing/Makefile
	rm -f mibs/.index
	rm -f mib_module_config.h ucd-snmp-config.h 	\
		agent/mibgroup/mib_module_includes.h 	\
		agent/mibgroup/mib_module_inits.h 	\
		agent/mibgroup/mib_module_shutdown.h 	\
		agent/mibgroup/mib_module_dot_conf.h
	rm -f *.core


$(srcdir)/configure: configure.in aclocal.m4
	cd ${srcdir} && $(AUTOCONF)
	echo "Please run configure now."
	sh -c exit 2


# autoheader might not change config.h.in, so touch a stamp file.
#
$(srcdir)/config.h.in: stamp-h.in
$(srcdir)/stamp-h.in: configure.in acconfig.h
	cd ${srcdir} && LC_COLLATE=C $(AUTOHEADER)
	echo timestamp > ${srcdir}/stamp-h.in

config.h: stamp-h
stamp-h: config.h.in config.status
	CONFIG_FILES= ./config.status
	echo timestamp > stamp-h

touchit:
	touch configure config.h.in
	touch config.status
	touch stamp-h stamp-h.in

Makefile:

config.status: configure
	./config.status --recheck

EXAMPLE.conf: sedscript EXAMPLE.conf.def
	$(SED) -f sedscript $(srcdir)/EXAMPLE.conf.def > EXAMPLE.conf

TAGS:
	find $(srcdir) -name '*.[ch]' -print | etags -

version:
	@if test "x$(VERSION)" = "x"; then \
	  echo "you need to supply a VERSION string."; \
	  exit 2; \
	fi
	agent/mibgroup/versiontag $(VERSION) reverse

tag:
	agent/mibgroup/versiontag $(VERSION) tag

tar:
	agent/mibgroup/versiontag $(VERSION) tar

dist: version tag tar

FAQ.html:
	local/FAQ2HTML FAQ
