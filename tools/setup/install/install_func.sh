#!/bin/bash
# TECS安装公共函数

#防止脚本重复被包含
if [ ! "$_INSTALL_FUNC_FILE" ];then 
_INSTALL_FUNC_DIR=`pwd`
#头文件包含
cd $_INSTALL_FUNC_DIR/../common/
.  tecs_common_func.sh
.  tecs_global_var.sh

cd $_INSTALL_FUNC_DIR/../base/
.  pgdb_interface.sh
.  qpid_interface.sh

cd $_INSTALL_FUNC_DIR/../uninstall/
.  uninstall_func.sh

cd $_INSTALL_FUNC_DIR
.  install_global_var.sh

#get and write cpu bench resust to file
function get_cpu_bench
{
    chmod +x /opt/tecs/hc/bin/ubench
    
    if [ ! -e /opt/tecs/hc/etc/cpu_bench_result ]; then
        echo "Will get cpu bench result,please wait a few minutes..."
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
    fi
}

# 询问并获取CPU计算能力
function ask_get_cpu_bench
{
    local component=$1
    local default_value=
    
    if [ -e /opt/tecs/hc/etc/cpu_bench_result ];then
        echo "cpu_bench_result file is exist!"
        return 0
    fi
  
    echo "[Notice]: Cpu bench result is used to test your host performance."
    echo "          no: Default choice. Recommand value is zero. User MUST configure it!!"
    echo "         yes: Recommand value get by ubench, only for reference. Need a few minutes!!"
    get_install_config "$component" "compute_cpu_bench" "bool" "Get cpu bench result?" "no"
    if [ "$config_value" == "yes" ]; then
        get_cpu_bench
    fi
}

# 产生一个ssh key
function generate_ssh_key
{
    local private_key_file=~/.ssh/id_dsa
    local public_key_file=~/.ssh/id_dsa.pub
    #ssh私钥或公钥有一个不存在就要重新产生一对
    if [ ! -e $private_key_file ] || [ ! -e $public_key_file ]; then
        rm -rf $public_key_file
        rm -rf $private_key_file
        ssh-keygen -t dsa -f $private_key_file -q -P '' -b 1024
        [ $? == 0 ] && echo "ssh-keygen -t dsa succssfully!"
    else
        echo "ssh key is ready!"
    fi
}

#导出一个nfs共享目录
function export_nfs_share
{
    local nfsshare=$1
    echo "checking nfs share path $nfsshare ..."
    if [ ! -e "$nfsshare" ]; then
        echo "creating directory $nfsshare ..."
        mkdir -p "$nfsshare"
    fi

    exportfs -ar
    local nfsexist=`exportfs | grep -c "$nfsshare"`
    if [ $nfsexist -gt 0 ]; then
        echo "nfs share $nfsshare ready!"
    else
        echo "$nfsshare *(ro,no_root_squash)" >> /etc/exports
        echo "nfs share $nfsshare added! "
    fi
    exportfs -ar
    echo "restarting nfs service ..."
    service nfs restart
}
# 清理exports文件
function clear_nfs_exports
{   
    local file=/etc/exports
    # 提取含/var/lib/tecs行中tecs的目录
    local tecspathall=""
    while read line
    do  
        local line1=`echo $line |grep "^[[:space:]]*/var/lib/tecs"`
        if [ "$line1" != "" ];then
            local tecspath=`echo $line1 |awk -F' ' '{print $1}'`
            tecspathall="$tecspathall $tecspath"
        fi
    done < $file
    
    [ "$tecspathall" = "" ] && return 0
    # 删除不存在的tecs目录
    for i in $tecspathall
    do
        if [ ! -e "$i" ];then
            local num=`cat $file |grep -nw "^[[:space:]]*${i}" |awk -F':' '{print $1}' |head -n1`
            sed -i "${num}d" $file
        fi
    done
}
#将域名转换为ip，并对ping不通的情况给出警告
function get_ip_from_ping
{
    input_ip=$1
    output_ip=$input_ip
    ping_result=`ping $input_ip -c 1 -w 5`
    if [ $? -eq 0 ];then
        ping_result=`echo $ping_result|tr '(' ' '|tr ')' ' '` 
        local ip=`echo $ping_result|sed -n '/from/ s/.*from[^0-9]* \([0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\).*/\1/p'`
        [ ! -z "$ip" ] && output_ip=$ip || echo -e "=============\nwarning: can't get ip address from \"ping $input_ip\"\n============="
    else
        echo -e "=============\nwarning: ping $input_ip failed.\n============="
    fi
}

