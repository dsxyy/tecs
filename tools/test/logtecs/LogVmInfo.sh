#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�LogVmInfo.sh
# �ļ���ʶ��
# ����ժҪ����¼xm�������Ϣ��
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012��02��24��
# �޸ļ�¼1��
#     �޸����ڣ�2012/02/24
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ���¼xm list��xm log��Ϣ��vminfo.log�ļ��С�
#------------------------------------------------------------------------------
#~~~��Ҫ������������~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog
LogFile=`date +"%m_%d"`_`hostname`_vminfo.log  #�磺02_27_hostname_vminfo.logÿ���¼һ����־�ļ����Ժ�Ҳ���Կ���ÿСʱ��¼һ����־�ļ���
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~��1: vminfolog null
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
#~~~��2: checkDir Dir
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

#ִ�в���
#case $1 in
#    "")               starttecs ;;  
#    "tc")             starttecs "tc" ;;  
#    ...
#    "help")           showhelp;;
#    *)               
#    echo "unknown argument!"
#    showhelp;;
#esac

#��ʾ����
#function showhelp
#{
#    echo "= Usage: starttecs.sh argument"
#    echo "= argument supported: "
#    echo "=    (default): start all tecs process"
#    echo -e "=    tc/cc/hc: start specific process;\nEX: starttecs.sh tc ֻ����tc��ؽ��̡�"
#    echo "=    help: show help message!"
#}
 