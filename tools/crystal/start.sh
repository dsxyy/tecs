#!/bin/sh
#make the crystal module

#detect crystal module
ZHQ=$(lsmod |grep "BladeCrystal" |awk '{print $1}')

# if the crystal module already in the system then remove the module and insert
# if the crystal module isn't in the system then insert the module
if [ "$ZHQ" = "BladeCrystal" ]
then
echo "find and remove the crystal module"
rmmod BladeCrystal
fi
echo "insmod BladeCrystal.ko"
insmod BladeCrystal.ko



