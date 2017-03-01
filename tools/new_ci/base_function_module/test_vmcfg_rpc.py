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
from interface.base_interface.rpc import vmcfg
from interface.vm_advance_package import vm_module

class UnitTestVmcfgFullQuery(unittest.TestCase):
    '''
    �������ѯ(tecs.vmcfg.full.query)
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


class UnitTestVmcfgGetIdByName(unittest.TestCase):
    '''
    ������������Ʋ�ѯid(tecs.vmcfg.get_id_by_name)
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


class UnitTestVmcfgQuery(unittest.TestCase):
    '''
    �������ѯ(tecs.vmcfg.query)
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


class UnitTestVmcfgQueryRelation(unittest.TestCase):
    '''
    ��ѯ����������ϵ(tecs.vmcfg.query_relation)
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


class UnitTestVmcfgQueryVmSynState(unittest.TestCase):
    '''
    �������ѯ(tecs.vmcfg.query_vm_syn_state)
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


class UnitTestVmcfgRelationSet(unittest.TestCase):
    '''
    ��������������ϵ��(tecs.vmcfg.relation.set)
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


class UnitTestVmcfgAction(unittest.TestCase):
    '''
    ���������(tecs.vmcfg.action)
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


class UnitTestVmcfgAllocate(unittest.TestCase):
    '''
    ���������(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # ���ô����ӿ�
        cfg_base = {}
        cfg_base["description"] = ""
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(256 * 1024 * 1024)
        cfg_base["machine"] = {"id":1L,"position":1,"size":33565184L,"bus":"ide","type":"machine","target":"hda","fstype":"","reserved_backup":0}
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["bootloader"] = ""
        cfg_base["virt_type"] = "hvm"
        cfg_base["hypervisor"] = "xen"
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        #cfg_base["disks"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3","reserved_backup":0},{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdc","position":1,"fstype":"ext3","reserved_backup":0}]
        cfg_base["nics"] = []
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["vnc_passwd"] = ""
        cfg_base["enable_coredump"] = False
        cfg_base["enable_serial"] = False
        cfg_base["enable_livemigrate"] = False
        cfg_base["devices"] = []
        #cfg_base["kernel"] = {"id":-1L,"size":0L}
        #cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}   
        
        
        cfg_info = {}
        cfg_info["base_info"] = cfg_base
        cfg_info["vm_name"] = "vm_top"
        cfg_info["project_name"] = "p1" 

        cfg = {}
        cfg["cfg_info"] = cfg_info
        cfg["vm_num"] = 3    
   
        self.assertTrue(self.vm_ctrl.allocate(cfg), "create vm fail")
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

class UnitTestVmcfgModify(unittest.TestCase):
    '''
    ���������(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # ���ô����ӿ�
        cfg_base = {}
        cfg_base["description"] = ""
        cfg_base["vcpu"] = 1
        cfg_base["tcu"] = 1
        cfg_base["memory"] = long(128 * 1024 * 1024)
        cfg_base["machine"] = {"id":1L,"position":1,"size":41943040L,"bus":"ide","type":"machine","target":"hda","fstype":""}
        cfg_base["root_device"] = ""
        cfg_base["kernel_command"] = ""
        cfg_base["bootloader"] = ""
        cfg_base["virt_type"] = "hvm"
        cfg_base["hypervisor"] = "xen"
        cfg_base["rawdata"] = ""
        cfg_base["context"] = {}
        cfg_base["qualifications"] = []
        #cfg_base["disks"] = []
        cfg_base["disks"] = [{"id":-1L,"size":long(128 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdb","position":1,"fstype":"ext3"},{"id":-1L,"size":long(256 * 1024 * 1024),"bus":"ide","type":"disk","target":"hdc","position":1,"fstype":"ext3"}]
        cfg_base["nics"] = []
        cfg_base["cluster"] = ""
        cfg_base["host"] = ""
        cfg_base["vm_wdtime"] = 0
        cfg_base["vnc_passwd"] = ""
        cfg_base["enable_coredump"] = False
        cfg_base["enable_serial"] = False
        cfg_base["enable_livemigrate"] = False
        cfg_base["devices"] = []
        #cfg_base["kernel"] = {"id":-1L,"size":0L}
        #cfg_base["ramdisk"] = {"id":-1L,"size":0L,"bus":"scsi"}   
        
        
        cfg_info = {}
        cfg_info["base_info"] = cfg_base
        cfg_info["vm_name"] = "vm_top"
        cfg_info["project_name"] = "p1" 
        
        cfg = {}
        cfg["cfg_info"] = cfg_info
        cfg["vid"] = 1L    
   
        new_vid = self.vm_ctrl.set(cfg)
        if new_vid<0 :
            self.assertTrue(False, "create vm fail")
        else:
            print "set vm id = %d success " % cfg["vid"]
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

class UnitTestVmcfgAllocateByVt(unittest.TestCase):
    '''
    ���������(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # ���ô����ӿ�
        
        
        
        allocate_info = {}
        allocate_info["vm_num"] = 3
        allocate_info["vt_name"] = "vt1"
        allocate_info["project_name"] = "p1" 

        self.assertTrue(self.vm_ctrl.allocatebyvt(allocate_info), "create vm by vt fail")
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

class UnitTestVmImageBackupRestore(unittest.TestCase):
    '''
    �ָ����������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ���Բ��裺
        '''
        # �޸���������񱸷���Ϣ
        self.assertTrue(self.vm_ctrl.restore_image_backup(1L, "73257119-92df-4204-998b-8cf165c32c1b", "hda"), "restore vm image backup fail")
        
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

class UnitTestVmcfgVmClone(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ���Բ��裺
        '''
        # �޸���������񱸷���Ϣ
        vid = 1L
        new_id = self.vm_ctrl.vm_clone(vid)
        if new_id >0:
            print "clone vm new_id=%s" % new_id
        else:
            print "fail clone vm " 
            self.assertTrue(False, "clone vm fail")
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


class UnitTestVmStaticInfoQuery(unittest.TestCase):
    '''
    �������ѯ(tecs.vmcfg.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        type =  "query_all"
        vid = 1L
        result = self.vm_ctrl.query_staticinfo(type, vid)
        if result == True:
            print "query vm static info success"
        else:
            print "query vm static info fail" 
            self.assertTrue(False, "fail query vm static info ")
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

class UnitTestVmRunningInfoQuery(unittest.TestCase):
    '''
    �������ѯ(tecs.vmcfg.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        type =  "query_all"
        vid = 0L
        result = self.vm_ctrl.query_runninginfo(type, vid)
        if result == True:
            print "query vm running info success"
        else:
            print "query vm running info fail" 
            self.assertTrue(False, "fail query vm running info ")
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

class UnitTestVmcfgImageBackupCreate(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ���Բ��裺
        '''
        # �޸���������񱸷���Ϣ
        self.assertTrue(self.vm_ctrl.create_image_backup(1L, "hda"), "create vm image backup fail")
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

class UnitTestVmcfgImageBackupDelete(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.delete)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ���Բ��裺
        '''
        # �޸���������񱸷���Ϣ
        self.assertTrue(self.vm_ctrl.delete_image_backup(1L, "ead32593-0e75-458c-a8eb-395d1e32018a"), "delete vm image backup fail")
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
        
class UnitTestVmcfgVmClone(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ���Բ��裺
        '''
        # �޸���������񱸷���Ϣ
        new_id = self.vm_ctrl.vm_clone(1L)
        if new_id >0:
            print "clone vm new_id=%s" % new_id
        else:
            print "fail clone vm " 
            self.assertTrue(False, "clone vm fail")
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

class UnitTestVmAddAffinityRegion(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ������ͼ��
        ���Բ��裺
        '''
        vids = [16L]
        cfg = {}
        cfg["affinity_region_id"] = 1L
        cfg["vids"] = vids
        ret = self.vm_ctrl.vm_add_ar(cfg)
        if ret==True:
            print "success add vm in affinity region"
        else:
            print "fail add vm in affinity region" 
            self.assertTrue(False, "add vm in affinity region fail")

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

class UnitTestVmDelAffinityRegion(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ������ͼ��
        ���Բ��裺
        '''
        vids = [1L,2L,3L,11L]
        cfg = {}
        cfg["affinity_region_id"] = 1L
        cfg["vids"] = vids
        ret = self.vm_ctrl.vm_del_ar(cfg)
        if ret==True:
            print "success delete vm in affinity region"
        else:
            print "fail delete vm in affinity region" 
            self.assertTrue(False, "delete vm in affinity region fail")

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

class UnitTestVmMigrate(unittest.TestCase):
    '''
    �������������(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        ����˽�л���׼����ֻ�ڸ��ļ�����Ч�����¸��ļ��У�����Ч�ˣ�,���ļ���RPC�ӿڲ����ļ����벻Ҫ�����������
        ��׼�������ŵ��������ŵ������Ҫ�ڸ�RPC�ӿڲ��Խ�����ʱ�򣬽����ֶ������٣��Ա�֤��Ӱ������RPC�ӿڲ���
        '''
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
        ������ͼ��
        ���Բ��裺
        '''
        cfg = {}
        cfg["vid"] = 1L
        cfg["hid"] = 1L
        cfg["bForced"] = False
        cfg["bLive"] = True

        ret = self.vm_ctrl.vm_migrate(cfg)
        if ret==True:
            print "success migrate vm in affinity region"
        else:
            print "fail migrate vm " 
            self.assertTrue(False, "fail migrate vm")

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


# ������ģ�����ִ�������
def all_test_entry(environment):
    UnitTestVmcfgFullQuery(environment).test_entery()
    UnitTestVmcfgGetIdByName(environment).test_entery()
    UnitTestVmcfgQuery(environment).test_entery()
    UnitTestVmcfgQueryRelation(environment).test_entery()
    UnitTestVmcfgQueryVmSynState(environment).test_entery()
    UnitTestVmcfgRelationSet(environment).test_entery()
    UnitTestVmcfgAction(environment).test_entery()
    #UnitTestVmcfgAllocate(environment).test_entery()
    #UnitTestVmcfgModify(environment).test_entery()
    #UnitTestVmcfgAllocateByVt(environment).test_entery()
    #UnitTestVmcfgVmClone(environment).test_entery()
    #UnitTestVmStaticInfoQuery(environment).test_entery()
    #UnitTestVmRunningInfoQuery(environment).test_entery()
    #UnitTestVmImageBackupRestore(environment).test_entery()
    #UnitTestVmcfgImageBackupCreate(environment).test_entery()
    #UnitTestVmcfgImageBackupDelete(environment).test_entery()
    #UnitTestVmAddAffinityRegion(environment).test_entery()
    #UnitTestVmDelAffinityRegion(environment).test_entery()
    #UnitTestVmMigrate(environment).test_entery()

# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "��������� RPC"
    cidoc.write_doc(UnitTestVmcfgFullQuery, module_name)
    cidoc.write_doc(UnitTestVmcfgGetIdByName, module_name)
    cidoc.write_doc(UnitTestVmcfgQuery, module_name)
    cidoc.write_doc(UnitTestVmcfgQueryRelation, module_name)
    cidoc.write_doc(UnitTestVmcfgQueryVmSynState, module_name)
    cidoc.write_doc(UnitTestVmcfgRelationSet, module_name)
    cidoc.write_doc(UnitTestVmcfgAction, module_name)
    cidoc.write_doc(UnitTestVmcfgAllocate, module_name)



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