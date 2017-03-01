#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 在测试目标机上本地运行，用于在本机安装、启动tecs
# 此脚本由ci server通过ssh的方式在目标机的远程执行
#
# 修改记录1：
#     修改日期：2012/4/12
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：原版本为ci\agent_tools\start.sh，修改支持系统集成测试。
# 修改记录2：
#     修改日期：2012/8/3
#     版 本 号：V2.1
#     修 改 人：xujie
#     修改内容：对齐修改

if [ -f /etc/init.d/functions ] ; then
  . /etc/init.d/functions
fi

[ -z $1 ] && echo "no tecs ci directory!" && exit -1
[ ! -z $1 ] && tecsdir=$1
[ ! -e $tecsdir ] && echo "tecs ci directory $tecsdir does not exist!" && exit -1

#测试tecs所用的各个对象名称，这里需要和python写的xmlrpc测试脚本保持一致！
tcname="st_cloud"
ccname="st_cluster"
hcname="st_host"
message_broker_url="127.0.0.1"
db_server_url="127.0.0.1"
db_user="tecs"
db_passwd="tecs"
evb_enable="no"

#用于编译tecs的key=value配置文件，修改指定key的value
function update_config
{
    local file=$1
    local key=$2
    local value=$3

    [ ! -e $file ] && return
    
    #echo update key $key to value $value in file $file ...
    local exist=`grep "^[[:space:]]*[^#]" $file | grep -c "$key[[:space:]]*=[[:space:]]*.*"`
    #注意：如果某行是注释，开头第一个字符必须是#号!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`
    
    if [ $exist -gt 0 ];then
        #如果已经存在未注释的有效配置行，直接更新value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key = $value#" $file
    elif [ $comment -gt 0 ];then
        #如果存在已经注释掉的对应配置行，则去掉注释，更新value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key = $value@" $file
    else
        #否则在末尾追加有效配置行
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key = $value" >> $file
    fi
}

#获取大网ip地址
function get_public_ip
{
    local interface=`route | grep default | awk -F' ' '{print $8}'`
    public_ip=`ifconfig "$interface" | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
    echo "public ip = $public_ip"
}

#配置image manager
function config_image_manager
{
    local file=$1
    nfsshare="/var/lib/tecs/image_repo"
    [ ! -e "$nfsshare" ] && mkdir -p "$nfsshare"
    get_public_ip

    update_config  "$file" "nfs_server_ip" "$public_ip"
    update_config  "$file" "nfs_server_dir" "$nfsshare"
}

#配置image agent
function config_image_agent
{
    local file=$1
    nfsshare="/var/lib/tecs/image_agent"  
    [ ! -e "$nfsshare" ] && mkdir -p "$nfsshare"
    get_public_ip

    update_config  "$file" "image_agent_mode" "nfs"
    update_config  "$file" "image_auto_cache" "yes"
    update_config  "$file" "nfs_agent_ip" "$public_ip"
    update_config  "$file" "nfs_agent_dir" "$nfsshare"
}

function install_apache
{
    local webpath=/usr/local/abyss
    cd tmp
    if [ ! -e ./apache.tar.gz ]; then
        echo "no apache files packaged!"
        return
    fi
    
    [ ! -e $webpath ] && mkdir -p $webpath
    
    echo "install apache to $webpath ..."
    tar -xzf ./apache.tar.gz -C $webpath
    if [ 0 -ne $? ]; then
        echo "failed to install apache!"
        return
    fi
    cd ..
    echo "apache installed!"  
    
    if grep -q /usr/local/abyss/apache/bin/httpd /etc/rc.d/rc.local;  then
        echo "apache auto start is already append to rc.local"
    else
        echo /usr/local/abyss/apache/bin/httpd -d /usr/local/abyss/apache/ -k start >> /etc/rc.d/rc.local
    fi

    /usr/local/abyss/apache/bin/httpd -d /usr/local/abyss/apache -k start
}

#创建数据库之前一定要先删除掉同名的数据库
function config_db
{
    local dbname=$1
    local tables=$2
    local views=$3
    local updates=$4
    
    cd tmp
    [ -z "$dbname" ]  && exit 1
    [ ! -e "$tables" ] && exit 1
    [ ! -e "$views" ] && exit 1
    [ ! -d "$updates" ] && exit 1
    
    #set env variable             
    export PGHOME=/usr/local/pgsql
    export PGDATA=/usr/local/pgsql/data
    export PATH=/usr/local/pgsql/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
    export PGPASSWORD=$db_passwd

    #已经存在的数据库需要先删除
    is_exist=`psql -h$db_server_url -p 5432 -U$db_user -dtemplate1 -w -c"select datname from pg_database where datname="\'""$dbname""\'";" | grep "$dbname"`
    if [ ! -z "$is_exist" ]; then
        echo "datebase $dbname already exist, drop it ..."
        dropdb -h "$db_server_url" -p 5432 -U "$db_user" -w "$dbname"
        RET=$?
        [ 0 -ne $RET ] && echo "failed to dropdb $dbname!" && exit $RET
    fi
    
    #安装并升级数据库，执行install_pgdb.sh脚本时不能带-i参数，非交互式
    chmod +x ./install_pgdb.sh
    ./install_pgdb.sh -s $db_server_url -u $db_user -p $db_passwd -t $tables -v $views  -f $updates -d $dbname
    RET=$?
    [ 0 -ne $RET ] && echo "install_pgdb $dbname failed! ret = $RET " exit $RET
    return $RET
}

