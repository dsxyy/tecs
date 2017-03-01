#!/bin/bash
if [ ! "$_CREATE_KERNEL_PORT_FILE" ];then
_CREATE_KERNEL_PORT_DIR=`pwd`
#头文件包含
cd $_CREATE_KERNEL_PORT_DIR/../common/
.  tecs_common_func.sh

cd $_CREATE_KERNEL_PORT_DIR

component=tecs-vna
logfile="/var/log/kernal"
function do_logfile
{
    rm -rf $logfile
	touch $logfile
	echo "##this is kernal port logfile create by vnet version 2.0" >>$logfile
}

#记录日志
function do_log
{
  local logstr=$1
  LANG=en_US.ISO8859-1
  echo -n `date '+%b %d %T'` >> $logfile
  echo " $logstr" >>$logfile
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

function check_ifhasip
{
    is_hascfg_ip=0
    local nic=$1
	local ip
	local file="/etc/sysconfig/network-scripts/ifcfg-$nic"
	ifconfig $nic |grep "inet addr"  > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    is_hascfg_ip=1
		return
	fi
	
	ip=`cat /etc/sysconfig/network-scripts/ifcfg-$nic | grep "^[[:space:]]*[^#]"|grep IPADDR |awk -F '=' '{print $2}'`
    if [ "$ip" != "" ];then
		is_hascfg_ip=1
		return
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
    lsmod |grep openvswitch_mod > /dev/null 2>&1
	if [ $? -ne 0 ];then
		#未安装OVS,return
        do_log "ovs modle is not install return" 
		exit -1
	fi 	
}

function get_default_gw
{
    defaultgw_ip=`route |grep default |awk  '{print $2}'`
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
  	#把当前的默认网关写入当前默认网关所在网卡的配置文件，如果是DHCP方式
	local route_nic=`route |grep  default |awk '{print $8}'`
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

function do_addif
{
    local bridge=$1
	local ifnet=$2
	local mac
	
	check_nic_exit $ifnet
    do_log "add if $bridge $ifnet "

 	    if [ "$nic_is_exsit" = "1" ]; then
	        ovs-vsctl add-port  $bridge $ifnet 
	do_log "ovs-vsctl add-port  $bridge $ifnet ret: $?"
		else
            ovs-vsctl add-port  $bridge $ifnet  -- set interface $ifnet  type=internal
	do_log "ovs-vsctl add-port  $bridge $ifnet ret: $?"		
            mac=`ifconfig $ifnet |grep HWaddr|awk '{print $5}'|tr -d ' '`
			ovs-vsctl set interface $ifnet mac=\"$mac\"
 			
	fi	
	return 0
}


#使用check_ifsriov后必须将is_sriov清0
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

function do_bridge
{
	  local netdev_name=$1
	  local bridge_name=$2
	  
	  echo  "do_bridge $netdev_name $bridge_name"
	  do_log "do_bridge $netdev_name $bridge_name"
	  #第一列是网桥名，现有网桥名不能是bridge_name
	  ovs-vsctl show 2>/dev/null |awk '{print $1}' |grep -x $bridge_name > /dev/null 2>&1
	  if [ $? -eq 0 ];then
	      do_log "$bridge_name is already bridge ,cant do_bridge again, will return"
	      return 0
	  fi
	  
	  ovs-vsctl add-br $bridge_name
	  ovs-vsctl add-port $bridge_name $netdev_name
	  if [ $? -ne 0 ];then
		  do_log "creat bridge use ovs netdev is $netdev_name bridgename is $bridge_name failed "
		  return;
	  fi
	  
	  check_ifsriov  $netdev_name
	  if [ "$is_sriov" = "1" ];then
	    ovs-vsctl set interface $netdev_name other-config:enable-vlan-splinters=true
		result=$?
		do_log "ovs-vsctl set interface $ifnet other-config:enable-vlan-splinters=true result:$result"
	  fi
	  
	  return 0
}


function do_bridge_bond
{
   local bond=$1
 
   ifconfig $bond |grep 00:00:00:00:00:00 >/dev/null 2>&1
   if [ $? -ne 0 ]; then
      do_log "will do_bridge $bond"
      do_bridge $bond
   fi
}

#创建虚拟网卡的配置文件，为了通用，这个函数不涉及到写IP操作。 写IP操作很复杂 由后面的函数处理
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


#非法则为1 
function check_bond
{
   local slave=$1
   is_bond_invalid=0
   
   do_log "check $slave"
   
   ifconfig $slave |grep SLAVE >/dev/null 2>&1
   if [ $? -eq 0 ];then
	   is_bond_invalid=1
	   do_log "ifconfig check_bond $slave is SLAVE cant do bond again!"
       return
   fi
	   
   cat /etc/sysconfig/network-scripts/ifcfg-$slave | grep "^[[:space:]]*[^#]"|grep SLAVE >/dev/null 2>&1
   if [ $? -eq 0 ];then
	  is_bond_invalid=1
	  do_log "cfg file check_bond $slave is SLAVE cant do bond again!"
      return
   fi


}

function check_adatper
{
    local adapter=$1
    local path
    local adatper_name_str

    check_adatper_valid=0

    if [[ `ls -al /sys/class/net/ | grep pci |awk '{print $9}'|grep -x "$adapter"` == "" ]];then
        check_adatper_valid=1
        echo "$adapter is not physic port"
        do_log "check_adatper $adapter failed!"
        return
    fi

    path="/etc/sysconfig/network-scripts/ifcfg-${adapter}"
    if [ -e $path ]; then 
        adatper_name_str=`cat $path |grep DEVICE |grep $adapter`
        [ "$adatper_name_str" = "" ]&&do_log "check_adatper $adapter failed!"&&check_adatper_valid=1
    else
        check_adatper_valid=1&&do_log "check_adatper $adapter failed"
    fi
}

eth_ip_copy=""
eth_mask_copy=""
eth_dhcp="0"
function get_ipaddr_adapter
{
    local ipstring
    local maskstring
    local eth_name_ip=$1
	
    eth_ip_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip | grep "^[[:space:]]*[^#]"|grep IPADDR |awk -F '=' '{print $2}'|tr -d '"'`
    eth_mask_copy=`cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip |grep "^[[:space:]]*[^#]"|grep NETMASK |awk -F '=' '{print $2}'|tr -d '"'`
	
	cat /etc/sysconfig/network-scripts/ifcfg-$eth_name_ip |grep "^[[:space:]]*[^#]"|grep -i BOOTPROTO |awk -F '=' '{print $2}'|grep -i DHCP > /dev/null 2>&1
	if [ $? -eq 0 ];then
	  eth_dhcp="1"
	else
        eth_dhcp="0"	
	fi
	
	#网卡配置文件里面读不到IP ，可能是DHCP方式获取，ifconfig获取
	if [ "$eth_ip_copy" = "" ]; then
	    ipstring=`ifconfig $eth_name_ip |grep 'inet addr' | awk '{print $2}'`
            eth_ip_copy=`echo "$ipstring" | awk -F ':' '{print $2}'`
	fi
	
	if [ "$eth_mask_copy" = "" ]; then
	    maskstring=`ifconfig $eth_name_ip |grep 'Mask' | awk '{print $4}'`
	    eth_mask_copy=`echo "$maskstring" | awk -F ':' '{print $2}'`
	fi
}

public_nic_of_ip=""
function get_nic_of_public_ip
{
    local default_route_nic=`route |grep default  |awk '{print $8}'`
	local ip_list=`ifconfig  | grep "inet addr:" | grep -v "127.0.0.1" | awk -F' ' {'print $2'} | awk -F':' {'print $2'}`
	local one_ip
	local one_ip_nic
	
	if [ "$default_route_nic" != "" ];then
	    check_adatper $default_route_nic
		if [ "$check_adatper_valid" = "0" ];then
		    public_nic_of_ip=$default_route_nic
		    return
		fi
	fi
	
	for one_ip in $ip_list
	do
	    one_ip_nic=`ifconfig |grep $one_ip |grep -B  1 $one_ip | grep HWaddr |awk '{print $1}'`
		if [ "$one_ip_nic" != "" ];then
			check_adatper $one_ip_nic
			if [ "$check_adatper_valid" = "0" ];then
				public_nic_of_ip=$one_ip_nic
				return
			fi
	   fi
	done
	
}

function config_upkernal_and_check
{	
    local all_nics=`ls -al /sys/class/net/ | grep pci |awk '{print $9}'`
    local default_nic=""

    get_nic_of_public_ip
    if [ "$public_nic_of_ip" != "" ];then
        all_nics="$public_nic_of_ip $all_nics"
    fi
	
    if [[ "$all_nics" == "" ]];then
        do_log "no uplink port for kernel, and will exit"
        echo "no uplink port for kernel, set up will exit"
        exit
    fi
   
    default_nic=`echo $all_nics |awk '{print $1}'`

    get_install_config "$component" "uplink_port" "string" "please input the uplink kernel port"  $default_nic
    check_adatper "$config_value"
    if [ "$check_adatper_valid" = "0" ];then
        return
    else
        config_upkernal_and_check
    fi	
}


function check_adapter_is_in_bridge
{
     local nic=$1
     is_inbridge="0"
     ovs-vsctl show |grep "Port \"$nic\"" > /dev/null 2>&1
    if [ $? -eq 0 ];then
       is_inbridge="1"
    fi
}

function config_bond_upkernal_and_check
{	
    local all_nics=`ls -al /sys/class/net/ | grep pci |awk '{print $9}'`
    local default_nics=`echo $all_nics |awk '{print $1 " " $2}'`
    local one_nic
    
    echo default_nics $default_nics
    
    get_install_config "$component" "uplink_port" "string" "please input ports to do bonding,seperator with space ,such as eth0 eth1"  "$default_nics"
    bond_slaves=$config_value
	
    if [[ "" == `echo $bond_slaves |awk '{print $2}'` ]];then
        echo "please input one more ports to do bonding!"
        config_bond_upkernal_and_check
        return
    fi
	
    for i in $bond_slaves
    do
        check_adatper $i
        check_adapter_is_in_bridge $i
        check_bond $i
        if [ "$check_adatper_valid" = "1" ];then
            echo "$i is not phyic port!"
            config_bond_upkernal_and_check
            return
        fi
		
        if [ "$is_inbridge" = "1" ];then
            echo "$i is already interface of other bridge!"
            config_bond_upkernal_and_check
            return
        fi
		
        if [ "$is_bond_invalid" = "1" ];then
            echo "$i is already already bond slave!"
            config_bond_upkernal_and_check
            return
        fi
		
        #检测是否2个相同的网卡
        if [ "$one_nic" = "$i" ];then
            echo "nic is same"
            config_bond_upkernal_and_check
            return
        else
            one_nic=$i		
        fi
		
    done
	
    config_bond_mode
}

function config_bond_mode
{
    get_install_config "$component" "bond_mode" "string" "please enter the bond mode ,1(active back-up), 4(lacp)" "1"
    if [ "$config_value" != "1" ]&&  [ "$config_value" != "4" ];then
        echo "model $config_value is not 1(active back-up) or 4(lacp)"
        config_bond_mode
        return
    fi

    global_bond_mode=$config_value
}

function do_install
{
    local nic_name="dvs_kernel"
    local br_name="sdvs_00"
    local is_using_bonding=$1
    local bonding_mode=$2
    local uplink_port=$3
   
    do_logfile
    do_log "=================================="
   
   echo "in do_install para is $1 $2 $3"
   
   ifconfig $nic_name >/dev/null 2>&1
   if [  $? -eq 0 ];then
   #已经有dvs_kernel直接退出
       do_log "dve_kernel is exsit, return"
       return
   else	  
        config_kernal_port $is_using_bonding $bonding_mode $uplink_port
   fi
   
}

function add_bond_ovs
{
    local br_name=$1
    local bond_name=$2
    local slaves=$3
    local mode=$4
    local relpase_slaves=${slaves//#/" "}
    local ret
    local dhcp_tmp=""
	
    do_log "will add_bond_ovs bridge:$br_name,bond_name=$bond_name  mode=$mode  slaves=$slaves"
		
    #取第一个有IP地址的网卡配置作为bond口的配置
    #如果都没有IP地址，则以第一个网卡的配置为准
    for i in $relpase_slaves
    do
        get_ipaddr_adapter $i

        if [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
            break;
        fi

        if [ "$dhcp_tmp" = "" ];then
            dhcp_tmp=$eth_dhcp
        else
            if [ "$eth_ip_copy" = "" ];then
                eth_dhcp=$dhcp_tmp
            fi
        fi
    done

    if [ "$eth_dhcp" = "0" ] && [ "$mode" != "all" ];then    
        if [ "$eth_ip_copy" = "" ];then            
            get_install_config "$component" "ip_addr" "string" "please input the ip address of dvs_kernel"  ""
            eth_ip_copy="$config_value"
	
            get_install_config "$component" "ip_mask" "string" "please input the mask address of dvs_kernel"  ""
            eth_mask_copy="$config_value"
        fi
    fi
	
    do_log "br_name:$br_name ,$bond_name nic:$i get ip $eth_ip_copy mask $eth_mask_copy"
    ovs-vsctl add-br $br_name
    ovs-vsctl add-bond $br_name $bond_name $relpase_slaves
    ret=$?
    do_log "ovs-vsctl add-bond $br_name $bond_name $relpase_slaves -- set port $bond_name bond_mode=active-backup ret:$ret"
	
    if [ "$mode" = "4" ];then
        ovs-vsctl set port $bond_name lacp=active
        ovs-vsctl set port $bond_name bond_mode=balance-slb
    else
        ovs-vsctl set port $bond_name bond_mode=active-backup
    fi
    
	# 需要判断是否是sriov口，如果是必须设置splinter true
	for j in $relpase_slaves
	do 
		check_ifsriov $j
	    if [ "$is_sriov" = "1" ];then
	        ovs-vsctl set interface $j other-config:enable-vlan-splinters=true
		    result=$?
		    do_log "ovs-vsctl set interface $j other-config:enable-vlan-splinters=true result:$result"
		    echo "ovs-vsctl set interface $j other-config:enable-vlan-splinters=true result:$result"
	    fi
	done 
    
    # 有可能有IP的网卡都需要将IP去掉
    for i in $relpase_slaves
    do 
        ifconfig $i 0
    done
}

#该函数用来创建bond
#传入参数为  1>修改的网卡名
#            2>修改bond口的模式默认是0，负荷分担。支持修改为1 主备,4 802.3AD
#            3>配置的SLAVE

function add_bond
{
    local bond_name=$1
	local bond_modle=$2
	local slaves=$3
	local relpase_slaves=${slaves//#/" "}
	local ip_nic
	local nic
	
	do_log "will add_bond name=$bond_name  mode=$bond_modle  slaves=$slaves"
		
	modprobe bonding miimon=1000 mode=4 >/dev/null 2>&1	
	
	#取到第一个有IP的网卡的IP
	for i in $relpase_slaves
	do
	    get_ipaddr_adapter $i
	    if [ "$eth_ip_copy" != "" ];then
		   ifconfig $i 0
	       break
	    fi
	done

	do_log "$bond_name get ip $eth_ip_copy mask $eth_mask_copy"
	
	echo +$bond_name > /sys/class/net/bonding_masters
	if [ $? -ne 0 ] ;then
	    #此处是明确的知道错误，返回为1，通常是不知道是否错误 都返回0
	    do_log "echo +$bond_name > /sys/class/net/bonding_masters failed"
		return 1
	fi
	
	sleep 1
	
	for nic in $relpase_slaves
	do
	   ifconfig $nic down > /dev/null 2>&1
	   echo +$nic > /sys/class/net/$bond_name/bonding/slaves
	   ifconfig $nic up   > /dev/null 2>&1
       modify_eth_cfg_file $nic  $bond_name  		
	done
	
    echo 100 > /sys/class/net/$bond_name/bonding/miimon
	
	    ifconfig $bond_name down
		echo $bond_modle > /sys/class/net/$bond_name/bonding/mode 
		ifconfig $bond_name up
}

kerner_bond="bond_kernel"
dvs_brname="sdvs_00"
kenel_name="dvs_kernel"
default_route_nic=""
bond_slaves=""
global_bond_mode="4"
defaultgw_ip=""
eth_ip_copy=""
eth_mask_copy=""
eth_dhcp="0"


function config_kernal_port
{  
    local nic
    local ip_addr
    local ip_mask
    local slaves_nics
    local relpase_slaves
    local is_using_bonding=$1
    local bonding_mode=$2 
    local uplink_ports=$3
    local file="/opt/tecs/network/vna/etc/tecs.conf"

    #放在文件里面调用
    if [ -f /etc/modprobe.d/bonding.conf ];then
        do_log "/etc/modprobe.d/bonding.conf is exsit"
    else
        touch /etc/modprobe.d/bonding.conf
        echo "alias bond0 bonding" > /etc/modprobe.d/bonding.conf
    fi
	
    #默认网关
    get_default_gw
    default_route_nic=`route |grep  default |awk '{print $8}'`

    if [ "$is_using_bonding" != "" ];then
        get_install_config "$component" "is_using_bonding" "bool" "do you want bonding as kernel uplink port"  "$is_using_bonding"
    else
        get_install_config "$component" "is_using_bonding" "bool" "do you want bonding as kernel uplink port"  "no"
    fi	   
	
    #非config情况，is_using_bond为空，config_value的值还是从get_install_config里面获取
    if [ "$is_using_bonding" != "" ];then
        config_value=$is_using_bonding
    fi
	
    if [ "$config_value" = "yes" ]; then
        config_bond_upkernal_and_check

        if [ "$uplink_ports" != "" ];then
            slaves_nics=$uplink_ports
        else
            slaves_nics=`echo $bond_slaves |sed 's/ /#/g'`
        fi

        if [ "$bonding_mode" != "" ];then
            global_bond_mode=$bonding_mode
        fi

        add_bond_ovs  "sdvs_00" "bond_kernel"  $slaves_nics $global_bond_mode
        do_addif  "sdvs_00"  "dvs_kernel"
        sleep 2

        #创建网卡配置文件
        creat_nic_cfg_file "dvs_kernel"  $eth_dhcp
        
        echo dhcp $eth_dhcp
        echo ip $eth_ip_copy
        echo mask $eth_mask_copy

        #配置IP 立刻生效
        if [ "$eth_dhcp" = "0" ];then
            if  [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
                ifconfig "dvs_kernel" $eth_ip_copy netmask $eth_mask_copy
                echo "IPADDR=$eth_ip_copy" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
                echo "NETMASK=$eth_mask_copy" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
            fi
        else
            #todo 考虑先释放，再通过bond口dhcp获取IP
            if  [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
                ifconfig "dvs_kernel" $eth_ip_copy netmask $eth_mask_copy              
            else
                dhclient "dvs_kernel" -r
                dhclient "dvs_kernel" &
                sleep 5
                get_ipaddr_adapter "dvs_kernel"
            fi
        fi
        
        update_config  $file "kernel_ip_uplink_port"         $eth_ip_copy
        update_config  $file "kernel_mask_uplink_port"       $eth_mask_copy
        update_config  $file "kernel_uplink_port_boot_type"  $eth_dhcp
        update_config  $file "kernel_bondmodle"              $global_bond_mode
        update_config  $file "kernel_uplink_port"            $kerner_bond
        update_config  $file "kernel_tap"                    $kenel_name
        
        #uplink口的vlan模式以及native值设置
        ovs-vsctl set port $kenel_name  tag=1 vlan_mode=native-untagged
        ovs-vsctl set port $kerner_bond tag=1 vlan_mode=native-untagged

        #静态方式才需要写
        #if [ "$eth_dhcp" = "0" ];then
        #    #配置文件中的IP修改.IP必须在dvs_kernel上面
        #    if [ "$eth_ip_copy" != "" ];then
        #        echo "IPADDR=$eth_ip_copy" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
        #    fi
        #
        #    if [ "$eth_mask_copy" != "" ];then
        #        echo "NETMASK=$eth_mask_copy" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
        #    fi	
        #fi

        relpase_slaves=${slaves_nics//#/" "}
        #将所有动态IP的成员口的网卡启动方式改成静态，静态IP配置文件中要注释掉
        for i in $relpase_slaves
        do
            get_ipaddr_adapter $i
            if [ "$eth_dhcp" = "1" ];then
                sed -i '/^BOOTPROTO/s/DHCP/STATIC/'  /etc/sysconfig/network-scripts/ifcfg-$i
                sed -i '/^BOOTPROTO/s/dhcp/static/'  /etc/sysconfig/network-scripts/ifcfg-$i
                echo "PRE_BOOT=DHCP" >>/etc/sysconfig/network-scripts/ifcfg-$i
            else
                sed -i '/IPADDR/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$i
                sed -i '/NETMASK/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$i
                sed -i '/GATEWAY/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$i     
            fi
        done

        return
    fi
	
    config_upkernal_and_check
    if [ "$uplink_ports" != "" ];then
        nic=$uplink_port
    else
        nic="$config_value"
    fi

    #ovs-vsctl add-br sdvs_00; ovs-vsctl add-port sdvs_00 eth6
    do_bridge $nic $dvs_brname
    sleep 2

    #ovs-vsctl  add-port  sdvs_00 dvs_kernel
    do_addif  $dvs_brname $kenel_name
    sleep 2
	
    get_ipaddr_adapter $nic

    #创建网卡配置文件
    creat_nic_cfg_file $dvs_brname	0
    creat_nic_cfg_file $kenel_name  $eth_dhcp

    if [ "$eth_dhcp" = "0" ];then
        if [ "$eth_ip_copy" != "" ];then
            sed -i '/IPADDR/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$nic
            sed -i '/NETMASK/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$nic
            sed -i '/GATEWAY/s/^/##a#/' /etc/sysconfig/network-scripts/ifcfg-$nic
            ip_addr="$eth_ip_copy"
            ip_mask="$eth_mask_copy"
            ifconfig $nic 0
        else
            if [ "$mode" != "all" ]; then
                get_install_config "$component" "ip_addr" "string" "please input the ip address of $nic"  ""
                ip_addr="$config_value"
		
                get_install_config "$component" "ip_mask" "string" "please input the mask address of $nic"  ""
                ip_mask="$config_value"
            fi
        fi
        
        if [ "$ip_addr" != "" ] && [ "$ip_mask" != "" ];then
            ifconfig $kenel_name $ip_addr netmask $ip_mask
            echo "IPADDR=$ip_addr" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
            echo "NETMASK=$ip_mask" >> /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
        fi
    else
        if [ "$eth_ip_copy" = "" ];then
            dhclient $kenel_name -r
            dhclient $kenel_name &
            sleep 2
        else
            ip_addr="$eth_ip_copy"
            ip_mask="$eth_mask_copy"
            ifconfig $nic 0
            ifconfig $kenel_name $ip_addr netmask $ip_mask
        fi
        sed -i '/^BOOTPROTO/s/DHCP/STATIC/'  /etc/sysconfig/network-scripts/ifcfg-$nic
        sed -i '/^BOOTPROTO/s/dhcp/static/'  /etc/sysconfig/network-scripts/ifcfg-$nic
        echo "PRE_BOOT=DHCP" >>/etc/sysconfig/network-scripts/ifcfg-$nic
    fi

    #uplink口的vlan模式以及native值设置
    ovs-vsctl set port $kenel_name  tag=1 vlan_mode=native-untagged
    ovs-vsctl set port $nic  tag=1 vlan_mode=native-untagged

    update_config  $file "kernel_ip_uplink_port"         $ip_addr
    update_config  $file "kernel_mask_uplink_port"       $ip_mask
    update_config  $file "kernel_uplink_port_boot_type"  $eth_dhcp
    update_config  $file "kernel_bondmodle_uplink_port"  "none"
    update_config  $file "kernel_uplink_port"            $nic
    update_config  $file "kernel_tap"                    $nic_name
}

#if [ "$1" != "" ] && [ "$2" != "" ] && [ "$3" != "" ];then
#    export INTERACTIVE=0
#fi
do_install $1 $2 $3 

set_default_gw
write_default_gw_file
_CREATE_KERNEL_PORT_FILE="create_kernel_port.sh"
fi
