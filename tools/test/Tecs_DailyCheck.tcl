#-------------------------------------------------------------------------------------------------------------------------
# 文件名称：  Tecs_DailyCheck.tcl                                                     
# 测试目的:   用于Tecs项目日验证(默认使用admin用户)
# 作    者：  沈毅
# 完成日期：  2012/03/08 16:36:00
# 修改记录1： 修改历史记录，包括修改日期、修改者及修改内容
# 修改日期：
# 修 改 人：
# 修改内容： 
# 修改记录2：…
#------------------------------------------------------------------------------------------------------------------------
#需要增加版本卸载自动化脚本

#需要增加版本安装自动化脚本

#需要增加TECS进程启动相关脚本

############################################################################################################################
#脚本执行前需要修改的配置项，包括TC所在的IP地址、CC名称、HC所在物理机名称、svcpu的数量
#TC所在物理机的IP地址
set tecs                 "10.43.178.187" 
#需要进行注册的CC的名称
set CCName               "cc_0305"
#HC进程所在的物理机名称
set HCName               "shenyi_host"
#CC配置的SVCPU的数量(0-256)
set CCsvcpuNum           "100"
#本地映像文件所在目录，最好设置磁盘根目录，暂不支持中文路径
set LocalDir             "I:\\07test_tty.img"

#文件映像部分的参数，参数请遵照字母、数字、下划线组合的方式命名
#映像文件名称，包含文件类型
set FileName             "07test_tty.img"
#映像名称，不包含文件类型
set ImageName            "07test_tty"
#映像类型
set ImageType            "machine"
#映像支持的系统结构
set ImageArchType        "x86_64"
#是否设置公有属性，TRUE：公有；FALSE：私有
set ImagePublicOrNot     "TRUE"
#是否设置映像可用，TRUE：可用；FALSE：不可用
set ImageAvailableOrNot  "TRUE"
#映像描述信息，支持字母、数字、下划线组合，不支持中文、空格等
set ImageDescription     "for_testing"

#工程组相关配置参数设置
#创建VM所在的工程名称，支持字母、数字、下划线组合
set ProjectName          "pj_03"
#工程描述信息，支持字幕、数字、下划线组合
set ProjectDes           "project_des"

#虚拟机配置相关参数设置
#虚拟机名称
set VmName               "vm_03"
#需要创建的虚拟机数量
set VmNum                "2"
#每个虚拟机所包含的虚拟机CPU的数量
set VmCpu                "3"
#虚拟内存大小，单位M，取值范围：1~17592186044416
set VmMemory             "50"
#虚拟类型，包括hvm、pvm。选择的虚拟类型需要和所加载的映像参数保持一致
set VmVirtType           "hvm"

#以上部分需要在脚本执行前手动配置相关参数，需人为保证参数关联正确，对入参不做校验
############################################################################################################################
#局部变量初始化
set sClusterState        "NULL"
set sHostState           "NULL"
set nHostCpuId           "NULL"
set nSvcpuNum            "0"
set buffer               {}
set nImageId             "0"
set nGetImageId          "0"
set SuccessOrNot         "0"
#内部用于当前SSH连接的ID
set a                    "0"
############################################################################################################################
#自动建立与Tecs系统中TC所在的“10.43.178.187”的Telnet连接，并进行自动登录，此处默认为admin登录
#该处会默认尝试3次连接，若均异常，请检查网络设置
set result [ZX_LoginHost $tecs 24 "Username:" "admin" "Password:" "admin" "CLI#"]

if {[string equal $result ""] == 1} {
	set buffer [ZX_TelnetSend $tecs "?"]
} else {
	return [puts $result]
}
#当前CLI所在目录：CLI#
############################################################################################################################
#集群注册到TC
#先进入集群管理
ZX_TelnetSend $tecs "cluster_manager"

#CLI上发送注册命令
ZX_TelnetSend $tecs "register $CCName"

#每次将查询结果放入buffer之前，清空buffer
set buffer         {}

#查询是否注册成功
set buffer [ZX_TelnetSend $tecs "show 0"]
regexp {register = (\w+)} $buffer {} sClusterState
if {$sClusterState == "REGISTER"} {
	ZX_Puts -normal "Success: cc($CCName) has registered in tc"
} else {
	ZX_Puts -critical "Failure: Fail to register cc($CCName).Please check the reason"
	return
}
#当前CLI所在目录：CLI(cluster_manager)#
############################################################################################################################
#主机注册到CC
#退出cluster_manager
ZX_TelnetSend $tecs "exit"

#进入host_manager
ZX_TelnetSend $tecs "host_manager"

#手动配置强制host加入指定CC
ZX_TelnetSend $tecs "host register $CCName $HCName 0"

