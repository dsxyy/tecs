#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�logtecs.sh
# �ļ���ʶ��
# ����ժҪ�����/var/log/tecs�����еĵ��������־��/home/tecsĿ¼�¡�
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012��02��24��
# �޸ļ�¼1��
#     �޸����ڣ�2012/02/28
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�
#------------------------------------------------------------------------------
#~~~��Ҫ������������~~~~~~~~~~~~
BakDir=/var/log/tecs/TecsExcLog             #��־�ɼ���Ŀ¼
CurDir=/home/tecs                           #��ȡ��־ѹ������ʱ���Ŀ¼
LogFile=`date +"%m_%d"`_`hostname`.tar.bz2  #�磺02_27_hostname.tar.bz2 #��־�ļ�ѹ������
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~��1: checkDir Dir
function checkDir
{
  Dir=$1
  if ! [ -d $Dir ];then
  mkdir $Dir
  chmod 777 $Dir
  fi
}
#~~~��2: pginfolog null
function tarlog
{ 
  echo -e "��ʼ�����ǰTECS��������־�����죩�������/home/tecsĿ¼�£��磺02_29_Pwvir01.tar.bz2"
  echo -e "������ȡ������־����ȥ/var/log/tecsĿ¼����ȡ��\n" 
  #/var/log/tecs/TecsExcLogĿ¼��˧ѡ�������־��cp��/var/log/tecs/%m_%d�ļ�����
  tempdir=/var/log/tecs/`date +"%m_%d"`
  checkDir $tempdir  
  cd $BakDir
  cp -f `find . -name "${match}*"` $tempdir
  cp -fr /var/log/tecs/HcExcLog    $tempdir
  cp -fr /var/log/tecs/LocalExcLog $tempdir
  
  #�����־�ļ�HcExcLog��LocalExcLog��%m_%d�����ƶ���/home/tecsĿ¼�£�
  tar -jvcf ${CurDir}/$LogFile $tempdir 
  
  #ɾ����ʱ��%m_%d�ļ���
  rm -rf $tempdir
}

case $1 in  
    "help")           
    echo "ָ��ȡĳ�����־�����������month_day����02_29" 
    exit 0 ;; 
    *)                match=$1   ;;
esac

match=$1
if [ -z $match ]; then
  #Ĭ��matchΪ���ҵ�����־�ļ��Ĺؼ���
  match=`date +"%m_%d"`
fi

checkDir $BakDir
checkDir $CurDir
tarlog
exit 0

