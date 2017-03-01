#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_009_host.py
# 测试内容：
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
#    修改内容：EC单611002999057 修改host属性改用set接口
# 修改记录3：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
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

def ci_exit(value=0,error="null"):
    '''exit function'''
    if value == 0:
        sys.exit(0)
    else:
        print "tecs ci test exit code = %d, error string = %s" % (value,error)
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

class HostTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testHost(self):
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

        # 创建一个用户组使用参数为 test_group1,1 ,应该成功
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))

        # 创建一个用户 参数为 user,test,test,3,test_group1  应该成功
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # 登录租户  获取租户session 以备后面使用
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session, None, "failed to get user session!")
        
        # 以管理员身份上传一个映像文件  应该成功  
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # 以租户身份上传一个映像文件  应该成功
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
        
        # 注册集群
        root_cluster_manager_method = clustermanager(server,root_session) 

        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # 等待 20S 等待资源上报
        #time.sleep(20)  

        # 创建工程
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
            
        # 设置tcu 8　
        root_host_method = host(server,root_session)
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id, None)
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))

        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,"",cpu_id))

        # 注册image
        # 根据文件名获取刚刚上传的映像文件在tecs中的id
        i = image(server,root_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id, None,"failed to get image id!")
          
        # ------------------管理员自身权限测试------------------
        # 测试注册
        # 注册主机     参数为 test_cluster,test_host,False,"test"  应该成功
        h = host(server,root_session)
        # 等待20S
        #time.sleep(20)
        # 查询主机的运行状态  参数为 0,100,test_cluster,test_host   应该为 3
        #刘毅cc整改时更改ON_LINE为2
        self.assertEqual(h.query_run_state(test_cluster,test_host), ON_LINE)
  
        # 测试使能
        # 使能主机 参数为  test_cluster,test_host  应该成功
        self.assertTrue(h.enable(test_cluster,test_host))

        # 查询主机的使能状态 参数为 0,100,test_cluster,test_host  应该为 0
        self.assertEqual(h.query_enable_state(test_cluster,test_host), HOST_DISABLE)
        # 测试禁能
        # 禁能主机 参数为  test_cluster,test_host,1  应该成功
        self.assertTrue(h.disable(test_cluster,test_host,HOST_ENABLE))
        # 查询主机的使能状态 参数为 0,100,test_cluster,test_host  应该为 1
        self.assertEqual(h.query_enable_state(test_cluster,test_host), HOST_ENABLE)

        #添加EC611002999057 修改host属性改用set接口
        self.assertTrue(h.set(test_cluster,test_host,"test"))

        #重复注册  应该成功
        self.assertTrue(h.set(test_cluster,test_host,"test"))

        # 注册主机  参数为 test_cluster,"","test"  应该失败（主机名为空）
        self.assertFalse(h.set(test_cluster,"","test"))
        # 注册主机  参数为 test_cluster,"no_exist","test"  应该失败（主机不存在）
        self.assertFalse(h.set(test_cluster,"no_exist","test"))  

        # 遗忘主机 参数为 test_cluster,test_host   应该失败
        self.assertTrue(h.forget(test_cluster,test_host))
        # 下面两条测试暂时不实现，因为要考虑到第3方HC的加入，现阶段只测试3合一的情况
        # 测试重启
        # 测试关机

        # ------------------租户自身权限测试------------------    
        user_h = host(server,user_session)

        # 测试使能
        # 使能主机 参数为  test_cluster,test_host  应该失败
        self.assertFalse(user_h.enable(test_cluster,test_host))
        
        # 测试禁能
        # 禁能主机 参数为  test_cluster,test_host,1  应该失败
        self.assertFalse(user_h.disable(test_cluster,test_host,HOST_ENABLE)) 

        # 测试遗忘
        # 遗忘主机 参数为 test_cluster,test_host   应该失败
        self.assertFalse(user_h.forget(test_cluster,test_host))
        # ------------------管理员其他权限测试------------------
        # 无


        # ------------------和虚拟机相关测试------------------
        time.sleep(20)
        
        # 以租户身份部署一个虚拟机，具体参数参考image 模块
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
        #部署虚拟机之前，打印集群资源，以便部署失败时查看信息
        cm = clustermanager(server,root_session) 
        self.assertTrue(cm.query(START_INDEX,QUERY_COUNT,SYN_MODE))
        #部署虚拟机
        self.assertTrue(v.action(v.get_vm_id(test_vm_name),"deploy"))

        # 等待2S，等待虚拟机部署完毕
        time.sleep(2)
        # 禁能主机 参数为  test_cluster,test_host  应该失败
        self.assertFalse(h.disable(test_cluster,test_host,HOST_DISABLE)) 

        # 查询主机运行的虚拟机数量  0,100,test_cluster,test_host  应该为 1
        self.assertEqual(h.query_running_vms(test_cluster,test_host), 1)
        #测试结束
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
