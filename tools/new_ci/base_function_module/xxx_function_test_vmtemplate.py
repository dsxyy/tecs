#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 测试内容：功能级别的测试模板
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/12/28
#*******************************************************************************/
import sys, os
import unittest 

# 公共组件引用
from interface.environment_prepare_package import * 
from interface.config_para import config_para
from interface.base_interface.rpc.contrib import tecs_common
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# 测试使用到的高级封装引用，根据需要来进行引用
#from interface.image_advance_package import image_module
#from interface.vm_advance_package import vm_module


class FunctionTestXXX(unittest.TestCase):
    '''
    xxx测试
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        # 执行当前测试类的环境
        self.class_test_environment_prepare()
        # 建立私有快照,之后恢复都会以当前点的环境进行恢复
        self.private_snapshot = private_snapshot(self.environment, self.function_test_environment_prepare)
        
    def class_test_environment_prepare(self):
        # 上传映像上去，首次调用是上传
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)

        
    def function_test_environment_prepare(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！
        '''
        # 获取到image模块的公共句柄
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)
        self.image_ctrl = image_ctrl
        # HC进程运行控制句柄
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # 虚拟机的SSH检测句柄
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # 虚拟机的db检测句柄
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        # 配置虚拟机的基本配置
        self.vm_ctrl.add_base("vm1", 1, 1, 256, image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
        self.vm_ctrl.add_disk(256, "scsi", "sda", 1, "ext4")
        self.vm_ctrl.add_context("test", "aaaaa")

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_1()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_2()

        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：xxxxxx
        测试步骤：1、xxxxxx
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：xxxxxx
        测试步骤：1、xxxxxx
        '''
        pass;

# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.Exception_test_level_3_1()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.Exception_test_level_3_2()

    def Exception_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：xxxxxx
        测试步骤：1、xxxxxx
        '''
        pass;
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：xxxxxx
        测试步骤：1、xxxxxx
        '''
        pass;
        
    def test_entery(self):
        # 测试入口
        self.normal_test_level_2()
        self.Exception_test_level_2()

# 下面是模块测试执行总入口
def TestEntery(environment):
    FunctionTestXXX(environment).test_entery()

# 模块文档输出总入口
def DocEntery(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "xxx测试模板"
    cidoc.write_doc(FunctionTestXXX, module_name)



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
        TestEntery(environment)
