#!/bin/sh
# 作者： 张文剑
# 编写日期：2012.09.10

COMPILE_DIR= 
IMAGE_FILE=

function usage
{
    echo "Usage: `basename $0` -d compile_dir -i image_file"
    exit 0
}

function read_bool
{
    local prompt=$1
    local default=$2
    
    echo -e "$prompt (y/n? default: $default): \c "
    read answer
    case $answer in
            "Y") answer="yes" ;;
            "y") answer="yes";;
            "n") answer="no";;
            "N") answer="no";;
            "") answer="$default";;
            *) echo "Please input y or n"; read_bool;;
    esac
    return 0
}

function read_string
{
    local prompt=$1
    local default=$2
    
    echo -e "$prompt (default: $default): \c "
    read answer
    [ -z $answer ] && answer="$default"
}

#命令行参数处理
while getopts "d:i:h" option
do
    case $option in
         d) COMPILE_DIR=$OPTARG ;;
         i) IMAGE_FILE=$OPTARG ;;
         h) usage ;;
         ?) echo "invalid argument!"; exit 1 ;;
    esac
done

[ -z "$IMAGE_FILE" ] && echo "no image $IMAGE_FILE!"&& exit -1 

if [ -z "$COMPILE_DIR" ]; then
    SELF_DIR=`dirname $0`
    COMPILE_DIR="$SELF_DIR/../.."
fi
[ ! -d $COMPILE_DIR ] && echo "no dir $COMPILE_DIR!" && exit -1
echo "COMPILE_DIR = $COMPILE_DIR"

RPM_DIR=$COMPILE_DIR/rpm
[ ! -d $RPM_DIR ] && echo "no rpm dir $RPM_DIR!" && exit -1
echo "RPM_DIR = $RPM_DIR"

PYTEST_DIR=$COMPILE_DIR/tools/ci/pytest
[ ! -d $PYTEST_DIR ] && echo "no pytest dir $PYTEST_DIR!" && exit -1
echo "PYTEST_DIR = $PYTEST_DIR"

AGENT_TOOLS_DIR=$COMPILE_DIR/tools/ci/agent_tools
[ ! -d $AGENT_TOOLS_DIR ] && echo "no agent tools dir $AGENT_TOOLS_DIR!" && exit -1
echo "AGENT_TOOLS_DIR = $AGENT_TOOLS_DIR"

echo "IMAGE_FILE = $IMAGE_FILE"

[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name tecs-tc-*.rpm" ] && echo "tecs-tc rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name tecs-cc-*.rpm" ] && echo "tecs-cc rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name tecs-hc-*.rpm" ]  && echo "tecs-hc rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name tecs-common-*.rpm" ] && echo "tecs-common rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name openais-*.rpm" ] && echo "openais rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name qpidc-broker-*.rpm" ] && echo "qpidc-broker rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name qpidc-client-*.rpm" ] && echo "qpidc-client rpm is not ready!" && exit -1
[ -z "find $RPM_DIR/RPMS/el6/x86_64/ -name postgresql-*.rpm" ] && echo "postgresql rpm is not ready!" && exit -1

CLEAR_SH=$AGENT_TOOLS_DIR/clear.sh
[ ! -e $CLEAR_SH ] && "echo no clear script $CLEAR_SH!" && exit -1
INSTALL_SH=$AGENT_TOOLS_DIR/install.sh
[ ! -e $INSTALL_SH ] && "echo no install script $INSTALL_SH!" && exit -1
START_SH=$AGENT_TOOLS_DIR/start.sh
[ ! -e $START_SH ] && "echo no start script $START_SH!" && exit -1

read_bool "clear tecs history?" "yes"
if [ $answer == "yes" ]; then
    $CLEAR_SH
fi

read_bool "intall tecs now?" "yes"
if [ $answer == "yes" ]; then
    $INSTALL_SH $COMPILE_DIR/../
fi

read_bool "start tecs now?" "yes"
if [ $answer == "yes" ]; then
    $START_SH $COMPILE_DIR/../
fi

while true
do
    service tecs-tc status
    service tecs-cc status
    service tecs-hc status
    read_bool "ready to test?" "yes"
    [ $answer == "yes" ] && break
    sleep 1
done

let index=0
for i in `ls $PYTEST_DIR/test_*.py`
do
    testcase["$index"]=`basename $i`
    echo $index ${testcase[$index]}
    let index+=1
done

read_string "Please select a test case to run:" "0"
TEST_CASE="$PYTEST_DIR/${testcase[$answer]}"
echo "test case = $TEST_CASE"
if [ ! -e $TEST_CASE ]; then
    echo "test case $TEST_CASE does not exist!"
    exit -1
fi
$TEST_CASE  -i $IMAGE_FILE

