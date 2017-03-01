#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�host_info.sh
# �ļ���ʶ��
# ����ժҪ����ȡ�����ϸ��ֻ�����Ϣ
# ��ǰ�汾��1.0
# ��    �ߣ�Ԭ����
# ������ڣ�2011��9��1��
#
# �޸ļ�¼1��
#     �޸����ڣ�2011/9/1
#     �� �� �ţ�V1.0
#     �� �� �ˣ�Ԭ����
#     �޸����ݣ�����
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# ��ȡCPU������Ϣ
if [ -f /proc/cpuinfo ]; then	
	let physical_id_num=`grep -i "physical id" /proc/cpuinfo | sort | uniq | wc -l`
	if [ $physical_id_num -eq 0 ]
	then
		let physical_id_num=1
	fi
	
	let cpu_cores=`grep -m 1 "core id " /proc/cpuinfo | sort | uniq | wc -l`
	if [ $cpu_cores -eq 0 ]
	then
		let cpu_cores=1
	fi
	
	let bogomips=`grep -m 1 "bogomips" /proc/cpuinfo | awk -F': ' '{print $2}' | awk -F'.' '{print $1}'`/100*100
	
        if [ -f /opt/tecs/hc/etc/cpu_bench_result ]
        then
            test_bench=`cat /opt/tecs/hc/etc/cpu_bench_result | grep  CPU | awk -F ' ' '{print $3}' |sed -n '1p'`
            if [ -z "$test_bench" ]; then
            let test_bench=0;
            fi
        else
            let test_bench=0;
        fi
        
	echo -e "\t<_cpu_info>\n\t\t<CpuInfo>\n\t\t\t<_info>"
#	echo -e "\t\t\t\t<processor_num>"`grep -c -i "^processor" /proc/cpuinfo`"</processor_num>"
	echo -e "\t\t\t\t<physical_id_num>$physical_id_num</physical_id_num>"
	echo -e "\t\t\t\t<cpu_cores>$cpu_cores</cpu_cores>"
	echo -e "\t\t\t\t<vendor_id>"`grep -m 1 "vendor_id" /proc/cpuinfo | awk -F': ' '{print $2}'`"</vendor_id>"
	echo -e "\t\t\t\t<cpu_family>"`grep -m 1 "cpu family" /proc/cpuinfo | awk -F': ' '{print $2}'`"</cpu_family>"
	echo -e "\t\t\t\t<model>"`grep -m 1 "model" /proc/cpuinfo | awk -F': ' '{print $2}'`"</model>"
	echo -e "\t\t\t\t<model_name>"`grep -m 1 "model name" /proc/cpuinfo | awk -F': ' '{print $2}'`"</model_name>"
	echo -e "\t\t\t\t<stepping>"`grep -m 1 "stepping" /proc/cpuinfo | awk -F': ' '{print $2}'`"</stepping>"
#	echo -e "\t\t\t\t<_cpu_MHz>"`grep -m 1 "cpu MHz" /proc/cpuinfo | awk -F': ' '{print $2}'`"</_cpu_MHz>"
	echo -e "\t\t\t\t<cache_size>"`grep -m 1 "cache size" /proc/cpuinfo | awk -F': ' '{print $2}' | awk -F' KB' '{print $1}'`"</cache_size>"
	echo -e "\t\t\t\t<fpu>"`grep -m 1 "fpu" /proc/cpuinfo | awk -F': ' '{print $2}'`"</fpu>"
	echo -e "\t\t\t\t<fpu_exception>"`grep -m 1 "fpu_exception" /proc/cpuinfo | awk -F': ' '{print $2}'`"</fpu_exception>"
	echo -e "\t\t\t\t<cpuid_level>"`grep -m 1 "cpuid level" /proc/cpuinfo | awk -F': ' '{print $2}'`"</cpuid_level>"
	echo -e "\t\t\t\t<wp>"`grep -m 1 "wp" /proc/cpuinfo | awk -F': ' '{print $2}'`"</wp>"
	echo -e "\t\t\t\t<flags>"`grep -m 1 "flags" /proc/cpuinfo | sed 's/up //' | awk -F': ' '{print $2}'`"</flags>"
