#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh

[ -z "$4" ] && echo "no ci make build_num!" && exit -1
build_num="$4"
[ -z "$5" ] && echo "no ci make build_name!" && exit -1
build_name="$5"
#��Ŀ����Ի��ϱ���tecs
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
    
    #����ʧ��ʱ���ù��ӽű�������֪ͨ�ʼ�    
    if [ ! -z $TEST_FAIL_HOOK ]; then
        echo "hook command  = $TEST_FAIL_HOOK `basename $0`"
        [ -e $TEST_FAIL_HOOK ] && $TEST_FAIL_HOOK `basename $0`
    else
        echo "no test fail hook!"
    fi
    exit $RET
fi

#��������ɹ�����ˣ���Ҫ�����û�и澯��Ҫ����
#���ȴ�ci agent�ı���·���н���������в����Ĵ�������ļ���������
scp $target:$agent_build_dir/make.txt $server_build_dir

#���˵�һЩ�����Ĵ�ӡ
sed -i '/skipping incompatible/d' $server_build_dir/make.txt

words=`wc -w $server_build_dir/make.txt  | awk -F' ' {'print $1'}`
#����Ѿ�û�д�ӡ��˵��û�и澯��ֱ��ɾ�����ļ�
if [ $words -eq 0  ]; then
    echo "no warnings in file $server_build_dir/make.txt!"
    rm -f $server_build_dir/make.txt
else
    if [ ! -z $MAKE_WARN_HOOK ]; then
        #���򽫸澯Ⱥ���ʼ�
        echo "hook command  = $MAKE_WARN_HOOK `basename $0` $server_build_dir/make.txt"
        [ -e $MAKE_WARN_HOOK ] && $MAKE_WARN_HOOK $server_build_dir/make.txt
    else
        echo "no make warn hook!"
    fi
fi
