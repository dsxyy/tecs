#-------------------------------------------------------------------------------------------------------------------------
# �ļ����ƣ�  Tecs_VersionCheck.tcl                                                     
# ����Ŀ��:   ����Tecs��Ŀ�汾��֤(Ĭ��ʹ��admin�û�)
# ��    �ߣ�  ����
# ������ڣ�  2012/03/08 16:36:00
# �޸ļ�¼1�� 
# �޸����ڣ�  2012/03/28
# �� �� �ˣ�  ��ΰ
# �޸����ݣ�  ��װ�������ù���Ϊ������������ã���������centos5��winxp������Ĳ���
#------------------------------------------------------------------------------------------------------------------------
############################################################################################################################
source "[file dirname [info script]]/Tecs_lib.tcl"

#�ű�ִ��ǰ��Ҫ�޸ĵ����������TC���ڵ�IP��ַ��CC���ơ�HC������������ơ�svcpu������
#TC�����������IP��ַ
set tecsIp               "10.44.129.115" 
#��Ҫ����ע���CC������
set CCName               "tecscluster"
#HC�������ڵ����������
set HCName               "Pwvir01"

#CC����HC��SVCPU������(0-2147483647)
set svcpuNum             "1000"

#�ļ�ӳ�񲿷ֵĲ�����������������ĸ�����֡��»�����ϵķ�ʽ����
#ӳ���ļ����ƣ������ļ�����
set tty_lin01            "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\ttylinux0.img"
set centos_lin02         "F:\\TecsDoc\\zxve\\Tecs_Images\\images4linux\\centos5_hvm_20120202.img"
set win_xp03             "F:\\TecsDoc\\zxve\\Tecs_Images\\images4windows\\windowsxp.img"
#ӳ�����ƣ��������ļ�����
set tty_img01            "ttylinux0"
set centos_img02         "centos5_img"
set winxp_img03          "winxp2012_img"
#ӳ������
#set ImageType            "machine"
#ӳ��֧�ֵ�ϵͳ�ṹ
#set ImageArchType        "x86_64"
#�Ƿ����ù������ԣ�TRUE�����У�FALSE��˽��
#set ImagePublicOrNot     "TRUE"
#�Ƿ�����ӳ����ã�TRUE�����ã�FALSE��������
#set ImageAvailableOrNot  "TRUE"
#ӳ��������Ϣ��֧����ĸ�����֡��»�����ϣ���֧�����ġ��ո��
#set ImageDescription     "hvm_system"

#������������ò�������
#����VM���ڵĹ������ƣ�֧����ĸ�����֡��»������
set Proj01                "default"
set Proj02                "linux_group"
#����������Ϣ��֧����Ļ�����֡��»������
set ProjDes01             "default_project"
set ProjDes02             "linux_system_workgroup"  

#�����������ز�������
#���������
set ttyvm01               "tty_linux01"
set centosvm02            "centos5"
set winxpvm03             "winxp2012"
#��Ҫ���������������
set VmNum01                "2"
set VmNum02                "1"
set VmNum03                "1"
#ÿ��������������������CPU������
set VmCpu01                "4"
set VmCpu02                "2"
set VmCpu03                "1"
#�����ڴ��С����λM��ȡֵ��Χ��1~17592186044416
set VmMemory01             "50"
set VmMemory02             "1024"
set VmMemory03             "2048"
#�������ͣ�����hvm��pvm��ѡ�������������Ҫ�������ص�ӳ���������һ��
set VmbusType              "ide"
set VmVirtType             "hvm"

#���ϲ�����Ҫ�ڽű�ִ��ǰ�ֶ�������ز���������Ϊ��֤����������ȷ������β���У��

############################################################################################################################
############################################################################################################################
#��¼CLI����CCע�ᣬHCע�������SVCPUcloudInit $tecsIp $HCName $svcpuNum

############################################################################################################################
after 2000
#��¼CLI����ӳ��ע��
#ע��ttylinux0.img��������IDset ttyimgid [regImage $tecsIp $tty_img01 $tty_lin01]
puts "tty linux image id is: $ttyimgid."
after 2000
set cetnosimgid [regImage $tecsIp $centos_img02 $centos_lin02]
puts "centos5 image id is: $cetnosimgid."
after 2000
set winxpimgid [regImage $tecsIp $winxp_img03 $win_xp03]
puts "winxp image id is: $winxpimgid."
############################################################################################################################
after 2000
#��¼CLI���й����鴴��
#������һ��������defaultnewProj $tecsIp $Proj01 $ProjDes01
after 2000
newProj $tecsIp $Proj02 $ProjDes02
############################################################################################################################
after 2000
#��¼CLI�������������
#����2��tty_linux��������������ص�һ�������ID
set ttyvmid [createVM $tecsIp $ttyvm01 $Proj01 $VmNum01 $VmCpu01 $VmMemory01 $ttyimgid $VmbusType $VmVirtType]
after 2000
set centosvmid [createVM $tecsIp $centosvm02 $Proj02 $VmNum02 $VmCpu02 $VmMemory02 $cetnosimgid $VmbusType $VmVirtType]
after 2000
set winxpvmid [createVM $tecsIp $winxpvm03 $Proj02 $VmNum03 $VmCpu03 $VmMemory03 $winxpimgid $VmbusType $VmVirtType]
############################################################################################################################
after 2000
#��¼CLI�������������Ͳ���
#����Ͳ���tty linux�����,�������Ϊ10s
operateVM $tecsIp $ttyvmid 10000
after 2000
#����Ͳ���centos linux�����,�������Ϊ30s
operateVM $tecsIp $centosvmid 30000
after 2000
#����Ͳ���windows xp�����,�������Ϊ180s
operateVM $tecsIp $winxpvmid 180000
after 10000
#��ǰCLI����Ŀ¼��CLI(vm)#
############################################################################################################################
#����ƽ��

#DHCP

#��������















