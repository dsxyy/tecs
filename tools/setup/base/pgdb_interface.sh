#!/bin/bash
# 数据库组件对外接口函数

if [ ! "$_PGDB_INTERFACE_FILE" ];then
_PGDB_INTERFACE_DIR=`pwd`
cd $_PGDB_INTERFACE_DIR/../common/
.  tecs_global_var.sh
.  tecs_common_func.sh

cd $_PGDB_INTERFACE_DIR


#########################################################################
# TECS DB配置
#########################################################################
# db_server_url选项配置
function config_db_server_url
{
    local component=$1
    
    get_install_config "$component" "db_server_url" "string" "Please input database server url" ""
    update_config $2 "db_server_url" "$config_value"
}

# db_user and db_passwd 选项配置
function config_db_user_and_pwd
{
    local component=$1
    local file=$2
    
    get_install_config "$component" "db_user" "string" "Please input database login username:" ""
    update_config $file "db_user" "$config_value"
    
    get_install_config "$component" "db_passwd" "string" "Please input database login password:" "" 
    update_config $file "db_passwd" "$config_value"
}

#########################################################################
# TECS 数据库组件入口配置
#########################################################################
function config_postgre
{
    local config_file=/usr/local/pgsql/share/auto_config
    
    rm -f $config_file   
    
    get_install_config "postgresql" "standby" "bool" "Specify standby PostgreSQL database configure?" ""
    #对于bool类型的变量必须填小写的yes/no
    echo standby=$config_value >$config_file
    
    get_install_config "postgresql" "port" "string" "Specify PostgreSQL server port" ""
    echo port=$config_value >>$config_file
    
    get_install_config "postgresql" "password" "string" "Specify superuser password" ""
    echo password=$config_value >>$config_file
    pgpassword=$config_value
    
    get_install_config "postgresql" "boot" "bool" "Do you want PostgreSQL server to be started on boot" ""
    echo boot=$config_value >>$config_file
    
    get_install_config "postgresql" "backup" "bool" "Do you want to schedule backups database?" ""
    echo backup=$config_value >>$config_file
    
    service postgresql start $config_file
}
function create_db_user
{
    local COUNTER=0
    local ISSTARTUP=`service postgresql status | grep "server is running"`
    while [ $COUNTER -lt 30 ] && [ -z "$ISSTARTUP" ]
    do
      sleep 1
      ISSTARTUP=`service postgresql status | grep "server is running"`
      COUNTER=`expr $COUNTER + 1`
    done

    sleep 5
    if [ ! -z "$ISSTARTUP" ]; then
       local ISMASTER=`/usr/local/pgsql/bin/config_postgresql.sh msstatus|grep production`
       sleep 5
       if [ ! -z "$ISMASTER" ];then
            export PGHOME=/usr/local/pgsql
            export PGDATA=/usr/local/pgsql/data
            export PATH=/usr/local/pgsql/bin:$PATH
            export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
            export PGUSER=postgresql
            export PGPASSWORD=$pgpassword
            export PGDATABASE=postgres
            export PGPORT=5432
            #创建tecs用户
            local result=`psql -w -c"CREATE ROLE tecs with PASSWORD "\'"md5475d33f764605fbcb226c5e686251d7f"\'" SUPERUSER CREATEDB CREATEROLE INHERIT LOGIN;"`
        fi
    fi
}
#########################################################################
# TECS 数据库组件入口配置
#########################################################################
function config_postgresql
{

        config_postgre

    if [ 0 -ne $? ]; then
        echo "failed to config database server!"
        exit
    fi

    if [ ! -f /usr/local/pgsql/share/no_standby ]; then
      sleep 8
    fi

    ISMASTER=`/usr/local/pgsql/bin/config_postgresql.sh msstatus|grep production`
    if [ ! -z "$ISMASTER" ]; then
      create_db_user
    fi
}

#install db 组件rpm包安装
function db_component_rpm_install
{
    local component="postgresql"
    check_and_install_rpm  $component
    if [ "$?" -eq 0 ];then
        config_postgresql
        please_guard_me $component
    fi
}
# 数据库组件的卸载，应该放在tc\cc\vnm后面卸载
function db_component_rpm_uninstall
{
    local component="postgresql"

    check_uninstall_tecs_rpm $component
    if [ $? -eq 0 ]; then
        uninstall_rpm_by_yum  $component
        rm -rf /etc/tecs/guard_list/$component
    fi
}
function install_db_component
{
    local component="postgresql"
    get_install_component "$component" "Install tecs database server?"
    [[ $answer == "no" ]] && return 0
    db_component_rpm_uninstall
    db_component_rpm_install
}

#########################################################################
# TECS 数据库升级脚本区
#########################################################################
# TC数据库升级脚本
function upgrade_tc_database
{
     if [ "$operation" = "install" -a "$mode" = "custom" ];then      
        service tecs-tc upgrade_db
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-tc database!"
            exit 1
        fi
       
    else
        service tecs-tc upgrade_db_auto
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-tc database!"
            exit 1
        fi
    fi
}

# CC数据库升级脚本
function upgrade_cc_database
{
    if [ "$operation" = "install" -a "$mode" = "custom" ];then 
        service tecs-cc upgrade_db
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-cc database!"
            exit 1
        fi
      
    else
        service tecs-cc upgrade_db_auto
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-cc database!"
            exit 1
        fi
    fi
}

