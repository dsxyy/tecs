#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh

# �������������ǵ��ĸ�����
[ -z "$4" ] && echo "no test case!" && exit -1
testcmd="$4"
temp=`echo $testcmd | awk -F' ' '{ print $1}' `
temp=`basename "$temp"`
testname=`echo "$temp" | awk -F'.' '{ print $1}'`
echo "test result will be save to $server_build_dir/${testname}.output.tar.gz!"

#ͨ��xmlrpc�ͻ��˵�Զ�̵��ò���Ŀ���
echo "=================================="
echo "xmlrpc test remote host $target ..."
echo "test case command = $testcmd"
echo "test case name = $testname"
echo "=================================="

$testcmd
RET=$?

# ���Գ���ʱ�����ֳ���Ϣ�Ա���Զ�λ
if [ 0 -ne $RET ]; then
    echo "tecs test failed on host $target! exit code = $RET."
    cmd="$agent_build_dir/agent_tools/gather.sh $agent_build_dir"
    ssh $target $cmd
    [ 0 -eq $? ] && scp $target:$agent_build_dir/output.tar.gz $server_build_dir/${testname}.output.tar.gz
    
#����ʧ��ʱ���ù��ӽű�������֪ͨ�ʼ�    
    if [ ! -z $TEST_FAIL_HOOK ]; then
        echo "hook command  = $TEST_FAIL_HOOK $testname"
        [ -e $TEST_FAIL_HOOK ] && $TEST_FAIL_HOOK $testname
    else
        echo "no fail hook!"
    fi
fi
exit $RET

