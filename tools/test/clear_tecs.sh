#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# �޸ļ�¼1��
#     �޸����ڣ�2012/4/12
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ��޸�֧��ϵͳ���ɲ��ԡ�


# �ű�������������
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

