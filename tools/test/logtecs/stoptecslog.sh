#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：stoptecslog.sh
# 文件标识：
# 内容摘要：定时检查tecs运行情况，停止则自动停止采集任务
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
#进程启动封装为函数
result=`service crond status |awk '{print $5}'`
if [ $result == "running..." ]; then
#check tecs process status is stop, then stop crond.
if ! [ `ps -elf |grep X86 |grep -v grep` ]; then
  service crond stop
  echo "stop logtecsinfo cron!"
fi
else
exit 0
fi

