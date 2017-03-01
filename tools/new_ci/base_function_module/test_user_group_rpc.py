#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
#
# �������ݣ�
# ��ǰ�汾��1.0
# ��    �ߣ���Т��
# ������ڣ�2012/11/24
#
#*******************************************************************************/
import sys, os
import unittest 

from interface.environment_prepare_package import prepare_environment 
from interface.base_interface.rpc import user
from interface.base_interface.rpc import usergroup
from interface.base_interface.rpc.contrib import tecs_common
from interface.config_para import config_para

class UnitTestUserAdd(unittest.TestCase):
    '''
    �����û�(tecs.user.allocate)
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
    

# �������������Դ���       
    def normal_test_level_2(self):
        '''
        ��������
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()
        
    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Թ���Ա�ʺţ�������һ������Ա�ʺŵ�����
        ���Բ��裺���û���test_usergroup���棬����һ������Ա�ʺţ��û���ΪA��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����������ȷ�����벻ͨ������
        ���Բ��裺1�����û���cigroup���棬����һ����ͨ�⻧�ʺţ��û���ΪA�������ȷ�����벻ͬ��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.allocate("A", "test", "test1", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Դ�������⻧�ʺ�����
        ���Բ��裺1�����û���cigroup���棬����һ����ͨ�⻧�ʺţ��û���ΪA��Ӧ�óɹ�
                  2�����û���cigroup���棬����һ����ͨ�⻧�ʺţ��û���ΪB��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Դ���һ���⻧�ʺ�����
        ���Բ��裺���û���test_usergroup���棬����һ����ͨ�⻧�ʺţ��û���ΪA��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete("A"))

# �������쳣���Դ��� 
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()

    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Դ��������û�������
        ���Բ��裺1�����û���cigroup���棬����һ����ͨ�⻧�ʺţ��û���ΪA��Ӧ�óɹ�
                  2�����û���cigroup���棬����һ����ͨ�⻧�ʺţ��û���ΪA��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertFalse(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete("A"))


