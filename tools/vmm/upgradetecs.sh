#!/bin/bash
YUM="yum --disablerepo=* --enablerepo=tecs_rpm"
#两个参数分别为操作与包的名字，eg:checkupdate tecs-tc
#$1=checkupdate/query/upgrade;$2=ftp://127.0.0.1/pub/tecs_rpm $3=tecs-wui/tecs-tc/tecs-cc/tecs-hc
IS_DEBUG="yes"
NO_UP_PACK="no upgraded version"
NO_IN_PACK="no installed version"
function debug
{
    if [ "$IS_DEBUG" == "no" ];then
        return 0
    else
        echo -e "$*"
    fi    
}
function check_lock
{
    local lock_flag=`echo "$*" | grep "Another app is currently holding the yum lock"`
    #echo "1...$1"
    echo "2...$lock_flag"
    if [[ "$lock_flag" == ""  ]];then
        debug "yum execute successfully!"
        return 0
    fi
    debug "yum is locked and waiting!"
    return 1
}

function run
{
    debug "run <$*> start"

     run_result=`$* 2>&1`        
     ret=$?
    if [ 0 -ne $ret ];then
        echo "$run_result"        
        echo "Fail to run <$*>"
        exit -1
    fi

    debug "run <$*> end"
}

function service_hc
{    
    if [[ "$1" == "start" ]];then

        [ -x "$tecs_hc" ] && service tecs-hc start
        [ -x "$guard" ] && service guard start

    elif [[ "$1" == "stop" ]];then

        [ -x "$tecs_guard" ] && service guard stop
        [ -x "$tecs_hc" ] && service tecs-hc stop
    else
        echo "invalid service choice $opt!"
    fi
}

function check_update
{
    debug "$YUM check-update $1  2>&1"

    run_result=`$YUM check-update $1 2>&1`        
    ret=$?    
    echo "$run_result"
    if [ $ret -eq 0 ];then
        run_result=$NO_UP_PACK
    elif [ $ret -eq 100  ];then
        run_result=`echo -n "$run_result" | grep $1 | awk'' '{print $2}'`
    else
        echo "Fail to run yum chekupdate!"
        exit -1
    fi

    return 0
}
function query_local_common
{
    run rpm -qa --queryformat '%{VERSION}-%{RELEASE}\n' tecs-common
    local_version=$run_result

    if [[ "$local_version" == ""  ]];then
        local_version=$NO_IN_PACK
    fi
    echo "local_common_version:{$local_version}"
}

function query_local_version
{
    run rpm -qa --queryformat '%{VERSION}-%{RELEASE}\n' $1
    local_version=$run_result

    if [[ "$local_version" == ""  ]];then
        local_version=$NO_IN_PACK
    fi
    echo "local_version:{$local_version}"
}

function query_repo_common
{
    check_update tecs-common
    repo_version=$run_result
    echo "repo_common_version:{$repo_version}"
}

function query_repo_version
{
    check_update $1
    repo_version=$run_result
    echo "repo_version:{$repo_version}"
}

function query_version
{
    query_repo_version $*
    query_repo_common
    query_local_version $*
    query_local_common
}

function upgrade_check_version
{    
    query_local_version $*
    query_repo_version $*
}

operation=$1
repo_addr=$2
package=$3

repo_dir=/etc/yum.repos.d
if [ ! -e $repo_dir ];then
    echo "Fail to run yum dir:$repo_dir"
    exit -1
fi 
[ ! -e $repo_dir/tecs.repo ] && touch /etc/yum.repos.d/tecs.repo
cat>/etc/yum.repos.d/tecs.repo<<EOF
[tecs_rpm]
name=rpms for tc
baseurl=$repo_addr
enabled=0
gpgcheck=0
repo_gpgcheck=0
failovermethod=priority
EOF

$YUM clean metadata

#不管检查更新还是升级查询都需要先查询下

#仅仅是做查询
if [[ "$operation" == "query"  ]];then 
    query_version $package
    exit
fi

#仅仅是先做检查是否有新的升级
if [[ "$operation" == "checkupdate"  ]];then
    check_update $package
    exit
fi

#正式的升级流程开始
#1.获取升级前的包的版本
#2.看是否有新的包开始，不依赖与代码的控制流程
upgrade_check_version $package
if [[ $? -ne 0 ]];then
    exit -1
fi
echo "yum upgrade percent:15%"
service_hc stop

#3.是wui包只升级包，不做数据库的升级
if  [[ "$package" == "tecs-wui" ]];then
    run $YUM -y upgrade $package
fi

#合一的时候tecs-common怎么处理??还是单独进行升级处理??tecs-guard单独处理
#3.tc,cc,hc升级公共的包，以及各自独有的包
#run $YUM -y upgrade tecs-common

echo "yum upgrade percent:45%"
run $YUM -y upgrade $package

#4.再查询下升级后的包的新的版本
#query_local_version $package

echo "yum upgrade percent:75%"
#5.升级tc与cc的包还需要升级数据库
if  [[ "$package" == "tecs-tc" ]];then
    if [ -e /opt/tecs/tc/scripts/update_pgdb.sh ];then
        run /opt/tecs/tc/scripts/update_pgdb.sh /opt/tecs/tc/etc
        echo "yum upgrade percent:80%"
    fi
elif [[ "$package" == "tecs-cc" ]];then
    if [ -e /opt/tecs/cc/scripts/update_pgdb.sh ];then
        run /opt/tecs/cc/scripts/update_pgdb.sh /opt/tecs/cc/etc
        echo "yum upgrade percent:80%"
    fi
fi
service_hc start
echo "yum upgrade percent:90%"

