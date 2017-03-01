#!/bin/sh
# 作者： 高明
# 编写日期：2012.10.27
if [ ! "$_FIREWALL_INTERFACE_FILE" ];then
_FIREWALL_INTERFACE_DIR=`pwd`
#头文件包含
cd $_FIREWALL_INTERFACE_DIR/../common/
.  tecs_common_func.sh

cd $_FIREWALL_INTERFACE_DIR
# 设置防火墙
function install_firewall_component
{     
    # 打开防火墙设置
    get_install_component "firewall" "Do you want to set the firewall?"
   
    cd $_FIREWALL_INTERFACE_DIR
    if [ "$config_answer" = "yes" ]; then        
        ./firewall_config.sh               
    else
        ./firewall_config.sh "clear"
    fi
    cd - >/dev/null 

}

# 清除防火墙
function firewall_component_uninstall
{     
    local rulefile=/etc/sysconfig/iptables
    [ ! -f $rulefile ] && echo "$rulefile doesn't exist, failed to handle firewall"
    local rule_exist=`cat  $rulefile| awk '/filter/,/DROP/'|grep INPUT[[:space:]]*DROP`
    if [ "$rule_exist" != "" ];then
        get_install_component "firewall" "Do you want to clear the firewall?"
        cd $_FIREWALL_INTERFACE_DIR
        if [ "$config_answer" = "yes" ]; then        
            ./firewall_config.sh "clear"     
        else
            ./firewall_config.sh 
        fi
        cd - >/dev/null 
    fi
}

_FIREWALL_INTERFACE_FILE="firewall_interface.sh"
fi
