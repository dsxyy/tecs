#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
#
# �޸ļ�¼1��
#     �޸����ڣ�2012/4/12
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ���ci_server���汾�����Ի������а�װ��������

# �ű�������������
source  ./common.sh

# Զ��ִ��start.sh����װ������tecs
#cmd="$agent_script_dir/test/install_start.sh $agent_build_dir </dev/null &>$agent_build_dir/start.txt"
cmd="$agent_script_dir/test/install_start.sh $agent_build_dir"
echo "=========================================="
echo "remote execute command on $target: $cmd"
echo "Now we will install and start tecs on $target, please wait ..."
echo "=========================================="

ssh -tt $target <<-STOP
    echo "remote command($cmd) start! "
    $cmd
    echo "remote command($cmd) finished! "
    exit
STOP
#ssh $target $cmd 
RET=$?

#scp $target:$agent_build_dir/start.txt $server_build_dir
#[ -e $server_build_dir/start.txt ] && cat $server_build_dir/start.txt
    
if [ 0 -ne $RET ]; then
    echo "tecs start failed on host $target, remote command $cmd returns $RET!"
    ssh $target $agent_script_dir/ci/agent_tools/gather.sh $agent_build_dir
    scp $target:$agent_build_dir/output.tar.gz $server_build_dir
    exit $RET
fi

