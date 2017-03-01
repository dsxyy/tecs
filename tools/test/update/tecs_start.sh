#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：installandstart.sh
# 文件标识：
# 内容摘要：安装tecs环境RPM包和启动各个进程
# 当前版本：2.0
# 作    者：彭伟
# 完成日期：2011年11月17日
#
# 修改记录1：
#     修改日期：2011/11/17
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建
# 修改记录2：
#     修改日期：2012/01/18
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：修改为只是升级时使用
# 修改记录3：
#     修改日期：2012/02/09
#     版 本 号：V3.0
#     修 改 人：彭伟
#     修改内容：支持自动安装方式--无用户输入时8s自动选择默认设置安装
# 修改记录4：
#     修改日期：2012/03/01
#     版 本 号：V4.0
#     修 改 人：彭伟
#     修改内容：1)增加自动卸载qpidc;2)取消tecs用户创建；3）启动调用starttecs.sh。
# 修改记录4：
#     修改日期：2012/03/16
#     版 本 号：V5.0
#     修 改 人：彭伟
#     修改内容：增加sed方式修改pg_hba.conf允许访问
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

#echo -e "版本开始升级，会先卸载旧版本再安装新版本!\n如果确认，请输入y；否则请输入n，只是启动tecs。"
#read answer
read_bool "版本开始升级，会先卸载旧版本再安装新版本!\n如果确认，请输入y；否则请输入n，只是启动tecs。" "yes" 

#first install
if [ "$answer" = "yes" ]; then
echo "开始安装tecs！"

export tecsbin=`find . -name "installtecs*"`

#[install tecs]
if [ `find . -name "installtecs*"` ]; then
echo $tecsbin "uninstall"
$tecsbin uninstall
#qpidc-client、devel uninstall
rpm -e qpidc-devel-0.12-1.x86_64
rpm -e qpidc-client-0.12-1.x86_64
rpm -e qpidc-0.12-1.x86_64

#drop database
read_bool "是否删除旧数据库，请输入y/n。" "no" 
if [ "$answer" = "yes" ]; then
echo -e "rm -rf /usr/local/pgsql——已经删除整个目录。\n"
rm -rf /usr/local/pgsql
sleep 1
fi

#删除旧HOST注册CC信息
read_bool "是否删除/dev/shm/tecs下的旧HOST注册信息，请输入y/n。" "no" 
if [ "$answer" = "yes" ]; then
echo -e "rm -rf /dev/shm/tecs/*\n"
rm -rf /dev/shm/tecs/*
sleep 1
fi

#删除旧虚拟机实例
read_bool "是否删除/var/lib/tecs/instances下的旧虚拟机实例，请输入y/n。" "no" 
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
echo "请cd到版本目录下执行或者把tecs_start.sh拷贝到版本目录下执行，谢谢！"
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
echo -e "\n开始启动tecs！"

/usr/local/bin/starttecs.sh