# �����Ǳ߽���Դ��� 
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������ʽ����
        ���Բ��裺1�������û���Ϊ12345678_test@zte.com.cn����ͨ�⻧�ʺţ�Ӧ���ɹ�
                  2�������û���Ϊ~12345678_test@zte.com.cn����ͨ�⻧�ʺţ�Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "~12345678_test@zte.com.cn"
        self.assertFalse(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        name = "12345678_test@zte.com.cn"
        self.assertTrue(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete(name))
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���������Ȳ���
        ���Բ��裺1�������û����Ƴ��ȵ���65���ַ��ģ���ͨ�⻧�ʺţ�Ӧ��ʧ��
                  2�������û����Ƴ��ȵ���64���ַ��ģ���ͨ�⻧�ʺţ�Ӧ���ɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz012"
        self.assertFalse(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        name = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz01"
        self.assertTrue(u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        #��ԭ���Ի���
        self.assertTrue(u.delete(name))

# ������Ȩ�޿������̲��Դ���         
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()

    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��������ͨ�⻧Ȩ������Ĳ���
        ���Բ��裺1��ʹ���⻧A����һ���⻧B��Ӧ��ʧ��
        '''
        root_u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        name = "A"
        self.assertTrue(root_u.allocate(name, "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        normal_session = user.login(self.environment.server, name, "test")
        normal_u = user.user(self.config_para.server_addr,  normal_session)
        self.assertFalse(normal_u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))

        #��ԭ���Ի���
        self.assertTrue(root_u.delete(name))
        
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Թ���ԱȨ������Ĳ���
        ���Բ��裺1��ʹ�ù���Ա����һ���û�
        '''
        self.normal_test_level_3_1()

    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()
 
class UnitTestUserDel(unittest.TestCase):
    '''
    ɾ���û�(tecs.user.delete)
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
        ������ͼ�����Թ���Աɾ��һ�������ڵ��ʺ��Ƿ�ɹ�
        ���Բ��裺1��ɾ��һ�������ڵ��ʺ�A��Ӧ��ʧ��
        '''
        root_u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(root_u.delete("A"))
        
        # �ָ�����
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Թ���Ա�Ƿ���ɾ��һ����ͨ�⻧���ʺ�
        ���Բ��裺1��ʹ�ù���Ա����һ���û�,Ȼ��ɾ�����ʻ���Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        
        # �ָ�����
        self.assertTrue(u.delete("A"))

# �쳣����
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()
        self.Exception_test_level_3_3()

        
    def Exception_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������Ƿ����ɾ��һ���Ѿ��������������û�
        ���Բ��裺1������һ����ͨ�⻧A������A�ʺ����沿��һ�������
                  2���ù���Ա�ʺŵ�¼����ɾ��A�ʺţ�Ӧ��ʧ��
        '''
        pass;
        
    def Exception_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������Ƿ����ɾ��Ĭ��admin�ʺ�
        ���Բ��裺1��������һ������Ա�ʺ�A
                  2����A�ʺŵ�¼��Ȼ��ɾ��admin�ʺţ�Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("admin"))

        # �ָ�����
        self.assertTrue(u.delete("A"))

        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������Ƿ����ɾ���Լ����ʺ�
        ���Բ��裺1��������һ������Ա�ʺ�A
                  2����A�ʺŵ�¼��Ȼ��ɾ���Լ���Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("A"))

        # �ָ�����
        self.assertTrue(u.delete("A"))
        

# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()

    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ��û�б߽�ֵ��Ҫ����
        ���Բ��裺��
        '''
        pass;

        # �ָ�����

        
# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
    
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��Ƿ����ɾ����һ���⻧���ʺ�
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2���ٴ�����һ����ͨ�⻧�ʺ�B
                  2����A�ʺŵ�¼��Ȼ��ɾ��B�ʺţ�Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        
        self.assertFalse(A_user.delete("B"))


        # �ָ�����
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
    
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestUserSet(unittest.TestCase):
    '''
    �޸��û�(tecs.user.set)
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
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()
        self.normal_test_level_3_5()
        self.normal_test_level_3_6()
        self.normal_test_level_3_7()
        self.normal_test_level_3_8()
        
        
    def normal_test_level_3_8(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա����Ĭ�Ϲ���Աadmin
        ���Բ��裺1������Ա�û�admin ����һ������Ա�ʺ�A
                  2����¼�ʺ�A��Ȼ�����admin,Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_session = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_session)
        self.assertFalse(A_user.set_enable_disable("admin", user.USER_DISABLE))

        # �ָ�����        
        self.assertTrue(u.delete("A"))      

    def normal_test_level_3_7(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա������һ������Ա
        ���Բ��裺1������Ա�û�admin ����һ������Ա�ʺ�admin_A
                  2�������ʺ�A,Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("admin_A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("admin_A", user.USER_DISABLE))
        
        # �ָ����� 
        self.assertTrue(u.delete("admin_A"))              

    def normal_test_level_3_6(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա����һ���⻧
        ���Բ��裺1������Ա�û�admin ����һ���⻧A
                  2�������⻧A,Ӧ�óɹ�
                  3��Ȼ���������⻧A��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("A", user.USER_DISABLE))
        self.assertTrue(u.set_enable_disable("A", user.USER_ENABLE))
        
        # �ָ�����        
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_5(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա����һ���⻧
        ���Բ��裺1������Ա�û�admin ����һ���⻧A
                  2�������⻧A,Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_enable_disable("A", user.USER_DISABLE))

        # �ָ�����    
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա�޸���ͨ�⻧�Ľ�ɫΪ����Ա
        ���Բ��裺1������Ա�û�admin ����һ����ͨ�⻧A
                  2���޸��⻧A�Ľ�ɫΪ����Ա��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.set_role("A", user.CLOUDADMIN))
        
        # �ָ�����
        self.assertTrue(u.delete("A"))
        
    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա�޸��Լ��Ľ�ɫ
        ���Բ��裺1������Ա�û�admin �޸��Լ��Ľ�ɫΪ��ͨ�û���Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.set_role("admin", user.ACCOUNTUSER))
        
        # �ָ�����

        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա�޸��Լ�����
        ���Բ��裺1������Ա�û�admin �޸��Լ������룬Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.set_pwd("admin", "aaaaa", "aaaaa"))
        
        # �ָ�����
        self.environment.refresh_root_session("aaaaa")
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.set_pwd("admin", "admin", "admin"))
        self.environment.refresh_root_session("admin")
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա�޸�����һ������Ľ�ɫΪ��ͨ�⻧
        ���Բ��裺1������Ա�û�admin ������һ������Աadmin_A
                  2��Ȼ�����Աadmin�޸���һ������Աadmin_A��Ȩ��Ϊ��ͨ�⻧��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("admin_A", "test", "test", user.CLOUDADMIN, self.config_para.test_usergroup))
        self.assertTrue(u.set_role("admin_A", user.ACCOUNTUSER))
        
        # �ָ�����
        self.assertTrue(u.delete("admin_A"))

# �쳣����
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()
        
        
    def Exception_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���޸��û����û��鵽һ�������ڵ��û������
        ���Բ��裺1��ʹ��Ĭ�Ϲ���Աadmin����һ���⻧�ʺ�A������Ϊtest
                  2���޸��⻧A���û���Ϊ��test_A��,Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertFalse(u.set_user_group("A", "test_A"))

        # �ָ�����
        self.assertTrue(u.delete("A"))
        
    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���޸�һ�������ڵ��û�
        ���Բ��裺1��ʹ��Ĭ�Ϲ���Աadmin�޸�һ�������ڵ��⻧�ʺ� ababbaba,Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertFalse(u.set_user_group("ababbaba", self.config_para.test_usergroup))

        # �ָ�����
        

# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()
        self.limit_test_level_3_3()
        self.limit_test_level_3_4()

    def limit_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������޸Ľ��ã����ã�������Χֵ�����(disable��Ч�ԣ�����Ƿ���Ϊ���޸ģ�������)
        ���Բ��裺1������һ���⻧A
                  2���޸��⻧A�Ľ�������ֵΪUSER_DISABLE+1��Ӧ��ʧ��
        '''

        # �ָ�����
        
    def limit_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������޸ĵĽ�ɫ���ڷ�Χ�ڵ����(��ɫ��Ч�ԣ�����Ƿ���Ϊ���޸ģ�������)
        ���Բ��裺1������һ���⻧A
                  2���޸��⻧A�Ľ�ɫΪACCOUNTUSER+1��Ӧ��ʧ��
        '''
        # �ָ�����
        
    def limit_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������޸������ȷ�������ڱ߽��ϣ���һ�����
        ���Բ��裺1������һ���⻧A
                  2���޸�����Ϊ 32��1��ȷ��������33��1��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        pwd     = "11111111111111111111111111111111"
        cof_pwd = "111111111111111111111111111111111"
        self.assertFalse(u.set_pwd("A", pwd, cof_pwd))

        # �ָ�����
        self.assertTrue(u.delete("A"))
        
    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ���������볤�ȱ߽�
        ���Բ��裺1������һ���⻧A
                  2���޸�����Ϊ 32��1��Ӧ�óɹ�
                  3���޸�����Ϊ33��1��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        limit_ok_pwd     = "11111111111111111111111111111111"
        self.assertTrue(u.set_pwd("A", limit_ok_pwd, limit_ok_pwd))
        limit_error_pwd  = "111111111111111111111111111111111"
        self.assertFalse(u.set_pwd("A", limit_error_pwd, limit_error_pwd))

        # �ָ�����
        self.assertTrue(u.delete("A"))

        
# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        self.auth_test_level_3_3()
        self.auth_test_level_3_4()
        self.auth_test_level_3_5()

    def auth_test_level_3_5(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��ÿ��Լ��ĵ绰���ʼ�����ַ��������Ϣ��userģ��û��֧���ÿչ��ܣ�
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2��ʹ���⻧A��¼���������Լ��ĵ绰���ʼ�����ַ��������Ϣ
                  3��Ȼ����ʹ�����ýӿڣ�����Լ��ĵ绰���ʼ�����ַ��������Ϣ
        '''
        
        # �ָ�����

        
    def auth_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��޸��Լ���Ⱥ����Ϣ(��Ϊ�������й��ϣ��û����������Լ����û���)
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2����¼A�ʺţ�ʹ�����ýӿڣ��޸��Լ���Ⱥ����Ϣ��Ӧ��ʧ��
        '''
        return
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        test_group = usergroup.usergroup(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(test_group.allocate("test_group", usergroup.GROUP_TYPE1))
        self.assertFalse(A_user.set_user_group("A", "test_group"))
        
        # �ָ�����
        self.assertTrue(u.delete("A"))
        self.assertTrue(test_group.delete("test_group"))

        
    def auth_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��޸��Լ��Ľ�ɫ��Ϣ
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2����¼A�ʺţ�ʹ�����ýӿڣ��޸��Լ��Ľ�ɫ��Ϣ��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertFalse(A_user.set_role("A", user.CLOUDADMIN))
        
        # �ָ�����
        self.assertTrue(u.delete("A"))
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��޸��Լ��ĵ绰���ʼ�����ַ��������Ϣ
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2����¼A�ʺţ�ʹ�����ýӿڣ��޸��Լ��ĵ绰���ʼ�����ַ��������Ϣ��Ӧ�óɹ�
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertTrue(A_user.set_phone("A", "13770511182"))
        self.assertTrue(A_user.set_email("A", "125aaaaa@sohu.com"))
        self.assertTrue(A_user.set_location("A", "zhong_xing_tong_xun"))
        self.assertTrue(A_user.set_description("A", "test description"))

        # �ָ�����.
        self.assertTrue(u.delete("A"))
        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧Ȩ���£��޸ı��˵���Ϣ
        ���Բ��裺1������һ����ͨ�⻧�ʺ�A
                  2���ٴ�����һ����ͨ�⻧�ʺ�B
                  3����A�ʺŵ�¼��Ȼ���޸�B�ʺŵ�������Ϣ��Ӧ��ʧ��
        '''
        u = user.user(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(u.allocate("A", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        self.assertTrue(u.allocate("B", "test", "test", user.ACCOUNTUSER, self.config_para.test_usergroup))
        A_seesion = user.login(self.environment.server, "A", "test")
        A_user = user.user(self.config_para.server_addr, A_seesion)
        self.assertFalse(A_user.set_description("B", "test description"))

        # �ָ�����
        self.assertTrue(u.delete("A"))
        self.assertTrue(u.delete("B"))
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


class UnitTestUserQuery(unittest.TestCase):
    '''
    ��ѯ�û�(tecs.user.query)
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
        self.normal_test_level_3_3()
        self.normal_test_level_3_4()

    def normal_test_level_3_4(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա��ѯ�����⻧��Ĭ����Ϣ
        ���Բ��裺1������Ա����һ���⻧A
                  2����ѯ�⻧��Ⱥ����Ϣ��Ӧ��Ϊ
                  3����ѯ�⻧��ʹ����Ϣ��Ӧ��Ϊʹ��
                  4����ѯ�⻧��ɫ��Ϣ��Ӧ��Ϊ
                  4����ѯ�Լ���phone,email,addr,description,��Ӧ��ΪNULL
        '''
        
        # �ָ�����

        
    def normal_test_level_3_3(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա��ѯ�Լ�����Ϣ
        ���Բ��裺1����ѯ�⻧��ɫ��Ϣ��Ӧ��Ϊ
                  2����ѯ�⻧��Ⱥ����Ϣ��Ӧ��Ϊ
                  3����ѯ�⻧��ʹ����Ϣ��Ӧ��Ϊʹ��
                  4�������Լ���phone Ϊ 13770591182
                  4����ѯ�Լ���phoneӦ��Ϊ13770591182
        '''
        
        # �ָ�����

        
    def normal_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա��ѯ����⻧��Ϣ
        ���Բ��裺1������Ա����һ���⻧A
                  2������Ա����һ���⻧B
                  3����ѯ�⻧��������Ӧ��Ϊ2
        '''
        
        # �ָ�����
        
        
    def normal_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ������Ա��ѯָ���⻧����Ϣ
        ���Բ��裺1������Ա����һ���⻧A
                  2������Ա����һ���⻧B
                  3�������⻧A��phone Ϊ 13606581182
                  3����ѯ�⻧A��ɫ��Ϣ��Ӧ��Ϊ
                  4����ѯ�⻧A��Ⱥ����Ϣ��Ӧ��Ϊ
                  5����ѯ�⻧A��ʹ����Ϣ��Ӧ��Ϊʹ��
                  6����ѯ�⻧A��phone,Ӧ��Ϊ 13606581182
        '''
        
        # �ָ�����
        
# �쳣���̲���
    def Exception_test_level_2(self):
        '''
        �쳣����
        '''

    def Exception_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Բ�ѯһ�������ڵ��⻧��Ϣ
        ���Բ��裺1������Ա��ѯһ�������ڵ��⻧A��Ϣ��Ӧ��ʧ��
        '''
        
        # �ָ�����
        
# �߽��������
    def limit_test_level_2(self):
        '''
        �߽����
        '''

    def limit_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�����Բ�ѯ�û���Ϊ�߽�����
        ���Բ��裺1������һ���⻧���ֳ���Ϊ64���û�
                  2����ѯ�⻧����Ϊ64�ĳ��ȣ�Ӧ�óɹ�
        '''
        
        # �ָ�����

# Ȩ�޲���
    def auth_test_level_2(self):
        '''
        Ȩ�޲���
        '''
        
    def auth_test_level_3_2(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧�������£���ѯ����һ���⻧����Ϣ
        ���Բ��裺1��
        '''
        
        # �ָ�����

        
    def auth_test_level_3_1(self):
        '''
        �Ƿ�ʵ�֣���
        ������ͼ�������⻧�������£���ѯ������Ϣ
        ���Բ��裺1��
        '''
        
        # �ָ�����
    
    def test_entery(self):
        # �������
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()
        
class UnitTestUserFullQuery(unittest.TestCase):
    '''
    ��ѯ�û�ȫ����Ϣ(tecs.user.full.query)
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
    UnitTestUserAdd(environment).test_entery()
    UnitTestUserDel(environment).test_entery()
    UnitTestUserSet(environment).test_entery()
    UnitTestUserQuery(environment).test_entery()
    UnitTestUserFullQuery(environment).test_entery()
    
    
# ģ���ĵ���������
def all_doc_entry(cidoc):
    # ������EXCEL�е�sheet��ǩ���ƶ���
    module_name = "�û�RPC"
    cidoc.write_doc(UnitTestUserAdd, module_name)
    cidoc.write_doc(UnitTestUserDel, module_name)
    cidoc.write_doc(UnitTestUserSet, module_name)
    cidoc.write_doc(UnitTestUserQuery, module_name)
    cidoc.write_doc(UnitTestUserFullQuery, module_name)


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