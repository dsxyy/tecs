#!/bin/bash
# 
if [ ! "$_UPGRADE_FUNC_FILE" ];then
#ͷ�ļ�����
_UPGRADE_FUNC_DIR=`pwd`
cd  $_UPGRADE_FUNC_DIR/../common/
.  tecs_common_func.sh
cd  $_UPGRADE_FUNC_DIR/../base/
.  pgdb_interface.sh

cd $_UPGRADE_FUNC_DIR

# ��ȡ��ǰ��װ������tecs������б�
function get_tecs_services
{    
    all_tecs_services=""
    for service in `ls /etc/init.d/ |grep "^tecs-"`
    do
        all_tecs_services="$all_tecs_services $service"
    done
}

#����ֵΪ���������й������ĸ���
function check_workflow
{
    local component=$1
    local tecs_conf=$2
       
    if [ -e /etc/init.d/${component} ];then
        [ -e "$tecs_conf" ] || { echo "$tecs_conf has lost"; exit 1; }
        echo "$component installed, begin to check $component database workflow table..."
    else
        return
    fi
     
    #���������ļ���ȡ���ݿ���Ϣ
    get_db_info "$tecs_conf"
    #���û�������
    set_db_env
    #�ж����ݿ��Ƿ����   
    is_db_exist  "$component" "$tecs_conf" || exit 1
    [ "$db_exist" = "" ] && { echo "no database named \"$db_name\" exists."; exit 1; }   
    # ��ѯ���������Ƿ���ڣ����������˳�����ִ��
    local result1=`psql -d $db_name -h $db_server -p $db_port -c "select * from pg_statio_user_tables where relname='workflow'" -t`
    [ "$result1" = "" ] && { echo "no workflow table exists."; return 0; }
    local result2=`psql -d $db_name -h $db_server -p $db_port -c "select count(*) from workflow" -t`
    [ "$result2" -eq 0 ] && return 0
    # ��ѯ���ӦΪ������������������Ϊ��ѯʧ��
    (( $result2 ))
    [ "$?" -ne 0 ] && { echo "error, psql cmd return non-integer data, return value is \"$result2\""; exit 1; }
    if [ "$result2" -ne 0 ];then
        echo "$result2 workflow exist in ${component} database, update ${component} failed, please try it later."; 
        exit 1
    fi
}

_UPGRADE_FUNC_FILE="upgrade_func.sh"

fi
