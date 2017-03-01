#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_007_image.py
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
#�����Ե�ģ������
test_vt_name = "ttylinux_vt"

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

class ImageTestCase(unittest.TestCase):
    def setUp(self):        
        pass; 
    def tearDown(self):        
        pass;
        
    def testImage(self):   
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

        # ����һ���û���ʹ�ò���Ϊ test_group1,1 ,Ӧ�óɹ�
        ug  = usergroup(server,root_session)
        self.assertTrue(ug.allocate("test_group1",GROUP_TYPE1))

        # ����һ���û� ����Ϊ user,test,test,3,test_group1  Ӧ�óɹ�
        u  = user(server,root_session)
        self.assertTrue(u.allocate("user","test","test",ACCOUNTUSER,"test_group1"))

        # ��¼�⻧  ��ȡ�⻧session �Ա�����ʹ��
        user_session = login(server,"user","test")
        self.assertNotEqual(user_session,None,"failed to get user session!")
        
        # �Թ���Ա����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�        
        self.assertTrue(upload_image_file(server_addr,test_root_user,test_root_passwd,test_image),"failed to upload_image_file!")
        # ���⻧����ϴ�һ��ӳ���ļ�  Ӧ�óɹ�
        self.assertTrue(upload_image_file(server_addr,"user","test",test_image),"failed to upload_image_file!")
            
        # ע�ἯȺ
        root_cluster_manager_method = clustermanager(server,root_session) 
        #time.sleep(20)
        self.assertTrue(root_cluster_manager_method.query(START_INDEX,QUERY_COUNT,SYN_MODE))

        # ע������
        root_host_method = host(server,root_session)
        # �ȴ� 20S �ȴ���Դ�ϱ�
        time.sleep(20)
        self.assertTrue(root_host_method.query(START_INDEX,QUERY_COUNT,test_cluster,test_host))

        # ��������
        user_project = project(server,user_session)
        self.assertTrue(user_project.allocate(test_project_name,"test project description"))
          
        root_project = project(server,root_session)
        self.assertTrue(root_project.allocate(test_project_name,"test project description"))
 
        # ����tcu 8��
        cpu_id = root_host_method.get_cpu_id(test_cluster,test_host)
        self.assertNotEqual(cpu_id,None)        

        print "cpu id of host %s = %d" % (test_host,cpu_id)    

        root_tcu_method = tcu(server,root_session)
        self.assertTrue(root_tcu_method.set(test_cluster,cpu_id,TCU_NUM8,"test cpu description"))
        
        self.assertTrue(root_tcu_method.query(START_INDEX,QUERY_COUNT,test_cluster,NO_CPU_INFO,cpu_id))

        # ------------------����Ա����Ȩ�޲���------------------
        # ����ע��
        # ��ѯ����Ա�ϴ���image id, ����Ϊӳ���ļ���������ֵΪӳ��id
        i = image(server,root_session)
        file_id = i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(file_id,None)   

        i_id = file_id;

        #��ѯ����ע��ʱ��
        self.assertNotEqual(i.query_register_time(file_id),None)   

        # �޸�image  ����Ϊ file_id, "ttylinux2", "", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION,"SUSE"))
        
        # ��ѯimage������ ��Ӧ��Ϊ"ttylinux2"
        self.assertEqual(i.query_name_by_id(i_id),"ttylinux2")

        # ��ѯimage��type ��Ӧ��Ϊ"machine"
        self.assertEqual(i.query_type_by_id(i_id),"machine")

        # �޸�image  ����Ϊ file_id, "", "kernel", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��type ��Ӧ��Ϊ"kernel"
        self.assertEqual(i.query_type_by_id(i_id),"kernel")

        # �޸�image  ����Ϊ file_id, "", "ramdisk", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "ramdisk", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��type ��Ӧ��Ϊ"ramdisk"
        self.assertEqual(i.query_type_by_id(i_id),"ramdisk")

        # �޸�image  ����Ϊ file_id, "", "cdrom", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "cdrom", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��type ��Ӧ��Ϊ"cdrom"
        self.assertEqual(i.query_type_by_id(i_id),"cdrom")
        
        # �޸�image  ����Ϊ file_id, "", "datablock", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, "datablock", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��type ��Ӧ��Ϊ"datablock"
        self.assertEqual(i.query_type_by_id(i_id),"datablock")

        # �޸�image  ����Ϊ file_id, "", "aaaaa", "", ""  Ӧ��ʧ��
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, "aaaaa", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��arch ��Ӧ��Ϊ"x86_64"
        self.assertEqual(i.query_arch_by_id(i_id),"x86_64")

        # �޸�image  ����Ϊ file_id, "", "", "i386", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��arch ��Ӧ��Ϊ"i386"
        self.assertEqual(i.query_arch_by_id(i_id),"i386")

        ########################################################    
        # �޸�image  ����Ϊ file_id, "", "", "i386", "ide",""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
            
        # ��ѯimage��arch ��Ӧ��Ϊ"i386"
        self.assertEqual(i.query_bus_by_id(i_id),"ide")
        
        # �޸�image  ����Ϊ file_id, "", "", "i386", "scsi",""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "scsi",NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��arch ��Ӧ��Ϊ"i386"
        self.assertEqual(i.query_bus_by_id(i_id),"scsi")
        
        # �޸�image  ����Ϊ file_id, "", "", "i386", "aaa",""  Ӧ��ʧ��
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "aaa",NO_IMAGE_DESCRIPTION, "SUSE"))

        # ��ѯimage��arch ��Ӧ����ȻΪ���һ�����õ�"scsi"
        self.assertEqual(i.query_bus_by_id(i_id),"scsi")        
        ########################################################    

        # �޸�image  ����Ϊ file_id, "", "", "aaaa", ""  Ӧ��ʧ��
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "aaaa", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        
        # ��ѯimage��description Ӧ��Ϊ "this is test image"
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide", "this is test image", "SUSE"))
        self.assertEqual(i.query_description_by_id(i_id),"this is test image")
        
        # �޸�image  ����Ϊ file_id, "", "", "", "test"  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", "test", "SUSE"))
        # ��ѯimage��description Ӧ��Ϊ "test"
        self.assertEqual(i.query_description_by_id(i_id), "test")

        # ����ʹ��
        # ��ѯimage ��ʹ��״̬  Ӧ��Ϊ 1
        self.assertEqual(i.query_state_by_id(file_id), 1)
        
        # ����image  ����Ϊ file_id, 0  Ӧ���ɹ�
        self.assertTrue(i.enable(file_id,I_DISABLE))
        # ��ѯimage ��ʹ��״̬  Ӧ��Ϊ 0
        self.assertEqual(i.query_state_by_id(file_id), 0)

        # ���Է���
        # ��ѯimage �ķ���״̬  Ӧ��Ϊ 0(Ĭ�ϣ�
        self.assertEqual(i.query_pub_by_id(file_id), 0)
        # ȡ��image����  ����Ϊ file_id, 0  Ӧ���ɹ�
        self.assertTrue(i.publish(file_id,NOT_PUBLIC))
        # ��ѯimage �ķ���״̬  Ӧ��Ϊ 0
        self.assertEqual(i.query_pub_by_id(file_id), 0)
        self.assertTrue(i.publish(file_id, IS_PUBLIC))
        self.assertEqual(i.query_pub_by_id(file_id), 1)
        
        # ���Բ�ѯ
        i_id = file_id;
        # ��ѯ�û�������image������   ����Ϊ 0,100, -4,"",0   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯ�û��ɼ���image������   ����Ϊ 0,100, -3,"",0   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_VISBLE_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯ�����ĵ�image������   ����Ϊ 0,100, -2,"",0   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_VISBLE_PUBLIC_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯָ���û���image������   ����Ϊ 0,100, 0,"admin",0   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"admin",NO_IMAGE_ID), 1)
        # ��ѯָ��id��image������   ����Ϊ 0,100, 1,"",file_id   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, i_id), 1)
        # ��ѯϵͳ������image������   ����Ϊ 0,100, 2,"",0   ���ص�����Ӧ����2,��Ϊ�Ѿ��Զ�ע��
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)

        # ------------------��ͨ�û�Ȩ��------------------
        # ����ע��
        # ��ѯ�⻧�ϴ���file_id, ���� test_image    ����ֵΪ�ļ�user_file_id
        user_i = image(server,user_session)
        user_file_id = user_i.get_id_by_filename(os.path.basename(test_image))
        self.assertNotEqual(user_file_id,None)

        user_i_id = user_file_id

        # �޸�image  ����Ϊ user_file_id, "ttylinux2", "", "", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage������ ��Ӧ��Ϊ"ttylinux2"
        self.assertEqual(user_i.query_name_by_id(user_i_id),"ttylinux2")
        # ��ѯimage��type ��Ӧ��Ϊ"machine"
        self.assertEqual(user_i.query_type_by_id(user_i_id),"machine")
        # �޸�image  ����Ϊ user_file_id, "", "kernel", "", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��type ��Ӧ��Ϊ"kernel"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "kernel")
        # �޸�image  ����Ϊ user_file_id, "", "ramdisk", "", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "ramdisk", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��type ��Ӧ��Ϊ"ramdisk"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "ramdisk")
        # �޸�image  ����Ϊ user_file_id, "", "cdrom", "", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "cdrom", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��type ��Ӧ��Ϊ"cdrom"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "cdrom")
        # �޸�image  ����Ϊ user_file_id, "", "datablock", "", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, "datablock", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��type ��Ӧ��Ϊ"datablock"
        self.assertEqual(user_i.query_type_by_id(user_i_id), "datablock")
        # �޸�image  ����Ϊ user_file_id, "", "aaaa", "", ""  Ӧ��ʧ��
        self.assertFalse(user_i.set(user_file_id, NO_IMAGE_NAME, "aaaa", NO_IMAGE_ARCH,"ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��arch ��Ӧ��Ϊ"x86_64"
        self.assertEqual(user_i.query_arch_by_id(user_i_id), "x86_64")
        # �޸�image  ����Ϊ user_file_id, "", "", "i386", ""  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��arch ��Ӧ��Ϊ"i386"
        self.assertEqual(user_i.query_arch_by_id(user_i_id), "i386")
        # �޸�image  ����Ϊ user_file_id, "", "", "aaaa", ""  Ӧ��ʧ��
        self.assertFalse(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "aaaa","ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # ��ѯimage��description Ӧ��Ϊ "this is test image"
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH,"ide", "this is test image", "SUSE"))
        self.assertEqual(user_i.query_description_by_id(user_i_id), "this is test image")
        # �޸�image  ����Ϊ user_file_id, "", "", "", "test"  Ӧ���ɹ�
        self.assertTrue(user_i.set(user_file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide","test", "SUSE"))
        # ��ѯimage��description Ӧ��Ϊ "test"
        self.assertEqual(user_i.query_description_by_id(user_i_id), "test")

        # ����ʹ��
        # ��ѯimage ��ʹ��״̬  Ӧ��Ϊ 1
        self.assertEqual(user_i.query_state_by_id(user_file_id), 1)
        # ����image  ����Ϊ user_file_id, 0  Ӧ��ʧ��
        self.assertFalse(user_i.enable(user_file_id,I_DISABLE))
        # ��ѯimage ��ʹ��״̬  Ӧ��Ϊ 1
        self.assertEqual(user_i.query_state_by_id(user_file_id), 1)

        # ���Է���
        # ��ѯimage �ķ���״̬  Ӧ��Ϊ 0
        self.assertEqual(user_i.query_pub_by_id(user_file_id), 0)
        # ����image  ����Ϊ user_file_id, 1  Ӧ��ʧ��
        self.assertFalse(user_i.publish(file_id,NOT_PUBLIC))
        # ��ѯimage �ķ���״̬  Ӧ��Ϊ 0
        self.assertEqual(user_i.query_pub_by_id(user_file_id), 0)

        # ���Բ�ѯ
        user_i_id = user_file_id 
        # ��ѯ�û�������image������   ����Ϊ 0,100, -4,"",0   ���ص�����Ӧ����1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯ�û��ɼ���image������   ����Ϊ 0,100, -3,"",0   ���ص�����Ӧ����2
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_CREAT_VISBLE_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)
        # ��ѯ�����ĵ�image������   ����Ϊ 0,100, -2,"",0   ���ص�����Ӧ����1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_USER_VISBLE_PUBLIC_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯָ���û���image������   ����Ϊ 0,100, 0,"user",0   ���ص�����Ӧ����1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER, NO_USER_NAME, NO_IMAGE_ID), 1)
        # ��ѯָ��id��image������   ����Ϊ 0,100, 1,"",user_file_id   ���ص�����Ӧ����1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, user_file_id), 1)
        # ��ѯϵͳ������image������   ����Ϊ 0,100, 2,"",0   Ӧ�÷���-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), None)

        # �����޸ı��˵�ӳ��
        # ʹ���û�session �޸�image ����Ϊ file_id, "ttylinux2", "", "", ""  Ӧ��ʧ��
        self.assertFalse(user_i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION, "SUSE"))
        # ����ʹ�ܱ��˵�ӳ��
        # ʹ���û�session ����image ����Ϊ file_id, 0  Ӧ��ʧ��
        self.assertFalse(user_i.enable(file_id, I_DISABLE))
        # ���Է������˵�ӳ��
        # ʹ���û�session ����image ����Ϊ file_id, 1  Ӧ��ʧ��
        self.assertFalse(user_i.publish(file_id, IS_PUBLIC))


        # ���Բ�ѯ���˵�ӳ��
        #���漸������ʹ���û�serssion
        # ��ѯָ���û���image������   ����Ϊ 0,100, 0,"admin",0   Ӧ������-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"admin",NO_IMAGE_ID), None)
        # ��ѯָ��id��image������   ����Ϊ 0,100, 1,"",file_id   
        #����Ա�����з�����ѯ����Ϊ0�����ص�����Ӧ����-1
        self.assertTrue(i.publish(file_id,NOT_PUBLIC))
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, file_id), None)
        #����Ա�������ѯ����Ϊ1
        self.assertTrue(i.publish(file_id,IS_PUBLIC))
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME, file_id), 1)
        # ��ѯϵͳ������image������   ����Ϊ 0,100, 2,"",0   Ӧ�÷���-1
        self.assertEqual(user_i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), None)

        # ------------------����Ա����Ȩ�޲���------------------
        # ����ע�����˵�ӳ��

        # �����޸ı��˵�ӳ��
        # ʹ�ù���Աsession �޸�image ����Ϊ user_file_id, "ttylinux2", "", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(user_file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide", NO_IMAGE_DESCRIPTION, "SUSE"))

        # ����ʹ�ܱ��˵�ӳ��
        # ʹ�ù���Աsession ����image ����Ϊ user_file_id, 0  Ӧ���ɹ�
        self.assertTrue(i.enable(user_file_id, I_DISABLE))
        # ʹ�ù���Աsession��ѯimage ��ʹ��״̬   Ӧ��Ϊ0
        self.assertEqual(i.query_state_by_id(user_file_id), 0)

        # ʹ�ù���Աsession ʹ��image ����Ϊ user_file_id, 1  Ӧ���ɹ�
        self.assertTrue(i.enable(user_file_id, I_ENABLE))
        # ʹ�ù���Աsession��ѯimage ��ʹ��״̬   Ӧ��Ϊ1
        self.assertEqual(i.query_state_by_id(user_file_id), 1)

        # ���Է������˵�ӳ��
        # ʹ�ù���Աsession ����image ����Ϊ user_file_id, 1  Ӧ���ɹ�
        self.assertTrue(i.publish(user_file_id, IS_PUBLIC))
        # ʹ�ù���Աsession��ѯimage �ķ���״̬   Ӧ��Ϊ1
        self.assertEqual(i.query_pub_by_id(user_file_id), IS_PUBLIC)
        # ʹ�ù���Աsession ȡ������image ����Ϊ user_file_id, 0  Ӧ���ɹ�
        self.assertTrue(i.publish(user_file_id, NOT_PUBLIC))
        # ʹ�ù���Աsession��ѯimage �ķ���״̬   Ӧ��Ϊ0
        self.assertEqual(i.query_pub_by_id(user_file_id), NOT_PUBLIC)

        # ���Բ�ѯ���˵�ӳ��
        #���漸������ʹ�ù���Աserssion
        # ��ѯָ���û���image������   ����Ϊ 0,100, 0,"user",0   Ӧ������1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_USER,"user", NO_IMAGE_ID), 1)
        # ��ѯָ��id��image������   ����Ϊ 0,100, 1,"",user_file_id   ���ص�����Ӧ����1
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_BY_IMAGE_ID, NO_USER_NAME,user_file_id), 1)
        # ��ѯϵͳ������image������   ����Ϊ 0,100, 2,"",0   Ӧ�÷���2
        self.assertEqual(i.get_query_count(START_INDEX,QUERY_COUNT, QUERY_ALL_IMAGE, NO_USER_NAME, NO_IMAGE_ID), 2)

        # ------------------image ��ʹ�ú�Ĳ���------------------
        # �ò��Զζ�ʹ�ù���Ա��������в���,������������⣬����������Ľӿ���Ҫʹ���⻧�����������
        # ������������������������Ĳ���(ʹ���⻧��ݽ��д���)     Ӧ�óɹ�
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
        # �޸�������ĸ����ֶ�
        # �޸�image ����Ϊ file_id, "ttylinux2", "", "", ""  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, "ttylinux2", NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # �޸�image ����Ϊ file_id, "", "kernel", "", ""  Ӧ��ʧ��
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, "kernel", NO_IMAGE_ARCH, "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # �޸�image ����Ϊ file_id, "", "", "i386", ""  Ӧ��ʧ��
        self.assertFalse(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, "i386", "ide",NO_IMAGE_DESCRIPTION, "SUSE"))
        # �޸�image ����Ϊ file_id, "", "", "", "test"  Ӧ���ɹ�
        self.assertTrue(i.set(file_id, NO_IMAGE_NAME, NO_IMAGE_TYPE, NO_IMAGE_ARCH, "ide","test", "SUSE"))

        #611003132165 web��������ʾ��image���ü����������ģ�����õĴ��� 
        #��ѯ����˽��ģ�����ô��� ��ʱδ������ģ�壬Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #��ѯ���񱻹���ģ�����ô��� ��ʱδ������ģ�壬Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        # ����ʹ�ÿ��ٴ����ӿ������д����������£�   Ӧ�óɹ� 
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
        #��ѯ����˽��ģ�����ô��� ��Ӧ��Ϊ1
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),1)
        #��ѯ���񱻹���ģ�����ô��� ��Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        #ģ�巢��
        self.assertTrue(vt.publish_vmt(test_vt_name))
        #��ѯ����˽��ģ�����ô��� ��Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #��ѯ���񱻹���ģ�����ô��� ��Ӧ��Ϊ1
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),1)
        #ȡ��ģ�巢��
        self.assertTrue(vt.unpublish_vmt(test_vt_name))
        #��ѯ����˽��ģ�����ô��� ��Ӧ��Ϊ1
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),1)
        #��ѯ���񱻹���ģ�����ô��� ��Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        #ɾ��ģ��
        self.assertTrue(vt.del_vmt(test_vt_name))
        #��ѯ����˽��ģ�����ô��� ��Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_private_vt(file_id),0)
        #��ѯ���񱻹���ģ�����ô��� ��Ӧ��Ϊ0
        self.assertEqual(i.query_ref_count_by_public_vt(file_id),0)
        # ʹ�ܲ���
        # ����image ����Ϊ file_id, 0  ,vms_running =1 ,Ӧ��ʧ��
        self.assertFalse(i.enable(file_id, I_DISABLE))
        # ��������
        # ȡ������image ����Ϊ file_id, 0  Ӧ��ʧ��
        self.assertFalse(i.publish(file_id, NOT_PUBLIC))

        # ע������
        # ��ѯimage �� running_vms  ����Ϊ 0,100, 1,"",file_id    Ӧ��Ϊ1
        self.assertEqual(i.query_running_vms_by_id(file_id), 1)
        #running_vms��=0ʱɾ������Ա�ϴ��ľ���Ӧ��ʧ��
        self.assertFalse(i.delete(file_id))
            
        # ɾ�������(ʹ���⻧����ݽ��������ɾ��)
        user_v_id = user_v.get_vm_id(test_vm_name)
        self.assertNotEqual(user_v_id,None)
        self.assertTrue(user_v.action(user_v_id,"delete"))

        # ��ѯimage �� running_vms ����Ϊ 0,100, 1,"",file_id     Ӧ��Ϊ0
        self.assertEqual(i.query_running_vms_by_id(file_id), 0)

        #ɾ������Ա�ϴ��ľ���
        self.assertTrue(i.delete(file_id))

        #ɾ���⻧�ϴ��ľ���
        self.assertTrue(i.delete(user_file_id))

        #ɾ���⻧�ϴ��ľ����������ֲ�ѯ��Ӧ��Ϊ��
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
