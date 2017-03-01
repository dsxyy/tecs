#!/bin/bash
HOST="localhost"
PORT=5432
USERNAME="tecs"
PASSWD="tecs"
DBNAME="tecscloud"
PSQL="/usr/local/pgsql/bin/psql"

CLOUD_IP=
CLUSTER_NAME=
CLUSTER_IP=
SA_NAME=
SA_IP=
HOST_NAME=
HOST_IP=
FILE=
FILE_PATH=
FILE_NAME=
FILE_PARAMETER=
COMMAND=
EXECUTE_TMP_DIR="/tmp/ssh/cmd"
IS_DEBUG="no"
ip_regexp=\\d*\.\d*\.\d*\.\d*
name_regexp=[^a-zA-Z0-9_-]
option_regexp=^-[hupdcf]$
refer_regexp=^-[ams]$
#注一:
#"Your input error:"与"Fail to run"作为代码中解析命令的错误标志，如果需要修改则要同步修改代码
#另外修改的时候请搜索本代码中的所有此字符串标志，一起修改
#此处将来可以优化定义一个函数来统一处理，目前对于$psql命令作为入参传入执行，有点问题
function error
{
    #见注一:
    echo "Your input error:$*! please --help"
    exit -1
}

function usage
{
    echo "Usage:    tc/cc/hc [OBJECT] [DATABASE] [COMMAND]/[FILE]"
    echo "OBJECT:   tc -a/-m/-s/tcip"
    echo "          cc -a/cc_name -a/-m/-s/cc_ip"
    echo "          hc -a/cc_name -a/hc_name/hc_ip"
    echo "DATABASE: -h 127.0.0.1  -u tecs -p tecs  -d tecscloud"
    echo "COMMAND:  -c command"
    echo "FILE:     -f file"
    echo "-a        all node"
    echo "-m        all master node"
    echo "-s        all slave node"
    echo "-h        tc database server url"
    echo "-u        tc database server user"
    echo "-p        tc database serve  password"
    echo "-d        tc database server name"
    echo "-c        the parameter of command"
    echo "-f        the parameter of file"
    echo "tc example:"
    echo "tc -a -c ifconfig"
    echo "tc -m -c ifconfig eth0"
    echo "tc -s -f ./ifconfig.sh"
    echo "tc 127.0.0.1 -f ./ifconfig.sh eth0"
    echo "tc -a -d tecscloud -c ifconfig"
    echo "tc -a -h 127.0.0.1 -u tecs -p tecs  -d tecscloud -c ifconfig"
    echo "cc example:"
    echo "cc -a -a -c ifconfig"
    echo "cc -a -m -c ifconfig eth0"
    echo "cc -a -s -c ifconfig eth0"
    echo "cc -a 1.1.1.1 -c ifconfig eth0"
    echo "cc tecscluster 10.43.179.10 -c ifconfig eth0"
    echo "cc tecscluster -a -f ./ifconfig.sh"
    echo "cc tecscluster -m -f ./ifconfig.sh"
    echo "cc tecscluster -s -f ./ifconfig.sh"
    echo "cc tecscluster 1.1.1.1 -f ./ifconfig.sh eth0"
    echo "cc -a -a -h 127.0.0.1 -u tecs -p tecs  -d tecscloud -c ifconfig"
    echo "hc example:"
    echo "hc -a -a -c ifconfig"
    echo "hc -a tecshost -c ifconfig eth0"
    echo "hc -a 1.1.1.1 -c ifconfig eth0"
    echo "hc tecscluster -a -c ifconfig eth0"
    echo "hc tecscluster tecshost -c ifconfig eth0"
    echo "hc tecscluster 1.1.1.1 -c ifconfig eth0"
    echo "hc -a -a -f ./ifconfig.sh"
    echo "hc tecscluster tecshost -f ./ifconfig.sh"
    echo "hc tecscluster 1.1.1.1 -f ./ifconfig.sh eth0"
    echo "hc -a -a -h 127.0.0.1 -u tecs -p tecs  -d tecscloud -c ifconfig"
    exit
}

function debug
{
    if [ "$IS_DEBUG" == "no" ];then
        return 0
    else
        echo -e "$*"
    fi    
}

function is_my_ip
{
    if [ -z "$1" ];then
        return 1
    fi
    
    ipaddr=`ifconfig -a | grep 'inet addr:' | cut -d : -f2 | awk '{print $1}'`
    local i
    for i in `echo "$ipaddr" | awk '{print $1}'`
    do
        if [[ "$1" == "$i" ]];then
            return 1
        fi
        
    done

    return 0
}

