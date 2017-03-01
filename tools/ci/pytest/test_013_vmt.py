#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_013_vmt.py
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
# �޸ļ�¼2��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ����611003050917 ģ������д������޸Ŀ��Ź�ʱ���޸Ĳ��ɹ���ʼ����0��
# �޸ļ�¼3��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
# �޸ļ�¼4��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611002997307 ����ģ��ʱ��������Ƴ�ͻ�������ǲ������󣬲�ֱ��
# �޸ļ�¼5��
#    �޸����ڣ�2012/07/19
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�#EC611003017919 ����16��Ӳ�̵�vm�޷�������ʾSchedule failed to allocate vid
#*******************************************************************************/
import sys,os,getopt,time,inspect
import hashlib
import unittest 
import string
from contrib import xmlrpclibex
from ftplib import FTP

from tecs.user import *
from tecs.usergroup import *
from tecs.clustermanager import *
from tecs.host import *
from tecs.tcu import *
from tecs.image import *
from tecs.vmcfg import *
from tecs.project import *
from tecs.file import *
from tecs.vmtemplate import *
from tecs.tecssystem import *
from tecs.ssh_cmd import *


#���ڲ��Ե�tc xmlrpc��������ַ
server_addr = "localhost"
#���ڲ��Ե�tc xmlrpc�������˿ں�
server_port = "8080"
ssh_port = "22"
# tc/cc/hc��������Ҫ��start.sh�еĶ��屣��һ�£�
#���ڲ��Ե�tc application����������
test_cloud = "tecscloud"
#���ڲ��Ե�cc application������Ⱥ����
test_cluster = "tecscluster"
#���ڲ��Ե�hc application�������������
test_host = "citesthost"
#����Ա�˺ź�����
test_root_user = "admin"
test_root_passwd = "admin"
#��ͨ�û�������
test_tenant_usergroup = "cigroup"
#�⻧�˺ź�����
test_tenant_user = "citest"
test_tenant_passwd = "citest"
#�����ϴ��������������ӳ���ļ�·��
test_image = "ttylinux.img"
#�����Ե����������
test_vm_name1 = "ttylinux1"
test_vm_name2 = "ttylinux2"
test_vm_name3 = "ttylinux3"
test_vt_name = "ttylinux_vt"
#�����ԵĹ�������
test_project_name = "test_prj_1"


def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -p,--port, the xmlrpc server port"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)

def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)

# �������������в���
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:p:i:v", ["help", "addr=","port=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,"GetoptError")

for o, v in opts:
    #��-h��--help��ӡ����
    if o in ("-h", "--help"):
        usage()
    #��-a��--addr����ָ��tc��xmlrpc��������ַ��Ĭ��Ϊlocalhost
    if o in ("-a", "--addr"):
        server_addr = v
    #��-p��--port����ָ��tc��xmlrpc�������˿ںţ�Ĭ��Ϊ8080
    if o in ("-p", "--port"):
        server_port = v
    #��-i��--image����ָ����������õ�ӳ���ļ�
    if o in ("-i", "--image"):
        test_image = v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)

# �Զ������ɲ��Կ�ʼ!!!
print "===================TECS CI TEST START==================="
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url,allow_none=True)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�
server.system.listMethods()

class VmtTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testVmt(self):
        # ------------------����׼��------------------
        # ��¼tecs����ȡ����Ա�˻����ܺ��session
        root_session = login(server,test_root_user,test_root_passwd)
        self.assertNotEqual(root_session,None,"failed to get encrypted root session!")

        #ϵͳ����״̬��ѯ EC:611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
        #�˴���ʱ20s������hc��MU_VM_HANDLER��MU_HOST_HEARTBEAT������
        time.sleep(100)
        #�����ǹ���Ա�˻�
        sys_runtime_query = tecssystem(server,root_session)
        #��ѯTC״̬
        self.assertEqual(int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,DEFAULT_TC)),RUNTIME_STATE_NORMAL,"tc status is abnormal")
        #��ѯCC״̬
        self.assertEqual(int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)),RUNTIME_STATE_NORMAL,"cc status is abnormal")
        #��ѯHC״̬
        test_cc_hc = test_cluster + "," + test_host
        hc_state = int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cc_hc))
        self.assertTrue(hc_state >= RUNTIME_STATE_NORMAL and hc_state <= RUNTIME_STATE_WARNNING,"hc status is abnormal")

        # ����һ���û���ʹ�ò���Ϊ test_group1,1 ,Ӧ�óɹ�
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))

        # ����һ���û� ����Ϊ user,test,test,3,test_group1  Ӧ�óɹ�
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # ��¼�⻧  ��ȡ�⻧session �Ա�����ʹ��
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session, None,"failed to get user session!")
        
        # �Թ���Ա����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�        
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # ���⻧����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
      
      
        # ע�ἯȺ
        root_cluster_manager_method = clustermanager(server,root_session) 

        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # ע������
        root_host_method = host(server,root_session)
        # �ȴ� 20S �ȴ���Դ�ϱ�
        #time.sleep(20) 

        self.assertTrue(root_host_method.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))


        # ����tcu 8��
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id, None)
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))
        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,NO_CPU_INFO,cpu_id))

        # ע��image
        # �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
        i = image(server,root_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id, None, "failed to get image id!")
        
        # ��������
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
        
        # ------------------����Ա����Ȩ�޲���------------------
        # ����ģ�����
        # ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ�óɹ�
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        #cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext2"}]

        vt = vmtemplate(server,root_session)
        self.assertFalse(vt.allocate(cfg))
        # ����image
        #Vmtemplate cannot public, for Image( -1 ) not public
        self.assertTrue(i.publish(image_id,IS_PUBLIC))
           
        # ����ģ�壺   Ӧ�óɹ� 
        self.assertTrue(vt.allocate(cfg))
        #EC611002997307 ����ģ��ʱ��������Ƴ�ͻ�������ǲ������󣬲�ֱ��
        #������ͬ���Ƶ�ģ�壬���ش������
        self.assertFalse(vt.allocate(cfg))
        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))
        # ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ�óɹ� 
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        self.assertTrue(vt.allocate(cfg))
        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))
        # ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ�óɹ� 
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        self.assertTrue(vt.allocate(cfg))
        # ��������  ��������    Ӧ���ɹ� 
        user_i = image(server,user_session)
        user_image_id = user_i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(user_image_id, None, "failed to get image id!")

        self.assertTrue(i.publish(user_image_id,IS_PUBLIC))
        
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 4
        cfg["base_info"]["memory"] = long(256 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["context"] = {}
        cfg["base_info"]["qualifications"] = []
        #cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"xvdb","position":0,"fstype":"ext2"}]
        disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        # test_vt_name �� disks = [disk1]
        cfg["base_info"]["disks"] = [disk1]
        self.assertTrue(vt.set(cfg))
        # ��������  ��������    Ӧ���ɹ�
        disk2 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext4"}
        # test_vt_name �� disks = [disk2]
        cfg["base_info"]["disks"] = [disk2]
        self.assertTrue(vt.set(cfg))
        # ��������  ��������    Ӧ���ɹ�
        disk3 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdd","position":2,"fstype":"ext4"}
        # test_vt_name �� disks = [disk3]
        cfg["base_info"]["disks"] = [disk3]
        self.assertTrue(vt.set(cfg))
        # ����image disk    Ӧ���ɹ�,ramdisk��ʱû��ʹ�ã�������
        disk4 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"datablock","target":"sde"}
        #if vt.set(cfg) != True:
        #    ci_exit_ex(-1,line())
        # ����image disk    Ӧ���ɹ�,ramdisk��ʱû��ʹ�ã�������
        disk5 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"cdrom","target":"sdf"}
        #if vt.set(cfg) != True:
        #    ci_exit_ex(-1,line())
        # ����image disk    Ӧ��ʧ�ܣ�,ramdisk��ʱû��ʹ�ã�������
        disk6 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdf"}
        #if vt.set(cfg) != False:
        #    ci_exit_ex(-1,line())
        # ��������  ��������    Ӧ��ʧ�ܣ�
        disk7 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdd","position":2,"fstype":"fat32"}
        # test_vt_name �� disks = [disk7]
        cfg["base_info"]["disks"] = [disk7]
        self.assertFalse(vt.set(cfg))
        # ��������  ��������    Ӧ���ɹ���
        disk8 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"cdrom","target":"sdd","position":2,"fstype":"ext4"}
        # test_vt_name �� disks = [disk8]
        cfg["base_info"]["disks"] = [disk8]
        self.assertTrue(vt.set(cfg))
        # ��������  ��������    Ӧ���ɹ�
        disk9 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdg","position":2,"fstype":"ext2"}
        # test_vt_name �� disks = [disk9]
        cfg["base_info"]["disks"] = [disk9]
        self.assertTrue(vt.set(cfg))
        # ��������  ��������    Ӧ���ɹ�
        # test_vt_name �� disks = [disk1 disk2 disk3 disk4 disk9]
        cfg["base_info"]["disks"] = [disk1,disk2,disk3,disk9]
        self.assertTrue(vt.set(cfg))
        # ��������  ��������    Ӧ��ʧ��
        # test_vt_name �� disks = [disk1 disk1]
        cfg["base_info"]["disks"] = [disk1,disk1]
        self.assertFalse(vt.set(cfg))
        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))

        self.assertTrue(i.publish(user_image_id,NOT_PUBLIC))

        # �������� ��������    Ӧ���ɹ�
        #EC611003123662 TC���ݿ����� gaom 2012-07-30 ����nic_index�ֶ�����
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 4
        cfg["base_info"]["memory"] = long(256 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["context"] = {}
        cfg["base_info"]["qualifications"] = []
        nic1 = {"nic_index":0,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        cfg["base_info"]["nics"] = [nic1]
        #����ģ�壬Ӧ�óɹ�
        self.assertTrue(vt.allocate(cfg))
        nic_index = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_FIRST_NIC)
        self.assertNotEqual(nic_index, 0)
        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))
        #����������ͬ��nic_index������ģ�壬Ӧ��ʧ��
        nic1 = {"nic_index":1,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        nic2 = {"nic_index":1,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        cfg["base_info"]["nics"] =[nic1,nic2]
        self.assertFalse(vt.allocate(cfg))
        #����������ͬ��nic_index������ģ�壬Ӧ�óɹ�
        nic1 = {"nic_index":0,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        nic2 = {"nic_index":2,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        cfg["base_info"]["nics"] =[nic1,nic2]
        self.assertTrue(vt.allocate(cfg))
        #��ѯ����������index��Ӧ�ò�ͬ    
        #nic_index0 = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_FIRST_NIC)
        #nic_index1 = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_SECOND_NIC)
        #self.assertNotEqual(nic_index0, nic_index1)
        #ɾ��ģ�� �ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))
         
        #���EC611003050917��ģ������д������޸Ŀ��Ź�ʱ���޸Ĳ��ɹ���ʼ����0
        # �޸�vm_wdtimeΪ0    Ӧ��ʧ��
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [disk1]
        cfg["base_info"]["vm_wdtime"] = 0
        self.assertTrue(vt.allocate(cfg) )
        # ��ѯvm_wdtime��   Ӧ��Ϊ0
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name),0)

        # �޸�vm_wdtimeΪ1    Ӧ��ʧ��
        cfg["base_info"]["vm_wdtime"] = 1
        self.assertFalse(vt.set(cfg))

        # �޸�vm_wdtimeΪ599    Ӧ��ʧ��
        cfg["base_info"]["vm_wdtime"] = 599
        self.assertFalse(vt.set(cfg))

        # �޸�vm_wdtimeΪ600    Ӧ�óɹ�
        cfg["base_info"]["vm_wdtime"] = 600
        self.assertTrue(vt.set(cfg))
        # ��ѯvm_wdtime��   Ӧ��Ϊ600
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name),600)

        # �޸�vm_wdtimeΪ999999    Ӧ�óɹ�
        cfg["base_info"]["vm_wdtime"] = 999999
        self.assertTrue(vt.set(cfg))
        # ��ѯvm_wdtime��   Ӧ��Ϊ999999
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name), 999999)

        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ�
        self.assertTrue(vt.del_vmt(test_vt_name))

        # ����ӳ��2������ΪDatablock��ʹ�ܣ� Ӧ���ɹ�  
        #�½�ģ�壬 ����ӳ��2����Datablock���͵Ĵ��̣� Ӧ��ʧ�ܣ�611003248365	
        self.assertTrue(i.enable(user_image_id,I_ENABLE))
        self.assertTrue(i.publish(user_image_id,IS_PUBLIC))
        self.assertTrue(i.set(image_id, NO_IMAGE_NAME, "datablock", NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION,"Linux"))
        self.assertTrue(i.enable(image_id,I_ENABLE))
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [{"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"datablock","target":"sdb"}]
        self.assertFalse(vt.allocate(cfg))
        #self.assertFalse(vt.del_vmt(test_vt_name))

        # ����ӳ��3������ΪDatablock�����ã� Ӧ���ɹ�  
        #�½�ģ�壬 ����ӳ��3����Datablock���͵Ĵ��̣� Ӧ��ʧ��
        self.assertTrue(i.enable(image_id,I_DISABLE))
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [{"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"datablock","target":"sdb"}]
        self.assertFalse(vt.allocate(cfg) )

        # ����ӳ��4������ΪCdrom��ʹ�ܣ� Ӧ���ɹ� 
        #�½�ģ�壬 ����ӳ��4����cdrom���͵Ĵ��̣� Ӧ���ɹ�
        self.assertTrue(i.set(image_id, NO_IMAGE_NAME, "cdrom", NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION,"Linux"))
        self.assertTrue(i.enable(image_id,I_ENABLE))
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [{"id":image_id,"position":1,"size":0L,"bus":"ide","type":"cdrom","target":"hdb"}]
        self.assertTrue(vt.allocate(cfg))
        self.assertTrue(vt.del_vmt(test_vt_name))

        # ����ӳ��5������ΪCdrom�����ã� Ӧ���ɹ�
        #�½�ģ�壬 ����ӳ��5����Cdrom���͵Ĵ��̣� Ӧ��ʧ��
        self.assertTrue(i.enable(image_id,I_DISABLE))
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [{"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"cdrom","target":"sdb"}]
        self.assertFalse(vt.allocate(cfg))


        # ʹ��ֱ�ӽӿڽ��������ģ�崴��  ��������  Ӧ���ɹ�
        self.assertTrue(i.set(image_id, NO_IMAGE_NAME, "machine", NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION,"Linux"))
        self.assertTrue(i.enable(image_id,I_ENABLE))
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["context"] = {}
        cfg["base_info"]["qualifications"] = []
        cfg["base_info"]["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
        #cfg["base_info"]["nics"] = [{"pci_order":0,"plane":test_netplane,"vlan":0,"dhcp":bool(1),"ip":"10.43.180.250","netmask":"255.255.254.0","gateway":"10.43.180.1","mac":""}]
        self.assertTrue(vt.allocate(cfg))
   
        # �޸Ĳ���
        # �޸ķ���״̬��ʹ�� publish����ӿڣ�  ����Ϊ test_vt_name,0   Ӧ�óɹ�
        self.assertTrue(vt.unpublish_vmt(test_vt_name))
        # ��ѯ �����ģ��ķ���״̬  0,30,3,test_vt_name  Ӧ��Ϊ0
        self.assertEqual(vt.query_pub(QUERY_BY_VT_NAME,test_vt_name), 0)

        # ʹ���޸Ľӿ��޸� ��������   Ӧ�óɹ�
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 4
        cfg["base_info"]["memory"] = long(256 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"xvda"}
        cfg["base_info"]["virt_type"] = "pvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["context"] = {}
        cfg["base_info"]["qualifications"] = []
        cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"xvdb","position":1,"fstype":"ext2"}]
        #611003123662 TC���ݿ����� ����description�ֶ�; vnc_passwd �ֶδ�ApiVmCfgAllocateData���Ƶ� ApiVmCfgAllocateData.base_info��
        cfg["base_info"]["description"] = "hello, world!"
        cfg["base_info"]["vnc_passwd"] = "vnc_passwd003,!"
        self.assertTrue(vt.set(cfg))
        # ��ѯ �����ģ��ķ���״̬  0,30,3,test_vt_name  Ӧ��Ϊ1
        self.assertEqual(vt.query_pub(QUERY_BY_VT_NAME,test_vt_name), IS_PUBLIC)

        # ��ѯ �����ģ���CPU����    0,30,3,test_vt_name  Ӧ��Ϊ4
        self.assertEqual(vt.query_vcpu(QUERY_BY_VT_NAME,test_vt_name), 4)

        # ��ѯ �����ģ����ڴ��С    0,30,3,test_vt_name  Ӧ��Ϊ256 * 1024 *1024
        self.assertEqual(vt.query_memory(QUERY_BY_VT_NAME,test_vt_name),(long(256 * 1024 *1024)))

        # ��ѯ �����ģ��ľ����������� 0,30,3,test_vt_name  Ӧ��Ϊ"ide"
        self.assertEqual(vt.query_machine_bus(QUERY_BY_VT_NAME,test_vt_name), "ide")
        # ��ѯ �����ģ������⻯����   0,30,3,test_vt_name  Ӧ��Ϊ"pvm"
        self.assertEqual(vt.query_virt_type(QUERY_BY_VT_NAME,test_vt_name), "pvm")
        # ��ѯ���ڵ�����  0,30,3,test_vt_name Ӧ��Ϊ 0
        self.assertEqual(vt.query_nic_count(QUERY_BY_VT_NAME,test_vt_name), 0)
        # ��ѯ���̵Ĵ�С  0,30,3,test_vt_name Ӧ��Ϊ 256 * 1024 * 1024
        self.assertEqual(vt.query_disks_size(QUERY_BY_VT_NAME,test_vt_name), (long(256 * 1024 * 1024)))
        # ��ѯ���̵��������� 0,30,3,test_vt_name Ӧ��Ϊ "scsi"
        self.assertEqual(vt.query_disks_bus(QUERY_BY_VT_NAME,test_vt_name), "scsi")
        # ��ѯ���̵�target 0,30,3,test_vt_name Ӧ��Ϊ "xvdb"
        self.assertEqual(vt.query_disks_target(QUERY_BY_VT_NAME,test_vt_name), "xvdb")
        #611003123662 TC���ݿ����� ����description�ֶ�; vnc_passwd �ֶδ�ApiVmCfgAllocateData���Ƶ� ApiVmCfgAllocateData.base_info��
        # ��ѯ�����������  Ӧ��Ϊhello, world!
        self.assertEqual(vt.query_description(QUERY_BY_VT_NAME,test_vt_name), "hello, world!")
        # ��ѯ�������vnc����  Ӧ��Ϊvnc_passwd003
        self.assertEqual(vt.query_vnc_passwd(QUERY_BY_VT_NAME,test_vt_name), "vnc_passwd003,!")
        # ��ѯ���̵ĸ�ʽ���ļ�ϵͳ 0,30,3,test_vt_name Ӧ��Ϊ "ext2"
        #zybĿǰ������!!�Ժ���Ҫ�ſ�
        #if vt.query_disks_fstype(3,test_vt_name) != "ext2":
        #    ci_exit_ex(-1,line())
        # ��ѯ����
        # ��ѯ�����ģ�������   ����Ϊ 0,30,1,""   Ӧ��Ϊ1
        self.assertEqual(vt.query_count(QUERY_USER_CREAT_VT,""), 1)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,2,""   Ӧ��Ϊ1
        self.assertEqual(vt.query_count(QUERY_USER_VISIBLE_VT,""), 1)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,3, test_vt_name   Ӧ��Ϊ1
        self.assertEqual(vt.query_count(QUERY_BY_VT_NAME,test_vt_name), 1)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,3, "test"   Ӧ��ΪNone
        # ���ش����룺��ѯʧ�ܣ���������������Ӧ��ΪNone 
        self.assertEqual(vt.query_count(QUERY_BY_VT_NAME,"test"), None)

        self.assertTrue(vt.publish_vmt(test_vt_name))

        # ------------------�⻧����Ȩ�޲���------------------
        # ����ģ�����
        # ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ��ʧ�� //chenww cli�ӿ�Ӧɾ��
        cfg = {}
        cfg["vt_name"] = "user_vt"
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        user_vt = vmtemplate(server,user_session)
        self.assertFalse(user_vt.allocate(cfg))
        # ��������  ��������    Ӧ��ʧ��     //chenww cli�ӿ�Ӧɾ��
        disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        # test_vt_name �� disks = [disk1]
        disks = [disk1]
        self.assertFalse(user_vt.set_disk(test_vt_name,disks))
        # �޸ķ���״̬��ʹ�� publish����ӿڣ�  ����Ϊ test_vt_name   Ӧ��ʧ��
        self.assertFalse(user_vt.publish_vmt(test_vt_name) )
        # ʹ���޸Ľӿ��޸� ��������   Ӧ��ʧ��
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(1)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 4
        cfg["base_info"]["memory"] = long(256 * 1024 * 1024)
        cfg["base_info"]["vm_wdtime"] = 0
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext2"}]

        self.assertFalse(user_vt.set(cfg))
        # ��ѯ�����ģ�������   ����Ϊ 0,30,1,""   Ӧ��Ϊ0
        self.assertEqual(user_vt.query_count(QUERY_USER_CREAT_VT,NO_VT_NAME),None)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,2,""   Ӧ��Ϊ1
        self.assertEqual(user_vt.query_count(QUERY_USER_VISIBLE_VT,NO_VT_NAME), 1)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,3, test_vt_name   Ӧ��Ϊ1
        self.assertEqual(user_vt.query_count(QUERY_BY_VT_NAME,test_vt_name), 1)
        # ��ѯ�����ģ�������   ����Ϊ 0,30,3, "test"   Ӧ��Ϊ0
        self.assertEqual(user_vt.query_count(QUERY_BY_VT_NAME,"test"), None)


        # ------------------����Ա����Ȩ�޲���------------------
        # ��




        # ------------------���������ز���------------------
        # ɾ�������ģ�� ����Ϊ test_vt_name   Ӧ���ɹ����ò���ʹ�ù���ԱȨ�޲�����
        self.assertTrue(vt.del_vmt(test_vt_name))

        #611003122482 VMCFG�����ֶγ�������
        #EC611003017919 ����16��Ӳ�̵�vm�޷�������ʾSchedule failed to allocate vid
        cfg = {}
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["vnc_passwd"] = "vnc_passwd"
        cfg["vm_name"] = test_vm_name1
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1  
        #����17�����̵����ݺ�9������������
        disk={}
        for i in range(0,17):
            disk[i] = {"id":-1L,"size":long(100 * 1024 * 1024),"bus":"ide","type":"disk","position":1,"fstype":"ext3"}
        nics={}
        #index = 0
        #for i in range(0,9):
         #   index = index + 1
         #   nics[i] = {"nic_index":index,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}    
              
        
        #���������ģ�� Ӧ�óɹ�
        self.assertFalse(vt.allocate(cfg))
       
        #һ�����̣��Ÿ������������������Ӧ��ʧ��
        cfg["base_info"]["disks"] = [disk[0]]        
        cfg["base_info"]["nics"] = [nics[0],nics[1],nics[2],nics[3],nics[4],nics[5],nics[6],nics[7],nics[8]] 
        #���������ģ�� Ӧ�óɹ�
        self.assertFalse(vt.allocate(cfg))
        
        
        #ʮ�������̣��˸������������������Ӧ�óɹ�
        cfg["base_info"]["disks"] = [disk[0],disk[1],disk[2],disk[3],disk[4],disk[5],disk[6],disk[7],disk[8],disk[9],disk[10],disk[11],disk[12],disk[13],disk[14],disk[15]]
       
        #����ģ�崴������� Ӧ�óɹ�
        v = vmcfg(server,root_session)
        self.assertTrue(v.allocate(cf))
        # �������ƻ�ȡ�ոմ������������id Ӧ�óɹ�
        vm_id = v.get_vm_id(test_vm_name1)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)   
         
        # ��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"deploy"))
            
        # ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬��״̬6Ϊλ��״̬
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17) 
        time.sleep(10)             
        #ֹͣ����� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"stop"))
        #��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"cancel"))
	# ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ16�Σ����16�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # ɾ�������ģ��  Ӧ���ɹ����ò���ʹ�ù���ԱȨ�޲�����
        self.assertTrue(vt.del_vmt(test_vt_name))
        
        #611003022063 ��tecs��storage ��check������Ӧ�ô����ļ�idΪ-1 ���ļ�����Ϊfile��iso�ļ�
        #611003184730 DEV�汾�����ô��̣�ֻ����һ�������ģ��ᵼ�²����쳣
        cfg = {}
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        cfg["base_info"]["vnc_passwd"] = "vnc_passwd"
        cfg["vm_name"] = test_vm_name2
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = CREAT_VM_NUM1        
        
        #cfg["base_info"]["disks"] = [{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext2"}]
        #����100k�����ݣ��������ļ����ƣ�
        data = ""
        for i in range(0,1024*100):
            data = data + "a"
        cfg["base_info"]["context"] = {"test.txt":data}
        #���������ģ�� Ӧ��ʧ��
        self.assertFalse(vt.allocate(cfg))
        #����100k������(�����ļ�����)
        data = ""
        for i in range(0,(1024*100-len("test.txt"))):
            data = data + "a"
        cfg["base_info"]["context"] = {"test.txt":data}
        #���������ģ�� Ӧ�óɹ�
        v = vmcfg(server,root_session)        
        self.assertTrue(v.allocate(cfg))
        # �������ƻ�ȡ�ոմ������������id Ӧ�óɹ�
        vm_id = v.get_vm_id(test_vm_name2)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)    
        # ��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"deploy"))   
        # ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬��״̬6Ϊλ��״̬
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        #611003154777 ��V4���⻯�������reset����ԭ���Ĳ���˱���˰�ˣ�
        #���������������CPUʹ���ʲ������������
        #ͨ��xm vcpu-list�����ѯCPU Affinity��ֵ��Ӧ��Ϊany cpu      
        sshcmd = ssh_cmd(server_addr,ssh_port)
        vmInstace = "instance_" + str(vm_id)
        tgt_list = sshcmd.xm_cmd("xm vcpu-list","Name",vmInstace)
        print tgt_list
        for tgt_dict in tgt_list:
            self.assertEqual(tgt_dict['CPU'],"any")
            if tgt_dict.has_key("Affinity"):
                self.assertEqual(tgt_dict['Affinity'],"cpu")
        #ǿ�����������
        time.sleep(20)
        self.assertTrue(v.action(vm_id,"reset"))  
        time.sleep(20)
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1            
            self.assertTrue(count<17)
        print "count =%s" %count
        #�ٴ�ͨ��xm vcpu-list�����ѯCPU Affinity��ֵ��Ӧ��Ϊany cpu  
        tgt_list = sshcmd.xm_cmd("xm vcpu-list","Name",vmInstace)
        print tgt_list
        for tgt_dict in tgt_list:
            self.assertEqual(tgt_dict['CPU'],"any")
            if tgt_dict.has_key("Affinity"):
                self.assertEqual(tgt_dict['Affinity'],"cpu")
            
        #ֹͣ����� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"stop"))
        #��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"cancel"))
	# ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ16�Σ����16�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # ɾ�������ģ��  Ӧ���ɹ����ò���ʹ�ù���ԱȨ�޲�����
        self.assertTrue(vt.del_vmt(test_vt_name))
        
        #�����ӳ��֧�ֶ�̬�������ܲ���
        cfg = {}
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 1
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""  
        cfg["base_info"]["vnc_passwd"] = "vnc_passwd"
        cfg["vm_name"] = test_vm_name3
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = CREAT_VM_NUM1           
        #��ȡ����Ա����Ȩ��
        v = vmcfg(server,root_session)
        #��������� Ӧ�óɹ�
        self.assertTrue(v.allocate(cfg))
        # �������ƻ�ȡ�ոմ������������id Ӧ�óɹ�
        vm_id = v.get_vm_id(test_vm_name3)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            self.assertTrue(count<20)
        print "waiting for hc power on ..."    
        time.sleep(180)    
        # ��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"deploy"))   
        # ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬
        count = 0
        vm_state = v.get_vm_state(vm_id)  
        while (vm_state != U_STATE_RUNNING):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)            
        # ������ػ� Ӧ�óɹ�        
        self.assertTrue(v.action(vm_id,"stop")) 
        # ��ȡ�����״̬   Ӧ���ǹػ�
        count = 0
        vm_state = v.get_vm_state(vm_id)
        while (vm_state != U_STATE_SHUTOFF):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<3) 
        #����ӳ��
        CfgModify={}
        CfgModify["base_info"]={}
        CfgModify["base_info"]["vcpu"] = 1
        CfgModify["base_info"]["memory"] = long(128 * 1024 * 1024)
        CfgModify["base_info"]["virt_type"] = "hvm"
        CfgModify["base_info"]["machine"] = {"id":user_image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        CfgModify["vid"]=vm_id
        CfgModify["vm_name"]=test_vm_name3
        CfgModify["project_name"]=test_project_name 
        self.assertTrue(v.set(CfgModify)) 
        # ����ͬ�� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"sync")) 
        # ��������� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"start"))           
        # ��ȡ�����״̬   Ӧ��������̬
        count = 0
        vm_state = v.get_vm_state(vm_id)  
        while (vm_state != U_STATE_RUNNING):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)   
        #��ѯ�����ӳ��id Ӧ��Ϊ user_image_id
        self.assertEqual(v.query_machine_id(vm_id), user_image_id,"failed to change vm image_id!")
        #���Խ���
        print "success to test test_013_vmt.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(VmtTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
