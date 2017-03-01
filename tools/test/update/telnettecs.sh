#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�telnettecs.sh
# �ļ���ʶ��
# ����ժҪ���Զ���¼tecs��̨���г���
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/3/22
#
# �޸ļ�¼1��
#     �޸����ڣ�2012/3/22
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����
#------------------------------------------------------------------------------
#include
. mylib.sh

#1. class:login--�Զ�telnet��¼��
#   arg:$1 procname:������ 
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