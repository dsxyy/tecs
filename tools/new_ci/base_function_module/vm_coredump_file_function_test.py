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


class FunctionTestCoredumpFile(unittest.TestCase):
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
        测试意图：测试HC模块是否会修改coredump的文件名为core.ok
        测试步骤：1、创建一个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.52-instance_x.2.core 文件，大小为1M左右，应该成功
                  4、重启HC进程，主要模拟的虚拟机的domid从-1变化到存在的时候，触发修改文件名称的流程
                  4、等待1分钟后，检测看是否存在2013-0130-0335.52-instance_x.2.core.ok.tar.gz文件，应该存在
                  5、删除测试产生的tar文件，恢复下环境
        '''
        pass; 

        
    def normal_test_level_3_6(self):
        '''
        是否实现：否
        测试意图：虚拟机生成coredump文件，没有被HC置为有效的话，是否会压缩
        测试步骤：1、创建一个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.52-instance_1.2.core 文件，大小为1M左右，应该成功
                  4、等待1分钟后，检测看是否存在2013-0130-0335.52-instance_1.2.core.tar.gz文件，应该不存在
                  5、删除测试产生的tar文件，恢复下环境
        '''
        pass; 

        
    def normal_test_level_3_5(self):
        '''
        是否实现：否
        测试意图：多个虚拟机产生coredump文件，看是否能够正确的被压缩
        测试步骤：1、创建一个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.52-instance_1.2.core.ok 文件，大小为1M左右，应该成功
                  4、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.53-instance_12.9.core.ok 文件，大小为1M左右，应该成功
                  5、等待1分钟后，检测看是否存在2013-0130-0335.52-instance_1.2.core.ok.tar.gz文件，应该存在
                  6、同时检测，检测看是否存在2013-0130-0335.53-instance_12.9.core.ok.tar.gz文件，应该存在
                  7、删除测试产生的两个tar文件，恢复下环境
        '''
        pass; 
        
    def normal_test_level_3_4(self):
        '''
        是否实现：否
        测试意图：同一个虚拟机产生多个coredump文件，只针对最新的一个coredump文件产生压缩文件
        测试步骤：1、创建一个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.52-instance_1.2.core.ok 文件，大小为1M左右，应该成功
                  4、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.53-instance_1.9.core.ok 文件，大小为1M左右，应该成功
                  5、等待1分钟后，检测看是否存在2013-0130-0335.54-instance_1.9.core.ok.tar.gz文件，应该存在
                  6、同时检测，看是否存在多个tar文件，应该不存在
                  7、删除2013-0130-0335.52-instance_x.9.core.ok.tar.gz文件，恢复下环境
        '''
        pass;

        
    def normal_test_level_3_3(self):
        '''
        是否实现：否
        测试意图：查看coredump文件产生后，看是否能够被压缩
        测试步骤：1、创建一个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、通过ssh 向/var/lib/xen/dump/instance_x 下面写入一个2013-0130-0335.52-instance_x.3.core.ok 文件，大小为10M左右，应该成功
                  4、等待1分钟后，检测看是否存在2013-0130-0335.52-instance_x.3.core.ok.tar.gz文件，应该存在
                  5、同时检测，看2013-0130-0335.52-instance_x.3.core.ok这个文件是否被删除，应该删除
                  6、删除2013-0130-0335.52-instance_x.3.core.ok.tar.gz文件，恢复下环境
        '''
        pass;
        
    def normal_test_level_3_2(self):
        '''
        是否实现：否
        测试意图：测试当虚拟机增加coredump选项，是否能够产生对应的配置文件
        测试步骤：1、创建两个基本的虚拟机，并且在虚拟机配置上增加coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、检测/var/lib/tecs/instances/instances_x.xml 看下crash 选项下面是否出现 coredump-restart，应该出现
                  4、通过df检测，看下/var/lib/xen/dump/instance_x 是否已经被mount过了，应该出现过mount
                  5、通过df检测，看下/var/lib/xen/dump/instance_x+1 是否已经被mount过了，应该出现过mount
        '''
        pass;
        
    def normal_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：测试当虚拟机没有配置coredump选项，是否能够产生对应的配置文件
        测试步骤：1、创建一个基本的虚拟机，不配置coredump选项
                  2、部署虚拟机，并等待部署成功
                  3、检测/var/lib/tecs/instances/instances_x.xml 看下crash 选项下面是否出现 coredump-restart，不应该出现
                  4、通过df检测，看下/var/lib/xen/dump/instance_x 是否已经被mount过了,不应该出现过mount 
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
        测试意图：无（真实虚拟机里面触发coredump暂时不好实现，无法触发当虚拟机崩溃的时候的异常）
        测试步骤：1、无
        '''
        pass;
        
    def test_entery(self):
        # 测试入口
        self.normal_test_level_2()
        self.Exception_test_level_2()

# 下面是模块测试执行总入口
def TestEntery(environment):
    FunctionTestCoredumpFile(environment).test_entery()

# 模块文档输出总入口
def DocEntery(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "CoredumpFile测试"
    cidoc.write_doc(FunctionTestCoredumpFile, module_name)



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
