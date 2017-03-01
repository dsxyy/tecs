#!/bin/bash
ntp_conf="/etc/ntp.conf"
if [ $1 == "register" ];then
    mv $ntp_conf $ntp_conf.bak
    echo "driftfile /var/lib/ntp/drift" >> $ntp_conf
    echo "server $2 prefer iburst" >> $ntp_conf
    service ntpd restart
else
    if [ $1 == "unregister" ];then
        mv $ntp_conf.bak $ntp_conf
    fi
fi
