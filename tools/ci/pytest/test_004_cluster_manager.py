#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_004_cluster_manager.py
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
    #��-i��--image����ָ����������õ�ӳ���ļ�
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

class CMTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testCM(self):
        # ------------------����׼��------------------
        # ��¼tecs����ȡ����Ա�˻����ܺ��session
        root_session = login(server,test_root_user,test_root_passwd)
        self.assertNotEqual(root_session,None,"failed to get encrypted root session!")

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

        #����һ���û���ʹ�ò���Ϊ test_group1,1 ,Ӧ�óɹ�
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))
        # ����һ���û� ����Ϊ user,test,test,3,test_group1  Ӧ�óɹ�
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # ��¼�⻧  ��ȡ�⻧session �Ա�����ʹ��
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session, None,"failed to get user session!")
        #�ȴ�20s
        #time.sleep(20)

        cm  = clustermanager(server,root_session)
        #��ѯ��һ�����ڵļ�Ⱥ  Ӧ���ܲ�ѯ��
        fisrt_clustername = cm.get_fisrt_clustername()
        self.assertNotEqual(fisrt_clustername,None,"get_fisrt_clustername fail")

        #���ò�ѯ���ļ�Ⱥ   Ӧ�óɹ�
        self.assertTrue(cm.set(fisrt_clustername,"test set"))

        #�ٴ��޸ļ�Ⱥ������Ϣ   Ӧ�óɹ�
        self.assertTrue(cm.set(fisrt_clustername,"test modify info"))

        #�鿴��Ⱥ��ʹ��״̬  Ӧ����1
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_ENABLE)
        #ͬ����ʽ�鿴��Ⱥ������״̬  Ӧ�������ߵ�
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_is_online(fisrt_clustername,SYN_MODE), IS_ONLINE)
        #����һ�������ڵļ�Ⱥ ����Ϊ"test_cluster" Ӧ��ʧ��
        #if  cm.set("test_cluster","test set not exist cc") != True:
        #    ci_exit_ex(-1)

        #�鿴�����ڼ�Ⱥ������״̬ Ӧ�������ߵ�
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_is_online("test_cluster",SYN_MODE),None)

        #ע�������ڵļ�Ⱥ   Ӧ���ǳɹ���
        #if  cm.forget("test_cluster") != True:
        #    ci_exit_ex(-1)

        #�������ڵļ�Ⱥ     Ӧ���ǳɹ���
        self.assertTrue(cm.forget(fisrt_clustername))
        #�ȴ����»�ȡ��Ⱥ
        count = 0 
        while (cm.get_fisrt_clustername() != fisrt_clustername):
            time.sleep(1)
            count = count + 1    
            self.assertTrue(count < 19)
        print "get_fisrt_clustername count= %d "% count       
        #disable һ��ע��ļ�Ⱥ  Ӧ���ǳɹ���
        self.assertTrue(cm.disable(fisrt_clustername))

        #�鿴��Ⱥ��ʹ��״̬Ӧ ��Ϊ0
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_DISABLE)

        #enable ע��ļ�Ⱥ   Ӧ�óɹ�
        self.assertTrue(cm.enable(fisrt_clustername))

        #�鿴��ǰ�ļ�Ⱥʹ��״̬ Ӧ��Ϊ1
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_enabled(fisrt_clustername,SYN_MODE), CM_ENABLE)

        #��20S
        #time.sleep(20)

        #�鿴��ǰ�ļ�Ⱥ����״̬  Ӧ��Ϊ����
        # 611003123663 TC���ݿ����ģ�ApiCluster�е�registe_state��run_stateɾ����������is_online��enabled�ֶ�
        self.assertEqual(cm.get_is_online(fisrt_clustername,SYN_MODE), IS_ONLINE)
        #�鿴��ǰ���core��Ŀ
        self.assertNotEqual(cm.get_core_num(),None)
        #�鿴��ǰ���tcu��Ŀ
        self.assertNotEqual(cm.get_tcu_num(),None)

        #�鿴ָ������ƽ��
        #self.assertNotEqual(cm.get_netplane(fisrt_clustername),None)

        #---------------��ͨ�⻧���Կ�ʼ------------
        print "************begin user test****************"
        cm3  = clustermanager(server,user_session)

        #��ѯ��Ⱥ��Ϣ  Ӧ����ʧ�ܵ�
        self.assertFalse(cm3.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        #���ü�Ⱥ��Ϣ  Ӧ����ʧ�ܵ�
        self.assertFalse(cm3.set(fisrt_clustername,"test setcc"))

        #������Ⱥ      Ӧ����ʧ�ܵ�
        self.assertFalse(cm3.forget(fisrt_clustername))
        #�޸ļ�Ⱥ��Ϣ  Ӧ����ʧ�ܵ�
        self.assertFalse(cm3.set(fisrt_clustername,"test modify info"))

        #�鿴��ǰ���core��Ŀ  Ӧ���ɹ�
        self.assertNotEqual(cm3.get_core_num(),None)
        #�鿴��ǰ���tcu��Ŀ  Ӧ���ɹ�
        self.assertNotEqual(cm3.get_tcu_num(),None)
        #�鿴ָ������ƽ��   Ӧ���ɹ�
        #self.assertNotEqual(cm3.get_netplane(fisrt_clustername),None)

        #���Խ���
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
