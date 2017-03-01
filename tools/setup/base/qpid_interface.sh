#!/bin/bash

if [ ! "$_QPID_INTERFACE_FILE" ];then
_QPID_INTERFACE_DIR=`pwd`
#头文件包含
cd $_QPID_INTERFACE_DIR/../common/
.  tecs_global_var.sh
.  tecs_common_func.sh

cd $_QPID_INTERFACE_DIR

# 根据一个应用名称，杀死该应用所有的进程
function killproc_by_name
{
    local app=$1
    
    for pid in `ps -ef | grep $app | grep -v grep | awk '{print $2}'`
    do
        echo "kill -9 $pid $app!"
        kill -9 $pid
    done
}

#########################################################################
# TECS 消息服务器配置 ，基本不怎么要修改的
#########################################################################
function config_qpid_broker
{
    local component=$1
    local file=$2
    local default_value=
    
    #获取配置参数
    get_install_config "$component" "config_HA" "bool" "Create tecs message server HA cluster?" ""
   
    if [ $config_value == "yes" ]; then
        get_install_config "$component" "cluster_name" "string" "Please input cluster name?" ""
        update_config  $file "cluster-name" "$config_value"
    else
        update_config  $file "cluster-name" "#"
    fi

    get_install_config "$component" "message_log" "bool" "Create tecs message server log?" "$default_value"
    
    if [ "$config_value" == "yes" ]; then
        mkdir -p /var/log/tecs/qpid
        update_config  $file "log-to-file" "/var/log/tecs/qpid/qpidd.log"
        update_config  $file "log-enable" "trace+"
    else
        update_config  $file "log-to-file" "#"
        update_config  $file "log-enable" "#"
    fi

}
# 消息服务器配置
function config_message_broker_url
{
    local component=$1
    local file=$2    
    local default_value=
  
    #这里比较特殊，由菜单选择临时输入的消息服务器latest_message_server，是为compute和sa安装模式特别定制的。
    if [ -z "$latest_message_server" ];then
        get_install_config "$component" "message_broker_url" "string" "Please input message server url" ""
        message_server=$config_value
    else
        echo -e "Please input message server url (default: ): $latest_message_server"
        #将当前配置保存起来
        user_config_save "$component" "message_broker_url" "$latest_message_server"
        message_server=$latest_message_server
    fi
    
    update_config $file "message_broker_url" "$message_server"
    
}

function config_two_message_broker
{
    local component=$1
    local file=$2
    local node_a=$3
    local node_b=$4

    get_install_config "$component" "multi_message_server" "bool" "Config multi message server for $component? " ""
   
    if [ $config_value == "yes" ]; then
        get_install_config "$component" "message_broker_url" "string" "Please input message server url between $component and $node_a" ""
        update_config $file "message_broker_url" "$config_value"

        get_install_config "$component" "message_broker_url2" "string" "Please input message server url between $component and $node_b,not same as the one above" ""
        update_config $file "message_broker_url2" "$config_value"
        update_config $file "message_broker_port2" "5672"
        update_config $file "broker_option2" "{reconnect-urls:[\"localhost:5682\"]}"
    else
        config_message_broker_url $component $file
        update_config $file "message_broker_url2" "#"
    fi
}

#install 消息服务器 组件rpm包安装
function qpid_service_component_rpm_install
{
    local app="qpidc-broker"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && please_guard_me "qpid" && config_qpid_broker "$app" "/usr/local/etc/qpidd.conf"
}

# qpid通讯组件卸载
function qpid_service_component_rpm_uninstall
{
    local app="qpidc-broker"

    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        killproc_by_name "qpid"
        uninstall_rpm_by_yum  $app
        dont_guard_me "qpid"
    fi
}


function install_qpid_service_component
{  
    local app="qpidc-broker"

    get_install_component "$app" "Install tecs message server?"
    [[ $answer == "no" ]] && return 0

    qpid_service_component_rpm_uninstall
    qpid_service_component_rpm_install
}

_QPID_INTERFACE_FILE="qpid_interface.sh"
fi
