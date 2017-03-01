#/bin/sh
# 作者： 李孝成
# 编写日期：2012.10.26
# 在测试目标机上本地运行，用于在本机安装、启动tecs
source ../../setup/common/tecs_common_func.sh

[ -z $1 ] && echo "no tecs ci directory!" && exit -1
[ ! -z $1 ] && tecsdir=$1
[ ! -e $tecsdir ] && echo "tecs ci directory $tecsdir does not exist!" && exit -1

echo "*******************************************************************************"
echo "$0 will install and start tecs on local host ..." 
echo "*******************************************************************************"

# 停止所有服务
service guard stop
service tecs-wui stop
service tecs-tc stop
service tecs-cc stop
service tecs-hc stop
service tecs-sa stop
service tecs-sd stop

# 先删除TC的数据库
get_config "/opt/tecs/tc/etc/tecs.conf" "db_server_url"
database_server=$config_answer
get_config "/opt/tecs/tc/etc/tecs.conf" "db_user"
dbusername=$config_answer    
get_config "/opt/tecs/tc/etc/tecs.conf" "db_passwd"
dbpassword=$config_answer    
get_config "/opt/tecs/tc/etc/tecs.conf" "db_name"
tcbasename=$config_answer  
if [[ $tcbasename == "" ]]; then
    get_config "/opt/tecs/tc/etc/tecs.conf" "application"
    tcbasename=$config_answer 
fi
get_config "/opt/tecs/cc/etc/tecs.conf" "db_name"
ccbasename=$config_answer  
if [[ $ccbasename == "" ]]; then
    get_config "/opt/tecs/cc/etc/tecs.conf" "application"
    ccbasename=$config_answer 
fi

export PGHOME=/usr/local/pgsql
export PGDATA=/usr/local/pgsql/data
export PATH=/usr/local/pgsql/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
export PGPASSWORD=$dbpassword
 
# 如果数据库服务存在，才执行下面语句 
if [[ -e /usr/local/pgsql/bin ]]; then
    #已经存在的数据库需要先删除，清理前先停止数据库，防止有占用的链接
    is_exist=`psql -h$database_server -p 5432 -U$dbusername -dtemplate1 -w -c"select datname from pg_database where datname="\'""$tcbasename""\'";" | grep "$tcbasename"`
    if [ ! -z "$is_exist" ]; then
        echo "datebase $tcbasename already exist, drop it ..."
        service postgresql restart
        dropdb -h "$database_server" -p 5432 -U "$dbusername" -w "$tcbasename"
        RET=$?
        [ 0 -ne $RET ] && echo "failed to dropdb $tcbasename!" && exit $RET
    fi

    is_exist=`psql -h$database_server -p 5432 -U$dbusername -dtemplate1 -w -c"select datname from pg_database where datname="\'""$ccbasename""\'";" | grep "$ccbasename"`
    if [ ! -z "$is_exist" ]; then
        echo "datebase $ccbasename already exist, drop it ..."
        dropdb -h "$database_server" -p 5432 -U "$dbusername" -w "$ccbasename"
        RET=$?
        [ 0 -ne $RET ] && echo "failed to dropdb $ccbasename!" && exit $RET
    fi
fi

# 清空数据库的日志
cat /dev/null > /usr/local/pgsql/data/pg_log/startup.log
# 清空TECS的日志
rm -fR /var/log/tecs/

# 清理TECS安装环境
$tecsdir/installtecs_el6_x86_64.bin clean

[  -e /etc/cpu_bench_result ] && mkdir -p /opt/tecs/hc/etc/
[  -e /etc/cpu_bench_result ] && cp /etc/cpu_bench_result /opt/tecs/hc/etc/
$tecsdir/installtecs_el6_x86_64.bin install all
stop_service_all

if [ $? -ne 0 ]; then
    echo "$tecsdir/installtecs_el6_x86_64.bin install_all stop failed!"
    exit -1
fi

#备份一个，省得每次都计算，太浪费时间
cp -f /opt/tecs/hc/etc/cpu_bench_result /etc/cpu_bench_result


exit 0