function is_option
{
    local match

    match=`echo "$*" | grep $option_regexp | wc -l`
    if [ 0 -ne $match ];then
        return 1
    fi

    return 0
}

function check_name
{
    local name_exp=$1

    if [ -z "$1" ];then
        error "name is null" 
    fi

    if echo $name_exp | grep -q '^-[a]$';then
        return 0
    fi

    if echo ${name_exp:0:1} | grep -q '[^a-zA-Z0-9]';then
        error "$1"
    fi

    if echo $name_exp | grep -q '[^a-zA-Z0-9_-]';then
        error "$1"
    fi
    
    return 0
}

function check_ip
{    
    if [ -z "$1" ];then
        error "ip is null" 
    fi

    local ip_exp=$1
   
    if echo $ip_exp | grep -q '^-[ams]$';then
        return 0
    fi 

    if echo $1 |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$' ; then
        a=`echo $1 | awk -F. '{print $1}'`
        b=`echo $1 | awk -F. '{print $2}'`
        c=`echo $1 | awk -F. '{print $3}'`
        d=`echo $1 | awk -F. '{print $4}'`

        for n in $a $b $c $d; do
            if [ $n -ge 255 ] || [ $n -lt 0 ]; then
                error "the number of the IP should less than 255 and greate than 0"
                return 0
            fi
        done
        if [ $a -le 0 ] || [ $d -le 0 ]; then
            error "the number of the IP should not equal with 0"
            return 0
        fi
   else
        error "$1"
   fi
    
    return 0
}

function check_name_ip
{
    if [ -z "$1" ];then
        error "name is null" 
    fi

    if echo $1 | grep -q '^-[a]$';then
        return 0
    fi

    if echo $1 |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$' ; then
        a=`echo $1 | awk -F. '{print $1}'`
        b=`echo $1 | awk -F. '{print $2}'`
        c=`echo $1 | awk -F. '{print $3}'`
        d=`echo $1 | awk -F. '{print $4}'`

        for n in $a $b $c $d; do
            if [ $n -ge 255 ] || [ $n -lt 0 ]; then
                error "the number of the IP should less than 255 and greate than 0"
                return 0
            fi
        done

        if [ $a -le 0 ] || [ $d -le 0 ]; then
            error "the number of the IP should not equal with 0"
            return 0
        fi

        return 0
   fi

    local name_exp=$1
    if echo ${name_exp:0:1} | grep -q '[^a-zA-Z0-9]';then
        error "$1"
    fi

    if echo $name_exp | grep -q '[^a-zA-Z0-9_-]';then
        error "$1"
    fi
    
    return 0
}

function switch_object
{
    local ob_name=$1

    if echo $ob_name | grep -q '^[tT][cC]$';then
        return 1
    fi

    if echo $ob_name | grep -q '^[cC][cC]$';then
        return 2
    fi

    if echo $ob_name | grep -q '^[hH][cC]$';then
        return 3
    fi

    if echo $ob_name | grep -q '^[sS][aA]$';then
        return 4
    fi

    return 0
}

function option_parse
{
    local finish="no"
    local option

    while [ "no" == "$finish" ]
    do     
        option=$1
        case $option in
            "-h") HOST=$2;shift ;;
            "-u") USERNAME=$2;shift ;;
            "-p") PASSWD=$2;shift ;;
            "-d") DBNAME=$2;shift ;;
            "-c") shift;COMMAND=$*;finish="yes";;
            "-f") shift;FILE=$1;shift;FILE_PARAMETER=$*;finish="yes";;
            *)error "$*";;
        esac
        shift
    done

    #dirname
    #basename
    if [ -n "$FILE" ];then
        FILE_PATH=`dirname "$FILE"`
        FILE_NAME=${FILE##*/}
        
    fi

    if [ -z "$COMMAND" -a -z "$FILE" ];then
        error "command and file are null";
    fi
    
}

function hc_parse
{ 
    is_option $1
    if [ $? -eq 1 ];then
        debug "hc default"
        CLUSTER_NAME="-a"
        HOST_IP="-a"
        option_parse $*
        return 0
    fi

    local host=
    local num=0
    while [ $num -lt 2 ];do
        case $num in
            0) CLUSTER_NAME=$1;check_name_ip $1;shift ;;
            1) host=$1;check_name_ip $1;shift ;;    
        esac
        num=$(( $num + 1 ))	
    done 

    if echo $host |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$' ; then
        HOST_IP=$host
    else 
        HOST_NAME=$host
    fi
 
    option_parse $*
}

