#!/bin/sh
# 此脚本用来模拟模拟hc重启
echo "$0 will simulate reboot hc"
#service guard stop

rm -rf /dev/shm/tecs/*
rm -rf /dev/shm/tecsprint/*

for vm in `xm list | awk -F' ' '{print $2}' | grep -v ID | grep -v 0`
do
    echo "xm destroy $vm ..."
    xm destroy $vm
    [ 0 -ne $? ] && exit -1
done

# kill tecs process
for i in `ps -elf | grep HC | grep -v grep | awk -F ' ' '{ print $4}'`
do
        if [ "$i" == "$$" ]; then
            continue
        fi

        if [ -d /proc/$i ]; then
            echo will kill pid $i ...
            kill -9 $i
        fi
done



#rm -rf /dev/shm/*
#service guard start
echo "will wait 60s"
sleep 60

xm list



echo "tecs simulate hc reboot success!"
exit 0
