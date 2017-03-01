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


class FunctionTestVmAction(unittest.TestCase):
    '''
    虚拟机操作测试
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

    def normal_test_level_3_3(self):
        '''
        是否实现：否
        测试意图：测试虚拟机热迁移
        测试步骤：1、创建虚拟机，映像和硬盘的磁盘位置都选“本地”，然后部署
                  2、如部署失败，则正常退出测试
                  3、如部署成功，待虚拟机状态为“running”时，发起热迁移，应当失败
                  4、撤销部署，重新修改映像和硬盘的磁盘位置都为“共享”，重新部署
                  5、如部署失败，则正常退出测试
                  6、待虚拟机状态为“running”时，发起热迁移，应成功，延时30秒后数据库vm_pool的hid字段应变化了
                  7、将虚拟机关机，发起热迁移，应失败，数据库vm_pool的hid字段无变化
        '''
        pass;
        
    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：在升级环境下，测试测试部署，重新部署
        测试步骤：1、获取升级测试虚拟机的ID，如果获取失败，则正常退出测试
                  2、撤销部署，等待撤销完成，应该成功
                  3、再次部署该虚拟机，看部署是否成功，应该当成功
        '''
        upgrade_vm_id = self.environment.get_upgrade_test_vm()
        if upgrade_vm_id == -1:
            return
        self.vm_ctrl.set_vmid(upgrade_vm_id)
        # 获取虚拟机所在集群
        cluster_name = self.db_vm_check_ctrl.get_cluster_name_by_vid(upgrade_vm_id)
        # 撤销虚拟机
        self.assertTrue(self.vm_ctrl.cancel(), "cancel Vm Fail")
        # 等待虚拟机的取消记录写入vm_cancel_pool表
        time_delay(1, "wait vm_cancel item is write!")
        # 检测测试是否成功
        self.assertTrue(self.db_vm_check_ctrl.check_vm_cancel_success_with_time_out(cluster_name, upgrade_vm_id, 60))
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 检测虚拟机部署是否成功
        self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：在升级环境下，测试重启，关机，开机
        测试步骤：1、获取升级测试虚拟机的ID，如果获取失败，则正常退出测试
                  2、重启该虚拟机，并检测是否重启成功，应该成功
                  3、关闭该虚拟机，并检测虚拟机是否关闭成功，应该成功
                  4、打开虚拟机，虚拟机应该打开成功
        '''
        upgrade_vm_id = self.environment.get_upgrade_test_vm()
        if upgrade_vm_id == -1:
            return
        self.vm_ctrl.set_vmid(upgrade_vm_id)
        # 获取重启前的虚拟机domid
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        # 重启虚拟机
        self.assertTrue(self.vm_ctrl.reboot(), "reboot Vm Fail")
        # 再检测操作是否完成
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_run, 60), "wait reboot fail")
        # 检测domid变化
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))
        # 强制关机
        self.assertTrue(self.vm_ctrl.destory(), "destory Vm Fail")
        # 检测虚拟机为关机态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_shutoff, 60), "wait destory fail")
        # 开机
        self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        # 检测虚拟机为运行态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(upgrade_vm_id, self.config_para.vm_state_run, 60), "wait start fail")


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
    FunctionTestVmAction(environment).test_entery()

# 模块文档输出总入口
def DocEntery(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "虚拟机操作测试"
    cidoc.write_doc(FunctionTestVmAction, module_name)



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
