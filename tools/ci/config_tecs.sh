#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh
key=""
value=""

[ -z "$4" ] && echo "no config mode" && exit -1
mode="$4"

key="$5"
value="$6"


# Զ��ִ��install.sh����װ������tecs
cmd="$agent_build_dir/agent_tools/config.sh $mode $key $value"

echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will install tecs on $target, please wait ..."
echo "=========================================="

ssh $target  <<-STOP
    $cmd
STOP


