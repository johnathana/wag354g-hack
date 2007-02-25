#!/bin/sh
cd router
make linux-clean
make linux-dep
make linux
make clean
make 
make linux-modules
make install
make rom
make upgrade
cd ..
