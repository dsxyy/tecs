#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：telnettecs.sh
# 文件标识：
# 内容摘要：自动登录tecs后台运行程序。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/3/22
#
# 修改记录1：
#     修改日期：2012/3/22
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建
#------------------------------------------------------------------------------
#include
. mylib.sh

#1. class:login--自动telnet登录；
#   arg:$1 procname:进程名 
#   style:login $procname

tecspath=/var/run/tecs
#process name, ex:Pwvir01-hc.telnet
match=*$1.telnet

login()
{
result=`find $tecspath/ -name "$match"`
if [ -z "$result" ]; then
    echo -e "no running tecs process, or process name is error!\n"
    echo "now running process!"
    cd $tecspath
    ls -1 *.telnet|awk -F '.' '{print $1}'|awk -F '-' '{print $2}'
    exit
fi

echo connecting active background tecs processes ...

#port:59388
port=`cat $tecspath/$match|grep -a "^port"|awk -F ':' '{print $2}'`
num=`cat $tecspath/$match|grep -a "^port"|awk -F ':' '{print $2}'|wc -l`
if [ $num -ne 1 ]; then
    echo "process name is error. now running process!"
    cd $tecspath
    ls -1 *.telnet|awk -F '.' '{print $1}'|awk -F '-' '{print $2}'
    exit
fi
echo telnet to $1 and port is $port ...
telnet 127.0.0.1 $port
}

login