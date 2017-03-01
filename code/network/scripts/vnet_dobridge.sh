#!/bin/bash

logfile="/var/log/dobridge"
function do_logfile
{
    rm -rf $logfile
	touch $logfile
	echo "##this is vbridge logfile create by vnet version 1.0" >>$logfile
}

#记录日志
function do_log
{
  local logstr=$1
  LANG=en_US.ISO8859-1
  echo -n `date '+%b %d %T'` >> $logfile
  echo " $logstr" >>$logfile
}

#update key = value config option in an ini file
function update_config
{
    local file=$1
    local key=$2
    local value=$3

    [ ! -e $file ] && return

    #echo update key $key to value $value in file $file ...
    local exist=`grep "^[[:space:]]*[^#]" $file | grep -c "$key[[:space:]]*=[[:space:]]*.*"`
    #注意：如果某行是注释，开头第一个字符必须是#号!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`

    if [ $exist -gt 0 ];then
        #如果已经存在未注释的有效配置行，直接更新value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key=$value#" $file
    elif [ $comment -gt 0 ];then
        #如果存在已经注释掉的对应配置行，则去掉注释，更新value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key=$value@" $file
    else
        #否则在末尾追加有效配置行
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "# added by $writer at $timestamp" >> $file
        echo "$key=$value" >> $file
    fi
}

#使用check_ifsriov后必须将is_sriov清0
is_sriov=0
function check_ifsriov
{
    local adapter=$1
    local sriov_specia_num
	
	echo  "check nic  $adapter is sriov or not"
    sriov_specia_num=`ls -ld /sys/class/net/$adapter/device |awk '{print $11}' |awk -F '/' '{print $4}'`

    lspci -s $sriov_specia_num -v |grep 'SR-IOV' >/dev/null 2>&1
    if [ $? -eq 0 ];then
        lspci -s $sriov_specia_num -n |grep '8086:10c9' >/dev/null 2>&1	
        if [ $? -ne 0 ];then
            is_sriov=1
            do_log "check_ifsriov $adapter is SR-IOV"
            return
        fi    		
    fi

}

is_bonding_member=0
function check_ifbonding_member
{
    local nic=$1
	local file="/etc/sysconfig/network-scripts/ifcfg-$nic"
	cat $file |grep -E "MASTER|SLAVE" >/dev/null 2>&1
	if [ $? -eq 0 ];then
	   is_bonding_member=1
	fi
}

function do_bridge_allbonds
{
   local all_bonds=`cat /sys/class/net/bonding_masters`
   for i in $all_bonds
   do
       do_bridge_bond $i
   done

}

function do_bridge_bond
{
   local bond=$1
   local sriov_bond=0
   local slaves=`cat /proc/net/bonding/$bond | grep Slave |awk -F ":" '{printf $2}'`
   for i in $slaves
   do
       is_sriov=0
	   check_ifsriov $i
	   if [ "$is_sriov" = "1" ];then
	       sriov_bond=1
	   fi
   done
   
   ifconfig $bond |grep 00:00:00:00:00:00 >/dev/null 2>&1
   if [ $? -ne 0 ] && [ "$sriov_bond" = "0" ];then
      do_log "will do_bridge $bond"
      do_bridge $bond
   fi
}

eth_ip_copy=""
eth_mask_copy=""
function get_ipaddr_adapter
{
    local ipstring
    local maskstring
    local eth_name_ip=$1
	
    eth_ip_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip | grep "^[[:space:]]*[^#]"|grep IPADDR |awk -F '=' '{print $2}'`
    eth_mask_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip |grep "^[[:space:]]*[^#]"|grep NETMASK |awk -F '=' '{print $2}'`
	
	#网卡配置文件里面读不到IP ，可能是DHCP方式获取，ifconfig获取
	if [[ $eth_ip_copy = "" ]]; then
	    ipstring=`ifconfig $eth_name_ip |grep 'inet addr' | awk '{print $2}'`
            eth_ip_copy=`echo "$ipstring" | awk -F ':' '{print $2}'`
	fi
	
	if [[ $eth_mask_copy = "" ]]; then
	    maskstring=`ifconfig $eth_name_ip |grep 'Mask' | awk '{print $4}'`
	    eth_mask_copy=`echo "$maskstring" | awk -F ':' '{print $2}'`
	fi
}
function do_bridge
{
	  local netdev_name=$1
	  local bridge_name=$2
	  
	  #第一列是网桥名，现有网桥名不能是bridge_name
	  brctl show 2>/dev/null |awk '{print $1}' |grep -x $bridge_name > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      do_log "$bridge_name is already bridge ,cant do_bridge again"
	      return
	  fi
	  
	  brctl show 2>/dev/null |awk '{print $4}' |grep -x "$netdev_name" > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      do_log "$netdev_name is already $netdev ,cant do_bridge again"
	      return
	  fi
	  
	  lsmod |grep openvswitch_mod > /dev/null 2>&1
	  if [ $? -ne 0 ];then
		  #未安装OVS,用xen的脚本创建网桥
		  do_log "start bridge on $netdev_name"
		  /opt/tecs/network/vna/scripts/xen_bridge  start netdev=$netdev_name bridge=$bridge_name
		  if [ $? -ne 0 ];then
		      do_log "creat bridge use xen netdev is $netdev_name bridgename is $bridge_name failed "
			  exit -1
		  fi
		  else
			  #安装了OVS，用OVS网桥
			  do_log " has installed ovs and start bridge on $netdev_name"
			  /opt/tecs/network/vna/scripts/network-bridge-ovs start netdev=$netdev_name bridge=$bridge_name
		  if [ $? -ne 0 ];then
		      do_log "creat bridge use ovs netdev is $netdev_name bridgename is $bridge_name failed "
			  exit -1
		  fi
	  fi
}

