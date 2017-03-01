#!/bin/bash
echo  "Usage: $0 WORKSPACE BUILD_NUMBER"
[ -z $1 ] && echo "no workspace!" && exit -1
[ -z $2 ] && echo "no build number!" && exit -1
workspace=$1
build_number=$2 

# 设置环境变量
export PATH=$PATH:/DITA-OT/
# 清理接口文档的doc临时文件
make -C $workspace/tecs/make docclean

echo "=========================================="
echo  "begin to make xml doc,please wait ..."
echo "=========================================="
# 编译对应的接口文档
make -C $workspace/tecs/make doc VER_I=$build_number 
if [ $? -ne 0 ]; then
    exit -1
fi

pdf_cn=$workspace/tecs/target/doc/xmlrpc/tecs_cn.pdf
pdf_en=$workspace/tecs/target/doc/xmlrpc/tecs_en.pdf

if [ ! -e $pdf_cn ] || [ ! -e $pdf_en ]; then
    exit -1
fi