#config tecs center
function  config_tc
{
    local file=$1
    update_config $file "debug" "no" 
    update_config $file "application" "$tcname"
    update_config $file "message_broker_url" "$message_broker_url"
    update_config $file "db_server_url" "$db_server_url"   
    update_config $file "db_user" "$db_user"
    update_config $file "db_passwd" "$db_passwd"
    config_image_manager $file
#    install_apache
    config_db "$tcname" "/opt/tecs/tc/etc/tc_baseline.sql" "/opt/tecs/tc/etc/tc_view.sql" "/opt/tecs/tc/etc/updates/"
    RET=$?
    [ 0 -ne $RET ]&& echo "config_tc config_db failed! ret = $RET " exit $RET
}

#config tecs wui
function  config_wui
{
    local file=$1
    update_config $file "dbname" "$tcname" 
    RET=$?
    [ 0 -ne $RET ]&& echo "config_wui failed! ret = $RET " exit $RET
}

#config tecs cluster
function  config_cc
{
    local file=$1
    update_config $file "debug" "no" 
	update_config $file "netplane_dhcp" "base,bond0" 
    update_config  $file "application" "$ccname"
    update_config $file "message_broker_url" "$message_broker_url"
    update_config $file "db_server_url" "$db_server_url"   
    update_config $file "db_user" "$db_user"
    update_config $file "db_passwd" "$db_passwd"
    config_db "$ccname" "/opt/tecs/cc/etc/cc_baseline.sql" "/opt/tecs/cc/etc/cc_view.sql" "/opt/tecs/cc/etc/updates/"
    RET=$?
    [ 0 -ne $RET ] && echo "config_cc config_db failed! ret = $RET" && exit -1
    config_image_agent $file
#    echo  "===============config dhcp=============="
#    netplane_name="base"
#    local interface0=`route | grep default | awk -F' ' '{print $8}'`
#    local interface1=`ifconfig |grep "eth" |grep -v "$interface0" |awk -F' ' {'print $1'}|head -n 1`
#    netplane_dhcp_eth_name="$interface1"
#    if [ -z $netplane_dhcp_eth_name ];then
#    echo "only one nic, so can not config dhcp!"
#    else
#    echo "netplane_dhcp_eth_name=$netplane_dhcp_eth_name"
#    update_config $file "netplane_dhcp" "${netplane_name},${netplane_dhcp_eth_name}"
#    cat /opt/tecs/cc/etc/tecs.conf  |grep netplane_dhcp
#    fi
}