function sa_parse
{
    is_option $1
    if [ $? -eq 1 ];then
        debug "sa default"
        SA_NAME="-a"
        SA_IP="-a"
        option_parse $*
        return 0
    fi

    local num=0
    while [ $num -lt 2 ];do
        case $num in
            0) SA_NAME=$1;check_name $1; shift ;;
            1) SA_IP=$1;check_ip $1;shift ;;            
        esac
        num=$(( $num + 1 ))	
    done

    option_parse $*
}

function cc_parse
{
    is_option $1
    if [ $? -eq 1 ];then
        debug "cc default"
        CLUSTER_NAME="-a"
        CLUSTER_IP="-a"
        option_parse $*
        return 0
    fi

    local num=0
    while [ $num -lt 2 ];do
        case $num in
            0) CLUSTER_NAME=$1;check_name $1; shift ;;
            1) CLUSTER_IP=$1;check_ip $1;shift ;;            
        esac
        num=$(( $num + 1 ))	
    done

    option_parse $*
}

function tc_parse
{
    is_option $1
    if [ $? -eq 1 ];then
        debug "tc default"
        CLOUD_IP="-a"
        option_parse $*
        return
    fi
    
    check_ip $1
    CLOUD_IP=$1
    shift
    option_parse $*
}

function parse
{
    if [ "$1" == "--help" ];then
        usage
    fi

    if [ "$1" == "-v" ];then
        IS_DEBUG="yes"
        shift
    fi
    debug $*
    switch_object $1    
    object_type=$?    
    case $object_type in
        1) shift;tc_parse $* ;;
        2) shift;cc_parse $* ;;
        3) shift;hc_parse $* ;;
        4) shift;sa_parse $* ;;
        0) error "$*";
    esac
}

function is_execute_file
{
    local cmd=$COMMAND
    local file=$FILE

    if [ -n "$cmd" -a -n "$file" ];then
        error "conflict between command $cmd and file $file";
    fi

    if [ -z "$cmd" -a -z "$file" ];then
        error "command and file are null";
    fi

    if [ -n "$FILE" ];then
        return 1;
    fi
}

function run
{
    debug "run <$*> start"
    $* 2>&1
    if [ 0 -ne $? ];then
        #见注一:
        echo "Fail to run <$*>"
        exit -1
    fi
    
    debug "run <$*> end"
}

function ssh_execute_file
{
    local target_ip=$1

    is_my_ip $target_ip
    if [ 1 -eq $? ];then
        cmd="$FILE $FILE_PARAMETER"
        run $FILE $FILE_PARAMETER
    else
        run ssh $target_ip 'bash -s' < $FILE $FILE_PARAMETER
    fi
}

function ssh_ssh_execute_file
{
    local media_ip=$1
    local target_ip=$2

    is_my_ip $target_ip
    if [ 1 -eq $? ];then
        run $FILE $FILE_PARAMETER
        return 0
    fi

    is_my_ip $media_ip
    if [ 1 -eq $? ] || [[ "$media_ip" == "$target_ip" ]];then
        ssh_execute_file $target_ip $FILE
    else
        run ssh $media_ip ssh $target_ip 'bash -s' < $FILE $FILE_PARAMETER
    fi  
}

function ssh_execute_cmd
{
    local ip=$1
    local cmd=$COMMAND

    is_my_ip $ip
    if [ 1 -eq $? ];then
        run $cmd
    else
        run "ssh $ip $cmd"
    fi    
}

function ssh_ssh_execute_cmd
{

    local media_ip=$1
    local target_ip=$2
    local cmd=$COMMAND

    is_my_ip $target_ip
    if [ 1 -eq $? ];then
        run $cmd
        return
    fi 

    is_my_ip $media_ip
    if [ 1 -eq $? ] || [[ "$media_ip" == "$target_ip" ]];then
        run "ssh $target_ip $cmd"
    else 
        run "ssh $media_ip "ssh $target_ip $cmd""
    fi
}

function ssh_execute
{
    is_execute_file
    if [ 1 -eq $? ];then
        ssh_execute_file $*
    else
        ssh_execute_cmd $*
    fi 
}

function ssh_ssh_execute
{
    is_execute_file
    if [ 1 -eq $? ];then
        ssh_ssh_execute_file $*
    else
        ssh_ssh_execute_cmd $*
    fi 
}

