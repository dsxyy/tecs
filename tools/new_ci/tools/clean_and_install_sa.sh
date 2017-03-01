#/bin/sh
# 作者： yw
# 编写日期：2012.10.26
# 在测试目标机上本地运行，用于在本机安装、启动tecs

homedir=$1
[ -z $homedir ] && echo "no tecs ci directory!" && exit -1
[ ! -z $homedir ] && tecsdir=$homedir/rpm
[ ! -e $tecsdir ] && echo "tecs ci directory $tecsdir does not exist!" && exit -1

source $homedir/tools/setup/common/tecs_common_func.sh



echo "*******************************************************************************"
echo "$0 will install and start tecs on local host ..." 
echo "*******************************************************************************"

# 停止所有服务
service guard stop
service tecs-sa stop
service tecs-sd stop
 
# 清空TECS的日志
rm -fR /var/log/tecs/

# 清理TECS安装环境
$tecsdir/installtecs_el6_x86_64.bin clean

$tecsdir/installtecs_el6_x86_64.bin install sa $2

exit 0


