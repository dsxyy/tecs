#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�base_test.py
# �������ݣ��������Գ���������ð�̲��Ժͻ�����RPC����
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/03/24
#*******************************************************************************/
import sys,os

# ��������ͷģ��
from base_function_module.interface.config_para import config_para
from base_function_module.interface.environment_prepare_package import *

# ���빦�ܲ��Ե�����
from base_function_module.all_rpc_test import AllRpcTest
from base_function_module import smoking_function

# �������Գ�����ʹ��ALLRPCģ���ð��ģ�������в�����֤
class BaseTest():
    def __init__(self, config_para):
        # ��¼��ǰ���Գ��������ò���
        self.config_para = config_para
        self.public_snapshot = public_snapshot(config_para)


    # RPC�ӿڲ���    
    def AllRpcTestCase(self):
        # ����ǰ׼������
        self.public_snapshot.recover()
        # �����������Ҫ�������µĺ�����������
        #self.public_snapshot.updata_tc_config(key, value)  
        #self.public_snapshot.updata_cc_config(key, value) 
        #self.public_snapshot.updata_hc_config(key, value)
        self.public_snapshot.start()
        self.public_snapshot.environment.prepare_public_environment()
        # ��ʼ���в���
        AllRpcTest().TestEntery(self.public_snapshot.environment)

    # ð�̲��Խӿ�    
    def SmokingTestCase(self):
        # ����ǰ׼������
        self.public_snapshot.recover()
        # �����������Ҫ�������µĺ�����������
        #self.public_snapshot.updata_tc_config(key, value)  
        #self.public_snapshot.updata_cc_config(key, value) 
        #self.public_snapshot.updata_hc_config(key, value)
        self.public_snapshot.start()
        # ׼��������������
        self.public_snapshot.environment.prepare_public_environment()
        # ��ʼ���в���
        smoking_function.TestEntery(self.public_snapshot.environment)

    # ģʽ��������ڣ�ֻ��������ܲ��Ժ���    
    def BaseTestEntery(self):
        self.AllRpcTestCase()
        self.SmokingTestCase()
        
if __name__ == "__main__":
        file_name = sys.argv[0]
        # ��ȡ���ò����ľ��
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # ��ʼ�������ģ��
        BaseTest(config_para_handler).BaseTestEntery()
