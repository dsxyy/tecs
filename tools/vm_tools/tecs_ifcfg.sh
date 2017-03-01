#!/bin/bash
eth_interface=""
if_ip=""
if_mac=""
if_netmask=""
if_gateway=""
string=""
mount_path="/home/disk"
ifcfg_path="/etc/sysconfig/network-scripts/"


function ifcfg_config
{
    local file=$1
    local nic

    echo file=$file

    if [ "$file" == "" ]; then
        echo "vm.xml not found"
        return
    fi

    while read line
    do
        string=`echo $line |grep "<item>"`
        if [ "$string" != "" ]; then
            eth_interface="eth"
            echo start
        fi
        
        string=`echo $line |grep "</item>"`
        if [ "$string" != "" ]; then
            eth_interface=""
            nic=`ifcfg_match_ifconfig $if_mac`
            if [ "" != "$nic" ]; then
                ifcfg_modify $if_ip $if_netmask $nic $if_gateway
                ifconfig $nic $if_ip netmask $if_netmask
                if [ "" != "$if_gateway" ]; then
                    route add default gw $if_gateway dev $nic >/dev/null 2>&1
                fi
            fi

            if_ip=""
            if_mac=""
            if_netmask=""
            if_gateway=""
            echo finish
        fi

        if [ "$eth_interface" != "" ]; then
            string=`echo $line |sed -rn '/ipaddr/s/[^>]+>([^<]+).*/\1/p'`
            if [ "$string" != "" ]; then
                if_ip=$string
            fi

            string=`echo $line |sed -rn '/mac/s/[^>]+>([^<]+).*/\1/p'`
            if [ "$string" != "" ]; then
                if_mac=$string
            fi

            string=`echo $line |sed -rn '/netmask/s/[^>]+>([^<]+).*/\1/p'`
            if [ "$string" != "" ]; then
                if_netmask=$string
            fi
            
            string=`echo $line |sed -rn '/gateway/s/[^>]+>([^<]+).*/\1/p'`
            if [ "$string" != "" ]; then
                if_gateway=$string
            fi
        fi
    done<$file

    umount $mount_path >/dev/null 2>&1
}

function xml_match
{
    local disk_path
    local xml_path
    local search_path
    local string
    local path

    rm -rf $mount_path
    mkdir $mount_path
    
    disk_path=`find /dev |grep -E '^/dev/sd.*[^0-9]\b'`
    if [ "$disk_path" != "" ]; then
        for disk in $disk_path; do
            
            mount $disk $mount_path >/dev/null 2>&1

            if [ "`mount |grep $mount_path`" != "" ]; then
                xml_path=`find $mount_path |grep "vm.xml"`
                if [ "$xml_path" != "" ]; then
                    echo $xml_path
                    return
                fi

                umount $mount_path >/dev/null 2>&1
            elif [ "`mount |grep $disk`" != "" ]; then
                search_path=`mount |grep $disk`
                #for path in $search_path; do
                    string=`echo $search_path |sed -rn '/on/s/[^ ]+ ([^(]+).*/\1/p'`
                    path=`echo $string |awk -F ' ' '{print $2}'`
                    xml_path=`find $path |grep "vm.xml"`
                    if [ "$xml_path" != "" ]; then
                        echo $xml_path
                        return
                    fi
                #done
            fi
        done
    fi

    disk_path=`find /dev |grep -E '^/dev/hd.*[^0-9]\b'`
    if [ "$disk_path" != "" ]; then
        for disk in $disk_path; do
            
            mount $disk $mount_path >/dev/null 2>&1

            if [ "`mount |grep $mount_path`" != "" ]; then
                xml_path=`find $mount_path |grep "vm.xml"`
                if [ "$xml_path" != "" ]; then
                    echo $xml_path
                    return
                fi

                umount $mount_path >/dev/null 2>&1
            elif [ "`mount |grep $disk`" != "" ]; then
                search_path=`mount |grep $disk`
                #for path in $search_path; do
                    string=`echo $search_path |sed -rn '/on/s/[^ ]+ ([^(]+).*/\1/p'`
                    path=`echo $string |awk -F ' ' '{print $2}'`
                    xml_path=`find $path |grep "vm.xml"`
                    if [ "$xml_path" != "" ]; then
                        echo $xml_path
                        return
                    fi
                #done
            fi
        done
    fi
    
    disk_path=`find /dev |grep -E '^/dev/sr0\b'`
    if [ "$disk_path" != "" ]; then
        for disk in $disk_path; do
            
            mount $disk $mount_path >/dev/null 2>&1

            if [ "`mount |grep $mount_path`" != "" ]; then
                xml_path=`find $mount_path |grep "vm.xml"`
                if [ "$xml_path" != "" ]; then
                    echo $xml_path
                    return
                fi

                umount $mount_path >/dev/null 2>&1
            elif [ "`mount |grep $disk`" != "" ]; then
                search_path=`mount |grep $disk`
                #for path in $search_path; do
                    string=`echo $search_path |sed -rn '/on/s/[^ ]+ ([^(]+).*/\1/p'`
                    path=`echo $string |awk -F ' ' '{print $2}'`
                    xml_path=`find $path |grep "vm.xml"`
                    if [ "$xml_path" != "" ]; then
                        echo $xml_path
                        return
                    fi
                #done
            fi
        done
    fi

    disk_path=`find /dev |grep -E '^/dev/xvd.*[^0-9]\b'`
    if [ "$disk_path" != "" ]; then
        for disk in $disk_path; do
            
            mount $disk $mount_path >/dev/null 2>&1

            if [ "`mount |grep $mount_path`" != "" ]; then
                xml_path=`find $mount_path |grep "vm.xml"`
                if [ "$xml_path" != "" ]; then
                    echo $xml_path
                    return
                fi

                umount $mount_path >/dev/null 2>&1
            elif [ "`mount |grep $disk`" != "" ]; then
                search_path=`mount |grep $disk`
                #for path in $search_path; do
                    string=`echo $search_path |sed -rn '/on/s/[^ ]+ ([^(]+).*/\1/p'`
                    path=`echo $string |awk -F ' ' '{print $2}'`
                    xml_path=`find $path |grep "vm.xml"`
                    if [ "$xml_path" != "" ]; then
                        echo $xml_path
                        return
                    fi
                #done
            fi
        done
    fi
}

