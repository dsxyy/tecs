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

# ���빦�ܲ��Ե������ļ�
#from base_function_module import Ҫ���Ե��ļ���


# ȫ�°�װ���Գ�������Ҫ�Ĳ������������Լ��뵽������в���
class InstallTest():
    def __init__(self, config_para):
        # ��¼��ǰ���Գ��������ò���
        self.config_para = config_para
        self.public_snapshot = public_snapshot(config_para)


    # xxx����    
    def xxxTestCase(self):
        # ����ǰ׼������
        self.public_snapshot.recover()
        # �����������Ҫ�������µĺ�����������
        #self.public_snapshot.updata_tc_config(key, value)  
        #self.public_snapshot.updata_cc_config(key, value) 
        #self.public_snapshot.updata_hc_config(key, value)
        self.public_snapshot.start()
        self.public_snapshot.environment.prepare_public_environment()
        # ��ʼ���в���
#        Ҫ���Ե��ļ���.TestEntery(self.public_snapshot.environment)


    # ģʽ��������ڣ�ֻ��������ܲ��Ժ���    
    def TestEntery(self):
#        self.xxxTestCase()
        pass;

        
if __name__ == "__main__":
        file_name = sys.argv[0]
        # ��ȡ���ò����ľ��
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # ��ʼ�������ģ��
        InstallTest(config_para_handler).TestEntery()