#config tecs host
function  config_hc
{
    local file=$1
    update_config $file "debug" "no" 
    update_config  $file "application" "$hcname"
    update_config $file "message_broker_url" "$message_broker_url"
    update_config $file "evb_enable" "$evb_enable"
    
    #cpu bench计算
    [  -e /etc/cpu_bench_result ] && cp /etc/cpu_bench_result /opt/tecs/hc/etc/
    if [ ! -e /opt/tecs/hc/etc/cpu_bench_result ]; then
        echo "cpu_bench_result file is not exist.Will get cpu bench result,please wait few minutes..."
        /opt/tecs/hc/bin/ubench > /opt/tecs/hc/etc/cpu_bench_result &
        while :
        do
           local ubenchRunning=`ps -ef |grep /opt/tecs/hc/bin/ubench |grep -v "grep"`
           if [ ! -z "$ubenchRunning" ] ; then
               sleep 1
               echo -n "."
           else
               echo -e
               break    
           fi
        done
        #备份一个，省得每次都计算，太浪费时间
        cp /opt/tecs/hc/etc/cpu_bench_result /etc/cpu_bench_result
    fi
}

echo "*******************************************************************************"
echo "$0 will install and start tecs on local host ..." 
echo "*******************************************************************************"

#解压版本包中的版本到当前目录下，并不自动运行内嵌脚本
cd $tecsdir
chmod +x ./installtecs_*
./installtecs_* --target ./tmp --noexec 

##获取os type 
#os_type="unknown"
#[ `uname -a | grep -c el5` -eq 1 ] && os_type="el5"
#[ `uname -a | grep -c el6` -eq 1 ] && os_type="el6"
#[ $os_type == "unknown" ] && echo "unknown os type: `uname -a`"  && exit -1
#echo "os type = $os_type" 

#rpmdir=$tecsdir/tecs/rpm/RPMS/$os_type/x86_64
rpmdir=$tecsdir/tmp
echo "rpmdir = $rpmdir" 
#rm -rf $tecsdir/tecs/rpm/RPMS/$os_type/x86_64/*devel*.rpm
ls $rpmdir -lh

function  install
{
    local pkgname=$1
    echo "install package $pkgname ... "
    rpmfile=`find $rpmdir/ -name "$pkgname-*.*.rpm"`
    [ -z "$rpmfile" ] && exit -1
    echo "package  $pkgname rpm file = $rpmfile"
    rpm -ivh  $rpmfile
    [ 0 -ne $? ] && echo "failed to install $rpmfile!" && exit -1
}

#安装openais
install "openais"
service openais start
[ 0 -ne $? ] && exit -1

#安装并启动消息服务器
install "qpidc-broker"
service qpid start
[ 0 -ne $? ] && exit -1

#安装消息客户端
install "qpidc-client"

#安装并启动数据库服务器
install "postgresql"
service postgresql start
[ 0 -ne $? ] && exit -1
sleep 10s

#安装common
install "tecs-common"

#安装并配置tc
install  "tecs-tc"
config_tc /opt/tecs/tc/etc/tecs.conf

#安装并配置wui
install "tecs-wui"
config_wui /opt/tecs/wui/htdocs/php/db_connect.ini

#安装并配置cc
install  "tecs-cc"
config_cc /opt/tecs/cc/etc/tecs.conf

#安装并配置hc
install  "tecs-hc"
config_hc /opt/tecs/hc/etc/tecs.conf

#重新启动xend
#service xend restart
#[ 0 -ne $? ] && exit -1

# tecs web服务器目前在APISVR进程中启动，使用xmlrpc自带的abyss，这个web root目录不能少
# 以后如果采用apache作为web服务器，这个步骤就可以忽略了
[ ! -e /usr/local/abyss/wui ] && mkdir -p /usr/local/abyss/wui

#更新tecs配置文件，将所有的控制台打印输出到指定的文件中，便于调试定位问题
tecs_output_dir=/dev/shm/tecsprint
[ ! -e $tecs_output_dir ] && mkdir -p $tecs_output_dir
echo "[ ! -e $tecs_output_dir ] && mkdir -p $tecs_output_dir" >> /etc/rc.local
tcexes=`awk -F' ' '{ print $1 }' /etc/tecs/tc.config`
for i in $tcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/tc.config
done

ccexes=`awk -F' ' '{ print $1 }' /etc/tecs/cc.config`
for i in $ccexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/cc.config
done

hcexes=`awk -F' ' '{ print $1 }' /etc/tecs/hc.config`
for i in $hcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/hc.config
done

service tecs-tc start
service tecs-cc start
service tecs-hc start

#安装并启动guard
install  "tecs-guard"
service guard start

#等待一段时间，tecs进程都上电完成了再返回?
echo "tecs is installed and started successfully!"
exit 0