after 1000
#每次将查询结果放入buffer之前，清空buffer
set buffer         {}

#检查HC是否成功加入CC
set buffer [ZX_TelnetSend $tecs "show host $CCName $HCName"]
after 2000
regexp {register_state = (\w+)} $buffer {} sHostState
if {$sHostState == "REGISTERED"} {
	ZX_Puts -normal "Success: host($HCName) has registered in cc($CCName)"
} else {
	ZX_Puts -critical "Failure: Fail to register host($HCName) in cc($CCName).Please check the reason"
	return
}
#当前CLI所在目录：CLI(host_manager)#
############################################################################################################################
#定义svcpu
#首先继续在host_manager下获取host的 cpu_info_id
regexp {cpu_info_id    = (\w+)} $buffer {} nHostCpuId


#问题：此处有时无法获取到nHostCpuId
puts "The host cpu id is $nHostCpuId"
#puts $buffer

#退出host_manager，并进入cluster_config，此处因为可能存在查询列表较多，需要输入多个空格才能获取有效信息
ZX_TelnetSend $tecs " "
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "cluster_config"

#每次将查询结果放入buffer之前，清空buffer
set buffer         {}

#为了保证查询获取的内容可以写入buffer，此处作延时5S处理
after 2000

#配置SVCPU
ZX_TelnetSend $tecs "svcpu $CCName $nHostCpuId $CCsvcpuNum"

after 5000
#检查是否配置SVCPU成功
#问题：此处发送查看命令时，有时无法发送成功，增加延时后解决
set buffer [ZX_TelnetSend $tecs "show svcpu $CCName $nHostCpuId"]

#查询SVCPU的数量是否和配置的一致，进行数据检查
regexp {svcpu_num      = (\d+)} $buffer {} nSvcpuNum
if {$nSvcpuNum == $CCsvcpuNum} {
	ZX_Puts -normal "Success: have cfg the svcpu($CCsvcpuNum) of CC($CCName)"
} else {
	ZX_Puts -critical "Failure: need to recfg svcpu($CCsvcpuNum) of CC($CCName) manually.Please check the reason"
	return
}

after 5000
ZX_TelnetSend $tecs "exit"
#当前CLI所在目录：CLI#
############################################################################################################################
after 5000
#上传映像文件到root目录
#问题：映像上传无法通过CLI命令来实现，考虑通过FTP的方式上传映像文件
#先上传映像文件到linux机器的本地目录/root
set a [ZX_SSHConnect $tecs 22 root tecs123]
puts $a

set buffer {}
set SuccessOrNot "0"
ZX_SSHSend $a "cd /root"
set buffer [ZX_SSHSend $a "ll"]
puts $buffer
puts $FileName
puts "SuccessOrNot is $SuccessOrNot"
#直接在buffer中查找配置的文件名称是否存在，若存在则返回1，否则返回0
#set SuccessOrNot [regexp -all {[$FileName]} $buffer {}]set SuccessOrNot [regexp "$FileName" $buffer {}]
after 2000
puts "SuccessOrNot is $SuccessOrNot"
#判断配置的文件是否可以正常查到
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "The ImageFile is already exist.No need to put the same file,or you can change your local file's name,and put it again."
} else {
	ZX_Puts -normal "The ImageFile is not exist.It will trans the file below."
#	after 2000
	ZX_SFTPOpen $a
	ZX_SFTPPutFile $a "$LocalDir" "/root/$FileName"

	ZX_SFTPClose $a
	ZX_SSHDisconnect $a	}
############################################################################################################################
#查询确认是否需要FTP文件，若没有注册，则先FTP文件，再注册映像；若已经注册，则直接获取ImageId.set buffer {}
set SuccessOrNot "0"
ZX_TelnetSend $tecs "image"
set buffer [ZX_TelnetSend $tecs "show create"]
after 5000
puts $buffer
puts $ImageName
puts "SuccessOrNot is $SuccessOrNot"
#直接在buffer中查找配置的映像名称是否存在，若存在则返回1，否则返回0
set SuccessOrNot [regexp "name(\\s+)=(\\s+)$ImageName" $buffer {}]
puts "SuccessOrNot is $SuccessOrNot"

