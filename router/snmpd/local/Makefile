# Generated automatically from Makefile.in by configure.
#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= ..
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
top_builddir=..

SNMPCONFPATH=/var/

SCRIPTS=snmpcheck tkmib mib2c snmpconf
PERLSCRIPTS=snmpcheck tkmib fixproc ipf-mod.pl rsnmp
SCRIPTSMADEFORPERL=snmpcheck.made tkmib.made mib2c.made fixproc.made \
	ipf-mod.pl.made rsnmp.made snmpconf.made
PERLPROG=/usr/bin/perl
DATASRCS=mib2c.conf mib2c.storage.conf mib2c.vartypes.conf
CONFINSTALLDIR=$(snmplibdir)/snmpconf
CONFDATASRC=snmpconf.dir
CONFDIRS=snmpd.conf snmp.conf snmptrapd.conf
CONFFILES=snmpd.conf/system snmpd.conf/acl snmpd.conf/trapsinks \
	snmpd.conf/monitor snmpd.conf/extending snmpd.conf/operation\
	snmpd.conf/basic_setup \
	snmp.conf/authopts snmp.conf/debugging snmp.conf/output snmp.conf/mibs \
	snmptrapd.conf/formatting snmptrapd.conf/traphandle

all: $(SCRIPTSMADEFORPERL)

snmpcheck: $(srcdir)/snmpcheck.def ../sedscript
	$(SED) -f ../sedscript $(srcdir)/snmpcheck.def > snmpcheck

clean:
	rm -f snmpcheck $(SCRIPTSMADEFORPERL)

snmpcheck.made: snmpcheck
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@' snmpcheck > snmpcheck.made; \
	else \
	  touch snmpcheck.made; \
        fi

tkmib.made: $(srcdir)/tkmib
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@' ${srcdir}/tkmib > tkmib.made; \
	else \
	  touch tkmib.made; \
        fi

mib2c.made: $(srcdir)/mib2c
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@;s@/usr/local/share/snmp@$(snmplibdir)@;' ${srcdir}/mib2c > mib2c.made; \
	else \
	  touch mib2c.made; \
        fi


ipf-mod.pl.made: $(srcdir)/ipf-mod.pl
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@' ${srcdir}/ipf-mod.pl > ipf-mod.pl.made; \
	else \
	  touch ipf-mod.pl.made; \
        fi

fixproc.made: $(srcdir)/fixproc
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@' ${srcdir}/fixproc > fixproc.made; \
	else \
	  touch fixproc.made; \
        fi

rsnmp.made: $(srcdir)/rsnmp
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@; s@/usr/local/share@$(datadir)@g;' ${srcdir}/rsnmp > rsnmp.made; \
	else \
	  touch rsnmp.made; \
        fi

snmpconf.made: $(srcdir)/snmpconf
	if test "x$(PERLPROG)" != "x"; then \
	  $(PERLPROG) -p -e 's@^#!.*/perl.*@#!$(PERLPROG)@; s@/usr/local/share@$(datadir)@g; s@/usr/local/etc/snmp@$(SNMPCONFPATH)@g' ${srcdir}/snmpconf > snmpconf.made; \
	else \
	  touch snmpconf.made; \
        fi


install: all
	@if test "x$(PERLPROG)" != "x"; then \
	  for i in $(SCRIPTS) ; do $(INSTALL) $$i.made $(bindir)/$$i ; echo "install:  installed $$i in $(bindir)" ; done; \
	  for i in $(DATASRCS) ; do $(INSTALL) $(srcdir)/$$i $(snmplibdir)/$$i ; echo "install:  installed $$i in $(snmplibdir)" ; done; \
	  for i in $(CONFDIRS); do $(SHELL) $(srcdir)/../mkinstalldirs $(CONFINSTALLDIR)/$$i; done; \
	  for i in $(CONFFILES); do $(INSTALL) $(srcdir)/$(CONFDATASRC)/$$i $(CONFINSTALLDIR)/$$i; echo "install:  installed $$i in $(CONFINSTALLDIR)"; done \
	fi
