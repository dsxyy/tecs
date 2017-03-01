#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：LogVmInfo.sh
# 文件标识：
# 内容摘要：记录xm的相关信息。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012年02月24日
# 修改记录1：
#     修改日期：2012/02/24
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：记录xm list和xm log信息到vminfo.log文件中。
#------------------------------------------------------------------------------
#~~~重要变量定义声明~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog
LogFile=`date +"%m_%d"`_`hostname`_vminfo.log  #如：02_27_hostname_vminfo.log每天记录一个日志文件；以后也可以考虑每小时记录一个日志文件；
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~类1: vminfolog null
function vminfolog
{ 
  echo "---Begin log xm info at $(date +"%Y-%m-%d %H:%M:%S")---" >>$LogFile
  echo -e "--------------------------------------------------\n" >>$LogFile
  /usr/sbin/xm list >>$LogFile
  echo -e "\n" >>$LogFile
  /usr/sbin/xm log |tail -50 >>$LogFile 
  echo -e "\n---End log xm info at $(date +"%Y-%m-%d %H:%M:%S"---)" >>$LogFile
  echo -e "--------------------------------------------------\n" >>$LogFile
}
#~~~类2: checkDir Dir
function checkDir
{
  Dir=$1
  if ! [ -d $Dir ];then
  mkdir $Dir
  chmod 777 $Dir
  fi
}

checkDir $BakDir
cd $BakDir
vminfolog
exit 0

#执行参数
#case $1 in
#    "")               starttecs ;;  
#    "tc")             starttecs "tc" ;;  
#    ...
#    "help")           showhelp;;
#    *)               
#    echo "unknown argument!"
#    showhelp;;
#esac

#提示帮助
#function showhelp
#{
#    echo "= Usage: starttecs.sh argument"
#    echo "= argument supported: "
#    echo "=    (default): start all tecs process"
#    echo -e "=    tc/cc/hc: start specific process;\nEX: starttecs.sh tc 只启动tc相关进程。"
#    echo "=    help: show help message!"
#}
 