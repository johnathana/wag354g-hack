#!/bin/sh

#
# $Id: runme.sh,v 1.1.1.2 2005/03/28 06:57:40 sparq Exp $
#
# use this script to run a single test from within that test directory.
# note that this assumes a "klipstest" type test.
#

. ../../../umlsetup.sh
. ../setup.sh
. ../functions.sh
. testparams.sh

if [ -z "${TEST_TYPE}" ]
then
    echo runme.sh now requires that testparams.sh defines TEST_TYPE=
fi

( cd .. && $TEST_TYPE $TESTNAME good )

# $Log: runme.sh,v $
# Revision 1.1.1.2  2005/03/28 06:57:40  sparq
# - GuangZhou release v1.00.57
#
# Revision 1.1.1.1  2004/07/26 01:39:25  sparq
# Mirror v0.01.9 release from K.C division of CyberTAN.
#
# Revision 1.2  2002/11/05 01:38:03  ken
# 1.99
#
# Revision 1.3.2.1  2002/11/03 04:38:43  mcr
# 	port of runme.sh script
#
# Revision 1.2  2002/05/23 14:26:39  mcr
# 	verify that $TEST_TYPE is actually set.
#
# Revision 1.1  2002/05/05 23:12:05  mcr
# 	runme.sh script now common for all test types.
#
# 

