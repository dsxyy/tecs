#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_014_vm.py
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
#    修改日期：2012/06/8
#    版 本 号：V1.0
#    修 改 人：刘雪峰
#    修改内容：模块调试合入1
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
from tecs.vmtemplate import *
from tecs.tecssystem import *
from tecs.ssh_cmd import *





#用于测试的tc xmlrpc服务器地址
server_addr = "localhost"
#用于测试的tc xmlrpc服务器端口号
server_port = "8080"
ssh_port = "22"
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
#test_image2 = "ttylinux2.img"
#test_image3 = "ttylinux3.img"
#test_image4 = "ttylinux4.img"
#test_image5 = "ttylinux5.img"
#待测试的虚拟机名称
vm_001="vm_001"
vm_002="vm_002"
vm_003="vm_003"
#待测试的工程名称
test_project_name = "test_prj_1"
test_project_name2 = "test_prj_2"
#网络平面
test_netplane = "base"

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
# 自动化集成测试开始!!!
print "===================TECS CI TEST START==================="
# 创建一个xml rpc客户端，之所以使用xmlrpclibex，而非系统原装的xmlrpclib.py，是因为原装的不支持i8数据类型
server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',test_root_user,':',test_root_passwd,'@',server_addr,':',server_port,'/RPC2')
print "Connect to xmlrpc url %s ..." % server_url

try:
    server = xmlrpclibex.ServerProxy(server_url,allow_none=True)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")
