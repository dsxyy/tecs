#!/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# ���ɲ��Եļ�������ű���Ҫ������ͬ��������������̷���
# ��˶����е��˽ű��д��������ű�����source��������

echo  "Usage: $0 WORKSPACE BUILD_ID TARGET"
[ -z $1 ] && echo "no workspace!" && exit -1
[ -z $2 ] && echo "no build_id!" && exit -1
[ -z $3 ] && echo "no target!" && exit -1

# workspace��jenkins�ṩ��$WORKSPACE������������ʾ����ci����Ĺ�����·��
# ������apache-tomcat-7.0.12/bin/startup.sh���Լ����壬����:
# export JENKINS_HOME=/home/ci_server/tecs_data��Ȼ��ĳ��job��ʵ�ʴ���·��
# ���ǣ�/home/ci_server/tecs_data/jobs/your-job-name/workspace/
workspace=$1
echo "workspace = $workspace"
[ ! -e $workspace ] && echo "workspace $workspace does not exist!" && exit -1

# build_id��jenkins�ṩ��$BUILD_ID������������ʾ����ci�����Ψһ��ʶ��
# һ�㶼������������ʱ��ϵͳʱ������ʾ������"2005-08-22_23-59-59" (YYYY-MM-DD_hh-mm-ss)
build_id=$2
echo "build_id = $build_id"

# target�Ǳ��β���ʹ�õ�Ŀ�������ip��ַ�����������з�����ҪsshԶ��ִ���������Ϊ����
target=$3
echo "target = $target"

# ͨ��jenkins�ṩ��$WORKSPACE��$BUILD_ID�������������ɱ��β��Եķ����·���Ϳͻ���·����
[ ! -e $workspace/build ] && mkdir -p $workspace/build
server_build_dir=$workspace/build/$build_id
[ ! -e $server_build_dir ] && mkdir -p $server_build_dir
echo "server build directory = $server_build_dir"

agent_build_dir=/home/ci_agent/$build_id
echo "agent build directory = $agent_build_dir"

job_bulid_dir=/home/ci_server/Tecs_stinit01
echo "job build directory = $job_bulid_dir"

agent_script_dir=/home/ci_agent/script_home
echo "agent script directory = $agent_script_dir"