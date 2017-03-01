#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �������ݣ�ð�̹��ܲ���
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/12/28
#*******************************************************************************/
import sys, os,time
import unittest 

# �����������
from interface.environment_prepare_package import * 
from interface.config_para import config_para
from interface.base_interface.rpc.contrib import tecs_common
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# ����ʹ�õ��ĸ߼���װ����
from interface.image_advance_package import image_module
from interface.vm_advance_package import vm_module


class FunctionTestSmoking(unittest.TestCase):
    '''
    TECSð�̲���
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        # ִ�е�ǰ������Ļ���
        self.class_test_environment_prepare()
        # ����˽�п���,֮��ָ������Ե�ǰ��Ļ������лָ�
        self.private_snapshot = private_snapshot(self.environment, self.function_test_environment_prepare)

    def class_test_environment_prepare(self):
        # �ϴ�ӳ����ȥ���״ε������ϴ�
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)

        
    def function_test_environment_prepare(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�
        '''
        # ��ȡ��imageģ��Ĺ������
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)
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
        self.vm_ctrl.add_base("vm1", 1, 1, 256, image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
        self.vm_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
        self.vm_ctrl.add_context("test", "aaaaa")

    
# ��������
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_1()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_2()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_3()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_4()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_5()

    def normal_test_level_3_6(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������ƶ��̹���(����ttylinux������������֧���Ȳ���ƶ���,����ֻ�ܲ��ùػ�->attach->�����ķ�ʽ����)
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2�����������
                  3�������ƶ���
                  4���ػ������
                  5��attach�ƶ���
                  6�����������
                  7���ػ������
                  8��detach�ƶ���
                  9��ɾ���ƶ���
        '''
        ## ���������
        #vid = self.vm_ctrl.create()
        #self.assertNotEqual(vid, -1, "create vm fail")
        ## ���������
        #self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        ## ������������ɹ��������״̬�Ƿ���ȷ
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        ## �����ƶ���
        #self.db_vm_check_ctrl.check_pd_storage_cluster()
        #request_id = self.vm_ctrl.create_portable_disk(134217728L, "tecscluster", "tecs")
        #self.assertNotEqual(request_id, False, "create portable disk fail!")
        ## ��������ƶ�����ر��Ƿ��ж�Ӧ��¼
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 0), "wait create portable disk fail")

        ## �ػ������
        #self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        ## ��������״̬Ϊ�ػ�̬
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")

        ## attach�ƶ���
        #vid=self.db_vm_check_ctrl.get_all_deploy_vid()
        #request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #self.assertNotEqual(request_id, False, "get request_id false!")
        #self.assertTrue(self.vm_ctrl.attach_portable_disk(request_id, long(vid[0]), "hdb", "ide"), "attach portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid[0]), 30, 2), "wait attach portable disk fail")

        ## ����
        #self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        ## ��������Ϊ����̬
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
        ## �ػ������
        #self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        ## ��������״̬Ϊ�ػ�̬
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")

        ## detach�ƶ���
        #request_id, vid = self.db_vm_check_ctrl.pd_get_attach_request_id_and_vid()
        #self.assertNotEqual(request_id, False, "get request_id and vid false!")
        #self.assertTrue(self.vm_ctrl.detach_portable_disk(long(vid), request_id), "detach portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid), 30, 3), "wait detach portable disk fail")

        ## ɾ���ƶ���
        #request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #self.assertNotEqual(request_id, False, "get request_id false!")
        #self.assertTrue(self.vm_ctrl.delete_portable_disk(request_id), "delete portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 1), "wait delete portable disk fail")

    def normal_test_level_3_5(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��ģ�ⵥ�����������������״̬�Ƿ��ܹ��ָ�����
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2�����������
                  3����ȡdomid,�������һ�����ID�Ƿ�仯
                  4��ģ�ⵥ������
                  5����������״̬DomID��Ӧ�÷��ͱ仯(60s�ȴ����)
        '''
        # ���������
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # ��ӡ��ռ�ռ������10�ű�
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # ������������ɹ��󣬲�ѯ�������domid,�����һ��ʹ��
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        # ģ��HC��������
        self.ssh_tecs_run_ctl.SimHcBoardReboot()
        # ��������domid
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))

       
    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��ģ��HC���������������״̬�Ƿ�仯
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2�����������
                  3����ȡdomid,�������һ�����ID�Ƿ�仯
                  4������HC����
                  5����������״̬DomID����Ӧ�ñ仯(�10Sʱ�����)
        '''
        # ���������
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # ��ӡ��ռ�ռ������10�ű�
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # ������������ɹ��󣬲�ѯ�������domid,�����һ��ʹ��
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        # ����HC����
        self.ssh_tecs_run_ctl.restart_hc_server()
        # ��������domid
        self.assertFalse(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 10))

    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������������������ԣ��������ػ���������ǿ�ƹػ�
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2�����������
                  3������Ƿ���ɹ�
                  4���ر������
                  5�����������Ƿ���ڣ�Ӧ�ò�����
                  6���������
                  7�����������Ƿ���ڣ�Ӧ�ô���
                  8��ǿ�ƹر������
                  9�����������Ƿ���ڣ�Ӧ�ò�����
        '''
        # ���������
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # ��ӡ��ռ�ռ������10�ű�
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # ������������ɹ��������״̬�Ƿ���ȷ
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        # ���������
        self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        # ��������״̬Ϊ�ػ�̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")
        # ����
        self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        # ��������Ϊ����̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
        # ǿ�ƹػ�
        self.assertTrue(self.vm_ctrl.destory(), "destory Vm Fail")
        # ��������Ϊ�ػ�̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait destory fail")
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������������������ԣ����������𣬽������
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2�����������
                  3������Ƿ���ɹ�
                  4�����������
                  5����������״̬��Ӧ�ù���
                  6���������
                  7����������״̬��Ӧ������
        '''
        # ���������
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # ��ӡ��ռ�ռ������10�ű�
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # ������������ɹ��������״̬�Ƿ���ȷ
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        # ���������
        self.assertTrue(self.vm_ctrl.pause(), "pause Vm Fail")
        # ��������״̬Ϊ����̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_pause, 60), "wait pause fail")
        # �������
        self.assertTrue(self.vm_ctrl.resume(), "resume Vm Fail")
        # ��������Ϊ����̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait resume fail")
        
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����������������Ĳ��ԣ�����,����������������ǿ�����������������ٴβ���
        ���Բ��裺1��ʹ�û���׼���е���������ã����д���
                  2������������
                  3����ȡ�ղ���������DomID
                  4�����������(��ȴ�60S�������Ż�ɹ�)
                  5�����������������DomID�Ƿ��ͱ仯��Ӧ�ñ仯
                  6��ǿ�����������
                  7����������ID�Ƿ��ͱ仯��Ӧ�ñ仯
        '''
        # ���������
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        
        # ��ʱ1���ӣ��ȴ�CCע�ᵽTC
        time_delay(60, "wait cc register to tc") 
        
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # ��ӡ��ռ�ռ������10�ű�
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # ��ȡ�������domid
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        time_delay(62, "reboot test need delay") 
        # reboot�����
        self.assertTrue(self.vm_ctrl.reboot(), "Reboot Vm Fail")
        # ���domid�仯
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 120))
        # reset �����
        time_delay(62, "reset test need delay") 
        self.assertTrue(self.vm_ctrl.reset(), "Reset Vm Fail")
        # ���domid�����仯
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.Exception_test_level_3_1()

    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ����
        ���Բ��裺1����
        '''
        pass;

    def test_entery(self):
        # �������
        self.normal_test_level_2()
        self.Exception_test_level_2()

# ������ģ�����ִ�������
def TestEntery(environment):
    FunctionTestSmoking(environment).test_entery()

# ģ���ĵ���������
def DocEntery(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "TECSð�̲���"
    cidoc.write_doc(FunctionTestSmoking, module_name)



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
        # ִ�в���
        TestEntery(environment)
