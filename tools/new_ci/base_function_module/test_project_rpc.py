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
from interface.base_interface.rpc import project
from interface.vm_advance_package import vm_module

class UnitTestProjectAllocate(unittest.TestCase):
    '''
    创建虚拟机工程(tecs.project.allocate)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectDel(unittest.TestCase):
    '''
    删除虚拟机工程(tecs.project.delete)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectQuery(unittest.TestCase):
    '''
    虚拟机工程查询(tecs.project.query)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectQuery_vm_by_project(unittest.TestCase):
    '''
    查询该工程下拥有的虚拟机(tecs.project.query_vm_by_project)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectSet(unittest.TestCase):
    '''
    修改虚拟机工程(tecs.project.set)
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
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestProjectSaveAsImage(unittest.TestCase):
    '''
    保存为镜像(tecs.project.save_as_image)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.project_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        self.assertTrue(self.project_ctrl.project_save_as_image("centos", False, True, "test"), "project save as image fail")
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        #self.Exception_test_level_2()
        #self.limit_test_level_2()
        #self.auth_test_level_2()

class UnitTestProjectCreateByImage(unittest.TestCase):
    '''
    从镜像创建(tecs.project.create_by_image)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.project_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        self.normal_test_level_3_2()
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        self.assertTrue(self.project_ctrl.project_create_by_image(37L, "create_by_image_4"), "project create by image fail")
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
# 边界参数测试
    def limit_test_level_2(self):
        '''
        边界测试
        '''
        self.limit_test_level_3_1()
        self.limit_test_level_3_2()

    def limit_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;

# 权限测试
    def auth_test_level_2(self):
        '''
        权限测试
        '''
        self.auth_test_level_3_1()
        self.auth_test_level_3_2()
        
    def auth_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
        
    def auth_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：1、
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        #self.Exception_test_level_2()
        #self.limit_test_level_2()
        #self.auth_test_level_2()

# 下面是模块测试执行总入口
def all_test_entry(environment):
    UnitTestProjectAllocate(environment).test_entery()
    UnitTestProjectDel(environment).test_entery()
    UnitTestProjectQuery(environment).test_entery()
    UnitTestProjectQuery_vm_by_project(environment).test_entery()
    UnitTestProjectSet(environment).test_entery()
    #UnitTestProjectSaveAsImage(environment).test_entery()
    #UnitTestProjectCreateByImage(environment).test_entery()
    


# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "工程管理RPC"
    cidoc.write_doc(UnitTestProjectAllocate, module_name)
    cidoc.write_doc(UnitTestProjectDel, module_name)
    cidoc.write_doc(UnitTestProjectQuery, module_name)
    cidoc.write_doc(UnitTestProjectQuery_vm_by_project, module_name)
    cidoc.write_doc(UnitTestProjectSet, module_name)



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