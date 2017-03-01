#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 修改记录1：
#     修改日期：2012/4/12
#     版 本 号：V2.0
#     修 改 人：彭伟
#     修改内容：修改支持系统集成测试。


# 脚本启动参数处理
source  ./common.sh

#cmd="$agent_script_dir/test/uninstall_clear.sh &>$agent_build_dir/clear.txt"
cmd="$agent_script_dir/test/uninstall_clear.sh"
echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will stop and uninstall tecs on remote host $target, please wait ..."
echo "=========================================="

ssh -tt $target  <<-STOP
    echo "remote command($cmd) start! "
    $cmd
    echo "remote command($cmd) finished! "
    exit
STOP
RET=$?

#scp $target:$agent_build_dir/clear.txt $server_build_dir
#cat $server_build_dir/clear.txt

if [ 0 -ne $RET ]; then
    echo "clear tecs failed on host $target, remote command $cmd returns $RET!"
    exit $RET
fi

