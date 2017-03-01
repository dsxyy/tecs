#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：starttecs.sh
# 文件标识：
# 内容摘要：启动各个进程
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012年3月11日
#------------------------------------------------------------------------------
#include
. mylib.sh

#进程启动封装为函数
#启动TECS相关进程
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

#启动tecs
function starttecs
{ 
  echo "start tecs ......"

  #[stop firewall]
  service iptables stop
  chkconfig --level 2345 iptables off
  
  #启动指定的tecs相关进程，默认全启动
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
  
  #启动运行信息自动采集
  read_bool "启动TECS相关信息采集？" "yes"
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
    echo -e "=    tc/cc/hc: start specific process;\nEX: starttecs.sh tc 只启动tc相关进程。"
    echo "=    help: show help message!"
}

#启动方式选择
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
