#!/bin/bash
YUM="yum --disablerepo=* --enablerepo=tecs_rpm"
#���������ֱ�Ϊ������������֣�eg:checkupdate tecs-tc
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

#���ܼ����»���������ѯ����Ҫ�Ȳ�ѯ��

#����������ѯ
if [[ "$operation" == "query"  ]];then 
    query_version $package
    exit
fi

#��������������Ƿ����µ�����
if [[ "$operation" == "checkupdate"  ]];then
    check_update $package
    exit
fi

#��ʽ���������̿�ʼ
#1.��ȡ����ǰ�İ��İ汾
#2.���Ƿ����µİ���ʼ�������������Ŀ�������
upgrade_check_version $package
if [[ $? -ne 0 ]];then
    exit -1
fi
echo "yum upgrade percent:15%"
service_hc stop

#3.��wui��ֻ���������������ݿ������
if  [[ "$package" == "tecs-wui" ]];then
    run $YUM -y upgrade $package
fi

#��һ��ʱ��tecs-common��ô����??���ǵ���������������??tecs-guard��������
#3.tc,cc,hc���������İ����Լ����Զ��еİ�
#run $YUM -y upgrade tecs-common

echo "yum upgrade percent:45%"
run $YUM -y upgrade $package

#4.�ٲ�ѯ��������İ����µİ汾
#query_local_version $package

echo "yum upgrade percent:75%"
#5.����tc��cc�İ�����Ҫ�������ݿ�
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

