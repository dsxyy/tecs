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
from interface.image_advance_package import image_module
from interface.vm_advance_package import vm_module


class FunctionTestUsbPassthrough(unittest.TestCase):
    '''
    当虚拟机崩溃的时候，保存Coredump文件测试
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
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_3()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_4()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_5()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_6()
        
        self.private_snapshot.recover()
        self.private_snapshot.private_run_env()
        self.normal_test_level_3_7()
        
    def normal_test_level_3_7(self):
        '''
        是否实现：否
        测试意图：测试在一个虚拟机界面下面，同一个USB设备插入两次
        测试步骤：1、创建1个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、检测是否成功(通过db判断操作进度是否为成功)，应该成功。
                  6、再次发送一个同一个USB插入命令。
                  7、检测是否成功(通过db判断操作进度是否为成功)，应该成功。
        '''
        pass;
   
    def normal_test_level_3_6(self):
        '''
        是否实现：否
        测试意图：测试在一个虚拟机界面下面，插入另一个虚拟机的USB设备
        测试步骤：1、创建2个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、然后使用RPC接口在另一个虚拟机下面，发送插入USB命令，应该失败。
        '''
        pass;

    def normal_test_level_3_5(self):
        '''
        是否实现：否
        测试意图：测试在一个虚拟机界面下面，拔出另一个虚拟机的USB设备
        测试步骤：1、创建2个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、然后使用RPC接口在另一个虚拟机下面，发送拔出命令，应该失败。
        '''
        pass;
        
    def normal_test_level_3_4(self):
        '''
        是否实现：否
        测试意图：测试在一个虚拟机界面下面，查看另一个虚拟机的USB信息
        测试步骤：1、创建2个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、然后使用RPC接口查询另一个虚拟机的USB信息，应查询出来正在使用，且使用者信息也存在
        '''
        pass;

    def normal_test_level_3_3(self):
        '''
        是否实现：否
        测试意图：测试向虚拟机更新U盘
        测试步骤：1、创建1个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、等待命令成功(通过db判断操作进度是否为成功)，
                  6、再发送更新USB的命令
                  7、等待命令执行成功(通过db判断操作进度是否为成功)
                  8、使用虚拟机的串口功能，访问虚拟机，应该USB设备还在
        '''
        pass;

    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：测试向虚拟机拔出usb的功能
        测试步骤：1、创建1个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、等待命令成功(通过db判断操作进度是否为成功)，
                  6、再发送拔出USB的命令
                  7、等待命令执行成功
                  8、使用虚拟机的串口功能，访问虚拟机，USB设备应该不在了
                  9、使用RPC接口查询看之前的USB设备是否已经处于空闲状态了
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：测试向虚拟机插入usb的功能
        测试步骤：1、创建一个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB。
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、等待命令成功(通过db判断操作进度是否为成功)，
                  6、使用虚拟机的串口功能，访问虚拟机，应该发现了一个USB设备(cat /proc/bus/usb/device)
                  7、使用RPC接口查询看之前的USB设备是否已经处于使用状态了
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
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：测试当操作USB插入的时候，CC进程异常重启的情况
        测试步骤：1、创建1个基本的虚拟机，并部署成功
                  2、轮询所有虚拟机，并通过RPC接口检测每个虚拟机对应主机是否有USB
                  3、如果没有USB，则退出该用例测试
                  4、如果检测到存在USB设备，则发送插入USB的命令
                  5、通过ssh命令，发送cc重启命令
                  6、检测操作命令是否成功(通过db判断操作进度是否为成功)，应该成功。
        '''
        pass;
        
    def test_entery(self):
        # 测试入口
        self.normal_test_level_2()
        self.Exception_test_level_2()

# 下面是模块测试执行总入口
def TestEntery(environment):
    FunctionTestUsbPassthrough(environment).test_entery()

# 模块文档输出总入口
def DocEntery(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "Usb透传测试"
    cidoc.write_doc(FunctionTestUsbPassthrough, module_name)



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
