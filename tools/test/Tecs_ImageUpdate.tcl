#-------------------------------------------------
#�ļ�����:Tecs_ImageUpdate.tcl
#�����������Զ��ϴ�image�ļ���ע��
#���������tecsIp�� TC�����IP��ַ
#          FileName��   ӳ������
#����ֵ��ID
#�޸�����        �޸���        �޸�����
#2012/3/27       PENGWEI       ���� 
#--------------------------------------------------
#TC�����������IP��ַ
set tecsIp                 "10.44.129.115" 
#�ļ�ӳ�񲿷ֵĲ�����������������ĸ�����֡��»�����ϵķ�ʽ����
#ӳ���ļ����ƣ������ļ�����
set ImageFile             "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\centos5_hvm_20120202.img"
#ӳ�����ƣ��������ļ�����
set ImageName            "centos5_hvm"
#ӳ������global ImageType
set ImageType            "machine"
#ӳ��֧�ֵ�ϵͳ�ṹglobal ImageArchType
set ImageArchType        "x86_64"
#�Ƿ����ù������ԣ�TRUE�����У�FALSE��˽��
global ImagePublicOrNot
set ImagePublicOrNot     "TRUE"
#�Ƿ�����ӳ����ã�TRUE�����ã�FALSE��������
global ImageAvailableOrNot
set ImageAvailableOrNot  "TRUE"
#ӳ��������Ϣ��֧����ĸ�����֡��»�����ϣ���֧�����ġ��ո��
global ImageDescription
set ImageDescription     "linux_system_centos5"

############################################################################################################################
############################################################################################################################
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
#����ֵ��ID��ʧ�ܷ��� 0
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
        retrun $g_imgID
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
    global ImageType
    global ImageArchType
    global ImagePublicOrNot
    global ImageAvailableOrNot
    global ImageDescription
    
    #��ȡӳ���ļ�����
    regexp {\w+.img} $ImageFile StorName 
    
    #��¼CLI
	set result [ZX_LoginHost $tecsIp 24 "Username:" "admin" "Password:" "admin" "CLI#"]

	if {[string equal $result ""] == 1} {
		set buffer [ZX_TelnetSend $tecsIp "?"]
	} else {
		return 0
	}
	#��ǰCLI����Ŀ¼��CLI#image
	ZX_TelnetSend $tecsIp "image"
	set SuccessOrNot "0"
	set buffer [ZX_TelnetSend $tecsIp "show create"]
	after 5000
	puts $buffer
	#puts $ImageName
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
	#puts $buffer
	after 5000
	#��show create�в�ѯ�Ƿ���ڸ�imageid��������ע��ɹ������򷵻�ע��ʧ��
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
#��¼CLI����ӳ��ע��
regImage $tecsIp $ImageName $ImageFile


