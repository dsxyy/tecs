#���libvirt��Ĭ���������ã���Ϊ����virbr0���ź�openvswitch�г�ͻ
ifconfig virbr0 down > /dev/null 2>&1
brctl delbr virbr0 > /dev/null 2>&1
rm -f /etc/libvirt/qemu/networks/autostart/default.xml > /dev/null 2>&1
    
chkconfig --del vbridge >/dev/null 2>&1
chkconfig --del vbridged >/dev/null 2>&1
chkconfig --del loopback >/dev/null 2>&1

#ɾ��bond,�Լ���ԭ����IP MASK GATEWAY
rm -rf /etc/sysconfig/network-scripts/ifcfg-bond*
nic_files=`ls /etc/sysconfig/network-scripts/ifcfg-*`
for i in $nic_files
do
    sed -i '/MASTER/d' $i
	sed -i '/SLAVE/d'  $i
done

#ɾ��loop��
rm -rf /etc/sysconfig/network-scripts/ifcfg-loop*
loops=`ifconfig |grep loop |awk '{print $1}'`
for i in $loops
do
 tunctl -d $i
done

rmmod bonding > /dev/null 2>&1
echo "will reboot system now ,please install tecs after reboot"
reboot