#判断配置的映像是否可以查到
if {$SuccessOrNot == "1"} {
    #若发现映像已经注册在系统中，则直接获取ImageId
	ZX_Puts -normal "Warning: The Image($ImageName) has registered in tc,no need to ftp the image again."
	set buffer         {}
	set buffer [ZX_TelnetSend $tecs "show_file upload"]
	puts $buffer
	puts $ImageName
	regexp "(\\d+)    $ImageName" $buffer {} nImageId
	puts "nImageId get is $nImageId"
	
} else {
    #若发现映像未在系统中注册，则先FTP该映像，再进行注册
	ZX_Puts -normal "Warning: The Image($ImageName) is not exist in tc.It will ftp the Image below,you need to wait for a minute."
	#通过FTP将映像上传到对应的目录
	#发送对应的FTP的IP
	set b [ZX_SSHConnect $tecs 22 root tecs123]
	set r1 [ZX_SSHSend $b "ftp $tecs" 20 "Name ($tecs:root): "]

	#发送对应的FTP的用户名，默认是admin
	set r2 [ZX_SSHSend $b "admin" 20 "Password:"]

	#发送对应的FTP的密码，默认是admin
	set r3 [ZX_SSHSend $b "admin" 20 "ftp>"]

	#校验是否FTP登录成功，此处昝略，可以通过发送pwd，来匹配结果是否是/mnt/tecs_tc/1来验证
	#
	#直接将映像上传
	set r4 [ZX_SSHSend $b "put $FileName" 600 "ftp>"]
#	puts $r4

	#校验此处是否正常获取文件存储的ID
	regexp {FileID in TC_FileStorageDB is : (\w+)} $r4 {} nImageId
	if {$nImageId == "0"} {
		ZX_Puts -critical "Failure: Fail to ftp the image file($FileName).Please check the reason"
		return
	} else {
		ZX_Puts -normal "Success: Succeed to ftp the image file($FileName)"
		ZX_Puts -normal "Success: The Image_file's($FileName) Image ID is $nImageId"
	}
	
#	after 2000
	set buffer [ZX_TelnetSend $tecs "register $nImageId $ImageName $ImageType $ImageArchType $ImagePublicOrNot $ImageAvailableOrNot description $ImageDescription"]

	set buffer {}
	set buffer [ZX_TelnetSend $tecs "show id $nImageId"]
	#puts $buffer
	after 5000
	#从show create中查询是否存在该imageid，若有则注册成功，否则返回注册失败
	regexp {image_id      =(\d+)} $buffer {} nGetImageId
	if {$nGetImageId != $nImageId} {
		ZX_Puts -critical "Failure: Fail to register the image(imageid: $nImageId).Please check the reason"
		return
	} else {
		ZX_Puts -normal "Success: Succeed to register the image(imageid: $nImageId)"
	}	
	
}
#当前CLI所在目录：CLI(image)#
############################################################################################################################
#进入工程组配置界面
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "project"

ZX_TelnetSend $tecs "create $ProjectName $ProjectDes"
set buffer {}
set buffer [ZX_TelnetSend $tecs "show name $ProjectName"]

#直接在buffer中查找配置的工程组名称是否存在，若存在则返回1，否则返回0
set SuccessOrNot [regexp {[$ProjectName]} $buffer {}]
#判断配置的工程组是否可以正常show到，是否配置成功
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have cfg the projcet($ProjectName)"
} else {
	ZX_Puts -critical "Failure: need to recfg the project($ProjectName) manually.Please check the reason"
	return
}
#当前CLI所在目录：CLI(pro_cfg)#
############################################################################################################################

#配置虚拟机
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "vm"

set buffer {}
set buffer [ZX_TelnetSend $tecs "create direct $VmName $ProjectName vm_num $VmNum $VmCpu $VmMemory machine $nImageId bus ide virt_type $VmVirtType"]
puts $buffer

#配置多个VM时，只获取第一个VM ID进行部署操作
regexp {The ID are:\n(\d+)\s+(\d+)} $buffer {} VmId
puts $VmId

set SuccessOrNot [regexp {Success create} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have cfg the vm($VmName)"
} else {
	ZX_Puts -critical "Failure: need to recfg the vm($VmName) manually.Please check the reason"
	return
}
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#部署虚拟机，只部署之前获取的第一个VM
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId deploy"]

#部署是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have deploy the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to deploy the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
after 10000
#停止虚拟机
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId stop"]
#停止是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: stop the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to stop the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}
#延时30秒，以保证VM真正进行状态跃迁
after 30000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#启动虚拟机
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId start"]
#启动是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: start the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to start the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}

after 10000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#暂停虚拟机
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId pause"]
#暂停是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: pause the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to pause the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}

after 10000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#恢复虚拟机
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId resume"]
#恢复是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]	
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: resume the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to resume the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}

after 10000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#撤销虚拟机
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId cancel"]
#撤销是否成功确认
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]	
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: cancel the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to cancel the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}

after 10000
#当前CLI所在目录：CLI(vm)#
############################################################################################################################
#网络平面

#DHCP

#其他补充