function ifcfg_match
{
    local path=$1
    local mac=$2
    local ifcfg_eth
    local result

    ifcfg_eth=`find $path |grep "ifcfg-"`

    for ifcfg in $ifcfg_eth; do
        result=`sed -n '/HWADDR/p' $ifcfg |awk -F '"' '{print $2}'`

        if [ "" == "$result" ]; then
            result=`sed -n '/HWADDR/p' $ifcfg |awk -F '=' '{print $2}'`
        fi

        if [ "$result" == "$mac" ]; then
            echo $ifcfg
            return
        fi    
    done
}

function ifcfg_match_ifconfig
{
    local mac=$1
    local ifcfg_eth
    local string

    string=`ifconfig -a |grep -i $mac`
    #echo string=$string

    if [ "" != "$string" ]; then
        ifcfg_eth=`echo $string |awk -F ' ' '{print $1}'`
        echo $ifcfg_eth
    fi	
}

function ifcfg_modify
{
    local ip=$1
    local netmask=$2
    local gateway=$4
    local nic=$3
    local file=""
    local path=""

    file=`find /etc/sysconfig |grep -E "ifcfg-$nic$"`
    if [ "" != "$file" ];then
        chmod 777 $file
        echo eth_file=$file
        sed -i '/BOOTPROTO/d' $file
        sed -i '/IPADDR/d' $file
        sed -i '/NETMASK/d' $file
        sed -i '/GATEWAY/d' $file
        
        echo "BOOTPROTO=static">>$file
        echo "IPADDR=$ip">>$file
        echo "NETMASK=$netmask">>$file
        echo "GATEWAY=$gateway">>$file
    else
        file=`find /etc/sysconfig |grep "ifcfg-eth"`
        if [ "" != "$file" ];then
            for i in $file
            do
                path=`echo $i |awk -F '/i' {'print $1'}`
                echo "DEVICE=$nic">>$path/ifcfg-$nic
                echo "BOOTPROTO=static">>$path/ifcfg-$nic
                echo "IPADDR=$ip">>$path/ifcfg-$nic
                echo "NETMASK=$netmask">>$path/ifcfg-$nic
                echo "GATEWAY=$gateway">>$path/ifcfg-$nic
                if [ "" != "`cat /etc/issue |grep -i suse`" ]; then
                    echo "STARTMODE=auto">>$path/ifcfg-$nic
                fi
                return
            done
        fi
    fi
}

ifcfg_config $(xml_match)
