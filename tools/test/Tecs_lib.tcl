#-------------------------------------------------
#函数名称：cloudInit {}
#功能描述：自动注册集群，注册HOST和配置SVCPU。
#输入参数：tecsIp：		TC物理机IP地址
#		   HCName： 	HC物理机名称
#          svcpuNum： 	HC设置svcpu数
#返回值：1 （成功），0（失败）
#--------------------------------------------------
proc cloudInit { tecsIp HCName svcpuNum } {
    #默认CC集群名
	set CCName "tecscluster"
	
	#局部变量初始化
	set sClusterState        "NULL"
	set sHostState           "NULL"
	set nHostCpuId           "NULL"
	set nSvcpuNum            "0"
	set buffer               {}
	
	puts "开始初始化配置TC/CC/HC环境。。。"
	#自动建立与tecsIp系统中TC所在的“10.43.178.187”的Telnet连接，并进行自动登录，此处默认为admin登录
	#该处会默认尝试3次连接，若均异常，请检查网络设置
	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]

	if {[string equal $result ""] == 1} {
		set buffer [ZX_TelnetSend $tecsIp "?"]
	} else {
		return 0
	}
	#当前CLI所在目录：CLI#
	############################################################################################################################
	#集群注册到TC
	#先进入集群管理
	ZX_TelnetSend $tecsIp "cluster_manager"

	#CLI上发送注册命令
	ZX_TelnetSend $tecsIp "register $CCName"

	#每次将查询结果放入buffer之前，清空buffer
	set buffer         {}

	#查询是否注册成功
	set buffer [ZX_TelnetSend $tecsIp "show 0"]
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
	ZX_TelnetSend $tecsIp "exit"

	#进入host_manager
	ZX_TelnetSend $tecsIp "host_manager"

	#手动配置强制host加入指定CC
	ZX_TelnetSend $tecsIp "host register $CCName $HCName 0"

	after 1000
	#每次将查询结果放入buffer之前，清空buffer
	set buffer         {}

	#检查HC是否成功加入CC
	set buffer [ZX_TelnetSend $tecsIp "show host $CCName $HCName"]
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
	ZX_TelnetSend $tecsIp " "
	ZX_TelnetSend $tecsIp "exit"
	ZX_TelnetSend $tecsIp "cluster_config"

	#每次将查询结果放入buffer之前，清空buffer
	set buffer         {}

	#为了保证查询获取的内容可以写入buffer，此处作延时5S处理
	after 2000

	#配置SVCPU
	ZX_TelnetSend $tecsIp "svcpu $CCName $nHostCpuId $svcpuNum"

	after 5000
	#检查是否配置SVCPU成功
	#问题：此处发送查看命令时，有时无法发送成功，增加延时后解决
	set buffer [ZX_TelnetSend $tecsIp "show svcpu $CCName $nHostCpuId"]

	#查询SVCPU的数量是否和配置的一致，进行数据检查
	regexp {svcpu_num      = (\d+)} $buffer {} nSvcpuNum
	if {$nSvcpuNum == $svcpuNum} {
		ZX_Puts -normal "Success: have cfg the svcpu($svcpuNum) of CC($CCName)"
		after 1000
		ZX_TelnetSend $tecsIp "exit"
#		ZX_DisconnectHost $tecsIp
		return 1
	} else {
		ZX_Puts -critical "Failure: need to recfg svcpu($svcpuNum) of CC($CCName) manually.Please check the reason"
		return 0
	}	
}

#cloudInit "10.44.129.115" "Pwvir01" "2000"

package require ftp

