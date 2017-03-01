#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：logtecs.sh
# 文件标识：
# 内容摘要：打包/var/log/tecs下所有的当天相关日志到/home/tecs目录下。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012年02月24日
# 修改记录1：
#     修改日期：2012/02/28
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：
#------------------------------------------------------------------------------
#~~~重要变量定义声明~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog             #日志采集根目录
CurDir=/home/tecs                           #所取日志压缩包临时存放目录
LogFile=`date +"%m_%d"`_`hostname`.tar.bz2  #如：02_27_hostname.tar.bz2 #日志文件压缩包；
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~类1: checkDir Dir
function checkDir
{
  Dir=$1
  if ! [ -d $Dir ];then
  mkdir $Dir
  chmod 777 $Dir
  fi
}
#~~~类2: pginfolog null
function tarlog
{ 
  echo -e "开始打包当前TECS的运行日志（当天）！存放在/home/tecs目录下，如：02_29_Pwvir01.tar.bz2"
  echo -e "如果想获取更多日志，请去/var/log/tecs目录下自取！\n" 
  #/var/log/tecs/TecsExcLog目录下帅选当天的日志，cp到/var/log/tecs/%m_%d文件夹中
  tempdir=/var/log/tecs/`date +"%m_%d"`
  checkDir $tempdir  
  cd $BakDir
  cp -f `find . -name "${match}*"` $tempdir
  cp -fr /var/log/tecs/HcExcLog    $tempdir
  cp -fr /var/log/tecs/LocalExcLog $tempdir
  
  #打包日志文件HcExcLog，LocalExcLog，%m_%d，并移动到/home/tecs目录下；
  tar -jvcf ${CurDir}/$LogFile $tempdir 
  
  #删除临时的%m_%d文件夹
  rm -rf $tempdir
}

case $1 in  
    "help")           
    echo "指定取某天的日志，请输入参数month_day，如02_29" 
    exit 0 ;; 
    *)                match=$1   ;;
esac

match=$1
if [ -z $match ]; then
  #默认match为查找当天日志文件的关键字
  match=`date +"%m_%d"`
fi

checkDir $BakDir
checkDir $CurDir
tarlog
exit 0

