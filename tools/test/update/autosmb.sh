#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：autoSamba.sh
# 文件标识：
# 内容摘要：自动安装，配置和启动smb服务；
# 当前版本：2.0
# 作    者：彭伟
# 完成日期：2012年01月13日
# 修改记录1：
#     修改日期：2012/01/13
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建；
# 修改记录2：
#     修改日期：2012/01/18
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：增加设置samba开机启动；
# 修改记录3：
#     修改日期：2012/04/9
#     版 本 号：V3.0
#     修 改 人：彭伟
#     修改内容：centos中配置smb服务；
#------------------------------------------------------------------------------
#rpm -ivh samba-3.0.33-3.14.el5.x86_64.rpm --nodeps --force
#rpm -ivh samba-client-3.0.33-3.14.el5.x86_64.rpm --nodeps --force
#rpm -ivh samba-common-3.0.33-3.14.el5.x86_64.rpm

echo -e "\n[root@linux samba]#  smbpasswd -a root    //设置smb root用户密码
New SMB password: ossdbg1
Retype new SMB password: ossdbg1
***********************\n"
smbpasswd -a root

if [ `find . -name "smb.conf"` ]; then
cp -f ./smb.conf /etc/samba/smb.conf
else 
echo "Can't find smb.conf"
fi

service smb restart

chkconfig smb --level 3 on

#[stop firewall]
service iptables stop