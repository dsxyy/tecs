#/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 在测试目标机上本地运行，用于清理tecs在本机上一次安装运行的痕迹
# 此脚本由ci server通过ssh的方式在目标机的远程执行
echo "*******************************************************************************"
echo "$0 will stop and clear network  on local host ..." 
echo "*******************************************************************************"

#vadd by vnet chenzhiwei begin
#清除网络的tap口 以及bond口
function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return
    #忽略井号开头的注释行以及空行之后再grep过滤"key="所在的行
    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep "^[[:space:]]*$key[[:space:]]*="`
    if [ -z "$line" ]; then
        config_answer=""
    else
        #将第一个=号替换为空格，再删除第一个单词得到value
        config_answer=`echo $line | sed 's/=/ /' | sed -e 's/^\w*\ *//'`
    fi
}

# 清除防火墙设置
iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -F
iptables -X 
# 保存后重启iptables服务
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

#分普通和bond口两种情况 恢复的方式不一样
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

#删除默认的dvs_kernel以及sdvs_00的配置文件
rm -rf /etc/sysconfig/network-scripts/ifcfg-dvs_kernel
rm -rf /etc/sysconfig/network-scripts/ifcfg-sdvs_00
#删除掉log文件
rm -rf /var/log/bondinginfo
rm -rf /var/log/dobridge_ovs

#删除bond,以及还原网卡IP MASK GATEWAY
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

#删除bridges
bridges=`ovs-vsctl show |grep Bridge |awk '{print $2}'`
for i in $bridges
do
   bridge=`echo $i | sed 's/\"//g'`
   ovs-vsctl del-br $bridge
   rm -rf /etc/sysconfig/network-scripts/ifcfg-$bridge
done

#删除kernel
rm -rf /etc/sysconfig/network-scripts/ifcfg-kernel*
kernerls=`ifconfig |grep kernel |awk '{print $1}'`
for i in $kernerls
do
 tunctl -d $i
done

#删除loop
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
	#还原 原来网卡的启动方式
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



