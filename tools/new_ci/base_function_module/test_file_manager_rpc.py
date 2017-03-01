#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/12/26
#*******************************************************************************/
import sys, os
import unittest 


# ���������װ
from interface.base_interface.rpc.contrib import tecs_common
from interface.environment_prepare_package import prepare_environment
from interface.config_para import config_para

# ���Խӿڷ�װ
from interface.base_interface.rpc import file_manager

class UnitTestQuery_coredump_url(unittest.TestCase):
    '''
    ��ѯcoredump�ļ���URL(tecs.file.query_coredump_url)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
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
        ������ͼ�����ò�ѯ�ӿڣ����Ƿ��ܹ���ѯ�ӿڳɹ�
        ���Բ��裺1������tecs.file.query_coredump_url
                  2��Ӧ�÷��سɹ�
        '''
        f = file_manager.file_manager(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(f.query())
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���ޣ���ѯ�ӿڣ��޲��������쳣���ԣ�
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
        ������ͼ���ޣ���ѯ�ӿڣ�����Σ��ޱ߽���ԣ�
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
        ������ͼ��ʹ���⻧Ȩ�ޣ�����ѯ�ӿ��Ƿ���óɹ�
        ���Բ��裺1��ʹ���⻧��¼
                  2�����ò�ѯ�ӿڣ�Ӧ��ʧ��
        '''
        f = file_manager.file_manager(self.config_para.server_addr, self.environment.get_user_session())
        self.assertFalse(f.query())

    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


# ������ģ�����ִ�������
def all_test_entry(environment):
    UnitTestQuery_coredump_url(environment).test_entery()


# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "file����RPC"
    cidoc.write_doc(UnitTestQuery_coredump_url, module_name)

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