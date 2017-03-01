#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 集成测试的几个步骤脚本都要处理相同的输入参数，过程繁琐
# 因此都集中到此脚本中处理，其它脚本可用source命令引入

echo  "Usage: $0 WORKSPACE BUILD_ID TARGET"
[ -z $1 ] && echo "no workspace!" && exit -1
[ -z $2 ] && echo "no build_id!" && exit -1
[ -z $3 ] && echo "no target!" && exit -1

# workspace是jenkins提供的$WORKSPACE环境变量，表示本次ci任务的工作区路径
# 可以在apache-tomcat-7.0.12/bin/startup.sh中自己定义，例如:
# export JENKINS_HOME=/home/ci_server/tecs_data，然后某个job的实际代码路径
# 就是：/home/ci_server/tecs_data/jobs/your-job-name/workspace/
workspace=$1
echo "workspace = $workspace"
[ ! -e $workspace ] && echo "workspace $workspace does not exist!" && exit -1

# build_id是jenkins提供的$BUILD_ID环境变量，表示本次ci任务的唯一标识符
# 一般都是用任务启动时的系统时间来表示，例如"2005-08-22_23-59-59" (YYYY-MM-DD_hh-mm-ss)
build_id=$2
echo "build_id = $build_id"

# target是本次测试使用的目标物理机ip地址，各个步骤中凡是需要ssh远程执行命令都以它为参数
target=$3
echo "target = $target"

# 通过jenkins提供的$WORKSPACE和$BUILD_ID两个变量，生成本次测试的服务端路径和客户端路径：
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