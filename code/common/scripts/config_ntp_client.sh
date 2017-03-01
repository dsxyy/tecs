#!/bin/bash
if [ $1 == "register" ];then
    #检测是否已经存在ntpdate的cron计划
    grep -q "ntpdate "$2" #add by tecs" /var/spool/cron/root 2>/dev/null
    if [[ $? -ne 0 ]];then
        echo "*/10 * * * * /usr/sbin/ntpdate "$2" #add by tecs" >> /var/spool/cron/root
    fi
else
    if [ $1 == "unregister" ];then
        sed -i "/ntpdate $2 #add by tecs/d" /var/spool/cron/root
    fi
fi
