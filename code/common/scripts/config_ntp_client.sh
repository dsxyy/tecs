#!/bin/bash
if [ $1 == "register" ];then
    #����Ƿ��Ѿ�����ntpdate��cron�ƻ�
    grep -q "ntpdate "$2" #add by tecs" /var/spool/cron/root 2>/dev/null
    if [[ $? -ne 0 ]];then
        echo "*/10 * * * * /usr/sbin/ntpdate "$2" #add by tecs" >> /var/spool/cron/root
    fi
else
    if [ $1 == "unregister" ];then
        sed -i "/ntpdate $2 #add by tecs/d" /var/spool/cron/root
    fi
fi
