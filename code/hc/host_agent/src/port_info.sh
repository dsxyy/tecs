#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：port_info.sh
# 文件标识：
# 内容摘要：获取主机上各种端口基本信息
# 当前版本：1.0
# 作    者：袁进坤
# 完成日期：2011年9月1日
#
# 修改记录1：
#     修改日期：2011/9/1
#     版 本 号：V1.0
#     修 改 人：liuyi
#     修改内容：创建
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# 获取物理网口列表

#获取网桥
pif_text=`brctl show 2>/dev/null | sed '1d'`
pif_lines=`echo -e "$pif_text" | wc -l`
echo -e "\t<_port_map>"
for ((a=1; a <= pif_lines  ; a++))
do
    interface_name=`echo -e "$pif_text" | grep -m 1 "" | awk -F' ' '{print $4}' |grep -v "\." | grep -v ":" | grep -v "_"`
    bridge_name=`echo -e "$pif_text" | grep -m 1 "" | awk -F' ' '{print $1}' `	
	if [ $interface_name ]
    then
		echo -e "\t\t<$bridge_name>$interface_name</$bridge_name>"
	fi
	pif_text=`echo -e "$pif_text" | sed '1d'`
done

#获取sriov网口
if [ -d "/sys/class/net" ];then
for nic in `ls /sys/class/net`
do
    if [ -n "$nic" ]
    then
        if [ -d "/sys/class/net/$nic/device" ]
	    then
	        vfcount=`ls /sys/class/net/$nic/device | grep -i 'virtfn' | wc -l` 
	        if [ $vfcount -ne 0 ]
	        then
	            echo -e "\t\t<$nic>$nic</$nic>"	    	  
            fi
		fi	
    fi    
done 
fi
echo -e "\t</_port_map>"
#------------------------------------------------------------------------------
# 获取物理网口列表
#获取网桥与网口映射关系
pif_text=`brctl show 2>/dev/null | sed '1d'`
pif_lines=`echo -e "$pif_text" | wc -l`
echo -e "\t<_port_names>"
for ((a=1; a <= pif_lines  ; a++))
do
    interface_name=`echo -e "$pif_text" | grep -m 1 "" | awk -F' ' '{print $4}'  |grep -v "\." | grep -v ":" | grep -v "_"`
    bridge_name=`echo -e "$pif_text" | grep -m 1 "" | awk -F' ' '{print $1}' `
	bond_info=`ls /proc/net/bonding `
	#echo "bond_info $bond_info"

	ret=`echo -e $bond_info | grep  "$interface_name"`
	#echo "ret = $ret !"
	if [ -z "$ret" ] && [ $interface_name ]
    then
		echo -e "\t\t<$bridge_name>$interface_name</$bridge_name>"
	fi
	pif_text=`echo -e "$pif_text" | sed '1d'`
done

#获取sriov网口
if [ -d "/sys/class/net" ];then
for nic in `ls /sys/class/net`
do
    if [ -n "$nic" ]
    then
        if [ -d "/sys/class/net/$nic/device" ]
	    then
	        vfcount=`ls /sys/class/net/$nic/device | grep -i 'virtfn' | wc -l` 
	        if [ $vfcount -ne 0 ]
	        then
	            echo -e "\t\t<$nic>$nic</$nic>"	    	  
            fi
		fi	
    fi    
done 
fi
echo -e "\t</_port_names>"

#------------------------------------------------------------------------------
# 获取bonding口列表
function bond_sh
{
    echo -e "\t<_trunk_info>"
    for bond in  $bond_all
    do
        echo -e "\t\t<item>"    
            echo -e "\t\t\t<Trunk>"
                echo -e "\t\t\t\t\t<_name>${bond}</_name>"
                
                file="/proc/net/bonding/${bond}"
                adatpter=`cat $file 2>/dev/null |grep "Slave"  | awk -F ':' '{print $2}'`
                echo -e "\t\t\t\t\t<_port_names>"
                if [ -n "$adatpter" ] ;then 
		    for i in $adatpter
		    do
        		
            	       echo -e "\t\t\t\t\t\t<item>$i</item>"		        
        		 
        	done
        		
        	fi
        	echo -e "\t\t\t\t\t</_port_names>"
            echo -e "\t\t\t</Trunk>"
        echo -e "\t\t</item>"

   done
   
   
   echo -e "\t</_trunk_info>"
}

bond_all=`ls /proc/net/bonding 2>/dev/null`
if [[ -n $bond_all ]] ;then
bond_sh
fi

#------------------------------------------------------------------------------
# 获取sr-iov端口PCI总线号

echo -e "\t<_sriov>"
if [ -d "/sys/class/net" ];then
for nic in `ls /sys/class/net`
do
    if [ -n "$nic" ]
    then
        if [ -d "/sys/class/net/$nic/device" ]
	then
	    vfcount=`ls /sys/class/net/$nic/device | grep -i 'virtfn' | wc -l` 
	    if [ $vfcount -ne 0 ]
	    then
	        echo -e "\t\t<item>\n\t\t\t<CPCIInfo>"
	        echo -e "\t\t\t\t<_nic_name>$nic</_nic_name>"
	        echo -e "\t\t\t\t<_pci_info>"	    
	  
	        for vir in `ls /sys/class/net/$nic/device | grep -i 'virtfn'`
	        do         
	            pci=`ls -ld /sys/class/net/$nic/device/$vir | tr -s ' ' | cut -f 11 -d ' ' | cut -b 4-`
	            if [ -n $pci ]
	            then
                        virtfn=`echo $vir |cut -b 7-` 
	                echo -e "\t\t\t\t\t<item>\n\t\t\t\t\t\t<CPCI>"
	                echo -e "\t\t\t\t\t\t\t<_pci>$pci</_pci>"
                        echo -e "\t\t\t\t\t\t\t<_virtfn>$virtfn</_virtfn>"
	                echo -e "\t\t\t\t\t\t</CPCI>\n\t\t\t\t\t</item>"
	            fi       
	        done    
	        echo -e "\t\t\t\t</_pci_info>"
                echo -e "\t\t\t</CPCIInfo>"
	        echo -e "\t\t</item>"    
            fi
        fi
    fi    
done 
fi
echo -e "\t</_sriov>"

exit 0
