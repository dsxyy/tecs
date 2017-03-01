#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：LogPgsqlInfo.sh
# 文件标识：
# 内容摘要：记录postgresql的数据库操作的相关日志。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012年02月24日
# 修改记录1：
#     修改日期：2012/02/28
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：记录startup.log信息到pginfo.log文件中。
#------------------------------------------------------------------------------
#~~~重要变量定义声明~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog
LogFile=`date +"%m_%d"`_`hostname`_pginfo.log  #如：02_27_pginfo.log每天记录一个日志文件；
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~类1: pginfolog null
function pginfolog
{ 
  echo "---Begin log postgresql operater info at $(date +"%Y-%m-%d %H:%M:%S")---" >>$LogFile
  echo -e "--------------------------------------------------\n" >>$LogFile
  cat /usr/local/pgsql/data/pg_log/startup.log |tail -100 >>$LogFile 
  echo -e "\n---End log postgresql operater info at $(date +"%Y-%m-%d %H:%M:%S"---)" >>$LogFile
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
pginfolog
exit 0

