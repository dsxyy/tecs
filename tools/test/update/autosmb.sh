#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�autoSamba.sh
# �ļ���ʶ��
# ����ժҪ���Զ���װ�����ú�����smb����
# ��ǰ�汾��2.0
# ��    �ߣ���ΰ
# ������ڣ�2012��01��13��
# �޸ļ�¼1��
#     �޸����ڣ�2012/01/13
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�������
# �޸ļ�¼2��
#     �޸����ڣ�2012/01/18
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ���������samba����������
# �޸ļ�¼3��
#     �޸����ڣ�2012/04/9
#     �� �� �ţ�V3.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�centos������smb����
#------------------------------------------------------------------------------
#rpm -ivh samba-3.0.33-3.14.el5.x86_64.rpm --nodeps --force
#rpm -ivh samba-client-3.0.33-3.14.el5.x86_64.rpm --nodeps --force
#rpm -ivh samba-common-3.0.33-3.14.el5.x86_64.rpm

echo -e "\n[root@linux samba]#  smbpasswd -a root    //����smb root�û�����
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