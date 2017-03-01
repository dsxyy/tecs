#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 测试内容：
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/12/26
#*******************************************************************************/
import sys, os
import unittest 


# 公共组件封装
from interface.base_interface.rpc.contrib import tecs_common
from interface.environment_prepare_package import prepare_environment
from interface.config_para import config_para

# 测试接口封装
from interface.base_interface.rpc import file_manager

class UnitTestQuery_coredump_url(unittest.TestCase):
    '''
    查询coredump文件的URL(tecs.file.query_coredump_url)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：调用查询接口，看是否能够查询接口成功
        测试步骤：1、调用tecs.file.query_coredump_url
                  2、应该返回成功
        '''
        f = file_manager.file_manager(self.config_para.server_addr, self.environment.get_root_session())
        self.assertTrue(f.query())
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：无（查询接口，无参数，无异常测试）
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：无（查询接口，无入参，无边界测试）
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：使用租户权限，看查询接口是否调用成功
        测试步骤：1、使用租户登录
                  2、调用查询接口，应该失败
        '''
        f = file_manager.file_manager(self.config_para.server_addr, self.environment.get_user_session())
        self.assertFalse(f.query())

    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


# 下面是模块测试执行总入口
def all_test_entry(environment):
    UnitTestQuery_coredump_url(environment).test_entery()


# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "file管理RPC"
    cidoc.write_doc(UnitTestQuery_coredump_url, module_name)

# 单个函数执行体的位置
if __name__ == "__main__":
        file_name = sys.argv[0]
        # 获取配置参数的句柄
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # 获取环境准备公共句柄，并初始化
        environment = prepare_environment(config_para_handler)
        environment.wait_sys_ok()
        environment.prepare_public_environment()
        all_test_entry(environment)