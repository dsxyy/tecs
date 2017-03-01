#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
#
# 文件名称：test_003_user_group.py
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
#用于上传，创建虚拟机的镜像文件路径
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
    #用-i或--image可以指定虚拟机所用的镜像文件
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

class UserGroupTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testUserGroup(self):
        # ------------------环境准备------------------
        # 登录tecs，获取管理员账户加密后的session
        root_session = login(server,test_root_user,test_root_passwd)
        self.assertNotEqual(root_session,None,"failed to get encrypted root session!")
        lognum = logqurey(server,root_session)
        self.assertEqual(lognum,1)    

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

        print "test user and usergroup start......"
        #创建一个用户组使用参数为 test_group1,1 ,应该成功
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))
        #创建一个用户组使用参数为 test_group1,2  应该失败
        self.assertFalse(ug.allocate("test_group1",GROUP_TYPE2))
        #查看test_group1 是否启用   应该为启用
        self.assertEqual(ug.get_use_flag("test_group1"),UG_ENABLE)
        #设置test_group1 为禁用
        self.assertTrue(ug.set("test_group1",UG_DISABLE,"disable test_group1"))
        #查看test_group1 是否启用   应该为禁用
        self.assertEqual(ug.get_use_flag("test_group1"), UG_DISABLE)
        #删除test_group1 用户组    应该成功
        self.assertTrue(ug.delete("test_group1"))
        #创建一个用户组使用参数为 test_group2,1  应该成功
        self.assertTrue(ug.allocate("test_group2",GROUP_TYPE1))
        #创建一个用户 参数为 test_cloud,test,test,1,test_group2  应该成功
        u  = user(server,root_session)
        self.assertTrue(u.allocate("test_cloud","test","test",CLOUDADMIN,"test_group2"))
        #设置test_group2 为禁用
        self.assertTrue(ug.set("test_group2",UG_DISABLE,"disable test_group2"))  
        #使用 test_cloud 用户登录    应该失败
        self.assertEqual(None,login(server,"test_cloud","test"))
        #设置test_group2 为启用
        self.assertTrue(ug.set("test_group2",UG_ENABLE,"enable test_group2"))
		
		#需要重新登录
        ns = login(server,"test_cloud","test")
        nu = user(server,ns)
        #使用 test_cloud 用户登录    应该成功
        self.assertTrue(nu.query(START_INDEX,START_INDEX,"test_cloud"),"test_cloud can't login !")
        #删除test_group2 用户组    应该失败
        self.assertFalse(ug.delete("test_group2"))
        #删除用户 参数为 test_cloud  应该成功
        self.assertTrue(u.delete("test_cloud"))
        #删除test_group2 用户组    应该成功
        self.assertTrue(ug.delete("test_group2"))
        #创建一个用户组使用参数为 test_group3,2  应该成功
        self.assertTrue(ug.allocate("test_group3",GROUP_TYPE2))
        #创建一个用户 参数为 test_cloud,aaaaaa,bbbbbb,1,test_group3  应该失败
        self.assertFalse(u.allocate("test_cloud","aaaaaa","bbbbbb",CLOUDADMIN,"test_group3"))
        #创建一个用户 参数为 test_cloud,aaaaaa,aaaaaa,1,test_group3  应该成功
        self.assertTrue(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #创建一个用户 参数为 test_cloud,aaaaaa,aaaaaa,1,test_group3  应该失败
        self.assertFalse(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #创建一个用户 参数为 test_account,aaaaaa,aaaaaa,2,test_group3  应该成功
        self.assertTrue(u.allocate("test_account","aaaaaa","aaaaaa",ACCOUNTADMIN,"test_group3"))
        #创建一个用户 参数为 test_user,aaaaaa,aaaaaa,3,test_group3  应该成功
        self.assertTrue(u.allocate("test_user","aaaaaa","aaaaaa",ACCOUNTUSER,"test_group3"))
        #创建一个用户组使用参数为 test_group4,2  应该成功
        self.assertTrue(ug.allocate("test_group4",GROUP_TYPE2))
        #创建一个用户 参数为 test_cloud,aaaaaa,aaaaaa,1,test_group3  应该失败
        self.assertFalse(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #创建一个用户 参数为 test_account,aaaaaa,aaaaaa,2,test_group3  应该失败
        self.assertFalse(u.allocate("test_account","aaaaaa","aaaaaa",ACCOUNTADMIN,"test_group3"))
        #创建一个用户 参数为 test_user,aaaaaa,aaaaaa,3,test_group3  应该失败
        self.assertFalse(u.allocate("test_user","aaaaaa","aaaaaa",ACCOUNTUSER,"test_group3"))

        # 管理员来修改租户信息
        # 修改用户参数 参数为 "test_user","cccccc","cccccc",1,"test_group4",2,"13970581182","email","addr" 应当失败
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertFalse(u.set("test_user","cccccc","cccccc",CLOUDADMIN,"test_group4",\
                                USER_DISABLE,"13970581182","email","addr",NO_DESCRIPTION))
        # 查询用户的描述 
        self.assertNotEqual(u.query_create_time(START_INDEX,QUERY_COUNT,"test_user"),None)
        # 修改用户参数 参数为 "test_user","cccccc","cccccc",3,"test_group3",1,"","","",""应当成功
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertTrue(u.set("test_user","cccccc","cccccc", ACCOUNTUSER, "test_group3", USER_ENABLE,\
                               NO_PHONE, NO_EMAIL, NO_ADDR,NO_DESCRIPTION))
        # 查询用户的电话 参数为 0,50,"test_user"    应该为 ""
        self.assertEqual(u.query_phone(START_INDEX,QUERY_COUNT,"test_user"), NO_PHONE)

        # 查询用户的email 参数为 0,50,"test_user"    应该为 ""
        self.assertEqual(u.query_email(START_INDEX,QUERY_COUNT,"test_user"), NO_EMAIL)

        # 查询用户的地址 参数为 0,50,"test_user"    应该为 ""
        self.assertEqual(u.query_location(START_INDEX,QUERY_COUNT,"test_user"), NO_ADDR)

        # 登录一个租户信息
        encrypted_user_session = login(server,"test_user","cccccc")
		
        if encrypted_user_session == None:	
            print "the encrypted_user_session is none "
        self.assertNotEqual(encrypted_user_session,None,"failed to get encrypted root session!")

        #测试王明辉新增加的两个接口：tecs.session.query 和 tecs.session.logout
        self.assertTrue(logout(server,encrypted_user_session))
        lognum = logqurey(server,root_session)
        print "the lognum is %d" %lognum
 
        # 登录一个租户信息
        encrypted_user_session = login(server,"test_user","cccccc")
        self.assertNotEqual(encrypted_user_session,None,"failed to get encrypted root session!")
        
        ur  = user(server,encrypted_user_session)
        # 修改用户参数 参数为 "test_user","","", 3, "", 1,"13970581182","email","addr" 应当成功
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertTrue(ur.set("test_user", "", "", ACCOUNTUSER, "", USER_ENABLE,\
                                "13970581182", "email", "addr","hello, world!"))
        # 查询用户的描述 应该为"hello, world!"
        self.assertEqual(u.query_description(START_INDEX,QUERY_COUNT,"test_user"), "hello, world!")
        # 查询用户的电话 参数为 0,50,"test_user"    应该为 "13970581182"
        self.assertEqual(ur.query_phone(START_INDEX,QUERY_COUNT,"test_user"), "13970581182")

        # 查询用户的email 参数为 0,50,"test_user"    应该为 "email"
        self.assertEqual(ur.query_email(START_INDEX,QUERY_COUNT,"test_user"), "email")

        # 查询用户的地址 参数为 0,50,"test_user"    应该为 "addr"
        self.assertEqual(ur.query_location(START_INDEX,QUERY_COUNT,"test_user"), "addr")

        # 修改用户参数 参数为 "test_user","cccccc1","cccccc1",3,"test_group3",1,"13970581181","email1","addr1" 应当成功
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertTrue(ur.set("test_user","cccccc1","cccccc1",ACCOUNTUSER,"test_group3",\
                                USER_ENABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        # 使用修改后的密码登录"test_user"
        encrypted_user1_session = login(server,"test_user","cccccc1")
        self.assertNotEqual(encrypted_user1_session,None,"failed to get encrypted root session!")
        
        ur1  = user(server,encrypted_user1_session)
        # 查询用户的电话 参数为 0,50,"test_user"    应该为 "13970581181"
        self.assertEqual(ur1.query_phone(START_INDEX,QUERY_COUNT,"test_user"), "13970581181")
        # 查询用户的email 参数为 0,50,"test_user"    应该为 "email1"
        self.assertEqual(ur1.query_email(START_INDEX,QUERY_COUNT,"test_user"), "email1")

        # 查询用户的地址 参数为 0,50,"test_user"    应该为 "addr1"
        self.assertEqual(ur1.query_location(START_INDEX,QUERY_COUNT,"test_user"), "addr1")

        # 租户权限修改非法参数
        # 修改用户参数 参数为 "test_user","cccccc1","cccccc1",1,"test_group3",1,"13970581181","email1","addr1" 应当失败
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertFalse(ur1.set("test_user","cccccc1","cccccc1",CLOUDADMIN,"test_group3",\
                                  USER_ENABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        # 修改用户参数 参数为 "test_user","cccccc1","cccccc1",3,"test_group3",2,"13970581181","email1","addr1" 应当失败
        # 611003123663 TC数据库整改，tecs.user.set接口增加description字段
        self.assertFalse(ur1.set("test_user","cccccc1","cccccc1",ACCOUNTUSER,"test_group3",\
                                  USER_DISABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        #删除用户 参数为 test_cloud  应该成功
        self.assertTrue(u.delete("test_cloud"))
        #删除用户 参数为 test_account  应该成功
        self.assertTrue(u.delete("test_account"))
        #删除用户 参数为 test_user  应该成功
        self.assertTrue(u.delete("test_user"))
        #删除test_group3 用户组    应该成功
        self.assertTrue(ug.delete("test_group3"))
        #删除test_group4 用户组    应该成功
        self.assertTrue(ug.delete("test_group4"))

        #测试结束
        print "success to test test_003_user_group.py"
        
        def suite():
                suite = unittest.TestSuite()
                suite.addTest(UserGroupTestCase())
                return suite
 
if __name__ == "__main__":
        file_name = sys.argv[0]
        sys.argv = None
        sys.argv = [file_name]
        unittest.main()
