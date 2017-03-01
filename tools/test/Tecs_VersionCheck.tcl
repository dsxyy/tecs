#-------------------------------------------------------------------------------------------------------------------------
# 文件名称：  Tecs_VersionCheck.tcl                                                     
# 测试目的:   用于Tecs项目版本验证(默认使用admin用户)
# 作    者：  沈毅
# 完成日期：  2012/03/08 16:36:00
# 修改记录1： 
# 修改日期：  2012/03/28
# 修 改 人：  彭伟
# 修改内容：  封装部分配置过程为函数，方便调用；另外增加centos5和winxp虚拟机的部署。
#------------------------------------------------------------------------------------------------------------------------
############################################################################################################################
source "[file dirname [info script]]/Tecs_lib.tcl"

#脚本执行前需要修改的配置项，包括TC所在的IP地址、CC名称、HC所在物理机名称、svcpu的数量
#TC所在物理机的IP地址
set tecsIp               "10.44.129.115" 
#需要进行注册的CC的名称
set CCName               "tecscluster"
#HC进程所在的物理机名称
set HCName               "Pwvir01"

#CC配置HC的SVCPU的数量(0-2147483647)
set svcpuNum             "1000"

#文件映像部分的参数，参数请遵照字母、数字、下划线组合的方式命名
#映像文件名称，包含文件类型
set tty_lin01            "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\ttylinux0.img"
set centos_lin02         "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\centos5_hvm_20120202.img"
set win_xp03             "F:\\TecsDoc\\zxve\\Tecs_Images\\images4windows\\windowsxp.img"
#映像名称，不包含文件类型
set tty_img01            "ttylinux0"
set centos_img02         "centos5_img"
set winxp_img03          "winxp2012_img"
#映像类型
#set ImageType            "machine"
#映像支持的系统结构
#set ImageArchType        "x86_64"
#是否设置公有属性，TRUE：公有；FALSE：私有
#set ImagePublicOrNot     "TRUE"
#是否设置映像可用，TRUE：可用；FALSE：不可用
#set ImageAvailableOrNot  "TRUE"
#映像描述信息，支持字母、数字、下划线组合，不支持中文、空格等
#set ImageDescription     "hvm_system"

#工程组相关配置参数设置
#创建VM所在的工程名称，支持字母、数字、下划线组合
set Proj01                "default"
set Proj02                "linux_group"
#工程描述信息，支持字幕、数字、下划线组合
set ProjDes01             "default_project"
set ProjDes02             "linux_system_workgroup"  

#虚拟机配置相关参数设置
#虚拟机名称
set ttyvm01               "tty_linux01"
set centosvm02            "centos5"
set winxpvm03             "winxp2012"
#需要创建的虚拟机数量
set VmNum01                "2"
set VmNum02                "1"
set VmNum03                "1"
#每个虚拟机所包含的虚拟机CPU的数量
set VmCpu01                "4"
set VmCpu02                "2"
set VmCpu03                "1"
#虚拟内存大小，单位M，取值范围：1~17592186044416
set VmMemory01             "50"
set VmMemory02             "1024"
set VmMemory03             "2048"
#虚拟类型，包括hvm、pvm。选择的虚拟类型需要和所加载的映像参数保持一致
set VmbusType              "ide"
set VmVirtType             "hvm"

#以上部分需要在脚本执行前手动配置相关参数，需人为保证参数关联正确，对入参不做校验

############################################################################################################################
############################################################################################################################
#登录CLI进行CC注册，HC注册和配置SVCPUcloudInit $tecsIp $HCName $svcpuNum

############################################################################################################################
after 2000
#登录CLI进行映像注册
#注册ttylinux0.img并返回其IDset ttyimgid [regImage $tecsIp $tty_img01 $tty_lin01]
puts "tty linux image id is: $ttyimgid."
after 2000
set cetnosimgid [regImage $tecsIp $centos_img02 $centos_lin02]
puts "centos5 image id is: $cetnosimgid."
after 2000
set winxpimgid [regImage $tecsIp $winxp_img03 $win_xp03]
puts "winxp image id is: $winxpimgid."
############################################################################################################################
after 2000
#登录CLI进行工程组创建
#创建第一个工作组defaultnewProj $tecsIp $Proj01 $ProjDes01
after 2000
newProj $tecsIp $Proj02 $ProjDes02
############################################################################################################################
after 2000
#登录CLI进行虚拟机创建
#创建2个tty_linux的虚拟机，并返回第一个虚拟机ID
set ttyvmid [createVM $tecsIp $ttyvm01 $Proj01 $VmNum01 $VmCpu01 $VmMemory01 $ttyimgid $VmbusType $VmVirtType]
after 2000
set centosvmid [createVM $tecsIp $centosvm02 $Proj02 $VmNum02 $VmCpu02 $VmMemory02 $cetnosimgid $VmbusType $VmVirtType]
after 2000
set winxpvmid [createVM $tecsIp $winxpvm03 $Proj02 $VmNum03 $VmCpu03 $VmMemory03 $winxpimgid $VmbusType $VmVirtType]
############################################################################################################################
after 2000
#登录CLI进行虚拟机部署和操作
#部署和操作tty linux虚拟机,操作间隔为10s
operateVM $tecsIp $ttyvmid 10000
after 2000
#部署和操作centos linux虚拟机,操作间隔为30s
operateVM $tecsIp $centosvmid 30000
after 2000
#部署和操作windows xp虚拟机,操作间隔为180s
operateVM $tecsIp $winxpvmid 180000
after 10000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#网络平面

#DHCP

#其他补充















