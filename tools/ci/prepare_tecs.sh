#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh

# 将需要测试的最新tecs源代码压缩打包到服务端build路径
make -C $workspace/tecs/make cleanall
echo "compress $workspace/tecs to $server_build_dir/tecs.tar.gz ..."
cd $workspace; tar -czf $server_build_dir/tecs.tar.gz ./tecs --exclude=*.svn; cd -
echo "source code is compressed to $server_build_dir/tecs.tar.gz"

##################################################################################
# 更新远程目标测试机的系统时间，否则时间误差太大的话，编译会有告警
# 这一步的前提是ci server上已经启动了ntp服务，这个需要人工启动一下
# ip route get可以查一下到达target所用的本地ip
echo "synchronize date time of $target with ci server ..."
server_ip=`ip route get $target | grep dev | awk -F' ' '{print $5}'`
ntpcmd="ntpdate $server_ip"
echo "execute remote command $ntpcmd ..."
ssh $target $ntpcmd
#date1=`date +%G%m%d`
#date2=`date +%T`
#ssh $target date -s $date1
#ssh $target date -s $date2

##################################################################################
# 在目标测试机上创建本次build临时目录
ssh $target "[ ! -e $agent_build_dir ] && mkdir -p $agent_build_dir"
[ 0 -ne $? ] && echo "failed to create dir $target:$agent_build_dir!" && exit -1

# 将源代码拷贝到目标测试机上的build目录
echo "scp $server_build_dir/tecs.tar.gz to $target:$agent_build_dir/"
scp $server_build_dir/tecs.tar.gz $target:$agent_build_dir/
[ 0 -ne $? ] && echo "failed to scp $server_build_dir/tecs.tar.gz to $target:$agent_build_dir/!" && exit -1
# 拷贝成功之后就删除服务器上的压缩文件，等待客户端编译通过之后再拷贝回来
rm -rf $server_build_dir/tecs.tar.gz
echo "scp tecs.tar.gz finished!" 

# 将需要在测试机本地运行的工具脚本拷贝到目标测试机上build目录下的agent_tools目录中
[ ! -e `dirname $0`/agent_tools ] && echo "agent tools directory does not exist!" && exit -1
ssh $target "[ ! -e $agent_build_dir/agent_tools ] && mkdir -p $agent_build_dir/agent_tools"
scp `dirname $0`/agent_tools/* $target:$agent_build_dir/agent_tools/
