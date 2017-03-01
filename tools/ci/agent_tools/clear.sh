#/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# �ڲ���Ŀ����ϱ������У���������tecs�ڱ�����һ�ΰ�װ���еĺۼ�
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��
echo "*******************************************************************************"
echo "$0 will stop and clear tecs on local host ..." 
echo "*******************************************************************************"
tecs_tc_conf=/opt/tecs/tc/etc/tecs.conf
tecs_cc_conf=/opt/tecs/cc/etc/tecs.conf
tecs_vnm_conf=/opt/tecs/network/vnm/etc/tecs.conf
current_time=`date +%Y-%m-%d-%H-%M-%S`
#vadd by vnet chenzhiwei begin
#��������tap�� �Լ�bond��
function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #���Ծ��ſ�ͷ��ע�����Լ�����֮����grep����"key="���ڵ���
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep "$key[[:space:]]*="`
    if [ -z "$line" ]; then
        echo "no key named \"$key\""
        config_answer=""
    else
        config_answer=`echo $line | awk -F '=' '{print $2}'`
    fi
}
#��ȡ���ݿ���Ϣ
function get_db_info
{
    local tecs_component_conf=$1
    # local operate="$2"
    [ -e "$tecs_component_conf" ] || { echo "$tecs_component_conf has lost"; return 1; }

    # ��ȡ��װ�������ݿ��������
    # ���ȴ�db_name�л�ȡ���ݿ����֣����û�вſ���application
    db_name_exist=`cat $tecs_component_conf | grep ^[[:space:]]*db_name[[:space:]]*=`
    if [ "$db_name_exist" != "" ];then
        get_config $tecs_component_conf "db_name"
    else
        get_config $tecs_component_conf "application"
    fi
    [ "$config_answer" != "" ] && db_name=$config_answer || { echo -e "database name doesn't exist in $tecs_component_conf"; return 1; }

    get_config $tecs_component_conf "db_server_url"
    [ "$config_answer" != "" ] && db_server=$config_answer || { echo -e "\"db_server_url\" value doesn't exist in $tecs_component_conf"; return 1; }
    get_config $tecs_component_conf "db_server_port"
    [ "$config_answer" != "" ] && db_port=$config_answer || db_port=5432
    get_config $tecs_component_conf "db_user"
    [ "$config_answer" != "" ] && db_user=$config_answer || { echo -e "\"db_user\" value doesn't exist in $tecs_component_conf"; return 1; }
    get_config $tecs_component_conf "db_passwd"
    [ "$config_answer" != "" ] && db_passwd=$config_answer || { echo -e "\"db_passwd\" value doesn't exist in $tecs_component_conf"; return 1; }
    
    return 0
}
#���û���������Ӧ��ִ��set_db_env
function set_db_env
{
    export PGHOME=/usr/local/pgsql
    export PGDATA=/usr/local/pgsql/data
    export PATH=/usr/local/pgsql/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH
    export PGUSER=$db_user
    export PGPASSWORD=$db_passwd
}
#�ж����ݿ��Ƿ���ڣ�Ӧ��ִ��get_db_info��set_db_env
function is_db_exist
{
    local component=$1
    local tecs_component_conf=$2
    
    #���ݿ��ѯ
    db_exist=`psql -h$db_server -p $db_port -U$db_user -dtemplate1 -w -c"select datname from pg_database where datname="\'""$db_name""\'";" | grep "$db_name"`
    RET=$?    
    [ 0 -ne $RET ] && { echo "query database failed for $component!"; db_exist=""; return $RET; }
}
#��������пͻ������������ݿ⣬��ʾ�û��ȶϿ�������û��ɾ���������Ӧ��ִ��get_db_info��set_db_env
function is_db_free
{    
    local client_number=0
    local counter=0
    client_number=`psql -h$db_server -p $db_port -U$db_user -w -dtemplate1 -c "SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$db_name""\'";"| head -n 3 | tail -n 1`

    if [ -z $client_number ]; then
        client_number=0
    fi
    
    while [ $counter -lt 3 ] && [ $client_number -gt 0 ]
    do
        echo "database $db_name is being accessed by other users, can not edit!!!"
        echo "you should do the following steps:"
        echo "step 1: service guard stop"
        echo "step 2: service tc(or cc or vnm) stop"
        echo "step 3: exit psql CLI shell and pgAdmin GUI client"
        echo -e "After done, press any key to continue ...\c "
        read answer  
        client_number=`psql -h$db_server -p $db_port -U$db_user -w -dtemplate1 -c"SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$db_name""\'";"| head -n 3 | tail -n 1`
        if [ -z $client_number ]; then
            client_number=0
        fi          
        counter=`expr $counter + 1`
    done
}
#���������ݿ⣬Ӧ��ִ��get_db_info��set_db_env
function rename_db
{
    local component=$1
    local tecs_component_conf=$2
    
    is_db_free
    
    local bak_name="${db_name}_bak_${current_time}"
    #�������е��л����滻Ϊ�»���
    bak_name=`echo $bak_name|tr '-' '_'`  
    echo "will rename database \"$db_name\" to \"$bak_name\""        
    psql -h$db_server -p $db_port -U$db_user -dtemplate1 -w -c"alter database "$db_name" rename to "$bak_name""  
    local RETVAL=$?    
    [ 0 -ne $RETVAL ] && echo "failed to rename database $db_name of $component." 

}
#CLEAN���������ݿ⴦���ݶ�Ϊ���������ݿ�
function db_process
{  
    local component="$1"
    local tecs_component_conf="$2"

    #���������ļ���ȡ���ݿ���Ϣ
    get_db_info "$tecs_component_conf" || { echo "failed to process database of $component"; return 1; } 

    #���û�������
    set_db_env
    is_db_exist $component $tecs_component_conf
    [ "$db_exist" = "" ] && { echo "no database named \"$db_name\" exists."; return 0; } 
    
    if [ "$db_exist" != "" ];then
        rename_db $component $tecs_component_conf
    else
        return 0
    fi
}

