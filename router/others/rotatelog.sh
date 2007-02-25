#!/bin/sh

if test  "$#" -le "0" 
then
	echo "Usage:$0 line-numbers sleep_time"
        exit
fi

MESS="/var/log/mess"
TEMP="/tmp/.mess"

until false
do
	if test -f $MESS ; then
		tail -n $1 $MESS > $TEMP
		cat $TEMP > $MESS
		rm -f $TEMP
	else
		continue
	fi
	if test "$2" -eq "" ; then
		exit 0;
	else
		sleep $2;
	fi
done
