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
from interface.base_interface.rpc import project
from interface.vm_advance_package import vm_module

class UnitTestProjectAllocate(unittest.TestCase):
    '''
    �������������(tecs.project.allocate)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectDel(unittest.TestCase):
    '''
    ɾ�����������(tecs.project.delete)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectQuery(unittest.TestCase):
    '''
    ��������̲�ѯ(tecs.project.query)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectQuery_vm_by_project(unittest.TestCase):
    '''
    ��ѯ�ù�����ӵ�е������(tecs.project.query_vm_by_project)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectSet(unittest.TestCase):
    '''
    �޸����������(tecs.project.set)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectSaveAsImage(unittest.TestCase):
    '''
    ����Ϊ����(tecs.project.save_as_image)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.project_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        self.assertTrue(self.project_ctrl.project_save_as_image("centos", False, True, "test"), "project save as image fail")
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        #self.Exception_test_level_2()
        #self.limit_test_level_2()
        #self.auth_test_level_2()

class UnitTestProjectCreateByImage(unittest.TestCase):
    '''
    �Ӿ��񴴽�(tecs.project.create_by_image)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.project_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        self.assertTrue(self.project_ctrl.project_create_by_image(37L, "create_by_image_4"), "project create by image fail")
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
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
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��
        ���Բ��裺1��
        '''
        pass;
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        #self.Exception_test_level_2()
        #self.limit_test_level_2()
        #self.auth_test_level_2()

# ������ģ�����ִ�������
def all_test_entry(environment):
    UnitTestProjectAllocate(environment).test_entery()
    UnitTestProjectDel(environment).test_entery()
    UnitTestProjectQuery(environment).test_entery()
    UnitTestProjectQuery_vm_by_project(environment).test_entery()
    UnitTestProjectSet(environment).test_entery()
    #UnitTestProjectSaveAsImage(environment).test_entery()
    #UnitTestProjectCreateByImage(environment).test_entery()
    


# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "���̹���RPC"
    cidoc.write_doc(UnitTestProjectAllocate, module_name)
    cidoc.write_doc(UnitTestProjectDel, module_name)
    cidoc.write_doc(UnitTestProjectQuery, module_name)
    cidoc.write_doc(UnitTestProjectQuery_vm_by_project, module_name)
    cidoc.write_doc(UnitTestProjectSet, module_name)



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