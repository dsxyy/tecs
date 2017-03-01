#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh

# 测试用例命令是第四个参数
[ -z "$4" ] && echo "no test case!" && exit -1
testcmd="$4"
temp=`echo $testcmd | awk -F' ' '{ print $1}' `
temp=`basename "$temp"`
testname=`echo "$temp" | awk -F'.' '{ print $1}'`
echo "test result will be save to $server_build_dir/${testname}.output.tar.gz!"

#通过xmlrpc客户端的远程调用测试目标机
echo "=================================="
echo "xmlrpc test remote host $target ..."
echo "test case command = $testcmd"
echo "test case name = $testname"
echo "current work path = $agent_script_dir"
echo "=================================="
ssh -tt $target <<-STOP
    cd $agent_script_dir
    $testcmd
    exit
STOP
RET=$?
echo "remote excute result is: $RET!"
# 测试出错时保存现场信息以便调试定位
if [ 0 -ne $RET ]; then
    echo "tecs test failed on host $target! exit code = $RET."
    cmd="$agent_script_dir/ci/agent_tools/gather.sh $agent_build_dir"
    ssh $target $cmd
    [ 0 -eq $? ] && scp $target:$agent_build_dir/output.tar.gz $server_build_dir/${testname}.output.tar.gz
fi
exit $RET

