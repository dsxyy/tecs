#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh
key=""
value=""

[ -z "$4" ] && echo "no config mode" && exit -1
mode="$4"

key="$5"
value="$6"


# 远程执行install.sh，安装并启动tecs
cmd="$agent_build_dir/agent_tools/config.sh $mode $key $value"

echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will install tecs on $target, please wait ..."
echo "=========================================="

ssh $target  <<-STOP
    $cmd
STOP


