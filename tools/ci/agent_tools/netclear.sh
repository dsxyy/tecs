#/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# �ڲ���Ŀ����ϱ������У���������tecs�ڱ�����һ�ΰ�װ���еĺۼ�
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��
echo "*******************************************************************************"
echo "$0 will stop and clear network  on local host ..." 
echo "*******************************************************************************"

#vadd by vnet chenzhiwei begin
#��������tap�� �Լ�bond��
function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #���Ծ��ſ�ͷ��ע�����Լ�����֮����grep����"key="���ڵ���
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep "^[[:space:]]*$key[[:space:]]*="`
    if [ -z "$line" ]; then
        config_answer=""
    else
        #����һ��=���滻Ϊ�ո���ɾ����һ�����ʵõ�value
        config_answer=`echo $line | sed 's/=/ /' | sed -e 's/^\w*\ *//'`
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
service tecs-vnm stop

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

function  uninstall
{
    local pkgname=$1
    echo "uninstall package $pkgname ... "
    rpm -e $pkgname --nodeps
}

uninstall "tecs-vna"
uninstall "tecs-vnm"
uninstall "tecs-wdg"

#add by vnet chenzhiwei
get_config /opt/tecs/network/vna/etc/tecs.conf "kernel_uplink_port"
uplink_port=$config_answer
echo "uplink_port is $uplink_port"

if [ "$uplink_port" = "" ];then
	get_config /opt/tecs/network/vna/etc/tecs.conf.rpmsave "kernel_uplink_port"
	uplink_port=$config_answer
	echo "uplink_port is $uplink_port"
fi

rm -rf /opt/tecs/network

rm -rf /etc/tecs/vna.config
rm -rf /etc/tecs/vnm.config
rm -rf /etc/tecs/wdg.config

rm -rf /etc/vna_uuid_file
rm -rf /etc/wdg_uuid_file
rm -rf /var/log/bondinfo
rm -rf /var/log/dobridge_ovs
rm -rf /var/log/dobridge

#vadd by vnet chenzhiwei begin
ovsstatus=`service openvswitch status |grep -c running`
if [ "$ovsstatus" != "2" ];then
   exit -1
fi
ovs-vsctl del-br sdvs_00  > /dev/null 2>&1

#����ͨ��bond��������� �ָ��ķ�ʽ��һ��
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

#ɾ��Ĭ�ϵ�dvs_kernel�Լ�sdvs_00�������ļ�
rm -rf /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
rm -rf /etc/sysconfig/network-scripts/ifcfg-sdvs_00
#ɾ����log�ļ�
rm -rf /var/log/bondinginfo
rm -rf /var/log/dobridge_ovs

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

rmmod bonding

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

echo "tecs network is cleared successfully!"
exit 0



