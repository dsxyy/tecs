#!/bin/bash

function  clean_xen_vm
{
    echo " destroy xen vm ..."
    for vm in `xm list | awk -F' ' '{print $2}' | grep -v ID`
    do
        if [ 0 -eq $vm ]; then
            continue
        fi
        echo "xm destroy $vm ..."
        xm destroy $vm
        [ 0 -ne $? ] && exit -1
    done
}

function  clean_kvm_vm
{
    echo " destroy kvm vm ..."
    for vm in `virsh list | awk -F' ' '{print $1}' | sed '1,2d'`
    do
        if [ 0 -eq $vm ]; then
            continue
        fi
        echo "virsh destroy $vm ..."
        virsh destroy $vm
        [ 0 -ne $? ] && exit -1
    done
}

if [ -e /proc/xen ];then
    clean_xen_vm
fi 

if [ -e /dev/kvm ];then
    clean_kvm_vm
fi 