#列出当前支持的所有xmlrpc接口
server.system.listMethods()
class VmTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testVm(self):
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
        self.assertNotEqual(user_session, None,"failed to get user session!")
        
        # 以管理员身份上传一个映像文件  应该成功        
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # 以租户身份上传一个映像文件  应该成功
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
      
      
        # 注册集群
        root_cluster_manager_method = clustermanager(server,root_session) 

        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # 注册主机
        root_host_method = host(server,root_session)
        # 等待 20S 等待资源上报
        #time.sleep(20) 

        self.assertTrue(root_host_method.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))


        # 设置tcu 8　
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)    
        self.assertNotEqual(cpu_id, None)
        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))
        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,NO_CPU_INFO,cpu_id))

        # 注册image
        # 根据文件名获取刚刚上传的映像文件在tecs中的id
        i = image(server,root_session)
        image_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(image_id, None, "failed to get image id!")
        self.assertTrue(i.publish(image_id,IS_PUBLIC))
        # 创建工程
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name2,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))

        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))

   
        # ------------------管理员自身权限测试------------------
        # 使用虚拟机模板创建虚拟机，在虚拟机模板测试流程中已经测试过了
        # test_project_name下直接创建虚拟机vm_001
        # 使用参数如下：     应当成功
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

        #获取管理员操作权限
        vm = vmcfg(server,root_session)
        self.assertTrue(vm.allocate(cfg))

        #创建完查询vm_id
        vm_id = vm.get_vm_id(vm_001)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        
        # 查看创建的虚拟机各参数是否和预期一致       
        # 查看VCPU    应该为1
        self.assertEqual(vm.query_vcpu(vm_id), 1)            
        # 查看tcu    应该为1
        self.assertEqual(vm.query_tcu(vm_id), 1) 
        # 查看内存大小 应该为128 * 1024 * 1024
        self.assertEqual(vm.query_memory(vm_id), (long(128 * 1024 *1024)))             
        # 查看machine 的id  应该为image_id
        self.assertEqual(vm.query_machine_id(vm_id),image_id) 
        # 查看machine 的bus  应该为 ide
        self.assertEqual(vm.query_machine_bus(vm_id), "ide")        
        # 查看machine 的type  应该为machine
        self.assertEqual(vm.query_machine_type(vm_id), "machine")
        # 查看machine 的target  应该为hda
        self.assertEqual(vm.query_machine_target(vm_id), "hda")
        # 查看虚拟化类型  应该为hvm
        self.assertEqual(vm.query_virt_type(vm_id), "hvm")
        # 查看磁盘大小   应该为 128 * 1024 * 1024
        self.assertEqual(vm.query_disks_size(vm_id), (long(128 * 1024 *1024))) 
        # 查看磁盘总线   应该为 ide
        self.assertEqual(vm.query_disks_bus(vm_id), "ide")
        # 查看磁盘的target   应该为 hdb
        self.assertEqual(vm.query_disks_target(vm_id), "hdb")
        # 查看磁盘的position   应该为 0
        self.assertEqual(vm.query_disks_position(vm_id), 1)
        # 查看磁盘的fstype   应该为 ext3
        self.assertEqual(vm.query_disks_fstype(vm_id), "ext3")

        # 查看虚拟机的名称  应该为 vm_001
        self.assertEqual(vm.query_vmname(vm_id), "vm_001")        
        # 查看虚拟机的归属工程  应该为 test_project_name
        self.assertEqual(vm.query_projectname(vm_id), "test_prj_1")  
        # 修改虚拟机配置测试
        # 修改vlan值为1，应该成功
       # cfg_base["nics"] = [{"pci_order":0,"plane":test_netplane,"vlan":1,"vlantrunk":bool(0), "dhcp":bool(1),"ip":"","netmask":"","gateway":"","mac":""}]
        cfg["vid"] = vm_id
        self.assertTrue(vm.set(cfg))  
       
        # test_project_name使用如下参创建虚拟机2(需要获取虚拟机的id,修改接口需要虚拟机id)，  应该成功
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

        #创建完查询vm_id
        vm_id = vm.get_vm_id(vm_002)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        # 查看创建的虚拟机各参数是否和预期一致
        # 查看VCPU    应该为2
        self.assertEqual(vm.query_vcpu(vm_id), 2)
        # 查看tcu    应该为2
        self.assertEqual(vm.query_tcu(vm_id), 2)        
        # 查看内存大小 应该为 200 * 1024 * 1024
        self.assertEqual(vm.query_memory(vm_id), (long(200 * 1024 *1024)))  
        # 查看machine 的bus  应该为"scsi"
        self.assertEqual(vm.query_machine_bus(vm_id), "ide") 
        # 查看machine 的type  应该为machine
        self.assertEqual(vm.query_machine_type(vm_id), "machine") 
        # 查看machine 的target  应该为xvdb
        self.assertEqual(vm.query_machine_target(vm_id), "xvdb") 
        # 查看虚拟化类型  应该为pvm
        self.assertEqual(vm.query_virt_type(vm_id), "pvm") 
        # 查看磁盘大小   应该为 200 * 1024 * 1024
        self.assertEqual(vm.query_disks_size(vm_id), (long(200 * 1024 *1024)))
        # 查看磁盘总线   应该为 "scsi"
        self.assertEqual(vm.query_disks_bus(vm_id), "scsi")
        # 查看磁盘的target   应该为 xvdb
        self.assertEqual(vm.query_disks_target(vm_id), "xvdb")
        # 查看磁盘的position   应该为 1
        self.assertEqual(vm.query_disks_position(vm_id), 1)
        # 查看磁盘的fstype   应该为 ext2
        self.assertEqual(vm.query_disks_fstype(vm_id), "ext2")
       
        # 查看虚拟机的名称  应该为 vm_002
        self.assertEqual(vm.query_vmname(vm_id), "vm_002")  
        # 查看虚拟机的归属工程  应该为 test_project_name
        self.assertEqual(vm.query_projectname(vm_id), "test_prj_1")
        # 错误参数创建测试
        # 调用创建接口，参数如下(machine type  错)      应该失败
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

        # 调用创建接口，参数如下 (machine id 错误)     应该失败
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

        # 调用创建接口，参数如下 (disk id 错误)     应该失败
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

        # 调用创建接口，参数如下 (disk 总线类型错误)     应该失败
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

        # 调用创建接口，参数如下 (disk 格式化类型错误)     应该失败
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

        # 调用创建接口，参数如下 (工程不存在)     应该失败
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

        # 增加映像2，类型为Datablock，使能， 应当成功	 chenww
#        i2 = image(server,root_session)
#        image_id2 = i2.get_id_by_filename(os.path.basename(test_image2))
#        self.assertNotEqual(image_id2, None, "failed to get image id!")  
#        self.assertTrue(i2.enable(image_id2,I_ENABLE))
        # 增加映像3，类型为Datablock，禁用， 应当成功	 chenww
#        i3 = image(server,root_session)
#        image_id3 = i3.get_id_by_filename(os.path.basename(test_image3))
#        self.assertNotEqual(image_id3, None, "failed to get image id!")  
#        self.assertTrue(i3.enable(image_id3,I_DISABLE))
        # 增加映像4，类型为Cdrom，使能， 应当成功	 chenww
