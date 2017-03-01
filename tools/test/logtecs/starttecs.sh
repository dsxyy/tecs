#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：starttecs.sh
# 文件标识：
# 内容摘要：安装tecs环境RPM包和启动各个进程
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2011年11月17日
#
# 修改记录1：
#     修改日期：2011/11/17
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建
# 修改记录2：
#     修改日期：2012/02/09
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：增加指定某个进程启动；
# 修改记录3：
#     修改日期：2012/02/28
#     版 本 号：V3.0
#     修 改 人：彭伟
#     修改内容：增加定时采集任务tecsinfocron启动方式；
#------------------------------------------------------------------------------
#进程启动封装为函数
#启动TC相关进程
function starttc
{
  #[start TC]
  echo "start tc ......"
  cd /opt/tecs/tc/bin
  if [ `find . -name "TC_CGSL*"` ]; then
  ./TC_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./TC_STORAGE_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./FTP_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./CLI_CGSL_X86_64_* -p ../scripts/
  ./VERSION_AGENT_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  
  else
  ./TC_PC_X86_64_* -f ../etc/tecs.conf -d        
  ./TC_STORAGE_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./FTP_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./CLI_PC_X86_64_* -p ../scripts/
  ./VERSION_AGENT_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d 
        
 fi
}

#启动CC相关进程
function startcc
{
  #[start CC]
  echo "start cc ......"
  cd /opt/tecs/cc/bin
  if [ `find . -name "CC_CGSL*"` ]; then
  ./CC_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./CC_STORAGE_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  
  else
  ./CC_PC_X86_64_* -f ../etc/tecs.conf -d         
  ./CC_STORAGE_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d 
    
  fi
} 

#启动HC相关进程
function starthc
{
  #[start HC]                                   
  echo "start hc ......"                     
  cd /opt/tecs/hc/bin                           
  if [ `find . -name "HC_CGSL*"` ]; then        
  ./HC_CGSL_X86_64_* -f ../etc/tecs.conf -d     
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d
                                                
  else                                          
  ./HC_PC_X86_64_* -f ../etc/tecs.conf -d       
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d  
                                                
  fi                                            
}

# get 'yes' or 'no' answer from user
function read_bool 
{
    local prompt=$1
    local default=$2
    local input
    echo -e "$prompt (y/n): \c "
    read -t 5 input
    case $input in
            "Y")    
            input="yes" ;;
            "y")    
            input="yes";;
            "n")    
            input="no";;
            "N")    
            input="no";;
            "")       
            input="$default";;
            *)       
            echo "Please input y or n" 
            read_bool;;
    esac
    answer=$input
    return 0
}

#启动tecs
function starttecs
{ 
  echo "start tecs ......"

  #[stop firewall]
  service iptables stop

  #[start qpid]
  qpidd -d 

  #[start postgresql]
  #/usr/local/pgsql/bin/config_postgresql.sh restart
  
  #启动指定的tecs相关进程，默认全启动
  case $1 in
    "")               
    starttc
    startcc        
    starthc ;;  
    "tc")             
    starttc ;;  
    "cc")             
    startcc ;;   
    "hc")             
    starthc ;;
    *)               
    echo  "parase shoud be tc/cc/hc or null" ;;
  esac
  
  #check process status
  echo "check process status."
  ps -elf |grep X86 |grep -v grep
  ps -elf |grep X86 | grep -v grep |wc -l 
  
  #启动运行信息自动采集
  read_bool "启动TECS相关信息采集？" "yes"
  if [ "$answer" = "yes" ]; then
  service crond start
  else
  service crond stop
  fi
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
    "tc")             starttecs "tc" ;;  
    "cc")             starttecs "cc" ;;   
    "hc")             starttecs "hc" ;;
    "help")           showhelp;;
    *)               
    echo "unknown argument!"
    showhelp;;
esac