function do_addif
{
    local bridge_name=$1
	local if_name=$2
	lsmod |grep openvswitch_mod > /dev/null 2>&1
	if [ $? -ne 0 ];then
	    brctl addif $bridge_name $if_name
	else
	    ovs-vsctl add-port $bridge_name $if_name
	fi
}

function do_bridge_noif
{
	local bridge_name=$1
	brctl addbr $bridge_name
	if [ $? -ne 0 ];then
	    echo "brctl addbr $bridge_name failed"
		do_log "brctl addbr $bridge_name failed"
	else
	    echo "successfuly brctl addbr $bridge_name !"
	fi
	
}
function del_bridge_noif
{
	local bridge_name=$1
	brctl delbr $bridge_name
	if [ $? -ne 0 ];then
	    echo "brctl delbr $bridge_name failed"
		do_log "brctl delbr $bridge_name failed"
	else
	    echo "successfuly brctl delbr $bridge_name !"
	fi
	
}

function stop_bridge
{
	  local netdev_name=$1
	  local bridge_name=$2
	  
	  lsmod |grep openvswitch_mod
	  if [ $? != 0 ];then
	      /opt/tecs/network/vna/scripts/xen_bridge  stop netdev=$netdev_name bridge=$bridge_name
	  	  if [ $? -ne 0 ];then
		      do_log "stop bridge use xen over $netdev_name bridge is $bridge_name failed "
			  exit -1
		  fi
	  else
	      #安装了OVS，用OVS网桥
	      /opt/tecs/network/vna/scripts/network-bridge-ovs stop netdev=$netdev_name bridge=$bridge_name
	  	  if [ $? -ne 0 ];then
		      do_log "stop bridge use ovs over $netdev_name bridge is $bridge_name failed "
			  exit -1
		  fi
	  fi
	  sleep 1
}

function add_if
{
    local bridge=$1
	local ifnet=$2
	do_log "add if $bridge $ifnet"
	ip link set $ifnet up
	brctl addif $bridge $ifnet
	if [ $? != 0 ];then
	    do_log "brctl addif $bridge $ifnet failed!"
		exit -1
	fi
	sleep 1
}

function del_if
{
    local bridge=$1
	local ifnet=$2
	do_log "brctl delif $bridge $ifnet"
	brctl delif $bridge $ifnet
	if [ $? != 0 ];then
	    do_log "brctl delif $bridge $ifnet failed!"
		exit -1
	fi
	sleep 1
}

function add_vlan_eth
{
   local nicname=$1
   local vlanid=$2
   do_log "vconfig add $nicname $vlanid"
   vconfig add $nicname $vlanid
   if [ $? != 0 ];then
	  do_log "vconfig add $nicname $vlanid failed!"
	  exit -1
   fi
   ip link set "$nicname.$vlanid" up
}

function del_vlan_eth
{
   local nicname=$1
   local vlanid=$2
   do_log "vconfig rem $nicname.$vlanid"
   vconfig rem "$nicname.$vlanid"
   if [ $? != 0 ];then
	  do_log "vconfig rem $nicname.$vlanid failed!"
	  exit -1
   fi
}