#########################################################################
# TECS 配置
#########################################################################

function config_debug_mode
{
   update_config $1 "debug" "$debugmode" 
}

#get tecs application name string answer from user
function read_application_name 
{
    local component=$1
    local prompt=$2
    local default=$3

    get_install_config "$component" "application" "string" "$prompt" "$default"
    application="$config_value"
    
    #application名称不能超过64个字符
    if [[ ${#application} -gt 64 ]];
    then
       echo "name too long, 64 characters limited!"
       read_application_name "$component" "$prompt" "$default"
    fi

    #tecs application 名称只能由字母数字下划线中划线等字符组成
    local regex="[a-zA-Z0-9_-]"
    local result=${application//$regex/}
    #将合法的字符去掉，看看结果是否为空，如果还不为空，表示存在非法字符
    if [ ! -z "$result" ]; then
        echo "name contains illegal characters!"
        read_application_name "$component" "$prompt" "$default"
    fi
}

# application配置
function config_application
{
    local component=$1
    local file=$2
    local description=$3
    local value=$4

    read_application_name "$component" "$description" "$value"
    update_config "$file" "application" "$application"
}

#########################################################################
# TECS 日志等级配置
#########################################################################
function config_exc_log_level
{
   update_config $1 "exc_log_level" "$exc_log_level"
}

function get_public_ip
{
    local_ip_list=`ifconfig  | grep "inet addr:" | grep -v "127.0.0.1" | awk -F' ' {'print $2'} | awk -F':' {'print $2'}`
    #tecs不为用户自动选择ip
    #通向默认网关的接口上通常配置的都是有效的大网地址
    local def_gw_if=`route | grep default | awk -F' ' '{print $8}'`
    public_ip=""
    if [[ -n "$def_gw_if" ]];then
        public_ip=`ifconfig "$def_gw_if" | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
    fi
    # INTERACTIVE 这个是自动化运行变量，如果为0，表示当前上下文是自动化过程
    # 如果是自动化的话，则nfs_server_url使用TC的application作为域名
    if [ "$operation" = "install" -a "$mode" != "custom" ];then     
         public_ip=$cloudname
    fi
}

#########################################################################
# TECS img_manager配置
#########################################################################
# nfs_server_url 选项的配置
function config_nfs_server_url
{
    local component=$1
    local file=$2
    
    #get_public_ip
    #default_value=$public_ip
    #if [[ $public_ip = $cloudname ]];then
    #    default_value="image.$public_ip"
    #fi
    default_value="im.image.tecs"
    #这里的默认值是脚本执行时才能获取到的，配置文件中没有 
    echo -e "\n=======Image Manager Configuration======="
    #get_install_config "$component" "nfs_server_url" "iplist" "Please input the url of nfs server in image manager" "$default_value" "$local_ip_list"
    update_config  "$file" "nfs_server_url" "$default_value"
    #local nfsserver="$config_value"

    #local isselfaddr=`ifconfig -a | grep "inet addr:.*$nfsserver"`
    #if [ -z "$isselfaddr" ]; then
    #    echo "[Notice]: Please config nfs share on $nfsserver correctly!"
    #    return 0
    #fi
}

# img_manager模块的配置
function config_img_manager
{
    local component=$1
    local file=$2
    config_nfs_server_url "$component" $file 

    local nfsshare="/var/lib/tecs/image_repo"
    export_nfs_share $nfsshare
    #将nfs服务加入guard监控列表
    please_guard_me "nfs"
}

# TECS management_interface配置
function config_management_interface
{
    #通向默认网关的接口上通常配置的都是有效的大网地址，默认就从它获取tecs管理平面ip
    local def_gw_if=`route | grep default | awk -F' ' '{print $8}'`
    update_config $1 "management_interface" "$def_gw_if"
}

#########################################################################
# TECS file_manager配置
#########################################################################
# file_manager_server_url 的配置
function config_file_manager_url
{
    local component=$1
    local file=$2
    
    get_public_ip
    get_install_config "$component" "file_manager_server_url" "iplist" "Please input the url of file manager" "$public_ip" "$local_ip_list"
    update_config  "$file" "file_manager_server_url" "$config_value"
}

# coredump_save_dir的配置
function config_coredump_save_dir
{
    local component=$1
    local file=$2
    local default_value=
    
    get_install_config "$component" "coredump_save_dir" "string" "Please input coredump save path" ""
    update_config  "$file" "coredump_save_dir" "$config_value"
}
#########################################################################
# TECS img_agent模块配置
#########################################################################
# image_agent_mode选项配置
function config_image_agent_mode
{
    local component=$1
    local file=$2
    local default_value=

    echo "=======Image Agent Configuration======="
    get_install_config "$component" "image_agent_mode" "bool" "Do not set image agent mode to none?" ""
    if [ "$config_value" = "no" ]; then
        update_config  $file "image_agent_mode" "none"
        return 0
    else
        update_config  $file "image_agent_mode" "nfs"
        #目前默认支持nfs代理方式
        echo "image agent will work in nfs mode!"
        return 1
    fi
}

# nfs_agent_url选项配置
function config_nfs_agent_url
{
    local component=$1
    local file=$1

    local local_ip_list=`ifconfig  | grep "inet addr:" | grep -v "127.0.0.1" | awk -F' ' {'print $2'} | awk -F':' {'print $2'}`
    #tecs不为用户自动选择ip
    #通向默认网关的接口上通常配置的都是有效的大网地址
    local def_gw_if=`route | grep default | awk -F' ' '{print $8}'`
    local public_ip
    if [[ -n "$def_gw_if" ]];then
        public_ip=`ifconfig "$def_gw_if" | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
    fi

    get_install_config "$component" "nfs_agent_url" "iplist" "Please input the url of nfs service in the image agent, hc will access image agent by the url:" "$public_ip" "$local_ip_list"
    update_config  "$file" "nfs_agent_url" "$config_value"
    
    local nfsserver="$answer"
    local isselfaddr=`ifconfig -a | grep "inet addr:.*$nfsserver"`
    if [ -z "$isselfaddr" ]; then
        echo "[Notice]: Please config nfs share on $nfsserver correctly!"
        return 0
    fi

    local nfsshare="/var/lib/tecs/image_agent"
    export_nfs_share $nfsshare
    #将nfs服务加入guard监控列表
    please_guard_me "nfs"
}

# img_agent模块配置入口
function config_img_agent
{
    local component=$1
    local file=$2
    
    config_image_agent_mode $component $file
    # 如果没有配置agent模式，退出
    [ $? -eq 0 ] && return 0
    
    config_nfs_agent_url $component $file
}

function config_vnm_message_broker
{
    echo "[Notice]: Please use multi message server according to your netmork!"
    echo "eg:"
    echo "       |<---->(10.0.0.1 message server) <--->tc"
    echo "vnm<-->|       "
    echo "       |<---->(192.0.0.1 message server)<--->vna"
    config_two_message_broker tecs-vnm $1 tecs-tc tecs-vna
}

function config_vna_message_broker
{
    echo "[Notice]: Please use multi message server according to your netmork!"
    echo "eg:"
    echo "       |<---->(10.0.0.1 message server) <--->hc"
    echo "vna<-->|       "
    echo "       |<---->(192.0.0.1 message server)<--->vnm"
    config_two_message_broker tecs-vna $1 tecs-hc tecs-vnm
}

#########################################################################
# TECS TC组件的配置入口
#########################################################################
function config_tc
{   
    local file=$1
    local component="tecs-tc"

    config_debug_mode $file
    config_exc_log_level $file 
    config_management_interface $file 
    config_application "$component" "$file" "Please input cloud name, and it will be tecs-tc database name for convenience" "$cloudname"
    cloudname=$application
    config_message_broker_url "$component" "$file"
    config_db_server_url "$component" $file
    config_img_manager "$component" $file
    config_file_manager_url "$component" $file
    config_coredump_save_dir "$component" $file
    config_db_user_and_pwd "$component" $file
    upgrade_tc_database
    generate_ssh_key
}

#########################################################################
# TECS CC组件入口配置
#########################################################################
#disable_nat 选项配置
function config_disable_nat
{
    update_config $1 "disable_nat" "yes"
}

# nat模块配置入口
function config_nat
{
    local file=$1
    config_disable_nat $file
}
function config_cc
{
    local file=$1
    local component=tecs-cc
    config_debug_mode $file
    config_exc_log_level $file 
    config_management_interface $file 

    config_application "$component" "$file" "Please input cluster name, and it will be tecs-cc database name for convenience" "$clustername"
    clustername=$application

    echo "[Notice]: Please use multi message server if tc and hc are not in same subnet!"
    echo "eg:"
    echo "      |<---->(10.0.0.1 message server) <--->TC "
    echo "CC<-->|       "
    echo "      |<---->(192.0.0.1 message server)<--->HC "

    config_two_message_broker "$component" "$file" "tecs-tc" "tecs-hc"  
  
    config_db_server_url "$component" $file
    config_db_user_and_pwd "$component" $file
    config_img_agent $component $file
    upgrade_cc_database

    generate_ssh_key

    #安装完CC后提示是否配置NAT
    local natconf=/opt/tecs/cc/etc/tecs.conf
    if [ -e $natconf ]; then
        config_nat $natconf
    fi
}
#########################################################################
# TECS HC组件入口配置
#########################################################################
function config_hc
{
    local file=$1
    local component=tecs-hc
    
    config_debug_mode $file

    #tecs默认使用主机名作为hc的application，但是如果主机名中带有.，必须另外输一个
    local hcname=`hostname`
    if [[ `echo "$hcname" | awk -F '.' '{ print $1}'` != "$hcname" ]]; then
        hcname=`uuidgen`
        config_application "$component" "$file" "Please input hc name" "$hcname"
    else
        config_application "$component" "$file" "Please input hc name" "$hcname"
        hcname=$application
    fi
    
    config_management_interface $file 
    config_message_broker_url "$component" $file

    # 清除老的映射关系
    sed -i "/$cloudname/d" /etc/hosts
    #从message_server中获取IP
    get_ip_from_ping $message_server
    # 给HC下载image的时候，映像地址增加个地址映射关系
    [ `cat /etc/hosts | grep -c $cloudname` -eq 0 ] && echo "$output_ip  $cloudname" >> /etc/hosts
    
    #cpu bench计算
    ask_get_cpu_bench "$component"
    generate_ssh_key
    
    [ -x /etc/init.d/xend ] && /sbin/chkconfig --level 2345 xend on
    #安装loop网卡，应用在单刀片设备中
    #/opt/tecs/hc/bin/install_loop.sh
    
    # 检测libvirt和xend,如果存在，则加入到监控列表中
    [ -e /etc/init.d/libvirtd ] && please_guard_me "libvirtd"
    [ -e /etc/init.d/xend ] && please_guard_me "xend"
	[ -e /etc/init.d/xenstored ] && please_guard_me "xenstored"
}


#########################################################################
# TECS wui模块配置
#########################################################################
#config wui service
function config_wui
{
    echo "[Notice]: Because tecs-wui can be installed on different host from tecs-tc and tecs_vnm, "
    echo "please give enough information to find tc's and vnm's database!"
       
    local file=/opt/tecs/wui/htdocs/newweb/php/db_connect.ini

    config_answer=
    # 先读取TC下面的配置
    get_config $tecs_tc_conf "db_server_url"
    local dbhost="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input database server url of tc, find it in $tecs_tc_conf"
        dbhost="$answer"
    fi  
    update_config $file "host" "$dbhost"
    
    # 先获取TC配置中的数据库名称，如果获取不到，使用application，如果配置文件获取不到，则要用户输入
    get_config $tecs_tc_conf "db_name"
    if [[ $config_answer == "" ]]; then
        get_config $tecs_tc_conf "application"
    fi
    local dbname="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input tc database name, find it in $tecs_tc_conf" 
        dbname="$answer"
    fi  
    update_config $file "dbname" "$dbname"
    
    # 获取TC配置文件中的数据库用户名配置
    get_config $tecs_tc_conf "db_user"
    local dbuser="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input user name of tc database, find it in $tecs_tc_conf"
        dbuser="$answer"
    fi  
    update_config $file "user" "$dbuser"

    # 获取TC配置文件中的数据库密码配置
    get_config $tecs_tc_conf "db_passwd"
    local dbpasswd="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input user password of tc database, find it in $tecs_tc_conf"
        dbpasswd="$answer"
    fi  
    update_config $file "password" "$dbpasswd"
    
    config_answer=
    # 先读取VNM下面的配置
    get_config $tecs_vnm_conf "db_server_url"
    local dbhost="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input database server url of vnm, find it in $tecs_vnm_conf"
        dbhost="$answer"
    fi  
    update_config $file "vnm_host" "$dbhost"
    
    # 先获取VNM配置中的数据库名称，如果获取不到，使用application，如果配置文件获取不到，则要用户输入
    get_config $tecs_vnm_conf "db_name"
    if [[ $config_answer == "" ]]; then
        get_config $tecs_vnm_conf "application"
    fi
    local dbname="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input vnm database name, find it in $tecs_vnm_conf" 
        dbname="$answer"
    fi  
    update_config $file "vnm_dbname" "$dbname"
    
    # 获取VNM配置文件中的数据库用户名配置
    get_config $tecs_vnm_conf "db_user"
    local dbuser="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input user name of vnm database, find it in $tecs_vnm_conf"
        dbuser="$answer"
    fi  
    update_config $file "vnm_user" "$dbuser"

    # 获取VNM配置文件中的数据库密码配置
    get_config $tecs_vnm_conf "db_passwd"
    local dbpasswd="$config_answer"
    if [[ $config_answer == "" ]]; then
        read_string_input_null_check "Please input user password of vnm database, find it in $tecs_vnm_conf"
        dbpasswd="$answer"
    fi  
    update_config $file "vnm_password" "$dbpasswd"
    
    echo "tecs-wui config finished!"
    echo "if any configuration changes in future, please modify $file manually!"
}


#########################################################################
# TECS VNA组件入口配置
#########################################################################
function config_vna
{
    local file=$1
    local kernal_uplink_port
    local ip_str
    local mask_str
    
    config_debug_mode $file

    config_vna_message_broker $file
    
    #创建默认的dvs-kernal
    #tunctl -t "dvs_kernal"
    #ifconfig dvs_kernal up
        
    get_default_gw_st
    cd $_INSTALL_FUNC_DIR
    ./create_kernel_port.sh
    cd - >/dev/null
    set_default_gw_st
    # 如果vna存在，则加入到监控列表中
    [ -e /etc/init.d/tecs-vna ] && please_guard_me "tecs-vna"
}


#########################################################################
# TECS VNM组件入口配置
#########################################################################
function config_vnm
{
    local file=$1
    local component=tecs-vnm
    local default_value=

    config_debug_mode $file
    config_vnm_message_broker $file
    # db_server_url选项配置
    config_db_server_url "$component" $file
    
    # db_name选项配置
    get_install_config "$component" "db_name" "string" "Please input database name" ""
    update_config $file "db_name" "$config_value"
    
    config_db_user_and_pwd "$component" $file
    
    upgrade_vnm_database
  
    #创建网络的数据库
    #chmod 777 /opt/tecs/network/vnm/scripts/update_vnm_pgdb.sh
    #/opt/tecs/network/vnm/scripts/update_vnm_pgdb.sh    
    
    #配置VNM的application
    #tecs默认使用主机名作为vnm的application，但是如果主机名中带有.，必须另外输一个
    local hcname=`hostname`
    if [[ `echo "$hcname" | awk -F '.' '{ print $1}'` != "$hcname" ]]; then
        hcname=`uuidgen`        
        config_application "$component" "$file" "Please input vnm name" "vnm$hcname"
    else
        config_application "$component" "$file" "Please input vnm name" "vnm$hcname"
        hcname=$application
    fi
    
    # 如果vna存在，则加入到监控列表中
    [ -e /etc/init.d/tecs-vnm ] && please_guard_me "tecs-vnm"
}

#########################################################################
# TECS WDG组件入口配置
#########################################################################
function config_wdg
{
    local file=$1
    local component=tecs-wdg
    config_message_broker_url "$component" $file

    # 如果wdg存在，则加入到监控列表中
    [ -e /etc/init.d/$component ] && please_guard_me "$component"
}
#########################################################################
# TECS sa模块配置(存储模块)
#########################################################################
# sa_cfg_file配置项配置
function config_sa_cfg_file
{
    local component=$1
    local file=$2 
    local default_value=
    
    echo "-------------------------------------------ATTENTION-------------------------------------------------------------------------------"
    echo "the default cfg file of sa is  /opt/tecs/sa/etc/sa.xml , you can change it in installing.  "
    echo "pls  generate the sa cfg file  /opt/tecs/sa/etc/sa.xml accroding to /opt/tecs/sa/etc/sa.xml.example and change "
    echo " its content  according your actual requirements before you start sa  "
    echo "if you don't know how to write sa.xml ,you can also use the command DbgWriteSacfg(\"/home/test.xml\") to generate a config example "
    echo "if you use localsan, you must use the prefix \"tecs_volume_\" for your vg name  "
    echo "------------------------------------------------------------------------------------------------------------------------------------"

    get_install_config "$component" "sa_cfg_file" "string" "Please input sa cfg file" ""
    update_config  $file "sa_cfg_file" "$config_value"
}

#config sa service
function config_sa
{   
    local file=$1 
    local component=tecs-sa
    #tecs默认使用主机名作为hc的application，但是如果主机名中带有.，必须另外输一个
    local hcname=`hostname`
    if [[ `echo "$hcname" | awk -F '.' '{ print $1}'` != "$hcname" ]]; then
        hcname=`uuidgen`
        config_application "$component" "$file" "Please input sa name" "$hcname"
    else
        config_application "$component" "$file" "Please input sa name" "$hcname"
        hcname=$application
    fi        
        
    config_message_broker_url "$component" $file
    config_sa_cfg_file "$component" $file
}
#########################################################################
# nicchk模块配置(网口检测)
#########################################################################
function config_nicchk
{
    local file=$1 
    local component=nicchk
    local component_prd=chk_period

    get_install_config "$component" "nic_list" "string" "Please input nic name to be monitored:" ""
    update_config $file "nic_list" "$config_value"

    get_install_config "$component" "chk_period" "string" "Please input expired seconds for nic error:" "300"
    update_config $file "chk_period" "$config_value"
    
}
##########################TECS组件安装与卸载#######################################
#install tc 组件rpm包安装
function tc_component_rpm_install
{
    local app="tecs-tc"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_tc $tecs_tc_conf
}
#install cc 组件rpm包安装
function cc_component_rpm_install
{
    local app="tecs-cc"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_cc  $tecs_cc_conf
}
#install hc 组件rpm安装
function hc_component_rpm_install
{
    update_libvirt

    local app="tecs-hc"
    check_and_install_rpm  $app
    if [ $? -eq 0 ]; then
        #HC lm_sensors支持需先使用sensors-detect检测
        ./config_sensors_detect.sh 2>/dev/null
        config_hc $tecs_hc_conf
    fi
}
#install wui 组件rpm包安装
function wui_component_rpm_install
{
    local app="tecs-wui"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_wui
}
#install guard 组件rpm包安装
function guard_component_rpm_install
{
    local app="tecs-guard"
    check_and_install_rpm  $app
}

#install nicchk 组件rpm包安装
function nicchk_component_rpm_install
{
    local app="nicchk"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_nicchk  $nicchk_conf
}

function install_tc_component
{
    local app="tecs-tc"
    get_install_component "$app" "Install tecs center? "
    [[ $config_answer == "no" ]] && return 0
    
    check_installed "$app"
    if [ "$has_installed" = "yes" ];then
        tc_component_rpm_uninstall 
    fi

    tc_component_rpm_install
}

function install_cc_component
{
    local app="tecs-cc"
    get_install_component "$app" "Install cluster controller? "
    [[ $config_answer == "no" ]] && return 0
    
    check_installed "$app"
    if [ "$has_installed" = "yes" ];then
        cc_component_rpm_uninstall 
    fi
     
    cc_component_rpm_install
}

function install_hc_component
{
    local app="tecs-hc"
    get_install_component "$app" "Install host controller? "
    [[ $config_answer == "no" ]] && return 0
    hc_component_rpm_uninstall
    hc_component_rpm_install
}

function install_wui_component
{
    local app="tecs-wui"
    get_install_component "$app" "Install tecs wui? "
    [[ $config_answer == "no" ]] && return 0
    
    wui_component_rpm_uninstall 
    wui_component_rpm_install
}
function install_guard_component
{
    local app="tecs-guard"
    get_install_component "$app" "Install tecs guard? "
    [[ $config_answer == "no" ]] && return 0
    
    guard_component_rpm_uninstall
    guard_component_rpm_install
}
#install ovs 组件rpm安装
function ovs_component_rpm_install
{
    local app1="kmod-openvswitch"
    local app2="openvswitch"
    
    ifconfig virbr0 down > /dev/null 2>&1
    brctl delbr virbr0  > /dev/null 2>&1
    
    ifconfig br_wd down > /dev/null 2>&1
    brctl delbr br_wd  > /dev/null 2>&1
    
    #if [ ! -e /lib/modules/`uname -r`/extra/openvswitch/openvswitch*.ko ] && [ ! -e /lib/modules/`uname -r`/kernel/net/openvswitch/openvswitch*.ko ]; then
        #echo "need to install open vswitch,will take a few minutes, please wait..."
        check_and_install_or_upgrade_rpm  $app1
        check_and_install_or_upgrade_rpm  $app2
    #fi
    
    service_restart "openvswitch"
}

#install vna 组件rpm安装
function vna_component_rpm_install
{
    local app="tecs-vna"
    
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_vna  /opt/tecs/network/vna/etc/tecs.conf
}

#install vnm 组件rpm安装
function vnm_component_rpm_install
{
    local app="tecs-vnm"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_vnm  /opt/tecs/network/vnm/etc/tecs.conf
}

#install wdg 组件rpm安装
function wdg_component_rpm_install
{
    local app="tecs-wdg"
    
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_wdg  /opt/tecs/network/wdg/etc/tecs.conf
}

#install sd 组件rpm安装
function sd_component_rpm_install
{
    local app="tecs-sd"
    check_and_install_rpm  $app
}

#install tgtd 组件rpm包安装
function tgtd_service_component_rpm_install
{
    local app="scsi-target-utils"
    check_and_install_rpm  $app "force"
    [ ! -e /etc/tecs/guard_list/ ] && mkdir -p /etc/tecs/guard_list/
    touch /etc/tecs/guard_list/tgtd
}

#install 存储 组件rpm包安装
function storage_component_rpm_install
{
    local app="tecs-sa"
    check_and_install_rpm  $app
    [ $? -eq 0 ] && config_sa "$tecs_sa_conf"
}

function install_ovs_component
{
    #kvm版本中ovs已经集成在centos 6.4 ISO中，不需要再安装ovs
    if [ -e /dev/kvm ]; then
        return 0
    fi
    
    local app="ovs"
    get_install_component "$app" "Install openvswitch "
    [[ $config_answer == "no" ]] && return 0
    
    ovs_component_rpm_install
}

function get_default_gw_st
{
    defaultgw_ip=`route |grep default |awk  '{print $2}'`
}

function set_default_gw_st
{
    if [ -n "$defaultgw_ip" ] ; then 
    route add default gw  $defaultgw_ip
    fi
}

function install_wdg_component
{
    local app="tecs-wdg"

    get_install_component "$app" "Install watch dog? "
    [[ $config_answer == "no" ]] && return 0

    ovs_component_rpm_install

    wdg_component_rpm_uninstall
    wdg_component_rpm_install
}

function install_vna_component
{
    local app="tecs-vna"

    get_install_component "$app" "Install vnet agent? "
    [[ $config_answer == "no" ]] && return 0

    ovs_component_rpm_install

    vna_component_rpm_uninstall
    vna_component_rpm_install
}

function install_vnm_component
{
    local app="tecs-vnm"
    
    get_install_component "$app" "Install vnet mangement? "
    [[ $config_answer == "no" ]] && return 0
        
    vnm_component_rpm_uninstall
    vnm_component_rpm_install
}
#根据条件提示是否安装
function install_storage_component
{
    local app="tecs-sa"
    
    get_install_component "$app" "Install tecs sa? "
    [[ $config_answer == "no" ]] && return 0
    
    storage_component_rpm_uninstall
    storage_component_rpm_install
}

function install_tgtd_service_component
{
    local app="tgtd"
    
    get_install_component "$app" "Install tgtd server? "
    [[ $config_answer == "no" ]] && return 0
    
    tgtd_service_component_rpm_uninstall
    tgtd_service_component_rpm_install
}

function install_sd_component
{
    local app="tecs-sd"
    
    get_install_component "$app" "Install tecs sd? "
    [[ $config_answer == "no" ]] && return 0
        
    sd_component_rpm_uninstall
    sd_component_rpm_install
}

function install_nicchk_component
{
    local app="nicchk"
    get_install_component "$app" "Install nicchk? "
    [[ $config_answer == "no" ]] && return 0
    
    check_installed "$app"
    if [ "$has_installed" = "yes" ];then
        nicchk_component_rpm_uninstall 
    fi

    nicchk_component_rpm_install
    
}

# 这个用于特殊场景，不从配置文件读取配置，必须手动输入，如host和sa安装模式时消息服务器的设置
function read_msg_broker
{ 
    local component=$1
    local promt=$2
    local default=$3

    #这个重要，一定要清空
    answer=    
    #如果用户指定配置文件中配置了，就直接读取
    if [ "$user_specify_config" != "" ];then    
         get_component_config "$component" "$user_specify_config" "message_broker_url"
         answer=$config_answer
    fi
    
    #指定配置文件中没有则交互提示给出消息服务器地址
    if [ "$answer" = "" ];then
        read_string "$promt" "$default"
        [ "$answer" = "" ] && read_msg_broker "$component" "$promt" "$default"
    else
        echo -e "$promt(default: $default): $answer" 
    fi
}

_INSTALL_FUNC_FILE="install_func.sh"
fi