function select_cc_db
{
    local condition=$1
    export PGPASSWORD=$PASSWD
    local psql="$PSQL -h $HOST -p $PORT -U $USERNAME -d $DBNAME -t -c"
    debug "$psql SELECT name,db_name,db_server_url,db_server_port,db_user,db_passwd FROM cluster_pool $condition"
    cc_dbs=`$psql "SELECT name,db_name,db_server_url,db_server_port,db_user,db_passwd FROM cluster_pool $condition"`
    if [ 0 -ne $? ];then
        #见注一:
        echo "Fail to run <$psql SELECT name,db_name,db_server_url,db_server_port,db_user,db_passwd FROM cluster_pool $condition>"
        exit -1
    fi
    debug "cc db:\n$cc_dbs"
    return 0
}

function select_hc_ip
{
    local host=$1
    local port=$2
    local user=$3
    local pass=$4
    local name=$5
    local condition=$6

    export PGPASSWORD=$pass
    local psql="$PSQL -h $host -p $port -U $user -d $name -t -c"
    debug $psql "SELECT name,ip_address FROM host_pool $condition"
    hc_ips=`$psql "SELECT name,ip_address FROM host_pool $condition"`
    if [ 0 -ne $? ];then
        #见注一:
        echo "Fail to run <$psql "SELECT name,ip_address FROM host_pool $condition">"
        exit -1
    fi
    debug "hc ip:\n$hc_ips"
    return 0
}

function hc_execute 
{
    if echo $CLUSTER_NAME |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$';then
        if echo $HOST_IP |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$';then
            ssh_ssh_execute $CLUSTER_NAME $HOST_IP
            return 0
        fi
    fi    
    
    if [[ "$CLUSTER_NAME" == "-a" ]];then
        select_cc_db
    else
        select_cc_db "where name='$CLUSTER_NAME'"
    fi

    cc_dbs_num=`echo "$cc_dbs" | grep [0-9] | wc -l`
    local i=1
    while [ $i -le  $cc_dbs_num ]
    do
        cc_db=`echo "$cc_dbs" | sed -n '1p'`
        cc_db=`echo "$cc_db" | sed 's/|*//g'`
        cc_dbs=`echo "$cc_dbs" | sed '1d'`
      
        cluster_name=`echo "$cc_db" | awk -F ' ' '{print$1}'`
        db_name=`echo "$cc_db" | awk -F ' ' '{print$2}'`
        db_server_url=`echo "$cc_db" | awk -F ' ' '{print$3}'`
        db_server_port=`echo "$cc_db" | awk -F ' ' '{print$4}'`
        db_user=`echo "$cc_db" | awk -F ' ' '{print$5}'`
        db_passwd=`echo "$cc_db" | awk -F ' ' '{print$6}'` 
        select_cc_ip "where is_master=1 and cluster_name='$cluster_name'"
        cluster_ip=$cc_ips

        if echo $HOST_IP |egrep -q '^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$';then
            ssh_ssh_execute $cluster_ip $HOST_IP
            i=$(( $i + 1 ))
            continue
        fi

        if [[ "$HOST_IP" == "-a" ]];then
            select_hc_ip $cluster_ip $db_server_port $db_user $db_passwd $db_name
        else
            select_hc_ip $cluster_ip $db_server_port $db_user $db_passwd $db_name "where name='$HOST_NAME'"
        fi

        hc_ips_num=`echo "$hc_ips" | grep \. |wc -l`
        local j=1
        while [ $j -le  $hc_ips_num ]
        do
            hc_ip=`echo -e "$hc_ips" | sed -n '1p'`
            hc_ip=`echo "$hc_ip" | sed 's/|*//g'`
            hc_ips=`echo "$hc_ips" | sed '1d'`
            local host_name=`echo "$hc_ip" | awk -F ' ' '{print$1}'`
            local host_ip=`echo "$hc_ip" | awk -F ' ' '{print$2}'`
            ssh_ssh_execute $cluster_ip $host_ip
            j=$(( $j + 1 ))	         
        done
        i=$(( $i + 1 ))
    done
}

function select_cc_ip
{
    local condition=$1
    
    export PGPASSWORD=$PASSWD
    local psql="$PSQL -h $HOST -p $PORT -U $USERNAME -d $DBNAME -t -c"
    debug "$psql "SELECT ip_address FROM cluster_node $condition""
    cc_ips=`$psql "SELECT ip_address FROM cluster_node $condition"`
    if [ 0 -ne $? ];then
        #见注一:
        echo "Fail to run <$psql "SELECT ip_address FROM cluster_node $condition">"
        exit -1
    fi
    debug "cc ip:\n$cc_ips"
    return 0
}