function creat_bond_cfg_file
{
	local bond=$1
    local ip=$2
    local mask=$3
    local file="/etc/sysconfig/network-scripts/ifcfg-${bond}"

    do_log "enter creat_bond_cfg_file, bond is $bond"
    if [ -f $file ]; then
        echo "/etc/sysconfig/network-scripts/ifcfg-${bond} has exsit, bond can do only one time!"
    else
        echo "DEVICE=${bond}">>/etc/sysconfig/network-scripts/ifcfg-${bond} 
        echo "BOOTPROTO=static">>/etc/sysconfig/network-scripts/ifcfg-${bond}
		
		#这里ip 和mask可能为空，为空则不写入
		if [[ $ip != "" ]]; then
		    echo "IPADDR=${ip}">>/etc/sysconfig/network-scripts/ifcfg-${bond} 
		fi
		
        if [[ $mask != "" ]] ; then
            echo "NETMASK=${mask}">>/etc/sysconfig/network-scripts/ifcfg-${bond} 
	    fi

	    #默认模式是802.3AD 链路检测100毫秒
	    echo 'BONDING_OPTS="mode=1 miimon=100"'>>/etc/sysconfig/network-scripts/ifcfg-${bond} 
    fi 
}

function modify_eth_cfg_file
{         
    local adapter=$1
    local master=$2
    local file
    file="/etc/sysconfig/network-scripts/ifcfg-${adapter}"
    
    if [ -e $file ]; then
	    #已经修改过就return ,重启后的情况
	    cat $file |grep MASTER >/dev/null 2>&1
	    if [ $? -eq 0 ];then
		     do_log "$file has modified ,not do again"
             return
		fi
	    cat $file |grep SLAVE >/dev/null 2>&1
	    if [ $? -eq 0 ];then
		     do_log "$file has modified ,not do again"
             return
		fi
        echo "MASTER=${master}">>/etc/sysconfig/network-scripts/ifcfg-${adapter}
        echo "SLAVE=yes">>/etc/sysconfig/network-scripts/ifcfg-${adapter}
    else
        do_log "/etc/sysconfig/network-scripts/ifcfg-${adapter} not exsit, cant bond ${adapter} to ${master}"
    fi
}

function add_kernal_port
{
    local kernalname=$1    #kernal port的名字
    local nics=$2          #uplink port 列表     
	local ip=$3
	local mask=$4
	local bond_name=$5
	local nics_replasces
	
	
	echo $nics | grep '#' >/dev/null 2>&1
	if [ $? -ne 0 ]; then
	    #不需要做bond
		tunctl -t $kernalname
		#修改网口的ip
		update_config "/etc/sysconfig/network-scripts/ifcfg-$nics" "IPADDR" $ip
		update_config "/etc/sysconfig/network-scripts/ifcfg-$nics" "IPADDR" $mask
		service network restart
		do_bridge $nics "br_$nics"
		do_addif   "br_$nics" $kernalname
	else
	    #需要做bond
		tunctl -t $kernalname
		creat_bond_cfg_file $bond_name $ip $mask
		nics_replasces=${nics//#/" "}
		for i in $nics_replasces
		do
		    modify_eth_cfg_file $i $bond_name
		done
		#重启网络使得bond口生效
		service network restart
		do_bridge $bond_name "br_$bond_name"
		do_addif  "br_$bond_name" $kernalname 
		
	fi
}

function add_loop
{
   local nic=$1
   tunctl -t $nic
   if [ $? -ne 0 ];then
       do_log "tunctl -t $nic failed!"
   fi
}

function del_loop
{
   local nic=$1
   tunctl -d $nic
   if [ $? -ne 0 ];then
       do_log "tunctl -d $nic failed!"
   fi
}

function is_bond_slave
{
    return 0
}

function  is_in_bridge
{

}

case "$1" in
    create_bridge)
        do_bridge $2 $3
        ;;

    delete_bridge)
        stop_bridge $2 $3
        ;;
		
	create_bridge_noif)
        do_bridge_noif $2
        ;;
		
	delete_bridge_noif)
        del_bridge_noif $2
        ;;

    add_ifnet)
        add_if $2 $3 
        ;;
		
    del_ifnet)
	    del_if $2 $3
		;;
		
	is_bond_slave)
       is_bond_slave
	   ;;
    
    is_in_bridge)
       is_in_bridge
       ;;
	   
	add_vlan)
        add_vlan_eth $2 $3
        ;;
    
    del_Vlan)
        del_vlan_eth $2 $3
        ;;
		
	add_kernal)
       add_kernal_port 
        ;;	   
		
    add_loop)
       add_loop
         ;;	 

    del_loop)
       del_loop
         ;;			 
	
    *)
        echo "Unknown command: $1" >&2
        exit -1
esac




