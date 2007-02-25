#
# use this script to undo effects of sourcing a "testparams.sh" into
# your shell, when testing.
#
# $Id: cleanup.sh,v 1.1.1.2 2005/03/28 06:57:40 sparq Exp $
#
unset SCRIPT
unset REFCONSOLEOUTPUT 
unset PRIVINPUT
unset PUBINPUT
unset REFPRIVOUTPUT
unset REFPUBOUTPUT
unset TCPDUMPARGS

#
# $Log: cleanup.sh,v $
# Revision 1.1.1.2  2005/03/28 06:57:40  sparq
# - GuangZhou release v1.00.57
#
# Revision 1.1.1.1  2004/07/26 01:39:25  sparq
# Mirror v0.01.9 release from K.C division of CyberTAN.
#
# Revision 1.1.1.1  2002/09/05 03:13:18  ken
# 1.98b
#
# Revision 1.3  2002/02/20 07:26:24  rgb
# Corrected de-pluralized variable names.
#
# Revision 1.2  2001/11/23 01:08:12  mcr
# 	pullup of test bench from klips2 branch.
#
# Revision 1.1.2.1  2001/10/23 04:43:18  mcr
# 	shell/testing cleaning script.
#
# 
