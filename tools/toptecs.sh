#!/bin/sh
###############################################################################
#                 TECS System Administration Scripts                          #
#                        Author: Zhang Wenjian                                #
#  version: 1.0                                                               #
#  usage: apply top command on running processes which name contain a string  #
#                                                                             #
###############################################################################

match="tecs-"
if [ ! -z $1 ]; then
match=$1
fi

pids=""
for i in `ps -elf | grep $match | grep -v grep | awk -F ' ' '{print $4}'`
do
    pids=" $pids  -p $i "
done
top  $pids
