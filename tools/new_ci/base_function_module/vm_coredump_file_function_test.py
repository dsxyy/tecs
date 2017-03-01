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


class FunctionTestCoredumpFile(unittest.TestCase):
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
        ������ͼ������HCģ���Ƿ���޸�coredump���ļ���Ϊcore.ok
        ���Բ��裺1������һ������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.52-instance_x.2.core �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  4������HC���̣���Ҫģ����������domid��-1�仯�����ڵ�ʱ�򣬴����޸��ļ����Ƶ�����
                  4���ȴ�1���Ӻ󣬼�⿴�Ƿ����2013-0130-0335.52-instance_x.2.core.ok.tar.gz�ļ���Ӧ�ô���
                  5��ɾ�����Բ�����tar�ļ����ָ��»���
        '''
        pass; 

        
    def normal_test_level_3_6(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������coredump�ļ���û�б�HC��Ϊ��Ч�Ļ����Ƿ��ѹ��
        ���Բ��裺1������һ������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.52-instance_1.2.core �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  4���ȴ�1���Ӻ󣬼�⿴�Ƿ����2013-0130-0335.52-instance_1.2.core.tar.gz�ļ���Ӧ�ò�����
                  5��ɾ�����Բ�����tar�ļ����ָ��»���
        '''
        pass; 

        
    def normal_test_level_3_5(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������������coredump�ļ������Ƿ��ܹ���ȷ�ı�ѹ��
        ���Բ��裺1������һ������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.52-instance_1.2.core.ok �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  4��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.53-instance_12.9.core.ok �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  5���ȴ�1���Ӻ󣬼�⿴�Ƿ����2013-0130-0335.52-instance_1.2.core.ok.tar.gz�ļ���Ӧ�ô���
                  6��ͬʱ��⣬��⿴�Ƿ����2013-0130-0335.53-instance_12.9.core.ok.tar.gz�ļ���Ӧ�ô���
                  7��ɾ�����Բ���������tar�ļ����ָ��»���
        '''
        pass; 
        
    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��ͬһ��������������coredump�ļ���ֻ������µ�һ��coredump�ļ�����ѹ���ļ�
        ���Բ��裺1������һ������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.52-instance_1.2.core.ok �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  4��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.53-instance_1.9.core.ok �ļ�����СΪ1M���ң�Ӧ�óɹ�
                  5���ȴ�1���Ӻ󣬼�⿴�Ƿ����2013-0130-0335.54-instance_1.9.core.ok.tar.gz�ļ���Ӧ�ô���
                  6��ͬʱ��⣬���Ƿ���ڶ��tar�ļ���Ӧ�ò�����
                  7��ɾ��2013-0130-0335.52-instance_x.9.core.ok.tar.gz�ļ����ָ��»���
        '''
        pass;

        
    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���鿴coredump�ļ������󣬿��Ƿ��ܹ���ѹ��
        ���Բ��裺1������һ������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3��ͨ��ssh ��/var/lib/xen/dump/instance_x ����д��һ��2013-0130-0335.52-instance_x.3.core.ok �ļ�����СΪ10M���ң�Ӧ�óɹ�
                  4���ȴ�1���Ӻ󣬼�⿴�Ƿ����2013-0130-0335.52-instance_x.3.core.ok.tar.gz�ļ���Ӧ�ô���
                  5��ͬʱ��⣬��2013-0130-0335.52-instance_x.3.core.ok����ļ��Ƿ�ɾ����Ӧ��ɾ��
                  6��ɾ��2013-0130-0335.52-instance_x.3.core.ok.tar.gz�ļ����ָ��»���
        '''
        pass;
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Ե����������coredumpѡ��Ƿ��ܹ�������Ӧ�������ļ�
        ���Բ��裺1��������������������������������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3�����/var/lib/tecs/instances/instances_x.xml ����crash ѡ�������Ƿ���� coredump-restart��Ӧ�ó���
                  4��ͨ��df��⣬����/var/lib/xen/dump/instance_x �Ƿ��Ѿ���mount���ˣ�Ӧ�ó��ֹ�mount
                  5��ͨ��df��⣬����/var/lib/xen/dump/instance_x+1 �Ƿ��Ѿ���mount���ˣ�Ӧ�ó��ֹ�mount
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Ե������û������coredumpѡ��Ƿ��ܹ�������Ӧ�������ļ�
        ���Բ��裺1������һ���������������������coredumpѡ��
                  2����������������ȴ�����ɹ�
                  3�����/var/lib/tecs/instances/instances_x.xml ����crash ѡ�������Ƿ���� coredump-restart����Ӧ�ó���
                  4��ͨ��df��⣬����/var/lib/xen/dump/instance_x �Ƿ��Ѿ���mount����,��Ӧ�ó��ֹ�mount 
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
        ������ͼ���ޣ���ʵ��������津��coredump��ʱ����ʵ�֣��޷������������������ʱ����쳣��
        ���Բ��裺1����
        '''
        pass;
        
    def test_entery(self):
        # �������
        self.normal_test_level_2()
        self.Exception_test_level_2()

# ������ģ�����ִ�������
def TestEntery(environment):
    FunctionTestCoredumpFile(environment).test_entery()

# ģ���ĵ���������
def DocEntery(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "CoredumpFile����"
    cidoc.write_doc(FunctionTestCoredumpFile, module_name)



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
