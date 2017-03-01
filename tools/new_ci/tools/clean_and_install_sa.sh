#/bin/sh
# ���ߣ� yw
# ��д���ڣ�2012.10.26
# �ڲ���Ŀ����ϱ������У������ڱ�����װ������tecs

homedir=$1
[ -z $homedir ] && echo "no tecs ci directory!" && exit -1
[ ! -z $homedir ] && tecsdir=$homedir/rpm
[ ! -e $tecsdir ] && echo "tecs ci directory $tecsdir does not exist!" && exit -1

source $homedir/tools/setup/common/tecs_common_func.sh



echo "*******************************************************************************"
echo "$0 will install and start tecs on local host ..." 
echo "*******************************************************************************"

# ֹͣ���з���
service guard stop
service tecs-sa stop
service tecs-sd stop
 
# ���TECS����־
rm -fR /var/log/tecs/

# ����TECS��װ����
$tecsdir/installtecs_el6_x86_64.bin clean

$tecsdir/installtecs_el6_x86_64.bin install sa $2

exit 0


