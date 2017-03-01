#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26

# �ű�������������
source  ./common.sh

# ����Ҫ���Ե�����tecsԴ����ѹ������������build·��
make -C $workspace/tecs/make cleanall
echo "compress $workspace/tecs to $server_build_dir/tecs.tar.gz ..."
cd $workspace; tar -czf $server_build_dir/tecs.tar.gz ./tecs --exclude=*.svn; cd -
echo "source code is compressed to $server_build_dir/tecs.tar.gz"

##################################################################################
# ����Զ��Ŀ����Ի���ϵͳʱ�䣬����ʱ�����̫��Ļ���������и澯
# ��һ����ǰ����ci server���Ѿ�������ntp���������Ҫ�˹�����һ��
# ip route get���Բ�һ�µ���target���õı���ip
echo "synchronize date time of $target with ci server ..."
server_ip=`ip route get $target | grep dev | awk -F' ' '{print $5}'`
ntpcmd="ntpdate $server_ip"
echo "execute remote command $ntpcmd ..."
ssh $target $ntpcmd
#date1=`date +%G%m%d`
#date2=`date +%T`
#ssh $target date -s $date1
#ssh $target date -s $date2

##################################################################################
# ��Ŀ����Ի��ϴ�������build��ʱĿ¼
ssh $target "[ ! -e $agent_build_dir ] && mkdir -p $agent_build_dir"
[ 0 -ne $? ] && echo "failed to create dir $target:$agent_build_dir!" && exit -1

# ��Դ���뿽����Ŀ����Ի��ϵ�buildĿ¼
echo "scp $server_build_dir/tecs.tar.gz to $target:$agent_build_dir/"
scp $server_build_dir/tecs.tar.gz $target:$agent_build_dir/
[ 0 -ne $? ] && echo "failed to scp $server_build_dir/tecs.tar.gz to $target:$agent_build_dir/!" && exit -1
# �����ɹ�֮���ɾ���������ϵ�ѹ���ļ����ȴ��ͻ��˱���ͨ��֮���ٿ�������
rm -rf $server_build_dir/tecs.tar.gz
echo "scp tecs.tar.gz finished!" 

# ����Ҫ�ڲ��Ի��������еĹ��߽ű�������Ŀ����Ի���buildĿ¼�µ�agent_toolsĿ¼��
[ ! -e `dirname $0`/agent_tools ] && echo "agent tools directory does not exist!" && exit -1
ssh $target "[ ! -e $agent_build_dir/agent_tools ] && mkdir -p $agent_build_dir/agent_tools"
scp `dirname $0`/agent_tools/* $target:$agent_build_dir/agent_tools/
