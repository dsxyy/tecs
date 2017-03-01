#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�cnfyum.sh
# �ļ���ʶ��
# ����ժҪ���Զ�����yum��
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/4/11
#------------------------------------------------------------------------------
yum_home=/etc/yum.repos.d

cd $yum_home

if ! [ -w tecs.repo ]; then
   echo -e "[tecs]" >tecs.repo
   echo -e "name=Tecs" >>tecs.repo
   echo -e "baseurl=http://10.43.178.191/iso/mnt_centos_x8664/" >>tecs.repo
   echo -e "enabled=1" >>tecs.repo
   echo -e "gpgcheck=0" >>tecs.repo
fi

echo "alias yum='yum --disablerepo=* --enablerepo=tecs'"