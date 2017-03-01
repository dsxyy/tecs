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
#from interface.image_advance_package import image_module
#from interface.vm_advance_package import vm_module


class FunctionTestXXX(unittest.TestCase):
    '''
    xxx����
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

        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��xxxxxx
        ���Բ��裺1��xxxxxx
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��xxxxxx
        ���Բ��裺1��xxxxxx
        '''
        pass;

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
    FunctionTestXXX(environment).test_entery()

# ģ���ĵ���������
def DocEntery(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "xxx����ģ��"
    cidoc.write_doc(FunctionTestXXX, module_name)



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
