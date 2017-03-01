#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001.py
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
# 修改记录2：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
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
print "===================TECS CI TEST START==================="        
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url
try:
    server = xmlrpclibex.ServerProxy(server_url)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口    
server.system.listMethods()

class SmokingTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testSmoking(self):  
        # ------------------环境准备------------------
        # 登录tecs，获取管理员账户加密后的session
        root_session = login(server,test_root_user,test_root_passwd)
        self.assertNotEqual(root_session,None,"failed to get encrypted root session!")

        #系统运行状态查询 EC:611003077308 tecs提供rpc接口查询系统运行时信息
        #此处延时20s，否则hc的MU_VM_HANDLER和MU_HOST_HEARTBEAT不存在
        time.sleep(100)
        #必须是管理员账户
        sys_runtime_query = tecssystem(server,root_session)
        #查询TC状态
        self.assertEqual(int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,DEFAULT_TC)),RUNTIME_STATE_NORMAL,"tc status is abnormal")
        #查询CC状态
        self.assertEqual(int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cluster)),RUNTIME_STATE_NORMAL,"cc status is abnormal")
        #查询HC状态
        test_cc_hc = test_cluster + "," + test_host
        hc_state = int(sys_runtime_query.get_runtime_state(TYPE_CACULATE,test_cc_hc))
        self.assertTrue(hc_state >= RUNTIME_STATE_NORMAL and hc_state <= RUNTIME_STATE_WARNNING,"hc status is abnormal")

        # 注册cc到tc
        print "=============================================="
        cm = clustermanager(server,root_session) 

        #TODO 需要增加循环等待
        count=0
        while (cm.get_cluster_max_count() == 0):
            time.sleep(1)
            count = count + 1 
            self.assertTrue(count < 19)         
        print "get_cluster_max_count count= %d "% count  
        
        #查询集群列表
        self.assertTrue(cm.query(START_INDEX,QUERY_COUNT,ASYN_MODE))

        # 注册hc到cc
        print "=============================================="
        h = host(server,root_session)

        #TODO 需要增加循环等待    
        self.assertTrue(h.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))

        # 设置tcu 
        print "=============================================="
        # 获取刚刚注册的主机的cpu id   
        cpu_id = h.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id,None)        
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        s = tcu(server,root_session)
        self.assertTrue(s.set(test_cluster,cpu_id,TCU_NUM2,"test cpu description"))
        self.assertTrue(s.query(START_INDEX,QUERY_COUNT,test_cluster,"",cpu_id))  

        # 创建一个类型为1的用户组，用于容纳普通租户的账号 
        print "=============================================="
        ug = usergroup(server,root_session)
        self.assertTrue(ug.allocate(test_tenant_usergroup,GROUP_TYPE1))
            
        # 创建一个普通账号用于后续模拟来自租户的操作 
        u = user(server,root_session)
        self.assertTrue(u.allocate(test_tenant_user,test_tenant_passwd,test_tenant_passwd,ACCOUNTUSER,test_tenant_usergroup))
            
        # 使用普通租户账户登录tecs，获取该账户加密后的session
        tenant_session = login(server,test_tenant_user,test_tenant_passwd)
        self.assertNotEqual(tenant_session,None,"failed to get encrypted tenant session!")    
        # 使用ftp客户端上传一个虚拟机machine映像
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

        # 根据文件名获取刚刚上传的映像文件在tecs中的id
        time.sleep(1)
        i = image(server,tenant_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id,None,"failed to get image id!")
        # 设置一个网络平面

        # 查询网络平面名称  不需要和设置的一样，有可能是下面上报的

        # 配置DHCP

        # 创建工程
        p = project(server,tenant_session)
        self.assertTrue(p.allocate(test_project_name,"test project description"))

        # 创建虚拟机
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

        # 根据名称获取刚刚创建的虚拟机的id    
        vm_id = v.get_vm_id(test_vm_name)
        self.assertNotEqual(vm_id,None,"failed to get vm id!")
        # 获取虚拟机状态
        vmstate = v.get_vm_state(vm_id)
        self.assertNotEqual(vmstate,None,"failed to get vm state!")            
        #增加一个磁盘
        disk1 = {"id":-1L,"size":long(512 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        disks = [disk1]
        self.assertTrue(v.set_disk(vm_id,disks))
                
        #增加一个网卡
        #nic1={"pci_order":0,"plane":"","vlan":0,"dhcp":bool(1),"ip":"10.43.180.250","netmask":"255.255.254.0","gateway":"10.43.180.1","mac":""}
        #nics = [nic1]
        #if v.set_nic(vm_id,nics) != True:
        #    ci_exit_ex(-1,line())
         
        #等待tc上集群资源信息到位 
        count = 0   
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query count = %d " %count

        print "waiting for hc power on ..."    
        time.sleep(180)   
         
        # 部署虚拟机
        self.assertTrue(v.action(vm_id,"deploy"))
            
        # 获取虚拟机状态   应该是部署成功态，状态6为位置状态
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)
  
        # 重启虚拟机
        self.assertTrue(v.action(vm_id,"reboot"))

        # 获取虚拟机状态，应该为2    
        count = 0
        vm_state = U_STATE_UNKNOWN  
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)

        # 强制重启虚拟机
        self.assertTrue(v.action(vm_id,"reset"))
            
        # 获取虚拟机状态，应该为2    
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)      
                
        # 取消虚拟机
        self.assertTrue(v.action(vm_id,"cancel"))    
        #取消需要3分钟，这里延时200秒
        print "wait 200s" 
        time.sleep(200)
        # 获取虚拟机状态，应该为0 
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_CONFIGURATION and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17) 
                
        #再次部署虚拟机
        self.assertTrue(v.action(vm_id,"deploy"))
        # 获取虚拟机状态，应该为2
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count < 17)       
            
        # 检测磁盘是否存在

        # 检测网络是否通

        # cancle虚拟机  应该成功

        #测试结束
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
