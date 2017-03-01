#!/bin/bash

# ��ȡ�ļ�·��
coredump_path=$1

if [[ $coredump_path == "" ]];then
    echo "scans path does not input, exit"
    exit
fi

if [ ! -d $coredump_path ];then
    echo "scans path does not exist, exit"
    exit
fi

# ��ȡ��ǰ�ļ�����Ӧ�����������
vmnames=`ls $coredump_path | awk -F '-' {'print $4'} | awk -F '.' {'print $1'}`

# ����Ƿ�ͬһ���������coredump�ļ���������core,�ִ���core.tar.gz,������ڣ������½���ѹ��
for vmname in $vmnames
do
    del_name=`ls $coredump_path -rt | grep "${vmname}\." | grep core.ok`
    for del_vm in $del_name
    do
        has_tar=`ls $coredump_path | grep $del_vm -c`
        if [[ $has_tar == 2 ]] ;then
            cd $coredump_path
            tar -czf $del_vm.tar.gz ./$del_vm
            rm -f ./$del_vm
        fi
    done
done


# ���ͬһ���������ֻ�������µ�coredump�ļ�,��������ϵ�core.tarҲ����ϵ�tarѹ���ļ���ɾ�����������µ�һ���ļ�
for vmname in $vmnames
do
    del_name=`ls $coredump_path -rt | grep "${vmname}\." | grep ok`
    del_num=`ls $coredump_path -rt | grep "${vmname}\." | grep ok -c`
    for del_vm in $del_name
    do
        if [[ $del_num > 1 ]];then
            rm -f $coredump_path/$del_vm
            del_num=`ls $coredump_path -rt | grep "${vmname}\."| grep ok -c`
        fi
    done
done

# ѹ�������Ѿ�׼���õĵ�coredump�ļ�
# �����ļ�����Ϊ.core.ok�ģ�����û�г��֡�core.ok.tar.gz�������ѹ��
files=`ls $coredump_path | grep -v tar | grep core.ok`
for file_name in $files
do
    # ����������޸�ΪOK�ˣ���ʾcoredump�ļ��Ѿ�д����
    cd $coredump_path
    tar -czf $file_name.tar.gz ./$file_name
    rm -f ./$file_name
done

