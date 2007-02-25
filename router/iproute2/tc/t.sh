#!/bin/sh

mipsel-uclibc-gcc   -D_GNU_SOURCE -O2 -Wstrict-prototypes -Wall -g -I../include-glibc -I/usr/include/db3 -include ../include-glibc/glibc-bugs.h -I/home/zhang/0509/src/linux/linux-2.4.17_mvl21/include -I../include -DRESOLVE_HOSTNAMES   -c -o q_htb.o q_htb.c
