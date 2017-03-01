#!/bin/bash
echo  "Usage: $0 WORKSPACE BUILD_NUMBER"
[ -z $1 ] && echo "no workspace!" && exit -1
[ -z $2 ] && echo "no build number!" && exit -1
workspace=$1
build_number=$2 

# ���û�������
export PATH=$PATH:/DITA-OT/
# ����ӿ��ĵ���doc��ʱ�ļ�
make -C $workspace/tecs/make docclean

echo "=========================================="
echo  "begin to make xml doc,please wait ..."
echo "=========================================="
# �����Ӧ�Ľӿ��ĵ�
make -C $workspace/tecs/make doc VER_I=$build_number 

pdf_cn=$workspace/tecs/code/tc/request_manager/doc/build/output/tecs_cn.pdf
pdf_en=$workspace/tecs/code/tc/request_manager/doc/build/output/tecs_en.pdf

if [ ! -e $pdf_cn ] || [ ! -e $pdf_en ]; then
    #����ʧ��ʱ���ù��ӽű�������֪ͨ�ʼ�    
    if [ ! -z $TEST_FAIL_HOOK ]; then
        echo "hook command  = $TEST_FAIL_HOOK `basename $0`"
        [ -e $TEST_FAIL_HOOK ] && $TEST_FAIL_HOOK `basename $0`
    else
        echo "no fail hook!"
    fi
    exit -1
fi


