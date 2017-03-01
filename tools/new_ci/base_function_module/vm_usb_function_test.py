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


class FunctionTestUsbPassthrough(unittest.TestCase):
    '''
    �������������ʱ�򣬱���Coredump�ļ�����
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
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_3()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_4()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_5()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_6()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_7()
        
    def normal_test_level_3_7(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������һ��������������棬ͬһ��USB�豸��������
        ���Բ��裺1������1���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5������Ƿ�ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��Ӧ�óɹ���
                  6���ٴη���һ��ͬһ��USB�������
                  7������Ƿ�ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��Ӧ�óɹ���
        '''
        pass;
   
    def normal_test_level_3_6(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������һ��������������棬������һ���������USB�豸
        ���Բ��裺1������2���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5��Ȼ��ʹ��RPC�ӿ�����һ����������棬���Ͳ���USB���Ӧ��ʧ�ܡ�
        '''
        pass;

    def normal_test_level_3_5(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������һ��������������棬�γ���һ���������USB�豸
        ���Բ��裺1������2���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5��Ȼ��ʹ��RPC�ӿ�����һ����������棬���Ͱγ����Ӧ��ʧ�ܡ�
        '''
        pass;
        
    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������һ��������������棬�鿴��һ���������USB��Ϣ
        ���Բ��裺1������2���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5��Ȼ��ʹ��RPC�ӿڲ�ѯ��һ���������USB��Ϣ��Ӧ��ѯ��������ʹ�ã���ʹ������ϢҲ����
        '''
        pass;

    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������������U��
        ���Բ��裺1������1���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5���ȴ�����ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��
                  6���ٷ��͸���USB������
                  7���ȴ�����ִ�гɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)
                  8��ʹ��������Ĵ��ڹ��ܣ������������Ӧ��USB�豸����
        '''
        pass;

    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������������γ�usb�Ĺ���
        ���Բ��裺1������1���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5���ȴ�����ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��
                  6���ٷ��Ͱγ�USB������
                  7���ȴ�����ִ�гɹ�
                  8��ʹ��������Ĵ��ڹ��ܣ������������USB�豸Ӧ�ò�����
                  9��ʹ��RPC�ӿڲ�ѯ��֮ǰ��USB�豸�Ƿ��Ѿ����ڿ���״̬��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������������usb�Ĺ���
        ���Բ��裺1������һ���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB��
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5���ȴ�����ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��
                  6��ʹ��������Ĵ��ڹ��ܣ������������Ӧ�÷�����һ��USB�豸(cat /proc/bus/usb/device)
                  7��ʹ��RPC�ӿڲ�ѯ��֮ǰ��USB�豸�Ƿ��Ѿ�����ʹ��״̬��
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
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Ե�����USB�����ʱ��CC�����쳣���������
        ���Բ��裺1������1���������������������ɹ�
                  2����ѯ�������������ͨ��RPC�ӿڼ��ÿ���������Ӧ�����Ƿ���USB
                  3�����û��USB�����˳�����������
                  4�������⵽����USB�豸�����Ͳ���USB������
                  5��ͨ��ssh�������cc��������
                  6�������������Ƿ�ɹ�(ͨ��db�жϲ��������Ƿ�Ϊ�ɹ�)��Ӧ�óɹ���
        '''
        pass;
        
    def test_entery(self):
        # �������
        self.normal_test_level_2()
        self.Exception_test_level_2()

# ������ģ�����ִ�������
def TestEntery(environment):
    FunctionTestUsbPassthrough(environment).test_entery()

# ģ���ĵ���������
def DocEntery(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "Usb͸������"
    cidoc.write_doc(FunctionTestUsbPassthrough, module_name)



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
