#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：getVersion.sh
# 文件标识：
# 内容摘要：自动更新下载TECS版本。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/3/11
#
# 修改记录1：
#     修改日期：2012/3/11
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：支持ftp下载版本
# 修改记录2：
#     修改日期：2012/3/27
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：修改支持分支版本下载
#------------------------------------------------------------------------------
#include
. mylib.sh

#class:getversion--ftp下载更新版本
#arg:$1 ver_no：版本号		$2 ver_sub：分支版本路径  	$3 ver_home：版本分支	  
#    $4 ftp_ip：ftp服务器IP  	$5 ftp_user：用户名  		$6 ftp_passwd：密码     
#style:getversion $ver_no $ver_sub $ver_home $ftp_ip $ftp_user $ftp_passwd 
function getversion 
{ 
  #ftp相关参数
  ftp_ip=$4
  ftp_user=$5
  ftp_passwd=$6
  #$1:版本号
  ver_no=$1
  ver_sub=$2
  #本地版本路径
  loc_home="/home/tecs"
  #FTP版本路径
  ver_home=$3
  check_dir $loc_home
  cd $loc_home

#检查远程FTP文件夹是否存在，如果存在再下载
/usr/bin/ftp -i -n $ftp_ip<<!>ftp1.log
user $ftp_user $ftp_passwd
cd "$ver_home"
ls -l
bye
!
  grep $ver_no ftp1.log
  if ! [ $? -eq 0 ]; then
     echo "Error! Version No. is not Exist. please check again!"
     rm -rf ftp1.log
     exit 0
  fi
  rm -rf ftp1.log
  check_dir $ver_no
  cd $ver_no
  
  temp=`ls -l |wc -l`
  
  if [ $temp -gt 9 ]; then
    ls -l
    echo "Congratulations! version already exist!"
    chmod 753 ./*
    exit 0
  fi

echo "start downloading version $ver_sub ......"
#注：ftp重定向后，后面命令前不能有空格，否则可能无法被ftp识别！  
/usr/bin/ftp -i -n $ftp_ip<<EOF
user $ftp_user $ftp_passwd
binary
cd "$ver_home/$ver_no/$ver_sub"
mget *
close
bye
EOF

#/usr/bin/ftp -i -n $ftp_ip<<!>ftp2.log
#user $ftp_user $ftp_passwd
#cd "$ver_home/$ver_no"
#ls -l
#bye
#!
#
#grep scripts ftp2.log
#if [ $? -eq 0 ]; then
#check_dir scripts 
#/usr/bin/ftp -i -n 10.43.178.198<<EOF
#user $ftp_user $ftp_passwd
#binary
#cd "$ver_home/$ver_no/scripts"
#lcd "scripts"
#mget *
#close
#bye
#EOF
#fi
#  rm -rf ftp2.log
  chmod -R 754 ./*
  toltal=`ls -l|grep -i "total"|awk '{print $2}'`
  if [ $toltal -gt 9 ]; then
    echo "Congratulations! Update version successfully!"
  else
    echo "Error! Update version failed!"
  fi
}
case $# in
     "1")
     getversion $1 el6 tecs 10.43.178.198 versioner versioner ;;   
     "2")
     getversion $1 $2 tecs 10.43.178.198 versioner versioner ;;
     "3")
     getversion $1 $2 $3 10.43.178.198 versioner versioner ;;
     "5")
     getversion $1 $2 $3 $4 $5 $6 ;;
     "0")
     echo -e "请输入版本号，如：getVersion.sh 20120220；\n若重新指定ftp服务器请输入，如：getversion.sh version_no el6 tecs_bugfix 10.43.178.198 user password!"
     exit;;
esac

 

