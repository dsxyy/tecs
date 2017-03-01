#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �������ݣ����ܼ���Ĳ���ģ��
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/12/28
#*******************************************************************************/
import sys, os
import unittest 

# �����������
from interface.environment_prepare_package import * 
from interface.config_para import config_para
from interface.base_interface.rpc.contrib import tecs_common
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# ����ʹ�õ��ĸ߼���װ���ã�������Ҫ����������
from interface.image_advance_package import image_module
from interface.vm_advance_package import vm_module


class FunctionTestVmAction(unittest.TestCase):
    '''
    �������������
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
        self.vm_ctrl.add_disk(256, "scsi", "sda", 1, "ext4")
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

    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������������Ǩ��
        ���Բ��裺1�������������ӳ���Ӳ�̵Ĵ���λ�ö�ѡ�����ء���Ȼ����
                  2���粿��ʧ�ܣ��������˳�����
                  3���粿��ɹ����������״̬Ϊ��running��ʱ��������Ǩ�ƣ�Ӧ��ʧ��
                  4���������������޸�ӳ���Ӳ�̵Ĵ���λ�ö�Ϊ�����������²���
                  5���粿��ʧ�ܣ��������˳�����
                  6���������״̬Ϊ��running��ʱ��������Ǩ�ƣ�Ӧ�ɹ�����ʱ30������ݿ�vm_pool��hid�ֶ�Ӧ�仯��
                  7����������ػ���������Ǩ�ƣ�Ӧʧ�ܣ����ݿ�vm_pool��hid�ֶ��ޱ仯
        '''
        pass;
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������������£����Բ��Բ������²���
        ���Բ��裺1����ȡ���������������ID�������ȡʧ�ܣ��������˳�����
                  2���������𣬵ȴ�������ɣ�Ӧ�óɹ�
                  3���ٴβ������������������Ƿ�ɹ���Ӧ�õ��ɹ�
        '''
        upgrade_vm_id = self.environment.get_upgrade_test_vm()
        if upgrade_vm_id == -1:
            return
        self.vm_ctrl.set_vmid(upgrade_vm_id)
        # ��ȡ��������ڼ�Ⱥ
        cluster_name = self.db_vm_check_ctrl.get_cluster_name_by_vid(upgrade_vm_id)
        # ���������
        self.assertTrue(self.vm_ctrl.cancel(), "cancel Vm Fail")
        # �ȴ��������ȡ����¼д��vm_cancel_pool��
        time_delay(1, "wait vm_cancel item is write!")
        # �������Ƿ�ɹ�
        self.assertTrue(self.db_vm_check_ctrl.check_vm_cancel_success_with_time_out(cluster_name, upgrade_vm_id, 60))
        # ���������
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # �������������Ƿ�ɹ�
        self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������������£������������ػ�������
        ���Բ��裺1����ȡ���������������ID�������ȡʧ�ܣ��������˳�����
                  2���������������������Ƿ������ɹ���Ӧ�óɹ�
                  3���رո�������������������Ƿ�رճɹ���Ӧ�óɹ�
                  4����������������Ӧ�ô򿪳ɹ�
        '''
        upgrade_vm_id = self.environment.get_upgrade_test_vm()
        if upgrade_vm_id == -1:
            return
        self.vm_ctrl.set_vmid(upgrade_vm_id)
        # ��ȡ����ǰ�������domid
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        # ���������
        self.assertTrue(self.vm_ctrl.reboot(), "reboot Vm Fail")
        # �ټ������Ƿ����
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_run, 60), "wait reboot fail")
        # ���domid�仯
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))
        # ǿ�ƹػ�
        self.assertTrue(self.vm_ctrl.destory(), "destory Vm Fail")
        # ��������Ϊ�ػ�̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_shutoff, 60), "wait destory fail")
        # ����
        self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        # ��������Ϊ����̬
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_run, 60), "wait start fail")


# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.Exception_test_level_3_1()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��xxxxxx
        ���Բ��裺1��xxxxxx
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��xxxxxx
        ���Բ��裺1��xxxxxx
        '''
        pass;
        
    def test_entery(self):
        # �������
        self.normal_test_level_2()
        self.Exception_test_level_2()

# ������ģ�����ִ�������
def TestEntery(environment):
    FunctionTestVmAction(environment).test_entery()

# ģ���ĵ���������
def DocEntery(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "�������������"
    cidoc.write_doc(FunctionTestVmAction, module_name)



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
        TestEntery(environment)
