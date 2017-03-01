#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�getVersion.sh
# �ļ���ʶ��
# ����ժҪ���Զ���������TECS�汾��
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/3/11
#
# �޸ļ�¼1��
#     �޸����ڣ�2012/3/11
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�֧��ftp���ذ汾
# �޸ļ�¼2��
#     �޸����ڣ�2012/3/27
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ��޸�֧�ַ�֧�汾����
#------------------------------------------------------------------------------
#include
. mylib.sh

#class:getversion--ftp���ظ��°汾
#arg:$1 ver_no���汾��		$2 ver_sub����֧�汾·��  	$3 ver_home���汾��֧	  
#    $4 ftp_ip��ftp������IP  	$5 ftp_user���û���  		$6 ftp_passwd������     
#style:getversion $ver_no $ver_sub $ver_home $ftp_ip $ftp_user $ftp_passwd 
function getversion 
{ 
  #ftp��ز���
  ftp_ip=$4
  ftp_user=$5
  ftp_passwd=$6
  #$1:�汾��
  ver_no=$1
  ver_sub=$2
  #���ذ汾·��
  loc_home="/home/tecs"
  #FTP�汾·��
  ver_home=$3
  check_dir $loc_home
  cd $loc_home

#���Զ��FTP�ļ����Ƿ���ڣ��������������
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
#ע��ftp�ض���󣬺�������ǰ�����пո񣬷�������޷���ftpʶ��  
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
     echo -e "������汾�ţ��磺getVersion.sh 20120220��\n������ָ��ftp�����������룬�磺getversion.sh version_no el6 tecs_bugfix 10.43.178.198 user password!"
     exit;;
esac

 