#FTP上传所需全局变量
#文件总大小
global g_TotalSize 
#最新下载百分比
global g_LastDownloadPercent
#映像文件ID
global g_imgID
set g_TotalSize 0
set g_LastDownloadPercent 0
set g_imgID 0
#函数名称:putslog {}：交互信息回显
proc putslog {aa bb  cc} {
    global g_imgID
	puts "$bb"
	regexp "TC_FileStorageDB is : (\\d+)" $bb {} g_imgID
	if { $g_imgID != 0 } {
	    return $g_imgID
	}
}
#函数名称:DownloadProgressBar {}：下载进度显示
proc DownloadProgressBar {downloadedsize} {
    global g_TotalSize
    global g_LastDownloadPercent
   if {$g_TotalSize > 0} {
       set percent  \
       [expr ($downloadedsize * 100) / $g_TotalSize]
       
       #当百分比有变化时才打印
       if {$percent > 0 && $percent != $g_LastDownloadPercent} {
           set upsize [expr $downloadedsize / 1024 / 1024]
           set tolsize [expr $g_TotalSize / 1024 / 1024]
           puts "已上传数据 $upsize M，\
           数据总大小 $tolsize M，完成 $percent %"
           
           set g_LastDownloadPercent $percent
       }
   } else {
       puts "已上传数据 $downloadedsize bytes。"
   }
}
#函数名称:GetFtpFileSize {}：计算文件的大小
proc GetFtpFileSize {filename} {
    global g_TotalSize
    set g_TotalSize [file size $filename]
    if {$g_TotalSize != ""} {      
        return $g_TotalSize   
    } else {
        ZX_Puts -warning "获取文件大小失败，无法计算下载进度。"
        return 0
    }
}
#-------------------------------------------------
#函数名称:ImgPut {}
#功能描述：自动上传image文件
#输入参数：tecsIp： TC物理机IP地址
#          ImageFile   映像名称
#返回值：ID（成功），0（失败）
#--------------------------------------------------
proc ImgPut {tecsIp ImageFile} {
    global g_imgID   
    #FTP上传映像到TECS机器
    set ::ftp::VERBOSE 1
    set buffer {}
	set handle [::ftp::Open $tecsIp admin admin \
							-blocksize 4096 -progress {DownloadProgressBar} -timeout 30 -output {putslog}]
	if {$handle == -1} {
    ZX_Puts -critical "Error:连接FTP服务器错误，请检查FTP地址配置是否正确。"
    return 0
} else { 
#准备上传映像文件      
    # 取得img文件大小
    set g_TotalSize [GetFtpFileSize $ImageFile]
    
    puts "开始往服务器上传文件 $ImageFile ..."
    set result [::ftp::Put $handle $ImageFile]
    if {$result == 0} {
        ZX_Puts -critical "Error:FTP上传文件$ImageFile 出现错误。"
        return 0
    } else {
        puts "完成文件 $ImageFile 下载。"
        puts "The FileID in TC_FileStorageDB is :$g_imgID"
        return $g_imgID
    }
}
::ftp::Close $handle
}
#-------------------------------------------------
#函数名称:regImage {tecsIp  ImageName ImageFile}
#功能描述：自动上传image文件
#输入参数：tecsIp：TC物理机IP地址
#		   ImageName：注册映像名
#          ImageFile：映像文件全路径
#返回值：ID，失败返回 0
#--------------------------------------------------
proc regImage {tecsIp ImageName ImageFile} {
    #映像类型
	set ImageType            "machine"
	#映像支持的系统结构
	set ImageArchType        "x86_64"
	#是否设置公有属性，TRUE：公有；FALSE：私有
	set ImagePublicOrNot     "TRUE"
	#是否设置映像可用，TRUE：可用；FALSE：不可用
	set ImageAvailableOrNot  "TRUE"
	#映像描述信息，支持字母、数字、下划线组合，不支持中文、空格等
	set ImageDescription     "hvm_system"
	
	#局部变量初始化
	set nImageId             "0"
	set nGetImageId          "0"
	set SuccessOrNot 		 "0"
	
	puts "开始上传和注册映像。。。"    
    #获取映像文件名称
    regexp {\w+.img} $ImageFile StorName 
    
    #登录CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#当前CLI所在目录：CLI#image
	ZX_TelnetSend $tecsIp "image"
	set buffer [ZX_TelnetSend $tecsIp "show create"]
	after 5000
	puts $buffer
	#直接在buffer中查找配置的映像名称是否存在，若存在则返回1，否则返回0
	set SuccessOrNot [regexp "name(\\s+)=(\\s+)$ImageName" $buffer {}]
	#puts "SuccessOrNot is $SuccessOrNot"

	#判断配置的映像是否可以查到
	if {$SuccessOrNot == "1"} {
		#若发现映像已经注册在系统中，则直接获取ImageId
		ZX_Puts -normal "Warning: The Image($ImageName) has registered in tc,no need to register the image again."
		set buffer         {}
		set buffer [ZX_TelnetSend $tecsIp "show_file upload"]
		puts $buffer
		regexp "(\\d+)    $StorName" $buffer {} nImageId
		puts "nImageId get is $nImageId"
	
	} else {
		#若发现映像未在系统中注册，再检查是否已经上传
		set SuccessOrNot "0"
		set buffer [ZX_TelnetSend $tecsIp "show_file upload"]
		set SuccessOrNot [ regexp "(\\d+)    $StorName" $buffer {} nImageId ]
		puts "nImageId get is $nImageId"
		if {$SuccessOrNot == 1} {
			#发现映像文件已经上传了，直接进行注册
			ZX_Puts -normal "Warning: The Image($StorName) has already exist in tc, just need to register it."
		} else {
			#发现映像文件没有上传过，需要FTP上传
			ZX_Puts -normal "Warning: The Image($StorName) is not exist in tc.It will ftp the Image below,you need to wait for a minute."
			#FTP上传映像文件到TC_FileStorage
			set nImageId [ImgPut $tecsIp $ImageFile]
			puts "nImageId get is $nImageId"
		}
	}
	#校验此处是否正常获取映像文件存储的ID
	if {$nImageId == "0"} {
		ZX_Puts -critical "Failure: Fail to ftp the image file($StorName).Please check the reason"
		return 0
	} else {
		ZX_Puts -normal "Success: Succeed to ftp the image file($StorName)"
		ZX_Puts -normal "Success: The Image_file's($StorName) Image ID is $nImageId"
	}
	
	set buffer [ZX_TelnetSend $tecsIp "register $nImageId $ImageName $ImageType $ImageArchType $ImagePublicOrNot $ImageAvailableOrNot description $ImageDescription"]

	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "show id $nImageId"]
	puts $buffer
	after 1000
	#从show create中查询是否存在该imageid，若有则注册成功，否则返回注册失败
	regexp {image_id      =(\d+)} $buffer {} nGetImageId
	if {$nGetImageId != $nImageId} {
		ZX_Puts -critical "Failure: Fail to register the image(imageid: $nImageId).Please check the reason"
		return 0
	} else {
		ZX_Puts -normal "Success: Succeed to register the image(imageid: $nImageId)"
		after 1000
		ZX_TelnetSend $tecsIp "exit"
#		ZX_DisconnectHost $tecsIp
		return $nImageId
	}			
}

