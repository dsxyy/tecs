#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�stoptecslog.sh
# �ļ���ʶ��
# ����ժҪ����ʱ���tecs���������ֹͣ���Զ�ֹͣ�ɼ�����
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
#����������װΪ����
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

