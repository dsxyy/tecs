#!/bin/bash
YUM="yum  --disablerepo=* --enablerepo=tecs_rpm -y"
#合一的时候tecs-common怎么处理??
$YUM clean metadata
hc_rpm=`$YUM check-update tecs-hc | grep tecs-hc`
if [[ -z $hc_rpm ]];then
    exit
fi

service guard stop
service tecs-hc stop
$YUM upgrade tecs-common
$YUM upgrade tecs-hc
$YUM upgrade tecs-guard
service tecs-hc start
service guard start


