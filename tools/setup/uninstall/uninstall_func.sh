#!/bin/bash
# 
if [ ! "$_UNINSTALL_FUNC" ];then
_UNINSTALL_FUNC_DIR=`pwd`
cd $_UNINSTALL_FUNC_DIR/../common
.  tecs_common_func.sh
cd $_UNINSTALL_FUNC_DIR/../base/
.  qpid_interface.sh
.  pgdb_interface.sh

cd $_UNINSTALL_FUNC_DIR


function operate_db
{
    local component="$1"
    local tecs_component_conf="$2"
    
    PS3="Please select an operation to process the datebase: " # 设置提示符字串.
    select db_operation in "rename" "drop" "nochange" "help"
    do
        echo
        echo "you select an operation: $db_operation."
        echo
        break  
    done   
    
    answer=$db_operation

    case $answer in
        "nochange")
        ;; 
        "rename")
        #确保之前已经执行过get_db_info和set_db_env
        rename_db "$component" "$tecs_component_conf"
        ;; 
        "drop")
        #确保之前已经执行过get_db_info和set_db_env
        clear_db "$component" "$tecs_component_conf"
        ;; 
        "help")
        echo "nochange: nothing to do"
        echo "rename: will reserve the database, and rename it to \"name__bak__'date'\""
        echo "drop: will delete the database."
        operate_db "$component" "$tecs_component_conf"
        ;;    
        *)
        echo "unknown install argument: $answer!"
        operate_db "$component" "$tecs_component_conf"
        ;;
    esac
}

#卸载过程中数据库处理
function db_process_uninstall
{  
    local component="$1"
    local tecs_component_conf="$2"
  
    #根据配置文件获取数据库信息
    get_db_info "$tecs_component_conf" || { echo "failed to process database of $component"; return 1; } 

    #设置环境变量
    set_db_env
    is_db_exist $component $tecs_component_conf
    if [ "$db_exist" != "" ];then
        echo -e "\n$component database exist, named \"$db_name\"."
    else
        return 0
    fi
    
    operate_db "$component" "$tecs_component_conf"
}

# 卸载tc组件
function tc_component_rpm_uninstall
{   
    local app="tecs-tc"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        db_process_uninstall "$app" "$tecs_tc_conf"
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# 卸载CC组件
function cc_component_rpm_uninstall
{
    local app="tecs-cc"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        db_process_uninstall "$app" "$tecs_cc_conf"
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# 卸载HC组件
function hc_component_rpm_uninstall
{
    local app="tecs-hc"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
        # 清除老的映射关系
        sed -i "/$cloudname/d" /etc/hosts
    fi
}
# wui组件卸载
function wui_component_rpm_uninstall
{
    local app="tecs-wui"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app 
    fi
}
# guard组件的卸载
function guard_component_rpm_uninstall
{
    local app="tecs-guard"
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $app
    fi
}

# nicchk组件的卸载
function nicchk_component_rpm_uninstall
{
    local app="nicchk"
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $app
    fi
}


# 卸载vna组件
function vna_component_rpm_uninstall
{   
    local app="tecs-vna"
    local uplink_port
    local nic_name="dvs_kernel"
    local br_name="sdvs_00"
    
    ifconfig virbr0 down > /dev/null 2>&1
    brctl delbr virbr0  > /dev/null 2>&1
    
    ifconfig br_wd down > /dev/null 2>&1
    brctl delbr br_wd  > /dev/null 2>&1
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# 卸载vnm组件
function vnm_component_rpm_uninstall
{   
    local app="tecs-vnm"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        db_process_uninstall "$app" "$tecs_vnm_conf"
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# 卸载wdg组件
function wdg_component_rpm_uninstall
{   
    local app="tecs-wdg"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# 卸载SD组件
function sd_component_rpm_uninstall
{
    local app="tecs-sd"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
    fi
}

# tgtd通讯组件卸载
function tgtd_service_component_rpm_uninstall
{
    local app="scsi-target-utils"

    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        killproc_by_name "tgtd"
        #第三方包卸载函数与tecs卸载函数不同，否则会多卸载
        uninstall_rpm_by_yum  $app
        dont_guard_me "tgtd"
    fi
}

# 存储组件的卸载
function storage_component_rpm_uninstall
{
    local app="tecs-sa"

    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $app 
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}

_UNINSTALL_FUNC="uninstall_func.sh"
fi
