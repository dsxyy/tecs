#!/bin/bash

logfile="/var/log/dobridge_ovs"
function do_logfile
{
    rm -rf $logfile
	touch $logfile
	echo "##this is vbridge logfile create by vnet version 2.0" >>$logfile
}

#��¼��־
function do_log
{
  local logstr=$1
  
  if [ ! -f /var/log/dobridge_ovs ];then
      touch $logfile
  fi
  local size=`ls -l /var/log/dobridge_ovs |awk '{print $5}'`
  LANG=en_US.ISO8859-1
 
  
  if [[ $size -gt 30000 ]];then
      rm -rf $logfile
	  touch $logfile
	  echo "##this is vbridge logfile create by vnet version 2.0" >>$logfile
  fi
  
  echo -n `date '+%b %d %T'` >> $logfile
  echo " $logstr" >>$logfile
}

function get_default_gw
{
    defaultgw_ip=`ip route list |grep default |awk  '{print $3}'`
	do_log "log default gw $defaultgw_ip" 
}

function set_default_gw
{
    if [ -n "$defaultgw_ip" ] ; then 
		route add default gw  $defaultgw_ip
		do_log "add  default gw $defaultgw_ip" 
    fi
}

function write_default_gw_file
{
  	#�ѵ�ǰ��Ĭ������д�뵱ǰĬ���������������������ļ��������DHCP��ʽ
	local route_nic=`ip route list |grep  default |awk '{print $5}'`
    if [ "$route_nic" != "" ];then
		get_ipaddr_adapter $route_nic
		if [ "$eth_dhcp" = "0" ];then
			if [ "$defaultgw_ip" != "" ];then
			    cat /etc/sysconfig/network-scripts/ifcfg-$route_nic |grep -i GATEWAY > /dev/null 2>&1
				if [ $? -ne 0 ];then
				    echo "GATEWAY=$defaultgw_ip" >> /etc/sysconfig/network-scripts/ifcfg-$route_nic
				    do_log "write GATEWAY=$defaultgw_ip to $route_nic"
				fi
			fi
		fi
    fi
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
    #ע�⣺���ĳ����ע�ͣ���ͷ��һ���ַ�������#��!!!
    local comment=`grep -c "^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*"  $file`
    
    if [[ $value == "#" ]];then
        if [ $exist -gt 0 ];then
            sed  -i "/^[^#]/s/$key[[:space:]]*=/\#$key=/" $file       
        fi
        return 0
    fi

    if [ $exist -gt 0 ];then
        #����Ѿ�����δע�͵���Ч�����У�ֱ�Ӹ���value
        sed  -i "/^[^#]/s#$key[[:space:]]*=.*#$key=$value#" $file
        
    elif [ $comment -gt 0 ];then
        #��������Ѿ�ע�͵��Ķ�Ӧ�����У���ȥ��ע�ͣ�����value
        sed -i "s@^[[:space:]]*#[[:space:]]*$key[[:space:]]*=[[:space:]]*.*@$key=$value@" $file
    else
        #������ĩβ׷����Ч������
        local timestamp=`env LANG=en_US.UTF-8 date`
        local writer=`basename $0`
        echo "" >> $file
        echo "$key=$value" >> $file
    fi
}

function check_nic_exit
{
    nic_is_exsit=0
    local nic=$1
    ifconfig $nic > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    nic_is_exsit=1
	fi	
}
function check_ovsmodel
{
    lsmod |grep openvswitch > /dev/null 2>&1
	if [ $? -ne 0 ];then
		#δ��װOVS,return
        do_log "ovs modle is not install return" 
		exit -1
	fi 	
}

