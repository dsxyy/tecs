#!/bin/bash

function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #忽略井号开头的注释行以及空行之后再grep过滤"key="所在的行
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep "$key[[:space:]]*="`
    if [ -z "$line" ]; then
        config_answer=""
    else
        #将第一个=号替换为空格，再删除第一个单词得到value
        config_answer=`echo $line | sed 's/=/ /' | sed -e 's/^\w*\ *//'`
    fi
}

# 功能：tecs数据库安装升级脚本
# 作者：张文剑
# 创建时间：2012.05.25
# 参数说明：
# -i 是否交互模式
# -s 数据库服务器url
# -u 连接数据库时使用的用户名称
# -p 连接数据库时使用的用户密码
# -f 存放补丁文件的目录名称，其中的补丁文件名称必须是update_*.sql，*部分是补丁版本号
# -d 要升级的数据库名称
# -t 要创建的表sql脚本
# -v 要创建的视图sql脚本

HOST="localhost"
PORT=5432
USERNAME=
PASSWD=
DBNAME=
SCHEMA_LOG_TABLE="_schema_log"
PSQL="/usr/local/pgsql/bin/psql"
PG_DUMP="/usr/local/pgsql/bin/pg_dump"
UPDATES_DIR=
CREATE_TABLES=
CREATE_VIEWS=
RETVAL=
INTERACTIVE=0
BACKUP_FILE=

export PGHOME=/usr/local/pgsql
export PGDATA=/usr/local/pgsql/data
export PATH=/usr/local/pgsql/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
#export PGUSER=postgresql 
#export PGPASSWORD=password
export PGUSER=tecs 
export PGPASSWORD=tecs
conf_dir=$1
#先存储IFS默认值
IFS_old=$IFS
IFS=$'\n'
#for line in `grep -E 'application|db' $conf_dir/*.conf | grep -v "#" `;
#do 
#    line=`echo "$line"| sed 's/.*://g' | sed 's/ *//g'`
#    cfg_name=`echo "$line"|awk -F '=' '{print$1}'`
#    cfg_value=`echo "$line"|awk -F '=' '{print$2}'`    
#    case $cfg_name in
#         db_server_url) HOST=$cfg_value ;;
#         db_server_port) PORT=$cfg_value ;;
#         db_user) USERNAME=$cfg_value ;;
#         db_passwd) PASSWD=$cfg_value ;;
#         application)DBNAME=$cfg_value ;;
#          ?) echo "error" ;;
#    esac
#done
IFS=$IFS_old

HOST=127.0.0.1
DBNAME=vnm_new
USERNAME=tecs
PASSWD=tecs

#UPDATES_DIR=/work/vnet_stor/gongxf/vnet_0220/vnet_99_code/code/network/vnm/db/sql
UPDATES_DIR=/opt/tecs/network/vnm/scripts

#CREATE_TABLES=$conf_dir/`ls $conf_dir|grep baseline.sql`
#UPDATES_DIR=$conf_dir/updates
#CREATE_VIEWS=$conf_dir/`ls $conf_dir|grep view.sql`


    local_file="/opt/tecs/network/vnm/etc/tecs.conf"
	
	get_config $local_file "db_server_url"
	if [ "$config_answer" != "" ]; then 
	    HOST="$config_answer"
	fi 
	
	get_config $local_file "db_name"
	if [ "$config_answer" != "" ]; then 
	    DBNAME="$config_answer"
	fi 
	
	get_config $local_file "db_server_port"
	if [ "$config_answer" != "" ]; then 
	    PORT="$config_answer"
	fi 
	
	get_config $local_file "db_user"
	if [ "$config_answer" != "" ]; then 
	    USERNAME="$config_answer"
	fi 
	
	get_config $local_file "db_passwd"
	if [ "$config_answer" != "" ]; then 
	    PASSWD="$config_answer"
	fi 
	
echo =====================
echo HOST=$HOST
echo USERNAME=$USERNAME
echo PASSWD=$PASSWD
echo DBNAME=$DBNAME
echo UPDATES_DIR=$UPDATES_DIR

export PGUSER=$USERNAME 
export PGPASSWORD=$PASSWD

export PGCLIENTENCODING=GBK
#echo CREATE_TABLES=$CREATE_TABLES
#echo CREATE_VIEWS=$CREATE_VIEWS
echo =====================	

    #从零开始创建最新版本的数据库
	#dropdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    createdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "createdb failed!" && exit $RETVAL
    
	#$PSQL SET CLIENT_ENCODING TO 'EUC_CN';
# create table	
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/table/table.txt -v ON_ERROR_STOP=on 1>/dev/null
    	
# create view
    $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/port.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/physical_port.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/vna.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/portgroup.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/netplane_portgroup.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/logicnetwork_portgroup_netplane.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/switch_port_vna.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/switch_vsi.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/vlanmap_netplane_pg_vlan.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/bond.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/bond_map.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/vsi.txt -v ON_ERROR_STOP=on 1>/dev/null
#	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/schedule.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/NEW_schedule.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/kernel_report.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/wildcast_create_vport.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/wildcast_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/wildcast_loopback.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_netplane.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_portgroup.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_logicnet.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_vna.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_port.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_vm.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_wildcast_create_vport.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_wildcast_loopback_port.txt -v ON_ERROR_STOP=on 1>/dev/null
	$PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/view/web_wildcast_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
	
# create procedure
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/serial.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/type.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/statistic_sriovvf_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/statistic_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vna.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vna_reg_module.txt -v ON_ERROR_STOP=on 1>/dev/null   
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_macrange.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_macpool.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_iprange.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_vlanrange.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_vlanpool.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/netplane_vlanmap.txt -v ON_ERROR_STOP=on 1>/dev/null   
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/phy_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/sriov_vf_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/uplink_loop_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/kernel_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/bond.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/bond_backup.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/bond_lacp.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/bond_map.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vswitch_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/port_ip.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/port_mac.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/port_group.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/pg_trunk_vlan_range.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/logicnetwork.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/logicnetwork_iprange.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/logicnetwork_ippool.txt -v ON_ERROR_STOP=on 1>/dev/null   
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vswitch.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vswitch_map.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vsi.txt -v ON_ERROR_STOP=on 1>/dev/null
#   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/schedule.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/NEW_schedule.txt -v ON_ERROR_STOP=on 1>/dev/null   
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/kernel_report.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/kernel_report_bond_map.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_create_vport.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_loopback_port.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_switch_bond_map.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_task_switch.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_task_loopback.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/wildcast_task_create_vport.txt -v ON_ERROR_STOP=on 1>/dev/null   
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vm_migrate.txt -v ON_ERROR_STOP=on 1>/dev/null
   $PSQL -h $HOST -p $PORT -U $USERNAME -w -q -d $DBNAME -f $UPDATES_DIR/procedure/vm.txt -v ON_ERROR_STOP=on 1>/dev/null
   
   
   
   echo "install vnm datebase success."
#
#if [[ "$1" == "/opt/tecs/tc/etc" ];then
#    /opt/tecs/tc/scripts/install_pgdb.sh -s $HOST  -u $USERNAME -p $PASSWD  -d $DBNAME -f $UPDATES_DIR -t $CREATE_TABLES -v $CREATE_VIEWS
#elif [[ "$1" == "/opt/tecs/cc/etc" ];then
#    /opt/tecs/cc/scripts/install_pgdb.sh -s $HOST  -u $USERNAME -p $PASSWD  -d $DBNAME -f $UPDATES_DIR -t $CREATE_TABLES -v $CREATE_VIEWS
#else
#    echo "error dir:$1"
#fi

