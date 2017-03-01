#!/bin/bash

# 获取文件路径
coredump_path=$1

if [[ $coredump_path == "" ]];then
    echo "scans path does not input, exit"
    exit
fi

if [ ! -d $coredump_path ];then
    echo "scans path does not exist, exit"
    exit
fi

# 获取当前文件所对应的虚拟机名称
vmnames=`ls $coredump_path | awk -F '-' {'print $4'} | awk -F '.' {'print $1'}`

# 检测是否同一个虚拟机的coredump文件名即存在core,又存在core.tar.gz,如果存在，则重新进行压缩
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


# 针对同一个虚拟机，只保留最新的coredump文件,如果存在老的core.tar也会把老的tar压缩文件给删除掉，保留新的一个文件
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

# 压缩所有已经准备好的的coredump文件
# 存在文件名称为.core.ok的，但是没有出现。core.ok.tar.gz的则进行压缩
files=`ls $coredump_path | grep -v tar | grep core.ok`
for file_name in $files
do
    # 虚拟机名称修改为OK了，表示coredump文件已经写好了
    cd $coredump_path
    tar -czf $file_name.tar.gz ./$file_name
    rm -f ./$file_name
done

