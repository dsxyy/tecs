#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�starttecs.sh
# �ļ���ʶ��
# ����ժҪ��������������
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012��3��11��
#------------------------------------------------------------------------------
#include
. mylib.sh

#����������װΪ����
#����TECS��ؽ���
#class startapp
#$1:proc name
#start()
function startapp
{ 
    local app=$1
  
    #[start Tecs Process]
    echo "start $app ......"
    cd /opt/tecs/$app/bin
    for result in `find . -iname "tecs-${app}*"`
    do
      $result -f ../etc/tecs.conf -d
    done
}

#����tecs
function starttecs
{ 
  echo "start tecs ......"

  #[stop firewall]
  service iptables stop
  chkconfig --level 2345 iptables off
  
  #����ָ����tecs��ؽ��̣�Ĭ��ȫ����
  case $1 in
    "")               
    startapp tc
    startapp cc        
    startapp hc ;;  
    "tc")             
    startapp tc ;;  
    "cc")             
    startapp cc ;;   
    "hc")             
    startapp hc ;;
    *)               
    echo  "parase shoud be tc/cc/hc or null" ;;
  esac
  
  #check process status
  echo "check process status."
  ps -elf |grep tecs- |grep -v grep
  ps -elf |grep tecs- | grep -v grep |wc -l 
  
  #����������Ϣ�Զ��ɼ�
  read_bool "����TECS�����Ϣ�ɼ���" "yes"
  if [ "$answer" = "yes" ]; then
  service crond start
  else
  service crond stop
  fi
  
  echo "Congratulations! All tecs processes are running now!"
}



function showhelp
{
    echo "= Usage: starttecs.sh argument"
    echo "= argument supported: "
    echo "=    (default): start all tecs process"
    echo -e "=    tc/cc/hc: start specific process;\nEX: starttecs.sh tc ֻ����tc��ؽ��̡�"
    echo "=    help: show help message!"
}

#������ʽѡ��
case $1 in
    "")               starttecs ;;  
    "tc")             starttecs tc ;;
    "TC")             starttecs tc ;;  
    "cc")             starttecs cc ;; 
    "CC")             starttecs cc ;;   
    "hc")             starttecs hc ;;
    "HC")             starttecs hc ;;
    "help")           showhelp;;
    *)               
    echo "unknown argument!"
    showhelp;;
esac
