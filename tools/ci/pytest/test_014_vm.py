#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_014_vm.py
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
#    �޸����ڣ�2012/06/8
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ѩ��
#    �޸����ݣ�ģ����Ժ���1
# �޸ļ�¼3��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ

#*******************************************************************************/
import sys,os,getopt,time,inspect
import hashlib
import unittest 
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
#test_image2 = "ttylinux2.img"
#test_image3 = "ttylinux3.img"
#test_image4 = "ttylinux4.img"
#test_image5 = "ttylinux5.img"
#�����Ե����������
vm_001="vm_001"
vm_002="vm_002"
vm_003="vm_003"
#�����ԵĹ�������
test_project_name = "test_prj_1"
test_project_name2 = "test_prj_2"
#����ƽ��
test_netplane = "base"

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
#    if o in ("-i", "--image"):
#        test_image2 = v
#        if os.path.isfile(test_image2) is not True:
#            ci_exit(-2,"image file %s not exist!" % test_image)
#    if o in ("-i", "--image"):
#        test_image3 = v
#        if os.path.isfile(test_image3) is not True:
#           ci_exit(-2,"image file %s not exist!" % test_image)   
#    if o in ("-i", "--image"):
#        test_image4 = v
#        if os.path.isfile(test_image4) is not True:
#            ci_exit(-2,"image file %s not exist!" % test_image)  
#    if o in ("-i", "--image"):
#        test_image5 = v
#        if os.path.isfile(test_image5) is not True:
#            ci_exit(-2,"image file %s not exist!" % test_image)            
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
class VmTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testVm(self):
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
        self.assertTrue(i.publish(image_id,IS_PUBLIC))
        # ��������
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name2,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))

        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))

   
        # ------------------����Ա����Ȩ�޲���------------------
        # ʹ�������ģ�崴����������������ģ������������Ѿ����Թ���
        # test_project_name��ֱ�Ӵ��������vm_001
        # ʹ�ò������£�     Ӧ���ɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
        #cfg_base["nics"] = [{"pci_order":0,"plane":test_netplane,"vlan":0,"vlantrunk":bool(0), "dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1

        #��ȡ����Ա����Ȩ��
        vm = vmcfg(server,root_session)
        self.assertTrue(vm.allocate(cfg))

        #�������ѯvm_id
        vm_id = vm.get_vm_id(vm_001)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        
        # �鿴������������������Ƿ��Ԥ��һ��       
        # �鿴VCPU    Ӧ��Ϊ1
        self.assertEqual(vm.query_vcpu(vm_id), 1)            
        # �鿴tcu    Ӧ��Ϊ1
        self.assertEqual(vm.query_tcu(vm_id), 1) 
        # �鿴�ڴ��С Ӧ��Ϊ128 * 1024 * 1024
        self.assertEqual(vm.query_memory(vm_id), (long(128 * 1024 *1024)))             
        # �鿴machine ��id  Ӧ��Ϊimage_id
        self.assertEqual(vm.query_machine_id(vm_id),image_id) 
        # �鿴machine ��bus  Ӧ��Ϊ ide
        self.assertEqual(vm.query_machine_bus(vm_id), "ide")        
        # �鿴machine ��type  Ӧ��Ϊmachine
        self.assertEqual(vm.query_machine_type(vm_id), "machine")
        # �鿴machine ��target  Ӧ��Ϊhda
        self.assertEqual(vm.query_machine_target(vm_id), "hda")
        # �鿴���⻯����  Ӧ��Ϊhvm
        self.assertEqual(vm.query_virt_type(vm_id), "hvm")
        # �鿴���̴�С   Ӧ��Ϊ 128 * 1024 * 1024
        self.assertEqual(vm.query_disks_size(vm_id), (long(128 * 1024 *1024))) 
        # �鿴��������   Ӧ��Ϊ ide
        self.assertEqual(vm.query_disks_bus(vm_id), "ide")
        # �鿴���̵�target   Ӧ��Ϊ hdb
        self.assertEqual(vm.query_disks_target(vm_id), "hdb")
        # �鿴���̵�position   Ӧ��Ϊ 0
        self.assertEqual(vm.query_disks_position(vm_id), 1)
        # �鿴���̵�fstype   Ӧ��Ϊ ext3
        self.assertEqual(vm.query_disks_fstype(vm_id), "ext3")

        # �鿴�����������  Ӧ��Ϊ vm_001
        self.assertEqual(vm.query_vmname(vm_id), "vm_001")        
        # �鿴������Ĺ�������  Ӧ��Ϊ test_project_name
        self.assertEqual(vm.query_projectname(vm_id), "test_prj_1")  
        # �޸���������ò���
        # �޸�vlanֵΪ1��Ӧ�óɹ�
       # cfg_base["nics"] = [{"pci_order":0,"plane":test_netplane,"vlan":1,"vlantrunk":bool(0), "dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}]
        cfg["vid"] = vm_id
        self.assertTrue(vm.set(cfg))  
       
        # test_project_nameʹ�����²δ��������2(��Ҫ��ȡ�������id,�޸Ľӿ���Ҫ�����id)��  Ӧ�óɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 2
        cfg_base["tcu"] = 2
        cfg_base["memory"] = long(200 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"xvdb"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "pvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"xvdb","position":1,"fstype":"ext2"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_002
        cfg["project_name"] = test_project_name
        
        vm = vmcfg(server,root_session)
        self.assertTrue(vm.allocate(cfg))

        #�������ѯvm_id
        vm_id = vm.get_vm_id(vm_002)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        # �鿴������������������Ƿ��Ԥ��һ��
        # �鿴VCPU    Ӧ��Ϊ2
        self.assertEqual(vm.query_vcpu(vm_id), 2)
        # �鿴tcu    Ӧ��Ϊ2
        self.assertEqual(vm.query_tcu(vm_id), 2)        
        # �鿴�ڴ��С Ӧ��Ϊ 200 * 1024 * 1024
        self.assertEqual(vm.query_memory(vm_id), (long(200 * 1024 *1024)))  
        # �鿴machine ��bus  Ӧ��Ϊ"scsi"
        self.assertEqual(vm.query_machine_bus(vm_id), "ide") 
        # �鿴machine ��type  Ӧ��Ϊmachine
        self.assertEqual(vm.query_machine_type(vm_id), "machine") 
        # �鿴machine ��target  Ӧ��Ϊxvdb
        self.assertEqual(vm.query_machine_target(vm_id), "xvdb") 
        # �鿴���⻯����  Ӧ��Ϊpvm
        self.assertEqual(vm.query_virt_type(vm_id), "pvm") 
        # �鿴���̴�С   Ӧ��Ϊ 200 * 1024 * 1024
        self.assertEqual(vm.query_disks_size(vm_id), (long(200 * 1024 *1024)))
        # �鿴��������   Ӧ��Ϊ "scsi"
        self.assertEqual(vm.query_disks_bus(vm_id), "scsi")
        # �鿴���̵�target   Ӧ��Ϊ xvdb
        self.assertEqual(vm.query_disks_target(vm_id), "xvdb")
        # �鿴���̵�position   Ӧ��Ϊ 1
        self.assertEqual(vm.query_disks_position(vm_id), 1)
        # �鿴���̵�fstype   Ӧ��Ϊ ext2
        self.assertEqual(vm.query_disks_fstype(vm_id), "ext2")
       
        # �鿴�����������  Ӧ��Ϊ vm_002
        self.assertEqual(vm.query_vmname(vm_id), "vm_002")  
        # �鿴������Ĺ�������  Ӧ��Ϊ test_project_name
        self.assertEqual(vm.query_projectname(vm_id), "test_prj_1")
        # ���������������
        # ���ô����ӿڣ���������(machine type  ��)      Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"kernel","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ���ô����ӿڣ��������� (machine id ����)     Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":1L,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ���ô����ӿڣ��������� (disk id ����)     Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":0L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ���ô����ӿڣ��������� (disk �������ʹ���)     Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"aaa","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ���ô����ӿڣ��������� (disk ��ʽ�����ʹ���)     Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"nfs"}]
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ���ô����ӿڣ��������� (���̲�����)     Ӧ��ʧ��
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = [{}]
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext4"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = "no_exit_project"  
        cfg["vm_num"] = 1

        self.assertFalse(vm.allocate(cfg))

        # ����ӳ��2������ΪDatablock��ʹ�ܣ� Ӧ���ɹ�	 chenww
#        i2 = image(server,root_session)
#        image_id2 = i2.get_id_by_filename(os.path.basename(test_image2))
#        self.assertNotEqual(image_id2, None, "failed to get image id!")  
#        self.assertTrue(i2.enable(image_id2,I_ENABLE))
        # ����ӳ��3������ΪDatablock�����ã� Ӧ���ɹ�	 chenww
#        i3 = image(server,root_session)
#        image_id3 = i3.get_id_by_filename(os.path.basename(test_image3))
#        self.assertNotEqual(image_id3, None, "failed to get image id!")  
#        self.assertTrue(i3.enable(image_id3,I_DISABLE))
        # ����ӳ��4������ΪCdrom��ʹ�ܣ� Ӧ���ɹ�	 chenww
#        i4 = image(server,root_session)
#        image_id4 = i4.get_id_by_filename(os.path.basename(test_image4))
#        self.assertNotEqual(image_id4, None, "failed to get image id!")  
#        self.assertTrue(i4.enable(image_id4,I_ENABLE))
        # ����ӳ��5������ΪCdrom�����ã� Ӧ���ɹ�	 chenww
#        i5 = image(server,root_session)
#        image_id5 = i5.get_id_by_filename(os.path.basename(test_image5))
#        self.assertNotEqual(image_id5, None, "failed to get image id!")  
#        self.assertTrue(i5.enable(image_id5,I_DISABLE))
        #�½�������� ����ӳ��2����Datablock���͵Ĵ��̣� Ӧ���ɹ�  chenww
#       cfg_base = {}
        #        cfg_base["vcpu"] = 2
#        cfg_base["tcu"] = 2
#        cfg_base["memory"] = long(200 * 1024 * 1024)
#        cfg_base["cluster"] = ""
#        cfg_base["host"] = ""
#        cfg_base["vm_wdtime"] = 0
#        cfg_base["machine"] = {"id":image_id2,"size":0L,"bus":"ide","type":"machine","target":"hda"}
#        cfg_base["kernel"] = {"id":-1L,"size":0L}
#        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
#        cfg_base["virt_type"] = "pvm"
#        cfg_base["root_device"] = ""
#        cfg_base["kernel_command"] = ""
#        cfg_base["rawdata"] = ""
#        cfg_base["context"] = {}
#        cfg_base["qualifications"] = []
#        cfg_base["disks"] = [{"id":image_id2,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg_base["nics"] = [{"pci_order":0,"plane":"test_plane1","vlan":0,"vlantrunk":bool(0), "dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}]
            
#        cfg = {}
#        cfg["base_info"] = cfg_base
#        cfg["vm_name"] = vm_001
#        cfg["project_name"] = test_project_name

#        self.assertTrue(v.allocate(cfg))        
        #�½�������� ����ӳ��3����Datablock���͵Ĵ��̣� Ӧ��ʧ��  chenww
#        disk3 = [{"id":image_id3,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk3]
#        self.assertFalse(v.allocate(cfg))  
        #�½�������� ����ӳ��4����cdrom���͵Ĵ��̣� Ӧ���ɹ�  chenww
#        disk4 = [{"id":image_id4,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk4]
#        self.assertTrue(v.allocate(cfg)) 
        #�½�������� ����ӳ��5����Cdrom���͵Ĵ��̣� Ӧ��ʧ��  chenww
#        disk5 = [{"id":image_id5,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk5]
#        self.assertFalse(v.allocate(cfg)) 
        # �������������
        # ��ʹ���޸Ľӿ� ��������  ��������   Ӧ�óɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_003
        cfg["project_name"] = test_project_name
        cfg["vm_num"] = 1    

        vm = vmcfg(server,root_session)
        self.assertTrue(vm.allocate(cfg))       
    
        #��ѯ�����ID
        vm_id = vm.get_vm_id(vm_003)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")

        #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(0L,100L,1) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)       
        # ��������� Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"deploy"))

        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN   
        while (vm.get_vm_state(vm_id) != U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)    
        # ֹͣ�����  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"stop"))
        # ��ѯ״̬    Ӧ����4���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_SHUTOFF and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)        
        # ���������   Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"start"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)  
        # ���������  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"pause"))
        # ��ѯ״̬    Ӧ����3���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_PAUSED and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # �ָ������  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"resume"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ���������  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"reboot"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ȡ�������  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"cancel"))
		
	# ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ16�Σ����16�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=2       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # ɾ�������  Ӧ���ɹ�
        self.assertTrue(vm.action(vm_id,"delete"))
        # ��ѯ��������� Ӧ��Ϊ2
        self.assertEqual(vm.query_count(-5L), 2)
        # ʹ��֮ǰ�����ɹ����������������2�������������һ����
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name 

        v = vmcfg(server,root_session)
        self.assertTrue(v.allocate(cfg))
        vm_id = v.get_vm_id(vm_001)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")        
        self.assertTrue(v.allocate(cfg))
        # ʹ�ò�ѯ�ӿ� vmcfg_query ���������������ѯ
        # ����Ϊ 0,30,-5     Ӧ��Ϊ 4
        self.assertEqual(v.query_count(-5L), 4)
        # ����Ϊ 0,30,����ʱ�����ɵ�id     Ӧ�óɹ�
        self.assertTrue(v.query(vm_id))
        # ����Ϊ 0,30,�����ڵ������id     Ӧ��ʧ��
        self.assertFalse(v.query(10L))
        # ʹ�ò�ѯ�ӿ� vmcfg_fullquery ���������������ѯ
        # ����Ϊ 0,30,-5,"",test_project_name,-1     Ӧ��Ϊ 4
        self.assertTrue(v.fullquery(-5L,"",test_project_name,0L,30L),4)
        # ����Ϊ 0,30,-5,"","",-1     Ӧ��Ϊ 4
        self.assertTrue(v.fullquery(-5L,"","",0L,30L),4)
        # ����Ϊ 0,30,-5,"",test_project_name2,-1     Ӧ��Ϊ 0
        self.assertTrue(v.fullquery(-5L,"",test_project_name2,0L,30L),0)
        # ����Ϊ 0,30,-4,"admin","",-1     Ӧ��Ϊ 4
        self.assertTrue(v.fullquery(-4L,"admin",test_project_name,0L,30L),4)
        # ����Ϊ 0,30,-4,"user","",-1     Ӧ��Ϊ 0
        self.assertTrue(v.fullquery(-4L,"user",test_project_name,0L,30L),0)
        # ����Ϊ 0,30,-3,"","",-1     Ӧ��Ϊ 4
        self.assertTrue(v.fullquery(-3L,"","",0L,30L),4)
        # ʹ�ò�ѯ�ӿ� vmcfg_vm_syn ���������������ѯ
        # ����Ϊ ��δ����������id1, ��δ����������id2   ���ؽ��Ӧ����2�����������̬

        # ʹ�� vmcfg_get_id_by_name ��ѯ
        # ����Ϊ -5,vm_001,""   Ӧ�÷���3
        self.assertTrue(v.get_id_by_name(-5L,"vm_001",""),3)
        # ����Ϊ -4,vm_001,"admin"   Ӧ�÷���3
        self.assertTrue(v.get_id_by_name(-4L,"vm_001","admin"),3)
        # ����Ϊ -3,vm_001,""   Ӧ�÷���3
        self.assertTrue(v.get_id_by_name(-3L,"vm_001",""),4)
        # ���������������ϵ����(id1, id2Ϊ���洴����2�������id)
        # ��������                       Ӧ���ɹ�
        cfg = {}
        cfg["type"] = 0
        #cfg["vec_vm"] = [{id1, id2}]
        # ��ѯ�����������ϵ
        # ����Ϊ 0     ���ؽ��Ϊ��1�����������ϵ
        # ����Ϊid1   ���ػ����ϵ����һ��id Ϊ id2
        # ɾ�������������ϵ
        # ��������                       Ӧ���ɹ�
        cfg = {}
        cfg["type"] = 1
        #cfg["vec_vm"] = [{id1, id2}]
        # ��ѯ�����������ϵ
        # ����Ϊ 0     ���ؽ��Ϊ��0�����������ϵ

        # ����Ϊid1   ����ΪNULL

        # ------------------�⻧����Ȩ�޲���------------------
        user_v = vmcfg(server,user_session)
        # ֱ�Ӵ��������
        # �⻧��test_project_name��ʹ�ò������£�     Ӧ���ɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1
        
        self.assertTrue(user_v.allocate(cfg)) 
        
        # �⻧��test_project_name�´���vm_002��     Ӧ���ɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_002
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1
        
        self.assertTrue(user_v.allocate(cfg)) 

        # �⻧��test_project_name2�´���vm_001��    Ӧ���ɹ���
        cfg["project_name"] = test_project_name2
        cfg["vm_name"] = vm_001
        self.assertTrue(user_v.allocate(cfg))
        
        # �޸���������ò���
        # ʹ�����²���(��Ҫ��ȡ�������id,�޸Ľӿ���Ҫ�����id)��  Ӧ�óɹ�
        vm_id = user_v.get_vm_id(vm_001)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")

        # �������������
        # ��ʹ���޸Ľӿ� ��������  ��������   Ӧ�óɹ�
        cfg_base = {}
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["machine"] = {"id":image_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg_base["kernel"] = {"id":-1L,"size":0L}
        cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}
        cfg_base["virt_type"] = "hvm"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}]
   
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = vm_001
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1
        cfg["vid"] = vm_id
        
        self.assertTrue(user_v.set(cfg))
        # ���������   Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"deploy"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=6   
        while (user_v.get_vm_state(vm_id) != U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)        
        # ֹͣ�����  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"stop"))
        # ��ѯ״̬    Ӧ����4���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_SHUTOFF and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ���������   Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"start"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ���������  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"pause"))
        # ��ѯ״̬    Ӧ����3���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_PAUSED and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # �ָ������  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"resume"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ���������  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"reboot"))
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # ȡ�������  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"cancel"))
        # ��ѯ��������� Ӧ��Ϊ3
        self.assertEqual(user_v.query_count(-5L), 3)
        # ��ʱ3����
        time.sleep(180)
        
        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ16�Σ����16�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # ɾ�������  Ӧ���ɹ�
        self.assertTrue(user_v.action(vm_id,"delete"))
        # ʹ��֮ǰ�����ɹ����������������2�������������һ����
        # ʹ�ò�ѯ�ӿ� vmcfg_query ���������������ѯ
        # ����Ϊ 0,30,����ʱ�����ɵ�id     Ӧ�óɹ�
        #self.assertTrue(user_v.query(vm_id))
        # ����Ϊ 0,30,�����ڵ������id     Ӧ��ʧ��
        self.assertFalse(user_v.query(23L))
        # ʹ�ò�ѯ�ӿ� vmcfg_fullquery ���������������ѯ
        # ����Ϊ 0,30,-5,"",test_project_name,-1     Ӧ��Ϊ 3
        self.assertTrue(user_v.fullquery(-5L,"",test_project_name,0L,30L),3)
        # ����Ϊ 0,30,-5,"","",-1     Ӧ��Ϊ 3
        self.assertTrue(user_v.fullquery(-5L,"","",0L,30L),3)
        # ����Ϊ 0,30,-5,"",test_project_name2,-1     Ӧ��Ϊ 0
        self.assertTrue(user_v.fullquery(-5L,"","test_project_name2",0L,30L),0)
        # ����Ϊ 0,30,-5,"","",0     Ӧ��Ϊ 3
        self.assertTrue(user_v.fullquery(-5L,"","",0L,30L),3)
        # ����Ϊ 0,30,-4,"admin","",-1     Ӧ��ʧ��
        self.assertFalse(user_v.fullquery(-4L,"admin","",0L,30L),0)
        # ����Ϊ 0,30,-4,"user","",-1     Ӧ��ʧ��
        self.assertFalse(user_v.fullquery(-4L,"user","",0L,30L),0)
        # ����Ϊ 0,30,-3,"","",-1     Ӧ��ʧ��
        self.assertFalse(user_v.fullquery(-3L,"admin","",0L,30L))
        # ʹ�ò�ѯ�ӿ� vmcfg_vm_syn ���������������ѯ
        # ����Ϊ ��δ����������id3, ��δ����������id4   ���ؽ��Ӧ����2�����������̬
        # ʹ�� vmcfg_get_id_by_name ��ѯ
        # ����Ϊ -5,vm_001,""   Ӧ�÷���3
        self.assertTrue(user_v.get_id_by_name(-5L,"vm_001",""),3)
        # ����Ϊ -4,vm_001,"admin"   Ӧ��ʧ��
        self.assertFalse(user_v.get_id_by_name(-4L,"vm_001","admin"))
        # ����Ϊ -4,vm_001,"user"   Ӧ��ʧ��
        self.assertFalse(user_v.get_id_by_name(-4L,"vm_001","user"))
        # ����Ϊ -3,vm_001,""   Ӧ��ʧ��
        self.assertFalse(user_v.get_id_by_name(-3L,"vm_001",""))
        # ���������������ϵ����(id3, id4Ϊ���洴����2�������id)
        # ��������                       Ӧ���ɹ�
        #cfg["vec_vm"] = [{id3, id4}]

        # ��ѯ�����������ϵ
        # ����Ϊ 0     ���ؽ��Ϊ��1�����������ϵ

        # ����Ϊid3   ���ػ����ϵ����һ��id Ϊ id4

        # ɾ�������������ϵ
        # ��������                       Ӧ���ɹ�
