#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 在测试目标机上本地运行，用于清理tecs在本机上一次安装运行的痕迹
# 修改记录1：
#     修改日期：2012/4/12
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：原版本为ci\agent_tools\clear.sh，只修改了名称。
# 修改记录2：
#     修改日期：2012/8/3
#     版 本 号：V2.1
#     修 改 人：xujie
#     修改内容：去除包卸载依赖关系

# 此脚本由ci server通过ssh的方式在目标机的远程执行
echo "*******************************************************************************"
echo "$0 will stop and clear tecs on local host ..." 
echo "*******************************************************************************"

service guard stop
service tecs-tc stop
service tecs-cc stop 
service tecs-hc stop 

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

for vm in `xm list | awk -F' ' '{print $2}' | grep -v ID`
do
    if [ 0 -eq $vm ]; then
        continue
    fi
    echo "xm destroy $vm ..."
    xm destroy $vm
    [ 0 -ne $? ] && exit -1
done

echo "wait a few seconds for block devices to be free ..."
sleep 5

# 清理虚拟机mount设备
for i in `mount | grep tecs_vg | awk -F' ' '{ print $3}'`
do
    echo "umount $i ..."
    umount $i
done

# 删除虚拟机lv
for i in `lvdisplay | grep "LV Name" | grep tecs_vg | awk -F' ' '{print $3}'`
do
    echo "lvremove -f $i ..."
    lvremove -f $i
done

function  uninstall
{
    local pkgname=$1
    echo "uninstall package $pkgname ... "
    rpm -e $pkgname --nodeps
}
function uninstall_apache
{
    local apachepath=/usr/local/abyss/apache
    [ ! -e $apachepath ] && return 0

    /usr/local/abyss/apache/bin/httpd -d /usr/local/abyss/apache -k stop

    echo "remove apache from $apachepath..."
    rm -rf $apachepath
}
uninstall "tecs-tc"
uninstall "tecs-cc"
uninstall "tecs-hc"
uninstall "tecs-common"
uninstall "tecs-guard"
rm -rf /opt/tecs

uninstall "tecs-wui"
#uninstall_apache
service qpid stop
uninstall "qpidc-client"
uninstall "qpidc-broker"

service postgresql stop
uninstall "postgresql"
uninstall "openais"

rm -rf /dev/shm/tecs
rm -rf /mnt/VolGroup00
rm -rf /mnt/tecs_tc
rm -rf /mnt/tecs_hc

rm -rf /etc/tecs
rm -rf /var/log/tecs
rm -rf /var/lib/tecs
rm -rf /dev/shm/tecsprint
service nfs stop
service xend restart
[ 0 -ne $? ] && exit -1
service libvirtd restart
[ 0 -ne $? ] && exit -1
# 
# xm destroy
# umount lv
# lvremove
echo "tecs is cleared successfully!"
exit 0



