#!/bin/sh
module="fsdriver"
device="fsdriver"
DriverLocation="/depot/fdriver/fdriver"
mode="664"

# invoke insmod with all arguments we were passed
# and use a pathname, as newer modutils don't look in . by default
/sbin/insmod -f $DriverLocation/$module.ko $* || exit 1

# remove stale nodes
rm -f /dev/${device}[0-3]

major=`awk "\\$2==\"$module\" {print \\$1}" /proc/devices`

echo Driver Name:  \"$module\"
echo Major number  \"$major\" 
echo Device File name  \"$device\" 

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

# from: http://www.xml.com/ldd/chapter/book/ch03.html 

## give appropriate group/permissions, and change the group.
## Not all distributions have staff; some have "wheel" instead.
#group="staff"
#grep '^staff:' /etc/group > /dev/null || group="wheel"
#
#chgrp $group /dev/${device}[0-3]
#chmod $mode /dev/${device}[0-3]



##!/bin/bash

#rmmod fdriver
#insmod fdriver
#rm /dev/fdriver
#mknod /dev/fdriver c 249 0
#chmod a+w /dev/fdriver
