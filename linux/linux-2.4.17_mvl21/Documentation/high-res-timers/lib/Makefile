CC = mips_fp_le-gcc

LIBPOSIXTIME = libposixtime.a

SOURCES = syscall_timer.c 
OBJECTS = $(SOURCES:.c=.o)

SYSASM =  -I../../../include/ -I../usr_incl 
CPPFLAGS = -D_POSIX_TIMERS=1 -D_POSIX_C_SOURCE=199309L -D_XOPEN_SOURCE $(SYSASM)
CFLAGS = -g -Wall 

all: $(LIBPOSIXTIME)($(OBJECTS))

clean:
	rm -f *.o *.a *~ core .depend

.depend: $(SOURCES)
	$(CC) $(CPPFLAGS)  -M $(SOURCES) | \
		sed -e '/:/s|^[^ :]*|$(LIBPOSIXTIME)(&)|' > .depend
	make

# The above make insures that "all:" is done with the newly created .depend

include .depend
