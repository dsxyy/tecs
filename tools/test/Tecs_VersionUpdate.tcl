#-------------------------------------------------------------------------------------------------------------------------
# 文件名称：  Tecs_DailyUpdate.tcl                                                     
# 测试目的:   用于Tecs项目版本验证(版本升级)
# 作    者：  pengwei
# 完成日期：  2012/03/13 16:36:00
# 修改记录1： 修改历史记录，包括修改日期、修改者及修改内容

#TC所在物理机的IP地址
set tecs                 "10.44.129.115" 
#版本号
set VersionNO            "20120326"

############################################################################################################################
#版本自动FTP下载
puts "-------------Begin Tecs download------------"
proc VerDown {versionerIp versionNo} {
     #ssh连接linux机器
     set ssh [ZX_SSHConnect $versionerIp 22 root tecs123]
     puts $ssh
     
     set buffer {}
     set result "0"
     #调用版本下载自动化脚本
     set buffer [ZX_SSHSend $ssh "getVersion.sh $versionNo" 60]
     puts $buffer
     set result [regexp "Error!" $buffer]
     if {$result == 1} {
         puts "Error! Tecs version download failed!"
         exit
     }
     #检查版本是否下载成功
     set buffer [ZX_SSHSend $ssh "cd /home/tecs/$versionNo"]
     set buffer [ZX_SSHSend $ssh "ls -l |wc -l"]
     puts $buffer
     
     set result [regexp {(\d+)} $buffer match ]
     #puts $match
     if {$match < 2} {
        puts "Error! Tecs version update failed!"
		ZX_SSHDisconnect $ssh
		return 0
     } else {puts "Congratulations! Tecs version update successfully!"}
      ZX_SSHDisconnect $ssh
     return 1
}

set suc 1
for {set var 0} {$var <= 2} {incr var} {
    set result [VerDown $tecs $VersionNO]
    if { $result != 1} {
        continue    
    } else {
        set suc 0
        break
    }
}
if { $suc != 0 } {
	set result "连续三次尝试下载版本失败，请检查FTP服务器设置和状态！"
	ZX_Puts $result
	exit
} 

#版本自动安装
puts "-------------Begin Tecs install------------"
proc VerInstall {tecsIp versionNo} {
       #ssh连接linux机器
    set ssh [ZX_SSHConnect $tecsIp 22 root tecs123]
    puts $ssh
     
    set buffer {}
    set result "0"
   
    #检查TECS当前版本和安装版本是否一致，如果一致不需要升级
	ZX_SSHSend $ssh "cd /home/tecs/$versionNo"
	set buffer1 [ZX_SSHSend $ssh "ls -l"]
	set result [regexp {tecs-guard-+(\d+\.+[^a-z]+\d)} $buffer1 match fversion ]
	#puts $match
	puts "new version is: $fversion"
	 
	set buffer2 [ZX_SSHSend $ssh "rpm -qa |grep tecs"]
	set result [regexp {tecs-guard-+(\d+\.+[^a-z]+\d)} $buffer2 match eversion ]
	#puts "$match"
	
	if {$result == 1} {
	   puts "current version is: $eversion"
    
		if {$fversion == $eversion} {
			puts "Congratulations! tecs has been installed successfully!"
			ZX_SSHDisconnect $ssh
			return 1
		} 
	}
    
	#自动更新版本
	set buffer [ZX_SSHSend $ssh "tecs_start.sh" 600]
	puts $buffer
#	after 120000
    
#    set times 0
#	#更新完再后检查版本是否正确安装完成
#	while {$fversion != $eversion && $times < 6} {
#	    puts $times
#	    set buffer2 [ZX_SSHSend $ssh "rpm -qa |grep tecs"]
#		set result [regexp {tecs-guard-+(\d+\.+[^a-z]+\d)} $buffer2 match eversion ]
#		puts "2222222 $result"
#		puts $match
#		puts $eversion
#	    puts "Installing Tecs, wait for a moment!"
#	    after 10000
#	    incr times
#	}
#	if { $times == 6} {
#	    puts "Error! Install tecs failed!"
#	    ZX_SSHDisconnect $ssh
#	    return 0
#	}
#	puts "Congratulations! tecs has been installed successfully!"
#	ZX_SSHDisconnect $ssh
	return 1
}

set suc 1
for {set var 0} {$var <= 2} {incr var} {
    set result [VerInstall $tecs $VersionNO]
    if { $result != 1} {
        continue    
    } else {
        set suc 0
        break
    }
}
if { $suc != 0 } {
	set result "连续三次尝试升级版本失败，请检查新版本是否有问题！"
	ZX_Puts $result
	exit
} 


#TECS进程自动启动
#puts "-------------Begin Tecs start------------"
#proc VerStart {tecsIp} {
#    #ssh连接linux机器
#    set ssh [ZX_SSHConnect $tecsIp 22 root tecs123]
#    puts $ssh
#     
#    set buffer {}
#    set result "0"
#   
#    #检查TECS进程是否已经启动
#    ZX_SSHSend $ssh "ps -elf |grep X86"
#	set buffer [ZX_SSHSend $ssh "ps -elf |grep X86 | grep -v grep |wc -l"]
#	puts buffer
#	set result [regexp {([^X86]\d+)} $buffer match ]
#    #puts $match
#	puts "Now running tecs process num is: $match"
#    if {$match > 9} {
#        puts "Congratulations! All tecs processes are running now!"
#        ZX_SSHDisconnect $ssh
#        return 1
#    }
#	
#	#自动启动TECS所有进程 
#	set buffer [ZX_SSHSend $ssh "starttecs.sh"]
#	puts $buffer
#	after 1000
#    
#	ZX_SSHDisconnect $ssh
#	return 1
#}
#
#VerStart $tecs