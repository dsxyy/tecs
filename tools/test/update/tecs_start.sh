#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�installandstart.sh
# �ļ���ʶ��
# ����ժҪ����װtecs����RPM����������������
# ��ǰ�汾��2.0
# ��    �ߣ���ΰ
# ������ڣ�2011��11��17��
#
# �޸ļ�¼1��
#     �޸����ڣ�2011/11/17
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����
# �޸ļ�¼2��
#     �޸����ڣ�2012/01/18
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ��޸�Ϊֻ������ʱʹ��
# �޸ļ�¼3��
#     �޸����ڣ�2012/02/09
#     �� �� �ţ�V3.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�֧���Զ���װ��ʽ--���û�����ʱ8s�Զ�ѡ��Ĭ�����ð�װ
# �޸ļ�¼4��
#     �޸����ڣ�2012/03/01
#     �� �� �ţ�V4.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�1)�����Զ�ж��qpidc;2)ȡ��tecs�û�������3����������starttecs.sh��
# �޸ļ�¼4��
#     �޸����ڣ�2012/03/16
#     �� �� �ţ�V5.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����sed��ʽ�޸�pg_hba.conf�������
#------------------------------------------------------------------------------
#record current version path
#export versionpath=`pwd`
#echo $versionpath

# get 'yes' or 'no' answer from user
function read_bool 
{
    local prompt=$1
    local default=$2
    local input
    echo -e "$prompt (y/n): \c "
    read -t 5 input
    case $input in
            "Y")    
            input="yes" ;;
            "y")    
            input="yes";;
            "n")    
            input="no";;
            "N")    
            input="no";;
            "")       
            input="$default";;
            *)       
            echo "Please input y or n" 
            read_bool;;
    esac
    answer=$input
    return 0
}

#echo -e "�汾��ʼ����������ж�ؾɰ汾�ٰ�װ�°汾!\n���ȷ�ϣ�������y������������n��ֻ������tecs��"
#read answer
read_bool "�汾��ʼ����������ж�ؾɰ汾�ٰ�װ�°汾!\n���ȷ�ϣ�������y������������n��ֻ������tecs��" "yes" 

#first install
if [ "$answer" = "yes" ]; then
echo "��ʼ��װtecs��"

export tecsbin=`find . -name "installtecs*"`

#[install tecs]
if [ `find . -name "installtecs*"` ]; then
echo $tecsbin "uninstall"
$tecsbin uninstall
#qpidc-client��devel uninstall
rpm -e qpidc-devel-0.12-1.x86_64
rpm -e qpidc-client-0.12-1.x86_64
rpm -e qpidc-0.12-1.x86_64

#drop database
read_bool "�Ƿ�ɾ�������ݿ⣬������y/n��" "no" 
if [ "$answer" = "yes" ]; then
echo -e "rm -rf /usr/local/pgsql�����Ѿ�ɾ������Ŀ¼��\n"
rm -rf /usr/local/pgsql
sleep 1
fi

#ɾ����HOSTע��CC��Ϣ
read_bool "�Ƿ�ɾ��/dev/shm/tecs�µľ�HOSTע����Ϣ��������y/n��" "no" 
if [ "$answer" = "yes" ]; then
echo -e "rm -rf /dev/shm/tecs/*\n"
rm -rf /dev/shm/tecs/*
sleep 1
fi

#ɾ���������ʵ��
read_bool "�Ƿ�ɾ��/var/lib/tecs/instances�µľ������ʵ����������y/n��" "no" 
if [ "$answer" = "yes" ]; then
echo -e "rm -rf /var/lib/tecs/instances/*\n"
rm -rf /var/lib/tecs/instances/*
sleep 1
fi

echo $tecsbin "install"
$tecsbin install

. /usr/local/pgsql/postgresql.env

#pgadmin access
sed -i '/^host    all         all         127.0.0.1/a\\host    all         all         10.0.0.0/8            trust' /usr/local/pgsql/data/pg_hba.conf
sed -i '/^host    all         all         127.0.0.1/ s/md5/trust/' /usr/local/pgsql/data/pg_hba.conf
if [ $? -eq 0 ];then
echo "Congratulations! pgAdmin can access DB."
fi
#sudo -u postgresql createuser -P -s tecs

else
echo "��cd���汾Ŀ¼��ִ�л��߰�tecs_start.sh�������汾Ŀ¼��ִ�У�лл��"
exit

fi

#check all rpm 
echo -e "\n"
rpm -qa |grep openais
rpm -qa |grep qpid
rpm -qa |grep postgresql
rpm -qa |grep tecs

fi

#start tecs
echo -e "\n��ʼ����tecs��"

/usr/local/bin/starttecs.sh