#regImage $tecsIp $ImageName $ImageFile

#-------------------------------------------------
#函数名称:newProj {tecsIp  ProjectName ImageFile}
#功能描述：创建工作组
#输入参数：tecsIp：TC物理机IP地址
#		   ProjectName：工程组名
#          ProjectDes：工程组描述信息
#返回值：1 （成功），0（失败）
#--------------------------------------------------proc newProj {tecsIp ProjectName ProjectDes} {
    #局部变量初始化
    set SuccessOrNot "0"
    set buffer {}
    
    puts "开始新建工程组。。。"   
    #登录CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#进入工程组配置界面
	ZX_TelnetSend $tecsIp "project"

	ZX_TelnetSend $tecsIp "create $ProjectName $ProjectDes"
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "show name $ProjectName"]

	#直接在buffer中查找配置的工程组名称是否存在，若存在则返回1，否则返回0
	set SuccessOrNot [regexp {[$ProjectName]} $buffer {}]
	#判断配置的工程组是否可以正常show到，是否配置成功
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: have cfg the projcet($ProjectName)"
		after 1000
		ZX_TelnetSend $tecsIp "exit"
#		ZX_DisconnectHost $tecsIp
		return 1
	} else {
		ZX_Puts -critical "Failure: need to recfg the project($ProjectName) manually.Please check the reason"
		return 0
	}		}

#newProj $tecsIp $ProjectName $ProjectDes

