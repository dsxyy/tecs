#!/bin/bash
if [ $# -ne 1 ];then
    echo "invalid parametrs $*"
    exit -1
fi

tecs_wui="/etc/init.d/tecs-wui"
tecs_tc="/etc/init.d/tecs-tc"
tecs_cc="/etc/init.d/tecs-cc"
tecs_hc="/etc/init.d/tecs-hc"
guard="/etc/init.d/guard"

if [[ "$1" == "start" ]];then
    [ -x "$tecs_tc" ] && service tecs-tc start
    echo "yum upgrade percent:92%"

    [ -x "$tecs_cc" ] && service tecs-cc start
    echo "yum upgrade percent:94%"

    [ -x "$tecs_hc" ] && service tecs-hc start
    echo "yum upgrade percent:96%"

    [ -x "$tecs_wui" ] && service tecs-wui start   
    echo "yum upgrade percent:98%"

    [ -x "$guard" ] && service guard start
    echo "yum upgrade percent:100%"
elif [[ "$1" == "stop" ]];then
    [ -x "$tecs_guard" ] && service guard stop
    echo "yum upgrade percent:2%"

    [ -x "$tecs_wui" ] && service tecs-wui stop
    echo "yum upgrade percent:4%"

    [ -x "$tecs_tc" ] && service tecs-tc stop
    echo "yum upgrade percent:6%"

    [ -x "$tecs_cc" ] && service tecs-cc stop
    echo "yum upgrade percent:8%"

    [ -x "$tecs_hc" ] && service tecs-hc stop
    echo "yum upgrade percent:10%"

else
    echo "invalid service choice $opt"
fi

