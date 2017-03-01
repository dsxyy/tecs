#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：base_test.py
# 测试内容：基本测试场景，包括冒烟测试和基本的RPC测试
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/03/24
#*******************************************************************************/
import sys,os

# 基本引用头模块
from base_function_module.interface.config_para import config_para
from base_function_module.interface.environment_prepare_package import *

# 加入功能测试的引用
from base_function_module.all_rpc_test import AllRpcTest
from base_function_module import smoking_function

# 基本测试场景，使用ALLRPC模块和冒烟模块来进行测试验证
class BaseTest():
    def __init__(self, config_para):
        # 记录当前测试场景的配置参数
        self.config_para = config_para
        self.public_snapshot = public_snapshot(config_para)


    # RPC接口测试    
    def AllRpcTestCase(self):
        # 启动前准备工作
        self.public_snapshot.recover()
        # 如果有配置需要调用如下的函数进行配置
        #self.public_snapshot.updata_tc_config(key, value)  
        #self.public_snapshot.updata_cc_config(key, value) 
        #self.public_snapshot.updata_hc_config(key, value)
        self.public_snapshot.start()
        self.public_snapshot.environment.prepare_public_environment()
        # 开始进行测试
        AllRpcTest().TestEntery(self.public_snapshot.environment)

    # 冒烟测试接口    
    def SmokingTestCase(self):
        # 启动前准备工作
        self.public_snapshot.recover()
        # 如果有配置需要调用如下的函数进行配置
        #self.public_snapshot.updata_tc_config(key, value)  
        #self.public_snapshot.updata_cc_config(key, value) 
        #self.public_snapshot.updata_hc_config(key, value)
        self.public_snapshot.start()
        # 准备公共环境变量
        self.public_snapshot.environment.prepare_public_environment()
        # 开始进行测试
        smoking_function.TestEntery(self.public_snapshot.environment)

    # 模式测试总入口，只负责掉功能测试函数    
    def BaseTestEntery(self):
        self.AllRpcTestCase()
        self.SmokingTestCase()
        
if __name__ == "__main__":
        file_name = sys.argv[0]
        # 获取配置参数的句柄
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # 开始进入测试模块
        BaseTest(config_para_handler).BaseTestEntery()