# �������ǽ����
iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -F
iptables -X 
# ���������iptables����
service iptables save
service iptables restart

service guard stop
service tecs-tc stop
service tecs-cc stop 
service tecs-hc stop 
service tecs-sd stop 
service tecs-sa stop
service tecs-vna stop
service tecs-wdg stop
service tecs-vnm stop
service tgtd stop
service ntpd stop

# kill tecs process
for i in `ps -elf | grep tecs- | grep -v grep | awk -F ' ' '{ print $4}'`
do
        if [ "$i" == "$$" ]; then
            continue
        fi

        if [ -d /proc/$i ]; then
            echo will kill pid $i ...
            kill -9 $i
        fi
done

function  clean_xen_vm
{
    echo " destroy xen vm ..."
    for vm in `xm list | awk -F' ' '{print $2}' | grep -v ID`
    do
        if [ 0 -eq $vm ]; then
            continue
        fi
        echo "xm destroy $vm ..."
        xm destroy $vm
        [ 0 -ne $? ] && exit -1
    done
}

function  clean_kvm_vm
{
    echo " destroy kvm vm ..."
    for vm in `virsh list | awk -F' ' '{print $1}' | sed '1,2d'`
    do
        if [ 0 -eq $vm ]; then
            continue
        fi
        echo "virsh destroy $vm ..."
        virsh destroy $vm
        [ 0 -ne $? ] && exit -1
    done
}

[ -e /proc/xen ] && clean_xen_vm
[ -e /dev/kvm ] && clean_kvm_vm

