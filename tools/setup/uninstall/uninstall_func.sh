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
    
    PS3="Please select an operation to process the datebase: " # ������ʾ���ִ�.
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
        #ȷ��֮ǰ�Ѿ�ִ�й�get_db_info��set_db_env
        rename_db "$component" "$tecs_component_conf"
        ;; 
        "drop")
        #ȷ��֮ǰ�Ѿ�ִ�й�get_db_info��set_db_env
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

#ж�ع��������ݿ⴦��
function db_process_uninstall
{  
    local component="$1"
    local tecs_component_conf="$2"
  
    #���������ļ���ȡ���ݿ���Ϣ
    get_db_info "$tecs_component_conf" || { echo "failed to process database of $component"; return 1; } 

    #���û�������
    set_db_env
    is_db_exist $component $tecs_component_conf
    if [ "$db_exist" != "" ];then
        echo -e "\n$component database exist, named \"$db_name\"."
    else
        return 0
    fi
    
    operate_db "$component" "$tecs_component_conf"
}

# ж��tc���
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
# ж��CC���
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
# ж��HC���
function hc_component_rpm_uninstall
{
    local app="tecs-hc"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
        # ����ϵ�ӳ���ϵ
        sed -i "/$cloudname/d" /etc/hosts
    fi
}
# wui���ж��
function wui_component_rpm_uninstall
{
    local app="tecs-wui"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app 
    fi
}
# guard�����ж��
function guard_component_rpm_uninstall
{
    local app="tecs-guard"
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $app
    fi
}

# nicchk�����ж��
function nicchk_component_rpm_uninstall
{
    local app="nicchk"
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $app
    fi
}


# ж��vna���
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
# ж��vnm���
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
# ж��wdg���
function wdg_component_rpm_uninstall
{   
    local app="tecs-wdg"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
        check_depend "tecs-common" || uninstall_rpm_by_yum  "tecs-common"
    fi
}
# ж��SD���
function sd_component_rpm_uninstall
{
    local app="tecs-sd"
    
    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ];then
        uninstall_rpm_by_yum  $app
    fi
}

# tgtdͨѶ���ж��
function tgtd_service_component_rpm_uninstall
{
    local app="scsi-target-utils"

    check_uninstall_tecs_rpm $app
    if [ $? -eq 0 ]; then
        killproc_by_name "tgtd"
        #��������ж�غ�����tecsж�غ�����ͬ��������ж��
        uninstall_rpm_by_yum  $app
        dont_guard_me "tgtd"
    fi
}

# �洢�����ж��
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
