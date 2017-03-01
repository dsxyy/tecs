#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_004_cluster_manager.py
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

class CMTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testCM(self):
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

        #创建一个用户组使用参数为 test_group1,1 ,应该成功
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))
        # 创建一个用户 参数为 user,test,test,3,test_group1  应该成功
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # 登录租户  获取租户session 以备后面使用
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session, None,"failed to get user session!")
        #等待20s
        #time.sleep(20)

        cm  = clustermanager(server,root_session)
        #查询第一个存在的集群  应该能查询到
        fisrt_clustername = cm.get_fisrt_clustername()
        self.assertNotEqual(fisrt_clustername,None,"get_fisrt_clustername fail")

        #设置查询到的集群   应该成功
        self.assertTrue(cm.set(fisrt_clustername,"test set"))

        #再次修改集群描述信息   应该成功
        self.assertTrue(cm.set(fisrt_clustername,"test modify info"))

        #查看集群是使能状态  应该是1
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_ENABLE)
        #同步方式查看集群的在线状态  应该是在线的
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_is_online(fisrt_clustername,SYN_MODE), IS_ONLINE)
        #设置一个不存在的集群 参数为"test_cluster" 应该失败
        #if  cm.set("test_cluster","test set not exist cc") != True:
        #    ci_exit_ex(-1)

        #查看不存在集群的在线状态 应该是离线的
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_is_online("test_cluster",SYN_MODE),None)

        #注销不存在的集群   应该是成功的
        #if  cm.forget("test_cluster") != True:
        #    ci_exit_ex(-1)

        #遗忘存在的集群     应该是成功的
        self.assertTrue(cm.forget(fisrt_clustername))
        #等待重新获取集群
        count = 0 
        while (cm.get_fisrt_clustername() != fisrt_clustername):
            time.sleep(1)
            count = count + 1    
            self.assertTrue(count < 19)
        print "get_fisrt_clustername count= %d "% count       
        #disable 一个注册的集群  应该是成功的
        self.assertTrue(cm.disable(fisrt_clustername))

        #查看集群的使能状态应 该为0
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_DISABLE)

        #enable 注册的集群   应该成功
        self.assertTrue(cm.enable(fisrt_clustername))

        #查看当前的集群使能状态 应该为1
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_ENABLE)

        #等20S
        #time.sleep(20)

        #查看当前的集群运行状态  应该为在线
        # 611003123663 TC数据库整改，ApiCluster中的registe_state、run_state删除掉，增加is_online、enabled字段
        self.assertEqual(cm.get_is_online(fisrt_clustername,SYN_MODE), IS_ONLINE)
        #查看当前最大core数目
        self.assertNotEqual(cm.get_core_num(),None)
        #查看当前最大tcu数目
        self.assertNotEqual(cm.get_tcu_num(),None)

        #查看指定网络平面
        #self.assertNotEqual(cm.get_netplane(fisrt_clustername),None)

        #---------------普通租户测试开始------------
        print "************begin user test****************"
        cm3  = clustermanager(server,user_session)

        #查询集群信息  应该是失败的
        self.assertFalse(cm3.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        #设置集群信息  应该是失败的
        self.assertFalse(cm3.set(fisrt_clustername,"test setcc"))

        #遗忘集群      应该是失败的
        self.assertFalse(cm3.forget(fisrt_clustername))
        #修改集群信息  应该是失败的
        self.assertFalse(cm3.set(fisrt_clustername,"test modify info"))

        #查看当前最大core数目  应当成功
        self.assertNotEqual(cm3.get_core_num(),None)
        #查看当前最大tcu数目  应当成功
        self.assertNotEqual(cm3.get_tcu_num(),None)
        #查看指定网络平面   应当成功
        #self.assertNotEqual(cm3.get_netplane(fisrt_clustername),None)

        #测试结束
        print "success to test test_004_cluster_manager.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(CMTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
