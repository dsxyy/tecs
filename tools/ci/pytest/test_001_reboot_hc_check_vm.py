#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001_reboot_hc_check_vm.py
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
print "===================TECS CI TEST_001_reboot_hc_check_vm==================="        
# ����һ��xml rpc�ͻ��ˣ�֮����ʹ��xmlrpclibex������ϵͳԭװ��xmlrpclib.py������Ϊԭװ�Ĳ�֧��i8��������
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit_ex(-1,line(),"failed to create xmlrpc ServerProxy!")

#�г���ǰ֧�ֵ�����xmlrpc�ӿ�    
server.system.listMethods()

class RebootVmTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testRebootVm(self):  
        # ʹ����ͨ�⻧�˻���¼tecs����ȡ���˻����ܺ��session
        tenant_session = login(server,test_tenant_user,test_tenant_passwd)
        self.assertNotEqual(tenant_session,None,"failed to get encrypted tenant session!")
        
        v = vmcfg(server,tenant_session)
        # �������ƻ�ȡ�ոմ������������id    
        vm_id = v.get_vm_id(test_vm_name)
        self.assertNotEqual(vm_id,None,"failed to get vm id!")

        count = 0
        vm_state =  v.get_vm_state(vm_id)
        while (vm_state != U_STATE_RUNNING):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)  
        #611003186498 �����ǿ�ƹػ���ťʵ��    
        #ǿ�ƹػ� Ӧ�óɹ�
        self.assertTrue(v.action(vm_id,"destroy"))
        # ��ȡ�����״̬��Ӧ��Ϊ�ػ�
        count=0
        vm_state = v.get_vm_state(vm_id) 
        while (vm_state != U_STATE_SHUTOFF):          
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1       
            self.assertTrue(count < 3)   
           
        # ɾ�������  
        self.assertTrue(v.action(vm_id,"cancel"))
        time.sleep(10)
        count = 0
        vm_state = v.get_vm_state(vm_id)   
        while (vm_state != U_STATE_CONFIGURATION and vm_state != None):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)
                    
        self.assertTrue(v.action(vm_id,"delete"))   

        vm_id = v.get_vm_id(test_vm_name)
        self.assertEqual(vm_id,None,"vm has been deleted ,get vm id need fail!")
        #���Խ���
        print "success to test test_001_reboot_hc_check_vm.py"         
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(RebootVmTestCase())
                return suite
                
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
