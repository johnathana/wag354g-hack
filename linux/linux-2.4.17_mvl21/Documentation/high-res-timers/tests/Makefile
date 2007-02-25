#
# Copyright (C) 1997 by the University of Kansas Center for Research,
# Inc.  This software was developed by the Information and
# Telecommunication Technology Center (ITTC) at the University of
# Kansas.  Partial funding for this project was provided by Sprint. This
# software may be used and distributed according to the terms of the GNU
# Public License, incorporated herein by reference.  Neither ITTC nor
# Sprint accept any liability whatsoever for this product.
#
# This project was developed under the direction of Dr. Douglas Niehaus.
#
# Authors: Shyam Pather, Balaji Srinivasan 
#
# Please send bug-reports/suggestions/comments to posix@ittc.ukans.edu
#
# Further details about this project can be obtained at
#    http://hegel.ittc.ukans.edu/projects/posix/
#

# Makefile for POSIX.4 test/example programs.
# 
# Targets: 
#         time_tests   --> POSIX.4 interval timers and time_query tests

OS = $(shell uname)
CC = mips_fp_le-gcc
POSIX_LIBDIR = ../lib
POSIX_INCDIR = ../lib

# to pick up the modified files, with out putting them in /usr/include
USE_INCDIR = ../usr_incl

CFLAGS = -g -Wall -ggdb
ifeq ($(OS),Linux)
CPPFLAGS = -D_POSIX_TIMERS=1 -I$(POSIX_INCDIR) -I$(USE_INCDIR) 
LDFLAGS = -L$(POSIX_LIBDIR)
LDLIBS = -lposixtime
endif
SOURCES = 	2timer_test.c \
		timer_test.c \
		jitter_test.c \
		clock_getrestest.c \
		clock_gettimetest.c \
		clock_settimetest.c \
		clock_gettimetest2.c \
		clock_gettimetest3.c \
		clock_gettimetest4.c \
		jt.c

PROGS = $(SOURCES:.c=) 

all: $(PROGS)

$(PROGS): $(POSIX_LIBDIR)/libposixtime.a

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(PROGS) *~ *.o core .depend 


.depend depend: $(SOURCES)
	$(CC) -M $(CPPFLAGS) $(SOURCES) | \
		sed -e '/:/s|\(^[^ :]*\)\.o|\1|' > .depend
	make

# This above make insures that we have the .depend when doing the build.

include .depend