function sa_execute 
{    
    if [[ $SA_NAME == "-a" ]];then
        if [[ "$SA_IP" == "-a" ]];then
           select_cc_ip
        elif [[ "$SA_IP" == "-m" ]];then
           select_cc_ip "where is_master=1"
        elif [[ "$SA_IP" == "-s" ]];then
           select_cc_ip "where is_master=0"
        else            
           ssh_execute $SA_IP
           return 0
        fi
    else
        if [[ "$SA_IP" == "-a" ]];then
            select_cc_ip "where name='$CLUSTER_NAME'"
        elif [[ "$SA_IP" == "-m" ]];then            
            select_cc_ip "where is_master=1 and name='$CLUSTER_NAME'"
        elif [[ "$SA_IP" == "-s" ]];then
            select_cc_ip "where is_master=0 and name='$CLUSTER_NAME'"
        else
            ssh_execute $SA_IP
            return 0
        fi
    fi
}

function ssh_cc_execute 
{
    cc_ips_num=`echo "$cc_ips" | grep \. | wc -l`
    for ((j=1; j <= cc_ips_num ; j++))
    do
        cc_ip=`echo -e "$cc_ips" | sed -n '1p'`
        cc_ip=`echo "$cc_ip" | sed 's/|*//g'`
        cc_ip=`echo "$cc_ip" | awk -F ' ' '{print$1}'`  
        ssh_execute $cc_ip
        cc_ips=`echo "$cc_ips" | sed '1d'`
    done
}



function cc_execute 
{    
    if [[ $CLUSTER_NAME == "-a" ]];then
        if [[ "$CLUSTER_IP" == "-a" ]];then
           select_cc_ip
        elif [[ "$CLUSTER_IP" == "-m" ]];then
           select_cc_ip "where is_master=1"
        elif [[ "$CLUSTER_IP" == "-s" ]];then
           select_cc_ip "where is_master=0"
        else            
           ssh_execute $CLUSTER_IP
           return 0
        fi
    else
        if [[ "$CLUSTER_IP" == "-a" ]];then
            select_cc_ip "where name='$CLUSTER_NAME'"
        elif [[ "$CLUSTER_IP" == "-m" ]];then            
            select_cc_ip "where is_master=1 and name='$CLUSTER_NAME'"
        elif [[ "$CLUSTER_IP" == "-s" ]];then
            select_cc_ip "where is_master=0 and name='$CLUSTER_NAME'"
        else
            ssh_execute $CLUSTER_IP
            return 0
        fi
    fi

    ssh_cc_execute
}

function select_tc_ip
{
    local condition=$1
    export PGPASSWORD=$PASSWD
    local psql="$PSQL -h $HOST -p $PORT -U $USERNAME -d $DBNAME  -t -c"
    debug "$psql "SELECT ip_address FROM cloud_node $condition""
    tc_ips=`$psql "SELECT ip_address FROM cloud_node $condition"`
    if [ 0 -ne $? ];then
        #见注一:
        echo "Fail to run <$psql "SELECT ip_address FROM cloud_node $condition">"
        exit -1
    fi
    debug "tc ip:\n$tc_ips"
    return 0
}

function ssh_tc_execute 
{
    tc_ips_num=`echo "$tc_ips" | grep \. | wc -l`
    local j
    for ((j=1; j <= tc_ips_num ; j++))
    do
        tc_ip=`echo -e "$tc_ips" | sed -n '1p'`
        tc_ip=`echo "$tc_ip" | sed 's/|*//g'` 
        tc_ip=`echo "$tc_ip" | awk -F ' ' '{print$1}'`
        ssh_execute $tc_ip
        tc_ips=`echo "$tc_ips" | sed '1d'`     
    done
}

function tc_execute 
{
    if [[ "$CLOUD_IP" == "-a" ]];then
        select_tc_ip 
    elif [[ "$CLOUD_IP" == "-m" ]];then
        select_tc_ip "where is_master=1"
    elif [[ "$CLOUD_IP" == "-s" ]];then
        select_tc_ip "where is_master=0"
    else
        ssh_execute $CLOUD_IP
        return 0
    fi
    ssh_tc_execute
}

function execute
{
    case $object_type in
        1) tc_execute ;;
        2) cc_execute ;;
        3) hc_execute ;;
        4) sa_execute ;;
        *)error "$object_typ";;
    esac
}

parse $*

debug "cluster name:$CLUSTER_NAME"
debug "host name:$HOST_NAME"
debug "cloud ip:$CLOUD_IP"
debug "cluster ip:$CLUSTER_IP"
debug "host ip:$HOST_IP"
debug "db host:$HOST"
debug "db user:$USERNAME"
debug "db pass:$PASSWD"
debug "db:$DBNAME"
debug "command:$COMMAND"
debug "file path:$FILE_PATH"
debug "file:$FILE"
debug "file parameter:$FILE_PARAMETER"

execute


