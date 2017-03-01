#!/bin/sh
# ����ű������ڲ��Ի��ϱ���ִ�У�����tecs
if [ ! -z $1 ]; then
    tecsdir=$1
else
    echo "no tecsdir!"
    exit -1
fi

if [ ! -z $2 ]; then
    build_id=$2
else
    build_id="no ci bulid id"
fi

if [ ! -z $3 ]; then
    build_num=$3
else
    build_num="no ci bulid num"
fi

if [ ! -z $4 ]; then
    build_name=$4
else
    build_name="no ci bulid name"
fi

[ ! -e $tecsdir ] && echo "tecsdir $tecsdir does not exist!" && exit -1

cd $tecsdir
if [ ! -e ./tecs.tar.gz ]; then
    echo "$tecsdir/tecs.tar.gz does not exist!"
    exit -1
fi

echo "*******************************************************************************"
echo "make allrpm in test host directory: $tecsdir/tecs/make"
echo "*******************************************************************************"
#��tecsԴ�����ѹ��ʼ����
tar -xzf ./tecs.tar.gz

#cliĿ¼�µ�api������Ҫ��ɾ��������Ϊ��������ڱ�ĵط��������������ǲ��е�
[ -h tecs/client/cli/api ] && unlink tecs/client/cli/api

# ��������ϰ�װ��wine�����Գ�����������dat�ű�
#which wine  &>/dev/null
#if [ $? -eq 0 ]; then
#cd tecs/client/cli/scripts; chmod +x ./makecliscript.sh; ./makecliscript.sh; cd -
#fi

#����������в����Ĵ���͸澯����һ��������ļ�make.txt��
touch make.txt
make -C tecs/make all 3>&1 1>&2 2>&3 | tee make.txt
# �����Ƕ����ָ��������׼����������÷�˵����
#The redirection operator n>&m makes file descriptor n to be a copy of file descriptor m. So, whe are:
#- Opening a new file descriptor, 3, that is a copy of file descriptor 1, the standard output;
#- Making file descriptor 1 a copy of file descriptor 2, the standard error output;
#- Making file descriptor 2 to be a copy of file descriptor 3 (the "backup" of the standard output)
# in a short: we swapped the standard output and the standard error output. 

make -C tecs/make allrpm _CI_VER_NO=$build_id _CI_VER_NUM=$build_num _CI_VER_NAME=$build_name VER_I=$build_num

