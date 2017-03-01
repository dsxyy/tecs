#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
#
# �ļ����ƣ�test_003_user_group.py
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
#�����ϴ�������������ľ����ļ�·��
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
    #��-i��--image����ָ����������õľ����ļ�
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

class UserGroupTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testUserGroup(self):
        # ------------------����׼��------------------
        # ��¼tecs����ȡ����Ա�˻����ܺ��session
        root_session = login(server,test_root_user,test_root_passwd)
        self.assertNotEqual(root_session,None,"failed to get encrypted root session!")
        lognum = logqurey(server,root_session)
        self.assertEqual(lognum,1)    

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

        print "test user and usergroup start......"
        #����һ���û���ʹ�ò���Ϊ test_group1,1 ,Ӧ�óɹ�
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))
        #����һ���û���ʹ�ò���Ϊ test_group1,2  Ӧ��ʧ��
        self.assertFalse(ug.allocate("test_group1",GROUP_TYPE2))
        #�鿴test_group1 �Ƿ�����   Ӧ��Ϊ����
        self.assertEqual(ug.get_use_flag("test_group1"),UG_ENABLE)
        #����test_group1 Ϊ����
        self.assertTrue(ug.set("test_group1",UG_DISABLE,"disable test_group1"))
        #�鿴test_group1 �Ƿ�����   Ӧ��Ϊ����
        self.assertEqual(ug.get_use_flag("test_group1"), UG_DISABLE)
        #ɾ��test_group1 �û���    Ӧ�óɹ�
        self.assertTrue(ug.delete("test_group1"))
        #����һ���û���ʹ�ò���Ϊ test_group2,1  Ӧ�óɹ�
        self.assertTrue(ug.allocate("test_group2",GROUP_TYPE1))
        #����һ���û� ����Ϊ test_cloud,test,test,1,test_group2  Ӧ�óɹ�
        u  = user(server,root_session)
        self.assertTrue(u.allocate("test_cloud","test","test",CLOUDADMIN,"test_group2"))
        #����test_group2 Ϊ����
        self.assertTrue(ug.set("test_group2",UG_DISABLE,"disable test_group2"))  
        #ʹ�� test_cloud �û���¼    Ӧ��ʧ��
        self.assertEqual(None,login(server,"test_cloud","test"))
        #����test_group2 Ϊ����
        self.assertTrue(ug.set("test_group2",UG_ENABLE,"enable test_group2"))
		
		#��Ҫ���µ�¼
        ns = login(server,"test_cloud","test")
        nu = user(server,ns)
        #ʹ�� test_cloud �û���¼    Ӧ�óɹ�
        self.assertTrue(nu.query(START_INDEX,START_INDEX,"test_cloud"),"test_cloud can't login !")
        #ɾ��test_group2 �û���    Ӧ��ʧ��
        self.assertFalse(ug.delete("test_group2"))
        #ɾ���û� ����Ϊ test_cloud  Ӧ�óɹ�
        self.assertTrue(u.delete("test_cloud"))
        #ɾ��test_group2 �û���    Ӧ�óɹ�
        self.assertTrue(ug.delete("test_group2"))
        #����һ���û���ʹ�ò���Ϊ test_group3,2  Ӧ�óɹ�
        self.assertTrue(ug.allocate("test_group3",GROUP_TYPE2))
        #����һ���û� ����Ϊ test_cloud,aaaaaa,bbbbbb,1,test_group3  Ӧ��ʧ��
        self.assertFalse(u.allocate("test_cloud","aaaaaa","bbbbbb",CLOUDADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_cloud,aaaaaa,aaaaaa,1,test_group3  Ӧ�óɹ�
        self.assertTrue(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_cloud,aaaaaa,aaaaaa,1,test_group3  Ӧ��ʧ��
        self.assertFalse(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_account,aaaaaa,aaaaaa,2,test_group3  Ӧ�óɹ�
        self.assertTrue(u.allocate("test_account","aaaaaa","aaaaaa",ACCOUNTADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_user,aaaaaa,aaaaaa,3,test_group3  Ӧ�óɹ�
        self.assertTrue(u.allocate("test_user","aaaaaa","aaaaaa",ACCOUNTUSER,"test_group3"))
        #����һ���û���ʹ�ò���Ϊ test_group4,2  Ӧ�óɹ�
        self.assertTrue(ug.allocate("test_group4",GROUP_TYPE2))
        #����һ���û� ����Ϊ test_cloud,aaaaaa,aaaaaa,1,test_group3  Ӧ��ʧ��
        self.assertFalse(u.allocate("test_cloud","aaaaaa","aaaaaa",CLOUDADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_account,aaaaaa,aaaaaa,2,test_group3  Ӧ��ʧ��
        self.assertFalse(u.allocate("test_account","aaaaaa","aaaaaa",ACCOUNTADMIN,"test_group3"))
        #����һ���û� ����Ϊ test_user,aaaaaa,aaaaaa,3,test_group3  Ӧ��ʧ��
        self.assertFalse(u.allocate("test_user","aaaaaa","aaaaaa",ACCOUNTUSER,"test_group3"))

        # ����Ա���޸��⻧��Ϣ
        # �޸��û����� ����Ϊ "test_user","cccccc","cccccc",1,"test_group4",2,"13970581182","email","addr" Ӧ��ʧ��
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertFalse(u.set("test_user","cccccc","cccccc",CLOUDADMIN,"test_group4",\
                                USER_DISABLE,"13970581182","email","addr",NO_DESCRIPTION))
        # ��ѯ�û������� 
        self.assertNotEqual(u.query_create_time(START_INDEX,QUERY_COUNT,"test_user"),None)
        # �޸��û����� ����Ϊ "test_user","cccccc","cccccc",3,"test_group3",1,"","","",""Ӧ���ɹ�
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertTrue(u.set("test_user","cccccc","cccccc", ACCOUNTUSER, "test_group3", USER_ENABLE,\
                               NO_PHONE, NO_EMAIL, NO_ADDR,NO_DESCRIPTION))
        # ��ѯ�û��ĵ绰 ����Ϊ 0,50,"test_user"    Ӧ��Ϊ ""
        self.assertEqual(u.query_phone(START_INDEX,QUERY_COUNT,"test_user"), NO_PHONE)

        # ��ѯ�û���email ����Ϊ 0,50,"test_user"    Ӧ��Ϊ ""
        self.assertEqual(u.query_email(START_INDEX,QUERY_COUNT,"test_user"), NO_EMAIL)

        # ��ѯ�û��ĵ�ַ ����Ϊ 0,50,"test_user"    Ӧ��Ϊ ""
        self.assertEqual(u.query_location(START_INDEX,QUERY_COUNT,"test_user"), NO_ADDR)

        # ��¼һ���⻧��Ϣ
        encrypted_user_session = login(server,"test_user","cccccc")
		
        if encrypted_user_session == None:	
            print "the encrypted_user_session is none "
        self.assertNotEqual(encrypted_user_session,None,"failed to get encrypted root session!")

        #���������������ӵ������ӿڣ�tecs.session.query �� tecs.session.logout
        self.assertTrue(logout(server,encrypted_user_session))
        lognum = logqurey(server,root_session)
        print "the lognum is %d" %lognum
 
        # ��¼һ���⻧��Ϣ
        encrypted_user_session = login(server,"test_user","cccccc")
        self.assertNotEqual(encrypted_user_session,None,"failed to get encrypted root session!")
        
        ur  = user(server,encrypted_user_session)
        # �޸��û����� ����Ϊ "test_user","","", 3, "", 1,"13970581182","email","addr" Ӧ���ɹ�
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertTrue(ur.set("test_user", "", "", ACCOUNTUSER, "", USER_ENABLE,\
                                "13970581182", "email", "addr","hello, world!"))
        # ��ѯ�û������� Ӧ��Ϊ"hello, world!"
        self.assertEqual(u.query_description(START_INDEX,QUERY_COUNT,"test_user"), "hello, world!")
        # ��ѯ�û��ĵ绰 ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "13970581182"
        self.assertEqual(ur.query_phone(START_INDEX,QUERY_COUNT,"test_user"), "13970581182")

        # ��ѯ�û���email ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "email"
        self.assertEqual(ur.query_email(START_INDEX,QUERY_COUNT,"test_user"), "email")

        # ��ѯ�û��ĵ�ַ ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "addr"
        self.assertEqual(ur.query_location(START_INDEX,QUERY_COUNT,"test_user"), "addr")

        # �޸��û����� ����Ϊ "test_user","cccccc1","cccccc1",3,"test_group3",1,"13970581181","email1","addr1" Ӧ���ɹ�
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertTrue(ur.set("test_user","cccccc1","cccccc1",ACCOUNTUSER,"test_group3",\
                                USER_ENABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        # ʹ���޸ĺ�������¼"test_user"
        encrypted_user1_session = login(server,"test_user","cccccc1")
        self.assertNotEqual(encrypted_user1_session,None,"failed to get encrypted root session!")
        
        ur1  = user(server,encrypted_user1_session)
        # ��ѯ�û��ĵ绰 ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "13970581181"
        self.assertEqual(ur1.query_phone(START_INDEX,QUERY_COUNT,"test_user"), "13970581181")
        # ��ѯ�û���email ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "email1"
        self.assertEqual(ur1.query_email(START_INDEX,QUERY_COUNT,"test_user"), "email1")

        # ��ѯ�û��ĵ�ַ ����Ϊ 0,50,"test_user"    Ӧ��Ϊ "addr1"
        self.assertEqual(ur1.query_location(START_INDEX,QUERY_COUNT,"test_user"), "addr1")

        # �⻧Ȩ���޸ķǷ�����
        # �޸��û����� ����Ϊ "test_user","cccccc1","cccccc1",1,"test_group3",1,"13970581181","email1","addr1" Ӧ��ʧ��
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertFalse(ur1.set("test_user","cccccc1","cccccc1",CLOUDADMIN,"test_group3",\
                                  USER_ENABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        # �޸��û����� ����Ϊ "test_user","cccccc1","cccccc1",3,"test_group3",2,"13970581181","email1","addr1" Ӧ��ʧ��
        # 611003123663 TC���ݿ����ģ�tecs.user.set�ӿ�����description�ֶ�
        self.assertFalse(ur1.set("test_user","cccccc1","cccccc1",ACCOUNTUSER,"test_group3",\
                                  USER_DISABLE,"13970581181","email1","addr1",NO_DESCRIPTION))

        #ɾ���û� ����Ϊ test_cloud  Ӧ�óɹ�
        self.assertTrue(u.delete("test_cloud"))
        #ɾ���û� ����Ϊ test_account  Ӧ�óɹ�
        self.assertTrue(u.delete("test_account"))
        #ɾ���û� ����Ϊ test_user  Ӧ�óɹ�
        self.assertTrue(u.delete("test_user"))
        #ɾ��test_group3 �û���    Ӧ�óɹ�
        self.assertTrue(ug.delete("test_group3"))
        #ɾ��test_group4 �û���    Ӧ�óɹ�
        self.assertTrue(ug.delete("test_group4"))

        #���Խ���
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
