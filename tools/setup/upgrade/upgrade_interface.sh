#!/bin/bash
# 提供和yum安装相关的公共函数和变量
if [ ! "$_UPGRADE_INTERFACE_FILE" ];then
_UPGRADE_INTERFACE_DIR=`pwd`
cd $_UPGRADE_INTERFACE_DIR/../common/
.  tecs_common_func.sh
cd $_UPGRADE_INTERFACE_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh

cd $_UPGRADE_INTERFACE_DIR/../install/
.  install_func.sh

cd $_UPGRADE_INTERFACE_DIR
.  upgrade_func.sh

function upgrade_tecs
{
    local result=0
    
    # 先停止所有服务
    echo "stop all service..."
    stop_service_all
   
    #检测tc和cc的工作流表是否为空
    check_workflow "tecs-tc" "$tecs_tc_conf"
    check_workflow "tecs-cc" "$tecs_cc_conf"
    check_workflow "tecs-vnm" "$tecs_vnm_conf"

    # 检测TC,CC是否需要升级
    check_app_is_upgrade "tecs-tc"
    tc_is_update=$is_update
    check_app_is_upgrade "tecs-cc"
    cc_is_update=$is_update
    check_app_is_upgrade "tecs-vnm"
    vnm_is_update=$is_update
    
    #检测消息服务端rpm和数据库服务端rpm是否需要升级
    check_installed "qpidc-broker"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "qpidc-broker"
        [ $is_update = "yes" ] && { $tecs_yum install qpidc-broker qpidc-client && service qpid start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade qpid failed,return $result"! && exit $result
    fi
    #数据库rpm服务包处理脚本有问题，临时处理，后面待版本都升级到p13b2后可改为如上qpid的处理形式
    check_installed "postgresql"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "postgresql"
        [ $is_update = "yes" ] && { $tecs_yum remove postgresql; $tecs_yum install postgresql postgresql-client && service postgresql start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade postgresql failed,return $result"! && exit $result
        [ -e /etc/tecs/guard_list/postgresql ] || please_guard_me "postgresql"
    fi
    #检测tgtd是否需要升级
    check_installed "scsi-target-utils"
    if [ $has_installed = "yes" ];then
        check_app_is_upgrade "scsi-target-utils"
        [ $is_update = "yes" ] && { $tecs_yum install scsi-target-utils && service tgtd start || result=$?; }
        [ "$result" -ne 0 ] && echo -e "\nupgrade tgtd failed,return $result"! && exit $result
    fi
    
    #检测nicchk是否安装及升级
    check_installed "nicchk"
    if [ $has_installed = "yes" ];then
        $tecs_yum upgrade nicchk
    else
        install_nicchk_component
    fi    
    service nicchk start
    #获取当前所有tecs服务包
    get_tecs_services
    
    # 升级tecs服务包
    upgrade_rpms_by_yum "$all_tecs_services"
    
    # 升级TC的数据库
    if [[ "$tc_is_update" == "yes" ]];then
        upgrade_tc_database
    fi
    
    # 升级CC的数据库
    if [[ "$cc_is_update" == "yes" ]];then
        upgrade_cc_database
    fi
    
    # 升级VNM的数据库
    if [[ "$vnm_is_update" == "yes" ]];then
        upgrade_vnm_database
    fi

	chkconfig --list |grep iptables|grep on >/dev/null
	[ "$?" -eq 0 ] && { cd $_UPGRADE_INTERFACE_DIR/../base/; ./firewall_config.sh; cd - >/dev/null; }
		
    # 启动TECS服务
    echo "start all service..."
    start_service_all
}

_UPGRADE_INTERFACE_FILE="upgrade_interface.sh"
fi
