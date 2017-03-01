#-------------------------------------------------------------------------------------------------------------------------
# �ļ����ƣ�  Tecs_DailyCheck.tcl                                                     
# ����Ŀ��:   ����Tecs��Ŀ����֤(Ĭ��ʹ��admin�û�)
# ��    �ߣ�  ����
# ������ڣ�  2012/03/08 16:36:00
# �޸ļ�¼1�� �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
# �޸����ڣ�
# �� �� �ˣ�
# �޸����ݣ� 
# �޸ļ�¼2����
#------------------------------------------------------------------------------------------------------------------------
#��Ҫ���Ӱ汾ж���Զ����ű�

#��Ҫ���Ӱ汾��װ�Զ����ű�

#��Ҫ����TECS����������ؽű�

############################################################################################################################
#�ű�ִ��ǰ��Ҫ�޸ĵ����������TC���ڵ�IP��ַ��CC���ơ�HC������������ơ�svcpu������
#TC�����������IP��ַ
set tecs                 "10.43.178.187" 
#��Ҫ����ע���CC������
set CCName               "cc_0305"
#HC�������ڵ����������
set HCName               "shenyi_host"
#CC���õ�SVCPU������(0-256)
set CCsvcpuNum           "100"
#����ӳ���ļ�����Ŀ¼��������ô��̸�Ŀ¼���ݲ�֧������·��
set LocalDir             "I:\\07test_tty.img"

#�ļ�ӳ�񲿷ֵĲ�����������������ĸ�����֡��»�����ϵķ�ʽ����
#ӳ���ļ����ƣ������ļ�����
set FileName             "07test_tty.img"
#ӳ�����ƣ��������ļ�����
set ImageName            "07test_tty"
#ӳ������
set ImageType            "machine"
#ӳ��֧�ֵ�ϵͳ�ṹ
set ImageArchType        "x86_64"
#�Ƿ����ù������ԣ�TRUE�����У�FALSE��˽��
set ImagePublicOrNot     "TRUE"
#�Ƿ�����ӳ����ã�TRUE�����ã�FALSE��������
set ImageAvailableOrNot  "TRUE"
#ӳ��������Ϣ��֧����ĸ�����֡��»�����ϣ���֧�����ġ��ո��
set ImageDescription     "for_testing"

#������������ò�������
#����VM���ڵĹ������ƣ�֧����ĸ�����֡��»������
set ProjectName          "pj_03"
#����������Ϣ��֧����Ļ�����֡��»������
set ProjectDes           "project_des"

#�����������ز�������
#���������
set VmName               "vm_03"
#��Ҫ���������������
set VmNum                "2"
#ÿ��������������������CPU������
set VmCpu                "3"
#�����ڴ��С����λM��ȡֵ��Χ��1~17592186044416
set VmMemory             "50"
#�������ͣ�����hvm��pvm��ѡ�������������Ҫ�������ص�ӳ���������һ��
set VmVirtType           "hvm"

#���ϲ�����Ҫ�ڽű�ִ��ǰ�ֶ�������ز���������Ϊ��֤����������ȷ������β���У��
############################################################################################################################
#�ֲ�������ʼ��
set sClusterState        "NULL"
set sHostState           "NULL"
set nHostCpuId           "NULL"
set nSvcpuNum            "0"
set buffer               {}
set nImageId             "0"
set nGetImageId          "0"
set SuccessOrNot         "0"
#�ڲ����ڵ�ǰSSH���ӵ�ID
set a                    "0"
############################################################################################################################
#�Զ�������Tecsϵͳ��TC���ڵġ�10.43.178.187����Telnet���ӣ��������Զ���¼���˴�Ĭ��Ϊadmin��¼
#�ô���Ĭ�ϳ���3�����ӣ������쳣��������������
set result [ZX_LoginHost $tecs 24 "Username:" "admin" "Password:" "admin" "CLI#"]

if {[string equal $result ""] == 1} {
	set buffer [ZX_TelnetSend $tecs "?"]
} else {
	return [puts $result]
}
#��ǰCLI����Ŀ¼��CLI#
############################################################################################################################
#��Ⱥע�ᵽTC
#�Ƚ��뼯Ⱥ����
ZX_TelnetSend $tecs "cluster_manager"

#CLI�Ϸ���ע������
ZX_TelnetSend $tecs "register $CCName"

#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
set buffer         {}

#��ѯ�Ƿ�ע��ɹ�
set buffer [ZX_TelnetSend $tecs "show 0"]
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
ZX_TelnetSend $tecs "exit"