# ����coredumpĿ¼
[ -e /proc/xen ] && [ -d /var/lib/tecs/coredump ] && rm -rf /var/lib/tecs/coredump/*
[ -e /proc/xen ] && [ -d /var/lib/xen/dump ] && rm -rf /var/lib/xen/dump/*

echo "wait a few seconds for block devices to be free ..."
sleep 5

# ���������mount�豸
for i in `cat /proc/mounts | grep tecs_vg | awk -F' ' '{ print $2}'`
do
    echo "umount $i ..."
    umount $i
done

# ����coredump ����mount���豸
for i in `cat /proc/mounts | grep xen/dump | awk -F' ' '{ print $2}'`
do
    echo "umount $i ..."
    umount $i
done

# ɾ�����豸����
for i in ` ls /dev/tecs_storage/ -l | awk -F' ' '{print $9}'`
do
    echo "rm  -rf /dev/tecs_storage/$i ..."
    rm -rf /dev/tecs_storage/$i 
done

# �ǳ����д洢�豸
iscsiadm -m node -U all
iscsiadm -m node -o delete

# ɾ�������lv
for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep tmp_`
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi 
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep base_`
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi  
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep free_`
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi  
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep i_ `
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi  
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep lv_ `
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi  
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep r_ `
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	echo "lvm $i remove fail ..."
        exit 1		  
    fi 
done

for i in `lvs --noheadings -o lv_path | grep tecs_vg | grep b_ `
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
          echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
       	  echo "lvm $i remove fail ..."
    	  exit  1    
    fi
done

for i in `lvs --noheadings -o lv_path | grep tecs_volume | grep lvm_`
do
    lvs --noheadings -o lv_path $i
    if [ $? -ne 0 ];then
	  echo "$i not exit...continue"
    	  continue    	     
    fi
    echo "lvremove -f $i ..."
    kpartx  -d  $i
    lvremove -f $i
    if [ $? -ne 0 ];then
    	  echo "lvm $i still in use,   pease clean hc first before clean sa..."
    	  exit  1    
    fi
    
done

function  uninstall
{
    local pkgname="$1"
    rpm -qi $pkgname >/dev/null
       
    if [ "$?" -eq 0 ];then
        echo "uninstall package $pkgname ... "
        rpm -e $pkgname --nodeps
    fi
}

[ -e "$tecs_tc_conf" ] && db_process "tecs-tc"  "$tecs_tc_conf"
[ -e "$tecs_tc_conf" ] && get_config $tecs_tc_conf "application" && sed -i "/$config_answer/d" /etc/hosts
uninstall "tecs-tc"
[ -e "$tecs_cc_conf" ] && db_process "tecs-cc"  "$tecs_cc_conf"
uninstall "tecs-cc"
uninstall "tecs-hc"
uninstall "tecs-vna"
[ -e "$tecs_vnm_conf" ] && db_process "tecs-vnm"  "$tecs_vnm_conf"
uninstall "tecs-vnm"
uninstall "tecs-wdg"
uninstall "tecs-sd"
uninstall "tecs-sa"
uninstall "scsi-target-utils"
uninstall "tecs-common"
uninstall "tecs-guard"
uninstall "postgresql-client"

#add by vnet chenzhiwei
get_config /opt/tecs/network/vna/etc/tecs.conf "kernel_uplink_port"
uplink_port=$config_answer
echo "uplink_port is $uplink_port"

if [ "$uplink_port" = "" ];then
	get_config /opt/tecs/network/vna/etc/tecs.conf.rpmsave "kernel_uplink_port"
	uplink_port=$config_answer
	echo "uplink_port is $uplink_port"
fi

rm -rf /opt/tecs

uninstall "tecs-wui"

service qpid stop
uninstall "qpidc-client"
uninstall "qpidc-broker"

service postgresql stop
uninstall "postgresql"
uninstall "openais"
uninstall "scsi-target-utils"

# ����ϵ�ӳ���ϵ
sed -i "/tecscloud/d" /etc/hosts

rm -rf /dev/shm/tecs
rm -rf /mnt/VolGroup00
rm -rf /mnt/tecs_tc
rm -rf /mnt/tecs_hc

rm -rf /etc/tecs
rm -rf /var/log/tecs
rm -rf /var/lib/tecs
rm -rf /dev/shm/tecsprint
rm -rf /tmp/tecs/light
service nfs stop

[ -e /proc/xen ] && service xend restart

#[ 0 -ne $? ] && exit -1
service libvirtd restart
#[ 0 -ne $? ] && exit -1


#ʹ��check_ifsriov����뽫is_sriov��0
function check_ifsriov
{
    local adapter=$1
    local sriov_specia_num
	
	is_sriov=0
	#echo  "check nic  $adapter is sriov or not"
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
            return 0
        fi    		
    fi

}

#��Ҫ��SRIOV ����Ϊfalse
function clear_ovs_sriovnic_cfg()
{
    #��ȡ����OVS BR
    local all_brs=`ovs-vsctl list-br |awk '{print $1}' `
	
    echo "clear_ovs_sriovnic_cfg ovs"
    for i in $all_brs
    do
	    local all_br_ports=`ovs-vsctl list-ports $i `
		for j in $all_br_ports
		do 
			check_ifsriov $j
	        if [ "$is_sriov" = "1" ];then
	            ovs-vsctl set interface $j other-config:enable-vlan-splinters=false
		        result=$?
				echo "ovs-vsctl set interface $j other-config:enable-vlan-splinters=false result:$result"
	        fi
		done         		
    done
	
	#��Ҫ����SRIOV����BOND���
	echo "clear_ovs_sriovnic_cfg bond"
	local all_bonds=`ovs-appctl bond/list |awk '{print $1}'|sed 1d`
	for k in $all_bonds
	do
	    local all_bond_ports=`ovs-appctl bond/show $k | grep slave | awk '{print $2}' |grep : |awk -F : '{print $1}'`
		for h in $all_bond_ports
		do 
		    check_ifsriov $h
	        if [ "$is_sriov" = "1" ];then
	            ovs-vsctl set interface $h other-config:enable-vlan-splinters=false
		        result=$?
				echo "ovs-vsctl set interface $j other-config:enable-vlan-splinters=false result:$result"
	        fi
		done 
	done 
}

clear_ovs_sriovnic_cfg

ovs-vsctl del-br sdvs_00  > /dev/null 2>&1

#����ͨ��bond��������� �ָ��ķ�ʽ��һ��
if [ -e /etc/sysconfig/network-scripts/ifcfg-$uplink_port ]; then
cat /etc/sysconfig/network-scripts/ifcfg-$uplink_port |grep "##a#" > /dev/null 2>&1
if [ $? -eq 0 ];then
    sed -i '/IPADDR/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
    sed -i '/NETMASK/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
    sed -i '/GATEWAY/s/##a#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
else 
    sed -i '/IPADDR/s/#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
    sed -i '/NETMASK/s/#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
    sed -i '/GATEWAY/s/#//' /etc/sysconfig/network-scripts/ifcfg-$uplink_port
fi
fi

#ɾ��Ĭ�ϵ�dvs_kernel�Լ�sdvs_00�������ļ�
rm -rf /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
rm -rf /etc/sysconfig/network-scripts/ifcfg-sdvs_00

#ɾ��bond,�Լ���ԭ����IP MASK GATEWAY
rm -rf /etc/sysconfig/network-scripts/ifcfg-bond*
nic_files=`ls /etc/sysconfig/network-scripts/ifcfg-*`
for i in $nic_files
do
    sed -i '/MASTER/d' $i
	sed -i '/SLAVE/d'  $i
	sed -i '/IPADDR/s/##a#//' $i
	sed -i '/IPADDR/s/##b#//' $i
	sed -i '/NETMASK/s/##a#//' $i
	sed -i '/NETMASK/s/##b#//' $i
	sed -i '/GATEWAY/s/##a#//' $i
	sed -i '/GATEWAY/s/##b#//' $i
done

#ɾ��bridges
bridges=`ovs-vsctl show |grep Bridge |awk '{print $2}'`
for i in $bridges
do
   bridge=`echo $i | sed 's/\"//g'`
   ovs-vsctl del-br $bridge
   rm -rf /etc/sysconfig/network-scripts/ifcfg-$bridge
done

#ɾ��kernel
rm -rf /etc/sysconfig/network-scripts/ifcfg-kernel*
kernerls=`ifconfig |grep kernel |awk '{print $1}'`
for i in $kernerls
do
 tunctl -d $i
done

#ɾ��loop
rm -rf /etc/sysconfig/network-scripts/ifcfg-loop*
loops=`ifconfig |grep loop |awk '{print $1}'`
for i in $loops
do
 tunctl -d $i
done
rm -rf /etc/vna_uuid_file
rm -rf /etc/wdg_uuid_file
rmmod bonding >/dev/null 2>&1

all_phy_nics=`ls -al /sys/class/net/ | grep pci |awk '{print $9}'`
for i in $all_phy_nics
do
	#��ԭ ԭ��������������ʽ
	cat /etc/sysconfig/network-scripts/ifcfg-$i |grep PRE_BOOT > /dev/null 2>&1
	if [ $? -eq 0 ];then
	    sed -i '/BOOTPROTO/d' /etc/sysconfig/network-scripts/ifcfg-$i
		echo "BOOTPROTO=dhcp">>/etc/sysconfig/network-scripts/ifcfg-$i
	fi
	sed -i '/PRE_BOOT/d' /etc/sysconfig/network-scripts/ifcfg-$i
done
	
service network restart
service network restart

#vadd by vnet chenzhiwei end

# ����exports
sed -i "/^[[:space:]]*\/var\/lib\/tecs/d" /etc/exports 
# 
# xm destroy
# umount lv
# lvremove
# ���� ntp ������
ntp_config="/etc/ntp.conf"
mv $ntp_config.bak $ntp_config
sed -i "/add by tecs/d" /var/spool/cron/root
# ���� dns ������
service dnsmasq stop
sed -i "/#added by tecs/d" /etc/hosts
sed -i "/#added by tecs/d" /etc/dnsmasq.conf
sed -i "/#added by tecs/d" /etc/resolv.conf

echo "tecs is cleared successfully!"
exit 0



