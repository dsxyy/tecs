#-------------------------------------------------
#�������ƣ�cloudInit {}
#�����������Զ�ע�ἯȺ��ע��HOST������SVCPU��
#���������tecsIp��		TC�����IP��ַ
#		   HCName�� 	HC���������
#          svcpuNum�� 	HC����svcpu��
#����ֵ��1 ���ɹ�����0��ʧ�ܣ�
#--------------------------------------------------
proc cloudInit { tecsIp HCName svcpuNum } {
    #Ĭ��CC��Ⱥ��
	set CCName "tecscluster"
	
	#�ֲ�������ʼ��
	set sClusterState        "NULL"
	set sHostState           "NULL"
	set nHostCpuId           "NULL"
	set nSvcpuNum            "0"
	set buffer               {}
	
	puts "��ʼ��ʼ������TC/CC/HC����������"
	#�Զ�������tecsIpϵͳ��TC���ڵġ�10.43.178.187����Telnet���ӣ��������Զ���¼���˴�Ĭ��Ϊadmin��¼
	#�ô���Ĭ�ϳ���3�����ӣ������쳣��������������
	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]

	if {[string equal $result ""] == 1} {
		set buffer [ZX_TelnetSend $tecsIp "?"]
	} else {
		return 0
	}
	#��ǰCLI����Ŀ¼��CLI#
	############################################################################################################################
	#��Ⱥע�ᵽTC
	#�Ƚ��뼯Ⱥ����
	ZX_TelnetSend $tecsIp "cluster_manager"

	#CLI�Ϸ���ע������
	ZX_TelnetSend $tecsIp "register $CCName"

	#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
	set buffer         {}

	#��ѯ�Ƿ�ע��ɹ�
	set buffer [ZX_TelnetSend $tecsIp "show 0"]
	regexp {register = (\w+)} $buffer {} sClusterState
	if {$sClusterState == "REGISTER"} {
		ZX_Puts -normal "Success: cc($CCName) has registered in tc"
	} else {
		ZX_Puts -critical "Failure: Fail to register cc($CCName).Please check the reason"
		return
	}
	#��ǰCLI����Ŀ¼��CLI(cluster_manager)#
	############################################################################################################################
	#����ע�ᵽCC
	#�˳�cluster_manager
	ZX_TelnetSend $tecsIp "exit"

	#����host_manager
	ZX_TelnetSend $tecsIp "host_manager"

	#�ֶ�����ǿ��host����ָ��CC
	ZX_TelnetSend $tecsIp "host register $CCName $HCName 0"

	after 1000
	#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
	set buffer         {}

	#���HC�Ƿ�ɹ�����CC
	set buffer [ZX_TelnetSend $tecsIp "show host $CCName $HCName"]
	after 2000
	regexp {register_state = (\w+)} $buffer {} sHostState
	if {$sHostState == "REGISTERED"} {
		ZX_Puts -normal "Success: host($HCName) has registered in cc($CCName)"
	} else {
		ZX_Puts -critical "Failure: Fail to register host($HCName) in cc($CCName).Please check the reason"
		return
	}
	#��ǰCLI����Ŀ¼��CLI(host_manager)#
	############################################################################################################################
	#����svcpu
	#���ȼ�����host_manager�»�ȡhost�� cpu_info_id
	regexp {cpu_info_id    = (\w+)} $buffer {} nHostCpuId


	#���⣺�˴���ʱ�޷���ȡ��nHostCpuId
	puts "The host cpu id is $nHostCpuId"
	#puts $buffer

	#�˳�host_manager��������cluster_config���˴���Ϊ���ܴ��ڲ�ѯ�б�϶࣬��Ҫ�������ո���ܻ�ȡ��Ч��Ϣ
	ZX_TelnetSend $tecsIp " "
	ZX_TelnetSend $tecsIp "exit"
	ZX_TelnetSend $tecsIp "cluster_config"

	#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
	set buffer         {}

	#Ϊ�˱�֤��ѯ��ȡ�����ݿ���д��buffer���˴�����ʱ5S����
	after 2000

	#����SVCPU
	ZX_TelnetSend $tecsIp "svcpu $CCName $nHostCpuId $svcpuNum"

	after 5000
	#����Ƿ�����SVCPU�ɹ�
	#���⣺�˴����Ͳ鿴����ʱ����ʱ�޷����ͳɹ���������ʱ����
	set buffer [ZX_TelnetSend $tecsIp "show svcpu $CCName $nHostCpuId"]

	#��ѯSVCPU�������Ƿ�����õ�һ�£��������ݼ��
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

