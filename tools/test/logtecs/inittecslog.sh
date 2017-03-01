#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：inittecslog.sh
# 文件标识：
# 内容摘要：安装和自动配置信息采集脚本
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012年2月28日
#
# 修改记录1：
#     修改日期：2012/2/28
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建
#------------------------------------------------------------------------------
#创建日志信息备份根目录
BakDir=/var/log/tecs/TecsExcLog

if ! [ -d $BakDir ];then
mkdir $BakDir
chmod 777 $BakDir
fi

#创建tecs信息采集的crontab
crontab tecsinfocron

#自动复制采集脚本到usr/local/bin
#强制更新starttecs.sh
cp -f ./starttecs.sh /usr/local/bin/starttecs.sh
#强制更新killtecs.sh
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


