#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_007_image.py
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
from tecs.vmtemplate import *

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
#待测试的模板名称
test_vt_name = "ttylinux_vt"

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

class ImageTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testImage(self):   
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
        self.assertNotEqual(user_session,None,"failed to get user session!")
        
        # 以管理员身份上传一个映像文件  应该成功        
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # 以租户身份上传一个映像文件  应该成功
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
            
        # 注册集群
        root_cluster_manager_method = clustermanager(server,root_session) 
        #time.sleep(20)
        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # 注册主机
        root_host_method = host(server,root_session)
        # 等待 20S 等待资源上报
        time.sleep(20)
        self.assertTrue(root_host_method.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))

        # 创建工程
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
          
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
 
        # 设置tcu 8　
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)
        self.assertNotEqual(cpu_id,None)        

        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))
        
        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,NO_CPU_INFO,cpu_id))

        # ------------------管理员自身权限测试------------------
        # 测试注册
        # 查询管理员上传的image id, 参数为映像文件名，返回值为映像id
        i = image(server,root_session)
        file_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(file_id,None)   

        i_id = file_id;

        #查询镜像注册时间
        self.assertNotEqual(i.query_register_time(file_id),None)   

        # 修改image  参数为 file_id, "ttylinux2", "", "", ""  应当成功
        self.assertTrue(i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION,"SUSE"))
        
        # 查询image的名字 ，应该为"ttylinux2"
        self.assertEqual(i.query_name_by_id(i_id),"ttylinux2")

        # 查询image的type ，应该为"machine"
        self.assertEqual(i.query_type_by_id(i_id),"machine")

        # 修改image  参数为 file_id, "", "kernel", "", ""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的type ，应该为"kernel"
        self.assertEqual(i.query_type_by_id(i_id),"kernel")

        # 修改image  参数为 file_id, "", "ramdisk", "", ""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "ramdisk", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的type ，应该为"ramdisk"
        self.assertEqual(i.query_type_by_id(i_id),"ramdisk")

        # 修改image  参数为 file_id, "", "cdrom", "", ""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "cdrom", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的type ，应该为"cdrom"
        self.assertEqual(i.query_type_by_id(i_id),"cdrom")
        
        # 修改image  参数为 file_id, "", "datablock", "", ""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "datablock", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的type ，应该为"datablock"
        self.assertEqual(i.query_type_by_id(i_id),"datablock")

        # 修改image  参数为 file_id, "", "aaaaa", "", ""  应当失败
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, "aaaaa", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的arch ，应该为"x86_64"
        self.assertEqual(i.query_arch_by_id(i_id),"x86_64")

        # 修改image  参数为 file_id, "", "", "i386", ""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的arch ，应该为"i386"
        self.assertEqual(i.query_arch_by_id(i_id),"i386")

        ########################################################    
        # 修改image  参数为 file_id, "", "", "i386", "ide",""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
            
        # 查询image的arch ，应该为"i386"
        self.assertEqual(i.query_bus_by_id(i_id),"ide")
        
        # 修改image  参数为 file_id, "", "", "i386", "scsi",""  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "scsi",NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的arch ，应该为"i386"
        self.assertEqual(i.query_bus_by_id(i_id),"scsi")
        
        # 修改image  参数为 file_id, "", "", "i386", "aaa",""  应当失败
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "aaa",NO_IMAGE_DESCRIPTION, "SUSE"))

        # 查询image的arch ，应该仍然为最后一次设置的"scsi"
        self.assertEqual(i.query_bus_by_id(i_id),"scsi")        
        ########################################################    

        # 修改image  参数为 file_id, "", "", "aaaa", ""  应当失败
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "aaaa", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        
        # 查询image的description 应该为 "this is test image"
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide", "this is test image", "SUSE"))
        self.assertEqual(i.query_description_by_id(i_id),"this is test image")
        
        # 修改image  参数为 file_id, "", "", "", "test"  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", "test", "SUSE"))
        # 查询image的description 应该为 "test"
        self.assertEqual(i.query_description_by_id(i_id), "test")

        # 测试使能
        # 查询image 的使能状态  应该为 1
        self.assertEqual(i.query_state_by_id(file_id), 1)
        
        # 禁能image  参数为 file_id, 0  应当成功
        self.assertTrue(i.enable(file_id,I_DISABLE))
        # 查询image 的使能状态  应该为 0
        self.assertEqual(i.query_state_by_id(file_id), 0)

        # 测试发布
        # 查询image 的发布状态  应该为 0(默认）
        self.assertEqual(i.query_pub_by_id(file_id), 0)
        # 取消image发布  参数为 file_id, 0  应当成功
        self.assertTrue(i.publish(file_id,NOT_PUBLIC))
        # 查询image 的发布状态  应该为 0
        self.assertEqual(i.query_pub_by_id(file_id), 0)
        self.assertTrue(i.publish(file_id, IS_PUBLIC))
        self.assertEqual(i.query_pub_by_id(file_id), 1)
        
        # 测试查询
        i_id = file_id;
        # 查询用户创建的image的数量   参数为 0,100, -4,"",0   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询用户可见的image的数量   参数为 0,100, -3,"",0   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_VISBLE_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询发布的的image的数量   参数为 0,100, -2,"",0   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_VISBLE_PUBLIC_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询指定用户的image的数量   参数为 0,100, 0,"admin",0   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"admin",NO_IMAGE_ID), 1)
        # 查询指定id的image的数量   参数为 0,100, 1,"",file_id   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, i_id), 1)
        # 查询系统中所有image的数量   参数为 0,100, 2,"",0   返回的数量应该是2,因为已经自动注册
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)

        # ------------------普通用户权限------------------
        # 测试注册
        # 查询租户上传的file_id, 参数 test_image    返回值为文件user_file_id
        user_i = image(server,user_session)
        user_file_id = user_i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(user_file_id,None)

        user_i_id = user_file_id

        # 修改image  参数为 user_file_id, "ttylinux2", "", "", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的名字 ，应该为"ttylinux2"
        self.assertEqual(user_i.query_name_by_id(user_i_id),"ttylinux2")
        # 查询image的type ，应该为"machine"
        self.assertEqual(user_i.query_type_by_id(user_i_id),"machine")
        # 修改image  参数为 user_file_id, "", "kernel", "", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的type ，应该为"kernel"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "kernel")
        # 修改image  参数为 user_file_id, "", "ramdisk", "", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "ramdisk", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的type ，应该为"ramdisk"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "ramdisk")
        # 修改image  参数为 user_file_id, "", "cdrom", "", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "cdrom", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的type ，应该为"cdrom"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "cdrom")
        # 修改image  参数为 user_file_id, "", "datablock", "", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "datablock", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的type ，应该为"datablock"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "datablock")
        # 修改image  参数为 user_file_id, "", "aaaa", "", ""  应当失败
        self.assertFalse(user_i.set(user_file_id, NO_IMAGE_NAME, "aaaa", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的arch ，应该为"x86_64"
        self.assertEqual(user_i.query_arch_by_id(user_i_id), "x86_64")
        # 修改image  参数为 user_file_id, "", "", "i386", ""  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的arch ，应该为"i386"
        self.assertEqual(user_i.query_arch_by_id(user_i_id), "i386")
        # 修改image  参数为 user_file_id, "", "", "aaaa", ""  应当失败
        self.assertFalse(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "aaaa","ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # 查询image的description 应该为 "this is test image"
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide", "this is test image", "SUSE"))
        self.assertEqual(user_i.query_description_by_id(user_i_id), "this is test image")
        # 修改image  参数为 user_file_id, "", "", "", "test"  应当成功
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide","test", "SUSE"))
        # 查询image的description 应该为 "test"
        self.assertEqual(user_i.query_description_by_id(user_i_id), "test")

        # 测试使能
        # 查询image 的使能状态  应该为 1
        self.assertEqual(user_i.query_state_by_id(user_file_id), 1)
        # 禁能image  参数为 user_file_id, 0  应当失败
        self.assertFalse(user_i.enable(user_file_id,I_DISABLE))
        # 查询image 的使能状态  应该为 1
        self.assertEqual(user_i.query_state_by_id(user_file_id), 1)

        # 测试发布
        # 查询image 的发布状态  应该为 0
        self.assertEqual(user_i.query_pub_by_id(user_file_id), 0)
        # 发布image  参数为 user_file_id, 1  应当失败
        self.assertFalse(user_i.publish(file_id,NOT_PUBLIC))
        # 查询image 的发布状态  应该为 0
        self.assertEqual(user_i.query_pub_by_id(user_file_id), 0)

        # 测试查询
        user_i_id = user_file_id 
        # 查询用户创建的image的数量   参数为 0,100, -4,"",0   返回的数量应该是1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询用户可见的image的数量   参数为 0,100, -3,"",0   返回的数量应该是2
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_VISBLE_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)
        # 查询发布的的image的数量   参数为 0,100, -2,"",0   返回的数量应该是1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_VISBLE_PUBLIC_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询指定用户的image的数量   参数为 0,100, 0,"user",0   返回的数量应该是1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER, NO_USER_NAME, NO_IMAGE_ID), 1)
        # 查询指定id的image的数量   参数为 0,100, 1,"",user_file_id   返回的数量应该是1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, user_file_id), 1)
        # 查询系统中所有image的数量   参数为 0,100, 2,"",0   应该返回-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), None)

        # 测试修改别人的映像
        # 使用用户session 修改image 参数为 file_id, "ttylinux2", "", "", ""  应当失败
        self.assertFalse(user_i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # 测试使能别人的映像
        # 使用用户session 禁能image 参数为 file_id, 0  应当失败
        self.assertFalse(user_i.enable(file_id, I_DISABLE))
        # 测试发布别人的映像
        # 使用用户session 发布image 参数为 file_id, 1  应当失败
        self.assertFalse(user_i.publish(file_id, IS_PUBLIC))


        # 测试查询别人的映像
        #下面几个测试使用用户serssion
        # 查询指定用户的image的数量   参数为 0,100, 0,"admin",0   应当返回-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"admin",NO_IMAGE_ID), None)
        # 查询指定id的image的数量   参数为 0,100, 1,"",file_id   
        #管理员不进行发布查询数量为0，返回的数量应该是-1
        self.assertTrue(i.publish(file_id,NOT_PUBLIC))
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, file_id), None)
        #管理员发布后查询数量为1
        self.assertTrue(i.publish(file_id,IS_PUBLIC))
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, file_id), 1)
        # 查询系统中所有image的数量   参数为 0,100, 2,"",0   应该返回-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), None)

        # ------------------管理员其他权限测试------------------
        # 测试注销别人的映像

        # 测试修改别人的映像
        # 使用管理员session 修改image 参数为 user_file_id, "ttylinux2", "", "", ""  应当成功
        self.assertTrue(i.set(user_file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # 测试使能别人的映像
        # 使用管理员session 禁能image 参数为 user_file_id, 0  应当成功
        self.assertTrue(i.enable(user_file_id, I_DISABLE))
        # 使用管理员session查询image 的使能状态   应该为0
        self.assertEqual(i.query_state_by_id(user_file_id), 0)

        # 使用管理员session 使能image 参数为 user_file_id, 1  应当成功
        self.assertTrue(i.enable(user_file_id, I_ENABLE))
        # 使用管理员session查询image 的使能状态   应该为1
        self.assertEqual(i.query_state_by_id(user_file_id), 1)

        # 测试发布别人的映像
        # 使用管理员session 发布image 参数为 user_file_id, 1  应当成功
        self.assertTrue(i.publish(user_file_id, IS_PUBLIC))
        # 使用管理员session查询image 的发布状态   应该为1
        self.assertEqual(i.query_pub_by_id(user_file_id), IS_PUBLIC)
        # 使用管理员session 取消发布image 参数为 user_file_id, 0  应当成功
        self.assertTrue(i.publish(user_file_id, NOT_PUBLIC))
        # 使用管理员session查询image 的发布状态   应该为0
        self.assertEqual(i.query_pub_by_id(user_file_id), NOT_PUBLIC)

        # 测试查询别人的映像
        #下面几个测试使用管理员serssion
        # 查询指定用户的image的数量   参数为 0,100, 0,"user",0   应当返回1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"user", NO_IMAGE_ID), 1)
        # 查询指定id的image的数量   参数为 0,100, 1,"",user_file_id   返回的数量应该是1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME,user_file_id), 1)
        # 查询系统中所有image的数量   参数为 0,100, 2,"",0   应该返回2
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)

        # ------------------image 被使用后的测试------------------
        # 该测试段都使用管理员身份来进行测试,创建虚拟机除外，创建虚拟机的接口需要使用租户的身份来创建
        # 创建虚拟下面是虚拟机创建的参数(使用租户身份进行创建)     应该成功
        cfg = {}
        cfg["vm_name"] = test_vm_name
        cfg["project_name"] = test_project_name   
        cfg["vm_num"] = 1   
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 4
        cfg["base_info"]["memory"] = long(256 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":file_id,"position":1,"size":0L,"bus":"ide","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"    
     
        user_v = vmcfg(server,user_session)
        self.assertFalse(user_v.allocate(cfg))
        self.assertTrue(i.enable(file_id, I_ENABLE))
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "machine", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "x86_64", "ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        self.assertTrue(user_v.allocate(cfg))
        self.assertNotEqual(user_v.get_vm_id(test_vm_name),None)
        # 修改虚拟机的各个字段
        # 修改image 参数为 file_id, "ttylinux2", "", "", ""  应当成功
        self.assertTrue(i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 修改image 参数为 file_id, "", "kernel", "", ""  应当失败
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 修改image 参数为 file_id, "", "", "i386", ""  应当失败
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # 修改image 参数为 file_id, "", "", "", "test"  应当成功
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide","test", "SUSE"))

        #611003132165 web界面上显示的image引用计数需包含被模板引用的次数 
        #查询镜像被私有模板引用次数 此时未创建过模板，应该为0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #查询镜像被公有模板引用次数 此时未创建过模板，应该为0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        # 测试使用快速创建接口来进行创建参数如下：   应该成功 
        cfg = {}
        cfg["vt_name"] = test_vt_name
        cfg["is_public"] = bool(0)
        cfg["base_info"] = {}
        cfg["base_info"]["vcpu"] = 2
        cfg["base_info"]["memory"] = long(128 * 1024 * 1024)
        cfg["base_info"]["machine"] = {"id":i_id,"position":1,"size":0L,"bus":"scsi","type":"machine","target":"hda"}
        cfg["base_info"]["virt_type"] = "hvm"
        cfg["base_info"]["root_device"] = ""
        cfg["base_info"]["kernel_command"] = ""
        cfg["base_info"]["rawdata"] = ""
        vt = vmtemplate(server,root_session)
        self.assertTrue(vt.allocate(cfg))
        #查询镜像被私有模板引用次数 ，应该为1
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),1)
        #查询镜像被公有模板引用次数 ，应该为0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        #模板发布
        self.assertTrue(vt.publish_vmt(test_vt_name))
        #查询镜像被私有模板引用次数 ，应该为0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #查询镜像被公有模板引用次数 ，应该为1
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),1)
        #取消模板发布
        self.assertTrue(vt.unpublish_vmt(test_vt_name))
        #查询镜像被私有模板引用次数 ，应该为1
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),1)
        #查询镜像被公有模板引用次数 ，应该为0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        #删除模板
        self.assertTrue(vt.del_vmt(test_vt_name))
        #查询镜像被私有模板引用次数 ，应该为0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #查询镜像被公有模板引用次数 ，应该为0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        # 使能测试
        # 禁能image 参数为 file_id, 0  ,vms_running =1 ,应当失败
        self.assertFalse(i.enable(file_id, I_DISABLE))
        # 发布测试
        # 取消发布image 参数为 file_id, 0  应当失败
        self.assertFalse(i.publish(file_id, NOT_PUBLIC))

        # 注销测试
        # 查询image 的 running_vms  参数为 0,100, 1,"",file_id    应该为1
        self.assertEqual(i.query_running_vms_by_id(file_id), 1)
        #running_vms！=0时删除管理员上传的镜像，应该失败
        self.assertFalse(i.delete(file_id))
            
        # 删除虚拟机(使用租户的身份进行虚拟机删除)
        user_v_id = user_v.get_vm_id(test_vm_name)
        self.assertNotEqual(user_v_id,None)
        self.assertTrue(user_v.action(user_v_id,"delete"))

        # 查询image 的 running_vms 参数为 0,100, 1,"",file_id     应该为0
        self.assertEqual(i.query_running_vms_by_id(file_id), 0)

        #删除管理员上传的镜像
        self.assertTrue(i.delete(file_id))

        #删除租户上传的镜像
        self.assertTrue(i.delete(user_file_id))

        #删除租户上传的镜像后根据名字查询，应该为空
        user_file_id = user_i.get_id_by_filename(os.path.basename(test_image))
        self.assertEqual(user_file_id, None)

        svr_info = user_i.query_image_ftp_svr_info()
        self.assertNotEqual(svr_info,None)

        print "success to test test_007_image.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(ImageTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
