#/bin/sh
# ���ߣ� ��Т��
# ��д���ڣ�2012.10.26
# �ڲ���Ŀ����ϱ������У������ڱ�����װ������tecs
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��

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
#����һ����ʡ��ÿ�ζ����㣬̫�˷�ʱ��
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