#����host_manager
ZX_TelnetSend $tecs "host_manager"

#�ֶ�����ǿ��host����ָ��CC
ZX_TelnetSend $tecs "host register $CCName $HCName 0"

after 1000
#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
set buffer         {}

#���HC�Ƿ�ɹ�����CC
set buffer [ZX_TelnetSend $tecs "show host $CCName $HCName"]
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
ZX_TelnetSend $tecs " "
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "cluster_config"

#ÿ�ν���ѯ�������buffer֮ǰ�����buffer
set buffer         {}

#Ϊ�˱�֤��ѯ��ȡ�����ݿ���д��buffer���˴�����ʱ5S����
after 2000

#����SVCPU
ZX_TelnetSend $tecs "svcpu $CCName $nHostCpuId $CCsvcpuNum"

after 5000
#����Ƿ�����SVCPU�ɹ�
#���⣺�˴����Ͳ鿴����ʱ����ʱ�޷����ͳɹ���������ʱ����
set buffer [ZX_TelnetSend $tecs "show svcpu $CCName $nHostCpuId"]

#��ѯSVCPU�������Ƿ�����õ�һ�£��������ݼ��
regexp {svcpu_num      = (\d+)} $buffer {} nSvcpuNum
if {$nSvcpuNum == $CCsvcpuNum} {
	ZX_Puts -normal "Success: have cfg the svcpu($CCsvcpuNum) of CC($CCName)"
} else {
	ZX_Puts -critical "Failure: need to recfg svcpu($CCsvcpuNum) of CC($CCName) manually.Please check the reason"
	return
}

after 5000
ZX_TelnetSend $tecs "exit"
#��ǰCLI����Ŀ¼��CLI#
############################################################################################################################
after 5000
#�ϴ�ӳ���ļ���rootĿ¼
#���⣺ӳ���ϴ��޷�ͨ��CLI������ʵ�֣�����ͨ��FTP�ķ�ʽ�ϴ�ӳ���ļ�
#���ϴ�ӳ���ļ���linux�����ı���Ŀ¼/root
set a [ZX_SSHConnect $tecs 22 root tecs123]
puts $a

set buffer {}
set SuccessOrNot "0"
ZX_SSHSend $a "cd /root"
set buffer [ZX_SSHSend $a "ll"]
puts $buffer
puts $FileName
puts "SuccessOrNot is $SuccessOrNot"
#ֱ����buffer�в������õ��ļ������Ƿ���ڣ��������򷵻�1�����򷵻�0
#set SuccessOrNot [regexp -all {[$FileName]} $buffer {}]set SuccessOrNot [regexp "$FileName" $buffer {}]
after 2000
puts "SuccessOrNot is $SuccessOrNot"
#�ж����õ��ļ��Ƿ���������鵽
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
#��ѯȷ���Ƿ���ҪFTP�ļ�����û��ע�ᣬ����FTP�ļ�����ע��ӳ�����Ѿ�ע�ᣬ��ֱ�ӻ�ȡImageId.set buffer {}
set SuccessOrNot "0"
ZX_TelnetSend $tecs "image"
set buffer [ZX_TelnetSend $tecs "show create"]
after 5000
puts $buffer
puts $ImageName
puts "SuccessOrNot is $SuccessOrNot"
#ֱ����buffer�в������õ�ӳ�������Ƿ���ڣ��������򷵻�1�����򷵻�0
set SuccessOrNot [regexp "name(\\s+)=(\\s+)$ImageName" $buffer {}]
puts "SuccessOrNot is $SuccessOrNot"

