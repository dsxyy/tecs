#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001_reboot_hc_check_vm.py
# 测试内容：测试虚拟机创建部署流程
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
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

#用于测试的tc xmlrpc服务器地址
server_addr = "localhost"
#用于测试的tc xmlrpc服务器端口号
server_port = "8080"
# tc/cc/hc的名字需要和start.sh中的定义保持一致！
#用于测试的tc application，即云名称
test_cloud = "tecscloud"
#用于测试的cc application，即集群名称
test_cluster = "tecscluster"
#用于测试的hc application，即物理机名称
test_host = "citesthost"
#管理员账号和密码
test_root_user = "admin"
test_root_passwd = "admin"
#普通用户组名称
test_tenant_usergroup = "cigroup"
#租户账号和密码
test_tenant_user = "citest"
test_tenant_passwd = "citest"
#用于上传，创建虚拟机的映像文件路径
test_image = "ttylinux.img"
#待测试的虚拟机名称
test_vm_name = "ttylinux"
#待测试的工程名称
test_project_name = "test_prj_1"

def usage():
    ''' 打印帮助信息'''
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

# 解析处理命令行参数
try:
    opts, args = getopt.getopt(sys.argv[1:], "h:a:p:i:v", ["help", "addr=","port=","image="])
except getopt.GetoptError:
    # print help information and exit:
    usage()
    ci_exit(-2,"GetoptError")
    
for o, v in opts:
    #用-h或--help打印帮助
    if o in ("-h", "--help"):
        usage()
    #用-a或--addr可以指定tc的xmlrpc服务器地址，默认为localhost
    if o in ("-a", "--addr"):
        server_addr = v
    #用-p或--port可以指定tc的xmlrpc服务器端口号，默认为8080
    if o in ("-p", "--port"):
        server_port = v
    #用-i或--image可以指定虚拟机所用的映像文件
    if o in ("-i", "--image"):
        test_image = v
        if os.path.isfile(test_image) is not True:
            ci_exit(-2,"image file %s not exist!" % test_image)
        
# 自动化集成测试开始!!!
print "===================TECS CI TEST_001_reboot_hc_check_vm==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit_ex(-1,line(),"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口    
server.system.listMethods()

class RebootVmTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testRebootVm(self):  
        # 使用普通租户账户登录tecs，获取该账户加密后的session
        tenant_session = login(server,test_tenant_user,test_tenant_passwd)
        self.assertNotEqual(tenant_session,None,"failed to get encrypted tenant session!")
        
        v = vmcfg(server,tenant_session)
        # 根据名称获取刚刚创建的虚拟机的id    
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
        #611003186498 虚拟机强制关机按钮实现    
        #强制关机 应该成功
        self.assertTrue(v.action(vm_id,"destroy"))
        # 获取虚拟机状态，应该为关机
        count=0
        vm_state = v.get_vm_state(vm_id) 
        while (vm_state != U_STATE_SHUTOFF):          
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1       
            self.assertTrue(count < 3)   
           
        # 删除虚拟机  
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
        #测试结束
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