#dom0�����һ������ˣ���flag�ֶ���������up�ֶΣ���ʾ���ˣ���Ϊ�˺������ݼ��ݣ���Ҫȥ��up�ֶ�
	echo -e "\t\t\t\t<bogomips>$bogomips</_bogomips>"	
	echo -e "\t\t\t\t<cpu_bench>$test_bench</cpu_bench>"		
	echo -e "\t\t\t</_info>\n\t\t</CpuInfo>\n\t</_cpu_info>"
fi

#------------------------------------------------------------------------------
# ��ȡOS��Ϣ������������
echo -e "\t<_os_info>"`uname -srvmpio`"</_os_info>"

#------------------------------------------------------------------------------
# ��ȡVMM��Ϣ
if [ -f /sys/hypervisor/type ]; then
	echo -e "\t<_vmm_info>"`cat /sys/hypervisor/type` `cat /sys/hypervisor/version/major`.\
`cat /sys/hypervisor/version/minor``cat /sys/hypervisor/version/extra`"</_vmm_info>"
fi

#------------------------------------------------------------------------------
# ��ȡ�����ڴ���Ϣ
if [ -f /usr/sbin/xl ]; then
xm_text=`/usr/sbin/xl info`
if [ 0 -ne $? ]; then
	exit
fi
#let total_memory=`echo -e "$xm_text" | grep "total_memory" | awk -F': ' '{print $2}'`*1024*1024
#let dom0_mem=`free | grep -m 1 "Mem:" | awk -F' ' '{print $2}'`*1024
#let free_memory=total_memory-dom0_mem
#echo -e "\t<_mem_total>$total_memory</_mem_total>"

let total_memory=`echo -e "$xm_text" | grep "total_memory" | awk -F': ' '{print $2}'`
let dom0_mem=`xl list | grep 'Domain-0'  | awk -F' ' '{print $3}'`
let temp=total_memory-dom0_mem-17
#free_memory=`expr $temp \* 1024 \* 1024 - $temp \*1024 \* 12 - $temp\*1024 \* 8`
#free_memory�ļ��㹫ʽΪ:��xm.info.total-dom0-8-9��* 1024 *1004 =��xm.info.total-dom0-8-9��*1024*1024 - ��xm.info.total-dom0-8-9��*12*1024 - ��xm.info.total-dom0-8-9��*8*102
#-9 ÿ���������Ҫ9M�Ĺ����ڴ�,-8 Domain0 ÿG�������ĵ�ҳ���ڴ�,-12dom0 ����ʣ���ڴ�ÿG����Ҫҳ���ڴ�ռ�,-8 DomainU ʹ��ÿG�ڴ���Ҫ�Ķ���ҳ���ڴ� 
free_memory=`expr $temp \* 1024 \* 1004`
total_memory=`expr $total_memory \* 1024 \* 1024`
echo -e "\t<_mem_total>$total_memory</_mem_total>"
echo -e "\t<_mem_max>$free_memory</_mem_max>"
#is_support_hvm=`echo -e "$xm_text" | grep xen_caps |awk -F ' ' '{print $5}' | awk -F '-' '{print $1}'`
is_support_hvm=`echo -e "$xm_text" | grep xen_caps |grep hvm`
    if [ -z "$is_support_hvm" ]
    then
        is_support_hvm="0"
    else
        is_support_hvm="1"
    fi

echo -e "\t<_is_support_hvm>$is_support_hvm</_is_support_hvm>"
fi

# ��ȡSSH_key
if [ -f ~/.ssh/id_dsa.pub ]; then
    echo -e "\t<_ssh_key>"`cat ~/.ssh/id_dsa.pub`"</_ssh_key>"
fi
exit 0