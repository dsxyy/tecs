#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：host_report.sh
# 文件标识：
# 内容摘要：获取主机上各种动态的监控信息
# 当前版本：1.0
# 作    者：袁进坤
# 完成日期：2011年9月1日
#
# 修改记录1：
#     修改日期：2011/9/1
#     版 本 号：V1.0
#     修 改 人：袁进坤
#     修改内容：创建
#------------------------------------------------------------------------------



# 获取晶振的状态, 输出好坏, 任意一个坏了就输出坏；其它情况（包括异常）下都是好的
# 当前执行目录下的osc_test 这个是用于测试
getpath()
{
local c=${1%/*}

if [ "$c" = "$1" ] ; then  
c="."
fi
echo "$c"
}
# 先判断是否有测试文件,如果有则使用测试文件
oscfile=$(getpath $0 )/osc_test
if [ ! -e $oscfile ]; then
	oscfile=/proc/zte/crystal
fi

if [ ! -e $oscfile ]; then
echo "\t<_oscillator> 0 </_oscillator>"
else
# 任意一个坏了,就按故障处理
awk  -F ':' ' BEGIN {a = 0} $3 ~ "ERROR" { a = 1 } END { print "\t<_oscillator> " a " </_oscillator>" }  ' $oscfile 2>/dev/null
fi
#---------------end of get crystal state---------------------------------------
    
#------------------------------------------------------------------------------
# 获取虚拟机信息
xentop_text=`/usr/sbin/xentop -b -d1 -i2 2>/dev/null`		    		# 取两次xentop信息并保存
if [ 0 -ne $? ]; then
	exit
fi
let dom_lines=`echo -e "$xentop_text" | wc -l`/2  				    	# 计算xentop获取信息的总行数
for ((a=1; a <= dom_lines ; a++))								    	# 删除第一次的xentop信息
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

xentop_text=`echo -e "$xentop_text" | sed -e 's/no limit/no_limit/'`	# 将含空格的no limit替换为不含空格的no_limit
total_cpu_usage=0														# 统计所有Domain的cpu利用率之和

echo -e "\t<_vm_info>"
for ((a=1; a <= dom_lines ; a++))										# 逐行获取各Domain的信息
do
	echo -e "\t\t<item>"
	echo -e "\t\t\t<VmInfo>"
	echo -e "\t\t\t\t<_vm_name>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $1}'`"</_vm_name>"
	echo -e "\t\t\t\t<_vm_vcpu_num>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $9}'`"</_vm_vcpu_num>"
	echo -e "\t\t\t\t<_vm_net_tx>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $11}'`"</_vm_net_tx>"
	echo -e "\t\t\t\t<_vm_net_rx>"`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $12}'`"</_vm_net_rx>"
	# 计算虚拟机的CPU利用率和内存使用情况
	let cpu_usage=`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $4}' | awk -F'.' '{print $1}'`*10+`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $4}' | awk -F'.' '{print $2}'`
	let total_cpu_usage+=cpu_usage
	let mem_max=`echo -e "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $5}'`*1024
#	let mem_free=mem_max-mem_max*`echo "$xentop_text" | grep -m 1 "" | awk -F' ' '{print $6}' | awk -F'.' '{print $1 $2}'`/1000
	echo -e "\t\t\t\t<_vm_cpu_usage>$cpu_usage</_vm_cpu_usage>"
	echo -e "\t\t\t\t<_vm_mem_max>$mem_max</_vm_mem_max>"
#	echo -e "\t\t\t\t<_vm_mem_free>$mem_free</_vm_mem_free>"
	echo -e "\t\t\t</VmInfo>"
	echo -e "\t\t</item>"
	xentop_text=`echo -e "$xentop_text" | sed '1d'`						# 删除已解析的行
done
echo -e "\t</_vm_info>"

#------------------------------------------------------------------------------
# 打印物理cpu利用率
if [ -f /proc/cpuinfo ]; then
	let total_cpu_usage/=`grep -c -i "processor" /proc/cpuinfo`
	if [ "$total_cpu_usage" -gt 1000 ]; then
		let total_cpu_usage=1000
	fi
	echo -e "\t<_cpu_usage>$total_cpu_usage</_cpu_usage>"
fi

exit 0
