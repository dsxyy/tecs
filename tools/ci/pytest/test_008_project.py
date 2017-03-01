#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_008_project.py
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
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#*******************************************************************************/
import sys,os,getopt,inspect,time
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
test_vm_name1 = "ttylinux1"
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

class ProjectTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testProject(self):
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

        #����һ���û���ʹ�ò���Ϊ test_group1,1 ,Ӧ�óɹ�
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))

        # ����һ���û� ����Ϊ user,test,test,3,test_group1  Ӧ�óɹ�
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # ��¼�⻧  ��ȡ�⻧session �Ա�����ʹ��
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session,None,"failed to get user session!")

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
        self.assertNotEqual(cpu_id,None)
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))

        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,NO_CPU_INFO,cpu_id))

        # ע��image
        # �����ļ�����ȡ�ո��ϴ���ӳ���ļ���tecs�е�id
        i = image(server,root_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id,None,"failed to get image id!")
           
        # ------------------����Ա����Ȩ�޲���------------------
        # ʹ�� root_session �����в���  
        # ��������
        # ����Ϊ test_project_name, "testaaa"     Ӧ�óɹ�
        p = project(server,root_session)
        self.assertTrue(p.allocate(test_project_name,"testaaa"))
        # ���������Ĺ��̡�
        # ����Ϊ test_project_name, "testbbb"     Ӧ��ʧ��
        self.assertFalse(p.allocate(test_project_name,"testbbb"))
        # �鿴���̵�����
        # ����Ϊ 0,100,0,test_project_name,""    Ӧ��Ϊ "testaaa"
        self.assertEqual(p.query_des_by_name(test_project_name), "testaaa")
        # �޸Ĺ�������
        # ����Ϊ test_project_name, "testbbb"     Ӧ�óɹ�
        self.assertTrue(p.set(test_project_name,"testbbb"))
        # �鿴�޸ĵ�����
        # ����Ϊ 0,100,0,test_project_name,""    Ӧ��Ϊ "testbbb"
        self.assertEqual(p.query_des_by_name(test_project_name), "testbbb")
        # ���̲�ѯ����
        # ��ѯ�����Ĺ�������
        # ����Ϊ 0,100,-5,"",""     Ӧ�÷���1
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), 1)
        # ��ѯ�����û������Ĺ�������
        # ����Ϊ 0,100,-3,"",""     Ӧ�÷���1
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,ALL_USER_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), 1)
        # ��ѯ������ӵ�����������
        # ����Ϊ 0,30,test_project_name,""  Ӧ�÷���0
        self.assertEqual(p.query_vm_cout_by_project(VM_START_INDEX,VM_QUERY_COUNT,test_project_name,NO_USER_NAME), None)
        # ����ɾ������
        # ����Ϊ test_project_name,""  Ӧ�÷��سɹ�
        self.assertTrue(p.delete(test_project_name,NO_USER_NAME))

        # ------------------�⻧����Ȩ�޲���------------------    
        # ʹ�� user_session �����в��� 
        # ��������
        # ����Ϊ test_project_name, "testaaa"     Ӧ�óɹ�
        user_p = project(server,user_session)
        self.assertTrue(user_p.allocate(test_project_name,"testaaa"))
        # ���������Ĺ��̡�
        # ����Ϊ test_project_name, "testbbb"     Ӧ��ʧ��
        self.assertFalse(user_p.allocate(test_project_name,"testbbb"))
        # �鿴���̵�����
        # ����Ϊ 0,100,0,test_project_name,""    Ӧ��Ϊ "testaaa"
        self.assertEqual(user_p.query_des_by_name(test_project_name), "testaaa")
        # �޸Ĺ�������
        # ����Ϊ test_project_name, "testbbb"     Ӧ�óɹ�
        self.assertTrue(user_p.set(test_project_name,"testbbb" ))
        # �鿴�޸ĵ�����
        # ����Ϊ 0,100,0,test_project_name,""    Ӧ��Ϊ "testbbb"
        self.assertEqual(user_p.query_des_by_name(test_project_name), "testbbb")
        # ���̲�ѯ����
        # ��ѯ�����Ĺ�������
        # ����Ϊ 0,100,-5,"",""     Ӧ�÷���1
        self.assertEqual(user_p.query_count(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), 1)
        # ��ѯ�����û������Ĺ�������
        # ����Ϊ 0,100,-3,"",""     Ӧ�÷��� -1
        self.assertEqual(user_p.query_count(START_INDEX,QUERY_COUNT,ALL_USER_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), None)
        # ��ѯ������ӵ�����������
        # ����Ϊ 0,30,test_project_name,""  Ӧ�÷���0
        self.assertEqual(user_p.query_vm_cout_by_project(VM_START_INDEX,VM_QUERY_COUNT,test_project_name,NO_USER_NAME), None)
        # ����ɾ������
        # ����Ϊ test_project_name,""  Ӧ�÷��سɹ�
        self.assertTrue(user_p.delete(test_project_name,NO_USER_NAME))
        # ����Ϊ test_project_name,"admin"  Ӧ�÷���ʧ��
        self.assertFalse(user_p.delete(test_project_name,"admin"))


        # ------------------����Ա����Ȩ�޲���------------------
        # �����ò��Զ�ʹ�õĻ���
        # ʹ��user_session ����һ������
        # ����Ϊ test_project_name, "testaaa"     Ӧ�óɹ�
        user_p = project(server,user_session)
        self.assertTrue(user_p.allocate(test_project_name,"testaaa"))

        # ��ѯ�⻧�����Ĺ���
        # ��ѯ�û��Ĺ�������
        # ����Ϊ 0,100,-3,"",""   �����û���  Ӧ�÷��� 1
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,ALL_USER_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), 1)
        # ����Ϊ 0,100,0, ���� ,"user" ��ָ���û��� Ӧ���� None
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,APPOINTED_USER_PROJECT,NO_PROJECT_NAME,"user"), 1)
        # ����Ϊ 0,100,-5,"",""   ��ǰ�û���  Ӧ�÷��� 0
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,test_project_name, NO_USER_NAME), 0)
		# ����Ϊ 0,100,-5,"",""   ��ǰ�û��ռ�ָ��project  Ӧ�÷��� None
        self.assertEqual(p.query_count(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,test_project_name, NO_USER_NAME), None)
        # ɾ���⻧�����Ĺ���
        # ����Ϊ test_project_name,"user"  Ӧ�÷��سɹ�
        self.assertTrue(p.delete(test_project_name,"user"))

        # ------------------���������ز���------------------
        # ����Ա��ݽ��в���
        # ����һ�������
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

        self.assertTrue( p.allocate(test_project_name,"testaaa"))

        v = vmcfg(server,root_session)
        self.assertTrue(v.allocate(cfg) )
        # ɾ��ӵ��������Ĺ���
        # ����Ϊ test_project_name,""  Ӧ�÷���ʧ��
        self.assertFalse(p.delete(test_project_name,""))
        # �޸Ĺ�������
        # ����Ϊ test_project_name, "testbbb"     Ӧ�óɹ�
        self.assertTrue(p.set(test_project_name, "testbbb"))
        # ��ѯ��������ӵ�е��������Ϣ
        # ���ù����в�ѯ������Ľӿڣ���ѯ�������
        # ����Ϊ 0,30,test_project_name,""  Ӧ�÷��� None
		# ��ʱ��ע��  chenww 2012/12/12
        self.assertEqual(p.query_vm_name_by_project(VM_START_INDEX,VM_QUERY_COUNT,test_project_name,NO_USER_NAME), None)
		
	
        # ��������� Ӧ���ɹ�
		#��ѯ�����ID
        vm_id = v.get_vm_id(test_vm_name)
        self.assertTrue(v.action(vm_id,"deploy"))

        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)    


        # �ٴ���һ�������
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
        cfg_base["hypervisor"] = "xen"
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg["base_info"] = cfg_base
        cfg["vm_name"] = test_vm_name1
        cfg["project_name"] = test_project_name
        cfg["vm_num"] = 1

        vm = vmcfg(server,root_session)
        self.assertTrue(vm.allocate(cfg) )	

        # ��������� Ӧ���ɹ�
		#��ѯ�����ID
        vm_id1 = vm.get_vm_id(test_vm_name1)
        self.assertTrue(vm.action(vm_id1,"deploy"))

        # ��ѯ״̬    Ӧ����2���ɲ���5S��ѯһ�Σ�һ����ѯ10�Σ����10�ζ���ѯʧ�ܣ����˳���
        count = 0
        vm_state=U_STATE_UNKNOWN   
        while (vm.get_vm_state(vm_id1) != U_STATE_RUNNING and vm.get_vm_state(vm_id1)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)    

            
        # ��ѯ�����µľ�̬����
        # ����Ϊ 0,100,-3,"",""   �����û���  Ӧ�÷��� 1
        self.assertEqual(p.statistics_static_data(START_INDEX,QUERY_COUNT,ALL_USER_PROJECT,NO_PROJECT_NAME,NO_USER_NAME), True)
        # ����Ϊ 0,100,-4, ���� ,"user" ��ָ���û��� Ӧ���� None
        self.assertEqual(p.statistics_static_data(START_INDEX,QUERY_COUNT,APPOINTED_USER_PROJECT,NO_PROJECT_NAME,"admin"), True)
        # ����Ϊ 0,100,-5,"",""   ��ǰ�û���  Ӧ�÷��� 0
        self.assertEqual(p.statistics_static_data(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,test_project_name, NO_USER_NAME), True)
		# ����Ϊ 0,100,0,"",""   ��ǰ�û��ռ�ָ��project  Ӧ�÷��� None
        self.assertEqual(p.statistics_static_data(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,test_project_name, NO_USER_NAME), True)

	
        # ɾ�������
        v_id = v.get_vm_id(test_vm_name)
        v.action(v_id,"cancel")
        while (v.get_vm_state(v_id)!=U_STATE_CONFIGURATION and v.get_vm_state(v_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
                    
        self.assertTrue(v.action(v_id,"delete") )
        
        vm.action(vm_id1,"cancel")
        while (vm.get_vm_state(vm_id1)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id1)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)        
        self.assertTrue(vm.action(vm_id1,"delete") )        
        # ɾ������
        # ����Ϊ test_project_name,""  Ӧ�÷��سɹ�
        self.assertTrue(p.delete(test_project_name,NO_USER_NAME))

        #���Խ���
        print "success to test test_008_project.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(ProjectTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
