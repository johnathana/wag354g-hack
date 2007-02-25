#!/bin/sh
for mylist in `ls`
do 
	cat $mylist | sed 's/Broadband Router/ADSL Gateway/g' > tmp; mv tmp $mylist
	cat $mylist | sed 's/More...//g' > tmp; mv tmp $mylist
done

