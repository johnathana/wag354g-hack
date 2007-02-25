#!/bin/sh

mipsel-uclibc-gcc  -O2 -Wstrict-prototypes -Wall -g -I../include-glibc -I/usr/include/db3  -I/home/zhang/0509/src/linux/linux-2.4.17_mvl21/include -I../include -c -o q_htb.o q_htb.c