#ʹ��check_ifsriov����뽫is_sriov��0
function check_ifsriov
{
    local adapter=$1
    local sriov_specia_num
	
	is_sriov=0
	echo  "check nic  $adapter is sriov or not"
	if [ -e /sys/class/net/$adapter/device ];then
        sriov_specia_num=`ls -ld /sys/class/net/$adapter/device |awk '{print $11}' |awk -F '/' '{print $4}'`
	else
        return	
	fi

    lspci -s $sriov_specia_num -v |grep 'SR-IOV' >/dev/null 2>&1
    if [ $? -eq 0 ];then
        lspci -s $sriov_specia_num -n |grep '8086:10c9' >/dev/null 2>&1	
        if [ $? -ne 0 ];then
            is_sriov=1
            do_log "check_ifsriov $adapter is SR-IOV"
            return 0
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


function get_ipaddr_adapter
{
    local ipstring
    local maskstring
    local eth_name_ip=$1
	
	eth_ip_copy=""
    eth_mask_copy=""
    eth_dhcp="0"

    eth_ip_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip | grep "^[[:space:]]*[^#]"|grep IPADDR |awk -F '=' '{print $2}'`
    eth_mask_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip |grep "^[[:space:]]*[^#]"|grep NETMASK |awk -F '=' '{print $2}'`
	
	cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip |grep "^[[:space:]]*[^#]"|grep -i BOOTPROTO |awk -F '=' '{print $2}'|grep -i DHCP > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    eth_dhcp="1"
	else
        eth_dhcp="0"	
	fi
	
	#������DHCP��ʽ��ȡ��ifconfig��ȡ
	if [ "$eth_ip_copy" = "" ]; then
	    ipstring=`ifconfig $eth_name_ip |grep 'inet addr' | awk '{print $2}'`
        eth_ip_copy=`echo "$ipstring" | awk -F ':' '{print $2}'`
	fi
	
	if [ "$eth_mask_copy" = "" ]; then
	    maskstring=`ifconfig $eth_name_ip |grep 'Mask' | awk '{print $4}'`
	    eth_mask_copy=`echo "$maskstring" | awk -F ':' '{print $2}'`
	fi
	
	do_log "get ip address of $1 ,ip:$eth_ip_copy ,mask:eth_mask_copy ,eth_dhcp:$eth_dhcp"
}

#�������������������ļ���Ϊ��ͨ�ã�����������漰��дIP������ дIP�����ܸ��� �ɺ���ĺ�������
function creat_nic_cfg_file
{
	local kernel=$1
	local is_dhcp=$2
	local bond_mode=$3
    local file="/etc/sysconfig/network-scripts/ifcfg-${kernel}"
	local bond_opt_str="\"mode=$bond_mode miimon=100\""

    do_log "enter creat_nic_cfg_file, nic is $kernel, is_dhcp:$2 bond_mode:$3"
    if [ -f $file ]; then
        echo "/etc/sysconfig/network-scripts/ifcfg-${kernel} has exsit, nic can do only one time!"
    else
        echo "DEVICE=${kernel}">>/etc/sysconfig/network-scripts/ifcfg-${kernel} 
		if [ "$is_dhcp" = "0" ];then
            echo "BOOTPROTO=static">>/etc/sysconfig/network-scripts/ifcfg-${kernel}
		else
		    echo "BOOTPROTO=dhcp">>/etc/sysconfig/network-scripts/ifcfg-${kernel}
		fi
		
		if [ "$bond_mode" != "" ];then
		    echo "BONDING_OPTS=$bond_opt_str">>/etc/sysconfig/network-scripts/ifcfg-${kernel} 
	    fi
		
    fi 
	return 0
}

function do_bridge
{
	  local netdev_name=$1
	  local bridge_name=$2
	  local nic_exsit="0"
	  local nic_isloop="0"
	  local nic_iskernel="0"
	  local result
	  
	  do_log "do_bridge $netdev_name $bridge_name"
	  #��һ����������������������������bridge_name
	  ovs-vsctl list-br |awk '{print $1}' |grep -x $bridge_name > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      do_log "$bridge_name is already bridge ,cant do_bridge again"
	      return 0
	  fi
	  
	  get_default_gw
	  
	  ovs-vsctl --may-exist add-br $bridge_name
	  if [ $? -ne 0 ];then
	      ehco "ovs-vsctl add-br $bridge_name failed!"
		  do_log "ovs-vsctl add-br $bridge_name failed!"
		  exit 
	  fi
	  
	  ovs-vsctl set bridge $bridge_name stp_enable=false
	  if [ $? -ne 0 ];then
	      ehco "ovs-vsctl set bridge $bridge_name stp_enable=false!"
		  do_log "ovs-vsctl set bridge $bridge_name stp_enable=false!"
		  exit 
	  fi
	  
	  ifconfig $netdev_name > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      nic_exsit="1"
	  fi
	  
	  echo $netdev_name |grep loop > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      nic_isloop="1"
	  fi
	  
	  echo $netdev_name |grep kernel > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      nic_iskernel="1"
	  fi
	  
	  if [ "$nic_iskernel" = "1" ];then
	      #kernel���ϲ��Ὠ���ţ�������̲������ߵ����ߵ����������
		  echo "do_bridge netdev is kernel nic, exit !"
		  do_log "do_bridge netdev is kernel nic, exit !"
		  exit
	  fi
	  
	  if [ "$nic_exsit" = "0" ] || [ "$nic_isloop" = "1" ];then
	      #���������ڻ������Ѿ����ڵ�loop�ڣ���Ҫ����Ϊinternalģʽ
		  ovs-vsctl --may-exist add-port $bridge_name $netdev_name  -- set interface $netdev_name  type=internal 
		  result=$?
		  do_log "exsit:$nic_exsit,loop:$nic_isloop"
		  do_log "ovs-vsctl add-port $bridge_name $netdev_name  -- set interface $netdev_name  type=internal result:$result"
	  else
	      ovs-vsctl --may-exist add-port $bridge_name $netdev_name
		  result=$?
		  do_log "ovs-vsctl add-port $bridge_name $netdev_name result:$result" 
	  fi
	  
      #�������ŵ������ļ�,IP��ʽDHCP����STATICȡ������������
	  get_ipaddr_adapter $netdev_name
      creat_nic_cfg_file $bridge_name $eth_dhcp
	  
	  #netdev�ĵ�ַ�Ƶ�bridge����
	  ifconfig $netdev_name 0
	  if [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
	      ifconfig $bridge_name $eth_ip_copy netmask $eth_mask_copy
	  fi
	  
	#��̬��ʽ����Ҫд
	if [ "$eth_dhcp" = "0" ];then
	      #�����ļ��е�IP�޸�.IP��������������
		  if [ "$eth_ip_copy" != "" ];then
		      echo "IPADDR=$eth_ip_copy" >> /etc/sysconfig/network-scripts/ifcfg-$bridge_name
		  fi
		  
		  if [ "$eth_mask_copy" != "" ];then
		      echo "NETMASK=$eth_mask_copy" >> /etc/sysconfig/network-scripts/ifcfg-$bridge_name
		  fi	
		  
		  #����Ǿ�̬IP��ʽ,ע�͵����������ļ������IP
    	  sed -i '/^IPADDR/s/^/##a#/'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name
		  sed -i '/^NETMASK/s/^/##a#/'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name 
		  sed -i '/^GATEWAY/s/^/##a#/'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name
	 fi
	   
    if [ "$eth_dhcp" = "1" ];then
		 #����Ƕ�̬IP��ʽ����Ҫ��IP��ʽ��Ϊ��̬
		 sed -i '/^BOOTPROTO/s/DHCP/STATIC/'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name
		 sed -i '/^BOOTPROTO/s/dhcp/static/'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name
		 echo "PRE_BOOT=DHCP" >>/etc/sysconfig/network-scripts/ifcfg-$netdev_name
	fi	 
	 
	 check_ifsriov $netdev_name
	 if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $netdev_name other-config:enable-vlan-splinters=true
		result=$?
		do_log "ovs-vsctl set interface $netdev_name other-config:enable-vlan-splinters=true result:$result"
	 fi
	 
     set_default_gw
	 write_default_gw_file
	 return 0
}

#�ú�����VNET2.0 C++�����в��ᱻ���ã�C++������õ���stop_bridge_noif
function stop_bridge
{
	  local netdev_name=$1
	  local bridge_name=$2
	  
	  do_log "stop_bridge $netdev_name $bridge_name"
	  
      get_default_gw 

      get_ipaddr_adapter $bridge_name	  
	  ovs-vsctl --if-exists del-br $bridge_name
	  
	  #ɾ�����������ļ�
	  rm -rf /etc/sysconfig/network-scripts/ifcfg-$bridge_name
	  
	  #��ԭע�͵����������ļ������IP,MASK,GATEWAY
	  sed -i '/^##a#/s/##a#//'  /etc/sysconfig/network-scripts/ifcfg-$netdev_name
	  	  
	  #��������������IPʹ��������Ч
	  if [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
	      ifconfig	$netdev_name $eth_ip_copy  netmask $eth_mask_copy
	  fi
	  
	  set_default_gw
	  write_default_gw_file
	  return 0
}

function set_controller
{
    local bridge_name=$1
    local controller=$2

    ovs-vsctl set-controller $bridge_name $controller

    return 0;
}

function del_controller
{
    local bridge_name=$1

    ovs-vsctl del-controller $bridge_name

    return 0;
}

function set_external_id
{
    local bridge=$1
    local port_name=$2
    local port_uuid=$3
    local mac=$4
    local vm_uuid=$5

    ovs-vsctl set port $port_name external-ids:iface-id=$port_uuid external-ids:iface-status=active external-ids:attached-mac=$mac external-ids:vm-uuid=$vm_uuid

    return 0;
}

function del_external_id
{
    local bridge=$1
    local port_name=$2	

    ovs-vsctl clear port $port_name external-ids

    return 0;
}

#����һ���յ����ţ���VNET2.0 C++�����в������
function do_bridge_noif
{
	local bridge_name=$1
	ovs-vsctl --may-exist add-br $bridge_name;
	if [ $? -ne 0 ];then
	    echo "/usr/local/bin/ovs-vsctl add-br $bridge_name"
		do_log "/usr/local/bin/ovs-vsctl add-br $bridge_name failed"
		return 0
	else
	    echo "/usr/local/bin/ovs-vsctl add-br $bridge_name !"
	fi
	return 0
}

function del_bridge_noif
{
	local bridge_name=$1
    local netif=`ovs-vsctl list-ports $bridge_name`
	
	echo "in del_bridge_noif, bridge:$bridge_name, get netif :$netif"
	
	check_ifsriov $netif
	if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $bridge_name other-config:enable-vlan-splinters=false
	    result=$?
	    do_log "ovs-vsctl set interface $netdev_name other-config:enable-vlan-splinters=false result:$result"  
	fi
	
	ovs-vsctl --if-exists del-br $bridge_name
	#ɾ�����������ļ�
	rm -rf /etc/sysconfig/network-scripts/ifcfg-$bridge_name
	return 0
}

#���ֻ��LOOP�ں�UPLINK�ڼ������ſ���.KERNEL�ڼ������Ų����������
function add_if
{
    local bridge=$1
	local ifnet=$2
	local vlan=$3
	local is_kernel="0"
	local is_loop="0"
	local is_exsit="0"
	local is_bond="0"
	local netdev_ip
	local netdev_mask
	local bridge_ip
	local bridge_mask
	local bridge_dhcp
	
    get_default_gw
	do_log "add if $bridge $ifnet $vlan"
	
	if [ "$vlan" != "0" ];then
	    do_log "vlan is not 0, exit"
		exit
	fi
	
	ifconfig $ifnet > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_exsit=1
	fi
	
	echo $ifnet |grep loop > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_loop=1
	fi
	
	echo $ifnet |grep kernel > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_kernel=1
	fi
	
	if [ "$is_kernel" = "1" ];then
	    echo "add_if is not user for kernel port"
		do_log "add_if is not user for kernel port"
		exit
	fi
	
	echo $ifnet |grep bond > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_bond=1
	fi

	
	do_log "in add_if is_exsit:$is_exsit, is_loop:$is_loop , is_bond:$is_bond"
	
    if [ "$is_exsit" = "1" ] && [ "$is_loop" = "0" ] ; then
		#ifnet����.���Ҳ���loop�ڣ����Ҳ���bond�ڣ�����Ϊinternal
	    ovs-vsctl --may-exist add-port  $bridge $ifnet
	    do_log "ovs-vsctl add-port  $bridge $ifnet resylt $?"
	else 
		#ifnet�����ڣ�Ҫ���internalģʽ
        ovs-vsctl --may-exist add-port  $bridge $ifnet  -- set interface $ifnet  type=internal
        do_log "ovs-vsctl add-port  $bridge $ifnet  -- set interface $ifnet  type=internal result $?"		
    fi	    		   
	
	#����������������������(kernel�ڣ�loop��)��������IP����ֱ���˳�
	if [ "$is_loop" = "1" ] ;then
	      #����һ�������������ļ�
	      if [ ! -f /etc/sysconfig/network-scripts/ifcfg-$bridge ];then
		      do_log "in add_if bridge config file is null!"
		      echo "DEVICE=${bridge}">>/etc/sysconfig/network-scripts/ifcfg-${bridge} 
		  fi
	      do_log "is loop nic,return"
	      return 0
	fi
	
	#��ͨuplink��������¼��if��IP���
	get_ipaddr_adapter $ifnet
	netdev_ip=$eth_ip_copy
	netdev_mask=$eth_mask_copy
	
	get_ipaddr_adapter $bridge
	bridge_ip=$eth_ip_copy
	bridge_mask=$eth_mask_copy
	bridge_dhcp=$eth_dhcp
	
	#�������û��IP,netdev��IP�Ƶ�������ȥ
	if [ "$netdev_ip" != "" ] && [ "$bridge_ip" = "" ];then
	    do_log "in add_if netdev has ip ,and bridge not ip ,transfer ip from netdev to bridge"
	    ifconfig $ifnet 0 >/dev/null 2>&1
	    ifconfig $bridge $netdev_ip netmask $netdev_mask
		#�޸�netdev��IP�����ļ���ע�͵�
	    sed -i '/^IPADDR/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$ifnet
		sed -i '/^NETMASK/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$ifnet
		sed -i '/^GATEWAY/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$ifnet
		#��������Ǿ�̬ģʽ�������netdev��IP����Ϣд�����ŵ������ļ�
		if [ "$bridge_dhcp" = "0" ];then
			echo "IPADDR=$netdev_ip" >> /etc/sysconfig/network-scripts/ifcfg-$ifnet
			echo "NETMASK=$netdev_mask" >> /etc/sysconfig/network-scripts/ifcfg-$ifnet
	    fi
	fi
	
	#����ģʽ�޸���Ϊ��̬����ֹ�������Ŷ���DHCPģʽ������£�Ҫ��ͬһ��IP
    if [ "$eth_dhcp" = "1" ];then
		 #����Ƕ�̬IP��ʽ����Ҫ��IP��ʽ��Ϊ��̬
		 sed -i '/^BOOTPROTO/s/DHCP/STATIC/'  /etc/sysconfig/network-scripts/ifcfg-$ifnet
		 sed -i '/^BOOTPROTO/s/dhcp/static/'  /etc/sysconfig/network-scripts/ifcfg-$ifnet
		 echo "PRE_BOOT=DHCP" >>/etc/sysconfig/network-scripts/ifcfg-$ifnet
	fi	
	
	check_ifsriov $ifnet
	if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=true
		result=$?
		do_log "ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=true result:$result"
	fi
	
	set_default_gw
	write_default_gw_file
	return 0
}

#���ֻ��LOOP�ں�UPLINK�ڼ������ſ���.KERNEL�ڼ������Ų����������
function add_if_ovs
{
    local bridge=$1
	local ifnet=$2
	local is_exsit=0
	local is_loop=0
	
	do_log "in add_if_ovs bridge is:$bridge, ifnet is:$ifnet"

	ifconfig $ifnet > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_exsit=1
	fi
	
	echo $ifnet |grep loop > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  is_loop=1
	fi
	
    if [ "$is_exsit" = "1" ] && [ "$is_loop" = "0" ] ; then
		#ifnet����.���Ҳ���loop�ڣ����Ҳ���bond�ڣ�����Ϊinternal
	    ovs-vsctl --may-exist add-port  $bridge $ifnet
	    do_log "ovs-vsctl add-port  $bridge $ifnet resylt $?"
	else 
		#ifnet�����ڣ�Ҫ���internalģʽ
        ovs-vsctl --may-exist add-port  $bridge $ifnet  -- set interface $ifnet  type=internal
        do_log "ovs-vsctl add-port  $bridge $ifnet  -- set interface $ifnet  type=internal result $?"		
    fi	  
	
	check_ifsriov $ifnet
	if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=true
		result=$?
		do_log "ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=true result:$result"
	fi
	
	return 0
}

function del_if
{
    local bridge=$1
	local ifnet=$2
	local br_ip
	local br_mask
	local dev_ip
	local is_loop="0"
	local is_kernel="0"
	local ifs
	
	do_log "del if $bridge $ifnet"
	
	echo $ifnet |grep loop > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    is_loop=1
	fi   
	
	echo $ifnet |grep kernel > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    is_kernel=1
	fi
	
    ovs-vsctl --if-exists del-port  $bridge $ifnet 
	if [ $? != 0 ];then
	    do_log "ovs-vsctl del-port  $bridge $ifnet failed!"
	fi
	
	#���ifcfg�ļ���IP MASK GATEWAY��ע�͵�����ԭ
	#���Ƕ�UPLINK�ڵ����(��kernel,��loop)
	sed -i '/IPADDR/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$ifnet
	sed -i '/NETMASK/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$ifnet
	sed -i '/GATEWAY/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$ifnet
	
	#�����ϵ�IP��������������ϵ�IPһ�£���ȥ�����ŵ�IP
	get_ipaddr_adapter $bridge
	br_ip=$eth_ip_copy
	br_mask=$eth_mask_copy
	
	#ǰ���netdev�������ļ��е�IP����ԭ�ˣ�������get_ipaddr_adapter����ȡ����������ȡ��������ΪDHCPû���棬���滹�д���
	get_ipaddr_adapter $ifnet
	dev_ip=$eth_ip_copy
	
	do_log "in del_if bridge ip=$br_ip ,bridge mask=$br_mask, netdev ip=$dev_ip"
	
	if [ "$br_ip" = "$dev_ip" ];then
	   if [ "$br_ip" != "" ];then
	        ifconfig $bridge 0
			sed -i '/IPADDR/d' /etc/sysconfig/network-scripts/ifcfg-$bridge
		    sed -i '/NETMASK/d' /etc/sysconfig/network-scripts/ifcfg-$bridge
		    sed -i '/GATEWAY/d' /etc/sysconfig/network-scripts/ifcfg-$bridge
			ifconfig $ifnet $br_ip netmask $br_mask
			do_log "bridge ip same with netdev ip, set netdev ip and remove bridge ip.ip:$eth_ip_copy mask:$eth_mask_copy"
       fi	   
	   
	   ifs=`ovs-vsctl list-ifaces $bridge`
	   if [ -z $ifs ];then
		   do_log "bridge has no interface, delete the ip in cfg file"
	   fi
	   
	fi
	
	#��ԭԭ��������������ʽ
	cat /etc/sysconfig/network-scripts/ifcfg-$ifnet |grep PRE_BOOT > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    sed -i '/BOOTPROTO/d' /etc/sysconfig/network-scripts/ifcfg-$ifnet
		echo "BOOTPROTO=DHCP">>/etc/sysconfig/network-scripts/ifcfg-$ifnet
		
		dhclient $ifnet -r
		dhclient $ifnet &
	fi
	sed -i '/PRE_BOOT/d' /etc/sysconfig/network-scripts/ifcfg-$ifnet
	
	#���ﴦ��Ƚϸ��ӣ����DHCP�������DHCP���������ļ�û�б����ַ
	get_ipaddr_adapter $ifnet
	dev_ip=$eth_ip_copy
	
	if [ "$br_ip" = "$dev_ip" ];then
	   if [ $"br_ip" != "" ];then
	        ifconfig $bridge 0
			do_log "bridge ip same with netdev ip, set netdev ip and remove bridge ip.ip:$eth_ip_copy mask:$eth_mask_copy"
       fi	   
	fi
	
	check_ifsriov $ifnet
	if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=false
		result=$?
		do_log "ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=false result:$result"
	fi
	
	set_default_gw
	write_default_gw_file
	return 0
}


function del_if_ovs
{
    local bridge=$1
	local ifnet=$2
	local ifs
	
	do_log "del_if_ovs $bridge $ifnet"
	
    ovs-vsctl --if-exists del-port  $bridge $ifnet 
	if [ $? != 0 ];then
	    do_log "ovs-vsctl del-port  $bridge $ifnet failed!"
	fi
	
	check_ifsriov $ifnet
	if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=false
		result=$?
		do_log "ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=false result:$result"
	fi
	return 0
}


function add_vlan_eth
{
   local nicname=$1
   local vlanid=$2
   do_log "add_vlan_eth $nicname $vlanid"
   ovs-vsctl -- set port $nicname "tag=$vlanid"
   if [ $? != 0 ];then
	  do_log "ovs-vsctl -- set port $nicname tag=$vlanid failed!"
	  return 1
   fi
   return 0
}

function del_vlan_eth
{
   local nicname=$1
   local vlanid=$2
   do_log "del_vlan_eth $nicname $vlanid"
   ovs-vsctl -- remove port $nicname "tag $vlanid"
   if [ $? != 0 ];then
	  do_log "ovs-vsctl -- remove port $nicname tag $vlanid failed!"
	  return 1
   fi
   return 0
}

function set_native_vlan_eth 
{
    local nicname=$1
	local native_vlan_id=$2
	local bridge
	
	do_log "set_native_vlan_eth $nicname $native_vlan_id"

    ovs-vsctl -- set port $nicname  tag=$native_vlan_id vlan_mode=native-untagged
    if [ $? != 0 ];then
	   do_log "ovs-vsctl -- set port $nicname  tag=$native_vlan_id vlan_mode=native-untagged failed!"
	   return 1
    fi
    return 0
	
}

function add_kernal_port
{
    local kernalname=$1    #kernal port������
    local bridge_name=$2   #���ŵ�����   
	local nics=$3          #uplink port����  
	local ip
	local mask
	local is_dhcp=0
	local vlan
	local mtu
	local paranum=$#
	local mac
	
	if [ $paranum = 8 ];then
	    ip=$4
		mask=$5
		is_dhcp=$6
		vlan=$7
                mtu=$8
	elif [ $paranum = 6 ];then
	    is_dhcp=1
		vlan=$5
		mtu=$6
	else
       do_log "para number is not 8 or 6, is :$paranum"
       exit 0	   
	fi
	
	get_default_gw
	
	do_log "add_kernal_port kernelname:$1 bridgename:$2 nic:$3 ip:$4 mask:$5 is_dhcp:$6"
	
	ovs-vsctl --may-exist add-port $bridge_name $kernalname -- set interface $kernalname type=internal
	mac=`ifconfig $kernalname |grep HWaddr|awk '{print $5}'|tr -d ' '`
	ovs-vsctl set interface $kernalname mac=\"$mac\"
	
	#����kernel�ڵ������ļ�
    creat_nic_cfg_file $kernalname $is_dhcp
	if [ "$is_dhcp" = "0" ];then
		if [ "$ip" != "" ];then
			echo "IPADDR=$ip" >> /etc/sysconfig/network-scripts/ifcfg-$kernalname
		fi
		
		if [ "$mask" != "" ];then
			echo "NETMASK=$mask" >> /etc/sysconfig/network-scripts/ifcfg-$kernalname 
		fi	
		
		ifconfig $kernalname $ip netmask $mask >/dev/null 2>&1
	fi	

	ifconfig $kernalname up >/dev/null 2>&1
	
        ovs-vsctl set port $kernalname tag=$vlan
        if [ "$mtu" != "0" ];then
            ip link set $kernalname mtu $mtu
        fi

	if [ "$is_dhcp" = "1" ];then
	   dhclient $kernalname -r
	   dhclient $kernalname &
	fi
	
	set_default_gw
	write_default_gw_file
    return 0
}

function del_kernal_port
{
    local bridge_name=$1    #���ŵ����� 
    local kernalname=$2     #kernal port������
	local uplink
	local ports_of_bridge

	do_log "del_kernal_port kernelname:$2 bridgename:$1 "

    get_default_gw	
	#dvs_kernel������ɾ��
	if [ "$kernalname" = "dvs_kernel" ];then
	    echo "dvs_kernel is not allow to delete!"
	    do_log "dvs_kernel is not allow to delete!"
		return 0
	fi

        ifconfig $kernalname 0
	ovs-vsctl --if-exists del-port $bridge_name $kernalname
	
	#�����������û���˶˿ڣ���ɾ�����ŵ������ļ�
	ports_of_bridge=`ovs-vsctl list-ports $bridge_name`
	if [ -z "$ports_of_bridge" ];then
       rm -rf /etc/sysconfig/network-scripts/ifcfg-$bridge_name
	fi
	
	#ifconfig $kernalname 0
	rm -rf /etc/sysconfig/network-scripts/ifcfg-$kernalname
	
    set_default_gw
	write_default_gw_file
	return 0
}

function set_port_mode
{
    local port=$1
	local mode=$2
	do_log "set_port_mode $port $mode"
	ovs-vsctl -- set port $port vlan_mode=$mode
	if [ $? -ne 0 ];then
		do_log "ovs-vsctl -- set port $port vlan_mode=$mode failed"
        return 0
	fi
	return 0
}

function del_port_mode
{
    local port=$1
	local mode=$2
	do_log "del_port_mode $port $mode"
	ovs-vsctl -- remove port $port vlan_mode $port
	if [ $? -ne 0 ];then
		do_log "ovs-vsctl -- remove port $port vlan_mode $port"
        return 0
	fi
	return 0
}

function add_loop
{
   local nic=$1
   local is_dhcp=0
      
   tunctl -t $nic
   if [ $? -ne 0 ];then
       do_log "tunctl -t $nic failed!"
   fi
   ifconfig $nic up >/dev/null 2>&1
   
   creat_nic_cfg_file $nic $is_dhcp
   
   return 0
}

function del_loop
{
   local nic=$1
   tunctl -d $nic
   if [ $? -ne 0 ];then
       do_log "tunctl -d $nic failed!"
   fi
   
   rm -rf /etc/sysconfig/network-scripts/ifcfg-$nic
   return 0
}

function is_bond_slave
{
    local nic=$1
	local cfg_file="/etc/sysconfig/network-scripts/ifcfg-$nic"
	local bond
	
	ifconfig $nic |grep SLAVE >/dev/null 2>&1
	if [ $? -ne 0 ];then
	    echo "error"
	    return 1
	fi
	
	bond=`cat $cfg_file |grep MASTER |awk -F "=" '{print $2}'`
	if [ "$bond" != "" ];then
	echo $bond
	fi
	
	return 0
}
	       
function is_bond_slave_ovs
{
    local nic=$1
	local nic_uuid
	local nic_all_uuids
	local bond
	nic_uuid=`ovs-vsctl list interface $nic |grep _uuid |awk -F ":" '{print $2}' |tr -d ' '`
	if [ "$nic_uuid" = "" ];then
	    do_log "is_bond_slave_ovs  nic uuid is null"
		echo "error"
	    return 1
	fi
	
	nic_all_uuids=`ovs-vsctl find port  |grep $nic_uuid |awk -F ":" '{print $2}' |tr -d "["|tr -d "]"|tr -d '"' |tr -d ' '`
	if [ "$nic_all_uuids" = "" ];then
	    do_log "is_bond_slave_ovs  nic_all_uuids is null"
		echo "error"
	    return 1
	fi
	
	bond=`ovs-vsctl find port interface=$nic_all_uuids |grep name |awk -F ":" '{print $2}' |tr -d '""'|tr -d ' '`
	if [ "$bond" = "" ];then
	    do_log "is_bond_slave_ovs  bond is null"
		echo "error"
	    return 1
	fi
	
	echo $bond |grep bond > /dev/null
	if [ $? -ne 0 ];then
	    do_log "$bond grep bond is null"
		echo "error"
	    return 1
	fi
	
	if [ "$bond" != "" ];then
	    echo $bond
	fi
	
	return 0
	
}

	       
function is_in_bridge
{
    local nic=$1
	local bridge
	bridge=`ovs-vsctl port-to-br $nic`
	if [  "$bridge" = "" ];then
	    echo "error"
		return 1
	fi
    echo $bridge
	return 0

}

function id_to_name
{
   local id=$1
   ovs-vsctl -- --columns=name find interface  external-ids:iface-id=$id |awk -F '"' '{print $2}'
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
		
    set_controller)
        set_controller $2 $3
        ;;

    del_controller)
        del_controller $2
        ;;

    set_external_id)
        set_external_id $2 $3 $4 $5 $6
        ;;
		
    del_external_id)
        del_external_id $2 $3
        ;;

    add_ifnet)
        add_if $2 $3 $4
        ;;
		
    add_ifnet_ovs)
        add_if_ovs $2 $3
        ;;
	        
	  
    del_ifnet)
	    get_default_gw
	    del_if $2 $3
		set_default_gw
		;;
	
	del_ifnet_ovs)
	    get_default_gw
	    del_if_ovs $2 $3
		set_default_gw
		;;
		
    set_port_mode)
         set_port_mode $2 $3
         ;;
    
    del_port_mode)
         del_port_mode $2 $3
        ;;		  
		
    add_vlan)
        add_vlan_eth $2 $3
        ;;

    del_vlan)    
	   del_vlan_eth $2 $3
	   ;; 
	   
	set_native_vlan) 
	   set_native_vlan_eth $2 $3
        ;;	
	   
	create_bridge_noif)
	   do_bridge_noif $2
	   ;;
		
	delete_bridge_noif)
       del_bridge_noif $2
       ;;
		
    is_bond_slave)
       is_bond_slave $2
	   ;;
    
	is_bond_slave_ovs)
       is_bond_slave_ovs $2
	   ;;
	   
    is_in_bridge)
       is_in_bridge $2
       ;;	   
	
	add_kernel)
       add_kernal_port $2 $3 $4 $5 $6 $7 $8 $9
        ;;	
		
	del_kernel)
       del_kernal_port $2 $3 
        ;;	
	
	add_loop)
       add_loop $2
	   
	    ;;
		
	del_loop)
       del_loop $2 
	    ;;
		
	id_to_portname)
       id_to_name $2	
	   ;;	
    *)
        echo "Unknown command: $1" >&2
        exit 1
esac




