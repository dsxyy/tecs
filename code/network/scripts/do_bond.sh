
logfile="/var/log/bondinfo"
function do_logfile
{
   if [ -f $logfile ];then
	   touch $logfile
	   echo "##this is vbridge logfile create by vnet do_bond.sh" >>$logfile
   else
	   echo "##this is vbridge logfile create by vnet do_bond.sh" >>$logfile
   fi
}

#记录日志
function do_log
{
  local logstr=$1
  
  if [ ! -f /var/log/bondinfo ];then
      touch $logfile
  fi
  
  local size=`ls -l /var/log/bondinfo |awk '{print $5}'`
  LANG=en_US.ISO8859-1
  

  
  if [[ $size -gt 30000 ]];then
      rm -rf $logfile
	  touch $logfile
	  echo "##this is vbridge logfile create by vnet version 2.0" >>$logfile
  fi
  
  echo -n `date '+%b %d %T'` >> $logfile
  echo " $logstr" >>$logfile
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

#0 PC, 1 ATCA , 2 ETCA
machine_type=0
function get_machine_type
{
    local notpc=0
	local cores
	
	#固化成功的话就不是pc
    service nicfix status
	if [ $? -eq 0 ];then
	   notpc=1
	fi
	
	if [ "$notpc" = 0 ];then
	machine_type=0
	fi
	
	if [ "$notpc" = 1 ];then
	    ls -al /sys/class/net/ | grep pci |awk '{print $9}'|grep eth4
		if [ $? -eq 0 ]; then
		    machine_type=2
		else
       		machine_type=1
		fi
	fi
}

function get_default_gw
{
    defaultgw_ip=`ip route list|grep default |awk  '{print $3}'`
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
	local route_nic=`ip route list |grep  default |awk '{print $5}'`
	local has_gw
	
    if [ "$route_nic" != "" ];then
		get_ipaddr_adapter $route_nic
		if [ "$eth_dhcp" = "0" ];then
			if [ "$defaultgw_ip" != "" ];then
				has_gw=`cat /etc/sysconfig/network-scripts/ifcfg-$route_nic |grep -i GATEWAY`
				if [ "$has_gw" = "" ];then
					echo "GATEWAY=$defaultgw_ip" >> /etc/sysconfig/network-scripts/ifcfg-$route_nic
				    do_log "write GATEWAY=$defaultgw_ip to $route_nic"
				fi
			fi
		fi
    fi
}

function get_ipaddr_adapter
{
    local ipstring
    local maskstring
    local eth_name_ip=$1
	
	#初始化一下
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

#修改bond成员口的配置文件
function modify_eth_cfg_file
{         
    local adapter=$1
    local master=$2
    local file
    file="/etc/sysconfig/network-scripts/ifcfg-${adapter}"
    
    if [ -e $file ]; then
	    #已经修改过就return,在这个脚本里面有MASTER和SLAVE只能说明是选的口已经是某个bond口的成员口
	    cat $file |grep MASTER >/dev/null 2>&1
	    if [ $? -eq 0 ];then
	        do_log "err $adapter is already slave"
            exit -1
	    fi
		
	    cat $file |grep SLAVE >/dev/null 2>&1
	    if [ $? -eq 0 ];then
 	        return
	    fi
           echo "MASTER=${master}">>/etc/sysconfig/network-scripts/ifcfg-${adapter}
           echo "SLAVE=yes">>/etc/sysconfig/network-scripts/ifcfg-${adapter}
        else
           do_log "/etc/sysconfig/network-scripts/ifcfg-${adapter} not exsit, cant bond ${adapter} to ${master}"
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

#处理add_bond ip操作的函数
function add_bond_ipctrl
{
    local bond_name=$1
	local bond_modle=$2
	local slave_num=$3
	local slaves=$4
	local relpase_slaves=${slaves//#/" "}
	local route_nic
	
	do_log "add_bond_ipctrl bond_name:$1 ,bond_modle:$2, slaves:$3"
	
	get_default_gw
	add_bond $bond_name $bond_modle $slaves
	
	#创建bond网卡的配置文件
	creat_nic_cfg_file $bond_name $eth_dhcp $bond_modle
	
	#配置bond地址使得立刻成效
	if [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
	    ifconfig $bond_name $eth_ip_copy netmask $eth_mask_copy
	fi
	
	#如果是静态IP方式，则写入IP
	if [ "$eth_dhcp" = "0" ];then
		if [ "$eth_ip_copy" != "" ];then
			echo "IPADDR=$eth_ip_copy" >> /etc/sysconfig/network-scripts/ifcfg-$bond_name
	    fi
	
		if [ "$eth_mask_copy" != "" ];then
			echo "NETMASK=$eth_mask_copy" >> /etc/sysconfig/network-scripts/ifcfg-$bond_name
		fi	
	fi
	
	#设置默认网关,并写入配置文件
	set_default_gw
	write_default_gw_file
}

#该函数用来创建bond,不做IP处理，ip处理由外层来控制
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
		   #eth_dhcp=0
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

function add_bond_ovs
{
    local bridge_name=$1
	local bond_name=$2
	local mode=$3
	local slaves=$4
	local relpase_slaves=${slaves//#/" "}  
	
	do_log "add_bond_ovs bridge_name:$bridge_name ,bond_name:$bond_name,mode:$mode, slaves:$relpase_slaves"
	
	ovs-vsctl  -- --if-exists del-port $bond_name
	ovs-vsctl add-bond $bridge_name $bond_name $relpase_slaves
	local ret=$?
	do_log "ovs-vsctl add-bond $bridge_name $bond_name $relpase_slaves -- set port $bond_name bond_mode=active-backup ret $ret" 
		
	if [ "$mode" = "4" ];then
	    ovs-vsctl set port $bond_name lacp=active
	    ret=$?
	    do_log "ovs-vsctl set port $bond_name lacp=active ret:$ret"
		ovs-vsctl set port $bond_name bond_mode=balance-slb
	    ret=$?
	    do_log "ovs-vsctl set port $bond_name bond_mode=balance-slb ret:$ret"
	else
            ovs-vsctl set port $bond_name bond_mode=active-backup	
	fi
	#针对SR-IOV网口设置子接口
	for nic in $relpase_slaves
	do
	    check_ifsriov $nic
	    if [ "$is_sriov" = "1" ];then
	        ovs-vsctl set interface $nic other-config:enable-vlan-splinters=true
		result=$?
		do_log "ovs-vsctl set interface $nic other-config:enable-vlan-splinters=true result:$result"
	    fi       
	done   
}

function rename_dvs
{
   local old_br=$1
   local new_br=$2
   local bond_name=$3
   
   do_log "rename_dvs_ipctrl old_br:$1, new_br:$2, bond_name:$3"
   
   ovs-vsctl set interface $old_br name=$new_br
   do_log "ovs-vsctl set interface $old_br name=$new_br"
   
   ovs-vsctl set port $old_br name=$new_br
   do_log "ovs-vsctl set port $old_br name=$new_br"

   ovs-vsctl set bridge $old_br name=$new_br
   do_log "ovs-vsctl set bridge $old_br name=$new_br"
   
   ifconfig $new_br up
   
   return 0
}

function del_bond_ipctrl
{
    local bond_name=$1
    local slaves=$2
	local relpase_slaves=${slaves//#/" "}   
	
	do_log "del_bond_ipctl $1 $2 $3"
	get_default_gw
	
	del_bond $bond_name $slaves $relpase_slaves
	
	#ip地址还原立刻生效
	for nic in $relpase_slaves
	do
	    get_ipaddr_adapter $nic
		if [ "$eth_ip_copy" != "" ] && [ "$eth_mask_copy" != "" ];then
		    ifconfig $nic $eth_ip_copy netmask $eth_mask_copy
		fi	
		
		if [ "$eth_dhcp" = "1" ];then
		    dhclient $nic -r
			dhclient $nic
		fi
	done	
	
	#设置默认网关与写入配置文件
	set_default_gw
	write_default_gw_file
}

#不做IP处理，ip处理由外层来控制
function del_bond
{
	local bond_name=$1
    local slaves=$2
	local relpase_slaves=${slaves//#/" "}
	local nic
	local bond_sriov=0

    do_log "will del_bond name=$bond_name slaves:$relpase_slaves"
	
	echo -$bond_name > /sys/class/net/bonding_masters

	rm -rf /etc/sysconfig/network-scripts/ifcfg-$bond_name
	for nic in $relpase_slaves
	do
	   sed -i '/MASTER/d' /etc/sysconfig/network-scripts/ifcfg-$nic
	   sed -i '/SLAVE/d'  /etc/sysconfig/network-scripts/ifcfg-$nic
	   ifconfig $nic up
	done
	return 0 
}

#不做IP处理，ip处理由外层来控制
function set_bond
{
    local bond_name=$1
	local para=$2
	local value=$3
	
	do_log "will modify para:$para, value:$value"
	echo "this operation is not allowed!"
	return 0 
}

function add_bond_slave_ipctrl
{
    local bond_name=$1
	local slave=$2
	local bond_ip
	local bond_mask
	local slave_ip
	local slave_mask		
    
	do_log "add_bond_slave_ipctrl bond_name:$1, slave:$2"
	get_default_gw
	add_bond_slave $bond_name $slave

	#slave加入bond口，判断bond口是否有地址，没有地址则从slave移到bond口
	get_ipaddr_adapter $bond_name
	bond_ip=$eth_ip_copy
	bond_mask=$eth_mask_copy
	
	get_ipaddr_adapter $slave
	slave_ip=$eth_ip_copy
	slave_mask=$eth_mask_copy
	
	if [ "$bond_ip" = "" ]&& [ "$slave_ip" != "" ];then
	    #slave地址移到bond
	    ifconfig $slave 0
		ifconfig $bond_name $slave_ip netmask $slave_mask
	fi
	
    set_default_gw
	write_default_gw_file
	
	
}
function add_bond_slave
{
    local bond_name=$1
	local slave=$2
	
	do_log "add bond $bond_name slave $slave"	
    modify_eth_cfg_file $slave  $bond_name 
	
	ifconfig $bond_name down
	ifconfig $slave down
	echo +$slave > /sys/class/net/$bond_name/bonding/slaves
	
	if [ $? -ne 0 ];then
	    do_log "echo +$slave > /sys/class/net/$bond_name/bonding/slaves failed"  
	    ifconfig $slave up
	    ifconfig $bond_name up
        return 1	   
	else
	    do_log "echo +$slave > /sys/class/net/$bond_name/bonding/slaves success"  	
	    ifconfig $slave up
	    ifconfig $bond_name up
        return 0 
	fi
}

function del_bond_slave_ipctrl
{
    local bond_name=$1
	local slave=$2
	local bond_ip
	local bond_mask
	local slave_ip
	local slave_mask
	local bridge_name
	local is_edvs="0"

	do_log "del_bond_slave_ipctrl $bond_name $slave"
	
	get_default_gw
	del_bond_slave $bond_name $slave
	
	bridge_name=`ovs-vsctl port-to-br $bond_name`
	do_log "del_bond_slave_ipctrl bridge name is $bridge_name"
	if [ -z $bridge_name  ];then
	    is_edvs="1"
		do_log "del_bond_slave_ipctrl $bond_name is edvs"
	fi	
	
	#如果bond口地址和要删除的成员口地址相同，则把bond口地址移回到成员口。
	#否则，bond口的地址不变，成员口地址还原成之前的地址。 
	#如果bond口只有一个成员口了，则把bond删掉，唯一的成员口地址还原为原来的。
	if [ ! -f /etc/sysconfig/network-scripts/ifcfg-$bond_name ];then
	   echo "$bond_name's cfg file is not exsit ,will exit!"
	   do_log "$bond_name's cfg file is not exsit ,will exit!"
	   exit 
	fi
	
	#取网桥的地址，如果是EDVS的情况，非EDVS取BOND口的地址
	if [ "$is_edvs" = "0" ];then
		get_ipaddr_adapter $bridge_name
		bond_ip=$eth_ip_copy
		bond_mask=$eth_mask_copy
	else
        get_ipaddr_adapter $bond_name
		bond_ip=$eth_ip_copy
		bond_mask=$eth_mask_copy	
	fi
	
	get_ipaddr_adapter $slave
	slave_ip=$eth_ip_copy
	slave_mask=$eth_mask_copy
	
	do_log "bond_ip $bond_ip bond_mask $bond_mask,slave_ip $slave_ip,slave_mask $slave_mask"
	
	if [ "$bond_ip" = "$slave_ip" ] && [ "$bond_mask" = "$slave_mask" ] ;then
	    #bond口IP和slave口的IP一样，把bond口的IP去掉，注意这里本来bond的IP就是被注释掉的，这里再彻底删除
        sed -i '/IPADDR/d' /etc/sysconfig/network-scripts/ifcfg-$bond_name
		sed -i '/NETMASK/d' /etc/sysconfig/network-scripts/ifcfg-$bond_name
		sed -i '/GATEWAY/d' /etc/sysconfig/network-scripts/ifcfg-$bond_name
		#网桥的IP要去掉
		ifconfig $bridge_name 0
	fi
	
	#还原slave的IP
	if [ "$slave_ip" != "" ] && [ "$slave_mask" != "" ];then
 	    ifconfig $slave  $slave_ip netmask $slave_mask
	fi
	
	#slave的IP方式的DHCP需要获取一下IP
	if [ "$eth_dhcp" = "1" ];then
	    dhclient $slave -r
		dhclient $slave
		get_ipaddr_adapter $slave
		do_log "restor ,$slave ip:$eth_ip_copy ,mask:$eth_mask_copy,$bond_name ip :$bond_ip ,$bond_name mask:$bond_mask"
		if [ "$bond_ip" = "$eth_ip_copy" ] && [ "$bond_mask" = "$eth_mask_copy" ];then
		    ifconfig $bond_name 0
		fi
	fi
	set_default_gw
	write_default_gw_file
}

function del_bond_slave
{
    local bond_name=$1
	local slave=$2
	
	do_log "del bond $bond_name slave $slave"
	
	sed -i '/MASTER/d' /etc/sysconfig/network-scripts/ifcfg-$slave
	sed -i '/SLAVE/d'  /etc/sysconfig/network-scripts/ifcfg-$slave 		
		
	ifconfig $bond_name down
    ifconfig $slave down	
	echo -$slave > /sys/class/net/$bond_name/bonding/slaves
	
	if [ $? -ne 0 ];then
	    do_log "echo -$slave > /sys/class/net/$bond_name/bonding/slaves failed"  
	    ifconfig $slave up
	    ifconfig $bond_name up
        return 1	   
	else
	    do_log "echo -$slave > /sys/class/net/$bond_name/bonding/slaves success"  	
	    ifconfig $slave up
	    ifconfig $bond_name up
        return 0 
	fi	
}

function backup_info
{
    local bond_name=$1
	local active_str
	local active_nic
	
	#bond是否存在
	ovs-appctl bond/show $bond_name >/dev/null 2>&1
	if [ $? -ne 0 ];then
	    echo "error"
	    return 1
	fi
	
	#bond的active是不存在
	active_str=`ovs-appctl bond/show $bond_name |grep -B1 'active slave'`
	if [ "$active_str" = "" ];then
            echo "no active"
	    ovs-appctl bond/show $bond_name |grep 'slave ' |awk -F ":" '{print $1}' |awk '{print $2}'
		return 2
	fi
	
	#bond的active是存在，先输出active,再输出others
	active_nic=`echo $active_str |grep 'active ' |awk -F ":" '{print $1}' |awk  '{print $2}'`
	echo $active_nic
	ovs-appctl bond/show $bond_name |grep 'slave ' |awk -F ":" '{print $1}' |awk '{print $2}' |grep -v $active_nic
	return 0
}


function lacp_info
{
    local bond_name=$1
	local aggid
	local parmac
	local sucnic
	local failnics
	
	do_log "in lacp_info bond_name $1"
	
	ovs-appctl bond/list |grep $bond_name >/dev/null 2>&1
	if [ $? -ne 0 ];then
	   do_log "$bond_name is not in bond"
	   exit 1
	fi
	
	ovs-appctl lacp/show $bond_name> /dev/null 2>&1
	if [ $? -ne 0 ];then
	   do_log "$bond_name is not lacp"
	   echo "error"
	   exit 1
	fi
	
	ovs-appctl lacp/show $bond_name |grep -A 1 $bond_name |grep active |grep negotiated> /dev/null 2>&1
	if [ $? -ne 0 ];then
	   do_log "$bond_name is not negotiaed ok"
	   echo "not negotiaed"
	   exit 2
	fi
	
	aggid=`ovs-appctl lacp/show $bond_name |grep 'aggregation key' |awk -F ":" '{print $2}'|tr -d ' '`
	if [ "$aggid" != "" ];then
	    echo $aggid
	fi
	
	parmac=`ovs-appctl lacp/show $bond_name |grep 'actor sys_id' |head -n 1 |awk  '{print $3}'|tr -d ' '`
	if [ "$parmac" != "" ];then
	    echo $parmac
	fi
	
	sucnic=`ovs-appctl lacp/show $bond_name  |grep 'current attached' |awk -F ":" '{print $2}' |tr -d ' '`
	if [ "$sucnic" != "" ];then
	    for i  in $sucnic
		do
	        echo "suc_nic$i"
		done
	fi
	
	failnics=`ovs-appctl lacp/show $bond_name  |grep -E "defaulted |detached"|grep slave|awk -F ":" '{print $2}' |tr -d ' '`
	if [ "$failnics" != "" ];then
	    for i  in $failnics
		do
	        echo "fail_nic$i"
		done
	fi
	
	return 0
}


case "$1" in
    add_bond)
        add_bond_ipctrl $2 $3 $4 $5
        ;;

	add_bond_ovs)
	    add_bond_ovs $2 $3 $4 $5
	;;	
	
	backup_info)
	    backup_info $2
	;;
	
	lacp_info)
	    lacp_info $2
	;;	
	
    del_bond)
        del_bond_ipctrl $2 $3
        ;;

    set_bond)
        set_bond_ipctrl $2 $3 $4
        ;;
		
	add_bond_slave)
        add_bond_slave_ipctrl $2 $3
        ;;
		
	del_bond_slave)
        del_bond_slave_ipctrl $2 $3
        ;;

	rename_dvs)
	    rename_dvs $2 $3 $4
		;;
    *)
        echo "Unknown command: $1" >&2
        exit 1
esac

