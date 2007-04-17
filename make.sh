#!/bin/sh
cd router
make linux-clean
make linux-dep
make -j2 linux
make clean
make -j2 
make linux-modules
make install
make rom
make upgrade
cd ..
