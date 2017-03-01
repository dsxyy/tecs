#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_009_host.py
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
#    �޸����ݣ�EC��611002999057 �޸�host���Ը���set�ӿ�
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
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�
server.system.listMethods()

class HostTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testHost(self):
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
        self.assertNotEqual(user_session, None, "failed to get user session!")
        
        # �Թ���Ա����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�  
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # ���⻧����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
        
        # ע�ἯȺ
        root_cluster_manager_method = clustermanager(server,root_session) 

        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # �ȴ� 20S �ȴ���Դ�ϱ�
        #time.sleep(20)  

        # ��������
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
            
        # ����tcu 8��
        root_host_method = host(server,root_session)
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id, None)
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))

        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,"",cpu_id))

        # ע��image
        # �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
        i = image(server,root_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id, None,"failed to get image id!")
          
        # ------------------����Ա����Ȩ�޲���------------------
        # ����ע��
        # ע������     ����Ϊ test_cluster,test_host,False,"test"  Ӧ�óɹ�
        h = host(server,root_session)
        # �ȴ�20S
        #time.sleep(20)
        # ��ѯ����������״̬  ����Ϊ 0,100,test_cluster,test_host   Ӧ��Ϊ 3
        #����cc����ʱ����ON_LINEΪ2
        self.assertEqual(h.query_run_state(test_cluster,test_host), ON_LINE)
  
        # ����ʹ��
        # ʹ������ ����Ϊ  test_cluster,test_host  Ӧ�óɹ�
        self.assertTrue(h.enable(test_cluster,test_host))

        # ��ѯ������ʹ��״̬ ����Ϊ 0,100,test_cluster,test_host  Ӧ��Ϊ 0
        self.assertEqual(h.query_enable_state(test_cluster,test_host), HOST_DISABLE)
        # ���Խ���
        # �������� ����Ϊ  test_cluster,test_host,1  Ӧ�óɹ�
        self.assertTrue(h.disable(test_cluster,test_host,HOST_ENABLE))
        # ��ѯ������ʹ��״̬ ����Ϊ 0,100,test_cluster,test_host  Ӧ��Ϊ 1
        self.assertEqual(h.query_enable_state(test_cluster,test_host), HOST_ENABLE)

        #���EC611002999057 �޸�host���Ը���set�ӿ�
        self.assertTrue(h.set(test_cluster,test_host,"test"))

        #�ظ�ע��  Ӧ�óɹ�
        self.assertTrue(h.set(test_cluster,test_host,"test"))

        # ע������  ����Ϊ test_cluster,"","test"  Ӧ��ʧ�ܣ�������Ϊ�գ�
        self.assertFalse(h.set(test_cluster,"","test"))
        # ע������  ����Ϊ test_cluster,"no_exist","test"  Ӧ��ʧ�ܣ����������ڣ�
        self.assertFalse(h.set(test_cluster,"no_exist","test"))  

        # �������� ����Ϊ test_cluster,test_host   Ӧ��ʧ��
        self.assertTrue(h.forget(test_cluster,test_host))
        # ��������������ʱ��ʵ�֣���ΪҪ���ǵ���3��HC�ļ��룬�ֽ׶�ֻ����3��һ�����
        # ��������
        # ���Թػ�

        # ------------------�⻧����Ȩ�޲���------------------    
        user_h = host(server,user_session)

        # ����ʹ��
        # ʹ������ ����Ϊ  test_cluster,test_host  Ӧ��ʧ��
        self.assertFalse(user_h.enable(test_cluster,test_host))
        
        # ���Խ���
        # �������� ����Ϊ  test_cluster,test_host,1  Ӧ��ʧ��
        self.assertFalse(user_h.disable(test_cluster,test_host,HOST_ENABLE)) 

        # ��������
        # �������� ����Ϊ test_cluster,test_host   Ӧ��ʧ��
        self.assertFalse(user_h.forget(test_cluster,test_host))
        # ------------------����Ա����Ȩ�޲���------------------
        # ��


        # ------------------���������ز���------------------
        time.sleep(20)
        
        # ���⻧��ݲ���һ�����������������ο�image ģ��
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
            
        cfg = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = test_vm_name
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1
        
        v = vmcfg(server,root_session)
        self.assertTrue(v.allocate(cfg))
        #���������֮ǰ����ӡ��Ⱥ��Դ���Ա㲿��ʧ��ʱ�鿴��Ϣ
        cm = clustermanager(server,root_session) 
        self.assertTrue(cm.query(START_INDEX,QUERY_COUNT,SYN_MODE))
        #���������
        self.assertTrue(v.action(v.get_vm_id(test_vm_name),"deploy"))

        # �ȴ�2S���ȴ�������������
        time.sleep(2)
        # �������� ����Ϊ  test_cluster,test_host  Ӧ��ʧ��
        self.assertFalse(h.disable(test_cluster,test_host,HOST_DISABLE)) 

        # ��ѯ�������е����������  0,100,test_cluster,test_host  Ӧ��Ϊ 1
        self.assertEqual(h.query_running_vms(test_cluster,test_host), 1)
        #���Խ���
        print "success to test test_009_host.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(HostTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
