#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�LogPgsqlInfo.sh
# �ļ���ʶ��
# ����ժҪ����¼postgresql�����ݿ�����������־��
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012��02��24��
# �޸ļ�¼1��
#     �޸����ڣ�2012/02/28
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ���¼startup.log��Ϣ��pginfo.log�ļ��С�
#------------------------------------------------------------------------------
#~~~��Ҫ������������~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog
LogFile=`date +"%m_%d"`_`hostname`_pginfo.log  #�磺02_27_pginfo.logÿ���¼һ����־�ļ���
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~~��1: pginfolog null
function pginfolog
{ 
  echo "---Begin log postgresql operater info at $(date +"%Y-%m-%d %H:%M:%S")---" >>$LogFile
  echo -e "--------------------------------------------------\n" >>$LogFile
  cat /usr/local/pgsql/data/pg_log/startup.log |tail -100 >>$LogFile 
  echo -e "\n---End log postgresql operater info at $(date +"%Y-%m-%d %H:%M:%S"---)" >>$LogFile
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
pginfolog
exit 0

