#!/bin/bash
# 
if [ ! "$_UNINSTALL_INTERFACE_FILE" ];then
_UNINSTALL_INTERFACE_DIR=`pwd`
cd $_UNINSTALL_INTERFACE_DIR/../common
.  tecs_common_func.sh
cd $_UNINSTALL_INTERFACE_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh
.  firewall_interface.sh

cd $_UNINSTALL_INTERFACE_DIR
.  uninstall_func.sh

function clean
{
    $_UNINSTALL_INTERFACE_DIR/clear.sh
}

function uninstall_tecs
{
    echo "Will uninstall rpm which has been install in the machines"
    
    # 先停止所有服务
    echo "stop all service..."
    stop_service_all
    
    vna_component_rpm_uninstall
    vnm_component_rpm_uninstall
    tc_component_rpm_uninstall
    cc_component_rpm_uninstall
    hc_component_rpm_uninstall

    wdg_component_rpm_uninstall
    sd_component_rpm_uninstall
    storage_component_rpm_uninstall
    tgtd_service_component_rpm_uninstall
    
    wui_component_rpm_uninstall
    nicchk_component_rpm_uninstall
    guard_component_rpm_uninstall
    #数据库模块应该放在tc\cc\vnm后面卸载
    db_component_rpm_uninstall
    qpid_service_component_rpm_uninstall
    firewall_component_uninstall
    # 清理exports文件
    clear_nfs_exports

    echo "Finish uninstall work,will exit!"
   
    ask_service_start_all
   
}

_UNINSTALL_INTERFACE_FILE="uninstall_interface.sh"
fi
