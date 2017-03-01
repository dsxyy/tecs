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
#from interface.base_interface.rpc import vnet_dhcp

class UnitTestVnetDhcpPxeSet(unittest.TestCase):
    '''
    配置DHCP PXE参数(tecs.dhcp_pxe.set)
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



class UnitTestVnetDhcpQuery(unittest.TestCase):
    '''
    查询DHCP配置信息(tecs.dhcp.query)
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



class UnitTestVnetDhcpRangeSet(unittest.TestCase):
    '''
    配置DHCP地址范围参数(tecs.dhcp_range.set)
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



class UnitTestVnetDhcpStart(unittest.TestCase):
    '''
    启动DHCP服务(tecs.dhcp.start)
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



class UnitTestVnetDhcpStateQuery(unittest.TestCase):
    '''
    查询DHCP服务状态(tecs.dhcp_state.query)
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



class UnitTestVnetDhcpStop(unittest.TestCase):
    '''
    停止DHCP服务(tecs.dhcp.stop)
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



class UnitTestVnetDhcpSubnetDel(unittest.TestCase):
    '''
    删除DHCP子网参数(tecs.dhcp_subnet.delete)
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



class UnitTestVnetDhcpSubnetSet(unittest.TestCase):
    '''
    配置DHCP子网参数(tecs.dhcp_subnet.set)
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



class UnitTestVnetDhcpGlobalSet(unittest.TestCase):
    '''
    配置DHCP全局参数(tecs.dhcp.global.set)
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



class UnitTestVnetDhcpIpQuery(unittest.TestCase):
    '''
    查询IP地址相关信息(tecs.dhcp.ip.query)
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



# 下面是模块测试执行总入口
def all_test_entry(environment):
    UnitTestVnetDhcpPxeSet(environment).test_entery()
    UnitTestVnetDhcpQuery(environment).test_entery()
    UnitTestVnetDhcpRangeSet(environment).test_entery()
    UnitTestVnetDhcpStart(environment).test_entery()
    UnitTestVnetDhcpStateQuery(environment).test_entery()
    UnitTestVnetDhcpStop(environment).test_entery()
    UnitTestVnetDhcpSubnetDel(environment).test_entery()
    UnitTestVnetDhcpSubnetSet(environment).test_entery()
    UnitTestVnetDhcpGlobalSet(environment).test_entery()
    UnitTestVnetDhcpIpQuery(environment).test_entery()


# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "Vnet DHCP RPC"
    cidoc.write_doc(UnitTestVnetDhcpPxeSet, module_name)
    cidoc.write_doc(UnitTestVnetDhcpQuery, module_name)
    cidoc.write_doc(UnitTestVnetDhcpRangeSet, module_name)
    cidoc.write_doc(UnitTestVnetDhcpStart, module_name)
    cidoc.write_doc(UnitTestVnetDhcpStateQuery, module_name)
    cidoc.write_doc(UnitTestVnetDhcpStop, module_name)
    cidoc.write_doc(UnitTestVnetDhcpSubnetDel, module_name)
    cidoc.write_doc(UnitTestVnetDhcpSubnetSet, module_name)
    cidoc.write_doc(UnitTestVnetDhcpGlobalSet, module_name)
    cidoc.write_doc(UnitTestVnetDhcpIpQuery, module_name)



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