#/bin/sh
# 作者： 李孝成
# 编写日期：2012.10.26
# 在测试目标机上本地运行，用于在本机安装、启动tecs
# 此脚本由ci server通过ssh的方式在目标机的远程执行

[ -z $1 ] && echo "no tecs ci directory!" && exit -1
[ ! -z $1 ] && tecsdir=$1
[ ! -e $tecsdir ] && echo "tecs ci directory $tecsdir does not exist!" && exit -1

echo "*******************************************************************************"
echo "$0 will install and start tecs on local host ..." 
echo "*******************************************************************************"

bindir=$tecsdir/tecs/rpm

[  -e /etc/cpu_bench_result ] && mkdir -p /opt/tecs/hc/etc/
[  -e /etc/cpu_bench_result ] && cp /etc/cpu_bench_result /opt/tecs/hc/etc/
$bindir/*.bin all stop
#备份一个，省得每次都计算，太浪费时间
cp /opt/tecs/hc/etc/cpu_bench_result /etc/cpu_bench_result

service openais start
[ 0 -ne $? ] && exit -1

service qpid start
[ 0 -ne $? ] && exit -1

service postgresql start
[ 0 -ne $? ] && exit -1
sleep 10s

echo "tecs is installed successfully!"
exit 0