#FTP�ϴ�����ȫ�ֱ���
#�ļ��ܴ�С
global g_TotalSize 
#�������ذٷֱ�
global g_LastDownloadPercent
#ӳ���ļ�ID
global g_imgID
set g_TotalSize 0
set g_LastDownloadPercent 0
set g_imgID 0
#��������:putslog {}��������Ϣ����
proc putslog {aa bb  cc} {
    global g_imgID
	puts "$bb"
	regexp "TC_FileStorageDB is : (\\d+)" $bb {} g_imgID
	if { $g_imgID != 0 } {
	    return $g_imgID
	}
}
#��������:DownloadProgressBar {}�����ؽ�����ʾ
proc DownloadProgressBar {downloadedsize} {
    global g_TotalSize
    global g_LastDownloadPercent
   if {$g_TotalSize > 0} {
       set percent  \
       [expr ($downloadedsize * 100) / $g_TotalSize]
       
       #���ٷֱ��б仯ʱ�Ŵ�ӡ
       if {$percent > 0 && $percent != $g_LastDownloadPercent} {
           set upsize [expr $downloadedsize / 1024 / 1024]
           set tolsize [expr $g_TotalSize / 1024 / 1024]
           puts "���ϴ����� $upsize M��\
           �����ܴ�С $tolsize M����� $percent %"
           
           set g_LastDownloadPercent $percent
       }
   } else {
       puts "���ϴ����� $downloadedsize bytes��"
   }
}
#��������:GetFtpFileSize {}�������ļ��Ĵ�С
proc GetFtpFileSize {filename} {
    global g_TotalSize
    set g_TotalSize [file size $filename]
    if {$g_TotalSize != ""} {      
        return $g_TotalSize   
    } else {
        ZX_Puts -warning "��ȡ�ļ���Сʧ�ܣ��޷��������ؽ��ȡ�"
        return 0
    }
}
#-------------------------------------------------
#��������:ImgPut {}
#�����������Զ��ϴ�image�ļ�
#���������tecsIp�� TC�����IP��ַ
#          ImageFile   ӳ������
#����ֵ��ID���ɹ�����0��ʧ�ܣ�
#--------------------------------------------------
proc ImgPut {tecsIp ImageFile} {
    global g_imgID   
    #FTP�ϴ�ӳ��TECS����
    set ::ftp::VERBOSE 1
    set buffer {}
	set handle [::ftp::Open $tecsIp admin admin \
							-blocksize 4096 -progress {DownloadProgressBar} -timeout 30 -output {putslog}]
	if {$handle == -1} {
    ZX_Puts -critical "Error:����FTP��������������FTP��ַ�����Ƿ���ȷ��"
    return 0
} else { 
#׼���ϴ�ӳ���ļ�      
    # ȡ��img�ļ���С
    set g_TotalSize [GetFtpFileSize $ImageFile]
    
    puts "��ʼ���������ϴ��ļ� $ImageFile ..."
    set result [::ftp::Put $handle $ImageFile]
    if {$result == 0} {
        ZX_Puts -critical "Error:FTP�ϴ��ļ�$ImageFile ���ִ���"
        return 0
    } else {
        puts "����ļ� $ImageFile ���ء�"
        puts "The FileID in TC_FileStorageDB is :$g_imgID"
        return $g_imgID
    }
}
::ftp::Close $handle
}
#-------------------------------------------------
#��������:regImage {tecsIp  ImageName ImageFile}
#�����������Զ��ϴ�image�ļ�
#���������tecsIp��TC�����IP��ַ
#		   ImageName��ע��ӳ����
#          ImageFile��ӳ���ļ�ȫ·��
#����ֵ��ID��ʧ�ܷ��� 0
#--------------------------------------------------
proc regImage {tecsIp ImageName ImageFile} {
    #ӳ������
	set ImageType            "machine"
	#ӳ��֧�ֵ�ϵͳ�ṹ
	set ImageArchType        "x86_64"
	#�Ƿ����ù������ԣ�TRUE�����У�FALSE��˽��
	set ImagePublicOrNot     "TRUE"
	#�Ƿ�����ӳ����ã�TRUE�����ã�FALSE��������
	set ImageAvailableOrNot  "TRUE"
	#ӳ��������Ϣ��֧����ĸ�����֡��»�����ϣ���֧�����ġ��ո��
	set ImageDescription     "hvm_system"
	
	#�ֲ�������ʼ��
	set nImageId             "0"
	set nGetImageId          "0"
	set SuccessOrNot 		 "0"
	
	puts "��ʼ�ϴ���ע��ӳ�񡣡���"    
    #��ȡӳ���ļ�����
    regexp {\w+.img} $ImageFile StorName 
    
    #��¼CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#��ǰCLI����Ŀ¼��CLI#image
	ZX_TelnetSend $tecsIp "image"
	set buffer [ZX_TelnetSend $tecsIp "show create"]
	after 5000
	puts $buffer
	#ֱ����buffer�в������õ�ӳ�������Ƿ���ڣ��������򷵻�1�����򷵻�0
	set SuccessOrNot [regexp "name(\\s+)=(\\s+)$ImageName" $buffer {}]
	#puts "SuccessOrNot is $SuccessOrNot"

	#�ж����õ�ӳ���Ƿ���Բ鵽
	if {$SuccessOrNot == "1"} {
		#������ӳ���Ѿ�ע����ϵͳ�У���ֱ�ӻ�ȡImageId
		ZX_Puts -normal "Warning: The Image($ImageName) has registered in tc,no need to register the image again."
		set buffer         {}
		set buffer [ZX_TelnetSend $tecsIp "show_file upload"]
		puts $buffer
		regexp "(\\d+)    $StorName" $buffer {} nImageId
		puts "nImageId get is $nImageId"
	
	} else {
		#������ӳ��δ��ϵͳ��ע�ᣬ�ټ���Ƿ��Ѿ��ϴ�
		set SuccessOrNot "0"
		set buffer [ZX_TelnetSend $tecsIp "show_file upload"]
		set SuccessOrNot [ regexp "(\\d+)    $StorName" $buffer {} nImageId ]
		puts "nImageId get is $nImageId"
		if {$SuccessOrNot == 1} {
			#����ӳ���ļ��Ѿ��ϴ��ˣ�ֱ�ӽ���ע��
			ZX_Puts -normal "Warning: The Image($StorName) has already exist in tc, just need to register it."
		} else {
			#����ӳ���ļ�û���ϴ�������ҪFTP�ϴ�
			ZX_Puts -normal "Warning: The Image($StorName) is not exist in tc.It will ftp the Image below,you need to wait for a minute."
			#FTP�ϴ�ӳ���ļ���TC_FileStorage
			set nImageId [ImgPut $tecsIp $ImageFile]
			puts "nImageId get is $nImageId"
		}
	}
	#У��˴��Ƿ�������ȡӳ���ļ��洢��ID
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
	#��show create�в�ѯ�Ƿ���ڸ�imageid��������ע��ɹ������򷵻�ע��ʧ��
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
#��������:newProj {tecsIp  ProjectName ImageFile}
#��������������������
#���������tecsIp��TC�����IP��ַ
#		   ProjectName����������
#          ProjectDes��������������Ϣ
#����ֵ��1 ���ɹ�����0��ʧ�ܣ�
#--------------------------------------------------proc newProj {tecsIp ProjectName ProjectDes} {
    #�ֲ�������ʼ��
    set SuccessOrNot "0"
    set buffer {}
    
    puts "��ʼ�½������顣����"   
    #��¼CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#���빤�������ý���
	ZX_TelnetSend $tecsIp "project"

	ZX_TelnetSend $tecsIp "create $ProjectName $ProjectDes"
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "show name $ProjectName"]

	#ֱ����buffer�в������õĹ����������Ƿ���ڣ��������򷵻�1�����򷵻�0
	set SuccessOrNot [regexp {[$ProjectName]} $buffer {}]
	#�ж����õĹ������Ƿ��������show�����Ƿ����óɹ�
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
#��������:createVM {tecsIp VmName ProjectName VmNum VmCpu VmMemory ImageId busType VmVirtType}
#���������������������
#���������tecsIp��TC�����IP��ַ
#		   VmName�����������
#          ProjectName����������#          VmNum�����������
#          VmCpu��vcpu����
#          VmMemory��������ڴ��С
#          ImageId��ӳ��ID
#          busType���洢����
#          VmVirtType�����������
#����ֵ��vm id ���ɹ�����0��ʧ�ܣ�
#--------------------------------------------------
proc createVM {tecsIp VmName ProjectName VmNum VmCpu VmMemory ImageId busType VmVirtType} {
	#�ֲ�������ʼ��
    set SuccessOrNot "0"
    set buffer {}
    
    puts "��ʼ���������������"   
    #��¼CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#���������
	ZX_TelnetSend $tecsIp "vm"
	set buffer [ZX_TelnetSend $tecsIp "create direct $VmName $ProjectName vm_num $VmNum $VmCpu $VmMemory machine $ImageId bus $busType virt_type $VmVirtType"]
	puts $buffer

	#���ö��VMʱ��ֻ��ȡ��һ��VM ID���в������
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
#��������:operateVM {tecsIp VmId  interval}
#��������������������
#���������tecsIp��TC�����IP��ַ
#		   VmId�������ID
#          interval���������
#����ֵ��1 ���ɹ�����0��ʧ�ܣ�
#--------------------------------------------------
proc operateVM {tecsIp VmId  interval} {
	#�ֲ�������ʼ��
    set SuccessOrNot "0"
    set buffer {}
    
    puts "��ʼ����Ͳ��������������"   
    #��¼CLI
#	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]
#
#	if {[string equal $result ""] == 1} {
#		set buffer [ZX_TelnetSend $tecsIp "?"]
#	} else {
#		return 0
#	}
	#�����������ֻ����֮ǰ��ȡ�ĵ�һ��VM
	ZX_TelnetSend $tecsIp "vm"
	set buffer [ZX_TelnetSend $tecsIp "show $VmId"]
	regexp {vm_name          = (\w+)} $buffer {} VmName
	
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "operate $VmId deploy"]

	#�����Ƿ�ɹ�ȷ��
	after 20000
	puts $buffer

	set SuccessOrNot [regexp {OK} $buffer {}]
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: have deploy the vm(VmName:$VmName VmId:$VmId)"
	} else {
		ZX_Puts -critical "Failure: fail to deploy the vm(VmName:$VmName  VmId:$VmId),please check the reason"
		return 0
	}
	#��ǰCLI����Ŀ¼��CLI(vm)#
	############################################################################################################################
	after $interval
	#ֹͣ�����
	set buffer {}
	set buffer [ZX_TelnetSend $tecsIp "operate $VmId stop"]
	#ֹͣ�Ƿ�ɹ�ȷ��
	after 20000
	puts $buffer

	set SuccessOrNot [regexp {OK} $buffer {}]
	if {$SuccessOrNot == "1"} {
		ZX_Puts -normal "Success: stop the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to stop the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #��ǰCLI����Ŀ¼��CLI(vm)#
    ############################################################################################################################
    #��ʱ30�룬�Ա�֤VM��������״̬ԾǨ
    after $interval
    #���������
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId start"]
    #�����Ƿ�ɹ�ȷ��
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: start the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to start the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #��ǰCLI����Ŀ¼��CLI(vm)#
    ############################################################################################################################
     after $interval
    #��ͣ�����
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId pause"]
    #��ͣ�Ƿ�ɹ�ȷ��
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: pause the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to pause the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #��ǰCLI����Ŀ¼��CLI(vm)#
    ############################################################################################################################    
    after $interval
    #�ָ������
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId resume"]
    #�ָ��Ƿ�ɹ�ȷ��
    after 20000
    puts $buffer
    
    set SuccessOrNot [regexp {OK} $buffer {}]	
    if {$SuccessOrNot == "1"} {
    	ZX_Puts -normal "Success: resume the vm(VmName:$VmName VmId:$VmId)"
    } else {
    	ZX_Puts -critical "Failure: fail to resume the vm(VmName:$VmName  VmId:$VmId),please check the reason"
    	return 0
    }
    #��ǰCLI����Ŀ¼��CLI(vm)#
    ############################################################################################################################
    after $interval
    #���������
    set buffer {}
    set buffer [ZX_TelnetSend $tecsIp "operate $VmId cancel"]
    #�����Ƿ�ɹ�ȷ��
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