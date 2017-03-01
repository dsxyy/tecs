#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ�
# ������ڣ�2012/12/26
#*******************************************************************************/
import sys, os
import unittest 


# ���������װ
from interface.base_interface.rpc.contrib import tecs_common
from interface.environment_prepare_package import prepare_environment
from interface.config_para import config_para
 
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# ���Խӿڷ�װ
from interface.base_interface.rpc import vmcfg
from interface.vm_advance_package import *
from interface.image_advance_package import *

class UnitTestPortableDiskCreate(unittest.TestCase):
    '''
    �����ƶ���(tecs.portabledisk.create)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        1.�ϴ�һ������
        2.����һ�������
        3.�������������
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        #image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)
        self.image_ctrl = image_ctrl
        # HC�������п��ƾ��
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # �������SSH�����
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # �������db�����
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        # ����������Ļ�������
     #   self.vm_ctrl.add_base("vm1", 1, 1, 256, image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
     #   self.vm_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
      #  self.vm_ctrl.add_context("test", "aaaaa")
        # ���������
     #   self.vm_ctrl.create()
        # ���������
     #   self.vm_ctrl.deploy()
        # ����
     #   self.vm_ctrl.start()
        # ��������Ϊ����̬
     #   self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
        pass;

# ��������
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()

    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
                  2��
                  3��
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������ƶ��̻�������  
        ���Բ��裺1�������ƶ���
                  2����ʱ�鿴SC�Ƿ�д�����
        '''
     #   self.vm_ctrl.add_base("vm1", 1, 1, 256, self.image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
      #  request_id = self.vm_ctrl.create_portable_disk(self,256,self.db_vm_check_ctrl.get_cluster_name_by_vid(self.vm_ctrl.get_vm_id("vm1")),"")  
        self.db_vm_check_ctrl.check_pd_storage_cluster()
        request_id = self.vm_ctrl.create_portable_disk(134217728L, "tecscluster", "tecs")
        print "create portable disk request_id=%s" % request_id
        self.assertNotEqual(request_id, False, "create portable disk fail!")
        # ��������ƶ�����ر��Ƿ��ж�Ӧ��¼
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 0), "wait create portable disk fail")
        print "pass!!!"
        pass;
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()
        
    def Exception_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������machine������Ϣ�쳣��������
        ���Բ��裺1�����ô�������Ϊ�ǡ�machine�������������������ʧ��
                  2������machineӳ��洢λ�ã��ȷǡ����ش洢����Ҳ�ǡ�����洢�������������������ʧ��
                  3�����ô����������ͣ��ȷǡ�ide����Ҳ�ǡ�scsi�������������������ʧ��
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������������Ϣ�쳣��������
        ���Բ��裺1���������⻯���ͣ��ȷǡ�ȫ���⻯����Ҳ�ǡ������⻯�������������������ʧ��
                  2����������������ĸ���Ⱥ����Ⱥ���Ƴ��ȳ���64�ֽڣ����������������ʧ��
                  3����������������ĸ��������������Ƴ��ȳ���64�ֽڣ����������������ʧ��
                  4��������������Ź�ʱ��ΪС��600-7200�����������������ʧ��
                  5������VNC�������룬���ȫΪ�ո���ַ��������������������ʧ��
        '''
        
        pass;
        
# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()
        
    def limit_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������������Ĺ������Ʊ߽��������
        ���Բ��裺1����������������Ĺ������ƣ����û��������и����ƵĹ��̣�������Ϊ1�������������Ӧ�óɹ�
                  2����������������Ĺ������ƣ����û��������и����ƵĹ��̣�������Ϊ32�������������Ӧ�óɹ�
                  3����������������Ĺ������ƣ����û��������и����ƵĹ��̣������ȴ���32�����������������ʧ��(���Բ��ˣ��ڴ�������ʱ�ͻ�ʧ��)
                  4����������������Ĺ������ƣ����û��������и����ƵĹ��̣�������Ϊ0�����������������ʧ��(���Բ��ˣ��ڴ�������ʱ�ͻ�ʧ��)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����������Ʊ߽��������
        ���Բ��裺1��������������ƣ�����Ϊ0�������������Ӧ�óɹ�
                  2��������������ƣ�����Ϊ32�������������Ӧ�óɹ�
                  3��������������ƣ����ȴ���32�����������������ʧ��
        '''

        pass;
        
# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����ͨ�û�����������Ƿ�ɹ�
        ���Բ��裺1����ͨ�û���¼
                  2����������������ĸ���Ⱥ�����������������ʧ��
                  3����������������ĸ����������������������ʧ��
                  4����������ƽ�����ƣ����������������ʧ��
                  5��������������������ȡֵ��Χ�Ϸ����������������Ӧ�óɹ�
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


