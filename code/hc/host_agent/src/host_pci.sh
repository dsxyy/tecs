#!/bin/bash

###################################################################################################
#name: identifyboardtype
#func: 识别单板类型
#para: 无
#return: 单板类型
function identifyboardtype()
{
    echo $kcs | grep -q "Id:537," && board="SBCJ"; return;

    board="UNKNOWNBOARD";
    echo "UNKNOWNBOARD";
}

###################################################################################################
#func: 对需要透传到domu的host上的PCI设备执行pciback的操作，为虚拟机使用PCI设备做准备。
function pciback2Domu()
{
    for pci in $@ ; do
        if [ -d "/sys/bus/pci/devices/$pci" ];then
            #1 unbind the driver that has beed loaded
            #不存在的时候才解绑
            [ ! -e "/sys/bus/pci/devices/$pci/driver/unbind" ] || echo -n $pci >/sys/bus/pci/devices/$pci/driver/unbind
            #2 bind pciback driver for virtual function
            if [ -f "/sys/bus/pci/drivers/pciback/new_slot" ] && [ -f "/sys/bus/pci/drivers/pciback/bind" ];
            then
                echo -n $pci >/sys/bus/pci/drivers/pciback/new_slot
                echo -n $pci >/sys/bus/pci/drivers/pciback/bind
                #echo $pci
            fi
        fi
    done
}

###################################################################################################
globaltab=0;
function PrntTab()
{
    for ((a=0;a<globaltab;a++)) ; do echo -n -e '\t' >> $pci_xml; done
}

pci_xml="/var/run/tecs/pci_passthrought.xml"

###################################################################################################
function SERIALIZE_BEGIN()
{
    PrntTab
    let globaltab++;
    echo "<"$1">" >> $pci_xml
}

###################################################################################################
function SERIALIZE_END()
{
    let globaltab--;
    PrntTab
    echo "</"$1">" >> $pci_xml
}

###################################################################################################
function WRITE_VALUE()
{
    PrntTab
    echo "<"$1">"$2"</"$1">" >> $pci_xml
}

###################################################################################################
function WRITE_PCI()
{
    SERIALIZE_BEGIN _pci_buses

    for pci in $@ ; do

        pci=${pci//./:}     #将pci最后一个.转换成:

        domain=`echo $pci | awk -F ":" '{print $1}'`
        bus=`echo $pci | awk -F ":" '{print $2}'`
        slot=`echo $pci | awk -F ":" '{print $3}'`
        function=`echo $pci | awk -F ":" '{print $4}'`

        SERIALIZE_BEGIN item
            SERIALIZE_BEGIN PciBus
                WRITE_VALUE '_domain' '0x'$domain
                WRITE_VALUE '_bus' '0x'$bus
                WRITE_VALUE '_slot' '0x'$slot
                WRITE_VALUE '_function' '0x'$function
            SERIALIZE_END PciBus
        SERIALIZE_END item

    done

    SERIALIZE_END _pci_buses
}

###################################################################################################
function PDHSerialize()
{
    #判断是否有pdh设备，有则返回1，无则返回0
    echo $kcs | grep -q 'Id:2562,' && echo 'have pdh' || { echo 'no pdh'; return 0; }

    case $board in
    SBCJ)   pci_array=("0000:88:00.2" "0000:86:00.0");;
    *)      echo "unknown"; return 0;;
    esac

    #pci back操作在centos里完成了，故脚本里不在需要。
    #pciback2Domu "${pci_array[@]}"

    SERIALIZE_BEGIN item
        SERIALIZE_BEGIN PCIPassthrough
            WRITE_VALUE '_type' '1'
            WRITE_VALUE '_vid' '0'
            WRITE_PCI "${pci_array[@]}"
        SERIALIZE_END PCIPassthrough
    SERIALIZE_END item

    return 1;
}

###################################################################################################
##将pci透传设备输出至pci_passthrought.xml
function PCIPthManagerSerialize()
{
    ##需先判断pci_passthrought.xml是否已存在
    if [ -e "$pci_xml" ]; then
        echo "$pci_xml is existed!"
        return;
    fi

    devices=0;

SERIALIZE_BEGIN PCIPthManager
    SERIALIZE_BEGIN _devices
        PDHSerialize
    let "devices+=$?"
if [ $devices -ne 0 ];then
    SERIALIZE_END _devices  #如果设备表是空的，就不需要输入这句话了
else
    let globaltab--
    sed -i "2d"  $pci_xml
    SERIALIZE_BEGIN _devices/
    let globaltab--    
fi
SERIALIZE_END PCIPthManager
}

###################################################################################################
##########################
## Main entry of pci operate  ##
##########################
if [ -x "/usr/local/bin/kcscmd" ];then
    kcs=`/usr/local/bin/kcscmd -i`;     #全局变量，保存从kcscmd读到的值
fi

board="UNKNOWNBOARD";               #全局变量，保存单板类型
identifyboardtype
echo "Board is "$board

PCIPthManagerSerialize

exit $?
