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
echo "current work path = $job_bulid_dir"
echo "=================================="
cd $job_bulid_dir
$testcmd
RET=$?

#ɨ����־�ļ��������ʧ�ܵĲ������������Զ�������־
if [`find /home/tecs/st_log/$target/ -name "$testname.log" |xargs grep "FAIL"`];then
  RET=-1
fi

# ���Գ���ʱ�����ֳ���Ϣ�Ա���Զ�λ
if [ 0 -ne $RET ]; then
    echo "tecs test failed on host $target! exit code = $RET."
    cmd="$agent_script_dir/ci/agent_tools/gather.sh $agent_build_dir"
    ssh $target $cmd
    [ 0 -eq $? ] && scp $target:$agent_build_dir/output.tar.gz $server_build_dir/${testname}.output.tar.gz
fi
exit $RET
