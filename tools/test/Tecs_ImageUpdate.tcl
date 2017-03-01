#-------------------------------------------------
#文件名称:Tecs_ImageUpdate.tcl
#功能描述：自动上传image文件和注册
#输入参数：tecsIp： TC物理机IP地址
#          FileName：   映像名称
#返回值：ID
#修改日期        修改人        修改内容
#2012/3/27       PENGWEI       新增 
#--------------------------------------------------
#TC所在物理机的IP地址
set tecsIp                 "10.44.129.115" 
#文件映像部分的参数，参数请遵照字母、数字、下划线组合的方式命名
#映像文件名称，包含文件类型
set ImageFile             "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\centos5_hvm_20120202.img"
#映像名称，不包含文件类型
set ImageName            "centos5_hvm"
#映像类型global ImageType
set ImageType            "machine"
#映像支持的系统结构global ImageArchType
set ImageArchType        "x86_64"
#是否设置公有属性，TRUE：公有；FALSE：私有
global ImagePublicOrNot
set ImagePublicOrNot     "TRUE"
#是否设置映像可用，TRUE：可用；FALSE：不可用
global ImageAvailableOrNot
set ImageAvailableOrNot  "TRUE"
#映像描述信息，支持字母、数字、下划线组合，不支持中文、空格等
global ImageDescription
set ImageDescription     "linux_system_centos5"

############################################################################################################################
############################################################################################################################
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
#返回值：ID，失败返回 0
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
        retrun $g_imgID
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
    global ImageType
    global ImageArchType
    global ImagePublicOrNot
    global ImageAvailableOrNot
    global ImageDescription
    
    #获取映像文件名称
    regexp {\w+.img} $ImageFile StorName 
    
    #登录CLI
	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]

	if {[string equal $result ""] == 1} {
		set buffer [ZX_TelnetSend $tecsIp "?"]
	} else {
		return 0
	}
	#当前CLI所在目录：CLI#image
	ZX_TelnetSend $tecsIp "image"
	set SuccessOrNot "0"
	set buffer [ZX_TelnetSend $tecsIp "show create"]
	after 5000
	puts $buffer
	#puts $ImageName
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
	#puts $buffer
	after 5000
	#从show create中查询是否存在该imageid，若有则注册成功，否则返回注册失败
	regexp {image_id      =(\d+)} $buffer {} nGetImageId
	if {$nGetImageId != $nImageId} {
		ZX_Puts -critical "Failure: Fail to register the image(imageid: $nImageId).Please check the reason"
		return 0
	} else {
		ZX_Puts -normal "Success: Succeed to register the image(imageid: $nImageId)"
		return $nImageId
	}		
}

############################################################################################################################
#登录CLI进行映像注册
regImage $tecsIp $ImageName $ImageFile