#�ж����õ�ӳ���Ƿ���Բ鵽
if {$SuccessOrNot == "1"} {
    #������ӳ���Ѿ�ע����ϵͳ�У���ֱ�ӻ�ȡImageId
	ZX_Puts -normal "Warning: The Image($ImageName) has registered in tc,no need to ftp the image again."
	set buffer         {}
	set buffer [ZX_TelnetSend $tecs "show_file upload"]
	puts $buffer
	puts $ImageName
	regexp "(\\d+)    $ImageName" $buffer {} nImageId
	puts "nImageId get is $nImageId"
	
} else {
    #������ӳ��δ��ϵͳ��ע�ᣬ����FTP��ӳ���ٽ���ע��
	ZX_Puts -normal "Warning: The Image($ImageName) is not exist in tc.It will ftp the Image below,you need to wait for a minute."
	#ͨ��FTP��ӳ���ϴ�����Ӧ��Ŀ¼
	#���Ͷ�Ӧ��FTP��IP
	set b [ZX_SSHConnect $tecs 22 root tecs123]
	set r1 [ZX_SSHSend $b "ftp $tecs" 20 "Name ($tecs:root): "]

	#���Ͷ�Ӧ��FTP���û�����Ĭ����admin
	set r2 [ZX_SSHSend $b "admin" 20 "Password:"]

	#���Ͷ�Ӧ��FTP�����룬Ĭ����admin
	set r3 [ZX_SSHSend $b "admin" 20 "ftp>"]

	#У���Ƿ�FTP��¼�ɹ����˴����ԣ�����ͨ������pwd����ƥ�����Ƿ���/mnt/tecs_tc/1����֤
	#
	#ֱ�ӽ�ӳ���ϴ�
	set r4 [ZX_SSHSend $b "put $FileName" 600 "ftp>"]
#	puts $r4

	#У��˴��Ƿ�������ȡ�ļ��洢��ID
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
	#��show create�в�ѯ�Ƿ���ڸ�imageid��������ע��ɹ������򷵻�ע��ʧ��
	regexp {image_id      =(\d+)} $buffer {} nGetImageId
	if {$nGetImageId != $nImageId} {
		ZX_Puts -critical "Failure: Fail to register the image(imageid: $nImageId).Please check the reason"
		return
	} else {
		ZX_Puts -normal "Success: Succeed to register the image(imageid: $nImageId)"
	}	
	
}
#��ǰCLI����Ŀ¼��CLI(image)#
############################################################################################################################
#���빤�������ý���
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "project"

ZX_TelnetSend $tecs "create $ProjectName $ProjectDes"
set buffer {}
set buffer [ZX_TelnetSend $tecs "show name $ProjectName"]

#ֱ����buffer�в������õĹ����������Ƿ���ڣ��������򷵻�1�����򷵻�0
set SuccessOrNot [regexp {[$ProjectName]} $buffer {}]
#�ж����õĹ������Ƿ��������show�����Ƿ����óɹ�
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have cfg the projcet($ProjectName)"
} else {
	ZX_Puts -critical "Failure: need to recfg the project($ProjectName) manually.Please check the reason"
	return
}
#��ǰCLI����Ŀ¼��CLI(pro_cfg)#
############################################################################################################################

#���������
ZX_TelnetSend $tecs "exit"
ZX_TelnetSend $tecs "vm"

set buffer {}
set buffer [ZX_TelnetSend $tecs "create direct $VmName $ProjectName vm_num $VmNum $VmCpu $VmMemory machine $nImageId bus ide virt_type $VmVirtType"]
puts $buffer

#���ö��VMʱ��ֻ��ȡ��һ��VM ID���в������
regexp {The ID are:\n(\d+)\s+(\d+)} $buffer {} VmId
puts $VmId

set SuccessOrNot [regexp {Success create} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have cfg the vm($VmName)"
} else {
	ZX_Puts -critical "Failure: need to recfg the vm($VmName) manually.Please check the reason"
	return
}
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#�����������ֻ����֮ǰ��ȡ�ĵ�һ��VM
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId deploy"]

#�����Ƿ�ɹ�ȷ��
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: have deploy the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to deploy the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
after 10000
#ֹͣ�����
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId stop"]
#ֹͣ�Ƿ�ɹ�ȷ��
after 20000
puts $buffer

set SuccessOrNot [regexp {OK} $buffer {}]
if {$SuccessOrNot == "1"} {
	ZX_Puts -normal "Success: stop the vm(VmName:$VmName VmId:$VmId)"
} else {
	ZX_Puts -critical "Failure: fail to stop the vm(VmName:$VmName  VmId:$VmId),please check the reason"
	return
}
#��ʱ30�룬�Ա�֤VM��������״̬ԾǨ
after 30000
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#���������
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId start"]
#�����Ƿ�ɹ�ȷ��
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
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#��ͣ�����
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId pause"]
#��ͣ�Ƿ�ɹ�ȷ��
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
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#�ָ������
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId resume"]
#�ָ��Ƿ�ɹ�ȷ��
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
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#���������
set buffer {}
set buffer [ZX_TelnetSend $tecs "operate $VmId cancel"]
#�����Ƿ�ɹ�ȷ��
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
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#����ƽ��

#DHCP

#��������















