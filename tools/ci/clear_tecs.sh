#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh

#cmd="$agent_build_dir/agent_tools/clear.sh &>$agent_build_dir/clear.txt"
cmd="$agent_build_dir/agent_tools/clear.sh"
echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will stop and uninstall tecs on remote host $target, please wait ..."
echo "=========================================="

ssh $target  <<-STOP
    $cmd
STOP
RET=$?

#scp $target:$agent_build_dir/clear.txt $server_build_dir
#cat $server_build_dir/clear.txt

if [ 0 -ne $RET ]; then
    echo "clear tecs failed on host $target, remote command $cmd returns $RET!"
    exit $RET
fi

