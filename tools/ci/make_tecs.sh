#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26

# 脚本启动参数处理
source  ./common.sh

[ -z "$4" ] && echo "no ci make build_num!" && exit -1
build_num="$4"
[ -z "$5" ] && echo "no ci make build_name!" && exit -1
build_name="$5"
#在目标测试机上编译tecs
#cmd="$agent_build_dir/agent_tools/make.sh $agent_build_dir  </dev/null &>$agent_build_dir/make.txt"
cmd="$agent_build_dir/agent_tools/make.sh $agent_build_dir $build_id $build_num $build_name"
echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will compile tecs on remote host $target, please wait ..."
echo "=========================================="

ssh $target $cmd
RET=$?

#scp $target:$agent_build_dir/make.txt $server_build_dir
#cat $server_build_dir/make.txt

if [ 0 -ne $RET ]; then
    echo "make tecs failed on host $target, remote command $cmd returns $RET!"
    
    #编译失败时调用钩子脚本，发送通知邮件    
    if [ ! -z $TEST_FAIL_HOOK ]; then
        echo "hook command  = $TEST_FAIL_HOOK `basename $0`"
        [ -e $TEST_FAIL_HOOK ] && $TEST_FAIL_HOOK `basename $0`
    else
        echo "no test fail hook!"
    fi
    exit $RET
fi

#编译如果成功完成了，还要检查有没有告警需要处理
#首先从ci agent的编译路径中将编译过程中产生的错误输出文件拷贝过来
scp $target:$agent_build_dir/make.txt $server_build_dir

#过滤掉一些正常的打印
sed -i '/skipping incompatible/d' $server_build_dir/make.txt

words=`wc -w $server_build_dir/make.txt  | awk -F' ' {'print $1'}`
#如果已经没有打印，说明没有告警，直接删除该文件
if [ $words -eq 0  ]; then
    echo "no warnings in file $server_build_dir/make.txt!"
    rm -f $server_build_dir/make.txt
else
    if [ ! -z $MAKE_WARN_HOOK ]; then
        #否则将告警群发邮件
        echo "hook command  = $MAKE_WARN_HOOK `basename $0` $server_build_dir/make.txt"
        [ -e $MAKE_WARN_HOOK ] && $MAKE_WARN_HOOK $server_build_dir/make.txt
    else
        echo "no make warn hook!"
    fi
fi