# VNM 数据库升级脚本
function upgrade_vnm_database
{
    if [ "$operation" = "install" -a "$mode" = "custom" ];then 
        service tecs-vnm upgrade_db
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-vnm database!"
            exit 1
        fi

    else
        service tecs-vnm upgrade_db_auto
        if [ 0 -ne $? ]; then
            echo "failed to upgrade tecs-vnm database!"
            exit 1
        fi 
    fi
}

#获取数据库信息
function get_db_info
{
    local tecs_component_conf=$1
    # local operate="$2"
    [ -e "$tecs_component_conf" ] || { echo "$tecs_component_conf has lost"; return 1; }

    # 获取安装服务数据库相关数据
    # 优先从db_name中获取数据库名字，如果没有才考虑application
    db_name_exist=`cat $tecs_component_conf | grep ^[[:space:]]*db_name[[:space:]]*=`
    if [ "$db_name_exist" != "" ];then
        get_config $tecs_component_conf "db_name"
    else
        get_config $tecs_component_conf "application"
    fi
    [ "$config_answer" != "" ] && db_name=$config_answer || { echo -e "database name doesn't exist in $tecs_component_conf"; return 1; }

    get_config $tecs_component_conf "db_server_url"
    [ "$config_answer" != "" ] && db_server=$config_answer || { echo -e "\"db_server_url\" value doesn't exist in $tecs_component_conf"; return 1; }
    get_config $tecs_component_conf "db_server_port"
    [ "$config_answer" != "" ] && db_port=$config_answer || db_port=5432
    get_config $tecs_component_conf "db_user"
    [ "$config_answer" != "" ] && db_user=$config_answer || { echo -e "\"db_user\" value doesn't exist in $tecs_component_conf"; return 1; }
    get_config $tecs_component_conf "db_passwd"
    [ "$config_answer" != "" ] && db_passwd=$config_answer || { echo -e "\"db_passwd\" value doesn't exist in $tecs_component_conf"; return 1; }
    
    return 0
}
#设置环境变量，应先执行set_db_env
function set_db_env
{
    export PGHOME=/usr/local/pgsql
    export PGDATA=/usr/local/pgsql/data
    export PATH=/usr/local/pgsql/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
    export PGUSER=$db_user
    export PGPASSWORD=$db_passwd
}
#判断数据库是否存在，应先执行get_db_info和set_db_env
function is_db_exist
{
    local component=$1
    local tecs_component_conf=$2
    
    #数据库查询
    db_exist=`psql -h$db_server -p $db_port -U$db_user -dtemplate1 -w -c"select datname from pg_database where datname="\'""$db_name""\'";" | grep "$db_name"`
    RET=$?    
    [ 0 -ne $RET ] && { echo "query database failed for $component!"; db_exist="";  }
	return $RET
}
#如果正在有客户端连接着数据库，提示用户先断开，否则没法删除或改名，应先执行get_db_info和set_db_env
function is_db_free
{    
    local client_number
    local counter
    client_number=`psql -w -dtemplate1 -c "SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$db_name""\'";"| head -n 3 | tail -n 1`
    if [ -z $client_number ]; then
        client_number=0
    fi
    
    counter=0
    while [ $counter -lt 3 ] && [ $client_number -gt 0 ]
    do
        echo "database $db_name is being accessed by other users, can not edit!!!"
        echo "you should do the following steps:"
        echo "step 1: service guard stop"
        echo "step 2: service tc(or cc or vnm) stop"
        echo "step 3: exit psql CLI shell and pgAdmin GUI client"
        echo -e "After done, press any key to continue ...\c "
        read answer  
        client_number=`psql -w -dtemplate1 -c"SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$db_name""\'";"| head -n 3 | tail -n 1`
        if [ -z $client_number ]; then
            client_number=0
        fi          
        counter=`expr $counter + 1`
    done
}
#删除数据库，应先执行get_db_info和set_db_env
function clear_db
{
    local component=$1
    local tecs_component_conf=$2
    
    is_db_exist  "$component" "$tecs_component_conf"  
    [ "$db_exist" = "" ] && { echo "no database named \"$db_name\" exists."; return 0; }   
    
    is_db_free
    echo "will drop database $db_name"   
    dropdb -h "$database_server" -p $db_port -U "$db_user" -w "$db_name"
    local RETVAL=$?
    [ 0 -ne $RETVAL ] && echo "failed to drop database $db_name of $component." 
}

#重命名数据库，应先执行get_db_info和set_db_env
function rename_db
{
    local component=$1
    local tecs_component_conf=$2
    
    is_db_exist "$component" "$tecs_component_conf"  
    [ "$db_exist" = "" ] && { echo "no database named \"$db_name\" exists."; return 0; }  
    
    is_db_free
    
    local bak_name="${db_name}_bak_${current_time}"
    #将名字中的中划线替换为下划线
    bak_name=`echo $bak_name|tr '-' '_'`  
    echo "rename database $db_name to $bak_name"        
    psql -h$db_server -p $db_port -U$db_user -dtemplate1 -w -c"alter database "$db_name" rename to "$bak_name""  
    local RETVAL=$?    
    [ 0 -ne $RETVAL ] && echo "failed to rename database $db_name of $component." 

}

_PGDB_INTERFACE_FILE="pgdb_interface.sh"
fi
