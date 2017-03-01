#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�inittecslog.sh
# �ļ���ʶ��
# ����ժҪ����װ���Զ�������Ϣ�ɼ��ű�
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012��2��28��
#
# �޸ļ�¼1��
#     �޸����ڣ�2012/2/28
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����
#------------------------------------------------------------------------------
#������־��Ϣ���ݸ�Ŀ¼
BakDir=/var/log/tecs/TecsExcLog

if ! [ -d $BakDir ];then
mkdir $BakDir
chmod 777 $BakDir
fi

#����tecs��Ϣ�ɼ���crontab
crontab tecsinfocron

#�Զ����Ʋɼ��ű���usr/local/bin
#ǿ�Ƹ���starttecs.sh
cp -f ./starttecs.sh /usr/local/bin/starttecs.sh
#ǿ�Ƹ���killtecs.sh
cp -f ./killtecs.sh /usr/local/bin/killtecs.sh

if [ `find . -name "LogVmInfo.sh"` ]; then
cp -f ./LogVmInfo.sh /usr/local/bin/LogVmInfo.sh
chmod 777 /usr/local/bin/LogVmInfo.sh
else
echo "Can't find LogVmInfo.sh"
fi

if [ `find . -name "LogPgsqlInfo.sh"` ]; then
cp -f ./LogPgsqlInfo.sh /usr/local/bin/LogPgsqlInfo.sh
chmod 777 /usr/local/bin/LogPgsqlInfo.sh
else
echo "Can't find LogPgsqlInfo.sh"
fi

if [ `find . -name "stoptecslog.sh"` ]; then
cp -f ./stoptecslog.sh /usr/local/bin/stoptecslog.sh
chmod 777 /usr/local/bin/stoptecslog.sh
else
echo "Can't find stoptecslog.sh"
fi

if [ `find . -name "logtecs.sh"` ]; then
cp -f ./logtecs.sh /usr/local/bin/logtecs.sh
chmod 777 /usr/local/bin/logtecs.sh
else
echo "Can't find logtecs.sh"
fi

echo "install auto_tecs_log success!"


