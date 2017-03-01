#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001.py
# �������ݣ����������������������
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
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#*******************************************************************************/
import sys,os,getopt,time
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
from tecs.tecssystem import *

#���ڲ��Ե�tc xmlrpc��������ַ
server_addr = "localhost"
#���ڲ��Ե�tc xmlrpc�������˿ں�
server_port = "8080"
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
test_vm_name = "ttylinux"
#�����ԵĹ�������
test_project_name = "test_prj_1"

def usage():
    ''' ��ӡ������Ϣ'''
    print "Usage: %s [option]" % os.path.basename(sys.argv[0])
    print " -a,--addr, the xmlrpc server ip address or host name"
    print " -p,--port, the xmlrpc server port"
    print " -i,--image, the vm image file to upload"
    sys.exit(0)
    
def line():
    try:
            raise Exception
    except:
            return sys.exc_info()[2].tb_frame.f_back.f_lineno 
            
def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
        sys.exit(value)
def ci_exit_ex(value=0,line = line(),error="null",):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:    
        print "tecs ci test exit code = %d, line = %d, error string = %s" % (value,line,error)
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
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()

class SmokingTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testSmoking(self):  
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

        # ע��cc��tc
        print "=============================================="
        cm = clustermanager(server,root_session) 

        #TODO ��Ҫ����ѭ���ȴ�
        count=0
        while (cm.get_cluster_max_count() == 0):
            time.sleep(1)
            count = count + 1 
            self.assertTrue(count < 19)         
        print "get_cluster_max_count count= %d "% count  
        
        #��ѯ��Ⱥ�б�
        self.assertTrue(cm.query(START_INDEX,QUERY_COUNT,ASYN_MODE))

        # ע��hc��cc
        print "=============================================="
        h = host(server,root_session)

        #TODO ��Ҫ����ѭ���ȴ�    
        self.assertTrue(h.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))

        # ����tcu 
        print "=============================================="
        # ��ȡ�ո�ע���������cpu id   
        cpu_id = h.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id,None)        
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        s = tcu(server,root_session)
        self.assertTrue(s.set(test_cluster,cpu_id,TCU_NUM2,"test cpu description"))
        self.assertTrue(s.query(START_INDEX,QUERY_COUNT,test_cluster,"",cpu_id))  

        # ����һ������Ϊ1���û��飬����������ͨ�⻧���˺� 
        print "=============================================="
        ug = usergroup(server,root_session)
        self.assertTrue(ug.allocate(test_tenant_usergroup,GROUP_TYPE1))
            
        # ����һ����ͨ�˺����ں���ģ�������⻧�Ĳ��� 
        u = user(server,root_session)
        self.assertTrue(u.allocate(test_tenant_user,test_tenant_passwd,test_tenant_passwd,ACCOUNTUSER,test_tenant_usergroup))
            
        # ʹ����ͨ�⻧�˻���¼tecs����ȡ���˻����ܺ��session
        tenant_session = login(server,test_tenant_user,test_tenant_passwd)
        self.assertNotEqual(tenant_session,None,"failed to get encrypted tenant session!")    
        # ʹ��ftp�ͻ����ϴ�һ�������machineӳ��
        ftp = FTP()
        try:
            ftp.connect(server_addr,21)
            ftp.login(test_tenant_user,test_tenant_passwd)
        except:
            ci_exit_ex(-1,line(),"ftp login failed!")

        print ftp.getwelcome()
        try:
            cmd='%s %s' % ("stor",os.path.basename(test_image))
            ftp.storbinary(cmd,open(test_image,'rb'),1024)
        except:
            ci_exit_ex(-1,line(),"ftp upload failed!")

        # �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
        time.sleep(1)
        i = image(server,tenant_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id,None,"failed to get image id!")
        # ����һ������ƽ��

        # ��ѯ����ƽ������  ����Ҫ�����õ�һ�����п����������ϱ���

        # ����DHCP

        # ��������
        p = project(server,tenant_session)
        self.assertTrue(p.allocate(test_project_name,"test project description"))

        # ���������
        cfg = {}
        cfg_base = {}
        cfg_base["vcpu"] = 2
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
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = test_vm_name
        cfg["project_name"] = test_project_name
        cfg["vm_num"] = 1
        v = vmcfg(server,tenant_session)
        self.assertTrue(v.allocate(cfg))

        # �������ƻ�ȡ�ոմ������������id    
        vm_id = v.get_vm_id(test_vm_name)
        self.assertNotEqual(vm_id,None,"failed to get vm id!")
        # ��ȡ�����״̬
        vmstate = v.get_vm_state(vm_id)
        self.assertNotEqual(vmstate,None,"failed to get vm state!")            
        #����һ������
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        disks = [disk1]
        self.assertTrue(v.set_disk(vm_id,disks))
                
        #����һ������
        #nic1={"pci_order":0,"plane":"","vlan":0,"dhcp":bool(1),"ip":"10.43.180.250","netmask":"255.255.254.0","gateway":"10.43.180.1","mac":""}
        #nics = [nic1]
        #if v.set_nic(vm_id,nics) != True:
        #    ci_exit_ex(-1,line())
         
        #�ȴ�tc�ϼ�Ⱥ��Դ��Ϣ��λ 
        count = 0   
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query count = %d " %count

        print "waiting for hc power on ..."    
        time.sleep(180)   
         
        # ���������
        self.assertTrue(v.action(vm_id,"deploy"))
            
        # ��ȡ�����״̬   Ӧ���ǲ���ɹ�̬��״̬6Ϊλ��״̬
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)
  
        # ���������
        self.assertTrue(v.action(vm_id,"reboot"))

        # ��ȡ�����״̬��Ӧ��Ϊ2    
        count = 0
        vm_state = U_STATE_UNKNOWN  
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)

        # ǿ�����������
        self.assertTrue(v.action(vm_id,"reset"))
            
        # ��ȡ�����״̬��Ӧ��Ϊ2    
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)      
                
        # ȡ�������
        self.assertTrue(v.action(vm_id,"cancel"))    
        #ȡ����Ҫ3���ӣ�������ʱ200��
        print "wait 200s" 
        time.sleep(200)
        # ��ȡ�����״̬��Ӧ��Ϊ0 
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_CONFIGURATION and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17) 
                
        #�ٴβ��������
        self.assertTrue(v.action(vm_id,"deploy"))
        # ��ȡ�����״̬��Ӧ��Ϊ2
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)       
            
        # �������Ƿ����

        # ��������Ƿ�ͨ

        # cancle�����  Ӧ�óɹ�

        #���Խ���
        print "success to test test_001.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(SmokingTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
