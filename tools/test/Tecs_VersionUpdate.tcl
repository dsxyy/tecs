#-------------------------------------------------------------------------------------------------------------------------
# �ļ����ƣ�  Tecs_DailyUpdate.tcl                                                     
# ����Ŀ��:   ����Tecs��Ŀ�汾��֤(�汾����)
# ��    �ߣ�  pengwei
# ������ڣ�  2012/03/13 16:36:00
# �޸ļ�¼1�� �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����

#TC�����������IP��ַ
set tecs                 "10.44.129.115" 
#�汾��
set VersionNO            "20120326"

############################################################################################################################
#�汾�Զ�FTP����
puts "-------------Begin Tecs download------------"
proc VerDown {versionerIp versionNo} {
     #ssh����linux����
     set ssh [ZX_SSHConnect $versionerIp 22 root tecs123]
     puts $ssh
     
     set buffer {}
     set result "0"
     #���ð汾�����Զ����ű�
     set buffer [ZX_SSHSend $ssh "getVersion.sh $versionNo" 60]
     puts $buffer
     set result [regexp "Error!" $buffer]
     if {$result == 1} {
         puts "Error! Tecs version download failed!"
         exit
     }
     #���汾�Ƿ����سɹ�
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
	set result "�������γ������ذ汾ʧ�ܣ�����FTP���������ú�״̬��"
	ZX_Puts $result
	exit
} 

#�汾�Զ���װ
puts "-------------Begin Tecs install------------"
proc VerInstall {tecsIp versionNo} {
       #ssh����linux����
    set ssh [ZX_SSHConnect $tecsIp 22 root tecs123]
    puts $ssh
     
    set buffer {}
    set result "0"
   
    #���TECS��ǰ�汾�Ͱ�װ�汾�Ƿ�һ�£����һ�²���Ҫ����
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
    
	#�Զ����°汾
	set buffer [ZX_SSHSend $ssh "tecs_start.sh" 600]
	puts $buffer
#	after 120000
    
#    set times 0
#	#�������ٺ���汾�Ƿ���ȷ��װ���
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
	set result "�������γ��������汾ʧ�ܣ������°汾�Ƿ������⣡"
	ZX_Puts $result
	exit
} 


#TECS�����Զ�����
#puts "-------------Begin Tecs start------------"
#proc VerStart {tecsIp} {
#    #ssh����linux����
#    set ssh [ZX_SSHConnect $tecsIp 22 root tecs123]
#    puts $ssh
#     
#    set buffer {}
#    set result "0"
#   
#    #���TECS�����Ƿ��Ѿ�����
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
#	#�Զ�����TECS���н��� 
#	set buffer [ZX_SSHSend $ssh "starttecs.sh"]
#	puts $buffer
#	after 1000
#    
#	ZX_SSHDisconnect $ssh
#	return 1
#}
#
#VerStart $tecs