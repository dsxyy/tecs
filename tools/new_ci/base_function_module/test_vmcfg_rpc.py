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
from interface.base_interface.rpc import vmcfg
from interface.vm_advance_package import vm_module

class UnitTestVmcfgFullQuery(unittest.TestCase):
    '''
    虚拟机查询(tecs.vmcfg.full.query)
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


class UnitTestVmcfgGetIdByName(unittest.TestCase):
    '''
    根据虚拟机名称查询id(tecs.vmcfg.get_id_by_name)
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


class UnitTestVmcfgQuery(unittest.TestCase):
    '''
    虚拟机查询(tecs.vmcfg.query)
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


class UnitTestVmcfgQueryRelation(unittest.TestCase):
    '''
    查询虚拟机互斥关系(tecs.vmcfg.query_relation)
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


class UnitTestVmcfgQueryVmSynState(unittest.TestCase):
    '''
    虚拟机查询(tecs.vmcfg.query_vm_syn_state)
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


class UnitTestVmcfgRelationSet(unittest.TestCase):
    '''
    配置虚拟机互斥关系。(tecs.vmcfg.relation.set)
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


class UnitTestVmcfgAction(unittest.TestCase):
    '''
    虚拟机操作(tecs.vmcfg.action)
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


class UnitTestVmcfgAllocate(unittest.TestCase):
    '''
    创建虚拟机(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # 调用创建接口
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

class UnitTestVmcfgModify(unittest.TestCase):
    '''
    创建虚拟机(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # 调用创建接口
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

class UnitTestVmcfgAllocateByVt(unittest.TestCase):
    '''
    创建虚拟机(tecs.vmcfg.allocate)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        # 调用创建接口
        
        
        
        allocate_info = {}
        allocate_info["vm_num"] = 3
        allocate_info["vt_name"] = "vt1"
        allocate_info["project_name"] = "p1" 

        self.assertTrue(self.vm_ctrl.allocatebyvt(allocate_info), "create vm by vt fail")
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

class UnitTestVmImageBackupRestore(unittest.TestCase):
    '''
    恢复虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建移动盘基本测试  
        测试步骤：
        '''
        # 修改虚拟机镜像备份信息
        self.assertTrue(self.vm_ctrl.restore_image_backup(1L, "73257119-92df-4204-998b-8cf165c32c1b", "hda"), "restore vm image backup fail")
        
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestVmcfgVmClone(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建移动盘基本测试  
        测试步骤：
        '''
        # 修改虚拟机镜像备份信息
        vid = 1L
        new_id = self.vm_ctrl.vm_clone(vid)
        if new_id >0:
            print "clone vm new_id=%s" % new_id
        else:
            print "fail clone vm " 
            self.assertTrue(False, "clone vm fail")
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()


class UnitTestVmStaticInfoQuery(unittest.TestCase):
    '''
    虚拟机查询(tecs.vmcfg.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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

class UnitTestVmRunningInfoQuery(unittest.TestCase):
    '''
    虚拟机查询(tecs.vmcfg.query)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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

class UnitTestVmcfgImageBackupCreate(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建移动盘基本测试  
        测试步骤：
        '''
        # 修改虚拟机镜像备份信息
        self.assertTrue(self.vm_ctrl.create_image_backup(1L, "hda"), "create vm image backup fail")
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestVmcfgImageBackupDelete(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.delete)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建移动盘基本测试  
        测试步骤：
        '''
        # 修改虚拟机镜像备份信息
        self.assertTrue(self.vm_ctrl.delete_image_backup(1L, "ead32593-0e75-458c-a8eb-395d1e32018a"), "delete vm image backup fail")
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()      
        
class UnitTestVmcfgVmClone(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建移动盘基本测试  
        测试步骤：
        '''
        # 修改虚拟机镜像备份信息
        new_id = self.vm_ctrl.vm_clone(1L)
        if new_id >0:
            print "clone vm new_id=%s" % new_id
        else:
            print "fail clone vm " 
            self.assertTrue(False, "clone vm fail")
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestVmAddAffinityRegion(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
        
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestVmDelAffinityRegion(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
     
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestVmMigrate(unittest.TestCase):
    '''
    创建虚拟机镜像(tecs.vmcfg.image_backup.restore)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        
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
                  2、
                  3、
        '''
     
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：
        测试步骤：
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
        测试意图：创建虚拟机machine基本信息异常参数测试
        测试步骤：1、设置磁盘类型为非“machine”，创建虚拟机，返回失败
                  2、设置machine映像存储位置，既非“本地存储”，也非“共享存储”，创建虚拟机，返回失败
                  3、设置磁盘总线类型，既非“ide”，也非“scsi”，创建虚拟机，返回失败
        '''
        
        pass;

    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：创建虚拟机基本信息异常参数测试
        测试步骤：1、设置虚拟化类型，既非“全虚拟化”，也非“半虚拟化”，创建虚拟机，返回失败
                  2、设置虚拟机部署到哪个集群，集群名称长度超过64字节，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，主机名称长度超过64字节，创建虚拟机，返回失败
                  4、设置虚拟机看门狗时长为小于600-7200，创建虚拟机，返回失败
                  5、设置VNC访问密码，填成全为空格的字符串，创建虚拟机，返回失败
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
        测试意图：虚拟机归属的工程名称边界参数测试
        测试步骤：1、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为1，创建虚拟机，应该成功
                  2、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为32，创建虚拟机，应该成功
                  3、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度大于32，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
                  4、设置虚拟机归属的工程名称（该用户名下已有该名称的工程），长度为0，创建虚拟机，返回失败(测试不了，在创建工程时就会失败)
        '''
        
        pass;
        
    def limit_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：虚拟机名称边界参数测试
        测试步骤：1、设置虚拟机名称，长度为0，创建虚拟机，应该成功
                  2、设置虚拟机名称，长度为32，创建虚拟机，应该成功
                  3、设置虚拟机名称，长度大于32，创建虚拟机，返回失败
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
        测试意图：普通用户创建虚拟机是否成功
        测试步骤：1、普通用户登录
                  2、设置虚拟机部署到哪个集群，创建虚拟机，返回失败
                  3、设置虚拟机部署到哪个主机，创建虚拟机，返回失败
                  4、设置网络平面名称，创建虚拟机，返回失败
                  5、配置其他参数（参数取值范围合法），创建虚拟机，应该成功
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

# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "虚拟机配置 RPC"
    cidoc.write_doc(UnitTestVmcfgFullQuery, module_name)
    cidoc.write_doc(UnitTestVmcfgGetIdByName, module_name)
    cidoc.write_doc(UnitTestVmcfgQuery, module_name)
    cidoc.write_doc(UnitTestVmcfgQueryRelation, module_name)
    cidoc.write_doc(UnitTestVmcfgQueryVmSynState, module_name)
    cidoc.write_doc(UnitTestVmcfgRelationSet, module_name)
    cidoc.write_doc(UnitTestVmcfgAction, module_name)
    cidoc.write_doc(UnitTestVmcfgAllocate, module_name)



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