#        i4 = image(server,root_session)
#        image_id4 = i4.get_id_by_filename(os.path.basename(test_image4))
#        self.assertNotEqual(image_id4, None, "failed to get image id!")  
#        self.assertTrue(i4.enable(image_id4,I_ENABLE))
        # 增加映像5，类型为Cdrom，禁用， 应当成功	 chenww
#        i5 = image(server,root_session)
#        image_id5 = i5.get_id_by_filename(os.path.basename(test_image5))
#        self.assertNotEqual(image_id5, None, "failed to get image id!")  
#        self.assertTrue(i5.enable(image_id5,I_DISABLE))
        #新建虚拟机， 基于映像2创建Datablock类型的磁盘， 应当成功  chenww
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
        #新建虚拟机， 基于映像3创建Datablock类型的磁盘， 应当失败  chenww
#        disk3 = [{"id":image_id3,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk3]
#        self.assertFalse(v.allocate(cfg))  
        #新建虚拟机， 基于映像4创建cdrom类型的磁盘， 应当成功  chenww
#        disk4 = [{"id":image_id4,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk4]
#        self.assertTrue(v.allocate(cfg)) 
        #新建虚拟机， 基于映像5创建Cdrom类型的磁盘， 应当失败  chenww
#        disk5 = [{"id":image_id5,"size":long(200 * 1024 * 1024),"bus":"scsi","type":"disk","target":"hda","position":1,"fstype":"ext2"}]
#        cfg["base_info"]["disks"] = [disk5]
#        self.assertFalse(v.allocate(cfg)) 
        # 操作虚拟机测试
        # 先使用修改接口 修正参数  参数如下   应该成功
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
    
        #查询虚拟机ID
        vm_id = vm.get_vm_id(vm_003)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")

        #等待tc上集群资源信息到位 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(0L,100L,1) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)       
        # 部署虚拟机 应当成功
        self.assertTrue(vm.action(vm_id,"deploy"))

        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN   
        while (vm.get_vm_state(vm_id) != U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)    
        # 停止虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"stop"))
        # 查询状态    应该是4（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_SHUTOFF and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)        
        # 启动虚拟机   应当成功
        self.assertTrue(vm.action(vm_id,"start"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)  
        # 挂起虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"pause"))
        # 查询状态    应该是3（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_PAUSED and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 恢复虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"resume"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 重启虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"reboot"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_RUNNING and vm.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 取消虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"cancel"))
		
	# 查询状态    应该是2（可采用5S查询一次，一共查询16次，如果16次都查询失败，则退出）
        count = 0
        vm_state=2       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # 删除虚拟机  应当成功
        self.assertTrue(vm.action(vm_id,"delete"))
        # 查询虚拟机数量 应该为2
        self.assertEqual(vm.query_count(-5L), 2)
        # 使用之前创建成功的虚拟机参数创建2个虚拟机（名称一样）
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
        # 使用查询接口 vmcfg_query 进行虚拟机数量查询
        # 参数为 0,30,-5     应该为 4
        self.assertEqual(v.query_count(-5L), 4)
        # 参数为 0,30,创建时候生成的id     应该成功
        self.assertTrue(v.query(vm_id))
        # 参数为 0,30,不存在的虚拟机id     应该失败
        self.assertFalse(v.query(10L))
        # 使用查询接口 vmcfg_fullquery 进行虚拟机数量查询
        # 参数为 0,30,-5,"",test_project_name,-1     应该为 4
        self.assertTrue(v.fullquery(-5L,"",test_project_name,0L,30L),4)
        # 参数为 0,30,-5,"","",-1     应该为 4
        self.assertTrue(v.fullquery(-5L,"","",0L,30L),4)
        # 参数为 0,30,-5,"",test_project_name2,-1     应该为 0
        self.assertTrue(v.fullquery(-5L,"",test_project_name2,0L,30L),0)
        # 参数为 0,30,-4,"admin","",-1     应该为 4
        self.assertTrue(v.fullquery(-4L,"admin",test_project_name,0L,30L),4)
        # 参数为 0,30,-4,"user","",-1     应该为 0
        self.assertTrue(v.fullquery(-4L,"user",test_project_name,0L,30L),0)
        # 参数为 0,30,-3,"","",-1     应该为 4
        self.assertTrue(v.fullquery(-3L,"","",0L,30L),4)
        # 使用查询接口 vmcfg_vm_syn 进行虚拟机数量查询
        # 参数为 这次创建的虚拟机id1, 这次创建的虚拟机id2   返回结果应该是2个虚拟机配置态

        # 使用 vmcfg_get_id_by_name 查询
        # 参数为 -5,vm_001,""   应该返回3
        self.assertTrue(v.get_id_by_name(-5L,"vm_001",""),3)
        # 参数为 -4,vm_001,"admin"   应该返回3
        self.assertTrue(v.get_id_by_name(-4L,"vm_001","admin"),3)
        # 参数为 -3,vm_001,""   应该返回3
        self.assertTrue(v.get_id_by_name(-3L,"vm_001",""),4)
        # 设置虚拟机依赖关系测试(id1, id2为上面创建的2个虚拟机id)
        # 参数如下                       应当成功
        cfg = {}
        cfg["type"] = 0
        #cfg["vec_vm"] = [{id1, id2}]
        # 查询虚拟机依赖关系
        # 参数为 0     返回结果为有1对依赖互斥关系
        # 参数为id1   返回互斥关系的另一个id 为 id2
        # 删除虚拟机依赖关系
        # 参数如下                       应当成功
        cfg = {}
        cfg["type"] = 1
        #cfg["vec_vm"] = [{id1, id2}]
        # 查询虚拟机依赖关系
        # 参数为 0     返回结果为有0对依赖互斥关系

        # 参数为id1   返回为NULL

        # ------------------租户自身权限测试------------------
        user_v = vmcfg(server,user_session)
        # 直接创建虚拟机
        # 租户在test_project_name下使用参数如下：     应当成功
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
        
        # 租户在test_project_name下创建vm_002：     应当成功
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

        # 租户在test_project_name2下创建vm_001：    应当成功？
        cfg["project_name"] = test_project_name2
        cfg["vm_name"] = vm_001
        self.assertTrue(user_v.allocate(cfg))
        
        # 修改虚拟机配置测试
        # 使用如下参数(需要获取虚拟机的id,修改接口需要虚拟机id)，  应该成功
        vm_id = user_v.get_vm_id(vm_001)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")

        # 操作虚拟机测试
        # 先使用修改接口 修正参数  参数如下   应该成功
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
        # 部署虚拟机   应当成功
        self.assertTrue(user_v.action(vm_id,"deploy"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=6   
        while (user_v.get_vm_state(vm_id) != U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)        
        # 停止虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"stop"))
        # 查询状态    应该是4（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_SHUTOFF and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 启动虚拟机   应当成功
        self.assertTrue(user_v.action(vm_id,"start"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 挂起虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"pause"))
        # 查询状态    应该是3（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_PAUSED and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 恢复虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"resume"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 重启虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"reboot"))
        # 查询状态    应该是2（可采用5S查询一次，一共查询10次，如果10次都查询失败，则退出）
        count = 0
        vm_state=6       
        while (user_v.get_vm_state(vm_id)!=U_STATE_RUNNING and user_v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        # 取消虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"cancel"))
        # 查询虚拟机数量 应该为3
        self.assertEqual(user_v.query_count(-5L), 3)
        # 延时3分钟
        time.sleep(180)
        
        # 查询状态    应该是2（可采用5S查询一次，一共查询16次，如果16次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # 删除虚拟机  应当成功
        self.assertTrue(user_v.action(vm_id,"delete"))
        # 使用之前创建成功的虚拟机参数创建2个虚拟机（名称一样）
        # 使用查询接口 vmcfg_query 进行虚拟机数量查询
        # 参数为 0,30,创建时候生成的id     应该成功
        #self.assertTrue(user_v.query(vm_id))
        # 参数为 0,30,不存在的虚拟机id     应该失败
        self.assertFalse(user_v.query(23L))
        # 使用查询接口 vmcfg_fullquery 进行虚拟机数量查询
        # 参数为 0,30,-5,"",test_project_name,-1     应该为 3
        self.assertTrue(user_v.fullquery(-5L,"",test_project_name,0L,30L),3)
        # 参数为 0,30,-5,"","",-1     应该为 3
        self.assertTrue(user_v.fullquery(-5L,"","",0L,30L),3)
        # 参数为 0,30,-5,"",test_project_name2,-1     应该为 0
        self.assertTrue(user_v.fullquery(-5L,"","test_project_name2",0L,30L),0)
        # 参数为 0,30,-5,"","",0     应该为 3
        self.assertTrue(user_v.fullquery(-5L,"","",0L,30L),3)
        # 参数为 0,30,-4,"admin","",-1     应该失败
        self.assertFalse(user_v.fullquery(-4L,"admin","",0L,30L),0)
        # 参数为 0,30,-4,"user","",-1     应该失败
        self.assertFalse(user_v.fullquery(-4L,"user","",0L,30L),0)
        # 参数为 0,30,-3,"","",-1     应该失败
        self.assertFalse(user_v.fullquery(-3L,"admin","",0L,30L))
        # 使用查询接口 vmcfg_vm_syn 进行虚拟机数量查询
        # 参数为 这次创建的虚拟机id3, 这次创建的虚拟机id4   返回结果应该是2个虚拟机配置态
        # 使用 vmcfg_get_id_by_name 查询
        # 参数为 -5,vm_001,""   应该返回3
        self.assertTrue(user_v.get_id_by_name(-5L,"vm_001",""),3)
        # 参数为 -4,vm_001,"admin"   应该失败
        self.assertFalse(user_v.get_id_by_name(-4L,"vm_001","admin"))
        # 参数为 -4,vm_001,"user"   应该失败
        self.assertFalse(user_v.get_id_by_name(-4L,"vm_001","user"))
        # 参数为 -3,vm_001,""   应该失败
        self.assertFalse(user_v.get_id_by_name(-3L,"vm_001",""))
        # 设置虚拟机依赖关系测试(id3, id4为上面创建的2个虚拟机id)
        # 参数如下                       应当成功
        #cfg["vec_vm"] = [{id3, id4}]

        # 查询虚拟机依赖关系
        # 参数为 0     返回结果为有1对依赖互斥关系

        # 参数为id3   返回互斥关系的另一个id 为 id4

        # 删除虚拟机依赖关系
        # 参数如下                       应当成功
#        cfg = {}
#        cfg["type"] = 1
        #cfg["vec_vm"] = [{id3, id4}]

        # 查询虚拟机依赖关系
        # 参数为 0     返回结果为有0对依赖互斥关系

        # 参数为id3   返回为NULL


        # ------------------管理员其他权限测试------------------
        # 测试修改租户的信息
        # 使用上面 租户创建的id3 来进行修改，随便修改其中一个参数，具体参数见上面的例子，   应该失败
        #获取管理员操作权限
        vm = vmcfg(server,root_session)
        cfg["vid"] = vm_id
        self.assertFalse(vm.set(cfg))
        # 测试查询租户的信息（使用租户的权限）
        # 使用查询接口 vmcfg_fullquery 进行虚拟机数量查询
        # 参数为 0,30,-5,"",test_project_name,-1     应该为 2

        # 参数为 0,30,-5,"","",-1     应该为 2

        # 参数为 0,30,-5,"",test_project_name2,-1     应该为 0

        # 参数为 0,30,-5,"","",0     应该为 2

        # 参数为 0,30,-5,"","",1     应该为 0

        # 参数为 0,30,-4,"admin","",-1     应该2

        # 参数为 0,30,-4,"user","",-1     应该2

        # 参数为 0,30,-3,"","",-1     应该4

        # 使用查询接口 vmcfg_vm_syn 进行虚拟机数量查询
        # 参数为 id1,id2,id3,id4   返回4个虚拟机的状态均为配置态

        # 使用 vmcfg_get_id_by_name 查询
        # 参数为 -5,vm_001,""   应该返回2

        # 参数为 -4,vm_001,"admin"   应该2

        # 参数为 -4,vm_001,"user"   应该2

        # 参数为 -3,vm_001,""   应该4


        # 测试部署租户的虚拟机(使用的配置为租户创建的虚拟机id3)
        # 部署虚拟机   应当失败

        # 使用租户角色部署id3

        # 停止虚拟机  应当失败

        # 启动虚拟机   应当失败

        # 挂起虚拟机  应当失败

        # 恢复虚拟机  应当失败

        # 重启虚拟机  应当失败

        # 取消虚拟机  应当成功

        # 延时3分钟

        # 查询状态    应该是0（可采用5S查询一次，一共查询3次，如果3次都查询失败，则退出）

        # 删除虚拟机  应当成功
        
        #测试结束
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


