#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh

# 远程执行install.sh，安装并启动tecs
cmd="$agent_build_dir/agent_tools/install.sh $agent_build_dir </dev/null &>$agent_build_dir/install.txt"

echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will install tecs on $target, please wait ..."
echo "=========================================="

ssh $target  <<-STOP
    $cmd
STOP

echo "remote command($cmd) finished!"
RET=$?

scp $target:$agent_build_dir/install.txt $server_build_dir
[ -e $server_build_dir/install.txt ] && cat $server_build_dir/install.txt
    
if [ 0 -ne $RET ]; then
    echo "tecs install failed on host $target, remote command $cmd returns $RET!"
    ssh $target $agent_build_dir/tecs/tools/ci/agent_tools/gather.sh $agent_build_dir
    scp $target:$agent_build_dir/output.tar.gz $server_build_dir
    
    #测试失败时调用钩子脚本，发送通知邮件    
    if [ ! -z $TEST_FAIL_HOOK ]; then
        echo "hook command  = $TEST_FAIL_HOOK `basename $0`"
        [ -e $TEST_FAIL_HOOK ] && $TEST_FAIL_HOOK `basename $0`
    else
        echo "no fail hook!"
    fi
fi
exit $RET