#-------------------------------------------------
#函数名称:createVM {tecsIp VmName ProjectName VmNum VmCpu VmMemory ImageId busType VmVirtType}
#功能描述：创建新虚拟机
#输入参数：tecsIp：TC物理机IP地址
#		   VmName：虚拟机名称
#          ProjectName：工程组名#          VmNum：虚拟机数量
#          VmCpu：vcpu个数
#          VmMemory：虚拟机内存大小
#          ImageId：映像ID
#          busType：存储内心
#          VmVirtType：虚拟机类型
#返回值：vm id （成功），0（失败）
#--------------------------------------------------
proc createVM {tecsIp VmName ProjectName VmNum VmCpu VmMemory ImageId busType VmVirtType} {
	#局部变量初始化
    set SuccessOrNot "0"
    set buffer {}
    
    puts "开始创建虚拟机。。。"   
    #登录CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#配置虚拟机
	ZX_TelnetSend $tecsIp "vm"
	set buffer [ZX_TelnetSend $tecsIp "create direct $VmName $ProjectName vm_num $VmNum $VmCpu $VmMemory machine $ImageId bus $busType virt_type $VmVirtType"]
	puts $buffer

	#配置多个VM时，只获取第一个VM ID进行部署操作
	regexp {The ID are:\n(\d+)\s+} $buffer {} VmId
	puts $VmId

	set SuccessOrNot [regexp {Success create} $buffer {}]
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: have cfg the vm($VmName)"
		after 1000
		ZX_TelnetSend $tecsIp "exit"
#		ZX_DisconnectHost $tecsIp
		return $VmId
	} else {
		ZX_Puts -critical "Failure: need to recfg the vm($VmName) manually.Please check the reason"
		return 0
	}		
}

#creatVM $tecsIp $VmName $ProjectName $VmNum $VmCpu $VmMemory $ImageId $busType $VmVirtType

#-------------------------------------------------
#函数名称:operateVM {tecsIp VmId  interval}
#功能描述：创建工作组
#输入参数：tecsIp：TC物理机IP地址
#		   VmId：虚拟机ID
#          interval：操作间隔
#返回值：1 （成功），0（失败）
#--------------------------------------------------
proc operateVM {tecsIp VmId  interval} {
	#局部变量初始化
    set SuccessOrNot "0"
    set buffer {}
    
    puts "开始部署和操作虚拟机。。。"   
    #登录CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#部署虚拟机，只部署之前获取的第一个VM
	ZX_TelnetSend $tecsIp "vm"
	set buffer [ZX_TelnetSend $tecsIp "show $VmId"]
	regexp {vm_name          = (\w+)} $buffer {} VmName
	
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "operate $VmId deploy"]

	#部署是否成功确认
	after 20000
	puts $buffer

	set SuccessOrNot [regexp {OK} $buffer {}]
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: have deploy the vm(VmName:$VmName VmId:$VmId)"
	} else {
		ZX_Puts -critical "Failure: fail to deploy the vm(VmName:$VmName  VmId:$VmId),please check the reason"
		return 0
	}
	#当前CLI所在目录：CLI(vm)#
	############################################################################################################################
	after $interval
	#停止虚拟机
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "operate $VmId stop"]
	#停止是否成功确认
	after 20000
	puts $buffer

	set SuccessOrNot [regexp {OK} $buffer {}]
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: stop the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to stop the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #当前CLI所在目录：CLI(vm)#
    ############################################################################################################################
    #延时30秒，以保证VM真正进行状态跃迁
    after $interval
    #启动虚拟机
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId start"]
    #启动是否成功确认
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: start the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to start the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #当前CLI所在目录：CLI(vm)#
    ############################################################################################################################
     after $interval
    #暂停虚拟机
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId pause"]
    #暂停是否成功确认
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: pause the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to pause the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #当前CLI所在目录：CLI(vm)#
    ############################################################################################################################    
    after $interval
    #恢复虚拟机
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId resume"]
    #恢复是否成功确认
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]	
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: resume the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to resume the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #当前CLI所在目录：CLI(vm)#
    ############################################################################################################################
    after $interval
    #撤销虚拟机
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId cancel"]
    #撤销是否成功确认
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]	
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: cancel the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to cancel the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    after 1000
	ZX_TelnetSend $tecsIp "exit"
#	ZX_DisconnectHost $tecsIp	
	return 1
}

#operateVM $tecsIp $VmId $interval