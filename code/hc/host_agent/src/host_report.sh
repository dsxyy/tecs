#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�host_report.sh
# �ļ���ʶ��
# ����ժҪ����ȡ�����ϸ��ֶ�̬�ļ����Ϣ
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



# ��ȡ�����״̬, ����û�, ����һ�����˾����������������������쳣���¶��Ǻõ�
# ��ǰִ��Ŀ¼�µ�osc_test ��������ڲ���
getpath()
{
local c=${1%/*}

if [ "$c" = "$1" ] ; then  
c="."
fi
echo "$c"
}
# ���ж��Ƿ��в����ļ�,�������ʹ�ò����ļ�
oscfile=$(getpath $0 )/osc_test
if [ ! -e $oscfile ]; then
	oscfile=/proc/zte/crystal
fi

if [ ! -e $oscfile ]; then
echo "\t<_oscillator> 0 </_oscillator>"
else
# ����һ������,�Ͱ����ϴ���
awk  -F ':' ' BEGIN {a = 0} $3 ~ "ERROR" { a = 1 } END { print "\t<_oscillator> " a " </_oscillator>" }  ' $oscfile 2>/dev/null
fi
#---------------end of get crystal state---------------------------------------
    
#------------------------------------------------------------------------------
# ��ȡ�������Ϣ
xentop_text=`/usr/sbin/xentop -b -d1 -i2 2>/dev/null`		    		# ȡ����xentop��Ϣ������
if [ 0 -ne $? ]; then
	exit
fi
let dom_lines=`echo -e "$xentop_text" | wc -l`/2  				    	# ����xentop��ȡ��Ϣ��������
for ((a=1; a <= dom_lines ; a++))								    	# ɾ����һ�ε�xentop��Ϣ
do
	xentop_text=`echo -e "$xentop_text" | sed '1d'`
done

for ((; dom_lines > 0 ; dom_lines--))
do
	find_title=`echo -e "$xentop_text" | grep -m 1 "" | grep "      NAME  STATE   CPU(sec)"`
	if [ "$find_title" ]
    then
		xentop_text=`echo -e "$xentop_text" | sed '1d'`
		let dom_lines-=1
		break
	else
		xentop_text=`echo -e "$xentop_text" | sed '1d'`		
	fi
done

xentop_text=`echo -e "$xentop_text" | sed -e 's/no limit/no_limit/'`	# �����ո��no limit�滻Ϊ�����ո��no_limit
total_cpu_usage=0														# ͳ������Domain��cpu������֮��

echo -e "\t<_vm_info>"
for ((a=1; a <= dom_lines ; a++))										# ���л�ȡ��Domain����Ϣ
do
	echo -e "\t\t<item>"
	echo -e "\t\t\t<VmInfo>"
	echo -e "\t\t\t\t<_vm_name>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $1}'`"</_vm_name>"
	echo -e "\t\t\t\t<_vm_vcpu_num>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $9}'`"</_vm_vcpu_num>"
	echo -e "\t\t\t\t<_vm_net_tx>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $11}'`"</_vm_net_tx>"
	echo -e "\t\t\t\t<_vm_net_rx>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $12}'`"</_vm_net_rx>"
	# �����������CPU�����ʺ��ڴ�ʹ�����
	let cpu_usage=`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $4}' | awk -F'.' '{print $1}'`*10+`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $4}' | awk -F'.' '{print $2}'`
	let total_cpu_usage+=cpu_usage
	let mem_max=`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $5}'`*1024
#	let mem_free=mem_max-mem_max*`echo "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $6}' | awk -F'.' '{print $1 $2}'`/1000
	echo -e "\t\t\t\t<_vm_cpu_usage>$cpu_usage</_vm_cpu_usage>"
	echo -e "\t\t\t\t<_vm_mem_max>$mem_max</_vm_mem_max>"
#	echo -e "\t\t\t\t<_vm_mem_free>$mem_free</_vm_mem_free>"
	echo -e "\t\t\t</VmInfo>"
	echo -e "\t\t</item>"
	xentop_text=`echo -e "$xentop_text" | sed '1d'`						# ɾ���ѽ�������
done
echo -e "\t</_vm_info>"

#------------------------------------------------------------------------------
# ��ӡ����cpu������
if [ -f /proc/cpuinfo ]; then
	let total_cpu_usage/=`grep -c -i "processor" /proc/cpuinfo`
	if [ "$total_cpu_usage" -gt 1000 ]; then
		let total_cpu_usage=1000
	fi
	echo -e "\t<_cpu_usage>$total_cpu_usage</_cpu_usage>"
fi

exit 0