#        cfg = {}
#        cfg["type"] = 1
        #cfg["vec_vm"] = [{id3, id4}]

        # ��ѯ�����������ϵ
        # ����Ϊ 0     ���ؽ��Ϊ��0�����������ϵ

        # ����Ϊid3   ����ΪNULL


        # ------------------����Ա����Ȩ�޲���------------------
        # �����޸��⻧����Ϣ
        # ʹ������ �⻧������id3 �������޸ģ�����޸�����һ�������������������������ӣ�   Ӧ��ʧ��
        #��ȡ����Ա����Ȩ��
        vm = vmcfg(server,root_session)
        cfg["vid"] = vm_id
        self.assertFalse(vm.set(cfg))
        # ���Բ�ѯ�⻧����Ϣ��ʹ���⻧��Ȩ�ޣ�
        # ʹ�ò�ѯ�ӿ� vmcfg_fullquery ���������������ѯ
        # ����Ϊ 0,30,-5,"",test_project_name,-1     Ӧ��Ϊ 2

        # ����Ϊ 0,30,-5,"","",-1     Ӧ��Ϊ 2

        # ����Ϊ 0,30,-5,"",test_project_name2,-1     Ӧ��Ϊ 0

        # ����Ϊ 0,30,-5,"","",0     Ӧ��Ϊ 2

        # ����Ϊ 0,30,-5,"","",1     Ӧ��Ϊ 0

        # ����Ϊ 0,30,-4,"admin","",-1     Ӧ��2

        # ����Ϊ 0,30,-4,"user","",-1     Ӧ��2

        # ����Ϊ 0,30,-3,"","",-1     Ӧ��4

        # ʹ�ò�ѯ�ӿ� vmcfg_vm_syn ���������������ѯ
        # ����Ϊ id1,id2,id3,id4   ����4���������״̬��Ϊ����̬

        # ʹ�� vmcfg_get_id_by_name ��ѯ
        # ����Ϊ -5,vm_001,""   Ӧ�÷���2

        # ����Ϊ -4,vm_001,"admin"   Ӧ��2

        # ����Ϊ -4,vm_001,"user"   Ӧ��2

        # ����Ϊ -3,vm_001,""   Ӧ��4


        # ���Բ����⻧�������(ʹ�õ�����Ϊ�⻧�����������id3)
        # ���������   Ӧ��ʧ��

        # ʹ���⻧��ɫ����id3

        # ֹͣ�����  Ӧ��ʧ��

        # ���������   Ӧ��ʧ��

        # ���������  Ӧ��ʧ��

        # �ָ������  Ӧ��ʧ��

        # ���������  Ӧ��ʧ��

        # ȡ�������  Ӧ���ɹ�

        # ��ʱ3����

        # ��ѯ״̬    Ӧ����0���ɲ���5S��ѯһ�Σ�һ����ѯ3�Σ����3�ζ���ѯʧ�ܣ����˳���

        # ɾ�������  Ӧ���ɹ�
        
        #���Խ���
        print "success to test test_014_vm.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(VmTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()


