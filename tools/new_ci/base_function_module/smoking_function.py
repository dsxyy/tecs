#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 测试内容：冒烟功能测试
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/12/28
#*******************************************************************************/
import sys, os,time
import unittest 

# 公共组件引用
from interface.environment_prepare_package import * 
from interface.config_para import config_para
from interface.base_interface.rpc.contrib import tecs_common
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# 测试使用到的高级封装引用
from interface.image_advance_package import image_module
from interface.vm_advance_package import vm_module


class FunctionTestSmoking(unittest.TestCase):
    '''
    TECS冒烟测试
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
        self.vm_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
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

    def normal_test_level_3_6(self):
        '''
        是否实现：否
        测试意图：测试移动盘功能(对于ttylinux镜像的虚拟机不支持热插拔移动盘,所以只能采用关机->attach->开机的方式测试)
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署虚拟机
                  3、创建移动盘
                  4、关机虚拟机
                  5、attach移动盘
                  6、开机虚拟机
                  7、关机虚拟机
                  8、detach移动盘
                  9、删除移动盘
        '''
        ## 创建虚拟机
        #vid = self.vm_ctrl.create()
        #self.assertNotEqual(vid, -1, "create vm fail")
        ## 部署虚拟机
        #self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        ## 检测虚拟机部署成功后，虚拟机状态是否正确
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        ## 创建移动盘
        #self.db_vm_check_ctrl.check_pd_storage_cluster()
        #request_id = self.vm_ctrl.create_portable_disk(134217728L, "tecscluster", "tecs")
        #self.assertNotEqual(request_id, False, "create portable disk fail!")
        ## 检测新增移动盘相关表是否有对应记录
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 0), "wait create portable disk fail")

        ## 关机虚拟机
        #self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        ## 检测虚拟机状态为关机态
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")

        ## attach移动盘
        #vid=self.db_vm_check_ctrl.get_all_deploy_vid()
        #request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #self.assertNotEqual(request_id, False, "get request_id false!")
        #self.assertTrue(self.vm_ctrl.attach_portable_disk(request_id, long(vid[0]), "hdb", "ide"), "attach portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid[0]), 30, 2), "wait attach portable disk fail")

        ## 开机
        #self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        ## 检测虚拟机为运行态
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
        ## 关机虚拟机
        #self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        ## 检测虚拟机状态为关机态
        #self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")

        ## detach移动盘
        #request_id, vid = self.db_vm_check_ctrl.pd_get_attach_request_id_and_vid()
        #self.assertNotEqual(request_id, False, "get request_id and vid false!")
        #self.assertTrue(self.vm_ctrl.detach_portable_disk(long(vid), request_id), "detach portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid), 30, 3), "wait detach portable disk fail")

        ## 删除移动盘
        #request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #self.assertNotEqual(request_id, False, "get request_id false!")
        #self.assertTrue(self.vm_ctrl.delete_portable_disk(request_id), "delete portable disk fail!")
        #self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 1), "wait delete portable disk fail")

    def normal_test_level_3_5(self):
        '''
        是否实现：是
        测试意图：模拟单板重启，看下虚拟机状态是否能够恢复正常
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署虚拟机
                  3、获取domid,方便最后一步检测ID是否变化
                  4、模拟单板重启
                  5、检测虚拟机状态DomID，应该发送变化(60s等待检测)
        '''
        # 创建虚拟机
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 打印表空间占用最多的10张表
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # 检测虚拟机部署成功后，查询虚拟机的domid,给最后一步使用
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        # 模拟HC单板重启
        self.ssh_tecs_run_ctl.SimHcBoardReboot()
        # 检测虚拟机domid
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))

       
    def normal_test_level_3_4(self):
        '''
        是否实现：是
        测试意图：模拟HC重启，看下虚拟机状态是否变化
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署虚拟机
                  3、获取domid,方便最后一步检测ID是否变化
                  4、重启HC进程
                  5、检测虚拟机状态DomID，不应该变化(最长10S时长检测)
        '''
        # 创建虚拟机
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 打印表空间占用最多的10张表
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # 检测虚拟机部署成功后，查询虚拟机的domid,给最后一步使用
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        # 重启HC进程
        self.ssh_tecs_run_ctl.restart_hc_server()
        # 检测虚拟机domid
        self.assertFalse(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 10))

    def normal_test_level_3_3(self):
        '''
        是否实现：是
        测试意图：虚拟机操作动作测试，包括、关机，开机，强制关机
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署虚拟机
                  3、检测是否部署成功
                  4、关闭虚拟机
                  5、检测虚拟机是否存在，应该不存在
                  6、打开虚拟机
                  7、检测虚拟机是否存在，应该存在
                  8、强制关闭虚拟机
                  9、检测虚拟机是否存在，应该不存在
        '''
        # 创建虚拟机
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 打印表空间占用最多的10张表
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # 检测虚拟机部署成功后，虚拟机状态是否正确
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        # 挂起虚拟机
        self.assertTrue(self.vm_ctrl.stop(), "stop Vm Fail")
        # 检测虚拟机状态为关机态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait vm_state_shutoff fail")
        # 开机
        self.assertTrue(self.vm_ctrl.start(), "start Vm Fail")
        # 检测虚拟机为运行态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
        # 强制关机
        self.assertTrue(self.vm_ctrl.destory(), "destory Vm Fail")
        # 检测虚拟机为关机态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_shutoff, 60), "wait destory fail")
        
    def normal_test_level_3_2(self):
        '''
        是否实现：是
        测试意图：虚拟机操作动作测试，包括、挂起，解除挂起
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署虚拟机
                  3、检测是否部署成功
                  4、挂起虚拟机
                  5、检测虚拟机状态，应该挂起
                  6、解除挂起，
                  7、检测虚拟机状态，应该运行
        '''
        # 创建虚拟机
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 打印表空间占用最多的10张表
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # 检测虚拟机部署成功后，虚拟机状态是否正确
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "Wait Vm Deploy Fail")
        # 挂起虚拟机
        self.assertTrue(self.vm_ctrl.pause(), "pause Vm Fail")
        # 检测虚拟机状态为挂起态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_pause, 60), "wait pause fail")
        # 解除挂起
        self.assertTrue(self.vm_ctrl.resume(), "resume Vm Fail")
        # 检测虚拟机为运行态
        self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait resume fail")
        
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：虚拟机操作动作的测试，包括,创建，部署，重启，强制重启，撤销部署，再次部署
        测试步骤：1、使用环境准备中的虚拟机配置，进行创建
                  2、部署该物理机
                  3、获取刚部署的虚拟机DomID
                  4、重启虚拟机(需等待60S，重启才会成功)
                  5、检测重启后的虚拟机DomID是否发送变化，应该变化
                  6、强制重启虚拟机
                  7、检测虚拟机ID是否发送变化，应该变化
        '''
        # 创建虚拟机
        vid = self.vm_ctrl.create()
        self.assertNotEqual(vid, -1, "create vm fail")
        
        # 延时1分钟，等待CC注册到TC
        time_delay(60, "wait cc register to tc") 
        
        # 部署虚拟机
        self.assertTrue(self.vm_ctrl.deploy(), "Deploy Vm Fail")
        # 打印表空间占用最多的10张表
        self.db_vm_check_ctrl.pd_get_table_size_info()
        
        # 获取虚拟机的domid
        old_dom_id = self.ssh_vm_check_ctrl.get_domid_timeout(self.vm_ctrl.get_vm_instance_name(), 60)
        self.assertNotEqual(old_dom_id, -1, "Wait Vm Deploy Fail")
        time_delay(62, "reboot test need delay") 
        # reboot虚拟机
        self.assertTrue(self.vm_ctrl.reboot(), "Reboot Vm Fail")
        # 检测domid变化
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 120))
        # reset 虚拟机
        time_delay(62, "reset test need delay") 
        self.assertTrue(self.vm_ctrl.reset(), "Reset Vm Fail")
        # 检测domid发生变化
        self.assertTrue(self.ssh_vm_check_ctrl.check_domid_change_timeout(self.vm_ctrl.get_vm_instance_name(), old_dom_id, 60))
        
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
        测试意图：无
        测试步骤：1、无
        '''
        pass;

    def test_entery(self):
        # 测试入口
        self.normal_test_level_2()
        self.Exception_test_level_2()

# 下面是模块测试执行总入口
def TestEntery(environment):
    FunctionTestSmoking(environment).test_entery()

# 模块文档输出总入口
def DocEntery(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "TECS冒烟测试"
    cidoc.write_doc(FunctionTestSmoking, module_name)



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
        # 执行测试
        TestEntery(environment)
