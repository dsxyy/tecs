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
from interface.base_interface.rpc import trunk

class UnitTestTrunkDel(unittest.TestCase):
    '''
    ɾ��trunk������(tecs.trunk.delete)
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



class UnitTestTrunkNetplaneSet(unittest.TestCase):
    '''
    ����trunk������ƽ��(tecs.trunk_netplane.set)
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



class UnitTestTrunkQuery(unittest.TestCase):
    '''
    ��ѯtrunk����Ϣ(tecs.trunk.query)
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



class UnitTestTrunkSet(unittest.TestCase):
    '''
    ����trunk������(tecs.trunk.set)
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



# ������ģ�����ִ�������
def all_test_entry(environment):
    UnitTestTrunkDel(environment).test_entery()
    UnitTestTrunkNetplaneSet(environment).test_entery()
    UnitTestTrunkQuery(environment).test_entery()
    UnitTestTrunkSet(environment).test_entery()


# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "trunk�˿ڹ��� RPC"
    cidoc.write_doc(UnitTestTrunkDel, module_name)
    cidoc.write_doc(UnitTestTrunkNetplaneSet, module_name)
    cidoc.write_doc(UnitTestTrunkQuery, module_name)
    cidoc.write_doc(UnitTestTrunkSet, module_name)



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