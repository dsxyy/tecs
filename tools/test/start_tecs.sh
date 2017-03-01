#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
#
# 修改记录1：
#     修改日期：2012/4/12
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：从ci_server传版本到测试机，进行安装和启动。

# 脚本启动参数处理
source  ./common.sh

# 远程执行start.sh，安装并启动tecs
#cmd="$agent_script_dir/test/install_start.sh $agent_build_dir </dev/null &>$agent_build_dir/start.txt"
cmd="$agent_script_dir/test/install_start.sh $agent_build_dir"
echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will install and start tecs on $target, please wait ..."
echo "=========================================="

ssh -tt $target <<-STOP
    echo "remote command($cmd) start! "
    $cmd
    echo "remote command($cmd) finished! "
    exit
STOP
#ssh $target $cmd 
RET=$?

#scp $target:$agent_build_dir/start.txt $server_build_dir
#[ -e $server_build_dir/start.txt ] && cat $server_build_dir/start.txt
    
if [ 0 -ne $RET ]; then
    echo "tecs start failed on host $target, remote command $cmd returns $RET!"
    ssh $target $agent_script_dir/ci/agent_tools/gather.sh $agent_build_dir
    scp $target:$agent_build_dir/output.tar.gz $server_build_dir
    exit $RET
fi