class UnitTestPortableDiskDelete(unittest.TestCase):
    '''
    ɾ���ƶ���(tecs.portabledisk.delete)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC�������п��ƾ��
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # �������SSH�����
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # �������db�����
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# ��������
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����ѯ���е��������Ϣ
        ���Բ��裺1��ϵͳ�����˶�������
                  2��ִ�в�ѯ������Ĳ���
                  3�������ʾ����Ϣ�Ƿ��걸
                  4��������ʾ�����Ŀ
                  5�������ʾ����Ϣ�Ƿ��걸
        '''
        request_id = self.db_vm_check_ctrl.pd_get_request_id()
        print "delete portable disk request_id=%s" % request_id
        self.assertNotEqual(request_id, False, "get request_id false!")
        #ɾ���ƶ���
        self.assertTrue(self.vm_ctrl.delete_portable_disk(request_id), "delete portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 1), "wait delete portable disk fail")
        print "pass!!!"
        pass;
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������WEB�Զ����ɲ�ѯ���ݲ���д
        ���Բ��裺1��
        '''
        pass;
        
# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()

    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����ͨ�û���ѯ�������Ϣ
        ���Բ��裺1����ͨ�û���¼
                  2��ִ�в�ѯ������Ĳ���
                  3����ͨ�û����Բ�ѯ�������Ϣ
                  4��������ʾ�����Ŀ
                  5����ͨ�û����Բ�ѯ�������Ϣ
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestPortableDiskAttach(unittest.TestCase):
    '''
    attach�ƶ���(tecs.portabledisk.attach)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC�������п��ƾ��
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # �������SSH�����
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # �������db�����
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# ��������
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������ƶ��̵������
        ���Բ��裺1������һ���ƶ���
                  2���ɹ�����һ��centos�������
                  3��attach�ƶ��̵������
                  4�����cc��vm_disk���ƶ����Ƿ�attach�ɹ�
        '''
        vid=self.db_vm_check_ctrl.get_all_deploy_vid()
        request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #request_id = "a1432371-aa91-4b24-88d8-f67ac56052ec"
        print "vid=%ld,request_id=%s" % (long(vid[0]), request_id)
        self.assertNotEqual(request_id, False, "get request_id false!")
        #attach�ƶ���
        self.assertTrue(self.vm_ctrl.attach_portable_disk(request_id, long(vid[0]), "hdb", "ide"), "attach portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid[0]), 30, 2), "wait attach portable disk fail")
        print "pass!!!"
        pass;
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������WEB�Զ����ɲ�ѯ���ݲ���д
        ���Բ��裺1��
        '''
        pass;
        
# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()

    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����ͨ�û���ѯ�������Ϣ
        ���Բ��裺1����ͨ�û���¼
                  2��ִ�в�ѯ������Ĳ���
                  3����ͨ�û����Բ�ѯ�������Ϣ
                  4��������ʾ�����Ŀ
                  5����ͨ�û����Բ�ѯ�������Ϣ
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestPortableDiskDetach(unittest.TestCase):
    '''
    attach�ƶ���(tecs.portabledisk.attach)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC�������п��ƾ��
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # �������SSH�����
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # �������db�����
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# ��������
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���������detachһ���ƶ���
        ���Բ��裺1��ѡ��һ�����ƶ��̵������
                  2��detach�ƶ���
        '''
        request_id, vid = self.db_vm_check_ctrl.pd_get_attach_request_id_and_vid()
        print "vid=%ld,request_id=%s" % (long(vid), request_id)
        self.assertNotEqual(request_id, False, "get request_id and vid false!")
        #attach�ƶ���
        self.assertTrue(self.vm_ctrl.detach_portable_disk(long(vid), request_id), "detach portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid), 30, 3), "wait detach portable disk fail")
        print "pass!!!"
        pass;
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������WEB�Զ����ɲ�ѯ���ݲ���д
        ���Բ��裺1��
        '''
        pass;
        
# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()

    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����ͨ�û���ѯ�������Ϣ
        ���Բ��裺1����ͨ�û���¼
                  2��ִ�в�ѯ������Ĳ���
                  3����ͨ�û����Բ�ѯ�������Ϣ
                  4��������ʾ�����Ŀ
                  5����ͨ�û����Բ�ѯ�������Ϣ
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

# ������ģ�����ִ�������
def all_test_entry(environment):
    #UnitTestPortableDiskCreate(environment).test_entery()
    UnitTestPortableDiskDelete(environment).test_entery()
    #UnitTestPortableDiskAttach(environment).test_entery()
    #UnitTestPortableDiskDetach(environment).test_entery()

# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "�ƶ��̲��� RPC"
    #cidoc.write_doc(UnitTestVmcfgFullQuery, module_name)
    #cidoc.write_doc(UnitTestVmcfgGetIdByName, module_name)
    #cidoc.write_doc(UnitTestVmcfgQuery, module_name)
    #cidoc.write_doc(UnitTestVmcfgQueryRelation, module_name)
    #cidoc.write_doc(UnitTestVmcfgQueryVmSynState, module_name)
    #cidoc.write_doc(UnitTestVmcfgRelationSet, module_name)
    #cidoc.write_doc(UnitTestVmcfgAction, module_name)
    #cidoc.write_doc(UnitTestVmcfgAllocate, module_name)



# ��������ִ�����λ��
if __name__ == "__main__":
        file_name = sys.argv[0]
        # ��ȡ���ò����ľ��
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # ��ȡ����׼���������������ʼ��
        environment = prepare_environment(config_para_handler)
        environment.wait_sys_ok()
        environment.prepare_public_environment()
        all_test_entry(environment)
