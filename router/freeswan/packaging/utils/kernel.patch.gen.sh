#!/bin/bash
#
# RCSID $Id: kernel.patch.gen.sh,v 1.1.1.2 2005/03/28 06:57:43 sparq Exp $

patchdir=`pwd`
kernelsrc=/usr/src/linux
[ "$1~" = "~" ] || kernelsrc=$1
cd $kernelsrc
# clean out destination file for all patch
#echo "">$patchdir/all

# find files to patch and loop
for i in  `find . -name '*.preipsec'`
do

# strip off '.preipsec' suffix
j=${i%.preipsec}

# strip off './' prefix
k=${j#\.\/}

# single unified diff
#diff -u $i $j >>$patchdir/all

# convert '/' in filename to '.' to avoid subdirectories
sed -e 's/\//\./g' << EOI > /tmp/t
$k
EOI
l=`cat /tmp/t`
rm -f /tmp/t

# *with* path from source root
#echo do diff -u $i $j '>' $patchdir/$l
echo found $i
echo "RCSID \$Id: kernel.patch.gen.sh,v 1.1.1.2 2005/03/28 06:57:43 sparq Exp $" >$patchdir/$l
diff -u $i $j >>$patchdir/$l

done

#
# $Log: kernel.patch.gen.sh,v $
# Revision 1.1.1.2  2005/03/28 06:57:43  sparq
# - GuangZhou release v1.00.57
#
# Revision 1.1.1.1  2004/07/26 01:39:26  sparq
# Mirror v0.01.9 release from K.C division of CyberTAN.
#
# Revision 1.1.1.1  2002/09/05 03:13:20  ken
# 1.98b
#
# Revision 1.4  1999/04/06 04:54:30  rgb
# Fix/Add RCSID Id: and Log: bits to make PHMDs happy.  This includes
# patch shell fixes.
#
#
