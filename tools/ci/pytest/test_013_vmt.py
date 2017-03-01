#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_013_vmt.py
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
#    修改内容：添加611003050917 模板管理中创建或修改看门狗时长修改不成功，始终是0。
# 修改记录3：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
# 修改记录4：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611002997307 创建模板时，如果名称冲突，报的是参数错误，不直观
# 修改记录5：
#    修改日期：2012/07/19
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：#EC611003017919 创建16个硬盘的vm无法部署，提示Schedule failed to allocate vid
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
#待测试的虚拟机名称
test_vm_name1 = "ttylinux1"
test_vm_name2 = "ttylinux2"
test_vm_name3 = "ttylinux3"
test_vt_name = "ttylinux_vt"
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
    server = xmlrpclibex.ServerProxy(server_url,allow_none=True)
except:
    ci_exit(-1,"failed to create xmlrpc ServerProxy!")

#列出当前支持的所有xmlrpc接口
server.system.listMethods()

class VmtTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testVmt(self):
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
        
        # 创建工程
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
            
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
        
        # ------------------管理员自身权限测试------------------
        # 创建模板测试
        # 测试使用快速创建接口来进行创建参数如下：   应该成功
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
        # 发布image
        #Vmtemplate cannot public, for Image( -1 ) not public
        self.assertTrue(i.publish(image_id,IS_PUBLIC))
           
        # 创件模板：   应该成功 
        self.assertTrue(vt.allocate(cfg))
        #EC611002997307 创建模板时，如果名称冲突，报的是参数错误，不直观
        #创建相同名称的模板，返回错误参数
        self.assertFalse(vt.allocate(cfg))
        # 删除虚拟机模板 参数为 test_vt_name   应当成功
        self.assertTrue(vt.del_vmt(test_vt_name))
        # 测试使用快速创建接口来进行创建参数如下：   应该成功 
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
        # 删除虚拟机模板 参数为 test_vt_name   应当成功
        self.assertTrue(vt.del_vmt(test_vt_name))
        # 测试使用快速创建接口来进行创建参数如下：   应该成功 
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
        # 创建磁盘  参数如下    应当成功 
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
        # test_vt_name 和 disks = [disk1]
        cfg["base_info"]["disks"] = [disk1]
        self.assertTrue(vt.set(cfg))
        # 创建磁盘  参数如下    应当成功
        disk2 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdb","position":1,"fstype":"ext4"}
        # test_vt_name 和 disks = [disk2]
        cfg["base_info"]["disks"] = [disk2]
        self.assertTrue(vt.set(cfg))
        # 创建磁盘  参数如下    应当成功
        disk3 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdd","position":2,"fstype":"ext4"}
        # test_vt_name 和 disks = [disk3]
        cfg["base_info"]["disks"] = [disk3]
        self.assertTrue(vt.set(cfg))
        # 创建image disk    应当成功,ramdisk暂时没有使用，不测试
        disk4 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"datablock","target":"sde"}
        #if vt.set(cfg) != True:
        #    ci_exit_ex(-1,line())
        # 创建image disk    应当成功,ramdisk暂时没有使用，不测试
        disk5 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"cdrom","target":"sdf"}
        #if vt.set(cfg) != True:
        #    ci_exit_ex(-1,line())
        # 创建image disk    应当失败，,ramdisk暂时没有使用，不测试
        disk6 = {"id":image_id,"position":1,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdf"}
        #if vt.set(cfg) != False:
        #    ci_exit_ex(-1,line())
        # 创建磁盘  参数如下    应当失败，
        disk7 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdd","position":2,"fstype":"fat32"}
        # test_vt_name 和 disks = [disk7]
        cfg["base_info"]["disks"] = [disk7]
        self.assertFalse(vt.set(cfg))
        # 创建磁盘  参数如下    应当成功，
        disk8 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"cdrom","target":"sdd","position":2,"fstype":"ext4"}
        # test_vt_name 和 disks = [disk8]
        cfg["base_info"]["disks"] = [disk8]
        self.assertTrue(vt.set(cfg))
        # 创建磁盘  参数如下    应当成功
        disk9 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"scsi","type":"disk","target":"sdg","position":2,"fstype":"ext2"}
        # test_vt_name 和 disks = [disk9]
        cfg["base_info"]["disks"] = [disk9]
        self.assertTrue(vt.set(cfg))
        # 创建磁盘  参数如下    应当成功
        # test_vt_name 和 disks = [disk1 disk2 disk3 disk4 disk9]
        cfg["base_info"]["disks"] = [disk1,disk2,disk3,disk9]
        self.assertTrue(vt.set(cfg))
        # 创建磁盘  参数如下    应当失败
        # test_vt_name 和 disks = [disk1 disk1]
        cfg["base_info"]["disks"] = [disk1,disk1]
        self.assertFalse(vt.set(cfg))
        # 删除虚拟机模板 参数为 test_vt_name   应当成功
        self.assertTrue(vt.del_vmt(test_vt_name))

        self.assertTrue(i.publish(user_image_id,NOT_PUBLIC))

        # 创建网口 参数如下    应当成功
        #EC611003123662 TC数据库整改 gaom 2012-07-30 增加nic_index字段用例
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
        #创建模板，应该成功
        self.assertTrue(vt.allocate(cfg))
        nic_index = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_FIRST_NIC)
        self.assertNotEqual(nic_index, 0)
        # 删除虚拟机模板 参数为 test_vt_name   应当成功
        self.assertTrue(vt.del_vmt(test_vt_name))
        #创建两个相同的nic_index网卡的模板，应该失败
        nic1 = {"nic_index":1,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        nic2 = {"nic_index":1,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        cfg["base_info"]["nics"] =[nic1,nic2]
        self.assertFalse(vt.allocate(cfg))
        #创建两个不同的nic_index网卡的模板，应该成功
        nic1 = {"nic_index":0,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        nic2 = {"nic_index":2,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}  
        cfg["base_info"]["nics"] =[nic1,nic2]
        self.assertTrue(vt.allocate(cfg))
        #查询两个网卡的index，应该不同    
        #nic_index0 = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_FIRST_NIC)
        #nic_index1 = vt.query_nics_index(QUERY_BY_VT_NAME,test_vt_name,QUERY_SECOND_NIC)
        #self.assertNotEqual(nic_index0, nic_index1)
        #删除模板 成功
        self.assertTrue(vt.del_vmt(test_vt_name))
         
        #添加EC611003050917：模板管理中创建或修改看门狗时长修改不成功，始终是0
        # 修改vm_wdtime为0    应该失败
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
        # 查询vm_wdtime：   应该为0
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name),0)

        # 修改vm_wdtime为1    应该失败
        cfg["base_info"]["vm_wdtime"] = 1
        self.assertFalse(vt.set(cfg))

        # 修改vm_wdtime为599    应该失败
        cfg["base_info"]["vm_wdtime"] = 599
        self.assertFalse(vt.set(cfg))

        # 修改vm_wdtime为600    应该成功
        cfg["base_info"]["vm_wdtime"] = 600
        self.assertTrue(vt.set(cfg))
        # 查询vm_wdtime：   应该为600
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name),600)

        # 修改vm_wdtime为999999    应该成功
        cfg["base_info"]["vm_wdtime"] = 999999
        self.assertTrue(vt.set(cfg))
        # 查询vm_wdtime：   应该为999999
        self.assertEqual(vt.query_vm_wdtime(QUERY_BY_VT_NAME,test_vt_name), 999999)

        # 删除虚拟机模板 参数为 test_vt_name   应当成功
        self.assertTrue(vt.del_vmt(test_vt_name))

        # 增加映像2，类型为Datablock，使能， 应当成功  
        #新建模板， 基于映像2创建Datablock类型的磁盘， 应当失败：611003248365	
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

        # 增加映像3，类型为Datablock，禁用， 应当成功  
        #新建模板， 基于映像3创建Datablock类型的磁盘， 应当失败
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

        # 增加映像4，类型为Cdrom，使能， 应当成功 
        #新建模板， 基于映像4创建cdrom类型的磁盘， 应当成功
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

        # 增加映像5，类型为Cdrom，禁用， 应当成功
        #新建模板， 基于映像5创建Cdrom类型的磁盘， 应当失败
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


        # 使用直接接口进行虚拟机模板创建  参数如下  应当成功
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
   
        # 修改测试
        # 修改发布状态（使用 publish这个接口）  参数为 test_vt_name,0   应该成功
        self.assertTrue(vt.unpublish_vmt(test_vt_name))
        # 查询 虚拟机模板的发布状态  0,30,3,test_vt_name  应该为0
        self.assertEqual(vt.query_pub(QUERY_BY_VT_NAME,test_vt_name), 0)

        # 使用修改接口修改 参数如下   应该成功
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
        #611003123662 TC数据库整改 增加description字段; vnc_passwd 字段从ApiVmCfgAllocateData中移到 ApiVmCfgAllocateData.base_info中
        cfg["base_info"]["description"] = "hello, world!"
        cfg["base_info"]["vnc_passwd"] = "vnc_passwd003,!"
        self.assertTrue(vt.set(cfg))
        # 查询 虚拟机模板的发布状态  0,30,3,test_vt_name  应该为1
        self.assertEqual(vt.query_pub(QUERY_BY_VT_NAME,test_vt_name), IS_PUBLIC)

        # 查询 虚拟机模板的CPU数量    0,30,3,test_vt_name  应该为4
        self.assertEqual(vt.query_vcpu(QUERY_BY_VT_NAME,test_vt_name), 4)

        # 查询 虚拟机模板的内存大小    0,30,3,test_vt_name  应该为256 * 1024 *1024
        self.assertEqual(vt.query_memory(QUERY_BY_VT_NAME,test_vt_name),(long(256 * 1024 *1024)))

        # 查询 虚拟机模板的镜像总线类型 0,30,3,test_vt_name  应该为"ide"
        self.assertEqual(vt.query_machine_bus(QUERY_BY_VT_NAME,test_vt_name), "ide")
        # 查询 虚拟机模板的虚拟化类型   0,30,3,test_vt_name  应该为"pvm"
        self.assertEqual(vt.query_virt_type(QUERY_BY_VT_NAME,test_vt_name), "pvm")
        # 查询网口的数量  0,30,3,test_vt_name 应该为 0
        self.assertEqual(vt.query_nic_count(QUERY_BY_VT_NAME,test_vt_name), 0)
        # 查询磁盘的大小  0,30,3,test_vt_name 应该为 256 * 1024 * 1024
        self.assertEqual(vt.query_disks_size(QUERY_BY_VT_NAME,test_vt_name), (long(256 * 1024 * 1024)))
        # 查询磁盘的总线类型 0,30,3,test_vt_name 应该为 "scsi"
        self.assertEqual(vt.query_disks_bus(QUERY_BY_VT_NAME,test_vt_name), "scsi")
        # 查询磁盘的target 0,30,3,test_vt_name 应该为 "xvdb"
        self.assertEqual(vt.query_disks_target(QUERY_BY_VT_NAME,test_vt_name), "xvdb")
        #611003123662 TC数据库整改 增加description字段; vnc_passwd 字段从ApiVmCfgAllocateData中移到 ApiVmCfgAllocateData.base_info中
        # 查询虚拟机的描述  应该为hello, world!
        self.assertEqual(vt.query_description(QUERY_BY_VT_NAME,test_vt_name), "hello, world!")
        # 查询虚拟机的vnc密码  应该为vnc_passwd003
        self.assertEqual(vt.query_vnc_passwd(QUERY_BY_VT_NAME,test_vt_name), "vnc_passwd003,!")
        # 查询磁盘的格式化文件系统 0,30,3,test_vt_name 应该为 "ext2"
        #zyb目前有问题!!以后需要放开
        #if vt.query_disks_fstype(3,test_vt_name) != "ext2":
        #    ci_exit_ex(-1,line())
        # 查询测试
        # 查询虚拟机模板的数量   参数为 0,30,1,""   应该为1
        self.assertEqual(vt.query_count(QUERY_USER_CREAT_VT,""), 1)
        # 查询虚拟机模板的数量   参数为 0,30,2,""   应该为1
        self.assertEqual(vt.query_count(QUERY_USER_VISIBLE_VT,""), 1)
        # 查询虚拟机模板的数量   参数为 0,30,3, test_vt_name   应该为1
        self.assertEqual(vt.query_count(QUERY_BY_VT_NAME,test_vt_name), 1)
        # 查询虚拟机模板的数量   参数为 0,30,3, "test"   应该为None
        # 返回错误码：查询失败，而测试用例返回应该为None 
        self.assertEqual(vt.query_count(QUERY_BY_VT_NAME,"test"), None)

        self.assertTrue(vt.publish_vmt(test_vt_name))

        # ------------------租户自身权限测试------------------
        # 创建模板测试
        # 测试使用快速创建接口来进行创建参数如下：   应该失败 //chenww cli接口应删除
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
        # 创建磁盘  参数如下    应当失败     //chenww cli接口应删除
        disk1 = {"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"}
        # test_vt_name 和 disks = [disk1]
        disks = [disk1]
        self.assertFalse(user_vt.set_disk(test_vt_name,disks))
        # 修改发布状态（使用 publish这个接口）  参数为 test_vt_name   应当失败
        self.assertFalse(user_vt.publish_vmt(test_vt_name) )
        # 使用修改接口修改 参数如下   应该失败
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
        # 查询虚拟机模板的数量   参数为 0,30,1,""   应该为0
        self.assertEqual(user_vt.query_count(QUERY_USER_CREAT_VT,NO_VT_NAME),None)
        # 查询虚拟机模板的数量   参数为 0,30,2,""   应该为1
        self.assertEqual(user_vt.query_count(QUERY_USER_VISIBLE_VT,NO_VT_NAME), 1)
        # 查询虚拟机模板的数量   参数为 0,30,3, test_vt_name   应该为1
        self.assertEqual(user_vt.query_count(QUERY_BY_VT_NAME,test_vt_name), 1)
        # 查询虚拟机模板的数量   参数为 0,30,3, "test"   应该为0
        self.assertEqual(user_vt.query_count(QUERY_BY_VT_NAME,"test"), None)


        # ------------------管理员其他权限测试------------------
        # 无




        # ------------------和虚拟机相关测试------------------
        # 删除虚拟机模板 参数为 test_vt_name   应当成功（该操作使用管理员权限操作）
        self.assertTrue(vt.del_vmt(test_vt_name))

        #611003122482 VMCFG配置字段长度限制
        #EC611003017919 创建16个硬盘的vm无法部署，提示Schedule failed to allocate vid
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
        #生成17个磁盘的数据和9个网卡的数据
        disk={}
        for i in range(0,17):
            disk[i] = {"id":-1L,"size":long(100 * 1024 * 1024),"bus":"ide","type":"disk","position":1,"fstype":"ext3"}
        nics={}
        #index = 0
        #for i in range(0,9):
         #   index = index + 1
         #   nics[i] = {"nic_index":index,"sriov":bool(0),"loop":bool(0),"logic_network_id":"logic_network_1"}    
              
        
        #创建虚拟机模板 应该成功
        self.assertFalse(vt.allocate(cfg))
       
        #一个磁盘，九个网卡，创建虚拟机，应该失败
        cfg["base_info"]["disks"] = [disk[0]]        
        cfg["base_info"]["nics"] = [nics[0],nics[1],nics[2],nics[3],nics[4],nics[5],nics[6],nics[7],nics[8]] 
        #创建虚拟机模板 应该成功
        self.assertFalse(vt.allocate(cfg))
        
        
        #十六个磁盘，八个网卡，创建虚拟机，应该成功
        cfg["base_info"]["disks"] = [disk[0],disk[1],disk[2],disk[3],disk[4],disk[5],disk[6],disk[7],disk[8],disk[9],disk[10],disk[11],disk[12],disk[13],disk[14],disk[15]]
       
        #根据模板创建虚拟机 应该成功
        v = vmcfg(server,root_session)
        self.assertTrue(v.allocate(cf))
        # 根据名称获取刚刚创建的虚拟机的id 应该成功
        vm_id = v.get_vm_id(test_vm_name1)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #等待tc上集群资源信息到位 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)   
         
        # 部署虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"deploy"))
            
        # 获取虚拟机状态   应该是部署成功态，状态6为位置状态
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17) 
        time.sleep(10)             
        #停止虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"stop"))
        #撤销虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"cancel"))
	# 查询状态    应该是2（可采用5S查询一次，一共查询16次，如果16次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # 删除虚拟机模板  应当成功（该操作使用管理员权限操作）
        self.assertTrue(vt.del_vmt(test_vt_name))
        
        #611003022063 【tecs】storage 的check操作不应该处理文件id为-1 且文件类型为file的iso文件
        #611003184730 DEV版本不配置磁盘，只配置一个上下文，会导致产生异常
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
        #生成100k的数据（不包括文件名称）
        data = ""
        for i in range(0,1024*100):
            data = data + "a"
        cfg["base_info"]["context"] = {"test.txt":data}
        #创建虚拟机模板 应该失败
        self.assertFalse(vt.allocate(cfg))
        #生成100k的数据(包括文件名称)
        data = ""
        for i in range(0,(1024*100-len("test.txt"))):
            data = data + "a"
        cfg["base_info"]["context"] = {"test.txt":data}
        #创建虚拟机模板 应该成功
        v = vmcfg(server,root_session)        
        self.assertTrue(v.allocate(cfg))
        # 根据名称获取刚刚创建的虚拟机的id 应该成功
        vm_id = v.get_vm_id(test_vm_name2)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #等待tc上集群资源信息到位 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            print "cluster query syn ", count

        print "waiting for hc power on ..."    
        time.sleep(180)    
        # 部署虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"deploy"))   
        # 获取虚拟机状态   应该是部署成功态，状态6为位置状态
        count = 0
        vm_state = U_STATE_UNKNOWN   
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING):
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)
        #611003154777 【V4虚拟化】虚拟机reset后，由原来的不绑核变成了绑核，
        #导致重启后虚拟机CPU使用率不够，单板挂死
        #通过xm vcpu-list结果查询CPU Affinity的值，应该为any cpu      
        sshcmd = ssh_cmd(server_addr,ssh_port)
        vmInstace = "instance_" + str(vm_id)
        tgt_list = sshcmd.xm_cmd("xm vcpu-list","Name",vmInstace)
        print tgt_list
        for tgt_dict in tgt_list:
            self.assertEqual(tgt_dict['CPU'],"any")
            if tgt_dict.has_key("Affinity"):
                self.assertEqual(tgt_dict['Affinity'],"cpu")
        #强制重启虚拟机
        time.sleep(20)
        self.assertTrue(v.action(vm_id,"reset"))  
        time.sleep(20)
        while (v.get_vm_state(vm_id) != U_STATE_RUNNING and v.get_vm_state(vm_id)!= None):
            time.sleep(10)
            count = count + 1            
            self.assertTrue(count<17)
        print "count =%s" %count
        #再次通过xm vcpu-list结果查询CPU Affinity的值，应该为any cpu  
        tgt_list = sshcmd.xm_cmd("xm vcpu-list","Name",vmInstace)
        print tgt_list
        for tgt_dict in tgt_list:
            self.assertEqual(tgt_dict['CPU'],"any")
            if tgt_dict.has_key("Affinity"):
                self.assertEqual(tgt_dict['Affinity'],"cpu")
            
        #停止虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"stop"))
        #撤销虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"cancel"))
	# 查询状态    应该是2（可采用5S查询一次，一共查询16次，如果16次都查询失败，则退出）
        count = 0
        vm_state=U_STATE_UNKNOWN       
        while (vm.get_vm_state(vm_id)!=U_STATE_CONFIGURATION and vm.get_vm_state(vm_id)!= None):
                    time.sleep(10)
                    count = count + 1
                    self.assertTrue(count<17)
        # 删除虚拟机模板  应当成功（该操作使用管理员权限操作）
        self.assertTrue(vt.del_vmt(test_vt_name))
        
        #虚拟机映像支持动态调整功能测试
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
        #获取管理员操作权限
        v = vmcfg(server,root_session)
        #创建虚拟机 应该成功
        self.assertTrue(v.allocate(cfg))
        # 根据名称获取刚刚创建的虚拟机的id 应该成功
        vm_id = v.get_vm_id(test_vm_name3)
        self.assertNotEqual(vm_id, None,"failed to get vm id!")
        #等待tc上集群资源信息到位 
        count = 0  
        cm = clustermanager(server,root_session) 
        while (cm.query(START_INDEX,QUERY_COUNT,SYN_MODE) != True):
            time.sleep(3)
            count = count + 1
            self.assertTrue(count<20)
        print "waiting for hc power on ..."    
        time.sleep(180)    
        # 部署虚拟机 应该成功
        self.assertTrue(v.action(vm_id,"deploy"))   
        # 获取虚拟机状态   应该是部署成功态
        count = 0
        vm_state = v.get_vm_state(vm_id)  
        while (vm_state != U_STATE_RUNNING):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)            
        # 虚拟机关机 应该成功        
        self.assertTrue(v.action(vm_id,"stop")) 
        # 获取虚拟机状态   应该是关机
        count = 0
        vm_state = v.get_vm_state(vm_id)
        while (vm_state != U_STATE_SHUTOFF):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<3) 
        #更换映像
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
        # 数据同步 应该成功
        self.assertTrue(v.action(vm_id,"sync")) 
        # 虚拟机开机 应该成功
        self.assertTrue(v.action(vm_id,"start"))           
        # 获取虚拟机状态   应该是运行态
        count = 0
        vm_state = v.get_vm_state(vm_id)  
        while (vm_state != U_STATE_RUNNING):
            vm_state = v.get_vm_state(vm_id)
            print "now vm state = %d" %vm_state
            time.sleep(10)
            count = count + 1
            self.assertTrue(count<17)   
        #查询虚拟机映像id 应该为 user_image_id
        self.assertEqual(v.query_machine_id(vm_id), user_image_id,"failed to change vm image_id!")
        #测试结束
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
