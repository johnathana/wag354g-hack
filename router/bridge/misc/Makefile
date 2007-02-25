# $Id: Makefile,v 1.1.1.2 2005/03/28 06:57:28 sparq Exp $

CFLAGS = -Wall -g -O2
ifeq ($(strip $(DOSTATIC)),true)
    CFLAGS += --static
endif


all:		bidi tap

clean:
		rcsclean *
		rm -f *~
		rm -f bidi
		rm -f tap

bidi:		bidi.c
		$(CC) $(CFLAGS) -o bidi bidi.c

tap:		tap.c
		$(CC) $(CFLAGS) -o tap tap.c
