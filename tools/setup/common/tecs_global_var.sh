#!/bin/bash
#1.最高全局变量，可能会被所有其他脚本调用，export后，也可被子进程中的执行的脚本调用
#2.所有脚本中不得有和这些变量名字冲突的变量
#3.请不要给这里的全局变量随意赋值，有些初始化必须为空 

#防止脚本重复被包含
if [ ! "$_GLOBAL_VAR_FILE" ];then

#tecs安装向导操作时间记录
export current_time=""
#TECS操作，包括install、uninstall、clean、upgrade等
export operation=""
#安装中的具体模式
export mode=""
#yum命令封装
export tecs_yum=""

#tecs安装文件目录
export tecs_install_path="/home/tecs_install"

#用户指定配置文件
export user_specify_config=""
#tecs安装包中自带的配置文件
export tecs_publish_config=""
#tecs安装过程中保存的本次配置路径及文件
export user_config_save_path=${tecs_install_path}
export user_config_save_file=""

#数据库用户名密码
export dbusername=tecs
export dbpassword=tecs

#tecs组件配置文件
export tecs_tc_conf=/opt/tecs/tc/etc/tecs.conf
export tecs_cc_conf=/opt/tecs/cc/etc/tecs.conf
export tecs_hc_conf=/opt/tecs/hc/etc/tecs.conf
export tecs_sa_conf=/opt/tecs/sa/etc/tecs.conf
export tecs_vnm_conf=/opt/tecs/network/vnm/etc/tecs.conf

#nicchk配置文件
export nicchk_conf=/etc/nicchk/nicchk.conf

export _GLOBAL_VAR_FILE="global_var.sh"
fi
