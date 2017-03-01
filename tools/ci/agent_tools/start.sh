#/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 在测试目标机上本地运行，用于在本机安装、启动tecs
# 此脚本由ci server通过ssh的方式在目标机的远程执行

[ -z $1 ] && echo "no tecs ci directory!" && exit -1
[ ! -z $1 ] && tecsdir=$1

#重新启动xend
#service xend restart
#[ 0 -ne $? ] && exit -1

# tecs web服务器目前在APISVR进程中启动，使用xmlrpc自带的abyss，这个web root目录不能少
# 以后如果采用apache作为web服务器，这个步骤就可以忽略了
[ ! -e /usr/local/abyss/wui ] && mkdir -p /usr/local/abyss/wui

#更新tecs配置文件，将所有的控制台打印输出到指定的文件中，便于调试定位问题
tecs_output_dir=/dev/shm/tecsprint
[ ! -e $tecs_output_dir ] && mkdir -p $tecs_output_dir
tcexes=`awk -F' ' '{ print $1 }' /etc/tecs/tc.config`
for i in $tcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/tc.config
done

ccexes=`awk -F' ' '{ print $1 }' /etc/tecs/cc.config`
for i in $ccexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/cc.config
done

hcexes=`awk -F' ' '{ print $1 }' /etc/tecs/hc.config`
for i in $hcexes
do
    exe=`basename $i`
    sed -i "/$exe/s@-d@& --daemon_output_file $tecs_output_dir/$exe.txt@g" /etc/tecs/hc.config
done

service tecs-tc start
[ 0 -ne $? ] && exit -1
service tecs-cc start
[ 0 -ne $? ] && exit -1
service tecs-hc start
[ 0 -ne $? ] && exit -1

#启动guard
service guard start
[ 0 -ne $? ] && exit -1

#等待一段时间，tecs进程都上电完成了再返回?
echo "tecs is started successfully!"
exit 0


